#ifndef __INPUT_CAPTURE_H__
#define __INPUT_CAPTURE_H__

#include "systemc.h"

SC_MODULE(PRESCALER) {
    sc_in     < sc_uint<3> >   ICM;
    sc_in     < bool >         ins;

    sc_out    < bool >         out;

    void prescaler(){
        switch (ICM.read()){
            case 1:
                out.write(!out.read());
                counter = 0;
                break;
            case 2:
                if (ins.read() == false)
                    out.write(!out.read());
                counter = 0;
                break;
            case 3:
                if (ins.read() == true)
                    out.write(!out.read());
                counter = 0;
                break;
            case 4:
                if (ins.read() == true)
                    counter++;
                if (counter == 4) {
                    out.write(!out.read());
                    counter = 0;
                }
                break;
            case 5:
                if (ins.read() == true)
                    counter++;
                if (counter == 16) {
                    out.write(!out.read());
                    counter = 0;
                }
                break;
            case 6:
                if (ins.read() == false)
                    counter++;
                if (counter == 4) {
                    out.write(!out.read());
                    counter = 0;
                }
                break;
            case 7:
                if (ins.read() == false)
                    counter++;
                if (counter == 16) {
                    out.write(!out.read());
                    counter = 0;
                }
                break;
            default:
                counter = 0;
                break;
        }
    }

    SC_CTOR(PRESCALER) : counter(0){
        SC_METHOD(prescaler);
        sensitive << ins;
    }

private:
    int counter;
};

SC_MODULE(INPUT_CAPTURE) {
    sc_in     < bool >         clk;

    sc_in     < sc_uint<32> >  addr_bi;
    sc_in     < sc_uint<32> >  data_bi;
    sc_out    < sc_uint<32> >  data_bo;

    sc_in     < bool >         rd_i;
    sc_in     < bool >         wr_i;

    sc_in     < bool >         ins;

    sc_in     < sc_uint<16> >  t_vals_bi[2];

    void bus_read() {
        if (wr_i.read()) {
            switch (addr_bi.read()) {
                case 0x00:
                    ICCONF.write(data_bi.read());
                    ICM.write(data_bi.read());
                    break;
                case 0x04:
                    ICBUF.write(data_bi.read());
                    break;
                default:
                    cout << "IC got unknown address" << endl;
                    break;
            }
        }
    }

    void bus_write() {
        if (rd_i.read()) {
            switch (addr_bi.read()) {
                case 0x00:
                    data_bo.write(ICCONF.read());
                    break;
                case 0x04:
                    //TODO: fifo communication
                    data_bo.write(ICBUF.read());
                    break;
                default:
                    cout << "IC got unknown address" << endl;
                    break;
            }
        }
    }

    SC_HAS_PROCESS(INPUT_CAPTURE);

    INPUT_CAPTURE(sc_module_name name, sc_trace_file *wf)
            : sc_module(name), prescaler("prescaler") {
        SC_METHOD(bus_read);
        sensitive << clk.pos();

        SC_METHOD(bus_write);
        sensitive << clk.pos();

        prescaler.ins(ins);
        prescaler.ICM(ICM);
        prescaler.out(prescaler_out);

        sc_trace(wf, ICM, "ICM");
        sc_trace(wf, prescaler_out, "PRESCALER_OUT");
        sc_trace(wf, ICCONF, "ICCONF");
    }

private:
    PRESCALER prescaler;

    //TODO: mb change to uint
    sc_signal < sc_uint<32> >  ICCONF;  // 0x00 settings
    sc_signal < sc_uint<32> >  ICBUF;   // 0x04 fifo top

    sc_signal < sc_uint<3> >   ICM;

    /* debug signals */
    sc_signal < bool >         prescaler_out;
};

#endif //__INPUT_CAPTURE_H__
