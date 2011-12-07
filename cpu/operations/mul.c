#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"

void mul(uint8_t setflags, uint8_t rd, uint8_t rn, uint8_t rm) {
	uint32_t result;

	result = CORE_reg_read(rn) * CORE_reg_read(rm);
	CORE_reg_write(rd, result);

	if (setflags) {
		uint32_t cpsr = CORE_cpsr_read();
		cpsr = GEN_NZCV(
				!!(result & xPSR_N),
				result == 0,
				!!(cpsr & xPSR_C),
				!!(cpsr & xPSR_V)
			       );
		CORE_cpsr_write(cpsr);
	}
}

void mul_t2(uint32_t inst) {
	uint8_t rm = (inst & 0xf);
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t rn = (inst & 0xf0000) >> 16;

	if ((rd >= 13) || (rn >= 13) || (rm >= 13)) {
		CORE_ERR_unpredictable("mul_t2 bad reg\n");
	}

	return mul(false, rd, rn, rm);
}

void umull(uint8_t rdlo, uint8_t rdhi, uint8_t rn, uint8_t rm, bool setflags) {
	assert(setflags == false);

	uint64_t rn_val = CORE_reg_read(rn);
	uint64_t rm_val = CORE_reg_read(rm);
	uint64_t result = rn_val * rm_val;

	CORE_reg_write(rdhi, (result >> 32) & 0xffffffff);
	CORE_reg_write(rdlo, result & 0xffffffff);
}

void umull_t1(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t rdhi = (inst >> 8) & 0xf;
	uint8_t rdlo = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	bool setflags = false;

	if (BadReg(rdlo) || BadReg(rdhi) || BadReg(rn) || BadReg(rm))
		CORE_ERR_unpredictable("umull bad reg\n");

	if (rdhi == rdlo)
		CORE_ERR_unpredictable("umull same hi lo regs\n");

	return umull(rdlo, rdhi, rn, rm, setflags);
}

void register_opcodes_mul(void) {
	// mul_t2: 1111 1011 0000 xxxx 1111 xxxx 0000 xxxx
	register_opcode_mask(0xfb00f000, 0x04f000f0, mul_t2);

	// umull_t1: 1111 1011 1010 xxxx xxxx xxxx 0000 xxxx
	register_opcode_mask(0xfba00000, 0x045000f0, umull_t1);
}
