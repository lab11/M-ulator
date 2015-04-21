//*******************************************************************
//Author: ZhiYoong Foo
//Description: MBUS lib file
//*******************************************************************

#include "mbus.h"

//Writes Arbitrary MBUS Messages
int write_mbus_message(uint32_t addr, uint32_t data) {
  uint32_t _mbus_addr = 0xa0000000;
  *((volatile uint32_t *) _mbus_addr) = addr;
  _mbus_addr |= 0x4;
  *((volatile uint32_t *) _mbus_addr) = data;
  return 0;
}

int write_mbus_msg(uint32_t short_prefix, uint32_t fuid, uint32_t data) {
	uint32_t _mbus_addr = 0x00000000;
	uint32_t _mbus_data = data;
	_mbus_addr = (short_prefix << 4) | fuid;
	return write_mbus_message(_mbus_addr, _mbus_data);
}

//Enumerates
int enumerate(uint32_t addr) {
  uint32_t _mbus_data = 0x20000000;
  _mbus_data |= (addr << 24);
  write_mbus_message(0x0, _mbus_data);
  return 0;
}

//Set system to sleep
int sleep(){
  write_mbus_message(0x1,0x0);
  return 0;
}

//Writes Register <reg> on <enum_addr> layer with <data>
int write_mbus_register(uint32_t enum_addr, uint8_t reg, uint32_t data){
  uint32_t _mbus_addr = 0;
  uint32_t _mbus_data = 0;
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24) | (data&0xFFFFFF); //Only use bottom 24 bits!
  write_mbus_message(_mbus_addr,_mbus_data);
  return 0;
}

//Reads Register <reg> on <enum_addr> layer and returns data to <return_addr>
int read_mbus_register(uint32_t enum_addr, uint8_t reg, uint8_t return_addr){
  uint32_t _mbus_addr = 1;
  uint32_t _mbus_data = (return_addr << 8);
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24);
  write_mbus_message(_mbus_addr,_mbus_data);
  return 0;
}

//Reads Register <reg> on <enum_addr> layer and returns data to <return_addr>
//ONLY FOR RADv4!
int read_mbus_register_RADv4(uint32_t enum_addr, uint8_t reg, uint8_t return_addr){
  uint32_t _mbus_addr = 1;
  uint32_t _mbus_data = (return_addr << 16);
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24);
  write_mbus_message(_mbus_addr,_mbus_data);
  return 0;
}
