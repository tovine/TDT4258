#ifndef EX2_HEADER
#define EX2_HEADER

#include "efm32gg.h"
#include "gpio.h"
#include "dac.h"
#include "timer.h"
#include "interrupt_handlers.h"

/* Declaration of peripheral setup functions */
void setupNVIC();
void setupDMA();

#endif
