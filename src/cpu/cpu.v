module cpu (
    input wire clk,
    output wire [15:0] addr_bus,
    inout wire [7:0] bus
);
    // TODO przejrzeć specyfikacje naszych fizycznych układów i ustalić gdzie jaki jest wymagany stan wejścia - szczególnie dla sygnałów

    `include "include/signals.v"

    // SIGNAL REGISTER
    reg [41:0] signals = 0;

    // PROGRAM COUNTER
    wire [7:0] pc_out;
    counter pc(
        .clk(signals[`PC_TICK]),
        .write(signals[`PC_LOAD]),
        .reset(signals[`PC_RST]),
        .countdown(1'b0),
        .in(bus),
        .out(pc_out)
    );
    tristate_buffer pc_to_bus(
        .data_in(pc_out),
        .enable(signals[`PC_OUT]),
        .data_out(bus)
    );

    // STACK COUNTER
    wire [15:0] stc_out;
    counter #(.width(16)) stc (
        .clk(signals[`STC_TICK]),
        .write(signals[`STC_LOAD]),
        .reset(signals[`STC_RST]),
        .countdown(signals[`STC_MODE]),
        .in(addr_bus),
        .out(stc_out)
    );
    tristate_buffer #(.width(16)) stc_to_bus (
        .data_in(stc_out),
        .enable(signals[`STC_OUT]),
        .data_out(addr_bus)
    );

    // ALU 
    wire [7:0] a_out;
    wire [7:0] b_out;
    wire [7:0] alu_out;
    wire [7:0] f_reg_out;
    wire [7:0] f_reg_in;

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
    register f_reg (
        .clk(clk),
        .enable(signals[`REG_F_LOAD]),
        .data_in(f_reg_in),
        .data_out(f_reg_out)
    );
    tristate_buffer f_reg_to_bus (
        .data_in(f_reg_out),
        .enable(signals[`REG_F_OUT]),
        .data_out(bus)
    );

    alu alu_unit (
        .clk(clk),
        .opcode(signals[`ALU_OPC_4:`ALU_OPC_0]),
        .reg_a(a_out),
        .reg_b(b_out),
        .out_enable(signals[`ALU_OUT]),
        .flags_register(f_reg_in),
        .data_out(alu_out)
    );
    tristate_buffer alu_to_bus (
        .data_in(alu_out),
        .enable(signals[`ALU_OUT]),
        .data_out(bus)
    );


    

endmodule
