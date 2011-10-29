#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"

uint32_t hamming(uint32_t val) {
	uint32_t hamm = 0;

	for (hamm = 0; val; hamm++) {
		val &= (val - 1); // clears LSB
	}

	return hamm;
}

// Only supports N == 32 bits
void AddWithCarry(uint32_t x, uint32_t y, bool carry_in,
		uint32_t *result, bool *carry_out, bool *overflow_out) {
	uint64_t unsigned_sum = x + y + carry_in;
	int64_t signed_sum = ((int32_t) x) + ((int32_t) y) + carry_in;

	*result = (uint32_t) unsigned_sum; // 64->32 truncation
	uint64_t result64_u = *result;
	int64_t result64_s = *result;
	*carry_out = !(result64_u == unsigned_sum);
	*overflow_out = !(result64_s == signed_sum);
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
