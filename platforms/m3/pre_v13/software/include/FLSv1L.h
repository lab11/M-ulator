//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv12 header file
//*******************************************************************

#ifndef FLSV1L_H
#define FLSV1L_H

//*********************************************************
// INCLUDES...
//*********************************************************
#include <stdint.h>
#include <stdbool.h>
#include "PREv13_RF.h"

//*********************************************************
// FUNCTIONS
//*********************************************************

/**
 * @brief   Turns on the flash memory
 *
 *          It turns on the voltage/current limiter and the flash memory.
 *          It does NOT enable the connection to large off-chip capacitor.
 *
 * @irq_payload 0x03
 *
 * @param   fls_enum    FLSv1L Short-Prefix
 */
void FLSv1L_turnOnFlash (uint8_t fls_enum);

/**
 * @brief   Turns off the flash memory
 *
 *          It turns off the voltage/current limiter and the flash memory.
 *          It does NOT disable the connection to large off-chip capacitor.
 *
 * @irq_payload 0x06
 *
 * @param   fls_enum    FLSv1L Short-Prefix
 */
void FLSv1L_turnOffFlash (uint8_t fls_enum);

/**
 * @brief   Enable the connection to the large off-chip capacitor
 *
 *          It may be used right after FLSv1L_turnOnFlash()
 *
 * @param   fls_enum    FLSv1L Short-Prefix
 */
void FLSv1L_enableLargeCap (uint8_t fls_enum);

/**
 * @brief   Disable the connection to the large off-chip capacitor
 *
 *          It may be used right after FLSv1L_turnOffFlash()
 *
 * @param   fls_enum    FLSv1L Short-Prefix
 */
void FLSv1L_disableLargeCap (uint8_t fls_enum);

/**
 * @brief   Sets IRQ Address
 *
 *          Whenever the flash layer sends out an interrupt message, 
 *          it will use the addresses specified by this function.
 *          The interrupt message has a form of 'Register Write' MBus message.
 *
 * @param   fls_enum    FLSv1L Short-Prefix
 * @param   short_addr  This becomes MBus Address in the interrupt message
 * @param   reg_addr    This becomes Destination Register Address in the interrupt message
 */
void FLSv1L_setIRQAddr (uint8_t fls_enum, uint8_t short_addr, uint8_t reg_addr);

/**
 * @brief   Optimize the setting for Flash Programming
 *
 *          It sets the following tuning bits:
 *              Tprog = 0x05
 *              Tcyc = 0x00C0
 *              VTG_TUNE = 0x8
 *              CRT_TUNE = 0x3F
 *
 * @param   fls_enum    FLSv1L Short-Prefix
 */
void FLSv1L_setOptTune (uint8_t fls_enum);

/**
 * @brief   Sets T5us, T10us, and Terase
 *
 *          It sets the following tuning bits:
 *              T5us    (Default: 0x0)
 *              T10us   (Default: 0x2)
 *              Terase  (Default:0x1AA0)
 *
 * @param   fls_enum    FLSv1L Short-Prefix
 * @param   T5us        T5us
 * @param   T10us       T10us
 * @param   Terase      Terase
 */
void FLSv1L_setTerase (uint8_t fls_enum, uint8_t T5us, uint8_t T10us, uint16_t Terase);

/**
 * @brief   Sets SRAM Start Address
 *
 *          32-bit Word, Total 8192 Words
 *          -----------------------------
 *          <ADDRESS SPACE>
 *          0x00000000 - 0x00001FFF
 *          -----------------------------
 *          Use for the following functions:
 *              FLSv1L_doCopyFlash2SRAM();
 *              FLSv1L_doCopySRAM2Flash();
 *
 * @param   fls_enum            FLSv1L Short-Prefix
 * @param   sram_start_addr     SRAM Start Address
 */
void FLSv1L_setSRAMStartAddr (uint8_t fls_enum, uint32_t sram_start_addr);

/**
 * @brief   Sets Flash Start Address
 *
 *          Double-Word-Aligned: LSB is always 0
 *          64-bit Word, 1024 Words/Page, 8kB/Page, Total 256 Pages
 *          For FLSv1L_doEraseFlash(), use the first address of the page
 *          -----------------------------
 *          <ADDRESS SPACE>
 *          0x00000000 - 0x000007FE: Page 0
 *          0x00000800 - 0x00000FFE: Page 1
 *          ....
 *          0x0007F800 - 0x0007FFFE: Page 255
 *          -----------------------------
 *          Use for the following functions:
 *              FLSv1L_doCopyFlash2SRAM();
 *              FLSv1L_doCopySRAM2Flash();
 *              FLSv1L_doEraseFlash();
 *
 * @param   fls_enum            FLSv1L Short-Prefix
 * @param   flash_start_addr    Flash Start Address
 */
void FLSv1L_setFlashStartAddr (uint8_t fls_enum, uint32_t flash_start_addr);

/**
 * @brief   Sets External Stream Width/Legnth
 *
 *          Use for the following functions:
 *              FLSv1L_doExtStream();
 *
 * @param   fls_enum    FLSv1L Short-Prefix
 *
 * @param   width       Bus width in streaming
 *                          width=2'h0: 1-bit
 *                          width=2'h1: 2-bits
 *                          width=2'h2: not defined
 *                          width=2'h3: 4-bits
 *
 * @param   length      (Number of 32-bit Words)-2; Max = 0x1FFE (8192 words = 32kB)
 *                      'length' should be an even number.
 */
void FLSv1L_setExtStream (uint8_t fls_enum, uint8_t width, uint16_t length);

/**
 * @brief   Copy Flash to SRAM
 *
 * @param   fls_enum    FLSv1L Short-Prefix
 *
 * @irq_payload 0x46
 *
 * @param   length      (Number of 32-bit Words)-2; Max = 0x7FE (2048 words = 8kB)
 *                      'length' should be an even number.
 */
void FLSv1L_doCopyFlash2SRAM (uint8_t fls_enum, uint32_t length);

/**
 * @brief   Copy SRAM to Flash
 *
 * @irq_payload 0x5C
 *
 * @param   fls_enum    FLSv1L Short-Prefix
 *
 * @param   length      (Number of 32-bit Words)-2; Max = 0x7FE (2048 words = 8kB)
 *                      'length' should be an even number.
 */
void FLSv1L_doCopySRAM2Flash (uint8_t fls_enum, uint32_t length);

/**
 * @brief   Erase Flash
 *
 *          Erase operation is done by page.
 *          Use FLSv1L_setFlashStartAddr() to specify the start address of the page to be erased.
 *
 * @irq_payload 0x74
 *
 * @param   fls_enum    FLSv1L Short-Prefix
 */
void FLSv1L_doEraseFlash (uint8_t fls_enum);

/**
 * @brief   Start External Input Stream
 *
 *          The start address is supposed to be the very first address (ADDR[0]) of SRAM.
 *          However, there is a bug so that the Flash Layer needs to be reset in order to start from ADDR[0].
 *          This reset can be either 'sleep -> wakeup sequence' or 'POR sequence'.
 *          If user does two separate 'stream-in' after a reset, the start address of the second stream-in 
 *          might not be ADDR[0]. It's undeterministic.
 *
 *          Length must be correctly set using FLSv1L_setExtStream().
 *
 * @irq_payload 0x82    Success
 *              0x80    Time-Out
 *              0x83    Too-Fast Streaming
 *
 * @param   fls_enum    FLSv1L Short-Prefix
 */
void FLSv1L_doExtStream (uint8_t fls_enum);

#endif // FLSV1L_H

