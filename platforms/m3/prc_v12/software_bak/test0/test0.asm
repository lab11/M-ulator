
test0/test0.elf:     file format elf32-littlearm


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
  40:	00000131 	.word	0x00000131
  44:	00000189 	.word	0x00000189
  48:	000001e5 	.word	0x000001e5
  4c:	0000020d 	.word	0x0000020d
  50:	00000235 	.word	0x00000235
  54:	0000025d 	.word	0x0000025d
  58:	00000285 	.word	0x00000285
  5c:	000002ad 	.word	0x000002ad
  60:	000002d5 	.word	0x000002d5
  64:	000002fd 	.word	0x000002fd
  68:	00000325 	.word	0x00000325
  6c:	0000034d 	.word	0x0000034d
  70:	00000375 	.word	0x00000375
  74:	0000039d 	.word	0x0000039d
  78:	000003c5 	.word	0x000003c5
  7c:	00000000 	.word	0x00000000

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 f9d6 	bl	440 <main>
  94:	e7fc      	b.n	90 <_start>

Disassembly of section .text.write_config_reg:

00000096 <write_config_reg>:
  96:	23a0      	movs	r3, #160	; 0xa0
  98:	061b      	lsls	r3, r3, #24
  9a:	0080      	lsls	r0, r0, #2
  9c:	4318      	orrs	r0, r3
  9e:	6001      	str	r1, [r0, #0]
  a0:	4770      	bx	lr

Disassembly of section .text.delay:

000000a2 <delay>:
  a2:	b500      	push	{lr}
  a4:	2300      	movs	r3, #0
  a6:	e001      	b.n	ac <delay+0xa>
  a8:	46c0      	nop			; (mov r8, r8)
  aa:	3301      	adds	r3, #1
  ac:	4283      	cmp	r3, r0
  ae:	d1fb      	bne.n	a8 <delay+0x6>
  b0:	bd00      	pop	{pc}

Disassembly of section .text.set_wakeup_timer:

000000b4 <set_wakeup_timer>:
  b4:	b500      	push	{lr}
  b6:	2900      	cmp	r1, #0
  b8:	d003      	beq.n	c2 <set_wakeup_timer+0xe>
  ba:	2380      	movs	r3, #128	; 0x80
  bc:	021b      	lsls	r3, r3, #8
  be:	4318      	orrs	r0, r3
  c0:	e001      	b.n	c6 <set_wakeup_timer+0x12>
  c2:	0440      	lsls	r0, r0, #17
  c4:	0c40      	lsrs	r0, r0, #17
  c6:	4b04      	ldr	r3, [pc, #16]	; (d8 <set_wakeup_timer+0x24>)
  c8:	6018      	str	r0, [r3, #0]
  ca:	2a00      	cmp	r2, #0
  cc:	d002      	beq.n	d4 <set_wakeup_timer+0x20>
  ce:	4b03      	ldr	r3, [pc, #12]	; (dc <set_wakeup_timer+0x28>)
  d0:	2201      	movs	r2, #1
  d2:	601a      	str	r2, [r3, #0]
  d4:	bd00      	pop	{pc}
  d6:	46c0      	nop			; (mov r8, r8)
  d8:	a0000034 	.word	0xa0000034
  dc:	a0001300 	.word	0xa0001300

Disassembly of section .text.set_halt_until_mbus_tx:

000000e0 <set_halt_until_mbus_tx>:
  e0:	4b05      	ldr	r3, [pc, #20]	; (f8 <set_halt_until_mbus_tx+0x18>)
  e2:	4a06      	ldr	r2, [pc, #24]	; (fc <set_halt_until_mbus_tx+0x1c>)
  e4:	6819      	ldr	r1, [r3, #0]
  e6:	400a      	ands	r2, r1
  e8:	21a0      	movs	r1, #160	; 0xa0
  ea:	0209      	lsls	r1, r1, #8
  ec:	430a      	orrs	r2, r1
  ee:	601a      	str	r2, [r3, #0]
  f0:	681a      	ldr	r2, [r3, #0]
  f2:	4b03      	ldr	r3, [pc, #12]	; (100 <set_halt_until_mbus_tx+0x20>)
  f4:	601a      	str	r2, [r3, #0]
  f6:	4770      	bx	lr
  f8:	000004d0 	.word	0x000004d0
  fc:	ffff0fff 	.word	0xffff0fff
 100:	a0000028 	.word	0xa0000028

Disassembly of section .text.mbus_write_message32:

00000104 <mbus_write_message32>:
 104:	4b02      	ldr	r3, [pc, #8]	; (110 <mbus_write_message32+0xc>)
 106:	0100      	lsls	r0, r0, #4
 108:	4318      	orrs	r0, r3
 10a:	6001      	str	r1, [r0, #0]
 10c:	2001      	movs	r0, #1
 10e:	4770      	bx	lr
 110:	a0003000 	.word	0xa0003000

Disassembly of section .text.init_interrupt:

00000114 <init_interrupt>:
 114:	4a03      	ldr	r2, [pc, #12]	; (124 <init_interrupt+0x10>)
 116:	4b04      	ldr	r3, [pc, #16]	; (128 <init_interrupt+0x14>)
 118:	601a      	str	r2, [r3, #0]
 11a:	4b04      	ldr	r3, [pc, #16]	; (12c <init_interrupt+0x18>)
 11c:	2200      	movs	r2, #0
 11e:	601a      	str	r2, [r3, #0]
 120:	4770      	bx	lr
 122:	46c0      	nop			; (mov r8, r8)
 124:	00007fff 	.word	0x00007fff
 128:	e000e280 	.word	0xe000e280
 12c:	e000e100 	.word	0xe000e100

Disassembly of section .text.handler_ext_int_0:

00000130 <handler_ext_int_0>:
 130:	4b10      	ldr	r3, [pc, #64]	; (174 <handler_ext_int_0+0x44>)
 132:	2201      	movs	r2, #1
 134:	b570      	push	{r4, r5, r6, lr}
 136:	601a      	str	r2, [r3, #0]
 138:	26a0      	movs	r6, #160	; 0xa0
 13a:	2380      	movs	r3, #128	; 0x80
 13c:	0636      	lsls	r6, r6, #24
 13e:	015b      	lsls	r3, r3, #5
 140:	6033      	str	r3, [r6, #0]
 142:	4b0d      	ldr	r3, [pc, #52]	; (178 <handler_ext_int_0+0x48>)
 144:	4d0d      	ldr	r5, [pc, #52]	; (17c <handler_ext_int_0+0x4c>)
 146:	681b      	ldr	r3, [r3, #0]
 148:	4c0d      	ldr	r4, [pc, #52]	; (180 <handler_ext_int_0+0x50>)
 14a:	602b      	str	r3, [r5, #0]
 14c:	4b0d      	ldr	r3, [pc, #52]	; (184 <handler_ext_int_0+0x54>)
 14e:	681a      	ldr	r2, [r3, #0]
 150:	6022      	str	r2, [r4, #0]
 152:	2200      	movs	r2, #0
 154:	601a      	str	r2, [r3, #0]
 156:	f7ff ffc3 	bl	e0 <set_halt_until_mbus_tx>
 15a:	6831      	ldr	r1, [r6, #0]
 15c:	20e0      	movs	r0, #224	; 0xe0
 15e:	f7ff ffd1 	bl	104 <mbus_write_message32>
 162:	6829      	ldr	r1, [r5, #0]
 164:	20e1      	movs	r0, #225	; 0xe1
 166:	f7ff ffcd 	bl	104 <mbus_write_message32>
 16a:	6821      	ldr	r1, [r4, #0]
 16c:	20e2      	movs	r0, #226	; 0xe2
 16e:	f7ff ffc9 	bl	104 <mbus_write_message32>
 172:	bd70      	pop	{r4, r5, r6, pc}
 174:	e000e280 	.word	0xe000e280
 178:	a000110c 	.word	0xa000110c
 17c:	a0000004 	.word	0xa0000004
 180:	a0000008 	.word	0xa0000008
 184:	a0001110 	.word	0xa0001110

Disassembly of section .text.handler_ext_int_1:

00000188 <handler_ext_int_1>:
 188:	4b10      	ldr	r3, [pc, #64]	; (1cc <handler_ext_int_1+0x44>)
 18a:	2202      	movs	r2, #2
 18c:	b570      	push	{r4, r5, r6, lr}
 18e:	601a      	str	r2, [r3, #0]
 190:	26a0      	movs	r6, #160	; 0xa0
 192:	4b0f      	ldr	r3, [pc, #60]	; (1d0 <handler_ext_int_1+0x48>)
 194:	0636      	lsls	r6, r6, #24
 196:	6033      	str	r3, [r6, #0]
 198:	4b0e      	ldr	r3, [pc, #56]	; (1d4 <handler_ext_int_1+0x4c>)
 19a:	4d0f      	ldr	r5, [pc, #60]	; (1d8 <handler_ext_int_1+0x50>)
 19c:	681b      	ldr	r3, [r3, #0]
 19e:	4c0f      	ldr	r4, [pc, #60]	; (1dc <handler_ext_int_1+0x54>)
 1a0:	602b      	str	r3, [r5, #0]
 1a2:	4b0f      	ldr	r3, [pc, #60]	; (1e0 <handler_ext_int_1+0x58>)
 1a4:	681a      	ldr	r2, [r3, #0]
 1a6:	6022      	str	r2, [r4, #0]
 1a8:	2200      	movs	r2, #0
 1aa:	601a      	str	r2, [r3, #0]
 1ac:	f7ff ff98 	bl	e0 <set_halt_until_mbus_tx>
 1b0:	6831      	ldr	r1, [r6, #0]
 1b2:	20e0      	movs	r0, #224	; 0xe0
 1b4:	f7ff ffa6 	bl	104 <mbus_write_message32>
 1b8:	6829      	ldr	r1, [r5, #0]
 1ba:	20e1      	movs	r0, #225	; 0xe1
 1bc:	f7ff ffa2 	bl	104 <mbus_write_message32>
 1c0:	6821      	ldr	r1, [r4, #0]
 1c2:	20e2      	movs	r0, #226	; 0xe2
 1c4:	f7ff ff9e 	bl	104 <mbus_write_message32>
 1c8:	bd70      	pop	{r4, r5, r6, pc}
 1ca:	46c0      	nop			; (mov r8, r8)
 1cc:	e000e280 	.word	0xe000e280
 1d0:	00001001 	.word	0x00001001
 1d4:	a0001020 	.word	0xa0001020
 1d8:	a0000004 	.word	0xa0000004
 1dc:	a0000008 	.word	0xa0000008
 1e0:	a0001024 	.word	0xa0001024

Disassembly of section .text.handler_ext_int_2:

000001e4 <handler_ext_int_2>:
 1e4:	b538      	push	{r3, r4, r5, lr}
 1e6:	4b07      	ldr	r3, [pc, #28]	; (204 <handler_ext_int_2+0x20>)
 1e8:	4c07      	ldr	r4, [pc, #28]	; (208 <handler_ext_int_2+0x24>)
 1ea:	2204      	movs	r2, #4
 1ec:	601a      	str	r2, [r3, #0]
 1ee:	23a0      	movs	r3, #160	; 0xa0
 1f0:	6825      	ldr	r5, [r4, #0]
 1f2:	021b      	lsls	r3, r3, #8
 1f4:	6023      	str	r3, [r4, #0]
 1f6:	2102      	movs	r1, #2
 1f8:	20ee      	movs	r0, #238	; 0xee
 1fa:	f7ff ff83 	bl	104 <mbus_write_message32>
 1fe:	6025      	str	r5, [r4, #0]
 200:	bd38      	pop	{r3, r4, r5, pc}
 202:	46c0      	nop			; (mov r8, r8)
 204:	e000e280 	.word	0xe000e280
 208:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_3:

0000020c <handler_ext_int_3>:
 20c:	b538      	push	{r3, r4, r5, lr}
 20e:	4b07      	ldr	r3, [pc, #28]	; (22c <handler_ext_int_3+0x20>)
 210:	2208      	movs	r2, #8
 212:	4c07      	ldr	r4, [pc, #28]	; (230 <handler_ext_int_3+0x24>)
 214:	601a      	str	r2, [r3, #0]
 216:	23a0      	movs	r3, #160	; 0xa0
 218:	6825      	ldr	r5, [r4, #0]
 21a:	4093      	lsls	r3, r2
 21c:	6023      	str	r3, [r4, #0]
 21e:	2103      	movs	r1, #3
 220:	20ee      	movs	r0, #238	; 0xee
 222:	f7ff ff6f 	bl	104 <mbus_write_message32>
 226:	6025      	str	r5, [r4, #0]
 228:	bd38      	pop	{r3, r4, r5, pc}
 22a:	46c0      	nop			; (mov r8, r8)
 22c:	e000e280 	.word	0xe000e280
 230:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_4:

00000234 <handler_ext_int_4>:
 234:	b538      	push	{r3, r4, r5, lr}
 236:	4b07      	ldr	r3, [pc, #28]	; (254 <handler_ext_int_4+0x20>)
 238:	4c07      	ldr	r4, [pc, #28]	; (258 <handler_ext_int_4+0x24>)
 23a:	2210      	movs	r2, #16
 23c:	601a      	str	r2, [r3, #0]
 23e:	23a0      	movs	r3, #160	; 0xa0
 240:	6825      	ldr	r5, [r4, #0]
 242:	021b      	lsls	r3, r3, #8
 244:	6023      	str	r3, [r4, #0]
 246:	2104      	movs	r1, #4
 248:	20ee      	movs	r0, #238	; 0xee
 24a:	f7ff ff5b 	bl	104 <mbus_write_message32>
 24e:	6025      	str	r5, [r4, #0]
 250:	bd38      	pop	{r3, r4, r5, pc}
 252:	46c0      	nop			; (mov r8, r8)
 254:	e000e280 	.word	0xe000e280
 258:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_5:

0000025c <handler_ext_int_5>:
 25c:	b538      	push	{r3, r4, r5, lr}
 25e:	4b07      	ldr	r3, [pc, #28]	; (27c <handler_ext_int_5+0x20>)
 260:	4c07      	ldr	r4, [pc, #28]	; (280 <handler_ext_int_5+0x24>)
 262:	2220      	movs	r2, #32
 264:	601a      	str	r2, [r3, #0]
 266:	23a0      	movs	r3, #160	; 0xa0
 268:	6825      	ldr	r5, [r4, #0]
 26a:	021b      	lsls	r3, r3, #8
 26c:	6023      	str	r3, [r4, #0]
 26e:	2105      	movs	r1, #5
 270:	20ee      	movs	r0, #238	; 0xee
 272:	f7ff ff47 	bl	104 <mbus_write_message32>
 276:	6025      	str	r5, [r4, #0]
 278:	bd38      	pop	{r3, r4, r5, pc}
 27a:	46c0      	nop			; (mov r8, r8)
 27c:	e000e280 	.word	0xe000e280
 280:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_6:

00000284 <handler_ext_int_6>:
 284:	b538      	push	{r3, r4, r5, lr}
 286:	4b07      	ldr	r3, [pc, #28]	; (2a4 <handler_ext_int_6+0x20>)
 288:	4c07      	ldr	r4, [pc, #28]	; (2a8 <handler_ext_int_6+0x24>)
 28a:	2240      	movs	r2, #64	; 0x40
 28c:	601a      	str	r2, [r3, #0]
 28e:	23a0      	movs	r3, #160	; 0xa0
 290:	6825      	ldr	r5, [r4, #0]
 292:	021b      	lsls	r3, r3, #8
 294:	6023      	str	r3, [r4, #0]
 296:	2106      	movs	r1, #6
 298:	20ee      	movs	r0, #238	; 0xee
 29a:	f7ff ff33 	bl	104 <mbus_write_message32>
 29e:	6025      	str	r5, [r4, #0]
 2a0:	bd38      	pop	{r3, r4, r5, pc}
 2a2:	46c0      	nop			; (mov r8, r8)
 2a4:	e000e280 	.word	0xe000e280
 2a8:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_7:

000002ac <handler_ext_int_7>:
 2ac:	b538      	push	{r3, r4, r5, lr}
 2ae:	4b07      	ldr	r3, [pc, #28]	; (2cc <handler_ext_int_7+0x20>)
 2b0:	4c07      	ldr	r4, [pc, #28]	; (2d0 <handler_ext_int_7+0x24>)
 2b2:	2280      	movs	r2, #128	; 0x80
 2b4:	601a      	str	r2, [r3, #0]
 2b6:	23a0      	movs	r3, #160	; 0xa0
 2b8:	6825      	ldr	r5, [r4, #0]
 2ba:	021b      	lsls	r3, r3, #8
 2bc:	6023      	str	r3, [r4, #0]
 2be:	2107      	movs	r1, #7
 2c0:	20ee      	movs	r0, #238	; 0xee
 2c2:	f7ff ff1f 	bl	104 <mbus_write_message32>
 2c6:	6025      	str	r5, [r4, #0]
 2c8:	bd38      	pop	{r3, r4, r5, pc}
 2ca:	46c0      	nop			; (mov r8, r8)
 2cc:	e000e280 	.word	0xe000e280
 2d0:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_8:

000002d4 <handler_ext_int_8>:
 2d4:	b538      	push	{r3, r4, r5, lr}
 2d6:	2280      	movs	r2, #128	; 0x80
 2d8:	4b06      	ldr	r3, [pc, #24]	; (2f4 <handler_ext_int_8+0x20>)
 2da:	4c07      	ldr	r4, [pc, #28]	; (2f8 <handler_ext_int_8+0x24>)
 2dc:	0052      	lsls	r2, r2, #1
 2de:	601a      	str	r2, [r3, #0]
 2e0:	23a0      	movs	r3, #160	; 0xa0
 2e2:	6825      	ldr	r5, [r4, #0]
 2e4:	021b      	lsls	r3, r3, #8
 2e6:	6023      	str	r3, [r4, #0]
 2e8:	2108      	movs	r1, #8
 2ea:	20ee      	movs	r0, #238	; 0xee
 2ec:	f7ff ff0a 	bl	104 <mbus_write_message32>
 2f0:	6025      	str	r5, [r4, #0]
 2f2:	bd38      	pop	{r3, r4, r5, pc}
 2f4:	e000e280 	.word	0xe000e280
 2f8:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_9:

000002fc <handler_ext_int_9>:
 2fc:	b538      	push	{r3, r4, r5, lr}
 2fe:	2280      	movs	r2, #128	; 0x80
 300:	4b06      	ldr	r3, [pc, #24]	; (31c <handler_ext_int_9+0x20>)
 302:	4c07      	ldr	r4, [pc, #28]	; (320 <handler_ext_int_9+0x24>)
 304:	0092      	lsls	r2, r2, #2
 306:	601a      	str	r2, [r3, #0]
 308:	23a0      	movs	r3, #160	; 0xa0
 30a:	6825      	ldr	r5, [r4, #0]
 30c:	021b      	lsls	r3, r3, #8
 30e:	6023      	str	r3, [r4, #0]
 310:	2109      	movs	r1, #9
 312:	20ee      	movs	r0, #238	; 0xee
 314:	f7ff fef6 	bl	104 <mbus_write_message32>
 318:	6025      	str	r5, [r4, #0]
 31a:	bd38      	pop	{r3, r4, r5, pc}
 31c:	e000e280 	.word	0xe000e280
 320:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_10:

00000324 <handler_ext_int_10>:
 324:	b538      	push	{r3, r4, r5, lr}
 326:	2280      	movs	r2, #128	; 0x80
 328:	4b06      	ldr	r3, [pc, #24]	; (344 <handler_ext_int_10+0x20>)
 32a:	4c07      	ldr	r4, [pc, #28]	; (348 <handler_ext_int_10+0x24>)
 32c:	00d2      	lsls	r2, r2, #3
 32e:	601a      	str	r2, [r3, #0]
 330:	23a0      	movs	r3, #160	; 0xa0
 332:	6825      	ldr	r5, [r4, #0]
 334:	021b      	lsls	r3, r3, #8
 336:	6023      	str	r3, [r4, #0]
 338:	2110      	movs	r1, #16
 33a:	20ee      	movs	r0, #238	; 0xee
 33c:	f7ff fee2 	bl	104 <mbus_write_message32>
 340:	6025      	str	r5, [r4, #0]
 342:	bd38      	pop	{r3, r4, r5, pc}
 344:	e000e280 	.word	0xe000e280
 348:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_11:

0000034c <handler_ext_int_11>:
 34c:	b538      	push	{r3, r4, r5, lr}
 34e:	2280      	movs	r2, #128	; 0x80
 350:	4b06      	ldr	r3, [pc, #24]	; (36c <handler_ext_int_11+0x20>)
 352:	4c07      	ldr	r4, [pc, #28]	; (370 <handler_ext_int_11+0x24>)
 354:	0112      	lsls	r2, r2, #4
 356:	601a      	str	r2, [r3, #0]
 358:	23a0      	movs	r3, #160	; 0xa0
 35a:	6825      	ldr	r5, [r4, #0]
 35c:	021b      	lsls	r3, r3, #8
 35e:	6023      	str	r3, [r4, #0]
 360:	2111      	movs	r1, #17
 362:	20ee      	movs	r0, #238	; 0xee
 364:	f7ff fece 	bl	104 <mbus_write_message32>
 368:	6025      	str	r5, [r4, #0]
 36a:	bd38      	pop	{r3, r4, r5, pc}
 36c:	e000e280 	.word	0xe000e280
 370:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_12:

00000374 <handler_ext_int_12>:
 374:	b538      	push	{r3, r4, r5, lr}
 376:	2280      	movs	r2, #128	; 0x80
 378:	4b06      	ldr	r3, [pc, #24]	; (394 <handler_ext_int_12+0x20>)
 37a:	4c07      	ldr	r4, [pc, #28]	; (398 <handler_ext_int_12+0x24>)
 37c:	0152      	lsls	r2, r2, #5
 37e:	601a      	str	r2, [r3, #0]
 380:	23a0      	movs	r3, #160	; 0xa0
 382:	6825      	ldr	r5, [r4, #0]
 384:	021b      	lsls	r3, r3, #8
 386:	6023      	str	r3, [r4, #0]
 388:	2112      	movs	r1, #18
 38a:	20ee      	movs	r0, #238	; 0xee
 38c:	f7ff feba 	bl	104 <mbus_write_message32>
 390:	6025      	str	r5, [r4, #0]
 392:	bd38      	pop	{r3, r4, r5, pc}
 394:	e000e280 	.word	0xe000e280
 398:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_13:

0000039c <handler_ext_int_13>:
 39c:	b538      	push	{r3, r4, r5, lr}
 39e:	2280      	movs	r2, #128	; 0x80
 3a0:	4b06      	ldr	r3, [pc, #24]	; (3bc <handler_ext_int_13+0x20>)
 3a2:	4c07      	ldr	r4, [pc, #28]	; (3c0 <handler_ext_int_13+0x24>)
 3a4:	0192      	lsls	r2, r2, #6
 3a6:	601a      	str	r2, [r3, #0]
 3a8:	23a0      	movs	r3, #160	; 0xa0
 3aa:	6825      	ldr	r5, [r4, #0]
 3ac:	021b      	lsls	r3, r3, #8
 3ae:	6023      	str	r3, [r4, #0]
 3b0:	2113      	movs	r1, #19
 3b2:	20ee      	movs	r0, #238	; 0xee
 3b4:	f7ff fea6 	bl	104 <mbus_write_message32>
 3b8:	6025      	str	r5, [r4, #0]
 3ba:	bd38      	pop	{r3, r4, r5, pc}
 3bc:	e000e280 	.word	0xe000e280
 3c0:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_14:

000003c4 <handler_ext_int_14>:
 3c4:	b538      	push	{r3, r4, r5, lr}
 3c6:	2280      	movs	r2, #128	; 0x80
 3c8:	4b06      	ldr	r3, [pc, #24]	; (3e4 <handler_ext_int_14+0x20>)
 3ca:	4c07      	ldr	r4, [pc, #28]	; (3e8 <handler_ext_int_14+0x24>)
 3cc:	01d2      	lsls	r2, r2, #7
 3ce:	601a      	str	r2, [r3, #0]
 3d0:	23a0      	movs	r3, #160	; 0xa0
 3d2:	6825      	ldr	r5, [r4, #0]
 3d4:	021b      	lsls	r3, r3, #8
 3d6:	6023      	str	r3, [r4, #0]
 3d8:	2114      	movs	r1, #20
 3da:	20ee      	movs	r0, #238	; 0xee
 3dc:	f7ff fe92 	bl	104 <mbus_write_message32>
 3e0:	6025      	str	r5, [r4, #0]
 3e2:	bd38      	pop	{r3, r4, r5, pc}
 3e4:	e000e280 	.word	0xe000e280
 3e8:	a0000028 	.word	0xa0000028

Disassembly of section .text.initialization:

000003ec <initialization>:
 3ec:	b508      	push	{r3, lr}
 3ee:	4a0d      	ldr	r2, [pc, #52]	; (424 <initialization+0x38>)
 3f0:	4b0d      	ldr	r3, [pc, #52]	; (428 <initialization+0x3c>)
 3f2:	2008      	movs	r0, #8
 3f4:	601a      	str	r2, [r3, #0]
 3f6:	4b0d      	ldr	r3, [pc, #52]	; (42c <initialization+0x40>)
 3f8:	2200      	movs	r2, #0
 3fa:	601a      	str	r2, [r3, #0]
 3fc:	4b0c      	ldr	r3, [pc, #48]	; (430 <initialization+0x44>)
 3fe:	881a      	ldrh	r2, [r3, #0]
 400:	4a0c      	ldr	r2, [pc, #48]	; (434 <initialization+0x48>)
 402:	801a      	strh	r2, [r3, #0]
 404:	6819      	ldr	r1, [r3, #0]
 406:	f7ff fe46 	bl	96 <write_config_reg>
 40a:	4b0b      	ldr	r3, [pc, #44]	; (438 <initialization+0x4c>)
 40c:	22c8      	movs	r2, #200	; 0xc8
 40e:	0252      	lsls	r2, r2, #9
 410:	601a      	str	r2, [r3, #0]
 412:	2190      	movs	r1, #144	; 0x90
 414:	4a09      	ldr	r2, [pc, #36]	; (43c <initialization+0x50>)
 416:	0589      	lsls	r1, r1, #22
 418:	6011      	str	r1, [r2, #0]
 41a:	22d0      	movs	r2, #208	; 0xd0
 41c:	0252      	lsls	r2, r2, #9
 41e:	601a      	str	r2, [r3, #0]
 420:	bd08      	pop	{r3, pc}
 422:	46c0      	nop			; (mov r8, r8)
 424:	deadbeef 	.word	0xdeadbeef
 428:	000004d4 	.word	0x000004d4
 42c:	000004d8 	.word	0x000004d8
 430:	000004dc 	.word	0x000004dc
 434:	ffffdead 	.word	0xffffdead
 438:	a0000028 	.word	0xa0000028
 43c:	a0003000 	.word	0xa0003000

Disassembly of section .text.startup.main:

00000440 <main>:
 440:	b510      	push	{r4, lr}
 442:	f7ff fe67 	bl	114 <init_interrupt>
 446:	4b19      	ldr	r3, [pc, #100]	; (4ac <main+0x6c>)
 448:	681a      	ldr	r2, [r3, #0]
 44a:	4b19      	ldr	r3, [pc, #100]	; (4b0 <main+0x70>)
 44c:	429a      	cmp	r2, r3
 44e:	d001      	beq.n	454 <main+0x14>
 450:	f7ff ffcc 	bl	3ec <initialization>
 454:	4c17      	ldr	r4, [pc, #92]	; (4b4 <main+0x74>)
 456:	20ef      	movs	r0, #239	; 0xef
 458:	6821      	ldr	r1, [r4, #0]
 45a:	f7ff fe53 	bl	104 <mbus_write_message32>
 45e:	4b16      	ldr	r3, [pc, #88]	; (4b8 <main+0x78>)
 460:	4a16      	ldr	r2, [pc, #88]	; (4bc <main+0x7c>)
 462:	601a      	str	r2, [r3, #0]
 464:	6823      	ldr	r3, [r4, #0]
 466:	2b00      	cmp	r3, #0
 468:	d104      	bne.n	474 <main+0x34>
 46a:	4915      	ldr	r1, [pc, #84]	; (4c0 <main+0x80>)
 46c:	20ef      	movs	r0, #239	; 0xef
 46e:	f7ff fe49 	bl	104 <mbus_write_message32>
 472:	e00e      	b.n	492 <main+0x52>
 474:	6823      	ldr	r3, [r4, #0]
 476:	20ef      	movs	r0, #239	; 0xef
 478:	2b01      	cmp	r3, #1
 47a:	d106      	bne.n	48a <main+0x4a>
 47c:	4911      	ldr	r1, [pc, #68]	; (4c4 <main+0x84>)
 47e:	f7ff fe41 	bl	104 <mbus_write_message32>
 482:	4811      	ldr	r0, [pc, #68]	; (4c8 <main+0x88>)
 484:	f7ff fe0d 	bl	a2 <delay>
 488:	e003      	b.n	492 <main+0x52>
 48a:	4910      	ldr	r1, [pc, #64]	; (4cc <main+0x8c>)
 48c:	f7ff fe3a 	bl	104 <mbus_write_message32>
 490:	e7fe      	b.n	490 <main+0x50>
 492:	6823      	ldr	r3, [r4, #0]
 494:	2101      	movs	r1, #1
 496:	3301      	adds	r3, #1
 498:	200a      	movs	r0, #10
 49a:	1c0a      	adds	r2, r1, #0
 49c:	6023      	str	r3, [r4, #0]
 49e:	f7ff fe09 	bl	b4 <set_wakeup_timer>
 4a2:	2001      	movs	r0, #1
 4a4:	2100      	movs	r1, #0
 4a6:	f7ff fe2d 	bl	104 <mbus_write_message32>
 4aa:	e7fe      	b.n	4aa <main+0x6a>
 4ac:	000004d4 	.word	0x000004d4
 4b0:	deadbeef 	.word	0xdeadbeef
 4b4:	000004d8 	.word	0x000004d8
 4b8:	a0003ef0 	.word	0xa0003ef0
 4bc:	cccccccc 	.word	0xcccccccc
 4c0:	aaaaaaaa 	.word	0xaaaaaaaa
 4c4:	bbbbbbbb 	.word	0xbbbbbbbb
 4c8:	00002710 	.word	0x00002710
 4cc:	0ea7f00d 	.word	0x0ea7f00d
