.cpu cortex-m0
.syntax unified
.thumb

/* Interrupt Vector Table for PREv22E (16kB SRAM) */
.section .vectors
.word	0x4000	@ stack top       /*  0 (0x00) */
.word	_start	@ reset vector    /*  1 (0x04) */
.word   handler_nmi               /*  2 (0x08) NMI */
.word   handler_hard              /*  3 (0x0C) HardFault */
.word   hang                      /*  4 (0x10) RESERVED */
.word   hang                      /*  5 (0x14) RESERVED */
.word   hang                      /*  6 (0x18) RESERVED */
.word   hang                      /*  7 (0x1C) RESERVED */
.word   hang                      /*  8 (0x20) RESERVED */
.word   hang                      /*  9 (0x24) RESERVED*/
.word   hang                      /* 10 (0x28) RESERVED */
.word   handler_svcall            /* 11 (0x2C) SVCall */
.word   hang                      /* 12 (0x30) RESERVED */
.word   hang                      /* 13 (0x34) RESERVED */
.word   handler_pendsv            /* 14 (0x38) PendSV */
.word   handler_systick           /* 15 (0x3C) SysTick */
.word   handler_ext_int_wakeup    /* 16 (0x40) External Interrupt(0)  */
.word   handler_ext_int_softreset /* 17 (0x44) External Interrupt(1)  */
.word   handler_ext_int_gocep     /* 18 (0x48) External Interrupt(2)  */
.word   handler_ext_int_timer32   /* 19 (0x4C) External Interrupt(3)  */
.word   handler_ext_int_timer16   /* 20 (0x50) External Interrupt(4)  */
.word   handler_ext_int_mbustx    /* 21 (0x54) External Interrupt(5)  */
.word   handler_ext_int_mbusrx    /* 22 (0x58) External Interrupt(6)  */
.word   handler_ext_int_mbusfwd   /* 23 (0x5C) External Interrupt(7)  */
.word   handler_ext_int_wfi       /* 24 (0x60) External Interrupt(8)  */
.word   handler_ext_int_wfi       /* 25 (0x64) External Interrupt(9)  */
.word   handler_ext_int_wfi       /* 26 (0x68) External Interrupt(10) */
.word   handler_ext_int_reg3      /* 27 (0x6C) External Interrupt(11) */
.word   handler_ext_int_reg4      /* 28 (0x70) External Interrupt(12) */
.word   handler_ext_int_wfi       /* 29 (0x74) External Interrupt(13) */
.word   handler_ext_int_reg6      /* 30 (0x78) External Interrupt(14) */
.word   handler_ext_int_reg7      /* 31 (0x7C) External Interrupt(15) */
.word   handler_ext_int_mbusmem   /* 32 (0x80) External Interrupt(16) */
.word   handler_ext_int_wfi       /* 33 (0x84) External Interrupt(17) */
.word   handler_ext_int_gpio      /* 34 (0x88) External Interrupt(18) */
.word   handler_ext_int_spi       /* 35 (0x8C) External Interrupt(19) */
.word   handler_ext_int_xot       /* 36 (0x90) External Interrupt(20) */


.align 4
.thumb_func
hang:   b .

.weak handler_nmi, hang
.weak handler_hard, hang
.weak handler_svcall, hang
.weak handler_pendsv, hang
.weak handler_systick, hang
.weak handler_ext_int_wakeup, hang
.weak handler_ext_int_softreset, hang
.weak handler_ext_int_gocep, hang
.weak handler_ext_int_timer32, hang
.weak handler_ext_int_timer16, hang
.weak handler_ext_int_mbustx, hang
.weak handler_ext_int_mbusrx, hang
.weak handler_ext_int_mbusfwd, hang
.weak handler_ext_int_wfi, hang
.weak handler_ext_int_reg3, hang
.weak handler_ext_int_reg4, hang
.weak handler_ext_int_reg6, hang
.weak handler_ext_int_reg7, hang
.weak handler_ext_int_mbusmem, hang
.weak handler_ext_int_gpio, hang
.weak handler_ext_int_spi, hang
.weak handler_ext_int_xot, hang

.text
.func _start
.global _start
_start:
	bl main		@ call main() function
	b _start	@ expect to never get here, but just in case restart
.endfunc

.end
