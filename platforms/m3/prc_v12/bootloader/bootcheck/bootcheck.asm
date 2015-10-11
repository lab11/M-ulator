
bootcheck/bootcheck.elf:     file format elf32-littlearm


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
  40:	0000011d 	.word	0x0000011d
  44:	00000129 	.word	0x00000129
  48:	00000135 	.word	0x00000135
  4c:	00000141 	.word	0x00000141
  50:	0000014d 	.word	0x0000014d
  54:	00000159 	.word	0x00000159
  58:	00000165 	.word	0x00000165
  5c:	00000171 	.word	0x00000171
  60:	0000017d 	.word	0x0000017d
  64:	0000018d 	.word	0x0000018d
  68:	0000019d 	.word	0x0000019d
  6c:	000001ad 	.word	0x000001ad
  70:	000001bd 	.word	0x000001bd
  74:	000001cd 	.word	0x000001cd
  78:	000001dd 	.word	0x000001dd
  7c:	00000000 	.word	0x00000000

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 f8ca 	bl	228 <main>
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

Disassembly of section .text.write_regfile:

000000a8 <write_regfile>:
  a8:	0880      	lsrs	r0, r0, #2
  aa:	0209      	lsls	r1, r1, #8
  ac:	b508      	push	{r3, lr}
  ae:	0a09      	lsrs	r1, r1, #8
  b0:	4b05      	ldr	r3, [pc, #20]	; (c8 <write_regfile+0x20>)
  b2:	0600      	lsls	r0, r0, #24
  b4:	4308      	orrs	r0, r1
  b6:	6018      	str	r0, [r3, #0]
  b8:	4b04      	ldr	r3, [pc, #16]	; (cc <write_regfile+0x24>)
  ba:	2210      	movs	r2, #16
  bc:	601a      	str	r2, [r3, #0]
  be:	200a      	movs	r0, #10
  c0:	f7ff ffe9 	bl	96 <delay>
  c4:	bd08      	pop	{r3, pc}
  c6:	46c0      	nop			; (mov r8, r8)
  c8:	a0002000 	.word	0xa0002000
  cc:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_write_message32:

000000d0 <mbus_write_message32>:
  d0:	4b02      	ldr	r3, [pc, #8]	; (dc <mbus_write_message32+0xc>)
  d2:	0100      	lsls	r0, r0, #4
  d4:	4318      	orrs	r0, r3
  d6:	6001      	str	r1, [r0, #0]
  d8:	2001      	movs	r0, #1
  da:	4770      	bx	lr
  dc:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_enumerate:

000000e0 <mbus_enumerate>:
  e0:	0603      	lsls	r3, r0, #24
  e2:	2080      	movs	r0, #128	; 0x80
  e4:	0580      	lsls	r0, r0, #22
  e6:	4318      	orrs	r0, r3
  e8:	4b01      	ldr	r3, [pc, #4]	; (f0 <mbus_enumerate+0x10>)
  ea:	6018      	str	r0, [r3, #0]
  ec:	4770      	bx	lr
  ee:	46c0      	nop			; (mov r8, r8)
  f0:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

000000f4 <mbus_sleep_all>:
  f4:	4b01      	ldr	r3, [pc, #4]	; (fc <mbus_sleep_all+0x8>)
  f6:	2200      	movs	r2, #0
  f8:	601a      	str	r2, [r3, #0]
  fa:	4770      	bx	lr
  fc:	a0003010 	.word	0xa0003010

Disassembly of section .text.init_interrupt:

00000100 <init_interrupt>:
 100:	4a03      	ldr	r2, [pc, #12]	; (110 <init_interrupt+0x10>)
 102:	4b04      	ldr	r3, [pc, #16]	; (114 <init_interrupt+0x14>)
 104:	601a      	str	r2, [r3, #0]
 106:	4b04      	ldr	r3, [pc, #16]	; (118 <init_interrupt+0x18>)
 108:	2200      	movs	r2, #0
 10a:	601a      	str	r2, [r3, #0]
 10c:	4770      	bx	lr
 10e:	46c0      	nop			; (mov r8, r8)
 110:	00007fff 	.word	0x00007fff
 114:	e000e280 	.word	0xe000e280
 118:	e000e100 	.word	0xe000e100

Disassembly of section .text.handler_ext_int_0:

0000011c <handler_ext_int_0>:
 11c:	4b01      	ldr	r3, [pc, #4]	; (124 <handler_ext_int_0+0x8>)
 11e:	2201      	movs	r2, #1
 120:	601a      	str	r2, [r3, #0]
 122:	4770      	bx	lr
 124:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

00000128 <handler_ext_int_1>:
 128:	4b01      	ldr	r3, [pc, #4]	; (130 <handler_ext_int_1+0x8>)
 12a:	2202      	movs	r2, #2
 12c:	601a      	str	r2, [r3, #0]
 12e:	4770      	bx	lr
 130:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

00000134 <handler_ext_int_2>:
 134:	4b01      	ldr	r3, [pc, #4]	; (13c <handler_ext_int_2+0x8>)
 136:	2204      	movs	r2, #4
 138:	601a      	str	r2, [r3, #0]
 13a:	4770      	bx	lr
 13c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

00000140 <handler_ext_int_3>:
 140:	4b01      	ldr	r3, [pc, #4]	; (148 <handler_ext_int_3+0x8>)
 142:	2208      	movs	r2, #8
 144:	601a      	str	r2, [r3, #0]
 146:	4770      	bx	lr
 148:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

0000014c <handler_ext_int_4>:
 14c:	4b01      	ldr	r3, [pc, #4]	; (154 <handler_ext_int_4+0x8>)
 14e:	2210      	movs	r2, #16
 150:	601a      	str	r2, [r3, #0]
 152:	4770      	bx	lr
 154:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

00000158 <handler_ext_int_5>:
 158:	4b01      	ldr	r3, [pc, #4]	; (160 <handler_ext_int_5+0x8>)
 15a:	2220      	movs	r2, #32
 15c:	601a      	str	r2, [r3, #0]
 15e:	4770      	bx	lr
 160:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

00000164 <handler_ext_int_6>:
 164:	4b01      	ldr	r3, [pc, #4]	; (16c <handler_ext_int_6+0x8>)
 166:	2240      	movs	r2, #64	; 0x40
 168:	601a      	str	r2, [r3, #0]
 16a:	4770      	bx	lr
 16c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

00000170 <handler_ext_int_7>:
 170:	4b01      	ldr	r3, [pc, #4]	; (178 <handler_ext_int_7+0x8>)
 172:	2280      	movs	r2, #128	; 0x80
 174:	601a      	str	r2, [r3, #0]
 176:	4770      	bx	lr
 178:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

0000017c <handler_ext_int_8>:
 17c:	4b02      	ldr	r3, [pc, #8]	; (188 <handler_ext_int_8+0xc>)
 17e:	2280      	movs	r2, #128	; 0x80
 180:	0052      	lsls	r2, r2, #1
 182:	601a      	str	r2, [r3, #0]
 184:	4770      	bx	lr
 186:	46c0      	nop			; (mov r8, r8)
 188:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

0000018c <handler_ext_int_9>:
 18c:	4b02      	ldr	r3, [pc, #8]	; (198 <handler_ext_int_9+0xc>)
 18e:	2280      	movs	r2, #128	; 0x80
 190:	0092      	lsls	r2, r2, #2
 192:	601a      	str	r2, [r3, #0]
 194:	4770      	bx	lr
 196:	46c0      	nop			; (mov r8, r8)
 198:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

0000019c <handler_ext_int_10>:
 19c:	4b02      	ldr	r3, [pc, #8]	; (1a8 <handler_ext_int_10+0xc>)
 19e:	2280      	movs	r2, #128	; 0x80
 1a0:	00d2      	lsls	r2, r2, #3
 1a2:	601a      	str	r2, [r3, #0]
 1a4:	4770      	bx	lr
 1a6:	46c0      	nop			; (mov r8, r8)
 1a8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

000001ac <handler_ext_int_11>:
 1ac:	4b02      	ldr	r3, [pc, #8]	; (1b8 <handler_ext_int_11+0xc>)
 1ae:	2280      	movs	r2, #128	; 0x80
 1b0:	0112      	lsls	r2, r2, #4
 1b2:	601a      	str	r2, [r3, #0]
 1b4:	4770      	bx	lr
 1b6:	46c0      	nop			; (mov r8, r8)
 1b8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

000001bc <handler_ext_int_12>:
 1bc:	4b02      	ldr	r3, [pc, #8]	; (1c8 <handler_ext_int_12+0xc>)
 1be:	2280      	movs	r2, #128	; 0x80
 1c0:	0152      	lsls	r2, r2, #5
 1c2:	601a      	str	r2, [r3, #0]
 1c4:	4770      	bx	lr
 1c6:	46c0      	nop			; (mov r8, r8)
 1c8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

000001cc <handler_ext_int_13>:
 1cc:	4b02      	ldr	r3, [pc, #8]	; (1d8 <handler_ext_int_13+0xc>)
 1ce:	2280      	movs	r2, #128	; 0x80
 1d0:	0192      	lsls	r2, r2, #6
 1d2:	601a      	str	r2, [r3, #0]
 1d4:	4770      	bx	lr
 1d6:	46c0      	nop			; (mov r8, r8)
 1d8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

000001dc <handler_ext_int_14>:
 1dc:	4b02      	ldr	r3, [pc, #8]	; (1e8 <handler_ext_int_14+0xc>)
 1de:	2280      	movs	r2, #128	; 0x80
 1e0:	01d2      	lsls	r2, r2, #7
 1e2:	601a      	str	r2, [r3, #0]
 1e4:	4770      	bx	lr
 1e6:	46c0      	nop			; (mov r8, r8)
 1e8:	e000e280 	.word	0xe000e280

Disassembly of section .text.initialization:

000001ec <initialization>:
 1ec:	b508      	push	{r3, lr}
 1ee:	4a09      	ldr	r2, [pc, #36]	; (214 <initialization+0x28>)
 1f0:	4b09      	ldr	r3, [pc, #36]	; (218 <initialization+0x2c>)
 1f2:	480a      	ldr	r0, [pc, #40]	; (21c <initialization+0x30>)
 1f4:	490a      	ldr	r1, [pc, #40]	; (220 <initialization+0x34>)
 1f6:	601a      	str	r2, [r3, #0]
 1f8:	f7ff ff56 	bl	a8 <write_regfile>
 1fc:	4909      	ldr	r1, [pc, #36]	; (224 <initialization+0x38>)
 1fe:	20aa      	movs	r0, #170	; 0xaa
 200:	f7ff ff66 	bl	d0 <mbus_write_message32>
 204:	2004      	movs	r0, #4
 206:	f7ff ff6b 	bl	e0 <mbus_enumerate>
 20a:	20fa      	movs	r0, #250	; 0xfa
 20c:	0080      	lsls	r0, r0, #2
 20e:	f7ff ff42 	bl	96 <delay>
 212:	bd08      	pop	{r3, pc}
 214:	deadbeef 	.word	0xdeadbeef
 218:	0000026c 	.word	0x0000026c
 21c:	a0000020 	.word	0xa0000020
 220:	0000dead 	.word	0x0000dead
 224:	aaaaaaaa 	.word	0xaaaaaaaa

Disassembly of section .text.startup.main:

00000228 <main>:
 228:	b508      	push	{r3, lr}
 22a:	f7ff ff69 	bl	100 <init_interrupt>
 22e:	4b0b      	ldr	r3, [pc, #44]	; (25c <main+0x34>)
 230:	681a      	ldr	r2, [r3, #0]
 232:	4b0b      	ldr	r3, [pc, #44]	; (260 <main+0x38>)
 234:	429a      	cmp	r2, r3
 236:	d001      	beq.n	23c <main+0x14>
 238:	f7ff ffd8 	bl	1ec <initialization>
 23c:	4909      	ldr	r1, [pc, #36]	; (264 <main+0x3c>)
 23e:	2041      	movs	r0, #65	; 0x41
 240:	f7ff ff46 	bl	d0 <mbus_write_message32>
 244:	20fa      	movs	r0, #250	; 0xfa
 246:	00c0      	lsls	r0, r0, #3
 248:	f7ff ff25 	bl	96 <delay>
 24c:	4906      	ldr	r1, [pc, #24]	; (268 <main+0x40>)
 24e:	20dd      	movs	r0, #221	; 0xdd
 250:	f7ff ff3e 	bl	d0 <mbus_write_message32>
 254:	f7ff ff4e 	bl	f4 <mbus_sleep_all>
 258:	e7fe      	b.n	258 <main+0x30>
 25a:	46c0      	nop			; (mov r8, r8)
 25c:	0000026c 	.word	0x0000026c
 260:	deadbeef 	.word	0xdeadbeef
 264:	0e001000 	.word	0x0e001000
 268:	0ea7f00d 	.word	0x0ea7f00d
