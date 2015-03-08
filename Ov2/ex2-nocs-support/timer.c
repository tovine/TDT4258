#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

#include "timer.h"

/* function to setup the timer */
void setupTimer(uint16_t period) {
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_TIMER1;
	*TIMER1_TOP = period;
	*TIMER1_IEN= 1;
}

void setupTimerLE(void) {
	*CMU_HFCORECLKEN0 |= (1 << 4); // Enable Low Energy Peripheral Clock
	*RTC_CTRL = 5;	// Enable RTC and set it to get TOP value from COMP0
	// With the compare match set to 0, we will get an interrupt every RTC tick (32.768kHz)- nice for pushing a new sample to the DAC, while saving lots of power...
	*RTC_COMP0 = 1;
	*RTC_IEN = 2;
	*CMU_LFACLKEN0 |= (1 << 1);	// Enable RTC clock signal
}

void startTimer(void) {
	*TIMER1_CMD= 1;
}
