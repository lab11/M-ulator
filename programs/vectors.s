.cpu cortex-m3
.syntax unified
.thumb

.word   0x2003fffc  /* stack top address */
.word   _start      /* 1 Reset */
.word   hang        /* 2 NMI */
.word   hang        /* 3 HardFault */
.word   hang        /* 4 MemManage */
.word   hang        /* 5 BusFault */
.word   hang        /* 6 UsageFault */
.word   hang        /* 7 RESERVED */
.word   hang        /* 8 RESERVED */
.word   hang        /* 9 RESERVED*/
.word   hang        /* 10 RESERVED */
.word   hang        /* 11 SVCall */
.word   hang        /* 12 Debug Monitor */
.word   hang        /* 13 RESERVED */
.word   hang        /* 14 PendSV */
.word   hang        /* 15 SysTick */
.word   hang        /* 16 External Interrupt(0) */
.word   hang        /* 17 External Interrupt(1) */
.word   hang        /* 18 External Interrupt(2) */
.word   hang        /* 19 ...   */


.align 2
hang:   b .

.align 2
.global _start
_start:
    bl main
    b hang

.global memcpy
memcpy:
	push	{r4, lr}
	mov	r3, r0
	b.n	lend
lbegin:
	ldrb.w	r4, [r1], #1
	subs	r2, #1
	strb.w	r4, [r3], #1
lend:
	cmp	r2, #0
	bne.n	lbegin
	pop	{r4, pc}

	.align	2
	.global	memset
	.thumb
	.thumb_func
	.type	memset, %function
memset:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r7}
	sub	sp, sp, #28
	add	r7, sp, #0
	str	r0, [r7, #12]
	str	r1, [r7, #8]
	str	r2, [r7, #4]
	ldr	r3, [r7, #12]
	str	r3, [r7, #20]
	b	.L2
.L3:
	ldr	r3, [r7, #8]
	uxtb	r2, r3
	ldr	r3, [r7, #20]
	strb	r2, [r3, #0]
	ldr	r3, [r7, #20]
	add	r3, r3, #1
	str	r3, [r7, #20]
.L2:
	ldr	r3, [r7, #4]
	cmp	r3, #0
	ite	eq
	moveq	r3, #0
	movne	r3, #1
	uxtb	r3, r3
	ldr	r2, [r7, #4]
	add	r2, r2, #-1
	str	r2, [r7, #4]
	cmp	r3, #0
	bne	.L3
	ldr	r3, [r7, #12]
	mov	r0, r3
	add	r7, r7, #28
	mov	sp, r7
	pop	{r7}
	bx	lr

.end
