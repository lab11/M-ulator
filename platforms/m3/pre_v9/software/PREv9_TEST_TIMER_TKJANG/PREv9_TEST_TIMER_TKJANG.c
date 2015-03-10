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
	write_mbus_message(0x13, 0x0F0F0004);
}
void handler_ext_int_5(void){
	*((volatile uint32_t *) 0xE000E280) = 0x20;
	write_mbus_message(0x13, 0x0F0F0005);
}
void handler_ext_int_6(void){
	*((volatile uint32_t *) 0xE000E280) = 0x40;
	write_mbus_message(0x13, 0x0F0F0006);
}
void handler_ext_int_7(void){
	*((volatile uint32_t *) 0xE000E280) = 0x80;
	write_mbus_message(0x13, 0x0F0F0007);
}
void handler_ext_int_8(void){
	*((volatile uint32_t *) 0xE000E280) = 0x100;
//	write_mbus_message(0x13, 0x0F0F0008);
//	write_mbus_message(0x13, *((volatile uint32_t *) 0xA500021C));
//	write_mbus_message(0x13, *((volatile uint32_t *) 0xA5000220));
}
void handler_ext_int_9(void){
	*((volatile uint32_t *) 0xE000E280) = 0x200;
	write_mbus_message(0x13, 0x0F0F0009);
//	write_mbus_message(0x13, *((volatile uint32_t *) 0xA8000000));
//	write_mbus_message(0x13, *((volatile uint32_t *) 0xA8000004));
}
void handler_ext_int_10(void){
	*((volatile uint32_t *) 0xE000E280) = 0x400;
	write_mbus_message(0x13, 0x0F0F000A);
}
void handler_ext_int_11(void){
	*((volatile uint32_t *) 0xE000E280) = 0x800;
	write_mbus_message(0x13, 0x0F0F000B);
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
//	write_mbus_message(0x13,data);
}


//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************

int main() {
	volatile uint32_t *address_gpio_direction;
	volatile uint32_t  gpio_data;
	volatile uint32_t  spi_data;
	volatile uint32_t i;

	//Clear All Pending Interrupts
	*((volatile uint32_t *) 0xE000E280) = 0x3FF;
	//Enable Interrupts
//	*((volatile uint32_t *) 0xE000E100) = 0x07F;	// All timer ignored
	*((volatile uint32_t *) 0xE000E100) = 0x3FF;	// All interrupt enable
//	*((volatile uint32_t *) 0xE000E100) = 0;	// All interrupt enable
	
	if( enumerated != 0xABCDEF01 ) initialize();

	//Truning off the watch dog
	*((volatile uint32_t *) 0xA5000000) = 0;


	// MEM&CORE CLK /16
	*((volatile uint32_t *) 0xA2000008) = 0x120F903;	//Isolation disable
//	*((volatile uint32_t *) 0xA2000008) = 0x020F903;	//Isolation Enable
	// MEM&CORE CLK /8
//	*((volatile uint32_t *) 0xA2000008) = 0x1202903;


	address_gpio_direction = (volatile uint32_t *) 0xA6001000;
	*address_gpio_direction = 0x00000000;		// Direction register is reversed.

	//Initialize

	gpio_data = 0x0003;
	gpio_control(gpio_data);
	
	// SPI control
	//*((volatile uint32_t *) 0xA7000000 ) = 0x0217;	//SSPCR0 TI Mode
	*((volatile uint32_t *) 0xA7000000 ) = 0x00000C87;	//SSPCR0 Motorola Mode
	*((volatile uint32_t *) 0xA7000004 ) = 0x00000002;	//SSPCR1 ? Slave SyncEnable LookBack
	//	*((volatile uint32_t *) 0xA700000C ) = 0x;	//SSPSR	Read Only

	//	*((volatile uint32_t *) 0xA7000014 ) = 0xF;	//SSPIMSC
	//	*((volatile uint32_t *) 0xA7000018 ) = 0x;	//SSPRIS Read Only
	//	*((volatile uint32_t *) 0xA700001C ) = 0x;	//SSPMIS Read only
	//	*((volatile uint32_t *) 0xA7000020 ) = 0x3;	//SSPICR Write Only

	//	*((volatile uint32_t *) 0xA7000024 ) = 0x3;	//SSPDMACR
		*((volatile uint32_t *) 0xA7000024 ) = 0x0;	//SSPDMACR


	gpio_data = 0x0000;
	gpio_control(gpio_data);
//	*((volatile uint32_t *) 0xA7000008 ) = 0x00000110; //WREN
	*((volatile uint32_t *) 0xA7000008 ) = 0x00000101; //RDSR
	delay(1000);

	
	while(1){
		for(i = 0; i < 10; i++){
			*((volatile uint32_t *) 0xA7000008 ) = 0x00000101; //RDSR
		}
		for(i = 0; i < 10; i++){
			spi_data = *((volatile uint32_t *) 0xA7000008 );
			write_mbus_message(0x13, spi_data);
		}
		delay(5000);
	}

}
