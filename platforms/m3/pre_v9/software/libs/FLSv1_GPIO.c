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
	FLSMBusGPIO_RxAddr = 0x00000000;
	FLSMBusGPIO_RxData0 = 0x00000000;
	FLSMBusGPIO_RxData1 = 0x00000000;
	FLSMBusGPIO_RxData2 = 0x00000000;
}
uint32_t FLSMBusGPIO_getRxAddr() {
	return FLSMBusGPIO_RxAddr;
}
uint32_t FLSMBusGPIO_getRxData0() {
	return FLSMBusGPIO_RxData0;
}
uint32_t FLSMBusGPIO_getRxData1() {
	return FLSMBusGPIO_RxData1;
}
uint32_t FLSMBusGPIO_getRxData2() {
	return FLSMBusGPIO_RxData2;
}
void FLSMBusGPIO_sleep() {
	FLSMBusGPIO_sendMBus8bit (0x01, 0x00000000);
}
void FLSMBusGPIO_enumeration(volatile uint32_t fls_enum) {
	FLSMBusGPIO_sendMBus8bit (0x00, (0x20000000 | (fls_enum << 24)));
}
void FLSMBusGPIO_writeReg(volatile uint32_t short_prefix, volatile uint32_t reg_addr, volatile uint32_t reg_data) {
	FLSMBusGPIO_sendMBus32bit ((short_prefix << 4), ((reg_addr << 24) | reg_data));
}
void FLSMBusGPIO_readReg(volatile uint32_t short_prefix, volatile uint32_t reg_addr, volatile uint32_t length_1, volatile uint32_t reply_addr, volatile uint32_t dest_reg_addr) {
	FLSMBusGPIO_sendMBus32bit (((short_prefix << 4) | 0x1), ((reg_addr << 24) | (length_1 << 16) | (reply_addr << 8) | dest_reg_addr));
}
void FLSMBusGPIO_writeMem32(volatile uint32_t short_prefix, volatile uint32_t mem_start_addr, volatile uint32_t mem_data) {
	FLSMBusGPIO_sendMBus64bit (((short_prefix << 4) | 0x2), mem_start_addr, mem_data);
}
void FLSMBusGPIO_writeMem64(volatile uint32_t short_prefix, volatile uint32_t mem_start_addr, volatile uint32_t mem_data_0, volatile uint32_t mem_data_1) {
	FLSMBusGPIO_sendMBus96bit (((short_prefix << 4) | 0x2), mem_start_addr, mem_data_0, mem_data_1);
}
void FLSMBusGPIO_readMem(volatile uint32_t short_prefix, volatile uint32_t reply_addr, volatile uint32_t length_1, volatile uint32_t mem_start_addr) {
	FLSMBusGPIO_sendMBus64bit (((short_prefix << 4) | 0x3), ((reply_addr << 24) | length_1), mem_start_addr);
}
void FLSMBusGPIO_turnOnFlash(volatile uint32_t fls_enum) {
	FLSMBusGPIO_writeReg(fls_enum, 0x11, 0x000003);
	FLSMBusGPIO_writeReg(fls_enum, 0x09, 0x0000003F);
}
void FLSMBusGPIO_turnOffFlash(volatile uint32_t fls_enum) {
	FLSMBusGPIO_writeReg(fls_enum, 0x11, 0x000002);
	FLSMBusGPIO_writeReg(fls_enum, 0x09, 0x00000000);
}
void FLSMBusGPIO_setIRQAddr(volatile uint32_t fls_enum, volatile uint32_t short_addr, volatile uint32_t reg_addr) {
	FLSMBusGPIO_writeReg(fls_enum, 0x0C, ((short_addr << 8) | (reg_addr << 0 )));
}
void FLSMBusGPIO_setOptTune(volatile uint32_t fls_enum) {
	// Set Tprog=0x5, Tcyc=0x00C0 
	FLSMBusGPIO_writeReg(fls_enum, 0x02, ((0x5 << 16) | (0x00C0 << 0 )));
	// Set VTG_TUNE = 0x8, CRT_TUNE=0x3F 
	FLSMBusGPIO_writeReg(fls_enum, 0x0A, ((0x8 << 6) | (0x3F << 0 )));
}
void FLSMBusGPIO_setFlashStartAddr(volatile uint32_t fls_enum, volatile uint32_t flash_start_addr) {
	FLSMBusGPIO_writeReg(fls_enum, 0x07, flash_start_addr);
}
void FLSMBusGPIO_setSRAMStartAddr(volatile uint32_t fls_enum, volatile uint32_t sram_start_addr) {
	FLSMBusGPIO_writeReg(fls_enum, 0x07, sram_start_addr);
}
void FLSMBusGPIO_setExtStreamLength(volatile uint32_t fls_enum, volatile uint32_t length) {
	FLSMBusGPIO_writeReg(fls_enum, 0x03, (length << 0));
}
void FLSMBusGPIO_doCopyFlash2SRAM(volatile uint32_t fls_enum, volatile uint32_t length) {
	FLSMBusGPIO_writeReg(fls_enum, 0x05, ((0x1 << 15) | (length << 4) | (0x1 << 1) | (0x1 << 0)));
}
void FLSMBusGPIO_doCopySRAM2Flash(volatile uint32_t fls_enum, volatile uint32_t length) {
	FLSMBusGPIO_writeReg(fls_enum, 0x05, ((0x1 << 15) | (length << 4) | (0x2 << 1) | (0x1 << 0)));
}
void FLSMBusGPIO_doEraseFlash(volatile uint32_t fls_enum) {
	FLSMBusGPIO_writeReg(fls_enum, 0x05, ((0x1 << 15) | (0x4 << 1) | (0x1 << 0)));
}
void FLSMBusGPIO_doExtStream(volatile uint32_t fls_enum) {
	FLSMBusGPIO_writeReg(fls_enum, 0x05, ((0x1 << 15) | (0x6 << 1) | (0x1 << 0)));
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

void FLSMBusGPIO_resetMBus () {
	volatile uint32_t numToggles = 6;
	volatile uint32_t i;

	kill_GPIO_bit(FLS_DIN);
	set_GPIO_bit(FLS_CIN);

	for (i=0; i<numToggles; i++) {
		delay(GPIO_MBus_HalfCycle);
		delay(GPIO_MBus_HalfCycle);
		kill_GPIO_bit(FLS_DIN);
		delay(GPIO_MBus_HalfCycle);
		delay(GPIO_MBus_HalfCycle);
		set_GPIO_bit(FLS_DIN);
	}
	delay(GPIO_MBus_HalfCycle);
	delay(GPIO_MBus_HalfCycle);
}

uint32_t FLSMBusGPIO_rxMsg () {
	// Received Addr is stored in FLSMBusGPIO_RxAddr
	// Received Data is stored in FLSMBusGPIO_RxData0, FLSMBusGPIO_RxData1, FLSMBusGPIO_RxData2
	volatile uint32_t clockNotForwarded = 0;
	volatile uint32_t isLast = 0;
	volatile uint32_t rxBit = 0;
	volatile uint32_t numRxBit = 0;
	FLSMBusGPIO_RxAddr = 0x00000000;
	FLSMBusGPIO_RxData0 = 0x00000000;
	FLSMBusGPIO_RxData1 = 0x00000000;
	FLSMBusGPIO_RxData2 = 0x00000000;

	//write_mbus_message(0xEE, 0x44444444);
	while(1) {
		if (FLSMBusGPIO_getCoutDout()==1) {
			delay(GPIO_MBus_RxBeginDelay); // Optional Delay
			break;
		}
	}
	//write_mbus_message(0xEE, 0x55555555);

	FLSMBusGPIO_toggleCinDin (0x00000004, 3); // Arbitration Sequence

	while(1) {
		kill_GPIO_bit(FLS_CIN);

		rxBit = FLSMBusGPIO_dataFwd();
		if ((FLSMBusGPIO_getCout()==1) & (clockNotForwarded==0)) clockNotForwarded = 1;
		else if (FLSMBusGPIO_getCout()==1) isLast = 1;

		delay(GPIO_MBus_HalfCycle);

		set_GPIO_bit(FLS_CIN);
		delay(GPIO_MBus_HalfCycle);

		if (FLSMBusGPIO_getCout()==0) return 9999;

		if (clockNotForwarded==0) {
			if (numRxBit < 8) { FLSMBusGPIO_RxAddr = ((FLSMBusGPIO_RxAddr << 1) | rxBit); }
			else if (numRxBit < 40) { FLSMBusGPIO_RxData0 = ((FLSMBusGPIO_RxData0 << 1) | rxBit); }
			else if (numRxBit < 72) { FLSMBusGPIO_RxData1 = ((FLSMBusGPIO_RxData1 << 1) | rxBit); }
			else if (numRxBit < 104) { FLSMBusGPIO_RxData2 = ((FLSMBusGPIO_RxData2 << 1) | rxBit); }
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

void FLSMBusGPIO_forceStop () {
	volatile uint32_t rxBit;

	FLSMBusGPIO_interjection();

	kill_GPIO_bit(FLS_CIN); rxBit = FLSMBusGPIO_dataFwd(); delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN); delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN); rxBit = FLSMBusGPIO_dataFwd(); delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN); delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN); kill_GPIO_bit(FLS_DIN); delay(GPIO_MBus_HalfCycle); // Acknowledge
	set_GPIO_bit(FLS_CIN); delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN); set_GPIO_bit(FLS_DIN); delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN); delay(GPIO_MBus_HalfCycle);
}
