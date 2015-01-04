//*******************************************************************
//Author: Gyouho Kim / Yoonmyung Lee / ZhiYoong Foo
//Description:	Post-tapeout Verification for PRCv8 (NOV 2013)
//		#05 - TIMERS
//*******************************************************************
#include "mbus.h"
#include "PRCv9.h"

#define RAD_ADDR 0x9
#define SNS_ADDR 0xA

#define DELAY_1 2000 //20000 // 1s

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
// Watchdog timer
  *((volatile uint32_t *) 0xE000E280) = 0x40;
	config_timer( 1, 0, 0, 0, 60000 ); // timer id, go, roi, init, sat (deafult 0x2FFFFFF)
	delay(100);
    write_mbus_register(SNS_ADDR,1,0xA);
}
void handler_ext_int_7(void){
// Timer 1
  *((volatile uint32_t *) 0xE000E280) = 0x80;
	config_timer( 1, 0, 0, 40000, 60000 );
	delay(100);
    write_mbus_register(SNS_ADDR,1,0xA);
}
void handler_ext_int_8(void){
// Timer 2
  *((volatile uint32_t *) 0xE000E280) = 0x100;
	config_timer( 2, 0, 0, 40000, 60000 );
	delay(100);
    write_mbus_register(SNS_ADDR,2,0xA);
}
void handler_ext_int_9(void){
// Timer 3
  *((volatile uint32_t *) 0xE000E280) = 0x200;
	config_timer( 3, 0, 1,    0, 300000 );
	delay(100);
    write_mbus_register(SNS_ADDR,3,0xA);
}


//*******************************************************************
//Main **************************************************************
//*******************************************************************
int main() {

	//Clear All Pending Interrupts
	*((volatile uint32_t *) 0xE000E280) = 0x03FF;
	//Enable Interrupts
	*((volatile uint32_t *) 0xE000E100) = 0x03FF;

	//Config WD timer to about 10 sec
	//config_timer( timer_id, go, roi, init_val, sat_val )
	config_timer( 0, 1, 0, 0, 1000000 );

	//Enumeration
//	enumerate(RAD_ADDR);
//	asm ("wfi;");
//	enumerate(SNS_ADDR);
//	asm ("wfi;");

	delay(DELAY_1);
	delay(DELAY_1);
	delay(DELAY_1);
  

	while(1);

}
