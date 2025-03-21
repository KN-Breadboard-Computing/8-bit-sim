#pragma once
#include "clockable_module.hpp"
#include <Vmonitor_tester.h>
#include <Vgpu.h>
#include <cstdint>
#include <imgui.h>
#include <raylib.h>
#include <fmt/format.h>
#include <expected.hpp>
#include <functional>

// VGA timing (based on http://www.tinyvga.com/vga-timing/640x480@60Hz)

constexpr static uint32_t h_visible_area = 640u;
constexpr static uint32_t v_visible_area = 480u;
constexpr static uint32_t h_front_porch = 16u;
constexpr static uint32_t v_front_porch = 10u;
constexpr static uint32_t h_sync_pulse_width = 96u;
constexpr static uint32_t v_sync_pulse_width = 2u;
constexpr static uint32_t h_back_porch = 48u;
constexpr static uint32_t v_back_porch = 33u;
constexpr static uint32_t h_total = h_front_porch + h_visible_area + h_sync_pulse_width + h_back_porch;
constexpr static uint32_t v_total = v_front_porch + v_visible_area + v_sync_pulse_width + v_back_porch;

// Simulator error types
struct HSyncUndetected { auto message() const { return "Hsync undetected"; } };
struct VSyncUndetected { auto message() const { return "Vsync undetected"; } };
struct MultiplePulsesDetected {
    bool is_hsync;
    auto message() const { return fmt::format("Multiple {} pulses detected", is_hsync ? "hsync" : "vsync"); }
};
struct IncorrectPulseWidth {
    bool is_hsync;
    uint32_t expected;
    uint32_t actual;
    auto message() const { return fmt::format("Incorrect {} sync pulse width, expected: {}, found {}",  is_hsync ? "hsync" : "vsync", expected, actual); }
};
struct IncorrectSyncTiming {
    bool is_hsync;
    uint32_t expected;
    uint32_t actual;
    auto message() const { return fmt::format("{} expected on pulse {}, found on: {}", is_hsync ? "hsync" : "vsync", expected, actual); }
};
struct IncorrectRowTiming {
    uint32_t row;
    auto message() const { return fmt::format("Incorrect row timing on row {}", row); }
};

using VGASimulatorError = std::variant<
    HSyncUndetected,
    VSyncUndetected,
    MultiplePulsesDetected,
    IncorrectPulseWidth,
    IncorrectSyncTiming,
    IncorrectRowTiming
>;

template <typename T>
concept VerilatedVGADriver = ClockableModule<T> && requires(T module) {
    module.red;
    module.green;
    module.blue;
    module.hsync;
    module.vsync;
};

template <VerilatedVGADriver T>
struct VGASimulator {
    T* vga_driver;
    ClockScheduler* scheduler;

    using DrawFunction = std::function<void(const uint32_t, const uint32_t, const Color)>;

    VGASimulator(T* vga_driver, ClockScheduler* scheduler)
        : vga_driver(vga_driver), scheduler(scheduler), current_row(0) {}

    // it assumes that the monitor and the simulator are synced up - the module is assumed to be in display time
    // run `sync()` before this
    auto process_vga_frame(DrawFunction draw_function) -> rd::expected<void, VGASimulatorError> {
        current_row = 0;

        VSyncInfo vsync_info{};

        bool is_in_visible_area{};

        while (current_row <= v_total) {
            is_in_visible_area = current_row < v_visible_area;
            if (const auto correct_row = process_vga_row(draw_function, is_in_visible_area) ; !correct_row) {
                return rd::unexpected{correct_row.error()};
            }

            detect_sync_pulse_change(vsync_info, vga_driver->vsync, current_row);
            current_row++;
        }

        if (!vsync_info.sync_detected) {
            return rd::unexpected(VSyncUndetected{});
        }

        if (vsync_info.multiple_pulses_detected) {
            return rd::unexpected(MultiplePulsesDetected{.is_hsync = false});
        }

        if (vsync_info.get_pulse_length() != v_sync_pulse_width) {
            return rd::unexpected(IncorrectPulseWidth{
                .is_hsync = false,
                .expected = vsync_info.get_pulse_length(),
                .actual = v_sync_pulse_width,
            });
        }

        if (vsync_info.sync_start != v_visible_area + v_front_porch) {
            return rd::unexpected(IncorrectSyncTiming{
                .is_hsync = false,
                .expected = v_visible_area + v_front_porch,
                .actual = vsync_info.sync_start
            });
        }

        return {};
    }

    // If this function succeeds, the `process_vga_frame` function should generate correct frames
    auto sync() -> rd::expected<void, VGASimulatorError> {
        static constexpr auto max_pulses = h_total * v_total * 5;
        auto i = 0u;

        HSyncInfo hsync_info{};

        while(i < max_pulses) {
            scheduler->advance();

            if(detect_sync_pulse_change(hsync_info, vga_driver->hsync, i) && !hsync_info.is_in_sync_pulse) {
                break;
            }
            i++;
        }

        if (!hsync_info.sync_detected) {
            return rd::unexpected(HSyncUndetected{});
        }

        if (hsync_info.multiple_pulses_detected) {
            return rd::unexpected(MultiplePulsesDetected{.is_hsync = true});
        }

        if (hsync_info.get_pulse_length() != h_sync_pulse_width) {
            return rd::unexpected(IncorrectPulseWidth{
                .is_hsync = true,
                .expected = h_sync_pulse_width,
                .actual = hsync_info.get_pulse_length()
            });
        }

        for (auto j = 0u; j < h_back_porch; j++) {
            scheduler->advance();
        }

        VSyncInfo vsync_info{};

        // then vsync
        i = 0u;
        while (i < max_pulses) {
            if (const auto correct_row = process_vga_row([](const uint32_t, const uint32_t, const Color){}, false) ; !correct_row) {
                i += h_total;
                return rd::unexpected{correct_row.error()};
            }

            if(detect_sync_pulse_change(vsync_info, vga_driver->vsync, current_row) && !vsync_info.is_in_sync_pulse) {
                break;
            }
            i++;
        }

        if (!vsync_info.sync_detected) {
            return rd::unexpected(VSyncUndetected{});
        }

        if (vsync_info.multiple_pulses_detected) {
            return rd::unexpected(MultiplePulsesDetected{.is_hsync = false});
        }

        for (auto j = 0u; j < v_back_porch; j++) {
            if (!process_vga_row([](const uint32_t, const uint32_t, const Color){}, false)) {
                fmt::println("Incorrect row timing on row {}", current_row);
                i += h_total;
            }
        }

        return {};
    }

private:
    std::uint32_t current_row = 0;
    bool is_in_sync = false;

    // it assumes that the monitor and the simulator are synced up - the module is assumed to be in display time
    auto process_vga_row(DrawFunction draw_function, bool is_in_vertical_visible_area) -> rd::expected<void, VGASimulatorError> {
        uint32_t current_col = 0;

        HSyncInfo hsync_info{};

        // Process one complete horizontal line
        while (current_col < h_total) {
            scheduler->advance();

            detect_sync_pulse_change(hsync_info, vga_driver->hsync, current_col);

            if (is_in_vertical_visible_area && current_col < h_visible_area) {
                Color color = {
                    static_cast<unsigned char>(vga_driver->red * 16),
                    static_cast<unsigned char>(vga_driver->green * 16),
                    static_cast<unsigned char>(vga_driver->blue * 16),
                    255
                };

                draw_function(current_col, current_row, color);
            }

            current_col++;
        }

        if (!hsync_info.sync_detected) {
            return rd::unexpected(HSyncUndetected{});
        }

        if (hsync_info.multiple_pulses_detected) {
            return rd::unexpected(MultiplePulsesDetected{.is_hsync = true});
        }

        if (hsync_info.get_pulse_length() != h_sync_pulse_width) {
            return rd::unexpected(IncorrectPulseWidth{
                .is_hsync = true,
                .expected = h_sync_pulse_width,
                .actual = hsync_info.get_pulse_length()
            });
        }

        if (hsync_info.sync_start != h_visible_area + h_front_porch - 1) {
            return rd::unexpected(IncorrectSyncTiming{
                .is_hsync = true,
                .expected = hsync_info.sync_start,
                .actual = h_visible_area + h_front_porch
            });
        }

        return {};
    }

    struct SignalSyncInfo {
        bool sync_detected = false;
        bool is_in_sync_pulse = false;
        uint32_t sync_start = 0;
        uint32_t sync_end = 0;
        bool multiple_pulses_detected = false;

        auto get_pulse_length() const -> uint32_t {
            return sync_end - sync_start;
        }
    };

    // TODO: Check vsync and hsync polarity (whether it should be high or low during sync pulse)
    bool detect_sync_pulse_change(SignalSyncInfo& info, bool sync_signal, uint32_t position) {
        bool state_changed = false;

        if (!info.sync_detected && sync_signal) {
            info.is_in_sync_pulse = true;
            info.sync_detected = true;
            info.sync_start = position;
            state_changed = true;
        } else if (!info.is_in_sync_pulse && info.sync_detected && sync_signal) {
            info.multiple_pulses_detected = true;
        } else if (info.is_in_sync_pulse && !sync_signal) {
            info.is_in_sync_pulse = false;
            info.sync_end = position;
            state_changed = true;
        }

        return state_changed;
    }

    using HSyncInfo = SignalSyncInfo;
    using VSyncInfo = SignalSyncInfo;
};

