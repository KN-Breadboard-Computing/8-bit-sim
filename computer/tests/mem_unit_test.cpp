#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Vmem_unit.h"
#include "verilated.h"
#include <iostream>

enum ReadFlags {
    READ_ZP = 1, // zero page
    READ_HP = 2, // high part
};

enum WriteFlags {
    WRITE_ZP = 1, // zero page
    WRITE_HP = 2, // high part
};

void reset(Vmem_unit& mem) {
    mem.zero_page = 1;
    mem.mem_part = 0;
    mem.mem_out = 1;
    mem.mem_in = 1;
    mem.reg_mbr_load = 0;
    mem.reg_mar_load = 0;
    mem.reg_mbr_word_dir = 1;
    mem.data_in_en = 0;
    //mem.reg_mbr_use_bus = 1;
}

void load_mar_mbr(Vmem_unit& mem, int addr, int data) {
    mem.reg_mar_load = 1;
    mem.reg_mbr_load = 1;
    mem.mem_out = 1;
    mem.mem_in = 1;
    mem.mem_part = 0;
    mem.zero_page = 1;
    mem.reg_mbr_word_dir = 1;
    mem.data_in = data;
    mem.data_in_en = 1;
    mem.address = addr;
    //mem.reg_mbr_use_bus = 0;
}

void load_mbr_to_mem(Vmem_unit& mem, int flags = 0) {
    mem.reg_mar_load = 0;
    mem.reg_mbr_load = 0;
    mem.mem_out = 1;
    mem.mem_in = 0;
    mem.mem_part = (flags & READ_HP) ? 1 : 0;
    mem.zero_page = (flags & READ_ZP) ? 0 : 1;
    mem.reg_mbr_word_dir = 1;
    mem.data_in_en = 0;
    //mem.reg_mbr_use_bus = 1;
}

void load_mem(Vmem_unit& mem, int flags = 0) {
    mem.reg_mar_load = 0;
    mem.reg_mbr_load = 0;
    mem.mem_out = 0;
    mem.mem_in = 1;
    mem.mem_part = (flags & READ_HP) ? 1 : 0;
    mem.zero_page = (flags & READ_ZP) ? 0 : 1;
    mem.reg_mbr_word_dir = 0;
    mem.data_in_en = 0;
    //mem.reg_mbr_use_bus = 0;
}

void load_mar(Vmem_unit& mem, int addr) {
    mem.reg_mar_load = 1;
    mem.reg_mbr_load = 0;
    mem.mem_out = 1;
    mem.mem_in = 1;
    mem.mem_part = 0;
    mem.zero_page = 1;
    mem.reg_mbr_word_dir = 1;
    mem.address = addr;
    mem.data_in_en = 0;
    //mem.reg_mbr_use_bus = 0;
}

TEST_CASE("Read/write from/to first byte works") {
    Vmem_unit mem;

    reset(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0x00);

    load_mar_mbr(mem, 0x0000, 0xAD);
    mem.eval();

    load_mbr_to_mem(mem);
    mem.eval();

    load_mar_mbr(mem, 0xFFFF, 0xB0);
    mem.eval();

    // generate an `reg_mar_load` edge
    mem.reg_mar_load = 0;
    mem.eval();

    load_mar(mem, 0x0000);
    mem.eval();

    load_mem(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0xAD);
}

TEST_CASE("Read/write from/to last byte works") {
    Vmem_unit mem;

    reset(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0x00);

    load_mar_mbr(mem, 0xFFFF, 0xAD);
    mem.eval();

    load_mbr_to_mem(mem);
    mem.eval();

    load_mar_mbr(mem, 0x0000, 0xB0);
    mem.eval();

    // generate an `reg_mar_load` edge
    mem.reg_mar_load = 0;
    mem.eval();

    load_mar(mem, 0xFFFF);
    mem.eval();

    load_mem(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0xAD);
}

TEST_CASE("Read/write works") {
    Vmem_unit mem;

    reset(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0x00);

    load_mar_mbr(mem, 0xA0A0, 0xAD);
    mem.eval();

    load_mbr_to_mem(mem);
    mem.eval();

    load_mar_mbr(mem, 0xB1B1, 0xBC);
    mem.eval();

    load_mbr_to_mem(mem);
    mem.eval();

    load_mar(mem, 0xA0A0);
    mem.eval();

    load_mem(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0xAD);

    load_mar(mem, 0xB1B1);
    mem.eval();

    load_mem(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0xBC);
}

TEST_CASE("Bulk reads/writes work") {
    const size_t LEN = 256;
    const size_t STEP = (1 << 16) / LEN;
    int data[LEN];
    for (size_t i = 0; i < LEN; i++) {
        data[i] = 0xDE;
    }

    Vmem_unit mem;

    reset(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0x00);

    for (size_t i = 0; i < LEN; i++) {
        load_mar_mbr(mem, (i * STEP), data[i]);
        mem.eval();

        load_mbr_to_mem(mem);
        mem.eval();
    } 
    
    for (size_t i = 0; i < LEN; i++) {
        int expected = data[i];
        load_mar_mbr(mem, (i * STEP), expected ^ 0xC0);
        mem.eval();

        load_mem(mem);
        mem.eval();

        CHECK_MESSAGE(mem.data_out == expected, "Mismatch at address ", i * STEP);
    }
}

TEST_CASE("Read/write first byte from/to zero page works") {
    Vmem_unit mem;

    reset(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0x00);

    load_mar_mbr(mem, 0xAA00, 0xAD);
    mem.eval();

    load_mbr_to_mem(mem, WRITE_ZP);
    mem.eval();

    load_mar_mbr(mem, 0xFFFF, 0xB0);
    mem.eval();

    // generate an `reg_mar_load` edge
    mem.reg_mar_load = 0;
    mem.eval();

    load_mar(mem, 0xBB00);
    mem.eval();

    load_mem(mem, READ_ZP);
    mem.eval();
    CHECK_EQ(mem.data_out, 0xAD);
}

TEST_CASE("Read/write last byte from/to zero page works") {
    Vmem_unit mem;

    reset(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0x00);

    load_mar_mbr(mem, 0xAAFF, 0xAD);
    mem.eval();

    load_mbr_to_mem(mem, WRITE_ZP);
    mem.eval();

    load_mar_mbr(mem, 0x0000, 0xB0);
    mem.eval();

    // generate an `reg_mar_load` edge
    mem.reg_mar_load = 0;
    mem.eval();

    load_mar(mem, 0xBBFF);
    mem.eval();

    load_mem(mem, READ_ZP);
    mem.eval();
    CHECK_EQ(mem.data_out, 0xAD);
}

TEST_CASE("Bulk read/write from/to zero page works") {
    const size_t LEN = 256;
    const size_t STEP = (1 << 8) / LEN;
    int data[LEN];
    for (size_t i = 0; i < LEN; i++) {
        data[i] = 0xDE ^ i;
    }

    Vmem_unit mem;

    reset(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0x00);

    for (size_t i = 0; i < LEN; i++) {
        load_mar_mbr(mem, 0xFF00 + (i * STEP), data[i]);
        mem.eval();

        load_mbr_to_mem(mem, WRITE_ZP);
        mem.eval();
    } 
    
    for (size_t i = 0; i < LEN; i++) {
        int expected = data[i];
        load_mar_mbr(mem, 0xEF00 + (i * STEP), expected ^ 0xC0);
        mem.eval();

        load_mem(mem, READ_ZP);
        mem.eval();

        CHECK_MESSAGE(mem.data_out == expected, "Mismatch at address ", i * STEP);
    }
}

TEST_CASE("Read/write first byte of high memory works") {
    Vmem_unit mem;

    reset(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0x00);

    load_mar_mbr(mem, 0x0000, 0xAD);
    mem.eval();

    load_mbr_to_mem(mem, WRITE_HP);
    mem.eval();

    load_mar_mbr(mem, 0xFFFF, 0xB0);
    mem.eval();

    // generate an `reg_mar_load` edge
    mem.reg_mar_load = 0;
    mem.eval();

    load_mar(mem, 0x0000);
    mem.eval();

    load_mem(mem, READ_HP);
    mem.eval();
    CHECK_EQ(mem.data_out, 0xAD);
}

TEST_CASE("Read/write last byte of high memory works") {
    Vmem_unit mem;

    reset(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0x00);

    load_mar_mbr(mem, 0xFFFF, 0xAD);
    mem.eval();

    load_mbr_to_mem(mem, WRITE_HP);
    mem.eval();

    load_mar_mbr(mem, 0x0000, 0xB0);
    mem.eval();

    // generate an `reg_mar_load` edge
    mem.reg_mar_load = 0;
    mem.eval();

    load_mar(mem, 0xFFFF);
    mem.eval();

    load_mem(mem, READ_HP);
    mem.eval();
    CHECK_EQ(mem.data_out, 0xAD);
}

TEST_CASE("Read/write from/to high memory works") {
    Vmem_unit mem;

    reset(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0x00);

    load_mar_mbr(mem, 0xA0A0, 0xAD);
    mem.eval();

    load_mbr_to_mem(mem, WRITE_HP);
    mem.eval();

    load_mar_mbr(mem, 0xB1B1, 0xBC);
    mem.eval();

    load_mbr_to_mem(mem, WRITE_HP);
    mem.eval();

    load_mar(mem, 0xA0A0);
    mem.eval();

    load_mem(mem, READ_HP);
    mem.eval();
    CHECK_EQ(mem.data_out, 0xAD);

    load_mar(mem, 0xB1B1);
    mem.eval();

    load_mem(mem, READ_HP);
    mem.eval();
    CHECK_EQ(mem.data_out, 0xBC);
}

TEST_CASE("Bulk high memory reads/writes work") {
    const size_t LEN = 256;
    const size_t STEP = (1 << 16) / LEN;
    int data[LEN];
    for (size_t i = 0; i < LEN; i++) {
        data[i] = 0xAD ^ i;
    }

    Vmem_unit mem;

    reset(mem);
    mem.eval();
    CHECK_EQ(mem.data_out, 0x00);

    for (size_t i = 0; i < LEN; i++) {
        load_mar_mbr(mem, (i * STEP), data[i]);
        mem.eval();

        load_mbr_to_mem(mem, WRITE_HP);
        mem.eval();
    } 
    
    for (size_t i = 0; i < LEN; i++) {
        int expected = data[i];
        load_mar_mbr(mem, (i * STEP), expected ^ 0xC0);
        mem.eval();

        load_mem(mem, READ_HP);
        mem.eval();

        CHECK_MESSAGE(mem.data_out == expected, "Mismatch at address ", i * STEP);
    }
}
