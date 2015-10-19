
CORv1_load_0/CORv1_load_0.elf:     file format elf32-littlearm


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
  40:	0000019d 	.word	0x0000019d
  44:	000001a9 	.word	0x000001a9
  48:	000001b5 	.word	0x000001b5
  4c:	000001c1 	.word	0x000001c1
  50:	000001cd 	.word	0x000001cd
  54:	000001d9 	.word	0x000001d9
  58:	000001e5 	.word	0x000001e5
  5c:	000001f1 	.word	0x000001f1
  60:	000001fd 	.word	0x000001fd
  64:	0000020d 	.word	0x0000020d
  68:	0000021d 	.word	0x0000021d
  6c:	0000022d 	.word	0x0000022d
  70:	0000023d 	.word	0x0000023d
  74:	0000024d 	.word	0x0000024d
  78:	0000025d 	.word	0x0000025d
  7c:	0000026d 	.word	0x0000026d
  80:	0000027d 	.word	0x0000027d
	...

00000090 <hang>:
  90:	e7fe      	b.n	90 <hang>
	...

000000a0 <_start>:
  a0:	f000 f912 	bl	2c8 <main>
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

Disassembly of section .text.init_interrupt:

00000180 <init_interrupt>:
 180:	4a03      	ldr	r2, [pc, #12]	; (190 <init_interrupt+0x10>)
 182:	4b04      	ldr	r3, [pc, #16]	; (194 <init_interrupt+0x14>)
 184:	601a      	str	r2, [r3, #0]
 186:	4b04      	ldr	r3, [pc, #16]	; (198 <init_interrupt+0x18>)
 188:	2200      	movs	r2, #0
 18a:	601a      	str	r2, [r3, #0]
 18c:	4770      	bx	lr
 18e:	46c0      	nop			; (mov r8, r8)
 190:	0001ffff 	.word	0x0001ffff
 194:	e000e280 	.word	0xe000e280
 198:	e000e100 	.word	0xe000e100

Disassembly of section .text.handler_ext_int_0:

0000019c <handler_ext_int_0>:
 19c:	4b01      	ldr	r3, [pc, #4]	; (1a4 <handler_ext_int_0+0x8>)
 19e:	2201      	movs	r2, #1
 1a0:	601a      	str	r2, [r3, #0]
 1a2:	4770      	bx	lr
 1a4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

000001a8 <handler_ext_int_1>:
 1a8:	4b01      	ldr	r3, [pc, #4]	; (1b0 <handler_ext_int_1+0x8>)
 1aa:	2202      	movs	r2, #2
 1ac:	601a      	str	r2, [r3, #0]
 1ae:	4770      	bx	lr
 1b0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

000001b4 <handler_ext_int_2>:
 1b4:	4b01      	ldr	r3, [pc, #4]	; (1bc <handler_ext_int_2+0x8>)
 1b6:	2204      	movs	r2, #4
 1b8:	601a      	str	r2, [r3, #0]
 1ba:	4770      	bx	lr
 1bc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

000001c0 <handler_ext_int_3>:
 1c0:	4b01      	ldr	r3, [pc, #4]	; (1c8 <handler_ext_int_3+0x8>)
 1c2:	2208      	movs	r2, #8
 1c4:	601a      	str	r2, [r3, #0]
 1c6:	4770      	bx	lr
 1c8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

000001cc <handler_ext_int_4>:
 1cc:	4b01      	ldr	r3, [pc, #4]	; (1d4 <handler_ext_int_4+0x8>)
 1ce:	2210      	movs	r2, #16
 1d0:	601a      	str	r2, [r3, #0]
 1d2:	4770      	bx	lr
 1d4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

000001d8 <handler_ext_int_5>:
 1d8:	4b01      	ldr	r3, [pc, #4]	; (1e0 <handler_ext_int_5+0x8>)
 1da:	2220      	movs	r2, #32
 1dc:	601a      	str	r2, [r3, #0]
 1de:	4770      	bx	lr
 1e0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

000001e4 <handler_ext_int_6>:
 1e4:	4b01      	ldr	r3, [pc, #4]	; (1ec <handler_ext_int_6+0x8>)
 1e6:	2240      	movs	r2, #64	; 0x40
 1e8:	601a      	str	r2, [r3, #0]
 1ea:	4770      	bx	lr
 1ec:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

000001f0 <handler_ext_int_7>:
 1f0:	4b01      	ldr	r3, [pc, #4]	; (1f8 <handler_ext_int_7+0x8>)
 1f2:	2280      	movs	r2, #128	; 0x80
 1f4:	601a      	str	r2, [r3, #0]
 1f6:	4770      	bx	lr
 1f8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

000001fc <handler_ext_int_8>:
 1fc:	4b02      	ldr	r3, [pc, #8]	; (208 <handler_ext_int_8+0xc>)
 1fe:	2280      	movs	r2, #128	; 0x80
 200:	0052      	lsls	r2, r2, #1
 202:	601a      	str	r2, [r3, #0]
 204:	4770      	bx	lr
 206:	46c0      	nop			; (mov r8, r8)
 208:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

0000020c <handler_ext_int_9>:
 20c:	4b02      	ldr	r3, [pc, #8]	; (218 <handler_ext_int_9+0xc>)
 20e:	2280      	movs	r2, #128	; 0x80
 210:	0092      	lsls	r2, r2, #2
 212:	601a      	str	r2, [r3, #0]
 214:	4770      	bx	lr
 216:	46c0      	nop			; (mov r8, r8)
 218:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

0000021c <handler_ext_int_10>:
 21c:	4b02      	ldr	r3, [pc, #8]	; (228 <handler_ext_int_10+0xc>)
 21e:	2280      	movs	r2, #128	; 0x80
 220:	00d2      	lsls	r2, r2, #3
 222:	601a      	str	r2, [r3, #0]
 224:	4770      	bx	lr
 226:	46c0      	nop			; (mov r8, r8)
 228:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

0000022c <handler_ext_int_11>:
 22c:	4b02      	ldr	r3, [pc, #8]	; (238 <handler_ext_int_11+0xc>)
 22e:	2280      	movs	r2, #128	; 0x80
 230:	0112      	lsls	r2, r2, #4
 232:	601a      	str	r2, [r3, #0]
 234:	4770      	bx	lr
 236:	46c0      	nop			; (mov r8, r8)
 238:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

0000023c <handler_ext_int_12>:
 23c:	4b02      	ldr	r3, [pc, #8]	; (248 <handler_ext_int_12+0xc>)
 23e:	2280      	movs	r2, #128	; 0x80
 240:	0152      	lsls	r2, r2, #5
 242:	601a      	str	r2, [r3, #0]
 244:	4770      	bx	lr
 246:	46c0      	nop			; (mov r8, r8)
 248:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

0000024c <handler_ext_int_13>:
 24c:	4b02      	ldr	r3, [pc, #8]	; (258 <handler_ext_int_13+0xc>)
 24e:	2280      	movs	r2, #128	; 0x80
 250:	0192      	lsls	r2, r2, #6
 252:	601a      	str	r2, [r3, #0]
 254:	4770      	bx	lr
 256:	46c0      	nop			; (mov r8, r8)
 258:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

0000025c <handler_ext_int_14>:
 25c:	4b02      	ldr	r3, [pc, #8]	; (268 <handler_ext_int_14+0xc>)
 25e:	2280      	movs	r2, #128	; 0x80
 260:	01d2      	lsls	r2, r2, #7
 262:	601a      	str	r2, [r3, #0]
 264:	4770      	bx	lr
 266:	46c0      	nop			; (mov r8, r8)
 268:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_15:

0000026c <handler_ext_int_15>:
 26c:	4b02      	ldr	r3, [pc, #8]	; (278 <handler_ext_int_15+0xc>)
 26e:	2280      	movs	r2, #128	; 0x80
 270:	0212      	lsls	r2, r2, #8
 272:	601a      	str	r2, [r3, #0]
 274:	4770      	bx	lr
 276:	46c0      	nop			; (mov r8, r8)
 278:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_16:

0000027c <handler_ext_int_16>:
 27c:	4b02      	ldr	r3, [pc, #8]	; (288 <handler_ext_int_16+0xc>)
 27e:	2280      	movs	r2, #128	; 0x80
 280:	0252      	lsls	r2, r2, #9
 282:	601a      	str	r2, [r3, #0]
 284:	4770      	bx	lr
 286:	46c0      	nop			; (mov r8, r8)
 288:	e000e280 	.word	0xe000e280

Disassembly of section .text.initialization:

0000028c <initialization>:
 28c:	b508      	push	{r3, lr}
 28e:	4a09      	ldr	r2, [pc, #36]	; (2b4 <initialization+0x28>)
 290:	4b09      	ldr	r3, [pc, #36]	; (2b8 <initialization+0x2c>)
 292:	480a      	ldr	r0, [pc, #40]	; (2bc <initialization+0x30>)
 294:	490a      	ldr	r1, [pc, #40]	; (2c0 <initialization+0x34>)
 296:	601a      	str	r2, [r3, #0]
 298:	f7ff ff0e 	bl	b8 <write_regfile>
 29c:	4909      	ldr	r1, [pc, #36]	; (2c4 <initialization+0x38>)
 29e:	20aa      	movs	r0, #170	; 0xaa
 2a0:	f7ff ff3e 	bl	120 <mbus_write_message32>
 2a4:	f7ff ff1c 	bl	e0 <set_halt_until_mbus_rx>
 2a8:	2004      	movs	r0, #4
 2aa:	f7ff ff59 	bl	160 <mbus_enumerate>
 2ae:	f7ff ff27 	bl	100 <set_halt_until_mbus_tx>
 2b2:	bd08      	pop	{r3, pc}
 2b4:	deadbeef 	.word	0xdeadbeef
 2b8:	00000c2c 	.word	0x00000c2c
 2bc:	a0000020 	.word	0xa0000020
 2c0:	0000dead 	.word	0x0000dead
 2c4:	aaaaaaaa 	.word	0xaaaaaaaa

Disassembly of section .text.startup.main:

000002c8 <main>:
 2c8:	b508      	push	{r3, lr}
 2ca:	f7ff ff59 	bl	180 <init_interrupt>
 2ce:	4b43      	ldr	r3, [pc, #268]	; (3dc <main+0x114>)
 2d0:	681a      	ldr	r2, [r3, #0]
 2d2:	4b43      	ldr	r3, [pc, #268]	; (3e0 <main+0x118>)
 2d4:	429a      	cmp	r2, r3
 2d6:	d001      	beq.n	2dc <main+0x14>
 2d8:	f7ff ffd8 	bl	28c <initialization>
 2dc:	f7ff ff10 	bl	100 <set_halt_until_mbus_tx>
 2e0:	2139      	movs	r1, #57	; 0x39
 2e2:	2040      	movs	r0, #64	; 0x40
 2e4:	f7ff ff1c 	bl	120 <mbus_write_message32>
 2e8:	493e      	ldr	r1, [pc, #248]	; (3e4 <main+0x11c>)
 2ea:	2202      	movs	r2, #2
 2ec:	2042      	movs	r0, #66	; 0x42
 2ee:	f7ff ff1f 	bl	130 <mbus_write_message>
 2f2:	493d      	ldr	r1, [pc, #244]	; (3e8 <main+0x120>)
 2f4:	2202      	movs	r2, #2
 2f6:	2042      	movs	r0, #66	; 0x42
 2f8:	f7ff ff1a 	bl	130 <mbus_write_message>
 2fc:	493b      	ldr	r1, [pc, #236]	; (3ec <main+0x124>)
 2fe:	2202      	movs	r2, #2
 300:	2042      	movs	r0, #66	; 0x42
 302:	f7ff ff15 	bl	130 <mbus_write_message>
 306:	493a      	ldr	r1, [pc, #232]	; (3f0 <main+0x128>)
 308:	2202      	movs	r2, #2
 30a:	2042      	movs	r0, #66	; 0x42
 30c:	f7ff ff10 	bl	130 <mbus_write_message>
 310:	4938      	ldr	r1, [pc, #224]	; (3f4 <main+0x12c>)
 312:	2202      	movs	r2, #2
 314:	2042      	movs	r0, #66	; 0x42
 316:	f7ff ff0b 	bl	130 <mbus_write_message>
 31a:	4937      	ldr	r1, [pc, #220]	; (3f8 <main+0x130>)
 31c:	2202      	movs	r2, #2
 31e:	2042      	movs	r0, #66	; 0x42
 320:	f7ff ff06 	bl	130 <mbus_write_message>
 324:	4935      	ldr	r1, [pc, #212]	; (3fc <main+0x134>)
 326:	2202      	movs	r2, #2
 328:	2042      	movs	r0, #66	; 0x42
 32a:	f7ff ff01 	bl	130 <mbus_write_message>
 32e:	4934      	ldr	r1, [pc, #208]	; (400 <main+0x138>)
 330:	2203      	movs	r2, #3
 332:	2042      	movs	r0, #66	; 0x42
 334:	f7ff fefc 	bl	130 <mbus_write_message>
 338:	4932      	ldr	r1, [pc, #200]	; (404 <main+0x13c>)
 33a:	2203      	movs	r2, #3
 33c:	2042      	movs	r0, #66	; 0x42
 33e:	f7ff fef7 	bl	130 <mbus_write_message>
 342:	4931      	ldr	r1, [pc, #196]	; (408 <main+0x140>)
 344:	2202      	movs	r2, #2
 346:	2042      	movs	r0, #66	; 0x42
 348:	f7ff fef2 	bl	130 <mbus_write_message>
 34c:	492f      	ldr	r1, [pc, #188]	; (40c <main+0x144>)
 34e:	2202      	movs	r2, #2
 350:	2042      	movs	r0, #66	; 0x42
 352:	f7ff feed 	bl	130 <mbus_write_message>
 356:	492e      	ldr	r1, [pc, #184]	; (410 <main+0x148>)
 358:	2202      	movs	r2, #2
 35a:	2042      	movs	r0, #66	; 0x42
 35c:	f7ff fee8 	bl	130 <mbus_write_message>
 360:	492c      	ldr	r1, [pc, #176]	; (414 <main+0x14c>)
 362:	2202      	movs	r2, #2
 364:	2042      	movs	r0, #66	; 0x42
 366:	f7ff fee3 	bl	130 <mbus_write_message>
 36a:	492b      	ldr	r1, [pc, #172]	; (418 <main+0x150>)
 36c:	2202      	movs	r2, #2
 36e:	2042      	movs	r0, #66	; 0x42
 370:	f7ff fede 	bl	130 <mbus_write_message>
 374:	4929      	ldr	r1, [pc, #164]	; (41c <main+0x154>)
 376:	2202      	movs	r2, #2
 378:	2042      	movs	r0, #66	; 0x42
 37a:	f7ff fed9 	bl	130 <mbus_write_message>
 37e:	4928      	ldr	r1, [pc, #160]	; (420 <main+0x158>)
 380:	2202      	movs	r2, #2
 382:	2042      	movs	r0, #66	; 0x42
 384:	f7ff fed4 	bl	130 <mbus_write_message>
 388:	4926      	ldr	r1, [pc, #152]	; (424 <main+0x15c>)
 38a:	2202      	movs	r2, #2
 38c:	2042      	movs	r0, #66	; 0x42
 38e:	f7ff fecf 	bl	130 <mbus_write_message>
 392:	4925      	ldr	r1, [pc, #148]	; (428 <main+0x160>)
 394:	2202      	movs	r2, #2
 396:	2042      	movs	r0, #66	; 0x42
 398:	f7ff feca 	bl	130 <mbus_write_message>
 39c:	4923      	ldr	r1, [pc, #140]	; (42c <main+0x164>)
 39e:	2202      	movs	r2, #2
 3a0:	2042      	movs	r0, #66	; 0x42
 3a2:	f7ff fec5 	bl	130 <mbus_write_message>
 3a6:	4922      	ldr	r1, [pc, #136]	; (430 <main+0x168>)
 3a8:	4a22      	ldr	r2, [pc, #136]	; (434 <main+0x16c>)
 3aa:	2042      	movs	r0, #66	; 0x42
 3ac:	f7ff fec0 	bl	130 <mbus_write_message>
 3b0:	4921      	ldr	r1, [pc, #132]	; (438 <main+0x170>)
 3b2:	2202      	movs	r2, #2
 3b4:	2042      	movs	r0, #66	; 0x42
 3b6:	f7ff febb 	bl	130 <mbus_write_message>
 3ba:	4920      	ldr	r1, [pc, #128]	; (43c <main+0x174>)
 3bc:	2202      	movs	r2, #2
 3be:	2042      	movs	r0, #66	; 0x42
 3c0:	f7ff feb6 	bl	130 <mbus_write_message>
 3c4:	491e      	ldr	r1, [pc, #120]	; (440 <main+0x178>)
 3c6:	2202      	movs	r2, #2
 3c8:	2042      	movs	r0, #66	; 0x42
 3ca:	f7ff feb1 	bl	130 <mbus_write_message>
 3ce:	491d      	ldr	r1, [pc, #116]	; (444 <main+0x17c>)
 3d0:	20dd      	movs	r0, #221	; 0xdd
 3d2:	f7ff fea5 	bl	120 <mbus_write_message32>
 3d6:	f7ff fecd 	bl	174 <mbus_sleep_all>
 3da:	e7fe      	b.n	3da <main+0x112>
 3dc:	00000c2c 	.word	0x00000c2c
 3e0:	deadbeef 	.word	0xdeadbeef
 3e4:	00000c04 	.word	0x00000c04
 3e8:	00000c0c 	.word	0x00000c0c
 3ec:	00000c14 	.word	0x00000c14
 3f0:	00000c1c 	.word	0x00000c1c
 3f4:	00000c24 	.word	0x00000c24
 3f8:	00000be8 	.word	0x00000be8
 3fc:	00000bfc 	.word	0x00000bfc
 400:	00000bf0 	.word	0x00000bf0
 404:	00000bdc 	.word	0x00000bdc
 408:	00000448 	.word	0x00000448
 40c:	00000450 	.word	0x00000450
 410:	00000458 	.word	0x00000458
 414:	00000460 	.word	0x00000460
 418:	00000468 	.word	0x00000468
 41c:	00000470 	.word	0x00000470
 420:	00000ba4 	.word	0x00000ba4
 424:	00000bb4 	.word	0x00000bb4
 428:	00000bc4 	.word	0x00000bc4
 42c:	00000bcc 	.word	0x00000bcc
 430:	00000478 	.word	0x00000478
 434:	000001cb 	.word	0x000001cb
 438:	00000bac 	.word	0x00000bac
 43c:	00000bd4 	.word	0x00000bd4
 440:	00000bbc 	.word	0x00000bbc
 444:	0ea7f00d 	.word	0x0ea7f00d
