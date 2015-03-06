#ifndef EX2_HEADER
#define EX2_HEADER

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "gpio.h"
#include "dac.h"
#include "timer.h"
#include "interrupt_handlers.h"

/* The period between sound samples, in clock cycles */
#define SAMPLE_PERIOD   317	// 14MHz/317 = 44.1kHz
#define	DAC_IDLE_VAL	0x7FF	// Idle output voltage should be 1/2 max

/* Declaration of peripheral setup functions */
void setupNVIC();
void setupDMA();

void handleKeypress(void);

extern uint16_t *curr_sound;
//uint16_t *curr_sample = NULL; // Pointer to current sample
extern bool repeat_sound;
extern unsigned int sound_length, curr_sample;

#endif
