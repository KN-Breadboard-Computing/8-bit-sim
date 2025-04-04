#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Vshift_reg.h"
#include "doctest/doctest.h"

TEST_CASE("Shift register - set value") {
    Vshift_reg shift_reg;
    unsigned char value = 0b10110101;

    shift_reg.clk = 0;
    shift_reg.eval();
    shift_reg.clk = 1;
    shift_reg.data_in_enable = 1;
    shift_reg.shift_enable = 0;
    shift_reg.data_in = value;
    shift_reg.eval();

    for (int i = 0; i < 8; i++) {
        unsigned char mask = 1 << (7 - i);
        unsigned char expected = (value & mask) >> (7 - i);

        CHECK(shift_reg.data_out == expected);

        shift_reg.clk = 0;
        shift_reg.eval();

        shift_reg.clk = 1;
        shift_reg.data_in_enable = 0;
        shift_reg.shift_enable = 1;
        shift_reg.eval();
    }
}
