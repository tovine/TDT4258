#include <stdint.h>
#include <stdbool.h>

//#include "efm32gg.h"

#include "ex2.h"

/* 
  TODO calculate the appropriate sample period for the sound wave(s) 
  you want to generate. The core clock (which the timer clock is derived
  from) runs at 14 MHz by default. Also remember that the timer counter
  registers are 16 bits.
*/
/* The period between sound samples, in clock cycles */
#define SAMPLE_PERIOD   317	// 14MHz/317 = 44.1kHz
#define	DAC_IDLE_VAL	0x7FF	// Idle output voltage should be 1/2 max

/* Declaration of peripheral setup functions 
void setupTimer(uint32_t period);
void setupDAC();
void setupNVIC();
*/

// Variables used to keep track of the current playing sound
uint16_t *curr_sound = NULL;
//uint16_t *curr_sample = NULL; // Pointer to current sample
bool repeat_sound = true;
unsigned int sound_length = 0, curr_sample = 0;

void playSound(uint16_t[] &WaveformArray) {
	sound_length = sizeof(WaveformAray);
	curr_sound = WaveformAray;
//	curr_sample = curr_sound;
	curr_sample = 0;
}

/* Your code will start executing here */
int main(void) 
{  
  /* Call the peripheral setup functions */
  setupGPIO();
  setupDAC();
//  setupTimer(254);
  setupTimer(SAMPLE_PERIOD);
  
  /* Enable interrupt handling */
  setupNVIC();
  startTimer();
  playSound(pacman); 
  /* TODO for higher energy efficiency, sleep while waiting for interrupts
     instead of infinite loop for busy-waiting
  */
  // Select sleep mode EM1 - timer does not run when in EM(>=2)
  *SCR = 2 ;
  while(1);

  return 0;
}

void setupDMA()
{
	// Enable DMA controller
	*DMA_CONFIG = 1;
	// TODO: set DMA_CTRLBASE to the base address of the control data structure
	// DMA_CHENS - [11:0] write 1 to enable the corresponding channel
	// Set [21:16] til 0b001010 (DAC0) og [3:0] til 0b0001
	*DMA_CH0_CTRL = 0x0A0000;	// Source: DAC0CH0
//	*DMA_CH1_CTRL = 0x0A0001;	// Source: DAC0CH1
	/* DMA configuration (32-bit word) - refer to page 65 in the EFM32GG manual
	0b01
	----
	Detail:
	dst_inc		= b01 - halfword (16 bits)
	dst_size	= b??
	src_inc		= b01 - halfword (16 bits)
	src_size	= b01 - halfword
	dst_prot_ctrl	= b000 - non-privileged
	src_prot_ctrl	= b000 - non-privileged
	R_power		= b????
	n_minus_1	= 
	next_useburst	= b0 - disable next_useburst?
	cycle_ctrl	= b011 - ping-pong mode
	*/
}
// DMA_IEN - enable DMA complete interrupt (1 bit per channel)s
// DMA_IF - interrupt flag settes høye når den korresponderende DMA-kanalen har fullført en overføring
// DMA_IFS/DMA_IFC - set/clear DMA_IF



void setupNVIC()
{
  /* TODO use the NVIC ISERx registers to enable handling of interrupt(s)
     remember two things are necessary for interrupt handling:
      - the peripheral must generate an interrupt signal
      - the NVIC must be configured to make the CPU handle the signal
     You will need TIMER1, GPIO odd and GPIO even interrupt handling for this
     assignment.
  */
	*ISER0 |= (1<<1) | (1<<11) | (1<<12); /*enable handling of interrupt TIMER1, GPIO_EVEN and GPIO_ODD*/
 	*ISER0 |= 0x802; 
}

/* if other interrupt handlers are needed, use the following names: 
   NMI_Handler
   HardFault_Handler
   MemManage_Handler
   BusFault_Handler
   UsageFault_Handler
   Reserved7_Handler
   Reserved8_Handler
   Reserved9_Handler
   Reserved10_Handler
   SVC_Handler
   DebugMon_Handler
   Reserved13_Handler
   PendSV_Handler
   SysTick_Handler
   DMA_IRQHandler
   GPIO_EVEN_IRQHandler
   TIMER0_IRQHandler
   USART0_RX_IRQHandler
   USART0_TX_IRQHandler
   USB_IRQHandler
   ACMP0_IRQHandler
   ADC0_IRQHandler
   DAC0_IRQHandler
   I2C0_IRQHandler
   I2C1_IRQHandler
   GPIO_ODD_IRQHandler
   TIMER1_IRQHandler
   TIMER2_IRQHandler
   TIMER3_IRQHandler
   USART1_RX_IRQHandler
   USART1_TX_IRQHandler
   LESENSE_IRQHandler
   USART2_RX_IRQHandler
   USART2_TX_IRQHandler
   UART0_RX_IRQHandler
   UART0_TX_IRQHandler
   UART1_RX_IRQHandler
   UART1_TX_IRQHandler
   LEUART0_IRQHandler
   LEUART1_IRQHandler
   LETIMER0_IRQHandler
   PCNT0_IRQHandler
   PCNT1_IRQHandler
   PCNT2_IRQHandler
   RTC_IRQHandler
   BURTC_IRQHandler
   CMU_IRQHandler
   VCMP_IRQHandler
   LCD_IRQHandler
   MSC_IRQHandler
   AES_IRQHandler
   EBI_IRQHandler
   EMU_IRQHandler
*/
