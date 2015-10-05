
FLSv1L_test2/FLSv1L_test2.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0x80>:
   0:	00002000 	.word	0x00002000
   4:	00000091 	.word	0x00000091
	...
  10:	00000080 	.word	0x00000080
  14:	00000080 	.word	0x00000080
  18:	00000080 	.word	0x00000080
  1c:	00000080 	.word	0x00000080
  20:	00000080 	.word	0x00000080
  24:	00000080 	.word	0x00000080
  28:	00000080 	.word	0x00000080
  2c:	00000000 	.word	0x00000000
  30:	00000080 	.word	0x00000080
  34:	00000080 	.word	0x00000080
	...

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 f802 	bl	98 <main>
  94:	e7fc      	b.n	90 <_start>

Disassembly of section .text.startup.main:

00000098 <main>:
  98:	b570      	push	{r4, r5, r6, lr}
  9a:	4a08      	ldr	r2, [pc, #32]	; (bc <main+0x24>)
  9c:	4908      	ldr	r1, [pc, #32]	; (c0 <main+0x28>)
  9e:	4809      	ldr	r0, [pc, #36]	; (c4 <main+0x2c>)
  a0:	4d09      	ldr	r5, [pc, #36]	; (c8 <main+0x30>)
  a2:	2400      	movs	r4, #0
  a4:	2623      	movs	r6, #35	; 0x23
  a6:	23fa      	movs	r3, #250	; 0xfa
  a8:	6011      	str	r1, [r2, #0]
  aa:	009b      	lsls	r3, r3, #2
  ac:	6004      	str	r4, [r0, #0]
  ae:	602e      	str	r6, [r5, #0]
  b0:	46c0      	nop			; (mov r8, r8)
  b2:	3b01      	subs	r3, #1
  b4:	2b00      	cmp	r3, #0
  b6:	d1fb      	bne.n	b0 <main+0x18>
  b8:	e7f5      	b.n	a6 <main+0xe>
  ba:	46c0      	nop			; (mov r8, r8)
  bc:	a0002000 	.word	0xa0002000
  c0:	aa000032 	.word	0xaa000032
  c4:	a0002004 	.word	0xa0002004
  c8:	a000200c 	.word	0xa000200c
