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
  write_mbus_message( 0xE1, gpio_readdata() );
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

	// Read & report default values
	write_mbus_message( 0xE1, gpio_readdata() );
	delay(10);
	write_mbus_message( 0xE2, gpio_read_dir() );
	delay(10);
	write_mbus_message( 0xE3, gpio_read_intmask() );
	delay(10);


	// CPS power on
	*((volatile uint32_t *) 0xA2000008) = 0x01202903;

	// output test - only GPIO[15:8] should change
	gpio_set_dir  ( 0x0000FF00 );
	delay(10);
	gpio_writedata( 0x00121212 );
	delay(10);
	gpio_writedata( 0x00343434 );
	delay(10);
	gpio_writedata( 0x00565656 );
	delay(10);
	write_mbus_message( 0xE2, gpio_read_dir() );
	delay(10);

	// intmask test
	gpio_set_intmask ( 0x000000F0 );
	delay(10);
	write_mbus_message( 0xE3, gpio_read_intmask() );



	while(1){
	}

}
