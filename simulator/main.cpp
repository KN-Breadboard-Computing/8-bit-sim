#include "Valu.h"
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
    for (auto i = 0u; i < scale; i++) {
        for (auto j = 0u; j < scale; j++) {
            const auto dx = x * scale + j;
            const auto dy = y * scale + i;
            pixels[dy * scaled_width + dx] = color;
        }
    }
}

auto main() -> int {
    Valu alu;

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
        const auto mouse = GetMousePosition();
        const auto x = static_cast<uint32_t>(mouse.x / scale);
        const auto y = static_cast<uint32_t>(mouse.y / scale);

        set_pixel(pixels, x, y, RED);

        UpdateTexture(texture, pixels.data());

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTexture(texture, 0, 0, RAYWHITE);

        EndDrawing();
    }
    return 0;
}
