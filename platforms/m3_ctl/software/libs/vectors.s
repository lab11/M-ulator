.cpu cortex-m0
.syntax unified
.thumb

/* Interrupt Vector Table */
.section .vectors
.word	0x0100	@ stack top
.word	_start	@ reset vector
.word   _start        /* 2 NMI */
.word   _start        /* 3 HardFault */
.word   _start        /* 4 MemManage */
.word   _start        /* 5 BusFault */
.word   _start        /* 6 UsageFault */
.word   _start        /* 7 RESERVED */
.word   _start        /* 8 RESERVED */
.word   _start        /* 9 RESERVED*/
.word   _start        /* 10 RESERVED */
.word   _start        /* 11 SVCall */
.word   _start        /* 12 Debug Monitor */
.word   _start        /* 13 RESERVED */
.word   _start        /* 14 PendSV */
.word   _start        /* 15 SysTick */
.word   _start        /* 16 External Interrupt(0) */
.word   _start        /* 17 External Interrupt(1) */
.word   _start        /* 18 External Interrupt(2) */
.word   _start        /* 19 External Interrupt(3) */

.text
.type _start,#function
.global _start
_start:
	bl main
	movs r0, #2	@ PMU sleep request is '2'
	bl PMU_req
	b _start	@ expect to never get here, but just in case
.end
