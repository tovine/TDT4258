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

void startTimer(void) {
	*TIMER1_CMD= 1;
}
