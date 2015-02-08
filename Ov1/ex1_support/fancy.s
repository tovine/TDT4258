toggle_this:
	// TODO: function to toggle the selected LED
	b toggle_this
//	.thumb_func	
move_dot:
	// TODO: one dot lit, move left/right
	// If button 'SW[1|5]' is pressed, move dot left, SW[3|7] move right
	// If button 'SW[2|6]' is pressed, increase intensity, SW[4|8] decrease (use timers for PWM dimming)
	// r7 is used to keep track of left dot position, r8 for right
	// r9 is left dot on/off, r10 is right dot on/off

	ands r1, r0, 0x04	// 0b01000100
	bne move1_right
	ands r1, r0, 0x01	// 0b00010001
	bne move1_left
	ands r1, r0, 0x02	// 0b00100010
	bne dot1_on
	ands r1, r0, 0x08	// 0b10001000
	bne dot1_off
	ands r1, r0, 0x40	// 0b01000100
	bne move2_right
	ands r1, r0, 0x10	// 0b00010001
	bne move2_left
	ands r1, r0, 0x20	// 0b00100010
	bne dot2_on
	ands r1, r0, 0x80	// 0b10001000
	bne dot2_off
	b return

dot1_on:
	mov r9, #0xFF
	b return
dot1_off:
	mov r9, #0x00
	b return
move1_right:
	cmp r7, #0b10000000
	bge return
	lsl r7, r7, #1
	b return
move1_left:
	cmp r7, #1
	beq return
	lsr r7, r7, #1
dot2_on:
	mov r10, #0xFF
	b return
dot2_off:
	mov r10, #0x00
	b return
move2_right:
	cmp r8, #0b10000000
	bge return
	lsl r8, r8, #1
	b return
move2_left:
	cmp r8, #1
	beq return
	lsr r8, r8, #1

return:
	and r0, r9, r7
	and r1, r10, r8
	orr r0, r1
	mvn r0, r0
	b set_leds
