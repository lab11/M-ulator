#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"

int mul(uint8_t setflags, uint8_t rd, uint8_t rn, uint8_t rm) {
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

	return SUCCESS;
}

int mul_t2(uint32_t inst) {
	uint8_t rm = (inst & 0xf);
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t rn = (inst & 0xf0000) >> 16;

	if ((rd >= 13) || (rn >= 13) || (rm >= 13)) {
		CORE_ERR_unpredictable("mul_t2 bad reg\n");
	}

	return mul(false, rd, rn, rm);
}

void register_opcodes_mul(void) {
	// mul_t2: 1111 1011 0000 xxxx 1111 xxxx 0000 xxxx
	register_opcode_mask(0xfb00f000, 0x04f000f0, mul_t2);
}
