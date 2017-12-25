`timescale 1ns / 1ps

module Fifo_control(
        input clk,
        input ins,
        input [1:0] ICTMR,
        input [15:0] t_val_bi_0, t_val_bi_1,
        input rd_i,
        
        output reg ICBNE = 0, ICOV = 0,
        output reg [31:0] ICBUF = 0
    );
    
    localparam FIFO_LENGTH = 20;
    reg [31:0] fifo [FIFO_LENGTH -1:0];
    
    integer f_start = 0, f_end = 0, i = 0;
    
    reg prev_ins = 0;
    
    initial begin
        for (i = 0; i < FIFO_LENGTH; i = i + 1)
            fifo[i] = 0;
    end
    
    always@(clk, ins) begin
        if (clk == 1 && rd_i == 1) begin
            if (f_start != f_end) begin
                ICOV <= 0;
                f_end <= (f_end + 1) % 20;
                if (((f_end + 1) % 20) == f_start) begin
                    ICBNE <= 0;
                end
                ICBUF <= fifo[f_end];
            end else begin
                $display("Read from fifo while its empty, time: %d", $time);
                ICBUF <= 0;
            end
        end
        if (ins != prev_ins) begin
            prev_ins <= ins;
            if (ICTMR > 0) begin
                case (ICTMR)
                    1: begin
                        fifo[f_start] <= t_val_bi_0;
                        if (f_start == f_end) ICBUF <= t_val_bi_0;
                    end
                    2: begin
                        fifo[f_start] <= t_val_bi_1;
                        if (f_start == f_end) ICBUF <= t_val_bi_1;
                    end
                    3: begin
                        fifo[f_start] <= (t_val_bi_1 << 16) | t_val_bi_0;
                        if (f_start == f_end) ICBUF <= (t_val_bi_1 << 16) | t_val_bi_0;
                    end
                    default: begin
                        $display("Unknown ICTMR in fifo control: %d time %d", ICTMR, $time);
                    end
                endcase
                
                ICBNE <= 1;
                f_start <= (f_start + 1) % FIFO_LENGTH;
                if (((f_start + 2) % FIFO_LENGTH) == f_end) begin
                    ICOV <= 1;
                end else if (((f_start + 1) % FIFO_LENGTH) == f_end) begin
                    $display("Too fast ins, buffer overflow, time: %d", $time);
                    ICOV <= 1;
                    f_end <= (f_end + 1) % FIFO_LENGTH;
                end
            end
        end
    end
    
endmodule
