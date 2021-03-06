/* Assumes PIC16F690 running at 24MHz */

#define _XTAL_FREQ 24000000

#include <pic.h>
#include <htc.h>
#include "common.h"
#include "ppmio.h"

void initTimers (void) {
	// Set up timer 0 for 1ms tick:
	T0CS = 0;
	
	PS2 = 1;
	PS1 = 0; // set prescaler to 32
	PS0 = 0;
	
	PSA = 0;
	T0IE = 1; // initialise timer interrupt.
	
	// Set up timer 1 for servo control:
	TMR1GE = 0;
	TMR1CS = 0; // use internal clock
	T1CKPS0 = 0;
	T1CKPS1 = 0; // disable prescaler
	nT1SYNC = 1;
	PEIE = 1;
	
	// Clear timer interrupts:
	T0IF = 0;
	TMR1IF = 0;
}

void initIO (void) {
	TRISA = 0xFC;
	TRISB = 0xF0;
	TRISC = 0xEF;
	ANSEL = 0x00;
	ANSELH = 0x00;
	nRABPU = 0;
	WPUB = 0xF0;
	WPUA = 0x04;
}

void initGlobals (void) {
	unsigned char i;
	
	in_sync = 0;
	input_done = 0;
	input_calibrated = 0;
	tick = 0;

	// Init output_pulse array to sane defaults:
	for (i=0;i<TOTAL_OUTPUT_CHANNELS;i++) {	
		output_pulse[i] = SERVO_MIN;
	}
}

void init (void) {
	initTimers();
	initIO();
	initGlobals();
	enableInterrupts();
}
