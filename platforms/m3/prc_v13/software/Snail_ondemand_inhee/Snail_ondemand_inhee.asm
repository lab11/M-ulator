
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
  40:	00000961 	.word	0x00000961
  44:	0000096d 	.word	0x0000096d
  48:	00000979 	.word	0x00000979
  4c:	00000991 	.word	0x00000991
  50:	000009a9 	.word	0x000009a9
  54:	000009c1 	.word	0x000009c1
  58:	000009d9 	.word	0x000009d9
  5c:	000009f1 	.word	0x000009f1
  60:	00000a09 	.word	0x00000a09
  64:	00000a21 	.word	0x00000a21
  68:	00000a39 	.word	0x00000a39
  6c:	00000a49 	.word	0x00000a49
  70:	00000a59 	.word	0x00000a59
  74:	00000a69 	.word	0x00000a69
  78:	00000a79 	.word	0x00000a79
  7c:	00000000 	.word	0x00000000

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 fcfa 	bl	a88 <main>
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
 2c4:	00001308 	.word	0x00001308
 2c8:	00001290 	.word	0x00001290
 2cc:	00001298 	.word	0x00001298
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
 364:	00001308 	.word	0x00001308
 368:	00001298 	.word	0x00001298
 36c:	fff7ffff 	.word	0xfff7ffff
 370:	00001290 	.word	0x00001290

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
 3a8:	000012b0 	.word	0x000012b0

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
 3c8:	000012a8 	.word	0x000012a8

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
 3f0:	000012a8 	.word	0x000012a8
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
 428:	000012a8 	.word	0x000012a8
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
 458:	00001308 	.word	0x00001308

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
 50c:	00001f8c 	.word	0x00001f8c
 510:	000012b8 	.word	0x000012b8
 514:	00001290 	.word	0x00001290
 518:	000012e0 	.word	0x000012e0
 51c:	fafafafa 	.word	0xfafafafa

Disassembly of section .text.operation_run:

00000520 <operation_run>:
 520:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
 522:	4bd7      	ldr	r3, [pc, #860]	; (880 <operation_run+0x360>)
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
 53e:	4bd1      	ldr	r3, [pc, #836]	; (884 <operation_run+0x364>)
 540:	2064      	movs	r0, #100	; 0x64
 542:	681a      	ldr	r2, [r3, #0]
 544:	4bd0      	ldr	r3, [pc, #832]	; (888 <operation_run+0x368>)
 546:	601a      	str	r2, [r3, #0]
 548:	f7ff fda5 	bl	96 <delay>
 54c:	2006      	movs	r0, #6
 54e:	1c21      	adds	r1, r4, #0
 550:	2209      	movs	r2, #9
 552:	f7ff fe36 	bl	1c2 <mbus_remote_register_write>
 556:	4bcd      	ldr	r3, [pc, #820]	; (88c <operation_run+0x36c>)
 558:	601d      	str	r5, [r3, #0]
 55a:	4bcd      	ldr	r3, [pc, #820]	; (890 <operation_run+0x370>)
 55c:	681b      	ldr	r3, [r3, #0]
 55e:	2b00      	cmp	r3, #0
 560:	d104      	bne.n	56c <operation_run+0x4c>
 562:	4bcc      	ldr	r3, [pc, #816]	; (894 <operation_run+0x374>)
 564:	681b      	ldr	r3, [r3, #0]
 566:	191b      	adds	r3, r3, r4
 568:	2b04      	cmp	r3, #4
 56a:	d801      	bhi.n	570 <operation_run+0x50>
 56c:	f7ff fe54 	bl	218 <radio_power_on>
 570:	4bc9      	ldr	r3, [pc, #804]	; (898 <operation_run+0x378>)
 572:	4aca      	ldr	r2, [pc, #808]	; (89c <operation_run+0x37c>)
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
 59e:	4bbe      	ldr	r3, [pc, #760]	; (898 <operation_run+0x378>)
 5a0:	4abf      	ldr	r2, [pc, #764]	; (8a0 <operation_run+0x380>)
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
 5cc:	4bb5      	ldr	r3, [pc, #724]	; (8a4 <operation_run+0x384>)
 5ce:	2280      	movs	r2, #128	; 0x80
 5d0:	6819      	ldr	r1, [r3, #0]
 5d2:	03d2      	lsls	r2, r2, #15
 5d4:	430a      	orrs	r2, r1
 5d6:	601a      	str	r2, [r3, #0]
 5d8:	6819      	ldr	r1, [r3, #0]
 5da:	4ab3      	ldr	r2, [pc, #716]	; (8a8 <operation_run+0x388>)
 5dc:	2005      	movs	r0, #5
 5de:	400a      	ands	r2, r1
 5e0:	601a      	str	r2, [r3, #0]
 5e2:	681a      	ldr	r2, [r3, #0]
 5e4:	210e      	movs	r1, #14
 5e6:	f7ff fdec 	bl	1c2 <mbus_remote_register_write>
 5ea:	e1b0      	b.n	94e <operation_run+0x42e>
 5ec:	781a      	ldrb	r2, [r3, #0]
 5ee:	2a03      	cmp	r2, #3
 5f0:	d12a      	bne.n	648 <operation_run+0x128>
 5f2:	4dae      	ldr	r5, [pc, #696]	; (8ac <operation_run+0x38c>)
 5f4:	2300      	movs	r3, #0
 5f6:	702b      	strb	r3, [r5, #0]
 5f8:	4baa      	ldr	r3, [pc, #680]	; (8a4 <operation_run+0x384>)
 5fa:	4aad      	ldr	r2, [pc, #692]	; (8b0 <operation_run+0x390>)
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
 616:	4a9d      	ldr	r2, [pc, #628]	; (88c <operation_run+0x36c>)
 618:	2300      	movs	r3, #0
 61a:	702b      	strb	r3, [r5, #0]
 61c:	6013      	str	r3, [r2, #0]
 61e:	e00f      	b.n	640 <operation_run+0x120>
 620:	2064      	movs	r0, #100	; 0x64
 622:	3c01      	subs	r4, #1
 624:	f7ff fd37 	bl	96 <delay>
 628:	2c00      	cmp	r4, #0
 62a:	d1f1      	bne.n	610 <operation_run+0xf0>
 62c:	49a1      	ldr	r1, [pc, #644]	; (8b4 <operation_run+0x394>)
 62e:	20aa      	movs	r0, #170	; 0xaa
 630:	f7ff fd86 	bl	140 <mbus_write_message32>
 634:	f7ff feba 	bl	3ac <temp_sensor_disable>
 638:	4b94      	ldr	r3, [pc, #592]	; (88c <operation_run+0x36c>)
 63a:	681a      	ldr	r2, [r3, #0]
 63c:	3201      	adds	r2, #1
 63e:	601a      	str	r2, [r3, #0]
 640:	4b8f      	ldr	r3, [pc, #572]	; (880 <operation_run+0x360>)
 642:	2204      	movs	r2, #4
 644:	701a      	strb	r2, [r3, #0]
 646:	e182      	b.n	94e <operation_run+0x42e>
 648:	781b      	ldrb	r3, [r3, #0]
 64a:	2b04      	cmp	r3, #4
 64c:	d000      	beq.n	650 <operation_run+0x130>
 64e:	e178      	b.n	942 <operation_run+0x422>
 650:	2100      	movs	r1, #0
 652:	2203      	movs	r2, #3
 654:	2007      	movs	r0, #7
 656:	f7ff fdb4 	bl	1c2 <mbus_remote_register_write>
 65a:	2064      	movs	r0, #100	; 0x64
 65c:	f7ff fd1b 	bl	96 <delay>
 660:	23a0      	movs	r3, #160	; 0xa0
 662:	061b      	lsls	r3, r3, #24
 664:	681a      	ldr	r2, [r3, #0]
 666:	4b94      	ldr	r3, [pc, #592]	; (8b8 <operation_run+0x398>)
 668:	b2d2      	uxtb	r2, r2
 66a:	2064      	movs	r0, #100	; 0x64
 66c:	601a      	str	r2, [r3, #0]
 66e:	4c85      	ldr	r4, [pc, #532]	; (884 <operation_run+0x364>)
 670:	f7ff fd11 	bl	96 <delay>
 674:	f7ff fdb8 	bl	1e8 <batadc_reset>
 678:	2064      	movs	r0, #100	; 0x64
 67a:	f7ff fd0c 	bl	96 <delay>
 67e:	f7ff fd43 	bl	108 <set_halt_until_mbus_rx>
 682:	2201      	movs	r2, #1
 684:	2005      	movs	r0, #5
 686:	2110      	movs	r1, #16
 688:	f7ff fda8 	bl	1dc <mbus_remote_register_read>
 68c:	6823      	ldr	r3, [r4, #0]
 68e:	2064      	movs	r0, #100	; 0x64
 690:	9301      	str	r3, [sp, #4]
 692:	f7ff fd00 	bl	96 <delay>
 696:	4b7d      	ldr	r3, [pc, #500]	; (88c <operation_run+0x36c>)
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
 6b0:	4b82      	ldr	r3, [pc, #520]	; (8bc <operation_run+0x39c>)
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
 6d8:	4b69      	ldr	r3, [pc, #420]	; (880 <operation_run+0x360>)
 6da:	2200      	movs	r2, #0
 6dc:	4c6d      	ldr	r4, [pc, #436]	; (894 <operation_run+0x374>)
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
 70a:	4f5f      	ldr	r7, [pc, #380]	; (888 <operation_run+0x368>)
 70c:	20cc      	movs	r0, #204	; 0xcc
 70e:	6839      	ldr	r1, [r7, #0]
 710:	f7ff fd16 	bl	140 <mbus_write_message32>
 714:	2064      	movs	r0, #100	; 0x64
 716:	f7ff fcbe 	bl	96 <delay>
 71a:	4e67      	ldr	r6, [pc, #412]	; (8b8 <operation_run+0x398>)
 71c:	20cc      	movs	r0, #204	; 0xcc
 71e:	6831      	ldr	r1, [r6, #0]
 720:	f7ff fd0e 	bl	140 <mbus_write_message32>
 724:	2064      	movs	r0, #100	; 0x64
 726:	f7ff fcb6 	bl	96 <delay>
 72a:	6823      	ldr	r3, [r4, #0]
 72c:	3301      	adds	r3, #1
 72e:	6023      	str	r3, [r4, #0]
 730:	4c63      	ldr	r4, [pc, #396]	; (8c0 <operation_run+0x3a0>)
 732:	4b64      	ldr	r3, [pc, #400]	; (8c4 <operation_run+0x3a4>)
 734:	6822      	ldr	r2, [r4, #0]
 736:	429a      	cmp	r2, r3
 738:	d833      	bhi.n	7a2 <operation_run+0x282>
 73a:	6823      	ldr	r3, [r4, #0]
 73c:	6832      	ldr	r2, [r6, #0]
 73e:	4e62      	ldr	r6, [pc, #392]	; (8c8 <operation_run+0x3a8>)
 740:	0412      	lsls	r2, r2, #16
 742:	432a      	orrs	r2, r5
 744:	009b      	lsls	r3, r3, #2
 746:	519a      	str	r2, [r3, r6]
 748:	6821      	ldr	r1, [r4, #0]
 74a:	20dd      	movs	r0, #221	; 0xdd
 74c:	f7ff fcf8 	bl	140 <mbus_write_message32>
 750:	2064      	movs	r0, #100	; 0x64
 752:	f7ff fca0 	bl	96 <delay>
 756:	6823      	ldr	r3, [r4, #0]
 758:	20dd      	movs	r0, #221	; 0xdd
 75a:	009b      	lsls	r3, r3, #2
 75c:	5999      	ldr	r1, [r3, r6]
 75e:	f7ff fcef 	bl	140 <mbus_write_message32>
 762:	2064      	movs	r0, #100	; 0x64
 764:	f7ff fc97 	bl	96 <delay>
 768:	6823      	ldr	r3, [r4, #0]
 76a:	20dd      	movs	r0, #221	; 0xdd
 76c:	3301      	adds	r3, #1
 76e:	6023      	str	r3, [r4, #0]
 770:	6823      	ldr	r3, [r4, #0]
 772:	683a      	ldr	r2, [r7, #0]
 774:	009b      	lsls	r3, r3, #2
 776:	519a      	str	r2, [r3, r6]
 778:	6821      	ldr	r1, [r4, #0]
 77a:	f7ff fce1 	bl	140 <mbus_write_message32>
 77e:	2064      	movs	r0, #100	; 0x64
 780:	f7ff fc89 	bl	96 <delay>
 784:	6823      	ldr	r3, [r4, #0]
 786:	20dd      	movs	r0, #221	; 0xdd
 788:	009b      	lsls	r3, r3, #2
 78a:	5999      	ldr	r1, [r3, r6]
 78c:	f7ff fcd8 	bl	140 <mbus_write_message32>
 790:	2064      	movs	r0, #100	; 0x64
 792:	f7ff fc80 	bl	96 <delay>
 796:	6822      	ldr	r2, [r4, #0]
 798:	4b4c      	ldr	r3, [pc, #304]	; (8cc <operation_run+0x3ac>)
 79a:	601a      	str	r2, [r3, #0]
 79c:	6823      	ldr	r3, [r4, #0]
 79e:	3301      	adds	r3, #1
 7a0:	6023      	str	r3, [r4, #0]
 7a2:	4b3b      	ldr	r3, [pc, #236]	; (890 <operation_run+0x370>)
 7a4:	681b      	ldr	r3, [r3, #0]
 7a6:	2b00      	cmp	r3, #0
 7a8:	d01d      	beq.n	7e6 <operation_run+0x2c6>
 7aa:	9901      	ldr	r1, [sp, #4]
 7ac:	2000      	movs	r0, #0
 7ae:	f7ff fe55 	bl	45c <send_radio_data_ppm>
 7b2:	20fa      	movs	r0, #250	; 0xfa
 7b4:	0100      	lsls	r0, r0, #4
 7b6:	f7ff fc6e 	bl	96 <delay>
 7ba:	1c29      	adds	r1, r5, #0
 7bc:	2000      	movs	r0, #0
 7be:	f7ff fe4d 	bl	45c <send_radio_data_ppm>
 7c2:	20fa      	movs	r0, #250	; 0xfa
 7c4:	0100      	lsls	r0, r0, #4
 7c6:	f7ff fc66 	bl	96 <delay>
 7ca:	4b2f      	ldr	r3, [pc, #188]	; (888 <operation_run+0x368>)
 7cc:	2000      	movs	r0, #0
 7ce:	6819      	ldr	r1, [r3, #0]
 7d0:	f7ff fe44 	bl	45c <send_radio_data_ppm>
 7d4:	20fa      	movs	r0, #250	; 0xfa
 7d6:	0100      	lsls	r0, r0, #4
 7d8:	f7ff fc5d 	bl	96 <delay>
 7dc:	4b36      	ldr	r3, [pc, #216]	; (8b8 <operation_run+0x398>)
 7de:	2000      	movs	r0, #0
 7e0:	6819      	ldr	r1, [r3, #0]
 7e2:	f7ff fe3b 	bl	45c <send_radio_data_ppm>
 7e6:	4b2b      	ldr	r3, [pc, #172]	; (894 <operation_run+0x374>)
 7e8:	681b      	ldr	r3, [r3, #0]
 7ea:	2b04      	cmp	r3, #4
 7ec:	d809      	bhi.n	802 <operation_run+0x2e2>
 7ee:	20fa      	movs	r0, #250	; 0xfa
 7f0:	0100      	lsls	r0, r0, #4
 7f2:	f7ff fc50 	bl	96 <delay>
 7f6:	2001      	movs	r0, #1
 7f8:	4935      	ldr	r1, [pc, #212]	; (8d0 <operation_run+0x3b0>)
 7fa:	f7ff fe2f 	bl	45c <send_radio_data_ppm>
 7fe:	4b35      	ldr	r3, [pc, #212]	; (8d4 <operation_run+0x3b4>)
 800:	e000      	b.n	804 <operation_run+0x2e4>
 802:	4b35      	ldr	r3, [pc, #212]	; (8d8 <operation_run+0x3b8>)
 804:	6818      	ldr	r0, [r3, #0]
 806:	2101      	movs	r1, #1
 808:	b280      	uxth	r0, r0
 80a:	1c0a      	adds	r2, r1, #0
 80c:	f7ff fc58 	bl	c0 <set_wakeup_timer>
 810:	4b32      	ldr	r3, [pc, #200]	; (8dc <operation_run+0x3bc>)
 812:	781b      	ldrb	r3, [r3, #0]
 814:	2b00      	cmp	r3, #0
 816:	d004      	beq.n	822 <operation_run+0x302>
 818:	4b31      	ldr	r3, [pc, #196]	; (8e0 <operation_run+0x3c0>)
 81a:	2200      	movs	r2, #0
 81c:	701a      	strb	r2, [r3, #0]
 81e:	f7ff fd59 	bl	2d4 <radio_power_off>
 822:	2101      	movs	r1, #1
 824:	2219      	movs	r2, #25
 826:	2006      	movs	r0, #6
 828:	f7ff fccb 	bl	1c2 <mbus_remote_register_write>
 82c:	4a22      	ldr	r2, [pc, #136]	; (8b8 <operation_run+0x398>)
 82e:	4b1a      	ldr	r3, [pc, #104]	; (898 <operation_run+0x378>)
 830:	6811      	ldr	r1, [r2, #0]
 832:	2900      	cmp	r1, #0
 834:	d058      	beq.n	8e8 <operation_run+0x3c8>
 836:	4c2b      	ldr	r4, [pc, #172]	; (8e4 <operation_run+0x3c4>)
 838:	6810      	ldr	r0, [r2, #0]
 83a:	6821      	ldr	r1, [r4, #0]
 83c:	4288      	cmp	r0, r1
 83e:	d253      	bcs.n	8e8 <operation_run+0x3c8>
 840:	6818      	ldr	r0, [r3, #0]
 842:	4917      	ldr	r1, [pc, #92]	; (8a0 <operation_run+0x380>)
 844:	4001      	ands	r1, r0
 846:	6019      	str	r1, [r3, #0]
 848:	6818      	ldr	r0, [r3, #0]
 84a:	4914      	ldr	r1, [pc, #80]	; (89c <operation_run+0x37c>)
 84c:	4001      	ands	r1, r0
 84e:	6019      	str	r1, [r3, #0]
 850:	6819      	ldr	r1, [r3, #0]
 852:	2002      	movs	r0, #2
 854:	4381      	bics	r1, r0
 856:	6019      	str	r1, [r3, #0]
 858:	6819      	ldr	r1, [r3, #0]
 85a:	2001      	movs	r0, #1
 85c:	4381      	bics	r1, r0
 85e:	6019      	str	r1, [r3, #0]
 860:	6811      	ldr	r1, [r2, #0]
 862:	20bb      	movs	r0, #187	; 0xbb
 864:	f7ff fc6c 	bl	140 <mbus_write_message32>
 868:	2064      	movs	r0, #100	; 0x64
 86a:	f7ff fc14 	bl	96 <delay>
 86e:	6821      	ldr	r1, [r4, #0]
 870:	20bb      	movs	r0, #187	; 0xbb
 872:	f7ff fc65 	bl	140 <mbus_write_message32>
 876:	2064      	movs	r0, #100	; 0x64
 878:	f7ff fc0d 	bl	96 <delay>
 87c:	e046      	b.n	90c <operation_run+0x3ec>
 87e:	46c0      	nop			; (mov r8, r8)
 880:	000012d8 	.word	0x000012d8
 884:	a0000004 	.word	0xa0000004
 888:	000012f0 	.word	0x000012f0
 88c:	000012f4 	.word	0x000012f4
 890:	000012cc 	.word	0x000012cc
 894:	000012c4 	.word	0x000012c4
 898:	000012b0 	.word	0x000012b0
 89c:	fffff7ff 	.word	0xfffff7ff
 8a0:	ffffefff 	.word	0xffffefff
 8a4:	000012a8 	.word	0x000012a8
 8a8:	ffdfffff 	.word	0xffdfffff
 8ac:	000012e0 	.word	0x000012e0
 8b0:	fffeffff 	.word	0xfffeffff
 8b4:	fafafafa 	.word	0xfafafafa
 8b8:	000012d4 	.word	0x000012d4
 8bc:	00001304 	.word	0x00001304
 8c0:	00001300 	.word	0x00001300
 8c4:	0000031f 	.word	0x0000031f
 8c8:	0000130c 	.word	0x0000130c
 8cc:	000012bc 	.word	0x000012bc
 8d0:	00faf000 	.word	0x00faf000
 8d4:	000012dc 	.word	0x000012dc
 8d8:	000012fc 	.word	0x000012fc
 8dc:	00001308 	.word	0x00001308
 8e0:	000012b8 	.word	0x000012b8
 8e4:	000012d0 	.word	0x000012d0
 8e8:	6819      	ldr	r1, [r3, #0]
 8ea:	2280      	movs	r2, #128	; 0x80
 8ec:	0152      	lsls	r2, r2, #5
 8ee:	430a      	orrs	r2, r1
 8f0:	601a      	str	r2, [r3, #0]
 8f2:	6819      	ldr	r1, [r3, #0]
 8f4:	2280      	movs	r2, #128	; 0x80
 8f6:	0112      	lsls	r2, r2, #4
 8f8:	430a      	orrs	r2, r1
 8fa:	601a      	str	r2, [r3, #0]
 8fc:	681a      	ldr	r2, [r3, #0]
 8fe:	2102      	movs	r1, #2
 900:	430a      	orrs	r2, r1
 902:	601a      	str	r2, [r3, #0]
 904:	681a      	ldr	r2, [r3, #0]
 906:	2101      	movs	r1, #1
 908:	430a      	orrs	r2, r1
 90a:	601a      	str	r2, [r3, #0]
 90c:	4b10      	ldr	r3, [pc, #64]	; (950 <operation_run+0x430>)
 90e:	2005      	movs	r0, #5
 910:	681a      	ldr	r2, [r3, #0]
 912:	2112      	movs	r1, #18
 914:	f7ff fc55 	bl	1c2 <mbus_remote_register_write>
 918:	2064      	movs	r0, #100	; 0x64
 91a:	f7ff fbbc 	bl	96 <delay>
 91e:	4b0d      	ldr	r3, [pc, #52]	; (954 <operation_run+0x434>)
 920:	681a      	ldr	r2, [r3, #0]
 922:	2a00      	cmp	r2, #0
 924:	d00a      	beq.n	93c <operation_run+0x41c>
 926:	4a0c      	ldr	r2, [pc, #48]	; (958 <operation_run+0x438>)
 928:	2132      	movs	r1, #50	; 0x32
 92a:	6812      	ldr	r2, [r2, #0]
 92c:	681b      	ldr	r3, [r3, #0]
 92e:	4099      	lsls	r1, r3
 930:	428a      	cmp	r2, r1
 932:	d903      	bls.n	93c <operation_run+0x41c>
 934:	4b09      	ldr	r3, [pc, #36]	; (95c <operation_run+0x43c>)
 936:	2200      	movs	r2, #0
 938:	701a      	strb	r2, [r3, #0]
 93a:	e004      	b.n	946 <operation_run+0x426>
 93c:	f7ff fc2a 	bl	194 <mbus_sleep_all>
 940:	e7fe      	b.n	940 <operation_run+0x420>
 942:	f7ff fd43 	bl	3cc <temp_sensor_assert_reset>
 946:	f7ff fd57 	bl	3f8 <temp_power_off>
 94a:	f7ff fd71 	bl	430 <operation_sleep_notimer>
 94e:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
 950:	000012b0 	.word	0x000012b0
 954:	000012e8 	.word	0x000012e8
 958:	000012c4 	.word	0x000012c4
 95c:	000012c0 	.word	0x000012c0

Disassembly of section .text.handler_ext_int_0:

00000960 <handler_ext_int_0>:
 960:	4b01      	ldr	r3, [pc, #4]	; (968 <handler_ext_int_0+0x8>)
 962:	2201      	movs	r2, #1
 964:	601a      	str	r2, [r3, #0]
 966:	4770      	bx	lr
 968:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

0000096c <handler_ext_int_1>:
 96c:	4b01      	ldr	r3, [pc, #4]	; (974 <handler_ext_int_1+0x8>)
 96e:	2202      	movs	r2, #2
 970:	601a      	str	r2, [r3, #0]
 972:	4770      	bx	lr
 974:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

00000978 <handler_ext_int_2>:
 978:	4b03      	ldr	r3, [pc, #12]	; (988 <handler_ext_int_2+0x10>)
 97a:	2204      	movs	r2, #4
 97c:	601a      	str	r2, [r3, #0]
 97e:	4b03      	ldr	r3, [pc, #12]	; (98c <handler_ext_int_2+0x14>)
 980:	2210      	movs	r2, #16
 982:	701a      	strb	r2, [r3, #0]
 984:	4770      	bx	lr
 986:	46c0      	nop			; (mov r8, r8)
 988:	e000e280 	.word	0xe000e280
 98c:	000012e0 	.word	0x000012e0

Disassembly of section .text.handler_ext_int_3:

00000990 <handler_ext_int_3>:
 990:	4b03      	ldr	r3, [pc, #12]	; (9a0 <handler_ext_int_3+0x10>)
 992:	2208      	movs	r2, #8
 994:	601a      	str	r2, [r3, #0]
 996:	4b03      	ldr	r3, [pc, #12]	; (9a4 <handler_ext_int_3+0x14>)
 998:	2211      	movs	r2, #17
 99a:	701a      	strb	r2, [r3, #0]
 99c:	4770      	bx	lr
 99e:	46c0      	nop			; (mov r8, r8)
 9a0:	e000e280 	.word	0xe000e280
 9a4:	000012e0 	.word	0x000012e0

Disassembly of section .text.handler_ext_int_4:

000009a8 <handler_ext_int_4>:
 9a8:	4b03      	ldr	r3, [pc, #12]	; (9b8 <handler_ext_int_4+0x10>)
 9aa:	2210      	movs	r2, #16
 9ac:	601a      	str	r2, [r3, #0]
 9ae:	4b03      	ldr	r3, [pc, #12]	; (9bc <handler_ext_int_4+0x14>)
 9b0:	2212      	movs	r2, #18
 9b2:	701a      	strb	r2, [r3, #0]
 9b4:	4770      	bx	lr
 9b6:	46c0      	nop			; (mov r8, r8)
 9b8:	e000e280 	.word	0xe000e280
 9bc:	000012e0 	.word	0x000012e0

Disassembly of section .text.handler_ext_int_5:

000009c0 <handler_ext_int_5>:
 9c0:	4b03      	ldr	r3, [pc, #12]	; (9d0 <handler_ext_int_5+0x10>)
 9c2:	2220      	movs	r2, #32
 9c4:	601a      	str	r2, [r3, #0]
 9c6:	4b03      	ldr	r3, [pc, #12]	; (9d4 <handler_ext_int_5+0x14>)
 9c8:	2213      	movs	r2, #19
 9ca:	701a      	strb	r2, [r3, #0]
 9cc:	4770      	bx	lr
 9ce:	46c0      	nop			; (mov r8, r8)
 9d0:	e000e280 	.word	0xe000e280
 9d4:	000012e0 	.word	0x000012e0

Disassembly of section .text.handler_ext_int_6:

000009d8 <handler_ext_int_6>:
 9d8:	4b03      	ldr	r3, [pc, #12]	; (9e8 <handler_ext_int_6+0x10>)
 9da:	2240      	movs	r2, #64	; 0x40
 9dc:	601a      	str	r2, [r3, #0]
 9de:	4b03      	ldr	r3, [pc, #12]	; (9ec <handler_ext_int_6+0x14>)
 9e0:	2214      	movs	r2, #20
 9e2:	701a      	strb	r2, [r3, #0]
 9e4:	4770      	bx	lr
 9e6:	46c0      	nop			; (mov r8, r8)
 9e8:	e000e280 	.word	0xe000e280
 9ec:	000012e0 	.word	0x000012e0

Disassembly of section .text.handler_ext_int_7:

000009f0 <handler_ext_int_7>:
 9f0:	4b03      	ldr	r3, [pc, #12]	; (a00 <handler_ext_int_7+0x10>)
 9f2:	2280      	movs	r2, #128	; 0x80
 9f4:	601a      	str	r2, [r3, #0]
 9f6:	4b03      	ldr	r3, [pc, #12]	; (a04 <handler_ext_int_7+0x14>)
 9f8:	2215      	movs	r2, #21
 9fa:	701a      	strb	r2, [r3, #0]
 9fc:	4770      	bx	lr
 9fe:	46c0      	nop			; (mov r8, r8)
 a00:	e000e280 	.word	0xe000e280
 a04:	000012e0 	.word	0x000012e0

Disassembly of section .text.handler_ext_int_8:

00000a08 <handler_ext_int_8>:
 a08:	4b03      	ldr	r3, [pc, #12]	; (a18 <handler_ext_int_8+0x10>)
 a0a:	2280      	movs	r2, #128	; 0x80
 a0c:	0052      	lsls	r2, r2, #1
 a0e:	601a      	str	r2, [r3, #0]
 a10:	4b02      	ldr	r3, [pc, #8]	; (a1c <handler_ext_int_8+0x14>)
 a12:	2216      	movs	r2, #22
 a14:	701a      	strb	r2, [r3, #0]
 a16:	4770      	bx	lr
 a18:	e000e280 	.word	0xe000e280
 a1c:	000012e0 	.word	0x000012e0

Disassembly of section .text.handler_ext_int_9:

00000a20 <handler_ext_int_9>:
 a20:	4b03      	ldr	r3, [pc, #12]	; (a30 <handler_ext_int_9+0x10>)
 a22:	2280      	movs	r2, #128	; 0x80
 a24:	0092      	lsls	r2, r2, #2
 a26:	601a      	str	r2, [r3, #0]
 a28:	4b02      	ldr	r3, [pc, #8]	; (a34 <handler_ext_int_9+0x14>)
 a2a:	2217      	movs	r2, #23
 a2c:	701a      	strb	r2, [r3, #0]
 a2e:	4770      	bx	lr
 a30:	e000e280 	.word	0xe000e280
 a34:	000012e0 	.word	0x000012e0

Disassembly of section .text.handler_ext_int_10:

00000a38 <handler_ext_int_10>:
 a38:	4b02      	ldr	r3, [pc, #8]	; (a44 <handler_ext_int_10+0xc>)
 a3a:	2280      	movs	r2, #128	; 0x80
 a3c:	00d2      	lsls	r2, r2, #3
 a3e:	601a      	str	r2, [r3, #0]
 a40:	4770      	bx	lr
 a42:	46c0      	nop			; (mov r8, r8)
 a44:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

00000a48 <handler_ext_int_11>:
 a48:	4b02      	ldr	r3, [pc, #8]	; (a54 <handler_ext_int_11+0xc>)
 a4a:	2280      	movs	r2, #128	; 0x80
 a4c:	0112      	lsls	r2, r2, #4
 a4e:	601a      	str	r2, [r3, #0]
 a50:	4770      	bx	lr
 a52:	46c0      	nop			; (mov r8, r8)
 a54:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

00000a58 <handler_ext_int_12>:
 a58:	4b02      	ldr	r3, [pc, #8]	; (a64 <handler_ext_int_12+0xc>)
 a5a:	2280      	movs	r2, #128	; 0x80
 a5c:	0152      	lsls	r2, r2, #5
 a5e:	601a      	str	r2, [r3, #0]
 a60:	4770      	bx	lr
 a62:	46c0      	nop			; (mov r8, r8)
 a64:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

00000a68 <handler_ext_int_13>:
 a68:	4b02      	ldr	r3, [pc, #8]	; (a74 <handler_ext_int_13+0xc>)
 a6a:	2280      	movs	r2, #128	; 0x80
 a6c:	0192      	lsls	r2, r2, #6
 a6e:	601a      	str	r2, [r3, #0]
 a70:	4770      	bx	lr
 a72:	46c0      	nop			; (mov r8, r8)
 a74:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

00000a78 <handler_ext_int_14>:
 a78:	4b02      	ldr	r3, [pc, #8]	; (a84 <handler_ext_int_14+0xc>)
 a7a:	2280      	movs	r2, #128	; 0x80
 a7c:	01d2      	lsls	r2, r2, #7
 a7e:	601a      	str	r2, [r3, #0]
 a80:	4770      	bx	lr
 a82:	46c0      	nop			; (mov r8, r8)
 a84:	e000e280 	.word	0xe000e280

Disassembly of section .text.startup.main:

00000a88 <main>:
     a88:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
     a8a:	2100      	movs	r1, #0
     a8c:	2201      	movs	r2, #1
     a8e:	2064      	movs	r0, #100	; 0x64
     a90:	f7ff fb16 	bl	c0 <set_wakeup_timer>
     a94:	f7ff fb2a 	bl	ec <enable_reg_irq>
     a98:	48f6      	ldr	r0, [pc, #984]	; (e74 <main+0x3ec>)
     a9a:	f7ff fb05 	bl	a8 <config_timerwd>
     a9e:	4af6      	ldr	r2, [pc, #984]	; (e78 <main+0x3f0>)
     aa0:	49f6      	ldr	r1, [pc, #984]	; (e7c <main+0x3f4>)
     aa2:	6813      	ldr	r3, [r2, #0]
     aa4:	428b      	cmp	r3, r1
     aa6:	d100      	bne.n	aaa <main+0x22>
     aa8:	e181      	b.n	dae <main+0x326>
     aaa:	4bf5      	ldr	r3, [pc, #980]	; (e80 <main+0x3f8>)
     aac:	2580      	movs	r5, #128	; 0x80
     aae:	6818      	ldr	r0, [r3, #0]
     ab0:	016d      	lsls	r5, r5, #5
     ab2:	4328      	orrs	r0, r5
     ab4:	6018      	str	r0, [r3, #0]
     ab6:	6818      	ldr	r0, [r3, #0]
     ab8:	4cf2      	ldr	r4, [pc, #968]	; (e84 <main+0x3fc>)
     aba:	26c0      	movs	r6, #192	; 0xc0
     abc:	4020      	ands	r0, r4
     abe:	2480      	movs	r4, #128	; 0x80
     ac0:	01a4      	lsls	r4, r4, #6
     ac2:	4320      	orrs	r0, r4
     ac4:	6018      	str	r0, [r3, #0]
     ac6:	681c      	ldr	r4, [r3, #0]
     ac8:	48ef      	ldr	r0, [pc, #956]	; (e88 <main+0x400>)
     aca:	02f6      	lsls	r6, r6, #11
     acc:	4020      	ands	r0, r4
     ace:	2480      	movs	r4, #128	; 0x80
     ad0:	0224      	lsls	r4, r4, #8
     ad2:	4320      	orrs	r0, r4
     ad4:	6018      	str	r0, [r3, #0]
     ad6:	6818      	ldr	r0, [r3, #0]
     ad8:	2400      	movs	r4, #0
     ada:	4330      	orrs	r0, r6
     adc:	6018      	str	r0, [r3, #0]
     ade:	6818      	ldr	r0, [r3, #0]
     ae0:	4bea      	ldr	r3, [pc, #936]	; (e8c <main+0x404>)
     ae2:	2707      	movs	r7, #7
     ae4:	6018      	str	r0, [r3, #0]
     ae6:	4bea      	ldr	r3, [pc, #936]	; (e90 <main+0x408>)
     ae8:	2064      	movs	r0, #100	; 0x64
     aea:	701c      	strb	r4, [r3, #0]
     aec:	4be9      	ldr	r3, [pc, #932]	; (e94 <main+0x40c>)
     aee:	6011      	str	r1, [r2, #0]
     af0:	601c      	str	r4, [r3, #0]
     af2:	4be9      	ldr	r3, [pc, #932]	; (e98 <main+0x410>)
     af4:	601c      	str	r4, [r3, #0]
     af6:	4be9      	ldr	r3, [pc, #932]	; (e9c <main+0x414>)
     af8:	701c      	strb	r4, [r3, #0]
     afa:	4be9      	ldr	r3, [pc, #932]	; (ea0 <main+0x418>)
     afc:	601c      	str	r4, [r3, #0]
     afe:	4be9      	ldr	r3, [pc, #932]	; (ea4 <main+0x41c>)
     b00:	601c      	str	r4, [r3, #0]
     b02:	f7ff fac8 	bl	96 <delay>
     b06:	2004      	movs	r0, #4
     b08:	f7ff fb3a 	bl	180 <mbus_enumerate>
     b0c:	2064      	movs	r0, #100	; 0x64
     b0e:	f7ff fac2 	bl	96 <delay>
     b12:	2005      	movs	r0, #5
     b14:	f7ff fb34 	bl	180 <mbus_enumerate>
     b18:	2064      	movs	r0, #100	; 0x64
     b1a:	f7ff fabc 	bl	96 <delay>
     b1e:	2006      	movs	r0, #6
     b20:	f7ff fb2e 	bl	180 <mbus_enumerate>
     b24:	2064      	movs	r0, #100	; 0x64
     b26:	f7ff fab6 	bl	96 <delay>
     b2a:	2007      	movs	r0, #7
     b2c:	f7ff fb28 	bl	180 <mbus_enumerate>
     b30:	2064      	movs	r0, #100	; 0x64
     b32:	f7ff fab0 	bl	96 <delay>
     b36:	2151      	movs	r1, #81	; 0x51
     b38:	2209      	movs	r2, #9
     b3a:	2007      	movs	r0, #7
     b3c:	f7ff fb41 	bl	1c2 <mbus_remote_register_write>
     b40:	2064      	movs	r0, #100	; 0x64
     b42:	f7ff faa8 	bl	96 <delay>
     b46:	2117      	movs	r1, #23
     b48:	4ad7      	ldr	r2, [pc, #860]	; (ea8 <main+0x420>)
     b4a:	2007      	movs	r0, #7
     b4c:	f7ff fb39 	bl	1c2 <mbus_remote_register_write>
     b50:	2064      	movs	r0, #100	; 0x64
     b52:	f7ff faa0 	bl	96 <delay>
     b56:	2117      	movs	r1, #23
     b58:	4ad3      	ldr	r2, [pc, #844]	; (ea8 <main+0x420>)
     b5a:	2007      	movs	r0, #7
     b5c:	f7ff fb31 	bl	1c2 <mbus_remote_register_write>
     b60:	2064      	movs	r0, #100	; 0x64
     b62:	f7ff fa98 	bl	96 <delay>
     b66:	2118      	movs	r1, #24
     b68:	4ad0      	ldr	r2, [pc, #832]	; (eac <main+0x424>)
     b6a:	2007      	movs	r0, #7
     b6c:	f7ff fb29 	bl	1c2 <mbus_remote_register_write>
     b70:	2064      	movs	r0, #100	; 0x64
     b72:	f7ff fa90 	bl	96 <delay>
     b76:	2119      	movs	r1, #25
     b78:	4acd      	ldr	r2, [pc, #820]	; (eb0 <main+0x428>)
     b7a:	2007      	movs	r0, #7
     b7c:	f7ff fb21 	bl	1c2 <mbus_remote_register_write>
     b80:	2064      	movs	r0, #100	; 0x64
     b82:	f7ff fa88 	bl	96 <delay>
     b86:	211a      	movs	r1, #26
     b88:	4aca      	ldr	r2, [pc, #808]	; (eb4 <main+0x42c>)
     b8a:	2007      	movs	r0, #7
     b8c:	f7ff fb19 	bl	1c2 <mbus_remote_register_write>
     b90:	2064      	movs	r0, #100	; 0x64
     b92:	f7ff fa80 	bl	96 <delay>
     b96:	2115      	movs	r1, #21
     b98:	4ac7      	ldr	r2, [pc, #796]	; (eb8 <main+0x430>)
     b9a:	2007      	movs	r0, #7
     b9c:	f7ff fb11 	bl	1c2 <mbus_remote_register_write>
     ba0:	2064      	movs	r0, #100	; 0x64
     ba2:	f7ff fa78 	bl	96 <delay>
     ba6:	2116      	movs	r1, #22
     ba8:	4ac4      	ldr	r2, [pc, #784]	; (ebc <main+0x434>)
     baa:	2007      	movs	r0, #7
     bac:	f7ff fb09 	bl	1c2 <mbus_remote_register_write>
     bb0:	2064      	movs	r0, #100	; 0x64
     bb2:	f7ff fa70 	bl	96 <delay>
     bb6:	22ab      	movs	r2, #171	; 0xab
     bb8:	2105      	movs	r1, #5
     bba:	0112      	lsls	r2, r2, #4
     bbc:	2007      	movs	r0, #7
     bbe:	f7ff fb00 	bl	1c2 <mbus_remote_register_write>
     bc2:	2064      	movs	r0, #100	; 0x64
     bc4:	f7ff fa67 	bl	96 <delay>
     bc8:	2136      	movs	r1, #54	; 0x36
     bca:	2201      	movs	r2, #1
     bcc:	2007      	movs	r0, #7
     bce:	f7ff faf8 	bl	1c2 <mbus_remote_register_write>
     bd2:	2064      	movs	r0, #100	; 0x64
     bd4:	f7ff fa5f 	bl	96 <delay>
     bd8:	2006      	movs	r0, #6
     bda:	1c21      	adds	r1, r4, #0
     bdc:	220f      	movs	r2, #15
     bde:	f7ff faf0 	bl	1c2 <mbus_remote_register_write>
     be2:	2006      	movs	r0, #6
     be4:	2101      	movs	r1, #1
     be6:	2209      	movs	r2, #9
     be8:	f7ff faeb 	bl	1c2 <mbus_remote_register_write>
     bec:	2006      	movs	r0, #6
     bee:	2103      	movs	r1, #3
     bf0:	4ab3      	ldr	r2, [pc, #716]	; (ec0 <main+0x438>)
     bf2:	f7ff fae6 	bl	1c2 <mbus_remote_register_write>
     bf6:	2006      	movs	r0, #6
     bf8:	2104      	movs	r1, #4
     bfa:	4ab2      	ldr	r2, [pc, #712]	; (ec4 <main+0x43c>)
     bfc:	f7ff fae1 	bl	1c2 <mbus_remote_register_write>
     c00:	1c2a      	adds	r2, r5, #0
     c02:	2006      	movs	r0, #6
     c04:	2105      	movs	r1, #5
     c06:	f7ff fadc 	bl	1c2 <mbus_remote_register_write>
     c0a:	2006      	movs	r0, #6
     c0c:	2101      	movs	r1, #1
     c0e:	2209      	movs	r2, #9
     c10:	f7ff fad7 	bl	1c2 <mbus_remote_register_write>
     c14:	4bac      	ldr	r3, [pc, #688]	; (ec8 <main+0x440>)
     c16:	2005      	movs	r0, #5
     c18:	681a      	ldr	r2, [r3, #0]
     c1a:	2119      	movs	r1, #25
     c1c:	0e12      	lsrs	r2, r2, #24
     c1e:	0612      	lsls	r2, r2, #24
     c20:	432a      	orrs	r2, r5
     c22:	601a      	str	r2, [r3, #0]
     c24:	681a      	ldr	r2, [r3, #0]
     c26:	f7ff facc 	bl	1c2 <mbus_remote_register_write>
     c2a:	4ba8      	ldr	r3, [pc, #672]	; (ecc <main+0x444>)
     c2c:	4aa8      	ldr	r2, [pc, #672]	; (ed0 <main+0x448>)
     c2e:	6819      	ldr	r1, [r3, #0]
     c30:	2005      	movs	r0, #5
     c32:	400a      	ands	r2, r1
     c34:	4316      	orrs	r6, r2
     c36:	601e      	str	r6, [r3, #0]
     c38:	681a      	ldr	r2, [r3, #0]
     c3a:	210e      	movs	r1, #14
     c3c:	f7ff fac1 	bl	1c2 <mbus_remote_register_write>
     c40:	4ba4      	ldr	r3, [pc, #656]	; (ed4 <main+0x44c>)
     c42:	260f      	movs	r6, #15
     c44:	681a      	ldr	r2, [r3, #0]
     c46:	2005      	movs	r0, #5
     c48:	43b2      	bics	r2, r6
     c4a:	433a      	orrs	r2, r7
     c4c:	601a      	str	r2, [r3, #0]
     c4e:	681a      	ldr	r2, [r3, #0]
     c50:	1c31      	adds	r1, r6, #0
     c52:	f7ff fab6 	bl	1c2 <mbus_remote_register_write>
     c56:	4ba0      	ldr	r3, [pc, #640]	; (ed8 <main+0x450>)
     c58:	2180      	movs	r1, #128	; 0x80
     c5a:	681a      	ldr	r2, [r3, #0]
     c5c:	0109      	lsls	r1, r1, #4
     c5e:	430a      	orrs	r2, r1
     c60:	601a      	str	r2, [r3, #0]
     c62:	681a      	ldr	r2, [r3, #0]
     c64:	2005      	movs	r0, #5
     c66:	4315      	orrs	r5, r2
     c68:	601d      	str	r5, [r3, #0]
     c6a:	6819      	ldr	r1, [r3, #0]
     c6c:	2280      	movs	r2, #128	; 0x80
     c6e:	0392      	lsls	r2, r2, #14
     c70:	430a      	orrs	r2, r1
     c72:	601a      	str	r2, [r3, #0]
     c74:	681a      	ldr	r2, [r3, #0]
     c76:	4983      	ldr	r1, [pc, #524]	; (e84 <main+0x3fc>)
     c78:	2504      	movs	r5, #4
     c7a:	400a      	ands	r2, r1
     c7c:	2180      	movs	r1, #128	; 0x80
     c7e:	40b9      	lsls	r1, r7
     c80:	430a      	orrs	r2, r1
     c82:	601a      	str	r2, [r3, #0]
     c84:	6819      	ldr	r1, [r3, #0]
     c86:	4a95      	ldr	r2, [pc, #596]	; (edc <main+0x454>)
     c88:	400a      	ands	r2, r1
     c8a:	2180      	movs	r1, #128	; 0x80
     c8c:	0309      	lsls	r1, r1, #12
     c8e:	430a      	orrs	r2, r1
     c90:	601a      	str	r2, [r3, #0]
     c92:	681a      	ldr	r2, [r3, #0]
     c94:	2112      	movs	r1, #18
     c96:	f7ff fa94 	bl	1c2 <mbus_remote_register_write>
     c9a:	2005      	movs	r0, #5
     c9c:	22c0      	movs	r2, #192	; 0xc0
     c9e:	4082      	lsls	r2, r0
     ca0:	2118      	movs	r1, #24
     ca2:	f7ff fa8e 	bl	1c2 <mbus_remote_register_write>
     ca6:	4b8e      	ldr	r3, [pc, #568]	; (ee0 <main+0x458>)
     ca8:	4a8e      	ldr	r2, [pc, #568]	; (ee4 <main+0x45c>)
     caa:	6819      	ldr	r1, [r3, #0]
     cac:	2004      	movs	r0, #4
     cae:	400a      	ands	r2, r1
     cb0:	21bc      	movs	r1, #188	; 0xbc
     cb2:	0409      	lsls	r1, r1, #16
     cb4:	430a      	orrs	r2, r1
     cb6:	601a      	str	r2, [r3, #0]
     cb8:	6819      	ldr	r1, [r3, #0]
     cba:	4a8b      	ldr	r2, [pc, #556]	; (ee8 <main+0x460>)
     cbc:	400a      	ands	r2, r1
     cbe:	601a      	str	r2, [r3, #0]
     cc0:	6819      	ldr	r1, [r3, #0]
     cc2:	4a8a      	ldr	r2, [pc, #552]	; (eec <main+0x464>)
     cc4:	400a      	ands	r2, r1
     cc6:	2190      	movs	r1, #144	; 0x90
     cc8:	0149      	lsls	r1, r1, #5
     cca:	430a      	orrs	r2, r1
     ccc:	601a      	str	r2, [r3, #0]
     cce:	6819      	ldr	r1, [r3, #0]
     cd0:	4a87      	ldr	r2, [pc, #540]	; (ef0 <main+0x468>)
     cd2:	400a      	ands	r2, r1
     cd4:	21c0      	movs	r1, #192	; 0xc0
     cd6:	0289      	lsls	r1, r1, #10
     cd8:	430a      	orrs	r2, r1
     cda:	601a      	str	r2, [r3, #0]
     cdc:	681a      	ldr	r2, [r3, #0]
     cde:	1c21      	adds	r1, r4, #0
     ce0:	f7ff fa6f 	bl	1c2 <mbus_remote_register_write>
     ce4:	4b83      	ldr	r3, [pc, #524]	; (ef4 <main+0x46c>)
     ce6:	211f      	movs	r1, #31
     ce8:	681a      	ldr	r2, [r3, #0]
     cea:	2004      	movs	r0, #4
     cec:	438a      	bics	r2, r1
     cee:	2110      	movs	r1, #16
     cf0:	430a      	orrs	r2, r1
     cf2:	601a      	str	r2, [r3, #0]
     cf4:	6819      	ldr	r1, [r3, #0]
     cf6:	4a80      	ldr	r2, [pc, #512]	; (ef8 <main+0x470>)
     cf8:	400a      	ands	r2, r1
     cfa:	21b8      	movs	r1, #184	; 0xb8
     cfc:	0089      	lsls	r1, r1, #2
     cfe:	430a      	orrs	r2, r1
     d00:	601a      	str	r2, [r3, #0]
     d02:	6819      	ldr	r1, [r3, #0]
     d04:	4a7d      	ldr	r2, [pc, #500]	; (efc <main+0x474>)
     d06:	400a      	ands	r2, r1
     d08:	21a0      	movs	r1, #160	; 0xa0
     d0a:	0249      	lsls	r1, r1, #9
     d0c:	430a      	orrs	r2, r1
     d0e:	601a      	str	r2, [r3, #0]
     d10:	681a      	ldr	r2, [r3, #0]
     d12:	210b      	movs	r1, #11
     d14:	f7ff fa55 	bl	1c2 <mbus_remote_register_write>
     d18:	4b79      	ldr	r3, [pc, #484]	; (f00 <main+0x478>)
     d1a:	22c0      	movs	r2, #192	; 0xc0
     d1c:	6819      	ldr	r1, [r3, #0]
     d1e:	0352      	lsls	r2, r2, #13
     d20:	430a      	orrs	r2, r1
     d22:	601a      	str	r2, [r3, #0]
     d24:	6819      	ldr	r1, [r3, #0]
     d26:	4a77      	ldr	r2, [pc, #476]	; (f04 <main+0x47c>)
     d28:	2004      	movs	r0, #4
     d2a:	400a      	ands	r2, r1
     d2c:	2180      	movs	r1, #128	; 0x80
     d2e:	0109      	lsls	r1, r1, #4
     d30:	430a      	orrs	r2, r1
     d32:	601a      	str	r2, [r3, #0]
     d34:	681a      	ldr	r2, [r3, #0]
     d36:	217f      	movs	r1, #127	; 0x7f
     d38:	438a      	bics	r2, r1
     d3a:	2160      	movs	r1, #96	; 0x60
     d3c:	430a      	orrs	r2, r1
     d3e:	601a      	str	r2, [r3, #0]
     d40:	681a      	ldr	r2, [r3, #0]
     d42:	2101      	movs	r1, #1
     d44:	f7ff fa3d 	bl	1c2 <mbus_remote_register_write>
     d48:	4b6f      	ldr	r3, [pc, #444]	; (f08 <main+0x480>)
     d4a:	213f      	movs	r1, #63	; 0x3f
     d4c:	781a      	ldrb	r2, [r3, #0]
     d4e:	1c28      	adds	r0, r5, #0
     d50:	438a      	bics	r2, r1
     d52:	432a      	orrs	r2, r5
     d54:	701a      	strb	r2, [r3, #0]
     d56:	681a      	ldr	r2, [r3, #0]
     d58:	210c      	movs	r1, #12
     d5a:	f7ff fa32 	bl	1c2 <mbus_remote_register_write>
     d5e:	22c8      	movs	r2, #200	; 0xc8
     d60:	1c28      	adds	r0, r5, #0
     d62:	1c31      	adds	r1, r6, #0
     d64:	0152      	lsls	r2, r2, #5
     d66:	f7ff fa2c 	bl	1c2 <mbus_remote_register_write>
     d6a:	4b68      	ldr	r3, [pc, #416]	; (f0c <main+0x484>)
     d6c:	4a68      	ldr	r2, [pc, #416]	; (f10 <main+0x488>)
     d6e:	213a      	movs	r1, #58	; 0x3a
     d70:	601a      	str	r2, [r3, #0]
     d72:	4b68      	ldr	r3, [pc, #416]	; (f14 <main+0x48c>)
     d74:	2203      	movs	r2, #3
     d76:	601a      	str	r2, [r3, #0]
     d78:	4b67      	ldr	r3, [pc, #412]	; (f18 <main+0x490>)
     d7a:	4a68      	ldr	r2, [pc, #416]	; (f1c <main+0x494>)
     d7c:	601c      	str	r4, [r3, #0]
     d7e:	4b68      	ldr	r3, [pc, #416]	; (f20 <main+0x498>)
     d80:	1c38      	adds	r0, r7, #0
     d82:	601c      	str	r4, [r3, #0]
     d84:	4b67      	ldr	r3, [pc, #412]	; (f24 <main+0x49c>)
     d86:	601c      	str	r4, [r3, #0]
     d88:	4b67      	ldr	r3, [pc, #412]	; (f28 <main+0x4a0>)
     d8a:	701c      	strb	r4, [r3, #0]
     d8c:	4b67      	ldr	r3, [pc, #412]	; (f2c <main+0x4a4>)
     d8e:	701c      	strb	r4, [r3, #0]
     d90:	4b67      	ldr	r3, [pc, #412]	; (f30 <main+0x4a8>)
     d92:	701c      	strb	r4, [r3, #0]
     d94:	4b67      	ldr	r3, [pc, #412]	; (f34 <main+0x4ac>)
     d96:	601c      	str	r4, [r3, #0]
     d98:	f7ff fa13 	bl	1c2 <mbus_remote_register_write>
     d9c:	2064      	movs	r0, #100	; 0x64
     d9e:	f7ff f97a 	bl	96 <delay>
     da2:	f7ff fa2d 	bl	200 <batadc_resetrelease>
     da6:	4b64      	ldr	r3, [pc, #400]	; (f38 <main+0x4b0>)
     da8:	220b      	movs	r2, #11
     daa:	601a      	str	r2, [r3, #0]
     dac:	e03c      	b.n	e28 <main+0x3a0>
     dae:	2278      	movs	r2, #120	; 0x78
     db0:	6815      	ldr	r5, [r2, #0]
     db2:	0c2e      	lsrs	r6, r5, #16
     db4:	0e2c      	lsrs	r4, r5, #24
     db6:	0a2b      	lsrs	r3, r5, #8
     db8:	b2f7      	uxtb	r7, r6
     dba:	2c01      	cmp	r4, #1
     dbc:	d136      	bne.n	e2c <main+0x3a4>
     dbe:	22ff      	movs	r2, #255	; 0xff
     dc0:	4f54      	ldr	r7, [pc, #336]	; (f14 <main+0x48c>)
     dc2:	4013      	ands	r3, r2
     dc4:	603b      	str	r3, [r7, #0]
     dc6:	4b34      	ldr	r3, [pc, #208]	; (e98 <main+0x410>)
     dc8:	4016      	ands	r6, r2
     dca:	4015      	ands	r5, r2
     dcc:	0236      	lsls	r6, r6, #8
     dce:	6819      	ldr	r1, [r3, #0]
     dd0:	19ad      	adds	r5, r5, r6
     dd2:	42a9      	cmp	r1, r5
     dd4:	d222      	bcs.n	e1c <main+0x394>
     dd6:	681a      	ldr	r2, [r3, #0]
     dd8:	3201      	adds	r2, #1
     dda:	601a      	str	r2, [r3, #0]
     ddc:	681d      	ldr	r5, [r3, #0]
     dde:	2d01      	cmp	r5, #1
     de0:	d109      	bne.n	df6 <main+0x36e>
     de2:	f7ff fa19 	bl	218 <radio_power_on>
     de6:	2002      	movs	r0, #2
     de8:	1c29      	adds	r1, r5, #0
     dea:	1c2a      	adds	r2, r5, #0
     dec:	f7ff f968 	bl	c0 <set_wakeup_timer>
     df0:	f7ff f9d0 	bl	194 <mbus_sleep_all>
     df4:	e7fe      	b.n	df4 <main+0x36c>
     df6:	6819      	ldr	r1, [r3, #0]
     df8:	4a50      	ldr	r2, [pc, #320]	; (f3c <main+0x4b4>)
     dfa:	2000      	movs	r0, #0
     dfc:	1889      	adds	r1, r1, r2
     dfe:	f7ff fb2d 	bl	45c <send_radio_data_ppm>
     e02:	20fa      	movs	r0, #250	; 0xfa
     e04:	0100      	lsls	r0, r0, #4
     e06:	f7ff f946 	bl	96 <delay>
     e0a:	6838      	ldr	r0, [r7, #0]
     e0c:	1c21      	adds	r1, r4, #0
     e0e:	b280      	uxth	r0, r0
     e10:	1c22      	adds	r2, r4, #0
     e12:	f7ff f955 	bl	c0 <set_wakeup_timer>
     e16:	f7ff f9bd 	bl	194 <mbus_sleep_all>
     e1a:	e7fe      	b.n	e1a <main+0x392>
     e1c:	2200      	movs	r2, #0
     e1e:	601a      	str	r2, [r3, #0]
     e20:	1c20      	adds	r0, r4, #0
     e22:	4947      	ldr	r1, [pc, #284]	; (f40 <main+0x4b8>)
     e24:	f7ff fb1a 	bl	45c <send_radio_data_ppm>
     e28:	f7ff fb02 	bl	430 <operation_sleep_notimer>
     e2c:	2c02      	cmp	r4, #2
     e2e:	d000      	beq.n	e32 <main+0x3aa>
     e30:	e095      	b.n	f5e <main+0x4d6>
     e32:	21ff      	movs	r1, #255	; 0xff
     e34:	400b      	ands	r3, r1
     e36:	400d      	ands	r5, r1
     e38:	021b      	lsls	r3, r3, #8
     e3a:	18ed      	adds	r5, r5, r3
     e3c:	4833      	ldr	r0, [pc, #204]	; (f0c <main+0x484>)
     e3e:	4b35      	ldr	r3, [pc, #212]	; (f14 <main+0x48c>)
     e40:	210f      	movs	r1, #15
     e42:	4031      	ands	r1, r6
     e44:	6005      	str	r5, [r0, #0]
     e46:	6019      	str	r1, [r3, #0]
     e48:	2310      	movs	r3, #16
     e4a:	401e      	ands	r6, r3
     e4c:	4b35      	ldr	r3, [pc, #212]	; (f24 <main+0x49c>)
     e4e:	4c36      	ldr	r4, [pc, #216]	; (f28 <main+0x4a0>)
     e50:	601e      	str	r6, [r3, #0]
     e52:	7823      	ldrb	r3, [r4, #0]
     e54:	2b00      	cmp	r3, #0
     e56:	d175      	bne.n	f44 <main+0x4bc>
     e58:	2101      	movs	r1, #1
     e5a:	2005      	movs	r0, #5
     e5c:	1c0a      	adds	r2, r1, #0
     e5e:	f7ff f92f 	bl	c0 <set_wakeup_timer>
     e62:	2301      	movs	r3, #1
     e64:	7023      	strb	r3, [r4, #0]
     e66:	4b33      	ldr	r3, [pc, #204]	; (f34 <main+0x4ac>)
     e68:	097f      	lsrs	r7, r7, #5
     e6a:	601f      	str	r7, [r3, #0]
     e6c:	f7ff f992 	bl	194 <mbus_sleep_all>
     e70:	e7fe      	b.n	e70 <main+0x3e8>
     e72:	46c0      	nop			; (mov r8, r8)
     e74:	003fffff 	.word	0x003fffff
     e78:	000012c8 	.word	0x000012c8
     e7c:	deadbeef 	.word	0xdeadbeef
     e80:	0000129c 	.word	0x0000129c
     e84:	ffff9fff 	.word	0xffff9fff
     e88:	fffe7fff 	.word	0xfffe7fff
     e8c:	a000002c 	.word	0xa000002c
     e90:	000012d8 	.word	0x000012d8
     e94:	000012c4 	.word	0x000012c4
     e98:	000012f8 	.word	0x000012f8
     e9c:	000012e0 	.word	0x000012e0
     ea0:	000012f0 	.word	0x000012f0
     ea4:	00001304 	.word	0x00001304
     ea8:	0000e204 	.word	0x0000e204
     eac:	0000e24a 	.word	0x0000e24a
     eb0:	00002203 	.word	0x00002203
     eb4:	00003088 	.word	0x00003088
     eb8:	0000e203 	.word	0x0000e203
     ebc:	0000f10f 	.word	0x0000f10f
     ec0:	00ffffff 	.word	0x00ffffff
     ec4:	000001d3 	.word	0x000001d3
     ec8:	000012b4 	.word	0x000012b4
     ecc:	000012a8 	.word	0x000012a8
     ed0:	fff1ffff 	.word	0xfff1ffff
     ed4:	000012ac 	.word	0x000012ac
     ed8:	000012b0 	.word	0x000012b0
     edc:	ffe07fff 	.word	0xffe07fff
     ee0:	0000128c 	.word	0x0000128c
     ee4:	ff03ffff 	.word	0xff03ffff
     ee8:	fffffe1f 	.word	0xfffffe1f
     eec:	ffffe1ff 	.word	0xffffe1ff
     ef0:	fffc7fff 	.word	0xfffc7fff
     ef4:	000012a0 	.word	0x000012a0
     ef8:	ffffe01f 	.word	0xffffe01f
     efc:	fff01fff 	.word	0xfff01fff
     f00:	00001294 	.word	0x00001294
     f04:	ffff03ff 	.word	0xffff03ff
     f08:	000012a4 	.word	0x000012a4
     f0c:	000012fc 	.word	0x000012fc
     f10:	000083d6 	.word	0x000083d6
     f14:	000012dc 	.word	0x000012dc
     f18:	00001300 	.word	0x00001300
     f1c:	00080800 	.word	0x00080800
     f20:	000012bc 	.word	0x000012bc
     f24:	000012cc 	.word	0x000012cc
     f28:	000012c0 	.word	0x000012c0
     f2c:	000012b8 	.word	0x000012b8
     f30:	00001308 	.word	0x00001308
     f34:	000012e8 	.word	0x000012e8
     f38:	000012d0 	.word	0x000012d0
     f3c:	00abc000 	.word	0x00abc000
     f40:	00faf000 	.word	0x00faf000
     f44:	49bb      	ldr	r1, [pc, #748]	; (1234 <main+0x7ac>)
     f46:	2300      	movs	r3, #0
     f48:	600b      	str	r3, [r1, #0]
     f4a:	49bb      	ldr	r1, [pc, #748]	; (1238 <main+0x7b0>)
     f4c:	600b      	str	r3, [r1, #0]
     f4e:	49bb      	ldr	r1, [pc, #748]	; (123c <main+0x7b4>)
     f50:	600b      	str	r3, [r1, #0]
     f52:	49bb      	ldr	r1, [pc, #748]	; (1240 <main+0x7b8>)
     f54:	600b      	str	r3, [r1, #0]
     f56:	6013      	str	r3, [r2, #0]
     f58:	4aba      	ldr	r2, [pc, #744]	; (1244 <main+0x7bc>)
     f5a:	6013      	str	r3, [r2, #0]
     f5c:	e166      	b.n	122c <main+0x7a4>
     f5e:	2c03      	cmp	r4, #3
     f60:	d141      	bne.n	fe6 <main+0x55e>
     f62:	22ff      	movs	r2, #255	; 0xff
     f64:	4eb8      	ldr	r6, [pc, #736]	; (1248 <main+0x7c0>)
     f66:	4013      	ands	r3, r2
     f68:	6033      	str	r3, [r6, #0]
     f6a:	4bb8      	ldr	r3, [pc, #736]	; (124c <main+0x7c4>)
     f6c:	2400      	movs	r4, #0
     f6e:	701c      	strb	r4, [r3, #0]
     f70:	4bb7      	ldr	r3, [pc, #732]	; (1250 <main+0x7c8>)
     f72:	4015      	ands	r5, r2
     f74:	701c      	strb	r4, [r3, #0]
     f76:	4bb7      	ldr	r3, [pc, #732]	; (1254 <main+0x7cc>)
     f78:	6819      	ldr	r1, [r3, #0]
     f7a:	42a9      	cmp	r1, r5
     f7c:	d231      	bcs.n	fe2 <main+0x55a>
     f7e:	681a      	ldr	r2, [r3, #0]
     f80:	3201      	adds	r2, #1
     f82:	601a      	str	r2, [r3, #0]
     f84:	681d      	ldr	r5, [r3, #0]
     f86:	2d01      	cmp	r5, #1
     f88:	d109      	bne.n	f9e <main+0x516>
     f8a:	f7ff f945 	bl	218 <radio_power_on>
     f8e:	2002      	movs	r0, #2
     f90:	1c29      	adds	r1, r5, #0
     f92:	1c2a      	adds	r2, r5, #0
     f94:	f7ff f894 	bl	c0 <set_wakeup_timer>
     f98:	f7ff f8fc 	bl	194 <mbus_sleep_all>
     f9c:	e7fe      	b.n	f9c <main+0x514>
     f9e:	4ba5      	ldr	r3, [pc, #660]	; (1234 <main+0x7ac>)
     fa0:	1c20      	adds	r0, r4, #0
     fa2:	6819      	ldr	r1, [r3, #0]
     fa4:	23c1      	movs	r3, #193	; 0xc1
     fa6:	03db      	lsls	r3, r3, #15
     fa8:	18c9      	adds	r1, r1, r3
     faa:	0049      	lsls	r1, r1, #1
     fac:	f7ff fa56 	bl	45c <send_radio_data_ppm>
     fb0:	20fa      	movs	r0, #250	; 0xfa
     fb2:	0100      	lsls	r0, r0, #4
     fb4:	f7ff f86f 	bl	96 <delay>
     fb8:	4ba7      	ldr	r3, [pc, #668]	; (1258 <main+0x7d0>)
     fba:	22c2      	movs	r2, #194	; 0xc2
     fbc:	6819      	ldr	r1, [r3, #0]
     fbe:	0412      	lsls	r2, r2, #16
     fc0:	1889      	adds	r1, r1, r2
     fc2:	1c20      	adds	r0, r4, #0
     fc4:	f7ff fa4a 	bl	45c <send_radio_data_ppm>
     fc8:	20fa      	movs	r0, #250	; 0xfa
     fca:	0100      	lsls	r0, r0, #4
     fcc:	f7ff f863 	bl	96 <delay>
     fd0:	6830      	ldr	r0, [r6, #0]
     fd2:	2101      	movs	r1, #1
     fd4:	b280      	uxth	r0, r0
     fd6:	1c0a      	adds	r2, r1, #0
     fd8:	f7ff f872 	bl	c0 <set_wakeup_timer>
     fdc:	f7ff f8da 	bl	194 <mbus_sleep_all>
     fe0:	e7fe      	b.n	fe0 <main+0x558>
     fe2:	601c      	str	r4, [r3, #0]
     fe4:	e120      	b.n	1228 <main+0x7a0>
     fe6:	2c04      	cmp	r4, #4
     fe8:	d17e      	bne.n	10e8 <main+0x660>
     fea:	22ff      	movs	r2, #255	; 0xff
     fec:	4996      	ldr	r1, [pc, #600]	; (1248 <main+0x7c0>)
     fee:	4013      	ands	r3, r2
     ff0:	600b      	str	r3, [r1, #0]
     ff2:	4b9a      	ldr	r3, [pc, #616]	; (125c <main+0x7d4>)
     ff4:	4015      	ands	r5, r2
     ff6:	4a91      	ldr	r2, [pc, #580]	; (123c <main+0x7b4>)
     ff8:	601d      	str	r5, [r3, #0]
     ffa:	6819      	ldr	r1, [r3, #0]
     ffc:	6812      	ldr	r2, [r2, #0]
     ffe:	4291      	cmp	r1, r2
    1000:	d301      	bcc.n	1006 <main+0x57e>
    1002:	2200      	movs	r2, #0
    1004:	601a      	str	r2, [r3, #0]
    1006:	4b93      	ldr	r3, [pc, #588]	; (1254 <main+0x7cc>)
    1008:	681a      	ldr	r2, [r3, #0]
    100a:	2a04      	cmp	r2, #4
    100c:	d840      	bhi.n	1090 <main+0x608>
    100e:	681a      	ldr	r2, [r3, #0]
    1010:	3201      	adds	r2, #1
    1012:	601a      	str	r2, [r3, #0]
    1014:	681c      	ldr	r4, [r3, #0]
    1016:	2c01      	cmp	r4, #1
    1018:	d109      	bne.n	102e <main+0x5a6>
    101a:	f7ff f8fd 	bl	218 <radio_power_on>
    101e:	2002      	movs	r0, #2
    1020:	1c21      	adds	r1, r4, #0
    1022:	1c22      	adds	r2, r4, #0
    1024:	f7ff f84c 	bl	c0 <set_wakeup_timer>
    1028:	f7ff f8b4 	bl	194 <mbus_sleep_all>
    102c:	e7fe      	b.n	102c <main+0x5a4>
    102e:	6819      	ldr	r1, [r3, #0]
    1030:	4b8b      	ldr	r3, [pc, #556]	; (1260 <main+0x7d8>)
    1032:	2000      	movs	r0, #0
    1034:	18c9      	adds	r1, r1, r3
    1036:	f7ff fa11 	bl	45c <send_radio_data_ppm>
    103a:	4b83      	ldr	r3, [pc, #524]	; (1248 <main+0x7c0>)
    103c:	2101      	movs	r1, #1
    103e:	6818      	ldr	r0, [r3, #0]
    1040:	1c0a      	adds	r2, r1, #0
    1042:	b280      	uxth	r0, r0
    1044:	f7ff f83c 	bl	c0 <set_wakeup_timer>
    1048:	f7ff f8a4 	bl	194 <mbus_sleep_all>
    104c:	e7fe      	b.n	104c <main+0x5c4>
    104e:	4885      	ldr	r0, [pc, #532]	; (1264 <main+0x7dc>)
    1050:	f7ff f82a 	bl	a8 <config_timerwd>
    1054:	6821      	ldr	r1, [r4, #0]
    1056:	20dd      	movs	r0, #221	; 0xdd
    1058:	f7ff f872 	bl	140 <mbus_write_message32>
    105c:	2064      	movs	r0, #100	; 0x64
    105e:	f7ff f81a 	bl	96 <delay>
    1062:	6823      	ldr	r3, [r4, #0]
    1064:	4d80      	ldr	r5, [pc, #512]	; (1268 <main+0x7e0>)
    1066:	009b      	lsls	r3, r3, #2
    1068:	5959      	ldr	r1, [r3, r5]
    106a:	20dd      	movs	r0, #221	; 0xdd
    106c:	f7ff f868 	bl	140 <mbus_write_message32>
    1070:	2064      	movs	r0, #100	; 0x64
    1072:	f7ff f810 	bl	96 <delay>
    1076:	6823      	ldr	r3, [r4, #0]
    1078:	2000      	movs	r0, #0
    107a:	009b      	lsls	r3, r3, #2
    107c:	5959      	ldr	r1, [r3, r5]
    107e:	f7ff f9ed 	bl	45c <send_radio_data_ppm>
    1082:	20fa      	movs	r0, #250	; 0xfa
    1084:	0100      	lsls	r0, r0, #4
    1086:	f7ff f806 	bl	96 <delay>
    108a:	6823      	ldr	r3, [r4, #0]
    108c:	3b01      	subs	r3, #1
    108e:	6023      	str	r3, [r4, #0]
    1090:	4b72      	ldr	r3, [pc, #456]	; (125c <main+0x7d4>)
    1092:	2500      	movs	r5, #0
    1094:	681a      	ldr	r2, [r3, #0]
    1096:	496a      	ldr	r1, [pc, #424]	; (1240 <main+0x7b8>)
    1098:	42aa      	cmp	r2, r5
    109a:	d102      	bne.n	10a2 <main+0x61a>
    109c:	680d      	ldr	r5, [r1, #0]
    109e:	1e6a      	subs	r2, r5, #1
    10a0:	4195      	sbcs	r5, r2
    10a2:	6818      	ldr	r0, [r3, #0]
    10a4:	2200      	movs	r2, #0
    10a6:	4290      	cmp	r0, r2
    10a8:	d007      	beq.n	10ba <main+0x632>
    10aa:	681a      	ldr	r2, [r3, #0]
    10ac:	4b63      	ldr	r3, [pc, #396]	; (123c <main+0x7b4>)
    10ae:	6809      	ldr	r1, [r1, #0]
    10b0:	681b      	ldr	r3, [r3, #0]
    10b2:	188a      	adds	r2, r1, r2
    10b4:	4293      	cmp	r3, r2
    10b6:	4192      	sbcs	r2, r2
    10b8:	4252      	negs	r2, r2
    10ba:	4315      	orrs	r5, r2
    10bc:	4c60      	ldr	r4, [pc, #384]	; (1240 <main+0x7b8>)
    10be:	d1c6      	bne.n	104e <main+0x5c6>
    10c0:	6822      	ldr	r2, [r4, #0]
    10c2:	4b69      	ldr	r3, [pc, #420]	; (1268 <main+0x7e0>)
    10c4:	0092      	lsls	r2, r2, #2
    10c6:	58d1      	ldr	r1, [r2, r3]
    10c8:	1c28      	adds	r0, r5, #0
    10ca:	f7ff f9c7 	bl	45c <send_radio_data_ppm>
    10ce:	4867      	ldr	r0, [pc, #412]	; (126c <main+0x7e4>)
    10d0:	f7fe ffe1 	bl	96 <delay>
    10d4:	2001      	movs	r0, #1
    10d6:	4966      	ldr	r1, [pc, #408]	; (1270 <main+0x7e8>)
    10d8:	f7ff f9c0 	bl	45c <send_radio_data_ppm>
    10dc:	4b5d      	ldr	r3, [pc, #372]	; (1254 <main+0x7cc>)
    10de:	601d      	str	r5, [r3, #0]
    10e0:	4b56      	ldr	r3, [pc, #344]	; (123c <main+0x7b4>)
    10e2:	681b      	ldr	r3, [r3, #0]
    10e4:	6023      	str	r3, [r4, #0]
    10e6:	e69f      	b.n	e28 <main+0x3a0>
    10e8:	2c07      	cmp	r4, #7
    10ea:	d151      	bne.n	1190 <main+0x708>
    10ec:	27ff      	movs	r7, #255	; 0xff
    10ee:	4956      	ldr	r1, [pc, #344]	; (1248 <main+0x7c0>)
    10f0:	403b      	ands	r3, r7
    10f2:	600b      	str	r3, [r1, #0]
    10f4:	4b5f      	ldr	r3, [pc, #380]	; (1274 <main+0x7ec>)
    10f6:	403e      	ands	r6, r7
    10f8:	601e      	str	r6, [r3, #0]
    10fa:	4b4e      	ldr	r3, [pc, #312]	; (1234 <main+0x7ac>)
    10fc:	2200      	movs	r2, #0
    10fe:	601a      	str	r2, [r3, #0]
    1100:	4b54      	ldr	r3, [pc, #336]	; (1254 <main+0x7cc>)
    1102:	403d      	ands	r5, r7
    1104:	6819      	ldr	r1, [r3, #0]
    1106:	42a9      	cmp	r1, r5
    1108:	d23a      	bcs.n	1180 <main+0x6f8>
    110a:	6819      	ldr	r1, [r3, #0]
    110c:	3101      	adds	r1, #1
    110e:	6019      	str	r1, [r3, #0]
    1110:	681d      	ldr	r5, [r3, #0]
    1112:	2d01      	cmp	r5, #1
    1114:	d11f      	bne.n	1156 <main+0x6ce>
    1116:	1c11      	adds	r1, r2, #0
    1118:	1c20      	adds	r0, r4, #0
    111a:	2203      	movs	r2, #3
    111c:	f7ff f851 	bl	1c2 <mbus_remote_register_write>
    1120:	2064      	movs	r0, #100	; 0x64
    1122:	f7fe ffb8 	bl	96 <delay>
    1126:	2064      	movs	r0, #100	; 0x64
    1128:	f7fe ffb5 	bl	96 <delay>
    112c:	23a0      	movs	r3, #160	; 0xa0
    112e:	061b      	lsls	r3, r3, #24
    1130:	681b      	ldr	r3, [r3, #0]
    1132:	401f      	ands	r7, r3
    1134:	4b50      	ldr	r3, [pc, #320]	; (1278 <main+0x7f0>)
    1136:	601f      	str	r7, [r3, #0]
    1138:	f7ff f856 	bl	1e8 <batadc_reset>
    113c:	2064      	movs	r0, #100	; 0x64
    113e:	f7fe ffaa 	bl	96 <delay>
    1142:	f7ff f869 	bl	218 <radio_power_on>
    1146:	2002      	movs	r0, #2
    1148:	1c29      	adds	r1, r5, #0
    114a:	1c2a      	adds	r2, r5, #0
    114c:	f7fe ffb8 	bl	c0 <set_wakeup_timer>
    1150:	f7ff f820 	bl	194 <mbus_sleep_all>
    1154:	e7fe      	b.n	1154 <main+0x6cc>
    1156:	4b48      	ldr	r3, [pc, #288]	; (1278 <main+0x7f0>)
    1158:	1c10      	adds	r0, r2, #0
    115a:	6819      	ldr	r1, [r3, #0]
    115c:	4b47      	ldr	r3, [pc, #284]	; (127c <main+0x7f4>)
    115e:	18c9      	adds	r1, r1, r3
    1160:	f7ff f97c 	bl	45c <send_radio_data_ppm>
    1164:	20fa      	movs	r0, #250	; 0xfa
    1166:	0100      	lsls	r0, r0, #4
    1168:	f7fe ff95 	bl	96 <delay>
    116c:	4c36      	ldr	r4, [pc, #216]	; (1248 <main+0x7c0>)
    116e:	2101      	movs	r1, #1
    1170:	6820      	ldr	r0, [r4, #0]
    1172:	1c0a      	adds	r2, r1, #0
    1174:	b280      	uxth	r0, r0
    1176:	f7fe ffa3 	bl	c0 <set_wakeup_timer>
    117a:	f7ff f80b 	bl	194 <mbus_sleep_all>
    117e:	e7fe      	b.n	117e <main+0x6f6>
    1180:	2001      	movs	r0, #1
    1182:	493b      	ldr	r1, [pc, #236]	; (1270 <main+0x7e8>)
    1184:	601a      	str	r2, [r3, #0]
    1186:	f7ff f969 	bl	45c <send_radio_data_ppm>
    118a:	f7ff f839 	bl	200 <batadc_resetrelease>
    118e:	e64b      	b.n	e28 <main+0x3a0>
    1190:	2c13      	cmp	r4, #19
    1192:	d14b      	bne.n	122c <main+0x7a4>
    1194:	21ff      	movs	r1, #255	; 0xff
    1196:	4c2c      	ldr	r4, [pc, #176]	; (1248 <main+0x7c0>)
    1198:	400b      	ands	r3, r1
    119a:	2064      	movs	r0, #100	; 0x64
    119c:	6023      	str	r3, [r4, #0]
    119e:	f7fe ff7a 	bl	96 <delay>
    11a2:	4b37      	ldr	r3, [pc, #220]	; (1280 <main+0x7f8>)
    11a4:	093f      	lsrs	r7, r7, #4
    11a6:	017a      	lsls	r2, r7, #5
    11a8:	4936      	ldr	r1, [pc, #216]	; (1284 <main+0x7fc>)
    11aa:	681f      	ldr	r7, [r3, #0]
    11ac:	2004      	movs	r0, #4
    11ae:	400f      	ands	r7, r1
    11b0:	4317      	orrs	r7, r2
    11b2:	220f      	movs	r2, #15
    11b4:	601f      	str	r7, [r3, #0]
    11b6:	4016      	ands	r6, r2
    11b8:	0272      	lsls	r2, r6, #9
    11ba:	4933      	ldr	r1, [pc, #204]	; (1288 <main+0x800>)
    11bc:	681e      	ldr	r6, [r3, #0]
    11be:	400e      	ands	r6, r1
    11c0:	4316      	orrs	r6, r2
    11c2:	601e      	str	r6, [r3, #0]
    11c4:	681a      	ldr	r2, [r3, #0]
    11c6:	2100      	movs	r1, #0
    11c8:	f7fe fffb 	bl	1c2 <mbus_remote_register_write>
    11cc:	2064      	movs	r0, #100	; 0x64
    11ce:	f7fe ff62 	bl	96 <delay>
    11d2:	4b20      	ldr	r3, [pc, #128]	; (1254 <main+0x7cc>)
    11d4:	21ff      	movs	r1, #255	; 0xff
    11d6:	681a      	ldr	r2, [r3, #0]
    11d8:	400d      	ands	r5, r1
    11da:	42aa      	cmp	r2, r5
    11dc:	d222      	bcs.n	1224 <main+0x79c>
    11de:	681a      	ldr	r2, [r3, #0]
    11e0:	3201      	adds	r2, #1
    11e2:	601a      	str	r2, [r3, #0]
    11e4:	681d      	ldr	r5, [r3, #0]
    11e6:	2d01      	cmp	r5, #1
    11e8:	d109      	bne.n	11fe <main+0x776>
    11ea:	f7ff f815 	bl	218 <radio_power_on>
    11ee:	2002      	movs	r0, #2
    11f0:	1c29      	adds	r1, r5, #0
    11f2:	1c2a      	adds	r2, r5, #0
    11f4:	f7fe ff64 	bl	c0 <set_wakeup_timer>
    11f8:	f7fe ffcc 	bl	194 <mbus_sleep_all>
    11fc:	e7fe      	b.n	11fc <main+0x774>
    11fe:	6819      	ldr	r1, [r3, #0]
    1200:	4a17      	ldr	r2, [pc, #92]	; (1260 <main+0x7d8>)
    1202:	2000      	movs	r0, #0
    1204:	1889      	adds	r1, r1, r2
    1206:	f7ff f929 	bl	45c <send_radio_data_ppm>
    120a:	20fa      	movs	r0, #250	; 0xfa
    120c:	0100      	lsls	r0, r0, #4
    120e:	f7fe ff42 	bl	96 <delay>
    1212:	6820      	ldr	r0, [r4, #0]
    1214:	2101      	movs	r1, #1
    1216:	b280      	uxth	r0, r0
    1218:	1c0a      	adds	r2, r1, #0
    121a:	f7fe ff51 	bl	c0 <set_wakeup_timer>
    121e:	f7fe ffb9 	bl	194 <mbus_sleep_all>
    1222:	e7fe      	b.n	1222 <main+0x79a>
    1224:	2200      	movs	r2, #0
    1226:	601a      	str	r2, [r3, #0]
    1228:	2001      	movs	r0, #1
    122a:	e5fa      	b.n	e22 <main+0x39a>
    122c:	f7ff f978 	bl	520 <operation_run>
    1230:	e7fc      	b.n	122c <main+0x7a4>
    1232:	46c0      	nop			; (mov r8, r8)
    1234:	000012c4 	.word	0x000012c4
    1238:	000012ec 	.word	0x000012ec
    123c:	00001300 	.word	0x00001300
    1240:	000012bc 	.word	0x000012bc
    1244:	000012f4 	.word	0x000012f4
    1248:	000012dc 	.word	0x000012dc
    124c:	000012c0 	.word	0x000012c0
    1250:	000012d8 	.word	0x000012d8
    1254:	000012f8 	.word	0x000012f8
    1258:	00001304 	.word	0x00001304
    125c:	000012e4 	.word	0x000012e4
    1260:	00abc000 	.word	0x00abc000
    1264:	000fffff 	.word	0x000fffff
    1268:	0000130c 	.word	0x0000130c
    126c:	00002ee0 	.word	0x00002ee0
    1270:	00faf000 	.word	0x00faf000
    1274:	000012d0 	.word	0x000012d0
    1278:	000012d4 	.word	0x000012d4
    127c:	00bbb000 	.word	0x00bbb000
    1280:	0000128c 	.word	0x0000128c
    1284:	fffffe1f 	.word	0xfffffe1f
    1288:	ffffe1ff 	.word	0xffffe1ff
