module cpu (
    input wire clk,
    output wire [15:0] addr_bus,
    inout wire [7:0] bus
);
    `include "include/signals.v"

    // SIGNAL REGISTER
    reg [41:0] signals = 0;

    // PROGRAM COUNTER
    wire [7:0] inner_pc_out;
    counter inner_pc (
        .clk(clk & ~signals[`PC_TICK]),
        .write(~signals[`PC_LOAD]),
        .reset(~signals[`PC_RST]),
        .countdown(0),
        .in(bus),
        .out(inner_pc_out),
    );
    tristate_buffer pc(
        .data_in(inner_pc_out),
        .enable(~signals[`PC_OUT]),
        .data_out(bus),
    );

    // ALU REGISTERS
    wire [7:0] alu_in;

    register a_reg (
        .clk(clk),
        .enable(signals[`REG_A_LOAD]),
        .data_in(bus),
        .data_out(alu_in),
    );
    register b_reg (
        .clk(clk),
        .enable(signals[`REG_B_LOAD]),
        .data_in(bus),
        .data_out(alu_in),
    );
    

endmodule
