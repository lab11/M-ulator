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
#define SPI_DELAY 5 

// Timer seeting
#define SPI_MISO  1	// SPI MISO, Expended in binary
#define SPI_CLK   4
#define SPI_MOSI  0
#define SPI_SS    15

// SRAM setting
#define SPI_0 	0	// SPI: MOSI, SQI: SIO0, Marked: MOSI
#define SPI_1 	1	// SPI: MISO, SQI: SIO1, Marked: MISO
#define SPI_2 	2	// SPI: N/A,  SQI: SIO2, Marked: MOSI1
#define SPI_3 	5	// SPI: HOLD, SQI: SOI3, Marked: MOSI2
#define SPI_SS_SRAM 	14
#define MUX_SEL	13	// 0: From M3, 1: From AFE

// Flash setting
#define SPI_SS_FLASH	12


#define SRAM_READ	0x00000003
#define SRAM_WRITE	0x00000002
#define SRAM_EDIO	0x0000003B
#define SRAM_EQIO	0x00000038
#define SRAM_RSTIO	0x000000FF
#define SRAM_RDMR	0x00000005
#define SRAM_WRMR	0x00000001




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


void handler_ext_int_0(void){ *((volatile uint32_t *) 0xE000E280) = 0x1; }
void handler_ext_int_1(void){ *((volatile uint32_t *) 0xE000E280) = 0x2; }
void handler_ext_int_2(void){ *((volatile uint32_t *) 0xE000E280) = 0x4; }
void handler_ext_int_3(void){ *((volatile uint32_t *) 0xE000E280) = 0x8; }
void handler_ext_int_4(void){ *((volatile uint32_t *) 0xE000E280) = 0x10; }
void handler_ext_int_5(void){ *((volatile uint32_t *) 0xE000E280) = 0x20; }
void handler_ext_int_6(void){ *((volatile uint32_t *) 0xE000E280) = 0x40; }
void handler_ext_int_7(void){ *((volatile uint32_t *) 0xE000E280) = 0x80; }
void handler_ext_int_8(void){ *((volatile uint32_t *) 0xE000E280) = 0x100; }
void handler_ext_int_9(void){
	*((volatile uint32_t *) 0xE000E280) = 0x200;
	*((volatile uint32_t *) 0xA8000000) = 0x0;
}
void handler_ext_int_10(void){ *((volatile uint32_t *) 0xE000E280) = 0x400; }
void handler_ext_int_11(void){ *((volatile uint32_t *) 0xE000E280) = 0x800; }

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

void GPIO_set(uint32_t loc){ *GPIO = (*GPIO | 1 << loc); }

void GPIO_kill(uint32_t loc){ *GPIO = ~(~(*GPIO) | 1 << loc); }


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
	GPIO_set(SPI_SS);
	delay(SPI_DELAY);

	//Start transmit
	GPIO_kill(SPI_SS);

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
		if(*GPIO & 2) readData = readData | (1 << (readBit-i-1));
		delay(SPI_DELAY);
		GPIO_kill(SPI_CLK);
		delay(SPI_DELAY);
		delay(SPI_DELAY);
	}
	GPIO_set(SPI_SS);
	delay(SPI_DELAY);

	return readData;

}


uint32_t readRDMR(){
	volatile uint32_t i;
	volatile uint32_t readData;
	volatile uint32_t pos;
	volatile uint32_t sendData;
	volatile uint32_t readBit;
	readBit = 8;
	readData = 0x00000000;
	sendData = SRAM_RDMR;
	pos = 1 << 7;
//	write_mbus_message(0x13,sendData);

	// Initialization
	GPIO_set(SPI_SS_SRAM);	// Chip select to 1
	GPIO_set(SPI_3);	// Setting Hold to 1
	GPIO_kill(SPI_CLK);	// Setting clk to 0
	GPIO_kill(SPI_0);	// Setting MO to 0
	delay(SPI_DELAY);


	//Start transmit
	GPIO_kill(SPI_SS_SRAM);

	for(i = 0; i < 8; i++){
		if((sendData & pos)) GPIO_set(SPI_0);
		else GPIO_kill(SPI_0);
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
	
	for(i = 0; i < 8; i++){
		GPIO_set(SPI_CLK);
		delay(SPI_DELAY);
//		write_mbus_message(0x13, *GPIO );
//		write_mbus_message(0x13, (1<<SPI_MISO));
//		write_mbus_message(0x13, *GPIO & (1<<SPI_MISO));
//		if(*GPIO & (1<<SPI_MISO)) readData = readData | (1 << (readBit-i-1));
		if(*GPIO & 2) readData = readData | (1 << (readBit-i-1));
		delay(SPI_DELAY);
		GPIO_kill(SPI_CLK);
		delay(SPI_DELAY);
		delay(SPI_DELAY);
	}
	GPIO_set(SPI_SS_SRAM);
	delay(SPI_DELAY);

	return readData;

}




void initSPIModeSRAM(){
	*GPIODIR = 0x00000002;
	GPIO_set(SPI_SS_SRAM);	// Chip select to 1
	GPIO_set(SPI_3);	// Setting Hold to 1
	GPIO_kill(SPI_CLK);	// Setting clk to 0
	GPIO_kill(SPI_0);	// Setting MO to 0
	delay(SPI_DELAY);
}

void startSPISRAM(uint32_t instSRAM, uint32_t addSRAM){
	volatile uint32_t i;
	volatile uint32_t pos;
	pos = 1 << 7;

	write_mbus_message(0x13,instSRAM);

	// Initialization
	GPIO_set(SPI_SS_SRAM);	// Chip select to 1
	GPIO_set(SPI_3);	// Setting Hold to 1
	GPIO_kill(SPI_CLK);	// Setting clk to 0
	GPIO_kill(SPI_0);	// Setting MO to 0
	delay(SPI_DELAY);

	//Start transmit
	GPIO_kill(SPI_SS_SRAM);

	for(i = 0; i < 8; i++){
		if((instSRAM & pos)) GPIO_set(SPI_0);
		else GPIO_kill(SPI_0);
		delay(SPI_DELAY);
		GPIO_set(SPI_CLK);
		delay(SPI_DELAY);
		delay(SPI_DELAY);
		GPIO_kill(SPI_CLK);
		delay(SPI_DELAY);
		pos = pos >> 1;
	}
	
	pos = 1 << 23;
	for(i = 0; i < 24; i++){
		if((addSRAM & pos)) GPIO_set(SPI_0);
		else GPIO_kill(SPI_0);
		delay(SPI_DELAY);
		GPIO_set(SPI_CLK);
		delay(SPI_DELAY);
		delay(SPI_DELAY);
		GPIO_kill(SPI_CLK);
		delay(SPI_DELAY);
		pos = pos >> 1;
	}
	
	delay(SPI_DELAY);
}

void writeByteSPISRAM(uint32_t data){
	volatile uint32_t i;
	volatile uint32_t pos;
	pos = 1 << 7;

//	write_mbus_message(0x13,data);

	for(i = 0; i < 8; i++){
		if((data & pos)) GPIO_set(SPI_0);
		else GPIO_kill(SPI_0);
		delay(SPI_DELAY);
		GPIO_set(SPI_CLK);
		delay(SPI_DELAY);
		delay(SPI_DELAY);
		GPIO_kill(SPI_CLK);
		delay(SPI_DELAY);
		pos = pos >> 1;
	}
	
}

uint32_t readByteSPISRAM(){
	volatile uint32_t i;
	volatile uint32_t readData;
	volatile uint32_t readBit;
	readData = 0x00000000;
	readBit = 8;

	for(i = 0; i < readBit; i++){
		GPIO_set(SPI_CLK);
		delay(SPI_DELAY);
		if(*GPIO & 2) readData = readData | (1 << (readBit-i-1));
		delay(SPI_DELAY);
		GPIO_kill(SPI_CLK);
		delay(SPI_DELAY);
		delay(SPI_DELAY);
	}
	return readData;
}


//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************

int main() {
	volatile uint32_t  SPI_WRITE;
	volatile uint32_t  SPI_READ;
	volatile uint32_t  SPI_INST;
	volatile uint32_t  SPI_ADDR;
	volatile uint32_t  i;

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

	SPI_WRITE = 0x00000000;
	SPI_READ  = 0x00000000;
	SPI_INST  = 0x00000000;
	SPI_ADDR  = 0x00000000;
	
	delay(5000);
	SPI_INST  = SRAM_WRITE;
	initSPIModeSRAM();
	GPIO_kill(MUX_SEL);		// SRAM access from M3

	delay(5000);

	SPI_READ = readRDMR();
	write_mbus_message(0x13, 0xBEEF);
	write_mbus_message(0x13, SPI_READ);
	write_mbus_message(0x13, 0xDEAD);
	


	startSPISRAM(SPI_INST, SPI_ADDR);
	for(i = 0; i < 4; i++){
		writeByteSPISRAM(SPI_WRITE);
		SPI_WRITE = SPI_WRITE + 1;
	}
	initSPIModeSRAM();
	delay(1000);
	
	SPI_INST = SRAM_READ;
	startSPISRAM(SPI_INST, SPI_ADDR);
	for(i = 0; i < 4; i++){
		SPI_READ = readByteSPISRAM();
		write_mbus_message(0x13, SPI_READ);
		delay(5000);
	}
	initSPIModeSRAM();
	
	
	
	while(1){
		delay(5000); //1s
	}

}
