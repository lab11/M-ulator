#include <stdint.h>
#include <stdbool.h>

#define I2C_SDA    0x00000020 // SDA = GPIO_DATA[5]

volatile uint32_t* _GPIO_DATA = (volatile uint32_t *) 0xA0005000;
volatile uint32_t* _GPIO_DIR  = (volatile uint32_t *) 0xA0005400; // 0: output, 1: input

//***************************************************
//Functions from mbus.h and PRCv9E.h
//***************************************************

//***************************************************
//Interrupt Handlers
//Must clear pending bit!
//***************************************************
//void handler_ext_int_0(void)  __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_1(void)  __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_2(void)  __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_3(void)  __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_4(void)  __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_5(void)  __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_6(void)  __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_7(void)  __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_8(void)  __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_9(void)  __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_12(void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_13(void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_14(void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_15(void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_16(void) __attribute__ ((interrupt ("IRQ")));
//
//void handler_ext_int_0(void) { *((volatile uint32_t *) 0xE000E280) = 0x1; }
//void handler_ext_int_1(void) { *((volatile uint32_t *) 0xE000E280) = 0x2; }
//void handler_ext_int_2(void) { *((volatile uint32_t *) 0xE000E280) = 0x4; }
//void handler_ext_int_3(void) { *((volatile uint32_t *) 0xE000E280) = 0x8; }
//void handler_ext_int_4(void) { *((volatile uint32_t *) 0xE000E280) = 0x10; }
//void handler_ext_int_5(void) { *((volatile uint32_t *) 0xE000E280) = 0x20; }
//void handler_ext_int_6(void) { *((volatile uint32_t *) 0xE000E280) = 0x40; }
//void handler_ext_int_7(void) { *((volatile uint32_t *) 0xE000E280) = 0x80; }
//void handler_ext_int_8(void) { *((volatile uint32_t *) 0xE000E280) = 0x100; }
//void handler_ext_int_9(void) { *((volatile uint32_t *) 0xE000E280) = 0x200; }
//void handler_ext_int_10(void){ *((volatile uint32_t *) 0xE000E280) = 0x400; }
//void handler_ext_int_11(void){ *((volatile uint32_t *) 0xE000E280) = 0x800; }
//void handler_ext_int_12(void){ *((volatile uint32_t *) 0xE000E280) = 0x1000; }
//void handler_ext_int_13(void){ *((volatile uint32_t *) 0xE000E280) = 0x2000; }
//void handler_ext_int_14(void){ *((volatile uint32_t *) 0xE000E280) = 0x4000; }
//void handler_ext_int_15(void){ *((volatile uint32_t *) 0xE000E280) = 0x8000; }
//void handler_ext_int_16(void){ *((volatile uint32_t *) 0xE000E280) = 0x10000; }

//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************

int main() {

	//Clear All Pending Interrupts
	*((volatile uint32_t *) 0xE000E280) = 0x1FFFF;
	//Enable Interrupts
	*((volatile uint32_t *) 0xE000E100) = 0x0;	// All interrupt disable
	
	//Truning off the watch dog
	*((volatile uint32_t *) 0xA0001200) = 0;

	//Initialize
	*_GPIO_DIR = 0x00000020;
	*_GPIO_DATA = 0x00000003;
	
	while(1){
		if(*_GPIO_DATA & I2C_SDA)
			*_GPIO_DATA = 0x0000000F;
		else
			*_GPIO_DATA = 0x00000003;
	}

}
