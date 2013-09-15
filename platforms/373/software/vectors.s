/* Copyright (c) 2011-2012  Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

.cpu cortex-m3
.syntax unified
.thumb

.word   0x20007ffc          /* stack top address */
.word   _start              /* 1 Reset */
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
.word   hang                /* 17 External Interrupt(1) */
.word   hang                /* 18 External Interrupt(2) */
.word   hang                /* 19 ...   */


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
    b hang

.align 2
.thumb_func
.global _start
_start:
    bl main
    b hang

	.align 2
	.global memcpy
	.thumb
	.thumb_func
	.type	memcpy, %function
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
