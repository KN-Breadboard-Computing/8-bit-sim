#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Vcontrol_unit.h"
#include "verilated.h"
#include <iostream>

TEST_CASE("Simple MOV works") {
    bool clk = false;

    Vcontrol_unit ctrl;
    // reset signals to known state
    ctrl.clk = clk;
    ctrl.not_clk = !clk;
    ctrl.reg_ir_load_override = 0;
    ctrl.mcc_rst_override = 0;
    ctrl.data = 0;
    ctrl.eval();
    CHECK_EQ(ctrl.signals, 0UL);
    
    // reset the control unit: force the IR to load and the MCC to reset
    // execute instruction `mov a, b` (any instruction is good here, just to kick start the cycle)
    clk = !clk;
    ctrl.clk = clk;
    ctrl.not_clk = !clk;
    ctrl.data = 0x01; // mov a, b
    ctrl.reg_ir_load_override = 1;
    ctrl.mcc_rst_override = 1;
    ctrl.eval();
    CHECK_EQ(ctrl.signals, 0UL);

    // load the first microcode (LOAD_PC_TO_MAR)
    clk = !clk;
    ctrl.clk = clk;
    ctrl.not_clk = !clk;
    ctrl.data = 0x00;
    ctrl.reg_ir_load_override = 0;
    ctrl.mcc_rst_override = 0;
    ctrl.eval();

    // next risig edge, all signals have the correct values (some are ANDed with clock)
    clk = !clk;
    ctrl.clk = clk;
    ctrl.not_clk = !clk;
    ctrl.eval();
    bool signals0[47] = {
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
    };

    for (size_t i = 0; i < sizeof(signals0); i++) {
        CHECK_MESSAGE(((ctrl.signals >> i) & 1) == signals0[i], "Mismatch at signal ", i);
    }

    // load the next microcode (LOAD_MEM[MAR]_TO_IR_PC++)
    clk = !clk;
    ctrl.clk = clk;
    ctrl.not_clk = !clk;
    ctrl.data = 0x01; // now the real next instruction is fetched, fake the same `mov a, b`
    ctrl.eval();

    // next risig edge, check signals
    clk = !clk;
    ctrl.clk = clk;
    ctrl.not_clk = !clk;
    ctrl.eval();

    bool signals1[47] = {
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1
    };

    for (size_t i = 0; i < sizeof(signals1); i++) {
        CHECK_MESSAGE(((ctrl.signals >> i) & 1) == signals1[i], "Mismatch at signal ", i);
    }

    // load the next microcode (LOAD_B_TO_A)
    clk = !clk;
    ctrl.clk = clk;
    ctrl.not_clk = !clk;
    ctrl.eval();

    // next risig edge, check signals
    clk = !clk;
    ctrl.clk = clk;
    ctrl.not_clk = !clk;
    ctrl.eval();

    bool signals2[47] = {
        1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
    };

    for (size_t i = 0; i < sizeof(signals2); i++) {
        CHECK_MESSAGE(((ctrl.signals >> i) & 1) == signals2[i], "Mismatch at signal ", i);
    }

    // load the next microcode (RST_MC)
    clk = !clk;
    ctrl.clk = clk;
    ctrl.not_clk = !clk;
    ctrl.eval();

    // next risig edge, check signals
    clk = !clk;
    ctrl.clk = clk;
    ctrl.not_clk = !clk;
    ctrl.eval();

    bool signals3[47] = {
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0
    };

    // Verilator is bugged (?) and does not report an error when a file to $fopen does not exist
    // Instead it fills whatever the destination was with zeros (or possibly garbage, or does nothing)
    for (size_t i = 0; i < sizeof(signals3); i++) {
        CHECK_MESSAGE(((ctrl.signals >> i) & 1) == signals3[i], "Mismatch at signal ", i);
    }
}
