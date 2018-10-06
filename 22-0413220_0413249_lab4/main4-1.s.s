.syntax unified
    .cpu cortex-m4
    .thumb
.data
        leds:       .byte 0
        direction:    .byte 0
        .align
.text
    .global main
    .equ time ,    0x100000
    .equ RCC_AHB2ENR  , 0x4002104C
    .equ GPIOB_MODER  , 0x48000400
    .equ GPIOB_OTYPER , 0x48000404
    .equ GPIOB_OSPEEDR, 0x48000408
    .equ GPIOB_PUPDR  , 0x4800040C
    .equ GPIOB_ODR    , 0x48000414
main:
    BL   GPIO_init
    MOVS    R1, #1
    LDR R0, =leds
    STRB    R1, [R0]
loop:
    // Write the display pattern into leds variable
    ldr     r0, =leds
    ldr     r1, =direction
    ldrb    r2, [r0]
    ldrb    r3, [r1]
    cbz     r3, L2
    // shift lighte LED 1 position to left:
    lsls    r2, r2, #1
    cmp     r2, #0x18
    it      eq
    moveq   r2, #0x08
    cmp     r2, #0x10
    itt     eq
    moveq   r2, #0x0C
    moveq   r3, #0
    b       L1
    // shift lighted LED 1 position to right
L2:
    lsrs    r2, #1
    itt     eq
    moveq   r2, #3
    moveq   r3, #1
L1:
    strb    r2, [r0]
    strb    r3, [r1]
    BL      DisplayLED
    BL      delay
    B       loop
GPIO_init:
  //TODO: Initial LED GPIO pins as output
  movs r0, 1 << 1
  ldr  r1, =RCC_AHB2ENR
  str  r0, [r1]
  movs r0, #0x1540
  ldr  r1, =GPIOB_MODER
  ldr  r2, [r1]
  and  r2, 0xFFFFC03F
  orrs r2, r0
  str  r2, [r1]
  movs r0, #0x2a80
  ldr  r1, =GPIOB_OSPEEDR
  str  r0, [r1]
  ldr  r0, =GPIOB_ODR
  BX LR
DisplayLED  :
    ldr     r0, =leds
    ldrb    r1, [r0]
    lsl      r0, r1, #3    // shift 3 bit left
    eor     r0, r0, #-1   // flip all bits
    ldr     r1, =GPIOB_ODR
    strh    r0, [r1]
    BX      lr
    BX LR
delay:
    movs    r5, #0
    movw    r6, #0xc95      // 0xffffffff / 4M * 3 => 0xc95
delay_d:
    adds    r5, r5, r6      // 1 cycle
    bcc     delay_d         // 2 cycles
    BX      lr
