#ifndef MBC_PARAMS_31
#define MBC_PARAMS_31

#define CHIP_ID 15	// 22 mod 8

//#define USE_GMB

#define PMU_55C 0x149E
const uint32_t PMU_TEMP_THRESH[6] = {0x1ED, 0x39B, 0x5B3, 0x809, 0xD40, 0x184D}; // {0, 10, 20, 30, 45, 60}

const uint8_t PMU_ADC_THRESH[4] = {83, 84, 85, 84};	// RUN FROM GMB NO NEED TO CHECK VOLT


#endif
