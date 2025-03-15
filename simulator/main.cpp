#include <Vmonitor_tester.h>
#include <cstdint>
#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>
#include <iostream>
#include <span>

// VGA timing (based on VGA.v)
constexpr static uint32_t h_visible_area = 640u;
constexpr static uint32_t v_visible_area = 480u;
constexpr static uint32_t h_total = 800u;
constexpr static uint32_t v_total = 525u;
constexpr static uint32_t h_front_porch = 144u;
constexpr static uint32_t v_front_porch = 35u;
constexpr static uint32_t h_sync_pulse_width = 96u;
constexpr static uint32_t v_sync_pulse_width = 2u;

// Raylib / Display constants
constexpr static uint32_t scale = 1u;
constexpr static auto scaled_width = static_cast<uint32_t>(h_visible_area * scale);
constexpr static auto scaled_height = static_cast<uint32_t>(v_visible_area * scale);

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

void clk_gpu(Vmonitor_tester& monitor_tester) {
    monitor_tester.clock25MHz = 0;
    monitor_tester.eval();
    monitor_tester.clock25MHz = 1;
    monitor_tester.eval();
}

auto main() -> int {
    auto pixels = std::array<Color, scaled_width * scaled_height>{};

    Vmonitor_tester monitor_tester{};

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
            for (uint32_t y = 0; y < v_total; y++) {
                for (uint32_t x = 0; x < h_total; x++) {
                    clk_gpu(monitor_tester);

                    if (x >= h_front_porch && x < h_front_porch + h_visible_area &&
                            y >= v_front_porch && y < v_front_porch + v_visible_area) {

                        const uint8_t r = monitor_tester.red * 16;
                        const uint8_t g = monitor_tester.green * 16;
                        const uint8_t b = monitor_tester.blue * 16;

                        const auto display_x = x - h_front_porch;
                        const auto display_y = y - v_front_porch;

                        set_pixel_scaled(pixels, display_x, display_y, Color{r, g, b, 255});
                    }
                }
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
