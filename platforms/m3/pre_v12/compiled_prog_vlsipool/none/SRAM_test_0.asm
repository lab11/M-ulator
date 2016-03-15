
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
  a0:	f000 f9ca 	bl	438 <main>
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

Disassembly of section .text.set_wakeup_timer:

00000110 <set_wakeup_timer>:
 110:	b510      	push	{r4, lr}
 112:	1c14      	adds	r4, r2, #0
 114:	2900      	cmp	r1, #0
 116:	d003      	beq.n	120 <set_wakeup_timer+0x10>
 118:	2180      	movs	r1, #128	; 0x80
 11a:	0209      	lsls	r1, r1, #8
 11c:	4301      	orrs	r1, r0
 11e:	e001      	b.n	124 <set_wakeup_timer+0x14>
 120:	0440      	lsls	r0, r0, #17
 122:	0c41      	lsrs	r1, r0, #17
 124:	4804      	ldr	r0, [pc, #16]	; (138 <set_wakeup_timer+0x28>)
 126:	f7ff ffdf 	bl	e8 <write_regfile>
 12a:	2c00      	cmp	r4, #0
 12c:	d002      	beq.n	134 <set_wakeup_timer+0x24>
 12e:	4b03      	ldr	r3, [pc, #12]	; (13c <set_wakeup_timer+0x2c>)
 130:	2201      	movs	r2, #1
 132:	601a      	str	r2, [r3, #0]
 134:	bd10      	pop	{r4, pc}
 136:	46c0      	nop			; (mov r8, r8)
 138:	a0000034 	.word	0xa0000034
 13c:	a0001300 	.word	0xa0001300

Disassembly of section .text.init_interrupt:

00000140 <init_interrupt>:
 140:	4a03      	ldr	r2, [pc, #12]	; (150 <init_interrupt+0x10>)
 142:	4b04      	ldr	r3, [pc, #16]	; (154 <init_interrupt+0x14>)
 144:	601a      	str	r2, [r3, #0]
 146:	4b04      	ldr	r3, [pc, #16]	; (158 <init_interrupt+0x18>)
 148:	2200      	movs	r2, #0
 14a:	601a      	str	r2, [r3, #0]
 14c:	4770      	bx	lr
 14e:	46c0      	nop			; (mov r8, r8)
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
 24c:	b508      	push	{r3, lr}
 24e:	4a07      	ldr	r2, [pc, #28]	; (26c <initialization+0x20>)
 250:	4b07      	ldr	r3, [pc, #28]	; (270 <initialization+0x24>)
 252:	2004      	movs	r0, #4
 254:	601a      	str	r2, [r3, #0]
 256:	4b07      	ldr	r3, [pc, #28]	; (274 <initialization+0x28>)
 258:	2200      	movs	r2, #0
 25a:	601a      	str	r2, [r3, #0]
 25c:	f7ff ff2c 	bl	b8 <mbus_enumerate>
 260:	20fa      	movs	r0, #250	; 0xfa
 262:	0080      	lsls	r0, r0, #2
 264:	f7ff ff38 	bl	d8 <delay>
 268:	bd08      	pop	{r3, pc}
 26a:	46c0      	nop			; (mov r8, r8)
 26c:	deadbeef 	.word	0xdeadbeef
 270:	00000570 	.word	0x00000570
 274:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle0:

00000278 <cycle0>:
 278:	b508      	push	{r3, lr}
 27a:	4b05      	ldr	r3, [pc, #20]	; (290 <cycle0+0x18>)
 27c:	20a0      	movs	r0, #160	; 0xa0
 27e:	6819      	ldr	r1, [r3, #0]
 280:	f7ff ff12 	bl	a8 <mbus_write_message32>
 284:	20fa      	movs	r0, #250	; 0xfa
 286:	0080      	lsls	r0, r0, #2
 288:	f7ff ff26 	bl	d8 <delay>
 28c:	bd08      	pop	{r3, pc}
 28e:	46c0      	nop			; (mov r8, r8)
 290:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle1:

00000294 <cycle1>:
 294:	b508      	push	{r3, lr}
 296:	4b05      	ldr	r3, [pc, #20]	; (2ac <cycle1+0x18>)
 298:	20a1      	movs	r0, #161	; 0xa1
 29a:	6819      	ldr	r1, [r3, #0]
 29c:	f7ff ff04 	bl	a8 <mbus_write_message32>
 2a0:	20fa      	movs	r0, #250	; 0xfa
 2a2:	0080      	lsls	r0, r0, #2
 2a4:	f7ff ff18 	bl	d8 <delay>
 2a8:	bd08      	pop	{r3, pc}
 2aa:	46c0      	nop			; (mov r8, r8)
 2ac:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle2:

000002b0 <cycle2>:
 2b0:	b508      	push	{r3, lr}
 2b2:	4b05      	ldr	r3, [pc, #20]	; (2c8 <cycle2+0x18>)
 2b4:	20a2      	movs	r0, #162	; 0xa2
 2b6:	6819      	ldr	r1, [r3, #0]
 2b8:	f7ff fef6 	bl	a8 <mbus_write_message32>
 2bc:	20fa      	movs	r0, #250	; 0xfa
 2be:	0080      	lsls	r0, r0, #2
 2c0:	f7ff ff0a 	bl	d8 <delay>
 2c4:	bd08      	pop	{r3, pc}
 2c6:	46c0      	nop			; (mov r8, r8)
 2c8:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle3:

000002cc <cycle3>:
 2cc:	b508      	push	{r3, lr}
 2ce:	4b05      	ldr	r3, [pc, #20]	; (2e4 <cycle3+0x18>)
 2d0:	20a3      	movs	r0, #163	; 0xa3
 2d2:	6819      	ldr	r1, [r3, #0]
 2d4:	f7ff fee8 	bl	a8 <mbus_write_message32>
 2d8:	20fa      	movs	r0, #250	; 0xfa
 2da:	0080      	lsls	r0, r0, #2
 2dc:	f7ff fefc 	bl	d8 <delay>
 2e0:	bd08      	pop	{r3, pc}
 2e2:	46c0      	nop			; (mov r8, r8)
 2e4:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle4:

000002e8 <cycle4>:
 2e8:	b508      	push	{r3, lr}
 2ea:	4b05      	ldr	r3, [pc, #20]	; (300 <cycle4+0x18>)
 2ec:	20a4      	movs	r0, #164	; 0xa4
 2ee:	6819      	ldr	r1, [r3, #0]
 2f0:	f7ff feda 	bl	a8 <mbus_write_message32>
 2f4:	20fa      	movs	r0, #250	; 0xfa
 2f6:	0080      	lsls	r0, r0, #2
 2f8:	f7ff feee 	bl	d8 <delay>
 2fc:	bd08      	pop	{r3, pc}
 2fe:	46c0      	nop			; (mov r8, r8)
 300:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle5:

00000304 <cycle5>:
 304:	b508      	push	{r3, lr}
 306:	4b05      	ldr	r3, [pc, #20]	; (31c <cycle5+0x18>)
 308:	20a5      	movs	r0, #165	; 0xa5
 30a:	6819      	ldr	r1, [r3, #0]
 30c:	f7ff fecc 	bl	a8 <mbus_write_message32>
 310:	20fa      	movs	r0, #250	; 0xfa
 312:	0080      	lsls	r0, r0, #2
 314:	f7ff fee0 	bl	d8 <delay>
 318:	bd08      	pop	{r3, pc}
 31a:	46c0      	nop			; (mov r8, r8)
 31c:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle6:

00000320 <cycle6>:
 320:	b508      	push	{r3, lr}
 322:	4b05      	ldr	r3, [pc, #20]	; (338 <cycle6+0x18>)
 324:	20a6      	movs	r0, #166	; 0xa6
 326:	6819      	ldr	r1, [r3, #0]
 328:	f7ff febe 	bl	a8 <mbus_write_message32>
 32c:	20fa      	movs	r0, #250	; 0xfa
 32e:	0080      	lsls	r0, r0, #2
 330:	f7ff fed2 	bl	d8 <delay>
 334:	bd08      	pop	{r3, pc}
 336:	46c0      	nop			; (mov r8, r8)
 338:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle7:

0000033c <cycle7>:
 33c:	b508      	push	{r3, lr}
 33e:	4b05      	ldr	r3, [pc, #20]	; (354 <cycle7+0x18>)
 340:	20a7      	movs	r0, #167	; 0xa7
 342:	6819      	ldr	r1, [r3, #0]
 344:	f7ff feb0 	bl	a8 <mbus_write_message32>
 348:	20fa      	movs	r0, #250	; 0xfa
 34a:	0080      	lsls	r0, r0, #2
 34c:	f7ff fec4 	bl	d8 <delay>
 350:	bd08      	pop	{r3, pc}
 352:	46c0      	nop			; (mov r8, r8)
 354:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle8:

00000358 <cycle8>:
 358:	b508      	push	{r3, lr}
 35a:	4b05      	ldr	r3, [pc, #20]	; (370 <cycle8+0x18>)
 35c:	20a8      	movs	r0, #168	; 0xa8
 35e:	6819      	ldr	r1, [r3, #0]
 360:	f7ff fea2 	bl	a8 <mbus_write_message32>
 364:	20fa      	movs	r0, #250	; 0xfa
 366:	0080      	lsls	r0, r0, #2
 368:	f7ff feb6 	bl	d8 <delay>
 36c:	bd08      	pop	{r3, pc}
 36e:	46c0      	nop			; (mov r8, r8)
 370:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle9:

00000374 <cycle9>:
 374:	b508      	push	{r3, lr}
 376:	4b05      	ldr	r3, [pc, #20]	; (38c <cycle9+0x18>)
 378:	20a9      	movs	r0, #169	; 0xa9
 37a:	6819      	ldr	r1, [r3, #0]
 37c:	f7ff fe94 	bl	a8 <mbus_write_message32>
 380:	20fa      	movs	r0, #250	; 0xfa
 382:	0080      	lsls	r0, r0, #2
 384:	f7ff fea8 	bl	d8 <delay>
 388:	bd08      	pop	{r3, pc}
 38a:	46c0      	nop			; (mov r8, r8)
 38c:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle10:

00000390 <cycle10>:
 390:	b508      	push	{r3, lr}
 392:	4b05      	ldr	r3, [pc, #20]	; (3a8 <cycle10+0x18>)
 394:	20aa      	movs	r0, #170	; 0xaa
 396:	6819      	ldr	r1, [r3, #0]
 398:	f7ff fe86 	bl	a8 <mbus_write_message32>
 39c:	20fa      	movs	r0, #250	; 0xfa
 39e:	0080      	lsls	r0, r0, #2
 3a0:	f7ff fe9a 	bl	d8 <delay>
 3a4:	bd08      	pop	{r3, pc}
 3a6:	46c0      	nop			; (mov r8, r8)
 3a8:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle11:

000003ac <cycle11>:
 3ac:	b508      	push	{r3, lr}
 3ae:	4b05      	ldr	r3, [pc, #20]	; (3c4 <cycle11+0x18>)
 3b0:	20ab      	movs	r0, #171	; 0xab
 3b2:	6819      	ldr	r1, [r3, #0]
 3b4:	f7ff fe78 	bl	a8 <mbus_write_message32>
 3b8:	20fa      	movs	r0, #250	; 0xfa
 3ba:	0080      	lsls	r0, r0, #2
 3bc:	f7ff fe8c 	bl	d8 <delay>
 3c0:	bd08      	pop	{r3, pc}
 3c2:	46c0      	nop			; (mov r8, r8)
 3c4:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle12:

000003c8 <cycle12>:
 3c8:	b508      	push	{r3, lr}
 3ca:	4b05      	ldr	r3, [pc, #20]	; (3e0 <cycle12+0x18>)
 3cc:	20ac      	movs	r0, #172	; 0xac
 3ce:	6819      	ldr	r1, [r3, #0]
 3d0:	f7ff fe6a 	bl	a8 <mbus_write_message32>
 3d4:	20fa      	movs	r0, #250	; 0xfa
 3d6:	0080      	lsls	r0, r0, #2
 3d8:	f7ff fe7e 	bl	d8 <delay>
 3dc:	bd08      	pop	{r3, pc}
 3de:	46c0      	nop			; (mov r8, r8)
 3e0:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle13:

000003e4 <cycle13>:
 3e4:	b508      	push	{r3, lr}
 3e6:	4b05      	ldr	r3, [pc, #20]	; (3fc <cycle13+0x18>)
 3e8:	20ad      	movs	r0, #173	; 0xad
 3ea:	6819      	ldr	r1, [r3, #0]
 3ec:	f7ff fe5c 	bl	a8 <mbus_write_message32>
 3f0:	20fa      	movs	r0, #250	; 0xfa
 3f2:	0080      	lsls	r0, r0, #2
 3f4:	f7ff fe70 	bl	d8 <delay>
 3f8:	bd08      	pop	{r3, pc}
 3fa:	46c0      	nop			; (mov r8, r8)
 3fc:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle14:

00000400 <cycle14>:
 400:	b508      	push	{r3, lr}
 402:	4b05      	ldr	r3, [pc, #20]	; (418 <cycle14+0x18>)
 404:	20ae      	movs	r0, #174	; 0xae
 406:	6819      	ldr	r1, [r3, #0]
 408:	f7ff fe4e 	bl	a8 <mbus_write_message32>
 40c:	20fa      	movs	r0, #250	; 0xfa
 40e:	0080      	lsls	r0, r0, #2
 410:	f7ff fe62 	bl	d8 <delay>
 414:	bd08      	pop	{r3, pc}
 416:	46c0      	nop			; (mov r8, r8)
 418:	0000056c 	.word	0x0000056c

Disassembly of section .text.cycle15:

0000041c <cycle15>:
 41c:	b508      	push	{r3, lr}
 41e:	4b05      	ldr	r3, [pc, #20]	; (434 <cycle15+0x18>)
 420:	20af      	movs	r0, #175	; 0xaf
 422:	6819      	ldr	r1, [r3, #0]
 424:	f7ff fe40 	bl	a8 <mbus_write_message32>
 428:	20fa      	movs	r0, #250	; 0xfa
 42a:	0080      	lsls	r0, r0, #2
 42c:	f7ff fe54 	bl	d8 <delay>
 430:	bd08      	pop	{r3, pc}
 432:	46c0      	nop			; (mov r8, r8)
 434:	0000056c 	.word	0x0000056c

Disassembly of section .text.startup.main:

00000438 <main>:
 438:	b538      	push	{r3, r4, r5, lr}
 43a:	f7ff fe81 	bl	140 <init_interrupt>
 43e:	4b45      	ldr	r3, [pc, #276]	; (554 <main+0x11c>)
 440:	681a      	ldr	r2, [r3, #0]
 442:	4b45      	ldr	r3, [pc, #276]	; (558 <main+0x120>)
 444:	429a      	cmp	r2, r3
 446:	d001      	beq.n	44c <main+0x14>
 448:	f7ff ff00 	bl	24c <initialization>
 44c:	2400      	movs	r4, #0
 44e:	4d43      	ldr	r5, [pc, #268]	; (55c <main+0x124>)
 450:	682b      	ldr	r3, [r5, #0]
 452:	2b00      	cmp	r3, #0
 454:	d104      	bne.n	460 <main+0x28>
 456:	20fa      	movs	r0, #250	; 0xfa
 458:	0080      	lsls	r0, r0, #2
 45a:	f7ff fe3d 	bl	d8 <delay>
 45e:	e057      	b.n	510 <main+0xd8>
 460:	230f      	movs	r3, #15
 462:	4023      	ands	r3, r4
 464:	d102      	bne.n	46c <main+0x34>
 466:	f7ff ff07 	bl	278 <cycle0>
 46a:	e047      	b.n	4fc <main+0xc4>
 46c:	2b01      	cmp	r3, #1
 46e:	d102      	bne.n	476 <main+0x3e>
 470:	f7ff ff10 	bl	294 <cycle1>
 474:	e042      	b.n	4fc <main+0xc4>
 476:	2b02      	cmp	r3, #2
 478:	d102      	bne.n	480 <main+0x48>
 47a:	f7ff ff19 	bl	2b0 <cycle2>
 47e:	e03d      	b.n	4fc <main+0xc4>
 480:	2b03      	cmp	r3, #3
 482:	d102      	bne.n	48a <main+0x52>
 484:	f7ff ff22 	bl	2cc <cycle3>
 488:	e038      	b.n	4fc <main+0xc4>
 48a:	2b04      	cmp	r3, #4
 48c:	d102      	bne.n	494 <main+0x5c>
 48e:	f7ff ff2b 	bl	2e8 <cycle4>
 492:	e033      	b.n	4fc <main+0xc4>
 494:	2b05      	cmp	r3, #5
 496:	d102      	bne.n	49e <main+0x66>
 498:	f7ff ff34 	bl	304 <cycle5>
 49c:	e02e      	b.n	4fc <main+0xc4>
 49e:	2b06      	cmp	r3, #6
 4a0:	d102      	bne.n	4a8 <main+0x70>
 4a2:	f7ff ff3d 	bl	320 <cycle6>
 4a6:	e029      	b.n	4fc <main+0xc4>
 4a8:	2b07      	cmp	r3, #7
 4aa:	d102      	bne.n	4b2 <main+0x7a>
 4ac:	f7ff ff46 	bl	33c <cycle7>
 4b0:	e024      	b.n	4fc <main+0xc4>
 4b2:	2b08      	cmp	r3, #8
 4b4:	d102      	bne.n	4bc <main+0x84>
 4b6:	f7ff ff4f 	bl	358 <cycle8>
 4ba:	e01f      	b.n	4fc <main+0xc4>
 4bc:	2b09      	cmp	r3, #9
 4be:	d102      	bne.n	4c6 <main+0x8e>
 4c0:	f7ff ff58 	bl	374 <cycle9>
 4c4:	e01a      	b.n	4fc <main+0xc4>
 4c6:	2b0a      	cmp	r3, #10
 4c8:	d102      	bne.n	4d0 <main+0x98>
 4ca:	f7ff ff61 	bl	390 <cycle10>
 4ce:	e015      	b.n	4fc <main+0xc4>
 4d0:	2b0b      	cmp	r3, #11
 4d2:	d102      	bne.n	4da <main+0xa2>
 4d4:	f7ff ff6a 	bl	3ac <cycle11>
 4d8:	e010      	b.n	4fc <main+0xc4>
 4da:	2b0c      	cmp	r3, #12
 4dc:	d102      	bne.n	4e4 <main+0xac>
 4de:	f7ff ff73 	bl	3c8 <cycle12>
 4e2:	e00b      	b.n	4fc <main+0xc4>
 4e4:	2b0d      	cmp	r3, #13
 4e6:	d102      	bne.n	4ee <main+0xb6>
 4e8:	f7ff ff7c 	bl	3e4 <cycle13>
 4ec:	e006      	b.n	4fc <main+0xc4>
 4ee:	2b0e      	cmp	r3, #14
 4f0:	d102      	bne.n	4f8 <main+0xc0>
 4f2:	f7ff ff85 	bl	400 <cycle14>
 4f6:	e001      	b.n	4fc <main+0xc4>
 4f8:	f7ff ff90 	bl	41c <cycle15>
 4fc:	682b      	ldr	r3, [r5, #0]
 4fe:	20fa      	movs	r0, #250	; 0xfa
 500:	3301      	adds	r3, #1
 502:	0080      	lsls	r0, r0, #2
 504:	3401      	adds	r4, #1
 506:	602b      	str	r3, [r5, #0]
 508:	f7ff fde6 	bl	d8 <delay>
 50c:	2c64      	cmp	r4, #100	; 0x64
 50e:	d19e      	bne.n	44e <main+0x16>
 510:	4b12      	ldr	r3, [pc, #72]	; (55c <main+0x124>)
 512:	4a13      	ldr	r2, [pc, #76]	; (560 <main+0x128>)
 514:	6819      	ldr	r1, [r3, #0]
 516:	4291      	cmp	r1, r2
 518:	d108      	bne.n	52c <main+0xf4>
 51a:	20dd      	movs	r0, #221	; 0xdd
 51c:	4911      	ldr	r1, [pc, #68]	; (564 <main+0x12c>)
 51e:	f7ff fdc3 	bl	a8 <mbus_write_message32>
 522:	20fa      	movs	r0, #250	; 0xfa
 524:	0080      	lsls	r0, r0, #2
 526:	f7ff fdd7 	bl	d8 <delay>
 52a:	e7fe      	b.n	52a <main+0xf2>
 52c:	681a      	ldr	r2, [r3, #0]
 52e:	490e      	ldr	r1, [pc, #56]	; (568 <main+0x130>)
 530:	3201      	adds	r2, #1
 532:	20dd      	movs	r0, #221	; 0xdd
 534:	601a      	str	r2, [r3, #0]
 536:	f7ff fdb7 	bl	a8 <mbus_write_message32>
 53a:	20fa      	movs	r0, #250	; 0xfa
 53c:	0080      	lsls	r0, r0, #2
 53e:	f7ff fdcb 	bl	d8 <delay>
 542:	2101      	movs	r1, #1
 544:	2002      	movs	r0, #2
 546:	1c0a      	adds	r2, r1, #0
 548:	f7ff fde2 	bl	110 <set_wakeup_timer>
 54c:	f7ff fdbe 	bl	cc <mbus_sleep_all>
 550:	2001      	movs	r0, #1
 552:	bd38      	pop	{r3, r4, r5, pc}
 554:	00000570 	.word	0x00000570
 558:	deadbeef 	.word	0xdeadbeef
 55c:	0000056c 	.word	0x0000056c
 560:	000003e7 	.word	0x000003e7
 564:	0ea70ea7 	.word	0x0ea70ea7
 568:	0ea7f00d 	.word	0x0ea7f00d
