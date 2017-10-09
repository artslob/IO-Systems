#include "systemc.h"

#include "cpu.h"
#include "bus_matrix.h"
#include "timer.h"
#include "input_capture.h"
#include "test_oscillator.h"

using namespace std;

int get_ratio(int argc, char* argv[]);

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

    sc_signal < sc_uint<16> >  t_vals[2];

    sc_signal < bool >  ins;


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
    timer1.t_val_bo(t_vals[0]);

    TIMER timer2("timer2");
    timer2.clk(clock);
    timer2.addr_bi(slv_addr_bi);
    timer2.data_bi(slv_data_bi);
    timer2.data_bo(slv_data_bo[1]);
    timer2.rd_i(slv_rd_i[1]);
    timer2.wr_i(slv_wr_i[1]);
    timer2.t_val_bo(t_vals[1]);

    INPUT_CAPTURE input_capture("input_capture");
    input_capture.clk(clock);
    input_capture.addr_bi(slv_addr_bi);
    input_capture.data_bi(slv_data_bi);
    input_capture.data_bo(slv_data_bo[2]);
    input_capture.rd_i(slv_rd_i[2]);
    input_capture.wr_i(slv_wr_i[2]);
    input_capture.t_vals_bi[0](t_vals[0]);
    input_capture.t_vals_bi[1](t_vals[1]);
    input_capture.ins(ins);

    TEST_OSCILLATOR test_oscillator("test_oscillator", get_ratio(argc, argv));
    test_oscillator.clk(clock);
    test_oscillator.ins(ins);

    sc_trace_file *wf = sc_create_vcd_trace_file("wave");
    sc_trace(wf, clock, "clk");
    sc_trace(wf, cpu_addr_bo, "cpu_addr_bo");
    sc_trace(wf, cpu_data_bo, "cpu_data_bo");
    sc_trace(wf, cpu_data_bi, "cpu_data_bi");
    sc_trace(wf, cpu_rd_o, "cpu_rd_o");
    sc_trace(wf, cpu_wr_o, "cpu_wr_o");

    sc_trace(wf, slv_addr_bi, "slv_addr_bi");
    sc_trace(wf, slv_data_bi, "slv_data_bi");
    sc_trace(wf, slv_data_bo[0], "T1_data_bo");
    sc_trace(wf, slv_data_bo[1], "T2_data_bo");
    sc_trace(wf, slv_data_bo[2], "ic_data_bo");
    sc_trace(wf, slv_rd_i[0], "T1_rd_i");
    sc_trace(wf, slv_rd_i[1], "T2_rd_i");
    sc_trace(wf, slv_rd_i[2], "ic_rd_i");
    sc_trace(wf, slv_wr_i[0], "T1_wr_i");
    sc_trace(wf, slv_wr_i[1], "T2_wr_i");
    sc_trace(wf, slv_wr_i[2], "ic_wr_i");
    sc_trace(wf, t_vals[0], "T1_value");
    sc_trace(wf, t_vals[1], "T2_value");
    sc_trace(wf, ins, "ins");

    //TODO: remove debug signals, move pointer to trace file to ctors of modules
    sc_trace(wf, test_oscillator.dbg_count, "counter");
    sc_trace(wf, input_capture.ICM, "ICM");
    sc_trace(wf, input_capture.prescaler_out, "PRESCALER_OUT");
    sc_trace(wf, input_capture.ICCONF, "ICCONF");

    sc_start();

    sc_close_vcd_trace_file(wf);

    return 0;
}

int get_ratio(int argc, char *argv[]) {
    if (argc > 1) {
        try {
            int ratio = stoi(argv[1]);
            if (0 <= ratio && ratio <= 100)
                return ratio;
            else {
                cout << "Ratio should be in 0..100, applied to default " << DEFAULT_RATIO << endl;
                return DEFAULT_RATIO;
            }
        } catch (invalid_argument &e) {
            cout << "Wrong ratio parameter, applied to default " << DEFAULT_RATIO << endl;
            return DEFAULT_RATIO;
        }
    }
    return DEFAULT_RATIO;
}
