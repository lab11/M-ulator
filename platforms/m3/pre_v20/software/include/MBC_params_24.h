#ifndef MBC_PARAMS_24
#define MBC_PARAMS_24

#define CHIP_ID 8	// 22 mod 8

//#define USE_GMB

#define PMU_55C 0x1480
const uint32_t PMU_TEMP_THRESH[6] = {0x1E0, 0x388, 0x58D, 0x7E0, 0xD00, 0x1830}; // {0, 10, 20, 30, 45, 60}

const uint8_t PMU_ADC_THRESH[4] = {82, 84, 85, 84};	// RUN FROM GMB NO NEED TO CHECK VOLT


#endif
