#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Vtristate_buffer.h"
#include "verilated.h"
#include <iostream>
#include <memory>

TEST_CASE("ALU test") {
    Vtristate_buffer buffer;

    buffer.enable = true;
    buffer.data_in= 0x55;
    buffer.eval();
    CHECK(buffer.data_out == 0x55);

    buffer.enable = false;
    buffer.data_in = 0x56;
    buffer.eval();
    CHECK(buffer.data_out == 0);
}
