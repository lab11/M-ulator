
PRCv12_power_0/PRCv12_power_0.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0x80>:
   0:	00002000 	.word	0x00002000
   4:	00000091 	.word	0x00000091
	...
  1c:	00000080 	.word	0x00000080
  20:	00000080 	.word	0x00000080
  24:	00000080 	.word	0x00000080
  28:	00000080 	.word	0x00000080
	...
  34:	00000080 	.word	0x00000080
	...

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 f848 	bl	124 <main>
  94:	e7fc      	b.n	90 <_start>

Disassembly of section .text.delay:

00000096 <delay>:
  96:	b500      	push	{lr}
  98:	2300      	movs	r3, #0
  9a:	e001      	b.n	a0 <delay+0xa>
  9c:	46c0      	nop			; (mov r8, r8)
  9e:	3301      	adds	r3, #1
  a0:	4283      	cmp	r3, r0
  a2:	d1fb      	bne.n	9c <delay+0x6>
  a4:	bd00      	pop	{pc}

Disassembly of section .text.write_regfile:

000000a8 <write_regfile>:
  a8:	0880      	lsrs	r0, r0, #2
  aa:	0209      	lsls	r1, r1, #8
  ac:	b508      	push	{r3, lr}
  ae:	0a09      	lsrs	r1, r1, #8
  b0:	4b05      	ldr	r3, [pc, #20]	; (c8 <write_regfile+0x20>)
  b2:	0600      	lsls	r0, r0, #24
  b4:	4308      	orrs	r0, r1
  b6:	6018      	str	r0, [r3, #0]
  b8:	4b04      	ldr	r3, [pc, #16]	; (cc <write_regfile+0x24>)
  ba:	2210      	movs	r2, #16
  bc:	601a      	str	r2, [r3, #0]
  be:	200a      	movs	r0, #10
  c0:	f7ff ffe9 	bl	96 <delay>
  c4:	bd08      	pop	{r3, pc}
  c6:	46c0      	nop			; (mov r8, r8)
  c8:	a0002000 	.word	0xa0002000
  cc:	a000200c 	.word	0xa000200c

Disassembly of section .text.set_wakeup_timer:

000000d0 <set_wakeup_timer>:
  d0:	b510      	push	{r4, lr}
  d2:	1c14      	adds	r4, r2, #0
  d4:	2900      	cmp	r1, #0
  d6:	d003      	beq.n	e0 <set_wakeup_timer+0x10>
  d8:	2180      	movs	r1, #128	; 0x80
  da:	0209      	lsls	r1, r1, #8
  dc:	4301      	orrs	r1, r0
  de:	e001      	b.n	e4 <set_wakeup_timer+0x14>
  e0:	0440      	lsls	r0, r0, #17
  e2:	0c41      	lsrs	r1, r0, #17
  e4:	4804      	ldr	r0, [pc, #16]	; (f8 <set_wakeup_timer+0x28>)
  e6:	f7ff ffdf 	bl	a8 <write_regfile>
  ea:	2c00      	cmp	r4, #0
  ec:	d002      	beq.n	f4 <set_wakeup_timer+0x24>
  ee:	4b03      	ldr	r3, [pc, #12]	; (fc <set_wakeup_timer+0x2c>)
  f0:	2201      	movs	r2, #1
  f2:	601a      	str	r2, [r3, #0]
  f4:	bd10      	pop	{r4, pc}
  f6:	46c0      	nop			; (mov r8, r8)
  f8:	a0000034 	.word	0xa0000034
  fc:	a0001300 	.word	0xa0001300

Disassembly of section .text.mbus_sleep_all:

00000100 <mbus_sleep_all>:
 100:	4b01      	ldr	r3, [pc, #4]	; (108 <mbus_sleep_all+0x8>)
 102:	2200      	movs	r2, #0
 104:	601a      	str	r2, [r3, #0]
 106:	4770      	bx	lr
 108:	a0003010 	.word	0xa0003010

Disassembly of section .text.init_interrupt:

0000010c <init_interrupt>:
 10c:	4b02      	ldr	r3, [pc, #8]	; (118 <init_interrupt+0xc>)
 10e:	4a03      	ldr	r2, [pc, #12]	; (11c <init_interrupt+0x10>)
 110:	6013      	str	r3, [r2, #0]
 112:	4a03      	ldr	r2, [pc, #12]	; (120 <init_interrupt+0x14>)
 114:	6013      	str	r3, [r2, #0]
 116:	4770      	bx	lr
 118:	00007fff 	.word	0x00007fff
 11c:	e000e280 	.word	0xe000e280
 120:	e000e100 	.word	0xe000e100

Disassembly of section .text.startup.main:

00000124 <main>:
 124:	b508      	push	{r3, lr}
 126:	f7ff fff1 	bl	10c <init_interrupt>
 12a:	4805      	ldr	r0, [pc, #20]	; (140 <main+0x1c>)
 12c:	f7ff ffb3 	bl	96 <delay>
 130:	2101      	movs	r1, #1
 132:	2005      	movs	r0, #5
 134:	1c0a      	adds	r2, r1, #0
 136:	f7ff ffcb 	bl	d0 <set_wakeup_timer>
 13a:	f7ff ffe1 	bl	100 <mbus_sleep_all>
 13e:	e7fe      	b.n	13e <main+0x1a>
 140:	00030d40 	.word	0x00030d40
