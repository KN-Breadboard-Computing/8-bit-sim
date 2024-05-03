module tmp(
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
    inout wire [7:0] data,
    inout wire [15:0] address
);

// data_dir
//  1 : A -> B (BUS -> REG)
//  0 : B -> A (REG -> BUS)

reg [7:0] tmph;
reg [7:0] tmpl;

/* verilator lint_off UNOPTFLAT */
wire [7:0] tmph_bus;
wire [7:0] tmpl_bus;

initial begin
    //$monitor("reg_tmph_pass_data = %1b, reg_tmpl_pass_data = %1b, reg_tmph_data_dir = %1b, reg_tmpl_data_dir = %1b", reg_tmph_pass_data, reg_tmpl_pass_data, reg_tmph_data_dir, reg_tmpl_data_dir);
end

assign data = ~reg_tmph_pass_data ? (~reg_tmph_data_dir ? tmph_bus : 8'hZ) : 8'hZ;
assign data = ~reg_tmpl_pass_data ? (~reg_tmpl_data_dir ? tmpl_bus : 8'hZ) : 8'hZ;
assign tmph_bus = reg_tmph_out & reg_tmph_data_dir ? data : 8'hZ;
assign tmpl_bus = reg_tmpl_out & reg_tmpl_data_dir ? data : 8'hZ;
assign tmph_bus = ~reg_tmp_pass_address & reg_tmp_address_dir ? address[15:8] : 8'hZ;
assign tmpl_bus = ~reg_tmp_pass_address & reg_tmp_address_dir ? address[7:0] : 8'hZ;

assign tmph_bus = ~reg_tmph_out ? tmph : 8'hZ;
assign tmpl_bus = ~reg_tmpl_out ? tmpl : 8'hZ;

assign address = ~reg_tmp_pass_address ? (~reg_tmp_address_dir ? { tmph_bus, tmpl_bus } : 16'hZ) : 16'hZ;

always_ff @(posedge reg_tmph_load) tmph <= tmph_bus;
always_ff @(posedge reg_tmpl_load) tmpl <= tmpl_bus;

endmodule
