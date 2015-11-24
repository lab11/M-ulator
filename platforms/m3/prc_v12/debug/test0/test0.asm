
test0/test0.elf:     file format elf32-littlearm


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

00000080 <hang>:
  80:	e7fe      	b.n	80 <hang>
	...

00000090 <_start>:
  90:	f000 f802 	bl	98 <main>
  94:	e7fc      	b.n	90 <_start>

Disassembly of section .text.startup.main:

00000098 <main>:
  98:	4a24      	ldr	r2, [pc, #144]	; (12c <main+0x94>)
  9a:	4b25      	ldr	r3, [pc, #148]	; (130 <main+0x98>)
  9c:	b530      	push	{r4, r5, lr}
  9e:	601a      	str	r2, [r3, #0]
  a0:	4a24      	ldr	r2, [pc, #144]	; (134 <main+0x9c>)
  a2:	2300      	movs	r3, #0
  a4:	6013      	str	r3, [r2, #0]
  a6:	4a24      	ldr	r2, [pc, #144]	; (138 <main+0xa0>)
  a8:	4924      	ldr	r1, [pc, #144]	; (13c <main+0xa4>)
  aa:	6810      	ldr	r0, [r2, #0]
  ac:	4288      	cmp	r0, r1
  ae:	d012      	beq.n	d6 <main+0x3e>
  b0:	6011      	str	r1, [r2, #0]
  b2:	4a23      	ldr	r2, [pc, #140]	; (140 <main+0xa8>)
  b4:	2190      	movs	r1, #144	; 0x90
  b6:	6013      	str	r3, [r2, #0]
  b8:	4a22      	ldr	r2, [pc, #136]	; (144 <main+0xac>)
  ba:	23f8      	movs	r3, #248	; 0xf8
  bc:	025b      	lsls	r3, r3, #9
  be:	6013      	str	r3, [r2, #0]
  c0:	4b21      	ldr	r3, [pc, #132]	; (148 <main+0xb0>)
  c2:	0589      	lsls	r1, r1, #22
  c4:	6019      	str	r1, [r3, #0]
  c6:	4b21      	ldr	r3, [pc, #132]	; (14c <main+0xb4>)
  c8:	46c0      	nop			; (mov r8, r8)
  ca:	3b01      	subs	r3, #1
  cc:	2b00      	cmp	r3, #0
  ce:	d1fb      	bne.n	c8 <main+0x30>
  d0:	23d0      	movs	r3, #208	; 0xd0
  d2:	025b      	lsls	r3, r3, #9
  d4:	6013      	str	r3, [r2, #0]
  d6:	4a1a      	ldr	r2, [pc, #104]	; (140 <main+0xa8>)
  d8:	4b1d      	ldr	r3, [pc, #116]	; (150 <main+0xb8>)
  da:	6811      	ldr	r1, [r2, #0]
  dc:	4c1d      	ldr	r4, [pc, #116]	; (154 <main+0xbc>)
  de:	6019      	str	r1, [r3, #0]
  e0:	4b1d      	ldr	r3, [pc, #116]	; (158 <main+0xc0>)
  e2:	481e      	ldr	r0, [pc, #120]	; (15c <main+0xc4>)
  e4:	6023      	str	r3, [r4, #0]
  e6:	2300      	movs	r3, #0
  e8:	6003      	str	r3, [r0, #0]
  ea:	4d1d      	ldr	r5, [pc, #116]	; (160 <main+0xc8>)
  ec:	4b1d      	ldr	r3, [pc, #116]	; (164 <main+0xcc>)
  ee:	2123      	movs	r1, #35	; 0x23
  f0:	6019      	str	r1, [r3, #0]
  f2:	6025      	str	r5, [r4, #0]
  f4:	24c8      	movs	r4, #200	; 0xc8
  f6:	6004      	str	r4, [r0, #0]
  f8:	6019      	str	r1, [r3, #0]
  fa:	6813      	ldr	r3, [r2, #0]
  fc:	3301      	adds	r3, #1
  fe:	6013      	str	r3, [r2, #0]
 100:	4b12      	ldr	r3, [pc, #72]	; (14c <main+0xb4>)
 102:	46c0      	nop			; (mov r8, r8)
 104:	3b01      	subs	r3, #1
 106:	2b00      	cmp	r3, #0
 108:	d1fb      	bne.n	102 <main+0x6a>
 10a:	4817      	ldr	r0, [pc, #92]	; (168 <main+0xd0>)
 10c:	4917      	ldr	r1, [pc, #92]	; (16c <main+0xd4>)
 10e:	6008      	str	r0, [r1, #0]
 110:	4917      	ldr	r1, [pc, #92]	; (170 <main+0xd8>)
 112:	2001      	movs	r0, #1
 114:	6008      	str	r0, [r1, #0]
 116:	6812      	ldr	r2, [r2, #0]
 118:	2a0a      	cmp	r2, #10
 11a:	d103      	bne.n	124 <main+0x8c>
 11c:	4a15      	ldr	r2, [pc, #84]	; (174 <main+0xdc>)
 11e:	4b16      	ldr	r3, [pc, #88]	; (178 <main+0xe0>)
 120:	601a      	str	r2, [r3, #0]
 122:	e7fe      	b.n	122 <main+0x8a>
 124:	4a15      	ldr	r2, [pc, #84]	; (17c <main+0xe4>)
 126:	6013      	str	r3, [r2, #0]
 128:	e7fe      	b.n	128 <main+0x90>
 12a:	46c0      	nop			; (mov r8, r8)
 12c:	00007fff 	.word	0x00007fff
 130:	e000e280 	.word	0xe000e280
 134:	e000e100 	.word	0xe000e100
 138:	00000180 	.word	0x00000180
 13c:	deadbeef 	.word	0xdeadbeef
 140:	00000184 	.word	0x00000184
 144:	a0000028 	.word	0xa0000028
 148:	a0003000 	.word	0xa0003000
 14c:	00002710 	.word	0x00002710
 150:	a0003ef0 	.word	0xa0003ef0
 154:	a0002000 	.word	0xa0002000
 158:	aa000032 	.word	0xaa000032
 15c:	a0002004 	.word	0xa0002004
 160:	ab000032 	.word	0xab000032
 164:	a000200c 	.word	0xa000200c
 168:	0001800a 	.word	0x0001800a
 16c:	a0000034 	.word	0xa0000034
 170:	a0001300 	.word	0xa0001300
 174:	0ea7f00d 	.word	0x0ea7f00d
 178:	a0003bb0 	.word	0xa0003bb0
 17c:	a0003010 	.word	0xa0003010
