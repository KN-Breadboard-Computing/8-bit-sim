`define CONST_ONE 0
`define CONST_MINUS_ONE 1
`define ADDER 2
`define XOR 3
`define LOGIC 4
`define SHIFT_LEFT 5
`define ARITHMETIC_SHIFT_RIGHT 6
`define LOGIC_SHIFT_RIGHT 7

module alu(
    input wire alu_out, // active low
    input wire reg_f_out, // active low
    input wire reg_f_load,
    input wire [4:0] opcode,
    input wire [7:0] reg_a,
    input wire [7:0] reg_b,
    output wire [7:0] data_out
);

logic inv_a;
logic inv_b;
logic zero_a;
logic zero_b;
logic inv_logic;
logic [4:0] mux;

wire [7:0] a_bus;
wire [7:0] b_bus;

assign a_bus = (reg_a & {8{~zero_a}}) ^ {8{inv_a}};
assign b_bus = (reg_b & {8{~zero_b}}) ^ {8{inv_b}};

wire [7:0] logic_bus;
wire [7:0] xor_bus;
wire [7:0] adder_bus;
wire c6_out;
wire c7_out;

assign logic_bus = (a_bus | b_bus) ^ {8{inv_logic}};
assign xor_bus = a_bus ^ b_bus;
assign {c6_out, adder_bus[6:0]} = a_bus[6:0] + b_bus[6:0] + {7'h00, inv_a | inv_b};
assign {c7_out, adder_bus[7]} = a_bus[7] + b_bus[7] + c6_out;

logic [7:0] data_bus;

reg [4:0] flags_reg;

assign data_out = ~alu_out ? data_bus : ~reg_f_out ? { 3'b000, flags_reg } : 8'hZ;

always_latch begin
    if (reg_f_load) begin
        // Guidelines say that non-blocking (<=) assignments should be used in
        // latches, however when used here Verilator would issue a COMBDLY
        // warning, so blocking (=) assignments are used instead 
        flags_reg[0] = data_bus[7];
        flags_reg[1] = data_bus[0];
        flags_reg[2] = data_bus != 8'h00;
        flags_reg[3] = c7_out;
        flags_reg[4] = c7_out ^ c6_out;
    end
end

always_comb begin
    case (mux) 
        `CONST_ONE: data_bus = 8'h01;
        `CONST_MINUS_ONE: data_bus = 8'hFF;
        `ADDER: data_bus = adder_bus;
        `XOR: data_bus = xor_bus;
        `LOGIC: data_bus = logic_bus;
        `SHIFT_LEFT: data_bus = adder_bus << 1;
        `ARITHMETIC_SHIFT_RIGHT: data_bus =  adder_bus >>> 1;
        `LOGIC_SHIFT_RIGHT: data_bus = adder_bus >> 1;
    endcase
end

always_comb begin
    case (opcode) 
    //CONST_ZERO
    'b00000: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 1;
            zero_b = 1;
            inv_logic = 0;
            mux = `ADDER;
        end
    //CONST_ONE
    'b00001: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 0;
            zero_b = 0;
            inv_logic = 0;
            mux = `CONST_ONE;
        end
    //CONST_MINUS_ONE
    'b00010: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 0;
            zero_b = 0;
            inv_logic = 0;
            mux = `CONST_MINUS_ONE;
        end
    //REG_A
    'b00011: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 0;
            zero_b = 1;
            inv_logic = 0;
            mux = `ADDER;
        end
    //REG_B
    'b00100: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 1;
            zero_b = 0;
            inv_logic = 0;
            mux = `ADDER;
        end
    //MINUS_REG_A
    'b00101: begin
            inv_a = 1;
            inv_b = 0;
            zero_a = 0;
            zero_b = 1;
            inv_logic = 0;
            mux = `ADDER;
        end
    //MINUS_REG_B
    'b00110: begin
            inv_a = 0;
            inv_b = 1;
            zero_a = 1;
            zero_b = 0;
            inv_logic = 0;
            mux = `ADDER;
        end
    //REG_A_PLUS_REG_B
    'b00111: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 0;
            zero_b = 0;
            inv_logic = 0;
            mux = `ADDER;
        end
    //REG_A_MINUS_REG_B
    'b01000: begin
            inv_a = 0;
            inv_b = 1;
            zero_a = 0;
            zero_b = 0;
            inv_logic = 0;
            mux = `ADDER;
        end
    //REG_B_MINUS_REG_A
    'b01001: begin
            inv_a = 1;
            inv_b = 0;
            zero_a = 0;
            zero_b = 0;
            inv_logic = 0;
            mux = `ADDER;
        end
    //NOT_REG_A
    'b01010: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 0;
            zero_b = 1;
            inv_logic = 1;
            mux = `LOGIC;
        end
    //NOT_REG_B
    'b01011: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 1;
            zero_b = 0;
            inv_logic = 1;
            mux = `LOGIC;
        end
    //REG_A_OR_REG_B
    'b01100: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 0;
            zero_b = 0;
            inv_logic = 0;
            mux = `LOGIC;
        end
    //REG_A_AND_REG_B
    'b01101: begin
            inv_a = 1;
            inv_b = 1;
            zero_a = 0;
            zero_b = 0;
            inv_logic = 1;
            mux = `LOGIC;
        end
    //REG_A_XOR_REG_B
    'b01110: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 0;
            zero_b = 0;
            inv_logic = 0;
            mux = `XOR;
        end
    //ARITHMETIC_RIGHT_SHIFT_REG_A
    'b01111: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 0;
            zero_b = 1;
            inv_logic = 0;
            mux = `ARITHMETIC_SHIFT_RIGHT;
        end
    //ARITHMETIC_RIGHT_SHIFT_REG_B
    'b10000: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 1;
            zero_b = 0;
            inv_logic = 0;
            mux = `ARITHMETIC_SHIFT_RIGHT;
        end
    //ARITHMETIC_LEFT_SHIFT_REG_A
    'b10001: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 0;
            zero_b = 1;
            inv_logic = 0;
            mux = `SHIFT_LEFT;
        end
    //ARITHMETIC_LEFT_SHIFT_REG_B
    'b10010: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 1;
            zero_b = 0;
            inv_logic = 0;
            mux = `SHIFT_LEFT;
        end
    //LOGIC_RIGHT_SHIFT_REG_A
    'b10011: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 0;
            zero_b = 1;
            inv_logic = 0;
            mux = `LOGIC_SHIFT_RIGHT;
        end
    //LOGIC_RIGHT_SHIFT_REG_B
    'b10100: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 1;
            zero_b = 0;
            inv_logic = 0;
            mux = `LOGIC_SHIFT_RIGHT;
        end
    //LOGIC_LEFT_SHIFT_REG_A
    'b10101: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 0;
            zero_b = 1;
            inv_logic = 0;
            mux = `SHIFT_LEFT;
        end
    //LOGIC_LEFT_SHIFT_REG_B
    'b10110: begin
            inv_a = 0;
            inv_b = 0;
            zero_a = 1;
            zero_b = 0;
            inv_logic = 0;
            mux = `SHIFT_LEFT;
        end
    endcase
end

endmodule
