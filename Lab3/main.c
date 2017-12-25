#include <xil_io.h>

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

unsigned int get_tlr_value(unsigned int period) {
	return 0xFFFFFFFF - (period / 10 - 2);
}

#define PERIODS_LENGTH 3

int main() {
	Xil_Out32(ICCONF, 0b1100001);
	Xil_Out32(T0_TMR, 0xFFFF);
	Xil_Out32(T0_TCONF, 0b10);
	wait(50);
	Xil_Out32(GPIO, Xil_In32(ICBUF));
	/*Xil_Out32(ICCONF, 0b1100001);
	Xil_Out32(ICCONF, 0b1100010);
	Xil_Out32(ICCONF, 0b1100011);
	Xil_Out32(ICCONF, 0b1100100);
	Xil_Out32(ICCONF, 0b1100101);
	Xil_Out32(ICCONF, 0b1100110);
	Xil_Out32(ICCONF, 0b1100111);
	Xil_In32(ICCONF);
	Xil_In32(ICCONF);
	Xil_In32(ICCONF);
	Xil_Out32(ICCONF, 0b1100000);
	Xil_Out32(ICCONF, 0b1100001);
	Xil_Out32(GPIO_ADDRESS, Xil_In32(ICCONF));*/

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

	while(1) {
		for (int i = 0; i < PERIODS_LENGTH; i++) {
			Xil_Out32(TLR0, PERIODS[i]    ); // pwm period
			Xil_Out32(TLR1, HIGH_TIMES[i] ); // pwm high time
			Xil_Out32(GPIO, TIMES[i]);
			for (volatile int j = 0; j < 69; j++);
		}
	}
}
