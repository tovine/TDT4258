#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ex2.h"
#include "efm32gg.h"
#include "interrupt_handlers.h"
//#include "sounds.c"

/* TIMER1 interrupt handler */

void __attribute__ ((interrupt)) TIMER1_IRQHandler()
//void __attribute__ ((interrupt)) RTC_IRQHandler() 
{  
  /*
    TODO feed new samples to the DAC
    remember to clear the pending interrupt by writing 1 to TIMER1_IFC
  */  
/*
	*DAC0_CH0DATA = sinewave[sample];
	*DAC0_CH1DATA = sinewave[sample];
	//else *DAC0_CH1DATA = (squarewave[sample] << 4);
//	*DAC0_CH0DATA = (rand() % 4095);
//	*DAC0_CH1DATA = (rand() % 4095);
	sample++;
*/
//	if(sample == 0) r = !r;
//	*GPIO_PA_DOUT = (sample << 8);
	if(curr_sound == NULL){
		// Sample pointer NULL/invalid - ouput 0
		*DAC0_CH0DATA = DAC_IDLE_VAL;
		*DAC0_CH1DATA = DAC_IDLE_VAL;
//	} else if (*curr_sample > 0xff) {
	} else if (curr_sample >= sound_length -1) {
		// We're at the end of an audio signal - repeat or stop
		if(!repeat_sound) curr_sound = NULL;
//		curr_sample = curr_sound;
		curr_sample = 0;
		// Clear input interrupt to repeat the sound as long as the button is pressed
		*GPIO_IFS= 0xff;
	} else {
		// Send the samples to DAC
		*DAC0_CH0DATA = curr_sound[curr_sample];
		*DAC0_CH1DATA = curr_sound[curr_sample];
//		curr_sample += 2; // Skip two samples (stereo)
		curr_sample++;
	}
	*TIMER1_IFC = 1;
	*RTC_IFC = 1;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
	handleKeypress();
    /* TODO handle button pressed event, remember to clear pending interrupt */
	*GPIO_PA_DOUT = 0xAA00;
	*GPIO_IFC= 0xff;
/*	curr_sound = &sinewave[0];
	if(repeat_sound) repeat_sound = false;
	else repeat_sound = true;
	curr_sample = curr_sound;
*/
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
	handleKeypress();
    /* TODO handle button pressed event, remember to clear pending interrupt */
	*GPIO_PA_DOUT = 0x5500;
	*GPIO_IFC= 0xff;
/*	curr_sound = &squarewave[0];
	if(repeat_sound) repeat_sound = false;
	else repeat_sound = true;
	curr_sample = curr_sound;
*/
}
