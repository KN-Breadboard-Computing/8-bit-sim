module modcounter #(parameter rst_value = 0, parameter width = 9) (
    input wire clk,
    output wire overflow,
    output wire [width - 1 : 0] data_out
);

reg [width - 1 : 0] data;
wire rst;

always_ff @(posedge clk) begin
    if (rst) data <= 0;
    else data <= data + 1;
end

assign rst = (data == rst_value);
assign overflow = rst;
assign data_out = data;

endmodule
