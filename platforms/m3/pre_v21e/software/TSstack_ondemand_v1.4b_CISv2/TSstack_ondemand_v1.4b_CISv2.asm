
TSstack_ondemand_v1.4b_CISv2/TSstack_ondemand_v1.4b_CISv2.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0xa0>:
   0:	00004000 	andeq	r4, r0, r0
   4:	000000a3 	andeq	r0, r0, r3, lsr #1
	...
  10:	000000a0 	andeq	r0, r0, r0, lsr #1
  14:	000000a0 	andeq	r0, r0, r0, lsr #1
  18:	000000a0 	andeq	r0, r0, r0, lsr #1
  1c:	000000a0 	andeq	r0, r0, r0, lsr #1
  20:	000000a0 	andeq	r0, r0, r0, lsr #1
  24:	000000a0 	andeq	r0, r0, r0, lsr #1
  28:	000000a0 	andeq	r0, r0, r0, lsr #1
  2c:	00000000 	andeq	r0, r0, r0
  30:	000000a0 	andeq	r0, r0, r0, lsr #1
  34:	000000a0 	andeq	r0, r0, r0, lsr #1
	...
  40:	00000109 	andeq	r0, r0, r9, lsl #2
  44:	00000000 	andeq	r0, r0, r0
  48:	000000e5 	andeq	r0, r0, r5, ror #1
  4c:	000000fd 	strdeq	r0, [r0], -sp
	...

000000a0 <hang>:
  a0:	e7fe      	b.n	a0 <hang>

000000a2 <_start>:
  a2:	f000 f855 	bl	150 <main>
  a6:	e7fc      	b.n	a2 <_start>

Disassembly of section .text.set_halt_until_mbus_tx:

000000a8 <set_halt_until_mbus_tx>:
// MBUS IRQ SETTING
//**************************************************
void set_halt_until_reg(uint32_t reg_id) { *SREG_CONF_HALT = reg_id; }
void set_halt_until_mem_wr(void) { *SREG_CONF_HALT = HALT_UNTIL_MEM_WR; }
void set_halt_until_mbus_rx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_RX; }
void set_halt_until_mbus_tx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_TX; }
  a8:	2209      	movs	r2, #9
  aa:	4b01      	ldr	r3, [pc, #4]	; (b0 <set_halt_until_mbus_tx+0x8>)
  ac:	601a      	str	r2, [r3, #0]
  ae:	4770      	bx	lr
  b0:	a000a000 	.word	0xa000a000

Disassembly of section .text.set_halt_until_mbus_trx:

000000b4 <set_halt_until_mbus_trx>:
void set_halt_until_mbus_trx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_TRX; }
  b4:	220c      	movs	r2, #12
  b6:	4b01      	ldr	r3, [pc, #4]	; (bc <set_halt_until_mbus_trx+0x8>)
  b8:	601a      	str	r2, [r3, #0]
  ba:	4770      	bx	lr
  bc:	a000a000 	.word	0xa000a000

Disassembly of section .text.mbus_write_message32:

000000c0 <mbus_write_message32>:
	// TODO: Read from LC
	return 1;
}

uint32_t mbus_write_message32(uint8_t addr, uint32_t data) {
    uint32_t mbus_addr = 0xA0003000 | (addr << 4);
  c0:	0103      	lsls	r3, r0, #4
  c2:	4802      	ldr	r0, [pc, #8]	; (cc <mbus_write_message32+0xc>)
  c4:	4318      	orrs	r0, r3
    *((volatile uint32_t *) mbus_addr) = data;
  c6:	6001      	str	r1, [r0, #0]
    return 1;
}
  c8:	2001      	movs	r0, #1
  ca:	4770      	bx	lr
  cc:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_enumerate:

000000d0 <mbus_enumerate>:

    return 1;
}

void mbus_enumerate(unsigned new_prefix) {
    mbus_write_message32(MBUS_DISC_AND_ENUM, ((MBUS_ENUMERATE_CMD << 28) | (new_prefix << 24)));
  d0:	0603      	lsls	r3, r0, #24
  d2:	2080      	movs	r0, #128	; 0x80
  d4:	0580      	lsls	r0, r0, #22
  d6:	4318      	orrs	r0, r3
    *((volatile uint32_t *) mbus_addr) = data;
  d8:	4b01      	ldr	r3, [pc, #4]	; (e0 <mbus_enumerate+0x10>)
  da:	6018      	str	r0, [r3, #0]
}
  dc:	4770      	bx	lr
  de:	46c0      	nop			; (mov r8, r8)
  e0:	a0003000 	.word	0xa0003000

Disassembly of section .text.handler_ext_int_gocep:

000000e4 <handler_ext_int_gocep>:
void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_gocep(void) { 
    *NVIC_ICPR = (0x1 << IRQ_GOCEP); 
  e4:	2304      	movs	r3, #4
  e6:	4a03      	ldr	r2, [pc, #12]	; (f4 <handler_ext_int_gocep+0x10>)
  e8:	6013      	str	r3, [r2, #0]
    irq_pending.gocep = 1;
  ea:	4a03      	ldr	r2, [pc, #12]	; (f8 <handler_ext_int_gocep+0x14>)
  ec:	6811      	ldr	r1, [r2, #0]
  ee:	430b      	orrs	r3, r1
  f0:	6013      	str	r3, [r2, #0]
}
  f2:	4770      	bx	lr
  f4:	e000e280 	.word	0xe000e280
  f8:	000001c4 	.word	0x000001c4

Disassembly of section .text.handler_ext_int_timer32:

000000fc <handler_ext_int_timer32>:

void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
  fc:	2208      	movs	r2, #8
  fe:	4b01      	ldr	r3, [pc, #4]	; (104 <handler_ext_int_timer32+0x8>)
 100:	601a      	str	r2, [r3, #0]
//    *REG1 = *TIMER32_CNT;
//    *REG2 = *TIMER32_STAT;
//    *TIMER32_STAT = 0x0;
//	wfi_timeout_flag = 1;
//    set_halt_until_mbus_tx();
    }
 102:	4770      	bx	lr
 104:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_wakeup:

00000108 <handler_ext_int_wakeup>:
//[ 4] = mbus message
//[ 8] = gpio[0]
//[ 9] = gpio[1]
//[10] = gpio[2]
//[11] = gpio[3]
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
 108:	2301      	movs	r3, #1
 10a:	4a03      	ldr	r2, [pc, #12]	; (118 <handler_ext_int_wakeup+0x10>)
 10c:	6013      	str	r3, [r2, #0]
    irq_pending.wakeup = 1;
 10e:	4a03      	ldr	r2, [pc, #12]	; (11c <handler_ext_int_wakeup+0x14>)
 110:	6811      	ldr	r1, [r2, #0]
 112:	430b      	orrs	r3, r1
 114:	6013      	str	r3, [r2, #0]
//    for (uint32_t i=0; i<65; i++) {
//        uint32_t temp = *(volatile uint32_t *) (0xA0000000 | (i<<2));
//	    mbus_write_message32(0xAB, (i << 24) | (temp & 0x00FFFFFF));
//    }
//
}
 116:	4770      	bx	lr
 118:	e000e280 	.word	0xe000e280
 11c:	000001c4 	.word	0x000001c4

Disassembly of section .text.operation_init:

00000120 <operation_init>:


// Temporary operation_init
void operation_init(void){
 120:	b510      	push	{r4, lr}
	mbus_write_message32(0xAF, 0x00000003);
 122:	2103      	movs	r1, #3
 124:	20af      	movs	r0, #175	; 0xaf
 126:	f7ff ffcb 	bl	c0 <mbus_write_message32>

    enumerated = 0x5453104b;
 12a:	4b07      	ldr	r3, [pc, #28]	; (148 <operation_init+0x28>)
 12c:	4a07      	ldr	r2, [pc, #28]	; (14c <operation_init+0x2c>)
 12e:	601a      	str	r2, [r3, #0]

    set_halt_until_mbus_trx();
 130:	f7ff ffc0 	bl	b4 <set_halt_until_mbus_trx>
    mbus_enumerate(MEM_ADDR);
 134:	2007      	movs	r0, #7
 136:	f7ff ffcb 	bl	d0 <mbus_enumerate>
    set_halt_until_mbus_tx();
 13a:	f7ff ffb5 	bl	a8 <set_halt_until_mbus_tx>

	mbus_write_message32(0xAF, 0x00000004);
 13e:	2104      	movs	r1, #4
 140:	20af      	movs	r0, #175	; 0xaf
 142:	f7ff ffbd 	bl	c0 <mbus_write_message32>
}
 146:	bd10      	pop	{r4, pc}
 148:	000001c8 	.word	0x000001c8
 14c:	5453104b 	.word	0x5453104b

Disassembly of section .text.startup.main:

00000150 <main>:

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(){
 150:	b570      	push	{r4, r5, r6, lr}
	mbus_write_message32(0xAF, 0x00000000);
 152:	2100      	movs	r1, #0
 154:	20af      	movs	r0, #175	; 0xaf
 156:	f7ff ffb3 	bl	c0 <mbus_write_message32>
	mbus_write_message32(0xAF, irq_pending.as_int);
 15a:	4c16      	ldr	r4, [pc, #88]	; (1b4 <main+0x64>)
 15c:	20af      	movs	r0, #175	; 0xaf
 15e:	6821      	ldr	r1, [r4, #0]
 160:	f7ff ffae 	bl	c0 <mbus_write_message32>

	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP);
 164:	2205      	movs	r2, #5
 166:	4b14      	ldr	r3, [pc, #80]	; (1b8 <main+0x68>)
 168:	601a      	str	r2, [r3, #0]
        
    if (enumerated != 0x5453104b){
 16a:	4b14      	ldr	r3, [pc, #80]	; (1bc <main+0x6c>)
 16c:	681a      	ldr	r2, [r3, #0]
 16e:	4b14      	ldr	r3, [pc, #80]	; (1c0 <main+0x70>)
 170:	429a      	cmp	r2, r3
 172:	d001      	beq.n	178 <main+0x28>
        operation_init();
 174:	f7ff ffd4 	bl	120 <operation_init>
    }

	mbus_write_message32(0xAF, 0xFFFFFFFF);
 178:	2101      	movs	r1, #1
 17a:	20af      	movs	r0, #175	; 0xaf
 17c:	4249      	negs	r1, r1
 17e:	f7ff ff9f 	bl	c0 <mbus_write_message32>

    while(1){

        if (irq_pending.wakeup) {
            irq_pending.wakeup = 0;
 182:	2601      	movs	r6, #1
	        mbus_write_message32(0xA5, 1);
 184:	25a5      	movs	r5, #165	; 0xa5
        if (irq_pending.wakeup) {
 186:	6823      	ldr	r3, [r4, #0]
 188:	07db      	lsls	r3, r3, #31
 18a:	d506      	bpl.n	19a <main+0x4a>
            irq_pending.wakeup = 0;
 18c:	6823      	ldr	r3, [r4, #0]
	        mbus_write_message32(0xA5, 1);
 18e:	2101      	movs	r1, #1
            irq_pending.wakeup = 0;
 190:	43b3      	bics	r3, r6
	        mbus_write_message32(0xA5, 1);
 192:	0028      	movs	r0, r5
            irq_pending.wakeup = 0;
 194:	6023      	str	r3, [r4, #0]
	        mbus_write_message32(0xA5, 1);
 196:	f7ff ff93 	bl	c0 <mbus_write_message32>

        }

        if (irq_pending.gocep) {
 19a:	6823      	ldr	r3, [r4, #0]
 19c:	075b      	lsls	r3, r3, #29
 19e:	d5f2      	bpl.n	186 <main+0x36>
            irq_pending.gocep = 0;
 1a0:	2204      	movs	r2, #4
 1a2:	6823      	ldr	r3, [r4, #0]
	        mbus_write_message32(0xA5, 2);
 1a4:	2102      	movs	r1, #2
            irq_pending.gocep = 0;
 1a6:	4393      	bics	r3, r2
	        mbus_write_message32(0xA5, 2);
 1a8:	0028      	movs	r0, r5
            irq_pending.gocep = 0;
 1aa:	6023      	str	r3, [r4, #0]
	        mbus_write_message32(0xA5, 2);
 1ac:	f7ff ff88 	bl	c0 <mbus_write_message32>
 1b0:	e7e9      	b.n	186 <main+0x36>
 1b2:	46c0      	nop			; (mov r8, r8)
 1b4:	000001c4 	.word	0x000001c4
 1b8:	e000e100 	.word	0xe000e100
 1bc:	000001c8 	.word	0x000001c8
 1c0:	5453104b 	.word	0x5453104b
