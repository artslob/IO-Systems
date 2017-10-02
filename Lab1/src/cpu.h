#ifndef __CPU_H__
#define __CPU_H__

#include "systemc.h"

SC_MODULE(CPU) {
    sc_in     < bool >         clk;

    sc_out    < sc_uint<32> >  addr_bo;
    sc_out    < sc_uint<32> >  data_bo;
    sc_in     < sc_uint<32> >  data_bi;

    sc_out    < bool >         rd_o;
    sc_out    < bool>          wr_o;

    void main_thread() {
        bus_write(0x01, 13);
        bus_read(0x00);

        sc_stop();
    }

    int bus_read(int addr) {
        int data;

        wait();
        addr_bo.write(addr);
        rd_o.write(1);

        wait();
        rd_o.write(0);

        wait();
        data = data_bi.read();

        cout << "MIPS32: READ " << endl;
        cout << "  -> addr: " << hex << addr << endl;
        cout << "  -> data: " << hex << data << endl;

        return data;

    }

    void bus_write(int addr, int data) {
        wait();
        addr_bo.write(addr);
        data_bo.write(data);
        wr_o.write(1);

        wait();
        wr_o.write(0);

        cout << "MIPS32: WRITE " << endl;
        cout << "  -> addr: " << hex << addr << endl;
        cout << "  -> data: " << hex << data << endl;
    }


    SC_CTOR(CPU){
        SC_CTHREAD(main_thread, clk.pos());
    }
};

#endif //__CPU_H__