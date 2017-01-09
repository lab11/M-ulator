
Tstack_ondemand_v1.10_debug2/Tstack_ondemand_v1.10_debug2.elf:     file format elf32-littlearm


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
  40:	000007f1 	.word	0x000007f1
  44:	000007fd 	.word	0x000007fd
  48:	00000809 	.word	0x00000809
  4c:	0000082d 	.word	0x0000082d
  50:	00000845 	.word	0x00000845
  54:	0000085d 	.word	0x0000085d
  58:	00000875 	.word	0x00000875
  5c:	0000088d 	.word	0x0000088d
  60:	000008a5 	.word	0x000008a5
  64:	000008bd 	.word	0x000008bd
  68:	000008d5 	.word	0x000008d5
  6c:	000008e5 	.word	0x000008e5
  70:	000008f5 	.word	0x000008f5
  74:	00000905 	.word	0x00000905
	...

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 fc40 	bl	914 <main>
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

Disassembly of section .text.mbus_remote_register_write:

000001a0 <mbus_remote_register_write>:
 1a0:	b507      	push	{r0, r1, r2, lr}
 1a2:	0212      	lsls	r2, r2, #8
 1a4:	0a12      	lsrs	r2, r2, #8
 1a6:	0609      	lsls	r1, r1, #24
 1a8:	4311      	orrs	r1, r2
 1aa:	0100      	lsls	r0, r0, #4
 1ac:	9101      	str	r1, [sp, #4]
 1ae:	b2c0      	uxtb	r0, r0
 1b0:	a901      	add	r1, sp, #4
 1b2:	2201      	movs	r2, #1
 1b4:	f7ff ffcc 	bl	150 <mbus_write_message>
 1b8:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.radio_power_off:

000001bc <radio_power_off>:
 1bc:	4a15      	ldr	r2, [pc, #84]	; (214 <radio_power_off+0x58>)
 1be:	b508      	push	{r3, lr}
 1c0:	2300      	movs	r3, #0
 1c2:	6013      	str	r3, [r2, #0]
 1c4:	4a14      	ldr	r2, [pc, #80]	; (218 <radio_power_off+0x5c>)
 1c6:	2004      	movs	r0, #4
 1c8:	6013      	str	r3, [r2, #0]
 1ca:	4b14      	ldr	r3, [pc, #80]	; (21c <radio_power_off+0x60>)
 1cc:	4a14      	ldr	r2, [pc, #80]	; (220 <radio_power_off+0x64>)
 1ce:	6819      	ldr	r1, [r3, #0]
 1d0:	400a      	ands	r2, r1
 1d2:	601a      	str	r2, [r3, #0]
 1d4:	6819      	ldr	r1, [r3, #0]
 1d6:	2280      	movs	r2, #128	; 0x80
 1d8:	02d2      	lsls	r2, r2, #11
 1da:	430a      	orrs	r2, r1
 1dc:	601a      	str	r2, [r3, #0]
 1de:	681a      	ldr	r2, [r3, #0]
 1e0:	2102      	movs	r1, #2
 1e2:	f7ff ffdd 	bl	1a0 <mbus_remote_register_write>
 1e6:	4b0f      	ldr	r3, [pc, #60]	; (224 <radio_power_off+0x68>)
 1e8:	2101      	movs	r1, #1
 1ea:	881a      	ldrh	r2, [r3, #0]
 1ec:	2004      	movs	r0, #4
 1ee:	430a      	orrs	r2, r1
 1f0:	801a      	strh	r2, [r3, #0]
 1f2:	881a      	ldrh	r2, [r3, #0]
 1f4:	2102      	movs	r1, #2
 1f6:	430a      	orrs	r2, r1
 1f8:	801a      	strh	r2, [r3, #0]
 1fa:	881a      	ldrh	r2, [r3, #0]
 1fc:	2108      	movs	r1, #8
 1fe:	4382      	bics	r2, r0
 200:	801a      	strh	r2, [r3, #0]
 202:	881a      	ldrh	r2, [r3, #0]
 204:	438a      	bics	r2, r1
 206:	801a      	strh	r2, [r3, #0]
 208:	681a      	ldr	r2, [r3, #0]
 20a:	210d      	movs	r1, #13
 20c:	f7ff ffc8 	bl	1a0 <mbus_remote_register_write>
 210:	bd08      	pop	{r3, pc}
 212:	46c0      	nop			; (mov r8, r8)
 214:	000013c0 	.word	0x000013c0
 218:	00001370 	.word	0x00001370
 21c:	00001338 	.word	0x00001338
 220:	fff7ffff 	.word	0xfff7ffff
 224:	00001330 	.word	0x00001330

Disassembly of section .text.ldo_power_off:

00000228 <ldo_power_off>:
 228:	b508      	push	{r3, lr}
 22a:	4b08      	ldr	r3, [pc, #32]	; (24c <ldo_power_off+0x24>)
 22c:	2280      	movs	r2, #128	; 0x80
 22e:	6819      	ldr	r1, [r3, #0]
 230:	0152      	lsls	r2, r2, #5
 232:	430a      	orrs	r2, r1
 234:	601a      	str	r2, [r3, #0]
 236:	6819      	ldr	r1, [r3, #0]
 238:	2280      	movs	r2, #128	; 0x80
 23a:	0112      	lsls	r2, r2, #4
 23c:	430a      	orrs	r2, r1
 23e:	601a      	str	r2, [r3, #0]
 240:	681a      	ldr	r2, [r3, #0]
 242:	2005      	movs	r0, #5
 244:	2112      	movs	r1, #18
 246:	f7ff ffab 	bl	1a0 <mbus_remote_register_write>
 24a:	bd08      	pop	{r3, pc}
 24c:	00001358 	.word	0x00001358

Disassembly of section .text.temp_power_off:

00000250 <temp_power_off>:
 250:	b508      	push	{r3, lr}
 252:	4b0b      	ldr	r3, [pc, #44]	; (280 <temp_power_off+0x30>)
 254:	2280      	movs	r2, #128	; 0x80
 256:	6819      	ldr	r1, [r3, #0]
 258:	0252      	lsls	r2, r2, #9
 25a:	430a      	orrs	r2, r1
 25c:	601a      	str	r2, [r3, #0]
 25e:	6819      	ldr	r1, [r3, #0]
 260:	4a08      	ldr	r2, [pc, #32]	; (284 <temp_power_off+0x34>)
 262:	2005      	movs	r0, #5
 264:	400a      	ands	r2, r1
 266:	601a      	str	r2, [r3, #0]
 268:	6819      	ldr	r1, [r3, #0]
 26a:	2280      	movs	r2, #128	; 0x80
 26c:	0392      	lsls	r2, r2, #14
 26e:	430a      	orrs	r2, r1
 270:	601a      	str	r2, [r3, #0]
 272:	681a      	ldr	r2, [r3, #0]
 274:	210e      	movs	r1, #14
 276:	f7ff ff93 	bl	1a0 <mbus_remote_register_write>
 27a:	f7ff ffd5 	bl	228 <ldo_power_off>
 27e:	bd08      	pop	{r3, pc}
 280:	0000134c 	.word	0x0000134c
 284:	ffbfffff 	.word	0xffbfffff

Disassembly of section .text.unlikely.batadc_reset:

00000288 <batadc_reset>:
 288:	b508      	push	{r3, lr}
 28a:	2006      	movs	r0, #6
 28c:	213b      	movs	r1, #59	; 0x3b
 28e:	4a03      	ldr	r2, [pc, #12]	; (29c <batadc_reset+0x14>)
 290:	f7ff ff86 	bl	1a0 <mbus_remote_register_write>
 294:	2064      	movs	r0, #100	; 0x64
 296:	f7ff fefe 	bl	96 <delay>
 29a:	bd08      	pop	{r3, pc}
 29c:	000fe7ff 	.word	0x000fe7ff

Disassembly of section .text.batadc_resetrelease:

000002a0 <batadc_resetrelease>:
 2a0:	b508      	push	{r3, lr}
 2a2:	2006      	movs	r0, #6
 2a4:	213b      	movs	r1, #59	; 0x3b
 2a6:	4a03      	ldr	r2, [pc, #12]	; (2b4 <batadc_resetrelease+0x14>)
 2a8:	f7ff ff7a 	bl	1a0 <mbus_remote_register_write>
 2ac:	2064      	movs	r0, #100	; 0x64
 2ae:	f7ff fef2 	bl	96 <delay>
 2b2:	bd08      	pop	{r3, pc}
 2b4:	000fefff 	.word	0x000fefff

Disassembly of section .text.reset_pmu_solar_short:

000002b8 <reset_pmu_solar_short>:
 2b8:	b510      	push	{r4, lr}
 2ba:	24c2      	movs	r4, #194	; 0xc2
 2bc:	00e4      	lsls	r4, r4, #3
 2be:	1c22      	adds	r2, r4, #0
 2c0:	210e      	movs	r1, #14
 2c2:	2006      	movs	r0, #6
 2c4:	f7ff ff6c 	bl	1a0 <mbus_remote_register_write>
 2c8:	2064      	movs	r0, #100	; 0x64
 2ca:	f7ff fee4 	bl	96 <delay>
 2ce:	1c22      	adds	r2, r4, #0
 2d0:	210e      	movs	r1, #14
 2d2:	2006      	movs	r0, #6
 2d4:	f7ff ff64 	bl	1a0 <mbus_remote_register_write>
 2d8:	2064      	movs	r0, #100	; 0x64
 2da:	f7ff fedc 	bl	96 <delay>
 2de:	2282      	movs	r2, #130	; 0x82
 2e0:	210e      	movs	r1, #14
 2e2:	00d2      	lsls	r2, r2, #3
 2e4:	2006      	movs	r0, #6
 2e6:	f7ff ff5b 	bl	1a0 <mbus_remote_register_write>
 2ea:	2064      	movs	r0, #100	; 0x64
 2ec:	f7ff fed3 	bl	96 <delay>
 2f0:	bd10      	pop	{r4, pc}

Disassembly of section .text.radio_power_on:

000002f4 <radio_power_on>:
 2f4:	b538      	push	{r3, r4, r5, lr}
 2f6:	4b1d      	ldr	r3, [pc, #116]	; (36c <radio_power_on+0x78>)
 2f8:	2200      	movs	r2, #0
 2fa:	601a      	str	r2, [r3, #0]
 2fc:	4a1c      	ldr	r2, [pc, #112]	; (370 <radio_power_on+0x7c>)
 2fe:	4c1d      	ldr	r4, [pc, #116]	; (374 <radio_power_on+0x80>)
 300:	2301      	movs	r3, #1
 302:	6013      	str	r3, [r2, #0]
 304:	8822      	ldrh	r2, [r4, #0]
 306:	210d      	movs	r1, #13
 308:	439a      	bics	r2, r3
 30a:	8022      	strh	r2, [r4, #0]
 30c:	6822      	ldr	r2, [r4, #0]
 30e:	2004      	movs	r0, #4
 310:	f7ff ff46 	bl	1a0 <mbus_remote_register_write>
 314:	2064      	movs	r0, #100	; 0x64
 316:	f7ff febe 	bl	96 <delay>
 31a:	4d17      	ldr	r5, [pc, #92]	; (378 <radio_power_on+0x84>)
 31c:	4b17      	ldr	r3, [pc, #92]	; (37c <radio_power_on+0x88>)
 31e:	682a      	ldr	r2, [r5, #0]
 320:	2102      	movs	r1, #2
 322:	4013      	ands	r3, r2
 324:	602b      	str	r3, [r5, #0]
 326:	682a      	ldr	r2, [r5, #0]
 328:	2004      	movs	r0, #4
 32a:	f7ff ff39 	bl	1a0 <mbus_remote_register_write>
 32e:	2096      	movs	r0, #150	; 0x96
 330:	0040      	lsls	r0, r0, #1
 332:	f7ff feb0 	bl	96 <delay>
 336:	682a      	ldr	r2, [r5, #0]
 338:	2380      	movs	r3, #128	; 0x80
 33a:	031b      	lsls	r3, r3, #12
 33c:	4313      	orrs	r3, r2
 33e:	602b      	str	r3, [r5, #0]
 340:	682a      	ldr	r2, [r5, #0]
 342:	2102      	movs	r1, #2
 344:	2004      	movs	r0, #4
 346:	f7ff ff2b 	bl	1a0 <mbus_remote_register_write>
 34a:	2064      	movs	r0, #100	; 0x64
 34c:	f7ff fea3 	bl	96 <delay>
 350:	8823      	ldrh	r3, [r4, #0]
 352:	2202      	movs	r2, #2
 354:	4393      	bics	r3, r2
 356:	8023      	strh	r3, [r4, #0]
 358:	6822      	ldr	r2, [r4, #0]
 35a:	210d      	movs	r1, #13
 35c:	2004      	movs	r0, #4
 35e:	f7ff ff1f 	bl	1a0 <mbus_remote_register_write>
 362:	2064      	movs	r0, #100	; 0x64
 364:	f7ff fe97 	bl	96 <delay>
 368:	bd38      	pop	{r3, r4, r5, pc}
 36a:	46c0      	nop			; (mov r8, r8)
 36c:	00001370 	.word	0x00001370
 370:	000013c0 	.word	0x000013c0
 374:	00001330 	.word	0x00001330
 378:	00001338 	.word	0x00001338
 37c:	fffbffff 	.word	0xfffbffff

Disassembly of section .text.operation_sleep_notimer:

00000380 <operation_sleep_notimer>:
 380:	b508      	push	{r3, lr}
 382:	f7ff ff51 	bl	228 <ldo_power_off>
 386:	4b08      	ldr	r3, [pc, #32]	; (3a8 <operation_sleep_notimer+0x28>)
 388:	681b      	ldr	r3, [r3, #0]
 38a:	2b00      	cmp	r3, #0
 38c:	d001      	beq.n	392 <operation_sleep_notimer+0x12>
 38e:	f7ff ff15 	bl	1bc <radio_power_off>
 392:	2000      	movs	r0, #0
 394:	1c02      	adds	r2, r0, #0
 396:	1c01      	adds	r1, r0, #0
 398:	f7ff fe92 	bl	c0 <set_wakeup_timer>
 39c:	2200      	movs	r2, #0
 39e:	2378      	movs	r3, #120	; 0x78
 3a0:	601a      	str	r2, [r3, #0]
 3a2:	f7ff fef7 	bl	194 <mbus_sleep_all>
 3a6:	e7fe      	b.n	3a6 <operation_sleep_notimer+0x26>
 3a8:	000013c0 	.word	0x000013c0

Disassembly of section .text.send_radio_data_ppm:

000003ac <send_radio_data_ppm>:
 3ac:	b570      	push	{r4, r5, r6, lr}
 3ae:	4b2b      	ldr	r3, [pc, #172]	; (45c <send_radio_data_ppm+0xb0>)
 3b0:	020a      	lsls	r2, r1, #8
 3b2:	6819      	ldr	r1, [r3, #0]
 3b4:	0a12      	lsrs	r2, r2, #8
 3b6:	0e09      	lsrs	r1, r1, #24
 3b8:	0609      	lsls	r1, r1, #24
 3ba:	4311      	orrs	r1, r2
 3bc:	6019      	str	r1, [r3, #0]
 3be:	681a      	ldr	r2, [r3, #0]
 3c0:	1c05      	adds	r5, r0, #0
 3c2:	2103      	movs	r1, #3
 3c4:	2004      	movs	r0, #4
 3c6:	f7ff feeb 	bl	1a0 <mbus_remote_register_write>
 3ca:	4b25      	ldr	r3, [pc, #148]	; (460 <send_radio_data_ppm+0xb4>)
 3cc:	681a      	ldr	r2, [r3, #0]
 3ce:	2a00      	cmp	r2, #0
 3d0:	d10d      	bne.n	3ee <send_radio_data_ppm+0x42>
 3d2:	2201      	movs	r2, #1
 3d4:	601a      	str	r2, [r3, #0]
 3d6:	4b23      	ldr	r3, [pc, #140]	; (464 <send_radio_data_ppm+0xb8>)
 3d8:	2004      	movs	r0, #4
 3da:	881a      	ldrh	r2, [r3, #0]
 3dc:	210d      	movs	r1, #13
 3de:	4302      	orrs	r2, r0
 3e0:	801a      	strh	r2, [r3, #0]
 3e2:	681a      	ldr	r2, [r3, #0]
 3e4:	f7ff fedc 	bl	1a0 <mbus_remote_register_write>
 3e8:	2064      	movs	r0, #100	; 0x64
 3ea:	f7ff fe54 	bl	96 <delay>
 3ee:	f7ff fe8b 	bl	108 <set_halt_until_mbus_rx>
 3f2:	4e1d      	ldr	r6, [pc, #116]	; (468 <send_radio_data_ppm+0xbc>)
 3f4:	2300      	movs	r3, #0
 3f6:	6033      	str	r3, [r6, #0]
 3f8:	4b1a      	ldr	r3, [pc, #104]	; (464 <send_radio_data_ppm+0xb8>)
 3fa:	2108      	movs	r1, #8
 3fc:	881a      	ldrh	r2, [r3, #0]
 3fe:	2004      	movs	r0, #4
 400:	430a      	orrs	r2, r1
 402:	801a      	strh	r2, [r3, #0]
 404:	681a      	ldr	r2, [r3, #0]
 406:	210d      	movs	r1, #13
 408:	f7ff feca 	bl	1a0 <mbus_remote_register_write>
 40c:	2432      	movs	r4, #50	; 0x32
 40e:	6833      	ldr	r3, [r6, #0]
 410:	2b00      	cmp	r3, #0
 412:	d015      	beq.n	440 <send_radio_data_ppm+0x94>
 414:	f7ff fe86 	bl	124 <set_halt_until_mbus_tx>
 418:	2300      	movs	r3, #0
 41a:	6033      	str	r3, [r6, #0]
 41c:	429d      	cmp	r5, r3
 41e:	d004      	beq.n	42a <send_radio_data_ppm+0x7e>
 420:	4a0f      	ldr	r2, [pc, #60]	; (460 <send_radio_data_ppm+0xb4>)
 422:	6013      	str	r3, [r2, #0]
 424:	f7ff feca 	bl	1bc <radio_power_off>
 428:	e016      	b.n	458 <send_radio_data_ppm+0xac>
 42a:	4b0e      	ldr	r3, [pc, #56]	; (464 <send_radio_data_ppm+0xb8>)
 42c:	2108      	movs	r1, #8
 42e:	881a      	ldrh	r2, [r3, #0]
 430:	2004      	movs	r0, #4
 432:	438a      	bics	r2, r1
 434:	801a      	strh	r2, [r3, #0]
 436:	681a      	ldr	r2, [r3, #0]
 438:	210d      	movs	r1, #13
 43a:	f7ff feb1 	bl	1a0 <mbus_remote_register_write>
 43e:	e00b      	b.n	458 <send_radio_data_ppm+0xac>
 440:	2064      	movs	r0, #100	; 0x64
 442:	3c01      	subs	r4, #1
 444:	f7ff fe27 	bl	96 <delay>
 448:	2c00      	cmp	r4, #0
 44a:	d1e0      	bne.n	40e <send_radio_data_ppm+0x62>
 44c:	f7ff fe6a 	bl	124 <set_halt_until_mbus_tx>
 450:	20bb      	movs	r0, #187	; 0xbb
 452:	4906      	ldr	r1, [pc, #24]	; (46c <send_radio_data_ppm+0xc0>)
 454:	f7ff fe74 	bl	140 <mbus_write_message32>
 458:	bd70      	pop	{r4, r5, r6, pc}
 45a:	46c0      	nop			; (mov r8, r8)
 45c:	00001a04 	.word	0x00001a04
 460:	00001370 	.word	0x00001370
 464:	00001330 	.word	0x00001330
 468:	000013a4 	.word	0x000013a4
 46c:	fafafafa 	.word	0xfafafafa

Disassembly of section .text.operation_temp_run:

00000470 <operation_temp_run>:
 470:	b538      	push	{r3, r4, r5, lr}
 472:	4bb9      	ldr	r3, [pc, #740]	; (758 <operation_temp_run+0x2e8>)
 474:	681a      	ldr	r2, [r3, #0]
 476:	2a00      	cmp	r2, #0
 478:	d122      	bne.n	4c0 <operation_temp_run+0x50>
 47a:	2101      	movs	r1, #1
 47c:	6019      	str	r1, [r3, #0]
 47e:	4bb7      	ldr	r3, [pc, #732]	; (75c <operation_temp_run+0x2ec>)
 480:	601a      	str	r2, [r3, #0]
 482:	4bb7      	ldr	r3, [pc, #732]	; (760 <operation_temp_run+0x2f0>)
 484:	681b      	ldr	r3, [r3, #0]
 486:	2b00      	cmp	r3, #0
 488:	d104      	bne.n	494 <operation_temp_run+0x24>
 48a:	4bb6      	ldr	r3, [pc, #728]	; (764 <operation_temp_run+0x2f4>)
 48c:	681b      	ldr	r3, [r3, #0]
 48e:	185b      	adds	r3, r3, r1
 490:	2b04      	cmp	r3, #4
 492:	d801      	bhi.n	498 <operation_temp_run+0x28>
 494:	f7ff ff2e 	bl	2f4 <radio_power_on>
 498:	4bb3      	ldr	r3, [pc, #716]	; (768 <operation_temp_run+0x2f8>)
 49a:	4ab4      	ldr	r2, [pc, #720]	; (76c <operation_temp_run+0x2fc>)
 49c:	6819      	ldr	r1, [r3, #0]
 49e:	2005      	movs	r0, #5
 4a0:	400a      	ands	r2, r1
 4a2:	601a      	str	r2, [r3, #0]
 4a4:	681a      	ldr	r2, [r3, #0]
 4a6:	2112      	movs	r1, #18
 4a8:	f7ff fe7a 	bl	1a0 <mbus_remote_register_write>
 4ac:	4bb0      	ldr	r3, [pc, #704]	; (770 <operation_temp_run+0x300>)
 4ae:	2101      	movs	r1, #1
 4b0:	6818      	ldr	r0, [r3, #0]
 4b2:	1c0a      	adds	r2, r1, #0
 4b4:	b280      	uxth	r0, r0
 4b6:	f7ff fe03 	bl	c0 <set_wakeup_timer>
 4ba:	f7ff fe6b 	bl	194 <mbus_sleep_all>
 4be:	e7fe      	b.n	4be <operation_temp_run+0x4e>
 4c0:	681c      	ldr	r4, [r3, #0]
 4c2:	2c01      	cmp	r4, #1
 4c4:	d115      	bne.n	4f2 <operation_temp_run+0x82>
 4c6:	2203      	movs	r2, #3
 4c8:	601a      	str	r2, [r3, #0]
 4ca:	4ba7      	ldr	r3, [pc, #668]	; (768 <operation_temp_run+0x2f8>)
 4cc:	4aa9      	ldr	r2, [pc, #676]	; (774 <operation_temp_run+0x304>)
 4ce:	6819      	ldr	r1, [r3, #0]
 4d0:	2005      	movs	r0, #5
 4d2:	400a      	ands	r2, r1
 4d4:	601a      	str	r2, [r3, #0]
 4d6:	681a      	ldr	r2, [r3, #0]
 4d8:	2112      	movs	r1, #18
 4da:	f7ff fe61 	bl	1a0 <mbus_remote_register_write>
 4de:	4ba4      	ldr	r3, [pc, #656]	; (770 <operation_temp_run+0x300>)
 4e0:	1c21      	adds	r1, r4, #0
 4e2:	6818      	ldr	r0, [r3, #0]
 4e4:	1c22      	adds	r2, r4, #0
 4e6:	b280      	uxth	r0, r0
 4e8:	f7ff fdea 	bl	c0 <set_wakeup_timer>
 4ec:	f7ff fe52 	bl	194 <mbus_sleep_all>
 4f0:	e7fe      	b.n	4f0 <operation_temp_run+0x80>
 4f2:	681a      	ldr	r2, [r3, #0]
 4f4:	2a03      	cmp	r2, #3
 4f6:	d124      	bne.n	542 <operation_temp_run+0xd2>
 4f8:	4c9f      	ldr	r4, [pc, #636]	; (778 <operation_temp_run+0x308>)
 4fa:	2202      	movs	r2, #2
 4fc:	601a      	str	r2, [r3, #0]
 4fe:	6822      	ldr	r2, [r4, #0]
 500:	2380      	movs	r3, #128	; 0x80
 502:	03db      	lsls	r3, r3, #15
 504:	4313      	orrs	r3, r2
 506:	6023      	str	r3, [r4, #0]
 508:	6822      	ldr	r2, [r4, #0]
 50a:	4b9c      	ldr	r3, [pc, #624]	; (77c <operation_temp_run+0x30c>)
 50c:	210e      	movs	r1, #14
 50e:	4013      	ands	r3, r2
 510:	6023      	str	r3, [r4, #0]
 512:	6822      	ldr	r2, [r4, #0]
 514:	2005      	movs	r0, #5
 516:	f7ff fe43 	bl	1a0 <mbus_remote_register_write>
 51a:	2064      	movs	r0, #100	; 0x64
 51c:	f7ff fdbb 	bl	96 <delay>
 520:	6822      	ldr	r2, [r4, #0]
 522:	4b97      	ldr	r3, [pc, #604]	; (780 <operation_temp_run+0x310>)
 524:	2005      	movs	r0, #5
 526:	4013      	ands	r3, r2
 528:	6023      	str	r3, [r4, #0]
 52a:	6822      	ldr	r2, [r4, #0]
 52c:	210e      	movs	r1, #14
 52e:	f7ff fe37 	bl	1a0 <mbus_remote_register_write>
 532:	2101      	movs	r1, #1
 534:	200a      	movs	r0, #10
 536:	1c0a      	adds	r2, r1, #0
 538:	f7ff fdc2 	bl	c0 <set_wakeup_timer>
 53c:	f7ff fe2a 	bl	194 <mbus_sleep_all>
 540:	e7fe      	b.n	540 <operation_temp_run+0xd0>
 542:	681a      	ldr	r2, [r3, #0]
 544:	2a02      	cmp	r2, #2
 546:	d11f      	bne.n	588 <operation_temp_run+0x118>
 548:	4d8e      	ldr	r5, [pc, #568]	; (784 <operation_temp_run+0x314>)
 54a:	2300      	movs	r3, #0
 54c:	4c8e      	ldr	r4, [pc, #568]	; (788 <operation_temp_run+0x318>)
 54e:	602b      	str	r3, [r5, #0]
 550:	682b      	ldr	r3, [r5, #0]
 552:	2b00      	cmp	r3, #0
 554:	d004      	beq.n	560 <operation_temp_run+0xf0>
 556:	4a81      	ldr	r2, [pc, #516]	; (75c <operation_temp_run+0x2ec>)
 558:	2300      	movs	r3, #0
 55a:	602b      	str	r3, [r5, #0]
 55c:	6013      	str	r3, [r2, #0]
 55e:	e00f      	b.n	580 <operation_temp_run+0x110>
 560:	200a      	movs	r0, #10
 562:	f7ff fd98 	bl	96 <delay>
 566:	3c01      	subs	r4, #1
 568:	4888      	ldr	r0, [pc, #544]	; (78c <operation_temp_run+0x31c>)
 56a:	f7ff fd9d 	bl	a8 <config_timerwd>
 56e:	2c00      	cmp	r4, #0
 570:	d1ee      	bne.n	550 <operation_temp_run+0xe0>
 572:	20fa      	movs	r0, #250	; 0xfa
 574:	4986      	ldr	r1, [pc, #536]	; (790 <operation_temp_run+0x320>)
 576:	f7ff fde3 	bl	140 <mbus_write_message32>
 57a:	4b78      	ldr	r3, [pc, #480]	; (75c <operation_temp_run+0x2ec>)
 57c:	2201      	movs	r2, #1
 57e:	601a      	str	r2, [r3, #0]
 580:	4b75      	ldr	r3, [pc, #468]	; (758 <operation_temp_run+0x2e8>)
 582:	2206      	movs	r2, #6
 584:	601a      	str	r2, [r3, #0]
 586:	e0e5      	b.n	754 <operation_temp_run+0x2e4>
 588:	681b      	ldr	r3, [r3, #0]
 58a:	2b06      	cmp	r3, #6
 58c:	d000      	beq.n	590 <operation_temp_run+0x120>
 58e:	e0ce      	b.n	72e <operation_temp_run+0x2be>
 590:	4b72      	ldr	r3, [pc, #456]	; (75c <operation_temp_run+0x2ec>)
 592:	4a80      	ldr	r2, [pc, #512]	; (794 <operation_temp_run+0x324>)
 594:	681b      	ldr	r3, [r3, #0]
 596:	2b00      	cmp	r3, #0
 598:	d002      	beq.n	5a0 <operation_temp_run+0x130>
 59a:	4b7f      	ldr	r3, [pc, #508]	; (798 <operation_temp_run+0x328>)
 59c:	6013      	str	r3, [r2, #0]
 59e:	e002      	b.n	5a6 <operation_temp_run+0x136>
 5a0:	6811      	ldr	r1, [r2, #0]
 5a2:	4b7e      	ldr	r3, [pc, #504]	; (79c <operation_temp_run+0x32c>)
 5a4:	6019      	str	r1, [r3, #0]
 5a6:	4b7e      	ldr	r3, [pc, #504]	; (7a0 <operation_temp_run+0x330>)
 5a8:	4c6b      	ldr	r4, [pc, #428]	; (758 <operation_temp_run+0x2e8>)
 5aa:	6819      	ldr	r1, [r3, #0]
 5ac:	2904      	cmp	r1, #4
 5ae:	d80a      	bhi.n	5c6 <operation_temp_run+0x156>
 5b0:	6819      	ldr	r1, [r3, #0]
 5b2:	6810      	ldr	r0, [r2, #0]
 5b4:	4a7b      	ldr	r2, [pc, #492]	; (7a4 <operation_temp_run+0x334>)
 5b6:	0089      	lsls	r1, r1, #2
 5b8:	5088      	str	r0, [r1, r2]
 5ba:	681a      	ldr	r2, [r3, #0]
 5bc:	3201      	adds	r2, #1
 5be:	601a      	str	r2, [r3, #0]
 5c0:	2302      	movs	r3, #2
 5c2:	6023      	str	r3, [r4, #0]
 5c4:	e0c6      	b.n	754 <operation_temp_run+0x2e4>
 5c6:	2500      	movs	r5, #0
 5c8:	601d      	str	r5, [r3, #0]
 5ca:	f7ff fe41 	bl	250 <temp_power_off>
 5ce:	6025      	str	r5, [r4, #0]
 5d0:	4d64      	ldr	r5, [pc, #400]	; (764 <operation_temp_run+0x2f4>)
 5d2:	20cc      	movs	r0, #204	; 0xcc
 5d4:	6829      	ldr	r1, [r5, #0]
 5d6:	f7ff fdb3 	bl	140 <mbus_write_message32>
 5da:	2064      	movs	r0, #100	; 0x64
 5dc:	f7ff fd5b 	bl	96 <delay>
 5e0:	4b71      	ldr	r3, [pc, #452]	; (7a8 <operation_temp_run+0x338>)
 5e2:	20cc      	movs	r0, #204	; 0xcc
 5e4:	6819      	ldr	r1, [r3, #0]
 5e6:	f7ff fdab 	bl	140 <mbus_write_message32>
 5ea:	2064      	movs	r0, #100	; 0x64
 5ec:	f7ff fd53 	bl	96 <delay>
 5f0:	4c6c      	ldr	r4, [pc, #432]	; (7a4 <operation_temp_run+0x334>)
 5f2:	20c0      	movs	r0, #192	; 0xc0
 5f4:	6821      	ldr	r1, [r4, #0]
 5f6:	f7ff fda3 	bl	140 <mbus_write_message32>
 5fa:	2064      	movs	r0, #100	; 0x64
 5fc:	f7ff fd4b 	bl	96 <delay>
 600:	6861      	ldr	r1, [r4, #4]
 602:	20c1      	movs	r0, #193	; 0xc1
 604:	f7ff fd9c 	bl	140 <mbus_write_message32>
 608:	2064      	movs	r0, #100	; 0x64
 60a:	f7ff fd44 	bl	96 <delay>
 60e:	68a1      	ldr	r1, [r4, #8]
 610:	20c2      	movs	r0, #194	; 0xc2
 612:	f7ff fd95 	bl	140 <mbus_write_message32>
 616:	2064      	movs	r0, #100	; 0x64
 618:	f7ff fd3d 	bl	96 <delay>
 61c:	68e1      	ldr	r1, [r4, #12]
 61e:	20c3      	movs	r0, #195	; 0xc3
 620:	f7ff fd8e 	bl	140 <mbus_write_message32>
 624:	2064      	movs	r0, #100	; 0x64
 626:	f7ff fd36 	bl	96 <delay>
 62a:	6921      	ldr	r1, [r4, #16]
 62c:	20c4      	movs	r0, #196	; 0xc4
 62e:	f7ff fd87 	bl	140 <mbus_write_message32>
 632:	682b      	ldr	r3, [r5, #0]
 634:	4a5d      	ldr	r2, [pc, #372]	; (7ac <operation_temp_run+0x33c>)
 636:	3301      	adds	r3, #1
 638:	602b      	str	r3, [r5, #0]
 63a:	4b5d      	ldr	r3, [pc, #372]	; (7b0 <operation_temp_run+0x340>)
 63c:	6819      	ldr	r1, [r3, #0]
 63e:	4291      	cmp	r1, r2
 640:	d80b      	bhi.n	65a <operation_temp_run+0x1ea>
 642:	4a54      	ldr	r2, [pc, #336]	; (794 <operation_temp_run+0x324>)
 644:	6819      	ldr	r1, [r3, #0]
 646:	6810      	ldr	r0, [r2, #0]
 648:	4a5a      	ldr	r2, [pc, #360]	; (7b4 <operation_temp_run+0x344>)
 64a:	0089      	lsls	r1, r1, #2
 64c:	5088      	str	r0, [r1, r2]
 64e:	6819      	ldr	r1, [r3, #0]
 650:	4a59      	ldr	r2, [pc, #356]	; (7b8 <operation_temp_run+0x348>)
 652:	6011      	str	r1, [r2, #0]
 654:	681a      	ldr	r2, [r3, #0]
 656:	3201      	adds	r2, #1
 658:	601a      	str	r2, [r3, #0]
 65a:	4b41      	ldr	r3, [pc, #260]	; (760 <operation_temp_run+0x2f0>)
 65c:	681b      	ldr	r3, [r3, #0]
 65e:	2b00      	cmp	r3, #0
 660:	d004      	beq.n	66c <operation_temp_run+0x1fc>
 662:	4b4c      	ldr	r3, [pc, #304]	; (794 <operation_temp_run+0x324>)
 664:	2000      	movs	r0, #0
 666:	6819      	ldr	r1, [r3, #0]
 668:	f7ff fea0 	bl	3ac <send_radio_data_ppm>
 66c:	4b3d      	ldr	r3, [pc, #244]	; (764 <operation_temp_run+0x2f4>)
 66e:	681b      	ldr	r3, [r3, #0]
 670:	2b04      	cmp	r3, #4
 672:	d809      	bhi.n	688 <operation_temp_run+0x218>
 674:	4851      	ldr	r0, [pc, #324]	; (7bc <operation_temp_run+0x34c>)
 676:	f7ff fd0e 	bl	96 <delay>
 67a:	2001      	movs	r0, #1
 67c:	4950      	ldr	r1, [pc, #320]	; (7c0 <operation_temp_run+0x350>)
 67e:	f7ff fe95 	bl	3ac <send_radio_data_ppm>
 682:	4b50      	ldr	r3, [pc, #320]	; (7c4 <operation_temp_run+0x354>)
 684:	6818      	ldr	r0, [r3, #0]
 686:	e028      	b.n	6da <operation_temp_run+0x26a>
 688:	4844      	ldr	r0, [pc, #272]	; (79c <operation_temp_run+0x32c>)
 68a:	494f      	ldr	r1, [pc, #316]	; (7c8 <operation_temp_run+0x358>)
 68c:	6804      	ldr	r4, [r0, #0]
 68e:	680a      	ldr	r2, [r1, #0]
 690:	4b4e      	ldr	r3, [pc, #312]	; (7cc <operation_temp_run+0x35c>)
 692:	3264      	adds	r2, #100	; 0x64
 694:	4294      	cmp	r4, r2
 696:	d201      	bcs.n	69c <operation_temp_run+0x22c>
 698:	2201      	movs	r2, #1
 69a:	e018      	b.n	6ce <operation_temp_run+0x25e>
 69c:	6805      	ldr	r5, [r0, #0]
 69e:	680c      	ldr	r4, [r1, #0]
 6a0:	4a4b      	ldr	r2, [pc, #300]	; (7d0 <operation_temp_run+0x360>)
 6a2:	34c8      	adds	r4, #200	; 0xc8
 6a4:	42a5      	cmp	r5, r4
 6a6:	d201      	bcs.n	6ac <operation_temp_run+0x23c>
 6a8:	6812      	ldr	r2, [r2, #0]
 6aa:	e010      	b.n	6ce <operation_temp_run+0x25e>
 6ac:	6805      	ldr	r5, [r0, #0]
 6ae:	680c      	ldr	r4, [r1, #0]
 6b0:	34f5      	adds	r4, #245	; 0xf5
 6b2:	34ff      	adds	r4, #255	; 0xff
 6b4:	42a5      	cmp	r5, r4
 6b6:	d201      	bcs.n	6bc <operation_temp_run+0x24c>
 6b8:	6852      	ldr	r2, [r2, #4]
 6ba:	e008      	b.n	6ce <operation_temp_run+0x25e>
 6bc:	6800      	ldr	r0, [r0, #0]
 6be:	4c45      	ldr	r4, [pc, #276]	; (7d4 <operation_temp_run+0x364>)
 6c0:	6809      	ldr	r1, [r1, #0]
 6c2:	1909      	adds	r1, r1, r4
 6c4:	4288      	cmp	r0, r1
 6c6:	d201      	bcs.n	6cc <operation_temp_run+0x25c>
 6c8:	6892      	ldr	r2, [r2, #8]
 6ca:	e000      	b.n	6ce <operation_temp_run+0x25e>
 6cc:	68d2      	ldr	r2, [r2, #12]
 6ce:	601a      	str	r2, [r3, #0]
 6d0:	4b41      	ldr	r3, [pc, #260]	; (7d8 <operation_temp_run+0x368>)
 6d2:	6818      	ldr	r0, [r3, #0]
 6d4:	4b3d      	ldr	r3, [pc, #244]	; (7cc <operation_temp_run+0x35c>)
 6d6:	681b      	ldr	r3, [r3, #0]
 6d8:	4358      	muls	r0, r3
 6da:	2101      	movs	r1, #1
 6dc:	b280      	uxth	r0, r0
 6de:	1c0a      	adds	r2, r1, #0
 6e0:	f7ff fcee 	bl	c0 <set_wakeup_timer>
 6e4:	4b3d      	ldr	r3, [pc, #244]	; (7dc <operation_temp_run+0x36c>)
 6e6:	681b      	ldr	r3, [r3, #0]
 6e8:	2b00      	cmp	r3, #0
 6ea:	d004      	beq.n	6f6 <operation_temp_run+0x286>
 6ec:	4b3c      	ldr	r3, [pc, #240]	; (7e0 <operation_temp_run+0x370>)
 6ee:	2200      	movs	r2, #0
 6f0:	601a      	str	r2, [r3, #0]
 6f2:	f7ff fd63 	bl	1bc <radio_power_off>
 6f6:	4a3b      	ldr	r2, [pc, #236]	; (7e4 <operation_temp_run+0x374>)
 6f8:	6813      	ldr	r3, [r2, #0]
 6fa:	2b00      	cmp	r3, #0
 6fc:	d006      	beq.n	70c <operation_temp_run+0x29c>
 6fe:	4b27      	ldr	r3, [pc, #156]	; (79c <operation_temp_run+0x32c>)
 700:	6819      	ldr	r1, [r3, #0]
 702:	4b31      	ldr	r3, [pc, #196]	; (7c8 <operation_temp_run+0x358>)
 704:	6019      	str	r1, [r3, #0]
 706:	2300      	movs	r3, #0
 708:	6013      	str	r3, [r2, #0]
 70a:	e00a      	b.n	722 <operation_temp_run+0x2b2>
 70c:	4a26      	ldr	r2, [pc, #152]	; (7a8 <operation_temp_run+0x338>)
 70e:	6811      	ldr	r1, [r2, #0]
 710:	2900      	cmp	r1, #0
 712:	d009      	beq.n	728 <operation_temp_run+0x2b8>
 714:	4913      	ldr	r1, [pc, #76]	; (764 <operation_temp_run+0x2f4>)
 716:	2032      	movs	r0, #50	; 0x32
 718:	6809      	ldr	r1, [r1, #0]
 71a:	6812      	ldr	r2, [r2, #0]
 71c:	4090      	lsls	r0, r2
 71e:	4281      	cmp	r1, r0
 720:	d902      	bls.n	728 <operation_temp_run+0x2b8>
 722:	4a31      	ldr	r2, [pc, #196]	; (7e8 <operation_temp_run+0x378>)
 724:	6013      	str	r3, [r2, #0]
 726:	e013      	b.n	750 <operation_temp_run+0x2e0>
 728:	f7ff fd34 	bl	194 <mbus_sleep_all>
 72c:	e7fe      	b.n	72c <operation_temp_run+0x2bc>
 72e:	4b12      	ldr	r3, [pc, #72]	; (778 <operation_temp_run+0x308>)
 730:	4a2e      	ldr	r2, [pc, #184]	; (7ec <operation_temp_run+0x37c>)
 732:	6819      	ldr	r1, [r3, #0]
 734:	2005      	movs	r0, #5
 736:	400a      	ands	r2, r1
 738:	601a      	str	r2, [r3, #0]
 73a:	6819      	ldr	r1, [r3, #0]
 73c:	2280      	movs	r2, #128	; 0x80
 73e:	0392      	lsls	r2, r2, #14
 740:	430a      	orrs	r2, r1
 742:	601a      	str	r2, [r3, #0]
 744:	681a      	ldr	r2, [r3, #0]
 746:	210e      	movs	r1, #14
 748:	f7ff fd2a 	bl	1a0 <mbus_remote_register_write>
 74c:	f7ff fd80 	bl	250 <temp_power_off>
 750:	f7ff fe16 	bl	380 <operation_sleep_notimer>
 754:	bd38      	pop	{r3, r4, r5, pc}
 756:	46c0      	nop			; (mov r8, r8)
 758:	0000139c 	.word	0x0000139c
 75c:	00001388 	.word	0x00001388
 760:	00001394 	.word	0x00001394
 764:	00001378 	.word	0x00001378
 768:	00001358 	.word	0x00001358
 76c:	fffff7ff 	.word	0xfffff7ff
 770:	00001320 	.word	0x00001320
 774:	ffffefff 	.word	0xffffefff
 778:	0000134c 	.word	0x0000134c
 77c:	ffdfffff 	.word	0xffdfffff
 780:	fffeffff 	.word	0xfffeffff
 784:	000013a4 	.word	0x000013a4
 788:	00004e20 	.word	0x00004e20
 78c:	000fffff 	.word	0x000fffff
 790:	fafafafa 	.word	0xfafafafa
 794:	000013b8 	.word	0x000013b8
 798:	00000666 	.word	0x00000666
 79c:	00001324 	.word	0x00001324
 7a0:	000013ac 	.word	0x000013ac
 7a4:	00001a08 	.word	0x00001a08
 7a8:	00001390 	.word	0x00001390
 7ac:	0000018f 	.word	0x0000018f
 7b0:	00001380 	.word	0x00001380
 7b4:	000013c4 	.word	0x000013c4
 7b8:	00001374 	.word	0x00001374
 7bc:	00001388 	.word	0x00001388
 7c0:	00faf000 	.word	0x00faf000
 7c4:	000013a0 	.word	0x000013a0
 7c8:	00001350 	.word	0x00001350
 7cc:	0000132c 	.word	0x0000132c
 7d0:	00001360 	.word	0x00001360
 7d4:	000005dc 	.word	0x000005dc
 7d8:	000013bc 	.word	0x000013bc
 7dc:	000013c0 	.word	0x000013c0
 7e0:	00001370 	.word	0x00001370
 7e4:	0000138c 	.word	0x0000138c
 7e8:	000013b4 	.word	0x000013b4
 7ec:	ffbfffff 	.word	0xffbfffff

Disassembly of section .text.handler_ext_int_0:

000007f0 <handler_ext_int_0>:
 7f0:	4b01      	ldr	r3, [pc, #4]	; (7f8 <handler_ext_int_0+0x8>)
 7f2:	2201      	movs	r2, #1
 7f4:	601a      	str	r2, [r3, #0]
 7f6:	4770      	bx	lr
 7f8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

000007fc <handler_ext_int_1>:
 7fc:	4b01      	ldr	r3, [pc, #4]	; (804 <handler_ext_int_1+0x8>)
 7fe:	2202      	movs	r2, #2
 800:	601a      	str	r2, [r3, #0]
 802:	4770      	bx	lr
 804:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

00000808 <handler_ext_int_2>:
 808:	4b05      	ldr	r3, [pc, #20]	; (820 <handler_ext_int_2+0x18>)
 80a:	2204      	movs	r2, #4
 80c:	601a      	str	r2, [r3, #0]
 80e:	4b05      	ldr	r3, [pc, #20]	; (824 <handler_ext_int_2+0x1c>)
 810:	2210      	movs	r2, #16
 812:	601a      	str	r2, [r3, #0]
 814:	23a0      	movs	r3, #160	; 0xa0
 816:	061b      	lsls	r3, r3, #24
 818:	681a      	ldr	r2, [r3, #0]
 81a:	4b03      	ldr	r3, [pc, #12]	; (828 <handler_ext_int_2+0x20>)
 81c:	601a      	str	r2, [r3, #0]
 81e:	4770      	bx	lr
 820:	e000e280 	.word	0xe000e280
 824:	000013a4 	.word	0x000013a4
 828:	000013b8 	.word	0x000013b8

Disassembly of section .text.handler_ext_int_3:

0000082c <handler_ext_int_3>:
 82c:	4b03      	ldr	r3, [pc, #12]	; (83c <handler_ext_int_3+0x10>)
 82e:	2208      	movs	r2, #8
 830:	601a      	str	r2, [r3, #0]
 832:	4b03      	ldr	r3, [pc, #12]	; (840 <handler_ext_int_3+0x14>)
 834:	2211      	movs	r2, #17
 836:	601a      	str	r2, [r3, #0]
 838:	4770      	bx	lr
 83a:	46c0      	nop			; (mov r8, r8)
 83c:	e000e280 	.word	0xe000e280
 840:	000013a4 	.word	0x000013a4

Disassembly of section .text.handler_ext_int_4:

00000844 <handler_ext_int_4>:
 844:	4b03      	ldr	r3, [pc, #12]	; (854 <handler_ext_int_4+0x10>)
 846:	2210      	movs	r2, #16
 848:	601a      	str	r2, [r3, #0]
 84a:	4b03      	ldr	r3, [pc, #12]	; (858 <handler_ext_int_4+0x14>)
 84c:	2212      	movs	r2, #18
 84e:	601a      	str	r2, [r3, #0]
 850:	4770      	bx	lr
 852:	46c0      	nop			; (mov r8, r8)
 854:	e000e280 	.word	0xe000e280
 858:	000013a4 	.word	0x000013a4

Disassembly of section .text.handler_ext_int_5:

0000085c <handler_ext_int_5>:
 85c:	4b03      	ldr	r3, [pc, #12]	; (86c <handler_ext_int_5+0x10>)
 85e:	2220      	movs	r2, #32
 860:	601a      	str	r2, [r3, #0]
 862:	4b03      	ldr	r3, [pc, #12]	; (870 <handler_ext_int_5+0x14>)
 864:	2213      	movs	r2, #19
 866:	601a      	str	r2, [r3, #0]
 868:	4770      	bx	lr
 86a:	46c0      	nop			; (mov r8, r8)
 86c:	e000e280 	.word	0xe000e280
 870:	000013a4 	.word	0x000013a4

Disassembly of section .text.handler_ext_int_6:

00000874 <handler_ext_int_6>:
 874:	4b03      	ldr	r3, [pc, #12]	; (884 <handler_ext_int_6+0x10>)
 876:	2240      	movs	r2, #64	; 0x40
 878:	601a      	str	r2, [r3, #0]
 87a:	4b03      	ldr	r3, [pc, #12]	; (888 <handler_ext_int_6+0x14>)
 87c:	2214      	movs	r2, #20
 87e:	601a      	str	r2, [r3, #0]
 880:	4770      	bx	lr
 882:	46c0      	nop			; (mov r8, r8)
 884:	e000e280 	.word	0xe000e280
 888:	000013a4 	.word	0x000013a4

Disassembly of section .text.handler_ext_int_7:

0000088c <handler_ext_int_7>:
 88c:	4b03      	ldr	r3, [pc, #12]	; (89c <handler_ext_int_7+0x10>)
 88e:	2280      	movs	r2, #128	; 0x80
 890:	601a      	str	r2, [r3, #0]
 892:	4b03      	ldr	r3, [pc, #12]	; (8a0 <handler_ext_int_7+0x14>)
 894:	2215      	movs	r2, #21
 896:	601a      	str	r2, [r3, #0]
 898:	4770      	bx	lr
 89a:	46c0      	nop			; (mov r8, r8)
 89c:	e000e280 	.word	0xe000e280
 8a0:	000013a4 	.word	0x000013a4

Disassembly of section .text.handler_ext_int_8:

000008a4 <handler_ext_int_8>:
 8a4:	4b03      	ldr	r3, [pc, #12]	; (8b4 <handler_ext_int_8+0x10>)
 8a6:	2280      	movs	r2, #128	; 0x80
 8a8:	0052      	lsls	r2, r2, #1
 8aa:	601a      	str	r2, [r3, #0]
 8ac:	4b02      	ldr	r3, [pc, #8]	; (8b8 <handler_ext_int_8+0x14>)
 8ae:	2216      	movs	r2, #22
 8b0:	601a      	str	r2, [r3, #0]
 8b2:	4770      	bx	lr
 8b4:	e000e280 	.word	0xe000e280
 8b8:	000013a4 	.word	0x000013a4

Disassembly of section .text.handler_ext_int_9:

000008bc <handler_ext_int_9>:
 8bc:	4b03      	ldr	r3, [pc, #12]	; (8cc <handler_ext_int_9+0x10>)
 8be:	2280      	movs	r2, #128	; 0x80
 8c0:	0092      	lsls	r2, r2, #2
 8c2:	601a      	str	r2, [r3, #0]
 8c4:	4b02      	ldr	r3, [pc, #8]	; (8d0 <handler_ext_int_9+0x14>)
 8c6:	2217      	movs	r2, #23
 8c8:	601a      	str	r2, [r3, #0]
 8ca:	4770      	bx	lr
 8cc:	e000e280 	.word	0xe000e280
 8d0:	000013a4 	.word	0x000013a4

Disassembly of section .text.handler_ext_int_10:

000008d4 <handler_ext_int_10>:
 8d4:	4b02      	ldr	r3, [pc, #8]	; (8e0 <handler_ext_int_10+0xc>)
 8d6:	2280      	movs	r2, #128	; 0x80
 8d8:	00d2      	lsls	r2, r2, #3
 8da:	601a      	str	r2, [r3, #0]
 8dc:	4770      	bx	lr
 8de:	46c0      	nop			; (mov r8, r8)
 8e0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

000008e4 <handler_ext_int_11>:
 8e4:	4b02      	ldr	r3, [pc, #8]	; (8f0 <handler_ext_int_11+0xc>)
 8e6:	2280      	movs	r2, #128	; 0x80
 8e8:	0112      	lsls	r2, r2, #4
 8ea:	601a      	str	r2, [r3, #0]
 8ec:	4770      	bx	lr
 8ee:	46c0      	nop			; (mov r8, r8)
 8f0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

000008f4 <handler_ext_int_12>:
 8f4:	4b02      	ldr	r3, [pc, #8]	; (900 <handler_ext_int_12+0xc>)
 8f6:	2280      	movs	r2, #128	; 0x80
 8f8:	0152      	lsls	r2, r2, #5
 8fa:	601a      	str	r2, [r3, #0]
 8fc:	4770      	bx	lr
 8fe:	46c0      	nop			; (mov r8, r8)
 900:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

00000904 <handler_ext_int_13>:
 904:	4b02      	ldr	r3, [pc, #8]	; (910 <handler_ext_int_13+0xc>)
 906:	2280      	movs	r2, #128	; 0x80
 908:	0192      	lsls	r2, r2, #6
 90a:	601a      	str	r2, [r3, #0]
 90c:	4770      	bx	lr
 90e:	46c0      	nop			; (mov r8, r8)
 910:	e000e280 	.word	0xe000e280

Disassembly of section .text.startup.main:

00000914 <main>:
     914:	b5f0      	push	{r4, r5, r6, r7, lr}
     916:	b085      	sub	sp, #20
     918:	f7ff fbe8 	bl	ec <enable_reg_irq>
     91c:	48f5      	ldr	r0, [pc, #980]	; (cf4 <main+0x3e0>)
     91e:	f7ff fbc3 	bl	a8 <config_timerwd>
     922:	4af5      	ldr	r2, [pc, #980]	; (cf8 <main+0x3e4>)
     924:	49f5      	ldr	r1, [pc, #980]	; (cfc <main+0x3e8>)
     926:	6813      	ldr	r3, [r2, #0]
     928:	428b      	cmp	r3, r1
     92a:	d100      	bne.n	92e <main+0x1a>
     92c:	e17f      	b.n	c2e <main+0x31a>
     92e:	4bf4      	ldr	r3, [pc, #976]	; (d00 <main+0x3ec>)
     930:	2780      	movs	r7, #128	; 0x80
     932:	6818      	ldr	r0, [r3, #0]
     934:	017f      	lsls	r7, r7, #5
     936:	4338      	orrs	r0, r7
     938:	6018      	str	r0, [r3, #0]
     93a:	681c      	ldr	r4, [r3, #0]
     93c:	48f1      	ldr	r0, [pc, #964]	; (d04 <main+0x3f0>)
     93e:	250f      	movs	r5, #15
     940:	4020      	ands	r0, r4
     942:	2480      	movs	r4, #128	; 0x80
     944:	01a4      	lsls	r4, r4, #6
     946:	4320      	orrs	r0, r4
     948:	6018      	str	r0, [r3, #0]
     94a:	681c      	ldr	r4, [r3, #0]
     94c:	48ee      	ldr	r0, [pc, #952]	; (d08 <main+0x3f4>)
     94e:	2606      	movs	r6, #6
     950:	4020      	ands	r0, r4
     952:	2480      	movs	r4, #128	; 0x80
     954:	0224      	lsls	r4, r4, #8
     956:	4320      	orrs	r0, r4
     958:	6018      	str	r0, [r3, #0]
     95a:	681c      	ldr	r4, [r3, #0]
     95c:	20c0      	movs	r0, #192	; 0xc0
     95e:	02c0      	lsls	r0, r0, #11
     960:	4320      	orrs	r0, r4
     962:	6018      	str	r0, [r3, #0]
     964:	6818      	ldr	r0, [r3, #0]
     966:	4be9      	ldr	r3, [pc, #932]	; (d0c <main+0x3f8>)
     968:	2400      	movs	r4, #0
     96a:	6018      	str	r0, [r3, #0]
     96c:	4be8      	ldr	r3, [pc, #928]	; (d10 <main+0x3fc>)
     96e:	2064      	movs	r0, #100	; 0x64
     970:	601c      	str	r4, [r3, #0]
     972:	4be8      	ldr	r3, [pc, #928]	; (d14 <main+0x400>)
     974:	6011      	str	r1, [r2, #0]
     976:	601c      	str	r4, [r3, #0]
     978:	4be7      	ldr	r3, [pc, #924]	; (d18 <main+0x404>)
     97a:	601c      	str	r4, [r3, #0]
     97c:	4be7      	ldr	r3, [pc, #924]	; (d1c <main+0x408>)
     97e:	601c      	str	r4, [r3, #0]
     980:	f7ff fb89 	bl	96 <delay>
     984:	2004      	movs	r0, #4
     986:	f7ff fbfb 	bl	180 <mbus_enumerate>
     98a:	2064      	movs	r0, #100	; 0x64
     98c:	f7ff fb83 	bl	96 <delay>
     990:	2005      	movs	r0, #5
     992:	f7ff fbf5 	bl	180 <mbus_enumerate>
     996:	2064      	movs	r0, #100	; 0x64
     998:	f7ff fb7d 	bl	96 <delay>
     99c:	2003      	movs	r0, #3
     99e:	f7ff fbef 	bl	180 <mbus_enumerate>
     9a2:	2064      	movs	r0, #100	; 0x64
     9a4:	f7ff fb77 	bl	96 <delay>
     9a8:	2006      	movs	r0, #6
     9aa:	f7ff fbe9 	bl	180 <mbus_enumerate>
     9ae:	2064      	movs	r0, #100	; 0x64
     9b0:	f7ff fb71 	bl	96 <delay>
     9b4:	2117      	movs	r1, #23
     9b6:	4ada      	ldr	r2, [pc, #872]	; (d20 <main+0x40c>)
     9b8:	2006      	movs	r0, #6
     9ba:	f7ff fbf1 	bl	1a0 <mbus_remote_register_write>
     9be:	2064      	movs	r0, #100	; 0x64
     9c0:	f7ff fb69 	bl	96 <delay>
     9c4:	2117      	movs	r1, #23
     9c6:	4ad6      	ldr	r2, [pc, #856]	; (d20 <main+0x40c>)
     9c8:	2006      	movs	r0, #6
     9ca:	f7ff fbe9 	bl	1a0 <mbus_remote_register_write>
     9ce:	2064      	movs	r0, #100	; 0x64
     9d0:	f7ff fb61 	bl	96 <delay>
     9d4:	2118      	movs	r1, #24
     9d6:	4ad3      	ldr	r2, [pc, #844]	; (d24 <main+0x410>)
     9d8:	2006      	movs	r0, #6
     9da:	f7ff fbe1 	bl	1a0 <mbus_remote_register_write>
     9de:	2064      	movs	r0, #100	; 0x64
     9e0:	f7ff fb59 	bl	96 <delay>
     9e4:	2119      	movs	r1, #25
     9e6:	4ad0      	ldr	r2, [pc, #832]	; (d28 <main+0x414>)
     9e8:	2006      	movs	r0, #6
     9ea:	f7ff fbd9 	bl	1a0 <mbus_remote_register_write>
     9ee:	2064      	movs	r0, #100	; 0x64
     9f0:	f7ff fb51 	bl	96 <delay>
     9f4:	211a      	movs	r1, #26
     9f6:	4acd      	ldr	r2, [pc, #820]	; (d2c <main+0x418>)
     9f8:	2006      	movs	r0, #6
     9fa:	f7ff fbd1 	bl	1a0 <mbus_remote_register_write>
     9fe:	2064      	movs	r0, #100	; 0x64
     a00:	f7ff fb49 	bl	96 <delay>
     a04:	2115      	movs	r1, #21
     a06:	4aca      	ldr	r2, [pc, #808]	; (d30 <main+0x41c>)
     a08:	2006      	movs	r0, #6
     a0a:	f7ff fbc9 	bl	1a0 <mbus_remote_register_write>
     a0e:	2064      	movs	r0, #100	; 0x64
     a10:	f7ff fb41 	bl	96 <delay>
     a14:	2116      	movs	r1, #22
     a16:	4ac7      	ldr	r2, [pc, #796]	; (d34 <main+0x420>)
     a18:	2006      	movs	r0, #6
     a1a:	f7ff fbc1 	bl	1a0 <mbus_remote_register_write>
     a1e:	2064      	movs	r0, #100	; 0x64
     a20:	f7ff fb39 	bl	96 <delay>
     a24:	2105      	movs	r1, #5
     a26:	4ac4      	ldr	r2, [pc, #784]	; (d38 <main+0x424>)
     a28:	2006      	movs	r0, #6
     a2a:	f7ff fbb9 	bl	1a0 <mbus_remote_register_write>
     a2e:	2064      	movs	r0, #100	; 0x64
     a30:	f7ff fb31 	bl	96 <delay>
     a34:	2105      	movs	r1, #5
     a36:	4ac1      	ldr	r2, [pc, #772]	; (d3c <main+0x428>)
     a38:	2006      	movs	r0, #6
     a3a:	f7ff fbb1 	bl	1a0 <mbus_remote_register_write>
     a3e:	2064      	movs	r0, #100	; 0x64
     a40:	f7ff fb29 	bl	96 <delay>
     a44:	2136      	movs	r1, #54	; 0x36
     a46:	4abe      	ldr	r2, [pc, #760]	; (d40 <main+0x42c>)
     a48:	2006      	movs	r0, #6
     a4a:	f7ff fba9 	bl	1a0 <mbus_remote_register_write>
     a4e:	2064      	movs	r0, #100	; 0x64
     a50:	f7ff fb21 	bl	96 <delay>
     a54:	f7ff fc30 	bl	2b8 <reset_pmu_solar_short>
     a58:	4bba      	ldr	r3, [pc, #744]	; (d44 <main+0x430>)
     a5a:	2005      	movs	r0, #5
     a5c:	681a      	ldr	r2, [r3, #0]
     a5e:	2119      	movs	r1, #25
     a60:	0e12      	lsrs	r2, r2, #24
     a62:	0612      	lsls	r2, r2, #24
     a64:	433a      	orrs	r2, r7
     a66:	601a      	str	r2, [r3, #0]
     a68:	681a      	ldr	r2, [r3, #0]
     a6a:	f7ff fb99 	bl	1a0 <mbus_remote_register_write>
     a6e:	4bb6      	ldr	r3, [pc, #728]	; (d48 <main+0x434>)
     a70:	2280      	movs	r2, #128	; 0x80
     a72:	6819      	ldr	r1, [r3, #0]
     a74:	0412      	lsls	r2, r2, #16
     a76:	430a      	orrs	r2, r1
     a78:	601a      	str	r2, [r3, #0]
     a7a:	6819      	ldr	r1, [r3, #0]
     a7c:	4ab3      	ldr	r2, [pc, #716]	; (d4c <main+0x438>)
     a7e:	2005      	movs	r0, #5
     a80:	400a      	ands	r2, r1
     a82:	21c0      	movs	r1, #192	; 0xc0
     a84:	0309      	lsls	r1, r1, #12
     a86:	430a      	orrs	r2, r1
     a88:	601a      	str	r2, [r3, #0]
     a8a:	681a      	ldr	r2, [r3, #0]
     a8c:	21f0      	movs	r1, #240	; 0xf0
     a8e:	438a      	bics	r2, r1
     a90:	601a      	str	r2, [r3, #0]
     a92:	681a      	ldr	r2, [r3, #0]
     a94:	210e      	movs	r1, #14
     a96:	43aa      	bics	r2, r5
     a98:	601a      	str	r2, [r3, #0]
     a9a:	681a      	ldr	r2, [r3, #0]
     a9c:	f7ff fb80 	bl	1a0 <mbus_remote_register_write>
     aa0:	4bab      	ldr	r3, [pc, #684]	; (d50 <main+0x43c>)
     aa2:	2170      	movs	r1, #112	; 0x70
     aa4:	681a      	ldr	r2, [r3, #0]
     aa6:	2005      	movs	r0, #5
     aa8:	430a      	orrs	r2, r1
     aaa:	601a      	str	r2, [r3, #0]
     aac:	6819      	ldr	r1, [r3, #0]
     aae:	4aa9      	ldr	r2, [pc, #676]	; (d54 <main+0x440>)
     ab0:	400a      	ands	r2, r1
     ab2:	601a      	str	r2, [r3, #0]
     ab4:	681a      	ldr	r2, [r3, #0]
     ab6:	1c29      	adds	r1, r5, #0
     ab8:	43aa      	bics	r2, r5
     aba:	4332      	orrs	r2, r6
     abc:	601a      	str	r2, [r3, #0]
     abe:	681a      	ldr	r2, [r3, #0]
     ac0:	f7ff fb6e 	bl	1a0 <mbus_remote_register_write>
     ac4:	4ba4      	ldr	r3, [pc, #656]	; (d58 <main+0x444>)
     ac6:	2180      	movs	r1, #128	; 0x80
     ac8:	681a      	ldr	r2, [r3, #0]
     aca:	0109      	lsls	r1, r1, #4
     acc:	430a      	orrs	r2, r1
     ace:	601a      	str	r2, [r3, #0]
     ad0:	681a      	ldr	r2, [r3, #0]
     ad2:	2005      	movs	r0, #5
     ad4:	4317      	orrs	r7, r2
     ad6:	601f      	str	r7, [r3, #0]
     ad8:	6819      	ldr	r1, [r3, #0]
     ada:	2280      	movs	r2, #128	; 0x80
     adc:	0392      	lsls	r2, r2, #14
     ade:	430a      	orrs	r2, r1
     ae0:	601a      	str	r2, [r3, #0]
     ae2:	6819      	ldr	r1, [r3, #0]
     ae4:	22c0      	movs	r2, #192	; 0xc0
     ae6:	01d2      	lsls	r2, r2, #7
     ae8:	430a      	orrs	r2, r1
     aea:	601a      	str	r2, [r3, #0]
     aec:	6819      	ldr	r1, [r3, #0]
     aee:	4a9b      	ldr	r2, [pc, #620]	; (d5c <main+0x448>)
     af0:	2704      	movs	r7, #4
     af2:	400a      	ands	r2, r1
     af4:	2180      	movs	r1, #128	; 0x80
     af6:	0349      	lsls	r1, r1, #13
     af8:	430a      	orrs	r2, r1
     afa:	601a      	str	r2, [r3, #0]
     afc:	681a      	ldr	r2, [r3, #0]
     afe:	2112      	movs	r1, #18
     b00:	f7ff fb4e 	bl	1a0 <mbus_remote_register_write>
     b04:	2005      	movs	r0, #5
     b06:	22c0      	movs	r2, #192	; 0xc0
     b08:	4082      	lsls	r2, r0
     b0a:	2118      	movs	r1, #24
     b0c:	f7ff fb48 	bl	1a0 <mbus_remote_register_write>
     b10:	4b93      	ldr	r3, [pc, #588]	; (d60 <main+0x44c>)
     b12:	4a94      	ldr	r2, [pc, #592]	; (d64 <main+0x450>)
     b14:	6819      	ldr	r1, [r3, #0]
     b16:	2004      	movs	r0, #4
     b18:	400a      	ands	r2, r1
     b1a:	21bc      	movs	r1, #188	; 0xbc
     b1c:	0409      	lsls	r1, r1, #16
     b1e:	430a      	orrs	r2, r1
     b20:	601a      	str	r2, [r3, #0]
     b22:	6819      	ldr	r1, [r3, #0]
     b24:	4a90      	ldr	r2, [pc, #576]	; (d68 <main+0x454>)
     b26:	400a      	ands	r2, r1
     b28:	601a      	str	r2, [r3, #0]
     b2a:	6819      	ldr	r1, [r3, #0]
     b2c:	4a8f      	ldr	r2, [pc, #572]	; (d6c <main+0x458>)
     b2e:	400a      	ands	r2, r1
     b30:	2180      	movs	r1, #128	; 0x80
     b32:	0109      	lsls	r1, r1, #4
     b34:	430a      	orrs	r2, r1
     b36:	601a      	str	r2, [r3, #0]
     b38:	6819      	ldr	r1, [r3, #0]
     b3a:	4a8d      	ldr	r2, [pc, #564]	; (d70 <main+0x45c>)
     b3c:	400a      	ands	r2, r1
     b3e:	21c0      	movs	r1, #192	; 0xc0
     b40:	0289      	lsls	r1, r1, #10
     b42:	430a      	orrs	r2, r1
     b44:	601a      	str	r2, [r3, #0]
     b46:	681a      	ldr	r2, [r3, #0]
     b48:	1c21      	adds	r1, r4, #0
     b4a:	f7ff fb29 	bl	1a0 <mbus_remote_register_write>
     b4e:	4b89      	ldr	r3, [pc, #548]	; (d74 <main+0x460>)
     b50:	211f      	movs	r1, #31
     b52:	681a      	ldr	r2, [r3, #0]
     b54:	2004      	movs	r0, #4
     b56:	438a      	bics	r2, r1
     b58:	2110      	movs	r1, #16
     b5a:	430a      	orrs	r2, r1
     b5c:	601a      	str	r2, [r3, #0]
     b5e:	6819      	ldr	r1, [r3, #0]
     b60:	4a85      	ldr	r2, [pc, #532]	; (d78 <main+0x464>)
     b62:	400a      	ands	r2, r1
     b64:	21b8      	movs	r1, #184	; 0xb8
     b66:	0089      	lsls	r1, r1, #2
     b68:	430a      	orrs	r2, r1
     b6a:	601a      	str	r2, [r3, #0]
     b6c:	6819      	ldr	r1, [r3, #0]
     b6e:	4a83      	ldr	r2, [pc, #524]	; (d7c <main+0x468>)
     b70:	400a      	ands	r2, r1
     b72:	21a0      	movs	r1, #160	; 0xa0
     b74:	0249      	lsls	r1, r1, #9
     b76:	430a      	orrs	r2, r1
     b78:	601a      	str	r2, [r3, #0]
     b7a:	681a      	ldr	r2, [r3, #0]
     b7c:	210b      	movs	r1, #11
     b7e:	f7ff fb0f 	bl	1a0 <mbus_remote_register_write>
     b82:	4b7f      	ldr	r3, [pc, #508]	; (d80 <main+0x46c>)
     b84:	22c0      	movs	r2, #192	; 0xc0
     b86:	6819      	ldr	r1, [r3, #0]
     b88:	0352      	lsls	r2, r2, #13
     b8a:	430a      	orrs	r2, r1
     b8c:	601a      	str	r2, [r3, #0]
     b8e:	6819      	ldr	r1, [r3, #0]
     b90:	4a7c      	ldr	r2, [pc, #496]	; (d84 <main+0x470>)
     b92:	2004      	movs	r0, #4
     b94:	400a      	ands	r2, r1
     b96:	2180      	movs	r1, #128	; 0x80
     b98:	0109      	lsls	r1, r1, #4
     b9a:	430a      	orrs	r2, r1
     b9c:	601a      	str	r2, [r3, #0]
     b9e:	681a      	ldr	r2, [r3, #0]
     ba0:	217f      	movs	r1, #127	; 0x7f
     ba2:	438a      	bics	r2, r1
     ba4:	2160      	movs	r1, #96	; 0x60
     ba6:	430a      	orrs	r2, r1
     ba8:	601a      	str	r2, [r3, #0]
     baa:	681a      	ldr	r2, [r3, #0]
     bac:	2101      	movs	r1, #1
     bae:	f7ff faf7 	bl	1a0 <mbus_remote_register_write>
     bb2:	4b75      	ldr	r3, [pc, #468]	; (d88 <main+0x474>)
     bb4:	213f      	movs	r1, #63	; 0x3f
     bb6:	781a      	ldrb	r2, [r3, #0]
     bb8:	1c38      	adds	r0, r7, #0
     bba:	438a      	bics	r2, r1
     bbc:	433a      	orrs	r2, r7
     bbe:	701a      	strb	r2, [r3, #0]
     bc0:	681a      	ldr	r2, [r3, #0]
     bc2:	210c      	movs	r1, #12
     bc4:	f7ff faec 	bl	1a0 <mbus_remote_register_write>
     bc8:	22c8      	movs	r2, #200	; 0xc8
     bca:	1c38      	adds	r0, r7, #0
     bcc:	1c29      	adds	r1, r5, #0
     bce:	0152      	lsls	r2, r2, #5
     bd0:	f7ff fae6 	bl	1a0 <mbus_remote_register_write>
     bd4:	4b6d      	ldr	r3, [pc, #436]	; (d8c <main+0x478>)
     bd6:	4a6e      	ldr	r2, [pc, #440]	; (d90 <main+0x47c>)
     bd8:	2003      	movs	r0, #3
     bda:	601a      	str	r2, [r3, #0]
     bdc:	4b6d      	ldr	r3, [pc, #436]	; (d94 <main+0x480>)
     bde:	1c21      	adds	r1, r4, #0
     be0:	6018      	str	r0, [r3, #0]
     be2:	4b6d      	ldr	r3, [pc, #436]	; (d98 <main+0x484>)
     be4:	601c      	str	r4, [r3, #0]
     be6:	4b6d      	ldr	r3, [pc, #436]	; (d9c <main+0x488>)
     be8:	601c      	str	r4, [r3, #0]
     bea:	4b6d      	ldr	r3, [pc, #436]	; (da0 <main+0x48c>)
     bec:	601c      	str	r4, [r3, #0]
     bee:	4b6d      	ldr	r3, [pc, #436]	; (da4 <main+0x490>)
     bf0:	601c      	str	r4, [r3, #0]
     bf2:	4b6d      	ldr	r3, [pc, #436]	; (da8 <main+0x494>)
     bf4:	601c      	str	r4, [r3, #0]
     bf6:	4b6d      	ldr	r3, [pc, #436]	; (dac <main+0x498>)
     bf8:	601c      	str	r4, [r3, #0]
     bfa:	4b6d      	ldr	r3, [pc, #436]	; (db0 <main+0x49c>)
     bfc:	601c      	str	r4, [r3, #0]
     bfe:	4b6d      	ldr	r3, [pc, #436]	; (db4 <main+0x4a0>)
     c00:	601c      	str	r4, [r3, #0]
     c02:	4b6d      	ldr	r3, [pc, #436]	; (db8 <main+0x4a4>)
     c04:	601c      	str	r4, [r3, #0]
     c06:	4b6d      	ldr	r3, [pc, #436]	; (dbc <main+0x4a8>)
     c08:	781a      	ldrb	r2, [r3, #0]
     c0a:	43aa      	bics	r2, r5
     c0c:	4332      	orrs	r2, r6
     c0e:	701a      	strb	r2, [r3, #0]
     c10:	681a      	ldr	r2, [r3, #0]
     c12:	f7ff fac5 	bl	1a0 <mbus_remote_register_write>
     c16:	2064      	movs	r0, #100	; 0x64
     c18:	f7ff fa3d 	bl	96 <delay>
     c1c:	1c30      	adds	r0, r6, #0
     c1e:	213a      	movs	r1, #58	; 0x3a
     c20:	4a67      	ldr	r2, [pc, #412]	; (dc0 <main+0x4ac>)
     c22:	f7ff fabd 	bl	1a0 <mbus_remote_register_write>
     c26:	2064      	movs	r0, #100	; 0x64
     c28:	f7ff fa35 	bl	96 <delay>
     c2c:	e20c      	b.n	1048 <main+0x734>
     c2e:	2378      	movs	r3, #120	; 0x78
     c30:	681b      	ldr	r3, [r3, #0]
     c32:	4f60      	ldr	r7, [pc, #384]	; (db4 <main+0x4a0>)
     c34:	603b      	str	r3, [r7, #0]
     c36:	683c      	ldr	r4, [r7, #0]
     c38:	683e      	ldr	r6, [r7, #0]
     c3a:	683a      	ldr	r2, [r7, #0]
     c3c:	683d      	ldr	r5, [r7, #0]
     c3e:	0e24      	lsrs	r4, r4, #24
     c40:	0c2d      	lsrs	r5, r5, #16
     c42:	b2eb      	uxtb	r3, r5
     c44:	9203      	str	r2, [sp, #12]
     c46:	9301      	str	r3, [sp, #4]
     c48:	2c00      	cmp	r4, #0
     c4a:	d100      	bne.n	c4e <main+0x33a>
     c4c:	e347      	b.n	12de <main+0x9ca>
     c4e:	4b32      	ldr	r3, [pc, #200]	; (d18 <main+0x404>)
     c50:	681b      	ldr	r3, [r3, #0]
     c52:	9302      	str	r3, [sp, #8]
     c54:	2b00      	cmp	r3, #0
     c56:	d114      	bne.n	c82 <main+0x36e>
     c58:	f7ff fa64 	bl	124 <set_halt_until_mbus_tx>
     c5c:	4959      	ldr	r1, [pc, #356]	; (dc4 <main+0x4b0>)
     c5e:	20aa      	movs	r0, #170	; 0xaa
     c60:	f7ff fa6e 	bl	140 <mbus_write_message32>
     c64:	6839      	ldr	r1, [r7, #0]
     c66:	20aa      	movs	r0, #170	; 0xaa
     c68:	f7ff fa6a 	bl	140 <mbus_write_message32>
     c6c:	9902      	ldr	r1, [sp, #8]
     c6e:	4b4e      	ldr	r3, [pc, #312]	; (da8 <main+0x494>)
     c70:	6019      	str	r1, [r3, #0]
     c72:	4b27      	ldr	r3, [pc, #156]	; (d10 <main+0x3fc>)
     c74:	6019      	str	r1, [r3, #0]
     c76:	f7ff faa1 	bl	1bc <radio_power_off>
     c7a:	f7ff fad5 	bl	228 <ldo_power_off>
     c7e:	f7ff fae7 	bl	250 <temp_power_off>
     c82:	9a03      	ldr	r2, [sp, #12]
     c84:	0a13      	lsrs	r3, r2, #8
     c86:	2c01      	cmp	r4, #1
     c88:	d000      	beq.n	c8c <main+0x378>
     c8a:	e0a1      	b.n	dd0 <main+0x4bc>
     c8c:	22ff      	movs	r2, #255	; 0xff
     c8e:	4f41      	ldr	r7, [pc, #260]	; (d94 <main+0x480>)
     c90:	4013      	ands	r3, r2
     c92:	603b      	str	r3, [r7, #0]
     c94:	4b20      	ldr	r3, [pc, #128]	; (d18 <main+0x404>)
     c96:	4015      	ands	r5, r2
     c98:	4016      	ands	r6, r2
     c9a:	022d      	lsls	r5, r5, #8
     c9c:	6819      	ldr	r1, [r3, #0]
     c9e:	1976      	adds	r6, r6, r5
     ca0:	42b1      	cmp	r1, r6
     ca2:	d21e      	bcs.n	ce2 <main+0x3ce>
     ca4:	681a      	ldr	r2, [r3, #0]
     ca6:	3201      	adds	r2, #1
     ca8:	601a      	str	r2, [r3, #0]
     caa:	681a      	ldr	r2, [r3, #0]
     cac:	2a01      	cmp	r2, #1
     cae:	d109      	bne.n	cc4 <main+0x3b0>
     cb0:	f7ff fb20 	bl	2f4 <radio_power_on>
     cb4:	2002      	movs	r0, #2
     cb6:	1c21      	adds	r1, r4, #0
     cb8:	1c22      	adds	r2, r4, #0
     cba:	f7ff fa01 	bl	c0 <set_wakeup_timer>
     cbe:	f7ff fa69 	bl	194 <mbus_sleep_all>
     cc2:	e7fe      	b.n	cc2 <main+0x3ae>
     cc4:	6819      	ldr	r1, [r3, #0]
     cc6:	4b40      	ldr	r3, [pc, #256]	; (dc8 <main+0x4b4>)
     cc8:	2000      	movs	r0, #0
     cca:	18c9      	adds	r1, r1, r3
     ccc:	f7ff fb6e 	bl	3ac <send_radio_data_ppm>
     cd0:	6838      	ldr	r0, [r7, #0]
     cd2:	1c21      	adds	r1, r4, #0
     cd4:	b280      	uxth	r0, r0
     cd6:	1c22      	adds	r2, r4, #0
     cd8:	f7ff f9f2 	bl	c0 <set_wakeup_timer>
     cdc:	f7ff fa5a 	bl	194 <mbus_sleep_all>
     ce0:	e7fe      	b.n	ce0 <main+0x3cc>
     ce2:	2200      	movs	r2, #0
     ce4:	601a      	str	r2, [r3, #0]
     ce6:	1c20      	adds	r0, r4, #0
     ce8:	4938      	ldr	r1, [pc, #224]	; (dcc <main+0x4b8>)
     cea:	f7ff fb5f 	bl	3ac <send_radio_data_ppm>
     cee:	f7ff fb47 	bl	380 <operation_sleep_notimer>
     cf2:	46c0      	nop			; (mov r8, r8)
     cf4:	000fffff 	.word	0x000fffff
     cf8:	0000137c 	.word	0x0000137c
     cfc:	deadbeef 	.word	0xdeadbeef
     d00:	00001344 	.word	0x00001344
     d04:	ffff9fff 	.word	0xffff9fff
     d08:	fffe7fff 	.word	0xfffe7fff
     d0c:	a000002c 	.word	0xa000002c
     d10:	0000139c 	.word	0x0000139c
     d14:	00001378 	.word	0x00001378
     d18:	000013b0 	.word	0x000013b0
     d1c:	000013a4 	.word	0x000013a4
     d20:	0000c204 	.word	0x0000c204
     d24:	0000c24a 	.word	0x0000c24a
     d28:	00000221 	.word	0x00000221
     d2c:	00001088 	.word	0x00001088
     d30:	0000c226 	.word	0x0000c226
     d34:	0000d10f 	.word	0x0000d10f
     d38:	0000082c 	.word	0x0000082c
     d3c:	00000aac 	.word	0x00000aac
     d40:	00000111 	.word	0x00000111
     d44:	0000135c 	.word	0x0000135c
     d48:	0000134c 	.word	0x0000134c
     d4c:	fff1ffff 	.word	0xfff1ffff
     d50:	0000133c 	.word	0x0000133c
     d54:	ff7fffff 	.word	0xff7fffff
     d58:	00001358 	.word	0x00001358
     d5c:	ffe07fff 	.word	0xffe07fff
     d60:	00001340 	.word	0x00001340
     d64:	ff03ffff 	.word	0xff03ffff
     d68:	fffffe1f 	.word	0xfffffe1f
     d6c:	ffffe1ff 	.word	0xffffe1ff
     d70:	fffc7fff 	.word	0xfffc7fff
     d74:	00001328 	.word	0x00001328
     d78:	ffffe01f 	.word	0xffffe01f
     d7c:	fff01fff 	.word	0xfff01fff
     d80:	00001334 	.word	0x00001334
     d84:	ffff03ff 	.word	0xffff03ff
     d88:	00001348 	.word	0x00001348
     d8c:	000013bc 	.word	0x000013bc
     d90:	000083d6 	.word	0x000083d6
     d94:	000013a0 	.word	0x000013a0
     d98:	00001380 	.word	0x00001380
     d9c:	00001374 	.word	0x00001374
     da0:	00001394 	.word	0x00001394
     da4:	0000138c 	.word	0x0000138c
     da8:	000013b4 	.word	0x000013b4
     dac:	00001370 	.word	0x00001370
     db0:	000013c0 	.word	0x000013c0
     db4:	00001384 	.word	0x00001384
     db8:	00001390 	.word	0x00001390
     dbc:	00001354 	.word	0x00001354
     dc0:	00080800 	.word	0x00080800
     dc4:	abcd1234 	.word	0xabcd1234
     dc8:	00abc000 	.word	0x00abc000
     dcc:	00faf000 	.word	0x00faf000
     dd0:	2c02      	cmp	r4, #2
     dd2:	d129      	bne.n	e28 <main+0x514>
     dd4:	22ff      	movs	r2, #255	; 0xff
     dd6:	4013      	ands	r3, r2
     dd8:	4016      	ands	r6, r2
     dda:	021b      	lsls	r3, r3, #8
     ddc:	18f6      	adds	r6, r6, r3
     dde:	49da      	ldr	r1, [pc, #872]	; (1148 <main+0x834>)
     de0:	230f      	movs	r3, #15
     de2:	4ada      	ldr	r2, [pc, #872]	; (114c <main+0x838>)
     de4:	402b      	ands	r3, r5
     de6:	600e      	str	r6, [r1, #0]
     de8:	6013      	str	r3, [r2, #0]
     dea:	2310      	movs	r3, #16
     dec:	401d      	ands	r5, r3
     dee:	4bd8      	ldr	r3, [pc, #864]	; (1150 <main+0x83c>)
     df0:	49d8      	ldr	r1, [pc, #864]	; (1154 <main+0x840>)
     df2:	601d      	str	r5, [r3, #0]
     df4:	4cd8      	ldr	r4, [pc, #864]	; (1158 <main+0x844>)
     df6:	2300      	movs	r3, #0
     df8:	600b      	str	r3, [r1, #0]
     dfa:	6821      	ldr	r1, [r4, #0]
     dfc:	4299      	cmp	r1, r3
     dfe:	d000      	beq.n	e02 <main+0x4ee>
     e00:	e1d8      	b.n	11b4 <main+0x8a0>
     e02:	6810      	ldr	r0, [r2, #0]
     e04:	2101      	movs	r1, #1
     e06:	1c0a      	adds	r2, r1, #0
     e08:	b280      	uxth	r0, r0
     e0a:	f7ff f959 	bl	c0 <set_wakeup_timer>
     e0e:	2301      	movs	r3, #1
     e10:	9901      	ldr	r1, [sp, #4]
     e12:	6023      	str	r3, [r4, #0]
     e14:	4bd1      	ldr	r3, [pc, #836]	; (115c <main+0x848>)
     e16:	094a      	lsrs	r2, r1, #5
     e18:	601a      	str	r2, [r3, #0]
     e1a:	4bd1      	ldr	r3, [pc, #836]	; (1160 <main+0x84c>)
     e1c:	681a      	ldr	r2, [r3, #0]
     e1e:	3201      	adds	r2, #1
     e20:	601a      	str	r2, [r3, #0]
     e22:	f7ff f9b7 	bl	194 <mbus_sleep_all>
     e26:	e7fe      	b.n	e26 <main+0x512>
     e28:	2c03      	cmp	r4, #3
     e2a:	d14a      	bne.n	ec2 <main+0x5ae>
     e2c:	27ff      	movs	r7, #255	; 0xff
     e2e:	4ac7      	ldr	r2, [pc, #796]	; (114c <main+0x838>)
     e30:	403b      	ands	r3, r7
     e32:	6013      	str	r3, [r2, #0]
     e34:	4bca      	ldr	r3, [pc, #808]	; (1160 <main+0x84c>)
     e36:	403e      	ands	r6, r7
     e38:	681a      	ldr	r2, [r3, #0]
     e3a:	42b2      	cmp	r2, r6
     e3c:	d300      	bcc.n	e40 <main+0x52c>
     e3e:	e24a      	b.n	12d6 <main+0x9c2>
     e40:	681a      	ldr	r2, [r3, #0]
     e42:	3201      	adds	r2, #1
     e44:	601a      	str	r2, [r3, #0]
     e46:	681d      	ldr	r5, [r3, #0]
     e48:	2d01      	cmp	r5, #1
     e4a:	d11f      	bne.n	e8c <main+0x578>
     e4c:	2100      	movs	r1, #0
     e4e:	1c22      	adds	r2, r4, #0
     e50:	2006      	movs	r0, #6
     e52:	f7ff f9a5 	bl	1a0 <mbus_remote_register_write>
     e56:	2064      	movs	r0, #100	; 0x64
     e58:	f7ff f91d 	bl	96 <delay>
     e5c:	2064      	movs	r0, #100	; 0x64
     e5e:	f7ff f91a 	bl	96 <delay>
     e62:	23a0      	movs	r3, #160	; 0xa0
     e64:	061b      	lsls	r3, r3, #24
     e66:	681b      	ldr	r3, [r3, #0]
     e68:	401f      	ands	r7, r3
     e6a:	4bbe      	ldr	r3, [pc, #760]	; (1164 <main+0x850>)
     e6c:	601f      	str	r7, [r3, #0]
     e6e:	f7ff fa0b 	bl	288 <batadc_reset>
     e72:	2064      	movs	r0, #100	; 0x64
     e74:	f7ff f90f 	bl	96 <delay>
     e78:	f7ff fa3c 	bl	2f4 <radio_power_on>
     e7c:	2002      	movs	r0, #2
     e7e:	1c29      	adds	r1, r5, #0
     e80:	1c2a      	adds	r2, r5, #0
     e82:	f7ff f91d 	bl	c0 <set_wakeup_timer>
     e86:	f7ff f985 	bl	194 <mbus_sleep_all>
     e8a:	e7fe      	b.n	e8a <main+0x576>
     e8c:	6818      	ldr	r0, [r3, #0]
     e8e:	2301      	movs	r3, #1
     e90:	4018      	ands	r0, r3
     e92:	d005      	beq.n	ea0 <main+0x58c>
     e94:	4bb3      	ldr	r3, [pc, #716]	; (1164 <main+0x850>)
     e96:	4ab4      	ldr	r2, [pc, #720]	; (1168 <main+0x854>)
     e98:	6819      	ldr	r1, [r3, #0]
     e9a:	2000      	movs	r0, #0
     e9c:	1889      	adds	r1, r1, r2
     e9e:	e004      	b.n	eaa <main+0x596>
     ea0:	4bb2      	ldr	r3, [pc, #712]	; (116c <main+0x858>)
     ea2:	6819      	ldr	r1, [r3, #0]
     ea4:	23c0      	movs	r3, #192	; 0xc0
     ea6:	041b      	lsls	r3, r3, #16
     ea8:	18c9      	adds	r1, r1, r3
     eaa:	f7ff fa7f 	bl	3ac <send_radio_data_ppm>
     eae:	4ba7      	ldr	r3, [pc, #668]	; (114c <main+0x838>)
     eb0:	2101      	movs	r1, #1
     eb2:	6818      	ldr	r0, [r3, #0]
     eb4:	1c0a      	adds	r2, r1, #0
     eb6:	b280      	uxth	r0, r0
     eb8:	f7ff f902 	bl	c0 <set_wakeup_timer>
     ebc:	f7ff f96a 	bl	194 <mbus_sleep_all>
     ec0:	e7fe      	b.n	ec0 <main+0x5ac>
     ec2:	2c04      	cmp	r4, #4
     ec4:	d177      	bne.n	fb6 <main+0x6a2>
     ec6:	22ff      	movs	r2, #255	; 0xff
     ec8:	49a0      	ldr	r1, [pc, #640]	; (114c <main+0x838>)
     eca:	4013      	ands	r3, r2
     ecc:	600b      	str	r3, [r1, #0]
     ece:	4ba8      	ldr	r3, [pc, #672]	; (1170 <main+0x85c>)
     ed0:	4016      	ands	r6, r2
     ed2:	4aa8      	ldr	r2, [pc, #672]	; (1174 <main+0x860>)
     ed4:	601e      	str	r6, [r3, #0]
     ed6:	6819      	ldr	r1, [r3, #0]
     ed8:	6812      	ldr	r2, [r2, #0]
     eda:	4291      	cmp	r1, r2
     edc:	d301      	bcc.n	ee2 <main+0x5ce>
     ede:	2200      	movs	r2, #0
     ee0:	601a      	str	r2, [r3, #0]
     ee2:	4c9f      	ldr	r4, [pc, #636]	; (1160 <main+0x84c>)
     ee4:	6823      	ldr	r3, [r4, #0]
     ee6:	2b02      	cmp	r3, #2
     ee8:	d840      	bhi.n	f6c <main+0x658>
     eea:	6823      	ldr	r3, [r4, #0]
     eec:	3301      	adds	r3, #1
     eee:	6023      	str	r3, [r4, #0]
     ef0:	6825      	ldr	r5, [r4, #0]
     ef2:	2d01      	cmp	r5, #1
     ef4:	d109      	bne.n	f0a <main+0x5f6>
     ef6:	f7ff f9fd 	bl	2f4 <radio_power_on>
     efa:	2002      	movs	r0, #2
     efc:	1c29      	adds	r1, r5, #0
     efe:	1c2a      	adds	r2, r5, #0
     f00:	f7ff f8de 	bl	c0 <set_wakeup_timer>
     f04:	f7ff f946 	bl	194 <mbus_sleep_all>
     f08:	e7fe      	b.n	f08 <main+0x5f4>
     f0a:	4a9b      	ldr	r2, [pc, #620]	; (1178 <main+0x864>)
     f0c:	6821      	ldr	r1, [r4, #0]
     f0e:	2000      	movs	r0, #0
     f10:	1889      	adds	r1, r1, r2
     f12:	f7ff fa4b 	bl	3ac <send_radio_data_ppm>
     f16:	4b8d      	ldr	r3, [pc, #564]	; (114c <main+0x838>)
     f18:	6822      	ldr	r2, [r4, #0]
     f1a:	6818      	ldr	r0, [r3, #0]
     f1c:	2101      	movs	r1, #1
     f1e:	b280      	uxth	r0, r0
     f20:	1c0a      	adds	r2, r1, #0
     f22:	f7ff f8cd 	bl	c0 <set_wakeup_timer>
     f26:	f7ff f935 	bl	194 <mbus_sleep_all>
     f2a:	e7fe      	b.n	f2a <main+0x616>
     f2c:	6821      	ldr	r1, [r4, #0]
     f2e:	20dd      	movs	r0, #221	; 0xdd
     f30:	f7ff f906 	bl	140 <mbus_write_message32>
     f34:	2064      	movs	r0, #100	; 0x64
     f36:	f7ff f8ae 	bl	96 <delay>
     f3a:	6823      	ldr	r3, [r4, #0]
     f3c:	4d8f      	ldr	r5, [pc, #572]	; (117c <main+0x868>)
     f3e:	009b      	lsls	r3, r3, #2
     f40:	5959      	ldr	r1, [r3, r5]
     f42:	20dd      	movs	r0, #221	; 0xdd
     f44:	f7ff f8fc 	bl	140 <mbus_write_message32>
     f48:	2064      	movs	r0, #100	; 0x64
     f4a:	f7ff f8a4 	bl	96 <delay>
     f4e:	488c      	ldr	r0, [pc, #560]	; (1180 <main+0x86c>)
     f50:	f7ff f8aa 	bl	a8 <config_timerwd>
     f54:	6823      	ldr	r3, [r4, #0]
     f56:	2000      	movs	r0, #0
     f58:	009b      	lsls	r3, r3, #2
     f5a:	5959      	ldr	r1, [r3, r5]
     f5c:	f7ff fa26 	bl	3ac <send_radio_data_ppm>
     f60:	4888      	ldr	r0, [pc, #544]	; (1184 <main+0x870>)
     f62:	f7ff f898 	bl	96 <delay>
     f66:	6823      	ldr	r3, [r4, #0]
     f68:	3b01      	subs	r3, #1
     f6a:	6023      	str	r3, [r4, #0]
     f6c:	4b80      	ldr	r3, [pc, #512]	; (1170 <main+0x85c>)
     f6e:	2500      	movs	r5, #0
     f70:	681a      	ldr	r2, [r3, #0]
     f72:	4985      	ldr	r1, [pc, #532]	; (1188 <main+0x874>)
     f74:	42aa      	cmp	r2, r5
     f76:	d102      	bne.n	f7e <main+0x66a>
     f78:	680d      	ldr	r5, [r1, #0]
     f7a:	1e6a      	subs	r2, r5, #1
     f7c:	4195      	sbcs	r5, r2
     f7e:	6818      	ldr	r0, [r3, #0]
     f80:	2200      	movs	r2, #0
     f82:	4290      	cmp	r0, r2
     f84:	d007      	beq.n	f96 <main+0x682>
     f86:	681a      	ldr	r2, [r3, #0]
     f88:	4b7a      	ldr	r3, [pc, #488]	; (1174 <main+0x860>)
     f8a:	6809      	ldr	r1, [r1, #0]
     f8c:	681b      	ldr	r3, [r3, #0]
     f8e:	188a      	adds	r2, r1, r2
     f90:	4293      	cmp	r3, r2
     f92:	4192      	sbcs	r2, r2
     f94:	4252      	negs	r2, r2
     f96:	4315      	orrs	r5, r2
     f98:	4c7b      	ldr	r4, [pc, #492]	; (1188 <main+0x874>)
     f9a:	d1c7      	bne.n	f2c <main+0x618>
     f9c:	487b      	ldr	r0, [pc, #492]	; (118c <main+0x878>)
     f9e:	f7ff f87a 	bl	96 <delay>
     fa2:	2001      	movs	r0, #1
     fa4:	497a      	ldr	r1, [pc, #488]	; (1190 <main+0x87c>)
     fa6:	f7ff fa01 	bl	3ac <send_radio_data_ppm>
     faa:	4b6d      	ldr	r3, [pc, #436]	; (1160 <main+0x84c>)
     fac:	601d      	str	r5, [r3, #0]
     fae:	4b71      	ldr	r3, [pc, #452]	; (1174 <main+0x860>)
     fb0:	681b      	ldr	r3, [r3, #0]
     fb2:	6023      	str	r3, [r4, #0]
     fb4:	e69b      	b.n	cee <main+0x3da>
     fb6:	2c07      	cmp	r4, #7
     fb8:	d149      	bne.n	104e <main+0x73a>
     fba:	24ff      	movs	r4, #255	; 0xff
     fbc:	4f63      	ldr	r7, [pc, #396]	; (114c <main+0x838>)
     fbe:	4023      	ands	r3, r4
     fc0:	603b      	str	r3, [r7, #0]
     fc2:	4b6a      	ldr	r3, [pc, #424]	; (116c <main+0x858>)
     fc4:	2500      	movs	r5, #0
     fc6:	601d      	str	r5, [r3, #0]
     fc8:	4b65      	ldr	r3, [pc, #404]	; (1160 <main+0x84c>)
     fca:	4026      	ands	r6, r4
     fcc:	681a      	ldr	r2, [r3, #0]
     fce:	42b2      	cmp	r2, r6
     fd0:	d235      	bcs.n	103e <main+0x72a>
     fd2:	681a      	ldr	r2, [r3, #0]
     fd4:	3201      	adds	r2, #1
     fd6:	601a      	str	r2, [r3, #0]
     fd8:	681e      	ldr	r6, [r3, #0]
     fda:	2e01      	cmp	r6, #1
     fdc:	d11f      	bne.n	101e <main+0x70a>
     fde:	1c29      	adds	r1, r5, #0
     fe0:	2203      	movs	r2, #3
     fe2:	2006      	movs	r0, #6
     fe4:	f7ff f8dc 	bl	1a0 <mbus_remote_register_write>
     fe8:	2064      	movs	r0, #100	; 0x64
     fea:	f7ff f854 	bl	96 <delay>
     fee:	2064      	movs	r0, #100	; 0x64
     ff0:	f7ff f851 	bl	96 <delay>
     ff4:	23a0      	movs	r3, #160	; 0xa0
     ff6:	061b      	lsls	r3, r3, #24
     ff8:	681b      	ldr	r3, [r3, #0]
     ffa:	401c      	ands	r4, r3
     ffc:	4b59      	ldr	r3, [pc, #356]	; (1164 <main+0x850>)
     ffe:	601c      	str	r4, [r3, #0]
    1000:	f7ff f942 	bl	288 <batadc_reset>
    1004:	2064      	movs	r0, #100	; 0x64
    1006:	f7ff f846 	bl	96 <delay>
    100a:	f7ff f973 	bl	2f4 <radio_power_on>
    100e:	2002      	movs	r0, #2
    1010:	1c31      	adds	r1, r6, #0
    1012:	1c32      	adds	r2, r6, #0
    1014:	f7ff f854 	bl	c0 <set_wakeup_timer>
    1018:	f7ff f8bc 	bl	194 <mbus_sleep_all>
    101c:	e7fe      	b.n	101c <main+0x708>
    101e:	4b51      	ldr	r3, [pc, #324]	; (1164 <main+0x850>)
    1020:	1c28      	adds	r0, r5, #0
    1022:	6819      	ldr	r1, [r3, #0]
    1024:	4b50      	ldr	r3, [pc, #320]	; (1168 <main+0x854>)
    1026:	18c9      	adds	r1, r1, r3
    1028:	f7ff f9c0 	bl	3ac <send_radio_data_ppm>
    102c:	6838      	ldr	r0, [r7, #0]
    102e:	2101      	movs	r1, #1
    1030:	b280      	uxth	r0, r0
    1032:	1c0a      	adds	r2, r1, #0
    1034:	f7ff f844 	bl	c0 <set_wakeup_timer>
    1038:	f7ff f8ac 	bl	194 <mbus_sleep_all>
    103c:	e7fe      	b.n	103c <main+0x728>
    103e:	2001      	movs	r0, #1
    1040:	4953      	ldr	r1, [pc, #332]	; (1190 <main+0x87c>)
    1042:	601d      	str	r5, [r3, #0]
    1044:	f7ff f9b2 	bl	3ac <send_radio_data_ppm>
    1048:	f7ff f92a 	bl	2a0 <batadc_resetrelease>
    104c:	e64f      	b.n	cee <main+0x3da>
    104e:	2c08      	cmp	r4, #8
    1050:	d132      	bne.n	10b8 <main+0x7a4>
    1052:	4a46      	ldr	r2, [pc, #280]	; (116c <main+0x858>)
    1054:	2400      	movs	r4, #0
    1056:	6014      	str	r4, [r2, #0]
    1058:	4a41      	ldr	r2, [pc, #260]	; (1160 <main+0x84c>)
    105a:	6811      	ldr	r1, [r2, #0]
    105c:	3101      	adds	r1, #1
    105e:	6011      	str	r1, [r2, #0]
    1060:	6817      	ldr	r7, [r2, #0]
    1062:	2f01      	cmp	r7, #1
    1064:	d005      	beq.n	1072 <main+0x75e>
    1066:	22ff      	movs	r2, #255	; 0xff
    1068:	4013      	ands	r3, r2
    106a:	4016      	ands	r6, r2
    106c:	021b      	lsls	r3, r3, #8
    106e:	18f6      	adds	r6, r6, r3
    1070:	e019      	b.n	10a6 <main+0x792>
    1072:	f7ff f93f 	bl	2f4 <radio_power_on>
    1076:	2002      	movs	r0, #2
    1078:	1c39      	adds	r1, r7, #0
    107a:	1c3a      	adds	r2, r7, #0
    107c:	f7ff f820 	bl	c0 <set_wakeup_timer>
    1080:	f7ff f888 	bl	194 <mbus_sleep_all>
    1084:	e7fe      	b.n	1084 <main+0x770>
    1086:	22bb      	movs	r2, #187	; 0xbb
    1088:	0412      	lsls	r2, r2, #16
    108a:	18a1      	adds	r1, r4, r2
    108c:	2000      	movs	r0, #0
    108e:	f7ff f98d 	bl	3ac <send_radio_data_ppm>
    1092:	483e      	ldr	r0, [pc, #248]	; (118c <main+0x878>)
    1094:	f7fe ffff 	bl	96 <delay>
    1098:	4839      	ldr	r0, [pc, #228]	; (1180 <main+0x86c>)
    109a:	f7ff f805 	bl	a8 <config_timerwd>
    109e:	483b      	ldr	r0, [pc, #236]	; (118c <main+0x878>)
    10a0:	3401      	adds	r4, #1
    10a2:	f7fe fff8 	bl	96 <delay>
    10a6:	42b4      	cmp	r4, r6
    10a8:	d1ed      	bne.n	1086 <main+0x772>
    10aa:	07eb      	lsls	r3, r5, #31
    10ac:	d501      	bpl.n	10b2 <main+0x79e>
    10ae:	f7ff f903 	bl	2b8 <reset_pmu_solar_short>
    10b2:	2200      	movs	r2, #0
    10b4:	4b2a      	ldr	r3, [pc, #168]	; (1160 <main+0x84c>)
    10b6:	e10f      	b.n	12d8 <main+0x9c4>
    10b8:	2c13      	cmp	r4, #19
    10ba:	d171      	bne.n	11a0 <main+0x88c>
    10bc:	27ff      	movs	r7, #255	; 0xff
    10be:	4c23      	ldr	r4, [pc, #140]	; (114c <main+0x838>)
    10c0:	403b      	ands	r3, r7
    10c2:	2064      	movs	r0, #100	; 0x64
    10c4:	6023      	str	r3, [r4, #0]
    10c6:	f7fe ffe6 	bl	96 <delay>
    10ca:	4b32      	ldr	r3, [pc, #200]	; (1194 <main+0x880>)
    10cc:	9a01      	ldr	r2, [sp, #4]
    10ce:	6818      	ldr	r0, [r3, #0]
    10d0:	0911      	lsrs	r1, r2, #4
    10d2:	4a31      	ldr	r2, [pc, #196]	; (1198 <main+0x884>)
    10d4:	0149      	lsls	r1, r1, #5
    10d6:	4002      	ands	r2, r0
    10d8:	430a      	orrs	r2, r1
    10da:	601a      	str	r2, [r3, #0]
    10dc:	220f      	movs	r2, #15
    10de:	4015      	ands	r5, r2
    10e0:	026a      	lsls	r2, r5, #9
    10e2:	492e      	ldr	r1, [pc, #184]	; (119c <main+0x888>)
    10e4:	681d      	ldr	r5, [r3, #0]
    10e6:	2004      	movs	r0, #4
    10e8:	400d      	ands	r5, r1
    10ea:	4315      	orrs	r5, r2
    10ec:	601d      	str	r5, [r3, #0]
    10ee:	681a      	ldr	r2, [r3, #0]
    10f0:	2100      	movs	r1, #0
    10f2:	f7ff f855 	bl	1a0 <mbus_remote_register_write>
    10f6:	2064      	movs	r0, #100	; 0x64
    10f8:	f7fe ffcd 	bl	96 <delay>
    10fc:	4b18      	ldr	r3, [pc, #96]	; (1160 <main+0x84c>)
    10fe:	403e      	ands	r6, r7
    1100:	681a      	ldr	r2, [r3, #0]
    1102:	42b2      	cmp	r2, r6
    1104:	d300      	bcc.n	1108 <main+0x7f4>
    1106:	e0e6      	b.n	12d6 <main+0x9c2>
    1108:	681a      	ldr	r2, [r3, #0]
    110a:	3201      	adds	r2, #1
    110c:	601a      	str	r2, [r3, #0]
    110e:	681d      	ldr	r5, [r3, #0]
    1110:	2d01      	cmp	r5, #1
    1112:	d109      	bne.n	1128 <main+0x814>
    1114:	f7ff f8ee 	bl	2f4 <radio_power_on>
    1118:	2002      	movs	r0, #2
    111a:	1c29      	adds	r1, r5, #0
    111c:	1c2a      	adds	r2, r5, #0
    111e:	f7fe ffcf 	bl	c0 <set_wakeup_timer>
    1122:	f7ff f837 	bl	194 <mbus_sleep_all>
    1126:	e7fe      	b.n	1126 <main+0x812>
    1128:	6819      	ldr	r1, [r3, #0]
    112a:	4b0f      	ldr	r3, [pc, #60]	; (1168 <main+0x854>)
    112c:	2000      	movs	r0, #0
    112e:	18c9      	adds	r1, r1, r3
    1130:	f7ff f93c 	bl	3ac <send_radio_data_ppm>
    1134:	6820      	ldr	r0, [r4, #0]
    1136:	2101      	movs	r1, #1
    1138:	b280      	uxth	r0, r0
    113a:	1c0a      	adds	r2, r1, #0
    113c:	f7fe ffc0 	bl	c0 <set_wakeup_timer>
    1140:	f7ff f828 	bl	194 <mbus_sleep_all>
    1144:	e7fe      	b.n	1144 <main+0x830>
    1146:	46c0      	nop			; (mov r8, r8)
    1148:	000013bc 	.word	0x000013bc
    114c:	000013a0 	.word	0x000013a0
    1150:	00001394 	.word	0x00001394
    1154:	0000138c 	.word	0x0000138c
    1158:	000013b4 	.word	0x000013b4
    115c:	00001390 	.word	0x00001390
    1160:	000013b0 	.word	0x000013b0
    1164:	00001398 	.word	0x00001398
    1168:	00bbb000 	.word	0x00bbb000
    116c:	00001378 	.word	0x00001378
    1170:	000013a8 	.word	0x000013a8
    1174:	00001380 	.word	0x00001380
    1178:	00abc000 	.word	0x00abc000
    117c:	000013c4 	.word	0x000013c4
    1180:	000fffff 	.word	0x000fffff
    1184:	00001388 	.word	0x00001388
    1188:	00001374 	.word	0x00001374
    118c:	00002710 	.word	0x00002710
    1190:	00faf000 	.word	0x00faf000
    1194:	00001340 	.word	0x00001340
    1198:	fffffe1f 	.word	0xfffffe1f
    119c:	ffffe1ff 	.word	0xffffe1ff
    11a0:	2c14      	cmp	r4, #20
    11a2:	d116      	bne.n	11d2 <main+0x8be>
    11a4:	4a4f      	ldr	r2, [pc, #316]	; (12e4 <main+0x9d0>)
    11a6:	2301      	movs	r3, #1
    11a8:	6013      	str	r3, [r2, #0]
    11aa:	4a4f      	ldr	r2, [pc, #316]	; (12e8 <main+0x9d4>)
    11ac:	6013      	str	r3, [r2, #0]
    11ae:	4a4f      	ldr	r2, [pc, #316]	; (12ec <main+0x9d8>)
    11b0:	6013      	str	r3, [r2, #0]
    11b2:	2300      	movs	r3, #0
    11b4:	4a4e      	ldr	r2, [pc, #312]	; (12f0 <main+0x9dc>)
    11b6:	6013      	str	r3, [r2, #0]
    11b8:	4a4e      	ldr	r2, [pc, #312]	; (12f4 <main+0x9e0>)
    11ba:	6013      	str	r3, [r2, #0]
    11bc:	4a4e      	ldr	r2, [pc, #312]	; (12f8 <main+0x9e4>)
    11be:	6013      	str	r3, [r2, #0]
    11c0:	4a4e      	ldr	r2, [pc, #312]	; (12fc <main+0x9e8>)
    11c2:	6013      	str	r3, [r2, #0]
    11c4:	2278      	movs	r2, #120	; 0x78
    11c6:	6013      	str	r3, [r2, #0]
    11c8:	4a4d      	ldr	r2, [pc, #308]	; (1300 <main+0x9ec>)
    11ca:	6013      	str	r3, [r2, #0]
    11cc:	4a4d      	ldr	r2, [pc, #308]	; (1304 <main+0x9f0>)
    11ce:	6013      	str	r3, [r2, #0]
    11d0:	e085      	b.n	12de <main+0x9ca>
    11d2:	2c15      	cmp	r4, #21
    11d4:	d12b      	bne.n	122e <main+0x91a>
    11d6:	4b4c      	ldr	r3, [pc, #304]	; (1308 <main+0x9f4>)
    11d8:	681a      	ldr	r2, [r3, #0]
    11da:	0212      	lsls	r2, r2, #8
    11dc:	d004      	beq.n	11e8 <main+0x8d4>
    11de:	681a      	ldr	r2, [r3, #0]
    11e0:	4b4a      	ldr	r3, [pc, #296]	; (130c <main+0x9f8>)
    11e2:	0212      	lsls	r2, r2, #8
    11e4:	0a12      	lsrs	r2, r2, #8
    11e6:	601a      	str	r2, [r3, #0]
    11e8:	4b45      	ldr	r3, [pc, #276]	; (1300 <main+0x9ec>)
    11ea:	681a      	ldr	r2, [r3, #0]
    11ec:	2a04      	cmp	r2, #4
    11ee:	d872      	bhi.n	12d6 <main+0x9c2>
    11f0:	681a      	ldr	r2, [r3, #0]
    11f2:	3201      	adds	r2, #1
    11f4:	601a      	str	r2, [r3, #0]
    11f6:	681c      	ldr	r4, [r3, #0]
    11f8:	2c01      	cmp	r4, #1
    11fa:	d109      	bne.n	1210 <main+0x8fc>
    11fc:	f7ff f87a 	bl	2f4 <radio_power_on>
    1200:	2002      	movs	r0, #2
    1202:	1c21      	adds	r1, r4, #0
    1204:	1c22      	adds	r2, r4, #0
    1206:	f7fe ff5b 	bl	c0 <set_wakeup_timer>
    120a:	f7fe ffc3 	bl	194 <mbus_sleep_all>
    120e:	e7fe      	b.n	120e <main+0x8fa>
    1210:	4b3e      	ldr	r3, [pc, #248]	; (130c <main+0x9f8>)
    1212:	2000      	movs	r0, #0
    1214:	6819      	ldr	r1, [r3, #0]
    1216:	f7ff f8c9 	bl	3ac <send_radio_data_ppm>
    121a:	4b3d      	ldr	r3, [pc, #244]	; (1310 <main+0x9fc>)
    121c:	2101      	movs	r1, #1
    121e:	6818      	ldr	r0, [r3, #0]
    1220:	1c0a      	adds	r2, r1, #0
    1222:	b280      	uxth	r0, r0
    1224:	f7fe ff4c 	bl	c0 <set_wakeup_timer>
    1228:	f7fe ffb4 	bl	194 <mbus_sleep_all>
    122c:	e7fe      	b.n	122c <main+0x918>
    122e:	2c20      	cmp	r4, #32
    1230:	d10c      	bne.n	124c <main+0x938>
    1232:	4b35      	ldr	r3, [pc, #212]	; (1308 <main+0x9f4>)
    1234:	681a      	ldr	r2, [r3, #0]
    1236:	0212      	lsls	r2, r2, #8
    1238:	d004      	beq.n	1244 <main+0x930>
    123a:	681a      	ldr	r2, [r3, #0]
    123c:	4b35      	ldr	r3, [pc, #212]	; (1314 <main+0xa00>)
    123e:	0212      	lsls	r2, r2, #8
    1240:	0a12      	lsrs	r2, r2, #8
    1242:	601a      	str	r2, [r3, #0]
    1244:	4b2e      	ldr	r3, [pc, #184]	; (1300 <main+0x9ec>)
    1246:	2200      	movs	r2, #0
    1248:	601a      	str	r2, [r3, #0]
    124a:	e550      	b.n	cee <main+0x3da>
    124c:	2c16      	cmp	r4, #22
    124e:	d146      	bne.n	12de <main+0x9ca>
    1250:	9901      	ldr	r1, [sp, #4]
    1252:	29ff      	cmp	r1, #255	; 0xff
    1254:	d007      	beq.n	1266 <main+0x952>
    1256:	2303      	movs	r3, #3
    1258:	401d      	ands	r5, r3
    125a:	4b2b      	ldr	r3, [pc, #172]	; (1308 <main+0x9f4>)
    125c:	00ad      	lsls	r5, r5, #2
    125e:	681a      	ldr	r2, [r3, #0]
    1260:	4b2d      	ldr	r3, [pc, #180]	; (1318 <main+0xa04>)
    1262:	b292      	uxth	r2, r2
    1264:	50ea      	str	r2, [r5, r3]
    1266:	4b26      	ldr	r3, [pc, #152]	; (1300 <main+0x9ec>)
    1268:	681a      	ldr	r2, [r3, #0]
    126a:	2a04      	cmp	r2, #4
    126c:	d833      	bhi.n	12d6 <main+0x9c2>
    126e:	681a      	ldr	r2, [r3, #0]
    1270:	3201      	adds	r2, #1
    1272:	601a      	str	r2, [r3, #0]
    1274:	681c      	ldr	r4, [r3, #0]
    1276:	2c01      	cmp	r4, #1
    1278:	d109      	bne.n	128e <main+0x97a>
    127a:	f7ff f83b 	bl	2f4 <radio_power_on>
    127e:	2002      	movs	r0, #2
    1280:	1c21      	adds	r1, r4, #0
    1282:	1c22      	adds	r2, r4, #0
    1284:	f7fe ff1c 	bl	c0 <set_wakeup_timer>
    1288:	f7fe ff84 	bl	194 <mbus_sleep_all>
    128c:	e7fe      	b.n	128c <main+0x978>
    128e:	4c22      	ldr	r4, [pc, #136]	; (1318 <main+0xa04>)
    1290:	2000      	movs	r0, #0
    1292:	6821      	ldr	r1, [r4, #0]
    1294:	f7ff f88a 	bl	3ac <send_radio_data_ppm>
    1298:	4820      	ldr	r0, [pc, #128]	; (131c <main+0xa08>)
    129a:	f7fe fefc 	bl	96 <delay>
    129e:	6861      	ldr	r1, [r4, #4]
    12a0:	2000      	movs	r0, #0
    12a2:	f7ff f883 	bl	3ac <send_radio_data_ppm>
    12a6:	481d      	ldr	r0, [pc, #116]	; (131c <main+0xa08>)
    12a8:	f7fe fef5 	bl	96 <delay>
    12ac:	68a1      	ldr	r1, [r4, #8]
    12ae:	2000      	movs	r0, #0
    12b0:	f7ff f87c 	bl	3ac <send_radio_data_ppm>
    12b4:	4819      	ldr	r0, [pc, #100]	; (131c <main+0xa08>)
    12b6:	f7fe feee 	bl	96 <delay>
    12ba:	68e1      	ldr	r1, [r4, #12]
    12bc:	2000      	movs	r0, #0
    12be:	f7ff f875 	bl	3ac <send_radio_data_ppm>
    12c2:	4b13      	ldr	r3, [pc, #76]	; (1310 <main+0x9fc>)
    12c4:	2101      	movs	r1, #1
    12c6:	6818      	ldr	r0, [r3, #0]
    12c8:	1c0a      	adds	r2, r1, #0
    12ca:	b280      	uxth	r0, r0
    12cc:	f7fe fef8 	bl	c0 <set_wakeup_timer>
    12d0:	f7fe ff60 	bl	194 <mbus_sleep_all>
    12d4:	e7fe      	b.n	12d4 <main+0x9c0>
    12d6:	2200      	movs	r2, #0
    12d8:	601a      	str	r2, [r3, #0]
    12da:	2001      	movs	r0, #1
    12dc:	e504      	b.n	ce8 <main+0x3d4>
    12de:	f7ff f8c7 	bl	470 <operation_temp_run>
    12e2:	e7fc      	b.n	12de <main+0x9ca>
    12e4:	00001394 	.word	0x00001394
    12e8:	0000138c 	.word	0x0000138c
    12ec:	000013b4 	.word	0x000013b4
    12f0:	00001378 	.word	0x00001378
    12f4:	000013ac 	.word	0x000013ac
    12f8:	00001380 	.word	0x00001380
    12fc:	00001374 	.word	0x00001374
    1300:	000013b0 	.word	0x000013b0
    1304:	00001388 	.word	0x00001388
    1308:	00001384 	.word	0x00001384
    130c:	00001350 	.word	0x00001350
    1310:	000013a0 	.word	0x000013a0
    1314:	00001320 	.word	0x00001320
    1318:	00001360 	.word	0x00001360
    131c:	00001388 	.word	0x00001388
