`timescale 1ns / 1ps

module Timer(
        input clk,
        
        input [12:0] addr_bi,
        input [31:0] data_bi,
        output [31:0] data_bo,
        
        input rd_i,
        input wr_i,
        
        output [15:0] t_val_bo
    );
endmodule
