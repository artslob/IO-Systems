`timescale 1ns / 1ps

module Mux(
        input [1:0] mux_ctrl,
        input [31:0]  in_0, in_1, in_2,
        output reg [31:0] out
    );
    
    always@(mux_ctrl, in_0, in_1, in_2) begin
        case (mux_ctrl)
            0: begin
                out <= in_0;
            end
            1: begin
                out <= in_1;
            end
            2: begin
                out <= in_2;
            end
        endcase
    end
    
endmodule
