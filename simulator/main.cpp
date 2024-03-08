#include "Valu.h"
#include "raylib.h"
#include <array>
#include <iostream>
#include <vector>

constexpr static int screen_width = 640;
constexpr static int screen_height = 480;
constexpr static int fps = 60;

auto main() -> int {
    Valu alu;

    auto pixels = std::array<Color, screen_width * screen_height>{};

    InitWindow(screen_width, screen_height, "Hello World!");

    SetTargetFPS(fps);

    for (auto i = 0u; i < screen_width * screen_height; i++)
        pixels[i] = RED;

    const Image image = {.data = pixels.data(),
                         .width = screen_width,
                         .height = screen_height,
                         .mipmaps = 1,
                         .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

    const auto texture = LoadTextureFromImage(image);

    while (!WindowShouldClose()) {
        const auto mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            std::cout << "Mouse clicked at: " << mouse.x << ", " << mouse.y << std::endl;

            pixels[(unsigned int)mouse.x + (unsigned int)mouse.y * screen_width] = BLUE;

            UpdateTexture(texture, pixels.data());
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTexture(texture, 0, 0, RAYWHITE);

        EndDrawing();
    }
    return 0;
}
