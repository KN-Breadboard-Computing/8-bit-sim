#include "clockable_module.hpp"
#include "vga_simulator.hpp"
#include <Vmonitor_tester.h>
#include <Vgpu.h>
#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>
#include <fmt/color.h>
#include <fmt/base.h>
#include <span>

// Raylib / Display constants
constexpr static uint32_t scale = 2u;
constexpr static auto scaled_width = static_cast<uint32_t>(h_visible_area * scale);
constexpr static auto scaled_height = static_cast<uint32_t>(v_visible_area * scale);

void set_pixel_scaled(const std::span<Color> pixels, uint32_t x, uint32_t y, Color color) {
    if (x >= h_visible_area || y >= v_visible_area) return;

    for (auto dy = 0u; dy < scale; dy++) {
        for (auto dx = 0u; dx < scale; dx++) {
            const auto index = (y * scale + dy) * scaled_width + (x * scale + dx);
            if (index < pixels.size()) {
                pixels[index] = color;
            }
        }
    }
}

void print_vga_error(const VGASimulatorError& error) {
    std::visit([&](const auto& err) {
        fmt::print(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "error: ");
        fmt::println("{}", err.message());
    }, error);
}

template <typename T>
void print_error_if_failed(const rd::expected<T, VGASimulatorError>& result) {
    if (!result) {
        print_vga_error(result.error());
    }
}

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

            auto is_timing_correct = simulator.process_vga_frame([&pixels](const uint32_t x, const uint32_t y, const Color color) {
                    set_pixel_scaled(pixels,x,y,color);
            });
            print_error_if_failed(is_timing_correct);

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
