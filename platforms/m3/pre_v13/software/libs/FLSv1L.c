//*******************************************************************
//Author: Yejoong Kim
//Description: FLSv1L lib file
//*******************************************************************

#include "FLSv1L.h"
#include "mbus.h"

//*******************************************************************
// OTHER FUNCTIONS
//*******************************************************************

void FLSv1L_turnOnFlash (uint8_t fls_enum) {
    mbus_remote_register_write (fls_enum, 0x11, 0x000003);
}

void FLSv1L_turnOffFlash (uint8_t fls_enum) {
    mbus_remote_register_write (fls_enum, 0x11, 0x000002);
}

void FLSv1L_enableLargeCap (uint8_t fls_enum) {
    mbus_remote_register_write (fls_enum, 0x09, 0x00003F);
}

void FLSv1L_disableLargeCap (uint8_t fls_enum) {
    mbus_remote_register_write (fls_enum, 0x09, 0x000000);
}

void FLSv1L_setIRQAddr (uint8_t fls_enum, uint8_t short_addr, uint8_t reg_addr) {
    mbus_remote_register_write (fls_enum, 0x0C, ((((uint32_t) short_addr) << 8) | (((uint32_t) reg_addr) << 0)));
}

void FLSv1L_setOptTune (uint8_t fls_enum) {
    // Set Tprog=0x5, Tcyc=0x00C0
    mbus_remote_register_write (fls_enum, 0x02, ((0x5 << 16) | (0x00C0 << 0)));
    // Set VTG_TUNE=0x8, CRT_TUNE=0x3F
    mbus_remote_register_write (fls_enum, 0x0A, ((0x8 << 6) | (0x3F << 0)));
}

void FLSv1L_setTerase (uint8_t fls_enum, uint8_t T5us, uint8_t T10us, uint16_t Terase) {
    mbus_remote_register_write (fls_enum, 0x01, ((((uint32_t) T10us) << 20) | (((uint32_t) T5us) << 16) | (((uint32_t) Terase) << 0)));
}

void FLSv1L_setSRAMStartAddr (uint8_t fls_enum, uint32_t sram_start_addr) {
    mbus_remote_register_write (fls_enum, 0x06, sram_start_addr);
}

void FLSv1L_setFlashStartAddr (uint8_t fls_enum, uint32_t flash_start_addr) {
    mbus_remote_register_write (fls_enum, 0x07, flash_start_addr);
}

void FLSv1L_setExtStream (uint8_t fls_enum, uint8_t width, uint16_t length) {
    width = width & 0x03;
    length = length & 0x1FFF;
    mbus_remote_register_write (fls_enum, 0x03, ((((uint32_t) length) << 2) | (((uint32_t) width) << 0)));
}

void FLSv1L_doCopyFlash2SRAM (uint8_t fls_enum, uint32_t length) {
    mbus_remote_register_write (fls_enum, 0x05, ((0x1 << 15) | (length << 4) | (0x1 << 1) | (0x1 << 0)));
}

void FLSv1L_doCopySRAM2Flash (uint8_t fls_enum, uint32_t length) {
    mbus_remote_register_write (fls_enum, 0x05, ((0x1 << 15) | (length << 4) | (0x2 << 1) | (0x1 << 0)));
}

void FLSv1L_doEraseFlash (uint8_t fls_enum) {
    mbus_remote_register_write (fls_enum, 0x05, ((0x1 << 15) | (0x4 << 1) | (0x1 << 0)));
}

void FLSv1L_doExtStream (uint8_t fls_enum) {
    mbus_remote_register_write (fls_enum, 0x05, ((0x1 << 15) | (0x6 << 1) | (0x1 << 0)));
}

