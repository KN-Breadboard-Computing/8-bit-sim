#include "clockable_module.hpp"
#include <Vmonitor_tester.h>
#include <Vgpu.h>
#include <concepts>
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

void set_pixel_scaled(const std::span<Color> colors, uint32_t x, uint32_t y, Color color) {
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

        bool hsync_detected = false;
        bool is_in_sync_pulse = false;
        uint32_t hsync_start = 0;
        uint32_t hsync_end = 0;

        bool multiple_hsync_pulses_detected = false;

        // Process one complete horizontal line
        while (current_col < h_total) {
            scheduler->advance();

            // TODO: Check hsync and vsync polarity (whether it should be high or low during sync pulse)
            if (!hsync_detected && vga_driver->hsync) {
                is_in_sync_pulse = true;
                hsync_detected = true;
                hsync_start = current_col;
            } else if (!is_in_sync_pulse && hsync_detected && vga_driver->hsync) {
                multiple_hsync_pulses_detected = true;
            } else if (is_in_sync_pulse && !vga_driver->hsync) {
                is_in_sync_pulse = false;
                hsync_end = current_col;
            }

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

        if (!hsync_detected) {
            fmt::println("error: Hsync undetected.");
            return false;
        }

        if (multiple_hsync_pulses_detected) {
            fmt::println("error: Multiple hsync pulses detected.");
            return false;
        }

        if (hsync_end - hsync_start != h_sync_pulse_width) {
            fmt::println("error: Incorrect hsync sync pulse width, expected: {}, found {}", h_sync_pulse_width, hsync_end - hsync_start);
            return false;
        }

        if (hsync_start != h_visible_area + h_front_porch - 1) {
            fmt::println("error: hsync expected on pulse {}, found on: {}", h_visible_area + h_front_porch, hsync_start);
        }

        return true;
    }

    auto process_vga_frame(std::span<Color> pixels) -> bool {
        current_row = 0;

        bool vsync_detected = false;
        bool is_in_sync_pulse = false;
        uint32_t vsync_start = 0;
        uint32_t vsync_end = 0;

        bool is_in_visible_area{};

        bool multiple_vsync_pulses_detected = false;

        while (current_row <= v_total) {
            is_in_visible_area = current_row < v_visible_area;
            if (!process_vga_row(pixels, is_in_visible_area)) {
                fmt::println("Incorrect row timing on row {}", current_row);
            }

            // TODO: Check vsync and vsync polarity (whether it should be high or low during sync pulse)
            if (!vsync_detected && vga_driver->vsync) {
                is_in_sync_pulse = true;
                vsync_detected = true;
                vsync_start = current_row;
            } else if (!is_in_sync_pulse && vsync_detected && vga_driver->vsync) {
                multiple_vsync_pulses_detected = true;
            } else if (is_in_sync_pulse && !vga_driver->vsync) {
                is_in_sync_pulse = false;
                vsync_end = current_row;
            }

            current_row++;
        }

        if (!vsync_detected) {
            fmt::println("error: vsync undetected.");
            return false;
        }

        if (multiple_vsync_pulses_detected) {
            fmt::println("error: Multiple vsync pulses detected.");
            return false;
        }

        if (vsync_end - vsync_start != v_sync_pulse_width) {
            fmt::println("error: Incorrect vsync sync pulse width, expected: {}, found {}", v_sync_pulse_width, vsync_end - vsync_start);
            return false;
        }

        if (vsync_start != v_visible_area + v_front_porch) {
            fmt::println("error: vsync expected on pulse {}, found on: {}", v_visible_area + v_front_porch, vsync_start);
        }

        return true;
    }

    // If this function succeeds, the `process_vga_frame` function should generate correct frames
    auto sync() -> bool {
        static constexpr auto max_pulses = h_total * v_total * 5;
        auto i = 0u;

        bool hsync_detected = false;
        bool is_in_sync_pulse = false;
        uint32_t hsync_start = 0;
        uint32_t hsync_end = 0;

        bool multiple_hsync_pulses_detected = false;

        while(i < max_pulses) {
            scheduler->advance();

            if (!hsync_detected && vga_driver->hsync) {
                is_in_sync_pulse = true;
                hsync_detected = true;
                hsync_start = i;
            } else if (!is_in_sync_pulse && hsync_detected && vga_driver->hsync) {
                multiple_hsync_pulses_detected = true;
            } else if (is_in_sync_pulse && !vga_driver->hsync) {
                is_in_sync_pulse = false;
                hsync_end = i;
                break;
            }
            i++;
        }

        if (!hsync_detected) {
            return false;
        }

        if (multiple_hsync_pulses_detected) {
            return false;
        }

        if (hsync_end - hsync_start != h_sync_pulse_width) {
            fmt::println("error: Incorrect hsync sync pulse width, expected: {}, found {}", h_sync_pulse_width, hsync_end - hsync_start);
            return false;
        }

        for (auto j = 0u; j < h_back_porch; j++) {
            scheduler->advance();
        }

        bool vsync_detected = false;
        uint32_t vsync_start = 0;
        uint32_t vsync_end = 0;
        is_in_sync_pulse = false;

        bool multiple_vsync_pulses_detected = false;

        auto pixels = std::array<Color, scaled_width * scaled_height>{};

        // then vsync
        i = 0u;
        while (i < max_pulses) {
            if (!process_vga_row(pixels, false)) {
                fmt::println("Incorrect row timing on row {}", current_row);
                i += h_total;
            }

            // TODO: Check vsync and vsync polarity (whether it should be high or low during sync pulse)
            if (!vsync_detected && vga_driver->vsync) {
                is_in_sync_pulse = true;
                vsync_detected = true;
                vsync_start = current_row;
            } else if (!is_in_sync_pulse && vsync_detected && vga_driver->vsync) {
                multiple_vsync_pulses_detected = true;
            } else if (is_in_sync_pulse && !vga_driver->vsync) {
                is_in_sync_pulse = false;
                vsync_end = current_row;
                break;
            }
            i++;
        }

        if (!vsync_detected) {
            return false;
        }

        if (multiple_vsync_pulses_detected) {
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
};

auto main() -> int {
    auto pixels = std::array<Color, scaled_width * scaled_height>{};

    Vmonitor_tester monitor_tester{};
    Vgpu gpu{};

    auto gpu_clock = Clock{&monitor_tester, 1, 0, true};
    auto clock_scheduler = ClockScheduler{};
    clock_scheduler.add_clock(&gpu_clock);

    VGASimulator<Vmonitor_tester> simulator(&monitor_tester, &clock_scheduler);

    simulator.sync();

    InitWindow(scaled_width, scaled_height, "VGA tester");

    const Image image = {.data = pixels.data(),
                         .width = scaled_width,
                         .height = scaled_height,
                         .mipmaps = 1,
                         .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

    const auto texture = LoadTextureFromImage(image);

    rlImGuiSetup(true);

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_SPACE)) {
            // do not ask why the +1
            /*for (uint32_t y = 0; y < v_total + 1; y++) {*/
            /*    for (uint32_t x = 0; x < h_total; x++) {*/
            /*        clk_gpu(monitor_tester);*/
            /*        gpu.clk = 0;*/
            /*        gpu.eval();*/
            /*        gpu.clk = 1;*/
            /*        gpu.eval();*/
            /*        if (x >= h_front_porch && x < h_front_porch + h_visible_area &&*/
            /*                y >= v_front_porch && y < v_front_porch + v_visible_area) {*/
            /**/
            /*            const uint8_t r = gpu.red_out * 16;*/
            /*            const uint8_t g = gpu.green_out * 16;*/
            /*            const uint8_t b = gpu.blue_out * 16;*/
            /**/
            /*            const auto display_x = x - h_front_porch;*/
            /*            const auto display_y = y - v_front_porch;*/
            /**/
            /*            set_pixel_scaled(pixels, display_x, display_y, Color{r, g, b, 255});*/
            /*        }*/
            /*    }*/
            /*}*/

            bool is_timing_correct = simulator.process_vga_frame(pixels);
            if(!is_timing_correct) {
                fmt::println("Error: incorrect timing");
            }
            UpdateTexture(texture, pixels.data());
        }

        BeginDrawing();

        DrawTexture(texture, 0, 0, RAYWHITE);

        /*rlImGuiBegin();*/
        /*ImGui::Begin("Hello, world!");*/
        /*ImGui::End();*/
        /*rlImGuiEnd();*/

        // DrawFPS(10, 10);

        EndDrawing();

    }
    return 0;
}
