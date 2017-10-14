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
        bus_write(0x00, 0xFFFF); // T0 TMR
        bus_write(0x08, 0x02);   // T0 TCONF

        bus_write(0x0C, 0xFFFF); // T1 TMR
        bus_write(0x14, 0x02);   // T1 TCONF

        bus_write(0x18, 0x21);   // ICCONF 010_0001

        unsigned int last = 0, buf[5] = {0};

        for (int i = 0; i < 100; i++) {
            unsigned int ICCONF = (uint) bus_read(0x18, false);
            sc_uint<3> ICM = ICCONF;
            sc_uint<1> ICBNE = ICCONF >> 3;
            sc_uint<1> ICOV = ICCONF >> 4;
            if (ICBNE == 0)
                continue;
            if (ICOV == 1) {
                cout << "IC buffer overflow" << endl;
                sc_stop();
            }
            buf[last++] = (uint) bus_read(0x1C, false);
            cout << "CPU: top of fifo: " << buf[last - 1] << "  " << sc_time_stamp() << endl;
            if (last == 5) {
                if (buf[4] - buf[3] == buf[2] - buf[1] && buf[3] - buf[2] == buf[1] - buf[0]) {
                    cout << "Signal is periodical" << endl;
                } else {
                    cout << "Signal is NOT periodical" << endl;
                }
                for (int j = 0; j < 5; j++){
                    buf[j] = 0;
                }
                last = 0;
            }
        }

        sc_stop();
    }

    int bus_read(int addr, bool dbg=true) {
        int data;

        wait();
        addr_bo.write(addr);
        rd_o.write(true);

        wait();
        rd_o.write(false);

        wait();
        data = (int) data_bi.read();

        if (dbg){
            cout << "CPU: READ  addr: " << setw(2) << hex << addr <<
                              " data: " << setw(4) << hex << data;
            cout << " time " << sc_time_stamp() << endl;
        }

        return data;

    }

    void bus_write(int addr, int data, bool dbg=true) {
        wait();
        addr_bo.write(addr);
        data_bo.write(data);
        wr_o.write(true);

        wait();
        wr_o.write(false);

        if (dbg) {
            cout << "CPU: WRITE addr: " << setw(2) << hex << addr <<
                              " data: " << setw(4) << hex << data;
            cout << " time " << sc_time_stamp() << endl;
        }
    }


    SC_CTOR(CPU){
        SC_CTHREAD(main_thread, clk.pos());
    }
};

#endif //__CPU_H__