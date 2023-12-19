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
        {3'b000, sign_flag, parity_flag, zero_flag, carry_flag, overflow_flag};

// TODO: Implement this in gate level.
    always @(posedge clk) begin
        if (out_enable) begin
            casez (opcode)
            //CONST_ZERO
                'b00000:
                    data_out <= 8'b00000000;
            //CONST_ONE
                'b00001:
                    data_out <= 8'b00000001;
            //CONST_MINUS_ONE
                'b00010:
                    data_out <= 8'b11111111;
            //REG_A
                'b00011:
                    data_out <= reg_a;
            //REG_B
                'b00100:
                    data_out <= reg_b;
            //MINUS_REG_A
                'b00101:
                    data_out <= ~reg_a + 1'b1;
            //MINUS_REG_B
                'b00110:
                    data_out <= ~reg_b + 1'b1;
            //REG_A_PLUS_REG_B
                'b00111:
                    data_out <= reg_a + reg_b;
            //REG_A_MINUS_REG_B
                'b01000:
                    data_out <= reg_a - reg_b;
            //REG_B_MINUS_REG_A
                'b01001:
                    data_out <= reg_b - reg_a;
            //NOT_REG_A
                'b01010:
                    data_out <= ~reg_a;
            //NOT_REG_B
                'b01011:
                    data_out <= ~reg_b;
            //REG_A_OR_REG_B
                'b01100:
                    data_out <= reg_a | reg_b;
            //REG_A_AND_REG_B
                'b01101:
                    data_out <= reg_a & reg_b;
            //REG_A_XOR_REG_B
                'b01110:
                    data_out <= reg_a ^ reg_b;
            //ARITHMETIC_RIGHT_SHIFT_REG_A
                'b01111:
                    data_out <= reg_a >> 1;
            //ARITHMETIC_RIGHT_SHIFT_REG_B
                'b10000:
                    data_out <= reg_b >> 1;
            //ARITHMETIC_LEFT_SHIFT_REG_A
                'b10001:
                    data_out <= reg_a << 1;
            //ARITHMETIC_LEFT_SHIFT_REG_B
                'b10010:
                    data_out <= reg_b << 1;
            //LOGIC_RIGHT_SHIFT_REG_A
                'b10011:
                    data_out <= reg_a >>> 1;
            //LOGIC_RIGHT_SHIFT_REG_B
                'b10100:
                    data_out <= reg_b >>> 1;
            //LOGIC_LEFT_SHIFT_REG_A
                'b10101:
                    data_out <= reg_a <<< 1;
            //LOGIC_LEFT_SHIFT_REG_B
                'b10110:
                    data_out <= reg_b <<< 1;
            endcase
        end

        sign_flag <= (data_out[7] == 1'b1);
        parity_flag <= (^data_out[7:0]);
        zero_flag <= (data_out == 8'b00000000);
        carry_flag <= (reg_a + reg_b > 9'b011111111);
        overflow_flag <= ((reg_a[7] == reg_b[7]) && (reg_a[7] != data_out[7]));
    end

endmodule
