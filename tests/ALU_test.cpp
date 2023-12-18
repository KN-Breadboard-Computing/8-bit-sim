#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Valu.h"
#include "verilated.h"
#include <iostream>
#include <memory>

TEST_CASE("ALU test") {
    Valu alu;

    alu.a = 3;
    alu.b = 2;
    alu.op = 0;
    alu.eval();
    CHECK((int)alu.result == 5);
}
