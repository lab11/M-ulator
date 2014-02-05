//*******************************************************************
//Author: Yoonmyung Lee / ZhiYoong Foo
//Description:	Post-tapeout Verification for PRCv8 (NOV 2013)
//		#04 - CLKGENv2
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


	uint8_t fastmode;
	int8_t div_cm;
//	int8_t div_mbus;
	int8_t ring;

	for( fastmode=0; fastmode<=1; ++fastmode ){
		for( div_cm=3; div_cm>=0; --div_cm ){
			for( ring=3; ring>=0; --ring ){
				delay(100000);
				set_clkfreq( fastmode, div_cm, div_cm, ring);
				delay(100000);
				write_mbus_message( 0x03, (fastmode<<6)|(div_cm<<4)|(div_cm<<4)|ring );
			}
		}
	}


	sleep();
	while(1);

}
