
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
  40:	00000205 	.word	0x00000205
  44:	00000211 	.word	0x00000211
  48:	0000021d 	.word	0x0000021d
  4c:	00000229 	.word	0x00000229
  50:	00000235 	.word	0x00000235
  54:	00000241 	.word	0x00000241
  58:	0000024d 	.word	0x0000024d
  5c:	00000259 	.word	0x00000259
  60:	00000265 	.word	0x00000265
  64:	00000275 	.word	0x00000275
  68:	00000285 	.word	0x00000285
  6c:	00000295 	.word	0x00000295
  70:	000002a5 	.word	0x000002a5
  74:	000002b5 	.word	0x000002b5
  78:	000002c5 	.word	0x000002c5
  7c:	000002d5 	.word	0x000002d5
  80:	000002e5 	.word	0x000002e5
	...

00000090 <hang>:
  90:	e7fe      	b.n	90 <hang>
	...

000000a0 <_start>:
  a0:	f000 f946 	bl	330 <main>
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

Disassembly of section .text.mbus_copy_registers_from_remote_to_local:

00000174 <mbus_copy_registers_from_remote_to_local>:
 174:	b507      	push	{r0, r1, r2, lr}
 176:	0609      	lsls	r1, r1, #24
 178:	430a      	orrs	r2, r1
 17a:	2180      	movs	r1, #128	; 0x80
 17c:	0149      	lsls	r1, r1, #5
 17e:	430a      	orrs	r2, r1
 180:	041b      	lsls	r3, r3, #16
 182:	431a      	orrs	r2, r3
 184:	9201      	str	r2, [sp, #4]
 186:	0100      	lsls	r0, r0, #4
 188:	2201      	movs	r2, #1
 18a:	4310      	orrs	r0, r2
 18c:	b2c0      	uxtb	r0, r0
 18e:	a901      	add	r1, sp, #4
 190:	f7ff ffce 	bl	130 <mbus_write_message>
 194:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_remote_register_write:

00000196 <mbus_remote_register_write>:
 196:	b507      	push	{r0, r1, r2, lr}
 198:	0212      	lsls	r2, r2, #8
 19a:	0a12      	lsrs	r2, r2, #8
 19c:	0609      	lsls	r1, r1, #24
 19e:	4311      	orrs	r1, r2
 1a0:	0100      	lsls	r0, r0, #4
 1a2:	9101      	str	r1, [sp, #4]
 1a4:	b2c0      	uxtb	r0, r0
 1a6:	a901      	add	r1, sp, #4
 1a8:	2201      	movs	r2, #1
 1aa:	f7ff ffc1 	bl	130 <mbus_write_message>
 1ae:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_remote_register_read:

000001b0 <mbus_remote_register_read>:
 1b0:	b508      	push	{r3, lr}
 1b2:	2300      	movs	r3, #0
 1b4:	f7ff ffde 	bl	174 <mbus_copy_registers_from_remote_to_local>
 1b8:	bd08      	pop	{r3, pc}

Disassembly of section .text.mbus_copy_mem_from_remote_to_any_bulk:

000001ba <mbus_copy_mem_from_remote_to_any_bulk>:
 1ba:	b530      	push	{r4, r5, lr}
 1bc:	b085      	sub	sp, #20
 1be:	9d08      	ldr	r5, [sp, #32]
 1c0:	2480      	movs	r4, #128	; 0x80
 1c2:	04a4      	lsls	r4, r4, #18
 1c4:	0712      	lsls	r2, r2, #28
 1c6:	4322      	orrs	r2, r4
 1c8:	032c      	lsls	r4, r5, #12
 1ca:	0b24      	lsrs	r4, r4, #12
 1cc:	4322      	orrs	r2, r4
 1ce:	9201      	str	r2, [sp, #4]
 1d0:	0100      	lsls	r0, r0, #4
 1d2:	2203      	movs	r2, #3
 1d4:	4310      	orrs	r0, r2
 1d6:	9102      	str	r1, [sp, #8]
 1d8:	b2c0      	uxtb	r0, r0
 1da:	a901      	add	r1, sp, #4
 1dc:	9303      	str	r3, [sp, #12]
 1de:	f7ff ffa7 	bl	130 <mbus_write_message>
 1e2:	b005      	add	sp, #20
 1e4:	bd30      	pop	{r4, r5, pc}

Disassembly of section .text.init_interrupt:

000001e8 <init_interrupt>:
 1e8:	4a03      	ldr	r2, [pc, #12]	; (1f8 <init_interrupt+0x10>)
 1ea:	4b04      	ldr	r3, [pc, #16]	; (1fc <init_interrupt+0x14>)
 1ec:	601a      	str	r2, [r3, #0]
 1ee:	4b04      	ldr	r3, [pc, #16]	; (200 <init_interrupt+0x18>)
 1f0:	2200      	movs	r2, #0
 1f2:	601a      	str	r2, [r3, #0]
 1f4:	4770      	bx	lr
 1f6:	46c0      	nop			; (mov r8, r8)
 1f8:	0001ffff 	.word	0x0001ffff
 1fc:	e000e280 	.word	0xe000e280
 200:	e000e100 	.word	0xe000e100

Disassembly of section .text.handler_ext_int_0:

00000204 <handler_ext_int_0>:
 204:	4b01      	ldr	r3, [pc, #4]	; (20c <handler_ext_int_0+0x8>)
 206:	2201      	movs	r2, #1
 208:	601a      	str	r2, [r3, #0]
 20a:	4770      	bx	lr
 20c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

00000210 <handler_ext_int_1>:
 210:	4b01      	ldr	r3, [pc, #4]	; (218 <handler_ext_int_1+0x8>)
 212:	2202      	movs	r2, #2
 214:	601a      	str	r2, [r3, #0]
 216:	4770      	bx	lr
 218:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

0000021c <handler_ext_int_2>:
 21c:	4b01      	ldr	r3, [pc, #4]	; (224 <handler_ext_int_2+0x8>)
 21e:	2204      	movs	r2, #4
 220:	601a      	str	r2, [r3, #0]
 222:	4770      	bx	lr
 224:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

00000228 <handler_ext_int_3>:
 228:	4b01      	ldr	r3, [pc, #4]	; (230 <handler_ext_int_3+0x8>)
 22a:	2208      	movs	r2, #8
 22c:	601a      	str	r2, [r3, #0]
 22e:	4770      	bx	lr
 230:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

00000234 <handler_ext_int_4>:
 234:	4b01      	ldr	r3, [pc, #4]	; (23c <handler_ext_int_4+0x8>)
 236:	2210      	movs	r2, #16
 238:	601a      	str	r2, [r3, #0]
 23a:	4770      	bx	lr
 23c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

00000240 <handler_ext_int_5>:
 240:	4b01      	ldr	r3, [pc, #4]	; (248 <handler_ext_int_5+0x8>)
 242:	2220      	movs	r2, #32
 244:	601a      	str	r2, [r3, #0]
 246:	4770      	bx	lr
 248:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

0000024c <handler_ext_int_6>:
 24c:	4b01      	ldr	r3, [pc, #4]	; (254 <handler_ext_int_6+0x8>)
 24e:	2240      	movs	r2, #64	; 0x40
 250:	601a      	str	r2, [r3, #0]
 252:	4770      	bx	lr
 254:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

00000258 <handler_ext_int_7>:
 258:	4b01      	ldr	r3, [pc, #4]	; (260 <handler_ext_int_7+0x8>)
 25a:	2280      	movs	r2, #128	; 0x80
 25c:	601a      	str	r2, [r3, #0]
 25e:	4770      	bx	lr
 260:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

00000264 <handler_ext_int_8>:
 264:	4b02      	ldr	r3, [pc, #8]	; (270 <handler_ext_int_8+0xc>)
 266:	2280      	movs	r2, #128	; 0x80
 268:	0052      	lsls	r2, r2, #1
 26a:	601a      	str	r2, [r3, #0]
 26c:	4770      	bx	lr
 26e:	46c0      	nop			; (mov r8, r8)
 270:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

00000274 <handler_ext_int_9>:
 274:	4b02      	ldr	r3, [pc, #8]	; (280 <handler_ext_int_9+0xc>)
 276:	2280      	movs	r2, #128	; 0x80
 278:	0092      	lsls	r2, r2, #2
 27a:	601a      	str	r2, [r3, #0]
 27c:	4770      	bx	lr
 27e:	46c0      	nop			; (mov r8, r8)
 280:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

00000284 <handler_ext_int_10>:
 284:	4b02      	ldr	r3, [pc, #8]	; (290 <handler_ext_int_10+0xc>)
 286:	2280      	movs	r2, #128	; 0x80
 288:	00d2      	lsls	r2, r2, #3
 28a:	601a      	str	r2, [r3, #0]
 28c:	4770      	bx	lr
 28e:	46c0      	nop			; (mov r8, r8)
 290:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

00000294 <handler_ext_int_11>:
 294:	4b02      	ldr	r3, [pc, #8]	; (2a0 <handler_ext_int_11+0xc>)
 296:	2280      	movs	r2, #128	; 0x80
 298:	0112      	lsls	r2, r2, #4
 29a:	601a      	str	r2, [r3, #0]
 29c:	4770      	bx	lr
 29e:	46c0      	nop			; (mov r8, r8)
 2a0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

000002a4 <handler_ext_int_12>:
 2a4:	4b02      	ldr	r3, [pc, #8]	; (2b0 <handler_ext_int_12+0xc>)
 2a6:	2280      	movs	r2, #128	; 0x80
 2a8:	0152      	lsls	r2, r2, #5
 2aa:	601a      	str	r2, [r3, #0]
 2ac:	4770      	bx	lr
 2ae:	46c0      	nop			; (mov r8, r8)
 2b0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

000002b4 <handler_ext_int_13>:
 2b4:	4b02      	ldr	r3, [pc, #8]	; (2c0 <handler_ext_int_13+0xc>)
 2b6:	2280      	movs	r2, #128	; 0x80
 2b8:	0192      	lsls	r2, r2, #6
 2ba:	601a      	str	r2, [r3, #0]
 2bc:	4770      	bx	lr
 2be:	46c0      	nop			; (mov r8, r8)
 2c0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

000002c4 <handler_ext_int_14>:
 2c4:	4b02      	ldr	r3, [pc, #8]	; (2d0 <handler_ext_int_14+0xc>)
 2c6:	2280      	movs	r2, #128	; 0x80
 2c8:	01d2      	lsls	r2, r2, #7
 2ca:	601a      	str	r2, [r3, #0]
 2cc:	4770      	bx	lr
 2ce:	46c0      	nop			; (mov r8, r8)
 2d0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_15:

000002d4 <handler_ext_int_15>:
 2d4:	4b02      	ldr	r3, [pc, #8]	; (2e0 <handler_ext_int_15+0xc>)
 2d6:	2280      	movs	r2, #128	; 0x80
 2d8:	0212      	lsls	r2, r2, #8
 2da:	601a      	str	r2, [r3, #0]
 2dc:	4770      	bx	lr
 2de:	46c0      	nop			; (mov r8, r8)
 2e0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_16:

000002e4 <handler_ext_int_16>:
 2e4:	4b02      	ldr	r3, [pc, #8]	; (2f0 <handler_ext_int_16+0xc>)
 2e6:	2280      	movs	r2, #128	; 0x80
 2e8:	0252      	lsls	r2, r2, #9
 2ea:	601a      	str	r2, [r3, #0]
 2ec:	4770      	bx	lr
 2ee:	46c0      	nop			; (mov r8, r8)
 2f0:	e000e280 	.word	0xe000e280

Disassembly of section .text.initialization:

000002f4 <initialization>:
 2f4:	b508      	push	{r3, lr}
 2f6:	4a09      	ldr	r2, [pc, #36]	; (31c <initialization+0x28>)
 2f8:	4b09      	ldr	r3, [pc, #36]	; (320 <initialization+0x2c>)
 2fa:	480a      	ldr	r0, [pc, #40]	; (324 <initialization+0x30>)
 2fc:	490a      	ldr	r1, [pc, #40]	; (328 <initialization+0x34>)
 2fe:	601a      	str	r2, [r3, #0]
 300:	f7ff feda 	bl	b8 <write_regfile>
 304:	4909      	ldr	r1, [pc, #36]	; (32c <initialization+0x38>)
 306:	20aa      	movs	r0, #170	; 0xaa
 308:	f7ff ff0a 	bl	120 <mbus_write_message32>
 30c:	f7ff fee8 	bl	e0 <set_halt_until_mbus_rx>
 310:	2004      	movs	r0, #4
 312:	f7ff ff25 	bl	160 <mbus_enumerate>
 316:	f7ff fef3 	bl	100 <set_halt_until_mbus_tx>
 31a:	bd08      	pop	{r3, pc}
 31c:	deadbeef 	.word	0xdeadbeef
 320:	00000ce8 	.word	0x00000ce8
 324:	a0000020 	.word	0xa0000020
 328:	0000dead 	.word	0x0000dead
 32c:	aaaaaaaa 	.word	0xaaaaaaaa

Disassembly of section .text.startup.main:

00000330 <main>:
 330:	b507      	push	{r0, r1, r2, lr}
 332:	f7ff ff59 	bl	1e8 <init_interrupt>
 336:	4b56      	ldr	r3, [pc, #344]	; (490 <main+0x160>)
 338:	681a      	ldr	r2, [r3, #0]
 33a:	4b56      	ldr	r3, [pc, #344]	; (494 <main+0x164>)
 33c:	429a      	cmp	r2, r3
 33e:	d001      	beq.n	344 <main+0x14>
 340:	f7ff ffd8 	bl	2f4 <initialization>
 344:	f7ff fedc 	bl	100 <set_halt_until_mbus_tx>
 348:	2004      	movs	r0, #4
 34a:	2100      	movs	r1, #0
 34c:	223d      	movs	r2, #61	; 0x3d
 34e:	f7ff ff22 	bl	196 <mbus_remote_register_write>
 352:	2004      	movs	r0, #4
 354:	2100      	movs	r1, #0
 356:	221d      	movs	r2, #29
 358:	f7ff ff1d 	bl	196 <mbus_remote_register_write>
 35c:	f7ff fec0 	bl	e0 <set_halt_until_mbus_rx>
 360:	2100      	movs	r1, #0
 362:	1c0a      	adds	r2, r1, #0
 364:	2004      	movs	r0, #4
 366:	f7ff ff23 	bl	1b0 <mbus_remote_register_read>
 36a:	f7ff fec9 	bl	100 <set_halt_until_mbus_tx>
 36e:	494a      	ldr	r1, [pc, #296]	; (498 <main+0x168>)
 370:	2202      	movs	r2, #2
 372:	2042      	movs	r0, #66	; 0x42
 374:	f7ff fedc 	bl	130 <mbus_write_message>
 378:	4948      	ldr	r1, [pc, #288]	; (49c <main+0x16c>)
 37a:	2202      	movs	r2, #2
 37c:	2042      	movs	r0, #66	; 0x42
 37e:	f7ff fed7 	bl	130 <mbus_write_message>
 382:	4947      	ldr	r1, [pc, #284]	; (4a0 <main+0x170>)
 384:	2202      	movs	r2, #2
 386:	2042      	movs	r0, #66	; 0x42
 388:	f7ff fed2 	bl	130 <mbus_write_message>
 38c:	4945      	ldr	r1, [pc, #276]	; (4a4 <main+0x174>)
 38e:	2202      	movs	r2, #2
 390:	2042      	movs	r0, #66	; 0x42
 392:	f7ff fecd 	bl	130 <mbus_write_message>
 396:	4944      	ldr	r1, [pc, #272]	; (4a8 <main+0x178>)
 398:	2202      	movs	r2, #2
 39a:	2042      	movs	r0, #66	; 0x42
 39c:	f7ff fec8 	bl	130 <mbus_write_message>
 3a0:	4942      	ldr	r1, [pc, #264]	; (4ac <main+0x17c>)
 3a2:	2202      	movs	r2, #2
 3a4:	2042      	movs	r0, #66	; 0x42
 3a6:	f7ff fec3 	bl	130 <mbus_write_message>
 3aa:	4941      	ldr	r1, [pc, #260]	; (4b0 <main+0x180>)
 3ac:	2202      	movs	r2, #2
 3ae:	2042      	movs	r0, #66	; 0x42
 3b0:	f7ff febe 	bl	130 <mbus_write_message>
 3b4:	493f      	ldr	r1, [pc, #252]	; (4b4 <main+0x184>)
 3b6:	2203      	movs	r2, #3
 3b8:	2042      	movs	r0, #66	; 0x42
 3ba:	f7ff feb9 	bl	130 <mbus_write_message>
 3be:	493e      	ldr	r1, [pc, #248]	; (4b8 <main+0x188>)
 3c0:	2203      	movs	r2, #3
 3c2:	2042      	movs	r0, #66	; 0x42
 3c4:	f7ff feb4 	bl	130 <mbus_write_message>
 3c8:	493c      	ldr	r1, [pc, #240]	; (4bc <main+0x18c>)
 3ca:	2202      	movs	r2, #2
 3cc:	2042      	movs	r0, #66	; 0x42
 3ce:	f7ff feaf 	bl	130 <mbus_write_message>
 3d2:	493b      	ldr	r1, [pc, #236]	; (4c0 <main+0x190>)
 3d4:	2202      	movs	r2, #2
 3d6:	2042      	movs	r0, #66	; 0x42
 3d8:	f7ff feaa 	bl	130 <mbus_write_message>
 3dc:	4939      	ldr	r1, [pc, #228]	; (4c4 <main+0x194>)
 3de:	2202      	movs	r2, #2
 3e0:	2042      	movs	r0, #66	; 0x42
 3e2:	f7ff fea5 	bl	130 <mbus_write_message>
 3e6:	4938      	ldr	r1, [pc, #224]	; (4c8 <main+0x198>)
 3e8:	2202      	movs	r2, #2
 3ea:	2042      	movs	r0, #66	; 0x42
 3ec:	f7ff fea0 	bl	130 <mbus_write_message>
 3f0:	4936      	ldr	r1, [pc, #216]	; (4cc <main+0x19c>)
 3f2:	2202      	movs	r2, #2
 3f4:	2042      	movs	r0, #66	; 0x42
 3f6:	f7ff fe9b 	bl	130 <mbus_write_message>
 3fa:	4935      	ldr	r1, [pc, #212]	; (4d0 <main+0x1a0>)
 3fc:	2202      	movs	r2, #2
 3fe:	2042      	movs	r0, #66	; 0x42
 400:	f7ff fe96 	bl	130 <mbus_write_message>
 404:	4933      	ldr	r1, [pc, #204]	; (4d4 <main+0x1a4>)
 406:	2202      	movs	r2, #2
 408:	2042      	movs	r0, #66	; 0x42
 40a:	f7ff fe91 	bl	130 <mbus_write_message>
 40e:	4932      	ldr	r1, [pc, #200]	; (4d8 <main+0x1a8>)
 410:	2202      	movs	r2, #2
 412:	2042      	movs	r0, #66	; 0x42
 414:	f7ff fe8c 	bl	130 <mbus_write_message>
 418:	4930      	ldr	r1, [pc, #192]	; (4dc <main+0x1ac>)
 41a:	2202      	movs	r2, #2
 41c:	2042      	movs	r0, #66	; 0x42
 41e:	f7ff fe87 	bl	130 <mbus_write_message>
 422:	492f      	ldr	r1, [pc, #188]	; (4e0 <main+0x1b0>)
 424:	2202      	movs	r2, #2
 426:	2042      	movs	r0, #66	; 0x42
 428:	f7ff fe82 	bl	130 <mbus_write_message>
 42c:	492d      	ldr	r1, [pc, #180]	; (4e4 <main+0x1b4>)
 42e:	4a2e      	ldr	r2, [pc, #184]	; (4e8 <main+0x1b8>)
 430:	2042      	movs	r0, #66	; 0x42
 432:	f7ff fe7d 	bl	130 <mbus_write_message>
 436:	492d      	ldr	r1, [pc, #180]	; (4ec <main+0x1bc>)
 438:	2202      	movs	r2, #2
 43a:	2042      	movs	r0, #66	; 0x42
 43c:	f7ff fe78 	bl	130 <mbus_write_message>
 440:	492b      	ldr	r1, [pc, #172]	; (4f0 <main+0x1c0>)
 442:	2202      	movs	r2, #2
 444:	2042      	movs	r0, #66	; 0x42
 446:	f7ff fe73 	bl	130 <mbus_write_message>
 44a:	20fa      	movs	r0, #250	; 0xfa
 44c:	0080      	lsls	r0, r0, #2
 44e:	f7ff fe2a 	bl	a6 <delay>
 452:	2202      	movs	r2, #2
 454:	4927      	ldr	r1, [pc, #156]	; (4f4 <main+0x1c4>)
 456:	2042      	movs	r0, #66	; 0x42
 458:	f7ff fe6a 	bl	130 <mbus_write_message>
 45c:	4826      	ldr	r0, [pc, #152]	; (4f8 <main+0x1c8>)
 45e:	f7ff fe22 	bl	a6 <delay>
 462:	4926      	ldr	r1, [pc, #152]	; (4fc <main+0x1cc>)
 464:	20dd      	movs	r0, #221	; 0xdd
 466:	f7ff fe5b 	bl	120 <mbus_write_message32>
 46a:	f7ff fe39 	bl	e0 <set_halt_until_mbus_rx>
 46e:	21e0      	movs	r1, #224	; 0xe0
 470:	00c9      	lsls	r1, r1, #3
 472:	2041      	movs	r0, #65	; 0x41
 474:	f7ff fe54 	bl	120 <mbus_write_message32>
 478:	20fa      	movs	r0, #250	; 0xfa
 47a:	0080      	lsls	r0, r0, #2
 47c:	f7ff fe13 	bl	a6 <delay>
 480:	2300      	movs	r3, #0
 482:	9300      	str	r3, [sp, #0]
 484:	2004      	movs	r0, #4
 486:	491e      	ldr	r1, [pc, #120]	; (500 <main+0x1d0>)
 488:	2207      	movs	r2, #7
 48a:	f7ff fe96 	bl	1ba <mbus_copy_mem_from_remote_to_any_bulk>
 48e:	e7fe      	b.n	48e <main+0x15e>
 490:	00000ce8 	.word	0x00000ce8
 494:	deadbeef 	.word	0xdeadbeef
 498:	00000cc0 	.word	0x00000cc0
 49c:	00000cc8 	.word	0x00000cc8
 4a0:	00000cd0 	.word	0x00000cd0
 4a4:	00000cd8 	.word	0x00000cd8
 4a8:	00000ce0 	.word	0x00000ce0
 4ac:	00000ca4 	.word	0x00000ca4
 4b0:	00000cb8 	.word	0x00000cb8
 4b4:	00000cac 	.word	0x00000cac
 4b8:	00000c98 	.word	0x00000c98
 4bc:	00000504 	.word	0x00000504
 4c0:	0000050c 	.word	0x0000050c
 4c4:	00000514 	.word	0x00000514
 4c8:	0000051c 	.word	0x0000051c
 4cc:	00000524 	.word	0x00000524
 4d0:	0000052c 	.word	0x0000052c
 4d4:	00000c60 	.word	0x00000c60
 4d8:	00000c70 	.word	0x00000c70
 4dc:	00000c80 	.word	0x00000c80
 4e0:	00000c88 	.word	0x00000c88
 4e4:	00000534 	.word	0x00000534
 4e8:	000001cb 	.word	0x000001cb
 4ec:	00000c68 	.word	0x00000c68
 4f0:	00000c90 	.word	0x00000c90
 4f4:	00000c78 	.word	0x00000c78
 4f8:	00002710 	.word	0x00002710
 4fc:	0ea7f00d 	.word	0x0ea7f00d
 500:	a2000124 	.word	0xa2000124
