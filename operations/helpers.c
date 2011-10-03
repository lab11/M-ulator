#include "../cortex_m3.h"
#include "cpsr.h"
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

void LoadWritePC(uint32_t addr) {
	BXWritePC(addr);
}

void BXWritePC(uint32_t addr) {
	// XXX: Mode handler / Exception stuff

	SET_THUMB_BIT(addr & 0x1);

	BranchTo(addr & 0xfffffffe);
}

void BranchTo(uint32_t addr) {
	CORE_reg_write(PC_REG, addr);
}

void BranchWritePC(uint32_t addr) {
	if (GET_ISETSTATE == INST_SET_ARM) {
#ifdef M_PROFILE
		assert(false && "Arm state in M profile?");
#endif
		BranchTo(addr & 0xfffffffc);
	} else if (GET_ISETSTATE == INST_SET_JAZELLE) {
#ifdef M_PROFILE
		assert(false && "Jazelle state in M profile?");
#endif
		CORE_ERR_not_implemented("Jazelle\n");
	} else {
		BranchTo(addr & 0xfffffffe);
	}
}
