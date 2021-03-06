/* Assumes PIC16F690 running at 24MHz */

#define _XTAL_FREQ 24000000

#include <xc.h>
#include <stdlib.h>
#include "config.h"
#include "common.h"
#include "ppmio.h"
#include "calculations.h"
#include "trim.h"
#include "init.h"

unsigned char tick; // timer tick (roughly 1ms using 24 MHz XTAL)

void main(void)
{
	init();
	initTrim();

	while(1)
	{
		syncPPM();
	
		while(in_sync)
		{
			if (input_done) {
				input_done = 0;

				output_pulse[CHANNEL1] = AILERON;
				output_pulse[CHANNEL2] = ELEVATOR;
				output_pulse[CHANNEL3] = THROTTLE;
				output_pulse[CHANNEL4] = RUDDER;
				
				trim(TRIM_SWITCH, EXCEPT CHANNEL3);
				
				startPPM(10, BEGIN);
			}
		}
	}
}
