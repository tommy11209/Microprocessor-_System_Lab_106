.data
arr1: .byte 0x19, 0x34, 0x14, 0x32, 0x52, 0x23, 0x61, 0x29
arr2: .byte 0x18, 0x17, 0x33, 0x16, 0xFA, 0x20, 0x55, 0xAC
.text
    .global main
do_sort:
    //TODO
.L6:
    cmp r2, r1    // i < n
    bge .L2
    mov r3, #0    // j = 0   (#12)
   // mov r4, r3    /temp j
.L5:
    sub r4, r1, r2    // n - i
    cmp r4, r3        // j < (n-i)
    ble .L3
    add r5, r0, r3    //r5= [a + j]
    mov r7, r5
    ldrb r5, [r5]
    add r3, r3, #1    // j+1
    add r6, r0, r3    //r6= [a+j +1]
    ldrb r6, [r6]
    sub r3, r3, #1
    cmp r5, r6
    ble .L4
    strb r6, [r7]
    add r7, r7, #1
    strb r5, [r7]
.L4:
    add r3, r3, #1
    b .L5
.L3:
    add r2, r2, #1   //i++
    b .L6
.L2:
    bx lr

main:
    ldr r0, =arr1

    mov r1, #8    //8 numbers  n = 8   (#16)
    mov r2, #1    //i = 1          (#8)

    bl do_sort


    ldr r0, =arr2
    mov r1, #8    //8 numbers  n = 8   (#16)
    mov r2, #1    //i = 1          (#8)
    bl do_sort
L: b L
