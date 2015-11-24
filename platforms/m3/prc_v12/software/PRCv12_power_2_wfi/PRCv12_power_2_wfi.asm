
PRCv12_power_2_wfi/PRCv12_power_2_wfi.elf:     file format elf32-littlearm


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
  90:	f000 f84a 	bl	128 <main>
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

Disassembly of section .text.WFI:

000000d0 <WFI>:
  d0:	bf30      	wfi
  d2:	4770      	bx	lr

Disassembly of section .text.set_wakeup_timer:

000000d4 <set_wakeup_timer>:
  d4:	b510      	push	{r4, lr}
  d6:	1c14      	adds	r4, r2, #0
  d8:	2900      	cmp	r1, #0
  da:	d003      	beq.n	e4 <set_wakeup_timer+0x10>
  dc:	2180      	movs	r1, #128	; 0x80
  de:	0209      	lsls	r1, r1, #8
  e0:	4301      	orrs	r1, r0
  e2:	e001      	b.n	e8 <set_wakeup_timer+0x14>
  e4:	0440      	lsls	r0, r0, #17
  e6:	0c41      	lsrs	r1, r0, #17
  e8:	4804      	ldr	r0, [pc, #16]	; (fc <set_wakeup_timer+0x28>)
  ea:	f7ff ffdd 	bl	a8 <write_regfile>
  ee:	2c00      	cmp	r4, #0
  f0:	d002      	beq.n	f8 <set_wakeup_timer+0x24>
  f2:	4b03      	ldr	r3, [pc, #12]	; (100 <set_wakeup_timer+0x2c>)
  f4:	2201      	movs	r2, #1
  f6:	601a      	str	r2, [r3, #0]
  f8:	bd10      	pop	{r4, pc}
  fa:	46c0      	nop			; (mov r8, r8)
  fc:	a0000034 	.word	0xa0000034
 100:	a0001300 	.word	0xa0001300

Disassembly of section .text.mbus_sleep_all:

00000104 <mbus_sleep_all>:
 104:	4b01      	ldr	r3, [pc, #4]	; (10c <mbus_sleep_all+0x8>)
 106:	2200      	movs	r2, #0
 108:	601a      	str	r2, [r3, #0]
 10a:	4770      	bx	lr
 10c:	a0003010 	.word	0xa0003010

Disassembly of section .text.init_interrupt:

00000110 <init_interrupt>:
 110:	4b02      	ldr	r3, [pc, #8]	; (11c <init_interrupt+0xc>)
 112:	4a03      	ldr	r2, [pc, #12]	; (120 <init_interrupt+0x10>)
 114:	6013      	str	r3, [r2, #0]
 116:	4a03      	ldr	r2, [pc, #12]	; (124 <init_interrupt+0x14>)
 118:	6013      	str	r3, [r2, #0]
 11a:	4770      	bx	lr
 11c:	00007fff 	.word	0x00007fff
 120:	e000e280 	.word	0xe000e280
 124:	e000e100 	.word	0xe000e100

Disassembly of section .text.startup.main:

00000128 <main>:
 128:	b508      	push	{r3, lr}
 12a:	f7ff fff1 	bl	110 <init_interrupt>
 12e:	2064      	movs	r0, #100	; 0x64
 130:	f7ff ffb1 	bl	96 <delay>
 134:	f7ff ffcc 	bl	d0 <WFI>
 138:	2101      	movs	r1, #1
 13a:	2005      	movs	r0, #5
 13c:	1c0a      	adds	r2, r1, #0
 13e:	f7ff ffc9 	bl	d4 <set_wakeup_timer>
 142:	f7ff ffdf 	bl	104 <mbus_sleep_all>
 146:	e7fe      	b.n	146 <main+0x1e>
