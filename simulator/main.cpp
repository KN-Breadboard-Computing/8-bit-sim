#include "clockable_module.hpp"
#include "vga_simulator.hpp"
#include <Vcpu___024root.h>
#include <Vmem_unit.h>
#include <Vmonitor_tester.h>
#include <Vgpu.h>
#include <Vcpu.h>
#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>
#include <fmt/color.h>
#include <fmt/base.h>
#include <span>
#include <ps2.hpp>

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

void print_cpu(const Vcpu& cpu) {
    const auto rootp = cpu.rootp;
    const auto a_out = rootp->cpu_adapter__DOT__cpu__DOT__a_out;
    const auto b_out = rootp->cpu_adapter__DOT__cpu__DOT__b_out;
    const auto pc_out = rootp->cpu_adapter__DOT__cpu__DOT__pc_out;
    fmt::println("CPU: PC: {} | A: {} | B: {}", pc_out, a_out, b_out);
}

struct CpuAndMem {
    Vcpu* cpu;
    Vmem_unit* mem;

    CData* clk() {
        return &cpu->clk;
    }

    void eval() {
        cpu->eval();
        mem->eval();
    }
};

auto main() -> int {
    auto pixels = std::array<Color, scaled_width * scaled_height>{};

    auto ps2 = ps2::Keyboard{};

    Vmonitor_tester monitor_tester{};
    Vgpu gpu{};
    Vcpu cpu{};

    Vmem_unit cpu_memory{};
    CpuAndMem cpu_and_mem{&cpu, &cpu_memory};

    auto cpu_clock = Clock{&cpu_and_mem, 4, 0, true};
    auto gpu_clock = Clock{&gpu, 1, 0, true};

    auto clock_scheduler = ClockScheduler{};
    clock_scheduler.add_clock(&gpu_clock);
    clock_scheduler.add_clock(&cpu_clock);

    gpu.rst = 0;
    VGASimulator simulator(&gpu, &clock_scheduler);

    simulator.sync();

    InitWindow(scaled_width, scaled_height, "VGA tester");

    const Image image = {.data = pixels.data(),
                         .width = scaled_width,
                         .height = scaled_height,
                         .mipmaps = 1,
                         .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

    const auto texture = LoadTextureFromImage(image);

    rlImGuiSetup(true);

    bool wait_for_key = false;
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_SPACE)) {

            auto is_timing_correct = simulator.process_vga_frame([&pixels](const uint32_t x, const uint32_t y, const Color color) {
                    set_pixel_scaled(pixels,x,y,color);
            });
            print_error_if_failed(is_timing_correct);
            print_cpu(cpu);

            UpdateTexture(texture, pixels.data());
        }

        if (IsKeyPressed(KEY_I)) {
            GetCharPressed(); // extract 'i' from the queue
            wait_for_key = true;
            // TODO: color support
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            gpu.interrupt_enable = 1;
            gpu.interrupt_code_in = 0b01;
            gpu.interrupt_data_in = 0x81;
            gpu.eval();
            gpu.interrupt_enable = 0;
            gpu.eval();
        }

        if (IsKeyPressed(KEY_LEFT)) {
            gpu.interrupt_enable = 1;
            gpu.interrupt_code_in = 0b01;
            gpu.interrupt_data_in = 0xFF;
            gpu.eval();
            gpu.interrupt_enable = 0;
            gpu.eval();
        }

        if (IsKeyPressed(KEY_UP)) {
            gpu.interrupt_enable = 1;
            gpu.interrupt_code_in = 0b01;
            gpu.interrupt_data_in = 0x7F;
            gpu.eval();
            gpu.interrupt_enable = 0;
            gpu.eval();
        }

        if (IsKeyPressed(KEY_DOWN)) {
            gpu.interrupt_enable = 1;
            gpu.interrupt_code_in = 0b01;
            gpu.interrupt_data_in = 0x01;
            gpu.eval();
            gpu.interrupt_enable = 0;
            gpu.eval();
        }

        if (wait_for_key) {
            char c;
            if ((c = (char)GetCharPressed())) {
                gpu.interrupt_enable = 1;
                gpu.interrupt_code_in = 0b00;
                gpu.interrupt_data_in = c;
                gpu.eval();
                gpu.interrupt_enable = 0;
                gpu.eval();
                wait_for_key = false;
            }
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
