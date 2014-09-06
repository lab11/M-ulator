//*******************************************************************
//Author: Yoonmyung Lee 
//Description:	PRCv9E_CPU verification 
//		TIMER
//*******************************************************************
#include "mbus.h"
//#include "PRCv8.h"
#include "PRCv9E.h"

#define RAD_ADDR 0x9
#define SNS_ADDR 0xA

#define TIMER0_PERIOD 1000000
#define TIMER1_PERIOD 1000
#define TIMER2_PERIOD 2000

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
  config_timer( 0, 0, 0, 0, TIMER0_PERIOD );
  write_mbus_message( 0x06, 0x6666 );
}
void handler_ext_int_7(void){	
  *((volatile uint32_t *) 0xE000E280) = 0x80;
  config_timer( 1, 0, 0, 0, TIMER1_PERIOD );
  write_mbus_message( 0x07, 0x7777 );
}
void handler_ext_int_8(void){
  *((volatile uint32_t *) 0xE000E280) = 0x100;
  write_mbus_message( 0x08, 0x8888 );
}
void handler_ext_int_9(void){
//  *((volatile uint32_t *) 0xE000E280) = 0x200;
//  config_timer( 3, 1, 0, 0, TIMER3_PERIOD );
//  write_mbus_message( 0x03, 0x3333 );
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
	
	// Set watchdog timer 
	config_timer( 0, 1, 0,     0, TIMER0_PERIOD );  //==> Watchdog!!!!
	// Set Timer1 (32bit)
	config_timer( 1, 1, 0,     0, TIMER1_PERIOD );  // IRQ7 @ 1000 cycle
	// Set Timer2 (16bit)
	timer16b_cfg_compare( 0, 3000);		// IRQ8 @ 3000 cycle 
	timer16b_cfg_compare( 1, 4000); 	// IRQ8 @ 4000 cycle
	timer16b_go( 1);


	while(1){
		delay(1800);
  		write_mbus_message( 0xE1, timer16b_read_compare(0) );
		delay(10);
  		write_mbus_message( 0xE2, timer16b_read_compare(1) );
		delay(10);
  		write_mbus_message( 0xE3, timer16b_read_capture(0) );
		delay(10);
  		write_mbus_message( 0xE4, timer16b_read_capture(1) );
		delay(10);
  		write_mbus_message( 0xE5, timer16b_read_capture(2) );
		delay(10);
  		write_mbus_message( 0xE6, timer16b_read_capture(3) );
		delay(10);
  		write_mbus_message( 0xE7, timer16b_read_count() );
		delay(10);
  		write_mbus_message( 0xE8, timer16b_read_stat() );
	}

}
