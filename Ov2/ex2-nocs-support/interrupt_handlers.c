#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ex2.h"
#include "efm32gg.h"
#include "interrupt_handlers.h"

/* TIMER1 interrupt handler */

void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{  
/*	DEBUG: generate white noise to check that the DAC is working correctly
	*DAC0_CH0DATA = (rand() % 4095);
	*DAC0_CH1DATA = (rand() % 4095);
*/
	if(curr_sound == NULL){
		// Sample pointer NULL/invalid - ouput no sound
		*DAC0_CH0DATA = DAC_IDLE_VAL;
		*DAC0_CH1DATA = DAC_IDLE_VAL;
		*SCR = 6;
	} else if (curr_sample >= sound_length -1) {
		// We're at the end of an audio signal - repeat or stop
		curr_sound = NULL;
		curr_sample = 0;
		// Re-set input interrupt to repeat the sound as long as the button is pressed
		*GPIO_IFS= 0xff;
	} else {
		// Send the samples to DAC
		*DAC0_CH0DATA = curr_sound[curr_sample];
		*DAC0_CH1DATA = curr_sound[curr_sample];
		curr_sample++;
	}
	*TIMER1_IFC = 1;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
	handleKeypress();
	*GPIO_IFC= 0xff;
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
	handleKeypress();
	*GPIO_IFC= 0xff;
}
