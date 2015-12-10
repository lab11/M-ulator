
SRAM_test_0/SRAM_test_0.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0x90>:
   0:	00002000 	.word	0x00002000
   4:	000000a1 	.word	0x000000a1
	...
  1c:	00000090 	.word	0x00000090
  20:	00000090 	.word	0x00000090
  24:	00000090 	.word	0x00000090
  28:	00000090 	.word	0x00000090
	...
  34:	00000090 	.word	0x00000090
	...
  40:	00000175 	.word	0x00000175
  44:	00000181 	.word	0x00000181
  48:	0000018d 	.word	0x0000018d
  4c:	00000199 	.word	0x00000199
  50:	000001a5 	.word	0x000001a5
  54:	000001b1 	.word	0x000001b1
  58:	000001bd 	.word	0x000001bd
  5c:	000001c9 	.word	0x000001c9
  60:	000001d5 	.word	0x000001d5
  64:	000001e5 	.word	0x000001e5
  68:	000001f5 	.word	0x000001f5
  6c:	00000205 	.word	0x00000205
  70:	00000215 	.word	0x00000215
  74:	00000225 	.word	0x00000225
  78:	00000235 	.word	0x00000235
  7c:	00000245 	.word	0x00000245
  80:	00000255 	.word	0x00000255
	...

00000090 <hang>:
  90:	e7fe      	b.n	90 <hang>
	...

000000a0 <_start>:
  a0:	f000 f9d8 	bl	454 <main>
  a4:	e7fc      	b.n	a0 <_start>

Disassembly of section .text.mbus_write_message32:

000000a8 <mbus_write_message32>:
  a8:	4b02      	ldr	r3, [pc, #8]	; (b4 <mbus_write_message32+0xc>)
  aa:	0100      	lsls	r0, r0, #4
  ac:	4318      	orrs	r0, r3
  ae:	6001      	str	r1, [r0, #0]
  b0:	2001      	movs	r0, #1
  b2:	4770      	bx	lr
  b4:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_enumerate:

000000b8 <mbus_enumerate>:
  b8:	0603      	lsls	r3, r0, #24
  ba:	2080      	movs	r0, #128	; 0x80
  bc:	0580      	lsls	r0, r0, #22
  be:	4318      	orrs	r0, r3
  c0:	4b01      	ldr	r3, [pc, #4]	; (c8 <mbus_enumerate+0x10>)
  c2:	6018      	str	r0, [r3, #0]
  c4:	4770      	bx	lr
  c6:	46c0      	nop			; (mov r8, r8)
  c8:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

000000cc <mbus_sleep_all>:
  cc:	4b01      	ldr	r3, [pc, #4]	; (d4 <mbus_sleep_all+0x8>)
  ce:	2200      	movs	r2, #0
  d0:	601a      	str	r2, [r3, #0]
  d2:	4770      	bx	lr
  d4:	a0003010 	.word	0xa0003010

Disassembly of section .text.delay:

000000d8 <delay>:
  d8:	b500      	push	{lr}
  da:	2300      	movs	r3, #0
  dc:	e001      	b.n	e2 <delay+0xa>
  de:	46c0      	nop			; (mov r8, r8)
  e0:	3301      	adds	r3, #1
  e2:	4283      	cmp	r3, r0
  e4:	d1fb      	bne.n	de <delay+0x6>
  e6:	bd00      	pop	{pc}

Disassembly of section .text.write_regfile:

000000e8 <write_regfile>:
  e8:	0880      	lsrs	r0, r0, #2
  ea:	0209      	lsls	r1, r1, #8
  ec:	b508      	push	{r3, lr}
  ee:	0a09      	lsrs	r1, r1, #8
  f0:	4b05      	ldr	r3, [pc, #20]	; (108 <write_regfile+0x20>)
  f2:	0600      	lsls	r0, r0, #24
  f4:	4308      	orrs	r0, r1
  f6:	6018      	str	r0, [r3, #0]
  f8:	4b04      	ldr	r3, [pc, #16]	; (10c <write_regfile+0x24>)
  fa:	2210      	movs	r2, #16
  fc:	601a      	str	r2, [r3, #0]
  fe:	200a      	movs	r0, #10
 100:	f7ff ffea 	bl	d8 <delay>
 104:	bd08      	pop	{r3, pc}
 106:	46c0      	nop			; (mov r8, r8)
 108:	a0002000 	.word	0xa0002000
 10c:	a000200c 	.word	0xa000200c

Disassembly of section .text.set_wakeup_timer:

00000110 <set_wakeup_timer>:
 110:	b510      	push	{r4, lr}
 112:	1c14      	adds	r4, r2, #0
 114:	2900      	cmp	r1, #0
 116:	d003      	beq.n	120 <set_wakeup_timer+0x10>
 118:	2180      	movs	r1, #128	; 0x80
 11a:	0209      	lsls	r1, r1, #8
 11c:	4301      	orrs	r1, r0
 11e:	e001      	b.n	124 <set_wakeup_timer+0x14>
 120:	0440      	lsls	r0, r0, #17
 122:	0c41      	lsrs	r1, r0, #17
 124:	4804      	ldr	r0, [pc, #16]	; (138 <set_wakeup_timer+0x28>)
 126:	f7ff ffdf 	bl	e8 <write_regfile>
 12a:	2c00      	cmp	r4, #0
 12c:	d002      	beq.n	134 <set_wakeup_timer+0x24>
 12e:	4b03      	ldr	r3, [pc, #12]	; (13c <set_wakeup_timer+0x2c>)
 130:	2201      	movs	r2, #1
 132:	601a      	str	r2, [r3, #0]
 134:	bd10      	pop	{r4, pc}
 136:	46c0      	nop			; (mov r8, r8)
 138:	a0000034 	.word	0xa0000034
 13c:	a0001300 	.word	0xa0001300

Disassembly of section .text.set_halt_disable:

00000140 <set_halt_disable>:
 140:	b508      	push	{r3, lr}
 142:	4804      	ldr	r0, [pc, #16]	; (154 <set_halt_disable+0x14>)
 144:	21f0      	movs	r1, #240	; 0xf0
 146:	6803      	ldr	r3, [r0, #0]
 148:	0209      	lsls	r1, r1, #8
 14a:	4319      	orrs	r1, r3
 14c:	f7ff ffcc 	bl	e8 <write_regfile>
 150:	bd08      	pop	{r3, pc}
 152:	46c0      	nop			; (mov r8, r8)
 154:	a0000028 	.word	0xa0000028

Disassembly of section .text.init_interrupt:

00000158 <init_interrupt>:
 158:	4a03      	ldr	r2, [pc, #12]	; (168 <init_interrupt+0x10>)
 15a:	4b04      	ldr	r3, [pc, #16]	; (16c <init_interrupt+0x14>)
 15c:	601a      	str	r2, [r3, #0]
 15e:	4b04      	ldr	r3, [pc, #16]	; (170 <init_interrupt+0x18>)
 160:	2200      	movs	r2, #0
 162:	601a      	str	r2, [r3, #0]
 164:	4770      	bx	lr
 166:	46c0      	nop			; (mov r8, r8)
 168:	0001ffff 	.word	0x0001ffff
 16c:	e000e280 	.word	0xe000e280
 170:	e000e100 	.word	0xe000e100

Disassembly of section .text.handler_ext_int_0:

00000174 <handler_ext_int_0>:
 174:	4b01      	ldr	r3, [pc, #4]	; (17c <handler_ext_int_0+0x8>)
 176:	2201      	movs	r2, #1
 178:	601a      	str	r2, [r3, #0]
 17a:	4770      	bx	lr
 17c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

00000180 <handler_ext_int_1>:
 180:	4b01      	ldr	r3, [pc, #4]	; (188 <handler_ext_int_1+0x8>)
 182:	2202      	movs	r2, #2
 184:	601a      	str	r2, [r3, #0]
 186:	4770      	bx	lr
 188:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

0000018c <handler_ext_int_2>:
 18c:	4b01      	ldr	r3, [pc, #4]	; (194 <handler_ext_int_2+0x8>)
 18e:	2204      	movs	r2, #4
 190:	601a      	str	r2, [r3, #0]
 192:	4770      	bx	lr
 194:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

00000198 <handler_ext_int_3>:
 198:	4b01      	ldr	r3, [pc, #4]	; (1a0 <handler_ext_int_3+0x8>)
 19a:	2208      	movs	r2, #8
 19c:	601a      	str	r2, [r3, #0]
 19e:	4770      	bx	lr
 1a0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

000001a4 <handler_ext_int_4>:
 1a4:	4b01      	ldr	r3, [pc, #4]	; (1ac <handler_ext_int_4+0x8>)
 1a6:	2210      	movs	r2, #16
 1a8:	601a      	str	r2, [r3, #0]
 1aa:	4770      	bx	lr
 1ac:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

000001b0 <handler_ext_int_5>:
 1b0:	4b01      	ldr	r3, [pc, #4]	; (1b8 <handler_ext_int_5+0x8>)
 1b2:	2220      	movs	r2, #32
 1b4:	601a      	str	r2, [r3, #0]
 1b6:	4770      	bx	lr
 1b8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

000001bc <handler_ext_int_6>:
 1bc:	4b01      	ldr	r3, [pc, #4]	; (1c4 <handler_ext_int_6+0x8>)
 1be:	2240      	movs	r2, #64	; 0x40
 1c0:	601a      	str	r2, [r3, #0]
 1c2:	4770      	bx	lr
 1c4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

000001c8 <handler_ext_int_7>:
 1c8:	4b01      	ldr	r3, [pc, #4]	; (1d0 <handler_ext_int_7+0x8>)
 1ca:	2280      	movs	r2, #128	; 0x80
 1cc:	601a      	str	r2, [r3, #0]
 1ce:	4770      	bx	lr
 1d0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

000001d4 <handler_ext_int_8>:
 1d4:	4b02      	ldr	r3, [pc, #8]	; (1e0 <handler_ext_int_8+0xc>)
 1d6:	2280      	movs	r2, #128	; 0x80
 1d8:	0052      	lsls	r2, r2, #1
 1da:	601a      	str	r2, [r3, #0]
 1dc:	4770      	bx	lr
 1de:	46c0      	nop			; (mov r8, r8)
 1e0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

000001e4 <handler_ext_int_9>:
 1e4:	4b02      	ldr	r3, [pc, #8]	; (1f0 <handler_ext_int_9+0xc>)
 1e6:	2280      	movs	r2, #128	; 0x80
 1e8:	0092      	lsls	r2, r2, #2
 1ea:	601a      	str	r2, [r3, #0]
 1ec:	4770      	bx	lr
 1ee:	46c0      	nop			; (mov r8, r8)
 1f0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

000001f4 <handler_ext_int_10>:
 1f4:	4b02      	ldr	r3, [pc, #8]	; (200 <handler_ext_int_10+0xc>)
 1f6:	2280      	movs	r2, #128	; 0x80
 1f8:	00d2      	lsls	r2, r2, #3
 1fa:	601a      	str	r2, [r3, #0]
 1fc:	4770      	bx	lr
 1fe:	46c0      	nop			; (mov r8, r8)
 200:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

00000204 <handler_ext_int_11>:
 204:	4b02      	ldr	r3, [pc, #8]	; (210 <handler_ext_int_11+0xc>)
 206:	2280      	movs	r2, #128	; 0x80
 208:	0112      	lsls	r2, r2, #4
 20a:	601a      	str	r2, [r3, #0]
 20c:	4770      	bx	lr
 20e:	46c0      	nop			; (mov r8, r8)
 210:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

00000214 <handler_ext_int_12>:
 214:	4b02      	ldr	r3, [pc, #8]	; (220 <handler_ext_int_12+0xc>)
 216:	2280      	movs	r2, #128	; 0x80
 218:	0152      	lsls	r2, r2, #5
 21a:	601a      	str	r2, [r3, #0]
 21c:	4770      	bx	lr
 21e:	46c0      	nop			; (mov r8, r8)
 220:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

00000224 <handler_ext_int_13>:
 224:	4b02      	ldr	r3, [pc, #8]	; (230 <handler_ext_int_13+0xc>)
 226:	2280      	movs	r2, #128	; 0x80
 228:	0192      	lsls	r2, r2, #6
 22a:	601a      	str	r2, [r3, #0]
 22c:	4770      	bx	lr
 22e:	46c0      	nop			; (mov r8, r8)
 230:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

00000234 <handler_ext_int_14>:
 234:	4b02      	ldr	r3, [pc, #8]	; (240 <handler_ext_int_14+0xc>)
 236:	2280      	movs	r2, #128	; 0x80
 238:	01d2      	lsls	r2, r2, #7
 23a:	601a      	str	r2, [r3, #0]
 23c:	4770      	bx	lr
 23e:	46c0      	nop			; (mov r8, r8)
 240:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_15:

00000244 <handler_ext_int_15>:
 244:	4b02      	ldr	r3, [pc, #8]	; (250 <handler_ext_int_15+0xc>)
 246:	2280      	movs	r2, #128	; 0x80
 248:	0212      	lsls	r2, r2, #8
 24a:	601a      	str	r2, [r3, #0]
 24c:	4770      	bx	lr
 24e:	46c0      	nop			; (mov r8, r8)
 250:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_16:

00000254 <handler_ext_int_16>:
 254:	4b02      	ldr	r3, [pc, #8]	; (260 <handler_ext_int_16+0xc>)
 256:	2280      	movs	r2, #128	; 0x80
 258:	0252      	lsls	r2, r2, #9
 25a:	601a      	str	r2, [r3, #0]
 25c:	4770      	bx	lr
 25e:	46c0      	nop			; (mov r8, r8)
 260:	e000e280 	.word	0xe000e280

Disassembly of section .text.initialization:

00000264 <initialization>:
 264:	b508      	push	{r3, lr}
 266:	4a08      	ldr	r2, [pc, #32]	; (288 <initialization+0x24>)
 268:	4b08      	ldr	r3, [pc, #32]	; (28c <initialization+0x28>)
 26a:	601a      	str	r2, [r3, #0]
 26c:	4b08      	ldr	r3, [pc, #32]	; (290 <initialization+0x2c>)
 26e:	2200      	movs	r2, #0
 270:	601a      	str	r2, [r3, #0]
 272:	f7ff ff65 	bl	140 <set_halt_disable>
 276:	2004      	movs	r0, #4
 278:	f7ff ff1e 	bl	b8 <mbus_enumerate>
 27c:	20fa      	movs	r0, #250	; 0xfa
 27e:	0080      	lsls	r0, r0, #2
 280:	f7ff ff2a 	bl	d8 <delay>
 284:	bd08      	pop	{r3, pc}
 286:	46c0      	nop			; (mov r8, r8)
 288:	deadbeef 	.word	0xdeadbeef
 28c:	0000058c 	.word	0x0000058c
 290:	00000588 	.word	0x00000588

Disassembly of section .text.cycle0:

00000294 <cycle0>:
 294:	b508      	push	{r3, lr}
 296:	4b05      	ldr	r3, [pc, #20]	; (2ac <cycle0+0x18>)
 298:	20a0      	movs	r0, #160	; 0xa0
 29a:	6819      	ldr	r1, [r3, #0]
 29c:	f7ff ff04 	bl	a8 <mbus_write_message32>
 2a0:	20fa      	movs	r0, #250	; 0xfa
 2a2:	0080      	lsls	r0, r0, #2
 2a4:	f7ff ff18 	bl	d8 <delay>
 2a8:	bd08      	pop	{r3, pc}
 2aa:	46c0      	nop			; (mov r8, r8)
 2ac:	00000588 	.word	0x00000588

Disassembly of section .text.cycle1:

000002b0 <cycle1>:
 2b0:	b508      	push	{r3, lr}
 2b2:	4b05      	ldr	r3, [pc, #20]	; (2c8 <cycle1+0x18>)
 2b4:	20a1      	movs	r0, #161	; 0xa1
 2b6:	6819      	ldr	r1, [r3, #0]
 2b8:	f7ff fef6 	bl	a8 <mbus_write_message32>
 2bc:	20fa      	movs	r0, #250	; 0xfa
 2be:	0080      	lsls	r0, r0, #2
 2c0:	f7ff ff0a 	bl	d8 <delay>
 2c4:	bd08      	pop	{r3, pc}
 2c6:	46c0      	nop			; (mov r8, r8)
 2c8:	00000588 	.word	0x00000588

Disassembly of section .text.cycle2:

000002cc <cycle2>:
 2cc:	b508      	push	{r3, lr}
 2ce:	4b05      	ldr	r3, [pc, #20]	; (2e4 <cycle2+0x18>)
 2d0:	20a2      	movs	r0, #162	; 0xa2
 2d2:	6819      	ldr	r1, [r3, #0]
 2d4:	f7ff fee8 	bl	a8 <mbus_write_message32>
 2d8:	20fa      	movs	r0, #250	; 0xfa
 2da:	0080      	lsls	r0, r0, #2
 2dc:	f7ff fefc 	bl	d8 <delay>
 2e0:	bd08      	pop	{r3, pc}
 2e2:	46c0      	nop			; (mov r8, r8)
 2e4:	00000588 	.word	0x00000588

Disassembly of section .text.cycle3:

000002e8 <cycle3>:
 2e8:	b508      	push	{r3, lr}
 2ea:	4b05      	ldr	r3, [pc, #20]	; (300 <cycle3+0x18>)
 2ec:	20a3      	movs	r0, #163	; 0xa3
 2ee:	6819      	ldr	r1, [r3, #0]
 2f0:	f7ff feda 	bl	a8 <mbus_write_message32>
 2f4:	20fa      	movs	r0, #250	; 0xfa
 2f6:	0080      	lsls	r0, r0, #2
 2f8:	f7ff feee 	bl	d8 <delay>
 2fc:	bd08      	pop	{r3, pc}
 2fe:	46c0      	nop			; (mov r8, r8)
 300:	00000588 	.word	0x00000588

Disassembly of section .text.cycle4:

00000304 <cycle4>:
 304:	b508      	push	{r3, lr}
 306:	4b05      	ldr	r3, [pc, #20]	; (31c <cycle4+0x18>)
 308:	20a4      	movs	r0, #164	; 0xa4
 30a:	6819      	ldr	r1, [r3, #0]
 30c:	f7ff fecc 	bl	a8 <mbus_write_message32>
 310:	20fa      	movs	r0, #250	; 0xfa
 312:	0080      	lsls	r0, r0, #2
 314:	f7ff fee0 	bl	d8 <delay>
 318:	bd08      	pop	{r3, pc}
 31a:	46c0      	nop			; (mov r8, r8)
 31c:	00000588 	.word	0x00000588

Disassembly of section .text.cycle5:

00000320 <cycle5>:
 320:	b508      	push	{r3, lr}
 322:	4b05      	ldr	r3, [pc, #20]	; (338 <cycle5+0x18>)
 324:	20a5      	movs	r0, #165	; 0xa5
 326:	6819      	ldr	r1, [r3, #0]
 328:	f7ff febe 	bl	a8 <mbus_write_message32>
 32c:	20fa      	movs	r0, #250	; 0xfa
 32e:	0080      	lsls	r0, r0, #2
 330:	f7ff fed2 	bl	d8 <delay>
 334:	bd08      	pop	{r3, pc}
 336:	46c0      	nop			; (mov r8, r8)
 338:	00000588 	.word	0x00000588

Disassembly of section .text.cycle6:

0000033c <cycle6>:
 33c:	b508      	push	{r3, lr}
 33e:	4b05      	ldr	r3, [pc, #20]	; (354 <cycle6+0x18>)
 340:	20a6      	movs	r0, #166	; 0xa6
 342:	6819      	ldr	r1, [r3, #0]
 344:	f7ff feb0 	bl	a8 <mbus_write_message32>
 348:	20fa      	movs	r0, #250	; 0xfa
 34a:	0080      	lsls	r0, r0, #2
 34c:	f7ff fec4 	bl	d8 <delay>
 350:	bd08      	pop	{r3, pc}
 352:	46c0      	nop			; (mov r8, r8)
 354:	00000588 	.word	0x00000588

Disassembly of section .text.cycle7:

00000358 <cycle7>:
 358:	b508      	push	{r3, lr}
 35a:	4b05      	ldr	r3, [pc, #20]	; (370 <cycle7+0x18>)
 35c:	20a7      	movs	r0, #167	; 0xa7
 35e:	6819      	ldr	r1, [r3, #0]
 360:	f7ff fea2 	bl	a8 <mbus_write_message32>
 364:	20fa      	movs	r0, #250	; 0xfa
 366:	0080      	lsls	r0, r0, #2
 368:	f7ff feb6 	bl	d8 <delay>
 36c:	bd08      	pop	{r3, pc}
 36e:	46c0      	nop			; (mov r8, r8)
 370:	00000588 	.word	0x00000588

Disassembly of section .text.cycle8:

00000374 <cycle8>:
 374:	b508      	push	{r3, lr}
 376:	4b05      	ldr	r3, [pc, #20]	; (38c <cycle8+0x18>)
 378:	20a8      	movs	r0, #168	; 0xa8
 37a:	6819      	ldr	r1, [r3, #0]
 37c:	f7ff fe94 	bl	a8 <mbus_write_message32>
 380:	20fa      	movs	r0, #250	; 0xfa
 382:	0080      	lsls	r0, r0, #2
 384:	f7ff fea8 	bl	d8 <delay>
 388:	bd08      	pop	{r3, pc}
 38a:	46c0      	nop			; (mov r8, r8)
 38c:	00000588 	.word	0x00000588

Disassembly of section .text.cycle9:

00000390 <cycle9>:
 390:	b508      	push	{r3, lr}
 392:	4b05      	ldr	r3, [pc, #20]	; (3a8 <cycle9+0x18>)
 394:	20a9      	movs	r0, #169	; 0xa9
 396:	6819      	ldr	r1, [r3, #0]
 398:	f7ff fe86 	bl	a8 <mbus_write_message32>
 39c:	20fa      	movs	r0, #250	; 0xfa
 39e:	0080      	lsls	r0, r0, #2
 3a0:	f7ff fe9a 	bl	d8 <delay>
 3a4:	bd08      	pop	{r3, pc}
 3a6:	46c0      	nop			; (mov r8, r8)
 3a8:	00000588 	.word	0x00000588

Disassembly of section .text.cycle10:

000003ac <cycle10>:
 3ac:	b508      	push	{r3, lr}
 3ae:	4b05      	ldr	r3, [pc, #20]	; (3c4 <cycle10+0x18>)
 3b0:	20aa      	movs	r0, #170	; 0xaa
 3b2:	6819      	ldr	r1, [r3, #0]
 3b4:	f7ff fe78 	bl	a8 <mbus_write_message32>
 3b8:	20fa      	movs	r0, #250	; 0xfa
 3ba:	0080      	lsls	r0, r0, #2
 3bc:	f7ff fe8c 	bl	d8 <delay>
 3c0:	bd08      	pop	{r3, pc}
 3c2:	46c0      	nop			; (mov r8, r8)
 3c4:	00000588 	.word	0x00000588

Disassembly of section .text.cycle11:

000003c8 <cycle11>:
 3c8:	b508      	push	{r3, lr}
 3ca:	4b05      	ldr	r3, [pc, #20]	; (3e0 <cycle11+0x18>)
 3cc:	20ab      	movs	r0, #171	; 0xab
 3ce:	6819      	ldr	r1, [r3, #0]
 3d0:	f7ff fe6a 	bl	a8 <mbus_write_message32>
 3d4:	20fa      	movs	r0, #250	; 0xfa
 3d6:	0080      	lsls	r0, r0, #2
 3d8:	f7ff fe7e 	bl	d8 <delay>
 3dc:	bd08      	pop	{r3, pc}
 3de:	46c0      	nop			; (mov r8, r8)
 3e0:	00000588 	.word	0x00000588

Disassembly of section .text.cycle12:

000003e4 <cycle12>:
 3e4:	b508      	push	{r3, lr}
 3e6:	4b05      	ldr	r3, [pc, #20]	; (3fc <cycle12+0x18>)
 3e8:	20ac      	movs	r0, #172	; 0xac
 3ea:	6819      	ldr	r1, [r3, #0]
 3ec:	f7ff fe5c 	bl	a8 <mbus_write_message32>
 3f0:	20fa      	movs	r0, #250	; 0xfa
 3f2:	0080      	lsls	r0, r0, #2
 3f4:	f7ff fe70 	bl	d8 <delay>
 3f8:	bd08      	pop	{r3, pc}
 3fa:	46c0      	nop			; (mov r8, r8)
 3fc:	00000588 	.word	0x00000588

Disassembly of section .text.cycle13:

00000400 <cycle13>:
 400:	b508      	push	{r3, lr}
 402:	4b05      	ldr	r3, [pc, #20]	; (418 <cycle13+0x18>)
 404:	20ad      	movs	r0, #173	; 0xad
 406:	6819      	ldr	r1, [r3, #0]
 408:	f7ff fe4e 	bl	a8 <mbus_write_message32>
 40c:	20fa      	movs	r0, #250	; 0xfa
 40e:	0080      	lsls	r0, r0, #2
 410:	f7ff fe62 	bl	d8 <delay>
 414:	bd08      	pop	{r3, pc}
 416:	46c0      	nop			; (mov r8, r8)
 418:	00000588 	.word	0x00000588

Disassembly of section .text.cycle14:

0000041c <cycle14>:
 41c:	b508      	push	{r3, lr}
 41e:	4b05      	ldr	r3, [pc, #20]	; (434 <cycle14+0x18>)
 420:	20ae      	movs	r0, #174	; 0xae
 422:	6819      	ldr	r1, [r3, #0]
 424:	f7ff fe40 	bl	a8 <mbus_write_message32>
 428:	20fa      	movs	r0, #250	; 0xfa
 42a:	0080      	lsls	r0, r0, #2
 42c:	f7ff fe54 	bl	d8 <delay>
 430:	bd08      	pop	{r3, pc}
 432:	46c0      	nop			; (mov r8, r8)
 434:	00000588 	.word	0x00000588

Disassembly of section .text.cycle15:

00000438 <cycle15>:
 438:	b508      	push	{r3, lr}
 43a:	4b05      	ldr	r3, [pc, #20]	; (450 <cycle15+0x18>)
 43c:	20af      	movs	r0, #175	; 0xaf
 43e:	6819      	ldr	r1, [r3, #0]
 440:	f7ff fe32 	bl	a8 <mbus_write_message32>
 444:	20fa      	movs	r0, #250	; 0xfa
 446:	0080      	lsls	r0, r0, #2
 448:	f7ff fe46 	bl	d8 <delay>
 44c:	bd08      	pop	{r3, pc}
 44e:	46c0      	nop			; (mov r8, r8)
 450:	00000588 	.word	0x00000588

Disassembly of section .text.startup.main:

00000454 <main>:
 454:	b538      	push	{r3, r4, r5, lr}
 456:	f7ff fe7f 	bl	158 <init_interrupt>
 45a:	4b45      	ldr	r3, [pc, #276]	; (570 <main+0x11c>)
 45c:	681a      	ldr	r2, [r3, #0]
 45e:	4b45      	ldr	r3, [pc, #276]	; (574 <main+0x120>)
 460:	429a      	cmp	r2, r3
 462:	d001      	beq.n	468 <main+0x14>
 464:	f7ff fefe 	bl	264 <initialization>
 468:	2400      	movs	r4, #0
 46a:	4d43      	ldr	r5, [pc, #268]	; (578 <main+0x124>)
 46c:	682b      	ldr	r3, [r5, #0]
 46e:	2b00      	cmp	r3, #0
 470:	d104      	bne.n	47c <main+0x28>
 472:	20fa      	movs	r0, #250	; 0xfa
 474:	0080      	lsls	r0, r0, #2
 476:	f7ff fe2f 	bl	d8 <delay>
 47a:	e057      	b.n	52c <main+0xd8>
 47c:	230f      	movs	r3, #15
 47e:	4023      	ands	r3, r4
 480:	d102      	bne.n	488 <main+0x34>
 482:	f7ff ff07 	bl	294 <cycle0>
 486:	e047      	b.n	518 <main+0xc4>
 488:	2b01      	cmp	r3, #1
 48a:	d102      	bne.n	492 <main+0x3e>
 48c:	f7ff ff10 	bl	2b0 <cycle1>
 490:	e042      	b.n	518 <main+0xc4>
 492:	2b02      	cmp	r3, #2
 494:	d102      	bne.n	49c <main+0x48>
 496:	f7ff ff19 	bl	2cc <cycle2>
 49a:	e03d      	b.n	518 <main+0xc4>
 49c:	2b03      	cmp	r3, #3
 49e:	d102      	bne.n	4a6 <main+0x52>
 4a0:	f7ff ff22 	bl	2e8 <cycle3>
 4a4:	e038      	b.n	518 <main+0xc4>
 4a6:	2b04      	cmp	r3, #4
 4a8:	d102      	bne.n	4b0 <main+0x5c>
 4aa:	f7ff ff2b 	bl	304 <cycle4>
 4ae:	e033      	b.n	518 <main+0xc4>
 4b0:	2b05      	cmp	r3, #5
 4b2:	d102      	bne.n	4ba <main+0x66>
 4b4:	f7ff ff34 	bl	320 <cycle5>
 4b8:	e02e      	b.n	518 <main+0xc4>
 4ba:	2b06      	cmp	r3, #6
 4bc:	d102      	bne.n	4c4 <main+0x70>
 4be:	f7ff ff3d 	bl	33c <cycle6>
 4c2:	e029      	b.n	518 <main+0xc4>
 4c4:	2b07      	cmp	r3, #7
 4c6:	d102      	bne.n	4ce <main+0x7a>
 4c8:	f7ff ff46 	bl	358 <cycle7>
 4cc:	e024      	b.n	518 <main+0xc4>
 4ce:	2b08      	cmp	r3, #8
 4d0:	d102      	bne.n	4d8 <main+0x84>
 4d2:	f7ff ff4f 	bl	374 <cycle8>
 4d6:	e01f      	b.n	518 <main+0xc4>
 4d8:	2b09      	cmp	r3, #9
 4da:	d102      	bne.n	4e2 <main+0x8e>
 4dc:	f7ff ff58 	bl	390 <cycle9>
 4e0:	e01a      	b.n	518 <main+0xc4>
 4e2:	2b0a      	cmp	r3, #10
 4e4:	d102      	bne.n	4ec <main+0x98>
 4e6:	f7ff ff61 	bl	3ac <cycle10>
 4ea:	e015      	b.n	518 <main+0xc4>
 4ec:	2b0b      	cmp	r3, #11
 4ee:	d102      	bne.n	4f6 <main+0xa2>
 4f0:	f7ff ff6a 	bl	3c8 <cycle11>
 4f4:	e010      	b.n	518 <main+0xc4>
 4f6:	2b0c      	cmp	r3, #12
 4f8:	d102      	bne.n	500 <main+0xac>
 4fa:	f7ff ff73 	bl	3e4 <cycle12>
 4fe:	e00b      	b.n	518 <main+0xc4>
 500:	2b0d      	cmp	r3, #13
 502:	d102      	bne.n	50a <main+0xb6>
 504:	f7ff ff7c 	bl	400 <cycle13>
 508:	e006      	b.n	518 <main+0xc4>
 50a:	2b0e      	cmp	r3, #14
 50c:	d102      	bne.n	514 <main+0xc0>
 50e:	f7ff ff85 	bl	41c <cycle14>
 512:	e001      	b.n	518 <main+0xc4>
 514:	f7ff ff90 	bl	438 <cycle15>
 518:	682b      	ldr	r3, [r5, #0]
 51a:	20fa      	movs	r0, #250	; 0xfa
 51c:	3301      	adds	r3, #1
 51e:	0080      	lsls	r0, r0, #2
 520:	3401      	adds	r4, #1
 522:	602b      	str	r3, [r5, #0]
 524:	f7ff fdd8 	bl	d8 <delay>
 528:	2c64      	cmp	r4, #100	; 0x64
 52a:	d19e      	bne.n	46a <main+0x16>
 52c:	4b12      	ldr	r3, [pc, #72]	; (578 <main+0x124>)
 52e:	4a13      	ldr	r2, [pc, #76]	; (57c <main+0x128>)
 530:	6819      	ldr	r1, [r3, #0]
 532:	4291      	cmp	r1, r2
 534:	d108      	bne.n	548 <main+0xf4>
 536:	20dd      	movs	r0, #221	; 0xdd
 538:	4911      	ldr	r1, [pc, #68]	; (580 <main+0x12c>)
 53a:	f7ff fdb5 	bl	a8 <mbus_write_message32>
 53e:	20fa      	movs	r0, #250	; 0xfa
 540:	0080      	lsls	r0, r0, #2
 542:	f7ff fdc9 	bl	d8 <delay>
 546:	e7fe      	b.n	546 <main+0xf2>
 548:	681a      	ldr	r2, [r3, #0]
 54a:	490e      	ldr	r1, [pc, #56]	; (584 <main+0x130>)
 54c:	3201      	adds	r2, #1
 54e:	20dd      	movs	r0, #221	; 0xdd
 550:	601a      	str	r2, [r3, #0]
 552:	f7ff fda9 	bl	a8 <mbus_write_message32>
 556:	20fa      	movs	r0, #250	; 0xfa
 558:	0080      	lsls	r0, r0, #2
 55a:	f7ff fdbd 	bl	d8 <delay>
 55e:	2101      	movs	r1, #1
 560:	2002      	movs	r0, #2
 562:	1c0a      	adds	r2, r1, #0
 564:	f7ff fdd4 	bl	110 <set_wakeup_timer>
 568:	f7ff fdb0 	bl	cc <mbus_sleep_all>
 56c:	2001      	movs	r0, #1
 56e:	bd38      	pop	{r3, r4, r5, pc}
 570:	0000058c 	.word	0x0000058c
 574:	deadbeef 	.word	0xdeadbeef
 578:	00000588 	.word	0x00000588
 57c:	000003e7 	.word	0x000003e7
 580:	0ea70ea7 	.word	0x0ea70ea7
 584:	0ea7f00d 	.word	0x0ea7f00d
