#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Vtmp.h"
#include "verilated.h"

TEST_CASE("TMP write and read work") {
    Vtmp tmp;

    tmp.reg_tmph_data_dir = 1;
    tmp.reg_tmph_pass_data = 1;
    tmp.reg_tmph_load = 0;
    tmp.reg_tmph_out = 1;
    tmp.reg_tmpl_data_dir = 1;
    tmp.reg_tmpl_pass_data = 1;
    tmp.reg_tmpl_load = 0;
    tmp.reg_tmpl_out = 1;
    tmp.reg_tmp_address_dir = 1;
    tmp.reg_tmp_pass_address = 1;
    tmp.data_in_en = 0;
    tmp.address_in_en = 0;
    tmp.eval();

    CHECK_EQ(tmp.data_out, 0);
    CHECK_EQ(tmp.address_out, 0);

    // Load HIGH
    tmp.reg_tmph_data_dir = 1;
    tmp.reg_tmph_pass_data = 0;
    tmp.reg_tmph_load = 1;
    tmp.reg_tmph_out = 1;

    tmp.reg_tmpl_data_dir = 1;
    tmp.reg_tmpl_pass_data = 1;
    tmp.reg_tmpl_load = 0;
    tmp.reg_tmpl_out = 1;

    tmp.reg_tmp_address_dir = 1;
    tmp.reg_tmp_pass_address = 1;
    tmp.data_in_en = 1;
    tmp.data_in = 0xDE;
    tmp.eval();

    // Load LOW
    tmp.reg_tmph_data_dir = 1;
    tmp.reg_tmph_pass_data = 1;
    tmp.reg_tmph_load = 0;
    tmp.reg_tmph_out = 1;

    tmp.reg_tmpl_data_dir = 1;
    tmp.reg_tmpl_pass_data = 0;
    tmp.reg_tmpl_load = 1;
    tmp.reg_tmpl_out = 1;

    tmp.reg_tmp_address_dir = 1;
    tmp.reg_tmp_pass_address = 1;
    tmp.data_in_en = 1;
    tmp.data_in = 0xAD;
    tmp.eval();

    // Check
    tmp.reg_tmph_data_dir = 1;
    tmp.reg_tmph_pass_data = 1;
    tmp.reg_tmph_load = 0;
    tmp.reg_tmph_out = 0;
    tmp.reg_tmpl_data_dir = 1;
    tmp.reg_tmpl_pass_data = 1;
    tmp.reg_tmpl_load = 0;
    tmp.reg_tmpl_out = 0;
    tmp.reg_tmp_address_dir = 0;
    tmp.reg_tmp_pass_address = 0;
    tmp.eval();
    CHECK_EQ(tmp.address_out, 0xDEAD);

    // Check HIGH
    tmp.reg_tmph_data_dir = 0;
    tmp.reg_tmph_pass_data = 0;
    tmp.reg_tmph_load = 0;
    tmp.reg_tmph_out = 0;
    tmp.reg_tmpl_data_dir = 1;
    tmp.reg_tmpl_pass_data = 1;
    tmp.reg_tmpl_load = 0;
    tmp.reg_tmpl_out = 0;
    tmp.reg_tmp_address_dir = 0;
    tmp.reg_tmp_pass_address = 1;
    tmp.data_in_en = 0;
    tmp.eval();
    CHECK_EQ(tmp.address_out, 0x0000);
    CHECK_EQ(tmp.data_out, 0xDE);

    // Check LOW
    tmp.reg_tmph_data_dir = 1;
    tmp.reg_tmph_pass_data = 1;
    tmp.reg_tmph_load = 0;
    tmp.reg_tmph_out = 0;
    tmp.reg_tmpl_data_dir = 0;
    tmp.reg_tmpl_pass_data = 0;
    tmp.reg_tmpl_load = 0;
    tmp.reg_tmpl_out = 0;
    tmp.reg_tmp_address_dir = 0;
    tmp.reg_tmp_pass_address = 1;
    tmp.data_in_en = 0;
    tmp.eval();
    CHECK_EQ(tmp.address_out, 0x0000);
    CHECK_EQ(tmp.data_out, 0xAD);
}
