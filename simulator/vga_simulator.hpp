#pragma once
#include "clockable_module.hpp"
#include <Vmonitor_tester.h>
#include <Vgpu.h>
#include <cstdint>
#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>
#include <span>

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

// Raylib / Display constants
constexpr static uint32_t scale = 1u;
constexpr static auto scaled_width = static_cast<uint32_t>(h_visible_area * scale);
constexpr static auto scaled_height = static_cast<uint32_t>(v_visible_area * scale);

template <typename T>
concept VerilatedVGADriver = ClockableModule<T> && requires(T module) {
    module.red;
    module.green;
    module.blue;
    module.hsync;
    module.vsync;
};

inline void set_pixel_scaled(const std::span<Color> colors, uint32_t x, uint32_t y, Color color) {
    if (x >= h_visible_area || y >= v_visible_area) return;

    for (auto dy = 0u; dy < scale; dy++) {
        for (auto dx = 0u; dx < scale; dx++) {
            const auto index = (y * scale + dy) * scaled_width + (x * scale + dx);
            if (index < colors.size()) {
                colors[index] = color;
            }
        }
    }
}

template <VerilatedVGADriver T>
struct VGASimulator {
    T* vga_driver;
    ClockScheduler* scheduler;

    VGASimulator(T* vga_driver, ClockScheduler* scheduler)
        : vga_driver(vga_driver), scheduler(scheduler), current_row(0) {}

    // it assumes that the monitor and the simulator are synced up and therefore it starts with display time
    auto process_vga_row(std::span<Color> pixels, bool is_in_vertical_visible_area) -> bool {
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

                set_pixel_scaled(pixels, current_col, current_row, color);
            }

            current_col++;
        }

        if (!hsync_info.sync_detected) {
            fmt::println("error: Hsync undetected.");
            return false;
        }

        if (hsync_info.multiple_pulses_detected) {
            fmt::println("error: Multiple hsync pulses detected.");
            return false;
        }

        if (hsync_info.get_pulse_length() != h_sync_pulse_width) {
            fmt::println("error: Incorrect hsync sync pulse width, expected: {}, found {}", h_sync_pulse_width, hsync_info.get_pulse_length());
            return false;
        }

        if (hsync_info.sync_start != h_visible_area + h_front_porch - 1) {
            fmt::println("error: hsync expected on pulse {}, found on: {}", h_visible_area + h_front_porch, hsync_info.sync_start);
        }

        return true;
    }

    auto process_vga_frame(std::span<Color> pixels) -> bool {
        current_row = 0;

        VSyncInfo vsync_info{};

        bool is_in_visible_area{};

        while (current_row <= v_total) {
            is_in_visible_area = current_row < v_visible_area;
            if (!process_vga_row(pixels, is_in_visible_area)) {
                fmt::println("Incorrect row timing on row {}", current_row);
            }

            detect_sync_pulse_change(vsync_info, vga_driver->vsync, current_row);
            current_row++;
        }

        if (!vsync_info.sync_detected) {
            fmt::println("error: vsync undetected.");
            return false;
        }

        if (vsync_info.multiple_pulses_detected) {
            fmt::println("error: Multiple vsync pulses detected.");
            return false;
        }

        if (vsync_info.get_pulse_length() != v_sync_pulse_width) {
            fmt::println("error: Incorrect vsync sync pulse width, expected: {}, found {}", v_sync_pulse_width, vsync_info.get_pulse_length());
            return false;
        }

        if (vsync_info.sync_start != v_visible_area + v_front_porch) {
            fmt::println("error: vsync expected on pulse {}, found on: {}", v_visible_area + v_front_porch, vsync_info.sync_start);
        }

        return true;
    }

    // If this function succeeds, the `process_vga_frame` function should generate correct frames
    auto sync() -> bool {
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
            return false;
        }

        if (hsync_info.multiple_pulses_detected) {
            return false;
        }

        if (hsync_info.get_pulse_length() != h_sync_pulse_width) {
            fmt::println("error: Incorrect hsync sync pulse width, expected: {}, found {}", h_sync_pulse_width, hsync_info.get_pulse_length());
            return false;
        }

        for (auto j = 0u; j < h_back_porch; j++) {
            scheduler->advance();
        }

        VSyncInfo vsync_info{};

        auto pixels = std::array<Color, scaled_width * scaled_height>{};

        // then vsync
        i = 0u;
        while (i < max_pulses) {
            if (!process_vga_row(pixels, false)) {
                fmt::println("Incorrect row timing on row {}", current_row);
                i += h_total;
            }

            if(detect_sync_pulse_change(vsync_info, vga_driver->vsync, current_row) && !vsync_info.is_in_sync_pulse) {
                break;
            }
            i++;
        }

        if (!vsync_info.sync_detected) {
            return false;
        }

        if (vsync_info.multiple_pulses_detected) {
            return false;
        }

        for (auto j = 0u; j < v_back_porch; j++) {
            if (!process_vga_row(pixels, false)) {
                fmt::println("Incorrect row timing on row {}", current_row);
                i += h_total;
            }
        }

        return is_in_sync;
    }

private:
    std::uint32_t current_row = 0;
    bool is_in_sync = false;

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

