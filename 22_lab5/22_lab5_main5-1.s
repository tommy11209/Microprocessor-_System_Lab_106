 	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	arr: .byte 0b01111110, 0b00110000, 0b01101101, 0b01111001, 0b00110011, 0b01011011, 0b01011111, 0b01110000, 0b01111111, 0b01111011, 0b01110111, 0b00011111, 0b01001110, 0b00111101, 0b01001111, 0b01000111
	ONE_SEC: .word 800000
	// arr: a0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, b, C, d, E, F

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
	.equ BSRR_OFFSET,  24
	.equ BRR_OFFSET,   40

gpio_init:
	mov r0, 0b00000000000000000000000000000001
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

	bx lr

MAX7219_init:
	push {r0, r1, r2, lr}

	ldr r0, =DECODE_MODE
	ldr r1, =0
	bl MAX7219_send

	ldr r0, =DISPLAY_TEST
	ldr r1, =0
	bl MAX7219_send

	ldr r0, =SCAN_LIMIT
	ldr r1, =0
	bl MAX7219_send

	ldr r0, =INTENSITY
	ldr r1, =10
	bl MAX7219_send

	ldr r0, =SHUTDOWN
	ldr r1, =1
	bl MAX7219_send

	pop {r0, r1, r2, pc}

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


main:
	bl gpio_init
	bl MAX7219_init

	mov r2, #0
	ldr r3, =arr
	mov r0, #1

	Display0toFLoop:
		ldrb r1, [r3, r2]
		bl MAX7219_send
		bl delay
		cmp r2, #15
		bne PLUS_ONE
		b TO_ZERO
		TO_ZERO:
			mov r2, #0
			b END_IFF
		PLUS_ONE:
			add r2, #1
		END_IFF:
		b Display0toFLoop


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


