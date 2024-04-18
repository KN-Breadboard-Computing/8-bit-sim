`define DISPLAY_WIDTH 640
`define DISPLAY_HEIGHT 480
`define TEXT_MODE_WIDTH 80
`define TEXT_MODE_HEIGHT 60
`define GLYPH_COUNT 256
`define GLYPH_WIDTH 8
`define GLYPH_HEIGHT 8

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
    output reg [7:0] red_out,
    output reg [7:0] green_out,
    output reg [7:0] blue_out
);

// NOTE: The order of array sizes might need to be reversed
// NOTE: The array is 2D because we need to store two buffers
//       - one active, one inactive
reg [7:0] glyph_buffers [0:`TEXT_MODE_WIDTH * `TEXT_MODE_HEIGHT - 1][0:1];
reg [7:0] color_buffers [0:`DISPLAY_WIDTH * `DISPLAY_HEIGHT - 1][0:1];
reg active_buf;

// NOTE: glyph_data[glyph_number + x%8 + (y%8 * 8)] = 1 <=> FG color
//       glyph_data[glyph_number + x%8 + (y%8 * 8)] = 0 <=> BG color
reg [7:0] glyph_data [0:(`GLYPH_COUNT / 8) * `GLYPH_WIDTH * `GLYPH_HEIGHT];

reg [6:0] text_mode_cursor_x; // [0,TEXT_MODE_WIDTH - 1]
reg [5:0] text_mode_cursor_y; // [0,TEXT_MODE_HEIGHT - 1]

reg [9:0] h_counter_val;
reg [9:0] v_counter_val;

reg shift_reg_load;
reg [7:0] shift_reg_in;
wire shift_reg_out;
wire shift_reg_enable;

assign shift_reg_enable = h_counter_val < `DISPLAY_WIDTH && v_counter_val < `DISPLAY_HEIGHT;

shift_reg glyph_row(
    .clk(clk),
    .data_in(shift_reg_in),
    .data_in_enable(shift_reg_load),
    .shift_enable(shift_reg_enable),
    .data_out(shift_reg_out)
);

initial begin
    integer file, i;
    file = $fopen(`"`FONT_PATH/font.bin`", "rb");

    // display path
    $display("Font path: %s", `"`FONT_PATH/font.bin`");

    if (file == 0) begin
        $display("Error opening file");
        $finish;
    end
    $fread(glyph_data, file);

    for (i = 0; i < 8; i = i + 1) begin
        $display("%8b", glyph_data[i]);
    end

    text_mode_cursor_x = 0;
    text_mode_cursor_y = 0;
    active_buf = 0;
    h_counter_val = 0;
    v_counter_val = 0;
end

always_ff @(posedge interrupt_enable) begin
    case (interrupt_in)
        `SIG_STORE_BYTE: begin
            glyph_buffers[text_mode_cursor_x + text_mode_cursor_y * `TEXT_MODE_WIDTH][1 - active_buf] <= data_in;
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
       `SIG_DISPLAY: begin
           active_buf <= 1 - active_buf;
       end
       `SIG_CLEAR: $display("Interrupt 3");
    endcase
end

always_ff @(posedge clk) begin
    if (h_counter_val < `DISPLAY_WIDTH && v_counter_val < `DISPLAY_HEIGHT) begin
        if (h_counter_val % 8 == 6) begin
            shift_reg_in <= glyph_data[{9'b0,v_counter_val[2:0]}];
            shift_reg_load <= 1;
        end else begin
            shift_reg_load <= 0;
        end
        // $display("shift_reg_out: %b", shift_reg_out);
        if (shift_reg_out == 1) begin
            red_out <= 255;
            green_out <= 255;
            blue_out <= 255;
        end else begin
            red_out <= 0;
            green_out <= 0;
            blue_out <= 0;
        end

    end else shift_reg_load <= 0;

    h_counter_val <= (h_counter_val + 1) % 800;


    if (h_counter_val == 799) begin
        v_counter_val <= (v_counter_val + 1) % 525;
    end

    if (h_counter_val == 798 && v_counter_val < `DISPLAY_HEIGHT) begin
        shift_reg_in <= glyph_data[{9'b0,v_counter_val[2:0] + 3'b1}];
        shift_reg_load <= 1;
    end
end

endmodule

/* verilator lint_on UNUSEDSIGNAL */

