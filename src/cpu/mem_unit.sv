`include "basics/ram.sv"

module mem_unit(
    input wire zero_page, // active low
    input wire mem_part,
    input wire mem_out, // active low
    input wire mem_in, // active low
    input wire reg_mbr_load,
    input wire reg_mbr_word_dir,
    input wire reg_mar_load,
    input wire [15:0] address,
    inout wire [7:0] data
);
    reg [15:0] mar;
    reg [7:0] mbr;

    wire [16:0] addr_bus;
    wire [7:0] data_bus;

    ram ram(
        .chip_enable(1'b0),
        .chip_enable2(1'b1),
        .write_enable(mem_in),
        .output_enable(mem_out),
        .address(addr_bus),
        .data(data_bus)
    );

    initial begin
        $display("[mem] mar = %04x, mbr = %02x", mar, mbr);
    end

    always_ff @(posedge reg_mar_load) begin
        mar <= address;
        $display("[mem] mar = %04x, mbr = %02x", mar, mbr);
    end
    always_ff @(posedge reg_mbr_load) begin
        mbr <= data;
        $display("[mem] mar = %04x, mbr = %02x", mar, mbr);
    end
    
    assign data = ~reg_mbr_word_dir ? data_bus : 8'hZ; 
    assign data_bus = (~mem_in & mem_out) ? mbr : 8'hZ;

    assign addr_bus = { mem_part, mar[15:8] & { 8{zero_page} }, mar[7:0] };

endmodule
