/**
 * RAM block
 *
 * Based on Intel docs
 *   https://www.intel.com/content/www/us/en/docs/programmable/683323/18-1/single-clock-synchronous-ram-with-new.html 
 */
module ram#(
    parameter DATA_WIDTH = 8,
    parameter ADDR_WIDTH = 8
)(
    input wire [DATA_WIDTH-1:0] data,
    input wire [ADDR_WIDTH-1:0] read_addr,
    input wire [ADDR_WIDTH-1:0] write_addr,
    input wire we,
    input wire clk,
    output reg [DATA_WIDTH-1:0] out
);

reg [DATA_WIDTH-1:0] mem [1<<ADDR_WIDTH];

always @(posedge clk) begin
    if (we)
        mem[write_addr] = data; // blocking assigment for new data read-during-write behaviour
    out = mem[read_addr];

    if (we) begin
        $display("[ram] wrote %02xh to %06xh", data, write_addr);
    end
end

endmodule
