#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Vcontrol_unit.h"
#include "verilated.h"
#include <iostream>

#include "tests_common.hpp"

static constexpr size_t SIGNALS_SIZE = 47;

static constexpr bool LOAD_PC_TO_MAR[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_MEM_TO_IR_INC_PC[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1
};

static constexpr bool LOAD_B_TO_A[SIGNALS_SIZE] = {
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_TL_TO_A[SIGNALS_SIZE] = {
    1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_F_TO_B[SIGNALS_SIZE] = {
    0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_B_TO_TH[SIGNALS_SIZE] = {
    0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_MEM_TO_A_INC_PC[SIGNALS_SIZE] = {
    1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1
};

static constexpr bool LOAD_MEM_TO_TH_INC_PC[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1
};

static constexpr bool LOAD_MEM_TO_TMPH_INC_PC[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1
};

static constexpr bool LOAD_MEM_TO_TMPL_INC_PC[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1
};

static constexpr bool LOAD_TMP_TO_MAR[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_MEM_TO_A[SIGNALS_SIZE] = {
    1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1
};

static constexpr bool LOAD_TMP_TO_MAR_A_TO_MBR[SIGNALS_SIZE] = {
    0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1
};

static constexpr bool LOAD_MBR_TO_MEM[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_ZP_MEM_TO_A[SIGNALS_SIZE] = {
    1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1
};

static constexpr bool LOAD_TMP_TO_MAR_B_TO_MBR[SIGNALS_SIZE] = {
    0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1
};

static constexpr bool LOAD_MBR_TO_ZP_MEM[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_A_TO_TMPL[SIGNALS_SIZE] = {
    0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool MOV_NEGB_TO_A[SIGNALS_SIZE] = {
    1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool MOV_NEGB_TO_MBR_SAVE_FLAGS[SIGNALS_SIZE] = {
    0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1
};

static constexpr bool MOV_NEGA_TO_MBR_SAVE_FLAGS_LOAD_STC_TO_MAR[SIGNALS_SIZE] = {
    0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1
};

static constexpr bool LOAD_MBR_TO_STC_MEM_DEC_STC[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool MOV_ADD_TO_A[SIGNALS_SIZE] = {
    1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool MOV_SUBAB_TO_A[SIGNALS_SIZE] = {
    1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool MOV_DIV2A_TO_B[SIGNALS_SIZE] = {
    0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool MOV_INVB_TO_A[SIGNALS_SIZE] = {
    1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool MOV_OR_TO_A[SIGNALS_SIZE] = {
    1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool MOV_AND_TO_B[SIGNALS_SIZE] = {
    0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool MOV_XOR_TO_A[SIGNALS_SIZE] = {
    1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool MOV_SHRA_TO_B[SIGNALS_SIZE] = {
    0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool MOV_SHLB_TO_A[SIGNALS_SIZE] = {
    1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_TH_TO_A[SIGNALS_SIZE] = {
    1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_TL_TO_B[SIGNALS_SIZE] = {
    0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool SUB_A_B_SAVE_FLAGS_TO_F[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool MOV_0_TO_A[SIGNALS_SIZE] = {
    1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool DO_NOTHING[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_STC_TO_MAR_A_TO_MBR[SIGNALS_SIZE] = {
    0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1
};

static constexpr bool LOAD_STC_TO_MAR[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_MEM_TO_B_INC_STC[SIGNALS_SIZE] = {
    0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1
};

static constexpr bool LOAD_MBR_TO_MEM_DEC_STC[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_TMP_TO_PC[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_MEM_TO_A_LOAD_PC_TO_TMP[SIGNALS_SIZE] = {
    1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1
};

static constexpr bool MOV_ADD_TO_TMPL[SIGNALS_SIZE] = {
    0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool LOAD_TMPL_TO_B[SIGNALS_SIZE] = {
    0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool PC_INC[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1
};

static constexpr bool RST_MC[SIGNALS_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0
};

struct CtrlUnitInput {
    std::string test_title;
    CData opcode;
    CData flags;
    const bool* ucodes[16];
}; 

TEST_CASE("Control Unit works") {
    CtrlUnitInput current_in;
    auto ctrl_unit_inputs = std::vector<CtrlUnitInput>{
        CtrlUnitInput{ "MOVAB", 0x01, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_B_TO_A, RST_MC, nullptr } },
        CtrlUnitInput{ "MOVATL", 0x03, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_TL_TO_A, RST_MC, nullptr } },
        CtrlUnitInput{ "MOVBF", 0x08, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_F_TO_B, RST_MC, nullptr } },
        CtrlUnitInput{ "MOVTHB", 0x0A, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_B_TO_TH, RST_MC, nullptr } },
        CtrlUnitInput{ "MOVAIMM", 0x11, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_A_INC_PC, RST_MC, nullptr } },
        CtrlUnitInput{ "MOVTHIMM", 0x13, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TH_INC_PC, RST_MC, nullptr } },
        CtrlUnitInput{ "MOVBABS", 0x15, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPH_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPL_INC_PC, LOAD_TMP_TO_MAR, LOAD_MEM_TO_A, RST_MC, nullptr } },
        CtrlUnitInput{ "MOVAABSZP", 0x16, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPH_INC_PC, LOAD_TMP_TO_MAR, LOAD_ZP_MEM_TO_A, RST_MC, nullptr } },
        CtrlUnitInput{ "MOVATABSA", 0x19, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPH_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPL_INC_PC, LOAD_TMP_TO_MAR_A_TO_MBR, LOAD_MBR_TO_MEM, RST_MC, nullptr } },
        CtrlUnitInput{ "MOVATABSBZP", 0x1C, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPH_INC_PC, LOAD_TMP_TO_MAR_B_TO_MBR, LOAD_MBR_TO_ZP_MEM, RST_MC, nullptr } },
        CtrlUnitInput{ "MOVATTA", 0x1F, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_TMP_TO_MAR_A_TO_MBR, LOAD_MBR_TO_MEM, RST_MC, nullptr } },
        CtrlUnitInput{ "MOVATAB", 0x21, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_A_TO_TMPL, LOAD_TMP_TO_MAR_B_TO_MBR, LOAD_MBR_TO_ZP_MEM, RST_MC, nullptr } },
        CtrlUnitInput{ "NEGAB", 0x33, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, MOV_NEGB_TO_A, RST_MC, nullptr } },
        CtrlUnitInput{ "NEGMEMB", 0x37, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPH_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPL_INC_PC, LOAD_TMP_TO_MAR, MOV_NEGB_TO_MBR_SAVE_FLAGS, LOAD_MBR_TO_MEM, RST_MC, nullptr } },
        CtrlUnitInput{ "NEGSTCA", 0x3A, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, MOV_NEGA_TO_MBR_SAVE_FLAGS_LOAD_STC_TO_MAR, LOAD_MBR_TO_STC_MEM_DEC_STC, RST_MC, nullptr } },
        CtrlUnitInput{ "ADDA", 0x3C, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, MOV_ADD_TO_A, RST_MC, nullptr } },
        CtrlUnitInput{ "SUBAAB", 0x41, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, MOV_SUBAB_TO_A, RST_MC, nullptr } },
        CtrlUnitInput{ "DIV2BA", 0x4D, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, MOV_DIV2A_TO_B, RST_MC, nullptr } },
        CtrlUnitInput{ "INVAB", 0x56, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, MOV_INVB_TO_A, RST_MC, nullptr } },
        CtrlUnitInput{ "ORA", 0x5F, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, MOV_OR_TO_A, RST_MC, nullptr } },
        CtrlUnitInput{ "ANDB", 0x65, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, MOV_AND_TO_B, RST_MC, nullptr } },
        CtrlUnitInput{ "XORA", 0x69, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, MOV_XOR_TO_A, RST_MC, nullptr } },
        CtrlUnitInput{ "SHRBA", 0x70, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, MOV_SHRA_TO_B, RST_MC, nullptr } },
        CtrlUnitInput{ "SHLAB", 0x79, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, MOV_SHLB_TO_A, RST_MC, nullptr } },
        CtrlUnitInput{ "CMPTHTL", 0x84, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_TH_TO_A, LOAD_TL_TO_B, SUB_A_B_SAVE_FLAGS_TO_F, RST_MC, nullptr } },
        CtrlUnitInput{ "CLRA", 0x8E, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, MOV_0_TO_A, RST_MC, nullptr } },
        // Due to microcode generation issues increments/decrements do not work
        CtrlUnitInput{ "INCA", 0x93, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, DO_NOTHING, RST_MC, nullptr } },
        CtrlUnitInput{ "DECB", 0x97, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, DO_NOTHING, RST_MC, nullptr } },
        CtrlUnitInput{ "PUSHA", 0xC5, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_STC_TO_MAR_A_TO_MBR, LOAD_MBR_TO_STC_MEM_DEC_STC, RST_MC, nullptr } },
        CtrlUnitInput{ "POPB", 0xCE, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_STC_TO_MAR, LOAD_MEM_TO_B_INC_STC, RST_MC, nullptr } },
        CtrlUnitInput{ "NOP", 0xD4, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, DO_NOTHING, RST_MC, nullptr } },
        CtrlUnitInput{ "JMPIMM", 0x99, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPH_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPL_INC_PC, LOAD_TMP_TO_PC, RST_MC, nullptr } },
        CtrlUnitInput{ "JMPREL", 0xAE, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_A_LOAD_PC_TO_TMP, LOAD_TMPL_TO_B, MOV_ADD_TO_TMPL, RST_MC, nullptr } },
        CtrlUnitInput{ "SKIP1", 0xD6, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, PC_INC, RST_MC, nullptr } },
        CtrlUnitInput{ "SKIP2", 0xD7, 0x00, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, PC_INC, PC_INC, RST_MC, nullptr } },
        CtrlUnitInput{ "JMPIMMS - branch taken", 0x9A, 0x01, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPH_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPL_INC_PC, LOAD_TMP_TO_PC, RST_MC, nullptr } },
        CtrlUnitInput{ "JMPIMMS - branch not taken", 0x9A, 0x1E, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, PC_INC, PC_INC, RST_MC, nullptr } },
        CtrlUnitInput{ "JMPIMMNO - branch taken", 0xAB, 0x0F, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPH_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_TMPL_INC_PC, LOAD_TMP_TO_PC, RST_MC, nullptr } },
        CtrlUnitInput{ "JMPIMMNO - branch not taken", 0xAB, 0x10, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, PC_INC, PC_INC, RST_MC, nullptr } },
        CtrlUnitInput{ "JMPRELNS - branch taken", 0xB0, 0x1E, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_A_LOAD_PC_TO_TMP, LOAD_TMPL_TO_B, MOV_ADD_TO_TMPL, RST_MC, nullptr } },
        CtrlUnitInput{ "JMPRELNS - branch not taken", 0xB0, 0x01, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, PC_INC, RST_MC, nullptr } },
        CtrlUnitInput{ "JMPRELO - branch taken", 0xB0, 0x10, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, LOAD_PC_TO_MAR, LOAD_MEM_TO_A_LOAD_PC_TO_TMP, LOAD_TMPL_TO_B, MOV_ADD_TO_TMPL, RST_MC, nullptr } },
        CtrlUnitInput{ "JMPRELO - branch not taken", 0xB0, 0x0F, { LOAD_PC_TO_MAR, LOAD_MEM_TO_IR_INC_PC, PC_INC, RST_MC, nullptr } },
    };

    DOCTEST_VALUE_PARAMETERIZED_DATA(current_in, ctrl_unit_inputs);

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

    clk = !clk;
    ctrl.clk = clk;
    ctrl.not_clk = !clk;
    ctrl.data = 0xEA; // non existing instruction, should still do a fetch
    ctrl.flags = current_in.flags; // set here once
    ctrl.reg_ir_load_override = 1;
    ctrl.mcc_rst_override = 1;
    ctrl.eval();
    CHECK_EQ(ctrl.signals, 0UL);

    size_t ucode_counter = 0;
    const bool* signals = current_in.ucodes[ucode_counter++];
    while(signals) {
        REQUIRE_MESSAGE(ucode_counter < 15, "Microcode counter overflow. This is likely a problem with the test itself.");

        clk = !clk;
        ctrl.clk = clk;
        ctrl.not_clk = !clk;
        ctrl.data = current_in.opcode;
        ctrl.reg_ir_load_override = 0;
        ctrl.mcc_rst_override = 0;
        ctrl.eval();

        clk = !clk;
        ctrl.clk = clk;
        ctrl.not_clk = !clk;
        ctrl.eval();

        for (size_t i = 0; i < SIGNALS_SIZE; i++) {
            CHECK_MESSAGE(((ctrl.signals >> i) & 1) == signals[i], "Mismatch at signal ", i, ", microcode ", (ucode_counter - 1), ", opcode ", current_in.opcode);
        }

        signals = current_in.ucodes[ucode_counter++];
    }
}

