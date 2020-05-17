#ifndef MBC_PARAMS_19
#define MBC_PARAMS_19

#define CHIP_ID 4	// 22 mod 8

#define USE_GMB

#define PMU_55C 0x15F1
const uint32_t PMU_TEMP_THRESH[6] = {0x1DD, 0x312, 0x4DE, 0x772, 0xC2C, 0x16A8}; // {0, 10, 20, 30, 45, 60}

//const uint8_t PMU_ADC_THRESH[4] = {90, 90, 90, 90};	// RUN FROM GMB NO NEED TO CHECK VOLT


#endif
