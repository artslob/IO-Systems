#ifndef __CPU_H__
#define __CPU_H__

#include "systemc.h"

#include <iomanip>

using std::setw;

SC_MODULE(CPU) {
    sc_in     < bool >         clk;

    sc_out    < sc_uint<32> >  addr_bo;
    sc_out    < sc_uint<32> >  data_bo;
    sc_in     < sc_uint<32> >  data_bi;

    sc_out    < bool >         rd_o;
    sc_out    < bool>          wr_o;

    void main_thread() {
        bus_write(0x00, 0x03); // T0 TMR
        bus_write(0x08, 0x02); // T0 TCONF

        bus_write(0x0C, 0x03); // T1 TMR
        bus_write(0x14, 0x02); // T1 TCONF

        bus_read(0x04); // T1 TVAL

        bus_write(0x18, 0x24);  // ICCONF
        bus_read(0x18);

        for (int i = 0; i < 200; i++)
            wait();
        sc_stop();
    }

    int bus_read(int addr) {
        int data;

        wait();
        addr_bo.write(addr);
        rd_o.write(true);

        wait();
        rd_o.write(false);

        wait();
        data = (int) data_bi.read();

        cout << "CPU: READ  addr: " << setw(2) << hex << addr << " data: " << setw(2) << hex << data;
        cout << " time " << sc_time_stamp() << endl;

        return data;

    }

    void bus_write(int addr, int data) {
        wait();
        addr_bo.write(addr);
        data_bo.write(data);
        wr_o.write(true);

        wait();
        wr_o.write(false);

        cout << "CPU: WRITE addr: " << setw(2) << hex << addr << " data: " << setw(2) << hex << data;
        cout << " time " << sc_time_stamp() << endl;
    }


    SC_CTOR(CPU){
        SC_CTHREAD(main_thread, clk.pos());
    }
};

#endif //__CPU_H__