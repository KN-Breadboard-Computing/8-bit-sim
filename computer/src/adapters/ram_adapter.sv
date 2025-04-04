module ram_adapter #(
    parameter ADDR_WIDTH = 17
)(
    input wire chip_enable,
    input wire chip_enable2,
    input wire write_enable,
    input wire output_enable,
    input wire [(ADDR_WIDTH - 1):0] address,
    input wire [7:0] data_in,
    input wire data_in_en,
    output wire [7:0] data_out
);
    wire [7:0] data;

    ram #(ADDR_WIDTH) r(
        .chip_enable(chip_enable),
        .chip_enable2(chip_enable2),
        .write_enable(write_enable),
        .output_enable(output_enable),
        .address(address),
        .data(data)
    );

    assign data = data_in_en ? data_in : 8'hZ;
    assign data_out = data;
endmodule
