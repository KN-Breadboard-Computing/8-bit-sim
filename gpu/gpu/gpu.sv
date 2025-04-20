`include "gpu/modcounter.sv"
`include "basics/counter.v"
`include "basics/shift_reg.sv"
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
    input wire rst,
    input wire [1:0] interrupt_code_in,
    input wire [7:0] interrupt_data_in,
    input wire interrupt_enable,
    output reg [7:0] red,
    output reg [7:0] green,
    output reg [7:0] blue,
    output logic hsync,
    output logic vsync
);

typedef enum logic [1:0] {
    FRONT_PORCH = 2'b00,
    SYNC        = 2'b01,
    BACK_PORCH  = 2'b10,
    DISPLAY     = 2'b11
} Region;

Region h_region, next_h_region;
Region v_region, next_v_region;

reg [9:0] h_counter;
reg rst_h_counter, next_rst_h_counter;
reg [8:0] v_counter;
reg rst_v_counter, next_rst_v_counter;

wire next_line;


always_ff @(posedge clk)
    if (!rst && !rst_h_counter) h_counter <= h_counter + 1;
    else h_counter <= '0;

always_comb begin
    next_h_region = h_region;
    rst_h_counter = 0;
    case (h_region)
        FRONT_PORCH: 
            if (h_counter == 15) begin
                next_h_region = SYNC;
                rst_h_counter = 1;
            end
        SYNC:
            if (h_counter == 95) begin
                next_h_region = BACK_PORCH;
                rst_h_counter = 1;
            end
        BACK_PORCH:
            if (h_counter == 47) begin
                next_h_region = DISPLAY;
                rst_h_counter = 1;
            end
        DISPLAY:
            if (h_counter == 639) begin
                next_h_region = FRONT_PORCH;
                rst_h_counter = 1;
            end
     endcase
end

always_ff @(posedge clk)
    h_region <= next_h_region;

assign next_line = h_region == FRONT_PORCH && next_h_region == SYNC;

always_ff @(posedge clk)
    if (next_line)
        if (!rst_v_counter) v_counter <= v_counter + 1;
        else v_counter <= '0;

always_comb begin
    next_v_region = v_region;
    rst_v_counter = 0;

    case (v_region)
        FRONT_PORCH:
            if (v_counter == 9) begin
                next_v_region = SYNC;
                rst_v_counter = 1;
            end
        SYNC:
            if (v_counter == 1) begin
                next_v_region = BACK_PORCH;
                rst_v_counter = 1;
            end
        BACK_PORCH:
            if (v_counter == 32) begin
                next_v_region = DISPLAY;
                rst_v_counter = 1;
            end
        DISPLAY:
            if (v_counter == 480) begin
                next_v_region = FRONT_PORCH;
                rst_v_counter = 1;
            end
    endcase
end

always_ff @(posedge clk) begin
    if (next_line)
        v_region <= next_v_region;
end

reg  [12:0] char_write_addr;
reg  [7:0]  char_in;
reg         write_char;
wire [12:0] char_read_addr;
wire [7:0]  char;

ram #(.ADDR_WIDTH(13)) char_buf(
    .clk(clk),
    .data(char_in),
    .write_addr(char_write_addr),
    .read_addr(char_read_addr),
    .out(char),
    .we(write_char)
);

reg [18:0] px_counter;
wire active_region;

assign active_region_early = next_h_region == DISPLAY && v_region == DISPLAY;
assign active_region_late  =      h_region == DISPLAY && v_region == DISPLAY;

always_ff @(posedge clk)
    if (active_region) 
        if (px_counter != 307199) px_counter <= px_counter + 1;
        else px_counter <= '0;

wire [12:0] cursor;

assign char_read_addr = cursor;

reg [12:0] color_write_addr;
reg [7:0]  color_in;
reg        write_color;
wire [12:0] color_read_addr;
wire [7:0]  color;

ram #(.ADDR_WIDTH(13)) color_buf(
    .clk(clk),
    .data(color_in),
    .write_addr(color_write_addr),
    .read_addr(color_read_addr),
    .out(color),
    .we(write_color)
);

assign color_read_addr = cursor;

reg [6:0] h_cursor;
reg [12:0] v_cursor;

always_ff @(posedge clk)
    if (next_line)
        if (v_region == DISPLAY) begin
            if (v_counter[2:0] == 7)
                v_cursor <= v_cursor + 13'd80;
        end else begin
            v_cursor <= '0;
        end

always_ff @(posedge clk)
    if (v_region == DISPLAY) begin
        if (h_region == DISPLAY) begin
            if (h_counter[2:0] == 7) begin
                h_cursor <= h_cursor + 1;
            end
        end else h_cursor <= '0;
    end

assign cursor = v_cursor + { 5'h00, h_cursor };

reg [7:0] glyph_rom [2048];
reg [4:0][2:0] palette_rom [32];

initial begin
    $readmemh(`"`FONT_PATH/font.mem`", glyph_rom);
    $readmemh(`"`FONT_PATH/palette.mem`", palette_rom);
end

reg [3:0] glyph_bit_sel;

always_ff @(posedge clk)
    if (active_region_late)
        glyph_bit_sel <= glyph_bit_sel - 1;

reg [7:0] glyph;
reg [4:0] fg_red, bg_red;
reg [4:0] fg_green, bg_green;
reg [4:0] fg_blue, bg_blue;

always_ff @(posedge clk)
    if (active_region_early) begin
        reg [7:0] tmp_glyph;

        tmp_glyph = glyph_rom[{ char, v_counter[2:0] }];
        glyph <= tmp_glyph;
        { fg_red, fg_green, fg_blue } <= palette_rom[{ 1'b0, color[3:0] }];
        { bg_red, bg_green, bg_blue } <= palette_rom[{ 1'b1, color[7:4] }];
    end

assign red = glyph[glyph_bit_sel] ? fg_red : bg_red;
assign green = glyph[glyph_bit_sel] ? fg_green : bg_green;
assign blue = glyph[glyph_bit_sel] ? fg_blue : bg_blue;

assign hsync = h_region == SYNC;
assign vsync = v_region == SYNC;

reg char_write_request;
reg char_write_pending;
reg char_write_done;
reg [12:0] write_cursor;
reg [7:0] pending_char;

always_ff @(posedge clk) begin
    if (char_write_request) begin
        char_in <= pending_char;
        char_write_addr <= write_cursor;
        char_write_pending <= 1'b1;
        write_char <= 1'b1;
    end else begin
        char_write_pending <= 1'b0;
        char_write_done <= 1'b0;
    end

    if (char_write_pending) begin
        write_char <= 1'b0;
        char_write_done <= 1'b1;
    end
end

always_latch @(interrupt_enable or char_write_done) begin
    if (interrupt_enable) begin
        case (interrupt_code_in)
            `SIG_STORE_BYTE: begin
                char_write_request = 1'b1;
                pending_char = interrupt_data_in;
                write_cursor = write_cursor + 1;
                if (write_cursor == 4800)
                    write_cursor = '0;
            end
            `SIG_MOVE_CURSOR: begin
                if (interrupt_data_in[7])
                    write_cursor = write_cursor + 13'(signed'(interrupt_data_in[6:0]));
                else 
                    write_cursor = write_cursor + 80*(13'(signed'(interrupt_data_in[5:0]))); // NOTE: multiply
                if (write_cursor >= 4800)
                    write_cursor = 0;
                $display("write cursor moved cursor = %04xh", write_cursor);
            end
        endcase
    end else if (char_write_done) char_write_request = 1'b0;
end

// TODO: proper signal triggered reset
initial begin
    v_region = SYNC;
    h_region = SYNC;
    h_counter = 0;
    v_counter = 0;
    rst_h_counter = 0;
    rst_v_counter = 0;
    write_char = 0;
    write_cursor = 4799;
    px_counter = 0;
    glyph_bit_sel = 0;
    color_buf.mem[0] = 8'hFF;
    h_cursor = 0;
    v_cursor = 0;
end

initial begin
    //$dumpfile("gpu.fst");
    //$dumpvars(0, gpu);
end

endmodule

/* verilator lint_on UNUSEDSIGNAL */

