#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Vram.h"
#include "verilated.h"
#include <iostream>

// TODO: add a test case for chaning address and data with write_enable set to assert that it does not do the write
TEST_CASE("Changing address while not writing has no effect") {
    Vram ram;

    // Enable chip
    ram.chip_enable = 0;
    ram.chip_enable2 = 1;
    ram.write_enable = 1;
    ram.output_enable = 1;
    ram.data_in_en = 0;
    ram.eval();

    CHECK_EQ(ram.data_out, 0x00);

    ram.write_enable = 0;
    ram.data_in_en = 1;
    ram.data_in = 0xAA;
    ram.address = 0x02020;
    ram.eval();

    ram.data_in = 0xBB;
    ram.address = 0x03030;
    ram.eval();

    ram.write_enable = 1;
    ram.eval();

    ram.data_in = 0xCD;
    ram.address = 0x02020;
    ram.eval();

    ram.write_enable = 1;
    ram.output_enable = 0;
    ram.address = 0x02020;
    ram.data_in_en = 0;
    ram.eval();
    CHECK_EQ(ram.data_out, 0xAA);

    ram.address = 0x03030;
    ram.eval();
    CHECK_EQ(ram.data_out, 0xBB);
}

TEST_CASE("Read/write to/from first byte works") {
    Vram ram;

    // Reset
    ram.chip_enable = 1;
    ram.chip_enable2 = 1;
    ram.write_enable = 1;
    ram.data_in_en = 0;
    ram.eval();

    CHECK_EQ(ram.data_out, 0x00);

    ram.chip_enable = 0;
    ram.write_enable = 0;
    ram.data_in_en = 1;
    ram.data_in = 0xEE;
    ram.address = 0x00000;
    ram.eval();

    ram.write_enable = 1;
    ram.output_enable = 1;
    ram.data_in_en = 0;
    ram.eval();
    CHECK_EQ(ram.data_out, 0x00);

    ram.output_enable = 0;
    ram.address = 0x00000;
    ram.eval();
    CHECK_EQ(ram.data_out, 0xEE);
}

TEST_CASE("Setting address then writing/reading works") {
    Vram ram;

    // Reset
    ram.chip_enable = 0;
    ram.chip_enable2 = 1;
    ram.write_enable = 1;
    ram.data_in_en = 0;
    ram.eval();

    CHECK_EQ(ram.data_out, 0x00);

    ram.address = 0x00000;
    ram.eval();

    ram.write_enable = 0;
    ram.data_in_en = 1;
    ram.data_in = 0xEE;
    ram.eval();

    ram.write_enable = 1;
    ram.output_enable = 1;
    ram.data_in_en = 0;
    ram.address = 0x1BEEF;
    ram.eval();
    CHECK_EQ(ram.data_out, 0x00);

    ram.output_enable = 1;
    ram.address = 0x00000;
    ram.eval();
    CHECK_EQ(ram.data_out, 0x00);

    ram.output_enable = 0;
    ram.eval();
    CHECK_EQ(ram.data_out, 0xEE);
}

TEST_CASE("Read/write to/from last byte works") {
    Vram ram;

    // Reset
    ram.chip_enable = 1;
    ram.chip_enable2 = 1;
    ram.write_enable = 1;
    ram.data_in_en = 0;
    ram.eval();

    CHECK_EQ(ram.data_out, 0x00);

    ram.chip_enable = 0;
    ram.write_enable = 0;
    ram.data_in_en = 1;
    ram.data_in = 0xEE;
    ram.address = 0x1FFFF;
    ram.eval();

    ram.write_enable = 1;
    ram.output_enable = 1;
    ram.data_in_en = 0;
    ram.eval();
    CHECK_EQ(ram.data_out, 0x00);

    ram.output_enable = 0;
    ram.address = 0x1FFFF;
    ram.eval();
    CHECK_EQ(ram.data_out, 0xEE);
}

TEST_CASE("Address controlled read/write work") {
    Vram ram;

    // Enable chip
    ram.chip_enable = 0;
    ram.chip_enable2 = 1;
    ram.write_enable = 1;
    ram.output_enable = 1;
    ram.data_in_en = 0;
    ram.eval();

    CHECK_EQ(ram.data_out, 0x00);

    ram.write_enable = 0;
    ram.data_in_en = 1;
    ram.data_in = 0xAA;
    ram.address = 0x02020;
    ram.eval();

    ram.data_in = 0xBB;
    ram.address = 0x03030;
    ram.eval();

    ram.write_enable = 1;
    ram.output_enable = 0;
    ram.data_in_en = 0;
    ram.address = 0x02020;
    ram.eval();

    CHECK_EQ(ram.data_out, 0xAA);

    ram.address = 0x03030;
    ram.eval();

    CHECK_EQ(ram.data_out, 0xBB);
}

TEST_CASE("Address controlled read/write work") {
    Vram ram;

    // Enable chip
    ram.chip_enable = 0;
    ram.chip_enable2 = 1;
    ram.write_enable = 1;
    ram.output_enable = 1;
    ram.data_in_en = 0;
    ram.eval();

    CHECK_EQ(ram.data_out, 0x00);

    ram.write_enable = 0;
    ram.data_in_en = 1;
    ram.data_in = 0xAA;
    ram.address = 0x02020;
    ram.eval();

    ram.data_in = 0xBB;
    ram.address = 0x03030;
    ram.eval();

    ram.write_enable = 1;
    ram.output_enable = 0;
    ram.data_in_en = 0;
    ram.address = 0x02020;
    ram.eval();

    CHECK_EQ(ram.data_out, 0xAA);

    ram.address = 0x03030;
    ram.eval();

    CHECK_EQ(ram.data_out, 0xBB);
}

TEST_CASE("Bulk read/write works") {
    const size_t LEN = 256;
    const size_t STEP = (1 << 17) / 256;
    int data[LEN];
    for (size_t i = 0; i < LEN; i++) { data[i] = i; }

    Vram ram;
    ram.chip_enable = 0;
    ram.chip_enable2 = 1;
    ram.write_enable = 1;
    ram.output_enable = 1;
    ram.data_in_en = 0;
    ram.eval();

    ram.write_enable = 0;
    ram.data_in_en = 1;
    for (size_t i = 0; i < LEN; i++) {
        ram.address = i * STEP;
        ram.data_in = data[i];
        ram.eval();
    }

    ram.write_enable = 1;
    ram.output_enable = 0;
    ram.data_in_en = 0;

    for (size_t i = 0; i < LEN; i++) {
        ram.address = i * STEP;
        ram.eval();
        CHECK_MESSAGE(ram.data_out == data[i], "Mismatch at address ", (i * STEP));
    }
}
