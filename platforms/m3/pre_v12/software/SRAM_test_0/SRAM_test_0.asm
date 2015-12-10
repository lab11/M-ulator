
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
  a0:	f000 f9b8 	bl	414 <main>
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
 28c:	00000540 	.word	0x00000540
 290:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle0:

00000294 <cycle0>:
 294:	b508      	push	{r3, lr}
 296:	4b04      	ldr	r3, [pc, #16]	; (2a8 <cycle0+0x14>)
 298:	20a0      	movs	r0, #160	; 0xa0
 29a:	6819      	ldr	r1, [r3, #0]
 29c:	f7ff ff04 	bl	a8 <mbus_write_message32>
 2a0:	2064      	movs	r0, #100	; 0x64
 2a2:	f7ff ff19 	bl	d8 <delay>
 2a6:	bd08      	pop	{r3, pc}
 2a8:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle1:

000002ac <cycle1>:
 2ac:	b508      	push	{r3, lr}
 2ae:	4b04      	ldr	r3, [pc, #16]	; (2c0 <cycle1+0x14>)
 2b0:	20a1      	movs	r0, #161	; 0xa1
 2b2:	6819      	ldr	r1, [r3, #0]
 2b4:	f7ff fef8 	bl	a8 <mbus_write_message32>
 2b8:	2064      	movs	r0, #100	; 0x64
 2ba:	f7ff ff0d 	bl	d8 <delay>
 2be:	bd08      	pop	{r3, pc}
 2c0:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle2:

000002c4 <cycle2>:
 2c4:	b508      	push	{r3, lr}
 2c6:	4b04      	ldr	r3, [pc, #16]	; (2d8 <cycle2+0x14>)
 2c8:	20a2      	movs	r0, #162	; 0xa2
 2ca:	6819      	ldr	r1, [r3, #0]
 2cc:	f7ff feec 	bl	a8 <mbus_write_message32>
 2d0:	2064      	movs	r0, #100	; 0x64
 2d2:	f7ff ff01 	bl	d8 <delay>
 2d6:	bd08      	pop	{r3, pc}
 2d8:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle3:

000002dc <cycle3>:
 2dc:	b508      	push	{r3, lr}
 2de:	4b04      	ldr	r3, [pc, #16]	; (2f0 <cycle3+0x14>)
 2e0:	20a3      	movs	r0, #163	; 0xa3
 2e2:	6819      	ldr	r1, [r3, #0]
 2e4:	f7ff fee0 	bl	a8 <mbus_write_message32>
 2e8:	2064      	movs	r0, #100	; 0x64
 2ea:	f7ff fef5 	bl	d8 <delay>
 2ee:	bd08      	pop	{r3, pc}
 2f0:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle4:

000002f4 <cycle4>:
 2f4:	b508      	push	{r3, lr}
 2f6:	4b04      	ldr	r3, [pc, #16]	; (308 <cycle4+0x14>)
 2f8:	20a4      	movs	r0, #164	; 0xa4
 2fa:	6819      	ldr	r1, [r3, #0]
 2fc:	f7ff fed4 	bl	a8 <mbus_write_message32>
 300:	2064      	movs	r0, #100	; 0x64
 302:	f7ff fee9 	bl	d8 <delay>
 306:	bd08      	pop	{r3, pc}
 308:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle5:

0000030c <cycle5>:
 30c:	b508      	push	{r3, lr}
 30e:	4b04      	ldr	r3, [pc, #16]	; (320 <cycle5+0x14>)
 310:	20a5      	movs	r0, #165	; 0xa5
 312:	6819      	ldr	r1, [r3, #0]
 314:	f7ff fec8 	bl	a8 <mbus_write_message32>
 318:	2064      	movs	r0, #100	; 0x64
 31a:	f7ff fedd 	bl	d8 <delay>
 31e:	bd08      	pop	{r3, pc}
 320:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle6:

00000324 <cycle6>:
 324:	b508      	push	{r3, lr}
 326:	4b04      	ldr	r3, [pc, #16]	; (338 <cycle6+0x14>)
 328:	20a6      	movs	r0, #166	; 0xa6
 32a:	6819      	ldr	r1, [r3, #0]
 32c:	f7ff febc 	bl	a8 <mbus_write_message32>
 330:	2064      	movs	r0, #100	; 0x64
 332:	f7ff fed1 	bl	d8 <delay>
 336:	bd08      	pop	{r3, pc}
 338:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle7:

0000033c <cycle7>:
 33c:	b508      	push	{r3, lr}
 33e:	4b04      	ldr	r3, [pc, #16]	; (350 <cycle7+0x14>)
 340:	20a7      	movs	r0, #167	; 0xa7
 342:	6819      	ldr	r1, [r3, #0]
 344:	f7ff feb0 	bl	a8 <mbus_write_message32>
 348:	2064      	movs	r0, #100	; 0x64
 34a:	f7ff fec5 	bl	d8 <delay>
 34e:	bd08      	pop	{r3, pc}
 350:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle8:

00000354 <cycle8>:
 354:	b508      	push	{r3, lr}
 356:	4b04      	ldr	r3, [pc, #16]	; (368 <cycle8+0x14>)
 358:	20a8      	movs	r0, #168	; 0xa8
 35a:	6819      	ldr	r1, [r3, #0]
 35c:	f7ff fea4 	bl	a8 <mbus_write_message32>
 360:	2064      	movs	r0, #100	; 0x64
 362:	f7ff feb9 	bl	d8 <delay>
 366:	bd08      	pop	{r3, pc}
 368:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle9:

0000036c <cycle9>:
 36c:	b508      	push	{r3, lr}
 36e:	4b04      	ldr	r3, [pc, #16]	; (380 <cycle9+0x14>)
 370:	20a9      	movs	r0, #169	; 0xa9
 372:	6819      	ldr	r1, [r3, #0]
 374:	f7ff fe98 	bl	a8 <mbus_write_message32>
 378:	2064      	movs	r0, #100	; 0x64
 37a:	f7ff fead 	bl	d8 <delay>
 37e:	bd08      	pop	{r3, pc}
 380:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle10:

00000384 <cycle10>:
 384:	b508      	push	{r3, lr}
 386:	4b04      	ldr	r3, [pc, #16]	; (398 <cycle10+0x14>)
 388:	20aa      	movs	r0, #170	; 0xaa
 38a:	6819      	ldr	r1, [r3, #0]
 38c:	f7ff fe8c 	bl	a8 <mbus_write_message32>
 390:	2064      	movs	r0, #100	; 0x64
 392:	f7ff fea1 	bl	d8 <delay>
 396:	bd08      	pop	{r3, pc}
 398:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle11:

0000039c <cycle11>:
 39c:	b508      	push	{r3, lr}
 39e:	4b04      	ldr	r3, [pc, #16]	; (3b0 <cycle11+0x14>)
 3a0:	20ab      	movs	r0, #171	; 0xab
 3a2:	6819      	ldr	r1, [r3, #0]
 3a4:	f7ff fe80 	bl	a8 <mbus_write_message32>
 3a8:	2064      	movs	r0, #100	; 0x64
 3aa:	f7ff fe95 	bl	d8 <delay>
 3ae:	bd08      	pop	{r3, pc}
 3b0:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle12:

000003b4 <cycle12>:
 3b4:	b508      	push	{r3, lr}
 3b6:	4b04      	ldr	r3, [pc, #16]	; (3c8 <cycle12+0x14>)
 3b8:	20ac      	movs	r0, #172	; 0xac
 3ba:	6819      	ldr	r1, [r3, #0]
 3bc:	f7ff fe74 	bl	a8 <mbus_write_message32>
 3c0:	2064      	movs	r0, #100	; 0x64
 3c2:	f7ff fe89 	bl	d8 <delay>
 3c6:	bd08      	pop	{r3, pc}
 3c8:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle13:

000003cc <cycle13>:
 3cc:	b508      	push	{r3, lr}
 3ce:	4b04      	ldr	r3, [pc, #16]	; (3e0 <cycle13+0x14>)
 3d0:	20ad      	movs	r0, #173	; 0xad
 3d2:	6819      	ldr	r1, [r3, #0]
 3d4:	f7ff fe68 	bl	a8 <mbus_write_message32>
 3d8:	2064      	movs	r0, #100	; 0x64
 3da:	f7ff fe7d 	bl	d8 <delay>
 3de:	bd08      	pop	{r3, pc}
 3e0:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle14:

000003e4 <cycle14>:
 3e4:	b508      	push	{r3, lr}
 3e6:	4b04      	ldr	r3, [pc, #16]	; (3f8 <cycle14+0x14>)
 3e8:	20ae      	movs	r0, #174	; 0xae
 3ea:	6819      	ldr	r1, [r3, #0]
 3ec:	f7ff fe5c 	bl	a8 <mbus_write_message32>
 3f0:	2064      	movs	r0, #100	; 0x64
 3f2:	f7ff fe71 	bl	d8 <delay>
 3f6:	bd08      	pop	{r3, pc}
 3f8:	0000053c 	.word	0x0000053c

Disassembly of section .text.cycle15:

000003fc <cycle15>:
 3fc:	b508      	push	{r3, lr}
 3fe:	4b04      	ldr	r3, [pc, #16]	; (410 <cycle15+0x14>)
 400:	20af      	movs	r0, #175	; 0xaf
 402:	6819      	ldr	r1, [r3, #0]
 404:	f7ff fe50 	bl	a8 <mbus_write_message32>
 408:	2064      	movs	r0, #100	; 0x64
 40a:	f7ff fe65 	bl	d8 <delay>
 40e:	bd08      	pop	{r3, pc}
 410:	0000053c 	.word	0x0000053c

Disassembly of section .text.startup.main:

00000414 <main>:
 414:	b538      	push	{r3, r4, r5, lr}
 416:	f7ff fe9f 	bl	158 <init_interrupt>
 41a:	4b43      	ldr	r3, [pc, #268]	; (528 <main+0x114>)
 41c:	681a      	ldr	r2, [r3, #0]
 41e:	4b43      	ldr	r3, [pc, #268]	; (52c <main+0x118>)
 420:	429a      	cmp	r2, r3
 422:	d001      	beq.n	428 <main+0x14>
 424:	f7ff ff1e 	bl	264 <initialization>
 428:	2400      	movs	r4, #0
 42a:	4d41      	ldr	r5, [pc, #260]	; (530 <main+0x11c>)
 42c:	682b      	ldr	r3, [r5, #0]
 42e:	2b00      	cmp	r3, #0
 430:	d105      	bne.n	43e <main+0x2a>
 432:	4b3f      	ldr	r3, [pc, #252]	; (530 <main+0x11c>)
 434:	4a3f      	ldr	r2, [pc, #252]	; (534 <main+0x120>)
 436:	6819      	ldr	r1, [r3, #0]
 438:	4291      	cmp	r1, r2
 43a:	d162      	bne.n	502 <main+0xee>
 43c:	e058      	b.n	4f0 <main+0xdc>
 43e:	230f      	movs	r3, #15
 440:	4023      	ands	r3, r4
 442:	d102      	bne.n	44a <main+0x36>
 444:	f7ff ff26 	bl	294 <cycle0>
 448:	e047      	b.n	4da <main+0xc6>
 44a:	2b01      	cmp	r3, #1
 44c:	d102      	bne.n	454 <main+0x40>
 44e:	f7ff ff2d 	bl	2ac <cycle1>
 452:	e042      	b.n	4da <main+0xc6>
 454:	2b02      	cmp	r3, #2
 456:	d102      	bne.n	45e <main+0x4a>
 458:	f7ff ff34 	bl	2c4 <cycle2>
 45c:	e03d      	b.n	4da <main+0xc6>
 45e:	2b03      	cmp	r3, #3
 460:	d102      	bne.n	468 <main+0x54>
 462:	f7ff ff3b 	bl	2dc <cycle3>
 466:	e038      	b.n	4da <main+0xc6>
 468:	2b04      	cmp	r3, #4
 46a:	d102      	bne.n	472 <main+0x5e>
 46c:	f7ff ff42 	bl	2f4 <cycle4>
 470:	e033      	b.n	4da <main+0xc6>
 472:	2b05      	cmp	r3, #5
 474:	d102      	bne.n	47c <main+0x68>
 476:	f7ff ff49 	bl	30c <cycle5>
 47a:	e02e      	b.n	4da <main+0xc6>
 47c:	2b06      	cmp	r3, #6
 47e:	d102      	bne.n	486 <main+0x72>
 480:	f7ff ff50 	bl	324 <cycle6>
 484:	e029      	b.n	4da <main+0xc6>
 486:	2b07      	cmp	r3, #7
 488:	d102      	bne.n	490 <main+0x7c>
 48a:	f7ff ff57 	bl	33c <cycle7>
 48e:	e024      	b.n	4da <main+0xc6>
 490:	2b08      	cmp	r3, #8
 492:	d102      	bne.n	49a <main+0x86>
 494:	f7ff ff5e 	bl	354 <cycle8>
 498:	e01f      	b.n	4da <main+0xc6>
 49a:	2b09      	cmp	r3, #9
 49c:	d102      	bne.n	4a4 <main+0x90>
 49e:	f7ff ff65 	bl	36c <cycle9>
 4a2:	e01a      	b.n	4da <main+0xc6>
 4a4:	2b0a      	cmp	r3, #10
 4a6:	d102      	bne.n	4ae <main+0x9a>
 4a8:	f7ff ff6c 	bl	384 <cycle10>
 4ac:	e015      	b.n	4da <main+0xc6>
 4ae:	2b0b      	cmp	r3, #11
 4b0:	d102      	bne.n	4b8 <main+0xa4>
 4b2:	f7ff ff73 	bl	39c <cycle11>
 4b6:	e010      	b.n	4da <main+0xc6>
 4b8:	2b0c      	cmp	r3, #12
 4ba:	d102      	bne.n	4c2 <main+0xae>
 4bc:	f7ff ff7a 	bl	3b4 <cycle12>
 4c0:	e00b      	b.n	4da <main+0xc6>
 4c2:	2b0d      	cmp	r3, #13
 4c4:	d102      	bne.n	4cc <main+0xb8>
 4c6:	f7ff ff81 	bl	3cc <cycle13>
 4ca:	e006      	b.n	4da <main+0xc6>
 4cc:	2b0e      	cmp	r3, #14
 4ce:	d102      	bne.n	4d6 <main+0xc2>
 4d0:	f7ff ff88 	bl	3e4 <cycle14>
 4d4:	e001      	b.n	4da <main+0xc6>
 4d6:	f7ff ff91 	bl	3fc <cycle15>
 4da:	682b      	ldr	r3, [r5, #0]
 4dc:	20fa      	movs	r0, #250	; 0xfa
 4de:	3301      	adds	r3, #1
 4e0:	0080      	lsls	r0, r0, #2
 4e2:	3401      	adds	r4, #1
 4e4:	602b      	str	r3, [r5, #0]
 4e6:	f7ff fdf7 	bl	d8 <delay>
 4ea:	2c64      	cmp	r4, #100	; 0x64
 4ec:	d19d      	bne.n	42a <main+0x16>
 4ee:	e7a0      	b.n	432 <main+0x1e>
 4f0:	20dd      	movs	r0, #221	; 0xdd
 4f2:	4911      	ldr	r1, [pc, #68]	; (538 <main+0x124>)
 4f4:	f7ff fdd8 	bl	a8 <mbus_write_message32>
 4f8:	20fa      	movs	r0, #250	; 0xfa
 4fa:	0080      	lsls	r0, r0, #2
 4fc:	f7ff fdec 	bl	d8 <delay>
 500:	e7fe      	b.n	500 <main+0xec>
 502:	681a      	ldr	r2, [r3, #0]
 504:	490c      	ldr	r1, [pc, #48]	; (538 <main+0x124>)
 506:	3201      	adds	r2, #1
 508:	20dd      	movs	r0, #221	; 0xdd
 50a:	601a      	str	r2, [r3, #0]
 50c:	f7ff fdcc 	bl	a8 <mbus_write_message32>
 510:	2064      	movs	r0, #100	; 0x64
 512:	f7ff fde1 	bl	d8 <delay>
 516:	2101      	movs	r1, #1
 518:	2002      	movs	r0, #2
 51a:	1c0a      	adds	r2, r1, #0
 51c:	f7ff fdf8 	bl	110 <set_wakeup_timer>
 520:	f7ff fdd4 	bl	cc <mbus_sleep_all>
 524:	2001      	movs	r0, #1
 526:	bd38      	pop	{r3, r4, r5, pc}
 528:	00000540 	.word	0x00000540
 52c:	deadbeef 	.word	0xdeadbeef
 530:	0000053c 	.word	0x0000053c
 534:	000003e7 	.word	0x000003e7
 538:	0ea7f00d 	.word	0x0ea7f00d
