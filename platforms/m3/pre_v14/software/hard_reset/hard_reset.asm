
hard_reset/hard_reset.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0x90>:
   0:	00002000 	.word	0x00002000
   4:	00000093 	.word	0x00000093
	...
  10:	00000090 	.word	0x00000090
  14:	00000090 	.word	0x00000090
  18:	00000090 	.word	0x00000090
  1c:	00000090 	.word	0x00000090
  20:	00000090 	.word	0x00000090
  24:	00000090 	.word	0x00000090
  28:	00000090 	.word	0x00000090
  2c:	00000000 	.word	0x00000000
  30:	00000090 	.word	0x00000090
  34:	00000090 	.word	0x00000090
	...
  40:	00000099 	.word	0x00000099
  44:	000000a5 	.word	0x000000a5
  48:	000000b1 	.word	0x000000b1
  4c:	000000bd 	.word	0x000000bd
  50:	000000c9 	.word	0x000000c9
  54:	000000d5 	.word	0x000000d5
  58:	000000e1 	.word	0x000000e1
  5c:	000000ed 	.word	0x000000ed
  60:	000000f9 	.word	0x000000f9
  64:	00000109 	.word	0x00000109
  68:	00000119 	.word	0x00000119
  6c:	00000129 	.word	0x00000129
  70:	00000139 	.word	0x00000139
  74:	00000149 	.word	0x00000149
  78:	00000159 	.word	0x00000159
  7c:	00000169 	.word	0x00000169
	...

00000090 <hang>:
  90:	e7fe      	b.n	90 <hang>

00000092 <_start>:
  92:	f000 f871 	bl	178 <main>
  96:	e7fc      	b.n	92 <_start>

Disassembly of section .text.handler_ext_int_0:

00000098 <handler_ext_int_0>:
  98:	2201      	movs	r2, #1
  9a:	4b01      	ldr	r3, [pc, #4]	; (a0 <handler_ext_int_0+0x8>)
  9c:	601a      	str	r2, [r3, #0]
  9e:	4770      	bx	lr
  a0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_1:

000000a4 <handler_ext_int_1>:
  a4:	2202      	movs	r2, #2
  a6:	4b01      	ldr	r3, [pc, #4]	; (ac <handler_ext_int_1+0x8>)
  a8:	601a      	str	r2, [r3, #0]
  aa:	4770      	bx	lr
  ac:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_2:

000000b0 <handler_ext_int_2>:
  b0:	2204      	movs	r2, #4
  b2:	4b01      	ldr	r3, [pc, #4]	; (b8 <handler_ext_int_2+0x8>)
  b4:	601a      	str	r2, [r3, #0]
  b6:	4770      	bx	lr
  b8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_3:

000000bc <handler_ext_int_3>:
  bc:	2208      	movs	r2, #8
  be:	4b01      	ldr	r3, [pc, #4]	; (c4 <handler_ext_int_3+0x8>)
  c0:	601a      	str	r2, [r3, #0]
  c2:	4770      	bx	lr
  c4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_4:

000000c8 <handler_ext_int_4>:
  c8:	2210      	movs	r2, #16
  ca:	4b01      	ldr	r3, [pc, #4]	; (d0 <handler_ext_int_4+0x8>)
  cc:	601a      	str	r2, [r3, #0]
  ce:	4770      	bx	lr
  d0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_5:

000000d4 <handler_ext_int_5>:
  d4:	2220      	movs	r2, #32
  d6:	4b01      	ldr	r3, [pc, #4]	; (dc <handler_ext_int_5+0x8>)
  d8:	601a      	str	r2, [r3, #0]
  da:	4770      	bx	lr
  dc:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_6:

000000e0 <handler_ext_int_6>:
  e0:	2240      	movs	r2, #64	; 0x40
  e2:	4b01      	ldr	r3, [pc, #4]	; (e8 <handler_ext_int_6+0x8>)
  e4:	601a      	str	r2, [r3, #0]
  e6:	4770      	bx	lr
  e8:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_7:

000000ec <handler_ext_int_7>:
  ec:	2280      	movs	r2, #128	; 0x80
  ee:	4b01      	ldr	r3, [pc, #4]	; (f4 <handler_ext_int_7+0x8>)
  f0:	601a      	str	r2, [r3, #0]
  f2:	4770      	bx	lr
  f4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_8:

000000f8 <handler_ext_int_8>:
  f8:	2280      	movs	r2, #128	; 0x80
  fa:	4b02      	ldr	r3, [pc, #8]	; (104 <handler_ext_int_8+0xc>)
  fc:	0052      	lsls	r2, r2, #1
  fe:	601a      	str	r2, [r3, #0]
 100:	4770      	bx	lr
 102:	46c0      	nop			; (mov r8, r8)
 104:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_9:

00000108 <handler_ext_int_9>:
 108:	2280      	movs	r2, #128	; 0x80
 10a:	4b02      	ldr	r3, [pc, #8]	; (114 <handler_ext_int_9+0xc>)
 10c:	0092      	lsls	r2, r2, #2
 10e:	601a      	str	r2, [r3, #0]
 110:	4770      	bx	lr
 112:	46c0      	nop			; (mov r8, r8)
 114:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_10:

00000118 <handler_ext_int_10>:
 118:	2280      	movs	r2, #128	; 0x80
 11a:	4b02      	ldr	r3, [pc, #8]	; (124 <handler_ext_int_10+0xc>)
 11c:	00d2      	lsls	r2, r2, #3
 11e:	601a      	str	r2, [r3, #0]
 120:	4770      	bx	lr
 122:	46c0      	nop			; (mov r8, r8)
 124:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_11:

00000128 <handler_ext_int_11>:
 128:	2280      	movs	r2, #128	; 0x80
 12a:	4b02      	ldr	r3, [pc, #8]	; (134 <handler_ext_int_11+0xc>)
 12c:	0112      	lsls	r2, r2, #4
 12e:	601a      	str	r2, [r3, #0]
 130:	4770      	bx	lr
 132:	46c0      	nop			; (mov r8, r8)
 134:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_12:

00000138 <handler_ext_int_12>:
 138:	2280      	movs	r2, #128	; 0x80
 13a:	4b02      	ldr	r3, [pc, #8]	; (144 <handler_ext_int_12+0xc>)
 13c:	0152      	lsls	r2, r2, #5
 13e:	601a      	str	r2, [r3, #0]
 140:	4770      	bx	lr
 142:	46c0      	nop			; (mov r8, r8)
 144:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_13:

00000148 <handler_ext_int_13>:
 148:	2280      	movs	r2, #128	; 0x80
 14a:	4b02      	ldr	r3, [pc, #8]	; (154 <handler_ext_int_13+0xc>)
 14c:	0192      	lsls	r2, r2, #6
 14e:	601a      	str	r2, [r3, #0]
 150:	4770      	bx	lr
 152:	46c0      	nop			; (mov r8, r8)
 154:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_14:

00000158 <handler_ext_int_14>:
 158:	2280      	movs	r2, #128	; 0x80
 15a:	4b02      	ldr	r3, [pc, #8]	; (164 <handler_ext_int_14+0xc>)
 15c:	01d2      	lsls	r2, r2, #7
 15e:	601a      	str	r2, [r3, #0]
 160:	4770      	bx	lr
 162:	46c0      	nop			; (mov r8, r8)
 164:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_15:

00000168 <handler_ext_int_15>:
 168:	2280      	movs	r2, #128	; 0x80
 16a:	4b02      	ldr	r3, [pc, #8]	; (174 <handler_ext_int_15+0xc>)
 16c:	0212      	lsls	r2, r2, #8
 16e:	601a      	str	r2, [r3, #0]
 170:	4770      	bx	lr
 172:	46c0      	nop			; (mov r8, r8)
 174:	e000e280 	.word	0xe000e280

Disassembly of section .text.startup.main:

00000178 <main>:
 178:	2200      	movs	r2, #0
 17a:	4b01      	ldr	r3, [pc, #4]	; (180 <main+0x8>)
 17c:	601a      	str	r2, [r3, #0]
 17e:	e7fe      	b.n	17e <main+0x6>
 180:	a0001204 	.word	0xa0001204
