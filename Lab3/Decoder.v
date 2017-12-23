`timescale 1ns / 1ps

module Decoder(
        input [12:0] addr_bi,
        output reg [12:0] addr_bo,
        output reg [1:0] mux_ctrl
    );
    
    always@(addr_bi) begin
        if (0 <= addr_bi && addr_bi < 12) begin
            addr_bo <= addr_bi;
            mux_ctrl <= 0;
        end
        else if (12 <= addr_bi && addr_bi < 24) begin
            addr_bo <= addr_bi - 12;
            mux_ctrl <= 1;
        end
        else if (24 <= addr_bi && addr_bi <= 28) begin
            addr_bo <= addr_bi - 24;
            mux_ctrl <= 2;
        end
    end
    
endmodule
