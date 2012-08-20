.equ LED_REG_ADDR, 0x40001000

.text
.syntax unified
.thumb

@ TTTA: Caller / callee saved registers?

@ r0 is offset from base addr
@ r1 is led_no
@ r2 is on / off
@ r3 is scratch and available

.global	LED_set
.type	LED_set, %function
LED_set:
	@ Load register address to modify
	ldr.n	r3, [pc, #20]	@ wonky as won't allow laddr here?
	adds.n	r3, r0		@ base_addr += offset
	@ Need to read out current LED state first
	ldr.n	r0, [r3, #0]
	@ Caller ensures r2 is only 0 or 1, so only need shift
	lsls.n	r2, r2, r1
	ittte	eq
	moveq	r2, #1
	lsleq	r2, r2, r1
	biceq	r0, r0, r2	@ Clears bit
	orrne	r0, r0, r2	@ Sets bit
	@ Write to LED register
	str.n	r0, [r3, #0]
	@ Done, return
	bx	lr
	adds.n	r0, #0		@ nop
	.word	LED_REG_ADDR
