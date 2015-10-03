
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
  98:	4a21      	ldr	r2, [pc, #132]	; (120 <main+0x88>)
  9a:	4b22      	ldr	r3, [pc, #136]	; (124 <main+0x8c>)
  9c:	b530      	push	{r4, r5, lr}
  9e:	601a      	str	r2, [r3, #0]
  a0:	4a21      	ldr	r2, [pc, #132]	; (128 <main+0x90>)
  a2:	2300      	movs	r3, #0
  a4:	6013      	str	r3, [r2, #0]
  a6:	4a21      	ldr	r2, [pc, #132]	; (12c <main+0x94>)
  a8:	4921      	ldr	r1, [pc, #132]	; (130 <main+0x98>)
  aa:	6810      	ldr	r0, [r2, #0]
  ac:	4288      	cmp	r0, r1
  ae:	d00d      	beq.n	cc <main+0x34>
  b0:	6011      	str	r1, [r2, #0]
  b2:	4a20      	ldr	r2, [pc, #128]	; (134 <main+0x9c>)
  b4:	2190      	movs	r1, #144	; 0x90
  b6:	6013      	str	r3, [r2, #0]
  b8:	4b1f      	ldr	r3, [pc, #124]	; (138 <main+0xa0>)
  ba:	22c8      	movs	r2, #200	; 0xc8
  bc:	0252      	lsls	r2, r2, #9
  be:	601a      	str	r2, [r3, #0]
  c0:	4a1e      	ldr	r2, [pc, #120]	; (13c <main+0xa4>)
  c2:	0589      	lsls	r1, r1, #22
  c4:	6011      	str	r1, [r2, #0]
  c6:	22d0      	movs	r2, #208	; 0xd0
  c8:	0252      	lsls	r2, r2, #9
  ca:	601a      	str	r2, [r3, #0]
  cc:	4a19      	ldr	r2, [pc, #100]	; (134 <main+0x9c>)
  ce:	4b1c      	ldr	r3, [pc, #112]	; (140 <main+0xa8>)
  d0:	6811      	ldr	r1, [r2, #0]
  d2:	4c1c      	ldr	r4, [pc, #112]	; (144 <main+0xac>)
  d4:	6019      	str	r1, [r3, #0]
  d6:	4b1c      	ldr	r3, [pc, #112]	; (148 <main+0xb0>)
  d8:	481c      	ldr	r0, [pc, #112]	; (14c <main+0xb4>)
  da:	6023      	str	r3, [r4, #0]
  dc:	2300      	movs	r3, #0
  de:	6003      	str	r3, [r0, #0]
  e0:	4d1b      	ldr	r5, [pc, #108]	; (150 <main+0xb8>)
  e2:	4b1c      	ldr	r3, [pc, #112]	; (154 <main+0xbc>)
  e4:	2123      	movs	r1, #35	; 0x23
  e6:	6019      	str	r1, [r3, #0]
  e8:	6025      	str	r5, [r4, #0]
  ea:	24c8      	movs	r4, #200	; 0xc8
  ec:	6004      	str	r4, [r0, #0]
  ee:	6019      	str	r1, [r3, #0]
  f0:	6813      	ldr	r3, [r2, #0]
  f2:	3301      	adds	r3, #1
  f4:	6013      	str	r3, [r2, #0]
  f6:	4b18      	ldr	r3, [pc, #96]	; (158 <main+0xc0>)
  f8:	46c0      	nop			; (mov r8, r8)
  fa:	3b01      	subs	r3, #1
  fc:	2b00      	cmp	r3, #0
  fe:	d1fb      	bne.n	f8 <main+0x60>
 100:	4816      	ldr	r0, [pc, #88]	; (15c <main+0xc4>)
 102:	4917      	ldr	r1, [pc, #92]	; (160 <main+0xc8>)
 104:	6008      	str	r0, [r1, #0]
 106:	4917      	ldr	r1, [pc, #92]	; (164 <main+0xcc>)
 108:	2001      	movs	r0, #1
 10a:	6008      	str	r0, [r1, #0]
 10c:	6812      	ldr	r2, [r2, #0]
 10e:	2a0a      	cmp	r2, #10
 110:	d103      	bne.n	11a <main+0x82>
 112:	4a15      	ldr	r2, [pc, #84]	; (168 <main+0xd0>)
 114:	4b15      	ldr	r3, [pc, #84]	; (16c <main+0xd4>)
 116:	601a      	str	r2, [r3, #0]
 118:	e7fe      	b.n	118 <main+0x80>
 11a:	4a15      	ldr	r2, [pc, #84]	; (170 <main+0xd8>)
 11c:	6013      	str	r3, [r2, #0]
 11e:	e7fe      	b.n	11e <main+0x86>
 120:	00007fff 	.word	0x00007fff
 124:	e000e280 	.word	0xe000e280
 128:	e000e100 	.word	0xe000e100
 12c:	00000174 	.word	0x00000174
 130:	deadbeef 	.word	0xdeadbeef
 134:	00000178 	.word	0x00000178
 138:	a0000028 	.word	0xa0000028
 13c:	a0003000 	.word	0xa0003000
 140:	a0003ef0 	.word	0xa0003ef0
 144:	a0002000 	.word	0xa0002000
 148:	aa000032 	.word	0xaa000032
 14c:	a0002004 	.word	0xa0002004
 150:	ab000032 	.word	0xab000032
 154:	a000200c 	.word	0xa000200c
 158:	00002710 	.word	0x00002710
 15c:	0001800a 	.word	0x0001800a
 160:	a0000034 	.word	0xa0000034
 164:	a0001300 	.word	0xa0001300
 168:	0ea7f00d 	.word	0x0ea7f00d
 16c:	a0003bb0 	.word	0xa0003bb0
 170:	a0003010 	.word	0xa0003010
