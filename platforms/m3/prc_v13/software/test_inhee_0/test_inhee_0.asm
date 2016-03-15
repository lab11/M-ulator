
test_inhee_0/test_inhee_0.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0x80>:
   0:	00002000 	.word	0x00002000
   4:	00000091 	.word	0x00000091
	...
  10:	00000080 	.word	0x00000080
  14:	00000080 	.word	0x00000080
  18:	00000080 	.word	0x00000080
  1c:	00000080 	.word	0x00000080
  20:	00000080 	.word	0x00000080
  24:	00000080 	.word	0x00000080
  28:	00000080 	.word	0x00000080
  2c:	00000000 	.word	0x00000000
  30:	00000080 	.word	0x00000080
  34:	00000080 	.word	0x00000080
	...
  40:	000001d1 	.word	0x000001d1
  44:	000001dd 	.word	0x000001dd
  48:	000001e9 	.word	0x000001e9
  4c:	000001f5 	.word	0x000001f5
  50:	00000201 	.word	0x00000201
  54:	0000020d 	.word	0x0000020d
  58:	00000219 	.word	0x00000219
  5c:	00000225 	.word	0x00000225
  60:	00000231 	.word	0x00000231
  64:	00000241 	.word	0x00000241
  68:	00000251 	.word	0x00000251
  6c:	00000261 	.word	0x00000261
  70:	00000271 	.word	0x00000271
  74:	00000281 	.word	0x00000281
	...

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 f912 	bl	2b8 <main>
  94:	e7fc      	b.n	90 <_start>

Disassembly of section .text.set_wakeup_timer:

00000098 <set_wakeup_timer>:
  98:	b500      	push	{lr}
  9a:	2900      	cmp	r1, #0
  9c:	d003      	beq.n	a6 <set_wakeup_timer+0xe>
  9e:	23c0      	movs	r3, #192	; 0xc0
  a0:	029b      	lsls	r3, r3, #10
  a2:	4318      	orrs	r0, r3
  a4:	e001      	b.n	aa <set_wakeup_timer+0x12>
  a6:	0440      	lsls	r0, r0, #17
  a8:	0c40      	lsrs	r0, r0, #17
  aa:	4b04      	ldr	r3, [pc, #16]	; (bc <set_wakeup_timer+0x24>)
  ac:	6018      	str	r0, [r3, #0]
  ae:	2a00      	cmp	r2, #0
  b0:	d002      	beq.n	b8 <set_wakeup_timer+0x20>
  b2:	4b03      	ldr	r3, [pc, #12]	; (c0 <set_wakeup_timer+0x28>)
  b4:	2201      	movs	r2, #1
  b6:	601a      	str	r2, [r3, #0]
  b8:	bd00      	pop	{pc}
  ba:	46c0      	nop			; (mov r8, r8)
  bc:	a0000044 	.word	0xa0000044
  c0:	a0001300 	.word	0xa0001300

Disassembly of section .text.disable_all_irq:

000000c4 <disable_all_irq>:
  c4:	4a03      	ldr	r2, [pc, #12]	; (d4 <disable_all_irq+0x10>)
  c6:	2301      	movs	r3, #1
  c8:	425b      	negs	r3, r3
  ca:	6013      	str	r3, [r2, #0]
  cc:	4a02      	ldr	r2, [pc, #8]	; (d8 <disable_all_irq+0x14>)
  ce:	6013      	str	r3, [r2, #0]
  d0:	4770      	bx	lr
  d2:	46c0      	nop			; (mov r8, r8)
  d4:	e000e280 	.word	0xe000e280
  d8:	e000e180 	.word	0xe000e180

Disassembly of section .text.set_halt_until_mbus_rx:

000000dc <set_halt_until_mbus_rx>:
  dc:	4b04      	ldr	r3, [pc, #16]	; (f0 <set_halt_until_mbus_rx+0x14>)
  de:	4a05      	ldr	r2, [pc, #20]	; (f4 <set_halt_until_mbus_rx+0x18>)
  e0:	6819      	ldr	r1, [r3, #0]
  e2:	400a      	ands	r2, r1
  e4:	2190      	movs	r1, #144	; 0x90
  e6:	0209      	lsls	r1, r1, #8
  e8:	430a      	orrs	r2, r1
  ea:	601a      	str	r2, [r3, #0]
  ec:	4770      	bx	lr
  ee:	46c0      	nop			; (mov r8, r8)
  f0:	a0000028 	.word	0xa0000028
  f4:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.set_halt_until_mbus_tx:

000000f8 <set_halt_until_mbus_tx>:
  f8:	4b04      	ldr	r3, [pc, #16]	; (10c <set_halt_until_mbus_tx+0x14>)
  fa:	4a05      	ldr	r2, [pc, #20]	; (110 <set_halt_until_mbus_tx+0x18>)
  fc:	6819      	ldr	r1, [r3, #0]
  fe:	400a      	ands	r2, r1
 100:	21a0      	movs	r1, #160	; 0xa0
 102:	0209      	lsls	r1, r1, #8
 104:	430a      	orrs	r2, r1
 106:	601a      	str	r2, [r3, #0]
 108:	4770      	bx	lr
 10a:	46c0      	nop			; (mov r8, r8)
 10c:	a0000028 	.word	0xa0000028
 110:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.set_halt_disable:

00000114 <set_halt_disable>:
 114:	4b03      	ldr	r3, [pc, #12]	; (124 <set_halt_disable+0x10>)
 116:	22f0      	movs	r2, #240	; 0xf0
 118:	6819      	ldr	r1, [r3, #0]
 11a:	0212      	lsls	r2, r2, #8
 11c:	430a      	orrs	r2, r1
 11e:	601a      	str	r2, [r3, #0]
 120:	4770      	bx	lr
 122:	46c0      	nop			; (mov r8, r8)
 124:	a0000028 	.word	0xa0000028

Disassembly of section .text.mbus_write_message32:

00000128 <mbus_write_message32>:
 128:	4b02      	ldr	r3, [pc, #8]	; (134 <mbus_write_message32+0xc>)
 12a:	0100      	lsls	r0, r0, #4
 12c:	4318      	orrs	r0, r3
 12e:	6001      	str	r1, [r0, #0]
 130:	2001      	movs	r0, #1
 132:	4770      	bx	lr
 134:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_write_message:

00000138 <mbus_write_message>:
 138:	2300      	movs	r3, #0
 13a:	b500      	push	{lr}
 13c:	429a      	cmp	r2, r3
 13e:	d00a      	beq.n	156 <mbus_write_message+0x1e>
 140:	4b06      	ldr	r3, [pc, #24]	; (15c <mbus_write_message+0x24>)
 142:	3a01      	subs	r2, #1
 144:	0600      	lsls	r0, r0, #24
 146:	4302      	orrs	r2, r0
 148:	601a      	str	r2, [r3, #0]
 14a:	4b05      	ldr	r3, [pc, #20]	; (160 <mbus_write_message+0x28>)
 14c:	2223      	movs	r2, #35	; 0x23
 14e:	6019      	str	r1, [r3, #0]
 150:	4b04      	ldr	r3, [pc, #16]	; (164 <mbus_write_message+0x2c>)
 152:	601a      	str	r2, [r3, #0]
 154:	2301      	movs	r3, #1
 156:	1c18      	adds	r0, r3, #0
 158:	bd00      	pop	{pc}
 15a:	46c0      	nop			; (mov r8, r8)
 15c:	a0002000 	.word	0xa0002000
 160:	a0002004 	.word	0xa0002004
 164:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_enumerate:

00000168 <mbus_enumerate>:
 168:	0603      	lsls	r3, r0, #24
 16a:	2080      	movs	r0, #128	; 0x80
 16c:	0580      	lsls	r0, r0, #22
 16e:	4318      	orrs	r0, r3
 170:	4b01      	ldr	r3, [pc, #4]	; (178 <mbus_enumerate+0x10>)
 172:	6018      	str	r0, [r3, #0]
 174:	4770      	bx	lr
 176:	46c0      	nop			; (mov r8, r8)
 178:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

0000017c <mbus_sleep_all>:
 17c:	4b01      	ldr	r3, [pc, #4]	; (184 <mbus_sleep_all+0x8>)
 17e:	2200      	movs	r2, #0
 180:	601a      	str	r2, [r3, #0]
 182:	4770      	bx	lr
 184:	a0003010 	.word	0xa0003010

Disassembly of section .text.mbus_copy_registers_from_remote_to_local:

00000188 <mbus_copy_registers_from_remote_to_local>:
 188:	b507      	push	{r0, r1, r2, lr}
 18a:	0609      	lsls	r1, r1, #24
 18c:	430a      	orrs	r2, r1
 18e:	2180      	movs	r1, #128	; 0x80
 190:	0149      	lsls	r1, r1, #5
 192:	430a      	orrs	r2, r1
 194:	041b      	lsls	r3, r3, #16
 196:	431a      	orrs	r2, r3
 198:	9201      	str	r2, [sp, #4]
 19a:	0100      	lsls	r0, r0, #4
 19c:	2201      	movs	r2, #1
 19e:	4310      	orrs	r0, r2
 1a0:	b2c0      	uxtb	r0, r0
 1a2:	a901      	add	r1, sp, #4
 1a4:	f7ff ffc8 	bl	138 <mbus_write_message>
 1a8:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_remote_register_write:

000001aa <mbus_remote_register_write>:
 1aa:	b507      	push	{r0, r1, r2, lr}
 1ac:	0212      	lsls	r2, r2, #8
 1ae:	0a12      	lsrs	r2, r2, #8
 1b0:	0609      	lsls	r1, r1, #24
 1b2:	4311      	orrs	r1, r2
 1b4:	0100      	lsls	r0, r0, #4
 1b6:	9101      	str	r1, [sp, #4]
 1b8:	b2c0      	uxtb	r0, r0
 1ba:	a901      	add	r1, sp, #4
 1bc:	2201      	movs	r2, #1
 1be:	f7ff ffbb 	bl	138 <mbus_write_message>
 1c2:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_remote_register_read:

000001c4 <mbus_remote_register_read>:
 1c4:	b508      	push	{r3, lr}
 1c6:	2300      	movs	r3, #0
 1c8:	f7ff ffde 	bl	188 <mbus_copy_registers_from_remote_to_local>
 1cc:	bd08      	pop	{r3, pc}

Disassembly of section .text.handler_ext_int_0:

000001d0 <handler_ext_int_0>:
 1d0:	4b01      	ldr	r3, [pc, #4]	; (1d8 <handler_ext_int_0+0x8>)
 1d2:	2201      	movs	r2, #1
 1d4:	601a      	str	r2, [r3, #0]
 1d6:	4770      	bx	lr
 1d8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

000001dc <handler_ext_int_1>:
 1dc:	4b01      	ldr	r3, [pc, #4]	; (1e4 <handler_ext_int_1+0x8>)
 1de:	2202      	movs	r2, #2
 1e0:	601a      	str	r2, [r3, #0]
 1e2:	4770      	bx	lr
 1e4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

000001e8 <handler_ext_int_2>:
 1e8:	4b01      	ldr	r3, [pc, #4]	; (1f0 <handler_ext_int_2+0x8>)
 1ea:	2204      	movs	r2, #4
 1ec:	601a      	str	r2, [r3, #0]
 1ee:	4770      	bx	lr
 1f0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

000001f4 <handler_ext_int_3>:
 1f4:	4b01      	ldr	r3, [pc, #4]	; (1fc <handler_ext_int_3+0x8>)
 1f6:	2208      	movs	r2, #8
 1f8:	601a      	str	r2, [r3, #0]
 1fa:	4770      	bx	lr
 1fc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

00000200 <handler_ext_int_4>:
 200:	4b01      	ldr	r3, [pc, #4]	; (208 <handler_ext_int_4+0x8>)
 202:	2210      	movs	r2, #16
 204:	601a      	str	r2, [r3, #0]
 206:	4770      	bx	lr
 208:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

0000020c <handler_ext_int_5>:
 20c:	4b01      	ldr	r3, [pc, #4]	; (214 <handler_ext_int_5+0x8>)
 20e:	2220      	movs	r2, #32
 210:	601a      	str	r2, [r3, #0]
 212:	4770      	bx	lr
 214:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

00000218 <handler_ext_int_6>:
 218:	4b01      	ldr	r3, [pc, #4]	; (220 <handler_ext_int_6+0x8>)
 21a:	2240      	movs	r2, #64	; 0x40
 21c:	601a      	str	r2, [r3, #0]
 21e:	4770      	bx	lr
 220:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

00000224 <handler_ext_int_7>:
 224:	4b01      	ldr	r3, [pc, #4]	; (22c <handler_ext_int_7+0x8>)
 226:	2280      	movs	r2, #128	; 0x80
 228:	601a      	str	r2, [r3, #0]
 22a:	4770      	bx	lr
 22c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

00000230 <handler_ext_int_8>:
 230:	4b02      	ldr	r3, [pc, #8]	; (23c <handler_ext_int_8+0xc>)
 232:	2280      	movs	r2, #128	; 0x80
 234:	0052      	lsls	r2, r2, #1
 236:	601a      	str	r2, [r3, #0]
 238:	4770      	bx	lr
 23a:	46c0      	nop			; (mov r8, r8)
 23c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

00000240 <handler_ext_int_9>:
 240:	4b02      	ldr	r3, [pc, #8]	; (24c <handler_ext_int_9+0xc>)
 242:	2280      	movs	r2, #128	; 0x80
 244:	0092      	lsls	r2, r2, #2
 246:	601a      	str	r2, [r3, #0]
 248:	4770      	bx	lr
 24a:	46c0      	nop			; (mov r8, r8)
 24c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

00000250 <handler_ext_int_10>:
 250:	4b02      	ldr	r3, [pc, #8]	; (25c <handler_ext_int_10+0xc>)
 252:	2280      	movs	r2, #128	; 0x80
 254:	00d2      	lsls	r2, r2, #3
 256:	601a      	str	r2, [r3, #0]
 258:	4770      	bx	lr
 25a:	46c0      	nop			; (mov r8, r8)
 25c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

00000260 <handler_ext_int_11>:
 260:	4b02      	ldr	r3, [pc, #8]	; (26c <handler_ext_int_11+0xc>)
 262:	2280      	movs	r2, #128	; 0x80
 264:	0112      	lsls	r2, r2, #4
 266:	601a      	str	r2, [r3, #0]
 268:	4770      	bx	lr
 26a:	46c0      	nop			; (mov r8, r8)
 26c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

00000270 <handler_ext_int_12>:
 270:	4b02      	ldr	r3, [pc, #8]	; (27c <handler_ext_int_12+0xc>)
 272:	2280      	movs	r2, #128	; 0x80
 274:	0152      	lsls	r2, r2, #5
 276:	601a      	str	r2, [r3, #0]
 278:	4770      	bx	lr
 27a:	46c0      	nop			; (mov r8, r8)
 27c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

00000280 <handler_ext_int_13>:
 280:	4b02      	ldr	r3, [pc, #8]	; (28c <handler_ext_int_13+0xc>)
 282:	2280      	movs	r2, #128	; 0x80
 284:	0192      	lsls	r2, r2, #6
 286:	601a      	str	r2, [r3, #0]
 288:	4770      	bx	lr
 28a:	46c0      	nop			; (mov r8, r8)
 28c:	e000e280 	.word	0xe000e280

Disassembly of section .text.initialization:

00000290 <initialization>:
 290:	b508      	push	{r3, lr}
 292:	4a06      	ldr	r2, [pc, #24]	; (2ac <initialization+0x1c>)
 294:	4b06      	ldr	r3, [pc, #24]	; (2b0 <initialization+0x20>)
 296:	601a      	str	r2, [r3, #0]
 298:	4b06      	ldr	r3, [pc, #24]	; (2b4 <initialization+0x24>)
 29a:	2200      	movs	r2, #0
 29c:	601a      	str	r2, [r3, #0]
 29e:	f7ff ff39 	bl	114 <set_halt_disable>
 2a2:	2004      	movs	r0, #4
 2a4:	f7ff ff60 	bl	168 <mbus_enumerate>
 2a8:	bd08      	pop	{r3, pc}
 2aa:	46c0      	nop			; (mov r8, r8)
 2ac:	deadbeef 	.word	0xdeadbeef
 2b0:	00000358 	.word	0x00000358
 2b4:	00000354 	.word	0x00000354

Disassembly of section .text.startup.main:

000002b8 <main>:
 2b8:	b530      	push	{r4, r5, lr}
 2ba:	4a1c      	ldr	r2, [pc, #112]	; (32c <main+0x74>)
 2bc:	b085      	sub	sp, #20
 2be:	4669      	mov	r1, sp
 2c0:	ca31      	ldmia	r2!, {r0, r4, r5}
 2c2:	c131      	stmia	r1!, {r0, r4, r5}
 2c4:	6812      	ldr	r2, [r2, #0]
 2c6:	600a      	str	r2, [r1, #0]
 2c8:	f7ff fefc 	bl	c4 <disable_all_irq>
 2cc:	4b18      	ldr	r3, [pc, #96]	; (330 <main+0x78>)
 2ce:	681a      	ldr	r2, [r3, #0]
 2d0:	4b18      	ldr	r3, [pc, #96]	; (334 <main+0x7c>)
 2d2:	429a      	cmp	r2, r3
 2d4:	d001      	beq.n	2da <main+0x22>
 2d6:	f7ff ffdb 	bl	290 <initialization>
 2da:	4c17      	ldr	r4, [pc, #92]	; (338 <main+0x80>)
 2dc:	20e1      	movs	r0, #225	; 0xe1
 2de:	6821      	ldr	r1, [r4, #0]
 2e0:	f7ff ff22 	bl	128 <mbus_write_message32>
 2e4:	4669      	mov	r1, sp
 2e6:	2204      	movs	r2, #4
 2e8:	20e2      	movs	r0, #226	; 0xe2
 2ea:	f7ff ff25 	bl	138 <mbus_write_message>
 2ee:	2004      	movs	r0, #4
 2f0:	2100      	movs	r1, #0
 2f2:	4a12      	ldr	r2, [pc, #72]	; (33c <main+0x84>)
 2f4:	f7ff ff59 	bl	1aa <mbus_remote_register_write>
 2f8:	f7ff fef0 	bl	dc <set_halt_until_mbus_rx>
 2fc:	2201      	movs	r2, #1
 2fe:	2004      	movs	r0, #4
 300:	2100      	movs	r1, #0
 302:	f7ff ff5f 	bl	1c4 <mbus_remote_register_read>
 306:	f7ff fef7 	bl	f8 <set_halt_until_mbus_tx>
 30a:	4b0d      	ldr	r3, [pc, #52]	; (340 <main+0x88>)
 30c:	20e1      	movs	r0, #225	; 0xe1
 30e:	6819      	ldr	r1, [r3, #0]
 310:	f7ff ff0a 	bl	128 <mbus_write_message32>
 314:	6823      	ldr	r3, [r4, #0]
 316:	2101      	movs	r1, #1
 318:	3301      	adds	r3, #1
 31a:	2003      	movs	r0, #3
 31c:	1c0a      	adds	r2, r1, #0
 31e:	6023      	str	r3, [r4, #0]
 320:	f7ff feba 	bl	98 <set_wakeup_timer>
 324:	f7ff ff2a 	bl	17c <mbus_sleep_all>
 328:	46c0      	nop			; (mov r8, r8)
 32a:	e7fd      	b.n	328 <main+0x70>
 32c:	00000344 	.word	0x00000344
 330:	00000358 	.word	0x00000358
 334:	deadbeef 	.word	0xdeadbeef
 338:	00000354 	.word	0x00000354
 33c:	0000ffff 	.word	0x0000ffff
 340:	a0000004 	.word	0xa0000004
