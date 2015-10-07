
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
  40:	00000189 	.word	0x00000189
  44:	00000195 	.word	0x00000195
  48:	000001a1 	.word	0x000001a1
  4c:	000001ad 	.word	0x000001ad
  50:	000001b9 	.word	0x000001b9
  54:	000001c5 	.word	0x000001c5
  58:	000001d1 	.word	0x000001d1
  5c:	000001dd 	.word	0x000001dd
  60:	000001e9 	.word	0x000001e9
  64:	000001f9 	.word	0x000001f9
  68:	00000209 	.word	0x00000209
  6c:	00000219 	.word	0x00000219
  70:	00000229 	.word	0x00000229
  74:	00000239 	.word	0x00000239
  78:	00000249 	.word	0x00000249
  7c:	00000000 	.word	0x00000000

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 f906 	bl	2a0 <main>
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

Disassembly of section .text.write_regfile:

000000a8 <write_regfile>:
  a8:	0880      	lsrs	r0, r0, #2
  aa:	0209      	lsls	r1, r1, #8
  ac:	b508      	push	{r3, lr}
  ae:	0a09      	lsrs	r1, r1, #8
  b0:	4b05      	ldr	r3, [pc, #20]	; (c8 <write_regfile+0x20>)
  b2:	0600      	lsls	r0, r0, #24
  b4:	4308      	orrs	r0, r1
  b6:	6018      	str	r0, [r3, #0]
  b8:	4b04      	ldr	r3, [pc, #16]	; (cc <write_regfile+0x24>)
  ba:	2210      	movs	r2, #16
  bc:	601a      	str	r2, [r3, #0]
  be:	200a      	movs	r0, #10
  c0:	f7ff ffe9 	bl	96 <delay>
  c4:	bd08      	pop	{r3, pc}
  c6:	46c0      	nop			; (mov r8, r8)
  c8:	a0002000 	.word	0xa0002000
  cc:	a000200c 	.word	0xa000200c

Disassembly of section .text.set_wakeup_timer:

000000d0 <set_wakeup_timer>:
  d0:	b510      	push	{r4, lr}
  d2:	1c14      	adds	r4, r2, #0
  d4:	2900      	cmp	r1, #0
  d6:	d003      	beq.n	e0 <set_wakeup_timer+0x10>
  d8:	2180      	movs	r1, #128	; 0x80
  da:	0209      	lsls	r1, r1, #8
  dc:	4301      	orrs	r1, r0
  de:	e001      	b.n	e4 <set_wakeup_timer+0x14>
  e0:	0440      	lsls	r0, r0, #17
  e2:	0c41      	lsrs	r1, r0, #17
  e4:	4804      	ldr	r0, [pc, #16]	; (f8 <set_wakeup_timer+0x28>)
  e6:	f7ff ffdf 	bl	a8 <write_regfile>
  ea:	2c00      	cmp	r4, #0
  ec:	d002      	beq.n	f4 <set_wakeup_timer+0x24>
  ee:	4b03      	ldr	r3, [pc, #12]	; (fc <set_wakeup_timer+0x2c>)
  f0:	2201      	movs	r2, #1
  f2:	601a      	str	r2, [r3, #0]
  f4:	bd10      	pop	{r4, pc}
  f6:	46c0      	nop			; (mov r8, r8)
  f8:	a0000034 	.word	0xa0000034
  fc:	a0001300 	.word	0xa0001300

Disassembly of section .text.set_halt_until_mbus_rx:

00000100 <set_halt_until_mbus_rx>:
 100:	4805      	ldr	r0, [pc, #20]	; (118 <set_halt_until_mbus_rx+0x18>)
 102:	b508      	push	{r3, lr}
 104:	6801      	ldr	r1, [r0, #0]
 106:	4b05      	ldr	r3, [pc, #20]	; (11c <set_halt_until_mbus_rx+0x1c>)
 108:	4019      	ands	r1, r3
 10a:	2390      	movs	r3, #144	; 0x90
 10c:	021b      	lsls	r3, r3, #8
 10e:	4319      	orrs	r1, r3
 110:	f7ff ffca 	bl	a8 <write_regfile>
 114:	bd08      	pop	{r3, pc}
 116:	46c0      	nop			; (mov r8, r8)
 118:	a0000028 	.word	0xa0000028
 11c:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.set_halt_until_mbus_tx:

00000120 <set_halt_until_mbus_tx>:
 120:	4805      	ldr	r0, [pc, #20]	; (138 <set_halt_until_mbus_tx+0x18>)
 122:	b508      	push	{r3, lr}
 124:	6801      	ldr	r1, [r0, #0]
 126:	4b05      	ldr	r3, [pc, #20]	; (13c <set_halt_until_mbus_tx+0x1c>)
 128:	4019      	ands	r1, r3
 12a:	23a0      	movs	r3, #160	; 0xa0
 12c:	021b      	lsls	r3, r3, #8
 12e:	4319      	orrs	r1, r3
 130:	f7ff ffba 	bl	a8 <write_regfile>
 134:	bd08      	pop	{r3, pc}
 136:	46c0      	nop			; (mov r8, r8)
 138:	a0000028 	.word	0xa0000028
 13c:	ffff0fff 	.word	0xffff0fff

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
 170:	4b02      	ldr	r3, [pc, #8]	; (17c <init_interrupt+0xc>)
 172:	4a03      	ldr	r2, [pc, #12]	; (180 <init_interrupt+0x10>)
 174:	6013      	str	r3, [r2, #0]
 176:	4a03      	ldr	r2, [pc, #12]	; (184 <init_interrupt+0x14>)
 178:	6013      	str	r3, [r2, #0]
 17a:	4770      	bx	lr
 17c:	00007fff 	.word	0x00007fff
 180:	e000e280 	.word	0xe000e280
 184:	e000e100 	.word	0xe000e100

Disassembly of section .text.handler_ext_int_0:

00000188 <handler_ext_int_0>:
 188:	4b01      	ldr	r3, [pc, #4]	; (190 <handler_ext_int_0+0x8>)
 18a:	2201      	movs	r2, #1
 18c:	601a      	str	r2, [r3, #0]
 18e:	4770      	bx	lr
 190:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

00000194 <handler_ext_int_1>:
 194:	4b01      	ldr	r3, [pc, #4]	; (19c <handler_ext_int_1+0x8>)
 196:	2202      	movs	r2, #2
 198:	601a      	str	r2, [r3, #0]
 19a:	4770      	bx	lr
 19c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

000001a0 <handler_ext_int_2>:
 1a0:	4b01      	ldr	r3, [pc, #4]	; (1a8 <handler_ext_int_2+0x8>)
 1a2:	2204      	movs	r2, #4
 1a4:	601a      	str	r2, [r3, #0]
 1a6:	4770      	bx	lr
 1a8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

000001ac <handler_ext_int_3>:
 1ac:	4b01      	ldr	r3, [pc, #4]	; (1b4 <handler_ext_int_3+0x8>)
 1ae:	2208      	movs	r2, #8
 1b0:	601a      	str	r2, [r3, #0]
 1b2:	4770      	bx	lr
 1b4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

000001b8 <handler_ext_int_4>:
 1b8:	4b01      	ldr	r3, [pc, #4]	; (1c0 <handler_ext_int_4+0x8>)
 1ba:	2210      	movs	r2, #16
 1bc:	601a      	str	r2, [r3, #0]
 1be:	4770      	bx	lr
 1c0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

000001c4 <handler_ext_int_5>:
 1c4:	4b01      	ldr	r3, [pc, #4]	; (1cc <handler_ext_int_5+0x8>)
 1c6:	2220      	movs	r2, #32
 1c8:	601a      	str	r2, [r3, #0]
 1ca:	4770      	bx	lr
 1cc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

000001d0 <handler_ext_int_6>:
 1d0:	4b01      	ldr	r3, [pc, #4]	; (1d8 <handler_ext_int_6+0x8>)
 1d2:	2240      	movs	r2, #64	; 0x40
 1d4:	601a      	str	r2, [r3, #0]
 1d6:	4770      	bx	lr
 1d8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

000001dc <handler_ext_int_7>:
 1dc:	4b01      	ldr	r3, [pc, #4]	; (1e4 <handler_ext_int_7+0x8>)
 1de:	2280      	movs	r2, #128	; 0x80
 1e0:	601a      	str	r2, [r3, #0]
 1e2:	4770      	bx	lr
 1e4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

000001e8 <handler_ext_int_8>:
 1e8:	4b02      	ldr	r3, [pc, #8]	; (1f4 <handler_ext_int_8+0xc>)
 1ea:	2280      	movs	r2, #128	; 0x80
 1ec:	0052      	lsls	r2, r2, #1
 1ee:	601a      	str	r2, [r3, #0]
 1f0:	4770      	bx	lr
 1f2:	46c0      	nop			; (mov r8, r8)
 1f4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

000001f8 <handler_ext_int_9>:
 1f8:	4b02      	ldr	r3, [pc, #8]	; (204 <handler_ext_int_9+0xc>)
 1fa:	2280      	movs	r2, #128	; 0x80
 1fc:	0092      	lsls	r2, r2, #2
 1fe:	601a      	str	r2, [r3, #0]
 200:	4770      	bx	lr
 202:	46c0      	nop			; (mov r8, r8)
 204:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

00000208 <handler_ext_int_10>:
 208:	4b02      	ldr	r3, [pc, #8]	; (214 <handler_ext_int_10+0xc>)
 20a:	2280      	movs	r2, #128	; 0x80
 20c:	00d2      	lsls	r2, r2, #3
 20e:	601a      	str	r2, [r3, #0]
 210:	4770      	bx	lr
 212:	46c0      	nop			; (mov r8, r8)
 214:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

00000218 <handler_ext_int_11>:
 218:	4b02      	ldr	r3, [pc, #8]	; (224 <handler_ext_int_11+0xc>)
 21a:	2280      	movs	r2, #128	; 0x80
 21c:	0112      	lsls	r2, r2, #4
 21e:	601a      	str	r2, [r3, #0]
 220:	4770      	bx	lr
 222:	46c0      	nop			; (mov r8, r8)
 224:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

00000228 <handler_ext_int_12>:
 228:	4b02      	ldr	r3, [pc, #8]	; (234 <handler_ext_int_12+0xc>)
 22a:	2280      	movs	r2, #128	; 0x80
 22c:	0152      	lsls	r2, r2, #5
 22e:	601a      	str	r2, [r3, #0]
 230:	4770      	bx	lr
 232:	46c0      	nop			; (mov r8, r8)
 234:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

00000238 <handler_ext_int_13>:
 238:	4b02      	ldr	r3, [pc, #8]	; (244 <handler_ext_int_13+0xc>)
 23a:	2280      	movs	r2, #128	; 0x80
 23c:	0192      	lsls	r2, r2, #6
 23e:	601a      	str	r2, [r3, #0]
 240:	4770      	bx	lr
 242:	46c0      	nop			; (mov r8, r8)
 244:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

00000248 <handler_ext_int_14>:
 248:	4b02      	ldr	r3, [pc, #8]	; (254 <handler_ext_int_14+0xc>)
 24a:	2280      	movs	r2, #128	; 0x80
 24c:	01d2      	lsls	r2, r2, #7
 24e:	601a      	str	r2, [r3, #0]
 250:	4770      	bx	lr
 252:	46c0      	nop			; (mov r8, r8)
 254:	e000e280 	.word	0xe000e280

Disassembly of section .text.initialization:

00000258 <initialization>:
 258:	b508      	push	{r3, lr}
 25a:	4a0b      	ldr	r2, [pc, #44]	; (288 <initialization+0x30>)
 25c:	4b0b      	ldr	r3, [pc, #44]	; (28c <initialization+0x34>)
 25e:	480c      	ldr	r0, [pc, #48]	; (290 <initialization+0x38>)
 260:	601a      	str	r2, [r3, #0]
 262:	4b0c      	ldr	r3, [pc, #48]	; (294 <initialization+0x3c>)
 264:	2200      	movs	r2, #0
 266:	490c      	ldr	r1, [pc, #48]	; (298 <initialization+0x40>)
 268:	601a      	str	r2, [r3, #0]
 26a:	f7ff ff1d 	bl	a8 <write_regfile>
 26e:	490b      	ldr	r1, [pc, #44]	; (29c <initialization+0x44>)
 270:	20aa      	movs	r0, #170	; 0xaa
 272:	f7ff ff65 	bl	140 <mbus_write_message32>
 276:	f7ff ff43 	bl	100 <set_halt_until_mbus_rx>
 27a:	2004      	movs	r0, #4
 27c:	f7ff ff68 	bl	150 <mbus_enumerate>
 280:	f7ff ff4e 	bl	120 <set_halt_until_mbus_tx>
 284:	bd08      	pop	{r3, pc}
 286:	46c0      	nop			; (mov r8, r8)
 288:	deadbeef 	.word	0xdeadbeef
 28c:	00000310 	.word	0x00000310
 290:	a0000020 	.word	0xa0000020
 294:	00000314 	.word	0x00000314
 298:	0000dead 	.word	0x0000dead
 29c:	aaaaaaaa 	.word	0xaaaaaaaa

Disassembly of section .text.startup.main:

000002a0 <main>:
 2a0:	b510      	push	{r4, lr}
 2a2:	f7ff ff65 	bl	170 <init_interrupt>
 2a6:	4b14      	ldr	r3, [pc, #80]	; (2f8 <main+0x58>)
 2a8:	681a      	ldr	r2, [r3, #0]
 2aa:	4b14      	ldr	r3, [pc, #80]	; (2fc <main+0x5c>)
 2ac:	429a      	cmp	r2, r3
 2ae:	d001      	beq.n	2b4 <main+0x14>
 2b0:	f7ff ffd2 	bl	258 <initialization>
 2b4:	4c12      	ldr	r4, [pc, #72]	; (300 <main+0x60>)
 2b6:	20ef      	movs	r0, #239	; 0xef
 2b8:	6821      	ldr	r1, [r4, #0]
 2ba:	f7ff ff41 	bl	140 <mbus_write_message32>
 2be:	6823      	ldr	r3, [r4, #0]
 2c0:	2b00      	cmp	r3, #0
 2c2:	d102      	bne.n	2ca <main+0x2a>
 2c4:	20ef      	movs	r0, #239	; 0xef
 2c6:	490f      	ldr	r1, [pc, #60]	; (304 <main+0x64>)
 2c8:	e004      	b.n	2d4 <main+0x34>
 2ca:	6823      	ldr	r3, [r4, #0]
 2cc:	20ef      	movs	r0, #239	; 0xef
 2ce:	2b01      	cmp	r3, #1
 2d0:	d103      	bne.n	2da <main+0x3a>
 2d2:	490d      	ldr	r1, [pc, #52]	; (308 <main+0x68>)
 2d4:	f7ff ff34 	bl	140 <mbus_write_message32>
 2d8:	e003      	b.n	2e2 <main+0x42>
 2da:	490c      	ldr	r1, [pc, #48]	; (30c <main+0x6c>)
 2dc:	f7ff ff30 	bl	140 <mbus_write_message32>
 2e0:	e7fe      	b.n	2e0 <main+0x40>
 2e2:	6823      	ldr	r3, [r4, #0]
 2e4:	2101      	movs	r1, #1
 2e6:	3301      	adds	r3, #1
 2e8:	2005      	movs	r0, #5
 2ea:	1c0a      	adds	r2, r1, #0
 2ec:	6023      	str	r3, [r4, #0]
 2ee:	f7ff feef 	bl	d0 <set_wakeup_timer>
 2f2:	f7ff ff37 	bl	164 <mbus_sleep_all>
 2f6:	e7fe      	b.n	2f6 <main+0x56>
 2f8:	00000310 	.word	0x00000310
 2fc:	deadbeef 	.word	0xdeadbeef
 300:	00000314 	.word	0x00000314
 304:	aaaaaaaa 	.word	0xaaaaaaaa
 308:	bbbbbbbb 	.word	0xbbbbbbbb
 30c:	0ea7f00d 	.word	0x0ea7f00d
