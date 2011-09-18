#include "../cortex_m3.h"

int str1(uint32_t inst) {
	int32_t immed5 = (inst & 0x7c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t address = CORE_reg_read(rn) + (immed5 * 4);
	uint32_t rd_val = CORE_reg_read(rd);
	if ((address & 0x3) == 0) {
		write_word(address, rd_val);
	} else {
		// misaligned
		return FAILURE;
	}

	DBG2("str1 r%02d, [r%02d, #%d*4]\t0x%08x = 0x%08x\n",
			rd, rn, immed5, address, rd_val);

	return SUCCESS;
}

int str3(uint32_t inst) {
	uint8_t rd = (inst & 0x700) >> 8;
	uint16_t immed8 = inst & 0xff;

	uint32_t sp = CORE_reg_read(SP_REG);
	uint32_t rd_val = CORE_reg_read(rd);

	uint32_t address = sp + (immed8 << 2);
	if ((address & 0x3) == 0) {
		write_word(address, rd_val);
	} else {
		CORE_ERR_invalid_addr(true, address);
		return FAILURE;
	}

	DBG2("str r%02d, [sp, #%d * 4]\n", rd, immed8);

	return SUCCESS;
}

int strb1(uint32_t inst) {
	uint8_t immed5 = (inst & 0x7c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t address = CORE_reg_read(rn) + immed5;
	write_byte(address, CORE_reg_read(rd) & 0xff);

	DBG2("strb r%02d, [r%02d, #%d]\n", rd, rn, immed5);

	return SUCCESS;
}

void register_opcodes_str(void) {
	// str1: 0110 0<x's>
	register_opcode_mask(0x6000, 0xffff9800, str1);

	// str3: 1001 0<x's>
	register_opcode_mask(0x9000, 0xffff6800, str3);

	// strb1: 0111 0<x's>
	register_opcode_mask(0x7000, 0xffff8800, strb1);
}
