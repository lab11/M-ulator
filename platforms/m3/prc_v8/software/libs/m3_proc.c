#include "m3_proc.h"

////////////////////////
// MBUS RELATED COMMANDS
////////////////////////

static int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
  _mbus_addr |= 0x4;
  *((volatile uint32_t *) _mbus_addr) = data;
  return 0;
}

int enumerate(uint32_t addr) {
  uint32_t _mbus_data = 0x20000000;
  _mbus_data |= (addr << 24);
  write_mbus_message(0x0, _mbus_data);
  return 0;
}

int sleep(){
  write_mbus_message(0x1,0x0);
  return 0;
}

int write_mbus_register(uint32_t enum_addr, uint8_t reg, uint32_t data){
  uint32_t _mbus_addr = 0;
  uint32_t _mbus_data = 0;
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24) | data;
  write_mbus_message(_mbus_addr,_mbus_data);
  return 0;
}

int read_mbus_register(uint32_t enum_addr, uint8_t reg, uint8_t return_addr){
  uint32_t _mbus_addr = 1;
  uint32_t _mbus_data = (return_addr << 8);
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24);
  write_mbus_message(_mbus_addr,_mbus_data);
  return 0;
}

int read_mbus_register_RADv4(uint32_t enum_addr, uint8_t reg, uint8_t return_addr){
  uint32_t _mbus_addr = 1;
  uint32_t _mbus_data = (return_addr << 16);
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24);
  write_mbus_message(_mbus_addr,_mbus_data);
  return 0;
}
//////////////////////////
//PROCv6 SPECIFIC COMMANDS
//////////////////////////

// [31:4] = 28'hA200000
//  [3:2] = Target register
//  [1:0] = 2'b00
int write_config_reg(uint8_t reg, uint32_t data) {
	uint32_t _addr = 0xa2000000;
	_addr |= (reg << 2);

	*((volatile uint32_t *) _addr) = data;
	return 0;
}
//////////////////////////
//PROCv8 SPECIFIC COMMANDS
//////////////////////////
int config_timer( uint8_t timer_id, uint8_t go, uint8_t roi, uint32_t init_val, uint32_t sat_val) {
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
