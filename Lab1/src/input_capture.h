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

SC_MODULE(FIFO_CONTROL){
    sc_in     < bool >         ins;
    sc_in     < sc_uint<2>  >  ICTMR;
    sc_in     < sc_uint<16> >  t_vals_bi[2];

    sc_out    < bool >         ICBNE; // 0 empty, 1 not empty
    sc_out    < bool >         ICOV;  // 0 not full, 1 full
    sc_out    < sc_uint<32> >  ICBUF;

    SC_CTOR(FIFO_CONTROL) : start(0), end(0) {
        SC_METHOD(capture);
        sensitive << ins;
    }

    sc_uint<32> read_data(){
        if (start != end) {
            sc_uint<32> return_value = buf[end];
            end = (end + 1) % 20;
            ICOV.write(false);
            if (end == start)
                ICBNE.write(false);
            return return_value;
        } else {
            cout << "Read from fifo while its empty" << endl;
            return 0;
        }
    }

private:
    sc_uint<32> buf[20];
    unsigned int start, end;

    void capture(){
        if (ICTMR.read() > 0){
            switch (ICTMR.read()){
                case 1:
                    buf[start] = t_vals_bi[0].read();
                    break;
                case 2:
                    buf[start] = t_vals_bi[1].read();
                    break;
                case 3:
                    buf[start] = (t_vals_bi[1].read() << 16) | t_vals_bi[0].read();
                    break;
                default:
                    cout << "Unknown ICTMR in fifo control" << endl;
                    break;
            }
            ICBUF.write(buf[start]);
            start = (start + 1) % 20;
            ICBNE.write(true);
            if ((start + 1) % 20 == end){
                ICOV.write(true);
            } else if (start == end) {
                cout << "Too fast ins, buffer overflow" << endl;
                ICOV.write(true);
                end++;
            }
        }
    }
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
            if (addr_bi.read() == 0x00) {
                unsigned int ICCONF = (unsigned int) data_bi.read();
                ICM.write(ICCONF);
                ICTMR.write(ICCONF >> 5);
            } else {
                cout << "IC got unknown address" << endl;
            }
        }
    }

    void bus_write() {
        if (rd_i.read()) {
            switch (addr_bi.read()) {
                case 0x00: {
                    unsigned int ICCONF = (unsigned int)
                        (ICTMR.read() << 5 | ICOV.read() << 4 | ICBNE.read() << 3 | ICM.read());
                    data_bo.write(ICCONF);
                    break;
                }
                case 0x04:
                    data_bo.write(fifo.read_data());
                    break;
                default:
                    cout << "IC got unknown address" << endl;
                    break;
            }
        }
    }

    SC_HAS_PROCESS(INPUT_CAPTURE);

    INPUT_CAPTURE(sc_module_name name, sc_trace_file *wf)
            : sc_module(name), prescaler("prescaler"), fifo("fifo") {
        SC_METHOD(bus_read);
        sensitive << clk.pos();

        SC_METHOD(bus_write);
        sensitive << clk.pos();

        prescaler.ins(ins);
        prescaler.ICM(ICM);
        prescaler.out(prescaler_out);

        fifo.ins(prescaler_out);
        fifo.ICTMR(ICTMR);
        fifo.t_vals_bi[0](t_vals_bi[0]);
        fifo.t_vals_bi[1](t_vals_bi[1]);
        fifo.ICBNE(ICBNE);
        fifo.ICOV(ICOV);
        fifo.ICBUF(ICBUF);

        sc_trace(wf, prescaler_out, "PRESCALER_OUT");
        sc_trace(wf, ICM, "ICM");
        sc_trace(wf, ICBNE, "ICBNE");
        sc_trace(wf, ICOV, "ICOV");
        sc_trace(wf, ICTMR, "ICTMR");
        sc_trace(wf, ICBUF, "ICBUF");
    }

private:
    PRESCALER prescaler;
    FIFO_CONTROL fifo;

    // 0x00 ICCONF - settings
    sc_signal < sc_uint<3>  >  ICM;   // bits 0-2
    sc_signal < bool >         ICBNE; // bit  3
    sc_signal < bool >         ICOV;  // bit  4
    sc_signal < sc_uint<2>  >  ICTMR; // bits 5-6

    // 0x04 ICBUF - fifo top
    sc_signal < sc_uint<32> >  ICBUF;

    sc_signal < bool >         prescaler_out;
};

#endif //__INPUT_CAPTURE_H__
