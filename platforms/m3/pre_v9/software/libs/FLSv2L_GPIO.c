//*******************************************************************
//Author: Yejoong Kim
//Description: FLSv2L_GPIO Lib File
//*******************************************************************

#include "FLSv2L_GPIO.h"


void FLSv2MBusGPIO_initialization(){
	// GPIO_DIR = 0 (output) (ex. FLS_CIN, FLS_DIN)
	//            1 (input) (ex. FLS_COUT, FLS_DOUT)
	volatile uint32_t gpio_dir = (0x00000000 | (1 << FLS_COUT) | (1 << FLS_DOUT));
	init_GPIO(gpio_dir);
	FLSv2MBusGPIO_RxAddr = 0x00000000;
	FLSv2MBusGPIO_RxData0 = 0x00000000;
	FLSv2MBusGPIO_RxData1 = 0x00000000;
	FLSv2MBusGPIO_RxData2 = 0x00000000;
	// Reset CIN & DIN to 1
	set_GPIO_bit(FLS_DIN);
	set_GPIO_bit(FLS_CIN);
	// There should be 2 reset sequences in series.
	// Not sure why, but it should be related to the fact that
	// CIN and DIN raise up almost at the same time, which is not the normal operation scenario.
	// This has been verified with Auto-Boot Disabled. 
	// It might be different with Auto-Boot Enabled.
	delay(10);
	FLSv2MBusGPIO_interjection();
	FLSv2MBusGPIO_controlAck();
	delay(10);
	delay(10);
	FLSv2MBusGPIO_interjection();
	FLSv2MBusGPIO_controlAck();
	delay(10);
}
uint32_t FLSv2MBusGPIO_getRxAddr() {
	return FLSv2MBusGPIO_RxAddr;
}
uint32_t FLSv2MBusGPIO_getRxData0() {
	return FLSv2MBusGPIO_RxData0;
}
uint32_t FLSv2MBusGPIO_getRxData1() {
	return FLSv2MBusGPIO_RxData1;
}
uint32_t FLSv2MBusGPIO_getRxData2() {
	return FLSv2MBusGPIO_RxData2;
}
void FLSv2MBusGPIO_sleep() {
	FLSv2MBusGPIO_sendMBus8bit (0x01, 0x00000000);
}
void FLSv2MBusGPIO_wakeup() {
	FLSv2MBusGPIO_sendMBus8bit (0x01, 0x10000000);
}
void FLSv2MBusGPIO_enumeration(volatile uint32_t fls_enum) {
	FLSv2MBusGPIO_sendMBus8bit (0x00, (0x20000000 | (fls_enum << 24)));
}
void FLSv2MBusGPIO_writeReg(volatile uint32_t short_prefix, volatile uint32_t reg_addr, volatile uint32_t reg_data) {
	FLSv2MBusGPIO_sendMBus32bit ((short_prefix << 4), ((reg_addr << 24) | reg_data));
}
void FLSv2MBusGPIO_readReg(volatile uint32_t short_prefix, volatile uint32_t reg_addr, volatile uint32_t length_1, volatile uint32_t reply_addr, volatile uint32_t dest_reg_addr) {
	FLSv2MBusGPIO_sendMBus32bit (((short_prefix << 4) | 0x1), ((reg_addr << 24) | (length_1 << 16) | (reply_addr << 8) | dest_reg_addr));
}
void FLSv2MBusGPIO_writeMem32(volatile uint32_t short_prefix, volatile uint32_t mem_start_addr, volatile uint32_t mem_data) {
	FLSv2MBusGPIO_sendMBus64bit (((short_prefix << 4) | 0x2), mem_start_addr, mem_data);
}
void FLSv2MBusGPIO_writeMem64(volatile uint32_t short_prefix, volatile uint32_t mem_start_addr, volatile uint32_t mem_data_0, volatile uint32_t mem_data_1) {
	FLSv2MBusGPIO_sendMBus96bit (((short_prefix << 4) | 0x2), mem_start_addr, mem_data_0, mem_data_1);
}
void FLSv2MBusGPIO_readMem(volatile uint32_t short_prefix, volatile uint32_t reply_addr, volatile uint32_t length_1, volatile uint32_t mem_start_addr) {
	FLSv2MBusGPIO_sendMBus64bit (((short_prefix << 4) | 0x3), ((reply_addr << 24) | length_1), mem_start_addr);
}
void FLSv2MBusGPIO_turnOn(volatile uint32_t fls_enum) {
    //      FLASH_POWER_DO_VREFCOMP  = 1
    //      FLASH_POWER_DO_LARGE_CAP = 1
    //      FLASH_POWER_DO_FLSH      = 1
    //      FLASH_POWER_IRQ_EN       = 1
    //      FLASH_POWER_SEL_ON       = 1
    //      FLASH_POWER_GO           = 1
	FLSv2MBusGPIO_writeReg(fls_enum, 0x11, 0x00003F);
}
void FLSv2MBusGPIO_turnOff(volatile uint32_t fls_enum) {
    //      FLASH_POWER_DO_VREFCOMP  = 1
    //      FLASH_POWER_DO_LARGE_CAP = 1
    //      FLASH_POWER_DO_FLSH      = 1
    //      FLASH_POWER_IRQ_EN       = 1
    //      FLASH_POWER_SEL_ON       = 0
    //      FLASH_POWER_GO           = 1
	FLSv2MBusGPIO_writeReg(fls_enum, 0x11, 0x00003D);
}
void FLSv2MBusGPIO_turnOnFlash(volatile uint32_t fls_enum) {
    //      FLASH_POWER_DO_VREFCOMP  = 1
    //      FLASH_POWER_DO_LARGE_CAP = 0
    //      FLASH_POWER_DO_FLSH      = 1
    //      FLASH_POWER_IRQ_EN       = 1
    //      FLASH_POWER_SEL_ON       = 1
    //      FLASH_POWER_GO           = 1
	FLSv2MBusGPIO_writeReg(fls_enum, 0x11, 0x00002F);
}
void FLSv2MBusGPIO_turnOffFlash(volatile uint32_t fls_enum) {
    //      FLASH_POWER_DO_VREFCOMP  = 1
    //      FLASH_POWER_DO_LARGE_CAP = 0
    //      FLASH_POWER_DO_FLSH      = 1
    //      FLASH_POWER_IRQ_EN       = 1
    //      FLASH_POWER_SEL_ON       = 0
    //      FLASH_POWER_GO           = 1
	FLSv2MBusGPIO_writeReg(fls_enum, 0x11, 0x00002D);
}
void FLSv2MBusGPIO_enableLargeCap(volatile uint32_t fls_enum) {
    //      FLASH_POWER_DO_VREFCOMP  = 0
    //      FLASH_POWER_DO_LARGE_CAP = 1
    //      FLASH_POWER_DO_FLSH      = 0
    //      FLASH_POWER_IRQ_EN       = 1
    //      FLASH_POWER_SEL_ON       = 1
    //      FLASH_POWER_GO           = 1
	FLSv2MBusGPIO_writeReg(fls_enum, 0x11, 0x000017);
}
void FLSv2MBusGPIO_disableLargeCap(volatile uint32_t fls_enum) {
    //      FLASH_POWER_DO_VREFCOMP  = 0
    //      FLASH_POWER_DO_LARGE_CAP = 1
    //      FLASH_POWER_DO_FLSH      = 0
    //      FLASH_POWER_IRQ_EN       = 1
    //      FLASH_POWER_SEL_ON       = 0
    //      FLASH_POWER_GO           = 1
	FLSv2MBusGPIO_writeReg(fls_enum, 0x11, 0x000015);
}
void FLSv2MBusGPIO_setIRQAddr(volatile uint32_t fls_enum, volatile uint32_t short_addr, volatile uint32_t reg_addr) {
	FLSv2MBusGPIO_writeReg(fls_enum, 0x0F, ((short_addr << 8) | (reg_addr << 0 )));
}
void FLSv2MBusGPIO_setFlashStartAddr(volatile uint32_t fls_enum, volatile uint32_t flash_start_addr) {
	FLSv2MBusGPIO_writeReg(fls_enum, 0x09, flash_start_addr);
}
void FLSv2MBusGPIO_setSRAMStartAddr(volatile uint32_t fls_enum, volatile uint32_t sram_start_addr) {
	FLSv2MBusGPIO_writeReg(fls_enum, 0x08, sram_start_addr);
}
void FLSv2MBusGPIO_configExtStream(volatile uint32_t fls_enum, volatile uint32_t bit_en, volatile uint32_t sram_start_addr, volatile uint32_t timeout) {
	FLSv2MBusGPIO_writeReg(fls_enum, 0x0C, (bit_en << 0));
	FLSv2MBusGPIO_writeReg(fls_enum, 0x0D, (timeout << 0));
	FLSv2MBusGPIO_writeReg(fls_enum, 0x0E, (sram_start_addr << 0));
}
void FLSv2MBusGPIO_doCopyFlash2SRAM(volatile uint32_t fls_enum, volatile uint32_t length) {
	FLSv2MBusGPIO_writeReg(fls_enum, 0x07, ((length << 6) | (0x1 << 5) | (0x1 << 1) | (0x1 << 0)));
}
void FLSv2MBusGPIO_doCopySRAM2Flash(volatile uint32_t fls_enum, volatile uint32_t length) {
	FLSv2MBusGPIO_writeReg(fls_enum, 0x07, ((length << 6) | (0x1 << 5) | (0x2 << 1) | (0x1 << 0)));
}
void FLSv2MBusGPIO_doEraseFlash(volatile uint32_t fls_enum) {
	FLSv2MBusGPIO_writeReg(fls_enum, 0x07, ((0x0 << 6) | (0x1 << 5) | (0x4 << 1) | (0x1 << 0)));
}
void FLSv2MBusGPIO_doExtStream(volatile uint32_t fls_enum, volatile uint32_t irq_en, volatile uint32_t length) {
	FLSv2MBusGPIO_writeReg(fls_enum, 0x07, ((length << 6) | (irq_en << 5) | (0x6 << 1) | (0x1 << 0)));
}
void FLSv2MBusGPIO_forceReset(volatile uint32_t fls_enum) {
	FLSv2MBusGPIO_writeReg(fls_enum, 0x1F, (0x0 << 0));
	FLSv2MBusGPIO_writeReg(fls_enum, 0x1F, (0x0 << 1));
}
void FLSv2MBusGPIO_resetCinDin () {
	set_GPIO_2bits(FLS_CIN, FLS_DIN);
	//delay(GPIO_MBus_HalfCycle);
}

void FLSv2MBusGPIO_toggleCinDin (volatile uint32_t data, volatile uint32_t numBit) {
	volatile uint32_t i;
	volatile uint32_t pos = 1 << (numBit - 1);

	for (i=0; i<numBit; i++){
		kill_GPIO_bit(FLS_CIN);
		if (data & pos) set_GPIO_bit(FLS_DIN);
		else kill_GPIO_bit(FLS_DIN);
		//delay(GPIO_MBus_HalfCycle);
		set_GPIO_bit(FLS_CIN);
		//delay(GPIO_MBus_HalfCycle);
		pos = pos >> 1;
	}

}

void FLSv2MBusGPIO_interjection () {
	volatile uint32_t i;

	set_GPIO_bit(FLS_CIN);

	for (i=0; i<6; i++){
		kill_GPIO_bit(FLS_DIN);
		//delay(GPIO_MBus_HalfCycle);
		//delay(GPIO_MBus_HalfCycle);
  		set_GPIO_bit(FLS_DIN);
		//delay(GPIO_MBus_HalfCycle);
		//delay(GPIO_MBus_HalfCycle);
	}
}

void FLSv2MBusGPIO_sendMBus8bit (volatile uint32_t short_prefix, volatile uint32_t data_0) {
	volatile uint32_t last_data_bit = (0x00000001 & data_0);
	volatile uint32_t tail_sequence = ((last_data_bit << 3) | 0x00000007); // EOM is always 1
	data_0 = data_0 >> (32 - 8); // Move the MSB 8bit to the right-most location.

	kill_GPIO_bit(FLS_DIN); // Pull-Down Data
	//delay(GPIO_MBus_HalfCycle);
	FLSv2MBusGPIO_toggleCinDin (0x00000000, 3); // Arbitration Sequence
	FLSv2MBusGPIO_toggleCinDin (short_prefix, 8); // Short Prefix
	FLSv2MBusGPIO_toggleCinDin (data_0, 8); // Data
	FLSv2MBusGPIO_interjection (); // Interjection
	FLSv2MBusGPIO_toggleCinDin (tail_sequence, 4); // Tail Sequence
}

void FLSv2MBusGPIO_sendMBus32bit (volatile uint32_t short_prefix, volatile uint32_t data_0) {
	volatile uint32_t last_data_bit = (0x00000001 & data_0);
	volatile uint32_t tail_sequence = ((last_data_bit << 3) | 0x00000007); // EOM is always 1

	kill_GPIO_bit(FLS_DIN); // Pull-Down Data
	//delay(GPIO_MBus_HalfCycle);
	FLSv2MBusGPIO_toggleCinDin (0x00000000, 3); // Arbitration Sequence
	FLSv2MBusGPIO_toggleCinDin (short_prefix, 8); // Short Prefix
	FLSv2MBusGPIO_toggleCinDin (data_0, 32); // Data
	FLSv2MBusGPIO_interjection (); // Interjection
	FLSv2MBusGPIO_toggleCinDin (tail_sequence, 4); // Tail Sequence
}

void FLSv2MBusGPIO_sendMBus64bit (volatile uint32_t short_prefix, volatile uint32_t data_0, volatile uint32_t data_1) {
	volatile uint32_t last_data_bit = (0x00000001 & data_0);
	volatile uint32_t tail_sequence = ((last_data_bit << 3) | 0x00000007); // EOM is always 1

	kill_GPIO_bit(FLS_DIN); // Pull-Down Data
	//delay(GPIO_MBus_HalfCycle);
	FLSv2MBusGPIO_toggleCinDin (0x00000000, 3); // Arbitration Sequence
	FLSv2MBusGPIO_toggleCinDin (short_prefix, 8); // Short Prefix
	FLSv2MBusGPIO_toggleCinDin (data_0, 32); // Data_0
	FLSv2MBusGPIO_toggleCinDin (data_1, 32); // Data_1
	FLSv2MBusGPIO_interjection (); // Interjection
	FLSv2MBusGPIO_toggleCinDin (tail_sequence, 4); // Tail Sequence
}

void FLSv2MBusGPIO_sendMBus96bit (volatile uint32_t short_prefix, volatile uint32_t data_0, volatile uint32_t data_1, volatile uint32_t data_2) {
	volatile uint32_t last_data_bit = (0x00000001 & data_0);
	volatile uint32_t tail_sequence = ((last_data_bit << 3) | 0x00000007); // EOM is always 1

	kill_GPIO_bit(FLS_DIN); // Pull-Down Data
	//delay(GPIO_MBus_HalfCycle);
	FLSv2MBusGPIO_toggleCinDin (0x00000000, 3); // Arbitration Sequence
	FLSv2MBusGPIO_toggleCinDin (short_prefix, 8); // Short Prefix
	FLSv2MBusGPIO_toggleCinDin (data_0, 32); // Data_0
	FLSv2MBusGPIO_toggleCinDin (data_1, 32); // Data_1
	FLSv2MBusGPIO_toggleCinDin (data_2, 32); // Data_2
	FLSv2MBusGPIO_interjection (); // Interjection
	FLSv2MBusGPIO_toggleCinDin (tail_sequence, 4); // Tail Sequence
}

uint32_t FLSv2MBusGPIO_getCoutDout () {
	// If DOUT = 0, COUT = 0: Return 0
	// If DOUT = 0, COUT = 1: Return 1
	// If DOUT = 1, COUT = 0: Return 2
	// If DOUT = 1, COUT = 1: Return 3
	volatile uint32_t mask = ((1 << FLS_COUT) | (1 << FLS_DOUT));
	volatile uint32_t data = ((get_GPIO_DATA() & mask) >> FLS_COUT);
	return data;
}

uint32_t FLSv2MBusGPIO_getCout () {
	volatile uint32_t mask = (1 << FLS_COUT);
	volatile uint32_t data = ((get_GPIO_DATA() & mask) >> FLS_COUT);
	return data;
}

uint32_t FLSv2MBusGPIO_getDout () {
	volatile uint32_t mask = (1 << FLS_DOUT);
	volatile uint32_t data = ((get_GPIO_DATA() & mask) >> FLS_DOUT);
	return data;
}

uint32_t FLSv2MBusGPIO_dataFwd () {
	volatile uint32_t rxBit = FLSv2MBusGPIO_getDout();
	if (rxBit==1) set_GPIO_bit(FLS_DIN);
	else kill_GPIO_bit(FLS_DIN);
	return rxBit;
}

void FLSv2MBusGPIO_resetMBus () {
	volatile uint32_t numToggles = 6;
	volatile uint32_t i;

	kill_GPIO_bit(FLS_DIN);
	set_GPIO_bit(FLS_CIN);

	for (i=0; i<numToggles; i++) {
		//delay(GPIO_MBus_HalfCycle);
		//delay(GPIO_MBus_HalfCycle);
		kill_GPIO_bit(FLS_DIN);
		//delay(GPIO_MBus_HalfCycle);
		//delay(GPIO_MBus_HalfCycle);
		set_GPIO_bit(FLS_DIN);
	}
	//delay(GPIO_MBus_HalfCycle);
	//delay(GPIO_MBus_HalfCycle);
}

uint32_t FLSv2MBusGPIO_rxMsg () {
	// Received Addr is stored in FLSv2MBusGPIO_RxAddr
	// Received Data is stored in FLSv2MBusGPIO_RxData0, FLSv2MBusGPIO_RxData1, FLSv2MBusGPIO_RxData2
	volatile uint32_t clockNotForwarded = 0;
	volatile uint32_t isLast = 0;
	volatile uint32_t rxBit = 0;
	volatile uint32_t numRxBit = 0;
	FLSv2MBusGPIO_RxAddr = 0x00000000;
	FLSv2MBusGPIO_RxData0 = 0x00000000;
	FLSv2MBusGPIO_RxData1 = 0x00000000;
	FLSv2MBusGPIO_RxData2 = 0x00000000;

	while(1) {
		if (FLSv2MBusGPIO_getCoutDout()==1) {
			//delay(GPIO_MBus_RxBeginDelay); // Optional Delay
			break;
		}
	}

	// Arbitration Sequence
	//	FLSv2MBusGPIO_toggleCinDin (0x00000004, 3); // Arbitration Sequence (OLD)
	kill_GPIO_bit(FLS_CIN); 
	set_GPIO_bit(FLS_DIN); 
	//delay(GPIO_MBus_QuarterCycle); 
	rxBit = ((rxBit << 1) | FLSv2MBusGPIO_getDout()); 
	//delay(GPIO_MBus_QuarterCycle);
	set_GPIO_bit(FLS_CIN); 
	//delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN); 
	//delay(GPIO_MBus_QuarterCycle); 
	rxBit = ((rxBit << 1) | FLSv2MBusGPIO_dataFwd()); 
	//delay(GPIO_MBus_QuarterCycle);
	set_GPIO_bit(FLS_CIN); 
	//delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN); 
	//delay(GPIO_MBus_QuarterCycle); 
	rxBit = ((rxBit << 1) | FLSv2MBusGPIO_dataFwd()); 
	//delay(GPIO_MBus_QuarterCycle);
	set_GPIO_bit(FLS_CIN); 
	//delay(GPIO_MBus_HalfCycle);

	// Main Part
	if (rxBit == 0x00000007) { // If Data remains 1 during the Arbitration Sequence
		kill_GPIO_bit(FLS_CIN); 
		//delay(GPIO_MBus_QuarterCycle); 
		rxBit = FLSv2MBusGPIO_dataFwd(); 
		//delay(GPIO_MBus_QuarterCycle);
		set_GPIO_bit(FLS_CIN); 
		//delay(GPIO_MBus_HalfCycle);
	}
	else { // If Data remains 0 during the Arbitration Sequence (this is normal, since there's no other layer involved)
		while(1) {
			kill_GPIO_bit(FLS_CIN);
			//delay(GPIO_MBus_QuarterCycle);

			rxBit = FLSv2MBusGPIO_dataFwd();
			if ((FLSv2MBusGPIO_getCout()==1) & (clockNotForwarded==0)) clockNotForwarded = 1;
			else if (FLSv2MBusGPIO_getCout()==1) isLast = 1;

			//delay(GPIO_MBus_QuarterCycle);

			set_GPIO_bit(FLS_CIN);
			//delay(GPIO_MBus_HalfCycle);

			if (FLSv2MBusGPIO_getCout()==0) return 9999;

			if (clockNotForwarded==0) {
				if (numRxBit < 8) { FLSv2MBusGPIO_RxAddr = ((FLSv2MBusGPIO_RxAddr << 1) | rxBit); }
				else if (numRxBit < 40) { FLSv2MBusGPIO_RxData0 = ((FLSv2MBusGPIO_RxData0 << 1) | rxBit); }
				else if (numRxBit < 72) { FLSv2MBusGPIO_RxData1 = ((FLSv2MBusGPIO_RxData1 << 1) | rxBit); }
				else if (numRxBit < 104) { FLSv2MBusGPIO_RxData2 = ((FLSv2MBusGPIO_RxData2 << 1) | rxBit); }
				else return 999; // Overflow
				numRxBit++;
			}

			if (isLast==1) break;
		}
	}

	// Interjection
	FLSv2MBusGPIO_interjection();

	// Control Bits
	kill_GPIO_bit(FLS_CIN); 
	rxBit = FLSv2MBusGPIO_dataFwd(); 
	//delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN); 
	//delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN); 
	rxBit = FLSv2MBusGPIO_dataFwd(); 
	//delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN); 
	//delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN); 
	kill_GPIO_bit(FLS_DIN); 
	//delay(GPIO_MBus_HalfCycle); // Acknowledge
	set_GPIO_bit(FLS_CIN); 
	//delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN); 
	set_GPIO_bit(FLS_DIN); 
	//delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN); 
	//delay(GPIO_MBus_HalfCycle);

	// Return the number of received bits
	return numRxBit;
}

void FLSv2MBusGPIO_forceStop () {
	volatile uint32_t rxBit;

	FLSv2MBusGPIO_interjection();

	kill_GPIO_bit(FLS_CIN);
	rxBit = FLSv2MBusGPIO_dataFwd();
	//delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN);
	//delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN);
	rxBit = FLSv2MBusGPIO_dataFwd();
	//delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN);
	//delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN);
	kill_GPIO_bit(FLS_DIN);
	//delay(GPIO_MBus_HalfCycle); // Acknowledge
	set_GPIO_bit(FLS_CIN);
	//delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN);
	set_GPIO_bit(FLS_DIN);
	//delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN);
	//delay(GPIO_MBus_HalfCycle);
}

void FLSv2MBusGPIO_controlAck () {
	volatile uint32_t rxBit;

	// Control Bits
	kill_GPIO_bit(FLS_CIN);
	rxBit = FLSv2MBusGPIO_dataFwd();
	//delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN);
	//delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN);
	rxBit = FLSv2MBusGPIO_dataFwd();
	//delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN);
	//delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN);
	kill_GPIO_bit(FLS_DIN);
	//delay(GPIO_MBus_HalfCycle); // Acknowledge
	set_GPIO_bit(FLS_CIN);
	//delay(GPIO_MBus_HalfCycle);

	kill_GPIO_bit(FLS_CIN);
	set_GPIO_bit(FLS_DIN);
	//delay(GPIO_MBus_HalfCycle);
	set_GPIO_bit(FLS_CIN);
	//delay(GPIO_MBus_HalfCycle);
}

void FLSv2MBusGPIO_sendMBus31bit (volatile uint32_t short_prefix, volatile uint32_t data_0) {
	data_0 = data_0 >> 1; // Drop the LSB
	kill_GPIO_bit(FLS_DIN); // Pull-Down Data
	//delay(GPIO_MBus_HalfCycle);
	FLSv2MBusGPIO_toggleCinDin (0x00000000, 3); // Arbitration Sequence
	FLSv2MBusGPIO_toggleCinDin (short_prefix, 8); // Short Prefix
	FLSv2MBusGPIO_toggleCinDin (data_0, 31); // Data
}

void FLSv2MBusGPIO_sendMBusLast1bit (volatile uint32_t short_prefix, volatile uint32_t data_0) {
	volatile uint32_t last_data_bit = (0x00000001 & data_0);
	volatile uint32_t tail_sequence = ((last_data_bit << 3) | 0x00000007); // EOM is always 1

	FLSv2MBusGPIO_toggleCinDin (last_data_bit, 1); // Data
	FLSv2MBusGPIO_interjection (); // Interjection
	FLSv2MBusGPIO_toggleCinDin (tail_sequence, 4); // Tail Sequence
}

