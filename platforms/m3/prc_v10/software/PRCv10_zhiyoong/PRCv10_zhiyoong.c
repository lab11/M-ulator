//*******************************************************************
//Author: ZhiYoong Foo
//Description: PRCv10 Functionality Tests
//*******************************************************************
#include "PRCv10.h"

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
  volatile a,b,c;
  
  //Clear All Pending Interrupts
  //  *((volatile uint32_t *) 0xE000E280) = 0x3FF;
  //Enable Interrupts
  //  *((volatile uint32_t *) 0xE000E100) = 0x3FF;
  
  //Chip ID
  write_config_reg(0x0,0xDEAD);
  
  //TIMER32 test
/*   config_timer32(0x100, 0x1, 0x0); */
  
/*   a=10; */
/*   b=20; */
/*   c=a*b; */
/*   write_config_reg(0x0,c); */
  volatile uint32_t payload = 0x24000000;
  *((volatile uint32_t *) 0xA0001400) = 0x0;
  *((volatile uint32_t *) 0xA0001404) = &payload;
  //  *((volatile uint32_t *) 0xA0001408) = 0x;
  *((volatile uint32_t *) 0xA000140C) = 0x3 | (0x2 << 4);


  //Initialize
  while(1){
	asm("nop;");
  }

}

int write_config_reg_local(uint8_t reg, uint32_t data ){
	uint32_t _addr = 0xA0000000;
	_addr |= (reg << 2);

	*((volatile uint32_t *) _addr) = data;
	return 0;
}
