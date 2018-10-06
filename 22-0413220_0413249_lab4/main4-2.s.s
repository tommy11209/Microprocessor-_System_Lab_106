.syntax    unified
    .cpu    cortex-m4
    .thumb

.data
    leds:        .byte    2
    a:           .word    0xffffffff
    direction:   .word    1

.text
    .global    main
    .equ    RCC_AHB2ENR,    0x4002104c
    .equ    time1,          0x3D090    //1s
    .equ    time2,          0x3A98         //10ms

    .equ    GPIOB_MODER,    0x48000400
    .equ    GPIOB_OTYPER,   0x48000404
    .equ    GPIOB_OSPEEDR,  0x48000408
    .equ    GPIOB_PUPDR,    0x4800040c
    .equ    GPIOB_ODR,      0x48000414

    .equ GPIOC_BOUNDARY, 0x48000800
    .equ GPIOC_MODER,    0x00 + GPIOC_BOUNDARY
    .equ GPIOC_OTYPER,   0x04 + GPIOC_BOUNDARY
    .equ GPIOC_OSPEEDR,  0x08 + GPIOC_BOUNDARY
    .equ GPIOC_PUPDR,    0x0C + GPIOC_BOUNDARY
    .equ GPIOC_IDR,      0x10 + GPIOC_BOUNDARY

main:
    bl    GPIO_init

loop:
    bl    displayLED
    bl    delay
    bl    L1
    b     loop

delay:

    push    {lr}
    ldr    r0, =time1
delay_L:

    ldr    r1, =GPIOC_IDR
    ldr    r1, [r1]
    lsrs   r1, 14
    it     cc
    blcc   wait

    ldr    r1, =a
    ldr    r1, [r1]
    cmp    r1, 0
    it     eq
    beq    delay_L
    subs   r0, r0, 1
    bne    delay_L
    pop    {pc}
wait:

    ldr    r1, =time2
deb_L:
    subs   r1, r1, 1
    bne    deb_L

    ldr    r1, =GPIOC_IDR
    ldr    r1, [r1]
    lsrs   r1, 14
    it     cc
    bcc    wait

    ldr    r1, =a
    ldr    r2, [r1]
    mvn    r2, r2
    str    r2, [r1]
    bx    lr

L1:
    ldr    r3, =direction
    ldr    r2, [r3]
    ldr    r0, =leds
    ldrb   r1, [r0]
    cmp    r1, 6
    it     eq
    mvneq  r2, 0
    cmp    r1, 2
    it     eq
    moveq  r2, 1
    adds   r1, r1, r2
    strb   r1, [r0]
    str    r2, [r3]
    bx     lr

displayLED:

    ldr     r1, =leds
    ldrb    r1, [r1]

    movs    r0, 0x3
    lsl     r0, r0, r1
    eor     r0, r0, #-1

    ldr     r1, =GPIOB_ODR
    strh    r0, [r1]
    BX      lr

GPIO_init:
    movs r0, 0b110
    ldr  r1, =RCC_AHB2ENR
    str  r0, [r1]

    movs r0, 0b01010101000000
    ldr  r1, =GPIOB_MODER
    ldr  r2, [r1]
    and  r2, 0xFFFFC03F
    orrs r2, r0
    str  r2, [r1]

    ldr  r1, =GPIOC_MODER
    ldr  r0, [r1]
    ldr  r2, =0xF3FFFFFF
    and  r0, r2
    str  r0, [r1]

    movs r0, 0b10101010000000
    ldr  r1, =GPIOB_OSPEEDR
    strh r0, [r1]

    ldr  r0, =GPIOB_ODR
    ldr  r10, =GPIOC_IDR
    bx   lr
