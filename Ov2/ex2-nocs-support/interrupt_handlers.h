#ifndef INT_HANDLERS_
#define INT_HANDLERS_

void __attribute__ ((interrupt)) TIMER1_IRQHandler();
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler();
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler();

#endif
