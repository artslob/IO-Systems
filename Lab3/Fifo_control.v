`timescale 1ns / 1ps

module Fifo_control(
    input ins,
    input [1:0] ICTMR,
    input [15:0] t_val_bi_0, t_val_bi_1,
    input rd_i,
    
    output ICBNE, ICOV,
    output [31:0] ICBUF
    );
    
endmodule
