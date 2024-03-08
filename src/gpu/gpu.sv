`define DISPLAY_WIDTH 640
`define DISPLAY_HEIGHT 480
`define TEXT_MODE_WIDTH 80
`define TEXT_MODE_HEIGHT 60

`define SIG_STORE_BYTE 2'b00
`define SIG_MOVE_CURSOR 2'b01
`define SIG_DISPLAY 2'b10
`define SIG_CLEAR 2'b11

/* verilator lint_off UNUSEDSIGNAL */

module gpu(
    input wire clk,
    input wire [1:0] interrupt_in,
    input wire [7:0] data_in,
    input wire interrupt_enable
);

reg [7:0] pixel_data [0:`DISPLAY_WIDTH * `DISPLAY_HEIGHT - 1];

reg [9:0] cursor_x;
reg [8:0] cursor_y;

initial begin
    cursor_x = 0;
    cursor_y = 0;
end

always_ff @(posedge clk) begin
    if (interrupt_enable) begin
        case (interrupt_in)
            `SIG_STORE_BYTE: $display("Interrupt 0");
            `SIG_MOVE_CURSOR: $display("Interrupt 1");
            `SIG_DISPLAY: $display("Interrupt 2");
            `SIG_CLEAR: $display("Interrupt 3");
        endcase
    end
end

endmodule

/* verilator lint_on UNUSEDSIGNAL */
