
Pstack_ondemand_v1.8/Pstack_ondemand_v1.8.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0x70>:
   0:	00000c00 	.word	0x00000c00
   4:	00000075 	.word	0x00000075
	...
  1c:	00000070 	.word	0x00000070
  20:	00000070 	.word	0x00000070
  24:	00000070 	.word	0x00000070
  28:	00000070 	.word	0x00000070
	...
  34:	00000070 	.word	0x00000070
	...
  40:	00000545 	.word	0x00000545
  44:	0000055d 	.word	0x0000055d
  48:	00000575 	.word	0x00000575
  4c:	0000058d 	.word	0x0000058d
	...

00000070 <hang>:
  70:	e7fe      	b.n	70 <hang>
	...

00000074 <_start>:
  74:	f000 fa96 	bl	5a4 <main>
  78:	e7fc      	b.n	74 <_start>

Disassembly of section .text.delay:

0000007a <delay>:
  7a:	b500      	push	{lr}
  7c:	2300      	movs	r3, #0
  7e:	e001      	b.n	84 <delay+0xa>
  80:	46c0      	nop			; (mov r8, r8)
  82:	3301      	adds	r3, #1
  84:	4283      	cmp	r3, r0
  86:	d1fb      	bne.n	80 <delay+0x6>
  88:	bd00      	pop	{pc}

Disassembly of section .text.config_timer:

0000008c <config_timer>:
  8c:	b570      	push	{r4, r5, r6, lr}
  8e:	24a5      	movs	r4, #165	; 0xa5
  90:	0100      	lsls	r0, r0, #4
  92:	0624      	lsls	r4, r4, #24
  94:	4304      	orrs	r4, r0
  96:	2500      	movs	r5, #0
  98:	6025      	str	r5, [r4, #0]
  9a:	4d06      	ldr	r5, [pc, #24]	; (b4 <config_timer+0x28>)
  9c:	9e04      	ldr	r6, [sp, #16]
  9e:	4305      	orrs	r5, r0
  a0:	602e      	str	r6, [r5, #0]
  a2:	4d05      	ldr	r5, [pc, #20]	; (b8 <config_timer+0x2c>)
  a4:	4305      	orrs	r5, r0
  a6:	602a      	str	r2, [r5, #0]
  a8:	4a04      	ldr	r2, [pc, #16]	; (bc <config_timer+0x30>)
  aa:	4310      	orrs	r0, r2
  ac:	6003      	str	r3, [r0, #0]
  ae:	6021      	str	r1, [r4, #0]
  b0:	bd70      	pop	{r4, r5, r6, pc}
  b2:	46c0      	nop			; (mov r8, r8)
  b4:	a5000004 	.word	0xa5000004
  b8:	a5000008 	.word	0xa5000008
  bc:	a500000c 	.word	0xa500000c

Disassembly of section .text.set_wakeup_timer:

000000c0 <set_wakeup_timer>:
  c0:	b500      	push	{lr}
  c2:	2900      	cmp	r1, #0
  c4:	d003      	beq.n	ce <set_wakeup_timer+0xe>
  c6:	2380      	movs	r3, #128	; 0x80
  c8:	021b      	lsls	r3, r3, #8
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
  e4:	a2000010 	.word	0xa2000010
  e8:	a2000014 	.word	0xa2000014

Disassembly of section .text.enumerate:

000000ec <enumerate>:
  ec:	2280      	movs	r2, #128	; 0x80
  ee:	0600      	lsls	r0, r0, #24
  f0:	0592      	lsls	r2, r2, #22
  f2:	23a0      	movs	r3, #160	; 0xa0
  f4:	4302      	orrs	r2, r0
  f6:	061b      	lsls	r3, r3, #24
  f8:	2000      	movs	r0, #0
  fa:	6018      	str	r0, [r3, #0]
  fc:	4b01      	ldr	r3, [pc, #4]	; (104 <enumerate+0x18>)
  fe:	601a      	str	r2, [r3, #0]
 100:	4770      	bx	lr
 102:	46c0      	nop			; (mov r8, r8)
 104:	a0000004 	.word	0xa0000004

Disassembly of section .text.sleep:

00000108 <sleep>:
 108:	23a0      	movs	r3, #160	; 0xa0
 10a:	2201      	movs	r2, #1
 10c:	061b      	lsls	r3, r3, #24
 10e:	601a      	str	r2, [r3, #0]
 110:	4b01      	ldr	r3, [pc, #4]	; (118 <sleep+0x10>)
 112:	2000      	movs	r0, #0
 114:	6018      	str	r0, [r3, #0]
 116:	4770      	bx	lr
 118:	a0000004 	.word	0xa0000004

Disassembly of section .text.write_mbus_register:

0000011c <write_mbus_register>:
 11c:	23a0      	movs	r3, #160	; 0xa0
 11e:	0100      	lsls	r0, r0, #4
 120:	061b      	lsls	r3, r3, #24
 122:	0212      	lsls	r2, r2, #8
 124:	6018      	str	r0, [r3, #0]
 126:	0a12      	lsrs	r2, r2, #8
 128:	4b02      	ldr	r3, [pc, #8]	; (134 <write_mbus_register+0x18>)
 12a:	0609      	lsls	r1, r1, #24
 12c:	4311      	orrs	r1, r2
 12e:	2000      	movs	r0, #0
 130:	6019      	str	r1, [r3, #0]
 132:	4770      	bx	lr
 134:	a0000004 	.word	0xa0000004

Disassembly of section .text.read_mbus_register:

00000138 <read_mbus_register>:
 138:	2301      	movs	r3, #1
 13a:	0100      	lsls	r0, r0, #4
 13c:	4318      	orrs	r0, r3
 13e:	23a0      	movs	r3, #160	; 0xa0
 140:	061b      	lsls	r3, r3, #24
 142:	6018      	str	r0, [r3, #0]
 144:	0212      	lsls	r2, r2, #8
 146:	4b03      	ldr	r3, [pc, #12]	; (154 <read_mbus_register+0x1c>)
 148:	0609      	lsls	r1, r1, #24
 14a:	4311      	orrs	r1, r2
 14c:	2000      	movs	r0, #0
 14e:	6019      	str	r1, [r3, #0]
 150:	4770      	bx	lr
 152:	46c0      	nop			; (mov r8, r8)
 154:	a0000004 	.word	0xa0000004

Disassembly of section .text.unlikely.send_radio_data:

00000158 <send_radio_data>:
 158:	b538      	push	{r3, r4, r5, lr}
 15a:	1c05      	adds	r5, r0, #0
 15c:	2417      	movs	r4, #23
 15e:	1c2a      	adds	r2, r5, #0
 160:	2301      	movs	r3, #1
 162:	40e2      	lsrs	r2, r4
 164:	401a      	ands	r2, r3
 166:	2004      	movs	r0, #4
 168:	2127      	movs	r1, #39	; 0x27
 16a:	2a00      	cmp	r2, #0
 16c:	d000      	beq.n	170 <send_radio_data+0x18>
 16e:	1c1a      	adds	r2, r3, #0
 170:	f7ff ffd4 	bl	11c <write_mbus_register>
 174:	2022      	movs	r0, #34	; 0x22
 176:	f7ff ff80 	bl	7a <delay>
 17a:	2004      	movs	r0, #4
 17c:	2127      	movs	r1, #39	; 0x27
 17e:	2200      	movs	r2, #0
 180:	f7ff ffcc 	bl	11c <write_mbus_register>
 184:	2022      	movs	r0, #34	; 0x22
 186:	f7ff ff78 	bl	7a <delay>
 18a:	3c01      	subs	r4, #1
 18c:	d2e7      	bcs.n	15e <send_radio_data+0x6>
 18e:	bd38      	pop	{r3, r4, r5, pc}

Disassembly of section .text.ldo_power_off:

00000190 <ldo_power_off>:
 190:	b508      	push	{r3, lr}
 192:	4b0e      	ldr	r3, [pc, #56]	; (1cc <ldo_power_off+0x3c>)
 194:	2102      	movs	r1, #2
 196:	681a      	ldr	r2, [r3, #0]
 198:	2005      	movs	r0, #5
 19a:	430a      	orrs	r2, r1
 19c:	601a      	str	r2, [r3, #0]
 19e:	6819      	ldr	r1, [r3, #0]
 1a0:	2280      	movs	r2, #128	; 0x80
 1a2:	0152      	lsls	r2, r2, #5
 1a4:	430a      	orrs	r2, r1
 1a6:	601a      	str	r2, [r3, #0]
 1a8:	681a      	ldr	r2, [r3, #0]
 1aa:	2101      	movs	r1, #1
 1ac:	430a      	orrs	r2, r1
 1ae:	601a      	str	r2, [r3, #0]
 1b0:	6819      	ldr	r1, [r3, #0]
 1b2:	2280      	movs	r2, #128	; 0x80
 1b4:	0112      	lsls	r2, r2, #4
 1b6:	430a      	orrs	r2, r1
 1b8:	601a      	str	r2, [r3, #0]
 1ba:	681a      	ldr	r2, [r3, #0]
 1bc:	2112      	movs	r1, #18
 1be:	f7ff ffad 	bl	11c <write_mbus_register>
 1c2:	2064      	movs	r0, #100	; 0x64
 1c4:	f7ff ff59 	bl	7a <delay>
 1c8:	bd08      	pop	{r3, pc}
 1ca:	46c0      	nop			; (mov r8, r8)
 1cc:	000009a0 	.word	0x000009a0

Disassembly of section .text.release_cdc_meas:

000001d0 <release_cdc_meas>:
 1d0:	b508      	push	{r3, lr}
 1d2:	4b07      	ldr	r3, [pc, #28]	; (1f0 <release_cdc_meas+0x20>)
 1d4:	2180      	movs	r1, #128	; 0x80
 1d6:	881a      	ldrh	r2, [r3, #0]
 1d8:	2005      	movs	r0, #5
 1da:	438a      	bics	r2, r1
 1dc:	801a      	strh	r2, [r3, #0]
 1de:	681a      	ldr	r2, [r3, #0]
 1e0:	2100      	movs	r1, #0
 1e2:	f7ff ff9b 	bl	11c <write_mbus_register>
 1e6:	2064      	movs	r0, #100	; 0x64
 1e8:	f7ff ff47 	bl	7a <delay>
 1ec:	bd08      	pop	{r3, pc}
 1ee:	46c0      	nop			; (mov r8, r8)
 1f0:	000009a4 	.word	0x000009a4

Disassembly of section .text.assert_cdc_reset:

000001f4 <assert_cdc_reset>:
 1f4:	b508      	push	{r3, lr}
 1f6:	4b07      	ldr	r3, [pc, #28]	; (214 <assert_cdc_reset+0x20>)
 1f8:	4a07      	ldr	r2, [pc, #28]	; (218 <assert_cdc_reset+0x24>)
 1fa:	8819      	ldrh	r1, [r3, #0]
 1fc:	2005      	movs	r0, #5
 1fe:	400a      	ands	r2, r1
 200:	801a      	strh	r2, [r3, #0]
 202:	681a      	ldr	r2, [r3, #0]
 204:	2100      	movs	r1, #0
 206:	f7ff ff89 	bl	11c <write_mbus_register>
 20a:	2064      	movs	r0, #100	; 0x64
 20c:	f7ff ff35 	bl	7a <delay>
 210:	bd08      	pop	{r3, pc}
 212:	46c0      	nop			; (mov r8, r8)
 214:	000009a4 	.word	0x000009a4
 218:	fffffeff 	.word	0xfffffeff

Disassembly of section .text.cdc_power_off:

0000021c <cdc_power_off>:
 21c:	b508      	push	{r3, lr}
 21e:	4b10      	ldr	r3, [pc, #64]	; (260 <cdc_power_off+0x44>)
 220:	2280      	movs	r2, #128	; 0x80
 222:	8819      	ldrh	r1, [r3, #0]
 224:	0092      	lsls	r2, r2, #2
 226:	430a      	orrs	r2, r1
 228:	801a      	strh	r2, [r3, #0]
 22a:	8819      	ldrh	r1, [r3, #0]
 22c:	2280      	movs	r2, #128	; 0x80
 22e:	0112      	lsls	r2, r2, #4
 230:	430a      	orrs	r2, r1
 232:	801a      	strh	r2, [r3, #0]
 234:	8819      	ldrh	r1, [r3, #0]
 236:	2280      	movs	r2, #128	; 0x80
 238:	00d2      	lsls	r2, r2, #3
 23a:	430a      	orrs	r2, r1
 23c:	801a      	strh	r2, [r3, #0]
 23e:	8819      	ldrh	r1, [r3, #0]
 240:	2280      	movs	r2, #128	; 0x80
 242:	0152      	lsls	r2, r2, #5
 244:	430a      	orrs	r2, r1
 246:	801a      	strh	r2, [r3, #0]
 248:	681a      	ldr	r2, [r3, #0]
 24a:	2100      	movs	r1, #0
 24c:	2005      	movs	r0, #5
 24e:	f7ff ff65 	bl	11c <write_mbus_register>
 252:	2064      	movs	r0, #100	; 0x64
 254:	f7ff ff11 	bl	7a <delay>
 258:	f7ff ff9a 	bl	190 <ldo_power_off>
 25c:	bd08      	pop	{r3, pc}
 25e:	46c0      	nop			; (mov r8, r8)
 260:	000009a4 	.word	0x000009a4

Disassembly of section .text.unlikely.operation_sleep_noirqreset:

00000264 <operation_sleep_noirqreset>:
 264:	b508      	push	{r3, lr}
 266:	4b03      	ldr	r3, [pc, #12]	; (274 <operation_sleep_noirqreset+0x10>)
 268:	2201      	movs	r2, #1
 26a:	601a      	str	r2, [r3, #0]
 26c:	f7ff ff4c 	bl	108 <sleep>
 270:	e7fe      	b.n	270 <operation_sleep_noirqreset+0xc>
 272:	46c0      	nop			; (mov r8, r8)
 274:	a2000014 	.word	0xa2000014

Disassembly of section .text.operation_sleep:

00000278 <operation_sleep>:
 278:	b508      	push	{r3, lr}
 27a:	4b04      	ldr	r3, [pc, #16]	; (28c <operation_sleep+0x14>)
 27c:	2201      	movs	r2, #1
 27e:	601a      	str	r2, [r3, #0]
 280:	2200      	movs	r2, #0
 282:	2368      	movs	r3, #104	; 0x68
 284:	601a      	str	r2, [r3, #0]
 286:	f7ff ff3f 	bl	108 <sleep>
 28a:	e7fe      	b.n	28a <operation_sleep+0x12>
 28c:	a2000014 	.word	0xa2000014

Disassembly of section .text.operation_sleep_notimer:

00000290 <operation_sleep_notimer>:
 290:	b508      	push	{r3, lr}
 292:	f7ff ff7d 	bl	190 <ldo_power_off>
 296:	2000      	movs	r0, #0
 298:	1c01      	adds	r1, r0, #0
 29a:	1c02      	adds	r2, r0, #0
 29c:	f7ff ff10 	bl	c0 <set_wakeup_timer>
 2a0:	f7ff ffea 	bl	278 <operation_sleep>

Disassembly of section .text.operation_cdc_run:

000002a4 <operation_cdc_run>:
 2a4:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 2a6:	4c91      	ldr	r4, [pc, #580]	; (4ec <operation_cdc_run+0x248>)
 2a8:	7825      	ldrb	r5, [r4, #0]
 2aa:	2d00      	cmp	r5, #0
 2ac:	d110      	bne.n	2d0 <operation_cdc_run+0x2c>
 2ae:	2304      	movs	r3, #4
 2b0:	7023      	strb	r3, [r4, #0]
 2b2:	4b8f      	ldr	r3, [pc, #572]	; (4f0 <operation_cdc_run+0x24c>)
 2b4:	2401      	movs	r4, #1
 2b6:	601d      	str	r5, [r3, #0]
 2b8:	4b8e      	ldr	r3, [pc, #568]	; (4f4 <operation_cdc_run+0x250>)
 2ba:	2112      	movs	r1, #18
 2bc:	681a      	ldr	r2, [r3, #0]
 2be:	2005      	movs	r0, #5
 2c0:	43a2      	bics	r2, r4
 2c2:	601a      	str	r2, [r3, #0]
 2c4:	681a      	ldr	r2, [r3, #0]
 2c6:	f7ff ff29 	bl	11c <write_mbus_register>
 2ca:	1c20      	adds	r0, r4, #0
 2cc:	1c21      	adds	r1, r4, #0
 2ce:	e092      	b.n	3f6 <operation_cdc_run+0x152>
 2d0:	7823      	ldrb	r3, [r4, #0]
 2d2:	2b04      	cmp	r3, #4
 2d4:	d10d      	bne.n	2f2 <operation_cdc_run+0x4e>
 2d6:	4b87      	ldr	r3, [pc, #540]	; (4f4 <operation_cdc_run+0x250>)
 2d8:	2005      	movs	r0, #5
 2da:	7020      	strb	r0, [r4, #0]
 2dc:	681a      	ldr	r2, [r3, #0]
 2de:	2102      	movs	r1, #2
 2e0:	438a      	bics	r2, r1
 2e2:	601a      	str	r2, [r3, #0]
 2e4:	2112      	movs	r1, #18
 2e6:	681a      	ldr	r2, [r3, #0]
 2e8:	f7ff ff18 	bl	11c <write_mbus_register>
 2ec:	2001      	movs	r0, #1
 2ee:	1c01      	adds	r1, r0, #0
 2f0:	e0f0      	b.n	4d4 <operation_cdc_run+0x230>
 2f2:	7825      	ldrb	r5, [r4, #0]
 2f4:	2d05      	cmp	r5, #5
 2f6:	d134      	bne.n	362 <operation_cdc_run+0xbe>
 2f8:	2301      	movs	r3, #1
 2fa:	7023      	strb	r3, [r4, #0]
 2fc:	4c7e      	ldr	r4, [pc, #504]	; (4f8 <operation_cdc_run+0x254>)
 2fe:	4b7f      	ldr	r3, [pc, #508]	; (4fc <operation_cdc_run+0x258>)
 300:	8822      	ldrh	r2, [r4, #0]
 302:	2100      	movs	r1, #0
 304:	4013      	ands	r3, r2
 306:	8023      	strh	r3, [r4, #0]
 308:	6822      	ldr	r2, [r4, #0]
 30a:	1c28      	adds	r0, r5, #0
 30c:	f7ff ff06 	bl	11c <write_mbus_register>
 310:	2064      	movs	r0, #100	; 0x64
 312:	f7ff feb2 	bl	7a <delay>
 316:	8822      	ldrh	r2, [r4, #0]
 318:	4b79      	ldr	r3, [pc, #484]	; (500 <operation_cdc_run+0x25c>)
 31a:	2100      	movs	r1, #0
 31c:	4013      	ands	r3, r2
 31e:	8023      	strh	r3, [r4, #0]
 320:	6822      	ldr	r2, [r4, #0]
 322:	1c28      	adds	r0, r5, #0
 324:	f7ff fefa 	bl	11c <write_mbus_register>
 328:	2064      	movs	r0, #100	; 0x64
 32a:	f7ff fea6 	bl	7a <delay>
 32e:	8822      	ldrh	r2, [r4, #0]
 330:	4b74      	ldr	r3, [pc, #464]	; (504 <operation_cdc_run+0x260>)
 332:	2100      	movs	r1, #0
 334:	4013      	ands	r3, r2
 336:	8023      	strh	r3, [r4, #0]
 338:	6822      	ldr	r2, [r4, #0]
 33a:	1c28      	adds	r0, r5, #0
 33c:	f7ff feee 	bl	11c <write_mbus_register>
 340:	2064      	movs	r0, #100	; 0x64
 342:	f7ff fe9a 	bl	7a <delay>
 346:	20c8      	movs	r0, #200	; 0xc8
 348:	f7ff fe97 	bl	7a <delay>
 34c:	8822      	ldrh	r2, [r4, #0]
 34e:	4b6e      	ldr	r3, [pc, #440]	; (508 <operation_cdc_run+0x264>)
 350:	1c28      	adds	r0, r5, #0
 352:	4013      	ands	r3, r2
 354:	8023      	strh	r3, [r4, #0]
 356:	6822      	ldr	r2, [r4, #0]
 358:	2100      	movs	r1, #0
 35a:	f7ff fedf 	bl	11c <write_mbus_register>
 35e:	2064      	movs	r0, #100	; 0x64
 360:	e052      	b.n	408 <operation_cdc_run+0x164>
 362:	7823      	ldrb	r3, [r4, #0]
 364:	2b01      	cmp	r3, #1
 366:	d152      	bne.n	40e <operation_cdc_run+0x16a>
 368:	4e68      	ldr	r6, [pc, #416]	; (50c <operation_cdc_run+0x268>)
 36a:	4c63      	ldr	r4, [pc, #396]	; (4f8 <operation_cdc_run+0x254>)
 36c:	2500      	movs	r5, #0
 36e:	7035      	strb	r5, [r6, #0]
 370:	8822      	ldrh	r2, [r4, #0]
 372:	2380      	movs	r3, #128	; 0x80
 374:	005b      	lsls	r3, r3, #1
 376:	4313      	orrs	r3, r2
 378:	8023      	strh	r3, [r4, #0]
 37a:	6822      	ldr	r2, [r4, #0]
 37c:	1c29      	adds	r1, r5, #0
 37e:	2005      	movs	r0, #5
 380:	f7ff fecc 	bl	11c <write_mbus_register>
 384:	2064      	movs	r0, #100	; 0x64
 386:	f7ff fe78 	bl	7a <delay>
 38a:	20c8      	movs	r0, #200	; 0xc8
 38c:	f7ff fe75 	bl	7a <delay>
 390:	8823      	ldrh	r3, [r4, #0]
 392:	2280      	movs	r2, #128	; 0x80
 394:	4313      	orrs	r3, r2
 396:	8023      	strh	r3, [r4, #0]
 398:	6822      	ldr	r2, [r4, #0]
 39a:	1c29      	adds	r1, r5, #0
 39c:	2005      	movs	r0, #5
 39e:	f7ff febd 	bl	11c <write_mbus_register>
 3a2:	2064      	movs	r0, #100	; 0x64
 3a4:	f7ff fe69 	bl	7a <delay>
 3a8:	25fa      	movs	r5, #250	; 0xfa
 3aa:	006d      	lsls	r5, r5, #1
 3ac:	1c34      	adds	r4, r6, #0
 3ae:	7823      	ldrb	r3, [r4, #0]
 3b0:	2b00      	cmp	r3, #0
 3b2:	d007      	beq.n	3c4 <operation_cdc_run+0x120>
 3b4:	4a4e      	ldr	r2, [pc, #312]	; (4f0 <operation_cdc_run+0x24c>)
 3b6:	2300      	movs	r3, #0
 3b8:	7023      	strb	r3, [r4, #0]
 3ba:	6013      	str	r3, [r2, #0]
 3bc:	4b4b      	ldr	r3, [pc, #300]	; (4ec <operation_cdc_run+0x248>)
 3be:	2203      	movs	r2, #3
 3c0:	701a      	strb	r2, [r3, #0]
 3c2:	e092      	b.n	4ea <operation_cdc_run+0x246>
 3c4:	2064      	movs	r0, #100	; 0x64
 3c6:	3d01      	subs	r5, #1
 3c8:	f7ff fe57 	bl	7a <delay>
 3cc:	2d00      	cmp	r5, #0
 3ce:	d1ee      	bne.n	3ae <operation_cdc_run+0x10a>
 3d0:	f7ff fefe 	bl	1d0 <release_cdc_meas>
 3d4:	4c46      	ldr	r4, [pc, #280]	; (4f0 <operation_cdc_run+0x24c>)
 3d6:	6823      	ldr	r3, [r4, #0]
 3d8:	2b00      	cmp	r3, #0
 3da:	d00e      	beq.n	3fa <operation_cdc_run+0x156>
 3dc:	6823      	ldr	r3, [r4, #0]
 3de:	3301      	adds	r3, #1
 3e0:	6023      	str	r3, [r4, #0]
 3e2:	f7ff ff07 	bl	1f4 <assert_cdc_reset>
 3e6:	6823      	ldr	r3, [r4, #0]
 3e8:	2b05      	cmp	r3, #5
 3ea:	d902      	bls.n	3f2 <operation_cdc_run+0x14e>
 3ec:	4b3f      	ldr	r3, [pc, #252]	; (4ec <operation_cdc_run+0x248>)
 3ee:	701d      	strb	r5, [r3, #0]
 3f0:	e077      	b.n	4e2 <operation_cdc_run+0x23e>
 3f2:	2002      	movs	r0, #2
 3f4:	2101      	movs	r1, #1
 3f6:	1c2a      	adds	r2, r5, #0
 3f8:	e06d      	b.n	4d6 <operation_cdc_run+0x232>
 3fa:	6823      	ldr	r3, [r4, #0]
 3fc:	3301      	adds	r3, #1
 3fe:	6023      	str	r3, [r4, #0]
 400:	f7ff fef8 	bl	1f4 <assert_cdc_reset>
 404:	20fa      	movs	r0, #250	; 0xfa
 406:	0040      	lsls	r0, r0, #1
 408:	f7ff fe37 	bl	7a <delay>
 40c:	e06d      	b.n	4ea <operation_cdc_run+0x246>
 40e:	7823      	ldrb	r3, [r4, #0]
 410:	2b03      	cmp	r3, #3
 412:	d164      	bne.n	4de <operation_cdc_run+0x23a>
 414:	2104      	movs	r1, #4
 416:	2215      	movs	r2, #21
 418:	2005      	movs	r0, #5
 41a:	f7ff fe8d 	bl	138 <read_mbus_register>
 41e:	4d3c      	ldr	r5, [pc, #240]	; (510 <operation_cdc_run+0x26c>)
 420:	2064      	movs	r0, #100	; 0x64
 422:	f7ff fe2a 	bl	7a <delay>
 426:	2106      	movs	r1, #6
 428:	2215      	movs	r2, #21
 42a:	2005      	movs	r0, #5
 42c:	682e      	ldr	r6, [r5, #0]
 42e:	f7ff fe83 	bl	138 <read_mbus_register>
 432:	2064      	movs	r0, #100	; 0x64
 434:	f7ff fe21 	bl	7a <delay>
 438:	4b36      	ldr	r3, [pc, #216]	; (514 <operation_cdc_run+0x270>)
 43a:	2700      	movs	r7, #0
 43c:	682d      	ldr	r5, [r5, #0]
 43e:	681a      	ldr	r2, [r3, #0]
 440:	601f      	str	r7, [r3, #0]
 442:	f7ff fec5 	bl	1d0 <release_cdc_meas>
 446:	f7ff fed5 	bl	1f4 <assert_cdc_reset>
 44a:	7027      	strb	r7, [r4, #0]
 44c:	f7ff fee6 	bl	21c <cdc_power_off>
 450:	4b31      	ldr	r3, [pc, #196]	; (518 <operation_cdc_run+0x274>)
 452:	781a      	ldrb	r2, [r3, #0]
 454:	42ba      	cmp	r2, r7
 456:	d003      	beq.n	460 <operation_cdc_run+0x1bc>
 458:	701f      	strb	r7, [r3, #0]
 45a:	4b30      	ldr	r3, [pc, #192]	; (51c <operation_cdc_run+0x278>)
 45c:	601d      	str	r5, [r3, #0]
 45e:	e044      	b.n	4ea <operation_cdc_run+0x246>
 460:	4b2f      	ldr	r3, [pc, #188]	; (520 <operation_cdc_run+0x27c>)
 462:	681a      	ldr	r2, [r3, #0]
 464:	3201      	adds	r2, #1
 466:	601a      	str	r2, [r3, #0]
 468:	4b2e      	ldr	r3, [pc, #184]	; (524 <operation_cdc_run+0x280>)
 46a:	681a      	ldr	r2, [r3, #0]
 46c:	2a27      	cmp	r2, #39	; 0x27
 46e:	d80f      	bhi.n	490 <operation_cdc_run+0x1ec>
 470:	6819      	ldr	r1, [r3, #0]
 472:	4a2d      	ldr	r2, [pc, #180]	; (528 <operation_cdc_run+0x284>)
 474:	0089      	lsls	r1, r1, #2
 476:	508e      	str	r6, [r1, r2]
 478:	6819      	ldr	r1, [r3, #0]
 47a:	4a2c      	ldr	r2, [pc, #176]	; (52c <operation_cdc_run+0x288>)
 47c:	0089      	lsls	r1, r1, #2
 47e:	508d      	str	r5, [r1, r2]
 480:	4a26      	ldr	r2, [pc, #152]	; (51c <operation_cdc_run+0x278>)
 482:	6015      	str	r5, [r2, #0]
 484:	681a      	ldr	r2, [r3, #0]
 486:	3201      	adds	r2, #1
 488:	601a      	str	r2, [r3, #0]
 48a:	681a      	ldr	r2, [r3, #0]
 48c:	4b28      	ldr	r3, [pc, #160]	; (530 <operation_cdc_run+0x28c>)
 48e:	601a      	str	r2, [r3, #0]
 490:	4b28      	ldr	r3, [pc, #160]	; (534 <operation_cdc_run+0x290>)
 492:	681b      	ldr	r3, [r3, #0]
 494:	2b00      	cmp	r3, #0
 496:	d010      	beq.n	4ba <operation_cdc_run+0x216>
 498:	24f0      	movs	r4, #240	; 0xf0
 49a:	0424      	lsls	r4, r4, #16
 49c:	1c20      	adds	r0, r4, #0
 49e:	4330      	orrs	r0, r6
 4a0:	f7ff fe5a 	bl	158 <send_radio_data>
 4a4:	20fa      	movs	r0, #250	; 0xfa
 4a6:	0080      	lsls	r0, r0, #2
 4a8:	f7ff fde7 	bl	7a <delay>
 4ac:	1c20      	adds	r0, r4, #0
 4ae:	4328      	orrs	r0, r5
 4b0:	f7ff fe52 	bl	158 <send_radio_data>
 4b4:	2064      	movs	r0, #100	; 0x64
 4b6:	f7ff fde0 	bl	7a <delay>
 4ba:	4b19      	ldr	r3, [pc, #100]	; (520 <operation_cdc_run+0x27c>)
 4bc:	681b      	ldr	r3, [r3, #0]
 4be:	2b03      	cmp	r3, #3
 4c0:	d804      	bhi.n	4cc <operation_cdc_run+0x228>
 4c2:	481d      	ldr	r0, [pc, #116]	; (538 <operation_cdc_run+0x294>)
 4c4:	f7ff fe48 	bl	158 <send_radio_data>
 4c8:	4b1c      	ldr	r3, [pc, #112]	; (53c <operation_cdc_run+0x298>)
 4ca:	e000      	b.n	4ce <operation_cdc_run+0x22a>
 4cc:	4b1c      	ldr	r3, [pc, #112]	; (540 <operation_cdc_run+0x29c>)
 4ce:	6818      	ldr	r0, [r3, #0]
 4d0:	2101      	movs	r1, #1
 4d2:	b280      	uxth	r0, r0
 4d4:	2200      	movs	r2, #0
 4d6:	f7ff fdf3 	bl	c0 <set_wakeup_timer>
 4da:	f7ff fecd 	bl	278 <operation_sleep>
 4de:	f7ff fe89 	bl	1f4 <assert_cdc_reset>
 4e2:	f7ff fe9b 	bl	21c <cdc_power_off>
 4e6:	f7ff fed3 	bl	290 <operation_sleep_notimer>
 4ea:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 4ec:	000009c8 	.word	0x000009c8
 4f0:	000009bc 	.word	0x000009bc
 4f4:	000009a0 	.word	0x000009a0
 4f8:	000009a4 	.word	0x000009a4
 4fc:	fffff7ff 	.word	0xfffff7ff
 500:	fffffbff 	.word	0xfffffbff
 504:	ffffefff 	.word	0xffffefff
 508:	fffffdff 	.word	0xfffffdff
 50c:	000009ec 	.word	0x000009ec
 510:	a0001014 	.word	0xa0001014
 514:	000009e8 	.word	0x000009e8
 518:	000009b4 	.word	0x000009b4
 51c:	000009f0 	.word	0x000009f0
 520:	000009b8 	.word	0x000009b8
 524:	000009dc 	.word	0x000009dc
 528:	00000aa0 	.word	0x00000aa0
 52c:	00000a00 	.word	0x00000a00
 530:	000009b0 	.word	0x000009b0
 534:	000009d8 	.word	0x000009d8
 538:	00faf000 	.word	0x00faf000
 53c:	000009e0 	.word	0x000009e0
 540:	000009f8 	.word	0x000009f8

Disassembly of section .text.handler_ext_int_0:

00000544 <handler_ext_int_0>:
 544:	4b03      	ldr	r3, [pc, #12]	; (554 <handler_ext_int_0+0x10>)
 546:	2201      	movs	r2, #1
 548:	601a      	str	r2, [r3, #0]
 54a:	4b03      	ldr	r3, [pc, #12]	; (558 <handler_ext_int_0+0x14>)
 54c:	2210      	movs	r2, #16
 54e:	701a      	strb	r2, [r3, #0]
 550:	4770      	bx	lr
 552:	46c0      	nop			; (mov r8, r8)
 554:	e000e280 	.word	0xe000e280
 558:	000009ec 	.word	0x000009ec

Disassembly of section .text.handler_ext_int_1:

0000055c <handler_ext_int_1>:
 55c:	4b03      	ldr	r3, [pc, #12]	; (56c <handler_ext_int_1+0x10>)
 55e:	2202      	movs	r2, #2
 560:	601a      	str	r2, [r3, #0]
 562:	4b03      	ldr	r3, [pc, #12]	; (570 <handler_ext_int_1+0x14>)
 564:	2211      	movs	r2, #17
 566:	701a      	strb	r2, [r3, #0]
 568:	4770      	bx	lr
 56a:	46c0      	nop			; (mov r8, r8)
 56c:	e000e280 	.word	0xe000e280
 570:	000009ec 	.word	0x000009ec

Disassembly of section .text.handler_ext_int_2:

00000574 <handler_ext_int_2>:
 574:	4b03      	ldr	r3, [pc, #12]	; (584 <handler_ext_int_2+0x10>)
 576:	2204      	movs	r2, #4
 578:	601a      	str	r2, [r3, #0]
 57a:	4b03      	ldr	r3, [pc, #12]	; (588 <handler_ext_int_2+0x14>)
 57c:	2212      	movs	r2, #18
 57e:	701a      	strb	r2, [r3, #0]
 580:	4770      	bx	lr
 582:	46c0      	nop			; (mov r8, r8)
 584:	e000e280 	.word	0xe000e280
 588:	000009ec 	.word	0x000009ec

Disassembly of section .text.handler_ext_int_3:

0000058c <handler_ext_int_3>:
 58c:	4b03      	ldr	r3, [pc, #12]	; (59c <handler_ext_int_3+0x10>)
 58e:	2208      	movs	r2, #8
 590:	601a      	str	r2, [r3, #0]
 592:	4b03      	ldr	r3, [pc, #12]	; (5a0 <handler_ext_int_3+0x14>)
 594:	2213      	movs	r2, #19
 596:	701a      	strb	r2, [r3, #0]
 598:	4770      	bx	lr
 59a:	46c0      	nop			; (mov r8, r8)
 59c:	e000e280 	.word	0xe000e280
 5a0:	000009ec 	.word	0x000009ec

Disassembly of section .text.startup.main:

000005a4 <main>:
 5a4:	4bd9      	ldr	r3, [pc, #868]	; (90c <main+0x368>)
 5a6:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
 5a8:	270f      	movs	r7, #15
 5aa:	601f      	str	r7, [r3, #0]
 5ac:	4bd8      	ldr	r3, [pc, #864]	; (910 <main+0x36c>)
 5ae:	2000      	movs	r0, #0
 5b0:	601f      	str	r7, [r3, #0]
 5b2:	4bd8      	ldr	r3, [pc, #864]	; (914 <main+0x370>)
 5b4:	2101      	movs	r1, #1
 5b6:	9300      	str	r3, [sp, #0]
 5b8:	1c02      	adds	r2, r0, #0
 5ba:	1c03      	adds	r3, r0, #0
 5bc:	f7ff fd66 	bl	8c <config_timer>
 5c0:	4dd5      	ldr	r5, [pc, #852]	; (918 <main+0x374>)
 5c2:	4ed6      	ldr	r6, [pc, #856]	; (91c <main+0x378>)
 5c4:	682b      	ldr	r3, [r5, #0]
 5c6:	42b3      	cmp	r3, r6
 5c8:	d100      	bne.n	5cc <main+0x28>
 5ca:	e0b8      	b.n	73e <main+0x19a>
 5cc:	4ad4      	ldr	r2, [pc, #848]	; (920 <main+0x37c>)
 5ce:	4bd5      	ldr	r3, [pc, #852]	; (924 <main+0x380>)
 5d0:	2064      	movs	r0, #100	; 0x64
 5d2:	601a      	str	r2, [r3, #0]
 5d4:	4ad4      	ldr	r2, [pc, #848]	; (928 <main+0x384>)
 5d6:	4bd5      	ldr	r3, [pc, #852]	; (92c <main+0x388>)
 5d8:	2400      	movs	r4, #0
 5da:	601a      	str	r2, [r3, #0]
 5dc:	f7ff fd4d 	bl	7a <delay>
 5e0:	4bd3      	ldr	r3, [pc, #844]	; (930 <main+0x38c>)
 5e2:	2004      	movs	r0, #4
 5e4:	701c      	strb	r4, [r3, #0]
 5e6:	4bd3      	ldr	r3, [pc, #844]	; (934 <main+0x390>)
 5e8:	602e      	str	r6, [r5, #0]
 5ea:	601c      	str	r4, [r3, #0]
 5ec:	4bd2      	ldr	r3, [pc, #840]	; (938 <main+0x394>)
 5ee:	2501      	movs	r5, #1
 5f0:	601c      	str	r4, [r3, #0]
 5f2:	4bd2      	ldr	r3, [pc, #840]	; (93c <main+0x398>)
 5f4:	2602      	movs	r6, #2
 5f6:	701c      	strb	r4, [r3, #0]
 5f8:	f7ff fd78 	bl	ec <enumerate>
 5fc:	20fa      	movs	r0, #250	; 0xfa
 5fe:	0080      	lsls	r0, r0, #2
 600:	f7ff fd3b 	bl	7a <delay>
 604:	2005      	movs	r0, #5
 606:	f7ff fd71 	bl	ec <enumerate>
 60a:	20fa      	movs	r0, #250	; 0xfa
 60c:	0080      	lsls	r0, r0, #2
 60e:	f7ff fd34 	bl	7a <delay>
 612:	2006      	movs	r0, #6
 614:	f7ff fd6a 	bl	ec <enumerate>
 618:	20fa      	movs	r0, #250	; 0xfa
 61a:	0080      	lsls	r0, r0, #2
 61c:	f7ff fd2d 	bl	7a <delay>
 620:	4bc7      	ldr	r3, [pc, #796]	; (940 <main+0x39c>)
 622:	2005      	movs	r0, #5
 624:	881a      	ldrh	r2, [r3, #0]
 626:	432a      	orrs	r2, r5
 628:	801a      	strh	r2, [r3, #0]
 62a:	881a      	ldrh	r2, [r3, #0]
 62c:	43b2      	bics	r2, r6
 62e:	801a      	strh	r2, [r3, #0]
 630:	8819      	ldrh	r1, [r3, #0]
 632:	4ac4      	ldr	r2, [pc, #784]	; (944 <main+0x3a0>)
 634:	400a      	ands	r2, r1
 636:	801a      	strh	r2, [r3, #0]
 638:	681a      	ldr	r2, [r3, #0]
 63a:	1c21      	adds	r1, r4, #0
 63c:	f7ff fd6e 	bl	11c <write_mbus_register>
 640:	4bc1      	ldr	r3, [pc, #772]	; (948 <main+0x3a4>)
 642:	1c29      	adds	r1, r5, #0
 644:	681a      	ldr	r2, [r3, #0]
 646:	2005      	movs	r0, #5
 648:	0a92      	lsrs	r2, r2, #10
 64a:	0292      	lsls	r2, r2, #10
 64c:	432a      	orrs	r2, r5
 64e:	601a      	str	r2, [r3, #0]
 650:	681a      	ldr	r2, [r3, #0]
 652:	f7ff fd63 	bl	11c <write_mbus_register>
 656:	4bbd      	ldr	r3, [pc, #756]	; (94c <main+0x3a8>)
 658:	4abd      	ldr	r2, [pc, #756]	; (950 <main+0x3ac>)
 65a:	6819      	ldr	r1, [r3, #0]
 65c:	2005      	movs	r0, #5
 65e:	400a      	ands	r2, r1
 660:	21c8      	movs	r1, #200	; 0xc8
 662:	0249      	lsls	r1, r1, #9
 664:	430a      	orrs	r2, r1
 666:	601a      	str	r2, [r3, #0]
 668:	681a      	ldr	r2, [r3, #0]
 66a:	1c31      	adds	r1, r6, #0
 66c:	f7ff fd56 	bl	11c <write_mbus_register>
 670:	4bb8      	ldr	r3, [pc, #736]	; (954 <main+0x3b0>)
 672:	4ab9      	ldr	r2, [pc, #740]	; (958 <main+0x3b4>)
 674:	6819      	ldr	r1, [r3, #0]
 676:	2005      	movs	r0, #5
 678:	400a      	ands	r2, r1
 67a:	601a      	str	r2, [r3, #0]
 67c:	6819      	ldr	r1, [r3, #0]
 67e:	22c0      	movs	r2, #192	; 0xc0
 680:	01d2      	lsls	r2, r2, #7
 682:	430a      	orrs	r2, r1
 684:	601a      	str	r2, [r3, #0]
 686:	6819      	ldr	r1, [r3, #0]
 688:	4ab4      	ldr	r2, [pc, #720]	; (95c <main+0x3b8>)
 68a:	400a      	ands	r2, r1
 68c:	2180      	movs	r1, #128	; 0x80
 68e:	0349      	lsls	r1, r1, #13
 690:	430a      	orrs	r2, r1
 692:	601a      	str	r2, [r3, #0]
 694:	681a      	ldr	r2, [r3, #0]
 696:	210c      	movs	r1, #12
 698:	438a      	bics	r2, r1
 69a:	601a      	str	r2, [r3, #0]
 69c:	6819      	ldr	r1, [r3, #0]
 69e:	4ab0      	ldr	r2, [pc, #704]	; (960 <main+0x3bc>)
 6a0:	400a      	ands	r2, r1
 6a2:	2180      	movs	r1, #128	; 0x80
 6a4:	40b1      	lsls	r1, r6
 6a6:	430a      	orrs	r2, r1
 6a8:	601a      	str	r2, [r3, #0]
 6aa:	681a      	ldr	r2, [r3, #0]
 6ac:	2112      	movs	r1, #18
 6ae:	f7ff fd35 	bl	11c <write_mbus_register>
 6b2:	2123      	movs	r1, #35	; 0x23
 6b4:	1c22      	adds	r2, r4, #0
 6b6:	2004      	movs	r0, #4
 6b8:	f7ff fd30 	bl	11c <write_mbus_register>
 6bc:	261f      	movs	r6, #31
 6be:	2064      	movs	r0, #100	; 0x64
 6c0:	f7ff fcdb 	bl	7a <delay>
 6c4:	2126      	movs	r1, #38	; 0x26
 6c6:	1c32      	adds	r2, r6, #0
 6c8:	2004      	movs	r0, #4
 6ca:	f7ff fd27 	bl	11c <write_mbus_register>
 6ce:	2064      	movs	r0, #100	; 0x64
 6d0:	f7ff fcd3 	bl	7a <delay>
 6d4:	1c32      	adds	r2, r6, #0
 6d6:	2120      	movs	r1, #32
 6d8:	2004      	movs	r0, #4
 6da:	f7ff fd1f 	bl	11c <write_mbus_register>
 6de:	2064      	movs	r0, #100	; 0x64
 6e0:	f7ff fccb 	bl	7a <delay>
 6e4:	2121      	movs	r1, #33	; 0x21
 6e6:	1c22      	adds	r2, r4, #0
 6e8:	2004      	movs	r0, #4
 6ea:	f7ff fd17 	bl	11c <write_mbus_register>
 6ee:	2064      	movs	r0, #100	; 0x64
 6f0:	f7ff fcc3 	bl	7a <delay>
 6f4:	2122      	movs	r1, #34	; 0x22
 6f6:	1c22      	adds	r2, r4, #0
 6f8:	2004      	movs	r0, #4
 6fa:	f7ff fd0f 	bl	11c <write_mbus_register>
 6fe:	2064      	movs	r0, #100	; 0x64
 700:	f7ff fcbb 	bl	7a <delay>
 704:	2125      	movs	r1, #37	; 0x25
 706:	2205      	movs	r2, #5
 708:	2004      	movs	r0, #4
 70a:	f7ff fd07 	bl	11c <write_mbus_register>
 70e:	2064      	movs	r0, #100	; 0x64
 710:	f7ff fcb3 	bl	7a <delay>
 714:	1c22      	adds	r2, r4, #0
 716:	2004      	movs	r0, #4
 718:	2127      	movs	r1, #39	; 0x27
 71a:	f7ff fcff 	bl	11c <write_mbus_register>
 71e:	4b91      	ldr	r3, [pc, #580]	; (964 <main+0x3c0>)
 720:	2264      	movs	r2, #100	; 0x64
 722:	601a      	str	r2, [r3, #0]
 724:	4b90      	ldr	r3, [pc, #576]	; (968 <main+0x3c4>)
 726:	601d      	str	r5, [r3, #0]
 728:	4b90      	ldr	r3, [pc, #576]	; (96c <main+0x3c8>)
 72a:	601c      	str	r4, [r3, #0]
 72c:	4b90      	ldr	r3, [pc, #576]	; (970 <main+0x3cc>)
 72e:	601c      	str	r4, [r3, #0]
 730:	4b90      	ldr	r3, [pc, #576]	; (974 <main+0x3d0>)
 732:	601c      	str	r4, [r3, #0]
 734:	4b90      	ldr	r3, [pc, #576]	; (978 <main+0x3d4>)
 736:	701c      	strb	r4, [r3, #0]
 738:	4b90      	ldr	r3, [pc, #576]	; (97c <main+0x3d8>)
 73a:	701c      	strb	r4, [r3, #0]
 73c:	e022      	b.n	784 <main+0x1e0>
 73e:	2668      	movs	r6, #104	; 0x68
 740:	6834      	ldr	r4, [r6, #0]
 742:	0e25      	lsrs	r5, r4, #24
 744:	0a23      	lsrs	r3, r4, #8
 746:	2d01      	cmp	r5, #1
 748:	d11e      	bne.n	788 <main+0x1e4>
 74a:	27ff      	movs	r7, #255	; 0xff
 74c:	4e86      	ldr	r6, [pc, #536]	; (968 <main+0x3c4>)
 74e:	403b      	ands	r3, r7
 750:	2064      	movs	r0, #100	; 0x64
 752:	6033      	str	r3, [r6, #0]
 754:	f7ff fc91 	bl	7a <delay>
 758:	4b77      	ldr	r3, [pc, #476]	; (938 <main+0x394>)
 75a:	403c      	ands	r4, r7
 75c:	681a      	ldr	r2, [r3, #0]
 75e:	42a2      	cmp	r2, r4
 760:	d209      	bcs.n	776 <main+0x1d2>
 762:	681a      	ldr	r2, [r3, #0]
 764:	3201      	adds	r2, #1
 766:	601a      	str	r2, [r3, #0]
 768:	6818      	ldr	r0, [r3, #0]
 76a:	4b85      	ldr	r3, [pc, #532]	; (980 <main+0x3dc>)
 76c:	18c0      	adds	r0, r0, r3
 76e:	f7ff fcf3 	bl	158 <send_radio_data>
 772:	6830      	ldr	r0, [r6, #0]
 774:	e077      	b.n	866 <main+0x2c2>
 776:	2200      	movs	r2, #0
 778:	601a      	str	r2, [r3, #0]
 77a:	6818      	ldr	r0, [r3, #0]
 77c:	4b80      	ldr	r3, [pc, #512]	; (980 <main+0x3dc>)
 77e:	18c0      	adds	r0, r0, r3
 780:	f7ff fcea 	bl	158 <send_radio_data>
 784:	f7ff fd84 	bl	290 <operation_sleep_notimer>
 788:	0c22      	lsrs	r2, r4, #16
 78a:	2d02      	cmp	r5, #2
 78c:	d12b      	bne.n	7e6 <main+0x242>
 78e:	21ff      	movs	r1, #255	; 0xff
 790:	400b      	ands	r3, r1
 792:	400c      	ands	r4, r1
 794:	021b      	lsls	r3, r3, #8
 796:	18e4      	adds	r4, r4, r3
 798:	4872      	ldr	r0, [pc, #456]	; (964 <main+0x3c0>)
 79a:	4b73      	ldr	r3, [pc, #460]	; (968 <main+0x3c4>)
 79c:	4017      	ands	r7, r2
 79e:	6004      	str	r4, [r0, #0]
 7a0:	601f      	str	r7, [r3, #0]
 7a2:	2310      	movs	r3, #16
 7a4:	401a      	ands	r2, r3
 7a6:	4b73      	ldr	r3, [pc, #460]	; (974 <main+0x3d0>)
 7a8:	2064      	movs	r0, #100	; 0x64
 7aa:	601a      	str	r2, [r3, #0]
 7ac:	4a75      	ldr	r2, [pc, #468]	; (984 <main+0x3e0>)
 7ae:	4b5d      	ldr	r3, [pc, #372]	; (924 <main+0x380>)
 7b0:	601a      	str	r2, [r3, #0]
 7b2:	f7ff fc62 	bl	7a <delay>
 7b6:	4c71      	ldr	r4, [pc, #452]	; (97c <main+0x3d8>)
 7b8:	7822      	ldrb	r2, [r4, #0]
 7ba:	2a00      	cmp	r2, #0
 7bc:	d106      	bne.n	7cc <main+0x228>
 7be:	2005      	movs	r0, #5
 7c0:	2101      	movs	r1, #1
 7c2:	f7ff fc7d 	bl	c0 <set_wakeup_timer>
 7c6:	2301      	movs	r3, #1
 7c8:	7023      	strb	r3, [r4, #0]
 7ca:	e051      	b.n	870 <main+0x2cc>
 7cc:	4a59      	ldr	r2, [pc, #356]	; (934 <main+0x390>)
 7ce:	2300      	movs	r3, #0
 7d0:	6013      	str	r3, [r2, #0]
 7d2:	4a6d      	ldr	r2, [pc, #436]	; (988 <main+0x3e4>)
 7d4:	6013      	str	r3, [r2, #0]
 7d6:	4a65      	ldr	r2, [pc, #404]	; (96c <main+0x3c8>)
 7d8:	6013      	str	r3, [r2, #0]
 7da:	4a65      	ldr	r2, [pc, #404]	; (970 <main+0x3cc>)
 7dc:	6013      	str	r3, [r2, #0]
 7de:	4a6b      	ldr	r2, [pc, #428]	; (98c <main+0x3e8>)
 7e0:	6033      	str	r3, [r6, #0]
 7e2:	6013      	str	r3, [r2, #0]
 7e4:	e08e      	b.n	904 <main+0x360>
 7e6:	2d03      	cmp	r5, #3
 7e8:	d11b      	bne.n	822 <main+0x27e>
 7ea:	495f      	ldr	r1, [pc, #380]	; (968 <main+0x3c4>)
 7ec:	b2db      	uxtb	r3, r3
 7ee:	600b      	str	r3, [r1, #0]
 7f0:	4b62      	ldr	r3, [pc, #392]	; (97c <main+0x3d8>)
 7f2:	2100      	movs	r1, #0
 7f4:	7019      	strb	r1, [r3, #0]
 7f6:	07d3      	lsls	r3, r2, #31
 7f8:	d502      	bpl.n	800 <main+0x25c>
 7fa:	4a49      	ldr	r2, [pc, #292]	; (920 <main+0x37c>)
 7fc:	4b49      	ldr	r3, [pc, #292]	; (924 <main+0x380>)
 7fe:	601a      	str	r2, [r3, #0]
 800:	4b4d      	ldr	r3, [pc, #308]	; (938 <main+0x394>)
 802:	b2e4      	uxtb	r4, r4
 804:	681a      	ldr	r2, [r3, #0]
 806:	42a2      	cmp	r2, r4
 808:	d209      	bcs.n	81e <main+0x27a>
 80a:	681a      	ldr	r2, [r3, #0]
 80c:	3201      	adds	r2, #1
 80e:	601a      	str	r2, [r3, #0]
 810:	4b48      	ldr	r3, [pc, #288]	; (934 <main+0x390>)
 812:	6818      	ldr	r0, [r3, #0]
 814:	4b5a      	ldr	r3, [pc, #360]	; (980 <main+0x3dc>)
 816:	18c0      	adds	r0, r0, r3
 818:	f7ff fc9e 	bl	158 <send_radio_data>
 81c:	e05d      	b.n	8da <main+0x336>
 81e:	2200      	movs	r2, #0
 820:	e06e      	b.n	900 <main+0x35c>
 822:	2d04      	cmp	r5, #4
 824:	d168      	bne.n	8f8 <main+0x354>
 826:	22ff      	movs	r2, #255	; 0xff
 828:	494f      	ldr	r1, [pc, #316]	; (968 <main+0x3c4>)
 82a:	4013      	ands	r3, r2
 82c:	600b      	str	r3, [r1, #0]
 82e:	4b58      	ldr	r3, [pc, #352]	; (990 <main+0x3ec>)
 830:	4014      	ands	r4, r2
 832:	4a4e      	ldr	r2, [pc, #312]	; (96c <main+0x3c8>)
 834:	601c      	str	r4, [r3, #0]
 836:	6819      	ldr	r1, [r3, #0]
 838:	6812      	ldr	r2, [r2, #0]
 83a:	4291      	cmp	r1, r2
 83c:	d301      	bcc.n	842 <main+0x29e>
 83e:	2200      	movs	r2, #0
 840:	601a      	str	r2, [r3, #0]
 842:	4c3d      	ldr	r4, [pc, #244]	; (938 <main+0x394>)
 844:	6823      	ldr	r3, [r4, #0]
 846:	2b02      	cmp	r3, #2
 848:	d814      	bhi.n	874 <main+0x2d0>
 84a:	6823      	ldr	r3, [r4, #0]
 84c:	3301      	adds	r3, #1
 84e:	6023      	str	r3, [r4, #0]
 850:	6820      	ldr	r0, [r4, #0]
 852:	4b4b      	ldr	r3, [pc, #300]	; (980 <main+0x3dc>)
 854:	18c0      	adds	r0, r0, r3
 856:	f7ff fc7f 	bl	158 <send_radio_data>
 85a:	6822      	ldr	r2, [r4, #0]
 85c:	4b42      	ldr	r3, [pc, #264]	; (968 <main+0x3c4>)
 85e:	6818      	ldr	r0, [r3, #0]
 860:	2a02      	cmp	r2, #2
 862:	d100      	bne.n	866 <main+0x2c2>
 864:	0040      	lsls	r0, r0, #1
 866:	b280      	uxth	r0, r0
 868:	2101      	movs	r1, #1
 86a:	2200      	movs	r2, #0
 86c:	f7ff fc28 	bl	c0 <set_wakeup_timer>
 870:	f7ff fcf8 	bl	264 <operation_sleep_noirqreset>
 874:	4c3e      	ldr	r4, [pc, #248]	; (970 <main+0x3cc>)
 876:	4b47      	ldr	r3, [pc, #284]	; (994 <main+0x3f0>)
 878:	6822      	ldr	r2, [r4, #0]
 87a:	25f0      	movs	r5, #240	; 0xf0
 87c:	0092      	lsls	r2, r2, #2
 87e:	58d7      	ldr	r7, [r2, r3]
 880:	6822      	ldr	r2, [r4, #0]
 882:	4b45      	ldr	r3, [pc, #276]	; (998 <main+0x3f4>)
 884:	0092      	lsls	r2, r2, #2
 886:	042d      	lsls	r5, r5, #16
 888:	2064      	movs	r0, #100	; 0x64
 88a:	58d6      	ldr	r6, [r2, r3]
 88c:	f7ff fbf5 	bl	7a <delay>
 890:	1c28      	adds	r0, r5, #0
 892:	4338      	orrs	r0, r7
 894:	f7ff fc60 	bl	158 <send_radio_data>
 898:	4840      	ldr	r0, [pc, #256]	; (99c <main+0x3f8>)
 89a:	f7ff fbee 	bl	7a <delay>
 89e:	1c28      	adds	r0, r5, #0
 8a0:	4330      	orrs	r0, r6
 8a2:	f7ff fc59 	bl	158 <send_radio_data>
 8a6:	4a3a      	ldr	r2, [pc, #232]	; (990 <main+0x3ec>)
 8a8:	2500      	movs	r5, #0
 8aa:	6813      	ldr	r3, [r2, #0]
 8ac:	42ab      	cmp	r3, r5
 8ae:	d102      	bne.n	8b6 <main+0x312>
 8b0:	6825      	ldr	r5, [r4, #0]
 8b2:	1e6b      	subs	r3, r5, #1
 8b4:	419d      	sbcs	r5, r3
 8b6:	6811      	ldr	r1, [r2, #0]
 8b8:	2300      	movs	r3, #0
 8ba:	4299      	cmp	r1, r3
 8bc:	d007      	beq.n	8ce <main+0x32a>
 8be:	6813      	ldr	r3, [r2, #0]
 8c0:	4a2a      	ldr	r2, [pc, #168]	; (96c <main+0x3c8>)
 8c2:	6821      	ldr	r1, [r4, #0]
 8c4:	6812      	ldr	r2, [r2, #0]
 8c6:	18cb      	adds	r3, r1, r3
 8c8:	429a      	cmp	r2, r3
 8ca:	419b      	sbcs	r3, r3
 8cc:	425b      	negs	r3, r3
 8ce:	431d      	orrs	r5, r3
 8d0:	4c27      	ldr	r4, [pc, #156]	; (970 <main+0x3cc>)
 8d2:	d005      	beq.n	8e0 <main+0x33c>
 8d4:	6823      	ldr	r3, [r4, #0]
 8d6:	3b01      	subs	r3, #1
 8d8:	6023      	str	r3, [r4, #0]
 8da:	4b23      	ldr	r3, [pc, #140]	; (968 <main+0x3c4>)
 8dc:	6818      	ldr	r0, [r3, #0]
 8de:	e7c2      	b.n	866 <main+0x2c2>
 8e0:	482e      	ldr	r0, [pc, #184]	; (99c <main+0x3f8>)
 8e2:	f7ff fbca 	bl	7a <delay>
 8e6:	4826      	ldr	r0, [pc, #152]	; (980 <main+0x3dc>)
 8e8:	f7ff fc36 	bl	158 <send_radio_data>
 8ec:	4b12      	ldr	r3, [pc, #72]	; (938 <main+0x394>)
 8ee:	601d      	str	r5, [r3, #0]
 8f0:	4b1e      	ldr	r3, [pc, #120]	; (96c <main+0x3c8>)
 8f2:	681b      	ldr	r3, [r3, #0]
 8f4:	6023      	str	r3, [r4, #0]
 8f6:	e745      	b.n	784 <main+0x1e0>
 8f8:	2d11      	cmp	r5, #17
 8fa:	d103      	bne.n	904 <main+0x360>
 8fc:	4a21      	ldr	r2, [pc, #132]	; (984 <main+0x3e0>)
 8fe:	4b09      	ldr	r3, [pc, #36]	; (924 <main+0x380>)
 900:	601a      	str	r2, [r3, #0]
 902:	e73f      	b.n	784 <main+0x1e0>
 904:	f7ff fcce 	bl	2a4 <operation_cdc_run>
 908:	e7fc      	b.n	904 <main+0x360>
 90a:	46c0      	nop			; (mov r8, r8)
 90c:	e000e280 	.word	0xe000e280
 910:	e000e100 	.word	0xe000e100
 914:	000f4240 	.word	0x000f4240
 918:	000009c4 	.word	0x000009c4
 91c:	deadbeef 	.word	0xdeadbeef
 920:	8f77184b 	.word	0x8f77184b
 924:	a200000c 	.word	0xa200000c
 928:	00202d08 	.word	0x00202d08
 92c:	a2000008 	.word	0xa2000008
 930:	000009c8 	.word	0x000009c8
 934:	000009b8 	.word	0x000009b8
 938:	000009f4 	.word	0x000009f4
 93c:	000009ec 	.word	0x000009ec
 940:	000009a4 	.word	0x000009a4
 944:	fffffeff 	.word	0xfffffeff
 948:	000009a8 	.word	0x000009a8
 94c:	000009ac 	.word	0x000009ac
 950:	fff003ff 	.word	0xfff003ff
 954:	000009a0 	.word	0x000009a0
 958:	fffffbff 	.word	0xfffffbff
 95c:	ffe07fff 	.word	0xffe07fff
 960:	fffffc0f 	.word	0xfffffc0f
 964:	000009f8 	.word	0x000009f8
 968:	000009e0 	.word	0x000009e0
 96c:	000009dc 	.word	0x000009dc
 970:	000009b0 	.word	0x000009b0
 974:	000009d8 	.word	0x000009d8
 978:	000009b4 	.word	0x000009b4
 97c:	000009fc 	.word	0x000009fc
 980:	00faf000 	.word	0x00faf000
 984:	8f77183b 	.word	0x8f77183b
 988:	000009e8 	.word	0x000009e8
 98c:	000009bc 	.word	0x000009bc
 990:	000009e4 	.word	0x000009e4
 994:	00000aa0 	.word	0x00000aa0
 998:	00000a00 	.word	0x00000a00
 99c:	00001770 	.word	0x00001770

