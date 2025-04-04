#include "Vgpu.h"
#include "ps2.hpp"
#include "raylib.h"
#include <array>
#include <format>
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <rlImGui.h>
#include <span>

constexpr static uint32_t screen_width = 640u;
constexpr static uint32_t screen_height = 480u;
constexpr static uint32_t fps = 60u;
constexpr static uint32_t scale = 2u;

constexpr static auto scaled_width = static_cast<uint32_t>(screen_width * scale);
constexpr static auto scaled_height = static_cast<uint32_t>(screen_height * scale);

// auto logs = std::ofstream{"logs.txt"};

void handle_input(ps2::Keyboard& keyboard) {
    static auto pressed_keys = std::vector<KeyboardKey>{};
    //    auto prt = [&]() -> std::string {
    //        std::string result;
    //        for (auto key: pressed_keys) { result += std::to_string(key); }
    //        if(!pressed_keys.empty())result+='\n';
    //        return result;
    //    };
    //    logs << prt();
    auto key = static_cast<KeyboardKey>(GetKeyPressed());
    while (key > 0) {
        const auto packets = keyboard.encode_key(key, false);
        if (packets.has_value()) {
            pressed_keys.push_back(key);
            for (auto pac : *packets) {
                keyboard.packets_to_send.push(pac);
            }
        }
        key = static_cast<KeyboardKey>(GetKeyPressed());
    }
    for (unsigned i = 0; i < pressed_keys.size(); ++i) {
        if (!IsKeyDown(pressed_keys.at(i))) {
            const auto packets = keyboard.encode_key(pressed_keys.at(i), true);
            if (packets.has_value()) {
                for (auto pac : *packets) {
                    keyboard.packets_to_send.push(pac);
                }
            }
            pressed_keys.erase(pressed_keys.begin() + i--);
        }
    }
}

// x must be in [0, screen_width) and y in [0, screen_height)
void set_pixel(const std::span<Color> pixels, uint32_t x, uint32_t y, Color color) {
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

// Represents a Veilrator module that has a .clk signal
template <typename T>
concept ClockableModule = requires(T module) {
    module.eval();
    module.clk;
};

struct ClockBase {
    virtual void tick() = 0;
    virtual void advance(uint32_t delta) = 0;
    virtual auto get_time_till_next_tick() const -> uint32_t = 0;
};

template <ClockableModule T> struct Clock : ClockBase {
    Clock(T *module, uint32_t pos_period, uint32_t neg_period = 0, bool should_start_posedge = false)
        : pos_period{pos_period}, neg_period{neg_period}, module(module), is_posedge{should_start_posedge} {
        assert(!(pos_period == 0u && neg_period == 0u));
    }

    void tick() override {
        time_since_last_tick = 0u;
        module->clk = is_posedge;
        module->eval();
        is_posedge = !is_posedge;
        if (current_period() == 0u) {
            tick();
        }
    }

    void advance(uint32_t delta) override {
        time_since_last_tick += delta;

        if (time_since_last_tick == current_period()) {
            tick();
            return;
        }

        std::cerr << "Clock overshot by " << time_since_last_tick - current_period() << " cycles\n";
    }

    auto get_time_till_next_tick() const -> uint32_t override { return current_period() - time_since_last_tick; }
    auto current_period() const -> uint32_t { return is_posedge ? pos_period : neg_period; }

    const uint32_t pos_period;
    const uint32_t neg_period;

  private:
    T *module;

    bool is_posedge;
    uint32_t time_since_last_tick = 0u;
};

struct ClockScheduler {
    void add_clock(ClockBase *clock) { clocks.push_back(clock); }

    void advance() {
        auto min_time = std::numeric_limits<uint32_t>::max();

        for (auto clock : clocks) {
            const auto time = clock->get_time_till_next_tick();
            min_time = std::min(min_time, time);
        }

        for (auto clock : clocks) {
            clock->advance(min_time);
        }
    }

    std::vector<ClockBase *> clocks;
};

auto main() -> int {
    auto gpu = Vgpu{};
    auto gpu_clock = Clock{&gpu, 1, 0, true};
    auto clock_scheduler = ClockScheduler{};
    clock_scheduler.add_clock(&gpu_clock);

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
    auto keyboard = ps2::Keyboard{};

    rlImGuiSetup(true);

    while (!WindowShouldClose()) {
        // handle_input(keyboard);
        
        if (IsKeyReleased(KEY_A)) {
            gpu.interrupt_data_in = 48;
            gpu.interrupt_code_in = 0;
            gpu.interrupt_enable = 1;
            gpu.eval();
            gpu.interrupt_enable = 0;
            gpu.eval();
        }

        if (IsKeyReleased(KEY_D)) {
            gpu.interrupt_code_in = 2;
            gpu.interrupt_enable = 1;
            gpu.eval();
            gpu.interrupt_enable = 0;
            gpu.eval();
        }

        if (IsKeyReleased(KEY_S)) {
            gpu.interrupt_code_in = 3;
            gpu.interrupt_enable = 1;
            gpu.eval();
            gpu.interrupt_enable = 0;
            gpu.eval();
        }

        if (IsKeyReleased(KEY_SPACE)) {
            for (int i = 0; i < 800 * 525; i++) {
                clock_scheduler.advance();

                if (v_counter < screen_height && h_counter < screen_width) {
                    const auto red = gpu.red_out;
                    const auto blue = gpu.blue_out;
                    const auto green = gpu.green_out;
                    set_pixel(pixels, h_counter, v_counter, Color{red, green, blue, 255});
                }

                h_counter = (h_counter + 1) % 800;
                v_counter = (v_counter + (h_counter == 0)) % 525;
            }
        }

        UpdateTexture(texture, pixels.data());

        BeginDrawing();

        DrawTexture(texture, 0, 0, RAYWHITE);

        rlImGuiBegin();
        ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");
        ImGui::End();
        rlImGuiEnd();

        // DrawFPS(10, 10);

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
