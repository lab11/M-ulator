#ifndef MBC_PARAMS_16
#define MBC_PARAMS_16

#define CHIP_ID 0	// 22 mod 8

#define USE_GMB

#define PMU_55C 0x1167
const uint32_t PMU_TEMP_THRESH[6] = {0x1DD, 0x30B, 0x4D1, 0x6CC, 0xC24, 0x1409}; // {0, 10, 20, 30, 45, 60}

//const uint8_t PMU_ADC_THRESH[4] = {90, 90, 90, 90};	// RUN FROM GMB NO NEED TO CHECK VOLT


#endif
