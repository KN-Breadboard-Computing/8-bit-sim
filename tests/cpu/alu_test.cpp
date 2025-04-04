#include <bitset>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Valu.h"
#include "doctest/doctest.h"
#include "tests_common.hpp"
#include "verilated.h"
#include <functional>
#include <random>

struct AluInputState {
    CData opcode;
    std::string test_title;
    CData reg_a = 0b00000000;
    CData reg_b = 0b00000000;
    std::function<CData()> expected_value_func;
};

constexpr auto TEST_COUNT = 1000u;

TEST_CASE("ALU Operations") {
    Valu *alu;
    AluInputState data;

    CData A;
    CData B;

    const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, std::numeric_limits<CData>::max());
    const auto random_cdata = [&]() -> CData { return static_cast<CData>(dist(rng)); };

    auto alu_input_state = std::vector<AluInputState>{
        {.opcode = 0b00000, .test_title = "Emit 0", .expected_value_func = [&]() -> CData { return 0; }},
        {.opcode = 0b00001, .test_title = "Emit 1", .expected_value_func = [&]() -> CData { return 1; }},
        {.opcode = 0b00010, .test_title = "Emit -1", .expected_value_func = [&]() -> CData { return (CData)-1; }},
        {.opcode = 0b00011, .test_title = "Emit A", .expected_value_func = [&]() -> CData { return A; }},
        {.opcode = 0b00100, .test_title = "Emit B", .expected_value_func = [&]() -> CData { return B; }},
        {.opcode = 0b00101, .test_title = "Emit -A", .expected_value_func = [&]() -> CData { return -A; }},
        {.opcode = 0b00110, .test_title = "Emit -B", .expected_value_func = [&]() -> CData { return -B; }},
        {.opcode = 0b00111, .test_title = "Emit A+B", .expected_value_func = [&]() -> CData { return A + B; }},
        {.opcode = 0b01000, .test_title = "Emit A-B", .expected_value_func = [&]() -> CData { return A - B; }},
        {.opcode = 0b01001, .test_title = "Emit B-A", .expected_value_func = [&]() -> CData { return B - A; }},
        {.opcode = 0b01010, .test_title = "Emit ~A", .expected_value_func = [&]() -> CData { return ~A; }},
        {.opcode = 0b01011, .test_title = "Emit ~B", .expected_value_func = [&]() -> CData { return ~B; }},
        {.opcode = 0b01100, .test_title = "Emit A|B", .expected_value_func = [&]() -> CData { return A | B; }},
        {.opcode = 0b01101, .test_title = "Emit A&B", .expected_value_func = [&]() -> CData { return A & B; }},
        {.opcode = 0b01110, .test_title = "Emit A^B", .expected_value_func = [&]() -> CData { return A ^ B; }},
        {.opcode = 0b01111, .test_title = "Emit A>>1", .expected_value_func = [&]() -> CData { return A >> 1; }},
        {.opcode = 0b10000, .test_title = "Emit B>>1", .expected_value_func = [&]() -> CData { return B >> 1; }},
        {.opcode = 0b10001, .test_title = "Emit A<<1", .expected_value_func = [&]() -> CData { return A << 1; }},
        {.opcode = 0b10010, .test_title = "Emit B<<1", .expected_value_func = [&]() -> CData { return B << 1; }},
    };

    DOCTEST_VALUE_PARAMETERIZED_DATA(data, alu_input_state);

    for (auto i = 0; i < TEST_COUNT; i++) {
        alu = new Valu();
        alu->eval();
        alu->alu_out = 0;
        alu->reg_f_load = 0;
        alu->reg_f_out = 1;
        alu->opcode = data.opcode;

        A = random_cdata();
        B = random_cdata();

        const auto expected = data.expected_value_func();

        INFO(data.test_title, ", opcode: ", std::bitset<8>(data.opcode), ", A: ", A, ", B: ", B,
             ", expected: ", expected);

        alu->reg_a = A;
        alu->reg_b = B;
        alu->eval();

        CHECK(alu->data_out == expected);

        delete alu;
    }
}


TEST_CASE("ALU Flags") {
    Valu *alu;
    AluInputState data;

    CData A;
    CData B;

    const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, std::numeric_limits<CData>::max());
    const auto random_cdata = [&]() -> CData { return static_cast<CData>(dist(rng)); };

    auto alu_input_state = std::vector<AluInputState>{
        {.opcode = 0b01101, .test_title = "Positive odd number bitwise and 00h", .reg_a = 0x7D, .reg_b = 0x00, .expected_value_func = [&]() -> CData { return 0b000'01000; }},
        {.opcode = 0b01101, .test_title = "Positive even number bitwise and 00h", .reg_a = 0x7E, .reg_b = 0x00, .expected_value_func = [&]() -> CData { return 0b000'01000; }},
        {.opcode = 0b01101, .test_title = "Positive odd number bitwise and FFh", .reg_a = 0x7D, .reg_b = 0xFF, .expected_value_func = [&]() -> CData { return 0b000'00110; }},
        {.opcode = 0b01101, .test_title = "Positive even number bitwise and FFh", .reg_a = 0x7E, .reg_b = 0xFF, .expected_value_func = [&]() -> CData { return 0b000'00100; }},
        {.opcode = 0b01101, .test_title = "Negative odd number bitwise and 00h", .reg_a = 0x8D, .reg_b = 0x00, .expected_value_func = [&]() -> CData { return 0b000'01000; }},
        {.opcode = 0b01101, .test_title = "Negative even number bitwise and 00h", .reg_a = 0x8E, .reg_b = 0x00, .expected_value_func = [&]() -> CData { return 0b000'01000; }},
        {.opcode = 0b01101, .test_title = "Negative odd number bitwise and FFh", .reg_a = 0x8D, .reg_b = 0xFF, .expected_value_func = [&]() -> CData { return 0b000'00111; }},
        {.opcode = 0b01101, .test_title = "Negative even number bitwise and FFh", .reg_a = 0x8E, .reg_b = 0xFF, .expected_value_func = [&]() -> CData { return 0b000'00101; }},
        {.opcode = 0b00111, .test_title = "Overflow", .reg_a = 0x55, .reg_b = 0x6A, .expected_value_func = [&]() -> CData { return 0b000'10111; }},
    };

    DOCTEST_VALUE_PARAMETERIZED_DATA(data, alu_input_state);

    alu = new Valu();
    alu->eval();
    alu->reg_f_load = 0;
    alu->reg_f_out = 1;
    alu->alu_out = 1;
    alu->opcode = data.opcode;

    A = data.reg_a;
    B = data.reg_b;

    const auto expected = data.expected_value_func();

    INFO(data.test_title, ", opcode: ", std::bitset<8>(data.opcode), ", A: ", A, ", B: ", B,
            ", expected: ", expected);

    alu->reg_a = A;
    alu->reg_b = B;
    alu->reg_f_load = 1;
    alu->reg_f_out = 0;
    alu->eval();

    CHECK(alu->data_out == expected);

    delete alu;
}
