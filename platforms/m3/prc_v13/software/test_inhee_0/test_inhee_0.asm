
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
  40:	00000161 	.word	0x00000161
  44:	0000016d 	.word	0x0000016d
  48:	00000179 	.word	0x00000179
  4c:	00000185 	.word	0x00000185
  50:	00000191 	.word	0x00000191
  54:	0000019d 	.word	0x0000019d
  58:	000001a9 	.word	0x000001a9
  5c:	000001b5 	.word	0x000001b5
  60:	000001c1 	.word	0x000001c1
  64:	000001d1 	.word	0x000001d1
  68:	000001e1 	.word	0x000001e1
  6c:	000001f1 	.word	0x000001f1
  70:	00000201 	.word	0x00000201
  74:	00000211 	.word	0x00000211
	...

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 f90a 	bl	2a8 <main>
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

Disassembly of section .text.set_wakeup_timer:

000000a8 <set_wakeup_timer>:
  a8:	b500      	push	{lr}
  aa:	2900      	cmp	r1, #0
  ac:	d003      	beq.n	b6 <set_wakeup_timer+0xe>
  ae:	23c0      	movs	r3, #192	; 0xc0
  b0:	029b      	lsls	r3, r3, #10
  b2:	4318      	orrs	r0, r3
  b4:	e001      	b.n	ba <set_wakeup_timer+0x12>
  b6:	0440      	lsls	r0, r0, #17
  b8:	0c40      	lsrs	r0, r0, #17
  ba:	4b04      	ldr	r3, [pc, #16]	; (cc <set_wakeup_timer+0x24>)
  bc:	6018      	str	r0, [r3, #0]
  be:	2a00      	cmp	r2, #0
  c0:	d002      	beq.n	c8 <set_wakeup_timer+0x20>
  c2:	4b03      	ldr	r3, [pc, #12]	; (d0 <set_wakeup_timer+0x28>)
  c4:	2201      	movs	r2, #1
  c6:	601a      	str	r2, [r3, #0]
  c8:	bd00      	pop	{pc}
  ca:	46c0      	nop			; (mov r8, r8)
  cc:	a0000044 	.word	0xa0000044
  d0:	a0001300 	.word	0xa0001300

Disassembly of section .text.disable_all_irq:

000000d4 <disable_all_irq>:
  d4:	4a03      	ldr	r2, [pc, #12]	; (e4 <disable_all_irq+0x10>)
  d6:	2301      	movs	r3, #1
  d8:	425b      	negs	r3, r3
  da:	6013      	str	r3, [r2, #0]
  dc:	4a02      	ldr	r2, [pc, #8]	; (e8 <disable_all_irq+0x14>)
  de:	6013      	str	r3, [r2, #0]
  e0:	4770      	bx	lr
  e2:	46c0      	nop			; (mov r8, r8)
  e4:	e000e280 	.word	0xe000e280
  e8:	e000e180 	.word	0xe000e180

Disassembly of section .text.set_halt_disable:

000000ec <set_halt_disable>:
  ec:	4b03      	ldr	r3, [pc, #12]	; (fc <set_halt_disable+0x10>)
  ee:	22f0      	movs	r2, #240	; 0xf0
  f0:	6819      	ldr	r1, [r3, #0]
  f2:	0212      	lsls	r2, r2, #8
  f4:	430a      	orrs	r2, r1
  f6:	601a      	str	r2, [r3, #0]
  f8:	4770      	bx	lr
  fa:	46c0      	nop			; (mov r8, r8)
  fc:	a0000028 	.word	0xa0000028

Disassembly of section .text.mbus_write_message32:

00000100 <mbus_write_message32>:
 100:	4b02      	ldr	r3, [pc, #8]	; (10c <mbus_write_message32+0xc>)
 102:	0100      	lsls	r0, r0, #4
 104:	4318      	orrs	r0, r3
 106:	6001      	str	r1, [r0, #0]
 108:	2001      	movs	r0, #1
 10a:	4770      	bx	lr
 10c:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_write_message:

00000110 <mbus_write_message>:
 110:	2300      	movs	r3, #0
 112:	b500      	push	{lr}
 114:	429a      	cmp	r2, r3
 116:	d00a      	beq.n	12e <mbus_write_message+0x1e>
 118:	4b06      	ldr	r3, [pc, #24]	; (134 <mbus_write_message+0x24>)
 11a:	3a01      	subs	r2, #1
 11c:	0600      	lsls	r0, r0, #24
 11e:	4302      	orrs	r2, r0
 120:	601a      	str	r2, [r3, #0]
 122:	4b05      	ldr	r3, [pc, #20]	; (138 <mbus_write_message+0x28>)
 124:	2223      	movs	r2, #35	; 0x23
 126:	6019      	str	r1, [r3, #0]
 128:	4b04      	ldr	r3, [pc, #16]	; (13c <mbus_write_message+0x2c>)
 12a:	601a      	str	r2, [r3, #0]
 12c:	2301      	movs	r3, #1
 12e:	1c18      	adds	r0, r3, #0
 130:	bd00      	pop	{pc}
 132:	46c0      	nop			; (mov r8, r8)
 134:	a0002000 	.word	0xa0002000
 138:	a0002004 	.word	0xa0002004
 13c:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_enumerate:

00000140 <mbus_enumerate>:
 140:	0603      	lsls	r3, r0, #24
 142:	2080      	movs	r0, #128	; 0x80
 144:	0580      	lsls	r0, r0, #22
 146:	4318      	orrs	r0, r3
 148:	4b01      	ldr	r3, [pc, #4]	; (150 <mbus_enumerate+0x10>)
 14a:	6018      	str	r0, [r3, #0]
 14c:	4770      	bx	lr
 14e:	46c0      	nop			; (mov r8, r8)
 150:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

00000154 <mbus_sleep_all>:
 154:	4b01      	ldr	r3, [pc, #4]	; (15c <mbus_sleep_all+0x8>)
 156:	2200      	movs	r2, #0
 158:	601a      	str	r2, [r3, #0]
 15a:	4770      	bx	lr
 15c:	a0003010 	.word	0xa0003010

Disassembly of section .text.handler_ext_int_0:

00000160 <handler_ext_int_0>:
 160:	4b01      	ldr	r3, [pc, #4]	; (168 <handler_ext_int_0+0x8>)
 162:	2201      	movs	r2, #1
 164:	601a      	str	r2, [r3, #0]
 166:	4770      	bx	lr
 168:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

0000016c <handler_ext_int_1>:
 16c:	4b01      	ldr	r3, [pc, #4]	; (174 <handler_ext_int_1+0x8>)
 16e:	2202      	movs	r2, #2
 170:	601a      	str	r2, [r3, #0]
 172:	4770      	bx	lr
 174:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

00000178 <handler_ext_int_2>:
 178:	4b01      	ldr	r3, [pc, #4]	; (180 <handler_ext_int_2+0x8>)
 17a:	2204      	movs	r2, #4
 17c:	601a      	str	r2, [r3, #0]
 17e:	4770      	bx	lr
 180:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

00000184 <handler_ext_int_3>:
 184:	4b01      	ldr	r3, [pc, #4]	; (18c <handler_ext_int_3+0x8>)
 186:	2208      	movs	r2, #8
 188:	601a      	str	r2, [r3, #0]
 18a:	4770      	bx	lr
 18c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

00000190 <handler_ext_int_4>:
 190:	4b01      	ldr	r3, [pc, #4]	; (198 <handler_ext_int_4+0x8>)
 192:	2210      	movs	r2, #16
 194:	601a      	str	r2, [r3, #0]
 196:	4770      	bx	lr
 198:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

0000019c <handler_ext_int_5>:
 19c:	4b01      	ldr	r3, [pc, #4]	; (1a4 <handler_ext_int_5+0x8>)
 19e:	2220      	movs	r2, #32
 1a0:	601a      	str	r2, [r3, #0]
 1a2:	4770      	bx	lr
 1a4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

000001a8 <handler_ext_int_6>:
 1a8:	4b01      	ldr	r3, [pc, #4]	; (1b0 <handler_ext_int_6+0x8>)
 1aa:	2240      	movs	r2, #64	; 0x40
 1ac:	601a      	str	r2, [r3, #0]
 1ae:	4770      	bx	lr
 1b0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

000001b4 <handler_ext_int_7>:
 1b4:	4b01      	ldr	r3, [pc, #4]	; (1bc <handler_ext_int_7+0x8>)
 1b6:	2280      	movs	r2, #128	; 0x80
 1b8:	601a      	str	r2, [r3, #0]
 1ba:	4770      	bx	lr
 1bc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

000001c0 <handler_ext_int_8>:
 1c0:	4b02      	ldr	r3, [pc, #8]	; (1cc <handler_ext_int_8+0xc>)
 1c2:	2280      	movs	r2, #128	; 0x80
 1c4:	0052      	lsls	r2, r2, #1
 1c6:	601a      	str	r2, [r3, #0]
 1c8:	4770      	bx	lr
 1ca:	46c0      	nop			; (mov r8, r8)
 1cc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

000001d0 <handler_ext_int_9>:
 1d0:	4b02      	ldr	r3, [pc, #8]	; (1dc <handler_ext_int_9+0xc>)
 1d2:	2280      	movs	r2, #128	; 0x80
 1d4:	0092      	lsls	r2, r2, #2
 1d6:	601a      	str	r2, [r3, #0]
 1d8:	4770      	bx	lr
 1da:	46c0      	nop			; (mov r8, r8)
 1dc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

000001e0 <handler_ext_int_10>:
 1e0:	4b02      	ldr	r3, [pc, #8]	; (1ec <handler_ext_int_10+0xc>)
 1e2:	2280      	movs	r2, #128	; 0x80
 1e4:	00d2      	lsls	r2, r2, #3
 1e6:	601a      	str	r2, [r3, #0]
 1e8:	4770      	bx	lr
 1ea:	46c0      	nop			; (mov r8, r8)
 1ec:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

000001f0 <handler_ext_int_11>:
 1f0:	4b02      	ldr	r3, [pc, #8]	; (1fc <handler_ext_int_11+0xc>)
 1f2:	2280      	movs	r2, #128	; 0x80
 1f4:	0112      	lsls	r2, r2, #4
 1f6:	601a      	str	r2, [r3, #0]
 1f8:	4770      	bx	lr
 1fa:	46c0      	nop			; (mov r8, r8)
 1fc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

00000200 <handler_ext_int_12>:
 200:	4b02      	ldr	r3, [pc, #8]	; (20c <handler_ext_int_12+0xc>)
 202:	2280      	movs	r2, #128	; 0x80
 204:	0152      	lsls	r2, r2, #5
 206:	601a      	str	r2, [r3, #0]
 208:	4770      	bx	lr
 20a:	46c0      	nop			; (mov r8, r8)
 20c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

00000210 <handler_ext_int_13>:
 210:	4b02      	ldr	r3, [pc, #8]	; (21c <handler_ext_int_13+0xc>)
 212:	2280      	movs	r2, #128	; 0x80
 214:	0192      	lsls	r2, r2, #6
 216:	601a      	str	r2, [r3, #0]
 218:	4770      	bx	lr
 21a:	46c0      	nop			; (mov r8, r8)
 21c:	e000e280 	.word	0xe000e280

Disassembly of section .text.initialization:

00000220 <initialization>:
 220:	4b19      	ldr	r3, [pc, #100]	; (288 <initialization+0x68>)
 222:	4a1a      	ldr	r2, [pc, #104]	; (28c <initialization+0x6c>)
 224:	b510      	push	{r4, lr}
 226:	601a      	str	r2, [r3, #0]
 228:	4b19      	ldr	r3, [pc, #100]	; (290 <initialization+0x70>)
 22a:	2200      	movs	r2, #0
 22c:	4c19      	ldr	r4, [pc, #100]	; (294 <initialization+0x74>)
 22e:	601a      	str	r2, [r3, #0]
 230:	f7ff ff5c 	bl	ec <set_halt_disable>
 234:	2004      	movs	r0, #4
 236:	f7ff ff83 	bl	140 <mbus_enumerate>
 23a:	1c20      	adds	r0, r4, #0
 23c:	f7ff ff2b 	bl	96 <delay>
 240:	2005      	movs	r0, #5
 242:	f7ff ff7d 	bl	140 <mbus_enumerate>
 246:	1c20      	adds	r0, r4, #0
 248:	f7ff ff25 	bl	96 <delay>
 24c:	4912      	ldr	r1, [pc, #72]	; (298 <initialization+0x78>)
 24e:	2050      	movs	r0, #80	; 0x50
 250:	f7ff ff56 	bl	100 <mbus_write_message32>
 254:	1c20      	adds	r0, r4, #0
 256:	f7ff ff1e 	bl	96 <delay>
 25a:	4910      	ldr	r1, [pc, #64]	; (29c <initialization+0x7c>)
 25c:	2050      	movs	r0, #80	; 0x50
 25e:	f7ff ff4f 	bl	100 <mbus_write_message32>
 262:	1c20      	adds	r0, r4, #0
 264:	f7ff ff17 	bl	96 <delay>
 268:	490d      	ldr	r1, [pc, #52]	; (2a0 <initialization+0x80>)
 26a:	2050      	movs	r0, #80	; 0x50
 26c:	f7ff ff48 	bl	100 <mbus_write_message32>
 270:	1c20      	adds	r0, r4, #0
 272:	f7ff ff10 	bl	96 <delay>
 276:	490b      	ldr	r1, [pc, #44]	; (2a4 <initialization+0x84>)
 278:	2050      	movs	r0, #80	; 0x50
 27a:	f7ff ff41 	bl	100 <mbus_write_message32>
 27e:	1c20      	adds	r0, r4, #0
 280:	f7ff ff09 	bl	96 <delay>
 284:	bd10      	pop	{r4, pc}
 286:	46c0      	nop			; (mov r8, r8)
 288:	0000032c 	.word	0x0000032c
 28c:	deadbeef 	.word	0xdeadbeef
 290:	00000328 	.word	0x00000328
 294:	00002710 	.word	0x00002710
 298:	05000ebd 	.word	0x05000ebd
 29c:	1800e24a 	.word	0x1800e24a
 2a0:	1500e206 	.word	0x1500e206
 2a4:	36000001 	.word	0x36000001

Disassembly of section .text.startup.main:

000002a8 <main>:
 2a8:	b530      	push	{r4, r5, lr}
 2aa:	4a16      	ldr	r2, [pc, #88]	; (304 <main+0x5c>)
 2ac:	b085      	sub	sp, #20
 2ae:	4669      	mov	r1, sp
 2b0:	ca31      	ldmia	r2!, {r0, r4, r5}
 2b2:	c131      	stmia	r1!, {r0, r4, r5}
 2b4:	6812      	ldr	r2, [r2, #0]
 2b6:	600a      	str	r2, [r1, #0]
 2b8:	f7ff ff0c 	bl	d4 <disable_all_irq>
 2bc:	4b12      	ldr	r3, [pc, #72]	; (308 <main+0x60>)
 2be:	681a      	ldr	r2, [r3, #0]
 2c0:	4b12      	ldr	r3, [pc, #72]	; (30c <main+0x64>)
 2c2:	429a      	cmp	r2, r3
 2c4:	d001      	beq.n	2ca <main+0x22>
 2c6:	f7ff ffab 	bl	220 <initialization>
 2ca:	4c11      	ldr	r4, [pc, #68]	; (310 <main+0x68>)
 2cc:	20e1      	movs	r0, #225	; 0xe1
 2ce:	6821      	ldr	r1, [r4, #0]
 2d0:	f7ff ff16 	bl	100 <mbus_write_message32>
 2d4:	480f      	ldr	r0, [pc, #60]	; (314 <main+0x6c>)
 2d6:	f7ff fede 	bl	96 <delay>
 2da:	4669      	mov	r1, sp
 2dc:	2204      	movs	r2, #4
 2de:	20e2      	movs	r0, #226	; 0xe2
 2e0:	f7ff ff16 	bl	110 <mbus_write_message>
 2e4:	480b      	ldr	r0, [pc, #44]	; (314 <main+0x6c>)
 2e6:	f7ff fed6 	bl	96 <delay>
 2ea:	6823      	ldr	r3, [r4, #0]
 2ec:	2101      	movs	r1, #1
 2ee:	3301      	adds	r3, #1
 2f0:	2002      	movs	r0, #2
 2f2:	1c0a      	adds	r2, r1, #0
 2f4:	6023      	str	r3, [r4, #0]
 2f6:	f7ff fed7 	bl	a8 <set_wakeup_timer>
 2fa:	f7ff ff2b 	bl	154 <mbus_sleep_all>
 2fe:	46c0      	nop			; (mov r8, r8)
 300:	e7fd      	b.n	2fe <main+0x56>
 302:	46c0      	nop			; (mov r8, r8)
 304:	00000318 	.word	0x00000318
 308:	0000032c 	.word	0x0000032c
 30c:	deadbeef 	.word	0xdeadbeef
 310:	00000328 	.word	0x00000328
 314:	00002710 	.word	0x00002710
