module tmp_adapter(
    input wire reg_tmph_data_dir,
    input wire reg_tmph_pass_data, // active low
    input wire reg_tmph_load,
    input wire reg_tmph_out, // active low
    input wire reg_tmpl_data_dir,
    input wire reg_tmpl_pass_data, // active low
    input wire reg_tmpl_load,
    input wire reg_tmpl_out, // active low
    input wire reg_tmp_pass_address, // active low
    input wire reg_tmp_address_dir,
    input wire data_in_en,
    input wire [7:0] data_in,
/* verilator lint_off UNOPTFLAT */
    output wire [7:0] data_out,
/* verilator lint_on UNOPTFLAT */
    input wire address_in_en,
    input wire [15:0] address_in,
    output wire [15:0] address_out
);

wire [7:0] data_bus;
wire [15:0] address_bus;

assign data_bus = data_in_en ? data_in : 8'hZ;
assign data_out = data_bus;
assign address_bus = address_in_en ? address_in : 16'hZ;
assign address_out = address_bus;

tmp tmp(
    .reg_tmph_data_dir(reg_tmph_data_dir),
    .reg_tmph_pass_data(reg_tmph_pass_data),
    .reg_tmph_load(reg_tmph_load),
    .reg_tmph_out(reg_tmph_out),
    .reg_tmpl_data_dir(reg_tmpl_data_dir),
    .reg_tmpl_pass_data(reg_tmpl_pass_data),
    .reg_tmpl_load(reg_tmpl_load),
    .reg_tmpl_out(reg_tmpl_out),
    .reg_tmp_pass_address(reg_tmp_pass_address),
    .reg_tmp_address_dir(reg_tmp_address_dir),
    .data(data_bus),
    .address(address_bus)
);
endmodule
