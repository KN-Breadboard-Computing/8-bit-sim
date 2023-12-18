module tristate_buffer (
    input [width-1: 0] data_in,
    input enable,
    output [width-1: 0] data_out
);
    parameter width = 8;

    assign data_out = enable ? data_in : {width{1'bz}};
endmodule
