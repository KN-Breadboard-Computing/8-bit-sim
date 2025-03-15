module Monitor_Tester(
   clock25MHz,
   red,
   blue,
   green,
   hsync,
   vsync
);
input  clock25MHz;
output reg [3:0]  red;
output reg [3:0]  blue;
output reg [3:0] green;
output hsync;
output vsync;

wire canDisplayImage;
wire [9:0] x;
wire [9:0] y;

wire [3:0] red1;
wire [3:0] green1;
wire [3:0] blue1;

// Instantiate VGA controller
VGA vga_inst(
    .clock25MHz(clock25MHz),
    .vsync(vsync),
    .hsync(hsync),
    .canDisplayImage(canDisplayImage),
    .x(x),
    .y(y)
);

// Instantiate TV pattern generator
tvPattern pattern_inst(
    .x(x),
    .y(y),
    .red(red1),
    .green(green1),
    .blue(blue1)
);

assign red = canDisplayImage ? red1 : 4'hF;
assign green = canDisplayImage ? green1 : 4'hF;
assign blue = canDisplayImage ? blue1 : 4'hF;

endmodule

