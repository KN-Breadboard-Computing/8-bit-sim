module alu(
    input [3:0] a,
    input [3:0] b,
    input op,  // operation: 0 for add, 1 for subtract
    output reg [4:0] result
);

    always @ (a, b, op)
    begin
        if (op == 0) 
            result = a + b;
        else 
            result = a - b;
    end

endmodule

