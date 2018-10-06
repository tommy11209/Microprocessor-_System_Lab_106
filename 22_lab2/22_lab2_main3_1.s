
.data
    result: .byte 0
.text
    .global main
.equ X, 0x55AA  //101010110101010
.equ Y, 0xAA55 //1010101001010101
hamm:
    //TODO
  cmp r3, #0
  beq .L1
  add r2, r2, #1
  mov r5, r3
  sub r3, r3, #1
  and r3, r3, r5
    bl hamm
.L1:
    ldr r6, =result
    str R2, [R6]
    b L
main:
  ldr r2, =result
  ldr r2, [r2]
  ldr r0, =X
  ldr r1, =Y
  eor r1, r1, r0
  mov r3, r1
  bl hamm
L: b L
