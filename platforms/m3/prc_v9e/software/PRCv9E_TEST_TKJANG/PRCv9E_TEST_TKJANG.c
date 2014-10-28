//*******************************************************************
//Author: Taekwang jang
//Description: PRCv9E Extended Layer (GPIO, Power Switch, SPI)
//*******************************************************************
#include "mbus.h"
#include "PRCv9E.h"


volatile uint32_t enumerated;

//***************************************************
//Interrupt Handlers
//Must clear pending bit!
//***************************************************
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
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));


void handler_ext_int_0(void){
	*((volatile uint32_t *) 0xE000E280) = 0x1;
	write_mbus_message(0x13, 0x0F0F00000);
}
void handler_ext_int_1(void){
	*((volatile uint32_t *) 0xE000E280) = 0x2;
	write_mbus_message(0x13, 0x0F0F00001);
}
void handler_ext_int_2(void){
	*((volatile uint32_t *) 0xE000E280) = 0x4;
	write_mbus_message(0x13, 0x0F0F00002);
}
void handler_ext_int_3(void){
	*((volatile uint32_t *) 0xE000E280) = 0x8;
	write_mbus_message(0x13, 0x0F0F00003);
}
void handler_ext_int_4(void){
//	uint32_t *address;
//	uint32_t data;
//	address	= (uint32_t *) 0xA6000000;
//	data = *address;
//	write_mbus_message(0xF0, data);
	*((volatile uint32_t *) 0xE000E280) = 0x10;
	write_mbus_message(0x13, 0x0F0F00004);
}
void handler_ext_int_5(void){
	*((volatile uint32_t *) 0xE000E280) = 0x20;
	write_mbus_message(0x13, 0x0F0F00005);
}
void handler_ext_int_6(void){
	*((volatile uint32_t *) 0xE000E280) = 0x40;
	write_mbus_message(0x13, 0x0F0F00006);
}
void handler_ext_int_7(void){
	*((volatile uint32_t *) 0xE000E280) = 0x80;
	write_mbus_message(0x13, 0x0F0F00007);
}
void handler_ext_int_8(void){
	*((volatile uint32_t *) 0xE000E280) = 0x100;
	write_mbus_message(0x13, 0x0F0F00008);
}
void handler_ext_int_9(void){
	*((volatile uint32_t *) 0xE000E280) = 0x200;
	write_mbus_message(0x13, 0x0F0F00009);
}
void handler_ext_int_10(void){
	*((volatile uint32_t *) 0xE000E280) = 0x400;
	write_mbus_message(0x13, 0x0F0F0000A);
}
void handler_ext_int_11(void){
	*((volatile uint32_t *) 0xE000E280) = 0x800;
	write_mbus_message(0x13, 0x0F0F0000B);
}

//***************************************************
// Subfunctions
//***************************************************



void initialize(){
	enumerated = 0xABCDEF01;
	write_mbus_message(0x13,0xeeeeeeee);
}

void gpio_control(uint32_t data){
	uint32_t *address;
	address = (uint32_t *) 0xA6000000;
	*address = data;
	write_mbus_message(0x13,data);
}


//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************

int main() {
	volatile uint32_t *address_gpio_direction;
	volatile uint32_t  gpio_data;

	//Clear All Pending Interrupts
	*((volatile uint32_t *) 0xE000E280) = 0x3FF;
	//Enable Interrupts
//	*((volatile uint32_t *) 0xE000E100) = 0x07F;	// All timer ignored
	*((volatile uint32_t *) 0xE000E100) = 0x3FF;	// All interrupt enable
	
	if( enumerated != 0xABCDEF01 ) initialize();


	// MEM&CORE CLK /16
//	*((volatile uint32_t *) 0xA2000008) = 0x1203903;	//Isolation disable
	*((volatile uint32_t *) 0xA2000008) = 0x1203903;	//Isolation Enable
	// MEM&CORE CLK /8
//	*((volatile uint32_t *) 0xA2000008) = 0x1202903;


	address_gpio_direction = (volatile uint32_t *) 0xA6001000;
	*address_gpio_direction = 0x0000FFFF;

	//Initialize

	gpio_data = 0x00000000;
	gpio_control(gpio_data);
	while(1){
		gpio_control(gpio_data);
		if(gpio_data == 0x0000FFFF)   gpio_data = 0;
		else 			    gpio_data = gpio_data +1;
		delay(1000);
	}

}
