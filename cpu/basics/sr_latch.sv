module sr_latch(
    input wire set,
    input wire rst,
    output wire out
);

/* verilator lint_off MULTIDRIVEN */
reg q;
/* verilator lint_on MULTIDRIVEN */

always_ff @(posedge set or posedge rst)
    if (rst) q <= 0;
    else q <= 1;

//always_ff @(posedge set) q <= 1;
//always_ff @(posedge rst) q <= 0;

assign out = q;

endmodule
