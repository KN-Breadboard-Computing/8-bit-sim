module shift_reg (
    input wire clk,
    input wire [7:0] data_in,
    input wire data_in_enable,
    input wire shift_enable,
    output reg data_out
);

reg [7:0] register;

assign data_out = register[7];

always_ff @(posedge clk) begin
    if (shift_enable) begin
        register <= {register[6:0], 1'b0};
    end

    if (data_in_enable) begin
        register <= data_in;
    end
end

endmodule
