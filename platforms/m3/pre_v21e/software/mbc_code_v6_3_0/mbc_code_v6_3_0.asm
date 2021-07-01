
mbc_code_v6_3_0/mbc_code_v6_3_0.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0xa0>:
   0:	00004000 	.word	0x00004000
   4:	000000a3 	.word	0x000000a3
	...
  10:	000000a0 	.word	0x000000a0
  14:	000000a0 	.word	0x000000a0
  18:	000000a0 	.word	0x000000a0
  1c:	000000a0 	.word	0x000000a0
  20:	000000a0 	.word	0x000000a0
  24:	000000a0 	.word	0x000000a0
  28:	000000a0 	.word	0x000000a0
  2c:	00000000 	.word	0x00000000
  30:	000000a0 	.word	0x000000a0
  34:	000000a0 	.word	0x000000a0
	...
  40:	00001b3d 	.word	0x00001b3d
  44:	00000000 	.word	0x00000000
  48:	00001b49 	.word	0x00001b49
  4c:	00001b55 	.word	0x00001b55
	...
  60:	00001b85 	.word	0x00001b85
  64:	00001b95 	.word	0x00001b95
  68:	00001ba5 	.word	0x00001ba5
  6c:	00001bb5 	.word	0x00001bb5
	...
  90:	00001b75 	.word	0x00001b75
	...

000000a0 <hang>:
  a0:	e7fe      	b.n	a0 <hang>

000000a2 <_start>:
  a2:	f001 fedd 	bl	1e60 <main>
  a6:	e7fc      	b.n	a2 <_start>

Disassembly of section .text.mbus_write_message32:

000000a8 <mbus_write_message32>:
	return 1;
}

uint32_t mbus_write_message32(uint8_t addr, uint32_t data) {
    uint32_t mbus_addr = 0xA0003000 | (addr << 4);
    *((volatile uint32_t *) mbus_addr) = data;
  a8:	4b02      	ldr	r3, [pc, #8]	; (b4 <mbus_write_message32+0xc>)
  aa:	0100      	lsls	r0, r0, #4
  ac:	4318      	orrs	r0, r3
  ae:	6001      	str	r1, [r0, #0]
    return 1;
}
  b0:	2001      	movs	r0, #1
  b2:	4770      	bx	lr
  b4:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_write_message:

000000b8 <mbus_write_message>:

uint32_t mbus_write_message(uint8_t addr, uint32_t data[], unsigned len) {
	// Goal: Use the "Memory Stream Write" to put unconstrained 32-bit data
	//       onto the bus.
	if (len == 0) return 0;
  b8:	2300      	movs	r3, #0
  ba:	429a      	cmp	r2, r3
  bc:	d00a      	beq.n	d4 <mbus_write_message+0x1c>

	*MBUS_CMD0 = (addr << 24) | (len-1);
  be:	4b06      	ldr	r3, [pc, #24]	; (d8 <mbus_write_message+0x20>)
  c0:	0600      	lsls	r0, r0, #24
  c2:	3a01      	subs	r2, #1
  c4:	4302      	orrs	r2, r0
  c6:	601a      	str	r2, [r3, #0]
	*MBUS_CMD1 = (uint32_t) data;
	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x2 << 4);
  c8:	2223      	movs	r2, #35	; 0x23
	// Goal: Use the "Memory Stream Write" to put unconstrained 32-bit data
	//       onto the bus.
	if (len == 0) return 0;

	*MBUS_CMD0 = (addr << 24) | (len-1);
	*MBUS_CMD1 = (uint32_t) data;
  ca:	4b04      	ldr	r3, [pc, #16]	; (dc <mbus_write_message+0x24>)
  cc:	6019      	str	r1, [r3, #0]
	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x2 << 4);
  ce:	4b04      	ldr	r3, [pc, #16]	; (e0 <mbus_write_message+0x28>)
  d0:	601a      	str	r2, [r3, #0]
  d2:	2301      	movs	r3, #1

    return 1;
}
  d4:	0018      	movs	r0, r3
  d6:	4770      	bx	lr
  d8:	a0002000 	.word	0xa0002000
  dc:	a0002004 	.word	0xa0002004
  e0:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_enumerate:

000000e4 <mbus_enumerate>:

void mbus_enumerate(unsigned new_prefix) {
    mbus_write_message32(MBUS_DISC_AND_ENUM, ((MBUS_ENUMERATE_CMD << 28) | (new_prefix << 24)));
  e4:	2380      	movs	r3, #128	; 0x80
  e6:	0600      	lsls	r0, r0, #24
  e8:	059b      	lsls	r3, r3, #22
  ea:	4318      	orrs	r0, r3
	return 1;
}

uint32_t mbus_write_message32(uint8_t addr, uint32_t data) {
    uint32_t mbus_addr = 0xA0003000 | (addr << 4);
    *((volatile uint32_t *) mbus_addr) = data;
  ec:	4b01      	ldr	r3, [pc, #4]	; (f4 <mbus_enumerate+0x10>)
  ee:	6018      	str	r0, [r3, #0]
    return 1;
}

void mbus_enumerate(unsigned new_prefix) {
    mbus_write_message32(MBUS_DISC_AND_ENUM, ((MBUS_ENUMERATE_CMD << 28) | (new_prefix << 24)));
}
  f0:	4770      	bx	lr
  f2:	46c0      	nop			; (mov r8, r8)
  f4:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

000000f8 <mbus_sleep_all>:
	return 1;
}

uint32_t mbus_write_message32(uint8_t addr, uint32_t data) {
    uint32_t mbus_addr = 0xA0003000 | (addr << 4);
    *((volatile uint32_t *) mbus_addr) = data;
  f8:	2200      	movs	r2, #0
  fa:	4b01      	ldr	r3, [pc, #4]	; (100 <mbus_sleep_all+0x8>)
  fc:	601a      	str	r2, [r3, #0]
    mbus_write_message32(MBUS_DISC_AND_ENUM, ((MBUS_ENUMERATE_CMD << 28) | (new_prefix << 24)));
}

void mbus_sleep_all(void) {
    mbus_write_message32(MBUS_POWER, MBUS_ALL_SLEEP << 28);
}
  fe:	4770      	bx	lr
 100:	a0003010 	.word	0xa0003010

Disassembly of section .text.mbus_copy_registers_from_remote_to_local:

00000104 <mbus_copy_registers_from_remote_to_local>:
void mbus_copy_registers_from_remote_to_local(
		uint8_t remote_prefix,
		uint8_t remote_reg_start,
		uint8_t local_reg_start,
		uint8_t length_minus_one
		) {
 104:	b513      	push	{r0, r1, r4, lr}
	// Put a register read command on the bus instructed to write this node

	uint8_t address = ((remote_prefix & 0xf) << 4) | MPQ_REG_READ;
	uint32_t data = 
 106:	2480      	movs	r4, #128	; 0x80
 108:	0164      	lsls	r4, r4, #5
 10a:	041b      	lsls	r3, r3, #16
 10c:	4322      	orrs	r2, r4
 10e:	431a      	orrs	r2, r3
 110:	0609      	lsls	r1, r1, #24
 112:	4311      	orrs	r1, r2
		(length_minus_one << 16) |
		(mbus_get_short_prefix() << 12) |
		(MPQ_REG_WRITE << 8) | // Write regs *to* _this_ node
		(local_reg_start << 0);

	mbus_write_message(address, &data, 1);
 114:	2201      	movs	r2, #1
 116:	0100      	lsls	r0, r0, #4
 118:	4310      	orrs	r0, r2
		uint8_t length_minus_one
		) {
	// Put a register read command on the bus instructed to write this node

	uint8_t address = ((remote_prefix & 0xf) << 4) | MPQ_REG_READ;
	uint32_t data = 
 11a:	9101      	str	r1, [sp, #4]
		(length_minus_one << 16) |
		(mbus_get_short_prefix() << 12) |
		(MPQ_REG_WRITE << 8) | // Write regs *to* _this_ node
		(local_reg_start << 0);

	mbus_write_message(address, &data, 1);
 11c:	b2c0      	uxtb	r0, r0
 11e:	a901      	add	r1, sp, #4
 120:	f7ff ffca 	bl	b8 <mbus_write_message>
}
 124:	bd13      	pop	{r0, r1, r4, pc}

Disassembly of section .text.mbus_remote_register_write:

00000126 <mbus_remote_register_write>:

void mbus_remote_register_write(
		uint8_t prefix,
		uint8_t dst_reg_addr,
		uint32_t dst_reg_val
		) {
 126:	b507      	push	{r0, r1, r2, lr}
	// assert (prefix < 16 && > 0);
	uint8_t address = ((prefix & 0xf) << 4) | MPQ_REG_WRITE;
	uint32_t data = (dst_reg_addr << 24) | (dst_reg_val & 0xffffff);
 128:	060b      	lsls	r3, r1, #24
 12a:	0211      	lsls	r1, r2, #8
 12c:	0a09      	lsrs	r1, r1, #8
 12e:	4319      	orrs	r1, r3
	mbus_write_message(address, &data, 1);
 130:	0100      	lsls	r0, r0, #4
		uint8_t dst_reg_addr,
		uint32_t dst_reg_val
		) {
	// assert (prefix < 16 && > 0);
	uint8_t address = ((prefix & 0xf) << 4) | MPQ_REG_WRITE;
	uint32_t data = (dst_reg_addr << 24) | (dst_reg_val & 0xffffff);
 132:	9101      	str	r1, [sp, #4]
	mbus_write_message(address, &data, 1);
 134:	b2c0      	uxtb	r0, r0
 136:	2201      	movs	r2, #1
 138:	a901      	add	r1, sp, #4
 13a:	f7ff ffbd 	bl	b8 <mbus_write_message>
}
 13e:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_copy_mem_from_local_to_remote_bulk:

00000140 <mbus_copy_mem_from_local_to_remote_bulk>:
void mbus_copy_mem_from_local_to_remote_bulk(
		uint8_t   remote_prefix,
		uint32_t* remote_memory_address,
		uint32_t* local_address,
		uint32_t  length_in_words_minus_one
		) {
 140:	b510      	push	{r4, lr}
	*MBUS_CMD0 = ( ((uint32_t) remote_prefix) << 28 ) | (MPQ_MEM_BULK_WRITE << 24) | (length_in_words_minus_one & 0xFFFFF);
 142:	2480      	movs	r4, #128	; 0x80
 144:	031b      	lsls	r3, r3, #12
 146:	04a4      	lsls	r4, r4, #18
 148:	0b1b      	lsrs	r3, r3, #12
 14a:	4323      	orrs	r3, r4
 14c:	0700      	lsls	r0, r0, #28
 14e:	4303      	orrs	r3, r0
 150:	4804      	ldr	r0, [pc, #16]	; (164 <mbus_copy_mem_from_local_to_remote_bulk+0x24>)
 152:	6003      	str	r3, [r0, #0]
	*MBUS_CMD1 = (uint32_t) local_address;
 154:	4b04      	ldr	r3, [pc, #16]	; (168 <mbus_copy_mem_from_local_to_remote_bulk+0x28>)
 156:	601a      	str	r2, [r3, #0]
	*MBUS_CMD2 = (uint32_t) remote_memory_address;

	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x3 << 4);
 158:	2233      	movs	r2, #51	; 0x33
		uint32_t* local_address,
		uint32_t  length_in_words_minus_one
		) {
	*MBUS_CMD0 = ( ((uint32_t) remote_prefix) << 28 ) | (MPQ_MEM_BULK_WRITE << 24) | (length_in_words_minus_one & 0xFFFFF);
	*MBUS_CMD1 = (uint32_t) local_address;
	*MBUS_CMD2 = (uint32_t) remote_memory_address;
 15a:	4b04      	ldr	r3, [pc, #16]	; (16c <mbus_copy_mem_from_local_to_remote_bulk+0x2c>)
 15c:	6019      	str	r1, [r3, #0]

	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x3 << 4);
 15e:	4b04      	ldr	r3, [pc, #16]	; (170 <mbus_copy_mem_from_local_to_remote_bulk+0x30>)
 160:	601a      	str	r2, [r3, #0]
}
 162:	bd10      	pop	{r4, pc}
 164:	a0002000 	.word	0xa0002000
 168:	a0002004 	.word	0xa0002004
 16c:	a0002008 	.word	0xa0002008
 170:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_copy_mem_from_remote_to_any_bulk:

00000174 <mbus_copy_mem_from_remote_to_any_bulk>:
		uint8_t   source_prefix,
		uint32_t* source_memory_address,
		uint8_t   destination_prefix,
		uint32_t* destination_memory_address,
		uint32_t  length_in_words_minus_one
		) {
 174:	b530      	push	{r4, r5, lr}
	uint32_t payload[3] = {
 176:	2580      	movs	r5, #128	; 0x80
		uint8_t   source_prefix,
		uint32_t* source_memory_address,
		uint8_t   destination_prefix,
		uint32_t* destination_memory_address,
		uint32_t  length_in_words_minus_one
		) {
 178:	b085      	sub	sp, #20
	uint32_t payload[3] = {
 17a:	9c08      	ldr	r4, [sp, #32]
 17c:	04ad      	lsls	r5, r5, #18
 17e:	0324      	lsls	r4, r4, #12
 180:	0b24      	lsrs	r4, r4, #12
 182:	432c      	orrs	r4, r5
 184:	0712      	lsls	r2, r2, #28
 186:	4314      	orrs	r4, r2
		( ((uint32_t) destination_prefix) << 28 )| (MPQ_MEM_BULK_WRITE << 24) | (length_in_words_minus_one & 0xFFFFF),
		(uint32_t) source_memory_address,
		(uint32_t) destination_memory_address,
	};
	mbus_write_message(((source_prefix << 4 ) | MPQ_MEM_READ), payload, 3);
 188:	2203      	movs	r2, #3
 18a:	0100      	lsls	r0, r0, #4
 18c:	4310      	orrs	r0, r2
		uint32_t* source_memory_address,
		uint8_t   destination_prefix,
		uint32_t* destination_memory_address,
		uint32_t  length_in_words_minus_one
		) {
	uint32_t payload[3] = {
 18e:	9102      	str	r1, [sp, #8]
		( ((uint32_t) destination_prefix) << 28 )| (MPQ_MEM_BULK_WRITE << 24) | (length_in_words_minus_one & 0xFFFFF),
		(uint32_t) source_memory_address,
		(uint32_t) destination_memory_address,
	};
	mbus_write_message(((source_prefix << 4 ) | MPQ_MEM_READ), payload, 3);
 190:	b2c0      	uxtb	r0, r0
 192:	a901      	add	r1, sp, #4
		uint32_t* source_memory_address,
		uint8_t   destination_prefix,
		uint32_t* destination_memory_address,
		uint32_t  length_in_words_minus_one
		) {
	uint32_t payload[3] = {
 194:	9401      	str	r4, [sp, #4]
 196:	9303      	str	r3, [sp, #12]
		( ((uint32_t) destination_prefix) << 28 )| (MPQ_MEM_BULK_WRITE << 24) | (length_in_words_minus_one & 0xFFFFF),
		(uint32_t) source_memory_address,
		(uint32_t) destination_memory_address,
	};
	mbus_write_message(((source_prefix << 4 ) | MPQ_MEM_READ), payload, 3);
 198:	f7ff ff8e 	bl	b8 <mbus_write_message>
}
 19c:	b005      	add	sp, #20
 19e:	bd30      	pop	{r4, r5, pc}

Disassembly of section .text.delay:

000001a0 <delay>:
// OTHER FUNCTIONS
//*******************************************************************

void delay(unsigned ticks){
  unsigned i;
  for (i=0; i < ticks; i++)
 1a0:	2300      	movs	r3, #0
 1a2:	4283      	cmp	r3, r0
 1a4:	d002      	beq.n	1ac <delay+0xc>
    asm("nop;");
 1a6:	46c0      	nop			; (mov r8, r8)
// OTHER FUNCTIONS
//*******************************************************************

void delay(unsigned ticks){
  unsigned i;
  for (i=0; i < ticks; i++)
 1a8:	3301      	adds	r3, #1
 1aa:	e7fa      	b.n	1a2 <delay+0x2>
    asm("nop;");
}
 1ac:	4770      	bx	lr

Disassembly of section .text.WFI:

000001ae <WFI>:

void WFI(){
  asm("wfi;");
 1ae:	bf30      	wfi
}
 1b0:	4770      	bx	lr

Disassembly of section .text.config_timer32:

000001b4 <config_timer32>:
	*TIMER16_CNT   = cnt;
	*TIMER16_STAT  = status;
	*TIMER16_GO    = 0x1;
}

void config_timer32(uint32_t cmp, uint8_t roi, uint32_t cnt, uint32_t status){
 1b4:	b530      	push	{r4, r5, lr}
	*TIMER32_GO   = 0x0;
 1b6:	2500      	movs	r5, #0
 1b8:	4c06      	ldr	r4, [pc, #24]	; (1d4 <config_timer32+0x20>)
 1ba:	6025      	str	r5, [r4, #0]
	*TIMER32_CMP  = cmp;
 1bc:	4d06      	ldr	r5, [pc, #24]	; (1d8 <config_timer32+0x24>)
 1be:	6028      	str	r0, [r5, #0]
	*TIMER32_ROI  = roi;
 1c0:	4806      	ldr	r0, [pc, #24]	; (1dc <config_timer32+0x28>)
 1c2:	6001      	str	r1, [r0, #0]
	*TIMER32_CNT  = cnt;
 1c4:	4906      	ldr	r1, [pc, #24]	; (1e0 <config_timer32+0x2c>)
 1c6:	600a      	str	r2, [r1, #0]
	*TIMER32_STAT = status;
 1c8:	4a06      	ldr	r2, [pc, #24]	; (1e4 <config_timer32+0x30>)
 1ca:	6013      	str	r3, [r2, #0]
	*TIMER32_GO   = 0x1;
 1cc:	2301      	movs	r3, #1
 1ce:	6023      	str	r3, [r4, #0]
}
 1d0:	bd30      	pop	{r4, r5, pc}
 1d2:	46c0      	nop			; (mov r8, r8)
 1d4:	a0001100 	.word	0xa0001100
 1d8:	a0001104 	.word	0xa0001104
 1dc:	a0001108 	.word	0xa0001108
 1e0:	a000110c 	.word	0xa000110c
 1e4:	a0001110 	.word	0xa0001110

Disassembly of section .text.enable_xo_timer:

000001e8 <enable_xo_timer>:
	if( reset ) *WUPT_RESET = 0x01;
}

void enable_xo_timer () {
    uint32_t regval = *REG_XOT_CONFIG;
    regval |= 0x800000; // XOT_ENABLE = 1;
 1e8:	2180      	movs	r1, #128	; 0x80

	if( reset ) *WUPT_RESET = 0x01;
}

void enable_xo_timer () {
    uint32_t regval = *REG_XOT_CONFIG;
 1ea:	4a03      	ldr	r2, [pc, #12]	; (1f8 <enable_xo_timer+0x10>)
    regval |= 0x800000; // XOT_ENABLE = 1;
 1ec:	0409      	lsls	r1, r1, #16

	if( reset ) *WUPT_RESET = 0x01;
}

void enable_xo_timer () {
    uint32_t regval = *REG_XOT_CONFIG;
 1ee:	6813      	ldr	r3, [r2, #0]
    regval |= 0x800000; // XOT_ENABLE = 1;
 1f0:	430b      	orrs	r3, r1
    *REG_XOT_CONFIG = regval;
 1f2:	6013      	str	r3, [r2, #0]
}
 1f4:	4770      	bx	lr
 1f6:	46c0      	nop			; (mov r8, r8)
 1f8:	a000004c 	.word	0xa000004c

Disassembly of section .text.set_xo_timer:

000001fc <set_xo_timer>:
    uint32_t regval = *REG_XOT_CONFIG;
    regval &= 0x7FFFFF; // XOT_ENABLE = 0;
    *REG_XOT_CONFIG = regval;
}

void set_xo_timer (uint8_t mode, uint32_t timestamp, uint8_t wreq_en, uint8_t irq_en) {
 1fc:	b510      	push	{r4, lr}
    uint32_t regval0 = timestamp & 0x0000FFFF;
 1fe:	b28c      	uxth	r4, r1
    uint32_t regval1 = (timestamp >> 16) & 0xFFFF;
 200:	0c09      	lsrs	r1, r1, #16
    //uint32_t regval1 = timestamp & 0xFFFF0000; // This is wrong (Reported by Roger Hsiao, Jan 10, 2020)

    regval0 |= 0x00800000; // XOT_ENABLE = 1;
    if (mode)    regval0 |= 0x00400000; // XOT_MODE = 1
 202:	2800      	cmp	r0, #0
 204:	d101      	bne.n	20a <set_xo_timer+0xe>
void set_xo_timer (uint8_t mode, uint32_t timestamp, uint8_t wreq_en, uint8_t irq_en) {
    uint32_t regval0 = timestamp & 0x0000FFFF;
    uint32_t regval1 = (timestamp >> 16) & 0xFFFF;
    //uint32_t regval1 = timestamp & 0xFFFF0000; // This is wrong (Reported by Roger Hsiao, Jan 10, 2020)

    regval0 |= 0x00800000; // XOT_ENABLE = 1;
 206:	2080      	movs	r0, #128	; 0x80
 208:	e000      	b.n	20c <set_xo_timer+0x10>
    if (mode)    regval0 |= 0x00400000; // XOT_MODE = 1
 20a:	20c0      	movs	r0, #192	; 0xc0
 20c:	0400      	lsls	r0, r0, #16
 20e:	4320      	orrs	r0, r4
    if (wreq_en) regval0 |= 0x00200000; // XOT_WREQ_EN = 1
 210:	2a00      	cmp	r2, #0
 212:	d002      	beq.n	21a <set_xo_timer+0x1e>
 214:	2280      	movs	r2, #128	; 0x80
 216:	0392      	lsls	r2, r2, #14
 218:	4310      	orrs	r0, r2
    if (irq_en)  regval0 |= 0x00100000; // XOT_IRQ_EN = 1
 21a:	2b00      	cmp	r3, #0
 21c:	d002      	beq.n	224 <set_xo_timer+0x28>
 21e:	2380      	movs	r3, #128	; 0x80
 220:	035b      	lsls	r3, r3, #13
 222:	4318      	orrs	r0, r3

    *REG_XOT_CONFIGU = regval1;
 224:	4b02      	ldr	r3, [pc, #8]	; (230 <set_xo_timer+0x34>)
 226:	6019      	str	r1, [r3, #0]
    *REG_XOT_CONFIG  = regval0;
 228:	4b02      	ldr	r3, [pc, #8]	; (234 <set_xo_timer+0x38>)
 22a:	6018      	str	r0, [r3, #0]
}
 22c:	bd10      	pop	{r4, pc}
 22e:	46c0      	nop			; (mov r8, r8)
 230:	a0000050 	.word	0xa0000050
 234:	a000004c 	.word	0xa000004c

Disassembly of section .text.reset_xo_cnt:

00000238 <reset_xo_cnt>:

void reset_xo_cnt  () { *XOT_RESET_CNT  = 0x1; }
 238:	2201      	movs	r2, #1
 23a:	4b01      	ldr	r3, [pc, #4]	; (240 <reset_xo_cnt+0x8>)
 23c:	601a      	str	r2, [r3, #0]
 23e:	4770      	bx	lr
 240:	a0001400 	.word	0xa0001400

Disassembly of section .text.start_xo_cnt:

00000244 <start_xo_cnt>:
void start_xo_cnt  () { *XOT_START_CNT  = 0x1; }
 244:	2201      	movs	r2, #1
 246:	4b01      	ldr	r3, [pc, #4]	; (24c <start_xo_cnt+0x8>)
 248:	601a      	str	r2, [r3, #0]
 24a:	4770      	bx	lr
 24c:	a0001404 	.word	0xa0001404

Disassembly of section .text.stop_xo_cnt:

00000250 <stop_xo_cnt>:
void stop_xo_cnt   () { *XOT_STOP_CNT   = 0x1; }
 250:	2201      	movs	r2, #1
 252:	4b01      	ldr	r3, [pc, #4]	; (258 <stop_xo_cnt+0x8>)
 254:	601a      	str	r2, [r3, #0]
 256:	4770      	bx	lr
 258:	a0001408 	.word	0xa0001408

Disassembly of section .text.set_halt_until_mbus_tx:

0000025c <set_halt_until_mbus_tx>:
// MBUS IRQ SETTING
//**************************************************
void set_halt_until_reg(uint32_t reg_id) { *SREG_CONF_HALT = reg_id; }
void set_halt_until_mem_wr(void) { *SREG_CONF_HALT = HALT_UNTIL_MEM_WR; }
void set_halt_until_mbus_rx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_RX; }
void set_halt_until_mbus_tx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_TX; }
 25c:	2209      	movs	r2, #9
 25e:	4b01      	ldr	r3, [pc, #4]	; (264 <set_halt_until_mbus_tx+0x8>)
 260:	601a      	str	r2, [r3, #0]
 262:	4770      	bx	lr
 264:	a000a000 	.word	0xa000a000

Disassembly of section .text.set_halt_until_mbus_trx:

00000268 <set_halt_until_mbus_trx>:
void set_halt_until_mbus_trx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_TRX; }
 268:	220c      	movs	r2, #12
 26a:	4b01      	ldr	r3, [pc, #4]	; (270 <set_halt_until_mbus_trx+0x8>)
 26c:	601a      	str	r2, [r3, #0]
 26e:	4770      	bx	lr
 270:	a000a000 	.word	0xa000a000

Disassembly of section .text.var_init:

00000274 <var_init>:
 * Initialization functions
 **********************************************/

static void var_init() {
    // initialize variables that could be corrupted at program time
    lnt_counter_base = 0;
 274:	2300      	movs	r3, #0
 276:	4a19      	ldr	r2, [pc, #100]	; (2dc <var_init+0x68>)
    rot_idx = 0;
    avg_light = 0;
    next_sunrise = 0;
    next_sunset = 0;

    code_cache_remainder = CODE_CACHE_MAX_REMAINDER;
 278:	4919      	ldr	r1, [pc, #100]	; (2e0 <var_init+0x6c>)
 * Initialization functions
 **********************************************/

static void var_init() {
    // initialize variables that could be corrupted at program time
    lnt_counter_base = 0;
 27a:	8013      	strh	r3, [r2, #0]

    rot_idx = 0;
 27c:	4a19      	ldr	r2, [pc, #100]	; (2e4 <var_init+0x70>)
 27e:	8013      	strh	r3, [r2, #0]
    avg_light = 0;
 280:	4a19      	ldr	r2, [pc, #100]	; (2e8 <var_init+0x74>)
 282:	8013      	strh	r3, [r2, #0]
    next_sunrise = 0;
 284:	4a19      	ldr	r2, [pc, #100]	; (2ec <var_init+0x78>)
 286:	6013      	str	r3, [r2, #0]
    next_sunset = 0;
 288:	4a19      	ldr	r2, [pc, #100]	; (2f0 <var_init+0x7c>)
 28a:	6013      	str	r3, [r2, #0]

    code_cache_remainder = CODE_CACHE_MAX_REMAINDER;
 28c:	2288      	movs	r2, #136	; 0x88
 28e:	0052      	lsls	r2, r2, #1
 290:	800a      	strh	r2, [r1, #0]
    code_addr = 0;
 292:	4918      	ldr	r1, [pc, #96]	; (2f4 <var_init+0x80>)
 294:	800b      	strh	r3, [r1, #0]
    max_unit_count = 0;
 296:	4918      	ldr	r1, [pc, #96]	; (2f8 <var_init+0x84>)
 298:	800b      	strh	r3, [r1, #0]
    radio_beacon_counter = 0;
 29a:	4918      	ldr	r1, [pc, #96]	; (2fc <var_init+0x88>)
 29c:	800b      	strh	r3, [r1, #0]
    low_pwr_count = 0;
 29e:	4918      	ldr	r1, [pc, #96]	; (300 <var_init+0x8c>)
 2a0:	800b      	strh	r3, [r1, #0]
    temp_packet_num = 0;
 2a2:	4918      	ldr	r1, [pc, #96]	; (304 <var_init+0x90>)
 2a4:	800b      	strh	r3, [r1, #0]
    light_packet_num = 0;
 2a6:	4918      	ldr	r1, [pc, #96]	; (308 <var_init+0x94>)
 2a8:	800b      	strh	r3, [r1, #0]
    has_header = false;
 2aa:	4918      	ldr	r1, [pc, #96]	; (30c <var_init+0x98>)
 2ac:	700b      	strb	r3, [r1, #0]
    has_time = false;
 2ae:	4918      	ldr	r1, [pc, #96]	; (310 <var_init+0x9c>)
 2b0:	700b      	strb	r3, [r1, #0]

    temp_cache_remainder = TEMP_CACHE_MAX_REMAINDER;
 2b2:	4918      	ldr	r1, [pc, #96]	; (314 <var_init+0xa0>)
 2b4:	800a      	strh	r2, [r1, #0]

    projected_end_time_in_sec = 0;
 2b6:	4a18      	ldr	r2, [pc, #96]	; (318 <var_init+0xa4>)
 2b8:	6013      	str	r3, [r2, #0]
    next_radio_debug_time = 0;
 2ba:	4a18      	ldr	r2, [pc, #96]	; (31c <var_init+0xa8>)
 2bc:	6013      	str	r3, [r2, #0]
    next_light_meas_time = 0;
 2be:	4a18      	ldr	r2, [pc, #96]	; (320 <var_init+0xac>)
 2c0:	6013      	str	r3, [r2, #0]
    last_log_temp = 0;
 2c2:	4a18      	ldr	r2, [pc, #96]	; (324 <var_init+0xb0>)
 2c4:	8013      	strh	r3, [r2, #0]

    mrr_cfo_val_fine = 0;
 2c6:	4a18      	ldr	r2, [pc, #96]	; (328 <var_init+0xb4>)
 2c8:	8013      	strh	r3, [r2, #0]

    error_code = 0;
 2ca:	4a18      	ldr	r2, [pc, #96]	; (32c <var_init+0xb8>)
 2cc:	6013      	str	r3, [r2, #0]
    error_time = 0;
 2ce:	4a18      	ldr	r2, [pc, #96]	; (330 <var_init+0xbc>)
 2d0:	6013      	str	r3, [r2, #0]

    data_collection_end_day_time = MID_DAY_TIME;
 2d2:	4a18      	ldr	r2, [pc, #96]	; (334 <var_init+0xc0>)
 2d4:	4b18      	ldr	r3, [pc, #96]	; (338 <var_init+0xc4>)
 2d6:	601a      	str	r2, [r3, #0]
}
 2d8:	4770      	bx	lr
 2da:	46c0      	nop			; (mov r8, r8)
 2dc:	00003a40 	.word	0x00003a40
 2e0:	000037f8 	.word	0x000037f8
 2e4:	00003aa4 	.word	0x00003aa4
 2e8:	00003abe 	.word	0x00003abe
 2ec:	00003a30 	.word	0x00003a30
 2f0:	00003a44 	.word	0x00003a44
 2f4:	00003ae8 	.word	0x00003ae8
 2f8:	00003a42 	.word	0x00003a42
 2fc:	00003b02 	.word	0x00003b02
 300:	00003ab4 	.word	0x00003ab4
 304:	00003a6c 	.word	0x00003a6c
 308:	00003aa2 	.word	0x00003aa2
 30c:	00003aa1 	.word	0x00003aa1
 310:	00003a74 	.word	0x00003a74
 314:	000038b4 	.word	0x000038b4
 318:	00003a60 	.word	0x00003a60
 31c:	00003af0 	.word	0x00003af0
 320:	00003a50 	.word	0x00003a50
 324:	00003afa 	.word	0x00003afa
 328:	00003a5a 	.word	0x00003a5a
 32c:	00003b14 	.word	0x00003b14
 330:	00003a9c 	.word	0x00003a9c
 334:	0000a8c0 	.word	0x0000a8c0
 338:	00003870 	.word	0x00003870

Disassembly of section .text.pmu_reg_write:

0000033c <pmu_reg_write>:


/**********************************************
 * PMU functions (PMUv12)
 **********************************************/
static void pmu_reg_write(uint32_t reg_addr, uint32_t reg_data) {
 33c:	b570      	push	{r4, r5, r6, lr}
 33e:	000d      	movs	r5, r1
 340:	0004      	movs	r4, r0
#ifdef USE_PMU
    set_halt_until_mbus_trx();
 342:	f7ff ff91 	bl	268 <set_halt_until_mbus_trx>
    mbus_remote_register_write(PMU_ADDR, reg_addr, reg_data);
 346:	002a      	movs	r2, r5
 348:	b2e1      	uxtb	r1, r4
 34a:	2005      	movs	r0, #5
 34c:	f7ff feeb 	bl	126 <mbus_remote_register_write>
    set_halt_until_mbus_tx();
 350:	f7ff ff84 	bl	25c <set_halt_until_mbus_tx>
#endif
}
 354:	bd70      	pop	{r4, r5, r6, pc}

Disassembly of section .text.pmu_set_sleep_clk:

00000356 <pmu_set_sleep_clk>:
                 (l <<  5) |    // frequency multiplier l
                 (base)));      // floor frequency base (0-63)
#endif
}

static void pmu_set_sleep_clk(uint32_t setting) {
 356:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    // Register 0x17: V3P6 SLEEP
    pmu_reg_write(0x17,         // PMU_EN_UPCONVERTER_TRIM_V3_SLEEP
                ((3 << 14) |    // desired vout/vin ratio; default: 0
                 (1 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l (actually l+1)
 358:	25ff      	movs	r5, #255	; 0xff
 35a:	0c01      	lsrs	r1, r0, #16
 35c:	4029      	ands	r1, r5
 35e:	014b      	lsls	r3, r1, #5
    uint16_t l = (setting >> 16) & 0xFF;
    uint16_t base = (setting >> 8) & 0xFF;
    uint16_t l_1p2 = setting & 0xFF;

    // Register 0x17: V3P6 SLEEP
    pmu_reg_write(0x17,         // PMU_EN_UPCONVERTER_TRIM_V3_SLEEP
 360:	21e0      	movs	r1, #224	; 0xe0
                ((3 << 14) |    // desired vout/vin ratio; default: 0
                 (1 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
 362:	0e06      	lsrs	r6, r0, #24
 364:	0276      	lsls	r6, r6, #9
                 (l <<  5) |    // frequency multiplier l (actually l+1)
 366:	0a04      	lsrs	r4, r0, #8
    uint16_t l = (setting >> 16) & 0xFF;
    uint16_t base = (setting >> 8) & 0xFF;
    uint16_t l_1p2 = setting & 0xFF;

    // Register 0x17: V3P6 SLEEP
    pmu_reg_write(0x17,         // PMU_EN_UPCONVERTER_TRIM_V3_SLEEP
 368:	0209      	lsls	r1, r1, #8
                ((3 << 14) |    // desired vout/vin ratio; default: 0
                 (1 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l (actually l+1)
 36a:	402c      	ands	r4, r5
    uint16_t l = (setting >> 16) & 0xFF;
    uint16_t base = (setting >> 8) & 0xFF;
    uint16_t l_1p2 = setting & 0xFF;

    // Register 0x17: V3P6 SLEEP
    pmu_reg_write(0x17,         // PMU_EN_UPCONVERTER_TRIM_V3_SLEEP
 36c:	4331      	orrs	r1, r6
 36e:	4321      	orrs	r1, r4
 370:	4319      	orrs	r1, r3
                 (l <<  5) |    // frequency multiplier l
                 (base)));      // floor frequency base (0-63)
#endif
}

static void pmu_set_sleep_clk(uint32_t setting) {
 372:	0007      	movs	r7, r0
    uint16_t l = (setting >> 16) & 0xFF;
    uint16_t base = (setting >> 8) & 0xFF;
    uint16_t l_1p2 = setting & 0xFF;

    // Register 0x17: V3P6 SLEEP
    pmu_reg_write(0x17,         // PMU_EN_UPCONVERTER_TRIM_V3_SLEEP
 374:	2017      	movs	r0, #23
                ((3 << 14) |    // desired vout/vin ratio; default: 0
                 (1 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l (actually l+1)
 376:	9301      	str	r3, [sp, #4]
    uint16_t l = (setting >> 16) & 0xFF;
    uint16_t base = (setting >> 8) & 0xFF;
    uint16_t l_1p2 = setting & 0xFF;

    // Register 0x17: V3P6 SLEEP
    pmu_reg_write(0x17,         // PMU_EN_UPCONVERTER_TRIM_V3_SLEEP
 378:	f7ff ffe0 	bl	33c <pmu_reg_write>
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l (actually l+1)
                 (base)));      // floor frequency base (0-63)

    // Register 0x15: V1P2 sleep
    pmu_reg_write(0x15,         // PMU_EN_SAR_TRIM_V3_SLEEP
 37c:	21c0      	movs	r1, #192	; 0xc0
 37e:	0209      	lsls	r1, r1, #8
 380:	403d      	ands	r5, r7
 382:	4331      	orrs	r1, r6
 384:	016d      	lsls	r5, r5, #5
 386:	4321      	orrs	r1, r4
 388:	4329      	orrs	r1, r5
 38a:	2015      	movs	r0, #21
 38c:	f7ff ffd6 	bl	33c <pmu_reg_write>
                 (r <<  9) |    // frequency multiplier r
                 (l_1p2 << 5) | // frequency multiplier l (actually l+1)
                 (base)));      // floor frequency base (0-63)

    // Register 0x19: V0P6 SLEEP
    pmu_reg_write(0x19,         // PMU_EN_DOWNCONVERTER_TRIM_V3_SLEEP
 390:	9901      	ldr	r1, [sp, #4]
 392:	4334      	orrs	r4, r6
 394:	4321      	orrs	r1, r4
 396:	2019      	movs	r0, #25
 398:	f7ff ffd0 	bl	33c <pmu_reg_write>
                ((0 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l (actually l+1)
                 (base)));      // floor frequency base (0-63)
#endif
}
 39c:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}

Disassembly of section .text.pmu_set_active_clk:

0000039e <pmu_set_active_clk>:
        | (1 << 20) //horizon
    ));
#endif
}

static void pmu_set_active_clk(uint32_t setting) {
 39e:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
                 (0 << 18) |    // enable PFM even when Vref is not used as ref
                 (0 << 17) |    // enable PFM
                 (cclk_div << 14) |    // comparator clock division ratio
                 (0 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
                 (0 << 5) | // frequency multiplier l (actually l+1) GC: fixed at 0
 3a0:	24ff      	movs	r4, #255	; 0xff

    // mbus_write_message32(0xDE, setting);

    // The first register write to PMU needs to be repeated
    // Register 0x16: V1P2 ACTIVE
    pmu_reg_write(0x16,         // PMU_EN_SAR_TRIM_V3_ACTIVE
 3a2:	0005      	movs	r5, r0
                 (0 << 18) |    // enable PFM even when Vref is not used as ref
                 (0 << 17) |    // enable PFM
                 (cclk_div << 14) |    // comparator clock division ratio
                 (0 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
                 (0 << 5) | // frequency multiplier l (actually l+1) GC: fixed at 0
 3a4:	0a01      	lsrs	r1, r0, #8

    // mbus_write_message32(0xDE, setting);

    // The first register write to PMU needs to be repeated
    // Register 0x16: V1P2 ACTIVE
    pmu_reg_write(0x16,         // PMU_EN_SAR_TRIM_V3_ACTIVE
 3a6:	4025      	ands	r5, r4
                 (0 << 18) |    // enable PFM even when Vref is not used as ref
                 (0 << 17) |    // enable PFM
                 (cclk_div << 14) |    // comparator clock division ratio
                 (0 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
                 (0 << 5) | // frequency multiplier l (actually l+1) GC: fixed at 0
 3a8:	4021      	ands	r1, r4
                ((0 << 19) |    // enable PFM even during periodic reset
                 (0 << 18) |    // enable PFM even when Vref is not used as ref
                 (0 << 17) |    // enable PFM
                 (cclk_div << 14) |    // comparator clock division ratio
                 (0 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
 3aa:	0e07      	lsrs	r7, r0, #24

    // mbus_write_message32(0xDE, setting);

    // The first register write to PMU needs to be repeated
    // Register 0x16: V1P2 ACTIVE
    pmu_reg_write(0x16,         // PMU_EN_SAR_TRIM_V3_ACTIVE
 3ac:	03ad      	lsls	r5, r5, #14
                ((0 << 19) |    // enable PFM even during periodic reset
                 (0 << 18) |    // enable PFM even when Vref is not used as ref
                 (0 << 17) |    // enable PFM
                 (cclk_div << 14) |    // comparator clock division ratio
                 (0 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
 3ae:	027f      	lsls	r7, r7, #9

    // mbus_write_message32(0xDE, setting);

    // The first register write to PMU needs to be repeated
    // Register 0x16: V1P2 ACTIVE
    pmu_reg_write(0x16,         // PMU_EN_SAR_TRIM_V3_ACTIVE
 3b0:	430d      	orrs	r5, r1
 3b2:	433d      	orrs	r5, r7
                 (0 << 18) |    // enable PFM even when Vref is not used as ref
                 (0 << 17) |    // enable PFM
                 (cclk_div << 14) |    // comparator clock division ratio
                 (0 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
                 (0 << 5) | // frequency multiplier l (actually l+1) GC: fixed at 0
 3b4:	9101      	str	r1, [sp, #4]
        | (1 << 20) //horizon
    ));
#endif
}

static void pmu_set_active_clk(uint32_t setting) {
 3b6:	0006      	movs	r6, r0

    // mbus_write_message32(0xDE, setting);

    // The first register write to PMU needs to be repeated
    // Register 0x16: V1P2 ACTIVE
    pmu_reg_write(0x16,         // PMU_EN_SAR_TRIM_V3_ACTIVE
 3b8:	0029      	movs	r1, r5
 3ba:	2016      	movs	r0, #22
 3bc:	f7ff ffbe 	bl	33c <pmu_reg_write>
                 (0 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
                 (0 << 5) | // frequency multiplier l (actually l+1) GC: fixed at 0
                 (base)));      // floor frequency base (0-63)

    pmu_reg_write(0x16,         // PMU_EN_SAR_TRIM_V3_ACTIVE
 3c0:	0029      	movs	r1, r5
 3c2:	2016      	movs	r0, #22
 3c4:	f7ff ffba 	bl	33c <pmu_reg_write>
                 (r <<  9) |    // frequency multiplier r
                 (0 << 5) | // frequency multiplier l (actually l+1) GC: fixed at 0
                 (base)));      // floor frequency base (0-63)

    // Register 0x18: V3P6 ACTIVE
    pmu_reg_write(0x18,         // PMU_EN_UPCONVERTER_TRIM_V3_ACTIVE
 3c8:	21e0      	movs	r1, #224	; 0xe0
 3ca:	9b01      	ldr	r3, [sp, #4]
                ((3 << 14) |    // desired vout/vin ratio; default: 0
                 (1 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l
 3cc:	0c36      	lsrs	r6, r6, #16
                 (r <<  9) |    // frequency multiplier r
                 (0 << 5) | // frequency multiplier l (actually l+1) GC: fixed at 0
                 (base)));      // floor frequency base (0-63)

    // Register 0x18: V3P6 ACTIVE
    pmu_reg_write(0x18,         // PMU_EN_UPCONVERTER_TRIM_V3_ACTIVE
 3ce:	0209      	lsls	r1, r1, #8
                ((3 << 14) |    // desired vout/vin ratio; default: 0
                 (1 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l
 3d0:	4034      	ands	r4, r6
                 (r <<  9) |    // frequency multiplier r
                 (0 << 5) | // frequency multiplier l (actually l+1) GC: fixed at 0
                 (base)));      // floor frequency base (0-63)

    // Register 0x18: V3P6 ACTIVE
    pmu_reg_write(0x18,         // PMU_EN_UPCONVERTER_TRIM_V3_ACTIVE
 3d2:	4339      	orrs	r1, r7
                ((3 << 14) |    // desired vout/vin ratio; default: 0
                 (1 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l
 3d4:	0164      	lsls	r4, r4, #5
                 (r <<  9) |    // frequency multiplier r
                 (0 << 5) | // frequency multiplier l (actually l+1) GC: fixed at 0
                 (base)));      // floor frequency base (0-63)

    // Register 0x18: V3P6 ACTIVE
    pmu_reg_write(0x18,         // PMU_EN_UPCONVERTER_TRIM_V3_ACTIVE
 3d6:	4319      	orrs	r1, r3
 3d8:	4321      	orrs	r1, r4
 3da:	2018      	movs	r0, #24
 3dc:	f7ff ffae 	bl	33c <pmu_reg_write>
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l
                 (base)));      // floor frequency base (0-63)

    // Register 0x1A: V0P6 ACTIVE
    pmu_reg_write(0x1A,         // PMU_EN_DOWNCONVERTER_TRIM_V3_ACTIVE
 3e0:	9b01      	ldr	r3, [sp, #4]
 3e2:	201a      	movs	r0, #26
 3e4:	431f      	orrs	r7, r3
 3e6:	0039      	movs	r1, r7
 3e8:	4321      	orrs	r1, r4
 3ea:	f7ff ffa7 	bl	33c <pmu_reg_write>
                ((0 << 13) |    // Makes the converter clock 2x slower (Default: 1'b0)
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l
                 (base)));      // floor frequency base (0-63)
#endif
}
 3ee:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}

Disassembly of section .text.unlikely.pmu_current_marker:

000003f0 <pmu_current_marker>:
    return;
#endif
}

// generate a high current
static void pmu_current_marker() {
 3f0:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
        // mbus_write_message32(0xB3, 0xFFFFFFFF);
        if(i == 0 || snt_sys_temp_code >= PMU_TEMP_THRESH[i - 1]) {
            uint32_t temp_setting = PMU_ACTIVE_SETTINGS[i] & 0xFFFFFFFF;
            // set base to a high value
            temp_setting |= 0x0000FF00;
            pmu_set_active_clk(temp_setting);
 3f2:	26ff      	movs	r6, #255	; 0xff
    return;
#endif
}

// generate a high current
static void pmu_current_marker() {
 3f4:	2306      	movs	r3, #6
        // mbus_write_message32(0xB3, 0xFFFFFFFF);
        if(i == 0 || snt_sys_temp_code >= PMU_TEMP_THRESH[i - 1]) {
            uint32_t temp_setting = PMU_ACTIVE_SETTINGS[i] & 0xFFFFFFFF;
            // set base to a high value
            temp_setting |= 0x0000FF00;
            pmu_set_active_clk(temp_setting);
 3f6:	0236      	lsls	r6, r6, #8
 3f8:	1e5c      	subs	r4, r3, #1
static void pmu_current_marker() {
#ifdef USE_PMU
    int8_t i;
    for(i = PMU_SETTINGS_LEN; i >= 0; i--) {
        // mbus_write_message32(0xB3, 0xFFFFFFFF);
        if(i == 0 || snt_sys_temp_code >= PMU_TEMP_THRESH[i - 1]) {
 3fa:	2b00      	cmp	r3, #0
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
 40e:	009f      	lsls	r7, r3, #2
 410:	5978      	ldr	r0, [r7, r5]
            // set base to a high value
            temp_setting |= 0x0000FF00;
            pmu_set_active_clk(temp_setting);
 412:	4330      	orrs	r0, r6
 414:	f7ff ffc3 	bl	39e <pmu_set_active_clk>
            pmu_set_active_clk(PMU_ACTIVE_SETTINGS[i]);
 418:	5978      	ldr	r0, [r7, r5]
 41a:	f7ff ffc0 	bl	39e <pmu_set_active_clk>
 41e:	0023      	movs	r3, r4

// generate a high current
static void pmu_current_marker() {
#ifdef USE_PMU
    int8_t i;
    for(i = PMU_SETTINGS_LEN; i >= 0; i--) {
 420:	1c62      	adds	r2, r4, #1
 422:	d1e9      	bne.n	3f8 <pmu_current_marker+0x8>
            pmu_set_active_clk(PMU_ACTIVE_SETTINGS[i]);
        }
    }
    return;
#endif
}
 424:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 426:	46c0      	nop			; (mov r8, r8)
 428:	00003934 	.word	0x00003934
 42c:	000038b8 	.word	0x000038b8
 430:	000037dc 	.word	0x000037dc

Disassembly of section .text.radio_power_off:

00000434 <radio_power_off>:

    // Enable PMU ADC
    //pmu_adc_enable();

    // In case continuous mode was running
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
 434:	2208      	movs	r2, #8
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    delay(MBUS_DELAY*5); // Freq stab

}

static void radio_power_off() {
 436:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
        
    // Turn off FSM
    mrrv11a_r03.TRX_ISOLATEN = 0;     //set ISOLATEN 0
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);

    mrrv11a_r11.FSM_EN = 0;  //Stop BB
 438:	2704      	movs	r7, #4
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
 43a:	2502      	movs	r5, #2
    mrrv11a_r11.FSM_SLEEP = 1;
 43c:	2601      	movs	r6, #1

    // Enable PMU ADC
    //pmu_adc_enable();

    // In case continuous mode was running
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
 43e:	4c2e      	ldr	r4, [pc, #184]	; (4f8 <radio_power_off+0xc4>)
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 440:	2111      	movs	r1, #17

    // Enable PMU ADC
    //pmu_adc_enable();

    // In case continuous mode was running
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
 442:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 444:	2002      	movs	r0, #2

    // Enable PMU ADC
    //pmu_adc_enable();

    // In case continuous mode was running
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
 446:	4393      	bics	r3, r2
 448:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 44a:	6822      	ldr	r2, [r4, #0]
 44c:	f7ff fe6b 	bl	126 <mbus_remote_register_write>
        
    // Turn off FSM
    mrrv11a_r03.TRX_ISOLATEN = 0;     //set ISOLATEN 0
 450:	4b2a      	ldr	r3, [pc, #168]	; (4fc <radio_power_off+0xc8>)
 452:	4a2b      	ldr	r2, [pc, #172]	; (500 <radio_power_off+0xcc>)
 454:	6819      	ldr	r1, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 456:	2002      	movs	r0, #2
    // In case continuous mode was running
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
        
    // Turn off FSM
    mrrv11a_r03.TRX_ISOLATEN = 0;     //set ISOLATEN 0
 458:	400a      	ands	r2, r1
 45a:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 45c:	681a      	ldr	r2, [r3, #0]
 45e:	2103      	movs	r1, #3
 460:	f7ff fe61 	bl	126 <mbus_remote_register_write>

    mrrv11a_r11.FSM_EN = 0;  //Stop BB
 464:	6823      	ldr	r3, [r4, #0]
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
    mrrv11a_r11.FSM_SLEEP = 1;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 466:	0028      	movs	r0, r5
        
    // Turn off FSM
    mrrv11a_r03.TRX_ISOLATEN = 0;     //set ISOLATEN 0
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);

    mrrv11a_r11.FSM_EN = 0;  //Stop BB
 468:	43bb      	bics	r3, r7
 46a:	6023      	str	r3, [r4, #0]
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
 46c:	6823      	ldr	r3, [r4, #0]
    mrrv11a_r11.FSM_SLEEP = 1;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 46e:	2111      	movs	r1, #17
    // Turn off FSM
    mrrv11a_r03.TRX_ISOLATEN = 0;     //set ISOLATEN 0
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);

    mrrv11a_r11.FSM_EN = 0;  //Stop BB
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
 470:	43ab      	bics	r3, r5
 472:	6023      	str	r3, [r4, #0]
    mrrv11a_r11.FSM_SLEEP = 1;
 474:	6823      	ldr	r3, [r4, #0]
 476:	4333      	orrs	r3, r6
 478:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 47a:	6822      	ldr	r2, [r4, #0]
 47c:	f7ff fe53 	bl	126 <mbus_remote_register_write>
    
    // Turn off Current Limiter Briefly
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
 480:	4c20      	ldr	r4, [pc, #128]	; (504 <radio_power_off+0xd0>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 482:	0028      	movs	r0, r5
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
    mrrv11a_r11.FSM_SLEEP = 1;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    
    // Turn off Current Limiter Briefly
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
 484:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 486:	2100      	movs	r1, #0
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
    mrrv11a_r11.FSM_SLEEP = 1;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    
    // Turn off Current Limiter Briefly
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
 488:	43b3      	bics	r3, r6
 48a:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 48c:	6822      	ldr	r2, [r4, #0]
 48e:	f7ff fe4a 	bl	126 <mbus_remote_register_write>

    // Current Limiter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
 492:	227e      	movs	r2, #126	; 0x7e
 494:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 496:	0028      	movs	r0, r5
    // Turn off Current Limiter Briefly
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Current Limiter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
 498:	4393      	bics	r3, r2
 49a:	3a5e      	subs	r2, #94	; 0x5e
 49c:	4313      	orrs	r3, r2
 49e:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 4a0:	6822      	ldr	r2, [r4, #0]
 4a2:	2100      	movs	r1, #0
 4a4:	f7ff fe3f 	bl	126 <mbus_remote_register_write>

    // Turn on Current Limiter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
 4a8:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 4aa:	0028      	movs	r0, r5
    // Current Limiter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Turn on Current Limiter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
 4ac:	4333      	orrs	r3, r6
 4ae:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 4b0:	6822      	ldr	r2, [r4, #0]
 4b2:	2100      	movs	r1, #0
 4b4:	f7ff fe37 	bl	126 <mbus_remote_register_write>

    // Turn off RO
    mrrv11a_r04.RO_RESET = 1;  //Release Reset TIMER
 4b8:	2208      	movs	r2, #8
 4ba:	4c13      	ldr	r4, [pc, #76]	; (508 <radio_power_off+0xd4>)
    mrrv11a_r04.RO_EN_CLK = 0; //Enable CLK TIMER
    mrrv11a_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 4bc:	0039      	movs	r1, r7
    // Turn on Current Limiter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Turn off RO
    mrrv11a_r04.RO_RESET = 1;  //Release Reset TIMER
 4be:	6823      	ldr	r3, [r4, #0]
    mrrv11a_r04.RO_EN_CLK = 0; //Enable CLK TIMER
    mrrv11a_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 4c0:	0028      	movs	r0, r5
    // Turn on Current Limiter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Turn off RO
    mrrv11a_r04.RO_RESET = 1;  //Release Reset TIMER
 4c2:	4313      	orrs	r3, r2
 4c4:	6023      	str	r3, [r4, #0]
    mrrv11a_r04.RO_EN_CLK = 0; //Enable CLK TIMER
 4c6:	6823      	ldr	r3, [r4, #0]
 4c8:	3218      	adds	r2, #24
 4ca:	4393      	bics	r3, r2
 4cc:	6023      	str	r3, [r4, #0]
    mrrv11a_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
 4ce:	6823      	ldr	r3, [r4, #0]
 4d0:	3a10      	subs	r2, #16
 4d2:	4313      	orrs	r3, r2
 4d4:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 4d6:	6822      	ldr	r2, [r4, #0]
 4d8:	f7ff fe25 	bl	126 <mbus_remote_register_write>

//remove?     mrr_ldo_power_off();

    // Enable timer power-gate
    mrrv11a_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
 4dc:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 4de:	0039      	movs	r1, r7
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);

//remove?     mrr_ldo_power_off();

    // Enable timer power-gate
    mrrv11a_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
 4e0:	43b3      	bics	r3, r6
 4e2:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 4e4:	6822      	ldr	r2, [r4, #0]
 4e6:	0028      	movs	r0, r5
 4e8:	f7ff fe1d 	bl	126 <mbus_remote_register_write>

    radio_on = 0;
 4ec:	2300      	movs	r3, #0
 4ee:	4a07      	ldr	r2, [pc, #28]	; (50c <radio_power_off+0xd8>)
 4f0:	8013      	strh	r3, [r2, #0]
    radio_ready = 0;
 4f2:	4a07      	ldr	r2, [pc, #28]	; (510 <radio_power_off+0xdc>)
 4f4:	8013      	strh	r3, [r2, #0]

}
 4f6:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 4f8:	00003858 	.word	0x00003858
 4fc:	0000380c 	.word	0x0000380c
 500:	ffbfffff 	.word	0xffbfffff
 504:	00003804 	.word	0x00003804
 508:	00003810 	.word	0x00003810
 50c:	00003a2e 	.word	0x00003a2e
 510:	00003986 	.word	0x00003986

Disassembly of section .text.operation_sleep:

00000514 <operation_sleep>:
 * End of program sleep operation
 **********************************************/

static void operation_sleep( void ) {
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;
 514:	238c      	movs	r3, #140	; 0x8c
 516:	2200      	movs	r2, #0

/**********************************************
 * End of program sleep operation
 **********************************************/

static void operation_sleep( void ) {
 518:	b510      	push	{r4, lr}
    // Reset GOC_DATA_IRQ
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
 528:	f7ff fde6 	bl	f8 <mbus_sleep_all>
 52c:	e7fe      	b.n	52c <operation_sleep+0x18>
 52e:	46c0      	nop			; (mov r8, r8)
 530:	00003a2e 	.word	0x00003a2e

Disassembly of section .text.xo_init:

00000534 <xo_init>:
        wfi_timeout_flag = 0;
        sys_err(0x04000000);
    }
}

void xo_init( void ) {
 534:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    prev21e_r19_t prev21e_r19 = PREv21E_R19_DEFAULT;
 536:	4b34      	ldr	r3, [pc, #208]	; (608 <xo_init+0xd4>)
    uint32_t xo_cap_drv = 0x3F;
    uint32_t xo_cap_in  = 0x3F;
    *REG_XO_CONF2 = ((xo_cap_drv << 6) | (xo_cap_in << 0));

    // XO xonfiguration
    prev21e_r19.XO_PULSE_SEL     = 0x8; // pulse with sel, 1-hot encoded
 538:	4c34      	ldr	r4, [pc, #208]	; (60c <xo_init+0xd8>)
        sys_err(0x04000000);
    }
}

void xo_init( void ) {
    prev21e_r19_t prev21e_r19 = PREv21E_R19_DEFAULT;
 53a:	681b      	ldr	r3, [r3, #0]
    // XO xonfiguration
    prev21e_r19.XO_PULSE_SEL     = 0x8; // pulse with sel, 1-hot encoded
    prev21e_r19.XO_DELAY_EN      = 0x7; // pair usage together with xo_pulse_sel
    prev21e_r19.XO_DRV_START_UP  = 0x0;
    prev21e_r19.XO_DRV_CORE      = 0x0;
    prev21e_r19.XO_SCN_CLK_SEL   = 0x0;
 53c:	2602      	movs	r6, #2
    uint32_t xo_cap_drv = 0x3F;
    uint32_t xo_cap_in  = 0x3F;
    *REG_XO_CONF2 = ((xo_cap_drv << 6) | (xo_cap_in << 0));

    // XO xonfiguration
    prev21e_r19.XO_PULSE_SEL     = 0x8; // pulse with sel, 1-hot encoded
 53e:	4023      	ands	r3, r4
    prev21e_r19.XO_DELAY_EN      = 0x7; // pair usage together with xo_pulse_sel
 540:	248e      	movs	r4, #142	; 0x8e
 542:	01e4      	lsls	r4, r4, #7
 544:	431c      	orrs	r4, r3
    prev21e_r19.XO_DRV_START_UP  = 0x0;
 546:	2380      	movs	r3, #128	; 0x80
 548:	439c      	bics	r4, r3
    prev21e_r19.XO_DRV_CORE      = 0x0;
 54a:	3b40      	subs	r3, #64	; 0x40
 54c:	439c      	bics	r4, r3
    prev21e_r19.XO_SCN_CLK_SEL   = 0x0;
 54e:	43b4      	bics	r4, r6
    prev21e_r19.XO_SCN_ENB       = 0x1;
 550:	3b3f      	subs	r3, #63	; 0x3f
 552:	431c      	orrs	r4, r3

    // TODO: check if need 32.768kHz clock
    prev21e_r19.XO_EN_DIV        = 0x1; // divider enable (also enables CLK_OUT)
 554:	2380      	movs	r3, #128	; 0x80
 556:	035b      	lsls	r3, r3, #13
 558:	431c      	orrs	r4, r3
    prev21e_r19.XO_S             = 0x0; // (not used) division ration for 16kHz out
    prev21e_r19.XO_SEL_CP_DIV    = 0x0; // 1: 0.3v-generation charge-pump uses divided clock
 55a:	4b2d      	ldr	r3, [pc, #180]	; (610 <xo_init+0xdc>)
    prev21e_r19_t prev21e_r19 = PREv21E_R19_DEFAULT;

    // Parasitic capacitance tuning (6 bits for each; each 1 adds 1.8pF)
    uint32_t xo_cap_drv = 0x3F;
    uint32_t xo_cap_in  = 0x3F;
    *REG_XO_CONF2 = ((xo_cap_drv << 6) | (xo_cap_in << 0));
 55c:	4a2d      	ldr	r2, [pc, #180]	; (614 <xo_init+0xe0>)
    prev21e_r19.XO_SCN_ENB       = 0x1;

    // TODO: check if need 32.768kHz clock
    prev21e_r19.XO_EN_DIV        = 0x1; // divider enable (also enables CLK_OUT)
    prev21e_r19.XO_S             = 0x0; // (not used) division ration for 16kHz out
    prev21e_r19.XO_SEL_CP_DIV    = 0x0; // 1: 0.3v-generation charge-pump uses divided clock
 55e:	401c      	ands	r4, r3
    prev21e_r19.XO_EN_OUT        = 0x1; // xo output enabled;
 560:	2380      	movs	r3, #128	; 0x80
 562:	021b      	lsls	r3, r3, #8
 564:	431c      	orrs	r4, r3
                           // Note: I think this means output to XOT
    // Pseudo-resistor selection
    prev21e_r19.XO_RP_LOW        = 0x0;
 566:	2320      	movs	r3, #32
 568:	439c      	bics	r4, r3
    prev21e_r19.XO_RP_MEDIA      = 0x1;
 56a:	3b10      	subs	r3, #16
 56c:	431c      	orrs	r4, r3
    prev21e_r19.XO_RP_MVT        = 0x0;
    prev21e_r19.XO_RP_SVT        = 0x0;

    prev21e_r19.XO_SLEEP = 0x0;
 56e:	4b2a      	ldr	r3, [pc, #168]	; (618 <xo_init+0xe4>)
    *REG_XO_CONF1 = prev21e_r19.as_int;
 570:	4d2a      	ldr	r5, [pc, #168]	; (61c <xo_init+0xe8>)
    prev21e_r19_t prev21e_r19 = PREv21E_R19_DEFAULT;

    // Parasitic capacitance tuning (6 bits for each; each 1 adds 1.8pF)
    uint32_t xo_cap_drv = 0x3F;
    uint32_t xo_cap_in  = 0x3F;
    *REG_XO_CONF2 = ((xo_cap_drv << 6) | (xo_cap_in << 0));
 572:	492b      	ldr	r1, [pc, #172]	; (620 <xo_init+0xec>)
    prev21e_r19.XO_RP_LOW        = 0x0;
    prev21e_r19.XO_RP_MEDIA      = 0x1;
    prev21e_r19.XO_RP_MVT        = 0x0;
    prev21e_r19.XO_RP_SVT        = 0x0;

    prev21e_r19.XO_SLEEP = 0x0;
 574:	401c      	ands	r4, r3
    prev21e_r19_t prev21e_r19 = PREv21E_R19_DEFAULT;

    // Parasitic capacitance tuning (6 bits for each; each 1 adds 1.8pF)
    uint32_t xo_cap_drv = 0x3F;
    uint32_t xo_cap_in  = 0x3F;
    *REG_XO_CONF2 = ((xo_cap_drv << 6) | (xo_cap_in << 0));
 576:	6011      	str	r1, [r2, #0]
    prev21e_r19.XO_RP_SVT        = 0x0;

    prev21e_r19.XO_SLEEP = 0x0;
    *REG_XO_CONF1 = prev21e_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(10000); // >= 1ms
 578:	4f2a      	ldr	r7, [pc, #168]	; (624 <xo_init+0xf0>)
    prev21e_r19.XO_RP_MEDIA      = 0x1;
    prev21e_r19.XO_RP_MVT        = 0x0;
    prev21e_r19.XO_RP_SVT        = 0x0;

    prev21e_r19.XO_SLEEP = 0x0;
    *REG_XO_CONF1 = prev21e_r19.as_int;
 57a:	602c      	str	r4, [r5, #0]
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 57c:	6829      	ldr	r1, [r5, #0]
 57e:	20a1      	movs	r0, #161	; 0xa1
 580:	f7ff fd92 	bl	a8 <mbus_write_message32>
    delay(10000); // >= 1ms
 584:	0038      	movs	r0, r7
 586:	f7ff fe0b 	bl	1a0 <delay>

    prev21e_r19.XO_ISOLATE = 0x0;
 58a:	4b27      	ldr	r3, [pc, #156]	; (628 <xo_init+0xf4>)
    *REG_XO_CONF1 = prev21e_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 58c:	20a1      	movs	r0, #161	; 0xa1
    prev21e_r19.XO_SLEEP = 0x0;
    *REG_XO_CONF1 = prev21e_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(10000); // >= 1ms

    prev21e_r19.XO_ISOLATE = 0x0;
 58e:	401c      	ands	r4, r3
    *REG_XO_CONF1 = prev21e_r19.as_int;
 590:	602c      	str	r4, [r5, #0]
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 592:	6829      	ldr	r1, [r5, #0]
 594:	f7ff fd88 	bl	a8 <mbus_write_message32>
    delay(10000); // >= 1ms
 598:	0038      	movs	r0, r7
 59a:	f7ff fe01 	bl	1a0 <delay>

    prev21e_r19.XO_DRV_START_UP = 0x1;
 59e:	2380      	movs	r3, #128	; 0x80
 5a0:	431c      	orrs	r4, r3
    *REG_XO_CONF1 = prev21e_r19.as_int;
 5a2:	602c      	str	r4, [r5, #0]
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(30000); // >= 1s
 5a4:	4f21      	ldr	r7, [pc, #132]	; (62c <xo_init+0xf8>)
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(10000); // >= 1ms

    prev21e_r19.XO_DRV_START_UP = 0x1;
    *REG_XO_CONF1 = prev21e_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 5a6:	6829      	ldr	r1, [r5, #0]
 5a8:	20a1      	movs	r0, #161	; 0xa1
 5aa:	f7ff fd7d 	bl	a8 <mbus_write_message32>
    delay(30000); // >= 1s

    prev21e_r19.XO_SCN_CLK_SEL = 0x1;
 5ae:	4334      	orrs	r4, r6
    delay(10000); // >= 1ms

    prev21e_r19.XO_DRV_START_UP = 0x1;
    *REG_XO_CONF1 = prev21e_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(30000); // >= 1s
 5b0:	0038      	movs	r0, r7
 5b2:	f7ff fdf5 	bl	1a0 <delay>

    prev21e_r19.XO_SCN_CLK_SEL = 0x1;
    *REG_XO_CONF1 = prev21e_r19.as_int;
 5b6:	602c      	str	r4, [r5, #0]
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 5b8:	6829      	ldr	r1, [r5, #0]
 5ba:	20a1      	movs	r0, #161	; 0xa1
 5bc:	f7ff fd74 	bl	a8 <mbus_write_message32>
    delay(3000); // >= 300us
 5c0:	481b      	ldr	r0, [pc, #108]	; (630 <xo_init+0xfc>)
 5c2:	f7ff fded 	bl	1a0 <delay>

    prev21e_r19.XO_SCN_CLK_SEL = 0x0;
    prev21e_r19.XO_SCN_ENB     = 0x0;
 5c6:	2301      	movs	r3, #1
    prev21e_r19.XO_SCN_CLK_SEL = 0x1;
    *REG_XO_CONF1 = prev21e_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(3000); // >= 300us

    prev21e_r19.XO_SCN_CLK_SEL = 0x0;
 5c8:	43b4      	bics	r4, r6
    prev21e_r19.XO_SCN_ENB     = 0x0;
 5ca:	439c      	bics	r4, r3
    *REG_XO_CONF1 = prev21e_r19.as_int;
 5cc:	602c      	str	r4, [r5, #0]
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 5ce:	6829      	ldr	r1, [r5, #0]
 5d0:	20a1      	movs	r0, #161	; 0xa1
 5d2:	f7ff fd69 	bl	a8 <mbus_write_message32>
    delay(30000);  // >= 1s
 5d6:	0038      	movs	r0, r7
 5d8:	f7ff fde2 	bl	1a0 <delay>

    prev21e_r19.XO_DRV_START_UP = 0x0;
 5dc:	2380      	movs	r3, #128	; 0x80
 5de:	439c      	bics	r4, r3
    prev21e_r19.XO_DRV_CORE     = 0x1;
 5e0:	3b40      	subs	r3, #64	; 0x40
 5e2:	431c      	orrs	r4, r3
    prev21e_r19.XO_SCN_CLK_SEL  = 0x1;
 5e4:	4326      	orrs	r6, r4
    *REG_XO_CONF1 = prev21e_r19.as_int;
 5e6:	602e      	str	r6, [r5, #0]
    mbus_write_message32(0xA1, *REG_XO_CONF1);
 5e8:	6829      	ldr	r1, [r5, #0]
 5ea:	20a1      	movs	r0, #161	; 0xa1
 5ec:	f7ff fd5c 	bl	a8 <mbus_write_message32>

    enable_xo_timer();
 5f0:	f7ff fdfa 	bl	1e8 <enable_xo_timer>
    reset_xo_cnt();
 5f4:	f7ff fe20 	bl	238 <reset_xo_cnt>
    start_xo_cnt();
 5f8:	f7ff fe24 	bl	244 <start_xo_cnt>

    // BREAKPOint 0x03
    mbus_write_message32(0xBA, 0x03);
 5fc:	2103      	movs	r1, #3
 5fe:	20ba      	movs	r0, #186	; 0xba
 600:	f7ff fd52 	bl	a8 <mbus_write_message32>

}
 604:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 606:	46c0      	nop			; (mov r8, r8)
 608:	00003698 	.word	0x00003698
 60c:	ffff87ff 	.word	0xffff87ff
 610:	fff0c7ff 	.word	0xfff0c7ff
 614:	a0000068 	.word	0xa0000068
 618:	ffb0c7f3 	.word	0xffb0c7f3
 61c:	a0000064 	.word	0xa0000064
 620:	00000fff 	.word	0x00000fff
 624:	00002710 	.word	0x00002710
 628:	ffdfffff 	.word	0xffdfffff
 62c:	00007530 	.word	0x00007530
 630:	00000bb8 	.word	0x00000bb8

Disassembly of section .text.divide_by_60:

00000634 <divide_by_60>:
    // mbus_write_message32(0xC1, xo_sys_time_in_sec);
    // mbus_write_message32(0xC0, xo_day_time_in_sec);
}

// divide any time by 60 to get representation in minutes
uint32_t divide_by_60(uint32_t source) {
 634:	b530      	push	{r4, r5, lr}
 636:	0002      	movs	r2, r0
    uint32_t lower = source >> 6;   // divide by 64
    uint32_t res;

    while(1) {
        res = (upper + lower) >> 1;  // average of upper and lower
        uint32_t comp = res * 60;
 638:	253c      	movs	r5, #60	; 0x3c
    // mbus_write_message32(0xC0, xo_day_time_in_sec);
}

// divide any time by 60 to get representation in minutes
uint32_t divide_by_60(uint32_t source) {
    uint32_t upper = source >> 5;   // divide by 32
 63a:	0941      	lsrs	r1, r0, #5
    uint32_t lower = source >> 6;   // divide by 64
 63c:	0984      	lsrs	r4, r0, #6
    uint32_t res;

    while(1) {
        res = (upper + lower) >> 1;  // average of upper and lower
 63e:	190b      	adds	r3, r1, r4
 640:	0858      	lsrs	r0, r3, #1
        uint32_t comp = res * 60;
 642:	002b      	movs	r3, r5
 644:	4343      	muls	r3, r0

        if(source < comp) {
 646:	429a      	cmp	r2, r3
 648:	d201      	bcs.n	64e <divide_by_60+0x1a>
    uint32_t upper = source >> 5;   // divide by 32
    uint32_t lower = source >> 6;   // divide by 64
    uint32_t res;

    while(1) {
        res = (upper + lower) >> 1;  // average of upper and lower
 64a:	0001      	movs	r1, r0
 64c:	e7f7      	b.n	63e <divide_by_60+0xa>
        uint32_t comp = res * 60;

        if(source < comp) {
            upper = res;
        }
        else if(source - comp < 60) {
 64e:	1ad3      	subs	r3, r2, r3
 650:	2b3b      	cmp	r3, #59	; 0x3b
 652:	d901      	bls.n	658 <divide_by_60+0x24>
    uint32_t upper = source >> 5;   // divide by 32
    uint32_t lower = source >> 6;   // divide by 64
    uint32_t res;

    while(1) {
        res = (upper + lower) >> 1;  // average of upper and lower
 654:	0004      	movs	r4, r0
 656:	e7f2      	b.n	63e <divide_by_60+0xa>
        else {
            lower = res;
        }
    }
    return res;
}
 658:	bd30      	pop	{r4, r5, pc}

Disassembly of section .text.right_shift:

0000065a <right_shift>:
 **********************************************/

#define LONG_INT_LEN 4
#define LOG2_RES 5

uint64_t right_shift(uint64_t input, int8_t shift) {
 65a:	b570      	push	{r4, r5, r6, lr}
 65c:	2300      	movs	r3, #0
    int8_t i;
    if(shift >= 0) {
 65e:	2a00      	cmp	r2, #0
 660:	db0a      	blt.n	678 <right_shift+0x1e>
        for(i = 0; i < shift; i++) {
 662:	4293      	cmp	r3, r2
 664:	d00e      	beq.n	684 <right_shift+0x2a>
        input = input >> 1;
 666:	07ce      	lsls	r6, r1, #31
 668:	0845      	lsrs	r5, r0, #1
 66a:	084c      	lsrs	r4, r1, #1
 66c:	432e      	orrs	r6, r5
 66e:	3301      	adds	r3, #1
 670:	0030      	movs	r0, r6
 672:	0021      	movs	r1, r4
 674:	b25b      	sxtb	r3, r3
 676:	e7f4      	b.n	662 <right_shift+0x8>
 678:	3b01      	subs	r3, #1
 67a:	b25b      	sxtb	r3, r3
    }
    }
    else {
        for(i = 0; i > shift; i--) {
        input = input << 1;
 67c:	1800      	adds	r0, r0, r0
 67e:	4149      	adcs	r1, r1
        for(i = 0; i < shift; i++) {
        input = input >> 1;
    }
    }
    else {
        for(i = 0; i > shift; i--) {
 680:	429a      	cmp	r2, r3
 682:	d1f9      	bne.n	678 <right_shift+0x1e>
        input = input << 1;
    }
    }
    return input;
}
 684:	bd70      	pop	{r4, r5, r6, pc}

Disassembly of section .text.lnt_stop:

00000688 <lnt_stop>:
    delay(MBUS_DELAY*100);
}

static void lnt_stop() {
    // // Change Counting Time 
    mbus_remote_register_write(LNT_ADDR,0x03,0xFFFFFF);
 688:	2103      	movs	r1, #3
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*100);
}

static void lnt_stop() {
 68a:	b510      	push	{r4, lr}
    // // Change Counting Time 
    mbus_remote_register_write(LNT_ADDR,0x03,0xFFFFFF);
 68c:	0008      	movs	r0, r1
 68e:	4a18      	ldr	r2, [pc, #96]	; (6f0 <lnt_stop+0x68>)
 690:	f7ff fd49 	bl	126 <mbus_remote_register_write>
    
    set_halt_until_mbus_trx();
 694:	f7ff fde8 	bl	268 <set_halt_until_mbus_trx>
    mbus_copy_registers_from_remote_to_local(LNT_ADDR, 0x10, 0x00, 1);
 698:	2301      	movs	r3, #1
 69a:	2200      	movs	r2, #0
 69c:	2110      	movs	r1, #16
 69e:	2003      	movs	r0, #3
 6a0:	f7ff fd30 	bl	104 <mbus_copy_registers_from_remote_to_local>
    set_halt_until_mbus_tx();
 6a4:	f7ff fdda 	bl	25c <set_halt_until_mbus_tx>
    lnt_sys_light = right_shift(((*REG1 & 0xFFFFFF) << 24) | (*REG0), lnt_counter_base); // >> lnt_counter_base;
 6a8:	22a0      	movs	r2, #160	; 0xa0
 6aa:	2400      	movs	r4, #0
 6ac:	4b11      	ldr	r3, [pc, #68]	; (6f4 <lnt_stop+0x6c>)
 6ae:	0612      	lsls	r2, r2, #24
 6b0:	681b      	ldr	r3, [r3, #0]
 6b2:	6810      	ldr	r0, [r2, #0]
 6b4:	4a10      	ldr	r2, [pc, #64]	; (6f8 <lnt_stop+0x70>)
 6b6:	061b      	lsls	r3, r3, #24
 6b8:	8812      	ldrh	r2, [r2, #0]
 6ba:	4318      	orrs	r0, r3
 6bc:	b252      	sxtb	r2, r2
 6be:	0021      	movs	r1, r4
 6c0:	f7ff ffcb 	bl	65a <right_shift>
 6c4:	4b0d      	ldr	r3, [pc, #52]	; (6fc <lnt_stop+0x74>)
 6c6:	6018      	str	r0, [r3, #0]
 6c8:	6059      	str	r1, [r3, #4]

    // // Stop FSM: Counter Idle -> Counter Counting 
    lntv1a_r00.DBE_ENABLE = 0x0; // Default : 0x0
 6ca:	2110      	movs	r1, #16
 6cc:	4b0c      	ldr	r3, [pc, #48]	; (700 <lnt_stop+0x78>)
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x0;
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
 6ce:	2003      	movs	r0, #3
    mbus_copy_registers_from_remote_to_local(LNT_ADDR, 0x10, 0x00, 1);
    set_halt_until_mbus_tx();
    lnt_sys_light = right_shift(((*REG1 & 0xFFFFFF) << 24) | (*REG0), lnt_counter_base); // >> lnt_counter_base;

    // // Stop FSM: Counter Idle -> Counter Counting 
    lntv1a_r00.DBE_ENABLE = 0x0; // Default : 0x0
 6d0:	781a      	ldrb	r2, [r3, #0]
 6d2:	438a      	bics	r2, r1
 6d4:	701a      	strb	r2, [r3, #0]
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x0;
 6d6:	781a      	ldrb	r2, [r3, #0]
 6d8:	3130      	adds	r1, #48	; 0x30
 6da:	438a      	bics	r2, r1
 6dc:	701a      	strb	r2, [r3, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
 6de:	681a      	ldr	r2, [r3, #0]
 6e0:	0021      	movs	r1, r4
 6e2:	f7ff fd20 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*100);
 6e6:	4807      	ldr	r0, [pc, #28]	; (704 <lnt_stop+0x7c>)
 6e8:	f7ff fd5a 	bl	1a0 <delay>
}
 6ec:	bd10      	pop	{r4, pc}
 6ee:	46c0      	nop			; (mov r8, r8)
 6f0:	00ffffff 	.word	0x00ffffff
 6f4:	a0000004 	.word	0xa0000004
 6f8:	00003a40 	.word	0x00003a40
 6fc:	00003a48 	.word	0x00003a48
 700:	000038ac 	.word	0x000038ac
 704:	00002710 	.word	0x00002710

Disassembly of section .text.right_shift_arr.part.0:

00000708 <right_shift_arr.part.0>:
// right shift an array
// + is right shift, - is left shift
// if shift_len is greater than or equal to 32, data must be 0
// if right_shift, data must be 0
// if left shift, then shift data into the array
void right_shift_arr(uint32_t* arr, uint32_t data, uint16_t arr_len, int16_t shift_len) {
 708:	b5f0      	push	{r4, r5, r6, r7, lr}
 70a:	b089      	sub	sp, #36	; 0x24
 70c:	9001      	str	r0, [sp, #4]
 70e:	9104      	str	r1, [sp, #16]
 710:	0014      	movs	r4, r2
 712:	001e      	movs	r6, r3
    if(shift_len == 0) {
        return;
    }
    else if(shift_len >= 32) {
 714:	2b1f      	cmp	r3, #31
 716:	dc02      	bgt.n	71e <right_shift_arr.part.0+0x16>
        data = 0;
    }

    int8_t sign = shift_len > 0? 1 : -1;
 718:	2b00      	cmp	r3, #0
 71a:	dd62      	ble.n	7e2 <right_shift_arr.part.0+0xda>
 71c:	e001      	b.n	722 <right_shift_arr.part.0+0x1a>
void right_shift_arr(uint32_t* arr, uint32_t data, uint16_t arr_len, int16_t shift_len) {
    if(shift_len == 0) {
        return;
    }
    else if(shift_len >= 32) {
        data = 0;
 71e:	2300      	movs	r3, #0
 720:	9304      	str	r3, [sp, #16]
// right shift an array
// + is right shift, - is left shift
// if shift_len is greater than or equal to 32, data must be 0
// if right_shift, data must be 0
// if left shift, then shift data into the array
void right_shift_arr(uint32_t* arr, uint32_t data, uint16_t arr_len, int16_t shift_len) {
 722:	2701      	movs	r7, #1
    if(sign == -1) {
        shift_len = -shift_len;
    }

    // filter data
    data &= (right_shift(1, -shift_len) - 1);
 724:	4272      	negs	r2, r6
 726:	b252      	sxtb	r2, r2
 728:	2001      	movs	r0, #1
 72a:	2100      	movs	r1, #0
 72c:	f7ff ff95 	bl	65a <right_shift>
 730:	3c01      	subs	r4, #1
 732:	9007      	str	r0, [sp, #28]

    uint16_t start, back;

    if(sign == 1) {
 734:	2f01      	cmp	r7, #1
 736:	d109      	bne.n	74c <right_shift_arr.part.0+0x44>
        start = arr_len - 1;
        back = 0;
 738:	2300      	movs	r3, #0
    data &= (right_shift(1, -shift_len) - 1);

    uint16_t start, back;

    if(sign == 1) {
        start = arr_len - 1;
 73a:	b2a4      	uxth	r4, r4
        back = 0;
 73c:	9302      	str	r3, [sp, #8]
    // shift word by word
    while(shift_len >= 32) {
        for(i = start; i != back; i -= sign) {
            arr[i] = arr[i - sign];
        }
        arr[back] = 0;
 73e:	9b02      	ldr	r3, [sp, #8]
 740:	009d      	lsls	r5, r3, #2
 742:	9b01      	ldr	r3, [sp, #4]
 744:	195d      	adds	r5, r3, r5
 746:	2300      	movs	r3, #0
 748:	469c      	mov	ip, r3
 74a:	e007      	b.n	75c <right_shift_arr.part.0+0x54>
        start = arr_len - 1;
        back = 0;
    }
    else {
        start = 0;
        back = arr_len - 1;
 74c:	b2a3      	uxth	r3, r4
 74e:	9302      	str	r3, [sp, #8]
    if(sign == 1) {
        start = arr_len - 1;
        back = 0;
    }
    else {
        start = 0;
 750:	2400      	movs	r4, #0
 752:	e7f4      	b.n	73e <right_shift_arr.part.0+0x36>
    // shift word by word
    while(shift_len >= 32) {
        for(i = start; i != back; i -= sign) {
            arr[i] = arr[i - sign];
        }
        arr[back] = 0;
 754:	4663      	mov	r3, ip
 756:	3e20      	subs	r6, #32
 758:	602b      	str	r3, [r5, #0]
 75a:	b236      	sxth	r6, r6
    }

    uint16_t i;

    // shift word by word
    while(shift_len >= 32) {
 75c:	2e1f      	cmp	r6, #31
 75e:	dd0b      	ble.n	778 <right_shift_arr.part.0+0x70>
 760:	0023      	movs	r3, r4
        for(i = start; i != back; i -= sign) {
 762:	9a02      	ldr	r2, [sp, #8]
 764:	4293      	cmp	r3, r2
 766:	d0f5      	beq.n	754 <right_shift_arr.part.0+0x4c>
            arr[i] = arr[i - sign];
 768:	1bda      	subs	r2, r3, r7
 76a:	9901      	ldr	r1, [sp, #4]
 76c:	0090      	lsls	r0, r2, #2
 76e:	5840      	ldr	r0, [r0, r1]
 770:	009b      	lsls	r3, r3, #2
 772:	5058      	str	r0, [r3, r1]
 774:	b293      	uxth	r3, r2
 776:	e7f4      	b.n	762 <right_shift_arr.part.0+0x5a>
        }
        arr[back] = 0;
        shift_len -= 32;
    }

    int8_t shift_len_complement = shift_len - 32;
 778:	0033      	movs	r3, r6
 77a:	3b20      	subs	r3, #32
 77c:	b2db      	uxtb	r3, r3

    if(sign == -1) {
 77e:	1c7a      	adds	r2, r7, #1
 780:	d102      	bne.n	788 <right_shift_arr.part.0+0x80>
        shift_len = -shift_len;
 782:	4276      	negs	r6, r6
 784:	b236      	sxth	r6, r6
        shift_len_complement = -shift_len_complement;
 786:	425b      	negs	r3, r3
 788:	b25b      	sxtb	r3, r3
 78a:	9303      	str	r3, [sp, #12]
 78c:	b273      	sxtb	r3, r6
 78e:	9306      	str	r3, [sp, #24]
    }

    for(i = start; i != back; i -= sign) {
 790:	9b02      	ldr	r3, [sp, #8]
 792:	429c      	cmp	r4, r3
 794:	d016      	beq.n	7c4 <right_shift_arr.part.0+0xbc>
        arr[i] = right_shift(arr[i], shift_len);
 796:	9b01      	ldr	r3, [sp, #4]
 798:	00a6      	lsls	r6, r4, #2
 79a:	199e      	adds	r6, r3, r6
 79c:	6830      	ldr	r0, [r6, #0]
 79e:	9a06      	ldr	r2, [sp, #24]
 7a0:	2100      	movs	r1, #0
 7a2:	f7ff ff5a 	bl	65a <right_shift>
        arr[i] |= right_shift(arr[i - sign], shift_len_complement);
 7a6:	1be4      	subs	r4, r4, r7
 7a8:	9a01      	ldr	r2, [sp, #4]
 7aa:	00a3      	lsls	r3, r4, #2
        shift_len = -shift_len;
        shift_len_complement = -shift_len_complement;
    }

    for(i = start; i != back; i -= sign) {
        arr[i] = right_shift(arr[i], shift_len);
 7ac:	6030      	str	r0, [r6, #0]
 7ae:	9005      	str	r0, [sp, #20]
        arr[i] |= right_shift(arr[i - sign], shift_len_complement);
 7b0:	2100      	movs	r1, #0
 7b2:	5898      	ldr	r0, [r3, r2]
 7b4:	9a03      	ldr	r2, [sp, #12]
 7b6:	f7ff ff50 	bl	65a <right_shift>
 7ba:	9b05      	ldr	r3, [sp, #20]
 7bc:	b2a4      	uxth	r4, r4
 7be:	4303      	orrs	r3, r0
 7c0:	6033      	str	r3, [r6, #0]
 7c2:	e7e5      	b.n	790 <right_shift_arr.part.0+0x88>
    }

    arr[back] = right_shift(arr[back], shift_len);
 7c4:	9a06      	ldr	r2, [sp, #24]
 7c6:	6828      	ldr	r0, [r5, #0]
 7c8:	2100      	movs	r1, #0
 7ca:	f7ff ff46 	bl	65a <right_shift>
    if(sign == -1) {
 7ce:	1c7b      	adds	r3, r7, #1
 7d0:	d104      	bne.n	7dc <right_shift_arr.part.0+0xd4>
        arr[back] |= data;
 7d2:	9b07      	ldr	r3, [sp, #28]
 7d4:	9a04      	ldr	r2, [sp, #16]
 7d6:	3b01      	subs	r3, #1
 7d8:	4013      	ands	r3, r2
 7da:	4318      	orrs	r0, r3
 7dc:	6028      	str	r0, [r5, #0]
    }
}
 7de:	b009      	add	sp, #36	; 0x24
 7e0:	bdf0      	pop	{r4, r5, r6, r7, pc}
    }
    else if(shift_len >= 32) {
        data = 0;
    }

    int8_t sign = shift_len > 0? 1 : -1;
 7e2:	2701      	movs	r7, #1

    // get abs(shift_len)
    if(sign == -1) {
        shift_len = -shift_len;
 7e4:	425e      	negs	r6, r3
 7e6:	b236      	sxth	r6, r6
    }
    else if(shift_len >= 32) {
        data = 0;
    }

    int8_t sign = shift_len > 0? 1 : -1;
 7e8:	427f      	negs	r7, r7
 7ea:	e79b      	b.n	724 <right_shift_arr.part.0+0x1c>

Disassembly of section .text.right_shift_arr:

000007ec <right_shift_arr>:
// right shift an array
// + is right shift, - is left shift
// if shift_len is greater than or equal to 32, data must be 0
// if right_shift, data must be 0
// if left shift, then shift data into the array
void right_shift_arr(uint32_t* arr, uint32_t data, uint16_t arr_len, int16_t shift_len) {
 7ec:	b510      	push	{r4, lr}
    if(shift_len == 0) {
 7ee:	2b00      	cmp	r3, #0
 7f0:	d001      	beq.n	7f6 <right_shift_arr+0xa>
 7f2:	f7ff ff89 	bl	708 <right_shift_arr.part.0>

    arr[back] = right_shift(arr[back], shift_len);
    if(sign == -1) {
        arr[back] |= data;
    }
}
 7f6:	bd10      	pop	{r4, pc}

Disassembly of section .text.mult:

000007f8 <mult>:

uint64_t mult(uint32_t lhs, uint16_t rhs) {
 7f8:	b570      	push	{r4, r5, r6, lr}
    uint32_t res1 = 0, res2 = 0;
    res1 = (lhs >> 16) * rhs;
    res2 = (lhs & 0xFFFF) * rhs;
 7fa:	b286      	uxth	r6, r0
    return right_shift(res1, -16) + res2;
 7fc:	0c00      	lsrs	r0, r0, #16
}

uint64_t mult(uint32_t lhs, uint16_t rhs) {
    uint32_t res1 = 0, res2 = 0;
    res1 = (lhs >> 16) * rhs;
    res2 = (lhs & 0xFFFF) * rhs;
 7fe:	434e      	muls	r6, r1
    return right_shift(res1, -16) + res2;
 800:	4341      	muls	r1, r0
 802:	2300      	movs	r3, #0
 804:	000a      	movs	r2, r1
 806:	2110      	movs	r1, #16
 808:	3901      	subs	r1, #1
 80a:	b2c9      	uxtb	r1, r1
        input = input >> 1;
    }
    }
    else {
        for(i = 0; i > shift; i--) {
        input = input << 1;
 80c:	1892      	adds	r2, r2, r2
 80e:	415b      	adcs	r3, r3
        for(i = 0; i < shift; i++) {
        input = input >> 1;
    }
    }
    else {
        for(i = 0; i > shift; i--) {
 810:	2900      	cmp	r1, #0
 812:	d1f9      	bne.n	808 <mult+0x10>

uint64_t mult(uint32_t lhs, uint16_t rhs) {
    uint32_t res1 = 0, res2 = 0;
    res1 = (lhs >> 16) * rhs;
    res2 = (lhs & 0xFFFF) * rhs;
    return right_shift(res1, -16) + res2;
 814:	0030      	movs	r0, r6
 816:	1880      	adds	r0, r0, r2
 818:	4159      	adcs	r1, r3
}
 81a:	bd70      	pop	{r4, r5, r6, pc}

Disassembly of section .text.get_timer_cnt_xo:

0000081c <get_timer_cnt_xo>:

    // uint64_t temp = mult(raw_cnt, xo_to_sec_mplier);
    // return right_shift(temp, XO_TO_SEC_MPLIER_SHIFT);
    
    // v6.3.0: sync xo timer read
    uint32_t raw_count = *XOT_VAL;
 81c:	4b06      	ldr	r3, [pc, #24]	; (838 <get_timer_cnt_xo+0x1c>)

volatile uint16_t XO_TO_SEC_MPLIER_SHIFT = 15;
volatile uint16_t xo_to_sec_mplier = 32768;  // 2^15
#define XO_SYNC_THRESH 4096 // 3 consecutive reads should be less than 1/8 seconds apart

uint32_t get_timer_cnt_xo() {
 81e:	b510      	push	{r4, lr}

    // uint64_t temp = mult(raw_cnt, xo_to_sec_mplier);
    // return right_shift(temp, XO_TO_SEC_MPLIER_SHIFT);
    
    // v6.3.0: sync xo timer read
    uint32_t raw_count = *XOT_VAL;
 820:	6818      	ldr	r0, [r3, #0]
    uint64_t temp = mult(raw_count, xo_to_sec_mplier);
 822:	4b06      	ldr	r3, [pc, #24]	; (83c <get_timer_cnt_xo+0x20>)
 824:	8819      	ldrh	r1, [r3, #0]
 826:	b289      	uxth	r1, r1
 828:	f7ff ffe6 	bl	7f8 <mult>
    return right_shift(temp, XO_TO_SEC_MPLIER_SHIFT);
 82c:	4b04      	ldr	r3, [pc, #16]	; (840 <get_timer_cnt_xo+0x24>)
 82e:	881a      	ldrh	r2, [r3, #0]
 830:	b252      	sxtb	r2, r2
 832:	f7ff ff12 	bl	65a <right_shift>
}
 836:	bd10      	pop	{r4, pc}
 838:	a0001420 	.word	0xa0001420
 83c:	00003932 	.word	0x00003932
 840:	00003800 	.word	0x00003800

Disassembly of section .text.long_int_mult:

00000844 <long_int_mult>:
    for(i = 0; i < LONG_INT_LEN; i++) {
        dest[i] = src[i];
    }
}

static void long_int_mult(const long_int lhs, const uint16_t rhs, long_int res) {
 844:	b5f0      	push	{r4, r5, r6, r7, lr}
 846:	2400      	movs	r4, #0
 848:	000f      	movs	r7, r1
 84a:	0015      	movs	r5, r2
    uint32_t carry_in = 0;
 84c:	0026      	movs	r6, r4
    for(i = 0; i < LONG_INT_LEN; i++) {
        dest[i] = src[i];
    }
}

static void long_int_mult(const long_int lhs, const uint16_t rhs, long_int res) {
 84e:	b089      	sub	sp, #36	; 0x24
 850:	9003      	str	r0, [sp, #12]
    uint32_t carry_in = 0;
    uint32_t temp_res[LONG_INT_LEN];
    uint16_t i;
    for(i = 0; i < LONG_INT_LEN; i++) {
        uint64_t temp = mult(lhs[i], rhs) + carry_in;
 852:	9b03      	ldr	r3, [sp, #12]
 854:	0039      	movs	r1, r7
 856:	5918      	ldr	r0, [r3, r4]
 858:	f7ff ffce 	bl	7f8 <mult>
 85c:	2300      	movs	r3, #0
 85e:	9600      	str	r6, [sp, #0]
 860:	9301      	str	r3, [sp, #4]
 862:	9a00      	ldr	r2, [sp, #0]
 864:	9b01      	ldr	r3, [sp, #4]
 866:	1880      	adds	r0, r0, r2
 868:	4159      	adcs	r1, r3
        carry_in = temp >> 32;
        temp_res[i] = temp & 0xFFFFFFFF;
 86a:	ab04      	add	r3, sp, #16
 86c:	50e0      	str	r0, [r4, r3]
 86e:	3404      	adds	r4, #4
    uint32_t carry_in = 0;
    uint32_t temp_res[LONG_INT_LEN];
    uint16_t i;
    for(i = 0; i < LONG_INT_LEN; i++) {
        uint64_t temp = mult(lhs[i], rhs) + carry_in;
        carry_in = temp >> 32;
 870:	000e      	movs	r6, r1

static void long_int_mult(const long_int lhs, const uint16_t rhs, long_int res) {
    uint32_t carry_in = 0;
    uint32_t temp_res[LONG_INT_LEN];
    uint16_t i;
    for(i = 0; i < LONG_INT_LEN; i++) {
 872:	2c10      	cmp	r4, #16
 874:	d1ed      	bne.n	852 <long_int_mult+0xe>
typedef uint32_t* long_int;

static void long_int_assign(long_int dest, const long_int src) {
    uint16_t i;
    for(i = 0; i < LONG_INT_LEN; i++) {
        dest[i] = src[i];
 876:	9b04      	ldr	r3, [sp, #16]
 878:	602b      	str	r3, [r5, #0]
 87a:	9b05      	ldr	r3, [sp, #20]
 87c:	606b      	str	r3, [r5, #4]
 87e:	9b06      	ldr	r3, [sp, #24]
 880:	60ab      	str	r3, [r5, #8]
 882:	9b07      	ldr	r3, [sp, #28]
 884:	60eb      	str	r3, [r5, #12]
        uint64_t temp = mult(lhs[i], rhs) + carry_in;
        carry_in = temp >> 32;
        temp_res[i] = temp & 0xFFFFFFFF;
    }
    long_int_assign(res, temp_res);
}
 886:	b009      	add	sp, #36	; 0x24
 888:	bdf0      	pop	{r4, r5, r6, r7, pc}

Disassembly of section .text.log2:

0000088c <log2>:

uint16_t log2(uint64_t input) {
    // mbus_write_message32(0xE7, input & 0xFFFFFFFF);
    // mbus_write_message32(0xE8, input >> 32);

    if(input == 0) { return 0; }
 88c:	0002      	movs	r2, r0
 88e:	2300      	movs	r3, #0
    }
    }
    return false;
}

uint16_t log2(uint64_t input) {
 890:	b5f0      	push	{r4, r5, r6, r7, lr}
    // mbus_write_message32(0xE7, input & 0xFFFFFFFF);
    // mbus_write_message32(0xE8, input >> 32);

    if(input == 0) { return 0; }
 892:	430a      	orrs	r2, r1
    }
    }
    return false;
}

uint16_t log2(uint64_t input) {
 894:	0006      	movs	r6, r0
 896:	b08d      	sub	sp, #52	; 0x34
 898:	000f      	movs	r7, r1
    // mbus_write_message32(0xE7, input & 0xFFFFFFFF);
    // mbus_write_message32(0xE8, input >> 32);

    if(input == 0) { return 0; }
 89a:	0018      	movs	r0, r3
 89c:	429a      	cmp	r2, r3
 89e:	d05e      	beq.n	95e <log2+0xd2>

    input_storage[0] = input & 0xFFFFFFFF;
    input_storage[1] = input >> 32;
    uint16_t out = 0;

    for(i = 47; i >= 0; i--) {
 8a0:	252f      	movs	r5, #47	; 0x2f
        if(right_shift(input, i) & 0b1) {
 8a2:	2401      	movs	r4, #1
    if(input == 0) { return 0; }

    uint32_t temp_result[LONG_INT_LEN], input_storage[LONG_INT_LEN];
    int8_t i;
    for(i = 0; i < LONG_INT_LEN; i++) {
        temp_result[i] = input_storage[i] = 0;
 8a4:	9300      	str	r3, [sp, #0]
 8a6:	9301      	str	r3, [sp, #4]
 8a8:	9306      	str	r3, [sp, #24]
 8aa:	9302      	str	r3, [sp, #8]
 8ac:	9307      	str	r3, [sp, #28]
 8ae:	9303      	str	r3, [sp, #12]
    }

    input_storage[0] = input & 0xFFFFFFFF;
 8b0:	9604      	str	r6, [sp, #16]
    input_storage[1] = input >> 32;
 8b2:	9105      	str	r1, [sp, #20]
    uint16_t out = 0;

    for(i = 47; i >= 0; i--) {
        if(right_shift(input, i) & 0b1) {
 8b4:	002a      	movs	r2, r5
 8b6:	0030      	movs	r0, r6
 8b8:	0039      	movs	r1, r7
 8ba:	f7ff fece 	bl	65a <right_shift>
 8be:	4020      	ands	r0, r4
 8c0:	d00d      	beq.n	8de <log2+0x52>
            uint64_t temp = right_shift(1, -i);
 8c2:	426a      	negs	r2, r5
 8c4:	b252      	sxtb	r2, r2
 8c6:	2001      	movs	r0, #1
 8c8:	2100      	movs	r1, #0
 8ca:	f7ff fec6 	bl	65a <right_shift>
            temp_result[0] = temp & 0xFFFFFFFF;
            temp_result[1] = temp >> 32;
            out = i << LOG2_RES;
 8ce:	016d      	lsls	r5, r5, #5
    uint16_t out = 0;

    for(i = 47; i >= 0; i--) {
        if(right_shift(input, i) & 0b1) {
            uint64_t temp = right_shift(1, -i);
            temp_result[0] = temp & 0xFFFFFFFF;
 8d0:	9000      	str	r0, [sp, #0]
            temp_result[1] = temp >> 32;
 8d2:	9101      	str	r1, [sp, #4]
            out = i << LOG2_RES;
 8d4:	b2ac      	uxth	r4, r5
        }
    }
    for(i = 0; i < LOG2_RES; i++) {
        uint32_t new_result[4];
        long_int_mult(temp_result, LOG_CONST_ARR[i], new_result);
        long_int_mult(input_storage, (1 << 15), input_storage);
 8d6:	2680      	movs	r6, #128	; 0x80
        temp_result[i] = input_storage[i] = 0;
    }

    input_storage[0] = input & 0xFFFFFFFF;
    input_storage[1] = input >> 32;
    uint16_t out = 0;
 8d8:	2500      	movs	r5, #0
        }
    }
    for(i = 0; i < LOG2_RES; i++) {
        uint32_t new_result[4];
        long_int_mult(temp_result, LOG_CONST_ARR[i], new_result);
        long_int_mult(input_storage, (1 << 15), input_storage);
 8da:	0236      	lsls	r6, r6, #8
 8dc:	e005      	b.n	8ea <log2+0x5e>
 8de:	3d01      	subs	r5, #1
 8e0:	b26d      	sxtb	r5, r5

    input_storage[0] = input & 0xFFFFFFFF;
    input_storage[1] = input >> 32;
    uint16_t out = 0;

    for(i = 47; i >= 0; i--) {
 8e2:	1c6b      	adds	r3, r5, #1
 8e4:	d1e6      	bne.n	8b4 <log2+0x28>
        temp_result[i] = input_storage[i] = 0;
    }

    input_storage[0] = input & 0xFFFFFFFF;
    input_storage[1] = input >> 32;
    uint16_t out = 0;
 8e6:	0004      	movs	r4, r0
 8e8:	e7f5      	b.n	8d6 <log2+0x4a>
            break;
        }
    }
    for(i = 0; i < LOG2_RES; i++) {
        uint32_t new_result[4];
        long_int_mult(temp_result, LOG_CONST_ARR[i], new_result);
 8ea:	4b1e      	ldr	r3, [pc, #120]	; (964 <log2+0xd8>)
 8ec:	006a      	lsls	r2, r5, #1
 8ee:	5ad1      	ldrh	r1, [r2, r3]
 8f0:	4668      	mov	r0, sp
 8f2:	aa08      	add	r2, sp, #32
 8f4:	f7ff ffa6 	bl	844 <long_int_mult>
        long_int_mult(input_storage, (1 << 15), input_storage);
 8f8:	aa04      	add	r2, sp, #16
 8fa:	0031      	movs	r1, r6
 8fc:	0010      	movs	r0, r2
 8fe:	f7ff ffa1 	bl	844 <long_int_mult>
}

static bool long_int_lte(const long_int lhs, const long_int rhs) {
    int8_t i;
    for(i = 3; i >= 0; i--) {
    if(lhs[i] != rhs[i]) {
 902:	990b      	ldr	r1, [sp, #44]	; 0x2c
 904:	9b07      	ldr	r3, [sp, #28]
 906:	4299      	cmp	r1, r3
 908:	d10c      	bne.n	924 <log2+0x98>
 90a:	9a0a      	ldr	r2, [sp, #40]	; 0x28
 90c:	9b06      	ldr	r3, [sp, #24]
 90e:	4293      	cmp	r3, r2
 910:	d109      	bne.n	926 <log2+0x9a>
 912:	9a09      	ldr	r2, [sp, #36]	; 0x24
 914:	9b05      	ldr	r3, [sp, #20]
 916:	429a      	cmp	r2, r3
 918:	d105      	bne.n	926 <log2+0x9a>
 91a:	9a08      	ldr	r2, [sp, #32]
 91c:	9b04      	ldr	r3, [sp, #16]
 91e:	429a      	cmp	r2, r3
 920:	d013      	beq.n	94a <log2+0xbe>
 922:	e000      	b.n	926 <log2+0x9a>
 924:	000a      	movs	r2, r1
    for(i = 0; i < LOG2_RES; i++) {
        uint32_t new_result[4];
        long_int_mult(temp_result, LOG_CONST_ARR[i], new_result);
        long_int_mult(input_storage, (1 << 15), input_storage);

        if(long_int_lte(new_result, input_storage)) {
 926:	4293      	cmp	r3, r2
 928:	d90f      	bls.n	94a <log2+0xbe>
typedef uint32_t* long_int;

static void long_int_assign(long_int dest, const long_int src) {
    uint16_t i;
    for(i = 0; i < LONG_INT_LEN; i++) {
        dest[i] = src[i];
 92a:	9b08      	ldr	r3, [sp, #32]
        long_int_mult(input_storage, (1 << 15), input_storage);

        if(long_int_lte(new_result, input_storage)) {
            long_int_assign(temp_result, new_result);
            // out |= (1 << (LOG2_RES - 1 - i));
        out |= right_shift(1, -(LOG2_RES - 1 - i));
 92c:	1f2a      	subs	r2, r5, #4
typedef uint32_t* long_int;

static void long_int_assign(long_int dest, const long_int src) {
    uint16_t i;
    for(i = 0; i < LONG_INT_LEN; i++) {
        dest[i] = src[i];
 92e:	9300      	str	r3, [sp, #0]
 930:	9b09      	ldr	r3, [sp, #36]	; 0x24
 932:	9103      	str	r1, [sp, #12]
 934:	9301      	str	r3, [sp, #4]
 936:	9b0a      	ldr	r3, [sp, #40]	; 0x28
        long_int_mult(input_storage, (1 << 15), input_storage);

        if(long_int_lte(new_result, input_storage)) {
            long_int_assign(temp_result, new_result);
            // out |= (1 << (LOG2_RES - 1 - i));
        out |= right_shift(1, -(LOG2_RES - 1 - i));
 938:	b252      	sxtb	r2, r2
 93a:	2001      	movs	r0, #1
 93c:	2100      	movs	r1, #0
typedef uint32_t* long_int;

static void long_int_assign(long_int dest, const long_int src) {
    uint16_t i;
    for(i = 0; i < LONG_INT_LEN; i++) {
        dest[i] = src[i];
 93e:	9302      	str	r3, [sp, #8]
        long_int_mult(input_storage, (1 << 15), input_storage);

        if(long_int_lte(new_result, input_storage)) {
            long_int_assign(temp_result, new_result);
            // out |= (1 << (LOG2_RES - 1 - i));
        out |= right_shift(1, -(LOG2_RES - 1 - i));
 940:	f7ff fe8b 	bl	65a <right_shift>
 944:	4304      	orrs	r4, r0
 946:	b2a4      	uxth	r4, r4
 948:	e004      	b.n	954 <log2+0xc8>
        }
        else {
            long_int_mult(temp_result, (1 << 15), temp_result);
 94a:	466a      	mov	r2, sp
 94c:	0031      	movs	r1, r6
 94e:	4668      	mov	r0, sp
 950:	f7ff ff78 	bl	844 <long_int_mult>
 954:	3501      	adds	r5, #1
            temp_result[1] = temp >> 32;
            out = i << LOG2_RES;
            break;
        }
    }
    for(i = 0; i < LOG2_RES; i++) {
 956:	2d05      	cmp	r5, #5
 958:	d1c7      	bne.n	8ea <log2+0x5e>
        }

    }

    // mbus_write_message32(0xE9, out & 0x7FF);
    return out & 0x7FF;
 95a:	0560      	lsls	r0, r4, #21
 95c:	0d40      	lsrs	r0, r0, #21
}
 95e:	b00d      	add	sp, #52	; 0x34
 960:	bdf0      	pop	{r4, r5, r6, r7, pc}
 962:	46c0      	nop			; (mov r8, r8)
 964:	000037d0 	.word	0x000037d0

Disassembly of section .text.store_code:

00000968 <store_code>:
#define LIGHT_TIME_LENGTH 17    // 17 bit time in min
bool has_header = false;
bool has_time = false;

// requires code_cache to have enough space
void store_code(int32_t code, uint16_t len) {
 968:	b510      	push	{r4, lr}
 96a:	000c      	movs	r4, r1
    right_shift_arr(code_cache, code, CODE_CACHE_LEN, -len);
 96c:	424b      	negs	r3, r1
 96e:	b21b      	sxth	r3, r3
 970:	0001      	movs	r1, r0
 972:	2209      	movs	r2, #9
 974:	4803      	ldr	r0, [pc, #12]	; (984 <store_code+0x1c>)
 976:	f7ff ff39 	bl	7ec <right_shift_arr>
    code_cache_remainder -= len;
 97a:	4b03      	ldr	r3, [pc, #12]	; (988 <store_code+0x20>)
 97c:	8819      	ldrh	r1, [r3, #0]
 97e:	1b09      	subs	r1, r1, r4
 980:	8019      	strh	r1, [r3, #0]
}
 982:	bd10      	pop	{r4, pc}
 984:	000039f4 	.word	0x000039f4
 988:	000037f8 	.word	0x000037f8

Disassembly of section .text.flush_code_cache:

0000098c <flush_code_cache>:

void flush_code_cache() {
    if(code_cache_remainder < CODE_CACHE_MAX_REMAINDER) {
 98c:	2210      	movs	r2, #16
void store_code(int32_t code, uint16_t len) {
    right_shift_arr(code_cache, code, CODE_CACHE_LEN, -len);
    code_cache_remainder -= len;
}

void flush_code_cache() {
 98e:	b570      	push	{r4, r5, r6, lr}
    if(code_cache_remainder < CODE_CACHE_MAX_REMAINDER) {
 990:	4c14      	ldr	r4, [pc, #80]	; (9e4 <flush_code_cache+0x58>)
 992:	32ff      	adds	r2, #255	; 0xff
 994:	8823      	ldrh	r3, [r4, #0]
 996:	4293      	cmp	r3, r2
 998:	d823      	bhi.n	9e2 <flush_code_cache+0x56>
        // this leaves exactly 16 bits of blank space at the front of the code cache
        right_shift_arr(code_cache, 0, CODE_CACHE_LEN, -code_cache_remainder);
 99a:	4d13      	ldr	r5, [pc, #76]	; (9e8 <flush_code_cache+0x5c>)
 99c:	425b      	negs	r3, r3
 99e:	3a07      	subs	r2, #7
 9a0:	b21b      	sxth	r3, r3
 9a2:	3aff      	subs	r2, #255	; 0xff
 9a4:	2100      	movs	r1, #0
 9a6:	0028      	movs	r0, r5
 9a8:	f7ff ff20 	bl	7ec <right_shift_arr>
        // The beginning of code cache should always be 1
        // Mark as light
        code_cache[0] &= 0x0000FFFF;    // FIXME: this line is not needed
        code_cache[0] |= 0x80000000;
 9ac:	2280      	movs	r2, #128	; 0x80
 9ae:	882b      	ldrh	r3, [r5, #0]

        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, code_cache, CODE_CACHE_LEN - 1);
 9b0:	4e0e      	ldr	r6, [pc, #56]	; (9ec <flush_code_cache+0x60>)
        // this leaves exactly 16 bits of blank space at the front of the code cache
        right_shift_arr(code_cache, 0, CODE_CACHE_LEN, -code_cache_remainder);
        // The beginning of code cache should always be 1
        // Mark as light
        code_cache[0] &= 0x0000FFFF;    // FIXME: this line is not needed
        code_cache[0] |= 0x80000000;
 9b2:	0612      	lsls	r2, r2, #24
 9b4:	4313      	orrs	r3, r2
 9b6:	602b      	str	r3, [r5, #0]

        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, code_cache, CODE_CACHE_LEN - 1);
 9b8:	002a      	movs	r2, r5
 9ba:	2308      	movs	r3, #8
 9bc:	8831      	ldrh	r1, [r6, #0]
 9be:	2006      	movs	r0, #6
 9c0:	f7ff fbbe 	bl	140 <mbus_copy_mem_from_local_to_remote_bulk>
        code_addr += CODE_CACHE_LEN << 2;
 9c4:	8833      	ldrh	r3, [r6, #0]
        max_unit_count++;
 9c6:	4a0a      	ldr	r2, [pc, #40]	; (9f0 <flush_code_cache+0x64>)
        // Mark as light
        code_cache[0] &= 0x0000FFFF;    // FIXME: this line is not needed
        code_cache[0] |= 0x80000000;

        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, code_cache, CODE_CACHE_LEN - 1);
        code_addr += CODE_CACHE_LEN << 2;
 9c8:	3324      	adds	r3, #36	; 0x24
 9ca:	8033      	strh	r3, [r6, #0]
        max_unit_count++;
 9cc:	8813      	ldrh	r3, [r2, #0]
 9ce:	3301      	adds	r3, #1
 9d0:	8013      	strh	r3, [r2, #0]
        code_cache_remainder = CODE_CACHE_MAX_REMAINDER;
 9d2:	2388      	movs	r3, #136	; 0x88
 9d4:	005b      	lsls	r3, r3, #1
 9d6:	8023      	strh	r3, [r4, #0]

        has_header = false;
 9d8:	2300      	movs	r3, #0
 9da:	4a06      	ldr	r2, [pc, #24]	; (9f4 <flush_code_cache+0x68>)
 9dc:	7013      	strb	r3, [r2, #0]
        has_time = false;
 9de:	4a06      	ldr	r2, [pc, #24]	; (9f8 <flush_code_cache+0x6c>)
 9e0:	7013      	strb	r3, [r2, #0]
    }
}
 9e2:	bd70      	pop	{r4, r5, r6, pc}
 9e4:	000037f8 	.word	0x000037f8
 9e8:	000039f4 	.word	0x000039f4
 9ec:	00003ae8 	.word	0x00003ae8
 9f0:	00003a42 	.word	0x00003a42
 9f4:	00003aa1 	.word	0x00003aa1
 9f8:	00003a74 	.word	0x00003a74

Disassembly of section .text.store_to_code_cache:

000009fc <store_to_code_cache>:

void store_to_code_cache(uint16_t log_light, uint16_t start_idx, uint32_t starting_time_in_min) {
 9fc:	b5f0      	push	{r4, r5, r6, r7, lr}
 9fe:	0005      	movs	r5, r0
    uint16_t code_idx = 0;
    uint16_t len_needed = 0;
    uint16_t code = 0;
    int16_t diff = log_light - last_log_light;
 a00:	4b39      	ldr	r3, [pc, #228]	; (ae8 <store_to_code_cache+0xec>)
        has_header = false;
        has_time = false;
    }
}

void store_to_code_cache(uint16_t log_light, uint16_t start_idx, uint32_t starting_time_in_min) {
 a02:	0017      	movs	r7, r2
    uint16_t code_idx = 0;
    uint16_t len_needed = 0;
    uint16_t code = 0;
    int16_t diff = log_light - last_log_light;
 a04:	8818      	ldrh	r0, [r3, #0]
        has_header = false;
        has_time = false;
    }
}

void store_to_code_cache(uint16_t log_light, uint16_t start_idx, uint32_t starting_time_in_min) {
 a06:	b085      	sub	sp, #20
    uint16_t code_idx = 0;
    uint16_t len_needed = 0;
    uint16_t code = 0;
    int16_t diff = log_light - last_log_light;
 a08:	1a28      	subs	r0, r5, r0
 a0a:	b282      	uxth	r2, r0
    last_log_light = log_light;

    // calculate len needed
    if(diff < 32 && diff >= -32) {
 a0c:	0014      	movs	r4, r2
 a0e:	3420      	adds	r4, #32
 a10:	b2a4      	uxth	r4, r4
        has_header = false;
        has_time = false;
    }
}

void store_to_code_cache(uint16_t log_light, uint16_t start_idx, uint32_t starting_time_in_min) {
 a12:	9102      	str	r1, [sp, #8]
    uint16_t code_idx = 0;
    uint16_t len_needed = 0;
    uint16_t code = 0;
    int16_t diff = log_light - last_log_light;
 a14:	9201      	str	r2, [sp, #4]
    last_log_light = log_light;
 a16:	801d      	strh	r5, [r3, #0]

    // calculate len needed
    if(diff < 32 && diff >= -32) {
 a18:	2c3f      	cmp	r4, #63	; 0x3f
 a1a:	d803      	bhi.n	a24 <store_to_code_cache+0x28>
        code_idx = diff + 32;
        len_needed = light_code_lengths[code_idx];
 a1c:	4b33      	ldr	r3, [pc, #204]	; (aec <store_to_code_cache+0xf0>)
 a1e:	0062      	lsls	r2, r4, #1
 a20:	5ad3      	ldrh	r3, [r2, r3]
 a22:	e00b      	b.n	a3c <store_to_code_cache+0x40>
    }
    else if(diff < 256 && diff >= -256) {
 a24:	9b01      	ldr	r3, [sp, #4]
 a26:	4a32      	ldr	r2, [pc, #200]	; (af0 <store_to_code_cache+0xf4>)
 a28:	3301      	adds	r3, #1
 a2a:	33ff      	adds	r3, #255	; 0xff
 a2c:	b29b      	uxth	r3, r3
 a2e:	4293      	cmp	r3, r2
 a30:	d802      	bhi.n	a38 <store_to_code_cache+0x3c>
        code_idx = 64;
        len_needed = light_code_lengths[code_idx] + 9;
 a32:	230d      	movs	r3, #13
    if(diff < 32 && diff >= -32) {
        code_idx = diff + 32;
        len_needed = light_code_lengths[code_idx];
    }
    else if(diff < 256 && diff >= -256) {
        code_idx = 64;
 a34:	2440      	movs	r4, #64	; 0x40
 a36:	e001      	b.n	a3c <store_to_code_cache+0x40>
        len_needed = light_code_lengths[code_idx] + 9;
    }
    else {
        code_idx = 65;
        len_needed = light_code_lengths[code_idx] + 11;
 a38:	2312      	movs	r3, #18
    else if(diff < 256 && diff >= -256) {
        code_idx = 64;
        len_needed = light_code_lengths[code_idx] + 9;
    }
    else {
        code_idx = 65;
 a3a:	2441      	movs	r4, #65	; 0x41
        len_needed = light_code_lengths[code_idx] + 11;
    }
    code = light_diff_codes[code_idx];
 a3c:	4a2d      	ldr	r2, [pc, #180]	; (af4 <store_to_code_cache+0xf8>)
 a3e:	0061      	lsls	r1, r4, #1
 a40:	5a8a      	ldrh	r2, [r1, r2]

    if(!has_time) {
 a42:	4e2d      	ldr	r6, [pc, #180]	; (af8 <store_to_code_cache+0xfc>)
    }
    else {
        code_idx = 65;
        len_needed = light_code_lengths[code_idx] + 11;
    }
    code = light_diff_codes[code_idx];
 a44:	9203      	str	r2, [sp, #12]

    if(!has_time) {
 a46:	7832      	ldrb	r2, [r6, #0]
 a48:	2a00      	cmp	r2, #0
 a4a:	d101      	bne.n	a50 <store_to_code_cache+0x54>
        len_needed += LIGHT_TIME_LENGTH;
 a4c:	3311      	adds	r3, #17
 a4e:	b29b      	uxth	r3, r3
    }

    if(!has_header) {
 a50:	4a2a      	ldr	r2, [pc, #168]	; (afc <store_to_code_cache+0x100>)
 a52:	7812      	ldrb	r2, [r2, #0]
 a54:	2a00      	cmp	r2, #0
 a56:	d101      	bne.n	a5c <store_to_code_cache+0x60>
        len_needed += UNIT_HEADER_SIZE;
 a58:	3309      	adds	r3, #9
 a5a:	b29b      	uxth	r3, r3
    }

    if(code_cache_remainder < len_needed) {
 a5c:	4a28      	ldr	r2, [pc, #160]	; (b00 <store_to_code_cache+0x104>)
 a5e:	8812      	ldrh	r2, [r2, #0]
 a60:	429a      	cmp	r2, r3
 a62:	d201      	bcs.n	a68 <store_to_code_cache+0x6c>
        flush_code_cache();
 a64:	f7ff ff92 	bl	98c <flush_code_cache>
    }

    // 17 starting timestamp is separate for each day state
    if(!has_time) {
 a68:	7833      	ldrb	r3, [r6, #0]
 a6a:	2b00      	cmp	r3, #0
 a6c:	d10a      	bne.n	a84 <store_to_code_cache+0x88>
        store_code(starting_time_in_min & 0x1FFFF, 17);
 a6e:	03f8      	lsls	r0, r7, #15
 a70:	0bc0      	lsrs	r0, r0, #15
 a72:	2111      	movs	r1, #17
 a74:	f7ff ff78 	bl	968 <store_code>
        mbus_write_message32(0xC4, starting_time_in_min);
 a78:	0039      	movs	r1, r7
 a7a:	20c4      	movs	r0, #196	; 0xc4
 a7c:	f7ff fb14 	bl	a8 <mbus_write_message32>
        has_time = true;
 a80:	2301      	movs	r3, #1
 a82:	7033      	strb	r3, [r6, #0]
    }

    // 2 bit day_state, 7 bit starting idx
    if(!has_header) {
 a84:	4f1d      	ldr	r7, [pc, #116]	; (afc <store_to_code_cache+0x100>)
 a86:	783b      	ldrb	r3, [r7, #0]
 a88:	2b00      	cmp	r3, #0
 a8a:	d112      	bne.n	ab2 <store_to_code_cache+0xb6>
        store_code(day_state, 2);
 a8c:	4e1d      	ldr	r6, [pc, #116]	; (b04 <store_to_code_cache+0x108>)
 a8e:	2102      	movs	r1, #2
 a90:	8830      	ldrh	r0, [r6, #0]
 a92:	b280      	uxth	r0, r0
 a94:	f7ff ff68 	bl	968 <store_code>
        store_code(start_idx, 7);
 a98:	2107      	movs	r1, #7
 a9a:	9802      	ldr	r0, [sp, #8]
 a9c:	f7ff ff64 	bl	968 <store_code>
        mbus_write_message32(0xC5, (day_state << 16) | start_idx);
 aa0:	8831      	ldrh	r1, [r6, #0]
 aa2:	9b02      	ldr	r3, [sp, #8]
 aa4:	0409      	lsls	r1, r1, #16
 aa6:	4319      	orrs	r1, r3
 aa8:	20c5      	movs	r0, #197	; 0xc5
 aaa:	f7ff fafd 	bl	a8 <mbus_write_message32>
        has_header = true;
 aae:	2301      	movs	r3, #1
 ab0:	703b      	strb	r3, [r7, #0]
    }

    if(code_cache_remainder == CODE_CACHE_MAX_REMAINDER - UNIT_HEADER_SIZE - LIGHT_TIME_LENGTH) {
 ab2:	4b13      	ldr	r3, [pc, #76]	; (b00 <store_to_code_cache+0x104>)
 ab4:	881b      	ldrh	r3, [r3, #0]
 ab6:	2bf6      	cmp	r3, #246	; 0xf6
 ab8:	d102      	bne.n	ac0 <store_to_code_cache+0xc4>
        // if first data in unit, just store full data
        store_code(log_light, 11);
 aba:	210b      	movs	r1, #11
 abc:	0028      	movs	r0, r5
 abe:	e00b      	b.n	ad8 <store_to_code_cache+0xdc>
    }
    else {
        // else store difference
        store_code(code, light_code_lengths[code_idx]);
 ac0:	4b0a      	ldr	r3, [pc, #40]	; (aec <store_to_code_cache+0xf0>)
 ac2:	0062      	lsls	r2, r4, #1
 ac4:	5ad1      	ldrh	r1, [r2, r3]
 ac6:	9803      	ldr	r0, [sp, #12]
 ac8:	f7ff ff4e 	bl	968 <store_code>

        if(code_idx == 64) {
 acc:	2c40      	cmp	r4, #64	; 0x40
 ace:	d106      	bne.n	ade <store_to_code_cache+0xe2>
            store_code(diff, 9);
 ad0:	466b      	mov	r3, sp
 ad2:	2109      	movs	r1, #9
 ad4:	2004      	movs	r0, #4
 ad6:	5e18      	ldrsh	r0, [r3, r0]
 ad8:	f7ff ff46 	bl	968 <store_code>
 adc:	e001      	b.n	ae2 <store_to_code_cache+0xe6>
        }
        else if(code_idx == 65) {
 ade:	2c41      	cmp	r4, #65	; 0x41
 ae0:	d0eb      	beq.n	aba <store_to_code_cache+0xbe>
            // if storing 11 bits, just store data in full
            store_code(log_light, 11);
        }
    }

}
 ae2:	b005      	add	sp, #20
 ae4:	bdf0      	pop	{r4, r5, r6, r7, pc}
 ae6:	46c0      	nop			; (mov r8, r8)
 ae8:	00003a64 	.word	0x00003a64
 aec:	0000374a 	.word	0x0000374a
 af0:	000001ff 	.word	0x000001ff
 af4:	000036c4 	.word	0x000036c4
 af8:	00003a74 	.word	0x00003a74
 afc:	00003aa1 	.word	0x00003aa1
 b00:	000037f8 	.word	0x000037f8
 b04:	00003930 	.word	0x00003930

Disassembly of section .text.store_day_state_stop:

00000b08 <store_day_state_stop>:

void store_day_state_stop() {
    uint16_t len_needed = light_code_lengths[66];
    if(code_cache_remainder < len_needed) {
 b08:	4b06      	ldr	r3, [pc, #24]	; (b24 <store_day_state_stop+0x1c>)
        }
    }

}

void store_day_state_stop() {
 b0a:	b510      	push	{r4, lr}
    uint16_t len_needed = light_code_lengths[66];
    if(code_cache_remainder < len_needed) {
 b0c:	881b      	ldrh	r3, [r3, #0]
 b0e:	2b05      	cmp	r3, #5
 b10:	d802      	bhi.n	b18 <store_day_state_stop+0x10>
        flush_code_cache();
 b12:	f7ff ff3b 	bl	98c <flush_code_cache>
 b16:	e003      	b.n	b20 <store_day_state_stop+0x18>
    }
    else {
        store_code(light_diff_codes[66], len_needed);
 b18:	2106      	movs	r1, #6
 b1a:	2035      	movs	r0, #53	; 0x35
 b1c:	f7ff ff24 	bl	968 <store_code>
    }
}
 b20:	bd10      	pop	{r4, pc}
 b22:	46c0      	nop			; (mov r8, r8)
 b24:	000037f8 	.word	0x000037f8

Disassembly of section .text.write_to_proc_cache:

00000b28 <write_to_proc_cache>:

/**********************************************
 * Sampling functions
 **********************************************/

void write_to_proc_cache(uint16_t data) {
 b28:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 b2a:	210b      	movs	r1, #11
 b2c:	2201      	movs	r2, #1
 b2e:	2300      	movs	r3, #0
 b30:	3901      	subs	r1, #1
 b32:	b2c9      	uxtb	r1, r1
        input = input >> 1;
    }
    }
    else {
        for(i = 0; i > shift; i--) {
        input = input << 1;
 b34:	1892      	adds	r2, r2, r2
 b36:	415b      	adcs	r3, r3
        for(i = 0; i < shift; i++) {
        input = input >> 1;
    }
    }
    else {
        for(i = 0; i > shift; i--) {
 b38:	2900      	cmp	r1, #0
 b3a:	d1f9      	bne.n	b30 <write_to_proc_cache+0x8>

void write_to_proc_cache(uint16_t data) {
    const uint16_t len = 11;
    data &= (right_shift(1, -len) - 1);

    if(len >= proc_cache_remainder) {
 b3c:	4c11      	ldr	r4, [pc, #68]	; (b84 <write_to_proc_cache+0x5c>)
 * Sampling functions
 **********************************************/

void write_to_proc_cache(uint16_t data) {
    const uint16_t len = 11;
    data &= (right_shift(1, -len) - 1);
 b3e:	3a01      	subs	r2, #1

    if(len >= proc_cache_remainder) {
 b40:	8823      	ldrh	r3, [r4, #0]
 * Sampling functions
 **********************************************/

void write_to_proc_cache(uint16_t data) {
    const uint16_t len = 11;
    data &= (right_shift(1, -len) - 1);
 b42:	4002      	ands	r2, r0
 b44:	0015      	movs	r5, r2
 b46:	4f10      	ldr	r7, [pc, #64]	; (b88 <write_to_proc_cache+0x60>)

    if(len >= proc_cache_remainder) {
 b48:	2b0b      	cmp	r3, #11
 b4a:	d80e      	bhi.n	b6a <write_to_proc_cache+0x42>
        // if no more cache space, store and switch to new cache
        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) cache_addr, proc_cache, PROC_CACHE_LEN - 1);
 b4c:	4e0f      	ldr	r6, [pc, #60]	; (b8c <write_to_proc_cache+0x64>)
 b4e:	2309      	movs	r3, #9
 b50:	8831      	ldrh	r1, [r6, #0]
 b52:	003a      	movs	r2, r7
 b54:	b289      	uxth	r1, r1
 b56:	2006      	movs	r0, #6
 b58:	f7ff faf2 	bl	140 <mbus_copy_mem_from_local_to_remote_bulk>
        cache_addr += (PROC_CACHE_LEN << 2);
 b5c:	8833      	ldrh	r3, [r6, #0]
 b5e:	3328      	adds	r3, #40	; 0x28
 b60:	b29b      	uxth	r3, r3
 b62:	8033      	strh	r3, [r6, #0]
        proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
 b64:	23a0      	movs	r3, #160	; 0xa0
 b66:	005b      	lsls	r3, r3, #1
 b68:	8023      	strh	r3, [r4, #0]
 b6a:	230b      	movs	r3, #11
 b6c:	0029      	movs	r1, r5
 b6e:	0038      	movs	r0, r7
 b70:	425b      	negs	r3, r3
 b72:	220a      	movs	r2, #10
 b74:	f7ff fdc8 	bl	708 <right_shift_arr.part.0>
    }

    right_shift_arr(proc_cache, data, PROC_CACHE_LEN, -len);
    proc_cache_remainder -= len;
 b78:	8823      	ldrh	r3, [r4, #0]
 b7a:	3b0b      	subs	r3, #11
 b7c:	b29b      	uxth	r3, r3
 b7e:	8023      	strh	r3, [r4, #0]
}
 b80:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 b82:	46c0      	nop			; (mov r8, r8)
 b84:	0000382c 	.word	0x0000382c
 b88:	000039bc 	.word	0x000039bc
 b8c:	00003890 	.word	0x00003890

Disassembly of section .text.read_next_from_proc_cache:

00000b90 <read_next_from_proc_cache>:

uint16_t read_next_from_proc_cache() {
    uint16_t res = 0;

    if(day_state == DUSK) {
 b90:	4b2d      	ldr	r3, [pc, #180]	; (c48 <read_next_from_proc_cache+0xb8>)

    right_shift_arr(proc_cache, data, PROC_CACHE_LEN, -len);
    proc_cache_remainder -= len;
}

uint16_t read_next_from_proc_cache() {
 b92:	b573      	push	{r0, r1, r4, r5, r6, lr}
    uint16_t res = 0;

    if(day_state == DUSK) {
 b94:	881b      	ldrh	r3, [r3, #0]
 b96:	4d2d      	ldr	r5, [pc, #180]	; (c4c <read_next_from_proc_cache+0xbc>)
 b98:	4e2d      	ldr	r6, [pc, #180]	; (c50 <read_next_from_proc_cache+0xc0>)
 b9a:	2b02      	cmp	r3, #2
 b9c:	d122      	bne.n	be4 <read_next_from_proc_cache+0x54>
        // start reading from the bottom
        if(proc_cache_remainder < 11) {
 b9e:	882b      	ldrh	r3, [r5, #0]
 ba0:	2b0a      	cmp	r3, #10
 ba2:	d814      	bhi.n	bce <read_next_from_proc_cache+0x3e>
            // decrement address before reading because we start on an incorrect address
            cache_addr -= PROC_CACHE_LEN << 2;
 ba4:	4c2b      	ldr	r4, [pc, #172]	; (c54 <read_next_from_proc_cache+0xc4>)
 ba6:	8823      	ldrh	r3, [r4, #0]
 ba8:	3b28      	subs	r3, #40	; 0x28
 baa:	b29b      	uxth	r3, r3
 bac:	8023      	strh	r3, [r4, #0]
            set_halt_until_mbus_trx();
 bae:	f7ff fb5b 	bl	268 <set_halt_until_mbus_trx>
            mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) cache_addr, PRE_ADDR, proc_cache, PROC_CACHE_LEN - 1);
 bb2:	2309      	movs	r3, #9
 bb4:	8821      	ldrh	r1, [r4, #0]
 bb6:	2201      	movs	r2, #1
 bb8:	b289      	uxth	r1, r1
 bba:	9300      	str	r3, [sp, #0]
 bbc:	2006      	movs	r0, #6
 bbe:	0033      	movs	r3, r6
 bc0:	f7ff fad8 	bl	174 <mbus_copy_mem_from_remote_to_any_bulk>
            set_halt_until_mbus_tx();
 bc4:	f7ff fb4a 	bl	25c <set_halt_until_mbus_tx>
            proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
 bc8:	23a0      	movs	r3, #160	; 0xa0
 bca:	005b      	lsls	r3, r3, #1
 bcc:	802b      	strh	r3, [r5, #0]
        }
        res = proc_cache[PROC_CACHE_LEN - 1] & 0x7FF;
 bce:	6a74      	ldr	r4, [r6, #36]	; 0x24
        proc_cache_remainder -= 11;
 bd0:	882b      	ldrh	r3, [r5, #0]
            set_halt_until_mbus_trx();
            mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) cache_addr, PRE_ADDR, proc_cache, PROC_CACHE_LEN - 1);
            set_halt_until_mbus_tx();
            proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
        }
        res = proc_cache[PROC_CACHE_LEN - 1] & 0x7FF;
 bd2:	0564      	lsls	r4, r4, #21
        proc_cache_remainder -= 11;
 bd4:	3b0b      	subs	r3, #11
 bd6:	b29b      	uxth	r3, r3
 bd8:	802b      	strh	r3, [r5, #0]
            set_halt_until_mbus_trx();
            mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) cache_addr, PRE_ADDR, proc_cache, PROC_CACHE_LEN - 1);
            set_halt_until_mbus_tx();
            proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
        }
        res = proc_cache[PROC_CACHE_LEN - 1] & 0x7FF;
 bda:	0d64      	lsrs	r4, r4, #21
 bdc:	230b      	movs	r3, #11
 bde:	220a      	movs	r2, #10
 be0:	2100      	movs	r1, #0
 be2:	e02b      	b.n	c3c <read_next_from_proc_cache+0xac>
        proc_cache_remainder -= 11;
        right_shift_arr(proc_cache, 0, PROC_CACHE_LEN, 11);
    }
    else {
        if(proc_cache_remainder < 11) {
 be4:	882b      	ldrh	r3, [r5, #0]
 be6:	2b0a      	cmp	r3, #10
 be8:	d814      	bhi.n	c14 <read_next_from_proc_cache+0x84>
            set_halt_until_mbus_trx();
 bea:	f7ff fb3d 	bl	268 <set_halt_until_mbus_trx>
            mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) cache_addr, PRE_ADDR, proc_cache, PROC_CACHE_LEN - 1);
 bee:	2309      	movs	r3, #9
 bf0:	4c18      	ldr	r4, [pc, #96]	; (c54 <read_next_from_proc_cache+0xc4>)
 bf2:	2201      	movs	r2, #1
 bf4:	8821      	ldrh	r1, [r4, #0]
 bf6:	2006      	movs	r0, #6
 bf8:	b289      	uxth	r1, r1
 bfa:	9300      	str	r3, [sp, #0]
 bfc:	0033      	movs	r3, r6
 bfe:	f7ff fab9 	bl	174 <mbus_copy_mem_from_remote_to_any_bulk>
            set_halt_until_mbus_tx();
 c02:	f7ff fb2b 	bl	25c <set_halt_until_mbus_tx>
            // increment address after we read from memory because we start on the correct addr
            cache_addr += PROC_CACHE_LEN << 2;
 c06:	8823      	ldrh	r3, [r4, #0]
 c08:	3328      	adds	r3, #40	; 0x28
 c0a:	b29b      	uxth	r3, r3
 c0c:	8023      	strh	r3, [r4, #0]
            proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
 c0e:	23a0      	movs	r3, #160	; 0xa0
 c10:	005b      	lsls	r3, r3, #1
 c12:	802b      	strh	r3, [r5, #0]
        }

        res = right_shift(proc_cache[0], 20) & 0x7FF;
 c14:	2300      	movs	r3, #0
 c16:	2114      	movs	r1, #20
 c18:	6830      	ldr	r0, [r6, #0]
 c1a:	3901      	subs	r1, #1

uint64_t right_shift(uint64_t input, int8_t shift) {
    int8_t i;
    if(shift >= 0) {
        for(i = 0; i < shift; i++) {
        input = input >> 1;
 c1c:	07da      	lsls	r2, r3, #31
 c1e:	0840      	lsrs	r0, r0, #1
 c20:	b2c9      	uxtb	r1, r1
 c22:	4310      	orrs	r0, r2
 c24:	085b      	lsrs	r3, r3, #1
#define LOG2_RES 5

uint64_t right_shift(uint64_t input, int8_t shift) {
    int8_t i;
    if(shift >= 0) {
        for(i = 0; i < shift; i++) {
 c26:	2900      	cmp	r1, #0
 c28:	d1f7      	bne.n	c1a <read_next_from_proc_cache+0x8a>
            cache_addr += PROC_CACHE_LEN << 2;
            proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
        }

        res = right_shift(proc_cache[0], 20) & 0x7FF;
        proc_cache_remainder -= 11;
 c2a:	882b      	ldrh	r3, [r5, #0]
 c2c:	220a      	movs	r2, #10
 c2e:	3b0b      	subs	r3, #11
 c30:	b29b      	uxth	r3, r3
 c32:	802b      	strh	r3, [r5, #0]
 c34:	230b      	movs	r3, #11
            // increment address after we read from memory because we start on the correct addr
            cache_addr += PROC_CACHE_LEN << 2;
            proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
        }

        res = right_shift(proc_cache[0], 20) & 0x7FF;
 c36:	0540      	lsls	r0, r0, #21
 c38:	0d44      	lsrs	r4, r0, #21
 c3a:	425b      	negs	r3, r3
 c3c:	0030      	movs	r0, r6
 c3e:	f7ff fd63 	bl	708 <right_shift_arr.part.0>
        proc_cache_remainder -= 11;

        right_shift_arr(proc_cache, 0, PROC_CACHE_LEN, -11);
    }
    return res;
}
 c42:	0020      	movs	r0, r4
 c44:	bd76      	pop	{r1, r2, r4, r5, r6, pc}
 c46:	46c0      	nop			; (mov r8, r8)
 c48:	00003930 	.word	0x00003930
 c4c:	0000382c 	.word	0x0000382c
 c50:	000039bc 	.word	0x000039bc
 c54:	00003890 	.word	0x00003890

Disassembly of section .text.check_new_edge:

00000c58 <check_new_edge>:

// These invariants are used to prevent pathological corner cases from crashing the system
bool check_new_edge(uint32_t target) {
    if(day_state == DAWN) {
 c58:	4b12      	ldr	r3, [pc, #72]	; (ca4 <check_new_edge+0x4c>)
 c5a:	881a      	ldrh	r2, [r3, #0]
 c5c:	b292      	uxth	r2, r2
 c5e:	2a00      	cmp	r2, #0
 c60:	d10f      	bne.n	c82 <check_new_edge+0x2a>
        return (target > EDGE_MARGIN2 + XO_32_MIN) && (target < MID_DAY_TIME - EDGE_MARGIN1 - XO_32_MIN);
 c62:	21f0      	movs	r1, #240	; 0xf0
 c64:	4b10      	ldr	r3, [pc, #64]	; (ca8 <check_new_edge+0x50>)
 c66:	00c9      	lsls	r1, r1, #3
 c68:	881b      	ldrh	r3, [r3, #0]
 c6a:	185b      	adds	r3, r3, r1
 c6c:	4298      	cmp	r0, r3
 c6e:	d906      	bls.n	c7e <check_new_edge+0x26>
 c70:	4b0e      	ldr	r3, [pc, #56]	; (cac <check_new_edge+0x54>)
 c72:	4a0f      	ldr	r2, [pc, #60]	; (cb0 <check_new_edge+0x58>)
 c74:	881b      	ldrh	r3, [r3, #0]
 c76:	1ad2      	subs	r2, r2, r3
 c78:	4290      	cmp	r0, r2
 c7a:	4180      	sbcs	r0, r0
 c7c:	4242      	negs	r2, r0
 c7e:	0010      	movs	r0, r2
 c80:	e00f      	b.n	ca2 <check_new_edge+0x4a>
    }
    else {
        return (target > MID_DAY_TIME + EDGE_MARGIN1 + XO_32_MIN) && (target < MAX_DAY_TIME - EDGE_MARGIN2 - XO_32_MIN);
 c82:	4b0a      	ldr	r3, [pc, #40]	; (cac <check_new_edge+0x54>)
 c84:	881a      	ldrh	r2, [r3, #0]
 c86:	4b0b      	ldr	r3, [pc, #44]	; (cb4 <check_new_edge+0x5c>)
 c88:	18d2      	adds	r2, r2, r3
 c8a:	2300      	movs	r3, #0
 c8c:	4290      	cmp	r0, r2
 c8e:	d907      	bls.n	ca0 <check_new_edge+0x48>
 c90:	4b05      	ldr	r3, [pc, #20]	; (ca8 <check_new_edge+0x50>)
 c92:	881a      	ldrh	r2, [r3, #0]
 c94:	23a5      	movs	r3, #165	; 0xa5
 c96:	025b      	lsls	r3, r3, #9
 c98:	1a9b      	subs	r3, r3, r2
 c9a:	4298      	cmp	r0, r3
 c9c:	4180      	sbcs	r0, r0
 c9e:	4243      	negs	r3, r0
 ca0:	0018      	movs	r0, r3
    }
}
 ca2:	4770      	bx	lr
 ca4:	00003930 	.word	0x00003930
 ca8:	00003956 	.word	0x00003956
 cac:	00003954 	.word	0x00003954
 cb0:	0000a140 	.word	0x0000a140
 cb4:	0000b040 	.word	0x0000b040

Disassembly of section .text.set_new_state:

00000cb8 <set_new_state>:


void set_new_state() {
 cb8:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    // reset new state variables
    cache_addr = CACHE_START_ADDR;
 cba:	4a3f      	ldr	r2, [pc, #252]	; (db8 <set_new_state+0x100>)
 cbc:	4b3f      	ldr	r3, [pc, #252]	; (dbc <set_new_state+0x104>)
 cbe:	4e40      	ldr	r6, [pc, #256]	; (dc0 <set_new_state+0x108>)
 cc0:	801a      	strh	r2, [r3, #0]
    proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
 cc2:	22a0      	movs	r2, #160	; 0xa0
 cc4:	4b3f      	ldr	r3, [pc, #252]	; (dc4 <set_new_state+0x10c>)
 cc6:	0052      	lsls	r2, r2, #1
 cc8:	801a      	strh	r2, [r3, #0]
    max_idx = 0;
 cca:	2200      	movs	r2, #0
 ccc:	4b3e      	ldr	r3, [pc, #248]	; (dc8 <set_new_state+0x110>)
 cce:	4c3f      	ldr	r4, [pc, #252]	; (dcc <set_new_state+0x114>)
 cd0:	801a      	strh	r2, [r3, #0]
    threshold_idx = IDX_INIT;
 cd2:	23ff      	movs	r3, #255	; 0xff
 cd4:	4a3e      	ldr	r2, [pc, #248]	; (dd0 <set_new_state+0x118>)
 cd6:	4d3f      	ldr	r5, [pc, #252]	; (dd4 <set_new_state+0x11c>)
 cd8:	8013      	strh	r3, [r2, #0]
    min_light_idx = IDX_INIT;
 cda:	4a3f      	ldr	r2, [pc, #252]	; (dd8 <set_new_state+0x120>)
 cdc:	4f3f      	ldr	r7, [pc, #252]	; (ddc <set_new_state+0x124>)
 cde:	8013      	strh	r3, [r2, #0]
    sum = MAX_UINT16;
 ce0:	2201      	movs	r2, #1
 ce2:	4b3f      	ldr	r3, [pc, #252]	; (de0 <set_new_state+0x128>)
 ce4:	4252      	negs	r2, r2
 ce6:	801a      	strh	r2, [r3, #0]

    // set day_state specific variables
    if(day_state == DAWN) {
 ce8:	4b3e      	ldr	r3, [pc, #248]	; (de4 <set_new_state+0x12c>)
 cea:	881b      	ldrh	r3, [r3, #0]
 cec:	2b00      	cmp	r3, #0
 cee:	d109      	bne.n	d04 <set_new_state+0x4c>
        cur_edge = cur_sunrise;
 cf0:	4a3d      	ldr	r2, [pc, #244]	; (de8 <set_new_state+0x130>)
        day_state_start_time = cur_sunrise - EDGE_MARGIN2;
 cf2:	493e      	ldr	r1, [pc, #248]	; (dec <set_new_state+0x134>)
    min_light_idx = IDX_INIT;
    sum = MAX_UINT16;

    // set day_state specific variables
    if(day_state == DAWN) {
        cur_edge = cur_sunrise;
 cf4:	6813      	ldr	r3, [r2, #0]
 cf6:	6033      	str	r3, [r6, #0]
        day_state_start_time = cur_sunrise - EDGE_MARGIN2;
 cf8:	6813      	ldr	r3, [r2, #0]
 cfa:	8809      	ldrh	r1, [r1, #0]
 cfc:	1a5b      	subs	r3, r3, r1
 cfe:	6023      	str	r3, [r4, #0]
        day_state_end_time = cur_sunrise + EDGE_MARGIN1;
 d00:	4b3b      	ldr	r3, [pc, #236]	; (df0 <set_new_state+0x138>)
 d02:	e00c      	b.n	d1e <set_new_state+0x66>
    }
    else if(day_state == DUSK) {
 d04:	4b37      	ldr	r3, [pc, #220]	; (de4 <set_new_state+0x12c>)
 d06:	881b      	ldrh	r3, [r3, #0]
 d08:	2b02      	cmp	r3, #2
 d0a:	d10c      	bne.n	d26 <set_new_state+0x6e>
        cur_edge = cur_sunset;
 d0c:	4a39      	ldr	r2, [pc, #228]	; (df4 <set_new_state+0x13c>)
        day_state_start_time = cur_sunset - EDGE_MARGIN1;
 d0e:	4938      	ldr	r1, [pc, #224]	; (df0 <set_new_state+0x138>)
        cur_edge = cur_sunrise;
        day_state_start_time = cur_sunrise - EDGE_MARGIN2;
        day_state_end_time = cur_sunrise + EDGE_MARGIN1;
    }
    else if(day_state == DUSK) {
        cur_edge = cur_sunset;
 d10:	6813      	ldr	r3, [r2, #0]
 d12:	6033      	str	r3, [r6, #0]
        day_state_start_time = cur_sunset - EDGE_MARGIN1;
 d14:	6813      	ldr	r3, [r2, #0]
 d16:	8809      	ldrh	r1, [r1, #0]
 d18:	1a5b      	subs	r3, r3, r1
 d1a:	6023      	str	r3, [r4, #0]
        day_state_end_time = cur_sunset + EDGE_MARGIN2;
 d1c:	4b33      	ldr	r3, [pc, #204]	; (dec <set_new_state+0x134>)
 d1e:	8819      	ldrh	r1, [r3, #0]
 d20:	6813      	ldr	r3, [r2, #0]
 d22:	18cb      	adds	r3, r1, r3
 d24:	e029      	b.n	d7a <set_new_state+0xc2>
    }
    else {
        // V5.2.1: align everything to the minute
        // day_state == NOON or NIGHT
        uint32_t day_time_in_min = divide_by_60(xo_day_time_in_sec);
 d26:	6838      	ldr	r0, [r7, #0]
 d28:	f7ff fc84 	bl	634 <divide_by_60>
        // TODO: check if this needs a bounds check
        cur_edge = day_time_in_min * 60 + XO_32_MIN;
 d2c:	233c      	movs	r3, #60	; 0x3c
 d2e:	4358      	muls	r0, r3
 d30:	23f0      	movs	r3, #240	; 0xf0
 d32:	00db      	lsls	r3, r3, #3
 d34:	18c0      	adds	r0, r0, r3
 d36:	6030      	str	r0, [r6, #0]
        day_state_start_time = cur_edge;
 d38:	6833      	ldr	r3, [r6, #0]
 d3a:	6023      	str	r3, [r4, #0]
        if(day_state == NOON) {
 d3c:	4b29      	ldr	r3, [pc, #164]	; (de4 <set_new_state+0x12c>)
 d3e:	881b      	ldrh	r3, [r3, #0]
 d40:	2b01      	cmp	r3, #1
 d42:	d10b      	bne.n	d5c <set_new_state+0xa4>
            // v5.3.1: setting new sunset at the start of NOON
            cur_sunset = next_sunset == 0? cur_sunset : next_sunset;
 d44:	4a2c      	ldr	r2, [pc, #176]	; (df8 <set_new_state+0x140>)
 d46:	4b2b      	ldr	r3, [pc, #172]	; (df4 <set_new_state+0x13c>)
 d48:	6811      	ldr	r1, [r2, #0]
 d4a:	2900      	cmp	r1, #0
 d4c:	d101      	bne.n	d52 <set_new_state+0x9a>
 d4e:	681a      	ldr	r2, [r3, #0]
 d50:	e000      	b.n	d54 <set_new_state+0x9c>
 d52:	6812      	ldr	r2, [r2, #0]
 d54:	601a      	str	r2, [r3, #0]
            day_state_end_time = cur_sunset - EDGE_MARGIN1 - XO_10_MIN;
 d56:	681b      	ldr	r3, [r3, #0]
 d58:	4a25      	ldr	r2, [pc, #148]	; (df0 <set_new_state+0x138>)
 d5a:	e00a      	b.n	d72 <set_new_state+0xba>
        }
        else {
            // v5.3.1: setting new sunrise at the start of NIGHT
            cur_sunrise = next_sunrise == 0? cur_sunrise : next_sunrise;
 d5c:	4a27      	ldr	r2, [pc, #156]	; (dfc <set_new_state+0x144>)
 d5e:	4b22      	ldr	r3, [pc, #136]	; (de8 <set_new_state+0x130>)
 d60:	6811      	ldr	r1, [r2, #0]
 d62:	2900      	cmp	r1, #0
 d64:	d101      	bne.n	d6a <set_new_state+0xb2>
 d66:	681a      	ldr	r2, [r3, #0]
 d68:	e000      	b.n	d6c <set_new_state+0xb4>
 d6a:	6812      	ldr	r2, [r2, #0]
 d6c:	601a      	str	r2, [r3, #0]
            day_state_end_time = cur_sunrise - EDGE_MARGIN2 - XO_10_MIN;
 d6e:	681b      	ldr	r3, [r3, #0]
 d70:	4a1e      	ldr	r2, [pc, #120]	; (dec <set_new_state+0x134>)
 d72:	4923      	ldr	r1, [pc, #140]	; (e00 <set_new_state+0x148>)
 d74:	8812      	ldrh	r2, [r2, #0]
 d76:	185b      	adds	r3, r3, r1
 d78:	1a9b      	subs	r3, r3, r2
 d7a:	602b      	str	r3, [r5, #0]
        }
    }

    // set new light measure time
    next_light_meas_time = day_state_start_time + xo_sys_time_in_sec - xo_day_time_in_sec;
 d7c:	4b21      	ldr	r3, [pc, #132]	; (e04 <set_new_state+0x14c>)
 d7e:	6821      	ldr	r1, [r4, #0]
 d80:	681b      	ldr	r3, [r3, #0]
 d82:	683a      	ldr	r2, [r7, #0]
 d84:	18cb      	adds	r3, r1, r3
 d86:	1a9b      	subs	r3, r3, r2
 d88:	4a1f      	ldr	r2, [pc, #124]	; (e08 <set_new_state+0x150>)
 d8a:	6013      	str	r3, [r2, #0]

    // if wrapping around. TODO: test this
    if(day_state_start_time < xo_day_time_in_sec) {
 d8c:	6821      	ldr	r1, [r4, #0]
 d8e:	683b      	ldr	r3, [r7, #0]
 d90:	4299      	cmp	r1, r3
 d92:	d203      	bcs.n	d9c <set_new_state+0xe4>
        next_light_meas_time += MAX_DAY_TIME;
 d94:	6813      	ldr	r3, [r2, #0]
 d96:	491d      	ldr	r1, [pc, #116]	; (e0c <set_new_state+0x154>)
 d98:	185b      	adds	r3, r3, r1
 d9a:	6013      	str	r3, [r2, #0]
    }

    // save first time in day_state
    light_meas_start_time_in_min = divide_by_60(next_light_meas_time - sys_sec_to_min_offset);
 d9c:	4b1c      	ldr	r3, [pc, #112]	; (e10 <set_new_state+0x158>)
 d9e:	6810      	ldr	r0, [r2, #0]
 da0:	681b      	ldr	r3, [r3, #0]
 da2:	1ac0      	subs	r0, r0, r3
 da4:	f7ff fc46 	bl	634 <divide_by_60>
 da8:	4b1a      	ldr	r3, [pc, #104]	; (e14 <set_new_state+0x15c>)
 daa:	6018      	str	r0, [r3, #0]
    mbus_write_message32(0xB8, light_meas_start_time_in_min);
 dac:	6819      	ldr	r1, [r3, #0]
 dae:	20b8      	movs	r0, #184	; 0xb8
 db0:	f7ff f97a 	bl	a8 <mbus_write_message32>
}
 db4:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 db6:	46c0      	nop			; (mov r8, r8)
 db8:	00003e98 	.word	0x00003e98
 dbc:	00003890 	.word	0x00003890
 dc0:	00003aac 	.word	0x00003aac
 dc4:	0000382c 	.word	0x0000382c
 dc8:	00003b00 	.word	0x00003b00
 dcc:	0000398c 	.word	0x0000398c
 dd0:	0000382a 	.word	0x0000382a
 dd4:	00003a18 	.word	0x00003a18
 dd8:	00003832 	.word	0x00003832
 ddc:	00003a78 	.word	0x00003a78
 de0:	0000390a 	.word	0x0000390a
 de4:	00003930 	.word	0x00003930
 de8:	00003ac4 	.word	0x00003ac4
 dec:	00003956 	.word	0x00003956
 df0:	00003954 	.word	0x00003954
 df4:	00003b1c 	.word	0x00003b1c
 df8:	00003a44 	.word	0x00003a44
 dfc:	00003a30 	.word	0x00003a30
 e00:	fffffda8 	.word	0xfffffda8
 e04:	00003acc 	.word	0x00003acc
 e08:	00003a50 	.word	0x00003a50
 e0c:	00015180 	.word	0x00015180
 e10:	00003a70 	.word	0x00003a70
 e14:	00003b08 	.word	0x00003b08

Disassembly of section .text.sample_light:

00000e18 <sample_light>:

void sample_light() {
     e18:	b5f0      	push	{r4, r5, r6, r7, lr}
    uint16_t log_light = log2(lnt_sys_light);
     e1a:	4ba9      	ldr	r3, [pc, #676]	; (10c0 <sample_light+0x2a8>)
    // save first time in day_state
    light_meas_start_time_in_min = divide_by_60(next_light_meas_time - sys_sec_to_min_offset);
    mbus_write_message32(0xB8, light_meas_start_time_in_min);
}

void sample_light() {
     e1c:	b085      	sub	sp, #20
    uint16_t log_light = log2(lnt_sys_light);
     e1e:	6818      	ldr	r0, [r3, #0]
     e20:	6859      	ldr	r1, [r3, #4]
     e22:	f7ff fd33 	bl	88c <log2>
    uint16_t last_avg_light = avg_light;
     e26:	4ea7      	ldr	r6, [pc, #668]	; (10c4 <sample_light+0x2ac>)
    light_meas_start_time_in_min = divide_by_60(next_light_meas_time - sys_sec_to_min_offset);
    mbus_write_message32(0xB8, light_meas_start_time_in_min);
}

void sample_light() {
    uint16_t log_light = log2(lnt_sys_light);
     e28:	9002      	str	r0, [sp, #8]
     e2a:	0004      	movs	r4, r0
    uint16_t last_avg_light = avg_light;
     e2c:	8835      	ldrh	r5, [r6, #0]
    write_to_proc_cache(log_light);
     e2e:	f7ff fe7b 	bl	b28 <write_to_proc_cache>

    // store to running sum
    if(sum == MAX_UINT16) {
     e32:	4ba5      	ldr	r3, [pc, #660]	; (10c8 <sample_light+0x2b0>)
     e34:	4aa5      	ldr	r2, [pc, #660]	; (10cc <sample_light+0x2b4>)
     e36:	8818      	ldrh	r0, [r3, #0]
    mbus_write_message32(0xB8, light_meas_start_time_in_min);
}

void sample_light() {
    uint16_t log_light = log2(lnt_sys_light);
    uint16_t last_avg_light = avg_light;
     e38:	b2ad      	uxth	r5, r5
     e3a:	49a5      	ldr	r1, [pc, #660]	; (10d0 <sample_light+0x2b8>)
    write_to_proc_cache(log_light);

    // store to running sum
    if(sum == MAX_UINT16) {
     e3c:	4290      	cmp	r0, r2
     e3e:	d110      	bne.n	e62 <sample_light+0x4a>
        // if uninitialized, initialize running avg
        uint16_t i;
        for(i = 0; i < 8; i++) {
            running_avg[i] = log_light;
            running_avg_time[i] = xo_day_time_in_sec;
     e40:	48a4      	ldr	r0, [pc, #656]	; (10d4 <sample_light+0x2bc>)
     e42:	2200      	movs	r2, #0
     e44:	4684      	mov	ip, r0
     e46:	4667      	mov	r7, ip
    // store to running sum
    if(sum == MAX_UINT16) {
        // if uninitialized, initialize running avg
        uint16_t i;
        for(i = 0; i < 8; i++) {
            running_avg[i] = log_light;
     e48:	0050      	lsls	r0, r2, #1
     e4a:	520c      	strh	r4, [r1, r0]
            running_avg_time[i] = xo_day_time_in_sec;
     e4c:	48a2      	ldr	r0, [pc, #648]	; (10d8 <sample_light+0x2c0>)
     e4e:	6805      	ldr	r5, [r0, #0]
     e50:	0090      	lsls	r0, r2, #2
     e52:	3201      	adds	r2, #1
     e54:	51c5      	str	r5, [r0, r7]

    // store to running sum
    if(sum == MAX_UINT16) {
        // if uninitialized, initialize running avg
        uint16_t i;
        for(i = 0; i < 8; i++) {
     e56:	2a08      	cmp	r2, #8
     e58:	d1f5      	bne.n	e46 <sample_light+0x2e>
            running_avg[i] = log_light;
            running_avg_time[i] = xo_day_time_in_sec;
        }
        sum = log_light << 3;
     e5a:	00e4      	lsls	r4, r4, #3
     e5c:	b2a4      	uxth	r4, r4
     e5e:	801c      	strh	r4, [r3, #0]
     e60:	e01d      	b.n	e9e <sample_light+0x86>
        // initialize last_avg_light to avoid crossing the threshold on first light meas
        last_avg_light = log_light;
    } 
    else {
        sum -= running_avg[rot_idx];
     e62:	4a9e      	ldr	r2, [pc, #632]	; (10dc <sample_light+0x2c4>)
     e64:	8810      	ldrh	r0, [r2, #0]
     e66:	0040      	lsls	r0, r0, #1
     e68:	5a0c      	ldrh	r4, [r1, r0]
     e6a:	8818      	ldrh	r0, [r3, #0]
     e6c:	1b00      	subs	r0, r0, r4
     e6e:	b280      	uxth	r0, r0
     e70:	8018      	strh	r0, [r3, #0]
        sum += log_light;
     e72:	8818      	ldrh	r0, [r3, #0]
     e74:	9c02      	ldr	r4, [sp, #8]
     e76:	1820      	adds	r0, r4, r0

        running_avg[rot_idx] = log_light;
     e78:	466c      	mov	r4, sp
        // initialize last_avg_light to avoid crossing the threshold on first light meas
        last_avg_light = log_light;
    } 
    else {
        sum -= running_avg[rot_idx];
        sum += log_light;
     e7a:	b280      	uxth	r0, r0
     e7c:	8018      	strh	r0, [r3, #0]

        running_avg[rot_idx] = log_light;
     e7e:	8810      	ldrh	r0, [r2, #0]
     e80:	8924      	ldrh	r4, [r4, #8]
     e82:	0040      	lsls	r0, r0, #1
     e84:	520c      	strh	r4, [r1, r0]
        running_avg_time[rot_idx] = xo_day_time_in_sec;
     e86:	4894      	ldr	r0, [pc, #592]	; (10d8 <sample_light+0x2c0>)
     e88:	8811      	ldrh	r1, [r2, #0]
     e8a:	6804      	ldr	r4, [r0, #0]
     e8c:	4891      	ldr	r0, [pc, #580]	; (10d4 <sample_light+0x2bc>)
     e8e:	0089      	lsls	r1, r1, #2
     e90:	500c      	str	r4, [r1, r0]
        rot_idx = (rot_idx + 1) & 7;
     e92:	8811      	ldrh	r1, [r2, #0]
    mbus_write_message32(0xB8, light_meas_start_time_in_min);
}

void sample_light() {
    uint16_t log_light = log2(lnt_sys_light);
    uint16_t last_avg_light = avg_light;
     e94:	9502      	str	r5, [sp, #8]
        sum -= running_avg[rot_idx];
        sum += log_light;

        running_avg[rot_idx] = log_light;
        running_avg_time[rot_idx] = xo_day_time_in_sec;
        rot_idx = (rot_idx + 1) & 7;
     e96:	1c48      	adds	r0, r1, #1
     e98:	2107      	movs	r1, #7
     e9a:	4001      	ands	r1, r0
     e9c:	8011      	strh	r1, [r2, #0]
    }

    avg_light = sum >> 3;
     e9e:	881b      	ldrh	r3, [r3, #0]
     ea0:	4c8f      	ldr	r4, [pc, #572]	; (10e0 <sample_light+0x2c8>)
     ea2:	08db      	lsrs	r3, r3, #3
     ea4:	8033      	strh	r3, [r6, #0]
    
    // record min_light in day_state
    if(min_light_idx == IDX_INIT || avg_light < min_light
     ea6:	4b8f      	ldr	r3, [pc, #572]	; (10e4 <sample_light+0x2cc>)
     ea8:	881b      	ldrh	r3, [r3, #0]
     eaa:	2bff      	cmp	r3, #255	; 0xff
     eac:	d00c      	beq.n	ec8 <sample_light+0xb0>
     eae:	4b8e      	ldr	r3, [pc, #568]	; (10e8 <sample_light+0x2d0>)
     eb0:	8832      	ldrh	r2, [r6, #0]
     eb2:	881b      	ldrh	r3, [r3, #0]
     eb4:	429a      	cmp	r2, r3
     eb6:	d307      	bcc.n	ec8 <sample_light+0xb0>
            || (day_state == DAWN && avg_light == min_light)) { // find last min_light at dawn
     eb8:	8823      	ldrh	r3, [r4, #0]
     eba:	2b00      	cmp	r3, #0
     ebc:	d10d      	bne.n	eda <sample_light+0xc2>
     ebe:	4b8a      	ldr	r3, [pc, #552]	; (10e8 <sample_light+0x2d0>)
     ec0:	8832      	ldrh	r2, [r6, #0]
     ec2:	881b      	ldrh	r3, [r3, #0]
     ec4:	429a      	cmp	r2, r3
     ec6:	d108      	bne.n	eda <sample_light+0xc2>
        min_light = avg_light;
     ec8:	8833      	ldrh	r3, [r6, #0]
     eca:	4a87      	ldr	r2, [pc, #540]	; (10e8 <sample_light+0x2d0>)
     ecc:	b29b      	uxth	r3, r3
     ece:	8013      	strh	r3, [r2, #0]
        min_light_idx = max_idx;
     ed0:	4b86      	ldr	r3, [pc, #536]	; (10ec <sample_light+0x2d4>)
     ed2:	4a84      	ldr	r2, [pc, #528]	; (10e4 <sample_light+0x2cc>)
     ed4:	881b      	ldrh	r3, [r3, #0]
     ed6:	b29b      	uxth	r3, r3
     ed8:	8013      	strh	r3, [r2, #0]
    }
    
    // save the time before updating
    uint32_t light_meas_end_time_in_min = divide_by_60(next_light_meas_time - sys_sec_to_min_offset);
     eda:	4d85      	ldr	r5, [pc, #532]	; (10f0 <sample_light+0x2d8>)
     edc:	4b85      	ldr	r3, [pc, #532]	; (10f4 <sample_light+0x2dc>)
     ede:	6828      	ldr	r0, [r5, #0]
     ee0:	681b      	ldr	r3, [r3, #0]
     ee2:	1ac0      	subs	r0, r0, r3
     ee4:	f7ff fba6 	bl	634 <divide_by_60>
    
    // set next light measure time
    if(day_state == DAWN || day_state == DUSK) {
     ee8:	8823      	ldrh	r3, [r4, #0]
        min_light = avg_light;
        min_light_idx = max_idx;
    }
    
    // save the time before updating
    uint32_t light_meas_end_time_in_min = divide_by_60(next_light_meas_time - sys_sec_to_min_offset);
     eea:	9001      	str	r0, [sp, #4]
    
    // set next light measure time
    if(day_state == DAWN || day_state == DUSK) {
     eec:	2b00      	cmp	r3, #0
     eee:	d002      	beq.n	ef6 <sample_light+0xde>
     ef0:	8823      	ldrh	r3, [r4, #0]
     ef2:	2b02      	cmp	r3, #2
     ef4:	d102      	bne.n	efc <sample_light+0xe4>
        next_light_meas_time += XO_1_MIN;
     ef6:	682b      	ldr	r3, [r5, #0]
     ef8:	333c      	adds	r3, #60	; 0x3c
     efa:	e003      	b.n	f04 <sample_light+0xec>
    }
    else {
        next_light_meas_time += XO_32_MIN;
     efc:	22f0      	movs	r2, #240	; 0xf0
     efe:	682b      	ldr	r3, [r5, #0]
     f00:	00d2      	lsls	r2, r2, #3
     f02:	189b      	adds	r3, r3, r2
     f04:	602b      	str	r3, [r5, #0]
    }

    // check if advancing day_state to be used for shifting window
    bool new_state = false;
    uint32_t temp = xo_day_time_in_sec + next_light_meas_time - xo_sys_time_in_sec;
     f06:	4b74      	ldr	r3, [pc, #464]	; (10d8 <sample_light+0x2c0>)
     f08:	4a7b      	ldr	r2, [pc, #492]	; (10f8 <sample_light+0x2e0>)
     f0a:	6819      	ldr	r1, [r3, #0]
     f0c:	682b      	ldr	r3, [r5, #0]
     f0e:	6812      	ldr	r2, [r2, #0]
     f10:	18cb      	adds	r3, r1, r3
     f12:	1a9b      	subs	r3, r3, r2
    if(day_state != NIGHT) {
     f14:	8821      	ldrh	r1, [r4, #0]
     f16:	4a79      	ldr	r2, [pc, #484]	; (10fc <sample_light+0x2e4>)
     f18:	2700      	movs	r7, #0
        new_state = (temp >= day_state_end_time);
     f1a:	6812      	ldr	r2, [r2, #0]
    }

    // check if advancing day_state to be used for shifting window
    bool new_state = false;
    uint32_t temp = xo_day_time_in_sec + next_light_meas_time - xo_sys_time_in_sec;
    if(day_state != NIGHT) {
     f1c:	2903      	cmp	r1, #3
     f1e:	d003      	beq.n	f28 <sample_light+0x110>
        new_state = (temp >= day_state_end_time);
     f20:	4293      	cmp	r3, r2
     f22:	417f      	adcs	r7, r7
     f24:	b2ff      	uxtb	r7, r7
     f26:	e007      	b.n	f38 <sample_light+0x120>
    }
    else {
        new_state = (temp >= day_state_end_time && temp < MID_DAY_TIME);
     f28:	4293      	cmp	r3, r2
     f2a:	d303      	bcc.n	f34 <sample_light+0x11c>
     f2c:	4a74      	ldr	r2, [pc, #464]	; (1100 <sample_light+0x2e8>)
     f2e:	429a      	cmp	r2, r3
     f30:	417f      	adcs	r7, r7
     f32:	b2ff      	uxtb	r7, r7
     f34:	2301      	movs	r3, #1
     f36:	401f      	ands	r7, r3
    }


    uint32_t target = 0;
    // test if crosses threshold
    if(day_state == DAWN && threshold_idx == IDX_INIT) { 
     f38:	8823      	ldrh	r3, [r4, #0]
     f3a:	2b00      	cmp	r3, #0
     f3c:	d121      	bne.n	f82 <sample_light+0x16a>
     f3e:	4a71      	ldr	r2, [pc, #452]	; (1104 <sample_light+0x2ec>)
     f40:	8813      	ldrh	r3, [r2, #0]
     f42:	2bff      	cmp	r3, #255	; 0xff
     f44:	d11d      	bne.n	f82 <sample_light+0x16a>
        if(avg_light >= EDGE_THRESHOLD && last_avg_light < EDGE_THRESHOLD) {
     f46:	4b70      	ldr	r3, [pc, #448]	; (1108 <sample_light+0x2f0>)
     f48:	8830      	ldrh	r0, [r6, #0]
     f4a:	8819      	ldrh	r1, [r3, #0]
     f4c:	4288      	cmp	r0, r1
     f4e:	d307      	bcc.n	f60 <sample_light+0x148>
     f50:	8819      	ldrh	r1, [r3, #0]
     f52:	9802      	ldr	r0, [sp, #8]
     f54:	4288      	cmp	r0, r1
     f56:	d203      	bcs.n	f60 <sample_light+0x148>
            // FIXME: the system will think that it crossed the threshold on the first light meas
            threshold_idx = max_idx;
     f58:	4b64      	ldr	r3, [pc, #400]	; (10ec <sample_light+0x2d4>)
     f5a:	881b      	ldrh	r3, [r3, #0]
     f5c:	b29b      	uxth	r3, r3
     f5e:	e021      	b.n	fa4 <sample_light+0x18c>
    else {
        new_state = (temp >= day_state_end_time && temp < MID_DAY_TIME);
    }


    uint32_t target = 0;
     f60:	2500      	movs	r5, #0
        if(avg_light >= EDGE_THRESHOLD && last_avg_light < EDGE_THRESHOLD) {
            // FIXME: the system will think that it crossed the threshold on the first light meas
            threshold_idx = max_idx;
            target = running_avg_time[(rot_idx + 3) & 7];
        }
        else if(new_state) {
     f62:	42af      	cmp	r7, r5
     f64:	d03d      	beq.n	fe2 <sample_light+0x1ca>
            // if advancing to NOON but haven't set new window for the next sunrise yet, 
            // then it means that the system did not see the threshold
            if(min_light > EDGE_THRESHOLD) {
     f66:	4a60      	ldr	r2, [pc, #384]	; (10e8 <sample_light+0x2d0>)
     f68:	8810      	ldrh	r0, [r2, #0]
     f6a:	8819      	ldrh	r1, [r3, #0]
     f6c:	4a67      	ldr	r2, [pc, #412]	; (110c <sample_light+0x2f4>)
     f6e:	4b68      	ldr	r3, [pc, #416]	; (1110 <sample_light+0x2f8>)
     f70:	4288      	cmp	r0, r1
     f72:	d902      	bls.n	f7a <sample_light+0x162>
                // minimum light seen in this window is greater than threshold
                // move towards earlier in the day
                target = cur_sunrise - PASSIVE_WINDOW_SHIFT;
     f74:	681d      	ldr	r5, [r3, #0]
     f76:	8813      	ldrh	r3, [r2, #0]
     f78:	e032      	b.n	fe0 <sample_light+0x1c8>
            }
            else {
                // move towards later in the day
                target = cur_sunrise + PASSIVE_WINDOW_SHIFT;
     f7a:	8812      	ldrh	r2, [r2, #0]
     f7c:	681d      	ldr	r5, [r3, #0]
     f7e:	1955      	adds	r5, r2, r5
     f80:	e02f      	b.n	fe2 <sample_light+0x1ca>
            }
        
        }
    }
    else if(day_state == DUSK) { // && threshold_idx == IDX_INIT) { // For dusk, we want the last time the threshold is crossed, no need to check if there is already a threshold crossing
     f82:	8823      	ldrh	r3, [r4, #0]
    else {
        new_state = (temp >= day_state_end_time && temp < MID_DAY_TIME);
    }


    uint32_t target = 0;
     f84:	2500      	movs	r5, #0
                target = cur_sunrise + PASSIVE_WINDOW_SHIFT;
            }
        
        }
    }
    else if(day_state == DUSK) { // && threshold_idx == IDX_INIT) { // For dusk, we want the last time the threshold is crossed, no need to check if there is already a threshold crossing
     f86:	2b02      	cmp	r3, #2
     f88:	d12b      	bne.n	fe2 <sample_light+0x1ca>
        if(avg_light <= EDGE_THRESHOLD && last_avg_light > EDGE_THRESHOLD) {
     f8a:	4b5f      	ldr	r3, [pc, #380]	; (1108 <sample_light+0x2f0>)
     f8c:	8831      	ldrh	r1, [r6, #0]
     f8e:	881a      	ldrh	r2, [r3, #0]
     f90:	4291      	cmp	r1, r2
     f92:	d811      	bhi.n	fb8 <sample_light+0x1a0>
     f94:	881a      	ldrh	r2, [r3, #0]
     f96:	9902      	ldr	r1, [sp, #8]
     f98:	4291      	cmp	r1, r2
     f9a:	d90d      	bls.n	fb8 <sample_light+0x1a0>
            threshold_idx = max_idx;
     f9c:	4b53      	ldr	r3, [pc, #332]	; (10ec <sample_light+0x2d4>)
     f9e:	4a59      	ldr	r2, [pc, #356]	; (1104 <sample_light+0x2ec>)
     fa0:	881b      	ldrh	r3, [r3, #0]
     fa2:	b29b      	uxth	r3, r3
     fa4:	8013      	strh	r3, [r2, #0]
            target = running_avg_time[(rot_idx + 3) & 7];
     fa6:	4b4d      	ldr	r3, [pc, #308]	; (10dc <sample_light+0x2c4>)
     fa8:	881b      	ldrh	r3, [r3, #0]
     faa:	1cda      	adds	r2, r3, #3
     fac:	2307      	movs	r3, #7
     fae:	4013      	ands	r3, r2
     fb0:	4a48      	ldr	r2, [pc, #288]	; (10d4 <sample_light+0x2bc>)
     fb2:	009b      	lsls	r3, r3, #2
     fb4:	589d      	ldr	r5, [r3, r2]
     fb6:	e014      	b.n	fe2 <sample_light+0x1ca>
    else {
        new_state = (temp >= day_state_end_time && temp < MID_DAY_TIME);
    }


    uint32_t target = 0;
     fb8:	2500      	movs	r5, #0
    else if(day_state == DUSK) { // && threshold_idx == IDX_INIT) { // For dusk, we want the last time the threshold is crossed, no need to check if there is already a threshold crossing
        if(avg_light <= EDGE_THRESHOLD && last_avg_light > EDGE_THRESHOLD) {
            threshold_idx = max_idx;
            target = running_avg_time[(rot_idx + 3) & 7];
        }
        else if(new_state && threshold_idx == IDX_INIT) {
     fba:	42af      	cmp	r7, r5
     fbc:	d011      	beq.n	fe2 <sample_light+0x1ca>
     fbe:	4a51      	ldr	r2, [pc, #324]	; (1104 <sample_light+0x2ec>)
     fc0:	8812      	ldrh	r2, [r2, #0]
     fc2:	2aff      	cmp	r2, #255	; 0xff
     fc4:	d10d      	bne.n	fe2 <sample_light+0x1ca>
            // if advancing to NIGHT day_state but haven't set new window yet, shift towards later in the day
            if(min_light > EDGE_THRESHOLD) {
     fc6:	4a48      	ldr	r2, [pc, #288]	; (10e8 <sample_light+0x2d0>)
     fc8:	8810      	ldrh	r0, [r2, #0]
     fca:	8819      	ldrh	r1, [r3, #0]
     fcc:	4a51      	ldr	r2, [pc, #324]	; (1114 <sample_light+0x2fc>)
     fce:	4b4f      	ldr	r3, [pc, #316]	; (110c <sample_light+0x2f4>)
     fd0:	4288      	cmp	r0, r1
     fd2:	d903      	bls.n	fdc <sample_light+0x1c4>
                // minimum light seen in this window is greater than threshold
                // in the afternoon, move towards later in the day
                target = cur_sunset + PASSIVE_WINDOW_SHIFT;
     fd4:	881b      	ldrh	r3, [r3, #0]
     fd6:	6815      	ldr	r5, [r2, #0]
     fd8:	195d      	adds	r5, r3, r5
     fda:	e002      	b.n	fe2 <sample_light+0x1ca>
            }
            else {
                // move towards earlier in the day
                target = cur_sunset - PASSIVE_WINDOW_SHIFT;
     fdc:	6815      	ldr	r5, [r2, #0]
     fde:	881b      	ldrh	r3, [r3, #0]
     fe0:	1aed      	subs	r5, r5, r3
            }
        }
    }

    // set new edge time after verifying target validity
    if(check_new_edge(target)) {
     fe2:	0028      	movs	r0, r5
     fe4:	f7ff fe38 	bl	c58 <check_new_edge>
     fe8:	2800      	cmp	r0, #0
     fea:	d031      	beq.n	1050 <sample_light+0x238>
        if(target > cur_edge + MAX_EDGE_SHIFT) {
     fec:	4b4a      	ldr	r3, [pc, #296]	; (1118 <sample_light+0x300>)
     fee:	4a4b      	ldr	r2, [pc, #300]	; (111c <sample_light+0x304>)
     ff0:	8818      	ldrh	r0, [r3, #0]
     ff2:	6811      	ldr	r1, [r2, #0]
     ff4:	1841      	adds	r1, r0, r1
     ff6:	428d      	cmp	r5, r1
     ff8:	d903      	bls.n	1002 <sample_light+0x1ea>
            target = cur_edge + MAX_EDGE_SHIFT;
     ffa:	881b      	ldrh	r3, [r3, #0]
     ffc:	6815      	ldr	r5, [r2, #0]
     ffe:	195d      	adds	r5, r3, r5
    1000:	e007      	b.n	1012 <sample_light+0x1fa>
        }
        else if(target < cur_edge - MAX_EDGE_SHIFT) {
    1002:	6811      	ldr	r1, [r2, #0]
    1004:	8818      	ldrh	r0, [r3, #0]
    1006:	1a09      	subs	r1, r1, r0
    1008:	428d      	cmp	r5, r1
    100a:	d202      	bcs.n	1012 <sample_light+0x1fa>
            target = cur_edge - MAX_EDGE_SHIFT;
    100c:	6815      	ldr	r5, [r2, #0]
    100e:	881b      	ldrh	r3, [r3, #0]
    1010:	1aed      	subs	r5, r5, r3
        }

        // check edge invariants before setting new edge
        if(day_state == DAWN && target > 0 && target < MID_DAY_TIME 
    1012:	8823      	ldrh	r3, [r4, #0]
    1014:	2b00      	cmp	r3, #0
    1016:	d10b      	bne.n	1030 <sample_light+0x218>
    1018:	4a41      	ldr	r2, [pc, #260]	; (1120 <sample_light+0x308>)
    101a:	1e6b      	subs	r3, r5, #1
    101c:	4293      	cmp	r3, r2
    101e:	d807      	bhi.n	1030 <sample_light+0x218>
                && (cur_sunset - target > XO_270_MIN)) {
    1020:	4b3c      	ldr	r3, [pc, #240]	; (1114 <sample_light+0x2fc>)
    1022:	4a40      	ldr	r2, [pc, #256]	; (1124 <sample_light+0x30c>)
    1024:	681b      	ldr	r3, [r3, #0]
    1026:	1b5b      	subs	r3, r3, r5
    1028:	4293      	cmp	r3, r2
    102a:	d901      	bls.n	1030 <sample_light+0x218>
            next_sunrise = target;
    102c:	4b3e      	ldr	r3, [pc, #248]	; (1128 <sample_light+0x310>)
    102e:	e00e      	b.n	104e <sample_light+0x236>
        }
        else if(day_state == DUSK && target > MID_DAY_TIME && target < MAX_DAY_TIME 
    1030:	8823      	ldrh	r3, [r4, #0]
    1032:	2b02      	cmp	r3, #2
    1034:	d10c      	bne.n	1050 <sample_light+0x238>
    1036:	4b3d      	ldr	r3, [pc, #244]	; (112c <sample_light+0x314>)
    1038:	4a39      	ldr	r2, [pc, #228]	; (1120 <sample_light+0x308>)
    103a:	18eb      	adds	r3, r5, r3
    103c:	4293      	cmp	r3, r2
    103e:	d807      	bhi.n	1050 <sample_light+0x238>
                && (target - cur_sunrise > XO_270_MIN)) {
    1040:	4b33      	ldr	r3, [pc, #204]	; (1110 <sample_light+0x2f8>)
    1042:	4a38      	ldr	r2, [pc, #224]	; (1124 <sample_light+0x30c>)
    1044:	681b      	ldr	r3, [r3, #0]
    1046:	1aeb      	subs	r3, r5, r3
    1048:	4293      	cmp	r3, r2
    104a:	d901      	bls.n	1050 <sample_light+0x238>
            next_sunset = target;
    104c:	4b38      	ldr	r3, [pc, #224]	; (1130 <sample_light+0x318>)
    104e:	601d      	str	r5, [r3, #0]
        }
    }

    if(new_state) {
    1050:	2f00      	cmp	r7, #0
    1052:	d100      	bne.n	1056 <sample_light+0x23e>
    1054:	e0f0      	b.n	1238 <sample_light+0x420>
        // FIXME: turn it back on later
        *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    1056:	2500      	movs	r5, #0
    1058:	4b36      	ldr	r3, [pc, #216]	; (1134 <sample_light+0x31c>)
    105a:	601d      	str	r5, [r3, #0]
        *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    105c:	4b36      	ldr	r3, [pc, #216]	; (1138 <sample_light+0x320>)
    105e:	601d      	str	r5, [r3, #0]
            
        if(day_state != NIGHT) {
    1060:	8823      	ldrh	r3, [r4, #0]
    1062:	2b03      	cmp	r3, #3
    1064:	d100      	bne.n	1068 <sample_light+0x250>
    1066:	e0de      	b.n	1226 <sample_light+0x40e>
            // resample and store
            uint16_t starting_idx = 0;
            int16_t start, end, sign;
            uint32_t sample_time_in_min;

            if(day_state == DUSK) {
    1068:	8823      	ldrh	r3, [r4, #0]
    106a:	4e34      	ldr	r6, [pc, #208]	; (113c <sample_light+0x324>)
                // from last to first
                // manual set remainder to current data in proc_cache to start reading
                proc_cache_remainder = PROC_CACHE_MAX_REMAINDER - proc_cache_remainder;
    106c:	8832      	ldrh	r2, [r6, #0]
            // resample and store
            uint16_t starting_idx = 0;
            int16_t start, end, sign;
            uint32_t sample_time_in_min;

            if(day_state == DUSK) {
    106e:	2b02      	cmp	r3, #2
    1070:	d16a      	bne.n	1148 <sample_light+0x330>
                // from last to first
                // manual set remainder to current data in proc_cache to start reading
                proc_cache_remainder = PROC_CACHE_MAX_REMAINDER - proc_cache_remainder;
    1072:	333f      	adds	r3, #63	; 0x3f
    1074:	33ff      	adds	r3, #255	; 0xff
    1076:	1a9b      	subs	r3, r3, r2
    1078:	b29b      	uxth	r3, r3
                // if crossed threshold
                if(threshold_idx != IDX_INIT) {
    107a:	4a22      	ldr	r2, [pc, #136]	; (1104 <sample_light+0x2ec>)
            uint32_t sample_time_in_min;

            if(day_state == DUSK) {
                // from last to first
                // manual set remainder to current data in proc_cache to start reading
                proc_cache_remainder = PROC_CACHE_MAX_REMAINDER - proc_cache_remainder;
    107c:	8033      	strh	r3, [r6, #0]
                // if crossed threshold
                if(threshold_idx != IDX_INIT) {
    107e:	8813      	ldrh	r3, [r2, #0]
    1080:	4d1a      	ldr	r5, [pc, #104]	; (10ec <sample_light+0x2d4>)
    1082:	2bff      	cmp	r3, #255	; 0xff
    1084:	d00c      	beq.n	10a0 <sample_light+0x288>
                    if(threshold_idx + THRESHOLD_IDX_SHIFT > max_idx) {
    1086:	492e      	ldr	r1, [pc, #184]	; (1140 <sample_light+0x328>)
    1088:	8814      	ldrh	r4, [r2, #0]
    108a:	880b      	ldrh	r3, [r1, #0]
    108c:	8828      	ldrh	r0, [r5, #0]
    108e:	18e3      	adds	r3, r4, r3
    1090:	4283      	cmp	r3, r0
    1092:	dd01      	ble.n	1098 <sample_light+0x280>
                        starting_idx = max_idx;
    1094:	882c      	ldrh	r4, [r5, #0]
    1096:	e005      	b.n	10a4 <sample_light+0x28c>
                    }
                    else {
                        starting_idx = threshold_idx + THRESHOLD_IDX_SHIFT;
    1098:	8813      	ldrh	r3, [r2, #0]
    109a:	880c      	ldrh	r4, [r1, #0]
    109c:	191c      	adds	r4, r3, r4
    109e:	e001      	b.n	10a4 <sample_light+0x28c>
                    }
                }
                else {
                    starting_idx = min_light_idx;
    10a0:	4b10      	ldr	r3, [pc, #64]	; (10e4 <sample_light+0x2cc>)
    10a2:	881c      	ldrh	r4, [r3, #0]
                }

                sample_time_in_min = light_meas_end_time_in_min;
                mbus_write_message32(0xB9, light_meas_start_time_in_min);
    10a4:	4b27      	ldr	r3, [pc, #156]	; (1144 <sample_light+0x32c>)
    10a6:	20b9      	movs	r0, #185	; 0xb9
    10a8:	6819      	ldr	r1, [r3, #0]
    10aa:	f7fe fffd 	bl	a8 <mbus_write_message32>

                // last to first
                start = max_idx;
                end = -1;
                sign = -1;
    10ae:	2301      	movs	r3, #1

                sample_time_in_min = light_meas_end_time_in_min;
                mbus_write_message32(0xB9, light_meas_start_time_in_min);

                // last to first
                start = max_idx;
    10b0:	882d      	ldrh	r5, [r5, #0]
                end = -1;
                sign = -1;
    10b2:	425b      	negs	r3, r3
                    else {
                        starting_idx = threshold_idx + THRESHOLD_IDX_SHIFT;
                    }
                }
                else {
                    starting_idx = min_light_idx;
    10b4:	b2a4      	uxth	r4, r4

                sample_time_in_min = light_meas_end_time_in_min;
                mbus_write_message32(0xB9, light_meas_start_time_in_min);

                // last to first
                start = max_idx;
    10b6:	b22d      	sxth	r5, r5
                end = -1;
                sign = -1;
    10b8:	9302      	str	r3, [sp, #8]
                sample_time_in_min = light_meas_end_time_in_min;
                mbus_write_message32(0xB9, light_meas_start_time_in_min);

                // last to first
                start = max_idx;
                end = -1;
    10ba:	9303      	str	r3, [sp, #12]
    10bc:	e078      	b.n	11b0 <sample_light+0x398>
    10be:	46c0      	nop			; (mov r8, r8)
    10c0:	00003a48 	.word	0x00003a48
    10c4:	00003abe 	.word	0x00003abe
    10c8:	0000390a 	.word	0x0000390a
    10cc:	0000ffff 	.word	0x0000ffff
    10d0:	00003ad8 	.word	0x00003ad8
    10d4:	00003a7c 	.word	0x00003a7c
    10d8:	00003a78 	.word	0x00003a78
    10dc:	00003aa4 	.word	0x00003aa4
    10e0:	00003930 	.word	0x00003930
    10e4:	00003832 	.word	0x00003832
    10e8:	0000390e 	.word	0x0000390e
    10ec:	00003b00 	.word	0x00003b00
    10f0:	00003a50 	.word	0x00003a50
    10f4:	00003a70 	.word	0x00003a70
    10f8:	00003acc 	.word	0x00003acc
    10fc:	00003a18 	.word	0x00003a18
    1100:	0000a8bf 	.word	0x0000a8bf
    1104:	0000382a 	.word	0x0000382a
    1108:	00003808 	.word	0x00003808
    110c:	00003b20 	.word	0x00003b20
    1110:	00003ac4 	.word	0x00003ac4
    1114:	00003b1c 	.word	0x00003b1c
    1118:	00003830 	.word	0x00003830
    111c:	00003aac 	.word	0x00003aac
    1120:	0000a8be 	.word	0x0000a8be
    1124:	00004c90 	.word	0x00004c90
    1128:	00003a30 	.word	0x00003a30
    112c:	ffff573f 	.word	0xffff573f
    1130:	00003a44 	.word	0x00003a44
    1134:	a0001200 	.word	0xa0001200
    1138:	a000007c 	.word	0xa000007c
    113c:	0000382c 	.word	0x0000382c
    1140:	00003910 	.word	0x00003910
    1144:	00003b08 	.word	0x00003b08
                sign = -1;
            }
            else {
                // going from first to last, left shift 0s into proc_cache
                right_shift_arr(proc_cache, 0, PROC_CACHE_LEN, 1 - proc_cache_remainder);
    1148:	2301      	movs	r3, #1
    114a:	4f3f      	ldr	r7, [pc, #252]	; (1248 <sample_light+0x430>)
    114c:	1a9b      	subs	r3, r3, r2
    114e:	b21b      	sxth	r3, r3
    1150:	220a      	movs	r2, #10
    1152:	0029      	movs	r1, r5
    1154:	0038      	movs	r0, r7
    1156:	f7ff fb49 	bl	7ec <right_shift_arr>
                mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) cache_addr, proc_cache, PROC_CACHE_LEN - 1);
    115a:	4b3c      	ldr	r3, [pc, #240]	; (124c <sample_light+0x434>)
    115c:	003a      	movs	r2, r7
    115e:	8819      	ldrh	r1, [r3, #0]
    1160:	2006      	movs	r0, #6
    1162:	2309      	movs	r3, #9
    1164:	b289      	uxth	r1, r1
    1166:	f7fe ffeb 	bl	140 <mbus_copy_mem_from_local_to_remote_bulk>

                proc_cache_remainder = 0; // manually set to 0 to start reading
                cache_addr = CACHE_START_ADDR;
    116a:	4b39      	ldr	r3, [pc, #228]	; (1250 <sample_light+0x438>)
    116c:	4a37      	ldr	r2, [pc, #220]	; (124c <sample_light+0x434>)
            else {
                // going from first to last, left shift 0s into proc_cache
                right_shift_arr(proc_cache, 0, PROC_CACHE_LEN, 1 - proc_cache_remainder);
                mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) cache_addr, proc_cache, PROC_CACHE_LEN - 1);

                proc_cache_remainder = 0; // manually set to 0 to start reading
    116e:	8035      	strh	r5, [r6, #0]
                cache_addr = CACHE_START_ADDR;
    1170:	8013      	strh	r3, [r2, #0]

                if(day_state == NOON) {
    1172:	8823      	ldrh	r3, [r4, #0]
                    starting_idx = 0;
    1174:	002c      	movs	r4, r5
                mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) cache_addr, proc_cache, PROC_CACHE_LEN - 1);

                proc_cache_remainder = 0; // manually set to 0 to start reading
                cache_addr = CACHE_START_ADDR;

                if(day_state == NOON) {
    1176:	2b01      	cmp	r3, #1
    1178:	d00f      	beq.n	119a <sample_light+0x382>
                    starting_idx = 0;
                }
                else if(threshold_idx != IDX_INIT) {
    117a:	4b36      	ldr	r3, [pc, #216]	; (1254 <sample_light+0x43c>)
    117c:	881a      	ldrh	r2, [r3, #0]
    117e:	2aff      	cmp	r2, #255	; 0xff
    1180:	d008      	beq.n	1194 <sample_light+0x37c>
                    if(threshold_idx < THRESHOLD_IDX_SHIFT) {
    1182:	4a35      	ldr	r2, [pc, #212]	; (1258 <sample_light+0x440>)
    1184:	8818      	ldrh	r0, [r3, #0]
    1186:	8811      	ldrh	r1, [r2, #0]
    1188:	4288      	cmp	r0, r1
    118a:	d306      	bcc.n	119a <sample_light+0x382>
                        starting_idx = 0;
                    }
                    else {
                        starting_idx = threshold_idx - THRESHOLD_IDX_SHIFT;
    118c:	881c      	ldrh	r4, [r3, #0]
    118e:	8813      	ldrh	r3, [r2, #0]
    1190:	1ae4      	subs	r4, r4, r3
    1192:	e001      	b.n	1198 <sample_light+0x380>
                    }
                }
                else {
                    starting_idx = min_light_idx;
    1194:	4b31      	ldr	r3, [pc, #196]	; (125c <sample_light+0x444>)
    1196:	881c      	ldrh	r4, [r3, #0]
    1198:	b2a4      	uxth	r4, r4
                }

                sample_time_in_min = light_meas_start_time_in_min;
    119a:	4b31      	ldr	r3, [pc, #196]	; (1260 <sample_light+0x448>)

                // first to last
                start = 0;
    119c:	2500      	movs	r5, #0
                }
                else {
                    starting_idx = min_light_idx;
                }

                sample_time_in_min = light_meas_start_time_in_min;
    119e:	681b      	ldr	r3, [r3, #0]
    11a0:	9301      	str	r3, [sp, #4]

                // first to last
                start = 0;
                end = max_idx + 1;
    11a2:	4b30      	ldr	r3, [pc, #192]	; (1264 <sample_light+0x44c>)
    11a4:	881b      	ldrh	r3, [r3, #0]
    11a6:	3301      	adds	r3, #1
    11a8:	b21b      	sxth	r3, r3
    11aa:	9303      	str	r3, [sp, #12]
                sign = 1;
    11ac:	2301      	movs	r3, #1
    11ae:	9302      	str	r3, [sp, #8]

            int16_t i;
            uint16_t next_sample_idx = starting_idx;
            uint16_t sample_idx = 0;
            uint16_t interval_idx = 0;
            has_time = false;       // Store time at the start of each day state TOOD: check if this is correct
    11b0:	2600      	movs	r6, #0
                sign = 1;
            }

            int16_t i;
            uint16_t next_sample_idx = starting_idx;
            uint16_t sample_idx = 0;
    11b2:	0037      	movs	r7, r6
            uint16_t interval_idx = 0;
            has_time = false;       // Store time at the start of each day state TOOD: check if this is correct
    11b4:	4b2c      	ldr	r3, [pc, #176]	; (1268 <sample_light+0x450>)
    11b6:	701e      	strb	r6, [r3, #0]
            for(i = start; i != end; i += sign) {
    11b8:	9b03      	ldr	r3, [sp, #12]
    11ba:	42ab      	cmp	r3, r5
    11bc:	d031      	beq.n	1222 <sample_light+0x40a>
                uint16_t log_light = read_next_from_proc_cache();
    11be:	f7ff fce7 	bl	b90 <read_next_from_proc_cache>
                if(i == next_sample_idx) {
    11c2:	42a5      	cmp	r5, r4
    11c4:	d119      	bne.n	11fa <sample_light+0x3e2>
                    store_to_code_cache(log_light, sample_idx, sample_time_in_min);
    11c6:	0039      	movs	r1, r7
    11c8:	9a01      	ldr	r2, [sp, #4]
    11ca:	f7ff fc17 	bl	9fc <store_to_code_cache>

                    sample_idx++;
                    if(day_state == NOON) {
    11ce:	4b27      	ldr	r3, [pc, #156]	; (126c <sample_light+0x454>)
            for(i = start; i != end; i += sign) {
                uint16_t log_light = read_next_from_proc_cache();
                if(i == next_sample_idx) {
                    store_to_code_cache(log_light, sample_idx, sample_time_in_min);

                    sample_idx++;
    11d0:	3701      	adds	r7, #1
                    if(day_state == NOON) {
    11d2:	881b      	ldrh	r3, [r3, #0]
            for(i = start; i != end; i += sign) {
                uint16_t log_light = read_next_from_proc_cache();
                if(i == next_sample_idx) {
                    store_to_code_cache(log_light, sample_idx, sample_time_in_min);

                    sample_idx++;
    11d4:	b2bf      	uxth	r7, r7
                    if(day_state == NOON) {
    11d6:	2b01      	cmp	r3, #1
    11d8:	d101      	bne.n	11de <sample_light+0x3c6>
                        next_sample_idx++;
    11da:	1c6c      	adds	r4, r5, #1
    11dc:	e00c      	b.n	11f8 <sample_light+0x3e0>
                    }
                    else {
                        if(sample_idx >= resample_indices[interval_idx]) {
    11de:	4a24      	ldr	r2, [pc, #144]	; (1270 <sample_light+0x458>)
    11e0:	0073      	lsls	r3, r6, #1
    11e2:	5a9b      	ldrh	r3, [r3, r2]
    11e4:	429f      	cmp	r7, r3
    11e6:	d301      	bcc.n	11ec <sample_light+0x3d4>
                            interval_idx++;
    11e8:	3601      	adds	r6, #1
    11ea:	b2b6      	uxth	r6, r6
                        }

                        next_sample_idx += (intervals[interval_idx] * sign);
    11ec:	4b21      	ldr	r3, [pc, #132]	; (1274 <sample_light+0x45c>)
    11ee:	0072      	lsls	r2, r6, #1
    11f0:	5ad3      	ldrh	r3, [r2, r3]
    11f2:	9a02      	ldr	r2, [sp, #8]
    11f4:	4353      	muls	r3, r2
    11f6:	18e4      	adds	r4, r4, r3
    11f8:	b2a4      	uxth	r4, r4
                    }
                }

                if(day_state == NOON) {
    11fa:	4b1c      	ldr	r3, [pc, #112]	; (126c <sample_light+0x454>)
    11fc:	881a      	ldrh	r2, [r3, #0]
    11fe:	2a01      	cmp	r2, #1
    1200:	d102      	bne.n	1208 <sample_light+0x3f0>
                    sample_time_in_min += 32;
    1202:	9b01      	ldr	r3, [sp, #4]
    1204:	3320      	adds	r3, #32
    1206:	e007      	b.n	1218 <sample_light+0x400>
                }
                else if(day_state == DAWN) {
    1208:	881b      	ldrh	r3, [r3, #0]
    120a:	2b00      	cmp	r3, #0
    120c:	d102      	bne.n	1214 <sample_light+0x3fc>
                    sample_time_in_min += 1;
    120e:	9b01      	ldr	r3, [sp, #4]
    1210:	3301      	adds	r3, #1
    1212:	e001      	b.n	1218 <sample_light+0x400>
                }
                else {
                    sample_time_in_min -= 1;
    1214:	9b01      	ldr	r3, [sp, #4]
    1216:	3b01      	subs	r3, #1
    1218:	9301      	str	r3, [sp, #4]
    121a:	9b02      	ldr	r3, [sp, #8]
    121c:	18ed      	adds	r5, r5, r3
    121e:	b22d      	sxth	r5, r5
    1220:	e7ca      	b.n	11b8 <sample_light+0x3a0>
                }
            }
            store_day_state_stop();
    1222:	f7ff fc71 	bl	b08 <store_day_state_stop>
        }
    
        // reset new state variables
        day_state = (day_state + 1) & 0b11;
    1226:	4911      	ldr	r1, [pc, #68]	; (126c <sample_light+0x454>)
    1228:	880b      	ldrh	r3, [r1, #0]
    122a:	1c5a      	adds	r2, r3, #1
    122c:	2303      	movs	r3, #3
    122e:	4013      	ands	r3, r2
    1230:	800b      	strh	r3, [r1, #0]
        set_new_state();
    1232:	f7ff fd41 	bl	cb8 <set_new_state>
    1236:	e004      	b.n	1242 <sample_light+0x42a>
    }
    else {
        max_idx++;
    1238:	4a0a      	ldr	r2, [pc, #40]	; (1264 <sample_light+0x44c>)
    123a:	8813      	ldrh	r3, [r2, #0]
    123c:	3301      	adds	r3, #1
    123e:	b29b      	uxth	r3, r3
    1240:	8013      	strh	r3, [r2, #0]
    }
}
    1242:	b005      	add	sp, #20
    1244:	bdf0      	pop	{r4, r5, r6, r7, pc}
    1246:	46c0      	nop			; (mov r8, r8)
    1248:	000039bc 	.word	0x000039bc
    124c:	00003890 	.word	0x00003890
    1250:	00003e98 	.word	0x00003e98
    1254:	0000382a 	.word	0x0000382a
    1258:	00003910 	.word	0x00003910
    125c:	00003832 	.word	0x00003832
    1260:	00003b08 	.word	0x00003b08
    1264:	00003b00 	.word	0x00003b00
    1268:	00003a74 	.word	0x00003a74
    126c:	00003930 	.word	0x00003930
    1270:	000038a4 	.word	0x000038a4
    1274:	000036a8 	.word	0x000036a8

Disassembly of section .text.flush_temp_cache:

00001278 <flush_temp_cache>:
volatile uint32_t temp_cache[TEMP_CACHE_LEN];
volatile uint16_t temp_cache_remainder = TEMP_CACHE_MAX_REMAINDER;
volatile uint16_t last_log_temp = 0;

void flush_temp_cache() {
    if(temp_cache_remainder < TEMP_CACHE_MAX_REMAINDER) {
    1278:	2310      	movs	r3, #16

volatile uint32_t temp_cache[TEMP_CACHE_LEN];
volatile uint16_t temp_cache_remainder = TEMP_CACHE_MAX_REMAINDER;
volatile uint16_t last_log_temp = 0;

void flush_temp_cache() {
    127a:	b570      	push	{r4, r5, r6, lr}
    if(temp_cache_remainder < TEMP_CACHE_MAX_REMAINDER) {
    127c:	4c11      	ldr	r4, [pc, #68]	; (12c4 <flush_temp_cache+0x4c>)
    127e:	33ff      	adds	r3, #255	; 0xff
    1280:	8822      	ldrh	r2, [r4, #0]
    1282:	429a      	cmp	r2, r3
    1284:	d81c      	bhi.n	12c0 <flush_temp_cache+0x48>
        // left shift 0s into unused space
        right_shift_arr(temp_cache, 0, TEMP_CACHE_LEN, -temp_cache_remainder);
    1286:	8823      	ldrh	r3, [r4, #0]
    1288:	4d0f      	ldr	r5, [pc, #60]	; (12c8 <flush_temp_cache+0x50>)
    128a:	425b      	negs	r3, r3
    128c:	b21b      	sxth	r3, r3
    128e:	2209      	movs	r2, #9
    1290:	2100      	movs	r1, #0
    1292:	0028      	movs	r0, r5
    1294:	f7ff faaa 	bl	7ec <right_shift_arr>

        // mark temp packet
        temp_cache[0] &= 0x0000FFFF;
    1298:	682b      	ldr	r3, [r5, #0]

        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, temp_cache, TEMP_CACHE_LEN - 1);
    129a:	4e0c      	ldr	r6, [pc, #48]	; (12cc <flush_temp_cache+0x54>)
    if(temp_cache_remainder < TEMP_CACHE_MAX_REMAINDER) {
        // left shift 0s into unused space
        right_shift_arr(temp_cache, 0, TEMP_CACHE_LEN, -temp_cache_remainder);

        // mark temp packet
        temp_cache[0] &= 0x0000FFFF;
    129c:	b29b      	uxth	r3, r3
    129e:	602b      	str	r3, [r5, #0]

        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, temp_cache, TEMP_CACHE_LEN - 1);
    12a0:	002a      	movs	r2, r5
    12a2:	2308      	movs	r3, #8
    12a4:	8831      	ldrh	r1, [r6, #0]
    12a6:	2006      	movs	r0, #6
    12a8:	f7fe ff4a 	bl	140 <mbus_copy_mem_from_local_to_remote_bulk>
        code_addr += TEMP_CACHE_LEN << 2;
    12ac:	8833      	ldrh	r3, [r6, #0]
        max_unit_count++;
    12ae:	4a08      	ldr	r2, [pc, #32]	; (12d0 <flush_temp_cache+0x58>)

        // mark temp packet
        temp_cache[0] &= 0x0000FFFF;

        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, temp_cache, TEMP_CACHE_LEN - 1);
        code_addr += TEMP_CACHE_LEN << 2;
    12b0:	3324      	adds	r3, #36	; 0x24
    12b2:	8033      	strh	r3, [r6, #0]
        max_unit_count++;
    12b4:	8813      	ldrh	r3, [r2, #0]
    12b6:	3301      	adds	r3, #1
    12b8:	8013      	strh	r3, [r2, #0]
        temp_cache_remainder = TEMP_CACHE_MAX_REMAINDER;
    12ba:	2388      	movs	r3, #136	; 0x88
    12bc:	005b      	lsls	r3, r3, #1
    12be:	8023      	strh	r3, [r4, #0]
    }
}
    12c0:	bd70      	pop	{r4, r5, r6, pc}
    12c2:	46c0      	nop			; (mov r8, r8)
    12c4:	000038b4 	.word	0x000038b4
    12c8:	00003994 	.word	0x00003994
    12cc:	00003ae8 	.word	0x00003ae8
    12d0:	00003a42 	.word	0x00003a42

Disassembly of section .text.store_code_to_temp_cache:

000012d4 <store_code_to_temp_cache>:

void store_code_to_temp_cache(uint16_t code, uint16_t len) {
    12d4:	b510      	push	{r4, lr}
    12d6:	000c      	movs	r4, r1
    right_shift_arr(temp_cache, code, TEMP_CACHE_LEN, -len);
    12d8:	424b      	negs	r3, r1
    12da:	b21b      	sxth	r3, r3
    12dc:	0001      	movs	r1, r0
    12de:	2209      	movs	r2, #9
    12e0:	4804      	ldr	r0, [pc, #16]	; (12f4 <store_code_to_temp_cache+0x20>)
    12e2:	f7ff fa83 	bl	7ec <right_shift_arr>
    temp_cache_remainder -= len;
    12e6:	4b04      	ldr	r3, [pc, #16]	; (12f8 <store_code_to_temp_cache+0x24>)
    12e8:	8819      	ldrh	r1, [r3, #0]
    12ea:	1b09      	subs	r1, r1, r4
    12ec:	b289      	uxth	r1, r1
    12ee:	8019      	strh	r1, [r3, #0]
}
    12f0:	bd10      	pop	{r4, pc}
    12f2:	46c0      	nop			; (mov r8, r8)
    12f4:	00003994 	.word	0x00003994
    12f8:	000038b4 	.word	0x000038b4

Disassembly of section .text.sample_temp:

000012fc <sample_temp>:

void sample_temp() {
    // Take 3 bits above decimal point and 4 bits under
    uint16_t log_temp = log2(snt_sys_temp_code) >> (8 - TEMP_RES); 
    12fc:	4b26      	ldr	r3, [pc, #152]	; (1398 <sample_temp+0x9c>)
void store_code_to_temp_cache(uint16_t code, uint16_t len) {
    right_shift_arr(temp_cache, code, TEMP_CACHE_LEN, -len);
    temp_cache_remainder -= len;
}

void sample_temp() {
    12fe:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    // Take 3 bits above decimal point and 4 bits under
    uint16_t log_temp = log2(snt_sys_temp_code) >> (8 - TEMP_RES); 
    1300:	6818      	ldr	r0, [r3, #0]
    1302:	2100      	movs	r1, #0
    1304:	f7ff fac2 	bl	88c <log2>
    int8_t diff = log_temp - last_log_temp;
    1308:	4b24      	ldr	r3, [pc, #144]	; (139c <sample_temp+0xa0>)
    temp_cache_remainder -= len;
}

void sample_temp() {
    // Take 3 bits above decimal point and 4 bits under
    uint16_t log_temp = log2(snt_sys_temp_code) >> (8 - TEMP_RES); 
    130a:	0840      	lsrs	r0, r0, #1
    int8_t diff = log_temp - last_log_temp;
    130c:	881c      	ldrh	r4, [r3, #0]
    temp_cache_remainder -= len;
}

void sample_temp() {
    // Take 3 bits above decimal point and 4 bits under
    uint16_t log_temp = log2(snt_sys_temp_code) >> (8 - TEMP_RES); 
    130e:	b285      	uxth	r5, r0
    int8_t diff = log_temp - last_log_temp;
    1310:	1b2c      	subs	r4, r5, r4
    1312:	b2e4      	uxtb	r4, r4
    last_log_temp = log_temp;
    1314:	801d      	strh	r5, [r3, #0]
    uint16_t code_idx = 0;
    uint16_t len_needed = 0;
    uint16_t code = 0;


    if(diff < 2 && diff >= -2) {
    1316:	1ca3      	adds	r3, r4, #2
    1318:	b2db      	uxtb	r3, r3
    131a:	2b03      	cmp	r3, #3
    131c:	d809      	bhi.n	1332 <sample_temp+0x36>
        code_idx = diff + 2;
    131e:	b264      	sxtb	r4, r4
    1320:	3402      	adds	r4, #2
    1322:	b2a4      	uxth	r4, r4
        len_needed = temp_code_lengths[code_idx];
    1324:	4b1e      	ldr	r3, [pc, #120]	; (13a0 <sample_temp+0xa4>)
        code = temp_diff_codes[code_idx];
    1326:	491f      	ldr	r1, [pc, #124]	; (13a4 <sample_temp+0xa8>)
    uint16_t code = 0;


    if(diff < 2 && diff >= -2) {
        code_idx = diff + 2;
        len_needed = temp_code_lengths[code_idx];
    1328:	0062      	lsls	r2, r4, #1
    132a:	5ad3      	ldrh	r3, [r2, r3]
        code = temp_diff_codes[code_idx];
    132c:	5a52      	ldrh	r2, [r2, r1]
    132e:	9201      	str	r2, [sp, #4]
    1330:	e003      	b.n	133a <sample_temp+0x3e>
    }
    else {
        code_idx = 4;
        len_needed = temp_code_lengths[code_idx] + TEMP_RES;
        code = temp_diff_codes[code_idx];   // FIXME: redundant code, move outside of block
    1332:	2309      	movs	r3, #9
        code_idx = diff + 2;
        len_needed = temp_code_lengths[code_idx];
        code = temp_diff_codes[code_idx];
    }
    else {
        code_idx = 4;
    1334:	2404      	movs	r4, #4
        len_needed = temp_code_lengths[code_idx] + TEMP_RES;
        code = temp_diff_codes[code_idx];   // FIXME: redundant code, move outside of block
    1336:	9301      	str	r3, [sp, #4]
        len_needed = temp_code_lengths[code_idx];
        code = temp_diff_codes[code_idx];
    }
    else {
        code_idx = 4;
        len_needed = temp_code_lengths[code_idx] + TEMP_RES;
    1338:	3302      	adds	r3, #2
        code = temp_diff_codes[code_idx];   // FIXME: redundant code, move outside of block
    }

    if(temp_cache_remainder < len_needed) {
    133a:	4f1b      	ldr	r7, [pc, #108]	; (13a8 <sample_temp+0xac>)
    133c:	883a      	ldrh	r2, [r7, #0]
    133e:	4293      	cmp	r3, r2
    1340:	d901      	bls.n	1346 <sample_temp+0x4a>
        flush_temp_cache();
    1342:	f7ff ff99 	bl	1278 <flush_temp_cache>
    }

    // increment temp index here to make sure we store the correct index
    store_temp_index++;
    1346:	4e19      	ldr	r6, [pc, #100]	; (13ac <sample_temp+0xb0>)
    1348:	8833      	ldrh	r3, [r6, #0]
    134a:	3301      	adds	r3, #1
    134c:	b29b      	uxth	r3, r3
    134e:	8033      	strh	r3, [r6, #0]

    if(store_temp_index > 47) {
    1350:	8833      	ldrh	r3, [r6, #0]
    1352:	2b2f      	cmp	r3, #47	; 0x2f
    1354:	d901      	bls.n	135a <sample_temp+0x5e>
        store_temp_index = 0;   // resets when taking the midnight data
    1356:	2300      	movs	r3, #0
    1358:	8033      	strh	r3, [r6, #0]
    }

    if(temp_cache_remainder == TEMP_CACHE_MAX_REMAINDER) {
    135a:	2388      	movs	r3, #136	; 0x88
    135c:	883a      	ldrh	r2, [r7, #0]
    135e:	005b      	lsls	r3, r3, #1
    1360:	429a      	cmp	r2, r3
    1362:	d10b      	bne.n	137c <sample_temp+0x80>
        // if first data, store timestamp and full data
        store_code_to_temp_cache(day_count, 7);
    1364:	4b12      	ldr	r3, [pc, #72]	; (13b0 <sample_temp+0xb4>)
    1366:	2107      	movs	r1, #7
    1368:	8818      	ldrh	r0, [r3, #0]
    136a:	b280      	uxth	r0, r0
    136c:	f7ff ffb2 	bl	12d4 <store_code_to_temp_cache>
        store_code_to_temp_cache(store_temp_index, 6);
    1370:	8830      	ldrh	r0, [r6, #0]
    1372:	2106      	movs	r1, #6
    1374:	b280      	uxth	r0, r0
    1376:	f7ff ffad 	bl	12d4 <store_code_to_temp_cache>
    137a:	e007      	b.n	138c <sample_temp+0x90>
        store_code_to_temp_cache(log_temp, TEMP_RES);
    }
    else {
        // else just store diff
        store_code_to_temp_cache(code, temp_code_lengths[code_idx]);
    137c:	4b08      	ldr	r3, [pc, #32]	; (13a0 <sample_temp+0xa4>)
    137e:	0062      	lsls	r2, r4, #1
    1380:	5ad1      	ldrh	r1, [r2, r3]
    1382:	9801      	ldr	r0, [sp, #4]
    1384:	f7ff ffa6 	bl	12d4 <store_code_to_temp_cache>

        if(code_idx == 4) {
    1388:	2c04      	cmp	r4, #4
    138a:	d103      	bne.n	1394 <sample_temp+0x98>
            // if storing 7 bits, just store full data
            store_code_to_temp_cache(log_temp, TEMP_RES);
    138c:	2107      	movs	r1, #7
    138e:	0028      	movs	r0, r5
    1390:	f7ff ffa0 	bl	12d4 <store_code_to_temp_cache>
        }
    }
}
    1394:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
    1396:	46c0      	nop			; (mov r8, r8)
    1398:	000038b8 	.word	0x000038b8
    139c:	00003afa 	.word	0x00003afa
    13a0:	000036ba 	.word	0x000036ba
    13a4:	000036b0 	.word	0x000036b0
    13a8:	000038b4 	.word	0x000038b4
    13ac:	00003a58 	.word	0x00003a58
    13b0:	00003a3a 	.word	0x00003a3a

Disassembly of section .text.set_projected_end_time:

000013b4 <set_projected_end_time>:
        }
    }
}

// set next wake up time, projected_end_time_in_sec == next_light_meas_time, then sample light
void set_projected_end_time() {
    13b4:	b510      	push	{r4, lr}
    uint32_t temp = next_light_meas_time - projected_end_time_in_sec;
    if(temp <= XO_8_MIN) {
    13b6:	24f0      	movs	r4, #240	; 0xf0
    }
}

// set next wake up time, projected_end_time_in_sec == next_light_meas_time, then sample light
void set_projected_end_time() {
    uint32_t temp = next_light_meas_time - projected_end_time_in_sec;
    13b8:	4811      	ldr	r0, [pc, #68]	; (1400 <set_projected_end_time+0x4c>)
    13ba:	4a12      	ldr	r2, [pc, #72]	; (1404 <set_projected_end_time+0x50>)
    13bc:	6803      	ldr	r3, [r0, #0]
    13be:	6811      	ldr	r1, [r2, #0]
    if(temp <= XO_8_MIN) {
    13c0:	0064      	lsls	r4, r4, #1
    }
}

// set next wake up time, projected_end_time_in_sec == next_light_meas_time, then sample light
void set_projected_end_time() {
    uint32_t temp = next_light_meas_time - projected_end_time_in_sec;
    13c2:	1a5b      	subs	r3, r3, r1
    if(temp <= XO_8_MIN) {
    13c4:	42a3      	cmp	r3, r4
    13c6:	d80d      	bhi.n	13e4 <set_projected_end_time+0x30>
        projected_end_time_in_sec = next_light_meas_time;
    13c8:	6801      	ldr	r1, [r0, #0]
    13ca:	6011      	str	r1, [r2, #0]
    13cc:	4a0e      	ldr	r2, [pc, #56]	; (1408 <set_projected_end_time+0x54>)
        if(temp <= XO_1_MIN) {
    13ce:	2b3c      	cmp	r3, #60	; 0x3c
    13d0:	d801      	bhi.n	13d6 <set_projected_end_time+0x22>
            lnt_counter_base = 0;
    13d2:	2300      	movs	r3, #0
    13d4:	e004      	b.n	13e0 <set_projected_end_time+0x2c>
        }
        else if(temp <= XO_2_MIN) {
    13d6:	2b78      	cmp	r3, #120	; 0x78
    13d8:	d801      	bhi.n	13de <set_projected_end_time+0x2a>
            lnt_counter_base = 1;
    13da:	2301      	movs	r3, #1
    13dc:	e000      	b.n	13e0 <set_projected_end_time+0x2c>
        }
        else {
            lnt_counter_base = 3;
    13de:	2303      	movs	r3, #3
    13e0:	8013      	strh	r3, [r2, #0]
    13e2:	e00b      	b.n	13fc <set_projected_end_time+0x48>
        }
    }
    else if(temp <= XO_18_MIN) {
    13e4:	2487      	movs	r4, #135	; 0x87
    13e6:	00e4      	lsls	r4, r4, #3
    13e8:	42a3      	cmp	r3, r4
    13ea:	d804      	bhi.n	13f6 <set_projected_end_time+0x42>
        // set 8 minute buffer time to make sure wake up time is power of 2
        // Gives a minimum of 2 minutes of sleep time to set to 8 minutes before light meas
        projected_end_time_in_sec = next_light_meas_time - XO_8_MIN;
    13ec:	6803      	ldr	r3, [r0, #0]
    13ee:	3be1      	subs	r3, #225	; 0xe1
    13f0:	3bff      	subs	r3, #255	; 0xff
    13f2:	6013      	str	r3, [r2, #0]
    13f4:	e002      	b.n	13fc <set_projected_end_time+0x48>
    }
    else {
        // 32 minute interval case
        projected_end_time_in_sec += XO_8_MIN;
    13f6:	31e1      	adds	r1, #225	; 0xe1
    13f8:	31ff      	adds	r1, #255	; 0xff
    13fa:	6011      	str	r1, [r2, #0]
    }
}
    13fc:	bd10      	pop	{r4, pc}
    13fe:	46c0      	nop			; (mov r8, r8)
    1400:	00003a50 	.word	0x00003a50
    1404:	00003a60 	.word	0x00003a60
    1408:	00003a40 	.word	0x00003a40

Disassembly of section .text.crcEnc16:

0000140c <crcEnc16>:

#define DATA_LEN 96
#define CRC_LEN 16

uint32_t* crcEnc16()
{
    140c:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    uint16_t poly_not = ~poly;
    uint16_t remainder = 0x0000;
    uint16_t remainder_shift = 0x0000;
    uint32_t data2 = (radio_data_arr[2] << CRC_LEN) + (radio_data_arr[1] >> CRC_LEN);
    uint32_t data1 = (radio_data_arr[1] << CRC_LEN) + (radio_data_arr[0] >> CRC_LEN);
    uint32_t data0 = radio_data_arr[0] << CRC_LEN;
    140e:	2100      	movs	r1, #0

    uint16_t poly = 0xc002;
    uint16_t poly_not = ~poly;
    uint16_t remainder = 0x0000;
    uint16_t remainder_shift = 0x0000;
    uint32_t data2 = (radio_data_arr[2] << CRC_LEN) + (radio_data_arr[1] >> CRC_LEN);
    1410:	4b1c      	ldr	r3, [pc, #112]	; (1484 <crcEnc16+0x78>)
    1412:	689a      	ldr	r2, [r3, #8]
    1414:	6858      	ldr	r0, [r3, #4]
    1416:	0412      	lsls	r2, r2, #16
    1418:	0c00      	lsrs	r0, r0, #16
    141a:	1810      	adds	r0, r2, r0
    uint32_t data1 = (radio_data_arr[1] << CRC_LEN) + (radio_data_arr[0] >> CRC_LEN);
    141c:	685a      	ldr	r2, [r3, #4]
    141e:	681d      	ldr	r5, [r3, #0]
    1420:	0412      	lsls	r2, r2, #16
    1422:	0c2d      	lsrs	r5, r5, #16
    1424:	1955      	adds	r5, r2, r5
            input_bit = (data1 << (i-32)) > 0x7fffffff;
        else
            input_bit = (data0 << (i-64)) > 0x7fffffff;

        remainder_shift = remainder << 1;
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff));
    1426:	4a18      	ldr	r2, [pc, #96]	; (1488 <crcEnc16+0x7c>)
    uint16_t poly_not = ~poly;
    uint16_t remainder = 0x0000;
    uint16_t remainder_shift = 0x0000;
    uint32_t data2 = (radio_data_arr[2] << CRC_LEN) + (radio_data_arr[1] >> CRC_LEN);
    uint32_t data1 = (radio_data_arr[1] << CRC_LEN) + (radio_data_arr[0] >> CRC_LEN);
    uint32_t data0 = radio_data_arr[0] << CRC_LEN;
    1428:	681c      	ldr	r4, [r3, #0]
            input_bit = (data1 << (i-32)) > 0x7fffffff;
        else
            input_bit = (data0 << (i-64)) > 0x7fffffff;

        remainder_shift = remainder << 1;
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff));
    142a:	4694      	mov	ip, r2
    // intialization
    uint16_t i;

    uint16_t poly = 0xc002;
    uint16_t poly_not = ~poly;
    uint16_t remainder = 0x0000;
    142c:	000b      	movs	r3, r1
    uint16_t remainder_shift = 0x0000;
    uint32_t data2 = (radio_data_arr[2] << CRC_LEN) + (radio_data_arr[1] >> CRC_LEN);
    uint32_t data1 = (radio_data_arr[1] << CRC_LEN) + (radio_data_arr[0] >> CRC_LEN);
    uint32_t data0 = radio_data_arr[0] << CRC_LEN;
    142e:	0424      	lsls	r4, r4, #16
    // LFSR
    uint16_t input_bit;
    for (i = 0; i < DATA_LEN; i++)
    {
        uint16_t MSB;
        if (remainder > 0x7fff)
    1430:	0bde      	lsrs	r6, r3, #15
    1432:	4277      	negs	r7, r6
    1434:	b2ba      	uxth	r2, r7
    1436:	9201      	str	r2, [sp, #4]
    1438:	b28a      	uxth	r2, r1
            MSB = 0xffff;
        else
            MSB = 0x0000;

        if (i < 32)
    143a:	291f      	cmp	r1, #31
    143c:	d802      	bhi.n	1444 <crcEnc16+0x38>
            input_bit = ((data2 << i) > 0x7fffffff);
    143e:	0002      	movs	r2, r0
    1440:	408a      	lsls	r2, r1
    1442:	e00a      	b.n	145a <crcEnc16+0x4e>
        else if (i < 64)
    1444:	2a3f      	cmp	r2, #63	; 0x3f
    1446:	d803      	bhi.n	1450 <crcEnc16+0x44>
            input_bit = (data1 << (i-32)) > 0x7fffffff;
    1448:	000a      	movs	r2, r1
    144a:	002f      	movs	r7, r5
    144c:	3a20      	subs	r2, #32
    144e:	e002      	b.n	1456 <crcEnc16+0x4a>
        else
            input_bit = (data0 << (i-64)) > 0x7fffffff;
    1450:	000a      	movs	r2, r1
    1452:	0027      	movs	r7, r4
    1454:	3a40      	subs	r2, #64	; 0x40
    1456:	4097      	lsls	r7, r2
    1458:	003a      	movs	r2, r7

        remainder_shift = remainder << 1;
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff));
    145a:	4667      	mov	r7, ip
        if (i < 32)
            input_bit = ((data2 << i) > 0x7fffffff);
        else if (i < 64)
            input_bit = (data1 << (i-32)) > 0x7fffffff;
        else
            input_bit = (data0 << (i-64)) > 0x7fffffff;
    145c:	0fd2      	lsrs	r2, r2, #31

        remainder_shift = remainder << 1;
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff));
    145e:	4072      	eors	r2, r6
        else if (i < 64)
            input_bit = (data1 << (i-32)) > 0x7fffffff;
        else
            input_bit = (data0 << (i-64)) > 0x7fffffff;

        remainder_shift = remainder << 1;
    1460:	005b      	lsls	r3, r3, #1
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff));
    1462:	9e01      	ldr	r6, [sp, #4]
        else if (i < 64)
            input_bit = (data1 << (i-32)) > 0x7fffffff;
        else
            input_bit = (data0 << (i-64)) > 0x7fffffff;

        remainder_shift = remainder << 1;
    1464:	b29b      	uxth	r3, r3
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff));
    1466:	405e      	eors	r6, r3
    1468:	401f      	ands	r7, r3
    146a:	0033      	movs	r3, r6
    146c:	4e07      	ldr	r6, [pc, #28]	; (148c <crcEnc16+0x80>)
    146e:	18ba      	adds	r2, r7, r2
    1470:	4033      	ands	r3, r6
    1472:	3101      	adds	r1, #1
    1474:	4313      	orrs	r3, r2
    uint32_t data1 = (radio_data_arr[1] << CRC_LEN) + (radio_data_arr[0] >> CRC_LEN);
    uint32_t data0 = radio_data_arr[0] << CRC_LEN;

    // LFSR
    uint16_t input_bit;
    for (i = 0; i < DATA_LEN; i++)
    1476:	2960      	cmp	r1, #96	; 0x60
    1478:	d1da      	bne.n	1430 <crcEnc16+0x24>
        remainder_shift = remainder << 1;
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff));
    }

    static uint32_t msg_out[1];
    msg_out[0] = data0 + remainder;
    147a:	4805      	ldr	r0, [pc, #20]	; (1490 <crcEnc16+0x84>)
    147c:	191b      	adds	r3, r3, r4
    147e:	6003      	str	r3, [r0, #0]
    //radio_data_arr[0] = data2;
    //radio_data_arr[1] = data1;
    //radio_data_arr[2] = data0;

    return msg_out;    
}
    1480:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    1482:	46c0      	nop			; (mov r8, r8)
    1484:	000039e8 	.word	0x000039e8
    1488:	00003ffd 	.word	0x00003ffd
    148c:	ffffc002 	.word	0xffffc002
    1490:	00003af4 	.word	0x00003af4

Disassembly of section .text.pmu_set_sar_ratio:

00001494 <pmu_set_sar_ratio>:
             (ratio)));  // binary converter's conversion ratio (7'h00)
    }
#endif
}*/

void pmu_set_sar_ratio (uint32_t sar_ratio) {
    1494:	b570      	push	{r4, r5, r6, lr}
        | (0 << 20) // horizon
    ));

    // Read the current ADC offset
    pmu_reg_write(0x00,0x03);
    uint32_t adc_vbat   = (*REG0 & 0xFF0000) >> 16;
    1496:	24a0      	movs	r4, #160	; 0xa0
             (ratio)));  // binary converter's conversion ratio (7'h00)
    }
#endif
}*/

void pmu_set_sar_ratio (uint32_t sar_ratio) {
    1498:	0005      	movs	r5, r0
        mbus_write_message32(0xD8, 0x00000000);
    #endif

    // Just to confirm that ADC has been disabled in Active
    // Also disable 'horizon' in Active so that this sar ratio change takes place only once.
    pmu_reg_write (60, // 0x3C (CTRL_DESIRED_STATE_ACTIVE)
    149a:	491c      	ldr	r1, [pc, #112]	; (150c <pmu_set_sar_ratio+0x78>)
    149c:	203c      	movs	r0, #60	; 0x3c
    149e:	f7fe ff4d 	bl	33c <pmu_reg_write>
        | (0 << 20) // horizon
    ));

    // Read the current ADC offset
    pmu_reg_write(0x00,0x03);
    uint32_t adc_vbat   = (*REG0 & 0xFF0000) >> 16;
    14a2:	0624      	lsls	r4, r4, #24
        | (0 << 19) // vbat_read_only
        | (0 << 20) // horizon
    ));

    // Read the current ADC offset
    pmu_reg_write(0x00,0x03);
    14a4:	2103      	movs	r1, #3
    14a6:	2000      	movs	r0, #0
    14a8:	f7fe ff48 	bl	33c <pmu_reg_write>
    uint32_t adc_vbat   = (*REG0 & 0xFF0000) >> 16;
    14ac:	6823      	ldr	r3, [r4, #0]
    uint32_t adc_offset = (*REG0 & 0x007F00) >> 8;
    14ae:	6821      	ldr	r1, [r4, #0]
    uint32_t adc_dout   = (*REG0 & 0x0000FF) >> 0;
    14b0:	6823      	ldr	r3, [r4, #0]
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x01 << 24) | *REG0));
    #endif

    // Override ADC Output so that the ADC always outputs near 0
    pmu_reg_write (2, // 0x02 (ADC_CONFIG_OVERRIDE)
    14b2:	2381      	movs	r3, #129	; 0x81
    14b4:	0449      	lsls	r1, r1, #17
    14b6:	041b      	lsls	r3, r3, #16
    14b8:	0e49      	lsrs	r1, r1, #25
    14ba:	4319      	orrs	r1, r3
    14bc:	2002      	movs	r0, #2
    14be:	f7fe ff3d 	bl	33c <pmu_reg_write>
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x02 << 24) | *REG0));
    #endif

    // Set the desired SAR Ratio
    pmu_reg_write (10, // 0x0A (SAR_RATIO_MINIMUM)
    14c2:	0029      	movs	r1, r5
    14c4:	200a      	movs	r0, #10
    14c6:	f7fe ff39 	bl	33c <pmu_reg_write>
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x03 << 24) | *REG0));
    #endif

    // Disable SAR_RESET override
    pmu_reg_write (5, // 0x05 (SAR_RATIO_OVERRIDE)
    14ca:	2180      	movs	r1, #128	; 0x80
    14cc:	2005      	movs	r0, #5
    14ce:	0189      	lsls	r1, r1, #6
    14d0:	f7fe ff34 	bl	33c <pmu_reg_write>
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x04 << 24) | *REG0));
    #endif

    // Run 'sar_ratio_adjusted'
    pmu_reg_write (60, // 0x3C (CTRL_DESIRED_STATE_ACTIVE)
    14d4:	490e      	ldr	r1, [pc, #56]	; (1510 <pmu_set_sar_ratio+0x7c>)
    14d6:	203c      	movs	r0, #60	; 0x3c
    14d8:	f7fe ff30 	bl	33c <pmu_reg_write>
        // Debug
        #ifdef DEBUG_SET_SAR_RATIO
            mbus_write_message32(0xD8, ((0x06 << 24) | *REG0));
        #endif

    } while (pmu_sar_ratio != sar_ratio);
    14dc:	267f      	movs	r6, #127	; 0x7f
uint32_t pmu_sar_ratio;
    // Now we don't know how long it would take to set the sar ratio.
    // so let's keep checking the actual sar ratio until it becomes as same as SAR_RATIO_MINIMUM
    do {
        // Read the current SAR RATIO
        pmu_reg_write(0x00,0x04);
    14de:	2104      	movs	r1, #4
    14e0:	2000      	movs	r0, #0
    14e2:	f7fe ff2b 	bl	33c <pmu_reg_write>
        pmu_sar_ratio   = *REG0 & 0x7F;
    14e6:	6823      	ldr	r3, [r4, #0]
        // Debug
        #ifdef DEBUG_SET_SAR_RATIO
            mbus_write_message32(0xD8, ((0x06 << 24) | *REG0));
        #endif

    } while (pmu_sar_ratio != sar_ratio);
    14e8:	4033      	ands	r3, r6
    14ea:	429d      	cmp	r5, r3
    14ec:	d1f7      	bne.n	14de <pmu_set_sar_ratio+0x4a>

    // Disable ADC & Auto Ratio Adjust
    pmu_reg_write (60, // 0x3C (CTRL_DESIRED_STATE_ACTIVE)
    14ee:	4907      	ldr	r1, [pc, #28]	; (150c <pmu_set_sar_ratio+0x78>)
    14f0:	203c      	movs	r0, #60	; 0x3c
    14f2:	f7fe ff23 	bl	33c <pmu_reg_write>
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x07 << 24) | *REG0));
    #endif

    // Override SAR_RESET again
    pmu_reg_write (5, // 0x05 (SAR_RATIO_OVERRIDE)
    14f6:	21a0      	movs	r1, #160	; 0xa0
    14f8:	2005      	movs	r0, #5
    14fa:	0189      	lsls	r1, r1, #6
    14fc:	f7fe ff1e 	bl	33c <pmu_reg_write>
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x08 << 24) | *REG0));
    #endif

    // Disable ADC override
    pmu_reg_write (2, // 0x02 (ADC_CONFIG_OVERRIDE)
    1500:	2100      	movs	r1, #0
    1502:	2002      	movs	r0, #2
    1504:	f7fe ff1a 	bl	33c <pmu_reg_write>
    // Debug
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x09 << 24) | *REG0));
    #endif

}
    1508:	bd70      	pop	{r4, r5, r6, pc}
    150a:	46c0      	nop			; (mov r8, r8)
    150c:	0007c7ff 	.word	0x0007c7ff
    1510:	0007efff 	.word	0x0007efff

Disassembly of section .text.pmu_setting_temp_based:

00001514 <pmu_setting_temp_based>:
}

// 0 : normal active
// 1 : radio active
// 2 : sleep
static void pmu_setting_temp_based(uint16_t mode) {
    1514:	2305      	movs	r3, #5
    1516:	b570      	push	{r4, r5, r6, lr}
#ifdef USE_PMU
    int8_t i;
    for(i = PMU_SETTINGS_LEN; i >= 0; i--) {
        // mbus_write_message32(0xB3, 0xFFFFFFFF);
        if(i == 0 || snt_sys_temp_code >= PMU_TEMP_THRESH[i - 1]) {
    1518:	4a12      	ldr	r2, [pc, #72]	; (1564 <pmu_setting_temp_based+0x50>)
    151a:	4913      	ldr	r1, [pc, #76]	; (1568 <pmu_setting_temp_based+0x54>)
    151c:	009d      	lsls	r5, r3, #2
    151e:	58ae      	ldr	r6, [r5, r2]
    1520:	680d      	ldr	r5, [r1, #0]
    1522:	1c5c      	adds	r4, r3, #1
    1524:	b264      	sxtb	r4, r4
    1526:	42ae      	cmp	r6, r5
    1528:	d902      	bls.n	1530 <pmu_setting_temp_based+0x1c>
    152a:	3b01      	subs	r3, #1
    152c:	d2f6      	bcs.n	151c <pmu_setting_temp_based+0x8>
    152e:	2400      	movs	r4, #0
    1530:	00a4      	lsls	r4, r4, #2
            if(mode == 0) {
    1532:	2800      	cmp	r0, #0
    1534:	d105      	bne.n	1542 <pmu_setting_temp_based+0x2e>
                pmu_set_active_clk(PMU_ACTIVE_SETTINGS[i]);
    1536:	4b0d      	ldr	r3, [pc, #52]	; (156c <pmu_setting_temp_based+0x58>)
    1538:	5918      	ldr	r0, [r3, r4]
    153a:	f7fe ff30 	bl	39e <pmu_set_active_clk>
                pmu_set_sar_ratio(PMU_ACTIVE_SAR_SETTINGS[i]);
    153e:	4b0c      	ldr	r3, [pc, #48]	; (1570 <pmu_setting_temp_based+0x5c>)
    1540:	e00c      	b.n	155c <pmu_setting_temp_based+0x48>
            }
            else if(mode == 2) {
    1542:	2802      	cmp	r0, #2
    1544:	d105      	bne.n	1552 <pmu_setting_temp_based+0x3e>
                pmu_set_sleep_clk(PMU_SLEEP_SETTINGS[i]);
    1546:	4b0b      	ldr	r3, [pc, #44]	; (1574 <pmu_setting_temp_based+0x60>)
    1548:	5918      	ldr	r0, [r3, r4]
    154a:	f7fe ff04 	bl	356 <pmu_set_sleep_clk>
                pmu_set_sar_ratio(PMU_SLEEP_SAR_SETTINGS[i]);
    154e:	4b0a      	ldr	r3, [pc, #40]	; (1578 <pmu_setting_temp_based+0x64>)
    1550:	e004      	b.n	155c <pmu_setting_temp_based+0x48>
            }
            else {
                pmu_set_active_clk(PMU_RADIO_SETTINGS[i]);
    1552:	4b0a      	ldr	r3, [pc, #40]	; (157c <pmu_setting_temp_based+0x68>)
    1554:	5918      	ldr	r0, [r3, r4]
    1556:	f7fe ff22 	bl	39e <pmu_set_active_clk>
                pmu_set_sar_ratio(PMU_RADIO_SAR_SETTINGS[i]);
    155a:	4b09      	ldr	r3, [pc, #36]	; (1580 <pmu_setting_temp_based+0x6c>)
    155c:	5918      	ldr	r0, [r3, r4]
    155e:	f7ff ff99 	bl	1494 <pmu_set_sar_ratio>
            break;
        }
    }
    return;
#endif
}
    1562:	bd70      	pop	{r4, r5, r6, pc}
    1564:	00003934 	.word	0x00003934
    1568:	000038b8 	.word	0x000038b8
    156c:	000037dc 	.word	0x000037dc
    1570:	000038c0 	.word	0x000038c0
    1574:	00003874 	.word	0x00003874
    1578:	00003838 	.word	0x00003838
    157c:	00003914 	.word	0x00003914
    1580:	0000395c 	.word	0x0000395c

Disassembly of section .text.set_system_error:

00001584 <set_system_error>:
    while(1);
}


void set_system_error(uint32_t code) {
    error_code = code;
    1584:	4b05      	ldr	r3, [pc, #20]	; (159c <set_system_error+0x18>)
    mbus_sleep_all();
    while(1);
}


void set_system_error(uint32_t code) {
    1586:	b510      	push	{r4, lr}
    error_code = code;
    1588:	6018      	str	r0, [r3, #0]
    error_time = xo_sys_time_in_sec;
    158a:	4b05      	ldr	r3, [pc, #20]	; (15a0 <set_system_error+0x1c>)
    mbus_sleep_all();
    while(1);
}


void set_system_error(uint32_t code) {
    158c:	0001      	movs	r1, r0
    error_code = code;
    error_time = xo_sys_time_in_sec;
    158e:	681a      	ldr	r2, [r3, #0]
    1590:	4b04      	ldr	r3, [pc, #16]	; (15a4 <set_system_error+0x20>)
    mbus_write_message32(0xBF, code);
    1592:	20bf      	movs	r0, #191	; 0xbf
}


void set_system_error(uint32_t code) {
    error_code = code;
    error_time = xo_sys_time_in_sec;
    1594:	601a      	str	r2, [r3, #0]
    mbus_write_message32(0xBF, code);
    1596:	f7fe fd87 	bl	a8 <mbus_write_message32>
}
    159a:	bd10      	pop	{r4, pc}
    159c:	00003b14 	.word	0x00003b14
    15a0:	00003acc 	.word	0x00003acc
    15a4:	00003a9c 	.word	0x00003a9c

Disassembly of section .text.update_system_time:

000015a8 <update_system_time>:

#define XO_MAX_DAY_TIME_IN_SEC 86400
#define XO_TO_SEC_SHIFT 15
#define MAX_UPDATE_TIME 3600    // v5.2.9: Upper bounds on time difference between two updates

void update_system_time() {
    15a8:	b570      	push	{r4, r5, r6, lr}
    // mbus_write_message32(0xC2, xo_sys_time);
    // mbus_write_message32(0xC1, xo_sys_time_in_sec);
    // mbus_write_message32(0xC0, xo_day_time_in_sec);

    // FIXME: global time doesn't reset bug
    uint32_t last_update_val = xo_sys_time;
    15aa:	4d1a      	ldr	r5, [pc, #104]	; (1614 <update_system_time+0x6c>)
    15ac:	682e      	ldr	r6, [r5, #0]
    xo_sys_time = get_timer_cnt_xo();
    15ae:	f7ff f935 	bl	81c <get_timer_cnt_xo>

    // check if XO clock has stopped
    if(xo_sys_time == last_xo_sys_time) {
    15b2:	4b19      	ldr	r3, [pc, #100]	; (1618 <update_system_time+0x70>)
    // mbus_write_message32(0xC1, xo_sys_time_in_sec);
    // mbus_write_message32(0xC0, xo_day_time_in_sec);

    // FIXME: global time doesn't reset bug
    uint32_t last_update_val = xo_sys_time;
    xo_sys_time = get_timer_cnt_xo();
    15b4:	6028      	str	r0, [r5, #0]

    // check if XO clock has stopped
    if(xo_sys_time == last_xo_sys_time) {
    15b6:	682a      	ldr	r2, [r5, #0]
    15b8:	681b      	ldr	r3, [r3, #0]
    15ba:	429a      	cmp	r2, r3
    15bc:	d102      	bne.n	15c4 <update_system_time+0x1c>
        set_system_error(0x3);
    15be:	2003      	movs	r0, #3
    15c0:	f7ff ffe0 	bl	1584 <set_system_error>
    }

    // calculate the difference in seconds
    uint32_t diff = (xo_sys_time >> XO_TO_SEC_SHIFT) - (last_update_val >> XO_TO_SEC_SHIFT); // Have to shift individually to account for underflow
    15c4:	682c      	ldr	r4, [r5, #0]
    15c6:	0bf3      	lsrs	r3, r6, #15
    15c8:	0be4      	lsrs	r4, r4, #15
    15ca:	1ae4      	subs	r4, r4, r3

    if(xo_sys_time < last_update_val) {
    15cc:	682b      	ldr	r3, [r5, #0]
    15ce:	429e      	cmp	r6, r3
    15d0:	d902      	bls.n	15d8 <update_system_time+0x30>
        // xo timer overflowed, want to make sure that 0x00000 - 0x1FFFF = 1
        diff += 0x20000;
    15d2:	2380      	movs	r3, #128	; 0x80
    15d4:	029b      	lsls	r3, r3, #10
    15d6:	18e4      	adds	r4, r4, r3
    }

    // v5.2.9
    if(diff > MAX_UPDATE_TIME) {
    15d8:	23e1      	movs	r3, #225	; 0xe1
    15da:	011b      	lsls	r3, r3, #4
    15dc:	429c      	cmp	r4, r3
    15de:	d902      	bls.n	15e6 <update_system_time+0x3e>
        set_system_error(0x4);
    15e0:	2004      	movs	r0, #4
    15e2:	f7ff ffcf 	bl	1584 <set_system_error>
    }

    xo_sys_time_in_sec += diff;
    15e6:	4a0d      	ldr	r2, [pc, #52]	; (161c <update_system_time+0x74>)
    15e8:	6813      	ldr	r3, [r2, #0]
    15ea:	18e3      	adds	r3, r4, r3
    15ec:	6013      	str	r3, [r2, #0]
    xo_day_time_in_sec += diff;
    15ee:	4b0c      	ldr	r3, [pc, #48]	; (1620 <update_system_time+0x78>)
    15f0:	681a      	ldr	r2, [r3, #0]
    15f2:	18a4      	adds	r4, r4, r2
    15f4:	601c      	str	r4, [r3, #0]

    if(xo_day_time_in_sec >= XO_MAX_DAY_TIME_IN_SEC) {
    15f6:	6819      	ldr	r1, [r3, #0]
    15f8:	4a0a      	ldr	r2, [pc, #40]	; (1624 <update_system_time+0x7c>)
    15fa:	4291      	cmp	r1, r2
    15fc:	d908      	bls.n	1610 <update_system_time+0x68>
        xo_day_time_in_sec -= XO_MAX_DAY_TIME_IN_SEC;
    15fe:	681a      	ldr	r2, [r3, #0]
    1600:	4909      	ldr	r1, [pc, #36]	; (1628 <update_system_time+0x80>)
    1602:	1852      	adds	r2, r2, r1
    1604:	601a      	str	r2, [r3, #0]
        day_count++;
    1606:	4a09      	ldr	r2, [pc, #36]	; (162c <update_system_time+0x84>)
    1608:	8813      	ldrh	r3, [r2, #0]
    160a:	3301      	adds	r3, #1
    160c:	b29b      	uxth	r3, r3
    160e:	8013      	strh	r3, [r2, #0]
    }

    // mbus_write_message32(0xC2, xo_sys_time);
    // mbus_write_message32(0xC1, xo_sys_time_in_sec);
    // mbus_write_message32(0xC0, xo_day_time_in_sec);
}
    1610:	bd70      	pop	{r4, r5, r6, pc}
    1612:	46c0      	nop			; (mov r8, r8)
    1614:	00003aec 	.word	0x00003aec
    1618:	00003afc 	.word	0x00003afc
    161c:	00003acc 	.word	0x00003acc
    1620:	00003a78 	.word	0x00003a78
    1624:	0001517f 	.word	0x0001517f
    1628:	fffeae80 	.word	0xfffeae80
    162c:	00003a3a 	.word	0x00003a3a

Disassembly of section .text.xo_check_is_day:

00001630 <xo_check_is_day>:
        }
    }
    return res;
}

bool xo_check_is_day() {
    1630:	b510      	push	{r4, lr}
    update_system_time();
    1632:	f7ff ffb9 	bl	15a8 <update_system_time>
    return (DAY_START_TIME <= xo_day_time_in_sec) && (xo_day_time_in_sec < DAY_END_TIME);
    1636:	4b07      	ldr	r3, [pc, #28]	; (1654 <xo_check_is_day+0x24>)
    1638:	2000      	movs	r0, #0
    163a:	6819      	ldr	r1, [r3, #0]
    163c:	4b06      	ldr	r3, [pc, #24]	; (1658 <xo_check_is_day+0x28>)
    163e:	681a      	ldr	r2, [r3, #0]
    1640:	4291      	cmp	r1, r2
    1642:	d805      	bhi.n	1650 <xo_check_is_day+0x20>
    1644:	6818      	ldr	r0, [r3, #0]
    1646:	4b05      	ldr	r3, [pc, #20]	; (165c <xo_check_is_day+0x2c>)
    1648:	681b      	ldr	r3, [r3, #0]
    164a:	4298      	cmp	r0, r3
    164c:	4180      	sbcs	r0, r0
    164e:	4240      	negs	r0, r0
}
    1650:	bd10      	pop	{r4, pc}
    1652:	46c0      	nop			; (mov r8, r8)
    1654:	00003980 	.word	0x00003980
    1658:	00003a78 	.word	0x00003a78
    165c:	00003818 	.word	0x00003818

Disassembly of section .text.mrr_send_radio_data:

00001660 <mrr_send_radio_data>:
}

uint16_t mrr_cfo_val_fine = 0;
uint16_t mrr_trx_cap_antn_tune_coarse = 0;

static void mrr_send_radio_data(uint16_t last_packet) {
    1660:	b5f0      	push	{r4, r5, r6, r7, lr}
    //     return;
    // }


    // clean up radio_data_arr[2]
    radio_data_arr[2] = radio_data_arr[2] & 0x0000FFFF;
    1662:	4fb7      	ldr	r7, [pc, #732]	; (1940 <mrr_send_radio_data+0x2e0>)
}

uint16_t mrr_cfo_val_fine = 0;
uint16_t mrr_trx_cap_antn_tune_coarse = 0;

static void mrr_send_radio_data(uint16_t last_packet) {
    1664:	b085      	sub	sp, #20
    //     return;
    // }


    // clean up radio_data_arr[2]
    radio_data_arr[2] = radio_data_arr[2] & 0x0000FFFF;
    1666:	68bb      	ldr	r3, [r7, #8]
}

uint16_t mrr_cfo_val_fine = 0;
uint16_t mrr_trx_cap_antn_tune_coarse = 0;

static void mrr_send_radio_data(uint16_t last_packet) {
    1668:	9003      	str	r0, [sp, #12]
    //     return;
    // }


    // clean up radio_data_arr[2]
    radio_data_arr[2] = radio_data_arr[2] & 0x0000FFFF;
    166a:	b29b      	uxth	r3, r3
    166c:	60bb      	str	r3, [r7, #8]
    mbus_write_message32(0xAA, 0x00000000);
    mbus_write_message32(0xAA, 0xAAAAAAAA);
#endif

    // CRC16 Encoding 
    uint32_t* crc_data = crcEnc16();
    166e:	f7ff fecd 	bl	140c <crcEnc16>
    // mbus_write_message32(0xBB, 0xBBBBBBBB);
#endif

    // TODO: add temp and voltage restrictions

    if(!radio_on) {
    1672:	4bb4      	ldr	r3, [pc, #720]	; (1944 <mrr_send_radio_data+0x2e4>)
    mbus_write_message32(0xAA, 0x00000000);
    mbus_write_message32(0xAA, 0xAAAAAAAA);
#endif

    // CRC16 Encoding 
    uint32_t* crc_data = crcEnc16();
    1674:	9002      	str	r0, [sp, #8]
    // mbus_write_message32(0xBB, 0xBBBBBBBB);
#endif

    // TODO: add temp and voltage restrictions

    if(!radio_on) {
    1676:	881e      	ldrh	r6, [r3, #0]
    1678:	b2b2      	uxth	r2, r6
    167a:	9201      	str	r2, [sp, #4]
    167c:	2a00      	cmp	r2, #0
    167e:	d000      	beq.n	1682 <mrr_send_radio_data+0x22>
    1680:	e095      	b.n	17ae <mrr_send_radio_data+0x14e>
        radio_on = 1;
    1682:	2601      	movs	r6, #1

    // Need to speed up sleep pmu clock
    //pmu_set_sleep_radio();

    // Turn off Current Limter Briefly
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
    1684:	4db0      	ldr	r5, [pc, #704]	; (1948 <mrr_send_radio_data+0x2e8>)
#endif

    // TODO: add temp and voltage restrictions

    if(!radio_on) {
        radio_on = 1;
    1686:	801e      	strh	r6, [r3, #0]

    // Need to speed up sleep pmu clock
    //pmu_set_sleep_radio();

    // Turn off Current Limter Briefly
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
    1688:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    168a:	2002      	movs	r0, #2

    // Need to speed up sleep pmu clock
    //pmu_set_sleep_radio();

    // Turn off Current Limter Briefly
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
    168c:	43b3      	bics	r3, r6
    168e:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1690:	682a      	ldr	r2, [r5, #0]
    1692:	9901      	ldr	r1, [sp, #4]
    1694:	f7fe fd47 	bl	126 <mbus_remote_register_write>

    // Set decap to parallel
    mrrv11a_r03.TRX_DCP_S_OW1 = 0;  //TX_Decap S (forced charge decaps)
    1698:	4cac      	ldr	r4, [pc, #688]	; (194c <mrr_send_radio_data+0x2ec>)
    169a:	4bad      	ldr	r3, [pc, #692]	; (1950 <mrr_send_radio_data+0x2f0>)
    169c:	6822      	ldr	r2, [r4, #0]
    mrrv11a_r03.TRX_DCP_S_OW2 = 0;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    169e:	2103      	movs	r1, #3
    // Turn off Current Limter Briefly
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Set decap to parallel
    mrrv11a_r03.TRX_DCP_S_OW1 = 0;  //TX_Decap S (forced charge decaps)
    16a0:	4013      	ands	r3, r2
    16a2:	6023      	str	r3, [r4, #0]
    mrrv11a_r03.TRX_DCP_S_OW2 = 0;  //TX_Decap S (forced charge decaps)
    16a4:	6822      	ldr	r2, [r4, #0]
    16a6:	4bab      	ldr	r3, [pc, #684]	; (1954 <mrr_send_radio_data+0x2f4>)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    16a8:	2002      	movs	r0, #2
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Set decap to parallel
    mrrv11a_r03.TRX_DCP_S_OW1 = 0;  //TX_Decap S (forced charge decaps)
    mrrv11a_r03.TRX_DCP_S_OW2 = 0;  //TX_Decap S (forced charge decaps)
    16aa:	4013      	ands	r3, r2
    16ac:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    16ae:	6822      	ldr	r2, [r4, #0]
    16b0:	f7fe fd39 	bl	126 <mbus_remote_register_write>
    mrrv11a_r03.TRX_DCP_P_OW1 = 1;  //RX_Decap P 
    16b4:	2280      	movs	r2, #128	; 0x80
    16b6:	6823      	ldr	r3, [r4, #0]
    16b8:	0392      	lsls	r2, r2, #14
    16ba:	4313      	orrs	r3, r2
    mrrv11a_r03.TRX_DCP_P_OW2 = 1;  //RX_Decap P 
    16bc:	2280      	movs	r2, #128	; 0x80

    // Set decap to parallel
    mrrv11a_r03.TRX_DCP_S_OW1 = 0;  //TX_Decap S (forced charge decaps)
    mrrv11a_r03.TRX_DCP_S_OW2 = 0;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    mrrv11a_r03.TRX_DCP_P_OW1 = 1;  //RX_Decap P 
    16be:	6023      	str	r3, [r4, #0]
    mrrv11a_r03.TRX_DCP_P_OW2 = 1;  //RX_Decap P 
    16c0:	6823      	ldr	r3, [r4, #0]
    16c2:	0352      	lsls	r2, r2, #13
    16c4:	4313      	orrs	r3, r2
    16c6:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    16c8:	6822      	ldr	r2, [r4, #0]
    16ca:	2103      	movs	r1, #3
    16cc:	2002      	movs	r0, #2
    16ce:	f7fe fd2a 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY);
    16d2:	2064      	movs	r0, #100	; 0x64
    16d4:	f7fe fd64 	bl	1a0 <delay>

    // Set decap to series
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    16d8:	6822      	ldr	r2, [r4, #0]
    16da:	4b9f      	ldr	r3, [pc, #636]	; (1958 <mrr_send_radio_data+0x2f8>)
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    16dc:	2103      	movs	r1, #3
    mrrv11a_r03.TRX_DCP_P_OW2 = 1;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    delay(MBUS_DELAY);

    // Set decap to series
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    16de:	4013      	ands	r3, r2
    16e0:	6023      	str	r3, [r4, #0]
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    16e2:	6822      	ldr	r2, [r4, #0]
    16e4:	4b9d      	ldr	r3, [pc, #628]	; (195c <mrr_send_radio_data+0x2fc>)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    16e6:	2002      	movs	r0, #2
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    delay(MBUS_DELAY);

    // Set decap to series
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    16e8:	4013      	ands	r3, r2
    16ea:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    16ec:	6822      	ldr	r2, [r4, #0]
    16ee:	f7fe fd1a 	bl	126 <mbus_remote_register_write>
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
    16f2:	2280      	movs	r2, #128	; 0x80
    16f4:	6823      	ldr	r3, [r4, #0]
    16f6:	0312      	lsls	r2, r2, #12
    16f8:	4313      	orrs	r3, r2
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    16fa:	2280      	movs	r2, #128	; 0x80

    // Set decap to series
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
    16fc:	6023      	str	r3, [r4, #0]
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    16fe:	6823      	ldr	r3, [r4, #0]
    1700:	02d2      	lsls	r2, r2, #11
    1702:	4313      	orrs	r3, r2
    1704:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    1706:	6822      	ldr	r2, [r4, #0]
    1708:	2103      	movs	r1, #3
    170a:	2002      	movs	r0, #2
    170c:	f7fe fd0b 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY);
    1710:	2064      	movs	r0, #100	; 0x64
    1712:	f7fe fd45 	bl	1a0 <delay>

    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    1716:	227e      	movs	r2, #126	; 0x7e
    1718:	2420      	movs	r4, #32
    171a:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    171c:	2002      	movs	r0, #2
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    delay(MBUS_DELAY);

    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    171e:	4393      	bics	r3, r2
    1720:	4323      	orrs	r3, r4
    1722:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1724:	682a      	ldr	r2, [r5, #0]
    1726:	9901      	ldr	r1, [sp, #4]
    1728:	f7fe fcfd 	bl	126 <mbus_remote_register_write>

    // New for MRRv11A
//remove?     mrr_ldo_power_on();
    
    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    172c:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    172e:	2002      	movs	r0, #2

    // New for MRRv11A
//remove?     mrr_ldo_power_on();
    
    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    1730:	4333      	orrs	r3, r6
    1732:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1734:	682a      	ldr	r2, [r5, #0]
    1736:	9901      	ldr	r1, [sp, #4]
    1738:	f7fe fcf5 	bl	126 <mbus_remote_register_write>

    // Release timer power-gate
    mrrv11a_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    173c:	4d88      	ldr	r5, [pc, #544]	; (1960 <mrr_send_radio_data+0x300>)
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    173e:	2104      	movs	r1, #4
    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Release timer power-gate
    mrrv11a_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    1740:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    1742:	2002      	movs	r0, #2
    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Release timer power-gate
    mrrv11a_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    1744:	4333      	orrs	r3, r6
    1746:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    1748:	682a      	ldr	r2, [r5, #0]
    174a:	f7fe fcec 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY);
    174e:	2064      	movs	r0, #100	; 0x64
    1750:	f7fe fd26 	bl	1a0 <delay>

    // Turn on timer
    mrrv11a_r04.RO_RESET = 0;  //Release Reset TIMER
    1754:	2208      	movs	r2, #8
    1756:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    1758:	2104      	movs	r1, #4
    mrrv11a_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    delay(MBUS_DELAY);

    // Turn on timer
    mrrv11a_r04.RO_RESET = 0;  //Release Reset TIMER
    175a:	4393      	bics	r3, r2
    175c:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    175e:	682a      	ldr	r2, [r5, #0]
    1760:	2002      	movs	r0, #2
    1762:	f7fe fce0 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY);
    1766:	2064      	movs	r0, #100	; 0x64
    1768:	f7fe fd1a 	bl	1a0 <delay>

    mrrv11a_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    176c:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    176e:	2104      	movs	r1, #4
    // Turn on timer
    mrrv11a_r04.RO_RESET = 0;  //Release Reset TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    delay(MBUS_DELAY);

    mrrv11a_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    1770:	431c      	orrs	r4, r3
    1772:	602c      	str	r4, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    1774:	682a      	ldr	r2, [r5, #0]
    1776:	2002      	movs	r0, #2
    1778:	f7fe fcd5 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY);
    177c:	2064      	movs	r0, #100	; 0x64
    177e:	f7fe fd0f 	bl	1a0 <delay>

    mrrv11a_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    1782:	2210      	movs	r2, #16
    1784:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    1786:	2104      	movs	r1, #4

    mrrv11a_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    delay(MBUS_DELAY);

    mrrv11a_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    1788:	4393      	bics	r3, r2
    178a:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    178c:	682a      	ldr	r2, [r5, #0]
    178e:	2002      	movs	r0, #2
    1790:	f7fe fcc9 	bl	126 <mbus_remote_register_write>

    // Release FSM Sleep
    mrrv11a_r11.FSM_SLEEP = 0;  // Power on BB
    1794:	4b73      	ldr	r3, [pc, #460]	; (1964 <mrr_send_radio_data+0x304>)
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1796:	2002      	movs	r0, #2

    mrrv11a_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);

    // Release FSM Sleep
    mrrv11a_r11.FSM_SLEEP = 0;  // Power on BB
    1798:	681a      	ldr	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    179a:	2111      	movs	r1, #17

    mrrv11a_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);

    // Release FSM Sleep
    mrrv11a_r11.FSM_SLEEP = 0;  // Power on BB
    179c:	43b2      	bics	r2, r6
    179e:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    17a0:	681a      	ldr	r2, [r3, #0]
    17a2:	f7fe fcc0 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*5); // Freq stab
    17a6:	20fa      	movs	r0, #250	; 0xfa
    17a8:	0040      	lsls	r0, r0, #1
    17aa:	f7fe fcf9 	bl	1a0 <delay>
        radio_on = 1;
    radio_power_on();
    }
    

    radio_data_arr[2] = crc_data[0] << 16 | radio_data_arr[2];
    17ae:	9a02      	ldr	r2, [sp, #8]
    17b0:	68bb      	ldr	r3, [r7, #8]
    17b2:	6812      	ldr	r2, [r2, #0]
    mbus_copy_mem_from_local_to_remote_bulk(MRR_ADDR, (uint32_t *) 0x00000000, (uint32_t *) radio_data_arr, 2);
    17b4:	2100      	movs	r1, #0
        radio_on = 1;
    radio_power_on();
    }
    

    radio_data_arr[2] = crc_data[0] << 16 | radio_data_arr[2];
    17b6:	0412      	lsls	r2, r2, #16
    17b8:	4313      	orrs	r3, r2
    17ba:	60bb      	str	r3, [r7, #8]
    mbus_copy_mem_from_local_to_remote_bulk(MRR_ADDR, (uint32_t *) 0x00000000, (uint32_t *) radio_data_arr, 2);
    17bc:	2302      	movs	r3, #2
    17be:	003a      	movs	r2, r7
    17c0:	0018      	movs	r0, r3
    17c2:	f7fe fcbd 	bl	140 <mbus_copy_mem_from_local_to_remote_bulk>

    if (!radio_ready){
    17c6:	4b68      	ldr	r3, [pc, #416]	; (1968 <mrr_send_radio_data+0x308>)
    17c8:	881a      	ldrh	r2, [r3, #0]
    17ca:	2a00      	cmp	r2, #0
    17cc:	d10f      	bne.n	17ee <mrr_send_radio_data+0x18e>
        // Release FSM Reset
        //mrrv11a_r11.MRR_RAD_FSM_RSTN = 1;  //UNRST BB
        //mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
        //delay(MBUS_DELAY);

        mrrv11a_r03.TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    17ce:	2180      	movs	r1, #128	; 0x80

    radio_data_arr[2] = crc_data[0] << 16 | radio_data_arr[2];
    mbus_copy_mem_from_local_to_remote_bulk(MRR_ADDR, (uint32_t *) 0x00000000, (uint32_t *) radio_data_arr, 2);

    if (!radio_ready){
        radio_ready = 1;
    17d0:	3201      	adds	r2, #1
    17d2:	801a      	strh	r2, [r3, #0]
        // Release FSM Reset
        //mrrv11a_r11.MRR_RAD_FSM_RSTN = 1;  //UNRST BB
        //mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
        //delay(MBUS_DELAY);

        mrrv11a_r03.TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    17d4:	4b5d      	ldr	r3, [pc, #372]	; (194c <mrr_send_radio_data+0x2ec>)
    17d6:	03c9      	lsls	r1, r1, #15
    17d8:	681a      	ldr	r2, [r3, #0]
        mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    17da:	2002      	movs	r0, #2
        // Release FSM Reset
        //mrrv11a_r11.MRR_RAD_FSM_RSTN = 1;  //UNRST BB
        //mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
        //delay(MBUS_DELAY);

        mrrv11a_r03.TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    17dc:	430a      	orrs	r2, r1
    17de:	601a      	str	r2, [r3, #0]
        mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    17e0:	681a      	ldr	r2, [r3, #0]
    17e2:	2103      	movs	r1, #3
    17e4:	f7fe fc9f 	bl	126 <mbus_remote_register_write>
        delay(MBUS_DELAY);
    17e8:	2064      	movs	r0, #100	; 0x64
    17ea:	f7fe fcd9 	bl	1a0 <delay>
    }
        
    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    17ee:	217e      	movs	r1, #126	; 0x7e
    17f0:	2002      	movs	r0, #2
    17f2:	4a55      	ldr	r2, [pc, #340]	; (1948 <mrr_send_radio_data+0x2e8>)
    17f4:	6813      	ldr	r3, [r2, #0]
    17f6:	438b      	bics	r3, r1
    17f8:	4303      	orrs	r3, r0
    17fa:	6013      	str	r3, [r2, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    17fc:	6812      	ldr	r2, [r2, #0]
    17fe:	2100      	movs	r1, #0
    1800:	f7fe fc91 	bl	126 <mbus_remote_register_write>

#ifdef USE_RAD
    // uint16_t mrr_cfo_val_fine = 0;
    uint16_t count = 0;
    1804:	2300      	movs	r3, #0
    1806:	9302      	str	r3, [sp, #8]
    // uint16_t num_packets = 1;
    // if (mrr_freq_hopping) num_packets = mrr_freq_hopping;

    // mrr_cfo_val_fine = 0x0000;

    while (count < mrr_freq_hopping){
    1808:	4b58      	ldr	r3, [pc, #352]	; (196c <mrr_send_radio_data+0x30c>)
    180a:	9a02      	ldr	r2, [sp, #8]
    180c:	881b      	ldrh	r3, [r3, #0]
    180e:	429a      	cmp	r2, r3
    1810:	d300      	bcc.n	1814 <mrr_send_radio_data+0x1b4>
    1812:	e087      	b.n	1924 <mrr_send_radio_data+0x2c4>
    count++;
    // may be able to remove 2 lines below, GC 1/6/20
        // FIXME: We should probably turn WD back on after sending
    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    1814:	2600      	movs	r6, #0
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer

    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
    1816:	213f      	movs	r1, #63	; 0x3f
    // if (mrr_freq_hopping) num_packets = mrr_freq_hopping;

    // mrr_cfo_val_fine = 0x0000;

    while (count < mrr_freq_hopping){
    count++;
    1818:	9b02      	ldr	r3, [sp, #8]
    // may be able to remove 2 lines below, GC 1/6/20
        // FIXME: We should probably turn WD back on after sending
    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer

    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
    181a:	4a55      	ldr	r2, [pc, #340]	; (1970 <mrr_send_radio_data+0x310>)
    // if (mrr_freq_hopping) num_packets = mrr_freq_hopping;

    // mrr_cfo_val_fine = 0x0000;

    while (count < mrr_freq_hopping){
    count++;
    181c:	3301      	adds	r3, #1
    181e:	b29b      	uxth	r3, r3
    1820:	9302      	str	r3, [sp, #8]
    // may be able to remove 2 lines below, GC 1/6/20
        // FIXME: We should probably turn WD back on after sending
    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    1822:	4b54      	ldr	r3, [pc, #336]	; (1974 <mrr_send_radio_data+0x314>)
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer

    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
    1824:	4854      	ldr	r0, [pc, #336]	; (1978 <mrr_send_radio_data+0x318>)

    while (count < mrr_freq_hopping){
    count++;
    // may be able to remove 2 lines below, GC 1/6/20
        // FIXME: We should probably turn WD back on after sending
    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    1826:	601e      	str	r6, [r3, #0]
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    1828:	4b54      	ldr	r3, [pc, #336]	; (197c <mrr_send_radio_data+0x31c>)
    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Fire off data
    mrrv11a_r11.FSM_RESET_B = 1;    
    182a:	2502      	movs	r5, #2
    while (count < mrr_freq_hopping){
    count++;
    // may be able to remove 2 lines below, GC 1/6/20
        // FIXME: We should probably turn WD back on after sending
    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    182c:	601e      	str	r6, [r3, #0]

    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
    182e:	4b54      	ldr	r3, [pc, #336]	; (1980 <mrr_send_radio_data+0x320>)
    1830:	6814      	ldr	r4, [r2, #0]
    1832:	881b      	ldrh	r3, [r3, #0]
    1834:	4020      	ands	r0, r4
    1836:	400b      	ands	r3, r1
    1838:	0419      	lsls	r1, r3, #16
    183a:	4301      	orrs	r1, r0
    183c:	6011      	str	r1, [r2, #0]
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine;
    183e:	6810      	ldr	r0, [r2, #0]
    1840:	4950      	ldr	r1, [pc, #320]	; (1984 <mrr_send_radio_data+0x324>)
    1842:	029b      	lsls	r3, r3, #10
    1844:	4001      	ands	r1, r0
    1846:	430b      	orrs	r3, r1
    1848:	6013      	str	r3, [r2, #0]
    mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
    184a:	6812      	ldr	r2, [r2, #0]
    184c:	2101      	movs	r1, #1
    184e:	2002      	movs	r0, #2
    1850:	f7fe fc69 	bl	126 <mbus_remote_register_write>
 **********************************************/

static void set_timer32_timeout(uint32_t val){
    // Use Timer32 as timeout counter
    wfi_timeout_flag = 0;
    config_timer32(val, 1, 0, 0);
    1854:	20a0      	movs	r0, #160	; 0xa0
 * Timeout Functions
 **********************************************/

static void set_timer32_timeout(uint32_t val){
    // Use Timer32 as timeout counter
    wfi_timeout_flag = 0;
    1856:	4b4c      	ldr	r3, [pc, #304]	; (1988 <mrr_send_radio_data+0x328>)
    config_timer32(val, 1, 0, 0);
    1858:	0032      	movs	r2, r6
 * Timeout Functions
 **********************************************/

static void set_timer32_timeout(uint32_t val){
    // Use Timer32 as timeout counter
    wfi_timeout_flag = 0;
    185a:	801e      	strh	r6, [r3, #0]
    config_timer32(val, 1, 0, 0);
    185c:	2101      	movs	r1, #1
    185e:	0033      	movs	r3, r6
    1860:	0300      	lsls	r0, r0, #12
    1862:	f7fe fca7 	bl	1b4 <config_timer32>

    // Use timer32 as timeout counter
    set_timer32_timeout(TIMER32_VAL);

    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;
    1866:	2201      	movs	r2, #1
    1868:	4f37      	ldr	r7, [pc, #220]	; (1948 <mrr_send_radio_data+0x2e8>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    186a:	0031      	movs	r1, r6

    // Use timer32 as timeout counter
    set_timer32_timeout(TIMER32_VAL);

    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;
    186c:	683b      	ldr	r3, [r7, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    186e:	2002      	movs	r0, #2

    // Use timer32 as timeout counter
    set_timer32_timeout(TIMER32_VAL);

    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;
    1870:	4313      	orrs	r3, r2
    1872:	603b      	str	r3, [r7, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1874:	683a      	ldr	r2, [r7, #0]
    1876:	f7fe fc56 	bl	126 <mbus_remote_register_write>

    // Fire off data
    mrrv11a_r11.FSM_RESET_B = 1;    
    187a:	4c3a      	ldr	r4, [pc, #232]	; (1964 <mrr_send_radio_data+0x304>)
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);    
    187c:	2111      	movs	r1, #17
    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Fire off data
    mrrv11a_r11.FSM_RESET_B = 1;    
    187e:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);    
    1880:	0028      	movs	r0, r5
    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Fire off data
    mrrv11a_r11.FSM_RESET_B = 1;    
    1882:	432b      	orrs	r3, r5
    1884:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);    
    1886:	6822      	ldr	r2, [r4, #0]
    1888:	f7fe fc4d 	bl	126 <mbus_remote_register_write>
    mrrv11a_r11.FSM_EN = 1;  //Start BB
    188c:	2204      	movs	r2, #4
    188e:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1890:	2111      	movs	r1, #17
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Fire off data
    mrrv11a_r11.FSM_RESET_B = 1;    
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);    
    mrrv11a_r11.FSM_EN = 1;  //Start BB
    1892:	4313      	orrs	r3, r2
    1894:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1896:	6822      	ldr	r2, [r4, #0]
    1898:	0028      	movs	r0, r5
    189a:	f7fe fc44 	bl	126 <mbus_remote_register_write>

    // Wait for radio response
    WFI();
    189e:	f7fe fc86 	bl	1ae <WFI>
    config_timer32(val, 1, 0, 0);
}

static void stop_timer32_timeout_check(){
    // Turn off Timer32
    *TIMER32_GO = 0;
    18a2:	4b3a      	ldr	r3, [pc, #232]	; (198c <mrr_send_radio_data+0x32c>)
    18a4:	601e      	str	r6, [r3, #0]
    if (wfi_timeout_flag){
    18a6:	4b38      	ldr	r3, [pc, #224]	; (1988 <mrr_send_radio_data+0x328>)
    18a8:	881b      	ldrh	r3, [r3, #0]
    18aa:	b29b      	uxth	r3, r3
    18ac:	9301      	str	r3, [sp, #4]
    18ae:	42b3      	cmp	r3, r6
    18b0:	d007      	beq.n	18c2 <mrr_send_radio_data+0x262>
    mbus_write_message32(0xBF, code);
}

static void sys_err(uint32_t code)
{
    set_system_error(code);
    18b2:	2080      	movs	r0, #128	; 0x80

static void stop_timer32_timeout_check(){
    // Turn off Timer32
    *TIMER32_GO = 0;
    if (wfi_timeout_flag){
        wfi_timeout_flag = 0;
    18b4:	4b34      	ldr	r3, [pc, #208]	; (1988 <mrr_send_radio_data+0x328>)
    mbus_write_message32(0xBF, code);
}

static void sys_err(uint32_t code)
{
    set_system_error(code);
    18b6:	04c0      	lsls	r0, r0, #19

static void stop_timer32_timeout_check(){
    // Turn off Timer32
    *TIMER32_GO = 0;
    if (wfi_timeout_flag){
        wfi_timeout_flag = 0;
    18b8:	801e      	strh	r6, [r3, #0]
    mbus_write_message32(0xBF, code);
}

static void sys_err(uint32_t code)
{
    set_system_error(code);
    18ba:	f7ff fe63 	bl	1584 <set_system_error>

    // mbus_write_message32(0xAF, code);
    operation_sleep();
    18be:	f7fe fe29 	bl	514 <operation_sleep>
    // Wait for radio response
    WFI();
    stop_timer32_timeout_check();

    // Turn off Current Limter
    mrrv11a_r00.TRX_CL_EN = 0;
    18c2:	2201      	movs	r2, #1
    18c4:	683b      	ldr	r3, [r7, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    18c6:	0028      	movs	r0, r5
    // Wait for radio response
    WFI();
    stop_timer32_timeout_check();

    // Turn off Current Limter
    mrrv11a_r00.TRX_CL_EN = 0;
    18c8:	4393      	bics	r3, r2
    18ca:	603b      	str	r3, [r7, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    18cc:	683a      	ldr	r2, [r7, #0]
    18ce:	9901      	ldr	r1, [sp, #4]
    18d0:	f7fe fc29 	bl	126 <mbus_remote_register_write>

    mrrv11a_r11.FSM_EN = 0;
    18d4:	2204      	movs	r2, #4
    18d6:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    18d8:	2111      	movs	r1, #17

    // Turn off Current Limter
    mrrv11a_r00.TRX_CL_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    mrrv11a_r11.FSM_EN = 0;
    18da:	4393      	bics	r3, r2
    18dc:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    18de:	6822      	ldr	r2, [r4, #0]
    18e0:	0028      	movs	r0, r5
    18e2:	f7fe fc20 	bl	126 <mbus_remote_register_write>
    mrrv11a_r11.FSM_RESET_B = 0;
    18e6:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    18e8:	2111      	movs	r1, #17
    mrrv11a_r00.TRX_CL_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    mrrv11a_r11.FSM_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    mrrv11a_r11.FSM_RESET_B = 0;
    18ea:	43ab      	bics	r3, r5
    18ec:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    18ee:	6822      	ldr	r2, [r4, #0]
    18f0:	0028      	movs	r0, r5
    18f2:	f7fe fc18 	bl	126 <mbus_remote_register_write>
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine;
    mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
    send_radio_data_mrr_sub1();

    // v6.3.0: switch this out for timer32
    config_timer32(RADIO_PACKET_DELAY, 1, 0, 0);
    18f6:	9b01      	ldr	r3, [sp, #4]
    18f8:	4825      	ldr	r0, [pc, #148]	; (1990 <mrr_send_radio_data+0x330>)
    18fa:	001a      	movs	r2, r3
    18fc:	2101      	movs	r1, #1
    18fe:	6800      	ldr	r0, [r0, #0]
    1900:	f7fe fc58 	bl	1b4 <config_timer32>
    WFI();
    1904:	f7fe fc53 	bl	1ae <WFI>
    // delay(RADIO_PACKET_DELAY);


    // mrr_cfo_val_fine = mrr_cfo_val_fine + mrr_freq_hopping_step; // 1: 0.8MHz, 2: 1.6MHz step
    // 5 hop pattern
    mrr_cfo_val_fine += 8;
    1908:	4b1d      	ldr	r3, [pc, #116]	; (1980 <mrr_send_radio_data+0x320>)
    190a:	881b      	ldrh	r3, [r3, #0]
    190c:	001a      	movs	r2, r3
    190e:	3208      	adds	r2, #8
    1910:	b292      	uxth	r2, r2
    if(mrr_cfo_val_fine > 20) {
    1912:	2a14      	cmp	r2, #20
    1914:	d802      	bhi.n	191c <mrr_send_radio_data+0x2bc>
    // delay(RADIO_PACKET_DELAY);


    // mrr_cfo_val_fine = mrr_cfo_val_fine + mrr_freq_hopping_step; // 1: 0.8MHz, 2: 1.6MHz step
    // 5 hop pattern
    mrr_cfo_val_fine += 8;
    1916:	4b1a      	ldr	r3, [pc, #104]	; (1980 <mrr_send_radio_data+0x320>)
    1918:	801a      	strh	r2, [r3, #0]
    191a:	e775      	b.n	1808 <mrr_send_radio_data+0x1a8>
    if(mrr_cfo_val_fine > 20) {
        mrr_cfo_val_fine -= 20;
    191c:	4a18      	ldr	r2, [pc, #96]	; (1980 <mrr_send_radio_data+0x320>)
    191e:	3b0c      	subs	r3, #12
    1920:	8013      	strh	r3, [r2, #0]
    1922:	e771      	b.n	1808 <mrr_send_radio_data+0x1a8>
    }
    }

    radio_packet_count++;
    1924:	4a1b      	ldr	r2, [pc, #108]	; (1994 <mrr_send_radio_data+0x334>)
    1926:	6813      	ldr	r3, [r2, #0]
    1928:	3301      	adds	r3, #1
    192a:	6013      	str	r3, [r2, #0]
#endif
    if (last_packet){
    192c:	9b03      	ldr	r3, [sp, #12]
    192e:	2b00      	cmp	r3, #0
    1930:	d004      	beq.n	193c <mrr_send_radio_data+0x2dc>
    radio_ready = 0;
    1932:	2200      	movs	r2, #0
    1934:	4b0c      	ldr	r3, [pc, #48]	; (1968 <mrr_send_radio_data+0x308>)
    1936:	801a      	strh	r2, [r3, #0]
    radio_power_off();
    1938:	f7fe fd7c 	bl	434 <radio_power_off>
    //}

    // TODO: check if this works 
    // *TIMERWD_GO = 0x1; // Turn on CPU watchdog timer
    // *REG_MBUS_WD = 1; // Enable Mbus watchdog timer
}
    193c:	b005      	add	sp, #20
    193e:	bdf0      	pop	{r4, r5, r6, r7, pc}
    1940:	000039e8 	.word	0x000039e8
    1944:	00003a2e 	.word	0x00003a2e
    1948:	00003804 	.word	0x00003804
    194c:	0000380c 	.word	0x0000380c
    1950:	fff7ffff 	.word	0xfff7ffff
    1954:	fffbffff 	.word	0xfffbffff
    1958:	ffdfffff 	.word	0xffdfffff
    195c:	ffefffff 	.word	0xffefffff
    1960:	00003810 	.word	0x00003810
    1964:	00003858 	.word	0x00003858
    1968:	00003986 	.word	0x00003986
    196c:	00003988 	.word	0x00003988
    1970:	00003b10 	.word	0x00003b10
    1974:	a0001200 	.word	0xa0001200
    1978:	ffc0ffff 	.word	0xffc0ffff
    197c:	a000007c 	.word	0xa000007c
    1980:	00003a5a 	.word	0x00003a5a
    1984:	ffff03ff 	.word	0xffff03ff
    1988:	00003af8 	.word	0x00003af8
    198c:	a0001100 	.word	0xa0001100
    1990:	00003950 	.word	0x00003950
    1994:	00003a20 	.word	0x00003a20

Disassembly of section .text.operation_temp_run:

00001998 <operation_temp_run>:
    sntv5_r01.TSNS_EN_SENSOR_LDO = 0;
    sntv5_r01.TSNS_ISOLATE       = 1;
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
}

static void operation_temp_run() {
    1998:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    if(snt_state == SNT_IDLE) {
    199a:	4c50      	ldr	r4, [pc, #320]	; (1adc <operation_temp_run+0x144>)
    199c:	8821      	ldrh	r1, [r4, #0]
    199e:	b289      	uxth	r1, r1
    19a0:	2900      	cmp	r1, #0
    19a2:	d10c      	bne.n	19be <operation_temp_run+0x26>
    sntv5_r01.TSNS_RESETn = 0;
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
}

static void snt_ldo_vref_on() {
    sntv5_r00.LDO_EN_VREF = 1;
    19a4:	2004      	movs	r0, #4
    19a6:	4b4e      	ldr	r3, [pc, #312]	; (1ae0 <operation_temp_run+0x148>)
    19a8:	881a      	ldrh	r2, [r3, #0]
    19aa:	4302      	orrs	r2, r0
    19ac:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
    19ae:	681a      	ldr	r2, [r3, #0]
    19b0:	f7fe fbb9 	bl	126 <mbus_remote_register_write>
    if(snt_state == SNT_IDLE) {

        // Turn on snt ldo vref; requires ~30 ms to settle
        // TODo: figure out delay time
        snt_ldo_vref_on();
        delay(MBUS_DELAY);
    19b4:	2064      	movs	r0, #100	; 0x64
    19b6:	f7fe fbf3 	bl	1a0 <delay>

        snt_state = SNT_TEMP_LDO;
    19ba:	2301      	movs	r3, #1
    19bc:	8023      	strh	r3, [r4, #0]

    }
    if(snt_state == SNT_TEMP_LDO) {
    19be:	8826      	ldrh	r6, [r4, #0]
    19c0:	b2b6      	uxth	r6, r6
    19c2:	2e01      	cmp	r6, #1
    19c4:	d12e      	bne.n	1a24 <operation_temp_run+0x8c>
    sntv5_r00.LDO_EN_VREF = 1;
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
}

static void snt_ldo_power_on() {
    sntv5_r00.LDO_EN_IREF = 1;
    19c6:	2702      	movs	r7, #2
    19c8:	4b45      	ldr	r3, [pc, #276]	; (1ae0 <operation_temp_run+0x148>)
    sntv5_r00.LDO_EN_LDO  = 1;
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
    19ca:	2100      	movs	r1, #0
    sntv5_r00.LDO_EN_VREF = 1;
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
}

static void snt_ldo_power_on() {
    sntv5_r00.LDO_EN_IREF = 1;
    19cc:	881a      	ldrh	r2, [r3, #0]
    sntv5_r00.LDO_EN_LDO  = 1;
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
    19ce:	2004      	movs	r0, #4
    sntv5_r00.LDO_EN_VREF = 1;
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
}

static void snt_ldo_power_on() {
    sntv5_r00.LDO_EN_IREF = 1;
    19d0:	433a      	orrs	r2, r7
    19d2:	801a      	strh	r2, [r3, #0]
    sntv5_r00.LDO_EN_LDO  = 1;
    19d4:	881a      	ldrh	r2, [r3, #0]
    19d6:	4332      	orrs	r2, r6
    19d8:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
    19da:	681a      	ldr	r2, [r3, #0]
    19dc:	f7fe fba3 	bl	126 <mbus_remote_register_write>
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
}

static void temp_sensor_power_on() {
    // Un-powergate digital block
    sntv5_r01.TSNS_SEL_LDO = 1;
    19e0:	2208      	movs	r2, #8
    19e2:	4d40      	ldr	r5, [pc, #256]	; (1ae4 <operation_temp_run+0x14c>)
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    19e4:	0031      	movs	r1, r6
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
}

static void temp_sensor_power_on() {
    // Un-powergate digital block
    sntv5_r01.TSNS_SEL_LDO = 1;
    19e6:	882b      	ldrh	r3, [r5, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    19e8:	2004      	movs	r0, #4
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
}

static void temp_sensor_power_on() {
    // Un-powergate digital block
    sntv5_r01.TSNS_SEL_LDO = 1;
    19ea:	4313      	orrs	r3, r2
    19ec:	802b      	strh	r3, [r5, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    19ee:	682a      	ldr	r2, [r5, #0]
    19f0:	f7fe fb99 	bl	126 <mbus_remote_register_write>
    // Un-powergate analog block
    sntv5_r01.TSNS_EN_SENSOR_LDO = 1;
    19f4:	2220      	movs	r2, #32
    19f6:	882b      	ldrh	r3, [r5, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    19f8:	0031      	movs	r1, r6
static void temp_sensor_power_on() {
    // Un-powergate digital block
    sntv5_r01.TSNS_SEL_LDO = 1;
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    // Un-powergate analog block
    sntv5_r01.TSNS_EN_SENSOR_LDO = 1;
    19fa:	4313      	orrs	r3, r2
    19fc:	802b      	strh	r3, [r5, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    19fe:	682a      	ldr	r2, [r5, #0]
    1a00:	2004      	movs	r0, #4
    1a02:	f7fe fb90 	bl	126 <mbus_remote_register_write>

    delay(MBUS_DELAY);
    1a06:	2064      	movs	r0, #100	; 0x64
    1a08:	f7fe fbca 	bl	1a0 <delay>

    // Release isolation
    sntv5_r01.TSNS_ISOLATE = 0;
    1a0c:	882b      	ldrh	r3, [r5, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    1a0e:	0031      	movs	r1, r6
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);

    delay(MBUS_DELAY);

    // Release isolation
    sntv5_r01.TSNS_ISOLATE = 0;
    1a10:	43bb      	bics	r3, r7
    1a12:	802b      	strh	r3, [r5, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    1a14:	682a      	ldr	r2, [r5, #0]
    1a16:	2004      	movs	r0, #4
    1a18:	f7fe fb85 	bl	126 <mbus_remote_register_write>
        // Power on snt ldo
        snt_ldo_power_on();

        // Power on temp sensor
        temp_sensor_power_on();
        delay(MBUS_DELAY);
    1a1c:	2064      	movs	r0, #100	; 0x64
    1a1e:	f7fe fbbf 	bl	1a0 <delay>

        snt_state = SNT_TEMP_START;
    1a22:	8027      	strh	r7, [r4, #0]
    }
    if(snt_state == SNT_TEMP_START) {
    1a24:	8823      	ldrh	r3, [r4, #0]
    1a26:	2b02      	cmp	r3, #2
    1a28:	d11d      	bne.n	1a66 <operation_temp_run+0xce>
        // Use TIMER32 as a timeout counter
        wfi_timeout_flag = 0;
    1a2a:	2500      	movs	r5, #0
        config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default
    1a2c:	20a0      	movs	r0, #160	; 0xa0

        snt_state = SNT_TEMP_START;
    }
    if(snt_state == SNT_TEMP_START) {
        // Use TIMER32 as a timeout counter
        wfi_timeout_flag = 0;
    1a2e:	4b2e      	ldr	r3, [pc, #184]	; (1ae8 <operation_temp_run+0x150>)
        config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default
    1a30:	002a      	movs	r2, r5

        snt_state = SNT_TEMP_START;
    }
    if(snt_state == SNT_TEMP_START) {
        // Use TIMER32 as a timeout counter
        wfi_timeout_flag = 0;
    1a32:	801d      	strh	r5, [r3, #0]
        config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default
    1a34:	2101      	movs	r1, #1
    1a36:	002b      	movs	r3, r5
    1a38:	0300      	lsls	r0, r0, #12
    1a3a:	f7fe fbbb 	bl	1b4 <config_timer32>
/**********************************************
 * Temp sensor functions (SNTv5)
 **********************************************/

static void temp_sensor_start() {
    sntv5_r01.TSNS_RESETn = 1;
    1a3e:	2101      	movs	r1, #1
    sntv5_r01.TSNS_EN_IRQ = 1;
    1a40:	2080      	movs	r0, #128	; 0x80
/**********************************************
 * Temp sensor functions (SNTv5)
 **********************************************/

static void temp_sensor_start() {
    sntv5_r01.TSNS_RESETn = 1;
    1a42:	4b28      	ldr	r3, [pc, #160]	; (1ae4 <operation_temp_run+0x14c>)
    sntv5_r01.TSNS_EN_IRQ = 1;
    1a44:	0040      	lsls	r0, r0, #1
/**********************************************
 * Temp sensor functions (SNTv5)
 **********************************************/

static void temp_sensor_start() {
    sntv5_r01.TSNS_RESETn = 1;
    1a46:	881a      	ldrh	r2, [r3, #0]
    1a48:	430a      	orrs	r2, r1
    1a4a:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_EN_IRQ = 1;
    1a4c:	881a      	ldrh	r2, [r3, #0]
    1a4e:	4302      	orrs	r2, r0
    1a50:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    1a52:	681a      	ldr	r2, [r3, #0]
    1a54:	38fc      	subs	r0, #252	; 0xfc
    1a56:	f7fe fb66 	bl	126 <mbus_remote_register_write>
        
        // Start temp sensor
        temp_sensor_start();

        // Wait for temp sensor output or TIMER32
        WFI();
    1a5a:	f7fe fba8 	bl	1ae <WFI>

        // Turn off timer32
        *TIMER32_GO = 0;
    1a5e:	4b23      	ldr	r3, [pc, #140]	; (1aec <operation_temp_run+0x154>)
    1a60:	601d      	str	r5, [r3, #0]

        snt_state = SNT_TEMP_READ;
    1a62:	2303      	movs	r3, #3
    1a64:	8023      	strh	r3, [r4, #0]
    }
    if(snt_state == SNT_TEMP_READ) {
    1a66:	8823      	ldrh	r3, [r4, #0]
    1a68:	2b03      	cmp	r3, #3
    1a6a:	d135      	bne.n	1ad8 <operation_temp_run+0x140>
        if(wfi_timeout_flag) {
    1a6c:	4b1e      	ldr	r3, [pc, #120]	; (1ae8 <operation_temp_run+0x150>)
    1a6e:	881d      	ldrh	r5, [r3, #0]
    1a70:	b2ad      	uxth	r5, r5
    1a72:	2d00      	cmp	r5, #0
    1a74:	d005      	beq.n	1a82 <operation_temp_run+0xea>
    mbus_write_message32(0xBF, code);
}

static void sys_err(uint32_t code)
{
    set_system_error(code);
    1a76:	2080      	movs	r0, #128	; 0x80
    1a78:	0440      	lsls	r0, r0, #17
    1a7a:	f7ff fd83 	bl	1584 <set_system_error>

    // mbus_write_message32(0xAF, code);
    operation_sleep();
    1a7e:	f7fe fd49 	bl	514 <operation_sleep>
        if(wfi_timeout_flag) {
            // if timeout, set error msg
            sys_err(0x01000000);
        }
        else {
            snt_sys_temp_code = *REG0;
    1a82:	23a0      	movs	r3, #160	; 0xa0
    sntv5_r01.TSNS_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
}

static void temp_sensor_power_off() {
    sntv5_r01.TSNS_RESETn        = 0;
    1a84:	2601      	movs	r6, #1
    sntv5_r01.TSNS_SEL_LDO       = 0;
    1a86:	2108      	movs	r1, #8
    sntv5_r01.TSNS_EN_SENSOR_LDO = 0;
    sntv5_r01.TSNS_ISOLATE       = 1;
    1a88:	2702      	movs	r7, #2
        if(wfi_timeout_flag) {
            // if timeout, set error msg
            sys_err(0x01000000);
        }
        else {
            snt_sys_temp_code = *REG0;
    1a8a:	061b      	lsls	r3, r3, #24
    1a8c:	681a      	ldr	r2, [r3, #0]
    1a8e:	4b18      	ldr	r3, [pc, #96]	; (1af0 <operation_temp_run+0x158>)
static void temp_sensor_power_off() {
    sntv5_r01.TSNS_RESETn        = 0;
    sntv5_r01.TSNS_SEL_LDO       = 0;
    sntv5_r01.TSNS_EN_SENSOR_LDO = 0;
    sntv5_r01.TSNS_ISOLATE       = 1;
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    1a90:	2004      	movs	r0, #4
        if(wfi_timeout_flag) {
            // if timeout, set error msg
            sys_err(0x01000000);
        }
        else {
            snt_sys_temp_code = *REG0;
    1a92:	601a      	str	r2, [r3, #0]
    sntv5_r01.TSNS_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
}

static void temp_sensor_power_off() {
    sntv5_r01.TSNS_RESETn        = 0;
    1a94:	4b13      	ldr	r3, [pc, #76]	; (1ae4 <operation_temp_run+0x14c>)
    1a96:	881a      	ldrh	r2, [r3, #0]
    1a98:	43b2      	bics	r2, r6
    1a9a:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_SEL_LDO       = 0;
    1a9c:	881a      	ldrh	r2, [r3, #0]
    1a9e:	438a      	bics	r2, r1
    1aa0:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_EN_SENSOR_LDO = 0;
    1aa2:	881a      	ldrh	r2, [r3, #0]
    1aa4:	3118      	adds	r1, #24
    1aa6:	438a      	bics	r2, r1
    1aa8:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_ISOLATE       = 1;
    1aaa:	881a      	ldrh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    1aac:	0031      	movs	r1, r6

static void temp_sensor_power_off() {
    sntv5_r01.TSNS_RESETn        = 0;
    sntv5_r01.TSNS_SEL_LDO       = 0;
    sntv5_r01.TSNS_EN_SENSOR_LDO = 0;
    sntv5_r01.TSNS_ISOLATE       = 1;
    1aae:	433a      	orrs	r2, r7
    1ab0:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    1ab2:	681a      	ldr	r2, [r3, #0]
    1ab4:	f7fe fb37 	bl	126 <mbus_remote_register_write>
    sntv5_r00.LDO_EN_LDO  = 1;
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
}

static void snt_ldo_power_off() {
    sntv5_r00.LDO_EN_VREF = 0;
    1ab8:	2004      	movs	r0, #4
    1aba:	4b09      	ldr	r3, [pc, #36]	; (1ae0 <operation_temp_run+0x148>)
    sntv5_r00.LDO_EN_IREF = 0;
    sntv5_r00.LDO_EN_LDO  = 0;
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
    1abc:	0029      	movs	r1, r5
    sntv5_r00.LDO_EN_LDO  = 1;
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
}

static void snt_ldo_power_off() {
    sntv5_r00.LDO_EN_VREF = 0;
    1abe:	881a      	ldrh	r2, [r3, #0]
    1ac0:	4382      	bics	r2, r0
    1ac2:	801a      	strh	r2, [r3, #0]
    sntv5_r00.LDO_EN_IREF = 0;
    1ac4:	881a      	ldrh	r2, [r3, #0]
    1ac6:	43ba      	bics	r2, r7
    1ac8:	801a      	strh	r2, [r3, #0]
    sntv5_r00.LDO_EN_LDO  = 0;
    1aca:	881a      	ldrh	r2, [r3, #0]
    1acc:	43b2      	bics	r2, r6
    1ace:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 0, sntv5_r00.as_int);
    1ad0:	681a      	ldr	r2, [r3, #0]
    1ad2:	f7fe fb28 	bl	126 <mbus_remote_register_write>
            
            // turn off temp sensor and ldo
            temp_sensor_power_off();
            snt_ldo_power_off();

            snt_state = SNT_IDLE;
    1ad6:	8025      	strh	r5, [r4, #0]
        }
    }
}
    1ad8:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    1ada:	46c0      	nop			; (mov r8, r8)
    1adc:	00003ad2 	.word	0x00003ad2
    1ae0:	000038b0 	.word	0x000038b0
    1ae4:	0000397c 	.word	0x0000397c
    1ae8:	00003af8 	.word	0x00003af8
    1aec:	a0001100 	.word	0xa0001100
    1af0:	000038b8 	.word	0x000038b8

Disassembly of section .text.set_goc_cmd:

00001af4 <set_goc_cmd>:

/**********************************************
 * Interrupt handlers
 **********************************************/

void set_goc_cmd() {
    1af4:	b570      	push	{r4, r5, r6, lr}
    lnt_stop();

    // goc_component = (*GOC_DATA_IRQ >> 24) & 0xFF;
    // goc_func_id = (*GOC_DATA_IRQ >> 16) & 0xFF;
    // goc_data = *GOC_DATA_IRQ & 0xFFFF;
    projected_end_time_in_sec = 0;
    1af6:	2400      	movs	r4, #0
    goc_data_full = *GOC_DATA_IRQ;
    1af8:	258c      	movs	r5, #140	; 0x8c
 * Interrupt handlers
 **********************************************/

void set_goc_cmd() {
    // disable timer
    lnt_stop();
    1afa:	f7fe fdc5 	bl	688 <lnt_stop>

    // goc_component = (*GOC_DATA_IRQ >> 24) & 0xFF;
    // goc_func_id = (*GOC_DATA_IRQ >> 16) & 0xFF;
    // goc_data = *GOC_DATA_IRQ & 0xFFFF;
    projected_end_time_in_sec = 0;
    1afe:	4b0a      	ldr	r3, [pc, #40]	; (1b28 <set_goc_cmd+0x34>)
    1b00:	601c      	str	r4, [r3, #0]
    goc_data_full = *GOC_DATA_IRQ;
    1b02:	682a      	ldr	r2, [r5, #0]
    1b04:	4b09      	ldr	r3, [pc, #36]	; (1b2c <set_goc_cmd+0x38>)
    1b06:	601a      	str	r2, [r3, #0]
    goc_state = 0;
    1b08:	4b09      	ldr	r3, [pc, #36]	; (1b30 <set_goc_cmd+0x3c>)
    1b0a:	801c      	strh	r4, [r3, #0]
    update_system_time();
    1b0c:	f7ff fd4c 	bl	15a8 <update_system_time>

    if((*GOC_DATA_IRQ >> 24) == 0x06) {
    1b10:	682b      	ldr	r3, [r5, #0]
    1b12:	0e1b      	lsrs	r3, r3, #24
    1b14:	2b06      	cmp	r3, #6
    1b16:	d105      	bne.n	1b24 <set_goc_cmd+0x30>
        // Timeout trigger implemented here to ensure that it's run
        *TIMERWD_GO = 0x1; // Turn on CPU watchdog timer
    1b18:	2201      	movs	r2, #1
    1b1a:	4b06      	ldr	r3, [pc, #24]	; (1b34 <set_goc_cmd+0x40>)
    1b1c:	601a      	str	r2, [r3, #0]
        *REG_MBUS_WD = 0; // Disable Mbus watchdog timer
    1b1e:	4b06      	ldr	r3, [pc, #24]	; (1b38 <set_goc_cmd+0x44>)
    1b20:	601c      	str	r4, [r3, #0]
    1b22:	e7fe      	b.n	1b22 <set_goc_cmd+0x2e>

        while(1);
    }
}
    1b24:	bd70      	pop	{r4, r5, r6, pc}
    1b26:	46c0      	nop			; (mov r8, r8)
    1b28:	00003a60 	.word	0x00003a60
    1b2c:	00003a28 	.word	0x00003a28
    1b30:	00003a1c 	.word	0x00003a1c
    1b34:	a0001200 	.word	0xa0001200
    1b38:	a000007c 	.word	0xa000007c

Disassembly of section .text.handler_ext_int_wakeup:

00001b3c <handler_ext_int_wakeup>:
void handler_ext_int_reg1       (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2       (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3       (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_wakeup( void ) { // WAKEUP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP);
    1b3c:	2201      	movs	r2, #1
    1b3e:	4b01      	ldr	r3, [pc, #4]	; (1b44 <handler_ext_int_wakeup+0x8>)
    1b40:	601a      	str	r2, [r3, #0]
}
    1b42:	4770      	bx	lr
    1b44:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_gocep:

00001b48 <handler_ext_int_gocep>:

void handler_ext_int_gocep( void ) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
    1b48:	2204      	movs	r2, #4
    1b4a:	4b01      	ldr	r3, [pc, #4]	; (1b50 <handler_ext_int_gocep+0x8>)
    1b4c:	601a      	str	r2, [r3, #0]
}
    1b4e:	4770      	bx	lr
    1b50:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_timer32:

00001b54 <handler_ext_int_timer32>:

void handler_ext_int_timer32( void ) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    1b54:	2208      	movs	r2, #8
    1b56:	4b04      	ldr	r3, [pc, #16]	; (1b68 <handler_ext_int_timer32+0x14>)
    1b58:	601a      	str	r2, [r3, #0]
    *TIMER32_STAT = 0x0;
    1b5a:	2200      	movs	r2, #0
    1b5c:	4b03      	ldr	r3, [pc, #12]	; (1b6c <handler_ext_int_timer32+0x18>)
    1b5e:	601a      	str	r2, [r3, #0]
    
    wfi_timeout_flag = 1;
    1b60:	4b03      	ldr	r3, [pc, #12]	; (1b70 <handler_ext_int_timer32+0x1c>)
    1b62:	3201      	adds	r2, #1
    1b64:	801a      	strh	r2, [r3, #0]
}
    1b66:	4770      	bx	lr
    1b68:	e000e280 	.word	0xe000e280
    1b6c:	a0001110 	.word	0xa0001110
    1b70:	00003af8 	.word	0x00003af8

Disassembly of section .text.handler_ext_int_xot:

00001b74 <handler_ext_int_xot>:

void handler_ext_int_xot( void ) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_XOT);
    1b74:	2280      	movs	r2, #128	; 0x80
    1b76:	4b02      	ldr	r3, [pc, #8]	; (1b80 <handler_ext_int_xot+0xc>)
    1b78:	0352      	lsls	r2, r2, #13
    1b7a:	601a      	str	r2, [r3, #0]
}
    1b7c:	4770      	bx	lr
    1b7e:	46c0      	nop			; (mov r8, r8)
    1b80:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_reg0:

00001b84 <handler_ext_int_reg0>:

void handler_ext_int_reg0( void ) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);
    1b84:	2280      	movs	r2, #128	; 0x80
    1b86:	4b02      	ldr	r3, [pc, #8]	; (1b90 <handler_ext_int_reg0+0xc>)
    1b88:	0052      	lsls	r2, r2, #1
    1b8a:	601a      	str	r2, [r3, #0]
}
    1b8c:	4770      	bx	lr
    1b8e:	46c0      	nop			; (mov r8, r8)
    1b90:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_reg1:

00001b94 <handler_ext_int_reg1>:

void handler_ext_int_reg1( void ) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1);
    1b94:	2280      	movs	r2, #128	; 0x80
    1b96:	4b02      	ldr	r3, [pc, #8]	; (1ba0 <handler_ext_int_reg1+0xc>)
    1b98:	0092      	lsls	r2, r2, #2
    1b9a:	601a      	str	r2, [r3, #0]
}
    1b9c:	4770      	bx	lr
    1b9e:	46c0      	nop			; (mov r8, r8)
    1ba0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_reg2:

00001ba4 <handler_ext_int_reg2>:

void handler_ext_int_reg2( void ) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2);
    1ba4:	2280      	movs	r2, #128	; 0x80
    1ba6:	4b02      	ldr	r3, [pc, #8]	; (1bb0 <handler_ext_int_reg2+0xc>)
    1ba8:	00d2      	lsls	r2, r2, #3
    1baa:	601a      	str	r2, [r3, #0]
}
    1bac:	4770      	bx	lr
    1bae:	46c0      	nop			; (mov r8, r8)
    1bb0:	e000e280 	.word	0xe000e280

Disassembly of section .text.handler_ext_int_reg3:

00001bb4 <handler_ext_int_reg3>:

void handler_ext_int_reg3( void ) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3);
    1bb4:	2280      	movs	r2, #128	; 0x80
    1bb6:	4b02      	ldr	r3, [pc, #8]	; (1bc0 <handler_ext_int_reg3+0xc>)
    1bb8:	0112      	lsls	r2, r2, #4
    1bba:	601a      	str	r2, [r3, #0]
}
    1bbc:	4770      	bx	lr
    1bbe:	46c0      	nop			; (mov r8, r8)
    1bc0:	e000e280 	.word	0xe000e280

Disassembly of section .text.radio_unit:

00001bc4 <radio_unit>:
/**********************************************
 * MAIN function starts here
 **********************************************/

// requires unit_count < max_unit_count
void radio_unit(uint16_t unit_count) {
    1bc4:	b5f0      	push	{r4, r5, r6, r7, lr}
    1bc6:	0005      	movs	r5, r0
    1bc8:	b089      	sub	sp, #36	; 0x24
    uint16_t addr = (unit_count * CODE_CACHE_LEN) << 2;    // CODE_CACHE_LEN is the size of a unit
    pmu_setting_temp_based(1);
    1bca:	2001      	movs	r0, #1
    1bcc:	f7ff fca2 	bl	1514 <pmu_setting_temp_based>
    // read out data and check if temp or light
    set_halt_until_mbus_trx();
    1bd0:	f7fe fb4a 	bl	268 <set_halt_until_mbus_trx>
    mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) addr, PRE_ADDR, code_cache, CODE_CACHE_LEN - 1);
    1bd4:	2109      	movs	r1, #9
    1bd6:	2308      	movs	r3, #8
    1bd8:	4369      	muls	r1, r5
            temp_arr[0] |= (1 << 15) | (CHIP_ID << 10) | (light_packet_num << 4);
            light_packet_num = (light_packet_num + 1) & 0x3F;
        }
        else {
            temp_arr[0] |= (CHIP_ID << 10) | (temp_packet_num << 4);
            temp_packet_num = (temp_packet_num + 1) & 0x3F;
    1bda:	263f      	movs	r6, #63	; 0x3f
    mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) addr, PRE_ADDR, code_cache, CODE_CACHE_LEN - 1);
    set_halt_until_mbus_tx();

    uint32_t temp_arr[3];
    uint16_t i;
    bool is_light = ((code_cache[0] & 0x80000000) != 0);
    1bdc:	2504      	movs	r5, #4
void radio_unit(uint16_t unit_count) {
    uint16_t addr = (unit_count * CODE_CACHE_LEN) << 2;    // CODE_CACHE_LEN is the size of a unit
    pmu_setting_temp_based(1);
    // read out data and check if temp or light
    set_halt_until_mbus_trx();
    mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) addr, PRE_ADDR, code_cache, CODE_CACHE_LEN - 1);
    1bde:	4c24      	ldr	r4, [pc, #144]	; (1c70 <radio_unit+0xac>)
    1be0:	0089      	lsls	r1, r1, #2
    1be2:	b289      	uxth	r1, r1
    1be4:	9300      	str	r3, [sp, #0]
    1be6:	2201      	movs	r2, #1
    1be8:	0023      	movs	r3, r4
    1bea:	2006      	movs	r0, #6
    1bec:	f7fe fac2 	bl	174 <mbus_copy_mem_from_remote_to_any_bulk>
    set_halt_until_mbus_tx();
    1bf0:	f7fe fb34 	bl	25c <set_halt_until_mbus_tx>

    uint32_t temp_arr[3];
    uint16_t i;
    bool is_light = ((code_cache[0] & 0x80000000) != 0);
    1bf4:	6827      	ldr	r7, [r4, #0]
    // radio out four times
    for(i = 0; i < 4; i++) {
        // first 96 bits are read out using temp_arr
        // This is needed because radio_data_arr is big endian
        temp_arr[0] = code_cache[0];
        temp_arr[1] = code_cache[1];
    1bf6:	6863      	ldr	r3, [r4, #4]
    1bf8:	2203      	movs	r2, #3
    1bfa:	9306      	str	r3, [sp, #24]
        temp_arr[2] = code_cache[2];
    1bfc:	68a3      	ldr	r3, [r4, #8]
    1bfe:	2100      	movs	r1, #0
    1c00:	9307      	str	r3, [sp, #28]

        // clear out top 16 bits
        temp_arr[0] &= 0x0000FFFF;
    1c02:	8823      	ldrh	r3, [r4, #0]
    1c04:	a805      	add	r0, sp, #20
    1c06:	9305      	str	r3, [sp, #20]
    1c08:	230c      	movs	r3, #12
    1c0a:	f7fe fd7d 	bl	708 <right_shift_arr.part.0>
    1c0e:	4b19      	ldr	r3, [pc, #100]	; (1c74 <radio_unit+0xb0>)
        // 12 bits for both light and temp
        right_shift_arr(temp_arr, 0, 3, LIGHT_HEADER_SIZE);

        // insert header
        if(is_light) {
            temp_arr[0] |= (1 << 15) | (CHIP_ID << 10) | (light_packet_num << 4);
    1c10:	881a      	ldrh	r2, [r3, #0]
        // right shift to make space for packet header
        // 12 bits for both light and temp
        right_shift_arr(temp_arr, 0, 3, LIGHT_HEADER_SIZE);

        // insert header
        if(is_light) {
    1c12:	2f00      	cmp	r7, #0
    1c14:	da07      	bge.n	1c26 <radio_unit+0x62>
            temp_arr[0] |= (1 << 15) | (CHIP_ID << 10) | (light_packet_num << 4);
    1c16:	4918      	ldr	r1, [pc, #96]	; (1c78 <radio_unit+0xb4>)
    1c18:	0290      	lsls	r0, r2, #10
    1c1a:	880b      	ldrh	r3, [r1, #0]
    1c1c:	011a      	lsls	r2, r3, #4
    1c1e:	4302      	orrs	r2, r0
    1c20:	2080      	movs	r0, #128	; 0x80
    1c22:	0200      	lsls	r0, r0, #8
    1c24:	e003      	b.n	1c2e <radio_unit+0x6a>
            light_packet_num = (light_packet_num + 1) & 0x3F;
        }
        else {
            temp_arr[0] |= (CHIP_ID << 10) | (temp_packet_num << 4);
    1c26:	4915      	ldr	r1, [pc, #84]	; (1c7c <radio_unit+0xb8>)
    1c28:	0290      	lsls	r0, r2, #10
    1c2a:	880b      	ldrh	r3, [r1, #0]
    1c2c:	011a      	lsls	r2, r3, #4
    1c2e:	4302      	orrs	r2, r0
    1c30:	9805      	ldr	r0, [sp, #20]
            temp_packet_num = (temp_packet_num + 1) & 0x3F;
    1c32:	3301      	adds	r3, #1
        if(is_light) {
            temp_arr[0] |= (1 << 15) | (CHIP_ID << 10) | (light_packet_num << 4);
            light_packet_num = (light_packet_num + 1) & 0x3F;
        }
        else {
            temp_arr[0] |= (CHIP_ID << 10) | (temp_packet_num << 4);
    1c34:	4302      	orrs	r2, r0
    1c36:	9205      	str	r2, [sp, #20]
            temp_packet_num = (temp_packet_num + 1) & 0x3F;
    1c38:	4033      	ands	r3, r6
    1c3a:	800b      	strh	r3, [r1, #0]
        }

        // assign to radio arr
        radio_data_arr[2] = temp_arr[0];
    1c3c:	9b05      	ldr	r3, [sp, #20]
        radio_data_arr[1] = temp_arr[1];
        radio_data_arr[0] = temp_arr[2];

        mrr_send_radio_data(0);
    1c3e:	2000      	movs	r0, #0
            temp_arr[0] |= (CHIP_ID << 10) | (temp_packet_num << 4);
            temp_packet_num = (temp_packet_num + 1) & 0x3F;
        }

        // assign to radio arr
        radio_data_arr[2] = temp_arr[0];
    1c40:	9303      	str	r3, [sp, #12]
    1c42:	9a03      	ldr	r2, [sp, #12]
    1c44:	4b0e      	ldr	r3, [pc, #56]	; (1c80 <radio_unit+0xbc>)
    1c46:	3d01      	subs	r5, #1
    1c48:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = temp_arr[1];
    1c4a:	9a06      	ldr	r2, [sp, #24]
    1c4c:	b2ad      	uxth	r5, r5
    1c4e:	605a      	str	r2, [r3, #4]
    1c50:	9203      	str	r2, [sp, #12]
        radio_data_arr[0] = temp_arr[2];
    1c52:	9a07      	ldr	r2, [sp, #28]
    1c54:	601a      	str	r2, [r3, #0]

        mrr_send_radio_data(0);
    1c56:	f7ff fd03 	bl	1660 <mrr_send_radio_data>
    1c5a:	2344      	movs	r3, #68	; 0x44
    1c5c:	2209      	movs	r2, #9
    1c5e:	425b      	negs	r3, r3
    1c60:	2100      	movs	r1, #0
    1c62:	0020      	movs	r0, r4
    1c64:	f7fe fd50 	bl	708 <right_shift_arr.part.0>

    uint32_t temp_arr[3];
    uint16_t i;
    bool is_light = ((code_cache[0] & 0x80000000) != 0);
    // radio out four times
    for(i = 0; i < 4; i++) {
    1c68:	2d00      	cmp	r5, #0
    1c6a:	d1c4      	bne.n	1bf6 <radio_unit+0x32>

        // left shift a new packet over
        // 68 bits for both light and temp
        right_shift_arr(code_cache, 0, CODE_CACHE_LEN, -LIGHT_CONTENT_SIZE);
    }
}
    1c6c:	b009      	add	sp, #36	; 0x24
    1c6e:	bdf0      	pop	{r4, r5, r6, r7, pc}
    1c70:	000039f4 	.word	0x000039f4
    1c74:	00003abc 	.word	0x00003abc
    1c78:	00003aa2 	.word	0x00003aa2
    1c7c:	00003a6c 	.word	0x00003a6c
    1c80:	000039e8 	.word	0x000039e8

Disassembly of section .text.set_send_enable:

00001c84 <set_send_enable>:
        radio_unit(start_unit_count + i);
    }
}

uint16_t set_send_enable() {
    if(OVERRIDE_RAD) {
    1c84:	4b14      	ldr	r3, [pc, #80]	; (1cd8 <set_send_enable+0x54>)
        }
        radio_unit(start_unit_count + i);
    }
}

uint16_t set_send_enable() {
    1c86:	b510      	push	{r4, lr}
    if(OVERRIDE_RAD) {
    1c88:	8818      	ldrh	r0, [r3, #0]
    1c8a:	b280      	uxth	r0, r0
    1c8c:	2800      	cmp	r0, #0
    1c8e:	d120      	bne.n	1cd2 <set_send_enable+0x4e>
        return 1;
    }
    if(snt_sys_temp_code < MRR_TEMP_THRESH_LOW || snt_sys_temp_code > MRR_TEMP_THRESH_HIGH) {
    1c90:	4b12      	ldr	r3, [pc, #72]	; (1cdc <set_send_enable+0x58>)
    1c92:	4a13      	ldr	r2, [pc, #76]	; (1ce0 <set_send_enable+0x5c>)
    1c94:	8819      	ldrh	r1, [r3, #0]
    1c96:	6813      	ldr	r3, [r2, #0]
    1c98:	4299      	cmp	r1, r3
    1c9a:	d81b      	bhi.n	1cd4 <set_send_enable+0x50>
    1c9c:	4b11      	ldr	r3, [pc, #68]	; (1ce4 <set_send_enable+0x60>)
    1c9e:	8819      	ldrh	r1, [r3, #0]
    1ca0:	6813      	ldr	r3, [r2, #0]
    1ca2:	4299      	cmp	r1, r3
    1ca4:	d316      	bcc.n	1cd4 <set_send_enable+0x50>
    1ca6:	2301      	movs	r3, #1
        return 0;
    }
    uint16_t i;
    for(i = 1; i < 6; i++) {
        if(snt_sys_temp_code < PMU_TEMP_THRESH[i]) {
    1ca8:	490f      	ldr	r1, [pc, #60]	; (1ce8 <set_send_enable+0x64>)
    1caa:	0098      	lsls	r0, r3, #2
    1cac:	5844      	ldr	r4, [r0, r1]
    1cae:	6810      	ldr	r0, [r2, #0]
    1cb0:	4284      	cmp	r4, r0
    1cb2:	d90b      	bls.n	1ccc <set_send_enable+0x48>
    }
}

uint16_t set_send_enable() {
    if(OVERRIDE_RAD) {
        return 1;
    1cb4:	2000      	movs	r0, #0
        return 0;
    }
    uint16_t i;
    for(i = 1; i < 6; i++) {
        if(snt_sys_temp_code < PMU_TEMP_THRESH[i]) {
            if(read_data_batadc <= PMU_ADC_THRESH[i - 1]) {
    1cb6:	4a0d      	ldr	r2, [pc, #52]	; (1cec <set_send_enable+0x68>)
    1cb8:	3b01      	subs	r3, #1
    1cba:	490d      	ldr	r1, [pc, #52]	; (1cf0 <set_send_enable+0x6c>)
    1cbc:	8812      	ldrh	r2, [r2, #0]
    1cbe:	009b      	lsls	r3, r3, #2
    1cc0:	585b      	ldr	r3, [r3, r1]
    1cc2:	b292      	uxth	r2, r2
    }
}

uint16_t set_send_enable() {
    if(OVERRIDE_RAD) {
        return 1;
    1cc4:	4293      	cmp	r3, r2
    1cc6:	4140      	adcs	r0, r0
    1cc8:	b280      	uxth	r0, r0
    1cca:	e003      	b.n	1cd4 <set_send_enable+0x50>
    1ccc:	3301      	adds	r3, #1
    }
    if(snt_sys_temp_code < MRR_TEMP_THRESH_LOW || snt_sys_temp_code > MRR_TEMP_THRESH_HIGH) {
        return 0;
    }
    uint16_t i;
    for(i = 1; i < 6; i++) {
    1cce:	2b06      	cmp	r3, #6
    1cd0:	d1eb      	bne.n	1caa <set_send_enable+0x26>
    }
}

uint16_t set_send_enable() {
    if(OVERRIDE_RAD) {
        return 1;
    1cd2:	2001      	movs	r0, #1
                return 0;
            }
        }   
    }
    return 1;
}
    1cd4:	bd10      	pop	{r4, pc}
    1cd6:	46c0      	nop			; (mov r8, r8)
    1cd8:	00003a3e 	.word	0x00003a3e
    1cdc:	00003958 	.word	0x00003958
    1ce0:	000038b8 	.word	0x000038b8
    1ce4:	0000386c 	.word	0x0000386c
    1ce8:	00003934 	.word	0x00003934
    1cec:	000039e4 	.word	0x000039e4
    1cf0:	000038dc 	.word	0x000038dc

Disassembly of section .text.set_low_pwr_low_trigger:

00001cf4 <set_low_pwr_low_trigger>:

// return true if PMU ADC output is higher than LOW_THRESH
// that means system should go into LOW_PWR
uint16_t set_low_pwr_low_trigger() {
    1cf4:	2300      	movs	r3, #0
    1cf6:	b510      	push	{r4, lr}
    uint16_t i;
    for(i = 0; i < 6; i++) {
        if(snt_sys_temp_code < PMU_TEMP_THRESH[i]) {
    1cf8:	4a0a      	ldr	r2, [pc, #40]	; (1d24 <set_low_pwr_low_trigger+0x30>)
    1cfa:	490b      	ldr	r1, [pc, #44]	; (1d28 <set_low_pwr_low_trigger+0x34>)
    1cfc:	0098      	lsls	r0, r3, #2
    1cfe:	5884      	ldr	r4, [r0, r2]
    1d00:	6808      	ldr	r0, [r1, #0]
    1d02:	4284      	cmp	r4, r0
    1d04:	d903      	bls.n	1d0e <set_low_pwr_low_trigger+0x1a>
            if(read_data_batadc > LOW_PWR_LOW_ADC_THRESH[i]) {
    1d06:	4a09      	ldr	r2, [pc, #36]	; (1d2c <set_low_pwr_low_trigger+0x38>)
    1d08:	005b      	lsls	r3, r3, #1
    1d0a:	5a98      	ldrh	r0, [r3, r2]
    1d0c:	e004      	b.n	1d18 <set_low_pwr_low_trigger+0x24>
    1d0e:	3301      	adds	r3, #1

// return true if PMU ADC output is higher than LOW_THRESH
// that means system should go into LOW_PWR
uint16_t set_low_pwr_low_trigger() {
    uint16_t i;
    for(i = 0; i < 6; i++) {
    1d10:	2b06      	cmp	r3, #6
    1d12:	d1f3      	bne.n	1cfc <set_low_pwr_low_trigger+0x8>
            }
            return 0;
        }
    }
    // > 60C
    if(read_data_batadc > LOW_PWR_LOW_ADC_THRESH[6]) {
    1d14:	4b05      	ldr	r3, [pc, #20]	; (1d2c <set_low_pwr_low_trigger+0x38>)
    1d16:	8998      	ldrh	r0, [r3, #12]
    1d18:	4b05      	ldr	r3, [pc, #20]	; (1d30 <set_low_pwr_low_trigger+0x3c>)
    1d1a:	881b      	ldrh	r3, [r3, #0]
uint16_t set_low_pwr_low_trigger() {
    uint16_t i;
    for(i = 0; i < 6; i++) {
        if(snt_sys_temp_code < PMU_TEMP_THRESH[i]) {
            if(read_data_batadc > LOW_PWR_LOW_ADC_THRESH[i]) {
                return 1;
    1d1c:	4298      	cmp	r0, r3
    1d1e:	4180      	sbcs	r0, r0
    1d20:	4240      	negs	r0, r0
    // > 60C
    if(read_data_batadc > LOW_PWR_LOW_ADC_THRESH[6]) {
        return 1;
    }
    return 0;
}
    1d22:	bd10      	pop	{r4, pc}
    1d24:	00003934 	.word	0x00003934
    1d28:	000038b8 	.word	0x000038b8
    1d2c:	0000381c 	.word	0x0000381c
    1d30:	000039e4 	.word	0x000039e4

Disassembly of section .text.set_low_pwr_high_trigger:

00001d34 <set_low_pwr_high_trigger>:

// return true if PMU ADC output is higher than HIGH_THRESH
// return false if PMU ADC output is lower than HIGH_THRESH
// that means system should get out of LOW_PWR
uint16_t set_low_pwr_high_trigger() {
    1d34:	2300      	movs	r3, #0
    1d36:	b510      	push	{r4, lr}
    uint16_t i;
    for(i = 0; i < 6; i++) {
        if(snt_sys_temp_code < PMU_TEMP_THRESH[i]) {
    1d38:	4a0a      	ldr	r2, [pc, #40]	; (1d64 <set_low_pwr_high_trigger+0x30>)
    1d3a:	490b      	ldr	r1, [pc, #44]	; (1d68 <set_low_pwr_high_trigger+0x34>)
    1d3c:	0098      	lsls	r0, r3, #2
    1d3e:	5884      	ldr	r4, [r0, r2]
    1d40:	6808      	ldr	r0, [r1, #0]
    1d42:	4284      	cmp	r4, r0
    1d44:	d903      	bls.n	1d4e <set_low_pwr_high_trigger+0x1a>
            if(read_data_batadc > LOW_PWR_HIGH_ADC_THRESH[i]) {
    1d46:	4a09      	ldr	r2, [pc, #36]	; (1d6c <set_low_pwr_high_trigger+0x38>)
    1d48:	005b      	lsls	r3, r3, #1
    1d4a:	5a98      	ldrh	r0, [r3, r2]
    1d4c:	e004      	b.n	1d58 <set_low_pwr_high_trigger+0x24>
    1d4e:	3301      	adds	r3, #1
// return true if PMU ADC output is higher than HIGH_THRESH
// return false if PMU ADC output is lower than HIGH_THRESH
// that means system should get out of LOW_PWR
uint16_t set_low_pwr_high_trigger() {
    uint16_t i;
    for(i = 0; i < 6; i++) {
    1d50:	2b06      	cmp	r3, #6
    1d52:	d1f3      	bne.n	1d3c <set_low_pwr_high_trigger+0x8>
            }
            return 0;
        }
    }
    // > 60C
    if(read_data_batadc > LOW_PWR_HIGH_ADC_THRESH[6]) {
    1d54:	4b05      	ldr	r3, [pc, #20]	; (1d6c <set_low_pwr_high_trigger+0x38>)
    1d56:	8998      	ldrh	r0, [r3, #12]
    1d58:	4b05      	ldr	r3, [pc, #20]	; (1d70 <set_low_pwr_high_trigger+0x3c>)
    1d5a:	881b      	ldrh	r3, [r3, #0]
uint16_t set_low_pwr_high_trigger() {
    uint16_t i;
    for(i = 0; i < 6; i++) {
        if(snt_sys_temp_code < PMU_TEMP_THRESH[i]) {
            if(read_data_batadc > LOW_PWR_HIGH_ADC_THRESH[i]) {
                return 1;
    1d5c:	4298      	cmp	r0, r3
    1d5e:	4180      	sbcs	r0, r0
    1d60:	4240      	negs	r0, r0
    // > 60C
    if(read_data_batadc > LOW_PWR_HIGH_ADC_THRESH[6]) {
        return 1;
    }
    return 0;
}
    1d62:	bd10      	pop	{r4, pc}
    1d64:	00003934 	.word	0x00003934
    1d68:	000038b8 	.word	0x000038b8
    1d6c:	000038f2 	.word	0x000038f2
    1d70:	000039e4 	.word	0x000039e4

Disassembly of section .text.send_beacon:

00001d74 <send_beacon>:

void send_beacon() {
    // v6.0.1 checking temp and voltage
    if(mrr_send_enable) {
    1d74:	4b05      	ldr	r3, [pc, #20]	; (1d8c <send_beacon+0x18>)
        return 1;
    }
    return 0;
}

void send_beacon() {
    1d76:	b510      	push	{r4, lr}
    // v6.0.1 checking temp and voltage
    if(mrr_send_enable) {
    1d78:	881b      	ldrh	r3, [r3, #0]
    1d7a:	2b00      	cmp	r3, #0
    1d7c:	d005      	beq.n	1d8a <send_beacon+0x16>
        pmu_setting_temp_based(1);
    1d7e:	2001      	movs	r0, #1
    1d80:	f7ff fbc8 	bl	1514 <pmu_setting_temp_based>
        // mrr_send_radio_data(1);
        mrr_send_radio_data(0); // FIXME: change this line in real code
    1d84:	2000      	movs	r0, #0
    1d86:	f7ff fc6b 	bl	1660 <mrr_send_radio_data>
        // pmu_setting_temp_based(0);   // FIXME: set this line back
    }
}
    1d8a:	bd10      	pop	{r4, pc}
    1d8c:	000038a0 	.word	0x000038a0

Disassembly of section .text.initialize_state_collect:

00001d90 <initialize_state_collect>:

// reduce repeated code
void initialize_state_collect() {
    // initialize day_state
    // starting from an edge 
    if(xo_day_time_in_sec < cur_sunrise - EDGE_MARGIN2 - XO_10_MIN) {
    1d90:	4b24      	ldr	r3, [pc, #144]	; (1e24 <initialize_state_collect+0x94>)
        // pmu_setting_temp_based(0);   // FIXME: set this line back
    }
}

// reduce repeated code
void initialize_state_collect() {
    1d92:	b570      	push	{r4, r5, r6, lr}
    // initialize day_state
    // starting from an edge 
    if(xo_day_time_in_sec < cur_sunrise - EDGE_MARGIN2 - XO_10_MIN) {
    1d94:	4a24      	ldr	r2, [pc, #144]	; (1e28 <initialize_state_collect+0x98>)
    1d96:	4c25      	ldr	r4, [pc, #148]	; (1e2c <initialize_state_collect+0x9c>)
    1d98:	681b      	ldr	r3, [r3, #0]
    1d9a:	4825      	ldr	r0, [pc, #148]	; (1e30 <initialize_state_collect+0xa0>)
    1d9c:	8812      	ldrh	r2, [r2, #0]
    1d9e:	6821      	ldr	r1, [r4, #0]
    1da0:	181b      	adds	r3, r3, r0
    1da2:	1a9b      	subs	r3, r3, r2
    1da4:	4a23      	ldr	r2, [pc, #140]	; (1e34 <initialize_state_collect+0xa4>)
    1da6:	428b      	cmp	r3, r1
    1da8:	d80b      	bhi.n	1dc2 <initialize_state_collect+0x32>
        day_state = DAWN;
    }
    else if(xo_day_time_in_sec < cur_sunset - EDGE_MARGIN1 - XO_10_MIN){
    1daa:	4b23      	ldr	r3, [pc, #140]	; (1e38 <initialize_state_collect+0xa8>)
    1dac:	4923      	ldr	r1, [pc, #140]	; (1e3c <initialize_state_collect+0xac>)
    1dae:	681b      	ldr	r3, [r3, #0]
    1db0:	4d1f      	ldr	r5, [pc, #124]	; (1e30 <initialize_state_collect+0xa0>)
    1db2:	8808      	ldrh	r0, [r1, #0]
    1db4:	6821      	ldr	r1, [r4, #0]
    1db6:	195b      	adds	r3, r3, r5
    1db8:	1a1b      	subs	r3, r3, r0
    1dba:	428b      	cmp	r3, r1
    1dbc:	d901      	bls.n	1dc2 <initialize_state_collect+0x32>
        day_state = DUSK;
    1dbe:	2302      	movs	r3, #2
    1dc0:	e000      	b.n	1dc4 <initialize_state_collect+0x34>
    }
    else {
        // trusting the wrap around from set_new_state()
        day_state = DAWN;
    1dc2:	2300      	movs	r3, #0
    }

    // set new state variables based on day_state
    // setting next sunrise and sunset so the system won't be confused
    next_sunrise = 0;
    1dc4:	2500      	movs	r5, #0
    else if(xo_day_time_in_sec < cur_sunset - EDGE_MARGIN1 - XO_10_MIN){
        day_state = DUSK;
    }
    else {
        // trusting the wrap around from set_new_state()
        day_state = DAWN;
    1dc6:	8013      	strh	r3, [r2, #0]
    }

    // set new state variables based on day_state
    // setting next sunrise and sunset so the system won't be confused
    next_sunrise = 0;
    1dc8:	4b1d      	ldr	r3, [pc, #116]	; (1e40 <initialize_state_collect+0xb0>)
    1dca:	601d      	str	r5, [r3, #0]
    next_sunset = 0;
    1dcc:	4b1d      	ldr	r3, [pc, #116]	; (1e44 <initialize_state_collect+0xb4>)
    1dce:	601d      	str	r5, [r3, #0]
    set_new_state();
    1dd0:	f7fe ff72 	bl	cb8 <set_new_state>

    // find suitable temp storage time based on day_state_start_time
    uint32_t store_temp_day_time = 0;
    1dd4:	002b      	movs	r3, r5

    // this variable should always reflect the last temp data stored
    store_temp_index = 0; 
    1dd6:	4a1c      	ldr	r2, [pc, #112]	; (1e48 <initialize_state_collect+0xb8>)
    while(store_temp_day_time < day_state_start_time) {
    1dd8:	481c      	ldr	r0, [pc, #112]	; (1e4c <initialize_state_collect+0xbc>)

    // find suitable temp storage time based on day_state_start_time
    uint32_t store_temp_day_time = 0;

    // this variable should always reflect the last temp data stored
    store_temp_index = 0; 
    1dda:	8015      	strh	r5, [r2, #0]
    while(store_temp_day_time < day_state_start_time) {
    1ddc:	6801      	ldr	r1, [r0, #0]
    1dde:	428b      	cmp	r3, r1
    1de0:	d207      	bcs.n	1df2 <initialize_state_collect+0x62>
        store_temp_day_time += XO_30_MIN;
    1de2:	21e1      	movs	r1, #225	; 0xe1
    1de4:	00c9      	lsls	r1, r1, #3
    1de6:	185b      	adds	r3, r3, r1
        store_temp_index++;
    1de8:	8811      	ldrh	r1, [r2, #0]
    1dea:	3101      	adds	r1, #1
    1dec:	b289      	uxth	r1, r1
    1dee:	8011      	strh	r1, [r2, #0]
    1df0:	e7f4      	b.n	1ddc <initialize_state_collect+0x4c>
    }
    // shift to index of last data stored
    store_temp_index--;
    1df2:	8811      	ldrh	r1, [r2, #0]
    1df4:	3901      	subs	r1, #1
    1df6:	b289      	uxth	r1, r1
    1df8:	8011      	strh	r1, [r2, #0]
    store_temp_timestamp = store_temp_day_time + xo_sys_time_in_sec - xo_day_time_in_sec;
    1dfa:	4a15      	ldr	r2, [pc, #84]	; (1e50 <initialize_state_collect+0xc0>)
    1dfc:	6812      	ldr	r2, [r2, #0]
    1dfe:	6821      	ldr	r1, [r4, #0]
    1e00:	1a52      	subs	r2, r2, r1
    1e02:	4914      	ldr	r1, [pc, #80]	; (1e54 <initialize_state_collect+0xc4>)
    1e04:	18d2      	adds	r2, r2, r3
    1e06:	600a      	str	r2, [r1, #0]
    // deal with wrap around issue
    if(store_temp_day_time < xo_day_time_in_sec) {
    1e08:	6822      	ldr	r2, [r4, #0]
    1e0a:	4293      	cmp	r3, r2
    1e0c:	d203      	bcs.n	1e16 <initialize_state_collect+0x86>
        store_temp_timestamp += MAX_DAY_TIME;
    1e0e:	680b      	ldr	r3, [r1, #0]
    1e10:	4a11      	ldr	r2, [pc, #68]	; (1e58 <initialize_state_collect+0xc8>)
    1e12:	189b      	adds	r3, r3, r2
    1e14:	600b      	str	r3, [r1, #0]
    }
    goc_state = STATE_COLLECT;
    1e16:	2201      	movs	r2, #1
    1e18:	4b10      	ldr	r3, [pc, #64]	; (1e5c <initialize_state_collect+0xcc>)
    1e1a:	801a      	strh	r2, [r3, #0]
    set_projected_end_time();
    1e1c:	f7ff faca 	bl	13b4 <set_projected_end_time>
}
    1e20:	bd70      	pop	{r4, r5, r6, pc}
    1e22:	46c0      	nop			; (mov r8, r8)
    1e24:	00003ac4 	.word	0x00003ac4
    1e28:	00003956 	.word	0x00003956
    1e2c:	00003a78 	.word	0x00003a78
    1e30:	fffffda8 	.word	0xfffffda8
    1e34:	00003930 	.word	0x00003930
    1e38:	00003b1c 	.word	0x00003b1c
    1e3c:	00003954 	.word	0x00003954
    1e40:	00003a30 	.word	0x00003a30
    1e44:	00003a44 	.word	0x00003a44
    1e48:	00003a58 	.word	0x00003a58
    1e4c:	0000398c 	.word	0x0000398c
    1e50:	00003acc 	.word	0x00003acc
    1e54:	00003a54 	.word	0x00003a54
    1e58:	00015180 	.word	0x00015180
    1e5c:	00003a1c 	.word	0x00003a1c

Disassembly of section .text.startup.main:

00001e60 <main>:

int main() {
    1e60:	b5f0      	push	{r4, r5, r6, r7, lr}
    // Only enable relevant interrupts (PREv18)
    *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
    1e62:	4bf8      	ldr	r3, [pc, #992]	; (2244 <main+0x3e4>)
    1e64:	4af8      	ldr	r2, [pc, #992]	; (2248 <main+0x3e8>)
          1 << IRQ_REG0 | 1 << IRQ_REG1 | 1 << IRQ_REG2 | 1 << IRQ_REG3);

    if(enumerated != ENUMID + VERSION_NUM) {
    1e66:	4cf9      	ldr	r4, [pc, #996]	; (224c <main+0x3ec>)
    set_projected_end_time();
}

int main() {
    // Only enable relevant interrupts (PREv18)
    *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
    1e68:	601a      	str	r2, [r3, #0]
          1 << IRQ_REG0 | 1 << IRQ_REG1 | 1 << IRQ_REG2 | 1 << IRQ_REG3);

    if(enumerated != ENUMID + VERSION_NUM) {
    1e6a:	6823      	ldr	r3, [r4, #0]
    1e6c:	4df8      	ldr	r5, [pc, #992]	; (2250 <main+0x3f0>)
    }
    goc_state = STATE_COLLECT;
    set_projected_end_time();
}

int main() {
    1e6e:	b085      	sub	sp, #20
    // Only enable relevant interrupts (PREv18)
    *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
          1 << IRQ_REG0 | 1 << IRQ_REG1 | 1 << IRQ_REG2 | 1 << IRQ_REG3);

    if(enumerated != ENUMID + VERSION_NUM) {
    1e70:	42ab      	cmp	r3, r5
    1e72:	d100      	bne.n	1e76 <main+0x16>
    1e74:	e2c0      	b.n	23f8 <main+0x598>

static void operation_init( void ) {
    // BREAKPOINT 0x01
    // mbus_write_message32(0xBA, 0x01);

    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    1e76:	2700      	movs	r7, #0
    1e78:	4bf6      	ldr	r3, [pc, #984]	; (2254 <main+0x3f4>)
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    config_timer32(0, 0, 0, 0);
    1e7a:	0039      	movs	r1, r7

static void operation_init( void ) {
    // BREAKPOINT 0x01
    // mbus_write_message32(0xBA, 0x01);

    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    1e7c:	601f      	str	r7, [r3, #0]
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    1e7e:	4bf6      	ldr	r3, [pc, #984]	; (2258 <main+0x3f8>)
    config_timer32(0, 0, 0, 0);
    1e80:	0038      	movs	r0, r7
static void operation_init( void ) {
    // BREAKPOINT 0x01
    // mbus_write_message32(0xBA, 0x01);

    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    1e82:	601f      	str	r7, [r3, #0]
    config_timer32(0, 0, 0, 0);
    1e84:	003a      	movs	r2, r7
    1e86:	003b      	movs	r3, r7
    1e88:	f7fe f994 	bl	1b4 <config_timer32>
    // bake version num into the code
    // so it will go through operation_init
    enumerated = ENUMID + VERSION_NUM;

    // timing variables don't belong to var_init
    xo_sys_time = 0;
    1e8c:	4bf3      	ldr	r3, [pc, #972]	; (225c <main+0x3fc>)
    config_timer32(0, 0, 0, 0);

    // Enumeration
    // bake version num into the code
    // so it will go through operation_init
    enumerated = ENUMID + VERSION_NUM;
    1e8e:	6025      	str	r5, [r4, #0]

    // timing variables don't belong to var_init
    xo_sys_time = 0;
    1e90:	601f      	str	r7, [r3, #0]
    xo_sys_time_in_sec = 0;
    1e92:	4bf3      	ldr	r3, [pc, #972]	; (2260 <main+0x400>)
    xo_day_time_in_sec = 0;

    xo_to_sec_mplier = 32768;
    1e94:	4af3      	ldr	r2, [pc, #972]	; (2264 <main+0x404>)
    // so it will go through operation_init
    enumerated = ENUMID + VERSION_NUM;

    // timing variables don't belong to var_init
    xo_sys_time = 0;
    xo_sys_time_in_sec = 0;
    1e96:	601f      	str	r7, [r3, #0]
    xo_day_time_in_sec = 0;
    1e98:	4bf3      	ldr	r3, [pc, #972]	; (2268 <main+0x408>)
    // BREAKPOINT 0x02
    // mbus_write_message32(0xBA, 0x02);
    // mbus_write_message32(0xED, PMU_ACTIVE_SETTINGS[0]);

#ifdef USE_SNT
    sntv5_r01.TSNS_BURST_MODE = 0;
    1e9a:	2640      	movs	r6, #64	; 0x40
    enumerated = ENUMID + VERSION_NUM;

    // timing variables don't belong to var_init
    xo_sys_time = 0;
    xo_sys_time_in_sec = 0;
    xo_day_time_in_sec = 0;
    1e9c:	601f      	str	r7, [r3, #0]

    xo_to_sec_mplier = 32768;
    1e9e:	4bf3      	ldr	r3, [pc, #972]	; (226c <main+0x40c>)
    mbus_remote_register_write(LNT_ADDR,0x22,lntv1a_r22.as_int);
    delay(MBUS_DELAY*10);
    
    lntv1a_r21.TMR_SEL_CAP = 0x80; // Default : 8'h8
    lntv1a_r21.TMR_SEL_DCAP = 0x3F; // Default : 6'h4
    lntv1a_r21.TMR_EN_TUNE1 = 0x1; // Default : 1'h1
    1ea0:	2402      	movs	r4, #2
    // timing variables don't belong to var_init
    xo_sys_time = 0;
    xo_sys_time_in_sec = 0;
    xo_day_time_in_sec = 0;

    xo_to_sec_mplier = 32768;
    1ea2:	801a      	strh	r2, [r3, #0]
    XO_TO_SEC_MPLIER_SHIFT = 15;
    1ea4:	220f      	movs	r2, #15
    1ea6:	4bf2      	ldr	r3, [pc, #968]	; (2270 <main+0x410>)
    lntv1a_r22.TMR_DIFF_CON = 0x3FFD; // Default: 0x3FFB
    lntv1a_r22.TMR_POLY_CON = 0x1; // Default: 0x1
    mbus_remote_register_write(LNT_ADDR,0x22,lntv1a_r22.as_int);
    delay(MBUS_DELAY*10);
    
    lntv1a_r21.TMR_SEL_CAP = 0x80; // Default : 8'h8
    1ea8:	4df2      	ldr	r5, [pc, #968]	; (2274 <main+0x414>)
    xo_sys_time = 0;
    xo_sys_time_in_sec = 0;
    xo_day_time_in_sec = 0;

    xo_to_sec_mplier = 32768;
    XO_TO_SEC_MPLIER_SHIFT = 15;
    1eaa:	801a      	strh	r2, [r3, #0]
    xo_lnt_mplier = 0x73;
    1eac:	2273      	movs	r2, #115	; 0x73
    1eae:	4bf2      	ldr	r3, [pc, #968]	; (2278 <main+0x418>)
    1eb0:	801a      	strh	r2, [r3, #0]
    LNT_MPLIER_SHIFT = 6;
    1eb2:	4bf2      	ldr	r3, [pc, #968]	; (227c <main+0x41c>)
    1eb4:	3a6d      	subs	r2, #109	; 0x6d
    1eb6:	801a      	strh	r2, [r3, #0]

    var_init();
    1eb8:	f7fe f9dc 	bl	274 <var_init>

#ifdef USE_MEM
    mbus_enumerate(MEM_ADDR);
    1ebc:	2006      	movs	r0, #6
    1ebe:	f7fe f911 	bl	e4 <mbus_enumerate>
    delay(MBUS_DELAY);
    1ec2:	2064      	movs	r0, #100	; 0x64
    1ec4:	f7fe f96c 	bl	1a0 <delay>
#endif
#ifdef USE_MRR
    mbus_enumerate(MRR_ADDR);
    1ec8:	2002      	movs	r0, #2
    1eca:	f7fe f90b 	bl	e4 <mbus_enumerate>
    delay(MBUS_DELAY);
    1ece:	2064      	movs	r0, #100	; 0x64
    1ed0:	f7fe f966 	bl	1a0 <delay>
#endif
#ifdef USE_LNT
    mbus_enumerate(LNT_ADDR);
    1ed4:	2003      	movs	r0, #3
    1ed6:	f7fe f905 	bl	e4 <mbus_enumerate>
    delay(MBUS_DELAY);
    1eda:	2064      	movs	r0, #100	; 0x64
    1edc:	f7fe f960 	bl	1a0 <delay>
#endif
#ifdef USE_SNT
    mbus_enumerate(SNT_ADDR);
    1ee0:	2004      	movs	r0, #4
    1ee2:	f7fe f8ff 	bl	e4 <mbus_enumerate>
    delay(MBUS_DELAY);
    1ee6:	2064      	movs	r0, #100	; 0x64
    1ee8:	f7fe f95a 	bl	1a0 <delay>
#endif
#ifdef USE_PMU
    mbus_enumerate(PMU_ADDR);
    1eec:	2005      	movs	r0, #5
    1eee:	f7fe f8f9 	bl	e4 <mbus_enumerate>
    delay(MBUS_DELAY);
    1ef2:	2064      	movs	r0, #100	; 0x64
    1ef4:	f7fe f954 	bl	1a0 <delay>
#endif

    // Default CPU halt function
    set_halt_until_mbus_tx();
    1ef8:	f7fe f9b0 	bl	25c <set_halt_until_mbus_tx>
    // mbus_write_message32(0xBA, 0x02);
    // mbus_write_message32(0xED, PMU_ACTIVE_SETTINGS[0]);

#ifdef USE_SNT
    sntv5_r01.TSNS_BURST_MODE = 0;
    sntv5_r01.TSNS_CONT_MODE  = 0;
    1efc:	2180      	movs	r1, #128	; 0x80
    // BREAKPOINT 0x02
    // mbus_write_message32(0xBA, 0x02);
    // mbus_write_message32(0xED, PMU_ACTIVE_SETTINGS[0]);

#ifdef USE_SNT
    sntv5_r01.TSNS_BURST_MODE = 0;
    1efe:	4be0      	ldr	r3, [pc, #896]	; (2280 <main+0x420>)
    sntv5_r01.TSNS_CONT_MODE  = 0;
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    1f00:	2004      	movs	r0, #4
    // BREAKPOINT 0x02
    // mbus_write_message32(0xBA, 0x02);
    // mbus_write_message32(0xED, PMU_ACTIVE_SETTINGS[0]);

#ifdef USE_SNT
    sntv5_r01.TSNS_BURST_MODE = 0;
    1f02:	881a      	ldrh	r2, [r3, #0]
    1f04:	43b2      	bics	r2, r6
    1f06:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_CONT_MODE  = 0;
    1f08:	881a      	ldrh	r2, [r3, #0]
    1f0a:	438a      	bics	r2, r1
    1f0c:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);
    1f0e:	681a      	ldr	r2, [r3, #0]
    1f10:	397f      	subs	r1, #127	; 0x7f
    1f12:	f7fe f908 	bl	126 <mbus_remote_register_write>

    sntv5_r07.TSNS_INT_RPLY_SHORT_ADDR = 0x10;
    1f16:	21ff      	movs	r1, #255	; 0xff
    1f18:	4ada      	ldr	r2, [pc, #872]	; (2284 <main+0x424>)
    sntv5_r07.TSNS_INT_RPLY_REG_ADDR   = 0x00;
    mbus_remote_register_write(SNT_ADDR, 7, sntv5_r07.as_int);
    1f1a:	2004      	movs	r0, #4
#ifdef USE_SNT
    sntv5_r01.TSNS_BURST_MODE = 0;
    sntv5_r01.TSNS_CONT_MODE  = 0;
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);

    sntv5_r07.TSNS_INT_RPLY_SHORT_ADDR = 0x10;
    1f1c:	8813      	ldrh	r3, [r2, #0]
    1f1e:	400b      	ands	r3, r1
    1f20:	2180      	movs	r1, #128	; 0x80
    1f22:	0149      	lsls	r1, r1, #5
    1f24:	430b      	orrs	r3, r1
    sntv5_r07.TSNS_INT_RPLY_REG_ADDR   = 0x00;
    1f26:	21ff      	movs	r1, #255	; 0xff
#ifdef USE_SNT
    sntv5_r01.TSNS_BURST_MODE = 0;
    sntv5_r01.TSNS_CONT_MODE  = 0;
    mbus_remote_register_write(SNT_ADDR, 1, sntv5_r01.as_int);

    sntv5_r07.TSNS_INT_RPLY_SHORT_ADDR = 0x10;
    1f28:	8013      	strh	r3, [r2, #0]
    sntv5_r07.TSNS_INT_RPLY_REG_ADDR   = 0x00;
    1f2a:	8813      	ldrh	r3, [r2, #0]
    1f2c:	438b      	bics	r3, r1
    1f2e:	8013      	strh	r3, [r2, #0]
    mbus_remote_register_write(SNT_ADDR, 7, sntv5_r07.as_int);
    1f30:	6812      	ldr	r2, [r2, #0]
    1f32:	39f8      	subs	r1, #248	; 0xf8
    1f34:	f7fe f8f7 	bl	126 <mbus_remote_register_write>

    snt_state = SNT_IDLE;
    1f38:	4bd3      	ldr	r3, [pc, #844]	; (2288 <main+0x428>)
    1f3a:	801f      	strh	r7, [r3, #0]
    operation_temp_run();
    1f3c:	f7ff fd2c 	bl	1998 <operation_temp_run>
static void lnt_init() {
    //////// TIMER OPERATION //////////
    // make variables local to save space
    lntv1a_r01_t lntv1a_r01 = LNTv1A_R01_DEFAULT;
    lntv1a_r02_t lntv1a_r02 = LNTv1A_R02_DEFAULT;
    lntv1a_r04_t lntv1a_r04 = LNTv1A_R04_DEFAULT;
    1f40:	210a      	movs	r1, #10
 **********************************************/

static void lnt_init() {
    //////// TIMER OPERATION //////////
    // make variables local to save space
    lntv1a_r01_t lntv1a_r01 = LNTv1A_R01_DEFAULT;
    1f42:	4bd2      	ldr	r3, [pc, #840]	; (228c <main+0x42c>)
    
    // TIMER TUNING  
    lntv1a_r22.TMR_S = 0x1; // Default: 0x4
    lntv1a_r22.TMR_DIFF_CON = 0x3FFD; // Default: 0x3FFB
    lntv1a_r22.TMR_POLY_CON = 0x1; // Default: 0x1
    mbus_remote_register_write(LNT_ADDR,0x22,lntv1a_r22.as_int);
    1f44:	2003      	movs	r0, #3
 **********************************************/

static void lnt_init() {
    //////// TIMER OPERATION //////////
    // make variables local to save space
    lntv1a_r01_t lntv1a_r01 = LNTv1A_R01_DEFAULT;
    1f46:	685a      	ldr	r2, [r3, #4]
    1f48:	9200      	str	r2, [sp, #0]
    lntv1a_r02_t lntv1a_r02 = LNTv1A_R02_DEFAULT;
    lntv1a_r04_t lntv1a_r04 = LNTv1A_R04_DEFAULT;
    1f4a:	4ad1      	ldr	r2, [pc, #836]	; (2290 <main+0x430>)
    1f4c:	4011      	ands	r1, r2
    lntv1a_r05_t lntv1a_r05 = LNTv1A_R05_DEFAULT;
    // lntv1a_r06_t lntv1a_r06 = LNTv1A_R06_DEFAULT;
    lntv1a_r17_t lntv1a_r17 = LNTv1A_R17_DEFAULT;
    lntv1a_r20_t lntv1a_r20 = LNTv1A_R20_DEFAULT;
    lntv1a_r21_t lntv1a_r21 = LNTv1A_R21_DEFAULT;
    1f4e:	689a      	ldr	r2, [r3, #8]
    lntv1a_r22_t lntv1a_r22 = LNTv1A_R22_DEFAULT;
    1f50:	68db      	ldr	r3, [r3, #12]
    lntv1a_r04_t lntv1a_r04 = LNTv1A_R04_DEFAULT;
    lntv1a_r05_t lntv1a_r05 = LNTv1A_R05_DEFAULT;
    // lntv1a_r06_t lntv1a_r06 = LNTv1A_R06_DEFAULT;
    lntv1a_r17_t lntv1a_r17 = LNTv1A_R17_DEFAULT;
    lntv1a_r20_t lntv1a_r20 = LNTv1A_R20_DEFAULT;
    lntv1a_r21_t lntv1a_r21 = LNTv1A_R21_DEFAULT;
    1f52:	9201      	str	r2, [sp, #4]
    lntv1a_r22_t lntv1a_r22 = LNTv1A_R22_DEFAULT;
    lntv1a_r40_t lntv1a_r40 = LNTv1A_R40_DEFAULT;
    
    // TIMER TUNING  
    lntv1a_r22.TMR_S = 0x1; // Default: 0x4
    1f54:	4acf      	ldr	r2, [pc, #828]	; (2294 <main+0x434>)
    lntv1a_r05_t lntv1a_r05 = LNTv1A_R05_DEFAULT;
    // lntv1a_r06_t lntv1a_r06 = LNTv1A_R06_DEFAULT;
    lntv1a_r17_t lntv1a_r17 = LNTv1A_R17_DEFAULT;
    lntv1a_r20_t lntv1a_r20 = LNTv1A_R20_DEFAULT;
    lntv1a_r21_t lntv1a_r21 = LNTv1A_R21_DEFAULT;
    lntv1a_r22_t lntv1a_r22 = LNTv1A_R22_DEFAULT;
    1f56:	9302      	str	r3, [sp, #8]
    lntv1a_r40_t lntv1a_r40 = LNTv1A_R40_DEFAULT;
    
    // TIMER TUNING  
    lntv1a_r22.TMR_S = 0x1; // Default: 0x4
    1f58:	401a      	ands	r2, r3
    1f5a:	2380      	movs	r3, #128	; 0x80
    1f5c:	039b      	lsls	r3, r3, #14
    1f5e:	4313      	orrs	r3, r2
    lntv1a_r22.TMR_DIFF_CON = 0x3FFD; // Default: 0x3FFB
    1f60:	4acd      	ldr	r2, [pc, #820]	; (2298 <main+0x438>)
static void lnt_init() {
    //////// TIMER OPERATION //////////
    // make variables local to save space
    lntv1a_r01_t lntv1a_r01 = LNTv1A_R01_DEFAULT;
    lntv1a_r02_t lntv1a_r02 = LNTv1A_R02_DEFAULT;
    lntv1a_r04_t lntv1a_r04 = LNTv1A_R04_DEFAULT;
    1f62:	9103      	str	r1, [sp, #12]
    lntv1a_r22_t lntv1a_r22 = LNTv1A_R22_DEFAULT;
    lntv1a_r40_t lntv1a_r40 = LNTv1A_R40_DEFAULT;
    
    // TIMER TUNING  
    lntv1a_r22.TMR_S = 0x1; // Default: 0x4
    lntv1a_r22.TMR_DIFF_CON = 0x3FFD; // Default: 0x3FFB
    1f64:	401a      	ands	r2, r3
    1f66:	4bcd      	ldr	r3, [pc, #820]	; (229c <main+0x43c>)
    lntv1a_r22.TMR_POLY_CON = 0x1; // Default: 0x1
    mbus_remote_register_write(LNT_ADDR,0x22,lntv1a_r22.as_int);
    1f68:	2122      	movs	r1, #34	; 0x22
    lntv1a_r22_t lntv1a_r22 = LNTv1A_R22_DEFAULT;
    lntv1a_r40_t lntv1a_r40 = LNTv1A_R40_DEFAULT;
    
    // TIMER TUNING  
    lntv1a_r22.TMR_S = 0x1; // Default: 0x4
    lntv1a_r22.TMR_DIFF_CON = 0x3FFD; // Default: 0x3FFB
    1f6a:	431a      	orrs	r2, r3
    lntv1a_r22.TMR_POLY_CON = 0x1; // Default: 0x1
    1f6c:	4332      	orrs	r2, r6
    mbus_remote_register_write(LNT_ADDR,0x22,lntv1a_r22.as_int);
    1f6e:	f7fe f8da 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    1f72:	20fa      	movs	r0, #250	; 0xfa
    1f74:	0080      	lsls	r0, r0, #2
    1f76:	f7fe f913 	bl	1a0 <delay>
    
    lntv1a_r21.TMR_SEL_CAP = 0x80; // Default : 8'h8
    lntv1a_r21.TMR_SEL_DCAP = 0x3F; // Default : 6'h4
    lntv1a_r21.TMR_EN_TUNE1 = 0x1; // Default : 1'h1
    lntv1a_r21.TMR_EN_TUNE2 = 0x1; // Default : 1'h1
    1f7a:	2301      	movs	r3, #1
    lntv1a_r22.TMR_DIFF_CON = 0x3FFD; // Default: 0x3FFB
    lntv1a_r22.TMR_POLY_CON = 0x1; // Default: 0x1
    mbus_remote_register_write(LNT_ADDR,0x22,lntv1a_r22.as_int);
    delay(MBUS_DELAY*10);
    
    lntv1a_r21.TMR_SEL_CAP = 0x80; // Default : 8'h8
    1f7c:	9a01      	ldr	r2, [sp, #4]
    lntv1a_r21.TMR_SEL_DCAP = 0x3F; // Default : 6'h4
    lntv1a_r21.TMR_EN_TUNE1 = 0x1; // Default : 1'h1
    lntv1a_r21.TMR_EN_TUNE2 = 0x1; // Default : 1'h1
    mbus_remote_register_write(LNT_ADDR,0x21,lntv1a_r21.as_int);
    1f7e:	2121      	movs	r1, #33	; 0x21
    lntv1a_r22.TMR_DIFF_CON = 0x3FFD; // Default: 0x3FFB
    lntv1a_r22.TMR_POLY_CON = 0x1; // Default: 0x1
    mbus_remote_register_write(LNT_ADDR,0x22,lntv1a_r22.as_int);
    delay(MBUS_DELAY*10);
    
    lntv1a_r21.TMR_SEL_CAP = 0x80; // Default : 8'h8
    1f80:	402a      	ands	r2, r5
    lntv1a_r21.TMR_SEL_DCAP = 0x3F; // Default : 6'h4
    1f82:	4dc7      	ldr	r5, [pc, #796]	; (22a0 <main+0x440>)
    lntv1a_r21.TMR_EN_TUNE1 = 0x1; // Default : 1'h1
    lntv1a_r21.TMR_EN_TUNE2 = 0x1; // Default : 1'h1
    mbus_remote_register_write(LNT_ADDR,0x21,lntv1a_r21.as_int);
    1f84:	2003      	movs	r0, #3
    lntv1a_r22.TMR_POLY_CON = 0x1; // Default: 0x1
    mbus_remote_register_write(LNT_ADDR,0x22,lntv1a_r22.as_int);
    delay(MBUS_DELAY*10);
    
    lntv1a_r21.TMR_SEL_CAP = 0x80; // Default : 8'h8
    lntv1a_r21.TMR_SEL_DCAP = 0x3F; // Default : 6'h4
    1f86:	4315      	orrs	r5, r2
    lntv1a_r21.TMR_EN_TUNE1 = 0x1; // Default : 1'h1
    1f88:	4325      	orrs	r5, r4
    lntv1a_r21.TMR_EN_TUNE2 = 0x1; // Default : 1'h1
    1f8a:	431d      	orrs	r5, r3
    mbus_remote_register_write(LNT_ADDR,0x21,lntv1a_r21.as_int);
    1f8c:	002a      	movs	r2, r5
    1f8e:	f7fe f8ca 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    1f92:	20fa      	movs	r0, #250	; 0xfa
    1f94:	0080      	lsls	r0, r0, #2
    1f96:	f7fe f903 	bl	1a0 <delay>
    
    // Enable Frequency Monitoring 
    lntv1a_r40.WUP_ENABLE_CLK_SLP_OUT = 0x0; 
    mbus_remote_register_write(LNT_ADDR,0x40,lntv1a_r40.as_int);
    1f9a:	22c0      	movs	r2, #192	; 0xc0
    1f9c:	0031      	movs	r1, r6
    1f9e:	03d2      	lsls	r2, r2, #15
    1fa0:	2003      	movs	r0, #3
    1fa2:	f7fe f8c0 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    1fa6:	20fa      	movs	r0, #250	; 0xfa
    
    // TIMER SELF_EN Disable 
    lntv1a_r21.TMR_SELF_EN = 0x0; // Default : 0x1
    1fa8:	4ebe      	ldr	r6, [pc, #760]	; (22a4 <main+0x444>)
    delay(MBUS_DELAY*10);
    
    // Enable Frequency Monitoring 
    lntv1a_r40.WUP_ENABLE_CLK_SLP_OUT = 0x0; 
    mbus_remote_register_write(LNT_ADDR,0x40,lntv1a_r40.as_int);
    delay(MBUS_DELAY*10);
    1faa:	0080      	lsls	r0, r0, #2
    
    // TIMER SELF_EN Disable 
    lntv1a_r21.TMR_SELF_EN = 0x0; // Default : 0x1
    1fac:	4035      	ands	r5, r6
    delay(MBUS_DELAY*10);
    
    // Enable Frequency Monitoring 
    lntv1a_r40.WUP_ENABLE_CLK_SLP_OUT = 0x0; 
    mbus_remote_register_write(LNT_ADDR,0x40,lntv1a_r40.as_int);
    delay(MBUS_DELAY*10);
    1fae:	f7fe f8f7 	bl	1a0 <delay>
    
    // TIMER SELF_EN Disable 
    lntv1a_r21.TMR_SELF_EN = 0x0; // Default : 0x1
    mbus_remote_register_write(LNT_ADDR,0x21,lntv1a_r21.as_int);
    1fb2:	002a      	movs	r2, r5
    1fb4:	2121      	movs	r1, #33	; 0x21
    1fb6:	2003      	movs	r0, #3
    1fb8:	f7fe f8b5 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    1fbc:	20fa      	movs	r0, #250	; 0xfa
    1fbe:	0080      	lsls	r0, r0, #2
    1fc0:	f7fe f8ee 	bl	1a0 <delay>
    
    // EN_OSC 
    lntv1a_r20.TMR_EN_OSC = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);
    1fc4:	2208      	movs	r2, #8
    1fc6:	2120      	movs	r1, #32
    1fc8:	2003      	movs	r0, #3
    1fca:	f7fe f8ac 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    1fce:	20fa      	movs	r0, #250	; 0xfa
    1fd0:	0080      	lsls	r0, r0, #2
    1fd2:	f7fe f8e5 	bl	1a0 <delay>
    
    // Release Reset 
    lntv1a_r20.TMR_RESETB = 0x1; // Default : 0x0
    lntv1a_r20.TMR_RESETB_DIV = 0x1; // Default : 0x0
    lntv1a_r20.TMR_RESETB_DCDC = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);
    1fd6:	221e      	movs	r2, #30
    1fd8:	2120      	movs	r1, #32
    1fda:	2003      	movs	r0, #3
    1fdc:	f7fe f8a3 	bl	126 <mbus_remote_register_write>
    delay(2000); 
    1fe0:	20fa      	movs	r0, #250	; 0xfa
    1fe2:	00c0      	lsls	r0, r0, #3
    1fe4:	f7fe f8dc 	bl	1a0 <delay>
    
    // TIMER EN_SEL_CLK Reset 
    lntv1a_r20.TMR_EN_SELF_CLK = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);
    1fe8:	221f      	movs	r2, #31
    1fea:	2120      	movs	r1, #32
    1fec:	2003      	movs	r0, #3
    1fee:	f7fe f89a 	bl	126 <mbus_remote_register_write>
    delay(10); 
    1ff2:	200a      	movs	r0, #10
    1ff4:	f7fe f8d4 	bl	1a0 <delay>
    
    // TIMER SELF_EN 
    lntv1a_r21.TMR_SELF_EN = 0x1; // Default : 0x0
    1ff8:	2280      	movs	r2, #128	; 0x80
    1ffa:	0392      	lsls	r2, r2, #14
    1ffc:	432a      	orrs	r2, r5
    mbus_remote_register_write(LNT_ADDR,0x21,lntv1a_r21.as_int);
    1ffe:	2121      	movs	r1, #33	; 0x21
    2000:	2003      	movs	r0, #3
    2002:	f7fe f890 	bl	126 <mbus_remote_register_write>
    delay(100000); 
    2006:	48a8      	ldr	r0, [pc, #672]	; (22a8 <main+0x448>)
    2008:	f7fe f8ca 	bl	1a0 <delay>
    
    // TIMER EN_SEL_CLK Reset 
    lntv1a_r20.TMR_EN_OSC = 0x0; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);
    200c:	2217      	movs	r2, #23
    200e:	2120      	movs	r1, #32
    2010:	2003      	movs	r0, #3
    2012:	f7fe f888 	bl	126 <mbus_remote_register_write>
    delay(100);
    2016:	2064      	movs	r0, #100	; 0x64
    2018:	f7fe f8c2 	bl	1a0 <delay>
    
    //////// CLOCK DIVIDER OPERATION //////////
    
    // Run FDIV
    lntv1a_r17.FDIV_RESETN = 0x1; // Default : 0x0
    lntv1a_r17.FDIV_CTRL_FREQ = 0x8; // Default : 0x0
    201c:	231e      	movs	r3, #30
    201e:	2201      	movs	r2, #1
    2020:	439a      	bics	r2, r3
    2022:	3b0e      	subs	r3, #14
    2024:	431a      	orrs	r2, r3
    mbus_remote_register_write(LNT_ADDR,0x17,lntv1a_r17.as_int);
    2026:	2117      	movs	r1, #23
    2028:	2003      	movs	r0, #3
    202a:	f7fe f87c 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    202e:	20fa      	movs	r0, #250	; 0xfa
    2030:	0080      	lsls	r0, r0, #2
    2032:	f7fe f8b5 	bl	1a0 <delay>
    
    //////// LNT SETTING //////////
    
    // Bias Current
    lntv1a_r01.CTRL_IBIAS_VBIAS = 0x7; // Default : 0x7
    2036:	23f0      	movs	r3, #240	; 0xf0
    2038:	9a00      	ldr	r2, [sp, #0]
    lntv1a_r01.CTRL_IBIAS_I = 0x2; // Default : 0x8
    lntv1a_r01.CTRL_VOFS_CANCEL = 0x1; // Default : 0x1
    mbus_remote_register_write(LNT_ADDR,0x01,lntv1a_r01.as_int);
    203a:	2101      	movs	r1, #1
    delay(MBUS_DELAY*10);
    
    //////// LNT SETTING //////////
    
    // Bias Current
    lntv1a_r01.CTRL_IBIAS_VBIAS = 0x7; // Default : 0x7
    203c:	439a      	bics	r2, r3
    203e:	3b80      	subs	r3, #128	; 0x80
    2040:	431a      	orrs	r2, r3
    lntv1a_r01.CTRL_IBIAS_I = 0x2; // Default : 0x8
    2042:	3b61      	subs	r3, #97	; 0x61
    2044:	439a      	bics	r2, r3
    2046:	4322      	orrs	r2, r4
    lntv1a_r01.CTRL_VOFS_CANCEL = 0x1; // Default : 0x1
    2048:	33f1      	adds	r3, #241	; 0xf1
    204a:	431a      	orrs	r2, r3
    mbus_remote_register_write(LNT_ADDR,0x01,lntv1a_r01.as_int);
    204c:	2003      	movs	r0, #3
    204e:	f7fe f86a 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    2052:	20fa      	movs	r0, #250	; 0xfa
    2054:	0080      	lsls	r0, r0, #2
    2056:	f7fe f8a3 	bl	1a0 <delay>
         
    // Vbase regulation voltage
    lntv1a_r02.CTRL_VREF_PV_V = 0x1; // Default : 0x2
    205a:	2380      	movs	r3, #128	; 0x80
    205c:	4a93      	ldr	r2, [pc, #588]	; (22ac <main+0x44c>)
    mbus_remote_register_write(LNT_ADDR,0x02,lntv1a_r02.as_int);
    205e:	0021      	movs	r1, r4
    lntv1a_r01.CTRL_VOFS_CANCEL = 0x1; // Default : 0x1
    mbus_remote_register_write(LNT_ADDR,0x01,lntv1a_r01.as_int);
    delay(MBUS_DELAY*10);
         
    // Vbase regulation voltage
    lntv1a_r02.CTRL_VREF_PV_V = 0x1; // Default : 0x2
    2060:	431a      	orrs	r2, r3
    mbus_remote_register_write(LNT_ADDR,0x02,lntv1a_r02.as_int);
    2062:	2003      	movs	r0, #3
    2064:	f7fe f85f 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    2068:	20fa      	movs	r0, #250	; 0xfa
    206a:	0080      	lsls	r0, r0, #2
    206c:	f7fe f898 	bl	1a0 <delay>
        
    // Set LNT Threshold
    lntv1a_r05.THRESHOLD_HIGH = 0x30; // Default : 12'd40
    lntv1a_r05.THRESHOLD_LOW = 0x10; // Default : 12'd20
    2070:	22c0      	movs	r2, #192	; 0xc0
    2072:	2310      	movs	r3, #16
    2074:	0292      	lsls	r2, r2, #10
    2076:	431a      	orrs	r2, r3
    mbus_remote_register_write(LNT_ADDR,0x05,lntv1a_r05.as_int);
    2078:	2105      	movs	r1, #5
    207a:	2003      	movs	r0, #3
    207c:	f7fe f853 	bl	126 <mbus_remote_register_write>
    
    // Monitor AFEOUT
    // lntv1a_r06.OBSEN_AFEOUT = 0x0; // Default : 0x0
    // mbus_remote_register_write(LNT_ADDR,0x06,lntv1a_r06.as_int);
    mbus_remote_register_write(LNT_ADDR,0x06,0);
    2080:	003a      	movs	r2, r7
    2082:	2106      	movs	r1, #6
    2084:	2003      	movs	r0, #3
    2086:	f7fe f84e 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    208a:	20fa      	movs	r0, #250	; 0xfa
    208c:	0080      	lsls	r0, r0, #2
    208e:	f7fe f887 	bl	1a0 <delay>
    
    // Change Counting Time 
    // lntv1a_r03.TIME_COUNTING = 0xFFFFFF; // Default : 0x258
    mbus_remote_register_write(LNT_ADDR,0x03,0xFFFFFF);
    2092:	2103      	movs	r1, #3
    2094:	4a86      	ldr	r2, [pc, #536]	; (22b0 <main+0x450>)
    2096:	0008      	movs	r0, r1
    2098:	f7fe f845 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    209c:	20fa      	movs	r0, #250	; 0xfa
    209e:	0080      	lsls	r0, r0, #2
    20a0:	f7fe f87e 	bl	1a0 <delay>
    
    // Change Monitoring & Hold Time 
    lntv1a_r04.TIME_MONITORING = 0x00A; // Default : 0x010
    mbus_remote_register_write(LNT_ADDR,0x04,lntv1a_r04.as_int);
    20a4:	9a03      	ldr	r2, [sp, #12]
    20a6:	2104      	movs	r1, #4
    20a8:	2003      	movs	r0, #3
    20aa:	f7fe f83c 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    20ae:	20fa      	movs	r0, #250	; 0xfa
    20b0:	0080      	lsls	r0, r0, #2
    20b2:	f7fe f875 	bl	1a0 <delay>
    
    // Release LDC_PG 
    lntv1a_r00.LDC_PG = 0x0; // Default : 0x1
    20b6:	2201      	movs	r2, #1
    20b8:	4d7e      	ldr	r5, [pc, #504]	; (22b4 <main+0x454>)
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    20ba:	0039      	movs	r1, r7
    lntv1a_r04.TIME_MONITORING = 0x00A; // Default : 0x010
    mbus_remote_register_write(LNT_ADDR,0x04,lntv1a_r04.as_int);
    delay(MBUS_DELAY*10);
    
    // Release LDC_PG 
    lntv1a_r00.LDC_PG = 0x0; // Default : 0x1
    20bc:	782b      	ldrb	r3, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    20be:	2003      	movs	r0, #3
    lntv1a_r04.TIME_MONITORING = 0x00A; // Default : 0x010
    mbus_remote_register_write(LNT_ADDR,0x04,lntv1a_r04.as_int);
    delay(MBUS_DELAY*10);
    
    // Release LDC_PG 
    lntv1a_r00.LDC_PG = 0x0; // Default : 0x1
    20c0:	4393      	bics	r3, r2
    20c2:	702b      	strb	r3, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    20c4:	682a      	ldr	r2, [r5, #0]
    20c6:	f7fe f82e 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    20ca:	20fa      	movs	r0, #250	; 0xfa
    20cc:	0080      	lsls	r0, r0, #2
    20ce:	f7fe f867 	bl	1a0 <delay>
    
    // Release LDC_ISOLATE
    lntv1a_r00.LDC_ISOLATE = 0x0; // Default : 0x1
    20d2:	782b      	ldrb	r3, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    20d4:	0039      	movs	r1, r7
    lntv1a_r00.LDC_PG = 0x0; // Default : 0x1
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*10);
    
    // Release LDC_ISOLATE
    lntv1a_r00.LDC_ISOLATE = 0x0; // Default : 0x1
    20d6:	43a3      	bics	r3, r4
    20d8:	702b      	strb	r3, [r5, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    20da:	682a      	ldr	r2, [r5, #0]
    20dc:	2003      	movs	r0, #3
    20de:	f7fe f822 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    20e2:	20fa      	movs	r0, #250	; 0xfa
    20e4:	0080      	lsls	r0, r0, #2
    20e6:	f7fe f85b 	bl	1a0 <delay>
    // MRR Settings --------------------------------------
    // using local variables to save space
    mrrv11a_r02_t mrrv11a_r02 = MRRv11A_R02_DEFAULT;
    mrrv11a_r14_t mrrv11a_r14 = MRRv11A_R14_DEFAULT;
    mrrv11a_r15_t mrrv11a_r15 = MRRv11A_R15_DEFAULT;
    mrrv11a_r1F_t mrrv11a_r1F = MRRv11A_R1F_DEFAULT;
    20ea:	2203      	movs	r2, #3
    20ec:	230c      	movs	r3, #12
    20ee:	4313      	orrs	r3, r2
    20f0:	320d      	adds	r2, #13
    // Required in MRRv11A to run like MRRv7
    //mrrv11a_r21.TRX_ENb_CONT_RC = 0;  //RX_Decap P 
    //mbus_remote_register_write(MRR_ADDR,0x21,mrrv11a_r21.as_int);
    
    // Decap in series
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    20f2:	4d71      	ldr	r5, [pc, #452]	; (22b8 <main+0x458>)
    // MRR Settings --------------------------------------
    // using local variables to save space
    mrrv11a_r02_t mrrv11a_r02 = MRRv11A_R02_DEFAULT;
    mrrv11a_r14_t mrrv11a_r14 = MRRv11A_R14_DEFAULT;
    mrrv11a_r15_t mrrv11a_r15 = MRRv11A_R15_DEFAULT;
    mrrv11a_r1F_t mrrv11a_r1F = MRRv11A_R1F_DEFAULT;
    20f4:	4393      	bics	r3, r2
    20f6:	9300      	str	r3, [sp, #0]
    // Required in MRRv11A to run like MRRv7
    //mrrv11a_r21.TRX_ENb_CONT_RC = 0;  //RX_Decap P 
    //mbus_remote_register_write(MRR_ADDR,0x21,mrrv11a_r21.as_int);
    
    // Decap in series
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    20f8:	682b      	ldr	r3, [r5, #0]
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    20fa:	0020      	movs	r0, r4
    // Required in MRRv11A to run like MRRv7
    //mrrv11a_r21.TRX_ENb_CONT_RC = 0;  //RX_Decap P 
    //mbus_remote_register_write(MRR_ADDR,0x21,mrrv11a_r21.as_int);
    
    // Decap in series
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    20fc:	401e      	ands	r6, r3
    20fe:	602e      	str	r6, [r5, #0]
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    2100:	682a      	ldr	r2, [r5, #0]
    2102:	4b6e      	ldr	r3, [pc, #440]	; (22bc <main+0x45c>)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    2104:	2103      	movs	r1, #3
    //mrrv11a_r21.TRX_ENb_CONT_RC = 0;  //RX_Decap P 
    //mbus_remote_register_write(MRR_ADDR,0x21,mrrv11a_r21.as_int);
    
    // Decap in series
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    2106:	4013      	ands	r3, r2
    2108:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    210a:	682a      	ldr	r2, [r5, #0]
    210c:	f7fe f80b 	bl	126 <mbus_remote_register_write>
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
    2110:	2280      	movs	r2, #128	; 0x80
    2112:	682b      	ldr	r3, [r5, #0]
    2114:	0312      	lsls	r2, r2, #12
    2116:	4313      	orrs	r3, r2
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    2118:	2280      	movs	r2, #128	; 0x80
    
    // Decap in series
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
    211a:	602b      	str	r3, [r5, #0]
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    211c:	682b      	ldr	r3, [r5, #0]
    211e:	02d2      	lsls	r2, r2, #11
    2120:	4313      	orrs	r3, r2
    2122:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    2124:	682a      	ldr	r2, [r5, #0]
    2126:	0020      	movs	r0, r4
    2128:	2103      	movs	r1, #3
    212a:	f7fd fffc 	bl	126 <mbus_remote_register_write>

    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    212e:	227e      	movs	r2, #126	; 0x7e
    2130:	4e63      	ldr	r6, [pc, #396]	; (22c0 <main+0x460>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    2132:	0039      	movs	r1, r7
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);

    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    2134:	6833      	ldr	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    2136:	0020      	movs	r0, r4
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);

    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    2138:	4393      	bics	r3, r2
    213a:	3a6e      	subs	r2, #110	; 0x6e
    213c:	4313      	orrs	r3, r2
    213e:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    2140:	6832      	ldr	r2, [r6, #0]
    2142:	f7fd fff0 	bl	126 <mbus_remote_register_write>

    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    2146:	2201      	movs	r2, #1
    2148:	6833      	ldr	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    214a:	0039      	movs	r1, r7
    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    214c:	4313      	orrs	r3, r2
    214e:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    2150:	6832      	ldr	r2, [r6, #0]
    2152:	0020      	movs	r0, r4
    2154:	f7fd ffe7 	bl	126 <mbus_remote_register_write>

    // Wait for charging decap
    // config_timerwd(TIMERWD_VAL);
    // *REG_MBUS_WD = 1500000*3; // default: 1500000
    delay(MBUS_DELAY*200); // Wait for decap to charge
    2158:	485a      	ldr	r0, [pc, #360]	; (22c4 <main+0x464>)
    215a:	f7fe f821 	bl	1a0 <delay>

    mrrv11a_r1F.LC_CLK_RING = 0x3;  // ~ 150 kHz
    mrrv11a_r1F.LC_CLK_DIV = 0x3;  // ~ 150 kHz
    mbus_remote_register_write(MRR_ADDR,0x1F,mrrv11a_r1F.as_int);
    215e:	9a00      	ldr	r2, [sp, #0]
    2160:	0020      	movs	r0, r4
    2162:	211f      	movs	r1, #31
    2164:	f7fd ffdf 	bl	126 <mbus_remote_register_write>

}

static void mrr_configure_pulse_width_long() {

    mrrv11a_r12.FSM_TX_PW_LEN = 12; //50us PW
    2168:	210c      	movs	r1, #12
    216a:	4a57      	ldr	r2, [pc, #348]	; (22c8 <main+0x468>)
    mrrv11a_r13.FSM_TX_C_LEN = 1105;
    mrrv11a_r12.FSM_TX_PS_LEN = 25; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv11a_r12.as_int);
    216c:	0020      	movs	r0, r4

}

static void mrr_configure_pulse_width_long() {

    mrrv11a_r12.FSM_TX_PW_LEN = 12; //50us PW
    216e:	6813      	ldr	r3, [r2, #0]
    2170:	0a9b      	lsrs	r3, r3, #10
    2172:	029b      	lsls	r3, r3, #10
    2174:	430b      	orrs	r3, r1
    2176:	6013      	str	r3, [r2, #0]
    mrrv11a_r13.FSM_TX_C_LEN = 1105;
    2178:	4b54      	ldr	r3, [pc, #336]	; (22cc <main+0x46c>)
    217a:	4955      	ldr	r1, [pc, #340]	; (22d0 <main+0x470>)
    217c:	881b      	ldrh	r3, [r3, #0]
    217e:	0bdb      	lsrs	r3, r3, #15
    2180:	03db      	lsls	r3, r3, #15
    2182:	430b      	orrs	r3, r1
    2184:	4951      	ldr	r1, [pc, #324]	; (22cc <main+0x46c>)
    2186:	800b      	strh	r3, [r1, #0]
    mrrv11a_r12.FSM_TX_PS_LEN = 25; // PW=PS   
    2188:	6811      	ldr	r1, [r2, #0]
    218a:	4b52      	ldr	r3, [pc, #328]	; (22d4 <main+0x474>)
    218c:	400b      	ands	r3, r1
    218e:	21c8      	movs	r1, #200	; 0xc8
    2190:	01c9      	lsls	r1, r1, #7
    2192:	430b      	orrs	r3, r1
    2194:	6013      	str	r3, [r2, #0]

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv11a_r12.as_int);
    2196:	6812      	ldr	r2, [r2, #0]
    2198:	2112      	movs	r1, #18
    219a:	f7fd ffc4 	bl	126 <mbus_remote_register_write>
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv11a_r13.as_int);
    219e:	4b4b      	ldr	r3, [pc, #300]	; (22cc <main+0x46c>)
    21a0:	0020      	movs	r0, r4
    21a2:	681a      	ldr	r2, [r3, #0]
    21a4:	2113      	movs	r1, #19
    21a6:	f7fd ffbe 	bl	126 <mbus_remote_register_write>
    mrr_configure_pulse_width_long();

    //mrr_freq_hopping = 5;
    //mrr_freq_hopping_step = 4;
    mrr_freq_hopping = 2;
    mrr_freq_hopping_step = 4; // determining center freq
    21aa:	2204      	movs	r2, #4
    //mrr_configure_pulse_width_short();
    mrr_configure_pulse_width_long();

    //mrr_freq_hopping = 5;
    //mrr_freq_hopping_step = 4;
    mrr_freq_hopping = 2;
    21ac:	4b4a      	ldr	r3, [pc, #296]	; (22d8 <main+0x478>)

    mrr_cfo_val_fine_min = 0x0000;

    // RO setup (SFO)
    // Adjust Diffusion R
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    21ae:	0020      	movs	r0, r4
    //mrr_configure_pulse_width_short();
    mrr_configure_pulse_width_long();

    //mrr_freq_hopping = 5;
    //mrr_freq_hopping_step = 4;
    mrr_freq_hopping = 2;
    21b0:	801c      	strh	r4, [r3, #0]
    mrr_freq_hopping_step = 4; // determining center freq
    21b2:	4b4a      	ldr	r3, [pc, #296]	; (22dc <main+0x47c>)

    mrr_cfo_val_fine_min = 0x0000;

    // RO setup (SFO)
    // Adjust Diffusion R
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    21b4:	2106      	movs	r1, #6
    mrr_configure_pulse_width_long();

    //mrr_freq_hopping = 5;
    //mrr_freq_hopping_step = 4;
    mrr_freq_hopping = 2;
    mrr_freq_hopping_step = 4; // determining center freq
    21b6:	801a      	strh	r2, [r3, #0]

    mrr_cfo_val_fine_min = 0x0000;

    // RO setup (SFO)
    // Adjust Diffusion R
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    21b8:	2280      	movs	r2, #128	; 0x80
    //mrr_freq_hopping = 5;
    //mrr_freq_hopping_step = 4;
    mrr_freq_hopping = 2;
    mrr_freq_hopping_step = 4; // determining center freq

    mrr_cfo_val_fine_min = 0x0000;
    21ba:	4b49      	ldr	r3, [pc, #292]	; (22e0 <main+0x480>)

    // RO setup (SFO)
    // Adjust Diffusion R
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    21bc:	0152      	lsls	r2, r2, #5
    //mrr_freq_hopping = 5;
    //mrr_freq_hopping_step = 4;
    mrr_freq_hopping = 2;
    mrr_freq_hopping_step = 4; // determining center freq

    mrr_cfo_val_fine_min = 0x0000;
    21be:	801f      	strh	r7, [r3, #0]

    // RO setup (SFO)
    // Adjust Diffusion R
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    21c0:	f7fd ffb1 	bl	126 <mbus_remote_register_write>

    // Adjust Poly R
    mbus_remote_register_write(MRR_ADDR,0x08,0x400000); // RO_POLY
    21c4:	2280      	movs	r2, #128	; 0x80
    21c6:	0020      	movs	r0, r4
    21c8:	03d2      	lsls	r2, r2, #15
    21ca:	2108      	movs	r1, #8
    21cc:	f7fd ffab 	bl	126 <mbus_remote_register_write>

    // Adjust C
    mrrv11a_r07.RO_MOM = 0x10;
    21d0:	217f      	movs	r1, #127	; 0x7f
    21d2:	4b44      	ldr	r3, [pc, #272]	; (22e4 <main+0x484>)
    mrrv11a_r07.RO_MIM = 0x10;
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);
    21d4:	0020      	movs	r0, r4

    // Adjust Poly R
    mbus_remote_register_write(MRR_ADDR,0x08,0x400000); // RO_POLY

    // Adjust C
    mrrv11a_r07.RO_MOM = 0x10;
    21d6:	881a      	ldrh	r2, [r3, #0]
    21d8:	438a      	bics	r2, r1
    21da:	396f      	subs	r1, #111	; 0x6f
    21dc:	430a      	orrs	r2, r1
    21de:	801a      	strh	r2, [r3, #0]
    mrrv11a_r07.RO_MIM = 0x10;
    21e0:	8819      	ldrh	r1, [r3, #0]
    21e2:	4a41      	ldr	r2, [pc, #260]	; (22e8 <main+0x488>)
    21e4:	400a      	ands	r2, r1
    21e6:	2180      	movs	r1, #128	; 0x80
    21e8:	0109      	lsls	r1, r1, #4
    21ea:	430a      	orrs	r2, r1
    21ec:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);
    21ee:	681a      	ldr	r2, [r3, #0]
    21f0:	2107      	movs	r1, #7
    21f2:	f7fd ff98 	bl	126 <mbus_remote_register_write>

    // TX Setup Carrier Freq
    mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = 0xFF;  //ANT CAP 10b unary 830.5 MHz // FIXME: adjust per stack    // TODO: make them the same variable
    21f6:	6832      	ldr	r2, [r6, #0]
    21f8:	4b3c      	ldr	r3, [pc, #240]	; (22ec <main+0x48c>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    21fa:	0039      	movs	r1, r7
    mrrv11a_r07.RO_MOM = 0x10;
    mrrv11a_r07.RO_MIM = 0x10;
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);

    // TX Setup Carrier Freq
    mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = 0xFF;  //ANT CAP 10b unary 830.5 MHz // FIXME: adjust per stack    // TODO: make them the same variable
    21fc:	4013      	ands	r3, r2
    21fe:	22ff      	movs	r2, #255	; 0xff
    2200:	01d2      	lsls	r2, r2, #7
    2202:	4313      	orrs	r3, r2
    2204:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    2206:	6832      	ldr	r2, [r6, #0]
    2208:	0020      	movs	r0, r4
    220a:	f7fd ff8c 	bl	126 <mbus_remote_register_write>
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_COARSE = 0xFF; //ANT CAP 10b unary 830.5 MHz
    220e:	21ff      	movs	r1, #255	; 0xff
    2210:	4a37      	ldr	r2, [pc, #220]	; (22f0 <main+0x490>)
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
    2212:	4838      	ldr	r0, [pc, #224]	; (22f4 <main+0x494>)
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);

    // TX Setup Carrier Freq
    mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = 0xFF;  //ANT CAP 10b unary 830.5 MHz // FIXME: adjust per stack    // TODO: make them the same variable
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_COARSE = 0xFF; //ANT CAP 10b unary 830.5 MHz
    2214:	6813      	ldr	r3, [r2, #0]
    2216:	0a9b      	lsrs	r3, r3, #10
    2218:	029b      	lsls	r3, r3, #10
    221a:	430b      	orrs	r3, r1
    221c:	6013      	str	r3, [r2, #0]
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
    221e:	4b30      	ldr	r3, [pc, #192]	; (22e0 <main+0x480>)
    2220:	8819      	ldrh	r1, [r3, #0]
    2222:	233f      	movs	r3, #63	; 0x3f
    2224:	6816      	ldr	r6, [r2, #0]
    2226:	4019      	ands	r1, r3
    2228:	4030      	ands	r0, r6
    222a:	0409      	lsls	r1, r1, #16
    222c:	4301      	orrs	r1, r0
    222e:	6011      	str	r1, [r2, #0]
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
    2230:	492b      	ldr	r1, [pc, #172]	; (22e0 <main+0x480>)
    2232:	8808      	ldrh	r0, [r1, #0]
    2234:	6811      	ldr	r1, [r2, #0]
    2236:	4003      	ands	r3, r0
    2238:	482f      	ldr	r0, [pc, #188]	; (22f8 <main+0x498>)
    223a:	029b      	lsls	r3, r3, #10
    223c:	4001      	ands	r1, r0
    223e:	430b      	orrs	r3, r1
    2240:	6013      	str	r3, [r2, #0]
    2242:	e05b      	b.n	22fc <main+0x49c>
    2244:	e000e100 	.word	0xe000e100
    2248:	00000f0d 	.word	0x00000f0d
    224c:	00003990 	.word	0x00003990
    2250:	deadc51f 	.word	0xdeadc51f
    2254:	a0001200 	.word	0xa0001200
    2258:	a000007c 	.word	0xa000007c
    225c:	00003aec 	.word	0x00003aec
    2260:	00003acc 	.word	0x00003acc
    2264:	ffff8000 	.word	0xffff8000
    2268:	00003a78 	.word	0x00003a78
    226c:	00003932 	.word	0x00003932
    2270:	00003800 	.word	0x00003800
    2274:	ffff00ff 	.word	0xffff00ff
    2278:	000038f0 	.word	0x000038f0
    227c:	0000382e 	.word	0x0000382e
    2280:	0000397c 	.word	0x0000397c
    2284:	00003868 	.word	0x00003868
    2288:	00003ad2 	.word	0x00003ad2
    228c:	00003698 	.word	0x00003698
    2290:	ff000fff 	.word	0xff000fff
    2294:	ff1fffff 	.word	0xff1fffff
    2298:	ffe0007f 	.word	0xffe0007f
    229c:	001ffe80 	.word	0x001ffe80
    22a0:	000080fc 	.word	0x000080fc
    22a4:	ffdfffff 	.word	0xffdfffff
    22a8:	000186a0 	.word	0x000186a0
    22ac:	0003ce67 	.word	0x0003ce67
    22b0:	00ffffff 	.word	0x00ffffff
    22b4:	000038ac 	.word	0x000038ac
    22b8:	0000380c 	.word	0x0000380c
    22bc:	ffefffff 	.word	0xffefffff
    22c0:	00003804 	.word	0x00003804
    22c4:	00004e20 	.word	0x00004e20
    22c8:	0000385c 	.word	0x0000385c
    22cc:	00003860 	.word	0x00003860
    22d0:	00000451 	.word	0x00000451
    22d4:	fff003ff 	.word	0xfff003ff
    22d8:	00003988 	.word	0x00003988
    22dc:	00003a24 	.word	0x00003a24
    22e0:	00003984 	.word	0x00003984
    22e4:	00003814 	.word	0x00003814
    22e8:	ffffc07f 	.word	0xffffc07f
    22ec:	fffe007f 	.word	0xfffe007f
    22f0:	00003b10 	.word	0x00003b10
    22f4:	ffc0ffff 	.word	0xffc0ffff
    22f8:	ffff03ff 	.word	0xffff03ff
    mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
    22fc:	0020      	movs	r0, r4
    22fe:	6812      	ldr	r2, [r2, #0]
    2300:	2101      	movs	r1, #1
    2302:	f7fd ff10 	bl	126 <mbus_remote_register_write>
    mrrv11a_r02.TX_EN_OW = 0; // Turn off TX_EN_OW
    mrrv11a_r02.TX_BIAS_TUNE = 0x1FFF;  //Set TX BIAS TUNE 13b // Max 0x1FFF
    mbus_remote_register_write(MRR_ADDR,0x02,mrrv11a_r02.as_int);
    2306:	0021      	movs	r1, r4
    2308:	0020      	movs	r0, r4
    230a:	4adc      	ldr	r2, [pc, #880]	; (267c <main+0x81c>)
    230c:	f7fd ff0b 	bl	126 <mbus_remote_register_write>

    // Turn off RX mode
    mrrv11a_r03.TRX_MODE_EN = 0; //Set TRX mode
    2310:	682a      	ldr	r2, [r5, #0]
    2312:	4bdb      	ldr	r3, [pc, #876]	; (2680 <main+0x820>)
    mbus_remote_register_write(MRR_ADDR,3,mrrv11a_r03.as_int);
    2314:	0020      	movs	r0, r4
    mrrv11a_r02.TX_EN_OW = 0; // Turn off TX_EN_OW
    mrrv11a_r02.TX_BIAS_TUNE = 0x1FFF;  //Set TX BIAS TUNE 13b // Max 0x1FFF
    mbus_remote_register_write(MRR_ADDR,0x02,mrrv11a_r02.as_int);

    // Turn off RX mode
    mrrv11a_r03.TRX_MODE_EN = 0; //Set TRX mode
    2316:	4013      	ands	r3, r2
    2318:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,3,mrrv11a_r03.as_int);
    231a:	682a      	ldr	r2, [r5, #0]
    231c:	2103      	movs	r1, #3
    231e:	f7fd ff02 	bl	126 <mbus_remote_register_write>

    mrrv11a_r14.FSM_TX_POWERON_LEN = 0; //3bits
    mrrv11a_r15.FSM_RX_DATA_BITS = 0x00; //Set RX data 1b
    mbus_remote_register_write(MRR_ADDR,0x14,mrrv11a_r14.as_int);
    2322:	003a      	movs	r2, r7
    2324:	0020      	movs	r0, r4
    2326:	2114      	movs	r1, #20
    2328:	f7fd fefd 	bl	126 <mbus_remote_register_write>
    mbus_remote_register_write(MRR_ADDR,0x15,mrrv11a_r15.as_int);
    232c:	003a      	movs	r2, r7
    232e:	0020      	movs	r0, r4
    2330:	2115      	movs	r1, #21
    2332:	f7fd fef8 	bl	126 <mbus_remote_register_write>

    // RAD_FSM set-up 
    mrrv11a_r10.FSM_TX_TP_LEN = 80;       // Num. Training Pulses (bit 0); Valid Range: 1 ~ 255 (Default: 80)
    2336:	21ff      	movs	r1, #255	; 0xff
    2338:	4bd2      	ldr	r3, [pc, #840]	; (2684 <main+0x824>)
    mrrv11a_r10.FSM_TX_PASSCODE = 0x7AC8; // 16-bit Passcode (sent from LSB to MSB)
    mbus_remote_register_write(MRR_ADDR,0x10,mrrv11a_r10.as_int);
    233a:	0020      	movs	r0, r4
    mrrv11a_r15.FSM_RX_DATA_BITS = 0x00; //Set RX data 1b
    mbus_remote_register_write(MRR_ADDR,0x14,mrrv11a_r14.as_int);
    mbus_remote_register_write(MRR_ADDR,0x15,mrrv11a_r15.as_int);

    // RAD_FSM set-up 
    mrrv11a_r10.FSM_TX_TP_LEN = 80;       // Num. Training Pulses (bit 0); Valid Range: 1 ~ 255 (Default: 80)
    233c:	681a      	ldr	r2, [r3, #0]
    233e:	438a      	bics	r2, r1
    2340:	39af      	subs	r1, #175	; 0xaf
    2342:	430a      	orrs	r2, r1
    2344:	601a      	str	r2, [r3, #0]
    mrrv11a_r10.FSM_TX_PASSCODE = 0x7AC8; // 16-bit Passcode (sent from LSB to MSB)
    2346:	6819      	ldr	r1, [r3, #0]
    2348:	4acf      	ldr	r2, [pc, #828]	; (2688 <main+0x828>)
    234a:	400a      	ands	r2, r1
    234c:	49cf      	ldr	r1, [pc, #828]	; (268c <main+0x82c>)
    234e:	430a      	orrs	r2, r1
    2350:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x10,mrrv11a_r10.as_int);
    2352:	681a      	ldr	r2, [r3, #0]
    2354:	2110      	movs	r1, #16
    2356:	f7fd fee6 	bl	126 <mbus_remote_register_write>
    mrrv11a_r11.FSM_TX_DATA_BITS = RADIO_DATA_NUM_WORDS * 32; //0-skip tx data
    235a:	4acd      	ldr	r2, [pc, #820]	; (2690 <main+0x830>)
    235c:	4bcd      	ldr	r3, [pc, #820]	; (2694 <main+0x834>)
    235e:	6811      	ldr	r1, [r2, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    2360:	0020      	movs	r0, r4

    // RAD_FSM set-up 
    mrrv11a_r10.FSM_TX_TP_LEN = 80;       // Num. Training Pulses (bit 0); Valid Range: 1 ~ 255 (Default: 80)
    mrrv11a_r10.FSM_TX_PASSCODE = 0x7AC8; // 16-bit Passcode (sent from LSB to MSB)
    mbus_remote_register_write(MRR_ADDR,0x10,mrrv11a_r10.as_int);
    mrrv11a_r11.FSM_TX_DATA_BITS = RADIO_DATA_NUM_WORDS * 32; //0-skip tx data
    2362:	400b      	ands	r3, r1
    2364:	21c0      	movs	r1, #192	; 0xc0
    2366:	01c9      	lsls	r1, r1, #7
    2368:	430b      	orrs	r3, r1
    236a:	6013      	str	r3, [r2, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    236c:	6812      	ldr	r2, [r2, #0]
    236e:	2111      	movs	r1, #17
    2370:	f7fd fed9 	bl	126 <mbus_remote_register_write>

    // Mbus return address
    mbus_remote_register_write(MRR_ADDR,0x1E,0x1002);
    2374:	4ac8      	ldr	r2, [pc, #800]	; (2698 <main+0x838>)
    2376:	211e      	movs	r1, #30
    2378:	0020      	movs	r0, r4
    237a:	f7fd fed4 	bl	126 <mbus_remote_register_write>
//remove?     mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);

    // Additional delay for charging decap
    // config_timerwd(TIMERWD_VAL);
    // *REG_MBUS_WD = 1500000; // default: 1500000
    delay(MBUS_DELAY*200); // Wait for decap to charge
    237e:	48c7      	ldr	r0, [pc, #796]	; (269c <main+0x83c>)
    2380:	f7fd ff0e 	bl	1a0 <delay>
    lnt_init();
#endif

#ifdef USE_MRR
    mrr_init();
    radio_on = 0;
    2384:	4bc6      	ldr	r3, [pc, #792]	; (26a0 <main+0x840>)
#endif
}

static void pmu_set_clk_init() { //need a safe PMU state before enabling slow loop & canary
#ifdef USE_PMU
    pmu_set_active_clk(0x0D020F0F);
    2386:	48c7      	ldr	r0, [pc, #796]	; (26a4 <main+0x844>)
    lnt_init();
#endif

#ifdef USE_MRR
    mrr_init();
    radio_on = 0;
    2388:	801f      	strh	r7, [r3, #0]
    radio_ready = 0;
    238a:	4bc7      	ldr	r3, [pc, #796]	; (26a8 <main+0x848>)
    238c:	801f      	strh	r7, [r3, #0]
#endif
}

static void pmu_set_clk_init() { //need a safe PMU state before enabling slow loop & canary
#ifdef USE_PMU
    pmu_set_active_clk(0x0D020F0F);
    238e:	f7fe f806 	bl	39e <pmu_set_active_clk>
    pmu_set_sleep_clk(0x0F010102);
    2392:	48c6      	ldr	r0, [pc, #792]	; (26ac <main+0x84c>)
    2394:	f7fd ffdf 	bl	356 <pmu_set_sleep_clk>
    pmu_set_sar_ratio(52);
    2398:	2034      	movs	r0, #52	; 0x34
    239a:	f7ff f87b 	bl	1494 <pmu_set_sar_ratio>
    pmu_set_clk_init();
    //pmu_enable_4V_harvesting();
    
    // New for PMUv9
    // VBAT_READ_TRIM Register
    pmu_reg_write(0x45,         // FIXME: this register is reserved in PMUv10
    239e:	2148      	movs	r1, #72	; 0x48
    23a0:	2045      	movs	r0, #69	; 0x45
    23a2:	f7fd ffcb 	bl	33c <pmu_reg_write>
    // (0 << 19) |    // state_vbat_read
    // (1 << 13) |    // ignore state_adc_output_ready; default: 0
    // (1 << 12) |    // ignore state_adc_output_ready; default:0
    // (1 << 11)));   // ignore state_adc_output_ready; default:0
    
    pmu_reg_write(0x10,         // PMU_SAR_TRIM_ACTIVE
    23a6:	49c2      	ldr	r1, [pc, #776]	; (26b0 <main+0x850>)
    23a8:	2010      	movs	r0, #16
    23aa:	f7fd ffc7 	bl	33c <pmu_reg_write>

static void pmu_adc_reset_setting() {
#ifdef USE_PMU
    // PMU ADC will be automatically reset when system wakes up
    // Updated for PMUv9
    pmu_reg_write(0x3C,         // PMU_EN_CONTROLLER_DESIRED_STATE_ACTIVE
    23ae:	49c1      	ldr	r1, [pc, #772]	; (26b4 <main+0x854>)
    23b0:	203c      	movs	r0, #60	; 0x3c
    23b2:	f7fd ffc3 	bl	33c <pmu_reg_write>
static void pmu_adc_enable() {
#ifdef USE_PMU
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE sleep
    // Updated for PMUv9
    pmu_reg_write(0x3B,         // PMU_EN_CONTROLLER_DESIRED_STATE_SLEEP
    23b6:	49c0      	ldr	r1, [pc, #768]	; (26b8 <main+0x858>)
    23b8:	203b      	movs	r0, #59	; 0x3b
    23ba:	f7fd ffbf 	bl	33c <pmu_reg_write>
}

static void pmu_prep_sar_ratio () {

    // Minimize the SAR_RESET pulse width
    pmu_reg_write (33, // 0x21 (TICK_SAR_RESET)
    23be:	2101      	movs	r1, #1
    23c0:	2021      	movs	r0, #33	; 0x21
    23c2:	f7fd ffbb 	bl	33c <pmu_reg_write>
        ( (1 << 0)     // TICK_SAR_RESET
    ));

    // Set SAR_RATIO_OFFSET_DOWN=0 to enable the ratio override.
    pmu_reg_write (7, // 0x7 (SAR_RATIO_OFFSET_DOWN_ACTIVE)
    23c6:	0039      	movs	r1, r7
    23c8:	2007      	movs	r0, #7
    23ca:	f7fd ffb7 	bl	33c <pmu_reg_write>
        ( (0 << 0)    // SAR_RATIO_OFFSET_DOWN_ACTIVE
    ));

    // Set SAR_RATIO_OFFSET_DOWN=0 to enable the ratio override.
    pmu_reg_write (6, // 0x6 (SAR_RATIO_OFFSET_DOWN_SLEEP)
    23ce:	0039      	movs	r1, r7
    23d0:	2006      	movs	r0, #6
    23d2:	f7fd ffb3 	bl	33c <pmu_reg_write>
        ( (0 << 0)    // SAR_RATIO_OFFSET_DOWN_SLEEP
    ));

    // Just to make sure you do not 'stall' adc operation in Active
    pmu_reg_write (58, // 0x3A (CTRL_STALL_STATE_ACTIVE)
    23d6:	0039      	movs	r1, r7
    23d8:	203a      	movs	r0, #58	; 0x3a
    23da:	f7fd ffaf 	bl	33c <pmu_reg_write>
    
    pmu_adc_reset_setting();
    pmu_adc_enable();
    pmu_prep_sar_ratio();

    pmu_reg_write(0x47, // use slow loop and active-mode PRC clk tracking
    23de:	49b7      	ldr	r1, [pc, #732]	; (26bc <main+0x85c>)
    23e0:	2047      	movs	r0, #71	; 0x47
    23e2:	f7fd ffab 	bl	33c <pmu_reg_write>
#ifdef USE_PMU
    pmu_init();
#endif

#ifdef USE_XO
    xo_init();
    23e6:	f7fe f8a5 	bl	534 <xo_init>

    update_system_time();
    23ea:	f7ff f8dd 	bl	15a8 <update_system_time>
    update_system_time();
    23ee:	f7ff f8db 	bl	15a8 <update_system_time>
    update_system_time();
    23f2:	f7ff f8d9 	bl	15a8 <update_system_time>
    23f6:	e072      	b.n	24de <main+0x67e>

static void pmu_adc_read_latest() {
#ifdef USE_PMU
    // Grab latest pmu adc readings 
    // PMU register read is handled differently
    pmu_reg_write(0x00, 0x03);
    23f8:	2103      	movs	r1, #3
    23fa:	2000      	movs	r0, #0
    23fc:	f7fd ff9e 	bl	33c <pmu_reg_write>
    // Updated for pmuv9
    read_data_batadc = *REG0 & 0xFF;    // TODO: check if only need low 8 bits
    2400:	23a0      	movs	r3, #160	; 0xa0
    2402:	061b      	lsls	r3, r3, #24
    2404:	681b      	ldr	r3, [r3, #0]
    2406:	4aae      	ldr	r2, [pc, #696]	; (26c0 <main+0x860>)
    2408:	b2db      	uxtb	r3, r3
        // operation_sleep_notimer();
    }

    // must read adc from sleep mode prior to adjusting PMU SAR ratio in active mode
    pmu_adc_read_latest();
    pmu_setting_temp_based(0);
    240a:	2000      	movs	r0, #0
#ifdef USE_PMU
    // Grab latest pmu adc readings 
    // PMU register read is handled differently
    pmu_reg_write(0x00, 0x03);
    // Updated for pmuv9
    read_data_batadc = *REG0 & 0xFF;    // TODO: check if only need low 8 bits
    240c:	8013      	strh	r3, [r2, #0]
        // operation_sleep_notimer();
    }

    // must read adc from sleep mode prior to adjusting PMU SAR ratio in active mode
    pmu_adc_read_latest();
    pmu_setting_temp_based(0);
    240e:	f7ff f881 	bl	1514 <pmu_setting_temp_based>

    update_system_time();
    2412:	f7ff f8c9 	bl	15a8 <update_system_time>
uint16_t xo_lnt_mplier = 0x73;
uint32_t projected_end_time_in_sec = 0;
uint32_t lnt_meas_time = 0;

static void update_lnt_timer() {
    if(projected_end_time_in_sec == 0) {
    2416:	4fab      	ldr	r7, [pc, #684]	; (26c4 <main+0x864>)
    2418:	683b      	ldr	r3, [r7, #0]
    241a:	2b00      	cmp	r3, #0
    241c:	d01b      	beq.n	2456 <main+0x5f6>
    241e:	2080      	movs	r0, #128	; 0x80
        return;
    }
    uint32_t projected_end_time = projected_end_time_in_sec << XO_TO_SEC_SHIFT;

    if(xo_sys_time - projected_end_time > TIMER_MARGIN 
    2420:	4ca9      	ldr	r4, [pc, #676]	; (26c8 <main+0x868>)

static void update_lnt_timer() {
    if(projected_end_time_in_sec == 0) {
        return;
    }
    uint32_t projected_end_time = projected_end_time_in_sec << XO_TO_SEC_SHIFT;
    2422:	03da      	lsls	r2, r3, #15

    if(xo_sys_time - projected_end_time > TIMER_MARGIN 
    2424:	6821      	ldr	r1, [r4, #0]
    2426:	00c0      	lsls	r0, r0, #3
    2428:	1a89      	subs	r1, r1, r2
    242a:	4281      	cmp	r1, r0
    242c:	d907      	bls.n	243e <main+0x5de>
    && xo_sys_time_in_sec >= projected_end_time_in_sec) {
    242e:	49a7      	ldr	r1, [pc, #668]	; (26cc <main+0x86c>)
    2430:	6809      	ldr	r1, [r1, #0]
    2432:	428b      	cmp	r3, r1
    2434:	d803      	bhi.n	243e <main+0x5de>
        xo_lnt_mplier--;
    2436:	4aa6      	ldr	r2, [pc, #664]	; (26d0 <main+0x870>)
    2438:	8813      	ldrh	r3, [r2, #0]
    243a:	3b01      	subs	r3, #1
    243c:	e00a      	b.n	2454 <main+0x5f4>
    }
    else if(projected_end_time - xo_sys_time > TIMER_MARGIN 
    243e:	6821      	ldr	r1, [r4, #0]
    2440:	1a52      	subs	r2, r2, r1
    2442:	4282      	cmp	r2, r0
    2444:	d907      	bls.n	2456 <main+0x5f6>
        && xo_sys_time_in_sec <= projected_end_time_in_sec) {
    2446:	4aa1      	ldr	r2, [pc, #644]	; (26cc <main+0x86c>)
    2448:	6812      	ldr	r2, [r2, #0]
    244a:	4293      	cmp	r3, r2
    244c:	d303      	bcc.n	2456 <main+0x5f6>
        xo_lnt_mplier++;
    244e:	4aa0      	ldr	r2, [pc, #640]	; (26d0 <main+0x870>)
    2450:	8813      	ldrh	r3, [r2, #0]
    2452:	3301      	adds	r3, #1
    2454:	8013      	strh	r3, [r2, #0]

    // mbus_write_message32(0xEE, *SREG_WAKEUP_SOURCE);
    // mbus_write_message32(0xEE, *GOC_DATA_IRQ);

    // check wakeup is due to GOC
    if(*SREG_WAKEUP_SOURCE & 1) {
    2456:	4b9f      	ldr	r3, [pc, #636]	; (26d4 <main+0x874>)
    2458:	681b      	ldr	r3, [r3, #0]
    245a:	07db      	lsls	r3, r3, #31
    245c:	d501      	bpl.n	2462 <main+0x602>
        // if(!(*GOC_DATA_IRQ)) {
        //     operation_sleep(); // Need to protect against spurious wakeups
        // }
        set_goc_cmd();
    245e:	f7ff fb49 	bl	1af4 <set_goc_cmd>
    }

    lnt_stop();
    2462:	f7fe f911 	bl	688 <lnt_stop>

    operation_temp_run();
    2466:	f7ff fa97 	bl	1998 <operation_temp_run>
    pmu_setting_temp_based(0);
    246a:	2000      	movs	r0, #0
    246c:	f7ff f852 	bl	1514 <pmu_setting_temp_based>

    mrr_send_enable = set_send_enable();
    2470:	f7ff fc08 	bl	1c84 <set_send_enable>
    2474:	4e98      	ldr	r6, [pc, #608]	; (26d8 <main+0x878>)

    uint16_t goc_data_header = goc_data_full >> 24;
    2476:	4c99      	ldr	r4, [pc, #612]	; (26dc <main+0x87c>)
    lnt_stop();

    operation_temp_run();
    pmu_setting_temp_based(0);

    mrr_send_enable = set_send_enable();
    2478:	8030      	strh	r0, [r6, #0]

    uint16_t goc_data_header = goc_data_full >> 24;
    247a:	6822      	ldr	r2, [r4, #0]
    // common option
    bool option = ((goc_data_full >> 23) & 0x1)? true : false;
    247c:	6825      	ldr	r5, [r4, #0]
    operation_temp_run();
    pmu_setting_temp_based(0);

    mrr_send_enable = set_send_enable();

    uint16_t goc_data_header = goc_data_full >> 24;
    247e:	0e12      	lsrs	r2, r2, #24
    // common option
    bool option = ((goc_data_full >> 23) & 0x1)? true : false;

    if(goc_data_header == 0x00) {
    2480:	d101      	bne.n	2486 <main+0x626>
    2482:	f001 f847 	bl	3514 <main+0x16b4>

    mrr_send_enable = set_send_enable();

    uint16_t goc_data_header = goc_data_full >> 24;
    // common option
    bool option = ((goc_data_full >> 23) & 0x1)? true : false;
    2486:	2101      	movs	r1, #1
    2488:	0ded      	lsrs	r5, r5, #23
    operation_temp_run();
    pmu_setting_temp_based(0);

    mrr_send_enable = set_send_enable();

    uint16_t goc_data_header = goc_data_full >> 24;
    248a:	b293      	uxth	r3, r2
    // common option
    bool option = ((goc_data_full >> 23) & 0x1)? true : false;
    248c:	400d      	ands	r5, r1

    if(goc_data_header == 0x00) {
        // do nothing
    }
   else if(goc_data_header == 0x01) {
    248e:	428a      	cmp	r2, r1
    2490:	d127      	bne.n	24e2 <main+0x682>
        // Alive beacon, safe sleep mode, and set chip id
        bool option2 = ((goc_data_full >> 22) & 0x1)? true : false;
    2492:	6823      	ldr	r3, [r4, #0]
    2494:	4992      	ldr	r1, [pc, #584]	; (26e0 <main+0x880>)
        if(option2) {
    2496:	0d9b      	lsrs	r3, r3, #22
    2498:	4213      	tst	r3, r2
    249a:	d003      	beq.n	24a4 <main+0x644>
            CHIP_ID = goc_data_full & 0x1F;
    249c:	231f      	movs	r3, #31
    249e:	6822      	ldr	r2, [r4, #0]
    24a0:	4013      	ands	r3, r2
    24a2:	800b      	strh	r3, [r1, #0]
        }
    
        // send alive beacon
        radio_data_arr[2] = (0x1 << 8) | CHIP_ID;
    24a4:	2380      	movs	r3, #128	; 0x80
    24a6:	880a      	ldrh	r2, [r1, #0]
        radio_data_arr[1] = (VERSION_NUM << 16) | read_data_batadc;
    24a8:	21c6      	movs	r1, #198	; 0xc6
        if(option2) {
            CHIP_ID = goc_data_full & 0x1F;
        }
    
        // send alive beacon
        radio_data_arr[2] = (0x1 << 8) | CHIP_ID;
    24aa:	005b      	lsls	r3, r3, #1
    24ac:	431a      	orrs	r2, r3
    24ae:	4b8d      	ldr	r3, [pc, #564]	; (26e4 <main+0x884>)
        radio_data_arr[1] = (VERSION_NUM << 16) | read_data_batadc;
    24b0:	04c9      	lsls	r1, r1, #19
        if(option2) {
            CHIP_ID = goc_data_full & 0x1F;
        }
    
        // send alive beacon
        radio_data_arr[2] = (0x1 << 8) | CHIP_ID;
    24b2:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = (VERSION_NUM << 16) | read_data_batadc;
    24b4:	4a82      	ldr	r2, [pc, #520]	; (26c0 <main+0x860>)
    24b6:	8812      	ldrh	r2, [r2, #0]
    24b8:	430a      	orrs	r2, r1
    24ba:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = snt_sys_temp_code;
    24bc:	4a8a      	ldr	r2, [pc, #552]	; (26e8 <main+0x888>)
    24be:	6812      	ldr	r2, [r2, #0]
    24c0:	601a      	str	r2, [r3, #0]
        send_beacon();
    24c2:	f7ff fc57 	bl	1d74 <send_beacon>

        if(option) {
    24c6:	2d00      	cmp	r5, #0
    24c8:	d101      	bne.n	24ce <main+0x66e>
    24ca:	f001 f823 	bl	3514 <main+0x16b4>
}

// safe sleep mode
static void pmu_set_sleep_low() {
#ifdef USE_PMU
    pmu_set_sleep_clk(PMU_SLEEP_SETTINGS[2]);
    24ce:	4b87      	ldr	r3, [pc, #540]	; (26ec <main+0x88c>)
    24d0:	6898      	ldr	r0, [r3, #8]
    24d2:	f7fd ff40 	bl	356 <pmu_set_sleep_clk>
    pmu_set_sar_ratio(PMU_SLEEP_SAR_SETTINGS[2]);
    24d6:	4b86      	ldr	r3, [pc, #536]	; (26f0 <main+0x890>)
    24d8:	6898      	ldr	r0, [r3, #8]
    24da:	f7fe ffdb 	bl	1494 <pmu_set_sar_ratio>
        if(option) {
            // safe sleep mode
            // FIXME: this might be bugged
        pmu_set_sleep_low();
            
        operation_sleep();
    24de:	f7fe f819 	bl	514 <operation_sleep>
        }
    }
    else if(goc_data_header == 0x02) {
    24e2:	2b02      	cmp	r3, #2
    24e4:	d13d      	bne.n	2562 <main+0x702>
        // packet blaster: send packets for N seconds, then sleep for M seconds
        uint8_t N = (goc_data_full >> 16) & 0xFF;
    24e6:	6825      	ldr	r5, [r4, #0]
        uint8_t M = (goc_data_full >> 8) & 0xFF;
    24e8:	6823      	ldr	r3, [r4, #0]

        if(goc_state == 0) {
    24ea:	4a82      	ldr	r2, [pc, #520]	; (26f4 <main+0x894>)
        }
    }
    else if(goc_data_header == 0x02) {
        // packet blaster: send packets for N seconds, then sleep for M seconds
        uint8_t N = (goc_data_full >> 16) & 0xFF;
        uint8_t M = (goc_data_full >> 8) & 0xFF;
    24ec:	0a1b      	lsrs	r3, r3, #8
    24ee:	9301      	str	r3, [sp, #4]

        if(goc_state == 0) {
    24f0:	8813      	ldrh	r3, [r2, #0]
    24f2:	b29b      	uxth	r3, r3
    24f4:	2b00      	cmp	r3, #0
    24f6:	d103      	bne.n	2500 <main+0x6a0>
            op_counter = 0;   
    24f8:	497f      	ldr	r1, [pc, #508]	; (26f8 <main+0x898>)
    24fa:	800b      	strh	r3, [r1, #0]
            goc_state = 1;
    24fc:	3301      	adds	r3, #1
    24fe:	8013      	strh	r3, [r2, #0]
        }

        pmu_setting_temp_based(1);
    2500:	2001      	movs	r0, #1
    2502:	f7ff f807 	bl	1514 <pmu_setting_temp_based>
        uint32_t start_time_in_sec = xo_sys_time_in_sec;
    2506:	4b71      	ldr	r3, [pc, #452]	; (26cc <main+0x86c>)
    2508:	681b      	ldr	r3, [r3, #0]
    250a:	9302      	str	r3, [sp, #8]
        if(mrr_send_enable) {
    250c:	8833      	ldrh	r3, [r6, #0]
    250e:	2b00      	cmp	r3, #0
    2510:	d104      	bne.n	251c <main+0x6bc>
                update_system_time();
            } while(xo_sys_time_in_sec - start_time_in_sec < N);
        }

        // set M second timer
        projected_end_time_in_sec = xo_sys_time_in_sec + M;
    2512:	466a      	mov	r2, sp
    2514:	4b6d      	ldr	r3, [pc, #436]	; (26cc <main+0x86c>)
    2516:	7912      	ldrb	r2, [r2, #4]
    2518:	681b      	ldr	r3, [r3, #0]
    251a:	e093      	b.n	2644 <main+0x7e4>
    251c:	23ff      	movs	r3, #255	; 0xff
    251e:	0c2d      	lsrs	r5, r5, #16
    2520:	401d      	ands	r5, r3
        pmu_setting_temp_based(1);
        uint32_t start_time_in_sec = xo_sys_time_in_sec;
        if(mrr_send_enable) {
            do {
                // radio out for N seconds
                radio_data_arr[2] = (0x2 << 8) | CHIP_ID;
    2522:	2680      	movs	r6, #128	; 0x80
    2524:	9500      	str	r5, [sp, #0]
                radio_data_arr[1] = goc_data_full & 0xFF;
    2526:	001d      	movs	r5, r3
        pmu_setting_temp_based(1);
        uint32_t start_time_in_sec = xo_sys_time_in_sec;
        if(mrr_send_enable) {
            do {
                // radio out for N seconds
                radio_data_arr[2] = (0x2 << 8) | CHIP_ID;
    2528:	00b6      	lsls	r6, r6, #2
    252a:	4b6d      	ldr	r3, [pc, #436]	; (26e0 <main+0x880>)
    252c:	4a6d      	ldr	r2, [pc, #436]	; (26e4 <main+0x884>)
    252e:	881b      	ldrh	r3, [r3, #0]
                radio_data_arr[1] = goc_data_full & 0xFF;
                radio_data_arr[0] = op_counter++;
    2530:	4871      	ldr	r0, [pc, #452]	; (26f8 <main+0x898>)
        pmu_setting_temp_based(1);
        uint32_t start_time_in_sec = xo_sys_time_in_sec;
        if(mrr_send_enable) {
            do {
                // radio out for N seconds
                radio_data_arr[2] = (0x2 << 8) | CHIP_ID;
    2532:	4333      	orrs	r3, r6
    2534:	6093      	str	r3, [r2, #8]
                radio_data_arr[1] = goc_data_full & 0xFF;
    2536:	6823      	ldr	r3, [r4, #0]
    2538:	402b      	ands	r3, r5
    253a:	6053      	str	r3, [r2, #4]
                radio_data_arr[0] = op_counter++;
    253c:	8803      	ldrh	r3, [r0, #0]
    253e:	b29b      	uxth	r3, r3
    2540:	1c59      	adds	r1, r3, #1
    2542:	b289      	uxth	r1, r1
    2544:	8001      	strh	r1, [r0, #0]
                mrr_send_radio_data(0);
    2546:	2000      	movs	r0, #0
        if(mrr_send_enable) {
            do {
                // radio out for N seconds
                radio_data_arr[2] = (0x2 << 8) | CHIP_ID;
                radio_data_arr[1] = goc_data_full & 0xFF;
                radio_data_arr[0] = op_counter++;
    2548:	6013      	str	r3, [r2, #0]
                mrr_send_radio_data(0);
    254a:	f7ff f889 	bl	1660 <mrr_send_radio_data>
                update_system_time();
    254e:	f7ff f82b 	bl	15a8 <update_system_time>
            } while(xo_sys_time_in_sec - start_time_in_sec < N);
    2552:	4b5e      	ldr	r3, [pc, #376]	; (26cc <main+0x86c>)
    2554:	9a02      	ldr	r2, [sp, #8]
    2556:	681b      	ldr	r3, [r3, #0]
    2558:	1a9b      	subs	r3, r3, r2
    255a:	9a00      	ldr	r2, [sp, #0]
    255c:	429a      	cmp	r2, r3
    255e:	d8e4      	bhi.n	252a <main+0x6ca>
    2560:	e7d7      	b.n	2512 <main+0x6b2>
        }

        // set M second timer
        projected_end_time_in_sec = xo_sys_time_in_sec + M;
    }
    else if(goc_data_header == 0x03) {
    2562:	2b03      	cmp	r3, #3
    2564:	d149      	bne.n	25fa <main+0x79a>
        // characterization
        // first set next wake up time before radioing
        uint16_t N = goc_data_full & 0xFFFF;
        if(goc_state == 0) {
    2566:	4a63      	ldr	r2, [pc, #396]	; (26f4 <main+0x894>)
        projected_end_time_in_sec = xo_sys_time_in_sec + M;
    }
    else if(goc_data_header == 0x03) {
        // characterization
        // first set next wake up time before radioing
        uint16_t N = goc_data_full & 0xFFFF;
    2568:	6823      	ldr	r3, [r4, #0]
        if(goc_state == 0) {
    256a:	8811      	ldrh	r1, [r2, #0]
    256c:	041b      	lsls	r3, r3, #16
    256e:	2900      	cmp	r1, #0
    2570:	d105      	bne.n	257e <main+0x71e>
            goc_state = 1;
    2572:	3101      	adds	r1, #1
    2574:	8011      	strh	r1, [r2, #0]
            projected_end_time_in_sec = xo_sys_time_in_sec + N;
    2576:	4a55      	ldr	r2, [pc, #340]	; (26cc <main+0x86c>)
    2578:	0c1b      	lsrs	r3, r3, #16
    257a:	6812      	ldr	r2, [r2, #0]
    257c:	e001      	b.n	2582 <main+0x722>
        }
        else {
            projected_end_time_in_sec += N;
    257e:	683a      	ldr	r2, [r7, #0]
    2580:	0c1b      	lsrs	r3, r3, #16
    2582:	18d3      	adds	r3, r2, r3
    2584:	603b      	str	r3, [r7, #0]
        }

        if(mrr_send_enable) {
    2586:	8833      	ldrh	r3, [r6, #0]
    2588:	2b00      	cmp	r3, #0
    258a:	d101      	bne.n	2590 <main+0x730>
    258c:	f000 ffc2 	bl	3514 <main+0x16b4>
            pmu_setting_temp_based(1);
            update_system_time();
            radio_data_arr[2] = (0x3 << 8) | CHIP_ID;
            radio_data_arr[1] = xo_sys_time_in_sec;
            radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
            mrr_send_radio_data(!option);
    2590:	2601      	movs	r6, #1
        else {
            projected_end_time_in_sec += N;
        }

        if(mrr_send_enable) {
            pmu_setting_temp_based(1);
    2592:	2001      	movs	r0, #1
    2594:	f7fe ffbe 	bl	1514 <pmu_setting_temp_based>
            update_system_time();
    2598:	f7ff f806 	bl	15a8 <update_system_time>
            radio_data_arr[2] = (0x3 << 8) | CHIP_ID;
    259c:	22c0      	movs	r2, #192	; 0xc0
            radio_data_arr[1] = xo_sys_time_in_sec;
            radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
            mrr_send_radio_data(!option);
    259e:	0030      	movs	r0, r6
        }

        if(mrr_send_enable) {
            pmu_setting_temp_based(1);
            update_system_time();
            radio_data_arr[2] = (0x3 << 8) | CHIP_ID;
    25a0:	4b4f      	ldr	r3, [pc, #316]	; (26e0 <main+0x880>)
    25a2:	4c50      	ldr	r4, [pc, #320]	; (26e4 <main+0x884>)
    25a4:	881b      	ldrh	r3, [r3, #0]
    25a6:	0092      	lsls	r2, r2, #2
    25a8:	4313      	orrs	r3, r2
    25aa:	60a3      	str	r3, [r4, #8]
            radio_data_arr[1] = xo_sys_time_in_sec;
    25ac:	4b47      	ldr	r3, [pc, #284]	; (26cc <main+0x86c>)
            radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
            mrr_send_radio_data(!option);
    25ae:	4068      	eors	r0, r5

        if(mrr_send_enable) {
            pmu_setting_temp_based(1);
            update_system_time();
            radio_data_arr[2] = (0x3 << 8) | CHIP_ID;
            radio_data_arr[1] = xo_sys_time_in_sec;
    25b0:	681b      	ldr	r3, [r3, #0]
    25b2:	6063      	str	r3, [r4, #4]
            radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
    25b4:	4b42      	ldr	r3, [pc, #264]	; (26c0 <main+0x860>)
    25b6:	881a      	ldrh	r2, [r3, #0]
    25b8:	4b4b      	ldr	r3, [pc, #300]	; (26e8 <main+0x888>)
    25ba:	0612      	lsls	r2, r2, #24
    25bc:	681b      	ldr	r3, [r3, #0]
    25be:	4313      	orrs	r3, r2
    25c0:	6023      	str	r3, [r4, #0]
            mrr_send_radio_data(!option);
    25c2:	f7ff f84d 	bl	1660 <mrr_send_radio_data>

            // if option is set, send 2 packets
            if(option) {
    25c6:	2d00      	cmp	r5, #0
    25c8:	d101      	bne.n	25ce <main+0x76e>
    25ca:	f000 ffa3 	bl	3514 <main+0x16b4>
                radio_data_arr[2] = (0x3 << 8) | (0x1 << 7) | CHIP_ID;
    25ce:	22e0      	movs	r2, #224	; 0xe0
    25d0:	4b43      	ldr	r3, [pc, #268]	; (26e0 <main+0x880>)
    25d2:	0092      	lsls	r2, r2, #2
    25d4:	881b      	ldrh	r3, [r3, #0]
    25d6:	4313      	orrs	r3, r2
    25d8:	60a3      	str	r3, [r4, #8]
                radio_data_arr[1] = (lnt_meas_time << 16) | lnt_sys_light >> 32;
    25da:	4b48      	ldr	r3, [pc, #288]	; (26fc <main+0x89c>)
    25dc:	4a48      	ldr	r2, [pc, #288]	; (2700 <main+0x8a0>)
    25de:	681b      	ldr	r3, [r3, #0]
    25e0:	6810      	ldr	r0, [r2, #0]
    25e2:	6851      	ldr	r1, [r2, #4]
    25e4:	041b      	lsls	r3, r3, #16
    25e6:	430b      	orrs	r3, r1
    25e8:	6063      	str	r3, [r4, #4]
                radio_data_arr[0] = (uint32_t) lnt_sys_light;
                mrr_send_radio_data(1);
    25ea:	0030      	movs	r0, r6

            // if option is set, send 2 packets
            if(option) {
                radio_data_arr[2] = (0x3 << 8) | (0x1 << 7) | CHIP_ID;
                radio_data_arr[1] = (lnt_meas_time << 16) | lnt_sys_light >> 32;
                radio_data_arr[0] = (uint32_t) lnt_sys_light;
    25ec:	6853      	ldr	r3, [r2, #4]
    25ee:	6812      	ldr	r2, [r2, #0]
    25f0:	6022      	str	r2, [r4, #0]
                mrr_send_radio_data(1);
    25f2:	f7ff f835 	bl	1660 <mrr_send_radio_data>
    25f6:	f000 ff8d 	bl	3514 <main+0x16b4>
            }
        }

    }
    else if(goc_data_header == 0x04) {
    25fa:	2b04      	cmp	r3, #4
    25fc:	d11c      	bne.n	2638 <main+0x7d8>
        // restart xo clock
        xo_init();
    25fe:	f7fd ff99 	bl	534 <xo_init>

        update_system_time();
    2602:	f7fe ffd1 	bl	15a8 <update_system_time>
        radio_data_arr[2] = (0x4 << 8) | CHIP_ID;
    2606:	2280      	movs	r2, #128	; 0x80
    2608:	4e35      	ldr	r6, [pc, #212]	; (26e0 <main+0x880>)
    260a:	4c36      	ldr	r4, [pc, #216]	; (26e4 <main+0x884>)
    260c:	8833      	ldrh	r3, [r6, #0]
    260e:	00d2      	lsls	r2, r2, #3
    2610:	4313      	orrs	r3, r2
    2612:	60a3      	str	r3, [r4, #8]
        radio_data_arr[1] = 0;
    2614:	2300      	movs	r3, #0
        radio_data_arr[0] = xo_sys_time;
    2616:	4d2c      	ldr	r5, [pc, #176]	; (26c8 <main+0x868>)
        // restart xo clock
        xo_init();

        update_system_time();
        radio_data_arr[2] = (0x4 << 8) | CHIP_ID;
        radio_data_arr[1] = 0;
    2618:	6063      	str	r3, [r4, #4]
        radio_data_arr[0] = xo_sys_time;
    261a:	682b      	ldr	r3, [r5, #0]
    261c:	6023      	str	r3, [r4, #0]
        send_beacon();
    261e:	f7ff fba9 	bl	1d74 <send_beacon>

        update_system_time();
    2622:	f7fe ffc1 	bl	15a8 <update_system_time>
        radio_data_arr[2] = (0x4 << 8) | CHIP_ID;
    2626:	2280      	movs	r2, #128	; 0x80
    2628:	8833      	ldrh	r3, [r6, #0]
    262a:	00d2      	lsls	r2, r2, #3
    262c:	4313      	orrs	r3, r2
    262e:	60a3      	str	r3, [r4, #8]
        radio_data_arr[1] = 1;
    2630:	2301      	movs	r3, #1
    2632:	6063      	str	r3, [r4, #4]
        radio_data_arr[0] = xo_sys_time;
    2634:	682b      	ldr	r3, [r5, #0]
    2636:	e379      	b.n	2d2c <main+0xecc>
        send_beacon();
    }
    else if(goc_data_header == 0x05) {
    2638:	2b05      	cmp	r3, #5
    263a:	d107      	bne.n	264c <main+0x7ec>
        // battery drain test: wake up every N seconds and take measurements
        // Which is equivalent to waking up every N seconds
        uint16_t N = goc_data_full & 0xFFFF;
        projected_end_time_in_sec = xo_sys_time_in_sec + N;
    263c:	4b23      	ldr	r3, [pc, #140]	; (26cc <main+0x86c>)
        send_beacon();
    }
    else if(goc_data_header == 0x05) {
        // battery drain test: wake up every N seconds and take measurements
        // Which is equivalent to waking up every N seconds
        uint16_t N = goc_data_full & 0xFFFF;
    263e:	6822      	ldr	r2, [r4, #0]
        projected_end_time_in_sec = xo_sys_time_in_sec + N;
    2640:	681b      	ldr	r3, [r3, #0]
    2642:	b292      	uxth	r2, r2
    2644:	18d3      	adds	r3, r2, r3
    2646:	603b      	str	r3, [r7, #0]
    2648:	f000 ff64 	bl	3514 <main+0x16b4>
    }
    else if(goc_data_header == 0x06) {
    264c:	2b06      	cmp	r3, #6
    264e:	d101      	bne.n	2654 <main+0x7f4>
    2650:	f000 ff60 	bl	3514 <main+0x16b4>
        // Implemented in interrupt servvice handler
    }
    else if(goc_data_header == 0x07) {
    2654:	2b07      	cmp	r3, #7
    2656:	d000      	beq.n	265a <main+0x7fa>
    2658:	e2e3      	b.n	2c22 <main+0xdc2>
        // start operation
        // new operation
        if(goc_state == STATE_INIT) {
    265a:	4b26      	ldr	r3, [pc, #152]	; (26f4 <main+0x894>)
    265c:	881c      	ldrh	r4, [r3, #0]
    265e:	b2a4      	uxth	r4, r4
    2660:	2c00      	cmp	r4, #0
    2662:	d14f      	bne.n	2704 <main+0x8a4>
            // reset all variables so that the system has a clean start
            var_init();
    2664:	f7fd fe06 	bl	274 <var_init>

            goc_state = STATE_VERIFY;
    2668:	2305      	movs	r3, #5
    266a:	4a22      	ldr	r2, [pc, #136]	; (26f4 <main+0x894>)
    266c:	8013      	strh	r3, [r2, #0]

            projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
    266e:	4b17      	ldr	r3, [pc, #92]	; (26cc <main+0x86c>)
    2670:	681b      	ldr	r3, [r3, #0]
    2672:	3378      	adds	r3, #120	; 0x78
    2674:	603b      	str	r3, [r7, #0]
            op_counter = 0;
    2676:	4b20      	ldr	r3, [pc, #128]	; (26f8 <main+0x898>)
    2678:	801c      	strh	r4, [r3, #0]
    267a:	e2aa      	b.n	2bd2 <main+0xd72>
    267c:	00039fff 	.word	0x00039fff
    2680:	ffffbfff 	.word	0xffffbfff
    2684:	00003854 	.word	0x00003854
    2688:	ff0000ff 	.word	0xff0000ff
    268c:	007ac800 	.word	0x007ac800
    2690:	00003858 	.word	0x00003858
    2694:	fffe00ff 	.word	0xfffe00ff
    2698:	00001002 	.word	0x00001002
    269c:	00004e20 	.word	0x00004e20
    26a0:	00003a2e 	.word	0x00003a2e
    26a4:	0d020f0f 	.word	0x0d020f0f
    26a8:	00003986 	.word	0x00003986
    26ac:	0f010102 	.word	0x0f010102
    26b0:	0070a201 	.word	0x0070a201
    26b4:	0017c7ff 	.word	0x0017c7ff
    26b8:	0017efff 	.word	0x0017efff
    26bc:	0008e28b 	.word	0x0008e28b
    26c0:	000039e4 	.word	0x000039e4
    26c4:	00003a60 	.word	0x00003a60
    26c8:	00003aec 	.word	0x00003aec
    26cc:	00003acc 	.word	0x00003acc
    26d0:	000038f0 	.word	0x000038f0
    26d4:	a000a008 	.word	0xa000a008
    26d8:	000038a0 	.word	0x000038a0
    26dc:	00003a28 	.word	0x00003a28
    26e0:	00003abc 	.word	0x00003abc
    26e4:	000039e8 	.word	0x000039e8
    26e8:	000038b8 	.word	0x000038b8
    26ec:	00003874 	.word	0x00003874
    26f0:	00003838 	.word	0x00003838
    26f4:	00003a1c 	.word	0x00003a1c
    26f8:	00003a2c 	.word	0x00003a2c
    26fc:	00003a34 	.word	0x00003a34
    2700:	00003a48 	.word	0x00003a48
        }
        else if(goc_state == STATE_VERIFY) {
    2704:	4bc3      	ldr	r3, [pc, #780]	; (2a14 <main+0xbb4>)
    2706:	881b      	ldrh	r3, [r3, #0]
    2708:	2b05      	cmp	r3, #5
    270a:	d13c      	bne.n	2786 <main+0x926>
            projected_end_time_in_sec += XO_2_MIN;
            op_counter++;
    270c:	48c2      	ldr	r0, [pc, #776]	; (2a18 <main+0xbb8>)

            projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
            op_counter = 0;
        }
        else if(goc_state == STATE_VERIFY) {
            projected_end_time_in_sec += XO_2_MIN;
    270e:	683b      	ldr	r3, [r7, #0]
            op_counter++;
    2710:	8802      	ldrh	r2, [r0, #0]

            projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
            op_counter = 0;
        }
        else if(goc_state == STATE_VERIFY) {
            projected_end_time_in_sec += XO_2_MIN;
    2712:	3378      	adds	r3, #120	; 0x78
            op_counter++;
    2714:	3201      	adds	r2, #1
    2716:	b292      	uxth	r2, r2
    2718:	8002      	strh	r2, [r0, #0]

            if(op_counter <= 2) {
    271a:	8804      	ldrh	r4, [r0, #0]

            projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
            op_counter = 0;
        }
        else if(goc_state == STATE_VERIFY) {
            projected_end_time_in_sec += XO_2_MIN;
    271c:	603b      	str	r3, [r7, #0]
    271e:	4abf      	ldr	r2, [pc, #764]	; (2a1c <main+0xbbc>)
    2720:	49bf      	ldr	r1, [pc, #764]	; (2a20 <main+0xbc0>)
            op_counter++;

            if(op_counter <= 2) {
    2722:	2c02      	cmp	r4, #2
    2724:	d813      	bhi.n	274e <main+0x8ee>
                radio_data_arr[2] = (0xAB << 8) | CHIP_ID;
    2726:	23ab      	movs	r3, #171	; 0xab
    2728:	8812      	ldrh	r2, [r2, #0]
    272a:	021b      	lsls	r3, r3, #8
    272c:	4313      	orrs	r3, r2
    272e:	608b      	str	r3, [r1, #8]
                radio_data_arr[1] = (op_counter << 28) | (lnt_sys_light & 0xFFFFFFF);
    2730:	4bbc      	ldr	r3, [pc, #752]	; (2a24 <main+0xbc4>)
    2732:	8802      	ldrh	r2, [r0, #0]
    2734:	681c      	ldr	r4, [r3, #0]
    2736:	685d      	ldr	r5, [r3, #4]
    2738:	0123      	lsls	r3, r4, #4
    273a:	0712      	lsls	r2, r2, #28
    273c:	091b      	lsrs	r3, r3, #4
    273e:	4313      	orrs	r3, r2
    2740:	604b      	str	r3, [r1, #4]
                radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
    2742:	4bb9      	ldr	r3, [pc, #740]	; (2a28 <main+0xbc8>)
    2744:	881a      	ldrh	r2, [r3, #0]
    2746:	4bb9      	ldr	r3, [pc, #740]	; (2a2c <main+0xbcc>)
    2748:	0612      	lsls	r2, r2, #24
    274a:	681b      	ldr	r3, [r3, #0]
    274c:	e010      	b.n	2770 <main+0x910>
            }
            else {
                radio_data_arr[2] = (0xCD << 8) | CHIP_ID;
    274e:	8814      	ldrh	r4, [r2, #0]
    2750:	22cd      	movs	r2, #205	; 0xcd
    2752:	0212      	lsls	r2, r2, #8
    2754:	4322      	orrs	r2, r4
    2756:	608a      	str	r2, [r1, #8]
                radio_data_arr[1] = (op_counter << 28) | (xo_sys_time_in_sec & 0xFFFFFFF);
    2758:	4ab5      	ldr	r2, [pc, #724]	; (2a30 <main+0xbd0>)
    275a:	8800      	ldrh	r0, [r0, #0]
    275c:	6812      	ldr	r2, [r2, #0]
    275e:	0700      	lsls	r0, r0, #28
    2760:	0112      	lsls	r2, r2, #4
    2762:	0912      	lsrs	r2, r2, #4
    2764:	4302      	orrs	r2, r0
    2766:	604a      	str	r2, [r1, #4]
                radio_data_arr[0] = (VERSION_NUM << 20) | (projected_end_time_in_sec & 0xFFFFF);
    2768:	22c6      	movs	r2, #198	; 0xc6
    276a:	031b      	lsls	r3, r3, #12
    276c:	0b1b      	lsrs	r3, r3, #12
    276e:	05d2      	lsls	r2, r2, #23
    2770:	4313      	orrs	r3, r2
    2772:	600b      	str	r3, [r1, #0]
            }
            if(op_counter >= 4) {
    2774:	4ba8      	ldr	r3, [pc, #672]	; (2a18 <main+0xbb8>)
    2776:	881b      	ldrh	r3, [r3, #0]
    2778:	2b03      	cmp	r3, #3
    277a:	d800      	bhi.n	277e <main+0x91e>
    277c:	e09b      	b.n	28b6 <main+0xa56>
                // go to STATE_WAIT1
                goc_state = STATE_WAIT1;
    277e:	2306      	movs	r3, #6
    2780:	4aa4      	ldr	r2, [pc, #656]	; (2a14 <main+0xbb4>)
    2782:	8013      	strh	r3, [r2, #0]
    2784:	e097      	b.n	28b6 <main+0xa56>
            }
            send_beacon();
        }
        else if(goc_state == STATE_WAIT1) {
    2786:	4ba3      	ldr	r3, [pc, #652]	; (2a14 <main+0xbb4>)
    2788:	881b      	ldrh	r3, [r3, #0]
    278a:	2b06      	cmp	r3, #6
    278c:	d111      	bne.n	27b2 <main+0x952>
            if(day_count + epoch_days_offset >= start_day_count) {
    278e:	4ba9      	ldr	r3, [pc, #676]	; (2a34 <main+0xbd4>)
    2790:	4aa9      	ldr	r2, [pc, #676]	; (2a38 <main+0xbd8>)
    2792:	8819      	ldrh	r1, [r3, #0]
    2794:	4ba9      	ldr	r3, [pc, #676]	; (2a3c <main+0xbdc>)
    2796:	881b      	ldrh	r3, [r3, #0]
    2798:	6812      	ldr	r2, [r2, #0]
    279a:	18cb      	adds	r3, r1, r3
    279c:	4293      	cmp	r3, r2
    279e:	d305      	bcc.n	27ac <main+0x94c>
                initialize_state_collect();
    27a0:	f7ff faf6 	bl	1d90 <initialize_state_collect>
                op_counter = 0;
    27a4:	2200      	movs	r2, #0
    27a6:	4b9c      	ldr	r3, [pc, #624]	; (2a18 <main+0xbb8>)
    27a8:	801a      	strh	r2, [r3, #0]
    27aa:	e212      	b.n	2bd2 <main+0xd72>
            }
            else {
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
    27ac:	4ba4      	ldr	r3, [pc, #656]	; (2a40 <main+0xbe0>)
    27ae:	881b      	ldrh	r3, [r3, #0]
    27b0:	e20c      	b.n	2bcc <main+0xd6c>
            }
        }
        else if(goc_state == STATE_COLLECT) {
    27b2:	4b98      	ldr	r3, [pc, #608]	; (2a14 <main+0xbb4>)
    27b4:	881b      	ldrh	r3, [r3, #0]
    27b6:	2b01      	cmp	r3, #1
    27b8:	d000      	beq.n	27bc <main+0x95c>
    27ba:	e07f      	b.n	28bc <main+0xa5c>
            // check LOW_PWR
            if(set_low_pwr_low_trigger()) {
    27bc:	f7ff fa9a 	bl	1cf4 <set_low_pwr_low_trigger>
    27c0:	4aa0      	ldr	r2, [pc, #640]	; (2a44 <main+0xbe4>)
    27c2:	2800      	cmp	r0, #0
    27c4:	d004      	beq.n	27d0 <main+0x970>
                low_pwr_count++;
    27c6:	8813      	ldrh	r3, [r2, #0]
    27c8:	3301      	adds	r3, #1
    27ca:	b29b      	uxth	r3, r3
    27cc:	8013      	strh	r3, [r2, #0]
    27ce:	e000      	b.n	27d2 <main+0x972>
            }
            else {
                low_pwr_count = 0;
    27d0:	8010      	strh	r0, [r2, #0]
            }
            
            // FIXME: using MID_DAY_TIME for debug here, see if it needs to be removed
            if(day_count + epoch_days_offset >= end_day_count && xo_day_time_in_sec >= data_collection_end_day_time) {
    27d2:	4998      	ldr	r1, [pc, #608]	; (2a34 <main+0xbd4>)
    27d4:	4b99      	ldr	r3, [pc, #612]	; (2a3c <main+0xbdc>)
    27d6:	489c      	ldr	r0, [pc, #624]	; (2a48 <main+0xbe8>)
    27d8:	880c      	ldrh	r4, [r1, #0]
    27da:	881b      	ldrh	r3, [r3, #0]
    27dc:	6800      	ldr	r0, [r0, #0]
    27de:	18e3      	adds	r3, r4, r3
    27e0:	4283      	cmp	r3, r0
    27e2:	d317      	bcc.n	2814 <main+0x9b4>
    27e4:	4b99      	ldr	r3, [pc, #612]	; (2a4c <main+0xbec>)
    27e6:	6818      	ldr	r0, [r3, #0]
    27e8:	4b99      	ldr	r3, [pc, #612]	; (2a50 <main+0xbf0>)
    27ea:	681b      	ldr	r3, [r3, #0]
    27ec:	4298      	cmp	r0, r3
    27ee:	d311      	bcc.n	2814 <main+0x9b4>
                flush_temp_cache();       // store everything in temp_code_storage
    27f0:	f7fe fd42 	bl	1278 <flush_temp_cache>
                flush_code_cache();
    27f4:	f7fe f8ca 	bl	98c <flush_code_cache>
                update_system_time();
    27f8:	f7fe fed6 	bl	15a8 <update_system_time>

                goc_state = STATE_WAIT2;
    27fc:	2307      	movs	r3, #7
    27fe:	4a85      	ldr	r2, [pc, #532]	; (2a14 <main+0xbb4>)
    2800:	8013      	strh	r3, [r2, #0]
                projected_end_time_in_sec += XO_2_MIN;
    2802:	683b      	ldr	r3, [r7, #0]

                // initialize radio variables
                next_beacon_time = 0;
    2804:	4a93      	ldr	r2, [pc, #588]	; (2a54 <main+0xbf4>)
                flush_temp_cache();       // store everything in temp_code_storage
                flush_code_cache();
                update_system_time();

                goc_state = STATE_WAIT2;
                projected_end_time_in_sec += XO_2_MIN;
    2806:	3378      	adds	r3, #120	; 0x78
    2808:	603b      	str	r3, [r7, #0]

                // initialize radio variables
                next_beacon_time = 0;
    280a:	2300      	movs	r3, #0
    280c:	6013      	str	r3, [r2, #0]
                next_data_time = 0;
    280e:	4a92      	ldr	r2, [pc, #584]	; (2a58 <main+0xbf8>)
    2810:	6013      	str	r3, [r2, #0]
    2812:	e1de      	b.n	2bd2 <main+0xd72>
            }
            else if(low_pwr_count >= 3) {
    2814:	8813      	ldrh	r3, [r2, #0]
    2816:	2b02      	cmp	r3, #2
    2818:	d90c      	bls.n	2834 <main+0x9d4>
                // goto LOW_PWR_MODE
                low_pwr_count = 0;
    281a:	2300      	movs	r3, #0
    281c:	8013      	strh	r3, [r2, #0]
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
    281e:	4b88      	ldr	r3, [pc, #544]	; (2a40 <main+0xbe0>)
    2820:	683a      	ldr	r2, [r7, #0]
    2822:	881b      	ldrh	r3, [r3, #0]
    2824:	18d3      	adds	r3, r2, r3
    2826:	603b      	str	r3, [r7, #0]
                low_pwr_state_end_day = day_count + 1;
    2828:	880b      	ldrh	r3, [r1, #0]
    282a:	4a8c      	ldr	r2, [pc, #560]	; (2a5c <main+0xbfc>)
    282c:	3301      	adds	r3, #1
    282e:	6013      	str	r3, [r2, #0]
                goc_state = STATE_LOW_PWR;
    2830:	230a      	movs	r3, #10
    2832:	e1a4      	b.n	2b7e <main+0xd1e>
            }
            else {
                if(projected_end_time_in_sec >= store_temp_timestamp) {
    2834:	4b8a      	ldr	r3, [pc, #552]	; (2a60 <main+0xc00>)
    2836:	6839      	ldr	r1, [r7, #0]
    2838:	681a      	ldr	r2, [r3, #0]
    283a:	4291      	cmp	r1, r2
    283c:	d306      	bcc.n	284c <main+0x9ec>
                    store_temp_timestamp += XO_30_MIN;    // increment by 30 minutes
    283e:	21e1      	movs	r1, #225	; 0xe1
    2840:	681a      	ldr	r2, [r3, #0]
    2842:	00c9      	lsls	r1, r1, #3
    2844:	1852      	adds	r2, r2, r1
    2846:	601a      	str	r2, [r3, #0]

                    sample_temp();
    2848:	f7fe fd58 	bl	12fc <sample_temp>
                }
                
                if(projected_end_time_in_sec == next_light_meas_time) {
    284c:	4b85      	ldr	r3, [pc, #532]	; (2a64 <main+0xc04>)
    284e:	683a      	ldr	r2, [r7, #0]
    2850:	681b      	ldr	r3, [r3, #0]
    2852:	429a      	cmp	r2, r3
    2854:	d101      	bne.n	285a <main+0x9fa>
                    sample_light();
    2856:	f7fe fadf 	bl	e18 <sample_light>
                }

                set_projected_end_time();
    285a:	f7fe fdab 	bl	13b4 <set_projected_end_time>

                // FIXME: this block needs to come before setting the next projected_end_time
                if((op_counter == 0) || (radio_debug && projected_end_time_in_sec >= next_radio_debug_time)) {
    285e:	4b6e      	ldr	r3, [pc, #440]	; (2a18 <main+0xbb8>)
    2860:	881a      	ldrh	r2, [r3, #0]
    2862:	2a00      	cmp	r2, #0
    2864:	d00a      	beq.n	287c <main+0xa1c>
    2866:	4a80      	ldr	r2, [pc, #512]	; (2a68 <main+0xc08>)
    2868:	7812      	ldrb	r2, [r2, #0]
    286a:	2a00      	cmp	r2, #0
    286c:	d100      	bne.n	2870 <main+0xa10>
    286e:	e1b0      	b.n	2bd2 <main+0xd72>
    2870:	4a7e      	ldr	r2, [pc, #504]	; (2a6c <main+0xc0c>)
    2872:	6839      	ldr	r1, [r7, #0]
    2874:	6812      	ldr	r2, [r2, #0]
    2876:	4291      	cmp	r1, r2
    2878:	d200      	bcs.n	287c <main+0xa1c>
    287a:	e1aa      	b.n	2bd2 <main+0xd72>
                    // radioing out on first collect regardless of radio_debug setting
                    op_counter = 1;
    287c:	2201      	movs	r2, #1
                    next_radio_debug_time = xo_sys_time_in_sec + RADIO_DEBUG_PERIOD;
    287e:	496c      	ldr	r1, [pc, #432]	; (2a30 <main+0xbd0>)
                set_projected_end_time();

                // FIXME: this block needs to come before setting the next projected_end_time
                if((op_counter == 0) || (radio_debug && projected_end_time_in_sec >= next_radio_debug_time)) {
                    // radioing out on first collect regardless of radio_debug setting
                    op_counter = 1;
    2880:	801a      	strh	r2, [r3, #0]
                    next_radio_debug_time = xo_sys_time_in_sec + RADIO_DEBUG_PERIOD;
    2882:	4b7b      	ldr	r3, [pc, #492]	; (2a70 <main+0xc10>)
    2884:	680a      	ldr	r2, [r1, #0]
    2886:	681b      	ldr	r3, [r3, #0]
    2888:	18d3      	adds	r3, r2, r3
    288a:	4a78      	ldr	r2, [pc, #480]	; (2a6c <main+0xc0c>)
    288c:	6013      	str	r3, [r2, #0]

                    if(mrr_send_enable) {
    288e:	8833      	ldrh	r3, [r6, #0]
    2890:	2b00      	cmp	r3, #0
    2892:	d100      	bne.n	2896 <main+0xa36>
    2894:	e19d      	b.n	2bd2 <main+0xd72>
                        // FIXME: consider removing this after a while
                        radio_data_arr[2] = (0xDE << 8) | CHIP_ID;
    2896:	22de      	movs	r2, #222	; 0xde
    2898:	4b60      	ldr	r3, [pc, #384]	; (2a1c <main+0xbbc>)
    289a:	0212      	lsls	r2, r2, #8
    289c:	881b      	ldrh	r3, [r3, #0]
    289e:	431a      	orrs	r2, r3
    28a0:	4b5f      	ldr	r3, [pc, #380]	; (2a20 <main+0xbc0>)
    28a2:	609a      	str	r2, [r3, #8]
                        radio_data_arr[1] = xo_sys_time_in_sec;
    28a4:	680a      	ldr	r2, [r1, #0]
    28a6:	605a      	str	r2, [r3, #4]
                        radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
    28a8:	4a5f      	ldr	r2, [pc, #380]	; (2a28 <main+0xbc8>)
    28aa:	8811      	ldrh	r1, [r2, #0]
    28ac:	4a5f      	ldr	r2, [pc, #380]	; (2a2c <main+0xbcc>)
    28ae:	0609      	lsls	r1, r1, #24
    28b0:	6812      	ldr	r2, [r2, #0]
    28b2:	430a      	orrs	r2, r1
    28b4:	601a      	str	r2, [r3, #0]
                        send_beacon();
    28b6:	f7ff fa5d 	bl	1d74 <send_beacon>
    28ba:	e18a      	b.n	2bd2 <main+0xd72>
                    }
                }
            }
        }
        else if(goc_state == STATE_WAIT2) {
    28bc:	4b55      	ldr	r3, [pc, #340]	; (2a14 <main+0xbb4>)
    28be:	881b      	ldrh	r3, [r3, #0]
    28c0:	2b07      	cmp	r3, #7
    28c2:	d000      	beq.n	28c6 <main+0xa66>
    28c4:	e0f8      	b.n	2ab8 <main+0xc58>
            if(projected_end_time_in_sec == next_beacon_time && mrr_send_enable) {
    28c6:	4d63      	ldr	r5, [pc, #396]	; (2a54 <main+0xbf4>)
    28c8:	683a      	ldr	r2, [r7, #0]
    28ca:	682b      	ldr	r3, [r5, #0]
    28cc:	429a      	cmp	r2, r3
    28ce:	d120      	bne.n	2912 <main+0xab2>
    28d0:	8833      	ldrh	r3, [r6, #0]
    28d2:	2b00      	cmp	r3, #0
    28d4:	d01d      	beq.n	2912 <main+0xab2>
                // send beacon
                radio_data_arr[2] = (0xEF << 8) | CHIP_ID;
    28d6:	4b51      	ldr	r3, [pc, #324]	; (2a1c <main+0xbbc>)
                radio_data_arr[1] = (radio_beacon_counter << 28) | (max_unit_count << 20) | xo_day_time_in_sec;
    28d8:	4c66      	ldr	r4, [pc, #408]	; (2a74 <main+0xc14>)
            }
        }
        else if(goc_state == STATE_WAIT2) {
            if(projected_end_time_in_sec == next_beacon_time && mrr_send_enable) {
                // send beacon
                radio_data_arr[2] = (0xEF << 8) | CHIP_ID;
    28da:	881a      	ldrh	r2, [r3, #0]
    28dc:	23ef      	movs	r3, #239	; 0xef
    28de:	021b      	lsls	r3, r3, #8
    28e0:	4313      	orrs	r3, r2
    28e2:	4a4f      	ldr	r2, [pc, #316]	; (2a20 <main+0xbc0>)
    28e4:	6093      	str	r3, [r2, #8]
                radio_data_arr[1] = (radio_beacon_counter << 28) | (max_unit_count << 20) | xo_day_time_in_sec;
    28e6:	4b59      	ldr	r3, [pc, #356]	; (2a4c <main+0xbec>)
    28e8:	6819      	ldr	r1, [r3, #0]
    28ea:	8823      	ldrh	r3, [r4, #0]
    28ec:	0718      	lsls	r0, r3, #28
    28ee:	4b62      	ldr	r3, [pc, #392]	; (2a78 <main+0xc18>)
    28f0:	881b      	ldrh	r3, [r3, #0]
    28f2:	051b      	lsls	r3, r3, #20
    28f4:	4303      	orrs	r3, r0
    28f6:	430b      	orrs	r3, r1
    28f8:	6053      	str	r3, [r2, #4]
                radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
    28fa:	4b4b      	ldr	r3, [pc, #300]	; (2a28 <main+0xbc8>)
    28fc:	8819      	ldrh	r1, [r3, #0]
    28fe:	4b4b      	ldr	r3, [pc, #300]	; (2a2c <main+0xbcc>)
    2900:	0609      	lsls	r1, r1, #24
    2902:	681b      	ldr	r3, [r3, #0]
    2904:	430b      	orrs	r3, r1
    2906:	6013      	str	r3, [r2, #0]
                send_beacon();
    2908:	f7ff fa34 	bl	1d74 <send_beacon>

                radio_beacon_counter++;
    290c:	8823      	ldrh	r3, [r4, #0]
    290e:	3301      	adds	r3, #1
    2910:	8023      	strh	r3, [r4, #0]
            }

            // wait for radio_day_count
            uint16_t epoch_day_count = day_count + epoch_days_offset;
    2912:	4a4a      	ldr	r2, [pc, #296]	; (2a3c <main+0xbdc>)
    2914:	4b47      	ldr	r3, [pc, #284]	; (2a34 <main+0xbd4>)
    2916:	881b      	ldrh	r3, [r3, #0]
    2918:	8816      	ldrh	r6, [r2, #0]
            if(epoch_day_count >= radio_day_count) {
    291a:	4a58      	ldr	r2, [pc, #352]	; (2a7c <main+0xc1c>)

                radio_beacon_counter++;
            }

            // wait for radio_day_count
            uint16_t epoch_day_count = day_count + epoch_days_offset;
    291c:	199e      	adds	r6, r3, r6
            if(epoch_day_count >= radio_day_count) {
    291e:	6813      	ldr	r3, [r2, #0]

                radio_beacon_counter++;
            }

            // wait for radio_day_count
            uint16_t epoch_day_count = day_count + epoch_days_offset;
    2920:	b2b6      	uxth	r6, r6
            if(epoch_day_count >= radio_day_count) {
    2922:	429e      	cmp	r6, r3
    2924:	d30f      	bcc.n	2946 <main+0xae6>
                radio_duty_cycle_end_day = epoch_day_count + RADIO_DUTY_DURATION;
    2926:	4b56      	ldr	r3, [pc, #344]	; (2a80 <main+0xc20>)
    2928:	4956      	ldr	r1, [pc, #344]	; (2a84 <main+0xc24>)
    292a:	881b      	ldrh	r3, [r3, #0]
    292c:	18f3      	adds	r3, r6, r3
    292e:	b29b      	uxth	r3, r3
    2930:	800b      	strh	r3, [r1, #0]
                radio_initial_data_start_day = epoch_day_count + RADIO_INITIAL_DATA_DAY;
    2932:	4b55      	ldr	r3, [pc, #340]	; (2a88 <main+0xc28>)
    2934:	4955      	ldr	r1, [pc, #340]	; (2a8c <main+0xc2c>)
    2936:	881b      	ldrh	r3, [r3, #0]
    2938:	18f3      	adds	r3, r6, r3
    293a:	b29b      	uxth	r3, r3
    293c:	800b      	strh	r3, [r1, #0]
                radio_day_count = epoch_day_count + RADIO_SEQUENCE_PERIOD;
    293e:	4b54      	ldr	r3, [pc, #336]	; (2a90 <main+0xc30>)
    2940:	881b      	ldrh	r3, [r3, #0]
    2942:	18f3      	adds	r3, r6, r3
    2944:	6013      	str	r3, [r2, #0]
            }

            // FIXME: this could be somewhat time consuming
            if(xo_sys_time_in_sec + XO_2_MIN >= next_beacon_time && xo_check_is_day()) {
    2946:	4c3a      	ldr	r4, [pc, #232]	; (2a30 <main+0xbd0>)
    2948:	6823      	ldr	r3, [r4, #0]
    294a:	682a      	ldr	r2, [r5, #0]
    294c:	3378      	adds	r3, #120	; 0x78
    294e:	4293      	cmp	r3, r2
    2950:	d313      	bcc.n	297a <main+0xb1a>
    2952:	f7fe fe6d 	bl	1630 <xo_check_is_day>
    2956:	2800      	cmp	r0, #0
    2958:	d00f      	beq.n	297a <main+0xb1a>
                // find next available beacon time
                uint32_t next_beacon_day_time = RADIO_INITIAL_BEACON_TIME;
    295a:	4b4e      	ldr	r3, [pc, #312]	; (2a94 <main+0xc34>)
                while(xo_day_time_in_sec + XO_2_MIN >= next_beacon_day_time) {
    295c:	493b      	ldr	r1, [pc, #236]	; (2a4c <main+0xbec>)
            }

            // FIXME: this could be somewhat time consuming
            if(xo_sys_time_in_sec + XO_2_MIN >= next_beacon_time && xo_check_is_day()) {
                // find next available beacon time
                uint32_t next_beacon_day_time = RADIO_INITIAL_BEACON_TIME;
    295e:	681b      	ldr	r3, [r3, #0]
                while(xo_day_time_in_sec + XO_2_MIN >= next_beacon_day_time) {
                    next_beacon_day_time += RADIO_BEACON_PERIOD;
    2960:	484d      	ldr	r0, [pc, #308]	; (2a98 <main+0xc38>)

            // FIXME: this could be somewhat time consuming
            if(xo_sys_time_in_sec + XO_2_MIN >= next_beacon_time && xo_check_is_day()) {
                // find next available beacon time
                uint32_t next_beacon_day_time = RADIO_INITIAL_BEACON_TIME;
                while(xo_day_time_in_sec + XO_2_MIN >= next_beacon_day_time) {
    2962:	680a      	ldr	r2, [r1, #0]
    2964:	3278      	adds	r2, #120	; 0x78
    2966:	4293      	cmp	r3, r2
    2968:	d802      	bhi.n	2970 <main+0xb10>
                    next_beacon_day_time += RADIO_BEACON_PERIOD;
    296a:	6802      	ldr	r2, [r0, #0]
    296c:	189b      	adds	r3, r3, r2
    296e:	e7f8      	b.n	2962 <main+0xb02>
                }
                // convert back to sys time
                next_beacon_time = next_beacon_day_time + xo_sys_time_in_sec - xo_day_time_in_sec;
    2970:	6822      	ldr	r2, [r4, #0]
    2972:	6809      	ldr	r1, [r1, #0]
    2974:	1a52      	subs	r2, r2, r1
    2976:	18d3      	adds	r3, r2, r3
    2978:	602b      	str	r3, [r5, #0]
            }
        
            if(xo_sys_time_in_sec + XO_2_MIN >= next_data_time && xo_check_is_day()) {
    297a:	4a37      	ldr	r2, [pc, #220]	; (2a58 <main+0xbf8>)
    297c:	6823      	ldr	r3, [r4, #0]
    297e:	6812      	ldr	r2, [r2, #0]
    2980:	3378      	adds	r3, #120	; 0x78
    2982:	4293      	cmp	r3, r2
    2984:	d315      	bcc.n	29b2 <main+0xb52>
    2986:	f7fe fe53 	bl	1630 <xo_check_is_day>
    298a:	2800      	cmp	r0, #0
    298c:	d011      	beq.n	29b2 <main+0xb52>
                // find next available data time
                uint32_t next_data_day_time = RADIO_INITIAL_DATA_TIME;
    298e:	4b43      	ldr	r3, [pc, #268]	; (2a9c <main+0xc3c>)
                while(xo_day_time_in_sec + XO_2_MIN >= next_data_day_time) {
    2990:	492e      	ldr	r1, [pc, #184]	; (2a4c <main+0xbec>)
                next_beacon_time = next_beacon_day_time + xo_sys_time_in_sec - xo_day_time_in_sec;
            }
        
            if(xo_sys_time_in_sec + XO_2_MIN >= next_data_time && xo_check_is_day()) {
                // find next available data time
                uint32_t next_data_day_time = RADIO_INITIAL_DATA_TIME;
    2992:	881b      	ldrh	r3, [r3, #0]
                while(xo_day_time_in_sec + XO_2_MIN >= next_data_day_time) {
                    next_data_day_time += RADIO_DATA_PERIOD;
    2994:	4842      	ldr	r0, [pc, #264]	; (2aa0 <main+0xc40>)
                next_beacon_time = next_beacon_day_time + xo_sys_time_in_sec - xo_day_time_in_sec;
            }
        
            if(xo_sys_time_in_sec + XO_2_MIN >= next_data_time && xo_check_is_day()) {
                // find next available data time
                uint32_t next_data_day_time = RADIO_INITIAL_DATA_TIME;
    2996:	b29b      	uxth	r3, r3
                while(xo_day_time_in_sec + XO_2_MIN >= next_data_day_time) {
    2998:	680a      	ldr	r2, [r1, #0]
    299a:	3278      	adds	r2, #120	; 0x78
    299c:	4293      	cmp	r3, r2
    299e:	d802      	bhi.n	29a6 <main+0xb46>
                    next_data_day_time += RADIO_DATA_PERIOD;
    29a0:	8802      	ldrh	r2, [r0, #0]
    29a2:	189b      	adds	r3, r3, r2
    29a4:	e7f8      	b.n	2998 <main+0xb38>
                }
                // convert back to sys time
                next_data_time = next_data_day_time + xo_sys_time_in_sec - xo_day_time_in_sec;
    29a6:	6822      	ldr	r2, [r4, #0]
    29a8:	6809      	ldr	r1, [r1, #0]
    29aa:	1a52      	subs	r2, r2, r1
    29ac:	18d3      	adds	r3, r2, r3
    29ae:	4a2a      	ldr	r2, [pc, #168]	; (2a58 <main+0xbf8>)
    29b0:	6013      	str	r3, [r2, #0]
            }
        
            // set projected end time
            // don't check mrr_send_enable here
            bool common_flag = (epoch_day_count < radio_duty_cycle_end_day
    29b2:	4b34      	ldr	r3, [pc, #208]	; (2a84 <main+0xc24>)
                                && xo_check_is_day());
    29b4:	2000      	movs	r0, #0
                next_data_time = next_data_day_time + xo_sys_time_in_sec - xo_day_time_in_sec;
            }
        
            // set projected end time
            // don't check mrr_send_enable here
            bool common_flag = (epoch_day_count < radio_duty_cycle_end_day
    29b6:	881b      	ldrh	r3, [r3, #0]
                                && xo_check_is_day());
    29b8:	429e      	cmp	r6, r3
    29ba:	d201      	bcs.n	29c0 <main+0xb60>
    29bc:	f7fe fe38 	bl	1630 <xo_check_is_day>
            uint32_t default_projected_end_time = projected_end_time_in_sec + PMU_WAKEUP_INTERVAL + XO_2_MIN;
    29c0:	683b      	ldr	r3, [r7, #0]
    29c2:	491f      	ldr	r1, [pc, #124]	; (2a40 <main+0xbe0>)
    29c4:	001c      	movs	r4, r3
    29c6:	880a      	ldrh	r2, [r1, #0]
    29c8:	3478      	adds	r4, #120	; 0x78
    29ca:	18a2      	adds	r2, r4, r2

            // check if can send data first
            if(epoch_day_count >= radio_initial_data_start_day
    29cc:	4c2f      	ldr	r4, [pc, #188]	; (2a8c <main+0xc2c>)
    29ce:	8824      	ldrh	r4, [r4, #0]
    29d0:	42a6      	cmp	r6, r4
    29d2:	d316      	bcc.n	2a02 <main+0xba2>
                    && common_flag
    29d4:	2800      	cmp	r0, #0
    29d6:	d100      	bne.n	29da <main+0xb7a>
    29d8:	e0d4      	b.n	2b84 <main+0xd24>
                    && next_data_time <= default_projected_end_time) {
    29da:	481f      	ldr	r0, [pc, #124]	; (2a58 <main+0xbf8>)
    29dc:	6804      	ldr	r4, [r0, #0]
    29de:	42a2      	cmp	r2, r4
    29e0:	d312      	bcc.n	2a08 <main+0xba8>
                projected_end_time_in_sec = next_data_time;
    29e2:	6803      	ldr	r3, [r0, #0]
                // go to radio data state
                goc_state = STATE_RADIO_DATA;
    29e4:	4a0b      	ldr	r2, [pc, #44]	; (2a14 <main+0xbb4>)

            // check if can send data first
            if(epoch_day_count >= radio_initial_data_start_day
                    && common_flag
                    && next_data_time <= default_projected_end_time) {
                projected_end_time_in_sec = next_data_time;
    29e6:	603b      	str	r3, [r7, #0]
                // go to radio data state
                goc_state = STATE_RADIO_DATA;
    29e8:	2308      	movs	r3, #8
    29ea:	8013      	strh	r3, [r2, #0]

                // initialize radio parameters
                radio_unit_counter = 0;
    29ec:	2300      	movs	r3, #0
    29ee:	4a2d      	ldr	r2, [pc, #180]	; (2aa4 <main+0xc44>)
    29f0:	8013      	strh	r3, [r2, #0]
                check_pmu_counter = 0;
    29f2:	4a2d      	ldr	r2, [pc, #180]	; (2aa8 <main+0xc48>)
    29f4:	8013      	strh	r3, [r2, #0]
                radio_rest_counter = 0;
    29f6:	4a2d      	ldr	r2, [pc, #180]	; (2aac <main+0xc4c>)
    29f8:	8013      	strh	r3, [r2, #0]
                light_packet_num = 0;
    29fa:	4a2d      	ldr	r2, [pc, #180]	; (2ab0 <main+0xc50>)
    29fc:	8013      	strh	r3, [r2, #0]
                temp_packet_num = 0;
    29fe:	4a2d      	ldr	r2, [pc, #180]	; (2ab4 <main+0xc54>)
    2a00:	e0be      	b.n	2b80 <main+0xd20>
            }
            else if(common_flag
    2a02:	2800      	cmp	r0, #0
    2a04:	d100      	bne.n	2a08 <main+0xba8>
    2a06:	e0bd      	b.n	2b84 <main+0xd24>
                    && next_beacon_time <= default_projected_end_time) {
    2a08:	6828      	ldr	r0, [r5, #0]
    2a0a:	4282      	cmp	r2, r0
    2a0c:	d200      	bcs.n	2a10 <main+0xbb0>
    2a0e:	e0b9      	b.n	2b84 <main+0xd24>
                projected_end_time_in_sec = next_beacon_time;
    2a10:	682b      	ldr	r3, [r5, #0]
    2a12:	e0dd      	b.n	2bd0 <main+0xd70>
    2a14:	00003a1c 	.word	0x00003a1c
    2a18:	00003a2c 	.word	0x00003a2c
    2a1c:	00003abc 	.word	0x00003abc
    2a20:	000039e8 	.word	0x000039e8
    2a24:	00003a48 	.word	0x00003a48
    2a28:	000039e4 	.word	0x000039e4
    2a2c:	000038b8 	.word	0x000038b8
    2a30:	00003acc 	.word	0x00003acc
    2a34:	00003a3a 	.word	0x00003a3a
    2a38:	00003ab0 	.word	0x00003ab0
    2a3c:	00003b0c 	.word	0x00003b0c
    2a40:	000038a2 	.word	0x000038a2
    2a44:	00003ab4 	.word	0x00003ab4
    2a48:	00003a5c 	.word	0x00003a5c
    2a4c:	00003a78 	.word	0x00003a78
    2a50:	00003870 	.word	0x00003870
    2a54:	00003ab8 	.word	0x00003ab8
    2a58:	00003ac0 	.word	0x00003ac0
    2a5c:	00003a68 	.word	0x00003a68
    2a60:	00003a54 	.word	0x00003a54
    2a64:	00003a50 	.word	0x00003a50
    2a68:	00003aa0 	.word	0x00003aa0
    2a6c:	00003af0 	.word	0x00003af0
    2a70:	000037fc 	.word	0x000037fc
    2a74:	00003b02 	.word	0x00003b02
    2a78:	00003a42 	.word	0x00003a42
    2a7c:	00003aa8 	.word	0x00003aa8
    2a80:	00003864 	.word	0x00003864
    2a84:	00003ac8 	.word	0x00003ac8
    2a88:	00003898 	.word	0x00003898
    2a8c:	00003a38 	.word	0x00003a38
    2a90:	0000390c 	.word	0x0000390c
    2a94:	0000389c 	.word	0x0000389c
    2a98:	00003904 	.word	0x00003904
    2a9c:	00003900 	.word	0x00003900
    2aa0:	0000394c 	.word	0x0000394c
    2aa4:	00003b04 	.word	0x00003b04
    2aa8:	00003a3c 	.word	0x00003a3c
    2aac:	00003ad0 	.word	0x00003ad0
    2ab0:	00003aa2 	.word	0x00003aa2
    2ab4:	00003a6c 	.word	0x00003a6c
            }
            else {
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
            }
        }
        else if(goc_state == STATE_RADIO_DATA) {
    2ab8:	4bbe      	ldr	r3, [pc, #760]	; (2db4 <main+0xf54>)
    2aba:	881b      	ldrh	r3, [r3, #0]
    2abc:	2b08      	cmp	r3, #8
    2abe:	d14d      	bne.n	2b5c <main+0xcfc>
            if(mrr_send_enable) {
    2ac0:	8833      	ldrh	r3, [r6, #0]
    2ac2:	2b00      	cmp	r3, #0
    2ac4:	d03d      	beq.n	2b42 <main+0xce2>
                while(1) {
                    if(radio_unit_counter >= max_unit_count) {
    2ac6:	4ebc      	ldr	r6, [pc, #752]	; (2db8 <main+0xf58>)
    2ac8:	4bbc      	ldr	r3, [pc, #752]	; (2dbc <main+0xf5c>)
    2aca:	8830      	ldrh	r0, [r6, #0]
    2acc:	881b      	ldrh	r3, [r3, #0]
    2ace:	4283      	cmp	r3, r0
    2ad0:	d806      	bhi.n	2ae0 <main+0xc80>
                        update_system_time();
    2ad2:	f7fe fd69 	bl	15a8 <update_system_time>
                        projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
    2ad6:	4bba      	ldr	r3, [pc, #744]	; (2dc0 <main+0xf60>)
    2ad8:	681b      	ldr	r3, [r3, #0]
    2ada:	3378      	adds	r3, #120	; 0x78
    2adc:	603b      	str	r3, [r7, #0]
    2ade:	e03b      	b.n	2b58 <main+0xcf8>
                        goc_state = STATE_WAIT2;
                        break;
                    }
                    else if(radio_rest_counter >= RADIO_REST_NUM_UNITS) {
    2ae0:	4bb8      	ldr	r3, [pc, #736]	; (2dc4 <main+0xf64>)
    2ae2:	4db9      	ldr	r5, [pc, #740]	; (2dc8 <main+0xf68>)
    2ae4:	881b      	ldrh	r3, [r3, #0]
    2ae6:	882a      	ldrh	r2, [r5, #0]
    2ae8:	4cb8      	ldr	r4, [pc, #736]	; (2dcc <main+0xf6c>)
    2aea:	429a      	cmp	r2, r3
    2aec:	d310      	bcc.n	2b10 <main+0xcb0>
                        radio_rest_counter = 0;
    2aee:	2300      	movs	r3, #0
    2af0:	802b      	strh	r3, [r5, #0]
                        check_pmu_counter = 0;  // Resetting check_pmu counter as well to avoid concatenating the sleep periods
    2af2:	8023      	strh	r3, [r4, #0]
                        update_system_time();
    2af4:	f7fe fd58 	bl	15a8 <update_system_time>
                        radio_rest_end_time = xo_sys_time_in_sec + RADIO_REST_TIME;
    2af8:	4bb5      	ldr	r3, [pc, #724]	; (2dd0 <main+0xf70>)
    2afa:	4ab1      	ldr	r2, [pc, #708]	; (2dc0 <main+0xf60>)
    2afc:	8819      	ldrh	r1, [r3, #0]
    2afe:	6813      	ldr	r3, [r2, #0]
    2b00:	18cb      	adds	r3, r1, r3
    2b02:	49b4      	ldr	r1, [pc, #720]	; (2dd4 <main+0xf74>)
    2b04:	600b      	str	r3, [r1, #0]
                        // NOTE: the system will sleep for 2 minutes before waking up
                        // So radio rest time should be greater than 2 minutes
                        projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
    2b06:	6813      	ldr	r3, [r2, #0]
    2b08:	3378      	adds	r3, #120	; 0x78
    2b0a:	603b      	str	r3, [r7, #0]
                        goc_state = STATE_RADIO_REST;
    2b0c:	2309      	movs	r3, #9
    2b0e:	e036      	b.n	2b7e <main+0xd1e>
                        break;
                    }
                    else if(check_pmu_counter >= CHECK_PMU_NUM_UNITS) {
    2b10:	4bb1      	ldr	r3, [pc, #708]	; (2dd8 <main+0xf78>)
    2b12:	8822      	ldrh	r2, [r4, #0]
    2b14:	881b      	ldrh	r3, [r3, #0]
    2b16:	429a      	cmp	r2, r3
    2b18:	d307      	bcc.n	2b2a <main+0xcca>
                        check_pmu_counter = 0;
    2b1a:	2300      	movs	r3, #0
    2b1c:	8023      	strh	r3, [r4, #0]
                        update_system_time();
    2b1e:	f7fe fd43 	bl	15a8 <update_system_time>
                        projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
    2b22:	4ba7      	ldr	r3, [pc, #668]	; (2dc0 <main+0xf60>)
    2b24:	681b      	ldr	r3, [r3, #0]
    2b26:	3378      	adds	r3, #120	; 0x78
    2b28:	e052      	b.n	2bd0 <main+0xd70>
                        break;
                    }

                    radio_unit(radio_unit_counter);
    2b2a:	f7ff f84b 	bl	1bc4 <radio_unit>
                    radio_unit_counter++;
    2b2e:	8833      	ldrh	r3, [r6, #0]
    2b30:	3301      	adds	r3, #1
    2b32:	8033      	strh	r3, [r6, #0]
                    check_pmu_counter++;
    2b34:	8823      	ldrh	r3, [r4, #0]
    2b36:	3301      	adds	r3, #1
    2b38:	8023      	strh	r3, [r4, #0]
                    radio_rest_counter++;
    2b3a:	882b      	ldrh	r3, [r5, #0]
    2b3c:	3301      	adds	r3, #1
    2b3e:	802b      	strh	r3, [r5, #0]
                }
    2b40:	e7c1      	b.n	2ac6 <main+0xc66>
            }
            else {
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
    2b42:	4ba6      	ldr	r3, [pc, #664]	; (2ddc <main+0xf7c>)
    2b44:	683a      	ldr	r2, [r7, #0]
    2b46:	881b      	ldrh	r3, [r3, #0]
    2b48:	18d3      	adds	r3, r2, r3
    2b4a:	603b      	str	r3, [r7, #0]
                
                // transition back to WAIT2 if too late
                if(xo_day_time_in_sec >= DAY_END_TIME) {
    2b4c:	4ba4      	ldr	r3, [pc, #656]	; (2de0 <main+0xf80>)
    2b4e:	681a      	ldr	r2, [r3, #0]
    2b50:	4ba4      	ldr	r3, [pc, #656]	; (2de4 <main+0xf84>)
    2b52:	681b      	ldr	r3, [r3, #0]
    2b54:	429a      	cmp	r2, r3
    2b56:	d33c      	bcc.n	2bd2 <main+0xd72>
                    goc_state = STATE_WAIT2;
    2b58:	2307      	movs	r3, #7
    2b5a:	e010      	b.n	2b7e <main+0xd1e>
                }
            }
        }
        else if(goc_state == STATE_RADIO_REST) {
    2b5c:	4b95      	ldr	r3, [pc, #596]	; (2db4 <main+0xf54>)
    2b5e:	881b      	ldrh	r3, [r3, #0]
    2b60:	2b09      	cmp	r3, #9
    2b62:	d111      	bne.n	2b88 <main+0xd28>
            // radio_data_arr[2] = 0x3DFB;
            // radio_data_arr[1] = xo_sys_time_in_sec;
            // radio_data_arr[0] = radio_rest_end_time;
            // send_beacon();

            if(projected_end_time_in_sec + PMU_WAKEUP_INTERVAL + XO_2_MIN >= radio_rest_end_time) {
    2b64:	683b      	ldr	r3, [r7, #0]
    2b66:	499d      	ldr	r1, [pc, #628]	; (2ddc <main+0xf7c>)
    2b68:	001d      	movs	r5, r3
    2b6a:	489a      	ldr	r0, [pc, #616]	; (2dd4 <main+0xf74>)
    2b6c:	880a      	ldrh	r2, [r1, #0]
    2b6e:	3578      	adds	r5, #120	; 0x78
    2b70:	6804      	ldr	r4, [r0, #0]
    2b72:	18aa      	adds	r2, r5, r2
    2b74:	42a2      	cmp	r2, r4
    2b76:	d305      	bcc.n	2b84 <main+0xd24>
                projected_end_time_in_sec = radio_rest_end_time;
    2b78:	6803      	ldr	r3, [r0, #0]
    2b7a:	603b      	str	r3, [r7, #0]
                goc_state = STATE_RADIO_DATA;
    2b7c:	2308      	movs	r3, #8
    2b7e:	4a8d      	ldr	r2, [pc, #564]	; (2db4 <main+0xf54>)
    2b80:	8013      	strh	r3, [r2, #0]
    2b82:	e026      	b.n	2bd2 <main+0xd72>
            } 
            else {
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
    2b84:	880a      	ldrh	r2, [r1, #0]
    2b86:	e022      	b.n	2bce <main+0xd6e>
            }
        }
        else if(goc_state == STATE_LOW_PWR) {
    2b88:	4b8a      	ldr	r3, [pc, #552]	; (2db4 <main+0xf54>)
    2b8a:	881b      	ldrh	r3, [r3, #0]
    2b8c:	2b0a      	cmp	r3, #10
    2b8e:	d120      	bne.n	2bd2 <main+0xd72>
            // must pass at least 1 day and be around noon to resume operation
            if(day_count >= low_pwr_state_end_day 
    2b90:	4b95      	ldr	r3, [pc, #596]	; (2de8 <main+0xf88>)
    2b92:	4c92      	ldr	r4, [pc, #584]	; (2ddc <main+0xf7c>)
    2b94:	881a      	ldrh	r2, [r3, #0]
    2b96:	4b95      	ldr	r3, [pc, #596]	; (2dec <main+0xf8c>)
    2b98:	681b      	ldr	r3, [r3, #0]
    2b9a:	429a      	cmp	r2, r3
    2b9c:	d315      	bcc.n	2bca <main+0xd6a>
                && xo_day_time_in_sec >= (MID_DAY_TIME - PMU_WAKEUP_INTERVAL)
    2b9e:	4a90      	ldr	r2, [pc, #576]	; (2de0 <main+0xf80>)
    2ba0:	8820      	ldrh	r0, [r4, #0]
    2ba2:	4b93      	ldr	r3, [pc, #588]	; (2df0 <main+0xf90>)
    2ba4:	6811      	ldr	r1, [r2, #0]
    2ba6:	1a1b      	subs	r3, r3, r0
    2ba8:	428b      	cmp	r3, r1
    2baa:	d80e      	bhi.n	2bca <main+0xd6a>
                && xo_day_time_in_sec <= (MID_DAY_TIME + PMU_WAKEUP_INTERVAL)
    2bac:	8823      	ldrh	r3, [r4, #0]
    2bae:	4990      	ldr	r1, [pc, #576]	; (2df0 <main+0xf90>)
    2bb0:	6812      	ldr	r2, [r2, #0]
    2bb2:	185b      	adds	r3, r3, r1
    2bb4:	4293      	cmp	r3, r2
    2bb6:	d308      	bcc.n	2bca <main+0xd6a>
                && !set_low_pwr_high_trigger()) {
    2bb8:	f7ff f8bc 	bl	1d34 <set_low_pwr_high_trigger>
    2bbc:	2800      	cmp	r0, #0
    2bbe:	d104      	bne.n	2bca <main+0xd6a>
                // reset go back to state collect
                // flushing temp cache is necessary because there's no day state header for temp cache
                flush_temp_cache();
    2bc0:	f7fe fb5a 	bl	1278 <flush_temp_cache>

                initialize_state_collect();
    2bc4:	f7ff f8e4 	bl	1d90 <initialize_state_collect>
    2bc8:	e003      	b.n	2bd2 <main+0xd72>
            }
            else {
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
    2bca:	8823      	ldrh	r3, [r4, #0]
    2bcc:	683a      	ldr	r2, [r7, #0]
    2bce:	18d3      	adds	r3, r2, r3
    2bd0:	603b      	str	r3, [r7, #0]
            }

        }

        // all error checking here
        if(projected_end_time_in_sec <= xo_sys_time_in_sec) {
    2bd2:	4c7b      	ldr	r4, [pc, #492]	; (2dc0 <main+0xf60>)
    2bd4:	683a      	ldr	r2, [r7, #0]
    2bd6:	6823      	ldr	r3, [r4, #0]
    2bd8:	429a      	cmp	r2, r3
    2bda:	d802      	bhi.n	2be2 <main+0xd82>
            set_system_error(0x02);
    2bdc:	2002      	movs	r0, #2
    2bde:	f7fe fcd1 	bl	1584 <set_system_error>
        }

        if(error_code != 0) {
    2be2:	4984      	ldr	r1, [pc, #528]	; (2df4 <main+0xf94>)
    2be4:	4b73      	ldr	r3, [pc, #460]	; (2db4 <main+0xf54>)
    2be6:	680a      	ldr	r2, [r1, #0]
    2be8:	2a00      	cmp	r2, #0
    2bea:	d001      	beq.n	2bf0 <main+0xd90>
            goc_state = STATE_ERROR;
    2bec:	220b      	movs	r2, #11
    2bee:	801a      	strh	r2, [r3, #0]
        }
        
        if(goc_state == STATE_ERROR) {
    2bf0:	881b      	ldrh	r3, [r3, #0]
    2bf2:	2b0b      	cmp	r3, #11
    2bf4:	d001      	beq.n	2bfa <main+0xd9a>
    2bf6:	f000 fc8d 	bl	3514 <main+0x16b4>
            // FIXME: freeze xo_lnt_mplier
            // keep in mind that the xo clock could no longer be running
            radio_data_arr[2] = 0xFF00 | CHIP_ID;
    2bfa:	22ff      	movs	r2, #255	; 0xff
    2bfc:	4b7e      	ldr	r3, [pc, #504]	; (2df8 <main+0xf98>)
    2bfe:	0212      	lsls	r2, r2, #8
    2c00:	881b      	ldrh	r3, [r3, #0]
    2c02:	431a      	orrs	r2, r3
    2c04:	4b7d      	ldr	r3, [pc, #500]	; (2dfc <main+0xf9c>)
    2c06:	609a      	str	r2, [r3, #8]
            radio_data_arr[1] = error_code;
    2c08:	680a      	ldr	r2, [r1, #0]
    2c0a:	605a      	str	r2, [r3, #4]
            radio_data_arr[0] = error_time;
    2c0c:	4a7c      	ldr	r2, [pc, #496]	; (2e00 <main+0xfa0>)
    2c0e:	6812      	ldr	r2, [r2, #0]
    2c10:	601a      	str	r2, [r3, #0]
            send_beacon();
    2c12:	f7ff f8af 	bl	1d74 <send_beacon>
            update_system_time();
    2c16:	f7fe fcc7 	bl	15a8 <update_system_time>
            projected_end_time_in_sec = xo_sys_time_in_sec + PMU_WAKEUP_INTERVAL;
    2c1a:	4b70      	ldr	r3, [pc, #448]	; (2ddc <main+0xf7c>)
    2c1c:	881a      	ldrh	r2, [r3, #0]
    2c1e:	6823      	ldr	r3, [r4, #0]
    2c20:	e510      	b.n	2644 <main+0x7e4>
        }
    }
    else if(goc_data_header == 0x08) {
    2c22:	2b08      	cmp	r3, #8
    2c24:	d111      	bne.n	2c4a <main+0xdea>
        // light huffman code
        uint8_t index = (goc_data_full >> 16) & 0x7F;

        // readonly
        radio_data_arr[2] = (0x8 << 8) | CHIP_ID;
    2c26:	4a74      	ldr	r2, [pc, #464]	; (2df8 <main+0xf98>)
            projected_end_time_in_sec = xo_sys_time_in_sec + PMU_WAKEUP_INTERVAL;
        }
    }
    else if(goc_data_header == 0x08) {
        // light huffman code
        uint8_t index = (goc_data_full >> 16) & 0x7F;
    2c28:	6823      	ldr	r3, [r4, #0]

        // readonly
        radio_data_arr[2] = (0x8 << 8) | CHIP_ID;
    2c2a:	8811      	ldrh	r1, [r2, #0]
    2c2c:	2280      	movs	r2, #128	; 0x80
    2c2e:	0112      	lsls	r2, r2, #4
    2c30:	4311      	orrs	r1, r2
    2c32:	4a72      	ldr	r2, [pc, #456]	; (2dfc <main+0xf9c>)
        radio_data_arr[1] = (index << 4) | light_code_lengths[index];
    2c34:	025b      	lsls	r3, r3, #9
    else if(goc_data_header == 0x08) {
        // light huffman code
        uint8_t index = (goc_data_full >> 16) & 0x7F;

        // readonly
        radio_data_arr[2] = (0x8 << 8) | CHIP_ID;
    2c36:	6091      	str	r1, [r2, #8]
        radio_data_arr[1] = (index << 4) | light_code_lengths[index];
    2c38:	0e5b      	lsrs	r3, r3, #25
    2c3a:	4972      	ldr	r1, [pc, #456]	; (2e04 <main+0xfa4>)
    2c3c:	0118      	lsls	r0, r3, #4
    2c3e:	005b      	lsls	r3, r3, #1
    2c40:	5a59      	ldrh	r1, [r3, r1]
    2c42:	4301      	orrs	r1, r0
    2c44:	6051      	str	r1, [r2, #4]
        radio_data_arr[0] = light_diff_codes[index];
    2c46:	4970      	ldr	r1, [pc, #448]	; (2e08 <main+0xfa8>)
    2c48:	e012      	b.n	2c70 <main+0xe10>
        send_beacon();
    }
    else if(goc_data_header == 0x09) {
    2c4a:	2b09      	cmp	r3, #9
    2c4c:	d113      	bne.n	2c76 <main+0xe16>
        // if(option) {
        //     temp_diff_codes[index] = goc_data_full & 0xFFF;
        //     temp_code_lengths[index] = (goc_data_full >> 12) & 0xF;
        // }

        radio_data_arr[2] = (0x9 << 8) | CHIP_ID;
    2c4e:	4a6a      	ldr	r2, [pc, #424]	; (2df8 <main+0xf98>)
        radio_data_arr[0] = light_diff_codes[index];
        send_beacon();
    }
    else if(goc_data_header == 0x09) {
        // temp huffman code
        uint8_t index = (goc_data_full >> 16) & 0x7F;
    2c50:	6823      	ldr	r3, [r4, #0]
        // if(option) {
        //     temp_diff_codes[index] = goc_data_full & 0xFFF;
        //     temp_code_lengths[index] = (goc_data_full >> 12) & 0xF;
        // }

        radio_data_arr[2] = (0x9 << 8) | CHIP_ID;
    2c52:	8811      	ldrh	r1, [r2, #0]
    2c54:	2290      	movs	r2, #144	; 0x90
    2c56:	0112      	lsls	r2, r2, #4
    2c58:	4311      	orrs	r1, r2
    2c5a:	4a68      	ldr	r2, [pc, #416]	; (2dfc <main+0xf9c>)
        radio_data_arr[1] = (index << 4) | temp_code_lengths[index];
    2c5c:	025b      	lsls	r3, r3, #9
        // if(option) {
        //     temp_diff_codes[index] = goc_data_full & 0xFFF;
        //     temp_code_lengths[index] = (goc_data_full >> 12) & 0xF;
        // }

        radio_data_arr[2] = (0x9 << 8) | CHIP_ID;
    2c5e:	6091      	str	r1, [r2, #8]
        radio_data_arr[1] = (index << 4) | temp_code_lengths[index];
    2c60:	0e5b      	lsrs	r3, r3, #25
    2c62:	496a      	ldr	r1, [pc, #424]	; (2e0c <main+0xfac>)
    2c64:	0118      	lsls	r0, r3, #4
    2c66:	005b      	lsls	r3, r3, #1
    2c68:	5a59      	ldrh	r1, [r3, r1]
    2c6a:	4301      	orrs	r1, r0
    2c6c:	6051      	str	r1, [r2, #4]
        radio_data_arr[0] = temp_diff_codes[index];
    2c6e:	4968      	ldr	r1, [pc, #416]	; (2e10 <main+0xfb0>)
    2c70:	5a5b      	ldrh	r3, [r3, r1]
    2c72:	f000 fbea 	bl	344a <main+0x15ea>
        send_beacon();
    }
    else if(goc_data_header == 0x0A) {
    2c76:	2b0a      	cmp	r3, #10
    2c78:	d112      	bne.n	2ca0 <main+0xe40>
    2c7a:	495b      	ldr	r1, [pc, #364]	; (2de8 <main+0xf88>)
    2c7c:	4865      	ldr	r0, [pc, #404]	; (2e14 <main+0xfb4>)
        // epoch days
        if(option) {
    2c7e:	2d00      	cmp	r5, #0
    2c80:	d004      	beq.n	2c8c <main+0xe2c>
            day_count = 0;
    2c82:	2300      	movs	r3, #0
    2c84:	800b      	strh	r3, [r1, #0]
            epoch_days_offset = goc_data_full & 0xFFFF;
    2c86:	6823      	ldr	r3, [r4, #0]
    2c88:	b29b      	uxth	r3, r3
    2c8a:	8003      	strh	r3, [r0, #0]
        }

        radio_data_arr[2] = (0xA << 8) | CHIP_ID;
    2c8c:	4b5a      	ldr	r3, [pc, #360]	; (2df8 <main+0xf98>)
    2c8e:	881a      	ldrh	r2, [r3, #0]
    2c90:	23a0      	movs	r3, #160	; 0xa0
    2c92:	011b      	lsls	r3, r3, #4
    2c94:	431a      	orrs	r2, r3
    2c96:	4b59      	ldr	r3, [pc, #356]	; (2dfc <main+0xf9c>)
    2c98:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = epoch_days_offset;
    2c9a:	8802      	ldrh	r2, [r0, #0]
    2c9c:	b292      	uxth	r2, r2
    2c9e:	e2c1      	b.n	3224 <main+0x13c4>
        radio_data_arr[0] = day_count;
        send_beacon();
    }
    else if(goc_data_header == 0x0B) {
    2ca0:	2b0b      	cmp	r3, #11
    2ca2:	d145      	bne.n	2d30 <main+0xed0>
    2ca4:	4e46      	ldr	r6, [pc, #280]	; (2dc0 <main+0xf60>)
        // epoch time
        if(option) {
    2ca6:	2d00      	cmp	r5, #0
    2ca8:	d010      	beq.n	2ccc <main+0xe6c>
            uint8_t H = (goc_data_full >> 6) & 0x1F;
            uint8_t M = goc_data_full & 0x3F;
            xo_day_time_in_sec = H * 3600 + M * 60;
    2caa:	23e1      	movs	r3, #225	; 0xe1
        send_beacon();
    }
    else if(goc_data_header == 0x0B) {
        // epoch time
        if(option) {
            uint8_t H = (goc_data_full >> 6) & 0x1F;
    2cac:	6822      	ldr	r2, [r4, #0]
            uint8_t M = goc_data_full & 0x3F;
            xo_day_time_in_sec = H * 3600 + M * 60;
    2cae:	011b      	lsls	r3, r3, #4
    2cb0:	0552      	lsls	r2, r2, #21
    2cb2:	0ed2      	lsrs	r2, r2, #27
    2cb4:	435a      	muls	r2, r3
    2cb6:	233f      	movs	r3, #63	; 0x3f
    }
    else if(goc_data_header == 0x0B) {
        // epoch time
        if(option) {
            uint8_t H = (goc_data_full >> 6) & 0x1F;
            uint8_t M = goc_data_full & 0x3F;
    2cb8:	6821      	ldr	r1, [r4, #0]
            xo_day_time_in_sec = H * 3600 + M * 60;
    2cba:	4019      	ands	r1, r3
    2cbc:	3b03      	subs	r3, #3
    2cbe:	434b      	muls	r3, r1
    2cc0:	18d3      	adds	r3, r2, r3
    2cc2:	4a47      	ldr	r2, [pc, #284]	; (2de0 <main+0xf80>)
    2cc4:	6013      	str	r3, [r2, #0]
            sys_sec_to_min_offset = xo_sys_time_in_sec;
    2cc6:	6832      	ldr	r2, [r6, #0]
    2cc8:	4b53      	ldr	r3, [pc, #332]	; (2e18 <main+0xfb8>)
    2cca:	601a      	str	r2, [r3, #0]
        }

        radio_data_arr[2] = (0xB << 8) | CHIP_ID;
    2ccc:	22b0      	movs	r2, #176	; 0xb0
    2cce:	4b4a      	ldr	r3, [pc, #296]	; (2df8 <main+0xf98>)
    2cd0:	4c4a      	ldr	r4, [pc, #296]	; (2dfc <main+0xf9c>)
    2cd2:	881b      	ldrh	r3, [r3, #0]
    2cd4:	0112      	lsls	r2, r2, #4
    2cd6:	4313      	orrs	r3, r2
    2cd8:	60a3      	str	r3, [r4, #8]
        radio_data_arr[1] = mult(86400, epoch_days_offset) + xo_day_time_in_sec; // epoch_time_offset
    2cda:	4b4e      	ldr	r3, [pc, #312]	; (2e14 <main+0xfb4>)
    2cdc:	484f      	ldr	r0, [pc, #316]	; (2e1c <main+0xfbc>)
    2cde:	8819      	ldrh	r1, [r3, #0]
    2ce0:	b289      	uxth	r1, r1
    2ce2:	f7fd fd89 	bl	7f8 <mult>
    2ce6:	4b3e      	ldr	r3, [pc, #248]	; (2de0 <main+0xf80>)
        radio_data_arr[0] = divide_by_60(xo_sys_time_in_sec - sys_sec_to_min_offset); // sys_time_in_min
    2ce8:	4d4b      	ldr	r5, [pc, #300]	; (2e18 <main+0xfb8>)
            xo_day_time_in_sec = H * 3600 + M * 60;
            sys_sec_to_min_offset = xo_sys_time_in_sec;
        }

        radio_data_arr[2] = (0xB << 8) | CHIP_ID;
        radio_data_arr[1] = mult(86400, epoch_days_offset) + xo_day_time_in_sec; // epoch_time_offset
    2cea:	681b      	ldr	r3, [r3, #0]
    2cec:	1818      	adds	r0, r3, r0
    2cee:	6060      	str	r0, [r4, #4]
        radio_data_arr[0] = divide_by_60(xo_sys_time_in_sec - sys_sec_to_min_offset); // sys_time_in_min
    2cf0:	6830      	ldr	r0, [r6, #0]
    2cf2:	682b      	ldr	r3, [r5, #0]
    2cf4:	1ac0      	subs	r0, r0, r3
    2cf6:	f7fd fc9d 	bl	634 <divide_by_60>
    2cfa:	6020      	str	r0, [r4, #0]
        send_beacon();
    2cfc:	f7ff f83a 	bl	1d74 <send_beacon>

        radio_data_arr[2] = (0xB << 8) | (0x1 << 6) | CHIP_ID;
    2d00:	22b4      	movs	r2, #180	; 0xb4
    2d02:	4b3d      	ldr	r3, [pc, #244]	; (2df8 <main+0xf98>)
    2d04:	0112      	lsls	r2, r2, #4
    2d06:	881b      	ldrh	r3, [r3, #0]
    2d08:	4313      	orrs	r3, r2
    2d0a:	60a3      	str	r3, [r4, #8]
        radio_data_arr[1] = xo_sys_time_in_sec;
    2d0c:	6833      	ldr	r3, [r6, #0]
    2d0e:	6063      	str	r3, [r4, #4]
        radio_data_arr[0] = sys_sec_to_min_offset;
    2d10:	682b      	ldr	r3, [r5, #0]
    2d12:	6023      	str	r3, [r4, #0]
        send_beacon();
    2d14:	f7ff f82e 	bl	1d74 <send_beacon>

        radio_data_arr[2] = (0xB << 8) | (0x2 << 6) | CHIP_ID;
    2d18:	22b8      	movs	r2, #184	; 0xb8
    2d1a:	4b37      	ldr	r3, [pc, #220]	; (2df8 <main+0xf98>)
    2d1c:	0112      	lsls	r2, r2, #4
    2d1e:	881b      	ldrh	r3, [r3, #0]
    2d20:	4313      	orrs	r3, r2
    2d22:	60a3      	str	r3, [r4, #8]
        radio_data_arr[1] = 0;
    2d24:	2300      	movs	r3, #0
    2d26:	6063      	str	r3, [r4, #4]
        radio_data_arr[0] = xo_day_time_in_sec;
    2d28:	4b2d      	ldr	r3, [pc, #180]	; (2de0 <main+0xf80>)
    2d2a:	681b      	ldr	r3, [r3, #0]
    2d2c:	6023      	str	r3, [r4, #0]
    2d2e:	e38d      	b.n	344c <main+0x15ec>
        send_beacon();
    }
    else if(goc_data_header == 0x0C) {
    2d30:	2b0c      	cmp	r3, #12
    2d32:	d127      	bne.n	2d84 <main+0xf24>
        uint8_t option = (goc_data_full >> 21) & 0x7;
    2d34:	6823      	ldr	r3, [r4, #0]
        uint16_t N = goc_data_full & 0xFFFF;
    2d36:	6822      	ldr	r2, [r4, #0]
        radio_data_arr[1] = 0;
        radio_data_arr[0] = xo_day_time_in_sec;
        send_beacon();
    }
    else if(goc_data_header == 0x0C) {
        uint8_t option = (goc_data_full >> 21) & 0x7;
    2d38:	021b      	lsls	r3, r3, #8
    2d3a:	0f5b      	lsrs	r3, r3, #29
        uint16_t N = goc_data_full & 0xFFFF;
    2d3c:	b292      	uxth	r2, r2
    2d3e:	4d38      	ldr	r5, [pc, #224]	; (2e20 <main+0xfc0>)
    2d40:	4938      	ldr	r1, [pc, #224]	; (2e24 <main+0xfc4>)
    2d42:	4839      	ldr	r0, [pc, #228]	; (2e28 <main+0xfc8>)
    2d44:	4c39      	ldr	r4, [pc, #228]	; (2e2c <main+0xfcc>)
        if(option == 1) {
    2d46:	2b01      	cmp	r3, #1
    2d48:	d101      	bne.n	2d4e <main+0xeee>
            xo_to_sec_mplier = N;
    2d4a:	802a      	strh	r2, [r5, #0]
    2d4c:	e00a      	b.n	2d64 <main+0xf04>
        }
        else if(option == 2) {
    2d4e:	2b02      	cmp	r3, #2
    2d50:	d101      	bne.n	2d56 <main+0xef6>
            xo_lnt_mplier = N;
    2d52:	8002      	strh	r2, [r0, #0]
    2d54:	e006      	b.n	2d64 <main+0xf04>
        }
        else if(option == 3) {
    2d56:	2b03      	cmp	r3, #3
    2d58:	d101      	bne.n	2d5e <main+0xefe>
            XO_TO_SEC_MPLIER_SHIFT = N;
    2d5a:	800a      	strh	r2, [r1, #0]
    2d5c:	e002      	b.n	2d64 <main+0xf04>
        }
        else if(option == 4) {
    2d5e:	2b04      	cmp	r3, #4
    2d60:	d100      	bne.n	2d64 <main+0xf04>
            LNT_MPLIER_SHIFT = N;
    2d62:	8022      	strh	r2, [r4, #0]
        }

        radio_data_arr[2] = (0xC << 8) | CHIP_ID;
    2d64:	4b24      	ldr	r3, [pc, #144]	; (2df8 <main+0xf98>)
    2d66:	881a      	ldrh	r2, [r3, #0]
    2d68:	23c0      	movs	r3, #192	; 0xc0
    2d6a:	011b      	lsls	r3, r3, #4
    2d6c:	431a      	orrs	r2, r3
    2d6e:	4b23      	ldr	r3, [pc, #140]	; (2dfc <main+0xf9c>)
    2d70:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = (XO_TO_SEC_MPLIER_SHIFT << 16) | xo_to_sec_mplier;
    2d72:	8809      	ldrh	r1, [r1, #0]
    2d74:	882a      	ldrh	r2, [r5, #0]
    2d76:	0409      	lsls	r1, r1, #16
    2d78:	430a      	orrs	r2, r1
    2d7a:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = (LNT_MPLIER_SHIFT << 16) | xo_lnt_mplier;
    2d7c:	8822      	ldrh	r2, [r4, #0]
    2d7e:	0411      	lsls	r1, r2, #16
    2d80:	8802      	ldrh	r2, [r0, #0]
    2d82:	e291      	b.n	32a8 <main+0x1448>
        send_beacon();
    }
    else if(goc_data_header == 0x0D) {
    2d84:	2b0d      	cmp	r3, #13
    2d86:	d155      	bne.n	2e34 <main+0xfd4>
        uint8_t index = (goc_data_full >> 21) & 0x3;
    2d88:	6823      	ldr	r3, [r4, #0]
    2d8a:	4e29      	ldr	r6, [pc, #164]	; (2e30 <main+0xfd0>)
    2d8c:	025b      	lsls	r3, r3, #9
    2d8e:	0f9b      	lsrs	r3, r3, #30
    2d90:	0058      	lsls	r0, r3, #1
        if(option) {
    2d92:	2d00      	cmp	r5, #0
    2d94:	d003      	beq.n	2d9e <main+0xf3e>
            resample_indices[index] = goc_data_full & 0x3F;
    2d96:	223f      	movs	r2, #63	; 0x3f
    2d98:	6821      	ldr	r1, [r4, #0]
    2d9a:	400a      	ands	r2, r1
    2d9c:	5232      	strh	r2, [r6, r0]
        }

        radio_data_arr[2] = (0xD << 8) | CHIP_ID;
    2d9e:	4a16      	ldr	r2, [pc, #88]	; (2df8 <main+0xf98>)
    2da0:	8811      	ldrh	r1, [r2, #0]
    2da2:	22d0      	movs	r2, #208	; 0xd0
    2da4:	0112      	lsls	r2, r2, #4
    2da6:	4311      	orrs	r1, r2
    2da8:	4a14      	ldr	r2, [pc, #80]	; (2dfc <main+0xf9c>)
    2daa:	6091      	str	r1, [r2, #8]
        radio_data_arr[1] = index;
    2dac:	6053      	str	r3, [r2, #4]
        radio_data_arr[0] = resample_indices[index];
    2dae:	5a33      	ldrh	r3, [r6, r0]
    2db0:	b29b      	uxth	r3, r3
    2db2:	e34a      	b.n	344a <main+0x15ea>
    2db4:	00003a1c 	.word	0x00003a1c
    2db8:	00003b04 	.word	0x00003b04
    2dbc:	00003a42 	.word	0x00003a42
    2dc0:	00003acc 	.word	0x00003acc
    2dc4:	00003834 	.word	0x00003834
    2dc8:	00003ad0 	.word	0x00003ad0
    2dcc:	00003a3c 	.word	0x00003a3c
    2dd0:	00003908 	.word	0x00003908
    2dd4:	00003ad4 	.word	0x00003ad4
    2dd8:	000038bc 	.word	0x000038bc
    2ddc:	000038a2 	.word	0x000038a2
    2de0:	00003a78 	.word	0x00003a78
    2de4:	00003818 	.word	0x00003818
    2de8:	00003a3a 	.word	0x00003a3a
    2dec:	00003a68 	.word	0x00003a68
    2df0:	0000a8c0 	.word	0x0000a8c0
    2df4:	00003b14 	.word	0x00003b14
    2df8:	00003abc 	.word	0x00003abc
    2dfc:	000039e8 	.word	0x000039e8
    2e00:	00003a9c 	.word	0x00003a9c
    2e04:	0000374a 	.word	0x0000374a
    2e08:	000036c4 	.word	0x000036c4
    2e0c:	000036ba 	.word	0x000036ba
    2e10:	000036b0 	.word	0x000036b0
    2e14:	00003b0c 	.word	0x00003b0c
    2e18:	00003a70 	.word	0x00003a70
    2e1c:	00015180 	.word	0x00015180
    2e20:	00003932 	.word	0x00003932
    2e24:	00003800 	.word	0x00003800
    2e28:	000038f0 	.word	0x000038f0
    2e2c:	0000382e 	.word	0x0000382e
    2e30:	000038a4 	.word	0x000038a4
        send_beacon();
    }
    else if(goc_data_header == 0x0E) {
    2e34:	2b0e      	cmp	r3, #14
    2e36:	d113      	bne.n	2e60 <main+0x1000>
    2e38:	49b7      	ldr	r1, [pc, #732]	; (3118 <main+0x12b8>)
    2e3a:	4ab8      	ldr	r2, [pc, #736]	; (311c <main+0x12bc>)
        if(option) {
    2e3c:	2d00      	cmp	r5, #0
    2e3e:	d00a      	beq.n	2e56 <main+0xff6>
            cur_sunrise = ((goc_data_full >> 11) & 0x7FF) * 60;
    2e40:	203c      	movs	r0, #60	; 0x3c
    2e42:	6823      	ldr	r3, [r4, #0]
    2e44:	029b      	lsls	r3, r3, #10
    2e46:	0d5b      	lsrs	r3, r3, #21
    2e48:	4343      	muls	r3, r0
    2e4a:	600b      	str	r3, [r1, #0]
            cur_sunset = (goc_data_full & 0x7FF) * 60;
    2e4c:	6823      	ldr	r3, [r4, #0]
    2e4e:	055b      	lsls	r3, r3, #21
    2e50:	0d5b      	lsrs	r3, r3, #21
    2e52:	4343      	muls	r3, r0
    2e54:	6013      	str	r3, [r2, #0]
        }

        radio_data_arr[2] = (0xE << 8) | CHIP_ID;
    2e56:	4bb2      	ldr	r3, [pc, #712]	; (3120 <main+0x12c0>)
    2e58:	8818      	ldrh	r0, [r3, #0]
    2e5a:	23e0      	movs	r3, #224	; 0xe0
    2e5c:	011b      	lsls	r3, r3, #4
    2e5e:	e09c      	b.n	2f9a <main+0x113a>
        radio_data_arr[1] = cur_sunrise;
        radio_data_arr[0] = cur_sunset;
        send_beacon();
    }
    else if(goc_data_header == 0x0F) {
    2e60:	2b0f      	cmp	r3, #15
    2e62:	d13f      	bne.n	2ee4 <main+0x1084>
        uint8_t option = (goc_data_full >> 22) & 0x3;
    2e64:	6825      	ldr	r5, [r4, #0]
        uint8_t N = (goc_data_full >> 8) & 0xFF;
    2e66:	6826      	ldr	r6, [r4, #0]
        uint8_t M = goc_data_full & 0xFF;
    2e68:	6823      	ldr	r3, [r4, #0]
        radio_data_arr[1] = cur_sunrise;
        radio_data_arr[0] = cur_sunset;
        send_beacon();
    }
    else if(goc_data_header == 0x0F) {
        uint8_t option = (goc_data_full >> 22) & 0x3;
    2e6a:	022d      	lsls	r5, r5, #8
    2e6c:	0fad      	lsrs	r5, r5, #30
        uint8_t N = (goc_data_full >> 8) & 0xFF;
        uint8_t M = goc_data_full & 0xFF;
    2e6e:	b2db      	uxtb	r3, r3
    2e70:	48ac      	ldr	r0, [pc, #688]	; (3124 <main+0x12c4>)
    2e72:	4cad      	ldr	r4, [pc, #692]	; (3128 <main+0x12c8>)
        if(option == 1) {
    2e74:	2d01      	cmp	r5, #1
    2e76:	d113      	bne.n	2ea0 <main+0x1040>
        radio_data_arr[0] = cur_sunset;
        send_beacon();
    }
    else if(goc_data_header == 0x0F) {
        uint8_t option = (goc_data_full >> 22) & 0x3;
        uint8_t N = (goc_data_full >> 8) & 0xFF;
    2e78:	0a35      	lsrs	r5, r6, #8
        uint8_t M = goc_data_full & 0xFF;
        if(option == 1) {
            EDGE_MARGIN1 = (N - 1) * 60;
    2e7a:	b2ee      	uxtb	r6, r5
    2e7c:	1e72      	subs	r2, r6, #1
    2e7e:	263c      	movs	r6, #60	; 0x3c
    2e80:	4669      	mov	r1, sp
    2e82:	4372      	muls	r2, r6
            EDGE_MARGIN2 = M * 60;
    2e84:	b29b      	uxth	r3, r3
    else if(goc_data_header == 0x0F) {
        uint8_t option = (goc_data_full >> 22) & 0x3;
        uint8_t N = (goc_data_full >> 8) & 0xFF;
        uint8_t M = goc_data_full & 0xFF;
        if(option == 1) {
            EDGE_MARGIN1 = (N - 1) * 60;
    2e86:	800a      	strh	r2, [r1, #0]
    2e88:	880a      	ldrh	r2, [r1, #0]
            EDGE_MARGIN2 = M * 60;
    2e8a:	435e      	muls	r6, r3
    else if(goc_data_header == 0x0F) {
        uint8_t option = (goc_data_full >> 22) & 0x3;
        uint8_t N = (goc_data_full >> 8) & 0xFF;
        uint8_t M = goc_data_full & 0xFF;
        if(option == 1) {
            EDGE_MARGIN1 = (N - 1) * 60;
    2e8c:	49a7      	ldr	r1, [pc, #668]	; (312c <main+0x12cc>)
            EDGE_MARGIN2 = M * 60;
            IDX_MAX = N + M - 1;
    2e8e:	3b01      	subs	r3, #1
    2e90:	b2ed      	uxtb	r5, r5
    else if(goc_data_header == 0x0F) {
        uint8_t option = (goc_data_full >> 22) & 0x3;
        uint8_t N = (goc_data_full >> 8) & 0xFF;
        uint8_t M = goc_data_full & 0xFF;
        if(option == 1) {
            EDGE_MARGIN1 = (N - 1) * 60;
    2e92:	800a      	strh	r2, [r1, #0]
            EDGE_MARGIN2 = M * 60;
            IDX_MAX = N + M - 1;
    2e94:	18eb      	adds	r3, r5, r3
        uint8_t option = (goc_data_full >> 22) & 0x3;
        uint8_t N = (goc_data_full >> 8) & 0xFF;
        uint8_t M = goc_data_full & 0xFF;
        if(option == 1) {
            EDGE_MARGIN1 = (N - 1) * 60;
            EDGE_MARGIN2 = M * 60;
    2e96:	4aa6      	ldr	r2, [pc, #664]	; (3130 <main+0x12d0>)
            IDX_MAX = N + M - 1;
    2e98:	b29b      	uxth	r3, r3
        uint8_t option = (goc_data_full >> 22) & 0x3;
        uint8_t N = (goc_data_full >> 8) & 0xFF;
        uint8_t M = goc_data_full & 0xFF;
        if(option == 1) {
            EDGE_MARGIN1 = (N - 1) * 60;
            EDGE_MARGIN2 = M * 60;
    2e9a:	8016      	strh	r6, [r2, #0]
            IDX_MAX = N + M - 1;
    2e9c:	8003      	strh	r3, [r0, #0]
    2e9e:	e00b      	b.n	2eb8 <main+0x1058>
        }
        else if(option == 2) {
    2ea0:	2d02      	cmp	r5, #2
    2ea2:	d103      	bne.n	2eac <main+0x104c>
            MAX_EDGE_SHIFT = M * 60;
    2ea4:	353a      	adds	r5, #58	; 0x3a
    2ea6:	436b      	muls	r3, r5
    2ea8:	8023      	strh	r3, [r4, #0]
    2eaa:	e005      	b.n	2eb8 <main+0x1058>
        }
        else if(option == 3) {
    2eac:	2d03      	cmp	r5, #3
    2eae:	d103      	bne.n	2eb8 <main+0x1058>
            PASSIVE_WINDOW_SHIFT = M * 60;
    2eb0:	3539      	adds	r5, #57	; 0x39
    2eb2:	436b      	muls	r3, r5
    2eb4:	4d9f      	ldr	r5, [pc, #636]	; (3134 <main+0x12d4>)
    2eb6:	802b      	strh	r3, [r5, #0]
        }
    
        radio_data_arr[2] = (0xF << 8) | CHIP_ID;
    2eb8:	25f0      	movs	r5, #240	; 0xf0
    2eba:	4b99      	ldr	r3, [pc, #612]	; (3120 <main+0x12c0>)
    2ebc:	012d      	lsls	r5, r5, #4
    2ebe:	881b      	ldrh	r3, [r3, #0]
    2ec0:	432b      	orrs	r3, r5
    2ec2:	4d9d      	ldr	r5, [pc, #628]	; (3138 <main+0x12d8>)
    2ec4:	60ab      	str	r3, [r5, #8]
        radio_data_arr[1] = (MAX_EDGE_SHIFT << 20) | (PASSIVE_WINDOW_SHIFT << 8) | IDX_MAX;
    2ec6:	4b9b      	ldr	r3, [pc, #620]	; (3134 <main+0x12d4>)
    2ec8:	8824      	ldrh	r4, [r4, #0]
    2eca:	881b      	ldrh	r3, [r3, #0]
    2ecc:	0524      	lsls	r4, r4, #20
    2ece:	8800      	ldrh	r0, [r0, #0]
    2ed0:	021b      	lsls	r3, r3, #8
    2ed2:	4323      	orrs	r3, r4
    2ed4:	4303      	orrs	r3, r0
    2ed6:	606b      	str	r3, [r5, #4]
        radio_data_arr[0] = (EDGE_MARGIN1 << 16) | EDGE_MARGIN2;
    2ed8:	4b94      	ldr	r3, [pc, #592]	; (312c <main+0x12cc>)
    2eda:	8819      	ldrh	r1, [r3, #0]
    2edc:	4b94      	ldr	r3, [pc, #592]	; (3130 <main+0x12d0>)
    2ede:	040a      	lsls	r2, r1, #16
    2ee0:	881b      	ldrh	r3, [r3, #0]
    2ee2:	e116      	b.n	3112 <main+0x12b2>
        send_beacon();
    }
    else if(goc_data_header == 0x10) {
    2ee4:	2b10      	cmp	r3, #16
    2ee6:	d143      	bne.n	2f70 <main+0x1110>
        uint8_t settings = (goc_data_full >> 20) & 0x7;
    2ee8:	2207      	movs	r2, #7
    2eea:	6823      	ldr	r3, [r4, #0]
        uint8_t index = (goc_data_full >> 17) & 0x7;
    2eec:	6821      	ldr	r1, [r4, #0]
        radio_data_arr[1] = (MAX_EDGE_SHIFT << 20) | (PASSIVE_WINDOW_SHIFT << 8) | IDX_MAX;
        radio_data_arr[0] = (EDGE_MARGIN1 << 16) | EDGE_MARGIN2;
        send_beacon();
    }
    else if(goc_data_header == 0x10) {
        uint8_t settings = (goc_data_full >> 20) & 0x7;
    2eee:	0d1b      	lsrs	r3, r3, #20
        uint8_t index = (goc_data_full >> 17) & 0x7;
    2ef0:	0c49      	lsrs	r1, r1, #17
        radio_data_arr[1] = (MAX_EDGE_SHIFT << 20) | (PASSIVE_WINDOW_SHIFT << 8) | IDX_MAX;
        radio_data_arr[0] = (EDGE_MARGIN1 << 16) | EDGE_MARGIN2;
        send_beacon();
    }
    else if(goc_data_header == 0x10) {
        uint8_t settings = (goc_data_full >> 20) & 0x7;
    2ef2:	4013      	ands	r3, r2
        uint8_t index = (goc_data_full >> 17) & 0x7;
    2ef4:	4011      	ands	r1, r2
        uint8_t U = (goc_data_full >> 16) & 0x1;
    2ef6:	6826      	ldr	r6, [r4, #0]
        uint16_t N = goc_data_full & 0xFFFF;
        volatile uint32_t* arr = 0;
        if(settings == 0) {
            arr = PMU_ACTIVE_SETTINGS;
    2ef8:	4a90      	ldr	r2, [pc, #576]	; (313c <main+0x12dc>)
    }
    else if(goc_data_header == 0x10) {
        uint8_t settings = (goc_data_full >> 20) & 0x7;
        uint8_t index = (goc_data_full >> 17) & 0x7;
        uint8_t U = (goc_data_full >> 16) & 0x1;
        uint16_t N = goc_data_full & 0xFFFF;
    2efa:	6820      	ldr	r0, [r4, #0]
        volatile uint32_t* arr = 0;
        if(settings == 0) {
    2efc:	2b00      	cmp	r3, #0
    2efe:	d012      	beq.n	2f26 <main+0x10c6>
            arr = PMU_ACTIVE_SETTINGS;
        }
        else if(settings == 1) {
            arr = PMU_RADIO_SETTINGS;
    2f00:	4a8f      	ldr	r2, [pc, #572]	; (3140 <main+0x12e0>)
        uint16_t N = goc_data_full & 0xFFFF;
        volatile uint32_t* arr = 0;
        if(settings == 0) {
            arr = PMU_ACTIVE_SETTINGS;
        }
        else if(settings == 1) {
    2f02:	2b01      	cmp	r3, #1
    2f04:	d00f      	beq.n	2f26 <main+0x10c6>
            arr = PMU_RADIO_SETTINGS;
        }
        else if(settings == 2) {
            arr = PMU_SLEEP_SETTINGS;
    2f06:	4a8f      	ldr	r2, [pc, #572]	; (3144 <main+0x12e4>)
            arr = PMU_ACTIVE_SETTINGS;
        }
        else if(settings == 1) {
            arr = PMU_RADIO_SETTINGS;
        }
        else if(settings == 2) {
    2f08:	2b02      	cmp	r3, #2
    2f0a:	d00c      	beq.n	2f26 <main+0x10c6>
            arr = PMU_SLEEP_SETTINGS;
        }
        else if(settings == 3) {
            arr = PMU_ACTIVE_SAR_SETTINGS;
    2f0c:	4a8e      	ldr	r2, [pc, #568]	; (3148 <main+0x12e8>)
            arr = PMU_RADIO_SETTINGS;
        }
        else if(settings == 2) {
            arr = PMU_SLEEP_SETTINGS;
        }
        else if(settings == 3) {
    2f0e:	2b03      	cmp	r3, #3
    2f10:	d009      	beq.n	2f26 <main+0x10c6>
            arr = PMU_ACTIVE_SAR_SETTINGS;
        }
        else if(settings == 4) {
            arr = PMU_RADIO_SAR_SETTINGS;
    2f12:	4a8e      	ldr	r2, [pc, #568]	; (314c <main+0x12ec>)
            arr = PMU_SLEEP_SETTINGS;
        }
        else if(settings == 3) {
            arr = PMU_ACTIVE_SAR_SETTINGS;
        }
        else if(settings == 4) {
    2f14:	2b04      	cmp	r3, #4
    2f16:	d006      	beq.n	2f26 <main+0x10c6>
            arr = PMU_RADIO_SAR_SETTINGS;
        }
        else if(settings == 5) {
            arr = PMU_SLEEP_SAR_SETTINGS;
    2f18:	4a8d      	ldr	r2, [pc, #564]	; (3150 <main+0x12f0>)
            arr = PMU_ACTIVE_SAR_SETTINGS;
        }
        else if(settings == 4) {
            arr = PMU_RADIO_SAR_SETTINGS;
        }
        else if(settings == 5) {
    2f1a:	2b05      	cmp	r3, #5
    2f1c:	d003      	beq.n	2f26 <main+0x10c6>
            arr = PMU_SLEEP_SAR_SETTINGS;
        }
        else if(settings == 6) {
            arr = PMU_TEMP_THRESH;
    2f1e:	4a8d      	ldr	r2, [pc, #564]	; (3154 <main+0x12f4>)
            arr = PMU_RADIO_SAR_SETTINGS;
        }
        else if(settings == 5) {
            arr = PMU_SLEEP_SAR_SETTINGS;
        }
        else if(settings == 6) {
    2f20:	2b06      	cmp	r3, #6
    2f22:	d000      	beq.n	2f26 <main+0x10c6>
            arr = PMU_TEMP_THRESH;
        }
        else {
            arr = PMU_ADC_THRESH;
    2f24:	4a8c      	ldr	r2, [pc, #560]	; (3158 <main+0x12f8>)
    2f26:	008c      	lsls	r4, r1, #2
        }

        if(option) {
    2f28:	2d00      	cmp	r5, #0
    2f2a:	d013      	beq.n	2f54 <main+0x10f4>
    2f2c:	0400      	lsls	r0, r0, #16
            if(U) {
    2f2e:	03f5      	lsls	r5, r6, #15
    2f30:	d507      	bpl.n	2f42 <main+0x10e2>
                arr[index] &= 0x0000FFFF;
    2f32:	1915      	adds	r5, r2, r4
    2f34:	682e      	ldr	r6, [r5, #0]
    2f36:	b2b6      	uxth	r6, r6
    2f38:	602e      	str	r6, [r5, #0]
                arr[index] |= (N << 16);
    2f3a:	682e      	ldr	r6, [r5, #0]
    2f3c:	4330      	orrs	r0, r6
    2f3e:	6028      	str	r0, [r5, #0]
    2f40:	e008      	b.n	2f54 <main+0x10f4>
            }
            else {
                arr[index] &= 0xFFFF0000;
    2f42:	1916      	adds	r6, r2, r4
    2f44:	6835      	ldr	r5, [r6, #0]
                arr[index] |= N;
    2f46:	0c00      	lsrs	r0, r0, #16
            if(U) {
                arr[index] &= 0x0000FFFF;
                arr[index] |= (N << 16);
            }
            else {
                arr[index] &= 0xFFFF0000;
    2f48:	0c2d      	lsrs	r5, r5, #16
    2f4a:	042d      	lsls	r5, r5, #16
    2f4c:	6035      	str	r5, [r6, #0]
                arr[index] |= N;
    2f4e:	6835      	ldr	r5, [r6, #0]
    2f50:	4328      	orrs	r0, r5
    2f52:	6030      	str	r0, [r6, #0]
            }
        }
    
        radio_data_arr[2] = (0x10 << 8) | CHIP_ID;
    2f54:	4872      	ldr	r0, [pc, #456]	; (3120 <main+0x12c0>)
        radio_data_arr[1] = (settings << 8) | index;
    2f56:	021b      	lsls	r3, r3, #8
                arr[index] &= 0xFFFF0000;
                arr[index] |= N;
            }
        }
    
        radio_data_arr[2] = (0x10 << 8) | CHIP_ID;
    2f58:	8805      	ldrh	r5, [r0, #0]
    2f5a:	2080      	movs	r0, #128	; 0x80
    2f5c:	0140      	lsls	r0, r0, #5
    2f5e:	4305      	orrs	r5, r0
    2f60:	4875      	ldr	r0, [pc, #468]	; (3138 <main+0x12d8>)
        radio_data_arr[1] = (settings << 8) | index;
    2f62:	430b      	orrs	r3, r1
                arr[index] &= 0xFFFF0000;
                arr[index] |= N;
            }
        }
    
        radio_data_arr[2] = (0x10 << 8) | CHIP_ID;
    2f64:	6085      	str	r5, [r0, #8]
        radio_data_arr[1] = (settings << 8) | index;
        radio_data_arr[0] = arr[index];
    2f66:	1912      	adds	r2, r2, r4
                arr[index] |= N;
            }
        }
    
        radio_data_arr[2] = (0x10 << 8) | CHIP_ID;
        radio_data_arr[1] = (settings << 8) | index;
    2f68:	6043      	str	r3, [r0, #4]
        radio_data_arr[0] = arr[index];
    2f6a:	6813      	ldr	r3, [r2, #0]
    2f6c:	6003      	str	r3, [r0, #0]
    2f6e:	e26d      	b.n	344c <main+0x15ec>
        send_beacon();
    }
    else if(goc_data_header == 0x11) {
    2f70:	2b11      	cmp	r3, #17
    2f72:	d119      	bne.n	2fa8 <main+0x1148>
    2f74:	4a79      	ldr	r2, [pc, #484]	; (315c <main+0x12fc>)
    2f76:	497a      	ldr	r1, [pc, #488]	; (3160 <main+0x1300>)
        // day time config
        if(option) {
    2f78:	2d00      	cmp	r5, #0
    2f7a:	d00a      	beq.n	2f92 <main+0x1132>
            DAY_START_TIME = ((goc_data_full >> 11) & 0x7FF) * 60;
    2f7c:	203c      	movs	r0, #60	; 0x3c
    2f7e:	6823      	ldr	r3, [r4, #0]
    2f80:	029b      	lsls	r3, r3, #10
    2f82:	0d5b      	lsrs	r3, r3, #21
    2f84:	4343      	muls	r3, r0
    2f86:	600b      	str	r3, [r1, #0]
            DAY_END_TIME = (goc_data_full & 0x7FF) * 60;
    2f88:	6823      	ldr	r3, [r4, #0]
    2f8a:	055b      	lsls	r3, r3, #21
    2f8c:	0d5b      	lsrs	r3, r3, #21
    2f8e:	4343      	muls	r3, r0
    2f90:	6013      	str	r3, [r2, #0]
        }

        radio_data_arr[2] = (0x11 << 8) | CHIP_ID;
    2f92:	4b63      	ldr	r3, [pc, #396]	; (3120 <main+0x12c0>)
    2f94:	8818      	ldrh	r0, [r3, #0]
    2f96:	2388      	movs	r3, #136	; 0x88
    2f98:	015b      	lsls	r3, r3, #5
    2f9a:	4318      	orrs	r0, r3
    2f9c:	4b66      	ldr	r3, [pc, #408]	; (3138 <main+0x12d8>)
    2f9e:	6098      	str	r0, [r3, #8]
        radio_data_arr[1] = DAY_START_TIME;
    2fa0:	6809      	ldr	r1, [r1, #0]
    2fa2:	6059      	str	r1, [r3, #4]
        radio_data_arr[0] = DAY_END_TIME;
    2fa4:	6812      	ldr	r2, [r2, #0]
    2fa6:	e225      	b.n	33f4 <main+0x1594>
        send_beacon();
    }
    else if(goc_data_header == 0x12) {
    2fa8:	2b12      	cmp	r3, #18
    2faa:	d11c      	bne.n	2fe6 <main+0x1186>
        // Radio config
        uint8_t option2 = (goc_data_full >> 22) & 0x1;
    2fac:	6820      	ldr	r0, [r4, #0]
    2fae:	496d      	ldr	r1, [pc, #436]	; (3164 <main+0x1304>)
    2fb0:	0240      	lsls	r0, r0, #9
        uint8_t N = (goc_data_full >> 17) & 0x1F;
    2fb2:	6822      	ldr	r2, [r4, #0]
        radio_data_arr[0] = DAY_END_TIME;
        send_beacon();
    }
    else if(goc_data_header == 0x12) {
        // Radio config
        uint8_t option2 = (goc_data_full >> 22) & 0x1;
    2fb4:	0fc0      	lsrs	r0, r0, #31
        uint8_t N = (goc_data_full >> 17) & 0x1F;
        uint32_t M = goc_data_full & 0x1FFFF;
    2fb6:	6823      	ldr	r3, [r4, #0]
        if(option) {
    2fb8:	2d00      	cmp	r5, #0
    2fba:	d002      	beq.n	2fc2 <main+0x1162>
            mrr_freq_hopping = N;
    2fbc:	0292      	lsls	r2, r2, #10
    2fbe:	0ed2      	lsrs	r2, r2, #27
    2fc0:	800a      	strh	r2, [r1, #0]
    2fc2:	4c69      	ldr	r4, [pc, #420]	; (3168 <main+0x1308>)
        }
        if(option2) {
    2fc4:	2800      	cmp	r0, #0
    2fc6:	d002      	beq.n	2fce <main+0x116e>
            RADIO_PACKET_DELAY = M;
    2fc8:	03db      	lsls	r3, r3, #15
    2fca:	0bdb      	lsrs	r3, r3, #15
    2fcc:	6023      	str	r3, [r4, #0]
        }
    
        radio_data_arr[2] = (0x12 << 8) | CHIP_ID;
    2fce:	4b54      	ldr	r3, [pc, #336]	; (3120 <main+0x12c0>)
    2fd0:	881a      	ldrh	r2, [r3, #0]
    2fd2:	2390      	movs	r3, #144	; 0x90
    2fd4:	015b      	lsls	r3, r3, #5
    2fd6:	431a      	orrs	r2, r3
    2fd8:	4b57      	ldr	r3, [pc, #348]	; (3138 <main+0x12d8>)
    2fda:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = mrr_freq_hopping;
    2fdc:	880a      	ldrh	r2, [r1, #0]
    2fde:	b292      	uxth	r2, r2
    2fe0:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = RADIO_PACKET_DELAY;
    2fe2:	6822      	ldr	r2, [r4, #0]
    2fe4:	e206      	b.n	33f4 <main+0x1594>
        send_beacon();
    }
    else if(goc_data_header == 0x13) {
    2fe6:	2b13      	cmp	r3, #19
    2fe8:	d11b      	bne.n	3022 <main+0x11c2>
        uint16_t option2 = (goc_data_full >> 22) & 0x1;
    2fea:	6822      	ldr	r2, [r4, #0]
    2fec:	495f      	ldr	r1, [pc, #380]	; (316c <main+0x130c>)
    2fee:	0252      	lsls	r2, r2, #9
        uint16_t N = (goc_data_full >> 17) & 0x1F;
    2ff0:	6823      	ldr	r3, [r4, #0]
        radio_data_arr[1] = mrr_freq_hopping;
        radio_data_arr[0] = RADIO_PACKET_DELAY;
        send_beacon();
    }
    else if(goc_data_header == 0x13) {
        uint16_t option2 = (goc_data_full >> 22) & 0x1;
    2ff2:	0fd2      	lsrs	r2, r2, #31
        uint16_t N = (goc_data_full >> 17) & 0x1F;
        uint16_t M = goc_data_full & 0xFFFF;
    2ff4:	6824      	ldr	r4, [r4, #0]
        if(option) {
    2ff6:	2d00      	cmp	r5, #0
    2ff8:	d002      	beq.n	3000 <main+0x11a0>
        radio_data_arr[0] = RADIO_PACKET_DELAY;
        send_beacon();
    }
    else if(goc_data_header == 0x13) {
        uint16_t option2 = (goc_data_full >> 22) & 0x1;
        uint16_t N = (goc_data_full >> 17) & 0x1F;
    2ffa:	029b      	lsls	r3, r3, #10
    2ffc:	0edb      	lsrs	r3, r3, #27
        uint16_t M = goc_data_full & 0xFFFF;
        if(option) {
            THRESHOLD_IDX_SHIFT = N;
    2ffe:	800b      	strh	r3, [r1, #0]
    3000:	485b      	ldr	r0, [pc, #364]	; (3170 <main+0x1310>)
        }
        if(option2) {
    3002:	2a00      	cmp	r2, #0
    3004:	d001      	beq.n	300a <main+0x11aa>
        send_beacon();
    }
    else if(goc_data_header == 0x13) {
        uint16_t option2 = (goc_data_full >> 22) & 0x1;
        uint16_t N = (goc_data_full >> 17) & 0x1F;
        uint16_t M = goc_data_full & 0xFFFF;
    3006:	b2a3      	uxth	r3, r4
        if(option) {
            THRESHOLD_IDX_SHIFT = N;
        }
        if(option2) {
            EDGE_THRESHOLD = M;
    3008:	8003      	strh	r3, [r0, #0]
        }
    
        radio_data_arr[2] = (0x13 << 8) | CHIP_ID;
    300a:	4b45      	ldr	r3, [pc, #276]	; (3120 <main+0x12c0>)
    300c:	881a      	ldrh	r2, [r3, #0]
    300e:	2398      	movs	r3, #152	; 0x98
    3010:	015b      	lsls	r3, r3, #5
    3012:	431a      	orrs	r2, r3
    3014:	4b48      	ldr	r3, [pc, #288]	; (3138 <main+0x12d8>)
    3016:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = THRESHOLD_IDX_SHIFT;
    3018:	880a      	ldrh	r2, [r1, #0]
    301a:	b292      	uxth	r2, r2
    301c:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = EDGE_THRESHOLD;
    301e:	8802      	ldrh	r2, [r0, #0]
    3020:	e102      	b.n	3228 <main+0x13c8>
        send_beacon();
    }
    else if(goc_data_header == 0x14) {
    3022:	2b14      	cmp	r3, #20
    3024:	d000      	beq.n	3028 <main+0x11c8>
    3026:	e0bb      	b.n	31a0 <main+0x1340>
        uint16_t option = (goc_data_full >> 20) & 0xF;
    3028:	6820      	ldr	r0, [r4, #0]
        uint32_t N = goc_data_full & 0xFFFFF;
    302a:	6823      	ldr	r3, [r4, #0]
        radio_data_arr[1] = THRESHOLD_IDX_SHIFT;
        radio_data_arr[0] = EDGE_THRESHOLD;
        send_beacon();
    }
    else if(goc_data_header == 0x14) {
        uint16_t option = (goc_data_full >> 20) & 0xF;
    302c:	0200      	lsls	r0, r0, #8
        uint32_t N = goc_data_full & 0xFFFFF;
    302e:	031b      	lsls	r3, r3, #12
        radio_data_arr[1] = THRESHOLD_IDX_SHIFT;
        radio_data_arr[0] = EDGE_THRESHOLD;
        send_beacon();
    }
    else if(goc_data_header == 0x14) {
        uint16_t option = (goc_data_full >> 20) & 0xF;
    3030:	0f00      	lsrs	r0, r0, #28
        uint32_t N = goc_data_full & 0xFFFFF;
    3032:	0b1b      	lsrs	r3, r3, #12
    3034:	494f      	ldr	r1, [pc, #316]	; (3174 <main+0x1314>)
    3036:	4a50      	ldr	r2, [pc, #320]	; (3178 <main+0x1318>)
    3038:	4c50      	ldr	r4, [pc, #320]	; (317c <main+0x131c>)

        if(option == 1) {
    303a:	2801      	cmp	r0, #1
    303c:	d102      	bne.n	3044 <main+0x11e4>
            PMU_WAKEUP_INTERVAL = N;
    303e:	b29b      	uxth	r3, r3
    3040:	800b      	strh	r3, [r1, #0]
    3042:	e030      	b.n	30a6 <main+0x1246>
        }
        else if(option == 2) {
    3044:	2802      	cmp	r0, #2
    3046:	d102      	bne.n	304e <main+0x11ee>
            RADIO_SEQUENCE_PERIOD = N;
    3048:	b29b      	uxth	r3, r3
    304a:	484d      	ldr	r0, [pc, #308]	; (3180 <main+0x1320>)
    304c:	e02a      	b.n	30a4 <main+0x1244>
        }
        else if(option == 3) {
    304e:	2803      	cmp	r0, #3
    3050:	d102      	bne.n	3058 <main+0x11f8>
            RADIO_DUTY_DURATION = N;
    3052:	b29b      	uxth	r3, r3
    3054:	8013      	strh	r3, [r2, #0]
    3056:	e026      	b.n	30a6 <main+0x1246>
        }
        else if(option == 4) {
    3058:	2804      	cmp	r0, #4
    305a:	d101      	bne.n	3060 <main+0x1200>
            RADIO_INITIAL_BEACON_TIME = N;
    305c:	4849      	ldr	r0, [pc, #292]	; (3184 <main+0x1324>)
    305e:	e002      	b.n	3066 <main+0x1206>
        }
        else if(option == 5) {
    3060:	2805      	cmp	r0, #5
    3062:	d102      	bne.n	306a <main+0x120a>
            RADIO_BEACON_PERIOD = N;
    3064:	4848      	ldr	r0, [pc, #288]	; (3188 <main+0x1328>)
    3066:	6003      	str	r3, [r0, #0]
    3068:	e01d      	b.n	30a6 <main+0x1246>
        }
        else if(option == 6) {
    306a:	2806      	cmp	r0, #6
    306c:	d102      	bne.n	3074 <main+0x1214>
            RADIO_INITIAL_DATA_DAY = N;
    306e:	b29b      	uxth	r3, r3
    3070:	8023      	strh	r3, [r4, #0]
    3072:	e018      	b.n	30a6 <main+0x1246>
        }
        else if(option == 7) {
    3074:	2807      	cmp	r0, #7
    3076:	d102      	bne.n	307e <main+0x121e>
            RADIO_INITIAL_DATA_TIME = N;
    3078:	b29b      	uxth	r3, r3
    307a:	4844      	ldr	r0, [pc, #272]	; (318c <main+0x132c>)
    307c:	e012      	b.n	30a4 <main+0x1244>
        }
        else if(option == 8) {
    307e:	2808      	cmp	r0, #8
    3080:	d102      	bne.n	3088 <main+0x1228>
            CHECK_PMU_NUM_UNITS = N;
    3082:	b29b      	uxth	r3, r3
    3084:	4842      	ldr	r0, [pc, #264]	; (3190 <main+0x1330>)
    3086:	e00d      	b.n	30a4 <main+0x1244>
        }
        else if(option == 9) {
    3088:	2809      	cmp	r0, #9
    308a:	d102      	bne.n	3092 <main+0x1232>
            RADIO_REST_NUM_UNITS = N;
    308c:	b29b      	uxth	r3, r3
    308e:	4841      	ldr	r0, [pc, #260]	; (3194 <main+0x1334>)
    3090:	e008      	b.n	30a4 <main+0x1244>
        }
        else if(option == 10) {
    3092:	280a      	cmp	r0, #10
    3094:	d102      	bne.n	309c <main+0x123c>
            RADIO_REST_TIME = N;
    3096:	b29b      	uxth	r3, r3
    3098:	483f      	ldr	r0, [pc, #252]	; (3198 <main+0x1338>)
    309a:	e003      	b.n	30a4 <main+0x1244>
        }
        else if(option == 11) {
    309c:	280b      	cmp	r0, #11
    309e:	d102      	bne.n	30a6 <main+0x1246>
            RADIO_DATA_PERIOD = N;
    30a0:	483e      	ldr	r0, [pc, #248]	; (319c <main+0x133c>)
    30a2:	b29b      	uxth	r3, r3
    30a4:	8003      	strh	r3, [r0, #0]
        }

        radio_data_arr[2] = (0x14 << 8) | CHIP_ID;
    30a6:	20a0      	movs	r0, #160	; 0xa0
    30a8:	4e1d      	ldr	r6, [pc, #116]	; (3120 <main+0x12c0>)
    30aa:	4d23      	ldr	r5, [pc, #140]	; (3138 <main+0x12d8>)
    30ac:	8833      	ldrh	r3, [r6, #0]
    30ae:	0140      	lsls	r0, r0, #5
    30b0:	4303      	orrs	r3, r0
    30b2:	60ab      	str	r3, [r5, #8]
        radio_data_arr[1] = (PMU_WAKEUP_INTERVAL << 16) | (RADIO_SEQUENCE_PERIOD << 8) | RADIO_DUTY_DURATION; 
    30b4:	4b32      	ldr	r3, [pc, #200]	; (3180 <main+0x1320>)
    30b6:	8809      	ldrh	r1, [r1, #0]
    30b8:	881b      	ldrh	r3, [r3, #0]
    30ba:	0409      	lsls	r1, r1, #16
    30bc:	8812      	ldrh	r2, [r2, #0]
    30be:	021b      	lsls	r3, r3, #8
    30c0:	430b      	orrs	r3, r1
    30c2:	4313      	orrs	r3, r2
    30c4:	606b      	str	r3, [r5, #4]
        radio_data_arr[0] = RADIO_INITIAL_BEACON_TIME;
    30c6:	4b2f      	ldr	r3, [pc, #188]	; (3184 <main+0x1324>)
    30c8:	681b      	ldr	r3, [r3, #0]
    30ca:	602b      	str	r3, [r5, #0]
        send_beacon();
    30cc:	f7fe fe52 	bl	1d74 <send_beacon>

        radio_data_arr[2] = (0x14 << 8) | (0x1 << 6) | CHIP_ID;
    30d0:	22a2      	movs	r2, #162	; 0xa2
    30d2:	8833      	ldrh	r3, [r6, #0]
    30d4:	0152      	lsls	r2, r2, #5
    30d6:	4313      	orrs	r3, r2
    30d8:	60ab      	str	r3, [r5, #8]
        radio_data_arr[1] = RADIO_BEACON_PERIOD;
    30da:	4b2b      	ldr	r3, [pc, #172]	; (3188 <main+0x1328>)
    30dc:	681b      	ldr	r3, [r3, #0]
    30de:	606b      	str	r3, [r5, #4]
        radio_data_arr[0] = (RADIO_INITIAL_DATA_DAY << 24) | RADIO_INITIAL_DATA_TIME;
    30e0:	4b2a      	ldr	r3, [pc, #168]	; (318c <main+0x132c>)
    30e2:	8822      	ldrh	r2, [r4, #0]
    30e4:	881b      	ldrh	r3, [r3, #0]
    30e6:	0612      	lsls	r2, r2, #24
    30e8:	4313      	orrs	r3, r2
    30ea:	602b      	str	r3, [r5, #0]
        send_beacon();
    30ec:	f7fe fe42 	bl	1d74 <send_beacon>

        radio_data_arr[2] = (0x14 << 8) | (0x2 << 6) | CHIP_ID;
    30f0:	22a4      	movs	r2, #164	; 0xa4
    30f2:	8833      	ldrh	r3, [r6, #0]
    30f4:	0152      	lsls	r2, r2, #5
    30f6:	4313      	orrs	r3, r2
    30f8:	60ab      	str	r3, [r5, #8]
        radio_data_arr[1] = (CHECK_PMU_NUM_UNITS << 20) | RADIO_REST_TIME;
    30fa:	4b25      	ldr	r3, [pc, #148]	; (3190 <main+0x1330>)
    30fc:	881a      	ldrh	r2, [r3, #0]
    30fe:	4b26      	ldr	r3, [pc, #152]	; (3198 <main+0x1338>)
    3100:	0512      	lsls	r2, r2, #20
    3102:	881b      	ldrh	r3, [r3, #0]
    3104:	4313      	orrs	r3, r2
    3106:	606b      	str	r3, [r5, #4]
        radio_data_arr[0] = (RADIO_REST_NUM_UNITS << 20) | RADIO_DATA_PERIOD;
    3108:	4b22      	ldr	r3, [pc, #136]	; (3194 <main+0x1334>)
    310a:	881a      	ldrh	r2, [r3, #0]
    310c:	4b23      	ldr	r3, [pc, #140]	; (319c <main+0x133c>)
    310e:	0512      	lsls	r2, r2, #20
    3110:	881b      	ldrh	r3, [r3, #0]
    3112:	4313      	orrs	r3, r2
    3114:	602b      	str	r3, [r5, #0]
    3116:	e199      	b.n	344c <main+0x15ec>
    3118:	00003ac4 	.word	0x00003ac4
    311c:	00003b1c 	.word	0x00003b1c
    3120:	00003abc 	.word	0x00003abc
    3124:	00003978 	.word	0x00003978
    3128:	00003830 	.word	0x00003830
    312c:	00003954 	.word	0x00003954
    3130:	00003956 	.word	0x00003956
    3134:	00003b20 	.word	0x00003b20
    3138:	000039e8 	.word	0x000039e8
    313c:	000037dc 	.word	0x000037dc
    3140:	00003914 	.word	0x00003914
    3144:	00003874 	.word	0x00003874
    3148:	000038c0 	.word	0x000038c0
    314c:	0000395c 	.word	0x0000395c
    3150:	00003838 	.word	0x00003838
    3154:	00003934 	.word	0x00003934
    3158:	000038dc 	.word	0x000038dc
    315c:	00003818 	.word	0x00003818
    3160:	00003980 	.word	0x00003980
    3164:	00003988 	.word	0x00003988
    3168:	00003950 	.word	0x00003950
    316c:	00003910 	.word	0x00003910
    3170:	00003808 	.word	0x00003808
    3174:	000038a2 	.word	0x000038a2
    3178:	00003864 	.word	0x00003864
    317c:	00003898 	.word	0x00003898
    3180:	0000390c 	.word	0x0000390c
    3184:	0000389c 	.word	0x0000389c
    3188:	00003904 	.word	0x00003904
    318c:	00003900 	.word	0x00003900
    3190:	000038bc 	.word	0x000038bc
    3194:	00003834 	.word	0x00003834
    3198:	00003908 	.word	0x00003908
    319c:	0000394c 	.word	0x0000394c
        send_beacon();
    }
    else if(goc_data_header == 0x15) {
    31a0:	2b15      	cmp	r3, #21
    31a2:	d11b      	bne.n	31dc <main+0x137c>
        uint8_t option2 = (goc_data_full >> 22) & 0x1;
    31a4:	2001      	movs	r0, #1
    31a6:	6823      	ldr	r3, [r4, #0]
    31a8:	4aaa      	ldr	r2, [pc, #680]	; (3454 <main+0x15f4>)
    31aa:	0d9b      	lsrs	r3, r3, #22
    31ac:	4003      	ands	r3, r0
    31ae:	49aa      	ldr	r1, [pc, #680]	; (3458 <main+0x15f8>)
        if(option) {
    31b0:	2d00      	cmp	r5, #0
    31b2:	d009      	beq.n	31c8 <main+0x1368>
            if(option2) {
    31b4:	2b00      	cmp	r3, #0
    31b6:	d006      	beq.n	31c6 <main+0x1366>
                radio_debug = 1;
    31b8:	7010      	strb	r0, [r2, #0]
                uint16_t N = goc_data_full & 0xFFFF;
    31ba:	6823      	ldr	r3, [r4, #0]
                RADIO_DEBUG_PERIOD = N * 60;
    31bc:	b298      	uxth	r0, r3
    31be:	233c      	movs	r3, #60	; 0x3c
    31c0:	4343      	muls	r3, r0
    31c2:	600b      	str	r3, [r1, #0]
    31c4:	e000      	b.n	31c8 <main+0x1368>
            }
            else {
                radio_debug = 0;
    31c6:	7013      	strb	r3, [r2, #0]
            }
        }

        radio_data_arr[2] = (0x15 << 8) | CHIP_ID;
    31c8:	4ba4      	ldr	r3, [pc, #656]	; (345c <main+0x15fc>)
    31ca:	8818      	ldrh	r0, [r3, #0]
    31cc:	23a8      	movs	r3, #168	; 0xa8
    31ce:	015b      	lsls	r3, r3, #5
    31d0:	4318      	orrs	r0, r3
    31d2:	4ba3      	ldr	r3, [pc, #652]	; (3460 <main+0x1600>)
    31d4:	6098      	str	r0, [r3, #8]
        radio_data_arr[1] = radio_debug;
    31d6:	7812      	ldrb	r2, [r2, #0]
    31d8:	b2d2      	uxtb	r2, r2
    31da:	e109      	b.n	33f0 <main+0x1590>
        radio_data_arr[0] = RADIO_DEBUG_PERIOD;
        send_beacon();
    }
    else if(goc_data_header == 0x16) {
    31dc:	2b16      	cmp	r3, #22
    31de:	d125      	bne.n	322c <main+0x13cc>
        uint32_t N = goc_data_full & 0x3FFFFF;
    31e0:	6823      	ldr	r3, [r4, #0]
        uint8_t option2 = (goc_data_full >> 22) & 0x3;
    31e2:	6822      	ldr	r2, [r4, #0]
        radio_data_arr[1] = radio_debug;
        radio_data_arr[0] = RADIO_DEBUG_PERIOD;
        send_beacon();
    }
    else if(goc_data_header == 0x16) {
        uint32_t N = goc_data_full & 0x3FFFFF;
    31e4:	029b      	lsls	r3, r3, #10
        uint8_t option2 = (goc_data_full >> 22) & 0x3;
    31e6:	0212      	lsls	r2, r2, #8
        radio_data_arr[1] = radio_debug;
        radio_data_arr[0] = RADIO_DEBUG_PERIOD;
        send_beacon();
    }
    else if(goc_data_header == 0x16) {
        uint32_t N = goc_data_full & 0x3FFFFF;
    31e8:	0a9b      	lsrs	r3, r3, #10
        uint8_t option2 = (goc_data_full >> 22) & 0x3;
    31ea:	0f92      	lsrs	r2, r2, #30
    31ec:	489d      	ldr	r0, [pc, #628]	; (3464 <main+0x1604>)
    31ee:	499e      	ldr	r1, [pc, #632]	; (3468 <main+0x1608>)
    31f0:	4c9e      	ldr	r4, [pc, #632]	; (346c <main+0x160c>)
        if(option2 == 1) {
    31f2:	2a01      	cmp	r2, #1
    31f4:	d102      	bne.n	31fc <main+0x139c>
            MRR_TEMP_THRESH_LOW = N;
    31f6:	b29b      	uxth	r3, r3
    31f8:	8003      	strh	r3, [r0, #0]
    31fa:	e008      	b.n	320e <main+0x13ae>
        }
        else if(option2 == 2) {
    31fc:	2a02      	cmp	r2, #2
    31fe:	d102      	bne.n	3206 <main+0x13a6>
            MRR_TEMP_THRESH_HIGH = N;
    3200:	b29b      	uxth	r3, r3
    3202:	800b      	strh	r3, [r1, #0]
    3204:	e003      	b.n	320e <main+0x13ae>
        }
        else if(option2 == 3) {
    3206:	2a03      	cmp	r2, #3
    3208:	d101      	bne.n	320e <main+0x13ae>
            OVERRIDE_RAD = N;
    320a:	b29b      	uxth	r3, r3
    320c:	8023      	strh	r3, [r4, #0]
        }

        radio_data_arr[2] = (0x16 << 8) | CHIP_ID;
    320e:	4b93      	ldr	r3, [pc, #588]	; (345c <main+0x15fc>)
    3210:	881a      	ldrh	r2, [r3, #0]
    3212:	23b0      	movs	r3, #176	; 0xb0
    3214:	015b      	lsls	r3, r3, #5
    3216:	431a      	orrs	r2, r3
    3218:	4b91      	ldr	r3, [pc, #580]	; (3460 <main+0x1600>)
    321a:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = (OVERRIDE_RAD << 16) | MRR_TEMP_THRESH_LOW;
    321c:	8824      	ldrh	r4, [r4, #0]
    321e:	8802      	ldrh	r2, [r0, #0]
    3220:	0420      	lsls	r0, r4, #16
    3222:	4302      	orrs	r2, r0
    3224:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = MRR_TEMP_THRESH_HIGH;
    3226:	880a      	ldrh	r2, [r1, #0]
    3228:	b292      	uxth	r2, r2
    322a:	e0e3      	b.n	33f4 <main+0x1594>
        send_beacon();
    }
    else if(goc_data_header == 0x17) {
    322c:	2b17      	cmp	r3, #23
    322e:	d10b      	bne.n	3248 <main+0x13e8>
        // read error code

        radio_data_arr[2] = (0x17 << 8) | CHIP_ID;
    3230:	4b8a      	ldr	r3, [pc, #552]	; (345c <main+0x15fc>)
    3232:	881a      	ldrh	r2, [r3, #0]
    3234:	23b8      	movs	r3, #184	; 0xb8
    3236:	015b      	lsls	r3, r3, #5
    3238:	431a      	orrs	r2, r3
    323a:	4b89      	ldr	r3, [pc, #548]	; (3460 <main+0x1600>)
    323c:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = error_code;
    323e:	4a8c      	ldr	r2, [pc, #560]	; (3470 <main+0x1610>)
    3240:	6812      	ldr	r2, [r2, #0]
    3242:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = error_time;
    3244:	4a8b      	ldr	r2, [pc, #556]	; (3474 <main+0x1614>)
    3246:	e6ad      	b.n	2fa4 <main+0x1144>
        send_beacon();
    }
    else if(goc_data_header == 0x18) {
    3248:	2b18      	cmp	r3, #24
    324a:	d10c      	bne.n	3266 <main+0x1406>
        uint8_t len = (goc_data_full >> 16) & 0xFF;
    324c:	6823      	ldr	r3, [r4, #0]
        uint32_t* N = (uint32_t*) (goc_data_full & 0xFFFF);

        // FIXME: debug
        // uint8_t i;
        // for(i = 0; i < len; i++) {
            radio_data_arr[2] = (0x18 << 8) | CHIP_ID;
    324e:	4b83      	ldr	r3, [pc, #524]	; (345c <main+0x15fc>)
        radio_data_arr[0] = error_time;
        send_beacon();
    }
    else if(goc_data_header == 0x18) {
        uint8_t len = (goc_data_full >> 16) & 0xFF;
        uint32_t* N = (uint32_t*) (goc_data_full & 0xFFFF);
    3250:	6822      	ldr	r2, [r4, #0]

        // FIXME: debug
        // uint8_t i;
        // for(i = 0; i < len; i++) {
            radio_data_arr[2] = (0x18 << 8) | CHIP_ID;
    3252:	8819      	ldrh	r1, [r3, #0]
    3254:	23c0      	movs	r3, #192	; 0xc0
    3256:	015b      	lsls	r3, r3, #5
    3258:	4319      	orrs	r1, r3
    325a:	4b81      	ldr	r3, [pc, #516]	; (3460 <main+0x1600>)
            radio_data_arr[1] = 0;
            radio_data_arr[0] = *N;
    325c:	b292      	uxth	r2, r2
        uint32_t* N = (uint32_t*) (goc_data_full & 0xFFFF);

        // FIXME: debug
        // uint8_t i;
        // for(i = 0; i < len; i++) {
            radio_data_arr[2] = (0x18 << 8) | CHIP_ID;
    325e:	6099      	str	r1, [r3, #8]
            radio_data_arr[1] = 0;
    3260:	2100      	movs	r1, #0
    3262:	6059      	str	r1, [r3, #4]
    3264:	e69e      	b.n	2fa4 <main+0x1144>
            radio_data_arr[0] = *N;
            send_beacon();
        // }
    }
    else if(goc_data_header == 0x19) {
    3266:	2b19      	cmp	r3, #25
    3268:	d120      	bne.n	32ac <main+0x144c>
        uint8_t option = (goc_data_full >> 22) & 0x3;
    326a:	6821      	ldr	r1, [r4, #0]
        uint8_t index = (goc_data_full >> 19) & 0x7;
    326c:	6822      	ldr	r2, [r4, #0]
        uint32_t N = goc_data_full & 0xFF;
    326e:	6823      	ldr	r3, [r4, #0]
            send_beacon();
        // }
    }
    else if(goc_data_header == 0x19) {
        uint8_t option = (goc_data_full >> 22) & 0x3;
        uint8_t index = (goc_data_full >> 19) & 0x7;
    3270:	0292      	lsls	r2, r2, #10
            radio_data_arr[0] = *N;
            send_beacon();
        // }
    }
    else if(goc_data_header == 0x19) {
        uint8_t option = (goc_data_full >> 22) & 0x3;
    3272:	0209      	lsls	r1, r1, #8
        uint8_t index = (goc_data_full >> 19) & 0x7;
    3274:	0f52      	lsrs	r2, r2, #29
            radio_data_arr[0] = *N;
            send_beacon();
        // }
    }
    else if(goc_data_header == 0x19) {
        uint8_t option = (goc_data_full >> 22) & 0x3;
    3276:	0f89      	lsrs	r1, r1, #30
        uint8_t index = (goc_data_full >> 19) & 0x7;
        uint32_t N = goc_data_full & 0xFF;
    3278:	b2db      	uxtb	r3, r3
    327a:	4d7f      	ldr	r5, [pc, #508]	; (3478 <main+0x1618>)
    327c:	0050      	lsls	r0, r2, #1
    327e:	4c7f      	ldr	r4, [pc, #508]	; (347c <main+0x161c>)

        if(option == 1) {
    3280:	2901      	cmp	r1, #1
    3282:	d102      	bne.n	328a <main+0x142a>
            LOW_PWR_LOW_ADC_THRESH[index] = N;
    3284:	b29b      	uxth	r3, r3
    3286:	522b      	strh	r3, [r5, r0]
    3288:	e003      	b.n	3292 <main+0x1432>
        }
        else if(option == 2) {
    328a:	2902      	cmp	r1, #2
    328c:	d101      	bne.n	3292 <main+0x1432>
            LOW_PWR_HIGH_ADC_THRESH[index] = N;
    328e:	b29b      	uxth	r3, r3
    3290:	5223      	strh	r3, [r4, r0]
        }

        radio_data_arr[2] = (0x19 << 8) | CHIP_ID;
    3292:	4b72      	ldr	r3, [pc, #456]	; (345c <main+0x15fc>)
    3294:	8819      	ldrh	r1, [r3, #0]
    3296:	23c8      	movs	r3, #200	; 0xc8
    3298:	015b      	lsls	r3, r3, #5
    329a:	4319      	orrs	r1, r3
    329c:	4b70      	ldr	r3, [pc, #448]	; (3460 <main+0x1600>)
    329e:	6099      	str	r1, [r3, #8]
        radio_data_arr[1] = index;
    32a0:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = (LOW_PWR_LOW_ADC_THRESH[index] << 8) | LOW_PWR_HIGH_ADC_THRESH[index];
    32a2:	5a29      	ldrh	r1, [r5, r0]
    32a4:	5a22      	ldrh	r2, [r4, r0]
    32a6:	0209      	lsls	r1, r1, #8
    32a8:	430a      	orrs	r2, r1
    32aa:	e0a3      	b.n	33f4 <main+0x1594>
        send_beacon();
    }
    else if(goc_data_header == 0x1A) {
    32ac:	2b1a      	cmp	r3, #26
    32ae:	d13d      	bne.n	332c <main+0x14cc>
    32b0:	4e73      	ldr	r6, [pc, #460]	; (3480 <main+0x1620>)
        if(option) {
    32b2:	2d00      	cmp	r5, #0
    32b4:	d02f      	beq.n	3316 <main+0x14b6>
            mrr_trx_cap_antn_tune_coarse = goc_data_full & 0x3FF;
    32b6:	6823      	ldr	r3, [r4, #0]
            // TX Setup Carrier Freq
            mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = mrr_trx_cap_antn_tune_coarse;  //ANT CAP 10b unary 830.5 MHz // TODO: make them the same variable
    32b8:	4a72      	ldr	r2, [pc, #456]	; (3484 <main+0x1624>)
        radio_data_arr[0] = (LOW_PWR_LOW_ADC_THRESH[index] << 8) | LOW_PWR_HIGH_ADC_THRESH[index];
        send_beacon();
    }
    else if(goc_data_header == 0x1A) {
        if(option) {
            mrr_trx_cap_antn_tune_coarse = goc_data_full & 0x3FF;
    32ba:	059b      	lsls	r3, r3, #22
    32bc:	0d9b      	lsrs	r3, r3, #22
    32be:	8033      	strh	r3, [r6, #0]
            // TX Setup Carrier Freq
            mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = mrr_trx_cap_antn_tune_coarse;  //ANT CAP 10b unary 830.5 MHz // TODO: make them the same variable
    32c0:	6810      	ldr	r0, [r2, #0]
    32c2:	4971      	ldr	r1, [pc, #452]	; (3488 <main+0x1628>)
    32c4:	01db      	lsls	r3, r3, #7
    32c6:	4001      	ands	r1, r0
    32c8:	430b      	orrs	r3, r1
    32ca:	6013      	str	r3, [r2, #0]
            mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    32cc:	6812      	ldr	r2, [r2, #0]
    32ce:	2100      	movs	r1, #0
    32d0:	2002      	movs	r0, #2
    32d2:	f7fc ff28 	bl	126 <mbus_remote_register_write>
            mrrv11a_r01.TRX_CAP_ANTN_TUNE_COARSE = mrr_trx_cap_antn_tune_coarse; //ANT CAP 10b unary 830.5 MHz
    32d6:	4a6d      	ldr	r2, [pc, #436]	; (348c <main+0x162c>)
    32d8:	8833      	ldrh	r3, [r6, #0]
    32da:	6811      	ldr	r1, [r2, #0]
    32dc:	059b      	lsls	r3, r3, #22
    32de:	0a89      	lsrs	r1, r1, #10
    32e0:	0289      	lsls	r1, r1, #10
    32e2:	0d9b      	lsrs	r3, r3, #22
    32e4:	430b      	orrs	r3, r1
    32e6:	6013      	str	r3, [r2, #0]
            mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
    32e8:	233f      	movs	r3, #63	; 0x3f
    32ea:	4869      	ldr	r0, [pc, #420]	; (3490 <main+0x1630>)
    32ec:	4d69      	ldr	r5, [pc, #420]	; (3494 <main+0x1634>)
    32ee:	8801      	ldrh	r1, [r0, #0]
    32f0:	6814      	ldr	r4, [r2, #0]
    32f2:	4019      	ands	r1, r3
    32f4:	0289      	lsls	r1, r1, #10
    32f6:	402c      	ands	r4, r5
    32f8:	4321      	orrs	r1, r4
    32fa:	6011      	str	r1, [r2, #0]
            mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
    32fc:	8801      	ldrh	r1, [r0, #0]
    32fe:	6810      	ldr	r0, [r2, #0]
    3300:	400b      	ands	r3, r1
    3302:	4965      	ldr	r1, [pc, #404]	; (3498 <main+0x1638>)
    3304:	041b      	lsls	r3, r3, #16
    3306:	4001      	ands	r1, r0
    3308:	430b      	orrs	r3, r1
    330a:	6013      	str	r3, [r2, #0]
            mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
    330c:	6812      	ldr	r2, [r2, #0]
    330e:	2101      	movs	r1, #1
    3310:	2002      	movs	r0, #2
    3312:	f7fc ff08 	bl	126 <mbus_remote_register_write>
        }

        radio_data_arr[2] = (0x1A << 8) | CHIP_ID;
    3316:	4b51      	ldr	r3, [pc, #324]	; (345c <main+0x15fc>)
    3318:	881a      	ldrh	r2, [r3, #0]
    331a:	23d0      	movs	r3, #208	; 0xd0
    331c:	015b      	lsls	r3, r3, #5
    331e:	431a      	orrs	r2, r3
    3320:	4b4f      	ldr	r3, [pc, #316]	; (3460 <main+0x1600>)
    3322:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = 0;
    3324:	2200      	movs	r2, #0
    3326:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = mrr_trx_cap_antn_tune_coarse;
    3328:	8832      	ldrh	r2, [r6, #0]
    332a:	e063      	b.n	33f4 <main+0x1594>
        send_beacon();
    }
    else if(goc_data_header == 0x1B) {
    332c:	2b1b      	cmp	r3, #27
    332e:	d122      	bne.n	3376 <main+0x1516>
        uint8_t option = (goc_data_full >> 22) & 0x3;
    3330:	6822      	ldr	r2, [r4, #0]
        uint32_t N = goc_data_full & 0x3FFFF;
    3332:	6823      	ldr	r3, [r4, #0]
        radio_data_arr[1] = 0;
        radio_data_arr[0] = mrr_trx_cap_antn_tune_coarse;
        send_beacon();
    }
    else if(goc_data_header == 0x1B) {
        uint8_t option = (goc_data_full >> 22) & 0x3;
    3334:	0212      	lsls	r2, r2, #8
        uint32_t N = goc_data_full & 0x3FFFF;
    3336:	039b      	lsls	r3, r3, #14
        radio_data_arr[1] = 0;
        radio_data_arr[0] = mrr_trx_cap_antn_tune_coarse;
        send_beacon();
    }
    else if(goc_data_header == 0x1B) {
        uint8_t option = (goc_data_full >> 22) & 0x3;
    3338:	0f92      	lsrs	r2, r2, #30
        uint32_t N = goc_data_full & 0x3FFFF;
    333a:	0b9b      	lsrs	r3, r3, #14
    333c:	4d57      	ldr	r5, [pc, #348]	; (349c <main+0x163c>)
    333e:	4c58      	ldr	r4, [pc, #352]	; (34a0 <main+0x1640>)
    3340:	4858      	ldr	r0, [pc, #352]	; (34a4 <main+0x1644>)

        if(option == 1) {
    3342:	2a01      	cmp	r2, #1
    3344:	d101      	bne.n	334a <main+0x14ea>
            start_day_count = N;
    3346:	602b      	str	r3, [r5, #0]
    3348:	e006      	b.n	3358 <main+0x14f8>
        }
        else if(option == 2) {
    334a:	2a02      	cmp	r2, #2
    334c:	d101      	bne.n	3352 <main+0x14f2>
            end_day_count = N;
    334e:	6023      	str	r3, [r4, #0]
    3350:	e002      	b.n	3358 <main+0x14f8>
        }
        else if(option == 3) {
    3352:	2a03      	cmp	r2, #3
    3354:	d100      	bne.n	3358 <main+0x14f8>
            radio_day_count = N;
    3356:	6003      	str	r3, [r0, #0]
        }

        radio_data_arr[2] = (0x1B << 8) | CHIP_ID;
    3358:	22d8      	movs	r2, #216	; 0xd8
    335a:	4b40      	ldr	r3, [pc, #256]	; (345c <main+0x15fc>)
    335c:	4940      	ldr	r1, [pc, #256]	; (3460 <main+0x1600>)
    335e:	881b      	ldrh	r3, [r3, #0]
    3360:	0152      	lsls	r2, r2, #5
    3362:	4313      	orrs	r3, r2
    3364:	608b      	str	r3, [r1, #8]
        radio_data_arr[1] = start_day_count;
    3366:	682b      	ldr	r3, [r5, #0]
    3368:	604b      	str	r3, [r1, #4]
        radio_data_arr[0] = (end_day_count << 16) | radio_day_count;
    336a:	6822      	ldr	r2, [r4, #0]
    336c:	6803      	ldr	r3, [r0, #0]
    336e:	0412      	lsls	r2, r2, #16
    3370:	4313      	orrs	r3, r2
    3372:	600b      	str	r3, [r1, #0]
    3374:	e06a      	b.n	344c <main+0x15ec>
        send_beacon();
    }
    else if(goc_data_header == 0x1C) {
    3376:	2b1c      	cmp	r3, #28
    3378:	d127      	bne.n	33ca <main+0x156a>
        uint16_t N1 = (goc_data_full >> 12) & 0xFFF;
    337a:	6823      	ldr	r3, [r4, #0]
    337c:	4d4a      	ldr	r5, [pc, #296]	; (34a8 <main+0x1648>)
    337e:	021b      	lsls	r3, r3, #8
    3380:	0d1a      	lsrs	r2, r3, #20
        uint16_t N2 = goc_data_full & 0xFFF;
    3382:	6823      	ldr	r3, [r4, #0]
    3384:	051b      	lsls	r3, r3, #20
    3386:	0d1b      	lsrs	r3, r3, #20
        if(!N2) {
    3388:	d10a      	bne.n	33a0 <main+0x1540>
            radio_data_arr[2] = (0x1C << 8) | CHIP_ID;
    338a:	4b34      	ldr	r3, [pc, #208]	; (345c <main+0x15fc>)
    338c:	881a      	ldrh	r2, [r3, #0]
    338e:	23e0      	movs	r3, #224	; 0xe0
    3390:	015b      	lsls	r3, r3, #5
    3392:	431a      	orrs	r2, r3
    3394:	4b32      	ldr	r3, [pc, #200]	; (3460 <main+0x1600>)
    3396:	609a      	str	r2, [r3, #8]
            radio_data_arr[1] = 0;
    3398:	2200      	movs	r2, #0
    339a:	605a      	str	r2, [r3, #4]
            radio_data_arr[0] = max_unit_count;
    339c:	882a      	ldrh	r2, [r5, #0]
    339e:	e029      	b.n	33f4 <main+0x1594>
            send_beacon();
        }
        else {
            light_packet_num = 0;
    33a0:	2100      	movs	r1, #0
    33a2:	0014      	movs	r4, r2
    33a4:	4841      	ldr	r0, [pc, #260]	; (34ac <main+0x164c>)
    33a6:	3b01      	subs	r3, #1
    33a8:	8001      	strh	r1, [r0, #0]
    33aa:	b29e      	uxth	r6, r3
            temp_packet_num = 0;
    33ac:	4840      	ldr	r0, [pc, #256]	; (34b0 <main+0x1650>)
    33ae:	18b6      	adds	r6, r6, r2
    33b0:	8001      	strh	r1, [r0, #0]
    33b2:	3601      	adds	r6, #1
            uint16_t i;
            for(i = 0; i < N2; i++) {
                if(N1 + i >= max_unit_count) {
    33b4:	882b      	ldrh	r3, [r5, #0]
    33b6:	429c      	cmp	r4, r3
    33b8:	db00      	blt.n	33bc <main+0x155c>
    33ba:	e0ab      	b.n	3514 <main+0x16b4>
                    break;
                }
                radio_unit(N1 + i);
    33bc:	b2a0      	uxth	r0, r4
    33be:	3401      	adds	r4, #1
    33c0:	f7fe fc00 	bl	1bc4 <radio_unit>
        }
        else {
            light_packet_num = 0;
            temp_packet_num = 0;
            uint16_t i;
            for(i = 0; i < N2; i++) {
    33c4:	42a6      	cmp	r6, r4
    33c6:	d1f5      	bne.n	33b4 <main+0x1554>
    33c8:	e0a4      	b.n	3514 <main+0x16b4>
                }
                radio_unit(N1 + i);
            }
        }
    }
    else if(goc_data_header == 0x1D) {
    33ca:	2b1d      	cmp	r3, #29
    33cc:	d114      	bne.n	33f8 <main+0x1598>
        // data collection end day time
        uint16_t N = goc_data_full & 0x7FF;
    33ce:	6823      	ldr	r3, [r4, #0]
    33d0:	4938      	ldr	r1, [pc, #224]	; (34b4 <main+0x1654>)
    33d2:	055b      	lsls	r3, r3, #21
    33d4:	0d5b      	lsrs	r3, r3, #21
        if(option) {
    33d6:	2d00      	cmp	r5, #0
    33d8:	d002      	beq.n	33e0 <main+0x1580>
            data_collection_end_day_time = N * 60;
    33da:	223c      	movs	r2, #60	; 0x3c
    33dc:	4353      	muls	r3, r2
    33de:	600b      	str	r3, [r1, #0]
        }
        radio_data_arr[2] = (0x1D << 8) | CHIP_ID;
    33e0:	4b1e      	ldr	r3, [pc, #120]	; (345c <main+0x15fc>)
    33e2:	881a      	ldrh	r2, [r3, #0]
    33e4:	23e8      	movs	r3, #232	; 0xe8
    33e6:	015b      	lsls	r3, r3, #5
    33e8:	431a      	orrs	r2, r3
    33ea:	4b1d      	ldr	r3, [pc, #116]	; (3460 <main+0x1600>)
    33ec:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = 0;
    33ee:	2200      	movs	r2, #0
    33f0:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = data_collection_end_day_time;
    33f2:	680a      	ldr	r2, [r1, #0]
    33f4:	601a      	str	r2, [r3, #0]
    33f6:	e029      	b.n	344c <main+0x15ec>
        send_beacon();
    }
    else if(goc_data_header == 0x1E) {
    33f8:	2b1e      	cmp	r3, #30
    33fa:	d163      	bne.n	34c4 <main+0x1664>
        // SFO trimming
        uint16_t N = (goc_data_full >> 8) & 0xFF;
    33fc:	4b2e      	ldr	r3, [pc, #184]	; (34b8 <main+0x1658>)
    33fe:	4c2f      	ldr	r4, [pc, #188]	; (34bc <main+0x165c>)
    3400:	681a      	ldr	r2, [r3, #0]
        uint16_t M = goc_data_full & 0xFF;
    3402:	6818      	ldr	r0, [r3, #0]
        radio_data_arr[0] = data_collection_end_day_time;
        send_beacon();
    }
    else if(goc_data_header == 0x1E) {
        // SFO trimming
        uint16_t N = (goc_data_full >> 8) & 0xFF;
    3404:	0a12      	lsrs	r2, r2, #8
        uint16_t M = goc_data_full & 0xFF;
        if(option) {
    3406:	2d00      	cmp	r5, #0
    3408:	d011      	beq.n	342e <main+0x15ce>
            mrrv11a_r07.RO_MOM = N;
    340a:	237f      	movs	r3, #127	; 0x7f
    340c:	8821      	ldrh	r1, [r4, #0]
    340e:	401a      	ands	r2, r3
    3410:	4399      	bics	r1, r3
    3412:	430a      	orrs	r2, r1
    3414:	8022      	strh	r2, [r4, #0]
            mrrv11a_r07.RO_MIM = M;
    3416:	8821      	ldrh	r1, [r4, #0]
    3418:	4a29      	ldr	r2, [pc, #164]	; (34c0 <main+0x1660>)
    341a:	4003      	ands	r3, r0
    341c:	400a      	ands	r2, r1
    341e:	01db      	lsls	r3, r3, #7
    3420:	4313      	orrs	r3, r2
    3422:	8023      	strh	r3, [r4, #0]
            mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);
    3424:	6822      	ldr	r2, [r4, #0]
    3426:	2107      	movs	r1, #7
    3428:	2002      	movs	r0, #2
    342a:	f7fc fe7c 	bl	126 <mbus_remote_register_write>
        }

        radio_data_arr[2] = (0x1E << 8) | CHIP_ID;
    342e:	22f0      	movs	r2, #240	; 0xf0
    3430:	4b0a      	ldr	r3, [pc, #40]	; (345c <main+0x15fc>)
    3432:	0152      	lsls	r2, r2, #5
    3434:	881b      	ldrh	r3, [r3, #0]
    3436:	4313      	orrs	r3, r2
    3438:	4a09      	ldr	r2, [pc, #36]	; (3460 <main+0x1600>)
    343a:	6093      	str	r3, [r2, #8]
        radio_data_arr[1] = mrrv11a_r07.RO_MOM;
    343c:	6823      	ldr	r3, [r4, #0]
    343e:	065b      	lsls	r3, r3, #25
    3440:	0e5b      	lsrs	r3, r3, #25
    3442:	6053      	str	r3, [r2, #4]
        radio_data_arr[0] = mrrv11a_r07.RO_MIM;
    3444:	6823      	ldr	r3, [r4, #0]
    3446:	049b      	lsls	r3, r3, #18
    3448:	0e5b      	lsrs	r3, r3, #25
    344a:	6013      	str	r3, [r2, #0]
        send_beacon();
    344c:	f7fe fc92 	bl	1d74 <send_beacon>
    3450:	e060      	b.n	3514 <main+0x16b4>
    3452:	46c0      	nop			; (mov r8, r8)
    3454:	00003aa0 	.word	0x00003aa0
    3458:	000037fc 	.word	0x000037fc
    345c:	00003abc 	.word	0x00003abc
    3460:	000039e8 	.word	0x000039e8
    3464:	00003958 	.word	0x00003958
    3468:	0000386c 	.word	0x0000386c
    346c:	00003a3e 	.word	0x00003a3e
    3470:	00003b14 	.word	0x00003b14
    3474:	00003a9c 	.word	0x00003a9c
    3478:	0000381c 	.word	0x0000381c
    347c:	000038f2 	.word	0x000038f2
    3480:	00003b18 	.word	0x00003b18
    3484:	00003804 	.word	0x00003804
    3488:	fffe007f 	.word	0xfffe007f
    348c:	00003b10 	.word	0x00003b10
    3490:	00003984 	.word	0x00003984
    3494:	ffff03ff 	.word	0xffff03ff
    3498:	ffc0ffff 	.word	0xffc0ffff
    349c:	00003ab0 	.word	0x00003ab0
    34a0:	00003a5c 	.word	0x00003a5c
    34a4:	00003aa8 	.word	0x00003aa8
    34a8:	00003a42 	.word	0x00003a42
    34ac:	00003aa2 	.word	0x00003aa2
    34b0:	00003a6c 	.word	0x00003a6c
    34b4:	00003870 	.word	0x00003870
    34b8:	00003a28 	.word	0x00003a28
    34bc:	00003814 	.word	0x00003814
    34c0:	ffffc07f 	.word	0xffffc07f
    }
    else if(goc_data_header == 0x1F) {
    34c4:	2b1f      	cmp	r3, #31
    34c6:	d125      	bne.n	3514 <main+0x16b4>

        // enable XOT interrupt
        *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
              1 << IRQ_REG0 | 1 << IRQ_REG1 | 1 << IRQ_REG2 | 1 << IRQ_REG3 | 1 << IRQ_XOT);

        *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    34c8:	2500      	movs	r5, #0
        send_beacon();
    }
    else if(goc_data_header == 0x1F) {
        // XO characterization

        uint32_t N = (goc_data_full & 0xFFFF) << XO_TO_SEC_SHIFT;
    34ca:	4b60      	ldr	r3, [pc, #384]	; (364c <main+0x17ec>)

        // enable XOT interrupt
        *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
    34cc:	4a60      	ldr	r2, [pc, #384]	; (3650 <main+0x17f0>)
        send_beacon();
    }
    else if(goc_data_header == 0x1F) {
        // XO characterization

        uint32_t N = (goc_data_full & 0xFFFF) << XO_TO_SEC_SHIFT;
    34ce:	6819      	ldr	r1, [r3, #0]

        // enable XOT interrupt
        *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
    34d0:	4b60      	ldr	r3, [pc, #384]	; (3654 <main+0x17f4>)
        send_beacon();
    }
    else if(goc_data_header == 0x1F) {
        // XO characterization

        uint32_t N = (goc_data_full & 0xFFFF) << XO_TO_SEC_SHIFT;
    34d2:	b289      	uxth	r1, r1

        // enable XOT interrupt
        *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
    34d4:	601a      	str	r2, [r3, #0]
              1 << IRQ_REG0 | 1 << IRQ_REG1 | 1 << IRQ_REG2 | 1 << IRQ_REG3 | 1 << IRQ_XOT);

        *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    34d6:	4b60      	ldr	r3, [pc, #384]	; (3658 <main+0x17f8>)
        send_beacon();
    }
    else if(goc_data_header == 0x1F) {
        // XO characterization

        uint32_t N = (goc_data_full & 0xFFFF) << XO_TO_SEC_SHIFT;
    34d8:	03cc      	lsls	r4, r1, #15

        // enable XOT interrupt
        *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
              1 << IRQ_REG0 | 1 << IRQ_REG1 | 1 << IRQ_REG2 | 1 << IRQ_REG3 | 1 << IRQ_XOT);

        *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    34da:	601d      	str	r5, [r3, #0]
        *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    34dc:	4b5f      	ldr	r3, [pc, #380]	; (365c <main+0x17fc>)

        // delay an arbitrary amount
        delay(10000);
    34de:	4860      	ldr	r0, [pc, #384]	; (3660 <main+0x1800>)
        // enable XOT interrupt
        *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
              1 << IRQ_REG0 | 1 << IRQ_REG1 | 1 << IRQ_REG2 | 1 << IRQ_REG3 | 1 << IRQ_XOT);

        *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
        *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    34e0:	601d      	str	r5, [r3, #0]

        // delay an arbitrary amount
        delay(10000);
    34e2:	f7fc fe5d 	bl	1a0 <delay>

        // set timer
        stop_xo_cnt();
    34e6:	f7fc feb3 	bl	250 <stop_xo_cnt>
        reset_xo_cnt();
    34ea:	f7fc fea5 	bl	238 <reset_xo_cnt>
        set_xo_timer(0, N, 0, 1);
    34ee:	2301      	movs	r3, #1
    34f0:	002a      	movs	r2, r5
    34f2:	0021      	movs	r1, r4
    34f4:	0028      	movs	r0, r5
    34f6:	f7fc fe81 	bl	1fc <set_xo_timer>

        // immediately go into low power mode
        asm("wfi;");
    34fa:	bf30      	wfi



        // some current event
        pmu_current_marker();
    34fc:	f7fc ff78 	bl	3f0 <pmu_current_marker>

        uint32_t diff = 0xFFFFFFFF;
        uint32_t xot_val = *XOT_VAL;
    3500:	4b58      	ldr	r3, [pc, #352]	; (3664 <main+0x1804>)
        uint32_t target = xot_val + N;

        while(diff >= (1 << 13)) {
    3502:	4859      	ldr	r0, [pc, #356]	; (3668 <main+0x1808>)

        // some current event
        pmu_current_marker();

        uint32_t diff = 0xFFFFFFFF;
        uint32_t xot_val = *XOT_VAL;
    3504:	6819      	ldr	r1, [r3, #0]
        uint32_t target = xot_val + N;
    3506:	1861      	adds	r1, r4, r1

        while(diff >= (1 << 13)) {
            xot_val = *XOT_VAL;
    3508:	681a      	ldr	r2, [r3, #0]

        uint32_t diff = 0xFFFFFFFF;
        uint32_t xot_val = *XOT_VAL;
        uint32_t target = xot_val + N;

        while(diff >= (1 << 13)) {
    350a:	1a52      	subs	r2, r2, r1
    350c:	4282      	cmp	r2, r0
    350e:	d8fb      	bhi.n	3508 <main+0x16a8>
            xot_val = *XOT_VAL;
            diff = xot_val - target;    // this will be a large positive number if xot_val is less then target
        }

        // some current event
        pmu_current_marker();
    3510:	f7fc ff6e 	bl	3f0 <pmu_current_marker>
    }

    update_system_time();
    3514:	f7fe f848 	bl	15a8 <update_system_time>

    if(projected_end_time_in_sec > xo_sys_time_in_sec) {
    3518:	4b54      	ldr	r3, [pc, #336]	; (366c <main+0x180c>)
    351a:	683a      	ldr	r2, [r7, #0]
    351c:	681b      	ldr	r3, [r3, #0]
    351e:	429a      	cmp	r2, r3
    3520:	d800      	bhi.n	3524 <main+0x16c4>
    3522:	e086      	b.n	3632 <main+0x17d2>
    3524:	2600      	movs	r6, #0
        uint64_t temp = mult(projected_end_time - xo_sys_time, xo_lnt_mplier);

        lnt_meas_time = right_shift(temp, LNT_MPLIER_SHIFT + XO_TO_SEC_SHIFT - 2); // the -2 is empirical
        // uint32_t val = (end_time - xo_sys_time_in_sec) * 4;
        lntv1a_r03.TIME_COUNTING = lnt_meas_time;
        mbus_remote_register_write(LNT_ADDR, 0x03, lntv1a_r03.as_int);
    3526:	2503      	movs	r5, #3
    // LNT FSM stuck fix
    int count = 0;
    int lnt_state = 0xaabb;
    while(count < 5) {
        count++;
        uint32_t projected_end_time = projected_end_time_in_sec << XO_TO_SEC_SHIFT;
    3528:	683b      	ldr	r3, [r7, #0]

    // LNT FSM stuck fix
    int count = 0;
    int lnt_state = 0xaabb;
    while(count < 5) {
        count++;
    352a:	3601      	adds	r6, #1
        uint32_t projected_end_time = projected_end_time_in_sec << XO_TO_SEC_SHIFT;
    352c:	03dc      	lsls	r4, r3, #15

        update_system_time();
    352e:	f7fe f83b 	bl	15a8 <update_system_time>
        uint64_t temp = mult(projected_end_time - xo_sys_time, xo_lnt_mplier);
    3532:	4b4f      	ldr	r3, [pc, #316]	; (3670 <main+0x1810>)
    3534:	6818      	ldr	r0, [r3, #0]
    3536:	4b4f      	ldr	r3, [pc, #316]	; (3674 <main+0x1814>)
    3538:	1a20      	subs	r0, r4, r0
    353a:	8819      	ldrh	r1, [r3, #0]
    353c:	f7fd f95c 	bl	7f8 <mult>

        lnt_meas_time = right_shift(temp, LNT_MPLIER_SHIFT + XO_TO_SEC_SHIFT - 2); // the -2 is empirical
    3540:	4b4d      	ldr	r3, [pc, #308]	; (3678 <main+0x1818>)
    3542:	881a      	ldrh	r2, [r3, #0]
    3544:	320d      	adds	r2, #13
    3546:	b252      	sxtb	r2, r2
    3548:	f7fd f887 	bl	65a <right_shift>
    354c:	4b4b      	ldr	r3, [pc, #300]	; (367c <main+0x181c>)
        // uint32_t val = (end_time - xo_sys_time_in_sec) * 4;
        lntv1a_r03.TIME_COUNTING = lnt_meas_time;
        mbus_remote_register_write(LNT_ADDR, 0x03, lntv1a_r03.as_int);
    354e:	0029      	movs	r1, r5
        uint32_t projected_end_time = projected_end_time_in_sec << XO_TO_SEC_SHIFT;

        update_system_time();
        uint64_t temp = mult(projected_end_time - xo_sys_time, xo_lnt_mplier);

        lnt_meas_time = right_shift(temp, LNT_MPLIER_SHIFT + XO_TO_SEC_SHIFT - 2); // the -2 is empirical
    3550:	6018      	str	r0, [r3, #0]
        // uint32_t val = (end_time - xo_sys_time_in_sec) * 4;
        lntv1a_r03.TIME_COUNTING = lnt_meas_time;
    3552:	4b4b      	ldr	r3, [pc, #300]	; (3680 <main+0x1820>)
    3554:	0200      	lsls	r0, r0, #8
    3556:	681a      	ldr	r2, [r3, #0]
    3558:	0a00      	lsrs	r0, r0, #8
    355a:	0e12      	lsrs	r2, r2, #24
    355c:	0612      	lsls	r2, r2, #24
    355e:	4310      	orrs	r0, r2
    3560:	6018      	str	r0, [r3, #0]
        mbus_remote_register_write(LNT_ADDR, 0x03, lntv1a_r03.as_int);
    3562:	681a      	ldr	r2, [r3, #0]
    3564:	0028      	movs	r0, r5
    3566:	f7fc fdde 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
}

static void lnt_start() {
    // Release Reset 
    lntv1a_r00.RESET_AFE = 0x0; // Defhttps://www.dropbox.com/s/yh15ux4h8141vu4/ISSCC2019-Digest.pdf?dl=0ault : 0x1
    356a:	2208      	movs	r2, #8
    356c:	4c45      	ldr	r4, [pc, #276]	; (3684 <main+0x1824>)
    lntv1a_r00.RESETN_DBE = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    356e:	2100      	movs	r1, #0
    delay(MBUS_DELAY*10);
}

static void lnt_start() {
    // Release Reset 
    lntv1a_r00.RESET_AFE = 0x0; // Defhttps://www.dropbox.com/s/yh15ux4h8141vu4/ISSCC2019-Digest.pdf?dl=0ault : 0x1
    3570:	7823      	ldrb	r3, [r4, #0]
    lntv1a_r00.RESETN_DBE = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    3572:	0028      	movs	r0, r5
    delay(MBUS_DELAY*10);
}

static void lnt_start() {
    // Release Reset 
    lntv1a_r00.RESET_AFE = 0x0; // Defhttps://www.dropbox.com/s/yh15ux4h8141vu4/ISSCC2019-Digest.pdf?dl=0ault : 0x1
    3574:	4393      	bics	r3, r2
    3576:	7023      	strb	r3, [r4, #0]
    lntv1a_r00.RESETN_DBE = 0x1; // Default : 0x0
    3578:	7823      	ldrb	r3, [r4, #0]
    357a:	3a04      	subs	r2, #4
    357c:	4313      	orrs	r3, r2
    357e:	7023      	strb	r3, [r4, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    3580:	6822      	ldr	r2, [r4, #0]
    3582:	f7fc fdd0 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    3586:	20fa      	movs	r0, #250	; 0xfa
    3588:	0080      	lsls	r0, r0, #2
    358a:	f7fc fe09 	bl	1a0 <delay>
    
    // LNT Start
    lntv1a_r00.DBE_ENABLE = 0x1; // Default : 0x0
    358e:	2210      	movs	r2, #16
    3590:	7823      	ldrb	r3, [r4, #0]
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x0; // Default : 0x0
    lntv1a_r00.MODE_CONTINUOUS = 0x0; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    3592:	2100      	movs	r1, #0
    lntv1a_r00.RESETN_DBE = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*10);
    
    // LNT Start
    lntv1a_r00.DBE_ENABLE = 0x1; // Default : 0x0
    3594:	4313      	orrs	r3, r2
    3596:	7023      	strb	r3, [r4, #0]
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x0; // Default : 0x0
    3598:	7823      	ldrb	r3, [r4, #0]
    359a:	3230      	adds	r2, #48	; 0x30
    359c:	4393      	bics	r3, r2
    359e:	7023      	strb	r3, [r4, #0]
    lntv1a_r00.MODE_CONTINUOUS = 0x0; // Default : 0x0
    35a0:	7823      	ldrb	r3, [r4, #0]
    35a2:	3a20      	subs	r2, #32
    35a4:	4393      	bics	r3, r2
    35a6:	7023      	strb	r3, [r4, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    35a8:	6822      	ldr	r2, [r4, #0]
    35aa:	0028      	movs	r0, r5
    35ac:	f7fc fdbb 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*10);
    35b0:	20fa      	movs	r0, #250	; 0xfa
    35b2:	0080      	lsls	r0, r0, #2
    35b4:	f7fc fdf4 	bl	1a0 <delay>

    lntv1a_r00.WAKEUP_WHEN_DONE = 0x1; // Default : 0x0
    35b8:	2240      	movs	r2, #64	; 0x40
    35ba:	7823      	ldrb	r3, [r4, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    35bc:	2100      	movs	r1, #0
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x0; // Default : 0x0
    lntv1a_r00.MODE_CONTINUOUS = 0x0; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*10);

    lntv1a_r00.WAKEUP_WHEN_DONE = 0x1; // Default : 0x0
    35be:	4313      	orrs	r3, r2
    35c0:	7023      	strb	r3, [r4, #0]
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    35c2:	6822      	ldr	r2, [r4, #0]
    35c4:	0028      	movs	r0, r5
    35c6:	f7fc fdae 	bl	126 <mbus_remote_register_write>
    delay(MBUS_DELAY*100);
    35ca:	4825      	ldr	r0, [pc, #148]	; (3660 <main+0x1800>)
    35cc:	f7fc fde8 	bl	1a0 <delay>
        // uint32_t val = (end_time - xo_sys_time_in_sec) * 4;
        lntv1a_r03.TIME_COUNTING = lnt_meas_time;
        mbus_remote_register_write(LNT_ADDR, 0x03, lntv1a_r03.as_int);
        lnt_start();

        set_halt_until_mbus_trx();
    35d0:	f7fc fe4a 	bl	268 <set_halt_until_mbus_trx>
        mbus_copy_registers_from_remote_to_local(LNT_ADDR, 0x16, 0, 0);
    35d4:	2300      	movs	r3, #0
    35d6:	2116      	movs	r1, #22
    35d8:	001a      	movs	r2, r3
    35da:	0028      	movs	r0, r5
    35dc:	f7fc fd92 	bl	104 <mbus_copy_registers_from_remote_to_local>
        set_halt_until_mbus_tx();
    35e0:	f7fc fe3c 	bl	25c <set_halt_until_mbus_tx>

        lnt_state = *REG0;
    35e4:	23a0      	movs	r3, #160	; 0xa0
    35e6:	061b      	lsls	r3, r3, #24
    35e8:	681c      	ldr	r4, [r3, #0]
        if(lnt_state == 0x1) {      // if LNT_COUNTER_STATE == STATE_COUNTING
    35ea:	2c01      	cmp	r4, #1
    35ec:	d004      	beq.n	35f8 <main+0x1798>
            break;
        }

        lnt_stop();
    35ee:	f7fd f84b 	bl	688 <lnt_stop>
    // mbus_write_message32(0xCE, projected_end_time_in_sec);

    // LNT FSM stuck fix
    int count = 0;
    int lnt_state = 0xaabb;
    while(count < 5) {
    35f2:	2e05      	cmp	r6, #5
    35f4:	d198      	bne.n	3528 <main+0x16c8>
    35f6:	e001      	b.n	35fc <main+0x179c>
        }

        lnt_stop();

    }
    if(count == 5) {
    35f8:	2e05      	cmp	r6, #5
    35fa:	d102      	bne.n	3602 <main+0x17a2>
        set_system_error(0x5);
    35fc:	2005      	movs	r0, #5
    35fe:	f7fd ffc1 	bl	1584 <set_system_error>
    }

    if(error_code == 5) {
    3602:	4921      	ldr	r1, [pc, #132]	; (3688 <main+0x1828>)
    3604:	680b      	ldr	r3, [r1, #0]
    3606:	2b05      	cmp	r3, #5
    3608:	d116      	bne.n	3638 <main+0x17d8>
    
        // Not going to wake up again, send beacons to alert
        radio_data_arr[2] = 0xFF00 | CHIP_ID;
    360a:	22ff      	movs	r2, #255	; 0xff
    360c:	4b1f      	ldr	r3, [pc, #124]	; (368c <main+0x182c>)
    360e:	0212      	lsls	r2, r2, #8
    3610:	881b      	ldrh	r3, [r3, #0]
    3612:	431a      	orrs	r2, r3
    3614:	4b1e      	ldr	r3, [pc, #120]	; (3690 <main+0x1830>)
    3616:	609a      	str	r2, [r3, #8]
        radio_data_arr[1] = error_code;
    3618:	680a      	ldr	r2, [r1, #0]
    361a:	605a      	str	r2, [r3, #4]
        radio_data_arr[0] = lnt_state;
    361c:	601c      	str	r4, [r3, #0]
    361e:	240a      	movs	r4, #10
        int i;
        for(i = 0; i < 10; i++) {
            delay(10000);
    3620:	480f      	ldr	r0, [pc, #60]	; (3660 <main+0x1800>)
    3622:	3c01      	subs	r4, #1
    3624:	f7fc fdbc 	bl	1a0 <delay>
            send_beacon();
    3628:	f7fe fba4 	bl	1d74 <send_beacon>
        // Not going to wake up again, send beacons to alert
        radio_data_arr[2] = 0xFF00 | CHIP_ID;
        radio_data_arr[1] = error_code;
        radio_data_arr[0] = lnt_state;
        int i;
        for(i = 0; i < 10; i++) {
    362c:	2c00      	cmp	r4, #0
    362e:	d1f7      	bne.n	3620 <main+0x17c0>
    3630:	e002      	b.n	3638 <main+0x17d8>

    if(projected_end_time_in_sec > xo_sys_time_in_sec) {
        set_lnt_timer();
    }
    else {
        set_system_error(0x01);
    3632:	2001      	movs	r0, #1
    3634:	f7fd ffa6 	bl	1584 <set_system_error>
    }
    


    last_xo_sys_time = xo_sys_time;
    3638:	4b0d      	ldr	r3, [pc, #52]	; (3670 <main+0x1810>)

    pmu_setting_temp_based(2);
    363a:	2002      	movs	r0, #2
        set_system_error(0x01);
    }
    


    last_xo_sys_time = xo_sys_time;
    363c:	681a      	ldr	r2, [r3, #0]
    363e:	4b15      	ldr	r3, [pc, #84]	; (3694 <main+0x1834>)
    3640:	601a      	str	r2, [r3, #0]

    pmu_setting_temp_based(2);
    3642:	f7fd ff67 	bl	1514 <pmu_setting_temp_based>
    3646:	f7fe ff4a 	bl	24de <main+0x67e>
    364a:	46c0      	nop			; (mov r8, r8)
    364c:	00003a28 	.word	0x00003a28
    3650:	00100f0d 	.word	0x00100f0d
    3654:	e000e100 	.word	0xe000e100
    3658:	a0001200 	.word	0xa0001200
    365c:	a000007c 	.word	0xa000007c
    3660:	00002710 	.word	0x00002710
    3664:	a0001420 	.word	0xa0001420
    3668:	00001fff 	.word	0x00001fff
    366c:	00003acc 	.word	0x00003acc
    3670:	00003aec 	.word	0x00003aec
    3674:	000038f0 	.word	0x000038f0
    3678:	0000382e 	.word	0x0000382e
    367c:	00003a34 	.word	0x00003a34
    3680:	00003894 	.word	0x00003894
    3684:	000038ac 	.word	0x000038ac
    3688:	00003b14 	.word	0x00003b14
    368c:	00003abc 	.word	0x00003abc
    3690:	000039e8 	.word	0x000039e8
    3694:	00003afc 	.word	0x00003afc
