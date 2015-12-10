
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
  a0:	f000 f9dc 	bl	45c <main>
  a4:	e7fc      	b.n	a0 <_start>

Disassembly of section .text.delay:

000000a6 <delay>:
  a6:	b500      	push	{lr}
  a8:	2300      	movs	r3, #0
  aa:	e001      	b.n	b0 <delay+0xa>
  ac:	46c0      	nop			; (mov r8, r8)
  ae:	3301      	adds	r3, #1
  b0:	4283      	cmp	r3, r0
  b2:	d1fb      	bne.n	ac <delay+0x6>
  b4:	bd00      	pop	{pc}

Disassembly of section .text.write_regfile:

000000b8 <write_regfile>:
  b8:	0880      	lsrs	r0, r0, #2
  ba:	0209      	lsls	r1, r1, #8
  bc:	b508      	push	{r3, lr}
  be:	0a09      	lsrs	r1, r1, #8
  c0:	4b05      	ldr	r3, [pc, #20]	; (d8 <write_regfile+0x20>)
  c2:	0600      	lsls	r0, r0, #24
  c4:	4308      	orrs	r0, r1
  c6:	6018      	str	r0, [r3, #0]
  c8:	4b04      	ldr	r3, [pc, #16]	; (dc <write_regfile+0x24>)
  ca:	2210      	movs	r2, #16
  cc:	601a      	str	r2, [r3, #0]
  ce:	200a      	movs	r0, #10
  d0:	f7ff ffe9 	bl	a6 <delay>
  d4:	bd08      	pop	{r3, pc}
  d6:	46c0      	nop			; (mov r8, r8)
  d8:	a0002000 	.word	0xa0002000
  dc:	a000200c 	.word	0xa000200c

Disassembly of section .text.set_wakeup_timer:

000000e0 <set_wakeup_timer>:
  e0:	b510      	push	{r4, lr}
  e2:	1c14      	adds	r4, r2, #0
  e4:	2900      	cmp	r1, #0
  e6:	d003      	beq.n	f0 <set_wakeup_timer+0x10>
  e8:	2180      	movs	r1, #128	; 0x80
  ea:	0209      	lsls	r1, r1, #8
  ec:	4301      	orrs	r1, r0
  ee:	e001      	b.n	f4 <set_wakeup_timer+0x14>
  f0:	0440      	lsls	r0, r0, #17
  f2:	0c41      	lsrs	r1, r0, #17
  f4:	4804      	ldr	r0, [pc, #16]	; (108 <set_wakeup_timer+0x28>)
  f6:	f7ff ffdf 	bl	b8 <write_regfile>
  fa:	2c00      	cmp	r4, #0
  fc:	d002      	beq.n	104 <set_wakeup_timer+0x24>
  fe:	4b03      	ldr	r3, [pc, #12]	; (10c <set_wakeup_timer+0x2c>)
 100:	2201      	movs	r2, #1
 102:	601a      	str	r2, [r3, #0]
 104:	bd10      	pop	{r4, pc}
 106:	46c0      	nop			; (mov r8, r8)
 108:	a0000034 	.word	0xa0000034
 10c:	a0001300 	.word	0xa0001300

Disassembly of section .text.set_halt_disable:

00000110 <set_halt_disable>:
 110:	b508      	push	{r3, lr}
 112:	4804      	ldr	r0, [pc, #16]	; (124 <set_halt_disable+0x14>)
 114:	21f0      	movs	r1, #240	; 0xf0
 116:	6803      	ldr	r3, [r0, #0]
 118:	0209      	lsls	r1, r1, #8
 11a:	4319      	orrs	r1, r3
 11c:	f7ff ffcc 	bl	b8 <write_regfile>
 120:	bd08      	pop	{r3, pc}
 122:	46c0      	nop			; (mov r8, r8)
 124:	a0000028 	.word	0xa0000028

Disassembly of section .text.mbus_write_message32:

00000128 <mbus_write_message32>:
 128:	4b02      	ldr	r3, [pc, #8]	; (134 <mbus_write_message32+0xc>)
 12a:	0100      	lsls	r0, r0, #4
 12c:	4318      	orrs	r0, r3
 12e:	6001      	str	r1, [r0, #0]
 130:	2001      	movs	r0, #1
 132:	4770      	bx	lr
 134:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_enumerate:

00000138 <mbus_enumerate>:
 138:	0603      	lsls	r3, r0, #24
 13a:	2080      	movs	r0, #128	; 0x80
 13c:	0580      	lsls	r0, r0, #22
 13e:	4318      	orrs	r0, r3
 140:	4b01      	ldr	r3, [pc, #4]	; (148 <mbus_enumerate+0x10>)
 142:	6018      	str	r0, [r3, #0]
 144:	4770      	bx	lr
 146:	46c0      	nop			; (mov r8, r8)
 148:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

0000014c <mbus_sleep_all>:
 14c:	4b01      	ldr	r3, [pc, #4]	; (154 <mbus_sleep_all+0x8>)
 14e:	2200      	movs	r2, #0
 150:	601a      	str	r2, [r3, #0]
 152:	4770      	bx	lr
 154:	a0003010 	.word	0xa0003010

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
 264:	4b0a      	ldr	r3, [pc, #40]	; (290 <initialization+0x2c>)
 266:	4a0b      	ldr	r2, [pc, #44]	; (294 <initialization+0x30>)
 268:	b510      	push	{r4, lr}
 26a:	601a      	str	r2, [r3, #0]
 26c:	24fa      	movs	r4, #250	; 0xfa
 26e:	4b0a      	ldr	r3, [pc, #40]	; (298 <initialization+0x34>)
 270:	2200      	movs	r2, #0
 272:	00a4      	lsls	r4, r4, #2
 274:	601a      	str	r2, [r3, #0]
 276:	f7ff ff4b 	bl	110 <set_halt_disable>
 27a:	1c20      	adds	r0, r4, #0
 27c:	f7ff ff13 	bl	a6 <delay>
 280:	2004      	movs	r0, #4
 282:	f7ff ff59 	bl	138 <mbus_enumerate>
 286:	1c20      	adds	r0, r4, #0
 288:	f7ff ff0d 	bl	a6 <delay>
 28c:	bd10      	pop	{r4, pc}
 28e:	46c0      	nop			; (mov r8, r8)
 290:	00000594 	.word	0x00000594
 294:	deadbeef 	.word	0xdeadbeef
 298:	00000590 	.word	0x00000590

Disassembly of section .text.cycle0:

0000029c <cycle0>:
 29c:	b508      	push	{r3, lr}
 29e:	4b05      	ldr	r3, [pc, #20]	; (2b4 <cycle0+0x18>)
 2a0:	20a0      	movs	r0, #160	; 0xa0
 2a2:	6819      	ldr	r1, [r3, #0]
 2a4:	f7ff ff40 	bl	128 <mbus_write_message32>
 2a8:	20fa      	movs	r0, #250	; 0xfa
 2aa:	0080      	lsls	r0, r0, #2
 2ac:	f7ff fefb 	bl	a6 <delay>
 2b0:	bd08      	pop	{r3, pc}
 2b2:	46c0      	nop			; (mov r8, r8)
 2b4:	00000590 	.word	0x00000590

Disassembly of section .text.cycle1:

000002b8 <cycle1>:
 2b8:	b508      	push	{r3, lr}
 2ba:	4b05      	ldr	r3, [pc, #20]	; (2d0 <cycle1+0x18>)
 2bc:	20a1      	movs	r0, #161	; 0xa1
 2be:	6819      	ldr	r1, [r3, #0]
 2c0:	f7ff ff32 	bl	128 <mbus_write_message32>
 2c4:	20fa      	movs	r0, #250	; 0xfa
 2c6:	0080      	lsls	r0, r0, #2
 2c8:	f7ff feed 	bl	a6 <delay>
 2cc:	bd08      	pop	{r3, pc}
 2ce:	46c0      	nop			; (mov r8, r8)
 2d0:	00000590 	.word	0x00000590

Disassembly of section .text.cycle2:

000002d4 <cycle2>:
 2d4:	b508      	push	{r3, lr}
 2d6:	4b05      	ldr	r3, [pc, #20]	; (2ec <cycle2+0x18>)
 2d8:	20a2      	movs	r0, #162	; 0xa2
 2da:	6819      	ldr	r1, [r3, #0]
 2dc:	f7ff ff24 	bl	128 <mbus_write_message32>
 2e0:	20fa      	movs	r0, #250	; 0xfa
 2e2:	0080      	lsls	r0, r0, #2
 2e4:	f7ff fedf 	bl	a6 <delay>
 2e8:	bd08      	pop	{r3, pc}
 2ea:	46c0      	nop			; (mov r8, r8)
 2ec:	00000590 	.word	0x00000590

Disassembly of section .text.cycle3:

000002f0 <cycle3>:
 2f0:	b508      	push	{r3, lr}
 2f2:	4b05      	ldr	r3, [pc, #20]	; (308 <cycle3+0x18>)
 2f4:	20a3      	movs	r0, #163	; 0xa3
 2f6:	6819      	ldr	r1, [r3, #0]
 2f8:	f7ff ff16 	bl	128 <mbus_write_message32>
 2fc:	20fa      	movs	r0, #250	; 0xfa
 2fe:	0080      	lsls	r0, r0, #2
 300:	f7ff fed1 	bl	a6 <delay>
 304:	bd08      	pop	{r3, pc}
 306:	46c0      	nop			; (mov r8, r8)
 308:	00000590 	.word	0x00000590

Disassembly of section .text.cycle4:

0000030c <cycle4>:
 30c:	b508      	push	{r3, lr}
 30e:	4b05      	ldr	r3, [pc, #20]	; (324 <cycle4+0x18>)
 310:	20a4      	movs	r0, #164	; 0xa4
 312:	6819      	ldr	r1, [r3, #0]
 314:	f7ff ff08 	bl	128 <mbus_write_message32>
 318:	20fa      	movs	r0, #250	; 0xfa
 31a:	0080      	lsls	r0, r0, #2
 31c:	f7ff fec3 	bl	a6 <delay>
 320:	bd08      	pop	{r3, pc}
 322:	46c0      	nop			; (mov r8, r8)
 324:	00000590 	.word	0x00000590

Disassembly of section .text.cycle5:

00000328 <cycle5>:
 328:	b508      	push	{r3, lr}
 32a:	4b05      	ldr	r3, [pc, #20]	; (340 <cycle5+0x18>)
 32c:	20a5      	movs	r0, #165	; 0xa5
 32e:	6819      	ldr	r1, [r3, #0]
 330:	f7ff fefa 	bl	128 <mbus_write_message32>
 334:	20fa      	movs	r0, #250	; 0xfa
 336:	0080      	lsls	r0, r0, #2
 338:	f7ff feb5 	bl	a6 <delay>
 33c:	bd08      	pop	{r3, pc}
 33e:	46c0      	nop			; (mov r8, r8)
 340:	00000590 	.word	0x00000590

Disassembly of section .text.cycle6:

00000344 <cycle6>:
 344:	b508      	push	{r3, lr}
 346:	4b05      	ldr	r3, [pc, #20]	; (35c <cycle6+0x18>)
 348:	20a6      	movs	r0, #166	; 0xa6
 34a:	6819      	ldr	r1, [r3, #0]
 34c:	f7ff feec 	bl	128 <mbus_write_message32>
 350:	20fa      	movs	r0, #250	; 0xfa
 352:	0080      	lsls	r0, r0, #2
 354:	f7ff fea7 	bl	a6 <delay>
 358:	bd08      	pop	{r3, pc}
 35a:	46c0      	nop			; (mov r8, r8)
 35c:	00000590 	.word	0x00000590

Disassembly of section .text.cycle7:

00000360 <cycle7>:
 360:	b508      	push	{r3, lr}
 362:	4b05      	ldr	r3, [pc, #20]	; (378 <cycle7+0x18>)
 364:	20a7      	movs	r0, #167	; 0xa7
 366:	6819      	ldr	r1, [r3, #0]
 368:	f7ff fede 	bl	128 <mbus_write_message32>
 36c:	20fa      	movs	r0, #250	; 0xfa
 36e:	0080      	lsls	r0, r0, #2
 370:	f7ff fe99 	bl	a6 <delay>
 374:	bd08      	pop	{r3, pc}
 376:	46c0      	nop			; (mov r8, r8)
 378:	00000590 	.word	0x00000590

Disassembly of section .text.cycle8:

0000037c <cycle8>:
 37c:	b508      	push	{r3, lr}
 37e:	4b05      	ldr	r3, [pc, #20]	; (394 <cycle8+0x18>)
 380:	20a8      	movs	r0, #168	; 0xa8
 382:	6819      	ldr	r1, [r3, #0]
 384:	f7ff fed0 	bl	128 <mbus_write_message32>
 388:	20fa      	movs	r0, #250	; 0xfa
 38a:	0080      	lsls	r0, r0, #2
 38c:	f7ff fe8b 	bl	a6 <delay>
 390:	bd08      	pop	{r3, pc}
 392:	46c0      	nop			; (mov r8, r8)
 394:	00000590 	.word	0x00000590

Disassembly of section .text.cycle9:

00000398 <cycle9>:
 398:	b508      	push	{r3, lr}
 39a:	4b05      	ldr	r3, [pc, #20]	; (3b0 <cycle9+0x18>)
 39c:	20a9      	movs	r0, #169	; 0xa9
 39e:	6819      	ldr	r1, [r3, #0]
 3a0:	f7ff fec2 	bl	128 <mbus_write_message32>
 3a4:	20fa      	movs	r0, #250	; 0xfa
 3a6:	0080      	lsls	r0, r0, #2
 3a8:	f7ff fe7d 	bl	a6 <delay>
 3ac:	bd08      	pop	{r3, pc}
 3ae:	46c0      	nop			; (mov r8, r8)
 3b0:	00000590 	.word	0x00000590

Disassembly of section .text.cycle10:

000003b4 <cycle10>:
 3b4:	b508      	push	{r3, lr}
 3b6:	4b05      	ldr	r3, [pc, #20]	; (3cc <cycle10+0x18>)
 3b8:	20aa      	movs	r0, #170	; 0xaa
 3ba:	6819      	ldr	r1, [r3, #0]
 3bc:	f7ff feb4 	bl	128 <mbus_write_message32>
 3c0:	20fa      	movs	r0, #250	; 0xfa
 3c2:	0080      	lsls	r0, r0, #2
 3c4:	f7ff fe6f 	bl	a6 <delay>
 3c8:	bd08      	pop	{r3, pc}
 3ca:	46c0      	nop			; (mov r8, r8)
 3cc:	00000590 	.word	0x00000590

Disassembly of section .text.cycle11:

000003d0 <cycle11>:
 3d0:	b508      	push	{r3, lr}
 3d2:	4b05      	ldr	r3, [pc, #20]	; (3e8 <cycle11+0x18>)
 3d4:	20ab      	movs	r0, #171	; 0xab
 3d6:	6819      	ldr	r1, [r3, #0]
 3d8:	f7ff fea6 	bl	128 <mbus_write_message32>
 3dc:	20fa      	movs	r0, #250	; 0xfa
 3de:	0080      	lsls	r0, r0, #2
 3e0:	f7ff fe61 	bl	a6 <delay>
 3e4:	bd08      	pop	{r3, pc}
 3e6:	46c0      	nop			; (mov r8, r8)
 3e8:	00000590 	.word	0x00000590

Disassembly of section .text.cycle12:

000003ec <cycle12>:
 3ec:	b508      	push	{r3, lr}
 3ee:	4b05      	ldr	r3, [pc, #20]	; (404 <cycle12+0x18>)
 3f0:	20ac      	movs	r0, #172	; 0xac
 3f2:	6819      	ldr	r1, [r3, #0]
 3f4:	f7ff fe98 	bl	128 <mbus_write_message32>
 3f8:	20fa      	movs	r0, #250	; 0xfa
 3fa:	0080      	lsls	r0, r0, #2
 3fc:	f7ff fe53 	bl	a6 <delay>
 400:	bd08      	pop	{r3, pc}
 402:	46c0      	nop			; (mov r8, r8)
 404:	00000590 	.word	0x00000590

Disassembly of section .text.cycle13:

00000408 <cycle13>:
 408:	b508      	push	{r3, lr}
 40a:	4b05      	ldr	r3, [pc, #20]	; (420 <cycle13+0x18>)
 40c:	20ad      	movs	r0, #173	; 0xad
 40e:	6819      	ldr	r1, [r3, #0]
 410:	f7ff fe8a 	bl	128 <mbus_write_message32>
 414:	20fa      	movs	r0, #250	; 0xfa
 416:	0080      	lsls	r0, r0, #2
 418:	f7ff fe45 	bl	a6 <delay>
 41c:	bd08      	pop	{r3, pc}
 41e:	46c0      	nop			; (mov r8, r8)
 420:	00000590 	.word	0x00000590

Disassembly of section .text.cycle14:

00000424 <cycle14>:
 424:	b508      	push	{r3, lr}
 426:	4b05      	ldr	r3, [pc, #20]	; (43c <cycle14+0x18>)
 428:	20ae      	movs	r0, #174	; 0xae
 42a:	6819      	ldr	r1, [r3, #0]
 42c:	f7ff fe7c 	bl	128 <mbus_write_message32>
 430:	20fa      	movs	r0, #250	; 0xfa
 432:	0080      	lsls	r0, r0, #2
 434:	f7ff fe37 	bl	a6 <delay>
 438:	bd08      	pop	{r3, pc}
 43a:	46c0      	nop			; (mov r8, r8)
 43c:	00000590 	.word	0x00000590

Disassembly of section .text.cycle15:

00000440 <cycle15>:
 440:	b508      	push	{r3, lr}
 442:	4b05      	ldr	r3, [pc, #20]	; (458 <cycle15+0x18>)
 444:	20af      	movs	r0, #175	; 0xaf
 446:	6819      	ldr	r1, [r3, #0]
 448:	f7ff fe6e 	bl	128 <mbus_write_message32>
 44c:	20fa      	movs	r0, #250	; 0xfa
 44e:	0080      	lsls	r0, r0, #2
 450:	f7ff fe29 	bl	a6 <delay>
 454:	bd08      	pop	{r3, pc}
 456:	46c0      	nop			; (mov r8, r8)
 458:	00000590 	.word	0x00000590

Disassembly of section .text.startup.main:

0000045c <main>:
 45c:	b538      	push	{r3, r4, r5, lr}
 45e:	f7ff fe7b 	bl	158 <init_interrupt>
 462:	4b45      	ldr	r3, [pc, #276]	; (578 <main+0x11c>)
 464:	681a      	ldr	r2, [r3, #0]
 466:	4b45      	ldr	r3, [pc, #276]	; (57c <main+0x120>)
 468:	429a      	cmp	r2, r3
 46a:	d001      	beq.n	470 <main+0x14>
 46c:	f7ff fefa 	bl	264 <initialization>
 470:	2400      	movs	r4, #0
 472:	4d43      	ldr	r5, [pc, #268]	; (580 <main+0x124>)
 474:	682b      	ldr	r3, [r5, #0]
 476:	2b00      	cmp	r3, #0
 478:	d104      	bne.n	484 <main+0x28>
 47a:	20fa      	movs	r0, #250	; 0xfa
 47c:	0080      	lsls	r0, r0, #2
 47e:	f7ff fe12 	bl	a6 <delay>
 482:	e057      	b.n	534 <main+0xd8>
 484:	230f      	movs	r3, #15
 486:	4023      	ands	r3, r4
 488:	d102      	bne.n	490 <main+0x34>
 48a:	f7ff ff07 	bl	29c <cycle0>
 48e:	e047      	b.n	520 <main+0xc4>
 490:	2b01      	cmp	r3, #1
 492:	d102      	bne.n	49a <main+0x3e>
 494:	f7ff ff10 	bl	2b8 <cycle1>
 498:	e042      	b.n	520 <main+0xc4>
 49a:	2b02      	cmp	r3, #2
 49c:	d102      	bne.n	4a4 <main+0x48>
 49e:	f7ff ff19 	bl	2d4 <cycle2>
 4a2:	e03d      	b.n	520 <main+0xc4>
 4a4:	2b03      	cmp	r3, #3
 4a6:	d102      	bne.n	4ae <main+0x52>
 4a8:	f7ff ff22 	bl	2f0 <cycle3>
 4ac:	e038      	b.n	520 <main+0xc4>
 4ae:	2b04      	cmp	r3, #4
 4b0:	d102      	bne.n	4b8 <main+0x5c>
 4b2:	f7ff ff2b 	bl	30c <cycle4>
 4b6:	e033      	b.n	520 <main+0xc4>
 4b8:	2b05      	cmp	r3, #5
 4ba:	d102      	bne.n	4c2 <main+0x66>
 4bc:	f7ff ff34 	bl	328 <cycle5>
 4c0:	e02e      	b.n	520 <main+0xc4>
 4c2:	2b06      	cmp	r3, #6
 4c4:	d102      	bne.n	4cc <main+0x70>
 4c6:	f7ff ff3d 	bl	344 <cycle6>
 4ca:	e029      	b.n	520 <main+0xc4>
 4cc:	2b07      	cmp	r3, #7
 4ce:	d102      	bne.n	4d6 <main+0x7a>
 4d0:	f7ff ff46 	bl	360 <cycle7>
 4d4:	e024      	b.n	520 <main+0xc4>
 4d6:	2b08      	cmp	r3, #8
 4d8:	d102      	bne.n	4e0 <main+0x84>
 4da:	f7ff ff4f 	bl	37c <cycle8>
 4de:	e01f      	b.n	520 <main+0xc4>
 4e0:	2b09      	cmp	r3, #9
 4e2:	d102      	bne.n	4ea <main+0x8e>
 4e4:	f7ff ff58 	bl	398 <cycle9>
 4e8:	e01a      	b.n	520 <main+0xc4>
 4ea:	2b0a      	cmp	r3, #10
 4ec:	d102      	bne.n	4f4 <main+0x98>
 4ee:	f7ff ff61 	bl	3b4 <cycle10>
 4f2:	e015      	b.n	520 <main+0xc4>
 4f4:	2b0b      	cmp	r3, #11
 4f6:	d102      	bne.n	4fe <main+0xa2>
 4f8:	f7ff ff6a 	bl	3d0 <cycle11>
 4fc:	e010      	b.n	520 <main+0xc4>
 4fe:	2b0c      	cmp	r3, #12
 500:	d102      	bne.n	508 <main+0xac>
 502:	f7ff ff73 	bl	3ec <cycle12>
 506:	e00b      	b.n	520 <main+0xc4>
 508:	2b0d      	cmp	r3, #13
 50a:	d102      	bne.n	512 <main+0xb6>
 50c:	f7ff ff7c 	bl	408 <cycle13>
 510:	e006      	b.n	520 <main+0xc4>
 512:	2b0e      	cmp	r3, #14
 514:	d102      	bne.n	51c <main+0xc0>
 516:	f7ff ff85 	bl	424 <cycle14>
 51a:	e001      	b.n	520 <main+0xc4>
 51c:	f7ff ff90 	bl	440 <cycle15>
 520:	682b      	ldr	r3, [r5, #0]
 522:	20fa      	movs	r0, #250	; 0xfa
 524:	3301      	adds	r3, #1
 526:	0080      	lsls	r0, r0, #2
 528:	3401      	adds	r4, #1
 52a:	602b      	str	r3, [r5, #0]
 52c:	f7ff fdbb 	bl	a6 <delay>
 530:	2c64      	cmp	r4, #100	; 0x64
 532:	d19e      	bne.n	472 <main+0x16>
 534:	4b12      	ldr	r3, [pc, #72]	; (580 <main+0x124>)
 536:	4a13      	ldr	r2, [pc, #76]	; (584 <main+0x128>)
 538:	6819      	ldr	r1, [r3, #0]
 53a:	4291      	cmp	r1, r2
 53c:	d108      	bne.n	550 <main+0xf4>
 53e:	20dd      	movs	r0, #221	; 0xdd
 540:	4911      	ldr	r1, [pc, #68]	; (588 <main+0x12c>)
 542:	f7ff fdf1 	bl	128 <mbus_write_message32>
 546:	20fa      	movs	r0, #250	; 0xfa
 548:	0080      	lsls	r0, r0, #2
 54a:	f7ff fdac 	bl	a6 <delay>
 54e:	e7fe      	b.n	54e <main+0xf2>
 550:	681a      	ldr	r2, [r3, #0]
 552:	490e      	ldr	r1, [pc, #56]	; (58c <main+0x130>)
 554:	3201      	adds	r2, #1
 556:	20dd      	movs	r0, #221	; 0xdd
 558:	601a      	str	r2, [r3, #0]
 55a:	f7ff fde5 	bl	128 <mbus_write_message32>
 55e:	20fa      	movs	r0, #250	; 0xfa
 560:	0080      	lsls	r0, r0, #2
 562:	f7ff fda0 	bl	a6 <delay>
 566:	2101      	movs	r1, #1
 568:	2002      	movs	r0, #2
 56a:	1c0a      	adds	r2, r1, #0
 56c:	f7ff fdb8 	bl	e0 <set_wakeup_timer>
 570:	f7ff fdec 	bl	14c <mbus_sleep_all>
 574:	2001      	movs	r0, #1
 576:	bd38      	pop	{r3, r4, r5, pc}
 578:	00000594 	.word	0x00000594
 57c:	deadbeef 	.word	0xdeadbeef
 580:	00000590 	.word	0x00000590
 584:	000003e7 	.word	0x000003e7
 588:	0ea70ea7 	.word	0x0ea70ea7
 58c:	0ea7f00d 	.word	0x0ea7f00d
