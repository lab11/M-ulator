#include "helpers.h"

int hamming(uint32_t val) {
	int hamm = 0;

	for (hamm = 0; val; hamm++) {
		val &= (val - 1); // clears LSB
	}

	return hamm;
}

uint32_t SignExtend(uint32_t val, uint8_t bits) {
	assert((bits > 0) && (bits < 32));

	if ((val & (1 << (bits-1))) > 0) {
		val |= (0xffffffff << bits);
	}

	return val;
}
