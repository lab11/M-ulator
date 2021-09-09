
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
  40:	00000a95 	muleq	r0, r5, sl
  44:	00000000 	andeq	r0, r0, r0
  48:	00000a65 	andeq	r0, r0, r5, ror #20
  4c:	00000a7d 	andeq	r0, r0, sp, ror sl
	...
  60:	000009f5 	strdeq	r0, [r0], -r5
  64:	00000a11 	andeq	r0, r0, r1, lsl sl
  68:	00000a2d 	andeq	r0, r0, sp, lsr #20
  6c:	00000a49 	andeq	r0, r0, r9, asr #20
	...

000000a0 <hang>:
  a0:	e7fe      	b.n	a0 <hang>

000000a2 <_start>:
  a2:	f001 f81d 	bl	10e0 <main>
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

Disassembly of section .text.config_timer32:

000000b8 <config_timer32>:
	*TIMER16_CNT   = cnt;
	*TIMER16_STAT  = status;
	*TIMER16_GO    = 0x1;
}

void config_timer32(uint32_t cmp, uint8_t roi, uint32_t cnt, uint32_t status){
  b8:	b530      	push	{r4, r5, lr}
	*TIMER32_GO   = 0x0;
  ba:	2500      	movs	r5, #0
  bc:	4c06      	ldr	r4, [pc, #24]	; (d8 <config_timer32+0x20>)
  be:	6025      	str	r5, [r4, #0]
	*TIMER32_CMP  = cmp;
  c0:	4d06      	ldr	r5, [pc, #24]	; (dc <config_timer32+0x24>)
  c2:	6028      	str	r0, [r5, #0]
	*TIMER32_ROI  = roi;
  c4:	4806      	ldr	r0, [pc, #24]	; (e0 <config_timer32+0x28>)
  c6:	6001      	str	r1, [r0, #0]
	*TIMER32_CNT  = cnt;
  c8:	4906      	ldr	r1, [pc, #24]	; (e4 <config_timer32+0x2c>)
  ca:	600a      	str	r2, [r1, #0]
	*TIMER32_STAT = status;
  cc:	4a06      	ldr	r2, [pc, #24]	; (e8 <config_timer32+0x30>)
  ce:	6013      	str	r3, [r2, #0]
	*TIMER32_GO   = 0x1;
  d0:	2301      	movs	r3, #1
  d2:	6023      	str	r3, [r4, #0]
}
  d4:	bd30      	pop	{r4, r5, pc}
  d6:	46c0      	nop			; (mov r8, r8)
  d8:	a0001100 	.word	0xa0001100
  dc:	a0001104 	.word	0xa0001104
  e0:	a0001108 	.word	0xa0001108
  e4:	a000110c 	.word	0xa000110c
  e8:	a0001110 	.word	0xa0001110

Disassembly of section .text.config_timerwd:

000000ec <config_timerwd>:

void config_timerwd(uint32_t cnt){
	*TIMERWD_GO  = 0x0;
  ec:	2200      	movs	r2, #0
  ee:	4b03      	ldr	r3, [pc, #12]	; (fc <config_timerwd+0x10>)
  f0:	601a      	str	r2, [r3, #0]
	*TIMERWD_CNT = cnt;
  f2:	4a03      	ldr	r2, [pc, #12]	; (100 <config_timerwd+0x14>)
  f4:	6010      	str	r0, [r2, #0]
	*TIMERWD_GO  = 0x1;
  f6:	2201      	movs	r2, #1
  f8:	601a      	str	r2, [r3, #0]
}
  fa:	4770      	bx	lr
  fc:	a0001200 	.word	0xa0001200
 100:	a0001204 	.word	0xa0001204

Disassembly of section .text.disable_timerwd:

00000104 <disable_timerwd>:

void disable_timerwd(){
	*TIMERWD_GO  = 0x0;
 104:	2200      	movs	r2, #0
 106:	4b01      	ldr	r3, [pc, #4]	; (10c <disable_timerwd+0x8>)
 108:	601a      	str	r2, [r3, #0]
}
 10a:	4770      	bx	lr
 10c:	a0001200 	.word	0xa0001200

Disassembly of section .text.set_wakeup_timer:

00000110 <set_wakeup_timer>:

void set_wakeup_timer( uint32_t timestamp, uint8_t irq_en, uint8_t reset ){
 110:	0003      	movs	r3, r0
    uint32_t regval = timestamp | 0x800000; // WUP Timer Enable
    if( irq_en ) regval |= 0xC00000;
 112:	20c0      	movs	r0, #192	; 0xc0
 114:	2900      	cmp	r1, #0
 116:	d102      	bne.n	11e <set_wakeup_timer+0xe>
    else         regval &= 0xBFFFFF;
 118:	4806      	ldr	r0, [pc, #24]	; (134 <set_wakeup_timer+0x24>)
 11a:	4003      	ands	r3, r0
 11c:	2080      	movs	r0, #128	; 0x80
 11e:	0400      	lsls	r0, r0, #16
 120:	4318      	orrs	r0, r3
    *REG_WUPT_CONFIG = regval;
 122:	4b05      	ldr	r3, [pc, #20]	; (138 <set_wakeup_timer+0x28>)
 124:	6018      	str	r0, [r3, #0]

	if( reset ) *WUPT_RESET = 0x01;
 126:	2a00      	cmp	r2, #0
 128:	d002      	beq.n	130 <set_wakeup_timer+0x20>
 12a:	2201      	movs	r2, #1
 12c:	4b03      	ldr	r3, [pc, #12]	; (13c <set_wakeup_timer+0x2c>)
 12e:	601a      	str	r2, [r3, #0]
}
 130:	4770      	bx	lr
 132:	46c0      	nop			; (mov r8, r8)
 134:	00bfffff 	.word	0x00bfffff
 138:	a0000044 	.word	0xa0000044
 13c:	a0001300 	.word	0xa0001300

Disassembly of section .text.set_halt_until_mbus_rx:

00000140 <set_halt_until_mbus_rx>:
//**************************************************
// MBUS IRQ SETTING
//**************************************************
void set_halt_until_reg(uint32_t reg_id) { *SREG_CONF_HALT = reg_id; }
void set_halt_until_mem_wr(void) { *SREG_CONF_HALT = HALT_UNTIL_MEM_WR; }
void set_halt_until_mbus_rx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_RX; }
 140:	220a      	movs	r2, #10
 142:	4b01      	ldr	r3, [pc, #4]	; (148 <set_halt_until_mbus_rx+0x8>)
 144:	601a      	str	r2, [r3, #0]
 146:	4770      	bx	lr
 148:	a000a000 	.word	0xa000a000

Disassembly of section .text.set_halt_until_mbus_tx:

0000014c <set_halt_until_mbus_tx>:
void set_halt_until_mbus_tx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_TX; }
 14c:	2209      	movs	r2, #9
 14e:	4b01      	ldr	r3, [pc, #4]	; (154 <set_halt_until_mbus_tx+0x8>)
 150:	601a      	str	r2, [r3, #0]
 152:	4770      	bx	lr
 154:	a000a000 	.word	0xa000a000

Disassembly of section .text.set_halt_until_mbus_trx:

00000158 <set_halt_until_mbus_trx>:
void set_halt_until_mbus_trx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_TRX; }
 158:	220c      	movs	r2, #12
 15a:	4b01      	ldr	r3, [pc, #4]	; (160 <set_halt_until_mbus_trx+0x8>)
 15c:	601a      	str	r2, [r3, #0]
 15e:	4770      	bx	lr
 160:	a000a000 	.word	0xa000a000

Disassembly of section .text.mbus_write_message32:

00000164 <mbus_write_message32>:
	// TODO: Read from LC
	return 1;
}

uint32_t mbus_write_message32(uint8_t addr, uint32_t data) {
    uint32_t mbus_addr = 0xA0003000 | (addr << 4);
 164:	0103      	lsls	r3, r0, #4
 166:	4802      	ldr	r0, [pc, #8]	; (170 <mbus_write_message32+0xc>)
 168:	4318      	orrs	r0, r3
    *((volatile uint32_t *) mbus_addr) = data;
 16a:	6001      	str	r1, [r0, #0]
    return 1;
}
 16c:	2001      	movs	r0, #1
 16e:	4770      	bx	lr
 170:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_write_message:

00000174 <mbus_write_message>:

uint32_t mbus_write_message(uint8_t addr, uint32_t data[], unsigned len) {
 174:	0003      	movs	r3, r0
 176:	1e10      	subs	r0, r2, #0
	// Goal: Use the "Memory Stream Write" to put unconstrained 32-bit data
	//       onto the bus.
	if (len == 0) return 0;
 178:	d00a      	beq.n	190 <mbus_write_message+0x1c>

	*MBUS_CMD0 = (addr << 24) | (len-1);
 17a:	061b      	lsls	r3, r3, #24
 17c:	3801      	subs	r0, #1
 17e:	4318      	orrs	r0, r3
 180:	4b04      	ldr	r3, [pc, #16]	; (194 <mbus_write_message+0x20>)
	*MBUS_CMD1 = (uint32_t) data;
	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x2 << 4);
 182:	2223      	movs	r2, #35	; 0x23
	*MBUS_CMD0 = (addr << 24) | (len-1);
 184:	6018      	str	r0, [r3, #0]
	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x2 << 4);
 186:	2001      	movs	r0, #1
	*MBUS_CMD1 = (uint32_t) data;
 188:	4b03      	ldr	r3, [pc, #12]	; (198 <mbus_write_message+0x24>)
 18a:	6019      	str	r1, [r3, #0]
	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x2 << 4);
 18c:	4b03      	ldr	r3, [pc, #12]	; (19c <mbus_write_message+0x28>)
 18e:	601a      	str	r2, [r3, #0]

    return 1;
}
 190:	4770      	bx	lr
 192:	46c0      	nop			; (mov r8, r8)
 194:	a0002000 	.word	0xa0002000
 198:	a0002004 	.word	0xa0002004
 19c:	a000200c 	.word	0xa000200c

Disassembly of section .text.mbus_enumerate:

000001a0 <mbus_enumerate>:

void mbus_enumerate(unsigned new_prefix) {
    mbus_write_message32(MBUS_DISC_AND_ENUM, ((MBUS_ENUMERATE_CMD << 28) | (new_prefix << 24)));
 1a0:	0603      	lsls	r3, r0, #24
 1a2:	2080      	movs	r0, #128	; 0x80
 1a4:	0580      	lsls	r0, r0, #22
 1a6:	4318      	orrs	r0, r3
    *((volatile uint32_t *) mbus_addr) = data;
 1a8:	4b01      	ldr	r3, [pc, #4]	; (1b0 <mbus_enumerate+0x10>)
 1aa:	6018      	str	r0, [r3, #0]
}
 1ac:	4770      	bx	lr
 1ae:	46c0      	nop			; (mov r8, r8)
 1b0:	a0003000 	.word	0xa0003000

Disassembly of section .text.mbus_sleep_all:

000001b4 <mbus_sleep_all>:
    *((volatile uint32_t *) mbus_addr) = data;
 1b4:	2200      	movs	r2, #0
 1b6:	4b01      	ldr	r3, [pc, #4]	; (1bc <mbus_sleep_all+0x8>)
 1b8:	601a      	str	r2, [r3, #0]

void mbus_sleep_all(void) {
    mbus_write_message32(MBUS_POWER, MBUS_ALL_SLEEP << 28);
}
 1ba:	4770      	bx	lr
 1bc:	a0003010 	.word	0xa0003010

Disassembly of section .text.mbus_copy_registers_from_remote_to_local:

000001c0 <mbus_copy_registers_from_remote_to_local>:
void mbus_copy_registers_from_remote_to_local(
		uint8_t remote_prefix,
		uint8_t remote_reg_start,
		uint8_t local_reg_start,
		uint8_t length_minus_one
		) {
 1c0:	b507      	push	{r0, r1, r2, lr}
	// Put a register read command on the bus instructed to write this node

	uint8_t address = ((remote_prefix & 0xf) << 4) | MPQ_REG_READ;
	uint32_t data = 
        (remote_reg_start << 24) |
 1c2:	0609      	lsls	r1, r1, #24
		(length_minus_one << 16) |
		(mbus_get_short_prefix() << 12) |
		(MPQ_REG_WRITE << 8) | // Write regs *to* _this_ node
 1c4:	4311      	orrs	r1, r2
		(length_minus_one << 16) |
 1c6:	041b      	lsls	r3, r3, #16
		(MPQ_REG_WRITE << 8) | // Write regs *to* _this_ node
 1c8:	4319      	orrs	r1, r3
 1ca:	2380      	movs	r3, #128	; 0x80
	uint8_t address = ((remote_prefix & 0xf) << 4) | MPQ_REG_READ;
 1cc:	2201      	movs	r2, #1
		(MPQ_REG_WRITE << 8) | // Write regs *to* _this_ node
 1ce:	015b      	lsls	r3, r3, #5
	uint8_t address = ((remote_prefix & 0xf) << 4) | MPQ_REG_READ;
 1d0:	0100      	lsls	r0, r0, #4
		(MPQ_REG_WRITE << 8) | // Write regs *to* _this_ node
 1d2:	4319      	orrs	r1, r3
	uint8_t address = ((remote_prefix & 0xf) << 4) | MPQ_REG_READ;
 1d4:	4310      	orrs	r0, r2
	uint32_t data = 
 1d6:	9101      	str	r1, [sp, #4]
		(local_reg_start << 0);

	mbus_write_message(address, &data, 1);
 1d8:	b2c0      	uxtb	r0, r0
 1da:	a901      	add	r1, sp, #4
 1dc:	f7ff ffca 	bl	174 <mbus_write_message>
}
 1e0:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_remote_register_write:

000001e2 <mbus_remote_register_write>:

void mbus_remote_register_write(
		uint8_t prefix,
		uint8_t dst_reg_addr,
		uint32_t dst_reg_val
		) {
 1e2:	b507      	push	{r0, r1, r2, lr}
	// assert (prefix < 16 && > 0);
	uint8_t address = ((prefix & 0xf) << 4) | MPQ_REG_WRITE;
	uint32_t data = (dst_reg_addr << 24) | (dst_reg_val & 0xffffff);
 1e4:	0212      	lsls	r2, r2, #8
 1e6:	0a12      	lsrs	r2, r2, #8
 1e8:	0609      	lsls	r1, r1, #24
 1ea:	4311      	orrs	r1, r2
	uint8_t address = ((prefix & 0xf) << 4) | MPQ_REG_WRITE;
 1ec:	0100      	lsls	r0, r0, #4
	uint32_t data = (dst_reg_addr << 24) | (dst_reg_val & 0xffffff);
 1ee:	9101      	str	r1, [sp, #4]
	mbus_write_message(address, &data, 1);
 1f0:	2201      	movs	r2, #1
 1f2:	b2c0      	uxtb	r0, r0
 1f4:	a901      	add	r1, sp, #4
 1f6:	f7ff ffbd 	bl	174 <mbus_write_message>
}
 1fa:	bd07      	pop	{r0, r1, r2, pc}

Disassembly of section .text.mbus_remote_register_read:

000001fc <mbus_remote_register_read>:

void mbus_remote_register_read(
		uint8_t remote_prefix,
		uint8_t remote_reg_addr,
		uint8_t local_reg_addr
		) {
 1fc:	b510      	push	{r4, lr}
	mbus_copy_registers_from_remote_to_local(
 1fe:	2300      	movs	r3, #0
 200:	f7ff ffde 	bl	1c0 <mbus_copy_registers_from_remote_to_local>
			remote_prefix, remote_reg_addr, local_reg_addr, 0);
}
 204:	bd10      	pop	{r4, pc}

Disassembly of section .text.mbus_copy_mem_from_local_to_remote_bulk:

00000208 <mbus_copy_mem_from_local_to_remote_bulk>:
		uint8_t   remote_prefix,
		uint32_t* remote_memory_address,
		uint32_t* local_address,
		uint32_t  length_in_words_minus_one
		) {
	*MBUS_CMD0 = ( ((uint32_t) remote_prefix) << 28 ) | (MPQ_MEM_BULK_WRITE << 24) | (length_in_words_minus_one & 0xFFFFF);
 208:	031b      	lsls	r3, r3, #12
 20a:	0b1b      	lsrs	r3, r3, #12
 20c:	0700      	lsls	r0, r0, #28
 20e:	4318      	orrs	r0, r3
 210:	2380      	movs	r3, #128	; 0x80
 212:	049b      	lsls	r3, r3, #18
 214:	4318      	orrs	r0, r3
 216:	4b05      	ldr	r3, [pc, #20]	; (22c <mbus_copy_mem_from_local_to_remote_bulk+0x24>)
 218:	6018      	str	r0, [r3, #0]
	*MBUS_CMD1 = (uint32_t) local_address;
 21a:	4b05      	ldr	r3, [pc, #20]	; (230 <mbus_copy_mem_from_local_to_remote_bulk+0x28>)
 21c:	601a      	str	r2, [r3, #0]
	*MBUS_CMD2 = (uint32_t) remote_memory_address;

	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x3 << 4);
 21e:	2233      	movs	r2, #51	; 0x33
	*MBUS_CMD2 = (uint32_t) remote_memory_address;
 220:	4b04      	ldr	r3, [pc, #16]	; (234 <mbus_copy_mem_from_local_to_remote_bulk+0x2c>)
 222:	6019      	str	r1, [r3, #0]
	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x3 << 4);
 224:	4b04      	ldr	r3, [pc, #16]	; (238 <mbus_copy_mem_from_local_to_remote_bulk+0x30>)
 226:	601a      	str	r2, [r3, #0]
}
 228:	4770      	bx	lr
 22a:	46c0      	nop			; (mov r8, r8)
 22c:	a0002000 	.word	0xa0002000
 230:	a0002004 	.word	0xa0002004
 234:	a0002008 	.word	0xa0002008
 238:	a000200c 	.word	0xa000200c

Disassembly of section .text.temp_sensor_power_off:

0000023c <temp_sensor_power_off>:
    // Release isolation
    sntv5_r01.TSNS_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
}
static void temp_sensor_power_off(){
    sntv5_r01.TSNS_RESETn = 0;
 23c:	2101      	movs	r1, #1
    sntv5_r01.TSNS_SEL_LDO = 0;
 23e:	2008      	movs	r0, #8
static void temp_sensor_power_off(){
 240:	b510      	push	{r4, lr}
    sntv5_r01.TSNS_RESETn = 0;
 242:	4b0a      	ldr	r3, [pc, #40]	; (26c <temp_sensor_power_off+0x30>)
 244:	881a      	ldrh	r2, [r3, #0]
 246:	438a      	bics	r2, r1
 248:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_SEL_LDO = 0;
 24a:	881a      	ldrh	r2, [r3, #0]
 24c:	4382      	bics	r2, r0
 24e:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_EN_SENSOR_LDO = 0;
 250:	881a      	ldrh	r2, [r3, #0]
 252:	3018      	adds	r0, #24
 254:	4382      	bics	r2, r0
 256:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_ISOLATE = 1;
 258:	2202      	movs	r2, #2
 25a:	8818      	ldrh	r0, [r3, #0]
 25c:	4302      	orrs	r2, r0
 25e:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
 260:	2005      	movs	r0, #5
 262:	681a      	ldr	r2, [r3, #0]
 264:	f7ff ffbd 	bl	1e2 <mbus_remote_register_write>
}
 268:	bd10      	pop	{r4, pc}
 26a:	46c0      	nop			; (mov r8, r8)
 26c:	00001d90 	.word	0x00001d90

Disassembly of section .text.sns_ldo_power_off:

00000270 <sns_ldo_power_off>:
    sntv5_r00.LDO_EN_IREF    = 1;
    sntv5_r00.LDO_EN_LDO    = 1;
    mbus_remote_register_write(SNT_ADDR,0x00,sntv5_r00.as_int);
}
static void sns_ldo_power_off(){
    sntv5_r00.LDO_EN_VREF    = 0;
 270:	2104      	movs	r1, #4
static void sns_ldo_power_off(){
 272:	b510      	push	{r4, lr}
    sntv5_r00.LDO_EN_VREF    = 0;
 274:	4b08      	ldr	r3, [pc, #32]	; (298 <sns_ldo_power_off+0x28>)
    sntv5_r00.LDO_EN_IREF    = 0;
    sntv5_r00.LDO_EN_LDO    = 0;
    mbus_remote_register_write(SNT_ADDR,0x00,sntv5_r00.as_int);
 276:	2005      	movs	r0, #5
    sntv5_r00.LDO_EN_VREF    = 0;
 278:	881a      	ldrh	r2, [r3, #0]
 27a:	438a      	bics	r2, r1
 27c:	801a      	strh	r2, [r3, #0]
    sntv5_r00.LDO_EN_IREF    = 0;
 27e:	881a      	ldrh	r2, [r3, #0]
 280:	3902      	subs	r1, #2
 282:	438a      	bics	r2, r1
 284:	801a      	strh	r2, [r3, #0]
    sntv5_r00.LDO_EN_LDO    = 0;
 286:	881a      	ldrh	r2, [r3, #0]
 288:	3901      	subs	r1, #1
 28a:	438a      	bics	r2, r1
 28c:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x00,sntv5_r00.as_int);
 28e:	2100      	movs	r1, #0
 290:	681a      	ldr	r2, [r3, #0]
 292:	f7ff ffa6 	bl	1e2 <mbus_remote_register_write>
}
 296:	bd10      	pop	{r4, pc}
 298:	00001d8c 	.word	0x00001d8c

Disassembly of section .text.snt_stop_timer:

0000029c <snt_stop_timer>:
    sntv5_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
}


static void snt_stop_timer(){
 29c:	b570      	push	{r4, r5, r6, lr}

    // EN_OSC
    sntv5_r08.TMR_EN_OSC = 0x0; // Default : 0x0
 29e:	2508      	movs	r5, #8
    // RESET
    sntv5_r08.TMR_EN_SELF_CLK = 0x0; // Default : 0x0
 2a0:	2201      	movs	r2, #1
    sntv5_r08.TMR_EN_OSC = 0x0; // Default : 0x0
 2a2:	4c19      	ldr	r4, [pc, #100]	; (308 <snt_stop_timer+0x6c>)
    sntv5_r08.TMR_RESETB = 0x0;// Default : 0x0
    sntv5_r08.TMR_RESETB_DIV = 0x0; // Default : 0x0
    sntv5_r08.TMR_RESETB_DCDC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
 2a4:	0029      	movs	r1, r5
    sntv5_r08.TMR_EN_OSC = 0x0; // Default : 0x0
 2a6:	7823      	ldrb	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
 2a8:	2005      	movs	r0, #5
    sntv5_r08.TMR_EN_OSC = 0x0; // Default : 0x0
 2aa:	43ab      	bics	r3, r5
 2ac:	7023      	strb	r3, [r4, #0]
    sntv5_r08.TMR_EN_SELF_CLK = 0x0; // Default : 0x0
 2ae:	7823      	ldrb	r3, [r4, #0]
 2b0:	4393      	bics	r3, r2
 2b2:	7023      	strb	r3, [r4, #0]
    sntv5_r08.TMR_RESETB = 0x0;// Default : 0x0
 2b4:	7823      	ldrb	r3, [r4, #0]
 2b6:	320f      	adds	r2, #15
 2b8:	4393      	bics	r3, r2
 2ba:	7023      	strb	r3, [r4, #0]
    sntv5_r08.TMR_RESETB_DIV = 0x0; // Default : 0x0
 2bc:	7823      	ldrb	r3, [r4, #0]
 2be:	3a0c      	subs	r2, #12
 2c0:	4393      	bics	r3, r2
 2c2:	7023      	strb	r3, [r4, #0]
    sntv5_r08.TMR_RESETB_DCDC = 0x0; // Default : 0x0
 2c4:	7823      	ldrb	r3, [r4, #0]
 2c6:	3a02      	subs	r2, #2
 2c8:	4393      	bics	r3, r2
 2ca:	7023      	strb	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
 2cc:	6822      	ldr	r2, [r4, #0]
 2ce:	f7ff ff88 	bl	1e2 <mbus_remote_register_write>
    snt_timer_enabled = 0;
 2d2:	2200      	movs	r2, #0
 2d4:	4b0d      	ldr	r3, [pc, #52]	; (30c <snt_stop_timer+0x70>)

    sntv5_r17.WUP_ENABLE = 0x0; // Default : 0x0
 2d6:	490e      	ldr	r1, [pc, #56]	; (310 <snt_stop_timer+0x74>)
    snt_timer_enabled = 0;
 2d8:	601a      	str	r2, [r3, #0]
    sntv5_r17.WUP_ENABLE = 0x0; // Default : 0x0
 2da:	4b0e      	ldr	r3, [pc, #56]	; (314 <snt_stop_timer+0x78>)
    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
 2dc:	2005      	movs	r0, #5
    sntv5_r17.WUP_ENABLE = 0x0; // Default : 0x0
 2de:	681a      	ldr	r2, [r3, #0]
 2e0:	400a      	ands	r2, r1
 2e2:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
 2e4:	681a      	ldr	r2, [r3, #0]
 2e6:	2117      	movs	r1, #23
 2e8:	f7ff ff7b 	bl	1e2 <mbus_remote_register_write>

    // New for SNTv3
    sntv5_r08.TMR_SLEEP = 0x1; // Default : 0x1
 2ec:	2340      	movs	r3, #64	; 0x40
 2ee:	7822      	ldrb	r2, [r4, #0]
    sntv5_r08.TMR_ISOLATE = 0x1; // Default : 0x1
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
 2f0:	0029      	movs	r1, r5
    sntv5_r08.TMR_SLEEP = 0x1; // Default : 0x1
 2f2:	4313      	orrs	r3, r2
 2f4:	7023      	strb	r3, [r4, #0]
    sntv5_r08.TMR_ISOLATE = 0x1; // Default : 0x1
 2f6:	2320      	movs	r3, #32
 2f8:	7822      	ldrb	r2, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
 2fa:	2005      	movs	r0, #5
    sntv5_r08.TMR_ISOLATE = 0x1; // Default : 0x1
 2fc:	4313      	orrs	r3, r2
 2fe:	7023      	strb	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
 300:	6822      	ldr	r2, [r4, #0]
 302:	f7ff ff6e 	bl	1e2 <mbus_remote_register_write>

}
 306:	bd70      	pop	{r4, r5, r6, pc}
 308:	00001d98 	.word	0x00001d98
 30c:	00001e28 	.word	0x00001e28
 310:	ff7fffff 	.word	0xff7fffff
 314:	00001da4 	.word	0x00001da4

Disassembly of section .text.radio_power_on:

00000318 <radio_power_on>:
static void radio_power_on(){
 318:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    mrrv11a_r04.LDO_EN_VREF    = 1;
 31a:	2380      	movs	r3, #128	; 0x80
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 31c:	2104      	movs	r1, #4
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
 31e:	2701      	movs	r7, #1
    mrrv11a_r04.LDO_EN_VREF    = 1;
 320:	4c56      	ldr	r4, [pc, #344]	; (47c <radio_power_on+0x164>)
 322:	015b      	lsls	r3, r3, #5
 324:	6822      	ldr	r2, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 326:	0008      	movs	r0, r1
    mrrv11a_r04.LDO_EN_VREF    = 1;
 328:	4313      	orrs	r3, r2
 32a:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 32c:	6822      	ldr	r2, [r4, #0]
 32e:	f7ff ff58 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
 332:	4e53      	ldr	r6, [pc, #332]	; (480 <radio_power_on+0x168>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 334:	2100      	movs	r1, #0
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
 336:	6833      	ldr	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 338:	2004      	movs	r0, #4
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
 33a:	43bb      	bics	r3, r7
 33c:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 33e:	6832      	ldr	r2, [r6, #0]
 340:	f7ff ff4f 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r03.TRX_DCP_S_OW1 = 0;  //TX_Decap S (forced charge decaps)
 344:	4d4f      	ldr	r5, [pc, #316]	; (484 <radio_power_on+0x16c>)
 346:	4a50      	ldr	r2, [pc, #320]	; (488 <radio_power_on+0x170>)
 348:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 34a:	2103      	movs	r1, #3
    mrrv11a_r03.TRX_DCP_S_OW1 = 0;  //TX_Decap S (forced charge decaps)
 34c:	4013      	ands	r3, r2
 34e:	602b      	str	r3, [r5, #0]
    mrrv11a_r03.TRX_DCP_S_OW2 = 0;  //TX_Decap S (forced charge decaps)
 350:	682b      	ldr	r3, [r5, #0]
 352:	4a4e      	ldr	r2, [pc, #312]	; (48c <radio_power_on+0x174>)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 354:	2004      	movs	r0, #4
    mrrv11a_r03.TRX_DCP_S_OW2 = 0;  //TX_Decap S (forced charge decaps)
 356:	4013      	ands	r3, r2
 358:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 35a:	682a      	ldr	r2, [r5, #0]
 35c:	f7ff ff41 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r03.TRX_DCP_P_OW1 = 1;  //RX_Decap P 
 360:	2380      	movs	r3, #128	; 0x80
 362:	682a      	ldr	r2, [r5, #0]
 364:	039b      	lsls	r3, r3, #14
 366:	4313      	orrs	r3, r2
 368:	602b      	str	r3, [r5, #0]
    mrrv11a_r03.TRX_DCP_P_OW2 = 1;  //RX_Decap P 
 36a:	2380      	movs	r3, #128	; 0x80
 36c:	682a      	ldr	r2, [r5, #0]
 36e:	035b      	lsls	r3, r3, #13
 370:	4313      	orrs	r3, r2
 372:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 374:	682a      	ldr	r2, [r5, #0]
 376:	2103      	movs	r1, #3
 378:	2004      	movs	r0, #4
 37a:	f7ff ff32 	bl	1e2 <mbus_remote_register_write>
    delay(MBUS_DELAY);
 37e:	20c8      	movs	r0, #200	; 0xc8
 380:	f7ff fe92 	bl	a8 <delay>
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
 384:	682b      	ldr	r3, [r5, #0]
 386:	4a42      	ldr	r2, [pc, #264]	; (490 <radio_power_on+0x178>)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 388:	2103      	movs	r1, #3
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
 38a:	4013      	ands	r3, r2
 38c:	602b      	str	r3, [r5, #0]
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
 38e:	682b      	ldr	r3, [r5, #0]
 390:	4a40      	ldr	r2, [pc, #256]	; (494 <radio_power_on+0x17c>)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 392:	2004      	movs	r0, #4
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
 394:	4013      	ands	r3, r2
 396:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 398:	682a      	ldr	r2, [r5, #0]
 39a:	f7ff ff22 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
 39e:	2380      	movs	r3, #128	; 0x80
 3a0:	682a      	ldr	r2, [r5, #0]
 3a2:	031b      	lsls	r3, r3, #12
 3a4:	4313      	orrs	r3, r2
 3a6:	602b      	str	r3, [r5, #0]
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
 3a8:	2380      	movs	r3, #128	; 0x80
 3aa:	682a      	ldr	r2, [r5, #0]
 3ac:	02db      	lsls	r3, r3, #11
 3ae:	4313      	orrs	r3, r2
 3b0:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 3b2:	682a      	ldr	r2, [r5, #0]
 3b4:	2103      	movs	r1, #3
 3b6:	2004      	movs	r0, #4
 3b8:	f7ff ff13 	bl	1e2 <mbus_remote_register_write>
    delay(MBUS_DELAY);
 3bc:	20c8      	movs	r0, #200	; 0xc8
 3be:	f7ff fe73 	bl	a8 <delay>
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
 3c2:	227e      	movs	r2, #126	; 0x7e
 3c4:	2520      	movs	r5, #32
 3c6:	6833      	ldr	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 3c8:	2100      	movs	r1, #0
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
 3ca:	4393      	bics	r3, r2
 3cc:	432b      	orrs	r3, r5
 3ce:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 3d0:	6832      	ldr	r2, [r6, #0]
 3d2:	2004      	movs	r0, #4
 3d4:	f7ff ff05 	bl	1e2 <mbus_remote_register_write>
    radio_on = 1;
 3d8:	4b2f      	ldr	r3, [pc, #188]	; (498 <radio_power_on+0x180>)
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 3da:	2104      	movs	r1, #4
    radio_on = 1;
 3dc:	601f      	str	r7, [r3, #0]
    mrrv11a_r04.LDO_EN_IREF    = 1;
 3de:	2380      	movs	r3, #128	; 0x80
 3e0:	6822      	ldr	r2, [r4, #0]
 3e2:	011b      	lsls	r3, r3, #4
 3e4:	4313      	orrs	r3, r2
 3e6:	6023      	str	r3, [r4, #0]
    mrrv11a_r04.LDO_EN_LDO    = 1;
 3e8:	2380      	movs	r3, #128	; 0x80
 3ea:	6822      	ldr	r2, [r4, #0]
 3ec:	00db      	lsls	r3, r3, #3
 3ee:	4313      	orrs	r3, r2
 3f0:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 3f2:	0008      	movs	r0, r1
 3f4:	6822      	ldr	r2, [r4, #0]
 3f6:	f7ff fef4 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
 3fa:	6833      	ldr	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 3fc:	2100      	movs	r1, #0
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
 3fe:	433b      	orrs	r3, r7
 400:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 402:	6832      	ldr	r2, [r6, #0]
 404:	2004      	movs	r0, #4
 406:	f7ff feec 	bl	1e2 <mbus_remote_register_write>
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 40a:	2104      	movs	r1, #4
    mrrv11a_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
 40c:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 40e:	0008      	movs	r0, r1
    mrrv11a_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
 410:	433b      	orrs	r3, r7
 412:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 414:	6822      	ldr	r2, [r4, #0]
 416:	f7ff fee4 	bl	1e2 <mbus_remote_register_write>
    delay(MBUS_DELAY);
 41a:	20c8      	movs	r0, #200	; 0xc8
 41c:	f7ff fe44 	bl	a8 <delay>
    mrrv11a_r04.RO_RESET = 0;  //Release Reset TIMER
 420:	2208      	movs	r2, #8
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 422:	2104      	movs	r1, #4
    mrrv11a_r04.RO_RESET = 0;  //Release Reset TIMER
 424:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 426:	0008      	movs	r0, r1
    mrrv11a_r04.RO_RESET = 0;  //Release Reset TIMER
 428:	4393      	bics	r3, r2
 42a:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 42c:	6822      	ldr	r2, [r4, #0]
 42e:	f7ff fed8 	bl	1e2 <mbus_remote_register_write>
    delay(MBUS_DELAY);
 432:	20c8      	movs	r0, #200	; 0xc8
 434:	f7ff fe38 	bl	a8 <delay>
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 438:	2104      	movs	r1, #4
    mrrv11a_r04.RO_EN_CLK = 1; //Enable CLK TIMER
 43a:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 43c:	0008      	movs	r0, r1
    mrrv11a_r04.RO_EN_CLK = 1; //Enable CLK TIMER
 43e:	431d      	orrs	r5, r3
 440:	6025      	str	r5, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 442:	6822      	ldr	r2, [r4, #0]
 444:	f7ff fecd 	bl	1e2 <mbus_remote_register_write>
    delay(MBUS_DELAY);
 448:	20c8      	movs	r0, #200	; 0xc8
 44a:	f7ff fe2d 	bl	a8 <delay>
    mrrv11a_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
 44e:	2210      	movs	r2, #16
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 450:	2104      	movs	r1, #4
    mrrv11a_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
 452:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 454:	0008      	movs	r0, r1
    mrrv11a_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
 456:	4393      	bics	r3, r2
 458:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 45a:	6822      	ldr	r2, [r4, #0]
 45c:	f7ff fec1 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r11.FSM_SLEEP = 0;  // Power on BB
 460:	4b0e      	ldr	r3, [pc, #56]	; (49c <radio_power_on+0x184>)
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 462:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_SLEEP = 0;  // Power on BB
 464:	681a      	ldr	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 466:	2004      	movs	r0, #4
    mrrv11a_r11.FSM_SLEEP = 0;  // Power on BB
 468:	43ba      	bics	r2, r7
 46a:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 46c:	681a      	ldr	r2, [r3, #0]
 46e:	f7ff feb8 	bl	1e2 <mbus_remote_register_write>
    delay(MBUS_DELAY*5); // Freq stab
 472:	20fa      	movs	r0, #250	; 0xfa
 474:	0080      	lsls	r0, r0, #2
 476:	f7ff fe17 	bl	a8 <delay>
}
 47a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 47c:	00001d64 	.word	0x00001d64
 480:	00001d58 	.word	0x00001d58
 484:	00001d60 	.word	0x00001d60
 488:	fff7ffff 	.word	0xfff7ffff
 48c:	fffbffff 	.word	0xfffbffff
 490:	ffdfffff 	.word	0xffdfffff
 494:	ffefffff 	.word	0xffefffff
 498:	00001e64 	.word	0x00001e64
 49c:	00001d70 	.word	0x00001d70

Disassembly of section .text.radio_power_off:

000004a0 <radio_power_off>:
static void radio_power_off(){
 4a0:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
 4a2:	2708      	movs	r7, #8
 4a4:	4c37      	ldr	r4, [pc, #220]	; (584 <radio_power_off+0xe4>)
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 4a6:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
 4a8:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 4aa:	2004      	movs	r0, #4
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
 4ac:	43bb      	bics	r3, r7
 4ae:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 4b0:	6822      	ldr	r2, [r4, #0]
 4b2:	f7ff fe96 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r03.TRX_ISOLATEN = 0;     //set ISOLATEN 0
 4b6:	4b34      	ldr	r3, [pc, #208]	; (588 <radio_power_off+0xe8>)
 4b8:	4934      	ldr	r1, [pc, #208]	; (58c <radio_power_off+0xec>)
 4ba:	681a      	ldr	r2, [r3, #0]
    mrrv11a_r11.FSM_EN = 0;  //Stop BB
 4bc:	2504      	movs	r5, #4
    mrrv11a_r03.TRX_ISOLATEN = 0;     //set ISOLATEN 0
 4be:	400a      	ands	r2, r1
 4c0:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 4c2:	681a      	ldr	r2, [r3, #0]
 4c4:	2103      	movs	r1, #3
 4c6:	2004      	movs	r0, #4
 4c8:	f7ff fe8b 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
 4cc:	2202      	movs	r2, #2
    mrrv11a_r11.FSM_SLEEP = 1;
 4ce:	2601      	movs	r6, #1
    mrrv11a_r11.FSM_EN = 0;  //Stop BB
 4d0:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 4d2:	0028      	movs	r0, r5
    mrrv11a_r11.FSM_EN = 0;  //Stop BB
 4d4:	43ab      	bics	r3, r5
 4d6:	6023      	str	r3, [r4, #0]
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
 4d8:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 4da:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
 4dc:	4393      	bics	r3, r2
 4de:	6023      	str	r3, [r4, #0]
    mrrv11a_r11.FSM_SLEEP = 1;
 4e0:	6823      	ldr	r3, [r4, #0]
 4e2:	4333      	orrs	r3, r6
 4e4:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 4e6:	6822      	ldr	r2, [r4, #0]
 4e8:	f7ff fe7b 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
 4ec:	4c28      	ldr	r4, [pc, #160]	; (590 <radio_power_off+0xf0>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 4ee:	0028      	movs	r0, r5
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
 4f0:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 4f2:	2100      	movs	r1, #0
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
 4f4:	43b3      	bics	r3, r6
 4f6:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 4f8:	6822      	ldr	r2, [r4, #0]
 4fa:	f7ff fe72 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
 4fe:	227e      	movs	r2, #126	; 0x7e
 500:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 502:	0028      	movs	r0, r5
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
 504:	4393      	bics	r3, r2
 506:	3a5e      	subs	r2, #94	; 0x5e
 508:	4313      	orrs	r3, r2
 50a:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 50c:	6822      	ldr	r2, [r4, #0]
 50e:	2100      	movs	r1, #0
 510:	f7ff fe67 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
 514:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 516:	0028      	movs	r0, r5
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
 518:	4333      	orrs	r3, r6
 51a:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 51c:	6822      	ldr	r2, [r4, #0]
 51e:	2100      	movs	r1, #0
 520:	f7ff fe5f 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r04.RO_EN_CLK = 0; //Enable CLK TIMER
 524:	2220      	movs	r2, #32
    mrrv11a_r04.RO_RESET = 1;  //Release Reset TIMER
 526:	4c1b      	ldr	r4, [pc, #108]	; (594 <radio_power_off+0xf4>)
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 528:	0029      	movs	r1, r5
    mrrv11a_r04.RO_RESET = 1;  //Release Reset TIMER
 52a:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 52c:	0028      	movs	r0, r5
    mrrv11a_r04.RO_RESET = 1;  //Release Reset TIMER
 52e:	431f      	orrs	r7, r3
 530:	6027      	str	r7, [r4, #0]
    mrrv11a_r04.RO_EN_CLK = 0; //Enable CLK TIMER
 532:	6823      	ldr	r3, [r4, #0]
 534:	4393      	bics	r3, r2
 536:	6023      	str	r3, [r4, #0]
    mrrv11a_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
 538:	2310      	movs	r3, #16
 53a:	6822      	ldr	r2, [r4, #0]
 53c:	4313      	orrs	r3, r2
 53e:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 540:	6822      	ldr	r2, [r4, #0]
 542:	f7ff fe4e 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r04.LDO_EN_VREF    = 0;
 546:	6823      	ldr	r3, [r4, #0]
 548:	4a13      	ldr	r2, [pc, #76]	; (598 <radio_power_off+0xf8>)
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 54a:	0029      	movs	r1, r5
    mrrv11a_r04.LDO_EN_VREF    = 0;
 54c:	4013      	ands	r3, r2
 54e:	6023      	str	r3, [r4, #0]
    mrrv11a_r04.LDO_EN_IREF    = 0;
 550:	6823      	ldr	r3, [r4, #0]
 552:	4a12      	ldr	r2, [pc, #72]	; (59c <radio_power_off+0xfc>)
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 554:	0028      	movs	r0, r5
    mrrv11a_r04.LDO_EN_IREF    = 0;
 556:	4013      	ands	r3, r2
 558:	6023      	str	r3, [r4, #0]
    mrrv11a_r04.LDO_EN_LDO    = 0;
 55a:	6823      	ldr	r3, [r4, #0]
 55c:	4a10      	ldr	r2, [pc, #64]	; (5a0 <radio_power_off+0x100>)
 55e:	4013      	ands	r3, r2
 560:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 562:	6822      	ldr	r2, [r4, #0]
 564:	f7ff fe3d 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
 568:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 56a:	0029      	movs	r1, r5
    mrrv11a_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
 56c:	43b3      	bics	r3, r6
 56e:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
 570:	0028      	movs	r0, r5
 572:	6822      	ldr	r2, [r4, #0]
 574:	f7ff fe35 	bl	1e2 <mbus_remote_register_write>
    radio_on = 0;
 578:	2300      	movs	r3, #0
 57a:	4a0a      	ldr	r2, [pc, #40]	; (5a4 <radio_power_off+0x104>)
 57c:	6013      	str	r3, [r2, #0]
    radio_ready = 0;
 57e:	4a0a      	ldr	r2, [pc, #40]	; (5a8 <radio_power_off+0x108>)
 580:	6013      	str	r3, [r2, #0]
}
 582:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 584:	00001d70 	.word	0x00001d70
 588:	00001d60 	.word	0x00001d60
 58c:	ffbfffff 	.word	0xffbfffff
 590:	00001d58 	.word	0x00001d58
 594:	00001d64 	.word	0x00001d64
 598:	ffffefff 	.word	0xffffefff
 59c:	fffff7ff 	.word	0xfffff7ff
 5a0:	fffffbff 	.word	0xfffffbff
 5a4:	00001e64 	.word	0x00001e64
 5a8:	00001dc0 	.word	0x00001dc0

Disassembly of section .text.set_timer32_timeout:

000005ac <set_timer32_timeout>:
    wfi_timeout_flag = 0;
 5ac:	2200      	movs	r2, #0
static void set_timer32_timeout(uint32_t val){
 5ae:	b510      	push	{r4, lr}
    wfi_timeout_flag = 0;
 5b0:	4b03      	ldr	r3, [pc, #12]	; (5c0 <set_timer32_timeout+0x14>)
    config_timer32(val, 1, 0, 0);
 5b2:	2101      	movs	r1, #1
    wfi_timeout_flag = 0;
 5b4:	601a      	str	r2, [r3, #0]
    config_timer32(val, 1, 0, 0);
 5b6:	0013      	movs	r3, r2
 5b8:	f7ff fd7e 	bl	b8 <config_timer32>
}
 5bc:	bd10      	pop	{r4, pc}
 5be:	46c0      	nop			; (mov r8, r8)
 5c0:	00001e60 	.word	0x00001e60

Disassembly of section .text.stop_timer32_timeout_check:

000005c4 <stop_timer32_timeout_check>:
    if (irq_pending.timer32) {
 5c4:	4a11      	ldr	r2, [pc, #68]	; (60c <stop_timer32_timeout_check+0x48>)
static void stop_timer32_timeout_check(uint32_t code){
 5c6:	b510      	push	{r4, lr}
    if (irq_pending.timer32) {
 5c8:	6811      	ldr	r1, [r2, #0]
 5ca:	4b11      	ldr	r3, [pc, #68]	; (610 <stop_timer32_timeout_check+0x4c>)
 5cc:	0709      	lsls	r1, r1, #28
 5ce:	d50f      	bpl.n	5f0 <stop_timer32_timeout_check+0x2c>
        irq_pending.timer32 = 0;
 5d0:	2408      	movs	r4, #8
 5d2:	6811      	ldr	r1, [r2, #0]
 5d4:	43a1      	bics	r1, r4
 5d6:	6011      	str	r1, [r2, #0]
        *REG1 = *TIMER32_CNT;
 5d8:	4a0e      	ldr	r2, [pc, #56]	; (614 <stop_timer32_timeout_check+0x50>)
 5da:	6811      	ldr	r1, [r2, #0]
 5dc:	4a0e      	ldr	r2, [pc, #56]	; (618 <stop_timer32_timeout_check+0x54>)
 5de:	6011      	str	r1, [r2, #0]
        *REG2 = *TIMER32_STAT;
 5e0:	4a0e      	ldr	r2, [pc, #56]	; (61c <stop_timer32_timeout_check+0x58>)
 5e2:	490f      	ldr	r1, [pc, #60]	; (620 <stop_timer32_timeout_check+0x5c>)
 5e4:	6814      	ldr	r4, [r2, #0]
 5e6:	600c      	str	r4, [r1, #0]
        *TIMER32_STAT = 0x0;
 5e8:	2100      	movs	r1, #0
 5ea:	6011      	str	r1, [r2, #0]
        wfi_timeout_flag = 1;
 5ec:	2201      	movs	r2, #1
 5ee:	601a      	str	r2, [r3, #0]
    *TIMER32_GO = 0;
 5f0:	2200      	movs	r2, #0
 5f2:	490c      	ldr	r1, [pc, #48]	; (624 <stop_timer32_timeout_check+0x60>)
 5f4:	600a      	str	r2, [r1, #0]
    if (wfi_timeout_flag){
 5f6:	6819      	ldr	r1, [r3, #0]
 5f8:	4291      	cmp	r1, r2
 5fa:	d006      	beq.n	60a <stop_timer32_timeout_check+0x46>
        wfi_timeout_flag = 0;
 5fc:	601a      	str	r2, [r3, #0]
        error_code = code;
 5fe:	4b0a      	ldr	r3, [pc, #40]	; (628 <stop_timer32_timeout_check+0x64>)
 600:	6018      	str	r0, [r3, #0]
        mbus_write_message32(0xFA, error_code);
 602:	6819      	ldr	r1, [r3, #0]
 604:	20fa      	movs	r0, #250	; 0xfa
 606:	f7ff fdad 	bl	164 <mbus_write_message32>
}
 60a:	bd10      	pop	{r4, pc}
 60c:	00001d54 	.word	0x00001d54
 610:	00001e60 	.word	0x00001e60
 614:	a000110c 	.word	0xa000110c
 618:	a0000004 	.word	0xa0000004
 61c:	a0001110 	.word	0xa0001110
 620:	a0000008 	.word	0xa0000008
 624:	a0001100 	.word	0xa0001100
 628:	00001e50 	.word	0x00001e50

Disassembly of section .text.snt_read_wup_counter:

0000062c <snt_read_wup_counter>:
    set_timer32_timeout(TIMER32_VAL);
 62c:	20a0      	movs	r0, #160	; 0xa0
static void snt_read_wup_counter(){
 62e:	b510      	push	{r4, lr}
    set_timer32_timeout(TIMER32_VAL);
 630:	02c0      	lsls	r0, r0, #11
 632:	f7ff ffbb 	bl	5ac <set_timer32_timeout>
    mbus_remote_register_write(SNT_ADDR,0x14,   // WUP_CNT_READ_REQ (Memory-Mapped)
 636:	2280      	movs	r2, #128	; 0x80
 638:	2114      	movs	r1, #20
 63a:	0152      	lsls	r2, r2, #5
 63c:	2005      	movs	r0, #5
 63e:	f7ff fdd0 	bl	1e2 <mbus_remote_register_write>
    while ((!irq_pending.reg1) & (!irq_pending.timer32));
 642:	4a0d      	ldr	r2, [pc, #52]	; (678 <snt_read_wup_counter+0x4c>)
 644:	6811      	ldr	r1, [r2, #0]
 646:	6813      	ldr	r3, [r2, #0]
 648:	0589      	lsls	r1, r1, #22
 64a:	071b      	lsls	r3, r3, #28
 64c:	430b      	orrs	r3, r1
 64e:	d5f9      	bpl.n	644 <snt_read_wup_counter+0x18>
    irq_pending.reg1 = 0;
 650:	6813      	ldr	r3, [r2, #0]
 652:	490a      	ldr	r1, [pc, #40]	; (67c <snt_read_wup_counter+0x50>)
    stop_timer32_timeout_check(code);
 654:	2004      	movs	r0, #4
    irq_pending.reg1 = 0;
 656:	400b      	ands	r3, r1
 658:	6013      	str	r3, [r2, #0]
    stop_timer32_timeout_check(code);
 65a:	f7ff ffb3 	bl	5c4 <stop_timer32_timeout_check>
    snt_wup_counter_cur = ((*REG0 & 0x000000FF) << 24) | (*REG1 & 0x00FFFFFF);
 65e:	23a0      	movs	r3, #160	; 0xa0
 660:	4a07      	ldr	r2, [pc, #28]	; (680 <snt_read_wup_counter+0x54>)
 662:	061b      	lsls	r3, r3, #24
 664:	681b      	ldr	r3, [r3, #0]
 666:	6812      	ldr	r2, [r2, #0]
 668:	061b      	lsls	r3, r3, #24
 66a:	0212      	lsls	r2, r2, #8
 66c:	0a12      	lsrs	r2, r2, #8
 66e:	4313      	orrs	r3, r2
 670:	4a04      	ldr	r2, [pc, #16]	; (684 <snt_read_wup_counter+0x58>)
 672:	6013      	str	r3, [r2, #0]
}
 674:	bd10      	pop	{r4, pc}
 676:	46c0      	nop			; (mov r8, r8)
 678:	00001d54 	.word	0x00001d54
 67c:	fffffdff 	.word	0xfffffdff
 680:	a0000004 	.word	0xa0000004
 684:	00001e5c 	.word	0x00001e5c

Disassembly of section .text.pmu_reg_write:

00000688 <pmu_reg_write>:
static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
 688:	b570      	push	{r4, r5, r6, lr}
 68a:	0004      	movs	r4, r0
    set_timer32_timeout(TIMER32_VAL);
 68c:	20a0      	movs	r0, #160	; 0xa0
static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
 68e:	000d      	movs	r5, r1
    set_timer32_timeout(TIMER32_VAL);
 690:	02c0      	lsls	r0, r0, #11
 692:	f7ff ff8b 	bl	5ac <set_timer32_timeout>
    mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
 696:	002a      	movs	r2, r5
 698:	2006      	movs	r0, #6
 69a:	b2e1      	uxtb	r1, r4
 69c:	f7ff fda1 	bl	1e2 <mbus_remote_register_write>
    while ((!irq_pending.reg0) & (!irq_pending.timer32));
 6a0:	4a07      	ldr	r2, [pc, #28]	; (6c0 <pmu_reg_write+0x38>)
 6a2:	6811      	ldr	r1, [r2, #0]
 6a4:	6813      	ldr	r3, [r2, #0]
 6a6:	05c9      	lsls	r1, r1, #23
 6a8:	071b      	lsls	r3, r3, #28
 6aa:	430b      	orrs	r3, r1
 6ac:	d5f9      	bpl.n	6a2 <pmu_reg_write+0x1a>
    irq_pending.reg0 = 0;
 6ae:	6813      	ldr	r3, [r2, #0]
 6b0:	4904      	ldr	r1, [pc, #16]	; (6c4 <pmu_reg_write+0x3c>)
    stop_timer32_timeout_check(code);
 6b2:	2007      	movs	r0, #7
    irq_pending.reg0 = 0;
 6b4:	400b      	ands	r3, r1
 6b6:	6013      	str	r3, [r2, #0]
    stop_timer32_timeout_check(code);
 6b8:	f7ff ff84 	bl	5c4 <stop_timer32_timeout_check>
}
 6bc:	bd70      	pop	{r4, r5, r6, pc}
 6be:	46c0      	nop			; (mov r8, r8)
 6c0:	00001d54 	.word	0x00001d54
 6c4:	fffffeff 	.word	0xfffffeff

Disassembly of section .text.pmu_set_active_clk:

000006c8 <pmu_set_active_clk>:
inline static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){
 6c8:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
        | (r << 9)      // Frequency multiplier R
 6ca:	26c0      	movs	r6, #192	; 0xc0
 6cc:	0245      	lsls	r5, r0, #9
inline static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){
 6ce:	000c      	movs	r4, r1
        | (r << 9)      // Frequency multiplier R
 6d0:	0236      	lsls	r6, r6, #8
        | (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
 6d2:	0159      	lsls	r1, r3, #5
        | (r << 9)      // Frequency multiplier R
 6d4:	432e      	orrs	r6, r5
        | (base)        // Floor frequency base (0-63)
 6d6:	4311      	orrs	r1, r2
inline static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){
 6d8:	0017      	movs	r7, r2
        | (base)        // Floor frequency base (0-63)
 6da:	4331      	orrs	r1, r6
    pmu_reg_write(0x16, 
 6dc:	2016      	movs	r0, #22
 6de:	f7ff ffd3 	bl	688 <pmu_reg_write>
        | (base)    // Floor frequency base (0-63)
 6e2:	0031      	movs	r1, r6
        | (l << 5)  // Frequency multiplier L (actually L+1)
 6e4:	0164      	lsls	r4, r4, #5
 6e6:	4327      	orrs	r7, r4
        | (base)    // Floor frequency base (0-63)
 6e8:	4339      	orrs	r1, r7
    pmu_reg_write(0x18, 
 6ea:	2018      	movs	r0, #24
 6ec:	f7ff ffcc 	bl	688 <pmu_reg_write>
        | (base)    // Floor frequency base (0-63)
 6f0:	0029      	movs	r1, r5
    pmu_reg_write(0x1A,
 6f2:	201a      	movs	r0, #26
        | (base)    // Floor frequency base (0-63)
 6f4:	4339      	orrs	r1, r7
    pmu_reg_write(0x1A,
 6f6:	f7ff ffc7 	bl	688 <pmu_reg_write>
}
 6fa:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

Disassembly of section .text.pmu_active_setting_temp_based:

000006fc <pmu_active_setting_temp_based>:
    if (pmu_setting_state >= PMU_95C){
 6fc:	4a33      	ldr	r2, [pc, #204]	; (7cc <pmu_active_setting_temp_based+0xd0>)
inline static void pmu_active_setting_temp_based(){
 6fe:	b510      	push	{r4, lr}
    if (pmu_setting_state >= PMU_95C){
 700:	6813      	ldr	r3, [r2, #0]
 702:	2b0c      	cmp	r3, #12
 704:	d906      	bls.n	714 <pmu_active_setting_temp_based+0x18>
        pmu_set_active_clk(0x1,0x1,0x10,0x2/*V1P2*/);
 706:	2302      	movs	r3, #2
 708:	2210      	movs	r2, #16
 70a:	2101      	movs	r1, #1
        pmu_set_active_clk(0x2,0x2,0x10,0x4/*V1P2*/);
 70c:	0008      	movs	r0, r1
        pmu_set_active_clk(0xF,0xA,0x1F,0xE/*V1P2*/);
 70e:	f7ff ffdb 	bl	6c8 <pmu_set_active_clk>
}
 712:	bd10      	pop	{r4, pc}
    }else if (pmu_setting_state == PMU_85C){
 714:	6813      	ldr	r3, [r2, #0]
 716:	2b0c      	cmp	r3, #12
 718:	d104      	bne.n	724 <pmu_active_setting_temp_based+0x28>
        pmu_set_active_clk(0x2,0x1,0x10,0x2/*V1P2*/);
 71a:	2210      	movs	r2, #16
 71c:	2101      	movs	r1, #1
 71e:	3b0a      	subs	r3, #10
        pmu_set_active_clk(0xE,0xA,0x10,0xE/*V1P2*/);
 720:	0018      	movs	r0, r3
 722:	e7f4      	b.n	70e <pmu_active_setting_temp_based+0x12>
    }else if (pmu_setting_state == PMU_75C){
 724:	6813      	ldr	r3, [r2, #0]
 726:	2b0b      	cmp	r3, #11
 728:	d103      	bne.n	732 <pmu_active_setting_temp_based+0x36>
        pmu_set_active_clk(0x2,0x2,0x10,0x4/*V1P2*/);
 72a:	2210      	movs	r2, #16
 72c:	2102      	movs	r1, #2
 72e:	3b07      	subs	r3, #7
 730:	e7ec      	b.n	70c <pmu_active_setting_temp_based+0x10>
    }else if (pmu_setting_state == PMU_65C){
 732:	6813      	ldr	r3, [r2, #0]
 734:	2b0a      	cmp	r3, #10
 736:	d103      	bne.n	740 <pmu_active_setting_temp_based+0x44>
        pmu_set_active_clk(0x4,0x2,0x10,0x4/*V1P2*/);
 738:	2210      	movs	r2, #16
 73a:	2102      	movs	r1, #2
 73c:	3b06      	subs	r3, #6
 73e:	e7ef      	b.n	720 <pmu_active_setting_temp_based+0x24>
    }else if (pmu_setting_state == PMU_55C){
 740:	6813      	ldr	r3, [r2, #0]
 742:	2b09      	cmp	r3, #9
 744:	d104      	bne.n	750 <pmu_active_setting_temp_based+0x54>
        pmu_set_active_clk(0x6,0x2,0x10,0x4/*V1P2*/);
 746:	2210      	movs	r2, #16
 748:	2102      	movs	r1, #2
 74a:	2006      	movs	r0, #6
 74c:	3b05      	subs	r3, #5
 74e:	e7de      	b.n	70e <pmu_active_setting_temp_based+0x12>
    }else if (pmu_setting_state == PMU_45C){
 750:	6813      	ldr	r3, [r2, #0]
 752:	2b08      	cmp	r3, #8
 754:	d104      	bne.n	760 <pmu_active_setting_temp_based+0x64>
        pmu_set_active_clk(0x9,0x2,0x10,0x4/*V1P2*/);
 756:	2210      	movs	r2, #16
 758:	2102      	movs	r1, #2
 75a:	2009      	movs	r0, #9
 75c:	3b04      	subs	r3, #4
 75e:	e7d6      	b.n	70e <pmu_active_setting_temp_based+0x12>
    }else if (pmu_setting_state == PMU_40C){
 760:	6813      	ldr	r3, [r2, #0]
 762:	2b07      	cmp	r3, #7
 764:	d104      	bne.n	770 <pmu_active_setting_temp_based+0x74>
        pmu_set_active_clk(0xB,0x2,0x10,0x4/*V1P2*/);
 766:	2210      	movs	r2, #16
 768:	2102      	movs	r1, #2
 76a:	200b      	movs	r0, #11
 76c:	3b03      	subs	r3, #3
 76e:	e7ce      	b.n	70e <pmu_active_setting_temp_based+0x12>
    }else if (pmu_setting_state == PMU_35C){
 770:	6813      	ldr	r3, [r2, #0]
 772:	2b06      	cmp	r3, #6
 774:	d104      	bne.n	780 <pmu_active_setting_temp_based+0x84>
        pmu_set_active_clk(0xD,0x2,0x10,0x4/*V1P2*/);
 776:	2210      	movs	r2, #16
 778:	2102      	movs	r1, #2
 77a:	3b02      	subs	r3, #2
        pmu_set_active_clk(0xD,0xA,0x10,0xE/*V1P2*/);
 77c:	200d      	movs	r0, #13
 77e:	e7c6      	b.n	70e <pmu_active_setting_temp_based+0x12>
    }else if (pmu_setting_state == PMU_30C){
 780:	6813      	ldr	r3, [r2, #0]
 782:	2b05      	cmp	r3, #5
 784:	d103      	bne.n	78e <pmu_active_setting_temp_based+0x92>
        pmu_set_active_clk(0xF,0x3,0x10,0x5/*V1P2*/);
 786:	2210      	movs	r2, #16
 788:	2103      	movs	r1, #3
        pmu_set_active_clk(0xF,0xA,0x1F,0xE/*V1P2*/);
 78a:	200f      	movs	r0, #15
 78c:	e7bf      	b.n	70e <pmu_active_setting_temp_based+0x12>
    }else if (pmu_setting_state == PMU_25C){
 78e:	6813      	ldr	r3, [r2, #0]
 790:	2b04      	cmp	r3, #4
 792:	d103      	bne.n	79c <pmu_active_setting_temp_based+0xa0>
        pmu_set_active_clk(0xF,0x5,0x10,0xA/*V1P2*/);
 794:	2210      	movs	r2, #16
 796:	2105      	movs	r1, #5
 798:	3306      	adds	r3, #6
 79a:	e7f6      	b.n	78a <pmu_active_setting_temp_based+0x8e>
    }else if (pmu_setting_state == PMU_20C){
 79c:	6813      	ldr	r3, [r2, #0]
 79e:	2b03      	cmp	r3, #3
 7a0:	d103      	bne.n	7aa <pmu_active_setting_temp_based+0xae>
        pmu_set_active_clk(0xD,0xA,0x10,0xE/*V1P2*/);
 7a2:	2210      	movs	r2, #16
 7a4:	210a      	movs	r1, #10
 7a6:	330b      	adds	r3, #11
 7a8:	e7e8      	b.n	77c <pmu_active_setting_temp_based+0x80>
    }else if (pmu_setting_state == PMU_15C){
 7aa:	6813      	ldr	r3, [r2, #0]
 7ac:	2b02      	cmp	r3, #2
 7ae:	d103      	bne.n	7b8 <pmu_active_setting_temp_based+0xbc>
        pmu_set_active_clk(0xE,0xA,0x10,0xE/*V1P2*/);
 7b0:	2210      	movs	r2, #16
 7b2:	210a      	movs	r1, #10
 7b4:	330c      	adds	r3, #12
 7b6:	e7b3      	b.n	720 <pmu_active_setting_temp_based+0x24>
    }else if (pmu_setting_state == PMU_10C){
 7b8:	6813      	ldr	r3, [r2, #0]
 7ba:	2b01      	cmp	r3, #1
 7bc:	d103      	bne.n	7c6 <pmu_active_setting_temp_based+0xca>
        pmu_set_active_clk(0xF,0xA,0x10,0xE/*V1P2*/);
 7be:	2210      	movs	r2, #16
 7c0:	330d      	adds	r3, #13
        pmu_set_active_clk(0xF,0xA,0x1F,0xE/*V1P2*/);
 7c2:	210a      	movs	r1, #10
 7c4:	e7e1      	b.n	78a <pmu_active_setting_temp_based+0x8e>
 7c6:	230e      	movs	r3, #14
 7c8:	221f      	movs	r2, #31
 7ca:	e7fa      	b.n	7c2 <pmu_active_setting_temp_based+0xc6>
 7cc:	00001e10 	.word	0x00001e10

Disassembly of section .text.pmu_set_sleep_clk:

000007d0 <pmu_set_sleep_clk>:
inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){
 7d0:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
        | (r << 9)  // Frequency multiplier R
 7d2:	26c0      	movs	r6, #192	; 0xc0
 7d4:	0244      	lsls	r4, r0, #9
 7d6:	0236      	lsls	r6, r6, #8
        | (l << 5)  // Frequency multiplier L (actually L+1)
 7d8:	0149      	lsls	r1, r1, #5
 7da:	4311      	orrs	r1, r2
        | (r << 9)  // Frequency multiplier R
 7dc:	4326      	orrs	r6, r4
 7de:	000d      	movs	r5, r1
        | (base)    // Floor frequency base (0-63)
 7e0:	0031      	movs	r1, r6
inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){
 7e2:	0017      	movs	r7, r2
        | (base)    // Floor frequency base (0-63)
 7e4:	4329      	orrs	r1, r5
    pmu_reg_write(0x17, 
 7e6:	2017      	movs	r0, #23
inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){
 7e8:	9301      	str	r3, [sp, #4]
    pmu_reg_write(0x17, 
 7ea:	f7ff ff4d 	bl	688 <pmu_reg_write>
        | (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
 7ee:	9b01      	ldr	r3, [sp, #4]
    pmu_reg_write(0x15, 
 7f0:	2015      	movs	r0, #21
        | (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
 7f2:	0159      	lsls	r1, r3, #5
        | (base)        // Floor frequency base (0-63)
 7f4:	4339      	orrs	r1, r7
 7f6:	4331      	orrs	r1, r6
    pmu_reg_write(0x15, 
 7f8:	f7ff ff46 	bl	688 <pmu_reg_write>
        | (base)    // Floor frequency base (0-63)
 7fc:	0021      	movs	r1, r4
    pmu_reg_write(0x19,
 7fe:	2019      	movs	r0, #25
        | (base)    // Floor frequency base (0-63)
 800:	4329      	orrs	r1, r5
    pmu_reg_write(0x19,
 802:	f7ff ff41 	bl	688 <pmu_reg_write>
}
 806:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}

Disassembly of section .text.pmu_set_sleep_low:

00000808 <pmu_set_sleep_low>:
    pmu_set_sleep_clk(0xF,0x0,0x1,0x1/*V1P2*/);
 808:	2301      	movs	r3, #1
inline static void pmu_set_sleep_low(){
 80a:	b510      	push	{r4, lr}
    pmu_set_sleep_clk(0xF,0x0,0x1,0x1/*V1P2*/);
 80c:	001a      	movs	r2, r3
 80e:	2100      	movs	r1, #0
 810:	200f      	movs	r0, #15
 812:	f7ff ffdd 	bl	7d0 <pmu_set_sleep_clk>
}
 816:	bd10      	pop	{r4, pc}

Disassembly of section .text.pmu_sleep_setting_temp_based:

00000818 <pmu_sleep_setting_temp_based>:
    if (pmu_setting_state >= PMU_95C){
 818:	4b18      	ldr	r3, [pc, #96]	; (87c <pmu_sleep_setting_temp_based+0x64>)
inline static void pmu_sleep_setting_temp_based(){
 81a:	b510      	push	{r4, lr}
    if (pmu_setting_state >= PMU_95C){
 81c:	681a      	ldr	r2, [r3, #0]
 81e:	2a0c      	cmp	r2, #12
 820:	d906      	bls.n	830 <pmu_sleep_setting_temp_based+0x18>
        pmu_set_sleep_clk(0x2,0x0,0x1,0x1/*V1P2*/);
 822:	2301      	movs	r3, #1
 824:	2100      	movs	r1, #0
 826:	001a      	movs	r2, r3
 828:	2002      	movs	r0, #2
        pmu_set_sleep_clk(0xF,0x1,0x1,0x2/*V1P2*/);
 82a:	f7ff ffd1 	bl	7d0 <pmu_set_sleep_clk>
}
 82e:	e014      	b.n	85a <pmu_sleep_setting_temp_based+0x42>
    }else if (pmu_setting_state >= PMU_75C){
 830:	681a      	ldr	r2, [r3, #0]
 832:	2a0a      	cmp	r2, #10
 834:	d904      	bls.n	840 <pmu_sleep_setting_temp_based+0x28>
        pmu_set_sleep_clk(0x4,0x0,0x1,0x1/*V1P2*/);
 836:	2301      	movs	r3, #1
 838:	2100      	movs	r1, #0
 83a:	001a      	movs	r2, r3
 83c:	2004      	movs	r0, #4
 83e:	e7f4      	b.n	82a <pmu_sleep_setting_temp_based+0x12>
    }else if (pmu_setting_state >= PMU_55C){
 840:	681a      	ldr	r2, [r3, #0]
 842:	2a08      	cmp	r2, #8
 844:	d904      	bls.n	850 <pmu_sleep_setting_temp_based+0x38>
        pmu_set_sleep_clk(0x6,0x0,0x1,0x1/*V1P2*/);
 846:	2301      	movs	r3, #1
 848:	2100      	movs	r1, #0
 84a:	001a      	movs	r2, r3
 84c:	2006      	movs	r0, #6
 84e:	e7ec      	b.n	82a <pmu_sleep_setting_temp_based+0x12>
    }else if (pmu_setting_state >= PMU_35C){
 850:	681a      	ldr	r2, [r3, #0]
 852:	2a05      	cmp	r2, #5
 854:	d902      	bls.n	85c <pmu_sleep_setting_temp_based+0x44>
        pmu_set_sleep_low();
 856:	f7ff ffd7 	bl	808 <pmu_set_sleep_low>
}
 85a:	bd10      	pop	{r4, pc}
    }else if (pmu_setting_state < PMU_10C){
 85c:	681a      	ldr	r2, [r3, #0]
 85e:	2a00      	cmp	r2, #0
 860:	d104      	bne.n	86c <pmu_sleep_setting_temp_based+0x54>
        pmu_set_sleep_clk(0xF,0x2,0x1,0x4/*V1P2*/);
 862:	2304      	movs	r3, #4
 864:	2102      	movs	r1, #2
 866:	3201      	adds	r2, #1
        pmu_set_sleep_clk(0xF,0x1,0x1,0x2/*V1P2*/);
 868:	200f      	movs	r0, #15
 86a:	e7de      	b.n	82a <pmu_sleep_setting_temp_based+0x12>
    }else if (pmu_setting_state < PMU_20C){
 86c:	681b      	ldr	r3, [r3, #0]
 86e:	2b02      	cmp	r3, #2
 870:	d8f1      	bhi.n	856 <pmu_sleep_setting_temp_based+0x3e>
        pmu_set_sleep_clk(0xF,0x1,0x1,0x2/*V1P2*/);
 872:	2201      	movs	r2, #1
 874:	2302      	movs	r3, #2
 876:	0011      	movs	r1, r2
 878:	e7f6      	b.n	868 <pmu_sleep_setting_temp_based+0x50>
 87a:	46c0      	nop			; (mov r8, r8)
 87c:	00001e10 	.word	0x00001e10

Disassembly of section .text.operation_sns_sleep_check:

00000880 <operation_sns_sleep_check>:
    if (sns_running){
 880:	4b0a      	ldr	r3, [pc, #40]	; (8ac <operation_sns_sleep_check+0x2c>)
static void operation_sns_sleep_check(void){
 882:	b510      	push	{r4, lr}
    if (sns_running){
 884:	681a      	ldr	r2, [r3, #0]
 886:	2a00      	cmp	r2, #0
 888:	d005      	beq.n	896 <operation_sns_sleep_check+0x16>
        sns_running = 0;
 88a:	2200      	movs	r2, #0
 88c:	601a      	str	r2, [r3, #0]
        temp_sensor_power_off();
 88e:	f7ff fcd5 	bl	23c <temp_sensor_power_off>
        sns_ldo_power_off();
 892:	f7ff fced 	bl	270 <sns_ldo_power_off>
    if (pmu_setting_state != PMU_25C){
 896:	4b06      	ldr	r3, [pc, #24]	; (8b0 <operation_sns_sleep_check+0x30>)
 898:	681a      	ldr	r2, [r3, #0]
 89a:	2a04      	cmp	r2, #4
 89c:	d005      	beq.n	8aa <operation_sns_sleep_check+0x2a>
        pmu_setting_state = PMU_25C;
 89e:	2204      	movs	r2, #4
 8a0:	601a      	str	r2, [r3, #0]
        pmu_active_setting_temp_based();
 8a2:	f7ff ff2b 	bl	6fc <pmu_active_setting_temp_based>
        pmu_sleep_setting_temp_based();
 8a6:	f7ff ffb7 	bl	818 <pmu_sleep_setting_temp_based>
}
 8aa:	bd10      	pop	{r4, pc}
 8ac:	00001de4 	.word	0x00001de4
 8b0:	00001e10 	.word	0x00001e10

Disassembly of section .text.pmu_set_sar_override:

000008b4 <pmu_set_sar_override>:
    pmu_reg_write(0x05,
 8b4:	21a8      	movs	r1, #168	; 0xa8
 8b6:	0109      	lsls	r1, r1, #4
static void pmu_set_sar_override(uint32_t val){
 8b8:	b510      	push	{r4, lr}
    pmu_reg_write(0x05,
 8ba:	4301      	orrs	r1, r0
static void pmu_set_sar_override(uint32_t val){
 8bc:	0004      	movs	r4, r0
    pmu_reg_write(0x05,
 8be:	2005      	movs	r0, #5
 8c0:	f7ff fee2 	bl	688 <pmu_reg_write>
    pmu_reg_write(0x05,
 8c4:	21aa      	movs	r1, #170	; 0xaa
 8c6:	0189      	lsls	r1, r1, #6
 8c8:	2005      	movs	r0, #5
 8ca:	4321      	orrs	r1, r4
 8cc:	f7ff fedc 	bl	688 <pmu_reg_write>
}
 8d0:	bd10      	pop	{r4, pc}

Disassembly of section .text.pmu_adc_read_latest:

000008d4 <pmu_adc_read_latest>:
    pmu_reg_write(0x00,0x03);
 8d4:	2103      	movs	r1, #3
 8d6:	2000      	movs	r0, #0
inline static void pmu_adc_read_latest(){
 8d8:	b510      	push	{r4, lr}
    pmu_reg_write(0x00,0x03);
 8da:	f7ff fed5 	bl	688 <pmu_reg_write>
    read_data_batadc = *REG0 & 0xFF; // [CISv2] *((volatile uint32_t *) REG0) -> *REG0
 8de:	23a0      	movs	r3, #160	; 0xa0
 8e0:	061b      	lsls	r3, r3, #24
 8e2:	681a      	ldr	r2, [r3, #0]
 8e4:	4b2e      	ldr	r3, [pc, #184]	; (9a0 <pmu_adc_read_latest+0xcc>)
 8e6:	b2d2      	uxtb	r2, r2
 8e8:	601a      	str	r2, [r3, #0]
    if (read_data_batadc<PMU_ADC_3P0_VAL){
 8ea:	4a2e      	ldr	r2, [pc, #184]	; (9a4 <pmu_adc_read_latest+0xd0>)
 8ec:	681c      	ldr	r4, [r3, #0]
 8ee:	6811      	ldr	r1, [r2, #0]
 8f0:	482d      	ldr	r0, [pc, #180]	; (9a8 <pmu_adc_read_latest+0xd4>)
 8f2:	428c      	cmp	r4, r1
 8f4:	d209      	bcs.n	90a <pmu_adc_read_latest+0x36>
        read_data_batadc_diff = 0;
 8f6:	2100      	movs	r1, #0
        read_data_batadc_diff = read_data_batadc - PMU_ADC_3P0_VAL;
 8f8:	6001      	str	r1, [r0, #0]
    if (read_data_batadc < (PMU_ADC_3P0_VAL)){
 8fa:	6818      	ldr	r0, [r3, #0]
 8fc:	6811      	ldr	r1, [r2, #0]
 8fe:	4288      	cmp	r0, r1
 900:	d207      	bcs.n	912 <pmu_adc_read_latest+0x3e>
        pmu_set_sar_override(0x3C);
 902:	203c      	movs	r0, #60	; 0x3c
        pmu_set_sar_override(0x5F);
 904:	f7ff ffd6 	bl	8b4 <pmu_set_sar_override>
}
 908:	bd10      	pop	{r4, pc}
        read_data_batadc_diff = read_data_batadc - PMU_ADC_3P0_VAL;
 90a:	6819      	ldr	r1, [r3, #0]
 90c:	6814      	ldr	r4, [r2, #0]
 90e:	1b09      	subs	r1, r1, r4
 910:	e7f2      	b.n	8f8 <pmu_adc_read_latest+0x24>
    }else if (read_data_batadc < PMU_ADC_3P0_VAL + 4){
 912:	6811      	ldr	r1, [r2, #0]
 914:	6818      	ldr	r0, [r3, #0]
 916:	3104      	adds	r1, #4
 918:	4281      	cmp	r1, r0
 91a:	d901      	bls.n	920 <pmu_adc_read_latest+0x4c>
        pmu_set_sar_override(0x3F);
 91c:	203f      	movs	r0, #63	; 0x3f
 91e:	e7f1      	b.n	904 <pmu_adc_read_latest+0x30>
    }else if (read_data_batadc < PMU_ADC_3P0_VAL + 8){
 920:	6811      	ldr	r1, [r2, #0]
 922:	6818      	ldr	r0, [r3, #0]
 924:	3108      	adds	r1, #8
 926:	4281      	cmp	r1, r0
 928:	d901      	bls.n	92e <pmu_adc_read_latest+0x5a>
        pmu_set_sar_override(0x41);
 92a:	2041      	movs	r0, #65	; 0x41
 92c:	e7ea      	b.n	904 <pmu_adc_read_latest+0x30>
    }else if (read_data_batadc < PMU_ADC_3P0_VAL + 12){
 92e:	6811      	ldr	r1, [r2, #0]
 930:	6818      	ldr	r0, [r3, #0]
 932:	310c      	adds	r1, #12
 934:	4281      	cmp	r1, r0
 936:	d901      	bls.n	93c <pmu_adc_read_latest+0x68>
        pmu_set_sar_override(0x43);
 938:	2043      	movs	r0, #67	; 0x43
 93a:	e7e3      	b.n	904 <pmu_adc_read_latest+0x30>
    }else if (read_data_batadc < PMU_ADC_3P0_VAL + 17){
 93c:	6811      	ldr	r1, [r2, #0]
 93e:	6818      	ldr	r0, [r3, #0]
 940:	3111      	adds	r1, #17
 942:	4281      	cmp	r1, r0
 944:	d901      	bls.n	94a <pmu_adc_read_latest+0x76>
        pmu_set_sar_override(0x45);
 946:	2045      	movs	r0, #69	; 0x45
 948:	e7dc      	b.n	904 <pmu_adc_read_latest+0x30>
    }else if (read_data_batadc < PMU_ADC_3P0_VAL + 21){
 94a:	6811      	ldr	r1, [r2, #0]
 94c:	6818      	ldr	r0, [r3, #0]
 94e:	3115      	adds	r1, #21
 950:	4281      	cmp	r1, r0
 952:	d901      	bls.n	958 <pmu_adc_read_latest+0x84>
        pmu_set_sar_override(0x48);
 954:	2048      	movs	r0, #72	; 0x48
 956:	e7d5      	b.n	904 <pmu_adc_read_latest+0x30>
    }else if (read_data_batadc < PMU_ADC_3P0_VAL + 27){
 958:	6811      	ldr	r1, [r2, #0]
 95a:	6818      	ldr	r0, [r3, #0]
 95c:	311b      	adds	r1, #27
 95e:	4281      	cmp	r1, r0
 960:	d901      	bls.n	966 <pmu_adc_read_latest+0x92>
        pmu_set_sar_override(0x4B);
 962:	204b      	movs	r0, #75	; 0x4b
 964:	e7ce      	b.n	904 <pmu_adc_read_latest+0x30>
    }else if (read_data_batadc < PMU_ADC_3P0_VAL + 32){
 966:	6811      	ldr	r1, [r2, #0]
 968:	6818      	ldr	r0, [r3, #0]
 96a:	3120      	adds	r1, #32
 96c:	4281      	cmp	r1, r0
 96e:	d901      	bls.n	974 <pmu_adc_read_latest+0xa0>
        pmu_set_sar_override(0x4E);
 970:	204e      	movs	r0, #78	; 0x4e
 972:	e7c7      	b.n	904 <pmu_adc_read_latest+0x30>
    }else if (read_data_batadc < PMU_ADC_3P0_VAL + 39){
 974:	6811      	ldr	r1, [r2, #0]
 976:	6818      	ldr	r0, [r3, #0]
 978:	3127      	adds	r1, #39	; 0x27
 97a:	4281      	cmp	r1, r0
 97c:	d901      	bls.n	982 <pmu_adc_read_latest+0xae>
        pmu_set_sar_override(0x51);
 97e:	2051      	movs	r0, #81	; 0x51
 980:	e7c0      	b.n	904 <pmu_adc_read_latest+0x30>
    }else if (read_data_batadc < PMU_ADC_3P0_VAL + 46){
 982:	6811      	ldr	r1, [r2, #0]
 984:	6818      	ldr	r0, [r3, #0]
 986:	312e      	adds	r1, #46	; 0x2e
 988:	4281      	cmp	r1, r0
 98a:	d901      	bls.n	990 <pmu_adc_read_latest+0xbc>
        pmu_set_sar_override(0x56);
 98c:	2056      	movs	r0, #86	; 0x56
 98e:	e7b9      	b.n	904 <pmu_adc_read_latest+0x30>
    }else if (read_data_batadc < PMU_ADC_3P0_VAL + 53){
 990:	6812      	ldr	r2, [r2, #0]
 992:	6819      	ldr	r1, [r3, #0]
 994:	3235      	adds	r2, #53	; 0x35
        pmu_set_sar_override(0x5A);
 996:	205a      	movs	r0, #90	; 0x5a
    }else if (read_data_batadc < PMU_ADC_3P0_VAL + 53){
 998:	428a      	cmp	r2, r1
 99a:	d8b3      	bhi.n	904 <pmu_adc_read_latest+0x30>
        pmu_set_sar_override(0x5F);
 99c:	205f      	movs	r0, #95	; 0x5f
 99e:	e7b1      	b.n	904 <pmu_adc_read_latest+0x30>
 9a0:	00001e20 	.word	0x00001e20
 9a4:	00001e4c 	.word	0x00001e4c
 9a8:	00001dd4 	.word	0x00001dd4

Disassembly of section .text.operation_sleep_notimer:

000009ac <operation_sleep_notimer>:
    exec_count_irq = 0;
 9ac:	2200      	movs	r2, #0
 9ae:	4b0e      	ldr	r3, [pc, #56]	; (9e8 <operation_sleep_notimer+0x3c>)
static void operation_sleep_notimer(void){
 9b0:	b510      	push	{r4, lr}
    exec_count_irq = 0;
 9b2:	601a      	str	r2, [r3, #0]
    operation_sns_sleep_check();
 9b4:	f7ff ff64 	bl	880 <operation_sns_sleep_check>
    if (radio_on){ radio_power_off(); }
 9b8:	4b0c      	ldr	r3, [pc, #48]	; (9ec <operation_sleep_notimer+0x40>)
 9ba:	681b      	ldr	r3, [r3, #0]
 9bc:	2b00      	cmp	r3, #0
 9be:	d001      	beq.n	9c4 <operation_sleep_notimer+0x18>
 9c0:	f7ff fd6e 	bl	4a0 <radio_power_off>
    if (snt_timer_enabled){ snt_stop_timer(); }
 9c4:	4b0a      	ldr	r3, [pc, #40]	; (9f0 <operation_sleep_notimer+0x44>)
 9c6:	681b      	ldr	r3, [r3, #0]
 9c8:	2b00      	cmp	r3, #0
 9ca:	d001      	beq.n	9d0 <operation_sleep_notimer+0x24>
 9cc:	f7ff fc66 	bl	29c <snt_stop_timer>
    set_wakeup_timer(0, 0, 0);
 9d0:	2200      	movs	r2, #0
 9d2:	0011      	movs	r1, r2
 9d4:	0010      	movs	r0, r2
 9d6:	f7ff fb9b 	bl	110 <set_wakeup_timer>
    *GOC_DATA_IRQ = 0;
 9da:	238c      	movs	r3, #140	; 0x8c
 9dc:	2200      	movs	r2, #0
 9de:	601a      	str	r2, [r3, #0]
    mbus_sleep_all();
 9e0:	f7ff fbe8 	bl	1b4 <mbus_sleep_all>
    while(1);
 9e4:	e7fe      	b.n	9e4 <operation_sleep_notimer+0x38>
 9e6:	46c0      	nop			; (mov r8, r8)
 9e8:	00001e3c 	.word	0x00001e3c
 9ec:	00001e64 	.word	0x00001e64
 9f0:	00001e28 	.word	0x00001e28

Disassembly of section .text.handler_ext_int_reg0:

000009f4 <handler_ext_int_reg0>:
void handler_ext_int_reg0(void)  { *NVIC_ICPR = (0x1 << IRQ_REG0);  irq_pending.reg0  = 0x1; }
 9f4:	2380      	movs	r3, #128	; 0x80
 9f6:	4a04      	ldr	r2, [pc, #16]	; (a08 <handler_ext_int_reg0+0x14>)
 9f8:	005b      	lsls	r3, r3, #1
 9fa:	6013      	str	r3, [r2, #0]
 9fc:	4a03      	ldr	r2, [pc, #12]	; (a0c <handler_ext_int_reg0+0x18>)
 9fe:	6811      	ldr	r1, [r2, #0]
 a00:	430b      	orrs	r3, r1
 a02:	6013      	str	r3, [r2, #0]
 a04:	4770      	bx	lr
 a06:	46c0      	nop			; (mov r8, r8)
 a08:	e000e280 	.word	0xe000e280
 a0c:	00001d54 	.word	0x00001d54

Disassembly of section .text.handler_ext_int_reg1:

00000a10 <handler_ext_int_reg1>:
void handler_ext_int_reg1(void)  { *NVIC_ICPR = (0x1 << IRQ_REG1);  irq_pending.reg1  = 0x1; }
 a10:	2380      	movs	r3, #128	; 0x80
 a12:	4a04      	ldr	r2, [pc, #16]	; (a24 <handler_ext_int_reg1+0x14>)
 a14:	009b      	lsls	r3, r3, #2
 a16:	6013      	str	r3, [r2, #0]
 a18:	4a03      	ldr	r2, [pc, #12]	; (a28 <handler_ext_int_reg1+0x18>)
 a1a:	6811      	ldr	r1, [r2, #0]
 a1c:	430b      	orrs	r3, r1
 a1e:	6013      	str	r3, [r2, #0]
 a20:	4770      	bx	lr
 a22:	46c0      	nop			; (mov r8, r8)
 a24:	e000e280 	.word	0xe000e280
 a28:	00001d54 	.word	0x00001d54

Disassembly of section .text.handler_ext_int_reg2:

00000a2c <handler_ext_int_reg2>:
void handler_ext_int_reg2(void)  { *NVIC_ICPR = (0x1 << IRQ_REG2);  irq_pending.reg2  = 0x1; }
 a2c:	2380      	movs	r3, #128	; 0x80
 a2e:	4a04      	ldr	r2, [pc, #16]	; (a40 <handler_ext_int_reg2+0x14>)
 a30:	00db      	lsls	r3, r3, #3
 a32:	6013      	str	r3, [r2, #0]
 a34:	4a03      	ldr	r2, [pc, #12]	; (a44 <handler_ext_int_reg2+0x18>)
 a36:	6811      	ldr	r1, [r2, #0]
 a38:	430b      	orrs	r3, r1
 a3a:	6013      	str	r3, [r2, #0]
 a3c:	4770      	bx	lr
 a3e:	46c0      	nop			; (mov r8, r8)
 a40:	e000e280 	.word	0xe000e280
 a44:	00001d54 	.word	0x00001d54

Disassembly of section .text.handler_ext_int_reg3:

00000a48 <handler_ext_int_reg3>:
void handler_ext_int_reg3(void)  { *NVIC_ICPR = (0x1 << IRQ_REG3);  irq_pending.reg3  = 0x1; }
 a48:	2380      	movs	r3, #128	; 0x80
 a4a:	4a04      	ldr	r2, [pc, #16]	; (a5c <handler_ext_int_reg3+0x14>)
 a4c:	011b      	lsls	r3, r3, #4
 a4e:	6013      	str	r3, [r2, #0]
 a50:	4a03      	ldr	r2, [pc, #12]	; (a60 <handler_ext_int_reg3+0x18>)
 a52:	6811      	ldr	r1, [r2, #0]
 a54:	430b      	orrs	r3, r1
 a56:	6013      	str	r3, [r2, #0]
 a58:	4770      	bx	lr
 a5a:	46c0      	nop			; (mov r8, r8)
 a5c:	e000e280 	.word	0xe000e280
 a60:	00001d54 	.word	0x00001d54

Disassembly of section .text.handler_ext_int_gocep:

00000a64 <handler_ext_int_gocep>:
void handler_ext_int_gocep(void) { *NVIC_ICPR = (0x1 << IRQ_GOCEP); irq_pending.gocep = 0x1; }
 a64:	2304      	movs	r3, #4
 a66:	4a03      	ldr	r2, [pc, #12]	; (a74 <handler_ext_int_gocep+0x10>)
 a68:	6013      	str	r3, [r2, #0]
 a6a:	4a03      	ldr	r2, [pc, #12]	; (a78 <handler_ext_int_gocep+0x14>)
 a6c:	6811      	ldr	r1, [r2, #0]
 a6e:	430b      	orrs	r3, r1
 a70:	6013      	str	r3, [r2, #0]
 a72:	4770      	bx	lr
 a74:	e000e280 	.word	0xe000e280
 a78:	00001d54 	.word	0x00001d54

Disassembly of section .text.handler_ext_int_timer32:

00000a7c <handler_ext_int_timer32>:
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
 a7c:	2308      	movs	r3, #8
 a7e:	4a03      	ldr	r2, [pc, #12]	; (a8c <handler_ext_int_timer32+0x10>)
 a80:	6013      	str	r3, [r2, #0]
    irq_pending.timer32 = 0x1;
 a82:	4a03      	ldr	r2, [pc, #12]	; (a90 <handler_ext_int_timer32+0x14>)
 a84:	6811      	ldr	r1, [r2, #0]
 a86:	430b      	orrs	r3, r1
 a88:	6013      	str	r3, [r2, #0]
    }
 a8a:	4770      	bx	lr
 a8c:	e000e280 	.word	0xe000e280
 a90:	00001d54 	.word	0x00001d54

Disassembly of section .text.handler_ext_int_wakeup:

00000a94 <handler_ext_int_wakeup>:
void handler_ext_int_wakeup(void) { *NVIC_ICPR = (0x1 << IRQ_WAKEUP); irq_pending.wakeup = 1;
 a94:	2301      	movs	r3, #1
 a96:	4a03      	ldr	r2, [pc, #12]	; (aa4 <handler_ext_int_wakeup+0x10>)
 a98:	6013      	str	r3, [r2, #0]
 a9a:	4a03      	ldr	r2, [pc, #12]	; (aa8 <handler_ext_int_wakeup+0x14>)
 a9c:	6811      	ldr	r1, [r2, #0]
 a9e:	430b      	orrs	r3, r1
 aa0:	6013      	str	r3, [r2, #0]
}
 aa2:	4770      	bx	lr
 aa4:	e000e280 	.word	0xe000e280
 aa8:	00001d54 	.word	0x00001d54

Disassembly of section .text.crcEnc16:

00000aac <crcEnc16>:
uint32_t* crcEnc16(uint32_t data2, uint32_t data1, uint32_t data0) {
 aac:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    data1 = (data1 << CRC_LEN) + (data0 >> CRC_LEN);
 aae:	0c13      	lsrs	r3, r2, #16
    data2 = (data2 << CRC_LEN) + (data1 >> CRC_LEN);
 ab0:	0c0e      	lsrs	r6, r1, #16
    data1 = (data1 << CRC_LEN) + (data0 >> CRC_LEN);
 ab2:	0409      	lsls	r1, r1, #16
 ab4:	1859      	adds	r1, r3, r1
    data0 = data0 << CRC_LEN;
 ab6:	0413      	lsls	r3, r2, #16
 ab8:	9300      	str	r3, [sp, #0]
    uint16_t remainder = 0x0000;
 aba:	2300      	movs	r3, #0
                         + (input_bit^(remainder > 0x7fff));
 abc:	4a16      	ldr	r2, [pc, #88]	; (b18 <crcEnc16+0x6c>)
    for (i = 0; i < DATA_LEN; i++)
 abe:	001d      	movs	r5, r3
                         + (input_bit^(remainder > 0x7fff));
 ac0:	4694      	mov	ip, r2
    data2 = (data2 << CRC_LEN) + (data1 >> CRC_LEN);
 ac2:	0400      	lsls	r0, r0, #16
 ac4:	1836      	adds	r6, r6, r0
        if (remainder > 0x7fff)
 ac6:	0bd8      	lsrs	r0, r3, #15
 ac8:	4247      	negs	r7, r0
 aca:	b2bf      	uxth	r7, r7
        if      (i < 32) input_bit = ((data2 << (i   )) > 0x7fffffff);
 acc:	2d1f      	cmp	r5, #31
 ace:	d818      	bhi.n	b02 <crcEnc16+0x56>
 ad0:	0034      	movs	r4, r6
 ad2:	40ac      	lsls	r4, r5
        remainder_shift = remainder << 1;
 ad4:	005b      	lsls	r3, r3, #1
 ad6:	b29a      	uxth	r2, r3
 ad8:	9201      	str	r2, [sp, #4]
                         + (input_bit^(remainder > 0x7fff));
 ada:	4662      	mov	r2, ip
 adc:	4013      	ands	r3, r2
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift))
 ade:	9a01      	ldr	r2, [sp, #4]
        else             input_bit = ((data0 << (i-64)) > 0x7fffffff);
 ae0:	0fe4      	lsrs	r4, r4, #31
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift))
 ae2:	4057      	eors	r7, r2
 ae4:	4a0d      	ldr	r2, [pc, #52]	; (b1c <crcEnc16+0x70>)
                         + (input_bit^(remainder > 0x7fff));
 ae6:	4044      	eors	r4, r0
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift))
 ae8:	4017      	ands	r7, r2
 aea:	191b      	adds	r3, r3, r4
    for (i = 0; i < DATA_LEN; i++)
 aec:	3501      	adds	r5, #1
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift))
 aee:	433b      	orrs	r3, r7
    for (i = 0; i < DATA_LEN; i++)
 af0:	2d60      	cmp	r5, #96	; 0x60
 af2:	d1e8      	bne.n	ac6 <crcEnc16+0x1a>
    data0 = data0 + remainder;
 af4:	9a00      	ldr	r2, [sp, #0]
    msg_out[0] = data2;
 af6:	480a      	ldr	r0, [pc, #40]	; (b20 <crcEnc16+0x74>)
    data0 = data0 + remainder;
 af8:	189b      	adds	r3, r3, r2
    msg_out[0] = data2;
 afa:	6006      	str	r6, [r0, #0]
    msg_out[1] = data1;
 afc:	6041      	str	r1, [r0, #4]
    msg_out[2] = data0;
 afe:	6083      	str	r3, [r0, #8]
}
 b00:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
        else if (i < 64) input_bit = ((data1 << (i-32)) > 0x7fffffff);
 b02:	002c      	movs	r4, r5
 b04:	2d3f      	cmp	r5, #63	; 0x3f
 b06:	d804      	bhi.n	b12 <crcEnc16+0x66>
 b08:	000a      	movs	r2, r1
 b0a:	3c20      	subs	r4, #32
        else             input_bit = ((data0 << (i-64)) > 0x7fffffff);
 b0c:	40a2      	lsls	r2, r4
 b0e:	0014      	movs	r4, r2
 b10:	e7e0      	b.n	ad4 <crcEnc16+0x28>
 b12:	9a00      	ldr	r2, [sp, #0]
 b14:	3c40      	subs	r4, #64	; 0x40
 b16:	e7f9      	b.n	b0c <crcEnc16+0x60>
 b18:	00003ffd 	.word	0x00003ffd
 b1c:	ffffc002 	.word	0xffffc002
 b20:	00001e74 	.word	0x00001e74

Disassembly of section .text.send_radio_data_mrr:

00000b24 <send_radio_data_mrr>:
static void send_radio_data_mrr(uint32_t last_packet, uint8_t radio_packet_prefix, uint32_t radio_data){
 b24:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    output_data = crcEnc16(((read_data_batadc & 0xFF)<<8) | ((radio_packet_prefix & 0xF)<<4) | ((radio_packet_count>>16)&0xF), ((radio_packet_count & 0xFFFF)<<16) | (*REG_CHIP_ID & 0xFFFF), radio_data);
 b26:	4d6f      	ldr	r5, [pc, #444]	; (ce4 <send_radio_data_mrr+0x1c0>)
 b28:	4e6f      	ldr	r6, [pc, #444]	; (ce8 <send_radio_data_mrr+0x1c4>)
static void send_radio_data_mrr(uint32_t last_packet, uint8_t radio_packet_prefix, uint32_t radio_data){
 b2a:	9100      	str	r1, [sp, #0]
 b2c:	9001      	str	r0, [sp, #4]
    output_data = crcEnc16(((read_data_batadc & 0xFF)<<8) | ((radio_packet_prefix & 0xF)<<4) | ((radio_packet_count>>16)&0xF), ((radio_packet_count & 0xFFFF)<<16) | (*REG_CHIP_ID & 0xFFFF), radio_data);
 b2e:	6830      	ldr	r0, [r6, #0]
 b30:	682b      	ldr	r3, [r5, #0]
 b32:	6829      	ldr	r1, [r5, #0]
 b34:	4f6d      	ldr	r7, [pc, #436]	; (cec <send_radio_data_mrr+0x1c8>)
 b36:	0409      	lsls	r1, r1, #16
 b38:	468c      	mov	ip, r1
static void send_radio_data_mrr(uint32_t last_packet, uint8_t radio_packet_prefix, uint32_t radio_data){
 b3a:	0014      	movs	r4, r2
    output_data = crcEnc16(((read_data_batadc & 0xFF)<<8) | ((radio_packet_prefix & 0xF)<<4) | ((radio_packet_count>>16)&0xF), ((radio_packet_count & 0xFFFF)<<16) | (*REG_CHIP_ID & 0xFFFF), radio_data);
 b3c:	683a      	ldr	r2, [r7, #0]
 b3e:	0600      	lsls	r0, r0, #24
 b40:	b291      	uxth	r1, r2
 b42:	4662      	mov	r2, ip
 b44:	031b      	lsls	r3, r3, #12
 b46:	0f1b      	lsrs	r3, r3, #28
 b48:	0c00      	lsrs	r0, r0, #16
 b4a:	4318      	orrs	r0, r3
 b4c:	9b00      	ldr	r3, [sp, #0]
 b4e:	4311      	orrs	r1, r2
 b50:	011b      	lsls	r3, r3, #4
 b52:	0022      	movs	r2, r4
 b54:	4318      	orrs	r0, r3
 b56:	f7ff ffa9 	bl	aac <crcEnc16>
    uint32_t tx_data_2 = (radio_packet_prefix<<20)|(radio_packet_count&0xFFFFF);
 b5a:	9b00      	ldr	r3, [sp, #0]
    uint32_t tx_data_1 = (*REG_CHIP_ID<<8)|(radio_data>>24);
 b5c:	683f      	ldr	r7, [r7, #0]
    uint32_t tx_data_2 = (radio_packet_prefix<<20)|(radio_packet_count&0xFFFFF);
 b5e:	682d      	ldr	r5, [r5, #0]
 b60:	0519      	lsls	r1, r3, #20
    uint32_t tx_data_3 = ((output_data[2] & 0xFFFF)/*CRC16*/<<8)|(read_data_batadc&0xFF);
 b62:	6836      	ldr	r6, [r6, #0]
 b64:	6883      	ldr	r3, [r0, #8]
    uint32_t tx_data_2 = (radio_packet_prefix<<20)|(radio_packet_count&0xFFFFF);
 b66:	032d      	lsls	r5, r5, #12
 b68:	0b2d      	lsrs	r5, r5, #12
    uint32_t tx_data_3 = ((output_data[2] & 0xFFFF)/*CRC16*/<<8)|(read_data_batadc&0xFF);
 b6a:	021b      	lsls	r3, r3, #8
 b6c:	b2f6      	uxtb	r6, r6
    uint32_t tx_data_1 = (*REG_CHIP_ID<<8)|(radio_data>>24);
 b6e:	0e22      	lsrs	r2, r4, #24
    uint32_t tx_data_2 = (radio_packet_prefix<<20)|(radio_packet_count&0xFFFFF);
 b70:	4329      	orrs	r1, r5
    uint32_t tx_data_3 = ((output_data[2] & 0xFFFF)/*CRC16*/<<8)|(read_data_batadc&0xFF);
 b72:	4333      	orrs	r3, r6
    uint32_t tx_data_0 = radio_data & 0xFFFFFF;
 b74:	0224      	lsls	r4, r4, #8
    mrr_tx_data[0] = ((tx_data_1 << 24) & 0xFF000000) | ( tx_data_0        & 0x00FFFFFF);
 b76:	485e      	ldr	r0, [pc, #376]	; (cf0 <send_radio_data_mrr+0x1cc>)
    mrr_tx_data[2] = ((tx_data_3 <<  8) & 0xFFFFFF00) | ((tx_data_2 >> 16) & 0x000000FF);
 b78:	021b      	lsls	r3, r3, #8
    mrr_tx_data[0] = ((tx_data_1 << 24) & 0xFF000000) | ( tx_data_0        & 0x00FFFFFF);
 b7a:	0612      	lsls	r2, r2, #24
    uint32_t tx_data_0 = radio_data & 0xFFFFFF;
 b7c:	0a24      	lsrs	r4, r4, #8
    mrr_tx_data[1] = ((tx_data_2 << 16) & 0xFFFF0000) | ((tx_data_1 >>  8) & 0x0000FFFF);
 b7e:	b2bf      	uxth	r7, r7
 b80:	042d      	lsls	r5, r5, #16
    mrr_tx_data[2] = ((tx_data_3 <<  8) & 0xFFFFFF00) | ((tx_data_2 >> 16) & 0x000000FF);
 b82:	0c09      	lsrs	r1, r1, #16
 b84:	4319      	orrs	r1, r3
    mrr_tx_data[0] = ((tx_data_1 << 24) & 0xFF000000) | ( tx_data_0        & 0x00FFFFFF);
 b86:	4322      	orrs	r2, r4
    mrr_tx_data[1] = ((tx_data_2 << 16) & 0xFFFF0000) | ((tx_data_1 >>  8) & 0x0000FFFF);
 b88:	433d      	orrs	r5, r7
    mrr_tx_data[0] = ((tx_data_1 << 24) & 0xFF000000) | ( tx_data_0        & 0x00FFFFFF);
 b8a:	6002      	str	r2, [r0, #0]
    mrr_tx_data[2] = ((tx_data_3 <<  8) & 0xFFFFFF00) | ((tx_data_2 >> 16) & 0x000000FF);
 b8c:	6081      	str	r1, [r0, #8]
    mbus_copy_mem_from_local_to_remote_bulk(MRR_ADDR, (uint32_t *) 0x00000000, (uint32_t *) mrr_tx_data, RADIO_DATA_NUM_WORDS-1);
 b8e:	0002      	movs	r2, r0
 b90:	2302      	movs	r3, #2
    mrr_tx_data[1] = ((tx_data_2 << 16) & 0xFFFF0000) | ((tx_data_1 >>  8) & 0x0000FFFF);
 b92:	6045      	str	r5, [r0, #4]
    mbus_copy_mem_from_local_to_remote_bulk(MRR_ADDR, (uint32_t *) 0x00000000, (uint32_t *) mrr_tx_data, RADIO_DATA_NUM_WORDS-1);
 b94:	2100      	movs	r1, #0
 b96:	2004      	movs	r0, #4
 b98:	f7ff fb36 	bl	208 <mbus_copy_mem_from_local_to_remote_bulk>
    if (!radio_ready){
 b9c:	4b55      	ldr	r3, [pc, #340]	; (cf4 <send_radio_data_mrr+0x1d0>)
 b9e:	681a      	ldr	r2, [r3, #0]
 ba0:	2a00      	cmp	r2, #0
 ba2:	d10f      	bne.n	bc4 <send_radio_data_mrr+0xa0>
        radio_ready = 1;
 ba4:	3201      	adds	r2, #1
 ba6:	601a      	str	r2, [r3, #0]
        mrrv11a_r03.TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
 ba8:	2280      	movs	r2, #128	; 0x80
 baa:	4b53      	ldr	r3, [pc, #332]	; (cf8 <send_radio_data_mrr+0x1d4>)
 bac:	03d2      	lsls	r2, r2, #15
 bae:	6819      	ldr	r1, [r3, #0]
        mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 bb0:	2004      	movs	r0, #4
        mrrv11a_r03.TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
 bb2:	430a      	orrs	r2, r1
 bb4:	601a      	str	r2, [r3, #0]
        mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
 bb6:	2103      	movs	r1, #3
 bb8:	681a      	ldr	r2, [r3, #0]
 bba:	f7ff fb12 	bl	1e2 <mbus_remote_register_write>
        delay(MBUS_DELAY);
 bbe:	20c8      	movs	r0, #200	; 0xc8
 bc0:	f7ff fa72 	bl	a8 <delay>
    mrrv11a_r00.TRX_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
 bc4:	217e      	movs	r1, #126	; 0x7e
 bc6:	4b4d      	ldr	r3, [pc, #308]	; (cfc <send_radio_data_mrr+0x1d8>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 bc8:	2004      	movs	r0, #4
    mrrv11a_r00.TRX_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
 bca:	681a      	ldr	r2, [r3, #0]
 bcc:	438a      	bics	r2, r1
 bce:	0011      	movs	r1, r2
 bd0:	2202      	movs	r2, #2
 bd2:	430a      	orrs	r2, r1
 bd4:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 bd6:	681a      	ldr	r2, [r3, #0]
 bd8:	2100      	movs	r1, #0
 bda:	f7ff fb02 	bl	1e2 <mbus_remote_register_write>
    uint32_t num_packets = 1;
 bde:	2101      	movs	r1, #1
    if (mrr_freq_hopping) num_packets = mrr_freq_hopping;
 be0:	4b47      	ldr	r3, [pc, #284]	; (d00 <send_radio_data_mrr+0x1dc>)
    uint32_t num_packets = 1;
 be2:	9100      	str	r1, [sp, #0]
    if (mrr_freq_hopping) num_packets = mrr_freq_hopping;
 be4:	681a      	ldr	r2, [r3, #0]
 be6:	2a00      	cmp	r2, #0
 be8:	d001      	beq.n	bee <send_radio_data_mrr+0xca>
 bea:	681b      	ldr	r3, [r3, #0]
 bec:	9300      	str	r3, [sp, #0]
    uint32_t count = 0;
 bee:	2500      	movs	r5, #0
    mrr_cfo_val_fine = mrr_cfo_val_fine_min;
 bf0:	4b44      	ldr	r3, [pc, #272]	; (d04 <send_radio_data_mrr+0x1e0>)
 bf2:	681e      	ldr	r6, [r3, #0]
    while (count < num_packets){
 bf4:	9b00      	ldr	r3, [sp, #0]
 bf6:	429d      	cmp	r5, r3
 bf8:	d109      	bne.n	c0e <send_radio_data_mrr+0xea>
    radio_packet_count++;
 bfa:	4a3a      	ldr	r2, [pc, #232]	; (ce4 <send_radio_data_mrr+0x1c0>)
 bfc:	6813      	ldr	r3, [r2, #0]
 bfe:	3301      	adds	r3, #1
 c00:	6013      	str	r3, [r2, #0]
    if (last_packet){
 c02:	9b01      	ldr	r3, [sp, #4]
 c04:	2b00      	cmp	r3, #0
 c06:	d001      	beq.n	c0c <send_radio_data_mrr+0xe8>
        radio_power_off();
 c08:	f7ff fc4a 	bl	4a0 <radio_power_off>
}
 c0c:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
        mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
 c0e:	203f      	movs	r0, #63	; 0x3f
 c10:	4b3d      	ldr	r3, [pc, #244]	; (d08 <send_radio_data_mrr+0x1e4>)
 c12:	4c3e      	ldr	r4, [pc, #248]	; (d0c <send_radio_data_mrr+0x1e8>)
 c14:	6819      	ldr	r1, [r3, #0]
 c16:	4030      	ands	r0, r6
 c18:	0402      	lsls	r2, r0, #16
 c1a:	4021      	ands	r1, r4
 c1c:	4311      	orrs	r1, r2
 c1e:	6019      	str	r1, [r3, #0]
        mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine;
 c20:	681a      	ldr	r2, [r3, #0]
 c22:	493b      	ldr	r1, [pc, #236]	; (d10 <send_radio_data_mrr+0x1ec>)
 c24:	0280      	lsls	r0, r0, #10
 c26:	400a      	ands	r2, r1
 c28:	4302      	orrs	r2, r0
 c2a:	601a      	str	r2, [r3, #0]
        mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
 c2c:	681a      	ldr	r2, [r3, #0]
 c2e:	2101      	movs	r1, #1
 c30:	2004      	movs	r0, #4
 c32:	f7ff fad6 	bl	1e2 <mbus_remote_register_write>
    set_timer32_timeout(TIMER32_VAL);
 c36:	20a0      	movs	r0, #160	; 0xa0
 c38:	02c0      	lsls	r0, r0, #11
 c3a:	f7ff fcb7 	bl	5ac <set_timer32_timeout>
    mrrv11a_r00.TRX_CL_EN = 1;
 c3e:	2301      	movs	r3, #1
 c40:	4c2e      	ldr	r4, [pc, #184]	; (cfc <send_radio_data_mrr+0x1d8>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 c42:	2100      	movs	r1, #0
    mrrv11a_r00.TRX_CL_EN = 1;
 c44:	6822      	ldr	r2, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 c46:	2004      	movs	r0, #4
    mrrv11a_r00.TRX_CL_EN = 1;
 c48:	4313      	orrs	r3, r2
 c4a:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 c4c:	6822      	ldr	r2, [r4, #0]
 c4e:	f7ff fac8 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r11.FSM_RESET_B = 1;    
 c52:	2302      	movs	r3, #2
 c54:	4f2f      	ldr	r7, [pc, #188]	; (d14 <send_radio_data_mrr+0x1f0>)
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 c56:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_RESET_B = 1;    
 c58:	683a      	ldr	r2, [r7, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 c5a:	2004      	movs	r0, #4
    mrrv11a_r11.FSM_RESET_B = 1;    
 c5c:	4313      	orrs	r3, r2
 c5e:	603b      	str	r3, [r7, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 c60:	683a      	ldr	r2, [r7, #0]
 c62:	f7ff fabe 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r11.FSM_EN = 1;  //Start BB
 c66:	2004      	movs	r0, #4
 c68:	683b      	ldr	r3, [r7, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 c6a:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_EN = 1;  //Start BB
 c6c:	4303      	orrs	r3, r0
 c6e:	603b      	str	r3, [r7, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 c70:	683a      	ldr	r2, [r7, #0]
 c72:	f7ff fab6 	bl	1e2 <mbus_remote_register_write>
    while ((!irq_pending.reg2) & (!irq_pending.timer32));
 c76:	4a28      	ldr	r2, [pc, #160]	; (d18 <send_radio_data_mrr+0x1f4>)
 c78:	6811      	ldr	r1, [r2, #0]
 c7a:	6813      	ldr	r3, [r2, #0]
 c7c:	0549      	lsls	r1, r1, #21
 c7e:	071b      	lsls	r3, r3, #28
 c80:	430b      	orrs	r3, r1
 c82:	d5f9      	bpl.n	c78 <send_radio_data_mrr+0x154>
    irq_pending.reg2 = 0;
 c84:	6813      	ldr	r3, [r2, #0]
 c86:	4925      	ldr	r1, [pc, #148]	; (d1c <send_radio_data_mrr+0x1f8>)
    stop_timer32_timeout_check(code);
 c88:	2003      	movs	r0, #3
    irq_pending.reg2 = 0;
 c8a:	400b      	ands	r3, r1
 c8c:	6013      	str	r3, [r2, #0]
    stop_timer32_timeout_check(code);
 c8e:	f7ff fc99 	bl	5c4 <stop_timer32_timeout_check>
    mrrv11a_r00.TRX_CL_EN = 0;
 c92:	2201      	movs	r2, #1
    mrrv11a_r11.FSM_EN = 0;
 c94:	2704      	movs	r7, #4
    mrrv11a_r00.TRX_CL_EN = 0;
 c96:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 c98:	2100      	movs	r1, #0
    mrrv11a_r00.TRX_CL_EN = 0;
 c9a:	4393      	bics	r3, r2
 c9c:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
 c9e:	6822      	ldr	r2, [r4, #0]
 ca0:	2004      	movs	r0, #4
 ca2:	f7ff fa9e 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r11.FSM_EN = 0;
 ca6:	4c1b      	ldr	r4, [pc, #108]	; (d14 <send_radio_data_mrr+0x1f0>)
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 ca8:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_EN = 0;
 caa:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 cac:	0038      	movs	r0, r7
    mrrv11a_r11.FSM_EN = 0;
 cae:	43bb      	bics	r3, r7
 cb0:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 cb2:	6822      	ldr	r2, [r4, #0]
 cb4:	f7ff fa95 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r11.FSM_RESET_B = 0;
 cb8:	2202      	movs	r2, #2
 cba:	6823      	ldr	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 cbc:	2111      	movs	r1, #17
    mrrv11a_r11.FSM_RESET_B = 0;
 cbe:	4393      	bics	r3, r2
 cc0:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
 cc2:	0038      	movs	r0, r7
 cc4:	6822      	ldr	r2, [r4, #0]
 cc6:	f7ff fa8c 	bl	1e2 <mbus_remote_register_write>
        if (count < num_packets){
 cca:	9b00      	ldr	r3, [sp, #0]
        count++;
 ccc:	3501      	adds	r5, #1
        if (count < num_packets){
 cce:	42ab      	cmp	r3, r5
 cd0:	d903      	bls.n	cda <send_radio_data_mrr+0x1b6>
            delay(RADIO_PACKET_DELAY);
 cd2:	4b13      	ldr	r3, [pc, #76]	; (d20 <send_radio_data_mrr+0x1fc>)
 cd4:	6818      	ldr	r0, [r3, #0]
 cd6:	f7ff f9e7 	bl	a8 <delay>
        mrr_cfo_val_fine = mrr_cfo_val_fine + mrr_freq_hopping_step; // 1: 0.8MHz, 2: 1.6MHz step
 cda:	4b12      	ldr	r3, [pc, #72]	; (d24 <send_radio_data_mrr+0x200>)
 cdc:	681b      	ldr	r3, [r3, #0]
 cde:	18f6      	adds	r6, r6, r3
 ce0:	e788      	b.n	bf4 <send_radio_data_mrr+0xd0>
 ce2:	46c0      	nop			; (mov r8, r8)
 ce4:	00001e54 	.word	0x00001e54
 ce8:	00001e20 	.word	0x00001e20
 cec:	a00000fc 	.word	0xa00000fc
 cf0:	00001dc8 	.word	0x00001dc8
 cf4:	00001dc0 	.word	0x00001dc0
 cf8:	00001d60 	.word	0x00001d60
 cfc:	00001d58 	.word	0x00001d58
 d00:	00001ddc 	.word	0x00001ddc
 d04:	00001db8 	.word	0x00001db8
 d08:	00001e68 	.word	0x00001e68
 d0c:	ffc0ffff 	.word	0xffc0ffff
 d10:	ffff03ff 	.word	0xffff03ff
 d14:	00001d70 	.word	0x00001d70
 d18:	00001d54 	.word	0x00001d54
 d1c:	fffffbff 	.word	0xfffffbff
 d20:	00001dbc 	.word	0x00001dbc
 d24:	00001e58 	.word	0x00001e58

Disassembly of section .text.operation_goc_trigger_radio:

00000d28 <operation_goc_trigger_radio>:
    stack_state = STK_IDLE;
    
    radio_power_off();
}

static void operation_goc_trigger_radio(uint32_t radio_tx_num, uint32_t wakeup_timer_val, uint8_t radio_tx_prefix, uint32_t radio_tx_data){
 d28:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
 d2a:	0016      	movs	r6, r2
 d2c:	001f      	movs	r7, r3
 d2e:	000d      	movs	r5, r1
 d30:	9001      	str	r0, [sp, #4]

    // Prepare radio TX
    radio_power_on();
 d32:	f7ff faf1 	bl	318 <radio_power_on>
    exec_count_irq++;
 d36:	4c0c      	ldr	r4, [pc, #48]	; (d68 <operation_goc_trigger_radio+0x40>)
    // radio
    send_radio_data_mrr(1,radio_tx_prefix,radio_tx_data);   
 d38:	003a      	movs	r2, r7
    exec_count_irq++;
 d3a:	6823      	ldr	r3, [r4, #0]
    send_radio_data_mrr(1,radio_tx_prefix,radio_tx_data);   
 d3c:	0031      	movs	r1, r6
    exec_count_irq++;
 d3e:	3301      	adds	r3, #1
    send_radio_data_mrr(1,radio_tx_prefix,radio_tx_data);   
 d40:	2001      	movs	r0, #1
    exec_count_irq++;
 d42:	6023      	str	r3, [r4, #0]
    send_radio_data_mrr(1,radio_tx_prefix,radio_tx_data);   
 d44:	f7ff feee 	bl	b24 <send_radio_data_mrr>

    if (exec_count_irq < radio_tx_num){
 d48:	6823      	ldr	r3, [r4, #0]
 d4a:	9a01      	ldr	r2, [sp, #4]
 d4c:	4293      	cmp	r3, r2
 d4e:	d207      	bcs.n	d60 <operation_goc_trigger_radio+0x38>
        // set timer
        set_wakeup_timer (wakeup_timer_val, 0x1, 0x1);
 d50:	2201      	movs	r2, #1
 d52:	0028      	movs	r0, r5
 d54:	0011      	movs	r1, r2
 d56:	f7ff f9db 	bl	110 <set_wakeup_timer>
    mbus_sleep_all();
 d5a:	f7ff fa2b 	bl	1b4 <mbus_sleep_all>
    while(1);
 d5e:	e7fe      	b.n	d5e <operation_goc_trigger_radio+0x36>
        // go to sleep and wake up with same condition
        operation_sleep_noirqreset();
        
    }else{
        exec_count_irq = 0;
 d60:	2300      	movs	r3, #0
 d62:	6023      	str	r3, [r4, #0]
        // Go to sleep without timer
        operation_sleep_notimer();
 d64:	f7ff fe22 	bl	9ac <operation_sleep_notimer>
 d68:	00001e3c 	.word	0x00001e3c

Disassembly of section .text.operation_sns_run:

00000d6c <operation_sns_run>:
static void operation_sns_run(void){
     d6c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    if (stack_state == STK_IDLE){
     d6e:	4ebc      	ldr	r6, [pc, #752]	; (1060 <operation_sns_run+0x2f4>)
     d70:	6831      	ldr	r1, [r6, #0]
     d72:	2900      	cmp	r1, #0
     d74:	d10d      	bne.n	d92 <operation_sns_run+0x26>
        stack_state = STK_LDO;
     d76:	2301      	movs	r3, #1
    sntv5_r00.LDO_EN_VREF    = 1;
     d78:	2204      	movs	r2, #4
        stack_state = STK_LDO;
     d7a:	6033      	str	r3, [r6, #0]
        wfi_timeout_flag = 0;
     d7c:	4bb9      	ldr	r3, [pc, #740]	; (1064 <operation_sns_run+0x2f8>)
     d7e:	6019      	str	r1, [r3, #0]
    sntv5_r00.LDO_EN_VREF    = 1;
     d80:	4bb9      	ldr	r3, [pc, #740]	; (1068 <operation_sns_run+0x2fc>)
     d82:	8818      	ldrh	r0, [r3, #0]
     d84:	4302      	orrs	r2, r0
     d86:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x00,sntv5_r00.as_int);
     d88:	2005      	movs	r0, #5
     d8a:	681a      	ldr	r2, [r3, #0]
     d8c:	f7ff fa29 	bl	1e2 <mbus_remote_register_write>
}
     d90:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    }else if (stack_state == STK_LDO){
     d92:	6835      	ldr	r5, [r6, #0]
     d94:	2d01      	cmp	r5, #1
     d96:	d12f      	bne.n	df8 <operation_sns_run+0x8c>
        stack_state = STK_TEMP_START;
     d98:	2702      	movs	r7, #2
    sntv5_r00.LDO_EN_IREF    = 1;
     d9a:	4bb3      	ldr	r3, [pc, #716]	; (1068 <operation_sns_run+0x2fc>)
        stack_state = STK_TEMP_START;
     d9c:	6037      	str	r7, [r6, #0]
    sntv5_r00.LDO_EN_IREF    = 1;
     d9e:	881a      	ldrh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x00,sntv5_r00.as_int);
     da0:	2100      	movs	r1, #0
    sntv5_r00.LDO_EN_IREF    = 1;
     da2:	433a      	orrs	r2, r7
     da4:	801a      	strh	r2, [r3, #0]
    sntv5_r00.LDO_EN_LDO    = 1;
     da6:	881a      	ldrh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x00,sntv5_r00.as_int);
     da8:	2005      	movs	r0, #5
    sntv5_r00.LDO_EN_LDO    = 1;
     daa:	432a      	orrs	r2, r5
     dac:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x00,sntv5_r00.as_int);
     dae:	681a      	ldr	r2, [r3, #0]
     db0:	f7ff fa17 	bl	1e2 <mbus_remote_register_write>
    sntv5_r01.TSNS_SEL_LDO = 1;
     db4:	2308      	movs	r3, #8
     db6:	4cad      	ldr	r4, [pc, #692]	; (106c <operation_sns_run+0x300>)
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
     db8:	0029      	movs	r1, r5
    sntv5_r01.TSNS_SEL_LDO = 1;
     dba:	8822      	ldrh	r2, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
     dbc:	2005      	movs	r0, #5
    sntv5_r01.TSNS_SEL_LDO = 1;
     dbe:	4313      	orrs	r3, r2
     dc0:	8023      	strh	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
     dc2:	6822      	ldr	r2, [r4, #0]
     dc4:	f7ff fa0d 	bl	1e2 <mbus_remote_register_write>
    sntv5_r01.TSNS_EN_SENSOR_LDO = 1;
     dc8:	2320      	movs	r3, #32
     dca:	8822      	ldrh	r2, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
     dcc:	0029      	movs	r1, r5
    sntv5_r01.TSNS_EN_SENSOR_LDO = 1;
     dce:	4313      	orrs	r3, r2
     dd0:	8023      	strh	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
     dd2:	6822      	ldr	r2, [r4, #0]
     dd4:	2005      	movs	r0, #5
     dd6:	f7ff fa04 	bl	1e2 <mbus_remote_register_write>
    delay(MBUS_DELAY);
     dda:	20c8      	movs	r0, #200	; 0xc8
     ddc:	f7ff f964 	bl	a8 <delay>
    sntv5_r01.TSNS_ISOLATE = 0;
     de0:	8823      	ldrh	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
     de2:	0029      	movs	r1, r5
    sntv5_r01.TSNS_ISOLATE = 0;
     de4:	43bb      	bics	r3, r7
     de6:	8023      	strh	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
     de8:	2005      	movs	r0, #5
     dea:	6822      	ldr	r2, [r4, #0]
     dec:	f7ff f9f9 	bl	1e2 <mbus_remote_register_write>
        delay(MBUS_DELAY);
     df0:	20c8      	movs	r0, #200	; 0xc8
     df2:	f7ff f959 	bl	a8 <delay>
     df6:	e7cb      	b.n	d90 <operation_sns_run+0x24>
    }else if (stack_state == STK_TEMP_START){
     df8:	6833      	ldr	r3, [r6, #0]
     dfa:	2b02      	cmp	r3, #2
     dfc:	d136      	bne.n	e6c <operation_sns_run+0x100>
        stack_state = STK_TEMP_READ;
     dfe:	3304      	adds	r3, #4
     e00:	6033      	str	r3, [r6, #0]
    if (pmu_setting_state >= PMU_75C){
     e02:	4b9b      	ldr	r3, [pc, #620]	; (1070 <operation_sns_run+0x304>)
     e04:	681a      	ldr	r2, [r3, #0]
     e06:	2a0a      	cmp	r2, #10
     e08:	d91d      	bls.n	e46 <operation_sns_run+0xda>
        pmu_set_sleep_clk(0x5,0xA,0x5,0xF/*V1P2*/);
     e0a:	2205      	movs	r2, #5
     e0c:	230f      	movs	r3, #15
     e0e:	210a      	movs	r1, #10
     e10:	0010      	movs	r0, r2
    sntv5_r01.TSNS_RESETn = 0;
     e12:	2501      	movs	r5, #1
        pmu_set_sleep_clk(0xF,0xA,0x5,0xF/*V1P2*/);
     e14:	f7ff fcdc 	bl	7d0 <pmu_set_sleep_clk>
    sntv5_r01.TSNS_RESETn = 0;
     e18:	4c94      	ldr	r4, [pc, #592]	; (106c <operation_sns_run+0x300>)
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
     e1a:	0029      	movs	r1, r5
    sntv5_r01.TSNS_RESETn = 0;
     e1c:	8823      	ldrh	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
     e1e:	2005      	movs	r0, #5
    sntv5_r01.TSNS_RESETn = 0;
     e20:	43ab      	bics	r3, r5
     e22:	8023      	strh	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
     e24:	6822      	ldr	r2, [r4, #0]
     e26:	f7ff f9dc 	bl	1e2 <mbus_remote_register_write>
    sntv5_r01.TSNS_RESETn = 1;
     e2a:	8823      	ldrh	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
     e2c:	0029      	movs	r1, r5
    sntv5_r01.TSNS_RESETn = 1;
     e2e:	432b      	orrs	r3, r5
     e30:	8023      	strh	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
     e32:	6822      	ldr	r2, [r4, #0]
     e34:	2005      	movs	r0, #5
     e36:	f7ff f9d4 	bl	1e2 <mbus_remote_register_write>
    *GOC_DATA_IRQ = 0;
     e3a:	238c      	movs	r3, #140	; 0x8c
     e3c:	2200      	movs	r2, #0
     e3e:	601a      	str	r2, [r3, #0]
    mbus_sleep_all();
     e40:	f7ff f9b8 	bl	1b4 <mbus_sleep_all>
    while(1);
     e44:	e7fe      	b.n	e44 <operation_sns_run+0xd8>
    }else if (pmu_setting_state >= PMU_35C){
     e46:	681a      	ldr	r2, [r3, #0]
     e48:	2a05      	cmp	r2, #5
     e4a:	d904      	bls.n	e56 <operation_sns_run+0xea>
        pmu_set_sleep_clk(0xA,0xA,0x5,0xF/*V1P2*/);
     e4c:	210a      	movs	r1, #10
     e4e:	230f      	movs	r3, #15
     e50:	2205      	movs	r2, #5
     e52:	0008      	movs	r0, r1
     e54:	e7dd      	b.n	e12 <operation_sns_run+0xa6>
    }else if (pmu_setting_state < PMU_20C){
     e56:	681b      	ldr	r3, [r3, #0]
     e58:	2b02      	cmp	r3, #2
     e5a:	d804      	bhi.n	e66 <operation_sns_run+0xfa>
        pmu_set_sleep_clk(0xF,0xA,0x7,0xF/*V1P2*/);
     e5c:	230f      	movs	r3, #15
     e5e:	2207      	movs	r2, #7
        pmu_set_sleep_clk(0xF,0xA,0x5,0xF/*V1P2*/);
     e60:	210a      	movs	r1, #10
     e62:	0018      	movs	r0, r3
     e64:	e7d5      	b.n	e12 <operation_sns_run+0xa6>
     e66:	230f      	movs	r3, #15
     e68:	2205      	movs	r2, #5
     e6a:	e7f9      	b.n	e60 <operation_sns_run+0xf4>
    }else if (stack_state == STK_TEMP_READ){
     e6c:	6834      	ldr	r4, [r6, #0]
     e6e:	2c06      	cmp	r4, #6
     e70:	d000      	beq.n	e74 <operation_sns_run+0x108>
     e72:	e0ef      	b.n	1054 <operation_sns_run+0x2e8>
        if (wfi_timeout_flag){
     e74:	4d7b      	ldr	r5, [pc, #492]	; (1064 <operation_sns_run+0x2f8>)
     e76:	682b      	ldr	r3, [r5, #0]
     e78:	2b00      	cmp	r3, #0
     e7a:	d01a      	beq.n	eb2 <operation_sns_run+0x146>
            mbus_write_message32(0xFA, 0xFAFAFAFA);
     e7c:	20fa      	movs	r0, #250	; 0xfa
     e7e:	497d      	ldr	r1, [pc, #500]	; (1074 <operation_sns_run+0x308>)
     e80:	f7ff f970 	bl	164 <mbus_write_message32>
        meas_count++;
     e84:	4c7c      	ldr	r4, [pc, #496]	; (1078 <operation_sns_run+0x30c>)
     e86:	6823      	ldr	r3, [r4, #0]
     e88:	3301      	adds	r3, #1
     e8a:	6023      	str	r3, [r4, #0]
        if (meas_count == NUM_TEMP_MEAS){
     e8c:	6823      	ldr	r3, [r4, #0]
     e8e:	2b01      	cmp	r3, #1
     e90:	d109      	bne.n	ea6 <operation_sns_run+0x13a>
            if (wfi_timeout_flag){
     e92:	682a      	ldr	r2, [r5, #0]
     e94:	4b79      	ldr	r3, [pc, #484]	; (107c <operation_sns_run+0x310>)
     e96:	2a00      	cmp	r2, #0
     e98:	d019      	beq.n	ece <operation_sns_run+0x162>
                temp_storage_latest = 0x666;
     e9a:	4a79      	ldr	r2, [pc, #484]	; (1080 <operation_sns_run+0x314>)
     e9c:	601a      	str	r2, [r3, #0]
                wfi_timeout_flag = 0;
     e9e:	2300      	movs	r3, #0
     ea0:	602b      	str	r3, [r5, #0]
                snt_read_wup_counter();
     ea2:	f7ff fbc3 	bl	62c <snt_read_wup_counter>
        if (meas_count < NUM_TEMP_MEAS){    
     ea6:	6823      	ldr	r3, [r4, #0]
     ea8:	2b00      	cmp	r3, #0
     eaa:	d114      	bne.n	ed6 <operation_sns_run+0x16a>
            stack_state = STK_TEMP_START;
     eac:	3302      	adds	r3, #2
     eae:	6033      	str	r3, [r6, #0]
}
     eb0:	e76e      	b.n	d90 <operation_sns_run+0x24>
            set_halt_until_mbus_rx();
     eb2:	f7ff f945 	bl	140 <set_halt_until_mbus_rx>
            mbus_remote_register_read(SNT_ADDR,0x06,1);
     eb6:	2201      	movs	r2, #1
     eb8:	0021      	movs	r1, r4
     eba:	2005      	movs	r0, #5
     ebc:	f7ff f99e 	bl	1fc <mbus_remote_register_read>
            read_data_temp = *REG1;
     ec0:	4b70      	ldr	r3, [pc, #448]	; (1084 <operation_sns_run+0x318>)
     ec2:	681a      	ldr	r2, [r3, #0]
     ec4:	4b70      	ldr	r3, [pc, #448]	; (1088 <operation_sns_run+0x31c>)
     ec6:	601a      	str	r2, [r3, #0]
            set_halt_until_mbus_tx();
     ec8:	f7ff f940 	bl	14c <set_halt_until_mbus_tx>
     ecc:	e7da      	b.n	e84 <operation_sns_run+0x118>
                temp_storage_latest = read_data_temp;
     ece:	4a6e      	ldr	r2, [pc, #440]	; (1088 <operation_sns_run+0x31c>)
     ed0:	6812      	ldr	r2, [r2, #0]
     ed2:	601a      	str	r2, [r3, #0]
     ed4:	e7e7      	b.n	ea6 <operation_sns_run+0x13a>
            meas_count = 0;
     ed6:	2300      	movs	r3, #0
     ed8:	6023      	str	r3, [r4, #0]
            pmu_adc_read_latest();
     eda:	f7ff fcfb 	bl	8d4 <pmu_adc_read_latest>
            uint32_t pmu_setting_prev = pmu_setting_state;
     ede:	4d64      	ldr	r5, [pc, #400]	; (1070 <operation_sns_run+0x304>)
            if (temp_storage_latest > PMU_95C_threshold_sns){
     ee0:	4c66      	ldr	r4, [pc, #408]	; (107c <operation_sns_run+0x310>)
     ee2:	4b6a      	ldr	r3, [pc, #424]	; (108c <operation_sns_run+0x320>)
            uint32_t pmu_setting_prev = pmu_setting_state;
     ee4:	682f      	ldr	r7, [r5, #0]
            if (temp_storage_latest > PMU_95C_threshold_sns){
     ee6:	6822      	ldr	r2, [r4, #0]
     ee8:	681b      	ldr	r3, [r3, #0]
     eea:	429a      	cmp	r2, r3
     eec:	d95c      	bls.n	fa8 <operation_sns_run+0x23c>
                pmu_setting_state = PMU_95C;
     eee:	230d      	movs	r3, #13
                pmu_setting_state = PMU_0C;
     ef0:	602b      	str	r3, [r5, #0]
            pmu_sleep_setting_temp_based();
     ef2:	f7ff fc91 	bl	818 <pmu_sleep_setting_temp_based>
            if (pmu_setting_prev != pmu_setting_state){
     ef6:	682b      	ldr	r3, [r5, #0]
     ef8:	429f      	cmp	r7, r3
     efa:	d001      	beq.n	f00 <operation_sns_run+0x194>
                pmu_active_setting_temp_based();
     efc:	f7ff fbfe 	bl	6fc <pmu_active_setting_temp_based>
            radio_power_on();
     f00:	f7ff fa0a 	bl	318 <radio_power_on>
            temp_sensor_power_off();
     f04:	f7ff f99a 	bl	23c <temp_sensor_power_off>
            sns_ldo_power_off();
     f08:	f7ff f9b2 	bl	270 <sns_ldo_power_off>
            stack_state = STK_IDLE;
     f0c:	2300      	movs	r3, #0
            mbus_write_message32(0xC0, (exec_count << 16) | temp_storage_latest);
     f0e:	4d60      	ldr	r5, [pc, #384]	; (1090 <operation_sns_run+0x324>)
            stack_state = STK_IDLE;
     f10:	6033      	str	r3, [r6, #0]
            mbus_write_message32(0xC0, (exec_count << 16) | temp_storage_latest);
     f12:	6829      	ldr	r1, [r5, #0]
     f14:	6823      	ldr	r3, [r4, #0]
     f16:	0409      	lsls	r1, r1, #16
     f18:	4319      	orrs	r1, r3
     f1a:	20c0      	movs	r0, #192	; 0xc0
     f1c:	f7ff f922 	bl	164 <mbus_write_message32>
            if (temp_storage_debug){
     f20:	4b5c      	ldr	r3, [pc, #368]	; (1094 <operation_sns_run+0x328>)
     f22:	681b      	ldr	r3, [r3, #0]
     f24:	2b00      	cmp	r3, #0
     f26:	d001      	beq.n	f2c <operation_sns_run+0x1c0>
                temp_storage_latest = exec_count;
     f28:	682b      	ldr	r3, [r5, #0]
     f2a:	6023      	str	r3, [r4, #0]
            send_radio_data_mrr(1,0x1,(TEMP_CALIB_B<<16) | (temp_storage_latest&0xFFFF));
     f2c:	2101      	movs	r1, #1
     f2e:	4b5a      	ldr	r3, [pc, #360]	; (1098 <operation_sns_run+0x32c>)
     f30:	0008      	movs	r0, r1
     f32:	681a      	ldr	r2, [r3, #0]
     f34:	6823      	ldr	r3, [r4, #0]
     f36:	0412      	lsls	r2, r2, #16
     f38:	b29b      	uxth	r3, r3
     f3a:	431a      	orrs	r2, r3
     f3c:	f7ff fdf2 	bl	b24 <send_radio_data_mrr>
            exec_count++;
     f40:	682b      	ldr	r3, [r5, #0]
     f42:	3301      	adds	r3, #1
     f44:	602b      	str	r3, [r5, #0]
            if (radio_on){
     f46:	4b55      	ldr	r3, [pc, #340]	; (109c <operation_sns_run+0x330>)
     f48:	681b      	ldr	r3, [r3, #0]
     f4a:	2b00      	cmp	r3, #0
     f4c:	d004      	beq.n	f58 <operation_sns_run+0x1ec>
                radio_ready = 0;
     f4e:	2200      	movs	r2, #0
     f50:	4b53      	ldr	r3, [pc, #332]	; (10a0 <operation_sns_run+0x334>)
     f52:	601a      	str	r2, [r3, #0]
                radio_power_off();
     f54:	f7ff faa4 	bl	4a0 <radio_power_off>
    snt_wup_counter_cur = sleep_count + snt_wup_counter_cur; // should handle rollover
     f58:	4c52      	ldr	r4, [pc, #328]	; (10a4 <operation_sns_run+0x338>)
            snt_set_wup_timer(WAKEUP_PERIOD_SNT);
     f5a:	4b53      	ldr	r3, [pc, #332]	; (10a8 <operation_sns_run+0x33c>)
    mbus_remote_register_write(SNT_ADDR,0x19,snt_wup_counter_cur>>24);
     f5c:	2119      	movs	r1, #25
            snt_set_wup_timer(WAKEUP_PERIOD_SNT);
     f5e:	681b      	ldr	r3, [r3, #0]
    snt_wup_counter_cur = sleep_count + snt_wup_counter_cur; // should handle rollover
     f60:	6822      	ldr	r2, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x19,snt_wup_counter_cur>>24);
     f62:	2005      	movs	r0, #5
    snt_wup_counter_cur = sleep_count + snt_wup_counter_cur; // should handle rollover
     f64:	189b      	adds	r3, r3, r2
     f66:	6023      	str	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x19,snt_wup_counter_cur>>24);
     f68:	6822      	ldr	r2, [r4, #0]
     f6a:	0e12      	lsrs	r2, r2, #24
     f6c:	f7ff f939 	bl	1e2 <mbus_remote_register_write>
    mbus_remote_register_write(SNT_ADDR,0x1A,snt_wup_counter_cur & 0xFFFFFF);
     f70:	6822      	ldr	r2, [r4, #0]
     f72:	211a      	movs	r1, #26
     f74:	0212      	lsls	r2, r2, #8
     f76:	2005      	movs	r0, #5
     f78:	0a12      	lsrs	r2, r2, #8
     f7a:	f7ff f932 	bl	1e2 <mbus_remote_register_write>
    sntv5_r17.WUP_ENABLE = 0x1;
     f7e:	2280      	movs	r2, #128	; 0x80
     f80:	4b4a      	ldr	r3, [pc, #296]	; (10ac <operation_sns_run+0x340>)
     f82:	0412      	lsls	r2, r2, #16
     f84:	6819      	ldr	r1, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
     f86:	2005      	movs	r0, #5
    sntv5_r17.WUP_ENABLE = 0x1;
     f88:	430a      	orrs	r2, r1
     f8a:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
     f8c:	681a      	ldr	r2, [r3, #0]
     f8e:	2117      	movs	r1, #23
     f90:	f7ff f927 	bl	1e2 <mbus_remote_register_write>
    *GOC_DATA_IRQ = 0;
     f94:	2000      	movs	r0, #0
     f96:	238c      	movs	r3, #140	; 0x8c
    set_wakeup_timer(0, 0, 0);
     f98:	0002      	movs	r2, r0
     f9a:	0001      	movs	r1, r0
    *GOC_DATA_IRQ = 0;
     f9c:	6018      	str	r0, [r3, #0]
    set_wakeup_timer(0, 0, 0);
     f9e:	f7ff f8b7 	bl	110 <set_wakeup_timer>
    mbus_sleep_all();
     fa2:	f7ff f907 	bl	1b4 <mbus_sleep_all>
    while(1);
     fa6:	e7fe      	b.n	fa6 <operation_sns_run+0x23a>
            }else if (temp_storage_latest > PMU_85C_threshold_sns){
     fa8:	4b41      	ldr	r3, [pc, #260]	; (10b0 <operation_sns_run+0x344>)
     faa:	6822      	ldr	r2, [r4, #0]
     fac:	681b      	ldr	r3, [r3, #0]
     fae:	429a      	cmp	r2, r3
     fb0:	d901      	bls.n	fb6 <operation_sns_run+0x24a>
                pmu_setting_state = PMU_85C;
     fb2:	230c      	movs	r3, #12
     fb4:	e79c      	b.n	ef0 <operation_sns_run+0x184>
            }else if (temp_storage_latest > PMU_75C_threshold_sns){
     fb6:	4b3f      	ldr	r3, [pc, #252]	; (10b4 <operation_sns_run+0x348>)
     fb8:	6822      	ldr	r2, [r4, #0]
     fba:	681b      	ldr	r3, [r3, #0]
     fbc:	429a      	cmp	r2, r3
     fbe:	d901      	bls.n	fc4 <operation_sns_run+0x258>
                pmu_setting_state = PMU_75C;
     fc0:	230b      	movs	r3, #11
     fc2:	e795      	b.n	ef0 <operation_sns_run+0x184>
            }else if (temp_storage_latest > PMU_65C_threshold_sns){
     fc4:	4b3c      	ldr	r3, [pc, #240]	; (10b8 <operation_sns_run+0x34c>)
     fc6:	6822      	ldr	r2, [r4, #0]
     fc8:	681b      	ldr	r3, [r3, #0]
     fca:	429a      	cmp	r2, r3
     fcc:	d901      	bls.n	fd2 <operation_sns_run+0x266>
                pmu_setting_state = PMU_65C;
     fce:	230a      	movs	r3, #10
     fd0:	e78e      	b.n	ef0 <operation_sns_run+0x184>
            }else if (temp_storage_latest > PMU_55C_threshold_sns){
     fd2:	4b3a      	ldr	r3, [pc, #232]	; (10bc <operation_sns_run+0x350>)
     fd4:	6822      	ldr	r2, [r4, #0]
     fd6:	681b      	ldr	r3, [r3, #0]
     fd8:	429a      	cmp	r2, r3
     fda:	d901      	bls.n	fe0 <operation_sns_run+0x274>
                pmu_setting_state = PMU_55C;
     fdc:	2309      	movs	r3, #9
     fde:	e787      	b.n	ef0 <operation_sns_run+0x184>
            }else if (temp_storage_latest > PMU_45C_threshold_sns){
     fe0:	4b37      	ldr	r3, [pc, #220]	; (10c0 <operation_sns_run+0x354>)
     fe2:	6822      	ldr	r2, [r4, #0]
     fe4:	681b      	ldr	r3, [r3, #0]
     fe6:	429a      	cmp	r2, r3
     fe8:	d901      	bls.n	fee <operation_sns_run+0x282>
                pmu_setting_state = PMU_45C;
     fea:	2308      	movs	r3, #8
     fec:	e780      	b.n	ef0 <operation_sns_run+0x184>
            }else if (temp_storage_latest > PMU_40C_threshold_sns){
     fee:	4b35      	ldr	r3, [pc, #212]	; (10c4 <operation_sns_run+0x358>)
     ff0:	6822      	ldr	r2, [r4, #0]
     ff2:	681b      	ldr	r3, [r3, #0]
     ff4:	429a      	cmp	r2, r3
     ff6:	d901      	bls.n	ffc <operation_sns_run+0x290>
                pmu_setting_state = PMU_40C;
     ff8:	2307      	movs	r3, #7
     ffa:	e779      	b.n	ef0 <operation_sns_run+0x184>
            }else if (temp_storage_latest > PMU_35C_threshold_sns){
     ffc:	4b32      	ldr	r3, [pc, #200]	; (10c8 <operation_sns_run+0x35c>)
     ffe:	6822      	ldr	r2, [r4, #0]
    1000:	681b      	ldr	r3, [r3, #0]
    1002:	429a      	cmp	r2, r3
    1004:	d901      	bls.n	100a <operation_sns_run+0x29e>
                pmu_setting_state = PMU_35C;
    1006:	2306      	movs	r3, #6
    1008:	e772      	b.n	ef0 <operation_sns_run+0x184>
            }else if (temp_storage_latest > PMU_30C_threshold_sns){
    100a:	4b30      	ldr	r3, [pc, #192]	; (10cc <operation_sns_run+0x360>)
    100c:	6822      	ldr	r2, [r4, #0]
    100e:	681b      	ldr	r3, [r3, #0]
    1010:	429a      	cmp	r2, r3
    1012:	d901      	bls.n	1018 <operation_sns_run+0x2ac>
                pmu_setting_state = PMU_30C;
    1014:	2305      	movs	r3, #5
    1016:	e76b      	b.n	ef0 <operation_sns_run+0x184>
            }else if (temp_storage_latest > PMU_25C_threshold_sns){
    1018:	4b2d      	ldr	r3, [pc, #180]	; (10d0 <operation_sns_run+0x364>)
    101a:	6822      	ldr	r2, [r4, #0]
    101c:	681b      	ldr	r3, [r3, #0]
    101e:	429a      	cmp	r2, r3
    1020:	d901      	bls.n	1026 <operation_sns_run+0x2ba>
                pmu_setting_state = PMU_25C;
    1022:	2304      	movs	r3, #4
    1024:	e764      	b.n	ef0 <operation_sns_run+0x184>
            }else if (temp_storage_latest > PMU_20C_threshold_sns){
    1026:	4b2b      	ldr	r3, [pc, #172]	; (10d4 <operation_sns_run+0x368>)
    1028:	6822      	ldr	r2, [r4, #0]
    102a:	681b      	ldr	r3, [r3, #0]
    102c:	429a      	cmp	r2, r3
    102e:	d901      	bls.n	1034 <operation_sns_run+0x2c8>
                pmu_setting_state = PMU_20C;
    1030:	2303      	movs	r3, #3
    1032:	e75d      	b.n	ef0 <operation_sns_run+0x184>
            }else if (temp_storage_latest > PMU_15C_threshold_sns){
    1034:	4b28      	ldr	r3, [pc, #160]	; (10d8 <operation_sns_run+0x36c>)
    1036:	6822      	ldr	r2, [r4, #0]
    1038:	681b      	ldr	r3, [r3, #0]
    103a:	429a      	cmp	r2, r3
    103c:	d901      	bls.n	1042 <operation_sns_run+0x2d6>
                pmu_setting_state = PMU_15C;
    103e:	2302      	movs	r3, #2
    1040:	e756      	b.n	ef0 <operation_sns_run+0x184>
            }else if (temp_storage_latest > PMU_10C_threshold_sns){
    1042:	4b26      	ldr	r3, [pc, #152]	; (10dc <operation_sns_run+0x370>)
    1044:	6822      	ldr	r2, [r4, #0]
    1046:	681b      	ldr	r3, [r3, #0]
    1048:	429a      	cmp	r2, r3
    104a:	d901      	bls.n	1050 <operation_sns_run+0x2e4>
                pmu_setting_state = PMU_10C;
    104c:	2301      	movs	r3, #1
    104e:	e74f      	b.n	ef0 <operation_sns_run+0x184>
                pmu_setting_state = PMU_0C;
    1050:	2300      	movs	r3, #0
    1052:	e74d      	b.n	ef0 <operation_sns_run+0x184>
        temp_sensor_power_off();
    1054:	f7ff f8f2 	bl	23c <temp_sensor_power_off>
        sns_ldo_power_off();
    1058:	f7ff f90a 	bl	270 <sns_ldo_power_off>
        operation_sleep_notimer();
    105c:	f7ff fca6 	bl	9ac <operation_sleep_notimer>
    1060:	00001e24 	.word	0x00001e24
    1064:	00001e60 	.word	0x00001e60
    1068:	00001d8c 	.word	0x00001d8c
    106c:	00001d90 	.word	0x00001d90
    1070:	00001e10 	.word	0x00001e10
    1074:	fafafafa 	.word	0xfafafafa
    1078:	00001e2c 	.word	0x00001e2c
    107c:	00001da8 	.word	0x00001da8
    1080:	00000666 	.word	0x00000666
    1084:	a0000004 	.word	0xa0000004
    1088:	00001e18 	.word	0x00001e18
    108c:	00001db4 	.word	0x00001db4
    1090:	00001de0 	.word	0x00001de0
    1094:	00001e44 	.word	0x00001e44
    1098:	00001e04 	.word	0x00001e04
    109c:	00001e64 	.word	0x00001e64
    10a0:	00001dc0 	.word	0x00001dc0
    10a4:	00001e5c 	.word	0x00001e5c
    10a8:	00001e34 	.word	0x00001e34
    10ac:	00001da4 	.word	0x00001da4
    10b0:	00001de8 	.word	0x00001de8
    10b4:	00001e38 	.word	0x00001e38
    10b8:	00001dac 	.word	0x00001dac
    10bc:	00001e30 	.word	0x00001e30
    10c0:	00001e08 	.word	0x00001e08
    10c4:	00001dec 	.word	0x00001dec
    10c8:	00001e14 	.word	0x00001e14
    10cc:	00001dd8 	.word	0x00001dd8
    10d0:	00001e0c 	.word	0x00001e0c
    10d4:	00001e48 	.word	0x00001e48
    10d8:	00001df0 	.word	0x00001df0
    10dc:	00001db0 	.word	0x00001db0

Disassembly of section .text.startup.main:

000010e0 <main>:

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(){
    10e0:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}

    // Only enable relevant interrupts
    *NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);
    10e2:	4bf8      	ldr	r3, [pc, #992]	; (14c4 <main+0x3e4>)
    10e4:	4af8      	ldr	r2, [pc, #992]	; (14c8 <main+0x3e8>)
    10e6:	4df9      	ldr	r5, [pc, #996]	; (14cc <main+0x3ec>)
    10e8:	601a      	str	r2, [r3, #0]

    // Wakeup IRQ handling
    if (irq_pending.wakeup) {
    10ea:	4bf9      	ldr	r3, [pc, #996]	; (14d0 <main+0x3f0>)
    10ec:	681b      	ldr	r3, [r3, #0]
    10ee:	07db      	lsls	r3, r3, #31
    10f0:	d50c      	bpl.n	110c <main+0x2c>
        //[ 4] = mbus message
        //[ 8] = gpio[0]
        //[ 9] = gpio[1]
        //[10] = gpio[2]
        //[11] = gpio[3]
        irq_pending.wakeup = 0;
    10f2:	2201      	movs	r2, #1
    10f4:	4bf6      	ldr	r3, [pc, #984]	; (14d0 <main+0x3f0>)
        delay(MBUS_DELAY);
    10f6:	20c8      	movs	r0, #200	; 0xc8
        irq_pending.wakeup = 0;
    10f8:	681b      	ldr	r3, [r3, #0]
    10fa:	4393      	bics	r3, r2
    10fc:	4af4      	ldr	r2, [pc, #976]	; (14d0 <main+0x3f0>)
    10fe:	6013      	str	r3, [r2, #0]
        delay(MBUS_DELAY);
    1100:	f7fe ffd2 	bl	a8 <delay>
        mbus_write_message32(0xAA,*SREG_WAKEUP_SOURCE); // 0x1: GOC; 0x2: PRC Timer; 0x10: SNT
    1104:	20aa      	movs	r0, #170	; 0xaa
    1106:	6829      	ldr	r1, [r5, #0]
    1108:	f7ff f82c 	bl	164 <mbus_write_message32>
    }
  
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);
    110c:	48f1      	ldr	r0, [pc, #964]	; (14d4 <main+0x3f4>)
    110e:	f7fe ffed 	bl	ec <config_timerwd>

    wakeup_count++;
    1112:	4cf1      	ldr	r4, [pc, #964]	; (14d8 <main+0x3f8>)
    1114:	6823      	ldr	r3, [r4, #0]
    1116:	3301      	adds	r3, #1
    1118:	6023      	str	r3, [r4, #0]

    // Figure out who triggered wakeup
    if(*SREG_WAKEUP_SOURCE & 0x00000008){
    111a:	682b      	ldr	r3, [r5, #0]
        mbus_write_message32(0xAA,0x11331133);
        #endif
    }

    // Initialization sequence
    if (enumerated != 0x5453104b){
    111c:	4bef      	ldr	r3, [pc, #956]	; (14dc <main+0x3fc>)
    111e:	4df0      	ldr	r5, [pc, #960]	; (14e0 <main+0x400>)
    1120:	681b      	ldr	r3, [r3, #0]
    1122:	42ab      	cmp	r3, r5
    1124:	d100      	bne.n	1128 <main+0x48>
    1126:	e2d7      	b.n	16d8 <main+0x5f8>
    mbus_write_message32(0xAF, 0x00001000);
    1128:	2180      	movs	r1, #128	; 0x80
    112a:	20af      	movs	r0, #175	; 0xaf
    112c:	0149      	lsls	r1, r1, #5
    112e:	f7ff f819 	bl	164 <mbus_write_message32>
    prev21e_r0B.CLK_GEN_RING = 0x1;         // 0x1
    1132:	4bec      	ldr	r3, [pc, #944]	; (14e4 <main+0x404>)
    1134:	49ec      	ldr	r1, [pc, #944]	; (14e8 <main+0x408>)
    1136:	681a      	ldr	r2, [r3, #0]
    prev21e_r0B.CLK_GEN_DIV_CORE = 0x2;     // 0x3
    1138:	2080      	movs	r0, #128	; 0x80
    prev21e_r0B.CLK_GEN_RING = 0x1;         // 0x1
    113a:	4011      	ands	r1, r2
    113c:	2280      	movs	r2, #128	; 0x80
    113e:	0192      	lsls	r2, r2, #6
    1140:	430a      	orrs	r2, r1
    1142:	601a      	str	r2, [r3, #0]
    prev21e_r0B.CLK_GEN_DIV_MBC = 0x1;      // 0x2
    1144:	681a      	ldr	r2, [r3, #0]
    1146:	49e9      	ldr	r1, [pc, #932]	; (14ec <main+0x40c>)
    prev21e_r0B.CLK_GEN_DIV_CORE = 0x2;     // 0x3
    1148:	0300      	lsls	r0, r0, #12
    prev21e_r0B.CLK_GEN_DIV_MBC = 0x1;      // 0x2
    114a:	400a      	ands	r2, r1
    114c:	2180      	movs	r1, #128	; 0x80
    114e:	0209      	lsls	r1, r1, #8
    1150:	430a      	orrs	r2, r1
    1152:	601a      	str	r2, [r3, #0]
    prev21e_r0B.CLK_GEN_DIV_CORE = 0x2;     // 0x3
    1154:	681a      	ldr	r2, [r3, #0]
    1156:	49e6      	ldr	r1, [pc, #920]	; (14f0 <main+0x410>)
    prev21e_r0B.GOC_SEL = 0xF;              // 0x8
    1158:	260f      	movs	r6, #15
    prev21e_r0B.CLK_GEN_DIV_CORE = 0x2;     // 0x3
    115a:	400a      	ands	r2, r1
    115c:	4302      	orrs	r2, r0
    115e:	601a      	str	r2, [r3, #0]
    prev21e_r0B.GOC_CLK_GEN_SEL_FREQ = 0x5; // 0x7
    1160:	681a      	ldr	r2, [r3, #0]
    1162:	49e4      	ldr	r1, [pc, #912]	; (14f4 <main+0x414>)
    stack_state = STK_IDLE;     //0x0;
    1164:	2700      	movs	r7, #0
    prev21e_r0B.GOC_CLK_GEN_SEL_FREQ = 0x5; // 0x7
    1166:	4011      	ands	r1, r2
    1168:	22a0      	movs	r2, #160	; 0xa0
    116a:	0092      	lsls	r2, r2, #2
    116c:	430a      	orrs	r2, r1
    116e:	601a      	str	r2, [r3, #0]
    prev21e_r0B.GOC_CLK_GEN_SEL_DIV = 0x0;  // 0x1
    1170:	681a      	ldr	r2, [r3, #0]
    1172:	49e1      	ldr	r1, [pc, #900]	; (14f8 <main+0x418>)
    mbus_write_message32(0xAF, 0x00001001);
    1174:	20af      	movs	r0, #175	; 0xaf
    prev21e_r0B.GOC_CLK_GEN_SEL_DIV = 0x0;  // 0x1
    1176:	400a      	ands	r2, r1
    1178:	601a      	str	r2, [r3, #0]
    prev21e_r0B.GOC_SEL = 0xF;              // 0x8
    117a:	681a      	ldr	r2, [r3, #0]
    mbus_write_message32(0xAF, 0x00001001);
    117c:	49df      	ldr	r1, [pc, #892]	; (14fc <main+0x41c>)
    prev21e_r0B.GOC_SEL = 0xF;              // 0x8
    117e:	4332      	orrs	r2, r6
    1180:	601a      	str	r2, [r3, #0]
    *REG_CLKGEN_TUNE = prev21e_r0B.as_int;
    1182:	681a      	ldr	r2, [r3, #0]
    1184:	4bde      	ldr	r3, [pc, #888]	; (1500 <main+0x420>)
    1186:	601a      	str	r2, [r3, #0]
    mbus_write_message32(0xAF, 0x00001001);
    1188:	f7fe ffec 	bl	164 <mbus_write_message32>
    stack_state = STK_IDLE;     //0x0;
    118c:	4bdd      	ldr	r3, [pc, #884]	; (1504 <main+0x424>)
    PMU_ADC_3P0_VAL = 0x62;
    118e:	2262      	movs	r2, #98	; 0x62
    stack_state = STK_IDLE;     //0x0;
    1190:	601f      	str	r7, [r3, #0]
    enumerated = 0x5453104b; // 0x5453 is TS in ascii
    1192:	4bd2      	ldr	r3, [pc, #840]	; (14dc <main+0x3fc>)
    mbus_write_message32(0xAF, 0x00001002);
    1194:	49dc      	ldr	r1, [pc, #880]	; (1508 <main+0x428>)
    enumerated = 0x5453104b; // 0x5453 is TS in ascii
    1196:	601d      	str	r5, [r3, #0]
    exec_count = 0;
    1198:	4bdc      	ldr	r3, [pc, #880]	; (150c <main+0x42c>)
    mbus_write_message32(0xAF, 0x00001002);
    119a:	20af      	movs	r0, #175	; 0xaf
    exec_count = 0;
    119c:	601f      	str	r7, [r3, #0]
    wakeup_count = 0;
    119e:	6027      	str	r7, [r4, #0]
    pmu_parking_mode = 3;
    11a0:	2403      	movs	r4, #3
    exec_count_irq = 0;
    11a2:	4bdb      	ldr	r3, [pc, #876]	; (1510 <main+0x430>)
    sntv5_r01.TSNS_BURST_MODE = 0;
    11a4:	2540      	movs	r5, #64	; 0x40
    exec_count_irq = 0;
    11a6:	601f      	str	r7, [r3, #0]
    PMU_ADC_3P0_VAL = 0x62;
    11a8:	4bda      	ldr	r3, [pc, #872]	; (1514 <main+0x434>)
    11aa:	601a      	str	r2, [r3, #0]
    pmu_parking_mode = 3;
    11ac:	4bda      	ldr	r3, [pc, #872]	; (1518 <main+0x438>)
    11ae:	601c      	str	r4, [r3, #0]
    mbus_write_message32(0xAF, 0x00001002);
    11b0:	f7fe ffd8 	bl	164 <mbus_write_message32>
    set_halt_until_mbus_trx();  // [CISv2] Newly Added
    11b4:	f7fe ffd0 	bl	158 <set_halt_until_mbus_trx>
    mbus_enumerate(SNT_ADDR);
    11b8:	2005      	movs	r0, #5
    11ba:	f7fe fff1 	bl	1a0 <mbus_enumerate>
    mbus_enumerate(MRR_ADDR);
    11be:	2004      	movs	r0, #4
    11c0:	f7fe ffee 	bl	1a0 <mbus_enumerate>
    mbus_enumerate(PMU_ADDR);
    11c4:	2006      	movs	r0, #6
    11c6:	f7fe ffeb 	bl	1a0 <mbus_enumerate>
    set_halt_until_mbus_tx();   // [CISv2] Newly Added
    11ca:	f7fe ffbf 	bl	14c <set_halt_until_mbus_tx>
    pmu_setting_state = PMU_25C;
    11ce:	4bd3      	ldr	r3, [pc, #844]	; (151c <main+0x43c>)
    11d0:	3401      	adds	r4, #1
    11d2:	601c      	str	r4, [r3, #0]
    pmu_active_setting_temp_based();
    11d4:	f7ff fa92 	bl	6fc <pmu_active_setting_temp_based>
    pmu_sleep_setting_temp_based();
    11d8:	f7ff fb1e 	bl	818 <pmu_sleep_setting_temp_based>
    pmu_reg_write(0x05,
    11dc:	49d0      	ldr	r1, [pc, #832]	; (1520 <main+0x440>)
    11de:	2005      	movs	r0, #5
    11e0:	f7ff fa52 	bl	688 <pmu_reg_write>
    pmu_set_sar_override(0x45);
    11e4:	2045      	movs	r0, #69	; 0x45
    11e6:	f7ff fb65 	bl	8b4 <pmu_set_sar_override>
    pmu_reg_write(0x3C,
    11ea:	203c      	movs	r0, #60	; 0x3c
    11ec:	49cd      	ldr	r1, [pc, #820]	; (1524 <main+0x444>)
    11ee:	f7ff fa4b 	bl	688 <pmu_reg_write>
    pmu_reg_write(0x36,val); 
    11f2:	2101      	movs	r1, #1
    11f4:	2036      	movs	r0, #54	; 0x36
    11f6:	f7ff fa47 	bl	688 <pmu_reg_write>
    delay(MBUS_DELAY*10);
    11fa:	20fa      	movs	r0, #250	; 0xfa
    11fc:	00c0      	lsls	r0, r0, #3
    11fe:	f7fe ff53 	bl	a8 <delay>
    pmu_reg_write(0x33,2);
    1202:	2102      	movs	r1, #2
    1204:	2033      	movs	r0, #51	; 0x33
    1206:	f7ff fa3f 	bl	688 <pmu_reg_write>
    pmu_reg_write(0x3C,
    120a:	203c      	movs	r0, #60	; 0x3c
    120c:	49c5      	ldr	r1, [pc, #788]	; (1524 <main+0x444>)
    120e:	f7ff fa3b 	bl	688 <pmu_reg_write>
    pmu_reg_write(0x3A, 
    1212:	203a      	movs	r0, #58	; 0x3a
    1214:	49c4      	ldr	r1, [pc, #784]	; (1528 <main+0x448>)
    1216:	f7ff fa37 	bl	688 <pmu_reg_write>
    pmu_reg_write(0x3C,
    121a:	203c      	movs	r0, #60	; 0x3c
    121c:	49c1      	ldr	r1, [pc, #772]	; (1524 <main+0x444>)
    121e:	f7ff fa33 	bl	688 <pmu_reg_write>
    pmu_reg_write(0x3B,
    1222:	203b      	movs	r0, #59	; 0x3b
    1224:	49c1      	ldr	r1, [pc, #772]	; (152c <main+0x44c>)
    1226:	f7ff fa2f 	bl	688 <pmu_reg_write>
    sntv5_r01.TSNS_RESETn = 0;
    122a:	2001      	movs	r0, #1
    122c:	4bc0      	ldr	r3, [pc, #768]	; (1530 <main+0x450>)
    122e:	881a      	ldrh	r2, [r3, #0]
    1230:	4382      	bics	r2, r0
    1232:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_EN_IRQ = 1;
    1234:	2280      	movs	r2, #128	; 0x80
    1236:	8819      	ldrh	r1, [r3, #0]
    1238:	0052      	lsls	r2, r2, #1
    123a:	430a      	orrs	r2, r1
    sntv5_r01.TSNS_CONT_MODE = 0;
    123c:	2180      	movs	r1, #128	; 0x80
    sntv5_r01.TSNS_EN_IRQ = 1;
    123e:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_BURST_MODE = 0;
    1240:	881a      	ldrh	r2, [r3, #0]
    1242:	43aa      	bics	r2, r5
    1244:	801a      	strh	r2, [r3, #0]
    sntv5_r01.TSNS_CONT_MODE = 0;
    1246:	881a      	ldrh	r2, [r3, #0]
    1248:	438a      	bics	r2, r1
    124a:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
    124c:	0001      	movs	r1, r0
    124e:	681a      	ldr	r2, [r3, #0]
    1250:	3004      	adds	r0, #4
    1252:	f7fe ffc6 	bl	1e2 <mbus_remote_register_write>
    sntv5_r03.TSNS_SEL_STB_TIME = 0x1; 
    1256:	4ab7      	ldr	r2, [pc, #732]	; (1534 <main+0x454>)
    1258:	49b7      	ldr	r1, [pc, #732]	; (1538 <main+0x458>)
    125a:	6813      	ldr	r3, [r2, #0]
    mbus_remote_register_write(SNT_ADDR,0x03,sntv5_r03.as_int);
    125c:	2005      	movs	r0, #5
    sntv5_r03.TSNS_SEL_STB_TIME = 0x1; 
    125e:	400b      	ands	r3, r1
    1260:	431d      	orrs	r5, r3
    1262:	6015      	str	r5, [r2, #0]
    sntv5_r03.TSNS_SEL_CONV_TIME = 0x6; // Default: 0x6
    1264:	2506      	movs	r5, #6
    1266:	6813      	ldr	r3, [r2, #0]
    mbus_remote_register_write(SNT_ADDR,0x03,sntv5_r03.as_int);
    1268:	31c5      	adds	r1, #197	; 0xc5
    sntv5_r03.TSNS_SEL_CONV_TIME = 0x6; // Default: 0x6
    126a:	43b3      	bics	r3, r6
    126c:	432b      	orrs	r3, r5
    126e:	6013      	str	r3, [r2, #0]
    mbus_remote_register_write(SNT_ADDR,0x03,sntv5_r03.as_int);
    1270:	6812      	ldr	r2, [r2, #0]
    1272:	31ff      	adds	r1, #255	; 0xff
    1274:	f7fe ffb5 	bl	1e2 <mbus_remote_register_write>
    snt_timer_enabled = 0;  // [CISv2] Newly Added
    1278:	4bb0      	ldr	r3, [pc, #704]	; (153c <main+0x45c>)
    sntv5_r0A.TMR_S = 0x1; // Default: 0x4, use 1 for good TC
    127a:	49b1      	ldr	r1, [pc, #708]	; (1540 <main+0x460>)
    snt_timer_enabled = 0;  // [CISv2] Newly Added
    127c:	601f      	str	r7, [r3, #0]
    sntv5_r0A.TMR_S = 0x1; // Default: 0x4, use 1 for good TC
    127e:	4bb1      	ldr	r3, [pc, #708]	; (1544 <main+0x464>)
    mbus_remote_register_write(SNT_ADDR,0x0A,sntv5_r0A.as_int);
    1280:	2005      	movs	r0, #5
    sntv5_r0A.TMR_S = 0x1; // Default: 0x4, use 1 for good TC
    1282:	681a      	ldr	r2, [r3, #0]
    sntv5_r17.WUP_CLK_SEL = 0x0; 
    1284:	4eb0      	ldr	r6, [pc, #704]	; (1548 <main+0x468>)
    sntv5_r0A.TMR_S = 0x1; // Default: 0x4, use 1 for good TC
    1286:	4011      	ands	r1, r2
    1288:	2280      	movs	r2, #128	; 0x80
    128a:	0392      	lsls	r2, r2, #14
    128c:	430a      	orrs	r2, r1
    128e:	601a      	str	r2, [r3, #0]
    sntv5_r0A.TMR_DIFF_CON = 0x3FEF; // Default: 0x3FFF
    1290:	681a      	ldr	r2, [r3, #0]
    1292:	49ae      	ldr	r1, [pc, #696]	; (154c <main+0x46c>)
    sntv5_r17.WUP_AUTO_RESET = 0x0; // Automatically reset counter to 0 upon sleep (Default: 1'h1)
    1294:	4dae      	ldr	r5, [pc, #696]	; (1550 <main+0x470>)
    sntv5_r0A.TMR_DIFF_CON = 0x3FEF; // Default: 0x3FFF
    1296:	4011      	ands	r1, r2
    1298:	4aae      	ldr	r2, [pc, #696]	; (1554 <main+0x474>)
    129a:	430a      	orrs	r2, r1
    129c:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x0A,sntv5_r0A.as_int);
    129e:	681a      	ldr	r2, [r3, #0]
    12a0:	210a      	movs	r1, #10
    12a2:	f7fe ff9e 	bl	1e2 <mbus_remote_register_write>
    sntv5_r09.TMR_SEL_CAP = 0x80; // Default : 8'h8
    12a6:	4aac      	ldr	r2, [pc, #688]	; (1558 <main+0x478>)
    12a8:	49ac      	ldr	r1, [pc, #688]	; (155c <main+0x47c>)
    12aa:	6813      	ldr	r3, [r2, #0]
    mbus_remote_register_write(SNT_ADDR,0x09,sntv5_r09.as_int);
    12ac:	2005      	movs	r0, #5
    sntv5_r09.TMR_SEL_CAP = 0x80; // Default : 8'h8
    12ae:	400b      	ands	r3, r1
    12b0:	2180      	movs	r1, #128	; 0x80
    12b2:	0209      	lsls	r1, r1, #8
    12b4:	430b      	orrs	r3, r1
    12b6:	6013      	str	r3, [r2, #0]
    sntv5_r09.TMR_SEL_DCAP = 0x3F; // Default : 6'h4
    12b8:	23fc      	movs	r3, #252	; 0xfc
    12ba:	6811      	ldr	r1, [r2, #0]
    12bc:	430b      	orrs	r3, r1
    12be:	6013      	str	r3, [r2, #0]
    mbus_remote_register_write(SNT_ADDR,0x09,sntv5_r09.as_int);
    12c0:	2109      	movs	r1, #9
    12c2:	6812      	ldr	r2, [r2, #0]
    12c4:	f7fe ff8d 	bl	1e2 <mbus_remote_register_write>
    sntv5_r17.WUP_CLK_SEL = 0x0; 
    12c8:	4ba5      	ldr	r3, [pc, #660]	; (1560 <main+0x480>)
    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
    12ca:	2117      	movs	r1, #23
    sntv5_r17.WUP_CLK_SEL = 0x0; 
    12cc:	681a      	ldr	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
    12ce:	2005      	movs	r0, #5
    sntv5_r17.WUP_CLK_SEL = 0x0; 
    12d0:	4032      	ands	r2, r6
    12d2:	601a      	str	r2, [r3, #0]
    sntv5_r17.WUP_AUTO_RESET = 0x0; // Automatically reset counter to 0 upon sleep (Default: 1'h1)
    12d4:	681a      	ldr	r2, [r3, #0]
    12d6:	402a      	ands	r2, r5
    12d8:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
    12da:	681a      	ldr	r2, [r3, #0]
    12dc:	f7fe ff81 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r21.TRX_ENb_CONT_RC = 0;  //RX_Decap P 
    12e0:	4ba0      	ldr	r3, [pc, #640]	; (1564 <main+0x484>)
    12e2:	49a1      	ldr	r1, [pc, #644]	; (1568 <main+0x488>)
    12e4:	881a      	ldrh	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x21,mrrv11a_r21.as_int);
    12e6:	0020      	movs	r0, r4
    mrrv11a_r21.TRX_ENb_CONT_RC = 0;  //RX_Decap P 
    12e8:	400a      	ands	r2, r1
    12ea:	801a      	strh	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x21,mrrv11a_r21.as_int);
    12ec:	681a      	ldr	r2, [r3, #0]
    12ee:	2121      	movs	r1, #33	; 0x21
    12f0:	f7fe ff77 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    12f4:	4d9d      	ldr	r5, [pc, #628]	; (156c <main+0x48c>)
    12f6:	4a96      	ldr	r2, [pc, #600]	; (1550 <main+0x470>)
    12f8:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    12fa:	2103      	movs	r1, #3
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    12fc:	4013      	ands	r3, r2
    12fe:	602b      	str	r3, [r5, #0]
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    1300:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    1302:	0020      	movs	r0, r4
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    1304:	401e      	ands	r6, r3
    1306:	602e      	str	r6, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    1308:	682a      	ldr	r2, [r5, #0]
    130a:	f7fe ff6a 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
    130e:	2280      	movs	r2, #128	; 0x80
    1310:	682b      	ldr	r3, [r5, #0]
    1312:	0312      	lsls	r2, r2, #12
    1314:	4313      	orrs	r3, r2
    1316:	602b      	str	r3, [r5, #0]
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    1318:	2380      	movs	r3, #128	; 0x80
    131a:	682a      	ldr	r2, [r5, #0]
    131c:	02db      	lsls	r3, r3, #11
    131e:	4313      	orrs	r3, r2
    1320:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    1322:	682a      	ldr	r2, [r5, #0]
    1324:	2103      	movs	r1, #3
    1326:	0020      	movs	r0, r4
    1328:	f7fe ff5b 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r00.TRX_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    132c:	227e      	movs	r2, #126	; 0x7e
    132e:	4e90      	ldr	r6, [pc, #576]	; (1570 <main+0x490>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1330:	0039      	movs	r1, r7
    mrrv11a_r00.TRX_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    1332:	6833      	ldr	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1334:	0020      	movs	r0, r4
    mrrv11a_r00.TRX_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    1336:	4393      	bics	r3, r2
    1338:	3a6e      	subs	r2, #110	; 0x6e
    133a:	4313      	orrs	r3, r2
    133c:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    133e:	6832      	ldr	r2, [r6, #0]
    1340:	f7fe ff4f 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    1344:	2001      	movs	r0, #1
    1346:	6833      	ldr	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1348:	0039      	movs	r1, r7
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    134a:	4303      	orrs	r3, r0
    134c:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    134e:	6832      	ldr	r2, [r6, #0]
    1350:	0020      	movs	r0, r4
    1352:	f7fe ff46 	bl	1e2 <mbus_remote_register_write>
    config_timerwd(TIMERWD_VAL);
    1356:	485f      	ldr	r0, [pc, #380]	; (14d4 <main+0x3f4>)
    1358:	f7fe fec8 	bl	ec <config_timerwd>
    *REG_MBUS_WD = 1500000*3; // default: 1500000
    135c:	4b85      	ldr	r3, [pc, #532]	; (1574 <main+0x494>)
    135e:	4a86      	ldr	r2, [pc, #536]	; (1578 <main+0x498>)
    delay(MBUS_DELAY*200); // Wait for decap to charge
    1360:	4886      	ldr	r0, [pc, #536]	; (157c <main+0x49c>)
    *REG_MBUS_WD = 1500000*3; // default: 1500000
    1362:	6013      	str	r3, [r2, #0]
    delay(MBUS_DELAY*200); // Wait for decap to charge
    1364:	f7fe fea0 	bl	a8 <delay>
    mrrv11a_r1F.LC_CLK_RING = 0x3;  // ~ 150 kHz
    1368:	2103      	movs	r1, #3
    136a:	4b85      	ldr	r3, [pc, #532]	; (1580 <main+0x4a0>)
    mbus_remote_register_write(MRR_ADDR,0x1F,mrrv11a_r1F.as_int);
    136c:	0020      	movs	r0, r4
    mrrv11a_r1F.LC_CLK_RING = 0x3;  // ~ 150 kHz
    136e:	781a      	ldrb	r2, [r3, #0]
    1370:	430a      	orrs	r2, r1
    1372:	701a      	strb	r2, [r3, #0]
    mrrv11a_r1F.LC_CLK_DIV = 0x3;  // ~ 150 kHz
    1374:	220c      	movs	r2, #12
    1376:	7819      	ldrb	r1, [r3, #0]
    1378:	430a      	orrs	r2, r1
    137a:	701a      	strb	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x1F,mrrv11a_r1F.as_int);
    137c:	681a      	ldr	r2, [r3, #0]
    137e:	211f      	movs	r1, #31
    1380:	f7fe ff2f 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r12.FSM_TX_PW_LEN = 24; //100us PW
    1384:	2118      	movs	r1, #24
    1386:	4b7f      	ldr	r3, [pc, #508]	; (1584 <main+0x4a4>)
    mrrv11a_r13.FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    1388:	487f      	ldr	r0, [pc, #508]	; (1588 <main+0x4a8>)
    mrrv11a_r12.FSM_TX_PW_LEN = 24; //100us PW
    138a:	681a      	ldr	r2, [r3, #0]
    138c:	0a92      	lsrs	r2, r2, #10
    138e:	0292      	lsls	r2, r2, #10
    1390:	430a      	orrs	r2, r1
    1392:	601a      	str	r2, [r3, #0]
    mrrv11a_r13.FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    1394:	8802      	ldrh	r2, [r0, #0]
    1396:	314c      	adds	r1, #76	; 0x4c
    1398:	0bd2      	lsrs	r2, r2, #15
    139a:	03d2      	lsls	r2, r2, #15
    139c:	430a      	orrs	r2, r1
    139e:	8002      	strh	r2, [r0, #0]
    mrrv11a_r12.FSM_TX_PS_LEN = 49; // PW=PS   
    13a0:	681a      	ldr	r2, [r3, #0]
    13a2:	497a      	ldr	r1, [pc, #488]	; (158c <main+0x4ac>)
    mbus_remote_register_write(MRR_ADDR,0x12,mrrv11a_r12.as_int);
    13a4:	0020      	movs	r0, r4
    mrrv11a_r12.FSM_TX_PS_LEN = 49; // PW=PS   
    13a6:	4011      	ands	r1, r2
    13a8:	22c4      	movs	r2, #196	; 0xc4
    13aa:	0212      	lsls	r2, r2, #8
    13ac:	430a      	orrs	r2, r1
    13ae:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x12,mrrv11a_r12.as_int);
    13b0:	681a      	ldr	r2, [r3, #0]
    13b2:	2112      	movs	r1, #18
    13b4:	f7fe ff15 	bl	1e2 <mbus_remote_register_write>
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv11a_r13.as_int);
    13b8:	4b73      	ldr	r3, [pc, #460]	; (1588 <main+0x4a8>)
    13ba:	2113      	movs	r1, #19
    13bc:	681a      	ldr	r2, [r3, #0]
    13be:	0020      	movs	r0, r4
    13c0:	f7fe ff0f 	bl	1e2 <mbus_remote_register_write>
    mrr_freq_hopping = 5;
    13c4:	2205      	movs	r2, #5
    13c6:	4b72      	ldr	r3, [pc, #456]	; (1590 <main+0x4b0>)
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    13c8:	2106      	movs	r1, #6
    mrr_freq_hopping = 5;
    13ca:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    13cc:	2280      	movs	r2, #128	; 0x80
    mrr_freq_hopping_step = 4;
    13ce:	4b71      	ldr	r3, [pc, #452]	; (1594 <main+0x4b4>)
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    13d0:	0020      	movs	r0, r4
    mrr_freq_hopping_step = 4;
    13d2:	601c      	str	r4, [r3, #0]
    mrr_cfo_val_fine_min = 0x0000;
    13d4:	4b70      	ldr	r3, [pc, #448]	; (1598 <main+0x4b8>)
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    13d6:	0152      	lsls	r2, r2, #5
    mrr_cfo_val_fine_min = 0x0000;
    13d8:	601f      	str	r7, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF
    13da:	f7fe ff02 	bl	1e2 <mbus_remote_register_write>
    mbus_remote_register_write(MRR_ADDR,0x08,0x400000); // RO_POLY
    13de:	2280      	movs	r2, #128	; 0x80
    13e0:	2108      	movs	r1, #8
    13e2:	0020      	movs	r0, r4
    13e4:	03d2      	lsls	r2, r2, #15
    13e6:	f7fe fefc 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r07.RO_MOM = 0x10;
    13ea:	217f      	movs	r1, #127	; 0x7f
    13ec:	4a6b      	ldr	r2, [pc, #428]	; (159c <main+0x4bc>)
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);
    13ee:	0020      	movs	r0, r4
    mrrv11a_r07.RO_MOM = 0x10;
    13f0:	8813      	ldrh	r3, [r2, #0]
    13f2:	438b      	bics	r3, r1
    13f4:	396f      	subs	r1, #111	; 0x6f
    13f6:	430b      	orrs	r3, r1
    13f8:	8013      	strh	r3, [r2, #0]
    mrrv11a_r07.RO_MIM = 0x10;
    13fa:	8813      	ldrh	r3, [r2, #0]
    13fc:	4968      	ldr	r1, [pc, #416]	; (15a0 <main+0x4c0>)
    13fe:	4019      	ands	r1, r3
    1400:	2380      	movs	r3, #128	; 0x80
    1402:	011b      	lsls	r3, r3, #4
    1404:	430b      	orrs	r3, r1
    1406:	8013      	strh	r3, [r2, #0]
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);
    1408:	2107      	movs	r1, #7
    140a:	6812      	ldr	r2, [r2, #0]
    140c:	f7fe fee9 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = 0x0;  //ANT CAP 10b unary 830.5 MHz
    1410:	6833      	ldr	r3, [r6, #0]
    1412:	4a64      	ldr	r2, [pc, #400]	; (15a4 <main+0x4c4>)
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1414:	0039      	movs	r1, r7
    mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = 0x0;  //ANT CAP 10b unary 830.5 MHz
    1416:	4013      	ands	r3, r2
    1418:	6033      	str	r3, [r6, #0]
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    141a:	6832      	ldr	r2, [r6, #0]
    141c:	0020      	movs	r0, r4
    141e:	f7fe fee0 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_COARSE = 0x0; //ANT CAP 10b unary 830.5 MHz
    1422:	4b61      	ldr	r3, [pc, #388]	; (15a8 <main+0x4c8>)
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
    1424:	4e5c      	ldr	r6, [pc, #368]	; (1598 <main+0x4b8>)
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_COARSE = 0x0; //ANT CAP 10b unary 830.5 MHz
    1426:	681a      	ldr	r2, [r3, #0]
    1428:	0a92      	lsrs	r2, r2, #10
    142a:	0292      	lsls	r2, r2, #10
    142c:	601a      	str	r2, [r3, #0]
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
    142e:	6830      	ldr	r0, [r6, #0]
    1430:	263f      	movs	r6, #63	; 0x3f
    1432:	4030      	ands	r0, r6
    1434:	6819      	ldr	r1, [r3, #0]
    1436:	0402      	lsls	r2, r0, #16
    1438:	485c      	ldr	r0, [pc, #368]	; (15ac <main+0x4cc>)
    143a:	4008      	ands	r0, r1
    143c:	4310      	orrs	r0, r2
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
    143e:	4a56      	ldr	r2, [pc, #344]	; (1598 <main+0x4b8>)
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
    1440:	6018      	str	r0, [r3, #0]
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
    1442:	6811      	ldr	r1, [r2, #0]
    1444:	485a      	ldr	r0, [pc, #360]	; (15b0 <main+0x4d0>)
    1446:	681a      	ldr	r2, [r3, #0]
    1448:	4031      	ands	r1, r6
    144a:	0289      	lsls	r1, r1, #10
    144c:	4002      	ands	r2, r0
    144e:	430a      	orrs	r2, r1
    1450:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
    1452:	681a      	ldr	r2, [r3, #0]
    1454:	2101      	movs	r1, #1
    1456:	0020      	movs	r0, r4
    1458:	f7fe fec3 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r02.TX_EN_OW = 0;  // [CISv2] Newly Added; Turn off TX_EN_OW
    145c:	4a55      	ldr	r2, [pc, #340]	; (15b4 <main+0x4d4>)
    145e:	4956      	ldr	r1, [pc, #344]	; (15b8 <main+0x4d8>)
    1460:	6813      	ldr	r3, [r2, #0]
    mbus_remote_register_write(MRR_ADDR,0x02,mrrv11a_r02.as_int);
    1462:	0020      	movs	r0, r4
    mrrv11a_r02.TX_EN_OW = 0;  // [CISv2] Newly Added; Turn off TX_EN_OW
    1464:	400b      	ands	r3, r1
    1466:	6013      	str	r3, [r2, #0]
    mrrv11a_r02.TX_BIAS_TUNE = 0x7FF;  //Set TX BIAS TUNE 13b // Max 0x1FFF
    1468:	6813      	ldr	r3, [r2, #0]
    146a:	4954      	ldr	r1, [pc, #336]	; (15bc <main+0x4dc>)
    146c:	0b5b      	lsrs	r3, r3, #13
    146e:	035b      	lsls	r3, r3, #13
    1470:	430b      	orrs	r3, r1
    1472:	6013      	str	r3, [r2, #0]
    mbus_remote_register_write(MRR_ADDR,0x02,mrrv11a_r02.as_int);
    1474:	2102      	movs	r1, #2
    1476:	6812      	ldr	r2, [r2, #0]
    1478:	f7fe feb3 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r03.TRX_MODE_EN = 0; //Set TRX mode
    147c:	682b      	ldr	r3, [r5, #0]
    147e:	4a50      	ldr	r2, [pc, #320]	; (15c0 <main+0x4e0>)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    1480:	2103      	movs	r1, #3
    mrrv11a_r03.TRX_MODE_EN = 0; //Set TRX mode
    1482:	4013      	ands	r3, r2
    1484:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    1486:	682a      	ldr	r2, [r5, #0]
    1488:	0020      	movs	r0, r4
    148a:	f7fe feaa 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r14.FSM_TX_POWERON_LEN = 0; //3bits
    148e:	4b4d      	ldr	r3, [pc, #308]	; (15c4 <main+0x4e4>)
    1490:	494d      	ldr	r1, [pc, #308]	; (15c8 <main+0x4e8>)
    1492:	681a      	ldr	r2, [r3, #0]
    mrrv11a_r15.FSM_RX_DATA_BITS = 0x00; //Set RX data 1b
    1494:	4d4d      	ldr	r5, [pc, #308]	; (15cc <main+0x4ec>)
    mrrv11a_r14.FSM_TX_POWERON_LEN = 0; //3bits
    1496:	400a      	ands	r2, r1
    1498:	601a      	str	r2, [r3, #0]
    mrrv11a_r15.FSM_RX_DATA_BITS = 0x00; //Set RX data 1b
    149a:	782a      	ldrb	r2, [r5, #0]
    149c:	702f      	strb	r7, [r5, #0]
    mbus_remote_register_write(MRR_ADDR,0x14,mrrv11a_r14.as_int);
    149e:	681a      	ldr	r2, [r3, #0]
    14a0:	2114      	movs	r1, #20
    14a2:	0020      	movs	r0, r4
    14a4:	f7fe fe9d 	bl	1e2 <mbus_remote_register_write>
    mbus_remote_register_write(MRR_ADDR,0x15,mrrv11a_r15.as_int);
    14a8:	682a      	ldr	r2, [r5, #0]
    14aa:	2115      	movs	r1, #21
    14ac:	0020      	movs	r0, r4
    14ae:	f7fe fe98 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r10.FSM_TX_TP_LEN = 80;       // Num. Training Pulses (bit 0); Valid Range: 1 ~ 255 (Default: 80)
    14b2:	4b47      	ldr	r3, [pc, #284]	; (15d0 <main+0x4f0>)
    14b4:	21ff      	movs	r1, #255	; 0xff
    14b6:	681a      	ldr	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x10,mrrv11a_r10.as_int);
    14b8:	0020      	movs	r0, r4
    mrrv11a_r10.FSM_TX_TP_LEN = 80;       // Num. Training Pulses (bit 0); Valid Range: 1 ~ 255 (Default: 80)
    14ba:	0015      	movs	r5, r2
    14bc:	2250      	movs	r2, #80	; 0x50
    14be:	438d      	bics	r5, r1
    14c0:	e088      	b.n	15d4 <main+0x4f4>
    14c2:	46c0      	nop			; (mov r8, r8)
    14c4:	e000e100 	.word	0xe000e100
    14c8:	00000f0d 	.word	0x00000f0d
    14cc:	a000a008 	.word	0xa000a008
    14d0:	00001d54 	.word	0x00001d54
    14d4:	000fffff 	.word	0x000fffff
    14d8:	00001e40 	.word	0x00001e40
    14dc:	00001df4 	.word	0x00001df4
    14e0:	5453104b 	.word	0x5453104b
    14e4:	00001d88 	.word	0x00001d88
    14e8:	ffff9fff 	.word	0xffff9fff
    14ec:	fffc7fff 	.word	0xfffc7fff
    14f0:	ffe3ffff 	.word	0xffe3ffff
    14f4:	fffffc7f 	.word	0xfffffc7f
    14f8:	fffff3ff 	.word	0xfffff3ff
    14fc:	00001001 	.word	0x00001001
    1500:	a000002c 	.word	0xa000002c
    1504:	00001e24 	.word	0x00001e24
    1508:	00001002 	.word	0x00001002
    150c:	00001de0 	.word	0x00001de0
    1510:	00001e3c 	.word	0x00001e3c
    1514:	00001e4c 	.word	0x00001e4c
    1518:	00001dc4 	.word	0x00001dc4
    151c:	00001e10 	.word	0x00001e10
    1520:	00000845 	.word	0x00000845
    1524:	0017c7ff 	.word	0x0017c7ff
    1528:	00103800 	.word	0x00103800
    152c:	0017efff 	.word	0x0017efff
    1530:	00001d90 	.word	0x00001d90
    1534:	00001d94 	.word	0x00001d94
    1538:	fffffe3f 	.word	0xfffffe3f
    153c:	00001e28 	.word	0x00001e28
    1540:	ff1fffff 	.word	0xff1fffff
    1544:	00001da0 	.word	0x00001da0
    1548:	ffefffff 	.word	0xffefffff
    154c:	ffe0007f 	.word	0xffe0007f
    1550:	ffdfffff 	.word	0xffdfffff
    1554:	001ff780 	.word	0x001ff780
    1558:	00001d9c 	.word	0x00001d9c
    155c:	ffff00ff 	.word	0xffff00ff
    1560:	00001da4 	.word	0x00001da4
    1564:	00001d80 	.word	0x00001d80
    1568:	fffffbff 	.word	0xfffffbff
    156c:	00001d60 	.word	0x00001d60
    1570:	00001d58 	.word	0x00001d58
    1574:	0044aa20 	.word	0x0044aa20
    1578:	a000007c 	.word	0xa000007c
    157c:	00009c40 	.word	0x00009c40
    1580:	00001d7c 	.word	0x00001d7c
    1584:	00001d74 	.word	0x00001d74
    1588:	00001d78 	.word	0x00001d78
    158c:	fff003ff 	.word	0xfff003ff
    1590:	00001ddc 	.word	0x00001ddc
    1594:	00001e58 	.word	0x00001e58
    1598:	00001db8 	.word	0x00001db8
    159c:	00001d68 	.word	0x00001d68
    15a0:	ffffc07f 	.word	0xffffc07f
    15a4:	fffe007f 	.word	0xfffe007f
    15a8:	00001e68 	.word	0x00001e68
    15ac:	ffc0ffff 	.word	0xffc0ffff
    15b0:	ffff03ff 	.word	0xffff03ff
    15b4:	00001d5c 	.word	0x00001d5c
    15b8:	ffffdfff 	.word	0xffffdfff
    15bc:	000007ff 	.word	0x000007ff
    15c0:	ffffbfff 	.word	0xffffbfff
    15c4:	00001e6c 	.word	0x00001e6c
    15c8:	fff8ffff 	.word	0xfff8ffff
    15cc:	00001e70 	.word	0x00001e70
    15d0:	00001d6c 	.word	0x00001d6c
    15d4:	432a      	orrs	r2, r5
    15d6:	601a      	str	r2, [r3, #0]
    mrrv11a_r10.FSM_TX_PASSCODE = 0x7AC8; // 16-bit Passcode (sent from LSB to MSB)
    15d8:	681a      	ldr	r2, [r3, #0]
    15da:	49e4      	ldr	r1, [pc, #912]	; (196c <main+0x88c>)
    15dc:	4011      	ands	r1, r2
    15de:	4ae4      	ldr	r2, [pc, #912]	; (1970 <main+0x890>)
    15e0:	430a      	orrs	r2, r1
    15e2:	601a      	str	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x10,mrrv11a_r10.as_int);
    15e4:	681a      	ldr	r2, [r3, #0]
    15e6:	2110      	movs	r1, #16
    15e8:	f7fe fdfb 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r11.FSM_TX_DATA_BITS = RADIO_DATA_NUM_WORDS * 32;   // [CISv2] RADIO_DATA_LENGTH -> RADIO_DATA_NUM_WORDS * 32
    15ec:	49e1      	ldr	r1, [pc, #900]	; (1974 <main+0x894>)
    15ee:	4ae2      	ldr	r2, [pc, #904]	; (1978 <main+0x898>)
    15f0:	680b      	ldr	r3, [r1, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    15f2:	0020      	movs	r0, r4
    mrrv11a_r11.FSM_TX_DATA_BITS = RADIO_DATA_NUM_WORDS * 32;   // [CISv2] RADIO_DATA_LENGTH -> RADIO_DATA_NUM_WORDS * 32
    15f4:	401a      	ands	r2, r3
    15f6:	23c0      	movs	r3, #192	; 0xc0
    15f8:	01db      	lsls	r3, r3, #7
    15fa:	4313      	orrs	r3, r2
    15fc:	600b      	str	r3, [r1, #0]
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    15fe:	680a      	ldr	r2, [r1, #0]
    1600:	2111      	movs	r1, #17
    1602:	f7fe fdee 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r2A.SEL_V1P2_BIAS = 0;
    1606:	2001      	movs	r0, #1
    1608:	4bdc      	ldr	r3, [pc, #880]	; (197c <main+0x89c>)
    160a:	781a      	ldrb	r2, [r3, #0]
    160c:	4382      	bics	r2, r0
    160e:	701a      	strb	r2, [r3, #0]
    mrrv11a_r2A.SEL_VLDO_BIAS = 1;
    1610:	2202      	movs	r2, #2
    1612:	7819      	ldrb	r1, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x2A,mrrv11a_r2A.as_int);
    1614:	0020      	movs	r0, r4
    mrrv11a_r2A.SEL_VLDO_BIAS = 1;
    1616:	430a      	orrs	r2, r1
    1618:	701a      	strb	r2, [r3, #0]
    mbus_remote_register_write(MRR_ADDR,0x2A,mrrv11a_r2A.as_int);
    161a:	681a      	ldr	r2, [r3, #0]
    161c:	212a      	movs	r1, #42	; 0x2a
    161e:	f7fe fde0 	bl	1e2 <mbus_remote_register_write>
    mrrv11a_r04.LDO_SEL_VOUT = 4; // New for MRRv11A
    1622:	4ad7      	ldr	r2, [pc, #860]	; (1980 <main+0x8a0>)
    1624:	49d7      	ldr	r1, [pc, #860]	; (1984 <main+0x8a4>)
    1626:	6813      	ldr	r3, [r2, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    1628:	0020      	movs	r0, r4
    mrrv11a_r04.LDO_SEL_VOUT = 4; // New for MRRv11A
    162a:	400b      	ands	r3, r1
    162c:	2180      	movs	r1, #128	; 0x80
    162e:	0209      	lsls	r1, r1, #8
    1630:	430b      	orrs	r3, r1
    1632:	6013      	str	r3, [r2, #0]
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    1634:	0021      	movs	r1, r4
    1636:	6812      	ldr	r2, [r2, #0]
    1638:	f7fe fdd3 	bl	1e2 <mbus_remote_register_write>
    config_timerwd(TIMERWD_VAL);
    163c:	48d2      	ldr	r0, [pc, #840]	; (1988 <main+0x8a8>)
    163e:	f7fe fd55 	bl	ec <config_timerwd>
    *REG_MBUS_WD = 1500000; // default: 1500000
    1642:	4bd2      	ldr	r3, [pc, #840]	; (198c <main+0x8ac>)
    1644:	4ad2      	ldr	r2, [pc, #840]	; (1990 <main+0x8b0>)
    delay(MBUS_DELAY*200); // Wait for decap to charge
    1646:	48d3      	ldr	r0, [pc, #844]	; (1994 <main+0x8b4>)
    *REG_MBUS_WD = 1500000; // default: 1500000
    1648:	6013      	str	r3, [r2, #0]
    delay(MBUS_DELAY*200); // Wait for decap to charge
    164a:	f7fe fd2d 	bl	a8 <delay>
    RADIO_PACKET_DELAY = 4000;
    164e:	22fa      	movs	r2, #250	; 0xfa
    sns_running = 0;
    1650:	4bd1      	ldr	r3, [pc, #836]	; (1998 <main+0x8b8>)
    RADIO_PACKET_DELAY = 4000;
    1652:	0112      	lsls	r2, r2, #4
    sns_running = 0;
    1654:	601f      	str	r7, [r3, #0]
    radio_ready = 0;
    1656:	4bd1      	ldr	r3, [pc, #836]	; (199c <main+0x8bc>)
    PMU_25C_threshold_sns = 1300; // Around 20C
    1658:	49d1      	ldr	r1, [pc, #836]	; (19a0 <main+0x8c0>)
    radio_ready = 0;
    165a:	601f      	str	r7, [r3, #0]
    radio_on = 0;
    165c:	4bd1      	ldr	r3, [pc, #836]	; (19a4 <main+0x8c4>)
    165e:	601f      	str	r7, [r3, #0]
    wakeup_data = 0;
    1660:	4bd1      	ldr	r3, [pc, #836]	; (19a8 <main+0x8c8>)
    1662:	601f      	str	r7, [r3, #0]
    RADIO_PACKET_DELAY = 4000;
    1664:	4bd1      	ldr	r3, [pc, #836]	; (19ac <main+0x8cc>)
    1666:	601a      	str	r2, [r3, #0]
    PMU_10C_threshold_sns = 600; // Around 10C
    1668:	2296      	movs	r2, #150	; 0x96
    radio_packet_count = 0;
    166a:	4bd1      	ldr	r3, [pc, #836]	; (19b0 <main+0x8d0>)
    PMU_10C_threshold_sns = 600; // Around 10C
    166c:	0092      	lsls	r2, r2, #2
    radio_packet_count = 0;
    166e:	601f      	str	r7, [r3, #0]
    error_code = 0;
    1670:	4bd0      	ldr	r3, [pc, #832]	; (19b4 <main+0x8d4>)
    1672:	601f      	str	r7, [r3, #0]
    PMU_10C_threshold_sns = 600; // Around 10C
    1674:	4bd0      	ldr	r3, [pc, #832]	; (19b8 <main+0x8d8>)
    1676:	601a      	str	r2, [r3, #0]
    PMU_15C_threshold_sns = 800; 
    1678:	4bd0      	ldr	r3, [pc, #832]	; (19bc <main+0x8dc>)
    167a:	32c8      	adds	r2, #200	; 0xc8
    167c:	601a      	str	r2, [r3, #0]
    PMU_20C_threshold_sns = 1000; // Around 20C
    167e:	23fa      	movs	r3, #250	; 0xfa
    1680:	4acf      	ldr	r2, [pc, #828]	; (19c0 <main+0x8e0>)
    1682:	009b      	lsls	r3, r3, #2
    1684:	6013      	str	r3, [r2, #0]
    PMU_25C_threshold_sns = 1300; // Around 20C
    1686:	4acf      	ldr	r2, [pc, #828]	; (19c4 <main+0x8e4>)
    1688:	6011      	str	r1, [r2, #0]
    PMU_30C_threshold_sns = 1600; 
    168a:	4acf      	ldr	r2, [pc, #828]	; (19c8 <main+0x8e8>)
    168c:	312d      	adds	r1, #45	; 0x2d
    168e:	31ff      	adds	r1, #255	; 0xff
    1690:	6011      	str	r1, [r2, #0]
    PMU_35C_threshold_sns = 1900; // Around 35C
    1692:	4ace      	ldr	r2, [pc, #824]	; (19cc <main+0x8ec>)
    1694:	49ce      	ldr	r1, [pc, #824]	; (19d0 <main+0x8f0>)
    1696:	6011      	str	r1, [r2, #0]
    PMU_40C_threshold_sns = 2200;
    1698:	4ace      	ldr	r2, [pc, #824]	; (19d4 <main+0x8f4>)
    169a:	49cf      	ldr	r1, [pc, #828]	; (19d8 <main+0x8f8>)
    169c:	6011      	str	r1, [r2, #0]
    PMU_45C_threshold_sns = 2500;
    169e:	4acf      	ldr	r2, [pc, #828]	; (19dc <main+0x8fc>)
    16a0:	49cf      	ldr	r1, [pc, #828]	; (19e0 <main+0x900>)
    16a2:	6011      	str	r1, [r2, #0]
    PMU_55C_threshold_sns = 3200; // Around 55C
    16a4:	21c8      	movs	r1, #200	; 0xc8
    16a6:	4acf      	ldr	r2, [pc, #828]	; (19e4 <main+0x904>)
    16a8:	0109      	lsls	r1, r1, #4
    16aa:	6011      	str	r1, [r2, #0]
    PMU_65C_threshold_sns = 4500; 
    16ac:	4ace      	ldr	r2, [pc, #824]	; (19e8 <main+0x908>)
    16ae:	49cf      	ldr	r1, [pc, #828]	; (19ec <main+0x90c>)
    16b0:	6011      	str	r1, [r2, #0]
    PMU_75C_threshold_sns = 7000; // Around 75C
    16b2:	4acf      	ldr	r2, [pc, #828]	; (19f0 <main+0x910>)
    16b4:	49cf      	ldr	r1, [pc, #828]	; (19f4 <main+0x914>)
    16b6:	6011      	str	r1, [r2, #0]
    PMU_85C_threshold_sns = 9400; 
    16b8:	4acf      	ldr	r2, [pc, #828]	; (19f8 <main+0x918>)
    16ba:	49d0      	ldr	r1, [pc, #832]	; (19fc <main+0x91c>)
    16bc:	6011      	str	r1, [r2, #0]
    PMU_95C_threshold_sns = 12000; // Around 95C
    16be:	4ad0      	ldr	r2, [pc, #832]	; (1a00 <main+0x920>)
    16c0:	49d0      	ldr	r1, [pc, #832]	; (1a04 <main+0x924>)
    16c2:	6011      	str	r1, [r2, #0]
    SNT_0P5S_VAL = 1000;
    16c4:	4ad0      	ldr	r2, [pc, #832]	; (1a08 <main+0x928>)
    16c6:	6013      	str	r3, [r2, #0]
    TEMP_CALIB_A = 240000;
    16c8:	4bd0      	ldr	r3, [pc, #832]	; (1a0c <main+0x92c>)
    16ca:	4ad1      	ldr	r2, [pc, #836]	; (1a10 <main+0x930>)
    16cc:	601a      	str	r2, [r3, #0]
    TEMP_CALIB_B = 20000; // (B-3400)*100
    16ce:	4bd1      	ldr	r3, [pc, #836]	; (1a14 <main+0x934>)
    16d0:	4ad1      	ldr	r2, [pc, #836]	; (1a18 <main+0x938>)
    16d2:	601a      	str	r2, [r3, #0]
            ii++;
        }
            send_radio_data_mrr(1,0x4,ii);  

        mrr_freq_hopping = mrr_freq_hopping_saved;
        operation_sleep_notimer();
    16d4:	f7ff f96a 	bl	9ac <operation_sleep_notimer>
    pmu_adc_read_latest();
    16d8:	f7ff f8fc 	bl	8d4 <pmu_adc_read_latest>
    wakeup_data = *GOC_DATA_IRQ;
    16dc:	238c      	movs	r3, #140	; 0x8c
    16de:	4cb2      	ldr	r4, [pc, #712]	; (19a8 <main+0x8c8>)
    16e0:	681b      	ldr	r3, [r3, #0]
    16e2:	6023      	str	r3, [r4, #0]
    uint32_t wakeup_data_header = (wakeup_data>>24) & 0xFF;
    16e4:	6825      	ldr	r5, [r4, #0]
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;
    16e6:	6823      	ldr	r3, [r4, #0]
    uint32_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
    16e8:	6827      	ldr	r7, [r4, #0]
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;
    16ea:	9300      	str	r3, [sp, #0]
    uint32_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;
    16ec:	6823      	ldr	r3, [r4, #0]
    uint32_t wakeup_data_header = (wakeup_data>>24) & 0xFF;
    16ee:	0e2d      	lsrs	r5, r5, #24
    uint32_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;
    16f0:	9301      	str	r3, [sp, #4]
    if ((wakeup_data_header != 0) && (exec_count_irq == 0)){
    16f2:	d100      	bne.n	16f6 <main+0x616>
    16f4:	e0bf      	b.n	1876 <main+0x796>
    16f6:	4bc9      	ldr	r3, [pc, #804]	; (1a1c <main+0x93c>)
    16f8:	681e      	ldr	r6, [r3, #0]
    16fa:	2e00      	cmp	r6, #0
    16fc:	d111      	bne.n	1722 <main+0x642>
    set_halt_until_mbus_tx();
    16fe:	f7fe fd25 	bl	14c <set_halt_until_mbus_tx>
    if (sns_running){
    1702:	4ba5      	ldr	r3, [pc, #660]	; (1998 <main+0x8b8>)
    1704:	681a      	ldr	r2, [r3, #0]
    1706:	2a00      	cmp	r2, #0
    1708:	d006      	beq.n	1718 <main+0x638>
        sns_running = 0;
    170a:	601e      	str	r6, [r3, #0]
        temp_sensor_power_off();
    170c:	f7fe fd96 	bl	23c <temp_sensor_power_off>
        sns_ldo_power_off();
    1710:	f7fe fdae 	bl	270 <sns_ldo_power_off>
        pmu_sleep_setting_temp_based();
    1714:	f7ff f880 	bl	818 <pmu_sleep_setting_temp_based>
    stack_state = STK_IDLE;
    1718:	2200      	movs	r2, #0
    171a:	4bc1      	ldr	r3, [pc, #772]	; (1a20 <main+0x940>)
    171c:	601a      	str	r2, [r3, #0]
    radio_power_off();
    171e:	f7fe febf 	bl	4a0 <radio_power_off>
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;
    1722:	20ff      	movs	r0, #255	; 0xff
    uint32_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;
    1724:	9b01      	ldr	r3, [sp, #4]
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;
    1726:	9e00      	ldr	r6, [sp, #0]
    uint32_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
    1728:	0a3f      	lsrs	r7, r7, #8
    uint32_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;
    172a:	0c1b      	lsrs	r3, r3, #16
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;
    172c:	4006      	ands	r6, r0
    uint32_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
    172e:	4007      	ands	r7, r0
    uint32_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;
    1730:	4018      	ands	r0, r3
    if(wakeup_data_header == 1){
    1732:	2d01      	cmp	r5, #1
    1734:	d107      	bne.n	1746 <main+0x666>
        operation_goc_trigger_radio(wakeup_data_field_0 + (wakeup_data_field_2<<8), wakeup_data_field_1, 0x4, exec_count_irq);
    1736:	2204      	movs	r2, #4
    1738:	0039      	movs	r1, r7
    173a:	4bb8      	ldr	r3, [pc, #736]	; (1a1c <main+0x93c>)
    173c:	0200      	lsls	r0, r0, #8
    173e:	681b      	ldr	r3, [r3, #0]
    1740:	1980      	adds	r0, r0, r6
        // Store the Chip ID
        puf_chip_id = *REG_PUF_CHIP_ID;
        // Power-Off PUF
        *REG_SYS_CONF = (0x1/*PUF_SLEEP*/ << 6) | (0x1/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);

        operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xC, puf_chip_id);
    1742:	f7ff faf1 	bl	d28 <operation_goc_trigger_radio>
    }else if(wakeup_data_header == 0x32){
    1746:	2d32      	cmp	r5, #50	; 0x32
    1748:	d000      	beq.n	174c <main+0x66c>
    174a:	e09c      	b.n	1886 <main+0x7a6>
        temp_storage_debug = wakeup_data_field_2 & 0x80;
    174c:	2280      	movs	r2, #128	; 0x80
    174e:	4013      	ands	r3, r2
    1750:	4ab4      	ldr	r2, [pc, #720]	; (1a24 <main+0x944>)
        WAKEUP_PERIOD_SNT = ((wakeup_data & 0xFFFF)<<1)*SNT_0P5S_VAL; // Unit is 0.5s
    1752:	49b5      	ldr	r1, [pc, #724]	; (1a28 <main+0x948>)
        temp_storage_debug = wakeup_data_field_2 & 0x80;
    1754:	6013      	str	r3, [r2, #0]
        WAKEUP_PERIOD_SNT = ((wakeup_data & 0xFFFF)<<1)*SNT_0P5S_VAL; // Unit is 0.5s
    1756:	6823      	ldr	r3, [r4, #0]
    1758:	4aab      	ldr	r2, [pc, #684]	; (1a08 <main+0x928>)
    175a:	005b      	lsls	r3, r3, #1
    175c:	6812      	ldr	r2, [r2, #0]
    175e:	400b      	ands	r3, r1
    1760:	4353      	muls	r3, r2
    1762:	4ab2      	ldr	r2, [pc, #712]	; (1a2c <main+0x94c>)
        exec_count_irq++;
    1764:	4cad      	ldr	r4, [pc, #692]	; (1a1c <main+0x93c>)
        WAKEUP_PERIOD_SNT = ((wakeup_data & 0xFFFF)<<1)*SNT_0P5S_VAL; // Unit is 0.5s
    1766:	6013      	str	r3, [r2, #0]
        exec_count_irq++;
    1768:	6823      	ldr	r3, [r4, #0]
    176a:	3301      	adds	r3, #1
    176c:	6023      	str	r3, [r4, #0]
        if (exec_count_irq == 1){
    176e:	6826      	ldr	r6, [r4, #0]
    1770:	2e01      	cmp	r6, #1
    1772:	d158      	bne.n	1826 <main+0x746>
    pmu_set_sleep_clk(0xF,0xA,0x5,0xF/*V1P2*/);
    1774:	230f      	movs	r3, #15
    1776:	2205      	movs	r2, #5
    1778:	0018      	movs	r0, r3
    177a:	210a      	movs	r1, #10
    177c:	f7ff f828 	bl	7d0 <pmu_set_sleep_clk>
    sntv5_r08.TMR_SLEEP = 0x0; // Default : 0x1
    1780:	2240      	movs	r2, #64	; 0x40
    1782:	4cab      	ldr	r4, [pc, #684]	; (1a30 <main+0x950>)
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    1784:	2108      	movs	r1, #8
    sntv5_r08.TMR_SLEEP = 0x0; // Default : 0x1
    1786:	7823      	ldrb	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    1788:	2005      	movs	r0, #5
    sntv5_r08.TMR_SLEEP = 0x0; // Default : 0x1
    178a:	4393      	bics	r3, r2
    178c:	7023      	strb	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    178e:	6822      	ldr	r2, [r4, #0]
    1790:	f7fe fd27 	bl	1e2 <mbus_remote_register_write>
    sntv5_r08.TMR_ISOLATE = 0x0; // Default : 0x1
    1794:	2220      	movs	r2, #32
    sntv5_r08.TMR_EN_OSC = 0x1; // Default : 0x0
    1796:	2708      	movs	r7, #8
    sntv5_r08.TMR_ISOLATE = 0x0; // Default : 0x1
    1798:	7823      	ldrb	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    179a:	2108      	movs	r1, #8
    sntv5_r08.TMR_ISOLATE = 0x0; // Default : 0x1
    179c:	4393      	bics	r3, r2
    179e:	7023      	strb	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    17a0:	2005      	movs	r0, #5
    17a2:	6822      	ldr	r2, [r4, #0]
    17a4:	f7fe fd1d 	bl	1e2 <mbus_remote_register_write>
    sntv5_r09.TMR_SELF_EN = 0x0; // Default : 0x1
    17a8:	4da2      	ldr	r5, [pc, #648]	; (1a34 <main+0x954>)
    17aa:	4aa3      	ldr	r2, [pc, #652]	; (1a38 <main+0x958>)
    17ac:	682b      	ldr	r3, [r5, #0]
    mbus_remote_register_write(SNT_ADDR,0x09,sntv5_r09.as_int);
    17ae:	2109      	movs	r1, #9
    sntv5_r09.TMR_SELF_EN = 0x0; // Default : 0x1
    17b0:	4013      	ands	r3, r2
    17b2:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(SNT_ADDR,0x09,sntv5_r09.as_int);
    17b4:	2005      	movs	r0, #5
    17b6:	682a      	ldr	r2, [r5, #0]
    17b8:	f7fe fd13 	bl	1e2 <mbus_remote_register_write>
    sntv5_r08.TMR_EN_OSC = 0x1; // Default : 0x0
    17bc:	7823      	ldrb	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    17be:	0039      	movs	r1, r7
    sntv5_r08.TMR_EN_OSC = 0x1; // Default : 0x0
    17c0:	433b      	orrs	r3, r7
    17c2:	7023      	strb	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    17c4:	2005      	movs	r0, #5
    17c6:	6822      	ldr	r2, [r4, #0]
    17c8:	f7fe fd0b 	bl	1e2 <mbus_remote_register_write>
    sntv5_r08.TMR_RESETB = 0x1; // Default : 0x0
    17cc:	2310      	movs	r3, #16
    17ce:	7822      	ldrb	r2, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    17d0:	0039      	movs	r1, r7
    sntv5_r08.TMR_RESETB = 0x1; // Default : 0x0
    17d2:	4313      	orrs	r3, r2
    17d4:	7023      	strb	r3, [r4, #0]
    sntv5_r08.TMR_RESETB_DIV = 0x1; // Default : 0x0
    17d6:	2304      	movs	r3, #4
    17d8:	7822      	ldrb	r2, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    17da:	2005      	movs	r0, #5
    sntv5_r08.TMR_RESETB_DIV = 0x1; // Default : 0x0
    17dc:	4313      	orrs	r3, r2
    17de:	7023      	strb	r3, [r4, #0]
    sntv5_r08.TMR_RESETB_DCDC = 0x1; // Default : 0x0
    17e0:	2302      	movs	r3, #2
    17e2:	7822      	ldrb	r2, [r4, #0]
    17e4:	4313      	orrs	r3, r2
    17e6:	7023      	strb	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    17e8:	6822      	ldr	r2, [r4, #0]
    17ea:	f7fe fcfa 	bl	1e2 <mbus_remote_register_write>
    sntv5_r08.TMR_EN_SELF_CLK = 0x1; // Default : 0x0
    17ee:	7823      	ldrb	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    17f0:	0039      	movs	r1, r7
    sntv5_r08.TMR_EN_SELF_CLK = 0x1; // Default : 0x0
    17f2:	4333      	orrs	r3, r6
    17f4:	7023      	strb	r3, [r4, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    17f6:	2005      	movs	r0, #5
    17f8:	6822      	ldr	r2, [r4, #0]
    17fa:	f7fe fcf2 	bl	1e2 <mbus_remote_register_write>
    sntv5_r09.TMR_SELF_EN = 0x1; // Default : 0x0
    17fe:	2380      	movs	r3, #128	; 0x80
    1800:	682a      	ldr	r2, [r5, #0]
    1802:	039b      	lsls	r3, r3, #14
    1804:	4313      	orrs	r3, r2
    1806:	602b      	str	r3, [r5, #0]
    mbus_remote_register_write(SNT_ADDR,0x09,sntv5_r09.as_int);
    1808:	2109      	movs	r1, #9
    180a:	2005      	movs	r0, #5
    180c:	682a      	ldr	r2, [r5, #0]
    180e:	f7fe fce8 	bl	1e2 <mbus_remote_register_write>
    snt_timer_enabled = 1;
    1812:	4b8a      	ldr	r3, [pc, #552]	; (1a3c <main+0x95c>)
            set_wakeup_timer (WAKEUP_PERIOD_RADIO_INIT*2, 0x1, 0x1);
    1814:	0032      	movs	r2, r6
    1816:	0031      	movs	r1, r6
    1818:	2014      	movs	r0, #20
    snt_timer_enabled = 1;
    181a:	601e      	str	r6, [r3, #0]
            set_wakeup_timer (WAKEUP_PERIOD_RADIO_INIT*2, 0x1, 0x1);
    181c:	f7fe fc78 	bl	110 <set_wakeup_timer>
    mbus_sleep_all();
    1820:	f7fe fcc8 	bl	1b4 <mbus_sleep_all>
    while(1);
    1824:	e7fe      	b.n	1824 <main+0x744>
        }else if (exec_count_irq == 2){
    1826:	6823      	ldr	r3, [r4, #0]
    1828:	2b02      	cmp	r3, #2
    182a:	d10c      	bne.n	1846 <main+0x766>
    sntv5_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    182c:	2108      	movs	r1, #8
    182e:	4b80      	ldr	r3, [pc, #512]	; (1a30 <main+0x950>)
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    1830:	2005      	movs	r0, #5
    sntv5_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    1832:	781a      	ldrb	r2, [r3, #0]
    1834:	438a      	bics	r2, r1
    1836:	701a      	strb	r2, [r3, #0]
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    1838:	681a      	ldr	r2, [r3, #0]
    183a:	f7fe fcd2 	bl	1e2 <mbus_remote_register_write>
            snt_read_wup_counter();
    183e:	f7fe fef5 	bl	62c <snt_read_wup_counter>
            pmu_set_sleep_low();
    1842:	f7fe ffe1 	bl	808 <pmu_set_sleep_low>
        radio_power_on();
    1846:	f7fe fd67 	bl	318 <radio_power_on>
        send_radio_data_mrr(1,0x5,0x0); 
    184a:	2200      	movs	r2, #0
    184c:	2105      	movs	r1, #5
    184e:	2001      	movs	r0, #1
    1850:	f7ff f968 	bl	b24 <send_radio_data_mrr>
        sns_running = 1;
    1854:	2201      	movs	r2, #1
    1856:	4b50      	ldr	r3, [pc, #320]	; (1998 <main+0x8b8>)
    1858:	601a      	str	r2, [r3, #0]
        exec_count = 0;
    185a:	2300      	movs	r3, #0
    185c:	4a78      	ldr	r2, [pc, #480]	; (1a40 <main+0x960>)
    185e:	6013      	str	r3, [r2, #0]
        meas_count = 0;
    1860:	4a78      	ldr	r2, [pc, #480]	; (1a44 <main+0x964>)
    1862:	6013      	str	r3, [r2, #0]
        temp_storage_count = 0;
    1864:	4a78      	ldr	r2, [pc, #480]	; (1a48 <main+0x968>)
    1866:	6013      	str	r3, [r2, #0]
        *GOC_DATA_IRQ = 0;
    1868:	228c      	movs	r2, #140	; 0x8c
    186a:	6013      	str	r3, [r2, #0]
        stack_state = STK_IDLE;
    186c:	4a6c      	ldr	r2, [pc, #432]	; (1a20 <main+0x940>)
        exec_count_irq = 0;
    186e:	6023      	str	r3, [r4, #0]
        stack_state = STK_IDLE;
    1870:	6013      	str	r3, [r2, #0]
        operation_sns_run();
    1872:	f7ff fa7b 	bl	d6c <operation_sns_run>
            // Go to sleep without timer
            operation_sleep_notimer();
        }
    }

    if (sns_running){
    1876:	4b48      	ldr	r3, [pc, #288]	; (1998 <main+0x8b8>)
    1878:	681b      	ldr	r3, [r3, #0]
    187a:	2b00      	cmp	r3, #0
    187c:	d100      	bne.n	1880 <main+0x7a0>
    187e:	e729      	b.n	16d4 <main+0x5f4>
        // Proceed to continuous mode
        while(1){
            operation_sns_run();
    1880:	f7ff fa74 	bl	d6c <operation_sns_run>
        while(1){
    1884:	e7fc      	b.n	1880 <main+0x7a0>
    }else if(wakeup_data_header == 0x33){
    1886:	2d33      	cmp	r5, #51	; 0x33
    1888:	d10c      	bne.n	18a4 <main+0x7c4>
        operation_sns_sleep_check();
    188a:	f7fe fff9 	bl	880 <operation_sns_sleep_check>
        snt_stop_timer();
    188e:	f7fe fd05 	bl	29c <snt_stop_timer>
        stack_state = STK_IDLE;
    1892:	2200      	movs	r2, #0
        operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0x6, exec_count);
    1894:	0039      	movs	r1, r7
        stack_state = STK_IDLE;
    1896:	4b62      	ldr	r3, [pc, #392]	; (1a20 <main+0x940>)
    1898:	601a      	str	r2, [r3, #0]
        operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0x6, exec_count);
    189a:	4b69      	ldr	r3, [pc, #420]	; (1a40 <main+0x960>)
    189c:	3206      	adds	r2, #6
    189e:	681b      	ldr	r3, [r3, #0]
        operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xC, puf_chip_id);
    18a0:	0030      	movs	r0, r6
    18a2:	e74e      	b.n	1742 <main+0x662>
    }else if(wakeup_data_header == 0x13){
    18a4:	2d13      	cmp	r5, #19
    18a6:	d10e      	bne.n	18c6 <main+0x7e6>
        sntv5_r0A.TMR_DIFF_CON = wakeup_data & 0x3FFF; // Default: 0x3FFF
    18a8:	4968      	ldr	r1, [pc, #416]	; (1a4c <main+0x96c>)
    18aa:	6822      	ldr	r2, [r4, #0]
    18ac:	4868      	ldr	r0, [pc, #416]	; (1a50 <main+0x970>)
    18ae:	680b      	ldr	r3, [r1, #0]
    18b0:	0492      	lsls	r2, r2, #18
    18b2:	0ad2      	lsrs	r2, r2, #11
    18b4:	4003      	ands	r3, r0
    18b6:	4313      	orrs	r3, r2
    18b8:	600b      	str	r3, [r1, #0]
        mbus_remote_register_write(SNT_ADDR,0x0A,sntv5_r0A.as_int);
    18ba:	680a      	ldr	r2, [r1, #0]
    18bc:	2005      	movs	r0, #5
    18be:	210a      	movs	r1, #10
        mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
    18c0:	f7fe fc8f 	bl	1e2 <mbus_remote_register_write>
    18c4:	e7d7      	b.n	1876 <main+0x796>
    }else if(wakeup_data_header == 0x14){
    18c6:	2d14      	cmp	r5, #20
    18c8:	d10a      	bne.n	18e0 <main+0x800>
        SNT_0P5S_VAL = wakeup_data & 0xFFFF;
    18ca:	6822      	ldr	r2, [r4, #0]
    18cc:	4b4e      	ldr	r3, [pc, #312]	; (1a08 <main+0x928>)
    18ce:	b292      	uxth	r2, r2
    18d0:	601a      	str	r2, [r3, #0]
        if (SNT_0P5S_VAL == 0){
    18d2:	681a      	ldr	r2, [r3, #0]
    18d4:	2a00      	cmp	r2, #0
    18d6:	d1ce      	bne.n	1876 <main+0x796>
            SNT_0P5S_VAL = 1000;
    18d8:	22fa      	movs	r2, #250	; 0xfa
    18da:	0092      	lsls	r2, r2, #2
            PMU_ADC_3P0_VAL = read_data_batadc;
    18dc:	601a      	str	r2, [r3, #0]
    18de:	e7ca      	b.n	1876 <main+0x796>
    }else if(wakeup_data_header == 0x15){
    18e0:	2d15      	cmp	r5, #21
    18e2:	d115      	bne.n	1910 <main+0x830>
        prev21e_r0B.GOC_CLK_GEN_SEL_FREQ = (wakeup_data >> 4)&0x7; // Default 0x7
    18e4:	4a5b      	ldr	r2, [pc, #364]	; (1a54 <main+0x974>)
    18e6:	6823      	ldr	r3, [r4, #0]
    18e8:	485b      	ldr	r0, [pc, #364]	; (1a58 <main+0x978>)
    18ea:	6811      	ldr	r1, [r2, #0]
    18ec:	065b      	lsls	r3, r3, #25
    18ee:	4001      	ands	r1, r0
        prev21e_r0B.GOC_CLK_GEN_SEL_DIV = wakeup_data & 0x3; // Default 0x1
    18f0:	2003      	movs	r0, #3
        prev21e_r0B.GOC_CLK_GEN_SEL_FREQ = (wakeup_data >> 4)&0x7; // Default 0x7
    18f2:	0f5b      	lsrs	r3, r3, #29
    18f4:	01db      	lsls	r3, r3, #7
    18f6:	430b      	orrs	r3, r1
    18f8:	6013      	str	r3, [r2, #0]
        prev21e_r0B.GOC_CLK_GEN_SEL_DIV = wakeup_data & 0x3; // Default 0x1
    18fa:	6821      	ldr	r1, [r4, #0]
    18fc:	6813      	ldr	r3, [r2, #0]
    18fe:	4001      	ands	r1, r0
    1900:	4856      	ldr	r0, [pc, #344]	; (1a5c <main+0x97c>)
    1902:	0289      	lsls	r1, r1, #10
    1904:	4003      	ands	r3, r0
    1906:	430b      	orrs	r3, r1
    1908:	6013      	str	r3, [r2, #0]
        *REG_CLKGEN_TUNE = prev21e_r0B.as_int;
    190a:	6812      	ldr	r2, [r2, #0]
    190c:	4b54      	ldr	r3, [pc, #336]	; (1a60 <main+0x980>)
    190e:	e7e5      	b.n	18dc <main+0x7fc>
    }else if(wakeup_data_header == 0x17){
    1910:	2d17      	cmp	r5, #23
    1912:	d107      	bne.n	1924 <main+0x844>
        if (wakeup_data_field_0 == 0){
    1914:	4b53      	ldr	r3, [pc, #332]	; (1a64 <main+0x984>)
    1916:	2e00      	cmp	r6, #0
    1918:	d102      	bne.n	1920 <main+0x840>
            PMU_ADC_3P0_VAL = read_data_batadc;
    191a:	4a53      	ldr	r2, [pc, #332]	; (1a68 <main+0x988>)
    191c:	6812      	ldr	r2, [r2, #0]
    191e:	e7dd      	b.n	18dc <main+0x7fc>
            PMU_ADC_3P0_VAL = wakeup_data_field_0;
    1920:	601e      	str	r6, [r3, #0]
    1922:	e7a8      	b.n	1876 <main+0x796>
    }else if(wakeup_data_header == 0x18){
    1924:	2d18      	cmp	r5, #24
    1926:	d103      	bne.n	1930 <main+0x850>
        pmu_set_sar_override(wakeup_data_field_0);
    1928:	0030      	movs	r0, r6
    192a:	f7fe ffc3 	bl	8b4 <pmu_set_sar_override>
    192e:	e7a2      	b.n	1876 <main+0x796>
    }else if(wakeup_data_header == 0x20){
    1930:	2d20      	cmp	r5, #32
    1932:	d10b      	bne.n	194c <main+0x86c>
        chip_id_user = (wakeup_data>>8) & 0xFFFF;
    1934:	6823      	ldr	r3, [r4, #0]
    1936:	4a4d      	ldr	r2, [pc, #308]	; (1a6c <main+0x98c>)
    1938:	0a1b      	lsrs	r3, r3, #8
    193a:	b29b      	uxth	r3, r3
        if (chip_id_user == 0){
    193c:	2b00      	cmp	r3, #0
    193e:	d103      	bne.n	1948 <main+0x868>
            chip_id_user = *REG_CHIP_ID;
    1940:	6813      	ldr	r3, [r2, #0]
        operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xA, chip_id_user);
    1942:	220a      	movs	r2, #10
    1944:	0011      	movs	r1, r2
    1946:	e7ab      	b.n	18a0 <main+0x7c0>
            *REG_CHIP_ID = chip_id_user;
    1948:	6013      	str	r3, [r2, #0]
    194a:	e7fa      	b.n	1942 <main+0x862>
    }else if(wakeup_data_header == 0x21){
    194c:	2d21      	cmp	r5, #33	; 0x21
    194e:	d000      	beq.n	1952 <main+0x872>
    1950:	e08e      	b.n	1a70 <main+0x990>
        if (user_val < 500){
    1952:	21fa      	movs	r1, #250	; 0xfa
        uint32_t user_val = wakeup_data & 0xFFFFFF;
    1954:	6823      	ldr	r3, [r4, #0]
    1956:	4a15      	ldr	r2, [pc, #84]	; (19ac <main+0x8cc>)
    1958:	021b      	lsls	r3, r3, #8
    195a:	0a1b      	lsrs	r3, r3, #8
        if (user_val < 500){
    195c:	0049      	lsls	r1, r1, #1
    195e:	428b      	cmp	r3, r1
    1960:	d201      	bcs.n	1966 <main+0x886>
            RADIO_PACKET_DELAY = 4000;
    1962:	23fa      	movs	r3, #250	; 0xfa
    1964:	011b      	lsls	r3, r3, #4
        TEMP_CALIB_B = wakeup_data & 0xFFFF; 
    1966:	6013      	str	r3, [r2, #0]
    1968:	e785      	b.n	1876 <main+0x796>
    196a:	46c0      	nop			; (mov r8, r8)
    196c:	ff0000ff 	.word	0xff0000ff
    1970:	007ac800 	.word	0x007ac800
    1974:	00001d70 	.word	0x00001d70
    1978:	fffe00ff 	.word	0xfffe00ff
    197c:	00001d84 	.word	0x00001d84
    1980:	00001d64 	.word	0x00001d64
    1984:	ffff1fff 	.word	0xffff1fff
    1988:	000fffff 	.word	0x000fffff
    198c:	0016e360 	.word	0x0016e360
    1990:	a000007c 	.word	0xa000007c
    1994:	00009c40 	.word	0x00009c40
    1998:	00001de4 	.word	0x00001de4
    199c:	00001dc0 	.word	0x00001dc0
    19a0:	00000514 	.word	0x00000514
    19a4:	00001e64 	.word	0x00001e64
    19a8:	00001dfc 	.word	0x00001dfc
    19ac:	00001dbc 	.word	0x00001dbc
    19b0:	00001e54 	.word	0x00001e54
    19b4:	00001e50 	.word	0x00001e50
    19b8:	00001db0 	.word	0x00001db0
    19bc:	00001df0 	.word	0x00001df0
    19c0:	00001e48 	.word	0x00001e48
    19c4:	00001e0c 	.word	0x00001e0c
    19c8:	00001dd8 	.word	0x00001dd8
    19cc:	00001e14 	.word	0x00001e14
    19d0:	0000076c 	.word	0x0000076c
    19d4:	00001dec 	.word	0x00001dec
    19d8:	00000898 	.word	0x00000898
    19dc:	00001e08 	.word	0x00001e08
    19e0:	000009c4 	.word	0x000009c4
    19e4:	00001e30 	.word	0x00001e30
    19e8:	00001dac 	.word	0x00001dac
    19ec:	00001194 	.word	0x00001194
    19f0:	00001e38 	.word	0x00001e38
    19f4:	00001b58 	.word	0x00001b58
    19f8:	00001de8 	.word	0x00001de8
    19fc:	000024b8 	.word	0x000024b8
    1a00:	00001db4 	.word	0x00001db4
    1a04:	00002ee0 	.word	0x00002ee0
    1a08:	00001e1c 	.word	0x00001e1c
    1a0c:	00001e00 	.word	0x00001e00
    1a10:	0003a980 	.word	0x0003a980
    1a14:	00001e04 	.word	0x00001e04
    1a18:	00004e20 	.word	0x00004e20
    1a1c:	00001e3c 	.word	0x00001e3c
    1a20:	00001e24 	.word	0x00001e24
    1a24:	00001e44 	.word	0x00001e44
    1a28:	0001fffe 	.word	0x0001fffe
    1a2c:	00001e34 	.word	0x00001e34
    1a30:	00001d98 	.word	0x00001d98
    1a34:	00001d9c 	.word	0x00001d9c
    1a38:	ffdfffff 	.word	0xffdfffff
    1a3c:	00001e28 	.word	0x00001e28
    1a40:	00001de0 	.word	0x00001de0
    1a44:	00001e2c 	.word	0x00001e2c
    1a48:	00001df8 	.word	0x00001df8
    1a4c:	00001da0 	.word	0x00001da0
    1a50:	ffe0007f 	.word	0xffe0007f
    1a54:	00001d88 	.word	0x00001d88
    1a58:	fffffc7f 	.word	0xfffffc7f
    1a5c:	fffff3ff 	.word	0xfffff3ff
    1a60:	a000002c 	.word	0xa000002c
    1a64:	00001e4c 	.word	0x00001e4c
    1a68:	00001e20 	.word	0x00001e20
    1a6c:	a00000fc 	.word	0xa00000fc
    }else if(wakeup_data_header == 0x22){
    1a70:	2d22      	cmp	r5, #34	; 0x22
    1a72:	d137      	bne.n	1ae4 <main+0xa04>
        mrr_freq_hopping = wakeup_data_field_2 & 0xF;
    1a74:	220f      	movs	r2, #15
    1a76:	4013      	ands	r3, r2
    1a78:	4a9e      	ldr	r2, [pc, #632]	; (1cf4 <main+0xc14>)
        mrr_freq_hopping_step = wakeup_data_field_2 >> 4;
    1a7a:	0900      	lsrs	r0, r0, #4
        mrr_freq_hopping = wakeup_data_field_2 & 0xF;
    1a7c:	6013      	str	r3, [r2, #0]
        mrr_freq_hopping_step = wakeup_data_field_2 >> 4;
    1a7e:	4b9e      	ldr	r3, [pc, #632]	; (1cf8 <main+0xc18>)
        mrr_cfo_val_fine_min = (wakeup_data >> 10) & 0x3F; // 6 bit
    1a80:	4e9e      	ldr	r6, [pc, #632]	; (1cfc <main+0xc1c>)
        mrr_freq_hopping_step = wakeup_data_field_2 >> 4;
    1a82:	6018      	str	r0, [r3, #0]
        mrr_cfo_val_fine_min = (wakeup_data >> 10) & 0x3F; // 6 bit
    1a84:	6823      	ldr	r3, [r4, #0]
    1a86:	351d      	adds	r5, #29
    1a88:	0a9b      	lsrs	r3, r3, #10
    1a8a:	402b      	ands	r3, r5
        mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = wakeup_data & 0x3FF; // 10 bit coarse setting 
    1a8c:	499c      	ldr	r1, [pc, #624]	; (1d00 <main+0xc20>)
        mrr_cfo_val_fine_min = (wakeup_data >> 10) & 0x3F; // 6 bit
    1a8e:	6033      	str	r3, [r6, #0]
        mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = wakeup_data & 0x3FF; // 10 bit coarse setting 
    1a90:	6822      	ldr	r2, [r4, #0]
    1a92:	489c      	ldr	r0, [pc, #624]	; (1d04 <main+0xc24>)
    1a94:	680b      	ldr	r3, [r1, #0]
    1a96:	0592      	lsls	r2, r2, #22
    1a98:	0bd2      	lsrs	r2, r2, #15
    1a9a:	4003      	ands	r3, r0
    1a9c:	4313      	orrs	r3, r2
    1a9e:	600b      	str	r3, [r1, #0]
        mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1aa0:	680a      	ldr	r2, [r1, #0]
    1aa2:	2004      	movs	r0, #4
    1aa4:	2100      	movs	r1, #0
    1aa6:	f7fe fb9c 	bl	1e2 <mbus_remote_register_write>
        mrrv11a_r01.TRX_CAP_ANTN_TUNE_COARSE = wakeup_data & 0x3FF; // 10 bit coarse setting
    1aaa:	4a97      	ldr	r2, [pc, #604]	; (1d08 <main+0xc28>)
    1aac:	6821      	ldr	r1, [r4, #0]
    1aae:	6813      	ldr	r3, [r2, #0]
    1ab0:	0589      	lsls	r1, r1, #22
    1ab2:	0a9b      	lsrs	r3, r3, #10
    1ab4:	0d89      	lsrs	r1, r1, #22
    1ab6:	029b      	lsls	r3, r3, #10
    1ab8:	430b      	orrs	r3, r1
    1aba:	6013      	str	r3, [r2, #0]
        mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min; 
    1abc:	6831      	ldr	r1, [r6, #0]
    1abe:	4893      	ldr	r0, [pc, #588]	; (1d0c <main+0xc2c>)
    1ac0:	6813      	ldr	r3, [r2, #0]
    1ac2:	4029      	ands	r1, r5
    1ac4:	0409      	lsls	r1, r1, #16
    1ac6:	4003      	ands	r3, r0
    1ac8:	430b      	orrs	r3, r1
    1aca:	6013      	str	r3, [r2, #0]
        mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min;
    1acc:	6831      	ldr	r1, [r6, #0]
    1ace:	6813      	ldr	r3, [r2, #0]
    1ad0:	4029      	ands	r1, r5
    1ad2:	4d8f      	ldr	r5, [pc, #572]	; (1d10 <main+0xc30>)
    1ad4:	0289      	lsls	r1, r1, #10
    1ad6:	401d      	ands	r5, r3
    1ad8:	430d      	orrs	r5, r1
        mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
    1ada:	2101      	movs	r1, #1
        mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min;
    1adc:	6015      	str	r5, [r2, #0]
        mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
    1ade:	6812      	ldr	r2, [r2, #0]
    1ae0:	2004      	movs	r0, #4
    1ae2:	e6ed      	b.n	18c0 <main+0x7e0>
    }else if(wakeup_data_header == 0x23){
    1ae4:	2d23      	cmp	r5, #35	; 0x23
    1ae6:	d113      	bne.n	1b10 <main+0xa30>
        mrrv11a_r07.RO_MOM = wakeup_data & 0x3F;
    1ae8:	213f      	movs	r1, #63	; 0x3f
    1aea:	227f      	movs	r2, #127	; 0x7f
    1aec:	4b89      	ldr	r3, [pc, #548]	; (1d14 <main+0xc34>)
    1aee:	6825      	ldr	r5, [r4, #0]
    1af0:	8818      	ldrh	r0, [r3, #0]
    1af2:	400d      	ands	r5, r1
    1af4:	4390      	bics	r0, r2
    1af6:	4328      	orrs	r0, r5
    1af8:	8018      	strh	r0, [r3, #0]
        mrrv11a_r07.RO_MIM = wakeup_data & 0x3F;
    1afa:	6822      	ldr	r2, [r4, #0]
    1afc:	4886      	ldr	r0, [pc, #536]	; (1d18 <main+0xc38>)
    1afe:	4011      	ands	r1, r2
    1b00:	881a      	ldrh	r2, [r3, #0]
    1b02:	01c9      	lsls	r1, r1, #7
    1b04:	4002      	ands	r2, r0
    1b06:	430a      	orrs	r2, r1
    1b08:	801a      	strh	r2, [r3, #0]
        mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);
    1b0a:	2107      	movs	r1, #7
    1b0c:	681a      	ldr	r2, [r3, #0]
    1b0e:	e7e7      	b.n	1ae0 <main+0xa00>
    }else if(wakeup_data_header == 0x25){
    1b10:	2d25      	cmp	r5, #37	; 0x25
    1b12:	d107      	bne.n	1b24 <main+0xa44>
        mrr_freq_hopping = wakeup_data_field_2 & 0xF;
    1b14:	220f      	movs	r2, #15
    1b16:	4013      	ands	r3, r2
    1b18:	4a76      	ldr	r2, [pc, #472]	; (1cf4 <main+0xc14>)
        mrr_freq_hopping_step = wakeup_data_field_2 >> 4;
    1b1a:	0900      	lsrs	r0, r0, #4
        mrr_freq_hopping = wakeup_data_field_2 & 0xF;
    1b1c:	6013      	str	r3, [r2, #0]
        mrr_freq_hopping_step = wakeup_data_field_2 >> 4;
    1b1e:	4b76      	ldr	r3, [pc, #472]	; (1cf8 <main+0xc18>)
    1b20:	6018      	str	r0, [r3, #0]
    1b22:	e6a8      	b.n	1876 <main+0x796>
    }else if(wakeup_data_header == 0x26){
    1b24:	2d26      	cmp	r5, #38	; 0x26
    1b26:	d10b      	bne.n	1b40 <main+0xa60>
        mrrv11a_r02.TX_BIAS_TUNE = wakeup_data & 0x1FFF;  //Set TX BIAS TUNE 13b // Set to max
    1b28:	497c      	ldr	r1, [pc, #496]	; (1d1c <main+0xc3c>)
    1b2a:	6822      	ldr	r2, [r4, #0]
    1b2c:	680b      	ldr	r3, [r1, #0]
    1b2e:	04d2      	lsls	r2, r2, #19
    1b30:	0b5b      	lsrs	r3, r3, #13
    1b32:	0cd2      	lsrs	r2, r2, #19
    1b34:	035b      	lsls	r3, r3, #13
    1b36:	4313      	orrs	r3, r2
    1b38:	600b      	str	r3, [r1, #0]
        mbus_remote_register_write(MRR_ADDR,0x02,mrrv11a_r02.as_int);
    1b3a:	680a      	ldr	r2, [r1, #0]
    1b3c:	2102      	movs	r1, #2
    1b3e:	e7cf      	b.n	1ae0 <main+0xa00>
    }else if(wakeup_data_header == 0x27){
    1b40:	2d27      	cmp	r5, #39	; 0x27
    1b42:	d10d      	bne.n	1b60 <main+0xa80>
        mrrv11a_r04.LDO_SEL_VOUT = wakeup_data & 0x7;
    1b44:	2007      	movs	r0, #7
    1b46:	6822      	ldr	r2, [r4, #0]
    1b48:	4975      	ldr	r1, [pc, #468]	; (1d20 <main+0xc40>)
    1b4a:	4002      	ands	r2, r0
    1b4c:	680b      	ldr	r3, [r1, #0]
    1b4e:	4875      	ldr	r0, [pc, #468]	; (1d24 <main+0xc44>)
    1b50:	0352      	lsls	r2, r2, #13
    1b52:	4003      	ands	r3, r0
    1b54:	4313      	orrs	r3, r2
    1b56:	600b      	str	r3, [r1, #0]
        mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    1b58:	680a      	ldr	r2, [r1, #0]
    1b5a:	2104      	movs	r1, #4
    1b5c:	0008      	movs	r0, r1
    1b5e:	e6af      	b.n	18c0 <main+0x7e0>
    }else if(wakeup_data_header == 0x2A){
    1b60:	2d2a      	cmp	r5, #42	; 0x2a
    1b62:	d104      	bne.n	1b6e <main+0xa8e>
        TEMP_CALIB_A = wakeup_data & 0xFFFFFF; 
    1b64:	6823      	ldr	r3, [r4, #0]
    1b66:	4a70      	ldr	r2, [pc, #448]	; (1d28 <main+0xc48>)
    1b68:	021b      	lsls	r3, r3, #8
    1b6a:	0a1b      	lsrs	r3, r3, #8
    1b6c:	e6fb      	b.n	1966 <main+0x886>
    }else if(wakeup_data_header == 0x2B){
    1b6e:	2d2b      	cmp	r5, #43	; 0x2b
    1b70:	d103      	bne.n	1b7a <main+0xa9a>
        TEMP_CALIB_B = wakeup_data & 0xFFFF; 
    1b72:	6823      	ldr	r3, [r4, #0]
    1b74:	4a6d      	ldr	r2, [pc, #436]	; (1d2c <main+0xc4c>)
    1b76:	b29b      	uxth	r3, r3
    1b78:	e6f5      	b.n	1966 <main+0x886>
    }else if(wakeup_data_header == 0x51){
    1b7a:	2d51      	cmp	r5, #81	; 0x51
    1b7c:	d106      	bne.n	1b8c <main+0xaac>
        operation_goc_trigger_radio(0xFFFFFFFF, wakeup_data_field_1, 0x4, exec_count_irq);
    1b7e:	2001      	movs	r0, #1
    1b80:	4b6b      	ldr	r3, [pc, #428]	; (1d30 <main+0xc50>)
    1b82:	2204      	movs	r2, #4
    1b84:	0039      	movs	r1, r7
    1b86:	681b      	ldr	r3, [r3, #0]
    1b88:	4240      	negs	r0, r0
    1b8a:	e5da      	b.n	1742 <main+0x662>
    }else if(wakeup_data_header == 0x52){
    1b8c:	2d52      	cmp	r5, #82	; 0x52
    1b8e:	d118      	bne.n	1bc2 <main+0xae2>
        mrr_freq_hopping = 0;
    1b90:	2500      	movs	r5, #0
        disable_timerwd();
    1b92:	f7fe fab7 	bl	104 <disable_timerwd>
        radio_power_on();
    1b96:	f7fe fbbf 	bl	318 <radio_power_on>
        uint32_t mrr_freq_hopping_saved = mrr_freq_hopping;
    1b9a:	4e56      	ldr	r6, [pc, #344]	; (1cf4 <main+0xc14>)
    1b9c:	6837      	ldr	r7, [r6, #0]
        mrr_freq_hopping = 0;
    1b9e:	6035      	str	r5, [r6, #0]
        while (ii < (wakeup_data & 0xFFFFFF)){
    1ba0:	6823      	ldr	r3, [r4, #0]
            send_radio_data_mrr(0,0x4,ii);  
    1ba2:	002a      	movs	r2, r5
        while (ii < (wakeup_data & 0xFFFFFF)){
    1ba4:	021b      	lsls	r3, r3, #8
            send_radio_data_mrr(0,0x4,ii);  
    1ba6:	2104      	movs	r1, #4
        while (ii < (wakeup_data & 0xFFFFFF)){
    1ba8:	0a1b      	lsrs	r3, r3, #8
    1baa:	42ab      	cmp	r3, r5
    1bac:	d804      	bhi.n	1bb8 <main+0xad8>
            send_radio_data_mrr(1,0x4,ii);  
    1bae:	2001      	movs	r0, #1
    1bb0:	f7fe ffb8 	bl	b24 <send_radio_data_mrr>
        mrr_freq_hopping = mrr_freq_hopping_saved;
    1bb4:	6037      	str	r7, [r6, #0]
    1bb6:	e58d      	b.n	16d4 <main+0x5f4>
            send_radio_data_mrr(0,0x4,ii);  
    1bb8:	2000      	movs	r0, #0
    1bba:	f7fe ffb3 	bl	b24 <send_radio_data_mrr>
            ii++;
    1bbe:	3501      	adds	r5, #1
    1bc0:	e7ee      	b.n	1ba0 <main+0xac0>
    }else if(wakeup_data_header == 0x53){
    1bc2:	2d53      	cmp	r5, #83	; 0x53
    1bc4:	d168      	bne.n	1c98 <main+0xbb8>
        disable_timerwd();
    1bc6:	f7fe fa9d 	bl	104 <disable_timerwd>
        *MBCWD_RESET = 1;
    1bca:	2201      	movs	r2, #1
        mrrv11a_r11.FSM_RESET_B = 1;  //UNRST BB
    1bcc:	2702      	movs	r7, #2
        *MBCWD_RESET = 1;
    1bce:	4b59      	ldr	r3, [pc, #356]	; (1d34 <main+0xc54>)
    1bd0:	601a      	str	r2, [r3, #0]
        radio_power_on();
    1bd2:	f7fe fba1 	bl	318 <radio_power_on>
        mrrv11a_r11.FSM_RESET_B = 1;  //UNRST BB
    1bd6:	4d58      	ldr	r5, [pc, #352]	; (1d38 <main+0xc58>)
        mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1bd8:	2111      	movs	r1, #17
        mrrv11a_r11.FSM_RESET_B = 1;  //UNRST BB
    1bda:	682b      	ldr	r3, [r5, #0]
        mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1bdc:	2004      	movs	r0, #4
        mrrv11a_r11.FSM_RESET_B = 1;  //UNRST BB
    1bde:	433b      	orrs	r3, r7
    1be0:	602b      	str	r3, [r5, #0]
        mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1be2:	682a      	ldr	r2, [r5, #0]
    1be4:	f7fe fafd 	bl	1e2 <mbus_remote_register_write>
        delay(MBUS_DELAY);
    1be8:	20c8      	movs	r0, #200	; 0xc8
    1bea:	f7fe fa5d 	bl	a8 <delay>
        mrrv11a_r03.TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    1bee:	2280      	movs	r2, #128	; 0x80
    1bf0:	4b52      	ldr	r3, [pc, #328]	; (1d3c <main+0xc5c>)
    1bf2:	03d2      	lsls	r2, r2, #15
    1bf4:	6819      	ldr	r1, [r3, #0]
        mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    1bf6:	2004      	movs	r0, #4
        mrrv11a_r03.TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    1bf8:	430a      	orrs	r2, r1
    1bfa:	601a      	str	r2, [r3, #0]
        mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    1bfc:	681a      	ldr	r2, [r3, #0]
    1bfe:	2103      	movs	r1, #3
    1c00:	f7fe faef 	bl	1e2 <mbus_remote_register_write>
        delay(MBUS_DELAY);
    1c04:	20c8      	movs	r0, #200	; 0xc8
    1c06:	f7fe fa4f 	bl	a8 <delay>
        mrrv11a_r00.TRX_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    1c0a:	227e      	movs	r2, #126	; 0x7e
    1c0c:	4e3c      	ldr	r6, [pc, #240]	; (1d00 <main+0xc20>)
        mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1c0e:	2100      	movs	r1, #0
        mrrv11a_r00.TRX_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    1c10:	6833      	ldr	r3, [r6, #0]
        mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1c12:	2004      	movs	r0, #4
        mrrv11a_r00.TRX_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    1c14:	4393      	bics	r3, r2
    1c16:	431f      	orrs	r7, r3
    1c18:	6037      	str	r7, [r6, #0]
        mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1c1a:	6832      	ldr	r2, [r6, #0]
    1c1c:	f7fe fae1 	bl	1e2 <mbus_remote_register_write>
        mrrv11a_r00.TRX_CL_EN = 1;
    1c20:	2201      	movs	r2, #1
    1c22:	6833      	ldr	r3, [r6, #0]
        mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1c24:	2100      	movs	r1, #0
        mrrv11a_r00.TRX_CL_EN = 1;
    1c26:	4313      	orrs	r3, r2
    1c28:	6033      	str	r3, [r6, #0]
        mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    1c2a:	2004      	movs	r0, #4
    1c2c:	6832      	ldr	r2, [r6, #0]
    1c2e:	f7fe fad8 	bl	1e2 <mbus_remote_register_write>
        mrrv11a_r11.FSM_CONT_PULSE_MODE = 1;
    1c32:	2308      	movs	r3, #8
    1c34:	682a      	ldr	r2, [r5, #0]
        mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1c36:	2111      	movs	r1, #17
        mrrv11a_r11.FSM_CONT_PULSE_MODE = 1;
    1c38:	4313      	orrs	r3, r2
    1c3a:	602b      	str	r3, [r5, #0]
        mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1c3c:	2004      	movs	r0, #4
    1c3e:	682a      	ldr	r2, [r5, #0]
    1c40:	f7fe facf 	bl	1e2 <mbus_remote_register_write>
        mrrv11a_r11.FSM_EN = 1;  //Start BB
    1c44:	2004      	movs	r0, #4
    1c46:	682b      	ldr	r3, [r5, #0]
        mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1c48:	2111      	movs	r1, #17
        mrrv11a_r11.FSM_EN = 1;  //Start BB
    1c4a:	4303      	orrs	r3, r0
    1c4c:	602b      	str	r3, [r5, #0]
        mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    1c4e:	682a      	ldr	r2, [r5, #0]
    1c50:	f7fe fac7 	bl	1e2 <mbus_remote_register_write>
        if ((wakeup_data & 0xFFFF) == 0){
    1c54:	6823      	ldr	r3, [r4, #0]
    1c56:	b29b      	uxth	r3, r3
    1c58:	2b00      	cmp	r3, #0
    1c5a:	d100      	bne.n	1c5e <main+0xb7e>
            while(1);
    1c5c:	e7fe      	b.n	1c5c <main+0xb7c>
            if ((wakeup_data & 0xFFFF) > 340){ // Max count is roughly 5.6 hours
    1c5e:	22aa      	movs	r2, #170	; 0xaa
    1c60:	6823      	ldr	r3, [r4, #0]
    1c62:	0052      	lsls	r2, r2, #1
    1c64:	b29b      	uxth	r3, r3
    1c66:	4293      	cmp	r3, r2
    1c68:	d910      	bls.n	1c8c <main+0xbac>
                set_timer32_timeout(0xFFFFFFFF);
    1c6a:	2001      	movs	r0, #1
    1c6c:	4240      	negs	r0, r0
                set_timer32_timeout(0xC00000*(wakeup_data & 0xFFFF));
    1c6e:	f7fe fc9d 	bl	5ac <set_timer32_timeout>
            while(!irq_pending.timer32);    // [CISv2] Newly Added
    1c72:	4b33      	ldr	r3, [pc, #204]	; (1d40 <main+0xc60>)
    1c74:	681b      	ldr	r3, [r3, #0]
    1c76:	071b      	lsls	r3, r3, #28
    1c78:	d5fb      	bpl.n	1c72 <main+0xb92>
            irq_pending.timer32 = 0;        // [CISv2] Newly Added
    1c7a:	2208      	movs	r2, #8
    1c7c:	4b30      	ldr	r3, [pc, #192]	; (1d40 <main+0xc60>)
    1c7e:	681b      	ldr	r3, [r3, #0]
    1c80:	4393      	bics	r3, r2
    1c82:	4a2f      	ldr	r2, [pc, #188]	; (1d40 <main+0xc60>)
    1c84:	6013      	str	r3, [r2, #0]
            radio_power_off();
    1c86:	f7fe fc0b 	bl	4a0 <radio_power_off>
    1c8a:	e5f4      	b.n	1876 <main+0x796>
                set_timer32_timeout(0xC00000*(wakeup_data & 0xFFFF));
    1c8c:	20c0      	movs	r0, #192	; 0xc0
    1c8e:	6823      	ldr	r3, [r4, #0]
    1c90:	0400      	lsls	r0, r0, #16
    1c92:	b29b      	uxth	r3, r3
    1c94:	4358      	muls	r0, r3
    1c96:	e7ea      	b.n	1c6e <main+0xb8e>
    }else if(wakeup_data_header == 0xF0){
    1c98:	2df0      	cmp	r5, #240	; 0xf0
    1c9a:	d104      	bne.n	1ca6 <main+0xbc6>
        operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xB, enumerated);
    1c9c:	220b      	movs	r2, #11
    1c9e:	4b29      	ldr	r3, [pc, #164]	; (1d44 <main+0xc64>)
    1ca0:	681b      	ldr	r3, [r3, #0]
        operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xC, puf_chip_id);
    1ca2:	210a      	movs	r1, #10
    1ca4:	e5fc      	b.n	18a0 <main+0x7c0>
    }else if(wakeup_data_header == 0xF1){
    1ca6:	2df1      	cmp	r5, #241	; 0xf1
    1ca8:	d10e      	bne.n	1cc8 <main+0xbe8>
        *REG_SYS_CONF = (0x0/*PUF_SLEEP*/ << 6) | (0x1/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);
    1caa:	2320      	movs	r3, #32
        delay(MBUS_DELAY*4);
    1cac:	20c8      	movs	r0, #200	; 0xc8
        *REG_SYS_CONF = (0x0/*PUF_SLEEP*/ << 6) | (0x1/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);
    1cae:	4c26      	ldr	r4, [pc, #152]	; (1d48 <main+0xc68>)
        delay(MBUS_DELAY*4);
    1cb0:	0080      	lsls	r0, r0, #2
        *REG_SYS_CONF = (0x0/*PUF_SLEEP*/ << 6) | (0x1/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);
    1cb2:	6023      	str	r3, [r4, #0]
        delay(MBUS_DELAY*4);
    1cb4:	f7fe f9f8 	bl	a8 <delay>
        *REG_SYS_CONF = (0x0/*PUF_SLEEP*/ << 6) | (0x0/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);
    1cb8:	2300      	movs	r3, #0
        *REG_SYS_CONF = (0x1/*PUF_SLEEP*/ << 6) | (0x1/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);
    1cba:	2260      	movs	r2, #96	; 0x60
        *REG_SYS_CONF = (0x0/*PUF_SLEEP*/ << 6) | (0x0/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);
    1cbc:	6023      	str	r3, [r4, #0]
        puf_chip_id = *REG_PUF_CHIP_ID;
    1cbe:	4b23      	ldr	r3, [pc, #140]	; (1d4c <main+0xc6c>)
    1cc0:	681b      	ldr	r3, [r3, #0]
        *REG_SYS_CONF = (0x1/*PUF_SLEEP*/ << 6) | (0x1/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);
    1cc2:	6022      	str	r2, [r4, #0]
        operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xC, puf_chip_id);
    1cc4:	3a54      	subs	r2, #84	; 0x54
    1cc6:	e7ec      	b.n	1ca2 <main+0xbc2>
    }else if(wakeup_data_header == 0xFA){
    1cc8:	2dfa      	cmp	r5, #250	; 0xfa
    1cca:	d000      	beq.n	1cce <main+0xbee>
    1ccc:	e502      	b.n	16d4 <main+0x5f4>
        if ((wakeup_data&0xFFFFFF) == 0x89D7E2){
    1cce:	6823      	ldr	r3, [r4, #0]
    1cd0:	4a1f      	ldr	r2, [pc, #124]	; (1d50 <main+0xc70>)
    1cd2:	021b      	lsls	r3, r3, #8
    1cd4:	0a1b      	lsrs	r3, r3, #8
    1cd6:	4293      	cmp	r3, r2
    1cd8:	d000      	beq.n	1cdc <main+0xbfc>
    1cda:	e5cc      	b.n	1876 <main+0x796>
            config_timerwd(0xFF);
    1cdc:	20ff      	movs	r0, #255	; 0xff
    1cde:	f7fe fa05 	bl	ec <config_timerwd>
                mbus_write_message32(0xE0, 0x0);
    1ce2:	2100      	movs	r1, #0
    1ce4:	20e0      	movs	r0, #224	; 0xe0
    1ce6:	f7fe fa3d 	bl	164 <mbus_write_message32>
                delay(MBUS_DELAY);
    1cea:	20c8      	movs	r0, #200	; 0xc8
    1cec:	f7fe f9dc 	bl	a8 <delay>
            while(1){
    1cf0:	e7f7      	b.n	1ce2 <main+0xc02>
    1cf2:	46c0      	nop			; (mov r8, r8)
    1cf4:	00001ddc 	.word	0x00001ddc
    1cf8:	00001e58 	.word	0x00001e58
    1cfc:	00001db8 	.word	0x00001db8
    1d00:	00001d58 	.word	0x00001d58
    1d04:	fffe007f 	.word	0xfffe007f
    1d08:	00001e68 	.word	0x00001e68
    1d0c:	ffc0ffff 	.word	0xffc0ffff
    1d10:	ffff03ff 	.word	0xffff03ff
    1d14:	00001d68 	.word	0x00001d68
    1d18:	ffffc07f 	.word	0xffffc07f
    1d1c:	00001d5c 	.word	0x00001d5c
    1d20:	00001d64 	.word	0x00001d64
    1d24:	ffff1fff 	.word	0xffff1fff
    1d28:	00001e00 	.word	0x00001e00
    1d2c:	00001e04 	.word	0x00001e04
    1d30:	00001e3c 	.word	0x00001e3c
    1d34:	a0001504 	.word	0xa0001504
    1d38:	00001d70 	.word	0x00001d70
    1d3c:	00001d60 	.word	0x00001d60
    1d40:	00001d54 	.word	0x00001d54
    1d44:	00001df4 	.word	0x00001df4
    1d48:	a000006c 	.word	0xa000006c
    1d4c:	a0000034 	.word	0xa0000034
    1d50:	0089d7e2 	.word	0x0089d7e2
