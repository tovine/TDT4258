toggle_this:
	// TODO: function to toggle the selected LED
	b toggle_this
//	.thumb_func	
move_dot:
	// TODO: one dot lit, move left/right
	// If button 'SW[1|5]' is pressed, move dot left, SW[3|7] move right
	// If button 'SW[2|6]' is pressed, increase intensity, SW[4|8] decrease (use timers for PWM dimming)
	// r7 is used to keep track of current dot position

	ands r1, r0, 0x44	// 0b01000100
	bne move_left
	ands r1, r0, 0x11	// 0b00010001
	bne move_right
	b return

move_left:
	cmp r7, #0b10000000
	bge return
	lsl r7, r7, #1
	b return
move_right:
	cmp r7, #1
	beq return
	lsr r7, r7, #1

return:
	mvn r0, r7
	b set_leds
