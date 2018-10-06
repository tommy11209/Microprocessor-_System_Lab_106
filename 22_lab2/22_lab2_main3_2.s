.text
    .global main
.equ N, 20
.equ a,0
.equ b,1
.equ check, 100  //check edge 1~100
.equ check2, 1

fib:
    //TODO

    cmp r0, #1                   //do fib(n)
    ble	.L3
    sub r0, r0, #1
    add r3, r1, r2
    mov r1, r2
    mov r2, r3

    cmp r3, #0                 //check overflow of result
    ble .L2

    bl	fib
 .L2:                          //if overflow, set r4 to -2
    sub r4, r7, #2

    bl L
 .L3:                          //when the add of all terms finish, store the result to r4
    mov r4, r2
    bl L
main:
    movs R0, #N
    movs r5, #check            //temp reg foe edge 1 and 100
    movs r6, #check2

    movs r1, #a                //first term and second term of fib series
    movs r2, #b

    movs r7, #a                //temp reg

    cmp r5, r0                 //check weaher N is in 1~100 or not
    ble .L1
    cmp r0, r6
    ble .L1

    bl fib
 .L1:
    sub r4, r1, #1              //if N not in range, set r4 to -1
    bl L

L: b L
