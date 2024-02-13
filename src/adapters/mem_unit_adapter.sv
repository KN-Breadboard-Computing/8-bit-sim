module mem_unit_adapter(
    input wire zero_page, // active low
    input wire mem_part,
    input wire mem_out, // active low
    input wire mem_in, // active low
    input wire reg_mbr_load,
    input wire reg_mbr_word_dir,
    input wire reg_mar_load,
    input wire [15:0] address,
    input wire [7:0] data_in,
    input wire data_in_en,
    output wire [7:0] data_out
);

    wire [7:0] data;

    mem_unit me(
        .zero_page(zero_page),
        .mem_part(mem_part),
        .mem_out(mem_out),
        .mem_in(mem_in),
        .reg_mbr_load(reg_mbr_load),
        .reg_mbr_word_dir(reg_mbr_word_dir),
        .reg_mar_load(reg_mar_load),
        .address(address),
        .data(data)
    );

    assign data = data_in_en ? data_in : 8'hZ;
    assign data_out = data;

endmodule
