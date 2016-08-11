
Snail_ondemand_inhee_32/Snail_ondemand_inhee_32.elf:     file format elf32-littlearm


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
  40:	0000099d 	.word	0x0000099d
  44:	000009a9 	.word	0x000009a9
  48:	000009b5 	.word	0x000009b5
  4c:	000009cd 	.word	0x000009cd
  50:	000009e5 	.word	0x000009e5
  54:	000009fd 	.word	0x000009fd
  58:	00000a15 	.word	0x00000a15
  5c:	00000a2d 	.word	0x00000a2d
  60:	00000a45 	.word	0x00000a45
  64:	00000a5d 	.word	0x00000a5d
  68:	00000a75 	.word	0x00000a75
  6c:	00000a85 	.word	0x00000a85
  70:	00000a95 	.word	0x00000a95
  74:	00000aa5 	.word	0x00000aa5
  78:	00000ab5 	.word	0x00000ab5
  7c:	00000000 	.word	0x00000000

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 fd18 	bl	ac4 <main>
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
 2c4:	0000135c 	.word	0x0000135c
 2c8:	000012e4 	.word	0x000012e4
 2cc:	000012ec 	.word	0x000012ec
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
 364:	0000135c 	.word	0x0000135c
 368:	000012ec 	.word	0x000012ec
 36c:	fff7ffff 	.word	0xfff7ffff
 370:	000012e4 	.word	0x000012e4

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
 3a8:	00001304 	.word	0x00001304

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
 3c8:	000012fc 	.word	0x000012fc

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
 3f0:	000012fc 	.word	0x000012fc
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
 428:	000012fc 	.word	0x000012fc
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
 458:	0000135c 	.word	0x0000135c

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
 50c:	00001fe0 	.word	0x00001fe0
 510:	0000130c 	.word	0x0000130c
 514:	000012e4 	.word	0x000012e4
 518:	00001334 	.word	0x00001334
 51c:	fafafafa 	.word	0xfafafafa

Disassembly of section .text.operation_run:

00000520 <operation_run>:
 520:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
 522:	4be5      	ldr	r3, [pc, #916]	; (8b8 <operation_run+0x398>)
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
 53e:	4bdf      	ldr	r3, [pc, #892]	; (8bc <operation_run+0x39c>)
 540:	2064      	movs	r0, #100	; 0x64
 542:	681a      	ldr	r2, [r3, #0]
 544:	4bde      	ldr	r3, [pc, #888]	; (8c0 <operation_run+0x3a0>)
 546:	601a      	str	r2, [r3, #0]
 548:	f7ff fda5 	bl	96 <delay>
 54c:	2006      	movs	r0, #6
 54e:	1c21      	adds	r1, r4, #0
 550:	2209      	movs	r2, #9
 552:	f7ff fe36 	bl	1c2 <mbus_remote_register_write>
 556:	4bdb      	ldr	r3, [pc, #876]	; (8c4 <operation_run+0x3a4>)
 558:	601d      	str	r5, [r3, #0]
 55a:	4bdb      	ldr	r3, [pc, #876]	; (8c8 <operation_run+0x3a8>)
 55c:	681b      	ldr	r3, [r3, #0]
 55e:	2b00      	cmp	r3, #0
 560:	d104      	bne.n	56c <operation_run+0x4c>
 562:	4bda      	ldr	r3, [pc, #872]	; (8cc <operation_run+0x3ac>)
 564:	681b      	ldr	r3, [r3, #0]
 566:	191b      	adds	r3, r3, r4
 568:	2b04      	cmp	r3, #4
 56a:	d801      	bhi.n	570 <operation_run+0x50>
 56c:	f7ff fe54 	bl	218 <radio_power_on>
 570:	4bd7      	ldr	r3, [pc, #860]	; (8d0 <operation_run+0x3b0>)
 572:	4ad8      	ldr	r2, [pc, #864]	; (8d4 <operation_run+0x3b4>)
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
 59e:	4bcc      	ldr	r3, [pc, #816]	; (8d0 <operation_run+0x3b0>)
 5a0:	4acd      	ldr	r2, [pc, #820]	; (8d8 <operation_run+0x3b8>)
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
 5cc:	4bc3      	ldr	r3, [pc, #780]	; (8dc <operation_run+0x3bc>)
 5ce:	2280      	movs	r2, #128	; 0x80
 5d0:	6819      	ldr	r1, [r3, #0]
 5d2:	03d2      	lsls	r2, r2, #15
 5d4:	430a      	orrs	r2, r1
 5d6:	601a      	str	r2, [r3, #0]
 5d8:	6819      	ldr	r1, [r3, #0]
 5da:	4ac1      	ldr	r2, [pc, #772]	; (8e0 <operation_run+0x3c0>)
 5dc:	2005      	movs	r0, #5
 5de:	400a      	ands	r2, r1
 5e0:	601a      	str	r2, [r3, #0]
 5e2:	681a      	ldr	r2, [r3, #0]
 5e4:	210e      	movs	r1, #14
 5e6:	f7ff fdec 	bl	1c2 <mbus_remote_register_write>
 5ea:	e1ce      	b.n	98a <operation_run+0x46a>
 5ec:	781a      	ldrb	r2, [r3, #0]
 5ee:	2a03      	cmp	r2, #3
 5f0:	d12a      	bne.n	648 <operation_run+0x128>
 5f2:	4dbc      	ldr	r5, [pc, #752]	; (8e4 <operation_run+0x3c4>)
 5f4:	2300      	movs	r3, #0
 5f6:	702b      	strb	r3, [r5, #0]
 5f8:	4bb8      	ldr	r3, [pc, #736]	; (8dc <operation_run+0x3bc>)
 5fa:	4abb      	ldr	r2, [pc, #748]	; (8e8 <operation_run+0x3c8>)
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
 616:	4aab      	ldr	r2, [pc, #684]	; (8c4 <operation_run+0x3a4>)
 618:	2300      	movs	r3, #0
 61a:	702b      	strb	r3, [r5, #0]
 61c:	6013      	str	r3, [r2, #0]
 61e:	e00f      	b.n	640 <operation_run+0x120>
 620:	2064      	movs	r0, #100	; 0x64
 622:	3c01      	subs	r4, #1
 624:	f7ff fd37 	bl	96 <delay>
 628:	2c00      	cmp	r4, #0
 62a:	d1f1      	bne.n	610 <operation_run+0xf0>
 62c:	49af      	ldr	r1, [pc, #700]	; (8ec <operation_run+0x3cc>)
 62e:	20aa      	movs	r0, #170	; 0xaa
 630:	f7ff fd86 	bl	140 <mbus_write_message32>
 634:	f7ff feba 	bl	3ac <temp_sensor_disable>
 638:	4ba2      	ldr	r3, [pc, #648]	; (8c4 <operation_run+0x3a4>)
 63a:	681a      	ldr	r2, [r3, #0]
 63c:	3201      	adds	r2, #1
 63e:	601a      	str	r2, [r3, #0]
 640:	4b9d      	ldr	r3, [pc, #628]	; (8b8 <operation_run+0x398>)
 642:	2204      	movs	r2, #4
 644:	701a      	strb	r2, [r3, #0]
 646:	e1a0      	b.n	98a <operation_run+0x46a>
 648:	781b      	ldrb	r3, [r3, #0]
 64a:	2b04      	cmp	r3, #4
 64c:	d000      	beq.n	650 <operation_run+0x130>
 64e:	e196      	b.n	97e <operation_run+0x45e>
 650:	2100      	movs	r1, #0
 652:	2203      	movs	r2, #3
 654:	2007      	movs	r0, #7
 656:	f7ff fdb4 	bl	1c2 <mbus_remote_register_write>
 65a:	2064      	movs	r0, #100	; 0x64
 65c:	f7ff fd1b 	bl	96 <delay>
 660:	23a0      	movs	r3, #160	; 0xa0
 662:	061b      	lsls	r3, r3, #24
 664:	681a      	ldr	r2, [r3, #0]
 666:	4ba2      	ldr	r3, [pc, #648]	; (8f0 <operation_run+0x3d0>)
 668:	b2d2      	uxtb	r2, r2
 66a:	2064      	movs	r0, #100	; 0x64
 66c:	601a      	str	r2, [r3, #0]
 66e:	4c93      	ldr	r4, [pc, #588]	; (8bc <operation_run+0x39c>)
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
 696:	4b8b      	ldr	r3, [pc, #556]	; (8c4 <operation_run+0x3a4>)
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
 6b0:	4b90      	ldr	r3, [pc, #576]	; (8f4 <operation_run+0x3d4>)
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
 6d8:	4b77      	ldr	r3, [pc, #476]	; (8b8 <operation_run+0x398>)
 6da:	2200      	movs	r2, #0
 6dc:	4c7b      	ldr	r4, [pc, #492]	; (8cc <operation_run+0x3ac>)
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
 70a:	4e6d      	ldr	r6, [pc, #436]	; (8c0 <operation_run+0x3a0>)
 70c:	20cc      	movs	r0, #204	; 0xcc
 70e:	6831      	ldr	r1, [r6, #0]
 710:	f7ff fd16 	bl	140 <mbus_write_message32>
 714:	2064      	movs	r0, #100	; 0x64
 716:	f7ff fcbe 	bl	96 <delay>
 71a:	4f75      	ldr	r7, [pc, #468]	; (8f0 <operation_run+0x3d0>)
 71c:	20cc      	movs	r0, #204	; 0xcc
 71e:	6839      	ldr	r1, [r7, #0]
 720:	f7ff fd0e 	bl	140 <mbus_write_message32>
 724:	2064      	movs	r0, #100	; 0x64
 726:	f7ff fcb6 	bl	96 <delay>
 72a:	6823      	ldr	r3, [r4, #0]
 72c:	3301      	adds	r3, #1
 72e:	6023      	str	r3, [r4, #0]
 730:	4c71      	ldr	r4, [pc, #452]	; (8f8 <operation_run+0x3d8>)
 732:	4b72      	ldr	r3, [pc, #456]	; (8fc <operation_run+0x3dc>)
 734:	6822      	ldr	r2, [r4, #0]
 736:	429a      	cmp	r2, r3
 738:	d84f      	bhi.n	7da <operation_run+0x2ba>
 73a:	6823      	ldr	r3, [r4, #0]
 73c:	6832      	ldr	r2, [r6, #0]
 73e:	4970      	ldr	r1, [pc, #448]	; (900 <operation_run+0x3e0>)
 740:	0c12      	lsrs	r2, r2, #16
 742:	009b      	lsls	r3, r3, #2
 744:	505a      	str	r2, [r3, r1]
 746:	6823      	ldr	r3, [r4, #0]
 748:	6832      	ldr	r2, [r6, #0]
 74a:	4e6e      	ldr	r6, [pc, #440]	; (904 <operation_run+0x3e4>)
 74c:	009b      	lsls	r3, r3, #2
 74e:	519a      	str	r2, [r3, r6]
 750:	6821      	ldr	r1, [r4, #0]
 752:	20dd      	movs	r0, #221	; 0xdd
 754:	f7ff fcf4 	bl	140 <mbus_write_message32>
 758:	2064      	movs	r0, #100	; 0x64
 75a:	f7ff fc9c 	bl	96 <delay>
 75e:	6823      	ldr	r3, [r4, #0]
 760:	4a67      	ldr	r2, [pc, #412]	; (900 <operation_run+0x3e0>)
 762:	009b      	lsls	r3, r3, #2
 764:	5899      	ldr	r1, [r3, r2]
 766:	20dd      	movs	r0, #221	; 0xdd
 768:	f7ff fcea 	bl	140 <mbus_write_message32>
 76c:	2064      	movs	r0, #100	; 0x64
 76e:	f7ff fc92 	bl	96 <delay>
 772:	6823      	ldr	r3, [r4, #0]
 774:	20dd      	movs	r0, #221	; 0xdd
 776:	009b      	lsls	r3, r3, #2
 778:	5999      	ldr	r1, [r3, r6]
 77a:	f7ff fce1 	bl	140 <mbus_write_message32>
 77e:	2064      	movs	r0, #100	; 0x64
 780:	f7ff fc89 	bl	96 <delay>
 784:	6823      	ldr	r3, [r4, #0]
 786:	495e      	ldr	r1, [pc, #376]	; (900 <operation_run+0x3e0>)
 788:	3301      	adds	r3, #1
 78a:	6023      	str	r3, [r4, #0]
 78c:	6823      	ldr	r3, [r4, #0]
 78e:	683a      	ldr	r2, [r7, #0]
 790:	009b      	lsls	r3, r3, #2
 792:	505a      	str	r2, [r3, r1]
 794:	6823      	ldr	r3, [r4, #0]
 796:	20dd      	movs	r0, #221	; 0xdd
 798:	009b      	lsls	r3, r3, #2
 79a:	519d      	str	r5, [r3, r6]
 79c:	6821      	ldr	r1, [r4, #0]
 79e:	f7ff fccf 	bl	140 <mbus_write_message32>
 7a2:	2064      	movs	r0, #100	; 0x64
 7a4:	f7ff fc77 	bl	96 <delay>
 7a8:	6823      	ldr	r3, [r4, #0]
 7aa:	4a55      	ldr	r2, [pc, #340]	; (900 <operation_run+0x3e0>)
 7ac:	009b      	lsls	r3, r3, #2
 7ae:	5899      	ldr	r1, [r3, r2]
 7b0:	20dd      	movs	r0, #221	; 0xdd
 7b2:	f7ff fcc5 	bl	140 <mbus_write_message32>
 7b6:	2064      	movs	r0, #100	; 0x64
 7b8:	f7ff fc6d 	bl	96 <delay>
 7bc:	6823      	ldr	r3, [r4, #0]
 7be:	20dd      	movs	r0, #221	; 0xdd
 7c0:	009b      	lsls	r3, r3, #2
 7c2:	5999      	ldr	r1, [r3, r6]
 7c4:	f7ff fcbc 	bl	140 <mbus_write_message32>
 7c8:	2064      	movs	r0, #100	; 0x64
 7ca:	f7ff fc64 	bl	96 <delay>
 7ce:	6822      	ldr	r2, [r4, #0]
 7d0:	4b4d      	ldr	r3, [pc, #308]	; (908 <operation_run+0x3e8>)
 7d2:	601a      	str	r2, [r3, #0]
 7d4:	6823      	ldr	r3, [r4, #0]
 7d6:	3301      	adds	r3, #1
 7d8:	6023      	str	r3, [r4, #0]
 7da:	4b3b      	ldr	r3, [pc, #236]	; (8c8 <operation_run+0x3a8>)
 7dc:	681b      	ldr	r3, [r3, #0]
 7de:	2b00      	cmp	r3, #0
 7e0:	d01d      	beq.n	81e <operation_run+0x2fe>
 7e2:	9901      	ldr	r1, [sp, #4]
 7e4:	2000      	movs	r0, #0
 7e6:	f7ff fe39 	bl	45c <send_radio_data_ppm>
 7ea:	20fa      	movs	r0, #250	; 0xfa
 7ec:	0100      	lsls	r0, r0, #4
 7ee:	f7ff fc52 	bl	96 <delay>
 7f2:	1c29      	adds	r1, r5, #0
 7f4:	2000      	movs	r0, #0
 7f6:	f7ff fe31 	bl	45c <send_radio_data_ppm>
 7fa:	20fa      	movs	r0, #250	; 0xfa
 7fc:	0100      	lsls	r0, r0, #4
 7fe:	f7ff fc4a 	bl	96 <delay>
 802:	4b2f      	ldr	r3, [pc, #188]	; (8c0 <operation_run+0x3a0>)
 804:	2000      	movs	r0, #0
 806:	6819      	ldr	r1, [r3, #0]
 808:	f7ff fe28 	bl	45c <send_radio_data_ppm>
 80c:	20fa      	movs	r0, #250	; 0xfa
 80e:	0100      	lsls	r0, r0, #4
 810:	f7ff fc41 	bl	96 <delay>
 814:	4b36      	ldr	r3, [pc, #216]	; (8f0 <operation_run+0x3d0>)
 816:	2000      	movs	r0, #0
 818:	6819      	ldr	r1, [r3, #0]
 81a:	f7ff fe1f 	bl	45c <send_radio_data_ppm>
 81e:	4b2b      	ldr	r3, [pc, #172]	; (8cc <operation_run+0x3ac>)
 820:	681b      	ldr	r3, [r3, #0]
 822:	2b04      	cmp	r3, #4
 824:	d809      	bhi.n	83a <operation_run+0x31a>
 826:	20fa      	movs	r0, #250	; 0xfa
 828:	0100      	lsls	r0, r0, #4
 82a:	f7ff fc34 	bl	96 <delay>
 82e:	2001      	movs	r0, #1
 830:	4936      	ldr	r1, [pc, #216]	; (90c <operation_run+0x3ec>)
 832:	f7ff fe13 	bl	45c <send_radio_data_ppm>
 836:	4b36      	ldr	r3, [pc, #216]	; (910 <operation_run+0x3f0>)
 838:	e000      	b.n	83c <operation_run+0x31c>
 83a:	4b36      	ldr	r3, [pc, #216]	; (914 <operation_run+0x3f4>)
 83c:	6818      	ldr	r0, [r3, #0]
 83e:	2101      	movs	r1, #1
 840:	b280      	uxth	r0, r0
 842:	1c0a      	adds	r2, r1, #0
 844:	f7ff fc3c 	bl	c0 <set_wakeup_timer>
 848:	4b33      	ldr	r3, [pc, #204]	; (918 <operation_run+0x3f8>)
 84a:	781b      	ldrb	r3, [r3, #0]
 84c:	2b00      	cmp	r3, #0
 84e:	d004      	beq.n	85a <operation_run+0x33a>
 850:	4b32      	ldr	r3, [pc, #200]	; (91c <operation_run+0x3fc>)
 852:	2200      	movs	r2, #0
 854:	701a      	strb	r2, [r3, #0]
 856:	f7ff fd3d 	bl	2d4 <radio_power_off>
 85a:	2101      	movs	r1, #1
 85c:	2219      	movs	r2, #25
 85e:	2006      	movs	r0, #6
 860:	f7ff fcaf 	bl	1c2 <mbus_remote_register_write>
 864:	4a22      	ldr	r2, [pc, #136]	; (8f0 <operation_run+0x3d0>)
 866:	4b1a      	ldr	r3, [pc, #104]	; (8d0 <operation_run+0x3b0>)
 868:	6811      	ldr	r1, [r2, #0]
 86a:	2900      	cmp	r1, #0
 86c:	d05a      	beq.n	924 <operation_run+0x404>
 86e:	4c2c      	ldr	r4, [pc, #176]	; (920 <operation_run+0x400>)
 870:	6810      	ldr	r0, [r2, #0]
 872:	6821      	ldr	r1, [r4, #0]
 874:	4288      	cmp	r0, r1
 876:	d255      	bcs.n	924 <operation_run+0x404>
 878:	6818      	ldr	r0, [r3, #0]
 87a:	4917      	ldr	r1, [pc, #92]	; (8d8 <operation_run+0x3b8>)
 87c:	4001      	ands	r1, r0
 87e:	6019      	str	r1, [r3, #0]
 880:	6818      	ldr	r0, [r3, #0]
 882:	4914      	ldr	r1, [pc, #80]	; (8d4 <operation_run+0x3b4>)
 884:	4001      	ands	r1, r0
 886:	6019      	str	r1, [r3, #0]
 888:	6819      	ldr	r1, [r3, #0]
 88a:	2002      	movs	r0, #2
 88c:	4381      	bics	r1, r0
 88e:	6019      	str	r1, [r3, #0]
 890:	6819      	ldr	r1, [r3, #0]
 892:	2001      	movs	r0, #1
 894:	4381      	bics	r1, r0
 896:	6019      	str	r1, [r3, #0]
 898:	6811      	ldr	r1, [r2, #0]
 89a:	20bb      	movs	r0, #187	; 0xbb
 89c:	f7ff fc50 	bl	140 <mbus_write_message32>
 8a0:	2064      	movs	r0, #100	; 0x64
 8a2:	f7ff fbf8 	bl	96 <delay>
 8a6:	6821      	ldr	r1, [r4, #0]
 8a8:	20bb      	movs	r0, #187	; 0xbb
 8aa:	f7ff fc49 	bl	140 <mbus_write_message32>
 8ae:	2064      	movs	r0, #100	; 0x64
 8b0:	f7ff fbf1 	bl	96 <delay>
 8b4:	e048      	b.n	948 <operation_run+0x428>
 8b6:	46c0      	nop			; (mov r8, r8)
 8b8:	0000132c 	.word	0x0000132c
 8bc:	a0000004 	.word	0xa0000004
 8c0:	00001344 	.word	0x00001344
 8c4:	00001348 	.word	0x00001348
 8c8:	00001320 	.word	0x00001320
 8cc:	00001318 	.word	0x00001318
 8d0:	00001304 	.word	0x00001304
 8d4:	fffff7ff 	.word	0xfffff7ff
 8d8:	ffffefff 	.word	0xffffefff
 8dc:	000012fc 	.word	0x000012fc
 8e0:	ffdfffff 	.word	0xffdfffff
 8e4:	00001334 	.word	0x00001334
 8e8:	fffeffff 	.word	0xfffeffff
 8ec:	fafafafa 	.word	0xfafafafa
 8f0:	00001328 	.word	0x00001328
 8f4:	00001358 	.word	0x00001358
 8f8:	00001354 	.word	0x00001354
 8fc:	0000018f 	.word	0x0000018f
 900:	00001360 	.word	0x00001360
 904:	000019a0 	.word	0x000019a0
 908:	00001310 	.word	0x00001310
 90c:	00faf000 	.word	0x00faf000
 910:	00001330 	.word	0x00001330
 914:	00001350 	.word	0x00001350
 918:	0000135c 	.word	0x0000135c
 91c:	0000130c 	.word	0x0000130c
 920:	00001324 	.word	0x00001324
 924:	6819      	ldr	r1, [r3, #0]
 926:	2280      	movs	r2, #128	; 0x80
 928:	0152      	lsls	r2, r2, #5
 92a:	430a      	orrs	r2, r1
 92c:	601a      	str	r2, [r3, #0]
 92e:	6819      	ldr	r1, [r3, #0]
 930:	2280      	movs	r2, #128	; 0x80
 932:	0112      	lsls	r2, r2, #4
 934:	430a      	orrs	r2, r1
 936:	601a      	str	r2, [r3, #0]
 938:	681a      	ldr	r2, [r3, #0]
 93a:	2102      	movs	r1, #2
 93c:	430a      	orrs	r2, r1
 93e:	601a      	str	r2, [r3, #0]
 940:	681a      	ldr	r2, [r3, #0]
 942:	2101      	movs	r1, #1
 944:	430a      	orrs	r2, r1
 946:	601a      	str	r2, [r3, #0]
 948:	4b10      	ldr	r3, [pc, #64]	; (98c <operation_run+0x46c>)
 94a:	2005      	movs	r0, #5
 94c:	681a      	ldr	r2, [r3, #0]
 94e:	2112      	movs	r1, #18
 950:	f7ff fc37 	bl	1c2 <mbus_remote_register_write>
 954:	2064      	movs	r0, #100	; 0x64
 956:	f7ff fb9e 	bl	96 <delay>
 95a:	4b0d      	ldr	r3, [pc, #52]	; (990 <operation_run+0x470>)
 95c:	681a      	ldr	r2, [r3, #0]
 95e:	2a00      	cmp	r2, #0
 960:	d00a      	beq.n	978 <operation_run+0x458>
 962:	4a0c      	ldr	r2, [pc, #48]	; (994 <operation_run+0x474>)
 964:	2132      	movs	r1, #50	; 0x32
 966:	6812      	ldr	r2, [r2, #0]
 968:	681b      	ldr	r3, [r3, #0]
 96a:	4099      	lsls	r1, r3
 96c:	428a      	cmp	r2, r1
 96e:	d903      	bls.n	978 <operation_run+0x458>
 970:	4b09      	ldr	r3, [pc, #36]	; (998 <operation_run+0x478>)
 972:	2200      	movs	r2, #0
 974:	701a      	strb	r2, [r3, #0]
 976:	e004      	b.n	982 <operation_run+0x462>
 978:	f7ff fc0c 	bl	194 <mbus_sleep_all>
 97c:	e7fe      	b.n	97c <operation_run+0x45c>
 97e:	f7ff fd25 	bl	3cc <temp_sensor_assert_reset>
 982:	f7ff fd39 	bl	3f8 <temp_power_off>
 986:	f7ff fd53 	bl	430 <operation_sleep_notimer>
 98a:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
 98c:	00001304 	.word	0x00001304
 990:	0000133c 	.word	0x0000133c
 994:	00001318 	.word	0x00001318
 998:	00001314 	.word	0x00001314

Disassembly of section .text.handler_ext_int_0:

0000099c <handler_ext_int_0>:
 99c:	4b01      	ldr	r3, [pc, #4]	; (9a4 <handler_ext_int_0+0x8>)
 99e:	2201      	movs	r2, #1
 9a0:	601a      	str	r2, [r3, #0]
 9a2:	4770      	bx	lr
 9a4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

000009a8 <handler_ext_int_1>:
 9a8:	4b01      	ldr	r3, [pc, #4]	; (9b0 <handler_ext_int_1+0x8>)
 9aa:	2202      	movs	r2, #2
 9ac:	601a      	str	r2, [r3, #0]
 9ae:	4770      	bx	lr
 9b0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

000009b4 <handler_ext_int_2>:
 9b4:	4b03      	ldr	r3, [pc, #12]	; (9c4 <handler_ext_int_2+0x10>)
 9b6:	2204      	movs	r2, #4
 9b8:	601a      	str	r2, [r3, #0]
 9ba:	4b03      	ldr	r3, [pc, #12]	; (9c8 <handler_ext_int_2+0x14>)
 9bc:	2210      	movs	r2, #16
 9be:	701a      	strb	r2, [r3, #0]
 9c0:	4770      	bx	lr
 9c2:	46c0      	nop			; (mov r8, r8)
 9c4:	e000e280 	.word	0xe000e280
 9c8:	00001334 	.word	0x00001334

Disassembly of section .text.handler_ext_int_3:

000009cc <handler_ext_int_3>:
 9cc:	4b03      	ldr	r3, [pc, #12]	; (9dc <handler_ext_int_3+0x10>)
 9ce:	2208      	movs	r2, #8
 9d0:	601a      	str	r2, [r3, #0]
 9d2:	4b03      	ldr	r3, [pc, #12]	; (9e0 <handler_ext_int_3+0x14>)
 9d4:	2211      	movs	r2, #17
 9d6:	701a      	strb	r2, [r3, #0]
 9d8:	4770      	bx	lr
 9da:	46c0      	nop			; (mov r8, r8)
 9dc:	e000e280 	.word	0xe000e280
 9e0:	00001334 	.word	0x00001334

Disassembly of section .text.handler_ext_int_4:

000009e4 <handler_ext_int_4>:
 9e4:	4b03      	ldr	r3, [pc, #12]	; (9f4 <handler_ext_int_4+0x10>)
 9e6:	2210      	movs	r2, #16
 9e8:	601a      	str	r2, [r3, #0]
 9ea:	4b03      	ldr	r3, [pc, #12]	; (9f8 <handler_ext_int_4+0x14>)
 9ec:	2212      	movs	r2, #18
 9ee:	701a      	strb	r2, [r3, #0]
 9f0:	4770      	bx	lr
 9f2:	46c0      	nop			; (mov r8, r8)
 9f4:	e000e280 	.word	0xe000e280
 9f8:	00001334 	.word	0x00001334

Disassembly of section .text.handler_ext_int_5:

000009fc <handler_ext_int_5>:
 9fc:	4b03      	ldr	r3, [pc, #12]	; (a0c <handler_ext_int_5+0x10>)
 9fe:	2220      	movs	r2, #32
 a00:	601a      	str	r2, [r3, #0]
 a02:	4b03      	ldr	r3, [pc, #12]	; (a10 <handler_ext_int_5+0x14>)
 a04:	2213      	movs	r2, #19
 a06:	701a      	strb	r2, [r3, #0]
 a08:	4770      	bx	lr
 a0a:	46c0      	nop			; (mov r8, r8)
 a0c:	e000e280 	.word	0xe000e280
 a10:	00001334 	.word	0x00001334

Disassembly of section .text.handler_ext_int_6:

00000a14 <handler_ext_int_6>:
 a14:	4b03      	ldr	r3, [pc, #12]	; (a24 <handler_ext_int_6+0x10>)
 a16:	2240      	movs	r2, #64	; 0x40
 a18:	601a      	str	r2, [r3, #0]
 a1a:	4b03      	ldr	r3, [pc, #12]	; (a28 <handler_ext_int_6+0x14>)
 a1c:	2214      	movs	r2, #20
 a1e:	701a      	strb	r2, [r3, #0]
 a20:	4770      	bx	lr
 a22:	46c0      	nop			; (mov r8, r8)
 a24:	e000e280 	.word	0xe000e280
 a28:	00001334 	.word	0x00001334

Disassembly of section .text.handler_ext_int_7:

00000a2c <handler_ext_int_7>:
 a2c:	4b03      	ldr	r3, [pc, #12]	; (a3c <handler_ext_int_7+0x10>)
 a2e:	2280      	movs	r2, #128	; 0x80
 a30:	601a      	str	r2, [r3, #0]
 a32:	4b03      	ldr	r3, [pc, #12]	; (a40 <handler_ext_int_7+0x14>)
 a34:	2215      	movs	r2, #21
 a36:	701a      	strb	r2, [r3, #0]
 a38:	4770      	bx	lr
 a3a:	46c0      	nop			; (mov r8, r8)
 a3c:	e000e280 	.word	0xe000e280
 a40:	00001334 	.word	0x00001334

Disassembly of section .text.handler_ext_int_8:

00000a44 <handler_ext_int_8>:
 a44:	4b03      	ldr	r3, [pc, #12]	; (a54 <handler_ext_int_8+0x10>)
 a46:	2280      	movs	r2, #128	; 0x80
 a48:	0052      	lsls	r2, r2, #1
 a4a:	601a      	str	r2, [r3, #0]
 a4c:	4b02      	ldr	r3, [pc, #8]	; (a58 <handler_ext_int_8+0x14>)
 a4e:	2216      	movs	r2, #22
 a50:	701a      	strb	r2, [r3, #0]
 a52:	4770      	bx	lr
 a54:	e000e280 	.word	0xe000e280
 a58:	00001334 	.word	0x00001334

Disassembly of section .text.handler_ext_int_9:

00000a5c <handler_ext_int_9>:
 a5c:	4b03      	ldr	r3, [pc, #12]	; (a6c <handler_ext_int_9+0x10>)
 a5e:	2280      	movs	r2, #128	; 0x80
 a60:	0092      	lsls	r2, r2, #2
 a62:	601a      	str	r2, [r3, #0]
 a64:	4b02      	ldr	r3, [pc, #8]	; (a70 <handler_ext_int_9+0x14>)
 a66:	2217      	movs	r2, #23
 a68:	701a      	strb	r2, [r3, #0]
 a6a:	4770      	bx	lr
 a6c:	e000e280 	.word	0xe000e280
 a70:	00001334 	.word	0x00001334

Disassembly of section .text.handler_ext_int_10:

00000a74 <handler_ext_int_10>:
 a74:	4b02      	ldr	r3, [pc, #8]	; (a80 <handler_ext_int_10+0xc>)
 a76:	2280      	movs	r2, #128	; 0x80
 a78:	00d2      	lsls	r2, r2, #3
 a7a:	601a      	str	r2, [r3, #0]
 a7c:	4770      	bx	lr
 a7e:	46c0      	nop			; (mov r8, r8)
 a80:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

00000a84 <handler_ext_int_11>:
 a84:	4b02      	ldr	r3, [pc, #8]	; (a90 <handler_ext_int_11+0xc>)
 a86:	2280      	movs	r2, #128	; 0x80
 a88:	0112      	lsls	r2, r2, #4
 a8a:	601a      	str	r2, [r3, #0]
 a8c:	4770      	bx	lr
 a8e:	46c0      	nop			; (mov r8, r8)
 a90:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

00000a94 <handler_ext_int_12>:
 a94:	4b02      	ldr	r3, [pc, #8]	; (aa0 <handler_ext_int_12+0xc>)
 a96:	2280      	movs	r2, #128	; 0x80
 a98:	0152      	lsls	r2, r2, #5
 a9a:	601a      	str	r2, [r3, #0]
 a9c:	4770      	bx	lr
 a9e:	46c0      	nop			; (mov r8, r8)
 aa0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

00000aa4 <handler_ext_int_13>:
 aa4:	4b02      	ldr	r3, [pc, #8]	; (ab0 <handler_ext_int_13+0xc>)
 aa6:	2280      	movs	r2, #128	; 0x80
 aa8:	0192      	lsls	r2, r2, #6
 aaa:	601a      	str	r2, [r3, #0]
 aac:	4770      	bx	lr
 aae:	46c0      	nop			; (mov r8, r8)
 ab0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

00000ab4 <handler_ext_int_14>:
 ab4:	4b02      	ldr	r3, [pc, #8]	; (ac0 <handler_ext_int_14+0xc>)
 ab6:	2280      	movs	r2, #128	; 0x80
 ab8:	01d2      	lsls	r2, r2, #7
 aba:	601a      	str	r2, [r3, #0]
 abc:	4770      	bx	lr
 abe:	46c0      	nop			; (mov r8, r8)
 ac0:	e000e280 	.word	0xe000e280

Disassembly of section .text.startup.main:

00000ac4 <main>:
     ac4:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
     ac6:	2100      	movs	r1, #0
     ac8:	2201      	movs	r2, #1
     aca:	2064      	movs	r0, #100	; 0x64
     acc:	f7ff faf8 	bl	c0 <set_wakeup_timer>
     ad0:	f7ff fb0c 	bl	ec <enable_reg_irq>
     ad4:	48f6      	ldr	r0, [pc, #984]	; (eb0 <main+0x3ec>)
     ad6:	f7ff fae7 	bl	a8 <config_timerwd>
     ada:	4af6      	ldr	r2, [pc, #984]	; (eb4 <main+0x3f0>)
     adc:	49f6      	ldr	r1, [pc, #984]	; (eb8 <main+0x3f4>)
     ade:	6813      	ldr	r3, [r2, #0]
     ae0:	428b      	cmp	r3, r1
     ae2:	d100      	bne.n	ae6 <main+0x22>
     ae4:	e181      	b.n	dea <main+0x326>
     ae6:	4bf5      	ldr	r3, [pc, #980]	; (ebc <main+0x3f8>)
     ae8:	2580      	movs	r5, #128	; 0x80
     aea:	6818      	ldr	r0, [r3, #0]
     aec:	016d      	lsls	r5, r5, #5
     aee:	4328      	orrs	r0, r5
     af0:	6018      	str	r0, [r3, #0]
     af2:	6818      	ldr	r0, [r3, #0]
     af4:	4cf2      	ldr	r4, [pc, #968]	; (ec0 <main+0x3fc>)
     af6:	26c0      	movs	r6, #192	; 0xc0
     af8:	4020      	ands	r0, r4
     afa:	2480      	movs	r4, #128	; 0x80
     afc:	01a4      	lsls	r4, r4, #6
     afe:	4320      	orrs	r0, r4
     b00:	6018      	str	r0, [r3, #0]
     b02:	681c      	ldr	r4, [r3, #0]
     b04:	48ef      	ldr	r0, [pc, #956]	; (ec4 <main+0x400>)
     b06:	02f6      	lsls	r6, r6, #11
     b08:	4020      	ands	r0, r4
     b0a:	2480      	movs	r4, #128	; 0x80
     b0c:	0224      	lsls	r4, r4, #8
     b0e:	4320      	orrs	r0, r4
     b10:	6018      	str	r0, [r3, #0]
     b12:	6818      	ldr	r0, [r3, #0]
     b14:	2400      	movs	r4, #0
     b16:	4330      	orrs	r0, r6
     b18:	6018      	str	r0, [r3, #0]
     b1a:	6818      	ldr	r0, [r3, #0]
     b1c:	4bea      	ldr	r3, [pc, #936]	; (ec8 <main+0x404>)
     b1e:	2707      	movs	r7, #7
     b20:	6018      	str	r0, [r3, #0]
     b22:	4bea      	ldr	r3, [pc, #936]	; (ecc <main+0x408>)
     b24:	2064      	movs	r0, #100	; 0x64
     b26:	701c      	strb	r4, [r3, #0]
     b28:	4be9      	ldr	r3, [pc, #932]	; (ed0 <main+0x40c>)
     b2a:	6011      	str	r1, [r2, #0]
     b2c:	601c      	str	r4, [r3, #0]
     b2e:	4be9      	ldr	r3, [pc, #932]	; (ed4 <main+0x410>)
     b30:	601c      	str	r4, [r3, #0]
     b32:	4be9      	ldr	r3, [pc, #932]	; (ed8 <main+0x414>)
     b34:	701c      	strb	r4, [r3, #0]
     b36:	4be9      	ldr	r3, [pc, #932]	; (edc <main+0x418>)
     b38:	601c      	str	r4, [r3, #0]
     b3a:	4be9      	ldr	r3, [pc, #932]	; (ee0 <main+0x41c>)
     b3c:	601c      	str	r4, [r3, #0]
     b3e:	f7ff faaa 	bl	96 <delay>
     b42:	2004      	movs	r0, #4
     b44:	f7ff fb1c 	bl	180 <mbus_enumerate>
     b48:	2064      	movs	r0, #100	; 0x64
     b4a:	f7ff faa4 	bl	96 <delay>
     b4e:	2005      	movs	r0, #5
     b50:	f7ff fb16 	bl	180 <mbus_enumerate>
     b54:	2064      	movs	r0, #100	; 0x64
     b56:	f7ff fa9e 	bl	96 <delay>
     b5a:	2006      	movs	r0, #6
     b5c:	f7ff fb10 	bl	180 <mbus_enumerate>
     b60:	2064      	movs	r0, #100	; 0x64
     b62:	f7ff fa98 	bl	96 <delay>
     b66:	2007      	movs	r0, #7
     b68:	f7ff fb0a 	bl	180 <mbus_enumerate>
     b6c:	2064      	movs	r0, #100	; 0x64
     b6e:	f7ff fa92 	bl	96 <delay>
     b72:	2151      	movs	r1, #81	; 0x51
     b74:	2209      	movs	r2, #9
     b76:	2007      	movs	r0, #7
     b78:	f7ff fb23 	bl	1c2 <mbus_remote_register_write>
     b7c:	2064      	movs	r0, #100	; 0x64
     b7e:	f7ff fa8a 	bl	96 <delay>
     b82:	2117      	movs	r1, #23
     b84:	4ad7      	ldr	r2, [pc, #860]	; (ee4 <main+0x420>)
     b86:	2007      	movs	r0, #7
     b88:	f7ff fb1b 	bl	1c2 <mbus_remote_register_write>
     b8c:	2064      	movs	r0, #100	; 0x64
     b8e:	f7ff fa82 	bl	96 <delay>
     b92:	2117      	movs	r1, #23
     b94:	4ad3      	ldr	r2, [pc, #844]	; (ee4 <main+0x420>)
     b96:	2007      	movs	r0, #7
     b98:	f7ff fb13 	bl	1c2 <mbus_remote_register_write>
     b9c:	2064      	movs	r0, #100	; 0x64
     b9e:	f7ff fa7a 	bl	96 <delay>
     ba2:	2118      	movs	r1, #24
     ba4:	4ad0      	ldr	r2, [pc, #832]	; (ee8 <main+0x424>)
     ba6:	2007      	movs	r0, #7
     ba8:	f7ff fb0b 	bl	1c2 <mbus_remote_register_write>
     bac:	2064      	movs	r0, #100	; 0x64
     bae:	f7ff fa72 	bl	96 <delay>
     bb2:	2119      	movs	r1, #25
     bb4:	4acd      	ldr	r2, [pc, #820]	; (eec <main+0x428>)
     bb6:	2007      	movs	r0, #7
     bb8:	f7ff fb03 	bl	1c2 <mbus_remote_register_write>
     bbc:	2064      	movs	r0, #100	; 0x64
     bbe:	f7ff fa6a 	bl	96 <delay>
     bc2:	211a      	movs	r1, #26
     bc4:	4aca      	ldr	r2, [pc, #808]	; (ef0 <main+0x42c>)
     bc6:	2007      	movs	r0, #7
     bc8:	f7ff fafb 	bl	1c2 <mbus_remote_register_write>
     bcc:	2064      	movs	r0, #100	; 0x64
     bce:	f7ff fa62 	bl	96 <delay>
     bd2:	2115      	movs	r1, #21
     bd4:	4ac7      	ldr	r2, [pc, #796]	; (ef4 <main+0x430>)
     bd6:	2007      	movs	r0, #7
     bd8:	f7ff faf3 	bl	1c2 <mbus_remote_register_write>
     bdc:	2064      	movs	r0, #100	; 0x64
     bde:	f7ff fa5a 	bl	96 <delay>
     be2:	2116      	movs	r1, #22
     be4:	4ac4      	ldr	r2, [pc, #784]	; (ef8 <main+0x434>)
     be6:	2007      	movs	r0, #7
     be8:	f7ff faeb 	bl	1c2 <mbus_remote_register_write>
     bec:	2064      	movs	r0, #100	; 0x64
     bee:	f7ff fa52 	bl	96 <delay>
     bf2:	22ab      	movs	r2, #171	; 0xab
     bf4:	2105      	movs	r1, #5
     bf6:	0112      	lsls	r2, r2, #4
     bf8:	2007      	movs	r0, #7
     bfa:	f7ff fae2 	bl	1c2 <mbus_remote_register_write>
     bfe:	2064      	movs	r0, #100	; 0x64
     c00:	f7ff fa49 	bl	96 <delay>
     c04:	2136      	movs	r1, #54	; 0x36
     c06:	2201      	movs	r2, #1
     c08:	2007      	movs	r0, #7
     c0a:	f7ff fada 	bl	1c2 <mbus_remote_register_write>
     c0e:	2064      	movs	r0, #100	; 0x64
     c10:	f7ff fa41 	bl	96 <delay>
     c14:	2006      	movs	r0, #6
     c16:	1c21      	adds	r1, r4, #0
     c18:	220f      	movs	r2, #15
     c1a:	f7ff fad2 	bl	1c2 <mbus_remote_register_write>
     c1e:	2006      	movs	r0, #6
     c20:	2101      	movs	r1, #1
     c22:	2209      	movs	r2, #9
     c24:	f7ff facd 	bl	1c2 <mbus_remote_register_write>
     c28:	2006      	movs	r0, #6
     c2a:	2103      	movs	r1, #3
     c2c:	4ab3      	ldr	r2, [pc, #716]	; (efc <main+0x438>)
     c2e:	f7ff fac8 	bl	1c2 <mbus_remote_register_write>
     c32:	2006      	movs	r0, #6
     c34:	2104      	movs	r1, #4
     c36:	4ab2      	ldr	r2, [pc, #712]	; (f00 <main+0x43c>)
     c38:	f7ff fac3 	bl	1c2 <mbus_remote_register_write>
     c3c:	1c2a      	adds	r2, r5, #0
     c3e:	2006      	movs	r0, #6
     c40:	2105      	movs	r1, #5
     c42:	f7ff fabe 	bl	1c2 <mbus_remote_register_write>
     c46:	2006      	movs	r0, #6
     c48:	2101      	movs	r1, #1
     c4a:	2209      	movs	r2, #9
     c4c:	f7ff fab9 	bl	1c2 <mbus_remote_register_write>
     c50:	4bac      	ldr	r3, [pc, #688]	; (f04 <main+0x440>)
     c52:	2005      	movs	r0, #5
     c54:	681a      	ldr	r2, [r3, #0]
     c56:	2119      	movs	r1, #25
     c58:	0e12      	lsrs	r2, r2, #24
     c5a:	0612      	lsls	r2, r2, #24
     c5c:	432a      	orrs	r2, r5
     c5e:	601a      	str	r2, [r3, #0]
     c60:	681a      	ldr	r2, [r3, #0]
     c62:	f7ff faae 	bl	1c2 <mbus_remote_register_write>
     c66:	4ba8      	ldr	r3, [pc, #672]	; (f08 <main+0x444>)
     c68:	4aa8      	ldr	r2, [pc, #672]	; (f0c <main+0x448>)
     c6a:	6819      	ldr	r1, [r3, #0]
     c6c:	2005      	movs	r0, #5
     c6e:	400a      	ands	r2, r1
     c70:	4316      	orrs	r6, r2
     c72:	601e      	str	r6, [r3, #0]
     c74:	681a      	ldr	r2, [r3, #0]
     c76:	210e      	movs	r1, #14
     c78:	f7ff faa3 	bl	1c2 <mbus_remote_register_write>
     c7c:	4ba4      	ldr	r3, [pc, #656]	; (f10 <main+0x44c>)
     c7e:	260f      	movs	r6, #15
     c80:	681a      	ldr	r2, [r3, #0]
     c82:	2005      	movs	r0, #5
     c84:	43b2      	bics	r2, r6
     c86:	433a      	orrs	r2, r7
     c88:	601a      	str	r2, [r3, #0]
     c8a:	681a      	ldr	r2, [r3, #0]
     c8c:	1c31      	adds	r1, r6, #0
     c8e:	f7ff fa98 	bl	1c2 <mbus_remote_register_write>
     c92:	4ba0      	ldr	r3, [pc, #640]	; (f14 <main+0x450>)
     c94:	2180      	movs	r1, #128	; 0x80
     c96:	681a      	ldr	r2, [r3, #0]
     c98:	0109      	lsls	r1, r1, #4
     c9a:	430a      	orrs	r2, r1
     c9c:	601a      	str	r2, [r3, #0]
     c9e:	681a      	ldr	r2, [r3, #0]
     ca0:	2005      	movs	r0, #5
     ca2:	4315      	orrs	r5, r2
     ca4:	601d      	str	r5, [r3, #0]
     ca6:	6819      	ldr	r1, [r3, #0]
     ca8:	2280      	movs	r2, #128	; 0x80
     caa:	0392      	lsls	r2, r2, #14
     cac:	430a      	orrs	r2, r1
     cae:	601a      	str	r2, [r3, #0]
     cb0:	681a      	ldr	r2, [r3, #0]
     cb2:	4983      	ldr	r1, [pc, #524]	; (ec0 <main+0x3fc>)
     cb4:	2504      	movs	r5, #4
     cb6:	400a      	ands	r2, r1
     cb8:	2180      	movs	r1, #128	; 0x80
     cba:	40b9      	lsls	r1, r7
     cbc:	430a      	orrs	r2, r1
     cbe:	601a      	str	r2, [r3, #0]
     cc0:	6819      	ldr	r1, [r3, #0]
     cc2:	4a95      	ldr	r2, [pc, #596]	; (f18 <main+0x454>)
     cc4:	400a      	ands	r2, r1
     cc6:	2180      	movs	r1, #128	; 0x80
     cc8:	0309      	lsls	r1, r1, #12
     cca:	430a      	orrs	r2, r1
     ccc:	601a      	str	r2, [r3, #0]
     cce:	681a      	ldr	r2, [r3, #0]
     cd0:	2112      	movs	r1, #18
     cd2:	f7ff fa76 	bl	1c2 <mbus_remote_register_write>
     cd6:	2005      	movs	r0, #5
     cd8:	22c0      	movs	r2, #192	; 0xc0
     cda:	4082      	lsls	r2, r0
     cdc:	2118      	movs	r1, #24
     cde:	f7ff fa70 	bl	1c2 <mbus_remote_register_write>
     ce2:	4b8e      	ldr	r3, [pc, #568]	; (f1c <main+0x458>)
     ce4:	4a8e      	ldr	r2, [pc, #568]	; (f20 <main+0x45c>)
     ce6:	6819      	ldr	r1, [r3, #0]
     ce8:	2004      	movs	r0, #4
     cea:	400a      	ands	r2, r1
     cec:	21bc      	movs	r1, #188	; 0xbc
     cee:	0409      	lsls	r1, r1, #16
     cf0:	430a      	orrs	r2, r1
     cf2:	601a      	str	r2, [r3, #0]
     cf4:	6819      	ldr	r1, [r3, #0]
     cf6:	4a8b      	ldr	r2, [pc, #556]	; (f24 <main+0x460>)
     cf8:	400a      	ands	r2, r1
     cfa:	601a      	str	r2, [r3, #0]
     cfc:	6819      	ldr	r1, [r3, #0]
     cfe:	4a8a      	ldr	r2, [pc, #552]	; (f28 <main+0x464>)
     d00:	400a      	ands	r2, r1
     d02:	2190      	movs	r1, #144	; 0x90
     d04:	0149      	lsls	r1, r1, #5
     d06:	430a      	orrs	r2, r1
     d08:	601a      	str	r2, [r3, #0]
     d0a:	6819      	ldr	r1, [r3, #0]
     d0c:	4a87      	ldr	r2, [pc, #540]	; (f2c <main+0x468>)
     d0e:	400a      	ands	r2, r1
     d10:	21c0      	movs	r1, #192	; 0xc0
     d12:	0289      	lsls	r1, r1, #10
     d14:	430a      	orrs	r2, r1
     d16:	601a      	str	r2, [r3, #0]
     d18:	681a      	ldr	r2, [r3, #0]
     d1a:	1c21      	adds	r1, r4, #0
     d1c:	f7ff fa51 	bl	1c2 <mbus_remote_register_write>
     d20:	4b83      	ldr	r3, [pc, #524]	; (f30 <main+0x46c>)
     d22:	211f      	movs	r1, #31
     d24:	681a      	ldr	r2, [r3, #0]
     d26:	2004      	movs	r0, #4
     d28:	438a      	bics	r2, r1
     d2a:	2110      	movs	r1, #16
     d2c:	430a      	orrs	r2, r1
     d2e:	601a      	str	r2, [r3, #0]
     d30:	6819      	ldr	r1, [r3, #0]
     d32:	4a80      	ldr	r2, [pc, #512]	; (f34 <main+0x470>)
     d34:	400a      	ands	r2, r1
     d36:	21b8      	movs	r1, #184	; 0xb8
     d38:	0089      	lsls	r1, r1, #2
     d3a:	430a      	orrs	r2, r1
     d3c:	601a      	str	r2, [r3, #0]
     d3e:	6819      	ldr	r1, [r3, #0]
     d40:	4a7d      	ldr	r2, [pc, #500]	; (f38 <main+0x474>)
     d42:	400a      	ands	r2, r1
     d44:	21a0      	movs	r1, #160	; 0xa0
     d46:	0249      	lsls	r1, r1, #9
     d48:	430a      	orrs	r2, r1
     d4a:	601a      	str	r2, [r3, #0]
     d4c:	681a      	ldr	r2, [r3, #0]
     d4e:	210b      	movs	r1, #11
     d50:	f7ff fa37 	bl	1c2 <mbus_remote_register_write>
     d54:	4b79      	ldr	r3, [pc, #484]	; (f3c <main+0x478>)
     d56:	22c0      	movs	r2, #192	; 0xc0
     d58:	6819      	ldr	r1, [r3, #0]
     d5a:	0352      	lsls	r2, r2, #13
     d5c:	430a      	orrs	r2, r1
     d5e:	601a      	str	r2, [r3, #0]
     d60:	6819      	ldr	r1, [r3, #0]
     d62:	4a77      	ldr	r2, [pc, #476]	; (f40 <main+0x47c>)
     d64:	2004      	movs	r0, #4
     d66:	400a      	ands	r2, r1
     d68:	2180      	movs	r1, #128	; 0x80
     d6a:	0109      	lsls	r1, r1, #4
     d6c:	430a      	orrs	r2, r1
     d6e:	601a      	str	r2, [r3, #0]
     d70:	681a      	ldr	r2, [r3, #0]
     d72:	217f      	movs	r1, #127	; 0x7f
     d74:	438a      	bics	r2, r1
     d76:	2160      	movs	r1, #96	; 0x60
     d78:	430a      	orrs	r2, r1
     d7a:	601a      	str	r2, [r3, #0]
     d7c:	681a      	ldr	r2, [r3, #0]
     d7e:	2101      	movs	r1, #1
     d80:	f7ff fa1f 	bl	1c2 <mbus_remote_register_write>
     d84:	4b6f      	ldr	r3, [pc, #444]	; (f44 <main+0x480>)
     d86:	213f      	movs	r1, #63	; 0x3f
     d88:	781a      	ldrb	r2, [r3, #0]
     d8a:	1c28      	adds	r0, r5, #0
     d8c:	438a      	bics	r2, r1
     d8e:	432a      	orrs	r2, r5
     d90:	701a      	strb	r2, [r3, #0]
     d92:	681a      	ldr	r2, [r3, #0]
     d94:	210c      	movs	r1, #12
     d96:	f7ff fa14 	bl	1c2 <mbus_remote_register_write>
     d9a:	22c8      	movs	r2, #200	; 0xc8
     d9c:	1c28      	adds	r0, r5, #0
     d9e:	1c31      	adds	r1, r6, #0
     da0:	0152      	lsls	r2, r2, #5
     da2:	f7ff fa0e 	bl	1c2 <mbus_remote_register_write>
     da6:	4b68      	ldr	r3, [pc, #416]	; (f48 <main+0x484>)
     da8:	4a68      	ldr	r2, [pc, #416]	; (f4c <main+0x488>)
     daa:	213a      	movs	r1, #58	; 0x3a
     dac:	601a      	str	r2, [r3, #0]
     dae:	4b68      	ldr	r3, [pc, #416]	; (f50 <main+0x48c>)
     db0:	2203      	movs	r2, #3
     db2:	601a      	str	r2, [r3, #0]
     db4:	4b67      	ldr	r3, [pc, #412]	; (f54 <main+0x490>)
     db6:	4a68      	ldr	r2, [pc, #416]	; (f58 <main+0x494>)
     db8:	601c      	str	r4, [r3, #0]
     dba:	4b68      	ldr	r3, [pc, #416]	; (f5c <main+0x498>)
     dbc:	1c38      	adds	r0, r7, #0
     dbe:	601c      	str	r4, [r3, #0]
     dc0:	4b67      	ldr	r3, [pc, #412]	; (f60 <main+0x49c>)
     dc2:	601c      	str	r4, [r3, #0]
     dc4:	4b67      	ldr	r3, [pc, #412]	; (f64 <main+0x4a0>)
     dc6:	701c      	strb	r4, [r3, #0]
     dc8:	4b67      	ldr	r3, [pc, #412]	; (f68 <main+0x4a4>)
     dca:	701c      	strb	r4, [r3, #0]
     dcc:	4b67      	ldr	r3, [pc, #412]	; (f6c <main+0x4a8>)
     dce:	701c      	strb	r4, [r3, #0]
     dd0:	4b67      	ldr	r3, [pc, #412]	; (f70 <main+0x4ac>)
     dd2:	601c      	str	r4, [r3, #0]
     dd4:	f7ff f9f5 	bl	1c2 <mbus_remote_register_write>
     dd8:	2064      	movs	r0, #100	; 0x64
     dda:	f7ff f95c 	bl	96 <delay>
     dde:	f7ff fa0f 	bl	200 <batadc_resetrelease>
     de2:	4b64      	ldr	r3, [pc, #400]	; (f74 <main+0x4b0>)
     de4:	220b      	movs	r2, #11
     de6:	601a      	str	r2, [r3, #0]
     de8:	e03c      	b.n	e64 <main+0x3a0>
     dea:	2278      	movs	r2, #120	; 0x78
     dec:	6815      	ldr	r5, [r2, #0]
     dee:	0c2e      	lsrs	r6, r5, #16
     df0:	0e2c      	lsrs	r4, r5, #24
     df2:	0a2b      	lsrs	r3, r5, #8
     df4:	b2f7      	uxtb	r7, r6
     df6:	2c01      	cmp	r4, #1
     df8:	d136      	bne.n	e68 <main+0x3a4>
     dfa:	22ff      	movs	r2, #255	; 0xff
     dfc:	4f54      	ldr	r7, [pc, #336]	; (f50 <main+0x48c>)
     dfe:	4013      	ands	r3, r2
     e00:	603b      	str	r3, [r7, #0]
     e02:	4b34      	ldr	r3, [pc, #208]	; (ed4 <main+0x410>)
     e04:	4016      	ands	r6, r2
     e06:	4015      	ands	r5, r2
     e08:	0236      	lsls	r6, r6, #8
     e0a:	6819      	ldr	r1, [r3, #0]
     e0c:	19ad      	adds	r5, r5, r6
     e0e:	42a9      	cmp	r1, r5
     e10:	d222      	bcs.n	e58 <main+0x394>
     e12:	681a      	ldr	r2, [r3, #0]
     e14:	3201      	adds	r2, #1
     e16:	601a      	str	r2, [r3, #0]
     e18:	681d      	ldr	r5, [r3, #0]
     e1a:	2d01      	cmp	r5, #1
     e1c:	d109      	bne.n	e32 <main+0x36e>
     e1e:	f7ff f9fb 	bl	218 <radio_power_on>
     e22:	2002      	movs	r0, #2
     e24:	1c29      	adds	r1, r5, #0
     e26:	1c2a      	adds	r2, r5, #0
     e28:	f7ff f94a 	bl	c0 <set_wakeup_timer>
     e2c:	f7ff f9b2 	bl	194 <mbus_sleep_all>
     e30:	e7fe      	b.n	e30 <main+0x36c>
     e32:	6819      	ldr	r1, [r3, #0]
     e34:	4a50      	ldr	r2, [pc, #320]	; (f78 <main+0x4b4>)
     e36:	2000      	movs	r0, #0
     e38:	1889      	adds	r1, r1, r2
     e3a:	f7ff fb0f 	bl	45c <send_radio_data_ppm>
     e3e:	20fa      	movs	r0, #250	; 0xfa
     e40:	0100      	lsls	r0, r0, #4
     e42:	f7ff f928 	bl	96 <delay>
     e46:	6838      	ldr	r0, [r7, #0]
     e48:	1c21      	adds	r1, r4, #0
     e4a:	b280      	uxth	r0, r0
     e4c:	1c22      	adds	r2, r4, #0
     e4e:	f7ff f937 	bl	c0 <set_wakeup_timer>
     e52:	f7ff f99f 	bl	194 <mbus_sleep_all>
     e56:	e7fe      	b.n	e56 <main+0x392>
     e58:	2200      	movs	r2, #0
     e5a:	601a      	str	r2, [r3, #0]
     e5c:	1c20      	adds	r0, r4, #0
     e5e:	4947      	ldr	r1, [pc, #284]	; (f7c <main+0x4b8>)
     e60:	f7ff fafc 	bl	45c <send_radio_data_ppm>
     e64:	f7ff fae4 	bl	430 <operation_sleep_notimer>
     e68:	2c02      	cmp	r4, #2
     e6a:	d000      	beq.n	e6e <main+0x3aa>
     e6c:	e095      	b.n	f9a <main+0x4d6>
     e6e:	21ff      	movs	r1, #255	; 0xff
     e70:	400b      	ands	r3, r1
     e72:	400d      	ands	r5, r1
     e74:	021b      	lsls	r3, r3, #8
     e76:	18ed      	adds	r5, r5, r3
     e78:	4833      	ldr	r0, [pc, #204]	; (f48 <main+0x484>)
     e7a:	4b35      	ldr	r3, [pc, #212]	; (f50 <main+0x48c>)
     e7c:	210f      	movs	r1, #15
     e7e:	4031      	ands	r1, r6
     e80:	6005      	str	r5, [r0, #0]
     e82:	6019      	str	r1, [r3, #0]
     e84:	2310      	movs	r3, #16
     e86:	401e      	ands	r6, r3
     e88:	4b35      	ldr	r3, [pc, #212]	; (f60 <main+0x49c>)
     e8a:	4c36      	ldr	r4, [pc, #216]	; (f64 <main+0x4a0>)
     e8c:	601e      	str	r6, [r3, #0]
     e8e:	7823      	ldrb	r3, [r4, #0]
     e90:	2b00      	cmp	r3, #0
     e92:	d175      	bne.n	f80 <main+0x4bc>
     e94:	2101      	movs	r1, #1
     e96:	2005      	movs	r0, #5
     e98:	1c0a      	adds	r2, r1, #0
     e9a:	f7ff f911 	bl	c0 <set_wakeup_timer>
     e9e:	2301      	movs	r3, #1
     ea0:	7023      	strb	r3, [r4, #0]
     ea2:	4b33      	ldr	r3, [pc, #204]	; (f70 <main+0x4ac>)
     ea4:	097f      	lsrs	r7, r7, #5
     ea6:	601f      	str	r7, [r3, #0]
     ea8:	f7ff f974 	bl	194 <mbus_sleep_all>
     eac:	e7fe      	b.n	eac <main+0x3e8>
     eae:	46c0      	nop			; (mov r8, r8)
     eb0:	003fffff 	.word	0x003fffff
     eb4:	0000131c 	.word	0x0000131c
     eb8:	deadbeef 	.word	0xdeadbeef
     ebc:	000012f0 	.word	0x000012f0
     ec0:	ffff9fff 	.word	0xffff9fff
     ec4:	fffe7fff 	.word	0xfffe7fff
     ec8:	a000002c 	.word	0xa000002c
     ecc:	0000132c 	.word	0x0000132c
     ed0:	00001318 	.word	0x00001318
     ed4:	0000134c 	.word	0x0000134c
     ed8:	00001334 	.word	0x00001334
     edc:	00001344 	.word	0x00001344
     ee0:	00001358 	.word	0x00001358
     ee4:	0000e204 	.word	0x0000e204
     ee8:	0000e24a 	.word	0x0000e24a
     eec:	00002203 	.word	0x00002203
     ef0:	00003088 	.word	0x00003088
     ef4:	0000e203 	.word	0x0000e203
     ef8:	0000f10f 	.word	0x0000f10f
     efc:	00ffffff 	.word	0x00ffffff
     f00:	000001d3 	.word	0x000001d3
     f04:	00001308 	.word	0x00001308
     f08:	000012fc 	.word	0x000012fc
     f0c:	fff1ffff 	.word	0xfff1ffff
     f10:	00001300 	.word	0x00001300
     f14:	00001304 	.word	0x00001304
     f18:	ffe07fff 	.word	0xffe07fff
     f1c:	000012e0 	.word	0x000012e0
     f20:	ff03ffff 	.word	0xff03ffff
     f24:	fffffe1f 	.word	0xfffffe1f
     f28:	ffffe1ff 	.word	0xffffe1ff
     f2c:	fffc7fff 	.word	0xfffc7fff
     f30:	000012f4 	.word	0x000012f4
     f34:	ffffe01f 	.word	0xffffe01f
     f38:	fff01fff 	.word	0xfff01fff
     f3c:	000012e8 	.word	0x000012e8
     f40:	ffff03ff 	.word	0xffff03ff
     f44:	000012f8 	.word	0x000012f8
     f48:	00001350 	.word	0x00001350
     f4c:	000083d6 	.word	0x000083d6
     f50:	00001330 	.word	0x00001330
     f54:	00001354 	.word	0x00001354
     f58:	00080800 	.word	0x00080800
     f5c:	00001310 	.word	0x00001310
     f60:	00001320 	.word	0x00001320
     f64:	00001314 	.word	0x00001314
     f68:	0000130c 	.word	0x0000130c
     f6c:	0000135c 	.word	0x0000135c
     f70:	0000133c 	.word	0x0000133c
     f74:	00001324 	.word	0x00001324
     f78:	00abc000 	.word	0x00abc000
     f7c:	00faf000 	.word	0x00faf000
     f80:	49c0      	ldr	r1, [pc, #768]	; (1284 <main+0x7c0>)
     f82:	2300      	movs	r3, #0
     f84:	600b      	str	r3, [r1, #0]
     f86:	49c0      	ldr	r1, [pc, #768]	; (1288 <main+0x7c4>)
     f88:	600b      	str	r3, [r1, #0]
     f8a:	49c0      	ldr	r1, [pc, #768]	; (128c <main+0x7c8>)
     f8c:	600b      	str	r3, [r1, #0]
     f8e:	49c0      	ldr	r1, [pc, #768]	; (1290 <main+0x7cc>)
     f90:	600b      	str	r3, [r1, #0]
     f92:	6013      	str	r3, [r2, #0]
     f94:	4abf      	ldr	r2, [pc, #764]	; (1294 <main+0x7d0>)
     f96:	6013      	str	r3, [r2, #0]
     f98:	e171      	b.n	127e <main+0x7ba>
     f9a:	2c03      	cmp	r4, #3
     f9c:	d141      	bne.n	1022 <main+0x55e>
     f9e:	22ff      	movs	r2, #255	; 0xff
     fa0:	4ebd      	ldr	r6, [pc, #756]	; (1298 <main+0x7d4>)
     fa2:	4013      	ands	r3, r2
     fa4:	6033      	str	r3, [r6, #0]
     fa6:	4bbd      	ldr	r3, [pc, #756]	; (129c <main+0x7d8>)
     fa8:	2400      	movs	r4, #0
     faa:	701c      	strb	r4, [r3, #0]
     fac:	4bbc      	ldr	r3, [pc, #752]	; (12a0 <main+0x7dc>)
     fae:	4015      	ands	r5, r2
     fb0:	701c      	strb	r4, [r3, #0]
     fb2:	4bbc      	ldr	r3, [pc, #752]	; (12a4 <main+0x7e0>)
     fb4:	6819      	ldr	r1, [r3, #0]
     fb6:	42a9      	cmp	r1, r5
     fb8:	d231      	bcs.n	101e <main+0x55a>
     fba:	681a      	ldr	r2, [r3, #0]
     fbc:	3201      	adds	r2, #1
     fbe:	601a      	str	r2, [r3, #0]
     fc0:	681d      	ldr	r5, [r3, #0]
     fc2:	2d01      	cmp	r5, #1
     fc4:	d109      	bne.n	fda <main+0x516>
     fc6:	f7ff f927 	bl	218 <radio_power_on>
     fca:	2002      	movs	r0, #2
     fcc:	1c29      	adds	r1, r5, #0
     fce:	1c2a      	adds	r2, r5, #0
     fd0:	f7ff f876 	bl	c0 <set_wakeup_timer>
     fd4:	f7ff f8de 	bl	194 <mbus_sleep_all>
     fd8:	e7fe      	b.n	fd8 <main+0x514>
     fda:	4baa      	ldr	r3, [pc, #680]	; (1284 <main+0x7c0>)
     fdc:	1c20      	adds	r0, r4, #0
     fde:	6819      	ldr	r1, [r3, #0]
     fe0:	23c1      	movs	r3, #193	; 0xc1
     fe2:	03db      	lsls	r3, r3, #15
     fe4:	18c9      	adds	r1, r1, r3
     fe6:	0049      	lsls	r1, r1, #1
     fe8:	f7ff fa38 	bl	45c <send_radio_data_ppm>
     fec:	20fa      	movs	r0, #250	; 0xfa
     fee:	0100      	lsls	r0, r0, #4
     ff0:	f7ff f851 	bl	96 <delay>
     ff4:	4bac      	ldr	r3, [pc, #688]	; (12a8 <main+0x7e4>)
     ff6:	22c2      	movs	r2, #194	; 0xc2
     ff8:	6819      	ldr	r1, [r3, #0]
     ffa:	0412      	lsls	r2, r2, #16
     ffc:	1889      	adds	r1, r1, r2
     ffe:	1c20      	adds	r0, r4, #0
    1000:	f7ff fa2c 	bl	45c <send_radio_data_ppm>
    1004:	20fa      	movs	r0, #250	; 0xfa
    1006:	0100      	lsls	r0, r0, #4
    1008:	f7ff f845 	bl	96 <delay>
    100c:	6830      	ldr	r0, [r6, #0]
    100e:	2101      	movs	r1, #1
    1010:	b280      	uxth	r0, r0
    1012:	1c0a      	adds	r2, r1, #0
    1014:	f7ff f854 	bl	c0 <set_wakeup_timer>
    1018:	f7ff f8bc 	bl	194 <mbus_sleep_all>
    101c:	e7fe      	b.n	101c <main+0x558>
    101e:	601c      	str	r4, [r3, #0]
    1020:	e12b      	b.n	127a <main+0x7b6>
    1022:	2c04      	cmp	r4, #4
    1024:	d000      	beq.n	1028 <main+0x564>
    1026:	e088      	b.n	113a <main+0x676>
    1028:	22ff      	movs	r2, #255	; 0xff
    102a:	499b      	ldr	r1, [pc, #620]	; (1298 <main+0x7d4>)
    102c:	4013      	ands	r3, r2
    102e:	600b      	str	r3, [r1, #0]
    1030:	4b9e      	ldr	r3, [pc, #632]	; (12ac <main+0x7e8>)
    1032:	4015      	ands	r5, r2
    1034:	4a95      	ldr	r2, [pc, #596]	; (128c <main+0x7c8>)
    1036:	601d      	str	r5, [r3, #0]
    1038:	6819      	ldr	r1, [r3, #0]
    103a:	6812      	ldr	r2, [r2, #0]
    103c:	4291      	cmp	r1, r2
    103e:	d301      	bcc.n	1044 <main+0x580>
    1040:	2200      	movs	r2, #0
    1042:	601a      	str	r2, [r3, #0]
    1044:	4b97      	ldr	r3, [pc, #604]	; (12a4 <main+0x7e0>)
    1046:	681a      	ldr	r2, [r3, #0]
    1048:	2a04      	cmp	r2, #4
    104a:	d84a      	bhi.n	10e2 <main+0x61e>
    104c:	681a      	ldr	r2, [r3, #0]
    104e:	3201      	adds	r2, #1
    1050:	601a      	str	r2, [r3, #0]
    1052:	681c      	ldr	r4, [r3, #0]
    1054:	2c01      	cmp	r4, #1
    1056:	d109      	bne.n	106c <main+0x5a8>
    1058:	f7ff f8de 	bl	218 <radio_power_on>
    105c:	2002      	movs	r0, #2
    105e:	1c21      	adds	r1, r4, #0
    1060:	1c22      	adds	r2, r4, #0
    1062:	f7ff f82d 	bl	c0 <set_wakeup_timer>
    1066:	f7ff f895 	bl	194 <mbus_sleep_all>
    106a:	e7fe      	b.n	106a <main+0x5a6>
    106c:	6819      	ldr	r1, [r3, #0]
    106e:	4b90      	ldr	r3, [pc, #576]	; (12b0 <main+0x7ec>)
    1070:	2000      	movs	r0, #0
    1072:	18c9      	adds	r1, r1, r3
    1074:	f7ff f9f2 	bl	45c <send_radio_data_ppm>
    1078:	4b87      	ldr	r3, [pc, #540]	; (1298 <main+0x7d4>)
    107a:	2101      	movs	r1, #1
    107c:	6818      	ldr	r0, [r3, #0]
    107e:	1c0a      	adds	r2, r1, #0
    1080:	b280      	uxth	r0, r0
    1082:	f7ff f81d 	bl	c0 <set_wakeup_timer>
    1086:	f7ff f885 	bl	194 <mbus_sleep_all>
    108a:	e7fe      	b.n	108a <main+0x5c6>
    108c:	4889      	ldr	r0, [pc, #548]	; (12b4 <main+0x7f0>)
    108e:	f7ff f80b 	bl	a8 <config_timerwd>
    1092:	6821      	ldr	r1, [r4, #0]
    1094:	20dd      	movs	r0, #221	; 0xdd
    1096:	f7ff f853 	bl	140 <mbus_write_message32>
    109a:	2064      	movs	r0, #100	; 0x64
    109c:	f7fe fffb 	bl	96 <delay>
    10a0:	6822      	ldr	r2, [r4, #0]
    10a2:	4b85      	ldr	r3, [pc, #532]	; (12b8 <main+0x7f4>)
    10a4:	0092      	lsls	r2, r2, #2
    10a6:	58d1      	ldr	r1, [r2, r3]
    10a8:	20dd      	movs	r0, #221	; 0xdd
    10aa:	f7ff f849 	bl	140 <mbus_write_message32>
    10ae:	2064      	movs	r0, #100	; 0x64
    10b0:	f7fe fff1 	bl	96 <delay>
    10b4:	6823      	ldr	r3, [r4, #0]
    10b6:	4d81      	ldr	r5, [pc, #516]	; (12bc <main+0x7f8>)
    10b8:	009b      	lsls	r3, r3, #2
    10ba:	5959      	ldr	r1, [r3, r5]
    10bc:	20dd      	movs	r0, #221	; 0xdd
    10be:	f7ff f83f 	bl	140 <mbus_write_message32>
    10c2:	2064      	movs	r0, #100	; 0x64
    10c4:	f7fe ffe7 	bl	96 <delay>
    10c8:	6823      	ldr	r3, [r4, #0]
    10ca:	2000      	movs	r0, #0
    10cc:	009b      	lsls	r3, r3, #2
    10ce:	5959      	ldr	r1, [r3, r5]
    10d0:	f7ff f9c4 	bl	45c <send_radio_data_ppm>
    10d4:	20fa      	movs	r0, #250	; 0xfa
    10d6:	0100      	lsls	r0, r0, #4
    10d8:	f7fe ffdd 	bl	96 <delay>
    10dc:	6823      	ldr	r3, [r4, #0]
    10de:	3b01      	subs	r3, #1
    10e0:	6023      	str	r3, [r4, #0]
    10e2:	4b72      	ldr	r3, [pc, #456]	; (12ac <main+0x7e8>)
    10e4:	2500      	movs	r5, #0
    10e6:	681a      	ldr	r2, [r3, #0]
    10e8:	4969      	ldr	r1, [pc, #420]	; (1290 <main+0x7cc>)
    10ea:	42aa      	cmp	r2, r5
    10ec:	d102      	bne.n	10f4 <main+0x630>
    10ee:	680d      	ldr	r5, [r1, #0]
    10f0:	1e6a      	subs	r2, r5, #1
    10f2:	4195      	sbcs	r5, r2
    10f4:	6818      	ldr	r0, [r3, #0]
    10f6:	2200      	movs	r2, #0
    10f8:	4290      	cmp	r0, r2
    10fa:	d007      	beq.n	110c <main+0x648>
    10fc:	681a      	ldr	r2, [r3, #0]
    10fe:	4b63      	ldr	r3, [pc, #396]	; (128c <main+0x7c8>)
    1100:	6809      	ldr	r1, [r1, #0]
    1102:	681b      	ldr	r3, [r3, #0]
    1104:	188a      	adds	r2, r1, r2
    1106:	4293      	cmp	r3, r2
    1108:	4192      	sbcs	r2, r2
    110a:	4252      	negs	r2, r2
    110c:	4315      	orrs	r5, r2
    110e:	4c60      	ldr	r4, [pc, #384]	; (1290 <main+0x7cc>)
    1110:	d1bc      	bne.n	108c <main+0x5c8>
    1112:	6822      	ldr	r2, [r4, #0]
    1114:	4b69      	ldr	r3, [pc, #420]	; (12bc <main+0x7f8>)
    1116:	0092      	lsls	r2, r2, #2
    1118:	58d1      	ldr	r1, [r2, r3]
    111a:	1c28      	adds	r0, r5, #0
    111c:	f7ff f99e 	bl	45c <send_radio_data_ppm>
    1120:	4867      	ldr	r0, [pc, #412]	; (12c0 <main+0x7fc>)
    1122:	f7fe ffb8 	bl	96 <delay>
    1126:	2001      	movs	r0, #1
    1128:	4966      	ldr	r1, [pc, #408]	; (12c4 <main+0x800>)
    112a:	f7ff f997 	bl	45c <send_radio_data_ppm>
    112e:	4b5d      	ldr	r3, [pc, #372]	; (12a4 <main+0x7e0>)
    1130:	601d      	str	r5, [r3, #0]
    1132:	4b56      	ldr	r3, [pc, #344]	; (128c <main+0x7c8>)
    1134:	681b      	ldr	r3, [r3, #0]
    1136:	6023      	str	r3, [r4, #0]
    1138:	e694      	b.n	e64 <main+0x3a0>
    113a:	2c07      	cmp	r4, #7
    113c:	d151      	bne.n	11e2 <main+0x71e>
    113e:	27ff      	movs	r7, #255	; 0xff
    1140:	4955      	ldr	r1, [pc, #340]	; (1298 <main+0x7d4>)
    1142:	403b      	ands	r3, r7
    1144:	600b      	str	r3, [r1, #0]
    1146:	4b60      	ldr	r3, [pc, #384]	; (12c8 <main+0x804>)
    1148:	403e      	ands	r6, r7
    114a:	601e      	str	r6, [r3, #0]
    114c:	4b4d      	ldr	r3, [pc, #308]	; (1284 <main+0x7c0>)
    114e:	2200      	movs	r2, #0
    1150:	601a      	str	r2, [r3, #0]
    1152:	4b54      	ldr	r3, [pc, #336]	; (12a4 <main+0x7e0>)
    1154:	403d      	ands	r5, r7
    1156:	6819      	ldr	r1, [r3, #0]
    1158:	42a9      	cmp	r1, r5
    115a:	d23a      	bcs.n	11d2 <main+0x70e>
    115c:	6819      	ldr	r1, [r3, #0]
    115e:	3101      	adds	r1, #1
    1160:	6019      	str	r1, [r3, #0]
    1162:	681d      	ldr	r5, [r3, #0]
    1164:	2d01      	cmp	r5, #1
    1166:	d11f      	bne.n	11a8 <main+0x6e4>
    1168:	1c11      	adds	r1, r2, #0
    116a:	1c20      	adds	r0, r4, #0
    116c:	2203      	movs	r2, #3
    116e:	f7ff f828 	bl	1c2 <mbus_remote_register_write>
    1172:	2064      	movs	r0, #100	; 0x64
    1174:	f7fe ff8f 	bl	96 <delay>
    1178:	2064      	movs	r0, #100	; 0x64
    117a:	f7fe ff8c 	bl	96 <delay>
    117e:	23a0      	movs	r3, #160	; 0xa0
    1180:	061b      	lsls	r3, r3, #24
    1182:	681b      	ldr	r3, [r3, #0]
    1184:	401f      	ands	r7, r3
    1186:	4b51      	ldr	r3, [pc, #324]	; (12cc <main+0x808>)
    1188:	601f      	str	r7, [r3, #0]
    118a:	f7ff f82d 	bl	1e8 <batadc_reset>
    118e:	2064      	movs	r0, #100	; 0x64
    1190:	f7fe ff81 	bl	96 <delay>
    1194:	f7ff f840 	bl	218 <radio_power_on>
    1198:	2002      	movs	r0, #2
    119a:	1c29      	adds	r1, r5, #0
    119c:	1c2a      	adds	r2, r5, #0
    119e:	f7fe ff8f 	bl	c0 <set_wakeup_timer>
    11a2:	f7fe fff7 	bl	194 <mbus_sleep_all>
    11a6:	e7fe      	b.n	11a6 <main+0x6e2>
    11a8:	4b48      	ldr	r3, [pc, #288]	; (12cc <main+0x808>)
    11aa:	1c10      	adds	r0, r2, #0
    11ac:	6819      	ldr	r1, [r3, #0]
    11ae:	4b48      	ldr	r3, [pc, #288]	; (12d0 <main+0x80c>)
    11b0:	18c9      	adds	r1, r1, r3
    11b2:	f7ff f953 	bl	45c <send_radio_data_ppm>
    11b6:	20fa      	movs	r0, #250	; 0xfa
    11b8:	0100      	lsls	r0, r0, #4
    11ba:	f7fe ff6c 	bl	96 <delay>
    11be:	4c36      	ldr	r4, [pc, #216]	; (1298 <main+0x7d4>)
    11c0:	2101      	movs	r1, #1
    11c2:	6820      	ldr	r0, [r4, #0]
    11c4:	1c0a      	adds	r2, r1, #0
    11c6:	b280      	uxth	r0, r0
    11c8:	f7fe ff7a 	bl	c0 <set_wakeup_timer>
    11cc:	f7fe ffe2 	bl	194 <mbus_sleep_all>
    11d0:	e7fe      	b.n	11d0 <main+0x70c>
    11d2:	2001      	movs	r0, #1
    11d4:	493b      	ldr	r1, [pc, #236]	; (12c4 <main+0x800>)
    11d6:	601a      	str	r2, [r3, #0]
    11d8:	f7ff f940 	bl	45c <send_radio_data_ppm>
    11dc:	f7ff f810 	bl	200 <batadc_resetrelease>
    11e0:	e640      	b.n	e64 <main+0x3a0>
    11e2:	2c13      	cmp	r4, #19
    11e4:	d14b      	bne.n	127e <main+0x7ba>
    11e6:	21ff      	movs	r1, #255	; 0xff
    11e8:	4c2b      	ldr	r4, [pc, #172]	; (1298 <main+0x7d4>)
    11ea:	400b      	ands	r3, r1
    11ec:	2064      	movs	r0, #100	; 0x64
    11ee:	6023      	str	r3, [r4, #0]
    11f0:	f7fe ff51 	bl	96 <delay>
    11f4:	4b37      	ldr	r3, [pc, #220]	; (12d4 <main+0x810>)
    11f6:	093f      	lsrs	r7, r7, #4
    11f8:	017a      	lsls	r2, r7, #5
    11fa:	4937      	ldr	r1, [pc, #220]	; (12d8 <main+0x814>)
    11fc:	681f      	ldr	r7, [r3, #0]
    11fe:	2004      	movs	r0, #4
    1200:	400f      	ands	r7, r1
    1202:	4317      	orrs	r7, r2
    1204:	220f      	movs	r2, #15
    1206:	601f      	str	r7, [r3, #0]
    1208:	4016      	ands	r6, r2
    120a:	0272      	lsls	r2, r6, #9
    120c:	4933      	ldr	r1, [pc, #204]	; (12dc <main+0x818>)
    120e:	681e      	ldr	r6, [r3, #0]
    1210:	400e      	ands	r6, r1
    1212:	4316      	orrs	r6, r2
    1214:	601e      	str	r6, [r3, #0]
    1216:	681a      	ldr	r2, [r3, #0]
    1218:	2100      	movs	r1, #0
    121a:	f7fe ffd2 	bl	1c2 <mbus_remote_register_write>
    121e:	2064      	movs	r0, #100	; 0x64
    1220:	f7fe ff39 	bl	96 <delay>
    1224:	4b1f      	ldr	r3, [pc, #124]	; (12a4 <main+0x7e0>)
    1226:	21ff      	movs	r1, #255	; 0xff
    1228:	681a      	ldr	r2, [r3, #0]
    122a:	400d      	ands	r5, r1
    122c:	42aa      	cmp	r2, r5
    122e:	d222      	bcs.n	1276 <main+0x7b2>
    1230:	681a      	ldr	r2, [r3, #0]
    1232:	3201      	adds	r2, #1
    1234:	601a      	str	r2, [r3, #0]
    1236:	681d      	ldr	r5, [r3, #0]
    1238:	2d01      	cmp	r5, #1
    123a:	d109      	bne.n	1250 <main+0x78c>
    123c:	f7fe ffec 	bl	218 <radio_power_on>
    1240:	2002      	movs	r0, #2
    1242:	1c29      	adds	r1, r5, #0
    1244:	1c2a      	adds	r2, r5, #0
    1246:	f7fe ff3b 	bl	c0 <set_wakeup_timer>
    124a:	f7fe ffa3 	bl	194 <mbus_sleep_all>
    124e:	e7fe      	b.n	124e <main+0x78a>
    1250:	6819      	ldr	r1, [r3, #0]
    1252:	4a17      	ldr	r2, [pc, #92]	; (12b0 <main+0x7ec>)
    1254:	2000      	movs	r0, #0
    1256:	1889      	adds	r1, r1, r2
    1258:	f7ff f900 	bl	45c <send_radio_data_ppm>
    125c:	20fa      	movs	r0, #250	; 0xfa
    125e:	0100      	lsls	r0, r0, #4
    1260:	f7fe ff19 	bl	96 <delay>
    1264:	6820      	ldr	r0, [r4, #0]
    1266:	2101      	movs	r1, #1
    1268:	b280      	uxth	r0, r0
    126a:	1c0a      	adds	r2, r1, #0
    126c:	f7fe ff28 	bl	c0 <set_wakeup_timer>
    1270:	f7fe ff90 	bl	194 <mbus_sleep_all>
    1274:	e7fe      	b.n	1274 <main+0x7b0>
    1276:	2200      	movs	r2, #0
    1278:	601a      	str	r2, [r3, #0]
    127a:	2001      	movs	r0, #1
    127c:	e5ef      	b.n	e5e <main+0x39a>
    127e:	f7ff f94f 	bl	520 <operation_run>
    1282:	e7fc      	b.n	127e <main+0x7ba>
    1284:	00001318 	.word	0x00001318
    1288:	00001340 	.word	0x00001340
    128c:	00001354 	.word	0x00001354
    1290:	00001310 	.word	0x00001310
    1294:	00001348 	.word	0x00001348
    1298:	00001330 	.word	0x00001330
    129c:	00001314 	.word	0x00001314
    12a0:	0000132c 	.word	0x0000132c
    12a4:	0000134c 	.word	0x0000134c
    12a8:	00001358 	.word	0x00001358
    12ac:	00001338 	.word	0x00001338
    12b0:	00abc000 	.word	0x00abc000
    12b4:	000fffff 	.word	0x000fffff
    12b8:	00001360 	.word	0x00001360
    12bc:	000019a0 	.word	0x000019a0
    12c0:	00002ee0 	.word	0x00002ee0
    12c4:	00faf000 	.word	0x00faf000
    12c8:	00001324 	.word	0x00001324
    12cc:	00001328 	.word	0x00001328
    12d0:	00bbb000 	.word	0x00bbb000
    12d4:	000012e0 	.word	0x000012e0
    12d8:	fffffe1f 	.word	0xfffffe1f
    12dc:	ffffe1ff 	.word	0xffffe1ff
