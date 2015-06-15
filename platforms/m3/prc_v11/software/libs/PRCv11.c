//*******************************************************************
//Author: Yoonmyung Lee
//        ZhiYoong Foo
//Description: PRCv11 lib file
//*******************************************************************

#include "PRCv11.h"

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
  unsigned i;
  for (i=0; i < ticks; i++)
    asm("nop;");
}

void WFI(){
  asm("wfi;");
}

void config_timer32(uint32_t cmp, uint8_t roi, uint32_t cnt){
	uint32_t _addr = 0xA0001100;
	*((volatile uint32_t *) (_addr | 0x0) ) = 0x0;
	*((volatile uint32_t *) (_addr | 0x4) ) = cmp;
	*((volatile uint32_t *) (_addr | 0x8) ) = roi;
	*((volatile uint32_t *) (_addr | 0xC) ) = cnt;
	*((volatile uint32_t *) (_addr | 0x0) ) = 0x1;
}

void set_wakeup_timer( uint16_t timestamp, uint8_t on, uint8_t reset ){
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

void pon_reset( void ){
	*((volatile uint32_t *) 0xA3000000) = 0x2;
}

void sleep_req_by_sw( void ){
	*((volatile uint32_t *) 0xA3000000) = 0x4;
}
