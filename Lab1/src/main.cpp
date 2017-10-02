#include "systemc.h"

#include "cpu.h"
#include "bus_matrix.h"
#include "timer.h"

int main(int argc, char* argv[]) {
    return sc_main(argc, argv);
}

int sc_main(int argc, char* argv[]){
    sc_clock clock("clock", sc_time(10, SC_NS), 0.5, sc_time(5, SC_NS));

    /* cpu signals */
    sc_signal < sc_uint<32> >  cpu_addr_bo;

    sc_signal < sc_uint<32> >  cpu_data_bo;
    sc_signal < sc_uint<32> >  cpu_data_bi;

    sc_signal < bool >         cpu_rd_o;
    sc_signal < bool >         cpu_wr_o;

    /* slaves signals */
    sc_signal < sc_uint<32> >  slv_addr_bi;

    sc_signal < sc_uint<32> >  slv_data_bi;
    sc_signal < sc_uint<32> >  slv_data_bo[3];

    sc_signal < bool >         slv_rd_i[3];
    sc_signal < bool >         slv_wr_i[3];


    CPU cpu("cpu");
    cpu.clk(clock);
    cpu.addr_bo(cpu_addr_bo);
    cpu.data_bo(cpu_data_bo);
    cpu.data_bi(cpu_data_bi);
    cpu.rd_o(cpu_rd_o);
    cpu.wr_o(cpu_wr_o);

    BUS_MATRIX bus_matrix("bus_matrix");
    bus_matrix.clk(clock);
    bus_matrix.addr_bi(cpu_addr_bo);
    bus_matrix.addr_bo(slv_addr_bi);
    bus_matrix.m_data_bi(cpu_data_bo);
    bus_matrix.m_data_bo(slv_data_bi);
    bus_matrix.s_data_bi[0](slv_data_bo[0]);
    bus_matrix.s_data_bi[1](slv_data_bo[1]);
    bus_matrix.s_data_bi[2](slv_data_bo[2]);
    bus_matrix.s_data_bo(cpu_data_bi);
    bus_matrix.rd_i(cpu_rd_o);
    bus_matrix.rd_o[0](slv_rd_i[0]);
    bus_matrix.rd_o[1](slv_rd_i[1]);
    bus_matrix.rd_o[2](slv_rd_i[2]);
    bus_matrix.wr_i(cpu_wr_o);
    bus_matrix.wr_o[0](slv_wr_i[0]);
    bus_matrix.wr_o[1](slv_wr_i[1]);
    bus_matrix.wr_o[2](slv_wr_i[2]);

    TIMER timer1("timer1");
    timer1.clk(clock);
    timer1.addr_bi(slv_addr_bi);
    timer1.data_bi(slv_data_bi);
    timer1.data_bo(slv_data_bo[0]);
    timer1.rd_i(slv_rd_i[0]);
    timer1.wr_i(slv_wr_i[0]);

    TIMER timer2("timer2");
    timer2.clk(clock);
    timer2.addr_bi(slv_addr_bi);
    timer2.data_bi(slv_data_bi);
    timer2.data_bo(slv_data_bo[1]);
    timer2.rd_i(slv_rd_i[1]);
    timer2.wr_i(slv_wr_i[1]);

    TIMER timer3("timer3");
    timer3.clk(clock);
    timer3.addr_bi(slv_addr_bi);
    timer3.data_bi(slv_data_bi);
    timer3.data_bo(slv_data_bo[2]);
    timer3.rd_i(slv_rd_i[2]);
    timer3.wr_i(slv_wr_i[2]);

    sc_trace_file *wf = sc_create_vcd_trace_file("wave");
    sc_trace(wf, clock, "clk");
    sc_trace(wf, cpu_addr_bo, "cpu_addr_bo");
    sc_trace(wf, cpu_data_bo, "cpu_data_bo");
    sc_trace(wf, cpu_data_bi, "cpu_data_bi");
    sc_trace(wf, cpu_rd_o, "cpu_rd_o");
    sc_trace(wf, cpu_wr_o, "cpu_wr_o");

    sc_trace(wf, slv_addr_bi, "slv_addr_bi");
    sc_trace(wf, slv_data_bi, "slv_data_bi");
    sc_trace(wf, slv_data_bo[0], "slv_data_bo_0");
    sc_trace(wf, slv_data_bo[1], "slv_data_bo_1");
    sc_trace(wf, slv_data_bo[2], "slv_data_bo_2");
    sc_trace(wf, slv_rd_i[0], "slv_rd_i_0");
    sc_trace(wf, slv_rd_i[1], "slv_rd_i_1");
    sc_trace(wf, slv_rd_i[2], "slv_rd_i_2");
    sc_trace(wf, slv_wr_i[0], "slv_wr_i_0");
    sc_trace(wf, slv_wr_i[1], "slv_wr_i_1");
    sc_trace(wf, slv_wr_i[2], "slv_wr_i_2");

    sc_start();

    sc_close_vcd_trace_file(wf);

    return 0;
}
