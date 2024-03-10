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
    input wire interrupt_enable,
    output wire [7:0] red_out,
    output wire [7:0] green_out,
    output wire [7:0] blue_out
);

reg [7:0] glyphs_data [0:`TEXT_MODE_WIDTH * `TEXT_MODE_HEIGHT - 1];
reg [6:0] text_mode_cursor_x; // [0,TEXT_MODE_WIDTH - 1]
reg [5:0] text_mode_cursor_y; // [0,TEXT_MODE_HEIGHT - 1]

initial begin
    text_mode_cursor_x = 0;
    text_mode_cursor_y = 0;
end

always_ff @(posedge interrupt_enable) begin
    case (interrupt_in)
        `SIG_STORE_BYTE: begin
            glyphs_data[text_mode_cursor_x + text_mode_cursor_y * `TEXT_MODE_WIDTH] <= data_in;
            text_mode_cursor_x <= text_mode_cursor_x + 1;
            if (text_mode_cursor_x == `TEXT_MODE_WIDTH) begin
                text_mode_cursor_x <= 0;
                text_mode_cursor_y <= text_mode_cursor_y + 1;
                if (text_mode_cursor_y == `TEXT_MODE_HEIGHT) begin
                    text_mode_cursor_y <= 0;
                end
            end
       end
       `SIG_MOVE_CURSOR:
       // depending on the MSB either move x or y
       if (data_in[7]) begin
           text_mode_cursor_x <= text_mode_cursor_x + data_in[6:0];
           end else begin
           text_mode_cursor_y <= text_mode_cursor_y + data_in[5:0];
       end
       `SIG_DISPLAY: $display("Interrupt 2");
       `SIG_CLEAR: $display("Interrupt 3");
    endcase
end

// always_ff @(posedge clk) begin
//     red_out <= 128;
//     green_out <= 255;
//     blue_out <= 0;
// end

assign red_out = 128;
assign green_out = 255;
assign blue_out = 0;

endmodule

/* verilator lint_on UNUSEDSIGNAL */
