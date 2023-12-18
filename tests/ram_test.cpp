#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Vram.h"
#include "verilated.h"
#include <iostream>

TEST_CASE("Ram test") {
    Vram ram;

    ram.clk = 0;
    ram.write_enable = 0;
    ram.output_enable = 0;
    ram.address = 0;
    ram.data = 0;

    for (int i = 0; i < 10; i++) {
        ram.clk = !ram.clk;
        ram.write_enable = 1;
        ram.output_enable = 0;
        ram.address = i;
        ram.data = i;

        ram.eval();
    }

    for (int i = 0; i < 10; i++) {
        ram.clk = !ram.clk;
        ram.write_enable = 0;
        ram.output_enable = 1;
        ram.address = i;

        ram.eval();

        CHECK(ram.data == i);
    }
}
