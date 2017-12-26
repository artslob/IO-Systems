`timescale 1ns / 1ps

module Fifo_control_calc(
        input clk,
        input ins,
        input [1:0] ICTMR,
        input [15:0] t_val_bi_0, t_val_bi_1,
        input rd_i,
        
        output reg ICBNE = 0, ICOV = 0,
        output reg [31:0] ICBUF = 0
    );
    
    localparam true = 1;
    localparam false = 0;
    
    localparam FIFO_LENGTH = 5;
    reg [31:0] fifo [FIFO_LENGTH -1:0];
    
    integer i = 0, last = 0;
    integer filled = false; 
    
    integer period = 0, frequency = 0;
    
    
    initial begin
        for (i = 0; i < FIFO_LENGTH; i = i + 1)
            fifo[i] = 0;
    end
    
    always@(ins) begin
        if (ICTMR > 0) begin
            fifo[last] = t_val_bi_0;
            last = (last + 1) % 5;
            
            if (filled == false && last != 0) begin
                ICBUF = 0;
            end else begin
                filled = true;
                
                if (
                    fifo[(last + 4) % 5] - fifo[(last + 3) % 5] == fifo[(last + 2) % 5] - fifo[(last + 1) % 5]
                    &&
                    fifo[(last + 3) % 5] - fifo[(last + 2) % 5] == fifo[(last + 1) % 5] - fifo[last]
                ) begin
                    period = (fifo[(last + 4) % 5] - fifo[(last + 2) % 5]) * 10;
                    frequency = 1 / period;
                    $display("T: %d, f: %d, time: %d", period, frequency, $time);
                    ICBUF = {1'd1, 1'd1, frequency[14:0], period[14:0]};
                end else begin
                    ICBUF = {1'd1, 1'd0, 30'd0};
                end
            end
        end
    end
    
endmodule
