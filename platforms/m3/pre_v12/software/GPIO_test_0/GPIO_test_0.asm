
GPIO_test_0/GPIO_test_0.elf:     file format elf32-littlearm


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
  40:	0000018d 	.word	0x0000018d
  44:	00000199 	.word	0x00000199
  48:	000001a5 	.word	0x000001a5
  4c:	000001b1 	.word	0x000001b1
  50:	000001bd 	.word	0x000001bd
  54:	000001c9 	.word	0x000001c9
  58:	000001d5 	.word	0x000001d5
  5c:	000001e1 	.word	0x000001e1
  60:	000001ed 	.word	0x000001ed
  64:	000001fd 	.word	0x000001fd
  68:	0000020d 	.word	0x0000020d
  6c:	0000021d 	.word	0x0000021d
  70:	0000022d 	.word	0x0000022d
  74:	0000023d 	.word	0x0000023d
  78:	0000024d 	.word	0x0000024d
  7c:	0000025d 	.word	0x0000025d
  80:	0000026d 	.word	0x0000026d
	...

00000090 <hang>:
  90:	e7fe      	b.n	90 <hang>
	...

000000a0 <_start>:
  a0:	f000 f918 	bl	2d4 <main>
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

Disassembly of section .text.enable_io_pad:

00000120 <enable_io_pad>:
 120:	b508      	push	{r3, lr}
 122:	4802      	ldr	r0, [pc, #8]	; (12c <enable_io_pad+0xc>)
 124:	2103      	movs	r1, #3
 126:	f7ff ffc7 	bl	b8 <write_regfile>
 12a:	bd08      	pop	{r3, pc}
 12c:	a00000a4 	.word	0xa00000a4

Disassembly of section .text.disable_io_pad:

00000130 <disable_io_pad>:
 130:	b508      	push	{r3, lr}
 132:	4802      	ldr	r0, [pc, #8]	; (13c <disable_io_pad+0xc>)
 134:	2100      	movs	r1, #0
 136:	f7ff ffbf 	bl	b8 <write_regfile>
 13a:	bd08      	pop	{r3, pc}
 13c:	a00000a4 	.word	0xa00000a4

Disassembly of section .text.mbus_write_message32:

00000140 <mbus_write_message32>:
 140:	4b02      	ldr	r3, [pc, #8]	; (14c <mbus_write_message32+0xc>)
 142:	0100      	lsls	r0, r0, #4
 144:	4318      	orrs	r0, r3
 146:	6001      	str	r1, [r0, #0]
 148:	2001      	movs	r0, #1
 14a:	4770      	bx	lr
 14c:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_enumerate:

00000150 <mbus_enumerate>:
 150:	0603      	lsls	r3, r0, #24
 152:	2080      	movs	r0, #128	; 0x80
 154:	0580      	lsls	r0, r0, #22
 156:	4318      	orrs	r0, r3
 158:	4b01      	ldr	r3, [pc, #4]	; (160 <mbus_enumerate+0x10>)
 15a:	6018      	str	r0, [r3, #0]
 15c:	4770      	bx	lr
 15e:	46c0      	nop			; (mov r8, r8)
 160:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

00000164 <mbus_sleep_all>:
 164:	4b01      	ldr	r3, [pc, #4]	; (16c <mbus_sleep_all+0x8>)
 166:	2200      	movs	r2, #0
 168:	601a      	str	r2, [r3, #0]
 16a:	4770      	bx	lr
 16c:	a0003010 	.word	0xa0003010

Disassembly of section .text.init_interrupt:

00000170 <init_interrupt>:
 170:	4a03      	ldr	r2, [pc, #12]	; (180 <init_interrupt+0x10>)
 172:	4b04      	ldr	r3, [pc, #16]	; (184 <init_interrupt+0x14>)
 174:	601a      	str	r2, [r3, #0]
 176:	4b04      	ldr	r3, [pc, #16]	; (188 <init_interrupt+0x18>)
 178:	2280      	movs	r2, #128	; 0x80
 17a:	0252      	lsls	r2, r2, #9
 17c:	601a      	str	r2, [r3, #0]
 17e:	4770      	bx	lr
 180:	0001ffff 	.word	0x0001ffff
 184:	e000e280 	.word	0xe000e280
 188:	e000e100 	.word	0xe000e100

Disassembly of section .text.handler_ext_int_0:

0000018c <handler_ext_int_0>:
 18c:	4b01      	ldr	r3, [pc, #4]	; (194 <handler_ext_int_0+0x8>)
 18e:	2201      	movs	r2, #1
 190:	601a      	str	r2, [r3, #0]
 192:	4770      	bx	lr
 194:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

00000198 <handler_ext_int_1>:
 198:	4b01      	ldr	r3, [pc, #4]	; (1a0 <handler_ext_int_1+0x8>)
 19a:	2202      	movs	r2, #2
 19c:	601a      	str	r2, [r3, #0]
 19e:	4770      	bx	lr
 1a0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

000001a4 <handler_ext_int_2>:
 1a4:	4b01      	ldr	r3, [pc, #4]	; (1ac <handler_ext_int_2+0x8>)
 1a6:	2204      	movs	r2, #4
 1a8:	601a      	str	r2, [r3, #0]
 1aa:	4770      	bx	lr
 1ac:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

000001b0 <handler_ext_int_3>:
 1b0:	4b01      	ldr	r3, [pc, #4]	; (1b8 <handler_ext_int_3+0x8>)
 1b2:	2208      	movs	r2, #8
 1b4:	601a      	str	r2, [r3, #0]
 1b6:	4770      	bx	lr
 1b8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

000001bc <handler_ext_int_4>:
 1bc:	4b01      	ldr	r3, [pc, #4]	; (1c4 <handler_ext_int_4+0x8>)
 1be:	2210      	movs	r2, #16
 1c0:	601a      	str	r2, [r3, #0]
 1c2:	4770      	bx	lr
 1c4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

000001c8 <handler_ext_int_5>:
 1c8:	4b01      	ldr	r3, [pc, #4]	; (1d0 <handler_ext_int_5+0x8>)
 1ca:	2220      	movs	r2, #32
 1cc:	601a      	str	r2, [r3, #0]
 1ce:	4770      	bx	lr
 1d0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

000001d4 <handler_ext_int_6>:
 1d4:	4b01      	ldr	r3, [pc, #4]	; (1dc <handler_ext_int_6+0x8>)
 1d6:	2240      	movs	r2, #64	; 0x40
 1d8:	601a      	str	r2, [r3, #0]
 1da:	4770      	bx	lr
 1dc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

000001e0 <handler_ext_int_7>:
 1e0:	4b01      	ldr	r3, [pc, #4]	; (1e8 <handler_ext_int_7+0x8>)
 1e2:	2280      	movs	r2, #128	; 0x80
 1e4:	601a      	str	r2, [r3, #0]
 1e6:	4770      	bx	lr
 1e8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

000001ec <handler_ext_int_8>:
 1ec:	4b02      	ldr	r3, [pc, #8]	; (1f8 <handler_ext_int_8+0xc>)
 1ee:	2280      	movs	r2, #128	; 0x80
 1f0:	0052      	lsls	r2, r2, #1
 1f2:	601a      	str	r2, [r3, #0]
 1f4:	4770      	bx	lr
 1f6:	46c0      	nop			; (mov r8, r8)
 1f8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

000001fc <handler_ext_int_9>:
 1fc:	4b02      	ldr	r3, [pc, #8]	; (208 <handler_ext_int_9+0xc>)
 1fe:	2280      	movs	r2, #128	; 0x80
 200:	0092      	lsls	r2, r2, #2
 202:	601a      	str	r2, [r3, #0]
 204:	4770      	bx	lr
 206:	46c0      	nop			; (mov r8, r8)
 208:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

0000020c <handler_ext_int_10>:
 20c:	4b02      	ldr	r3, [pc, #8]	; (218 <handler_ext_int_10+0xc>)
 20e:	2280      	movs	r2, #128	; 0x80
 210:	00d2      	lsls	r2, r2, #3
 212:	601a      	str	r2, [r3, #0]
 214:	4770      	bx	lr
 216:	46c0      	nop			; (mov r8, r8)
 218:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

0000021c <handler_ext_int_11>:
 21c:	4b02      	ldr	r3, [pc, #8]	; (228 <handler_ext_int_11+0xc>)
 21e:	2280      	movs	r2, #128	; 0x80
 220:	0112      	lsls	r2, r2, #4
 222:	601a      	str	r2, [r3, #0]
 224:	4770      	bx	lr
 226:	46c0      	nop			; (mov r8, r8)
 228:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

0000022c <handler_ext_int_12>:
 22c:	4b02      	ldr	r3, [pc, #8]	; (238 <handler_ext_int_12+0xc>)
 22e:	2280      	movs	r2, #128	; 0x80
 230:	0152      	lsls	r2, r2, #5
 232:	601a      	str	r2, [r3, #0]
 234:	4770      	bx	lr
 236:	46c0      	nop			; (mov r8, r8)
 238:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

0000023c <handler_ext_int_13>:
 23c:	4b02      	ldr	r3, [pc, #8]	; (248 <handler_ext_int_13+0xc>)
 23e:	2280      	movs	r2, #128	; 0x80
 240:	0192      	lsls	r2, r2, #6
 242:	601a      	str	r2, [r3, #0]
 244:	4770      	bx	lr
 246:	46c0      	nop			; (mov r8, r8)
 248:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

0000024c <handler_ext_int_14>:
 24c:	4b02      	ldr	r3, [pc, #8]	; (258 <handler_ext_int_14+0xc>)
 24e:	2280      	movs	r2, #128	; 0x80
 250:	01d2      	lsls	r2, r2, #7
 252:	601a      	str	r2, [r3, #0]
 254:	4770      	bx	lr
 256:	46c0      	nop			; (mov r8, r8)
 258:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_15:

0000025c <handler_ext_int_15>:
 25c:	4b02      	ldr	r3, [pc, #8]	; (268 <handler_ext_int_15+0xc>)
 25e:	2280      	movs	r2, #128	; 0x80
 260:	0212      	lsls	r2, r2, #8
 262:	601a      	str	r2, [r3, #0]
 264:	4770      	bx	lr
 266:	46c0      	nop			; (mov r8, r8)
 268:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_16:

0000026c <handler_ext_int_16>:
 26c:	4b07      	ldr	r3, [pc, #28]	; (28c <handler_ext_int_16+0x20>)
 26e:	2280      	movs	r2, #128	; 0x80
 270:	0252      	lsls	r2, r2, #9
 272:	b500      	push	{lr}
 274:	601a      	str	r2, [r3, #0]
 276:	4b06      	ldr	r3, [pc, #24]	; (290 <handler_ext_int_16+0x24>)
 278:	681a      	ldr	r2, [r3, #0]
 27a:	3201      	adds	r2, #1
 27c:	601a      	str	r2, [r3, #0]
 27e:	681a      	ldr	r2, [r3, #0]
 280:	2a07      	cmp	r2, #7
 282:	d802      	bhi.n	28a <handler_ext_int_16+0x1e>
 284:	681a      	ldr	r2, [r3, #0]
 286:	4b03      	ldr	r3, [pc, #12]	; (294 <handler_ext_int_16+0x28>)
 288:	601a      	str	r2, [r3, #0]
 28a:	bd00      	pop	{pc}
 28c:	e000e280 	.word	0xe000e280
 290:	00000350 	.word	0x00000350
 294:	a0005000 	.word	0xa0005000

Disassembly of section .text.initialization:

00000298 <initialization>:
 298:	b508      	push	{r3, lr}
 29a:	4a09      	ldr	r2, [pc, #36]	; (2c0 <initialization+0x28>)
 29c:	4b09      	ldr	r3, [pc, #36]	; (2c4 <initialization+0x2c>)
 29e:	480a      	ldr	r0, [pc, #40]	; (2c8 <initialization+0x30>)
 2a0:	490a      	ldr	r1, [pc, #40]	; (2cc <initialization+0x34>)
 2a2:	601a      	str	r2, [r3, #0]
 2a4:	f7ff ff08 	bl	b8 <write_regfile>
 2a8:	4909      	ldr	r1, [pc, #36]	; (2d0 <initialization+0x38>)
 2aa:	20aa      	movs	r0, #170	; 0xaa
 2ac:	f7ff ff48 	bl	140 <mbus_write_message32>
 2b0:	f7ff ff16 	bl	e0 <set_halt_until_mbus_rx>
 2b4:	2004      	movs	r0, #4
 2b6:	f7ff ff4b 	bl	150 <mbus_enumerate>
 2ba:	f7ff ff21 	bl	100 <set_halt_until_mbus_tx>
 2be:	bd08      	pop	{r3, pc}
 2c0:	deadbeef 	.word	0xdeadbeef
 2c4:	0000034c 	.word	0x0000034c
 2c8:	a0000020 	.word	0xa0000020
 2cc:	0000dead 	.word	0x0000dead
 2d0:	aaaaaaaa 	.word	0xaaaaaaaa

Disassembly of section .text.startup.main:

000002d4 <main>:
 2d4:	b508      	push	{r3, lr}
 2d6:	f7ff ff4b 	bl	170 <init_interrupt>
 2da:	4b13      	ldr	r3, [pc, #76]	; (328 <main+0x54>)
 2dc:	681a      	ldr	r2, [r3, #0]
 2de:	4b13      	ldr	r3, [pc, #76]	; (32c <main+0x58>)
 2e0:	429a      	cmp	r2, r3
 2e2:	d001      	beq.n	2e8 <main+0x14>
 2e4:	f7ff ffd8 	bl	298 <initialization>
 2e8:	f7ff ff0a 	bl	100 <set_halt_until_mbus_tx>
 2ec:	2064      	movs	r0, #100	; 0x64
 2ee:	f7ff feda 	bl	a6 <delay>
 2f2:	f7ff ff15 	bl	120 <enable_io_pad>
 2f6:	4a0e      	ldr	r2, [pc, #56]	; (330 <main+0x5c>)
 2f8:	4b0e      	ldr	r3, [pc, #56]	; (334 <main+0x60>)
 2fa:	20fa      	movs	r0, #250	; 0xfa
 2fc:	601a      	str	r2, [r3, #0]
 2fe:	4a0e      	ldr	r2, [pc, #56]	; (338 <main+0x64>)
 300:	2302      	movs	r3, #2
 302:	6013      	str	r3, [r2, #0]
 304:	4a0d      	ldr	r2, [pc, #52]	; (33c <main+0x68>)
 306:	0080      	lsls	r0, r0, #2
 308:	6013      	str	r3, [r2, #0]
 30a:	4a0d      	ldr	r2, [pc, #52]	; (340 <main+0x6c>)
 30c:	4b0d      	ldr	r3, [pc, #52]	; (344 <main+0x70>)
 30e:	601a      	str	r2, [r3, #0]
 310:	f7ff fec9 	bl	a6 <delay>
 314:	f7ff ff0c 	bl	130 <disable_io_pad>
 318:	490b      	ldr	r1, [pc, #44]	; (348 <main+0x74>)
 31a:	20dd      	movs	r0, #221	; 0xdd
 31c:	f7ff ff10 	bl	140 <mbus_write_message32>
 320:	f7ff ff20 	bl	164 <mbus_sleep_all>
 324:	e7fe      	b.n	324 <main+0x50>
 326:	46c0      	nop			; (mov r8, r8)
 328:	0000034c 	.word	0x0000034c
 32c:	deadbeef 	.word	0xdeadbeef
 330:	00000287 	.word	0x00000287
 334:	a0004000 	.word	0xa0004000
 338:	a0004004 	.word	0xa0004004
 33c:	a0004010 	.word	0xa0004010
 340:	00001234 	.word	0x00001234
 344:	a0004008 	.word	0xa0004008
 348:	0ea7f00d 	.word	0x0ea7f00d
