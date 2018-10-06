	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	display_data: .byte 0, 0, 0, 0, 0, 0, 0, 0 //TODO: put your student id here
	display_limit: .word 0
	ten_million: .word 10000000
	feb_first: .word 0
	feb_second: .word 1
	feb_max: .word 90000000
	top_found: .word 0
	ONE_SEC: .word 800000


.text
	.global main

	.equ RCC_AHB2ENR,  0x4002104C

	.equ DECODE_MODE,  0b00001001
	.equ DISPLAY_TEST, 0b00001111
	.equ SCAN_LIMIT,   0b00001011
	.equ INTENSITY,    0b00001010
	.equ SHUTDOWN,     0b00001100

	.equ MAX7219_DIN,  32 // PA5
	.equ MAX7219_CS,   64 // PA6
	.equ MAX7219_CLK,  128 // PA7

	.equ GPIOA_BASE,   0x48000000
	.equ GPIOC_BASE,   0x48000800
	.equ BSRR_OFFSET,  24
	.equ BRR_OFFSET,   40

GPIO_init:
	mov r0, 0b00000000000000000000000000000101
	ldr r1, =RCC_AHB2ENR
	str r0, [r1]

	ldr r1, =GPIOA_BASE // GPIOA_MODER
	ldr r2, [r1]
	and r2, 0b11111111111111110000001111111111
	orr r2, 0b00000000000000000101010000000000
	str r2, [r1]

	add r1, #4
	ldr r2, [r1]
	and r2, 0b11111111111111111111111100011111
	str r2, [r1]

	add r1, #4
	ldr r2, [r1]
	and r2, 0b11111111111111110000001111111111
	orr r2, 0b00000000000000000101010000000000
	str r2, [r1]

    ldr r1, =GPIOC_BASE 
    ldr r2, [r1]
    and r2, 0b11110011111111111111111111111111
    str r2, [r1]
    add r10, r1, 0x10
	bx lr

MAX7219_init:
	push {r0, r1, r2, lr}

	ldr r0, =DECODE_MODE
	ldr r1, =0xFF
	bl MAX7219_send

	ldr r0, =DISPLAY_TEST
	ldr r1, =0
	bl MAX7219_send

	ldr r0, =SCAN_LIMIT
	ldr r1, =7
	bl MAX7219_send

	ldr r0, =INTENSITY
	ldr r1, =10
	bl MAX7219_send

	ldr r0, =SHUTDOWN
	ldr r1, =1
	bl MAX7219_send

	pop {r0, r1, r2, pc}

MAX7219_send:
	// input parameter: r0 is ADDRESS , r1 is DATA
	push {r0, r1, r2, r3, r4, r5, r6, r7, r8, lr}
	lsl r0, r0, #8
	add r0, r1
	ldr r1, =GPIOA_BASE
	ldr r3, =MAX7219_DIN
	ldr r4, =MAX7219_CLK
	ldr r5, =BSRR_OFFSET
	ldr r6, =BRR_OFFSET
	ldr r7, =#15 // currently sending r7-th bit

	MAX7219_send_loop:
		mov r2, #1
		lsl r2, r2, r7
		str r4, [r1, r6] // clk -> 0
		tst r0, r2 // ANDS but discard result
		beq MAX7219_IF_ENDS
		b MAX7219_IF_NOT_ENDS
		MAX7219_IF_ENDS:
			str r3, [r1, r6] // din -> 0
			b END_MAX7219_IF_ENDS
		MAX7219_IF_NOT_ENDS:
			str r3, [r1, r5] // din -> 1
			b END_MAX7219_IF_ENDS
		END_MAX7219_IF_ENDS:
		str r4, [r1, r5] // clk -> 1
		subs r7, #1
		bge MAX7219_send_loop
		ldr r2, =MAX7219_CS
		str r2, [r1, r6] // cs -> 0
		str r2, [r1, r5] // cs -> 1
		pop {r0, r1, r2, r3, r4, r5, r6, r7, r8, pc}


display:
	PUSH {r0-r3, lr}
	PUSH {r1}
	ldr r0, =SCAN_LIMIT
	ldr r2, =display_limit
	ldr r2, [r2]
	mov r1, r2
	bl MAX7219_send
	POP {r1}

	mov r0, #1
	mov r2, #7
	ldr r3, =display_data

	REFRASH_WHILE:
		ldrb r1, [r3, r2]
		bl MAX7219_send

		cmp r2, #0
		beq IF_FINISHED
		b IF_FINISHED_ELSE
		IF_FINISHED:
			mov r0, #1
			mov r2, #7
			b END_REFRASH_WHILE
		IF_FINISHED_ELSE:
			add r0, r0, #1
			sub r2, r2, #1
		END_IF_FINISHED:
		b REFRASH_WHILE
	END_REFRASH_WHILE:
	POP {r0-r3, pc}
	bx lr

do_feb:
	PUSH {r0, r1, r2, r3}
	ldr r0, =feb_first
	ldr r0, [r0]
	ldr r1, =feb_second
	ldr r1, [r1]
	add r2, r0, r1
	ldr r3, =feb_max
	ldr r3, [r3]
	cmp r1, r3
	bgt REACHES_MAX
	b NOT_REACHES_MAX
	REACHES_MAX:
		ldr r0, =feb_first
		str r3, [r0]
		b END_REACHES_MAX
	NOT_REACHES_MAX:
		ldr r0, =feb_first
		str r1, [r0]
		ldr r1, =feb_second
		str r2, [r1]
	END_REACHES_MAX:

	POP {r0, r1, r2, r3}
	bx lr

set_display_array:
	PUSH {r0-r7}
	ldr r0, =feb_first
	ldr r0, [r0]
	cmp r0, #0
	beq FEB_ZERO_SPECIAL_CASE

	ldr r2, =feb_max
	ldr r2, [r2]
	cmp r0, r2
	beq FEB_EXPLOSION_SPECIAL_CASE

	ldr r2, =ten_million
	ldr r2, [r2]

	ldr r3, =display_limit
	mov r4, #7 // number head
	str r4, [r3]
	ldr r4, =display_data
	mov r5, #0 // array pos @ 0
	ldr r6, =top_found // == false
	str r5, [r6]

	WHILE_GET_NUM:
		udiv r1, r0, r2

		strb r1, [r4, r5]
		cmp r1, #0
		bne NON_ZERO_NUM_FOUND
		b ZERO_NUM_FOUND
		NON_ZERO_NUM_FOUND:
			ldr r6, =top_found
			ldr r6, [r6]
			cmp r6, #0
			beq SET_FOUND_AND_TOP_NUM
			b END_SET_FOUND_AND_TOP_NUM
			SET_FOUND_AND_TOP_NUM:
				ldr r6, =top_found
				mov r7, #1
				str r7, [r6]
				ldr r6, =display_limit
				mov r7, #7
				sub r7, r7, r5
				str r7, [r6]
			END_SET_FOUND_AND_TOP_NUM:
			b END_NON_ZERO_NUM_FOUND
		ZERO_NUM_FOUND:

		END_NON_ZERO_NUM_FOUND:
		add r5, r5, #1
		mul r6, r1, r2
		sub r0, r0, r6
		mov r7, #10
		udiv r2, r2, r7
		cmp r5, #8
		beq END_WHILE_GET_NUM // break
		b WHILE_GET_NUM
	END_WHILE_GET_NUM:
	POP {r0-r7}
	bx lr
	// two special cases for function do_feb; I don't know where to put it, so here o'v'o
	FEB_ZERO_SPECIAL_CASE:
		ldr r1, =display_data
		strb r0, [r1, #7]
		ldr r1, =display_limit
		mov r0, #0
		str r0, [r1]
		b END_WHILE_GET_NUM
	FEB_EXPLOSION_SPECIAL_CASE:
		ldr r1, =display_data
		mov r0, #1
		strb r0, [r1, #7]
		mov r0, #10
		strb r0, [r1, #6]
		ldr r1, =display_limit
		mov r0, #1
		str r0, [r1]
		b END_WHILE_GET_NUM

delay:
	push {r0}
	ldr r0, =ONE_SEC
	ldr r0, [r0]
	WHILE_LOOP_DELAY:
		cmp r0, #0
		beq END_WHILE_LOOP_DELAY
		sub r0, r0, #1
		b WHILE_LOOP_DELAY
	END_WHILE_LOOP_DELAY:
	pop {r0}
	BX LR

reset_display_feb:
	PUSH {r0, r1}
	mov r0, #0
	ldr r1, =display_data
	strb r0, [r1, #0]
	strb r0, [r1, #1]
	strb r0, [r1, #2]
	strb r0, [r1, #3]
	strb r0, [r1, #4]
	strb r0, [r1, #5]
	strb r0, [r1, #6]
	strb r0, [r1, #7]
	ldr r1, =feb_first
	str r0, [r1]
	ldr r1, =display_limit
	str r0, [r1]
	ldr r1, =feb_second
	mov r0, #1
	str r0, [r1]
	PUSH {r0, r1}
	BX LR

check_button_init:
    ldr r0, =4000000
    movs r0, r0
    b check_button_delay

check_button_delay:
    beq check_button_init
    ldr r1, =0b11111111111111111
    ands r1, r0
    beq check_button
    subs r0, #8
    b check_button_delay

check_button:
    // r10 gpio button input
    // r11 latest button value
    // r12 confirmed button value
    ldrh r1, [r10]
    lsr  r1, 13
    mov  r4, 1
    ands r1, r4
    beq  check_button_increment
    cmp  r9, 31
    bge  LONG_BUTTON_PRESSED
    mov  r9, 0
    cmp  r1, r11
    mov  r11, r1
    beq  check_button_confirmed
    subs r0, 8
    b    check_button_delay
check_button_increment:
    add  r9, 1
    cmp  r1, r11
    mov  r11, r1
    beq  check_button_confirmed
    subs r0, 8
    b    check_button_delay
 
check_button_confirmed:
    subs r1, r11, r12
    cmp  r1, 1
    mov  r12, r11
    beq  check_button_end
    subs r0, 8
    b    check_button_delay
check_button_end:
	bx   lr

LONG_BUTTON_PRESSED:
	bl reset_display_feb
	b main

main:
    mov  r9, #0  // counter for long press
    mov  r11, #1
    mov  r12, #1
	BL GPIO_init
	BL MAX7219_init
	BL reset_display_feb
	BL display
	loooop:
		//BL delay
		mov r9, 0x0
		mov r11, 0x1
		mov r12, 0x1
		bl check_button_init
		BL do_feb
		BL set_display_array
		BL display
	b loooop


