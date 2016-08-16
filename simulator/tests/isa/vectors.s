.syntax unified

vector_table:
.word   0x20007ffc          /* stack top address */
.word   _start              /* 1 Reset */

.align 2
.thumb_func
hang:   b .

.align 2
.thumb_func
.global _start
_start:
    // Call test case
    bl main

    // Spin
    b hang

.end
