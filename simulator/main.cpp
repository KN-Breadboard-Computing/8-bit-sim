#include "Vgpu.h"
#include "raylib.h"
#include <array>
#include <fstream>
#include <iostream>
#include <span>

constexpr static uint32_t screen_width = 640u;
constexpr static uint32_t screen_height = 480u;
constexpr static uint32_t fps = 60u;
constexpr static uint32_t scale = 2u;

constexpr static auto scaled_width = static_cast<uint32_t>(screen_width * scale);
constexpr static auto scaled_height = static_cast<uint32_t>(screen_height * scale);

// x must be in [0, screen_width) and y in [0, screen_height)
void set_pixel(std::span<Color> pixels, uint32_t x, uint32_t y, Color color) {
    if (x >= screen_width || y >= screen_height) {
        return;
    }

    for (auto i = 0u; i < scale; i++) {
        for (auto j = 0u; j < scale; j++) {
            const auto dx = x * scale + j;
            const auto dy = y * scale + i;
            pixels[dy * scaled_width + dx] = color;
        }
    }
}

auto main() -> int {
    auto gpu = Vgpu{};

    auto pixels = std::array<Color, scaled_width * scaled_height>{};

    InitWindow(scaled_width, scaled_height, "Hello World!");

    unsigned int v_counter = 0u;
    unsigned int h_counter = 0u;

    const Image image = {.data = pixels.data(),
                         .width = scaled_width,
                         .height = scaled_height,
                         .mipmaps = 1,
                         .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

    const auto texture = LoadTextureFromImage(image);

    // open file
    auto logs = std::ofstream{"logs.txt"};

    while (!WindowShouldClose()) {
        for (int i = 0; i < 800 * 525; i++) {
            gpu.clk = 0;
            gpu.eval();
            gpu.clk = 1;
            gpu.eval();

            if (v_counter < screen_height && h_counter < screen_width) {
                const auto red = gpu.red_out;
                const auto blue = gpu.blue_out;
                const auto green = gpu.green_out;
                const auto is_black = gpu.red_out == 0 && gpu.blue_out == 0 && gpu.green_out == 0;
                logs << "x: " << h_counter << ", y: " << v_counter << ", color: " << (is_black ? 0 : 1) << std::endl;
                set_pixel(pixels, h_counter, v_counter, Color{red, green, blue, 255});
            }

            h_counter = (h_counter + 1) % 800;
            v_counter = (v_counter + (h_counter == 0)) % 525;
        }

        UpdateTexture(texture, pixels.data());

        BeginDrawing();

        DrawTexture(texture, 0, 0, RAYWHITE);

        DrawFPS(10, 10);

        EndDrawing();

        return 0;
    }
    return 0;
}
