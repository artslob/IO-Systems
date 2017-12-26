`timescale 1ns / 1ps

module InputCapture(
        input clk,
        input rst_i,
        
        input [12:0] addr_bi,
        input [31:0] data_bi,
        output reg [31:0] data_bo,
        
        input rd_i,
        input wr_i,
        
        input ins,
        input [15:0] t_val_bi_0, t_val_bi_1
    );
    
    reg [2:0] ICM = 0;
    reg [1:0] ICTMR = 0;
    
    wire prescaler_out;
    wire ICBNE, ICOV;
    wire [31:0] ICBUF;
    wire fifo_rd_i;
    
    assign fifo_rd_i = (rd_i == 1 && addr_bi == 4) ? 1 : 0;
    
    Prescaler prescaler(
        .ins(ins),
        .rst_i(rst_i),
        .ICM(ICM),
        .out(prescaler_out)
    );
    
    Fifo_control_calc fifo(
        .clk(clk),
        .ins(prescaler_out),
        .rd_i(fifo_rd_i),
        .ICTMR(ICTMR),
        .t_val_bi_0(t_val_bi_0),
        .t_val_bi_1(t_val_bi_1),
        .ICBNE(ICBNE),
        .ICOV(ICOV),
        .ICBUF(ICBUF)
    );
    
    always@(posedge clk) begin
        if (wr_i == 1) begin
            case (addr_bi)
                0: begin
                    ICM <= (data_bi & 7);
                    ICTMR <= ((data_bi >> 5) & 3);
                end
                default: begin
                    $display("Input Capture got unknown WRITE address: %d time: %d", addr_bi, $time);
                end
            endcase
        end
        
        if (rd_i == 1) begin
            case (addr_bi)
                0: begin
                    data_bo <= (ICTMR << 5 | ICOV << 4 | ICBNE << 3 | ICM);
                end
                4: begin
                    // TODO: implement fifo read
                    data_bo <= ICBUF;
                end
                default: begin
                    $display("Input Capture got unknown READ address: %d time: %d", addr_bi, $time);
                end
            endcase
        end
    end
    
endmodule
