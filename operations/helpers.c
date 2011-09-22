#include "helpers.h"

int hamming(uint32_t val) {
	int hamm = 0;

	for (hamm = 0; val; hamm++) {
		val &= (val - 1); // clears LSB
	}

	return hamm;
}

