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
        bool filled = false;

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

            buf[last] = (uint) bus_read(0x1C, false);
            cout << "CPU: top of fifo: " << buf[last] << "  " << sc_time_stamp() << endl;
            last = (last + 1) % 5;

            if (!filled && last != 0)
                continue;
            else filled = true;

            if (buf[mod5(last + 4)] - buf[mod5(last + 3)] == buf[mod5(last + 2)] - buf[mod5(last + 1)] &&
                buf[mod5(last + 3)] - buf[mod5(last + 2)] == buf[mod5(last + 1)] - buf[last])
            {
                sc_time period = (buf[mod5(last + 4)] - buf[mod5(last + 2)]) * this->time;
                cout << "Periodical signal: T = " << period;
                cout << "; f = " << 1 / period.to_seconds() << " Hz" << endl;
            } else {
                cout << "Signal is NOT periodical" << endl;
            }
        }

        sc_stop();
    }

    int mod5(int param){
        return param % 5;
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

    SC_HAS_PROCESS(CPU);

    CPU(sc_module_name name, const sc_time& clock_time) : sc_module(name), time(clock_time) {
        SC_CTHREAD(main_thread, clk.pos());
    }

private:
    sc_time time;
};

#endif //__CPU_H__