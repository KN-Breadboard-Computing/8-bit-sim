module shift_reg (
    input wire clk,
    input wire [7:0] data_in,
    input wire data_in_enable,
    input wire shift_enable,
    output reg data_out
);

reg [7:0] register;

always_ff @(posedge clk) begin
    if (data_in_enable)
        register <= data_in;
    else if (shift_enable) begin
        data_out <= register[7];
        register <= register << 1;
    end
end

endmodule
