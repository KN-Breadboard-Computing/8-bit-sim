#include "clockable_module.hpp"
#include "vga_simulator.hpp"
#include <Vmonitor_tester.h>
#include <Vgpu.h>
#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

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
