module gpu (
    input wire clk,
    output reg [7:0] red,
    output reg [7:0] green,
    output reg [7:0] blue
);

always_ff @(posedge clk) begin
    red <= 8'b11111111;
    green <= 8'b00000000;
    blue <= 8'b00000000;
end

endmodule
