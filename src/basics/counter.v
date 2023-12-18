module counter(
  input wire clk,
  input wire write,
  input wire reset,
  input wire countdown,
  input wire [width-1:0] in,
  output reg [width-1:0] out
);
  parameter width = 8;

  initial
    out = 0;

  always @(posedge clk) begin
    if (reset)
        out <= 0;
    else if (write)
        out <= in;
    else if (countdown)
        out <= out - 1;
    else
        out <= out + 1;
  end

endmodule