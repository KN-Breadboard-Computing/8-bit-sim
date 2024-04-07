module cpu (
    input wire clk,
    input wire rst, // active low
    output wire zero_page,
    output wire mem_part,
    output wire mem_out,
    output wire mem_in,
    output wire reg_mbr_load,
    output wire reg_mbr_word_dir,
    output wire reg_mar_load,
    output wire [15:0] addr_bus,
    output wire [4:0] int_bus,
    inout wire [7:0] data_bus
);
    // TODO przejrzeć specyfikacje naszych fizycznych układów i ustalić gdzie jaki jest wymagany stan wejścia - szczególnie dla sygnałów

    `include "include/signals.v"

    /* verilator lint_off UNOPTFLAT */
    wire [7:0] bus;
    /* verilator lint_on UNOPTFLAT */
    wire [7:0] flags;
    wire [15:0] addr;

    // SIGNAL REGISTER
    /* verilator lint_off UNUSEDSIGNAL */
    wire [46:0] signals;
    /* verilator lint_on UNUSEDSIGNAL */

    control_unit ctrl_unit(
        .clk(clk),
        .not_clk(~clk),
        .rst(rst),
        .reg_ir_load_override(1'b0),
        .mcc_rst_override(1'b1),
        .data(bus),
        .flags(flags),
        .signals(signals)
    );

    // PROGRAM COUNTER
    wire [15:0] pc_out;
    counter #(.width(16)) pc(
        .clk(signals[`PC_TICK]),
        .write(~signals[`PC_LOAD]),
        .reset(~signals[`PC_RST]),
        .countdown(1'b0),
        .in(addr),
        .out(pc_out)
    );
    tristate_buffer #(.width(16)) pc_to_bus(
        .data_in(pc_out),
        .enable(~signals[`PC_OUT]),
        .data_out(addr)
    );

    // STACK COUNTER
    wire [15:0] stc_out;
    counter #(.width(16)) stc (
        .clk(signals[`STC_TICK]),
        .write(~signals[`STC_LOAD]),
        .reset(signals[`STC_RST]),
        .countdown(signals[`STC_MODE]),
        .in(addr_bus),
        .out(stc_out)
    );
    tristate_buffer #(.width(16)) stc_to_bus (
        .data_in(stc_out),
        .enable(~signals[`STC_OUT]),
        .data_out(addr_bus)
    );

    // ALU 
    wire [7:0] a_out;
    wire [7:0] b_out;

    register a_reg (
        .clk(clk),
        .enable(signals[`REG_A_LOAD]),
        .data_in(bus),
        .data_out(a_out)
    );
    register b_reg (
        .clk(clk),
        .enable(signals[`REG_B_LOAD]),
        .data_in(bus),
        .data_out(b_out)
    );

    alu alu_unit (
        .alu_out(signals[`ALU_OUT]),
        .reg_f_out(signals[`REG_F_OUT]),
        .reg_f_load(signals[`REG_F_LOAD]),
        .opcode(signals[`ALU_OPC_4:`ALU_OPC_0]),
        .reg_a(a_out),
        .reg_b(b_out),
        .data_out(bus),
        .flags_out(flags)
    );

    // TEMPORARY REGISTER
    tmp tmp(
        .reg_tmph_data_dir(signals[`REG_TMPH_DATA_DIR]),
        .reg_tmph_pass_data(signals[`REG_TMPH_PASS_DATA]),
        .reg_tmph_load(signals[`REG_TMPH_LOAD]),
        .reg_tmph_out(signals[`REG_TMPH_OUT]),
        .reg_tmpl_data_dir(signals[`REG_TMPL_DATA_DIR]),
        .reg_tmpl_pass_data(signals[`REG_TMPL_PASS_DATA]),
        .reg_tmpl_load(signals[`REG_TMPL_LOAD]),
        .reg_tmpl_out(signals[`REG_TMPL_OUT]),
        .reg_tmp_pass_address(signals[`REG_TMP_PASS_ADDRESS]),
        .reg_tmp_address_dir(signals[`REG_TMP_ADDRESS_DIR]),
        .data(bus),
        .address(addr)
    );

    wire cpu_out;
    assign cpu_out = reg_mbr_word_dir & mem_out;

    assign data_bus = cpu_out ? bus : 8'hZ;
    assign bus = ~cpu_out ? data_bus : 8'hZ;
    assign addr_bus = addr;
    
    assign zero_page = signals[`ZERO_PAGE];
    assign mem_part = signals[`MEM_PART];
    assign mem_out = signals[`MEM_OUT];
    assign mem_in = signals[`MEM_IN];
    assign reg_mbr_load = signals[`REG_MBR_LOAD];
    assign reg_mbr_word_dir = signals[`REG_MBR_WORD_DIR];
    assign reg_mar_load = signals[`REG_MAR_LOAD];
    assign int_bus = signals[`INT4:`INT0];

endmodule
