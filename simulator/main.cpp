#include "Vgpu.h"
#include "raylib.h"
#include <array>
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
    gpu.eval();

    auto pixels = std::array<Color, scaled_width * scaled_height>{};

    InitWindow(scaled_width, scaled_height, "Hello World!");

    SetTargetFPS(fps);

    const Image image = {.data = pixels.data(),
                         .width = scaled_width,
                         .height = scaled_height,
                         .mipmaps = 1,
                         .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

    const auto texture = LoadTextureFromImage(image);

    while (!WindowShouldClose()) {
        gpu.clk = !gpu.clk;
        gpu.eval();

        for (auto y = 0u; y < screen_height; y++) {
            for (auto x = 0u; x < screen_width; x++) {
                const auto red = gpu.red_out;
                const auto blue = gpu.blue_out;
                const auto green = gpu.green_out;
                set_pixel(pixels, x, y, Color{red, green, blue, 255});
            }
        }

        UpdateTexture(texture, pixels.data());

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTexture(texture, 0, 0, RAYWHITE);

        DrawFPS(10, 10);

        EndDrawing();
    }
    return 0;
}
