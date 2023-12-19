#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Valu.h"
#include "verilated.h"

TEST_CASE("ALU Operations") {
    Valu alu;

    SUBCASE("0") {
        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b00000;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == 0);
    }

    SUBCASE("1") {
        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b00001;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == 1);
    }

    SUBCASE("-1") {
        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b00010;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == (uint8_t)-1);
    }

    SUBCASE("A") {
        uint8_t reg_a_val = 5;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b00011;
        alu.reg_a = reg_a_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == reg_a_val);
    }

    SUBCASE("B") {
        uint8_t reg_b_val = 5;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b00100;
        alu.reg_b = reg_b_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == reg_b_val);
    }

    SUBCASE("-A") {
        uint8_t reg_a_val = 5;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b00101;
        alu.reg_a = reg_a_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == (uint8_t)-reg_a_val);
    }

    SUBCASE("-B") {
        uint8_t reg_b_val = 5;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b00110;
        alu.reg_b = reg_b_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == (uint8_t)-reg_b_val);
    }

    SUBCASE("A + B") {
        uint8_t reg_a_val = 5;
        uint8_t reg_b_val = 10;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b00111;
        alu.reg_a = reg_a_val;
        alu.reg_b = reg_b_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == 15);
    }


    SUBCASE("A - B") {
        uint8_t reg_a_val = 15;
        uint8_t reg_b_val = 5;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b01000;
        alu.reg_a = reg_a_val;
        alu.reg_b = reg_b_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == 10);
    }

    SUBCASE("B - A") {
        uint8_t reg_a_val = 5;
        uint8_t reg_b_val = 15;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b01001;
        alu.reg_a = reg_a_val;
        alu.reg_b = reg_b_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == 10);
    }

    SUBCASE("~A") {
        uint8_t reg_a_val = 5;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b01010;
        alu.reg_a = reg_a_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == (uint8_t)~reg_a_val);
    }

    SUBCASE("~B") {
        uint8_t reg_b_val = 5;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b01011;
        alu.reg_b = reg_b_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == (uint8_t)~reg_b_val);
    }

    SUBCASE("A | B") {
        uint8_t reg_a_val = 0b10101010;
        uint8_t reg_b_val = 0b01010101;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b01100;
        alu.reg_a = reg_a_val;
        alu.reg_b = reg_b_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == (reg_a_val | reg_b_val));
    }

    SUBCASE("A & B") {
        uint8_t reg_a_val = 0b10101010;
        uint8_t reg_b_val = 0b01010101;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b01101;
        alu.reg_a = reg_a_val;
        alu.reg_b = reg_b_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == (reg_a_val & reg_b_val));
    }

    SUBCASE("A ^ B") {
        uint8_t reg_a_val = 0b10101010;
        uint8_t reg_b_val = 0b01010101;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b01110;
        alu.reg_a = reg_a_val;
        alu.reg_b = reg_b_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == (reg_a_val ^ reg_b_val));
    }

    SUBCASE("A >> 1") {
        uint8_t reg_a_val = 0b10101010;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b01111;
        alu.reg_a = reg_a_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == (reg_a_val >> 1));
    }

    SUBCASE("B >> 1") {
        uint8_t reg_b_val = 0b10101010;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b10000;
        alu.reg_b = reg_b_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == (reg_b_val >> 1));
    }

    SUBCASE("A << 1") {
        uint8_t reg_a_val = 0b10101010;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b10001;
        alu.reg_a = reg_a_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == (uint8_t)(reg_a_val << 1));
    }

    SUBCASE("B << 1") {
        uint8_t reg_b_val = 0b10101010;

        alu.clk = 0;
        alu.eval();
        alu.out_enable = 1;
        alu.opcode = 0b10010;
        alu.reg_b = reg_b_val;

        alu.clk = !alu.clk;
        alu.eval();

        CHECK(alu.data_out == (uint8_t)(reg_b_val << 1));
    }

    // TODO: Add logic shifts
}

// TODO: Add tests for ALU flags

