`timescale 1ns / 1ps

module Fifo_control_calc_v2(
        input clk,
        input ins,
        input [1:0] ICTMR,
        input [15:0] t_val_bi_0, t_val_bi_1,
        input rd_i,
        
        output reg ICBNE = 0, ICOV = 0,
        output reg [31:0] ICBUF = 0
    );
    
    localparam FIFO_LENGTH = 5;
    reg [31:0] fifo [FIFO_LENGTH -1:0];
    
    reg [14:0] period = 0;
    reg prev_ins = 0;
    
    reg [7:0] i;
    initial begin
        for (i = 0; i < FIFO_LENGTH; i = i + 1)
            fifo[i] = 0;
    end
    
    always@(posedge clk) begin
        if (ins != prev_ins) begin
            prev_ins = ins;
            fifo[4] = fifo[3];
            fifo[3] = fifo[2];
            fifo[2] = fifo[1];
            fifo[1] = fifo[0];
            fifo[0] = t_val_bi_0;
            
            if (
                fifo[4] - fifo[3] == fifo[2] - fifo[1]
                &&
                fifo[3] - fifo[2] == fifo[1] - fifo[0]
            ) begin
                period = (fifo[2] - fifo[4]) * 10;
                $display("T: %d, time: %d", period, $time);
                ICBUF = {1'd1, 1'd1, 15'd0, period[14:0]};
            end else begin
                $display("Sygnal is not periodical, time: %d", $time);
                ICBUF = {1'd1, 1'd0, 30'd0};
            end
        end
    end
    
endmodule
