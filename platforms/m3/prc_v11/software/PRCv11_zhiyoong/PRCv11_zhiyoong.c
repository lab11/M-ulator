//*******************************************************************
//Author: ZhiYoong Foo
//Description: PRCv11 Functionality Tests
//*******************************************************************
#include "PRCv11.h"
#include "mbus.h"

//***************************************************
//Interrupt Handlers
//Must clear pending bit!
//***************************************************
void handler_ext_int_0(void) __attribute__ ((interrupt ("IRQ")));

//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************

//TIMER32
void handler_ext_int_0(void){
    *((volatile uint32_t *) 0xE000E280) = 0x1;
	write_config_reg(0x0,0x1000);
	uint32_t _cmp = *((volatile uint32_t *) 0xA0001104 );
	_cmp = _cmp & 0xFFFF;
	_cmp = _cmp * 71;
	write_config_reg(0x0, _cmp);
}
//TIMER16
void handler_ext_int_1(void){
    *((volatile uint32_t *) 0xE000E280) = 0x2;
	write_config_reg(0x0,0x1001);
}


int main() {
  
  //Clear All Pending Interrupts
  //  *((volatile uint32_t *) 0xE000E280) = 0x3FF;
  //Enable Interrupts
  //  *((volatile uint32_t *) 0xE000E100) = 0x3FF;
  
  //Chip ID
  write_config_reg(0x8,0xDEAD);
  

  //MBUS
  mbus_enumerate(0x4);
  
  reset_checking_local();

  //Never Quit
  while(1){
	asm("nop;");
  }

}


void reset_checking_local(void){
  uint32_t data;
  data = read_config_reg(0x0);
}
