//*******************************************************************
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
  unsigned i;
  for (i=0; i < ticks; i++)
    asm("nop;");
}

void WFI(){
  asm("wfi;");
}

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
void pon_reset( void ){
	*((volatile uint32_t *) 0xA3000000) = 0x2;
}

// FIXME: Not Verified (Yejoong Kim)
void sleep_req_by_sw( void ){
	*((volatile uint32_t *) 0xA3000000) = 0x4;
}
