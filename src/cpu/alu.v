module alu (
    input wire clk,
    input wire [4:0] opcode,
    input wire [7:0] reg_a,
    input wire [7:0] reg_b,
    input wire out_enable,
    output reg [7:0] flags_register,
    output reg [7:0] data_out
);
    reg sign_flag;
    reg parity_flag;
    reg zero_flag;
    reg carry_flag;
    reg overflow_flag;

    assign flags_register =
        {0, 0, 0, sign_flag, parity_flag, zero_flag, carry_flag, overflow_flag};

// TODO: Implement this in gate level.
    always @(posedge clk) begin
        if (out_enable) begin
            casez (opcode)
            //CONST_ZERO
                'b0000000000000:
                    data_out <= 8'b00000000;
            //CONST_ONE
                'b0000000000001:
                    data_out <= 8'b00000001;
            //CONST_MINUS_ONE
                'b0000000000010:
                    data_out <= 8'b11111111;
            //REG_A
                'b0000000000011:
                    data_out <= reg_a;
            //REG_B
                'b0000000000100:
                    data_out <= reg_b;
            //MINUS_REG_A
                'b0000000000101:
                    data_out <= ~reg_a + 1'b1;
            //MINUS_REG_B
                'b0000000000110:
                    data_out <= ~reg_b + 1'b1;
            //REG_A_PLUS_REG_B
                'b0000000000111:
                    data_out <= reg_a + reg_b;
            //REG_A_MINUS_REG_B
                'b0000000001000:
                    data_out <= reg_a - reg_b;
            //REG_B_MINUS_REG_A
                'b0000000001001:
                    data_out <= reg_b - reg_a;
            //NOT_REG_A
                'b0000000001010:
                    data_out <= ~reg_a;
            //NOT_REG_B
                'b0000000001011:
                    data_out <= ~reg_b;
            //REG_A_OR_REG_B
                'b0000000001100:
                    data_out <= reg_a | reg_b;
            //REG_A_AND_REG_B
                'b0000000001101:
                    data_out <= reg_a & reg_b;
            //REG_A_XOR_REG_B
                'b0000000001110:
                    data_out <= reg_a ^ reg_b;
            //ARITHMETIC_RIGHT_SHIFT_REG_A
                'b0000000001111:
                    data_out <= reg_a >> 1;
            //ARITHMETIC_RIGHT_SHIFT_REG_B
                'b0000000010000:
                    data_out <= reg_b >> 1;
            //ARITHMETIC_LEFT_SHIFT_REG_A
                'b0000000010001:
                    data_out <= reg_a << 1;
            //ARITHMETIC_LEFT_SHIFT_REG_B
                'b0000000010010:
                    data_out <= reg_b << 1;
            //LOGIC_RIGHT_SHIFT_REG_A
                'b0000000010011:
                    data_out <= reg_a >>> 1;
            //LOGIC_RIGHT_SHIFT_REG_B
                'b0000000010100:
                    data_out <= reg_b >>> 1;
            //LOGIC_LEFT_SHIFT_REG_A
                'b0000000010101:
                    data_out <= reg_a <<< 1;
            //LOGIC_LEFT_SHIFT_REG_B
                'b0000000010110:
                    data_out <= reg_b <<< 1;
            endcase
        end
    end

    assign flags_register =
        out_enable ? {0, 0, 0, sign_flag, parity_flag, zero_flag, carry_flag, overflow_flag} : 8'bz;

endmodule
