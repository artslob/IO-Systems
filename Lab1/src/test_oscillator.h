#ifndef __TEST_OSCILLATOR_H__
#define __TEST_OSCILLATOR_H__

#include "systemc.h"

const int DEFAULT_RATIO = 50;
const int SIGNAL_PERIOD = 40;


SC_MODULE(TEST_OSCILLATOR) {
    sc_in     < bool >         clk;
    sc_out    < bool >         ins;

    SC_HAS_PROCESS(TEST_OSCILLATOR);

    TEST_OSCILLATOR(sc_module_name name, int ratio, sc_trace_file *wf)
            : sc_module(name), counter(0), ratio(ratio) {
        cout << "Test oscillator started with " << ratio << " ratio" << endl;
        left = (unsigned int) ((ratio / 100.0f) * SIGNAL_PERIOD);
        right = (unsigned int) ((100 - ratio) / 100.0f * SIGNAL_PERIOD);
        cout << "left is " << left << " right is " << right << endl;
        cout << "left/2 is " << left / 2 << " right/2 is " << right / 2 << endl;
        SC_CTHREAD(generate, clk.pos());
        sc_trace(wf, counter, "osc_counter"); // TODO: this is debug
    }

    void generate() {
        while (true) {
            if (ratio >= 0) {
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
            }
            else if (ratio == -2 || ratio == -1) {
                ins.write((bool) (ratio + 2));
            }

            wait();
            if (sc_end_of_simulation_invoked())
                break;
        }
    }

private:
    unsigned int counter;
    unsigned int left;
    unsigned int right;
    int ratio;
};

#endif //__TEST_OSCILLATOR_H__
