/* pinout assumes PIC16F690 */

#include <pic.h>

// PPM signal
#define PPM_IN   RC5
#define PPM_OUT  RC4

// Analog/Potentiometer input
#define ANALOG1  RC7
#define ANALOG2  RC6
#define ANALOG3  RC3

// Digital I/O
#define DIGITAL1 RB4
#define DIGITAL2 RB5
#define DIGITAL3 RB6
#define DIGITAL4 RB7
#define TRIM_SWITCH RC2

#define MODEL_0 RA2&&RC1&&RC0
#define MODEL_1 !RA2&&RC1&&RC0
#define MODEL_2 RA2&&!RC1&&RC0
#define MODEL_3 !RA2&&!RC1&&RC0
#define MODEL_4 RA2&&RC1&&!RC0
#define MODEL_5 !RA2&&RC1&&!RC0
#define MODEL_6 RA2&&!RC1&&!RC0

#define LED_DATA DIGITAL1
#define LED_CLOCK DIGITAL2
#define LED_LATCH DIGITAL3

