
mbc_code_v6_3_0/mbc_code_v6_3_0.elf:     file format elf32-littlearm


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
  40:	00001b41 	andeq	r1, r0, r1, asr #22
  44:	00000000 	andeq	r0, r0, r0
  48:	00001b4d 	andeq	r1, r0, sp, asr #22
  4c:	00001b59 	andeq	r1, r0, r9, asr fp
	...
  60:	00001b89 	andeq	r1, r0, r9, lsl #23
  64:	00001b99 	muleq	r0, r9, fp
  68:	00001ba9 	andeq	r1, r0, r9, lsr #23
  6c:	00001bb9 			; <UNDEFINED> instruction: 0x00001bb9
	...
  90:	00001b79 	andeq	r1, r0, r9, ror fp
	...

000000a0 <hang>:
  a0:	e7fe      	b.n	a0 <hang>

000000a2 <_start>:
  a2:	f001 fee7 	bl	1e74 <main>
  a6:	e7fc      	b.n	a2 <_start>

Disassembly of section .text.delay:

000000a8 <delay>:
// OTHER FUNCTIONS
//*******************************************************************

void delay(unsigned ticks){
  unsigned i;
  for (i=0; i < ticks; i++)
  a8:	2300      	movs	r3, #0
  aa:	4283      	cmp	r3, r0
  ac:	d100      	bne.n	b0 <delay+0x8>
    asm("nop;");
}
  ae:	4770      	bx	lr
    asm("nop;");
  b0:	46c0      	nop			; (mov r8, r8)
  for (i=0; i < ticks; i++)
  b2:	3301      	adds	r3, #1
  b4:	e7f9      	b.n	aa <delay+0x2>

Disassembly of section .text.WFI:

000000b6 <WFI>:

void WFI(){
  asm("wfi;");
  b6:	bf30      	wfi
}
  b8:	4770      	bx	lr

Disassembly of section .text.config_timer32:

000000bc <config_timer32>:
	*TIMER16_CNT   = cnt;
	*TIMER16_STAT  = status;
	*TIMER16_GO    = 0x1;
}

void config_timer32(uint32_t cmp, uint8_t roi, uint32_t cnt, uint32_t status){
  bc:	b530      	push	{r4, r5, lr}
	*TIMER32_GO   = 0x0;
  be:	2500      	movs	r5, #0
  c0:	4c06      	ldr	r4, [pc, #24]	; (dc <config_timer32+0x20>)
  c2:	6025      	str	r5, [r4, #0]
	*TIMER32_CMP  = cmp;
  c4:	4d06      	ldr	r5, [pc, #24]	; (e0 <config_timer32+0x24>)
  c6:	6028      	str	r0, [r5, #0]
	*TIMER32_ROI  = roi;
  c8:	4806      	ldr	r0, [pc, #24]	; (e4 <config_timer32+0x28>)
  ca:	6001      	str	r1, [r0, #0]
	*TIMER32_CNT  = cnt;
  cc:	4906      	ldr	r1, [pc, #24]	; (e8 <config_timer32+0x2c>)
  ce:	600a      	str	r2, [r1, #0]
	*TIMER32_STAT = status;
  d0:	4a06      	ldr	r2, [pc, #24]	; (ec <config_timer32+0x30>)
  d2:	6013      	str	r3, [r2, #0]
	*TIMER32_GO   = 0x1;
  d4:	2301      	movs	r3, #1
  d6:	6023      	str	r3, [r4, #0]
}
  d8:	bd30      	pop	{r4, r5, pc}
  da:	46c0      	nop			; (mov r8, r8)
  dc:	a0001100 	.word	0xa0001100
  e0:	a0001104 	.word	0xa0001104
  e4:	a0001108 	.word	0xa0001108
  e8:	a000110c 	.word	0xa000110c
  ec:	a0001110 	.word	0xa0001110

Disassembly of section .text.enable_xo_timer:

000000f0 <enable_xo_timer>:
	if( reset ) *WUPT_RESET = 0x01;
}

void enable_xo_timer () {
    uint32_t regval = *REG_XOT_CONFIG;
    regval |= 0x800000; // XOT_ENABLE = 1;
  f0:	2380      	movs	r3, #128	; 0x80
    uint32_t regval = *REG_XOT_CONFIG;
  f2:	4a03      	ldr	r2, [pc, #12]	; (100 <enable_xo_timer+0x10>)
    regval |= 0x800000; // XOT_ENABLE = 1;
  f4:	041b      	lsls	r3, r3, #16
    uint32_t regval = *REG_XOT_CONFIG;
  f6:	6811      	ldr	r1, [r2, #0]
    regval |= 0x800000; // XOT_ENABLE = 1;
  f8:	430b      	orrs	r3, r1
    *REG_XOT_CONFIG = regval;
  fa:	6013      	str	r3, [r2, #0]
}
  fc:	4770      	bx	lr
  fe:	46c0      	nop			; (mov r8, r8)
 100:	a000004c 	.word	0xa000004c

Disassembly of section .text.set_xo_timer:

00000104 <set_xo_timer>:
    uint32_t regval = *REG_XOT_CONFIG;
    regval &= 0x7FFFFF; // XOT_ENABLE = 0;
    *REG_XOT_CONFIG = regval;
}

void set_xo_timer (uint8_t mode, uint32_t timestamp, uint8_t wreq_en, uint8_t irq_en) {
 104:	b510      	push	{r4, lr}
    uint32_t regval0 = timestamp & 0x0000FFFF;
 106:	b28c      	uxth	r4, r1
    uint32_t regval1 = (timestamp >> 16) & 0xFFFF;
 108:	0c09      	lsrs	r1, r1, #16
    //uint32_t regval1 = timestamp & 0xFFFF0000; // This is wrong (Reported by Roger Hsiao, Jan 10, 2020)

    regval0 |= 0x00800000; // XOT_ENABLE = 1;
    if (mode)    regval0 |= 0x00400000; // XOT_MODE = 1
 10a:	2800      	cmp	r0, #0
 10c:	d111      	bne.n	132 <set_xo_timer+0x2e>
    regval0 |= 0x00800000; // XOT_ENABLE = 1;
 10e:	2080      	movs	r0, #128	; 0x80
    if (mode)    regval0 |= 0x00400000; // XOT_MODE = 1
 110:	0400      	lsls	r0, r0, #16
 112:	4320      	orrs	r0, r4
    if (wreq_en) regval0 |= 0x00200000; // XOT_WREQ_EN = 1
 114:	2a00      	cmp	r2, #0
 116:	d002      	beq.n	11e <set_xo_timer+0x1a>
 118:	2280      	movs	r2, #128	; 0x80
 11a:	0392      	lsls	r2, r2, #14
 11c:	4310      	orrs	r0, r2
    if (irq_en)  regval0 |= 0x00100000; // XOT_IRQ_EN = 1
 11e:	2b00      	cmp	r3, #0
 120:	d002      	beq.n	128 <set_xo_timer+0x24>
 122:	2380      	movs	r3, #128	; 0x80
 124:	035b      	lsls	r3, r3, #13
 126:	4318      	orrs	r0, r3

    *REG_XOT_CONFIGU = regval1;
 128:	4b03      	ldr	r3, [pc, #12]	; (138 <set_xo_timer+0x34>)
 12a:	6019      	str	r1, [r3, #0]
    *REG_XOT_CONFIG  = regval0;
 12c:	4b03      	ldr	r3, [pc, #12]	; (13c <set_xo_timer+0x38>)
 12e:	6018      	str	r0, [r3, #0]
}
 130:	bd10      	pop	{r4, pc}
    if (mode)    regval0 |= 0x00400000; // XOT_MODE = 1
 132:	20c0      	movs	r0, #192	; 0xc0
 134:	e7ec      	b.n	110 <set_xo_timer+0xc>
 136:	46c0      	nop			; (mov r8, r8)
 138:	a0000050 	.word	0xa0000050
 13c:	a000004c 	.word	0xa000004c

Disassembly of section .text.reset_xo_cnt:

00000140 <reset_xo_cnt>:

void reset_xo_cnt  () { *XOT_RESET_CNT  = 0x1; }
 140:	2201      	movs	r2, #1
 142:	4b01      	ldr	r3, [pc, #4]	; (148 <reset_xo_cnt+0x8>)
 144:	601a      	str	r2, [r3, #0]
 146:	4770      	bx	lr
 148:	a0001400 	.word	0xa0001400

Disassembly of section .text.start_xo_cnt:

0000014c <start_xo_cnt>:
void start_xo_cnt  () { *XOT_START_CNT  = 0x1; }
 14c:	2201      	movs	r2, #1
 14e:	4b01      	ldr	r3, [pc, #4]	; (154 <start_xo_cnt+0x8>)
 150:	601a      	str	r2, [r3, #0]
 152:	4770      	bx	lr
 154:	a0001404 	.word	0xa0001404

Disassembly of section .text.stop_xo_cnt:

00000158 <stop_xo_cnt>:
void stop_xo_cnt   () { *XOT_STOP_CNT   = 0x1; }
 158:	2201      	movs	r2, #1
 15a:	4b01      	ldr	r3, [pc, #4]	; (160 <stop_xo_cnt+0x8>)
 15c:	601a      	str	r2, [r3, #0]
 15e:	4770      	bx	lr
 160:	a0001408 	.word	0xa0001408

Disassembly of section .text.set_halt_until_mbus_tx:

00000164 <set_halt_until_mbus_tx>:
// MBUS IRQ SETTING
//**************************************************
void set_halt_until_reg(uint32_t reg_id) { *SREG_CONF_HALT = reg_id; }
void set_halt_until_mem_wr(void) { *SREG_CONF_HALT = HALT_UNTIL_MEM_WR; }
void set_halt_until_mbus_rx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_RX; }
void set_halt_until_mbus_tx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_TX; }
 164:	2209      	movs	r2, #9
 166:	4b01      	ldr	r3, [pc, #4]	; (16c <set_halt_until_mbus_tx+0x8>)
 168:	601a      	str	r2, [r3, #0]
 16a:	4770      	bx	lr
 16c:	a000a000 	.word	0xa000a000

Disassembly of section .text.set_halt_until_mbus_trx:

00000170 <set_halt_until_mbus_trx>:
void set_halt_until_mbus_trx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_TRX; }
 170:	220c      	movs	r2, #12
 172:	4b01      	ldr	r3, [pc, #4]	; (178 <set_halt_until_mbus_trx+0x8>)
 174:	601a      	str	r2, [r3, #0]
 176:	4770      	bx	lr
 178:	a000a000 	.word	0xa000a000

Disassembly of section .text.mbus_write_message32:

0000017c <mbus_write_message32>:
	// TODO: Read from LC
	return 1;
}

uint32_t mbus_write_message32(uint8_t addr, uint32_t data) {
    uint32_t mbus_addr = 0xA0003000 | (addr << 4);
 17c:	0103      	lsls	r3, r0, #4
 17e:	4802      	ldr	r0, [pc, #8]	; (188 <mbus_write_message32+0xc>)
 180:	4318      	orrs	r0, r3
    *((volatile uint32_t *) mbus_addr) = data;
 182:	6001      	str	r1, [r0, #0]
    return 1;
}
 184:	2001      	movs	r0, #1
 186:	4770      	bx	lr
 188:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_write_message:

0000018c <mbus_write_message>:

uint32_t mbus_write_message(uint8_t addr, uint32_t data[], unsigned len) {
 18c:	0003      	movs	r3, r0
 18e:	1e10      	subs	r0, r2, #0
	// Goal: Use the "Memory Stream Write" to put unconstrained 32-bit data
	//       onto the bus.
	if (len == 0) return 0;
 190:	d00a      	beq.n	1a8 <mbus_write_message+0x1c>

	*MBUS_CMD0 = (addr << 24) | (len-1);
 192:	061b      	lsls	r3, r3, #24
 194:	3801      	subs	r0, #1
 196:	4318      	orrs	r0, r3
 198:	4b04      	ldr	r3, [pc, #16]	; (1ac <mbus_write_message+0x20>)
	*MBUS_CMD1 = (uint32_t) data;
	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x2 << 4);
 19a:	2223      	movs	r2, #35	; 0x23
	*MBUS_CMD0 = (addr << 24) | (len-1);
 19c:	6018      	str	r0, [r3, #0]
	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x2 << 4);
 19e:	2001      	movs	r0, #1
	*MBUS_CMD1 = (uint32_t) data;
 1a0:	4b03      	ldr	r3, [pc, #12]	; (1b0 <mbus_write_message+0x24>)
 1a2:	6019      	str	r1, [r3, #0]
	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x2 << 4);
 1a4:	4b03      	ldr	r3, [pc, #12]	; (1b4 <mbus_write_message+0x28>)
 1a6:	601a      	str	r2, [r3, #0]

    return 1;
}
 1a8:	4770      	bx	lr
 1aa:	46c0      	nop			; (mov r8, r8)
 1ac:	a0002000 	.word	0xa0002000
 1b0:	a0002004 	.word	0xa0002004
 1b4:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_enumerate:

000001b8 <mbus_enumerate>:

void mbus_enumerate(unsigned new_prefix) {
    mbus_write_message32(MBUS_DISC_AND_ENUM, ((MBUS_ENUMERATE_CMD << 28) | (new_prefix << 24)));
 1b8:	0603      	lsls	r3, r0, #24
 1ba:	2080      	movs	r0, #128	; 0x80
 1bc:	0580      	lsls	r0, r0, #22
 1be:	4318      	orrs	r0, r3
    *((volatile uint32_t *) mbus_addr) = data;
 1c0:	4b01      	ldr	r3, [pc, #4]	; (1c8 <mbus_enumerate+0x10>)
 1c2:	6018      	str	r0, [r3, #0]
}
 1c4:	4770      	bx	lr
 1c6:	46c0      	nop			; (mov r8, r8)
 1c8:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

000001cc <mbus_sleep_all>:
    *((volatile uint32_t *) mbus_addr) = data;
 1cc:	2200      	movs	r2, #0
 1ce:	4b01      	ldr	r3, [pc, #4]	; (1d4 <mbus_sleep_all+0x8>)
 1d0:	601a      	str	r2, [r3, #0]

void mbus_sleep_all(void) {
    mbus_write_message32(MBUS_POWER, MBUS_ALL_SLEEP << 28);
}
 1d2:	4770      	bx	lr
 1d4:	a0003010 	.word	0xa0003010

Disassembly of section .text.mbus_copy_registers_from_remote_to_local:

000001d8 <mbus_copy_registers_from_remote_to_local>:
void mbus_copy_registers_from_remote_to_local(
		uint8_t remote_prefix,
		uint8_t remote_reg_start,
		uint8_t local_reg_start,
		uint8_t length_minus_one
		) {
 1d8:	b507      	push	{r0, r1, r2, lr}
	// Put a register read command on the bus instructed to write this node

	uint8_t address = ((remote_prefix & 0xf) << 4) | MPQ_REG_READ;
	uint32_t data = 
        (remote_reg_start << 24) |
 1da:	0609      	lsls	r1, r1, #24
		(length_minus_one << 16) |
		(mbus_get_short_prefix() << 12) |
		(MPQ_REG_WRITE << 8) | // Write regs *to* _this_ node
 1dc:	4311      	orrs	r1, r2
		(length_minus_one << 16) |
 1de:	041b      	lsls	r3, r3, #16
		(MPQ_REG_WRITE << 8) | // Write regs *to* _this_ node
 1e0:	4319      	orrs	r1, r3
 1e2:	2380      	movs	r3, #128	; 0x80
	uint8_t address = ((remote_prefix & 0xf) << 4) | MPQ_REG_READ;
 1e4:	2201      	movs	r2, #1
		(MPQ_REG_WRITE << 8) | // Write regs *to* _this_ node
 1e6:	015b      	lsls	r3, r3, #5
	uint8_t address = ((remote_prefix & 0xf) << 4) | MPQ_REG_READ;
 1e8:	0100      	lsls	r0, r0, #4
		(MPQ_REG_WRITE << 8) | // Write regs *to* _this_ node
 1ea:	4319      	orrs	r1, r3
	uint8_t address = ((remote_prefix & 0xf) << 4) | MPQ_REG_READ;
 1ec:	4310      	orrs	r0, r2
	uint32_t data = 
 1ee:	9101      	str	r1, [sp, #4]
		(local_reg_start << 0);

	mbus_write_message(address, &data, 1);
 1f0:	b2c0      	uxtb	r0, r0
 1f2:	a901      	add	r1, sp, #4
 1f4:	f7ff ffca 	bl	18c <mbus_write_message>
}
 1f8:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_remote_register_write:

000001fa <mbus_remote_register_write>:

void mbus_remote_register_write(
		uint8_t prefix,
		uint8_t dst_reg_addr,
		uint32_t dst_reg_val
		) {
 1fa:	b507      	push	{r0, r1, r2, lr}
	// assert (prefix < 16 && > 0);
	uint8_t address = ((prefix & 0xf) << 4) | MPQ_REG_WRITE;
	uint32_t data = (dst_reg_addr << 24) | (dst_reg_val & 0xffffff);
 1fc:	0212      	lsls	r2, r2, #8
 1fe:	0a12      	lsrs	r2, r2, #8
 200:	0609      	lsls	r1, r1, #24
 202:	4311      	orrs	r1, r2
	uint8_t address = ((prefix & 0xf) << 4) | MPQ_REG_WRITE;
 204:	0100      	lsls	r0, r0, #4
	uint32_t data = (dst_reg_addr << 24) | (dst_reg_val & 0xffffff);
 206:	9101      	str	r1, [sp, #4]
	mbus_write_message(address, &data, 1);
 208:	2201      	movs	r2, #1
 20a:	b2c0      	uxtb	r0, r0
 20c:	a901      	add	r1, sp, #4
 20e:	f7ff ffbd 	bl	18c <mbus_write_message>
}
 212:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_copy_mem_from_local_to_remote_bulk:

00000214 <mbus_copy_mem_from_local_to_remote_bulk>:
		uint8_t   remote_prefix,
		uint32_t* remote_memory_address,
		uint32_t* local_address,
		uint32_t  length_in_words_minus_one
		) {
	*MBUS_CMD0 = ( ((uint32_t) remote_prefix) << 28 ) | (MPQ_MEM_BULK_WRITE << 24) | (length_in_words_minus_one & 0xFFFFF);
 214:	031b      	lsls	r3, r3, #12
 216:	0b1b      	lsrs	r3, r3, #12
 218:	0700      	lsls	r0, r0, #28
 21a:	4318      	orrs	r0, r3
 21c:	2380      	movs	r3, #128	; 0x80
 21e:	049b      	lsls	r3, r3, #18
 220:	4318      	orrs	r0, r3
 222:	4b05      	ldr	r3, [pc, #20]	; (238 <mbus_copy_mem_from_local_to_remote_bulk+0x24>)
 224:	6018      	str	r0, [r3, #0]
	*MBUS_CMD1 = (uint32_t) local_address;
 226:	4b05      	ldr	r3, [pc, #20]	; (23c <mbus_copy_mem_from_local_to_remote_bulk+0x28>)
 228:	601a      	str	r2, [r3, #0]
	*MBUS_CMD2 = (uint32_t) remote_memory_address;

	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x3 << 4);
 22a:	2233      	movs	r2, #51	; 0x33
	*MBUS_CMD2 = (uint32_t) remote_memory_address;
 22c:	4b04      	ldr	r3, [pc, #16]	; (240 <mbus_copy_mem_from_local_to_remote_bulk+0x2c>)
 22e:	6019      	str	r1, [r3, #0]
	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x3 << 4);
 230:	4b04      	ldr	r3, [pc, #16]	; (244 <mbus_copy_mem_from_local_to_remote_bulk+0x30>)
 232:	601a      	str	r2, [r3, #0]
}
 234:	4770      	bx	lr
 236:	46c0      	nop			; (mov r8, r8)
 238:	a0002000 	.word	0xa0002000
 23c:	a0002004 	.word	0xa0002004
 240:	a0002008 	.word	0xa0002008
 244:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_copy_mem_from_remote_to_any_bulk:

00000248 <mbus_copy_mem_from_remote_to_any_bulk>:
		uint8_t   source_prefix,
		uint32_t* source_memory_address,
		uint8_t   destination_prefix,
		uint32_t* destination_memory_address,
		uint32_t  length_in_words_minus_one
		) {
 248:	b51f      	push	{r0, r1, r2, r3, r4, lr}
	uint32_t payload[3] = {
		( ((uint32_t) destination_prefix) << 28 )| (MPQ_MEM_BULK_WRITE << 24) | (length_in_words_minus_one & 0xFFFFF),
 24a:	9c06      	ldr	r4, [sp, #24]
 24c:	0712      	lsls	r2, r2, #28
 24e:	0324      	lsls	r4, r4, #12
 250:	0b24      	lsrs	r4, r4, #12
 252:	4322      	orrs	r2, r4
 254:	2480      	movs	r4, #128	; 0x80
 256:	04a4      	lsls	r4, r4, #18
 258:	4322      	orrs	r2, r4
	uint32_t payload[3] = {
 25a:	9201      	str	r2, [sp, #4]
		(uint32_t) source_memory_address,
		(uint32_t) destination_memory_address,
	};
	mbus_write_message(((source_prefix << 4 ) | MPQ_MEM_READ), payload, 3);
 25c:	2203      	movs	r2, #3
 25e:	0100      	lsls	r0, r0, #4
 260:	4310      	orrs	r0, r2
	uint32_t payload[3] = {
 262:	9102      	str	r1, [sp, #8]
	mbus_write_message(((source_prefix << 4 ) | MPQ_MEM_READ), payload, 3);
 264:	b2c0      	uxtb	r0, r0
 266:	a901      	add	r1, sp, #4
	uint32_t payload[3] = {
 268:	9303      	str	r3, [sp, #12]
	mbus_write_message(((source_prefix << 4 ) | MPQ_MEM_READ), payload, 3);
 26a:	f7ff ff8f 	bl	18c <mbus_write_message>
}
 26e:	bd1f      	pop	{r0, r1, r2, r3, r4, pc}

Disassembly of section .text.var_init:

00000270 <var_init>:
 * Initialization functions
 **********************************************/

static void var_init() {
    // initialize variables that could be corrupted at program time
    lnt_counter_base = 0;
 270:	2300      	movs	r3, #0
 272:	4a19      	ldr	r2, [pc, #100]	; (2d8 <var_init+0x68>)
    rot_idx = 0;
    avg_light = 0;
    next_sunrise = 0;
    next_sunset = 0;

    code_cache_remainder = CODE_CACHE_MAX_REMAINDER;
 274:	4919      	ldr	r1, [pc, #100]	; (2dc <var_init+0x6c>)
    lnt_counter_base = 0;
 276:	8013      	strh	r3, [r2, #0]
    rot_idx = 0;
 278:	4a19      	ldr	r2, [pc, #100]	; (2e0 <var_init+0x70>)
 27a:	8013      	strh	r3, [r2, #0]
    avg_light = 0;
 27c:	4a19      	ldr	r2, [pc, #100]	; (2e4 <var_init+0x74>)
 27e:	8013      	strh	r3, [r2, #0]
    next_sunrise = 0;
 280:	4a19      	ldr	r2, [pc, #100]	; (2e8 <var_init+0x78>)
 282:	6013      	str	r3, [r2, #0]
    next_sunset = 0;
 284:	4a19      	ldr	r2, [pc, #100]	; (2ec <var_init+0x7c>)
 286:	6013      	str	r3, [r2, #0]
    code_cache_remainder = CODE_CACHE_MAX_REMAINDER;
 288:	2288      	movs	r2, #136	; 0x88
 28a:	0052      	lsls	r2, r2, #1
 28c:	800a      	strh	r2, [r1, #0]
    code_addr = 0;
 28e:	4918      	ldr	r1, [pc, #96]	; (2f0 <var_init+0x80>)
 290:	800b      	strh	r3, [r1, #0]
    max_unit_count = 0;
 292:	4918      	ldr	r1, [pc, #96]	; (2f4 <var_init+0x84>)
 294:	800b      	strh	r3, [r1, #0]
    radio_beacon_counter = 0;
 296:	4918      	ldr	r1, [pc, #96]	; (2f8 <var_init+0x88>)
 298:	800b      	strh	r3, [r1, #0]
    low_pwr_count = 0;
 29a:	4918      	ldr	r1, [pc, #96]	; (2fc <var_init+0x8c>)
 29c:	800b      	strh	r3, [r1, #0]
    temp_packet_num = 0;
 29e:	4918      	ldr	r1, [pc, #96]	; (300 <var_init+0x90>)
 2a0:	800b      	strh	r3, [r1, #0]
    light_packet_num = 0;
 2a2:	4918      	ldr	r1, [pc, #96]	; (304 <var_init+0x94>)
 2a4:	800b      	strh	r3, [r1, #0]
    has_header = false;
 2a6:	4918      	ldr	r1, [pc, #96]	; (308 <var_init+0x98>)
 2a8:	700b      	strb	r3, [r1, #0]
    has_time = false;
 2aa:	4918      	ldr	r1, [pc, #96]	; (30c <var_init+0x9c>)
 2ac:	700b      	strb	r3, [r1, #0]

    temp_cache_remainder = TEMP_CACHE_MAX_REMAINDER;
 2ae:	4918      	ldr	r1, [pc, #96]	; (310 <var_init+0xa0>)
 2b0:	800a      	strh	r2, [r1, #0]

    projected_end_time_in_sec = 0;
 2b2:	4a18      	ldr	r2, [pc, #96]	; (314 <var_init+0xa4>)
 2b4:	6013      	str	r3, [r2, #0]
    next_radio_debug_time = 0;
 2b6:	4a18      	ldr	r2, [pc, #96]	; (318 <var_init+0xa8>)
 2b8:	6013      	str	r3, [r2, #0]
    next_light_meas_time = 0;
 2ba:	4a18      	ldr	r2, [pc, #96]	; (31c <var_init+0xac>)
 2bc:	6013      	str	r3, [r2, #0]
    last_log_temp = 0;
 2be:	4a18      	ldr	r2, [pc, #96]	; (320 <var_init+0xb0>)
 2c0:	8013      	strh	r3, [r2, #0]

    mrr_cfo_val_fine = 0;
 2c2:	4a18      	ldr	r2, [pc, #96]	; (324 <var_init+0xb4>)
 2c4:	8013      	strh	r3, [r2, #0]

    error_code = 0;
 2c6:	4a18      	ldr	r2, [pc, #96]	; (328 <var_init+0xb8>)
 2c8:	6013      	str	r3, [r2, #0]
    error_time = 0;
 2ca:	4a18      	ldr	r2, [pc, #96]	; (32c <var_init+0xbc>)
 2cc:	6013      	str	r3, [r2, #0]

    data_collection_end_day_time = MID_DAY_TIME;
 2ce:	4b18      	ldr	r3, [pc, #96]	; (330 <var_init+0xc0>)
 2d0:	4a18      	ldr	r2, [pc, #96]	; (334 <var_init+0xc4>)
 2d2:	601a      	str	r2, [r3, #0]
}
 2d4:	4770      	bx	lr
 2d6:	46c0      	nop			; (mov r8, r8)
 2d8:	00003a3e 	.word	0x00003a3e
 2dc:	000038f0 	.word	0x000038f0
 2e0:	00003a9a 	.word	0x00003a9a
 2e4:	00003a06 	.word	0x00003a06
 2e8:	00003a78 	.word	0x00003a78
 2ec:	00003a7c 	.word	0x00003a7c
 2f0:	00003a0a 	.word	0x00003a0a
 2f4:	00003a5a 	.word	0x00003a5a
 2f8:	00003a84 	.word	0x00003a84
 2fc:	00003a50 	.word	0x00003a50
 300:	00003ae0 	.word	0x00003ae0
 304:	00003a3c 	.word	0x00003a3c
 308:	00003a2c 	.word	0x00003a2c
 30c:	00003a2d 	.word	0x00003a2d
 310:	0000394a 	.word	0x0000394a
 314:	00003a80 	.word	0x00003a80
 318:	00003a74 	.word	0x00003a74
 31c:	00003a70 	.word	0x00003a70
 320:	00003a30 	.word	0x00003a30
 324:	00003a5c 	.word	0x00003a5c
 328:	00003a24 	.word	0x00003a24
 32c:	00003a28 	.word	0x00003a28
 330:	000038f4 	.word	0x000038f4
 334:	0000a8c0 	.word	0x0000a8c0

Disassembly of section .text.pmu_reg_write:

00000338 <pmu_reg_write>:
static void pmu_reg_write(uint32_t reg_addr, uint32_t reg_data) {
 338:	b570      	push	{r4, r5, r6, lr}
 33a:	000d      	movs	r5, r1
 33c:	0004      	movs	r4, r0
    set_halt_until_mbus_trx();
 33e:	f7ff ff17 	bl	170 <set_halt_until_mbus_trx>
    mbus_remote_register_write(PMU_ADDR, reg_addr, reg_data);
 342:	002a      	movs	r2, r5
 344:	2005      	movs	r0, #5
 346:	b2e1      	uxtb	r1, r4
 348:	f7ff ff57 	bl	1fa <mbus_remote_register_write>
    set_halt_until_mbus_tx();
 34c:	f7ff ff0a 	bl	164 <set_halt_until_mbus_tx>
}
 350:	bd70      	pop	{r4, r5, r6, pc}

Disassembly of section .text.pmu_set_sleep_clk:

00000352 <pmu_set_sleep_clk>:
static void pmu_set_sleep_clk(uint32_t setting) {
 352:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    uint16_t l = (setting >> 16) & 0xFF;
 354:	27ff      	movs	r7, #255	; 0xff
 356:	0c01      	lsrs	r1, r0, #16
                 (l <<  5) |    // frequency multiplier l (actually l+1)
 358:	4039      	ands	r1, r7
 35a:	014b      	lsls	r3, r1, #5
 35c:	21e0      	movs	r1, #224	; 0xe0
static void pmu_set_sleep_clk(uint32_t setting) {
 35e:	0004      	movs	r4, r0
    uint16_t r = (setting >> 24) & 0xFF;
 360:	0e05      	lsrs	r5, r0, #24
                 (r <<  9) |    // frequency multiplier r
 362:	026d      	lsls	r5, r5, #9
    uint16_t base = (setting >> 8) & 0xFF;
 364:	0a06      	lsrs	r6, r0, #8
                 (l <<  5) |    // frequency multiplier l (actually l+1)
 366:	403e      	ands	r6, r7
 368:	9301      	str	r3, [sp, #4]
 36a:	432b      	orrs	r3, r5
 36c:	4333      	orrs	r3, r6
 36e:	0209      	lsls	r1, r1, #8
 370:	4319      	orrs	r1, r3
    pmu_reg_write(0x17,         // PMU_EN_UPCONVERTER_TRIM_V3_SLEEP
 372:	2017      	movs	r0, #23
 374:	f7ff ffe0 	bl	338 <pmu_reg_write>
                 (l_1p2 << 5) | // frequency multiplier l (actually l+1)
 378:	0021      	movs	r1, r4
 37a:	4039      	ands	r1, r7
 37c:	27c0      	movs	r7, #192	; 0xc0
 37e:	0149      	lsls	r1, r1, #5
 380:	4331      	orrs	r1, r6
 382:	023f      	lsls	r7, r7, #8
 384:	4329      	orrs	r1, r5
 386:	4339      	orrs	r1, r7
    pmu_reg_write(0x15,         // PMU_EN_SAR_TRIM_V3_SLEEP
 388:	2015      	movs	r0, #21
 38a:	f7ff ffd5 	bl	338 <pmu_reg_write>
                 (l <<  5) |    // frequency multiplier l (actually l+1)
 38e:	9901      	ldr	r1, [sp, #4]
 390:	4335      	orrs	r5, r6
 392:	4329      	orrs	r1, r5
    pmu_reg_write(0x19,         // PMU_EN_DOWNCONVERTER_TRIM_V3_SLEEP
 394:	2019      	movs	r0, #25
 396:	f7ff ffcf 	bl	338 <pmu_reg_write>
}
 39a:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}

Disassembly of section .text.pmu_set_active_clk:

0000039c <pmu_set_active_clk>:
static void pmu_set_active_clk(uint32_t setting) {
 39c:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    uint16_t base = (setting >> 8) & 0xFF;
 39e:	27ff      	movs	r7, #255	; 0xff
                 (cclk_div << 14) |    // comparator clock division ratio
 3a0:	0006      	movs	r6, r0
    uint16_t base = (setting >> 8) & 0xFF;
 3a2:	0a01      	lsrs	r1, r0, #8
                 (cclk_div << 14) |    // comparator clock division ratio
 3a4:	403e      	ands	r6, r7
                 (0 << 5) | // frequency multiplier l (actually l+1) GC: fixed at 0
 3a6:	4039      	ands	r1, r7
    uint16_t r = (setting >> 24) & 0xFF;
 3a8:	0e05      	lsrs	r5, r0, #24
                 (cclk_div << 14) |    // comparator clock division ratio
 3aa:	03b6      	lsls	r6, r6, #14
                 (r <<  9) |    // frequency multiplier r
 3ac:	026d      	lsls	r5, r5, #9
                 (0 << 5) | // frequency multiplier l (actually l+1) GC: fixed at 0
 3ae:	430e      	orrs	r6, r1
 3b0:	432e      	orrs	r6, r5
static void pmu_set_active_clk(uint32_t setting) {
 3b2:	0004      	movs	r4, r0
                 (0 << 5) | // frequency multiplier l (actually l+1) GC: fixed at 0
 3b4:	9101      	str	r1, [sp, #4]
    pmu_reg_write(0x16,         // PMU_EN_SAR_TRIM_V3_ACTIVE
 3b6:	2016      	movs	r0, #22
 3b8:	0031      	movs	r1, r6
 3ba:	f7ff ffbd 	bl	338 <pmu_reg_write>
    pmu_reg_write(0x16,         // PMU_EN_SAR_TRIM_V3_ACTIVE
 3be:	0031      	movs	r1, r6
 3c0:	2016      	movs	r0, #22
 3c2:	f7ff ffb9 	bl	338 <pmu_reg_write>
                 (l <<  5) |    // frequency multiplier l
 3c6:	002b      	movs	r3, r5
 3c8:	21e0      	movs	r1, #224	; 0xe0
    uint16_t l = (setting >> 16) & 0xFF;
 3ca:	0c24      	lsrs	r4, r4, #16
                 (l <<  5) |    // frequency multiplier l
 3cc:	4027      	ands	r7, r4
 3ce:	9a01      	ldr	r2, [sp, #4]
 3d0:	017f      	lsls	r7, r7, #5
 3d2:	433b      	orrs	r3, r7
 3d4:	4313      	orrs	r3, r2
 3d6:	0209      	lsls	r1, r1, #8
 3d8:	4319      	orrs	r1, r3
    pmu_reg_write(0x18,         // PMU_EN_UPCONVERTER_TRIM_V3_ACTIVE
 3da:	2018      	movs	r0, #24
 3dc:	f7ff ffac 	bl	338 <pmu_reg_write>
                 (l <<  5) |    // frequency multiplier l
 3e0:	9b01      	ldr	r3, [sp, #4]
    pmu_reg_write(0x1A,         // PMU_EN_DOWNCONVERTER_TRIM_V3_ACTIVE
 3e2:	201a      	movs	r0, #26
                 (l <<  5) |    // frequency multiplier l
 3e4:	431d      	orrs	r5, r3
 3e6:	0029      	movs	r1, r5
 3e8:	4339      	orrs	r1, r7
    pmu_reg_write(0x1A,         // PMU_EN_DOWNCONVERTER_TRIM_V3_ACTIVE
 3ea:	f7ff ffa5 	bl	338 <pmu_reg_write>
}
 3ee:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}

Disassembly of section .text.pmu_current_marker:

000003f0 <pmu_current_marker>:
static void pmu_current_marker() {
 3f0:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
            temp_setting |= 0x0000FF00;
 3f2:	26ff      	movs	r6, #255	; 0xff
static void pmu_current_marker() {
 3f4:	2706      	movs	r7, #6
            temp_setting |= 0x0000FF00;
 3f6:	0236      	lsls	r6, r6, #8
        if(i == 0 || snt_sys_temp_code >= PMU_TEMP_THRESH[i - 1]) {
 3f8:	1e7c      	subs	r4, r7, #1
 3fa:	2f00      	cmp	r7, #0
 3fc:	d006      	beq.n	40c <pmu_current_marker+0x1c>
 3fe:	490a      	ldr	r1, [pc, #40]	; (428 <pmu_current_marker+0x38>)
 400:	00a2      	lsls	r2, r4, #2
 402:	5851      	ldr	r1, [r2, r1]
 404:	4a09      	ldr	r2, [pc, #36]	; (42c <pmu_current_marker+0x3c>)
 406:	6812      	ldr	r2, [r2, #0]
 408:	4291      	cmp	r1, r2
 40a:	d808      	bhi.n	41e <pmu_current_marker+0x2e>
            uint32_t temp_setting = PMU_ACTIVE_SETTINGS[i] & 0xFFFFFFFF;
 40c:	4d08      	ldr	r5, [pc, #32]	; (430 <pmu_current_marker+0x40>)
 40e:	00bf      	lsls	r7, r7, #2
 410:	5978      	ldr	r0, [r7, r5]
            temp_setting |= 0x0000FF00;
 412:	4330      	orrs	r0, r6
            pmu_set_active_clk(temp_setting);
 414:	f7ff ffc2 	bl	39c <pmu_set_active_clk>
            pmu_set_active_clk(PMU_ACTIVE_SETTINGS[i]);
 418:	5978      	ldr	r0, [r7, r5]
 41a:	f7ff ffbf 	bl	39c <pmu_set_active_clk>
    for(i = PMU_SETTINGS_LEN; i >= 0; i--) {
 41e:	0027      	movs	r7, r4
 420:	1c63      	adds	r3, r4, #1
 422:	d1e9      	bne.n	3f8 <pmu_current_marker+0x8>
}
 424:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 426:	46c0      	nop			; (mov r8, r8)
 428:	000038ac 	.word	0x000038ac
 42c:	00003938 	.word	0x00003938
 430:	0000380c 	.word	0x0000380c

Disassembly of section .text.radio_power_off:

00000434 <radio_power_off>:
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
 434:	2208      	movs	r2, #8
static void radio_power_off() {
 436:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    mrrv11a_r11.FSM_EN = 0;  //Stop BB
 438:	2704      	movs	r7, #4
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
 43a:	2502      	movs	r5, #2
    mrrv11a_r11.FSM_SLEEP = 1;
 43c:	2601      	movs	r6, #1
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
 43e:	4c2e      	ldr	r4, [pc, #184]	; (4f8 <radio_power_off+0xc4>)
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 440:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
 442:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 444:	2002      	movs	r0, #2
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
 446:	4393      	bics	r3, r2
 448:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 44a:	6822      	ldr	r2, [r4, #0]
 44c:	f7ff fed5 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r03.TRX_ISOLATEN = 0;     //set ISOLATEN 0
 450:	4b2a      	ldr	r3, [pc, #168]	; (4fc <radio_power_off+0xc8>)
 452:	492b      	ldr	r1, [pc, #172]	; (500 <radio_power_off+0xcc>)
 454:	681a      	ldr	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 456:	2002      	movs	r0, #2
    mrrv11a_r03.TRX_ISOLATEN = 0;     //set ISOLATEN 0
 458:	400a      	ands	r2, r1
 45a:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 45c:	681a      	ldr	r2, [r3, #0]
 45e:	2103      	movs	r1, #3
 460:	f7ff fecb 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r11.FSM_EN = 0;  //Stop BB
 464:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 466:	0028      	movs	r0, r5
    mrrv11a_r11.FSM_EN = 0;  //Stop BB
 468:	43bb      	bics	r3, r7
 46a:	6023      	str	r3, [r4, #0]
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
 46c:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 46e:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
 470:	43ab      	bics	r3, r5
 472:	6023      	str	r3, [r4, #0]
    mrrv11a_r11.FSM_SLEEP = 1;
 474:	6823      	ldr	r3, [r4, #0]
 476:	4333      	orrs	r3, r6
 478:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 47a:	6822      	ldr	r2, [r4, #0]
 47c:	f7ff febd 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
 480:	4c20      	ldr	r4, [pc, #128]	; (504 <radio_power_off+0xd0>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 482:	0028      	movs	r0, r5
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
 484:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 486:	2100      	movs	r1, #0
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
 488:	43b3      	bics	r3, r6
 48a:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 48c:	6822      	ldr	r2, [r4, #0]
 48e:	f7ff feb4 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
 492:	227e      	movs	r2, #126	; 0x7e
 494:	2120      	movs	r1, #32
 496:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 498:	0028      	movs	r0, r5
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
 49a:	4393      	bics	r3, r2
 49c:	430b      	orrs	r3, r1
 49e:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 4a0:	6822      	ldr	r2, [r4, #0]
 4a2:	2100      	movs	r1, #0
 4a4:	f7ff fea9 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
 4a8:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 4aa:	0028      	movs	r0, r5
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
 4ac:	4333      	orrs	r3, r6
 4ae:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 4b0:	6822      	ldr	r2, [r4, #0]
 4b2:	2100      	movs	r1, #0
 4b4:	f7ff fea1 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r04.RO_RESET = 1;  //Release Reset TIMER
 4b8:	2208      	movs	r2, #8
    mrrv11a_r04.RO_EN_CLK = 0; //Enable CLK TIMER
 4ba:	2120      	movs	r1, #32
    mrrv11a_r04.RO_RESET = 1;  //Release Reset TIMER
 4bc:	4c12      	ldr	r4, [pc, #72]	; (508 <radio_power_off+0xd4>)
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 4be:	0028      	movs	r0, r5
    mrrv11a_r04.RO_RESET = 1;  //Release Reset TIMER
 4c0:	6823      	ldr	r3, [r4, #0]
 4c2:	4313      	orrs	r3, r2
 4c4:	6023      	str	r3, [r4, #0]
    mrrv11a_r04.RO_EN_CLK = 0; //Enable CLK TIMER
 4c6:	6823      	ldr	r3, [r4, #0]
 4c8:	438b      	bics	r3, r1
 4ca:	6023      	str	r3, [r4, #0]
    mrrv11a_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
 4cc:	2310      	movs	r3, #16
 4ce:	6822      	ldr	r2, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 4d0:	0039      	movs	r1, r7
    mrrv11a_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
 4d2:	4313      	orrs	r3, r2
 4d4:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 4d6:	6822      	ldr	r2, [r4, #0]
 4d8:	f7ff fe8f 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
 4dc:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 4de:	0039      	movs	r1, r7
    mrrv11a_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
 4e0:	43b3      	bics	r3, r6
 4e2:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 4e4:	0028      	movs	r0, r5
 4e6:	6822      	ldr	r2, [r4, #0]
 4e8:	f7ff fe87 	bl	1fa <mbus_remote_register_write>
    radio_on = 0;
 4ec:	2300      	movs	r3, #0
 4ee:	4a07      	ldr	r2, [pc, #28]	; (50c <radio_power_off+0xd8>)
 4f0:	8013      	strh	r3, [r2, #0]
    radio_ready = 0;
 4f2:	4a07      	ldr	r2, [pc, #28]	; (510 <radio_power_off+0xdc>)
 4f4:	8013      	strh	r3, [r2, #0]
}
 4f6:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 4f8:	00003920 	.word	0x00003920
 4fc:	00003910 	.word	0x00003910
 500:	ffbfffff 	.word	0xffbfffff
 504:	0000390c 	.word	0x0000390c
 508:	00003914 	.word	0x00003914
 50c:	000039fe 	.word	0x000039fe
 510:	00003956 	.word	0x00003956

Disassembly of section .text.operation_sleep:

00000514 <operation_sleep>:
 * End of program sleep operation
 **********************************************/

static void operation_sleep( void ) {
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;
 514:	238c      	movs	r3, #140	; 0x8c
 516:	2200      	movs	r2, #0
static void operation_sleep( void ) {
 518:	b510      	push	{r4, lr}
    *GOC_DATA_IRQ = 0;
 51a:	601a      	str	r2, [r3, #0]

#ifdef USE_MRR
    if(radio_on) {
 51c:	4b04      	ldr	r3, [pc, #16]	; (530 <operation_sleep+0x1c>)
 51e:	881b      	ldrh	r3, [r3, #0]
 520:	4293      	cmp	r3, r2
 522:	d001      	beq.n	528 <operation_sleep+0x14>
        radio_power_off();
 524:	f7ff ff86 	bl	434 <radio_power_off>
    }
#endif

    mbus_sleep_all();
 528:	f7ff fe50 	bl	1cc <mbus_sleep_all>
    while(1);
 52c:	e7fe      	b.n	52c <operation_sleep+0x18>
 52e:	46c0      	nop			; (mov r8, r8)
 530:	000039fe 	.word	0x000039fe

Disassembly of section .text.xo_init:

00000534 <xo_init>:
void xo_init( void ) {
 534:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    prev21e_r19_t prev21e_r19 = PREv21E_R19_DEFAULT;
 536:	4b2c      	ldr	r3, [pc, #176]	; (5e8 <xo_init+0xb4>)
    *REG_XO_CONF2 = ((xo_cap_drv << 6) | (xo_cap_in << 0));
 538:	4a2c      	ldr	r2, [pc, #176]	; (5ec <xo_init+0xb8>)
    prev21e_r19_t prev21e_r19 = PREv21E_R19_DEFAULT;
 53a:	681c      	ldr	r4, [r3, #0]
    *REG_XO_CONF2 = ((xo_cap_drv << 6) | (xo_cap_in << 0));
 53c:	4b2c      	ldr	r3, [pc, #176]	; (5f0 <xo_init+0xbc>)
    *REG_XO_CONF1 = prev21e_r19.as_int;
 53e:	4d2d      	ldr	r5, [pc, #180]	; (5f4 <xo_init+0xc0>)
    *REG_XO_CONF2 = ((xo_cap_drv << 6) | (xo_cap_in << 0));
 540:	601a      	str	r2, [r3, #0]
    prev21e_r19.XO_SCN_ENB       = 0x1;
 542:	4b2d      	ldr	r3, [pc, #180]	; (5f8 <xo_init+0xc4>)
    delay(10000); // >= 1ms
 544:	4e2d      	ldr	r6, [pc, #180]	; (5fc <xo_init+0xc8>)
    prev21e_r19.XO_SCN_ENB       = 0x1;
 546:	401c      	ands	r4, r3
 548:	4b2d      	ldr	r3, [pc, #180]	; (600 <xo_init+0xcc>)
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 54a:	20a1      	movs	r0, #161	; 0xa1
    prev21e_r19.XO_SCN_ENB       = 0x1;
 54c:	431c      	orrs	r4, r3
    *REG_XO_CONF1 = prev21e_r19.as_int;
 54e:	602c      	str	r4, [r5, #0]
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 550:	6829      	ldr	r1, [r5, #0]
 552:	f7ff fe13 	bl	17c <mbus_write_message32>
    delay(10000); // >= 1ms
 556:	0030      	movs	r0, r6
 558:	f7ff fda6 	bl	a8 <delay>
    prev21e_r19.XO_ISOLATE = 0x0;
 55c:	4b29      	ldr	r3, [pc, #164]	; (604 <xo_init+0xd0>)
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 55e:	20a1      	movs	r0, #161	; 0xa1
    prev21e_r19.XO_ISOLATE = 0x0;
 560:	401c      	ands	r4, r3
    *REG_XO_CONF1 = prev21e_r19.as_int;
 562:	602c      	str	r4, [r5, #0]
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 564:	6829      	ldr	r1, [r5, #0]
 566:	f7ff fe09 	bl	17c <mbus_write_message32>
    delay(10000); // >= 1ms
 56a:	0030      	movs	r0, r6
 56c:	f7ff fd9c 	bl	a8 <delay>
    prev21e_r19.XO_DRV_START_UP = 0x1;
 570:	2380      	movs	r3, #128	; 0x80
 572:	431c      	orrs	r4, r3
    *REG_XO_CONF1 = prev21e_r19.as_int;
 574:	602c      	str	r4, [r5, #0]
    delay(30000); // >= 1s
 576:	4f24      	ldr	r7, [pc, #144]	; (608 <xo_init+0xd4>)
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 578:	6829      	ldr	r1, [r5, #0]
 57a:	20a1      	movs	r0, #161	; 0xa1
 57c:	f7ff fdfe 	bl	17c <mbus_write_message32>
    delay(30000); // >= 1s
 580:	0038      	movs	r0, r7
 582:	f7ff fd91 	bl	a8 <delay>
    prev21e_r19.XO_SCN_CLK_SEL = 0x1;
 586:	2302      	movs	r3, #2
 588:	431c      	orrs	r4, r3
    *REG_XO_CONF1 = prev21e_r19.as_int;
 58a:	602c      	str	r4, [r5, #0]
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 58c:	6829      	ldr	r1, [r5, #0]
 58e:	20a1      	movs	r0, #161	; 0xa1
 590:	f7ff fdf4 	bl	17c <mbus_write_message32>
    delay(3000); // >= 300us
 594:	481d      	ldr	r0, [pc, #116]	; (60c <xo_init+0xd8>)
 596:	f7ff fd87 	bl	a8 <delay>
    prev21e_r19.XO_SCN_ENB     = 0x0;
 59a:	2603      	movs	r6, #3
 59c:	0023      	movs	r3, r4
 59e:	22ff      	movs	r2, #255	; 0xff
 5a0:	43b3      	bics	r3, r6
 5a2:	b2db      	uxtb	r3, r3
 5a4:	4394      	bics	r4, r2
 5a6:	431c      	orrs	r4, r3
    *REG_XO_CONF1 = prev21e_r19.as_int;
 5a8:	602c      	str	r4, [r5, #0]
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 5aa:	6829      	ldr	r1, [r5, #0]
 5ac:	20a1      	movs	r0, #161	; 0xa1
 5ae:	f7ff fde5 	bl	17c <mbus_write_message32>
    delay(30000);  // >= 1s
 5b2:	0038      	movs	r0, r7
 5b4:	f7ff fd78 	bl	a8 <delay>
    prev21e_r19.XO_SCN_CLK_SEL  = 0x1;
 5b8:	223d      	movs	r2, #61	; 0x3d
 5ba:	2342      	movs	r3, #66	; 0x42
 5bc:	4022      	ands	r2, r4
 5be:	4313      	orrs	r3, r2
 5c0:	22ff      	movs	r2, #255	; 0xff
 5c2:	4394      	bics	r4, r2
 5c4:	431c      	orrs	r4, r3
    *REG_XO_CONF1 = prev21e_r19.as_int;
 5c6:	602c      	str	r4, [r5, #0]
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 5c8:	6829      	ldr	r1, [r5, #0]
 5ca:	20a1      	movs	r0, #161	; 0xa1
 5cc:	f7ff fdd6 	bl	17c <mbus_write_message32>
    enable_xo_timer();
 5d0:	f7ff fd8e 	bl	f0 <enable_xo_timer>
    reset_xo_cnt();
 5d4:	f7ff fdb4 	bl	140 <reset_xo_cnt>
    start_xo_cnt();
 5d8:	f7ff fdb8 	bl	14c <start_xo_cnt>
    mbus_write_message32(0xBA, 0x03);
 5dc:	0031      	movs	r1, r6
 5de:	20ba      	movs	r0, #186	; 0xba
 5e0:	f7ff fdcc 	bl	17c <mbus_write_message32>
}
 5e4:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 5e6:	46c0      	nop			; (mov r8, r8)
 5e8:	00003678 	.word	0x00003678
 5ec:	00000fff 	.word	0x00000fff
 5f0:	a0000068 	.word	0xa0000068
 5f4:	a0000064 	.word	0xa0000064
 5f8:	ffa00000 	.word	0xffa00000
 5fc:	00002710 	.word	0x00002710
 600:	0010c711 	.word	0x0010c711
 604:	ffdfffff 	.word	0xffdfffff
 608:	00007530 	.word	0x00007530
 60c:	00000bb8 	.word	0x00000bb8

Disassembly of section .text.divide_by_60:

00000610 <divide_by_60>:
uint32_t divide_by_60(uint32_t source) {
 610:	b530      	push	{r4, r5, lr}
 612:	0003      	movs	r3, r0
        uint32_t comp = res * 60;
 614:	253c      	movs	r5, #60	; 0x3c
    uint32_t upper = source >> 5;   // divide by 32
 616:	0941      	lsrs	r1, r0, #5
    uint32_t lower = source >> 6;   // divide by 64
 618:	0984      	lsrs	r4, r0, #6
        res = (upper + lower) >> 1;  // average of upper and lower
 61a:	0008      	movs	r0, r1
        uint32_t comp = res * 60;
 61c:	002a      	movs	r2, r5
 61e:	0001      	movs	r1, r0
        res = (upper + lower) >> 1;  // average of upper and lower
 620:	1900      	adds	r0, r0, r4
 622:	0840      	lsrs	r0, r0, #1
        uint32_t comp = res * 60;
 624:	4342      	muls	r2, r0
        if(source < comp) {
 626:	4293      	cmp	r3, r2
 628:	d3f8      	bcc.n	61c <divide_by_60+0xc>
        else if(source - comp < 60) {
 62a:	1a9a      	subs	r2, r3, r2
 62c:	2a3b      	cmp	r2, #59	; 0x3b
 62e:	d800      	bhi.n	632 <divide_by_60+0x22>
}
 630:	bd30      	pop	{r4, r5, pc}
        res = (upper + lower) >> 1;  // average of upper and lower
 632:	0004      	movs	r4, r0
 634:	e7f1      	b.n	61a <divide_by_60+0xa>

Disassembly of section .text.right_shift:

00000636 <right_shift>:
        for(i = 0; i < shift; i++) {
 636:	2300      	movs	r3, #0
uint64_t right_shift(uint64_t input, int8_t shift) {
 638:	b570      	push	{r4, r5, r6, lr}
 63a:	0004      	movs	r4, r0
 63c:	000d      	movs	r5, r1
    if(shift >= 0) {
 63e:	429a      	cmp	r2, r3
 640:	da0e      	bge.n	660 <right_shift+0x2a>
        input = input << 1;
 642:	3b01      	subs	r3, #1
 644:	b25b      	sxtb	r3, r3
 646:	1924      	adds	r4, r4, r4
 648:	416d      	adcs	r5, r5
        for(i = 0; i > shift; i--) {
 64a:	429a      	cmp	r2, r3
 64c:	d1f9      	bne.n	642 <right_shift+0xc>
 64e:	e009      	b.n	664 <right_shift+0x2e>
        input = input >> 1;
 650:	07ee      	lsls	r6, r5, #31
 652:	0860      	lsrs	r0, r4, #1
 654:	0869      	lsrs	r1, r5, #1
 656:	4306      	orrs	r6, r0
 658:	0034      	movs	r4, r6
 65a:	000d      	movs	r5, r1
        for(i = 0; i < shift; i++) {
 65c:	3301      	adds	r3, #1
 65e:	b25b      	sxtb	r3, r3
 660:	4293      	cmp	r3, r2
 662:	d1f5      	bne.n	650 <right_shift+0x1a>
}
 664:	0029      	movs	r1, r5
 666:	0020      	movs	r0, r4
 668:	bd70      	pop	{r4, r5, r6, pc}

Disassembly of section .text.lnt_stop:

0000066c <lnt_stop>:
    mbus_remote_register_write(LNT_ADDR,0x03,0xFFFFFF);
 66c:	2103      	movs	r1, #3
static void lnt_stop() {
 66e:	b510      	push	{r4, lr}
    lnt_sys_light = right_shift(((*REG1 & 0xFFFFFF) << 24) | (*REG0), lnt_counter_base); // >> lnt_counter_base;
 670:	2400      	movs	r4, #0
    mbus_remote_register_write(LNT_ADDR,0x03,0xFFFFFF);
 672:	0008      	movs	r0, r1
 674:	4a17      	ldr	r2, [pc, #92]	; (6d4 <lnt_stop+0x68>)
 676:	f7ff fdc0 	bl	1fa <mbus_remote_register_write>
    set_halt_until_mbus_trx();
 67a:	f7ff fd79 	bl	170 <set_halt_until_mbus_trx>
    mbus_copy_registers_from_remote_to_local(LNT_ADDR, 0x10, 0x00, 1);
 67e:	2301      	movs	r3, #1
 680:	2200      	movs	r2, #0
 682:	2110      	movs	r1, #16
 684:	2003      	movs	r0, #3
 686:	f7ff fda7 	bl	1d8 <mbus_copy_registers_from_remote_to_local>
    set_halt_until_mbus_tx();
 68a:	f7ff fd6b 	bl	164 <set_halt_until_mbus_tx>
    lnt_sys_light = right_shift(((*REG1 & 0xFFFFFF) << 24) | (*REG0), lnt_counter_base); // >> lnt_counter_base;
 68e:	4b12      	ldr	r3, [pc, #72]	; (6d8 <lnt_stop+0x6c>)
 690:	4a12      	ldr	r2, [pc, #72]	; (6dc <lnt_stop+0x70>)
 692:	6818      	ldr	r0, [r3, #0]
 694:	3b04      	subs	r3, #4
 696:	681b      	ldr	r3, [r3, #0]
 698:	8812      	ldrh	r2, [r2, #0]
 69a:	0600      	lsls	r0, r0, #24
 69c:	0021      	movs	r1, r4
 69e:	b252      	sxtb	r2, r2
 6a0:	4318      	orrs	r0, r3
 6a2:	f7ff ffc8 	bl	636 <right_shift>
 6a6:	0002      	movs	r2, r0
 6a8:	000b      	movs	r3, r1
 6aa:	490d      	ldr	r1, [pc, #52]	; (6e0 <lnt_stop+0x74>)
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
 6ac:	2003      	movs	r0, #3
    lnt_sys_light = right_shift(((*REG1 & 0xFFFFFF) << 24) | (*REG0), lnt_counter_base); // >> lnt_counter_base;
 6ae:	600a      	str	r2, [r1, #0]
 6b0:	604b      	str	r3, [r1, #4]
    lntv1a_r00.DBE_ENABLE = 0x0; // Default : 0x0
 6b2:	2110      	movs	r1, #16
 6b4:	4b0b      	ldr	r3, [pc, #44]	; (6e4 <lnt_stop+0x78>)
 6b6:	781a      	ldrb	r2, [r3, #0]
 6b8:	438a      	bics	r2, r1
 6ba:	701a      	strb	r2, [r3, #0]
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x0;
 6bc:	781a      	ldrb	r2, [r3, #0]
 6be:	3130      	adds	r1, #48	; 0x30
 6c0:	438a      	bics	r2, r1
 6c2:	701a      	strb	r2, [r3, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
 6c4:	0021      	movs	r1, r4
 6c6:	681a      	ldr	r2, [r3, #0]
 6c8:	f7ff fd97 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*100);
 6cc:	4806      	ldr	r0, [pc, #24]	; (6e8 <lnt_stop+0x7c>)
 6ce:	f7ff fceb 	bl	a8 <delay>
}
 6d2:	bd10      	pop	{r4, pc}
 6d4:	00ffffff 	.word	0x00ffffff
 6d8:	a0000004 	.word	0xa0000004
 6dc:	00003a3e 	.word	0x00003a3e
 6e0:	00003a48 	.word	0x00003a48
 6e4:	000038fc 	.word	0x000038fc
 6e8:	00002710 	.word	0x00002710

Disassembly of section .text.right_shift_arr.part.0:

000006ec <right_shift_arr.part.0>:
void right_shift_arr(uint32_t* arr, uint32_t data, uint16_t arr_len, int16_t shift_len) {
 6ec:	b5f0      	push	{r4, r5, r6, r7, lr}
 6ee:	b089      	sub	sp, #36	; 0x24
 6f0:	9003      	str	r0, [sp, #12]
 6f2:	9106      	str	r1, [sp, #24]
 6f4:	9302      	str	r3, [sp, #8]
    else if(shift_len >= 32) {
 6f6:	2b1f      	cmp	r3, #31
 6f8:	dc1a      	bgt.n	730 <right_shift_arr.part.0+0x44>
    int8_t sign = shift_len > 0? 1 : -1;
 6fa:	2301      	movs	r3, #1
 6fc:	9301      	str	r3, [sp, #4]
 6fe:	9b02      	ldr	r3, [sp, #8]
 700:	2b00      	cmp	r3, #0
 702:	dc05      	bgt.n	710 <right_shift_arr.part.0+0x24>
        shift_len = -shift_len;
 704:	425b      	negs	r3, r3
 706:	b21b      	sxth	r3, r3
 708:	9302      	str	r3, [sp, #8]
    int8_t sign = shift_len > 0? 1 : -1;
 70a:	2301      	movs	r3, #1
 70c:	425b      	negs	r3, r3
 70e:	9301      	str	r3, [sp, #4]
        back = 0;
 710:	2300      	movs	r3, #0
 712:	9304      	str	r3, [sp, #16]
    if(sign == 1) {
 714:	9b01      	ldr	r3, [sp, #4]
 716:	3a01      	subs	r2, #1
 718:	b294      	uxth	r4, r2
 71a:	2b01      	cmp	r3, #1
 71c:	d001      	beq.n	722 <right_shift_arr.part.0+0x36>
        back = arr_len - 1;
 71e:	9404      	str	r4, [sp, #16]
        start = 0;
 720:	2400      	movs	r4, #0
        arr[back] = 0;
 722:	9b04      	ldr	r3, [sp, #16]
 724:	2100      	movs	r1, #0
 726:	009e      	lsls	r6, r3, #2
 728:	9b03      	ldr	r3, [sp, #12]
 72a:	9d02      	ldr	r5, [sp, #8]
 72c:	199e      	adds	r6, r3, r6
 72e:	e011      	b.n	754 <right_shift_arr.part.0+0x68>
        data = 0;
 730:	2300      	movs	r3, #0
 732:	9306      	str	r3, [sp, #24]
    int8_t sign = shift_len > 0? 1 : -1;
 734:	3301      	adds	r3, #1
 736:	e7ea      	b.n	70e <right_shift_arr.part.0+0x22>
            arr[i] = arr[i - sign];
 738:	9a01      	ldr	r2, [sp, #4]
 73a:	9f03      	ldr	r7, [sp, #12]
 73c:	1a9a      	subs	r2, r3, r2
 73e:	0090      	lsls	r0, r2, #2
 740:	59c0      	ldr	r0, [r0, r7]
 742:	009b      	lsls	r3, r3, #2
 744:	51d8      	str	r0, [r3, r7]
        for(i = start; i != back; i -= sign) {
 746:	b293      	uxth	r3, r2
 748:	9a04      	ldr	r2, [sp, #16]
 74a:	4293      	cmp	r3, r2
 74c:	d1f4      	bne.n	738 <right_shift_arr.part.0+0x4c>
        arr[back] = 0;
 74e:	3d20      	subs	r5, #32
 750:	6031      	str	r1, [r6, #0]
        shift_len -= 32;
 752:	b22d      	sxth	r5, r5
    while(shift_len >= 32) {
 754:	2d1f      	cmp	r5, #31
 756:	dc16      	bgt.n	786 <right_shift_arr.part.0+0x9a>
    if(sign == -1) {
 758:	9a01      	ldr	r2, [sp, #4]
    int8_t shift_len_complement = shift_len - 32;
 75a:	b2eb      	uxtb	r3, r5
    if(sign == -1) {
 75c:	3201      	adds	r2, #1
 75e:	d014      	beq.n	78a <right_shift_arr.part.0+0x9e>
    int8_t shift_len_complement = shift_len - 32;
 760:	3b20      	subs	r3, #32
        shift_len_complement = -shift_len_complement;
 762:	b25b      	sxtb	r3, r3
 764:	9305      	str	r3, [sp, #20]
    for(i = start; i != back; i -= sign) {
 766:	b26d      	sxtb	r5, r5
 768:	9b04      	ldr	r3, [sp, #16]
 76a:	42a3      	cmp	r3, r4
 76c:	d112      	bne.n	794 <right_shift_arr.part.0+0xa8>
    arr[back] = right_shift(arr[back], shift_len);
 76e:	002a      	movs	r2, r5
 770:	2100      	movs	r1, #0
 772:	6830      	ldr	r0, [r6, #0]
 774:	f7ff ff5f 	bl	636 <right_shift>
    if(sign == -1) {
 778:	9b01      	ldr	r3, [sp, #4]
    arr[back] = right_shift(arr[back], shift_len);
 77a:	0004      	movs	r4, r0
    if(sign == -1) {
 77c:	3301      	adds	r3, #1
 77e:	d021      	beq.n	7c4 <right_shift_arr.part.0+0xd8>
        arr[back] |= data;
 780:	6034      	str	r4, [r6, #0]
}
 782:	b009      	add	sp, #36	; 0x24
 784:	bdf0      	pop	{r4, r5, r6, r7, pc}
        for(i = start; i != back; i -= sign) {
 786:	0023      	movs	r3, r4
 788:	e7de      	b.n	748 <right_shift_arr.part.0+0x5c>
        shift_len_complement = -shift_len_complement;
 78a:	2220      	movs	r2, #32
        shift_len = -shift_len;
 78c:	426d      	negs	r5, r5
 78e:	b22d      	sxth	r5, r5
        shift_len_complement = -shift_len_complement;
 790:	1ad3      	subs	r3, r2, r3
 792:	e7e6      	b.n	762 <right_shift_arr.part.0+0x76>
        arr[i] = right_shift(arr[i], shift_len);
 794:	9b03      	ldr	r3, [sp, #12]
 796:	00a7      	lsls	r7, r4, #2
 798:	19df      	adds	r7, r3, r7
 79a:	6838      	ldr	r0, [r7, #0]
 79c:	002a      	movs	r2, r5
 79e:	2100      	movs	r1, #0
 7a0:	f7ff ff49 	bl	636 <right_shift>
        arr[i] |= right_shift(arr[i - sign], shift_len_complement);
 7a4:	9b01      	ldr	r3, [sp, #4]
 7a6:	9a03      	ldr	r2, [sp, #12]
 7a8:	1ae4      	subs	r4, r4, r3
 7aa:	00a3      	lsls	r3, r4, #2
        arr[i] = right_shift(arr[i], shift_len);
 7ac:	6038      	str	r0, [r7, #0]
 7ae:	9007      	str	r0, [sp, #28]
        arr[i] |= right_shift(arr[i - sign], shift_len_complement);
 7b0:	2100      	movs	r1, #0
 7b2:	5898      	ldr	r0, [r3, r2]
 7b4:	9a05      	ldr	r2, [sp, #20]
 7b6:	f7ff ff3e 	bl	636 <right_shift>
 7ba:	9b07      	ldr	r3, [sp, #28]
    for(i = start; i != back; i -= sign) {
 7bc:	b2a4      	uxth	r4, r4
        arr[i] |= right_shift(arr[i - sign], shift_len_complement);
 7be:	4318      	orrs	r0, r3
 7c0:	6038      	str	r0, [r7, #0]
    for(i = start; i != back; i -= sign) {
 7c2:	e7d1      	b.n	768 <right_shift_arr.part.0+0x7c>
    data &= (right_shift(1, -shift_len) - 1);
 7c4:	9b02      	ldr	r3, [sp, #8]
 7c6:	2001      	movs	r0, #1
 7c8:	425a      	negs	r2, r3
 7ca:	2100      	movs	r1, #0
 7cc:	b252      	sxtb	r2, r2
 7ce:	f7ff ff32 	bl	636 <right_shift>
 7d2:	9b06      	ldr	r3, [sp, #24]
 7d4:	1e41      	subs	r1, r0, #1
 7d6:	4019      	ands	r1, r3
        arr[back] |= data;
 7d8:	430c      	orrs	r4, r1
 7da:	e7d1      	b.n	780 <right_shift_arr.part.0+0x94>

Disassembly of section .text.right_shift_arr:

000007dc <right_shift_arr>:
void right_shift_arr(uint32_t* arr, uint32_t data, uint16_t arr_len, int16_t shift_len) {
 7dc:	b510      	push	{r4, lr}
    if(shift_len == 0) {
 7de:	2b00      	cmp	r3, #0
 7e0:	d001      	beq.n	7e6 <right_shift_arr+0xa>
 7e2:	f7ff ff83 	bl	6ec <right_shift_arr.part.0>
}
 7e6:	bd10      	pop	{r4, pc}

Disassembly of section .text.mult:

000007e8 <mult>:
uint64_t mult(uint32_t lhs, uint16_t rhs) {
 7e8:	b530      	push	{r4, r5, lr}
 7ea:	000c      	movs	r4, r1
    res1 = (lhs >> 16) * rhs;
 7ec:	0c03      	lsrs	r3, r0, #16
    return right_shift(res1, -16) + res2;
 7ee:	435c      	muls	r4, r3
    res2 = (lhs & 0xFFFF) * rhs;
 7f0:	b282      	uxth	r2, r0
 7f2:	434a      	muls	r2, r1
    return right_shift(res1, -16) + res2;
 7f4:	0020      	movs	r0, r4
 7f6:	2100      	movs	r1, #0
 7f8:	2310      	movs	r3, #16
        input = input << 1;
 7fa:	3b01      	subs	r3, #1
 7fc:	b2db      	uxtb	r3, r3
 7fe:	1800      	adds	r0, r0, r0
 800:	4149      	adcs	r1, r1
        for(i = 0; i > shift; i--) {
 802:	2b00      	cmp	r3, #0
 804:	d1f9      	bne.n	7fa <mult+0x12>
    return right_shift(res1, -16) + res2;
 806:	001d      	movs	r5, r3
 808:	0014      	movs	r4, r2
 80a:	1900      	adds	r0, r0, r4
 80c:	4169      	adcs	r1, r5
}
 80e:	bd30      	pop	{r4, r5, pc}

Disassembly of section .text.get_timer_cnt_xo:

00000810 <get_timer_cnt_xo>:
    uint32_t raw_count = *XOT_VAL;
 810:	4b06      	ldr	r3, [pc, #24]	; (82c <get_timer_cnt_xo+0x1c>)
uint32_t get_timer_cnt_xo() {
 812:	b510      	push	{r4, lr}
    uint32_t raw_count = *XOT_VAL;
 814:	6818      	ldr	r0, [r3, #0]
    uint64_t temp = mult(raw_count, xo_to_sec_mplier);
 816:	4b06      	ldr	r3, [pc, #24]	; (830 <get_timer_cnt_xo+0x20>)
 818:	8819      	ldrh	r1, [r3, #0]
 81a:	b289      	uxth	r1, r1
 81c:	f7ff ffe4 	bl	7e8 <mult>
    return right_shift(temp, XO_TO_SEC_MPLIER_SHIFT);
 820:	4b04      	ldr	r3, [pc, #16]	; (834 <get_timer_cnt_xo+0x24>)
 822:	881a      	ldrh	r2, [r3, #0]
 824:	b252      	sxtb	r2, r2
 826:	f7ff ff06 	bl	636 <right_shift>
}
 82a:	bd10      	pop	{r4, pc}
 82c:	a0001420 	.word	0xa0001420
 830:	00003950 	.word	0x00003950
 834:	000038ec 	.word	0x000038ec

Disassembly of section .text.long_int_mult:

00000838 <long_int_mult>:
static void long_int_mult(const long_int lhs, const uint16_t rhs, long_int res) {
 838:	b5f0      	push	{r4, r5, r6, r7, lr}
 83a:	2500      	movs	r5, #0
 83c:	000e      	movs	r6, r1
 83e:	0014      	movs	r4, r2
    uint32_t carry_in = 0;
 840:	002f      	movs	r7, r5
static void long_int_mult(const long_int lhs, const uint16_t rhs, long_int res) {
 842:	b089      	sub	sp, #36	; 0x24
 844:	9003      	str	r0, [sp, #12]
        uint64_t temp = mult(lhs[i], rhs) + carry_in;
 846:	9b03      	ldr	r3, [sp, #12]
 848:	0031      	movs	r1, r6
 84a:	5958      	ldr	r0, [r3, r5]
 84c:	f7ff ffcc 	bl	7e8 <mult>
 850:	2300      	movs	r3, #0
 852:	9700      	str	r7, [sp, #0]
 854:	9301      	str	r3, [sp, #4]
 856:	9a00      	ldr	r2, [sp, #0]
 858:	9b01      	ldr	r3, [sp, #4]
 85a:	1880      	adds	r0, r0, r2
 85c:	4159      	adcs	r1, r3
        temp_res[i] = temp & 0xFFFFFFFF;
 85e:	ab04      	add	r3, sp, #16
 860:	50e8      	str	r0, [r5, r3]
 862:	3504      	adds	r5, #4
        carry_in = temp >> 32;
 864:	000f      	movs	r7, r1
    for(i = 0; i < LONG_INT_LEN; i++) {
 866:	2d10      	cmp	r5, #16
 868:	d1ed      	bne.n	846 <long_int_mult+0xe>
        dest[i] = src[i];
 86a:	9b04      	ldr	r3, [sp, #16]
 86c:	6023      	str	r3, [r4, #0]
 86e:	9b05      	ldr	r3, [sp, #20]
 870:	6063      	str	r3, [r4, #4]
 872:	9b06      	ldr	r3, [sp, #24]
 874:	60a3      	str	r3, [r4, #8]
 876:	9b07      	ldr	r3, [sp, #28]
 878:	60e3      	str	r3, [r4, #12]
}
 87a:	b009      	add	sp, #36	; 0x24
 87c:	bdf0      	pop	{r4, r5, r6, r7, pc}

Disassembly of section .text.log2:

00000880 <log2>:
    if(input == 0) { return 0; }
 880:	0002      	movs	r2, r0
 882:	2300      	movs	r3, #0
uint16_t log2(uint64_t input) {
 884:	b5f0      	push	{r4, r5, r6, r7, lr}
    if(input == 0) { return 0; }
 886:	430a      	orrs	r2, r1
uint16_t log2(uint64_t input) {
 888:	0006      	movs	r6, r0
 88a:	000f      	movs	r7, r1
    if(input == 0) { return 0; }
 88c:	0010      	movs	r0, r2
uint16_t log2(uint64_t input) {
 88e:	b08d      	sub	sp, #52	; 0x34
    if(input == 0) { return 0; }
 890:	429a      	cmp	r2, r3
 892:	d052      	beq.n	93a <log2+0xba>
    for(i = 47; i >= 0; i--) {
 894:	252f      	movs	r5, #47	; 0x2f
        if(right_shift(input, i) & 0b1) {
 896:	2401      	movs	r4, #1
        temp_result[i] = input_storage[i] = 0;
 898:	9300      	str	r3, [sp, #0]
 89a:	9301      	str	r3, [sp, #4]
 89c:	9306      	str	r3, [sp, #24]
 89e:	9302      	str	r3, [sp, #8]
 8a0:	9307      	str	r3, [sp, #28]
 8a2:	9303      	str	r3, [sp, #12]
    input_storage[0] = input & 0xFFFFFFFF;
 8a4:	9604      	str	r6, [sp, #16]
    input_storage[1] = input >> 32;
 8a6:	9105      	str	r1, [sp, #20]
        if(right_shift(input, i) & 0b1) {
 8a8:	002a      	movs	r2, r5
 8aa:	0030      	movs	r0, r6
 8ac:	0039      	movs	r1, r7
 8ae:	f7ff fec2 	bl	636 <right_shift>
 8b2:	0003      	movs	r3, r0
 8b4:	b2ea      	uxtb	r2, r5
 8b6:	4023      	ands	r3, r4
 8b8:	4220      	tst	r0, r4
 8ba:	d040      	beq.n	93e <log2+0xbe>
            uint64_t temp = right_shift(1, -i);
 8bc:	4252      	negs	r2, r2
 8be:	2001      	movs	r0, #1
 8c0:	2100      	movs	r1, #0
 8c2:	b252      	sxtb	r2, r2
 8c4:	f7ff feb7 	bl	636 <right_shift>
            out = i << LOG2_RES;
 8c8:	016d      	lsls	r5, r5, #5
            temp_result[0] = temp & 0xFFFFFFFF;
 8ca:	9000      	str	r0, [sp, #0]
            temp_result[1] = temp >> 32;
 8cc:	9101      	str	r1, [sp, #4]
            out = i << LOG2_RES;
 8ce:	b2ac      	uxth	r4, r5
        long_int_mult(input_storage, (1 << 15), input_storage);
 8d0:	2780      	movs	r7, #128	; 0x80
    uint16_t out = 0;
 8d2:	25fc      	movs	r5, #252	; 0xfc
 8d4:	4e21      	ldr	r6, [pc, #132]	; (95c <log2+0xdc>)
        long_int_mult(input_storage, (1 << 15), input_storage);
 8d6:	023f      	lsls	r7, r7, #8
        long_int_mult(temp_result, LOG_CONST_ARR[i], new_result);
 8d8:	4668      	mov	r0, sp
 8da:	8831      	ldrh	r1, [r6, #0]
 8dc:	aa08      	add	r2, sp, #32
 8de:	f7ff ffab 	bl	838 <long_int_mult>
        long_int_mult(input_storage, (1 << 15), input_storage);
 8e2:	aa04      	add	r2, sp, #16
 8e4:	0039      	movs	r1, r7
 8e6:	0010      	movs	r0, r2
 8e8:	f7ff ffa6 	bl	838 <long_int_mult>
    if(lhs[i] != rhs[i]) {
 8ec:	990b      	ldr	r1, [sp, #44]	; 0x2c
 8ee:	9b07      	ldr	r3, [sp, #28]
 8f0:	4299      	cmp	r1, r3
 8f2:	d12a      	bne.n	94a <log2+0xca>
 8f4:	9a0a      	ldr	r2, [sp, #40]	; 0x28
 8f6:	9b06      	ldr	r3, [sp, #24]
 8f8:	429a      	cmp	r2, r3
 8fa:	d107      	bne.n	90c <log2+0x8c>
 8fc:	9a09      	ldr	r2, [sp, #36]	; 0x24
 8fe:	9b05      	ldr	r3, [sp, #20]
 900:	429a      	cmp	r2, r3
 902:	d103      	bne.n	90c <log2+0x8c>
 904:	9a08      	ldr	r2, [sp, #32]
 906:	9b04      	ldr	r3, [sp, #16]
 908:	429a      	cmp	r2, r3
 90a:	d020      	beq.n	94e <log2+0xce>
        if(long_int_lte(new_result, input_storage)) {
 90c:	429a      	cmp	r2, r3
 90e:	d21e      	bcs.n	94e <log2+0xce>
        dest[i] = src[i];
 910:	9b08      	ldr	r3, [sp, #32]
 912:	9103      	str	r1, [sp, #12]
 914:	9300      	str	r3, [sp, #0]
 916:	9b09      	ldr	r3, [sp, #36]	; 0x24
        out |= right_shift(1, -(LOG2_RES - 1 - i));
 918:	2001      	movs	r0, #1
        dest[i] = src[i];
 91a:	9301      	str	r3, [sp, #4]
 91c:	9b0a      	ldr	r3, [sp, #40]	; 0x28
        out |= right_shift(1, -(LOG2_RES - 1 - i));
 91e:	2100      	movs	r1, #0
 920:	b26a      	sxtb	r2, r5
        dest[i] = src[i];
 922:	9302      	str	r3, [sp, #8]
        out |= right_shift(1, -(LOG2_RES - 1 - i));
 924:	f7ff fe87 	bl	636 <right_shift>
 928:	4304      	orrs	r4, r0
 92a:	b2a4      	uxth	r4, r4
    for(i = 0; i < LOG2_RES; i++) {
 92c:	3501      	adds	r5, #1
 92e:	b2ed      	uxtb	r5, r5
 930:	3602      	adds	r6, #2
 932:	2d01      	cmp	r5, #1
 934:	d1d0      	bne.n	8d8 <log2+0x58>
    return out & 0x7FF;
 936:	0560      	lsls	r0, r4, #21
 938:	0d40      	lsrs	r0, r0, #21
}
 93a:	b00d      	add	sp, #52	; 0x34
 93c:	bdf0      	pop	{r4, r5, r6, r7, pc}
    for(i = 47; i >= 0; i--) {
 93e:	3a01      	subs	r2, #1
 940:	b255      	sxtb	r5, r2
 942:	1c6a      	adds	r2, r5, #1
 944:	d1b0      	bne.n	8a8 <log2+0x28>
    uint16_t out = 0;
 946:	001c      	movs	r4, r3
 948:	e7c2      	b.n	8d0 <log2+0x50>
    if(lhs[i] != rhs[i]) {
 94a:	000a      	movs	r2, r1
 94c:	e7de      	b.n	90c <log2+0x8c>
            long_int_mult(temp_result, (1 << 15), temp_result);
 94e:	466a      	mov	r2, sp
 950:	0039      	movs	r1, r7
 952:	4668      	mov	r0, sp
 954:	f7ff ff70 	bl	838 <long_int_mult>
 958:	e7e8      	b.n	92c <log2+0xac>
 95a:	46c0      	nop			; (mov r8, r8)
 95c:	00003688 	.word	0x00003688

Disassembly of section .text.store_code:

00000960 <store_code>:
void store_code(int32_t code, uint16_t len) {
 960:	b510      	push	{r4, lr}
 962:	000c      	movs	r4, r1
    right_shift_arr(code_cache, code, CODE_CACHE_LEN, -len);
 964:	424b      	negs	r3, r1
 966:	2209      	movs	r2, #9
 968:	0001      	movs	r1, r0
 96a:	b21b      	sxth	r3, r3
 96c:	4803      	ldr	r0, [pc, #12]	; (97c <store_code+0x1c>)
 96e:	f7ff ff35 	bl	7dc <right_shift_arr>
    code_cache_remainder -= len;
 972:	4b03      	ldr	r3, [pc, #12]	; (980 <store_code+0x20>)
 974:	8819      	ldrh	r1, [r3, #0]
 976:	1b0c      	subs	r4, r1, r4
 978:	801c      	strh	r4, [r3, #0]
}
 97a:	bd10      	pop	{r4, pc}
 97c:	000039c4 	.word	0x000039c4
 980:	000038f0 	.word	0x000038f0

Disassembly of section .text.flush_code_cache:

00000984 <flush_code_cache>:
void flush_code_cache() {
 984:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    if(code_cache_remainder < CODE_CACHE_MAX_REMAINDER) {
 986:	2588      	movs	r5, #136	; 0x88
 988:	4c13      	ldr	r4, [pc, #76]	; (9d8 <flush_code_cache+0x54>)
 98a:	006d      	lsls	r5, r5, #1
 98c:	8823      	ldrh	r3, [r4, #0]
 98e:	42ab      	cmp	r3, r5
 990:	d221      	bcs.n	9d6 <flush_code_cache+0x52>
        right_shift_arr(code_cache, 0, CODE_CACHE_LEN, -code_cache_remainder);
 992:	4e12      	ldr	r6, [pc, #72]	; (9dc <flush_code_cache+0x58>)
 994:	425b      	negs	r3, r3
 996:	2209      	movs	r2, #9
 998:	2100      	movs	r1, #0
 99a:	0030      	movs	r0, r6
 99c:	b21b      	sxth	r3, r3
 99e:	f7ff ff1d 	bl	7dc <right_shift_arr>
        code_cache[0] &= 0x0000FFFF;    // FIXME: this line is not needed
 9a2:	6833      	ldr	r3, [r6, #0]
        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, code_cache, CODE_CACHE_LEN - 1);
 9a4:	4f0e      	ldr	r7, [pc, #56]	; (9e0 <flush_code_cache+0x5c>)
        code_cache[0] &= 0x0000FFFF;    // FIXME: this line is not needed
 9a6:	b29a      	uxth	r2, r3
        code_cache[0] |= 0x80000000;
 9a8:	2380      	movs	r3, #128	; 0x80
 9aa:	061b      	lsls	r3, r3, #24
 9ac:	4313      	orrs	r3, r2
 9ae:	6033      	str	r3, [r6, #0]
        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, code_cache, CODE_CACHE_LEN - 1);
 9b0:	0032      	movs	r2, r6
 9b2:	2308      	movs	r3, #8
 9b4:	2006      	movs	r0, #6
 9b6:	8839      	ldrh	r1, [r7, #0]
 9b8:	f7ff fc2c 	bl	214 <mbus_copy_mem_from_local_to_remote_bulk>
        code_addr += CODE_CACHE_LEN << 2;
 9bc:	883b      	ldrh	r3, [r7, #0]
        max_unit_count++;
 9be:	4a09      	ldr	r2, [pc, #36]	; (9e4 <flush_code_cache+0x60>)
        code_addr += CODE_CACHE_LEN << 2;
 9c0:	3324      	adds	r3, #36	; 0x24
 9c2:	803b      	strh	r3, [r7, #0]
        max_unit_count++;
 9c4:	8813      	ldrh	r3, [r2, #0]
        code_cache_remainder = CODE_CACHE_MAX_REMAINDER;
 9c6:	8025      	strh	r5, [r4, #0]
        max_unit_count++;
 9c8:	3301      	adds	r3, #1
 9ca:	8013      	strh	r3, [r2, #0]
        has_header = false;
 9cc:	2300      	movs	r3, #0
 9ce:	4a06      	ldr	r2, [pc, #24]	; (9e8 <flush_code_cache+0x64>)
 9d0:	7013      	strb	r3, [r2, #0]
        has_time = false;
 9d2:	4a06      	ldr	r2, [pc, #24]	; (9ec <flush_code_cache+0x68>)
 9d4:	7013      	strb	r3, [r2, #0]
}
 9d6:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 9d8:	000038f0 	.word	0x000038f0
 9dc:	000039c4 	.word	0x000039c4
 9e0:	00003a0a 	.word	0x00003a0a
 9e4:	00003a5a 	.word	0x00003a5a
 9e8:	00003a2c 	.word	0x00003a2c
 9ec:	00003a2d 	.word	0x00003a2d

Disassembly of section .text.store_to_code_cache:

000009f0 <store_to_code_cache>:
void store_to_code_cache(uint16_t log_light, uint16_t start_idx, uint32_t starting_time_in_min) {
 9f0:	b5f0      	push	{r4, r5, r6, r7, lr}
 9f2:	0005      	movs	r5, r0
    int16_t diff = log_light - last_log_light;
 9f4:	4b39      	ldr	r3, [pc, #228]	; (adc <store_to_code_cache+0xec>)
void store_to_code_cache(uint16_t log_light, uint16_t start_idx, uint32_t starting_time_in_min) {
 9f6:	0016      	movs	r6, r2
    int16_t diff = log_light - last_log_light;
 9f8:	8818      	ldrh	r0, [r3, #0]
void store_to_code_cache(uint16_t log_light, uint16_t start_idx, uint32_t starting_time_in_min) {
 9fa:	b085      	sub	sp, #20
    int16_t diff = log_light - last_log_light;
 9fc:	1a28      	subs	r0, r5, r0
 9fe:	b282      	uxth	r2, r0
    if(diff < 32 && diff >= -32) {
 a00:	0014      	movs	r4, r2
 a02:	3420      	adds	r4, #32
 a04:	b2a4      	uxth	r4, r4
void store_to_code_cache(uint16_t log_light, uint16_t start_idx, uint32_t starting_time_in_min) {
 a06:	9102      	str	r1, [sp, #8]
    int16_t diff = log_light - last_log_light;
 a08:	9201      	str	r2, [sp, #4]
    last_log_light = log_light;
 a0a:	801d      	strh	r5, [r3, #0]
    if(diff < 32 && diff >= -32) {
 a0c:	2c3f      	cmp	r4, #63	; 0x3f
 a0e:	d847      	bhi.n	aa0 <store_to_code_cache+0xb0>
        len_needed = light_code_lengths[code_idx];
 a10:	4b33      	ldr	r3, [pc, #204]	; (ae0 <store_to_code_cache+0xf0>)
 a12:	0062      	lsls	r2, r4, #1
 a14:	5ad3      	ldrh	r3, [r2, r3]
    code = light_diff_codes[code_idx];
 a16:	4a33      	ldr	r2, [pc, #204]	; (ae4 <store_to_code_cache+0xf4>)
 a18:	0061      	lsls	r1, r4, #1
 a1a:	5a8a      	ldrh	r2, [r1, r2]
    if(!has_time) {
 a1c:	4f32      	ldr	r7, [pc, #200]	; (ae8 <store_to_code_cache+0xf8>)
    code = light_diff_codes[code_idx];
 a1e:	9203      	str	r2, [sp, #12]
    if(!has_time) {
 a20:	783a      	ldrb	r2, [r7, #0]
 a22:	2a00      	cmp	r2, #0
 a24:	d101      	bne.n	a2a <store_to_code_cache+0x3a>
        len_needed += LIGHT_TIME_LENGTH;
 a26:	3311      	adds	r3, #17
 a28:	b29b      	uxth	r3, r3
    if(!has_header) {
 a2a:	4a30      	ldr	r2, [pc, #192]	; (aec <store_to_code_cache+0xfc>)
 a2c:	7812      	ldrb	r2, [r2, #0]
 a2e:	2a00      	cmp	r2, #0
 a30:	d101      	bne.n	a36 <store_to_code_cache+0x46>
        len_needed += UNIT_HEADER_SIZE;
 a32:	3309      	adds	r3, #9
 a34:	b29b      	uxth	r3, r3
    if(code_cache_remainder < len_needed) {
 a36:	4a2e      	ldr	r2, [pc, #184]	; (af0 <store_to_code_cache+0x100>)
 a38:	8812      	ldrh	r2, [r2, #0]
 a3a:	429a      	cmp	r2, r3
 a3c:	d201      	bcs.n	a42 <store_to_code_cache+0x52>
        flush_code_cache();
 a3e:	f7ff ffa1 	bl	984 <flush_code_cache>
    if(!has_time) {
 a42:	783b      	ldrb	r3, [r7, #0]
 a44:	2b00      	cmp	r3, #0
 a46:	d10a      	bne.n	a5e <store_to_code_cache+0x6e>
        store_code(starting_time_in_min & 0x1FFFF, 17);
 a48:	03f0      	lsls	r0, r6, #15
 a4a:	2111      	movs	r1, #17
 a4c:	0bc0      	lsrs	r0, r0, #15
 a4e:	f7ff ff87 	bl	960 <store_code>
        mbus_write_message32(0xC4, starting_time_in_min);
 a52:	0031      	movs	r1, r6
 a54:	20c4      	movs	r0, #196	; 0xc4
 a56:	f7ff fb91 	bl	17c <mbus_write_message32>
        has_time = true;
 a5a:	2301      	movs	r3, #1
 a5c:	703b      	strb	r3, [r7, #0]
    if(!has_header) {
 a5e:	4f23      	ldr	r7, [pc, #140]	; (aec <store_to_code_cache+0xfc>)
 a60:	783b      	ldrb	r3, [r7, #0]
 a62:	2b00      	cmp	r3, #0
 a64:	d112      	bne.n	a8c <store_to_code_cache+0x9c>
        store_code(day_state, 2);
 a66:	4e23      	ldr	r6, [pc, #140]	; (af4 <store_to_code_cache+0x104>)
 a68:	2102      	movs	r1, #2
 a6a:	8830      	ldrh	r0, [r6, #0]
 a6c:	b280      	uxth	r0, r0
 a6e:	f7ff ff77 	bl	960 <store_code>
        store_code(start_idx, 7);
 a72:	2107      	movs	r1, #7
 a74:	9802      	ldr	r0, [sp, #8]
 a76:	f7ff ff73 	bl	960 <store_code>
        mbus_write_message32(0xC5, (day_state << 16) | start_idx);
 a7a:	8831      	ldrh	r1, [r6, #0]
 a7c:	9b02      	ldr	r3, [sp, #8]
 a7e:	0409      	lsls	r1, r1, #16
 a80:	4319      	orrs	r1, r3
 a82:	20c5      	movs	r0, #197	; 0xc5
 a84:	f7ff fb7a 	bl	17c <mbus_write_message32>
        has_header = true;
 a88:	2301      	movs	r3, #1
 a8a:	703b      	strb	r3, [r7, #0]
    if(code_cache_remainder == CODE_CACHE_MAX_REMAINDER - UNIT_HEADER_SIZE - LIGHT_TIME_LENGTH) {
 a8c:	4b18      	ldr	r3, [pc, #96]	; (af0 <store_to_code_cache+0x100>)
 a8e:	881b      	ldrh	r3, [r3, #0]
 a90:	2bf6      	cmp	r3, #246	; 0xf6
 a92:	d113      	bne.n	abc <store_to_code_cache+0xcc>
        store_code(log_light, 11);
 a94:	210b      	movs	r1, #11
 a96:	0028      	movs	r0, r5
            store_code(diff, 9);
 a98:	f7ff ff62 	bl	960 <store_code>
}
 a9c:	b005      	add	sp, #20
 a9e:	bdf0      	pop	{r4, r5, r6, r7, pc}
    else if(diff < 256 && diff >= -256) {
 aa0:	2280      	movs	r2, #128	; 0x80
 aa2:	9b01      	ldr	r3, [sp, #4]
 aa4:	0092      	lsls	r2, r2, #2
 aa6:	3301      	adds	r3, #1
 aa8:	33ff      	adds	r3, #255	; 0xff
 aaa:	b29b      	uxth	r3, r3
 aac:	4293      	cmp	r3, r2
 aae:	d202      	bcs.n	ab6 <store_to_code_cache+0xc6>
        len_needed = light_code_lengths[code_idx] + 9;
 ab0:	230d      	movs	r3, #13
        code_idx = 64;
 ab2:	2440      	movs	r4, #64	; 0x40
 ab4:	e7af      	b.n	a16 <store_to_code_cache+0x26>
        len_needed = light_code_lengths[code_idx] + 11;
 ab6:	2312      	movs	r3, #18
        code_idx = 65;
 ab8:	2441      	movs	r4, #65	; 0x41
 aba:	e7ac      	b.n	a16 <store_to_code_cache+0x26>
        store_code(code, light_code_lengths[code_idx]);
 abc:	4b08      	ldr	r3, [pc, #32]	; (ae0 <store_to_code_cache+0xf0>)
 abe:	0062      	lsls	r2, r4, #1
 ac0:	5ad1      	ldrh	r1, [r2, r3]
 ac2:	9803      	ldr	r0, [sp, #12]
 ac4:	f7ff ff4c 	bl	960 <store_code>
        if(code_idx == 64) {
 ac8:	2c40      	cmp	r4, #64	; 0x40
 aca:	d104      	bne.n	ad6 <store_to_code_cache+0xe6>
            store_code(diff, 9);
 acc:	466b      	mov	r3, sp
 ace:	2109      	movs	r1, #9
 ad0:	2004      	movs	r0, #4
 ad2:	5e18      	ldrsh	r0, [r3, r0]
 ad4:	e7e0      	b.n	a98 <store_to_code_cache+0xa8>
        else if(code_idx == 65) {
 ad6:	2c41      	cmp	r4, #65	; 0x41
 ad8:	d1e0      	bne.n	a9c <store_to_code_cache+0xac>
 ada:	e7db      	b.n	a94 <store_to_code_cache+0xa4>
 adc:	00003a2e 	.word	0x00003a2e
 ae0:	0000369a 	.word	0x0000369a
 ae4:	00003720 	.word	0x00003720
 ae8:	00003a2d 	.word	0x00003a2d
 aec:	00003a2c 	.word	0x00003a2c
 af0:	000038f0 	.word	0x000038f0
 af4:	000038f8 	.word	0x000038f8

Disassembly of section .text.store_day_state_stop:

00000af8 <store_day_state_stop>:
    if(code_cache_remainder < len_needed) {
 af8:	4b06      	ldr	r3, [pc, #24]	; (b14 <store_day_state_stop+0x1c>)
void store_day_state_stop() {
 afa:	b510      	push	{r4, lr}
    if(code_cache_remainder < len_needed) {
 afc:	881b      	ldrh	r3, [r3, #0]
 afe:	2b05      	cmp	r3, #5
 b00:	d802      	bhi.n	b08 <store_day_state_stop+0x10>
        flush_code_cache();
 b02:	f7ff ff3f 	bl	984 <flush_code_cache>
}
 b06:	bd10      	pop	{r4, pc}
        store_code(light_diff_codes[66], len_needed);
 b08:	2106      	movs	r1, #6
 b0a:	2035      	movs	r0, #53	; 0x35
 b0c:	f7ff ff28 	bl	960 <store_code>
}
 b10:	e7f9      	b.n	b06 <store_day_state_stop+0xe>
 b12:	46c0      	nop			; (mov r8, r8)
 b14:	000038f0 	.word	0x000038f0

Disassembly of section .text.write_to_proc_cache:

00000b18 <write_to_proc_cache>:
void write_to_proc_cache(uint16_t data) {
 b18:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 b1a:	210b      	movs	r1, #11
 b1c:	2401      	movs	r4, #1
 b1e:	2500      	movs	r5, #0
        input = input << 1;
 b20:	3901      	subs	r1, #1
 b22:	b2c9      	uxtb	r1, r1
 b24:	1924      	adds	r4, r4, r4
 b26:	416d      	adcs	r5, r5
        for(i = 0; i > shift; i--) {
 b28:	2900      	cmp	r1, #0
 b2a:	d1f9      	bne.n	b20 <write_to_proc_cache+0x8>
    if(len >= proc_cache_remainder) {
 b2c:	4d10      	ldr	r5, [pc, #64]	; (b70 <write_to_proc_cache+0x58>)
    data &= (right_shift(1, -len) - 1);
 b2e:	3c01      	subs	r4, #1
    if(len >= proc_cache_remainder) {
 b30:	882b      	ldrh	r3, [r5, #0]
 b32:	4f10      	ldr	r7, [pc, #64]	; (b74 <write_to_proc_cache+0x5c>)
    data &= (right_shift(1, -len) - 1);
 b34:	4004      	ands	r4, r0
    if(len >= proc_cache_remainder) {
 b36:	2b0b      	cmp	r3, #11
 b38:	d80e      	bhi.n	b58 <write_to_proc_cache+0x40>
        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) cache_addr, proc_cache, PROC_CACHE_LEN - 1);
 b3a:	4e0f      	ldr	r6, [pc, #60]	; (b78 <write_to_proc_cache+0x60>)
 b3c:	2309      	movs	r3, #9
 b3e:	8831      	ldrh	r1, [r6, #0]
 b40:	003a      	movs	r2, r7
 b42:	2006      	movs	r0, #6
 b44:	b289      	uxth	r1, r1
 b46:	f7ff fb65 	bl	214 <mbus_copy_mem_from_local_to_remote_bulk>
        cache_addr += (PROC_CACHE_LEN << 2);
 b4a:	8833      	ldrh	r3, [r6, #0]
 b4c:	3328      	adds	r3, #40	; 0x28
 b4e:	b29b      	uxth	r3, r3
 b50:	8033      	strh	r3, [r6, #0]
        proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
 b52:	23a0      	movs	r3, #160	; 0xa0
 b54:	005b      	lsls	r3, r3, #1
 b56:	802b      	strh	r3, [r5, #0]
    if(shift_len == 0) {
 b58:	230b      	movs	r3, #11
 b5a:	0038      	movs	r0, r7
 b5c:	220a      	movs	r2, #10
 b5e:	0021      	movs	r1, r4
 b60:	425b      	negs	r3, r3
 b62:	f7ff fdc3 	bl	6ec <right_shift_arr.part.0>
    proc_cache_remainder -= len;
 b66:	882b      	ldrh	r3, [r5, #0]
 b68:	3b0b      	subs	r3, #11
 b6a:	b29b      	uxth	r3, r3
 b6c:	802b      	strh	r3, [r5, #0]
}
 b6e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 b70:	0000392c 	.word	0x0000392c
 b74:	0000398c 	.word	0x0000398c
 b78:	000038ee 	.word	0x000038ee

Disassembly of section .text.read_next_from_proc_cache:

00000b7c <read_next_from_proc_cache>:
    if(day_state == DUSK) {
 b7c:	4b2d      	ldr	r3, [pc, #180]	; (c34 <read_next_from_proc_cache+0xb8>)
uint16_t read_next_from_proc_cache() {
 b7e:	b573      	push	{r0, r1, r4, r5, r6, lr}
    if(day_state == DUSK) {
 b80:	881b      	ldrh	r3, [r3, #0]
 b82:	4d2d      	ldr	r5, [pc, #180]	; (c38 <read_next_from_proc_cache+0xbc>)
 b84:	4e2d      	ldr	r6, [pc, #180]	; (c3c <read_next_from_proc_cache+0xc0>)
 b86:	2b02      	cmp	r3, #2
 b88:	d126      	bne.n	bd8 <read_next_from_proc_cache+0x5c>
        if(proc_cache_remainder < 11) {
 b8a:	882b      	ldrh	r3, [r5, #0]
 b8c:	2b0a      	cmp	r3, #10
 b8e:	d814      	bhi.n	bba <read_next_from_proc_cache+0x3e>
            cache_addr -= PROC_CACHE_LEN << 2;
 b90:	4c2b      	ldr	r4, [pc, #172]	; (c40 <read_next_from_proc_cache+0xc4>)
 b92:	8823      	ldrh	r3, [r4, #0]
 b94:	3b28      	subs	r3, #40	; 0x28
 b96:	b29b      	uxth	r3, r3
 b98:	8023      	strh	r3, [r4, #0]
            set_halt_until_mbus_trx();
 b9a:	f7ff fae9 	bl	170 <set_halt_until_mbus_trx>
            mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) cache_addr, PRE_ADDR, proc_cache, PROC_CACHE_LEN - 1);
 b9e:	2309      	movs	r3, #9
 ba0:	8821      	ldrh	r1, [r4, #0]
 ba2:	2201      	movs	r2, #1
 ba4:	2006      	movs	r0, #6
 ba6:	b289      	uxth	r1, r1
 ba8:	9300      	str	r3, [sp, #0]
 baa:	0033      	movs	r3, r6
 bac:	f7ff fb4c 	bl	248 <mbus_copy_mem_from_remote_to_any_bulk>
            set_halt_until_mbus_tx();
 bb0:	f7ff fad8 	bl	164 <set_halt_until_mbus_tx>
            proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
 bb4:	23a0      	movs	r3, #160	; 0xa0
 bb6:	005b      	lsls	r3, r3, #1
 bb8:	802b      	strh	r3, [r5, #0]
        res = proc_cache[PROC_CACHE_LEN - 1] & 0x7FF;
 bba:	6a74      	ldr	r4, [r6, #36]	; 0x24
        proc_cache_remainder -= 11;
 bbc:	882b      	ldrh	r3, [r5, #0]
 bbe:	220a      	movs	r2, #10
 bc0:	3b0b      	subs	r3, #11
 bc2:	b29b      	uxth	r3, r3
 bc4:	802b      	strh	r3, [r5, #0]
 bc6:	2100      	movs	r1, #0
 bc8:	230b      	movs	r3, #11
        res = proc_cache[PROC_CACHE_LEN - 1] & 0x7FF;
 bca:	0564      	lsls	r4, r4, #21
 bcc:	0d64      	lsrs	r4, r4, #21
    if(shift_len == 0) {
 bce:	0030      	movs	r0, r6
 bd0:	f7ff fd8c 	bl	6ec <right_shift_arr.part.0>
}
 bd4:	0020      	movs	r0, r4
 bd6:	bd76      	pop	{r1, r2, r4, r5, r6, pc}
        if(proc_cache_remainder < 11) {
 bd8:	882b      	ldrh	r3, [r5, #0]
 bda:	2b0a      	cmp	r3, #10
 bdc:	d814      	bhi.n	c08 <read_next_from_proc_cache+0x8c>
            set_halt_until_mbus_trx();
 bde:	f7ff fac7 	bl	170 <set_halt_until_mbus_trx>
            mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) cache_addr, PRE_ADDR, proc_cache, PROC_CACHE_LEN - 1);
 be2:	2309      	movs	r3, #9
 be4:	4c16      	ldr	r4, [pc, #88]	; (c40 <read_next_from_proc_cache+0xc4>)
 be6:	2201      	movs	r2, #1
 be8:	8821      	ldrh	r1, [r4, #0]
 bea:	2006      	movs	r0, #6
 bec:	b289      	uxth	r1, r1
 bee:	9300      	str	r3, [sp, #0]
 bf0:	0033      	movs	r3, r6
 bf2:	f7ff fb29 	bl	248 <mbus_copy_mem_from_remote_to_any_bulk>
            set_halt_until_mbus_tx();
 bf6:	f7ff fab5 	bl	164 <set_halt_until_mbus_tx>
            cache_addr += PROC_CACHE_LEN << 2;
 bfa:	8823      	ldrh	r3, [r4, #0]
 bfc:	3328      	adds	r3, #40	; 0x28
 bfe:	b29b      	uxth	r3, r3
 c00:	8023      	strh	r3, [r4, #0]
            proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
 c02:	23a0      	movs	r3, #160	; 0xa0
 c04:	005b      	lsls	r3, r3, #1
 c06:	802b      	strh	r3, [r5, #0]
        res = right_shift(proc_cache[0], 20) & 0x7FF;
 c08:	2300      	movs	r3, #0
 c0a:	2114      	movs	r1, #20
 c0c:	6830      	ldr	r0, [r6, #0]
        input = input >> 1;
 c0e:	3901      	subs	r1, #1
 c10:	07da      	lsls	r2, r3, #31
 c12:	0840      	lsrs	r0, r0, #1
 c14:	b2c9      	uxtb	r1, r1
 c16:	4310      	orrs	r0, r2
 c18:	085b      	lsrs	r3, r3, #1
        for(i = 0; i < shift; i++) {
 c1a:	2900      	cmp	r1, #0
 c1c:	d1f7      	bne.n	c0e <read_next_from_proc_cache+0x92>
        proc_cache_remainder -= 11;
 c1e:	882b      	ldrh	r3, [r5, #0]
        res = right_shift(proc_cache[0], 20) & 0x7FF;
 c20:	0540      	lsls	r0, r0, #21
        proc_cache_remainder -= 11;
 c22:	3b0b      	subs	r3, #11
 c24:	b29b      	uxth	r3, r3
 c26:	802b      	strh	r3, [r5, #0]
 c28:	230b      	movs	r3, #11
 c2a:	220a      	movs	r2, #10
        res = right_shift(proc_cache[0], 20) & 0x7FF;
 c2c:	0d44      	lsrs	r4, r0, #21
    if(shift_len == 0) {
 c2e:	425b      	negs	r3, r3
 c30:	e7cd      	b.n	bce <read_next_from_proc_cache+0x52>
 c32:	46c0      	nop			; (mov r8, r8)
 c34:	000038f8 	.word	0x000038f8
 c38:	0000392c 	.word	0x0000392c
 c3c:	0000398c 	.word	0x0000398c
 c40:	000038ee 	.word	0x000038ee

Disassembly of section .text.check_new_edge:

00000c44 <check_new_edge>:
    if(day_state == DAWN) {
 c44:	4b10      	ldr	r3, [pc, #64]	; (c88 <check_new_edge+0x44>)
bool check_new_edge(uint32_t target) {
 c46:	0002      	movs	r2, r0
    if(day_state == DAWN) {
 c48:	881b      	ldrh	r3, [r3, #0]
 c4a:	b298      	uxth	r0, r3
 c4c:	2b00      	cmp	r3, #0
 c4e:	d10e      	bne.n	c6e <check_new_edge+0x2a>
        return (target > EDGE_MARGIN2 + XO_32_MIN) && (target < MID_DAY_TIME - EDGE_MARGIN1 - XO_32_MIN);
 c50:	21f0      	movs	r1, #240	; 0xf0
 c52:	4b0e      	ldr	r3, [pc, #56]	; (c8c <check_new_edge+0x48>)
 c54:	00c9      	lsls	r1, r1, #3
 c56:	881b      	ldrh	r3, [r3, #0]
 c58:	185b      	adds	r3, r3, r1
 c5a:	429a      	cmp	r2, r3
 c5c:	d906      	bls.n	c6c <check_new_edge+0x28>
 c5e:	4b0c      	ldr	r3, [pc, #48]	; (c90 <check_new_edge+0x4c>)
 c60:	8818      	ldrh	r0, [r3, #0]
 c62:	4b0c      	ldr	r3, [pc, #48]	; (c94 <check_new_edge+0x50>)
 c64:	1a18      	subs	r0, r3, r0
        return (target > MID_DAY_TIME + EDGE_MARGIN1 + XO_32_MIN) && (target < MAX_DAY_TIME - EDGE_MARGIN2 - XO_32_MIN);
 c66:	4282      	cmp	r2, r0
 c68:	4180      	sbcs	r0, r0
 c6a:	4240      	negs	r0, r0
}
 c6c:	4770      	bx	lr
        return (target > MID_DAY_TIME + EDGE_MARGIN1 + XO_32_MIN) && (target < MAX_DAY_TIME - EDGE_MARGIN2 - XO_32_MIN);
 c6e:	4b08      	ldr	r3, [pc, #32]	; (c90 <check_new_edge+0x4c>)
 c70:	4909      	ldr	r1, [pc, #36]	; (c98 <check_new_edge+0x54>)
 c72:	881b      	ldrh	r3, [r3, #0]
 c74:	2000      	movs	r0, #0
 c76:	185b      	adds	r3, r3, r1
 c78:	4293      	cmp	r3, r2
 c7a:	d2f7      	bcs.n	c6c <check_new_edge+0x28>
 c7c:	20a5      	movs	r0, #165	; 0xa5
 c7e:	4b03      	ldr	r3, [pc, #12]	; (c8c <check_new_edge+0x48>)
 c80:	0240      	lsls	r0, r0, #9
 c82:	881b      	ldrh	r3, [r3, #0]
 c84:	1ac0      	subs	r0, r0, r3
 c86:	e7ee      	b.n	c66 <check_new_edge+0x22>
 c88:	000038f8 	.word	0x000038f8
 c8c:	000037c6 	.word	0x000037c6
 c90:	000037c4 	.word	0x000037c4
 c94:	0000a140 	.word	0x0000a140
 c98:	0000b040 	.word	0x0000b040

Disassembly of section .text.set_new_state:

00000c9c <set_new_state>:
void set_new_state() {
 c9c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    cache_addr = CACHE_START_ADDR;
 c9e:	4a3f      	ldr	r2, [pc, #252]	; (d9c <set_new_state+0x100>)
 ca0:	4b3f      	ldr	r3, [pc, #252]	; (da0 <set_new_state+0x104>)
 ca2:	4e40      	ldr	r6, [pc, #256]	; (da4 <set_new_state+0x108>)
 ca4:	801a      	strh	r2, [r3, #0]
    proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
 ca6:	22a0      	movs	r2, #160	; 0xa0
 ca8:	4b3f      	ldr	r3, [pc, #252]	; (da8 <set_new_state+0x10c>)
 caa:	0052      	lsls	r2, r2, #1
 cac:	801a      	strh	r2, [r3, #0]
    max_idx = 0;
 cae:	2200      	movs	r2, #0
 cb0:	4b3e      	ldr	r3, [pc, #248]	; (dac <set_new_state+0x110>)
 cb2:	4c3f      	ldr	r4, [pc, #252]	; (db0 <set_new_state+0x114>)
 cb4:	801a      	strh	r2, [r3, #0]
    threshold_idx = IDX_INIT;
 cb6:	23ff      	movs	r3, #255	; 0xff
 cb8:	4a3e      	ldr	r2, [pc, #248]	; (db4 <set_new_state+0x118>)
 cba:	4d3f      	ldr	r5, [pc, #252]	; (db8 <set_new_state+0x11c>)
 cbc:	8013      	strh	r3, [r2, #0]
    min_light_idx = IDX_INIT;
 cbe:	4a3f      	ldr	r2, [pc, #252]	; (dbc <set_new_state+0x120>)
 cc0:	4f3f      	ldr	r7, [pc, #252]	; (dc0 <set_new_state+0x124>)
 cc2:	8013      	strh	r3, [r2, #0]
    sum = MAX_UINT16;
 cc4:	2201      	movs	r2, #1
 cc6:	4b3f      	ldr	r3, [pc, #252]	; (dc4 <set_new_state+0x128>)
 cc8:	4252      	negs	r2, r2
 cca:	801a      	strh	r2, [r3, #0]
    if(day_state == DAWN) {
 ccc:	4b3e      	ldr	r3, [pc, #248]	; (dc8 <set_new_state+0x12c>)
 cce:	881b      	ldrh	r3, [r3, #0]
 cd0:	2b00      	cmp	r3, #0
 cd2:	d129      	bne.n	d28 <set_new_state+0x8c>
        cur_edge = cur_sunrise;
 cd4:	4a3d      	ldr	r2, [pc, #244]	; (dcc <set_new_state+0x130>)
        day_state_start_time = cur_sunrise - EDGE_MARGIN2;
 cd6:	493e      	ldr	r1, [pc, #248]	; (dd0 <set_new_state+0x134>)
        cur_edge = cur_sunrise;
 cd8:	6813      	ldr	r3, [r2, #0]
 cda:	6033      	str	r3, [r6, #0]
        day_state_start_time = cur_sunrise - EDGE_MARGIN2;
 cdc:	6813      	ldr	r3, [r2, #0]
 cde:	8809      	ldrh	r1, [r1, #0]
 ce0:	1a5b      	subs	r3, r3, r1
 ce2:	6023      	str	r3, [r4, #0]
        day_state_end_time = cur_sunrise + EDGE_MARGIN1;
 ce4:	4b3b      	ldr	r3, [pc, #236]	; (dd4 <set_new_state+0x138>)
        day_state_end_time = cur_sunset + EDGE_MARGIN2;
 ce6:	881b      	ldrh	r3, [r3, #0]
 ce8:	6812      	ldr	r2, [r2, #0]
 cea:	189b      	adds	r3, r3, r2
    next_light_meas_time = day_state_start_time + xo_sys_time_in_sec - xo_day_time_in_sec;
 cec:	4a3a      	ldr	r2, [pc, #232]	; (dd8 <set_new_state+0x13c>)
            day_state_end_time = cur_sunrise - EDGE_MARGIN2 - XO_10_MIN;
 cee:	602b      	str	r3, [r5, #0]
    next_light_meas_time = day_state_start_time + xo_sys_time_in_sec - xo_day_time_in_sec;
 cf0:	6823      	ldr	r3, [r4, #0]
 cf2:	6811      	ldr	r1, [r2, #0]
 cf4:	683a      	ldr	r2, [r7, #0]
 cf6:	185b      	adds	r3, r3, r1
 cf8:	1a9b      	subs	r3, r3, r2
 cfa:	4a38      	ldr	r2, [pc, #224]	; (ddc <set_new_state+0x140>)
 cfc:	6013      	str	r3, [r2, #0]
    if(day_state_start_time < xo_day_time_in_sec) {
 cfe:	6821      	ldr	r1, [r4, #0]
 d00:	683b      	ldr	r3, [r7, #0]
 d02:	4299      	cmp	r1, r3
 d04:	d203      	bcs.n	d0e <set_new_state+0x72>
        next_light_meas_time += MAX_DAY_TIME;
 d06:	6813      	ldr	r3, [r2, #0]
 d08:	4935      	ldr	r1, [pc, #212]	; (de0 <set_new_state+0x144>)
 d0a:	185b      	adds	r3, r3, r1
 d0c:	6013      	str	r3, [r2, #0]
    light_meas_start_time_in_min = divide_by_60(next_light_meas_time - sys_sec_to_min_offset);
 d0e:	4b35      	ldr	r3, [pc, #212]	; (de4 <set_new_state+0x148>)
 d10:	6810      	ldr	r0, [r2, #0]
 d12:	681b      	ldr	r3, [r3, #0]
 d14:	1ac0      	subs	r0, r0, r3
 d16:	f7ff fc7b 	bl	610 <divide_by_60>
 d1a:	4b33      	ldr	r3, [pc, #204]	; (de8 <set_new_state+0x14c>)
 d1c:	6018      	str	r0, [r3, #0]
    mbus_write_message32(0xB8, light_meas_start_time_in_min);
 d1e:	6819      	ldr	r1, [r3, #0]
 d20:	20b8      	movs	r0, #184	; 0xb8
 d22:	f7ff fa2b 	bl	17c <mbus_write_message32>
}
 d26:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    else if(day_state == DUSK) {
 d28:	4b27      	ldr	r3, [pc, #156]	; (dc8 <set_new_state+0x12c>)
 d2a:	881b      	ldrh	r3, [r3, #0]
 d2c:	2b02      	cmp	r3, #2
 d2e:	d109      	bne.n	d44 <set_new_state+0xa8>
        cur_edge = cur_sunset;
 d30:	4a2e      	ldr	r2, [pc, #184]	; (dec <set_new_state+0x150>)
        day_state_start_time = cur_sunset - EDGE_MARGIN1;
 d32:	4928      	ldr	r1, [pc, #160]	; (dd4 <set_new_state+0x138>)
        cur_edge = cur_sunset;
 d34:	6813      	ldr	r3, [r2, #0]
 d36:	6033      	str	r3, [r6, #0]
        day_state_start_time = cur_sunset - EDGE_MARGIN1;
 d38:	6813      	ldr	r3, [r2, #0]
 d3a:	8809      	ldrh	r1, [r1, #0]
 d3c:	1a5b      	subs	r3, r3, r1
 d3e:	6023      	str	r3, [r4, #0]
        day_state_end_time = cur_sunset + EDGE_MARGIN2;
 d40:	4b23      	ldr	r3, [pc, #140]	; (dd0 <set_new_state+0x134>)
 d42:	e7d0      	b.n	ce6 <set_new_state+0x4a>
        uint32_t day_time_in_min = divide_by_60(xo_day_time_in_sec);
 d44:	6838      	ldr	r0, [r7, #0]
 d46:	f7ff fc63 	bl	610 <divide_by_60>
        cur_edge = day_time_in_min * 60 + XO_32_MIN;
 d4a:	233c      	movs	r3, #60	; 0x3c
 d4c:	22f0      	movs	r2, #240	; 0xf0
 d4e:	4343      	muls	r3, r0
 d50:	00d2      	lsls	r2, r2, #3
 d52:	189b      	adds	r3, r3, r2
 d54:	6033      	str	r3, [r6, #0]
        day_state_start_time = cur_edge;
 d56:	6833      	ldr	r3, [r6, #0]
 d58:	6023      	str	r3, [r4, #0]
        if(day_state == NOON) {
 d5a:	4b1b      	ldr	r3, [pc, #108]	; (dc8 <set_new_state+0x12c>)
 d5c:	881b      	ldrh	r3, [r3, #0]
 d5e:	2b01      	cmp	r3, #1
 d60:	d10f      	bne.n	d82 <set_new_state+0xe6>
            cur_sunset = next_sunset == 0? cur_sunset : next_sunset;
 d62:	4a23      	ldr	r2, [pc, #140]	; (df0 <set_new_state+0x154>)
 d64:	4b21      	ldr	r3, [pc, #132]	; (dec <set_new_state+0x150>)
 d66:	6811      	ldr	r1, [r2, #0]
 d68:	2900      	cmp	r1, #0
 d6a:	d108      	bne.n	d7e <set_new_state+0xe2>
 d6c:	681a      	ldr	r2, [r3, #0]
 d6e:	601a      	str	r2, [r3, #0]
            day_state_end_time = cur_sunset - EDGE_MARGIN1 - XO_10_MIN;
 d70:	681b      	ldr	r3, [r3, #0]
 d72:	4a18      	ldr	r2, [pc, #96]	; (dd4 <set_new_state+0x138>)
            day_state_end_time = cur_sunrise - EDGE_MARGIN2 - XO_10_MIN;
 d74:	491f      	ldr	r1, [pc, #124]	; (df4 <set_new_state+0x158>)
 d76:	8812      	ldrh	r2, [r2, #0]
 d78:	185b      	adds	r3, r3, r1
 d7a:	1a9b      	subs	r3, r3, r2
 d7c:	e7b6      	b.n	cec <set_new_state+0x50>
            cur_sunset = next_sunset == 0? cur_sunset : next_sunset;
 d7e:	6812      	ldr	r2, [r2, #0]
 d80:	e7f5      	b.n	d6e <set_new_state+0xd2>
            cur_sunrise = next_sunrise == 0? cur_sunrise : next_sunrise;
 d82:	4a1d      	ldr	r2, [pc, #116]	; (df8 <set_new_state+0x15c>)
 d84:	4b11      	ldr	r3, [pc, #68]	; (dcc <set_new_state+0x130>)
 d86:	6811      	ldr	r1, [r2, #0]
 d88:	2900      	cmp	r1, #0
 d8a:	d104      	bne.n	d96 <set_new_state+0xfa>
 d8c:	681a      	ldr	r2, [r3, #0]
 d8e:	601a      	str	r2, [r3, #0]
            day_state_end_time = cur_sunrise - EDGE_MARGIN2 - XO_10_MIN;
 d90:	681b      	ldr	r3, [r3, #0]
 d92:	4a0f      	ldr	r2, [pc, #60]	; (dd0 <set_new_state+0x134>)
 d94:	e7ee      	b.n	d74 <set_new_state+0xd8>
            cur_sunrise = next_sunrise == 0? cur_sunrise : next_sunrise;
 d96:	6812      	ldr	r2, [r2, #0]
 d98:	e7f9      	b.n	d8e <set_new_state+0xf2>
 d9a:	46c0      	nop			; (mov r8, r8)
 d9c:	00003e98 	.word	0x00003e98
 da0:	000038ee 	.word	0x000038ee
 da4:	00003a0c 	.word	0x00003a0c
 da8:	0000392c 	.word	0x0000392c
 dac:	00003a58 	.word	0x00003a58
 db0:	0000395c 	.word	0x0000395c
 db4:	0000394c 	.word	0x0000394c
 db8:	000039e8 	.word	0x000039e8
 dbc:	00003906 	.word	0x00003906
 dc0:	00003ae4 	.word	0x00003ae4
 dc4:	00003948 	.word	0x00003948
 dc8:	000038f8 	.word	0x000038f8
 dcc:	00003a10 	.word	0x00003a10
 dd0:	000037c6 	.word	0x000037c6
 dd4:	000037c4 	.word	0x000037c4
 dd8:	00003aec 	.word	0x00003aec
 ddc:	00003a70 	.word	0x00003a70
 de0:	00015180 	.word	0x00015180
 de4:	00003adc 	.word	0x00003adc
 de8:	00003a38 	.word	0x00003a38
 dec:	00003a14 	.word	0x00003a14
 df0:	00003a7c 	.word	0x00003a7c
 df4:	fffffda8 	.word	0xfffffda8
 df8:	00003a78 	.word	0x00003a78

Disassembly of section .text.sample_light:

00000dfc <sample_light>:
void sample_light() {
     dfc:	b5f0      	push	{r4, r5, r6, r7, lr}
    uint16_t log_light = log2(lnt_sys_light);
     dfe:	4bb8      	ldr	r3, [pc, #736]	; (10e0 <sample_light+0x2e4>)
void sample_light() {
     e00:	b085      	sub	sp, #20
    uint16_t log_light = log2(lnt_sys_light);
     e02:	6818      	ldr	r0, [r3, #0]
     e04:	6859      	ldr	r1, [r3, #4]
     e06:	f7ff fd3b 	bl	880 <log2>
    uint16_t last_avg_light = avg_light;
     e0a:	4bb6      	ldr	r3, [pc, #728]	; (10e4 <sample_light+0x2e8>)
    uint16_t log_light = log2(lnt_sys_light);
     e0c:	0007      	movs	r7, r0
    uint16_t last_avg_light = avg_light;
     e0e:	881d      	ldrh	r5, [r3, #0]
    uint16_t log_light = log2(lnt_sys_light);
     e10:	0004      	movs	r4, r0
    write_to_proc_cache(log_light);
     e12:	f7ff fe81 	bl	b18 <write_to_proc_cache>
    if(sum == MAX_UINT16) {
     e16:	4bb4      	ldr	r3, [pc, #720]	; (10e8 <sample_light+0x2ec>)
     e18:	4ab4      	ldr	r2, [pc, #720]	; (10ec <sample_light+0x2f0>)
     e1a:	8818      	ldrh	r0, [r3, #0]
     e1c:	49b4      	ldr	r1, [pc, #720]	; (10f0 <sample_light+0x2f4>)
    uint16_t last_avg_light = avg_light;
     e1e:	b2ad      	uxth	r5, r5
    if(sum == MAX_UINT16) {
     e20:	4290      	cmp	r0, r2
     e22:	d000      	beq.n	e26 <sample_light+0x2a>
     e24:	e0da      	b.n	fdc <sample_light+0x1e0>
            running_avg_time[i] = xo_day_time_in_sec;
     e26:	48b3      	ldr	r0, [pc, #716]	; (10f4 <sample_light+0x2f8>)
     e28:	2200      	movs	r2, #0
     e2a:	4684      	mov	ip, r0
     e2c:	4666      	mov	r6, ip
            running_avg[i] = log_light;
     e2e:	0050      	lsls	r0, r2, #1
     e30:	520c      	strh	r4, [r1, r0]
            running_avg_time[i] = xo_day_time_in_sec;
     e32:	48b1      	ldr	r0, [pc, #708]	; (10f8 <sample_light+0x2fc>)
     e34:	6805      	ldr	r5, [r0, #0]
     e36:	0090      	lsls	r0, r2, #2
     e38:	3201      	adds	r2, #1
     e3a:	5185      	str	r5, [r0, r6]
        for(i = 0; i < 8; i++) {
     e3c:	2a08      	cmp	r2, #8
     e3e:	d1f5      	bne.n	e2c <sample_light+0x30>
        sum = log_light << 3;
     e40:	00e4      	lsls	r4, r4, #3
     e42:	b2a4      	uxth	r4, r4
     e44:	801c      	strh	r4, [r3, #0]
    avg_light = sum >> 3;
     e46:	881b      	ldrh	r3, [r3, #0]
     e48:	4aa6      	ldr	r2, [pc, #664]	; (10e4 <sample_light+0x2e8>)
     e4a:	08db      	lsrs	r3, r3, #3
     e4c:	8013      	strh	r3, [r2, #0]
    if(min_light_idx == IDX_INIT || avg_light < min_light
     e4e:	4bab      	ldr	r3, [pc, #684]	; (10fc <sample_light+0x300>)
     e50:	4cab      	ldr	r4, [pc, #684]	; (1100 <sample_light+0x304>)
     e52:	881b      	ldrh	r3, [r3, #0]
     e54:	2bff      	cmp	r3, #255	; 0xff
     e56:	d00d      	beq.n	e74 <sample_light+0x78>
     e58:	4baa      	ldr	r3, [pc, #680]	; (1104 <sample_light+0x308>)
     e5a:	8812      	ldrh	r2, [r2, #0]
     e5c:	881b      	ldrh	r3, [r3, #0]
     e5e:	429a      	cmp	r2, r3
     e60:	d308      	bcc.n	e74 <sample_light+0x78>
            || (day_state == DAWN && avg_light == min_light)) { // find last min_light at dawn
     e62:	8823      	ldrh	r3, [r4, #0]
     e64:	2b00      	cmp	r3, #0
     e66:	d10f      	bne.n	e88 <sample_light+0x8c>
     e68:	4b9e      	ldr	r3, [pc, #632]	; (10e4 <sample_light+0x2e8>)
     e6a:	881a      	ldrh	r2, [r3, #0]
     e6c:	4ba5      	ldr	r3, [pc, #660]	; (1104 <sample_light+0x308>)
     e6e:	881b      	ldrh	r3, [r3, #0]
     e70:	429a      	cmp	r2, r3
     e72:	d109      	bne.n	e88 <sample_light+0x8c>
        min_light = avg_light;
     e74:	4b9b      	ldr	r3, [pc, #620]	; (10e4 <sample_light+0x2e8>)
     e76:	4aa3      	ldr	r2, [pc, #652]	; (1104 <sample_light+0x308>)
     e78:	881b      	ldrh	r3, [r3, #0]
     e7a:	b29b      	uxth	r3, r3
     e7c:	8013      	strh	r3, [r2, #0]
        min_light_idx = max_idx;
     e7e:	4ba2      	ldr	r3, [pc, #648]	; (1108 <sample_light+0x30c>)
     e80:	4a9e      	ldr	r2, [pc, #632]	; (10fc <sample_light+0x300>)
     e82:	881b      	ldrh	r3, [r3, #0]
     e84:	b29b      	uxth	r3, r3
     e86:	8013      	strh	r3, [r2, #0]
    uint32_t light_meas_end_time_in_min = divide_by_60(next_light_meas_time - sys_sec_to_min_offset);
     e88:	4da0      	ldr	r5, [pc, #640]	; (110c <sample_light+0x310>)
     e8a:	4ba1      	ldr	r3, [pc, #644]	; (1110 <sample_light+0x314>)
     e8c:	6828      	ldr	r0, [r5, #0]
     e8e:	681b      	ldr	r3, [r3, #0]
     e90:	1ac0      	subs	r0, r0, r3
     e92:	f7ff fbbd 	bl	610 <divide_by_60>
    if(day_state == DAWN || day_state == DUSK) {
     e96:	8823      	ldrh	r3, [r4, #0]
    uint32_t light_meas_end_time_in_min = divide_by_60(next_light_meas_time - sys_sec_to_min_offset);
     e98:	9001      	str	r0, [sp, #4]
    if(day_state == DAWN || day_state == DUSK) {
     e9a:	2b00      	cmp	r3, #0
     e9c:	d003      	beq.n	ea6 <sample_light+0xaa>
     e9e:	8823      	ldrh	r3, [r4, #0]
     ea0:	2b02      	cmp	r3, #2
     ea2:	d000      	beq.n	ea6 <sample_light+0xaa>
     ea4:	e0b6      	b.n	1014 <sample_light+0x218>
        next_light_meas_time += XO_1_MIN;
     ea6:	682b      	ldr	r3, [r5, #0]
     ea8:	333c      	adds	r3, #60	; 0x3c
        next_light_meas_time += XO_32_MIN;
     eaa:	602b      	str	r3, [r5, #0]
    uint32_t temp = xo_day_time_in_sec + next_light_meas_time - xo_sys_time_in_sec;
     eac:	4b92      	ldr	r3, [pc, #584]	; (10f8 <sample_light+0x2fc>)
     eae:	4a99      	ldr	r2, [pc, #612]	; (1114 <sample_light+0x318>)
     eb0:	681b      	ldr	r3, [r3, #0]
     eb2:	6829      	ldr	r1, [r5, #0]
     eb4:	6812      	ldr	r2, [r2, #0]
     eb6:	185b      	adds	r3, r3, r1
     eb8:	1a9b      	subs	r3, r3, r2
    if(day_state != NIGHT) {
     eba:	8821      	ldrh	r1, [r4, #0]
     ebc:	4a96      	ldr	r2, [pc, #600]	; (1118 <sample_light+0x31c>)
     ebe:	2600      	movs	r6, #0
        new_state = (temp >= day_state_end_time);
     ec0:	6812      	ldr	r2, [r2, #0]
    if(day_state != NIGHT) {
     ec2:	2903      	cmp	r1, #3
     ec4:	d100      	bne.n	ec8 <sample_light+0xcc>
     ec6:	e0aa      	b.n	101e <sample_light+0x222>
        new_state = (temp >= day_state_end_time);
     ec8:	4293      	cmp	r3, r2
     eca:	4176      	adcs	r6, r6
     ecc:	b2f6      	uxtb	r6, r6
    if(day_state == DAWN && threshold_idx == IDX_INIT) { 
     ece:	8823      	ldrh	r3, [r4, #0]
     ed0:	2b00      	cmp	r3, #0
     ed2:	d000      	beq.n	ed6 <sample_light+0xda>
     ed4:	e0bd      	b.n	1052 <sample_light+0x256>
     ed6:	4a91      	ldr	r2, [pc, #580]	; (111c <sample_light+0x320>)
     ed8:	8813      	ldrh	r3, [r2, #0]
     eda:	2bff      	cmp	r3, #255	; 0xff
     edc:	d000      	beq.n	ee0 <sample_light+0xe4>
     ede:	e0b8      	b.n	1052 <sample_light+0x256>
        if(avg_light >= EDGE_THRESHOLD && last_avg_light < EDGE_THRESHOLD) {
     ee0:	4b80      	ldr	r3, [pc, #512]	; (10e4 <sample_light+0x2e8>)
     ee2:	8818      	ldrh	r0, [r3, #0]
     ee4:	4b8e      	ldr	r3, [pc, #568]	; (1120 <sample_light+0x324>)
     ee6:	8819      	ldrh	r1, [r3, #0]
     ee8:	4288      	cmp	r0, r1
     eea:	d200      	bcs.n	eee <sample_light+0xf2>
     eec:	e09e      	b.n	102c <sample_light+0x230>
     eee:	8819      	ldrh	r1, [r3, #0]
     ef0:	42b9      	cmp	r1, r7
     ef2:	d800      	bhi.n	ef6 <sample_light+0xfa>
     ef4:	e09a      	b.n	102c <sample_light+0x230>
            threshold_idx = max_idx;
     ef6:	4b84      	ldr	r3, [pc, #528]	; (1108 <sample_light+0x30c>)
     ef8:	881b      	ldrh	r3, [r3, #0]
     efa:	b29b      	uxth	r3, r3
            threshold_idx = max_idx;
     efc:	8013      	strh	r3, [r2, #0]
            target = running_avg_time[(rot_idx + 3) & 7];
     efe:	2207      	movs	r2, #7
     f00:	4b88      	ldr	r3, [pc, #544]	; (1124 <sample_light+0x328>)
     f02:	881b      	ldrh	r3, [r3, #0]
     f04:	3303      	adds	r3, #3
     f06:	4013      	ands	r3, r2
     f08:	4a7a      	ldr	r2, [pc, #488]	; (10f4 <sample_light+0x2f8>)
     f0a:	009b      	lsls	r3, r3, #2
     f0c:	589d      	ldr	r5, [r3, r2]
    if(check_new_edge(target)) {
     f0e:	0028      	movs	r0, r5
     f10:	f7ff fe98 	bl	c44 <check_new_edge>
     f14:	2800      	cmp	r0, #0
     f16:	d01c      	beq.n	f52 <sample_light+0x156>
        if(target > cur_edge + MAX_EDGE_SHIFT) {
     f18:	4b83      	ldr	r3, [pc, #524]	; (1128 <sample_light+0x32c>)
     f1a:	4a84      	ldr	r2, [pc, #528]	; (112c <sample_light+0x330>)
     f1c:	8819      	ldrh	r1, [r3, #0]
     f1e:	6810      	ldr	r0, [r2, #0]
     f20:	1809      	adds	r1, r1, r0
     f22:	42a9      	cmp	r1, r5
     f24:	d300      	bcc.n	f28 <sample_light+0x12c>
     f26:	e0bd      	b.n	10a4 <sample_light+0x2a8>
            target = cur_edge + MAX_EDGE_SHIFT;
     f28:	881d      	ldrh	r5, [r3, #0]
     f2a:	6813      	ldr	r3, [r2, #0]
     f2c:	18ed      	adds	r5, r5, r3
        if(day_state == DAWN && target > 0 && target < MID_DAY_TIME 
     f2e:	8823      	ldrh	r3, [r4, #0]
     f30:	2b00      	cmp	r3, #0
     f32:	d000      	beq.n	f36 <sample_light+0x13a>
     f34:	e0c0      	b.n	10b8 <sample_light+0x2bc>
     f36:	4a7e      	ldr	r2, [pc, #504]	; (1130 <sample_light+0x334>)
     f38:	1e6b      	subs	r3, r5, #1
     f3a:	4293      	cmp	r3, r2
     f3c:	d900      	bls.n	f40 <sample_light+0x144>
     f3e:	e0bb      	b.n	10b8 <sample_light+0x2bc>
                && (cur_sunset - target > XO_270_MIN)) {
     f40:	4b7c      	ldr	r3, [pc, #496]	; (1134 <sample_light+0x338>)
     f42:	4a7d      	ldr	r2, [pc, #500]	; (1138 <sample_light+0x33c>)
     f44:	681b      	ldr	r3, [r3, #0]
     f46:	1b5b      	subs	r3, r3, r5
     f48:	4293      	cmp	r3, r2
     f4a:	d800      	bhi.n	f4e <sample_light+0x152>
     f4c:	e0b4      	b.n	10b8 <sample_light+0x2bc>
            next_sunrise = target;
     f4e:	4b7b      	ldr	r3, [pc, #492]	; (113c <sample_light+0x340>)
            next_sunset = target;
     f50:	601d      	str	r5, [r3, #0]
    if(new_state) {
     f52:	2e00      	cmp	r6, #0
     f54:	d100      	bne.n	f58 <sample_light+0x15c>
     f56:	e177      	b.n	1248 <sample_light+0x44c>
        *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
     f58:	2500      	movs	r5, #0
     f5a:	4b79      	ldr	r3, [pc, #484]	; (1140 <sample_light+0x344>)
     f5c:	601d      	str	r5, [r3, #0]
        *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
     f5e:	4b79      	ldr	r3, [pc, #484]	; (1144 <sample_light+0x348>)
     f60:	601d      	str	r5, [r3, #0]
        if(day_state != NIGHT) {
     f62:	8823      	ldrh	r3, [r4, #0]
     f64:	2b03      	cmp	r3, #3
     f66:	d02f      	beq.n	fc8 <sample_light+0x1cc>
            if(day_state == DUSK) {
     f68:	8823      	ldrh	r3, [r4, #0]
     f6a:	4e77      	ldr	r6, [pc, #476]	; (1148 <sample_light+0x34c>)
                proc_cache_remainder = PROC_CACHE_MAX_REMAINDER - proc_cache_remainder;
     f6c:	8832      	ldrh	r2, [r6, #0]
            if(day_state == DUSK) {
     f6e:	2b02      	cmp	r3, #2
     f70:	d000      	beq.n	f74 <sample_light+0x178>
     f72:	e102      	b.n	117a <sample_light+0x37e>
                proc_cache_remainder = PROC_CACHE_MAX_REMAINDER - proc_cache_remainder;
     f74:	333f      	adds	r3, #63	; 0x3f
     f76:	33ff      	adds	r3, #255	; 0xff
     f78:	1a9b      	subs	r3, r3, r2
     f7a:	b29b      	uxth	r3, r3
                if(threshold_idx != IDX_INIT) {
     f7c:	4a67      	ldr	r2, [pc, #412]	; (111c <sample_light+0x320>)
                proc_cache_remainder = PROC_CACHE_MAX_REMAINDER - proc_cache_remainder;
     f7e:	8033      	strh	r3, [r6, #0]
                if(threshold_idx != IDX_INIT) {
     f80:	8813      	ldrh	r3, [r2, #0]
     f82:	4d61      	ldr	r5, [pc, #388]	; (1108 <sample_light+0x30c>)
     f84:	2bff      	cmp	r3, #255	; 0xff
     f86:	d100      	bne.n	f8a <sample_light+0x18e>
     f88:	e0f4      	b.n	1174 <sample_light+0x378>
                    if(threshold_idx + THRESHOLD_IDX_SHIFT > max_idx) {
     f8a:	4970      	ldr	r1, [pc, #448]	; (114c <sample_light+0x350>)
     f8c:	8813      	ldrh	r3, [r2, #0]
     f8e:	880c      	ldrh	r4, [r1, #0]
     f90:	8828      	ldrh	r0, [r5, #0]
     f92:	191b      	adds	r3, r3, r4
     f94:	4283      	cmp	r3, r0
     f96:	dc00      	bgt.n	f9a <sample_light+0x19e>
     f98:	e0e8      	b.n	116c <sample_light+0x370>
                        starting_idx = max_idx;
     f9a:	882c      	ldrh	r4, [r5, #0]
                mbus_write_message32(0xB9, light_meas_start_time_in_min);
     f9c:	4b6c      	ldr	r3, [pc, #432]	; (1150 <sample_light+0x354>)
     f9e:	20b9      	movs	r0, #185	; 0xb9
     fa0:	6819      	ldr	r1, [r3, #0]
     fa2:	f7ff f8eb 	bl	17c <mbus_write_message32>
                sign = -1;
     fa6:	2301      	movs	r3, #1
                start = max_idx;
     fa8:	882d      	ldrh	r5, [r5, #0]
                sign = -1;
     faa:	425b      	negs	r3, r3
                    starting_idx = min_light_idx;
     fac:	b2a4      	uxth	r4, r4
                start = max_idx;
     fae:	b22d      	sxth	r5, r5
                sign = -1;
     fb0:	9302      	str	r3, [sp, #8]
                end = -1;
     fb2:	9303      	str	r3, [sp, #12]
            has_time = false;       // Store time at the start of each day state TOOD: check if this is correct
     fb4:	2600      	movs	r6, #0
            uint16_t sample_idx = 0;
     fb6:	0037      	movs	r7, r6
            has_time = false;       // Store time at the start of each day state TOOD: check if this is correct
     fb8:	4b66      	ldr	r3, [pc, #408]	; (1154 <sample_light+0x358>)
     fba:	701e      	strb	r6, [r3, #0]
            for(i = start; i != end; i += sign) {
     fbc:	9b03      	ldr	r3, [sp, #12]
     fbe:	42ab      	cmp	r3, r5
     fc0:	d000      	beq.n	fc4 <sample_light+0x1c8>
     fc2:	e10f      	b.n	11e4 <sample_light+0x3e8>
            store_day_state_stop();
     fc4:	f7ff fd98 	bl	af8 <store_day_state_stop>
        day_state = (day_state + 1) & 0b11;
     fc8:	2103      	movs	r1, #3
     fca:	4a4d      	ldr	r2, [pc, #308]	; (1100 <sample_light+0x304>)
     fcc:	8813      	ldrh	r3, [r2, #0]
     fce:	3301      	adds	r3, #1
     fd0:	400b      	ands	r3, r1
     fd2:	8013      	strh	r3, [r2, #0]
        set_new_state();
     fd4:	f7ff fe62 	bl	c9c <set_new_state>
}
     fd8:	b005      	add	sp, #20
     fda:	bdf0      	pop	{r4, r5, r6, r7, pc}
        sum -= running_avg[rot_idx];
     fdc:	4a51      	ldr	r2, [pc, #324]	; (1124 <sample_light+0x328>)
     fde:	8810      	ldrh	r0, [r2, #0]
     fe0:	0040      	lsls	r0, r0, #1
     fe2:	5a0c      	ldrh	r4, [r1, r0]
     fe4:	8818      	ldrh	r0, [r3, #0]
     fe6:	1b00      	subs	r0, r0, r4
     fe8:	b280      	uxth	r0, r0
     fea:	8018      	strh	r0, [r3, #0]
        sum += log_light;
     fec:	8818      	ldrh	r0, [r3, #0]
     fee:	19c0      	adds	r0, r0, r7
     ff0:	b280      	uxth	r0, r0
     ff2:	8018      	strh	r0, [r3, #0]
        running_avg[rot_idx] = log_light;
     ff4:	8810      	ldrh	r0, [r2, #0]
     ff6:	0040      	lsls	r0, r0, #1
     ff8:	520f      	strh	r7, [r1, r0]
        running_avg_time[rot_idx] = xo_day_time_in_sec;
     ffa:	483f      	ldr	r0, [pc, #252]	; (10f8 <sample_light+0x2fc>)
     ffc:	8811      	ldrh	r1, [r2, #0]
     ffe:	6804      	ldr	r4, [r0, #0]
    1000:	483c      	ldr	r0, [pc, #240]	; (10f4 <sample_light+0x2f8>)
    1002:	0089      	lsls	r1, r1, #2
    1004:	500c      	str	r4, [r1, r0]
        rot_idx = (rot_idx + 1) & 7;
    1006:	2007      	movs	r0, #7
    1008:	8811      	ldrh	r1, [r2, #0]
    uint16_t last_avg_light = avg_light;
    100a:	002f      	movs	r7, r5
        rot_idx = (rot_idx + 1) & 7;
    100c:	3101      	adds	r1, #1
    100e:	4001      	ands	r1, r0
    1010:	8011      	strh	r1, [r2, #0]
    1012:	e718      	b.n	e46 <sample_light+0x4a>
        next_light_meas_time += XO_32_MIN;
    1014:	22f0      	movs	r2, #240	; 0xf0
    1016:	682b      	ldr	r3, [r5, #0]
    1018:	00d2      	lsls	r2, r2, #3
    101a:	189b      	adds	r3, r3, r2
    101c:	e745      	b.n	eaa <sample_light+0xae>
        new_state = (temp >= day_state_end_time && temp < MID_DAY_TIME);
    101e:	429a      	cmp	r2, r3
    1020:	d900      	bls.n	1024 <sample_light+0x228>
    1022:	e754      	b.n	ece <sample_light+0xd2>
    1024:	4a4c      	ldr	r2, [pc, #304]	; (1158 <sample_light+0x35c>)
    1026:	429a      	cmp	r2, r3
    1028:	4176      	adcs	r6, r6
    102a:	e750      	b.n	ece <sample_light+0xd2>
    uint32_t target = 0;
    102c:	2500      	movs	r5, #0
        else if(new_state) {
    102e:	42ae      	cmp	r6, r5
    1030:	d100      	bne.n	1034 <sample_light+0x238>
    1032:	e76c      	b.n	f0e <sample_light+0x112>
            if(min_light > EDGE_THRESHOLD) {
    1034:	4a33      	ldr	r2, [pc, #204]	; (1104 <sample_light+0x308>)
    1036:	8810      	ldrh	r0, [r2, #0]
    1038:	8819      	ldrh	r1, [r3, #0]
    103a:	4a48      	ldr	r2, [pc, #288]	; (115c <sample_light+0x360>)
    103c:	4b48      	ldr	r3, [pc, #288]	; (1160 <sample_light+0x364>)
    103e:	4288      	cmp	r0, r1
    1040:	d903      	bls.n	104a <sample_light+0x24e>
                target = cur_sunrise - PASSIVE_WINDOW_SHIFT;
    1042:	681d      	ldr	r5, [r3, #0]
    1044:	8813      	ldrh	r3, [r2, #0]
                target = cur_sunset - PASSIVE_WINDOW_SHIFT;
    1046:	1aed      	subs	r5, r5, r3
    1048:	e761      	b.n	f0e <sample_light+0x112>
                target = cur_sunrise + PASSIVE_WINDOW_SHIFT;
    104a:	8815      	ldrh	r5, [r2, #0]
    104c:	681b      	ldr	r3, [r3, #0]
                target = cur_sunset + PASSIVE_WINDOW_SHIFT;
    104e:	18ed      	adds	r5, r5, r3
    1050:	e75d      	b.n	f0e <sample_light+0x112>
    else if(day_state == DUSK) { // && threshold_idx == IDX_INIT) { // For dusk, we want the last time the threshold is crossed, no need to check if there is already a threshold crossing
    1052:	8823      	ldrh	r3, [r4, #0]
    uint32_t target = 0;
    1054:	2500      	movs	r5, #0
    else if(day_state == DUSK) { // && threshold_idx == IDX_INIT) { // For dusk, we want the last time the threshold is crossed, no need to check if there is already a threshold crossing
    1056:	2b02      	cmp	r3, #2
    1058:	d000      	beq.n	105c <sample_light+0x260>
    105a:	e758      	b.n	f0e <sample_light+0x112>
        if(avg_light <= EDGE_THRESHOLD && last_avg_light > EDGE_THRESHOLD) {
    105c:	4b21      	ldr	r3, [pc, #132]	; (10e4 <sample_light+0x2e8>)
    105e:	8819      	ldrh	r1, [r3, #0]
    1060:	4b2f      	ldr	r3, [pc, #188]	; (1120 <sample_light+0x324>)
    1062:	881a      	ldrh	r2, [r3, #0]
    1064:	4291      	cmp	r1, r2
    1066:	d807      	bhi.n	1078 <sample_light+0x27c>
    1068:	881a      	ldrh	r2, [r3, #0]
    106a:	42ba      	cmp	r2, r7
    106c:	d204      	bcs.n	1078 <sample_light+0x27c>
            threshold_idx = max_idx;
    106e:	4b26      	ldr	r3, [pc, #152]	; (1108 <sample_light+0x30c>)
    1070:	4a2a      	ldr	r2, [pc, #168]	; (111c <sample_light+0x320>)
    1072:	881b      	ldrh	r3, [r3, #0]
    1074:	b29b      	uxth	r3, r3
    1076:	e741      	b.n	efc <sample_light+0x100>
    uint32_t target = 0;
    1078:	2500      	movs	r5, #0
        else if(new_state && threshold_idx == IDX_INIT) {
    107a:	42ae      	cmp	r6, r5
    107c:	d100      	bne.n	1080 <sample_light+0x284>
    107e:	e746      	b.n	f0e <sample_light+0x112>
    1080:	4a26      	ldr	r2, [pc, #152]	; (111c <sample_light+0x320>)
    1082:	8812      	ldrh	r2, [r2, #0]
    1084:	2aff      	cmp	r2, #255	; 0xff
    1086:	d000      	beq.n	108a <sample_light+0x28e>
    1088:	e741      	b.n	f0e <sample_light+0x112>
            if(min_light > EDGE_THRESHOLD) {
    108a:	4a1e      	ldr	r2, [pc, #120]	; (1104 <sample_light+0x308>)
    108c:	8810      	ldrh	r0, [r2, #0]
    108e:	8819      	ldrh	r1, [r3, #0]
    1090:	4a28      	ldr	r2, [pc, #160]	; (1134 <sample_light+0x338>)
    1092:	4b32      	ldr	r3, [pc, #200]	; (115c <sample_light+0x360>)
    1094:	4288      	cmp	r0, r1
    1096:	d902      	bls.n	109e <sample_light+0x2a2>
                target = cur_sunset + PASSIVE_WINDOW_SHIFT;
    1098:	881d      	ldrh	r5, [r3, #0]
    109a:	6813      	ldr	r3, [r2, #0]
    109c:	e7d7      	b.n	104e <sample_light+0x252>
                target = cur_sunset - PASSIVE_WINDOW_SHIFT;
    109e:	6815      	ldr	r5, [r2, #0]
    10a0:	881b      	ldrh	r3, [r3, #0]
    10a2:	e7d0      	b.n	1046 <sample_light+0x24a>
        else if(target < cur_edge - MAX_EDGE_SHIFT) {
    10a4:	6811      	ldr	r1, [r2, #0]
    10a6:	8818      	ldrh	r0, [r3, #0]
    10a8:	1a09      	subs	r1, r1, r0
    10aa:	42a9      	cmp	r1, r5
    10ac:	d800      	bhi.n	10b0 <sample_light+0x2b4>
    10ae:	e73e      	b.n	f2e <sample_light+0x132>
            target = cur_edge - MAX_EDGE_SHIFT;
    10b0:	6815      	ldr	r5, [r2, #0]
    10b2:	881b      	ldrh	r3, [r3, #0]
    10b4:	1aed      	subs	r5, r5, r3
    10b6:	e73a      	b.n	f2e <sample_light+0x132>
        else if(day_state == DUSK && target > MID_DAY_TIME && target < MAX_DAY_TIME 
    10b8:	8823      	ldrh	r3, [r4, #0]
    10ba:	2b02      	cmp	r3, #2
    10bc:	d000      	beq.n	10c0 <sample_light+0x2c4>
    10be:	e748      	b.n	f52 <sample_light+0x156>
    10c0:	4b28      	ldr	r3, [pc, #160]	; (1164 <sample_light+0x368>)
    10c2:	4a1b      	ldr	r2, [pc, #108]	; (1130 <sample_light+0x334>)
    10c4:	18eb      	adds	r3, r5, r3
    10c6:	4293      	cmp	r3, r2
    10c8:	d900      	bls.n	10cc <sample_light+0x2d0>
    10ca:	e742      	b.n	f52 <sample_light+0x156>
                && (target - cur_sunrise > XO_270_MIN)) {
    10cc:	4b24      	ldr	r3, [pc, #144]	; (1160 <sample_light+0x364>)
    10ce:	4a1a      	ldr	r2, [pc, #104]	; (1138 <sample_light+0x33c>)
    10d0:	681b      	ldr	r3, [r3, #0]
    10d2:	1aeb      	subs	r3, r5, r3
    10d4:	4293      	cmp	r3, r2
    10d6:	d800      	bhi.n	10da <sample_light+0x2de>
    10d8:	e73b      	b.n	f52 <sample_light+0x156>
            next_sunset = target;
    10da:	4b23      	ldr	r3, [pc, #140]	; (1168 <sample_light+0x36c>)
    10dc:	e738      	b.n	f50 <sample_light+0x154>
    10de:	46c0      	nop			; (mov r8, r8)
    10e0:	00003a48 	.word	0x00003a48
    10e4:	00003a06 	.word	0x00003a06
    10e8:	00003948 	.word	0x00003948
    10ec:	0000ffff 	.word	0x0000ffff
    10f0:	00003a9c 	.word	0x00003a9c
    10f4:	00003aac 	.word	0x00003aac
    10f8:	00003ae4 	.word	0x00003ae4
    10fc:	00003906 	.word	0x00003906
    1100:	000038f8 	.word	0x000038f8
    1104:	00003904 	.word	0x00003904
    1108:	00003a58 	.word	0x00003a58
    110c:	00003a70 	.word	0x00003a70
    1110:	00003adc 	.word	0x00003adc
    1114:	00003aec 	.word	0x00003aec
    1118:	000039e8 	.word	0x000039e8
    111c:	0000394c 	.word	0x0000394c
    1120:	000037c8 	.word	0x000037c8
    1124:	00003a9a 	.word	0x00003a9a
    1128:	000037ea 	.word	0x000037ea
    112c:	00003a0c 	.word	0x00003a0c
    1130:	0000a8be 	.word	0x0000a8be
    1134:	00003a14 	.word	0x00003a14
    1138:	00004c90 	.word	0x00004c90
    113c:	00003a78 	.word	0x00003a78
    1140:	a0001200 	.word	0xa0001200
    1144:	a000007c 	.word	0xa000007c
    1148:	0000392c 	.word	0x0000392c
    114c:	000038ea 	.word	0x000038ea
    1150:	00003a38 	.word	0x00003a38
    1154:	00003a2d 	.word	0x00003a2d
    1158:	0000a8bf 	.word	0x0000a8bf
    115c:	00003a04 	.word	0x00003a04
    1160:	00003a10 	.word	0x00003a10
    1164:	ffff573f 	.word	0xffff573f
    1168:	00003a7c 	.word	0x00003a7c
                        starting_idx = threshold_idx + THRESHOLD_IDX_SHIFT;
    116c:	8814      	ldrh	r4, [r2, #0]
    116e:	880b      	ldrh	r3, [r1, #0]
    1170:	18e4      	adds	r4, r4, r3
    1172:	e713      	b.n	f9c <sample_light+0x1a0>
                    starting_idx = min_light_idx;
    1174:	4b37      	ldr	r3, [pc, #220]	; (1254 <sample_light+0x458>)
    1176:	881c      	ldrh	r4, [r3, #0]
    1178:	e710      	b.n	f9c <sample_light+0x1a0>
                right_shift_arr(proc_cache, 0, PROC_CACHE_LEN, 1 - proc_cache_remainder);
    117a:	2301      	movs	r3, #1
    117c:	4f36      	ldr	r7, [pc, #216]	; (1258 <sample_light+0x45c>)
    117e:	1a9b      	subs	r3, r3, r2
    1180:	0029      	movs	r1, r5
    1182:	220a      	movs	r2, #10
    1184:	0038      	movs	r0, r7
    1186:	b21b      	sxth	r3, r3
    1188:	f7ff fb28 	bl	7dc <right_shift_arr>
                mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) cache_addr, proc_cache, PROC_CACHE_LEN - 1);
    118c:	4b33      	ldr	r3, [pc, #204]	; (125c <sample_light+0x460>)
    118e:	003a      	movs	r2, r7
    1190:	8819      	ldrh	r1, [r3, #0]
    1192:	2006      	movs	r0, #6
    1194:	2309      	movs	r3, #9
    1196:	b289      	uxth	r1, r1
    1198:	f7ff f83c 	bl	214 <mbus_copy_mem_from_local_to_remote_bulk>
                cache_addr = CACHE_START_ADDR;
    119c:	4b30      	ldr	r3, [pc, #192]	; (1260 <sample_light+0x464>)
    119e:	4a2f      	ldr	r2, [pc, #188]	; (125c <sample_light+0x460>)
                proc_cache_remainder = 0; // manually set to 0 to start reading
    11a0:	8035      	strh	r5, [r6, #0]
                cache_addr = CACHE_START_ADDR;
    11a2:	8013      	strh	r3, [r2, #0]
                if(day_state == NOON) {
    11a4:	8823      	ldrh	r3, [r4, #0]
                    starting_idx = 0;
    11a6:	002c      	movs	r4, r5
                if(day_state == NOON) {
    11a8:	2b01      	cmp	r3, #1
    11aa:	d00c      	beq.n	11c6 <sample_light+0x3ca>
                else if(threshold_idx != IDX_INIT) {
    11ac:	4b2d      	ldr	r3, [pc, #180]	; (1264 <sample_light+0x468>)
    11ae:	881a      	ldrh	r2, [r3, #0]
    11b0:	2aff      	cmp	r2, #255	; 0xff
    11b2:	d014      	beq.n	11de <sample_light+0x3e2>
                    if(threshold_idx < THRESHOLD_IDX_SHIFT) {
    11b4:	4a2c      	ldr	r2, [pc, #176]	; (1268 <sample_light+0x46c>)
    11b6:	8818      	ldrh	r0, [r3, #0]
    11b8:	8811      	ldrh	r1, [r2, #0]
    11ba:	4288      	cmp	r0, r1
    11bc:	d303      	bcc.n	11c6 <sample_light+0x3ca>
                        starting_idx = threshold_idx - THRESHOLD_IDX_SHIFT;
    11be:	881c      	ldrh	r4, [r3, #0]
    11c0:	8813      	ldrh	r3, [r2, #0]
    11c2:	1ae4      	subs	r4, r4, r3
                    starting_idx = min_light_idx;
    11c4:	b2a4      	uxth	r4, r4
                sample_time_in_min = light_meas_start_time_in_min;
    11c6:	4b29      	ldr	r3, [pc, #164]	; (126c <sample_light+0x470>)
                start = 0;
    11c8:	2500      	movs	r5, #0
                sample_time_in_min = light_meas_start_time_in_min;
    11ca:	681b      	ldr	r3, [r3, #0]
    11cc:	9301      	str	r3, [sp, #4]
                end = max_idx + 1;
    11ce:	4b28      	ldr	r3, [pc, #160]	; (1270 <sample_light+0x474>)
    11d0:	881b      	ldrh	r3, [r3, #0]
    11d2:	3301      	adds	r3, #1
    11d4:	b21b      	sxth	r3, r3
    11d6:	9303      	str	r3, [sp, #12]
                sign = 1;
    11d8:	2301      	movs	r3, #1
    11da:	9302      	str	r3, [sp, #8]
    11dc:	e6ea      	b.n	fb4 <sample_light+0x1b8>
                    starting_idx = min_light_idx;
    11de:	4b1d      	ldr	r3, [pc, #116]	; (1254 <sample_light+0x458>)
    11e0:	881c      	ldrh	r4, [r3, #0]
    11e2:	e7ef      	b.n	11c4 <sample_light+0x3c8>
                uint16_t log_light = read_next_from_proc_cache();
    11e4:	f7ff fcca 	bl	b7c <read_next_from_proc_cache>
                if(i == next_sample_idx) {
    11e8:	42a5      	cmp	r5, r4
    11ea:	d10b      	bne.n	1204 <sample_light+0x408>
                    store_to_code_cache(log_light, sample_idx, sample_time_in_min);
    11ec:	0039      	movs	r1, r7
    11ee:	9a01      	ldr	r2, [sp, #4]
    11f0:	f7ff fbfe 	bl	9f0 <store_to_code_cache>
                    if(day_state == NOON) {
    11f4:	4b1f      	ldr	r3, [pc, #124]	; (1274 <sample_light+0x478>)
                    sample_idx++;
    11f6:	3701      	adds	r7, #1
                    if(day_state == NOON) {
    11f8:	881b      	ldrh	r3, [r3, #0]
                    sample_idx++;
    11fa:	b2bf      	uxth	r7, r7
                    if(day_state == NOON) {
    11fc:	2b01      	cmp	r3, #1
    11fe:	d10c      	bne.n	121a <sample_light+0x41e>
                        next_sample_idx++;
    1200:	1c6c      	adds	r4, r5, #1
                        next_sample_idx += (intervals[interval_idx] * sign);
    1202:	b2a4      	uxth	r4, r4
                if(day_state == NOON) {
    1204:	4b1b      	ldr	r3, [pc, #108]	; (1274 <sample_light+0x478>)
    1206:	881a      	ldrh	r2, [r3, #0]
    1208:	2a01      	cmp	r2, #1
    120a:	d114      	bne.n	1236 <sample_light+0x43a>
                    sample_time_in_min += 32;
    120c:	9b01      	ldr	r3, [sp, #4]
    120e:	3320      	adds	r3, #32
                    sample_time_in_min -= 1;
    1210:	9301      	str	r3, [sp, #4]
            for(i = start; i != end; i += sign) {
    1212:	9b02      	ldr	r3, [sp, #8]
    1214:	195d      	adds	r5, r3, r5
    1216:	b22d      	sxth	r5, r5
    1218:	e6d0      	b.n	fbc <sample_light+0x1c0>
                        if(sample_idx >= resample_indices[interval_idx]) {
    121a:	4a17      	ldr	r2, [pc, #92]	; (1278 <sample_light+0x47c>)
    121c:	0073      	lsls	r3, r6, #1
    121e:	5a9b      	ldrh	r3, [r3, r2]
    1220:	42bb      	cmp	r3, r7
    1222:	d801      	bhi.n	1228 <sample_light+0x42c>
                            interval_idx++;
    1224:	3601      	adds	r6, #1
    1226:	b2b6      	uxth	r6, r6
                        next_sample_idx += (intervals[interval_idx] * sign);
    1228:	4b14      	ldr	r3, [pc, #80]	; (127c <sample_light+0x480>)
    122a:	0072      	lsls	r2, r6, #1
    122c:	5ad3      	ldrh	r3, [r2, r3]
    122e:	9a02      	ldr	r2, [sp, #8]
    1230:	4353      	muls	r3, r2
    1232:	18e4      	adds	r4, r4, r3
    1234:	e7e5      	b.n	1202 <sample_light+0x406>
                else if(day_state == DAWN) {
    1236:	881b      	ldrh	r3, [r3, #0]
    1238:	2b00      	cmp	r3, #0
    123a:	d102      	bne.n	1242 <sample_light+0x446>
                    sample_time_in_min += 1;
    123c:	9b01      	ldr	r3, [sp, #4]
    123e:	3301      	adds	r3, #1
    1240:	e7e6      	b.n	1210 <sample_light+0x414>
                    sample_time_in_min -= 1;
    1242:	9b01      	ldr	r3, [sp, #4]
    1244:	3b01      	subs	r3, #1
    1246:	e7e3      	b.n	1210 <sample_light+0x414>
        max_idx++;
    1248:	4a09      	ldr	r2, [pc, #36]	; (1270 <sample_light+0x474>)
    124a:	8813      	ldrh	r3, [r2, #0]
    124c:	3301      	adds	r3, #1
    124e:	b29b      	uxth	r3, r3
    1250:	8013      	strh	r3, [r2, #0]
}
    1252:	e6c1      	b.n	fd8 <sample_light+0x1dc>
    1254:	00003906 	.word	0x00003906
    1258:	0000398c 	.word	0x0000398c
    125c:	000038ee 	.word	0x000038ee
    1260:	00003e98 	.word	0x00003e98
    1264:	0000394c 	.word	0x0000394c
    1268:	000038ea 	.word	0x000038ea
    126c:	00003a38 	.word	0x00003a38
    1270:	00003a58 	.word	0x00003a58
    1274:	000038f8 	.word	0x000038f8
    1278:	0000392e 	.word	0x0000392e
    127c:	00003692 	.word	0x00003692

Disassembly of section .text.flush_temp_cache:

00001280 <flush_temp_cache>:
void flush_temp_cache() {
    1280:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    if(temp_cache_remainder < TEMP_CACHE_MAX_REMAINDER) {
    1282:	2588      	movs	r5, #136	; 0x88
    1284:	4c10      	ldr	r4, [pc, #64]	; (12c8 <flush_temp_cache+0x48>)
    1286:	006d      	lsls	r5, r5, #1
    1288:	8823      	ldrh	r3, [r4, #0]
    128a:	42ab      	cmp	r3, r5
    128c:	d21a      	bcs.n	12c4 <flush_temp_cache+0x44>
        right_shift_arr(temp_cache, 0, TEMP_CACHE_LEN, -temp_cache_remainder);
    128e:	8823      	ldrh	r3, [r4, #0]
    1290:	4e0e      	ldr	r6, [pc, #56]	; (12cc <flush_temp_cache+0x4c>)
    1292:	425b      	negs	r3, r3
    1294:	2209      	movs	r2, #9
    1296:	2100      	movs	r1, #0
    1298:	0030      	movs	r0, r6
    129a:	b21b      	sxth	r3, r3
    129c:	f7ff fa9e 	bl	7dc <right_shift_arr>
        temp_cache[0] &= 0x0000FFFF;
    12a0:	6833      	ldr	r3, [r6, #0]
        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, temp_cache, TEMP_CACHE_LEN - 1);
    12a2:	4f0b      	ldr	r7, [pc, #44]	; (12d0 <flush_temp_cache+0x50>)
        temp_cache[0] &= 0x0000FFFF;
    12a4:	b29b      	uxth	r3, r3
        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, temp_cache, TEMP_CACHE_LEN - 1);
    12a6:	0032      	movs	r2, r6
        temp_cache[0] &= 0x0000FFFF;
    12a8:	6033      	str	r3, [r6, #0]
        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, temp_cache, TEMP_CACHE_LEN - 1);
    12aa:	2006      	movs	r0, #6
    12ac:	2308      	movs	r3, #8
    12ae:	8839      	ldrh	r1, [r7, #0]
    12b0:	f7fe ffb0 	bl	214 <mbus_copy_mem_from_local_to_remote_bulk>
        code_addr += TEMP_CACHE_LEN << 2;
    12b4:	883b      	ldrh	r3, [r7, #0]
        max_unit_count++;
    12b6:	4a07      	ldr	r2, [pc, #28]	; (12d4 <flush_temp_cache+0x54>)
        code_addr += TEMP_CACHE_LEN << 2;
    12b8:	3324      	adds	r3, #36	; 0x24
    12ba:	803b      	strh	r3, [r7, #0]
        max_unit_count++;
    12bc:	8813      	ldrh	r3, [r2, #0]
        temp_cache_remainder = TEMP_CACHE_MAX_REMAINDER;
    12be:	8025      	strh	r5, [r4, #0]
        max_unit_count++;
    12c0:	3301      	adds	r3, #1
    12c2:	8013      	strh	r3, [r2, #0]
}
    12c4:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    12c6:	46c0      	nop			; (mov r8, r8)
    12c8:	0000394a 	.word	0x0000394a
    12cc:	00003964 	.word	0x00003964
    12d0:	00003a0a 	.word	0x00003a0a
    12d4:	00003a5a 	.word	0x00003a5a

Disassembly of section .text.store_code_to_temp_cache:

000012d8 <store_code_to_temp_cache>:
void store_code_to_temp_cache(uint16_t code, uint16_t len) {
    12d8:	b510      	push	{r4, lr}
    12da:	000c      	movs	r4, r1
    right_shift_arr(temp_cache, code, TEMP_CACHE_LEN, -len);
    12dc:	424b      	negs	r3, r1
    12de:	2209      	movs	r2, #9
    12e0:	0001      	movs	r1, r0
    12e2:	b21b      	sxth	r3, r3
    12e4:	4804      	ldr	r0, [pc, #16]	; (12f8 <store_code_to_temp_cache+0x20>)
    12e6:	f7ff fa79 	bl	7dc <right_shift_arr>
    temp_cache_remainder -= len;
    12ea:	4a04      	ldr	r2, [pc, #16]	; (12fc <store_code_to_temp_cache+0x24>)
    12ec:	8811      	ldrh	r1, [r2, #0]
    12ee:	1b09      	subs	r1, r1, r4
    12f0:	b289      	uxth	r1, r1
    12f2:	8011      	strh	r1, [r2, #0]
}
    12f4:	bd10      	pop	{r4, pc}
    12f6:	46c0      	nop			; (mov r8, r8)
    12f8:	00003964 	.word	0x00003964
    12fc:	0000394a 	.word	0x0000394a

Disassembly of section .text.sample_temp:

00001300 <sample_temp>:
void sample_temp() {
    1300:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    uint16_t log_temp = log2(snt_sys_temp_code) >> (8 - TEMP_RES); 
    1302:	4b25      	ldr	r3, [pc, #148]	; (1398 <sample_temp+0x98>)
    1304:	2100      	movs	r1, #0
    1306:	6818      	ldr	r0, [r3, #0]
    1308:	f7ff faba 	bl	880 <log2>
    int8_t diff = log_temp - last_log_temp;
    130c:	4b23      	ldr	r3, [pc, #140]	; (139c <sample_temp+0x9c>)
    uint16_t log_temp = log2(snt_sys_temp_code) >> (8 - TEMP_RES); 
    130e:	0840      	lsrs	r0, r0, #1
    int8_t diff = log_temp - last_log_temp;
    1310:	881c      	ldrh	r4, [r3, #0]
    uint16_t log_temp = log2(snt_sys_temp_code) >> (8 - TEMP_RES); 
    1312:	b285      	uxth	r5, r0
    int8_t diff = log_temp - last_log_temp;
    1314:	1b2c      	subs	r4, r5, r4
    1316:	b2e4      	uxtb	r4, r4
    last_log_temp = log_temp;
    1318:	801d      	strh	r5, [r3, #0]
    if(diff < 2 && diff >= -2) {
    131a:	1ca3      	adds	r3, r4, #2
    131c:	b2db      	uxtb	r3, r3
    131e:	2b03      	cmp	r3, #3
    1320:	d82d      	bhi.n	137e <sample_temp+0x7e>
        code_idx = diff + 2;
    1322:	b264      	sxtb	r4, r4
    1324:	3402      	adds	r4, #2
        len_needed = temp_code_lengths[code_idx];
    1326:	4b1e      	ldr	r3, [pc, #120]	; (13a0 <sample_temp+0xa0>)
        code = temp_diff_codes[code_idx];
    1328:	491e      	ldr	r1, [pc, #120]	; (13a4 <sample_temp+0xa4>)
        code_idx = diff + 2;
    132a:	b2a4      	uxth	r4, r4
        len_needed = temp_code_lengths[code_idx];
    132c:	0062      	lsls	r2, r4, #1
    132e:	5ad3      	ldrh	r3, [r2, r3]
        code = temp_diff_codes[code_idx];
    1330:	5a57      	ldrh	r7, [r2, r1]
    if(temp_cache_remainder < len_needed) {
    1332:	4a1d      	ldr	r2, [pc, #116]	; (13a8 <sample_temp+0xa8>)
    1334:	8812      	ldrh	r2, [r2, #0]
    1336:	429a      	cmp	r2, r3
    1338:	d201      	bcs.n	133e <sample_temp+0x3e>
        flush_temp_cache();
    133a:	f7ff ffa1 	bl	1280 <flush_temp_cache>
    store_temp_index++;
    133e:	4e1b      	ldr	r6, [pc, #108]	; (13ac <sample_temp+0xac>)
    1340:	8833      	ldrh	r3, [r6, #0]
    1342:	3301      	adds	r3, #1
    1344:	b29b      	uxth	r3, r3
    1346:	8033      	strh	r3, [r6, #0]
    if(store_temp_index > 47) {
    1348:	8833      	ldrh	r3, [r6, #0]
    134a:	2b2f      	cmp	r3, #47	; 0x2f
    134c:	d901      	bls.n	1352 <sample_temp+0x52>
        store_temp_index = 0;   // resets when taking the midnight data
    134e:	2300      	movs	r3, #0
    1350:	8033      	strh	r3, [r6, #0]
    if(temp_cache_remainder == TEMP_CACHE_MAX_REMAINDER) {
    1352:	4b15      	ldr	r3, [pc, #84]	; (13a8 <sample_temp+0xa8>)
    1354:	881a      	ldrh	r2, [r3, #0]
    1356:	2388      	movs	r3, #136	; 0x88
    1358:	005b      	lsls	r3, r3, #1
    135a:	429a      	cmp	r2, r3
    135c:	d113      	bne.n	1386 <sample_temp+0x86>
        store_code_to_temp_cache(day_count, 7);
    135e:	4b14      	ldr	r3, [pc, #80]	; (13b0 <sample_temp+0xb0>)
    1360:	2107      	movs	r1, #7
    1362:	8818      	ldrh	r0, [r3, #0]
    1364:	b280      	uxth	r0, r0
    1366:	f7ff ffb7 	bl	12d8 <store_code_to_temp_cache>
        store_code_to_temp_cache(store_temp_index, 6);
    136a:	8830      	ldrh	r0, [r6, #0]
    136c:	2106      	movs	r1, #6
    136e:	b280      	uxth	r0, r0
    1370:	f7ff ffb2 	bl	12d8 <store_code_to_temp_cache>
            store_code_to_temp_cache(log_temp, TEMP_RES);
    1374:	2107      	movs	r1, #7
    1376:	0028      	movs	r0, r5
    1378:	f7ff ffae 	bl	12d8 <store_code_to_temp_cache>
}
    137c:	e00b      	b.n	1396 <sample_temp+0x96>
        code = temp_diff_codes[code_idx];   // FIXME: redundant code, move outside of block
    137e:	2709      	movs	r7, #9
        len_needed = temp_code_lengths[code_idx] + TEMP_RES;
    1380:	230b      	movs	r3, #11
        code_idx = 4;
    1382:	2404      	movs	r4, #4
    1384:	e7d5      	b.n	1332 <sample_temp+0x32>
        store_code_to_temp_cache(code, temp_code_lengths[code_idx]);
    1386:	4b06      	ldr	r3, [pc, #24]	; (13a0 <sample_temp+0xa0>)
    1388:	0062      	lsls	r2, r4, #1
    138a:	0038      	movs	r0, r7
    138c:	5ad1      	ldrh	r1, [r2, r3]
    138e:	f7ff ffa3 	bl	12d8 <store_code_to_temp_cache>
        if(code_idx == 4) {
    1392:	2c04      	cmp	r4, #4
    1394:	d0ee      	beq.n	1374 <sample_temp+0x74>
}
    1396:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    1398:	00003938 	.word	0x00003938
    139c:	00003a30 	.word	0x00003a30
    13a0:	000037a6 	.word	0x000037a6
    13a4:	000037b0 	.word	0x000037b0
    13a8:	0000394a 	.word	0x0000394a
    13ac:	00003ad4 	.word	0x00003ad4
    13b0:	00003a18 	.word	0x00003a18

Disassembly of section .text.set_projected_end_time:

000013b4 <set_projected_end_time>:
void set_projected_end_time() {
    13b4:	b510      	push	{r4, lr}
    if(temp <= XO_8_MIN) {
    13b6:	24f0      	movs	r4, #240	; 0xf0
    uint32_t temp = next_light_meas_time - projected_end_time_in_sec;
    13b8:	4811      	ldr	r0, [pc, #68]	; (1400 <set_projected_end_time+0x4c>)
    13ba:	4a12      	ldr	r2, [pc, #72]	; (1404 <set_projected_end_time+0x50>)
    13bc:	6803      	ldr	r3, [r0, #0]
    13be:	6811      	ldr	r1, [r2, #0]
    if(temp <= XO_8_MIN) {
    13c0:	0064      	lsls	r4, r4, #1
    uint32_t temp = next_light_meas_time - projected_end_time_in_sec;
    13c2:	1a5b      	subs	r3, r3, r1
    if(temp <= XO_8_MIN) {
    13c4:	42a3      	cmp	r3, r4
    13c6:	d80d      	bhi.n	13e4 <set_projected_end_time+0x30>
        projected_end_time_in_sec = next_light_meas_time;
    13c8:	6801      	ldr	r1, [r0, #0]
    13ca:	6011      	str	r1, [r2, #0]
        if(temp <= XO_1_MIN) {
    13cc:	4a0e      	ldr	r2, [pc, #56]	; (1408 <set_projected_end_time+0x54>)
    13ce:	2b3c      	cmp	r3, #60	; 0x3c
    13d0:	d802      	bhi.n	13d8 <set_projected_end_time+0x24>
            lnt_counter_base = 0;
    13d2:	2300      	movs	r3, #0
            lnt_counter_base = 3;
    13d4:	8013      	strh	r3, [r2, #0]
}
    13d6:	bd10      	pop	{r4, pc}
        else if(temp <= XO_2_MIN) {
    13d8:	2b78      	cmp	r3, #120	; 0x78
    13da:	d801      	bhi.n	13e0 <set_projected_end_time+0x2c>
            lnt_counter_base = 1;
    13dc:	2301      	movs	r3, #1
    13de:	e7f9      	b.n	13d4 <set_projected_end_time+0x20>
            lnt_counter_base = 3;
    13e0:	2303      	movs	r3, #3
    13e2:	e7f7      	b.n	13d4 <set_projected_end_time+0x20>
    else if(temp <= XO_18_MIN) {
    13e4:	2487      	movs	r4, #135	; 0x87
    13e6:	00e4      	lsls	r4, r4, #3
    13e8:	42a3      	cmp	r3, r4
    13ea:	d804      	bhi.n	13f6 <set_projected_end_time+0x42>
        projected_end_time_in_sec = next_light_meas_time - XO_8_MIN;
    13ec:	6803      	ldr	r3, [r0, #0]
    13ee:	3be1      	subs	r3, #225	; 0xe1
    13f0:	3bff      	subs	r3, #255	; 0xff
    13f2:	6013      	str	r3, [r2, #0]
}
    13f4:	e7ef      	b.n	13d6 <set_projected_end_time+0x22>
        projected_end_time_in_sec += XO_8_MIN;
    13f6:	31e1      	adds	r1, #225	; 0xe1
    13f8:	31ff      	adds	r1, #255	; 0xff
    13fa:	6011      	str	r1, [r2, #0]
}
    13fc:	e7eb      	b.n	13d6 <set_projected_end_time+0x22>
    13fe:	46c0      	nop			; (mov r8, r8)
    1400:	00003a70 	.word	0x00003a70
    1404:	00003a80 	.word	0x00003a80
    1408:	00003a3e 	.word	0x00003a3e

Disassembly of section .text.crcEnc16:

0000140c <crcEnc16>:
    uint32_t data2 = (radio_data_arr[2] << CRC_LEN) + (radio_data_arr[1] >> CRC_LEN);
    140c:	4b1d      	ldr	r3, [pc, #116]	; (1484 <crcEnc16+0x78>)
{
    140e:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    uint32_t data2 = (radio_data_arr[2] << CRC_LEN) + (radio_data_arr[1] >> CRC_LEN);
    1410:	6898      	ldr	r0, [r3, #8]
    1412:	685a      	ldr	r2, [r3, #4]
    1414:	0400      	lsls	r0, r0, #16
    1416:	0c12      	lsrs	r2, r2, #16
    1418:	1882      	adds	r2, r0, r2
    uint32_t data1 = (radio_data_arr[1] << CRC_LEN) + (radio_data_arr[0] >> CRC_LEN);
    141a:	685c      	ldr	r4, [r3, #4]
    uint32_t data2 = (radio_data_arr[2] << CRC_LEN) + (radio_data_arr[1] >> CRC_LEN);
    141c:	9200      	str	r2, [sp, #0]
    uint32_t data1 = (radio_data_arr[1] << CRC_LEN) + (radio_data_arr[0] >> CRC_LEN);
    141e:	681a      	ldr	r2, [r3, #0]
    1420:	0424      	lsls	r4, r4, #16
    1422:	0c12      	lsrs	r2, r2, #16
    1424:	18a4      	adds	r4, r4, r2
    uint32_t data0 = radio_data_arr[0] << CRC_LEN;
    1426:	2200      	movs	r2, #0
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff));
    1428:	4917      	ldr	r1, [pc, #92]	; (1488 <crcEnc16+0x7c>)
    uint32_t data0 = radio_data_arr[0] << CRC_LEN;
    142a:	681d      	ldr	r5, [r3, #0]
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff));
    142c:	468c      	mov	ip, r1
    uint16_t remainder = 0x0000;
    142e:	0013      	movs	r3, r2
    uint32_t data0 = radio_data_arr[0] << CRC_LEN;
    1430:	042d      	lsls	r5, r5, #16
        if (remainder > 0x7fff)
    1432:	0bde      	lsrs	r6, r3, #15
    1434:	4277      	negs	r7, r6
    1436:	b2bf      	uxth	r7, r7
        if (i < 32)
    1438:	b291      	uxth	r1, r2
    143a:	2a1f      	cmp	r2, #31
    143c:	d815      	bhi.n	146a <crcEnc16+0x5e>
            input_bit = ((data2 << i) > 0x7fffffff);
    143e:	9900      	ldr	r1, [sp, #0]
    1440:	4091      	lsls	r1, r2
        remainder_shift = remainder << 1;
    1442:	005b      	lsls	r3, r3, #1
    1444:	b298      	uxth	r0, r3
    1446:	9001      	str	r0, [sp, #4]
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff));
    1448:	4660      	mov	r0, ip
    144a:	4003      	ands	r3, r0
    144c:	9801      	ldr	r0, [sp, #4]
            input_bit = (data0 << (i-64)) > 0x7fffffff;
    144e:	0fc9      	lsrs	r1, r1, #31
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff));
    1450:	4047      	eors	r7, r0
    1452:	480e      	ldr	r0, [pc, #56]	; (148c <crcEnc16+0x80>)
    1454:	4071      	eors	r1, r6
    1456:	4007      	ands	r7, r0
    1458:	185b      	adds	r3, r3, r1
    145a:	3201      	adds	r2, #1
    145c:	433b      	orrs	r3, r7
    for (i = 0; i < DATA_LEN; i++)
    145e:	2a60      	cmp	r2, #96	; 0x60
    1460:	d1e7      	bne.n	1432 <crcEnc16+0x26>
    msg_out[0] = data0 + remainder;
    1462:	480b      	ldr	r0, [pc, #44]	; (1490 <crcEnc16+0x84>)
    1464:	195b      	adds	r3, r3, r5
    1466:	6003      	str	r3, [r0, #0]
}
    1468:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
        else if (i < 64)
    146a:	293f      	cmp	r1, #63	; 0x3f
    146c:	d805      	bhi.n	147a <crcEnc16+0x6e>
            input_bit = (data1 << (i-32)) > 0x7fffffff;
    146e:	0011      	movs	r1, r2
    1470:	0020      	movs	r0, r4
    1472:	3920      	subs	r1, #32
            input_bit = (data0 << (i-64)) > 0x7fffffff;
    1474:	4088      	lsls	r0, r1
    1476:	0001      	movs	r1, r0
    1478:	e7e3      	b.n	1442 <crcEnc16+0x36>
    147a:	0011      	movs	r1, r2
    147c:	0028      	movs	r0, r5
    147e:	3940      	subs	r1, #64	; 0x40
    1480:	e7f8      	b.n	1474 <crcEnc16+0x68>
    1482:	46c0      	nop			; (mov r8, r8)
    1484:	000039b8 	.word	0x000039b8
    1488:	00003ffd 	.word	0x00003ffd
    148c:	ffffc002 	.word	0xffffc002
    1490:	00003a64 	.word	0x00003a64

Disassembly of section .text.pmu_set_sar_ratio:

00001494 <pmu_set_sar_ratio>:
void pmu_set_sar_ratio (uint32_t sar_ratio) {
    1494:	b570      	push	{r4, r5, r6, lr}
    uint32_t adc_vbat   = (*REG0 & 0xFF0000) >> 16;
    1496:	24a0      	movs	r4, #160	; 0xa0
void pmu_set_sar_ratio (uint32_t sar_ratio) {
    1498:	0005      	movs	r5, r0
    pmu_reg_write (60, // 0x3C (CTRL_DESIRED_STATE_ACTIVE)
    149a:	491c      	ldr	r1, [pc, #112]	; (150c <pmu_set_sar_ratio+0x78>)
    149c:	203c      	movs	r0, #60	; 0x3c
    149e:	f7fe ff4b 	bl	338 <pmu_reg_write>
    uint32_t adc_vbat   = (*REG0 & 0xFF0000) >> 16;
    14a2:	0624      	lsls	r4, r4, #24
    pmu_reg_write(0x00,0x03);
    14a4:	2103      	movs	r1, #3
    14a6:	2000      	movs	r0, #0
    14a8:	f7fe ff46 	bl	338 <pmu_reg_write>
    uint32_t adc_offset = (*REG0 & 0x007F00) >> 8;
    14ac:	267f      	movs	r6, #127	; 0x7f
    uint32_t adc_vbat   = (*REG0 & 0xFF0000) >> 16;
    14ae:	6823      	ldr	r3, [r4, #0]
    uint32_t adc_offset = (*REG0 & 0x007F00) >> 8;
    14b0:	6821      	ldr	r1, [r4, #0]
    uint32_t adc_dout   = (*REG0 & 0x0000FF) >> 0;
    14b2:	6823      	ldr	r3, [r4, #0]
    pmu_reg_write (2, // 0x02 (ADC_CONFIG_OVERRIDE)
    14b4:	2381      	movs	r3, #129	; 0x81
    uint32_t adc_offset = (*REG0 & 0x007F00) >> 8;
    14b6:	0a09      	lsrs	r1, r1, #8
    pmu_reg_write (2, // 0x02 (ADC_CONFIG_OVERRIDE)
    14b8:	041b      	lsls	r3, r3, #16
    uint32_t adc_offset = (*REG0 & 0x007F00) >> 8;
    14ba:	4031      	ands	r1, r6
    pmu_reg_write (2, // 0x02 (ADC_CONFIG_OVERRIDE)
    14bc:	4319      	orrs	r1, r3
    14be:	2002      	movs	r0, #2
    14c0:	f7fe ff3a 	bl	338 <pmu_reg_write>
    pmu_reg_write (10, // 0x0A (SAR_RATIO_MINIMUM)
    14c4:	0029      	movs	r1, r5
    14c6:	200a      	movs	r0, #10
    14c8:	f7fe ff36 	bl	338 <pmu_reg_write>
    pmu_reg_write (5, // 0x05 (SAR_RATIO_OVERRIDE)
    14cc:	2180      	movs	r1, #128	; 0x80
    14ce:	2005      	movs	r0, #5
    14d0:	0189      	lsls	r1, r1, #6
    14d2:	f7fe ff31 	bl	338 <pmu_reg_write>
    pmu_reg_write (60, // 0x3C (CTRL_DESIRED_STATE_ACTIVE)
    14d6:	203c      	movs	r0, #60	; 0x3c
    14d8:	490d      	ldr	r1, [pc, #52]	; (1510 <pmu_set_sar_ratio+0x7c>)
    14da:	f7fe ff2d 	bl	338 <pmu_reg_write>
        pmu_reg_write(0x00,0x04);
    14de:	2104      	movs	r1, #4
    14e0:	2000      	movs	r0, #0
    14e2:	f7fe ff29 	bl	338 <pmu_reg_write>
        pmu_sar_ratio   = *REG0 & 0x7F;
    14e6:	6823      	ldr	r3, [r4, #0]
    14e8:	4033      	ands	r3, r6
    } while (pmu_sar_ratio != sar_ratio);
    14ea:	429d      	cmp	r5, r3
    14ec:	d1f7      	bne.n	14de <pmu_set_sar_ratio+0x4a>
    pmu_reg_write (60, // 0x3C (CTRL_DESIRED_STATE_ACTIVE)
    14ee:	203c      	movs	r0, #60	; 0x3c
    14f0:	4906      	ldr	r1, [pc, #24]	; (150c <pmu_set_sar_ratio+0x78>)
    14f2:	f7fe ff21 	bl	338 <pmu_reg_write>
    pmu_reg_write (5, // 0x05 (SAR_RATIO_OVERRIDE)
    14f6:	21a0      	movs	r1, #160	; 0xa0
    14f8:	2005      	movs	r0, #5
    14fa:	0189      	lsls	r1, r1, #6
    14fc:	f7fe ff1c 	bl	338 <pmu_reg_write>
    pmu_reg_write (2, // 0x02 (ADC_CONFIG_OVERRIDE)
    1500:	2100      	movs	r1, #0
    1502:	2002      	movs	r0, #2
    1504:	f7fe ff18 	bl	338 <pmu_reg_write>
}
    1508:	bd70      	pop	{r4, r5, r6, pc}
    150a:	46c0      	nop			; (mov r8, r8)
    150c:	0007c7ff 	.word	0x0007c7ff
    1510:	0007efff 	.word	0x0007efff

Disassembly of section .text.pmu_setting_temp_based:

00001514 <pmu_setting_temp_based>:
static void pmu_setting_temp_based(uint16_t mode) {
    1514:	2305      	movs	r3, #5
    1516:	b570      	push	{r4, r5, r6, lr}
        if(i == 0 || snt_sys_temp_code >= PMU_TEMP_THRESH[i - 1]) {
    1518:	4a13      	ldr	r2, [pc, #76]	; (1568 <pmu_setting_temp_based+0x54>)
    151a:	4914      	ldr	r1, [pc, #80]	; (156c <pmu_setting_temp_based+0x58>)
    151c:	009d      	lsls	r5, r3, #2
    151e:	58ae      	ldr	r6, [r5, r2]
    1520:	680d      	ldr	r5, [r1, #0]
    1522:	1c5c      	adds	r4, r3, #1
    1524:	b264      	sxtb	r4, r4
    1526:	42ae      	cmp	r6, r5
    1528:	d819      	bhi.n	155e <pmu_setting_temp_based+0x4a>
            if(mode == 0) {
    152a:	00a4      	lsls	r4, r4, #2
    152c:	2800      	cmp	r0, #0
    152e:	d108      	bne.n	1542 <pmu_setting_temp_based+0x2e>
                pmu_set_active_clk(PMU_ACTIVE_SETTINGS[i]);
    1530:	4b0f      	ldr	r3, [pc, #60]	; (1570 <pmu_setting_temp_based+0x5c>)
    1532:	5918      	ldr	r0, [r3, r4]
    1534:	f7fe ff32 	bl	39c <pmu_set_active_clk>
                pmu_set_sar_ratio(PMU_ACTIVE_SAR_SETTINGS[i]);
    1538:	4b0e      	ldr	r3, [pc, #56]	; (1574 <pmu_setting_temp_based+0x60>)
                pmu_set_sar_ratio(PMU_RADIO_SAR_SETTINGS[i]);
    153a:	5918      	ldr	r0, [r3, r4]
    153c:	f7ff ffaa 	bl	1494 <pmu_set_sar_ratio>
}
    1540:	bd70      	pop	{r4, r5, r6, pc}
            else if(mode == 2) {
    1542:	2802      	cmp	r0, #2
    1544:	d105      	bne.n	1552 <pmu_setting_temp_based+0x3e>
                pmu_set_sleep_clk(PMU_SLEEP_SETTINGS[i]);
    1546:	4b0c      	ldr	r3, [pc, #48]	; (1578 <pmu_setting_temp_based+0x64>)
    1548:	5918      	ldr	r0, [r3, r4]
    154a:	f7fe ff02 	bl	352 <pmu_set_sleep_clk>
                pmu_set_sar_ratio(PMU_SLEEP_SAR_SETTINGS[i]);
    154e:	4b0b      	ldr	r3, [pc, #44]	; (157c <pmu_setting_temp_based+0x68>)
    1550:	e7f3      	b.n	153a <pmu_setting_temp_based+0x26>
                pmu_set_active_clk(PMU_RADIO_SETTINGS[i]);
    1552:	4b0b      	ldr	r3, [pc, #44]	; (1580 <pmu_setting_temp_based+0x6c>)
    1554:	5918      	ldr	r0, [r3, r4]
    1556:	f7fe ff21 	bl	39c <pmu_set_active_clk>
                pmu_set_sar_ratio(PMU_RADIO_SAR_SETTINGS[i]);
    155a:	4b0a      	ldr	r3, [pc, #40]	; (1584 <pmu_setting_temp_based+0x70>)
    155c:	e7ed      	b.n	153a <pmu_setting_temp_based+0x26>
        if(i == 0 || snt_sys_temp_code >= PMU_TEMP_THRESH[i - 1]) {
    155e:	3b01      	subs	r3, #1
    1560:	d2dc      	bcs.n	151c <pmu_setting_temp_based+0x8>
    1562:	2400      	movs	r4, #0
    1564:	e7e1      	b.n	152a <pmu_setting_temp_based+0x16>
    1566:	46c0      	nop			; (mov r8, r8)
    1568:	000038ac 	.word	0x000038ac
    156c:	00003938 	.word	0x00003938
    1570:	0000380c 	.word	0x0000380c
    1574:	000037f0 	.word	0x000037f0
    1578:	00003890 	.word	0x00003890
    157c:	00003874 	.word	0x00003874
    1580:	00003858 	.word	0x00003858
    1584:	0000383c 	.word	0x0000383c

Disassembly of section .text.set_system_error:

00001588 <set_system_error>:
}


void set_system_error(uint32_t code) {
    error_code = code;
    1588:	4b05      	ldr	r3, [pc, #20]	; (15a0 <set_system_error+0x18>)
void set_system_error(uint32_t code) {
    158a:	b510      	push	{r4, lr}
    error_code = code;
    158c:	6018      	str	r0, [r3, #0]
    error_time = xo_sys_time_in_sec;
    158e:	4b05      	ldr	r3, [pc, #20]	; (15a4 <set_system_error+0x1c>)
void set_system_error(uint32_t code) {
    1590:	0001      	movs	r1, r0
    error_time = xo_sys_time_in_sec;
    1592:	681a      	ldr	r2, [r3, #0]
    1594:	4b04      	ldr	r3, [pc, #16]	; (15a8 <set_system_error+0x20>)
    mbus_write_message32(0xBF, code);
    1596:	20bf      	movs	r0, #191	; 0xbf
    error_time = xo_sys_time_in_sec;
    1598:	601a      	str	r2, [r3, #0]
    mbus_write_message32(0xBF, code);
    159a:	f7fe fdef 	bl	17c <mbus_write_message32>
}
    159e:	bd10      	pop	{r4, pc}
    15a0:	00003a24 	.word	0x00003a24
    15a4:	00003aec 	.word	0x00003aec
    15a8:	00003a28 	.word	0x00003a28

Disassembly of section .text.update_system_time:

000015ac <update_system_time>:
void update_system_time() {
    15ac:	b570      	push	{r4, r5, r6, lr}
    uint32_t last_update_val = xo_sys_time;
    15ae:	4d1a      	ldr	r5, [pc, #104]	; (1618 <update_system_time+0x6c>)
    15b0:	682e      	ldr	r6, [r5, #0]
    xo_sys_time = get_timer_cnt_xo();
    15b2:	f7ff f92d 	bl	810 <get_timer_cnt_xo>
    if(xo_sys_time == last_xo_sys_time) {
    15b6:	4b19      	ldr	r3, [pc, #100]	; (161c <update_system_time+0x70>)
    xo_sys_time = get_timer_cnt_xo();
    15b8:	6028      	str	r0, [r5, #0]
    if(xo_sys_time == last_xo_sys_time) {
    15ba:	682a      	ldr	r2, [r5, #0]
    15bc:	681b      	ldr	r3, [r3, #0]
    15be:	429a      	cmp	r2, r3
    15c0:	d102      	bne.n	15c8 <update_system_time+0x1c>
        set_system_error(0x3);
    15c2:	2003      	movs	r0, #3
    15c4:	f7ff ffe0 	bl	1588 <set_system_error>
    uint32_t diff = (xo_sys_time >> XO_TO_SEC_SHIFT) - (last_update_val >> XO_TO_SEC_SHIFT); // Have to shift individually to account for underflow
    15c8:	682c      	ldr	r4, [r5, #0]
    15ca:	0bf3      	lsrs	r3, r6, #15
    15cc:	0be4      	lsrs	r4, r4, #15
    15ce:	1ae4      	subs	r4, r4, r3
    if(xo_sys_time < last_update_val) {
    15d0:	682b      	ldr	r3, [r5, #0]
    15d2:	42b3      	cmp	r3, r6
    15d4:	d202      	bcs.n	15dc <update_system_time+0x30>
        diff += 0x20000;
    15d6:	2380      	movs	r3, #128	; 0x80
    15d8:	029b      	lsls	r3, r3, #10
    15da:	18e4      	adds	r4, r4, r3
    if(diff > MAX_UPDATE_TIME) {
    15dc:	23e1      	movs	r3, #225	; 0xe1
    15de:	011b      	lsls	r3, r3, #4
    15e0:	429c      	cmp	r4, r3
    15e2:	d902      	bls.n	15ea <update_system_time+0x3e>
        set_system_error(0x4);
    15e4:	2004      	movs	r0, #4
    15e6:	f7ff ffcf 	bl	1588 <set_system_error>
    xo_sys_time_in_sec += diff;
    15ea:	4a0d      	ldr	r2, [pc, #52]	; (1620 <update_system_time+0x74>)
    15ec:	6813      	ldr	r3, [r2, #0]
    15ee:	191b      	adds	r3, r3, r4
    15f0:	6013      	str	r3, [r2, #0]
    xo_day_time_in_sec += diff;
    15f2:	4b0c      	ldr	r3, [pc, #48]	; (1624 <update_system_time+0x78>)
    15f4:	681a      	ldr	r2, [r3, #0]
    15f6:	1914      	adds	r4, r2, r4
    15f8:	601c      	str	r4, [r3, #0]
    if(xo_day_time_in_sec >= XO_MAX_DAY_TIME_IN_SEC) {
    15fa:	6819      	ldr	r1, [r3, #0]
    15fc:	4a0a      	ldr	r2, [pc, #40]	; (1628 <update_system_time+0x7c>)
    15fe:	4291      	cmp	r1, r2
    1600:	d908      	bls.n	1614 <update_system_time+0x68>
        xo_day_time_in_sec -= XO_MAX_DAY_TIME_IN_SEC;
    1602:	681a      	ldr	r2, [r3, #0]
    1604:	4909      	ldr	r1, [pc, #36]	; (162c <update_system_time+0x80>)
    1606:	1852      	adds	r2, r2, r1
    1608:	601a      	str	r2, [r3, #0]
        day_count++;
    160a:	4a09      	ldr	r2, [pc, #36]	; (1630 <update_system_time+0x84>)
    160c:	8813      	ldrh	r3, [r2, #0]
    160e:	3301      	adds	r3, #1
    1610:	b29b      	uxth	r3, r3
    1612:	8013      	strh	r3, [r2, #0]
}
    1614:	bd70      	pop	{r4, r5, r6, pc}
    1616:	46c0      	nop			; (mov r8, r8)
    1618:	00003ae8 	.word	0x00003ae8
    161c:	00003a34 	.word	0x00003a34
    1620:	00003aec 	.word	0x00003aec
    1624:	00003ae4 	.word	0x00003ae4
    1628:	0001517f 	.word	0x0001517f
    162c:	fffeae80 	.word	0xfffeae80
    1630:	00003a18 	.word	0x00003a18

Disassembly of section .text.xo_check_is_day:

00001634 <xo_check_is_day>:
bool xo_check_is_day() {
    1634:	b510      	push	{r4, lr}
    update_system_time();
    1636:	f7ff ffb9 	bl	15ac <update_system_time>
    return (DAY_START_TIME <= xo_day_time_in_sec) && (xo_day_time_in_sec < DAY_END_TIME);
    163a:	4b07      	ldr	r3, [pc, #28]	; (1658 <xo_check_is_day+0x24>)
    163c:	2000      	movs	r0, #0
    163e:	6819      	ldr	r1, [r3, #0]
    1640:	4b06      	ldr	r3, [pc, #24]	; (165c <xo_check_is_day+0x28>)
    1642:	681a      	ldr	r2, [r3, #0]
    1644:	4291      	cmp	r1, r2
    1646:	d805      	bhi.n	1654 <xo_check_is_day+0x20>
    1648:	6818      	ldr	r0, [r3, #0]
    164a:	4b05      	ldr	r3, [pc, #20]	; (1660 <xo_check_is_day+0x2c>)
    164c:	681b      	ldr	r3, [r3, #0]
    164e:	4298      	cmp	r0, r3
    1650:	4180      	sbcs	r0, r0
    1652:	4240      	negs	r0, r0
}
    1654:	bd10      	pop	{r4, pc}
    1656:	46c0      	nop			; (mov r8, r8)
    1658:	000037c0 	.word	0x000037c0
    165c:	00003ae4 	.word	0x00003ae4
    1660:	000037bc 	.word	0x000037bc

Disassembly of section .text.operation_temp_run:

00001664 <operation_temp_run>:
static void operation_temp_run() {
    1664:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    if(snt_state == SNT_IDLE) {
    1666:	4c50      	ldr	r4, [pc, #320]	; (17a8 <operation_temp_run+0x144>)
    1668:	8823      	ldrh	r3, [r4, #0]
    166a:	b299      	uxth	r1, r3
    166c:	2b00      	cmp	r3, #0
    166e:	d10c      	bne.n	168a <operation_temp_run+0x26>
    sntv5_r00.LDO_EN_VREF = 1;
    1670:	2004      	movs	r0, #4
    1672:	4b4e      	ldr	r3, [pc, #312]	; (17ac <operation_temp_run+0x148>)
    1674:	881a      	ldrh	r2, [r3, #0]
    1676:	4302      	orrs	r2, r0
    1678:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
    167a:	681a      	ldr	r2, [r3, #0]
    167c:	f7fe fdbd 	bl	1fa <mbus_remote_register_write>
        delay(MBUS_DELAY);
    1680:	2064      	movs	r0, #100	; 0x64
    1682:	f7fe fd11 	bl	a8 <delay>
        snt_state = SNT_TEMP_LDO;
    1686:	2301      	movs	r3, #1
    1688:	8023      	strh	r3, [r4, #0]
    if(snt_state == SNT_TEMP_LDO) {
    168a:	8823      	ldrh	r3, [r4, #0]
    168c:	b29e      	uxth	r6, r3
    168e:	2b01      	cmp	r3, #1
    1690:	d12e      	bne.n	16f0 <operation_temp_run+0x8c>
    sntv5_r00.LDO_EN_IREF = 1;
    1692:	2702      	movs	r7, #2
    1694:	4b45      	ldr	r3, [pc, #276]	; (17ac <operation_temp_run+0x148>)
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
    1696:	2100      	movs	r1, #0
    sntv5_r00.LDO_EN_IREF = 1;
    1698:	881a      	ldrh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
    169a:	2004      	movs	r0, #4
    sntv5_r00.LDO_EN_IREF = 1;
    169c:	433a      	orrs	r2, r7
    169e:	801a      	strh	r2, [r3, #0]
    sntv5_r00.LDO_EN_LDO  = 1;
    16a0:	881a      	ldrh	r2, [r3, #0]
    16a2:	4332      	orrs	r2, r6
    16a4:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
    16a6:	681a      	ldr	r2, [r3, #0]
    16a8:	f7fe fda7 	bl	1fa <mbus_remote_register_write>
    sntv5_r01.TSNS_SEL_LDO = 1;
    16ac:	2308      	movs	r3, #8
    16ae:	4d40      	ldr	r5, [pc, #256]	; (17b0 <operation_temp_run+0x14c>)
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    16b0:	0031      	movs	r1, r6
    sntv5_r01.TSNS_SEL_LDO = 1;
    16b2:	882a      	ldrh	r2, [r5, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    16b4:	2004      	movs	r0, #4
    sntv5_r01.TSNS_SEL_LDO = 1;
    16b6:	4313      	orrs	r3, r2
    16b8:	802b      	strh	r3, [r5, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    16ba:	682a      	ldr	r2, [r5, #0]
    16bc:	f7fe fd9d 	bl	1fa <mbus_remote_register_write>
    sntv5_r01.TSNS_EN_SENSOR_LDO = 1;
    16c0:	2320      	movs	r3, #32
    16c2:	882a      	ldrh	r2, [r5, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    16c4:	0031      	movs	r1, r6
    sntv5_r01.TSNS_EN_SENSOR_LDO = 1;
    16c6:	4313      	orrs	r3, r2
    16c8:	802b      	strh	r3, [r5, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    16ca:	682a      	ldr	r2, [r5, #0]
    16cc:	2004      	movs	r0, #4
    16ce:	f7fe fd94 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY);
    16d2:	2064      	movs	r0, #100	; 0x64
    16d4:	f7fe fce8 	bl	a8 <delay>
    sntv5_r01.TSNS_ISOLATE = 0;
    16d8:	882b      	ldrh	r3, [r5, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    16da:	0031      	movs	r1, r6
    sntv5_r01.TSNS_ISOLATE = 0;
    16dc:	43bb      	bics	r3, r7
    16de:	802b      	strh	r3, [r5, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    16e0:	2004      	movs	r0, #4
    16e2:	682a      	ldr	r2, [r5, #0]
    16e4:	f7fe fd89 	bl	1fa <mbus_remote_register_write>
        delay(MBUS_DELAY);
    16e8:	2064      	movs	r0, #100	; 0x64
    16ea:	f7fe fcdd 	bl	a8 <delay>
        snt_state = SNT_TEMP_START;
    16ee:	8027      	strh	r7, [r4, #0]
    if(snt_state == SNT_TEMP_START) {
    16f0:	8823      	ldrh	r3, [r4, #0]
    16f2:	2b02      	cmp	r3, #2
    16f4:	d11d      	bne.n	1732 <operation_temp_run+0xce>
        wfi_timeout_flag = 0;
    16f6:	2500      	movs	r5, #0
        config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default
    16f8:	20a0      	movs	r0, #160	; 0xa0
        wfi_timeout_flag = 0;
    16fa:	4b2e      	ldr	r3, [pc, #184]	; (17b4 <operation_temp_run+0x150>)
        config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default
    16fc:	002a      	movs	r2, r5
    16fe:	2101      	movs	r1, #1
        wfi_timeout_flag = 0;
    1700:	801d      	strh	r5, [r3, #0]
        config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default
    1702:	0300      	lsls	r0, r0, #12
    1704:	002b      	movs	r3, r5
    1706:	f7fe fcd9 	bl	bc <config_timer32>
    sntv5_r01.TSNS_RESETn = 1;
    170a:	2101      	movs	r1, #1
    170c:	4b28      	ldr	r3, [pc, #160]	; (17b0 <operation_temp_run+0x14c>)
    170e:	881a      	ldrh	r2, [r3, #0]
    1710:	430a      	orrs	r2, r1
    1712:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_EN_IRQ = 1;
    1714:	2280      	movs	r2, #128	; 0x80
    1716:	8818      	ldrh	r0, [r3, #0]
    1718:	0052      	lsls	r2, r2, #1
    171a:	4302      	orrs	r2, r0
    171c:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    171e:	681a      	ldr	r2, [r3, #0]
    1720:	2004      	movs	r0, #4
    1722:	f7fe fd6a 	bl	1fa <mbus_remote_register_write>
        WFI();
    1726:	f7fe fcc6 	bl	b6 <WFI>
        *TIMER32_GO = 0;
    172a:	4b23      	ldr	r3, [pc, #140]	; (17b8 <operation_temp_run+0x154>)
    172c:	601d      	str	r5, [r3, #0]
        snt_state = SNT_TEMP_READ;
    172e:	2303      	movs	r3, #3
    1730:	8023      	strh	r3, [r4, #0]
    if(snt_state == SNT_TEMP_READ) {
    1732:	8823      	ldrh	r3, [r4, #0]
    1734:	2b03      	cmp	r3, #3
    1736:	d135      	bne.n	17a4 <operation_temp_run+0x140>
        if(wfi_timeout_flag) {
    1738:	4b1e      	ldr	r3, [pc, #120]	; (17b4 <operation_temp_run+0x150>)
    173a:	881b      	ldrh	r3, [r3, #0]
    173c:	b29e      	uxth	r6, r3
    173e:	2b00      	cmp	r3, #0
    1740:	d005      	beq.n	174e <operation_temp_run+0xea>

static void sys_err(uint32_t code)
{
    set_system_error(code);
    1742:	2080      	movs	r0, #128	; 0x80
    1744:	0440      	lsls	r0, r0, #17
    1746:	f7ff ff1f 	bl	1588 <set_system_error>

    // mbus_write_message32(0xAF, code);
    operation_sleep();
    174a:	f7fe fee3 	bl	514 <operation_sleep>
            snt_sys_temp_code = *REG0;
    174e:	23a0      	movs	r3, #160	; 0xa0
    sntv5_r01.TSNS_RESETn        = 0;
    1750:	2501      	movs	r5, #1
    sntv5_r01.TSNS_SEL_LDO       = 0;
    1752:	2108      	movs	r1, #8
    sntv5_r01.TSNS_ISOLATE       = 1;
    1754:	2702      	movs	r7, #2
            snt_sys_temp_code = *REG0;
    1756:	061b      	lsls	r3, r3, #24
    1758:	681a      	ldr	r2, [r3, #0]
    175a:	4b18      	ldr	r3, [pc, #96]	; (17bc <operation_temp_run+0x158>)
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    175c:	2004      	movs	r0, #4
            snt_sys_temp_code = *REG0;
    175e:	601a      	str	r2, [r3, #0]
    sntv5_r01.TSNS_RESETn        = 0;
    1760:	4b13      	ldr	r3, [pc, #76]	; (17b0 <operation_temp_run+0x14c>)
    1762:	881a      	ldrh	r2, [r3, #0]
    1764:	43aa      	bics	r2, r5
    1766:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_SEL_LDO       = 0;
    1768:	881a      	ldrh	r2, [r3, #0]
    176a:	438a      	bics	r2, r1
    176c:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_EN_SENSOR_LDO = 0;
    176e:	881a      	ldrh	r2, [r3, #0]
    1770:	3118      	adds	r1, #24
    1772:	438a      	bics	r2, r1
    1774:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_ISOLATE       = 1;
    1776:	881a      	ldrh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    1778:	0029      	movs	r1, r5
    sntv5_r01.TSNS_ISOLATE       = 1;
    177a:	433a      	orrs	r2, r7
    177c:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    177e:	681a      	ldr	r2, [r3, #0]
    1780:	f7fe fd3b 	bl	1fa <mbus_remote_register_write>
    sntv5_r00.LDO_EN_VREF = 0;
    1784:	2004      	movs	r0, #4
    1786:	4b09      	ldr	r3, [pc, #36]	; (17ac <operation_temp_run+0x148>)
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
    1788:	0031      	movs	r1, r6
    sntv5_r00.LDO_EN_VREF = 0;
    178a:	881a      	ldrh	r2, [r3, #0]
    178c:	4382      	bics	r2, r0
    178e:	801a      	strh	r2, [r3, #0]
    sntv5_r00.LDO_EN_IREF = 0;
    1790:	881a      	ldrh	r2, [r3, #0]
    1792:	43ba      	bics	r2, r7
    1794:	801a      	strh	r2, [r3, #0]
    sntv5_r00.LDO_EN_LDO  = 0;
    1796:	881a      	ldrh	r2, [r3, #0]
    1798:	43aa      	bics	r2, r5
    179a:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
    179c:	681a      	ldr	r2, [r3, #0]
    179e:	f7fe fd2c 	bl	1fa <mbus_remote_register_write>
            snt_state = SNT_IDLE;
    17a2:	8026      	strh	r6, [r4, #0]
}
    17a4:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    17a6:	46c0      	nop			; (mov r8, r8)
    17a8:	00003acc 	.word	0x00003acc
    17ac:	0000393c 	.word	0x0000393c
    17b0:	00003940 	.word	0x00003940
    17b4:	00003ae2 	.word	0x00003ae2
    17b8:	a0001100 	.word	0xa0001100
    17bc:	00003938 	.word	0x00003938

Disassembly of section .text.mrr_send_radio_data:

000017c0 <mrr_send_radio_data>:
static void mrr_send_radio_data(uint16_t last_packet) {
    17c0:	b5f0      	push	{r4, r5, r6, r7, lr}
    radio_data_arr[2] = radio_data_arr[2] & 0x0000FFFF;
    17c2:	4eb7      	ldr	r6, [pc, #732]	; (1aa0 <mrr_send_radio_data+0x2e0>)
static void mrr_send_radio_data(uint16_t last_packet) {
    17c4:	b085      	sub	sp, #20
    radio_data_arr[2] = radio_data_arr[2] & 0x0000FFFF;
    17c6:	68b3      	ldr	r3, [r6, #8]
static void mrr_send_radio_data(uint16_t last_packet) {
    17c8:	9003      	str	r0, [sp, #12]
    radio_data_arr[2] = radio_data_arr[2] & 0x0000FFFF;
    17ca:	b29b      	uxth	r3, r3
    17cc:	60b3      	str	r3, [r6, #8]
    uint32_t* crc_data = crcEnc16();
    17ce:	f7ff fe1d 	bl	140c <crcEnc16>
    if(!radio_on) {
    17d2:	4bb4      	ldr	r3, [pc, #720]	; (1aa4 <mrr_send_radio_data+0x2e4>)
    uint32_t* crc_data = crcEnc16();
    17d4:	9001      	str	r0, [sp, #4]
    if(!radio_on) {
    17d6:	881a      	ldrh	r2, [r3, #0]
    17d8:	b297      	uxth	r7, r2
    17da:	2a00      	cmp	r2, #0
    17dc:	d000      	beq.n	17e0 <mrr_send_radio_data+0x20>
    17de:	e097      	b.n	1910 <mrr_send_radio_data+0x150>
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
    17e0:	4db1      	ldr	r5, [pc, #708]	; (1aa8 <mrr_send_radio_data+0x2e8>)
        radio_on = 1;
    17e2:	3201      	adds	r2, #1
    17e4:	801a      	strh	r2, [r3, #0]
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
    17e6:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    17e8:	0039      	movs	r1, r7
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
    17ea:	4393      	bics	r3, r2
    17ec:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    17ee:	682a      	ldr	r2, [r5, #0]
    17f0:	2002      	movs	r0, #2
    17f2:	f7fe fd02 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r03.TRX_DCP_S_OW1 = 0;  //TX_Decap S (forced charge decaps)
    17f6:	4cad      	ldr	r4, [pc, #692]	; (1aac <mrr_send_radio_data+0x2ec>)
    17f8:	4aad      	ldr	r2, [pc, #692]	; (1ab0 <mrr_send_radio_data+0x2f0>)
    17fa:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    17fc:	2103      	movs	r1, #3
    mrrv11a_r03.TRX_DCP_S_OW1 = 0;  //TX_Decap S (forced charge decaps)
    17fe:	4013      	ands	r3, r2
    1800:	6023      	str	r3, [r4, #0]
    mrrv11a_r03.TRX_DCP_S_OW2 = 0;  //TX_Decap S (forced charge decaps)
    1802:	6823      	ldr	r3, [r4, #0]
    1804:	4aab      	ldr	r2, [pc, #684]	; (1ab4 <mrr_send_radio_data+0x2f4>)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    1806:	2002      	movs	r0, #2
    mrrv11a_r03.TRX_DCP_S_OW2 = 0;  //TX_Decap S (forced charge decaps)
    1808:	4013      	ands	r3, r2
    180a:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    180c:	6822      	ldr	r2, [r4, #0]
    180e:	f7fe fcf4 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r03.TRX_DCP_P_OW1 = 1;  //RX_Decap P 
    1812:	2380      	movs	r3, #128	; 0x80
    1814:	6822      	ldr	r2, [r4, #0]
    1816:	039b      	lsls	r3, r3, #14
    1818:	4313      	orrs	r3, r2
    181a:	6023      	str	r3, [r4, #0]
    mrrv11a_r03.TRX_DCP_P_OW2 = 1;  //RX_Decap P 
    181c:	2380      	movs	r3, #128	; 0x80
    181e:	6822      	ldr	r2, [r4, #0]
    1820:	035b      	lsls	r3, r3, #13
    1822:	4313      	orrs	r3, r2
    1824:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    1826:	6822      	ldr	r2, [r4, #0]
    1828:	2103      	movs	r1, #3
    182a:	2002      	movs	r0, #2
    182c:	f7fe fce5 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY);
    1830:	2064      	movs	r0, #100	; 0x64
    1832:	f7fe fc39 	bl	a8 <delay>
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    1836:	6823      	ldr	r3, [r4, #0]
    1838:	4a9f      	ldr	r2, [pc, #636]	; (1ab8 <mrr_send_radio_data+0x2f8>)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    183a:	2103      	movs	r1, #3
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    183c:	4013      	ands	r3, r2
    183e:	6023      	str	r3, [r4, #0]
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    1840:	6823      	ldr	r3, [r4, #0]
    1842:	4a9e      	ldr	r2, [pc, #632]	; (1abc <mrr_send_radio_data+0x2fc>)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    1844:	2002      	movs	r0, #2
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    1846:	4013      	ands	r3, r2
    1848:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    184a:	6822      	ldr	r2, [r4, #0]
    184c:	f7fe fcd5 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
    1850:	2380      	movs	r3, #128	; 0x80
    1852:	6822      	ldr	r2, [r4, #0]
    1854:	031b      	lsls	r3, r3, #12
    1856:	4313      	orrs	r3, r2
    1858:	6023      	str	r3, [r4, #0]
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    185a:	2380      	movs	r3, #128	; 0x80
    185c:	6822      	ldr	r2, [r4, #0]
    185e:	02db      	lsls	r3, r3, #11
    1860:	4313      	orrs	r3, r2
    1862:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    1864:	6822      	ldr	r2, [r4, #0]
    1866:	2103      	movs	r1, #3
    1868:	2002      	movs	r0, #2
    186a:	f7fe fcc6 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY);
    186e:	2064      	movs	r0, #100	; 0x64
    1870:	f7fe fc1a 	bl	a8 <delay>
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    1874:	227e      	movs	r2, #126	; 0x7e
    1876:	2420      	movs	r4, #32
    1878:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    187a:	0039      	movs	r1, r7
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    187c:	4393      	bics	r3, r2
    187e:	4323      	orrs	r3, r4
    1880:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1882:	682a      	ldr	r2, [r5, #0]
    1884:	2002      	movs	r0, #2
    1886:	f7fe fcb8 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    188a:	2201      	movs	r2, #1
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    188c:	0039      	movs	r1, r7
    mrrv11a_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    188e:	2701      	movs	r7, #1
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    1890:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1892:	2002      	movs	r0, #2
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    1894:	4313      	orrs	r3, r2
    1896:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1898:	682a      	ldr	r2, [r5, #0]
    189a:	f7fe fcae 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    189e:	4d88      	ldr	r5, [pc, #544]	; (1ac0 <mrr_send_radio_data+0x300>)
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    18a0:	2104      	movs	r1, #4
    mrrv11a_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    18a2:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    18a4:	2002      	movs	r0, #2
    mrrv11a_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    18a6:	433b      	orrs	r3, r7
    18a8:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    18aa:	682a      	ldr	r2, [r5, #0]
    18ac:	f7fe fca5 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY);
    18b0:	2064      	movs	r0, #100	; 0x64
    18b2:	f7fe fbf9 	bl	a8 <delay>
    mrrv11a_r04.RO_RESET = 0;  //Release Reset TIMER
    18b6:	2208      	movs	r2, #8
    18b8:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    18ba:	2104      	movs	r1, #4
    mrrv11a_r04.RO_RESET = 0;  //Release Reset TIMER
    18bc:	4393      	bics	r3, r2
    18be:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    18c0:	682a      	ldr	r2, [r5, #0]
    18c2:	2002      	movs	r0, #2
    18c4:	f7fe fc99 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY);
    18c8:	2064      	movs	r0, #100	; 0x64
    18ca:	f7fe fbed 	bl	a8 <delay>
    mrrv11a_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    18ce:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    18d0:	2104      	movs	r1, #4
    mrrv11a_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    18d2:	431c      	orrs	r4, r3
    18d4:	602c      	str	r4, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    18d6:	682a      	ldr	r2, [r5, #0]
    18d8:	2002      	movs	r0, #2
    18da:	f7fe fc8e 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY);
    18de:	2064      	movs	r0, #100	; 0x64
    18e0:	f7fe fbe2 	bl	a8 <delay>
    mrrv11a_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    18e4:	2210      	movs	r2, #16
    18e6:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    18e8:	2104      	movs	r1, #4
    mrrv11a_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    18ea:	4393      	bics	r3, r2
    18ec:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    18ee:	2002      	movs	r0, #2
    18f0:	682a      	ldr	r2, [r5, #0]
    18f2:	f7fe fc82 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r11.FSM_SLEEP = 0;  // Power on BB
    18f6:	4b73      	ldr	r3, [pc, #460]	; (1ac4 <mrr_send_radio_data+0x304>)
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    18f8:	2002      	movs	r0, #2
    mrrv11a_r11.FSM_SLEEP = 0;  // Power on BB
    18fa:	681a      	ldr	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    18fc:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_SLEEP = 0;  // Power on BB
    18fe:	43ba      	bics	r2, r7
    1900:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1902:	681a      	ldr	r2, [r3, #0]
    1904:	f7fe fc79 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*5); // Freq stab
    1908:	20fa      	movs	r0, #250	; 0xfa
    190a:	0040      	lsls	r0, r0, #1
    190c:	f7fe fbcc 	bl	a8 <delay>
    radio_data_arr[2] = crc_data[0] << 16 | radio_data_arr[2];
    1910:	9b01      	ldr	r3, [sp, #4]
    1912:	68b2      	ldr	r2, [r6, #8]
    1914:	681b      	ldr	r3, [r3, #0]
    mbus_copy_mem_from_local_to_remote_bulk(MRR_ADDR, (uint32_t *) 0x00000000, (uint32_t *) radio_data_arr, 2);
    1916:	2100      	movs	r1, #0
    radio_data_arr[2] = crc_data[0] << 16 | radio_data_arr[2];
    1918:	041b      	lsls	r3, r3, #16
    191a:	4313      	orrs	r3, r2
    191c:	60b3      	str	r3, [r6, #8]
    mbus_copy_mem_from_local_to_remote_bulk(MRR_ADDR, (uint32_t *) 0x00000000, (uint32_t *) radio_data_arr, 2);
    191e:	2302      	movs	r3, #2
    1920:	0032      	movs	r2, r6
    1922:	0018      	movs	r0, r3
    1924:	f7fe fc76 	bl	214 <mbus_copy_mem_from_local_to_remote_bulk>
    if (!radio_ready){
    1928:	4b67      	ldr	r3, [pc, #412]	; (1ac8 <mrr_send_radio_data+0x308>)
    192a:	881a      	ldrh	r2, [r3, #0]
    192c:	2a00      	cmp	r2, #0
    192e:	d10f      	bne.n	1950 <mrr_send_radio_data+0x190>
        radio_ready = 1;
    1930:	3201      	adds	r2, #1
    1932:	801a      	strh	r2, [r3, #0]
        mrrv11a_r03.TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    1934:	2280      	movs	r2, #128	; 0x80
    1936:	4b5d      	ldr	r3, [pc, #372]	; (1aac <mrr_send_radio_data+0x2ec>)
    1938:	03d2      	lsls	r2, r2, #15
    193a:	6819      	ldr	r1, [r3, #0]
        mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    193c:	2002      	movs	r0, #2
        mrrv11a_r03.TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    193e:	430a      	orrs	r2, r1
    1940:	601a      	str	r2, [r3, #0]
        mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    1942:	2103      	movs	r1, #3
    1944:	681a      	ldr	r2, [r3, #0]
    1946:	f7fe fc58 	bl	1fa <mbus_remote_register_write>
        delay(MBUS_DELAY);
    194a:	2064      	movs	r0, #100	; 0x64
    194c:	f7fe fbac 	bl	a8 <delay>
    mrrv11a_r00.TRX_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    1950:	217e      	movs	r1, #126	; 0x7e
    1952:	2002      	movs	r0, #2
    1954:	4a54      	ldr	r2, [pc, #336]	; (1aa8 <mrr_send_radio_data+0x2e8>)
    1956:	6813      	ldr	r3, [r2, #0]
    1958:	438b      	bics	r3, r1
    195a:	4303      	orrs	r3, r0
    195c:	6013      	str	r3, [r2, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    195e:	2100      	movs	r1, #0
    1960:	6812      	ldr	r2, [r2, #0]
    1962:	f7fe fc4a 	bl	1fa <mbus_remote_register_write>
    uint16_t count = 0;
    1966:	2300      	movs	r3, #0
    1968:	9301      	str	r3, [sp, #4]
    while (count < mrr_freq_hopping){
    196a:	4b58      	ldr	r3, [pc, #352]	; (1acc <mrr_send_radio_data+0x30c>)
    196c:	9a01      	ldr	r2, [sp, #4]
    196e:	881b      	ldrh	r3, [r3, #0]
    1970:	4293      	cmp	r3, r2
    1972:	d80d      	bhi.n	1990 <mrr_send_radio_data+0x1d0>
    radio_packet_count++;
    1974:	4a56      	ldr	r2, [pc, #344]	; (1ad0 <mrr_send_radio_data+0x310>)
    1976:	6813      	ldr	r3, [r2, #0]
    1978:	3301      	adds	r3, #1
    197a:	6013      	str	r3, [r2, #0]
    if (last_packet){
    197c:	9b03      	ldr	r3, [sp, #12]
    197e:	2b00      	cmp	r3, #0
    1980:	d004      	beq.n	198c <mrr_send_radio_data+0x1cc>
    radio_ready = 0;
    1982:	2200      	movs	r2, #0
    1984:	4b50      	ldr	r3, [pc, #320]	; (1ac8 <mrr_send_radio_data+0x308>)
    1986:	801a      	strh	r2, [r3, #0]
    radio_power_off();
    1988:	f7fe fd54 	bl	434 <radio_power_off>
}
    198c:	b005      	add	sp, #20
    198e:	bdf0      	pop	{r4, r5, r6, r7, pc}
    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    1990:	2500      	movs	r5, #0
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
    1992:	203f      	movs	r0, #63	; 0x3f
    count++;
    1994:	9b01      	ldr	r3, [sp, #4]
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
    1996:	4a4f      	ldr	r2, [pc, #316]	; (1ad4 <mrr_send_radio_data+0x314>)
    count++;
    1998:	3301      	adds	r3, #1
    199a:	b29b      	uxth	r3, r3
    199c:	9301      	str	r3, [sp, #4]
    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    199e:	4b4e      	ldr	r3, [pc, #312]	; (1ad8 <mrr_send_radio_data+0x318>)
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
    19a0:	4c4e      	ldr	r4, [pc, #312]	; (1adc <mrr_send_radio_data+0x31c>)
    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    19a2:	601d      	str	r5, [r3, #0]
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    19a4:	4b4e      	ldr	r3, [pc, #312]	; (1ae0 <mrr_send_radio_data+0x320>)
    mrrv11a_r11.FSM_RESET_B = 1;    
    19a6:	2702      	movs	r7, #2
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    19a8:	601d      	str	r5, [r3, #0]
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
    19aa:	4b4e      	ldr	r3, [pc, #312]	; (1ae4 <mrr_send_radio_data+0x324>)
    19ac:	6811      	ldr	r1, [r2, #0]
    19ae:	881b      	ldrh	r3, [r3, #0]
    19b0:	4021      	ands	r1, r4
    19b2:	4003      	ands	r3, r0
    19b4:	0418      	lsls	r0, r3, #16
    19b6:	4301      	orrs	r1, r0
    19b8:	6011      	str	r1, [r2, #0]
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine;
    19ba:	6811      	ldr	r1, [r2, #0]
    19bc:	0298      	lsls	r0, r3, #10
    19be:	4b4a      	ldr	r3, [pc, #296]	; (1ae8 <mrr_send_radio_data+0x328>)
    19c0:	400b      	ands	r3, r1
    19c2:	4303      	orrs	r3, r0
    19c4:	6013      	str	r3, [r2, #0]
    mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
    19c6:	2101      	movs	r1, #1
    19c8:	2002      	movs	r0, #2
    19ca:	6812      	ldr	r2, [r2, #0]
    19cc:	f7fe fc15 	bl	1fa <mbus_remote_register_write>
    config_timer32(val, 1, 0, 0);
    19d0:	20a0      	movs	r0, #160	; 0xa0
    wfi_timeout_flag = 0;
    19d2:	4b46      	ldr	r3, [pc, #280]	; (1aec <mrr_send_radio_data+0x32c>)
    config_timer32(val, 1, 0, 0);
    19d4:	002a      	movs	r2, r5
    19d6:	2101      	movs	r1, #1
    wfi_timeout_flag = 0;
    19d8:	801d      	strh	r5, [r3, #0]
    config_timer32(val, 1, 0, 0);
    19da:	0300      	lsls	r0, r0, #12
    19dc:	002b      	movs	r3, r5
    19de:	f7fe fb6d 	bl	bc <config_timer32>
    mrrv11a_r00.TRX_CL_EN = 1;
    19e2:	2201      	movs	r2, #1
    19e4:	4e30      	ldr	r6, [pc, #192]	; (1aa8 <mrr_send_radio_data+0x2e8>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    19e6:	0029      	movs	r1, r5
    mrrv11a_r00.TRX_CL_EN = 1;
    19e8:	6833      	ldr	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    19ea:	2002      	movs	r0, #2
    mrrv11a_r00.TRX_CL_EN = 1;
    19ec:	4313      	orrs	r3, r2
    19ee:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    19f0:	6832      	ldr	r2, [r6, #0]
    19f2:	f7fe fc02 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r11.FSM_RESET_B = 1;    
    19f6:	4c33      	ldr	r4, [pc, #204]	; (1ac4 <mrr_send_radio_data+0x304>)
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);    
    19f8:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_RESET_B = 1;    
    19fa:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);    
    19fc:	0038      	movs	r0, r7
    mrrv11a_r11.FSM_RESET_B = 1;    
    19fe:	433b      	orrs	r3, r7
    1a00:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);    
    1a02:	6822      	ldr	r2, [r4, #0]
    1a04:	f7fe fbf9 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r11.FSM_EN = 1;  //Start BB
    1a08:	2204      	movs	r2, #4
    1a0a:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1a0c:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_EN = 1;  //Start BB
    1a0e:	4313      	orrs	r3, r2
    1a10:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1a12:	6822      	ldr	r2, [r4, #0]
    1a14:	0038      	movs	r0, r7
    1a16:	f7fe fbf0 	bl	1fa <mbus_remote_register_write>
    WFI();
    1a1a:	f7fe fb4c 	bl	b6 <WFI>
    *TIMER32_GO = 0;
    1a1e:	4b34      	ldr	r3, [pc, #208]	; (1af0 <mrr_send_radio_data+0x330>)
    1a20:	601d      	str	r5, [r3, #0]
    if (wfi_timeout_flag){
    1a22:	4b32      	ldr	r3, [pc, #200]	; (1aec <mrr_send_radio_data+0x32c>)
    1a24:	881b      	ldrh	r3, [r3, #0]
    1a26:	b29a      	uxth	r2, r3
    1a28:	9202      	str	r2, [sp, #8]
    1a2a:	42ab      	cmp	r3, r5
    1a2c:	d007      	beq.n	1a3e <mrr_send_radio_data+0x27e>
    set_system_error(code);
    1a2e:	2080      	movs	r0, #128	; 0x80
        wfi_timeout_flag = 0;
    1a30:	4b2e      	ldr	r3, [pc, #184]	; (1aec <mrr_send_radio_data+0x32c>)
    set_system_error(code);
    1a32:	04c0      	lsls	r0, r0, #19
        wfi_timeout_flag = 0;
    1a34:	801d      	strh	r5, [r3, #0]
    set_system_error(code);
    1a36:	f7ff fda7 	bl	1588 <set_system_error>
    operation_sleep();
    1a3a:	f7fe fd6b 	bl	514 <operation_sleep>
    mrrv11a_r00.TRX_CL_EN = 0;
    1a3e:	2201      	movs	r2, #1
    1a40:	6833      	ldr	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1a42:	0038      	movs	r0, r7
    mrrv11a_r00.TRX_CL_EN = 0;
    1a44:	4393      	bics	r3, r2
    1a46:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1a48:	6832      	ldr	r2, [r6, #0]
    1a4a:	9902      	ldr	r1, [sp, #8]
    1a4c:	f7fe fbd5 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r11.FSM_EN = 0;
    1a50:	2204      	movs	r2, #4
    1a52:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1a54:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_EN = 0;
    1a56:	4393      	bics	r3, r2
    1a58:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1a5a:	0038      	movs	r0, r7
    1a5c:	6822      	ldr	r2, [r4, #0]
    1a5e:	f7fe fbcc 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r11.FSM_RESET_B = 0;
    1a62:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1a64:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_RESET_B = 0;
    1a66:	43bb      	bics	r3, r7
    1a68:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1a6a:	0038      	movs	r0, r7
    1a6c:	6822      	ldr	r2, [r4, #0]
    1a6e:	f7fe fbc4 	bl	1fa <mbus_remote_register_write>
    config_timer32(RADIO_PACKET_DELAY, 1, 0, 0);
    1a72:	9b02      	ldr	r3, [sp, #8]
    1a74:	481f      	ldr	r0, [pc, #124]	; (1af4 <mrr_send_radio_data+0x334>)
    1a76:	001a      	movs	r2, r3
    1a78:	2101      	movs	r1, #1
    1a7a:	6800      	ldr	r0, [r0, #0]
    1a7c:	f7fe fb1e 	bl	bc <config_timer32>
    WFI();
    1a80:	f7fe fb19 	bl	b6 <WFI>
    mrr_cfo_val_fine += 8;
    1a84:	4b17      	ldr	r3, [pc, #92]	; (1ae4 <mrr_send_radio_data+0x324>)
    1a86:	881b      	ldrh	r3, [r3, #0]
    1a88:	001a      	movs	r2, r3
    1a8a:	3208      	adds	r2, #8
    1a8c:	b292      	uxth	r2, r2
    if(mrr_cfo_val_fine > 20) {
    1a8e:	2a14      	cmp	r2, #20
    1a90:	d802      	bhi.n	1a98 <mrr_send_radio_data+0x2d8>
    mrr_cfo_val_fine += 8;
    1a92:	4b14      	ldr	r3, [pc, #80]	; (1ae4 <mrr_send_radio_data+0x324>)
    1a94:	801a      	strh	r2, [r3, #0]
    1a96:	e768      	b.n	196a <mrr_send_radio_data+0x1aa>
        mrr_cfo_val_fine -= 20;
    1a98:	4a12      	ldr	r2, [pc, #72]	; (1ae4 <mrr_send_radio_data+0x324>)
    1a9a:	3b0c      	subs	r3, #12
    1a9c:	8013      	strh	r3, [r2, #0]
    1a9e:	e764      	b.n	196a <mrr_send_radio_data+0x1aa>
    1aa0:	000039b8 	.word	0x000039b8
    1aa4:	000039fe 	.word	0x000039fe
    1aa8:	0000390c 	.word	0x0000390c
    1aac:	00003910 	.word	0x00003910
    1ab0:	fff7ffff 	.word	0xfff7ffff
    1ab4:	fffbffff 	.word	0xfffbffff
    1ab8:	ffdfffff 	.word	0xffdfffff
    1abc:	ffefffff 	.word	0xffefffff
    1ac0:	00003914 	.word	0x00003914
    1ac4:	00003920 	.word	0x00003920
    1ac8:	00003956 	.word	0x00003956
    1acc:	00003958 	.word	0x00003958
    1ad0:	000039f0 	.word	0x000039f0
    1ad4:	00003a60 	.word	0x00003a60
    1ad8:	a0001200 	.word	0xa0001200
    1adc:	ffc0ffff 	.word	0xffc0ffff
    1ae0:	a000007c 	.word	0xa000007c
    1ae4:	00003a5c 	.word	0x00003a5c
    1ae8:	ffff03ff 	.word	0xffff03ff
    1aec:	00003ae2 	.word	0x00003ae2
    1af0:	a0001100 	.word	0xa0001100
    1af4:	000038e0 	.word	0x000038e0

Disassembly of section .text.set_goc_cmd:

00001af8 <set_goc_cmd>:

/**********************************************
 * Interrupt handlers
 **********************************************/

void set_goc_cmd() {
    1af8:	b570      	push	{r4, r5, r6, lr}
    lnt_stop();

    // goc_component = (*GOC_DATA_IRQ >> 24) & 0xFF;
    // goc_func_id = (*GOC_DATA_IRQ >> 16) & 0xFF;
    // goc_data = *GOC_DATA_IRQ & 0xFFFF;
    projected_end_time_in_sec = 0;
    1afa:	2400      	movs	r4, #0
    goc_data_full = *GOC_DATA_IRQ;
    1afc:	258c      	movs	r5, #140	; 0x8c
    lnt_stop();
    1afe:	f7fe fdb5 	bl	66c <lnt_stop>
    projected_end_time_in_sec = 0;
    1b02:	4b0a      	ldr	r3, [pc, #40]	; (1b2c <set_goc_cmd+0x34>)
    1b04:	601c      	str	r4, [r3, #0]
    goc_data_full = *GOC_DATA_IRQ;
    1b06:	682a      	ldr	r2, [r5, #0]
    1b08:	4b09      	ldr	r3, [pc, #36]	; (1b30 <set_goc_cmd+0x38>)
    1b0a:	601a      	str	r2, [r3, #0]
    goc_state = 0;
    1b0c:	4b09      	ldr	r3, [pc, #36]	; (1b34 <set_goc_cmd+0x3c>)
    1b0e:	801c      	strh	r4, [r3, #0]
    update_system_time();
    1b10:	f7ff fd4c 	bl	15ac <update_system_time>

    if((*GOC_DATA_IRQ >> 24) == 0x06) {
    1b14:	682b      	ldr	r3, [r5, #0]
    1b16:	0e1b      	lsrs	r3, r3, #24
    1b18:	2b06      	cmp	r3, #6
    1b1a:	d105      	bne.n	1b28 <set_goc_cmd+0x30>
        // Timeout trigger implemented here to ensure that it's run
        *TIMERWD_GO = 0x1; // Turn on CPU watchdog timer
    1b1c:	2201      	movs	r2, #1
    1b1e:	4b06      	ldr	r3, [pc, #24]	; (1b38 <set_goc_cmd+0x40>)
    1b20:	601a      	str	r2, [r3, #0]
        *REG_MBUS_WD = 0; // Disable Mbus watchdog timer
    1b22:	4b06      	ldr	r3, [pc, #24]	; (1b3c <set_goc_cmd+0x44>)
    1b24:	601c      	str	r4, [r3, #0]

        while(1);
    1b26:	e7fe      	b.n	1b26 <set_goc_cmd+0x2e>
    }
}
    1b28:	bd70      	pop	{r4, r5, r6, pc}
    1b2a:	46c0      	nop			; (mov r8, r8)
    1b2c:	00003a80 	.word	0x00003a80
    1b30:	000039f8 	.word	0x000039f8
    1b34:	000039ec 	.word	0x000039ec
    1b38:	a0001200 	.word	0xa0001200
    1b3c:	a000007c 	.word	0xa000007c

Disassembly of section .text.handler_ext_int_wakeup:

00001b40 <handler_ext_int_wakeup>:
void handler_ext_int_reg1       (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2       (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3       (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_wakeup( void ) { // WAKEUP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP);
    1b40:	2201      	movs	r2, #1
    1b42:	4b01      	ldr	r3, [pc, #4]	; (1b48 <handler_ext_int_wakeup+0x8>)
    1b44:	601a      	str	r2, [r3, #0]
}
    1b46:	4770      	bx	lr
    1b48:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_gocep:

00001b4c <handler_ext_int_gocep>:

void handler_ext_int_gocep( void ) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
    1b4c:	2204      	movs	r2, #4
    1b4e:	4b01      	ldr	r3, [pc, #4]	; (1b54 <handler_ext_int_gocep+0x8>)
    1b50:	601a      	str	r2, [r3, #0]
}
    1b52:	4770      	bx	lr
    1b54:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_timer32:

00001b58 <handler_ext_int_timer32>:

void handler_ext_int_timer32( void ) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    1b58:	2208      	movs	r2, #8
    1b5a:	4b04      	ldr	r3, [pc, #16]	; (1b6c <handler_ext_int_timer32+0x14>)
    1b5c:	601a      	str	r2, [r3, #0]
    *TIMER32_STAT = 0x0;
    1b5e:	2200      	movs	r2, #0
    1b60:	4b03      	ldr	r3, [pc, #12]	; (1b70 <handler_ext_int_timer32+0x18>)
    1b62:	601a      	str	r2, [r3, #0]
    
    wfi_timeout_flag = 1;
    1b64:	4b03      	ldr	r3, [pc, #12]	; (1b74 <handler_ext_int_timer32+0x1c>)
    1b66:	3201      	adds	r2, #1
    1b68:	801a      	strh	r2, [r3, #0]
}
    1b6a:	4770      	bx	lr
    1b6c:	e000e280 	.word	0xe000e280
    1b70:	a0001110 	.word	0xa0001110
    1b74:	00003ae2 	.word	0x00003ae2

Disassembly of section .text.handler_ext_int_xot:

00001b78 <handler_ext_int_xot>:

void handler_ext_int_xot( void ) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_XOT);
    1b78:	2280      	movs	r2, #128	; 0x80
    1b7a:	4b02      	ldr	r3, [pc, #8]	; (1b84 <handler_ext_int_xot+0xc>)
    1b7c:	0352      	lsls	r2, r2, #13
    1b7e:	601a      	str	r2, [r3, #0]
}
    1b80:	4770      	bx	lr
    1b82:	46c0      	nop			; (mov r8, r8)
    1b84:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_reg0:

00001b88 <handler_ext_int_reg0>:

void handler_ext_int_reg0( void ) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);
    1b88:	2280      	movs	r2, #128	; 0x80
    1b8a:	4b02      	ldr	r3, [pc, #8]	; (1b94 <handler_ext_int_reg0+0xc>)
    1b8c:	0052      	lsls	r2, r2, #1
    1b8e:	601a      	str	r2, [r3, #0]
}
    1b90:	4770      	bx	lr
    1b92:	46c0      	nop			; (mov r8, r8)
    1b94:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_reg1:

00001b98 <handler_ext_int_reg1>:

void handler_ext_int_reg1( void ) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1);
    1b98:	2280      	movs	r2, #128	; 0x80
    1b9a:	4b02      	ldr	r3, [pc, #8]	; (1ba4 <handler_ext_int_reg1+0xc>)
    1b9c:	0092      	lsls	r2, r2, #2
    1b9e:	601a      	str	r2, [r3, #0]
}
    1ba0:	4770      	bx	lr
    1ba2:	46c0      	nop			; (mov r8, r8)
    1ba4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_reg2:

00001ba8 <handler_ext_int_reg2>:

void handler_ext_int_reg2( void ) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2);
    1ba8:	2280      	movs	r2, #128	; 0x80
    1baa:	4b02      	ldr	r3, [pc, #8]	; (1bb4 <handler_ext_int_reg2+0xc>)
    1bac:	00d2      	lsls	r2, r2, #3
    1bae:	601a      	str	r2, [r3, #0]
}
    1bb0:	4770      	bx	lr
    1bb2:	46c0      	nop			; (mov r8, r8)
    1bb4:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_reg3:

00001bb8 <handler_ext_int_reg3>:

void handler_ext_int_reg3( void ) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3);
    1bb8:	2280      	movs	r2, #128	; 0x80
    1bba:	4b02      	ldr	r3, [pc, #8]	; (1bc4 <handler_ext_int_reg3+0xc>)
    1bbc:	0112      	lsls	r2, r2, #4
    1bbe:	601a      	str	r2, [r3, #0]
}
    1bc0:	4770      	bx	lr
    1bc2:	46c0      	nop			; (mov r8, r8)
    1bc4:	e000e280 	.word	0xe000e280

Disassembly of section .text.radio_unit:

00001bc8 <radio_unit>:
/**********************************************
 * MAIN function starts here
 **********************************************/

// requires unit_count < max_unit_count
void radio_unit(uint16_t unit_count) {
    1bc8:	b5f0      	push	{r4, r5, r6, r7, lr}
    1bca:	0005      	movs	r5, r0
    1bcc:	b089      	sub	sp, #36	; 0x24
    uint16_t addr = (unit_count * CODE_CACHE_LEN) << 2;    // CODE_CACHE_LEN is the size of a unit
    pmu_setting_temp_based(1);
    1bce:	2001      	movs	r0, #1
    1bd0:	f7ff fca0 	bl	1514 <pmu_setting_temp_based>
    // read out data and check if temp or light
    set_halt_until_mbus_trx();
    1bd4:	f7fe facc 	bl	170 <set_halt_until_mbus_trx>
    uint16_t addr = (unit_count * CODE_CACHE_LEN) << 2;    // CODE_CACHE_LEN is the size of a unit
    1bd8:	2109      	movs	r1, #9
    mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) addr, PRE_ADDR, code_cache, CODE_CACHE_LEN - 1);
    1bda:	2308      	movs	r3, #8
    uint16_t addr = (unit_count * CODE_CACHE_LEN) << 2;    // CODE_CACHE_LEN is the size of a unit
    1bdc:	4369      	muls	r1, r5
            temp_arr[0] |= (1 << 15) | (CHIP_ID << 10) | (light_packet_num << 4);
            light_packet_num = (light_packet_num + 1) & 0x3F;
        }
        else {
            temp_arr[0] |= (CHIP_ID << 10) | (temp_packet_num << 4);
            temp_packet_num = (temp_packet_num + 1) & 0x3F;
    1bde:	263f      	movs	r6, #63	; 0x3f
    bool is_light = ((code_cache[0] & 0x80000000) != 0);
    1be0:	2504      	movs	r5, #4
    mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) addr, PRE_ADDR, code_cache, CODE_CACHE_LEN - 1);
    1be2:	4c29      	ldr	r4, [pc, #164]	; (1c88 <radio_unit+0xc0>)
    uint16_t addr = (unit_count * CODE_CACHE_LEN) << 2;    // CODE_CACHE_LEN is the size of a unit
    1be4:	0089      	lsls	r1, r1, #2
    mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) addr, PRE_ADDR, code_cache, CODE_CACHE_LEN - 1);
    1be6:	2201      	movs	r2, #1
    1be8:	2006      	movs	r0, #6
    1bea:	b289      	uxth	r1, r1
    1bec:	9300      	str	r3, [sp, #0]
    1bee:	0023      	movs	r3, r4
    1bf0:	f7fe fb2a 	bl	248 <mbus_copy_mem_from_remote_to_any_bulk>
    set_halt_until_mbus_tx();
    1bf4:	f7fe fab6 	bl	164 <set_halt_until_mbus_tx>
    bool is_light = ((code_cache[0] & 0x80000000) != 0);
    1bf8:	6827      	ldr	r7, [r4, #0]
        temp_arr[1] = code_cache[1];
    1bfa:	6863      	ldr	r3, [r4, #4]
    1bfc:	2203      	movs	r2, #3
    1bfe:	9306      	str	r3, [sp, #24]
        temp_arr[2] = code_cache[2];
    1c00:	68a3      	ldr	r3, [r4, #8]
    1c02:	2100      	movs	r1, #0
    1c04:	9307      	str	r3, [sp, #28]
        temp_arr[0] &= 0x0000FFFF;
    1c06:	6823      	ldr	r3, [r4, #0]
    1c08:	a805      	add	r0, sp, #20
    1c0a:	b29b      	uxth	r3, r3
    1c0c:	9305      	str	r3, [sp, #20]
    if(shift_len == 0) {
    1c0e:	230c      	movs	r3, #12
    1c10:	f7fe fd6c 	bl	6ec <right_shift_arr.part.0>
        if(is_light) {
    1c14:	4b1d      	ldr	r3, [pc, #116]	; (1c8c <radio_unit+0xc4>)
    1c16:	2f00      	cmp	r7, #0
    1c18:	da28      	bge.n	1c6c <radio_unit+0xa4>
            temp_arr[0] |= (1 << 15) | (CHIP_ID << 10) | (light_packet_num << 4);
    1c1a:	491d      	ldr	r1, [pc, #116]	; (1c90 <radio_unit+0xc8>)
    1c1c:	881a      	ldrh	r2, [r3, #0]
    1c1e:	880b      	ldrh	r3, [r1, #0]
    1c20:	0292      	lsls	r2, r2, #10
    1c22:	0118      	lsls	r0, r3, #4
    1c24:	4310      	orrs	r0, r2
    1c26:	2280      	movs	r2, #128	; 0x80
    1c28:	0212      	lsls	r2, r2, #8
    1c2a:	4302      	orrs	r2, r0
    1c2c:	9805      	ldr	r0, [sp, #20]
            light_packet_num = (light_packet_num + 1) & 0x3F;
    1c2e:	3301      	adds	r3, #1
            temp_arr[0] |= (1 << 15) | (CHIP_ID << 10) | (light_packet_num << 4);
    1c30:	4302      	orrs	r2, r0
            light_packet_num = (light_packet_num + 1) & 0x3F;
    1c32:	4033      	ands	r3, r6
            temp_arr[0] |= (1 << 15) | (CHIP_ID << 10) | (light_packet_num << 4);
    1c34:	9205      	str	r2, [sp, #20]
            light_packet_num = (light_packet_num + 1) & 0x3F;
    1c36:	800b      	strh	r3, [r1, #0]
        }

        // assign to radio arr
        radio_data_arr[2] = temp_arr[0];
    1c38:	9b05      	ldr	r3, [sp, #20]
        radio_data_arr[1] = temp_arr[1];
        radio_data_arr[0] = temp_arr[2];

        mrr_send_radio_data(0);
    1c3a:	2000      	movs	r0, #0
        radio_data_arr[2] = temp_arr[0];
    1c3c:	9303      	str	r3, [sp, #12]
    1c3e:	9a03      	ldr	r2, [sp, #12]
    1c40:	4b14      	ldr	r3, [pc, #80]	; (1c94 <radio_unit+0xcc>)
    1c42:	3d01      	subs	r5, #1
    1c44:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = temp_arr[1];
    1c46:	9a06      	ldr	r2, [sp, #24]
    1c48:	b2ad      	uxth	r5, r5
    1c4a:	605a      	str	r2, [r3, #4]
    1c4c:	9203      	str	r2, [sp, #12]
        radio_data_arr[0] = temp_arr[2];
    1c4e:	9a07      	ldr	r2, [sp, #28]
    1c50:	601a      	str	r2, [r3, #0]
        mrr_send_radio_data(0);
    1c52:	f7ff fdb5 	bl	17c0 <mrr_send_radio_data>
    if(shift_len == 0) {
    1c56:	2344      	movs	r3, #68	; 0x44
    1c58:	2209      	movs	r2, #9
    1c5a:	2100      	movs	r1, #0
    1c5c:	0020      	movs	r0, r4
    1c5e:	425b      	negs	r3, r3
    1c60:	f7fe fd44 	bl	6ec <right_shift_arr.part.0>
    for(i = 0; i < 4; i++) {
    1c64:	2d00      	cmp	r5, #0
    1c66:	d1c8      	bne.n	1bfa <radio_unit+0x32>

        // left shift a new packet over
        // 68 bits for both light and temp
        right_shift_arr(code_cache, 0, CODE_CACHE_LEN, -LIGHT_CONTENT_SIZE);
    }
}
    1c68:	b009      	add	sp, #36	; 0x24
    1c6a:	bdf0      	pop	{r4, r5, r6, r7, pc}
            temp_arr[0] |= (CHIP_ID << 10) | (temp_packet_num << 4);
    1c6c:	490a      	ldr	r1, [pc, #40]	; (1c98 <radio_unit+0xd0>)
    1c6e:	881b      	ldrh	r3, [r3, #0]
    1c70:	880a      	ldrh	r2, [r1, #0]
    1c72:	029b      	lsls	r3, r3, #10
    1c74:	0110      	lsls	r0, r2, #4
    1c76:	4303      	orrs	r3, r0
    1c78:	9805      	ldr	r0, [sp, #20]
            temp_packet_num = (temp_packet_num + 1) & 0x3F;
    1c7a:	3201      	adds	r2, #1
            temp_arr[0] |= (CHIP_ID << 10) | (temp_packet_num << 4);
    1c7c:	4303      	orrs	r3, r0
            temp_packet_num = (temp_packet_num + 1) & 0x3F;
    1c7e:	4032      	ands	r2, r6
            temp_arr[0] |= (CHIP_ID << 10) | (temp_packet_num << 4);
    1c80:	9305      	str	r3, [sp, #20]
            temp_packet_num = (temp_packet_num + 1) & 0x3F;
    1c82:	800a      	strh	r2, [r1, #0]
    1c84:	e7d8      	b.n	1c38 <radio_unit+0x70>
    1c86:	46c0      	nop			; (mov r8, r8)
    1c88:	000039c4 	.word	0x000039c4
    1c8c:	00003a00 	.word	0x00003a00
    1c90:	00003a3c 	.word	0x00003a3c
    1c94:	000039b8 	.word	0x000039b8
    1c98:	00003ae0 	.word	0x00003ae0

Disassembly of section .text.set_send_enable:

00001c9c <set_send_enable>:
        radio_unit(start_unit_count + i);
    }
}

uint16_t set_send_enable() {
    if(OVERRIDE_RAD) {
    1c9c:	4b14      	ldr	r3, [pc, #80]	; (1cf0 <set_send_enable+0x54>)
uint16_t set_send_enable() {
    1c9e:	b510      	push	{r4, lr}
    if(OVERRIDE_RAD) {
    1ca0:	881b      	ldrh	r3, [r3, #0]
    1ca2:	b298      	uxth	r0, r3
    1ca4:	2b00      	cmp	r3, #0
    1ca6:	d120      	bne.n	1cea <set_send_enable+0x4e>
        return 1;
    }
    if(snt_sys_temp_code < MRR_TEMP_THRESH_LOW || snt_sys_temp_code > MRR_TEMP_THRESH_HIGH) {
    1ca8:	4b12      	ldr	r3, [pc, #72]	; (1cf4 <set_send_enable+0x58>)
    1caa:	4a13      	ldr	r2, [pc, #76]	; (1cf8 <set_send_enable+0x5c>)
    1cac:	8819      	ldrh	r1, [r3, #0]
    1cae:	6813      	ldr	r3, [r2, #0]
    1cb0:	4299      	cmp	r1, r3
    1cb2:	d816      	bhi.n	1ce2 <set_send_enable+0x46>
    1cb4:	4b11      	ldr	r3, [pc, #68]	; (1cfc <set_send_enable+0x60>)
    1cb6:	8819      	ldrh	r1, [r3, #0]
    1cb8:	6813      	ldr	r3, [r2, #0]
    1cba:	4299      	cmp	r1, r3
    1cbc:	d311      	bcc.n	1ce2 <set_send_enable+0x46>
    1cbe:	2301      	movs	r3, #1
        return 0;
    }
    uint16_t i;
    for(i = 1; i < 6; i++) {
        if(snt_sys_temp_code < PMU_TEMP_THRESH[i]) {
    1cc0:	490f      	ldr	r1, [pc, #60]	; (1d00 <set_send_enable+0x64>)
    1cc2:	0098      	lsls	r0, r3, #2
    1cc4:	5844      	ldr	r4, [r0, r1]
    1cc6:	6810      	ldr	r0, [r2, #0]
    1cc8:	4284      	cmp	r4, r0
    1cca:	d90b      	bls.n	1ce4 <set_send_enable+0x48>
            if(read_data_batadc <= PMU_ADC_THRESH[i - 1]) {
    1ccc:	2000      	movs	r0, #0
    1cce:	4a0d      	ldr	r2, [pc, #52]	; (1d04 <set_send_enable+0x68>)
    1cd0:	490d      	ldr	r1, [pc, #52]	; (1d08 <set_send_enable+0x6c>)
    1cd2:	3b01      	subs	r3, #1
    1cd4:	8812      	ldrh	r2, [r2, #0]
    1cd6:	009b      	lsls	r3, r3, #2
    1cd8:	585b      	ldr	r3, [r3, r1]
    1cda:	b292      	uxth	r2, r2
    1cdc:	4293      	cmp	r3, r2
    1cde:	4140      	adcs	r0, r0
        return 1;
    1ce0:	b280      	uxth	r0, r0
                return 0;
            }
        }   
    }
    return 1;
}
    1ce2:	bd10      	pop	{r4, pc}
    for(i = 1; i < 6; i++) {
    1ce4:	3301      	adds	r3, #1
    1ce6:	2b06      	cmp	r3, #6
    1ce8:	d1eb      	bne.n	1cc2 <set_send_enable+0x26>
        return 1;
    1cea:	2001      	movs	r0, #1
    1cec:	e7f9      	b.n	1ce2 <set_send_enable+0x46>
    1cee:	46c0      	nop			; (mov r8, r8)
    1cf0:	00003a02 	.word	0x00003a02
    1cf4:	000037ee 	.word	0x000037ee
    1cf8:	00003938 	.word	0x00003938
    1cfc:	000037ec 	.word	0x000037ec
    1d00:	000038ac 	.word	0x000038ac
    1d04:	000039b4 	.word	0x000039b4
    1d08:	00003828 	.word	0x00003828

Disassembly of section .text.set_low_pwr_low_trigger:

00001d0c <set_low_pwr_low_trigger>:

// return true if PMU ADC output is higher than LOW_THRESH
// that means system should go into LOW_PWR
uint16_t set_low_pwr_low_trigger() {
    1d0c:	2300      	movs	r3, #0
    1d0e:	b510      	push	{r4, lr}
    uint16_t i;
    for(i = 0; i < 6; i++) {
        if(snt_sys_temp_code < PMU_TEMP_THRESH[i]) {
    1d10:	4a0a      	ldr	r2, [pc, #40]	; (1d3c <set_low_pwr_low_trigger+0x30>)
    1d12:	490b      	ldr	r1, [pc, #44]	; (1d40 <set_low_pwr_low_trigger+0x34>)
    1d14:	0098      	lsls	r0, r3, #2
    1d16:	5884      	ldr	r4, [r0, r2]
    1d18:	6808      	ldr	r0, [r1, #0]
    1d1a:	4284      	cmp	r4, r0
    1d1c:	d908      	bls.n	1d30 <set_low_pwr_low_trigger+0x24>
            if(read_data_batadc > LOW_PWR_LOW_ADC_THRESH[i]) {
    1d1e:	4a09      	ldr	r2, [pc, #36]	; (1d44 <set_low_pwr_low_trigger+0x38>)
    1d20:	005b      	lsls	r3, r3, #1
    1d22:	5a98      	ldrh	r0, [r3, r2]
            }
            return 0;
        }
    }
    // > 60C
    if(read_data_batadc > LOW_PWR_LOW_ADC_THRESH[6]) {
    1d24:	4b08      	ldr	r3, [pc, #32]	; (1d48 <set_low_pwr_low_trigger+0x3c>)
    1d26:	881b      	ldrh	r3, [r3, #0]
                return 1;
    1d28:	4298      	cmp	r0, r3
    1d2a:	4180      	sbcs	r0, r0
    1d2c:	4240      	negs	r0, r0
        return 1;
    }
    return 0;
}
    1d2e:	bd10      	pop	{r4, pc}
    for(i = 0; i < 6; i++) {
    1d30:	3301      	adds	r3, #1
    1d32:	2b06      	cmp	r3, #6
    1d34:	d1ee      	bne.n	1d14 <set_low_pwr_low_trigger+0x8>
    if(read_data_batadc > LOW_PWR_LOW_ADC_THRESH[6]) {
    1d36:	4b03      	ldr	r3, [pc, #12]	; (1d44 <set_low_pwr_low_trigger+0x38>)
    1d38:	8998      	ldrh	r0, [r3, #12]
    1d3a:	e7f3      	b.n	1d24 <set_low_pwr_low_trigger+0x18>
    1d3c:	000038ac 	.word	0x000038ac
    1d40:	00003938 	.word	0x00003938
    1d44:	000037dc 	.word	0x000037dc
    1d48:	000039b4 	.word	0x000039b4

Disassembly of section .text.set_low_pwr_high_trigger:

00001d4c <set_low_pwr_high_trigger>:

// return true if PMU ADC output is higher than HIGH_THRESH
// return false if PMU ADC output is lower than HIGH_THRESH
// that means system should get out of LOW_PWR
uint16_t set_low_pwr_high_trigger() {
    1d4c:	2300      	movs	r3, #0
    1d4e:	b510      	push	{r4, lr}
    uint16_t i;
    for(i = 0; i < 6; i++) {
        if(snt_sys_temp_code < PMU_TEMP_THRESH[i]) {
    1d50:	4a0a      	ldr	r2, [pc, #40]	; (1d7c <set_low_pwr_high_trigger+0x30>)
    1d52:	490b      	ldr	r1, [pc, #44]	; (1d80 <set_low_pwr_high_trigger+0x34>)
    1d54:	0098      	lsls	r0, r3, #2
    1d56:	5884      	ldr	r4, [r0, r2]
    1d58:	6808      	ldr	r0, [r1, #0]
    1d5a:	4284      	cmp	r4, r0
    1d5c:	d908      	bls.n	1d70 <set_low_pwr_high_trigger+0x24>
            if(read_data_batadc > LOW_PWR_HIGH_ADC_THRESH[i]) {
    1d5e:	4a09      	ldr	r2, [pc, #36]	; (1d84 <set_low_pwr_high_trigger+0x38>)
    1d60:	005b      	lsls	r3, r3, #1
    1d62:	5a98      	ldrh	r0, [r3, r2]
            }
            return 0;
        }
    }
    // > 60C
    if(read_data_batadc > LOW_PWR_HIGH_ADC_THRESH[6]) {
    1d64:	4b08      	ldr	r3, [pc, #32]	; (1d88 <set_low_pwr_high_trigger+0x3c>)
    1d66:	881b      	ldrh	r3, [r3, #0]
                return 1;
    1d68:	4298      	cmp	r0, r3
    1d6a:	4180      	sbcs	r0, r0
    1d6c:	4240      	negs	r0, r0
        return 1;
    }
    return 0;
}
    1d6e:	bd10      	pop	{r4, pc}
    for(i = 0; i < 6; i++) {
    1d70:	3301      	adds	r3, #1
    1d72:	2b06      	cmp	r3, #6
    1d74:	d1ee      	bne.n	1d54 <set_low_pwr_high_trigger+0x8>
    if(read_data_batadc > LOW_PWR_HIGH_ADC_THRESH[6]) {
    1d76:	4b03      	ldr	r3, [pc, #12]	; (1d84 <set_low_pwr_high_trigger+0x38>)
    1d78:	8998      	ldrh	r0, [r3, #12]
    1d7a:	e7f3      	b.n	1d64 <set_low_pwr_high_trigger+0x18>
    1d7c:	000038ac 	.word	0x000038ac
    1d80:	00003938 	.word	0x00003938
    1d84:	000037ce 	.word	0x000037ce
    1d88:	000039b4 	.word	0x000039b4

Disassembly of section .text.send_beacon:

00001d8c <send_beacon>:

void send_beacon() {
    // v6.0.1 checking temp and voltage
    if(mrr_send_enable) {
    1d8c:	4b05      	ldr	r3, [pc, #20]	; (1da4 <send_beacon+0x18>)
void send_beacon() {
    1d8e:	b510      	push	{r4, lr}
    if(mrr_send_enable) {
    1d90:	881b      	ldrh	r3, [r3, #0]
    1d92:	2b00      	cmp	r3, #0
    1d94:	d005      	beq.n	1da2 <send_beacon+0x16>
        pmu_setting_temp_based(1);
    1d96:	2001      	movs	r0, #1
    1d98:	f7ff fbbc 	bl	1514 <pmu_setting_temp_based>
        // mrr_send_radio_data(1);
        mrr_send_radio_data(0); // FIXME: change this line in real code
    1d9c:	2000      	movs	r0, #0
    1d9e:	f7ff fd0f 	bl	17c0 <mrr_send_radio_data>
        // pmu_setting_temp_based(0);   // FIXME: set this line back
    }
}
    1da2:	bd10      	pop	{r4, pc}
    1da4:	00003908 	.word	0x00003908

Disassembly of section .text.initialize_state_collect:

00001da8 <initialize_state_collect>:

// reduce repeated code
void initialize_state_collect() {
    // initialize day_state
    // starting from an edge 
    if(xo_day_time_in_sec < cur_sunrise - EDGE_MARGIN2 - XO_10_MIN) {
    1da8:	4b23      	ldr	r3, [pc, #140]	; (1e38 <initialize_state_collect+0x90>)
void initialize_state_collect() {
    1daa:	b570      	push	{r4, r5, r6, lr}
    if(xo_day_time_in_sec < cur_sunrise - EDGE_MARGIN2 - XO_10_MIN) {
    1dac:	4a23      	ldr	r2, [pc, #140]	; (1e3c <initialize_state_collect+0x94>)
    1dae:	4c24      	ldr	r4, [pc, #144]	; (1e40 <initialize_state_collect+0x98>)
    1db0:	681b      	ldr	r3, [r3, #0]
    1db2:	4824      	ldr	r0, [pc, #144]	; (1e44 <initialize_state_collect+0x9c>)
    1db4:	8812      	ldrh	r2, [r2, #0]
    1db6:	6821      	ldr	r1, [r4, #0]
    1db8:	181b      	adds	r3, r3, r0
    1dba:	1a9b      	subs	r3, r3, r2
    1dbc:	4a22      	ldr	r2, [pc, #136]	; (1e48 <initialize_state_collect+0xa0>)
    1dbe:	428b      	cmp	r3, r1
    1dc0:	d901      	bls.n	1dc6 <initialize_state_collect+0x1e>
    else if(xo_day_time_in_sec < cur_sunset - EDGE_MARGIN1 - XO_10_MIN){
        day_state = DUSK;
    }
    else {
        // trusting the wrap around from set_new_state()
        day_state = DAWN;
    1dc2:	2300      	movs	r3, #0
    1dc4:	e00a      	b.n	1ddc <initialize_state_collect+0x34>
    else if(xo_day_time_in_sec < cur_sunset - EDGE_MARGIN1 - XO_10_MIN){
    1dc6:	4b21      	ldr	r3, [pc, #132]	; (1e4c <initialize_state_collect+0xa4>)
    1dc8:	4921      	ldr	r1, [pc, #132]	; (1e50 <initialize_state_collect+0xa8>)
    1dca:	681b      	ldr	r3, [r3, #0]
    1dcc:	4d1d      	ldr	r5, [pc, #116]	; (1e44 <initialize_state_collect+0x9c>)
    1dce:	8808      	ldrh	r0, [r1, #0]
    1dd0:	6821      	ldr	r1, [r4, #0]
    1dd2:	195b      	adds	r3, r3, r5
    1dd4:	1a1b      	subs	r3, r3, r0
    1dd6:	428b      	cmp	r3, r1
    1dd8:	d9f3      	bls.n	1dc2 <initialize_state_collect+0x1a>
        day_state = DUSK;
    1dda:	2302      	movs	r3, #2
    }

    // set new state variables based on day_state
    // setting next sunrise and sunset so the system won't be confused
    next_sunrise = 0;
    1ddc:	2500      	movs	r5, #0
        day_state = DAWN;
    1dde:	8013      	strh	r3, [r2, #0]
    next_sunrise = 0;
    1de0:	4b1c      	ldr	r3, [pc, #112]	; (1e54 <initialize_state_collect+0xac>)
    1de2:	601d      	str	r5, [r3, #0]
    next_sunset = 0;
    1de4:	4b1c      	ldr	r3, [pc, #112]	; (1e58 <initialize_state_collect+0xb0>)
    1de6:	601d      	str	r5, [r3, #0]
    set_new_state();
    1de8:	f7fe ff58 	bl	c9c <set_new_state>

    // find suitable temp storage time based on day_state_start_time
    uint32_t store_temp_day_time = 0;

    // this variable should always reflect the last temp data stored
    store_temp_index = 0; 
    1dec:	4a1b      	ldr	r2, [pc, #108]	; (1e5c <initialize_state_collect+0xb4>)
    while(store_temp_day_time < day_state_start_time) {
    1dee:	481c      	ldr	r0, [pc, #112]	; (1e60 <initialize_state_collect+0xb8>)
    store_temp_index = 0; 
    1df0:	8015      	strh	r5, [r2, #0]
    while(store_temp_day_time < day_state_start_time) {
    1df2:	6801      	ldr	r1, [r0, #0]
    1df4:	42a9      	cmp	r1, r5
    1df6:	d817      	bhi.n	1e28 <initialize_state_collect+0x80>
        store_temp_day_time += XO_30_MIN;
        store_temp_index++;
    }
    // shift to index of last data stored
    store_temp_index--;
    1df8:	8811      	ldrh	r1, [r2, #0]
    1dfa:	3901      	subs	r1, #1
    1dfc:	b289      	uxth	r1, r1
    1dfe:	8011      	strh	r1, [r2, #0]
    store_temp_timestamp = store_temp_day_time + xo_sys_time_in_sec - xo_day_time_in_sec;
    1e00:	4a18      	ldr	r2, [pc, #96]	; (1e64 <initialize_state_collect+0xbc>)
    1e02:	6812      	ldr	r2, [r2, #0]
    1e04:	6821      	ldr	r1, [r4, #0]
    1e06:	1a52      	subs	r2, r2, r1
    1e08:	4917      	ldr	r1, [pc, #92]	; (1e68 <initialize_state_collect+0xc0>)
    1e0a:	1952      	adds	r2, r2, r5
    1e0c:	600a      	str	r2, [r1, #0]
    // deal with wrap around issue
    if(store_temp_day_time < xo_day_time_in_sec) {
    1e0e:	6822      	ldr	r2, [r4, #0]
    1e10:	42aa      	cmp	r2, r5
    1e12:	d903      	bls.n	1e1c <initialize_state_collect+0x74>
        store_temp_timestamp += MAX_DAY_TIME;
    1e14:	680b      	ldr	r3, [r1, #0]
    1e16:	4a15      	ldr	r2, [pc, #84]	; (1e6c <initialize_state_collect+0xc4>)
    1e18:	189b      	adds	r3, r3, r2
    1e1a:	600b      	str	r3, [r1, #0]
    }
    goc_state = STATE_COLLECT;
    1e1c:	2201      	movs	r2, #1
    1e1e:	4b14      	ldr	r3, [pc, #80]	; (1e70 <initialize_state_collect+0xc8>)
    1e20:	801a      	strh	r2, [r3, #0]
    set_projected_end_time();
    1e22:	f7ff fac7 	bl	13b4 <set_projected_end_time>
}
    1e26:	bd70      	pop	{r4, r5, r6, pc}
        store_temp_day_time += XO_30_MIN;
    1e28:	23e1      	movs	r3, #225	; 0xe1
        store_temp_index++;
    1e2a:	8811      	ldrh	r1, [r2, #0]
        store_temp_day_time += XO_30_MIN;
    1e2c:	00db      	lsls	r3, r3, #3
        store_temp_index++;
    1e2e:	3101      	adds	r1, #1
    1e30:	b289      	uxth	r1, r1
        store_temp_day_time += XO_30_MIN;
    1e32:	18ed      	adds	r5, r5, r3
        store_temp_index++;
    1e34:	8011      	strh	r1, [r2, #0]
    1e36:	e7dc      	b.n	1df2 <initialize_state_collect+0x4a>
    1e38:	00003a10 	.word	0x00003a10
    1e3c:	000037c6 	.word	0x000037c6
    1e40:	00003ae4 	.word	0x00003ae4
    1e44:	fffffda8 	.word	0xfffffda8
    1e48:	000038f8 	.word	0x000038f8
    1e4c:	00003a14 	.word	0x00003a14
    1e50:	000037c4 	.word	0x000037c4
    1e54:	00003a78 	.word	0x00003a78
    1e58:	00003a7c 	.word	0x00003a7c
    1e5c:	00003ad4 	.word	0x00003ad4
    1e60:	0000395c 	.word	0x0000395c
    1e64:	00003aec 	.word	0x00003aec
    1e68:	00003ad8 	.word	0x00003ad8
    1e6c:	00015180 	.word	0x00015180
    1e70:	000039ec 	.word	0x000039ec

Disassembly of section .text.startup.main:

00001e74 <main>:

int main() {
    1e74:	b5f0      	push	{r4, r5, r6, r7, lr}
    // Only enable relevant interrupts (PREv18)
    *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
    1e76:	4bf8      	ldr	r3, [pc, #992]	; (2258 <main+0x3e4>)
    1e78:	4af8      	ldr	r2, [pc, #992]	; (225c <main+0x3e8>)
          1 << IRQ_REG0 | 1 << IRQ_REG1 | 1 << IRQ_REG2 | 1 << IRQ_REG3);

    if(enumerated != ENUMID + VERSION_NUM) {
    1e7a:	4cf9      	ldr	r4, [pc, #996]	; (2260 <main+0x3ec>)
    *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
    1e7c:	601a      	str	r2, [r3, #0]
    if(enumerated != ENUMID + VERSION_NUM) {
    1e7e:	6823      	ldr	r3, [r4, #0]
    1e80:	4df8      	ldr	r5, [pc, #992]	; (2264 <main+0x3f0>)
int main() {
    1e82:	b085      	sub	sp, #20
    if(enumerated != ENUMID + VERSION_NUM) {
    1e84:	42ab      	cmp	r3, r5
    1e86:	d100      	bne.n	1e8a <main+0x16>
    1e88:	e2a7      	b.n	23da <main+0x566>
    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    1e8a:	2700      	movs	r7, #0
    1e8c:	4bf6      	ldr	r3, [pc, #984]	; (2268 <main+0x3f4>)
    config_timer32(0, 0, 0, 0);
    1e8e:	0039      	movs	r1, r7
    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    1e90:	601f      	str	r7, [r3, #0]
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    1e92:	4bf6      	ldr	r3, [pc, #984]	; (226c <main+0x3f8>)
    config_timer32(0, 0, 0, 0);
    1e94:	0038      	movs	r0, r7
    1e96:	003a      	movs	r2, r7
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    1e98:	601f      	str	r7, [r3, #0]
    config_timer32(0, 0, 0, 0);
    1e9a:	003b      	movs	r3, r7
    1e9c:	f7fe f90e 	bl	bc <config_timer32>
    xo_sys_time = 0;
    1ea0:	4bf3      	ldr	r3, [pc, #972]	; (2270 <main+0x3fc>)
    enumerated = ENUMID + VERSION_NUM;
    1ea2:	6025      	str	r5, [r4, #0]
    xo_sys_time = 0;
    1ea4:	601f      	str	r7, [r3, #0]
    xo_sys_time_in_sec = 0;
    1ea6:	4bf3      	ldr	r3, [pc, #972]	; (2274 <main+0x400>)
    xo_to_sec_mplier = 32768;
    1ea8:	4af3      	ldr	r2, [pc, #972]	; (2278 <main+0x404>)
    xo_sys_time_in_sec = 0;
    1eaa:	601f      	str	r7, [r3, #0]
    xo_day_time_in_sec = 0;
    1eac:	4bf3      	ldr	r3, [pc, #972]	; (227c <main+0x408>)
    sntv5_r01.TSNS_BURST_MODE = 0;
    1eae:	2640      	movs	r6, #64	; 0x40
    xo_day_time_in_sec = 0;
    1eb0:	601f      	str	r7, [r3, #0]
    xo_to_sec_mplier = 32768;
    1eb2:	4bf3      	ldr	r3, [pc, #972]	; (2280 <main+0x40c>)
    lntv1a_r04_t lntv1a_r04 = LNTv1A_R04_DEFAULT;
    1eb4:	240a      	movs	r4, #10
    xo_to_sec_mplier = 32768;
    1eb6:	801a      	strh	r2, [r3, #0]
    XO_TO_SEC_MPLIER_SHIFT = 15;
    1eb8:	220f      	movs	r2, #15
    1eba:	4bf2      	ldr	r3, [pc, #968]	; (2284 <main+0x410>)
    1ebc:	801a      	strh	r2, [r3, #0]
    xo_lnt_mplier = 0x73;
    1ebe:	4bf2      	ldr	r3, [pc, #968]	; (2288 <main+0x414>)
    1ec0:	3264      	adds	r2, #100	; 0x64
    1ec2:	801a      	strh	r2, [r3, #0]
    LNT_MPLIER_SHIFT = 6;
    1ec4:	4bf1      	ldr	r3, [pc, #964]	; (228c <main+0x418>)
    1ec6:	3a6d      	subs	r2, #109	; 0x6d
    1ec8:	801a      	strh	r2, [r3, #0]
    var_init();
    1eca:	f7fe f9d1 	bl	270 <var_init>
    mbus_enumerate(MEM_ADDR);
    1ece:	2006      	movs	r0, #6
    1ed0:	f7fe f972 	bl	1b8 <mbus_enumerate>
    delay(MBUS_DELAY);
    1ed4:	2064      	movs	r0, #100	; 0x64
    1ed6:	f7fe f8e7 	bl	a8 <delay>
    mbus_enumerate(MRR_ADDR);
    1eda:	2002      	movs	r0, #2
    1edc:	f7fe f96c 	bl	1b8 <mbus_enumerate>
    delay(MBUS_DELAY);
    1ee0:	2064      	movs	r0, #100	; 0x64
    1ee2:	f7fe f8e1 	bl	a8 <delay>
    mbus_enumerate(LNT_ADDR);
    1ee6:	2003      	movs	r0, #3
    1ee8:	f7fe f966 	bl	1b8 <mbus_enumerate>
    delay(MBUS_DELAY);
    1eec:	2064      	movs	r0, #100	; 0x64
    1eee:	f7fe f8db 	bl	a8 <delay>
    mbus_enumerate(SNT_ADDR);
    1ef2:	2004      	movs	r0, #4
    1ef4:	f7fe f960 	bl	1b8 <mbus_enumerate>
    delay(MBUS_DELAY);
    1ef8:	2064      	movs	r0, #100	; 0x64
    1efa:	f7fe f8d5 	bl	a8 <delay>
    mbus_enumerate(PMU_ADDR);
    1efe:	2005      	movs	r0, #5
    1f00:	f7fe f95a 	bl	1b8 <mbus_enumerate>
    delay(MBUS_DELAY);
    1f04:	2064      	movs	r0, #100	; 0x64
    1f06:	f7fe f8cf 	bl	a8 <delay>
    set_halt_until_mbus_tx();
    1f0a:	f7fe f92b 	bl	164 <set_halt_until_mbus_tx>
    sntv5_r01.TSNS_CONT_MODE  = 0;
    1f0e:	2180      	movs	r1, #128	; 0x80
    sntv5_r01.TSNS_BURST_MODE = 0;
    1f10:	4bdf      	ldr	r3, [pc, #892]	; (2290 <main+0x41c>)
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    1f12:	2004      	movs	r0, #4
    sntv5_r01.TSNS_BURST_MODE = 0;
    1f14:	881a      	ldrh	r2, [r3, #0]
    1f16:	43b2      	bics	r2, r6
    1f18:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_CONT_MODE  = 0;
    1f1a:	881a      	ldrh	r2, [r3, #0]
    1f1c:	438a      	bics	r2, r1
    1f1e:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    1f20:	681a      	ldr	r2, [r3, #0]
    1f22:	397f      	subs	r1, #127	; 0x7f
    1f24:	f7fe f969 	bl	1fa <mbus_remote_register_write>
    sntv5_r07.TSNS_INT_RPLY_SHORT_ADDR = 0x10;
    1f28:	21ff      	movs	r1, #255	; 0xff
    1f2a:	2080      	movs	r0, #128	; 0x80
    1f2c:	4ad9      	ldr	r2, [pc, #868]	; (2294 <main+0x420>)
    1f2e:	0140      	lsls	r0, r0, #5
    1f30:	8813      	ldrh	r3, [r2, #0]
    1f32:	400b      	ands	r3, r1
    1f34:	4303      	orrs	r3, r0
    1f36:	8013      	strh	r3, [r2, #0]
    sntv5_r07.TSNS_INT_RPLY_REG_ADDR   = 0x00;
    1f38:	8813      	ldrh	r3, [r2, #0]
    mbus_remote_register_write(SNT_ADDR, 7, sntv5_r07.as_int);
    1f3a:	2004      	movs	r0, #4
    sntv5_r07.TSNS_INT_RPLY_REG_ADDR   = 0x00;
    1f3c:	438b      	bics	r3, r1
    1f3e:	8013      	strh	r3, [r2, #0]
    mbus_remote_register_write(SNT_ADDR, 7, sntv5_r07.as_int);
    1f40:	6812      	ldr	r2, [r2, #0]
    1f42:	39f8      	subs	r1, #248	; 0xf8
    1f44:	f7fe f959 	bl	1fa <mbus_remote_register_write>
    snt_state = SNT_IDLE;
    1f48:	4bd3      	ldr	r3, [pc, #844]	; (2298 <main+0x424>)
    1f4a:	801f      	strh	r7, [r3, #0]
    operation_temp_run();
    1f4c:	f7ff fb8a 	bl	1664 <operation_temp_run>
    lntv1a_r01_t lntv1a_r01 = LNTv1A_R01_DEFAULT;
    1f50:	4bd2      	ldr	r3, [pc, #840]	; (229c <main+0x428>)
    mbus_remote_register_write(LNT_ADDR,0x22,lntv1a_r22.as_int);
    1f52:	2122      	movs	r1, #34	; 0x22
    lntv1a_r01_t lntv1a_r01 = LNTv1A_R01_DEFAULT;
    1f54:	685a      	ldr	r2, [r3, #4]
    mbus_remote_register_write(LNT_ADDR,0x22,lntv1a_r22.as_int);
    1f56:	2003      	movs	r0, #3
    lntv1a_r01_t lntv1a_r01 = LNTv1A_R01_DEFAULT;
    1f58:	9201      	str	r2, [sp, #4]
    lntv1a_r21_t lntv1a_r21 = LNTv1A_R21_DEFAULT;
    1f5a:	689a      	ldr	r2, [r3, #8]
    lntv1a_r22_t lntv1a_r22 = LNTv1A_R22_DEFAULT;
    1f5c:	68db      	ldr	r3, [r3, #12]
    lntv1a_r21_t lntv1a_r21 = LNTv1A_R21_DEFAULT;
    1f5e:	9202      	str	r2, [sp, #8]
    lntv1a_r22_t lntv1a_r22 = LNTv1A_R22_DEFAULT;
    1f60:	9303      	str	r3, [sp, #12]
    lntv1a_r22.TMR_POLY_CON = 0x1; // Default: 0x1
    1f62:	9a03      	ldr	r2, [sp, #12]
    1f64:	4bce      	ldr	r3, [pc, #824]	; (22a0 <main+0x42c>)
    1f66:	4013      	ands	r3, r2
    1f68:	4ace      	ldr	r2, [pc, #824]	; (22a4 <main+0x430>)
    1f6a:	431a      	orrs	r2, r3
    mbus_remote_register_write(LNT_ADDR,0x22,lntv1a_r22.as_int);
    1f6c:	f7fe f945 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    1f70:	20fa      	movs	r0, #250	; 0xfa
    1f72:	0080      	lsls	r0, r0, #2
    1f74:	f7fe f898 	bl	a8 <delay>
    lntv1a_r21.TMR_EN_TUNE2 = 0x1; // Default : 1'h1
    1f78:	9b02      	ldr	r3, [sp, #8]
    1f7a:	4acb      	ldr	r2, [pc, #812]	; (22a8 <main+0x434>)
    1f7c:	0c1d      	lsrs	r5, r3, #16
    1f7e:	042d      	lsls	r5, r5, #16
    1f80:	4315      	orrs	r5, r2
    mbus_remote_register_write(LNT_ADDR,0x21,lntv1a_r21.as_int);
    1f82:	002a      	movs	r2, r5
    1f84:	2121      	movs	r1, #33	; 0x21
    1f86:	2003      	movs	r0, #3
    1f88:	f7fe f937 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    1f8c:	20fa      	movs	r0, #250	; 0xfa
    1f8e:	0080      	lsls	r0, r0, #2
    1f90:	f7fe f88a 	bl	a8 <delay>
    mbus_remote_register_write(LNT_ADDR,0x40,lntv1a_r40.as_int);
    1f94:	22c0      	movs	r2, #192	; 0xc0
    1f96:	0031      	movs	r1, r6
    1f98:	03d2      	lsls	r2, r2, #15
    1f9a:	2003      	movs	r0, #3
    1f9c:	f7fe f92d 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    1fa0:	20fa      	movs	r0, #250	; 0xfa
    lntv1a_r21.TMR_SELF_EN = 0x0; // Default : 0x1
    1fa2:	4ec2      	ldr	r6, [pc, #776]	; (22ac <main+0x438>)
    delay(MBUS_DELAY*10);
    1fa4:	0080      	lsls	r0, r0, #2
    lntv1a_r21.TMR_SELF_EN = 0x0; // Default : 0x1
    1fa6:	4035      	ands	r5, r6
    delay(MBUS_DELAY*10);
    1fa8:	f7fe f87e 	bl	a8 <delay>
    mbus_remote_register_write(LNT_ADDR,0x21,lntv1a_r21.as_int);
    1fac:	002a      	movs	r2, r5
    1fae:	2121      	movs	r1, #33	; 0x21
    1fb0:	2003      	movs	r0, #3
    1fb2:	f7fe f922 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    1fb6:	20fa      	movs	r0, #250	; 0xfa
    1fb8:	0080      	lsls	r0, r0, #2
    1fba:	f7fe f875 	bl	a8 <delay>
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);
    1fbe:	2208      	movs	r2, #8
    1fc0:	2120      	movs	r1, #32
    1fc2:	2003      	movs	r0, #3
    1fc4:	f7fe f919 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    1fc8:	20fa      	movs	r0, #250	; 0xfa
    1fca:	0080      	lsls	r0, r0, #2
    1fcc:	f7fe f86c 	bl	a8 <delay>
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);
    1fd0:	221e      	movs	r2, #30
    1fd2:	2120      	movs	r1, #32
    1fd4:	2003      	movs	r0, #3
    1fd6:	f7fe f910 	bl	1fa <mbus_remote_register_write>
    delay(2000); 
    1fda:	20fa      	movs	r0, #250	; 0xfa
    1fdc:	00c0      	lsls	r0, r0, #3
    1fde:	f7fe f863 	bl	a8 <delay>
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);
    1fe2:	221f      	movs	r2, #31
    1fe4:	2120      	movs	r1, #32
    1fe6:	2003      	movs	r0, #3
    1fe8:	f7fe f907 	bl	1fa <mbus_remote_register_write>
    delay(10); 
    1fec:	0020      	movs	r0, r4
    1fee:	f7fe f85b 	bl	a8 <delay>
    lntv1a_r21.TMR_SELF_EN = 0x1; // Default : 0x0
    1ff2:	2280      	movs	r2, #128	; 0x80
    1ff4:	0392      	lsls	r2, r2, #14
    1ff6:	432a      	orrs	r2, r5
    mbus_remote_register_write(LNT_ADDR,0x21,lntv1a_r21.as_int);
    1ff8:	2121      	movs	r1, #33	; 0x21
    1ffa:	2003      	movs	r0, #3
    1ffc:	f7fe f8fd 	bl	1fa <mbus_remote_register_write>
    delay(100000); 
    2000:	48ab      	ldr	r0, [pc, #684]	; (22b0 <main+0x43c>)
    2002:	f7fe f851 	bl	a8 <delay>
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);
    2006:	2217      	movs	r2, #23
    2008:	2120      	movs	r1, #32
    200a:	2003      	movs	r0, #3
    200c:	f7fe f8f5 	bl	1fa <mbus_remote_register_write>
    delay(100);
    2010:	2064      	movs	r0, #100	; 0x64
    2012:	f7fe f849 	bl	a8 <delay>
    mbus_remote_register_write(LNT_ADDR,0x17,lntv1a_r17.as_int);
    2016:	2211      	movs	r2, #17
    2018:	2117      	movs	r1, #23
    201a:	2003      	movs	r0, #3
    201c:	f7fe f8ed 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    2020:	20fa      	movs	r0, #250	; 0xfa
    2022:	0080      	lsls	r0, r0, #2
    2024:	f7fe f840 	bl	a8 <delay>
    lntv1a_r01.CTRL_IBIAS_I = 0x2; // Default : 0x8
    2028:	22b9      	movs	r2, #185	; 0xb9
    202a:	9b01      	ldr	r3, [sp, #4]
    202c:	0052      	lsls	r2, r2, #1
    202e:	0a5b      	lsrs	r3, r3, #9
    2030:	025b      	lsls	r3, r3, #9
    2032:	4313      	orrs	r3, r2
    2034:	9a01      	ldr	r2, [sp, #4]
    2036:	b29b      	uxth	r3, r3
    2038:	0c12      	lsrs	r2, r2, #16
    203a:	0412      	lsls	r2, r2, #16
    203c:	431a      	orrs	r2, r3
    mbus_remote_register_write(LNT_ADDR,0x01,lntv1a_r01.as_int);
    203e:	2101      	movs	r1, #1
    2040:	2003      	movs	r0, #3
    2042:	f7fe f8da 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    2046:	20fa      	movs	r0, #250	; 0xfa
    2048:	0080      	lsls	r0, r0, #2
    204a:	f7fe f82d 	bl	a8 <delay>
    mbus_remote_register_write(LNT_ADDR,0x02,lntv1a_r02.as_int);
    204e:	2102      	movs	r1, #2
    2050:	4a98      	ldr	r2, [pc, #608]	; (22b4 <main+0x440>)
    2052:	2003      	movs	r0, #3
    2054:	f7fe f8d1 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    2058:	20fa      	movs	r0, #250	; 0xfa
    205a:	0080      	lsls	r0, r0, #2
    205c:	f7fe f824 	bl	a8 <delay>
    mbus_remote_register_write(LNT_ADDR,0x05,lntv1a_r05.as_int);
    2060:	2105      	movs	r1, #5
    2062:	2003      	movs	r0, #3
    2064:	4a94      	ldr	r2, [pc, #592]	; (22b8 <main+0x444>)
    2066:	f7fe f8c8 	bl	1fa <mbus_remote_register_write>
    mbus_remote_register_write(LNT_ADDR,0x06,0);
    206a:	003a      	movs	r2, r7
    206c:	2106      	movs	r1, #6
    206e:	2003      	movs	r0, #3
    2070:	f7fe f8c3 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    2074:	20fa      	movs	r0, #250	; 0xfa
    2076:	0080      	lsls	r0, r0, #2
    2078:	f7fe f816 	bl	a8 <delay>
    mbus_remote_register_write(LNT_ADDR,0x03,0xFFFFFF);
    207c:	2103      	movs	r1, #3
    207e:	4a8f      	ldr	r2, [pc, #572]	; (22bc <main+0x448>)
    2080:	0008      	movs	r0, r1
    2082:	f7fe f8ba 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    2086:	20fa      	movs	r0, #250	; 0xfa
    2088:	0080      	lsls	r0, r0, #2
    208a:	f7fe f80d 	bl	a8 <delay>
    mbus_remote_register_write(LNT_ADDR,0x04,lntv1a_r04.as_int);
    208e:	0022      	movs	r2, r4
    2090:	2104      	movs	r1, #4
    2092:	2003      	movs	r0, #3
    2094:	f7fe f8b1 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    2098:	20fa      	movs	r0, #250	; 0xfa
    209a:	0080      	lsls	r0, r0, #2
    209c:	f7fe f804 	bl	a8 <delay>
    lntv1a_r00.LDC_PG = 0x0; // Default : 0x1
    20a0:	2201      	movs	r2, #1
    20a2:	4d87      	ldr	r5, [pc, #540]	; (22c0 <main+0x44c>)
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    20a4:	0039      	movs	r1, r7
    lntv1a_r00.LDC_PG = 0x0; // Default : 0x1
    20a6:	782b      	ldrb	r3, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    20a8:	2003      	movs	r0, #3
    lntv1a_r00.LDC_PG = 0x0; // Default : 0x1
    20aa:	4393      	bics	r3, r2
    20ac:	702b      	strb	r3, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    20ae:	682a      	ldr	r2, [r5, #0]
    20b0:	f7fe f8a3 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    20b4:	20fa      	movs	r0, #250	; 0xfa
    20b6:	0080      	lsls	r0, r0, #2
    20b8:	f7fd fff6 	bl	a8 <delay>
    lntv1a_r00.LDC_ISOLATE = 0x0; // Default : 0x1
    20bc:	782b      	ldrb	r3, [r5, #0]
    20be:	3c08      	subs	r4, #8
    20c0:	43a3      	bics	r3, r4
    20c2:	702b      	strb	r3, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    20c4:	682a      	ldr	r2, [r5, #0]
    20c6:	0039      	movs	r1, r7
    20c8:	2003      	movs	r0, #3
    20ca:	f7fe f896 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    20ce:	20fa      	movs	r0, #250	; 0xfa
    20d0:	0080      	lsls	r0, r0, #2
    20d2:	f7fd ffe9 	bl	a8 <delay>
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    20d6:	4d7b      	ldr	r5, [pc, #492]	; (22c4 <main+0x450>)
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    20d8:	4a7b      	ldr	r2, [pc, #492]	; (22c8 <main+0x454>)
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    20da:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    20dc:	2103      	movs	r1, #3
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    20de:	401e      	ands	r6, r3
    20e0:	602e      	str	r6, [r5, #0]
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    20e2:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    20e4:	0020      	movs	r0, r4
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    20e6:	4013      	ands	r3, r2
    20e8:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    20ea:	682a      	ldr	r2, [r5, #0]
    20ec:	f7fe f885 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
    20f0:	2380      	movs	r3, #128	; 0x80
    20f2:	682a      	ldr	r2, [r5, #0]
    20f4:	031b      	lsls	r3, r3, #12
    20f6:	4313      	orrs	r3, r2
    20f8:	602b      	str	r3, [r5, #0]
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    20fa:	2380      	movs	r3, #128	; 0x80
    20fc:	682a      	ldr	r2, [r5, #0]
    20fe:	02db      	lsls	r3, r3, #11
    2100:	4313      	orrs	r3, r2
    2102:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    2104:	2103      	movs	r1, #3
    2106:	0020      	movs	r0, r4
    2108:	682a      	ldr	r2, [r5, #0]
    210a:	f7fe f876 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r00.TRX_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    210e:	227e      	movs	r2, #126	; 0x7e
    2110:	2010      	movs	r0, #16
    2112:	4e6e      	ldr	r6, [pc, #440]	; (22cc <main+0x458>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    2114:	0039      	movs	r1, r7
    mrrv11a_r00.TRX_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    2116:	6833      	ldr	r3, [r6, #0]
    2118:	4393      	bics	r3, r2
    211a:	4303      	orrs	r3, r0
    211c:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    211e:	6832      	ldr	r2, [r6, #0]
    2120:	0020      	movs	r0, r4
    2122:	f7fe f86a 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    2126:	2201      	movs	r2, #1
    2128:	6833      	ldr	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    212a:	0039      	movs	r1, r7
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    212c:	4313      	orrs	r3, r2
    212e:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    2130:	6832      	ldr	r2, [r6, #0]
    2132:	0020      	movs	r0, r4
    2134:	f7fe f861 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*200); // Wait for decap to charge
    2138:	4865      	ldr	r0, [pc, #404]	; (22d0 <main+0x45c>)
    213a:	f7fd ffb5 	bl	a8 <delay>
    mbus_remote_register_write(MRR_ADDR,0x1F,mrrv11a_r1F.as_int);
    213e:	220f      	movs	r2, #15
    2140:	211f      	movs	r1, #31
    2142:	0020      	movs	r0, r4
    2144:	f7fe f859 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r12.FSM_TX_PW_LEN = 12; //50us PW
    2148:	210c      	movs	r1, #12
    214a:	4b62      	ldr	r3, [pc, #392]	; (22d4 <main+0x460>)
    mrrv11a_r13.FSM_TX_C_LEN = 1105;
    214c:	4862      	ldr	r0, [pc, #392]	; (22d8 <main+0x464>)
    mrrv11a_r12.FSM_TX_PW_LEN = 12; //50us PW
    214e:	681a      	ldr	r2, [r3, #0]
    2150:	0a92      	lsrs	r2, r2, #10
    2152:	0292      	lsls	r2, r2, #10
    2154:	430a      	orrs	r2, r1
    2156:	601a      	str	r2, [r3, #0]
    mrrv11a_r13.FSM_TX_C_LEN = 1105;
    2158:	4a5f      	ldr	r2, [pc, #380]	; (22d8 <main+0x464>)
    215a:	4960      	ldr	r1, [pc, #384]	; (22dc <main+0x468>)
    215c:	8812      	ldrh	r2, [r2, #0]
    215e:	0bd2      	lsrs	r2, r2, #15
    2160:	03d2      	lsls	r2, r2, #15
    2162:	430a      	orrs	r2, r1
    2164:	8002      	strh	r2, [r0, #0]
    mrrv11a_r12.FSM_TX_PS_LEN = 25; // PW=PS   
    2166:	681a      	ldr	r2, [r3, #0]
    2168:	495d      	ldr	r1, [pc, #372]	; (22e0 <main+0x46c>)
    mbus_remote_register_write(MRR_ADDR,0x12,mrrv11a_r12.as_int);
    216a:	0020      	movs	r0, r4
    mrrv11a_r12.FSM_TX_PS_LEN = 25; // PW=PS   
    216c:	4011      	ands	r1, r2
    216e:	22c8      	movs	r2, #200	; 0xc8
    2170:	01d2      	lsls	r2, r2, #7
    2172:	430a      	orrs	r2, r1
    2174:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x12,mrrv11a_r12.as_int);
    2176:	681a      	ldr	r2, [r3, #0]
    2178:	2112      	movs	r1, #18
    217a:	f7fe f83e 	bl	1fa <mbus_remote_register_write>
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv11a_r13.as_int);
    217e:	4b56      	ldr	r3, [pc, #344]	; (22d8 <main+0x464>)
    2180:	2113      	movs	r1, #19
    2182:	681a      	ldr	r2, [r3, #0]
    2184:	0020      	movs	r0, r4
    2186:	f7fe f838 	bl	1fa <mbus_remote_register_write>
    mrr_freq_hopping_step = 4; // determining center freq
    218a:	2204      	movs	r2, #4
    mrr_freq_hopping = 2;
    218c:	4b55      	ldr	r3, [pc, #340]	; (22e4 <main+0x470>)
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    218e:	2106      	movs	r1, #6
    mrr_freq_hopping = 2;
    2190:	801c      	strh	r4, [r3, #0]
    mrr_freq_hopping_step = 4; // determining center freq
    2192:	4b55      	ldr	r3, [pc, #340]	; (22e8 <main+0x474>)
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    2194:	0020      	movs	r0, r4
    mrr_freq_hopping_step = 4; // determining center freq
    2196:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    2198:	2280      	movs	r2, #128	; 0x80
    mrr_cfo_val_fine_min = 0x0000;
    219a:	4b54      	ldr	r3, [pc, #336]	; (22ec <main+0x478>)
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    219c:	0152      	lsls	r2, r2, #5
    mrr_cfo_val_fine_min = 0x0000;
    219e:	801f      	strh	r7, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    21a0:	f7fe f82b 	bl	1fa <mbus_remote_register_write>
    mbus_remote_register_write(MRR_ADDR,0x08,0x400000); // RO_POLY
    21a4:	2280      	movs	r2, #128	; 0x80
    21a6:	2108      	movs	r1, #8
    21a8:	0020      	movs	r0, r4
    21aa:	03d2      	lsls	r2, r2, #15
    21ac:	f7fe f825 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r07.RO_MOM = 0x10;
    21b0:	217f      	movs	r1, #127	; 0x7f
    21b2:	2010      	movs	r0, #16
    21b4:	4a4e      	ldr	r2, [pc, #312]	; (22f0 <main+0x47c>)
    21b6:	8813      	ldrh	r3, [r2, #0]
    21b8:	438b      	bics	r3, r1
    21ba:	4303      	orrs	r3, r0
    21bc:	8013      	strh	r3, [r2, #0]
    mrrv11a_r07.RO_MIM = 0x10;
    21be:	8813      	ldrh	r3, [r2, #0]
    21c0:	494c      	ldr	r1, [pc, #304]	; (22f4 <main+0x480>)
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);
    21c2:	0020      	movs	r0, r4
    mrrv11a_r07.RO_MIM = 0x10;
    21c4:	4019      	ands	r1, r3
    21c6:	2380      	movs	r3, #128	; 0x80
    21c8:	011b      	lsls	r3, r3, #4
    21ca:	430b      	orrs	r3, r1
    21cc:	8013      	strh	r3, [r2, #0]
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);
    21ce:	2107      	movs	r1, #7
    21d0:	6812      	ldr	r2, [r2, #0]
    21d2:	f7fe f812 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = 0xFF;  //ANT CAP 10b unary 830.5 MHz // FIXME: adjust per stack    // TODO: make them the same variable
    21d6:	6833      	ldr	r3, [r6, #0]
    21d8:	4a47      	ldr	r2, [pc, #284]	; (22f8 <main+0x484>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    21da:	0039      	movs	r1, r7
    mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = 0xFF;  //ANT CAP 10b unary 830.5 MHz // FIXME: adjust per stack    // TODO: make them the same variable
    21dc:	401a      	ands	r2, r3
    21de:	23ff      	movs	r3, #255	; 0xff
    21e0:	01db      	lsls	r3, r3, #7
    21e2:	4313      	orrs	r3, r2
    21e4:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    21e6:	6832      	ldr	r2, [r6, #0]
    21e8:	0020      	movs	r0, r4
    21ea:	f7fe f806 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_COARSE = 0xFF; //ANT CAP 10b unary 830.5 MHz
    21ee:	21ff      	movs	r1, #255	; 0xff
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
    21f0:	263f      	movs	r6, #63	; 0x3f
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_COARSE = 0xFF; //ANT CAP 10b unary 830.5 MHz
    21f2:	4a42      	ldr	r2, [pc, #264]	; (22fc <main+0x488>)
    21f4:	6813      	ldr	r3, [r2, #0]
    21f6:	0a9b      	lsrs	r3, r3, #10
    21f8:	029b      	lsls	r3, r3, #10
    21fa:	430b      	orrs	r3, r1
    21fc:	6013      	str	r3, [r2, #0]
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
    21fe:	4b3b      	ldr	r3, [pc, #236]	; (22ec <main+0x478>)
    2200:	8818      	ldrh	r0, [r3, #0]
    2202:	6811      	ldr	r1, [r2, #0]
    2204:	4030      	ands	r0, r6
    2206:	0403      	lsls	r3, r0, #16
    2208:	483d      	ldr	r0, [pc, #244]	; (2300 <main+0x48c>)
    220a:	4008      	ands	r0, r1
    220c:	4318      	orrs	r0, r3
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
    220e:	4b37      	ldr	r3, [pc, #220]	; (22ec <main+0x478>)
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
    2210:	6010      	str	r0, [r2, #0]
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
    2212:	8819      	ldrh	r1, [r3, #0]
    2214:	483b      	ldr	r0, [pc, #236]	; (2304 <main+0x490>)
    2216:	6813      	ldr	r3, [r2, #0]
    2218:	4031      	ands	r1, r6
    221a:	0289      	lsls	r1, r1, #10
    221c:	4003      	ands	r3, r0
    221e:	430b      	orrs	r3, r1
    2220:	6013      	str	r3, [r2, #0]
    mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
    2222:	2101      	movs	r1, #1
    2224:	0020      	movs	r0, r4
    2226:	6812      	ldr	r2, [r2, #0]
    2228:	f7fd ffe7 	bl	1fa <mbus_remote_register_write>
    mbus_remote_register_write(MRR_ADDR,0x02,mrrv11a_r02.as_int);
    222c:	0021      	movs	r1, r4
    222e:	0020      	movs	r0, r4
    2230:	4a35      	ldr	r2, [pc, #212]	; (2308 <main+0x494>)
    2232:	f7fd ffe2 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r03.TRX_MODE_EN = 0; //Set TRX mode
    2236:	682b      	ldr	r3, [r5, #0]
    2238:	4a34      	ldr	r2, [pc, #208]	; (230c <main+0x498>)
    mbus_remote_register_write(MRR_ADDR,3,mrrv11a_r03.as_int);
    223a:	2103      	movs	r1, #3
    mrrv11a_r03.TRX_MODE_EN = 0; //Set TRX mode
    223c:	4013      	ands	r3, r2
    223e:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,3,mrrv11a_r03.as_int);
    2240:	0020      	movs	r0, r4
    2242:	682a      	ldr	r2, [r5, #0]
    2244:	f7fd ffd9 	bl	1fa <mbus_remote_register_write>
    mbus_remote_register_write(MRR_ADDR,0x14,mrrv11a_r14.as_int);
    2248:	003a      	movs	r2, r7
    224a:	2114      	movs	r1, #20
    224c:	0020      	movs	r0, r4
    224e:	f7fd ffd4 	bl	1fa <mbus_remote_register_write>
    mbus_remote_register_write(MRR_ADDR,0x15,mrrv11a_r15.as_int);
    2252:	003a      	movs	r2, r7
    2254:	2115      	movs	r1, #21
    2256:	e05b      	b.n	2310 <main+0x49c>
    2258:	e000e100 	.word	0xe000e100
    225c:	00000f0d 	.word	0x00000f0d
    2260:	00003960 	.word	0x00003960
    2264:	deadc51f 	.word	0xdeadc51f
    2268:	a0001200 	.word	0xa0001200
    226c:	a000007c 	.word	0xa000007c
    2270:	00003ae8 	.word	0x00003ae8
    2274:	00003aec 	.word	0x00003aec
    2278:	ffff8000 	.word	0xffff8000
    227c:	00003ae4 	.word	0x00003ae4
    2280:	00003950 	.word	0x00003950
    2284:	000038ec 	.word	0x000038ec
    2288:	0000394e 	.word	0x0000394e
    228c:	000037cc 	.word	0x000037cc
    2290:	00003940 	.word	0x00003940
    2294:	00003944 	.word	0x00003944
    2298:	00003acc 	.word	0x00003acc
    229c:	00003678 	.word	0x00003678
    22a0:	ff00003f 	.word	0xff00003f
    22a4:	003ffec0 	.word	0x003ffec0
    22a8:	000080ff 	.word	0x000080ff
    22ac:	ffdfffff 	.word	0xffdfffff
    22b0:	000186a0 	.word	0x000186a0
    22b4:	0003cee7 	.word	0x0003cee7
    22b8:	00030010 	.word	0x00030010
    22bc:	00ffffff 	.word	0x00ffffff
    22c0:	000038fc 	.word	0x000038fc
    22c4:	00003910 	.word	0x00003910
    22c8:	ffefffff 	.word	0xffefffff
    22cc:	0000390c 	.word	0x0000390c
    22d0:	00004e20 	.word	0x00004e20
    22d4:	00003924 	.word	0x00003924
    22d8:	00003928 	.word	0x00003928
    22dc:	00000451 	.word	0x00000451
    22e0:	fff003ff 	.word	0xfff003ff
    22e4:	00003958 	.word	0x00003958
    22e8:	000039f4 	.word	0x000039f4
    22ec:	00003954 	.word	0x00003954
    22f0:	00003918 	.word	0x00003918
    22f4:	ffffc07f 	.word	0xffffc07f
    22f8:	fffe007f 	.word	0xfffe007f
    22fc:	00003a60 	.word	0x00003a60
    2300:	ffc0ffff 	.word	0xffc0ffff
    2304:	ffff03ff 	.word	0xffff03ff
    2308:	00039fff 	.word	0x00039fff
    230c:	ffffbfff 	.word	0xffffbfff
    2310:	0020      	movs	r0, r4
    2312:	f7fd ff72 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r10.FSM_TX_TP_LEN = 80;       // Num. Training Pulses (bit 0); Valid Range: 1 ~ 255 (Default: 80)
    2316:	22ff      	movs	r2, #255	; 0xff
    2318:	4b89      	ldr	r3, [pc, #548]	; (2540 <main+0x6cc>)
    mbus_remote_register_write(MRR_ADDR,0x10,mrrv11a_r10.as_int);
    231a:	0020      	movs	r0, r4
    mrrv11a_r10.FSM_TX_TP_LEN = 80;       // Num. Training Pulses (bit 0); Valid Range: 1 ~ 255 (Default: 80)
    231c:	6819      	ldr	r1, [r3, #0]
    231e:	4391      	bics	r1, r2
    2320:	3aaf      	subs	r2, #175	; 0xaf
    2322:	430a      	orrs	r2, r1
    2324:	601a      	str	r2, [r3, #0]
    mrrv11a_r10.FSM_TX_PASSCODE = 0x7AC8; // 16-bit Passcode (sent from LSB to MSB)
    2326:	681a      	ldr	r2, [r3, #0]
    2328:	4986      	ldr	r1, [pc, #536]	; (2544 <main+0x6d0>)
    232a:	4011      	ands	r1, r2
    232c:	4a86      	ldr	r2, [pc, #536]	; (2548 <main+0x6d4>)
    232e:	430a      	orrs	r2, r1
    2330:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x10,mrrv11a_r10.as_int);
    2332:	681a      	ldr	r2, [r3, #0]
    2334:	2110      	movs	r1, #16
    2336:	f7fd ff60 	bl	1fa <mbus_remote_register_write>
    mrrv11a_r11.FSM_TX_DATA_BITS = RADIO_DATA_NUM_WORDS * 32; //0-skip tx data
    233a:	4984      	ldr	r1, [pc, #528]	; (254c <main+0x6d8>)
    233c:	4a84      	ldr	r2, [pc, #528]	; (2550 <main+0x6dc>)
    233e:	680b      	ldr	r3, [r1, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    2340:	0020      	movs	r0, r4
    mrrv11a_r11.FSM_TX_DATA_BITS = RADIO_DATA_NUM_WORDS * 32; //0-skip tx data
    2342:	401a      	ands	r2, r3
    2344:	23c0      	movs	r3, #192	; 0xc0
    2346:	01db      	lsls	r3, r3, #7
    2348:	4313      	orrs	r3, r2
    234a:	600b      	str	r3, [r1, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    234c:	680a      	ldr	r2, [r1, #0]
    234e:	2111      	movs	r1, #17
    2350:	f7fd ff53 	bl	1fa <mbus_remote_register_write>
    mbus_remote_register_write(MRR_ADDR,0x1E,0x1002);
    2354:	4a7f      	ldr	r2, [pc, #508]	; (2554 <main+0x6e0>)
    2356:	211e      	movs	r1, #30
    2358:	0020      	movs	r0, r4
    235a:	f7fd ff4e 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*200); // Wait for decap to charge
    235e:	487e      	ldr	r0, [pc, #504]	; (2558 <main+0x6e4>)
    2360:	f7fd fea2 	bl	a8 <delay>
    radio_on = 0;
    2364:	4b7d      	ldr	r3, [pc, #500]	; (255c <main+0x6e8>)
    pmu_set_active_clk(0x0D020F0F);
    2366:	487e      	ldr	r0, [pc, #504]	; (2560 <main+0x6ec>)
    radio_on = 0;
    2368:	801f      	strh	r7, [r3, #0]
    radio_ready = 0;
    236a:	4b7e      	ldr	r3, [pc, #504]	; (2564 <main+0x6f0>)
    236c:	801f      	strh	r7, [r3, #0]
    pmu_set_active_clk(0x0D020F0F);
    236e:	f7fe f815 	bl	39c <pmu_set_active_clk>
    pmu_set_sleep_clk(0x0F010102);
    2372:	487d      	ldr	r0, [pc, #500]	; (2568 <main+0x6f4>)
    2374:	f7fd ffed 	bl	352 <pmu_set_sleep_clk>
    pmu_set_sar_ratio(52);
    2378:	2034      	movs	r0, #52	; 0x34
    237a:	f7ff f88b 	bl	1494 <pmu_set_sar_ratio>
    pmu_reg_write(0x45,         // FIXME: this register is reserved in PMUv10
    237e:	2148      	movs	r1, #72	; 0x48
    2380:	2045      	movs	r0, #69	; 0x45
    2382:	f7fd ffd9 	bl	338 <pmu_reg_write>
    pmu_reg_write(0x10,         // PMU_SAR_TRIM_ACTIVE
    2386:	2010      	movs	r0, #16
    2388:	4978      	ldr	r1, [pc, #480]	; (256c <main+0x6f8>)
    238a:	f7fd ffd5 	bl	338 <pmu_reg_write>
    pmu_reg_write(0x3C,         // PMU_EN_CONTROLLER_DESIRED_STATE_ACTIVE
    238e:	203c      	movs	r0, #60	; 0x3c
    2390:	4977      	ldr	r1, [pc, #476]	; (2570 <main+0x6fc>)
    2392:	f7fd ffd1 	bl	338 <pmu_reg_write>
    pmu_reg_write(0x3B,         // PMU_EN_CONTROLLER_DESIRED_STATE_SLEEP
    2396:	203b      	movs	r0, #59	; 0x3b
    2398:	4976      	ldr	r1, [pc, #472]	; (2574 <main+0x700>)
    239a:	f7fd ffcd 	bl	338 <pmu_reg_write>
    pmu_reg_write (33, // 0x21 (TICK_SAR_RESET)
    239e:	2101      	movs	r1, #1
    23a0:	2021      	movs	r0, #33	; 0x21
    23a2:	f7fd ffc9 	bl	338 <pmu_reg_write>
    pmu_reg_write (7, // 0x7 (SAR_RATIO_OFFSET_DOWN_ACTIVE)
    23a6:	0039      	movs	r1, r7
    23a8:	2007      	movs	r0, #7
    23aa:	f7fd ffc5 	bl	338 <pmu_reg_write>
    pmu_reg_write (6, // 0x6 (SAR_RATIO_OFFSET_DOWN_SLEEP)
    23ae:	0039      	movs	r1, r7
    23b0:	2006      	movs	r0, #6
    23b2:	f7fd ffc1 	bl	338 <pmu_reg_write>
    pmu_reg_write (58, // 0x3A (CTRL_STALL_STATE_ACTIVE)
    23b6:	0039      	movs	r1, r7
    23b8:	203a      	movs	r0, #58	; 0x3a
    23ba:	f7fd ffbd 	bl	338 <pmu_reg_write>
    pmu_reg_write(0x47, // use slow loop and active-mode PRC clk tracking
    23be:	2047      	movs	r0, #71	; 0x47
    23c0:	496d      	ldr	r1, [pc, #436]	; (2578 <main+0x704>)
    23c2:	f7fd ffb9 	bl	338 <pmu_reg_write>
    xo_init();
    23c6:	f7fe f8b5 	bl	534 <xo_init>
    update_system_time();
    23ca:	f7ff f8ef 	bl	15ac <update_system_time>
    update_system_time();
    23ce:	f7ff f8ed 	bl	15ac <update_system_time>
    update_system_time();
    23d2:	f7ff f8eb 	bl	15ac <update_system_time>
        if(option) {
            // safe sleep mode
            // FIXME: this might be bugged
        pmu_set_sleep_low();
            
        operation_sleep();
    23d6:	f7fe f89d 	bl	514 <operation_sleep>
    pmu_reg_write(0x00, 0x03);
    23da:	2103      	movs	r1, #3
    23dc:	2000      	movs	r0, #0
    23de:	f7fd ffab 	bl	338 <pmu_reg_write>
    read_data_batadc = *REG0 & 0xFF;    // TODO: check if only need low 8 bits
    23e2:	23a0      	movs	r3, #160	; 0xa0
    23e4:	061b      	lsls	r3, r3, #24
    23e6:	681b      	ldr	r3, [r3, #0]
    23e8:	4a64      	ldr	r2, [pc, #400]	; (257c <main+0x708>)
    23ea:	b2db      	uxtb	r3, r3
    pmu_setting_temp_based(0);
    23ec:	2000      	movs	r0, #0
    read_data_batadc = *REG0 & 0xFF;    // TODO: check if only need low 8 bits
    23ee:	8013      	strh	r3, [r2, #0]
    pmu_setting_temp_based(0);
    23f0:	f7ff f890 	bl	1514 <pmu_setting_temp_based>
    update_system_time();
    23f4:	f7ff f8da 	bl	15ac <update_system_time>
    if(projected_end_time_in_sec == 0) {
    23f8:	4c61      	ldr	r4, [pc, #388]	; (2580 <main+0x70c>)
    23fa:	6821      	ldr	r1, [r4, #0]
    23fc:	2900      	cmp	r1, #0
    23fe:	d00f      	beq.n	2420 <main+0x5ac>
    if(xo_sys_time - projected_end_time > TIMER_MARGIN 
    2400:	2080      	movs	r0, #128	; 0x80
    2402:	4d60      	ldr	r5, [pc, #384]	; (2584 <main+0x710>)
    uint32_t projected_end_time = projected_end_time_in_sec << XO_TO_SEC_SHIFT;
    2404:	03cb      	lsls	r3, r1, #15
    if(xo_sys_time - projected_end_time > TIMER_MARGIN 
    2406:	682a      	ldr	r2, [r5, #0]
    2408:	00c0      	lsls	r0, r0, #3
    240a:	1ad2      	subs	r2, r2, r3
    240c:	4282      	cmp	r2, r0
    240e:	d94a      	bls.n	24a6 <main+0x632>
    && xo_sys_time_in_sec >= projected_end_time_in_sec) {
    2410:	4a5d      	ldr	r2, [pc, #372]	; (2588 <main+0x714>)
    2412:	6812      	ldr	r2, [r2, #0]
    2414:	4291      	cmp	r1, r2
    2416:	d846      	bhi.n	24a6 <main+0x632>
        xo_lnt_mplier--;
    2418:	4a5c      	ldr	r2, [pc, #368]	; (258c <main+0x718>)
    241a:	8813      	ldrh	r3, [r2, #0]
    241c:	3b01      	subs	r3, #1
        xo_lnt_mplier++;
    241e:	8013      	strh	r3, [r2, #0]
    if(*SREG_WAKEUP_SOURCE & 1) {
    2420:	4b5b      	ldr	r3, [pc, #364]	; (2590 <main+0x71c>)
    2422:	681b      	ldr	r3, [r3, #0]
    2424:	07db      	lsls	r3, r3, #31
    2426:	d501      	bpl.n	242c <main+0x5b8>
        set_goc_cmd();
    2428:	f7ff fb66 	bl	1af8 <set_goc_cmd>
    lnt_stop();
    242c:	f7fe f91e 	bl	66c <lnt_stop>
    operation_temp_run();
    2430:	f7ff f918 	bl	1664 <operation_temp_run>
    pmu_setting_temp_based(0);
    2434:	2000      	movs	r0, #0
    2436:	f7ff f86d 	bl	1514 <pmu_setting_temp_based>
    mrr_send_enable = set_send_enable();
    243a:	f7ff fc2f 	bl	1c9c <set_send_enable>
    243e:	4e55      	ldr	r6, [pc, #340]	; (2594 <main+0x720>)
    uint16_t goc_data_header = goc_data_full >> 24;
    2440:	4b55      	ldr	r3, [pc, #340]	; (2598 <main+0x724>)
    mrr_send_enable = set_send_enable();
    2442:	8030      	strh	r0, [r6, #0]
    uint16_t goc_data_header = goc_data_full >> 24;
    2444:	6819      	ldr	r1, [r3, #0]
    bool option = ((goc_data_full >> 23) & 0x1)? true : false;
    2446:	681d      	ldr	r5, [r3, #0]
    uint16_t goc_data_header = goc_data_full >> 24;
    2448:	0e09      	lsrs	r1, r1, #24
    if(goc_data_header == 0x00) {
    244a:	d100      	bne.n	244e <main+0x5da>
    244c:	e0f8      	b.n	2640 <main+0x7cc>
    bool option = ((goc_data_full >> 23) & 0x1)? true : false;
    244e:	2701      	movs	r7, #1
    2450:	0ded      	lsrs	r5, r5, #23
    uint16_t goc_data_header = goc_data_full >> 24;
    2452:	b28a      	uxth	r2, r1
    bool option = ((goc_data_full >> 23) & 0x1)? true : false;
    2454:	403d      	ands	r5, r7
   else if(goc_data_header == 0x01) {
    2456:	42b9      	cmp	r1, r7
    2458:	d131      	bne.n	24be <main+0x64a>
        bool option2 = ((goc_data_full >> 22) & 0x1)? true : false;
    245a:	681a      	ldr	r2, [r3, #0]
        if(option2) {
    245c:	484f      	ldr	r0, [pc, #316]	; (259c <main+0x728>)
        bool option2 = ((goc_data_full >> 22) & 0x1)? true : false;
    245e:	0d92      	lsrs	r2, r2, #22
        if(option2) {
    2460:	420a      	tst	r2, r1
    2462:	d003      	beq.n	246c <main+0x5f8>
            CHIP_ID = goc_data_full & 0x1F;
    2464:	221f      	movs	r2, #31
    2466:	681b      	ldr	r3, [r3, #0]
    2468:	4013      	ands	r3, r2
    246a:	8003      	strh	r3, [r0, #0]
        radio_data_arr[2] = (0x1 << 8) | CHIP_ID;
    246c:	2380      	movs	r3, #128	; 0x80
    246e:	8802      	ldrh	r2, [r0, #0]
    2470:	005b      	lsls	r3, r3, #1
    2472:	4313      	orrs	r3, r2
    2474:	4a4a      	ldr	r2, [pc, #296]	; (25a0 <main+0x72c>)
    2476:	6093      	str	r3, [r2, #8]
        radio_data_arr[1] = (VERSION_NUM << 16) | read_data_batadc;
    2478:	4b40      	ldr	r3, [pc, #256]	; (257c <main+0x708>)
    247a:	8819      	ldrh	r1, [r3, #0]
    247c:	23c6      	movs	r3, #198	; 0xc6
    247e:	04db      	lsls	r3, r3, #19
    2480:	430b      	orrs	r3, r1
    2482:	6053      	str	r3, [r2, #4]
        radio_data_arr[0] = snt_sys_temp_code;
    2484:	4b47      	ldr	r3, [pc, #284]	; (25a4 <main+0x730>)
    2486:	681b      	ldr	r3, [r3, #0]
    2488:	6013      	str	r3, [r2, #0]
        send_beacon();
    248a:	f7ff fc7f 	bl	1d8c <send_beacon>
        if(option) {
    248e:	2d00      	cmp	r5, #0
    2490:	d100      	bne.n	2494 <main+0x620>
    2492:	e0d5      	b.n	2640 <main+0x7cc>
    pmu_set_sleep_clk(PMU_SLEEP_SETTINGS[2]);
    2494:	4b44      	ldr	r3, [pc, #272]	; (25a8 <main+0x734>)
    2496:	6898      	ldr	r0, [r3, #8]
    2498:	f7fd ff5b 	bl	352 <pmu_set_sleep_clk>
    pmu_set_sar_ratio(PMU_SLEEP_SAR_SETTINGS[2]);
    249c:	4b43      	ldr	r3, [pc, #268]	; (25ac <main+0x738>)
    249e:	6898      	ldr	r0, [r3, #8]
    24a0:	f7fe fff8 	bl	1494 <pmu_set_sar_ratio>
    24a4:	e797      	b.n	23d6 <main+0x562>
    else if(projected_end_time - xo_sys_time > TIMER_MARGIN 
    24a6:	682a      	ldr	r2, [r5, #0]
    24a8:	1a9b      	subs	r3, r3, r2
    24aa:	4283      	cmp	r3, r0
    24ac:	d9b8      	bls.n	2420 <main+0x5ac>
        && xo_sys_time_in_sec <= projected_end_time_in_sec) {
    24ae:	4b36      	ldr	r3, [pc, #216]	; (2588 <main+0x714>)
    24b0:	681b      	ldr	r3, [r3, #0]
    24b2:	4299      	cmp	r1, r3
    24b4:	d3b4      	bcc.n	2420 <main+0x5ac>
        xo_lnt_mplier++;
    24b6:	4a35      	ldr	r2, [pc, #212]	; (258c <main+0x718>)
    24b8:	8813      	ldrh	r3, [r2, #0]
    24ba:	3301      	adds	r3, #1
    24bc:	e7af      	b.n	241e <main+0x5aa>
        }
    }
    else if(goc_data_header == 0x02) {
    24be:	2a02      	cmp	r2, #2
    24c0:	d000      	beq.n	24c4 <main+0x650>
    24c2:	e079      	b.n	25b8 <main+0x744>
        // packet blaster: send packets for N seconds, then sleep for M seconds
        uint8_t N = (goc_data_full >> 16) & 0xFF;
    24c4:	681d      	ldr	r5, [r3, #0]
        uint8_t M = (goc_data_full >> 8) & 0xFF;
    24c6:	681b      	ldr	r3, [r3, #0]
    24c8:	0a1b      	lsrs	r3, r3, #8
    24ca:	9302      	str	r3, [sp, #8]

        if(goc_state == 0) {
    24cc:	4b38      	ldr	r3, [pc, #224]	; (25b0 <main+0x73c>)
    24ce:	881a      	ldrh	r2, [r3, #0]
    24d0:	b291      	uxth	r1, r2
    24d2:	2a00      	cmp	r2, #0
    24d4:	d102      	bne.n	24dc <main+0x668>
            op_counter = 0;   
    24d6:	4a37      	ldr	r2, [pc, #220]	; (25b4 <main+0x740>)
    24d8:	8011      	strh	r1, [r2, #0]
            goc_state = 1;
    24da:	801f      	strh	r7, [r3, #0]
        }

        pmu_setting_temp_based(1);
    24dc:	2001      	movs	r0, #1
    24de:	f7ff f819 	bl	1514 <pmu_setting_temp_based>
        uint32_t start_time_in_sec = xo_sys_time_in_sec;
    24e2:	4b29      	ldr	r3, [pc, #164]	; (2588 <main+0x714>)
    24e4:	681f      	ldr	r7, [r3, #0]
        if(mrr_send_enable) {
    24e6:	8833      	ldrh	r3, [r6, #0]
    24e8:	2b00      	cmp	r3, #0
    24ea:	d106      	bne.n	24fa <main+0x686>
                update_system_time();
            } while(xo_sys_time_in_sec - start_time_in_sec < N);
        }

        // set M second timer
        projected_end_time_in_sec = xo_sys_time_in_sec + M;
    24ec:	4b26      	ldr	r3, [pc, #152]	; (2588 <main+0x714>)
    24ee:	681a      	ldr	r2, [r3, #0]
    24f0:	466b      	mov	r3, sp
    24f2:	7a1b      	ldrb	r3, [r3, #8]
    }
    else if(goc_data_header == 0x05) {
        // battery drain test: wake up every N seconds and take measurements
        // Which is equivalent to waking up every N seconds
        uint16_t N = goc_data_full & 0xFFFF;
        projected_end_time_in_sec = xo_sys_time_in_sec + N;
    24f4:	189b      	adds	r3, r3, r2
    24f6:	6023      	str	r3, [r4, #0]
    24f8:	e0a2      	b.n	2640 <main+0x7cc>
                radio_data_arr[2] = (0x2 << 8) | CHIP_ID;
    24fa:	2680      	movs	r6, #128	; 0x80
        uint8_t N = (goc_data_full >> 16) & 0xFF;
    24fc:	0c2b      	lsrs	r3, r5, #16
                radio_data_arr[1] = goc_data_full & 0xFF;
    24fe:	25ff      	movs	r5, #255	; 0xff
        uint8_t N = (goc_data_full >> 16) & 0xFF;
    2500:	9301      	str	r3, [sp, #4]
                radio_data_arr[2] = (0x2 << 8) | CHIP_ID;
    2502:	00b6      	lsls	r6, r6, #2
    2504:	4b25      	ldr	r3, [pc, #148]	; (259c <main+0x728>)
    2506:	4a26      	ldr	r2, [pc, #152]	; (25a0 <main+0x72c>)
    2508:	881b      	ldrh	r3, [r3, #0]
                radio_data_arr[0] = op_counter++;
    250a:	492a      	ldr	r1, [pc, #168]	; (25b4 <main+0x740>)
                radio_data_arr[2] = (0x2 << 8) | CHIP_ID;
    250c:	4333      	orrs	r3, r6
    250e:	6093      	str	r3, [r2, #8]
                radio_data_arr[1] = goc_data_full & 0xFF;
    2510:	4b21      	ldr	r3, [pc, #132]	; (2598 <main+0x724>)
    2512:	681b      	ldr	r3, [r3, #0]
    2514:	402b      	ands	r3, r5
    2516:	6053      	str	r3, [r2, #4]
                radio_data_arr[0] = op_counter++;
    2518:	880b      	ldrh	r3, [r1, #0]
    251a:	b298      	uxth	r0, r3
    251c:	3301      	adds	r3, #1
    251e:	b29b      	uxth	r3, r3
    2520:	800b      	strh	r3, [r1, #0]
    2522:	6010      	str	r0, [r2, #0]
                mrr_send_radio_data(0);
    2524:	2000      	movs	r0, #0
    2526:	f7ff f94b 	bl	17c0 <mrr_send_radio_data>
                update_system_time();
    252a:	f7ff f83f 	bl	15ac <update_system_time>
            } while(xo_sys_time_in_sec - start_time_in_sec < N);
    252e:	4b16      	ldr	r3, [pc, #88]	; (2588 <main+0x714>)
    2530:	9a01      	ldr	r2, [sp, #4]
    2532:	681b      	ldr	r3, [r3, #0]
    2534:	402a      	ands	r2, r5
    2536:	1bdb      	subs	r3, r3, r7
    2538:	4293      	cmp	r3, r2
    253a:	d3e3      	bcc.n	2504 <main+0x690>
    253c:	e7d6      	b.n	24ec <main+0x678>
    253e:	46c0      	nop			; (mov r8, r8)
    2540:	0000391c 	.word	0x0000391c
    2544:	ff0000ff 	.word	0xff0000ff
    2548:	007ac800 	.word	0x007ac800
    254c:	00003920 	.word	0x00003920
    2550:	fffe00ff 	.word	0xfffe00ff
    2554:	00001002 	.word	0x00001002
    2558:	00004e20 	.word	0x00004e20
    255c:	000039fe 	.word	0x000039fe
    2560:	0d020f0f 	.word	0x0d020f0f
    2564:	00003956 	.word	0x00003956
    2568:	0f010102 	.word	0x0f010102
    256c:	0070a201 	.word	0x0070a201
    2570:	0017c7ff 	.word	0x0017c7ff
    2574:	0017efff 	.word	0x0017efff
    2578:	0008e28b 	.word	0x0008e28b
    257c:	000039b4 	.word	0x000039b4
    2580:	00003a80 	.word	0x00003a80
    2584:	00003ae8 	.word	0x00003ae8
    2588:	00003aec 	.word	0x00003aec
    258c:	0000394e 	.word	0x0000394e
    2590:	a000a008 	.word	0xa000a008
    2594:	00003908 	.word	0x00003908
    2598:	000039f8 	.word	0x000039f8
    259c:	00003a00 	.word	0x00003a00
    25a0:	000039b8 	.word	0x000039b8
    25a4:	00003938 	.word	0x00003938
    25a8:	00003890 	.word	0x00003890
    25ac:	00003874 	.word	0x00003874
    25b0:	000039ec 	.word	0x000039ec
    25b4:	000039fc 	.word	0x000039fc
    else if(goc_data_header == 0x03) {
    25b8:	2a03      	cmp	r2, #3
    25ba:	d000      	beq.n	25be <main+0x74a>
    25bc:	e0bc      	b.n	2738 <main+0x8c4>
        if(goc_state == 0) {
    25be:	4ad4      	ldr	r2, [pc, #848]	; (2910 <main+0xa9c>)
        uint16_t N = goc_data_full & 0xFFFF;
    25c0:	681b      	ldr	r3, [r3, #0]
        if(goc_state == 0) {
    25c2:	8811      	ldrh	r1, [r2, #0]
    25c4:	041b      	lsls	r3, r3, #16
    25c6:	2900      	cmp	r1, #0
    25c8:	d000      	beq.n	25cc <main+0x758>
    25ca:	e0b2      	b.n	2732 <main+0x8be>
            goc_state = 1;
    25cc:	8017      	strh	r7, [r2, #0]
            projected_end_time_in_sec = xo_sys_time_in_sec + N;
    25ce:	4ad1      	ldr	r2, [pc, #836]	; (2914 <main+0xaa0>)
    25d0:	0c1b      	lsrs	r3, r3, #16
    25d2:	6812      	ldr	r2, [r2, #0]
            projected_end_time_in_sec += N;
    25d4:	18d3      	adds	r3, r2, r3
    25d6:	6023      	str	r3, [r4, #0]
        if(mrr_send_enable) {
    25d8:	8833      	ldrh	r3, [r6, #0]
    25da:	2b00      	cmp	r3, #0
    25dc:	d030      	beq.n	2640 <main+0x7cc>
            mrr_send_radio_data(!option);
    25de:	2701      	movs	r7, #1
            pmu_setting_temp_based(1);
    25e0:	2001      	movs	r0, #1
    25e2:	f7fe ff97 	bl	1514 <pmu_setting_temp_based>
            update_system_time();
    25e6:	f7fe ffe1 	bl	15ac <update_system_time>
            radio_data_arr[2] = (0x3 << 8) | CHIP_ID;
    25ea:	4bcb      	ldr	r3, [pc, #812]	; (2918 <main+0xaa4>)
            mrr_send_radio_data(!option);
    25ec:	0038      	movs	r0, r7
            radio_data_arr[2] = (0x3 << 8) | CHIP_ID;
    25ee:	881a      	ldrh	r2, [r3, #0]
    25f0:	23c0      	movs	r3, #192	; 0xc0
    25f2:	4eca      	ldr	r6, [pc, #808]	; (291c <main+0xaa8>)
    25f4:	009b      	lsls	r3, r3, #2
    25f6:	4313      	orrs	r3, r2
    25f8:	60b3      	str	r3, [r6, #8]
            radio_data_arr[1] = xo_sys_time_in_sec;
    25fa:	4bc6      	ldr	r3, [pc, #792]	; (2914 <main+0xaa0>)
            radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
    25fc:	4ac8      	ldr	r2, [pc, #800]	; (2920 <main+0xaac>)
            radio_data_arr[1] = xo_sys_time_in_sec;
    25fe:	681b      	ldr	r3, [r3, #0]
            mrr_send_radio_data(!option);
    2600:	4068      	eors	r0, r5
            radio_data_arr[1] = xo_sys_time_in_sec;
    2602:	6073      	str	r3, [r6, #4]
            radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
    2604:	4bc7      	ldr	r3, [pc, #796]	; (2924 <main+0xab0>)
    2606:	881b      	ldrh	r3, [r3, #0]
    2608:	6812      	ldr	r2, [r2, #0]
    260a:	061b      	lsls	r3, r3, #24
    260c:	4313      	orrs	r3, r2
    260e:	6033      	str	r3, [r6, #0]
            mrr_send_radio_data(!option);
    2610:	f7ff f8d6 	bl	17c0 <mrr_send_radio_data>
            if(option) {
    2614:	2d00      	cmp	r5, #0
    2616:	d013      	beq.n	2640 <main+0x7cc>
                radio_data_arr[2] = (0x3 << 8) | (0x1 << 7) | CHIP_ID;
    2618:	4bbf      	ldr	r3, [pc, #764]	; (2918 <main+0xaa4>)
    261a:	881a      	ldrh	r2, [r3, #0]
    261c:	23e0      	movs	r3, #224	; 0xe0
    261e:	009b      	lsls	r3, r3, #2
    2620:	4313      	orrs	r3, r2
    2622:	60b3      	str	r3, [r6, #8]
                radio_data_arr[1] = (lnt_meas_time << 16) | lnt_sys_light >> 32;
    2624:	4bc0      	ldr	r3, [pc, #768]	; (2928 <main+0xab4>)
    2626:	4ac1      	ldr	r2, [pc, #772]	; (292c <main+0xab8>)
    2628:	681b      	ldr	r3, [r3, #0]
    262a:	6810      	ldr	r0, [r2, #0]
    262c:	6851      	ldr	r1, [r2, #4]
    262e:	041b      	lsls	r3, r3, #16
    2630:	430b      	orrs	r3, r1
    2632:	6073      	str	r3, [r6, #4]
                mrr_send_radio_data(1);
    2634:	0038      	movs	r0, r7
                radio_data_arr[0] = (uint32_t) lnt_sys_light;
    2636:	6853      	ldr	r3, [r2, #4]
    2638:	6812      	ldr	r2, [r2, #0]
    263a:	6032      	str	r2, [r6, #0]
                mrr_send_radio_data(1);
    263c:	f7ff f8c0 	bl	17c0 <mrr_send_radio_data>

        // some current event
        pmu_current_marker();
    }

    update_system_time();
    2640:	f7fe ffb4 	bl	15ac <update_system_time>

    if(projected_end_time_in_sec > xo_sys_time_in_sec) {
    2644:	4bb3      	ldr	r3, [pc, #716]	; (2914 <main+0xaa0>)
    2646:	6822      	ldr	r2, [r4, #0]
    2648:	681b      	ldr	r3, [r3, #0]
    264a:	429a      	cmp	r2, r3
    264c:	d801      	bhi.n	2652 <main+0x7de>
    264e:	f000 ffe3 	bl	3618 <main+0x17a4>
    int count = 0;
    2652:	2700      	movs	r7, #0
        mbus_remote_register_write(LNT_ADDR, 0x03, lntv1a_r03.as_int);
    2654:	2603      	movs	r6, #3
        uint32_t projected_end_time = projected_end_time_in_sec << XO_TO_SEC_SHIFT;
    2656:	6823      	ldr	r3, [r4, #0]
        count++;
    2658:	3701      	adds	r7, #1
        uint32_t projected_end_time = projected_end_time_in_sec << XO_TO_SEC_SHIFT;
    265a:	03dd      	lsls	r5, r3, #15
        update_system_time();
    265c:	f7fe ffa6 	bl	15ac <update_system_time>
        uint64_t temp = mult(projected_end_time - xo_sys_time, xo_lnt_mplier);
    2660:	4bb3      	ldr	r3, [pc, #716]	; (2930 <main+0xabc>)
    2662:	6818      	ldr	r0, [r3, #0]
    2664:	4bb3      	ldr	r3, [pc, #716]	; (2934 <main+0xac0>)
    2666:	1a28      	subs	r0, r5, r0
    2668:	8819      	ldrh	r1, [r3, #0]
    266a:	f7fe f8bd 	bl	7e8 <mult>
        lnt_meas_time = right_shift(temp, LNT_MPLIER_SHIFT + XO_TO_SEC_SHIFT - 2); // the -2 is empirical
    266e:	4bb2      	ldr	r3, [pc, #712]	; (2938 <main+0xac4>)
    2670:	881a      	ldrh	r2, [r3, #0]
    2672:	320d      	adds	r2, #13
    2674:	b252      	sxtb	r2, r2
    2676:	f7fd ffde 	bl	636 <right_shift>
    267a:	4bab      	ldr	r3, [pc, #684]	; (2928 <main+0xab4>)
        mbus_remote_register_write(LNT_ADDR, 0x03, lntv1a_r03.as_int);
    267c:	0031      	movs	r1, r6
        lnt_meas_time = right_shift(temp, LNT_MPLIER_SHIFT + XO_TO_SEC_SHIFT - 2); // the -2 is empirical
    267e:	6018      	str	r0, [r3, #0]
        lntv1a_r03.TIME_COUNTING = lnt_meas_time;
    2680:	4bae      	ldr	r3, [pc, #696]	; (293c <main+0xac8>)
    2682:	0200      	lsls	r0, r0, #8
    2684:	681a      	ldr	r2, [r3, #0]
    2686:	0a00      	lsrs	r0, r0, #8
    2688:	0e12      	lsrs	r2, r2, #24
    268a:	0612      	lsls	r2, r2, #24
    268c:	4310      	orrs	r0, r2
    268e:	6018      	str	r0, [r3, #0]
        mbus_remote_register_write(LNT_ADDR, 0x03, lntv1a_r03.as_int);
    2690:	681a      	ldr	r2, [r3, #0]
    2692:	0030      	movs	r0, r6
    2694:	f7fd fdb1 	bl	1fa <mbus_remote_register_write>
    lntv1a_r00.RESET_AFE = 0x0; // Defhttps://www.dropbox.com/s/yh15ux4h8141vu4/ISSCC2019-Digest.pdf?dl=0ault : 0x1
    2698:	2208      	movs	r2, #8
    269a:	4da9      	ldr	r5, [pc, #676]	; (2940 <main+0xacc>)
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    269c:	2100      	movs	r1, #0
    lntv1a_r00.RESET_AFE = 0x0; // Defhttps://www.dropbox.com/s/yh15ux4h8141vu4/ISSCC2019-Digest.pdf?dl=0ault : 0x1
    269e:	782b      	ldrb	r3, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    26a0:	0030      	movs	r0, r6
    lntv1a_r00.RESET_AFE = 0x0; // Defhttps://www.dropbox.com/s/yh15ux4h8141vu4/ISSCC2019-Digest.pdf?dl=0ault : 0x1
    26a2:	4393      	bics	r3, r2
    26a4:	702b      	strb	r3, [r5, #0]
    lntv1a_r00.RESETN_DBE = 0x1; // Default : 0x0
    26a6:	2304      	movs	r3, #4
    26a8:	782a      	ldrb	r2, [r5, #0]
    26aa:	4313      	orrs	r3, r2
    26ac:	702b      	strb	r3, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    26ae:	682a      	ldr	r2, [r5, #0]
    26b0:	f7fd fda3 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    26b4:	20fa      	movs	r0, #250	; 0xfa
    26b6:	0080      	lsls	r0, r0, #2
    26b8:	f7fd fcf6 	bl	a8 <delay>
    lntv1a_r00.DBE_ENABLE = 0x1; // Default : 0x0
    26bc:	2310      	movs	r3, #16
    26be:	782a      	ldrb	r2, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    26c0:	2100      	movs	r1, #0
    lntv1a_r00.DBE_ENABLE = 0x1; // Default : 0x0
    26c2:	4313      	orrs	r3, r2
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x0; // Default : 0x0
    26c4:	2240      	movs	r2, #64	; 0x40
    lntv1a_r00.DBE_ENABLE = 0x1; // Default : 0x0
    26c6:	702b      	strb	r3, [r5, #0]
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x0; // Default : 0x0
    26c8:	782b      	ldrb	r3, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    26ca:	0030      	movs	r0, r6
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x0; // Default : 0x0
    26cc:	4393      	bics	r3, r2
    26ce:	702b      	strb	r3, [r5, #0]
    lntv1a_r00.MODE_CONTINUOUS = 0x0; // Default : 0x0
    26d0:	782b      	ldrb	r3, [r5, #0]
    26d2:	3a20      	subs	r2, #32
    26d4:	4393      	bics	r3, r2
    26d6:	702b      	strb	r3, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    26d8:	682a      	ldr	r2, [r5, #0]
    26da:	f7fd fd8e 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    26de:	20fa      	movs	r0, #250	; 0xfa
    26e0:	0080      	lsls	r0, r0, #2
    26e2:	f7fd fce1 	bl	a8 <delay>
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x1; // Default : 0x0
    26e6:	2240      	movs	r2, #64	; 0x40
    26e8:	782b      	ldrb	r3, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    26ea:	2100      	movs	r1, #0
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x1; // Default : 0x0
    26ec:	4313      	orrs	r3, r2
    26ee:	702b      	strb	r3, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    26f0:	682a      	ldr	r2, [r5, #0]
    26f2:	0030      	movs	r0, r6
    26f4:	f7fd fd81 	bl	1fa <mbus_remote_register_write>
    delay(MBUS_DELAY*100);
    26f8:	4892      	ldr	r0, [pc, #584]	; (2944 <main+0xad0>)
    26fa:	f7fd fcd5 	bl	a8 <delay>
        set_halt_until_mbus_trx();
    26fe:	f7fd fd37 	bl	170 <set_halt_until_mbus_trx>
        mbus_copy_registers_from_remote_to_local(LNT_ADDR, 0x16, 0, 0);
    2702:	2300      	movs	r3, #0
    2704:	2116      	movs	r1, #22
    2706:	001a      	movs	r2, r3
    2708:	0030      	movs	r0, r6
    270a:	f7fd fd65 	bl	1d8 <mbus_copy_registers_from_remote_to_local>
        set_halt_until_mbus_tx();
    270e:	f7fd fd29 	bl	164 <set_halt_until_mbus_tx>
        lnt_state = *REG0;
    2712:	23a0      	movs	r3, #160	; 0xa0
    2714:	061b      	lsls	r3, r3, #24
    2716:	681d      	ldr	r5, [r3, #0]
        if(lnt_state == 0x1) {      // if LNT_COUNTER_STATE == STATE_COUNTING
    2718:	2d01      	cmp	r5, #1
    271a:	d101      	bne.n	2720 <main+0x8ac>
    271c:	f000 ff58 	bl	35d0 <main+0x175c>
        lnt_stop();
    2720:	f7fd ffa4 	bl	66c <lnt_stop>
    while(count < 5) {
    2724:	2f05      	cmp	r7, #5
    2726:	d196      	bne.n	2656 <main+0x7e2>
        set_system_error(0x5);
    2728:	2005      	movs	r0, #5
    272a:	f7fe ff2d 	bl	1588 <set_system_error>
    272e:	f000 ff53 	bl	35d8 <main+0x1764>
            projected_end_time_in_sec += N;
    2732:	6822      	ldr	r2, [r4, #0]
    2734:	0c1b      	lsrs	r3, r3, #16
    2736:	e74d      	b.n	25d4 <main+0x760>
    else if(goc_data_header == 0x04) {
    2738:	2a04      	cmp	r2, #4
    273a:	d11f      	bne.n	277c <main+0x908>
        xo_init();
    273c:	f7fd fefa 	bl	534 <xo_init>
        update_system_time();
    2740:	f7fe ff34 	bl	15ac <update_system_time>
        radio_data_arr[2] = (0x4 << 8) | CHIP_ID;
    2744:	2280      	movs	r2, #128	; 0x80
    2746:	4b74      	ldr	r3, [pc, #464]	; (2918 <main+0xaa4>)
    2748:	4d74      	ldr	r5, [pc, #464]	; (291c <main+0xaa8>)
    274a:	881b      	ldrh	r3, [r3, #0]
    274c:	00d2      	lsls	r2, r2, #3
    274e:	4313      	orrs	r3, r2
    2750:	60ab      	str	r3, [r5, #8]
        radio_data_arr[1] = 0;
    2752:	2300      	movs	r3, #0
        radio_data_arr[0] = xo_sys_time;
    2754:	4e76      	ldr	r6, [pc, #472]	; (2930 <main+0xabc>)
        radio_data_arr[1] = 0;
    2756:	606b      	str	r3, [r5, #4]
        radio_data_arr[0] = xo_sys_time;
    2758:	6833      	ldr	r3, [r6, #0]
    275a:	602b      	str	r3, [r5, #0]
        send_beacon();
    275c:	f7ff fb16 	bl	1d8c <send_beacon>
        update_system_time();
    2760:	f7fe ff24 	bl	15ac <update_system_time>
        radio_data_arr[2] = (0x4 << 8) | CHIP_ID;
    2764:	2280      	movs	r2, #128	; 0x80
    2766:	4b6c      	ldr	r3, [pc, #432]	; (2918 <main+0xaa4>)
    2768:	00d2      	lsls	r2, r2, #3
    276a:	881b      	ldrh	r3, [r3, #0]
    276c:	4313      	orrs	r3, r2
    276e:	60ab      	str	r3, [r5, #8]
        radio_data_arr[1] = 1;
    2770:	606f      	str	r7, [r5, #4]
        radio_data_arr[0] = xo_sys_time;
    2772:	6833      	ldr	r3, [r6, #0]
        radio_data_arr[0] = xo_day_time_in_sec;
    2774:	602b      	str	r3, [r5, #0]
        send_beacon();
    2776:	f7ff fb09 	bl	1d8c <send_beacon>
    277a:	e761      	b.n	2640 <main+0x7cc>
    else if(goc_data_header == 0x05) {
    277c:	2a05      	cmp	r2, #5
    277e:	d104      	bne.n	278a <main+0x916>
        uint16_t N = goc_data_full & 0xFFFF;
    2780:	681b      	ldr	r3, [r3, #0]
        projected_end_time_in_sec = xo_sys_time_in_sec + N;
    2782:	4a64      	ldr	r2, [pc, #400]	; (2914 <main+0xaa0>)
    2784:	b29b      	uxth	r3, r3
    2786:	6812      	ldr	r2, [r2, #0]
    2788:	e6b4      	b.n	24f4 <main+0x680>
    else if(goc_data_header == 0x06) {
    278a:	2a06      	cmp	r2, #6
    278c:	d100      	bne.n	2790 <main+0x91c>
    278e:	e757      	b.n	2640 <main+0x7cc>
    else if(goc_data_header == 0x07) {
    2790:	2a07      	cmp	r2, #7
    2792:	d000      	beq.n	2796 <main+0x922>
    2794:	e2d0      	b.n	2d38 <main+0xec4>
        if(goc_state == STATE_INIT) {
    2796:	4f5e      	ldr	r7, [pc, #376]	; (2910 <main+0xa9c>)
    2798:	883b      	ldrh	r3, [r7, #0]
    279a:	b29d      	uxth	r5, r3
    279c:	2b00      	cmp	r3, #0
    279e:	d130      	bne.n	2802 <main+0x98e>
            var_init();
    27a0:	f7fd fd66 	bl	270 <var_init>
            goc_state = STATE_VERIFY;
    27a4:	2305      	movs	r3, #5
    27a6:	803b      	strh	r3, [r7, #0]
            projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
    27a8:	4b5a      	ldr	r3, [pc, #360]	; (2914 <main+0xaa0>)
    27aa:	681b      	ldr	r3, [r3, #0]
    27ac:	3378      	adds	r3, #120	; 0x78
    27ae:	6023      	str	r3, [r4, #0]
            op_counter = 0;
    27b0:	4b65      	ldr	r3, [pc, #404]	; (2948 <main+0xad4>)
    27b2:	801d      	strh	r5, [r3, #0]
        if(projected_end_time_in_sec <= xo_sys_time_in_sec) {
    27b4:	4d57      	ldr	r5, [pc, #348]	; (2914 <main+0xaa0>)
    27b6:	6822      	ldr	r2, [r4, #0]
    27b8:	682b      	ldr	r3, [r5, #0]
    27ba:	429a      	cmp	r2, r3
    27bc:	d802      	bhi.n	27c4 <main+0x950>
            set_system_error(0x02);
    27be:	2002      	movs	r0, #2
    27c0:	f7fe fee2 	bl	1588 <set_system_error>
        if(error_code != 0) {
    27c4:	4961      	ldr	r1, [pc, #388]	; (294c <main+0xad8>)
    27c6:	4b52      	ldr	r3, [pc, #328]	; (2910 <main+0xa9c>)
    27c8:	680a      	ldr	r2, [r1, #0]
    27ca:	2a00      	cmp	r2, #0
    27cc:	d001      	beq.n	27d2 <main+0x95e>
            goc_state = STATE_ERROR;
    27ce:	220b      	movs	r2, #11
    27d0:	801a      	strh	r2, [r3, #0]
        if(goc_state == STATE_ERROR) {
    27d2:	881b      	ldrh	r3, [r3, #0]
    27d4:	2b0b      	cmp	r3, #11
    27d6:	d000      	beq.n	27da <main+0x966>
    27d8:	e732      	b.n	2640 <main+0x7cc>
            radio_data_arr[2] = 0xFF00 | CHIP_ID;
    27da:	4b4f      	ldr	r3, [pc, #316]	; (2918 <main+0xaa4>)
    27dc:	881a      	ldrh	r2, [r3, #0]
    27de:	23ff      	movs	r3, #255	; 0xff
    27e0:	021b      	lsls	r3, r3, #8
    27e2:	431a      	orrs	r2, r3
    27e4:	4b4d      	ldr	r3, [pc, #308]	; (291c <main+0xaa8>)
    27e6:	609a      	str	r2, [r3, #8]
            radio_data_arr[1] = error_code;
    27e8:	680a      	ldr	r2, [r1, #0]
    27ea:	605a      	str	r2, [r3, #4]
            radio_data_arr[0] = error_time;
    27ec:	4a58      	ldr	r2, [pc, #352]	; (2950 <main+0xadc>)
    27ee:	6812      	ldr	r2, [r2, #0]
    27f0:	601a      	str	r2, [r3, #0]
            send_beacon();
    27f2:	f7ff facb 	bl	1d8c <send_beacon>
            update_system_time();
    27f6:	f7fe fed9 	bl	15ac <update_system_time>
            projected_end_time_in_sec = xo_sys_time_in_sec + PMU_WAKEUP_INTERVAL;
    27fa:	4b56      	ldr	r3, [pc, #344]	; (2954 <main+0xae0>)
    27fc:	881b      	ldrh	r3, [r3, #0]
    27fe:	682a      	ldr	r2, [r5, #0]
    2800:	e678      	b.n	24f4 <main+0x680>
        else if(goc_state == STATE_VERIFY) {
    2802:	883b      	ldrh	r3, [r7, #0]
    2804:	2b05      	cmp	r3, #5
    2806:	d13c      	bne.n	2882 <main+0xa0e>
            op_counter++;
    2808:	494f      	ldr	r1, [pc, #316]	; (2948 <main+0xad4>)
            projected_end_time_in_sec += XO_2_MIN;
    280a:	6823      	ldr	r3, [r4, #0]
            op_counter++;
    280c:	880a      	ldrh	r2, [r1, #0]
            projected_end_time_in_sec += XO_2_MIN;
    280e:	3378      	adds	r3, #120	; 0x78
            op_counter++;
    2810:	3201      	adds	r2, #1
    2812:	b292      	uxth	r2, r2
    2814:	800a      	strh	r2, [r1, #0]
            if(op_counter <= 2) {
    2816:	880d      	ldrh	r5, [r1, #0]
    2818:	483f      	ldr	r0, [pc, #252]	; (2918 <main+0xaa4>)
    281a:	4a40      	ldr	r2, [pc, #256]	; (291c <main+0xaa8>)
            projected_end_time_in_sec += XO_2_MIN;
    281c:	6023      	str	r3, [r4, #0]
            if(op_counter <= 2) {
    281e:	2d02      	cmp	r5, #2
    2820:	d81d      	bhi.n	285e <main+0x9ea>
                radio_data_arr[2] = (0xAB << 8) | CHIP_ID;
    2822:	8803      	ldrh	r3, [r0, #0]
    2824:	20ab      	movs	r0, #171	; 0xab
    2826:	0200      	lsls	r0, r0, #8
    2828:	4303      	orrs	r3, r0
    282a:	6093      	str	r3, [r2, #8]
                radio_data_arr[1] = (op_counter << 28) | (lnt_sys_light & 0xFFFFFFF);
    282c:	880b      	ldrh	r3, [r1, #0]
    282e:	493f      	ldr	r1, [pc, #252]	; (292c <main+0xab8>)
    2830:	071b      	lsls	r3, r3, #28
    2832:	6808      	ldr	r0, [r1, #0]
    2834:	6849      	ldr	r1, [r1, #4]
    2836:	0100      	lsls	r0, r0, #4
    2838:	0900      	lsrs	r0, r0, #4
    283a:	4303      	orrs	r3, r0
    283c:	6053      	str	r3, [r2, #4]
                radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
    283e:	4b39      	ldr	r3, [pc, #228]	; (2924 <main+0xab0>)
    2840:	4937      	ldr	r1, [pc, #220]	; (2920 <main+0xaac>)
    2842:	881b      	ldrh	r3, [r3, #0]
    2844:	6809      	ldr	r1, [r1, #0]
    2846:	061b      	lsls	r3, r3, #24
                radio_data_arr[0] = (VERSION_NUM << 20) | (projected_end_time_in_sec & 0xFFFFF);
    2848:	430b      	orrs	r3, r1
    284a:	6013      	str	r3, [r2, #0]
            if(op_counter >= 4) {
    284c:	4b3e      	ldr	r3, [pc, #248]	; (2948 <main+0xad4>)
    284e:	881b      	ldrh	r3, [r3, #0]
    2850:	2b03      	cmp	r3, #3
    2852:	d901      	bls.n	2858 <main+0x9e4>
                goc_state = STATE_WAIT1;
    2854:	2306      	movs	r3, #6
    2856:	803b      	strh	r3, [r7, #0]
                        send_beacon();
    2858:	f7ff fa98 	bl	1d8c <send_beacon>
    285c:	e7aa      	b.n	27b4 <main+0x940>
                radio_data_arr[2] = (0xCD << 8) | CHIP_ID;
    285e:	25cd      	movs	r5, #205	; 0xcd
    2860:	8800      	ldrh	r0, [r0, #0]
    2862:	022d      	lsls	r5, r5, #8
    2864:	4328      	orrs	r0, r5
    2866:	6090      	str	r0, [r2, #8]
                radio_data_arr[1] = (op_counter << 28) | (xo_sys_time_in_sec & 0xFFFFFFF);
    2868:	482a      	ldr	r0, [pc, #168]	; (2914 <main+0xaa0>)
    286a:	8809      	ldrh	r1, [r1, #0]
    286c:	6800      	ldr	r0, [r0, #0]
    286e:	0709      	lsls	r1, r1, #28
    2870:	0100      	lsls	r0, r0, #4
    2872:	0900      	lsrs	r0, r0, #4
    2874:	4301      	orrs	r1, r0
    2876:	6051      	str	r1, [r2, #4]
                radio_data_arr[0] = (VERSION_NUM << 20) | (projected_end_time_in_sec & 0xFFFFF);
    2878:	21c6      	movs	r1, #198	; 0xc6
    287a:	031b      	lsls	r3, r3, #12
    287c:	0b1b      	lsrs	r3, r3, #12
    287e:	05c9      	lsls	r1, r1, #23
    2880:	e7e2      	b.n	2848 <main+0x9d4>
        else if(goc_state == STATE_WAIT1) {
    2882:	883b      	ldrh	r3, [r7, #0]
    2884:	2b06      	cmp	r3, #6
    2886:	d113      	bne.n	28b0 <main+0xa3c>
            if(day_count + epoch_days_offset >= start_day_count) {
    2888:	4a33      	ldr	r2, [pc, #204]	; (2958 <main+0xae4>)
    288a:	4b34      	ldr	r3, [pc, #208]	; (295c <main+0xae8>)
    288c:	881b      	ldrh	r3, [r3, #0]
    288e:	8811      	ldrh	r1, [r2, #0]
    2890:	4a33      	ldr	r2, [pc, #204]	; (2960 <main+0xaec>)
    2892:	185b      	adds	r3, r3, r1
    2894:	6812      	ldr	r2, [r2, #0]
    2896:	4293      	cmp	r3, r2
    2898:	d305      	bcc.n	28a6 <main+0xa32>
                initialize_state_collect();
    289a:	f7ff fa85 	bl	1da8 <initialize_state_collect>
                op_counter = 0;
    289e:	2200      	movs	r2, #0
    28a0:	4b29      	ldr	r3, [pc, #164]	; (2948 <main+0xad4>)
    28a2:	801a      	strh	r2, [r3, #0]
    28a4:	e786      	b.n	27b4 <main+0x940>
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
    28a6:	4b2b      	ldr	r3, [pc, #172]	; (2954 <main+0xae0>)
    28a8:	881b      	ldrh	r3, [r3, #0]
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
    28aa:	6822      	ldr	r2, [r4, #0]
    28ac:	18d3      	adds	r3, r2, r3
    28ae:	e163      	b.n	2b78 <main+0xd04>
        else if(goc_state == STATE_COLLECT) {
    28b0:	883b      	ldrh	r3, [r7, #0]
    28b2:	2b01      	cmp	r3, #1
    28b4:	d000      	beq.n	28b8 <main+0xa44>
    28b6:	e0b3      	b.n	2a20 <main+0xbac>
            if(set_low_pwr_low_trigger()) {
    28b8:	f7ff fa28 	bl	1d0c <set_low_pwr_low_trigger>
    28bc:	4a29      	ldr	r2, [pc, #164]	; (2964 <main+0xaf0>)
    28be:	2800      	cmp	r0, #0
    28c0:	d023      	beq.n	290a <main+0xa96>
                low_pwr_count++;
    28c2:	8813      	ldrh	r3, [r2, #0]
    28c4:	3301      	adds	r3, #1
    28c6:	b29b      	uxth	r3, r3
    28c8:	8013      	strh	r3, [r2, #0]
            if(day_count + epoch_days_offset >= end_day_count && xo_day_time_in_sec >= data_collection_end_day_time) {
    28ca:	4823      	ldr	r0, [pc, #140]	; (2958 <main+0xae4>)
    28cc:	4923      	ldr	r1, [pc, #140]	; (295c <main+0xae8>)
    28ce:	880b      	ldrh	r3, [r1, #0]
    28d0:	8805      	ldrh	r5, [r0, #0]
    28d2:	4825      	ldr	r0, [pc, #148]	; (2968 <main+0xaf4>)
    28d4:	195b      	adds	r3, r3, r5
    28d6:	6800      	ldr	r0, [r0, #0]
    28d8:	4283      	cmp	r3, r0
    28da:	d34f      	bcc.n	297c <main+0xb08>
    28dc:	4b23      	ldr	r3, [pc, #140]	; (296c <main+0xaf8>)
    28de:	6818      	ldr	r0, [r3, #0]
    28e0:	4b23      	ldr	r3, [pc, #140]	; (2970 <main+0xafc>)
    28e2:	681b      	ldr	r3, [r3, #0]
    28e4:	4298      	cmp	r0, r3
    28e6:	d349      	bcc.n	297c <main+0xb08>
                flush_temp_cache();       // store everything in temp_code_storage
    28e8:	f7fe fcca 	bl	1280 <flush_temp_cache>
                flush_code_cache();
    28ec:	f7fe f84a 	bl	984 <flush_code_cache>
                update_system_time();
    28f0:	f7fe fe5c 	bl	15ac <update_system_time>
                goc_state = STATE_WAIT2;
    28f4:	2307      	movs	r3, #7
    28f6:	803b      	strh	r3, [r7, #0]
                projected_end_time_in_sec += XO_2_MIN;
    28f8:	6823      	ldr	r3, [r4, #0]
                next_beacon_time = 0;
    28fa:	4a1e      	ldr	r2, [pc, #120]	; (2974 <main+0xb00>)
                projected_end_time_in_sec += XO_2_MIN;
    28fc:	3378      	adds	r3, #120	; 0x78
    28fe:	6023      	str	r3, [r4, #0]
                next_beacon_time = 0;
    2900:	2300      	movs	r3, #0
    2902:	6013      	str	r3, [r2, #0]
                next_data_time = 0;
    2904:	4a1c      	ldr	r2, [pc, #112]	; (2978 <main+0xb04>)
    2906:	6013      	str	r3, [r2, #0]
    2908:	e754      	b.n	27b4 <main+0x940>
                low_pwr_count = 0;
    290a:	8010      	strh	r0, [r2, #0]
    290c:	e7dd      	b.n	28ca <main+0xa56>
    290e:	46c0      	nop			; (mov r8, r8)
    2910:	000039ec 	.word	0x000039ec
    2914:	00003aec 	.word	0x00003aec
    2918:	00003a00 	.word	0x00003a00
    291c:	000039b8 	.word	0x000039b8
    2920:	00003938 	.word	0x00003938
    2924:	000039b4 	.word	0x000039b4
    2928:	00003a40 	.word	0x00003a40
    292c:	00003a48 	.word	0x00003a48
    2930:	00003ae8 	.word	0x00003ae8
    2934:	0000394e 	.word	0x0000394e
    2938:	000037cc 	.word	0x000037cc
    293c:	00003900 	.word	0x00003900
    2940:	000038fc 	.word	0x000038fc
    2944:	00002710 	.word	0x00002710
    2948:	000039fc 	.word	0x000039fc
    294c:	00003a24 	.word	0x00003a24
    2950:	00003a28 	.word	0x00003a28
    2954:	000038c4 	.word	0x000038c4
    2958:	00003a20 	.word	0x00003a20
    295c:	00003a18 	.word	0x00003a18
    2960:	00003ad0 	.word	0x00003ad0
    2964:	00003a50 	.word	0x00003a50
    2968:	00003a1c 	.word	0x00003a1c
    296c:	00003ae4 	.word	0x00003ae4
    2970:	000038f4 	.word	0x000038f4
    2974:	00003a68 	.word	0x00003a68
    2978:	00003a6c 	.word	0x00003a6c
            else if(low_pwr_count >= 3) {
    297c:	8813      	ldrh	r3, [r2, #0]
    297e:	6820      	ldr	r0, [r4, #0]
    2980:	2b02      	cmp	r3, #2
    2982:	d90c      	bls.n	299e <main+0xb2a>
                low_pwr_count = 0;
    2984:	2300      	movs	r3, #0
    2986:	8013      	strh	r3, [r2, #0]
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
    2988:	4bc1      	ldr	r3, [pc, #772]	; (2c90 <main+0xe1c>)
                low_pwr_state_end_day = day_count + 1;
    298a:	4ac2      	ldr	r2, [pc, #776]	; (2c94 <main+0xe20>)
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
    298c:	881b      	ldrh	r3, [r3, #0]
    298e:	181b      	adds	r3, r3, r0
    2990:	6023      	str	r3, [r4, #0]
                low_pwr_state_end_day = day_count + 1;
    2992:	880b      	ldrh	r3, [r1, #0]
    2994:	3301      	adds	r3, #1
    2996:	6013      	str	r3, [r2, #0]
                goc_state = STATE_LOW_PWR;
    2998:	230a      	movs	r3, #10
                goc_state = STATE_RADIO_DATA;
    299a:	803b      	strh	r3, [r7, #0]
    299c:	e70a      	b.n	27b4 <main+0x940>
                if(projected_end_time_in_sec >= store_temp_timestamp) {
    299e:	4bbe      	ldr	r3, [pc, #760]	; (2c98 <main+0xe24>)
    29a0:	681a      	ldr	r2, [r3, #0]
    29a2:	4282      	cmp	r2, r0
    29a4:	d806      	bhi.n	29b4 <main+0xb40>
                    store_temp_timestamp += XO_30_MIN;    // increment by 30 minutes
    29a6:	21e1      	movs	r1, #225	; 0xe1
    29a8:	681a      	ldr	r2, [r3, #0]
    29aa:	00c9      	lsls	r1, r1, #3
    29ac:	1852      	adds	r2, r2, r1
    29ae:	601a      	str	r2, [r3, #0]
                    sample_temp();
    29b0:	f7fe fca6 	bl	1300 <sample_temp>
                if(projected_end_time_in_sec == next_light_meas_time) {
    29b4:	4bb9      	ldr	r3, [pc, #740]	; (2c9c <main+0xe28>)
    29b6:	6822      	ldr	r2, [r4, #0]
    29b8:	681b      	ldr	r3, [r3, #0]
    29ba:	429a      	cmp	r2, r3
    29bc:	d101      	bne.n	29c2 <main+0xb4e>
                    sample_light();
    29be:	f7fe fa1d 	bl	dfc <sample_light>
                set_projected_end_time();
    29c2:	f7fe fcf7 	bl	13b4 <set_projected_end_time>
                if((op_counter == 0) || (radio_debug && projected_end_time_in_sec >= next_radio_debug_time)) {
    29c6:	4bb6      	ldr	r3, [pc, #728]	; (2ca0 <main+0xe2c>)
    29c8:	881a      	ldrh	r2, [r3, #0]
    29ca:	2a00      	cmp	r2, #0
    29cc:	d00a      	beq.n	29e4 <main+0xb70>
    29ce:	4ab5      	ldr	r2, [pc, #724]	; (2ca4 <main+0xe30>)
    29d0:	7812      	ldrb	r2, [r2, #0]
    29d2:	2a00      	cmp	r2, #0
    29d4:	d100      	bne.n	29d8 <main+0xb64>
    29d6:	e6ed      	b.n	27b4 <main+0x940>
    29d8:	4ab3      	ldr	r2, [pc, #716]	; (2ca8 <main+0xe34>)
    29da:	6821      	ldr	r1, [r4, #0]
    29dc:	6812      	ldr	r2, [r2, #0]
    29de:	428a      	cmp	r2, r1
    29e0:	d900      	bls.n	29e4 <main+0xb70>
    29e2:	e6e7      	b.n	27b4 <main+0x940>
                    op_counter = 1;
    29e4:	2201      	movs	r2, #1
                    next_radio_debug_time = xo_sys_time_in_sec + RADIO_DEBUG_PERIOD;
    29e6:	49b1      	ldr	r1, [pc, #708]	; (2cac <main+0xe38>)
                    op_counter = 1;
    29e8:	801a      	strh	r2, [r3, #0]
                    next_radio_debug_time = xo_sys_time_in_sec + RADIO_DEBUG_PERIOD;
    29ea:	4ab1      	ldr	r2, [pc, #708]	; (2cb0 <main+0xe3c>)
    29ec:	680b      	ldr	r3, [r1, #0]
    29ee:	6812      	ldr	r2, [r2, #0]
    29f0:	189b      	adds	r3, r3, r2
    29f2:	4aad      	ldr	r2, [pc, #692]	; (2ca8 <main+0xe34>)
    29f4:	6013      	str	r3, [r2, #0]
                    if(mrr_send_enable) {
    29f6:	8833      	ldrh	r3, [r6, #0]
    29f8:	2b00      	cmp	r3, #0
    29fa:	d100      	bne.n	29fe <main+0xb8a>
    29fc:	e6da      	b.n	27b4 <main+0x940>
                        radio_data_arr[2] = (0xDE << 8) | CHIP_ID;
    29fe:	22de      	movs	r2, #222	; 0xde
    2a00:	4bac      	ldr	r3, [pc, #688]	; (2cb4 <main+0xe40>)
    2a02:	0212      	lsls	r2, r2, #8
    2a04:	881b      	ldrh	r3, [r3, #0]
    2a06:	4313      	orrs	r3, r2
    2a08:	4aab      	ldr	r2, [pc, #684]	; (2cb8 <main+0xe44>)
    2a0a:	6093      	str	r3, [r2, #8]
                        radio_data_arr[1] = xo_sys_time_in_sec;
    2a0c:	680b      	ldr	r3, [r1, #0]
                        radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
    2a0e:	49ab      	ldr	r1, [pc, #684]	; (2cbc <main+0xe48>)
                        radio_data_arr[1] = xo_sys_time_in_sec;
    2a10:	6053      	str	r3, [r2, #4]
                        radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
    2a12:	4bab      	ldr	r3, [pc, #684]	; (2cc0 <main+0xe4c>)
    2a14:	881b      	ldrh	r3, [r3, #0]
    2a16:	6809      	ldr	r1, [r1, #0]
    2a18:	061b      	lsls	r3, r3, #24
    2a1a:	430b      	orrs	r3, r1
    2a1c:	6013      	str	r3, [r2, #0]
    2a1e:	e71b      	b.n	2858 <main+0x9e4>
        else if(goc_state == STATE_WAIT2) {
    2a20:	883b      	ldrh	r3, [r7, #0]
    2a22:	2b07      	cmp	r3, #7
    2a24:	d000      	beq.n	2a28 <main+0xbb4>
    2a26:	e0a9      	b.n	2b7c <main+0xd08>
            if(projected_end_time_in_sec == next_beacon_time && mrr_send_enable) {
    2a28:	4ba6      	ldr	r3, [pc, #664]	; (2cc4 <main+0xe50>)
    2a2a:	6822      	ldr	r2, [r4, #0]
    2a2c:	681b      	ldr	r3, [r3, #0]
    2a2e:	429a      	cmp	r2, r3
    2a30:	d120      	bne.n	2a74 <main+0xc00>
    2a32:	8833      	ldrh	r3, [r6, #0]
    2a34:	2b00      	cmp	r3, #0
    2a36:	d01d      	beq.n	2a74 <main+0xc00>
                radio_data_arr[2] = (0xEF << 8) | CHIP_ID;
    2a38:	22ef      	movs	r2, #239	; 0xef
    2a3a:	4b9e      	ldr	r3, [pc, #632]	; (2cb4 <main+0xe40>)
    2a3c:	0212      	lsls	r2, r2, #8
    2a3e:	881b      	ldrh	r3, [r3, #0]
                radio_data_arr[1] = (radio_beacon_counter << 28) | (max_unit_count << 20) | xo_day_time_in_sec;
    2a40:	4da1      	ldr	r5, [pc, #644]	; (2cc8 <main+0xe54>)
                radio_data_arr[2] = (0xEF << 8) | CHIP_ID;
    2a42:	4313      	orrs	r3, r2
    2a44:	4a9c      	ldr	r2, [pc, #624]	; (2cb8 <main+0xe44>)
                radio_data_arr[1] = (radio_beacon_counter << 28) | (max_unit_count << 20) | xo_day_time_in_sec;
    2a46:	49a1      	ldr	r1, [pc, #644]	; (2ccc <main+0xe58>)
                radio_data_arr[2] = (0xEF << 8) | CHIP_ID;
    2a48:	6093      	str	r3, [r2, #8]
                radio_data_arr[1] = (radio_beacon_counter << 28) | (max_unit_count << 20) | xo_day_time_in_sec;
    2a4a:	4ba1      	ldr	r3, [pc, #644]	; (2cd0 <main+0xe5c>)
    2a4c:	8809      	ldrh	r1, [r1, #0]
    2a4e:	6818      	ldr	r0, [r3, #0]
    2a50:	882b      	ldrh	r3, [r5, #0]
    2a52:	0509      	lsls	r1, r1, #20
    2a54:	071b      	lsls	r3, r3, #28
    2a56:	430b      	orrs	r3, r1
    2a58:	4303      	orrs	r3, r0
    2a5a:	6053      	str	r3, [r2, #4]
                radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
    2a5c:	4b98      	ldr	r3, [pc, #608]	; (2cc0 <main+0xe4c>)
    2a5e:	4997      	ldr	r1, [pc, #604]	; (2cbc <main+0xe48>)
    2a60:	881b      	ldrh	r3, [r3, #0]
    2a62:	6809      	ldr	r1, [r1, #0]
    2a64:	061b      	lsls	r3, r3, #24
    2a66:	430b      	orrs	r3, r1
    2a68:	6013      	str	r3, [r2, #0]
                send_beacon();
    2a6a:	f7ff f98f 	bl	1d8c <send_beacon>
                radio_beacon_counter++;
    2a6e:	882b      	ldrh	r3, [r5, #0]
    2a70:	3301      	adds	r3, #1
    2a72:	802b      	strh	r3, [r5, #0]
            uint16_t epoch_day_count = day_count + epoch_days_offset;
    2a74:	4b97      	ldr	r3, [pc, #604]	; (2cd4 <main+0xe60>)
            if(epoch_day_count >= radio_day_count) {
    2a76:	4a98      	ldr	r2, [pc, #608]	; (2cd8 <main+0xe64>)
            uint16_t epoch_day_count = day_count + epoch_days_offset;
    2a78:	881e      	ldrh	r6, [r3, #0]
    2a7a:	4b98      	ldr	r3, [pc, #608]	; (2cdc <main+0xe68>)
    2a7c:	881b      	ldrh	r3, [r3, #0]
    2a7e:	18f6      	adds	r6, r6, r3
            if(epoch_day_count >= radio_day_count) {
    2a80:	6813      	ldr	r3, [r2, #0]
            uint16_t epoch_day_count = day_count + epoch_days_offset;
    2a82:	b2b6      	uxth	r6, r6
            if(epoch_day_count >= radio_day_count) {
    2a84:	429e      	cmp	r6, r3
    2a86:	d30f      	bcc.n	2aa8 <main+0xc34>
                radio_duty_cycle_end_day = epoch_day_count + RADIO_DUTY_DURATION;
    2a88:	4b95      	ldr	r3, [pc, #596]	; (2ce0 <main+0xe6c>)
    2a8a:	4996      	ldr	r1, [pc, #600]	; (2ce4 <main+0xe70>)
    2a8c:	881b      	ldrh	r3, [r3, #0]
    2a8e:	199b      	adds	r3, r3, r6
    2a90:	b29b      	uxth	r3, r3
    2a92:	800b      	strh	r3, [r1, #0]
                radio_initial_data_start_day = epoch_day_count + RADIO_INITIAL_DATA_DAY;
    2a94:	4b94      	ldr	r3, [pc, #592]	; (2ce8 <main+0xe74>)
    2a96:	4995      	ldr	r1, [pc, #596]	; (2cec <main+0xe78>)
    2a98:	881b      	ldrh	r3, [r3, #0]
    2a9a:	199b      	adds	r3, r3, r6
    2a9c:	b29b      	uxth	r3, r3
    2a9e:	800b      	strh	r3, [r1, #0]
                radio_day_count = epoch_day_count + RADIO_SEQUENCE_PERIOD;
    2aa0:	4b93      	ldr	r3, [pc, #588]	; (2cf0 <main+0xe7c>)
    2aa2:	881b      	ldrh	r3, [r3, #0]
    2aa4:	18f3      	adds	r3, r6, r3
    2aa6:	6013      	str	r3, [r2, #0]
            if(xo_sys_time_in_sec + XO_2_MIN >= next_beacon_time && xo_check_is_day()) {
    2aa8:	4d80      	ldr	r5, [pc, #512]	; (2cac <main+0xe38>)
    2aaa:	4a86      	ldr	r2, [pc, #536]	; (2cc4 <main+0xe50>)
    2aac:	682b      	ldr	r3, [r5, #0]
    2aae:	6812      	ldr	r2, [r2, #0]
    2ab0:	3378      	adds	r3, #120	; 0x78
    2ab2:	4293      	cmp	r3, r2
    2ab4:	d311      	bcc.n	2ada <main+0xc66>
    2ab6:	f7fe fdbd 	bl	1634 <xo_check_is_day>
    2aba:	2800      	cmp	r0, #0
    2abc:	d00d      	beq.n	2ada <main+0xc66>
                uint32_t next_beacon_day_time = RADIO_INITIAL_BEACON_TIME;
    2abe:	4b8d      	ldr	r3, [pc, #564]	; (2cf4 <main+0xe80>)
                    next_beacon_day_time += RADIO_BEACON_PERIOD;
    2ac0:	488d      	ldr	r0, [pc, #564]	; (2cf8 <main+0xe84>)
                uint32_t next_beacon_day_time = RADIO_INITIAL_BEACON_TIME;
    2ac2:	6819      	ldr	r1, [r3, #0]
                while(xo_day_time_in_sec + XO_2_MIN >= next_beacon_day_time) {
    2ac4:	4b82      	ldr	r3, [pc, #520]	; (2cd0 <main+0xe5c>)
    2ac6:	681a      	ldr	r2, [r3, #0]
    2ac8:	3278      	adds	r2, #120	; 0x78
    2aca:	428a      	cmp	r2, r1
    2acc:	d246      	bcs.n	2b5c <main+0xce8>
                next_beacon_time = next_beacon_day_time + xo_sys_time_in_sec - xo_day_time_in_sec;
    2ace:	682a      	ldr	r2, [r5, #0]
    2ad0:	681b      	ldr	r3, [r3, #0]
    2ad2:	1ad3      	subs	r3, r2, r3
    2ad4:	4a7b      	ldr	r2, [pc, #492]	; (2cc4 <main+0xe50>)
    2ad6:	185b      	adds	r3, r3, r1
    2ad8:	6013      	str	r3, [r2, #0]
            if(xo_sys_time_in_sec + XO_2_MIN >= next_data_time && xo_check_is_day()) {
    2ada:	4a88      	ldr	r2, [pc, #544]	; (2cfc <main+0xe88>)
    2adc:	682b      	ldr	r3, [r5, #0]
    2ade:	6812      	ldr	r2, [r2, #0]
    2ae0:	3378      	adds	r3, #120	; 0x78
    2ae2:	4293      	cmp	r3, r2
    2ae4:	d312      	bcc.n	2b0c <main+0xc98>
    2ae6:	f7fe fda5 	bl	1634 <xo_check_is_day>
    2aea:	2800      	cmp	r0, #0
    2aec:	d00e      	beq.n	2b0c <main+0xc98>
                uint32_t next_data_day_time = RADIO_INITIAL_DATA_TIME;
    2aee:	4b84      	ldr	r3, [pc, #528]	; (2d00 <main+0xe8c>)
                while(xo_day_time_in_sec + XO_2_MIN >= next_data_day_time) {
    2af0:	4877      	ldr	r0, [pc, #476]	; (2cd0 <main+0xe5c>)
                uint32_t next_data_day_time = RADIO_INITIAL_DATA_TIME;
    2af2:	8819      	ldrh	r1, [r3, #0]
                    next_data_day_time += RADIO_DATA_PERIOD;
    2af4:	4a83      	ldr	r2, [pc, #524]	; (2d04 <main+0xe90>)
                uint32_t next_data_day_time = RADIO_INITIAL_DATA_TIME;
    2af6:	b289      	uxth	r1, r1
                while(xo_day_time_in_sec + XO_2_MIN >= next_data_day_time) {
    2af8:	6803      	ldr	r3, [r0, #0]
    2afa:	3378      	adds	r3, #120	; 0x78
    2afc:	428b      	cmp	r3, r1
    2afe:	d230      	bcs.n	2b62 <main+0xcee>
                next_data_time = next_data_day_time + xo_sys_time_in_sec - xo_day_time_in_sec;
    2b00:	682a      	ldr	r2, [r5, #0]
    2b02:	6803      	ldr	r3, [r0, #0]
    2b04:	1ad3      	subs	r3, r2, r3
    2b06:	4a7d      	ldr	r2, [pc, #500]	; (2cfc <main+0xe88>)
    2b08:	185b      	adds	r3, r3, r1
    2b0a:	6013      	str	r3, [r2, #0]
            bool common_flag = (epoch_day_count < radio_duty_cycle_end_day
    2b0c:	4b75      	ldr	r3, [pc, #468]	; (2ce4 <main+0xe70>)
                                && xo_check_is_day());
    2b0e:	2000      	movs	r0, #0
            bool common_flag = (epoch_day_count < radio_duty_cycle_end_day
    2b10:	881b      	ldrh	r3, [r3, #0]
                                && xo_check_is_day());
    2b12:	42b3      	cmp	r3, r6
    2b14:	d901      	bls.n	2b1a <main+0xca6>
    2b16:	f7fe fd8d 	bl	1634 <xo_check_is_day>
            uint32_t default_projected_end_time = projected_end_time_in_sec + PMU_WAKEUP_INTERVAL + XO_2_MIN;
    2b1a:	6821      	ldr	r1, [r4, #0]
    2b1c:	4b5c      	ldr	r3, [pc, #368]	; (2c90 <main+0xe1c>)
    2b1e:	000d      	movs	r5, r1
    2b20:	881a      	ldrh	r2, [r3, #0]
    2b22:	3578      	adds	r5, #120	; 0x78
    2b24:	18aa      	adds	r2, r5, r2
            if(epoch_day_count >= radio_initial_data_start_day
    2b26:	4d71      	ldr	r5, [pc, #452]	; (2cec <main+0xe78>)
    2b28:	882d      	ldrh	r5, [r5, #0]
    2b2a:	42b5      	cmp	r5, r6
    2b2c:	d81c      	bhi.n	2b68 <main+0xcf4>
                    && common_flag
    2b2e:	2800      	cmp	r0, #0
    2b30:	d100      	bne.n	2b34 <main+0xcc0>
    2b32:	e088      	b.n	2c46 <main+0xdd2>
                    && next_data_time <= default_projected_end_time) {
    2b34:	4871      	ldr	r0, [pc, #452]	; (2cfc <main+0xe88>)
    2b36:	6805      	ldr	r5, [r0, #0]
    2b38:	4295      	cmp	r5, r2
    2b3a:	d817      	bhi.n	2b6c <main+0xcf8>
                projected_end_time_in_sec = next_data_time;
    2b3c:	6803      	ldr	r3, [r0, #0]
                radio_unit_counter = 0;
    2b3e:	4a72      	ldr	r2, [pc, #456]	; (2d08 <main+0xe94>)
                projected_end_time_in_sec = next_data_time;
    2b40:	6023      	str	r3, [r4, #0]
                goc_state = STATE_RADIO_DATA;
    2b42:	2308      	movs	r3, #8
    2b44:	803b      	strh	r3, [r7, #0]
                radio_unit_counter = 0;
    2b46:	2300      	movs	r3, #0
    2b48:	8013      	strh	r3, [r2, #0]
                check_pmu_counter = 0;
    2b4a:	4a70      	ldr	r2, [pc, #448]	; (2d0c <main+0xe98>)
    2b4c:	8013      	strh	r3, [r2, #0]
                radio_rest_counter = 0;
    2b4e:	4a70      	ldr	r2, [pc, #448]	; (2d10 <main+0xe9c>)
    2b50:	8013      	strh	r3, [r2, #0]
                light_packet_num = 0;
    2b52:	4a70      	ldr	r2, [pc, #448]	; (2d14 <main+0xea0>)
    2b54:	8013      	strh	r3, [r2, #0]
                temp_packet_num = 0;
    2b56:	4a70      	ldr	r2, [pc, #448]	; (2d18 <main+0xea4>)
    2b58:	8013      	strh	r3, [r2, #0]
    2b5a:	e62b      	b.n	27b4 <main+0x940>
                    next_beacon_day_time += RADIO_BEACON_PERIOD;
    2b5c:	6802      	ldr	r2, [r0, #0]
    2b5e:	1889      	adds	r1, r1, r2
    2b60:	e7b1      	b.n	2ac6 <main+0xc52>
                    next_data_day_time += RADIO_DATA_PERIOD;
    2b62:	8813      	ldrh	r3, [r2, #0]
    2b64:	18c9      	adds	r1, r1, r3
    2b66:	e7c7      	b.n	2af8 <main+0xc84>
            else if(common_flag
    2b68:	2800      	cmp	r0, #0
    2b6a:	d06c      	beq.n	2c46 <main+0xdd2>
                    && next_beacon_time <= default_projected_end_time) {
    2b6c:	4855      	ldr	r0, [pc, #340]	; (2cc4 <main+0xe50>)
    2b6e:	6800      	ldr	r0, [r0, #0]
    2b70:	4290      	cmp	r0, r2
    2b72:	d868      	bhi.n	2c46 <main+0xdd2>
                projected_end_time_in_sec = next_beacon_time;
    2b74:	4b53      	ldr	r3, [pc, #332]	; (2cc4 <main+0xe50>)
    2b76:	681b      	ldr	r3, [r3, #0]
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
    2b78:	6023      	str	r3, [r4, #0]
    2b7a:	e61b      	b.n	27b4 <main+0x940>
        else if(goc_state == STATE_RADIO_DATA) {
    2b7c:	883b      	ldrh	r3, [r7, #0]
    2b7e:	2b08      	cmp	r3, #8
    2b80:	d150      	bne.n	2c24 <main+0xdb0>
            if(mrr_send_enable) {
    2b82:	8833      	ldrh	r3, [r6, #0]
    2b84:	2b00      	cmp	r3, #0
    2b86:	d040      	beq.n	2c0a <main+0xd96>
                    if(radio_unit_counter >= max_unit_count) {
    2b88:	4b5f      	ldr	r3, [pc, #380]	; (2d08 <main+0xe94>)
    2b8a:	8818      	ldrh	r0, [r3, #0]
    2b8c:	4b4f      	ldr	r3, [pc, #316]	; (2ccc <main+0xe58>)
    2b8e:	881b      	ldrh	r3, [r3, #0]
    2b90:	4283      	cmp	r3, r0
    2b92:	d807      	bhi.n	2ba4 <main+0xd30>
                        update_system_time();
    2b94:	f7fe fd0a 	bl	15ac <update_system_time>
                        projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
    2b98:	4b44      	ldr	r3, [pc, #272]	; (2cac <main+0xe38>)
    2b9a:	681b      	ldr	r3, [r3, #0]
    2b9c:	3378      	adds	r3, #120	; 0x78
    2b9e:	6023      	str	r3, [r4, #0]
                    goc_state = STATE_WAIT2;
    2ba0:	2307      	movs	r3, #7
    2ba2:	e6fa      	b.n	299a <main+0xb26>
                    else if(radio_rest_counter >= RADIO_REST_NUM_UNITS) {
    2ba4:	4b5d      	ldr	r3, [pc, #372]	; (2d1c <main+0xea8>)
    2ba6:	4e5a      	ldr	r6, [pc, #360]	; (2d10 <main+0xe9c>)
    2ba8:	881b      	ldrh	r3, [r3, #0]
    2baa:	8832      	ldrh	r2, [r6, #0]
    2bac:	4d57      	ldr	r5, [pc, #348]	; (2d0c <main+0xe98>)
    2bae:	429a      	cmp	r2, r3
    2bb0:	d310      	bcc.n	2bd4 <main+0xd60>
                        radio_rest_counter = 0;
    2bb2:	2300      	movs	r3, #0
    2bb4:	8033      	strh	r3, [r6, #0]
                        check_pmu_counter = 0;  // Resetting check_pmu counter as well to avoid concatenating the sleep periods
    2bb6:	802b      	strh	r3, [r5, #0]
                        update_system_time();
    2bb8:	f7fe fcf8 	bl	15ac <update_system_time>
                        radio_rest_end_time = xo_sys_time_in_sec + RADIO_REST_TIME;
    2bbc:	4a3b      	ldr	r2, [pc, #236]	; (2cac <main+0xe38>)
    2bbe:	4b58      	ldr	r3, [pc, #352]	; (2d20 <main+0xeac>)
    2bc0:	881b      	ldrh	r3, [r3, #0]
    2bc2:	6811      	ldr	r1, [r2, #0]
    2bc4:	185b      	adds	r3, r3, r1
    2bc6:	4957      	ldr	r1, [pc, #348]	; (2d24 <main+0xeb0>)
    2bc8:	600b      	str	r3, [r1, #0]
                        projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
    2bca:	6813      	ldr	r3, [r2, #0]
    2bcc:	3378      	adds	r3, #120	; 0x78
    2bce:	6023      	str	r3, [r4, #0]
                        goc_state = STATE_RADIO_REST;
    2bd0:	2309      	movs	r3, #9
    2bd2:	e6e2      	b.n	299a <main+0xb26>
                    else if(check_pmu_counter >= CHECK_PMU_NUM_UNITS) {
    2bd4:	4b54      	ldr	r3, [pc, #336]	; (2d28 <main+0xeb4>)
    2bd6:	882a      	ldrh	r2, [r5, #0]
    2bd8:	881b      	ldrh	r3, [r3, #0]
    2bda:	429a      	cmp	r2, r3
    2bdc:	d307      	bcc.n	2bee <main+0xd7a>
                        check_pmu_counter = 0;
    2bde:	2300      	movs	r3, #0
    2be0:	802b      	strh	r3, [r5, #0]
                        update_system_time();
    2be2:	f7fe fce3 	bl	15ac <update_system_time>
                        projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
    2be6:	4b31      	ldr	r3, [pc, #196]	; (2cac <main+0xe38>)
    2be8:	681b      	ldr	r3, [r3, #0]
    2bea:	3378      	adds	r3, #120	; 0x78
    2bec:	e7c4      	b.n	2b78 <main+0xd04>
                    radio_unit(radio_unit_counter);
    2bee:	f7fe ffeb 	bl	1bc8 <radio_unit>
                    radio_unit_counter++;
    2bf2:	4b45      	ldr	r3, [pc, #276]	; (2d08 <main+0xe94>)
    2bf4:	4a44      	ldr	r2, [pc, #272]	; (2d08 <main+0xe94>)
    2bf6:	881b      	ldrh	r3, [r3, #0]
    2bf8:	3301      	adds	r3, #1
    2bfa:	8013      	strh	r3, [r2, #0]
                    check_pmu_counter++;
    2bfc:	882b      	ldrh	r3, [r5, #0]
    2bfe:	3301      	adds	r3, #1
    2c00:	802b      	strh	r3, [r5, #0]
                    radio_rest_counter++;
    2c02:	8833      	ldrh	r3, [r6, #0]
    2c04:	3301      	adds	r3, #1
    2c06:	8033      	strh	r3, [r6, #0]
                    if(radio_unit_counter >= max_unit_count) {
    2c08:	e7be      	b.n	2b88 <main+0xd14>
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
    2c0a:	4b21      	ldr	r3, [pc, #132]	; (2c90 <main+0xe1c>)
    2c0c:	6822      	ldr	r2, [r4, #0]
    2c0e:	881b      	ldrh	r3, [r3, #0]
    2c10:	18d3      	adds	r3, r2, r3
    2c12:	6023      	str	r3, [r4, #0]
                if(xo_day_time_in_sec >= DAY_END_TIME) {
    2c14:	4b2e      	ldr	r3, [pc, #184]	; (2cd0 <main+0xe5c>)
    2c16:	681a      	ldr	r2, [r3, #0]
    2c18:	4b44      	ldr	r3, [pc, #272]	; (2d2c <main+0xeb8>)
    2c1a:	681b      	ldr	r3, [r3, #0]
    2c1c:	429a      	cmp	r2, r3
    2c1e:	d200      	bcs.n	2c22 <main+0xdae>
    2c20:	e5c8      	b.n	27b4 <main+0x940>
    2c22:	e7bd      	b.n	2ba0 <main+0xd2c>
        else if(goc_state == STATE_RADIO_REST) {
    2c24:	883b      	ldrh	r3, [r7, #0]
    2c26:	2b09      	cmp	r3, #9
    2c28:	d110      	bne.n	2c4c <main+0xdd8>
            if(projected_end_time_in_sec + PMU_WAKEUP_INTERVAL + XO_2_MIN >= radio_rest_end_time) {
    2c2a:	6821      	ldr	r1, [r4, #0]
    2c2c:	4b18      	ldr	r3, [pc, #96]	; (2c90 <main+0xe1c>)
    2c2e:	000a      	movs	r2, r1
    2c30:	483c      	ldr	r0, [pc, #240]	; (2d24 <main+0xeb0>)
    2c32:	881e      	ldrh	r6, [r3, #0]
    2c34:	3278      	adds	r2, #120	; 0x78
    2c36:	6805      	ldr	r5, [r0, #0]
    2c38:	1992      	adds	r2, r2, r6
    2c3a:	42aa      	cmp	r2, r5
    2c3c:	d303      	bcc.n	2c46 <main+0xdd2>
                projected_end_time_in_sec = radio_rest_end_time;
    2c3e:	6803      	ldr	r3, [r0, #0]
    2c40:	6023      	str	r3, [r4, #0]
                goc_state = STATE_RADIO_DATA;
    2c42:	2308      	movs	r3, #8
    2c44:	e6a9      	b.n	299a <main+0xb26>
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
    2c46:	881b      	ldrh	r3, [r3, #0]
    2c48:	185b      	adds	r3, r3, r1
    2c4a:	e795      	b.n	2b78 <main+0xd04>
        else if(goc_state == STATE_LOW_PWR) {
    2c4c:	883b      	ldrh	r3, [r7, #0]
    2c4e:	2b0a      	cmp	r3, #10
    2c50:	d000      	beq.n	2c54 <main+0xde0>
    2c52:	e5af      	b.n	27b4 <main+0x940>
            if(day_count >= low_pwr_state_end_day 
    2c54:	4b1f      	ldr	r3, [pc, #124]	; (2cd4 <main+0xe60>)
    2c56:	4d0e      	ldr	r5, [pc, #56]	; (2c90 <main+0xe1c>)
    2c58:	881a      	ldrh	r2, [r3, #0]
    2c5a:	4b0e      	ldr	r3, [pc, #56]	; (2c94 <main+0xe20>)
    2c5c:	681b      	ldr	r3, [r3, #0]
    2c5e:	429a      	cmp	r2, r3
    2c60:	d368      	bcc.n	2d34 <main+0xec0>
                && xo_day_time_in_sec >= (MID_DAY_TIME - PMU_WAKEUP_INTERVAL)
    2c62:	4a1b      	ldr	r2, [pc, #108]	; (2cd0 <main+0xe5c>)
    2c64:	8828      	ldrh	r0, [r5, #0]
    2c66:	4b32      	ldr	r3, [pc, #200]	; (2d30 <main+0xebc>)
    2c68:	6811      	ldr	r1, [r2, #0]
    2c6a:	1a1b      	subs	r3, r3, r0
    2c6c:	428b      	cmp	r3, r1
    2c6e:	d861      	bhi.n	2d34 <main+0xec0>
                && xo_day_time_in_sec <= (MID_DAY_TIME + PMU_WAKEUP_INTERVAL)
    2c70:	882b      	ldrh	r3, [r5, #0]
    2c72:	492f      	ldr	r1, [pc, #188]	; (2d30 <main+0xebc>)
    2c74:	6812      	ldr	r2, [r2, #0]
    2c76:	185b      	adds	r3, r3, r1
    2c78:	4293      	cmp	r3, r2
    2c7a:	d35b      	bcc.n	2d34 <main+0xec0>
                && !set_low_pwr_high_trigger()) {
    2c7c:	f7ff f866 	bl	1d4c <set_low_pwr_high_trigger>
    2c80:	2800      	cmp	r0, #0
    2c82:	d157      	bne.n	2d34 <main+0xec0>
                flush_temp_cache();
    2c84:	f7fe fafc 	bl	1280 <flush_temp_cache>
                initialize_state_collect();
    2c88:	f7ff f88e 	bl	1da8 <initialize_state_collect>
    2c8c:	e592      	b.n	27b4 <main+0x940>
    2c8e:	46c0      	nop			; (mov r8, r8)
    2c90:	000038c4 	.word	0x000038c4
    2c94:	00003a54 	.word	0x00003a54
    2c98:	00003ad8 	.word	0x00003ad8
    2c9c:	00003a70 	.word	0x00003a70
    2ca0:	000039fc 	.word	0x000039fc
    2ca4:	00003a8c 	.word	0x00003a8c
    2ca8:	00003a74 	.word	0x00003a74
    2cac:	00003aec 	.word	0x00003aec
    2cb0:	000038d0 	.word	0x000038d0
    2cb4:	00003a00 	.word	0x00003a00
    2cb8:	000039b8 	.word	0x000039b8
    2cbc:	00003938 	.word	0x00003938
    2cc0:	000039b4 	.word	0x000039b4
    2cc4:	00003a68 	.word	0x00003a68
    2cc8:	00003a84 	.word	0x00003a84
    2ccc:	00003a5a 	.word	0x00003a5a
    2cd0:	00003ae4 	.word	0x00003ae4
    2cd4:	00003a18 	.word	0x00003a18
    2cd8:	00003a88 	.word	0x00003a88
    2cdc:	00003a20 	.word	0x00003a20
    2ce0:	000038d4 	.word	0x000038d4
    2ce4:	00003a8e 	.word	0x00003a8e
    2ce8:	000038dc 	.word	0x000038dc
    2cec:	00003a90 	.word	0x00003a90
    2cf0:	000038e8 	.word	0x000038e8
    2cf4:	000038d8 	.word	0x000038d8
    2cf8:	000038c8 	.word	0x000038c8
    2cfc:	00003a6c 	.word	0x00003a6c
    2d00:	000038de 	.word	0x000038de
    2d04:	000038cc 	.word	0x000038cc
    2d08:	00003a98 	.word	0x00003a98
    2d0c:	00003a08 	.word	0x00003a08
    2d10:	00003a92 	.word	0x00003a92
    2d14:	00003a3c 	.word	0x00003a3c
    2d18:	00003ae0 	.word	0x00003ae0
    2d1c:	000038e4 	.word	0x000038e4
    2d20:	000038e6 	.word	0x000038e6
    2d24:	00003a94 	.word	0x00003a94
    2d28:	000037ba 	.word	0x000037ba
    2d2c:	000037bc 	.word	0x000037bc
    2d30:	0000a8c0 	.word	0x0000a8c0
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
    2d34:	882b      	ldrh	r3, [r5, #0]
    2d36:	e5b8      	b.n	28aa <main+0xa36>
    else if(goc_data_header == 0x08) {
    2d38:	2a08      	cmp	r2, #8
    2d3a:	d113      	bne.n	2d64 <main+0xef0>
        radio_data_arr[2] = (0x8 << 8) | CHIP_ID;
    2d3c:	4ac0      	ldr	r2, [pc, #768]	; (3040 <main+0x11cc>)
        uint8_t index = (goc_data_full >> 16) & 0x7F;
    2d3e:	681b      	ldr	r3, [r3, #0]
        radio_data_arr[2] = (0x8 << 8) | CHIP_ID;
    2d40:	8811      	ldrh	r1, [r2, #0]
    2d42:	2280      	movs	r2, #128	; 0x80
    2d44:	0112      	lsls	r2, r2, #4
    2d46:	430a      	orrs	r2, r1
        uint8_t index = (goc_data_full >> 16) & 0x7F;
    2d48:	025b      	lsls	r3, r3, #9
        radio_data_arr[2] = (0x8 << 8) | CHIP_ID;
    2d4a:	49be      	ldr	r1, [pc, #760]	; (3044 <main+0x11d0>)
        radio_data_arr[1] = (index << 4) | light_code_lengths[index];
    2d4c:	0e5b      	lsrs	r3, r3, #25
    2d4e:	48be      	ldr	r0, [pc, #760]	; (3048 <main+0x11d4>)
        radio_data_arr[2] = (0x8 << 8) | CHIP_ID;
    2d50:	608a      	str	r2, [r1, #8]
        radio_data_arr[1] = (index << 4) | light_code_lengths[index];
    2d52:	011a      	lsls	r2, r3, #4
    2d54:	005b      	lsls	r3, r3, #1
    2d56:	5a18      	ldrh	r0, [r3, r0]
    2d58:	4302      	orrs	r2, r0
    2d5a:	604a      	str	r2, [r1, #4]
        radio_data_arr[0] = light_diff_codes[index];
    2d5c:	4abb      	ldr	r2, [pc, #748]	; (304c <main+0x11d8>)
        radio_data_arr[0] = temp_diff_codes[index];
    2d5e:	5a9b      	ldrh	r3, [r3, r2]
        radio_data_arr[0] = (LOW_PWR_LOW_ADC_THRESH[index] << 8) | LOW_PWR_HIGH_ADC_THRESH[index];
    2d60:	600b      	str	r3, [r1, #0]
        send_beacon();
    2d62:	e508      	b.n	2776 <main+0x902>
    else if(goc_data_header == 0x09) {
    2d64:	2a09      	cmp	r2, #9
    2d66:	d111      	bne.n	2d8c <main+0xf18>
        radio_data_arr[2] = (0x9 << 8) | CHIP_ID;
    2d68:	4ab5      	ldr	r2, [pc, #724]	; (3040 <main+0x11cc>)
        uint8_t index = (goc_data_full >> 16) & 0x7F;
    2d6a:	681b      	ldr	r3, [r3, #0]
        radio_data_arr[2] = (0x9 << 8) | CHIP_ID;
    2d6c:	8811      	ldrh	r1, [r2, #0]
    2d6e:	2290      	movs	r2, #144	; 0x90
    2d70:	0112      	lsls	r2, r2, #4
    2d72:	430a      	orrs	r2, r1
        uint8_t index = (goc_data_full >> 16) & 0x7F;
    2d74:	025b      	lsls	r3, r3, #9
        radio_data_arr[2] = (0x9 << 8) | CHIP_ID;
    2d76:	49b3      	ldr	r1, [pc, #716]	; (3044 <main+0x11d0>)
        radio_data_arr[1] = (index << 4) | temp_code_lengths[index];
    2d78:	0e5b      	lsrs	r3, r3, #25
    2d7a:	48b5      	ldr	r0, [pc, #724]	; (3050 <main+0x11dc>)
        radio_data_arr[2] = (0x9 << 8) | CHIP_ID;
    2d7c:	608a      	str	r2, [r1, #8]
        radio_data_arr[1] = (index << 4) | temp_code_lengths[index];
    2d7e:	011a      	lsls	r2, r3, #4
    2d80:	005b      	lsls	r3, r3, #1
    2d82:	5a18      	ldrh	r0, [r3, r0]
    2d84:	4302      	orrs	r2, r0
    2d86:	604a      	str	r2, [r1, #4]
        radio_data_arr[0] = temp_diff_codes[index];
    2d88:	4ab2      	ldr	r2, [pc, #712]	; (3054 <main+0x11e0>)
    2d8a:	e7e8      	b.n	2d5e <main+0xeea>
    else if(goc_data_header == 0x0A) {
    2d8c:	2a0a      	cmp	r2, #10
    2d8e:	d115      	bne.n	2dbc <main+0xf48>
        if(option) {
    2d90:	49b1      	ldr	r1, [pc, #708]	; (3058 <main+0x11e4>)
    2d92:	48b2      	ldr	r0, [pc, #712]	; (305c <main+0x11e8>)
    2d94:	2d00      	cmp	r5, #0
    2d96:	d004      	beq.n	2da2 <main+0xf2e>
            day_count = 0;
    2d98:	2200      	movs	r2, #0
    2d9a:	800a      	strh	r2, [r1, #0]
            epoch_days_offset = goc_data_full & 0xFFFF;
    2d9c:	681b      	ldr	r3, [r3, #0]
    2d9e:	b29b      	uxth	r3, r3
    2da0:	8003      	strh	r3, [r0, #0]
        radio_data_arr[2] = (0xA << 8) | CHIP_ID;
    2da2:	22a0      	movs	r2, #160	; 0xa0
    2da4:	4ba6      	ldr	r3, [pc, #664]	; (3040 <main+0x11cc>)
    2da6:	0112      	lsls	r2, r2, #4
    2da8:	881b      	ldrh	r3, [r3, #0]
    2daa:	431a      	orrs	r2, r3
    2dac:	4ba5      	ldr	r3, [pc, #660]	; (3044 <main+0x11d0>)
    2dae:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = epoch_days_offset;
    2db0:	8802      	ldrh	r2, [r0, #0]
    2db2:	b292      	uxth	r2, r2
    2db4:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = day_count;
    2db6:	880a      	ldrh	r2, [r1, #0]
        radio_data_arr[0] = EDGE_THRESHOLD;
    2db8:	b292      	uxth	r2, r2
    2dba:	e1a3      	b.n	3104 <main+0x1290>
    else if(goc_data_header == 0x0B) {
    2dbc:	2a0b      	cmp	r2, #11
    2dbe:	d144      	bne.n	2e4a <main+0xfd6>
        if(option) {
    2dc0:	4ea7      	ldr	r6, [pc, #668]	; (3060 <main+0x11ec>)
    2dc2:	2d00      	cmp	r5, #0
    2dc4:	d010      	beq.n	2de8 <main+0xf74>
            uint8_t H = (goc_data_full >> 6) & 0x1F;
    2dc6:	681a      	ldr	r2, [r3, #0]
            uint8_t M = goc_data_full & 0x3F;
    2dc8:	6819      	ldr	r1, [r3, #0]
            xo_day_time_in_sec = H * 3600 + M * 60;
    2dca:	23e1      	movs	r3, #225	; 0xe1
            uint8_t H = (goc_data_full >> 6) & 0x1F;
    2dcc:	0552      	lsls	r2, r2, #21
            xo_day_time_in_sec = H * 3600 + M * 60;
    2dce:	0ed2      	lsrs	r2, r2, #27
    2dd0:	011b      	lsls	r3, r3, #4
    2dd2:	4353      	muls	r3, r2
    2dd4:	223f      	movs	r2, #63	; 0x3f
    2dd6:	4011      	ands	r1, r2
    2dd8:	3a03      	subs	r2, #3
    2dda:	434a      	muls	r2, r1
    2ddc:	189b      	adds	r3, r3, r2
    2dde:	4aa1      	ldr	r2, [pc, #644]	; (3064 <main+0x11f0>)
    2de0:	6013      	str	r3, [r2, #0]
            sys_sec_to_min_offset = xo_sys_time_in_sec;
    2de2:	6832      	ldr	r2, [r6, #0]
    2de4:	4ba0      	ldr	r3, [pc, #640]	; (3068 <main+0x11f4>)
    2de6:	601a      	str	r2, [r3, #0]
        radio_data_arr[2] = (0xB << 8) | CHIP_ID;
    2de8:	4b95      	ldr	r3, [pc, #596]	; (3040 <main+0x11cc>)
    2dea:	4d96      	ldr	r5, [pc, #600]	; (3044 <main+0x11d0>)
    2dec:	881a      	ldrh	r2, [r3, #0]
    2dee:	23b0      	movs	r3, #176	; 0xb0
    2df0:	011b      	lsls	r3, r3, #4
    2df2:	4313      	orrs	r3, r2
    2df4:	60ab      	str	r3, [r5, #8]
        radio_data_arr[1] = mult(86400, epoch_days_offset) + xo_day_time_in_sec; // epoch_time_offset
    2df6:	4b99      	ldr	r3, [pc, #612]	; (305c <main+0x11e8>)
    2df8:	489c      	ldr	r0, [pc, #624]	; (306c <main+0x11f8>)
    2dfa:	8819      	ldrh	r1, [r3, #0]
    2dfc:	b289      	uxth	r1, r1
    2dfe:	f7fd fcf3 	bl	7e8 <mult>
    2e02:	4b98      	ldr	r3, [pc, #608]	; (3064 <main+0x11f0>)
        radio_data_arr[0] = divide_by_60(xo_sys_time_in_sec - sys_sec_to_min_offset); // sys_time_in_min
    2e04:	4f98      	ldr	r7, [pc, #608]	; (3068 <main+0x11f4>)
        radio_data_arr[1] = mult(86400, epoch_days_offset) + xo_day_time_in_sec; // epoch_time_offset
    2e06:	681b      	ldr	r3, [r3, #0]
    2e08:	181b      	adds	r3, r3, r0
    2e0a:	606b      	str	r3, [r5, #4]
        radio_data_arr[0] = divide_by_60(xo_sys_time_in_sec - sys_sec_to_min_offset); // sys_time_in_min
    2e0c:	6830      	ldr	r0, [r6, #0]
    2e0e:	683b      	ldr	r3, [r7, #0]
    2e10:	1ac0      	subs	r0, r0, r3
    2e12:	f7fd fbfd 	bl	610 <divide_by_60>
    2e16:	6028      	str	r0, [r5, #0]
        send_beacon();
    2e18:	f7fe ffb8 	bl	1d8c <send_beacon>
        radio_data_arr[2] = (0xB << 8) | (0x1 << 6) | CHIP_ID;
    2e1c:	4b88      	ldr	r3, [pc, #544]	; (3040 <main+0x11cc>)
    2e1e:	881a      	ldrh	r2, [r3, #0]
    2e20:	23b4      	movs	r3, #180	; 0xb4
    2e22:	011b      	lsls	r3, r3, #4
    2e24:	4313      	orrs	r3, r2
    2e26:	60ab      	str	r3, [r5, #8]
        radio_data_arr[1] = xo_sys_time_in_sec;
    2e28:	6833      	ldr	r3, [r6, #0]
    2e2a:	606b      	str	r3, [r5, #4]
        radio_data_arr[0] = sys_sec_to_min_offset;
    2e2c:	683b      	ldr	r3, [r7, #0]
    2e2e:	602b      	str	r3, [r5, #0]
        send_beacon();
    2e30:	f7fe ffac 	bl	1d8c <send_beacon>
        radio_data_arr[2] = (0xB << 8) | (0x2 << 6) | CHIP_ID;
    2e34:	4b82      	ldr	r3, [pc, #520]	; (3040 <main+0x11cc>)
    2e36:	881a      	ldrh	r2, [r3, #0]
    2e38:	23b8      	movs	r3, #184	; 0xb8
    2e3a:	011b      	lsls	r3, r3, #4
    2e3c:	4313      	orrs	r3, r2
    2e3e:	60ab      	str	r3, [r5, #8]
        radio_data_arr[1] = 0;
    2e40:	2300      	movs	r3, #0
    2e42:	606b      	str	r3, [r5, #4]
        radio_data_arr[0] = xo_day_time_in_sec;
    2e44:	4b87      	ldr	r3, [pc, #540]	; (3064 <main+0x11f0>)
    2e46:	681b      	ldr	r3, [r3, #0]
    2e48:	e494      	b.n	2774 <main+0x900>
    else if(goc_data_header == 0x0C) {
    2e4a:	2a0c      	cmp	r2, #12
    2e4c:	d128      	bne.n	2ea0 <main+0x102c>
        uint8_t option = (goc_data_full >> 21) & 0x7;
    2e4e:	681a      	ldr	r2, [r3, #0]
        uint16_t N = goc_data_full & 0xFFFF;
    2e50:	681b      	ldr	r3, [r3, #0]
        uint8_t option = (goc_data_full >> 21) & 0x7;
    2e52:	0212      	lsls	r2, r2, #8
    2e54:	4d86      	ldr	r5, [pc, #536]	; (3070 <main+0x11fc>)
    2e56:	4e87      	ldr	r6, [pc, #540]	; (3074 <main+0x1200>)
    2e58:	4987      	ldr	r1, [pc, #540]	; (3078 <main+0x1204>)
    2e5a:	4888      	ldr	r0, [pc, #544]	; (307c <main+0x1208>)
    2e5c:	0f52      	lsrs	r2, r2, #29
        uint16_t N = goc_data_full & 0xFFFF;
    2e5e:	b29b      	uxth	r3, r3
        if(option == 1) {
    2e60:	2a01      	cmp	r2, #1
    2e62:	d111      	bne.n	2e88 <main+0x1014>
            xo_to_sec_mplier = N;
    2e64:	802b      	strh	r3, [r5, #0]
        radio_data_arr[2] = (0xC << 8) | CHIP_ID;
    2e66:	4b76      	ldr	r3, [pc, #472]	; (3040 <main+0x11cc>)
        radio_data_arr[0] = (LNT_MPLIER_SHIFT << 16) | xo_lnt_mplier;
    2e68:	8809      	ldrh	r1, [r1, #0]
        radio_data_arr[2] = (0xC << 8) | CHIP_ID;
    2e6a:	881a      	ldrh	r2, [r3, #0]
    2e6c:	23c0      	movs	r3, #192	; 0xc0
    2e6e:	011b      	lsls	r3, r3, #4
    2e70:	4313      	orrs	r3, r2
    2e72:	4a74      	ldr	r2, [pc, #464]	; (3044 <main+0x11d0>)
    2e74:	6093      	str	r3, [r2, #8]
        radio_data_arr[1] = (XO_TO_SEC_MPLIER_SHIFT << 16) | xo_to_sec_mplier;
    2e76:	8833      	ldrh	r3, [r6, #0]
    2e78:	882d      	ldrh	r5, [r5, #0]
    2e7a:	041b      	lsls	r3, r3, #16
    2e7c:	432b      	orrs	r3, r5
    2e7e:	6053      	str	r3, [r2, #4]
        radio_data_arr[0] = (LNT_MPLIER_SHIFT << 16) | xo_lnt_mplier;
    2e80:	8803      	ldrh	r3, [r0, #0]
    2e82:	041b      	lsls	r3, r3, #16
        radio_data_arr[0] = (end_day_count << 16) | radio_day_count;
    2e84:	430b      	orrs	r3, r1
    2e86:	e213      	b.n	32b0 <main+0x143c>
        else if(option == 2) {
    2e88:	2a02      	cmp	r2, #2
    2e8a:	d101      	bne.n	2e90 <main+0x101c>
            xo_lnt_mplier = N;
    2e8c:	800b      	strh	r3, [r1, #0]
    2e8e:	e7ea      	b.n	2e66 <main+0xff2>
        else if(option == 3) {
    2e90:	2a03      	cmp	r2, #3
    2e92:	d101      	bne.n	2e98 <main+0x1024>
            XO_TO_SEC_MPLIER_SHIFT = N;
    2e94:	8033      	strh	r3, [r6, #0]
    2e96:	e7e6      	b.n	2e66 <main+0xff2>
        else if(option == 4) {
    2e98:	2a04      	cmp	r2, #4
    2e9a:	d1e4      	bne.n	2e66 <main+0xff2>
            LNT_MPLIER_SHIFT = N;
    2e9c:	8003      	strh	r3, [r0, #0]
    2e9e:	e7e2      	b.n	2e66 <main+0xff2>
    else if(goc_data_header == 0x0D) {
    2ea0:	2a0d      	cmp	r2, #13
    2ea2:	d114      	bne.n	2ece <main+0x105a>
        uint8_t index = (goc_data_full >> 21) & 0x3;
    2ea4:	681a      	ldr	r2, [r3, #0]
    2ea6:	4e76      	ldr	r6, [pc, #472]	; (3080 <main+0x120c>)
    2ea8:	0252      	lsls	r2, r2, #9
    2eaa:	0f92      	lsrs	r2, r2, #30
        if(option) {
    2eac:	0050      	lsls	r0, r2, #1
    2eae:	2d00      	cmp	r5, #0
    2eb0:	d003      	beq.n	2eba <main+0x1046>
            resample_indices[index] = goc_data_full & 0x3F;
    2eb2:	213f      	movs	r1, #63	; 0x3f
    2eb4:	681b      	ldr	r3, [r3, #0]
    2eb6:	400b      	ands	r3, r1
    2eb8:	5233      	strh	r3, [r6, r0]
        radio_data_arr[2] = (0xD << 8) | CHIP_ID;
    2eba:	21d0      	movs	r1, #208	; 0xd0
    2ebc:	4b60      	ldr	r3, [pc, #384]	; (3040 <main+0x11cc>)
    2ebe:	0109      	lsls	r1, r1, #4
    2ec0:	881b      	ldrh	r3, [r3, #0]
    2ec2:	4319      	orrs	r1, r3
    2ec4:	4b5f      	ldr	r3, [pc, #380]	; (3044 <main+0x11d0>)
    2ec6:	6099      	str	r1, [r3, #8]
        radio_data_arr[1] = index;
    2ec8:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = resample_indices[index];
    2eca:	5a32      	ldrh	r2, [r6, r0]
    2ecc:	e774      	b.n	2db8 <main+0xf44>
    else if(goc_data_header == 0x0E) {
    2ece:	2a0e      	cmp	r2, #14
    2ed0:	d119      	bne.n	2f06 <main+0x1092>
        if(option) {
    2ed2:	486c      	ldr	r0, [pc, #432]	; (3084 <main+0x1210>)
    2ed4:	496c      	ldr	r1, [pc, #432]	; (3088 <main+0x1214>)
    2ed6:	2d00      	cmp	r5, #0
    2ed8:	d00a      	beq.n	2ef0 <main+0x107c>
            cur_sunrise = ((goc_data_full >> 11) & 0x7FF) * 60;
    2eda:	253c      	movs	r5, #60	; 0x3c
    2edc:	681a      	ldr	r2, [r3, #0]
    2ede:	0292      	lsls	r2, r2, #10
    2ee0:	0d52      	lsrs	r2, r2, #21
    2ee2:	436a      	muls	r2, r5
    2ee4:	6002      	str	r2, [r0, #0]
            cur_sunset = (goc_data_full & 0x7FF) * 60;
    2ee6:	681b      	ldr	r3, [r3, #0]
    2ee8:	055b      	lsls	r3, r3, #21
    2eea:	0d5b      	lsrs	r3, r3, #21
    2eec:	436b      	muls	r3, r5
    2eee:	600b      	str	r3, [r1, #0]
        radio_data_arr[2] = (0xE << 8) | CHIP_ID;
    2ef0:	22e0      	movs	r2, #224	; 0xe0
    2ef2:	4b53      	ldr	r3, [pc, #332]	; (3040 <main+0x11cc>)
    2ef4:	0112      	lsls	r2, r2, #4
    2ef6:	881b      	ldrh	r3, [r3, #0]
        radio_data_arr[2] = (0x11 << 8) | CHIP_ID;
    2ef8:	431a      	orrs	r2, r3
    2efa:	4b52      	ldr	r3, [pc, #328]	; (3044 <main+0x11d0>)
    2efc:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = DAY_START_TIME;
    2efe:	6802      	ldr	r2, [r0, #0]
        radio_data_arr[1] = 0;
    2f00:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = data_collection_end_day_time;
    2f02:	680a      	ldr	r2, [r1, #0]
    2f04:	e0fe      	b.n	3104 <main+0x1290>
    else if(goc_data_header == 0x0F) {
    2f06:	2a0f      	cmp	r2, #15
    2f08:	d13f      	bne.n	2f8a <main+0x1116>
        uint8_t option = (goc_data_full >> 22) & 0x3;
    2f0a:	681f      	ldr	r7, [r3, #0]
        uint8_t N = (goc_data_full >> 8) & 0xFF;
    2f0c:	681a      	ldr	r2, [r3, #0]
        uint8_t M = goc_data_full & 0xFF;
    2f0e:	681e      	ldr	r6, [r3, #0]
        uint8_t option = (goc_data_full >> 22) & 0x3;
    2f10:	023f      	lsls	r7, r7, #8
    2f12:	495e      	ldr	r1, [pc, #376]	; (308c <main+0x1218>)
    2f14:	485e      	ldr	r0, [pc, #376]	; (3090 <main+0x121c>)
    2f16:	4d5f      	ldr	r5, [pc, #380]	; (3094 <main+0x1220>)
    2f18:	0fbf      	lsrs	r7, r7, #30
        uint8_t M = goc_data_full & 0xFF;
    2f1a:	b2f6      	uxtb	r6, r6
        if(option == 1) {
    2f1c:	2f01      	cmp	r7, #1
    2f1e:	d127      	bne.n	2f70 <main+0x10fc>
        uint8_t N = (goc_data_full >> 8) & 0xFF;
    2f20:	0a13      	lsrs	r3, r2, #8
            EDGE_MARGIN1 = (N - 1) * 60;
    2f22:	223c      	movs	r2, #60	; 0x3c
    2f24:	b2df      	uxtb	r7, r3
    2f26:	3f01      	subs	r7, #1
    2f28:	4357      	muls	r7, r2
    2f2a:	b2bf      	uxth	r7, r7
    2f2c:	800f      	strh	r7, [r1, #0]
            EDGE_MARGIN2 = M * 60;
    2f2e:	0017      	movs	r7, r2
    2f30:	4377      	muls	r7, r6
            IDX_MAX = N + M - 1;
    2f32:	b2db      	uxtb	r3, r3
    2f34:	3e01      	subs	r6, #1
            EDGE_MARGIN2 = M * 60;
    2f36:	4a58      	ldr	r2, [pc, #352]	; (3098 <main+0x1224>)
            IDX_MAX = N + M - 1;
    2f38:	199b      	adds	r3, r3, r6
    2f3a:	b29b      	uxth	r3, r3
            EDGE_MARGIN2 = M * 60;
    2f3c:	8017      	strh	r7, [r2, #0]
            IDX_MAX = N + M - 1;
    2f3e:	8003      	strh	r3, [r0, #0]
        radio_data_arr[2] = (0xF << 8) | CHIP_ID;
    2f40:	4b3f      	ldr	r3, [pc, #252]	; (3040 <main+0x11cc>)
        radio_data_arr[0] = (EDGE_MARGIN1 << 16) | EDGE_MARGIN2;
    2f42:	4a55      	ldr	r2, [pc, #340]	; (3098 <main+0x1224>)
        radio_data_arr[2] = (0xF << 8) | CHIP_ID;
    2f44:	881e      	ldrh	r6, [r3, #0]
    2f46:	23f0      	movs	r3, #240	; 0xf0
    2f48:	011b      	lsls	r3, r3, #4
    2f4a:	4333      	orrs	r3, r6
    2f4c:	4e3d      	ldr	r6, [pc, #244]	; (3044 <main+0x11d0>)
    2f4e:	60b3      	str	r3, [r6, #8]
        radio_data_arr[1] = (MAX_EDGE_SHIFT << 20) | (PASSIVE_WINDOW_SHIFT << 8) | IDX_MAX;
    2f50:	882b      	ldrh	r3, [r5, #0]
    2f52:	4d52      	ldr	r5, [pc, #328]	; (309c <main+0x1228>)
    2f54:	051b      	lsls	r3, r3, #20
    2f56:	882f      	ldrh	r7, [r5, #0]
    2f58:	8805      	ldrh	r5, [r0, #0]
    2f5a:	023f      	lsls	r7, r7, #8
    2f5c:	433b      	orrs	r3, r7
    2f5e:	432b      	orrs	r3, r5
    2f60:	6073      	str	r3, [r6, #4]
        radio_data_arr[0] = (EDGE_MARGIN1 << 16) | EDGE_MARGIN2;
    2f62:	880b      	ldrh	r3, [r1, #0]
    2f64:	8812      	ldrh	r2, [r2, #0]
    2f66:	041b      	lsls	r3, r3, #16
        radio_data_arr[0] = (RADIO_REST_NUM_UNITS << 20) | RADIO_DATA_PERIOD;
    2f68:	4313      	orrs	r3, r2
    2f6a:	6033      	str	r3, [r6, #0]
        send_beacon();
    2f6c:	f7ff fc03 	bl	2776 <main+0x902>
        else if(option == 2) {
    2f70:	2f02      	cmp	r7, #2
    2f72:	d103      	bne.n	2f7c <main+0x1108>
            MAX_EDGE_SHIFT = M * 60;
    2f74:	233c      	movs	r3, #60	; 0x3c
    2f76:	435e      	muls	r6, r3
    2f78:	802e      	strh	r6, [r5, #0]
    2f7a:	e7e1      	b.n	2f40 <main+0x10cc>
        else if(option == 3) {
    2f7c:	2f03      	cmp	r7, #3
    2f7e:	d1df      	bne.n	2f40 <main+0x10cc>
            PASSIVE_WINDOW_SHIFT = M * 60;
    2f80:	233c      	movs	r3, #60	; 0x3c
    2f82:	435e      	muls	r6, r3
    2f84:	4b45      	ldr	r3, [pc, #276]	; (309c <main+0x1228>)
    2f86:	801e      	strh	r6, [r3, #0]
    2f88:	e7da      	b.n	2f40 <main+0x10cc>
    else if(goc_data_header == 0x10) {
    2f8a:	2a10      	cmp	r2, #16
    2f8c:	d142      	bne.n	3014 <main+0x11a0>
        uint8_t settings = (goc_data_full >> 20) & 0x7;
    2f8e:	3a09      	subs	r2, #9
    2f90:	4694      	mov	ip, r2
        uint8_t index = (goc_data_full >> 17) & 0x7;
    2f92:	4661      	mov	r1, ip
        uint8_t settings = (goc_data_full >> 20) & 0x7;
    2f94:	681e      	ldr	r6, [r3, #0]
        uint8_t index = (goc_data_full >> 17) & 0x7;
    2f96:	6818      	ldr	r0, [r3, #0]
        uint8_t U = (goc_data_full >> 16) & 0x1;
    2f98:	681f      	ldr	r7, [r3, #0]
        uint16_t N = goc_data_full & 0xFFFF;
    2f9a:	681b      	ldr	r3, [r3, #0]
        uint8_t settings = (goc_data_full >> 20) & 0x7;
    2f9c:	0d36      	lsrs	r6, r6, #20
        uint8_t index = (goc_data_full >> 17) & 0x7;
    2f9e:	0c40      	lsrs	r0, r0, #17
        uint16_t N = goc_data_full & 0xFFFF;
    2fa0:	9301      	str	r3, [sp, #4]
        uint8_t settings = (goc_data_full >> 20) & 0x7;
    2fa2:	4032      	ands	r2, r6
            arr = PMU_ACTIVE_SETTINGS;
    2fa4:	4b3e      	ldr	r3, [pc, #248]	; (30a0 <main+0x122c>)
        uint8_t index = (goc_data_full >> 17) & 0x7;
    2fa6:	4008      	ands	r0, r1
        if(settings == 0) {
    2fa8:	420e      	tst	r6, r1
    2faa:	d012      	beq.n	2fd2 <main+0x115e>
            arr = PMU_RADIO_SETTINGS;
    2fac:	4b3d      	ldr	r3, [pc, #244]	; (30a4 <main+0x1230>)
        else if(settings == 1) {
    2fae:	2a01      	cmp	r2, #1
    2fb0:	d00f      	beq.n	2fd2 <main+0x115e>
            arr = PMU_SLEEP_SETTINGS;
    2fb2:	4b3d      	ldr	r3, [pc, #244]	; (30a8 <main+0x1234>)
        else if(settings == 2) {
    2fb4:	2a02      	cmp	r2, #2
    2fb6:	d00c      	beq.n	2fd2 <main+0x115e>
            arr = PMU_ACTIVE_SAR_SETTINGS;
    2fb8:	4b3c      	ldr	r3, [pc, #240]	; (30ac <main+0x1238>)
        else if(settings == 3) {
    2fba:	2a03      	cmp	r2, #3
    2fbc:	d009      	beq.n	2fd2 <main+0x115e>
            arr = PMU_RADIO_SAR_SETTINGS;
    2fbe:	4b3c      	ldr	r3, [pc, #240]	; (30b0 <main+0x123c>)
        else if(settings == 4) {
    2fc0:	2a04      	cmp	r2, #4
    2fc2:	d006      	beq.n	2fd2 <main+0x115e>
            arr = PMU_SLEEP_SAR_SETTINGS;
    2fc4:	4b3b      	ldr	r3, [pc, #236]	; (30b4 <main+0x1240>)
        else if(settings == 5) {
    2fc6:	2a05      	cmp	r2, #5
    2fc8:	d003      	beq.n	2fd2 <main+0x115e>
            arr = PMU_TEMP_THRESH;
    2fca:	4b3b      	ldr	r3, [pc, #236]	; (30b8 <main+0x1244>)
        else if(settings == 6) {
    2fcc:	2a06      	cmp	r2, #6
    2fce:	d000      	beq.n	2fd2 <main+0x115e>
            arr = PMU_ADC_THRESH;
    2fd0:	4b3a      	ldr	r3, [pc, #232]	; (30bc <main+0x1248>)
        if(option) {
    2fd2:	0086      	lsls	r6, r0, #2
    2fd4:	199b      	adds	r3, r3, r6
    2fd6:	2d00      	cmp	r5, #0
    2fd8:	d009      	beq.n	2fee <main+0x117a>
            if(U) {
    2fda:	9901      	ldr	r1, [sp, #4]
    2fdc:	0409      	lsls	r1, r1, #16
    2fde:	03fd      	lsls	r5, r7, #15
    2fe0:	d511      	bpl.n	3006 <main+0x1192>
                arr[index] &= 0x0000FFFF;
    2fe2:	681d      	ldr	r5, [r3, #0]
    2fe4:	b2ad      	uxth	r5, r5
    2fe6:	601d      	str	r5, [r3, #0]
                arr[index] |= (N << 16);
    2fe8:	681d      	ldr	r5, [r3, #0]
                arr[index] |= N;
    2fea:	4329      	orrs	r1, r5
    2fec:	6019      	str	r1, [r3, #0]
        radio_data_arr[2] = (0x10 << 8) | CHIP_ID;
    2fee:	2580      	movs	r5, #128	; 0x80
    2ff0:	4913      	ldr	r1, [pc, #76]	; (3040 <main+0x11cc>)
    2ff2:	016d      	lsls	r5, r5, #5
    2ff4:	8809      	ldrh	r1, [r1, #0]
        radio_data_arr[1] = (settings << 8) | index;
    2ff6:	0212      	lsls	r2, r2, #8
        radio_data_arr[2] = (0x10 << 8) | CHIP_ID;
    2ff8:	430d      	orrs	r5, r1
    2ffa:	4912      	ldr	r1, [pc, #72]	; (3044 <main+0x11d0>)
        radio_data_arr[1] = (settings << 8) | index;
    2ffc:	4302      	orrs	r2, r0
        radio_data_arr[2] = (0x10 << 8) | CHIP_ID;
    2ffe:	608d      	str	r5, [r1, #8]
        radio_data_arr[1] = (settings << 8) | index;
    3000:	604a      	str	r2, [r1, #4]
        radio_data_arr[0] = arr[index];
    3002:	681b      	ldr	r3, [r3, #0]
    3004:	e6ac      	b.n	2d60 <main+0xeec>
                arr[index] &= 0xFFFF0000;
    3006:	681d      	ldr	r5, [r3, #0]
                arr[index] |= N;
    3008:	0c09      	lsrs	r1, r1, #16
                arr[index] &= 0xFFFF0000;
    300a:	0c2d      	lsrs	r5, r5, #16
    300c:	042d      	lsls	r5, r5, #16
    300e:	601d      	str	r5, [r3, #0]
                arr[index] |= N;
    3010:	681d      	ldr	r5, [r3, #0]
    3012:	e7ea      	b.n	2fea <main+0x1176>
    else if(goc_data_header == 0x11) {
    3014:	2a11      	cmp	r2, #17
    3016:	d157      	bne.n	30c8 <main+0x1254>
        if(option) {
    3018:	4929      	ldr	r1, [pc, #164]	; (30c0 <main+0x124c>)
    301a:	482a      	ldr	r0, [pc, #168]	; (30c4 <main+0x1250>)
    301c:	2d00      	cmp	r5, #0
    301e:	d00a      	beq.n	3036 <main+0x11c2>
            DAY_START_TIME = ((goc_data_full >> 11) & 0x7FF) * 60;
    3020:	253c      	movs	r5, #60	; 0x3c
    3022:	681a      	ldr	r2, [r3, #0]
    3024:	0292      	lsls	r2, r2, #10
    3026:	0d52      	lsrs	r2, r2, #21
    3028:	436a      	muls	r2, r5
    302a:	6002      	str	r2, [r0, #0]
            DAY_END_TIME = (goc_data_full & 0x7FF) * 60;
    302c:	681b      	ldr	r3, [r3, #0]
    302e:	055b      	lsls	r3, r3, #21
    3030:	0d5b      	lsrs	r3, r3, #21
    3032:	436b      	muls	r3, r5
    3034:	600b      	str	r3, [r1, #0]
        radio_data_arr[2] = (0x11 << 8) | CHIP_ID;
    3036:	2288      	movs	r2, #136	; 0x88
    3038:	4b01      	ldr	r3, [pc, #4]	; (3040 <main+0x11cc>)
    303a:	0152      	lsls	r2, r2, #5
    303c:	881b      	ldrh	r3, [r3, #0]
    303e:	e75b      	b.n	2ef8 <main+0x1084>
    3040:	00003a00 	.word	0x00003a00
    3044:	000039b8 	.word	0x000039b8
    3048:	0000369a 	.word	0x0000369a
    304c:	00003720 	.word	0x00003720
    3050:	000037a6 	.word	0x000037a6
    3054:	000037b0 	.word	0x000037b0
    3058:	00003a18 	.word	0x00003a18
    305c:	00003a20 	.word	0x00003a20
    3060:	00003aec 	.word	0x00003aec
    3064:	00003ae4 	.word	0x00003ae4
    3068:	00003adc 	.word	0x00003adc
    306c:	00015180 	.word	0x00015180
    3070:	00003950 	.word	0x00003950
    3074:	000038ec 	.word	0x000038ec
    3078:	0000394e 	.word	0x0000394e
    307c:	000037cc 	.word	0x000037cc
    3080:	0000392e 	.word	0x0000392e
    3084:	00003a10 	.word	0x00003a10
    3088:	00003a14 	.word	0x00003a14
    308c:	000037c4 	.word	0x000037c4
    3090:	000037ca 	.word	0x000037ca
    3094:	000037ea 	.word	0x000037ea
    3098:	000037c6 	.word	0x000037c6
    309c:	00003a04 	.word	0x00003a04
    30a0:	0000380c 	.word	0x0000380c
    30a4:	00003858 	.word	0x00003858
    30a8:	00003890 	.word	0x00003890
    30ac:	000037f0 	.word	0x000037f0
    30b0:	0000383c 	.word	0x0000383c
    30b4:	00003874 	.word	0x00003874
    30b8:	000038ac 	.word	0x000038ac
    30bc:	00003828 	.word	0x00003828
    30c0:	000037bc 	.word	0x000037bc
    30c4:	000037c0 	.word	0x000037c0
    else if(goc_data_header == 0x12) {
    30c8:	2a12      	cmp	r2, #18
    30ca:	d11e      	bne.n	310a <main+0x1296>
        uint8_t option2 = (goc_data_full >> 22) & 0x1;
    30cc:	6818      	ldr	r0, [r3, #0]
    30ce:	49c0      	ldr	r1, [pc, #768]	; (33d0 <main+0x155c>)
    30d0:	0240      	lsls	r0, r0, #9
        uint8_t N = (goc_data_full >> 17) & 0x1F;
    30d2:	681a      	ldr	r2, [r3, #0]
        uint8_t option2 = (goc_data_full >> 22) & 0x1;
    30d4:	0fc0      	lsrs	r0, r0, #31
        uint32_t M = goc_data_full & 0x1FFFF;
    30d6:	681b      	ldr	r3, [r3, #0]
        if(option) {
    30d8:	2d00      	cmp	r5, #0
    30da:	d002      	beq.n	30e2 <main+0x126e>
        uint8_t N = (goc_data_full >> 17) & 0x1F;
    30dc:	0292      	lsls	r2, r2, #10
            mrr_freq_hopping = N;
    30de:	0ed2      	lsrs	r2, r2, #27
    30e0:	800a      	strh	r2, [r1, #0]
        if(option2) {
    30e2:	4dbc      	ldr	r5, [pc, #752]	; (33d4 <main+0x1560>)
    30e4:	2800      	cmp	r0, #0
    30e6:	d002      	beq.n	30ee <main+0x127a>
        uint32_t M = goc_data_full & 0x1FFFF;
    30e8:	03db      	lsls	r3, r3, #15
    30ea:	0bdb      	lsrs	r3, r3, #15
            RADIO_PACKET_DELAY = M;
    30ec:	602b      	str	r3, [r5, #0]
        radio_data_arr[2] = (0x12 << 8) | CHIP_ID;
    30ee:	2290      	movs	r2, #144	; 0x90
    30f0:	4bb9      	ldr	r3, [pc, #740]	; (33d8 <main+0x1564>)
    30f2:	0152      	lsls	r2, r2, #5
    30f4:	881b      	ldrh	r3, [r3, #0]
    30f6:	431a      	orrs	r2, r3
    30f8:	4bb8      	ldr	r3, [pc, #736]	; (33dc <main+0x1568>)
    30fa:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = mrr_freq_hopping;
    30fc:	880a      	ldrh	r2, [r1, #0]
    30fe:	b292      	uxth	r2, r2
    3100:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = RADIO_PACKET_DELAY;
    3102:	682a      	ldr	r2, [r5, #0]
        radio_data_arr[0] = data_collection_end_day_time;
    3104:	601a      	str	r2, [r3, #0]
    3106:	f7ff fb36 	bl	2776 <main+0x902>
    else if(goc_data_header == 0x13) {
    310a:	2a13      	cmp	r2, #19
    310c:	d11b      	bne.n	3146 <main+0x12d2>
        uint16_t option2 = (goc_data_full >> 22) & 0x1;
    310e:	6819      	ldr	r1, [r3, #0]
    3110:	48b3      	ldr	r0, [pc, #716]	; (33e0 <main+0x156c>)
    3112:	0249      	lsls	r1, r1, #9
        uint16_t N = (goc_data_full >> 17) & 0x1F;
    3114:	681a      	ldr	r2, [r3, #0]
        uint16_t option2 = (goc_data_full >> 22) & 0x1;
    3116:	0fc9      	lsrs	r1, r1, #31
        uint16_t M = goc_data_full & 0xFFFF;
    3118:	681e      	ldr	r6, [r3, #0]
        if(option) {
    311a:	2d00      	cmp	r5, #0
    311c:	d002      	beq.n	3124 <main+0x12b0>
        uint16_t N = (goc_data_full >> 17) & 0x1F;
    311e:	0293      	lsls	r3, r2, #10
    3120:	0edb      	lsrs	r3, r3, #27
            THRESHOLD_IDX_SHIFT = N;
    3122:	8003      	strh	r3, [r0, #0]
        if(option2) {
    3124:	4daf      	ldr	r5, [pc, #700]	; (33e4 <main+0x1570>)
    3126:	2900      	cmp	r1, #0
    3128:	d001      	beq.n	312e <main+0x12ba>
        uint16_t M = goc_data_full & 0xFFFF;
    312a:	b2b6      	uxth	r6, r6
            EDGE_THRESHOLD = M;
    312c:	802e      	strh	r6, [r5, #0]
        radio_data_arr[2] = (0x13 << 8) | CHIP_ID;
    312e:	2298      	movs	r2, #152	; 0x98
    3130:	4ba9      	ldr	r3, [pc, #676]	; (33d8 <main+0x1564>)
    3132:	0152      	lsls	r2, r2, #5
    3134:	881b      	ldrh	r3, [r3, #0]
    3136:	431a      	orrs	r2, r3
    3138:	4ba8      	ldr	r3, [pc, #672]	; (33dc <main+0x1568>)
    313a:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = THRESHOLD_IDX_SHIFT;
    313c:	8802      	ldrh	r2, [r0, #0]
    313e:	b292      	uxth	r2, r2
    3140:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = EDGE_THRESHOLD;
    3142:	882a      	ldrh	r2, [r5, #0]
    3144:	e638      	b.n	2db8 <main+0xf44>
    else if(goc_data_header == 0x14) {
    3146:	2a14      	cmp	r2, #20
    3148:	d000      	beq.n	314c <main+0x12d8>
    314a:	e075      	b.n	3238 <main+0x13c4>
        uint16_t option = (goc_data_full >> 20) & 0xF;
    314c:	6818      	ldr	r0, [r3, #0]
        uint32_t N = goc_data_full & 0xFFFFF;
    314e:	681b      	ldr	r3, [r3, #0]
        uint16_t option = (goc_data_full >> 20) & 0xF;
    3150:	0200      	lsls	r0, r0, #8
        uint32_t N = goc_data_full & 0xFFFFF;
    3152:	031b      	lsls	r3, r3, #12
    3154:	49a4      	ldr	r1, [pc, #656]	; (33e8 <main+0x1574>)
    3156:	4aa5      	ldr	r2, [pc, #660]	; (33ec <main+0x1578>)
    3158:	4da5      	ldr	r5, [pc, #660]	; (33f0 <main+0x157c>)
        uint16_t option = (goc_data_full >> 20) & 0xF;
    315a:	0f00      	lsrs	r0, r0, #28
        uint32_t N = goc_data_full & 0xFFFFF;
    315c:	0b1b      	lsrs	r3, r3, #12
        if(option == 1) {
    315e:	2801      	cmp	r0, #1
    3160:	d138      	bne.n	31d4 <main+0x1360>
            PMU_WAKEUP_INTERVAL = N;
    3162:	b29b      	uxth	r3, r3
    3164:	800b      	strh	r3, [r1, #0]
        radio_data_arr[2] = (0x14 << 8) | CHIP_ID;
    3166:	23a0      	movs	r3, #160	; 0xa0
    3168:	4f9b      	ldr	r7, [pc, #620]	; (33d8 <main+0x1564>)
    316a:	4e9c      	ldr	r6, [pc, #624]	; (33dc <main+0x1568>)
    316c:	8838      	ldrh	r0, [r7, #0]
    316e:	015b      	lsls	r3, r3, #5
    3170:	4303      	orrs	r3, r0
    3172:	60b3      	str	r3, [r6, #8]
        radio_data_arr[1] = (PMU_WAKEUP_INTERVAL << 16) | (RADIO_SEQUENCE_PERIOD << 8) | RADIO_DUTY_DURATION; 
    3174:	880b      	ldrh	r3, [r1, #0]
    3176:	499f      	ldr	r1, [pc, #636]	; (33f4 <main+0x1580>)
    3178:	041b      	lsls	r3, r3, #16
    317a:	8808      	ldrh	r0, [r1, #0]
    317c:	8811      	ldrh	r1, [r2, #0]
    317e:	0200      	lsls	r0, r0, #8
    3180:	4303      	orrs	r3, r0
    3182:	430b      	orrs	r3, r1
    3184:	6073      	str	r3, [r6, #4]
        radio_data_arr[0] = RADIO_INITIAL_BEACON_TIME;
    3186:	4b9c      	ldr	r3, [pc, #624]	; (33f8 <main+0x1584>)
    3188:	681b      	ldr	r3, [r3, #0]
    318a:	6033      	str	r3, [r6, #0]
        send_beacon();
    318c:	f7fe fdfe 	bl	1d8c <send_beacon>
        radio_data_arr[2] = (0x14 << 8) | (0x1 << 6) | CHIP_ID;
    3190:	23a2      	movs	r3, #162	; 0xa2
    3192:	883a      	ldrh	r2, [r7, #0]
    3194:	015b      	lsls	r3, r3, #5
    3196:	4313      	orrs	r3, r2
    3198:	60b3      	str	r3, [r6, #8]
        radio_data_arr[1] = RADIO_BEACON_PERIOD;
    319a:	4b98      	ldr	r3, [pc, #608]	; (33fc <main+0x1588>)
        radio_data_arr[0] = (RADIO_INITIAL_DATA_DAY << 24) | RADIO_INITIAL_DATA_TIME;
    319c:	4a98      	ldr	r2, [pc, #608]	; (3400 <main+0x158c>)
        radio_data_arr[1] = RADIO_BEACON_PERIOD;
    319e:	681b      	ldr	r3, [r3, #0]
    31a0:	6073      	str	r3, [r6, #4]
        radio_data_arr[0] = (RADIO_INITIAL_DATA_DAY << 24) | RADIO_INITIAL_DATA_TIME;
    31a2:	882b      	ldrh	r3, [r5, #0]
    31a4:	8812      	ldrh	r2, [r2, #0]
    31a6:	061b      	lsls	r3, r3, #24
    31a8:	4313      	orrs	r3, r2
    31aa:	6033      	str	r3, [r6, #0]
        send_beacon();
    31ac:	f7fe fdee 	bl	1d8c <send_beacon>
        radio_data_arr[2] = (0x14 << 8) | (0x2 << 6) | CHIP_ID;
    31b0:	23a4      	movs	r3, #164	; 0xa4
    31b2:	883a      	ldrh	r2, [r7, #0]
    31b4:	015b      	lsls	r3, r3, #5
    31b6:	4313      	orrs	r3, r2
    31b8:	60b3      	str	r3, [r6, #8]
        radio_data_arr[1] = (CHECK_PMU_NUM_UNITS << 20) | RADIO_REST_TIME;
    31ba:	4b92      	ldr	r3, [pc, #584]	; (3404 <main+0x1590>)
    31bc:	4a92      	ldr	r2, [pc, #584]	; (3408 <main+0x1594>)
    31be:	881b      	ldrh	r3, [r3, #0]
    31c0:	8812      	ldrh	r2, [r2, #0]
    31c2:	051b      	lsls	r3, r3, #20
    31c4:	4313      	orrs	r3, r2
    31c6:	6073      	str	r3, [r6, #4]
        radio_data_arr[0] = (RADIO_REST_NUM_UNITS << 20) | RADIO_DATA_PERIOD;
    31c8:	4b90      	ldr	r3, [pc, #576]	; (340c <main+0x1598>)
    31ca:	4a91      	ldr	r2, [pc, #580]	; (3410 <main+0x159c>)
    31cc:	881b      	ldrh	r3, [r3, #0]
    31ce:	8812      	ldrh	r2, [r2, #0]
    31d0:	051b      	lsls	r3, r3, #20
    31d2:	e6c9      	b.n	2f68 <main+0x10f4>
        else if(option == 2) {
    31d4:	2802      	cmp	r0, #2
    31d6:	d103      	bne.n	31e0 <main+0x136c>
            RADIO_SEQUENCE_PERIOD = N;
    31d8:	4886      	ldr	r0, [pc, #536]	; (33f4 <main+0x1580>)
    31da:	b29b      	uxth	r3, r3
            RADIO_DATA_PERIOD = N;
    31dc:	8003      	strh	r3, [r0, #0]
    31de:	e7c2      	b.n	3166 <main+0x12f2>
        else if(option == 3) {
    31e0:	2803      	cmp	r0, #3
    31e2:	d102      	bne.n	31ea <main+0x1376>
            RADIO_DUTY_DURATION = N;
    31e4:	b29b      	uxth	r3, r3
    31e6:	8013      	strh	r3, [r2, #0]
    31e8:	e7bd      	b.n	3166 <main+0x12f2>
        else if(option == 4) {
    31ea:	2804      	cmp	r0, #4
    31ec:	d102      	bne.n	31f4 <main+0x1380>
            RADIO_INITIAL_BEACON_TIME = N;
    31ee:	4882      	ldr	r0, [pc, #520]	; (33f8 <main+0x1584>)
            RADIO_BEACON_PERIOD = N;
    31f0:	6003      	str	r3, [r0, #0]
    31f2:	e7b8      	b.n	3166 <main+0x12f2>
        else if(option == 5) {
    31f4:	2805      	cmp	r0, #5
    31f6:	d101      	bne.n	31fc <main+0x1388>
            RADIO_BEACON_PERIOD = N;
    31f8:	4880      	ldr	r0, [pc, #512]	; (33fc <main+0x1588>)
    31fa:	e7f9      	b.n	31f0 <main+0x137c>
        else if(option == 6) {
    31fc:	2806      	cmp	r0, #6
    31fe:	d102      	bne.n	3206 <main+0x1392>
            RADIO_INITIAL_DATA_DAY = N;
    3200:	b29b      	uxth	r3, r3
    3202:	802b      	strh	r3, [r5, #0]
    3204:	e7af      	b.n	3166 <main+0x12f2>
        else if(option == 7) {
    3206:	2807      	cmp	r0, #7
    3208:	d102      	bne.n	3210 <main+0x139c>
            RADIO_INITIAL_DATA_TIME = N;
    320a:	487d      	ldr	r0, [pc, #500]	; (3400 <main+0x158c>)
    320c:	b29b      	uxth	r3, r3
    320e:	e7e5      	b.n	31dc <main+0x1368>
        else if(option == 8) {
    3210:	2808      	cmp	r0, #8
    3212:	d102      	bne.n	321a <main+0x13a6>
            CHECK_PMU_NUM_UNITS = N;
    3214:	487b      	ldr	r0, [pc, #492]	; (3404 <main+0x1590>)
    3216:	b29b      	uxth	r3, r3
    3218:	e7e0      	b.n	31dc <main+0x1368>
        else if(option == 9) {
    321a:	2809      	cmp	r0, #9
    321c:	d102      	bne.n	3224 <main+0x13b0>
            RADIO_REST_NUM_UNITS = N;
    321e:	487b      	ldr	r0, [pc, #492]	; (340c <main+0x1598>)
    3220:	b29b      	uxth	r3, r3
    3222:	e7db      	b.n	31dc <main+0x1368>
        else if(option == 10) {
    3224:	280a      	cmp	r0, #10
    3226:	d102      	bne.n	322e <main+0x13ba>
            RADIO_REST_TIME = N;
    3228:	4877      	ldr	r0, [pc, #476]	; (3408 <main+0x1594>)
    322a:	b29b      	uxth	r3, r3
    322c:	e7d6      	b.n	31dc <main+0x1368>
        else if(option == 11) {
    322e:	280b      	cmp	r0, #11
    3230:	d199      	bne.n	3166 <main+0x12f2>
            RADIO_DATA_PERIOD = N;
    3232:	4877      	ldr	r0, [pc, #476]	; (3410 <main+0x159c>)
    3234:	b29b      	uxth	r3, r3
    3236:	e7d1      	b.n	31dc <main+0x1368>
    else if(goc_data_header == 0x15) {
    3238:	2a15      	cmp	r2, #21
    323a:	d11c      	bne.n	3276 <main+0x1402>
        uint8_t option2 = (goc_data_full >> 22) & 0x1;
    323c:	681a      	ldr	r2, [r3, #0]
        if(option) {
    323e:	4875      	ldr	r0, [pc, #468]	; (3414 <main+0x15a0>)
    3240:	4975      	ldr	r1, [pc, #468]	; (3418 <main+0x15a4>)
    3242:	2d00      	cmp	r5, #0
    3244:	d00b      	beq.n	325e <main+0x13ea>
        uint8_t option2 = (goc_data_full >> 22) & 0x1;
    3246:	0d92      	lsrs	r2, r2, #22
    3248:	2501      	movs	r5, #1
            if(option2) {
    324a:	0016      	movs	r6, r2
    324c:	402e      	ands	r6, r5
    324e:	422a      	tst	r2, r5
    3250:	d00f      	beq.n	3272 <main+0x13fe>
                radio_debug = 1;
    3252:	7005      	strb	r5, [r0, #0]
                uint16_t N = goc_data_full & 0xFFFF;
    3254:	681a      	ldr	r2, [r3, #0]
                RADIO_DEBUG_PERIOD = N * 60;
    3256:	233c      	movs	r3, #60	; 0x3c
    3258:	b292      	uxth	r2, r2
    325a:	4353      	muls	r3, r2
    325c:	600b      	str	r3, [r1, #0]
        radio_data_arr[2] = (0x15 << 8) | CHIP_ID;
    325e:	22a8      	movs	r2, #168	; 0xa8
    3260:	4b5d      	ldr	r3, [pc, #372]	; (33d8 <main+0x1564>)
    3262:	0152      	lsls	r2, r2, #5
    3264:	881b      	ldrh	r3, [r3, #0]
    3266:	431a      	orrs	r2, r3
    3268:	4b5c      	ldr	r3, [pc, #368]	; (33dc <main+0x1568>)
    326a:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = radio_debug;
    326c:	7802      	ldrb	r2, [r0, #0]
    326e:	b2d2      	uxtb	r2, r2
    3270:	e646      	b.n	2f00 <main+0x108c>
                radio_debug = 0;
    3272:	7006      	strb	r6, [r0, #0]
    3274:	e7f3      	b.n	325e <main+0x13ea>
    else if(goc_data_header == 0x16) {
    3276:	2a16      	cmp	r2, #22
    3278:	d127      	bne.n	32ca <main+0x1456>
        uint32_t N = goc_data_full & 0x3FFFFF;
    327a:	681a      	ldr	r2, [r3, #0]
        uint8_t option2 = (goc_data_full >> 22) & 0x3;
    327c:	681b      	ldr	r3, [r3, #0]
        uint32_t N = goc_data_full & 0x3FFFFF;
    327e:	0292      	lsls	r2, r2, #10
        uint8_t option2 = (goc_data_full >> 22) & 0x3;
    3280:	021b      	lsls	r3, r3, #8
    3282:	4866      	ldr	r0, [pc, #408]	; (341c <main+0x15a8>)
    3284:	4966      	ldr	r1, [pc, #408]	; (3420 <main+0x15ac>)
    3286:	4d67      	ldr	r5, [pc, #412]	; (3424 <main+0x15b0>)
        uint32_t N = goc_data_full & 0x3FFFFF;
    3288:	0a92      	lsrs	r2, r2, #10
        uint8_t option2 = (goc_data_full >> 22) & 0x3;
    328a:	0f9b      	lsrs	r3, r3, #30
        if(option2 == 1) {
    328c:	2b01      	cmp	r3, #1
    328e:	d112      	bne.n	32b6 <main+0x1442>
            MRR_TEMP_THRESH_LOW = N;
    3290:	b292      	uxth	r2, r2
    3292:	8002      	strh	r2, [r0, #0]
        radio_data_arr[2] = (0x16 << 8) | CHIP_ID;
    3294:	4b50      	ldr	r3, [pc, #320]	; (33d8 <main+0x1564>)
    3296:	881a      	ldrh	r2, [r3, #0]
    3298:	23b0      	movs	r3, #176	; 0xb0
    329a:	015b      	lsls	r3, r3, #5
    329c:	4313      	orrs	r3, r2
    329e:	4a4f      	ldr	r2, [pc, #316]	; (33dc <main+0x1568>)
    32a0:	6093      	str	r3, [r2, #8]
        radio_data_arr[1] = (OVERRIDE_RAD << 16) | MRR_TEMP_THRESH_LOW;
    32a2:	882b      	ldrh	r3, [r5, #0]
    32a4:	8800      	ldrh	r0, [r0, #0]
    32a6:	041b      	lsls	r3, r3, #16
    32a8:	4303      	orrs	r3, r0
    32aa:	6053      	str	r3, [r2, #4]
        radio_data_arr[0] = MRR_TEMP_THRESH_HIGH;
    32ac:	880b      	ldrh	r3, [r1, #0]
    32ae:	b29b      	uxth	r3, r3
        radio_data_arr[0] = mrrv11a_r07.RO_MIM;
    32b0:	6013      	str	r3, [r2, #0]
        send_beacon();
    32b2:	f7ff fa60 	bl	2776 <main+0x902>
        else if(option2 == 2) {
    32b6:	2b02      	cmp	r3, #2
    32b8:	d102      	bne.n	32c0 <main+0x144c>
            MRR_TEMP_THRESH_HIGH = N;
    32ba:	b292      	uxth	r2, r2
    32bc:	800a      	strh	r2, [r1, #0]
    32be:	e7e9      	b.n	3294 <main+0x1420>
        else if(option2 == 3) {
    32c0:	2b03      	cmp	r3, #3
    32c2:	d1e7      	bne.n	3294 <main+0x1420>
            OVERRIDE_RAD = N;
    32c4:	b292      	uxth	r2, r2
    32c6:	802a      	strh	r2, [r5, #0]
    32c8:	e7e4      	b.n	3294 <main+0x1420>
    else if(goc_data_header == 0x17) {
    32ca:	2a17      	cmp	r2, #23
    32cc:	d10c      	bne.n	32e8 <main+0x1474>
        radio_data_arr[2] = (0x17 << 8) | CHIP_ID;
    32ce:	22b8      	movs	r2, #184	; 0xb8
    32d0:	4b41      	ldr	r3, [pc, #260]	; (33d8 <main+0x1564>)
    32d2:	0152      	lsls	r2, r2, #5
    32d4:	881b      	ldrh	r3, [r3, #0]
    32d6:	431a      	orrs	r2, r3
    32d8:	4b40      	ldr	r3, [pc, #256]	; (33dc <main+0x1568>)
    32da:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = error_code;
    32dc:	4a52      	ldr	r2, [pc, #328]	; (3428 <main+0x15b4>)
    32de:	6812      	ldr	r2, [r2, #0]
    32e0:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = error_time;
    32e2:	4a52      	ldr	r2, [pc, #328]	; (342c <main+0x15b8>)
            radio_data_arr[0] = *N;
    32e4:	6812      	ldr	r2, [r2, #0]
    32e6:	e70d      	b.n	3104 <main+0x1290>
    else if(goc_data_header == 0x18) {
    32e8:	2a18      	cmp	r2, #24
    32ea:	d10c      	bne.n	3306 <main+0x1492>
            radio_data_arr[2] = (0x18 << 8) | CHIP_ID;
    32ec:	21c0      	movs	r1, #192	; 0xc0
        uint8_t len = (goc_data_full >> 16) & 0xFF;
    32ee:	681a      	ldr	r2, [r3, #0]
        uint32_t* N = (uint32_t*) (goc_data_full & 0xFFFF);
    32f0:	681a      	ldr	r2, [r3, #0]
            radio_data_arr[2] = (0x18 << 8) | CHIP_ID;
    32f2:	4b39      	ldr	r3, [pc, #228]	; (33d8 <main+0x1564>)
    32f4:	0149      	lsls	r1, r1, #5
    32f6:	881b      	ldrh	r3, [r3, #0]
        uint32_t* N = (uint32_t*) (goc_data_full & 0xFFFF);
    32f8:	b292      	uxth	r2, r2
            radio_data_arr[2] = (0x18 << 8) | CHIP_ID;
    32fa:	4319      	orrs	r1, r3
    32fc:	4b37      	ldr	r3, [pc, #220]	; (33dc <main+0x1568>)
    32fe:	6099      	str	r1, [r3, #8]
            radio_data_arr[1] = 0;
    3300:	2100      	movs	r1, #0
    3302:	6059      	str	r1, [r3, #4]
            radio_data_arr[0] = *N;
    3304:	e7ee      	b.n	32e4 <main+0x1470>
    else if(goc_data_header == 0x19) {
    3306:	2a19      	cmp	r2, #25
    3308:	d120      	bne.n	334c <main+0x14d8>
        uint8_t option = (goc_data_full >> 22) & 0x3;
    330a:	6819      	ldr	r1, [r3, #0]
        uint8_t index = (goc_data_full >> 19) & 0x7;
    330c:	681a      	ldr	r2, [r3, #0]
        uint32_t N = goc_data_full & 0xFF;
    330e:	681b      	ldr	r3, [r3, #0]
        uint8_t index = (goc_data_full >> 19) & 0x7;
    3310:	0292      	lsls	r2, r2, #10
        uint8_t option = (goc_data_full >> 22) & 0x3;
    3312:	0209      	lsls	r1, r1, #8
        uint8_t index = (goc_data_full >> 19) & 0x7;
    3314:	0f52      	lsrs	r2, r2, #29
    3316:	4e46      	ldr	r6, [pc, #280]	; (3430 <main+0x15bc>)
    3318:	4d46      	ldr	r5, [pc, #280]	; (3434 <main+0x15c0>)
        uint8_t option = (goc_data_full >> 22) & 0x3;
    331a:	0f89      	lsrs	r1, r1, #30
        uint32_t N = goc_data_full & 0xFF;
    331c:	b2db      	uxtb	r3, r3
        if(option == 1) {
    331e:	0050      	lsls	r0, r2, #1
    3320:	2901      	cmp	r1, #1
    3322:	d10e      	bne.n	3342 <main+0x14ce>
            LOW_PWR_LOW_ADC_THRESH[index] = N;
    3324:	b29b      	uxth	r3, r3
    3326:	5233      	strh	r3, [r6, r0]
        radio_data_arr[2] = (0x19 << 8) | CHIP_ID;
    3328:	4b2b      	ldr	r3, [pc, #172]	; (33d8 <main+0x1564>)
    332a:	8819      	ldrh	r1, [r3, #0]
    332c:	23c8      	movs	r3, #200	; 0xc8
    332e:	015b      	lsls	r3, r3, #5
    3330:	430b      	orrs	r3, r1
    3332:	492a      	ldr	r1, [pc, #168]	; (33dc <main+0x1568>)
    3334:	608b      	str	r3, [r1, #8]
        radio_data_arr[1] = index;
    3336:	604a      	str	r2, [r1, #4]
        radio_data_arr[0] = (LOW_PWR_LOW_ADC_THRESH[index] << 8) | LOW_PWR_HIGH_ADC_THRESH[index];
    3338:	5a33      	ldrh	r3, [r6, r0]
    333a:	5a2a      	ldrh	r2, [r5, r0]
    333c:	021b      	lsls	r3, r3, #8
    333e:	4313      	orrs	r3, r2
    3340:	e50e      	b.n	2d60 <main+0xeec>
        else if(option == 2) {
    3342:	2902      	cmp	r1, #2
    3344:	d1f0      	bne.n	3328 <main+0x14b4>
            LOW_PWR_HIGH_ADC_THRESH[index] = N;
    3346:	b29b      	uxth	r3, r3
    3348:	522b      	strh	r3, [r5, r0]
    334a:	e7ed      	b.n	3328 <main+0x14b4>
    else if(goc_data_header == 0x1A) {
    334c:	2a1a      	cmp	r2, #26
    334e:	d000      	beq.n	3352 <main+0x14de>
    3350:	e080      	b.n	3454 <main+0x15e0>
        if(option) {
    3352:	4e39      	ldr	r6, [pc, #228]	; (3438 <main+0x15c4>)
    3354:	2d00      	cmp	r5, #0
    3356:	d02f      	beq.n	33b8 <main+0x1544>
            mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
    3358:	273f      	movs	r7, #63	; 0x3f
            mrr_trx_cap_antn_tune_coarse = goc_data_full & 0x3FF;
    335a:	681b      	ldr	r3, [r3, #0]
            mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = mrr_trx_cap_antn_tune_coarse;  //ANT CAP 10b unary 830.5 MHz // TODO: make them the same variable
    335c:	4937      	ldr	r1, [pc, #220]	; (343c <main+0x15c8>)
            mrr_trx_cap_antn_tune_coarse = goc_data_full & 0x3FF;
    335e:	059b      	lsls	r3, r3, #22
    3360:	0d9b      	lsrs	r3, r3, #22
    3362:	8033      	strh	r3, [r6, #0]
            mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = mrr_trx_cap_antn_tune_coarse;  //ANT CAP 10b unary 830.5 MHz // TODO: make them the same variable
    3364:	680a      	ldr	r2, [r1, #0]
    3366:	4836      	ldr	r0, [pc, #216]	; (3440 <main+0x15cc>)
    3368:	01db      	lsls	r3, r3, #7
    336a:	4002      	ands	r2, r0
    336c:	4313      	orrs	r3, r2
    336e:	600b      	str	r3, [r1, #0]
            mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    3370:	680a      	ldr	r2, [r1, #0]
    3372:	2002      	movs	r0, #2
    3374:	2100      	movs	r1, #0
    3376:	f7fc ff40 	bl	1fa <mbus_remote_register_write>
            mrrv11a_r01.TRX_CAP_ANTN_TUNE_COARSE = mrr_trx_cap_antn_tune_coarse; //ANT CAP 10b unary 830.5 MHz
    337a:	4a32      	ldr	r2, [pc, #200]	; (3444 <main+0x15d0>)
    337c:	8831      	ldrh	r1, [r6, #0]
    337e:	6813      	ldr	r3, [r2, #0]
    3380:	0589      	lsls	r1, r1, #22
    3382:	0a9b      	lsrs	r3, r3, #10
    3384:	0d89      	lsrs	r1, r1, #22
    3386:	029b      	lsls	r3, r3, #10
    3388:	430b      	orrs	r3, r1
    338a:	6013      	str	r3, [r2, #0]
            mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
    338c:	4b2e      	ldr	r3, [pc, #184]	; (3448 <main+0x15d4>)
    338e:	492f      	ldr	r1, [pc, #188]	; (344c <main+0x15d8>)
    3390:	881d      	ldrh	r5, [r3, #0]
    3392:	6810      	ldr	r0, [r2, #0]
    3394:	403d      	ands	r5, r7
    3396:	4008      	ands	r0, r1
    3398:	02ad      	lsls	r5, r5, #10
    339a:	4328      	orrs	r0, r5
    339c:	6010      	str	r0, [r2, #0]
            mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
    339e:	8819      	ldrh	r1, [r3, #0]
    33a0:	482b      	ldr	r0, [pc, #172]	; (3450 <main+0x15dc>)
    33a2:	6813      	ldr	r3, [r2, #0]
    33a4:	4039      	ands	r1, r7
    33a6:	0409      	lsls	r1, r1, #16
    33a8:	4003      	ands	r3, r0
    33aa:	430b      	orrs	r3, r1
    33ac:	6013      	str	r3, [r2, #0]
            mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
    33ae:	2101      	movs	r1, #1
    33b0:	2002      	movs	r0, #2
    33b2:	6812      	ldr	r2, [r2, #0]
    33b4:	f7fc ff21 	bl	1fa <mbus_remote_register_write>
        radio_data_arr[2] = (0x1A << 8) | CHIP_ID;
    33b8:	22d0      	movs	r2, #208	; 0xd0
    33ba:	4b07      	ldr	r3, [pc, #28]	; (33d8 <main+0x1564>)
    33bc:	0152      	lsls	r2, r2, #5
    33be:	881b      	ldrh	r3, [r3, #0]
    33c0:	431a      	orrs	r2, r3
    33c2:	4b06      	ldr	r3, [pc, #24]	; (33dc <main+0x1568>)
    33c4:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = 0;
    33c6:	2200      	movs	r2, #0
    33c8:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = mrr_trx_cap_antn_tune_coarse;
    33ca:	8832      	ldrh	r2, [r6, #0]
    33cc:	e69a      	b.n	3104 <main+0x1290>
    33ce:	46c0      	nop			; (mov r8, r8)
    33d0:	00003958 	.word	0x00003958
    33d4:	000038e0 	.word	0x000038e0
    33d8:	00003a00 	.word	0x00003a00
    33dc:	000039b8 	.word	0x000039b8
    33e0:	000038ea 	.word	0x000038ea
    33e4:	000037c8 	.word	0x000037c8
    33e8:	000038c4 	.word	0x000038c4
    33ec:	000038d4 	.word	0x000038d4
    33f0:	000038dc 	.word	0x000038dc
    33f4:	000038e8 	.word	0x000038e8
    33f8:	000038d8 	.word	0x000038d8
    33fc:	000038c8 	.word	0x000038c8
    3400:	000038de 	.word	0x000038de
    3404:	000037ba 	.word	0x000037ba
    3408:	000038e6 	.word	0x000038e6
    340c:	000038e4 	.word	0x000038e4
    3410:	000038cc 	.word	0x000038cc
    3414:	00003a8c 	.word	0x00003a8c
    3418:	000038d0 	.word	0x000038d0
    341c:	000037ee 	.word	0x000037ee
    3420:	000037ec 	.word	0x000037ec
    3424:	00003a02 	.word	0x00003a02
    3428:	00003a24 	.word	0x00003a24
    342c:	00003a28 	.word	0x00003a28
    3430:	000037dc 	.word	0x000037dc
    3434:	000037ce 	.word	0x000037ce
    3438:	00003a5e 	.word	0x00003a5e
    343c:	0000390c 	.word	0x0000390c
    3440:	fffe007f 	.word	0xfffe007f
    3444:	00003a60 	.word	0x00003a60
    3448:	00003954 	.word	0x00003954
    344c:	ffff03ff 	.word	0xffff03ff
    3450:	ffc0ffff 	.word	0xffc0ffff
    else if(goc_data_header == 0x1B) {
    3454:	2a1b      	cmp	r2, #27
    3456:	d120      	bne.n	349a <main+0x1626>
        uint8_t option = (goc_data_full >> 22) & 0x3;
    3458:	681a      	ldr	r2, [r3, #0]
        uint32_t N = goc_data_full & 0x3FFFF;
    345a:	681b      	ldr	r3, [r3, #0]
        uint8_t option = (goc_data_full >> 22) & 0x3;
    345c:	0212      	lsls	r2, r2, #8
        uint32_t N = goc_data_full & 0x3FFFF;
    345e:	039b      	lsls	r3, r3, #14
    3460:	4d6f      	ldr	r5, [pc, #444]	; (3620 <main+0x17ac>)
    3462:	4870      	ldr	r0, [pc, #448]	; (3624 <main+0x17b0>)
    3464:	4970      	ldr	r1, [pc, #448]	; (3628 <main+0x17b4>)
        uint8_t option = (goc_data_full >> 22) & 0x3;
    3466:	0f92      	lsrs	r2, r2, #30
        uint32_t N = goc_data_full & 0x3FFFF;
    3468:	0b9b      	lsrs	r3, r3, #14
        if(option == 1) {
    346a:	2a01      	cmp	r2, #1
    346c:	d10d      	bne.n	348a <main+0x1616>
            start_day_count = N;
    346e:	602b      	str	r3, [r5, #0]
        radio_data_arr[2] = (0x1B << 8) | CHIP_ID;
    3470:	4b6e      	ldr	r3, [pc, #440]	; (362c <main+0x17b8>)
    3472:	881a      	ldrh	r2, [r3, #0]
    3474:	23d8      	movs	r3, #216	; 0xd8
    3476:	015b      	lsls	r3, r3, #5
    3478:	4313      	orrs	r3, r2
    347a:	4a6d      	ldr	r2, [pc, #436]	; (3630 <main+0x17bc>)
    347c:	6093      	str	r3, [r2, #8]
        radio_data_arr[1] = start_day_count;
    347e:	682b      	ldr	r3, [r5, #0]
    3480:	6053      	str	r3, [r2, #4]
        radio_data_arr[0] = (end_day_count << 16) | radio_day_count;
    3482:	6803      	ldr	r3, [r0, #0]
    3484:	6809      	ldr	r1, [r1, #0]
    3486:	041b      	lsls	r3, r3, #16
    3488:	e4fc      	b.n	2e84 <main+0x1010>
        else if(option == 2) {
    348a:	2a02      	cmp	r2, #2
    348c:	d101      	bne.n	3492 <main+0x161e>
            end_day_count = N;
    348e:	6003      	str	r3, [r0, #0]
    3490:	e7ee      	b.n	3470 <main+0x15fc>
        else if(option == 3) {
    3492:	2a03      	cmp	r2, #3
    3494:	d1ec      	bne.n	3470 <main+0x15fc>
            radio_day_count = N;
    3496:	600b      	str	r3, [r1, #0]
    3498:	e7ea      	b.n	3470 <main+0x15fc>
    else if(goc_data_header == 0x1C) {
    349a:	2a1c      	cmp	r2, #28
    349c:	d12b      	bne.n	34f6 <main+0x1682>
        uint16_t N1 = (goc_data_full >> 12) & 0xFFF;
    349e:	681e      	ldr	r6, [r3, #0]
        uint16_t N2 = goc_data_full & 0xFFF;
    34a0:	681b      	ldr	r3, [r3, #0]
    34a2:	4f64      	ldr	r7, [pc, #400]	; (3634 <main+0x17c0>)
    34a4:	051b      	lsls	r3, r3, #20
    34a6:	0d1a      	lsrs	r2, r3, #20
    34a8:	9201      	str	r2, [sp, #4]
        if(!N2) {
    34aa:	2b00      	cmp	r3, #0
    34ac:	d10c      	bne.n	34c8 <main+0x1654>
            radio_data_arr[2] = (0x1C << 8) | CHIP_ID;
    34ae:	22e0      	movs	r2, #224	; 0xe0
    34b0:	4b5e      	ldr	r3, [pc, #376]	; (362c <main+0x17b8>)
    34b2:	0152      	lsls	r2, r2, #5
    34b4:	881b      	ldrh	r3, [r3, #0]
    34b6:	8839      	ldrh	r1, [r7, #0]
    34b8:	431a      	orrs	r2, r3
    34ba:	4b5d      	ldr	r3, [pc, #372]	; (3630 <main+0x17bc>)
    34bc:	609a      	str	r2, [r3, #8]
            radio_data_arr[1] = 0;
    34be:	2200      	movs	r2, #0
    34c0:	605a      	str	r2, [r3, #4]
            radio_data_arr[0] = max_unit_count;
    34c2:	6019      	str	r1, [r3, #0]
            send_beacon();
    34c4:	f7ff f957 	bl	2776 <main+0x902>
            light_packet_num = 0;
    34c8:	2500      	movs	r5, #0
    34ca:	4b5b      	ldr	r3, [pc, #364]	; (3638 <main+0x17c4>)
        uint16_t N1 = (goc_data_full >> 12) & 0xFFF;
    34cc:	0236      	lsls	r6, r6, #8
            light_packet_num = 0;
    34ce:	801d      	strh	r5, [r3, #0]
            temp_packet_num = 0;
    34d0:	4b5a      	ldr	r3, [pc, #360]	; (363c <main+0x17c8>)
        uint16_t N1 = (goc_data_full >> 12) & 0xFFF;
    34d2:	0d36      	lsrs	r6, r6, #20
            temp_packet_num = 0;
    34d4:	801d      	strh	r5, [r3, #0]
                if(N1 + i >= max_unit_count) {
    34d6:	883b      	ldrh	r3, [r7, #0]
    34d8:	1970      	adds	r0, r6, r5
    34da:	4283      	cmp	r3, r0
    34dc:	dc01      	bgt.n	34e2 <main+0x166e>
    34de:	f7ff f8af 	bl	2640 <main+0x7cc>
                radio_unit(N1 + i);
    34e2:	b280      	uxth	r0, r0
    34e4:	f7fe fb70 	bl	1bc8 <radio_unit>
            for(i = 0; i < N2; i++) {
    34e8:	3501      	adds	r5, #1
    34ea:	9a01      	ldr	r2, [sp, #4]
    34ec:	b2ab      	uxth	r3, r5
    34ee:	4293      	cmp	r3, r2
    34f0:	d3f1      	bcc.n	34d6 <main+0x1662>
    34f2:	f7ff f8a5 	bl	2640 <main+0x7cc>
    else if(goc_data_header == 0x1D) {
    34f6:	2a1d      	cmp	r2, #29
    34f8:	d111      	bne.n	351e <main+0x16aa>
        uint16_t N = goc_data_full & 0x7FF;
    34fa:	681b      	ldr	r3, [r3, #0]
        if(option) {
    34fc:	4950      	ldr	r1, [pc, #320]	; (3640 <main+0x17cc>)
    34fe:	2d00      	cmp	r5, #0
    3500:	d004      	beq.n	350c <main+0x1698>
        uint16_t N = goc_data_full & 0x7FF;
    3502:	055b      	lsls	r3, r3, #21
            data_collection_end_day_time = N * 60;
    3504:	0d5b      	lsrs	r3, r3, #21
    3506:	321f      	adds	r2, #31
    3508:	4353      	muls	r3, r2
    350a:	600b      	str	r3, [r1, #0]
        radio_data_arr[2] = (0x1D << 8) | CHIP_ID;
    350c:	22e8      	movs	r2, #232	; 0xe8
    350e:	4b47      	ldr	r3, [pc, #284]	; (362c <main+0x17b8>)
    3510:	0152      	lsls	r2, r2, #5
    3512:	881b      	ldrh	r3, [r3, #0]
    3514:	431a      	orrs	r2, r3
    3516:	4b46      	ldr	r3, [pc, #280]	; (3630 <main+0x17bc>)
    3518:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = 0;
    351a:	2200      	movs	r2, #0
    351c:	e4f0      	b.n	2f00 <main+0x108c>
    else if(goc_data_header == 0x1E) {
    351e:	2a1e      	cmp	r2, #30
    3520:	d128      	bne.n	3574 <main+0x1700>
        uint16_t N = (goc_data_full >> 8) & 0xFF;
    3522:	4b48      	ldr	r3, [pc, #288]	; (3644 <main+0x17d0>)
    3524:	4e48      	ldr	r6, [pc, #288]	; (3648 <main+0x17d4>)
    3526:	6819      	ldr	r1, [r3, #0]
        uint16_t M = goc_data_full & 0xFF;
    3528:	681b      	ldr	r3, [r3, #0]
        if(option) {
    352a:	2d00      	cmp	r5, #0
    352c:	d013      	beq.n	3556 <main+0x16e2>
            mrrv11a_r07.RO_MOM = N;
    352e:	3261      	adds	r2, #97	; 0x61
        uint16_t N = (goc_data_full >> 8) & 0xFF;
    3530:	0a09      	lsrs	r1, r1, #8
            mrrv11a_r07.RO_MOM = N;
    3532:	4011      	ands	r1, r2
    3534:	0008      	movs	r0, r1
    3536:	8831      	ldrh	r1, [r6, #0]
    3538:	4391      	bics	r1, r2
    353a:	4301      	orrs	r1, r0
    353c:	8031      	strh	r1, [r6, #0]
            mrrv11a_r07.RO_MIM = M;
    353e:	401a      	ands	r2, r3
    3540:	4942      	ldr	r1, [pc, #264]	; (364c <main+0x17d8>)
    3542:	8833      	ldrh	r3, [r6, #0]
    3544:	01d2      	lsls	r2, r2, #7
    3546:	400b      	ands	r3, r1
    3548:	4313      	orrs	r3, r2
    354a:	8033      	strh	r3, [r6, #0]
            mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);
    354c:	2107      	movs	r1, #7
    354e:	2002      	movs	r0, #2
    3550:	6832      	ldr	r2, [r6, #0]
    3552:	f7fc fe52 	bl	1fa <mbus_remote_register_write>
        radio_data_arr[2] = (0x1E << 8) | CHIP_ID;
    3556:	4b35      	ldr	r3, [pc, #212]	; (362c <main+0x17b8>)
    3558:	881a      	ldrh	r2, [r3, #0]
    355a:	23f0      	movs	r3, #240	; 0xf0
    355c:	015b      	lsls	r3, r3, #5
    355e:	4313      	orrs	r3, r2
    3560:	4a33      	ldr	r2, [pc, #204]	; (3630 <main+0x17bc>)
    3562:	6093      	str	r3, [r2, #8]
        radio_data_arr[1] = mrrv11a_r07.RO_MOM;
    3564:	6833      	ldr	r3, [r6, #0]
    3566:	065b      	lsls	r3, r3, #25
    3568:	0e5b      	lsrs	r3, r3, #25
    356a:	6053      	str	r3, [r2, #4]
        radio_data_arr[0] = mrrv11a_r07.RO_MIM;
    356c:	6833      	ldr	r3, [r6, #0]
    356e:	049b      	lsls	r3, r3, #18
    3570:	0e5b      	lsrs	r3, r3, #25
    3572:	e69d      	b.n	32b0 <main+0x143c>
    else if(goc_data_header == 0x1F) {
    3574:	2a1f      	cmp	r2, #31
    3576:	d001      	beq.n	357c <main+0x1708>
    3578:	f7ff f862 	bl	2640 <main+0x7cc>
        *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    357c:	2600      	movs	r6, #0
        uint32_t N = (goc_data_full & 0xFFFF) << XO_TO_SEC_SHIFT;
    357e:	4b31      	ldr	r3, [pc, #196]	; (3644 <main+0x17d0>)
        *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
    3580:	4a33      	ldr	r2, [pc, #204]	; (3650 <main+0x17dc>)
        uint32_t N = (goc_data_full & 0xFFFF) << XO_TO_SEC_SHIFT;
    3582:	681d      	ldr	r5, [r3, #0]
    3584:	4b33      	ldr	r3, [pc, #204]	; (3654 <main+0x17e0>)
    3586:	03ed      	lsls	r5, r5, #15
    3588:	401d      	ands	r5, r3
        *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
    358a:	4b33      	ldr	r3, [pc, #204]	; (3658 <main+0x17e4>)
        delay(10000);
    358c:	4833      	ldr	r0, [pc, #204]	; (365c <main+0x17e8>)
        *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
    358e:	601a      	str	r2, [r3, #0]
        *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    3590:	4b33      	ldr	r3, [pc, #204]	; (3660 <main+0x17ec>)
    3592:	601e      	str	r6, [r3, #0]
        *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    3594:	4b33      	ldr	r3, [pc, #204]	; (3664 <main+0x17f0>)
    3596:	601e      	str	r6, [r3, #0]
        delay(10000);
    3598:	f7fc fd86 	bl	a8 <delay>
        stop_xo_cnt();
    359c:	f7fc fddc 	bl	158 <stop_xo_cnt>
        reset_xo_cnt();
    35a0:	f7fc fdce 	bl	140 <reset_xo_cnt>
        set_xo_timer(0, N, 0, 1);
    35a4:	2301      	movs	r3, #1
    35a6:	0032      	movs	r2, r6
    35a8:	0029      	movs	r1, r5
    35aa:	0030      	movs	r0, r6
    35ac:	f7fc fdaa 	bl	104 <set_xo_timer>
        asm("wfi;");
    35b0:	bf30      	wfi
        pmu_current_marker();
    35b2:	f7fc ff1d 	bl	3f0 <pmu_current_marker>
        while(diff >= (1 << 13)) {
    35b6:	2180      	movs	r1, #128	; 0x80
        uint32_t xot_val = *XOT_VAL;
    35b8:	4b2b      	ldr	r3, [pc, #172]	; (3668 <main+0x17f4>)
        while(diff >= (1 << 13)) {
    35ba:	0189      	lsls	r1, r1, #6
        uint32_t xot_val = *XOT_VAL;
    35bc:	681a      	ldr	r2, [r3, #0]
        uint32_t target = xot_val + N;
    35be:	18ad      	adds	r5, r5, r2
            xot_val = *XOT_VAL;
    35c0:	681a      	ldr	r2, [r3, #0]
            diff = xot_val - target;    // this will be a large positive number if xot_val is less then target
    35c2:	1b52      	subs	r2, r2, r5
        while(diff >= (1 << 13)) {
    35c4:	428a      	cmp	r2, r1
    35c6:	d2fb      	bcs.n	35c0 <main+0x174c>
        pmu_current_marker();
    35c8:	f7fc ff12 	bl	3f0 <pmu_current_marker>
    35cc:	f7ff f838 	bl	2640 <main+0x7cc>
    if(count == 5) {
    35d0:	2f05      	cmp	r7, #5
    35d2:	d101      	bne.n	35d8 <main+0x1764>
    35d4:	f7ff f8a8 	bl	2728 <main+0x8b4>
    if(error_code == 5) {
    35d8:	4924      	ldr	r1, [pc, #144]	; (366c <main+0x17f8>)
    35da:	680b      	ldr	r3, [r1, #0]
    35dc:	2b05      	cmp	r3, #5
    35de:	d112      	bne.n	3606 <main+0x1792>
        radio_data_arr[2] = 0xFF00 | CHIP_ID;
    35e0:	4b12      	ldr	r3, [pc, #72]	; (362c <main+0x17b8>)
        radio_data_arr[0] = lnt_state;
    35e2:	240a      	movs	r4, #10
        radio_data_arr[2] = 0xFF00 | CHIP_ID;
    35e4:	881a      	ldrh	r2, [r3, #0]
    35e6:	23ff      	movs	r3, #255	; 0xff
    35e8:	021b      	lsls	r3, r3, #8
    35ea:	431a      	orrs	r2, r3
    35ec:	4b10      	ldr	r3, [pc, #64]	; (3630 <main+0x17bc>)
    35ee:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = error_code;
    35f0:	680a      	ldr	r2, [r1, #0]
    35f2:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = lnt_state;
    35f4:	601d      	str	r5, [r3, #0]
            delay(10000);
    35f6:	4819      	ldr	r0, [pc, #100]	; (365c <main+0x17e8>)
    35f8:	3c01      	subs	r4, #1
    35fa:	f7fc fd55 	bl	a8 <delay>
            send_beacon();
    35fe:	f7fe fbc5 	bl	1d8c <send_beacon>
        for(i = 0; i < 10; i++) {
    3602:	2c00      	cmp	r4, #0
    3604:	d1f7      	bne.n	35f6 <main+0x1782>
        set_system_error(0x01);
    }
    


    last_xo_sys_time = xo_sys_time;
    3606:	4b1a      	ldr	r3, [pc, #104]	; (3670 <main+0x17fc>)

    pmu_setting_temp_based(2);
    3608:	2002      	movs	r0, #2
    last_xo_sys_time = xo_sys_time;
    360a:	681a      	ldr	r2, [r3, #0]
    360c:	4b19      	ldr	r3, [pc, #100]	; (3674 <main+0x1800>)
    360e:	601a      	str	r2, [r3, #0]
    pmu_setting_temp_based(2);
    3610:	f7fd ff80 	bl	1514 <pmu_setting_temp_based>
    operation_sleep();
    3614:	f7fe fedf 	bl	23d6 <main+0x562>
        set_system_error(0x01);
    3618:	2001      	movs	r0, #1
    361a:	f7fd ffb5 	bl	1588 <set_system_error>
    361e:	e7f2      	b.n	3606 <main+0x1792>
    3620:	00003ad0 	.word	0x00003ad0
    3624:	00003a1c 	.word	0x00003a1c
    3628:	00003a88 	.word	0x00003a88
    362c:	00003a00 	.word	0x00003a00
    3630:	000039b8 	.word	0x000039b8
    3634:	00003a5a 	.word	0x00003a5a
    3638:	00003a3c 	.word	0x00003a3c
    363c:	00003ae0 	.word	0x00003ae0
    3640:	000038f4 	.word	0x000038f4
    3644:	000039f8 	.word	0x000039f8
    3648:	00003918 	.word	0x00003918
    364c:	ffffc07f 	.word	0xffffc07f
    3650:	00100f0d 	.word	0x00100f0d
    3654:	7fff8000 	.word	0x7fff8000
    3658:	e000e100 	.word	0xe000e100
    365c:	00002710 	.word	0x00002710
    3660:	a0001200 	.word	0xa0001200
    3664:	a000007c 	.word	0xa000007c
    3668:	a0001420 	.word	0xa0001420
    366c:	00003a24 	.word	0x00003a24
    3670:	00003ae8 	.word	0x00003ae8
    3674:	00003a34 	.word	0x00003a34
