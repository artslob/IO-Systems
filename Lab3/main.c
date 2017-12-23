#include <xil_io.h>

#define GPIO_ADDRESS   0x40000000

/* AXI Timer register addresses */
#define AXI_TIMER_ADDRESS  0x41C00000
unsigned int TCSR0 = AXI_TIMER_ADDRESS + 0x00; // Timer 0 Control and Status Register
unsigned int TLR0  = AXI_TIMER_ADDRESS + 0x04; // Timer 0 Load Register
unsigned int TCR0  = AXI_TIMER_ADDRESS + 0x08; // Timer 0 Counter Register
unsigned int TCSR1 = AXI_TIMER_ADDRESS + 0x10; // Timer 1 Control and Status Register
unsigned int TLR1  = AXI_TIMER_ADDRESS + 0x14; // Timer 1 Load Register
unsigned int TCR1  = AXI_TIMER_ADDRESS + 0x18; // Timer 1 Counter Register

#define AXI_UARTLITE_ADDRESS   0x40600000

#define AXI_BRAM_ADDRESS   0xC0000000
/* Timer 0 */
unsigned int T0_TMR   = AXI_BRAM_ADDRESS + 0x00; // Timer 0 TMR
unsigned int T0_TVAL  = AXI_BRAM_ADDRESS + 0x04; // Timer 0 TVAL
unsigned int T0_TCONF = AXI_BRAM_ADDRESS + 0x08; // Timer 0 TCONF
/* Timer 1 */
unsigned int T1_TMR   = AXI_BRAM_ADDRESS + 0x0C; // Timer 1 TMR
unsigned int T1_TVAL  = AXI_BRAM_ADDRESS + 0x10; // Timer 1 TVAL
unsigned int T1_TCONF = AXI_BRAM_ADDRESS + 0x14; // Timer 1 TCONF
/* Input Capture */
unsigned int ICCONF   = AXI_BRAM_ADDRESS + 0x18; // IC ICCONF
unsigned int ICBUF    = AXI_BRAM_ADDRESS + 0x1C; // IC ICBUF


unsigned int get_tlr_value(unsigned int period) {
	return 0xFFFFFFFF - (period / 10 - 2);
}

#define PERIODS_LENGTH 3

int main() {
	Xil_Out32(T0_TMR, 0xF);
	Xil_Out32(T1_TMR, 0xF);
	Xil_Out32(T0_TCONF, 0x2);
	Xil_Out32(T1_TCONF, 0x2);

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
			Xil_Out32(GPIO_ADDRESS, TIMES[i]);
			for (volatile int j = 0; j < 69; j++);
		}
	}
}
