#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Vcounter.h"
#include "verilated.h"
#include <vector>
#include <iostream>

TEST_CASE("Counter - set value") {
    Vcounter counter;
    int value = 10;

    counter.clk = 0;
    counter.write = 0;
    counter.reset = 0;
    counter.countdown = 0;
    counter.in = 0;
    counter.eval();

    counter.clk = 1;
    counter.in = value;
    counter.write = 1;
    counter.eval();

    CHECK(counter.out == value);
}

TEST_CASE("Counter - reset") {
    Vcounter counter;
    int value = 10;

    counter.clk = 0;
    counter.write = 0;
    counter.reset = 0;
    counter.countdown = 0;
    counter.in = 0;
    counter.eval();

    counter.clk = !counter.clk;
    counter.in = value;
    counter.write = 1;
    counter.eval();

    CHECK(counter.out == value);

    counter.clk = !counter.clk;
    counter.eval();

    counter.clk = !counter.clk;
    counter.reset = 1;
    counter.eval();

    CHECK(counter.out == 0);
}

TEST_CASE("Counter - count up") {
    SUBCASE("Basic") {
        Vcounter counter;

        counter.clk = 0;
        counter.write = 0;
        counter.reset = 0;
        counter.countdown = 0;
        counter.in = 0;

        for(int i = 0; i < 50; i++) {
            counter.reset = 0;
            counter.clk = !counter.clk;
            counter.eval();

            CHECK(counter.out == (i / 2));
        }
    }

    SUBCASE("Wrap around") {
        Vcounter counter;

        counter.clk = 0;
        counter.write = 0;
        counter.reset = 0;
        counter.countdown = 0;
        counter.in = 0;

        for(int i = 0; i < 1000; i++) {
            counter.reset = 0;
            counter.clk = !counter.clk;
            counter.eval();

            CHECK(counter.out == (i / 2) % 256);
        }
    }
}


TEST_CASE("Counter - count down") {
    SUBCASE("Basic") {
        Vcounter counter;

        int value = 50;

        counter.clk = 0;
        counter.write = 0;
        counter.reset = 0;
        counter.countdown = 0;
        counter.in = 0;
        counter.eval();

        counter.clk = 1;
        counter.in = value;
        counter.write = 1;
        counter.eval();

        for(int i = value; i >= 0; i--) {
            counter.write = 0;
            counter.reset = 0;
            counter.clk = !counter.clk;
            counter.countdown = 1;
            counter.eval();

            CHECK(counter.out == (i / 2) + value / 2);
        }

    }

    SUBCASE("Wrap around") {
        Vcounter counter;

        counter.clk = 0;
        counter.write = 0;
        counter.reset = 0;
        counter.countdown = 0;
        counter.in = 0;

        for(int i = 0; i < 1000; i++) {
            counter.write = 0;
            counter.reset = 0;
            counter.clk = !counter.clk;
            counter.countdown = 1;
            counter.eval();

            CHECK(counter.out == (uint8_t)((-i/2) % 256));
        }
    }
}
