#include <Vmonitor_tester.h>
#include <cstdint>
#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>
#include <iostream>

constexpr static uint32_t screen_width = 640u;
constexpr static uint32_t screen_height = 480u;
constexpr static uint32_t fps = 60u;
constexpr static uint32_t scale = 2u;

constexpr static auto scaled_width = static_cast<uint32_t>(screen_width * scale);
constexpr static auto scaled_height = static_cast<uint32_t>(screen_height * scale);

using VGAChannel = uint8_t;

struct SignalVerifier {
    // input
    bool hsync;
    bool vsync;
    VGAChannel red;
    VGAChannel Green;
    VGAChannel Blue;

    private:
    uint64_t clock = 0;
};

auto main() -> int {
    size_t gpu_frame = 0;

    auto pixels = std::array<Color, scaled_width * scaled_height>{};

    Vmonitor_tester monitor_tester{};

    unsigned int v_counter = 0u;
    unsigned int h_counter = 0u;

    InitWindow(scaled_width, scaled_height, "VGA tester");

    const Image image = {.data = pixels.data(),
                         .width = scaled_width,
                         .height = scaled_height,
                         .mipmaps = 1,
                         .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

    const auto texture = LoadTextureFromImage(image);

    rlImGuiSetup(true);

    auto i = 0u;

    while (!WindowShouldClose()) {
        monitor_tester.clock25MHz = 0;
        monitor_tester.eval();
        monitor_tester.clock25MHz = 1;
        monitor_tester.eval();

        std::cout << "R: " << (int)monitor_tester.red << " G: " << (int)monitor_tester.green << " B: " << (int)monitor_tester.blue
                  << std::endl;

        if ( i < screen_width * screen_height ) {
            pixels[i] = Color{monitor_tester.red, monitor_tester.green, monitor_tester.blue, 255};

            i++;

            gpu_frame++;
        }

        UpdateTexture(texture, pixels.data());

        BeginDrawing();

        DrawTexture(texture, 0, 0, RAYWHITE);

        rlImGuiBegin();
        ImGui::Begin("Hello, world!");
        //ImGui::Text("This is some useful text.");
        ImGui::Text("Frame: %lu", gpu_frame);
        ImGui::End();
        rlImGuiEnd();

        // DrawFPS(10, 10);

        EndDrawing();

    }
    return 0;
}
