.include "efm32gg.s"
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
	//Set high drive strength 
	ldr r1, =GPIO_PA_BASE
	mov r2, #0x2
	str r2, [r1, #GPIO_CTRL]
 
	//load CMU base address
	ldr r1, =CMU_BASE
	mov r2, #1				// Load '1' to temp. register r2
	lsl r2, r2, #CMU_HFPERCLKEN0_GPIO	// Shift '1' to GPIO position
	str r2, [r1, #CMU_HFPERCLKEN0]		// Write result to control register

	// Set up GPIO - PORT_C[0-7] = SWITCHES, PORT_A[8-15] = LEDS
	// -> SWITCHES: strong pull-up, input with filter - MODEn = 0b0011, DOUT = 1
	// -> LEDS: output mode
	ldr r1,=GPIO_PC_BASE
	ldr r2,=0x33333333
	str r2, [r1, #GPIO_MODEL]
	
	mov r2, #0xFF
	str r2, [r1, #GPIO_DOUT]	// Pull direction: up
	
	//LEDS, modeh=0b0100 push pull
	ldr r1,=GPIO_PA_BASE
	ldr r2,=0x55555555
	str r2, [r1, #GPIO_MODEH] // Her går alle leds på - PUSHPULLDRIVE, value from DOUT

//GPIO interrupts	

	// Select PORTC for all pin change interrupts	
	ldr r1,=GPIO_BASE
	ldr r2,=0x22222222
	str r2, [r1, #GPIO_EXTIPSELL]

	mov r2, #0xFF

	// Set interrupts on falling edge for pins 0-7
	str r2, [r1, #GPIO_EXTIFALL]
	// Set interrupts on rising edge for pins 0-7
	str r2, [r1, #GPIO_EXTIRISE]
	// Enable interrupts for pins 0-7
	str r2, [r1, #GPIO_IEN]

	ldr r1, =ISER0
	movw r2, #0x802
	str r2, [r1]

//TODO: go to sleep after initializing (see page 100 in Cortex-M3 manual)
	ldr r1, =SCR
	mov r2, #6
	str r2, [r1]
	wfi

main_loop:
	b main_loop
//GPIO interrupts	
	
	/////////////////////////////////////////////////////////////////////////////
	//
  // GPIO handler
  // The CPU will jump here when there is a GPIO interrupt
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
gpio_handler: 
//TODO: go to sleep when returning from interrupt (see page 100 in Cortex-M3 manual) 

	ldr r1, =GPIO_PA_BASE
	ldr r0, =GPIO_PC_BASE
	// Get which pin was pressed
	ldr r3, =GPIO_BASE
	ldr r2, [r3, #GPIO_IF]
	// Clear interrupt flag so it will not trigger again immediately
	str r2, [r3, #GPIO_IFC]

	ldr r4, [r0, #GPIO_DIN]
//	mov r4, #0xFF
//	and r4, r2, r4
	lsl r4, r4, #8
	// Toggle corresponding LED
	str r4, [r1, #GPIO_DOUT]
	bx lr

//	mov r2, #0x00  //set pin 15-8 high
//	lsl r3,r2, #8
//	orr r3,r3,r2
//	str r2, [r1, #GPIO_DOUT] 	
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
        b .  // do nothing

