#include "Valu.h"
#include "verilated.h"
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Valu alu;

    alu.a = 3;
    alu.b = 2;
    alu.op = 0;
    alu.eval();
    std::cout << "Test Addition: 3 + 2 = " << (int)alu.result << std::endl;

    alu.a = 5;
    alu.b = 3;
    alu.op = 1;
    alu.eval();
    std::cout << "Test Subtraction: 5 - 3 = " << (int)alu.result << std::endl;

    alu.final();
    return 0;
}
