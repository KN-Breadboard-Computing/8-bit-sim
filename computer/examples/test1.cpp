#include <iostream>
#include "Valu.h"
#include "verilated.h"

auto main() -> int {
    Valu alu;

    alu.clk = 0;
    alu.eval();

    alu.clk = 1;
    alu.reg_a = 5;
    alu.reg_b = 10;
    alu.opcode = 0b00111;
    alu.out_enable = 1;
    alu.eval();

    std::cout << "alu.data_out = " << (int)alu.data_out << std::endl;
    std::cout << "Hello, world!" << std::endl;
}
