#include <xil_io.h>
#include <stdlib.h>
//#include <stdio.h>

#define GPIO      0x40000000

/* AXI Timer register addresses */
#define AXI_TIMER 0x41C00000
#define TCSR0     0x41C00000  // Timer 0 Control and Status Register
#define TLR0      0x41C00004  // Timer 0 Load Register
#define TCR0      0x41C00008  // Timer 0 Counter Register
#define TCSR1     0x41C00010  // Timer 1 Control and Status Register
#define TLR1      0x41C00014  // Timer 1 Load Register
#define TCR1      0x41C00018  // Timer 1 Counter Register

#define UART      0x40600000
#define UART_RX   0x40600000
#define UART_TX   0x40600004
#define UART_ST   0x40600008
#define UART_CT   0x4060000C

#define AXI_BRAM  0xC0000000
/* Timer 0 */
#define T0_TMR    0xC0000000  // Timer 0 TMR
#define T0_TVAL   0xC0000004  // Timer 0 TVAL
#define T0_TCONF  0xC0000008  // Timer 0 TCONF
/* Timer 1 */
#define T1_TMR    0xC000000C  // Timer 1 TMR
#define T1_TVAL   0xC0000010  // Timer 1 TVAL
#define T1_TCONF  0xC0000014  // Timer 1 TCONF
/* Input Capture */
#define ICCONF    0xC0000018  // IC ICCONF
#define ICBUF     0xC000001C  // IC ICBUF

void wait(unsigned int tacts) {
	for (volatile int i = 0; i < tacts; i++);
}

unsigned char mod5(unsigned char param) {
	return param % 5;
}

void print_period(unsigned int period) {
	char str[17] = {0};
	itoa(period, str, 10);
	//sprintf(str, "%d %.2f\n", period, (float)(1.0f / (float) period));
	Xil_Out32(UART_CT, 0b1);
	char * p = str;
	while(*p){
		Xil_Out32(UART_TX, *p++);
	}
}

void print_non_periodic() {
	Xil_Out32(UART_CT, 0b1);
	char * p = "Non periodic\n";
	while(*p){
		Xil_Out32(UART_TX, *p++);
	}
}

unsigned int get_tlr_value(unsigned int period) {
	return 0xFFFFFFFF - (period / 10 - 2);
}

#define PERIODS_LENGTH 3

int fifo_calc() {
	Xil_Out32(ICCONF, 0b0100001);
	Xil_Out32(T0_TMR, 0xFFFF);
	Xil_Out32(T0_TCONF, 0b10);

	unsigned int TIMES[] = {500, 600, 700};

	unsigned int PERIODS[] = {
			get_tlr_value( TIMES[0] ),
			get_tlr_value( TIMES[1] ),
			get_tlr_value( TIMES[2] )
	};

	unsigned int HIGH_TIMES[] = {
			get_tlr_value( TIMES[0] / 2 ),
			get_tlr_value( TIMES[1] / 2 ),
			get_tlr_value( TIMES[2] / 2 )
	};

	Xil_Out32(TLR0, PERIODS[0]     );    // pwm period
	Xil_Out32(TLR1, HIGH_TIMES[0]  );    // pwm high time

	Xil_Out32(TCSR0, 0b000000100000);    // LOAD0=1
	Xil_Out32(TCSR1, 0b000000100000);    // LOAD1=1

	Xil_Out32(TCSR0, 0b001010000100);    // GENT0=1, ENT0=1, PWMA0=1
	Xil_Out32(TCSR1, 0b001010000100);    // GENT1=1, ENT1=1, PWMB0=1

	unsigned int icbuf = 0, period = 0, prev_period = 0;
	unsigned char non_periodical = 0;

	while(1) {
		for (int i = 0; i < PERIODS_LENGTH; i++) {
			Xil_Out32(TLR0, PERIODS[i]    ); // pwm period
			Xil_Out32(TLR1, HIGH_TIMES[i] ); // pwm high time
//			Xil_Out32(GPIO, TIMES[0]);

			for (int j = 0; j < 10; j++) {
				icbuf = Xil_In32(ICBUF);
				if ((icbuf & 0x80000000) == 0) {
					continue;
				}
				if ((icbuf & 0x40000000) == 0) {
					if (non_periodical == 0) {
						non_periodical = 1;
						Xil_Out32(GPIO, 0);
						print_non_periodic();
					}
				}
				else {
					period = icbuf & 0x7FFF;
					non_periodical = 0;
					if (period != prev_period) {
						prev_period = period;
						Xil_Out32(GPIO, period);
						print_period(period);
					}
				}
			}
		}
	}

	return 0;
}

/*int fifo() {
	Xil_Out32(ICCONF, 0b0100001);
	Xil_Out32(T0_TMR, 0xFFFF);
	Xil_Out32(T0_TCONF, 0b10);

	unsigned int TIMES[] = {30000, 35000, 40000};

	unsigned int PERIODS[] = {
			get_tlr_value( TIMES[0] ),
			get_tlr_value( TIMES[1] ),
			get_tlr_value( TIMES[2] )
	};

	unsigned int HIGH_TIMES[] = {
			get_tlr_value( TIMES[0] / 2 ),
			get_tlr_value( TIMES[1] / 2 ),
			get_tlr_value( TIMES[2] / 2 )
	};

	Xil_Out32(TLR0, PERIODS[0]     );    // pwm period
	Xil_Out32(TLR1, HIGH_TIMES[0]  );    // pwm high time

	Xil_Out32(TCSR0, 0b000000100000);    // LOAD0=1
	Xil_Out32(TCSR1, 0b000000100000);    // LOAD1=1

	Xil_Out32(TCSR0, 0b001010000100);    // GENT0=1, ENT0=1, PWMA0=1
	Xil_Out32(TCSR1, 0b001010000100);    // GENT1=1, ENT1=1, PWMB0=1

	unsigned int buf[5] = {0};
	unsigned char filled = 0, last = 0;

	while (1) {
		for (int i = 0; i < PERIODS_LENGTH; i++) {
			Xil_Out32(TLR0, PERIODS[i]    ); // pwm period
			Xil_Out32(TLR1, HIGH_TIMES[i] ); // pwm high time
//			Xil_Out32(GPIO, TIMES[0]);

			for (int j = 0; j < 5; j++) {
				unsigned int icconf = Xil_In32(ICCONF);

				if (((icconf >> 3) & 1) == 0) {
					j--;
					continue;
				}

				buf[last] = Xil_In32(ICBUF);
				last = (last + 1) % 5;

				if (!filled && last != 0)
					continue;
				else filled = 1;

				if (buf[mod5(last + 4)] - buf[mod5(last + 3)] == buf[mod5(last + 2)] - buf[mod5(last + 1)] &&
					buf[mod5(last + 3)] - buf[mod5(last + 2)] == buf[mod5(last + 1)] - buf[last])
				{
					unsigned int period = (buf[mod5(last + 4)] - buf[mod5(last + 2)]) * 10;
					Xil_Out32(GPIO, period);
				} else {
					Xil_Out32(GPIO, 0);
				}
			}
		}
	}
	return 0;
}*/

int main() {
	return fifo_calc();
}
