#ifndef __TIMER_H__
#define __TIMER_H__

#include "systemc.h"

SC_MODULE(TIMER) {
    sc_in     < bool >         clk;

    sc_in     < sc_uint<32> >  addr_bi;
    sc_in     < sc_uint<32> >  data_bi;
    sc_out    < sc_uint<32> >  data_bo;

    sc_in     < bool >         rd_i;
    sc_in     < bool >         wr_i;

    sc_out    < sc_uint<16> >  t_val_bo;

    void bus_read() {
        if (wr_i.read()) {
            switch (addr_bi.read()) {
                case 0x00:
                    TMR = (unsigned int) data_bi.read();
                    break;
                case 0x04:
                    TVAL = (unsigned int) data_bi.read();
                    break;
                case 0x08:
                    TCONF = (unsigned int) data_bi.read();
                    break;
                default:
                    cout << "Timer got unknown address" << endl;
                    break;
            }
        }
    }

    void bus_write() {
        if (rd_i.read()) {
            switch (addr_bi.read()) {
                case 0x00:
                    data_bo.write(TMR);
                    break;
                case 0x04:
                    data_bo.write(TVAL);
                    break;
                case 0x08:
                    data_bo.write(TCONF);
                    break;
                default:
                    cout << "Timer got unknown address" << endl;
                    break;
            }
        }
    }

    SC_CTOR(TIMER){
        SC_METHOD(bus_read);
        sensitive << clk.pos();

        SC_METHOD(bus_write);
        sensitive << clk.pos();

        SC_METHOD(timer);
        sensitive << clk.pos();
    }

private:
    unsigned int TMR;   // initialization value
    unsigned int TVAL;  // current value
    unsigned int TCONF; // settings

    void timer(){
        if (is_running()) {
            if (is_incremental()) {
                t_val_bo.write(TVAL);
                if (TVAL == TMR)
                    TVAL = 0;
                else
                    TVAL++;

            } else {
                if (TVAL == 0)
                    TVAL = TMR;
                else
                    TVAL--;
                t_val_bo.write(TVAL);
            }
        } else {
            t_val_bo.write(TVAL);
        }
    }

    bool is_running() {
        return (TCONF & 0x2) > 0;
    }

    bool is_incremental() {
        return (TCONF & 0x1) == 0;
    }
};

#endif //__TIMER_H__