
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
  40:	0000027d 	.word	0x0000027d
  44:	00000289 	.word	0x00000289
  48:	00000295 	.word	0x00000295
  4c:	000002a1 	.word	0x000002a1
  50:	000002ad 	.word	0x000002ad
  54:	000002b9 	.word	0x000002b9
  58:	000002c5 	.word	0x000002c5
  5c:	000002d1 	.word	0x000002d1
  60:	000002dd 	.word	0x000002dd
  64:	000002ed 	.word	0x000002ed
  68:	000002fd 	.word	0x000002fd
  6c:	0000030d 	.word	0x0000030d
  70:	0000031d 	.word	0x0000031d
  74:	0000032d 	.word	0x0000032d
  78:	0000033d 	.word	0x0000033d
  7c:	00000000 	.word	0x00000000

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 f9ce 	bl	430 <main>
  94:	e7fc      	b.n	90 <_start>

Disassembly of section .text.FLSv1L_turnOnFlash:

00000096 <FLSv1L_turnOnFlash>:
  96:	b508      	push	{r3, lr}
  98:	2111      	movs	r1, #17
  9a:	2203      	movs	r2, #3
  9c:	f000 f8ba 	bl	214 <mbus_remote_register_write>
  a0:	bd08      	pop	{r3, pc}

Disassembly of section .text.FLSv1L_turnOffFlash:

000000a2 <FLSv1L_turnOffFlash>:
  a2:	b508      	push	{r3, lr}
  a4:	2111      	movs	r1, #17
  a6:	2202      	movs	r2, #2
  a8:	f000 f8b4 	bl	214 <mbus_remote_register_write>
  ac:	bd08      	pop	{r3, pc}

Disassembly of section .text.FLSv1L_enableLargeCap:

000000ae <FLSv1L_enableLargeCap>:
  ae:	b508      	push	{r3, lr}
  b0:	2109      	movs	r1, #9
  b2:	223f      	movs	r2, #63	; 0x3f
  b4:	f000 f8ae 	bl	214 <mbus_remote_register_write>
  b8:	bd08      	pop	{r3, pc}

Disassembly of section .text.FLSv1L_disableLargeCap:

000000ba <FLSv1L_disableLargeCap>:
  ba:	b508      	push	{r3, lr}
  bc:	2109      	movs	r1, #9
  be:	2200      	movs	r2, #0
  c0:	f000 f8a8 	bl	214 <mbus_remote_register_write>
  c4:	bd08      	pop	{r3, pc}

Disassembly of section .text.FLSv1L_setIRQAddr:

000000c6 <FLSv1L_setIRQAddr>:
  c6:	b508      	push	{r3, lr}
  c8:	0209      	lsls	r1, r1, #8
  ca:	430a      	orrs	r2, r1
  cc:	210c      	movs	r1, #12
  ce:	f000 f8a1 	bl	214 <mbus_remote_register_write>
  d2:	bd08      	pop	{r3, pc}

Disassembly of section .text.FLSv1L_setOptTune:

000000d4 <FLSv1L_setOptTune>:
  d4:	b510      	push	{r4, lr}
  d6:	2102      	movs	r1, #2
  d8:	1c04      	adds	r4, r0, #0
  da:	4a04      	ldr	r2, [pc, #16]	; (ec <FLSv1L_setOptTune+0x18>)
  dc:	f000 f89a 	bl	214 <mbus_remote_register_write>
  e0:	1c20      	adds	r0, r4, #0
  e2:	210a      	movs	r1, #10
  e4:	4a02      	ldr	r2, [pc, #8]	; (f0 <FLSv1L_setOptTune+0x1c>)
  e6:	f000 f895 	bl	214 <mbus_remote_register_write>
  ea:	bd10      	pop	{r4, pc}
  ec:	000500c0 	.word	0x000500c0
  f0:	0000023f 	.word	0x0000023f

Disassembly of section .text.FLSv1L_setSRAMStartAddr:

000000f4 <FLSv1L_setSRAMStartAddr>:
  f4:	b508      	push	{r3, lr}
  f6:	1c0a      	adds	r2, r1, #0
  f8:	2106      	movs	r1, #6
  fa:	f000 f88b 	bl	214 <mbus_remote_register_write>
  fe:	bd08      	pop	{r3, pc}

Disassembly of section .text.FLSv1L_setFlashStartAddr:

00000100 <FLSv1L_setFlashStartAddr>:
 100:	b508      	push	{r3, lr}
 102:	1c0a      	adds	r2, r1, #0
 104:	2107      	movs	r1, #7
 106:	f000 f885 	bl	214 <mbus_remote_register_write>
 10a:	bd08      	pop	{r3, pc}

Disassembly of section .text.delay:

0000010c <delay>:
 10c:	b500      	push	{lr}
 10e:	2300      	movs	r3, #0
 110:	e001      	b.n	116 <delay+0xa>
 112:	46c0      	nop			; (mov r8, r8)
 114:	3301      	adds	r3, #1
 116:	4283      	cmp	r3, r0
 118:	d1fb      	bne.n	112 <delay+0x6>
 11a:	bd00      	pop	{pc}

Disassembly of section .text.write_regfile:

0000011c <write_regfile>:
 11c:	0880      	lsrs	r0, r0, #2
 11e:	0209      	lsls	r1, r1, #8
 120:	b508      	push	{r3, lr}
 122:	0a09      	lsrs	r1, r1, #8
 124:	4b05      	ldr	r3, [pc, #20]	; (13c <write_regfile+0x20>)
 126:	0600      	lsls	r0, r0, #24
 128:	4308      	orrs	r0, r1
 12a:	6018      	str	r0, [r3, #0]
 12c:	4b04      	ldr	r3, [pc, #16]	; (140 <write_regfile+0x24>)
 12e:	2210      	movs	r2, #16
 130:	601a      	str	r2, [r3, #0]
 132:	200a      	movs	r0, #10
 134:	f7ff ffea 	bl	10c <delay>
 138:	bd08      	pop	{r3, pc}
 13a:	46c0      	nop			; (mov r8, r8)
 13c:	a0002000 	.word	0xa0002000
 140:	a000200c 	.word	0xa000200c

Disassembly of section .text.set_wakeup_timer:

00000144 <set_wakeup_timer>:
 144:	b510      	push	{r4, lr}
 146:	1c14      	adds	r4, r2, #0
 148:	2900      	cmp	r1, #0
 14a:	d003      	beq.n	154 <set_wakeup_timer+0x10>
 14c:	2180      	movs	r1, #128	; 0x80
 14e:	0209      	lsls	r1, r1, #8
 150:	4301      	orrs	r1, r0
 152:	e001      	b.n	158 <set_wakeup_timer+0x14>
 154:	0440      	lsls	r0, r0, #17
 156:	0c41      	lsrs	r1, r0, #17
 158:	4804      	ldr	r0, [pc, #16]	; (16c <set_wakeup_timer+0x28>)
 15a:	f7ff ffdf 	bl	11c <write_regfile>
 15e:	2c00      	cmp	r4, #0
 160:	d002      	beq.n	168 <set_wakeup_timer+0x24>
 162:	4b03      	ldr	r3, [pc, #12]	; (170 <set_wakeup_timer+0x2c>)
 164:	2201      	movs	r2, #1
 166:	601a      	str	r2, [r3, #0]
 168:	bd10      	pop	{r4, pc}
 16a:	46c0      	nop			; (mov r8, r8)
 16c:	a0000034 	.word	0xa0000034
 170:	a0001300 	.word	0xa0001300

Disassembly of section .text.set_halt_until_mbus_rx:

00000174 <set_halt_until_mbus_rx>:
 174:	4805      	ldr	r0, [pc, #20]	; (18c <set_halt_until_mbus_rx+0x18>)
 176:	b508      	push	{r3, lr}
 178:	6801      	ldr	r1, [r0, #0]
 17a:	4b05      	ldr	r3, [pc, #20]	; (190 <set_halt_until_mbus_rx+0x1c>)
 17c:	4019      	ands	r1, r3
 17e:	2390      	movs	r3, #144	; 0x90
 180:	021b      	lsls	r3, r3, #8
 182:	4319      	orrs	r1, r3
 184:	f7ff ffca 	bl	11c <write_regfile>
 188:	bd08      	pop	{r3, pc}
 18a:	46c0      	nop			; (mov r8, r8)
 18c:	a0000028 	.word	0xa0000028
 190:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.set_halt_until_mbus_tx:

00000194 <set_halt_until_mbus_tx>:
 194:	4805      	ldr	r0, [pc, #20]	; (1ac <set_halt_until_mbus_tx+0x18>)
 196:	b508      	push	{r3, lr}
 198:	6801      	ldr	r1, [r0, #0]
 19a:	4b05      	ldr	r3, [pc, #20]	; (1b0 <set_halt_until_mbus_tx+0x1c>)
 19c:	4019      	ands	r1, r3
 19e:	23a0      	movs	r3, #160	; 0xa0
 1a0:	021b      	lsls	r3, r3, #8
 1a2:	4319      	orrs	r1, r3
 1a4:	f7ff ffba 	bl	11c <write_regfile>
 1a8:	bd08      	pop	{r3, pc}
 1aa:	46c0      	nop			; (mov r8, r8)
 1ac:	a0000028 	.word	0xa0000028
 1b0:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.mbus_write_message32:

000001b4 <mbus_write_message32>:
 1b4:	4b02      	ldr	r3, [pc, #8]	; (1c0 <mbus_write_message32+0xc>)
 1b6:	0100      	lsls	r0, r0, #4
 1b8:	4318      	orrs	r0, r3
 1ba:	6001      	str	r1, [r0, #0]
 1bc:	2001      	movs	r0, #1
 1be:	4770      	bx	lr
 1c0:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_write_message:

000001c4 <mbus_write_message>:
 1c4:	2300      	movs	r3, #0
 1c6:	b500      	push	{lr}
 1c8:	429a      	cmp	r2, r3
 1ca:	d00a      	beq.n	1e2 <mbus_write_message+0x1e>
 1cc:	4b06      	ldr	r3, [pc, #24]	; (1e8 <mbus_write_message+0x24>)
 1ce:	3a01      	subs	r2, #1
 1d0:	0600      	lsls	r0, r0, #24
 1d2:	4302      	orrs	r2, r0
 1d4:	601a      	str	r2, [r3, #0]
 1d6:	4b05      	ldr	r3, [pc, #20]	; (1ec <mbus_write_message+0x28>)
 1d8:	2223      	movs	r2, #35	; 0x23
 1da:	6019      	str	r1, [r3, #0]
 1dc:	4b04      	ldr	r3, [pc, #16]	; (1f0 <mbus_write_message+0x2c>)
 1de:	601a      	str	r2, [r3, #0]
 1e0:	2301      	movs	r3, #1
 1e2:	1c18      	adds	r0, r3, #0
 1e4:	bd00      	pop	{pc}
 1e6:	46c0      	nop			; (mov r8, r8)
 1e8:	a0002000 	.word	0xa0002000
 1ec:	a0002004 	.word	0xa0002004
 1f0:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_enumerate:

000001f4 <mbus_enumerate>:
 1f4:	0603      	lsls	r3, r0, #24
 1f6:	2080      	movs	r0, #128	; 0x80
 1f8:	0580      	lsls	r0, r0, #22
 1fa:	4318      	orrs	r0, r3
 1fc:	4b01      	ldr	r3, [pc, #4]	; (204 <mbus_enumerate+0x10>)
 1fe:	6018      	str	r0, [r3, #0]
 200:	4770      	bx	lr
 202:	46c0      	nop			; (mov r8, r8)
 204:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

00000208 <mbus_sleep_all>:
 208:	4b01      	ldr	r3, [pc, #4]	; (210 <mbus_sleep_all+0x8>)
 20a:	2200      	movs	r2, #0
 20c:	601a      	str	r2, [r3, #0]
 20e:	4770      	bx	lr
 210:	a0003010 	.word	0xa0003010

Disassembly of section .text.mbus_remote_register_write:

00000214 <mbus_remote_register_write>:
 214:	b507      	push	{r0, r1, r2, lr}
 216:	0212      	lsls	r2, r2, #8
 218:	0a12      	lsrs	r2, r2, #8
 21a:	0609      	lsls	r1, r1, #24
 21c:	4311      	orrs	r1, r2
 21e:	0100      	lsls	r0, r0, #4
 220:	9101      	str	r1, [sp, #4]
 222:	b2c0      	uxtb	r0, r0
 224:	a901      	add	r1, sp, #4
 226:	2201      	movs	r2, #1
 228:	f7ff ffcc 	bl	1c4 <mbus_write_message>
 22c:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_copy_mem_from_local_to_remote_stream:

00000230 <mbus_copy_mem_from_local_to_remote_stream>:
 230:	b510      	push	{r4, lr}
 232:	2480      	movs	r4, #128	; 0x80
 234:	04e4      	lsls	r4, r4, #19
 236:	0600      	lsls	r0, r0, #24
 238:	4320      	orrs	r0, r4
 23a:	031b      	lsls	r3, r3, #12
 23c:	070c      	lsls	r4, r1, #28
 23e:	1c01      	adds	r1, r0, #0
 240:	4321      	orrs	r1, r4
 242:	0b1b      	lsrs	r3, r3, #12
 244:	4319      	orrs	r1, r3
 246:	4b04      	ldr	r3, [pc, #16]	; (258 <mbus_copy_mem_from_local_to_remote_stream+0x28>)
 248:	6019      	str	r1, [r3, #0]
 24a:	4b04      	ldr	r3, [pc, #16]	; (25c <mbus_copy_mem_from_local_to_remote_stream+0x2c>)
 24c:	601a      	str	r2, [r3, #0]
 24e:	4b04      	ldr	r3, [pc, #16]	; (260 <mbus_copy_mem_from_local_to_remote_stream+0x30>)
 250:	2223      	movs	r2, #35	; 0x23
 252:	601a      	str	r2, [r3, #0]
 254:	bd10      	pop	{r4, pc}
 256:	46c0      	nop			; (mov r8, r8)
 258:	a0002000 	.word	0xa0002000
 25c:	a0002004 	.word	0xa0002004
 260:	a000200c 	.word	0xa000200c

Disassembly of section .text.init_interrupt:

00000264 <init_interrupt>:
 264:	4b02      	ldr	r3, [pc, #8]	; (270 <init_interrupt+0xc>)
 266:	4a03      	ldr	r2, [pc, #12]	; (274 <init_interrupt+0x10>)
 268:	6013      	str	r3, [r2, #0]
 26a:	4a03      	ldr	r2, [pc, #12]	; (278 <init_interrupt+0x14>)
 26c:	6013      	str	r3, [r2, #0]
 26e:	4770      	bx	lr
 270:	00007fff 	.word	0x00007fff
 274:	e000e280 	.word	0xe000e280
 278:	e000e100 	.word	0xe000e100

Disassembly of section .text.handler_ext_int_0:

0000027c <handler_ext_int_0>:
 27c:	4b01      	ldr	r3, [pc, #4]	; (284 <handler_ext_int_0+0x8>)
 27e:	2201      	movs	r2, #1
 280:	601a      	str	r2, [r3, #0]
 282:	4770      	bx	lr
 284:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

00000288 <handler_ext_int_1>:
 288:	4b01      	ldr	r3, [pc, #4]	; (290 <handler_ext_int_1+0x8>)
 28a:	2202      	movs	r2, #2
 28c:	601a      	str	r2, [r3, #0]
 28e:	4770      	bx	lr
 290:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

00000294 <handler_ext_int_2>:
 294:	4b01      	ldr	r3, [pc, #4]	; (29c <handler_ext_int_2+0x8>)
 296:	2204      	movs	r2, #4
 298:	601a      	str	r2, [r3, #0]
 29a:	4770      	bx	lr
 29c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

000002a0 <handler_ext_int_3>:
 2a0:	4b01      	ldr	r3, [pc, #4]	; (2a8 <handler_ext_int_3+0x8>)
 2a2:	2208      	movs	r2, #8
 2a4:	601a      	str	r2, [r3, #0]
 2a6:	4770      	bx	lr
 2a8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

000002ac <handler_ext_int_4>:
 2ac:	4b01      	ldr	r3, [pc, #4]	; (2b4 <handler_ext_int_4+0x8>)
 2ae:	2210      	movs	r2, #16
 2b0:	601a      	str	r2, [r3, #0]
 2b2:	4770      	bx	lr
 2b4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

000002b8 <handler_ext_int_5>:
 2b8:	4b01      	ldr	r3, [pc, #4]	; (2c0 <handler_ext_int_5+0x8>)
 2ba:	2220      	movs	r2, #32
 2bc:	601a      	str	r2, [r3, #0]
 2be:	4770      	bx	lr
 2c0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

000002c4 <handler_ext_int_6>:
 2c4:	4b01      	ldr	r3, [pc, #4]	; (2cc <handler_ext_int_6+0x8>)
 2c6:	2240      	movs	r2, #64	; 0x40
 2c8:	601a      	str	r2, [r3, #0]
 2ca:	4770      	bx	lr
 2cc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

000002d0 <handler_ext_int_7>:
 2d0:	4b01      	ldr	r3, [pc, #4]	; (2d8 <handler_ext_int_7+0x8>)
 2d2:	2280      	movs	r2, #128	; 0x80
 2d4:	601a      	str	r2, [r3, #0]
 2d6:	4770      	bx	lr
 2d8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

000002dc <handler_ext_int_8>:
 2dc:	4b02      	ldr	r3, [pc, #8]	; (2e8 <handler_ext_int_8+0xc>)
 2de:	2280      	movs	r2, #128	; 0x80
 2e0:	0052      	lsls	r2, r2, #1
 2e2:	601a      	str	r2, [r3, #0]
 2e4:	4770      	bx	lr
 2e6:	46c0      	nop			; (mov r8, r8)
 2e8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

000002ec <handler_ext_int_9>:
 2ec:	4b02      	ldr	r3, [pc, #8]	; (2f8 <handler_ext_int_9+0xc>)
 2ee:	2280      	movs	r2, #128	; 0x80
 2f0:	0092      	lsls	r2, r2, #2
 2f2:	601a      	str	r2, [r3, #0]
 2f4:	4770      	bx	lr
 2f6:	46c0      	nop			; (mov r8, r8)
 2f8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

000002fc <handler_ext_int_10>:
 2fc:	4b02      	ldr	r3, [pc, #8]	; (308 <handler_ext_int_10+0xc>)
 2fe:	2280      	movs	r2, #128	; 0x80
 300:	00d2      	lsls	r2, r2, #3
 302:	601a      	str	r2, [r3, #0]
 304:	4770      	bx	lr
 306:	46c0      	nop			; (mov r8, r8)
 308:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

0000030c <handler_ext_int_11>:
 30c:	4b02      	ldr	r3, [pc, #8]	; (318 <handler_ext_int_11+0xc>)
 30e:	2280      	movs	r2, #128	; 0x80
 310:	0112      	lsls	r2, r2, #4
 312:	601a      	str	r2, [r3, #0]
 314:	4770      	bx	lr
 316:	46c0      	nop			; (mov r8, r8)
 318:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

0000031c <handler_ext_int_12>:
 31c:	4b02      	ldr	r3, [pc, #8]	; (328 <handler_ext_int_12+0xc>)
 31e:	2280      	movs	r2, #128	; 0x80
 320:	0152      	lsls	r2, r2, #5
 322:	601a      	str	r2, [r3, #0]
 324:	4770      	bx	lr
 326:	46c0      	nop			; (mov r8, r8)
 328:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

0000032c <handler_ext_int_13>:
 32c:	4b02      	ldr	r3, [pc, #8]	; (338 <handler_ext_int_13+0xc>)
 32e:	2280      	movs	r2, #128	; 0x80
 330:	0192      	lsls	r2, r2, #6
 332:	601a      	str	r2, [r3, #0]
 334:	4770      	bx	lr
 336:	46c0      	nop			; (mov r8, r8)
 338:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

0000033c <handler_ext_int_14>:
 33c:	4b02      	ldr	r3, [pc, #8]	; (348 <handler_ext_int_14+0xc>)
 33e:	2280      	movs	r2, #128	; 0x80
 340:	01d2      	lsls	r2, r2, #7
 342:	601a      	str	r2, [r3, #0]
 344:	4770      	bx	lr
 346:	46c0      	nop			; (mov r8, r8)
 348:	e000e280 	.word	0xe000e280

Disassembly of section .text.initialization:

0000034c <initialization>:
 34c:	4a0f      	ldr	r2, [pc, #60]	; (38c <initialization+0x40>)
 34e:	4b10      	ldr	r3, [pc, #64]	; (390 <initialization+0x44>)
 350:	b510      	push	{r4, lr}
 352:	601a      	str	r2, [r3, #0]
 354:	4b0f      	ldr	r3, [pc, #60]	; (394 <initialization+0x48>)
 356:	2400      	movs	r4, #0
 358:	480f      	ldr	r0, [pc, #60]	; (398 <initialization+0x4c>)
 35a:	4910      	ldr	r1, [pc, #64]	; (39c <initialization+0x50>)
 35c:	601c      	str	r4, [r3, #0]
 35e:	f7ff fedd 	bl	11c <write_regfile>
 362:	490f      	ldr	r1, [pc, #60]	; (3a0 <initialization+0x54>)
 364:	20aa      	movs	r0, #170	; 0xaa
 366:	f7ff ff25 	bl	1b4 <mbus_write_message32>
 36a:	f7ff ff03 	bl	174 <set_halt_until_mbus_rx>
 36e:	2004      	movs	r0, #4
 370:	f7ff ff40 	bl	1f4 <mbus_enumerate>
 374:	f7ff ff0e 	bl	194 <set_halt_until_mbus_tx>
 378:	2004      	movs	r0, #4
 37a:	f7ff feab 	bl	d4 <FLSv1L_setOptTune>
 37e:	2004      	movs	r0, #4
 380:	2110      	movs	r1, #16
 382:	1c22      	adds	r2, r4, #0
 384:	f7ff fe9f 	bl	c6 <FLSv1L_setIRQAddr>
 388:	bd10      	pop	{r4, pc}
 38a:	46c0      	nop			; (mov r8, r8)
 38c:	deadbeef 	.word	0xdeadbeef
 390:	000004e4 	.word	0x000004e4
 394:	000004e8 	.word	0x000004e8
 398:	a0000020 	.word	0xa0000020
 39c:	0000dead 	.word	0x0000dead
 3a0:	aaaaaaaa 	.word	0xaaaaaaaa

Disassembly of section .text.cycle0:

000003a4 <cycle0>:
 3a4:	b508      	push	{r3, lr}
 3a6:	f7ff fee5 	bl	174 <set_halt_until_mbus_rx>
 3aa:	2004      	movs	r0, #4
 3ac:	f7ff fe73 	bl	96 <FLSv1L_turnOnFlash>
 3b0:	f7ff fef0 	bl	194 <set_halt_until_mbus_tx>
 3b4:	2004      	movs	r0, #4
 3b6:	f7ff fe7a 	bl	ae <FLSv1L_enableLargeCap>
 3ba:	4807      	ldr	r0, [pc, #28]	; (3d8 <cycle0+0x34>)
 3bc:	f7ff fea6 	bl	10c <delay>
 3c0:	2004      	movs	r0, #4
 3c2:	f7ff fe7a 	bl	ba <FLSv1L_disableLargeCap>
 3c6:	f7ff fed5 	bl	174 <set_halt_until_mbus_rx>
 3ca:	2004      	movs	r0, #4
 3cc:	f7ff fe69 	bl	a2 <FLSv1L_turnOffFlash>
 3d0:	f7ff fee0 	bl	194 <set_halt_until_mbus_tx>
 3d4:	bd08      	pop	{r3, pc}
 3d6:	46c0      	nop			; (mov r8, r8)
 3d8:	00002710 	.word	0x00002710

Disassembly of section .text.cycle1:

000003dc <cycle1>:
 3dc:	b530      	push	{r4, r5, lr}
 3de:	2004      	movs	r0, #4
 3e0:	b085      	sub	sp, #20
 3e2:	2100      	movs	r1, #0
 3e4:	f7ff fe86 	bl	f4 <FLSv1L_setSRAMStartAddr>
 3e8:	2004      	movs	r0, #4
 3ea:	2100      	movs	r1, #0
 3ec:	f7ff fe88 	bl	100 <FLSv1L_setFlashStartAddr>
 3f0:	490d      	ldr	r1, [pc, #52]	; (428 <cycle1+0x4c>)
 3f2:	2210      	movs	r2, #16
 3f4:	2044      	movs	r0, #68	; 0x44
 3f6:	f7ff fee5 	bl	1c4 <mbus_write_message>
 3fa:	f7ff febb 	bl	174 <set_halt_until_mbus_rx>
 3fe:	a901      	add	r1, sp, #4
 400:	4b0a      	ldr	r3, [pc, #40]	; (42c <cycle1+0x50>)
 402:	1c0a      	adds	r2, r1, #0
 404:	cb31      	ldmia	r3!, {r0, r4, r5}
 406:	c231      	stmia	r2!, {r0, r4, r5}
 408:	2203      	movs	r2, #3
 40a:	2043      	movs	r0, #67	; 0x43
 40c:	f7ff feda 	bl	1c4 <mbus_write_message>
 410:	f7ff fec0 	bl	194 <set_halt_until_mbus_tx>
 414:	22e0      	movs	r2, #224	; 0xe0
 416:	2000      	movs	r0, #0
 418:	2104      	movs	r1, #4
 41a:	0152      	lsls	r2, r2, #5
 41c:	230f      	movs	r3, #15
 41e:	f7ff ff07 	bl	230 <mbus_copy_mem_from_local_to_remote_stream>
 422:	b005      	add	sp, #20
 424:	bd30      	pop	{r4, r5, pc}
 426:	46c0      	nop			; (mov r8, r8)
 428:	000004a4 	.word	0x000004a4
 42c:	00000498 	.word	0x00000498

Disassembly of section .text.startup.main:

00000430 <main>:
 430:	b510      	push	{r4, lr}
 432:	f7ff ff17 	bl	264 <init_interrupt>
 436:	4b14      	ldr	r3, [pc, #80]	; (488 <main+0x58>)
 438:	681a      	ldr	r2, [r3, #0]
 43a:	4b14      	ldr	r3, [pc, #80]	; (48c <main+0x5c>)
 43c:	429a      	cmp	r2, r3
 43e:	d001      	beq.n	444 <main+0x14>
 440:	f7ff ff84 	bl	34c <initialization>
 444:	4c12      	ldr	r4, [pc, #72]	; (490 <main+0x60>)
 446:	20ef      	movs	r0, #239	; 0xef
 448:	6821      	ldr	r1, [r4, #0]
 44a:	f7ff feb3 	bl	1b4 <mbus_write_message32>
 44e:	6823      	ldr	r3, [r4, #0]
 450:	2b00      	cmp	r3, #0
 452:	d102      	bne.n	45a <main+0x2a>
 454:	f7ff ffa6 	bl	3a4 <cycle0>
 458:	e00a      	b.n	470 <main+0x40>
 45a:	6823      	ldr	r3, [r4, #0]
 45c:	2b01      	cmp	r3, #1
 45e:	d102      	bne.n	466 <main+0x36>
 460:	f7ff ffbc 	bl	3dc <cycle1>
 464:	e004      	b.n	470 <main+0x40>
 466:	20ef      	movs	r0, #239	; 0xef
 468:	490a      	ldr	r1, [pc, #40]	; (494 <main+0x64>)
 46a:	f7ff fea3 	bl	1b4 <mbus_write_message32>
 46e:	e7fe      	b.n	46e <main+0x3e>
 470:	6823      	ldr	r3, [r4, #0]
 472:	2101      	movs	r1, #1
 474:	3301      	adds	r3, #1
 476:	2005      	movs	r0, #5
 478:	1c0a      	adds	r2, r1, #0
 47a:	6023      	str	r3, [r4, #0]
 47c:	f7ff fe62 	bl	144 <set_wakeup_timer>
 480:	f7ff fec2 	bl	208 <mbus_sleep_all>
 484:	e7fe      	b.n	484 <main+0x54>
 486:	46c0      	nop			; (mov r8, r8)
 488:	000004e4 	.word	0x000004e4
 48c:	deadbeef 	.word	0xdeadbeef
 490:	000004e8 	.word	0x000004e8
 494:	0ea7f00d 	.word	0x0ea7f00d
