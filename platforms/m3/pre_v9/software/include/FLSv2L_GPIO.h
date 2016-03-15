//*******************************************************************
//Author: Yejoong Kim
//Description: FLSv2L_GPIO Header File
//*******************************************************************

#ifndef FLSv2L_GPIO_H
#define FLSv2L_GPIO_H

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

volatile uint32_t  FLSv2MBusGPIO_RxAddr;
volatile uint32_t  FLSv2MBusGPIO_RxData0;
volatile uint32_t  FLSv2MBusGPIO_RxData1;
volatile uint32_t  FLSv2MBusGPIO_RxData2;

//*******************************************************************
// Initialization
//*******************************************************************
void FLSv2MBusGPIO_initialization();


//*******************************************************************
// Access to Variables
//*******************************************************************

//Return the Addr part of the most recently received MBus Msg
uint32_t FLSv2MBusGPIO_getRxAddr();

//Return the Data part (the first 32-bit) of the most recently received MBus Msg
uint32_t FLSv2MBusGPIO_getRxData0();

//Return the Data part (the second 32-bit, if any) of the most recently received MBus Msg
uint32_t FLSv2MBusGPIO_getRxData1();

//Return the Data part (the third 32-bit, if any) of the most recently received MBus Msg
uint32_t FLSv2MBusGPIO_getRxData2();


//*******************************************************************
// General Commands
//*******************************************************************

//Sleep Message
void FLSv2MBusGPIO_sleep();

//WakeUp Message
void FLSv2MBusGPIO_wakeup();

//Enumeration (FLSv2MBusGPIO_rxMsg() must follow)
void FLSv2MBusGPIO_enumeration(volatile uint32_t fls_enum);

//Write Register
void FLSv2MBusGPIO_writeReg(volatile uint32_t short_prefix, volatile uint32_t reg_addr, volatile uint32_t reg_data);

//Read Register (FLSv2MBusGPIO_rxMsg() must follow)
void FLSv2MBusGPIO_readReg(volatile uint32_t short_prefix, volatile uint32_t reg_addr, volatile uint32_t length_1, volatile uint32_t reply_addr, volatile uint32_t dest_reg_addr);

//Write Memory (32-bit)
void FLSv2MBusGPIO_writeMem32(volatile uint32_t short_prefix, volatile uint32_t mem_start_addr, volatile uint32_t mem_data);

//Write Memory (64-bit)
void FLSv2MBusGPIO_writeMem64(volatile uint32_t short_prefix, volatile uint32_t mem_start_addr, volatile uint32_t mem_data_0, volatile uint32_t mem_data_1);

//Read Memory (FLSv2MBusGPIO_rxMsg() must follow)
void FLSv2MBusGPIO_readMem(volatile uint32_t short_prefix, volatile uint32_t reply_addr, volatile uint32_t length_1, volatile uint32_t mem_start_addr);


//*******************************************************************
// Flash-Specific Commands
//*******************************************************************

//Turn On Flash and Large Cap (FLSv2MBusGPIO_rxMsg() must follow)
//    NOTE: FLSv2MBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0xB5.
void FLSv2MBusGPIO_turnOn(volatile uint32_t fls_enum);

//Turn Off Flash and Large Cap (FLSv2MBusGPIO_rxMsg() must follow)
//    NOTE: FLSv2MBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0xBB.
void FLSv2MBusGPIO_turnOff(volatile uint32_t fls_enum);

//Turn On Flash (FLSv2MBusGPIO_rxMsg() must follow)
//    NOTE: FLSv2MBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0xB5.
void FLSv2MBusGPIO_turnOnFlash(volatile uint32_t fls_enum);

//Turn Off Flash (FLSv2MBusGPIO_rxMsg() must follow)
//    NOTE: FLSv2MBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0xBB.
void FLSv2MBusGPIO_turnOffFlash(volatile uint32_t fls_enum);

//Enable Large Cap
//    NOTE: This must be used right after FLSv2MBusGPIO_turnOnFlash().
//    NOTE: FLSv2MBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0xB5.
void FLSv2MBusGPIO_enableLargeCap(volatile uint32_t fls_enum);

//Disable Large Cap
//    NOTE: This must be used right after FLSv2MBusGPIO_turnOffFlash().
//    NOTE: FLSv2MBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0xBB.
void FLSv2MBusGPIO_disableLargeCap(volatile uint32_t fls_enum);

//set SRAM Start Address
//    - Used for the following functions: 
//          FLSv2MBusGPIO_doCopyFlash2SRAM()
//          FLSv2MBusGPIO_doCopySRAM2Flash()
//    - 32-bit Word, Total 8192 Words
//    -------------------------------
//    <ADDRESS SPACE>
//    0x00000000 - 0x00001FFF
void FLSv2MBusGPIO_setSRAMStartAddr(volatile uint32_t fls_enum, volatile uint32_t sram_start_addr);

//set Flash Start Address
//    - Used for the following functions: 
//          FLSv2MBusGPIO_doCopyFlash2SRAM()
//          FLSv2MBusGPIO_doCopySRAM2Flash()
//          FLSv2MBusGPIO_doEraseFlash()
//    - 32-bit Word, 2048 Words/Page, 8kB Page, Total 256 Pages
//    - For FLSv2MBusGPIO_doEraseFlash(), use the first address of the page.
//    -------------------------------
//    <ADDRESS SPACE>
//    0x00000000 - 0x000007FF: Page 0
//    0x00000800 - 0x00000FFF: Page 1
//    ....
//    0x0007F800 - 0x0007FFFF: Page 255
void FLSv2MBusGPIO_setFlashStartAddr(volatile uint32_t fls_enum, volatile uint32_t flash_start_addr);

//Copy SRAM to Flash
//    NOTE: FLSv2MBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0x3D.
//
//    - You need to use the following functions to set addresses before using this fuction
//          FLSv2MBusGPIO_setSRAMStartAddr()
//          FLSv2MBusGPIO_setFlashStartAddr()
//    - length = (Number of 32-bit Words) - 1
//          MAX = 0x1FFF (8192 words = 32kB)
//    - Following timing parameters are used:
//          T5us
//          T10us
//          Tprog
//          Tcyc_prog
void FLSv2MBusGPIO_doCopySRAM2Flash(volatile uint32_t fls_enum, volatile uint32_t length);

//Copy Flash to SRAM
//    NOTE: FLSv2MBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0x27.
//
//    - You need to use the following functions to set addresses before using this fuction
//          FLSv2MBusGPIO_setSRAMStartAddr()
//          FLSv2MBusGPIO_setFlashStartAddr()
//    - length = (Number of 32-bit Words) - 1
//          MAX = 0x1FFF (8192 words = 32kB)
//    - Following timing parameters are used:
//          Tcyc_read
void FLSv2MBusGPIO_doCopyFlash2SRAM(volatile uint32_t fls_enum, volatile uint32_t length);

//Erase Flash (FLSv2MBusGPIO_rxMsg() must follow)
//    NOTE: FLSv2MBusGPIO_rxMsg() must follow this function. If correct, the interrupt payload is 0x55.
//
//    - You need to use the following functions to set addresses before using this fuction
//          FLSv2MBusGPIO_setFlashStartAddr()
//    - Erase operation is done by page. And the FlashStartAddress must be a start address of a page.
//          (i.e., it must be a multiple of 0x800)
//    - Following timing parameters are used:
//          T5us
//          Terase
void FLSv2MBusGPIO_doEraseFlash(volatile uint32_t fls_enum);

//Config External Stream
//    - Use this function to configure exteranal streaming options.
//      This needs to be done before starting FLSv2MBusGPIO_doExtStream()
//    - bit_en: configure which pad to use
//              bit_en :              description
//              ---------------------------------------------------
//                0x0  : DATA_EXT[1] disabled, DATA_EXT[0] disabled
//                0x1  : DATA_EXT[1] disabled, DATA_EXT[0] enabled
//                0x2  : DATA_EXT[1] enabled,  DATA_EXT[0] disabled
//                0x3  : DATA_EXT[1] enabled,  DATA_EXT[0] enabled
//    - sram_start_addr: 
//                Address in SRAM at which the first data will be written
//                Range: 0x0000 ~ 0x1FFF
//    - timeout: 
//               Number of FSM clock cycles that triggers the time-out error during external streaming.
//               If timeout=0, it will NOT trigger the time-out error regardless of the idle time,
//               and it will be in the streaming mode indefinitely until one of the following events occurs:
//                  * Number of words streamed becomes equal to the value specified in LENGTH, which is set by FLSv2MBusGPIO_doExtStream()
//                  * User manually resets the controller using FLSv2MBusGPIO_forceReset()
//               Range: 0x00000 ~ 0xFFFFF
void FLSv2MBusGPIO_configExtStream(volatile uint32_t fls_enum, volatile uint32_t bit_en, volatile uint32_t sram_start_addr, volatile uint32_t timeout);

//External Input Stream (FLSv2MBusGPIO_rxMsg() must follow)
//    NOTE: If irq_en=1, FLSv2MBusGPIO_rxMsg() must follow this function. Below is the list of interrup payloads:
//               0xF5: External streaming time-out error
//               0xF7: External streaming too-fast error
//               0xF9: External streaming successful
//               0xFB: Has not yet received the number of words specified in 'length', but the address reaches the last SRAM address while wrapping is disabled.
//               0xFD: External streaming successful. Has received the number of words specified in 'length', but the address reaches the last SRAM address while wrapping is disabled.
//    - Use FLSv2MBusGPIO_configExtStream to configure options before using this function.
//    - irq_en:
//              Interrupt enable. If 1, it will send a reply once the external streaming finishes or becomes interruped due to an error.
//              You need to use irq_en=0, if timeout is set to 0 in FLSv2MBusGPIO_configExtStream().
//    - length: 
//              (Number of 32-bit Words) - 1
//              MAX = 0x1FFF (8192 words = 32kB)
void FLSv2MBusGPIO_doExtStream(volatile uint32_t fls_enum, volatile uint32_t irq_en, volatile uint32_t length);

//Force Reset
//    - Use this to terminate the external streaming mode when 'timeout' is set to 0.
void FLSv2MBusGPIO_forceReset(volatile uint32_t fls_enum);

//Set IRQ Address
//    - Whenever the flash layer sends out an interrupt message,
//      it will use the addresses specified by this function.
//    - The interrupt message has a form of 'Register Write' MBus message.
//    - 'short_addr' becomes the MBus address.
//    - 'reg_addr' becomes the Destination Register address.
void FLSv2MBusGPIO_setIRQAddr(volatile uint32_t fls_enum, volatile uint32_t short_addr, volatile uint32_t reg_addr);


//*******************************************************************
// Direct MBus Control
//*******************************************************************

// Reset MBus
void FLSv2MBusGPIO_resetMBus ();

//Send 8-bit Addr, 8-bit Data
void FLSv2MBusGPIO_sendMBus8bit (volatile uint32_t short_prefix, volatile uint32_t data_0);

//Send 8-bit Addr, 32-bit Data
void FLSv2MBusGPIO_sendMBus32bit (volatile uint32_t short_prefix, volatile uint32_t data_0);

//Send 8-bit Addr, 64-bit Data
void FLSv2MBusGPIO_sendMBus64bit (volatile uint32_t short_prefix, volatile uint32_t data_0, volatile uint32_t data_1);

//Send 8-bit Addr, 96-bit Data
void FLSv2MBusGPIO_sendMBus96bit (volatile uint32_t short_prefix, volatile uint32_t data_0, volatile uint32_t data_1, volatile uint32_t data_2);

//Receive an incoming MBus Msg
uint32_t FLSv2MBusGPIO_rxMsg ();

// Force Stop
void FLSv2MBusGPIO_forceStop ();

//Send 8-bit Addr, 31-bit Data: This is supposed to be used for flash streaming 'go' signal
//LSB in data_0 will NOT be sent.
void FLSv2MBusGPIO_sendMBus31bit (volatile uint32_t short_prefix, volatile uint32_t data_0);

//Send the Last 1-bit Data: This is supposed to be used for flash streaming 'go' signal
//Only LSB in data_0 will be sent as well as the tail sequence.
void FLSv2MBusGPIO_sendMBusLast1bit (volatile uint32_t short_prefix, volatile uint32_t data_0);


//*******************************************************************
// Sub Functions
//*******************************************************************
void FLSv2MBusGPIO_resetCinDin ();
void FLSv2MBusGPIO_toggleCinDin (volatile uint32_t data, volatile uint32_t numBit);
void FLSv2MBusGPIO_interjection ();
void FLSv2MBusGPIO_controlAck ();
uint32_t FLSv2MBusGPIO_getCoutDout ();
uint32_t FLSv2MBusGPIO_getCout ();
uint32_t FLSv2MBusGPIO_getDout ();
uint32_t FLSv2MBusGPIO_dataFwd ();

#endif // FLSv2L_GPIO_H

