#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "interrupt_handlers.h"
#include "sounds.c"
/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
  /*
    TODO feed new samples to the DAC
    remember to clear the pending interrupt by writing 1 to TIMER1_IFC
  */  
	*DAC0_CH0DATA = sinewave[sample];
	sample++;
//	*DAC0_CH1DATA;
	*TIMER1_IFC = 1;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
    /* TODO handle button pressed event, remember to clear pending interrupt */
	*GPIO_PA_DOUT = 0xAA00;
	*GPIO_IFC= 0xff;
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
    /* TODO handle button pressed event, remember to clear pending interrupt */
	*GPIO_PA_DOUT = 0x5500;
//	*GPIO_PA_DOUT ^= *GPIO_PC_DOUT;
	*GPIO_IFC= 0xff;
}
