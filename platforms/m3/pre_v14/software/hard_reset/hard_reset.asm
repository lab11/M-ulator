
hard_reset/hard_reset.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0x80>:
   0:	00002000 	.word	0x00002000
   4:	00000083 	.word	0x00000083
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
  40:	00000089 	.word	0x00000089
  44:	00000095 	.word	0x00000095
  48:	000000a1 	.word	0x000000a1
  4c:	000000ad 	.word	0x000000ad
  50:	000000b9 	.word	0x000000b9
  54:	000000c5 	.word	0x000000c5
  58:	000000d1 	.word	0x000000d1
  5c:	000000dd 	.word	0x000000dd
  60:	000000e9 	.word	0x000000e9
  64:	000000f9 	.word	0x000000f9
  68:	00000109 	.word	0x00000109
  6c:	00000119 	.word	0x00000119
  70:	00000129 	.word	0x00000129
  74:	00000139 	.word	0x00000139
  78:	00000149 	.word	0x00000149
  7c:	00000159 	.word	0x00000159

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>

00000082 <_start>:
  82:	f000 f871 	bl	168 <main>
  86:	e7fc      	b.n	82 <_start>

Disassembly of section .text.handler_ext_int_0:

00000088 <handler_ext_int_0>:
  88:	2201      	movs	r2, #1
  8a:	4b01      	ldr	r3, [pc, #4]	; (90 <handler_ext_int_0+0x8>)
  8c:	601a      	str	r2, [r3, #0]
  8e:	4770      	bx	lr
  90:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

00000094 <handler_ext_int_1>:
  94:	2202      	movs	r2, #2
  96:	4b01      	ldr	r3, [pc, #4]	; (9c <handler_ext_int_1+0x8>)
  98:	601a      	str	r2, [r3, #0]
  9a:	4770      	bx	lr
  9c:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

000000a0 <handler_ext_int_2>:
  a0:	2204      	movs	r2, #4
  a2:	4b01      	ldr	r3, [pc, #4]	; (a8 <handler_ext_int_2+0x8>)
  a4:	601a      	str	r2, [r3, #0]
  a6:	4770      	bx	lr
  a8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

000000ac <handler_ext_int_3>:
  ac:	2208      	movs	r2, #8
  ae:	4b01      	ldr	r3, [pc, #4]	; (b4 <handler_ext_int_3+0x8>)
  b0:	601a      	str	r2, [r3, #0]
  b2:	4770      	bx	lr
  b4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

000000b8 <handler_ext_int_4>:
  b8:	2210      	movs	r2, #16
  ba:	4b01      	ldr	r3, [pc, #4]	; (c0 <handler_ext_int_4+0x8>)
  bc:	601a      	str	r2, [r3, #0]
  be:	4770      	bx	lr
  c0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

000000c4 <handler_ext_int_5>:
  c4:	2220      	movs	r2, #32
  c6:	4b01      	ldr	r3, [pc, #4]	; (cc <handler_ext_int_5+0x8>)
  c8:	601a      	str	r2, [r3, #0]
  ca:	4770      	bx	lr
  cc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

000000d0 <handler_ext_int_6>:
  d0:	2240      	movs	r2, #64	; 0x40
  d2:	4b01      	ldr	r3, [pc, #4]	; (d8 <handler_ext_int_6+0x8>)
  d4:	601a      	str	r2, [r3, #0]
  d6:	4770      	bx	lr
  d8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

000000dc <handler_ext_int_7>:
  dc:	2280      	movs	r2, #128	; 0x80
  de:	4b01      	ldr	r3, [pc, #4]	; (e4 <handler_ext_int_7+0x8>)
  e0:	601a      	str	r2, [r3, #0]
  e2:	4770      	bx	lr
  e4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

000000e8 <handler_ext_int_8>:
  e8:	2280      	movs	r2, #128	; 0x80
  ea:	4b02      	ldr	r3, [pc, #8]	; (f4 <handler_ext_int_8+0xc>)
  ec:	0052      	lsls	r2, r2, #1
  ee:	601a      	str	r2, [r3, #0]
  f0:	4770      	bx	lr
  f2:	46c0      	nop			; (mov r8, r8)
  f4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

000000f8 <handler_ext_int_9>:
  f8:	2280      	movs	r2, #128	; 0x80
  fa:	4b02      	ldr	r3, [pc, #8]	; (104 <handler_ext_int_9+0xc>)
  fc:	0092      	lsls	r2, r2, #2
  fe:	601a      	str	r2, [r3, #0]
 100:	4770      	bx	lr
 102:	46c0      	nop			; (mov r8, r8)
 104:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

00000108 <handler_ext_int_10>:
 108:	2280      	movs	r2, #128	; 0x80
 10a:	4b02      	ldr	r3, [pc, #8]	; (114 <handler_ext_int_10+0xc>)
 10c:	00d2      	lsls	r2, r2, #3
 10e:	601a      	str	r2, [r3, #0]
 110:	4770      	bx	lr
 112:	46c0      	nop			; (mov r8, r8)
 114:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

00000118 <handler_ext_int_11>:
 118:	2280      	movs	r2, #128	; 0x80
 11a:	4b02      	ldr	r3, [pc, #8]	; (124 <handler_ext_int_11+0xc>)
 11c:	0112      	lsls	r2, r2, #4
 11e:	601a      	str	r2, [r3, #0]
 120:	4770      	bx	lr
 122:	46c0      	nop			; (mov r8, r8)
 124:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

00000128 <handler_ext_int_12>:
 128:	2280      	movs	r2, #128	; 0x80
 12a:	4b02      	ldr	r3, [pc, #8]	; (134 <handler_ext_int_12+0xc>)
 12c:	0152      	lsls	r2, r2, #5
 12e:	601a      	str	r2, [r3, #0]
 130:	4770      	bx	lr
 132:	46c0      	nop			; (mov r8, r8)
 134:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

00000138 <handler_ext_int_13>:
 138:	2280      	movs	r2, #128	; 0x80
 13a:	4b02      	ldr	r3, [pc, #8]	; (144 <handler_ext_int_13+0xc>)
 13c:	0192      	lsls	r2, r2, #6
 13e:	601a      	str	r2, [r3, #0]
 140:	4770      	bx	lr
 142:	46c0      	nop			; (mov r8, r8)
 144:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

00000148 <handler_ext_int_14>:
 148:	2280      	movs	r2, #128	; 0x80
 14a:	4b02      	ldr	r3, [pc, #8]	; (154 <handler_ext_int_14+0xc>)
 14c:	01d2      	lsls	r2, r2, #7
 14e:	601a      	str	r2, [r3, #0]
 150:	4770      	bx	lr
 152:	46c0      	nop			; (mov r8, r8)
 154:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_15:

00000158 <handler_ext_int_15>:
 158:	2280      	movs	r2, #128	; 0x80
 15a:	4b02      	ldr	r3, [pc, #8]	; (164 <handler_ext_int_15+0xc>)
 15c:	0212      	lsls	r2, r2, #8
 15e:	601a      	str	r2, [r3, #0]
 160:	4770      	bx	lr
 162:	46c0      	nop			; (mov r8, r8)
 164:	e000e280 	.word	0xe000e280

Disassembly of section .text.startup.main:

00000168 <main>:
 168:	2200      	movs	r2, #0
 16a:	4b01      	ldr	r3, [pc, #4]	; (170 <main+0x8>)
 16c:	601a      	str	r2, [r3, #0]
 16e:	e7fe      	b.n	16e <main+0x6>
 170:	a0001204 	.word	0xa0001204
