#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Vregister.h"
#include "verilated.h"
#include <iostream>

TEST_CASE("Register test enable data_in") {
    Vregister reg;

    reg.clk = 0;
    reg.enable = 0;
    reg.data_in = 0;

    for (int i = 0; i < 10; i++) {
        reg.clk = !reg.clk;
        reg.enable = 1;
        reg.data_in = i;

        reg.eval();

        CHECK(reg.data_out == 2 * (i / 2));
    }
}

TEST_CASE("Register test not enabled data_in") {
    Vregister reg;

    reg.clk = 0;
    reg.enable = 0;
    reg.data_in = 0;

    for (int i = 0; i < 10; i++) {
        reg.clk = !reg.clk;
        reg.enable = 0;
        reg.data_in = i;

        reg.eval();

        CHECK(reg.data_out == 0);
    }
}
