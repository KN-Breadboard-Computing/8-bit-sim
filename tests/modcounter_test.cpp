#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Vmodcounter_test_wrapper.h"

TEST_CASE("Modcounter") {
    Vmodcounter_test_wrapper modcounter;

    for(int i = 0; i < 10000; i++) {
        CHECK(modcounter.data_out == i % 800);

        modcounter.clk = 0;
        modcounter.eval();
        modcounter.clk = 1;
        modcounter.eval();
    }
}
