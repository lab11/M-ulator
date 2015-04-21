//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv9E Extended Layer for controlling Flash thru GPIO
//Note: Based on PREv9_TEST_SYS_TKJANG
//*******************************************************************
#include "mbus.h"
#include "PRCv9E.h"

// GPIO Pin Assignment
#define FLS_CIN  12
#define FLS_DIN  13
#define FLS_COUT 14
#define FLS_DOUT 15

#define GPIO_MBus_HalfCycle 10
#define GPIO_MBus_RxBeginDelay 100

volatile uint32_t* GPIO    = (volatile uint32_t *) 0xA6000008;
volatile uint32_t* GPIODIR = (volatile uint32_t *) 0xA6001000;
volatile uint32_t  GPIO_WRITE;
volatile uint32_t  GPIO_RxAddr;
volatile uint32_t  GPIO_RxData0;
volatile uint32_t  GPIO_RxData1;
volatile uint32_t  GPIO_RxData2;
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
	//Clear All Pending Interrupts
	*((volatile uint32_t *) 0xE000E280) = 0x3FF;
	//Enable Interrupts
//	*((volatile uint32_t *) 0xE000E100) = 0x07F;	// All timer ignored
//	*((volatile uint32_t *) 0xE000E100) = 0x3FF;	// All interrupt enable
	*((volatile uint32_t *) 0xE000E100) = 0;	// All interrupt disable
	// MEM&CORE CLK /16
	*((volatile uint32_t *) 0xA2000008) = 0x120F903;	//Isolation disable
	//Truning off the watch dog
	*((volatile uint32_t *) 0xA5000000) = 0;
	write_mbus_message(0x13,0xEEEEEEEE);
}

void GPIO_ctrl(volatile uint32_t data){ 
	GPIO_WRITE = data;
	*GPIO = GPIO_WRITE; 
}
void GPIO_set(uint32_t loc){ 
	GPIO_WRITE = (GPIO_WRITE | (1 << loc));
	*GPIO = GPIO_WRITE;
}
void GPIO_kill(uint32_t loc){ 
	GPIO_WRITE = ~( (~GPIO_WRITE) | (1 << loc));
	*GPIO = GPIO_WRITE;
}
void GPIO_set2(uint32_t loc0, uint32_t loc1){ 
	GPIO_WRITE = (GPIO_WRITE | (1 << loc0) | (1 << loc1));
	*GPIO = GPIO_WRITE;
}

void FLSMBusGPIO_initialization(){
	// GPIO_DIR = 0 (output) (ex. FLS_CIN, FLS_DIN)
	//            1 (input) (ex. FLS_COUT, FLS_DOUT)
	*GPIODIR = (0x00000000 | (1 << FLS_COUT) | (1 << FLS_DOUT));
	GPIO_WRITE = 0x00000000;
	GPIO_RxAddr = 0x00000000;
	GPIO_RxData0 = 0x00000000;
	GPIO_RxData1 = 0x00000000;
	GPIO_RxData2 = 0x00000000;
}

void FLSMBusGPIO_resetCinDin () {
	GPIO_set2(FLS_CIN, FLS_DIN);
	delay(GPIO_MBus_HalfCycle);
}

void FLSMBusGPIO_toggleCinDin (volatile uint32_t data, volatile uint32_t numBit) {
	volatile uint32_t i;
	volatile uint32_t pos = 1 << (numBit - 1);

	for (i=0; i<numBit; i++){
		GPIO_kill(FLS_CIN);
		if (data & pos) GPIO_set(FLS_DIN);
		else GPIO_kill(FLS_DIN);
		delay(GPIO_MBus_HalfCycle);
		GPIO_set(FLS_CIN);
		delay(GPIO_MBus_HalfCycle);
		pos = pos >> 1;
	}

}

void FLSMBusGPIO_interjection () {
	volatile uint32_t i;

	GPIO_set(FLS_CIN);

	for (i=0; i<6; i++){
		GPIO_kill(FLS_DIN);
		delay(GPIO_MBus_HalfCycle);
		GPIO_set(FLS_DIN);
		delay(GPIO_MBus_HalfCycle);
	}
}

void FLSMBusGPIO_send8bit (volatile uint32_t short_prefix, volatile uint32_t data_0) {
	volatile uint32_t last_data_bit = (0x00000001 & data_0);
	volatile uint32_t tail_sequence = ((last_data_bit << 3) | 0x00000007); // EOM is always 1
	data_0 = data_0 >> (32 - 8); // Move the MSB 8bit to the right-most location.

	GPIO_kill(FLS_DIN); // Pull-Down Data
	delay(GPIO_MBus_HalfCycle);
	FLSMBusGPIO_toggleCinDin (0x00000000, 3); // Arbitration Sequence
	FLSMBusGPIO_toggleCinDin (short_prefix, 8); // Short Prefix
	FLSMBusGPIO_toggleCinDin (data_0, 8); // Data
	FLSMBusGPIO_interjection (); // Interjection
	FLSMBusGPIO_toggleCinDin (tail_sequence, 4); // Tail Sequence
}

void FLSMBusGPIO_send32bit (volatile uint32_t short_prefix, volatile uint32_t data_0) {
	volatile uint32_t last_data_bit = (0x00000001 & data_0);
	volatile uint32_t tail_sequence = ((last_data_bit << 3) | 0x00000007); // EOM is always 1

	GPIO_kill(FLS_DIN); // Pull-Down Data
	delay(GPIO_MBus_HalfCycle);
	FLSMBusGPIO_toggleCinDin (0x00000000, 3); // Arbitration Sequence
	FLSMBusGPIO_toggleCinDin (short_prefix, 8); // Short Prefix
	FLSMBusGPIO_toggleCinDin (data_0, 32); // Data
	FLSMBusGPIO_interjection (); // Interjection
	FLSMBusGPIO_toggleCinDin (tail_sequence, 4); // Tail Sequence
}

void FLSMBusGPIO_send64bit (volatile uint32_t short_prefix, volatile uint32_t data_0, volatile uint32_t data_1) {
	volatile uint32_t last_data_bit = (0x00000001 & data_0);
	volatile uint32_t tail_sequence = ((last_data_bit << 3) | 0x00000007); // EOM is always 1

	GPIO_kill(FLS_DIN); // Pull-Down Data
	delay(GPIO_MBus_HalfCycle);
	FLSMBusGPIO_toggleCinDin (0x00000000, 3); // Arbitration Sequence
	FLSMBusGPIO_toggleCinDin (short_prefix, 8); // Short Prefix
	FLSMBusGPIO_toggleCinDin (data_0, 32); // Data_0
	FLSMBusGPIO_toggleCinDin (data_1, 32); // Data_1
	FLSMBusGPIO_interjection (); // Interjection
	FLSMBusGPIO_toggleCinDin (tail_sequence, 4); // Tail Sequence
}

void FLSMBusGPIO_send96bit (volatile uint32_t short_prefix, volatile uint32_t data_0, volatile uint32_t data_1, volatile uint32_t data_2) {
	volatile uint32_t last_data_bit = (0x00000001 & data_0);
	volatile uint32_t tail_sequence = ((last_data_bit << 3) | 0x00000007); // EOM is always 1

	GPIO_kill(FLS_DIN); // Pull-Down Data
	delay(GPIO_MBus_HalfCycle);
	FLSMBusGPIO_toggleCinDin (0x00000000, 3); // Arbitration Sequence
	FLSMBusGPIO_toggleCinDin (short_prefix, 8); // Short Prefix
	FLSMBusGPIO_toggleCinDin (data_0, 32); // Data_0
	FLSMBusGPIO_toggleCinDin (data_1, 32); // Data_1
	FLSMBusGPIO_toggleCinDin (data_2, 32); // Data_2
	FLSMBusGPIO_interjection (); // Interjection
	FLSMBusGPIO_toggleCinDin (tail_sequence, 4); // Tail Sequence
}

uint32_t FLSMBusGPIO_getCoutDout () {
	// If DOUT = 0, COUT = 0: Return 0
	// If DOUT = 0, COUT = 1: Return 1
	// If DOUT = 1, COUT = 0: Return 2
	// If DOUT = 1, COUT = 1: Return 3
	volatile uint32_t mask = ((1 << FLS_COUT) | (1 << FLS_DOUT));
	volatile uint32_t data = ((*GPIO & mask) >> FLS_COUT);
	return data;
}

uint32_t FLSMBusGPIO_getCout () {
	volatile uint32_t mask = (1 << FLS_COUT);
	volatile uint32_t data = ((*GPIO & mask) >> FLS_COUT);
	return data;
}

uint32_t FLSMBusGPIO_getDout () {
	volatile uint32_t mask = (1 << FLS_DOUT);
	volatile uint32_t data = ((*GPIO & mask) >> FLS_DOUT);
	return data;
}

uint32_t FLSMBusGPIO_dataFwd () {
	volatile uint32_t rxBit = FLSMBusGPIO_getDout();
	if (rxBit==1) GPIO_set(FLS_DIN);
	else GPIO_kill(FLS_DIN);
	return rxBit;
}

uint32_t FLSMBusGPIO_rxMsg () {
	// Received Addr is stored in GPIO_RxAddr
	// Received Data is stored in GPIO_RxData0, GPIO_RxData1, GPIO_RxData2
	volatile uint32_t clockNotForwarded = 0;
	volatile uint32_t isLast = 0;
	volatile uint32_t rxBit = 0;
	volatile uint32_t numRxBit = 0;
	GPIO_RxAddr = 0x00000000;

	while(1) {
		if (FLSMBusGPIO_getCoutDout()==1) {
			delay(GPIO_MBus_RxBeginDelay); // Optional Delay
			break;
		}
	}

	FLSMBusGPIO_toggleCinDin (0x00000004, 3); // Arbitration Sequence

	while(1) {
		GPIO_kill(FLS_CIN);

		rxBit = FLSMBusGPIO_dataFwd();
		if ((FLSMBusGPIO_getCout()==1) & (clockNotForwarded==0)) clockNotForwarded = 1;
		else if (FLSMBusGPIO_getCout()==1) isLast = 1;

		delay(GPIO_MBus_HalfCycle);

		GPIO_set(FLS_CIN);
		delay(GPIO_MBus_HalfCycle);

		if (clockNotForwarded==0) {
			if (numRxBit < 8) { GPIO_RxAddr = ((GPIO_RxAddr << 1) | rxBit); }
			else if (numRxBit < 40) { GPIO_RxData0 = ((GPIO_RxData0 << 1) | rxBit); }
			else if (numRxBit < 72) { GPIO_RxData1 = ((GPIO_RxData1 << 1) | rxBit); }
			else if (numRxBit < 104) { GPIO_RxData2 = ((GPIO_RxData2 << 1) | rxBit); }
			else return 999; // Overflow
			numRxBit++;
		}

		if (isLast==1) break;
	}

	FLSMBusGPIO_interjection();

	GPIO_kill(FLS_CIN); rxBit = FLSMBusGPIO_dataFwd(); delay(GPIO_MBus_HalfCycle);
	GPIO_set(FLS_CIN); delay(GPIO_MBus_HalfCycle);

	GPIO_kill(FLS_CIN); rxBit = FLSMBusGPIO_dataFwd(); delay(GPIO_MBus_HalfCycle);
	GPIO_set(FLS_CIN); delay(GPIO_MBus_HalfCycle);

	GPIO_kill(FLS_CIN); GPIO_kill(FLS_DIN); delay(GPIO_MBus_HalfCycle); // Acknowledge
	GPIO_set(FLS_CIN); delay(GPIO_MBus_HalfCycle);

	GPIO_kill(FLS_CIN); GPIO_set(FLS_DIN); delay(GPIO_MBus_HalfCycle);
	GPIO_set(FLS_CIN); delay(GPIO_MBus_HalfCycle);

	return numRxBit;
}

//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************

int main() {
	volatile uint32_t temp_addr_0;
	volatile uint32_t temp_data_0;
	volatile uint32_t i;

	//initialize	
	if( enumerated != 0xABCDEF01 ) initialize();
	FLSMBusGPIO_initialization();

	// Enumerate FLS
	FLSMBusGPIO_send8bit (0x00, 0x24000000);
	FLSMBusGPIO_rxMsg(); // Rx Response

	// Turn on the Flash
	FLSMBusGPIO_send32bit (0x40, 0x11000003);
	FLSMBusGPIO_rxMsg();

	// Set CRT_TUNE=0x3F
	FLSMBusGPIO_send32bit (0x40, 0x0A00023F);

	for (i=0; i<40; i=i+4) {
		// Read-Out SRAM
		FLSMBusGPIO_send64bit (0x43, (i << 24), i);
		FLSMBusGPIO_rxMsg();
	
		// Store result
		temp_addr_0 = GPIO_RxAddr;
		temp_data_0 = GPIO_RxData0;
	
		// send MBus message for snooping
		write_mbus_message(0xFF, temp_addr_0);
		write_mbus_message(0xFF, temp_data_0);
	}

	// Copy Flash -> SRAM (Length = 0x7FE)
	FLSMBusGPIO_send32bit (0x40, 0x0500FFE3);
	FLSMBusGPIO_rxMsg(); // Rx Interrupt


	for (i=0; i<40; i=i+4) {
		// Read-Out SRAM
		FLSMBusGPIO_send64bit (0x43, (i << 24), i);
		FLSMBusGPIO_rxMsg();
	
		// Store result
		temp_addr_0 = GPIO_RxAddr;
		temp_data_0 = GPIO_RxData0;
	
		// send MBus message for snooping
		write_mbus_message(0xFF, temp_addr_0);
		write_mbus_message(0xFF, temp_data_0);
	}



	// Turn off the Flash
	FLSMBusGPIO_send32bit (0x40, 0x11000002);
	FLSMBusGPIO_rxMsg();

	// Make FLS go to sleep
	FLSMBusGPIO_send8bit (0x01, 0x00000000);

	// System goes to sleep
	sleep();


	while(1){
		delay(5000); //1s
	}

}
