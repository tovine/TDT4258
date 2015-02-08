move_dot:
	/*
	* The button groups control one dot of light each, the left and right
	* buttons move the corresponding dot sideways, while the top buttons
	* turn it on and the bottom buttons turn it off.
	* Register 'r7' is used to keep track of left dot position, 'r8' for right
	* 'r9' is left dot on/off, 'r10' is right dot on/off
	*/

	// Find out which button was pressed and execute the correct command
	ands r1, r0, 0x04	// 0b00000100
	bne move1_right
	ands r1, r0, 0x01	// 0b00000001
	bne move1_left
	ands r1, r0, 0x02	// 0b00000010
	bne dot1_on
	ands r1, r0, 0x08	// 0b00001000
	bne dot1_off
	ands r1, r0, 0x40	// 0b01000000
	bne move2_right
	ands r1, r0, 0x10	// 0b00010000
	bne move2_left
	ands r1, r0, 0x20	// 0b00100000
	bne dot2_on
	ands r1, r0, 0x80	// 0b10000000
	bne dot2_off
	b return

// Functions to turn on/off the dots
dot1_on:
	mov r9, #0xFF
	b return
dot1_off:
	mov r9, #0x00
	b return
dot2_on:
	mov r10, #0xFF
	b return
dot2_off:
	mov r10, #0x00
	b return

// Functions to move the dots sideways
move1_right:
	cmp r7, #0b10000000	// Make sure that we don't move the dot outside the range of the LEDs
	bge return
	lsl r7, r7, #1		// If dot is not at the edge, shift it one step right
	b return
move1_left:
	cmp r7, #1
	beq return
	lsr r7, r7, #1
move2_right:
	cmp r8, #0b10000000
	bge return
	lsl r8, r8, #1
	b return
move2_left:
	cmp r8, #1
	beq return
	lsr r8, r8, #1

// Combine the LED registers and return to the main routine
return:
	and r0, r9, r7		// Light the LED only if the corresponding dot is turned on
	and r1, r10, r8
	orr r0, r1		// Combine the bits for both dots
	mvn r0, r0		// Negate the result because '0' output gives a voltage difference over the LED and causes current flow -> light
	b set_leds
