#ifndef __TEST_OSCILLATOR_H__
#define __TEST_OSCILLATOR_H__

#include "systemc.h"

const int DEFAULT_RATIO = 50;
const int SIGNAL_PERIOD = 20;


SC_MODULE(TEST_OSCILLATOR) {
    sc_in     < bool >         clk;
    sc_out    < bool >         ins;

    sc_out    < sc_uint<32> >  dbg_count;

    SC_HAS_PROCESS(TEST_OSCILLATOR);

    TEST_OSCILLATOR(sc_module_name name, int ratio) : sc_module(name), counter(0) {
        cout << "Test oscillator started with " << ratio << " ratio" << endl;
        left = (unsigned int) ((ratio / 100.0f) * SIGNAL_PERIOD);
        right = (unsigned int) ((100 - ratio) / 100.0f * SIGNAL_PERIOD);
        cout << "left is " << left << " right is " << right << endl;
        cout << "left/2 is " << left / 2 << " right/2 is " << right / 2 << endl;
        SC_CTHREAD(generate, clk.pos());
    }

    void generate() {
        while (true) {
            dbg_count.write(counter);
            if (counter < left / 2)
                ins.write(false);
            else if (left / 2 <= counter && counter < left)
                ins.write(true);
            else if (left <= counter && counter < (left + right / 2))
                ins.write(false);
            else if ((left + right / 2) <= counter && counter < left + right)
                ins.write(true);
            if (counter == left + right - 1) {
                ins.write(true);
                counter = 0;
            }
            else counter++;
            wait();
            if (sc_end_of_simulation_invoked())
                break;
        }
    }

private:
    unsigned int counter;
    unsigned int left;
    unsigned int right;
};

#endif //__TEST_OSCILLATOR_H__