
Pstack_ondemand_v1.8/Pstack_ondemand_v1.8.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0x70>:
   0:	00000c00 	.word	0x00000c00
   4:	00000075 	.word	0x00000075
	...
  1c:	00000070 	.word	0x00000070
  20:	00000070 	.word	0x00000070
  24:	00000070 	.word	0x00000070
  28:	00000070 	.word	0x00000070
	...
  34:	00000070 	.word	0x00000070
	...
  40:	000007fd 	.word	0x000007fd
  44:	00000815 	.word	0x00000815
  48:	0000082d 	.word	0x0000082d
  4c:	00000845 	.word	0x00000845
	...

00000070 <hang>:
  70:	e7fe      	b.n	70 <hang>
	...

00000074 <_start>:
  74:	f000 fbf2 	bl	85c <main>
  78:	e7fc      	b.n	74 <_start>

Disassembly of section .text.memcpy:

0000007a <memcpy>:
// Naive memcpy

void *memcpy(void *dest, const void *src, int n) {
  7a:	b510      	push	{r4, lr}
	char *d = (char *) dest;
	char const *s = (char const *) src;

	while (n--) {
  7c:	2300      	movs	r3, #0
  7e:	e002      	b.n	86 <memcpy+0xc>
		*d++ = *s++;
  80:	5ccc      	ldrb	r4, [r1, r3]
  82:	54c4      	strb	r4, [r0, r3]
  84:	3301      	adds	r3, #1

void *memcpy(void *dest, const void *src, int n) {
	char *d = (char *) dest;
	char const *s = (char const *) src;

	while (n--) {
  86:	4293      	cmp	r3, r2
  88:	d1fa      	bne.n	80 <memcpy+0x6>
		*d++ = *s++;
	}

	return dest;
}
  8a:	bd10      	pop	{r4, pc}

Disassembly of section .text.write_config_reg:

0000008c <write_config_reg>:
// [31:4] = 28'hA200000
//  [3:2] = Target register
//  [1:0] = 2'b00
int write_config_reg(uint8_t reg, uint32_t data) {
	uint32_t _addr = 0xA2000000;
	_addr |= (reg << 2);
  8c:	23a2      	movs	r3, #162	; 0xa2
  8e:	061b      	lsls	r3, r3, #24
  90:	0080      	lsls	r0, r0, #2
  92:	4318      	orrs	r0, r3

	*((volatile uint32_t *) _addr) = data;
  94:	6001      	str	r1, [r0, #0]
	return 0;
}
  96:	2000      	movs	r0, #0
  98:	4770      	bx	lr

Disassembly of section .text.delay:

0000009a <delay>:

void delay(unsigned ticks) {
  9a:	b500      	push	{lr}
  unsigned i;
  for (i=0; i < ticks; i++)
  9c:	2300      	movs	r3, #0
  9e:	e001      	b.n	a4 <delay+0xa>
    asm("nop;");
  a0:	46c0      	nop			; (mov r8, r8)
	return 0;
}

void delay(unsigned ticks) {
  unsigned i;
  for (i=0; i < ticks; i++)
  a2:	3301      	adds	r3, #1
  a4:	4283      	cmp	r3, r0
  a6:	d1fb      	bne.n	a0 <delay+0x6>
    asm("nop;");
}
  a8:	bd00      	pop	{pc}

Disassembly of section .text.WFI:

000000aa <WFI>:

void WFI(){
  asm("wfi;");
  aa:	bf30      	wfi
}
  ac:	4770      	bx	lr

Disassembly of section .text.config_timer:

000000b0 <config_timer>:

void config_timer( uint8_t timer_id, uint8_t go, uint8_t roi, uint32_t init_val, uint32_t sat_val) {
  b0:	b570      	push	{r4, r5, r6, lr}
	uint32_t _addr = 0xA5000000;
	_addr |= (timer_id<<4);
  b2:	24a5      	movs	r4, #165	; 0xa5
  b4:	0100      	lsls	r0, r0, #4
  b6:	0624      	lsls	r4, r4, #24
  b8:	4304      	orrs	r4, r0
	// GO  = 0x0
	// SAT = 0x4
	// ROI = 0x8
	// CNT = 0xC
	*((volatile uint32_t *) (_addr | 0x0) ) = 0x0;		// stop timer first
  ba:	2500      	movs	r5, #0
  bc:	6025      	str	r5, [r4, #0]
	*((volatile uint32_t *) (_addr | 0x4) ) = sat_val;	// set up values	
  be:	4d06      	ldr	r5, [pc, #24]	; (d8 <config_timer+0x28>)
  c0:	9e04      	ldr	r6, [sp, #16]
  c2:	4305      	orrs	r5, r0
  c4:	602e      	str	r6, [r5, #0]
	*((volatile uint32_t *) (_addr | 0x8) ) = roi;
  c6:	4d05      	ldr	r5, [pc, #20]	; (dc <config_timer+0x2c>)
  c8:	4305      	orrs	r5, r0
  ca:	602a      	str	r2, [r5, #0]
	*((volatile uint32_t *) (_addr | 0xC) ) = init_val;
  cc:	4a04      	ldr	r2, [pc, #16]	; (e0 <config_timer+0x30>)
  ce:	4310      	orrs	r0, r2
  d0:	6003      	str	r3, [r0, #0]
	*((volatile uint32_t *) (_addr | 0x0) ) = go;		// run
  d2:	6021      	str	r1, [r4, #0]
}
  d4:	bd70      	pop	{r4, r5, r6, pc}
  d6:	46c0      	nop			; (mov r8, r8)
  d8:	a5000004 	.word	0xa5000004
  dc:	a5000008 	.word	0xa5000008
  e0:	a500000c 	.word	0xa500000c

Disassembly of section .text.set_wakeup_timer:

000000e4 <set_wakeup_timer>:

void set_wakeup_timer( uint16_t timestamp, uint8_t on, uint8_t reset ) {
  e4:	b500      	push	{lr}
	uint32_t regval = timestamp;
	if( on )	regval |= 0x8000;
  e6:	2900      	cmp	r1, #0
  e8:	d003      	beq.n	f2 <set_wakeup_timer+0xe>
  ea:	2380      	movs	r3, #128	; 0x80
  ec:	021b      	lsls	r3, r3, #8
  ee:	4318      	orrs	r0, r3
  f0:	e001      	b.n	f6 <set_wakeup_timer+0x12>
	else		regval &= 0x7FFF;
  f2:	0440      	lsls	r0, r0, #17
  f4:	0c40      	lsrs	r0, r0, #17
	*((volatile uint32_t *) 0xA2000010) = regval;
  f6:	4b04      	ldr	r3, [pc, #16]	; (108 <set_wakeup_timer+0x24>)
  f8:	6018      	str	r0, [r3, #0]

	if( reset )
  fa:	2a00      	cmp	r2, #0
  fc:	d002      	beq.n	104 <set_wakeup_timer+0x20>
		*((volatile uint32_t *) 0xA2000014) = 0x01;
  fe:	4b03      	ldr	r3, [pc, #12]	; (10c <set_wakeup_timer+0x28>)
 100:	2201      	movs	r2, #1
 102:	601a      	str	r2, [r3, #0]
}
 104:	bd00      	pop	{pc}
 106:	46c0      	nop			; (mov r8, r8)
 108:	a2000010 	.word	0xa2000010
 10c:	a2000014 	.word	0xa2000014

Disassembly of section .text.set_clkfreq:

00000110 <set_clkfreq>:

void set_clkfreq( uint8_t fastmode, uint8_t div_cm, uint8_t div_mbus, uint8_t ring ) {
 110:	b530      	push	{r4, r5, lr}
	uint32_t regval = *((volatile uint32_t *) 0xA0001028 );		// Read original reg value
 112:	4c08      	ldr	r4, [pc, #32]	; (134 <set_clkfreq+0x24>)
	regval &= 0xFF7FC0FF;
	regval |= (fastmode<<23) | (div_cm<<12) | (div_mbus<<10) | (ring<<8) ;
 114:	05c0      	lsls	r0, r0, #23
	if( reset )
		*((volatile uint32_t *) 0xA2000014) = 0x01;
}

void set_clkfreq( uint8_t fastmode, uint8_t div_cm, uint8_t div_mbus, uint8_t ring ) {
	uint32_t regval = *((volatile uint32_t *) 0xA0001028 );		// Read original reg value
 116:	6825      	ldr	r5, [r4, #0]
	regval &= 0xFF7FC0FF;
	regval |= (fastmode<<23) | (div_cm<<12) | (div_mbus<<10) | (ring<<8) ;
 118:	0309      	lsls	r1, r1, #12
		*((volatile uint32_t *) 0xA2000014) = 0x01;
}

void set_clkfreq( uint8_t fastmode, uint8_t div_cm, uint8_t div_mbus, uint8_t ring ) {
	uint32_t regval = *((volatile uint32_t *) 0xA0001028 );		// Read original reg value
	regval &= 0xFF7FC0FF;
 11a:	4c07      	ldr	r4, [pc, #28]	; (138 <set_clkfreq+0x28>)
	regval |= (fastmode<<23) | (div_cm<<12) | (div_mbus<<10) | (ring<<8) ;
 11c:	4301      	orrs	r1, r0
		*((volatile uint32_t *) 0xA2000014) = 0x01;
}

void set_clkfreq( uint8_t fastmode, uint8_t div_cm, uint8_t div_mbus, uint8_t ring ) {
	uint32_t regval = *((volatile uint32_t *) 0xA0001028 );		// Read original reg value
	regval &= 0xFF7FC0FF;
 11e:	402c      	ands	r4, r5
	regval |= (fastmode<<23) | (div_cm<<12) | (div_mbus<<10) | (ring<<8) ;
 120:	0295      	lsls	r5, r2, #10
 122:	1c0a      	adds	r2, r1, #0
 124:	432a      	orrs	r2, r5
 126:	021b      	lsls	r3, r3, #8
 128:	431a      	orrs	r2, r3
 12a:	4322      	orrs	r2, r4
	*((volatile uint32_t *) 0xA2000008) = regval;			// Write updated reg value
 12c:	4c03      	ldr	r4, [pc, #12]	; (13c <set_clkfreq+0x2c>)
 12e:	6022      	str	r2, [r4, #0]
}
 130:	bd30      	pop	{r4, r5, pc}
 132:	46c0      	nop			; (mov r8, r8)
 134:	a0001028 	.word	0xa0001028
 138:	ff7fc0ff 	.word	0xff7fc0ff
 13c:	a2000008 	.word	0xa2000008

Disassembly of section .text.pmu_div5_ovrd:

00000140 <pmu_div5_ovrd>:

void pmu_div5_ovrd( uint8_t ovrd ) {
	uint32_t regval = *((volatile uint32_t *) 0xA000102C );		// Read original reg value
 140:	4b07      	ldr	r3, [pc, #28]	; (160 <pmu_div5_ovrd+0x20>)
	regval &= 0xFF7FC0FF;
	regval |= (fastmode<<23) | (div_cm<<12) | (div_mbus<<10) | (ring<<8) ;
	*((volatile uint32_t *) 0xA2000008) = regval;			// Write updated reg value
}

void pmu_div5_ovrd( uint8_t ovrd ) {
 142:	b500      	push	{lr}
	uint32_t regval = *((volatile uint32_t *) 0xA000102C );		// Read original reg value
 144:	681b      	ldr	r3, [r3, #0]
	if( ovrd ){
 146:	2800      	cmp	r0, #0
 148:	d005      	beq.n	156 <pmu_div5_ovrd+0x16>
		regval |= 0x40000000;					// Set bit 30 and reset bit 31
                regval &= 0x7FFFFFFF;
 14a:	005b      	lsls	r3, r3, #1
 14c:	085a      	lsrs	r2, r3, #1
 14e:	2380      	movs	r3, #128	; 0x80
 150:	05db      	lsls	r3, r3, #23
 152:	4313      	orrs	r3, r2
 154:	e001      	b.n	15a <pmu_div5_ovrd+0x1a>
        }
	else
		regval &= 0x3FFFFFFF;					// Reset bit 30 and bit 31
 156:	009b      	lsls	r3, r3, #2
 158:	089b      	lsrs	r3, r3, #2
	*((volatile uint32_t *) 0xA200000C) = regval;			// Write updated reg value
 15a:	4a02      	ldr	r2, [pc, #8]	; (164 <pmu_div5_ovrd+0x24>)
 15c:	6013      	str	r3, [r2, #0]
}
 15e:	bd00      	pop	{pc}
 160:	a000102c 	.word	0xa000102c
 164:	a200000c 	.word	0xa200000c

Disassembly of section .text.pmu_div6_ovrd:

00000168 <pmu_div6_ovrd>:

void pmu_div6_ovrd( uint8_t ovrd ) {
	uint32_t regval = *((volatile uint32_t *) 0xA000102C );		// Read original reg value
 168:	4b07      	ldr	r3, [pc, #28]	; (188 <pmu_div6_ovrd+0x20>)
	else
		regval &= 0x3FFFFFFF;					// Reset bit 30 and bit 31
	*((volatile uint32_t *) 0xA200000C) = regval;			// Write updated reg value
}

void pmu_div6_ovrd( uint8_t ovrd ) {
 16a:	b500      	push	{lr}
	uint32_t regval = *((volatile uint32_t *) 0xA000102C );		// Read original reg value
 16c:	681b      	ldr	r3, [r3, #0]
	if( ovrd ){
 16e:	2800      	cmp	r0, #0
 170:	d005      	beq.n	17e <pmu_div6_ovrd+0x16>
		regval |= 0x80000000;					// Set bit 31 and reset bit 30
                regval &= 0xBFFFFFFF;
 172:	4a06      	ldr	r2, [pc, #24]	; (18c <pmu_div6_ovrd+0x24>)
 174:	4013      	ands	r3, r2
 176:	2280      	movs	r2, #128	; 0x80
 178:	0612      	lsls	r2, r2, #24
 17a:	4313      	orrs	r3, r2
 17c:	e001      	b.n	182 <pmu_div6_ovrd+0x1a>
        }
	else
		regval &= 0x3FFFFFFF;					// Reset bit 31 and bit 30
 17e:	009b      	lsls	r3, r3, #2
 180:	089b      	lsrs	r3, r3, #2
	*((volatile uint32_t *) 0xA200000C) = regval;			// Write updated reg value
 182:	4a03      	ldr	r2, [pc, #12]	; (190 <pmu_div6_ovrd+0x28>)
 184:	6013      	str	r3, [r2, #0]
}
 186:	bd00      	pop	{pc}
 188:	a000102c 	.word	0xa000102c
 18c:	bfffffff 	.word	0xbfffffff
 190:	a200000c 	.word	0xa200000c

Disassembly of section .text.pmu_set_force_wake:

00000194 <pmu_set_force_wake>:

void pmu_set_force_wake( uint8_t set ) {
	uint32_t regval = *((volatile uint32_t *) 0xA0001028 );		// Read original reg value
 194:	4b05      	ldr	r3, [pc, #20]	; (1ac <pmu_set_force_wake+0x18>)
	else
		regval &= 0x3FFFFFFF;					// Reset bit 31 and bit 30
	*((volatile uint32_t *) 0xA200000C) = regval;			// Write updated reg value
}

void pmu_set_force_wake( uint8_t set ) {
 196:	b500      	push	{lr}
	uint32_t regval = *((volatile uint32_t *) 0xA0001028 );		// Read original reg value
 198:	681b      	ldr	r3, [r3, #0]
 19a:	2280      	movs	r2, #128	; 0x80
	if( set )
 19c:	2800      	cmp	r0, #0
 19e:	d001      	beq.n	1a4 <pmu_set_force_wake+0x10>
		regval |= 0x00000080;					// Set bit 7
 1a0:	4313      	orrs	r3, r2
 1a2:	e000      	b.n	1a6 <pmu_set_force_wake+0x12>
	else
		regval &= 0xFFFFFF7F;					// Reset bit 7
 1a4:	4393      	bics	r3, r2
	*((volatile uint32_t *) 0xA2000008) = regval;			// Write updated reg value
 1a6:	4a02      	ldr	r2, [pc, #8]	; (1b0 <pmu_set_force_wake+0x1c>)
 1a8:	6013      	str	r3, [r2, #0]
}
 1aa:	bd00      	pop	{pc}
 1ac:	a0001028 	.word	0xa0001028
 1b0:	a2000008 	.word	0xa2000008

Disassembly of section .text.pon_reset:

000001b4 <pon_reset>:

void pon_reset( void ){
	*((volatile uint32_t *) 0xA3000000) = 0x2;
 1b4:	23a3      	movs	r3, #163	; 0xa3
 1b6:	2202      	movs	r2, #2
 1b8:	061b      	lsls	r3, r3, #24
 1ba:	601a      	str	r2, [r3, #0]
}
 1bc:	4770      	bx	lr

Disassembly of section .text.sleep_req_by_sw:

000001be <sleep_req_by_sw>:

void sleep_req_by_sw( void ){
	*((volatile uint32_t *) 0xA3000000) = 0x4;
 1be:	23a3      	movs	r3, #163	; 0xa3
 1c0:	2204      	movs	r2, #4
 1c2:	061b      	lsls	r3, r3, #24
 1c4:	601a      	str	r2, [r3, #0]
}
 1c6:	4770      	bx	lr

Disassembly of section .text.generate_ECC:

000001c8 <generate_ECC>:
//
// Output Format =
//  data_out[31:6] = data_in[25:0]
//  data_out[ 5:0] = 6bit ECC

uint32_t generate_ECC( uint32_t data_in ) {
 1c8:	b5f0      	push	{r4, r5, r6, r7, lr}
	uint32_t P5 = 
		((data_in>>25)&0x1) ^ 
 1ca:	2401      	movs	r4, #1
 1cc:	0e47      	lsrs	r7, r0, #25
		((data_in>>24)&0x1) ^ 
 1ce:	0e06      	lsrs	r6, r0, #24
//
// Output Format =
//  data_out[31:6] = data_in[25:0]
//  data_out[ 5:0] = 6bit ECC

uint32_t generate_ECC( uint32_t data_in ) {
 1d0:	b099      	sub	sp, #100	; 0x64
	uint32_t P5 = 
		((data_in>>25)&0x1) ^ 
 1d2:	4027      	ands	r7, r4
		((data_in>>24)&0x1) ^ 
 1d4:	4026      	ands	r6, r4
//  data_out[31:6] = data_in[25:0]
//  data_out[ 5:0] = 6bit ECC

uint32_t generate_ECC( uint32_t data_in ) {
	uint32_t P5 = 
		((data_in>>25)&0x1) ^ 
 1d6:	9709      	str	r7, [sp, #36]	; 0x24
 1d8:	4077      	eors	r7, r6
 1da:	970a      	str	r7, [sp, #40]	; 0x28
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
		((data_in>>22)&0x1) ^ 
 1dc:	0d86      	lsrs	r6, r0, #22

uint32_t generate_ECC( uint32_t data_in ) {
	uint32_t P5 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
 1de:	0dc7      	lsrs	r7, r0, #23
 1e0:	4027      	ands	r7, r4
		((data_in>>22)&0x1) ^ 
 1e2:	4026      	ands	r6, r4

uint32_t generate_ECC( uint32_t data_in ) {
	uint32_t P5 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
 1e4:	970b      	str	r7, [sp, #44]	; 0x2c
//  data_out[ 5:0] = 6bit ECC

uint32_t generate_ECC( uint32_t data_in ) {
	uint32_t P5 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
 1e6:	407e      	eors	r6, r7
		((data_in>>23)&0x1) ^ 
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
 1e8:	0d47      	lsrs	r7, r0, #21

uint32_t generate_ECC( uint32_t data_in ) {
	uint32_t P5 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
 1ea:	9a0a      	ldr	r2, [sp, #40]	; 0x28
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
 1ec:	4027      	ands	r7, r4
 1ee:	970c      	str	r7, [sp, #48]	; 0x30

uint32_t generate_ECC( uint32_t data_in ) {
	uint32_t P5 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
 1f0:	4072      	eors	r2, r6
		((data_in>>22)&0x1) ^ 
 1f2:	9b0c      	ldr	r3, [sp, #48]	; 0x30
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
 1f4:	0d06      	lsrs	r6, r0, #20
 1f6:	4026      	ands	r6, r4
		((data_in>>19)&0x1) ^ 
 1f8:	0cc7      	lsrs	r7, r0, #19
 1fa:	4027      	ands	r7, r4
uint32_t generate_ECC( uint32_t data_in ) {
	uint32_t P5 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
		((data_in>>22)&0x1) ^ 
 1fc:	4073      	eors	r3, r6
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
 1fe:	970e      	str	r7, [sp, #56]	; 0x38
uint32_t generate_ECC( uint32_t data_in ) {
	uint32_t P5 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
		((data_in>>22)&0x1) ^ 
 200:	930d      	str	r3, [sp, #52]	; 0x34
		((data_in>>21)&0x1) ^ 
 202:	407b      	eors	r3, r7
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
 204:	0c87      	lsrs	r7, r0, #18
 206:	4027      	ands	r7, r4
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
 208:	405f      	eors	r7, r3

uint32_t generate_ECC( uint32_t data_in ) {
	uint32_t P5 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
 20a:	9203      	str	r2, [sp, #12]
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
 20c:	407a      	eors	r2, r7
		((data_in>>18)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
 20e:	0c47      	lsrs	r7, r0, #17
 210:	4027      	ands	r7, r4
 212:	46bc      	mov	ip, r7
		((data_in>>16)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
 214:	0bc7      	lsrs	r7, r0, #15
 216:	4027      	ands	r7, r4
 218:	9711      	str	r7, [sp, #68]	; 0x44
		((data_in>>14)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
 21a:	0b47      	lsrs	r7, r0, #13
 21c:	4027      	ands	r7, r4
 21e:	9704      	str	r7, [sp, #16]
		((data_in>>12)&0x1) ^ 
 220:	0b07      	lsrs	r7, r0, #12
 222:	4027      	ands	r7, r4
 224:	9713      	str	r7, [sp, #76]	; 0x4c
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
 226:	0c06      	lsrs	r6, r0, #16
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
		((data_in>>12)&0x1) ^ 
		((data_in>>11)&0x1);
 228:	0ac7      	lsrs	r7, r0, #11
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
 22a:	4026      	ands	r6, r4
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
		((data_in>>12)&0x1) ^ 
		((data_in>>11)&0x1);
 22c:	4027      	ands	r7, r4
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
 22e:	920f      	str	r2, [sp, #60]	; 0x3c
		((data_in>>18)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
 230:	9610      	str	r6, [sp, #64]	; 0x40
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
 232:	9a11      	ldr	r2, [sp, #68]	; 0x44
		((data_in>>16)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
		((data_in>>12)&0x1) ^ 
		((data_in>>11)&0x1);
 234:	9714      	str	r7, [sp, #80]	; 0x50
		((data_in>>23)&0x1) ^ 
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
 236:	1c37      	adds	r7, r6, #0
 238:	4666      	mov	r6, ip
 23a:	4077      	eors	r7, r6
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
 23c:	0b83      	lsrs	r3, r0, #14
 23e:	4023      	ands	r3, r4
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
 240:	4057      	eors	r7, r2
		((data_in>>16)&0x1) ^ 
 242:	405f      	eors	r7, r3
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
 244:	0a02      	lsrs	r2, r0, #8
		((data_in>>7 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
 246:	0943      	lsrs	r3, r0, #5
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
 248:	4022      	ands	r2, r4
		((data_in>>7 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
 24a:	4023      	ands	r3, r4
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
 24c:	9712      	str	r7, [sp, #72]	; 0x48
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
 24e:	9205      	str	r2, [sp, #20]
		((data_in>>7 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
 250:	9306      	str	r3, [sp, #24]
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
 252:	0a85      	lsrs	r5, r0, #10
		((data_in>>9 )&0x1) ^ 
 254:	0a47      	lsrs	r7, r0, #9
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
 256:	09c2      	lsrs	r2, r0, #7
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
		((data_in>>4 )&0x1);
 258:	0903      	lsrs	r3, r0, #4
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
 25a:	4025      	ands	r5, r4
		((data_in>>9 )&0x1) ^ 
 25c:	4027      	ands	r7, r4
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
 25e:	4022      	ands	r2, r4
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
		((data_in>>4 )&0x1);
 260:	4023      	ands	r3, r4
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
 262:	9216      	str	r2, [sp, #88]	; 0x58
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
		((data_in>>4 )&0x1);
 264:	9307      	str	r3, [sp, #28]
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
 266:	9701      	str	r7, [sp, #4]
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
 268:	9b05      	ldr	r3, [sp, #20]
		((data_in>>23)&0x1) ^ 
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
 26a:	406f      	eors	r7, r5
 26c:	9715      	str	r7, [sp, #84]	; 0x54
		((data_in>>10)&0x1) ^ 
 26e:	1c3e      	adds	r6, r7, #0
		((data_in>>9 )&0x1) ^ 
 270:	9f16      	ldr	r7, [sp, #88]	; 0x58
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
 272:	405e      	eors	r6, r3
		((data_in>>9 )&0x1) ^ 
 274:	407e      	eors	r6, r7
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
 276:	0982      	lsrs	r2, r0, #6
 278:	4022      	ands	r2, r4
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
 27a:	9b06      	ldr	r3, [sp, #24]
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
 27c:	1c31      	adds	r1, r6, #0
		((data_in>>7 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
 27e:	9f07      	ldr	r7, [sp, #28]
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
 280:	4051      	eors	r1, r2
		((data_in>>7 )&0x1) ^ 
 282:	4059      	eors	r1, r3
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
		((data_in>>12)&0x1) ^ 
		((data_in>>11)&0x1);
	uint32_t P4 = 
 284:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
 286:	4079      	eors	r1, r7
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
		((data_in>>12)&0x1) ^ 
		((data_in>>11)&0x1);
	uint32_t P4 = 
 288:	404b      	eors	r3, r1
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
 28a:	0881      	lsrs	r1, r0, #2
 28c:	4021      	ands	r1, r4
 28e:	9108      	str	r1, [sp, #32]
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
		((data_in>>12)&0x1) ^ 
		((data_in>>11)&0x1);
	uint32_t P4 = 
 290:	9317      	str	r3, [sp, #92]	; 0x5c
		((data_in>>14)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
 292:	08c3      	lsrs	r3, r0, #3
 294:	4023      	ands	r3, r4
		((data_in>>16)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
 296:	9f08      	ldr	r7, [sp, #32]
		((data_in>>7 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>1 )&0x1);
 298:	0841      	lsrs	r1, r0, #1
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
 29a:	405e      	eors	r6, r3
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>1 )&0x1);
 29c:	4021      	ands	r1, r4
		((data_in>>16)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
 29e:	407e      	eors	r6, r7
		((data_in>>7 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
 2a0:	9f03      	ldr	r7, [sp, #12]
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
 2a2:	404e      	eors	r6, r1
		((data_in>>3 )&0x1) ^ 
 2a4:	407e      	eors	r6, r7
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
		((data_in>>4 )&0x1);
	uint32_t P3 = 
 2a6:	9f12      	ldr	r7, [sp, #72]	; 0x48
		((data_in>>9 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>0 )&0x1);
 2a8:	4004      	ands	r4, r0
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
		((data_in>>4 )&0x1);
	uint32_t P3 = 
 2aa:	4077      	eors	r7, r6
 2ac:	9703      	str	r7, [sp, #12]
		((data_in>>2 )&0x1) ^ 
		((data_in>>1 )&0x1);
	uint32_t P2 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
 2ae:	9e0d      	ldr	r6, [sp, #52]	; 0x34
		((data_in>>20)&0x1) ^ 
 2b0:	4667      	mov	r7, ip
		((data_in>>2 )&0x1) ^ 
		((data_in>>1 )&0x1);
	uint32_t P2 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
 2b2:	4066      	eors	r6, r4
		((data_in>>20)&0x1) ^ 
 2b4:	407e      	eors	r6, r7
		((data_in>>17)&0x1) ^ 
 2b6:	9f10      	ldr	r7, [sp, #64]	; 0x40
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>1 )&0x1) ^ 
		((data_in>>0 )&0x1) ^ 
		P5 ^ P4 ^ P3 ^ P2 ^ P1 ;
	uint32_t data_out = (data_in<<6)|(P5<<5)|(P4<<4)|(P3<<3)|(P2<<2)|(P1<<1)|(P0<<0);
 2b8:	0180      	lsls	r0, r0, #6
	uint32_t P2 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
 2ba:	407e      	eors	r6, r7
		((data_in>>16)&0x1) ^ 
 2bc:	9f04      	ldr	r7, [sp, #16]
 2be:	407e      	eors	r6, r7
		((data_in>>13)&0x1) ^ 
 2c0:	9f13      	ldr	r7, [sp, #76]	; 0x4c
 2c2:	407e      	eors	r6, r7
		((data_in>>12)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
 2c4:	9f01      	ldr	r7, [sp, #4]
		((data_in>>24)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
 2c6:	960d      	str	r6, [sp, #52]	; 0x34
		((data_in>>12)&0x1) ^ 
 2c8:	406e      	eors	r6, r5
		((data_in>>10)&0x1) ^ 
 2ca:	407e      	eors	r6, r7
		((data_in>>9 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
 2cc:	9f06      	ldr	r7, [sp, #24]
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
		((data_in>>12)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
 2ce:	4056      	eors	r6, r2
		((data_in>>6 )&0x1) ^ 
 2d0:	407e      	eors	r6, r7
		((data_in>>5 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
 2d2:	9f08      	ldr	r7, [sp, #32]
		((data_in>>13)&0x1) ^ 
		((data_in>>12)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
 2d4:	405e      	eors	r6, r3
		((data_in>>3 )&0x1) ^ 
 2d6:	407e      	eors	r6, r7
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>1 )&0x1);
	uint32_t P2 = 
 2d8:	9f0a      	ldr	r7, [sp, #40]	; 0x28
 2da:	4077      	eors	r7, r6
 2dc:	970a      	str	r7, [sp, #40]	; 0x28
		((data_in>>5 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>0 )&0x1);
	uint32_t P1 = 
		((data_in>>25)&0x1) ^ 
 2de:	9e0b      	ldr	r6, [sp, #44]	; 0x2c
 2e0:	9f09      	ldr	r7, [sp, #36]	; 0x24
 2e2:	4077      	eors	r7, r6
		((data_in>>23)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
 2e4:	9e0c      	ldr	r6, [sp, #48]	; 0x30
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>0 )&0x1);
	uint32_t P1 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
 2e6:	4067      	eors	r7, r4
		((data_in>>21)&0x1) ^ 
 2e8:	4077      	eors	r7, r6
		((data_in>>19)&0x1) ^ 
 2ea:	9e0e      	ldr	r6, [sp, #56]	; 0x38
 2ec:	4077      	eors	r7, r6
		((data_in>>17)&0x1) ^ 
 2ee:	4666      	mov	r6, ip
 2f0:	4077      	eors	r7, r6
		((data_in>>15)&0x1) ^ 
 2f2:	9e11      	ldr	r6, [sp, #68]	; 0x44
 2f4:	4077      	eors	r7, r6
		((data_in>>13)&0x1) ^ 
 2f6:	9e04      	ldr	r6, [sp, #16]
 2f8:	4077      	eors	r7, r6
		((data_in>>11)&0x1) ^ 
 2fa:	9e14      	ldr	r6, [sp, #80]	; 0x50
 2fc:	4077      	eors	r7, r6
		((data_in>>10)&0x1) ^ 
 2fe:	407d      	eors	r5, r7
		((data_in>>8 )&0x1) ^
 300:	9f05      	ldr	r7, [sp, #20]
		((data_in>>6 )&0x1) ^ 
		((data_in>>4 )&0x1) ^ 
 302:	9e07      	ldr	r6, [sp, #28]
		((data_in>>17)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
		((data_in>>11)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>8 )&0x1) ^
 304:	407d      	eors	r5, r7
		((data_in>>6 )&0x1) ^ 
 306:	4055      	eors	r5, r2
		((data_in>>4 )&0x1) ^ 
 308:	4075      	eors	r5, r6
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
 30a:	9f13      	ldr	r7, [sp, #76]	; 0x4c
 30c:	9e04      	ldr	r6, [sp, #16]
		((data_in>>11)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>8 )&0x1) ^
		((data_in>>6 )&0x1) ^ 
		((data_in>>4 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
 30e:	405d      	eors	r5, r3
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
 310:	4077      	eors	r7, r6
		((data_in>>14)&0x1) ^ 
 312:	9e14      	ldr	r6, [sp, #80]	; 0x50
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>0 )&0x1);
	uint32_t P1 = 
 314:	404d      	eors	r5, r1
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
 316:	4077      	eors	r7, r6
		((data_in>>13)&0x1) ^ 
 318:	9e12      	ldr	r6, [sp, #72]	; 0x48
 31a:	4077      	eors	r7, r6
// Output Format =
//  data_out[31:6] = data_in[25:0]
//  data_out[ 5:0] = 6bit ECC

uint32_t generate_ECC( uint32_t data_in ) {
	uint32_t P5 = 
 31c:	9e0f      	ldr	r6, [sp, #60]	; 0x3c
 31e:	4077      	eors	r7, r6
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>1 )&0x1) ^ 
		((data_in>>0 )&0x1) ^ 
		P5 ^ P4 ^ P3 ^ P2 ^ P1 ;
	uint32_t data_out = (data_in<<6)|(P5<<5)|(P4<<4)|(P3<<3)|(P2<<2)|(P1<<1)|(P0<<0);
 320:	017f      	lsls	r7, r7, #5
 322:	9e17      	ldr	r6, [sp, #92]	; 0x5c
 324:	46bc      	mov	ip, r7
 326:	0137      	lsls	r7, r6, #4
 328:	4666      	mov	r6, ip
 32a:	4337      	orrs	r7, r6
 32c:	9e03      	ldr	r6, [sp, #12]
 32e:	4307      	orrs	r7, r0
 330:	00f0      	lsls	r0, r6, #3
 332:	9e0a      	ldr	r6, [sp, #40]	; 0x28
 334:	4307      	orrs	r7, r0
 336:	00b0      	lsls	r0, r6, #2
 338:	4338      	orrs	r0, r7
		((data_in>>13)&0x1) ^ 
		((data_in>>12)&0x1) ^ 
		((data_in>>11)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
 33a:	9f15      	ldr	r7, [sp, #84]	; 0x54
		((data_in>>7 )&0x1) ^ 
 33c:	9e05      	ldr	r6, [sp, #20]
		((data_in>>13)&0x1) ^ 
		((data_in>>12)&0x1) ^ 
		((data_in>>11)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
 33e:	407c      	eors	r4, r7
		((data_in>>7 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
 340:	9f16      	ldr	r7, [sp, #88]	; 0x58
		((data_in>>12)&0x1) ^ 
		((data_in>>11)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
 342:	4074      	eors	r4, r6
		((data_in>>6 )&0x1) ^ 
 344:	407c      	eors	r4, r7
		((data_in>>5 )&0x1) ^ 
 346:	4062      	eors	r2, r4
		((data_in>>4 )&0x1) ^ 
 348:	9c06      	ldr	r4, [sp, #24]
		((data_in>>3 )&0x1) ^ 
 34a:	9e07      	ldr	r6, [sp, #28]
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
		((data_in>>4 )&0x1) ^ 
 34c:	4062      	eors	r2, r4
		((data_in>>3 )&0x1) ^ 
 34e:	4072      	eors	r2, r6
		((data_in>>2 )&0x1) ^ 
		((data_in>>1 )&0x1) ^ 
 350:	9f08      	ldr	r7, [sp, #32]
		((data_in>>7 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
		((data_in>>4 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
 352:	4053      	eors	r3, r2
		((data_in>>1 )&0x1) ^ 
 354:	407b      	eors	r3, r7
		((data_in>>0 )&0x1) ^ 
		P5 ^ P4 ^ P3 ^ P2 ^ P1 ;
 356:	9a17      	ldr	r2, [sp, #92]	; 0x5c
		((data_in>>5 )&0x1) ^ 
		((data_in>>4 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>1 )&0x1) ^ 
		((data_in>>0 )&0x1) ^ 
 358:	4059      	eors	r1, r3
		P5 ^ P4 ^ P3 ^ P2 ^ P1 ;
 35a:	9b03      	ldr	r3, [sp, #12]
 35c:	4051      	eors	r1, r2
 35e:	9e0a      	ldr	r6, [sp, #40]	; 0x28
 360:	4059      	eors	r1, r3
 362:	404e      	eors	r6, r1
		((data_in>>6 )&0x1) ^ 
		((data_in>>4 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>1 )&0x1) ^ 
		((data_in>>0 )&0x1);
	uint32_t P0 = 
 364:	406e      	eors	r6, r5
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>1 )&0x1) ^ 
		((data_in>>0 )&0x1) ^ 
		P5 ^ P4 ^ P3 ^ P2 ^ P1 ;
	uint32_t data_out = (data_in<<6)|(P5<<5)|(P4<<4)|(P3<<3)|(P2<<2)|(P1<<1)|(P0<<0);
 366:	4330      	orrs	r0, r6
 368:	006d      	lsls	r5, r5, #1
 36a:	4328      	orrs	r0, r5
	return data_out;	
}
 36c:	b019      	add	sp, #100	; 0x64
 36e:	bdf0      	pop	{r4, r5, r6, r7, pc}

Disassembly of section .text.write_mbus_message:

00000370 <write_mbus_message>:
#include "mbus.h"

//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
 370:	23a0      	movs	r3, #160	; 0xa0
 372:	061b      	lsls	r3, r3, #24
 374:	6018      	str	r0, [r3, #0]
  _mbus_addr |= 0x4;
  *((volatile uint32_t *) _mbus_addr) = data;
 376:	4b02      	ldr	r3, [pc, #8]	; (380 <write_mbus_message+0x10>)
  return 0;
}
 378:	2000      	movs	r0, #0
//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
  _mbus_addr |= 0x4;
  *((volatile uint32_t *) _mbus_addr) = data;
 37a:	6019      	str	r1, [r3, #0]
  return 0;
}
 37c:	4770      	bx	lr
 37e:	46c0      	nop			; (mov r8, r8)
 380:	a0000004 	.word	0xa0000004

Disassembly of section .text.enumerate:

00000384 <enumerate>:

//Enumerates
int enumerate(uint32_t addr) {
  uint32_t _mbus_data = 0x20000000;
  _mbus_data |= (addr << 24);
 384:	2280      	movs	r2, #128	; 0x80
 386:	0600      	lsls	r0, r0, #24
 388:	0592      	lsls	r2, r2, #22
#include "mbus.h"

//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
 38a:	23a0      	movs	r3, #160	; 0xa0
}

//Enumerates
int enumerate(uint32_t addr) {
  uint32_t _mbus_data = 0x20000000;
  _mbus_data |= (addr << 24);
 38c:	4302      	orrs	r2, r0
#include "mbus.h"

//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
 38e:	061b      	lsls	r3, r3, #24
 390:	2000      	movs	r0, #0
 392:	6018      	str	r0, [r3, #0]
  _mbus_addr |= 0x4;
  *((volatile uint32_t *) _mbus_addr) = data;
 394:	4b01      	ldr	r3, [pc, #4]	; (39c <enumerate+0x18>)
 396:	601a      	str	r2, [r3, #0]
int enumerate(uint32_t addr) {
  uint32_t _mbus_data = 0x20000000;
  _mbus_data |= (addr << 24);
  write_mbus_message(0x0, _mbus_data);
  return 0;
}
 398:	4770      	bx	lr
 39a:	46c0      	nop			; (mov r8, r8)
 39c:	a0000004 	.word	0xa0000004

Disassembly of section .text.sleep:

000003a0 <sleep>:
#include "mbus.h"

//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
 3a0:	23a0      	movs	r3, #160	; 0xa0
 3a2:	2201      	movs	r2, #1
 3a4:	061b      	lsls	r3, r3, #24
 3a6:	601a      	str	r2, [r3, #0]
  _mbus_addr |= 0x4;
  *((volatile uint32_t *) _mbus_addr) = data;
 3a8:	4b01      	ldr	r3, [pc, #4]	; (3b0 <sleep+0x10>)
 3aa:	2000      	movs	r0, #0
 3ac:	6018      	str	r0, [r3, #0]

//Set system to sleep
int sleep(){
  write_mbus_message(0x1,0x0);
  return 0;
}
 3ae:	4770      	bx	lr
 3b0:	a0000004 	.word	0xa0000004

Disassembly of section .text.write_mbus_register:

000003b4 <write_mbus_register>:
#include "mbus.h"

//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
 3b4:	23a0      	movs	r3, #160	; 0xa0

//Writes Register <reg> on <enum_addr> layer with <data>
int write_mbus_register(uint32_t enum_addr, uint8_t reg, uint32_t data){
  uint32_t _mbus_addr = 0;
  uint32_t _mbus_data = 0;
  _mbus_addr |= (enum_addr << 4);
 3b6:	0100      	lsls	r0, r0, #4
#include "mbus.h"

//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
 3b8:	061b      	lsls	r3, r3, #24
//Writes Register <reg> on <enum_addr> layer with <data>
int write_mbus_register(uint32_t enum_addr, uint8_t reg, uint32_t data){
  uint32_t _mbus_addr = 0;
  uint32_t _mbus_data = 0;
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24) | (data&0xFFFFFF); //Only use bottom 24 bits!
 3ba:	0212      	lsls	r2, r2, #8
#include "mbus.h"

//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
 3bc:	6018      	str	r0, [r3, #0]
//Writes Register <reg> on <enum_addr> layer with <data>
int write_mbus_register(uint32_t enum_addr, uint8_t reg, uint32_t data){
  uint32_t _mbus_addr = 0;
  uint32_t _mbus_data = 0;
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24) | (data&0xFFFFFF); //Only use bottom 24 bits!
 3be:	0a12      	lsrs	r2, r2, #8
//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
  _mbus_addr |= 0x4;
  *((volatile uint32_t *) _mbus_addr) = data;
 3c0:	4b02      	ldr	r3, [pc, #8]	; (3cc <write_mbus_register+0x18>)
//Writes Register <reg> on <enum_addr> layer with <data>
int write_mbus_register(uint32_t enum_addr, uint8_t reg, uint32_t data){
  uint32_t _mbus_addr = 0;
  uint32_t _mbus_data = 0;
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24) | (data&0xFFFFFF); //Only use bottom 24 bits!
 3c2:	0609      	lsls	r1, r1, #24
 3c4:	4311      	orrs	r1, r2
  write_mbus_message(_mbus_addr,_mbus_data);
  return 0;
}
 3c6:	2000      	movs	r0, #0
//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
  _mbus_addr |= 0x4;
  *((volatile uint32_t *) _mbus_addr) = data;
 3c8:	6019      	str	r1, [r3, #0]
  uint32_t _mbus_data = 0;
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24) | (data&0xFFFFFF); //Only use bottom 24 bits!
  write_mbus_message(_mbus_addr,_mbus_data);
  return 0;
}
 3ca:	4770      	bx	lr
 3cc:	a0000004 	.word	0xa0000004

Disassembly of section .text.read_mbus_register:

000003d0 <read_mbus_register>:

//Reads Register <reg> on <enum_addr> layer and returns data to <return_addr>
int read_mbus_register(uint32_t enum_addr, uint8_t reg, uint8_t return_addr){
  uint32_t _mbus_addr = 1;
  uint32_t _mbus_data = (return_addr << 8);
  _mbus_addr |= (enum_addr << 4);
 3d0:	2301      	movs	r3, #1
 3d2:	0100      	lsls	r0, r0, #4
 3d4:	4318      	orrs	r0, r3
#include "mbus.h"

//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
 3d6:	23a0      	movs	r3, #160	; 0xa0
 3d8:	061b      	lsls	r3, r3, #24
 3da:	6018      	str	r0, [r3, #0]
}

//Reads Register <reg> on <enum_addr> layer and returns data to <return_addr>
int read_mbus_register(uint32_t enum_addr, uint8_t reg, uint8_t return_addr){
  uint32_t _mbus_addr = 1;
  uint32_t _mbus_data = (return_addr << 8);
 3dc:	0212      	lsls	r2, r2, #8
//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
  _mbus_addr |= 0x4;
  *((volatile uint32_t *) _mbus_addr) = data;
 3de:	4b03      	ldr	r3, [pc, #12]	; (3ec <read_mbus_register+0x1c>)
//Reads Register <reg> on <enum_addr> layer and returns data to <return_addr>
int read_mbus_register(uint32_t enum_addr, uint8_t reg, uint8_t return_addr){
  uint32_t _mbus_addr = 1;
  uint32_t _mbus_data = (return_addr << 8);
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24);
 3e0:	0609      	lsls	r1, r1, #24
 3e2:	4311      	orrs	r1, r2
  write_mbus_message(_mbus_addr,_mbus_data);
  return 0;
}
 3e4:	2000      	movs	r0, #0
//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
  _mbus_addr |= 0x4;
  *((volatile uint32_t *) _mbus_addr) = data;
 3e6:	6019      	str	r1, [r3, #0]
  uint32_t _mbus_data = (return_addr << 8);
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24);
  write_mbus_message(_mbus_addr,_mbus_data);
  return 0;
}
 3e8:	4770      	bx	lr
 3ea:	46c0      	nop			; (mov r8, r8)
 3ec:	a0000004 	.word	0xa0000004

Disassembly of section .text.read_mbus_register_RADv4:

000003f0 <read_mbus_register_RADv4>:
//Reads Register <reg> on <enum_addr> layer and returns data to <return_addr>
//ONLY FOR RADv4!
int read_mbus_register_RADv4(uint32_t enum_addr, uint8_t reg, uint8_t return_addr){
  uint32_t _mbus_addr = 1;
  uint32_t _mbus_data = (return_addr << 16);
  _mbus_addr |= (enum_addr << 4);
 3f0:	2301      	movs	r3, #1
 3f2:	0100      	lsls	r0, r0, #4
 3f4:	4318      	orrs	r0, r3
#include "mbus.h"

//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
 3f6:	23a0      	movs	r3, #160	; 0xa0
 3f8:	061b      	lsls	r3, r3, #24
 3fa:	6018      	str	r0, [r3, #0]

//Reads Register <reg> on <enum_addr> layer and returns data to <return_addr>
//ONLY FOR RADv4!
int read_mbus_register_RADv4(uint32_t enum_addr, uint8_t reg, uint8_t return_addr){
  uint32_t _mbus_addr = 1;
  uint32_t _mbus_data = (return_addr << 16);
 3fc:	0412      	lsls	r2, r2, #16
//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
  _mbus_addr |= 0x4;
  *((volatile uint32_t *) _mbus_addr) = data;
 3fe:	4b03      	ldr	r3, [pc, #12]	; (40c <read_mbus_register_RADv4+0x1c>)
//ONLY FOR RADv4!
int read_mbus_register_RADv4(uint32_t enum_addr, uint8_t reg, uint8_t return_addr){
  uint32_t _mbus_addr = 1;
  uint32_t _mbus_data = (return_addr << 16);
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24);
 400:	0609      	lsls	r1, r1, #24
 402:	4311      	orrs	r1, r2
  write_mbus_message(_mbus_addr,_mbus_data);
  return 0;
}
 404:	2000      	movs	r0, #0
//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
  _mbus_addr |= 0x4;
  *((volatile uint32_t *) _mbus_addr) = data;
 406:	6019      	str	r1, [r3, #0]
  uint32_t _mbus_data = (return_addr << 16);
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24);
  write_mbus_message(_mbus_addr,_mbus_data);
  return 0;
}
 408:	4770      	bx	lr
 40a:	46c0      	nop			; (mov r8, r8)
 40c:	a0000004 	.word	0xa0000004

Disassembly of section .text.unlikely.send_radio_data:

00000410 <send_radio_data>:
//Send Radio Data MSB-->LSB
//Two Delays:
//Bit Delay: Delay between each bit (20-bit data, 4-bit header)
//Delays are nop delays (therefore dependent on core speed)
//***************************************************
static void send_radio_data(uint32_t radio_data){
 410:	b538      	push	{r3, r4, r5, lr}
 412:	1c05      	adds	r5, r0, #0
  int32_t i; //loop var
  uint32_t j; //loop var
	for(i=23;i>=0;i--){ //Bit Loop
 414:	2417      	movs	r4, #23
		if ((radio_data>>i)&1) write_mbus_register(RAD_ADDR,0x27,0x1);
 416:	1c2a      	adds	r2, r5, #0
 418:	2301      	movs	r3, #1
 41a:	40e2      	lsrs	r2, r4
 41c:	401a      	ands	r2, r3
 41e:	2004      	movs	r0, #4
 420:	2127      	movs	r1, #39	; 0x27
 422:	2a00      	cmp	r2, #0
 424:	d000      	beq.n	428 <send_radio_data+0x18>
 426:	1c1a      	adds	r2, r3, #0
		else                   write_mbus_register(RAD_ADDR,0x27,0x0);
 428:	f7ff ffc4 	bl	3b4 <write_mbus_register>
		//Must clear register
		delay(RAD_BIT_DELAY);
 42c:	2022      	movs	r0, #34	; 0x22
 42e:	f7ff fe34 	bl	9a <delay>
		write_mbus_register(RAD_ADDR,0x27,0x0);
 432:	2004      	movs	r0, #4
 434:	2127      	movs	r1, #39	; 0x27
 436:	2200      	movs	r2, #0
 438:	f7ff ffbc 	bl	3b4 <write_mbus_register>
		delay(RAD_BIT_DELAY); //Set delay between sending subsequent bit
 43c:	2022      	movs	r0, #34	; 0x22
 43e:	f7ff fe2c 	bl	9a <delay>
//Delays are nop delays (therefore dependent on core speed)
//***************************************************
static void send_radio_data(uint32_t radio_data){
  int32_t i; //loop var
  uint32_t j; //loop var
	for(i=23;i>=0;i--){ //Bit Loop
 442:	3c01      	subs	r4, #1
 444:	d2e7      	bcs.n	416 <send_radio_data+0x6>
		//Must clear register
		delay(RAD_BIT_DELAY);
		write_mbus_register(RAD_ADDR,0x27,0x0);
		delay(RAD_BIT_DELAY); //Set delay between sending subsequent bit
	}
}
 446:	bd38      	pop	{r3, r4, r5, pc}

Disassembly of section .text.ldo_power_off:

00000448 <ldo_power_off>:
static void release_cdc_meas(){
    snsv5_r0.CDCW_ENABLE = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void ldo_power_off(){
 448:	b508      	push	{r3, lr}
	snsv5_r18.CDC_LDO_CDC_LDO_DLY_ENB = 0x1;
 44a:	4b0e      	ldr	r3, [pc, #56]	; (484 <ldo_power_off+0x3c>)
 44c:	2102      	movs	r1, #2
 44e:	681a      	ldr	r2, [r3, #0]
	snsv5_r18.ADC_LDO_ADC_LDO_DLY_ENB = 0x1;
	snsv5_r18.CDC_LDO_CDC_LDO_ENB = 0x1;
	snsv5_r18.ADC_LDO_ADC_LDO_ENB = 0x1;
	write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
 450:	2005      	movs	r0, #5
    snsv5_r0.CDCW_ENABLE = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void ldo_power_off(){
	snsv5_r18.CDC_LDO_CDC_LDO_DLY_ENB = 0x1;
 452:	430a      	orrs	r2, r1
 454:	601a      	str	r2, [r3, #0]
	snsv5_r18.ADC_LDO_ADC_LDO_DLY_ENB = 0x1;
 456:	6819      	ldr	r1, [r3, #0]
 458:	2280      	movs	r2, #128	; 0x80
 45a:	0152      	lsls	r2, r2, #5
 45c:	430a      	orrs	r2, r1
 45e:	601a      	str	r2, [r3, #0]
	snsv5_r18.CDC_LDO_CDC_LDO_ENB = 0x1;
 460:	681a      	ldr	r2, [r3, #0]
 462:	2101      	movs	r1, #1
 464:	430a      	orrs	r2, r1
 466:	601a      	str	r2, [r3, #0]
	snsv5_r18.ADC_LDO_ADC_LDO_ENB = 0x1;
 468:	6819      	ldr	r1, [r3, #0]
 46a:	2280      	movs	r2, #128	; 0x80
 46c:	0112      	lsls	r2, r2, #4
 46e:	430a      	orrs	r2, r1
 470:	601a      	str	r2, [r3, #0]
	write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
 472:	681a      	ldr	r2, [r3, #0]
 474:	2112      	movs	r1, #18
 476:	f7ff ff9d 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 47a:	2064      	movs	r0, #100	; 0x64
 47c:	f7ff fe0d 	bl	9a <delay>
}
 480:	bd08      	pop	{r3, pc}
 482:	46c0      	nop			; (mov r8, r8)
 484:	00000c58 	.word	0x00000c58

Disassembly of section .text.release_cdc_meas:

00000488 <release_cdc_meas>:
static void fire_cdc_meas(){
    snsv5_r0.CDCW_ENABLE = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_meas(){
 488:	b508      	push	{r3, lr}
    snsv5_r0.CDCW_ENABLE = 0x0;
 48a:	4b07      	ldr	r3, [pc, #28]	; (4a8 <release_cdc_meas+0x20>)
 48c:	2180      	movs	r1, #128	; 0x80
 48e:	881a      	ldrh	r2, [r3, #0]
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 490:	2005      	movs	r0, #5
    snsv5_r0.CDCW_ENABLE = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_meas(){
    snsv5_r0.CDCW_ENABLE = 0x0;
 492:	438a      	bics	r2, r1
 494:	801a      	strh	r2, [r3, #0]
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 496:	681a      	ldr	r2, [r3, #0]
 498:	2100      	movs	r1, #0
 49a:	f7ff ff8b 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 49e:	2064      	movs	r0, #100	; 0x64
 4a0:	f7ff fdfb 	bl	9a <delay>
}
 4a4:	bd08      	pop	{r3, pc}
 4a6:	46c0      	nop			; (mov r8, r8)
 4a8:	00000c5c 	.word	0x00000c5c

Disassembly of section .text.assert_cdc_reset:

000004ac <assert_cdc_reset>:
static void release_cdc_isolate(){
    snsv5_r0.CDCW_ISO = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void assert_cdc_reset(){
 4ac:	b508      	push	{r3, lr}
    snsv5_r0.CDCW_RESETn = 0x0;
 4ae:	4b07      	ldr	r3, [pc, #28]	; (4cc <assert_cdc_reset+0x20>)
 4b0:	4a07      	ldr	r2, [pc, #28]	; (4d0 <assert_cdc_reset+0x24>)
 4b2:	8819      	ldrh	r1, [r3, #0]
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 4b4:	2005      	movs	r0, #5
    snsv5_r0.CDCW_ISO = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void assert_cdc_reset(){
    snsv5_r0.CDCW_RESETn = 0x0;
 4b6:	400a      	ands	r2, r1
 4b8:	801a      	strh	r2, [r3, #0]
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 4ba:	681a      	ldr	r2, [r3, #0]
 4bc:	2100      	movs	r1, #0
 4be:	f7ff ff79 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 4c2:	2064      	movs	r0, #100	; 0x64
 4c4:	f7ff fde9 	bl	9a <delay>
}
 4c8:	bd08      	pop	{r3, pc}
 4ca:	46c0      	nop			; (mov r8, r8)
 4cc:	00000c5c 	.word	0x00000c5c
 4d0:	fffffeff 	.word	0xfffffeff

Disassembly of section .text.cdc_power_off:

000004d4 <cdc_power_off>:
	snsv5_r18.CDC_LDO_CDC_LDO_ENB = 0x1;
	snsv5_r18.ADC_LDO_ADC_LDO_ENB = 0x1;
	write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
    delay(MBUS_DELAY);
}
static void cdc_power_off(){
 4d4:	b508      	push	{r3, lr}
    snsv5_r0.CDCW_ISO = 0x1;
 4d6:	4b10      	ldr	r3, [pc, #64]	; (518 <cdc_power_off+0x44>)
 4d8:	2280      	movs	r2, #128	; 0x80
 4da:	8819      	ldrh	r1, [r3, #0]
 4dc:	0092      	lsls	r2, r2, #2
 4de:	430a      	orrs	r2, r1
 4e0:	801a      	strh	r2, [r3, #0]
    snsv5_r0.CDCW_PG_V1P2 = 0x1;
 4e2:	8819      	ldrh	r1, [r3, #0]
 4e4:	2280      	movs	r2, #128	; 0x80
 4e6:	0112      	lsls	r2, r2, #4
 4e8:	430a      	orrs	r2, r1
 4ea:	801a      	strh	r2, [r3, #0]
    snsv5_r0.CDCW_PG_VBAT = 0x1;
 4ec:	8819      	ldrh	r1, [r3, #0]
 4ee:	2280      	movs	r2, #128	; 0x80
 4f0:	00d2      	lsls	r2, r2, #3
 4f2:	430a      	orrs	r2, r1
 4f4:	801a      	strh	r2, [r3, #0]
    snsv5_r0.CDCW_PG_VLDO = 0x1;
 4f6:	8819      	ldrh	r1, [r3, #0]
 4f8:	2280      	movs	r2, #128	; 0x80
 4fa:	0152      	lsls	r2, r2, #5
 4fc:	430a      	orrs	r2, r1
 4fe:	801a      	strh	r2, [r3, #0]
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 500:	681a      	ldr	r2, [r3, #0]
 502:	2100      	movs	r1, #0
 504:	2005      	movs	r0, #5
 506:	f7ff ff55 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 50a:	2064      	movs	r0, #100	; 0x64
 50c:	f7ff fdc5 	bl	9a <delay>
	ldo_power_off();
 510:	f7ff ff9a 	bl	448 <ldo_power_off>
}
 514:	bd08      	pop	{r3, pc}
 516:	46c0      	nop			; (mov r8, r8)
 518:	00000c5c 	.word	0x00000c5c

Disassembly of section .text.unlikely.operation_sleep_noirqreset:

0000051c <operation_sleep_noirqreset>:
  sleep();
  while(1);

}

static void operation_sleep_noirqreset(void){
 51c:	b508      	push	{r3, lr}

  // Reset wakeup counter
  // This is required to go back to sleep!!
  *((volatile uint32_t *) 0xA2000014) = 0x1;
 51e:	4b03      	ldr	r3, [pc, #12]	; (52c <operation_sleep_noirqreset+0x10>)
 520:	2201      	movs	r2, #1
 522:	601a      	str	r2, [r3, #0]

  // Go to Sleep
  sleep();
 524:	f7ff ff3c 	bl	3a0 <sleep>
 528:	e7fe      	b.n	528 <operation_sleep_noirqreset+0xc>
 52a:	46c0      	nop			; (mov r8, r8)
 52c:	a2000014 	.word	0xa2000014

Disassembly of section .text.operation_sleep:

00000530 <operation_sleep>:
}

//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){
 530:	b508      	push	{r3, lr}

  // Reset wakeup counter
  // This is required to go back to sleep!!
  //set_wakeup_timer (0xFFF, 0x0, 0x1);
  *((volatile uint32_t *) 0xA2000014) = 0x1;
 532:	4b04      	ldr	r3, [pc, #16]	; (544 <operation_sleep+0x14>)
 534:	2201      	movs	r2, #1
 536:	601a      	str	r2, [r3, #0]

  // Reset IRQ10VEC
  *((volatile uint32_t *) IRQ10VEC/*IMSG0*/) = 0;
 538:	2200      	movs	r2, #0
 53a:	2368      	movs	r3, #104	; 0x68
 53c:	601a      	str	r2, [r3, #0]

  // Go to Sleep
  sleep();
 53e:	f7ff ff2f 	bl	3a0 <sleep>
 542:	e7fe      	b.n	542 <operation_sleep+0x12>
 544:	a2000014 	.word	0xa2000014

Disassembly of section .text.operation_sleep_notimer:

00000548 <operation_sleep_notimer>:
  sleep();
  while(1);

}

static void operation_sleep_notimer(void){
 548:	b508      	push	{r3, lr}
    
	// Make sure LDO is off
	ldo_power_off();
 54a:	f7ff ff7d 	bl	448 <ldo_power_off>
	// Disable Timer
	set_wakeup_timer (0, 0x0, 0x0);
 54e:	2000      	movs	r0, #0
 550:	1c01      	adds	r1, r0, #0
 552:	1c02      	adds	r2, r0, #0
 554:	f7ff fdc6 	bl	e4 <set_wakeup_timer>
	// Go to sleep without timer
	operation_sleep();
 558:	f7ff ffea 	bl	530 <operation_sleep>

Disassembly of section .text.operation_cdc_run:

0000055c <operation_cdc_run>:
    
    // Go to sleep without timer
    operation_sleep_notimer();
}

static void operation_cdc_run(){
 55c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    uint32_t read_data;
    uint32_t count; 

	if (Pstack_state == PSTK_IDLE){
 55e:	4c91      	ldr	r4, [pc, #580]	; (7a4 <operation_cdc_run+0x248>)
 560:	7825      	ldrb	r5, [r4, #0]
 562:	2d00      	cmp	r5, #0
 564:	d110      	bne.n	588 <operation_cdc_run+0x2c>
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x0);
		#endif
		Pstack_state = PSTK_LDO1;
 566:	2304      	movs	r3, #4
 568:	7023      	strb	r3, [r4, #0]

		cdc_reset_timeout_count = 0;
 56a:	4b8f      	ldr	r3, [pc, #572]	; (7a8 <operation_cdc_run+0x24c>)

		snsv5_r18.CDC_LDO_CDC_LDO_ENB = 0x0;
 56c:	2401      	movs	r4, #1
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x0);
		#endif
		Pstack_state = PSTK_LDO1;

		cdc_reset_timeout_count = 0;
 56e:	601d      	str	r5, [r3, #0]

		snsv5_r18.CDC_LDO_CDC_LDO_ENB = 0x0;
 570:	4b8e      	ldr	r3, [pc, #568]	; (7ac <operation_cdc_run+0x250>)
		//snsv5_r18.ADC_LDO_ADC_LDO_ENB = 0x0;
		write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
 572:	2112      	movs	r1, #18
		#endif
		Pstack_state = PSTK_LDO1;

		cdc_reset_timeout_count = 0;

		snsv5_r18.CDC_LDO_CDC_LDO_ENB = 0x0;
 574:	681a      	ldr	r2, [r3, #0]
		//snsv5_r18.ADC_LDO_ADC_LDO_ENB = 0x0;
		write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
 576:	2005      	movs	r0, #5
		#endif
		Pstack_state = PSTK_LDO1;

		cdc_reset_timeout_count = 0;

		snsv5_r18.CDC_LDO_CDC_LDO_ENB = 0x0;
 578:	43a2      	bics	r2, r4
 57a:	601a      	str	r2, [r3, #0]
		//snsv5_r18.ADC_LDO_ADC_LDO_ENB = 0x0;
		write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
 57c:	681a      	ldr	r2, [r3, #0]
 57e:	f7ff ff19 	bl	3b4 <write_mbus_register>
		// Long delay required here
		// Put system to sleep
		set_wakeup_timer (WAKEUP_PERIOD_LDO, 0x1, 0x0);
 582:	1c20      	adds	r0, r4, #0
 584:	1c21      	adds	r1, r4, #0
 586:	e092      	b.n	6ae <operation_cdc_run+0x152>
		operation_sleep();

	}else if (Pstack_state == PSTK_LDO1){
 588:	7823      	ldrb	r3, [r4, #0]
 58a:	2b04      	cmp	r3, #4
 58c:	d10d      	bne.n	5aa <operation_cdc_run+0x4e>
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x1);
		#endif
		Pstack_state = PSTK_LDO2;
		snsv5_r18.CDC_LDO_CDC_LDO_DLY_ENB = 0x0;
 58e:	4b87      	ldr	r3, [pc, #540]	; (7ac <operation_cdc_run+0x250>)

	}else if (Pstack_state == PSTK_LDO1){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x1);
		#endif
		Pstack_state = PSTK_LDO2;
 590:	2005      	movs	r0, #5
 592:	7020      	strb	r0, [r4, #0]
		snsv5_r18.CDC_LDO_CDC_LDO_DLY_ENB = 0x0;
 594:	681a      	ldr	r2, [r3, #0]
 596:	2102      	movs	r1, #2
 598:	438a      	bics	r2, r1
 59a:	601a      	str	r2, [r3, #0]
		write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
 59c:	2112      	movs	r1, #18
 59e:	681a      	ldr	r2, [r3, #0]
 5a0:	f7ff ff08 	bl	3b4 <write_mbus_register>
		//delay(LDO_DELAY); // This delay is required to avoid current spike
		//snsv5_r18.ADC_LDO_ADC_LDO_DLY_ENB = 0x0;
		//write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
		// Put system to sleep
		set_wakeup_timer (WAKEUP_PERIOD_LDO, 0x1, 0x0);
 5a4:	2001      	movs	r0, #1
 5a6:	1c01      	adds	r1, r0, #0
 5a8:	e0f0      	b.n	78c <operation_cdc_run+0x230>
		operation_sleep();
    
	}else if (Pstack_state == PSTK_LDO2){
 5aa:	7825      	ldrb	r5, [r4, #0]
 5ac:	2d05      	cmp	r5, #5
 5ae:	d134      	bne.n	61a <operation_cdc_run+0xbe>
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x2);
		#endif
		Pstack_state = PSTK_CDC_RST;
 5b0:	2301      	movs	r3, #1
 5b2:	7023      	strb	r3, [r4, #0]
//***************************************************
// CDCW Functions
// SNSv5
//***************************************************
static void release_cdc_pg(){
    snsv5_r0.CDCW_PG_V1P2 = 0x0;
 5b4:	4c7e      	ldr	r4, [pc, #504]	; (7b0 <operation_cdc_run+0x254>)
 5b6:	4b7f      	ldr	r3, [pc, #508]	; (7b4 <operation_cdc_run+0x258>)
 5b8:	8822      	ldrh	r2, [r4, #0]
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 5ba:	2100      	movs	r1, #0
//***************************************************
// CDCW Functions
// SNSv5
//***************************************************
static void release_cdc_pg(){
    snsv5_r0.CDCW_PG_V1P2 = 0x0;
 5bc:	4013      	ands	r3, r2
 5be:	8023      	strh	r3, [r4, #0]
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 5c0:	6822      	ldr	r2, [r4, #0]
 5c2:	1c28      	adds	r0, r5, #0
 5c4:	f7ff fef6 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 5c8:	2064      	movs	r0, #100	; 0x64
 5ca:	f7ff fd66 	bl	9a <delay>
    snsv5_r0.CDCW_PG_VBAT = 0x0;
 5ce:	8822      	ldrh	r2, [r4, #0]
 5d0:	4b79      	ldr	r3, [pc, #484]	; (7b8 <operation_cdc_run+0x25c>)
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 5d2:	2100      	movs	r1, #0
//***************************************************
static void release_cdc_pg(){
    snsv5_r0.CDCW_PG_V1P2 = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
    snsv5_r0.CDCW_PG_VBAT = 0x0;
 5d4:	4013      	ands	r3, r2
 5d6:	8023      	strh	r3, [r4, #0]
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 5d8:	6822      	ldr	r2, [r4, #0]
 5da:	1c28      	adds	r0, r5, #0
 5dc:	f7ff feea 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 5e0:	2064      	movs	r0, #100	; 0x64
 5e2:	f7ff fd5a 	bl	9a <delay>
    snsv5_r0.CDCW_PG_VLDO = 0x0;
 5e6:	8822      	ldrh	r2, [r4, #0]
 5e8:	4b74      	ldr	r3, [pc, #464]	; (7bc <operation_cdc_run+0x260>)
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 5ea:	2100      	movs	r1, #0
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
    snsv5_r0.CDCW_PG_VBAT = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
    snsv5_r0.CDCW_PG_VLDO = 0x0;
 5ec:	4013      	ands	r3, r2
 5ee:	8023      	strh	r3, [r4, #0]
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 5f0:	6822      	ldr	r2, [r4, #0]
 5f2:	1c28      	adds	r0, r5, #0
 5f4:	f7ff fede 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 5f8:	2064      	movs	r0, #100	; 0x64
 5fa:	f7ff fd4e 	bl	9a <delay>
		#endif
		Pstack_state = PSTK_CDC_RST;

		// Release CDC isolation
		release_cdc_pg();
		delay(MBUS_DELAY*2);
 5fe:	20c8      	movs	r0, #200	; 0xc8
 600:	f7ff fd4b 	bl	9a <delay>
    snsv5_r0.CDCW_PG_VLDO = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_isolate(){
    snsv5_r0.CDCW_ISO = 0x0;
 604:	8822      	ldrh	r2, [r4, #0]
 606:	4b6e      	ldr	r3, [pc, #440]	; (7c0 <operation_cdc_run+0x264>)
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 608:	1c28      	adds	r0, r5, #0
    snsv5_r0.CDCW_PG_VLDO = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_isolate(){
    snsv5_r0.CDCW_ISO = 0x0;
 60a:	4013      	ands	r3, r2
 60c:	8023      	strh	r3, [r4, #0]
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 60e:	6822      	ldr	r2, [r4, #0]
 610:	2100      	movs	r1, #0
 612:	f7ff fecf 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 616:	2064      	movs	r0, #100	; 0x64
 618:	e052      	b.n	6c0 <operation_cdc_run+0x164>
		// Release CDC isolation
		release_cdc_pg();
		delay(MBUS_DELAY*2);
		release_cdc_isolate();

	}else if (Pstack_state == PSTK_CDC_RST){
 61a:	7823      	ldrb	r3, [r4, #0]
 61c:	2b01      	cmp	r3, #1
 61e:	d152      	bne.n	6c6 <operation_cdc_run+0x16a>
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x11111111);
			delay(MBUS_DELAY);
		#endif

		MBus_msg_flag = 0;
 620:	4e68      	ldr	r6, [pc, #416]	; (7c4 <operation_cdc_run+0x268>)
    snsv5_r0.CDCW_RESETn = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_reset(){
    snsv5_r0.CDCW_RESETn = 0x1;
 622:	4c63      	ldr	r4, [pc, #396]	; (7b0 <operation_cdc_run+0x254>)
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x11111111);
			delay(MBUS_DELAY);
		#endif

		MBus_msg_flag = 0;
 624:	2500      	movs	r5, #0
 626:	7035      	strb	r5, [r6, #0]
    snsv5_r0.CDCW_RESETn = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_reset(){
    snsv5_r0.CDCW_RESETn = 0x1;
 628:	8822      	ldrh	r2, [r4, #0]
 62a:	2380      	movs	r3, #128	; 0x80
 62c:	005b      	lsls	r3, r3, #1
 62e:	4313      	orrs	r3, r2
 630:	8023      	strh	r3, [r4, #0]
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 632:	6822      	ldr	r2, [r4, #0]
 634:	1c29      	adds	r1, r5, #0
 636:	2005      	movs	r0, #5
 638:	f7ff febc 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 63c:	2064      	movs	r0, #100	; 0x64
 63e:	f7ff fd2c 	bl	9a <delay>
			delay(MBUS_DELAY);
		#endif

		MBus_msg_flag = 0;
		release_cdc_reset();
		delay(MBUS_DELAY*2);
 642:	20c8      	movs	r0, #200	; 0xc8
 644:	f7ff fd29 	bl	9a <delay>
    snsv5_r0.CDCW_RESETn = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void fire_cdc_meas(){
    snsv5_r0.CDCW_ENABLE = 0x1;
 648:	8823      	ldrh	r3, [r4, #0]
 64a:	2280      	movs	r2, #128	; 0x80
 64c:	4313      	orrs	r3, r2
 64e:	8023      	strh	r3, [r4, #0]
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 650:	6822      	ldr	r2, [r4, #0]
 652:	1c29      	adds	r1, r5, #0
 654:	2005      	movs	r0, #5
 656:	f7ff fead 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 65a:	2064      	movs	r0, #100	; 0x64
 65c:	f7ff fd1d 	bl	9a <delay>
 660:	25fa      	movs	r5, #250	; 0xfa
 662:	006d      	lsls	r5, r5, #1
 664:	1c34      	adds	r4, r6, #0
		release_cdc_reset();
		delay(MBUS_DELAY*2);
		fire_cdc_meas();

		for( count=0; count<CDC_TIMEOUT_COUNT; count++ ){
			if( MBus_msg_flag ){
 666:	7823      	ldrb	r3, [r4, #0]
 668:	2b00      	cmp	r3, #0
 66a:	d007      	beq.n	67c <operation_cdc_run+0x120>
				MBus_msg_flag = 0;
				cdc_reset_timeout_count = 0;
 66c:	4a4e      	ldr	r2, [pc, #312]	; (7a8 <operation_cdc_run+0x24c>)
		delay(MBUS_DELAY*2);
		fire_cdc_meas();

		for( count=0; count<CDC_TIMEOUT_COUNT; count++ ){
			if( MBus_msg_flag ){
				MBus_msg_flag = 0;
 66e:	2300      	movs	r3, #0
 670:	7023      	strb	r3, [r4, #0]
				cdc_reset_timeout_count = 0;
 672:	6013      	str	r3, [r2, #0]
				Pstack_state = PSTK_CDC_READ;
 674:	4b4b      	ldr	r3, [pc, #300]	; (7a4 <operation_cdc_run+0x248>)
 676:	2203      	movs	r2, #3
 678:	701a      	strb	r2, [r3, #0]
				return;
 67a:	e092      	b.n	7a2 <operation_cdc_run+0x246>
			}else{
				delay(MBUS_DELAY);
 67c:	2064      	movs	r0, #100	; 0x64
 67e:	3d01      	subs	r5, #1
 680:	f7ff fd0b 	bl	9a <delay>
		MBus_msg_flag = 0;
		release_cdc_reset();
		delay(MBUS_DELAY*2);
		fire_cdc_meas();

		for( count=0; count<CDC_TIMEOUT_COUNT; count++ ){
 684:	2d00      	cmp	r5, #0
 686:	d1ee      	bne.n	666 <operation_cdc_run+0x10a>
		// Time out
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0xFAFAFAFA);
		#endif

		release_cdc_meas();
 688:	f7ff fefe 	bl	488 <release_cdc_meas>
		if (cdc_reset_timeout_count > 0){
 68c:	4c46      	ldr	r4, [pc, #280]	; (7a8 <operation_cdc_run+0x24c>)
 68e:	6823      	ldr	r3, [r4, #0]
 690:	2b00      	cmp	r3, #0
 692:	d00e      	beq.n	6b2 <operation_cdc_run+0x156>
			cdc_reset_timeout_count++;
 694:	6823      	ldr	r3, [r4, #0]
 696:	3301      	adds	r3, #1
 698:	6023      	str	r3, [r4, #0]
			assert_cdc_reset();
 69a:	f7ff ff07 	bl	4ac <assert_cdc_reset>

			if (cdc_reset_timeout_count > 5){
 69e:	6823      	ldr	r3, [r4, #0]
 6a0:	2b05      	cmp	r3, #5
 6a2:	d902      	bls.n	6aa <operation_cdc_run+0x14e>
				// CDC is not resetting for some reason. Go to sleep forever
				Pstack_state = PSTK_IDLE;
 6a4:	4b3f      	ldr	r3, [pc, #252]	; (7a4 <operation_cdc_run+0x248>)
 6a6:	701d      	strb	r5, [r3, #0]
 6a8:	e077      	b.n	79a <operation_cdc_run+0x23e>
				cdc_power_off();
				operation_sleep_notimer();
			}else{
				// Put system to sleep to reset the layer controller
				set_wakeup_timer (WAKEUP_PERIOD_RESET, 0x1, 0x0);
 6aa:	2002      	movs	r0, #2
 6ac:	2101      	movs	r1, #1
 6ae:	1c2a      	adds	r2, r5, #0
 6b0:	e06d      	b.n	78e <operation_cdc_run+0x232>
				operation_sleep();
			}

	    }else{
		// Try one more time
	    	cdc_reset_timeout_count++;
 6b2:	6823      	ldr	r3, [r4, #0]
 6b4:	3301      	adds	r3, #1
 6b6:	6023      	str	r3, [r4, #0]
			assert_cdc_reset();
 6b8:	f7ff fef8 	bl	4ac <assert_cdc_reset>
			delay(MBUS_DELAY*5);
 6bc:	20fa      	movs	r0, #250	; 0xfa
 6be:	0040      	lsls	r0, r0, #1
 6c0:	f7ff fceb 	bl	9a <delay>
 6c4:	e06d      	b.n	7a2 <operation_cdc_run+0x246>
	    }

	}else if (Pstack_state == PSTK_CDC_READ){
 6c6:	7823      	ldrb	r3, [r4, #0]
 6c8:	2b03      	cmp	r3, #3
 6ca:	d164      	bne.n	796 <operation_cdc_run+0x23a>

		// Grab CDC Data
    	uint32_t read_data_reg4;
    	uint32_t read_data_reg6;
		// CDCW_OUT0[23:0]
		read_mbus_register(SNS_ADDR,4,0x15);
 6cc:	2104      	movs	r1, #4
 6ce:	2215      	movs	r2, #21
 6d0:	2005      	movs	r0, #5
 6d2:	f7ff fe7d 	bl	3d0 <read_mbus_register>
		delay(MBUS_DELAY);
		read_data_reg4 = *((volatile uint32_t *) 0xA0001014);
 6d6:	4d3c      	ldr	r5, [pc, #240]	; (7c8 <operation_cdc_run+0x26c>)
		// Grab CDC Data
    	uint32_t read_data_reg4;
    	uint32_t read_data_reg6;
		// CDCW_OUT0[23:0]
		read_mbus_register(SNS_ADDR,4,0x15);
		delay(MBUS_DELAY);
 6d8:	2064      	movs	r0, #100	; 0x64
 6da:	f7ff fcde 	bl	9a <delay>
		read_data_reg4 = *((volatile uint32_t *) 0xA0001014);
		// CDCW_OUT1[23:0]
		read_mbus_register(SNS_ADDR,6,0x15);
 6de:	2106      	movs	r1, #6
 6e0:	2215      	movs	r2, #21
 6e2:	2005      	movs	r0, #5
    	uint32_t read_data_reg4;
    	uint32_t read_data_reg6;
		// CDCW_OUT0[23:0]
		read_mbus_register(SNS_ADDR,4,0x15);
		delay(MBUS_DELAY);
		read_data_reg4 = *((volatile uint32_t *) 0xA0001014);
 6e4:	682e      	ldr	r6, [r5, #0]
		// CDCW_OUT1[23:0]
		read_mbus_register(SNS_ADDR,6,0x15);
 6e6:	f7ff fe73 	bl	3d0 <read_mbus_register>
		delay(MBUS_DELAY);
 6ea:	2064      	movs	r0, #100	; 0x64
 6ec:	f7ff fcd5 	bl	9a <delay>
			delay(MBUS_DELAY*10);
			write_mbus_message(0x70, radio_tx_count);
		#endif

		// Option to take multiple measurements per wakeup
		if (meas_count < 0){	
 6f0:	4b36      	ldr	r3, [pc, #216]	; (7cc <operation_cdc_run+0x270>)
			release_cdc_meas();
			delay(MBUS_DELAY);
			Pstack_state = PSTK_CDC_RST;
			
		}else{
			meas_count = 0;
 6f2:	2700      	movs	r7, #0
		delay(MBUS_DELAY);
		read_data_reg4 = *((volatile uint32_t *) 0xA0001014);
		// CDCW_OUT1[23:0]
		read_mbus_register(SNS_ADDR,6,0x15);
		delay(MBUS_DELAY);
		read_data_reg6 = *((volatile uint32_t *) 0xA0001014);
 6f4:	682d      	ldr	r5, [r5, #0]
			delay(MBUS_DELAY*10);
			write_mbus_message(0x70, radio_tx_count);
		#endif

		// Option to take multiple measurements per wakeup
		if (meas_count < 0){	
 6f6:	681a      	ldr	r2, [r3, #0]
			release_cdc_meas();
			delay(MBUS_DELAY);
			Pstack_state = PSTK_CDC_RST;
			
		}else{
			meas_count = 0;
 6f8:	601f      	str	r7, [r3, #0]

			// Finalize CDC operation
			release_cdc_meas();
 6fa:	f7ff fec5 	bl	488 <release_cdc_meas>
			assert_cdc_reset();
 6fe:	f7ff fed5 	bl	4ac <assert_cdc_reset>
			Pstack_state = PSTK_IDLE;
 702:	7027      	strb	r7, [r4, #0]
			#ifdef DEBUG_MBUS_MSG
				write_mbus_message(0xAA, 0x33333333);
			#endif
		
			// Assert CDC isolation & turn off CDC power
			cdc_power_off();
 704:	f7ff fee6 	bl	4d4 <cdc_power_off>

			// Check if this is for VBAT measurement
			if (cdc_run_single){
 708:	4b31      	ldr	r3, [pc, #196]	; (7d0 <operation_cdc_run+0x274>)
 70a:	781a      	ldrb	r2, [r3, #0]
 70c:	42ba      	cmp	r2, r7
 70e:	d003      	beq.n	718 <operation_cdc_run+0x1bc>
				cdc_run_single = 0;
 710:	701f      	strb	r7, [r3, #0]
				cdc_storage_cref_latest = read_data_reg6;
 712:	4b30      	ldr	r3, [pc, #192]	; (7d4 <operation_cdc_run+0x278>)
 714:	601d      	str	r5, [r3, #0]
				return;
 716:	e044      	b.n	7a2 <operation_cdc_run+0x246>
			}else{
				exec_count++;
 718:	4b2f      	ldr	r3, [pc, #188]	; (7d8 <operation_cdc_run+0x27c>)
 71a:	681a      	ldr	r2, [r3, #0]
 71c:	3201      	adds	r2, #1
 71e:	601a      	str	r2, [r3, #0]
				// Store results in memory; unless buffer is full
				if (cdc_storage_count < CDC_STORAGE_SIZE){
 720:	4b2e      	ldr	r3, [pc, #184]	; (7dc <operation_cdc_run+0x280>)
 722:	681a      	ldr	r2, [r3, #0]
 724:	2a27      	cmp	r2, #39	; 0x27
 726:	d80f      	bhi.n	748 <operation_cdc_run+0x1ec>
					cdc_storage[cdc_storage_count] = read_data_reg4;
 728:	6819      	ldr	r1, [r3, #0]
 72a:	4a2d      	ldr	r2, [pc, #180]	; (7e0 <operation_cdc_run+0x284>)
 72c:	0089      	lsls	r1, r1, #2
 72e:	508e      	str	r6, [r1, r2]
					cdc_storage_cref[cdc_storage_count] = read_data_reg6;
 730:	6819      	ldr	r1, [r3, #0]
 732:	4a2c      	ldr	r2, [pc, #176]	; (7e4 <operation_cdc_run+0x288>)
 734:	0089      	lsls	r1, r1, #2
 736:	508d      	str	r5, [r1, r2]
					cdc_storage_cref_latest = read_data_reg6;
 738:	4a26      	ldr	r2, [pc, #152]	; (7d4 <operation_cdc_run+0x278>)
 73a:	6015      	str	r5, [r2, #0]
					cdc_storage_count++;
 73c:	681a      	ldr	r2, [r3, #0]
 73e:	3201      	adds	r2, #1
 740:	601a      	str	r2, [r3, #0]
					radio_tx_count = cdc_storage_count;
 742:	681a      	ldr	r2, [r3, #0]
 744:	4b28      	ldr	r3, [pc, #160]	; (7e8 <operation_cdc_run+0x28c>)
 746:	601a      	str	r2, [r3, #0]
				}
				// Optionally transmit the data
				if (radio_tx_option){
 748:	4b28      	ldr	r3, [pc, #160]	; (7ec <operation_cdc_run+0x290>)
 74a:	681b      	ldr	r3, [r3, #0]
 74c:	2b00      	cmp	r3, #0
 74e:	d010      	beq.n	772 <operation_cdc_run+0x216>
					send_radio_data(0xF00000 | read_data_reg4);
 750:	24f0      	movs	r4, #240	; 0xf0
 752:	0424      	lsls	r4, r4, #16
 754:	1c20      	adds	r0, r4, #0
 756:	4330      	orrs	r0, r6
 758:	f7ff fe5a 	bl	410 <send_radio_data>
					delay(MBUS_DELAY*10);
 75c:	20fa      	movs	r0, #250	; 0xfa
 75e:	0080      	lsls	r0, r0, #2
 760:	f7ff fc9b 	bl	9a <delay>
					send_radio_data(0xF00000 | read_data_reg6);
 764:	1c20      	adds	r0, r4, #0
 766:	4328      	orrs	r0, r5
 768:	f7ff fe52 	bl	410 <send_radio_data>
					delay(MBUS_DELAY);
 76c:	2064      	movs	r0, #100	; 0x64
 76e:	f7ff fc94 	bl	9a <delay>
				}

				// Enter long sleep
				if(exec_count < 4){
 772:	4b19      	ldr	r3, [pc, #100]	; (7d8 <operation_cdc_run+0x27c>)
 774:	681b      	ldr	r3, [r3, #0]
 776:	2b03      	cmp	r3, #3
 778:	d804      	bhi.n	784 <operation_cdc_run+0x228>
					// Send some signal
					send_radio_data(0xFAF000);
 77a:	481d      	ldr	r0, [pc, #116]	; (7f0 <operation_cdc_run+0x294>)
 77c:	f7ff fe48 	bl	410 <send_radio_data>
					set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
 780:	4b1c      	ldr	r3, [pc, #112]	; (7f4 <operation_cdc_run+0x298>)
 782:	e000      	b.n	786 <operation_cdc_run+0x22a>

				}else{
					set_wakeup_timer (WAKEUP_PERIOD_CONT, 0x1, 0x0);
 784:	4b1c      	ldr	r3, [pc, #112]	; (7f8 <operation_cdc_run+0x29c>)
 786:	6818      	ldr	r0, [r3, #0]
 788:	2101      	movs	r1, #1
 78a:	b280      	uxth	r0, r0
 78c:	2200      	movs	r2, #0
 78e:	f7ff fca9 	bl	e4 <set_wakeup_timer>
				}
				operation_sleep();
 792:	f7ff fecd 	bl	530 <operation_sleep>
		}

	}else{
        //default:  // THIS SHOULD NOT HAPPEN
		// Reset CDC
		assert_cdc_reset();
 796:	f7ff fe89 	bl	4ac <assert_cdc_reset>
		cdc_power_off();
 79a:	f7ff fe9b 	bl	4d4 <cdc_power_off>
		operation_sleep_notimer();
 79e:	f7ff fed3 	bl	548 <operation_sleep_notimer>
	}

}
 7a2:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 7a4:	00000c80 	.word	0x00000c80
 7a8:	00000c74 	.word	0x00000c74
 7ac:	00000c58 	.word	0x00000c58
 7b0:	00000c5c 	.word	0x00000c5c
 7b4:	fffff7ff 	.word	0xfffff7ff
 7b8:	fffffbff 	.word	0xfffffbff
 7bc:	ffffefff 	.word	0xffffefff
 7c0:	fffffdff 	.word	0xfffffdff
 7c4:	00000ca4 	.word	0x00000ca4
 7c8:	a0001014 	.word	0xa0001014
 7cc:	00000ca0 	.word	0x00000ca0
 7d0:	00000c6c 	.word	0x00000c6c
 7d4:	00000ca8 	.word	0x00000ca8
 7d8:	00000c70 	.word	0x00000c70
 7dc:	00000c94 	.word	0x00000c94
 7e0:	00000d58 	.word	0x00000d58
 7e4:	00000cb8 	.word	0x00000cb8
 7e8:	00000c68 	.word	0x00000c68
 7ec:	00000c90 	.word	0x00000c90
 7f0:	00faf000 	.word	0x00faf000
 7f4:	00000c98 	.word	0x00000c98
 7f8:	00000cb0 	.word	0x00000cb0

Disassembly of section .text.handler_ext_int_0:

000007fc <handler_ext_int_0>:
void handler_ext_int_0(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_0(void){
    *((volatile uint32_t *) 0xE000E280) = 0x1;
 7fc:	4b03      	ldr	r3, [pc, #12]	; (80c <handler_ext_int_0+0x10>)
 7fe:	2201      	movs	r2, #1
 800:	601a      	str	r2, [r3, #0]
    MBus_msg_flag = 0x10;
 802:	4b03      	ldr	r3, [pc, #12]	; (810 <handler_ext_int_0+0x14>)
 804:	2210      	movs	r2, #16
 806:	701a      	strb	r2, [r3, #0]
}
 808:	4770      	bx	lr
 80a:	46c0      	nop			; (mov r8, r8)
 80c:	e000e280 	.word	0xe000e280
 810:	00000ca4 	.word	0x00000ca4

Disassembly of section .text.handler_ext_int_1:

00000814 <handler_ext_int_1>:
void handler_ext_int_1(void){
    *((volatile uint32_t *) 0xE000E280) = 0x2;
 814:	4b03      	ldr	r3, [pc, #12]	; (824 <handler_ext_int_1+0x10>)
 816:	2202      	movs	r2, #2
 818:	601a      	str	r2, [r3, #0]
    MBus_msg_flag = 0x11;
 81a:	4b03      	ldr	r3, [pc, #12]	; (828 <handler_ext_int_1+0x14>)
 81c:	2211      	movs	r2, #17
 81e:	701a      	strb	r2, [r3, #0]
}
 820:	4770      	bx	lr
 822:	46c0      	nop			; (mov r8, r8)
 824:	e000e280 	.word	0xe000e280
 828:	00000ca4 	.word	0x00000ca4

Disassembly of section .text.handler_ext_int_2:

0000082c <handler_ext_int_2>:
void handler_ext_int_2(void){
    *((volatile uint32_t *) 0xE000E280) = 0x4;
 82c:	4b03      	ldr	r3, [pc, #12]	; (83c <handler_ext_int_2+0x10>)
 82e:	2204      	movs	r2, #4
 830:	601a      	str	r2, [r3, #0]
    MBus_msg_flag = 0x12;
 832:	4b03      	ldr	r3, [pc, #12]	; (840 <handler_ext_int_2+0x14>)
 834:	2212      	movs	r2, #18
 836:	701a      	strb	r2, [r3, #0]
}
 838:	4770      	bx	lr
 83a:	46c0      	nop			; (mov r8, r8)
 83c:	e000e280 	.word	0xe000e280
 840:	00000ca4 	.word	0x00000ca4

Disassembly of section .text.handler_ext_int_3:

00000844 <handler_ext_int_3>:
void handler_ext_int_3(void){
    *((volatile uint32_t *) 0xE000E280) = 0x8;
 844:	4b03      	ldr	r3, [pc, #12]	; (854 <handler_ext_int_3+0x10>)
 846:	2208      	movs	r2, #8
 848:	601a      	str	r2, [r3, #0]
    MBus_msg_flag = 0x13;
 84a:	4b03      	ldr	r3, [pc, #12]	; (858 <handler_ext_int_3+0x14>)
 84c:	2213      	movs	r2, #19
 84e:	701a      	strb	r2, [r3, #0]
}
 850:	4770      	bx	lr
 852:	46c0      	nop			; (mov r8, r8)
 854:	e000e280 	.word	0xe000e280
 858:	00000ca4 	.word	0x00000ca4

Disassembly of section .text.startup.main:

0000085c <main>:
// MAIN function starts here             
//***************************************************************************************
int main() {
  
    //Clear All Pending Interrupts
    *((volatile uint32_t *) 0xE000E280) = 0xF;
 85c:	4bd9      	ldr	r3, [pc, #868]	; (bc4 <main+0x368>)
}

//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
 85e:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
  
    //Clear All Pending Interrupts
    *((volatile uint32_t *) 0xE000E280) = 0xF;
 860:	270f      	movs	r7, #15
 862:	601f      	str	r7, [r3, #0]
    //Enable Interrupts
    *((volatile uint32_t *) 0xE000E100) = 0xF;
 864:	4bd8      	ldr	r3, [pc, #864]	; (bc8 <main+0x36c>)
  
	//Config watchdog timer to about 10 sec: 1,000,000 with default PRCv9
	//config_timer( timer_id, go, roi, init_val, sat_val )
	config_timer( 0, 1, 0, 0, 1000000 );
 866:	2000      	movs	r0, #0
int main() {
  
    //Clear All Pending Interrupts
    *((volatile uint32_t *) 0xE000E280) = 0xF;
    //Enable Interrupts
    *((volatile uint32_t *) 0xE000E100) = 0xF;
 868:	601f      	str	r7, [r3, #0]
  
	//Config watchdog timer to about 10 sec: 1,000,000 with default PRCv9
	//config_timer( timer_id, go, roi, init_val, sat_val )
	config_timer( 0, 1, 0, 0, 1000000 );
 86a:	4bd8      	ldr	r3, [pc, #864]	; (bcc <main+0x370>)
 86c:	2101      	movs	r1, #1
 86e:	9300      	str	r3, [sp, #0]
 870:	1c02      	adds	r2, r0, #0
 872:	1c03      	adds	r3, r0, #0
 874:	f7ff fc1c 	bl	b0 <config_timer>

    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
 878:	4dd5      	ldr	r5, [pc, #852]	; (bd0 <main+0x374>)
 87a:	4ed6      	ldr	r6, [pc, #856]	; (bd4 <main+0x378>)
 87c:	682b      	ldr	r3, [r5, #0]
 87e:	42b3      	cmp	r3, r6
 880:	d100      	bne.n	884 <main+0x28>
 882:	e0b8      	b.n	9f6 <main+0x19a>
    // Change PMU_CTRL Register
    // PRCv9 Default: 0x8F770049
    // Decrease 5x division switching threshold
    //*((volatile uint32_t *) 0xA200000C) = 0x8F77004B;
	// Increase active pmu clock (for PRCv11)
    *((volatile uint32_t *) 0xA200000C) = 0x8F77184B;
 884:	4ad4      	ldr	r2, [pc, #848]	; (bd8 <main+0x37c>)
 886:	4bd5      	ldr	r3, [pc, #852]	; (bdc <main+0x380>)
    //*((volatile uint32_t *) 0xA2000008) = 0x00202908;
	// Slow down MBUS frequency 
	// Gyouho: This is required for running on the board w/o PMU assist
    *((volatile uint32_t *) 0xA2000008) = 0x00202D08;
  
    delay(100);
 888:	2064      	movs	r0, #100	; 0x64
    // Change PMU_CTRL Register
    // PRCv9 Default: 0x8F770049
    // Decrease 5x division switching threshold
    //*((volatile uint32_t *) 0xA200000C) = 0x8F77004B;
	// Increase active pmu clock (for PRCv11)
    *((volatile uint32_t *) 0xA200000C) = 0x8F77184B;
 88a:	601a      	str	r2, [r3, #0]
    // Speed up GOC frontend to match PMU frequency
    // PRCv9 Default: 0x00202903
    //*((volatile uint32_t *) 0xA2000008) = 0x00202908;
	// Slow down MBUS frequency 
	// Gyouho: This is required for running on the board w/o PMU assist
    *((volatile uint32_t *) 0xA2000008) = 0x00202D08;
 88c:	4ad4      	ldr	r2, [pc, #848]	; (be0 <main+0x384>)
 88e:	4bd5      	ldr	r3, [pc, #852]	; (be4 <main+0x388>)
  
    delay(100);
  
    //Enumerate & Initialize Registers
    Pstack_state = PSTK_IDLE; 	//0x0;
 890:	2400      	movs	r4, #0
    // Speed up GOC frontend to match PMU frequency
    // PRCv9 Default: 0x00202903
    //*((volatile uint32_t *) 0xA2000008) = 0x00202908;
	// Slow down MBUS frequency 
	// Gyouho: This is required for running on the board w/o PMU assist
    *((volatile uint32_t *) 0xA2000008) = 0x00202D08;
 892:	601a      	str	r2, [r3, #0]
  
    delay(100);
 894:	f7ff fc01 	bl	9a <delay>
  
    //Enumerate & Initialize Registers
    Pstack_state = PSTK_IDLE; 	//0x0;
 898:	4bd3      	ldr	r3, [pc, #844]	; (be8 <main+0x38c>)
    exec_count = 0;
    exec_count_irq = 0;
    MBus_msg_flag = 0;

    //Enumeration
    enumerate(RAD_ADDR);
 89a:	2004      	movs	r0, #4
    *((volatile uint32_t *) 0xA2000008) = 0x00202D08;
  
    delay(100);
  
    //Enumerate & Initialize Registers
    Pstack_state = PSTK_IDLE; 	//0x0;
 89c:	701c      	strb	r4, [r3, #0]
    enumerated = 0xDEADBEEF;
    exec_count = 0;
 89e:	4bd3      	ldr	r3, [pc, #844]	; (bec <main+0x390>)
  
    delay(100);
  
    //Enumerate & Initialize Registers
    Pstack_state = PSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEF;
 8a0:	602e      	str	r6, [r5, #0]
    exec_count = 0;
 8a2:	601c      	str	r4, [r3, #0]
    exec_count_irq = 0;
 8a4:	4bd2      	ldr	r3, [pc, #840]	; (bf0 <main+0x394>)
    enumerate(HRV_ADDR);
    delay(MBUS_DELAY*10);

    // CDC Settings --------------------------------------
    // snsv5_r0
    snsv5_r0.CDCW_IRQ_EN	= 1;
 8a6:	2501      	movs	r5, #1
  
    //Enumerate & Initialize Registers
    Pstack_state = PSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    exec_count_irq = 0;
 8a8:	601c      	str	r4, [r3, #0]
    MBus_msg_flag = 0;
 8aa:	4bd2      	ldr	r3, [pc, #840]	; (bf4 <main+0x398>)
    delay(MBUS_DELAY*10);

    // CDC Settings --------------------------------------
    // snsv5_r0
    snsv5_r0.CDCW_IRQ_EN	= 1;
    snsv5_r0.CDCW_MODE_PAR	= 0;
 8ac:	2602      	movs	r6, #2
    //Enumerate & Initialize Registers
    Pstack_state = PSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    exec_count_irq = 0;
    MBus_msg_flag = 0;
 8ae:	701c      	strb	r4, [r3, #0]

    //Enumeration
    enumerate(RAD_ADDR);
 8b0:	f7ff fd68 	bl	384 <enumerate>
    delay(MBUS_DELAY*10);
 8b4:	20fa      	movs	r0, #250	; 0xfa
 8b6:	0080      	lsls	r0, r0, #2
 8b8:	f7ff fbef 	bl	9a <delay>

	//write_mbus_message(0x70, radio_tx_count);
	//operation_sleep_notimer();

    enumerate(SNS_ADDR);
 8bc:	2005      	movs	r0, #5
 8be:	f7ff fd61 	bl	384 <enumerate>
    delay(MBUS_DELAY*10);
 8c2:	20fa      	movs	r0, #250	; 0xfa
 8c4:	0080      	lsls	r0, r0, #2
 8c6:	f7ff fbe8 	bl	9a <delay>
    enumerate(HRV_ADDR);
 8ca:	2006      	movs	r0, #6
 8cc:	f7ff fd5a 	bl	384 <enumerate>
    delay(MBUS_DELAY*10);
 8d0:	20fa      	movs	r0, #250	; 0xfa
 8d2:	0080      	lsls	r0, r0, #2
 8d4:	f7ff fbe1 	bl	9a <delay>

    // CDC Settings --------------------------------------
    // snsv5_r0
    snsv5_r0.CDCW_IRQ_EN	= 1;
 8d8:	4bc7      	ldr	r3, [pc, #796]	; (bf8 <main+0x39c>)
    snsv5_r0.CDCW_MODE_PAR	= 0;
    snsv5_r0.CDCW_RESETn 	= 0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 8da:	2005      	movs	r0, #5
    enumerate(HRV_ADDR);
    delay(MBUS_DELAY*10);

    // CDC Settings --------------------------------------
    // snsv5_r0
    snsv5_r0.CDCW_IRQ_EN	= 1;
 8dc:	881a      	ldrh	r2, [r3, #0]
 8de:	432a      	orrs	r2, r5
 8e0:	801a      	strh	r2, [r3, #0]
    snsv5_r0.CDCW_MODE_PAR	= 0;
 8e2:	881a      	ldrh	r2, [r3, #0]
 8e4:	43b2      	bics	r2, r6
 8e6:	801a      	strh	r2, [r3, #0]
    snsv5_r0.CDCW_RESETn 	= 0;
 8e8:	8819      	ldrh	r1, [r3, #0]
 8ea:	4ac4      	ldr	r2, [pc, #784]	; (bfc <main+0x3a0>)
 8ec:	400a      	ands	r2, r1
 8ee:	801a      	strh	r2, [r3, #0]
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
 8f0:	681a      	ldr	r2, [r3, #0]
 8f2:	1c21      	adds	r1, r4, #0
 8f4:	f7ff fd5e 	bl	3b4 <write_mbus_register>

    // snsv5_r1
    snsv5_r1.CDCW_N_CYCLE_SINGLE	= 1; // Default: 8; Min: 1
 8f8:	4bc1      	ldr	r3, [pc, #772]	; (c00 <main+0x3a4>)
    write_mbus_register(SNS_ADDR,1,snsv5_r1.as_int);
 8fa:	1c29      	adds	r1, r5, #0
    snsv5_r0.CDCW_MODE_PAR	= 0;
    snsv5_r0.CDCW_RESETn 	= 0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);

    // snsv5_r1
    snsv5_r1.CDCW_N_CYCLE_SINGLE	= 1; // Default: 8; Min: 1
 8fc:	681a      	ldr	r2, [r3, #0]
    write_mbus_register(SNS_ADDR,1,snsv5_r1.as_int);
 8fe:	2005      	movs	r0, #5
    snsv5_r0.CDCW_MODE_PAR	= 0;
    snsv5_r0.CDCW_RESETn 	= 0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);

    // snsv5_r1
    snsv5_r1.CDCW_N_CYCLE_SINGLE	= 1; // Default: 8; Min: 1
 900:	0a92      	lsrs	r2, r2, #10
 902:	0292      	lsls	r2, r2, #10
 904:	432a      	orrs	r2, r5
 906:	601a      	str	r2, [r3, #0]
    write_mbus_register(SNS_ADDR,1,snsv5_r1.as_int);
 908:	681a      	ldr	r2, [r3, #0]
 90a:	f7ff fd53 	bl	3b4 <write_mbus_register>
	
    // snsv5_r2
    snsv5_r2.CDCW_N_CYCLE_SET	= 100; // Min: 0
 90e:	4bbd      	ldr	r3, [pc, #756]	; (c04 <main+0x3a8>)
 910:	4abd      	ldr	r2, [pc, #756]	; (c08 <main+0x3ac>)
 912:	6819      	ldr	r1, [r3, #0]
    write_mbus_register(SNS_ADDR,2,snsv5_r2.as_int);
 914:	2005      	movs	r0, #5
    // snsv5_r1
    snsv5_r1.CDCW_N_CYCLE_SINGLE	= 1; // Default: 8; Min: 1
    write_mbus_register(SNS_ADDR,1,snsv5_r1.as_int);
	
    // snsv5_r2
    snsv5_r2.CDCW_N_CYCLE_SET	= 100; // Min: 0
 916:	400a      	ands	r2, r1
 918:	21c8      	movs	r1, #200	; 0xc8
 91a:	0249      	lsls	r1, r1, #9
 91c:	430a      	orrs	r2, r1
 91e:	601a      	str	r2, [r3, #0]
    write_mbus_register(SNS_ADDR,2,snsv5_r2.as_int);
 920:	681a      	ldr	r2, [r3, #0]
 922:	1c31      	adds	r1, r6, #0
 924:	f7ff fd46 	bl	3b4 <write_mbus_register>

    // SNSv5_R18
    snsv5_r18.CDC_LDO_CDC_CURRENT_2X  = 0x0;
 928:	4bb8      	ldr	r3, [pc, #736]	; (c0c <main+0x3b0>)
 92a:	4ab9      	ldr	r2, [pc, #740]	; (c10 <main+0x3b4>)
 92c:	6819      	ldr	r1, [r3, #0]

    // Set CDC LDO to around 1.03V: 0x0//0x20
    snsv5_r18.CDC_LDO_CDC_VREF_MUX_SEL = 0x0;
    snsv5_r18.CDC_LDO_CDC_VREF_SEL     = 0x20;

    write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
 92e:	2005      	movs	r0, #5
    // snsv5_r2
    snsv5_r2.CDCW_N_CYCLE_SET	= 100; // Min: 0
    write_mbus_register(SNS_ADDR,2,snsv5_r2.as_int);

    // SNSv5_R18
    snsv5_r18.CDC_LDO_CDC_CURRENT_2X  = 0x0;
 930:	400a      	ands	r2, r1
 932:	601a      	str	r2, [r3, #0]

    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv5_r18.ADC_LDO_ADC_VREF_MUX_SEL = 0x3;
 934:	6819      	ldr	r1, [r3, #0]
 936:	22c0      	movs	r2, #192	; 0xc0
 938:	01d2      	lsls	r2, r2, #7
 93a:	430a      	orrs	r2, r1
 93c:	601a      	str	r2, [r3, #0]
    snsv5_r18.ADC_LDO_ADC_VREF_SEL     = 0x20;
 93e:	6819      	ldr	r1, [r3, #0]
 940:	4ab4      	ldr	r2, [pc, #720]	; (c14 <main+0x3b8>)
 942:	400a      	ands	r2, r1
 944:	2180      	movs	r1, #128	; 0x80
 946:	0349      	lsls	r1, r1, #13
 948:	430a      	orrs	r2, r1
 94a:	601a      	str	r2, [r3, #0]

    // Set CDC LDO to around 1.03V: 0x0//0x20
    snsv5_r18.CDC_LDO_CDC_VREF_MUX_SEL = 0x0;
 94c:	681a      	ldr	r2, [r3, #0]
 94e:	210c      	movs	r1, #12
 950:	438a      	bics	r2, r1
 952:	601a      	str	r2, [r3, #0]
    snsv5_r18.CDC_LDO_CDC_VREF_SEL     = 0x20;
 954:	6819      	ldr	r1, [r3, #0]
 956:	4ab0      	ldr	r2, [pc, #704]	; (c18 <main+0x3bc>)
 958:	400a      	ands	r2, r1
 95a:	2180      	movs	r1, #128	; 0x80
 95c:	40b1      	lsls	r1, r6
 95e:	430a      	orrs	r2, r1
 960:	601a      	str	r2, [r3, #0]

    write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
 962:	681a      	ldr	r2, [r3, #0]
 964:	2112      	movs	r1, #18
 966:	f7ff fd25 	bl	3b4 <write_mbus_register>
    radv5_r22_t radv5_r22; //Tune Freq 2
    radv5_r25_t radv5_r25; //Tune TX Time
    radv5_r27_t radv5_r27; //Zero the TX register
    //RADv5 R23
    radv5_r23.RADIO_EXT_CTRL_EN_1P2 = 0x0; //Ext Ctrl En
    write_mbus_register(RAD_ADDR,0x23,radv5_r23.as_int);
 96a:	2123      	movs	r1, #35	; 0x23
 96c:	1c22      	adds	r2, r4, #0
 96e:	2004      	movs	r0, #4
 970:	f7ff fd20 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
    //RADv5 R26
    radv5_r26.RADIO_TUNE_CURRENT_LIMITER_1P2 = 0x1F; //Current Limiter 2F = 30uA, 1F = 3uA
 974:	261f      	movs	r6, #31
    radv5_r25_t radv5_r25; //Tune TX Time
    radv5_r27_t radv5_r27; //Zero the TX register
    //RADv5 R23
    radv5_r23.RADIO_EXT_CTRL_EN_1P2 = 0x0; //Ext Ctrl En
    write_mbus_register(RAD_ADDR,0x23,radv5_r23.as_int);
    delay(MBUS_DELAY);
 976:	2064      	movs	r0, #100	; 0x64
 978:	f7ff fb8f 	bl	9a <delay>
    //RADv5 R26
    radv5_r26.RADIO_TUNE_CURRENT_LIMITER_1P2 = 0x1F; //Current Limiter 2F = 30uA, 1F = 3uA
    write_mbus_register(RAD_ADDR,0x26,radv5_r26.as_int);
 97c:	2126      	movs	r1, #38	; 0x26
 97e:	1c32      	adds	r2, r6, #0
 980:	2004      	movs	r0, #4
 982:	f7ff fd17 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 986:	2064      	movs	r0, #100	; 0x64
 988:	f7ff fb87 	bl	9a <delay>
    //RADv5 R20
    radv5_r20.RADIO_TUNE_POWER_1P2 = 0x1F; //Tune Power 0x1F = Full Power
 98c:	1c32      	adds	r2, r6, #0
    write_mbus_register(RAD_ADDR,0x20,radv5_r20.as_int);
 98e:	2120      	movs	r1, #32
 990:	2004      	movs	r0, #4
 992:	f7ff fd0f 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 996:	2064      	movs	r0, #100	; 0x64
 998:	f7ff fb7f 	bl	9a <delay>
    //RADv5 R21
    radv5_r21.RADIO_TUNE_FREQ1_1P2 = 0x0; //Tune Freq 1
    write_mbus_register(RAD_ADDR,0x21,radv5_r21.as_int);
 99c:	2121      	movs	r1, #33	; 0x21
 99e:	1c22      	adds	r2, r4, #0
 9a0:	2004      	movs	r0, #4
 9a2:	f7ff fd07 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 9a6:	2064      	movs	r0, #100	; 0x64
 9a8:	f7ff fb77 	bl	9a <delay>
    //RADv5 R22
    radv5_r22.RADIO_TUNE_FREQ2_1P2 = 0x0; //Tune Freq 2 //0x0,0x0 = 902MHz on Pblr005
    write_mbus_register(RAD_ADDR,0x22,radv5_r22.as_int);
 9ac:	2122      	movs	r1, #34	; 0x22
 9ae:	1c22      	adds	r2, r4, #0
 9b0:	2004      	movs	r0, #4
 9b2:	f7ff fcff 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 9b6:	2064      	movs	r0, #100	; 0x64
 9b8:	f7ff fb6f 	bl	9a <delay>
    //RADv5 R25
    radv5_r25.RADIO_TUNE_TX_TIME_1P2 = 0x5; //Tune TX Time  0x5: no pulse; around 2.4ms // 0x4: 380ns
    write_mbus_register(RAD_ADDR,0x25,radv5_r25.as_int);
 9bc:	2125      	movs	r1, #37	; 0x25
 9be:	2205      	movs	r2, #5
 9c0:	2004      	movs	r0, #4
 9c2:	f7ff fcf7 	bl	3b4 <write_mbus_register>
    delay(MBUS_DELAY);
 9c6:	2064      	movs	r0, #100	; 0x64
 9c8:	f7ff fb67 	bl	9a <delay>
    //RADv5 R27
    radv5_r27.RADIO_DATA_1P2 = 0x0; //Zero the TX register
    write_mbus_register(RAD_ADDR,0x27,radv5_r27.as_int);
 9cc:	1c22      	adds	r2, r4, #0
 9ce:	2004      	movs	r0, #4
 9d0:	2127      	movs	r1, #39	; 0x27
 9d2:	f7ff fcef 	bl	3b4 <write_mbus_register>
  
    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 100;   // 1: 2-4 sec with PRCv9
 9d6:	4b91      	ldr	r3, [pc, #580]	; (c1c <main+0x3c0>)
 9d8:	2264      	movs	r2, #100	; 0x64
 9da:	601a      	str	r2, [r3, #0]
    WAKEUP_PERIOD_CONT_INIT = 1;   // 0x1E (30): ~1 min with PRCv9
 9dc:	4b90      	ldr	r3, [pc, #576]	; (c20 <main+0x3c4>)
 9de:	601d      	str	r5, [r3, #0]
    cdc_storage_count = 0;
 9e0:	4b90      	ldr	r3, [pc, #576]	; (c24 <main+0x3c8>)
 9e2:	601c      	str	r4, [r3, #0]
    radio_tx_count = 0;
 9e4:	4b90      	ldr	r3, [pc, #576]	; (c28 <main+0x3cc>)
 9e6:	601c      	str	r4, [r3, #0]
    radio_tx_option = 0;
 9e8:	4b90      	ldr	r3, [pc, #576]	; (c2c <main+0x3d0>)
 9ea:	601c      	str	r4, [r3, #0]
	cdc_run_single = 0;
 9ec:	4b90      	ldr	r3, [pc, #576]	; (c30 <main+0x3d4>)
 9ee:	701c      	strb	r4, [r3, #0]
	cdc_running = 0;
 9f0:	4b90      	ldr	r3, [pc, #576]	; (c34 <main+0x3d8>)
 9f2:	701c      	strb	r4, [r3, #0]
 9f4:	e022      	b.n	a3c <main+0x1e0>
    }

    // Check if wakeup is due to GOC interrupt  
    // 0x68 is reserved for GOC-triggered wakeup (Named IRQ10VEC)
    // 8 MSB bits of the wakeup data are used for function ID
    uint32_t wakeup_data = *((volatile uint32_t *) IRQ10VEC);
 9f6:	2668      	movs	r6, #104	; 0x68
 9f8:	6834      	ldr	r4, [r6, #0]
    uint8_t wakeup_data_header = wakeup_data>>24;
 9fa:	0e25      	lsrs	r5, r4, #24
    uint8_t wakeup_data_field_0 = wakeup_data & 0xFF;
    uint8_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
 9fc:	0a23      	lsrs	r3, r4, #8
    uint8_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;

    if(wakeup_data_header == 1){
 9fe:	2d01      	cmp	r5, #1
 a00:	d11e      	bne.n	a40 <main+0x1e4>
        // Debug mode: Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
 a02:	27ff      	movs	r7, #255	; 0xff
 a04:	4e86      	ldr	r6, [pc, #536]	; (c20 <main+0x3c4>)
 a06:	403b      	ands	r3, r7
        delay(MBUS_DELAY);
 a08:	2064      	movs	r0, #100	; 0x64

    if(wakeup_data_header == 1){
        // Debug mode: Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
 a0a:	6033      	str	r3, [r6, #0]
        delay(MBUS_DELAY);
 a0c:	f7ff fb45 	bl	9a <delay>
        if (exec_count_irq < wakeup_data_field_0){
 a10:	4b77      	ldr	r3, [pc, #476]	; (bf0 <main+0x394>)
 a12:	403c      	ands	r4, r7
 a14:	681a      	ldr	r2, [r3, #0]
 a16:	42a2      	cmp	r2, r4
 a18:	d209      	bcs.n	a2e <main+0x1d2>
            exec_count_irq++;
 a1a:	681a      	ldr	r2, [r3, #0]
 a1c:	3201      	adds	r2, #1
 a1e:	601a      	str	r2, [r3, #0]
            // radio
            send_radio_data(0xFAF000+exec_count_irq);	
 a20:	6818      	ldr	r0, [r3, #0]
 a22:	4b85      	ldr	r3, [pc, #532]	; (c38 <main+0x3dc>)
 a24:	18c0      	adds	r0, r0, r3
 a26:	f7ff fcf3 	bl	410 <send_radio_data>
            // set timer
            set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
 a2a:	6830      	ldr	r0, [r6, #0]
 a2c:	e077      	b.n	b1e <main+0x2c2>
            // go to sleep and wake up with same condition
            operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
 a2e:	2200      	movs	r2, #0
 a30:	601a      	str	r2, [r3, #0]
            // radio
            send_radio_data(0xFAF000+exec_count_irq);	
 a32:	6818      	ldr	r0, [r3, #0]
 a34:	4b80      	ldr	r3, [pc, #512]	; (c38 <main+0x3dc>)
 a36:	18c0      	adds	r0, r0, r3
 a38:	f7ff fcea 	bl	410 <send_radio_data>
            // Go to sleep without timer
            operation_sleep_notimer();
 a3c:	f7ff fd84 	bl	548 <operation_sleep_notimer>
    // 8 MSB bits of the wakeup data are used for function ID
    uint32_t wakeup_data = *((volatile uint32_t *) IRQ10VEC);
    uint8_t wakeup_data_header = wakeup_data>>24;
    uint8_t wakeup_data_field_0 = wakeup_data & 0xFF;
    uint8_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
    uint8_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;
 a40:	0c22      	lsrs	r2, r4, #16
            send_radio_data(0xFAF000+exec_count_irq);	
            // Go to sleep without timer
            operation_sleep_notimer();
        }
   
    }else if(wakeup_data_header == 2){
 a42:	2d02      	cmp	r5, #2
 a44:	d12b      	bne.n	a9e <main+0x242>

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}
 a46:	21ff      	movs	r1, #255	; 0xff
 a48:	400b      	ands	r3, r1
 a4a:	400c      	ands	r4, r1
    }else if(wakeup_data_header == 2){
		// Slow down PMU sleep osc and run CDC code with desired wakeup period
        // wakeup_data[15:0] is the user-specified period
        // wakeup_data[19:16] is the initial user-specified period
        // wakeup_data[20] enables radio tx for each measurement
    	WAKEUP_PERIOD_CONT = wakeup_data_field_0 + (wakeup_data_field_1<<8);
 a4c:	021b      	lsls	r3, r3, #8
 a4e:	18e4      	adds	r4, r4, r3
 a50:	4872      	ldr	r0, [pc, #456]	; (c1c <main+0x3c0>)
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_2 & 0xF;
 a52:	4b73      	ldr	r3, [pc, #460]	; (c20 <main+0x3c4>)
 a54:	4017      	ands	r7, r2
    }else if(wakeup_data_header == 2){
		// Slow down PMU sleep osc and run CDC code with desired wakeup period
        // wakeup_data[15:0] is the user-specified period
        // wakeup_data[19:16] is the initial user-specified period
        // wakeup_data[20] enables radio tx for each measurement
    	WAKEUP_PERIOD_CONT = wakeup_data_field_0 + (wakeup_data_field_1<<8);
 a56:	6004      	str	r4, [r0, #0]
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_2 & 0xF;
 a58:	601f      	str	r7, [r3, #0]
        radio_tx_option = wakeup_data_field_2 & 0x10;
 a5a:	2310      	movs	r3, #16
 a5c:	401a      	ands	r2, r3
 a5e:	4b73      	ldr	r3, [pc, #460]	; (c2c <main+0x3d0>)

        set_pmu_sleep_clk_low();
        delay(MBUS_DELAY);
 a60:	2064      	movs	r0, #100	; 0x64
        // wakeup_data[15:0] is the user-specified period
        // wakeup_data[19:16] is the initial user-specified period
        // wakeup_data[20] enables radio tx for each measurement
    	WAKEUP_PERIOD_CONT = wakeup_data_field_0 + (wakeup_data_field_1<<8);
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_2 & 0xF;
        radio_tx_option = wakeup_data_field_2 & 0x10;
 a62:	601a      	str	r2, [r3, #0]
}


inline static void set_pmu_sleep_clk_low(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F77183B; // 0x8F77003B: use GOC x0.6-2
 a64:	4a75      	ldr	r2, [pc, #468]	; (c3c <main+0x3e0>)
 a66:	4b5d      	ldr	r3, [pc, #372]	; (bdc <main+0x380>)
 a68:	601a      	str	r2, [r3, #0]
    	WAKEUP_PERIOD_CONT = wakeup_data_field_0 + (wakeup_data_field_1<<8);
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_2 & 0xF;
        radio_tx_option = wakeup_data_field_2 & 0x10;

        set_pmu_sleep_clk_low();
        delay(MBUS_DELAY);
 a6a:	f7ff fb16 	bl	9a <delay>

		if (!cdc_running){
 a6e:	4c71      	ldr	r4, [pc, #452]	; (c34 <main+0x3d8>)
 a70:	7822      	ldrb	r2, [r4, #0]
 a72:	2a00      	cmp	r2, #0
 a74:	d106      	bne.n	a84 <main+0x228>
			// Go to sleep for initial settling of pressure // FIXME
			set_wakeup_timer (5, 0x1, 0x0); // 150: around 5 min
 a76:	2005      	movs	r0, #5
 a78:	2101      	movs	r1, #1
 a7a:	f7ff fb33 	bl	e4 <set_wakeup_timer>
			cdc_running = 1;
 a7e:	2301      	movs	r3, #1
 a80:	7023      	strb	r3, [r4, #0]
 a82:	e051      	b.n	b28 <main+0x2cc>
			operation_sleep_noirqreset();
		}
        exec_count = 0;
 a84:	4a59      	ldr	r2, [pc, #356]	; (bec <main+0x390>)
 a86:	2300      	movs	r3, #0
 a88:	6013      	str	r3, [r2, #0]
		meas_count = 0;
 a8a:	4a6d      	ldr	r2, [pc, #436]	; (c40 <main+0x3e4>)
 a8c:	6013      	str	r3, [r2, #0]
        cdc_storage_count = 0;
 a8e:	4a65      	ldr	r2, [pc, #404]	; (c24 <main+0x3c8>)
 a90:	6013      	str	r3, [r2, #0]
		radio_tx_count = 0;
 a92:	4a65      	ldr	r2, [pc, #404]	; (c28 <main+0x3cc>)
 a94:	6013      	str	r3, [r2, #0]

		// Reset IRQ10VEC
		*((volatile uint32_t *) IRQ10VEC) = 0;

        // Run CDC Program
		cdc_reset_timeout_count = 0;
 a96:	4a6b      	ldr	r2, [pc, #428]	; (c44 <main+0x3e8>)
		meas_count = 0;
        cdc_storage_count = 0;
		radio_tx_count = 0;

		// Reset IRQ10VEC
		*((volatile uint32_t *) IRQ10VEC) = 0;
 a98:	6033      	str	r3, [r6, #0]

        // Run CDC Program
		cdc_reset_timeout_count = 0;
 a9a:	6013      	str	r3, [r2, #0]
 a9c:	e08e      	b.n	bbc <main+0x360>
        operation_cdc_run();

    }else if(wakeup_data_header == 3){
 a9e:	2d03      	cmp	r5, #3
 aa0:	d11b      	bne.n	ada <main+0x27e>
		// Stop CDC program and transmit the execution count n times
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        // wakeup_data[16] indicates whether or not to speed up PMU sleep clock
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
 aa2:	495f      	ldr	r1, [pc, #380]	; (c20 <main+0x3c4>)
 aa4:	b2db      	uxtb	r3, r3
 aa6:	600b      	str	r3, [r1, #0]
		cdc_running = 0;
 aa8:	4b62      	ldr	r3, [pc, #392]	; (c34 <main+0x3d8>)
 aaa:	2100      	movs	r1, #0
 aac:	7019      	strb	r1, [r3, #0]

		if (wakeup_data_field_2 & 0x1){
 aae:	07d3      	lsls	r3, r2, #31
 ab0:	d502      	bpl.n	ab8 <main+0x25c>
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F77183B; // 0x8F77003B: use GOC x0.6-2
}
inline static void set_pmu_sleep_clk_default(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F77184B; // 0x8F77004B: use GOC x10-45
 ab2:	4a49      	ldr	r2, [pc, #292]	; (bd8 <main+0x37c>)
 ab4:	4b49      	ldr	r3, [pc, #292]	; (bdc <main+0x380>)
 ab6:	601a      	str	r2, [r3, #0]
		if (wakeup_data_field_2 & 0x1){
			// Speed up PMU sleep osc
			set_pmu_sleep_clk_default();
		}

        if (exec_count_irq < wakeup_data_field_0){
 ab8:	4b4d      	ldr	r3, [pc, #308]	; (bf0 <main+0x394>)
 aba:	b2e4      	uxtb	r4, r4
 abc:	681a      	ldr	r2, [r3, #0]
 abe:	42a2      	cmp	r2, r4
 ac0:	d209      	bcs.n	ad6 <main+0x27a>
            exec_count_irq++;
 ac2:	681a      	ldr	r2, [r3, #0]
 ac4:	3201      	adds	r2, #1
 ac6:	601a      	str	r2, [r3, #0]
            // radio
            send_radio_data(0xFAF000+exec_count);	
 ac8:	4b48      	ldr	r3, [pc, #288]	; (bec <main+0x390>)
 aca:	6818      	ldr	r0, [r3, #0]
 acc:	4b5a      	ldr	r3, [pc, #360]	; (c38 <main+0x3dc>)
 ace:	18c0      	adds	r0, r0, r3
 ad0:	f7ff fc9e 	bl	410 <send_radio_data>
 ad4:	e05d      	b.n	b92 <main+0x336>
            set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
            // go to sleep and wake up with same condition
            operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
 ad6:	2200      	movs	r2, #0
 ad8:	e06e      	b.n	bb8 <main+0x35c>
            // Go to sleep without timer
            operation_sleep_notimer();
        }

    }else if(wakeup_data_header == 4){
 ada:	2d04      	cmp	r5, #4
 adc:	d168      	bne.n	bb0 <main+0x354>
        // Transmit the stored cdc data
        // wakeup_data[7:0] is the # of data to transmit; if zero, all stored data is sent
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
 ade:	22ff      	movs	r2, #255	; 0xff
 ae0:	494f      	ldr	r1, [pc, #316]	; (c20 <main+0x3c4>)
 ae2:	4013      	ands	r3, r2
 ae4:	600b      	str	r3, [r1, #0]

		radio_tx_numdata = wakeup_data_field_0;
 ae6:	4b58      	ldr	r3, [pc, #352]	; (c48 <main+0x3ec>)
 ae8:	4014      	ands	r4, r2
		// Make sure the requested numdata makes sense
		if (radio_tx_numdata >= cdc_storage_count){
 aea:	4a4e      	ldr	r2, [pc, #312]	; (c24 <main+0x3c8>)
        // Transmit the stored cdc data
        // wakeup_data[7:0] is the # of data to transmit; if zero, all stored data is sent
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

		radio_tx_numdata = wakeup_data_field_0;
 aec:	601c      	str	r4, [r3, #0]
		// Make sure the requested numdata makes sense
		if (radio_tx_numdata >= cdc_storage_count){
 aee:	6819      	ldr	r1, [r3, #0]
 af0:	6812      	ldr	r2, [r2, #0]
 af2:	4291      	cmp	r1, r2
 af4:	d301      	bcc.n	afa <main+0x29e>
			radio_tx_numdata = 0;
 af6:	2200      	movs	r2, #0
 af8:	601a      	str	r2, [r3, #0]
		}
    
        if (exec_count_irq < 3){
 afa:	4c3d      	ldr	r4, [pc, #244]	; (bf0 <main+0x394>)
 afc:	6823      	ldr	r3, [r4, #0]
 afe:	2b02      	cmp	r3, #2
 b00:	d814      	bhi.n	b2c <main+0x2d0>
			exec_count_irq++;
 b02:	6823      	ldr	r3, [r4, #0]
 b04:	3301      	adds	r3, #1
 b06:	6023      	str	r3, [r4, #0]
			// radio
			send_radio_data(0xFAF000+exec_count_irq);
 b08:	6820      	ldr	r0, [r4, #0]
 b0a:	4b4b      	ldr	r3, [pc, #300]	; (c38 <main+0x3dc>)
 b0c:	18c0      	adds	r0, r0, r3
 b0e:	f7ff fc7f 	bl	410 <send_radio_data>
			if (exec_count_irq == 2){
 b12:	6822      	ldr	r2, [r4, #0]
 b14:	4b42      	ldr	r3, [pc, #264]	; (c20 <main+0x3c4>)
				// set timer
				set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT*2, 0x1, 0x0);
 b16:	6818      	ldr	r0, [r3, #0]
    
        if (exec_count_irq < 3){
			exec_count_irq++;
			// radio
			send_radio_data(0xFAF000+exec_count_irq);
			if (exec_count_irq == 2){
 b18:	2a02      	cmp	r2, #2
 b1a:	d100      	bne.n	b1e <main+0x2c2>
				// set timer
				set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT*2, 0x1, 0x0);
 b1c:	0040      	lsls	r0, r0, #1
			}else{
				// set timer
				set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
 b1e:	b280      	uxth	r0, r0
 b20:	2101      	movs	r1, #1
 b22:	2200      	movs	r2, #0
 b24:	f7ff fade 	bl	e4 <set_wakeup_timer>
			}
			// go to sleep and wake up with same condition
			operation_sleep_noirqreset();
 b28:	f7ff fcf8 	bl	51c <operation_sleep_noirqreset>
}

static void operation_tx_stored(void){

	//Fire off stored data to radio
	uint32_t data1 = 0xF00000 | cdc_storage[radio_tx_count];
 b2c:	4c3e      	ldr	r4, [pc, #248]	; (c28 <main+0x3cc>)
 b2e:	4b47      	ldr	r3, [pc, #284]	; (c4c <main+0x3f0>)
 b30:	6822      	ldr	r2, [r4, #0]
 b32:	25f0      	movs	r5, #240	; 0xf0
 b34:	0092      	lsls	r2, r2, #2
 b36:	58d7      	ldr	r7, [r2, r3]
	uint32_t data2 = 0xF00000 | cdc_storage_cref[radio_tx_count];
 b38:	6822      	ldr	r2, [r4, #0]
 b3a:	4b45      	ldr	r3, [pc, #276]	; (c50 <main+0x3f4>)
 b3c:	0092      	lsls	r2, r2, #2
}

static void operation_tx_stored(void){

	//Fire off stored data to radio
	uint32_t data1 = 0xF00000 | cdc_storage[radio_tx_count];
 b3e:	042d      	lsls	r5, r5, #16
	uint32_t data2 = 0xF00000 | cdc_storage_cref[radio_tx_count];
	delay(MBUS_DELAY);
 b40:	2064      	movs	r0, #100	; 0x64

static void operation_tx_stored(void){

	//Fire off stored data to radio
	uint32_t data1 = 0xF00000 | cdc_storage[radio_tx_count];
	uint32_t data2 = 0xF00000 | cdc_storage_cref[radio_tx_count];
 b42:	58d6      	ldr	r6, [r2, r3]
	delay(MBUS_DELAY);
 b44:	f7ff faa9 	bl	9a <delay>
}

static void operation_tx_stored(void){

	//Fire off stored data to radio
	uint32_t data1 = 0xF00000 | cdc_storage[radio_tx_count];
 b48:	1c28      	adds	r0, r5, #0
 b4a:	4338      	orrs	r0, r7
		write_mbus_message(0x76, cdc_storage_cref[radio_tx_count]);
		delay(MBUS_DELAY*10);
		write_mbus_message(0x70, radio_tx_count);
		delay(MBUS_DELAY*10);
	#endif
	send_radio_data(data1);
 b4c:	f7ff fc60 	bl	410 <send_radio_data>
	delay(RAD_PACKET_DELAY*3); //Set delays between sending subsequent packet
 b50:	4840      	ldr	r0, [pc, #256]	; (c54 <main+0x3f8>)
 b52:	f7ff faa2 	bl	9a <delay>

static void operation_tx_stored(void){

	//Fire off stored data to radio
	uint32_t data1 = 0xF00000 | cdc_storage[radio_tx_count];
	uint32_t data2 = 0xF00000 | cdc_storage_cref[radio_tx_count];
 b56:	1c28      	adds	r0, r5, #0
 b58:	4330      	orrs	r0, r6
		write_mbus_message(0x70, radio_tx_count);
		delay(MBUS_DELAY*10);
	#endif
	send_radio_data(data1);
	delay(RAD_PACKET_DELAY*3); //Set delays between sending subsequent packet
	send_radio_data(data2);
 b5a:	f7ff fc59 	bl	410 <send_radio_data>

	if (((!radio_tx_numdata)&&(radio_tx_count > 0)) | ((radio_tx_numdata)&&((radio_tx_numdata+radio_tx_count) > cdc_storage_count))){
 b5e:	4a3a      	ldr	r2, [pc, #232]	; (c48 <main+0x3ec>)
 b60:	2500      	movs	r5, #0
 b62:	6813      	ldr	r3, [r2, #0]
 b64:	42ab      	cmp	r3, r5
 b66:	d102      	bne.n	b6e <main+0x312>
 b68:	6825      	ldr	r5, [r4, #0]
 b6a:	1e6b      	subs	r3, r5, #1
 b6c:	419d      	sbcs	r5, r3
 b6e:	6811      	ldr	r1, [r2, #0]
 b70:	2300      	movs	r3, #0
 b72:	4299      	cmp	r1, r3
 b74:	d007      	beq.n	b86 <main+0x32a>
 b76:	6813      	ldr	r3, [r2, #0]
 b78:	4a2a      	ldr	r2, [pc, #168]	; (c24 <main+0x3c8>)
 b7a:	6821      	ldr	r1, [r4, #0]
 b7c:	6812      	ldr	r2, [r2, #0]
 b7e:	18cb      	adds	r3, r1, r3
 b80:	429a      	cmp	r2, r3
 b82:	419b      	sbcs	r3, r3
 b84:	425b      	negs	r3, r3
 b86:	431d      	orrs	r5, r3
 b88:	4c27      	ldr	r4, [pc, #156]	; (c28 <main+0x3cc>)
 b8a:	d005      	beq.n	b98 <main+0x33c>
		radio_tx_count--;
 b8c:	6823      	ldr	r3, [r4, #0]
 b8e:	3b01      	subs	r3, #1
 b90:	6023      	str	r3, [r4, #0]
		// set timer
		set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
 b92:	4b23      	ldr	r3, [pc, #140]	; (c20 <main+0x3c4>)
 b94:	6818      	ldr	r0, [r3, #0]
 b96:	e7c2      	b.n	b1e <main+0x2c2>
		// go to sleep and wake up with same condition
		operation_sleep_noirqreset();

	}else{
		delay(RAD_PACKET_DELAY*3); //Set delays between sending subsequent packet
 b98:	482e      	ldr	r0, [pc, #184]	; (c54 <main+0x3f8>)
 b9a:	f7ff fa7e 	bl	9a <delay>
		send_radio_data(0xFAF000);
 b9e:	4826      	ldr	r0, [pc, #152]	; (c38 <main+0x3dc>)
 ba0:	f7ff fc36 	bl	410 <send_radio_data>

		// This is also the end of this IRQ routine
		exec_count_irq = 0;
 ba4:	4b12      	ldr	r3, [pc, #72]	; (bf0 <main+0x394>)
 ba6:	601d      	str	r5, [r3, #0]
		// Go to sleep without timer
		radio_tx_count = cdc_storage_count; // allows data to be sent more than once
 ba8:	4b1e      	ldr	r3, [pc, #120]	; (c24 <main+0x3c8>)
 baa:	681b      	ldr	r3, [r3, #0]
 bac:	6023      	str	r3, [r4, #0]
 bae:	e745      	b.n	a3c <main+0x1e0>
            // Go to sleep without timer
            operation_sleep_notimer();
        }
*/

    }else if(wakeup_data_header == 0x11){
 bb0:	2d11      	cmp	r5, #17
 bb2:	d103      	bne.n	bbc <main+0x360>
}


inline static void set_pmu_sleep_clk_low(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F77183B; // 0x8F77003B: use GOC x0.6-2
 bb4:	4a21      	ldr	r2, [pc, #132]	; (c3c <main+0x3e0>)
 bb6:	4b09      	ldr	r3, [pc, #36]	; (bdc <main+0x380>)
 bb8:	601a      	str	r2, [r3, #0]
 bba:	e73f      	b.n	a3c <main+0x1e0>
*/
    }

    // Proceed to continuous mode
    while(1){
        operation_cdc_run();
 bbc:	f7ff fcce 	bl	55c <operation_cdc_run>
 bc0:	e7fc      	b.n	bbc <main+0x360>
 bc2:	46c0      	nop			; (mov r8, r8)
 bc4:	e000e280 	.word	0xe000e280
 bc8:	e000e100 	.word	0xe000e100
 bcc:	000f4240 	.word	0x000f4240
 bd0:	00000c7c 	.word	0x00000c7c
 bd4:	deadbeef 	.word	0xdeadbeef
 bd8:	8f77184b 	.word	0x8f77184b
 bdc:	a200000c 	.word	0xa200000c
 be0:	00202d08 	.word	0x00202d08
 be4:	a2000008 	.word	0xa2000008
 be8:	00000c80 	.word	0x00000c80
 bec:	00000c70 	.word	0x00000c70
 bf0:	00000cac 	.word	0x00000cac
 bf4:	00000ca4 	.word	0x00000ca4
 bf8:	00000c5c 	.word	0x00000c5c
 bfc:	fffffeff 	.word	0xfffffeff
 c00:	00000c60 	.word	0x00000c60
 c04:	00000c64 	.word	0x00000c64
 c08:	fff003ff 	.word	0xfff003ff
 c0c:	00000c58 	.word	0x00000c58
 c10:	fffffbff 	.word	0xfffffbff
 c14:	ffe07fff 	.word	0xffe07fff
 c18:	fffffc0f 	.word	0xfffffc0f
 c1c:	00000cb0 	.word	0x00000cb0
 c20:	00000c98 	.word	0x00000c98
 c24:	00000c94 	.word	0x00000c94
 c28:	00000c68 	.word	0x00000c68
 c2c:	00000c90 	.word	0x00000c90
 c30:	00000c6c 	.word	0x00000c6c
 c34:	00000cb4 	.word	0x00000cb4
 c38:	00faf000 	.word	0x00faf000
 c3c:	8f77183b 	.word	0x8f77183b
 c40:	00000ca0 	.word	0x00000ca0
 c44:	00000c74 	.word	0x00000c74
 c48:	00000c9c 	.word	0x00000c9c
 c4c:	00000d58 	.word	0x00000d58
 c50:	00000cb8 	.word	0x00000cb8
 c54:	00001770 	.word	0x00001770
