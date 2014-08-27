//*******************************************************************
//Author: ZhiYoong Foo, Yoonmyung Lee
//Description: PRCv9E lib file
//*******************************************************************

#include "PRCv9E.h"

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
  unsigned i;
  for (i=0; i < ticks; i++)
    asm("nop;");
}

void WFI(){
  asm("wfi;");
}

// Watchdog timer or 32bit timer
void config_timer( uint8_t timer_id, uint8_t go, uint8_t roi, uint32_t init_val, uint32_t sat_val) {
	uint32_t _addr = 0xA5000000;
	_addr |= (timer_id<<8);
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

// 16bit timer related functions
void timer16b_go( uint8_t go ){
	uint32_t _addr = 0xA5000200;
	*((volatile uint32_t *) (_addr | 0x0) ) = go ;		
}

void timer16b_cfg_compare( uint8_t cmp_id, uint32_t cmp_value ){
	uint32_t _addr = 0xA5000200;
	_addr |= (cmp_id+1)<<2;
	*((volatile uint32_t *) (_addr) ) = cmp_value;		// load cmp value
}

void timer16b_cfg_capture( uint8_t cap_id, uint32_t cap_value ){
	uint32_t _addr = 0xA5000200;
	_addr |= (cap_id+3)<<2;
	*((volatile uint32_t *) (_addr) ) = cap_value;		// load cap value
}

uint32_t timer16b_read_compare( uint8_t cmp_id ){
	uint32_t _addr = 0xA5000200;
	volatile uint32_t _data;
	_addr |= (cmp_id+1)<<2;
	_data = *((volatile uint32_t *) _addr);
	return ( _data );
}

uint32_t timer16b_read_capture( uint8_t cap_id ){
	uint32_t _addr = 0xA5000200;
	volatile uint32_t _data;
	_addr |= (cap_id+3)<<2;
	_data = *((volatile uint32_t *) _addr);
	return ( _data );
}

uint32_t timer16b_read_count(){
	uint32_t _addr = 0xA500021C;
	volatile uint32_t _data;
	_data = *((volatile uint32_t *) _addr);
	return ( _data );
}

uint32_t timer16b_read_stat(){
	uint32_t _addr = 0xA5000220;
	volatile uint32_t _data;
	_data = *((volatile uint32_t *) _addr);
	return ( _data );
}

// GPIO
// 0xA600_0000
void gpio_set_dir(uint32_t dir){
	uint32_t _addr = 0xA6001000;
	*((volatile uint32_t *) (_addr) ) = dir;
}
uint32_t gpio_read_dir(){
	uint32_t _addr = 0xA6001000;
	volatile uint32_t _data;
	_data = *((volatile uint32_t *) _addr);
	return ( _data );
}
void gpio_set_intmask(uint32_t intmask){
	uint32_t _addr = 0xA6001040;
	*((volatile uint32_t *) (_addr) ) = intmask;
}
uint32_t gpio_read_intmask(){
	uint32_t _addr = 0xA6001040;
	volatile uint32_t _data;
	_data = *((volatile uint32_t *) _addr);
	return ( _data );
}
void gpio_writedata(uint32_t writedata){
	uint32_t _addr = 0xA6000000;
	*((volatile uint32_t *) (_addr) ) = writedata;
}
uint32_t gpio_readdata(){
	uint32_t _addr = 0xA6000000;
	volatile uint32_t _data;
	_data = *((volatile uint32_t *) _addr);
	return ( _data );
}


// GPS_FSM
// 0xA700_0000

/*
void config_timer_16b( uint8_t timer_id, uint8_t go, uint8_t roi, uint32_t init_val, uint32_t sat_val, uint32_t cmp_val) {
	uint32_t _addr = 0xA5000000;
	_addr |= (timer_id<<8);
	// GO   = 0x00
	// CMP0 = 0x04
	// CMP1 = 0x08
	// CAP0 = 0x0C
	// CAP1 = 0x10
	// CAP2 = 0x14
	// CAP3 = 0x18
	// CNT  = 0x1C
	// STAT = 0x20
	*((volatile uint32_t *) (_addr | 0x0) ) = 0x0;		// stop timer first
	*((volatile uint32_t *) (_addr | 0x4) ) = sat_val;	// set up values	
	*((volatile uint32_t *) (_addr | 0x8) ) = roi;
	*((volatile uint32_t *) (_addr | 0xC) ) = init_val;
	*((volatile uint32_t *) (_addr | 0x0) ) = go;		// run
}*/

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
	if( ovrd )
		regval |= 0x40000000;					// Set bit 30
	else
		regval &= 0xBFFFFFFF;					// Reset bit 30
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

void pon_reset( void ){
	*((volatile uint32_t *) 0xA3000000) = 0x2;
}

void sleep_req_by_sw( void ){
	*((volatile uint32_t *) 0xA3000000) = 0x4;
}
