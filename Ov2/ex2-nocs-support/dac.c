#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "dac.h"

void setupDAC()
{
	*CMU_HFPERCLKEN0 |= (1 << 17);	// Enable DAC
	*DAC0_CTRL = 0x50010;		// Prescale DAC clock
	*DAC0_CH0CTRL = 1;		// Enable left and right audio channels
	*DAC0_CH1CTRL = 1;
}
