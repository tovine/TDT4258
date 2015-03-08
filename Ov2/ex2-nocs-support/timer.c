#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

#include "timer.h"

/* function to setup the timer */
void setupTimer(uint16_t period) {
  /*
    TODO enable and set up the timer
    
    1. Enable clock to timer by setting bit 6 in CMU_HFPERCLKEN0
    2. Write the period to register TIMER1_TOP
    3. Enable timer interrupt generation by writing 1 to TIMER1_IEN
    4. Start the timer by writing 1 to TIMER1_CMD
    
    This will cause a timer interrupt to be generated every (period) cycles. Remember to configure the NVIC as well, otherwise the interrupt handler will not be invoked.
  */
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_TIMER1;
	*TIMER1_TOP = period;
	*TIMER1_IEN= 1;
//	*ISER0 |= (1<<12);
}

void setupTimerLE(void) {
	*CMU_HFCORECLKEN0 |= (1 << 4); // Enable Low Energy Peripheral Clock
	*RTC_CTRL = 5;	// Enable RTC and set it to get TOP value from COMP0
	// With the compare match set to 0, we will get an interrupt every RTC tick (32.768kHz)- nice for pushing a new sample to the DAC, while saving lots of power...
	*RTC_COMP0 = 1;
	*RTC_IEN = 1;
	*CMU_LFACLKEN0 |= (1 << 1);	// Enable RTC clock signal
}

void startTimer(void) {
	*TIMER1_CMD= 1;
}
