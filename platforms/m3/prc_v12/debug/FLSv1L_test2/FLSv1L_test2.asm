
FLSv1L_test2/FLSv1L_test2.elf:     file format elf32-littlearm


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

int main() {
    unsigned i;

    //Initialize Interrupts
    *NVIC_ICPR = 0x7FFF; //Clear All Pending Interrupts
  98:	4a1b      	ldr	r2, [pc, #108]	; (108 <main+0x70>)
  9a:	4b1c      	ldr	r3, [pc, #112]	; (10c <main+0x74>)

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
  9c:	b510      	push	{r4, lr}
    unsigned i;

    //Initialize Interrupts
    *NVIC_ICPR = 0x7FFF; //Clear All Pending Interrupts
  9e:	601a      	str	r2, [r3, #0]
    //*NVIC_ISER = 0x7FFF; //Enable Interrupts
    *NVIC_ISER = 0x0; //Disable all Interrupts
  a0:	4b1b      	ldr	r3, [pc, #108]	; (110 <main+0x78>)
  a2:	2200      	movs	r2, #0
  
    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
  a4:	491b      	ldr	r1, [pc, #108]	; (114 <main+0x7c>)
    unsigned i;

    //Initialize Interrupts
    *NVIC_ICPR = 0x7FFF; //Clear All Pending Interrupts
    //*NVIC_ISER = 0x7FFF; //Enable Interrupts
    *NVIC_ISER = 0x0; //Disable all Interrupts
  a6:	601a      	str	r2, [r3, #0]
  
    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
  a8:	680c      	ldr	r4, [r1, #0]
  aa:	481b      	ldr	r0, [pc, #108]	; (118 <main+0x80>)
  ac:	4b1b      	ldr	r3, [pc, #108]	; (11c <main+0x84>)
  ae:	4284      	cmp	r4, r0
  b0:	d001      	beq.n	b6 <main+0x1e>
        enumerated = 0xDEADBEEF;
  b2:	6008      	str	r0, [r1, #0]
        num_cycle = 0;
  b4:	601a      	str	r2, [r3, #0]
        //*((volatile uint32_t *) 0xA0003000) = 0x24000000; // Enumeration (0x4)
        //*((volatile uint32_t *) 0xA0000028) = 0x0001A000; // Reg0x0A, enable backward-compatibility, halt_until_mbus_tx, disable all other irqs
    }
    
    // cycle number
    *((volatile uint32_t *) 0xA0003EF0) = num_cycle;
  b6:	6819      	ldr	r1, [r3, #0]
  b8:	4a19      	ldr	r2, [pc, #100]	; (120 <main+0x88>)

    // Spits out all mem data
    *MBUS_CMD0 = (0xAA << 24) | (32-1);
    *MBUS_CMD1 = 0x00000000;
    *MBUS_FUID_LEN = 0x23; // CMDLEN=2, MPQ_MEM_READ(3)
  ba:	2023      	movs	r0, #35	; 0x23
        //*((volatile uint32_t *) 0xA0003000) = 0x24000000; // Enumeration (0x4)
        //*((volatile uint32_t *) 0xA0000028) = 0x0001A000; // Reg0x0A, enable backward-compatibility, halt_until_mbus_tx, disable all other irqs
    }
    
    // cycle number
    *((volatile uint32_t *) 0xA0003EF0) = num_cycle;
  bc:	6011      	str	r1, [r2, #0]

    // Spits out all mem data
    *MBUS_CMD0 = (0xAA << 24) | (32-1);
  be:	4919      	ldr	r1, [pc, #100]	; (124 <main+0x8c>)
  c0:	4a19      	ldr	r2, [pc, #100]	; (128 <main+0x90>)
  c2:	6011      	str	r1, [r2, #0]
    *MBUS_CMD1 = 0x00000000;
  c4:	4a19      	ldr	r2, [pc, #100]	; (12c <main+0x94>)
  c6:	2100      	movs	r1, #0
  c8:	6011      	str	r1, [r2, #0]
    *MBUS_FUID_LEN = 0x23; // CMDLEN=2, MPQ_MEM_READ(3)
  ca:	4a19      	ldr	r2, [pc, #100]	; (130 <main+0x98>)
  cc:	6010      	str	r0, [r2, #0]

    num_cycle++;
  ce:	681a      	ldr	r2, [r3, #0]

    // Wakeup Timer
    *((volatile uint32_t *) 0xA0000034) = 0x00018014; // WUP Timer. Sleep-Only. IRQ_EN, Duration=20
  d0:	4818      	ldr	r0, [pc, #96]	; (134 <main+0x9c>)
    // Spits out all mem data
    *MBUS_CMD0 = (0xAA << 24) | (32-1);
    *MBUS_CMD1 = 0x00000000;
    *MBUS_FUID_LEN = 0x23; // CMDLEN=2, MPQ_MEM_READ(3)

    num_cycle++;
  d2:	3201      	adds	r2, #1
  d4:	601a      	str	r2, [r3, #0]

    // Wakeup Timer
    *((volatile uint32_t *) 0xA0000034) = 0x00018014; // WUP Timer. Sleep-Only. IRQ_EN, Duration=20
  d6:	4a18      	ldr	r2, [pc, #96]	; (138 <main+0xa0>)
  d8:	6010      	str	r0, [r2, #0]
    *((volatile uint32_t *) 0xA0001300) = 0x00000001; // WUP Timer Reset
  da:	4a18      	ldr	r2, [pc, #96]	; (13c <main+0xa4>)
  dc:	2001      	movs	r0, #1
  de:	6010      	str	r0, [r2, #0]

    if (num_cycle == 10) {
  e0:	681b      	ldr	r3, [r3, #0]
  e2:	4a17      	ldr	r2, [pc, #92]	; (140 <main+0xa8>)
  e4:	2b0a      	cmp	r3, #10
  e6:	d102      	bne.n	ee <main+0x56>
        *((volatile uint32_t *) 0xA0003FF0) = 0x0EA7F00D; // MBus Msg, 0xFF, 0x0EA7F00D
  e8:	4b16      	ldr	r3, [pc, #88]	; (144 <main+0xac>)
  ea:	6013      	str	r3, [r2, #0]
  ec:	e7fe      	b.n	ec <main+0x54>
        while(1); 
    }
    else {
        // Sleep Message
        *((volatile uint32_t *) 0xA0003010) = 0x00000000;
  ee:	4b16      	ldr	r3, [pc, #88]	; (148 <main+0xb0>)
  f0:	6019      	str	r1, [r3, #0]
    }

    //for (i=0; i < 1000; i++) asm("nop;");
    //Never Quit (should not come here.)
    while(1){
        *((volatile uint32_t *) 0xA0003FF0) = 0xFFFFFFFF; // MBus Msg, 0xFF, 0xFFFFFFFF
  f2:	2101      	movs	r1, #1
  f4:	4249      	negs	r1, r1
  f6:	23fa      	movs	r3, #250	; 0xfa
  f8:	6011      	str	r1, [r2, #0]
  fa:	009b      	lsls	r3, r3, #2
        for (i=0; i < 1000; i++) asm("nop;");
  fc:	46c0      	nop			; (mov r8, r8)
  fe:	3b01      	subs	r3, #1
 100:	2b00      	cmp	r3, #0
 102:	d1fb      	bne.n	fc <main+0x64>
 104:	e7f7      	b.n	f6 <main+0x5e>
 106:	46c0      	nop			; (mov r8, r8)
 108:	00007fff 	.word	0x00007fff
 10c:	e000e280 	.word	0xe000e280
 110:	e000e100 	.word	0xe000e100
 114:	0000014c 	.word	0x0000014c
 118:	deadbeef 	.word	0xdeadbeef
 11c:	00000150 	.word	0x00000150
 120:	a0003ef0 	.word	0xa0003ef0
 124:	aa00001f 	.word	0xaa00001f
 128:	a0002000 	.word	0xa0002000
 12c:	a0002004 	.word	0xa0002004
 130:	a000200c 	.word	0xa000200c
 134:	00018014 	.word	0x00018014
 138:	a0000034 	.word	0xa0000034
 13c:	a0001300 	.word	0xa0001300
 140:	a0003ff0 	.word	0xa0003ff0
 144:	0ea7f00d 	.word	0x0ea7f00d
 148:	a0003010 	.word	0xa0003010
