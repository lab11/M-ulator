
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
  a0:	f000 f998 	bl	3d4 <main>
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
 264:	b508      	push	{r3, lr}
 266:	4a07      	ldr	r2, [pc, #28]	; (284 <initialization+0x20>)
 268:	4b07      	ldr	r3, [pc, #28]	; (288 <initialization+0x24>)
 26a:	601a      	str	r2, [r3, #0]
 26c:	4b07      	ldr	r3, [pc, #28]	; (28c <initialization+0x28>)
 26e:	2200      	movs	r2, #0
 270:	601a      	str	r2, [r3, #0]
 272:	f7ff ff4d 	bl	110 <set_halt_disable>
 276:	2004      	movs	r0, #4
 278:	f7ff ff5e 	bl	138 <mbus_enumerate>
 27c:	4804      	ldr	r0, [pc, #16]	; (290 <initialization+0x2c>)
 27e:	f7ff ff12 	bl	a6 <delay>
 282:	bd08      	pop	{r3, pc}
 284:	deadbeef 	.word	0xdeadbeef
 288:	00000504 	.word	0x00000504
 28c:	00000500 	.word	0x00000500
 290:	00001388 	.word	0x00001388

Disassembly of section .text.cycle0:

00000294 <cycle0>:
 294:	b508      	push	{r3, lr}
 296:	4b03      	ldr	r3, [pc, #12]	; (2a4 <cycle0+0x10>)
 298:	20a0      	movs	r0, #160	; 0xa0
 29a:	6819      	ldr	r1, [r3, #0]
 29c:	f7ff ff44 	bl	128 <mbus_write_message32>
 2a0:	bd08      	pop	{r3, pc}
 2a2:	46c0      	nop			; (mov r8, r8)
 2a4:	00000500 	.word	0x00000500

Disassembly of section .text.cycle1:

000002a8 <cycle1>:
 2a8:	b508      	push	{r3, lr}
 2aa:	4b03      	ldr	r3, [pc, #12]	; (2b8 <cycle1+0x10>)
 2ac:	20a1      	movs	r0, #161	; 0xa1
 2ae:	6819      	ldr	r1, [r3, #0]
 2b0:	f7ff ff3a 	bl	128 <mbus_write_message32>
 2b4:	bd08      	pop	{r3, pc}
 2b6:	46c0      	nop			; (mov r8, r8)
 2b8:	00000500 	.word	0x00000500

Disassembly of section .text.cycle2:

000002bc <cycle2>:
 2bc:	b508      	push	{r3, lr}
 2be:	4b03      	ldr	r3, [pc, #12]	; (2cc <cycle2+0x10>)
 2c0:	20a2      	movs	r0, #162	; 0xa2
 2c2:	6819      	ldr	r1, [r3, #0]
 2c4:	f7ff ff30 	bl	128 <mbus_write_message32>
 2c8:	bd08      	pop	{r3, pc}
 2ca:	46c0      	nop			; (mov r8, r8)
 2cc:	00000500 	.word	0x00000500

Disassembly of section .text.cycle3:

000002d0 <cycle3>:
 2d0:	b508      	push	{r3, lr}
 2d2:	4b03      	ldr	r3, [pc, #12]	; (2e0 <cycle3+0x10>)
 2d4:	20a3      	movs	r0, #163	; 0xa3
 2d6:	6819      	ldr	r1, [r3, #0]
 2d8:	f7ff ff26 	bl	128 <mbus_write_message32>
 2dc:	bd08      	pop	{r3, pc}
 2de:	46c0      	nop			; (mov r8, r8)
 2e0:	00000500 	.word	0x00000500

Disassembly of section .text.cycle4:

000002e4 <cycle4>:
 2e4:	b508      	push	{r3, lr}
 2e6:	4b03      	ldr	r3, [pc, #12]	; (2f4 <cycle4+0x10>)
 2e8:	20a4      	movs	r0, #164	; 0xa4
 2ea:	6819      	ldr	r1, [r3, #0]
 2ec:	f7ff ff1c 	bl	128 <mbus_write_message32>
 2f0:	bd08      	pop	{r3, pc}
 2f2:	46c0      	nop			; (mov r8, r8)
 2f4:	00000500 	.word	0x00000500

Disassembly of section .text.cycle5:

000002f8 <cycle5>:
 2f8:	b508      	push	{r3, lr}
 2fa:	4b03      	ldr	r3, [pc, #12]	; (308 <cycle5+0x10>)
 2fc:	20a5      	movs	r0, #165	; 0xa5
 2fe:	6819      	ldr	r1, [r3, #0]
 300:	f7ff ff12 	bl	128 <mbus_write_message32>
 304:	bd08      	pop	{r3, pc}
 306:	46c0      	nop			; (mov r8, r8)
 308:	00000500 	.word	0x00000500

Disassembly of section .text.cycle6:

0000030c <cycle6>:
 30c:	b508      	push	{r3, lr}
 30e:	4b03      	ldr	r3, [pc, #12]	; (31c <cycle6+0x10>)
 310:	20a6      	movs	r0, #166	; 0xa6
 312:	6819      	ldr	r1, [r3, #0]
 314:	f7ff ff08 	bl	128 <mbus_write_message32>
 318:	bd08      	pop	{r3, pc}
 31a:	46c0      	nop			; (mov r8, r8)
 31c:	00000500 	.word	0x00000500

Disassembly of section .text.cycle7:

00000320 <cycle7>:
 320:	b508      	push	{r3, lr}
 322:	4b03      	ldr	r3, [pc, #12]	; (330 <cycle7+0x10>)
 324:	20a7      	movs	r0, #167	; 0xa7
 326:	6819      	ldr	r1, [r3, #0]
 328:	f7ff fefe 	bl	128 <mbus_write_message32>
 32c:	bd08      	pop	{r3, pc}
 32e:	46c0      	nop			; (mov r8, r8)
 330:	00000500 	.word	0x00000500

Disassembly of section .text.cycle8:

00000334 <cycle8>:
 334:	b508      	push	{r3, lr}
 336:	4b03      	ldr	r3, [pc, #12]	; (344 <cycle8+0x10>)
 338:	20a8      	movs	r0, #168	; 0xa8
 33a:	6819      	ldr	r1, [r3, #0]
 33c:	f7ff fef4 	bl	128 <mbus_write_message32>
 340:	bd08      	pop	{r3, pc}
 342:	46c0      	nop			; (mov r8, r8)
 344:	00000500 	.word	0x00000500

Disassembly of section .text.cycle9:

00000348 <cycle9>:
 348:	b508      	push	{r3, lr}
 34a:	4b03      	ldr	r3, [pc, #12]	; (358 <cycle9+0x10>)
 34c:	20a9      	movs	r0, #169	; 0xa9
 34e:	6819      	ldr	r1, [r3, #0]
 350:	f7ff feea 	bl	128 <mbus_write_message32>
 354:	bd08      	pop	{r3, pc}
 356:	46c0      	nop			; (mov r8, r8)
 358:	00000500 	.word	0x00000500

Disassembly of section .text.cycle10:

0000035c <cycle10>:
 35c:	b508      	push	{r3, lr}
 35e:	4b03      	ldr	r3, [pc, #12]	; (36c <cycle10+0x10>)
 360:	20aa      	movs	r0, #170	; 0xaa
 362:	6819      	ldr	r1, [r3, #0]
 364:	f7ff fee0 	bl	128 <mbus_write_message32>
 368:	bd08      	pop	{r3, pc}
 36a:	46c0      	nop			; (mov r8, r8)
 36c:	00000500 	.word	0x00000500

Disassembly of section .text.cycle11:

00000370 <cycle11>:
 370:	b508      	push	{r3, lr}
 372:	4b03      	ldr	r3, [pc, #12]	; (380 <cycle11+0x10>)
 374:	20ab      	movs	r0, #171	; 0xab
 376:	6819      	ldr	r1, [r3, #0]
 378:	f7ff fed6 	bl	128 <mbus_write_message32>
 37c:	bd08      	pop	{r3, pc}
 37e:	46c0      	nop			; (mov r8, r8)
 380:	00000500 	.word	0x00000500

Disassembly of section .text.cycle12:

00000384 <cycle12>:
 384:	b508      	push	{r3, lr}
 386:	4b03      	ldr	r3, [pc, #12]	; (394 <cycle12+0x10>)
 388:	20ac      	movs	r0, #172	; 0xac
 38a:	6819      	ldr	r1, [r3, #0]
 38c:	f7ff fecc 	bl	128 <mbus_write_message32>
 390:	bd08      	pop	{r3, pc}
 392:	46c0      	nop			; (mov r8, r8)
 394:	00000500 	.word	0x00000500

Disassembly of section .text.cycle13:

00000398 <cycle13>:
 398:	b508      	push	{r3, lr}
 39a:	4b03      	ldr	r3, [pc, #12]	; (3a8 <cycle13+0x10>)
 39c:	20ad      	movs	r0, #173	; 0xad
 39e:	6819      	ldr	r1, [r3, #0]
 3a0:	f7ff fec2 	bl	128 <mbus_write_message32>
 3a4:	bd08      	pop	{r3, pc}
 3a6:	46c0      	nop			; (mov r8, r8)
 3a8:	00000500 	.word	0x00000500

Disassembly of section .text.cycle14:

000003ac <cycle14>:
 3ac:	b508      	push	{r3, lr}
 3ae:	4b03      	ldr	r3, [pc, #12]	; (3bc <cycle14+0x10>)
 3b0:	20ae      	movs	r0, #174	; 0xae
 3b2:	6819      	ldr	r1, [r3, #0]
 3b4:	f7ff feb8 	bl	128 <mbus_write_message32>
 3b8:	bd08      	pop	{r3, pc}
 3ba:	46c0      	nop			; (mov r8, r8)
 3bc:	00000500 	.word	0x00000500

Disassembly of section .text.cycle15:

000003c0 <cycle15>:
 3c0:	b508      	push	{r3, lr}
 3c2:	4b03      	ldr	r3, [pc, #12]	; (3d0 <cycle15+0x10>)
 3c4:	20af      	movs	r0, #175	; 0xaf
 3c6:	6819      	ldr	r1, [r3, #0]
 3c8:	f7ff feae 	bl	128 <mbus_write_message32>
 3cc:	bd08      	pop	{r3, pc}
 3ce:	46c0      	nop			; (mov r8, r8)
 3d0:	00000500 	.word	0x00000500

Disassembly of section .text.startup.main:

000003d4 <main>:
 3d4:	b538      	push	{r3, r4, r5, lr}
 3d6:	f7ff febf 	bl	158 <init_interrupt>
 3da:	4b43      	ldr	r3, [pc, #268]	; (4e8 <main+0x114>)
 3dc:	681a      	ldr	r2, [r3, #0]
 3de:	4b43      	ldr	r3, [pc, #268]	; (4ec <main+0x118>)
 3e0:	429a      	cmp	r2, r3
 3e2:	d001      	beq.n	3e8 <main+0x14>
 3e4:	f7ff ff3e 	bl	264 <initialization>
 3e8:	2400      	movs	r4, #0
 3ea:	4d41      	ldr	r5, [pc, #260]	; (4f0 <main+0x11c>)
 3ec:	682b      	ldr	r3, [r5, #0]
 3ee:	2b00      	cmp	r3, #0
 3f0:	d105      	bne.n	3fe <main+0x2a>
 3f2:	4b3f      	ldr	r3, [pc, #252]	; (4f0 <main+0x11c>)
 3f4:	4a3f      	ldr	r2, [pc, #252]	; (4f4 <main+0x120>)
 3f6:	6819      	ldr	r1, [r3, #0]
 3f8:	4291      	cmp	r1, r2
 3fa:	d161      	bne.n	4c0 <main+0xec>
 3fc:	e058      	b.n	4b0 <main+0xdc>
 3fe:	230f      	movs	r3, #15
 400:	4023      	ands	r3, r4
 402:	d102      	bne.n	40a <main+0x36>
 404:	f7ff ff46 	bl	294 <cycle0>
 408:	e047      	b.n	49a <main+0xc6>
 40a:	2b01      	cmp	r3, #1
 40c:	d102      	bne.n	414 <main+0x40>
 40e:	f7ff ff4b 	bl	2a8 <cycle1>
 412:	e042      	b.n	49a <main+0xc6>
 414:	2b02      	cmp	r3, #2
 416:	d102      	bne.n	41e <main+0x4a>
 418:	f7ff ff50 	bl	2bc <cycle2>
 41c:	e03d      	b.n	49a <main+0xc6>
 41e:	2b03      	cmp	r3, #3
 420:	d102      	bne.n	428 <main+0x54>
 422:	f7ff ff55 	bl	2d0 <cycle3>
 426:	e038      	b.n	49a <main+0xc6>
 428:	2b04      	cmp	r3, #4
 42a:	d102      	bne.n	432 <main+0x5e>
 42c:	f7ff ff5a 	bl	2e4 <cycle4>
 430:	e033      	b.n	49a <main+0xc6>
 432:	2b05      	cmp	r3, #5
 434:	d102      	bne.n	43c <main+0x68>
 436:	f7ff ff5f 	bl	2f8 <cycle5>
 43a:	e02e      	b.n	49a <main+0xc6>
 43c:	2b06      	cmp	r3, #6
 43e:	d102      	bne.n	446 <main+0x72>
 440:	f7ff ff64 	bl	30c <cycle6>
 444:	e029      	b.n	49a <main+0xc6>
 446:	2b07      	cmp	r3, #7
 448:	d102      	bne.n	450 <main+0x7c>
 44a:	f7ff ff69 	bl	320 <cycle7>
 44e:	e024      	b.n	49a <main+0xc6>
 450:	2b08      	cmp	r3, #8
 452:	d102      	bne.n	45a <main+0x86>
 454:	f7ff ff6e 	bl	334 <cycle8>
 458:	e01f      	b.n	49a <main+0xc6>
 45a:	2b09      	cmp	r3, #9
 45c:	d102      	bne.n	464 <main+0x90>
 45e:	f7ff ff73 	bl	348 <cycle9>
 462:	e01a      	b.n	49a <main+0xc6>
 464:	2b0a      	cmp	r3, #10
 466:	d102      	bne.n	46e <main+0x9a>
 468:	f7ff ff78 	bl	35c <cycle10>
 46c:	e015      	b.n	49a <main+0xc6>
 46e:	2b0b      	cmp	r3, #11
 470:	d102      	bne.n	478 <main+0xa4>
 472:	f7ff ff7d 	bl	370 <cycle11>
 476:	e010      	b.n	49a <main+0xc6>
 478:	2b0c      	cmp	r3, #12
 47a:	d102      	bne.n	482 <main+0xae>
 47c:	f7ff ff82 	bl	384 <cycle12>
 480:	e00b      	b.n	49a <main+0xc6>
 482:	2b0d      	cmp	r3, #13
 484:	d102      	bne.n	48c <main+0xb8>
 486:	f7ff ff87 	bl	398 <cycle13>
 48a:	e006      	b.n	49a <main+0xc6>
 48c:	2b0e      	cmp	r3, #14
 48e:	d102      	bne.n	496 <main+0xc2>
 490:	f7ff ff8c 	bl	3ac <cycle14>
 494:	e001      	b.n	49a <main+0xc6>
 496:	f7ff ff93 	bl	3c0 <cycle15>
 49a:	682b      	ldr	r3, [r5, #0]
 49c:	20fa      	movs	r0, #250	; 0xfa
 49e:	3301      	adds	r3, #1
 4a0:	0080      	lsls	r0, r0, #2
 4a2:	3401      	adds	r4, #1
 4a4:	602b      	str	r3, [r5, #0]
 4a6:	f7ff fdfe 	bl	a6 <delay>
 4aa:	2c64      	cmp	r4, #100	; 0x64
 4ac:	d19d      	bne.n	3ea <main+0x16>
 4ae:	e7a0      	b.n	3f2 <main+0x1e>
 4b0:	20dd      	movs	r0, #221	; 0xdd
 4b2:	4911      	ldr	r1, [pc, #68]	; (4f8 <main+0x124>)
 4b4:	f7ff fe38 	bl	128 <mbus_write_message32>
 4b8:	4810      	ldr	r0, [pc, #64]	; (4fc <main+0x128>)
 4ba:	f7ff fdf4 	bl	a6 <delay>
 4be:	e7fe      	b.n	4be <main+0xea>
 4c0:	681a      	ldr	r2, [r3, #0]
 4c2:	490d      	ldr	r1, [pc, #52]	; (4f8 <main+0x124>)
 4c4:	3201      	adds	r2, #1
 4c6:	20dd      	movs	r0, #221	; 0xdd
 4c8:	601a      	str	r2, [r3, #0]
 4ca:	f7ff fe2d 	bl	128 <mbus_write_message32>
 4ce:	480b      	ldr	r0, [pc, #44]	; (4fc <main+0x128>)
 4d0:	f7ff fde9 	bl	a6 <delay>
 4d4:	2101      	movs	r1, #1
 4d6:	2002      	movs	r0, #2
 4d8:	1c0a      	adds	r2, r1, #0
 4da:	f7ff fe01 	bl	e0 <set_wakeup_timer>
 4de:	f7ff fe35 	bl	14c <mbus_sleep_all>
 4e2:	2001      	movs	r0, #1
 4e4:	bd38      	pop	{r3, r4, r5, pc}
 4e6:	46c0      	nop			; (mov r8, r8)
 4e8:	00000504 	.word	0x00000504
 4ec:	deadbeef 	.word	0xdeadbeef
 4f0:	00000500 	.word	0x00000500
 4f4:	000003e7 	.word	0x000003e7
 4f8:	0ea7f00d 	.word	0x0ea7f00d
 4fc:	00001388 	.word	0x00001388
