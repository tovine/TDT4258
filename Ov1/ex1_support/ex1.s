.include "efm32gg.s"
.syntax unified
.include "fancy.s"

	/////////////////////////////////////////////////////////////////////////////
	//
	// Exception vector table
	// This table contains addresses for all exception handlers
	//
	/////////////////////////////////////////////////////////////////////////////
	

        .section .vectors
	
	      .long   stack_top               /* Top of Stack                 */
	      .long   _reset                  /* Reset Handler                */
	      .long   dummy_handler           /* NMI Handler                  */
	      .long   dummy_handler           /* Hard Fault Handler           */
	      .long   dummy_handler           /* MPU Fault Handler            */
	      .long   dummy_handler           /* Bus Fault Handler            */
	      .long   dummy_handler           /* Usage Fault Handler          */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* SVCall Handler               */
	      .long   dummy_handler           /* Debug Monitor Handler        */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* PendSV Handler               */
	      .long   dummy_handler           /* SysTick Handler              */

	      /* External Interrupts */
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO even handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO odd handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler

	      .section .text

	/////////////////////////////////////////////////////////////////////////////
	//
	// Reset handler
	// The CPU will start executing here after a reset
	//
	/////////////////////////////////////////////////////////////////////////////

	      .globl  _reset
	      .type   _reset, %function
        .thumb_func
_reset:
	// Store the commonly used addresses in "static" registers to avoid having
	// to constantly reload them whenever used...
	ldr r4, =GPIO_PA_BASE
	ldr r5, =GPIO_PC_BASE
	ldr r6, =GPIO_BASE

	//Set high drive strength 
	mov r2, #0x2
	str r2, [r4, #GPIO_CTRL] // GPIO_PA_CTRL
 
	//load CMU base address
	ldr r1, =CMU_BASE
	mov r2, #1				// Load '1' to temp. register r2
	lsl r2, r2, #CMU_HFPERCLKEN0_GPIO	// Shift '1' to GPIO position
	str r2, [r1, #CMU_HFPERCLKEN0]		// Write result to control register

	// Set up GPIO - PORT_C[0-7] = SWITCHES, PORT_A[8-15] = LEDS
	// -> SWITCHES: strong pull-up, input with filter - MODEn = 0b0011, DOUT = 1
	// -> LEDS: output mode
	ldr r2,=0x33333333
	str r2, [r5, #GPIO_MODEL]	// GPIO_PC_MODEL
	
	mov r2, #0xFF
	str r2, [r5, #GPIO_DOUT]	// Pull direction: up
	
	//LEDS, modeh=0b0100 push pull
	ldr r2,=0x55555555
	str r2, [r4, #GPIO_MODEH]	// GPIO_PA_MODEH

	// Enable GPIO interrupts, select PORTC for all pin change interrupts	
	ldr r2,=0x22222222
	str r2, [r6, #GPIO_EXTIPSELL]

	mov r2, #0xFF

	// Set interrupts on falling edge for pins 0-7
	str r2, [r6, #GPIO_EXTIFALL]
	// Set interrupts on rising edge for pins 0-7
//	str r2, [r6, #GPIO_EXTIRISE]	// We only listen for button press events
	// Enable interrupts for pins 0-7
	str r2, [r6, #GPIO_IEN]

	ldr r1, =ISER0
	movw r2, #0x802
	str r2, [r1]

	mov r7, #1	// Variable for use in fancy patterns
	mov r8, #128	// Variable for use in fancy patterns

	//Go to sleep after initializing (see page 100 in Cortex-M3 manual)
	ldr r1, =SCR
	mov r2, #6	// Select DEEPSLEEP as the low power mode, and SLEEPONEXIT
	str r2, [r1]
	wfi

	/////////////////////////////////////////////////////////////////////////////
	//
	// GPIO handler
	// The CPU will jump here when there is a GPIO interrupt
	//
	/////////////////////////////////////////////////////////////////////////////
        .thumb_func
gpio_handler: 

	ldr r2, [r6, #GPIO_IF]
	// Clear interrupt flag so it will not trigger again immediately
	str r2, [r6, #GPIO_IFC]
	// Read the current state of button inputs
	ldrb r0, [r5, #GPIO_DIN]
	// Invert because buttons have "inverted" meaning
	mvn r0, r0

	b move_dot	// Function in fancy.s

	// Return here after modifying LED display register
set_leds:
	// Left shift because LEDs are on pins 8-15
	lsl r0, r0, #8
	// Update LEDs
	str r0, [r4, #GPIO_DOUT]
	
	// Return from interrupt handler
	bx lr

	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
        b .  // do nothing

