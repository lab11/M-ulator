.cpu cortex-m3
.thumb
.syntax unified

/* TTTA: This program will work even with an empty memmap file, why? */

.word  0x00000000  /* TTTA: This will only work for this program, why? */
.word  _go

/* You should compile this and use objdump to see what actually gets created */

.align 2
.thumb_func
_go:
    add r0, r0, #4
loop:
    movs r1, r0
    sub r0, r1, #2
    bne  loop
    mov r0, #0
hang:    b .
