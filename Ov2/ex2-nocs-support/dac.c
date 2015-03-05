#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "dac.h"

void setupDAC()
{
  /*
    TODO enable and set up the Digital-Analog Converter
    
    1. Enable the DAC clock by setting bit 17 in CMU_HFPERCLKEN0
    2. Prescale DAC clock by writing 0x50010 to DAC0_CTRL
    3. Enable left and right audio channels by writing 1 to DAC0_CH0CTRL and DAC0_CH1CTRL
    4. Write a continuous stream of samples to the DAC data registers, DAC0_CH0DATA and DAC0_CH1DATA, for example from a timer interrupt
  */
	*CMU_HFPERCLKEN0 |= (1 << 17);	// Enable DAC
	*DAC0_CTRL = 0x50010;		// Prescale DAC clock
	*DAC0_CH0CTRL = 1;		// Enable left and right audio channels
	*DAC0_CH1CTRL = 1;
//	*DAC0_IEN = 3;			// Enable conversion complete interrupt for both channels TODO: remember to set the correct bits in DAC0_IFC register after handling interrupt
	// TODO: use DAC0_CAL to set channel offset and adjust output volume
	
}
