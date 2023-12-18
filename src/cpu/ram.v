// FIXME: The data is not being written to the memory.
module ram (
    input wire clk,
    input wire write_enable,
    input wire output_enable,
    input wire [15:0] address,
    inout wire [7:0] data
);
    
    reg [7:0] memory [0:65535];
    reg [7:0] buffer;

    always @(posedge clk) begin
        if (write_enable) begin
            $display("Memory: set [0x%h] => 0x%h (%d)", address, data, data);
            memory[address] <= data;
        end else begin
            buffer <= memory[address];
        end
    end

    assign data = (output_enable & ~write_enable) ? buffer : 8'bz;

endmodule
