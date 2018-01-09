`timescale 1ns / 1ps

module Prescaler(
        input ins,
        input rst_i,
        input [2:0] ICM,
        output reg out = 0
    );
    
    reg [31:0] counter = 0;
    
    always@(rst_i, ins) begin
        if (rst_i == 1) begin
            out <= 0;
        end else begin
            case (ICM)
                1: begin
                    out <= ~out;
                    counter <= 0;
                end
                2: begin
                    if (ins == 0)
                        out <= ~out;
                    counter <= 0;
                end
                3: begin
                    if (ins == 1)
                        out <= ~out;
                    counter <= 0;
                end
                4: begin
                    if (ins == 1)
                        counter = counter + 1;
                    if (counter >= 4) begin
                        out <= ~out;
                        counter <= 0;
                    end
                end
                5: begin
                    if (ins == 1)
                        counter = counter + 1;
                    if (counter >= 16) begin
                        out <= ~out;
                        counter <= 0;
                    end
                end
                6: begin
                    if (ins == 0)
                        counter = counter + 1;
                    if (counter >= 4) begin
                        out <= ~out;
                        counter <= 0;
                    end
                end
                7: begin
                    if (ins == 0)
                        counter = counter + 1;
                    if (counter >= 16) begin
                        out <= ~out;
                        counter <= 0;
                    end
                end
                default: begin
                    counter <= 0;
                    out <= out;
                end
            endcase
        end
    end
    
endmodule
