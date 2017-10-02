#ifndef __MUX_H__
#define __MUX_H__

#include "systemc.h"

SC_MODULE(MUX) {
    sc_in     < bool >         clk;

    sc_in     < sc_uint<2> >   mux_ctrl;
    sc_in     < sc_uint<32> >  ins[3];
    sc_out     < sc_uint<32> >  out;

    void mux() {
        out.write(ins[mux_ctrl.read()].read());
    }

    SC_CTOR(MUX){
        SC_METHOD(mux);
        sensitive << clk.pos();
    }
};

SC_MODULE(DEMUX) {
    sc_in     < bool >         clk;

    sc_in     < sc_uint<2> >   mux_ctrl;
    sc_in     < bool >         in;
    sc_out     < bool >        outs[3];

    void demux() {
        for (int i = 0; i < 3; i++)
            outs[i].write(0);
        outs[mux_ctrl.read()].write(in.read());
    }


    SC_CTOR(DEMUX){
        SC_METHOD(demux);
        sensitive << clk.pos();
    }
};

#endif //__MUX_H__
