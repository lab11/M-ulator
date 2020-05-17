#ifndef MBC_PARAMS_22
#define MBC_PARAMS_22

#define CHIP_ID 6	// 22 mod 8

// #define USE_GMB

#define PMU_55C 0x149E
const uint32_t PMU_TEMP_THRESH[6] = {0x20D, 0x35B, 0x54E, 0x817, 0xD40, 0x184D}; // {0, 10, 20, 30, 45, 60}

const uint8_t PMU_ADC_THRESH[4] = {90, 90, 90, 90};	// RUN FROM GMB NO NEED TO CHECK VOLT


#endif
