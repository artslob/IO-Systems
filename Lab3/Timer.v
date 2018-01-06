`timescale 1ns / 1ps

module Timer(
        input clk,
        
        input [12:0] addr_bi,
        input [31:0] data_bi,
        output reg [31:0] data_bo,
        
        input rd_i,
        input wr_i,
        
        output reg [15:0] t_val_bo = 0
    );
    
    reg [15:0] TMR = 0, TVAL = 0, TCONF = 0;
    
    always@(posedge clk) begin
        if (wr_i == 1) begin
            case (addr_bi)
                0: begin
                    TMR <= data_bi;
                end
                4: begin
                    TVAL <= data_bi;
                end
                8: begin
                    TCONF <= data_bi;
                end
                default: begin
                    $display("Timer got unknown WRITE address: %d time: %d", addr_bi, $time);
                end
            endcase
        end
        
        if ((TCONF & 2) > 0) begin // running
            if ((TCONF & 1) == 0) begin //incremental
                t_val_bo <= TVAL;
                if (TVAL == TMR) begin
                    TVAL <= 0;
                end else begin
                    TVAL <= TVAL + 1;
                end
            end else begin // decremental
                if (TVAL == 0) begin
                    TVAL <= TMR;
                end else begin
                    TVAL <= TVAL - 1;
                end
                t_val_bo <= TVAL;
            end
        end else begin // not working
            t_val_bo <= TVAL;
        end
        
        if (rd_i == 1) begin
            case (addr_bi)
                0: begin
                    data_bo <= TMR;
                end
                4: begin
                    data_bo <= TVAL;
                end
                8: begin
                    data_bo <= TCONF;
                end
                default: begin
                    $display("Timer got unknown READ address: %d time: %d", addr_bi, $time);
                end
            endcase
        end
    end
    
endmodule
