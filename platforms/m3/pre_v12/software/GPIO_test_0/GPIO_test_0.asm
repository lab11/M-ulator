
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

Disassembly of section .text.set_halt_until_mbus_rx:

00000110 <set_halt_until_mbus_rx>:
 110:	4805      	ldr	r0, [pc, #20]	; (128 <set_halt_until_mbus_rx+0x18>)
 112:	b508      	push	{r3, lr}
 114:	6801      	ldr	r1, [r0, #0]
 116:	4b05      	ldr	r3, [pc, #20]	; (12c <set_halt_until_mbus_rx+0x1c>)
 118:	4019      	ands	r1, r3
 11a:	2390      	movs	r3, #144	; 0x90
 11c:	021b      	lsls	r3, r3, #8
 11e:	4319      	orrs	r1, r3
 120:	f7ff ffe2 	bl	e8 <write_regfile>
 124:	bd08      	pop	{r3, pc}
 126:	46c0      	nop			; (mov r8, r8)
 128:	a0000028 	.word	0xa0000028
 12c:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.set_halt_until_mbus_tx:

00000130 <set_halt_until_mbus_tx>:
 130:	4805      	ldr	r0, [pc, #20]	; (148 <set_halt_until_mbus_tx+0x18>)
 132:	b508      	push	{r3, lr}
 134:	6801      	ldr	r1, [r0, #0]
 136:	4b05      	ldr	r3, [pc, #20]	; (14c <set_halt_until_mbus_tx+0x1c>)
 138:	4019      	ands	r1, r3
 13a:	23a0      	movs	r3, #160	; 0xa0
 13c:	021b      	lsls	r3, r3, #8
 13e:	4319      	orrs	r1, r3
 140:	f7ff ffd2 	bl	e8 <write_regfile>
 144:	bd08      	pop	{r3, pc}
 146:	46c0      	nop			; (mov r8, r8)
 148:	a0000028 	.word	0xa0000028
 14c:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.enable_io_pad:

00000150 <enable_io_pad>:
 150:	b508      	push	{r3, lr}
 152:	4802      	ldr	r0, [pc, #8]	; (15c <enable_io_pad+0xc>)
 154:	2103      	movs	r1, #3
 156:	f7ff ffc7 	bl	e8 <write_regfile>
 15a:	bd08      	pop	{r3, pc}
 15c:	a00000a4 	.word	0xa00000a4

Disassembly of section .text.disable_io_pad:

00000160 <disable_io_pad>:
 160:	b508      	push	{r3, lr}
 162:	4802      	ldr	r0, [pc, #8]	; (16c <disable_io_pad+0xc>)
 164:	2100      	movs	r1, #0
 166:	f7ff ffbf 	bl	e8 <write_regfile>
 16a:	bd08      	pop	{r3, pc}
 16c:	a00000a4 	.word	0xa00000a4

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
 2a4:	f7ff ff20 	bl	e8 <write_regfile>
 2a8:	4909      	ldr	r1, [pc, #36]	; (2d0 <initialization+0x38>)
 2aa:	20aa      	movs	r0, #170	; 0xaa
 2ac:	f7ff fefc 	bl	a8 <mbus_write_message32>
 2b0:	f7ff ff2e 	bl	110 <set_halt_until_mbus_rx>
 2b4:	2004      	movs	r0, #4
 2b6:	f7ff feff 	bl	b8 <mbus_enumerate>
 2ba:	f7ff ff39 	bl	130 <set_halt_until_mbus_tx>
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
 2e8:	f7ff ff22 	bl	130 <set_halt_until_mbus_tx>
 2ec:	2064      	movs	r0, #100	; 0x64
 2ee:	f7ff fef3 	bl	d8 <delay>
 2f2:	f7ff ff2d 	bl	150 <enable_io_pad>
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
 310:	f7ff fee2 	bl	d8 <delay>
 314:	f7ff ff24 	bl	160 <disable_io_pad>
 318:	490b      	ldr	r1, [pc, #44]	; (348 <main+0x74>)
 31a:	20dd      	movs	r0, #221	; 0xdd
 31c:	f7ff fec4 	bl	a8 <mbus_write_message32>
 320:	f7ff fed4 	bl	cc <mbus_sleep_all>
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
