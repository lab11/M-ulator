
design_tb_0/design_tb_0.elf:     file format elf32-littlearm


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
  40:	00000355 	.word	0x00000355
  44:	000003c1 	.word	0x000003c1
  48:	00000431 	.word	0x00000431
  4c:	00000455 	.word	0x00000455
  50:	00000479 	.word	0x00000479
  54:	0000049d 	.word	0x0000049d
  58:	000004c1 	.word	0x000004c1
  5c:	000004e5 	.word	0x000004e5
  60:	00000509 	.word	0x00000509
  64:	00000531 	.word	0x00000531
  68:	00000559 	.word	0x00000559
  6c:	00000581 	.word	0x00000581
  70:	000005a9 	.word	0x000005a9
  74:	000005d1 	.word	0x000005d1
	...

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 fe00 	bl	c94 <main>
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

Disassembly of section .text.WFI:

000000a6 <WFI>:
  a6:	bf30      	wfi
  a8:	4770      	bx	lr

Disassembly of section .text.config_timer16:

000000ac <config_timer16>:
  ac:	b530      	push	{r4, r5, lr}
  ae:	4c08      	ldr	r4, [pc, #32]	; (d0 <config_timer16+0x24>)
  b0:	2500      	movs	r5, #0
  b2:	6025      	str	r5, [r4, #0]
  b4:	4d07      	ldr	r5, [pc, #28]	; (d4 <config_timer16+0x28>)
  b6:	6028      	str	r0, [r5, #0]
  b8:	4807      	ldr	r0, [pc, #28]	; (d8 <config_timer16+0x2c>)
  ba:	6001      	str	r1, [r0, #0]
  bc:	4907      	ldr	r1, [pc, #28]	; (dc <config_timer16+0x30>)
  be:	600a      	str	r2, [r1, #0]
  c0:	4a07      	ldr	r2, [pc, #28]	; (e0 <config_timer16+0x34>)
  c2:	6013      	str	r3, [r2, #0]
  c4:	9a03      	ldr	r2, [sp, #12]
  c6:	4b07      	ldr	r3, [pc, #28]	; (e4 <config_timer16+0x38>)
  c8:	601a      	str	r2, [r3, #0]
  ca:	2301      	movs	r3, #1
  cc:	6023      	str	r3, [r4, #0]
  ce:	bd30      	pop	{r4, r5, pc}
  d0:	a0001000 	.word	0xa0001000
  d4:	a0001004 	.word	0xa0001004
  d8:	a0001008 	.word	0xa0001008
  dc:	a000100c 	.word	0xa000100c
  e0:	a0001020 	.word	0xa0001020
  e4:	a0001024 	.word	0xa0001024

Disassembly of section .text.config_timer32:

000000e8 <config_timer32>:
  e8:	b530      	push	{r4, r5, lr}
  ea:	4c07      	ldr	r4, [pc, #28]	; (108 <config_timer32+0x20>)
  ec:	2500      	movs	r5, #0
  ee:	6025      	str	r5, [r4, #0]
  f0:	4d06      	ldr	r5, [pc, #24]	; (10c <config_timer32+0x24>)
  f2:	6028      	str	r0, [r5, #0]
  f4:	4806      	ldr	r0, [pc, #24]	; (110 <config_timer32+0x28>)
  f6:	6001      	str	r1, [r0, #0]
  f8:	4906      	ldr	r1, [pc, #24]	; (114 <config_timer32+0x2c>)
  fa:	600a      	str	r2, [r1, #0]
  fc:	4a06      	ldr	r2, [pc, #24]	; (118 <config_timer32+0x30>)
  fe:	6013      	str	r3, [r2, #0]
 100:	2301      	movs	r3, #1
 102:	6023      	str	r3, [r4, #0]
 104:	bd30      	pop	{r4, r5, pc}
 106:	46c0      	nop			; (mov r8, r8)
 108:	a0001100 	.word	0xa0001100
 10c:	a0001104 	.word	0xa0001104
 110:	a0001108 	.word	0xa0001108
 114:	a000110c 	.word	0xa000110c
 118:	a0001110 	.word	0xa0001110

Disassembly of section .text.config_timerwd:

0000011c <config_timerwd>:
 11c:	4b03      	ldr	r3, [pc, #12]	; (12c <config_timerwd+0x10>)
 11e:	2200      	movs	r2, #0
 120:	601a      	str	r2, [r3, #0]
 122:	4a03      	ldr	r2, [pc, #12]	; (130 <config_timerwd+0x14>)
 124:	6010      	str	r0, [r2, #0]
 126:	2201      	movs	r2, #1
 128:	601a      	str	r2, [r3, #0]
 12a:	4770      	bx	lr
 12c:	a0001200 	.word	0xa0001200
 130:	a0001204 	.word	0xa0001204

Disassembly of section .text.disable_all_irq:

00000134 <disable_all_irq>:
 134:	4a03      	ldr	r2, [pc, #12]	; (144 <disable_all_irq+0x10>)
 136:	2301      	movs	r3, #1
 138:	425b      	negs	r3, r3
 13a:	6013      	str	r3, [r2, #0]
 13c:	4a02      	ldr	r2, [pc, #8]	; (148 <disable_all_irq+0x14>)
 13e:	6013      	str	r3, [r2, #0]
 140:	4770      	bx	lr
 142:	46c0      	nop			; (mov r8, r8)
 144:	e000e280 	.word	0xe000e280
 148:	e000e180 	.word	0xe000e180

Disassembly of section .text.clear_all_pend_irq:

0000014c <clear_all_pend_irq>:
 14c:	4b02      	ldr	r3, [pc, #8]	; (158 <clear_all_pend_irq+0xc>)
 14e:	2201      	movs	r2, #1
 150:	4252      	negs	r2, r2
 152:	601a      	str	r2, [r3, #0]
 154:	4770      	bx	lr
 156:	46c0      	nop			; (mov r8, r8)
 158:	e000e280 	.word	0xe000e280

Disassembly of section .text.set_halt_until_reg:

0000015c <set_halt_until_reg>:
 15c:	4b03      	ldr	r3, [pc, #12]	; (16c <set_halt_until_reg+0x10>)
 15e:	4a04      	ldr	r2, [pc, #16]	; (170 <set_halt_until_reg+0x14>)
 160:	6819      	ldr	r1, [r3, #0]
 162:	0300      	lsls	r0, r0, #12
 164:	400a      	ands	r2, r1
 166:	4310      	orrs	r0, r2
 168:	6018      	str	r0, [r3, #0]
 16a:	4770      	bx	lr
 16c:	a0000028 	.word	0xa0000028
 170:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.set_halt_until_mbus_rx:

00000174 <set_halt_until_mbus_rx>:
 174:	4b04      	ldr	r3, [pc, #16]	; (188 <set_halt_until_mbus_rx+0x14>)
 176:	4a05      	ldr	r2, [pc, #20]	; (18c <set_halt_until_mbus_rx+0x18>)
 178:	6819      	ldr	r1, [r3, #0]
 17a:	400a      	ands	r2, r1
 17c:	2190      	movs	r1, #144	; 0x90
 17e:	0209      	lsls	r1, r1, #8
 180:	430a      	orrs	r2, r1
 182:	601a      	str	r2, [r3, #0]
 184:	4770      	bx	lr
 186:	46c0      	nop			; (mov r8, r8)
 188:	a0000028 	.word	0xa0000028
 18c:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.set_halt_until_mbus_tx:

00000190 <set_halt_until_mbus_tx>:
 190:	4b04      	ldr	r3, [pc, #16]	; (1a4 <set_halt_until_mbus_tx+0x14>)
 192:	4a05      	ldr	r2, [pc, #20]	; (1a8 <set_halt_until_mbus_tx+0x18>)
 194:	6819      	ldr	r1, [r3, #0]
 196:	400a      	ands	r2, r1
 198:	21a0      	movs	r1, #160	; 0xa0
 19a:	0209      	lsls	r1, r1, #8
 19c:	430a      	orrs	r2, r1
 19e:	601a      	str	r2, [r3, #0]
 1a0:	4770      	bx	lr
 1a2:	46c0      	nop			; (mov r8, r8)
 1a4:	a0000028 	.word	0xa0000028
 1a8:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.set_halt_until_mbus_fwd:

000001ac <set_halt_until_mbus_fwd>:
 1ac:	4b04      	ldr	r3, [pc, #16]	; (1c0 <set_halt_until_mbus_fwd+0x14>)
 1ae:	4a05      	ldr	r2, [pc, #20]	; (1c4 <set_halt_until_mbus_fwd+0x18>)
 1b0:	6819      	ldr	r1, [r3, #0]
 1b2:	400a      	ands	r2, r1
 1b4:	21b0      	movs	r1, #176	; 0xb0
 1b6:	0209      	lsls	r1, r1, #8
 1b8:	430a      	orrs	r2, r1
 1ba:	601a      	str	r2, [r3, #0]
 1bc:	4770      	bx	lr
 1be:	46c0      	nop			; (mov r8, r8)
 1c0:	a0000028 	.word	0xa0000028
 1c4:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.halt_cpu:

000001c8 <halt_cpu>:
 1c8:	4a01      	ldr	r2, [pc, #4]	; (1d0 <halt_cpu+0x8>)
 1ca:	4b02      	ldr	r3, [pc, #8]	; (1d4 <halt_cpu+0xc>)
 1cc:	601a      	str	r2, [r3, #0]
 1ce:	4770      	bx	lr
 1d0:	cafef00d 	.word	0xcafef00d
 1d4:	affff000 	.word	0xaffff000

Disassembly of section .text.arb_debug_reg:

000001d8 <arb_debug_reg>:
 1d8:	4b01      	ldr	r3, [pc, #4]	; (1e0 <arb_debug_reg+0x8>)
 1da:	6018      	str	r0, [r3, #0]
 1dc:	4770      	bx	lr
 1de:	46c0      	nop			; (mov r8, r8)
 1e0:	affffff8 	.word	0xaffffff8

Disassembly of section .text.mbus_write_message:

000001e4 <mbus_write_message>:
 1e4:	2300      	movs	r3, #0
 1e6:	b500      	push	{lr}
 1e8:	429a      	cmp	r2, r3
 1ea:	d00a      	beq.n	202 <mbus_write_message+0x1e>
 1ec:	4b06      	ldr	r3, [pc, #24]	; (208 <mbus_write_message+0x24>)
 1ee:	3a01      	subs	r2, #1
 1f0:	0600      	lsls	r0, r0, #24
 1f2:	4302      	orrs	r2, r0
 1f4:	601a      	str	r2, [r3, #0]
 1f6:	4b05      	ldr	r3, [pc, #20]	; (20c <mbus_write_message+0x28>)
 1f8:	2223      	movs	r2, #35	; 0x23
 1fa:	6019      	str	r1, [r3, #0]
 1fc:	4b04      	ldr	r3, [pc, #16]	; (210 <mbus_write_message+0x2c>)
 1fe:	601a      	str	r2, [r3, #0]
 200:	2301      	movs	r3, #1
 202:	1c18      	adds	r0, r3, #0
 204:	bd00      	pop	{pc}
 206:	46c0      	nop			; (mov r8, r8)
 208:	a0002000 	.word	0xa0002000
 20c:	a0002004 	.word	0xa0002004
 210:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_enumerate:

00000214 <mbus_enumerate>:
 214:	0603      	lsls	r3, r0, #24
 216:	2080      	movs	r0, #128	; 0x80
 218:	0580      	lsls	r0, r0, #22
 21a:	4318      	orrs	r0, r3
 21c:	4b01      	ldr	r3, [pc, #4]	; (224 <mbus_enumerate+0x10>)
 21e:	6018      	str	r0, [r3, #0]
 220:	4770      	bx	lr
 222:	46c0      	nop			; (mov r8, r8)
 224:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

00000228 <mbus_sleep_all>:
 228:	4b01      	ldr	r3, [pc, #4]	; (230 <mbus_sleep_all+0x8>)
 22a:	2200      	movs	r2, #0
 22c:	601a      	str	r2, [r3, #0]
 22e:	4770      	bx	lr
 230:	a0003010 	.word	0xa0003010

Disassembly of section .text.mbus_copy_registers_from_local_to_remote:

00000234 <mbus_copy_registers_from_local_to_remote>:
 234:	0700      	lsls	r0, r0, #28
 236:	b510      	push	{r4, lr}
 238:	0c00      	lsrs	r0, r0, #16
 23a:	4301      	orrs	r1, r0
 23c:	0612      	lsls	r2, r2, #24
 23e:	4311      	orrs	r1, r2
 240:	041b      	lsls	r3, r3, #16
 242:	4319      	orrs	r1, r3
 244:	4c02      	ldr	r4, [pc, #8]	; (250 <mbus_copy_registers_from_local_to_remote+0x1c>)
 246:	4b03      	ldr	r3, [pc, #12]	; (254 <mbus_copy_registers_from_local_to_remote+0x20>)
 248:	2211      	movs	r2, #17
 24a:	6021      	str	r1, [r4, #0]
 24c:	601a      	str	r2, [r3, #0]
 24e:	bd10      	pop	{r4, pc}
 250:	a0002000 	.word	0xa0002000
 254:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_copy_registers_from_remote_to_local:

00000258 <mbus_copy_registers_from_remote_to_local>:
 258:	b507      	push	{r0, r1, r2, lr}
 25a:	0609      	lsls	r1, r1, #24
 25c:	430a      	orrs	r2, r1
 25e:	2180      	movs	r1, #128	; 0x80
 260:	0149      	lsls	r1, r1, #5
 262:	430a      	orrs	r2, r1
 264:	041b      	lsls	r3, r3, #16
 266:	431a      	orrs	r2, r3
 268:	9201      	str	r2, [sp, #4]
 26a:	0100      	lsls	r0, r0, #4
 26c:	2201      	movs	r2, #1
 26e:	4310      	orrs	r0, r2
 270:	b2c0      	uxtb	r0, r0
 272:	a901      	add	r1, sp, #4
 274:	f7ff ffb6 	bl	1e4 <mbus_write_message>
 278:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_remote_register_write:

0000027a <mbus_remote_register_write>:
 27a:	b507      	push	{r0, r1, r2, lr}
 27c:	0212      	lsls	r2, r2, #8
 27e:	0a12      	lsrs	r2, r2, #8
 280:	0609      	lsls	r1, r1, #24
 282:	4311      	orrs	r1, r2
 284:	0100      	lsls	r0, r0, #4
 286:	9101      	str	r1, [sp, #4]
 288:	b2c0      	uxtb	r0, r0
 28a:	a901      	add	r1, sp, #4
 28c:	2201      	movs	r2, #1
 28e:	f7ff ffa9 	bl	1e4 <mbus_write_message>
 292:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_copy_mem_from_local_to_remote_bulk:

00000294 <mbus_copy_mem_from_local_to_remote_bulk>:
 294:	b510      	push	{r4, lr}
 296:	2480      	movs	r4, #128	; 0x80
 298:	04a4      	lsls	r4, r4, #18
 29a:	0700      	lsls	r0, r0, #28
 29c:	031b      	lsls	r3, r3, #12
 29e:	4320      	orrs	r0, r4
 2a0:	0b1b      	lsrs	r3, r3, #12
 2a2:	4318      	orrs	r0, r3
 2a4:	4b04      	ldr	r3, [pc, #16]	; (2b8 <mbus_copy_mem_from_local_to_remote_bulk+0x24>)
 2a6:	6018      	str	r0, [r3, #0]
 2a8:	4b04      	ldr	r3, [pc, #16]	; (2bc <mbus_copy_mem_from_local_to_remote_bulk+0x28>)
 2aa:	601a      	str	r2, [r3, #0]
 2ac:	4b04      	ldr	r3, [pc, #16]	; (2c0 <mbus_copy_mem_from_local_to_remote_bulk+0x2c>)
 2ae:	2233      	movs	r2, #51	; 0x33
 2b0:	6019      	str	r1, [r3, #0]
 2b2:	4b04      	ldr	r3, [pc, #16]	; (2c4 <mbus_copy_mem_from_local_to_remote_bulk+0x30>)
 2b4:	601a      	str	r2, [r3, #0]
 2b6:	bd10      	pop	{r4, pc}
 2b8:	a0002000 	.word	0xa0002000
 2bc:	a0002004 	.word	0xa0002004
 2c0:	a0002008 	.word	0xa0002008
 2c4:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_copy_mem_from_remote_to_any_bulk:

000002c8 <mbus_copy_mem_from_remote_to_any_bulk>:
 2c8:	b530      	push	{r4, r5, lr}
 2ca:	b085      	sub	sp, #20
 2cc:	9d08      	ldr	r5, [sp, #32]
 2ce:	2480      	movs	r4, #128	; 0x80
 2d0:	04a4      	lsls	r4, r4, #18
 2d2:	0712      	lsls	r2, r2, #28
 2d4:	4322      	orrs	r2, r4
 2d6:	032c      	lsls	r4, r5, #12
 2d8:	0b24      	lsrs	r4, r4, #12
 2da:	4322      	orrs	r2, r4
 2dc:	9201      	str	r2, [sp, #4]
 2de:	0100      	lsls	r0, r0, #4
 2e0:	2203      	movs	r2, #3
 2e2:	4310      	orrs	r0, r2
 2e4:	9102      	str	r1, [sp, #8]
 2e6:	b2c0      	uxtb	r0, r0
 2e8:	a901      	add	r1, sp, #4
 2ea:	9303      	str	r3, [sp, #12]
 2ec:	f7ff ff7a 	bl	1e4 <mbus_write_message>
 2f0:	b005      	add	sp, #20
 2f2:	bd30      	pop	{r4, r5, pc}

Disassembly of section .text.mbus_copy_mem_from_local_to_remote_stream:

000002f4 <mbus_copy_mem_from_local_to_remote_stream>:
 2f4:	b510      	push	{r4, lr}
 2f6:	2480      	movs	r4, #128	; 0x80
 2f8:	04e4      	lsls	r4, r4, #19
 2fa:	0600      	lsls	r0, r0, #24
 2fc:	4320      	orrs	r0, r4
 2fe:	031b      	lsls	r3, r3, #12
 300:	070c      	lsls	r4, r1, #28
 302:	1c01      	adds	r1, r0, #0
 304:	4321      	orrs	r1, r4
 306:	0b1b      	lsrs	r3, r3, #12
 308:	4319      	orrs	r1, r3
 30a:	4b04      	ldr	r3, [pc, #16]	; (31c <mbus_copy_mem_from_local_to_remote_stream+0x28>)
 30c:	6019      	str	r1, [r3, #0]
 30e:	4b04      	ldr	r3, [pc, #16]	; (320 <mbus_copy_mem_from_local_to_remote_stream+0x2c>)
 310:	601a      	str	r2, [r3, #0]
 312:	4b04      	ldr	r3, [pc, #16]	; (324 <mbus_copy_mem_from_local_to_remote_stream+0x30>)
 314:	2223      	movs	r2, #35	; 0x23
 316:	601a      	str	r2, [r3, #0]
 318:	bd10      	pop	{r4, pc}
 31a:	46c0      	nop			; (mov r8, r8)
 31c:	a0002000 	.word	0xa0002000
 320:	a0002004 	.word	0xa0002004
 324:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_copy_mem_from_remote_to_any_stream:

00000328 <mbus_copy_mem_from_remote_to_any_stream>:
 328:	b513      	push	{r0, r1, r4, lr}
 32a:	2480      	movs	r4, #128	; 0x80
 32c:	04e4      	lsls	r4, r4, #19
 32e:	0600      	lsls	r0, r0, #24
 330:	4320      	orrs	r0, r4
 332:	071b      	lsls	r3, r3, #28
 334:	4318      	orrs	r0, r3
 336:	9b04      	ldr	r3, [sp, #16]
 338:	0109      	lsls	r1, r1, #4
 33a:	031c      	lsls	r4, r3, #12
 33c:	0b24      	lsrs	r4, r4, #12
 33e:	4320      	orrs	r0, r4
 340:	9000      	str	r0, [sp, #0]
 342:	2003      	movs	r0, #3
 344:	4308      	orrs	r0, r1
 346:	9201      	str	r2, [sp, #4]
 348:	4669      	mov	r1, sp
 34a:	2202      	movs	r2, #2
 34c:	b2c0      	uxtb	r0, r0
 34e:	f7ff ff49 	bl	1e4 <mbus_write_message>
 352:	bd13      	pop	{r0, r1, r4, pc}

Disassembly of section .text.handler_ext_int_0:

00000354 <handler_ext_int_0>:
 354:	4b14      	ldr	r3, [pc, #80]	; (3a8 <handler_ext_int_0+0x54>)
 356:	2201      	movs	r2, #1
 358:	b570      	push	{r4, r5, r6, lr}
 35a:	601a      	str	r2, [r3, #0]
 35c:	4b13      	ldr	r3, [pc, #76]	; (3ac <handler_ext_int_0+0x58>)
 35e:	24a0      	movs	r4, #160	; 0xa0
 360:	6819      	ldr	r1, [r3, #0]
 362:	0624      	lsls	r4, r4, #24
 364:	430a      	orrs	r2, r1
 366:	601a      	str	r2, [r3, #0]
 368:	2380      	movs	r3, #128	; 0x80
 36a:	015b      	lsls	r3, r3, #5
 36c:	6023      	str	r3, [r4, #0]
 36e:	4b10      	ldr	r3, [pc, #64]	; (3b0 <handler_ext_int_0+0x5c>)
 370:	4e10      	ldr	r6, [pc, #64]	; (3b4 <handler_ext_int_0+0x60>)
 372:	681b      	ldr	r3, [r3, #0]
 374:	4d10      	ldr	r5, [pc, #64]	; (3b8 <handler_ext_int_0+0x64>)
 376:	6033      	str	r3, [r6, #0]
 378:	4b10      	ldr	r3, [pc, #64]	; (3bc <handler_ext_int_0+0x68>)
 37a:	1c20      	adds	r0, r4, #0
 37c:	681a      	ldr	r2, [r3, #0]
 37e:	602a      	str	r2, [r5, #0]
 380:	2200      	movs	r2, #0
 382:	601a      	str	r2, [r3, #0]
 384:	f7ff ff28 	bl	1d8 <arb_debug_reg>
 388:	6820      	ldr	r0, [r4, #0]
 38a:	24a1      	movs	r4, #161	; 0xa1
 38c:	0624      	lsls	r4, r4, #24
 38e:	4320      	orrs	r0, r4
 390:	f7ff ff22 	bl	1d8 <arb_debug_reg>
 394:	6830      	ldr	r0, [r6, #0]
 396:	4320      	orrs	r0, r4
 398:	f7ff ff1e 	bl	1d8 <arb_debug_reg>
 39c:	6828      	ldr	r0, [r5, #0]
 39e:	4320      	orrs	r0, r4
 3a0:	f7ff ff1a 	bl	1d8 <arb_debug_reg>
 3a4:	bd70      	pop	{r4, r5, r6, pc}
 3a6:	46c0      	nop			; (mov r8, r8)
 3a8:	e000e280 	.word	0xe000e280
 3ac:	00000dd0 	.word	0x00000dd0
 3b0:	a000110c 	.word	0xa000110c
 3b4:	a0000004 	.word	0xa0000004
 3b8:	a0000008 	.word	0xa0000008
 3bc:	a0001110 	.word	0xa0001110

Disassembly of section .text.handler_ext_int_1:

000003c0 <handler_ext_int_1>:
 3c0:	4b13      	ldr	r3, [pc, #76]	; (410 <handler_ext_int_1+0x50>)
 3c2:	2202      	movs	r2, #2
 3c4:	b570      	push	{r4, r5, r6, lr}
 3c6:	601a      	str	r2, [r3, #0]
 3c8:	4b12      	ldr	r3, [pc, #72]	; (414 <handler_ext_int_1+0x54>)
 3ca:	24a0      	movs	r4, #160	; 0xa0
 3cc:	6819      	ldr	r1, [r3, #0]
 3ce:	0624      	lsls	r4, r4, #24
 3d0:	430a      	orrs	r2, r1
 3d2:	601a      	str	r2, [r3, #0]
 3d4:	4b10      	ldr	r3, [pc, #64]	; (418 <handler_ext_int_1+0x58>)
 3d6:	4e11      	ldr	r6, [pc, #68]	; (41c <handler_ext_int_1+0x5c>)
 3d8:	6023      	str	r3, [r4, #0]
 3da:	4b11      	ldr	r3, [pc, #68]	; (420 <handler_ext_int_1+0x60>)
 3dc:	4d11      	ldr	r5, [pc, #68]	; (424 <handler_ext_int_1+0x64>)
 3de:	681b      	ldr	r3, [r3, #0]
 3e0:	4811      	ldr	r0, [pc, #68]	; (428 <handler_ext_int_1+0x68>)
 3e2:	6033      	str	r3, [r6, #0]
 3e4:	4b11      	ldr	r3, [pc, #68]	; (42c <handler_ext_int_1+0x6c>)
 3e6:	681a      	ldr	r2, [r3, #0]
 3e8:	602a      	str	r2, [r5, #0]
 3ea:	2200      	movs	r2, #0
 3ec:	601a      	str	r2, [r3, #0]
 3ee:	f7ff fef3 	bl	1d8 <arb_debug_reg>
 3f2:	6820      	ldr	r0, [r4, #0]
 3f4:	24a1      	movs	r4, #161	; 0xa1
 3f6:	0624      	lsls	r4, r4, #24
 3f8:	4320      	orrs	r0, r4
 3fa:	f7ff feed 	bl	1d8 <arb_debug_reg>
 3fe:	6830      	ldr	r0, [r6, #0]
 400:	4320      	orrs	r0, r4
 402:	f7ff fee9 	bl	1d8 <arb_debug_reg>
 406:	6828      	ldr	r0, [r5, #0]
 408:	4320      	orrs	r0, r4
 40a:	f7ff fee5 	bl	1d8 <arb_debug_reg>
 40e:	bd70      	pop	{r4, r5, r6, pc}
 410:	e000e280 	.word	0xe000e280
 414:	00000dd0 	.word	0x00000dd0
 418:	00001001 	.word	0x00001001
 41c:	a0000004 	.word	0xa0000004
 420:	a0001020 	.word	0xa0001020
 424:	a0000008 	.word	0xa0000008
 428:	a0000001 	.word	0xa0000001
 42c:	a0001024 	.word	0xa0001024

Disassembly of section .text.handler_ext_int_2:

00000430 <handler_ext_int_2>:
 430:	b508      	push	{r3, lr}
 432:	4b05      	ldr	r3, [pc, #20]	; (448 <handler_ext_int_2+0x18>)
 434:	2204      	movs	r2, #4
 436:	601a      	str	r2, [r3, #0]
 438:	4b04      	ldr	r3, [pc, #16]	; (44c <handler_ext_int_2+0x1c>)
 43a:	4805      	ldr	r0, [pc, #20]	; (450 <handler_ext_int_2+0x20>)
 43c:	6819      	ldr	r1, [r3, #0]
 43e:	430a      	orrs	r2, r1
 440:	601a      	str	r2, [r3, #0]
 442:	f7ff fec9 	bl	1d8 <arb_debug_reg>
 446:	bd08      	pop	{r3, pc}
 448:	e000e280 	.word	0xe000e280
 44c:	00000dd0 	.word	0x00000dd0
 450:	a0000002 	.word	0xa0000002

Disassembly of section .text.handler_ext_int_3:

00000454 <handler_ext_int_3>:
 454:	b508      	push	{r3, lr}
 456:	4b05      	ldr	r3, [pc, #20]	; (46c <handler_ext_int_3+0x18>)
 458:	2208      	movs	r2, #8
 45a:	601a      	str	r2, [r3, #0]
 45c:	4b04      	ldr	r3, [pc, #16]	; (470 <handler_ext_int_3+0x1c>)
 45e:	4805      	ldr	r0, [pc, #20]	; (474 <handler_ext_int_3+0x20>)
 460:	6819      	ldr	r1, [r3, #0]
 462:	430a      	orrs	r2, r1
 464:	601a      	str	r2, [r3, #0]
 466:	f7ff feb7 	bl	1d8 <arb_debug_reg>
 46a:	bd08      	pop	{r3, pc}
 46c:	e000e280 	.word	0xe000e280
 470:	00000dd0 	.word	0x00000dd0
 474:	a0000003 	.word	0xa0000003

Disassembly of section .text.handler_ext_int_4:

00000478 <handler_ext_int_4>:
 478:	b508      	push	{r3, lr}
 47a:	4b05      	ldr	r3, [pc, #20]	; (490 <handler_ext_int_4+0x18>)
 47c:	2210      	movs	r2, #16
 47e:	601a      	str	r2, [r3, #0]
 480:	4b04      	ldr	r3, [pc, #16]	; (494 <handler_ext_int_4+0x1c>)
 482:	4805      	ldr	r0, [pc, #20]	; (498 <handler_ext_int_4+0x20>)
 484:	6819      	ldr	r1, [r3, #0]
 486:	430a      	orrs	r2, r1
 488:	601a      	str	r2, [r3, #0]
 48a:	f7ff fea5 	bl	1d8 <arb_debug_reg>
 48e:	bd08      	pop	{r3, pc}
 490:	e000e280 	.word	0xe000e280
 494:	00000dd0 	.word	0x00000dd0
 498:	a0000004 	.word	0xa0000004

Disassembly of section .text.handler_ext_int_5:

0000049c <handler_ext_int_5>:
 49c:	b508      	push	{r3, lr}
 49e:	4b05      	ldr	r3, [pc, #20]	; (4b4 <handler_ext_int_5+0x18>)
 4a0:	2220      	movs	r2, #32
 4a2:	601a      	str	r2, [r3, #0]
 4a4:	4b04      	ldr	r3, [pc, #16]	; (4b8 <handler_ext_int_5+0x1c>)
 4a6:	4805      	ldr	r0, [pc, #20]	; (4bc <handler_ext_int_5+0x20>)
 4a8:	6819      	ldr	r1, [r3, #0]
 4aa:	430a      	orrs	r2, r1
 4ac:	601a      	str	r2, [r3, #0]
 4ae:	f7ff fe93 	bl	1d8 <arb_debug_reg>
 4b2:	bd08      	pop	{r3, pc}
 4b4:	e000e280 	.word	0xe000e280
 4b8:	00000dd0 	.word	0x00000dd0
 4bc:	a0000005 	.word	0xa0000005

Disassembly of section .text.handler_ext_int_6:

000004c0 <handler_ext_int_6>:
 4c0:	b508      	push	{r3, lr}
 4c2:	4b05      	ldr	r3, [pc, #20]	; (4d8 <handler_ext_int_6+0x18>)
 4c4:	2240      	movs	r2, #64	; 0x40
 4c6:	601a      	str	r2, [r3, #0]
 4c8:	4b04      	ldr	r3, [pc, #16]	; (4dc <handler_ext_int_6+0x1c>)
 4ca:	4805      	ldr	r0, [pc, #20]	; (4e0 <handler_ext_int_6+0x20>)
 4cc:	6819      	ldr	r1, [r3, #0]
 4ce:	430a      	orrs	r2, r1
 4d0:	601a      	str	r2, [r3, #0]
 4d2:	f7ff fe81 	bl	1d8 <arb_debug_reg>
 4d6:	bd08      	pop	{r3, pc}
 4d8:	e000e280 	.word	0xe000e280
 4dc:	00000dd0 	.word	0x00000dd0
 4e0:	a0000006 	.word	0xa0000006

Disassembly of section .text.handler_ext_int_7:

000004e4 <handler_ext_int_7>:
 4e4:	b508      	push	{r3, lr}
 4e6:	4b05      	ldr	r3, [pc, #20]	; (4fc <handler_ext_int_7+0x18>)
 4e8:	2280      	movs	r2, #128	; 0x80
 4ea:	601a      	str	r2, [r3, #0]
 4ec:	4b04      	ldr	r3, [pc, #16]	; (500 <handler_ext_int_7+0x1c>)
 4ee:	4805      	ldr	r0, [pc, #20]	; (504 <handler_ext_int_7+0x20>)
 4f0:	6819      	ldr	r1, [r3, #0]
 4f2:	430a      	orrs	r2, r1
 4f4:	601a      	str	r2, [r3, #0]
 4f6:	f7ff fe6f 	bl	1d8 <arb_debug_reg>
 4fa:	bd08      	pop	{r3, pc}
 4fc:	e000e280 	.word	0xe000e280
 500:	00000dd0 	.word	0x00000dd0
 504:	a0000007 	.word	0xa0000007

Disassembly of section .text.handler_ext_int_8:

00000508 <handler_ext_int_8>:
 508:	b508      	push	{r3, lr}
 50a:	2280      	movs	r2, #128	; 0x80
 50c:	4b05      	ldr	r3, [pc, #20]	; (524 <handler_ext_int_8+0x1c>)
 50e:	0052      	lsls	r2, r2, #1
 510:	601a      	str	r2, [r3, #0]
 512:	4b05      	ldr	r3, [pc, #20]	; (528 <handler_ext_int_8+0x20>)
 514:	4805      	ldr	r0, [pc, #20]	; (52c <handler_ext_int_8+0x24>)
 516:	6819      	ldr	r1, [r3, #0]
 518:	430a      	orrs	r2, r1
 51a:	601a      	str	r2, [r3, #0]
 51c:	f7ff fe5c 	bl	1d8 <arb_debug_reg>
 520:	bd08      	pop	{r3, pc}
 522:	46c0      	nop			; (mov r8, r8)
 524:	e000e280 	.word	0xe000e280
 528:	00000dd0 	.word	0x00000dd0
 52c:	a0000008 	.word	0xa0000008

Disassembly of section .text.handler_ext_int_9:

00000530 <handler_ext_int_9>:
 530:	b508      	push	{r3, lr}
 532:	2280      	movs	r2, #128	; 0x80
 534:	4b05      	ldr	r3, [pc, #20]	; (54c <handler_ext_int_9+0x1c>)
 536:	0092      	lsls	r2, r2, #2
 538:	601a      	str	r2, [r3, #0]
 53a:	4b05      	ldr	r3, [pc, #20]	; (550 <handler_ext_int_9+0x20>)
 53c:	4805      	ldr	r0, [pc, #20]	; (554 <handler_ext_int_9+0x24>)
 53e:	6819      	ldr	r1, [r3, #0]
 540:	430a      	orrs	r2, r1
 542:	601a      	str	r2, [r3, #0]
 544:	f7ff fe48 	bl	1d8 <arb_debug_reg>
 548:	bd08      	pop	{r3, pc}
 54a:	46c0      	nop			; (mov r8, r8)
 54c:	e000e280 	.word	0xe000e280
 550:	00000dd0 	.word	0x00000dd0
 554:	a0000009 	.word	0xa0000009

Disassembly of section .text.handler_ext_int_10:

00000558 <handler_ext_int_10>:
 558:	b508      	push	{r3, lr}
 55a:	2280      	movs	r2, #128	; 0x80
 55c:	4b05      	ldr	r3, [pc, #20]	; (574 <handler_ext_int_10+0x1c>)
 55e:	00d2      	lsls	r2, r2, #3
 560:	601a      	str	r2, [r3, #0]
 562:	4b05      	ldr	r3, [pc, #20]	; (578 <handler_ext_int_10+0x20>)
 564:	4805      	ldr	r0, [pc, #20]	; (57c <handler_ext_int_10+0x24>)
 566:	6819      	ldr	r1, [r3, #0]
 568:	430a      	orrs	r2, r1
 56a:	601a      	str	r2, [r3, #0]
 56c:	f7ff fe34 	bl	1d8 <arb_debug_reg>
 570:	bd08      	pop	{r3, pc}
 572:	46c0      	nop			; (mov r8, r8)
 574:	e000e280 	.word	0xe000e280
 578:	00000dd0 	.word	0x00000dd0
 57c:	a000000a 	.word	0xa000000a

Disassembly of section .text.handler_ext_int_11:

00000580 <handler_ext_int_11>:
 580:	b508      	push	{r3, lr}
 582:	2280      	movs	r2, #128	; 0x80
 584:	4b05      	ldr	r3, [pc, #20]	; (59c <handler_ext_int_11+0x1c>)
 586:	0112      	lsls	r2, r2, #4
 588:	601a      	str	r2, [r3, #0]
 58a:	4b05      	ldr	r3, [pc, #20]	; (5a0 <handler_ext_int_11+0x20>)
 58c:	4805      	ldr	r0, [pc, #20]	; (5a4 <handler_ext_int_11+0x24>)
 58e:	6819      	ldr	r1, [r3, #0]
 590:	430a      	orrs	r2, r1
 592:	601a      	str	r2, [r3, #0]
 594:	f7ff fe20 	bl	1d8 <arb_debug_reg>
 598:	bd08      	pop	{r3, pc}
 59a:	46c0      	nop			; (mov r8, r8)
 59c:	e000e280 	.word	0xe000e280
 5a0:	00000dd0 	.word	0x00000dd0
 5a4:	a000000b 	.word	0xa000000b

Disassembly of section .text.handler_ext_int_12:

000005a8 <handler_ext_int_12>:
 5a8:	b508      	push	{r3, lr}
 5aa:	2280      	movs	r2, #128	; 0x80
 5ac:	4b05      	ldr	r3, [pc, #20]	; (5c4 <handler_ext_int_12+0x1c>)
 5ae:	0152      	lsls	r2, r2, #5
 5b0:	601a      	str	r2, [r3, #0]
 5b2:	4b05      	ldr	r3, [pc, #20]	; (5c8 <handler_ext_int_12+0x20>)
 5b4:	4805      	ldr	r0, [pc, #20]	; (5cc <handler_ext_int_12+0x24>)
 5b6:	6819      	ldr	r1, [r3, #0]
 5b8:	430a      	orrs	r2, r1
 5ba:	601a      	str	r2, [r3, #0]
 5bc:	f7ff fe0c 	bl	1d8 <arb_debug_reg>
 5c0:	bd08      	pop	{r3, pc}
 5c2:	46c0      	nop			; (mov r8, r8)
 5c4:	e000e280 	.word	0xe000e280
 5c8:	00000dd0 	.word	0x00000dd0
 5cc:	a000000c 	.word	0xa000000c

Disassembly of section .text.handler_ext_int_13:

000005d0 <handler_ext_int_13>:
 5d0:	b508      	push	{r3, lr}
 5d2:	2280      	movs	r2, #128	; 0x80
 5d4:	4b05      	ldr	r3, [pc, #20]	; (5ec <handler_ext_int_13+0x1c>)
 5d6:	0192      	lsls	r2, r2, #6
 5d8:	601a      	str	r2, [r3, #0]
 5da:	4b05      	ldr	r3, [pc, #20]	; (5f0 <handler_ext_int_13+0x20>)
 5dc:	4805      	ldr	r0, [pc, #20]	; (5f4 <handler_ext_int_13+0x24>)
 5de:	6819      	ldr	r1, [r3, #0]
 5e0:	430a      	orrs	r2, r1
 5e2:	601a      	str	r2, [r3, #0]
 5e4:	f7ff fdf8 	bl	1d8 <arb_debug_reg>
 5e8:	bd08      	pop	{r3, pc}
 5ea:	46c0      	nop			; (mov r8, r8)
 5ec:	e000e280 	.word	0xe000e280
 5f0:	00000dd0 	.word	0x00000dd0
 5f4:	a000000d 	.word	0xa000000d

Disassembly of section .text.initialization:

000005f8 <initialization>:
 5f8:	b508      	push	{r3, lr}
 5fa:	4a0c      	ldr	r2, [pc, #48]	; (62c <initialization+0x34>)
 5fc:	4b0c      	ldr	r3, [pc, #48]	; (630 <initialization+0x38>)
 5fe:	601a      	str	r2, [r3, #0]
 600:	4a0c      	ldr	r2, [pc, #48]	; (634 <initialization+0x3c>)
 602:	2300      	movs	r3, #0
 604:	6013      	str	r3, [r2, #0]
 606:	4a0c      	ldr	r2, [pc, #48]	; (638 <initialization+0x40>)
 608:	6013      	str	r3, [r2, #0]
 60a:	f7ff fdb3 	bl	174 <set_halt_until_mbus_rx>
 60e:	2004      	movs	r0, #4
 610:	f7ff fe00 	bl	214 <mbus_enumerate>
 614:	2008      	movs	r0, #8
 616:	f7ff fdfd 	bl	214 <mbus_enumerate>
 61a:	200c      	movs	r0, #12
 61c:	f7ff fdfa 	bl	214 <mbus_enumerate>
 620:	200e      	movs	r0, #14
 622:	f7ff fdf7 	bl	214 <mbus_enumerate>
 626:	f7ff fdb3 	bl	190 <set_halt_until_mbus_tx>
 62a:	bd08      	pop	{r3, pc}
 62c:	deadbeef 	.word	0xdeadbeef
 630:	00000dcc 	.word	0x00000dcc
 634:	00000dc8 	.word	0x00000dc8
 638:	00000dd0 	.word	0x00000dd0

Disassembly of section .text.pass:

0000063c <pass>:
 63c:	b538      	push	{r3, r4, r5, lr}
 63e:	1c05      	adds	r5, r0, #0
 640:	4805      	ldr	r0, [pc, #20]	; (658 <pass+0x1c>)
 642:	1c0c      	adds	r4, r1, #0
 644:	f7ff fdc8 	bl	1d8 <arb_debug_reg>
 648:	1c28      	adds	r0, r5, #0
 64a:	f7ff fdc5 	bl	1d8 <arb_debug_reg>
 64e:	1c20      	adds	r0, r4, #0
 650:	f7ff fdc2 	bl	1d8 <arb_debug_reg>
 654:	bd38      	pop	{r3, r4, r5, pc}
 656:	46c0      	nop			; (mov r8, r8)
 658:	0ea7f00d 	.word	0x0ea7f00d

Disassembly of section .text.fail:

0000065c <fail>:
 65c:	b538      	push	{r3, r4, r5, lr}
 65e:	1c05      	adds	r5, r0, #0
 660:	4806      	ldr	r0, [pc, #24]	; (67c <fail+0x20>)
 662:	1c0c      	adds	r4, r1, #0
 664:	f7ff fdb8 	bl	1d8 <arb_debug_reg>
 668:	1c28      	adds	r0, r5, #0
 66a:	f7ff fdb5 	bl	1d8 <arb_debug_reg>
 66e:	1c20      	adds	r0, r4, #0
 670:	f7ff fdb2 	bl	1d8 <arb_debug_reg>
 674:	4a02      	ldr	r2, [pc, #8]	; (680 <fail+0x24>)
 676:	4b03      	ldr	r3, [pc, #12]	; (684 <fail+0x28>)
 678:	601a      	str	r2, [r3, #0]
 67a:	bd38      	pop	{r3, r4, r5, pc}
 67c:	deadbeef 	.word	0xdeadbeef
 680:	0000ffff 	.word	0x0000ffff
 684:	a0000020 	.word	0xa0000020

Disassembly of section .text.cycle1:

00000688 <cycle1>:
 688:	b538      	push	{r3, r4, r5, lr}
 68a:	4b3b      	ldr	r3, [pc, #236]	; (778 <cycle1+0xf0>)
 68c:	681d      	ldr	r5, [r3, #0]
 68e:	2d01      	cmp	r5, #1
 690:	d170      	bne.n	774 <cycle1+0xec>
 692:	f7ff fd6f 	bl	174 <set_halt_until_mbus_rx>
 696:	24a0      	movs	r4, #160	; 0xa0
 698:	200e      	movs	r0, #14
 69a:	2100      	movs	r1, #0
 69c:	0624      	lsls	r4, r4, #24
 69e:	1c2a      	adds	r2, r5, #0
 6a0:	f7ff fdeb 	bl	27a <mbus_remote_register_write>
 6a4:	6823      	ldr	r3, [r4, #0]
 6a6:	20a0      	movs	r0, #160	; 0xa0
 6a8:	6821      	ldr	r1, [r4, #0]
 6aa:	42ab      	cmp	r3, r5
 6ac:	d104      	bne.n	6b8 <cycle1+0x30>
 6ae:	f7ff ffc5 	bl	63c <pass>
 6b2:	2300      	movs	r3, #0
 6b4:	6023      	str	r3, [r4, #0]
 6b6:	e001      	b.n	6bc <cycle1+0x34>
 6b8:	f7ff ffd0 	bl	65c <fail>
 6bc:	3501      	adds	r5, #1
 6be:	2d42      	cmp	r5, #66	; 0x42
 6c0:	d1e9      	bne.n	696 <cycle1+0xe>
 6c2:	f7ff fd65 	bl	190 <set_halt_until_mbus_tx>
 6c6:	4b2d      	ldr	r3, [pc, #180]	; (77c <cycle1+0xf4>)
 6c8:	22ff      	movs	r2, #255	; 0xff
 6ca:	8819      	ldrh	r1, [r3, #0]
 6cc:	2401      	movs	r4, #1
 6ce:	4391      	bics	r1, r2
 6d0:	4321      	orrs	r1, r4
 6d2:	8019      	strh	r1, [r3, #0]
 6d4:	8819      	ldrh	r1, [r3, #0]
 6d6:	200e      	movs	r0, #14
 6d8:	400a      	ands	r2, r1
 6da:	2180      	movs	r1, #128	; 0x80
 6dc:	0149      	lsls	r1, r1, #5
 6de:	430a      	orrs	r2, r1
 6e0:	801a      	strh	r2, [r3, #0]
 6e2:	681a      	ldr	r2, [r3, #0]
 6e4:	1c29      	adds	r1, r5, #0
 6e6:	f7ff fdc8 	bl	27a <mbus_remote_register_write>
 6ea:	f7ff fd43 	bl	174 <set_halt_until_mbus_rx>
 6ee:	b2e1      	uxtb	r1, r4
 6f0:	4d23      	ldr	r5, [pc, #140]	; (780 <cycle1+0xf8>)
 6f2:	3401      	adds	r4, #1
 6f4:	200e      	movs	r0, #14
 6f6:	1c22      	adds	r2, r4, #0
 6f8:	f7ff fdbf 	bl	27a <mbus_remote_register_write>
 6fc:	682b      	ldr	r3, [r5, #0]
 6fe:	20a1      	movs	r0, #161	; 0xa1
 700:	6829      	ldr	r1, [r5, #0]
 702:	42a3      	cmp	r3, r4
 704:	d104      	bne.n	710 <cycle1+0x88>
 706:	f7ff ff99 	bl	63c <pass>
 70a:	2300      	movs	r3, #0
 70c:	602b      	str	r3, [r5, #0]
 70e:	e001      	b.n	714 <cycle1+0x8c>
 710:	f7ff ffa4 	bl	65c <fail>
 714:	2c41      	cmp	r4, #65	; 0x41
 716:	d9ea      	bls.n	6ee <cycle1+0x66>
 718:	f7ff fd3a 	bl	190 <set_halt_until_mbus_tx>
 71c:	4b17      	ldr	r3, [pc, #92]	; (77c <cycle1+0xf4>)
 71e:	22ff      	movs	r2, #255	; 0xff
 720:	8819      	ldrh	r1, [r3, #0]
 722:	2002      	movs	r0, #2
 724:	4391      	bics	r1, r2
 726:	4301      	orrs	r1, r0
 728:	8019      	strh	r1, [r3, #0]
 72a:	8819      	ldrh	r1, [r3, #0]
 72c:	200e      	movs	r0, #14
 72e:	400a      	ands	r2, r1
 730:	2180      	movs	r1, #128	; 0x80
 732:	0149      	lsls	r1, r1, #5
 734:	430a      	orrs	r2, r1
 736:	801a      	strh	r2, [r3, #0]
 738:	681a      	ldr	r2, [r3, #0]
 73a:	2142      	movs	r1, #66	; 0x42
 73c:	f7ff fd9d 	bl	27a <mbus_remote_register_write>
 740:	f7ff fd18 	bl	174 <set_halt_until_mbus_rx>
 744:	2401      	movs	r4, #1
 746:	4d0f      	ldr	r5, [pc, #60]	; (784 <cycle1+0xfc>)
 748:	200e      	movs	r0, #14
 74a:	2100      	movs	r1, #0
 74c:	1c22      	adds	r2, r4, #0
 74e:	f7ff fd94 	bl	27a <mbus_remote_register_write>
 752:	682b      	ldr	r3, [r5, #0]
 754:	3401      	adds	r4, #1
 756:	6829      	ldr	r1, [r5, #0]
 758:	20a2      	movs	r0, #162	; 0xa2
 75a:	42a3      	cmp	r3, r4
 75c:	d104      	bne.n	768 <cycle1+0xe0>
 75e:	f7ff ff6d 	bl	63c <pass>
 762:	2300      	movs	r3, #0
 764:	602b      	str	r3, [r5, #0]
 766:	e001      	b.n	76c <cycle1+0xe4>
 768:	f7ff ff78 	bl	65c <fail>
 76c:	2c41      	cmp	r4, #65	; 0x41
 76e:	d9ea      	bls.n	746 <cycle1+0xbe>
 770:	f7ff fd0e 	bl	190 <set_halt_until_mbus_tx>
 774:	bd38      	pop	{r3, r4, r5, pc}
 776:	46c0      	nop			; (mov r8, r8)
 778:	00000dd4 	.word	0x00000dd4
 77c:	00000db8 	.word	0x00000db8
 780:	a0000004 	.word	0xa0000004
 784:	a0000008 	.word	0xa0000008

Disassembly of section .text.cycle2:

00000788 <cycle2>:
 788:	4b3d      	ldr	r3, [pc, #244]	; (880 <cycle2+0xf8>)
 78a:	b570      	push	{r4, r5, r6, lr}
 78c:	681c      	ldr	r4, [r3, #0]
 78e:	2c01      	cmp	r4, #1
 790:	d175      	bne.n	87e <cycle2+0xf6>
 792:	f7ff fcef 	bl	174 <set_halt_until_mbus_rx>
 796:	2200      	movs	r2, #0
 798:	1c21      	adds	r1, r4, #0
 79a:	1c13      	adds	r3, r2, #0
 79c:	2004      	movs	r0, #4
 79e:	f7ff fd5b 	bl	258 <mbus_copy_registers_from_remote_to_local>
 7a2:	4d38      	ldr	r5, [pc, #224]	; (884 <cycle2+0xfc>)
 7a4:	1c21      	adds	r1, r4, #0
 7a6:	2004      	movs	r0, #4
 7a8:	2200      	movs	r2, #0
 7aa:	2303      	movs	r3, #3
 7ac:	26de      	movs	r6, #222	; 0xde
 7ae:	f7ff fd53 	bl	258 <mbus_copy_registers_from_remote_to_local>
 7b2:	0176      	lsls	r6, r6, #5
 7b4:	f7ff fcca 	bl	14c <clear_all_pend_irq>
 7b8:	2004      	movs	r0, #4
 7ba:	1c21      	adds	r1, r4, #0
 7bc:	1c02      	adds	r2, r0, #0
 7be:	2303      	movs	r3, #3
 7c0:	602e      	str	r6, [r5, #0]
 7c2:	f7ff fd49 	bl	258 <mbus_copy_registers_from_remote_to_local>
 7c6:	4c30      	ldr	r4, [pc, #192]	; (888 <cycle2+0x100>)
 7c8:	2000      	movs	r0, #0
 7ca:	6823      	ldr	r3, [r4, #0]
 7cc:	6821      	ldr	r1, [r4, #0]
 7ce:	42b3      	cmp	r3, r6
 7d0:	d104      	bne.n	7dc <cycle2+0x54>
 7d2:	f7ff ff33 	bl	63c <pass>
 7d6:	2300      	movs	r3, #0
 7d8:	6023      	str	r3, [r4, #0]
 7da:	e001      	b.n	7e0 <cycle2+0x58>
 7dc:	f7ff ff3e 	bl	65c <fail>
 7e0:	26c0      	movs	r6, #192	; 0xc0
 7e2:	f7ff fca7 	bl	134 <disable_all_irq>
 7e6:	0176      	lsls	r6, r6, #5
 7e8:	f7ff fcb0 	bl	14c <clear_all_pend_irq>
 7ec:	2004      	movs	r0, #4
 7ee:	2101      	movs	r1, #1
 7f0:	1c02      	adds	r2, r0, #0
 7f2:	2303      	movs	r3, #3
 7f4:	602e      	str	r6, [r5, #0]
 7f6:	f7ff fd2f 	bl	258 <mbus_copy_registers_from_remote_to_local>
 7fa:	4c23      	ldr	r4, [pc, #140]	; (888 <cycle2+0x100>)
 7fc:	2001      	movs	r0, #1
 7fe:	6823      	ldr	r3, [r4, #0]
 800:	6821      	ldr	r1, [r4, #0]
 802:	42b3      	cmp	r3, r6
 804:	d104      	bne.n	810 <cycle2+0x88>
 806:	f7ff ff19 	bl	63c <pass>
 80a:	2300      	movs	r3, #0
 80c:	6023      	str	r3, [r4, #0]
 80e:	e001      	b.n	814 <cycle2+0x8c>
 810:	f7ff ff24 	bl	65c <fail>
 814:	f7ff fc8e 	bl	134 <disable_all_irq>
 818:	2004      	movs	r0, #4
 81a:	1c01      	adds	r1, r0, #0
 81c:	1c02      	adds	r2, r0, #0
 81e:	2300      	movs	r3, #0
 820:	f7ff fd1a 	bl	258 <mbus_copy_registers_from_remote_to_local>
 824:	4c18      	ldr	r4, [pc, #96]	; (888 <cycle2+0x100>)
 826:	2002      	movs	r0, #2
 828:	6826      	ldr	r6, [r4, #0]
 82a:	6821      	ldr	r1, [r4, #0]
 82c:	2e00      	cmp	r6, #0
 82e:	d103      	bne.n	838 <cycle2+0xb0>
 830:	f7ff ff04 	bl	63c <pass>
 834:	6026      	str	r6, [r4, #0]
 836:	e001      	b.n	83c <cycle2+0xb4>
 838:	f7ff ff10 	bl	65c <fail>
 83c:	f7ff fc7a 	bl	134 <disable_all_irq>
 840:	26f0      	movs	r6, #240	; 0xf0
 842:	2007      	movs	r0, #7
 844:	f7ff fc8a 	bl	15c <set_halt_until_reg>
 848:	00b6      	lsls	r6, r6, #2
 84a:	f7ff fc7f 	bl	14c <clear_all_pend_irq>
 84e:	2004      	movs	r0, #4
 850:	2101      	movs	r1, #1
 852:	1c02      	adds	r2, r0, #0
 854:	2303      	movs	r3, #3
 856:	602e      	str	r6, [r5, #0]
 858:	f7ff fcfe 	bl	258 <mbus_copy_registers_from_remote_to_local>
 85c:	4c0a      	ldr	r4, [pc, #40]	; (888 <cycle2+0x100>)
 85e:	2003      	movs	r0, #3
 860:	6823      	ldr	r3, [r4, #0]
 862:	6821      	ldr	r1, [r4, #0]
 864:	42b3      	cmp	r3, r6
 866:	d104      	bne.n	872 <cycle2+0xea>
 868:	f7ff fee8 	bl	63c <pass>
 86c:	2300      	movs	r3, #0
 86e:	6023      	str	r3, [r4, #0]
 870:	e001      	b.n	876 <cycle2+0xee>
 872:	f7ff fef3 	bl	65c <fail>
 876:	f7ff fc5d 	bl	134 <disable_all_irq>
 87a:	f7ff fc89 	bl	190 <set_halt_until_mbus_tx>
 87e:	bd70      	pop	{r4, r5, r6, pc}
 880:	00000da4 	.word	0x00000da4
 884:	e000e100 	.word	0xe000e100
 888:	00000dd0 	.word	0x00000dd0

Disassembly of section .text.cycle3:

0000088c <cycle3>:
 88c:	4b25      	ldr	r3, [pc, #148]	; (924 <cycle3+0x98>)
 88e:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
 890:	681c      	ldr	r4, [r3, #0]
 892:	2c01      	cmp	r4, #1
 894:	d145      	bne.n	922 <cycle3+0x96>
 896:	f7ff fc6d 	bl	174 <set_halt_until_mbus_rx>
 89a:	2500      	movs	r5, #0
 89c:	23a0      	movs	r3, #160	; 0xa0
 89e:	2004      	movs	r0, #4
 8a0:	1c29      	adds	r1, r5, #0
 8a2:	1c22      	adds	r2, r4, #0
 8a4:	015b      	lsls	r3, r3, #5
 8a6:	9500      	str	r5, [sp, #0]
 8a8:	f7ff fd0e 	bl	2c8 <mbus_copy_mem_from_remote_to_any_bulk>
 8ac:	2309      	movs	r3, #9
 8ae:	9300      	str	r3, [sp, #0]
 8b0:	23a8      	movs	r3, #168	; 0xa8
 8b2:	2004      	movs	r0, #4
 8b4:	1c29      	adds	r1, r5, #0
 8b6:	1c22      	adds	r2, r4, #0
 8b8:	015b      	lsls	r3, r3, #5
 8ba:	f7ff fd05 	bl	2c8 <mbus_copy_mem_from_remote_to_any_bulk>
 8be:	4f1a      	ldr	r7, [pc, #104]	; (928 <cycle3+0x9c>)
 8c0:	23a2      	movs	r3, #162	; 0xa2
 8c2:	00db      	lsls	r3, r3, #3
 8c4:	603b      	str	r3, [r7, #0]
 8c6:	4e19      	ldr	r6, [pc, #100]	; (92c <cycle3+0xa0>)
 8c8:	23a0      	movs	r3, #160	; 0xa0
 8ca:	00db      	lsls	r3, r3, #3
 8cc:	6033      	str	r3, [r6, #0]
 8ce:	1c20      	adds	r0, r4, #0
 8d0:	2104      	movs	r1, #4
 8d2:	1c2a      	adds	r2, r5, #0
 8d4:	1c23      	adds	r3, r4, #0
 8d6:	9500      	str	r5, [sp, #0]
 8d8:	f7ff fd26 	bl	328 <mbus_copy_mem_from_remote_to_any_stream>
 8dc:	6832      	ldr	r2, [r6, #0]
 8de:	4b14      	ldr	r3, [pc, #80]	; (930 <cycle3+0xa4>)
 8e0:	6831      	ldr	r1, [r6, #0]
 8e2:	2004      	movs	r0, #4
 8e4:	429a      	cmp	r2, r3
 8e6:	d102      	bne.n	8ee <cycle3+0x62>
 8e8:	f7ff fea8 	bl	63c <pass>
 8ec:	e001      	b.n	8f2 <cycle3+0x66>
 8ee:	f7ff feb5 	bl	65c <fail>
 8f2:	2309      	movs	r3, #9
 8f4:	2000      	movs	r0, #0
 8f6:	9300      	str	r3, [sp, #0]
 8f8:	2104      	movs	r1, #4
 8fa:	1c02      	adds	r2, r0, #0
 8fc:	2301      	movs	r3, #1
 8fe:	f7ff fd13 	bl	328 <mbus_copy_mem_from_remote_to_any_stream>
 902:	6839      	ldr	r1, [r7, #0]
 904:	4a0b      	ldr	r2, [pc, #44]	; (934 <cycle3+0xa8>)
 906:	4b08      	ldr	r3, [pc, #32]	; (928 <cycle3+0x9c>)
 908:	4291      	cmp	r1, r2
 90a:	d104      	bne.n	916 <cycle3+0x8a>
 90c:	6819      	ldr	r1, [r3, #0]
 90e:	2005      	movs	r0, #5
 910:	f7ff fe94 	bl	63c <pass>
 914:	e003      	b.n	91e <cycle3+0x92>
 916:	6819      	ldr	r1, [r3, #0]
 918:	2005      	movs	r0, #5
 91a:	f7ff fe9f 	bl	65c <fail>
 91e:	f7ff fc37 	bl	190 <set_halt_until_mbus_tx>
 922:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
 924:	00000da8 	.word	0x00000da8
 928:	a00000c0 	.word	0xa00000c0
 92c:	a00000b0 	.word	0xa00000b0
 930:	00000501 	.word	0x00000501
 934:	0000051a 	.word	0x0000051a

Disassembly of section .text.cycle4:

00000938 <cycle4>:
 938:	4b57      	ldr	r3, [pc, #348]	; (a98 <cycle4+0x160>)
 93a:	b570      	push	{r4, r5, r6, lr}
 93c:	681e      	ldr	r6, [r3, #0]
 93e:	2e01      	cmp	r6, #1
 940:	d000      	beq.n	944 <cycle4+0xc>
 942:	e0a7      	b.n	a94 <cycle4+0x15c>
 944:	f7ff fc24 	bl	190 <set_halt_until_mbus_tx>
 948:	4c54      	ldr	r4, [pc, #336]	; (a9c <cycle4+0x164>)
 94a:	23ff      	movs	r3, #255	; 0xff
 94c:	8822      	ldrh	r2, [r4, #0]
 94e:	2004      	movs	r0, #4
 950:	439a      	bics	r2, r3
 952:	8022      	strh	r2, [r4, #0]
 954:	8822      	ldrh	r2, [r4, #0]
 956:	210f      	movs	r1, #15
 958:	4013      	ands	r3, r2
 95a:	2280      	movs	r2, #128	; 0x80
 95c:	0152      	lsls	r2, r2, #5
 95e:	4313      	orrs	r3, r2
 960:	8023      	strh	r3, [r4, #0]
 962:	6822      	ldr	r2, [r4, #0]
 964:	f7ff fc89 	bl	27a <mbus_remote_register_write>
 968:	4b4d      	ldr	r3, [pc, #308]	; (aa0 <cycle4+0x168>)
 96a:	2102      	movs	r1, #2
 96c:	781a      	ldrb	r2, [r3, #0]
 96e:	2504      	movs	r5, #4
 970:	4332      	orrs	r2, r6
 972:	701a      	strb	r2, [r3, #0]
 974:	781a      	ldrb	r2, [r3, #0]
 976:	1c28      	adds	r0, r5, #0
 978:	430a      	orrs	r2, r1
 97a:	701a      	strb	r2, [r3, #0]
 97c:	781a      	ldrb	r2, [r3, #0]
 97e:	2138      	movs	r1, #56	; 0x38
 980:	43aa      	bics	r2, r5
 982:	701a      	strb	r2, [r3, #0]
 984:	781a      	ldrb	r2, [r3, #0]
 986:	438a      	bics	r2, r1
 988:	701a      	strb	r2, [r3, #0]
 98a:	781a      	ldrb	r2, [r3, #0]
 98c:	2140      	movs	r1, #64	; 0x40
 98e:	438a      	bics	r2, r1
 990:	701a      	strb	r2, [r3, #0]
 992:	681a      	ldr	r2, [r3, #0]
 994:	2112      	movs	r1, #18
 996:	f7ff fc70 	bl	27a <mbus_remote_register_write>
 99a:	2000      	movs	r0, #0
 99c:	f7ff fbde 	bl	15c <set_halt_until_reg>
 9a0:	4b40      	ldr	r3, [pc, #256]	; (aa4 <cycle4+0x16c>)
 9a2:	211e      	movs	r1, #30
 9a4:	681a      	ldr	r2, [r3, #0]
 9a6:	1c28      	adds	r0, r5, #0
 9a8:	4316      	orrs	r6, r2
 9aa:	601e      	str	r6, [r3, #0]
 9ac:	681a      	ldr	r2, [r3, #0]
 9ae:	438a      	bics	r2, r1
 9b0:	2108      	movs	r1, #8
 9b2:	430a      	orrs	r2, r1
 9b4:	601a      	str	r2, [r3, #0]
 9b6:	681a      	ldr	r2, [r3, #0]
 9b8:	2120      	movs	r1, #32
 9ba:	430a      	orrs	r2, r1
 9bc:	601a      	str	r2, [r3, #0]
 9be:	6819      	ldr	r1, [r3, #0]
 9c0:	4a39      	ldr	r2, [pc, #228]	; (aa8 <cycle4+0x170>)
 9c2:	400a      	ands	r2, r1
 9c4:	601a      	str	r2, [r3, #0]
 9c6:	681a      	ldr	r2, [r3, #0]
 9c8:	2107      	movs	r1, #7
 9ca:	f7ff fc56 	bl	27a <mbus_remote_register_write>
 9ce:	23a0      	movs	r3, #160	; 0xa0
 9d0:	061b      	lsls	r3, r3, #24
 9d2:	681a      	ldr	r2, [r3, #0]
 9d4:	2006      	movs	r0, #6
 9d6:	6819      	ldr	r1, [r3, #0]
 9d8:	2a55      	cmp	r2, #85	; 0x55
 9da:	d102      	bne.n	9e2 <cycle4+0xaa>
 9dc:	f7ff fe2e 	bl	63c <pass>
 9e0:	e001      	b.n	9e6 <cycle4+0xae>
 9e2:	f7ff fe3b 	bl	65c <fail>
 9e6:	f7ff fbd3 	bl	190 <set_halt_until_mbus_tx>
 9ea:	8822      	ldrh	r2, [r4, #0]
 9ec:	23ff      	movs	r3, #255	; 0xff
 9ee:	2504      	movs	r5, #4
 9f0:	439a      	bics	r2, r3
 9f2:	432a      	orrs	r2, r5
 9f4:	8022      	strh	r2, [r4, #0]
 9f6:	8822      	ldrh	r2, [r4, #0]
 9f8:	210f      	movs	r1, #15
 9fa:	4013      	ands	r3, r2
 9fc:	2280      	movs	r2, #128	; 0x80
 9fe:	0152      	lsls	r2, r2, #5
 a00:	4313      	orrs	r3, r2
 a02:	8023      	strh	r3, [r4, #0]
 a04:	6822      	ldr	r2, [r4, #0]
 a06:	1c28      	adds	r0, r5, #0
 a08:	f7ff fc37 	bl	27a <mbus_remote_register_write>
 a0c:	1c28      	adds	r0, r5, #0
 a0e:	f7ff fba5 	bl	15c <set_halt_until_reg>
 a12:	4b24      	ldr	r3, [pc, #144]	; (aa4 <cycle4+0x16c>)
 a14:	2101      	movs	r1, #1
 a16:	681a      	ldr	r2, [r3, #0]
 a18:	1c28      	adds	r0, r5, #0
 a1a:	430a      	orrs	r2, r1
 a1c:	601a      	str	r2, [r3, #0]
 a1e:	681a      	ldr	r2, [r3, #0]
 a20:	2107      	movs	r1, #7
 a22:	f7ff fc2a 	bl	27a <mbus_remote_register_write>
 a26:	4b21      	ldr	r3, [pc, #132]	; (aac <cycle4+0x174>)
 a28:	2007      	movs	r0, #7
 a2a:	681a      	ldr	r2, [r3, #0]
 a2c:	6819      	ldr	r1, [r3, #0]
 a2e:	2a55      	cmp	r2, #85	; 0x55
 a30:	d102      	bne.n	a38 <cycle4+0x100>
 a32:	f7ff fe03 	bl	63c <pass>
 a36:	e001      	b.n	a3c <cycle4+0x104>
 a38:	f7ff fe10 	bl	65c <fail>
 a3c:	f7ff fba8 	bl	190 <set_halt_until_mbus_tx>
 a40:	4b16      	ldr	r3, [pc, #88]	; (a9c <cycle4+0x164>)
 a42:	22ff      	movs	r2, #255	; 0xff
 a44:	8819      	ldrh	r1, [r3, #0]
 a46:	2004      	movs	r0, #4
 a48:	4391      	bics	r1, r2
 a4a:	8019      	strh	r1, [r3, #0]
 a4c:	8819      	ldrh	r1, [r3, #0]
 a4e:	400a      	ands	r2, r1
 a50:	21f8      	movs	r1, #248	; 0xf8
 a52:	0149      	lsls	r1, r1, #5
 a54:	430a      	orrs	r2, r1
 a56:	801a      	strh	r2, [r3, #0]
 a58:	681a      	ldr	r2, [r3, #0]
 a5a:	210f      	movs	r1, #15
 a5c:	f7ff fc0d 	bl	27a <mbus_remote_register_write>
 a60:	2007      	movs	r0, #7
 a62:	f7ff fb7b 	bl	15c <set_halt_until_reg>
 a66:	4b0f      	ldr	r3, [pc, #60]	; (aa4 <cycle4+0x16c>)
 a68:	2101      	movs	r1, #1
 a6a:	681a      	ldr	r2, [r3, #0]
 a6c:	2004      	movs	r0, #4
 a6e:	430a      	orrs	r2, r1
 a70:	601a      	str	r2, [r3, #0]
 a72:	681a      	ldr	r2, [r3, #0]
 a74:	2107      	movs	r1, #7
 a76:	f7ff fc00 	bl	27a <mbus_remote_register_write>
 a7a:	4b0d      	ldr	r3, [pc, #52]	; (ab0 <cycle4+0x178>)
 a7c:	2008      	movs	r0, #8
 a7e:	681a      	ldr	r2, [r3, #0]
 a80:	6819      	ldr	r1, [r3, #0]
 a82:	2a55      	cmp	r2, #85	; 0x55
 a84:	d102      	bne.n	a8c <cycle4+0x154>
 a86:	f7ff fdd9 	bl	63c <pass>
 a8a:	e001      	b.n	a90 <cycle4+0x158>
 a8c:	f7ff fde6 	bl	65c <fail>
 a90:	f7ff fb7e 	bl	190 <set_halt_until_mbus_tx>
 a94:	bd70      	pop	{r4, r5, r6, pc}
 a96:	46c0      	nop			; (mov r8, r8)
 a98:	00000dac 	.word	0x00000dac
 a9c:	00000dbc 	.word	0x00000dbc
 aa0:	00000dd8 	.word	0x00000dd8
 aa4:	00000ddc 	.word	0x00000ddc
 aa8:	fffe003f 	.word	0xfffe003f
 aac:	a0000010 	.word	0xa0000010
 ab0:	a000001c 	.word	0xa000001c

Disassembly of section .text.cycle5:

00000ab4 <cycle5>:
 ab4:	4b1a      	ldr	r3, [pc, #104]	; (b20 <cycle5+0x6c>)
 ab6:	b507      	push	{r0, r1, r2, lr}
 ab8:	681b      	ldr	r3, [r3, #0]
 aba:	2b01      	cmp	r3, #1
 abc:	d12f      	bne.n	b1e <cycle5+0x6a>
 abe:	f7ff fb67 	bl	190 <set_halt_until_mbus_tx>
 ac2:	2100      	movs	r1, #0
 ac4:	1c0a      	adds	r2, r1, #0
 ac6:	2004      	movs	r0, #4
 ac8:	230f      	movs	r3, #15
 aca:	f7ff fbe3 	bl	294 <mbus_copy_mem_from_local_to_remote_bulk>
 ace:	f7ff fb6d 	bl	1ac <set_halt_until_mbus_fwd>
 ad2:	2100      	movs	r1, #0
 ad4:	230f      	movs	r3, #15
 ad6:	2004      	movs	r0, #4
 ad8:	2208      	movs	r2, #8
 ada:	9300      	str	r3, [sp, #0]
 adc:	1c0b      	adds	r3, r1, #0
 ade:	f7ff fbf3 	bl	2c8 <mbus_copy_mem_from_remote_to_any_bulk>
 ae2:	f7ff fb55 	bl	190 <set_halt_until_mbus_tx>
 ae6:	2100      	movs	r1, #0
 ae8:	237f      	movs	r3, #127	; 0x7f
 aea:	2004      	movs	r0, #4
 aec:	220c      	movs	r2, #12
 aee:	9300      	str	r3, [sp, #0]
 af0:	1c0b      	adds	r3, r1, #0
 af2:	f7ff fbe9 	bl	2c8 <mbus_copy_mem_from_remote_to_any_bulk>
 af6:	f7ff fb59 	bl	1ac <set_halt_until_mbus_fwd>
 afa:	f7ff fb65 	bl	1c8 <halt_cpu>
 afe:	f7ff fb39 	bl	174 <set_halt_until_mbus_rx>
 b02:	2004      	movs	r0, #4
 b04:	1c03      	adds	r3, r0, #0
 b06:	2123      	movs	r1, #35	; 0x23
 b08:	2200      	movs	r2, #0
 b0a:	f7ff fba5 	bl	258 <mbus_copy_registers_from_remote_to_local>
 b0e:	f7ff fb3f 	bl	190 <set_halt_until_mbus_tx>
 b12:	200d      	movs	r0, #13
 b14:	2115      	movs	r1, #21
 b16:	2200      	movs	r2, #0
 b18:	2304      	movs	r3, #4
 b1a:	f7ff fb8b 	bl	234 <mbus_copy_registers_from_local_to_remote>
 b1e:	bd07      	pop	{r0, r1, r2, pc}
 b20:	00000db0 	.word	0x00000db0

Disassembly of section .text.cycle6:

00000b24 <cycle6>:
 b24:	4b13      	ldr	r3, [pc, #76]	; (b74 <cycle6+0x50>)
 b26:	b537      	push	{r0, r1, r2, r4, r5, lr}
 b28:	681d      	ldr	r5, [r3, #0]
 b2a:	2d01      	cmp	r5, #1
 b2c:	d120      	bne.n	b70 <cycle6+0x4c>
 b2e:	4c12      	ldr	r4, [pc, #72]	; (b78 <cycle6+0x54>)
 b30:	f7ff fb2e 	bl	190 <set_halt_until_mbus_tx>
 b34:	2000      	movs	r0, #0
 b36:	1c02      	adds	r2, r0, #0
 b38:	2104      	movs	r1, #4
 b3a:	1c23      	adds	r3, r4, #0
 b3c:	f7ff fbda 	bl	2f4 <mbus_copy_mem_from_local_to_remote_stream>
 b40:	f7ff fb34 	bl	1ac <set_halt_until_mbus_fwd>
 b44:	2000      	movs	r0, #0
 b46:	1c02      	adds	r2, r0, #0
 b48:	2104      	movs	r1, #4
 b4a:	2308      	movs	r3, #8
 b4c:	9400      	str	r4, [sp, #0]
 b4e:	f7ff fbeb 	bl	328 <mbus_copy_mem_from_remote_to_any_stream>
 b52:	f7ff fb1d 	bl	190 <set_halt_until_mbus_tx>
 b56:	1c28      	adds	r0, r5, #0
 b58:	2104      	movs	r1, #4
 b5a:	2200      	movs	r2, #0
 b5c:	230c      	movs	r3, #12
 b5e:	9400      	str	r4, [sp, #0]
 b60:	f7ff fbe2 	bl	328 <mbus_copy_mem_from_remote_to_any_stream>
 b64:	f7ff fb22 	bl	1ac <set_halt_until_mbus_fwd>
 b68:	f7ff fb2e 	bl	1c8 <halt_cpu>
 b6c:	f7ff fb10 	bl	190 <set_halt_until_mbus_tx>
 b70:	bd37      	pop	{r0, r1, r2, r4, r5, pc}
 b72:	46c0      	nop			; (mov r8, r8)
 b74:	00000dc4 	.word	0x00000dc4
 b78:	000001ff 	.word	0x000001ff

Disassembly of section .text.cycle7:

00000b7c <cycle7>:
 b7c:	4b1c      	ldr	r3, [pc, #112]	; (bf0 <cycle7+0x74>)
 b7e:	b537      	push	{r0, r1, r2, r4, r5, lr}
 b80:	681b      	ldr	r3, [r3, #0]
 b82:	2b01      	cmp	r3, #1
 b84:	d133      	bne.n	bee <cycle7+0x72>
 b86:	f7ff fb03 	bl	190 <set_halt_until_mbus_tx>
 b8a:	f7ff fadf 	bl	14c <clear_all_pend_irq>
 b8e:	4b19      	ldr	r3, [pc, #100]	; (bf4 <cycle7+0x78>)
 b90:	2202      	movs	r2, #2
 b92:	2500      	movs	r5, #0
 b94:	2080      	movs	r0, #128	; 0x80
 b96:	21f0      	movs	r1, #240	; 0xf0
 b98:	601a      	str	r2, [r3, #0]
 b9a:	0140      	lsls	r0, r0, #5
 b9c:	0209      	lsls	r1, r1, #8
 b9e:	1c2b      	adds	r3, r5, #0
 ba0:	2203      	movs	r2, #3
 ba2:	9500      	str	r5, [sp, #0]
 ba4:	f7ff fa82 	bl	ac <config_timer16>
 ba8:	f7ff fa7d 	bl	a6 <WFI>
 bac:	4c12      	ldr	r4, [pc, #72]	; (bf8 <cycle7+0x7c>)
 bae:	2009      	movs	r0, #9
 bb0:	6823      	ldr	r3, [r4, #0]
 bb2:	6821      	ldr	r1, [r4, #0]
 bb4:	2b02      	cmp	r3, #2
 bb6:	d103      	bne.n	bc0 <cycle7+0x44>
 bb8:	f7ff fd40 	bl	63c <pass>
 bbc:	6025      	str	r5, [r4, #0]
 bbe:	e001      	b.n	bc4 <cycle7+0x48>
 bc0:	f7ff fd4c 	bl	65c <fail>
 bc4:	f7ff fa6f 	bl	a6 <WFI>
 bc8:	6823      	ldr	r3, [r4, #0]
 bca:	200a      	movs	r0, #10
 bcc:	6821      	ldr	r1, [r4, #0]
 bce:	2b02      	cmp	r3, #2
 bd0:	d104      	bne.n	bdc <cycle7+0x60>
 bd2:	f7ff fd33 	bl	63c <pass>
 bd6:	2300      	movs	r3, #0
 bd8:	6023      	str	r3, [r4, #0]
 bda:	e001      	b.n	be0 <cycle7+0x64>
 bdc:	f7ff fd3e 	bl	65c <fail>
 be0:	f7ff faa8 	bl	134 <disable_all_irq>
 be4:	4a05      	ldr	r2, [pc, #20]	; (bfc <cycle7+0x80>)
 be6:	2300      	movs	r3, #0
 be8:	6013      	str	r3, [r2, #0]
 bea:	4a05      	ldr	r2, [pc, #20]	; (c00 <cycle7+0x84>)
 bec:	6013      	str	r3, [r2, #0]
 bee:	bd37      	pop	{r0, r1, r2, r4, r5, pc}
 bf0:	00000d9c 	.word	0x00000d9c
 bf4:	e000e100 	.word	0xe000e100
 bf8:	00000dd0 	.word	0x00000dd0
 bfc:	a0001000 	.word	0xa0001000
 c00:	a0001020 	.word	0xa0001020

Disassembly of section .text.cycle8:

00000c04 <cycle8>:
 c04:	4b13      	ldr	r3, [pc, #76]	; (c54 <cycle8+0x50>)
 c06:	b510      	push	{r4, lr}
 c08:	681c      	ldr	r4, [r3, #0]
 c0a:	2c01      	cmp	r4, #1
 c0c:	d120      	bne.n	c50 <cycle8+0x4c>
 c0e:	f7ff fabf 	bl	190 <set_halt_until_mbus_tx>
 c12:	f7ff fa9b 	bl	14c <clear_all_pend_irq>
 c16:	4b10      	ldr	r3, [pc, #64]	; (c58 <cycle8+0x54>)
 c18:	2200      	movs	r2, #0
 c1a:	2080      	movs	r0, #128	; 0x80
 c1c:	1c21      	adds	r1, r4, #0
 c1e:	601c      	str	r4, [r3, #0]
 c20:	0180      	lsls	r0, r0, #6
 c22:	1c13      	adds	r3, r2, #0
 c24:	f7ff fa60 	bl	e8 <config_timer32>
 c28:	f7ff fa3d 	bl	a6 <WFI>
 c2c:	4c0b      	ldr	r4, [pc, #44]	; (c5c <cycle8+0x58>)
 c2e:	200b      	movs	r0, #11
 c30:	6823      	ldr	r3, [r4, #0]
 c32:	6821      	ldr	r1, [r4, #0]
 c34:	2b01      	cmp	r3, #1
 c36:	d104      	bne.n	c42 <cycle8+0x3e>
 c38:	f7ff fd00 	bl	63c <pass>
 c3c:	2300      	movs	r3, #0
 c3e:	6023      	str	r3, [r4, #0]
 c40:	e001      	b.n	c46 <cycle8+0x42>
 c42:	f7ff fd0b 	bl	65c <fail>
 c46:	4a06      	ldr	r2, [pc, #24]	; (c60 <cycle8+0x5c>)
 c48:	2300      	movs	r3, #0
 c4a:	6013      	str	r3, [r2, #0]
 c4c:	4a05      	ldr	r2, [pc, #20]	; (c64 <cycle8+0x60>)
 c4e:	6013      	str	r3, [r2, #0]
 c50:	bd10      	pop	{r4, pc}
 c52:	46c0      	nop			; (mov r8, r8)
 c54:	00000db4 	.word	0x00000db4
 c58:	e000e100 	.word	0xe000e100
 c5c:	00000dd0 	.word	0x00000dd0
 c60:	a0001100 	.word	0xa0001100
 c64:	a000110c 	.word	0xa000110c

Disassembly of section .text.cycle9:

00000c68 <cycle9>:
 c68:	b508      	push	{r3, lr}
 c6a:	4b08      	ldr	r3, [pc, #32]	; (c8c <cycle9+0x24>)
 c6c:	681b      	ldr	r3, [r3, #0]
 c6e:	2b01      	cmp	r3, #1
 c70:	d10a      	bne.n	c88 <cycle9+0x20>
 c72:	f7ff fa8d 	bl	190 <set_halt_until_mbus_tx>
 c76:	2064      	movs	r0, #100	; 0x64
 c78:	f7ff fa50 	bl	11c <config_timerwd>
 c7c:	20c8      	movs	r0, #200	; 0xc8
 c7e:	f7ff fa0a 	bl	96 <delay>
 c82:	4803      	ldr	r0, [pc, #12]	; (c90 <cycle9+0x28>)
 c84:	f7ff fa4a 	bl	11c <config_timerwd>
 c88:	bd08      	pop	{r3, pc}
 c8a:	46c0      	nop			; (mov r8, r8)
 c8c:	00000dc0 	.word	0x00000dc0
 c90:	001e8480 	.word	0x001e8480

Disassembly of section .text.startup.main:

00000c94 <main>:
 c94:	b570      	push	{r4, r5, r6, lr}
 c96:	4e36      	ldr	r6, [pc, #216]	; (d70 <main+0xdc>)
 c98:	f7ff fa4c 	bl	134 <disable_all_irq>
 c9c:	6832      	ldr	r2, [r6, #0]
 c9e:	4b35      	ldr	r3, [pc, #212]	; (d74 <main+0xe0>)
 ca0:	429a      	cmp	r2, r3
 ca2:	d000      	beq.n	ca6 <main+0x12>
 ca4:	e7fe      	b.n	ca4 <main+0x10>
 ca6:	4b34      	ldr	r3, [pc, #208]	; (d78 <main+0xe4>)
 ca8:	681a      	ldr	r2, [r3, #0]
 caa:	4b34      	ldr	r3, [pc, #208]	; (d7c <main+0xe8>)
 cac:	429a      	cmp	r2, r3
 cae:	d001      	beq.n	cb4 <main+0x20>
 cb0:	f7ff fca2 	bl	5f8 <initialization>
 cb4:	4c32      	ldr	r4, [pc, #200]	; (d80 <main+0xec>)
 cb6:	4d33      	ldr	r5, [pc, #204]	; (d84 <main+0xf0>)
 cb8:	6823      	ldr	r3, [r4, #0]
 cba:	2b00      	cmp	r3, #0
 cbc:	d101      	bne.n	cc2 <main+0x2e>
 cbe:	4b32      	ldr	r3, [pc, #200]	; (d88 <main+0xf4>)
 cc0:	e03a      	b.n	d38 <main+0xa4>
 cc2:	6823      	ldr	r3, [r4, #0]
 cc4:	2b01      	cmp	r3, #1
 cc6:	d102      	bne.n	cce <main+0x3a>
 cc8:	f7ff fcde 	bl	688 <cycle1>
 ccc:	e03c      	b.n	d48 <main+0xb4>
 cce:	6823      	ldr	r3, [r4, #0]
 cd0:	2b02      	cmp	r3, #2
 cd2:	d102      	bne.n	cda <main+0x46>
 cd4:	f7ff fd58 	bl	788 <cycle2>
 cd8:	e036      	b.n	d48 <main+0xb4>
 cda:	6823      	ldr	r3, [r4, #0]
 cdc:	2b03      	cmp	r3, #3
 cde:	d102      	bne.n	ce6 <main+0x52>
 ce0:	f7ff fdd4 	bl	88c <cycle3>
 ce4:	e030      	b.n	d48 <main+0xb4>
 ce6:	6823      	ldr	r3, [r4, #0]
 ce8:	2b04      	cmp	r3, #4
 cea:	d102      	bne.n	cf2 <main+0x5e>
 cec:	f7ff fe24 	bl	938 <cycle4>
 cf0:	e02a      	b.n	d48 <main+0xb4>
 cf2:	6823      	ldr	r3, [r4, #0]
 cf4:	2b05      	cmp	r3, #5
 cf6:	d102      	bne.n	cfe <main+0x6a>
 cf8:	f7ff fedc 	bl	ab4 <cycle5>
 cfc:	e024      	b.n	d48 <main+0xb4>
 cfe:	6823      	ldr	r3, [r4, #0]
 d00:	2b06      	cmp	r3, #6
 d02:	d102      	bne.n	d0a <main+0x76>
 d04:	f7ff ff0e 	bl	b24 <cycle6>
 d08:	e01e      	b.n	d48 <main+0xb4>
 d0a:	6823      	ldr	r3, [r4, #0]
 d0c:	2b07      	cmp	r3, #7
 d0e:	d102      	bne.n	d16 <main+0x82>
 d10:	f7ff ff34 	bl	b7c <cycle7>
 d14:	e018      	b.n	d48 <main+0xb4>
 d16:	6823      	ldr	r3, [r4, #0]
 d18:	2b08      	cmp	r3, #8
 d1a:	d102      	bne.n	d22 <main+0x8e>
 d1c:	f7ff ff72 	bl	c04 <cycle8>
 d20:	e012      	b.n	d48 <main+0xb4>
 d22:	6823      	ldr	r3, [r4, #0]
 d24:	2b09      	cmp	r3, #9
 d26:	d102      	bne.n	d2e <main+0x9a>
 d28:	f7ff ff9e 	bl	c68 <cycle9>
 d2c:	e00c      	b.n	d48 <main+0xb4>
 d2e:	4b14      	ldr	r3, [pc, #80]	; (d80 <main+0xec>)
 d30:	681a      	ldr	r2, [r3, #0]
 d32:	2a0a      	cmp	r2, #10
 d34:	d102      	bne.n	d3c <main+0xa8>
 d36:	4b15      	ldr	r3, [pc, #84]	; (d8c <main+0xf8>)
 d38:	681b      	ldr	r3, [r3, #0]
 d3a:	e005      	b.n	d48 <main+0xb4>
 d3c:	681a      	ldr	r2, [r3, #0]
 d3e:	2a0b      	cmp	r2, #11
 d40:	d101      	bne.n	d46 <main+0xb2>
 d42:	4b13      	ldr	r3, [pc, #76]	; (d90 <main+0xfc>)
 d44:	e7f8      	b.n	d38 <main+0xa4>
 d46:	601d      	str	r5, [r3, #0]
 d48:	6823      	ldr	r3, [r4, #0]
 d4a:	42ab      	cmp	r3, r5
 d4c:	d102      	bne.n	d54 <main+0xc0>
 d4e:	4b11      	ldr	r3, [pc, #68]	; (d94 <main+0x100>)
 d50:	6033      	str	r3, [r6, #0]
 d52:	e007      	b.n	d64 <main+0xd0>
 d54:	6823      	ldr	r3, [r4, #0]
 d56:	2201      	movs	r2, #1
 d58:	3301      	adds	r3, #1
 d5a:	6023      	str	r3, [r4, #0]
 d5c:	4b0e      	ldr	r3, [pc, #56]	; (d98 <main+0x104>)
 d5e:	601a      	str	r2, [r3, #0]
 d60:	f7ff fa62 	bl	228 <mbus_sleep_all>
 d64:	4805      	ldr	r0, [pc, #20]	; (d7c <main+0xe8>)
 d66:	f7ff fa37 	bl	1d8 <arb_debug_reg>
 d6a:	46c0      	nop			; (mov r8, r8)
 d6c:	e7fa      	b.n	d64 <main+0xd0>
 d6e:	46c0      	nop			; (mov r8, r8)
 d70:	a0000020 	.word	0xa0000020
 d74:	00001234 	.word	0x00001234
 d78:	00000dcc 	.word	0x00000dcc
 d7c:	deadbeef 	.word	0xdeadbeef
 d80:	00000dc8 	.word	0x00000dc8
 d84:	000003e7 	.word	0x000003e7
 d88:	00000da0 	.word	0x00000da0
 d8c:	00000de4 	.word	0x00000de4
 d90:	00000de0 	.word	0x00000de0
 d94:	0000ffff 	.word	0x0000ffff
 d98:	a0001300 	.word	0xa0001300
