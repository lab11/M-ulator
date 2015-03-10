//*******************************************************************
//Author: Taekwang jang
//Description: PRCv9E Extended Layer (GPIO, Power Switch, SPI)
//*******************************************************************
#include "mbus.h"
#include "PRCv9E.h"

#define IAI_INT0 (*GPIO &= ~(1 << 5))
#define IAI_INT1 (*GPIO |= 1 << 5)
#define IAI_MISO0 (*GPIO &= ~(1))
#define IAI_MISO1 (*GPIO |= 1)
#define SPI_DELAY 100

#define SPI_MISO  0
#define SPI_CLK   1
#define SPI_MOSI  2
#define SPI_SS    3



volatile uint32_t* GPIO = (uint32_t *) 0xA6000008;
volatile uint32_t* GPIODIR = (uint32_t *) 0xA6001000;
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
//	write_mbus_message(0x13, 0x0F0F00000);
}
void handler_ext_int_1(void){
	*((volatile uint32_t *) 0xE000E280) = 0x2;
//	write_mbus_message(0x13, 0x0F0F00001);
}
void handler_ext_int_2(void){
	*((volatile uint32_t *) 0xE000E280) = 0x4;
//	write_mbus_message(0x13, 0x0F0F00002);
}
void handler_ext_int_3(void){
	*((volatile uint32_t *) 0xE000E280) = 0x8;
//	write_mbus_message(0x13, 0x0F0F00003);
}
void handler_ext_int_4(void){
//	uint32_t *address;
//	uint32_t data;
//	address	= (uint32_t *) 0xA6000000;
//	data = *address;
//	write_mbus_message(0xF0, data);
	*((volatile uint32_t *) 0xE000E280) = 0x10;
//	write_mbus_message(0x13, 0x0F0F0004);
}
void handler_ext_int_5(void){
	*((volatile uint32_t *) 0xE000E280) = 0x20;
//	write_mbus_message(0x13, 0x0F0F0005);
}
void handler_ext_int_6(void){
	*((volatile uint32_t *) 0xE000E280) = 0x40;
//	write_mbus_message(0x13, 0x0F0F0006);
}
void handler_ext_int_7(void){
	*((volatile uint32_t *) 0xE000E280) = 0x80;
//	write_mbus_message(0x13, 0x0F0F0007);
}
void handler_ext_int_8(void){
	*((volatile uint32_t *) 0xE000E280) = 0x100;
//	write_mbus_message(0x13, 0x0F0F0008);
//	write_mbus_message(0x13, *((volatile uint32_t *) 0xA500021C));
//	write_mbus_message(0x13, *((volatile uint32_t *) 0xA5000220));
}
void handler_ext_int_9(void){
	*((volatile uint32_t *) 0xE000E280) = 0x200;
	*((volatile uint32_t *) 0xA8000000) = 0x0;
//	*((volatile uint32_t *) 0xA8000004) = 0x0;
//	write_mbus_message(0x13, 0x0F0F0009);
//	write_mbus_message(0x13, *((volatile uint32_t *) 0xA8000000));
//	write_mbus_message(0x13, *((volatile uint32_t *) 0xA8000004));
}
void handler_ext_int_10(void){
	*((volatile uint32_t *) 0xE000E280) = 0x400;
//	write_mbus_message(0x13, 0x0F0F000A);
}
void handler_ext_int_11(void){
	*((volatile uint32_t *) 0xE000E280) = 0x800;
//	write_mbus_message(0x13, 0x0F0F000B);
}

//***************************************************
// Subfunctions
//***************************************************



void initialize(){
	enumerated = 0xABCDEF01;
	write_mbus_message(0x13,0xeeeeeeee);
	//Truning off the watch dog
	*((volatile uint32_t *) 0xA5000000) = 0;
}

void gpio_control(uint32_t data){
	uint32_t *address;
	address = (uint32_t *) 0xA6000000;
	*address = data;
//	write_mbus_message(0x13,data);
}

void GPIO_set(uint32_t loc){
	*GPIO = (*GPIO | 1 << loc);
}

void GPIO_kill(uint32_t loc){
	*GPIO = ~(~(*GPIO) | 1 << loc);
}

/*
void bbspi_transfer(int bb_value){
	uint32_t i = 0;
	uint32_t single_bit = 0x00;

	write_mbus_message(0x13,bb_value);
	while((~(*GPIO)) & (1 << 2));
	
		for(i=0 ; i<8 ; i++){
			
			//idle while SCLK is 0
			while((~(*GPIO)) & (1 << 2));
			
			//once SCLK is 1, we need to put value on so it can be clocked on the falling edge
			//assign the value to variable to do manipulation on
			single_bit = bb_value;
			//now shift it from it's location to the 0th bit to get a binary 1 or 0
			single_bit = single_bit >> i;
			//examine if it's a 1 or 0
			if((single_bit & 0x01) == 0x01){
				IAI_MISO1;
			}else{
				IAI_MISO0;
			}
			
			//wait for the data to get clocked
			while(*GPIO & (1 << 2));
		}
}
*/

uint32_t bbspi_transfer(uint32_t sendData, uint32_t writeBit, uint32_t readBit){
	volatile uint32_t i;
	volatile uint32_t readData;
	volatile uint32_t pos;
	readData = 0x00000000;
	pos = 1 << (writeBit-1);
//	write_mbus_message(0x13,sendData);

	// Initialization
	GPIO_kill(SPI_CLK);
	GPIO_kill(SPI_MOSI);
//	GPIO_set(SPI_SS);
	GPIO_set(15);
	delay(SPI_DELAY);

	//Start transmit
//	GPIO_kill(SPI_SS);
	GPIO_kill(15);

	for(i = 0; i < writeBit; i++){
		if((sendData & pos)) GPIO_set(SPI_MOSI);
		else GPIO_kill(SPI_MOSI);
		delay(SPI_DELAY);
		GPIO_set(SPI_CLK);
		delay(SPI_DELAY);
		delay(SPI_DELAY);
		GPIO_kill(SPI_CLK);
		delay(SPI_DELAY);
		pos = pos >> 1;
	}
	GPIO_kill(SPI_MOSI);
	delay(SPI_DELAY);
	
	for(i = 0; i < readBit; i++){
		GPIO_set(SPI_CLK);
		delay(SPI_DELAY);
		if(*GPIO & 1) readData = readData | (1 << (readBit-i-1));
		delay(SPI_DELAY);
		GPIO_kill(SPI_CLK);
		delay(SPI_DELAY);
		delay(SPI_DELAY);
	}
//	GPIO_set(SPI_SS);
	GPIO_set(15);
	delay(SPI_DELAY);

	return readData;

}


//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************

int main() {
	volatile uint32_t  SPI_WRITE;
	volatile uint32_t  SPI_READ;
//	volatile uint32_t i;

	//Clear All Pending Interrupts
	*((volatile uint32_t *) 0xE000E280) = 0x3FF;
	//Enable Interrupts
//	*((volatile uint32_t *) 0xE000E100) = 0x07F;	// All timer ignored
//	*((volatile uint32_t *) 0xE000E100) = 0x3FF;	// All interrupt enable
	*((volatile uint32_t *) 0xE000E100) = 0;	// All interrupt disable


	//initialize	
	if( enumerated != 0xABCDEF01 ) initialize();


	// MEM&CORE CLK /16
	*((volatile uint32_t *) 0xA2000008) = 0x120F903;	//Isolation disable
//	*((volatile uint32_t *) 0xA2000008) = 0x020F903;	//Isolation Enable
	// MEM&CORE CLK /8
//	*((volatile uint32_t *) 0xA2000008) = 0x1202903;

	*GPIODIR = 0x00000001;				// 0:MISO 1:CLK 2:MOSI 3: SS
//	*GPIODIR = 0x000000FE;				// 0:MISO 1:CLK 2:MOSI 3: SS

/*
	write_mbus_message(0x13,0x0000BEEF);
	SPI_WRITE = 0x1C;				//Read to Oscillation control
	SPI_READ = bbspi_transfer(SPI_WRITE, 8, 16);
	write_mbus_message(0x13,SPI_READ);
	delay(1000);
	SPI_WRITE = (0x9C << 8) | (1 << 7) | SPI_READ;				//Read to Oscillation control
	SPI_READ = bbspi_transfer(SPI_WRITE, 16, 0);
	delay(1000);
	SPI_WRITE = 0x1C;				//Read to Oscillation control
	SPI_READ = bbspi_transfer(SPI_WRITE, 8, 16);
	write_mbus_message(0x13,SPI_READ);

	write_mbus_message(0x13,0x0000DEAD);
*/
	SPI_WRITE = 0x01;	

	delay(10000);

	
	while(1){
		SPI_READ = bbspi_transfer(SPI_WRITE, 8, 16);
		write_mbus_message(0x13,SPI_READ);
//		gpio_control(gpio_data);
/*		if(SPI_WRITE == 0x0000007F){
			SPI_WRITE = 0;
		}
		else{
			SPI_WRITE = SPI_WRITE +0x1;
		}
*/
		delay(5000); //1s
	}

}
