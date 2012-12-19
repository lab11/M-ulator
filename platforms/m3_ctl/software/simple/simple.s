.cpu cortex-m0
.syntax unified
.thumb

.equ	GPIO_DATA,	0xA4000000
.equ	GPIO_DIR,	0xA4001000
.equ	GPIO_INT_MASK,	0xA4001040

.global main
.type main,#function
main:
	ldr.n	r3, [pc, #20]	@ r3 = GPIO_DIR
	ldr.n	r4, [pc, #24]	@ r4 = GPIO_DATA
	ldr.n	r0, [pc, #24]	@ r0 = 0xffff, GPIO 0-15 to output
	ldr.n	r1, [pc, #28]	@ r1 = 0x5555, GPIO 0-15 to striped
	str.n	r0, [r3]	@ Write r0 to GPIO_DIR
	str.n	r1, [r4]	@ Write r1 to GPIO_DATA
hang:
	nop
	nop
	nop
	nop
	b.n	hang
.align 2
mem:
	.word	0xA0000260
	.word	0x12345678
	.word	0x12345678
	.word	0x5555
/* 4 bytes data -> i2c addr 98 is 00 1001 1000 00 --> 0010 0110 0000 */
;	.word	GPIO_DIR
;	.word	GPIO_DATA
;	.word	0xffff
;	.word	0x5555
.end
