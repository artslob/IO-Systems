#ifndef __INPUT_CAPTURE_H__
#define __INPUT_CAPTURE_H__

#include "systemc.h"

SC_MODULE(INPUT_CAPTURE) {
    sc_in     < bool >         clk;

    sc_in     < sc_uint<32> >  addr_bi;
    sc_in     < sc_uint<32> >  data_bi;
    sc_out    < sc_uint<32> >  data_bo;

    sc_in     < bool >         rd_i;
    sc_in     < bool >         wr_i;

    sc_in     < bool >         ins;

    sc_in     < sc_uint<16> >  t_vals_bi[2];


    SC_CTOR(INPUT_CAPTURE) {

    }

};

#endif //__INPUT_CAPTURE_H__
