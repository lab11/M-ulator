#ifndef __HELPERS_H
#define __HELPERS_H

#include "../cortex_m3.h"

int hamming(uint32_t val);

uint32_t SignExtend(uint32_t val, uint8_t bits);

#endif // __HELPERS_H
