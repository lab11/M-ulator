.cpu cortex-m0
.syntax unified
.thumb

/* Interrupt Vector Table (32kB SRAM) */
.section .vectors
.word	0x0FFC	@ stack top    /* reserve 8kB for software managed memory (raw_input, output, parameters, etc) */
.word	_start	@ reset vector
.word   handler_nmi          /* 2 NMI */
.word   handler_hard         /* 3 HardFault */
.word   hang                 /* 4 RESERVED */
.word   hang                 /* 5 RESERVED */
.word   hang                 /* 6 RESERVED */
.word   hang                 /* 7 RESERVED */
.word   hang                 /* 8 RESERVED */
.word   hang                 /* 9 RESERVED*/
.word   hang                 /* 10 RESERVED */
.word   handler_svcall       /* 11 SVCall */
.word   hang                 /* 12 RESERVED */
.word   hang                 /* 13 RESERVED */
.word   handler_pendsv       /* 14 PendSV */
.word   handler_systick      /* 15 SysTick */
.word   handler_ext_int_0    /* 16 External Interrupt(0) */
.word   handler_ext_int_1    /* 17 External Interrupt(1) */
.word   handler_ext_int_2    /* 18 External Interrupt(2) */
.word   handler_ext_int_3    /* 19 External Interrupt(3) */
.word   handler_ext_int_4    /* 20 External Interrupt(4) */
.word   handler_ext_int_5    /* 21 External Interrupt(5) */
.word   handler_ext_int_6    /* 22 External Interrupt(6) */
.word   handler_ext_int_7    /* 23 External Interrupt(7) */
.word   handler_ext_int_8    /* 24 External Interrupt(8) */
.word   handler_ext_int_9    /* 25 External Interrupt(9) */
.word   handler_ext_int_10   /* 26 External Interrupt(10) */
.word   handler_ext_int_11   /* 27 External Interrupt(11) */
.word   handler_ext_int_12   /* 28 External Interrupt(12) */
.word   handler_ext_int_13   /* 29 External Interrupt(13) */


.align 4
.thumb_func
hang:   b .

.weak handler_nmi, hang
.weak handler_hard, hang
.weak handler_svcall, hang
.weak handler_pendsv, hang
.weak handler_systick, hang
.weak handler_ext_int_0, hang
.weak handler_ext_int_1, hang
.weak handler_ext_int_2, hang
.weak handler_ext_int_3, hang
.weak handler_ext_int_4, hang
.weak handler_ext_int_5, hang
.weak handler_ext_int_6, hang
.weak handler_ext_int_7, hang
.weak handler_ext_int_8, hang
.weak handler_ext_int_9, hang
.weak handler_ext_int_10, hang
.weak handler_ext_int_11, hang
.weak handler_ext_int_12, hang
.weak handler_ext_int_13, hang

.text
.func _start
.global _start
_start:
	bl main		@ call main() function
	b _start	@ expect to never get here, but just in case restart
.endfunc

.end
