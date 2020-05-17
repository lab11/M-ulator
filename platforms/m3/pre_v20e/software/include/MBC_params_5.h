#ifndef MBC_PARAMS_5
#define MBC_PARAMS_5

#define CHIP_ID 5	// 22 mod 8

//#define USE_GMB

#define PMU_55C 0x1420
const uint32_t PMU_TEMP_THRESH[6] = {0x1ED, 0x322, 0x4F3, 0x709, 0xD00, 0x16C0}; // {0, 10, 20, 30, 45, 60}

const uint8_t PMU_ADC_THRESH[4] = {81, 83, 84, 83};	// RUN FROM GMB NO NEED TO CHECK VOLT


#endif
