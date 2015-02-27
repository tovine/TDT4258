	//Power down SRAM to conserve energy - this didn't work for us, TODO: fjern fra innlevering
	ldr r0, =EMU_BASE
	mov r1, #7				// Disable SRAM blocks 1-3
//	str r1, [r0, #0x4] 			// Write to EMU_MEMCTRL
/*	.thumb_func
pwm_init:
	//load CMU base address
	ldr r1, =CMU_BASE
	//Set bit #2 high to enable LETIMER0 clock
	mov r2, #4
	str r2, [r1, #CMU_LFACLKEN0]
	// Bits 11-8 in LFAPRESC0 set the prescaler for LETIMER0

	// LETIMER_CTRL: COMP0TOP=1 (COMP0 is top value), UFOA0=3 (PWM for output 0)
	// PWM: LETn_O0 is set idle on CNT underflow, and active on compare match with COMP1

	// LETIMER_CMD - used for start/stop timer, see page 594 in efm32GG manual
*/	

// Some more register addresses
	CMU_LFACLKEN0	= 0x058
	CMU_LFAPRESC0	= 0x068

LETIMER_BASE = 0x40082000
	LETIMER_CTRL	= 0x000
	LETIMER_CMD	= 0x004
	LETIMER_STATUS	= 0x008
	LETIMER_COMP0	= 0x010
	LETIMER_COMP1	= 0x014
	LETIMER_IF	= 0x020
	LETIMER_IFC	= 0x028
	LETIMER_IEN	= 0x02C
	LETIMER_ROUTE	= 0x040
