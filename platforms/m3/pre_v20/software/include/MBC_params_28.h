#ifndef MBC_PARAMS_28
#define MBC_PARAMS_28

#define CHIP_ID 2	// 22 mod 8

#define USE_GMB

#define PMU_55C 0x1400
const uint32_t PMU_TEMP_THRESH[6] = {0x1F0, 0x33A, 0x525, 0x74B, 0xCD0, 0x1720}; // {0, 10, 20, 30, 45, 60}

//const uint8_t PMU_ADC_THRESH[4] = {90, 90, 90, 90};	// RUN FROM GMB NO NEED TO CHECK VOLT


#endif
