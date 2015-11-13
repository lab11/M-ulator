
XO_test_0/XO_test_0.elf:     file format elf32-littlearm


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
  40:	00000169 	.word	0x00000169
  44:	00000175 	.word	0x00000175
  48:	00000181 	.word	0x00000181
  4c:	0000018d 	.word	0x0000018d
  50:	00000199 	.word	0x00000199
  54:	000001a5 	.word	0x000001a5
  58:	000001b1 	.word	0x000001b1
  5c:	000001bd 	.word	0x000001bd
  60:	000001c9 	.word	0x000001c9
  64:	000001d9 	.word	0x000001d9
  68:	000001e9 	.word	0x000001e9
  6c:	000001f9 	.word	0x000001f9
  70:	00000209 	.word	0x00000209
  74:	00000219 	.word	0x00000219
  78:	00000229 	.word	0x00000229
  7c:	00000239 	.word	0x00000239
  80:	00000249 	.word	0x00000249
	...

00000090 <hang>:
  90:	e7fe      	b.n	90 <hang>
	...

000000a0 <_start>:
  a0:	f000 f8f8 	bl	294 <main>
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

Disassembly of section .text.set_halt_until_mbus_rx:

00000110 <set_halt_until_mbus_rx>:
 110:	4805      	ldr	r0, [pc, #20]	; (128 <set_halt_until_mbus_rx+0x18>)
 112:	b508      	push	{r3, lr}
 114:	6801      	ldr	r1, [r0, #0]
 116:	4b05      	ldr	r3, [pc, #20]	; (12c <set_halt_until_mbus_rx+0x1c>)
 118:	4019      	ands	r1, r3
 11a:	2390      	movs	r3, #144	; 0x90
 11c:	021b      	lsls	r3, r3, #8
 11e:	4319      	orrs	r1, r3
 120:	f7ff ffe2 	bl	e8 <write_regfile>
 124:	bd08      	pop	{r3, pc}
 126:	46c0      	nop			; (mov r8, r8)
 128:	a0000028 	.word	0xa0000028
 12c:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.set_halt_until_mbus_tx:

00000130 <set_halt_until_mbus_tx>:
 130:	4805      	ldr	r0, [pc, #20]	; (148 <set_halt_until_mbus_tx+0x18>)
 132:	b508      	push	{r3, lr}
 134:	6801      	ldr	r1, [r0, #0]
 136:	4b05      	ldr	r3, [pc, #20]	; (14c <set_halt_until_mbus_tx+0x1c>)
 138:	4019      	ands	r1, r3
 13a:	23a0      	movs	r3, #160	; 0xa0
 13c:	021b      	lsls	r3, r3, #8
 13e:	4319      	orrs	r1, r3
 140:	f7ff ffd2 	bl	e8 <write_regfile>
 144:	bd08      	pop	{r3, pc}
 146:	46c0      	nop			; (mov r8, r8)
 148:	a0000028 	.word	0xa0000028
 14c:	ffff0fff 	.word	0xffff0fff

Disassembly of section .text.init_interrupt:

00000150 <init_interrupt>:
 150:	4b02      	ldr	r3, [pc, #8]	; (15c <init_interrupt+0xc>)
 152:	4a03      	ldr	r2, [pc, #12]	; (160 <init_interrupt+0x10>)
 154:	6013      	str	r3, [r2, #0]
 156:	4a03      	ldr	r2, [pc, #12]	; (164 <init_interrupt+0x14>)
 158:	6013      	str	r3, [r2, #0]
 15a:	4770      	bx	lr
 15c:	0001ffff 	.word	0x0001ffff
 160:	e000e280 	.word	0xe000e280
 164:	e000e100 	.word	0xe000e100

Disassembly of section .text.handler_ext_int_0:

00000168 <handler_ext_int_0>:
 168:	4b01      	ldr	r3, [pc, #4]	; (170 <handler_ext_int_0+0x8>)
 16a:	2201      	movs	r2, #1
 16c:	601a      	str	r2, [r3, #0]
 16e:	4770      	bx	lr
 170:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

00000174 <handler_ext_int_1>:
 174:	4b01      	ldr	r3, [pc, #4]	; (17c <handler_ext_int_1+0x8>)
 176:	2202      	movs	r2, #2
 178:	601a      	str	r2, [r3, #0]
 17a:	4770      	bx	lr
 17c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

00000180 <handler_ext_int_2>:
 180:	4b01      	ldr	r3, [pc, #4]	; (188 <handler_ext_int_2+0x8>)
 182:	2204      	movs	r2, #4
 184:	601a      	str	r2, [r3, #0]
 186:	4770      	bx	lr
 188:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

0000018c <handler_ext_int_3>:
 18c:	4b01      	ldr	r3, [pc, #4]	; (194 <handler_ext_int_3+0x8>)
 18e:	2208      	movs	r2, #8
 190:	601a      	str	r2, [r3, #0]
 192:	4770      	bx	lr
 194:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

00000198 <handler_ext_int_4>:
 198:	4b01      	ldr	r3, [pc, #4]	; (1a0 <handler_ext_int_4+0x8>)
 19a:	2210      	movs	r2, #16
 19c:	601a      	str	r2, [r3, #0]
 19e:	4770      	bx	lr
 1a0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

000001a4 <handler_ext_int_5>:
 1a4:	4b01      	ldr	r3, [pc, #4]	; (1ac <handler_ext_int_5+0x8>)
 1a6:	2220      	movs	r2, #32
 1a8:	601a      	str	r2, [r3, #0]
 1aa:	4770      	bx	lr
 1ac:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

000001b0 <handler_ext_int_6>:
 1b0:	4b01      	ldr	r3, [pc, #4]	; (1b8 <handler_ext_int_6+0x8>)
 1b2:	2240      	movs	r2, #64	; 0x40
 1b4:	601a      	str	r2, [r3, #0]
 1b6:	4770      	bx	lr
 1b8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

000001bc <handler_ext_int_7>:
 1bc:	4b01      	ldr	r3, [pc, #4]	; (1c4 <handler_ext_int_7+0x8>)
 1be:	2280      	movs	r2, #128	; 0x80
 1c0:	601a      	str	r2, [r3, #0]
 1c2:	4770      	bx	lr
 1c4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

000001c8 <handler_ext_int_8>:
 1c8:	4b02      	ldr	r3, [pc, #8]	; (1d4 <handler_ext_int_8+0xc>)
 1ca:	2280      	movs	r2, #128	; 0x80
 1cc:	0052      	lsls	r2, r2, #1
 1ce:	601a      	str	r2, [r3, #0]
 1d0:	4770      	bx	lr
 1d2:	46c0      	nop			; (mov r8, r8)
 1d4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

000001d8 <handler_ext_int_9>:
 1d8:	4b02      	ldr	r3, [pc, #8]	; (1e4 <handler_ext_int_9+0xc>)
 1da:	2280      	movs	r2, #128	; 0x80
 1dc:	0092      	lsls	r2, r2, #2
 1de:	601a      	str	r2, [r3, #0]
 1e0:	4770      	bx	lr
 1e2:	46c0      	nop			; (mov r8, r8)
 1e4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

000001e8 <handler_ext_int_10>:
 1e8:	4b02      	ldr	r3, [pc, #8]	; (1f4 <handler_ext_int_10+0xc>)
 1ea:	2280      	movs	r2, #128	; 0x80
 1ec:	00d2      	lsls	r2, r2, #3
 1ee:	601a      	str	r2, [r3, #0]
 1f0:	4770      	bx	lr
 1f2:	46c0      	nop			; (mov r8, r8)
 1f4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

000001f8 <handler_ext_int_11>:
 1f8:	4b02      	ldr	r3, [pc, #8]	; (204 <handler_ext_int_11+0xc>)
 1fa:	2280      	movs	r2, #128	; 0x80
 1fc:	0112      	lsls	r2, r2, #4
 1fe:	601a      	str	r2, [r3, #0]
 200:	4770      	bx	lr
 202:	46c0      	nop			; (mov r8, r8)
 204:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

00000208 <handler_ext_int_12>:
 208:	4b02      	ldr	r3, [pc, #8]	; (214 <handler_ext_int_12+0xc>)
 20a:	2280      	movs	r2, #128	; 0x80
 20c:	0152      	lsls	r2, r2, #5
 20e:	601a      	str	r2, [r3, #0]
 210:	4770      	bx	lr
 212:	46c0      	nop			; (mov r8, r8)
 214:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

00000218 <handler_ext_int_13>:
 218:	4b02      	ldr	r3, [pc, #8]	; (224 <handler_ext_int_13+0xc>)
 21a:	2280      	movs	r2, #128	; 0x80
 21c:	0192      	lsls	r2, r2, #6
 21e:	601a      	str	r2, [r3, #0]
 220:	4770      	bx	lr
 222:	46c0      	nop			; (mov r8, r8)
 224:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

00000228 <handler_ext_int_14>:
 228:	4b02      	ldr	r3, [pc, #8]	; (234 <handler_ext_int_14+0xc>)
 22a:	2280      	movs	r2, #128	; 0x80
 22c:	01d2      	lsls	r2, r2, #7
 22e:	601a      	str	r2, [r3, #0]
 230:	4770      	bx	lr
 232:	46c0      	nop			; (mov r8, r8)
 234:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_15:

00000238 <handler_ext_int_15>:
 238:	4b02      	ldr	r3, [pc, #8]	; (244 <handler_ext_int_15+0xc>)
 23a:	2280      	movs	r2, #128	; 0x80
 23c:	0212      	lsls	r2, r2, #8
 23e:	601a      	str	r2, [r3, #0]
 240:	4770      	bx	lr
 242:	46c0      	nop			; (mov r8, r8)
 244:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_16:

00000248 <handler_ext_int_16>:
 248:	4b02      	ldr	r3, [pc, #8]	; (254 <handler_ext_int_16+0xc>)
 24a:	2280      	movs	r2, #128	; 0x80
 24c:	0252      	lsls	r2, r2, #9
 24e:	601a      	str	r2, [r3, #0]
 250:	4770      	bx	lr
 252:	46c0      	nop			; (mov r8, r8)
 254:	e000e280 	.word	0xe000e280

Disassembly of section .text.initialization:

00000258 <initialization>:
 258:	b508      	push	{r3, lr}
 25a:	4a09      	ldr	r2, [pc, #36]	; (280 <initialization+0x28>)
 25c:	4b09      	ldr	r3, [pc, #36]	; (284 <initialization+0x2c>)
 25e:	480a      	ldr	r0, [pc, #40]	; (288 <initialization+0x30>)
 260:	490a      	ldr	r1, [pc, #40]	; (28c <initialization+0x34>)
 262:	601a      	str	r2, [r3, #0]
 264:	f7ff ff40 	bl	e8 <write_regfile>
 268:	4909      	ldr	r1, [pc, #36]	; (290 <initialization+0x38>)
 26a:	20aa      	movs	r0, #170	; 0xaa
 26c:	f7ff ff1c 	bl	a8 <mbus_write_message32>
 270:	f7ff ff4e 	bl	110 <set_halt_until_mbus_rx>
 274:	2004      	movs	r0, #4
 276:	f7ff ff1f 	bl	b8 <mbus_enumerate>
 27a:	f7ff ff59 	bl	130 <set_halt_until_mbus_tx>
 27e:	bd08      	pop	{r3, pc}
 280:	deadbeef 	.word	0xdeadbeef
 284:	0000032c 	.word	0x0000032c
 288:	a0000020 	.word	0xa0000020
 28c:	0000dead 	.word	0x0000dead
 290:	aaaaaaaa 	.word	0xaaaaaaaa

Disassembly of section .text.startup.main:

00000294 <main>:
 294:	b508      	push	{r3, lr}
 296:	f7ff ff5b 	bl	150 <init_interrupt>
 29a:	4b1a      	ldr	r3, [pc, #104]	; (304 <main+0x70>)
 29c:	681a      	ldr	r2, [r3, #0]
 29e:	4b1a      	ldr	r3, [pc, #104]	; (308 <main+0x74>)
 2a0:	429a      	cmp	r2, r3
 2a2:	d001      	beq.n	2a8 <main+0x14>
 2a4:	f7ff ffd8 	bl	258 <initialization>
 2a8:	4918      	ldr	r1, [pc, #96]	; (30c <main+0x78>)
 2aa:	4819      	ldr	r0, [pc, #100]	; (310 <main+0x7c>)
 2ac:	f7ff ff1c 	bl	e8 <write_regfile>
 2b0:	4818      	ldr	r0, [pc, #96]	; (314 <main+0x80>)
 2b2:	f7ff ff11 	bl	d8 <delay>
 2b6:	4918      	ldr	r1, [pc, #96]	; (318 <main+0x84>)
 2b8:	4815      	ldr	r0, [pc, #84]	; (310 <main+0x7c>)
 2ba:	f7ff ff15 	bl	e8 <write_regfile>
 2be:	4815      	ldr	r0, [pc, #84]	; (314 <main+0x80>)
 2c0:	f7ff ff0a 	bl	d8 <delay>
 2c4:	2188      	movs	r1, #136	; 0x88
 2c6:	0089      	lsls	r1, r1, #2
 2c8:	4811      	ldr	r0, [pc, #68]	; (310 <main+0x7c>)
 2ca:	f7ff ff0d 	bl	e8 <write_regfile>
 2ce:	4811      	ldr	r0, [pc, #68]	; (314 <main+0x80>)
 2d0:	f7ff ff02 	bl	d8 <delay>
 2d4:	2184      	movs	r1, #132	; 0x84
 2d6:	0089      	lsls	r1, r1, #2
 2d8:	480d      	ldr	r0, [pc, #52]	; (310 <main+0x7c>)
 2da:	f7ff ff05 	bl	e8 <write_regfile>
 2de:	f7ff ff27 	bl	130 <set_halt_until_mbus_tx>
 2e2:	480e      	ldr	r0, [pc, #56]	; (31c <main+0x88>)
 2e4:	f7ff fef8 	bl	d8 <delay>
 2e8:	20dd      	movs	r0, #221	; 0xdd
 2ea:	490d      	ldr	r1, [pc, #52]	; (320 <main+0x8c>)
 2ec:	f7ff fedc 	bl	a8 <mbus_write_message32>
 2f0:	4b0c      	ldr	r3, [pc, #48]	; (324 <main+0x90>)
 2f2:	22c4      	movs	r2, #196	; 0xc4
 2f4:	0252      	lsls	r2, r2, #9
 2f6:	601a      	str	r2, [r3, #0]
 2f8:	4b0b      	ldr	r3, [pc, #44]	; (328 <main+0x94>)
 2fa:	2201      	movs	r2, #1
 2fc:	601a      	str	r2, [r3, #0]
 2fe:	f7ff fee5 	bl	cc <mbus_sleep_all>
 302:	e7fe      	b.n	302 <main+0x6e>
 304:	0000032c 	.word	0x0000032c
 308:	deadbeef 	.word	0xdeadbeef
 30c:	00000221 	.word	0x00000221
 310:	a0000094 	.word	0xa0000094
 314:	00061a80 	.word	0x00061a80
 318:	00000223 	.word	0x00000223
 31c:	00002710 	.word	0x00002710
 320:	0ea7f00d 	.word	0x0ea7f00d
 324:	a0000098 	.word	0xa0000098
 328:	a0001400 	.word	0xa0001400
