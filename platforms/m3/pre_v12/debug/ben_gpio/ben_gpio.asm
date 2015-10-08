
ben_gpio/ben_gpio.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0x84>:
   0:	00002000 	.word	0x00002000
   4:	00000089 	.word	0x00000089
	...
  1c:	00000084 	.word	0x00000084
  20:	00000084 	.word	0x00000084
  24:	00000084 	.word	0x00000084
  28:	00000084 	.word	0x00000084
	...
  34:	00000084 	.word	0x00000084
	...

00000084 <hang>:
  84:	e7fe      	b.n	84 <hang>
	...

00000088 <_start>:
  88:	f000 f802 	bl	90 <main>
  8c:	e7fc      	b.n	88 <_start>

Disassembly of section .text.startup.main:

00000090 <main>:
  90:	4a0c      	ldr	r2, [pc, #48]	; (c4 <main+0x34>)
  92:	4b0d      	ldr	r3, [pc, #52]	; (c8 <main+0x38>)
  94:	b510      	push	{r4, lr}
  96:	601a      	str	r2, [r3, #0]
  98:	4a0c      	ldr	r2, [pc, #48]	; (cc <main+0x3c>)
  9a:	2300      	movs	r3, #0
  9c:	6013      	str	r3, [r2, #0]
  9e:	4a0c      	ldr	r2, [pc, #48]	; (d0 <main+0x40>)
  a0:	2120      	movs	r1, #32
  a2:	6013      	str	r3, [r2, #0]
  a4:	4b0b      	ldr	r3, [pc, #44]	; (d4 <main+0x44>)
  a6:	2203      	movs	r2, #3
  a8:	681b      	ldr	r3, [r3, #0]
  aa:	200f      	movs	r0, #15
  ac:	6019      	str	r1, [r3, #0]
  ae:	4b0a      	ldr	r3, [pc, #40]	; (d8 <main+0x48>)
  b0:	681b      	ldr	r3, [r3, #0]
  b2:	601a      	str	r2, [r3, #0]
  b4:	681c      	ldr	r4, [r3, #0]
  b6:	420c      	tst	r4, r1
  b8:	d001      	beq.n	be <main+0x2e>
  ba:	6018      	str	r0, [r3, #0]
  bc:	e7fa      	b.n	b4 <main+0x24>
  be:	601a      	str	r2, [r3, #0]
  c0:	e7f8      	b.n	b4 <main+0x24>
  c2:	46c0      	nop			; (mov r8, r8)
  c4:	0001ffff 	.word	0x0001ffff
  c8:	e000e280 	.word	0xe000e280
  cc:	e000e100 	.word	0xe000e100
  d0:	a0001200 	.word	0xa0001200
  d4:	000000e0 	.word	0x000000e0
  d8:	000000dc 	.word	0x000000dc
