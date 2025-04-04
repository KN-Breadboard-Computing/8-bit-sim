#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Vtristate_buffer.h"
#include "verilated.h"
#include <iostream>

TEST_CASE("Tristate buffer test") {
    Vtristate_buffer buffer;

    const auto value1 = 0x55;
    const auto value2 = 0x56;

    buffer.enable = true;
    buffer.data_in = value1;
    buffer.eval();
    CHECK(buffer.data_out == value1);

    buffer.enable = false;
    buffer.data_in = value2;
    buffer.eval();
    CHECK(buffer.data_out == 0);
}
