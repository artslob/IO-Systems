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

    void bus_read() {
        if (wr_i.read()) {
            data = data_bi.read();
            cout << data;
        }
    }

    void bus_write() {
        if (rd_i.read())
            data_bo.write(data);
    }

    SC_CTOR(TIMER){
        data = 0;

        SC_METHOD(bus_read);
        sensitive << clk.pos();

        SC_METHOD(bus_write);
        sensitive << clk.pos();
    }

private:
    int data;
};

#endif //__TIMER_H__