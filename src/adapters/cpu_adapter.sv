module cpu_adapter(
    input wire clk,
    input wire rst,
    input wire [4:0] int_in,
    output wire zero_page,
    output wire mem_part,
    output wire mem_out,
    output wire mem_in,
    output wire reg_mbr_load,
    output wire reg_mbr_word_dir,
    output wire reg_mar_load,
    output wire [15:0] addr_bus,
    output wire [4:0] int_bus,
    input wire bus_in_en,
    input wire [7:0] bus_in,
    output wire [7:0] bus_out
);

    wire [7:0] bus;

    cpu cpu(
        .clk(clk),
        .rst(rst),
        .int_in(int_in),
        .zero_page(zero_page),
        .mem_part(mem_part),
        .mem_out(mem_out),
        .mem_in(mem_in),
        .reg_mbr_load(reg_mbr_load),
        .reg_mbr_word_dir(reg_mbr_word_dir),
        .reg_mar_load(reg_mar_load),
        .addr_bus(addr_bus),
        .int_bus(int_bus),
        .data_bus(bus)
    );

    assign bus = bus_in_en ? bus_in : 8'hZ;
    assign bus_out = bus;

    initial begin
        $dumpfile("dump.fst");
        $dumpvars(0, cpu_adapter);
    end

endmodule
