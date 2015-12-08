
XO_test_0/XO_test_0.elf:     file format elf32-littlearm


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
  40:	0000015d 	.word	0x0000015d
  44:	00000169 	.word	0x00000169
  48:	00000175 	.word	0x00000175
  4c:	00000181 	.word	0x00000181
  50:	0000018d 	.word	0x0000018d
  54:	00000199 	.word	0x00000199
  58:	000001a5 	.word	0x000001a5
  5c:	000001b1 	.word	0x000001b1
  60:	000001bd 	.word	0x000001bd
  64:	000001cd 	.word	0x000001cd
  68:	000001dd 	.word	0x000001dd
  6c:	000001ed 	.word	0x000001ed
  70:	000001fd 	.word	0x000001fd
  74:	0000020d 	.word	0x0000020d
  78:	0000021d 	.word	0x0000021d
  7c:	0000022d 	.word	0x0000022d
  80:	0000023d 	.word	0x0000023d
	...

00000090 <hang>:
  90:	e7fe      	b.n	90 <hang>
	...

000000a0 <_start>:
  a0:	f000 f948 	bl	334 <main>
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

Disassembly of section .text.set_halt_until_mbus_rx:

000000e0 <set_halt_until_mbus_rx>:
  e0:	4805      	ldr	r0, [pc, #20]	; (f8 <set_halt_until_mbus_rx+0x18>)
  e2:	b508      	push	{r3, lr}
  e4:	6801      	ldr	r1, [r0, #0]
  e6:	4b05      	ldr	r3, [pc, #20]	; (fc <set_halt_until_mbus_rx+0x1c>)
  e8:	4019      	ands	r1, r3
  ea:	2390      	movs	r3, #144	; 0x90
  ec:	021b      	lsls	r3, r3, #8
  ee:	4319      	orrs	r1, r3
  f0:	f7ff ffe2 	bl	b8 <write_regfile>
  f4:	bd08      	pop	{r3, pc}
  f6:	46c0      	nop			; (mov r8, r8)
  f8:	a0000028 	.word	0xa0000028
  fc:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.set_halt_until_mbus_tx:

00000100 <set_halt_until_mbus_tx>:
 100:	4805      	ldr	r0, [pc, #20]	; (118 <set_halt_until_mbus_tx+0x18>)
 102:	b508      	push	{r3, lr}
 104:	6801      	ldr	r1, [r0, #0]
 106:	4b05      	ldr	r3, [pc, #20]	; (11c <set_halt_until_mbus_tx+0x1c>)
 108:	4019      	ands	r1, r3
 10a:	23a0      	movs	r3, #160	; 0xa0
 10c:	021b      	lsls	r3, r3, #8
 10e:	4319      	orrs	r1, r3
 110:	f7ff ffd2 	bl	b8 <write_regfile>
 114:	bd08      	pop	{r3, pc}
 116:	46c0      	nop			; (mov r8, r8)
 118:	a0000028 	.word	0xa0000028
 11c:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.mbus_write_message32:

00000120 <mbus_write_message32>:
 120:	4b02      	ldr	r3, [pc, #8]	; (12c <mbus_write_message32+0xc>)
 122:	0100      	lsls	r0, r0, #4
 124:	4318      	orrs	r0, r3
 126:	6001      	str	r1, [r0, #0]
 128:	2001      	movs	r0, #1
 12a:	4770      	bx	lr
 12c:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_enumerate:

00000130 <mbus_enumerate>:
 130:	0603      	lsls	r3, r0, #24
 132:	2080      	movs	r0, #128	; 0x80
 134:	0580      	lsls	r0, r0, #22
 136:	4318      	orrs	r0, r3
 138:	4b01      	ldr	r3, [pc, #4]	; (140 <mbus_enumerate+0x10>)
 13a:	6018      	str	r0, [r3, #0]
 13c:	4770      	bx	lr
 13e:	46c0      	nop			; (mov r8, r8)
 140:	a0003000 	.word	0xa0003000

Disassembly of section .text.init_interrupt:

00000144 <init_interrupt>:
 144:	4b02      	ldr	r3, [pc, #8]	; (150 <init_interrupt+0xc>)
 146:	4a03      	ldr	r2, [pc, #12]	; (154 <init_interrupt+0x10>)
 148:	6013      	str	r3, [r2, #0]
 14a:	4a03      	ldr	r2, [pc, #12]	; (158 <init_interrupt+0x14>)
 14c:	6013      	str	r3, [r2, #0]
 14e:	4770      	bx	lr
 150:	0001ffff 	.word	0x0001ffff
 154:	e000e280 	.word	0xe000e280
 158:	e000e100 	.word	0xe000e100

Disassembly of section .text.handler_ext_int_0:

0000015c <handler_ext_int_0>:
 15c:	4b01      	ldr	r3, [pc, #4]	; (164 <handler_ext_int_0+0x8>)
 15e:	2201      	movs	r2, #1
 160:	601a      	str	r2, [r3, #0]
 162:	4770      	bx	lr
 164:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

00000168 <handler_ext_int_1>:
 168:	4b01      	ldr	r3, [pc, #4]	; (170 <handler_ext_int_1+0x8>)
 16a:	2202      	movs	r2, #2
 16c:	601a      	str	r2, [r3, #0]
 16e:	4770      	bx	lr
 170:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

00000174 <handler_ext_int_2>:
 174:	4b01      	ldr	r3, [pc, #4]	; (17c <handler_ext_int_2+0x8>)
 176:	2204      	movs	r2, #4
 178:	601a      	str	r2, [r3, #0]
 17a:	4770      	bx	lr
 17c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

00000180 <handler_ext_int_3>:
 180:	4b01      	ldr	r3, [pc, #4]	; (188 <handler_ext_int_3+0x8>)
 182:	2208      	movs	r2, #8
 184:	601a      	str	r2, [r3, #0]
 186:	4770      	bx	lr
 188:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

0000018c <handler_ext_int_4>:
 18c:	4b01      	ldr	r3, [pc, #4]	; (194 <handler_ext_int_4+0x8>)
 18e:	2210      	movs	r2, #16
 190:	601a      	str	r2, [r3, #0]
 192:	4770      	bx	lr
 194:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

00000198 <handler_ext_int_5>:
 198:	4b01      	ldr	r3, [pc, #4]	; (1a0 <handler_ext_int_5+0x8>)
 19a:	2220      	movs	r2, #32
 19c:	601a      	str	r2, [r3, #0]
 19e:	4770      	bx	lr
 1a0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

000001a4 <handler_ext_int_6>:
 1a4:	4b01      	ldr	r3, [pc, #4]	; (1ac <handler_ext_int_6+0x8>)
 1a6:	2240      	movs	r2, #64	; 0x40
 1a8:	601a      	str	r2, [r3, #0]
 1aa:	4770      	bx	lr
 1ac:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

000001b0 <handler_ext_int_7>:
 1b0:	4b01      	ldr	r3, [pc, #4]	; (1b8 <handler_ext_int_7+0x8>)
 1b2:	2280      	movs	r2, #128	; 0x80
 1b4:	601a      	str	r2, [r3, #0]
 1b6:	4770      	bx	lr
 1b8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

000001bc <handler_ext_int_8>:
 1bc:	4b02      	ldr	r3, [pc, #8]	; (1c8 <handler_ext_int_8+0xc>)
 1be:	2280      	movs	r2, #128	; 0x80
 1c0:	0052      	lsls	r2, r2, #1
 1c2:	601a      	str	r2, [r3, #0]
 1c4:	4770      	bx	lr
 1c6:	46c0      	nop			; (mov r8, r8)
 1c8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

000001cc <handler_ext_int_9>:
 1cc:	4b02      	ldr	r3, [pc, #8]	; (1d8 <handler_ext_int_9+0xc>)
 1ce:	2280      	movs	r2, #128	; 0x80
 1d0:	0092      	lsls	r2, r2, #2
 1d2:	601a      	str	r2, [r3, #0]
 1d4:	4770      	bx	lr
 1d6:	46c0      	nop			; (mov r8, r8)
 1d8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

000001dc <handler_ext_int_10>:
 1dc:	4b02      	ldr	r3, [pc, #8]	; (1e8 <handler_ext_int_10+0xc>)
 1de:	2280      	movs	r2, #128	; 0x80
 1e0:	00d2      	lsls	r2, r2, #3
 1e2:	601a      	str	r2, [r3, #0]
 1e4:	4770      	bx	lr
 1e6:	46c0      	nop			; (mov r8, r8)
 1e8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

000001ec <handler_ext_int_11>:
 1ec:	4b02      	ldr	r3, [pc, #8]	; (1f8 <handler_ext_int_11+0xc>)
 1ee:	2280      	movs	r2, #128	; 0x80
 1f0:	0112      	lsls	r2, r2, #4
 1f2:	601a      	str	r2, [r3, #0]
 1f4:	4770      	bx	lr
 1f6:	46c0      	nop			; (mov r8, r8)
 1f8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

000001fc <handler_ext_int_12>:
 1fc:	4b02      	ldr	r3, [pc, #8]	; (208 <handler_ext_int_12+0xc>)
 1fe:	2280      	movs	r2, #128	; 0x80
 200:	0152      	lsls	r2, r2, #5
 202:	601a      	str	r2, [r3, #0]
 204:	4770      	bx	lr
 206:	46c0      	nop			; (mov r8, r8)
 208:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

0000020c <handler_ext_int_13>:
 20c:	4b02      	ldr	r3, [pc, #8]	; (218 <handler_ext_int_13+0xc>)
 20e:	2280      	movs	r2, #128	; 0x80
 210:	0192      	lsls	r2, r2, #6
 212:	601a      	str	r2, [r3, #0]
 214:	4770      	bx	lr
 216:	46c0      	nop			; (mov r8, r8)
 218:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

0000021c <handler_ext_int_14>:
 21c:	4b02      	ldr	r3, [pc, #8]	; (228 <handler_ext_int_14+0xc>)
 21e:	2280      	movs	r2, #128	; 0x80
 220:	01d2      	lsls	r2, r2, #7
 222:	601a      	str	r2, [r3, #0]
 224:	4770      	bx	lr
 226:	46c0      	nop			; (mov r8, r8)
 228:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_15:

0000022c <handler_ext_int_15>:
 22c:	4b02      	ldr	r3, [pc, #8]	; (238 <handler_ext_int_15+0xc>)
 22e:	2280      	movs	r2, #128	; 0x80
 230:	0212      	lsls	r2, r2, #8
 232:	601a      	str	r2, [r3, #0]
 234:	4770      	bx	lr
 236:	46c0      	nop			; (mov r8, r8)
 238:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_16:

0000023c <handler_ext_int_16>:
 23c:	4b02      	ldr	r3, [pc, #8]	; (248 <handler_ext_int_16+0xc>)
 23e:	2280      	movs	r2, #128	; 0x80
 240:	0252      	lsls	r2, r2, #9
 242:	601a      	str	r2, [r3, #0]
 244:	4770      	bx	lr
 246:	46c0      	nop			; (mov r8, r8)
 248:	e000e280 	.word	0xe000e280

Disassembly of section .text.initialization:

0000024c <initialization>:
 24c:	4a2b      	ldr	r2, [pc, #172]	; (2fc <initialization+0xb0>)
 24e:	4b2c      	ldr	r3, [pc, #176]	; (300 <initialization+0xb4>)
 250:	b570      	push	{r4, r5, r6, lr}
 252:	482c      	ldr	r0, [pc, #176]	; (304 <initialization+0xb8>)
 254:	492c      	ldr	r1, [pc, #176]	; (308 <initialization+0xbc>)
 256:	601a      	str	r2, [r3, #0]
 258:	24fa      	movs	r4, #250	; 0xfa
 25a:	f7ff ff2d 	bl	b8 <write_regfile>
 25e:	492b      	ldr	r1, [pc, #172]	; (30c <initialization+0xc0>)
 260:	20aa      	movs	r0, #170	; 0xaa
 262:	f7ff ff5d 	bl	120 <mbus_write_message32>
 266:	00a4      	lsls	r4, r4, #2
 268:	f7ff ff3a 	bl	e0 <set_halt_until_mbus_rx>
 26c:	2004      	movs	r0, #4
 26e:	f7ff ff5f 	bl	130 <mbus_enumerate>
 272:	f7ff ff45 	bl	100 <set_halt_until_mbus_tx>
 276:	1c20      	adds	r0, r4, #0
 278:	f7ff ff15 	bl	a6 <delay>
 27c:	2100      	movs	r1, #0
 27e:	20dd      	movs	r0, #221	; 0xdd
 280:	f7ff ff4e 	bl	120 <mbus_write_message32>
 284:	1c20      	adds	r0, r4, #0
 286:	4c22      	ldr	r4, [pc, #136]	; (310 <initialization+0xc4>)
 288:	4d22      	ldr	r5, [pc, #136]	; (314 <initialization+0xc8>)
 28a:	f7ff ff0c 	bl	a6 <delay>
 28e:	1c20      	adds	r0, r4, #0
 290:	4921      	ldr	r1, [pc, #132]	; (318 <initialization+0xcc>)
 292:	f7ff ff11 	bl	b8 <write_regfile>
 296:	4e21      	ldr	r6, [pc, #132]	; (31c <initialization+0xd0>)
 298:	1c28      	adds	r0, r5, #0
 29a:	f7ff ff04 	bl	a6 <delay>
 29e:	2101      	movs	r1, #1
 2a0:	20dd      	movs	r0, #221	; 0xdd
 2a2:	f7ff ff3d 	bl	120 <mbus_write_message32>
 2a6:	1c30      	adds	r0, r6, #0
 2a8:	f7ff fefd 	bl	a6 <delay>
 2ac:	1c20      	adds	r0, r4, #0
 2ae:	491c      	ldr	r1, [pc, #112]	; (320 <initialization+0xd4>)
 2b0:	f7ff ff02 	bl	b8 <write_regfile>
 2b4:	1c28      	adds	r0, r5, #0
 2b6:	f7ff fef6 	bl	a6 <delay>
 2ba:	2102      	movs	r1, #2
 2bc:	20dd      	movs	r0, #221	; 0xdd
 2be:	f7ff ff2f 	bl	120 <mbus_write_message32>
 2c2:	4818      	ldr	r0, [pc, #96]	; (324 <initialization+0xd8>)
 2c4:	f7ff feef 	bl	a6 <delay>
 2c8:	1c20      	adds	r0, r4, #0
 2ca:	4917      	ldr	r1, [pc, #92]	; (328 <initialization+0xdc>)
 2cc:	f7ff fef4 	bl	b8 <write_regfile>
 2d0:	1c28      	adds	r0, r5, #0
 2d2:	f7ff fee8 	bl	a6 <delay>
 2d6:	2103      	movs	r1, #3
 2d8:	20dd      	movs	r0, #221	; 0xdd
 2da:	f7ff ff21 	bl	120 <mbus_write_message32>
 2de:	4813      	ldr	r0, [pc, #76]	; (32c <initialization+0xe0>)
 2e0:	f7ff fee1 	bl	a6 <delay>
 2e4:	1c20      	adds	r0, r4, #0
 2e6:	4912      	ldr	r1, [pc, #72]	; (330 <initialization+0xe4>)
 2e8:	f7ff fee6 	bl	b8 <write_regfile>
 2ec:	1c30      	adds	r0, r6, #0
 2ee:	f7ff feda 	bl	a6 <delay>
 2f2:	2104      	movs	r1, #4
 2f4:	20dd      	movs	r0, #221	; 0xdd
 2f6:	f7ff ff13 	bl	120 <mbus_write_message32>
 2fa:	bd70      	pop	{r4, r5, r6, pc}
 2fc:	deadbeef 	.word	0xdeadbeef
 300:	00000434 	.word	0x00000434
 304:	a0000020 	.word	0xa0000020
 308:	0000dead 	.word	0x0000dead
 30c:	aaaaaaaa 	.word	0xaaaaaaaa
 310:	a0000094 	.word	0xa0000094
 314:	000003fd 	.word	0x000003fd
 318:	000011e5 	.word	0x000011e5
 31c:	0000ea60 	.word	0x0000ea60
 320:	000011e7 	.word	0x000011e7
 324:	00009c40 	.word	0x00009c40
 328:	000011e4 	.word	0x000011e4
 32c:	000927c0 	.word	0x000927c0
 330:	000011d4 	.word	0x000011d4

Disassembly of section .text.startup.main:

00000334 <main>:
 334:	b570      	push	{r4, r5, r6, lr}
 336:	f7ff ff05 	bl	144 <init_interrupt>
 33a:	4b39      	ldr	r3, [pc, #228]	; (420 <main+0xec>)
 33c:	681a      	ldr	r2, [r3, #0]
 33e:	4b39      	ldr	r3, [pc, #228]	; (424 <main+0xf0>)
 340:	429a      	cmp	r2, r3
 342:	d001      	beq.n	348 <main+0x14>
 344:	f7ff ff82 	bl	24c <initialization>
 348:	4837      	ldr	r0, [pc, #220]	; (428 <main+0xf4>)
 34a:	f7ff feac 	bl	a6 <delay>
 34e:	f7ff fed7 	bl	100 <set_halt_until_mbus_tx>
 352:	4d36      	ldr	r5, [pc, #216]	; (42c <main+0xf8>)
 354:	2601      	movs	r6, #1
 356:	4c36      	ldr	r4, [pc, #216]	; (430 <main+0xfc>)
 358:	602e      	str	r6, [r5, #0]
 35a:	6821      	ldr	r1, [r4, #0]
 35c:	20dd      	movs	r0, #221	; 0xdd
 35e:	f7ff fedf 	bl	120 <mbus_write_message32>
 362:	4831      	ldr	r0, [pc, #196]	; (428 <main+0xf4>)
 364:	f7ff fe9f 	bl	a6 <delay>
 368:	6821      	ldr	r1, [r4, #0]
 36a:	20dd      	movs	r0, #221	; 0xdd
 36c:	f7ff fed8 	bl	120 <mbus_write_message32>
 370:	482d      	ldr	r0, [pc, #180]	; (428 <main+0xf4>)
 372:	f7ff fe98 	bl	a6 <delay>
 376:	6821      	ldr	r1, [r4, #0]
 378:	20dd      	movs	r0, #221	; 0xdd
 37a:	f7ff fed1 	bl	120 <mbus_write_message32>
 37e:	482a      	ldr	r0, [pc, #168]	; (428 <main+0xf4>)
 380:	f7ff fe91 	bl	a6 <delay>
 384:	6821      	ldr	r1, [r4, #0]
 386:	20dd      	movs	r0, #221	; 0xdd
 388:	f7ff feca 	bl	120 <mbus_write_message32>
 38c:	4826      	ldr	r0, [pc, #152]	; (428 <main+0xf4>)
 38e:	f7ff fe8a 	bl	a6 <delay>
 392:	6821      	ldr	r1, [r4, #0]
 394:	20dd      	movs	r0, #221	; 0xdd
 396:	f7ff fec3 	bl	120 <mbus_write_message32>
 39a:	4823      	ldr	r0, [pc, #140]	; (428 <main+0xf4>)
 39c:	f7ff fe83 	bl	a6 <delay>
 3a0:	6821      	ldr	r1, [r4, #0]
 3a2:	20dd      	movs	r0, #221	; 0xdd
 3a4:	f7ff febc 	bl	120 <mbus_write_message32>
 3a8:	481f      	ldr	r0, [pc, #124]	; (428 <main+0xf4>)
 3aa:	f7ff fe7c 	bl	a6 <delay>
 3ae:	6821      	ldr	r1, [r4, #0]
 3b0:	20dd      	movs	r0, #221	; 0xdd
 3b2:	f7ff feb5 	bl	120 <mbus_write_message32>
 3b6:	481c      	ldr	r0, [pc, #112]	; (428 <main+0xf4>)
 3b8:	f7ff fe75 	bl	a6 <delay>
 3bc:	6821      	ldr	r1, [r4, #0]
 3be:	20dd      	movs	r0, #221	; 0xdd
 3c0:	f7ff feae 	bl	120 <mbus_write_message32>
 3c4:	4818      	ldr	r0, [pc, #96]	; (428 <main+0xf4>)
 3c6:	f7ff fe6e 	bl	a6 <delay>
 3ca:	6821      	ldr	r1, [r4, #0]
 3cc:	20dd      	movs	r0, #221	; 0xdd
 3ce:	f7ff fea7 	bl	120 <mbus_write_message32>
 3d2:	4815      	ldr	r0, [pc, #84]	; (428 <main+0xf4>)
 3d4:	f7ff fe67 	bl	a6 <delay>
 3d8:	6821      	ldr	r1, [r4, #0]
 3da:	20dd      	movs	r0, #221	; 0xdd
 3dc:	f7ff fea0 	bl	120 <mbus_write_message32>
 3e0:	4811      	ldr	r0, [pc, #68]	; (428 <main+0xf4>)
 3e2:	f7ff fe60 	bl	a6 <delay>
 3e6:	6821      	ldr	r1, [r4, #0]
 3e8:	20dd      	movs	r0, #221	; 0xdd
 3ea:	f7ff fe99 	bl	120 <mbus_write_message32>
 3ee:	480e      	ldr	r0, [pc, #56]	; (428 <main+0xf4>)
 3f0:	f7ff fe59 	bl	a6 <delay>
 3f4:	6821      	ldr	r1, [r4, #0]
 3f6:	20dd      	movs	r0, #221	; 0xdd
 3f8:	f7ff fe92 	bl	120 <mbus_write_message32>
 3fc:	480a      	ldr	r0, [pc, #40]	; (428 <main+0xf4>)
 3fe:	f7ff fe52 	bl	a6 <delay>
 402:	6821      	ldr	r1, [r4, #0]
 404:	20dd      	movs	r0, #221	; 0xdd
 406:	f7ff fe8b 	bl	120 <mbus_write_message32>
 40a:	4807      	ldr	r0, [pc, #28]	; (428 <main+0xf4>)
 40c:	f7ff fe4b 	bl	a6 <delay>
 410:	6821      	ldr	r1, [r4, #0]
 412:	20dd      	movs	r0, #221	; 0xdd
 414:	f7ff fe84 	bl	120 <mbus_write_message32>
 418:	4803      	ldr	r0, [pc, #12]	; (428 <main+0xf4>)
 41a:	f7ff fe44 	bl	a6 <delay>
 41e:	e79a      	b.n	356 <main+0x22>
 420:	00000434 	.word	0x00000434
 424:	deadbeef 	.word	0xdeadbeef
 428:	000003fd 	.word	0x000003fd
 42c:	a0001400 	.word	0xa0001400
 430:	a000009c 	.word	0xa000009c
