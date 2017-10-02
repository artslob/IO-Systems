#ifndef __BUS_MATRIX__
#define __BUS_MATRIX__

#include "mux.h"
#include "systemc.h"

SC_MODULE(DECODER) {
    sc_in     < sc_uint<32> >  addr_bi;
    sc_out    < sc_uint<32> >  addr_bo;
    sc_out    < sc_uint<2>  >  mux_ctrl;

    void decode() {
        sc_uint<32> addr = addr_bi.read();
        if (0 <= addr && addr < 12) {
            addr_bo.write(addr);
            mux_ctrl.write(0);
        }
        else if (12 <= addr && addr < 24) {
            addr_bo.write(addr - 12);
            mux_ctrl.write(1);
        }
        else if (24 <= addr && addr <= 28) {
            addr_bo.write(addr - 24);
            mux_ctrl.write(2);
        }
    }

    SC_CTOR(DECODER) {
        SC_METHOD(decode);
        sensitive << addr_bi;
    }
};

SC_MODULE(BUS_MATRIX) {
    sc_in     < bool >         clk;

    sc_in     < sc_uint<32> >  addr_bi;
    sc_out    < sc_uint<32> >  addr_bo;

    sc_in     < sc_uint<32> >  m_data_bi;
    sc_out    < sc_uint<32> >  m_data_bo;

    sc_in     < sc_uint<32> >  s_data_bi[3];
    sc_out    < sc_uint<32> >  s_data_bo;

    sc_in     < bool >         rd_i;
    sc_out    < bool >         rd_o[3];

    sc_in     < bool >         wr_i;
    sc_out    < bool >         wr_o[3];

    SC_HAS_PROCESS(BUS_MATRIX);

    BUS_MATRIX(sc_module_name name): sc_module(name),
        rd_demux("rd_demux"), wr_demux("wr_demux"), data_mux("data_mux"), decoder("decoder")
    {
        decoder.addr_bi(addr_bi);
        decoder.addr_bo(addr_bo);
        decoder.mux_ctrl(mux_ctrl);

        rd_demux.clk(clk);
        rd_demux.mux_ctrl(mux_ctrl);
        rd_demux.in(rd_i);
        for (int i = 0; i < 3; i++)
            rd_demux.outs[i](rd_o[i]);

        wr_demux.clk(clk);
        wr_demux.mux_ctrl(mux_ctrl);
        wr_demux.in(wr_i);
        for (int i = 0; i < 3; i++)
            wr_demux.outs[i](wr_o[i]);

        data_mux.clk(clk);
        data_mux.mux_ctrl(mux_ctrl);
        data_mux.out(s_data_bo);
        for (int i = 0; i < 3; i++)
            data_mux.ins[i](s_data_bi[i]);

        SC_METHOD(write_data);
        sensitive << clk.pos();
    }

private:
    DEMUX rd_demux;
    DEMUX wr_demux;
    MUX data_mux;
    DECODER decoder;

    sc_signal < sc_uint<2>  >  mux_ctrl;

    void write_data() {
        m_data_bo.write(m_data_bi.read());
    }
};

#endif //__BUS_MATRIX__