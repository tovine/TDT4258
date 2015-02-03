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
	orr r2,r2,r1
	str r2, [r1, #0]
 
	//load CMU base address
	ldr r1, =CMU_BASE
	mov r2, #1				// Load '1' to temp. register r2
	lsl r2, r2, #CMU_HFPERCLKEN0_GPIO	// Shift '1' to GPIO position
	str r2, [r1, #CMU_HFPERCLKEN0]		// Write result to control register

	// Set up GPIO - PORT_C[0-7] = SWITCHES, PORT_A[8-15] = LEDS
	// -> SWITCHES: strong pull-up, input (maybe filter) - MODEn = 0b0010, DOUT = 1
	// -> LEDS: output mode
//	ldr r1,=GPIO_PC_BASE
//	mov r2, #0x22222222 // = 0b0010
//	str r2, [r1, #GPIO_MODEL]
	
//	mov r2, #0xFF
//	str r2, [r1, #GPIO_DOUT]
	
	//LEDS, modeh=0b0100 push pull
	ldr r1,=GPIO_PA_BASE
	mov r2, #0x55
	lsl r3, r2, #8
	orr r3,r3,r2
	lsl r4,r3,#16
	orr r4,r4,r3
	str r4, [r1, #GPIO_MODEH]
	
	
	/////////////////////////////////////////////////////////////////////////////
	//
  // GPIO handler
  // The CPU will jump here when there is a GPIO interrupt
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
gpio_handler:  

//	      b .  // do nothing
//Set LEDS high
	ldr r1, =GPIO_PA_BASE
	mov r2, #0x00  //set pin 15-8 high
//	lsl r3,r2, #8
//	orr r3,r3,r2
	str r2, [r1, #GPIO_DOUT] 	
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
        b .  // do nothing

