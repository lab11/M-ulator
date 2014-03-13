//*******************************************************************
//Author: Gyouho Kim
//Description:	testing if register write works
//*******************************************************************
#include "m3_proc.h"

#define RAD_ADDR 0x9
#define SNS_ADDR 0xA

//Interrupt Handlers ************************************************
void handler_ext_int_0(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_4(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_5(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_6(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_7(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_8(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_9(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_0(void){
  *((volatile uint32_t *) 0xE000E280) = 0x1;
}
void handler_ext_int_1(void){
  *((volatile uint32_t *) 0xE000E280) = 0x2;
}
void handler_ext_int_2(void){
  *((volatile uint32_t *) 0xE000E280) = 0x4;
}
void handler_ext_int_3(void){
  *((volatile uint32_t *) 0xE000E280) = 0x8;
}
void handler_ext_int_4(void){
  *((volatile uint32_t *) 0xE000E280) = 0x10;
}
void handler_ext_int_5(void){
  *((volatile uint32_t *) 0xE000E280) = 0x20;
}
void handler_ext_int_6(void){
  *((volatile uint32_t *) 0xE000E280) = 0x40;
}
void handler_ext_int_7(void){
  *((volatile uint32_t *) 0xE000E280) = 0x80;
  config_timer( 1, 1, 0, 0, 10000 );
  write_mbus_message( 0x01, 0x1111 );
}
void handler_ext_int_8(void){
  *((volatile uint32_t *) 0xE000E280) = 0x100;
  config_timer( 2, 1, 0, 5000, 15000 );
  write_mbus_message( 0x02, 0x2222 );
}
void handler_ext_int_9(void){
  *((volatile uint32_t *) 0xE000E280) = 0x200;
  config_timer( 3, 1, 0, 0, 2000 );
  write_mbus_message( 0x03, 0x3333 );
}


//Internal Functions ************************************************
void delay(unsigned ticks) {
	unsigned i;
	for (i=0; i < ticks; i++)
	asm("nop;");
}


//*******************************************************************
//Main **************************************************************
//*******************************************************************
int main() {

	//Clear All Pending Interrupts
	*((volatile uint32_t *) 0xE000E280) = 0x03FF;
	//Enable Interrupts
	*((volatile uint32_t *) 0xE000E100) = 0x03FF;

	//Enumeration
//	enumerate(RAD_ADDR);
//	asm ("wfi;");
//	enumerate(SNS_ADDR);
//	asm ("wfi;");

	uint32_t regval = *((volatile uint32_t *) 0xA0001028 );		// Read original reg value

	write_mbus_message( 0x03, regval );
	write_mbus_message( 0x03, regval );
	write_mbus_message( 0x03, regval );
	//regval |= 0x00000030;						// Set bits
	regval = 0xFFFFFFF;
	*((volatile uint32_t *) 0xA2000008) = regval;			// Write updated reg value
	*((volatile uint32_t *) 0xA2000008) = regval;			// Write updated reg value
	
	uint32_t regval2 = *((volatile uint32_t *) 0xA0001028 );		// Read updated reg value
	write_mbus_message( 0x03, regval2 );
	write_mbus_message( 0x03, regval2 );
	write_mbus_message( 0x03, regval2 );

	while(1);

}
