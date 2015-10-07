
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
  40:	0000011d 	.word	0x0000011d
  44:	00000175 	.word	0x00000175
  48:	000001d1 	.word	0x000001d1
  4c:	000001f9 	.word	0x000001f9
  50:	00000221 	.word	0x00000221
  54:	00000249 	.word	0x00000249
  58:	00000271 	.word	0x00000271
  5c:	00000299 	.word	0x00000299
  60:	000002c1 	.word	0x000002c1
  64:	000002e9 	.word	0x000002e9
  68:	00000311 	.word	0x00000311
  6c:	00000339 	.word	0x00000339
  70:	00000361 	.word	0x00000361
  74:	00000389 	.word	0x00000389
  78:	000003b1 	.word	0x000003b1
  7c:	00000000 	.word	0x00000000

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 fb52 	bl	738 <main>
  94:	e7fc      	b.n	90 <_start>

Disassembly of section .text.delay:

00000096 <delay>:
  96:	b500      	push	{lr}
  98:	2300      	movs	r3, #0
  9a:	e001      	b.n	a0 <delay+0xa>
  9c:	46c0      	nop			; (mov r8, r8)
  9e:	3301      	adds	r3, #1
  a0:	4283      	cmp	r3, r0
  a2:	d1fb      	bne.n	9c <delay+0x6>
  a4:	bd00      	pop	{pc}

Disassembly of section .text.set_wakeup_timer:

000000a8 <set_wakeup_timer>:
  a8:	b500      	push	{lr}
  aa:	2900      	cmp	r1, #0
  ac:	d003      	beq.n	b6 <set_wakeup_timer+0xe>
  ae:	2380      	movs	r3, #128	; 0x80
  b0:	021b      	lsls	r3, r3, #8
  b2:	4318      	orrs	r0, r3
  b4:	e001      	b.n	ba <set_wakeup_timer+0x12>
  b6:	0440      	lsls	r0, r0, #17
  b8:	0c40      	lsrs	r0, r0, #17
  ba:	4b04      	ldr	r3, [pc, #16]	; (cc <set_wakeup_timer+0x24>)
  bc:	6018      	str	r0, [r3, #0]
  be:	2a00      	cmp	r2, #0
  c0:	d002      	beq.n	c8 <set_wakeup_timer+0x20>
  c2:	4b03      	ldr	r3, [pc, #12]	; (d0 <set_wakeup_timer+0x28>)
  c4:	2201      	movs	r2, #1
  c6:	601a      	str	r2, [r3, #0]
  c8:	bd00      	pop	{pc}
  ca:	46c0      	nop			; (mov r8, r8)
  cc:	a0000034 	.word	0xa0000034
  d0:	a0001300 	.word	0xa0001300

Disassembly of section .text.set_halt_until_mbus_tx:

000000d4 <set_halt_until_mbus_tx>:
  d4:	4b04      	ldr	r3, [pc, #16]	; (e8 <set_halt_until_mbus_tx+0x14>)
  d6:	4a05      	ldr	r2, [pc, #20]	; (ec <set_halt_until_mbus_tx+0x18>)
  d8:	6819      	ldr	r1, [r3, #0]
  da:	400a      	ands	r2, r1
  dc:	21a0      	movs	r1, #160	; 0xa0
  de:	0209      	lsls	r1, r1, #8
  e0:	430a      	orrs	r2, r1
  e2:	601a      	str	r2, [r3, #0]
  e4:	4770      	bx	lr
  e6:	46c0      	nop			; (mov r8, r8)
  e8:	a0000028 	.word	0xa0000028
  ec:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.mbus_write_message32:

000000f0 <mbus_write_message32>:
  f0:	4b02      	ldr	r3, [pc, #8]	; (fc <mbus_write_message32+0xc>)
  f2:	0100      	lsls	r0, r0, #4
  f4:	4318      	orrs	r0, r3
  f6:	6001      	str	r1, [r0, #0]
  f8:	2001      	movs	r0, #1
  fa:	4770      	bx	lr
  fc:	a0003000 	.word	0xa0003000

Disassembly of section .text.init_interrupt:

00000100 <init_interrupt>:
 100:	4a03      	ldr	r2, [pc, #12]	; (110 <init_interrupt+0x10>)
 102:	4b04      	ldr	r3, [pc, #16]	; (114 <init_interrupt+0x14>)
 104:	601a      	str	r2, [r3, #0]
 106:	4b04      	ldr	r3, [pc, #16]	; (118 <init_interrupt+0x18>)
 108:	2200      	movs	r2, #0
 10a:	601a      	str	r2, [r3, #0]
 10c:	4770      	bx	lr
 10e:	46c0      	nop			; (mov r8, r8)
 110:	00007fff 	.word	0x00007fff
 114:	e000e280 	.word	0xe000e280
 118:	e000e100 	.word	0xe000e100

Disassembly of section .text.handler_ext_int_0:

0000011c <handler_ext_int_0>:
 11c:	4b10      	ldr	r3, [pc, #64]	; (160 <handler_ext_int_0+0x44>)
 11e:	2201      	movs	r2, #1
 120:	b570      	push	{r4, r5, r6, lr}
 122:	601a      	str	r2, [r3, #0]
 124:	26a0      	movs	r6, #160	; 0xa0
 126:	2380      	movs	r3, #128	; 0x80
 128:	0636      	lsls	r6, r6, #24
 12a:	015b      	lsls	r3, r3, #5
 12c:	6033      	str	r3, [r6, #0]
 12e:	4b0d      	ldr	r3, [pc, #52]	; (164 <handler_ext_int_0+0x48>)
 130:	4d0d      	ldr	r5, [pc, #52]	; (168 <handler_ext_int_0+0x4c>)
 132:	681b      	ldr	r3, [r3, #0]
 134:	4c0d      	ldr	r4, [pc, #52]	; (16c <handler_ext_int_0+0x50>)
 136:	602b      	str	r3, [r5, #0]
 138:	4b0d      	ldr	r3, [pc, #52]	; (170 <handler_ext_int_0+0x54>)
 13a:	681a      	ldr	r2, [r3, #0]
 13c:	6022      	str	r2, [r4, #0]
 13e:	2200      	movs	r2, #0
 140:	601a      	str	r2, [r3, #0]
 142:	f7ff ffc7 	bl	d4 <set_halt_until_mbus_tx>
 146:	6831      	ldr	r1, [r6, #0]
 148:	20e0      	movs	r0, #224	; 0xe0
 14a:	f7ff ffd1 	bl	f0 <mbus_write_message32>
 14e:	6829      	ldr	r1, [r5, #0]
 150:	20e1      	movs	r0, #225	; 0xe1
 152:	f7ff ffcd 	bl	f0 <mbus_write_message32>
 156:	6821      	ldr	r1, [r4, #0]
 158:	20e2      	movs	r0, #226	; 0xe2
 15a:	f7ff ffc9 	bl	f0 <mbus_write_message32>
 15e:	bd70      	pop	{r4, r5, r6, pc}
 160:	e000e280 	.word	0xe000e280
 164:	a000110c 	.word	0xa000110c
 168:	a0000004 	.word	0xa0000004
 16c:	a0000008 	.word	0xa0000008
 170:	a0001110 	.word	0xa0001110

Disassembly of section .text.handler_ext_int_1:

00000174 <handler_ext_int_1>:
 174:	4b10      	ldr	r3, [pc, #64]	; (1b8 <handler_ext_int_1+0x44>)
 176:	2202      	movs	r2, #2
 178:	b570      	push	{r4, r5, r6, lr}
 17a:	601a      	str	r2, [r3, #0]
 17c:	26a0      	movs	r6, #160	; 0xa0
 17e:	4b0f      	ldr	r3, [pc, #60]	; (1bc <handler_ext_int_1+0x48>)
 180:	0636      	lsls	r6, r6, #24
 182:	6033      	str	r3, [r6, #0]
 184:	4b0e      	ldr	r3, [pc, #56]	; (1c0 <handler_ext_int_1+0x4c>)
 186:	4d0f      	ldr	r5, [pc, #60]	; (1c4 <handler_ext_int_1+0x50>)
 188:	681b      	ldr	r3, [r3, #0]
 18a:	4c0f      	ldr	r4, [pc, #60]	; (1c8 <handler_ext_int_1+0x54>)
 18c:	602b      	str	r3, [r5, #0]
 18e:	4b0f      	ldr	r3, [pc, #60]	; (1cc <handler_ext_int_1+0x58>)
 190:	681a      	ldr	r2, [r3, #0]
 192:	6022      	str	r2, [r4, #0]
 194:	2200      	movs	r2, #0
 196:	601a      	str	r2, [r3, #0]
 198:	f7ff ff9c 	bl	d4 <set_halt_until_mbus_tx>
 19c:	6831      	ldr	r1, [r6, #0]
 19e:	20e0      	movs	r0, #224	; 0xe0
 1a0:	f7ff ffa6 	bl	f0 <mbus_write_message32>
 1a4:	6829      	ldr	r1, [r5, #0]
 1a6:	20e1      	movs	r0, #225	; 0xe1
 1a8:	f7ff ffa2 	bl	f0 <mbus_write_message32>
 1ac:	6821      	ldr	r1, [r4, #0]
 1ae:	20e2      	movs	r0, #226	; 0xe2
 1b0:	f7ff ff9e 	bl	f0 <mbus_write_message32>
 1b4:	bd70      	pop	{r4, r5, r6, pc}
 1b6:	46c0      	nop			; (mov r8, r8)
 1b8:	e000e280 	.word	0xe000e280
 1bc:	00001001 	.word	0x00001001
 1c0:	a0001020 	.word	0xa0001020
 1c4:	a0000004 	.word	0xa0000004
 1c8:	a0000008 	.word	0xa0000008
 1cc:	a0001024 	.word	0xa0001024

Disassembly of section .text.handler_ext_int_2:

000001d0 <handler_ext_int_2>:
 1d0:	b538      	push	{r3, r4, r5, lr}
 1d2:	4b07      	ldr	r3, [pc, #28]	; (1f0 <handler_ext_int_2+0x20>)
 1d4:	4c07      	ldr	r4, [pc, #28]	; (1f4 <handler_ext_int_2+0x24>)
 1d6:	2204      	movs	r2, #4
 1d8:	601a      	str	r2, [r3, #0]
 1da:	23a0      	movs	r3, #160	; 0xa0
 1dc:	6825      	ldr	r5, [r4, #0]
 1de:	021b      	lsls	r3, r3, #8
 1e0:	6023      	str	r3, [r4, #0]
 1e2:	2102      	movs	r1, #2
 1e4:	20ee      	movs	r0, #238	; 0xee
 1e6:	f7ff ff83 	bl	f0 <mbus_write_message32>
 1ea:	6025      	str	r5, [r4, #0]
 1ec:	bd38      	pop	{r3, r4, r5, pc}
 1ee:	46c0      	nop			; (mov r8, r8)
 1f0:	e000e280 	.word	0xe000e280
 1f4:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_3:

000001f8 <handler_ext_int_3>:
 1f8:	b538      	push	{r3, r4, r5, lr}
 1fa:	4b07      	ldr	r3, [pc, #28]	; (218 <handler_ext_int_3+0x20>)
 1fc:	2208      	movs	r2, #8
 1fe:	4c07      	ldr	r4, [pc, #28]	; (21c <handler_ext_int_3+0x24>)
 200:	601a      	str	r2, [r3, #0]
 202:	23a0      	movs	r3, #160	; 0xa0
 204:	6825      	ldr	r5, [r4, #0]
 206:	4093      	lsls	r3, r2
 208:	6023      	str	r3, [r4, #0]
 20a:	2103      	movs	r1, #3
 20c:	20ee      	movs	r0, #238	; 0xee
 20e:	f7ff ff6f 	bl	f0 <mbus_write_message32>
 212:	6025      	str	r5, [r4, #0]
 214:	bd38      	pop	{r3, r4, r5, pc}
 216:	46c0      	nop			; (mov r8, r8)
 218:	e000e280 	.word	0xe000e280
 21c:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_4:

00000220 <handler_ext_int_4>:
 220:	b538      	push	{r3, r4, r5, lr}
 222:	4b07      	ldr	r3, [pc, #28]	; (240 <handler_ext_int_4+0x20>)
 224:	4c07      	ldr	r4, [pc, #28]	; (244 <handler_ext_int_4+0x24>)
 226:	2210      	movs	r2, #16
 228:	601a      	str	r2, [r3, #0]
 22a:	23a0      	movs	r3, #160	; 0xa0
 22c:	6825      	ldr	r5, [r4, #0]
 22e:	021b      	lsls	r3, r3, #8
 230:	6023      	str	r3, [r4, #0]
 232:	2104      	movs	r1, #4
 234:	20ee      	movs	r0, #238	; 0xee
 236:	f7ff ff5b 	bl	f0 <mbus_write_message32>
 23a:	6025      	str	r5, [r4, #0]
 23c:	bd38      	pop	{r3, r4, r5, pc}
 23e:	46c0      	nop			; (mov r8, r8)
 240:	e000e280 	.word	0xe000e280
 244:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_5:

00000248 <handler_ext_int_5>:
 248:	b538      	push	{r3, r4, r5, lr}
 24a:	4b07      	ldr	r3, [pc, #28]	; (268 <handler_ext_int_5+0x20>)
 24c:	4c07      	ldr	r4, [pc, #28]	; (26c <handler_ext_int_5+0x24>)
 24e:	2220      	movs	r2, #32
 250:	601a      	str	r2, [r3, #0]
 252:	23a0      	movs	r3, #160	; 0xa0
 254:	6825      	ldr	r5, [r4, #0]
 256:	021b      	lsls	r3, r3, #8
 258:	6023      	str	r3, [r4, #0]
 25a:	2105      	movs	r1, #5
 25c:	20ee      	movs	r0, #238	; 0xee
 25e:	f7ff ff47 	bl	f0 <mbus_write_message32>
 262:	6025      	str	r5, [r4, #0]
 264:	bd38      	pop	{r3, r4, r5, pc}
 266:	46c0      	nop			; (mov r8, r8)
 268:	e000e280 	.word	0xe000e280
 26c:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_6:

00000270 <handler_ext_int_6>:
 270:	b538      	push	{r3, r4, r5, lr}
 272:	4b07      	ldr	r3, [pc, #28]	; (290 <handler_ext_int_6+0x20>)
 274:	4c07      	ldr	r4, [pc, #28]	; (294 <handler_ext_int_6+0x24>)
 276:	2240      	movs	r2, #64	; 0x40
 278:	601a      	str	r2, [r3, #0]
 27a:	23a0      	movs	r3, #160	; 0xa0
 27c:	6825      	ldr	r5, [r4, #0]
 27e:	021b      	lsls	r3, r3, #8
 280:	6023      	str	r3, [r4, #0]
 282:	2106      	movs	r1, #6
 284:	20ee      	movs	r0, #238	; 0xee
 286:	f7ff ff33 	bl	f0 <mbus_write_message32>
 28a:	6025      	str	r5, [r4, #0]
 28c:	bd38      	pop	{r3, r4, r5, pc}
 28e:	46c0      	nop			; (mov r8, r8)
 290:	e000e280 	.word	0xe000e280
 294:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_7:

00000298 <handler_ext_int_7>:
 298:	b538      	push	{r3, r4, r5, lr}
 29a:	4b07      	ldr	r3, [pc, #28]	; (2b8 <handler_ext_int_7+0x20>)
 29c:	4c07      	ldr	r4, [pc, #28]	; (2bc <handler_ext_int_7+0x24>)
 29e:	2280      	movs	r2, #128	; 0x80
 2a0:	601a      	str	r2, [r3, #0]
 2a2:	23a0      	movs	r3, #160	; 0xa0
 2a4:	6825      	ldr	r5, [r4, #0]
 2a6:	021b      	lsls	r3, r3, #8
 2a8:	6023      	str	r3, [r4, #0]
 2aa:	2107      	movs	r1, #7
 2ac:	20ee      	movs	r0, #238	; 0xee
 2ae:	f7ff ff1f 	bl	f0 <mbus_write_message32>
 2b2:	6025      	str	r5, [r4, #0]
 2b4:	bd38      	pop	{r3, r4, r5, pc}
 2b6:	46c0      	nop			; (mov r8, r8)
 2b8:	e000e280 	.word	0xe000e280
 2bc:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_8:

000002c0 <handler_ext_int_8>:
 2c0:	b538      	push	{r3, r4, r5, lr}
 2c2:	2280      	movs	r2, #128	; 0x80
 2c4:	4b06      	ldr	r3, [pc, #24]	; (2e0 <handler_ext_int_8+0x20>)
 2c6:	4c07      	ldr	r4, [pc, #28]	; (2e4 <handler_ext_int_8+0x24>)
 2c8:	0052      	lsls	r2, r2, #1
 2ca:	601a      	str	r2, [r3, #0]
 2cc:	23a0      	movs	r3, #160	; 0xa0
 2ce:	6825      	ldr	r5, [r4, #0]
 2d0:	021b      	lsls	r3, r3, #8
 2d2:	6023      	str	r3, [r4, #0]
 2d4:	2108      	movs	r1, #8
 2d6:	20ee      	movs	r0, #238	; 0xee
 2d8:	f7ff ff0a 	bl	f0 <mbus_write_message32>
 2dc:	6025      	str	r5, [r4, #0]
 2de:	bd38      	pop	{r3, r4, r5, pc}
 2e0:	e000e280 	.word	0xe000e280
 2e4:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_9:

000002e8 <handler_ext_int_9>:
 2e8:	b538      	push	{r3, r4, r5, lr}
 2ea:	2280      	movs	r2, #128	; 0x80
 2ec:	4b06      	ldr	r3, [pc, #24]	; (308 <handler_ext_int_9+0x20>)
 2ee:	4c07      	ldr	r4, [pc, #28]	; (30c <handler_ext_int_9+0x24>)
 2f0:	0092      	lsls	r2, r2, #2
 2f2:	601a      	str	r2, [r3, #0]
 2f4:	23a0      	movs	r3, #160	; 0xa0
 2f6:	6825      	ldr	r5, [r4, #0]
 2f8:	021b      	lsls	r3, r3, #8
 2fa:	6023      	str	r3, [r4, #0]
 2fc:	2109      	movs	r1, #9
 2fe:	20ee      	movs	r0, #238	; 0xee
 300:	f7ff fef6 	bl	f0 <mbus_write_message32>
 304:	6025      	str	r5, [r4, #0]
 306:	bd38      	pop	{r3, r4, r5, pc}
 308:	e000e280 	.word	0xe000e280
 30c:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_10:

00000310 <handler_ext_int_10>:
 310:	b538      	push	{r3, r4, r5, lr}
 312:	2280      	movs	r2, #128	; 0x80
 314:	4b06      	ldr	r3, [pc, #24]	; (330 <handler_ext_int_10+0x20>)
 316:	4c07      	ldr	r4, [pc, #28]	; (334 <handler_ext_int_10+0x24>)
 318:	00d2      	lsls	r2, r2, #3
 31a:	601a      	str	r2, [r3, #0]
 31c:	23a0      	movs	r3, #160	; 0xa0
 31e:	6825      	ldr	r5, [r4, #0]
 320:	021b      	lsls	r3, r3, #8
 322:	6023      	str	r3, [r4, #0]
 324:	2110      	movs	r1, #16
 326:	20ee      	movs	r0, #238	; 0xee
 328:	f7ff fee2 	bl	f0 <mbus_write_message32>
 32c:	6025      	str	r5, [r4, #0]
 32e:	bd38      	pop	{r3, r4, r5, pc}
 330:	e000e280 	.word	0xe000e280
 334:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_11:

00000338 <handler_ext_int_11>:
 338:	b538      	push	{r3, r4, r5, lr}
 33a:	2280      	movs	r2, #128	; 0x80
 33c:	4b06      	ldr	r3, [pc, #24]	; (358 <handler_ext_int_11+0x20>)
 33e:	4c07      	ldr	r4, [pc, #28]	; (35c <handler_ext_int_11+0x24>)
 340:	0112      	lsls	r2, r2, #4
 342:	601a      	str	r2, [r3, #0]
 344:	23a0      	movs	r3, #160	; 0xa0
 346:	6825      	ldr	r5, [r4, #0]
 348:	021b      	lsls	r3, r3, #8
 34a:	6023      	str	r3, [r4, #0]
 34c:	2111      	movs	r1, #17
 34e:	20ee      	movs	r0, #238	; 0xee
 350:	f7ff fece 	bl	f0 <mbus_write_message32>
 354:	6025      	str	r5, [r4, #0]
 356:	bd38      	pop	{r3, r4, r5, pc}
 358:	e000e280 	.word	0xe000e280
 35c:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_12:

00000360 <handler_ext_int_12>:
 360:	b538      	push	{r3, r4, r5, lr}
 362:	2280      	movs	r2, #128	; 0x80
 364:	4b06      	ldr	r3, [pc, #24]	; (380 <handler_ext_int_12+0x20>)
 366:	4c07      	ldr	r4, [pc, #28]	; (384 <handler_ext_int_12+0x24>)
 368:	0152      	lsls	r2, r2, #5
 36a:	601a      	str	r2, [r3, #0]
 36c:	23a0      	movs	r3, #160	; 0xa0
 36e:	6825      	ldr	r5, [r4, #0]
 370:	021b      	lsls	r3, r3, #8
 372:	6023      	str	r3, [r4, #0]
 374:	2112      	movs	r1, #18
 376:	20ee      	movs	r0, #238	; 0xee
 378:	f7ff feba 	bl	f0 <mbus_write_message32>
 37c:	6025      	str	r5, [r4, #0]
 37e:	bd38      	pop	{r3, r4, r5, pc}
 380:	e000e280 	.word	0xe000e280
 384:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_13:

00000388 <handler_ext_int_13>:
 388:	b538      	push	{r3, r4, r5, lr}
 38a:	2280      	movs	r2, #128	; 0x80
 38c:	4b06      	ldr	r3, [pc, #24]	; (3a8 <handler_ext_int_13+0x20>)
 38e:	4c07      	ldr	r4, [pc, #28]	; (3ac <handler_ext_int_13+0x24>)
 390:	0192      	lsls	r2, r2, #6
 392:	601a      	str	r2, [r3, #0]
 394:	23a0      	movs	r3, #160	; 0xa0
 396:	6825      	ldr	r5, [r4, #0]
 398:	021b      	lsls	r3, r3, #8
 39a:	6023      	str	r3, [r4, #0]
 39c:	2113      	movs	r1, #19
 39e:	20ee      	movs	r0, #238	; 0xee
 3a0:	f7ff fea6 	bl	f0 <mbus_write_message32>
 3a4:	6025      	str	r5, [r4, #0]
 3a6:	bd38      	pop	{r3, r4, r5, pc}
 3a8:	e000e280 	.word	0xe000e280
 3ac:	a0000028 	.word	0xa0000028

Disassembly of section .text.handler_ext_int_14:

000003b0 <handler_ext_int_14>:
 3b0:	b538      	push	{r3, r4, r5, lr}
 3b2:	2280      	movs	r2, #128	; 0x80
 3b4:	4b06      	ldr	r3, [pc, #24]	; (3d0 <handler_ext_int_14+0x20>)
 3b6:	4c07      	ldr	r4, [pc, #28]	; (3d4 <handler_ext_int_14+0x24>)
 3b8:	01d2      	lsls	r2, r2, #7
 3ba:	601a      	str	r2, [r3, #0]
 3bc:	23a0      	movs	r3, #160	; 0xa0
 3be:	6825      	ldr	r5, [r4, #0]
 3c0:	021b      	lsls	r3, r3, #8
 3c2:	6023      	str	r3, [r4, #0]
 3c4:	2114      	movs	r1, #20
 3c6:	20ee      	movs	r0, #238	; 0xee
 3c8:	f7ff fe92 	bl	f0 <mbus_write_message32>
 3cc:	6025      	str	r5, [r4, #0]
 3ce:	bd38      	pop	{r3, r4, r5, pc}
 3d0:	e000e280 	.word	0xe000e280
 3d4:	a0000028 	.word	0xa0000028

Disassembly of section .text.initialization:

000003d8 <initialization>:
 3d8:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 3da:	4ac6      	ldr	r2, [pc, #792]	; (6f4 <initialization+0x31c>)
 3dc:	4bc6      	ldr	r3, [pc, #792]	; (6f8 <initialization+0x320>)
 3de:	4ec7      	ldr	r6, [pc, #796]	; (6fc <initialization+0x324>)
 3e0:	601a      	str	r2, [r3, #0]
 3e2:	4bc7      	ldr	r3, [pc, #796]	; (700 <initialization+0x328>)
 3e4:	2200      	movs	r2, #0
 3e6:	601a      	str	r2, [r3, #0]
 3e8:	27a0      	movs	r7, #160	; 0xa0
 3ea:	4ac6      	ldr	r2, [pc, #792]	; (704 <initialization+0x32c>)
 3ec:	2301      	movs	r3, #1
 3ee:	063f      	lsls	r7, r7, #24
 3f0:	425b      	negs	r3, r3
 3f2:	603b      	str	r3, [r7, #0]
 3f4:	6033      	str	r3, [r6, #0]
 3f6:	6013      	str	r3, [r2, #0]
 3f8:	4ac3      	ldr	r2, [pc, #780]	; (708 <initialization+0x330>)
 3fa:	4cc4      	ldr	r4, [pc, #784]	; (70c <initialization+0x334>)
 3fc:	6013      	str	r3, [r2, #0]
 3fe:	4ac4      	ldr	r2, [pc, #784]	; (710 <initialization+0x338>)
 400:	4dc4      	ldr	r5, [pc, #784]	; (714 <initialization+0x33c>)
 402:	6013      	str	r3, [r2, #0]
 404:	4ac4      	ldr	r2, [pc, #784]	; (718 <initialization+0x340>)
 406:	49bb      	ldr	r1, [pc, #748]	; (6f4 <initialization+0x31c>)
 408:	6013      	str	r3, [r2, #0]
 40a:	20ef      	movs	r0, #239	; 0xef
 40c:	6023      	str	r3, [r4, #0]
 40e:	602b      	str	r3, [r5, #0]
 410:	f7ff fe6e 	bl	f0 <mbus_write_message32>
 414:	6839      	ldr	r1, [r7, #0]
 416:	20e0      	movs	r0, #224	; 0xe0
 418:	f7ff fe6a 	bl	f0 <mbus_write_message32>
 41c:	6831      	ldr	r1, [r6, #0]
 41e:	20e1      	movs	r0, #225	; 0xe1
 420:	f7ff fe66 	bl	f0 <mbus_write_message32>
 424:	4bb7      	ldr	r3, [pc, #732]	; (704 <initialization+0x32c>)
 426:	20e2      	movs	r0, #226	; 0xe2
 428:	6819      	ldr	r1, [r3, #0]
 42a:	f7ff fe61 	bl	f0 <mbus_write_message32>
 42e:	4ab6      	ldr	r2, [pc, #728]	; (708 <initialization+0x330>)
 430:	20e3      	movs	r0, #227	; 0xe3
 432:	6811      	ldr	r1, [r2, #0]
 434:	f7ff fe5c 	bl	f0 <mbus_write_message32>
 438:	4bb5      	ldr	r3, [pc, #724]	; (710 <initialization+0x338>)
 43a:	20e4      	movs	r0, #228	; 0xe4
 43c:	6819      	ldr	r1, [r3, #0]
 43e:	f7ff fe57 	bl	f0 <mbus_write_message32>
 442:	4ab5      	ldr	r2, [pc, #724]	; (718 <initialization+0x340>)
 444:	20e5      	movs	r0, #229	; 0xe5
 446:	6811      	ldr	r1, [r2, #0]
 448:	f7ff fe52 	bl	f0 <mbus_write_message32>
 44c:	6821      	ldr	r1, [r4, #0]
 44e:	20e6      	movs	r0, #230	; 0xe6
 450:	f7ff fe4e 	bl	f0 <mbus_write_message32>
 454:	6829      	ldr	r1, [r5, #0]
 456:	20e7      	movs	r0, #231	; 0xe7
 458:	f7ff fe4a 	bl	f0 <mbus_write_message32>
 45c:	49a5      	ldr	r1, [pc, #660]	; (6f4 <initialization+0x31c>)
 45e:	20ef      	movs	r0, #239	; 0xef
 460:	f7ff fe46 	bl	f0 <mbus_write_message32>
 464:	4aa7      	ldr	r2, [pc, #668]	; (704 <initialization+0x32c>)
 466:	2300      	movs	r3, #0
 468:	603b      	str	r3, [r7, #0]
 46a:	6033      	str	r3, [r6, #0]
 46c:	6013      	str	r3, [r2, #0]
 46e:	4aa6      	ldr	r2, [pc, #664]	; (708 <initialization+0x330>)
 470:	49a0      	ldr	r1, [pc, #640]	; (6f4 <initialization+0x31c>)
 472:	6013      	str	r3, [r2, #0]
 474:	4aa6      	ldr	r2, [pc, #664]	; (710 <initialization+0x338>)
 476:	20ef      	movs	r0, #239	; 0xef
 478:	6013      	str	r3, [r2, #0]
 47a:	4aa7      	ldr	r2, [pc, #668]	; (718 <initialization+0x340>)
 47c:	6013      	str	r3, [r2, #0]
 47e:	6023      	str	r3, [r4, #0]
 480:	602b      	str	r3, [r5, #0]
 482:	f7ff fe35 	bl	f0 <mbus_write_message32>
 486:	6839      	ldr	r1, [r7, #0]
 488:	20e0      	movs	r0, #224	; 0xe0
 48a:	f7ff fe31 	bl	f0 <mbus_write_message32>
 48e:	6831      	ldr	r1, [r6, #0]
 490:	20e1      	movs	r0, #225	; 0xe1
 492:	f7ff fe2d 	bl	f0 <mbus_write_message32>
 496:	4b9b      	ldr	r3, [pc, #620]	; (704 <initialization+0x32c>)
 498:	20e2      	movs	r0, #226	; 0xe2
 49a:	6819      	ldr	r1, [r3, #0]
 49c:	f7ff fe28 	bl	f0 <mbus_write_message32>
 4a0:	4a99      	ldr	r2, [pc, #612]	; (708 <initialization+0x330>)
 4a2:	20e3      	movs	r0, #227	; 0xe3
 4a4:	6811      	ldr	r1, [r2, #0]
 4a6:	f7ff fe23 	bl	f0 <mbus_write_message32>
 4aa:	4b99      	ldr	r3, [pc, #612]	; (710 <initialization+0x338>)
 4ac:	20e4      	movs	r0, #228	; 0xe4
 4ae:	6819      	ldr	r1, [r3, #0]
 4b0:	f7ff fe1e 	bl	f0 <mbus_write_message32>
 4b4:	4a98      	ldr	r2, [pc, #608]	; (718 <initialization+0x340>)
 4b6:	20e5      	movs	r0, #229	; 0xe5
 4b8:	6811      	ldr	r1, [r2, #0]
 4ba:	f7ff fe19 	bl	f0 <mbus_write_message32>
 4be:	6821      	ldr	r1, [r4, #0]
 4c0:	20e6      	movs	r0, #230	; 0xe6
 4c2:	f7ff fe15 	bl	f0 <mbus_write_message32>
 4c6:	6829      	ldr	r1, [r5, #0]
 4c8:	20e7      	movs	r0, #231	; 0xe7
 4ca:	f7ff fe11 	bl	f0 <mbus_write_message32>
 4ce:	4989      	ldr	r1, [pc, #548]	; (6f4 <initialization+0x31c>)
 4d0:	20ef      	movs	r0, #239	; 0xef
 4d2:	f7ff fe0d 	bl	f0 <mbus_write_message32>
 4d6:	4a8b      	ldr	r2, [pc, #556]	; (704 <initialization+0x32c>)
 4d8:	2301      	movs	r3, #1
 4da:	425b      	negs	r3, r3
 4dc:	603b      	str	r3, [r7, #0]
 4de:	6033      	str	r3, [r6, #0]
 4e0:	6013      	str	r3, [r2, #0]
 4e2:	4a89      	ldr	r2, [pc, #548]	; (708 <initialization+0x330>)
 4e4:	4983      	ldr	r1, [pc, #524]	; (6f4 <initialization+0x31c>)
 4e6:	6013      	str	r3, [r2, #0]
 4e8:	4a89      	ldr	r2, [pc, #548]	; (710 <initialization+0x338>)
 4ea:	20ef      	movs	r0, #239	; 0xef
 4ec:	6013      	str	r3, [r2, #0]
 4ee:	4a8a      	ldr	r2, [pc, #552]	; (718 <initialization+0x340>)
 4f0:	6013      	str	r3, [r2, #0]
 4f2:	6023      	str	r3, [r4, #0]
 4f4:	602b      	str	r3, [r5, #0]
 4f6:	f7ff fdfb 	bl	f0 <mbus_write_message32>
 4fa:	6839      	ldr	r1, [r7, #0]
 4fc:	20e0      	movs	r0, #224	; 0xe0
 4fe:	f7ff fdf7 	bl	f0 <mbus_write_message32>
 502:	6831      	ldr	r1, [r6, #0]
 504:	20e1      	movs	r0, #225	; 0xe1
 506:	f7ff fdf3 	bl	f0 <mbus_write_message32>
 50a:	4b7e      	ldr	r3, [pc, #504]	; (704 <initialization+0x32c>)
 50c:	20e2      	movs	r0, #226	; 0xe2
 50e:	6819      	ldr	r1, [r3, #0]
 510:	f7ff fdee 	bl	f0 <mbus_write_message32>
 514:	4a7c      	ldr	r2, [pc, #496]	; (708 <initialization+0x330>)
 516:	20e3      	movs	r0, #227	; 0xe3
 518:	6811      	ldr	r1, [r2, #0]
 51a:	f7ff fde9 	bl	f0 <mbus_write_message32>
 51e:	4b7c      	ldr	r3, [pc, #496]	; (710 <initialization+0x338>)
 520:	20e4      	movs	r0, #228	; 0xe4
 522:	6819      	ldr	r1, [r3, #0]
 524:	f7ff fde4 	bl	f0 <mbus_write_message32>
 528:	4a7b      	ldr	r2, [pc, #492]	; (718 <initialization+0x340>)
 52a:	20e5      	movs	r0, #229	; 0xe5
 52c:	6811      	ldr	r1, [r2, #0]
 52e:	f7ff fddf 	bl	f0 <mbus_write_message32>
 532:	6821      	ldr	r1, [r4, #0]
 534:	20e6      	movs	r0, #230	; 0xe6
 536:	f7ff fddb 	bl	f0 <mbus_write_message32>
 53a:	6829      	ldr	r1, [r5, #0]
 53c:	20e7      	movs	r0, #231	; 0xe7
 53e:	f7ff fdd7 	bl	f0 <mbus_write_message32>
 542:	496c      	ldr	r1, [pc, #432]	; (6f4 <initialization+0x31c>)
 544:	20ef      	movs	r0, #239	; 0xef
 546:	f7ff fdd3 	bl	f0 <mbus_write_message32>
 54a:	4a6e      	ldr	r2, [pc, #440]	; (704 <initialization+0x32c>)
 54c:	2300      	movs	r3, #0
 54e:	603b      	str	r3, [r7, #0]
 550:	6033      	str	r3, [r6, #0]
 552:	6013      	str	r3, [r2, #0]
 554:	4a6c      	ldr	r2, [pc, #432]	; (708 <initialization+0x330>)
 556:	4967      	ldr	r1, [pc, #412]	; (6f4 <initialization+0x31c>)
 558:	6013      	str	r3, [r2, #0]
 55a:	4a6d      	ldr	r2, [pc, #436]	; (710 <initialization+0x338>)
 55c:	20ef      	movs	r0, #239	; 0xef
 55e:	6013      	str	r3, [r2, #0]
 560:	4a6d      	ldr	r2, [pc, #436]	; (718 <initialization+0x340>)
 562:	6013      	str	r3, [r2, #0]
 564:	6023      	str	r3, [r4, #0]
 566:	602b      	str	r3, [r5, #0]
 568:	f7ff fdc2 	bl	f0 <mbus_write_message32>
 56c:	6839      	ldr	r1, [r7, #0]
 56e:	20e0      	movs	r0, #224	; 0xe0
 570:	f7ff fdbe 	bl	f0 <mbus_write_message32>
 574:	6831      	ldr	r1, [r6, #0]
 576:	20e1      	movs	r0, #225	; 0xe1
 578:	f7ff fdba 	bl	f0 <mbus_write_message32>
 57c:	4b61      	ldr	r3, [pc, #388]	; (704 <initialization+0x32c>)
 57e:	20e2      	movs	r0, #226	; 0xe2
 580:	6819      	ldr	r1, [r3, #0]
 582:	f7ff fdb5 	bl	f0 <mbus_write_message32>
 586:	4a60      	ldr	r2, [pc, #384]	; (708 <initialization+0x330>)
 588:	20e3      	movs	r0, #227	; 0xe3
 58a:	6811      	ldr	r1, [r2, #0]
 58c:	f7ff fdb0 	bl	f0 <mbus_write_message32>
 590:	4b5f      	ldr	r3, [pc, #380]	; (710 <initialization+0x338>)
 592:	20e4      	movs	r0, #228	; 0xe4
 594:	6819      	ldr	r1, [r3, #0]
 596:	f7ff fdab 	bl	f0 <mbus_write_message32>
 59a:	4a5f      	ldr	r2, [pc, #380]	; (718 <initialization+0x340>)
 59c:	20e5      	movs	r0, #229	; 0xe5
 59e:	6811      	ldr	r1, [r2, #0]
 5a0:	f7ff fda6 	bl	f0 <mbus_write_message32>
 5a4:	6821      	ldr	r1, [r4, #0]
 5a6:	20e6      	movs	r0, #230	; 0xe6
 5a8:	f7ff fda2 	bl	f0 <mbus_write_message32>
 5ac:	6829      	ldr	r1, [r5, #0]
 5ae:	20e7      	movs	r0, #231	; 0xe7
 5b0:	f7ff fd9e 	bl	f0 <mbus_write_message32>
 5b4:	494f      	ldr	r1, [pc, #316]	; (6f4 <initialization+0x31c>)
 5b6:	20ef      	movs	r0, #239	; 0xef
 5b8:	f7ff fd9a 	bl	f0 <mbus_write_message32>
 5bc:	4b57      	ldr	r3, [pc, #348]	; (71c <initialization+0x344>)
 5be:	4a51      	ldr	r2, [pc, #324]	; (704 <initialization+0x32c>)
 5c0:	603b      	str	r3, [r7, #0]
 5c2:	6033      	str	r3, [r6, #0]
 5c4:	6013      	str	r3, [r2, #0]
 5c6:	4a50      	ldr	r2, [pc, #320]	; (708 <initialization+0x330>)
 5c8:	494a      	ldr	r1, [pc, #296]	; (6f4 <initialization+0x31c>)
 5ca:	6013      	str	r3, [r2, #0]
 5cc:	4a50      	ldr	r2, [pc, #320]	; (710 <initialization+0x338>)
 5ce:	20ef      	movs	r0, #239	; 0xef
 5d0:	6013      	str	r3, [r2, #0]
 5d2:	4a51      	ldr	r2, [pc, #324]	; (718 <initialization+0x340>)
 5d4:	6013      	str	r3, [r2, #0]
 5d6:	6023      	str	r3, [r4, #0]
 5d8:	602b      	str	r3, [r5, #0]
 5da:	f7ff fd89 	bl	f0 <mbus_write_message32>
 5de:	6839      	ldr	r1, [r7, #0]
 5e0:	20e0      	movs	r0, #224	; 0xe0
 5e2:	f7ff fd85 	bl	f0 <mbus_write_message32>
 5e6:	6831      	ldr	r1, [r6, #0]
 5e8:	20e1      	movs	r0, #225	; 0xe1
 5ea:	f7ff fd81 	bl	f0 <mbus_write_message32>
 5ee:	4b45      	ldr	r3, [pc, #276]	; (704 <initialization+0x32c>)
 5f0:	20e2      	movs	r0, #226	; 0xe2
 5f2:	6819      	ldr	r1, [r3, #0]
 5f4:	f7ff fd7c 	bl	f0 <mbus_write_message32>
 5f8:	4a43      	ldr	r2, [pc, #268]	; (708 <initialization+0x330>)
 5fa:	20e3      	movs	r0, #227	; 0xe3
 5fc:	6811      	ldr	r1, [r2, #0]
 5fe:	f7ff fd77 	bl	f0 <mbus_write_message32>
 602:	4b43      	ldr	r3, [pc, #268]	; (710 <initialization+0x338>)
 604:	20e4      	movs	r0, #228	; 0xe4
 606:	6819      	ldr	r1, [r3, #0]
 608:	f7ff fd72 	bl	f0 <mbus_write_message32>
 60c:	4a42      	ldr	r2, [pc, #264]	; (718 <initialization+0x340>)
 60e:	20e5      	movs	r0, #229	; 0xe5
 610:	6811      	ldr	r1, [r2, #0]
 612:	f7ff fd6d 	bl	f0 <mbus_write_message32>
 616:	6821      	ldr	r1, [r4, #0]
 618:	20e6      	movs	r0, #230	; 0xe6
 61a:	f7ff fd69 	bl	f0 <mbus_write_message32>
 61e:	6829      	ldr	r1, [r5, #0]
 620:	20e7      	movs	r0, #231	; 0xe7
 622:	f7ff fd65 	bl	f0 <mbus_write_message32>
 626:	4933      	ldr	r1, [pc, #204]	; (6f4 <initialization+0x31c>)
 628:	20ef      	movs	r0, #239	; 0xef
 62a:	f7ff fd61 	bl	f0 <mbus_write_message32>
 62e:	4b3c      	ldr	r3, [pc, #240]	; (720 <initialization+0x348>)
 630:	4a34      	ldr	r2, [pc, #208]	; (704 <initialization+0x32c>)
 632:	603b      	str	r3, [r7, #0]
 634:	6033      	str	r3, [r6, #0]
 636:	6013      	str	r3, [r2, #0]
 638:	4a33      	ldr	r2, [pc, #204]	; (708 <initialization+0x330>)
 63a:	492e      	ldr	r1, [pc, #184]	; (6f4 <initialization+0x31c>)
 63c:	6013      	str	r3, [r2, #0]
 63e:	4a34      	ldr	r2, [pc, #208]	; (710 <initialization+0x338>)
 640:	20ef      	movs	r0, #239	; 0xef
 642:	6013      	str	r3, [r2, #0]
 644:	4a34      	ldr	r2, [pc, #208]	; (718 <initialization+0x340>)
 646:	6013      	str	r3, [r2, #0]
 648:	6023      	str	r3, [r4, #0]
 64a:	602b      	str	r3, [r5, #0]
 64c:	f7ff fd50 	bl	f0 <mbus_write_message32>
 650:	6839      	ldr	r1, [r7, #0]
 652:	20e0      	movs	r0, #224	; 0xe0
 654:	f7ff fd4c 	bl	f0 <mbus_write_message32>
 658:	6831      	ldr	r1, [r6, #0]
 65a:	20e1      	movs	r0, #225	; 0xe1
 65c:	f7ff fd48 	bl	f0 <mbus_write_message32>
 660:	4b28      	ldr	r3, [pc, #160]	; (704 <initialization+0x32c>)
 662:	20e2      	movs	r0, #226	; 0xe2
 664:	6819      	ldr	r1, [r3, #0]
 666:	f7ff fd43 	bl	f0 <mbus_write_message32>
 66a:	4a27      	ldr	r2, [pc, #156]	; (708 <initialization+0x330>)
 66c:	20e3      	movs	r0, #227	; 0xe3
 66e:	6811      	ldr	r1, [r2, #0]
 670:	f7ff fd3e 	bl	f0 <mbus_write_message32>
 674:	4b26      	ldr	r3, [pc, #152]	; (710 <initialization+0x338>)
 676:	20e4      	movs	r0, #228	; 0xe4
 678:	6819      	ldr	r1, [r3, #0]
 67a:	f7ff fd39 	bl	f0 <mbus_write_message32>
 67e:	4a26      	ldr	r2, [pc, #152]	; (718 <initialization+0x340>)
 680:	20e5      	movs	r0, #229	; 0xe5
 682:	6811      	ldr	r1, [r2, #0]
 684:	f7ff fd34 	bl	f0 <mbus_write_message32>
 688:	6821      	ldr	r1, [r4, #0]
 68a:	20e6      	movs	r0, #230	; 0xe6
 68c:	f7ff fd30 	bl	f0 <mbus_write_message32>
 690:	6829      	ldr	r1, [r5, #0]
 692:	20e7      	movs	r0, #231	; 0xe7
 694:	f7ff fd2c 	bl	f0 <mbus_write_message32>
 698:	4916      	ldr	r1, [pc, #88]	; (6f4 <initialization+0x31c>)
 69a:	20ef      	movs	r0, #239	; 0xef
 69c:	f7ff fd28 	bl	f0 <mbus_write_message32>
 6a0:	4b20      	ldr	r3, [pc, #128]	; (724 <initialization+0x34c>)
 6a2:	20e8      	movs	r0, #232	; 0xe8
 6a4:	6819      	ldr	r1, [r3, #0]
 6a6:	f7ff fd23 	bl	f0 <mbus_write_message32>
 6aa:	4b1f      	ldr	r3, [pc, #124]	; (728 <initialization+0x350>)
 6ac:	4c1f      	ldr	r4, [pc, #124]	; (72c <initialization+0x354>)
 6ae:	6819      	ldr	r1, [r3, #0]
 6b0:	20e9      	movs	r0, #233	; 0xe9
 6b2:	f7ff fd1d 	bl	f0 <mbus_write_message32>
 6b6:	6821      	ldr	r1, [r4, #0]
 6b8:	20ea      	movs	r0, #234	; 0xea
 6ba:	f7ff fd19 	bl	f0 <mbus_write_message32>
 6be:	4b1c      	ldr	r3, [pc, #112]	; (730 <initialization+0x358>)
 6c0:	20eb      	movs	r0, #235	; 0xeb
 6c2:	6819      	ldr	r1, [r3, #0]
 6c4:	f7ff fd14 	bl	f0 <mbus_write_message32>
 6c8:	4b1a      	ldr	r3, [pc, #104]	; (734 <initialization+0x35c>)
 6ca:	20ec      	movs	r0, #236	; 0xec
 6cc:	6819      	ldr	r1, [r3, #0]
 6ce:	f7ff fd0f 	bl	f0 <mbus_write_message32>
 6d2:	4908      	ldr	r1, [pc, #32]	; (6f4 <initialization+0x31c>)
 6d4:	20ef      	movs	r0, #239	; 0xef
 6d6:	f7ff fd0b 	bl	f0 <mbus_write_message32>
 6da:	2301      	movs	r3, #1
 6dc:	425b      	negs	r3, r3
 6de:	6023      	str	r3, [r4, #0]
 6e0:	6821      	ldr	r1, [r4, #0]
 6e2:	20ea      	movs	r0, #234	; 0xea
 6e4:	f7ff fd04 	bl	f0 <mbus_write_message32>
 6e8:	20ef      	movs	r0, #239	; 0xef
 6ea:	4902      	ldr	r1, [pc, #8]	; (6f4 <initialization+0x31c>)
 6ec:	f7ff fd00 	bl	f0 <mbus_write_message32>
 6f0:	e7fe      	b.n	6f0 <initialization+0x318>
 6f2:	46c0      	nop			; (mov r8, r8)
 6f4:	deadbeef 	.word	0xdeadbeef
 6f8:	000007c8 	.word	0x000007c8
 6fc:	a0000004 	.word	0xa0000004
 700:	000007cc 	.word	0x000007cc
 704:	a0000008 	.word	0xa0000008
 708:	a000000c 	.word	0xa000000c
 70c:	a0000018 	.word	0xa0000018
 710:	a0000010 	.word	0xa0000010
 714:	a000001c 	.word	0xa000001c
 718:	a0000014 	.word	0xa0000014
 71c:	aaaaaaaa 	.word	0xaaaaaaaa
 720:	bbbbbbbb 	.word	0xbbbbbbbb
 724:	a0000020 	.word	0xa0000020
 728:	a0000024 	.word	0xa0000024
 72c:	a0000028 	.word	0xa0000028
 730:	a000002c 	.word	0xa000002c
 734:	a0000030 	.word	0xa0000030

Disassembly of section .text.startup.main:

00000738 <main>:
 738:	b510      	push	{r4, lr}
 73a:	f7ff fce1 	bl	100 <init_interrupt>
 73e:	4b19      	ldr	r3, [pc, #100]	; (7a4 <main+0x6c>)
 740:	681a      	ldr	r2, [r3, #0]
 742:	4b19      	ldr	r3, [pc, #100]	; (7a8 <main+0x70>)
 744:	429a      	cmp	r2, r3
 746:	d001      	beq.n	74c <main+0x14>
 748:	f7ff fe46 	bl	3d8 <initialization>
 74c:	4c17      	ldr	r4, [pc, #92]	; (7ac <main+0x74>)
 74e:	20ef      	movs	r0, #239	; 0xef
 750:	6821      	ldr	r1, [r4, #0]
 752:	f7ff fccd 	bl	f0 <mbus_write_message32>
 756:	4b16      	ldr	r3, [pc, #88]	; (7b0 <main+0x78>)
 758:	4a16      	ldr	r2, [pc, #88]	; (7b4 <main+0x7c>)
 75a:	601a      	str	r2, [r3, #0]
 75c:	6823      	ldr	r3, [r4, #0]
 75e:	2b00      	cmp	r3, #0
 760:	d104      	bne.n	76c <main+0x34>
 762:	4915      	ldr	r1, [pc, #84]	; (7b8 <main+0x80>)
 764:	20ef      	movs	r0, #239	; 0xef
 766:	f7ff fcc3 	bl	f0 <mbus_write_message32>
 76a:	e00e      	b.n	78a <main+0x52>
 76c:	6823      	ldr	r3, [r4, #0]
 76e:	20ef      	movs	r0, #239	; 0xef
 770:	2b01      	cmp	r3, #1
 772:	d106      	bne.n	782 <main+0x4a>
 774:	4911      	ldr	r1, [pc, #68]	; (7bc <main+0x84>)
 776:	f7ff fcbb 	bl	f0 <mbus_write_message32>
 77a:	4811      	ldr	r0, [pc, #68]	; (7c0 <main+0x88>)
 77c:	f7ff fc8b 	bl	96 <delay>
 780:	e003      	b.n	78a <main+0x52>
 782:	4910      	ldr	r1, [pc, #64]	; (7c4 <main+0x8c>)
 784:	f7ff fcb4 	bl	f0 <mbus_write_message32>
 788:	e7fe      	b.n	788 <main+0x50>
 78a:	6823      	ldr	r3, [r4, #0]
 78c:	2101      	movs	r1, #1
 78e:	3301      	adds	r3, #1
 790:	200a      	movs	r0, #10
 792:	1c0a      	adds	r2, r1, #0
 794:	6023      	str	r3, [r4, #0]
 796:	f7ff fc87 	bl	a8 <set_wakeup_timer>
 79a:	2001      	movs	r0, #1
 79c:	2100      	movs	r1, #0
 79e:	f7ff fca7 	bl	f0 <mbus_write_message32>
 7a2:	e7fe      	b.n	7a2 <main+0x6a>
 7a4:	000007c8 	.word	0x000007c8
 7a8:	deadbeef 	.word	0xdeadbeef
 7ac:	000007cc 	.word	0x000007cc
 7b0:	a0003ef0 	.word	0xa0003ef0
 7b4:	cccccccc 	.word	0xcccccccc
 7b8:	aaaaaaaa 	.word	0xaaaaaaaa
 7bc:	bbbbbbbb 	.word	0xbbbbbbbb
 7c0:	00002710 	.word	0x00002710
 7c4:	0ea7f00d 	.word	0x0ea7f00d
