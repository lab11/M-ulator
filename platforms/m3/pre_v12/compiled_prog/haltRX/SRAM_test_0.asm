
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
  a0:	f000 f9ea 	bl	478 <main>
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
 120:	f7ff ffca 	bl	b8 <write_regfile>
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
 140:	f7ff ffba 	bl	b8 <write_regfile>
 144:	bd08      	pop	{r3, pc}
 146:	46c0      	nop			; (mov r8, r8)
 148:	a0000028 	.word	0xa0000028
 14c:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.mbus_write_message32:

00000150 <mbus_write_message32>:
 150:	4b02      	ldr	r3, [pc, #8]	; (15c <mbus_write_message32+0xc>)
 152:	0100      	lsls	r0, r0, #4
 154:	4318      	orrs	r0, r3
 156:	6001      	str	r1, [r0, #0]
 158:	2001      	movs	r0, #1
 15a:	4770      	bx	lr
 15c:	a0003000 	.word	0xa0003000

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
 28e:	4a07      	ldr	r2, [pc, #28]	; (2ac <initialization+0x20>)
 290:	4b07      	ldr	r3, [pc, #28]	; (2b0 <initialization+0x24>)
 292:	601a      	str	r2, [r3, #0]
 294:	4b07      	ldr	r3, [pc, #28]	; (2b4 <initialization+0x28>)
 296:	2200      	movs	r2, #0
 298:	601a      	str	r2, [r3, #0]
 29a:	f7ff ff39 	bl	110 <set_halt_until_mbus_rx>
 29e:	2004      	movs	r0, #4
 2a0:	f7ff ff5e 	bl	160 <mbus_enumerate>
 2a4:	f7ff ff44 	bl	130 <set_halt_until_mbus_tx>
 2a8:	bd08      	pop	{r3, pc}
 2aa:	46c0      	nop			; (mov r8, r8)
 2ac:	deadbeef 	.word	0xdeadbeef
 2b0:	000005b0 	.word	0x000005b0
 2b4:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle0:

000002b8 <cycle0>:
 2b8:	b508      	push	{r3, lr}
 2ba:	4b05      	ldr	r3, [pc, #20]	; (2d0 <cycle0+0x18>)
 2bc:	20a0      	movs	r0, #160	; 0xa0
 2be:	6819      	ldr	r1, [r3, #0]
 2c0:	f7ff ff46 	bl	150 <mbus_write_message32>
 2c4:	20fa      	movs	r0, #250	; 0xfa
 2c6:	0080      	lsls	r0, r0, #2
 2c8:	f7ff feed 	bl	a6 <delay>
 2cc:	bd08      	pop	{r3, pc}
 2ce:	46c0      	nop			; (mov r8, r8)
 2d0:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle1:

000002d4 <cycle1>:
 2d4:	b508      	push	{r3, lr}
 2d6:	4b05      	ldr	r3, [pc, #20]	; (2ec <cycle1+0x18>)
 2d8:	20a1      	movs	r0, #161	; 0xa1
 2da:	6819      	ldr	r1, [r3, #0]
 2dc:	f7ff ff38 	bl	150 <mbus_write_message32>
 2e0:	20fa      	movs	r0, #250	; 0xfa
 2e2:	0080      	lsls	r0, r0, #2
 2e4:	f7ff fedf 	bl	a6 <delay>
 2e8:	bd08      	pop	{r3, pc}
 2ea:	46c0      	nop			; (mov r8, r8)
 2ec:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle2:

000002f0 <cycle2>:
 2f0:	b508      	push	{r3, lr}
 2f2:	4b05      	ldr	r3, [pc, #20]	; (308 <cycle2+0x18>)
 2f4:	20a2      	movs	r0, #162	; 0xa2
 2f6:	6819      	ldr	r1, [r3, #0]
 2f8:	f7ff ff2a 	bl	150 <mbus_write_message32>
 2fc:	20fa      	movs	r0, #250	; 0xfa
 2fe:	0080      	lsls	r0, r0, #2
 300:	f7ff fed1 	bl	a6 <delay>
 304:	bd08      	pop	{r3, pc}
 306:	46c0      	nop			; (mov r8, r8)
 308:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle3:

0000030c <cycle3>:
 30c:	b508      	push	{r3, lr}
 30e:	4b05      	ldr	r3, [pc, #20]	; (324 <cycle3+0x18>)
 310:	20a3      	movs	r0, #163	; 0xa3
 312:	6819      	ldr	r1, [r3, #0]
 314:	f7ff ff1c 	bl	150 <mbus_write_message32>
 318:	20fa      	movs	r0, #250	; 0xfa
 31a:	0080      	lsls	r0, r0, #2
 31c:	f7ff fec3 	bl	a6 <delay>
 320:	bd08      	pop	{r3, pc}
 322:	46c0      	nop			; (mov r8, r8)
 324:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle4:

00000328 <cycle4>:
 328:	b508      	push	{r3, lr}
 32a:	4b05      	ldr	r3, [pc, #20]	; (340 <cycle4+0x18>)
 32c:	20a4      	movs	r0, #164	; 0xa4
 32e:	6819      	ldr	r1, [r3, #0]
 330:	f7ff ff0e 	bl	150 <mbus_write_message32>
 334:	20fa      	movs	r0, #250	; 0xfa
 336:	0080      	lsls	r0, r0, #2
 338:	f7ff feb5 	bl	a6 <delay>
 33c:	bd08      	pop	{r3, pc}
 33e:	46c0      	nop			; (mov r8, r8)
 340:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle5:

00000344 <cycle5>:
 344:	b508      	push	{r3, lr}
 346:	4b05      	ldr	r3, [pc, #20]	; (35c <cycle5+0x18>)
 348:	20a5      	movs	r0, #165	; 0xa5
 34a:	6819      	ldr	r1, [r3, #0]
 34c:	f7ff ff00 	bl	150 <mbus_write_message32>
 350:	20fa      	movs	r0, #250	; 0xfa
 352:	0080      	lsls	r0, r0, #2
 354:	f7ff fea7 	bl	a6 <delay>
 358:	bd08      	pop	{r3, pc}
 35a:	46c0      	nop			; (mov r8, r8)
 35c:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle6:

00000360 <cycle6>:
 360:	b508      	push	{r3, lr}
 362:	4b05      	ldr	r3, [pc, #20]	; (378 <cycle6+0x18>)
 364:	20a6      	movs	r0, #166	; 0xa6
 366:	6819      	ldr	r1, [r3, #0]
 368:	f7ff fef2 	bl	150 <mbus_write_message32>
 36c:	20fa      	movs	r0, #250	; 0xfa
 36e:	0080      	lsls	r0, r0, #2
 370:	f7ff fe99 	bl	a6 <delay>
 374:	bd08      	pop	{r3, pc}
 376:	46c0      	nop			; (mov r8, r8)
 378:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle7:

0000037c <cycle7>:
 37c:	b508      	push	{r3, lr}
 37e:	4b05      	ldr	r3, [pc, #20]	; (394 <cycle7+0x18>)
 380:	20a7      	movs	r0, #167	; 0xa7
 382:	6819      	ldr	r1, [r3, #0]
 384:	f7ff fee4 	bl	150 <mbus_write_message32>
 388:	20fa      	movs	r0, #250	; 0xfa
 38a:	0080      	lsls	r0, r0, #2
 38c:	f7ff fe8b 	bl	a6 <delay>
 390:	bd08      	pop	{r3, pc}
 392:	46c0      	nop			; (mov r8, r8)
 394:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle8:

00000398 <cycle8>:
 398:	b508      	push	{r3, lr}
 39a:	4b05      	ldr	r3, [pc, #20]	; (3b0 <cycle8+0x18>)
 39c:	20a8      	movs	r0, #168	; 0xa8
 39e:	6819      	ldr	r1, [r3, #0]
 3a0:	f7ff fed6 	bl	150 <mbus_write_message32>
 3a4:	20fa      	movs	r0, #250	; 0xfa
 3a6:	0080      	lsls	r0, r0, #2
 3a8:	f7ff fe7d 	bl	a6 <delay>
 3ac:	bd08      	pop	{r3, pc}
 3ae:	46c0      	nop			; (mov r8, r8)
 3b0:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle9:

000003b4 <cycle9>:
 3b4:	b508      	push	{r3, lr}
 3b6:	4b05      	ldr	r3, [pc, #20]	; (3cc <cycle9+0x18>)
 3b8:	20a9      	movs	r0, #169	; 0xa9
 3ba:	6819      	ldr	r1, [r3, #0]
 3bc:	f7ff fec8 	bl	150 <mbus_write_message32>
 3c0:	20fa      	movs	r0, #250	; 0xfa
 3c2:	0080      	lsls	r0, r0, #2
 3c4:	f7ff fe6f 	bl	a6 <delay>
 3c8:	bd08      	pop	{r3, pc}
 3ca:	46c0      	nop			; (mov r8, r8)
 3cc:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle10:

000003d0 <cycle10>:
 3d0:	b508      	push	{r3, lr}
 3d2:	4b05      	ldr	r3, [pc, #20]	; (3e8 <cycle10+0x18>)
 3d4:	20aa      	movs	r0, #170	; 0xaa
 3d6:	6819      	ldr	r1, [r3, #0]
 3d8:	f7ff feba 	bl	150 <mbus_write_message32>
 3dc:	20fa      	movs	r0, #250	; 0xfa
 3de:	0080      	lsls	r0, r0, #2
 3e0:	f7ff fe61 	bl	a6 <delay>
 3e4:	bd08      	pop	{r3, pc}
 3e6:	46c0      	nop			; (mov r8, r8)
 3e8:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle11:

000003ec <cycle11>:
 3ec:	b508      	push	{r3, lr}
 3ee:	4b05      	ldr	r3, [pc, #20]	; (404 <cycle11+0x18>)
 3f0:	20ab      	movs	r0, #171	; 0xab
 3f2:	6819      	ldr	r1, [r3, #0]
 3f4:	f7ff feac 	bl	150 <mbus_write_message32>
 3f8:	20fa      	movs	r0, #250	; 0xfa
 3fa:	0080      	lsls	r0, r0, #2
 3fc:	f7ff fe53 	bl	a6 <delay>
 400:	bd08      	pop	{r3, pc}
 402:	46c0      	nop			; (mov r8, r8)
 404:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle12:

00000408 <cycle12>:
 408:	b508      	push	{r3, lr}
 40a:	4b05      	ldr	r3, [pc, #20]	; (420 <cycle12+0x18>)
 40c:	20ac      	movs	r0, #172	; 0xac
 40e:	6819      	ldr	r1, [r3, #0]
 410:	f7ff fe9e 	bl	150 <mbus_write_message32>
 414:	20fa      	movs	r0, #250	; 0xfa
 416:	0080      	lsls	r0, r0, #2
 418:	f7ff fe45 	bl	a6 <delay>
 41c:	bd08      	pop	{r3, pc}
 41e:	46c0      	nop			; (mov r8, r8)
 420:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle13:

00000424 <cycle13>:
 424:	b508      	push	{r3, lr}
 426:	4b05      	ldr	r3, [pc, #20]	; (43c <cycle13+0x18>)
 428:	20ad      	movs	r0, #173	; 0xad
 42a:	6819      	ldr	r1, [r3, #0]
 42c:	f7ff fe90 	bl	150 <mbus_write_message32>
 430:	20fa      	movs	r0, #250	; 0xfa
 432:	0080      	lsls	r0, r0, #2
 434:	f7ff fe37 	bl	a6 <delay>
 438:	bd08      	pop	{r3, pc}
 43a:	46c0      	nop			; (mov r8, r8)
 43c:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle14:

00000440 <cycle14>:
 440:	b508      	push	{r3, lr}
 442:	4b05      	ldr	r3, [pc, #20]	; (458 <cycle14+0x18>)
 444:	20ae      	movs	r0, #174	; 0xae
 446:	6819      	ldr	r1, [r3, #0]
 448:	f7ff fe82 	bl	150 <mbus_write_message32>
 44c:	20fa      	movs	r0, #250	; 0xfa
 44e:	0080      	lsls	r0, r0, #2
 450:	f7ff fe29 	bl	a6 <delay>
 454:	bd08      	pop	{r3, pc}
 456:	46c0      	nop			; (mov r8, r8)
 458:	000005ac 	.word	0x000005ac

Disassembly of section .text.cycle15:

0000045c <cycle15>:
 45c:	b508      	push	{r3, lr}
 45e:	4b05      	ldr	r3, [pc, #20]	; (474 <cycle15+0x18>)
 460:	20af      	movs	r0, #175	; 0xaf
 462:	6819      	ldr	r1, [r3, #0]
 464:	f7ff fe74 	bl	150 <mbus_write_message32>
 468:	20fa      	movs	r0, #250	; 0xfa
 46a:	0080      	lsls	r0, r0, #2
 46c:	f7ff fe1b 	bl	a6 <delay>
 470:	bd08      	pop	{r3, pc}
 472:	46c0      	nop			; (mov r8, r8)
 474:	000005ac 	.word	0x000005ac

Disassembly of section .text.startup.main:

00000478 <main>:
 478:	b538      	push	{r3, r4, r5, lr}
 47a:	f7ff fe81 	bl	180 <init_interrupt>
 47e:	4b45      	ldr	r3, [pc, #276]	; (594 <main+0x11c>)
 480:	681a      	ldr	r2, [r3, #0]
 482:	4b45      	ldr	r3, [pc, #276]	; (598 <main+0x120>)
 484:	429a      	cmp	r2, r3
 486:	d001      	beq.n	48c <main+0x14>
 488:	f7ff ff00 	bl	28c <initialization>
 48c:	2400      	movs	r4, #0
 48e:	4d43      	ldr	r5, [pc, #268]	; (59c <main+0x124>)
 490:	682b      	ldr	r3, [r5, #0]
 492:	2b00      	cmp	r3, #0
 494:	d104      	bne.n	4a0 <main+0x28>
 496:	20fa      	movs	r0, #250	; 0xfa
 498:	0080      	lsls	r0, r0, #2
 49a:	f7ff fe04 	bl	a6 <delay>
 49e:	e057      	b.n	550 <main+0xd8>
 4a0:	230f      	movs	r3, #15
 4a2:	4023      	ands	r3, r4
 4a4:	d102      	bne.n	4ac <main+0x34>
 4a6:	f7ff ff07 	bl	2b8 <cycle0>
 4aa:	e047      	b.n	53c <main+0xc4>
 4ac:	2b01      	cmp	r3, #1
 4ae:	d102      	bne.n	4b6 <main+0x3e>
 4b0:	f7ff ff10 	bl	2d4 <cycle1>
 4b4:	e042      	b.n	53c <main+0xc4>
 4b6:	2b02      	cmp	r3, #2
 4b8:	d102      	bne.n	4c0 <main+0x48>
 4ba:	f7ff ff19 	bl	2f0 <cycle2>
 4be:	e03d      	b.n	53c <main+0xc4>
 4c0:	2b03      	cmp	r3, #3
 4c2:	d102      	bne.n	4ca <main+0x52>
 4c4:	f7ff ff22 	bl	30c <cycle3>
 4c8:	e038      	b.n	53c <main+0xc4>
 4ca:	2b04      	cmp	r3, #4
 4cc:	d102      	bne.n	4d4 <main+0x5c>
 4ce:	f7ff ff2b 	bl	328 <cycle4>
 4d2:	e033      	b.n	53c <main+0xc4>
 4d4:	2b05      	cmp	r3, #5
 4d6:	d102      	bne.n	4de <main+0x66>
 4d8:	f7ff ff34 	bl	344 <cycle5>
 4dc:	e02e      	b.n	53c <main+0xc4>
 4de:	2b06      	cmp	r3, #6
 4e0:	d102      	bne.n	4e8 <main+0x70>
 4e2:	f7ff ff3d 	bl	360 <cycle6>
 4e6:	e029      	b.n	53c <main+0xc4>
 4e8:	2b07      	cmp	r3, #7
 4ea:	d102      	bne.n	4f2 <main+0x7a>
 4ec:	f7ff ff46 	bl	37c <cycle7>
 4f0:	e024      	b.n	53c <main+0xc4>
 4f2:	2b08      	cmp	r3, #8
 4f4:	d102      	bne.n	4fc <main+0x84>
 4f6:	f7ff ff4f 	bl	398 <cycle8>
 4fa:	e01f      	b.n	53c <main+0xc4>
 4fc:	2b09      	cmp	r3, #9
 4fe:	d102      	bne.n	506 <main+0x8e>
 500:	f7ff ff58 	bl	3b4 <cycle9>
 504:	e01a      	b.n	53c <main+0xc4>
 506:	2b0a      	cmp	r3, #10
 508:	d102      	bne.n	510 <main+0x98>
 50a:	f7ff ff61 	bl	3d0 <cycle10>
 50e:	e015      	b.n	53c <main+0xc4>
 510:	2b0b      	cmp	r3, #11
 512:	d102      	bne.n	51a <main+0xa2>
 514:	f7ff ff6a 	bl	3ec <cycle11>
 518:	e010      	b.n	53c <main+0xc4>
 51a:	2b0c      	cmp	r3, #12
 51c:	d102      	bne.n	524 <main+0xac>
 51e:	f7ff ff73 	bl	408 <cycle12>
 522:	e00b      	b.n	53c <main+0xc4>
 524:	2b0d      	cmp	r3, #13
 526:	d102      	bne.n	52e <main+0xb6>
 528:	f7ff ff7c 	bl	424 <cycle13>
 52c:	e006      	b.n	53c <main+0xc4>
 52e:	2b0e      	cmp	r3, #14
 530:	d102      	bne.n	538 <main+0xc0>
 532:	f7ff ff85 	bl	440 <cycle14>
 536:	e001      	b.n	53c <main+0xc4>
 538:	f7ff ff90 	bl	45c <cycle15>
 53c:	682b      	ldr	r3, [r5, #0]
 53e:	20fa      	movs	r0, #250	; 0xfa
 540:	3301      	adds	r3, #1
 542:	0080      	lsls	r0, r0, #2
 544:	3401      	adds	r4, #1
 546:	602b      	str	r3, [r5, #0]
 548:	f7ff fdad 	bl	a6 <delay>
 54c:	2c64      	cmp	r4, #100	; 0x64
 54e:	d19e      	bne.n	48e <main+0x16>
 550:	4b12      	ldr	r3, [pc, #72]	; (59c <main+0x124>)
 552:	4a13      	ldr	r2, [pc, #76]	; (5a0 <main+0x128>)
 554:	6819      	ldr	r1, [r3, #0]
 556:	4291      	cmp	r1, r2
 558:	d108      	bne.n	56c <main+0xf4>
 55a:	20dd      	movs	r0, #221	; 0xdd
 55c:	4911      	ldr	r1, [pc, #68]	; (5a4 <main+0x12c>)
 55e:	f7ff fdf7 	bl	150 <mbus_write_message32>
 562:	20fa      	movs	r0, #250	; 0xfa
 564:	0080      	lsls	r0, r0, #2
 566:	f7ff fd9e 	bl	a6 <delay>
 56a:	e7fe      	b.n	56a <main+0xf2>
 56c:	681a      	ldr	r2, [r3, #0]
 56e:	490e      	ldr	r1, [pc, #56]	; (5a8 <main+0x130>)
 570:	3201      	adds	r2, #1
 572:	20dd      	movs	r0, #221	; 0xdd
 574:	601a      	str	r2, [r3, #0]
 576:	f7ff fdeb 	bl	150 <mbus_write_message32>
 57a:	20fa      	movs	r0, #250	; 0xfa
 57c:	0080      	lsls	r0, r0, #2
 57e:	f7ff fd92 	bl	a6 <delay>
 582:	2101      	movs	r1, #1
 584:	2002      	movs	r0, #2
 586:	1c0a      	adds	r2, r1, #0
 588:	f7ff fdaa 	bl	e0 <set_wakeup_timer>
 58c:	f7ff fdf2 	bl	174 <mbus_sleep_all>
 590:	2001      	movs	r0, #1
 592:	bd38      	pop	{r3, r4, r5, pc}
 594:	000005b0 	.word	0x000005b0
 598:	deadbeef 	.word	0xdeadbeef
 59c:	000005ac 	.word	0x000005ac
 5a0:	000003e7 	.word	0x000003e7
 5a4:	0ea70ea7 	.word	0x0ea70ea7
 5a8:	0ea7f00d 	.word	0x0ea7f00d
