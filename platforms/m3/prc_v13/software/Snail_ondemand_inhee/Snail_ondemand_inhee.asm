
Snail_ondemand_inhee/Snail_ondemand_inhee.elf:     file format elf32-littlearm


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
  40:	00000995 	.word	0x00000995
  44:	000009a1 	.word	0x000009a1
  48:	000009ad 	.word	0x000009ad
  4c:	000009c5 	.word	0x000009c5
  50:	000009dd 	.word	0x000009dd
  54:	000009f5 	.word	0x000009f5
  58:	00000a0d 	.word	0x00000a0d
  5c:	00000a25 	.word	0x00000a25
  60:	00000a3d 	.word	0x00000a3d
  64:	00000a55 	.word	0x00000a55
  68:	00000a6d 	.word	0x00000a6d
  6c:	00000a7d 	.word	0x00000a7d
  70:	00000a8d 	.word	0x00000a8d
  74:	00000a9d 	.word	0x00000a9d
  78:	00000aad 	.word	0x00000aad
  7c:	00000000 	.word	0x00000000

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 fd14 	bl	abc <main>
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

Disassembly of section .text.config_timerwd:

000000a8 <config_timerwd>:
  a8:	4b03      	ldr	r3, [pc, #12]	; (b8 <config_timerwd+0x10>)
  aa:	2200      	movs	r2, #0
  ac:	601a      	str	r2, [r3, #0]
  ae:	4a03      	ldr	r2, [pc, #12]	; (bc <config_timerwd+0x14>)
  b0:	6010      	str	r0, [r2, #0]
  b2:	2201      	movs	r2, #1
  b4:	601a      	str	r2, [r3, #0]
  b6:	4770      	bx	lr
  b8:	a0001200 	.word	0xa0001200
  bc:	a0001204 	.word	0xa0001204

Disassembly of section .text.set_wakeup_timer:

000000c0 <set_wakeup_timer>:
  c0:	b500      	push	{lr}
  c2:	2900      	cmp	r1, #0
  c4:	d003      	beq.n	ce <set_wakeup_timer+0xe>
  c6:	23c0      	movs	r3, #192	; 0xc0
  c8:	029b      	lsls	r3, r3, #10
  ca:	4318      	orrs	r0, r3
  cc:	e001      	b.n	d2 <set_wakeup_timer+0x12>
  ce:	0440      	lsls	r0, r0, #17
  d0:	0c40      	lsrs	r0, r0, #17
  d2:	4b04      	ldr	r3, [pc, #16]	; (e4 <set_wakeup_timer+0x24>)
  d4:	6018      	str	r0, [r3, #0]
  d6:	2a00      	cmp	r2, #0
  d8:	d002      	beq.n	e0 <set_wakeup_timer+0x20>
  da:	4b03      	ldr	r3, [pc, #12]	; (e8 <set_wakeup_timer+0x28>)
  dc:	2201      	movs	r2, #1
  de:	601a      	str	r2, [r3, #0]
  e0:	bd00      	pop	{pc}
  e2:	46c0      	nop			; (mov r8, r8)
  e4:	a0000044 	.word	0xa0000044
  e8:	a0001300 	.word	0xa0001300

Disassembly of section .text.enable_reg_irq:

000000ec <enable_reg_irq>:
  ec:	4b04      	ldr	r3, [pc, #16]	; (100 <enable_reg_irq+0x14>)
  ee:	2201      	movs	r2, #1
  f0:	4252      	negs	r2, r2
  f2:	601a      	str	r2, [r3, #0]
  f4:	4b03      	ldr	r3, [pc, #12]	; (104 <enable_reg_irq+0x18>)
  f6:	22ff      	movs	r2, #255	; 0xff
  f8:	0092      	lsls	r2, r2, #2
  fa:	601a      	str	r2, [r3, #0]
  fc:	4770      	bx	lr
  fe:	46c0      	nop			; (mov r8, r8)
 100:	e000e280 	.word	0xe000e280
 104:	e000e100 	.word	0xe000e100

Disassembly of section .text.set_halt_until_mbus_rx:

00000108 <set_halt_until_mbus_rx>:
 108:	4b04      	ldr	r3, [pc, #16]	; (11c <set_halt_until_mbus_rx+0x14>)
 10a:	4a05      	ldr	r2, [pc, #20]	; (120 <set_halt_until_mbus_rx+0x18>)
 10c:	6819      	ldr	r1, [r3, #0]
 10e:	400a      	ands	r2, r1
 110:	2190      	movs	r1, #144	; 0x90
 112:	0209      	lsls	r1, r1, #8
 114:	430a      	orrs	r2, r1
 116:	601a      	str	r2, [r3, #0]
 118:	4770      	bx	lr
 11a:	46c0      	nop			; (mov r8, r8)
 11c:	a0000028 	.word	0xa0000028
 120:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.set_halt_until_mbus_tx:

00000124 <set_halt_until_mbus_tx>:
 124:	4b04      	ldr	r3, [pc, #16]	; (138 <set_halt_until_mbus_tx+0x14>)
 126:	4a05      	ldr	r2, [pc, #20]	; (13c <set_halt_until_mbus_tx+0x18>)
 128:	6819      	ldr	r1, [r3, #0]
 12a:	400a      	ands	r2, r1
 12c:	21a0      	movs	r1, #160	; 0xa0
 12e:	0209      	lsls	r1, r1, #8
 130:	430a      	orrs	r2, r1
 132:	601a      	str	r2, [r3, #0]
 134:	4770      	bx	lr
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

Disassembly of section .text.mbus_write_message:

00000150 <mbus_write_message>:
 150:	2300      	movs	r3, #0
 152:	b500      	push	{lr}
 154:	429a      	cmp	r2, r3
 156:	d00a      	beq.n	16e <mbus_write_message+0x1e>
 158:	4b06      	ldr	r3, [pc, #24]	; (174 <mbus_write_message+0x24>)
 15a:	3a01      	subs	r2, #1
 15c:	0600      	lsls	r0, r0, #24
 15e:	4302      	orrs	r2, r0
 160:	601a      	str	r2, [r3, #0]
 162:	4b05      	ldr	r3, [pc, #20]	; (178 <mbus_write_message+0x28>)
 164:	2223      	movs	r2, #35	; 0x23
 166:	6019      	str	r1, [r3, #0]
 168:	4b04      	ldr	r3, [pc, #16]	; (17c <mbus_write_message+0x2c>)
 16a:	601a      	str	r2, [r3, #0]
 16c:	2301      	movs	r3, #1
 16e:	1c18      	adds	r0, r3, #0
 170:	bd00      	pop	{pc}
 172:	46c0      	nop			; (mov r8, r8)
 174:	a0002000 	.word	0xa0002000
 178:	a0002004 	.word	0xa0002004
 17c:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_enumerate:

00000180 <mbus_enumerate>:
 180:	0603      	lsls	r3, r0, #24
 182:	2080      	movs	r0, #128	; 0x80
 184:	0580      	lsls	r0, r0, #22
 186:	4318      	orrs	r0, r3
 188:	4b01      	ldr	r3, [pc, #4]	; (190 <mbus_enumerate+0x10>)
 18a:	6018      	str	r0, [r3, #0]
 18c:	4770      	bx	lr
 18e:	46c0      	nop			; (mov r8, r8)
 190:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

00000194 <mbus_sleep_all>:
 194:	4b01      	ldr	r3, [pc, #4]	; (19c <mbus_sleep_all+0x8>)
 196:	2200      	movs	r2, #0
 198:	601a      	str	r2, [r3, #0]
 19a:	4770      	bx	lr
 19c:	a0003010 	.word	0xa0003010

Disassembly of section .text.mbus_copy_registers_from_remote_to_local:

000001a0 <mbus_copy_registers_from_remote_to_local>:
 1a0:	b507      	push	{r0, r1, r2, lr}
 1a2:	0609      	lsls	r1, r1, #24
 1a4:	430a      	orrs	r2, r1
 1a6:	2180      	movs	r1, #128	; 0x80
 1a8:	0149      	lsls	r1, r1, #5
 1aa:	430a      	orrs	r2, r1
 1ac:	041b      	lsls	r3, r3, #16
 1ae:	431a      	orrs	r2, r3
 1b0:	9201      	str	r2, [sp, #4]
 1b2:	0100      	lsls	r0, r0, #4
 1b4:	2201      	movs	r2, #1
 1b6:	4310      	orrs	r0, r2
 1b8:	b2c0      	uxtb	r0, r0
 1ba:	a901      	add	r1, sp, #4
 1bc:	f7ff ffc8 	bl	150 <mbus_write_message>
 1c0:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_remote_register_write:

000001c2 <mbus_remote_register_write>:
 1c2:	b507      	push	{r0, r1, r2, lr}
 1c4:	0212      	lsls	r2, r2, #8
 1c6:	0a12      	lsrs	r2, r2, #8
 1c8:	0609      	lsls	r1, r1, #24
 1ca:	4311      	orrs	r1, r2
 1cc:	0100      	lsls	r0, r0, #4
 1ce:	9101      	str	r1, [sp, #4]
 1d0:	b2c0      	uxtb	r0, r0
 1d2:	a901      	add	r1, sp, #4
 1d4:	2201      	movs	r2, #1
 1d6:	f7ff ffbb 	bl	150 <mbus_write_message>
 1da:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_remote_register_read:

000001dc <mbus_remote_register_read>:
 1dc:	b508      	push	{r3, lr}
 1de:	2300      	movs	r3, #0
 1e0:	f7ff ffde 	bl	1a0 <mbus_copy_registers_from_remote_to_local>
 1e4:	bd08      	pop	{r3, pc}

Disassembly of section .text.batadc_reset:

000001e8 <batadc_reset>:
 1e8:	b508      	push	{r3, lr}
 1ea:	2007      	movs	r0, #7
 1ec:	213b      	movs	r1, #59	; 0x3b
 1ee:	4a03      	ldr	r2, [pc, #12]	; (1fc <batadc_reset+0x14>)
 1f0:	f7ff ffe7 	bl	1c2 <mbus_remote_register_write>
 1f4:	2064      	movs	r0, #100	; 0x64
 1f6:	f7ff ff4e 	bl	96 <delay>
 1fa:	bd08      	pop	{r3, pc}
 1fc:	000fe7ff 	.word	0x000fe7ff

Disassembly of section .text.batadc_resetrelease:

00000200 <batadc_resetrelease>:
 200:	b508      	push	{r3, lr}
 202:	2007      	movs	r0, #7
 204:	213b      	movs	r1, #59	; 0x3b
 206:	4a03      	ldr	r2, [pc, #12]	; (214 <batadc_resetrelease+0x14>)
 208:	f7ff ffdb 	bl	1c2 <mbus_remote_register_write>
 20c:	2064      	movs	r0, #100	; 0x64
 20e:	f7ff ff42 	bl	96 <delay>
 212:	bd08      	pop	{r3, pc}
 214:	000fefff 	.word	0x000fefff

Disassembly of section .text.unlikely.radio_power_on:

00000218 <radio_power_on>:
 218:	b538      	push	{r3, r4, r5, lr}
 21a:	4c28      	ldr	r4, [pc, #160]	; (2bc <radio_power_on+0xa4>)
 21c:	2119      	movs	r1, #25
 21e:	1c22      	adds	r2, r4, #0
 220:	2007      	movs	r0, #7
 222:	f7ff ffce 	bl	1c2 <mbus_remote_register_write>
 226:	2064      	movs	r0, #100	; 0x64
 228:	f7ff ff35 	bl	96 <delay>
 22c:	1c22      	adds	r2, r4, #0
 22e:	2119      	movs	r1, #25
 230:	2007      	movs	r0, #7
 232:	f7ff ffc6 	bl	1c2 <mbus_remote_register_write>
 236:	2064      	movs	r0, #100	; 0x64
 238:	f7ff ff2d 	bl	96 <delay>
 23c:	2115      	movs	r1, #21
 23e:	4a20      	ldr	r2, [pc, #128]	; (2c0 <radio_power_on+0xa8>)
 240:	2007      	movs	r0, #7
 242:	f7ff ffbe 	bl	1c2 <mbus_remote_register_write>
 246:	20fa      	movs	r0, #250	; 0xfa
 248:	0080      	lsls	r0, r0, #2
 24a:	f7ff ff24 	bl	96 <delay>
 24e:	4a1d      	ldr	r2, [pc, #116]	; (2c4 <radio_power_on+0xac>)
 250:	4c1d      	ldr	r4, [pc, #116]	; (2c8 <radio_power_on+0xb0>)
 252:	2301      	movs	r3, #1
 254:	7013      	strb	r3, [r2, #0]
 256:	8822      	ldrh	r2, [r4, #0]
 258:	210d      	movs	r1, #13
 25a:	439a      	bics	r2, r3
 25c:	8022      	strh	r2, [r4, #0]
 25e:	6822      	ldr	r2, [r4, #0]
 260:	2004      	movs	r0, #4
 262:	f7ff ffae 	bl	1c2 <mbus_remote_register_write>
 266:	2064      	movs	r0, #100	; 0x64
 268:	f7ff ff15 	bl	96 <delay>
 26c:	4d17      	ldr	r5, [pc, #92]	; (2cc <radio_power_on+0xb4>)
 26e:	4b18      	ldr	r3, [pc, #96]	; (2d0 <radio_power_on+0xb8>)
 270:	682a      	ldr	r2, [r5, #0]
 272:	2102      	movs	r1, #2
 274:	4013      	ands	r3, r2
 276:	602b      	str	r3, [r5, #0]
 278:	682a      	ldr	r2, [r5, #0]
 27a:	2004      	movs	r0, #4
 27c:	f7ff ffa1 	bl	1c2 <mbus_remote_register_write>
 280:	2096      	movs	r0, #150	; 0x96
 282:	0040      	lsls	r0, r0, #1
 284:	f7ff ff07 	bl	96 <delay>
 288:	682a      	ldr	r2, [r5, #0]
 28a:	2380      	movs	r3, #128	; 0x80
 28c:	031b      	lsls	r3, r3, #12
 28e:	4313      	orrs	r3, r2
 290:	602b      	str	r3, [r5, #0]
 292:	682a      	ldr	r2, [r5, #0]
 294:	2102      	movs	r1, #2
 296:	2004      	movs	r0, #4
 298:	f7ff ff93 	bl	1c2 <mbus_remote_register_write>
 29c:	2064      	movs	r0, #100	; 0x64
 29e:	f7ff fefa 	bl	96 <delay>
 2a2:	8823      	ldrh	r3, [r4, #0]
 2a4:	2202      	movs	r2, #2
 2a6:	4393      	bics	r3, r2
 2a8:	8023      	strh	r3, [r4, #0]
 2aa:	6822      	ldr	r2, [r4, #0]
 2ac:	210d      	movs	r1, #13
 2ae:	2004      	movs	r0, #4
 2b0:	f7ff ff87 	bl	1c2 <mbus_remote_register_write>
 2b4:	2064      	movs	r0, #100	; 0x64
 2b6:	f7ff feee 	bl	96 <delay>
 2ba:	bd38      	pop	{r3, r4, r5, pc}
 2bc:	00002221 	.word	0x00002221
 2c0:	0000e226 	.word	0x0000e226
 2c4:	00001354 	.word	0x00001354
 2c8:	000012dc 	.word	0x000012dc
 2cc:	000012e4 	.word	0x000012e4
 2d0:	fffbffff 	.word	0xfffbffff

Disassembly of section .text.radio_power_off:

000002d4 <radio_power_off>:
 2d4:	b510      	push	{r4, lr}
 2d6:	4c21      	ldr	r4, [pc, #132]	; (35c <radio_power_off+0x88>)
 2d8:	2119      	movs	r1, #25
 2da:	1c22      	adds	r2, r4, #0
 2dc:	2007      	movs	r0, #7
 2de:	f7ff ff70 	bl	1c2 <mbus_remote_register_write>
 2e2:	2064      	movs	r0, #100	; 0x64
 2e4:	f7ff fed7 	bl	96 <delay>
 2e8:	1c22      	adds	r2, r4, #0
 2ea:	2119      	movs	r1, #25
 2ec:	2007      	movs	r0, #7
 2ee:	f7ff ff68 	bl	1c2 <mbus_remote_register_write>
 2f2:	2064      	movs	r0, #100	; 0x64
 2f4:	f7ff fecf 	bl	96 <delay>
 2f8:	2115      	movs	r1, #21
 2fa:	4a19      	ldr	r2, [pc, #100]	; (360 <radio_power_off+0x8c>)
 2fc:	2007      	movs	r0, #7
 2fe:	f7ff ff60 	bl	1c2 <mbus_remote_register_write>
 302:	20fa      	movs	r0, #250	; 0xfa
 304:	0080      	lsls	r0, r0, #2
 306:	f7ff fec6 	bl	96 <delay>
 30a:	4b16      	ldr	r3, [pc, #88]	; (364 <radio_power_off+0x90>)
 30c:	2200      	movs	r2, #0
 30e:	701a      	strb	r2, [r3, #0]
 310:	4b15      	ldr	r3, [pc, #84]	; (368 <radio_power_off+0x94>)
 312:	4a16      	ldr	r2, [pc, #88]	; (36c <radio_power_off+0x98>)
 314:	6819      	ldr	r1, [r3, #0]
 316:	2004      	movs	r0, #4
 318:	400a      	ands	r2, r1
 31a:	601a      	str	r2, [r3, #0]
 31c:	6819      	ldr	r1, [r3, #0]
 31e:	2280      	movs	r2, #128	; 0x80
 320:	02d2      	lsls	r2, r2, #11
 322:	430a      	orrs	r2, r1
 324:	601a      	str	r2, [r3, #0]
 326:	681a      	ldr	r2, [r3, #0]
 328:	2102      	movs	r1, #2
 32a:	f7ff ff4a 	bl	1c2 <mbus_remote_register_write>
 32e:	4b10      	ldr	r3, [pc, #64]	; (370 <radio_power_off+0x9c>)
 330:	2101      	movs	r1, #1
 332:	881a      	ldrh	r2, [r3, #0]
 334:	2004      	movs	r0, #4
 336:	430a      	orrs	r2, r1
 338:	801a      	strh	r2, [r3, #0]
 33a:	881a      	ldrh	r2, [r3, #0]
 33c:	2102      	movs	r1, #2
 33e:	430a      	orrs	r2, r1
 340:	801a      	strh	r2, [r3, #0]
 342:	881a      	ldrh	r2, [r3, #0]
 344:	2108      	movs	r1, #8
 346:	4382      	bics	r2, r0
 348:	801a      	strh	r2, [r3, #0]
 34a:	881a      	ldrh	r2, [r3, #0]
 34c:	438a      	bics	r2, r1
 34e:	801a      	strh	r2, [r3, #0]
 350:	681a      	ldr	r2, [r3, #0]
 352:	210d      	movs	r1, #13
 354:	f7ff ff35 	bl	1c2 <mbus_remote_register_write>
 358:	bd10      	pop	{r4, pc}
 35a:	46c0      	nop			; (mov r8, r8)
 35c:	00002203 	.word	0x00002203
 360:	0000e203 	.word	0x0000e203
 364:	00001354 	.word	0x00001354
 368:	000012e4 	.word	0x000012e4
 36c:	fff7ffff 	.word	0xfff7ffff
 370:	000012dc 	.word	0x000012dc

Disassembly of section .text.ldo_power_off:

00000374 <ldo_power_off>:
 374:	b508      	push	{r3, lr}
 376:	4b0c      	ldr	r3, [pc, #48]	; (3a8 <ldo_power_off+0x34>)
 378:	2280      	movs	r2, #128	; 0x80
 37a:	6819      	ldr	r1, [r3, #0]
 37c:	0152      	lsls	r2, r2, #5
 37e:	430a      	orrs	r2, r1
 380:	601a      	str	r2, [r3, #0]
 382:	6819      	ldr	r1, [r3, #0]
 384:	2280      	movs	r2, #128	; 0x80
 386:	0112      	lsls	r2, r2, #4
 388:	430a      	orrs	r2, r1
 38a:	601a      	str	r2, [r3, #0]
 38c:	681a      	ldr	r2, [r3, #0]
 38e:	2102      	movs	r1, #2
 390:	430a      	orrs	r2, r1
 392:	601a      	str	r2, [r3, #0]
 394:	681a      	ldr	r2, [r3, #0]
 396:	2101      	movs	r1, #1
 398:	430a      	orrs	r2, r1
 39a:	601a      	str	r2, [r3, #0]
 39c:	681a      	ldr	r2, [r3, #0]
 39e:	2005      	movs	r0, #5
 3a0:	2112      	movs	r1, #18
 3a2:	f7ff ff0e 	bl	1c2 <mbus_remote_register_write>
 3a6:	bd08      	pop	{r3, pc}
 3a8:	000012fc 	.word	0x000012fc

Disassembly of section .text.temp_sensor_disable:

000003ac <temp_sensor_disable>:
 3ac:	b508      	push	{r3, lr}
 3ae:	4b06      	ldr	r3, [pc, #24]	; (3c8 <temp_sensor_disable+0x1c>)
 3b0:	2280      	movs	r2, #128	; 0x80
 3b2:	6819      	ldr	r1, [r3, #0]
 3b4:	0252      	lsls	r2, r2, #9
 3b6:	430a      	orrs	r2, r1
 3b8:	601a      	str	r2, [r3, #0]
 3ba:	681a      	ldr	r2, [r3, #0]
 3bc:	2005      	movs	r0, #5
 3be:	210e      	movs	r1, #14
 3c0:	f7ff feff 	bl	1c2 <mbus_remote_register_write>
 3c4:	bd08      	pop	{r3, pc}
 3c6:	46c0      	nop			; (mov r8, r8)
 3c8:	000012f4 	.word	0x000012f4

Disassembly of section .text.temp_sensor_assert_reset:

000003cc <temp_sensor_assert_reset>:
 3cc:	b508      	push	{r3, lr}
 3ce:	4b08      	ldr	r3, [pc, #32]	; (3f0 <temp_sensor_assert_reset+0x24>)
 3d0:	4a08      	ldr	r2, [pc, #32]	; (3f4 <temp_sensor_assert_reset+0x28>)
 3d2:	6819      	ldr	r1, [r3, #0]
 3d4:	2005      	movs	r0, #5
 3d6:	400a      	ands	r2, r1
 3d8:	601a      	str	r2, [r3, #0]
 3da:	6819      	ldr	r1, [r3, #0]
 3dc:	2280      	movs	r2, #128	; 0x80
 3de:	0392      	lsls	r2, r2, #14
 3e0:	430a      	orrs	r2, r1
 3e2:	601a      	str	r2, [r3, #0]
 3e4:	681a      	ldr	r2, [r3, #0]
 3e6:	210e      	movs	r1, #14
 3e8:	f7ff feeb 	bl	1c2 <mbus_remote_register_write>
 3ec:	bd08      	pop	{r3, pc}
 3ee:	46c0      	nop			; (mov r8, r8)
 3f0:	000012f4 	.word	0x000012f4
 3f4:	ffbfffff 	.word	0xffbfffff

Disassembly of section .text.temp_power_off:

000003f8 <temp_power_off>:
 3f8:	b508      	push	{r3, lr}
 3fa:	4b0b      	ldr	r3, [pc, #44]	; (428 <temp_power_off+0x30>)
 3fc:	2280      	movs	r2, #128	; 0x80
 3fe:	6819      	ldr	r1, [r3, #0]
 400:	0252      	lsls	r2, r2, #9
 402:	430a      	orrs	r2, r1
 404:	601a      	str	r2, [r3, #0]
 406:	6819      	ldr	r1, [r3, #0]
 408:	4a08      	ldr	r2, [pc, #32]	; (42c <temp_power_off+0x34>)
 40a:	2005      	movs	r0, #5
 40c:	400a      	ands	r2, r1
 40e:	601a      	str	r2, [r3, #0]
 410:	6819      	ldr	r1, [r3, #0]
 412:	2280      	movs	r2, #128	; 0x80
 414:	0392      	lsls	r2, r2, #14
 416:	430a      	orrs	r2, r1
 418:	601a      	str	r2, [r3, #0]
 41a:	681a      	ldr	r2, [r3, #0]
 41c:	210e      	movs	r1, #14
 41e:	f7ff fed0 	bl	1c2 <mbus_remote_register_write>
 422:	f7ff ffa7 	bl	374 <ldo_power_off>
 426:	bd08      	pop	{r3, pc}
 428:	000012f4 	.word	0x000012f4
 42c:	ffbfffff 	.word	0xffbfffff

Disassembly of section .text.operation_sleep_notimer:

00000430 <operation_sleep_notimer>:
 430:	b508      	push	{r3, lr}
 432:	f7ff ff9f 	bl	374 <ldo_power_off>
 436:	4b08      	ldr	r3, [pc, #32]	; (458 <operation_sleep_notimer+0x28>)
 438:	781b      	ldrb	r3, [r3, #0]
 43a:	2b00      	cmp	r3, #0
 43c:	d001      	beq.n	442 <operation_sleep_notimer+0x12>
 43e:	f7ff ff49 	bl	2d4 <radio_power_off>
 442:	2000      	movs	r0, #0
 444:	1c02      	adds	r2, r0, #0
 446:	1c01      	adds	r1, r0, #0
 448:	f7ff fe3a 	bl	c0 <set_wakeup_timer>
 44c:	2200      	movs	r2, #0
 44e:	2378      	movs	r3, #120	; 0x78
 450:	601a      	str	r2, [r3, #0]
 452:	f7ff fe9f 	bl	194 <mbus_sleep_all>
 456:	e7fe      	b.n	456 <operation_sleep_notimer+0x26>
 458:	00001354 	.word	0x00001354

Disassembly of section .text.send_radio_data_ppm:

0000045c <send_radio_data_ppm>:
 45c:	b570      	push	{r4, r5, r6, lr}
 45e:	4b2b      	ldr	r3, [pc, #172]	; (50c <send_radio_data_ppm+0xb0>)
 460:	020a      	lsls	r2, r1, #8
 462:	6819      	ldr	r1, [r3, #0]
 464:	0a12      	lsrs	r2, r2, #8
 466:	0e09      	lsrs	r1, r1, #24
 468:	0609      	lsls	r1, r1, #24
 46a:	4311      	orrs	r1, r2
 46c:	6019      	str	r1, [r3, #0]
 46e:	681a      	ldr	r2, [r3, #0]
 470:	1c05      	adds	r5, r0, #0
 472:	2103      	movs	r1, #3
 474:	2004      	movs	r0, #4
 476:	f7ff fea4 	bl	1c2 <mbus_remote_register_write>
 47a:	4b25      	ldr	r3, [pc, #148]	; (510 <send_radio_data_ppm+0xb4>)
 47c:	781a      	ldrb	r2, [r3, #0]
 47e:	2a00      	cmp	r2, #0
 480:	d10d      	bne.n	49e <send_radio_data_ppm+0x42>
 482:	2201      	movs	r2, #1
 484:	701a      	strb	r2, [r3, #0]
 486:	4b23      	ldr	r3, [pc, #140]	; (514 <send_radio_data_ppm+0xb8>)
 488:	2004      	movs	r0, #4
 48a:	881a      	ldrh	r2, [r3, #0]
 48c:	210d      	movs	r1, #13
 48e:	4302      	orrs	r2, r0
 490:	801a      	strh	r2, [r3, #0]
 492:	681a      	ldr	r2, [r3, #0]
 494:	f7ff fe95 	bl	1c2 <mbus_remote_register_write>
 498:	2064      	movs	r0, #100	; 0x64
 49a:	f7ff fdfc 	bl	96 <delay>
 49e:	f7ff fe33 	bl	108 <set_halt_until_mbus_rx>
 4a2:	4e1d      	ldr	r6, [pc, #116]	; (518 <send_radio_data_ppm+0xbc>)
 4a4:	2300      	movs	r3, #0
 4a6:	7033      	strb	r3, [r6, #0]
 4a8:	4b1a      	ldr	r3, [pc, #104]	; (514 <send_radio_data_ppm+0xb8>)
 4aa:	2108      	movs	r1, #8
 4ac:	881a      	ldrh	r2, [r3, #0]
 4ae:	2004      	movs	r0, #4
 4b0:	430a      	orrs	r2, r1
 4b2:	801a      	strh	r2, [r3, #0]
 4b4:	681a      	ldr	r2, [r3, #0]
 4b6:	210d      	movs	r1, #13
 4b8:	f7ff fe83 	bl	1c2 <mbus_remote_register_write>
 4bc:	2464      	movs	r4, #100	; 0x64
 4be:	7833      	ldrb	r3, [r6, #0]
 4c0:	2b00      	cmp	r3, #0
 4c2:	d015      	beq.n	4f0 <send_radio_data_ppm+0x94>
 4c4:	f7ff fe2e 	bl	124 <set_halt_until_mbus_tx>
 4c8:	2300      	movs	r3, #0
 4ca:	7033      	strb	r3, [r6, #0]
 4cc:	429d      	cmp	r5, r3
 4ce:	d004      	beq.n	4da <send_radio_data_ppm+0x7e>
 4d0:	4a0f      	ldr	r2, [pc, #60]	; (510 <send_radio_data_ppm+0xb4>)
 4d2:	7013      	strb	r3, [r2, #0]
 4d4:	f7ff fefe 	bl	2d4 <radio_power_off>
 4d8:	e016      	b.n	508 <send_radio_data_ppm+0xac>
 4da:	4b0e      	ldr	r3, [pc, #56]	; (514 <send_radio_data_ppm+0xb8>)
 4dc:	2108      	movs	r1, #8
 4de:	881a      	ldrh	r2, [r3, #0]
 4e0:	2004      	movs	r0, #4
 4e2:	438a      	bics	r2, r1
 4e4:	801a      	strh	r2, [r3, #0]
 4e6:	681a      	ldr	r2, [r3, #0]
 4e8:	210d      	movs	r1, #13
 4ea:	f7ff fe6a 	bl	1c2 <mbus_remote_register_write>
 4ee:	e00b      	b.n	508 <send_radio_data_ppm+0xac>
 4f0:	2064      	movs	r0, #100	; 0x64
 4f2:	3c01      	subs	r4, #1
 4f4:	f7ff fdcf 	bl	96 <delay>
 4f8:	2c00      	cmp	r4, #0
 4fa:	d1e0      	bne.n	4be <send_radio_data_ppm+0x62>
 4fc:	f7ff fe12 	bl	124 <set_halt_until_mbus_tx>
 500:	20bb      	movs	r0, #187	; 0xbb
 502:	4906      	ldr	r1, [pc, #24]	; (51c <send_radio_data_ppm+0xc0>)
 504:	f7ff fe1c 	bl	140 <mbus_write_message32>
 508:	bd70      	pop	{r4, r5, r6, pc}
 50a:	46c0      	nop			; (mov r8, r8)
 50c:	00001fa4 	.word	0x00001fa4
 510:	00001304 	.word	0x00001304
 514:	000012dc 	.word	0x000012dc
 518:	0000132c 	.word	0x0000132c
 51c:	fafafafa 	.word	0xfafafafa

Disassembly of section .text.operation_run:

00000520 <operation_run>:
 520:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
 522:	4be3      	ldr	r3, [pc, #908]	; (8b0 <operation_run+0x390>)
 524:	781d      	ldrb	r5, [r3, #0]
 526:	2d00      	cmp	r5, #0
 528:	d134      	bne.n	594 <operation_run+0x74>
 52a:	2401      	movs	r4, #1
 52c:	2102      	movs	r1, #2
 52e:	1c22      	adds	r2, r4, #0
 530:	2006      	movs	r0, #6
 532:	701c      	strb	r4, [r3, #0]
 534:	f7ff fe52 	bl	1dc <mbus_remote_register_read>
 538:	2064      	movs	r0, #100	; 0x64
 53a:	f7ff fdac 	bl	96 <delay>
 53e:	4bdd      	ldr	r3, [pc, #884]	; (8b4 <operation_run+0x394>)
 540:	2064      	movs	r0, #100	; 0x64
 542:	681a      	ldr	r2, [r3, #0]
 544:	4bdc      	ldr	r3, [pc, #880]	; (8b8 <operation_run+0x398>)
 546:	601a      	str	r2, [r3, #0]
 548:	f7ff fda5 	bl	96 <delay>
 54c:	2006      	movs	r0, #6
 54e:	1c21      	adds	r1, r4, #0
 550:	2209      	movs	r2, #9
 552:	f7ff fe36 	bl	1c2 <mbus_remote_register_write>
 556:	4bd9      	ldr	r3, [pc, #868]	; (8bc <operation_run+0x39c>)
 558:	601d      	str	r5, [r3, #0]
 55a:	4bd9      	ldr	r3, [pc, #868]	; (8c0 <operation_run+0x3a0>)
 55c:	681b      	ldr	r3, [r3, #0]
 55e:	2b00      	cmp	r3, #0
 560:	d104      	bne.n	56c <operation_run+0x4c>
 562:	4bd8      	ldr	r3, [pc, #864]	; (8c4 <operation_run+0x3a4>)
 564:	681b      	ldr	r3, [r3, #0]
 566:	191b      	adds	r3, r3, r4
 568:	2b04      	cmp	r3, #4
 56a:	d801      	bhi.n	570 <operation_run+0x50>
 56c:	f7ff fe54 	bl	218 <radio_power_on>
 570:	4bd5      	ldr	r3, [pc, #852]	; (8c8 <operation_run+0x3a8>)
 572:	4ad6      	ldr	r2, [pc, #856]	; (8cc <operation_run+0x3ac>)
 574:	6819      	ldr	r1, [r3, #0]
 576:	2005      	movs	r0, #5
 578:	400a      	ands	r2, r1
 57a:	601a      	str	r2, [r3, #0]
 57c:	681a      	ldr	r2, [r3, #0]
 57e:	2112      	movs	r1, #18
 580:	f7ff fe1f 	bl	1c2 <mbus_remote_register_write>
 584:	2101      	movs	r1, #1
 586:	2002      	movs	r0, #2
 588:	1c0a      	adds	r2, r1, #0
 58a:	f7ff fd99 	bl	c0 <set_wakeup_timer>
 58e:	f7ff fe01 	bl	194 <mbus_sleep_all>
 592:	e7fe      	b.n	592 <operation_run+0x72>
 594:	781c      	ldrb	r4, [r3, #0]
 596:	2c01      	cmp	r4, #1
 598:	d113      	bne.n	5c2 <operation_run+0xa2>
 59a:	2502      	movs	r5, #2
 59c:	701d      	strb	r5, [r3, #0]
 59e:	4bca      	ldr	r3, [pc, #808]	; (8c8 <operation_run+0x3a8>)
 5a0:	4acb      	ldr	r2, [pc, #812]	; (8d0 <operation_run+0x3b0>)
 5a2:	6819      	ldr	r1, [r3, #0]
 5a4:	2005      	movs	r0, #5
 5a6:	400a      	ands	r2, r1
 5a8:	601a      	str	r2, [r3, #0]
 5aa:	681a      	ldr	r2, [r3, #0]
 5ac:	2112      	movs	r1, #18
 5ae:	f7ff fe08 	bl	1c2 <mbus_remote_register_write>
 5b2:	1c28      	adds	r0, r5, #0
 5b4:	1c21      	adds	r1, r4, #0
 5b6:	1c22      	adds	r2, r4, #0
 5b8:	f7ff fd82 	bl	c0 <set_wakeup_timer>
 5bc:	f7ff fdea 	bl	194 <mbus_sleep_all>
 5c0:	e7fe      	b.n	5c0 <operation_run+0xa0>
 5c2:	781a      	ldrb	r2, [r3, #0]
 5c4:	2a02      	cmp	r2, #2
 5c6:	d111      	bne.n	5ec <operation_run+0xcc>
 5c8:	2203      	movs	r2, #3
 5ca:	701a      	strb	r2, [r3, #0]
 5cc:	4bc1      	ldr	r3, [pc, #772]	; (8d4 <operation_run+0x3b4>)
 5ce:	2280      	movs	r2, #128	; 0x80
 5d0:	6819      	ldr	r1, [r3, #0]
 5d2:	03d2      	lsls	r2, r2, #15
 5d4:	430a      	orrs	r2, r1
 5d6:	601a      	str	r2, [r3, #0]
 5d8:	6819      	ldr	r1, [r3, #0]
 5da:	4abf      	ldr	r2, [pc, #764]	; (8d8 <operation_run+0x3b8>)
 5dc:	2005      	movs	r0, #5
 5de:	400a      	ands	r2, r1
 5e0:	601a      	str	r2, [r3, #0]
 5e2:	681a      	ldr	r2, [r3, #0]
 5e4:	210e      	movs	r1, #14
 5e6:	f7ff fdec 	bl	1c2 <mbus_remote_register_write>
 5ea:	e1ca      	b.n	982 <operation_run+0x462>
 5ec:	781a      	ldrb	r2, [r3, #0]
 5ee:	2a03      	cmp	r2, #3
 5f0:	d12a      	bne.n	648 <operation_run+0x128>
 5f2:	4dba      	ldr	r5, [pc, #744]	; (8dc <operation_run+0x3bc>)
 5f4:	2300      	movs	r3, #0
 5f6:	702b      	strb	r3, [r5, #0]
 5f8:	4bb6      	ldr	r3, [pc, #728]	; (8d4 <operation_run+0x3b4>)
 5fa:	4ab9      	ldr	r2, [pc, #740]	; (8e0 <operation_run+0x3c0>)
 5fc:	6819      	ldr	r1, [r3, #0]
 5fe:	2005      	movs	r0, #5
 600:	400a      	ands	r2, r1
 602:	601a      	str	r2, [r3, #0]
 604:	681a      	ldr	r2, [r3, #0]
 606:	210e      	movs	r1, #14
 608:	f7ff fddb 	bl	1c2 <mbus_remote_register_write>
 60c:	24fa      	movs	r4, #250	; 0xfa
 60e:	00a4      	lsls	r4, r4, #2
 610:	782b      	ldrb	r3, [r5, #0]
 612:	2b00      	cmp	r3, #0
 614:	d004      	beq.n	620 <operation_run+0x100>
 616:	4aa9      	ldr	r2, [pc, #676]	; (8bc <operation_run+0x39c>)
 618:	2300      	movs	r3, #0
 61a:	702b      	strb	r3, [r5, #0]
 61c:	6013      	str	r3, [r2, #0]
 61e:	e00f      	b.n	640 <operation_run+0x120>
 620:	2064      	movs	r0, #100	; 0x64
 622:	3c01      	subs	r4, #1
 624:	f7ff fd37 	bl	96 <delay>
 628:	2c00      	cmp	r4, #0
 62a:	d1f1      	bne.n	610 <operation_run+0xf0>
 62c:	49ad      	ldr	r1, [pc, #692]	; (8e4 <operation_run+0x3c4>)
 62e:	20aa      	movs	r0, #170	; 0xaa
 630:	f7ff fd86 	bl	140 <mbus_write_message32>
 634:	f7ff feba 	bl	3ac <temp_sensor_disable>
 638:	4ba0      	ldr	r3, [pc, #640]	; (8bc <operation_run+0x39c>)
 63a:	681a      	ldr	r2, [r3, #0]
 63c:	3201      	adds	r2, #1
 63e:	601a      	str	r2, [r3, #0]
 640:	4b9b      	ldr	r3, [pc, #620]	; (8b0 <operation_run+0x390>)
 642:	2204      	movs	r2, #4
 644:	701a      	strb	r2, [r3, #0]
 646:	e19c      	b.n	982 <operation_run+0x462>
 648:	781b      	ldrb	r3, [r3, #0]
 64a:	2b04      	cmp	r3, #4
 64c:	d000      	beq.n	650 <operation_run+0x130>
 64e:	e192      	b.n	976 <operation_run+0x456>
 650:	2100      	movs	r1, #0
 652:	2203      	movs	r2, #3
 654:	2007      	movs	r0, #7
 656:	f7ff fdb4 	bl	1c2 <mbus_remote_register_write>
 65a:	2064      	movs	r0, #100	; 0x64
 65c:	f7ff fd1b 	bl	96 <delay>
 660:	23a0      	movs	r3, #160	; 0xa0
 662:	061b      	lsls	r3, r3, #24
 664:	681a      	ldr	r2, [r3, #0]
 666:	4ba0      	ldr	r3, [pc, #640]	; (8e8 <operation_run+0x3c8>)
 668:	b2d2      	uxtb	r2, r2
 66a:	2064      	movs	r0, #100	; 0x64
 66c:	601a      	str	r2, [r3, #0]
 66e:	4c91      	ldr	r4, [pc, #580]	; (8b4 <operation_run+0x394>)
 670:	f7ff fd11 	bl	96 <delay>
 674:	f7ff fdb8 	bl	1e8 <batadc_reset>
 678:	2064      	movs	r0, #100	; 0x64
 67a:	f7ff fd0c 	bl	96 <delay>
 67e:	f7ff fd43 	bl	108 <set_halt_until_mbus_rx>
 682:	2201      	movs	r2, #1
 684:	2005      	movs	r0, #5
 686:	2110      	movs	r1, #16
 688:	f7ff fda8 	bl	1dc <mbus_remote_register_read>
 68c:	6821      	ldr	r1, [r4, #0]
 68e:	2064      	movs	r0, #100	; 0x64
 690:	9101      	str	r1, [sp, #4]
 692:	f7ff fd00 	bl	96 <delay>
 696:	4b89      	ldr	r3, [pc, #548]	; (8bc <operation_run+0x39c>)
 698:	681a      	ldr	r2, [r3, #0]
 69a:	2a00      	cmp	r2, #0
 69c:	d106      	bne.n	6ac <operation_run+0x18c>
 69e:	2005      	movs	r0, #5
 6a0:	2111      	movs	r1, #17
 6a2:	2201      	movs	r2, #1
 6a4:	f7ff fd9a 	bl	1dc <mbus_remote_register_read>
 6a8:	6825      	ldr	r5, [r4, #0]
 6aa:	e005      	b.n	6b8 <operation_run+0x198>
 6ac:	2500      	movs	r5, #0
 6ae:	601d      	str	r5, [r3, #0]
 6b0:	4b8e      	ldr	r3, [pc, #568]	; (8ec <operation_run+0x3cc>)
 6b2:	681a      	ldr	r2, [r3, #0]
 6b4:	3201      	adds	r2, #1
 6b6:	601a      	str	r2, [r3, #0]
 6b8:	2064      	movs	r0, #100	; 0x64
 6ba:	f7ff fcec 	bl	96 <delay>
 6be:	f7ff fd31 	bl	124 <set_halt_until_mbus_tx>
 6c2:	2064      	movs	r0, #100	; 0x64
 6c4:	f7ff fce7 	bl	96 <delay>
 6c8:	f7ff fd9a 	bl	200 <batadc_resetrelease>
 6cc:	f7ff fe6e 	bl	3ac <temp_sensor_disable>
 6d0:	f7ff fe7c 	bl	3cc <temp_sensor_assert_reset>
 6d4:	f7ff fe90 	bl	3f8 <temp_power_off>
 6d8:	4b75      	ldr	r3, [pc, #468]	; (8b0 <operation_run+0x390>)
 6da:	2200      	movs	r2, #0
 6dc:	4c79      	ldr	r4, [pc, #484]	; (8c4 <operation_run+0x3a4>)
 6de:	701a      	strb	r2, [r3, #0]
 6e0:	6821      	ldr	r1, [r4, #0]
 6e2:	20cc      	movs	r0, #204	; 0xcc
 6e4:	f7ff fd2c 	bl	140 <mbus_write_message32>
 6e8:	2064      	movs	r0, #100	; 0x64
 6ea:	f7ff fcd4 	bl	96 <delay>
 6ee:	9901      	ldr	r1, [sp, #4]
 6f0:	20cc      	movs	r0, #204	; 0xcc
 6f2:	f7ff fd25 	bl	140 <mbus_write_message32>
 6f6:	2064      	movs	r0, #100	; 0x64
 6f8:	f7ff fccd 	bl	96 <delay>
 6fc:	1c29      	adds	r1, r5, #0
 6fe:	20cc      	movs	r0, #204	; 0xcc
 700:	f7ff fd1e 	bl	140 <mbus_write_message32>
 704:	2064      	movs	r0, #100	; 0x64
 706:	f7ff fcc6 	bl	96 <delay>
 70a:	4e6b      	ldr	r6, [pc, #428]	; (8b8 <operation_run+0x398>)
 70c:	20cc      	movs	r0, #204	; 0xcc
 70e:	6831      	ldr	r1, [r6, #0]
 710:	f7ff fd16 	bl	140 <mbus_write_message32>
 714:	2064      	movs	r0, #100	; 0x64
 716:	f7ff fcbe 	bl	96 <delay>
 71a:	4f73      	ldr	r7, [pc, #460]	; (8e8 <operation_run+0x3c8>)
 71c:	20cc      	movs	r0, #204	; 0xcc
 71e:	6839      	ldr	r1, [r7, #0]
 720:	f7ff fd0e 	bl	140 <mbus_write_message32>
 724:	2064      	movs	r0, #100	; 0x64
 726:	f7ff fcb6 	bl	96 <delay>
 72a:	6823      	ldr	r3, [r4, #0]
 72c:	3301      	adds	r3, #1
 72e:	6023      	str	r3, [r4, #0]
 730:	4c6f      	ldr	r4, [pc, #444]	; (8f0 <operation_run+0x3d0>)
 732:	4b70      	ldr	r3, [pc, #448]	; (8f4 <operation_run+0x3d4>)
 734:	6822      	ldr	r2, [r4, #0]
 736:	429a      	cmp	r2, r3
 738:	d84b      	bhi.n	7d2 <operation_run+0x2b2>
 73a:	6823      	ldr	r3, [r4, #0]
 73c:	6832      	ldr	r2, [r6, #0]
 73e:	496e      	ldr	r1, [pc, #440]	; (8f8 <operation_run+0x3d8>)
 740:	0c12      	lsrs	r2, r2, #16
 742:	54ca      	strb	r2, [r1, r3]
 744:	6823      	ldr	r3, [r4, #0]
 746:	6832      	ldr	r2, [r6, #0]
 748:	4e6c      	ldr	r6, [pc, #432]	; (8fc <operation_run+0x3dc>)
 74a:	005b      	lsls	r3, r3, #1
 74c:	539a      	strh	r2, [r3, r6]
 74e:	6821      	ldr	r1, [r4, #0]
 750:	20dd      	movs	r0, #221	; 0xdd
 752:	f7ff fcf5 	bl	140 <mbus_write_message32>
 756:	2064      	movs	r0, #100	; 0x64
 758:	f7ff fc9d 	bl	96 <delay>
 75c:	4a66      	ldr	r2, [pc, #408]	; (8f8 <operation_run+0x3d8>)
 75e:	6823      	ldr	r3, [r4, #0]
 760:	20dd      	movs	r0, #221	; 0xdd
 762:	5cd1      	ldrb	r1, [r2, r3]
 764:	f7ff fcec 	bl	140 <mbus_write_message32>
 768:	2064      	movs	r0, #100	; 0x64
 76a:	f7ff fc94 	bl	96 <delay>
 76e:	6823      	ldr	r3, [r4, #0]
 770:	20dd      	movs	r0, #221	; 0xdd
 772:	005b      	lsls	r3, r3, #1
 774:	5b99      	ldrh	r1, [r3, r6]
 776:	f7ff fce3 	bl	140 <mbus_write_message32>
 77a:	2064      	movs	r0, #100	; 0x64
 77c:	f7ff fc8b 	bl	96 <delay>
 780:	6823      	ldr	r3, [r4, #0]
 782:	495d      	ldr	r1, [pc, #372]	; (8f8 <operation_run+0x3d8>)
 784:	3301      	adds	r3, #1
 786:	6023      	str	r3, [r4, #0]
 788:	6823      	ldr	r3, [r4, #0]
 78a:	683a      	ldr	r2, [r7, #0]
 78c:	20dd      	movs	r0, #221	; 0xdd
 78e:	54ca      	strb	r2, [r1, r3]
 790:	6822      	ldr	r2, [r4, #0]
 792:	0052      	lsls	r2, r2, #1
 794:	5395      	strh	r5, [r2, r6]
 796:	6821      	ldr	r1, [r4, #0]
 798:	f7ff fcd2 	bl	140 <mbus_write_message32>
 79c:	2064      	movs	r0, #100	; 0x64
 79e:	f7ff fc7a 	bl	96 <delay>
 7a2:	4a55      	ldr	r2, [pc, #340]	; (8f8 <operation_run+0x3d8>)
 7a4:	6823      	ldr	r3, [r4, #0]
 7a6:	20dd      	movs	r0, #221	; 0xdd
 7a8:	5cd1      	ldrb	r1, [r2, r3]
 7aa:	f7ff fcc9 	bl	140 <mbus_write_message32>
 7ae:	2064      	movs	r0, #100	; 0x64
 7b0:	f7ff fc71 	bl	96 <delay>
 7b4:	6823      	ldr	r3, [r4, #0]
 7b6:	20dd      	movs	r0, #221	; 0xdd
 7b8:	005b      	lsls	r3, r3, #1
 7ba:	5b99      	ldrh	r1, [r3, r6]
 7bc:	f7ff fcc0 	bl	140 <mbus_write_message32>
 7c0:	2064      	movs	r0, #100	; 0x64
 7c2:	f7ff fc68 	bl	96 <delay>
 7c6:	6822      	ldr	r2, [r4, #0]
 7c8:	4b4d      	ldr	r3, [pc, #308]	; (900 <operation_run+0x3e0>)
 7ca:	601a      	str	r2, [r3, #0]
 7cc:	6823      	ldr	r3, [r4, #0]
 7ce:	3301      	adds	r3, #1
 7d0:	6023      	str	r3, [r4, #0]
 7d2:	4b3b      	ldr	r3, [pc, #236]	; (8c0 <operation_run+0x3a0>)
 7d4:	681b      	ldr	r3, [r3, #0]
 7d6:	2b00      	cmp	r3, #0
 7d8:	d01d      	beq.n	816 <operation_run+0x2f6>
 7da:	9901      	ldr	r1, [sp, #4]
 7dc:	2000      	movs	r0, #0
 7de:	f7ff fe3d 	bl	45c <send_radio_data_ppm>
 7e2:	20fa      	movs	r0, #250	; 0xfa
 7e4:	0100      	lsls	r0, r0, #4
 7e6:	f7ff fc56 	bl	96 <delay>
 7ea:	1c29      	adds	r1, r5, #0
 7ec:	2000      	movs	r0, #0
 7ee:	f7ff fe35 	bl	45c <send_radio_data_ppm>
 7f2:	20fa      	movs	r0, #250	; 0xfa
 7f4:	0100      	lsls	r0, r0, #4
 7f6:	f7ff fc4e 	bl	96 <delay>
 7fa:	4b2f      	ldr	r3, [pc, #188]	; (8b8 <operation_run+0x398>)
 7fc:	2000      	movs	r0, #0
 7fe:	6819      	ldr	r1, [r3, #0]
 800:	f7ff fe2c 	bl	45c <send_radio_data_ppm>
 804:	20fa      	movs	r0, #250	; 0xfa
 806:	0100      	lsls	r0, r0, #4
 808:	f7ff fc45 	bl	96 <delay>
 80c:	4b36      	ldr	r3, [pc, #216]	; (8e8 <operation_run+0x3c8>)
 80e:	2000      	movs	r0, #0
 810:	6819      	ldr	r1, [r3, #0]
 812:	f7ff fe23 	bl	45c <send_radio_data_ppm>
 816:	4b2b      	ldr	r3, [pc, #172]	; (8c4 <operation_run+0x3a4>)
 818:	681b      	ldr	r3, [r3, #0]
 81a:	2b04      	cmp	r3, #4
 81c:	d809      	bhi.n	832 <operation_run+0x312>
 81e:	20fa      	movs	r0, #250	; 0xfa
 820:	0100      	lsls	r0, r0, #4
 822:	f7ff fc38 	bl	96 <delay>
 826:	2001      	movs	r0, #1
 828:	4936      	ldr	r1, [pc, #216]	; (904 <operation_run+0x3e4>)
 82a:	f7ff fe17 	bl	45c <send_radio_data_ppm>
 82e:	4b36      	ldr	r3, [pc, #216]	; (908 <operation_run+0x3e8>)
 830:	e000      	b.n	834 <operation_run+0x314>
 832:	4b36      	ldr	r3, [pc, #216]	; (90c <operation_run+0x3ec>)
 834:	6818      	ldr	r0, [r3, #0]
 836:	2101      	movs	r1, #1
 838:	b280      	uxth	r0, r0
 83a:	1c0a      	adds	r2, r1, #0
 83c:	f7ff fc40 	bl	c0 <set_wakeup_timer>
 840:	4b33      	ldr	r3, [pc, #204]	; (910 <operation_run+0x3f0>)
 842:	781b      	ldrb	r3, [r3, #0]
 844:	2b00      	cmp	r3, #0
 846:	d004      	beq.n	852 <operation_run+0x332>
 848:	4b32      	ldr	r3, [pc, #200]	; (914 <operation_run+0x3f4>)
 84a:	2200      	movs	r2, #0
 84c:	701a      	strb	r2, [r3, #0]
 84e:	f7ff fd41 	bl	2d4 <radio_power_off>
 852:	2101      	movs	r1, #1
 854:	2219      	movs	r2, #25
 856:	2006      	movs	r0, #6
 858:	f7ff fcb3 	bl	1c2 <mbus_remote_register_write>
 85c:	4a22      	ldr	r2, [pc, #136]	; (8e8 <operation_run+0x3c8>)
 85e:	4b1a      	ldr	r3, [pc, #104]	; (8c8 <operation_run+0x3a8>)
 860:	6811      	ldr	r1, [r2, #0]
 862:	2900      	cmp	r1, #0
 864:	d05a      	beq.n	91c <operation_run+0x3fc>
 866:	4c2c      	ldr	r4, [pc, #176]	; (918 <operation_run+0x3f8>)
 868:	6810      	ldr	r0, [r2, #0]
 86a:	6821      	ldr	r1, [r4, #0]
 86c:	4288      	cmp	r0, r1
 86e:	d255      	bcs.n	91c <operation_run+0x3fc>
 870:	6818      	ldr	r0, [r3, #0]
 872:	4917      	ldr	r1, [pc, #92]	; (8d0 <operation_run+0x3b0>)
 874:	4001      	ands	r1, r0
 876:	6019      	str	r1, [r3, #0]
 878:	6818      	ldr	r0, [r3, #0]
 87a:	4914      	ldr	r1, [pc, #80]	; (8cc <operation_run+0x3ac>)
 87c:	4001      	ands	r1, r0
 87e:	6019      	str	r1, [r3, #0]
 880:	6819      	ldr	r1, [r3, #0]
 882:	2002      	movs	r0, #2
 884:	4381      	bics	r1, r0
 886:	6019      	str	r1, [r3, #0]
 888:	6819      	ldr	r1, [r3, #0]
 88a:	2001      	movs	r0, #1
 88c:	4381      	bics	r1, r0
 88e:	6019      	str	r1, [r3, #0]
 890:	6811      	ldr	r1, [r2, #0]
 892:	20bb      	movs	r0, #187	; 0xbb
 894:	f7ff fc54 	bl	140 <mbus_write_message32>
 898:	2064      	movs	r0, #100	; 0x64
 89a:	f7ff fbfc 	bl	96 <delay>
 89e:	6821      	ldr	r1, [r4, #0]
 8a0:	20bb      	movs	r0, #187	; 0xbb
 8a2:	f7ff fc4d 	bl	140 <mbus_write_message32>
 8a6:	2064      	movs	r0, #100	; 0x64
 8a8:	f7ff fbf5 	bl	96 <delay>
 8ac:	e048      	b.n	940 <operation_run+0x420>
 8ae:	46c0      	nop			; (mov r8, r8)
 8b0:	00001324 	.word	0x00001324
 8b4:	a0000004 	.word	0xa0000004
 8b8:	0000133c 	.word	0x0000133c
 8bc:	00001340 	.word	0x00001340
 8c0:	00001318 	.word	0x00001318
 8c4:	00001310 	.word	0x00001310
 8c8:	000012fc 	.word	0x000012fc
 8cc:	fffff7ff 	.word	0xfffff7ff
 8d0:	ffffefff 	.word	0xffffefff
 8d4:	000012f4 	.word	0x000012f4
 8d8:	ffdfffff 	.word	0xffdfffff
 8dc:	0000132c 	.word	0x0000132c
 8e0:	fffeffff 	.word	0xfffeffff
 8e4:	fafafafa 	.word	0xfafafafa
 8e8:	00001320 	.word	0x00001320
 8ec:	00001350 	.word	0x00001350
 8f0:	0000134c 	.word	0x0000134c
 8f4:	00000419 	.word	0x00000419
 8f8:	00001355 	.word	0x00001355
 8fc:	00001770 	.word	0x00001770
 900:	00001308 	.word	0x00001308
 904:	00faf000 	.word	0x00faf000
 908:	00001328 	.word	0x00001328
 90c:	00001348 	.word	0x00001348
 910:	00001354 	.word	0x00001354
 914:	00001304 	.word	0x00001304
 918:	0000131c 	.word	0x0000131c
 91c:	6819      	ldr	r1, [r3, #0]
 91e:	2280      	movs	r2, #128	; 0x80
 920:	0152      	lsls	r2, r2, #5
 922:	430a      	orrs	r2, r1
 924:	601a      	str	r2, [r3, #0]
 926:	6819      	ldr	r1, [r3, #0]
 928:	2280      	movs	r2, #128	; 0x80
 92a:	0112      	lsls	r2, r2, #4
 92c:	430a      	orrs	r2, r1
 92e:	601a      	str	r2, [r3, #0]
 930:	681a      	ldr	r2, [r3, #0]
 932:	2102      	movs	r1, #2
 934:	430a      	orrs	r2, r1
 936:	601a      	str	r2, [r3, #0]
 938:	681a      	ldr	r2, [r3, #0]
 93a:	2101      	movs	r1, #1
 93c:	430a      	orrs	r2, r1
 93e:	601a      	str	r2, [r3, #0]
 940:	4b10      	ldr	r3, [pc, #64]	; (984 <operation_run+0x464>)
 942:	2005      	movs	r0, #5
 944:	681a      	ldr	r2, [r3, #0]
 946:	2112      	movs	r1, #18
 948:	f7ff fc3b 	bl	1c2 <mbus_remote_register_write>
 94c:	2064      	movs	r0, #100	; 0x64
 94e:	f7ff fba2 	bl	96 <delay>
 952:	4b0d      	ldr	r3, [pc, #52]	; (988 <operation_run+0x468>)
 954:	681a      	ldr	r2, [r3, #0]
 956:	2a00      	cmp	r2, #0
 958:	d00a      	beq.n	970 <operation_run+0x450>
 95a:	4a0c      	ldr	r2, [pc, #48]	; (98c <operation_run+0x46c>)
 95c:	2132      	movs	r1, #50	; 0x32
 95e:	6812      	ldr	r2, [r2, #0]
 960:	681b      	ldr	r3, [r3, #0]
 962:	4099      	lsls	r1, r3
 964:	428a      	cmp	r2, r1
 966:	d903      	bls.n	970 <operation_run+0x450>
 968:	4b09      	ldr	r3, [pc, #36]	; (990 <operation_run+0x470>)
 96a:	2200      	movs	r2, #0
 96c:	701a      	strb	r2, [r3, #0]
 96e:	e004      	b.n	97a <operation_run+0x45a>
 970:	f7ff fc10 	bl	194 <mbus_sleep_all>
 974:	e7fe      	b.n	974 <operation_run+0x454>
 976:	f7ff fd29 	bl	3cc <temp_sensor_assert_reset>
 97a:	f7ff fd3d 	bl	3f8 <temp_power_off>
 97e:	f7ff fd57 	bl	430 <operation_sleep_notimer>
 982:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
 984:	000012fc 	.word	0x000012fc
 988:	00001334 	.word	0x00001334
 98c:	00001310 	.word	0x00001310
 990:	0000130c 	.word	0x0000130c

Disassembly of section .text.handler_ext_int_0:

00000994 <handler_ext_int_0>:
 994:	4b01      	ldr	r3, [pc, #4]	; (99c <handler_ext_int_0+0x8>)
 996:	2201      	movs	r2, #1
 998:	601a      	str	r2, [r3, #0]
 99a:	4770      	bx	lr
 99c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

000009a0 <handler_ext_int_1>:
 9a0:	4b01      	ldr	r3, [pc, #4]	; (9a8 <handler_ext_int_1+0x8>)
 9a2:	2202      	movs	r2, #2
 9a4:	601a      	str	r2, [r3, #0]
 9a6:	4770      	bx	lr
 9a8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

000009ac <handler_ext_int_2>:
 9ac:	4b03      	ldr	r3, [pc, #12]	; (9bc <handler_ext_int_2+0x10>)
 9ae:	2204      	movs	r2, #4
 9b0:	601a      	str	r2, [r3, #0]
 9b2:	4b03      	ldr	r3, [pc, #12]	; (9c0 <handler_ext_int_2+0x14>)
 9b4:	2210      	movs	r2, #16
 9b6:	701a      	strb	r2, [r3, #0]
 9b8:	4770      	bx	lr
 9ba:	46c0      	nop			; (mov r8, r8)
 9bc:	e000e280 	.word	0xe000e280
 9c0:	0000132c 	.word	0x0000132c

Disassembly of section .text.handler_ext_int_3:

000009c4 <handler_ext_int_3>:
 9c4:	4b03      	ldr	r3, [pc, #12]	; (9d4 <handler_ext_int_3+0x10>)
 9c6:	2208      	movs	r2, #8
 9c8:	601a      	str	r2, [r3, #0]
 9ca:	4b03      	ldr	r3, [pc, #12]	; (9d8 <handler_ext_int_3+0x14>)
 9cc:	2211      	movs	r2, #17
 9ce:	701a      	strb	r2, [r3, #0]
 9d0:	4770      	bx	lr
 9d2:	46c0      	nop			; (mov r8, r8)
 9d4:	e000e280 	.word	0xe000e280
 9d8:	0000132c 	.word	0x0000132c

Disassembly of section .text.handler_ext_int_4:

000009dc <handler_ext_int_4>:
 9dc:	4b03      	ldr	r3, [pc, #12]	; (9ec <handler_ext_int_4+0x10>)
 9de:	2210      	movs	r2, #16
 9e0:	601a      	str	r2, [r3, #0]
 9e2:	4b03      	ldr	r3, [pc, #12]	; (9f0 <handler_ext_int_4+0x14>)
 9e4:	2212      	movs	r2, #18
 9e6:	701a      	strb	r2, [r3, #0]
 9e8:	4770      	bx	lr
 9ea:	46c0      	nop			; (mov r8, r8)
 9ec:	e000e280 	.word	0xe000e280
 9f0:	0000132c 	.word	0x0000132c

Disassembly of section .text.handler_ext_int_5:

000009f4 <handler_ext_int_5>:
 9f4:	4b03      	ldr	r3, [pc, #12]	; (a04 <handler_ext_int_5+0x10>)
 9f6:	2220      	movs	r2, #32
 9f8:	601a      	str	r2, [r3, #0]
 9fa:	4b03      	ldr	r3, [pc, #12]	; (a08 <handler_ext_int_5+0x14>)
 9fc:	2213      	movs	r2, #19
 9fe:	701a      	strb	r2, [r3, #0]
 a00:	4770      	bx	lr
 a02:	46c0      	nop			; (mov r8, r8)
 a04:	e000e280 	.word	0xe000e280
 a08:	0000132c 	.word	0x0000132c

Disassembly of section .text.handler_ext_int_6:

00000a0c <handler_ext_int_6>:
 a0c:	4b03      	ldr	r3, [pc, #12]	; (a1c <handler_ext_int_6+0x10>)
 a0e:	2240      	movs	r2, #64	; 0x40
 a10:	601a      	str	r2, [r3, #0]
 a12:	4b03      	ldr	r3, [pc, #12]	; (a20 <handler_ext_int_6+0x14>)
 a14:	2214      	movs	r2, #20
 a16:	701a      	strb	r2, [r3, #0]
 a18:	4770      	bx	lr
 a1a:	46c0      	nop			; (mov r8, r8)
 a1c:	e000e280 	.word	0xe000e280
 a20:	0000132c 	.word	0x0000132c

Disassembly of section .text.handler_ext_int_7:

00000a24 <handler_ext_int_7>:
 a24:	4b03      	ldr	r3, [pc, #12]	; (a34 <handler_ext_int_7+0x10>)
 a26:	2280      	movs	r2, #128	; 0x80
 a28:	601a      	str	r2, [r3, #0]
 a2a:	4b03      	ldr	r3, [pc, #12]	; (a38 <handler_ext_int_7+0x14>)
 a2c:	2215      	movs	r2, #21
 a2e:	701a      	strb	r2, [r3, #0]
 a30:	4770      	bx	lr
 a32:	46c0      	nop			; (mov r8, r8)
 a34:	e000e280 	.word	0xe000e280
 a38:	0000132c 	.word	0x0000132c

Disassembly of section .text.handler_ext_int_8:

00000a3c <handler_ext_int_8>:
 a3c:	4b03      	ldr	r3, [pc, #12]	; (a4c <handler_ext_int_8+0x10>)
 a3e:	2280      	movs	r2, #128	; 0x80
 a40:	0052      	lsls	r2, r2, #1
 a42:	601a      	str	r2, [r3, #0]
 a44:	4b02      	ldr	r3, [pc, #8]	; (a50 <handler_ext_int_8+0x14>)
 a46:	2216      	movs	r2, #22
 a48:	701a      	strb	r2, [r3, #0]
 a4a:	4770      	bx	lr
 a4c:	e000e280 	.word	0xe000e280
 a50:	0000132c 	.word	0x0000132c

Disassembly of section .text.handler_ext_int_9:

00000a54 <handler_ext_int_9>:
 a54:	4b03      	ldr	r3, [pc, #12]	; (a64 <handler_ext_int_9+0x10>)
 a56:	2280      	movs	r2, #128	; 0x80
 a58:	0092      	lsls	r2, r2, #2
 a5a:	601a      	str	r2, [r3, #0]
 a5c:	4b02      	ldr	r3, [pc, #8]	; (a68 <handler_ext_int_9+0x14>)
 a5e:	2217      	movs	r2, #23
 a60:	701a      	strb	r2, [r3, #0]
 a62:	4770      	bx	lr
 a64:	e000e280 	.word	0xe000e280
 a68:	0000132c 	.word	0x0000132c

Disassembly of section .text.handler_ext_int_10:

00000a6c <handler_ext_int_10>:
 a6c:	4b02      	ldr	r3, [pc, #8]	; (a78 <handler_ext_int_10+0xc>)
 a6e:	2280      	movs	r2, #128	; 0x80
 a70:	00d2      	lsls	r2, r2, #3
 a72:	601a      	str	r2, [r3, #0]
 a74:	4770      	bx	lr
 a76:	46c0      	nop			; (mov r8, r8)
 a78:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

00000a7c <handler_ext_int_11>:
 a7c:	4b02      	ldr	r3, [pc, #8]	; (a88 <handler_ext_int_11+0xc>)
 a7e:	2280      	movs	r2, #128	; 0x80
 a80:	0112      	lsls	r2, r2, #4
 a82:	601a      	str	r2, [r3, #0]
 a84:	4770      	bx	lr
 a86:	46c0      	nop			; (mov r8, r8)
 a88:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

00000a8c <handler_ext_int_12>:
 a8c:	4b02      	ldr	r3, [pc, #8]	; (a98 <handler_ext_int_12+0xc>)
 a8e:	2280      	movs	r2, #128	; 0x80
 a90:	0152      	lsls	r2, r2, #5
 a92:	601a      	str	r2, [r3, #0]
 a94:	4770      	bx	lr
 a96:	46c0      	nop			; (mov r8, r8)
 a98:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

00000a9c <handler_ext_int_13>:
 a9c:	4b02      	ldr	r3, [pc, #8]	; (aa8 <handler_ext_int_13+0xc>)
 a9e:	2280      	movs	r2, #128	; 0x80
 aa0:	0192      	lsls	r2, r2, #6
 aa2:	601a      	str	r2, [r3, #0]
 aa4:	4770      	bx	lr
 aa6:	46c0      	nop			; (mov r8, r8)
 aa8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

00000aac <handler_ext_int_14>:
 aac:	4b02      	ldr	r3, [pc, #8]	; (ab8 <handler_ext_int_14+0xc>)
 aae:	2280      	movs	r2, #128	; 0x80
 ab0:	01d2      	lsls	r2, r2, #7
 ab2:	601a      	str	r2, [r3, #0]
 ab4:	4770      	bx	lr
 ab6:	46c0      	nop			; (mov r8, r8)
 ab8:	e000e280 	.word	0xe000e280

Disassembly of section .text.startup.main:

00000abc <main>:
     abc:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
     abe:	2100      	movs	r1, #0
     ac0:	2201      	movs	r2, #1
     ac2:	2064      	movs	r0, #100	; 0x64
     ac4:	f7ff fafc 	bl	c0 <set_wakeup_timer>
     ac8:	f7ff fb10 	bl	ec <enable_reg_irq>
     acc:	48f6      	ldr	r0, [pc, #984]	; (ea8 <main+0x3ec>)
     ace:	f7ff faeb 	bl	a8 <config_timerwd>
     ad2:	4af6      	ldr	r2, [pc, #984]	; (eac <main+0x3f0>)
     ad4:	49f6      	ldr	r1, [pc, #984]	; (eb0 <main+0x3f4>)
     ad6:	6813      	ldr	r3, [r2, #0]
     ad8:	428b      	cmp	r3, r1
     ada:	d100      	bne.n	ade <main+0x22>
     adc:	e181      	b.n	de2 <main+0x326>
     ade:	4bf5      	ldr	r3, [pc, #980]	; (eb4 <main+0x3f8>)
     ae0:	2580      	movs	r5, #128	; 0x80
     ae2:	6818      	ldr	r0, [r3, #0]
     ae4:	016d      	lsls	r5, r5, #5
     ae6:	4328      	orrs	r0, r5
     ae8:	6018      	str	r0, [r3, #0]
     aea:	6818      	ldr	r0, [r3, #0]
     aec:	4cf2      	ldr	r4, [pc, #968]	; (eb8 <main+0x3fc>)
     aee:	26c0      	movs	r6, #192	; 0xc0
     af0:	4020      	ands	r0, r4
     af2:	2480      	movs	r4, #128	; 0x80
     af4:	01a4      	lsls	r4, r4, #6
     af6:	4320      	orrs	r0, r4
     af8:	6018      	str	r0, [r3, #0]
     afa:	681c      	ldr	r4, [r3, #0]
     afc:	48ef      	ldr	r0, [pc, #956]	; (ebc <main+0x400>)
     afe:	02f6      	lsls	r6, r6, #11
     b00:	4020      	ands	r0, r4
     b02:	2480      	movs	r4, #128	; 0x80
     b04:	0224      	lsls	r4, r4, #8
     b06:	4320      	orrs	r0, r4
     b08:	6018      	str	r0, [r3, #0]
     b0a:	6818      	ldr	r0, [r3, #0]
     b0c:	2400      	movs	r4, #0
     b0e:	4330      	orrs	r0, r6
     b10:	6018      	str	r0, [r3, #0]
     b12:	6818      	ldr	r0, [r3, #0]
     b14:	4bea      	ldr	r3, [pc, #936]	; (ec0 <main+0x404>)
     b16:	2707      	movs	r7, #7
     b18:	6018      	str	r0, [r3, #0]
     b1a:	4bea      	ldr	r3, [pc, #936]	; (ec4 <main+0x408>)
     b1c:	2064      	movs	r0, #100	; 0x64
     b1e:	701c      	strb	r4, [r3, #0]
     b20:	4be9      	ldr	r3, [pc, #932]	; (ec8 <main+0x40c>)
     b22:	6011      	str	r1, [r2, #0]
     b24:	601c      	str	r4, [r3, #0]
     b26:	4be9      	ldr	r3, [pc, #932]	; (ecc <main+0x410>)
     b28:	601c      	str	r4, [r3, #0]
     b2a:	4be9      	ldr	r3, [pc, #932]	; (ed0 <main+0x414>)
     b2c:	701c      	strb	r4, [r3, #0]
     b2e:	4be9      	ldr	r3, [pc, #932]	; (ed4 <main+0x418>)
     b30:	601c      	str	r4, [r3, #0]
     b32:	4be9      	ldr	r3, [pc, #932]	; (ed8 <main+0x41c>)
     b34:	601c      	str	r4, [r3, #0]
     b36:	f7ff faae 	bl	96 <delay>
     b3a:	2004      	movs	r0, #4
     b3c:	f7ff fb20 	bl	180 <mbus_enumerate>
     b40:	2064      	movs	r0, #100	; 0x64
     b42:	f7ff faa8 	bl	96 <delay>
     b46:	2005      	movs	r0, #5
     b48:	f7ff fb1a 	bl	180 <mbus_enumerate>
     b4c:	2064      	movs	r0, #100	; 0x64
     b4e:	f7ff faa2 	bl	96 <delay>
     b52:	2006      	movs	r0, #6
     b54:	f7ff fb14 	bl	180 <mbus_enumerate>
     b58:	2064      	movs	r0, #100	; 0x64
     b5a:	f7ff fa9c 	bl	96 <delay>
     b5e:	2007      	movs	r0, #7
     b60:	f7ff fb0e 	bl	180 <mbus_enumerate>
     b64:	2064      	movs	r0, #100	; 0x64
     b66:	f7ff fa96 	bl	96 <delay>
     b6a:	2151      	movs	r1, #81	; 0x51
     b6c:	2209      	movs	r2, #9
     b6e:	2007      	movs	r0, #7
     b70:	f7ff fb27 	bl	1c2 <mbus_remote_register_write>
     b74:	2064      	movs	r0, #100	; 0x64
     b76:	f7ff fa8e 	bl	96 <delay>
     b7a:	2117      	movs	r1, #23
     b7c:	4ad7      	ldr	r2, [pc, #860]	; (edc <main+0x420>)
     b7e:	2007      	movs	r0, #7
     b80:	f7ff fb1f 	bl	1c2 <mbus_remote_register_write>
     b84:	2064      	movs	r0, #100	; 0x64
     b86:	f7ff fa86 	bl	96 <delay>
     b8a:	2117      	movs	r1, #23
     b8c:	4ad3      	ldr	r2, [pc, #844]	; (edc <main+0x420>)
     b8e:	2007      	movs	r0, #7
     b90:	f7ff fb17 	bl	1c2 <mbus_remote_register_write>
     b94:	2064      	movs	r0, #100	; 0x64
     b96:	f7ff fa7e 	bl	96 <delay>
     b9a:	2118      	movs	r1, #24
     b9c:	4ad0      	ldr	r2, [pc, #832]	; (ee0 <main+0x424>)
     b9e:	2007      	movs	r0, #7
     ba0:	f7ff fb0f 	bl	1c2 <mbus_remote_register_write>
     ba4:	2064      	movs	r0, #100	; 0x64
     ba6:	f7ff fa76 	bl	96 <delay>
     baa:	2119      	movs	r1, #25
     bac:	4acd      	ldr	r2, [pc, #820]	; (ee4 <main+0x428>)
     bae:	2007      	movs	r0, #7
     bb0:	f7ff fb07 	bl	1c2 <mbus_remote_register_write>
     bb4:	2064      	movs	r0, #100	; 0x64
     bb6:	f7ff fa6e 	bl	96 <delay>
     bba:	211a      	movs	r1, #26
     bbc:	4aca      	ldr	r2, [pc, #808]	; (ee8 <main+0x42c>)
     bbe:	2007      	movs	r0, #7
     bc0:	f7ff faff 	bl	1c2 <mbus_remote_register_write>
     bc4:	2064      	movs	r0, #100	; 0x64
     bc6:	f7ff fa66 	bl	96 <delay>
     bca:	2115      	movs	r1, #21
     bcc:	4ac7      	ldr	r2, [pc, #796]	; (eec <main+0x430>)
     bce:	2007      	movs	r0, #7
     bd0:	f7ff faf7 	bl	1c2 <mbus_remote_register_write>
     bd4:	2064      	movs	r0, #100	; 0x64
     bd6:	f7ff fa5e 	bl	96 <delay>
     bda:	2116      	movs	r1, #22
     bdc:	4ac4      	ldr	r2, [pc, #784]	; (ef0 <main+0x434>)
     bde:	2007      	movs	r0, #7
     be0:	f7ff faef 	bl	1c2 <mbus_remote_register_write>
     be4:	2064      	movs	r0, #100	; 0x64
     be6:	f7ff fa56 	bl	96 <delay>
     bea:	22ab      	movs	r2, #171	; 0xab
     bec:	2105      	movs	r1, #5
     bee:	0112      	lsls	r2, r2, #4
     bf0:	2007      	movs	r0, #7
     bf2:	f7ff fae6 	bl	1c2 <mbus_remote_register_write>
     bf6:	2064      	movs	r0, #100	; 0x64
     bf8:	f7ff fa4d 	bl	96 <delay>
     bfc:	2136      	movs	r1, #54	; 0x36
     bfe:	2201      	movs	r2, #1
     c00:	2007      	movs	r0, #7
     c02:	f7ff fade 	bl	1c2 <mbus_remote_register_write>
     c06:	2064      	movs	r0, #100	; 0x64
     c08:	f7ff fa45 	bl	96 <delay>
     c0c:	2006      	movs	r0, #6
     c0e:	1c21      	adds	r1, r4, #0
     c10:	220f      	movs	r2, #15
     c12:	f7ff fad6 	bl	1c2 <mbus_remote_register_write>
     c16:	2006      	movs	r0, #6
     c18:	2101      	movs	r1, #1
     c1a:	2209      	movs	r2, #9
     c1c:	f7ff fad1 	bl	1c2 <mbus_remote_register_write>
     c20:	2006      	movs	r0, #6
     c22:	2103      	movs	r1, #3
     c24:	4ab3      	ldr	r2, [pc, #716]	; (ef4 <main+0x438>)
     c26:	f7ff facc 	bl	1c2 <mbus_remote_register_write>
     c2a:	2006      	movs	r0, #6
     c2c:	2104      	movs	r1, #4
     c2e:	4ab2      	ldr	r2, [pc, #712]	; (ef8 <main+0x43c>)
     c30:	f7ff fac7 	bl	1c2 <mbus_remote_register_write>
     c34:	1c2a      	adds	r2, r5, #0
     c36:	2006      	movs	r0, #6
     c38:	2105      	movs	r1, #5
     c3a:	f7ff fac2 	bl	1c2 <mbus_remote_register_write>
     c3e:	2006      	movs	r0, #6
     c40:	2101      	movs	r1, #1
     c42:	2209      	movs	r2, #9
     c44:	f7ff fabd 	bl	1c2 <mbus_remote_register_write>
     c48:	4bac      	ldr	r3, [pc, #688]	; (efc <main+0x440>)
     c4a:	2005      	movs	r0, #5
     c4c:	681a      	ldr	r2, [r3, #0]
     c4e:	2119      	movs	r1, #25
     c50:	0e12      	lsrs	r2, r2, #24
     c52:	0612      	lsls	r2, r2, #24
     c54:	432a      	orrs	r2, r5
     c56:	601a      	str	r2, [r3, #0]
     c58:	681a      	ldr	r2, [r3, #0]
     c5a:	f7ff fab2 	bl	1c2 <mbus_remote_register_write>
     c5e:	4ba8      	ldr	r3, [pc, #672]	; (f00 <main+0x444>)
     c60:	4aa8      	ldr	r2, [pc, #672]	; (f04 <main+0x448>)
     c62:	6819      	ldr	r1, [r3, #0]
     c64:	2005      	movs	r0, #5
     c66:	400a      	ands	r2, r1
     c68:	4316      	orrs	r6, r2
     c6a:	601e      	str	r6, [r3, #0]
     c6c:	681a      	ldr	r2, [r3, #0]
     c6e:	210e      	movs	r1, #14
     c70:	f7ff faa7 	bl	1c2 <mbus_remote_register_write>
     c74:	4ba4      	ldr	r3, [pc, #656]	; (f08 <main+0x44c>)
     c76:	260f      	movs	r6, #15
     c78:	681a      	ldr	r2, [r3, #0]
     c7a:	2005      	movs	r0, #5
     c7c:	43b2      	bics	r2, r6
     c7e:	433a      	orrs	r2, r7
     c80:	601a      	str	r2, [r3, #0]
     c82:	681a      	ldr	r2, [r3, #0]
     c84:	1c31      	adds	r1, r6, #0
     c86:	f7ff fa9c 	bl	1c2 <mbus_remote_register_write>
     c8a:	4ba0      	ldr	r3, [pc, #640]	; (f0c <main+0x450>)
     c8c:	2180      	movs	r1, #128	; 0x80
     c8e:	681a      	ldr	r2, [r3, #0]
     c90:	0109      	lsls	r1, r1, #4
     c92:	430a      	orrs	r2, r1
     c94:	601a      	str	r2, [r3, #0]
     c96:	681a      	ldr	r2, [r3, #0]
     c98:	2005      	movs	r0, #5
     c9a:	4315      	orrs	r5, r2
     c9c:	601d      	str	r5, [r3, #0]
     c9e:	6819      	ldr	r1, [r3, #0]
     ca0:	2280      	movs	r2, #128	; 0x80
     ca2:	0392      	lsls	r2, r2, #14
     ca4:	430a      	orrs	r2, r1
     ca6:	601a      	str	r2, [r3, #0]
     ca8:	681a      	ldr	r2, [r3, #0]
     caa:	4983      	ldr	r1, [pc, #524]	; (eb8 <main+0x3fc>)
     cac:	2504      	movs	r5, #4
     cae:	400a      	ands	r2, r1
     cb0:	2180      	movs	r1, #128	; 0x80
     cb2:	40b9      	lsls	r1, r7
     cb4:	430a      	orrs	r2, r1
     cb6:	601a      	str	r2, [r3, #0]
     cb8:	6819      	ldr	r1, [r3, #0]
     cba:	4a95      	ldr	r2, [pc, #596]	; (f10 <main+0x454>)
     cbc:	400a      	ands	r2, r1
     cbe:	2180      	movs	r1, #128	; 0x80
     cc0:	0309      	lsls	r1, r1, #12
     cc2:	430a      	orrs	r2, r1
     cc4:	601a      	str	r2, [r3, #0]
     cc6:	681a      	ldr	r2, [r3, #0]
     cc8:	2112      	movs	r1, #18
     cca:	f7ff fa7a 	bl	1c2 <mbus_remote_register_write>
     cce:	2005      	movs	r0, #5
     cd0:	22c0      	movs	r2, #192	; 0xc0
     cd2:	4082      	lsls	r2, r0
     cd4:	2118      	movs	r1, #24
     cd6:	f7ff fa74 	bl	1c2 <mbus_remote_register_write>
     cda:	4b8e      	ldr	r3, [pc, #568]	; (f14 <main+0x458>)
     cdc:	4a8e      	ldr	r2, [pc, #568]	; (f18 <main+0x45c>)
     cde:	6819      	ldr	r1, [r3, #0]
     ce0:	2004      	movs	r0, #4
     ce2:	400a      	ands	r2, r1
     ce4:	21bc      	movs	r1, #188	; 0xbc
     ce6:	0409      	lsls	r1, r1, #16
     ce8:	430a      	orrs	r2, r1
     cea:	601a      	str	r2, [r3, #0]
     cec:	6819      	ldr	r1, [r3, #0]
     cee:	4a8b      	ldr	r2, [pc, #556]	; (f1c <main+0x460>)
     cf0:	400a      	ands	r2, r1
     cf2:	601a      	str	r2, [r3, #0]
     cf4:	6819      	ldr	r1, [r3, #0]
     cf6:	4a8a      	ldr	r2, [pc, #552]	; (f20 <main+0x464>)
     cf8:	400a      	ands	r2, r1
     cfa:	2190      	movs	r1, #144	; 0x90
     cfc:	0149      	lsls	r1, r1, #5
     cfe:	430a      	orrs	r2, r1
     d00:	601a      	str	r2, [r3, #0]
     d02:	6819      	ldr	r1, [r3, #0]
     d04:	4a87      	ldr	r2, [pc, #540]	; (f24 <main+0x468>)
     d06:	400a      	ands	r2, r1
     d08:	21c0      	movs	r1, #192	; 0xc0
     d0a:	0289      	lsls	r1, r1, #10
     d0c:	430a      	orrs	r2, r1
     d0e:	601a      	str	r2, [r3, #0]
     d10:	681a      	ldr	r2, [r3, #0]
     d12:	1c21      	adds	r1, r4, #0
     d14:	f7ff fa55 	bl	1c2 <mbus_remote_register_write>
     d18:	4b83      	ldr	r3, [pc, #524]	; (f28 <main+0x46c>)
     d1a:	211f      	movs	r1, #31
     d1c:	681a      	ldr	r2, [r3, #0]
     d1e:	2004      	movs	r0, #4
     d20:	438a      	bics	r2, r1
     d22:	2110      	movs	r1, #16
     d24:	430a      	orrs	r2, r1
     d26:	601a      	str	r2, [r3, #0]
     d28:	6819      	ldr	r1, [r3, #0]
     d2a:	4a80      	ldr	r2, [pc, #512]	; (f2c <main+0x470>)
     d2c:	400a      	ands	r2, r1
     d2e:	21b8      	movs	r1, #184	; 0xb8
     d30:	0089      	lsls	r1, r1, #2
     d32:	430a      	orrs	r2, r1
     d34:	601a      	str	r2, [r3, #0]
     d36:	6819      	ldr	r1, [r3, #0]
     d38:	4a7d      	ldr	r2, [pc, #500]	; (f30 <main+0x474>)
     d3a:	400a      	ands	r2, r1
     d3c:	21a0      	movs	r1, #160	; 0xa0
     d3e:	0249      	lsls	r1, r1, #9
     d40:	430a      	orrs	r2, r1
     d42:	601a      	str	r2, [r3, #0]
     d44:	681a      	ldr	r2, [r3, #0]
     d46:	210b      	movs	r1, #11
     d48:	f7ff fa3b 	bl	1c2 <mbus_remote_register_write>
     d4c:	4b79      	ldr	r3, [pc, #484]	; (f34 <main+0x478>)
     d4e:	22c0      	movs	r2, #192	; 0xc0
     d50:	6819      	ldr	r1, [r3, #0]
     d52:	0352      	lsls	r2, r2, #13
     d54:	430a      	orrs	r2, r1
     d56:	601a      	str	r2, [r3, #0]
     d58:	6819      	ldr	r1, [r3, #0]
     d5a:	4a77      	ldr	r2, [pc, #476]	; (f38 <main+0x47c>)
     d5c:	2004      	movs	r0, #4
     d5e:	400a      	ands	r2, r1
     d60:	2180      	movs	r1, #128	; 0x80
     d62:	0109      	lsls	r1, r1, #4
     d64:	430a      	orrs	r2, r1
     d66:	601a      	str	r2, [r3, #0]
     d68:	681a      	ldr	r2, [r3, #0]
     d6a:	217f      	movs	r1, #127	; 0x7f
     d6c:	438a      	bics	r2, r1
     d6e:	2160      	movs	r1, #96	; 0x60
     d70:	430a      	orrs	r2, r1
     d72:	601a      	str	r2, [r3, #0]
     d74:	681a      	ldr	r2, [r3, #0]
     d76:	2101      	movs	r1, #1
     d78:	f7ff fa23 	bl	1c2 <mbus_remote_register_write>
     d7c:	4b6f      	ldr	r3, [pc, #444]	; (f3c <main+0x480>)
     d7e:	213f      	movs	r1, #63	; 0x3f
     d80:	781a      	ldrb	r2, [r3, #0]
     d82:	1c28      	adds	r0, r5, #0
     d84:	438a      	bics	r2, r1
     d86:	432a      	orrs	r2, r5
     d88:	701a      	strb	r2, [r3, #0]
     d8a:	681a      	ldr	r2, [r3, #0]
     d8c:	210c      	movs	r1, #12
     d8e:	f7ff fa18 	bl	1c2 <mbus_remote_register_write>
     d92:	22c8      	movs	r2, #200	; 0xc8
     d94:	1c28      	adds	r0, r5, #0
     d96:	1c31      	adds	r1, r6, #0
     d98:	0152      	lsls	r2, r2, #5
     d9a:	f7ff fa12 	bl	1c2 <mbus_remote_register_write>
     d9e:	4b68      	ldr	r3, [pc, #416]	; (f40 <main+0x484>)
     da0:	4a68      	ldr	r2, [pc, #416]	; (f44 <main+0x488>)
     da2:	213a      	movs	r1, #58	; 0x3a
     da4:	601a      	str	r2, [r3, #0]
     da6:	4b68      	ldr	r3, [pc, #416]	; (f48 <main+0x48c>)
     da8:	2203      	movs	r2, #3
     daa:	601a      	str	r2, [r3, #0]
     dac:	4b67      	ldr	r3, [pc, #412]	; (f4c <main+0x490>)
     dae:	4a68      	ldr	r2, [pc, #416]	; (f50 <main+0x494>)
     db0:	601c      	str	r4, [r3, #0]
     db2:	4b68      	ldr	r3, [pc, #416]	; (f54 <main+0x498>)
     db4:	1c38      	adds	r0, r7, #0
     db6:	601c      	str	r4, [r3, #0]
     db8:	4b67      	ldr	r3, [pc, #412]	; (f58 <main+0x49c>)
     dba:	601c      	str	r4, [r3, #0]
     dbc:	4b67      	ldr	r3, [pc, #412]	; (f5c <main+0x4a0>)
     dbe:	701c      	strb	r4, [r3, #0]
     dc0:	4b67      	ldr	r3, [pc, #412]	; (f60 <main+0x4a4>)
     dc2:	701c      	strb	r4, [r3, #0]
     dc4:	4b67      	ldr	r3, [pc, #412]	; (f64 <main+0x4a8>)
     dc6:	701c      	strb	r4, [r3, #0]
     dc8:	4b67      	ldr	r3, [pc, #412]	; (f68 <main+0x4ac>)
     dca:	601c      	str	r4, [r3, #0]
     dcc:	f7ff f9f9 	bl	1c2 <mbus_remote_register_write>
     dd0:	2064      	movs	r0, #100	; 0x64
     dd2:	f7ff f960 	bl	96 <delay>
     dd6:	f7ff fa13 	bl	200 <batadc_resetrelease>
     dda:	4b64      	ldr	r3, [pc, #400]	; (f6c <main+0x4b0>)
     ddc:	220b      	movs	r2, #11
     dde:	601a      	str	r2, [r3, #0]
     de0:	e03c      	b.n	e5c <main+0x3a0>
     de2:	2278      	movs	r2, #120	; 0x78
     de4:	6815      	ldr	r5, [r2, #0]
     de6:	0c2e      	lsrs	r6, r5, #16
     de8:	0e2c      	lsrs	r4, r5, #24
     dea:	0a2b      	lsrs	r3, r5, #8
     dec:	b2f7      	uxtb	r7, r6
     dee:	2c01      	cmp	r4, #1
     df0:	d136      	bne.n	e60 <main+0x3a4>
     df2:	22ff      	movs	r2, #255	; 0xff
     df4:	4f54      	ldr	r7, [pc, #336]	; (f48 <main+0x48c>)
     df6:	4013      	ands	r3, r2
     df8:	603b      	str	r3, [r7, #0]
     dfa:	4b34      	ldr	r3, [pc, #208]	; (ecc <main+0x410>)
     dfc:	4016      	ands	r6, r2
     dfe:	4015      	ands	r5, r2
     e00:	0236      	lsls	r6, r6, #8
     e02:	6819      	ldr	r1, [r3, #0]
     e04:	19ad      	adds	r5, r5, r6
     e06:	42a9      	cmp	r1, r5
     e08:	d222      	bcs.n	e50 <main+0x394>
     e0a:	681a      	ldr	r2, [r3, #0]
     e0c:	3201      	adds	r2, #1
     e0e:	601a      	str	r2, [r3, #0]
     e10:	681d      	ldr	r5, [r3, #0]
     e12:	2d01      	cmp	r5, #1
     e14:	d109      	bne.n	e2a <main+0x36e>
     e16:	f7ff f9ff 	bl	218 <radio_power_on>
     e1a:	2002      	movs	r0, #2
     e1c:	1c29      	adds	r1, r5, #0
     e1e:	1c2a      	adds	r2, r5, #0
     e20:	f7ff f94e 	bl	c0 <set_wakeup_timer>
     e24:	f7ff f9b6 	bl	194 <mbus_sleep_all>
     e28:	e7fe      	b.n	e28 <main+0x36c>
     e2a:	6819      	ldr	r1, [r3, #0]
     e2c:	4a50      	ldr	r2, [pc, #320]	; (f70 <main+0x4b4>)
     e2e:	2000      	movs	r0, #0
     e30:	1889      	adds	r1, r1, r2
     e32:	f7ff fb13 	bl	45c <send_radio_data_ppm>
     e36:	20fa      	movs	r0, #250	; 0xfa
     e38:	0100      	lsls	r0, r0, #4
     e3a:	f7ff f92c 	bl	96 <delay>
     e3e:	6838      	ldr	r0, [r7, #0]
     e40:	1c21      	adds	r1, r4, #0
     e42:	b280      	uxth	r0, r0
     e44:	1c22      	adds	r2, r4, #0
     e46:	f7ff f93b 	bl	c0 <set_wakeup_timer>
     e4a:	f7ff f9a3 	bl	194 <mbus_sleep_all>
     e4e:	e7fe      	b.n	e4e <main+0x392>
     e50:	2200      	movs	r2, #0
     e52:	601a      	str	r2, [r3, #0]
     e54:	1c20      	adds	r0, r4, #0
     e56:	4947      	ldr	r1, [pc, #284]	; (f74 <main+0x4b8>)
     e58:	f7ff fb00 	bl	45c <send_radio_data_ppm>
     e5c:	f7ff fae8 	bl	430 <operation_sleep_notimer>
     e60:	2c02      	cmp	r4, #2
     e62:	d000      	beq.n	e66 <main+0x3aa>
     e64:	e095      	b.n	f92 <main+0x4d6>
     e66:	21ff      	movs	r1, #255	; 0xff
     e68:	400b      	ands	r3, r1
     e6a:	400d      	ands	r5, r1
     e6c:	021b      	lsls	r3, r3, #8
     e6e:	18ed      	adds	r5, r5, r3
     e70:	4833      	ldr	r0, [pc, #204]	; (f40 <main+0x484>)
     e72:	4b35      	ldr	r3, [pc, #212]	; (f48 <main+0x48c>)
     e74:	210f      	movs	r1, #15
     e76:	4031      	ands	r1, r6
     e78:	6005      	str	r5, [r0, #0]
     e7a:	6019      	str	r1, [r3, #0]
     e7c:	2310      	movs	r3, #16
     e7e:	401e      	ands	r6, r3
     e80:	4b35      	ldr	r3, [pc, #212]	; (f58 <main+0x49c>)
     e82:	4c36      	ldr	r4, [pc, #216]	; (f5c <main+0x4a0>)
     e84:	601e      	str	r6, [r3, #0]
     e86:	7823      	ldrb	r3, [r4, #0]
     e88:	2b00      	cmp	r3, #0
     e8a:	d175      	bne.n	f78 <main+0x4bc>
     e8c:	2101      	movs	r1, #1
     e8e:	2005      	movs	r0, #5
     e90:	1c0a      	adds	r2, r1, #0
     e92:	f7ff f915 	bl	c0 <set_wakeup_timer>
     e96:	2301      	movs	r3, #1
     e98:	7023      	strb	r3, [r4, #0]
     e9a:	4b33      	ldr	r3, [pc, #204]	; (f68 <main+0x4ac>)
     e9c:	097f      	lsrs	r7, r7, #5
     e9e:	601f      	str	r7, [r3, #0]
     ea0:	f7ff f978 	bl	194 <mbus_sleep_all>
     ea4:	e7fe      	b.n	ea4 <main+0x3e8>
     ea6:	46c0      	nop			; (mov r8, r8)
     ea8:	003fffff 	.word	0x003fffff
     eac:	00001314 	.word	0x00001314
     eb0:	deadbeef 	.word	0xdeadbeef
     eb4:	000012e8 	.word	0x000012e8
     eb8:	ffff9fff 	.word	0xffff9fff
     ebc:	fffe7fff 	.word	0xfffe7fff
     ec0:	a000002c 	.word	0xa000002c
     ec4:	00001324 	.word	0x00001324
     ec8:	00001310 	.word	0x00001310
     ecc:	00001344 	.word	0x00001344
     ed0:	0000132c 	.word	0x0000132c
     ed4:	0000133c 	.word	0x0000133c
     ed8:	00001350 	.word	0x00001350
     edc:	0000e204 	.word	0x0000e204
     ee0:	0000e24a 	.word	0x0000e24a
     ee4:	00002203 	.word	0x00002203
     ee8:	00003088 	.word	0x00003088
     eec:	0000e203 	.word	0x0000e203
     ef0:	0000f10f 	.word	0x0000f10f
     ef4:	00ffffff 	.word	0x00ffffff
     ef8:	000001d3 	.word	0x000001d3
     efc:	00001300 	.word	0x00001300
     f00:	000012f4 	.word	0x000012f4
     f04:	fff1ffff 	.word	0xfff1ffff
     f08:	000012f8 	.word	0x000012f8
     f0c:	000012fc 	.word	0x000012fc
     f10:	ffe07fff 	.word	0xffe07fff
     f14:	000012d8 	.word	0x000012d8
     f18:	ff03ffff 	.word	0xff03ffff
     f1c:	fffffe1f 	.word	0xfffffe1f
     f20:	ffffe1ff 	.word	0xffffe1ff
     f24:	fffc7fff 	.word	0xfffc7fff
     f28:	000012ec 	.word	0x000012ec
     f2c:	ffffe01f 	.word	0xffffe01f
     f30:	fff01fff 	.word	0xfff01fff
     f34:	000012e0 	.word	0x000012e0
     f38:	ffff03ff 	.word	0xffff03ff
     f3c:	000012f0 	.word	0x000012f0
     f40:	00001348 	.word	0x00001348
     f44:	000083d6 	.word	0x000083d6
     f48:	00001328 	.word	0x00001328
     f4c:	0000134c 	.word	0x0000134c
     f50:	00080800 	.word	0x00080800
     f54:	00001308 	.word	0x00001308
     f58:	00001318 	.word	0x00001318
     f5c:	0000130c 	.word	0x0000130c
     f60:	00001304 	.word	0x00001304
     f64:	00001354 	.word	0x00001354
     f68:	00001334 	.word	0x00001334
     f6c:	0000131c 	.word	0x0000131c
     f70:	00abc000 	.word	0x00abc000
     f74:	00faf000 	.word	0x00faf000
     f78:	49c0      	ldr	r1, [pc, #768]	; (127c <main+0x7c0>)
     f7a:	2300      	movs	r3, #0
     f7c:	600b      	str	r3, [r1, #0]
     f7e:	49c0      	ldr	r1, [pc, #768]	; (1280 <main+0x7c4>)
     f80:	600b      	str	r3, [r1, #0]
     f82:	49c0      	ldr	r1, [pc, #768]	; (1284 <main+0x7c8>)
     f84:	600b      	str	r3, [r1, #0]
     f86:	49c0      	ldr	r1, [pc, #768]	; (1288 <main+0x7cc>)
     f88:	600b      	str	r3, [r1, #0]
     f8a:	6013      	str	r3, [r2, #0]
     f8c:	4abf      	ldr	r2, [pc, #764]	; (128c <main+0x7d0>)
     f8e:	6013      	str	r3, [r2, #0]
     f90:	e170      	b.n	1274 <main+0x7b8>
     f92:	2c03      	cmp	r4, #3
     f94:	d141      	bne.n	101a <main+0x55e>
     f96:	22ff      	movs	r2, #255	; 0xff
     f98:	4ebd      	ldr	r6, [pc, #756]	; (1290 <main+0x7d4>)
     f9a:	4013      	ands	r3, r2
     f9c:	6033      	str	r3, [r6, #0]
     f9e:	4bbd      	ldr	r3, [pc, #756]	; (1294 <main+0x7d8>)
     fa0:	2400      	movs	r4, #0
     fa2:	701c      	strb	r4, [r3, #0]
     fa4:	4bbc      	ldr	r3, [pc, #752]	; (1298 <main+0x7dc>)
     fa6:	4015      	ands	r5, r2
     fa8:	701c      	strb	r4, [r3, #0]
     faa:	4bbc      	ldr	r3, [pc, #752]	; (129c <main+0x7e0>)
     fac:	6819      	ldr	r1, [r3, #0]
     fae:	42a9      	cmp	r1, r5
     fb0:	d231      	bcs.n	1016 <main+0x55a>
     fb2:	681a      	ldr	r2, [r3, #0]
     fb4:	3201      	adds	r2, #1
     fb6:	601a      	str	r2, [r3, #0]
     fb8:	681d      	ldr	r5, [r3, #0]
     fba:	2d01      	cmp	r5, #1
     fbc:	d109      	bne.n	fd2 <main+0x516>
     fbe:	f7ff f92b 	bl	218 <radio_power_on>
     fc2:	2002      	movs	r0, #2
     fc4:	1c29      	adds	r1, r5, #0
     fc6:	1c2a      	adds	r2, r5, #0
     fc8:	f7ff f87a 	bl	c0 <set_wakeup_timer>
     fcc:	f7ff f8e2 	bl	194 <mbus_sleep_all>
     fd0:	e7fe      	b.n	fd0 <main+0x514>
     fd2:	4baa      	ldr	r3, [pc, #680]	; (127c <main+0x7c0>)
     fd4:	1c20      	adds	r0, r4, #0
     fd6:	6819      	ldr	r1, [r3, #0]
     fd8:	23c1      	movs	r3, #193	; 0xc1
     fda:	03db      	lsls	r3, r3, #15
     fdc:	18c9      	adds	r1, r1, r3
     fde:	0049      	lsls	r1, r1, #1
     fe0:	f7ff fa3c 	bl	45c <send_radio_data_ppm>
     fe4:	20fa      	movs	r0, #250	; 0xfa
     fe6:	0100      	lsls	r0, r0, #4
     fe8:	f7ff f855 	bl	96 <delay>
     fec:	4bac      	ldr	r3, [pc, #688]	; (12a0 <main+0x7e4>)
     fee:	22c2      	movs	r2, #194	; 0xc2
     ff0:	6819      	ldr	r1, [r3, #0]
     ff2:	0412      	lsls	r2, r2, #16
     ff4:	1889      	adds	r1, r1, r2
     ff6:	1c20      	adds	r0, r4, #0
     ff8:	f7ff fa30 	bl	45c <send_radio_data_ppm>
     ffc:	20fa      	movs	r0, #250	; 0xfa
     ffe:	0100      	lsls	r0, r0, #4
    1000:	f7ff f849 	bl	96 <delay>
    1004:	6830      	ldr	r0, [r6, #0]
    1006:	2101      	movs	r1, #1
    1008:	b280      	uxth	r0, r0
    100a:	1c0a      	adds	r2, r1, #0
    100c:	f7ff f858 	bl	c0 <set_wakeup_timer>
    1010:	f7ff f8c0 	bl	194 <mbus_sleep_all>
    1014:	e7fe      	b.n	1014 <main+0x558>
    1016:	601c      	str	r4, [r3, #0]
    1018:	e12a      	b.n	1270 <main+0x7b4>
    101a:	2c04      	cmp	r4, #4
    101c:	d000      	beq.n	1020 <main+0x564>
    101e:	e087      	b.n	1130 <main+0x674>
    1020:	22ff      	movs	r2, #255	; 0xff
    1022:	499b      	ldr	r1, [pc, #620]	; (1290 <main+0x7d4>)
    1024:	4013      	ands	r3, r2
    1026:	600b      	str	r3, [r1, #0]
    1028:	4b9e      	ldr	r3, [pc, #632]	; (12a4 <main+0x7e8>)
    102a:	4015      	ands	r5, r2
    102c:	4a95      	ldr	r2, [pc, #596]	; (1284 <main+0x7c8>)
    102e:	601d      	str	r5, [r3, #0]
    1030:	6819      	ldr	r1, [r3, #0]
    1032:	6812      	ldr	r2, [r2, #0]
    1034:	4291      	cmp	r1, r2
    1036:	d301      	bcc.n	103c <main+0x580>
    1038:	2200      	movs	r2, #0
    103a:	601a      	str	r2, [r3, #0]
    103c:	4b97      	ldr	r3, [pc, #604]	; (129c <main+0x7e0>)
    103e:	681a      	ldr	r2, [r3, #0]
    1040:	2a04      	cmp	r2, #4
    1042:	d849      	bhi.n	10d8 <main+0x61c>
    1044:	681a      	ldr	r2, [r3, #0]
    1046:	3201      	adds	r2, #1
    1048:	601a      	str	r2, [r3, #0]
    104a:	681c      	ldr	r4, [r3, #0]
    104c:	2c01      	cmp	r4, #1
    104e:	d109      	bne.n	1064 <main+0x5a8>
    1050:	f7ff f8e2 	bl	218 <radio_power_on>
    1054:	2002      	movs	r0, #2
    1056:	1c21      	adds	r1, r4, #0
    1058:	1c22      	adds	r2, r4, #0
    105a:	f7ff f831 	bl	c0 <set_wakeup_timer>
    105e:	f7ff f899 	bl	194 <mbus_sleep_all>
    1062:	e7fe      	b.n	1062 <main+0x5a6>
    1064:	6819      	ldr	r1, [r3, #0]
    1066:	4b90      	ldr	r3, [pc, #576]	; (12a8 <main+0x7ec>)
    1068:	2000      	movs	r0, #0
    106a:	18c9      	adds	r1, r1, r3
    106c:	f7ff f9f6 	bl	45c <send_radio_data_ppm>
    1070:	4b87      	ldr	r3, [pc, #540]	; (1290 <main+0x7d4>)
    1072:	2101      	movs	r1, #1
    1074:	6818      	ldr	r0, [r3, #0]
    1076:	1c0a      	adds	r2, r1, #0
    1078:	b280      	uxth	r0, r0
    107a:	f7ff f821 	bl	c0 <set_wakeup_timer>
    107e:	f7ff f889 	bl	194 <mbus_sleep_all>
    1082:	e7fe      	b.n	1082 <main+0x5c6>
    1084:	4889      	ldr	r0, [pc, #548]	; (12ac <main+0x7f0>)
    1086:	f7ff f80f 	bl	a8 <config_timerwd>
    108a:	6821      	ldr	r1, [r4, #0]
    108c:	20dd      	movs	r0, #221	; 0xdd
    108e:	f7ff f857 	bl	140 <mbus_write_message32>
    1092:	2064      	movs	r0, #100	; 0x64
    1094:	f7fe ffff 	bl	96 <delay>
    1098:	4a85      	ldr	r2, [pc, #532]	; (12b0 <main+0x7f4>)
    109a:	6823      	ldr	r3, [r4, #0]
    109c:	20dd      	movs	r0, #221	; 0xdd
    109e:	5cd1      	ldrb	r1, [r2, r3]
    10a0:	f7ff f84e 	bl	140 <mbus_write_message32>
    10a4:	2064      	movs	r0, #100	; 0x64
    10a6:	f7fe fff6 	bl	96 <delay>
    10aa:	6823      	ldr	r3, [r4, #0]
    10ac:	4d81      	ldr	r5, [pc, #516]	; (12b4 <main+0x7f8>)
    10ae:	005b      	lsls	r3, r3, #1
    10b0:	5b59      	ldrh	r1, [r3, r5]
    10b2:	20dd      	movs	r0, #221	; 0xdd
    10b4:	f7ff f844 	bl	140 <mbus_write_message32>
    10b8:	2064      	movs	r0, #100	; 0x64
    10ba:	f7fe ffec 	bl	96 <delay>
    10be:	6823      	ldr	r3, [r4, #0]
    10c0:	2000      	movs	r0, #0
    10c2:	005b      	lsls	r3, r3, #1
    10c4:	5b59      	ldrh	r1, [r3, r5]
    10c6:	f7ff f9c9 	bl	45c <send_radio_data_ppm>
    10ca:	20fa      	movs	r0, #250	; 0xfa
    10cc:	0100      	lsls	r0, r0, #4
    10ce:	f7fe ffe2 	bl	96 <delay>
    10d2:	6823      	ldr	r3, [r4, #0]
    10d4:	3b01      	subs	r3, #1
    10d6:	6023      	str	r3, [r4, #0]
    10d8:	4b72      	ldr	r3, [pc, #456]	; (12a4 <main+0x7e8>)
    10da:	2500      	movs	r5, #0
    10dc:	681a      	ldr	r2, [r3, #0]
    10de:	496a      	ldr	r1, [pc, #424]	; (1288 <main+0x7cc>)
    10e0:	42aa      	cmp	r2, r5
    10e2:	d102      	bne.n	10ea <main+0x62e>
    10e4:	680d      	ldr	r5, [r1, #0]
    10e6:	1e6a      	subs	r2, r5, #1
    10e8:	4195      	sbcs	r5, r2
    10ea:	6818      	ldr	r0, [r3, #0]
    10ec:	2200      	movs	r2, #0
    10ee:	4290      	cmp	r0, r2
    10f0:	d007      	beq.n	1102 <main+0x646>
    10f2:	681a      	ldr	r2, [r3, #0]
    10f4:	4b63      	ldr	r3, [pc, #396]	; (1284 <main+0x7c8>)
    10f6:	6809      	ldr	r1, [r1, #0]
    10f8:	681b      	ldr	r3, [r3, #0]
    10fa:	188a      	adds	r2, r1, r2
    10fc:	4293      	cmp	r3, r2
    10fe:	4192      	sbcs	r2, r2
    1100:	4252      	negs	r2, r2
    1102:	4315      	orrs	r5, r2
    1104:	4c60      	ldr	r4, [pc, #384]	; (1288 <main+0x7cc>)
    1106:	d1bd      	bne.n	1084 <main+0x5c8>
    1108:	6822      	ldr	r2, [r4, #0]
    110a:	4b6a      	ldr	r3, [pc, #424]	; (12b4 <main+0x7f8>)
    110c:	0052      	lsls	r2, r2, #1
    110e:	5ad1      	ldrh	r1, [r2, r3]
    1110:	1c28      	adds	r0, r5, #0
    1112:	f7ff f9a3 	bl	45c <send_radio_data_ppm>
    1116:	4868      	ldr	r0, [pc, #416]	; (12b8 <main+0x7fc>)
    1118:	f7fe ffbd 	bl	96 <delay>
    111c:	2001      	movs	r0, #1
    111e:	4967      	ldr	r1, [pc, #412]	; (12bc <main+0x800>)
    1120:	f7ff f99c 	bl	45c <send_radio_data_ppm>
    1124:	4b5d      	ldr	r3, [pc, #372]	; (129c <main+0x7e0>)
    1126:	601d      	str	r5, [r3, #0]
    1128:	4b56      	ldr	r3, [pc, #344]	; (1284 <main+0x7c8>)
    112a:	681b      	ldr	r3, [r3, #0]
    112c:	6023      	str	r3, [r4, #0]
    112e:	e695      	b.n	e5c <main+0x3a0>
    1130:	2c07      	cmp	r4, #7
    1132:	d151      	bne.n	11d8 <main+0x71c>
    1134:	27ff      	movs	r7, #255	; 0xff
    1136:	4956      	ldr	r1, [pc, #344]	; (1290 <main+0x7d4>)
    1138:	403b      	ands	r3, r7
    113a:	600b      	str	r3, [r1, #0]
    113c:	4b60      	ldr	r3, [pc, #384]	; (12c0 <main+0x804>)
    113e:	403e      	ands	r6, r7
    1140:	601e      	str	r6, [r3, #0]
    1142:	4b4e      	ldr	r3, [pc, #312]	; (127c <main+0x7c0>)
    1144:	2200      	movs	r2, #0
    1146:	601a      	str	r2, [r3, #0]
    1148:	4b54      	ldr	r3, [pc, #336]	; (129c <main+0x7e0>)
    114a:	403d      	ands	r5, r7
    114c:	6819      	ldr	r1, [r3, #0]
    114e:	42a9      	cmp	r1, r5
    1150:	d23a      	bcs.n	11c8 <main+0x70c>
    1152:	6819      	ldr	r1, [r3, #0]
    1154:	3101      	adds	r1, #1
    1156:	6019      	str	r1, [r3, #0]
    1158:	681d      	ldr	r5, [r3, #0]
    115a:	2d01      	cmp	r5, #1
    115c:	d11f      	bne.n	119e <main+0x6e2>
    115e:	1c11      	adds	r1, r2, #0
    1160:	1c20      	adds	r0, r4, #0
    1162:	2203      	movs	r2, #3
    1164:	f7ff f82d 	bl	1c2 <mbus_remote_register_write>
    1168:	2064      	movs	r0, #100	; 0x64
    116a:	f7fe ff94 	bl	96 <delay>
    116e:	2064      	movs	r0, #100	; 0x64
    1170:	f7fe ff91 	bl	96 <delay>
    1174:	23a0      	movs	r3, #160	; 0xa0
    1176:	061b      	lsls	r3, r3, #24
    1178:	681b      	ldr	r3, [r3, #0]
    117a:	401f      	ands	r7, r3
    117c:	4b51      	ldr	r3, [pc, #324]	; (12c4 <main+0x808>)
    117e:	601f      	str	r7, [r3, #0]
    1180:	f7ff f832 	bl	1e8 <batadc_reset>
    1184:	2064      	movs	r0, #100	; 0x64
    1186:	f7fe ff86 	bl	96 <delay>
    118a:	f7ff f845 	bl	218 <radio_power_on>
    118e:	2002      	movs	r0, #2
    1190:	1c29      	adds	r1, r5, #0
    1192:	1c2a      	adds	r2, r5, #0
    1194:	f7fe ff94 	bl	c0 <set_wakeup_timer>
    1198:	f7fe fffc 	bl	194 <mbus_sleep_all>
    119c:	e7fe      	b.n	119c <main+0x6e0>
    119e:	4b49      	ldr	r3, [pc, #292]	; (12c4 <main+0x808>)
    11a0:	1c10      	adds	r0, r2, #0
    11a2:	6819      	ldr	r1, [r3, #0]
    11a4:	4b48      	ldr	r3, [pc, #288]	; (12c8 <main+0x80c>)
    11a6:	18c9      	adds	r1, r1, r3
    11a8:	f7ff f958 	bl	45c <send_radio_data_ppm>
    11ac:	20fa      	movs	r0, #250	; 0xfa
    11ae:	0100      	lsls	r0, r0, #4
    11b0:	f7fe ff71 	bl	96 <delay>
    11b4:	4c36      	ldr	r4, [pc, #216]	; (1290 <main+0x7d4>)
    11b6:	2101      	movs	r1, #1
    11b8:	6820      	ldr	r0, [r4, #0]
    11ba:	1c0a      	adds	r2, r1, #0
    11bc:	b280      	uxth	r0, r0
    11be:	f7fe ff7f 	bl	c0 <set_wakeup_timer>
    11c2:	f7fe ffe7 	bl	194 <mbus_sleep_all>
    11c6:	e7fe      	b.n	11c6 <main+0x70a>
    11c8:	2001      	movs	r0, #1
    11ca:	493c      	ldr	r1, [pc, #240]	; (12bc <main+0x800>)
    11cc:	601a      	str	r2, [r3, #0]
    11ce:	f7ff f945 	bl	45c <send_radio_data_ppm>
    11d2:	f7ff f815 	bl	200 <batadc_resetrelease>
    11d6:	e641      	b.n	e5c <main+0x3a0>
    11d8:	2c13      	cmp	r4, #19
    11da:	d14b      	bne.n	1274 <main+0x7b8>
    11dc:	21ff      	movs	r1, #255	; 0xff
    11de:	4c2c      	ldr	r4, [pc, #176]	; (1290 <main+0x7d4>)
    11e0:	400b      	ands	r3, r1
    11e2:	2064      	movs	r0, #100	; 0x64
    11e4:	6023      	str	r3, [r4, #0]
    11e6:	f7fe ff56 	bl	96 <delay>
    11ea:	4b38      	ldr	r3, [pc, #224]	; (12cc <main+0x810>)
    11ec:	093f      	lsrs	r7, r7, #4
    11ee:	017a      	lsls	r2, r7, #5
    11f0:	4937      	ldr	r1, [pc, #220]	; (12d0 <main+0x814>)
    11f2:	681f      	ldr	r7, [r3, #0]
    11f4:	2004      	movs	r0, #4
    11f6:	400f      	ands	r7, r1
    11f8:	4317      	orrs	r7, r2
    11fa:	220f      	movs	r2, #15
    11fc:	601f      	str	r7, [r3, #0]
    11fe:	4016      	ands	r6, r2
    1200:	0272      	lsls	r2, r6, #9
    1202:	4934      	ldr	r1, [pc, #208]	; (12d4 <main+0x818>)
    1204:	681e      	ldr	r6, [r3, #0]
    1206:	400e      	ands	r6, r1
    1208:	4316      	orrs	r6, r2
    120a:	601e      	str	r6, [r3, #0]
    120c:	681a      	ldr	r2, [r3, #0]
    120e:	2100      	movs	r1, #0
    1210:	f7fe ffd7 	bl	1c2 <mbus_remote_register_write>
    1214:	2064      	movs	r0, #100	; 0x64
    1216:	f7fe ff3e 	bl	96 <delay>
    121a:	4b20      	ldr	r3, [pc, #128]	; (129c <main+0x7e0>)
    121c:	21ff      	movs	r1, #255	; 0xff
    121e:	681a      	ldr	r2, [r3, #0]
    1220:	400d      	ands	r5, r1
    1222:	42aa      	cmp	r2, r5
    1224:	d222      	bcs.n	126c <main+0x7b0>
    1226:	681a      	ldr	r2, [r3, #0]
    1228:	3201      	adds	r2, #1
    122a:	601a      	str	r2, [r3, #0]
    122c:	681d      	ldr	r5, [r3, #0]
    122e:	2d01      	cmp	r5, #1
    1230:	d109      	bne.n	1246 <main+0x78a>
    1232:	f7fe fff1 	bl	218 <radio_power_on>
    1236:	2002      	movs	r0, #2
    1238:	1c29      	adds	r1, r5, #0
    123a:	1c2a      	adds	r2, r5, #0
    123c:	f7fe ff40 	bl	c0 <set_wakeup_timer>
    1240:	f7fe ffa8 	bl	194 <mbus_sleep_all>
    1244:	e7fe      	b.n	1244 <main+0x788>
    1246:	6819      	ldr	r1, [r3, #0]
    1248:	4a17      	ldr	r2, [pc, #92]	; (12a8 <main+0x7ec>)
    124a:	2000      	movs	r0, #0
    124c:	1889      	adds	r1, r1, r2
    124e:	f7ff f905 	bl	45c <send_radio_data_ppm>
    1252:	20fa      	movs	r0, #250	; 0xfa
    1254:	0100      	lsls	r0, r0, #4
    1256:	f7fe ff1e 	bl	96 <delay>
    125a:	6820      	ldr	r0, [r4, #0]
    125c:	2101      	movs	r1, #1
    125e:	b280      	uxth	r0, r0
    1260:	1c0a      	adds	r2, r1, #0
    1262:	f7fe ff2d 	bl	c0 <set_wakeup_timer>
    1266:	f7fe ff95 	bl	194 <mbus_sleep_all>
    126a:	e7fe      	b.n	126a <main+0x7ae>
    126c:	2200      	movs	r2, #0
    126e:	601a      	str	r2, [r3, #0]
    1270:	2001      	movs	r0, #1
    1272:	e5f0      	b.n	e56 <main+0x39a>
    1274:	f7ff f954 	bl	520 <operation_run>
    1278:	e7fc      	b.n	1274 <main+0x7b8>
    127a:	46c0      	nop			; (mov r8, r8)
    127c:	00001310 	.word	0x00001310
    1280:	00001338 	.word	0x00001338
    1284:	0000134c 	.word	0x0000134c
    1288:	00001308 	.word	0x00001308
    128c:	00001340 	.word	0x00001340
    1290:	00001328 	.word	0x00001328
    1294:	0000130c 	.word	0x0000130c
    1298:	00001324 	.word	0x00001324
    129c:	00001344 	.word	0x00001344
    12a0:	00001350 	.word	0x00001350
    12a4:	00001330 	.word	0x00001330
    12a8:	00abc000 	.word	0x00abc000
    12ac:	000fffff 	.word	0x000fffff
    12b0:	00001355 	.word	0x00001355
    12b4:	00001770 	.word	0x00001770
    12b8:	00002ee0 	.word	0x00002ee0
    12bc:	00faf000 	.word	0x00faf000
    12c0:	0000131c 	.word	0x0000131c
    12c4:	00001320 	.word	0x00001320
    12c8:	00bbb000 	.word	0x00bbb000
    12cc:	000012d8 	.word	0x000012d8
    12d0:	fffffe1f 	.word	0xfffffe1f
    12d4:	ffffe1ff 	.word	0xffffe1ff
