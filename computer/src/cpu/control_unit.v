`include "include/signals.v"
`include "basics/counter.v"
`include "basics/sr_latch.sv"

module control_unit #(
    parameter ROM_A = `"`ROMS_PATH/A.bin`",
    parameter ROM_B = `"`ROMS_PATH/B.bin`",
    parameter ROM_C = `"`ROMS_PATH/C.bin`",
    parameter ROM_D = `"`ROMS_PATH/D.bin`",
    parameter ROM_E = `"`ROMS_PATH/E.bin`",
    parameter ROM_F = `"`ROMS_PATH/F.bin`",
    parameter ROM_G = `"`ROMS_PATH/G.bin`",
    parameter ROM_INT = `"`ROMS_PATH/INT.bin`",
    parameter ROM_BRANCH = `"`ROMS_PATH/BRANCH.bin`"
)(
    input wire clk,
    input wire not_clk,
    input wire rst, // active low
    input wire reg_ir_load_override, // TODO: figure out how this thing resets
    // verilator lint_off UNUSED
    input wire mcc_rst_override,   // TODO: figure out how this thing resets
    input wire [7:0] data,
    input wire [7:0] flags,
    // verilator lint_off UNUSED
    input wire [4:0] int_in,
    output wire [4:0] irq_no,
    output wire [47:0] signals
);
wire reg_ir_load;
wire mcc_tick;
wire mcc_rst;
wire [3:0] mcc_bus;
wire [12:0] raw_inst_bus;
wire [12:0] int_inst_bus;
//wire [12:0] branched_inst_bus;
wire [12:0] inst_bus;
wire [4:0] int_bus;
wire [4:0] latched_int_bus;
wire int_en_sig;

reg [7:0] inst_reg;
reg [7:0] sig_a;
reg [7:0] sig_b;
reg [7:0] sig_c;
reg [7:0] sig_d;
// verilator lint_off UNUSED
reg [7:0] sig_e;
reg [7:0] sig_f;
// verilator lint_on UNUSED
reg [7:0] sig_g;

/*
 * INT_0-4 -> F3-F7
 * SET_INT_ENABLE -> G0
 * RST_INT_ENABLE -> G1
 * INT_ADDRESS_OUT -> G2
 * RST_INT_0-4 -> G3-G7
 *
 * ADDR0 = 0
 * ADDR1 = (~INT3 & ~INT1) | (~INT3 & INT2) | INT4
 * ADDR2 = (~INT4 & ~INT3 & INT1) | (~INT4 & ~INT3 & INT2)
 * ADDR3 = INT3 | INT4
 */

sr_latch int0(
    .set(int_in[0]),
    .rst(sig_g[3] | ~rst),
    .out(latched_int_bus[0])
);
sr_latch int1(
    .set(int_in[1]),
    .rst(sig_g[4] | ~rst),
    .out(latched_int_bus[1])
);
sr_latch int2(
    .set(int_in[2]),
    .rst(sig_g[5] | ~rst),
    .out(latched_int_bus[2])
);
sr_latch int3(
    .set(int_in[3]),
    .rst(sig_g[6] | ~rst),
    .out(latched_int_bus[3])
);
sr_latch int4(
    .set(int_in[4]),
    .rst(sig_g[7] | ~rst),
    .out(latched_int_bus[4])
);
sr_latch int_en(
    .set(sig_g[0] | ~rst),
    .rst(sig_g[1]),
    .out(int_en_sig)
);

reg [7:0] rom_a [(1 << 13)];
reg [7:0] rom_b [(1 << 13)];
reg [7:0] rom_c [(1 << 13)];
reg [7:0] rom_d [(1 << 13)];
reg [7:0] rom_e [(1 << 13)];
reg [7:0] rom_f [(1 << 13)];
reg [7:0] rom_g [(1 << 13)];
reg [7:0] rom_cjmp [(1 << 13)];
reg [7:0] rom_int [(1 << 13)];

counter #( .width(4) ) mcc (
    .clk(mcc_tick | ~mcc_rst),
    .write(1'b0),
    .reset(~mcc_rst), // mcc_rst is active low
    .countdown(1'b0),
    .in(4'b0),
    .out(mcc_bus)
);

always @(posedge reg_ir_load or posedge reg_ir_load_override or negedge rst) begin
    $display("data = %02h", data);
    inst_reg <= rom_cjmp[int_inst_bus];
end

always @(posedge not_clk) begin
    sig_a <= rom_a[inst_bus];
    sig_b <= rom_b[inst_bus];
    sig_c <= rom_c[inst_bus];
    sig_d <= rom_d[inst_bus];
    sig_e <= rom_e[inst_bus];
    sig_f <= rom_f[inst_bus];
    sig_g <= rom_g[inst_bus];
end

initial begin
    //$monitor("[ctrl] mcc = %02h, mcc_tick = %1h, mcc_rst = %1h, rst = %1h, inst_reg = %02h, reg_ir_load = %1h, data = %02h, latched_int = %02h, int = %02h", mcc_bus, mcc_tick, ~mcc_rst, rst, inst_reg, reg_ir_load, data, latched_int_bus, int_bus);
end

assign irq_no = latched_int_bus;

assign int_bus = latched_int_bus & {5{int_en_sig}};

assign raw_inst_bus = { int_bus, data };
assign int_inst_bus = { rom_int[raw_inst_bus], flags[4:0] };
assign inst_bus = { 1'b0, inst_reg, mcc_bus };

assign signals[`REG_A_LOAD] = sig_a[0] & clk;
assign signals[`REG_B_LOAD] = sig_a[1] & clk;
assign signals[`ALU_OPC_4:`ALU_OPC_0] = sig_a[6:2];
assign signals[`REG_F_LOAD] = sig_a[7] & clk;

assign signals[`ALU_OUT:`REG_F_OUT] = sig_b[1:0];
assign signals[`REG_TMPH_LOAD] = sig_b[2] & clk;
assign signals[`REG_TMPL_LOAD] = sig_b[3] & clk;
assign signals[`REG_TMPH_PASS_DATA:`REG_TMPH_OUT] = sig_b[7:4];

assign signals[`PC_RST:`REG_TMPL_PASS_DATA] = sig_c[5:0];
assign signals[`PC_TICK] = sig_c[6];
assign signals[`PC_OUT] = sig_c[7];

assign signals[`STC_RST:`STC_LOAD] = sig_d[1:0];
assign signals[`STC_TICK] = sig_d[2] & clk;
assign signals[`STC_OUT:`STC_MODE] = sig_d[4:3];
assign signals[`REG_MAR_LOAD] = sig_d[5];
assign signals[`REG_MBR_LOAD] = sig_d[6];
assign signals[`MEM_OUT] = sig_d[7];

assign signals[`REG_MBR_USE_BUS:`MEM_IN] = sig_e[6:0];

assign reg_ir_load = sig_f[0] & clk;
assign signals[`REG_IR_LOAD] = reg_ir_load;
assign mcc_tick = (sig_f[1] & clk) | ~rst;
assign signals[`MCC_TICK] = mcc_tick;
assign mcc_rst = sig_f[2] & rst;
assign signals[`MCC_RST] = mcc_rst;
assign signals[`INT4:`INT0] = sig_f[7:3];
assign signals[`INT_ADDRESS_OUT] = sig_g[2];

// Load data into ROM, uses a plain binary file, not a text file with $memreadb/h
// format
initial begin
    integer file;
    file = $fopen(ROM_A, "rb");
    if (file == 0) $fatal("failed to open ROM file");
    $fread(rom_a, file);
    file = $fopen(ROM_B, "rb");
    $fread(rom_b, file);
    file = $fopen(ROM_C, "rb");
    $fread(rom_c, file);
    file = $fopen(ROM_D, "rb");
    $fread(rom_d, file);
    file = $fopen(ROM_E, "rb");
    $fread(rom_e, file);
    file = $fopen(ROM_F, "rb");
    $fread(rom_f, file);
    file = $fopen(ROM_G, "rb");
    $fread(rom_g, file);
    file = $fopen(ROM_INT, "rb");
    $fread(rom_int, file);
    file = $fopen(ROM_BRANCH, "rb");
    $fread(rom_cjmp, file);
end

endmodule
