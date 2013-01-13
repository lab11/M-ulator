.cpu cortex-m0
.syntax unified
.thumb

.equ	GPIO_DATA,	0xA4000000
.equ	GPIO_DIR,	0xA4001000
.equ	GPIO_INT_MASK,	0xA4001040

.global main
.type main,#function
main:
	/* Twiddle some GPIOs */
/*
	ldr.n	r3, [pc, #28]	@ r3 = GPIO_DIR
	ldr.n	r4, [pc, #32]	@ r4 = GPIO_DATA
	ldr.n	r0, [pc, #32]	@ r0 = 0xffff, GPIO 0-15 to output
	ldr.n	r1, [pc, #36]	@ r1 = 0x5555, GPIO 0-15 to striped
	str.n	r0, [r3]	@ Write r0 to GPIO_DIR
	str.n	r1, [r4]	@ Write r1 to GPIO_DATA
*/
	/* Send an I2C message */
	ldr.n	r5, [pc, #32]	@ r5 = CPU-Issue I2C message Addr
	ldr.n	r6, [pc, #36]	@ r6 = Data to send
	str.n	r6, [r5]	@ Write r6 to r5
.align 2
hang:
	nop
	nop
	nop
	nop
	b.n	hang
.align 2
mem:
	.word	GPIO_DIR
	.word	GPIO_DATA
	.word	0xffff
	.word	0x5555
/* 4 bytes data -> i2c addr 98 is 00 1001 1000 00 --> 0010 0110 0000 */
	.word	0xA0000260
	.word	0xdeadbeef
.end
