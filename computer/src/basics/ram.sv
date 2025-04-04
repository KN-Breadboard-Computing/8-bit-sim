module ram #(
    parameter ADDR_WIDTH = 17
)(
    input wire chip_enable,
    input wire chip_enable2,
    input wire write_enable,
    input wire output_enable,
    input wire [(ADDR_WIDTH - 1):0] address,
    inout wire [7:0] data
);

    reg [7:0] storage [1 << ADDR_WIDTH];
    reg [7:0] buffer;

    always_ff @(negedge write_enable or address) begin
        /* verilator lint_off SYNCASYNCNET */
        if (~chip_enable & chip_enable2 & ~write_enable) begin
            $display("[ram] wrote = %02x @ %04x", data, address);
            storage[address] <= data;
        end
        /* verilator lint_on SYNCASYNCNET */
    end

    always_ff @(negedge output_enable or address) begin
        if (~chip_enable & chip_enable2 & write_enable) begin
            buffer <= storage[address];
        end
    end

    assign data = (~chip_enable & chip_enable2 & write_enable & ~output_enable) ? buffer : 8'hZ;

endmodule
