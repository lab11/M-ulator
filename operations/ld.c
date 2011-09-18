#include "../cortex_m3.h"

int ldr1(uint32_t inst) {
	uint8_t immed5 = (inst & 0x7c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t address = CORE_reg_read(rn) + (immed5 * 4);
	if ((address & 0x3) == 0) {
		CORE_reg_write(rd, read_word(address));
	} else {
		// misaligned
		DBG2("address: %08x\n", address);
		DBG2("immed5: %x, immed5*4: %x\n", immed5, immed5*4);
		return FAILURE;
	}

	DBG2("ldr1 r%02d, [r%02d, #%d*4]\n", rd, rn, immed5);

	return SUCCESS;
}

int ldr3(uint32_t inst) {
	uint8_t rd = (inst & 0x700) >> 8;
	uint16_t immed8 = inst & 0xff;

	uint32_t pc = CORE_reg_read(PC_REG);
	uint32_t address;

	address = (pc & 0xfffffffe) + (immed8 * 4) + 2; // PC_HACK
	CORE_reg_write(rd, read_word(address));

	DBG2("ldr3 r%02d, [PC, #%d*4]\n", rd, immed8);

	return SUCCESS;
}

int ldr4(uint32_t inst) {
	uint8_t rd = (inst & 0x700) >> 8;
	uint16_t immed8 = inst & 0xff;

	uint32_t sp = CORE_reg_read(SP_REG);

	uint32_t address = sp + (immed8 << 2);

	if ((address & 0x3) == 0) {
		uint32_t rd_val = read_word(address);
		CORE_reg_write(rd, rd_val);
	} else {
		CORE_ERR_invalid_addr(true, address);
		return FAILURE;
	}

	DBG2("ldr r%02d, [sp, #%d * 4]\n", rd, immed8);

	return SUCCESS;
}

int ldrb1(uint32_t inst) {
	uint8_t immed5 = (inst & 0x7c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t address = CORE_reg_read(rn) + immed5;
	CORE_reg_write(rd, read_byte(address));

	DBG2("ldrb r%02d, [r%02d, #%d]\n", rd, rn, immed5);

	return SUCCESS;
}

void register_opcodes_ld(void) {
	// ldr1: 0110 1<x's>
	register_opcode_mask(0x6800, 0xffff9000, ldr1);

	// ldr3: 0100 1<x's>
	register_opcode_mask(0x4800, 0xffffb000, ldr3);

	// ldr4: 1001 1<x's>
	register_opcode_mask(0x9800, 0xffff6000, ldr4);

	// ldrb1: 0111 1<x's>
	register_opcode_mask(0x7800, 0xffff8000, ldrb1);
}
