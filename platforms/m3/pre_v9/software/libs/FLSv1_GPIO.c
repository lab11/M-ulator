//*******************************************************************
//Author: Yejoong Kim
//Description: FLSv1_GPIO Lib File
//*******************************************************************

#include "FLSv1_GPIO.h"

void FLSMBusGPIO_initialization(){
	// GPIO_DIR = 0 (output) (ex. FLS_CIN, FLS_DIN)
	//            1 (input) (ex. FLS_COUT, FLS_DOUT)
	volatile uint32_t gpio_dir = (0x00000000 | (1 << FLS_COUT) | (1 << FLS_DOUT));
	init_GPIO(gpio_dir);
	RxAddr = 0x00000000;
	RxData0 = 0x00000000;
	RxData1 = 0x00000000;
	RxData2 = 0x00000000;
}

uint32_t FLSMBusGPIO_getRxAddr() {
	return RxAddr;
}
uint32_t FLSMBusGPIO_getRxData0() {
	return RxData0;
}
uint32_t FLSMBusGPIO_getRxData1() {
	return RxData1;
}
uint32_t FLSMBusGPIO_getRxData2() {
	return RxData2;
}

void FLSMBusGPIO_resetCinDin () {
	set_GPIO_2bits(FLS_CIN, FLS_DIN);
	delay(GPIO_MBus_HalfCycle);
}

void FLSMBusGPIO_toggleCinDin (volatile uint32_t data, volatile uint32_t numBit) {
	volatile uint32_t i;
	volatile uint32_t pos = 1 << (numBit - 1);

	for (i=0; i<numBit; i++){
		kill_GPIO_bit(FLS_CIN);
		if (data & pos) set_GPIO_bit(FLS_DIN);
		else kill_GPIO_bit(FLS_DIN);
		delay(GPIO_MBus_HalfCycle);
		set_GPIO_bit(FLS_CIN);
		delay(GPIO_MBus_HalfCycle);
		pos = pos >> 1;
	}

}

void FLSMBusGPIO_interjection () {
	volatile uint32_t i;

	set_GPIO_bit(FLS_CIN);

	for (i=0; i<6; i++){
		kill_GPIO_bit(FLS_DIN);
		delay(GPIO_MBus_HalfCycle);
		set_GPIO_bit(FLS_DIN);
		delay(GPIO_MBus_HalfCycle);
	}
}

void FLSMBusGPIO_sendMBus8bit (volatile uint32_t short_prefix, volatile uint32_t data_0) {
	volatile uint32_t last_data_bit = (0x00000001 & data_0);
	volatile uint32_t tail_sequence = ((last_data_bit << 3) | 0x00000007); // EOM is always 1
	data_0 = data_0 >> (32 - 8); // Move the MSB 8bit to the right-most location.

	kill_GPIO_bit(FLS_DIN); // Pull-Down Data
	delay(GPIO_MBus_HalfCycle);
	FLSMBusGPIO_toggleCinDin (0x00000000, 3); // Arbitration Sequence
	FLSMBusGPIO_toggleCinDin (short_prefix, 8); // Short Prefix
	FLSMBusGPIO_toggleCinDin (data_0, 8); // Data
	FLSMBusGPIO_interjection (); // Interjection
	FLSMBusGPIO_toggleCinDin (tail_sequence, 4); // Tail Sequence
}

void FLSMBusGPIO_sendMBus32bit (volatile uint32_t short_prefix, volatile uint32_t data_0) {
	volatile uint32_t last_data_bit = (0x00000001 & data_0);
	volatile uint32_t tail_sequence = ((last_data_bit << 3) | 0x00000007); // EOM is always 1

	kill_GPIO_bit(FLS_DIN); // Pull-Down Data
	delay(GPIO_MBus_HalfCycle);
	FLSMBusGPIO_toggleCinDin (0x00000000, 3); // Arbitration Sequence
	FLSMBusGPIO_toggleCinDin (short_prefix, 8); // Short Prefix
	FLSMBusGPIO_toggleCinDin (data_0, 32); // Data
	FLSMBusGPIO_interjection (); // Interjection
	FLSMBusGPIO_toggleCinDin (tail_sequence, 4); // Tail Sequence
}

void FLSMBusGPIO_sendMBus64bit (volatile uint32_t short_prefix, volatile uint32_t data_0, volatile uint32_t data_1) {
	volatile uint32_t last_data_bit = (0x00000001 & data_0);
	volatile uint32_t tail_sequence = ((last_data_bit << 3) | 0x00000007); // EOM is always 1

	kill_GPIO_bit(FLS_DIN); // Pull-Down Data
	delay(GPIO_MBus_HalfCycle);
	FLSMBusGPIO_toggleCinDin (0x00000000, 3); // Arbitration Sequence
	FLSMBusGPIO_toggleCinDin (short_prefix, 8); // Short Prefix
	FLSMBusGPIO_toggleCinDin (data_0, 32); // Data_0
	FLSMBusGPIO_toggleCinDin (data_1, 32); // Data_1
	FLSMBusGPIO_interjection (); // Interjection
	FLSMBusGPIO_toggleCinDin (tail_sequence, 4); // Tail Sequence
}

void FLSMBusGPIO_sendMBus96bit (volatile uint32_t short_prefix, volatile uint32_t data_0, volatile uint32_t data_1, volatile uint32_t data_2) {
	volatile uint32_t last_data_bit = (0x00000001 & data_0);
	volatile uint32_t tail_sequence = ((last_data_bit << 3) | 0x00000007); // EOM is always 1

	kill_GPIO_bit(FLS_DIN); // Pull-Down Data
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
	volatile uint32_t data = ((get_GPIO_DATA() & mask) >> FLS_COUT);
	return data;
}

uint32_t FLSMBusGPIO_getCout () {
	volatile uint32_t mask = (1 << FLS_COUT);
	volatile uint32_t data = ((get_GPIO_DATA() & mask) >> FLS_COUT);
	return data;
}

uint32_t FLSMBusGPIO_getDout () {
	volatile uint32_t mask = (1 << FLS_DOUT);
	volatile uint32_t data = ((get_GPIO_DATA() & mask) >> FLS_DOUT);
	return data;
}

uint32_t FLSMBusGPIO_dataFwd () {
	volatile uint32_t rxBit = FLSMBusGPIO_getDout();
	if (rxBit==1) set_GPIO_bit(FLS_DIN);
	else kill_GPIO_bit(FLS_DIN);
	return rxBit;
}

uint32_t FLSMBusGPIO_rxMsg () {
	// Received Addr is stored in RxAddr
	// Received Data is stored in RxData0, RxData1, RxData2
	volatile uint32_t clockNotForwarded = 0;
	volatile uint32_t isLast = 0;
	volatile uint32_t rxBit = 0;
	volatile uint32_t numRxBit = 0;
	RxAddr = 0x00000000;

	while(1) {
		if (FLSMBusGPIO_getCoutDout()==1) {
			delay(GPIO_MBus_RxBeginDelay); // Optional Delay
			break;
		}
	}

	FLSMBusGPIO_toggleCinDin (0x00000004, 3); // Arbitration Sequence

	while(1) {
		kill_GPIO_bit(FLS_CIN);

		rxBit = FLSMBusGPIO_dataFwd();
		if ((FLSMBusGPIO_getCout()==1) & (clockNotForwarded==0)) clockNotForwarded = 1;
		else if (FLSMBusGPIO_getCout()==1) isLast = 1;

		delay(GPIO_MBus_HalfCycle);

		set_GPIO_bit(FLS_CIN);
		delay(GPIO_MBus_HalfCycle);

		if (clockNotForwarded==0) {
			if (numRxBit < 8) { RxAddr = ((RxAddr << 1) | rxBit); }
			else if (numRxBit < 40) { RxData0 = ((RxData0 << 1) | rxBit); }
			else if (numRxBit < 72) { RxData1 = ((RxData1 << 1) | rxBit); }
			else if (numRxBit < 104) { RxData2 = ((RxData2 << 1) | rxBit); }
			else return 999; // Overflow
			numRxBit++;
		}

		if (isLast==1) break;
	}

	FLSMBusGPIO_interjection();

	kill_GPIO_bit(FLS_CIN); rxBit = FLSMBusGPIO_dataFwd(); delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN); delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN); rxBit = FLSMBusGPIO_dataFwd(); delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN); delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN); kill_GPIO_bit(FLS_DIN); delay(GPIO_MBus_HalfCycle); // Acknowledge
	set_GPIO_bit(FLS_CIN); delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN); set_GPIO_bit(FLS_DIN); delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN); delay(GPIO_MBus_HalfCycle);

	return numRxBit;
}

