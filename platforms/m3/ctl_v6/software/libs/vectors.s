.cpu cortex-m0
.syntax unified
.thumb

/* Interrupt Vector Table */
.section .vectors
.word	0x0800	@ stack top
.word	_start	@ reset vector
.word   handler_nmi         /* 2 NMI */
.word   handler_hard        /* 3 HardFault */
.word   handler_memmange    /* 4 MemManage */
.word   handler_bus         /* 5 BusFault */
.word   handler_usage       /* 6 UsageFault */
.word   hang                /* 7 RESERVED */
.word   hang                /* 8 RESERVED */
.word   hang                /* 9 RESERVED*/
.word   hang                /* 10 RESERVED */
.word   handler_svcall      /* 11 SVCall */
.word   handler_debug_mon   /* 12 Debug Monitor */
.word   hang                /* 13 RESERVED */
.word   handler_pendsv      /* 14 PendSV */
.word   handler_systick     /* 15 SysTick */
.word   handler_ext_int_0   /* 16 External Interrupt(0) */
.word   handler_ext_int_1   /* 17 External Interrupt(1) */
.word   handler_ext_int_2   /* 18 External Interrupt(2) */
.word   handler_ext_int_3   /* 19 ...   */

.align 2
.thumb_func
hang:   b .

.thumb_func
.weak handler_nmi
handler_nmi:
    b hang

.thumb_func
.weak handler_hard
handler_hard:
    b hang

.thumb_func
.weak handler_memmange
handler_memmange:
    b hang

.thumb_func
.weak handler_bus
handler_bus:
    b hang

.thumb_func
.weak handler_usage
handler_usage:
    b hang

.thumb_func
.weak handler_svcall
handler_svcall:
    b hang

.thumb_func
.weak handler_debug_mon
handler_debug_mon:
    b hang

.thumb_func
.weak handler_pendsv
handler_pendsv:
    b hang

.thumb_func
.weak handler_systick
handler_systick:
    b hang

.thumb_func
.weak handler_ext_int_0
handler_ext_int_0:
    bx lr

.thumb_func
.weak handler_ext_int_1
handler_ext_int_1:
    bx lr

.thumb_func
.weak handler_ext_int_2
handler_ext_int_2:
    bx lr

.thumb_func
.weak handler_ext_int_3
handler_ext_int_3:
    bx lr

.text
.type _start,#function
.global _start
_start:
	bl main
	movs r0, #2	@ PMU sleep request is '2'
//	bl PMU_req
	b _start	@ expect to never get here, but just in case
.end
