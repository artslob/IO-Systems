`timescale 1ns / 1ps

module Demux(
        input [1:0] mux_ctrl,
        input [31:0]  in,
        output reg [31:0] out_0, out_1, out_2
    );
    
    always@(mux_ctrl, in) begin
        case (mux_ctrl)
            0: begin
                out_0 <= in;
                out_1 <= 0;
                out_2 <= 0;
            end
            1: begin
                out_0 <= 0;
                out_1 <= in;
                out_2 <= 0;
            end
            2: begin
                out_0 <= 0;
                out_1 <= 0;
                out_2 <= in;
            end
        endcase
    end
    
endmodule
