
bootloader/bootloader.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0x80>:
   0:	00002000 	.word	0x00002000
   4:	00000091 	.word	0x00000091
	...
  1c:	00000080 	.word	0x00000080
  20:	00000080 	.word	0x00000080
  24:	00000080 	.word	0x00000080
  28:	00000080 	.word	0x00000080
	...
  34:	00000080 	.word	0x00000080
	...
  40:	00000241 	.word	0x00000241
  44:	0000024d 	.word	0x0000024d
  48:	00000259 	.word	0x00000259
  4c:	00000265 	.word	0x00000265
  50:	00000271 	.word	0x00000271
  54:	0000027d 	.word	0x0000027d
  58:	00000289 	.word	0x00000289
  5c:	00000295 	.word	0x00000295
  60:	000002a1 	.word	0x000002a1
  64:	000002b1 	.word	0x000002b1
  68:	000002c1 	.word	0x000002c1
  6c:	000002d1 	.word	0x000002d1
  70:	000002e1 	.word	0x000002e1
  74:	000002f1 	.word	0x000002f1
  78:	00000301 	.word	0x00000301
  7c:	00000000 	.word	0x00000000

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 f964 	bl	35c <main>
  94:	e7fc      	b.n	90 <_start>

Disassembly of section .text.FLSv1L_turnOnFlash:

00000096 <FLSv1L_turnOnFlash>:
  96:	b508      	push	{r3, lr}
  98:	2111      	movs	r1, #17
  9a:	2203      	movs	r2, #3
  9c:	f000 f8b4 	bl	208 <mbus_remote_register_write>
  a0:	bd08      	pop	{r3, pc}

Disassembly of section .text.FLSv1L_turnOffFlash:

000000a2 <FLSv1L_turnOffFlash>:
  a2:	b508      	push	{r3, lr}
  a4:	2111      	movs	r1, #17
  a6:	2202      	movs	r2, #2
  a8:	f000 f8ae 	bl	208 <mbus_remote_register_write>
  ac:	bd08      	pop	{r3, pc}

Disassembly of section .text.FLSv1L_enableLargeCap:

000000ae <FLSv1L_enableLargeCap>:
  ae:	b508      	push	{r3, lr}
  b0:	2109      	movs	r1, #9
  b2:	223f      	movs	r2, #63	; 0x3f
  b4:	f000 f8a8 	bl	208 <mbus_remote_register_write>
  b8:	bd08      	pop	{r3, pc}

Disassembly of section .text.FLSv1L_disableLargeCap:

000000ba <FLSv1L_disableLargeCap>:
  ba:	b508      	push	{r3, lr}
  bc:	2109      	movs	r1, #9
  be:	2200      	movs	r2, #0
  c0:	f000 f8a2 	bl	208 <mbus_remote_register_write>
  c4:	bd08      	pop	{r3, pc}

Disassembly of section .text.FLSv1L_setIRQAddr:

000000c6 <FLSv1L_setIRQAddr>:
  c6:	b508      	push	{r3, lr}
  c8:	0209      	lsls	r1, r1, #8
  ca:	430a      	orrs	r2, r1
  cc:	210c      	movs	r1, #12
  ce:	f000 f89b 	bl	208 <mbus_remote_register_write>
  d2:	bd08      	pop	{r3, pc}

Disassembly of section .text.FLSv1L_setOptTune:

000000d4 <FLSv1L_setOptTune>:
  d4:	b510      	push	{r4, lr}
  d6:	2102      	movs	r1, #2
  d8:	1c04      	adds	r4, r0, #0
  da:	4a04      	ldr	r2, [pc, #16]	; (ec <FLSv1L_setOptTune+0x18>)
  dc:	f000 f894 	bl	208 <mbus_remote_register_write>
  e0:	1c20      	adds	r0, r4, #0
  e2:	210a      	movs	r1, #10
  e4:	4a02      	ldr	r2, [pc, #8]	; (f0 <FLSv1L_setOptTune+0x1c>)
  e6:	f000 f88f 	bl	208 <mbus_remote_register_write>
  ea:	bd10      	pop	{r4, pc}
  ec:	000500c0 	.word	0x000500c0
  f0:	0000023f 	.word	0x0000023f

Disassembly of section .text.FLSv1L_setSRAMStartAddr:

000000f4 <FLSv1L_setSRAMStartAddr>:
  f4:	b508      	push	{r3, lr}
  f6:	1c0a      	adds	r2, r1, #0
  f8:	2106      	movs	r1, #6
  fa:	f000 f885 	bl	208 <mbus_remote_register_write>
  fe:	bd08      	pop	{r3, pc}

Disassembly of section .text.FLSv1L_setFlashStartAddr:

00000100 <FLSv1L_setFlashStartAddr>:
 100:	b508      	push	{r3, lr}
 102:	1c0a      	adds	r2, r1, #0
 104:	2107      	movs	r1, #7
 106:	f000 f87f 	bl	208 <mbus_remote_register_write>
 10a:	bd08      	pop	{r3, pc}

Disassembly of section .text.FLSv1L_doCopySRAM2Flash:

0000010c <FLSv1L_doCopySRAM2Flash>:
 10c:	b508      	push	{r3, lr}
 10e:	4a03      	ldr	r2, [pc, #12]	; (11c <FLSv1L_doCopySRAM2Flash+0x10>)
 110:	0109      	lsls	r1, r1, #4
 112:	430a      	orrs	r2, r1
 114:	2105      	movs	r1, #5
 116:	f000 f877 	bl	208 <mbus_remote_register_write>
 11a:	bd08      	pop	{r3, pc}
 11c:	00008005 	.word	0x00008005

Disassembly of section .text.FLSv1L_doEraseFlash:

00000120 <FLSv1L_doEraseFlash>:
 120:	b508      	push	{r3, lr}
 122:	2105      	movs	r1, #5
 124:	4a01      	ldr	r2, [pc, #4]	; (12c <FLSv1L_doEraseFlash+0xc>)
 126:	f000 f86f 	bl	208 <mbus_remote_register_write>
 12a:	bd08      	pop	{r3, pc}
 12c:	00008009 	.word	0x00008009

Disassembly of section .text.delay:

00000130 <delay>:
 130:	b500      	push	{lr}
 132:	2300      	movs	r3, #0
 134:	e001      	b.n	13a <delay+0xa>
 136:	46c0      	nop			; (mov r8, r8)
 138:	3301      	adds	r3, #1
 13a:	4283      	cmp	r3, r0
 13c:	d1fb      	bne.n	136 <delay+0x6>
 13e:	bd00      	pop	{pc}

Disassembly of section .text.write_regfile:

00000140 <write_regfile>:
 140:	0880      	lsrs	r0, r0, #2
 142:	0209      	lsls	r1, r1, #8
 144:	b508      	push	{r3, lr}
 146:	0a09      	lsrs	r1, r1, #8
 148:	4b05      	ldr	r3, [pc, #20]	; (160 <write_regfile+0x20>)
 14a:	0600      	lsls	r0, r0, #24
 14c:	4308      	orrs	r0, r1
 14e:	6018      	str	r0, [r3, #0]
 150:	4b04      	ldr	r3, [pc, #16]	; (164 <write_regfile+0x24>)
 152:	2210      	movs	r2, #16
 154:	601a      	str	r2, [r3, #0]
 156:	200a      	movs	r0, #10
 158:	f7ff ffea 	bl	130 <delay>
 15c:	bd08      	pop	{r3, pc}
 15e:	46c0      	nop			; (mov r8, r8)
 160:	a0002000 	.word	0xa0002000
 164:	a000200c 	.word	0xa000200c

Disassembly of section .text.set_halt_until_mbus_rx:

00000168 <set_halt_until_mbus_rx>:
 168:	4805      	ldr	r0, [pc, #20]	; (180 <set_halt_until_mbus_rx+0x18>)
 16a:	b508      	push	{r3, lr}
 16c:	6801      	ldr	r1, [r0, #0]
 16e:	4b05      	ldr	r3, [pc, #20]	; (184 <set_halt_until_mbus_rx+0x1c>)
 170:	4019      	ands	r1, r3
 172:	2390      	movs	r3, #144	; 0x90
 174:	021b      	lsls	r3, r3, #8
 176:	4319      	orrs	r1, r3
 178:	f7ff ffe2 	bl	140 <write_regfile>
 17c:	bd08      	pop	{r3, pc}
 17e:	46c0      	nop			; (mov r8, r8)
 180:	a0000028 	.word	0xa0000028
 184:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.set_halt_until_mbus_tx:

00000188 <set_halt_until_mbus_tx>:
 188:	4805      	ldr	r0, [pc, #20]	; (1a0 <set_halt_until_mbus_tx+0x18>)
 18a:	b508      	push	{r3, lr}
 18c:	6801      	ldr	r1, [r0, #0]
 18e:	4b05      	ldr	r3, [pc, #20]	; (1a4 <set_halt_until_mbus_tx+0x1c>)
 190:	4019      	ands	r1, r3
 192:	23a0      	movs	r3, #160	; 0xa0
 194:	021b      	lsls	r3, r3, #8
 196:	4319      	orrs	r1, r3
 198:	f7ff ffd2 	bl	140 <write_regfile>
 19c:	bd08      	pop	{r3, pc}
 19e:	46c0      	nop			; (mov r8, r8)
 1a0:	a0000028 	.word	0xa0000028
 1a4:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.mbus_write_message32:

000001a8 <mbus_write_message32>:
 1a8:	4b02      	ldr	r3, [pc, #8]	; (1b4 <mbus_write_message32+0xc>)
 1aa:	0100      	lsls	r0, r0, #4
 1ac:	4318      	orrs	r0, r3
 1ae:	6001      	str	r1, [r0, #0]
 1b0:	2001      	movs	r0, #1
 1b2:	4770      	bx	lr
 1b4:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_write_message:

000001b8 <mbus_write_message>:
 1b8:	2300      	movs	r3, #0
 1ba:	b500      	push	{lr}
 1bc:	429a      	cmp	r2, r3
 1be:	d00a      	beq.n	1d6 <mbus_write_message+0x1e>
 1c0:	4b06      	ldr	r3, [pc, #24]	; (1dc <mbus_write_message+0x24>)
 1c2:	3a01      	subs	r2, #1
 1c4:	0600      	lsls	r0, r0, #24
 1c6:	4302      	orrs	r2, r0
 1c8:	601a      	str	r2, [r3, #0]
 1ca:	4b05      	ldr	r3, [pc, #20]	; (1e0 <mbus_write_message+0x28>)
 1cc:	2223      	movs	r2, #35	; 0x23
 1ce:	6019      	str	r1, [r3, #0]
 1d0:	4b04      	ldr	r3, [pc, #16]	; (1e4 <mbus_write_message+0x2c>)
 1d2:	601a      	str	r2, [r3, #0]
 1d4:	2301      	movs	r3, #1
 1d6:	1c18      	adds	r0, r3, #0
 1d8:	bd00      	pop	{pc}
 1da:	46c0      	nop			; (mov r8, r8)
 1dc:	a0002000 	.word	0xa0002000
 1e0:	a0002004 	.word	0xa0002004
 1e4:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_enumerate:

000001e8 <mbus_enumerate>:
 1e8:	0603      	lsls	r3, r0, #24
 1ea:	2080      	movs	r0, #128	; 0x80
 1ec:	0580      	lsls	r0, r0, #22
 1ee:	4318      	orrs	r0, r3
 1f0:	4b01      	ldr	r3, [pc, #4]	; (1f8 <mbus_enumerate+0x10>)
 1f2:	6018      	str	r0, [r3, #0]
 1f4:	4770      	bx	lr
 1f6:	46c0      	nop			; (mov r8, r8)
 1f8:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

000001fc <mbus_sleep_all>:
 1fc:	4b01      	ldr	r3, [pc, #4]	; (204 <mbus_sleep_all+0x8>)
 1fe:	2200      	movs	r2, #0
 200:	601a      	str	r2, [r3, #0]
 202:	4770      	bx	lr
 204:	a0003010 	.word	0xa0003010

Disassembly of section .text.mbus_remote_register_write:

00000208 <mbus_remote_register_write>:
 208:	b507      	push	{r0, r1, r2, lr}
 20a:	0212      	lsls	r2, r2, #8
 20c:	0a12      	lsrs	r2, r2, #8
 20e:	0609      	lsls	r1, r1, #24
 210:	4311      	orrs	r1, r2
 212:	0100      	lsls	r0, r0, #4
 214:	9101      	str	r1, [sp, #4]
 216:	b2c0      	uxtb	r0, r0
 218:	a901      	add	r1, sp, #4
 21a:	2201      	movs	r2, #1
 21c:	f7ff ffcc 	bl	1b8 <mbus_write_message>
 220:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.init_interrupt:

00000224 <init_interrupt>:
 224:	4a03      	ldr	r2, [pc, #12]	; (234 <init_interrupt+0x10>)
 226:	4b04      	ldr	r3, [pc, #16]	; (238 <init_interrupt+0x14>)
 228:	601a      	str	r2, [r3, #0]
 22a:	4b04      	ldr	r3, [pc, #16]	; (23c <init_interrupt+0x18>)
 22c:	2200      	movs	r2, #0
 22e:	601a      	str	r2, [r3, #0]
 230:	4770      	bx	lr
 232:	46c0      	nop			; (mov r8, r8)
 234:	00007fff 	.word	0x00007fff
 238:	e000e280 	.word	0xe000e280
 23c:	e000e100 	.word	0xe000e100

Disassembly of section .text.handler_ext_int_0:

00000240 <handler_ext_int_0>:
 240:	4b01      	ldr	r3, [pc, #4]	; (248 <handler_ext_int_0+0x8>)
 242:	2201      	movs	r2, #1
 244:	601a      	str	r2, [r3, #0]
 246:	4770      	bx	lr
 248:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

0000024c <handler_ext_int_1>:
 24c:	4b01      	ldr	r3, [pc, #4]	; (254 <handler_ext_int_1+0x8>)
 24e:	2202      	movs	r2, #2
 250:	601a      	str	r2, [r3, #0]
 252:	4770      	bx	lr
 254:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

00000258 <handler_ext_int_2>:
 258:	4b01      	ldr	r3, [pc, #4]	; (260 <handler_ext_int_2+0x8>)
 25a:	2204      	movs	r2, #4
 25c:	601a      	str	r2, [r3, #0]
 25e:	4770      	bx	lr
 260:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

00000264 <handler_ext_int_3>:
 264:	4b01      	ldr	r3, [pc, #4]	; (26c <handler_ext_int_3+0x8>)
 266:	2208      	movs	r2, #8
 268:	601a      	str	r2, [r3, #0]
 26a:	4770      	bx	lr
 26c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

00000270 <handler_ext_int_4>:
 270:	4b01      	ldr	r3, [pc, #4]	; (278 <handler_ext_int_4+0x8>)
 272:	2210      	movs	r2, #16
 274:	601a      	str	r2, [r3, #0]
 276:	4770      	bx	lr
 278:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

0000027c <handler_ext_int_5>:
 27c:	4b01      	ldr	r3, [pc, #4]	; (284 <handler_ext_int_5+0x8>)
 27e:	2220      	movs	r2, #32
 280:	601a      	str	r2, [r3, #0]
 282:	4770      	bx	lr
 284:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

00000288 <handler_ext_int_6>:
 288:	4b01      	ldr	r3, [pc, #4]	; (290 <handler_ext_int_6+0x8>)
 28a:	2240      	movs	r2, #64	; 0x40
 28c:	601a      	str	r2, [r3, #0]
 28e:	4770      	bx	lr
 290:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

00000294 <handler_ext_int_7>:
 294:	4b01      	ldr	r3, [pc, #4]	; (29c <handler_ext_int_7+0x8>)
 296:	2280      	movs	r2, #128	; 0x80
 298:	601a      	str	r2, [r3, #0]
 29a:	4770      	bx	lr
 29c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

000002a0 <handler_ext_int_8>:
 2a0:	4b02      	ldr	r3, [pc, #8]	; (2ac <handler_ext_int_8+0xc>)
 2a2:	2280      	movs	r2, #128	; 0x80
 2a4:	0052      	lsls	r2, r2, #1
 2a6:	601a      	str	r2, [r3, #0]
 2a8:	4770      	bx	lr
 2aa:	46c0      	nop			; (mov r8, r8)
 2ac:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

000002b0 <handler_ext_int_9>:
 2b0:	4b02      	ldr	r3, [pc, #8]	; (2bc <handler_ext_int_9+0xc>)
 2b2:	2280      	movs	r2, #128	; 0x80
 2b4:	0092      	lsls	r2, r2, #2
 2b6:	601a      	str	r2, [r3, #0]
 2b8:	4770      	bx	lr
 2ba:	46c0      	nop			; (mov r8, r8)
 2bc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

000002c0 <handler_ext_int_10>:
 2c0:	4b02      	ldr	r3, [pc, #8]	; (2cc <handler_ext_int_10+0xc>)
 2c2:	2280      	movs	r2, #128	; 0x80
 2c4:	00d2      	lsls	r2, r2, #3
 2c6:	601a      	str	r2, [r3, #0]
 2c8:	4770      	bx	lr
 2ca:	46c0      	nop			; (mov r8, r8)
 2cc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

000002d0 <handler_ext_int_11>:
 2d0:	4b02      	ldr	r3, [pc, #8]	; (2dc <handler_ext_int_11+0xc>)
 2d2:	2280      	movs	r2, #128	; 0x80
 2d4:	0112      	lsls	r2, r2, #4
 2d6:	601a      	str	r2, [r3, #0]
 2d8:	4770      	bx	lr
 2da:	46c0      	nop			; (mov r8, r8)
 2dc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

000002e0 <handler_ext_int_12>:
 2e0:	4b02      	ldr	r3, [pc, #8]	; (2ec <handler_ext_int_12+0xc>)
 2e2:	2280      	movs	r2, #128	; 0x80
 2e4:	0152      	lsls	r2, r2, #5
 2e6:	601a      	str	r2, [r3, #0]
 2e8:	4770      	bx	lr
 2ea:	46c0      	nop			; (mov r8, r8)
 2ec:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

000002f0 <handler_ext_int_13>:
 2f0:	4b02      	ldr	r3, [pc, #8]	; (2fc <handler_ext_int_13+0xc>)
 2f2:	2280      	movs	r2, #128	; 0x80
 2f4:	0192      	lsls	r2, r2, #6
 2f6:	601a      	str	r2, [r3, #0]
 2f8:	4770      	bx	lr
 2fa:	46c0      	nop			; (mov r8, r8)
 2fc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

00000300 <handler_ext_int_14>:
 300:	4b02      	ldr	r3, [pc, #8]	; (30c <handler_ext_int_14+0xc>)
 302:	2280      	movs	r2, #128	; 0x80
 304:	01d2      	lsls	r2, r2, #7
 306:	601a      	str	r2, [r3, #0]
 308:	4770      	bx	lr
 30a:	46c0      	nop			; (mov r8, r8)
 30c:	e000e280 	.word	0xe000e280

Disassembly of section .text.initialization:

00000310 <initialization>:
 310:	b508      	push	{r3, lr}
 312:	4a0d      	ldr	r2, [pc, #52]	; (348 <initialization+0x38>)
 314:	4b0d      	ldr	r3, [pc, #52]	; (34c <initialization+0x3c>)
 316:	480e      	ldr	r0, [pc, #56]	; (350 <initialization+0x40>)
 318:	490e      	ldr	r1, [pc, #56]	; (354 <initialization+0x44>)
 31a:	601a      	str	r2, [r3, #0]
 31c:	f7ff ff10 	bl	140 <write_regfile>
 320:	490d      	ldr	r1, [pc, #52]	; (358 <initialization+0x48>)
 322:	20aa      	movs	r0, #170	; 0xaa
 324:	f7ff ff40 	bl	1a8 <mbus_write_message32>
 328:	f7ff ff1e 	bl	168 <set_halt_until_mbus_rx>
 32c:	2004      	movs	r0, #4
 32e:	f7ff ff5b 	bl	1e8 <mbus_enumerate>
 332:	f7ff ff29 	bl	188 <set_halt_until_mbus_tx>
 336:	2004      	movs	r0, #4
 338:	f7ff fecc 	bl	d4 <FLSv1L_setOptTune>
 33c:	2004      	movs	r0, #4
 33e:	2110      	movs	r1, #16
 340:	2200      	movs	r2, #0
 342:	f7ff fec0 	bl	c6 <FLSv1L_setIRQAddr>
 346:	bd08      	pop	{r3, pc}
 348:	deadbeef 	.word	0xdeadbeef
 34c:	00000814 	.word	0x00000814
 350:	a0000020 	.word	0xa0000020
 354:	0000dead 	.word	0x0000dead
 358:	aaaaaaaa 	.word	0xaaaaaaaa

Disassembly of section .text.startup.main:

0000035c <main>:
 35c:	b510      	push	{r4, lr}
 35e:	f7ff ff61 	bl	224 <init_interrupt>
 362:	4b42      	ldr	r3, [pc, #264]	; (46c <main+0x110>)
 364:	681a      	ldr	r2, [r3, #0]
 366:	4b42      	ldr	r3, [pc, #264]	; (470 <main+0x114>)
 368:	429a      	cmp	r2, r3
 36a:	d001      	beq.n	370 <main+0x14>
 36c:	f7ff ffd0 	bl	310 <initialization>
 370:	4940      	ldr	r1, [pc, #256]	; (474 <main+0x118>)
 372:	22e5      	movs	r2, #229	; 0xe5
 374:	2044      	movs	r0, #68	; 0x44
 376:	24a0      	movs	r4, #160	; 0xa0
 378:	f7ff ff1e 	bl	1b8 <mbus_write_message>
 37c:	0624      	lsls	r4, r4, #24
 37e:	f7ff fef3 	bl	168 <set_halt_until_mbus_rx>
 382:	2004      	movs	r0, #4
 384:	f7ff fe87 	bl	96 <FLSv1L_turnOnFlash>
 388:	6823      	ldr	r3, [r4, #0]
 38a:	2b03      	cmp	r3, #3
 38c:	d00c      	beq.n	3a8 <main+0x4c>
 38e:	f7ff fefb 	bl	188 <set_halt_until_mbus_tx>
 392:	4937      	ldr	r1, [pc, #220]	; (470 <main+0x114>)
 394:	20e0      	movs	r0, #224	; 0xe0
 396:	f7ff ff07 	bl	1a8 <mbus_write_message32>
 39a:	6821      	ldr	r1, [r4, #0]
 39c:	20e0      	movs	r0, #224	; 0xe0
 39e:	f7ff ff03 	bl	1a8 <mbus_write_message32>
 3a2:	f7ff ff2b 	bl	1fc <mbus_sleep_all>
 3a6:	e7fe      	b.n	3a6 <main+0x4a>
 3a8:	f7ff feee 	bl	188 <set_halt_until_mbus_tx>
 3ac:	2004      	movs	r0, #4
 3ae:	f7ff fe7e 	bl	ae <FLSv1L_enableLargeCap>
 3b2:	f7ff fee9 	bl	188 <set_halt_until_mbus_tx>
 3b6:	2004      	movs	r0, #4
 3b8:	2100      	movs	r1, #0
 3ba:	f7ff fea1 	bl	100 <FLSv1L_setFlashStartAddr>
 3be:	f7ff fed3 	bl	168 <set_halt_until_mbus_rx>
 3c2:	2004      	movs	r0, #4
 3c4:	f7ff feac 	bl	120 <FLSv1L_doEraseFlash>
 3c8:	6823      	ldr	r3, [r4, #0]
 3ca:	2b74      	cmp	r3, #116	; 0x74
 3cc:	d00c      	beq.n	3e8 <main+0x8c>
 3ce:	f7ff fedb 	bl	188 <set_halt_until_mbus_tx>
 3d2:	4927      	ldr	r1, [pc, #156]	; (470 <main+0x114>)
 3d4:	20e1      	movs	r0, #225	; 0xe1
 3d6:	f7ff fee7 	bl	1a8 <mbus_write_message32>
 3da:	6821      	ldr	r1, [r4, #0]
 3dc:	20e1      	movs	r0, #225	; 0xe1
 3de:	f7ff fee3 	bl	1a8 <mbus_write_message32>
 3e2:	f7ff ff0b 	bl	1fc <mbus_sleep_all>
 3e6:	e7fe      	b.n	3e6 <main+0x8a>
 3e8:	f7ff fece 	bl	188 <set_halt_until_mbus_tx>
 3ec:	2004      	movs	r0, #4
 3ee:	2100      	movs	r1, #0
 3f0:	f7ff fe86 	bl	100 <FLSv1L_setFlashStartAddr>
 3f4:	2004      	movs	r0, #4
 3f6:	2100      	movs	r1, #0
 3f8:	f7ff fe7c 	bl	f4 <FLSv1L_setSRAMStartAddr>
 3fc:	f7ff feb4 	bl	168 <set_halt_until_mbus_rx>
 400:	2004      	movs	r0, #4
 402:	491d      	ldr	r1, [pc, #116]	; (478 <main+0x11c>)
 404:	f7ff fe82 	bl	10c <FLSv1L_doCopySRAM2Flash>
 408:	6823      	ldr	r3, [r4, #0]
 40a:	2b5c      	cmp	r3, #92	; 0x5c
 40c:	d00c      	beq.n	428 <main+0xcc>
 40e:	f7ff febb 	bl	188 <set_halt_until_mbus_tx>
 412:	4917      	ldr	r1, [pc, #92]	; (470 <main+0x114>)
 414:	20e2      	movs	r0, #226	; 0xe2
 416:	f7ff fec7 	bl	1a8 <mbus_write_message32>
 41a:	6821      	ldr	r1, [r4, #0]
 41c:	20e2      	movs	r0, #226	; 0xe2
 41e:	f7ff fec3 	bl	1a8 <mbus_write_message32>
 422:	f7ff feeb 	bl	1fc <mbus_sleep_all>
 426:	e7fe      	b.n	426 <main+0xca>
 428:	f7ff fe9e 	bl	168 <set_halt_until_mbus_rx>
 42c:	2004      	movs	r0, #4
 42e:	f7ff fe38 	bl	a2 <FLSv1L_turnOffFlash>
 432:	6823      	ldr	r3, [r4, #0]
 434:	2b06      	cmp	r3, #6
 436:	d00c      	beq.n	452 <main+0xf6>
 438:	f7ff fea6 	bl	188 <set_halt_until_mbus_tx>
 43c:	490c      	ldr	r1, [pc, #48]	; (470 <main+0x114>)
 43e:	20e3      	movs	r0, #227	; 0xe3
 440:	f7ff feb2 	bl	1a8 <mbus_write_message32>
 444:	6821      	ldr	r1, [r4, #0]
 446:	20e3      	movs	r0, #227	; 0xe3
 448:	f7ff feae 	bl	1a8 <mbus_write_message32>
 44c:	f7ff fed6 	bl	1fc <mbus_sleep_all>
 450:	e7fe      	b.n	450 <main+0xf4>
 452:	f7ff fe99 	bl	188 <set_halt_until_mbus_tx>
 456:	2004      	movs	r0, #4
 458:	f7ff fe2f 	bl	ba <FLSv1L_disableLargeCap>
 45c:	4907      	ldr	r1, [pc, #28]	; (47c <main+0x120>)
 45e:	20dd      	movs	r0, #221	; 0xdd
 460:	f7ff fea2 	bl	1a8 <mbus_write_message32>
 464:	f7ff feca 	bl	1fc <mbus_sleep_all>
 468:	e7fe      	b.n	468 <main+0x10c>
 46a:	46c0      	nop			; (mov r8, r8)
 46c:	00000814 	.word	0x00000814
 470:	deadbeef 	.word	0xdeadbeef
 474:	00000480 	.word	0x00000480
 478:	000007fe 	.word	0x000007fe
 47c:	0ea7f00d 	.word	0x0ea7f00d
