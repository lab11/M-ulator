//*******************************************************************
//Author: Yejoong Kim
//Description: FLSv1_GPIO Header File
//*******************************************************************

#ifndef FLSv1_GPIO_H
#define FLSv1_GPIO_H

// GPIO Pin Assignment
#define FLS_CIN  12
#define FLS_DIN  13
#define FLS_COUT 14
#define FLS_DOUT 15

// Delay
#define GPIO_MBus_QuarterCycle 1 // 10
#define GPIO_MBus_HalfCycle 2 // 20
#define GPIO_MBus_RxBeginDelay 4 // 100

#include <stdint.h>
#include <stdbool.h>
#include <PRCv9E.h>
#include <GPIO.h>

volatile uint32_t  FLSMBusGPIO_RxAddr;
volatile uint32_t  FLSMBusGPIO_RxData0;
volatile uint32_t  FLSMBusGPIO_RxData1;
volatile uint32_t  FLSMBusGPIO_RxData2;

//*******************************************************************
// Initialization
//*******************************************************************
void FLSMBusGPIO_initialization();


//*******************************************************************
// Access to Variables
//*******************************************************************

//Return the Addr part of the most recently received MBus Msg
uint32_t FLSMBusGPIO_getRxAddr();

//Return the Data part (the first 32-bit) of the most recently received MBus Msg
uint32_t FLSMBusGPIO_getRxData0();

//Return the Data part (the second 32-bit, if any) of the most recently received MBus Msg
uint32_t FLSMBusGPIO_getRxData1();

//Return the Data part (the third 32-bit, if any) of the most recently received MBus Msg
uint32_t FLSMBusGPIO_getRxData2();


//*******************************************************************
// General Commands
//*******************************************************************

//Sleep Message
void FLSMBusGPIO_sleep();

//WakeUp Message
void FLSMBusGPIO_wakeup();

//Enumeration (FLSMBusGPIO_rxMsg() must follow)
void FLSMBusGPIO_enumeration(volatile uint32_t fls_enum);

//Write Register
void FLSMBusGPIO_writeReg(volatile uint32_t short_prefix, volatile uint32_t reg_addr, volatile uint32_t reg_data);

//Read Register (FLSMBusGPIO_rxMsg() must follow)
void FLSMBusGPIO_readReg(volatile uint32_t short_prefix, volatile uint32_t reg_addr, volatile uint32_t length_1, volatile uint32_t reply_addr, volatile uint32_t dest_reg_addr);

//Write Memory (32-bit)
void FLSMBusGPIO_writeMem32(volatile uint32_t short_prefix, volatile uint32_t mem_start_addr, volatile uint32_t mem_data);

//Write Memory (64-bit)
void FLSMBusGPIO_writeMem64(volatile uint32_t short_prefix, volatile uint32_t mem_start_addr, volatile uint32_t mem_data_0, volatile uint32_t mem_data_1);

//Read Memory (FLSMBusGPIO_rxMsg() must follow)
void FLSMBusGPIO_readMem(volatile uint32_t short_prefix, volatile uint32_t reply_addr, volatile uint32_t length_1, volatile uint32_t mem_start_addr);


//*******************************************************************
// Flash-Specific Commands
//*******************************************************************

//Turn On Flash (FLSMBusGPIO_rxMsg() must follow)
//    NOTE: FLSMBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0x03.
void FLSMBusGPIO_turnOnFlash(volatile uint32_t fls_enum);

//Turn Off Flash (FLSMBusGPIO_rxMsg() must follow)
//    NOTE: FLSMBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0x06.
void FLSMBusGPIO_turnOffFlash(volatile uint32_t fls_enum);

//Enable Large Cap
//    NOTE: This must be used righat after FLSMBusGPIO_turnOnFlash()
void FLSMBusGPIO_enableLargeCap(volatile uint32_t fls_enum);

//Disable Large Cap
//    NOTE: This must be used righat after FLSMBusGPIO_turnOffFlash()
void FLSMBusGPIO_disableLargeCap(volatile uint32_t fls_enum);

//Set IRQ Address
//    - Whenever the flash layer sends out an interrupt message,
//      it will use the addresses specified by this function.
//    - The interrupt message has a form of 'Register Write' MBus message.
//    - 'short_addr' becomes the MBus address.
//    - 'reg_addr' becomes the Destination Register address.
void FLSMBusGPIO_setIRQAddr(volatile uint32_t fls_enum, volatile uint32_t short_addr, volatile uint32_t reg_addr);

//Tune settings for Flash Programming
//    - Set the following tuning bits:
//          Tprog = 0x05
//          Tcyc = 0x00C0
//          VTG_TUNE = 0x8
//          CRT_TUNE = 0x3F
void FLSMBusGPIO_setOptTune(volatile uint32_t fls_enum);

//set Flash Start Address
//    - Used for the following functions: 
//          FLSMBusGPIO_doCopyFlash2SRAM()
//          FLSMBusGPIO_doCopySRAM2Flash()
//          FLSMBusGPIO_doEraseFlash()
//    - Double-Word-Aligned: LSB is always 0
//    - 64-bit Word, 1024 Words/Page, 8kB Page, Total 256 Pages
//    - For FLSMBusGPIO_doEraseFlash(), use the first address of the page.
//    -------------------------------
//    <ADDRESS SPACE>
//    0x00000000 - 0x000007FE: Page 0
//    0x00000800 - 0x00000FFE: Page 1
//    ....
//    0x0007F800 - 0x0007FFFE: Page 255
void FLSMBusGPIO_setFlashStartAddr(volatile uint32_t fls_enum, volatile uint32_t flash_start_addr);

//set SRAM Start Address
//    - Used for the following functions: 
//          FLSMBusGPIO_doCopyFlash2SRAM()
//          FLSMBusGPIO_doCopySRAM2Flash()
//    - 32-bit Word, Total 8192 Words
//    -------------------------------
//    <ADDRESS SPACE>
//    0x00000000 - 0x00001FFF
void FLSMBusGPIO_setSRAMStartAddr(volatile uint32_t fls_enum, volatile uint32_t sram_start_addr);

//set External Stream Length
//    - Used for the following functions:
//          FLSMBusGPIO_doExtStream()
//    - length = (Number of 32-bit Words) - 2
//          MAX = 0x1FFE (8192 words = 32kB)
//    NOTE: (Number of 32-bit Words) should be an even number.
void FLSMBusGPIO_setExtStreamLength(volatile uint32_t fls_enum, volatile uint32_t length);

//Copy Flash to SRAM
//    - length = (Number of 32-bit Words) - 2
//          MAX = 0x7FE (2048 words = 8kB)
//    NOTE: (Number of 32-bit Words) should be an even number.
//    NOTE: FLSMBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0x43.
void FLSMBusGPIO_doCopyFlash2SRAM(volatile uint32_t fls_enum, volatile uint32_t length);

//Copy SRAM to Flash
//    - length = (Number of 32-bit Words) - 2
//          MAX = 0x7FE (2048 words = 8kB)
//    NOTE: (Number of 32-bit Words) should be an even number.
//    NOTE: FLSMBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0x5C.
void FLSMBusGPIO_doCopySRAM2Flash(volatile uint32_t fls_enum, volatile uint32_t length);

//Erase Flash (FLSMBusGPIO_rxMsg() must follow)
//    - Erase operation is done by page.
//    - Use FLSMBusGPIO_setFlashStartAddr() to specify the start address of the page to be erased.
//    NOTE: FLSMBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0x74.
void FLSMBusGPIO_doEraseFlash(volatile uint32_t fls_enum);

//External Input Stream (FLSMBusGPIO_rxMsg() must follow)
//    - The start address is supposed to be the very first address (ADDR[0]) of SRAM.
//      However, there is a bug so that the Flash Layer needs to be reset in order to start from ADDR[0].
//      This reset can be either 'sleep -> wakeup sequence' or 'POR sequence'.
//      If user does two separate 'stream-in' after a reset, the start address of the second stream-in 
//      might not be ADDR[0]. It's undeterministic.
//    - Length must be correctly set using FLSMBusGPIO_setExtStreamLength().
//    NOTE: FLSMBusGPIO_rxMsg() must follow this function. If correct, the interrupt payoload is 0x82.
void FLSMBusGPIO_doExtStream(volatile uint32_t fls_enum);


//*******************************************************************
// Direct MBus Control
//*******************************************************************

// Reset MBus
void FLSMBusGPIO_resetMBus ();

//Send 8-bit Addr, 8-bit Data
void FLSMBusGPIO_sendMBus8bit (volatile uint32_t short_prefix, volatile uint32_t data_0);

//Send 8-bit Addr, 32-bit Data
void FLSMBusGPIO_sendMBus32bit (volatile uint32_t short_prefix, volatile uint32_t data_0);

//Send 8-bit Addr, 64-bit Data
void FLSMBusGPIO_sendMBus64bit (volatile uint32_t short_prefix, volatile uint32_t data_0, volatile uint32_t data_1);

//Send 8-bit Addr, 96-bit Data
void FLSMBusGPIO_sendMBus96bit (volatile uint32_t short_prefix, volatile uint32_t data_0, volatile uint32_t data_1, volatile uint32_t data_2);

//Receive an incoming MBus Msg
uint32_t FLSMBusGPIO_rxMsg ();

// Force Stop
void FLSMBusGPIO_forceStop ();

//Send 8-bit Addr, 31-bit Data: This is supposed to be used for flash streaming 'go' signal
//LSB in data_0 will NOT be sent.
void FLSMBusGPIO_sendMBus31bit (volatile uint32_t short_prefix, volatile uint32_t data_0);

//Send the Last 1-bit Data: This is supposed to be used for flash streaming 'go' signal
//Only LSB in data_0 will be sent as well as the tail sequence.
void FLSMBusGPIO_sendMBusLast1bit (volatile uint32_t short_prefix, volatile uint32_t data_0);


//*******************************************************************
// Sub Functions
//*******************************************************************
void FLSMBusGPIO_resetCinDin ();
void FLSMBusGPIO_toggleCinDin (volatile uint32_t data, volatile uint32_t numBit);
void FLSMBusGPIO_interjection ();
void FLSMBusGPIO_controlAck ();
uint32_t FLSMBusGPIO_getCoutDout ();
uint32_t FLSMBusGPIO_getCout ();
uint32_t FLSMBusGPIO_getDout ();
uint32_t FLSMBusGPIO_dataFwd ();

#endif // FLSv1_GPIO_H

