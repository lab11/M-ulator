
test_inhee_0/test_inhee_0.elf:     file format elf32-littlearm


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
  40:	00000181 	.word	0x00000181
  44:	0000018d 	.word	0x0000018d
  48:	00000199 	.word	0x00000199
  4c:	000001a5 	.word	0x000001a5
  50:	000001b1 	.word	0x000001b1
  54:	000001bd 	.word	0x000001bd
  58:	000001c9 	.word	0x000001c9
  5c:	000001d5 	.word	0x000001d5
  60:	000001e1 	.word	0x000001e1
  64:	000001f1 	.word	0x000001f1
  68:	00000201 	.word	0x00000201
  6c:	00000211 	.word	0x00000211
  70:	00000221 	.word	0x00000221
  74:	00000231 	.word	0x00000231
	...

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 f8ec 	bl	26c <main>
  94:	e7fc      	b.n	90 <_start>

Disassembly of section .text.set_wakeup_timer:

00000098 <set_wakeup_timer>:
  98:	b500      	push	{lr}
  9a:	2900      	cmp	r1, #0
  9c:	d003      	beq.n	a6 <set_wakeup_timer+0xe>
  9e:	23c0      	movs	r3, #192	; 0xc0
  a0:	029b      	lsls	r3, r3, #10
  a2:	4318      	orrs	r0, r3
  a4:	e001      	b.n	aa <set_wakeup_timer+0x12>
  a6:	0440      	lsls	r0, r0, #17
  a8:	0c40      	lsrs	r0, r0, #17
  aa:	4b04      	ldr	r3, [pc, #16]	; (bc <set_wakeup_timer+0x24>)
  ac:	6018      	str	r0, [r3, #0]
  ae:	2a00      	cmp	r2, #0
  b0:	d002      	beq.n	b8 <set_wakeup_timer+0x20>
  b2:	4b03      	ldr	r3, [pc, #12]	; (c0 <set_wakeup_timer+0x28>)
  b4:	2201      	movs	r2, #1
  b6:	601a      	str	r2, [r3, #0]
  b8:	bd00      	pop	{pc}
  ba:	46c0      	nop			; (mov r8, r8)
  bc:	a0000044 	.word	0xa0000044
  c0:	a0001300 	.word	0xa0001300

Disassembly of section .text.disable_all_irq:

000000c4 <disable_all_irq>:
  c4:	4a03      	ldr	r2, [pc, #12]	; (d4 <disable_all_irq+0x10>)
  c6:	2301      	movs	r3, #1
  c8:	425b      	negs	r3, r3
  ca:	6013      	str	r3, [r2, #0]
  cc:	4a02      	ldr	r2, [pc, #8]	; (d8 <disable_all_irq+0x14>)
  ce:	6013      	str	r3, [r2, #0]
  d0:	4770      	bx	lr
  d2:	46c0      	nop			; (mov r8, r8)
  d4:	e000e280 	.word	0xe000e280
  d8:	e000e180 	.word	0xe000e180

Disassembly of section .text.set_halt_until_mbus_rx:

000000dc <set_halt_until_mbus_rx>:
  dc:	4b04      	ldr	r3, [pc, #16]	; (f0 <set_halt_until_mbus_rx+0x14>)
  de:	4a05      	ldr	r2, [pc, #20]	; (f4 <set_halt_until_mbus_rx+0x18>)
  e0:	6819      	ldr	r1, [r3, #0]
  e2:	400a      	ands	r2, r1
  e4:	2190      	movs	r1, #144	; 0x90
  e6:	0209      	lsls	r1, r1, #8
  e8:	430a      	orrs	r2, r1
  ea:	601a      	str	r2, [r3, #0]
  ec:	4770      	bx	lr
  ee:	46c0      	nop			; (mov r8, r8)
  f0:	a0000028 	.word	0xa0000028
  f4:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.set_halt_until_mbus_tx:

000000f8 <set_halt_until_mbus_tx>:
  f8:	4b04      	ldr	r3, [pc, #16]	; (10c <set_halt_until_mbus_tx+0x14>)
  fa:	4a05      	ldr	r2, [pc, #20]	; (110 <set_halt_until_mbus_tx+0x18>)
  fc:	6819      	ldr	r1, [r3, #0]
  fe:	400a      	ands	r2, r1
 100:	21a0      	movs	r1, #160	; 0xa0
 102:	0209      	lsls	r1, r1, #8
 104:	430a      	orrs	r2, r1
 106:	601a      	str	r2, [r3, #0]
 108:	4770      	bx	lr
 10a:	46c0      	nop			; (mov r8, r8)
 10c:	a0000028 	.word	0xa0000028
 110:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.arb_debug_reg:

00000114 <arb_debug_reg>:
 114:	4b01      	ldr	r3, [pc, #4]	; (11c <arb_debug_reg+0x8>)
 116:	6018      	str	r0, [r3, #0]
 118:	4770      	bx	lr
 11a:	46c0      	nop			; (mov r8, r8)
 11c:	affffff8 	.word	0xaffffff8

Disassembly of section .text.mbus_write_message32:

00000120 <mbus_write_message32>:
 120:	4b02      	ldr	r3, [pc, #8]	; (12c <mbus_write_message32+0xc>)
 122:	0100      	lsls	r0, r0, #4
 124:	4318      	orrs	r0, r3
 126:	6001      	str	r1, [r0, #0]
 128:	2001      	movs	r0, #1
 12a:	4770      	bx	lr
 12c:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_write_message:

00000130 <mbus_write_message>:
 130:	2300      	movs	r3, #0
 132:	b500      	push	{lr}
 134:	429a      	cmp	r2, r3
 136:	d00a      	beq.n	14e <mbus_write_message+0x1e>
 138:	4b06      	ldr	r3, [pc, #24]	; (154 <mbus_write_message+0x24>)
 13a:	3a01      	subs	r2, #1
 13c:	0600      	lsls	r0, r0, #24
 13e:	4302      	orrs	r2, r0
 140:	601a      	str	r2, [r3, #0]
 142:	4b05      	ldr	r3, [pc, #20]	; (158 <mbus_write_message+0x28>)
 144:	2223      	movs	r2, #35	; 0x23
 146:	6019      	str	r1, [r3, #0]
 148:	4b04      	ldr	r3, [pc, #16]	; (15c <mbus_write_message+0x2c>)
 14a:	601a      	str	r2, [r3, #0]
 14c:	2301      	movs	r3, #1
 14e:	1c18      	adds	r0, r3, #0
 150:	bd00      	pop	{pc}
 152:	46c0      	nop			; (mov r8, r8)
 154:	a0002000 	.word	0xa0002000
 158:	a0002004 	.word	0xa0002004
 15c:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_enumerate:

00000160 <mbus_enumerate>:
 160:	0603      	lsls	r3, r0, #24
 162:	2080      	movs	r0, #128	; 0x80
 164:	0580      	lsls	r0, r0, #22
 166:	4318      	orrs	r0, r3
 168:	4b01      	ldr	r3, [pc, #4]	; (170 <mbus_enumerate+0x10>)
 16a:	6018      	str	r0, [r3, #0]
 16c:	4770      	bx	lr
 16e:	46c0      	nop			; (mov r8, r8)
 170:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

00000174 <mbus_sleep_all>:
 174:	4b01      	ldr	r3, [pc, #4]	; (17c <mbus_sleep_all+0x8>)
 176:	2200      	movs	r2, #0
 178:	601a      	str	r2, [r3, #0]
 17a:	4770      	bx	lr
 17c:	a0003010 	.word	0xa0003010

Disassembly of section .text.handler_ext_int_0:

00000180 <handler_ext_int_0>:
 180:	4b01      	ldr	r3, [pc, #4]	; (188 <handler_ext_int_0+0x8>)
 182:	2201      	movs	r2, #1
 184:	601a      	str	r2, [r3, #0]
 186:	4770      	bx	lr
 188:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

0000018c <handler_ext_int_1>:
 18c:	4b01      	ldr	r3, [pc, #4]	; (194 <handler_ext_int_1+0x8>)
 18e:	2202      	movs	r2, #2
 190:	601a      	str	r2, [r3, #0]
 192:	4770      	bx	lr
 194:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

00000198 <handler_ext_int_2>:
 198:	4b01      	ldr	r3, [pc, #4]	; (1a0 <handler_ext_int_2+0x8>)
 19a:	2204      	movs	r2, #4
 19c:	601a      	str	r2, [r3, #0]
 19e:	4770      	bx	lr
 1a0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

000001a4 <handler_ext_int_3>:
 1a4:	4b01      	ldr	r3, [pc, #4]	; (1ac <handler_ext_int_3+0x8>)
 1a6:	2208      	movs	r2, #8
 1a8:	601a      	str	r2, [r3, #0]
 1aa:	4770      	bx	lr
 1ac:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

000001b0 <handler_ext_int_4>:
 1b0:	4b01      	ldr	r3, [pc, #4]	; (1b8 <handler_ext_int_4+0x8>)
 1b2:	2210      	movs	r2, #16
 1b4:	601a      	str	r2, [r3, #0]
 1b6:	4770      	bx	lr
 1b8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

000001bc <handler_ext_int_5>:
 1bc:	4b01      	ldr	r3, [pc, #4]	; (1c4 <handler_ext_int_5+0x8>)
 1be:	2220      	movs	r2, #32
 1c0:	601a      	str	r2, [r3, #0]
 1c2:	4770      	bx	lr
 1c4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

000001c8 <handler_ext_int_6>:
 1c8:	4b01      	ldr	r3, [pc, #4]	; (1d0 <handler_ext_int_6+0x8>)
 1ca:	2240      	movs	r2, #64	; 0x40
 1cc:	601a      	str	r2, [r3, #0]
 1ce:	4770      	bx	lr
 1d0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

000001d4 <handler_ext_int_7>:
 1d4:	4b01      	ldr	r3, [pc, #4]	; (1dc <handler_ext_int_7+0x8>)
 1d6:	2280      	movs	r2, #128	; 0x80
 1d8:	601a      	str	r2, [r3, #0]
 1da:	4770      	bx	lr
 1dc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

000001e0 <handler_ext_int_8>:
 1e0:	4b02      	ldr	r3, [pc, #8]	; (1ec <handler_ext_int_8+0xc>)
 1e2:	2280      	movs	r2, #128	; 0x80
 1e4:	0052      	lsls	r2, r2, #1
 1e6:	601a      	str	r2, [r3, #0]
 1e8:	4770      	bx	lr
 1ea:	46c0      	nop			; (mov r8, r8)
 1ec:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

000001f0 <handler_ext_int_9>:
 1f0:	4b02      	ldr	r3, [pc, #8]	; (1fc <handler_ext_int_9+0xc>)
 1f2:	2280      	movs	r2, #128	; 0x80
 1f4:	0092      	lsls	r2, r2, #2
 1f6:	601a      	str	r2, [r3, #0]
 1f8:	4770      	bx	lr
 1fa:	46c0      	nop			; (mov r8, r8)
 1fc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

00000200 <handler_ext_int_10>:
 200:	4b02      	ldr	r3, [pc, #8]	; (20c <handler_ext_int_10+0xc>)
 202:	2280      	movs	r2, #128	; 0x80
 204:	00d2      	lsls	r2, r2, #3
 206:	601a      	str	r2, [r3, #0]
 208:	4770      	bx	lr
 20a:	46c0      	nop			; (mov r8, r8)
 20c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

00000210 <handler_ext_int_11>:
 210:	4b02      	ldr	r3, [pc, #8]	; (21c <handler_ext_int_11+0xc>)
 212:	2280      	movs	r2, #128	; 0x80
 214:	0112      	lsls	r2, r2, #4
 216:	601a      	str	r2, [r3, #0]
 218:	4770      	bx	lr
 21a:	46c0      	nop			; (mov r8, r8)
 21c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

00000220 <handler_ext_int_12>:
 220:	4b02      	ldr	r3, [pc, #8]	; (22c <handler_ext_int_12+0xc>)
 222:	2280      	movs	r2, #128	; 0x80
 224:	0152      	lsls	r2, r2, #5
 226:	601a      	str	r2, [r3, #0]
 228:	4770      	bx	lr
 22a:	46c0      	nop			; (mov r8, r8)
 22c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

00000230 <handler_ext_int_13>:
 230:	4b02      	ldr	r3, [pc, #8]	; (23c <handler_ext_int_13+0xc>)
 232:	2280      	movs	r2, #128	; 0x80
 234:	0192      	lsls	r2, r2, #6
 236:	601a      	str	r2, [r3, #0]
 238:	4770      	bx	lr
 23a:	46c0      	nop			; (mov r8, r8)
 23c:	e000e280 	.word	0xe000e280

Disassembly of section .text.initialization:

00000240 <initialization>:
 240:	b508      	push	{r3, lr}
 242:	4a07      	ldr	r2, [pc, #28]	; (260 <initialization+0x20>)
 244:	4b07      	ldr	r3, [pc, #28]	; (264 <initialization+0x24>)
 246:	601a      	str	r2, [r3, #0]
 248:	4b07      	ldr	r3, [pc, #28]	; (268 <initialization+0x28>)
 24a:	2200      	movs	r2, #0
 24c:	601a      	str	r2, [r3, #0]
 24e:	f7ff ff45 	bl	dc <set_halt_until_mbus_rx>
 252:	2004      	movs	r0, #4
 254:	f7ff ff84 	bl	160 <mbus_enumerate>
 258:	f7ff ff4e 	bl	f8 <set_halt_until_mbus_tx>
 25c:	bd08      	pop	{r3, pc}
 25e:	46c0      	nop			; (mov r8, r8)
 260:	deadbeef 	.word	0xdeadbeef
 264:	000002e4 	.word	0x000002e4
 268:	000002e0 	.word	0x000002e0

Disassembly of section .text.startup.main:

0000026c <main>:
 26c:	b530      	push	{r4, r5, lr}
 26e:	4a14      	ldr	r2, [pc, #80]	; (2c0 <main+0x54>)
 270:	b085      	sub	sp, #20
 272:	4669      	mov	r1, sp
 274:	ca31      	ldmia	r2!, {r0, r4, r5}
 276:	c131      	stmia	r1!, {r0, r4, r5}
 278:	6812      	ldr	r2, [r2, #0]
 27a:	600a      	str	r2, [r1, #0]
 27c:	f7ff ff22 	bl	c4 <disable_all_irq>
 280:	4b10      	ldr	r3, [pc, #64]	; (2c4 <main+0x58>)
 282:	681a      	ldr	r2, [r3, #0]
 284:	4b10      	ldr	r3, [pc, #64]	; (2c8 <main+0x5c>)
 286:	429a      	cmp	r2, r3
 288:	d001      	beq.n	28e <main+0x22>
 28a:	f7ff ffd9 	bl	240 <initialization>
 28e:	4c0f      	ldr	r4, [pc, #60]	; (2cc <main+0x60>)
 290:	20e1      	movs	r0, #225	; 0xe1
 292:	6821      	ldr	r1, [r4, #0]
 294:	f7ff ff44 	bl	120 <mbus_write_message32>
 298:	4669      	mov	r1, sp
 29a:	2204      	movs	r2, #4
 29c:	20e2      	movs	r0, #226	; 0xe2
 29e:	f7ff ff47 	bl	130 <mbus_write_message>
 2a2:	6823      	ldr	r3, [r4, #0]
 2a4:	2101      	movs	r1, #1
 2a6:	3301      	adds	r3, #1
 2a8:	2003      	movs	r0, #3
 2aa:	1c0a      	adds	r2, r1, #0
 2ac:	6023      	str	r3, [r4, #0]
 2ae:	f7ff fef3 	bl	98 <set_wakeup_timer>
 2b2:	f7ff ff5f 	bl	174 <mbus_sleep_all>
 2b6:	4804      	ldr	r0, [pc, #16]	; (2c8 <main+0x5c>)
 2b8:	f7ff ff2c 	bl	114 <arb_debug_reg>
 2bc:	46c0      	nop			; (mov r8, r8)
 2be:	e7fa      	b.n	2b6 <main+0x4a>
 2c0:	000002d0 	.word	0x000002d0
 2c4:	000002e4 	.word	0x000002e4
 2c8:	deadbeef 	.word	0xdeadbeef
 2cc:	000002e0 	.word	0x000002e0
