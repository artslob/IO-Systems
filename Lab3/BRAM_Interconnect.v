`timescale 1ns / 1ps

module BRAM_Interconnect(
        input clk,
        input rst_i,
        
        input [12:0] addr_bi,
        output [12:0] addr_bo,

        input [31:0] m_data_bi,
        output [31:0] m_data_bo,
        
        input [31:0] s_data_bi_0, s_data_bi_1, s_data_bi_2,
        output [31:0] s_data_bo,
        
        input en_i,
        input [3:0] we_bi,
        
        output rd_o_0, rd_o_1, rd_o_2,
        
        output wr_o_0, wr_o_1, wr_o_2
    );
    
    wire [1:0] mux_ctrl;
    wire rd_i, wr_i;
    
    assign m_data_bo = m_data_bi;
    
    assign rd_i = (rst_i == 0 && en_i == 1 && we_bi == 0) ? 1 : 0;
    assign wr_i = (rst_i == 0 && en_i == 1 && we_bi == 4'hF) ? 1 : 0;
    
    Decoder decoder (
        .addr_bi(addr_bi),
        .addr_bo(addr_bo),
        .mux_ctrl(mux_ctrl)
    );
    
    Mux data_mux (
        .mux_ctrl(mux_ctrl),
        .in_0(s_data_bi_0),
        .in_1(s_data_bi_1),
        .in_2(s_data_bi_2),
        .out(s_data_bo)
    );
    
    Demux rd_demux (
        .mux_ctrl(mux_ctrl),
        .in(rd_i),
        .out_0(rd_o_0),
        .out_1(rd_o_1),
        .out_2(rd_o_2)
    );
    
    Demux wr_demux (
        .mux_ctrl(mux_ctrl),
        .in(wr_i),
        .out_0(wr_o_0),
        .out_1(wr_o_1),
        .out_2(wr_o_2)
    );
    
endmodule
