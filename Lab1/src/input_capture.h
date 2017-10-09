#ifndef __INPUT_CAPTURE_H__
#define __INPUT_CAPTURE_H__

#include "systemc.h"

SC_MODULE(PRESCALER) {
    sc_in     < sc_uint<3> >   ICM;
    sc_in     < bool >         ins;

    sc_out    < bool >         out;

    void prescaler(){
        if (1 <= ICM.read() && ICM.read() <= 3) {
            counter = 0;
            out.write(!out.read());
        }
        if (4 <= ICM.read()){
            counter++;
            if ((ICM.read() == 4 || ICM.read() == 6) && counter == 4) {
                counter = 0;
                out.write(!out.read());
            } else if ((ICM.read() == 5 || ICM.read() == 7) && counter == 16) {
                counter = 0;
                out.write(!out.read());
            }
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

    SC_CTOR(INPUT_CAPTURE) : prescaler("prescaler"){
        SC_METHOD(bus_read);
        sensitive << clk.pos();

        SC_METHOD(bus_write);
        sensitive << clk.pos();

        prescaler.ins(ins);
        prescaler.ICM(ICM);
        prescaler.out(prescaler_out);
    }

//private: TODO: make private again
    PRESCALER prescaler;

    //TODO: mb change to uint
    sc_signal < sc_uint<32> >  ICCONF;  // 0x00 settings
    sc_signal < sc_uint<32> >  ICBUF;   // 0x04 fifo top

    sc_signal < sc_uint<3> >   ICM;

// TODO: remove debug
    sc_signal < bool >         prescaler_out;
};

#endif //__INPUT_CAPTURE_H__
