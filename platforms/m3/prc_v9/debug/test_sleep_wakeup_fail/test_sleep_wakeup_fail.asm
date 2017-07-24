
test_sleep_wakeup_fail/test_sleep_wakeup_fail.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0x70>:
   0:	00000c00 	.word	0x00000c00
   4:	00000075 	.word	0x00000075
	...
  1c:	00000070 	.word	0x00000070
  20:	00000070 	.word	0x00000070
  24:	00000070 	.word	0x00000070
  28:	00000070 	.word	0x00000070
	...
  34:	00000070 	.word	0x00000070
	...

00000070 <hang>:
  70:	e7fe      	b.n	70 <hang>
	...

00000074 <_start>:
  74:	f000 f842 	bl	fc <main>
  78:	e7fc      	b.n	74 <_start>

Disassembly of section .text.delay:

0000007a <delay>:
  7a:	b500      	push	{lr}
  7c:	2300      	movs	r3, #0
  7e:	e001      	b.n	84 <delay+0xa>
  80:	46c0      	nop			; (mov r8, r8)
  82:	3301      	adds	r3, #1
  84:	4283      	cmp	r3, r0
  86:	d1fb      	bne.n	80 <delay+0x6>
  88:	bd00      	pop	{pc}

Disassembly of section .text.set_wakeup_timer:

0000008c <set_wakeup_timer>:
  8c:	b500      	push	{lr}
  8e:	2900      	cmp	r1, #0
  90:	d003      	beq.n	9a <set_wakeup_timer+0xe>
  92:	2380      	movs	r3, #128	; 0x80
  94:	021b      	lsls	r3, r3, #8
  96:	4318      	orrs	r0, r3
  98:	e001      	b.n	9e <set_wakeup_timer+0x12>
  9a:	0440      	lsls	r0, r0, #17
  9c:	0c40      	lsrs	r0, r0, #17
  9e:	4b04      	ldr	r3, [pc, #16]	; (b0 <set_wakeup_timer+0x24>)
  a0:	6018      	str	r0, [r3, #0]
  a2:	2a00      	cmp	r2, #0
  a4:	d002      	beq.n	ac <set_wakeup_timer+0x20>
  a6:	4b03      	ldr	r3, [pc, #12]	; (b4 <set_wakeup_timer+0x28>)
  a8:	2201      	movs	r2, #1
  aa:	601a      	str	r2, [r3, #0]
  ac:	bd00      	pop	{pc}
  ae:	46c0      	nop			; (mov r8, r8)
  b0:	a2000010 	.word	0xa2000010
  b4:	a2000014 	.word	0xa2000014

Disassembly of section .text.write_mbus_message:

000000b8 <write_mbus_message>:
  b8:	23a0      	movs	r3, #160	; 0xa0
  ba:	061b      	lsls	r3, r3, #24
  bc:	6018      	str	r0, [r3, #0]
  be:	4b02      	ldr	r3, [pc, #8]	; (c8 <write_mbus_message+0x10>)
  c0:	2000      	movs	r0, #0
  c2:	6019      	str	r1, [r3, #0]
  c4:	4770      	bx	lr
  c6:	46c0      	nop			; (mov r8, r8)
  c8:	a0000004 	.word	0xa0000004

Disassembly of section .text.enumerate:

000000cc <enumerate>:
  cc:	2280      	movs	r2, #128	; 0x80
  ce:	0600      	lsls	r0, r0, #24
  d0:	0592      	lsls	r2, r2, #22
  d2:	23a0      	movs	r3, #160	; 0xa0
  d4:	4302      	orrs	r2, r0
  d6:	061b      	lsls	r3, r3, #24
  d8:	2000      	movs	r0, #0
  da:	6018      	str	r0, [r3, #0]
  dc:	4b01      	ldr	r3, [pc, #4]	; (e4 <enumerate+0x18>)
  de:	601a      	str	r2, [r3, #0]
  e0:	4770      	bx	lr
  e2:	46c0      	nop			; (mov r8, r8)
  e4:	a0000004 	.word	0xa0000004

Disassembly of section .text.sleep:

000000e8 <sleep>:
  e8:	23a0      	movs	r3, #160	; 0xa0
  ea:	2201      	movs	r2, #1
  ec:	061b      	lsls	r3, r3, #24
  ee:	601a      	str	r2, [r3, #0]
  f0:	4b01      	ldr	r3, [pc, #4]	; (f8 <sleep+0x10>)
  f2:	2000      	movs	r0, #0
  f4:	6018      	str	r0, [r3, #0]
  f6:	4770      	bx	lr
  f8:	a0000004 	.word	0xa0000004

Disassembly of section .text.startup.main:

000000fc <main>:
  fc:	4a17      	ldr	r2, [pc, #92]	; (15c <main+0x60>)
  fe:	b508      	push	{r3, lr}
 100:	230f      	movs	r3, #15
 102:	6013      	str	r3, [r2, #0]
 104:	4a16      	ldr	r2, [pc, #88]	; (160 <main+0x64>)
 106:	2064      	movs	r0, #100	; 0x64
 108:	6013      	str	r3, [r2, #0]
 10a:	f7ff ffb6 	bl	7a <delay>
 10e:	21fa      	movs	r1, #250	; 0xfa
 110:	20aa      	movs	r0, #170	; 0xaa
 112:	f7ff ffd1 	bl	b8 <write_mbus_message>
 116:	2064      	movs	r0, #100	; 0x64
 118:	f7ff ffaf 	bl	7a <delay>
 11c:	2004      	movs	r0, #4
 11e:	f7ff ffd5 	bl	cc <enumerate>
 122:	2064      	movs	r0, #100	; 0x64
 124:	f7ff ffa9 	bl	7a <delay>
 128:	21fa      	movs	r1, #250	; 0xfa
 12a:	20aa      	movs	r0, #170	; 0xaa
 12c:	f7ff ffc4 	bl	b8 <write_mbus_message>
 130:	480c      	ldr	r0, [pc, #48]	; (164 <main+0x68>)
 132:	f7ff ffa2 	bl	7a <delay>
 136:	21fa      	movs	r1, #250	; 0xfa
 138:	20aa      	movs	r0, #170	; 0xaa
 13a:	f7ff ffbd 	bl	b8 <write_mbus_message>
 13e:	2200      	movs	r2, #0
 140:	2002      	movs	r0, #2
 142:	2101      	movs	r1, #1
 144:	f7ff ffa2 	bl	8c <set_wakeup_timer>
 148:	4b07      	ldr	r3, [pc, #28]	; (168 <main+0x6c>)
 14a:	2201      	movs	r2, #1
 14c:	601a      	str	r2, [r3, #0]
 14e:	2200      	movs	r2, #0
 150:	2368      	movs	r3, #104	; 0x68
 152:	601a      	str	r2, [r3, #0]
 154:	f7ff ffc8 	bl	e8 <sleep>
 158:	e7fe      	b.n	158 <main+0x5c>
 15a:	46c0      	nop			; (mov r8, r8)
 15c:	e000e280 	.word	0xe000e280
 160:	e000e100 	.word	0xe000e100
 164:	00004e20 	.word	0x00004e20
 168:	a2000014 	.word	0xa2000014
