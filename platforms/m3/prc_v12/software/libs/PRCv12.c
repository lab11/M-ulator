//*******************************************************************
<<<<<<< HEAD
//Author: Yejoong Kim
//Description: PRCv12 lib file
//*******************************************************************

#include "PRCv12.h"

//*******************************************************************
// INTERRUPT FUNCTIONS
//*******************************************************************
void init_interrupt (void) {
  //Clear All Pending Interrupts
  *((volatile uint32_t *) 0xE000E280) = (uint32_t) NUM_EXT_INT;
  //Enable Interrupts
  *((volatile uint32_t *) 0xE000E100) = (uint32_t) NUM_EXT_INT;
}

//TIMER32
void handler_ext_int_0(void) { 
    *((volatile uint32_t *) 0xE000E280) = (0x1 << 0);
	write_config_reg(0x0,0x1000);
	uint32_t _cmp = *((volatile uint32_t *) 0xA0001104 );
	_cmp = _cmp & 0xFFFF;
	//_cmp = _cmp * 71;
	write_config_reg(0x1, _cmp);
}

//TIMER16
void handler_ext_int_1(void) { 
    *((volatile uint32_t *) 0xE000E280) = (0x1 << 1);
	write_config_reg(0x0,0x1001);
}

//MSG0-7 (This can be disabled by RF_WR_IRQ_MASK) 
void handler_ext_int_2(void) { *((volatile uint32_t *) 0xE000E280) = (0x1 << 2); }
void handler_ext_int_3(void) { *((volatile uint32_t *) 0xE000E280) = (0x1 << 3); }
void handler_ext_int_4(void) { *((volatile uint32_t *) 0xE000E280) = (0x1 << 4); }
void handler_ext_int_5(void) { *((volatile uint32_t *) 0xE000E280) = (0x1 << 5); }
void handler_ext_int_6(void) { *((volatile uint32_t *) 0xE000E280) = (0x1 << 6); }
void handler_ext_int_7(void) { *((volatile uint32_t *) 0xE000E280) = (0x1 << 7); }
void handler_ext_int_8(void) { *((volatile uint32_t *) 0xE000E280) = (0x1 << 8); }
void handler_ext_int_9(void) { *((volatile uint32_t *) 0xE000E280) = (0x1 << 9); }

//SRAM_WRITE (This can be disabled by MEM_WR_IRQ_MASK)
void handler_ext_int_10(void) { *((volatile uint32_t *) 0xE000E280) = (0x1 << 10); }

//MBUS_RX (This can be disabled by MBUS_RX_IRQ_MASK)
void handler_ext_int_11(void) { *((volatile uint32_t *) 0xE000E280) = (0x1 << 11); }

//MBUS_TX (This can be disabled by MBUS_TX_IRQ_MASK)
void handler_ext_int_12(void) { *((volatile uint32_t *) 0xE000E280) = (0x1 << 12); }

//GOCEP
void handler_ext_int_13(void) { *((volatile uint32_t *) 0xE000E280) = (0x1 << 13); }


//*******************************************************************
// OHTER FUNCTIONS
//*******************************************************************
void write_config_reg(uint8_t reg, uint32_t data ){
  uint32_t _addr = 0xA0000000;
  _addr |= (reg << 2);
  
  *((volatile uint32_t *) _addr) = data;
  return 0;
}

int read_config_reg(uint8_t reg){
  uint32_t _addr = 0xA0000000;
  _addr |= (reg << 2);
  return *((volatile uint32_t *) _addr);
}

void delay(unsigned ticks){
=======
//Author: Yoonmyung Lee
//        ZhiYoong Foo
//Description: PRCv9 lib file
//*******************************************************************

#include "PRCv9.h"

// [31:4] = 28'hA200000
//  [3:2] = Target register
//  [1:0] = 2'b00
int write_config_reg(uint8_t reg, uint32_t data) {
	uint32_t _addr = 0xA2000000;
	_addr |= (reg << 2);

	*((volatile uint32_t *) _addr) = data;
	return 0;
}

void delay(unsigned ticks) {
>>>>>>> d33c1a0a94b8f9f6a48504f759d7a12dd8637ff5
  unsigned i;
  for (i=0; i < ticks; i++)
    asm("nop;");
}

void WFI(){
  asm("wfi;");
}

<<<<<<< HEAD
void config_timer16(uint32_t cmp0, uint32_t cmp1, uint8_t irq_en, uint32_t cnt, uint32_t status){
	uint32_t _addr = 0xA0001000;
	*((volatile uint32_t *) (_addr | 0x00) ) = 0x0;
	*((volatile uint32_t *) (_addr | 0x04) ) = cmp0;
	*((volatile uint32_t *) (_addr | 0x08) ) = cmp1;
	*((volatile uint32_t *) (_addr | 0x0C) ) = irq_en;
	*((volatile uint32_t *) (_addr | 0x20) ) = cnt;
	*((volatile uint32_t *) (_addr | 0x24) ) = status;
	*((volatile uint32_t *) (_addr | 0x00) ) = 0x1;
}

void config_timer32(uint32_t cmp, uint8_t roi, uint32_t cnt, uint32_t status){
	uint32_t _addr = 0xA0001100;
	*((volatile uint32_t *) (_addr | 0x00) ) = 0x0;
	*((volatile uint32_t *) (_addr | 0x04) ) = cmp;
	*((volatile uint32_t *) (_addr | 0x08) ) = roi;
	*((volatile uint32_t *) (_addr | 0x0C) ) = cnt;
	*((volatile uint32_t *) (_addr | 0x10) ) = status;
	*((volatile uint32_t *) (_addr | 0x00) ) = 0x1;
}

void config_timerwd(uint32_t cnt){
	uint32_t _addr = 0xA0001200;
	*((volatile uint32_t *) (_addr | 0x00) ) = 0x0;
	*((volatile uint32_t *) (_addr | 0x04) ) = cnt;
	*((volatile uint32_t *) (_addr | 0x00) ) = 0x1;
}

void set_wakeup_timer( uint16_t timestamp, uint8_t irq_en, uint8_t reset ){
	uint32_t regval = timestamp;
	if( irq_en ) regval |= 0x8000;
	else		 regval &= 0x7FFF;
	*((volatile uint32_t *) 0xA0000034) = regval;

	if( reset ) *((volatile uint32_t *) 0xA0001300) = 0x01;
}

void set_clkfreq( uint8_t fastmode, uint8_t div_core, uint8_t div_mbus, uint8_t ring ) {
	uint32_t regval = *((volatile uint32_t *) 0xA000002C );		// Read original reg value
	regval &= 0x0001FFFF; // Reset reg value
	regval |= (fastmode<<23) | (div_core<<21) | (div_mbus<<19) | (ring<<17) ;
	*((volatile uint32_t *) 0xA200002C) = regval;			// Write updated reg value
}

// FIXME: Not Verified (Yejoong Kim)
=======
void config_timer( uint8_t timer_id, uint8_t go, uint8_t roi, uint32_t init_val, uint32_t sat_val) {
	uint32_t _addr = 0xA5000000;
	_addr |= (timer_id<<4);
	// GO  = 0x0
	// SAT = 0x4
	// ROI = 0x8
	// CNT = 0xC
	*((volatile uint32_t *) (_addr | 0x0) ) = 0x0;		// stop timer first
	*((volatile uint32_t *) (_addr | 0x4) ) = sat_val;	// set up values	
	*((volatile uint32_t *) (_addr | 0x8) ) = roi;
	*((volatile uint32_t *) (_addr | 0xC) ) = init_val;
	*((volatile uint32_t *) (_addr | 0x0) ) = go;		// run
}

void set_wakeup_timer( uint16_t timestamp, uint8_t on, uint8_t reset ) {
	uint32_t regval = timestamp;
	if( on )	regval |= 0x8000;
	else		regval &= 0x7FFF;
	*((volatile uint32_t *) 0xA2000010) = regval;

	if( reset )
		*((volatile uint32_t *) 0xA2000014) = 0x01;
}

void set_clkfreq( uint8_t fastmode, uint8_t div_cm, uint8_t div_mbus, uint8_t ring ) {
	uint32_t regval = *((volatile uint32_t *) 0xA0001028 );		// Read original reg value
	regval &= 0xFF7FC0FF;
	regval |= (fastmode<<23) | (div_cm<<12) | (div_mbus<<10) | (ring<<8) ;
	*((volatile uint32_t *) 0xA2000008) = regval;			// Write updated reg value
}

void pmu_div5_ovrd( uint8_t ovrd ) {
	uint32_t regval = *((volatile uint32_t *) 0xA000102C );		// Read original reg value
	if( ovrd ){
		regval |= 0x40000000;					// Set bit 30 and reset bit 31
                regval &= 0x7FFFFFFF;
        }
	else
		regval &= 0x3FFFFFFF;					// Reset bit 30 and bit 31
	*((volatile uint32_t *) 0xA200000C) = regval;			// Write updated reg value
}

void pmu_div6_ovrd( uint8_t ovrd ) {
	uint32_t regval = *((volatile uint32_t *) 0xA000102C );		// Read original reg value
	if( ovrd ){
		regval |= 0x80000000;					// Set bit 31 and reset bit 30
                regval &= 0xBFFFFFFF;
        }
	else
		regval &= 0x3FFFFFFF;					// Reset bit 31 and bit 30
	*((volatile uint32_t *) 0xA200000C) = regval;			// Write updated reg value
}

void pmu_set_force_wake( uint8_t set ) {
	uint32_t regval = *((volatile uint32_t *) 0xA0001028 );		// Read original reg value
	if( set )
		regval |= 0x00000080;					// Set bit 7
	else
		regval &= 0xFFFFFF7F;					// Reset bit 7
	*((volatile uint32_t *) 0xA2000008) = regval;			// Write updated reg value
}

>>>>>>> d33c1a0a94b8f9f6a48504f759d7a12dd8637ff5
void pon_reset( void ){
	*((volatile uint32_t *) 0xA3000000) = 0x2;
}

<<<<<<< HEAD
// FIXME: Not Verified (Yejoong Kim)
void sleep_req_by_sw( void ){
	*((volatile uint32_t *) 0xA3000000) = 0x4;
}
=======
void sleep_req_by_sw( void ){
	*((volatile uint32_t *) 0xA3000000) = 0x4;
}

// ECC generation code -------------------------------
// will generate and return ECC for up to 26bit data
// MSBs of input data (data_in[31:26]) is ignored
// Empty data bits should be padded with 0
//    ex> if data is 16bit: data_in = 0x0000ABCD
// Valid ECC range are:
//    12-26bit data: 5bit ECC
//    5 -11bit data: 4bit ECC
//
// ECC Algorithm: (P = parity) (D = data)
// SEC requires 5 bits
// DED requires 1 additional for total parity check
//  P[0] :0 : 00000 = 1^2^3^4^5^6^7^8^9^10^11^12^13^14^15^16^17^18^19^20^21^22^23^24^25^26^27^28^29^30^31
// SEC Section:
//  P[1] :1 : 00001 = 3^5^7^9^11^13^15^17^19^21^23^25^27^29^31
//  P[2] :2 : 00010 = 3^6^7^10^11^14^15^18^19^22^23^26^27^30^31
//  D[0] :3 : 00011
//  P[3] :4 : 00100 = 5^6^7^12^13^14^15^20^21^22^23^28^29^30^31
//  D[1] :5 : 00101
//  D[2] :6 : 00110
//  D[3] :7 : 00111
//  P[4] :8 : 01000 = 9^10^11^12^13^14^15^24^25^26^27^28^29^30^31
//  D[4] :9 : 01001
//  D[5] :10: 01010
//  D[6] :11: 01011
//  D[7] :12: 01100
//  D[8] :13: 01101
//  D[9] :14: 01110
//  D[10]:15: 01111
//  P[5] :16: 10000 = 17^18^19^20^21^22^23^24^25^26^27^28^29^30^31
//  D[11]:17: 10001
//  D[12]:18: 10010
//  D[13]:19: 10011
//  D[14]:20: 10100
//  D[15]:21: 10101
//  D[16]:22: 10110
//  D[17]:23: 10111
//  D[18]:24: 11000
//  D[19]:25: 11001
//  D[20]:26: 11010
//  D[21]:27: 11011
//  D[22]:28: 11100
//  D[23]:29: 11101
//  D[24]:30: 11110
//  D[25]:31: 11111
//
// Output Format =
//  data_out[31:6] = data_in[25:0]
//  data_out[ 5:0] = 6bit ECC

uint32_t generate_ECC( uint32_t data_in ) {
	uint32_t P5 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
		((data_in>>12)&0x1) ^ 
		((data_in>>11)&0x1);
	uint32_t P4 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
		((data_in>>4 )&0x1);
	uint32_t P3 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
		((data_in>>22)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>1 )&0x1);
	uint32_t P2 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
		((data_in>>12)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>0 )&0x1);
	uint32_t P1 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
		((data_in>>11)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>8 )&0x1) ^
		((data_in>>6 )&0x1) ^ 
		((data_in>>4 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>1 )&0x1) ^ 
		((data_in>>0 )&0x1);
	uint32_t P0 = 
		((data_in>>25)&0x1) ^ 
		((data_in>>24)&0x1) ^ 
		((data_in>>23)&0x1) ^ 
		((data_in>>22)&0x1) ^ 
		((data_in>>21)&0x1) ^ 
		((data_in>>20)&0x1) ^ 
		((data_in>>19)&0x1) ^ 
		((data_in>>18)&0x1) ^ 
		((data_in>>17)&0x1) ^ 
		((data_in>>16)&0x1) ^ 
		((data_in>>15)&0x1) ^ 
		((data_in>>14)&0x1) ^ 
		((data_in>>13)&0x1) ^ 
		((data_in>>12)&0x1) ^ 
		((data_in>>11)&0x1) ^ 
		((data_in>>10)&0x1) ^ 
		((data_in>>9 )&0x1) ^ 
		((data_in>>8 )&0x1) ^ 
		((data_in>>7 )&0x1) ^ 
		((data_in>>6 )&0x1) ^ 
		((data_in>>5 )&0x1) ^ 
		((data_in>>4 )&0x1) ^ 
		((data_in>>3 )&0x1) ^ 
		((data_in>>2 )&0x1) ^ 
		((data_in>>1 )&0x1) ^ 
		((data_in>>0 )&0x1) ^ 
		P5 ^ P4 ^ P3 ^ P2 ^ P1 ;
	uint32_t data_out = (data_in<<6)|(P5<<5)|(P4<<4)|(P3<<3)|(P2<<2)|(P1<<1)|(P0<<0);
	return data_out;	
}

>>>>>>> d33c1a0a94b8f9f6a48504f759d7a12dd8637ff5
