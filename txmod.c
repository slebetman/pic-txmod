/* Assumes PIC16F690 running at 24MHz */

#define _XTAL_FREQ 24000000

#include <pic.h>
#include <htc.h>
#include "config.h"
#include "pinout.h"

#define enableInterrupts() GIE = 1
#define disableInterrupts() GIE = 0
#define stopCapture() CCP1CON = 0x00
#define stopPPM() TMR1ON=0

bit tick;          // timer tick (roughly 1ms using 24 MHz XTAL)
bit input_done;

#define TOTAL_OUTPUT_CHANNELS 6
#define TOTAL_INPUT_CHANNELS 4
char channel; // which servo channel to pulse

struct twoBytes {
	unsigned char low;
	unsigned char high;
};

union intOrBytes {
	unsigned int integer;
	struct twoBytes bytes;
};

union intOrBytes input_pulse[TOTAL_INPUT_CHANNELS];
union intOrBytes output_pulse[TOTAL_OUTPUT_CHANNELS];

void initTimers(void)
{
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
	T1SYNC = 1;
	TMR1IE = 1;
	PEIE = 1;
	
	// Clear timer interrupts:
	T0IF = 0;
	TMR1IF = 0;
}

#define BEGIN -1
#define CONTINUE 0

void startPPM (union intOrBytes duration,char mode) {
	channel = mode;
	TMR1H = duration.bytes.high;
	TMR1L = duration.bytes.low;
	TMR1IF = 0;
	TMR1ON = 1;
}

void startCapture (char mode) {
	channel = mode;
	TMR1H = 0;
	TMR1L = 0;
	CCP1IF = 0;
	CCP1CON = 0x04;
}

void processInput () {
	if (channel < TOTAL_INPUT_CHANNELS) {
		if (channel > 0) {
			input_pulse[channel].bytes.high = TMR1H;
			input_pulse[channel].bytes.low  = TMR1L;
		}
		channel++;
		startCapture(CONTINUE);
	}
	else {
		stopCapture();
		channel = 0;
		input_done = 1;
	}
}

void processOutput () {
	if (channel < TOTAL_OUTPUT_CHANNELS) {
		PPM_OUT = 0;
		channel++;
		startPPM(output_pulse[channel],CONTINUE);
		NOP();
		NOP();
		NOP();
		PPM_OUT = 1;
	}
	else {
		PPM_OUT = 0;
		stopPPM();
		channel = -1;
		startCapture(BEGIN);
		NOP();
		NOP();
		NOP();
		PPM_OUT = 1;
	}
}

void interrupt HANDLER(void)
{
	if(CCP1IF)
	{
		processInput();
	}
	if(TMR1IF)
	{
		processOutput();
		TMR1IF = 0; // reset timer interrupt
	}
	if(T0IF)
	{
		TMR0 = 55;
		T0IF = 0;   // reset timer interrupt
		tick = 1;
	}
}

void main(void)
{
	#define PPM_BLANK_CHECK 5
	static bit in_sync = 0;
	static bit last_ppm = 0;
	unsigned char ppm_time = 0;

	initTimers();
	input_done = 0;
	tick = 0;
	enableInterrupts();

	while(1)
	{
		/*
		 * Find the start of PPM frame by looking
		 * for the end of a PPM frame.
		 */
		 
		if (tick) {
			tick = 0;
			ppm_time ++;
		}
		if (PPM_IN != last_ppm) {
			ppm_time = 0;
			last_ppm = PPM_IN;
		}
		if (ppm_time > PPM_BLANK_CHECK) {
			// assume we are at the end of PPM frame
			ppm_time = 0;
			in_sync = 1;
		}
	
		while(in_sync)
		{
			if (input_done) {
				input_done = 0;
				// time to calculate mixes:
				
				channel = -1;
				startCapture(BEGIN);
				// end of calculations, start outputting PPM:
				//startPPM({65500},BEGIN);
			}
		}
	}
}
