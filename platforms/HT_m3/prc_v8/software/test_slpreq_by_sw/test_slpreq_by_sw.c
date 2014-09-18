//*******************************************************************
//Author: Yoonmyung Lee / ZhiYoong Foo
//Description:	Test code for WatchDog Timer (Nov 2013 Tape out)
//		Configure WD timer and let it fail
//*******************************************************************
#include "mbus.h"
#include "PRCv8.h"

#define RAD_ADDR 0x9
#define SNS_ADDR 0xA

//Interrupt Handlers
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
	config_timer( 1, 0, 0, 0, 6000 );
    write_mbus_register(SNS_ADDR,1,0xA);
}
void handler_ext_int_8(void){
  *((volatile uint32_t *) 0xE000E280) = 0x100;
	config_timer( 2, 0, 0, 0, 6000 );
    write_mbus_register(SNS_ADDR,2,0xA);
}
void handler_ext_int_9(void){
  *((volatile uint32_t *) 0xE000E280) = 0x200;
	config_timer( 3, 0, 0, 0, 3000 );
    write_mbus_register(SNS_ADDR,3,0xA);
}


int main() {

	//Clear All Pending Interrupts
	*((volatile uint32_t *) 0xE000E280) = 0x03FF;
	//Enable Interrupts
	*((volatile uint32_t *) 0xE000E100) = 0x03FF;

	//Enumeration
	enumerate(RAD_ADDR);
	asm ("wfi;");
	enumerate(SNS_ADDR);
	asm ("wfi;");

	//MBus threshold adjust
	*((volatile uint32_t *) 0xA2000004) = 0x1234ABCD;

	//PMUCFB_DIV5_OVRD test
	*((volatile uint32_t *) 0xA200000C) = 0x4F770029;

	//Sleep Request by SW
	*((volatile uint32_t *) 0xA3000000) = 0x0004;


	//
	while(1);
}
