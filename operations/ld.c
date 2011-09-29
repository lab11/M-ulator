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

int ldr2(uint32_t inst) {
	uint8_t rm = (inst & 0x1c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7);

	uint32_t address = CORE_reg_read(rn) + CORE_reg_read(rm);

	if (address & 0x3) {
		CORE_ERR_unpredictable("ldr2 bad addr\n");
	} else {
		CORE_reg_write(rd, read_word(address));
	}

	DBG2("ldr2 r%02d, [r%02d, r%02d]\n", rd, rn, rm);

	return SUCCESS;
}

int ldr3(uint32_t inst) {
	uint8_t rd = (inst & 0x700) >> 8;
	uint16_t immed8 = inst & 0xff;

	uint32_t pc = CORE_reg_read(PC_REG);
	uint32_t address;

	address = (pc & 0xfffffffe) + (immed8 * 4);
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

int ldrd_imm(uint32_t inst) {
	uint8_t imm8 = (inst & 0xff);
	uint8_t rt2 = (inst & 0xf00) >> 8;
	uint8_t rt = (inst & 0xf000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t W = !!(inst & 0x200000);
	uint8_t U = !!(inst & 0x800000);
	uint8_t P = !!(inst & 0x1000000);

	if ((P == 0) && (W == 0))
		CORE_ERR_unpredictable("ldrd_imm -> EX\n");

	if (rn == 0xf)
		CORE_ERR_unpredictable("ldrd_imm -> LDRD_lit\n");

	uint32_t imm32 = imm8 << 2;
	// index = P
	// add = U
	// wback = W

	if (W && ((rn == rt) || (rn == rt2)))
		CORE_ERR_unpredictable("ldrd_imm wbck + regs\n");

	if ((rt >= 13) || (rt2 >= 13) || (rt == rt2))
		CORE_ERR_unpredictable("ldrd_imm bad regs\n");

	uint32_t offset_addr;
	if (U) {
		offset_addr = CORE_reg_read(rn) + imm32;
	} else {
		offset_addr = CORE_reg_read(rn) - imm32;
	}

	uint32_t address;
	if (P) {
		address = offset_addr;
	} else {
		address = CORE_reg_read(rn);
	}

	CORE_reg_write(rt, read_word(address));
	CORE_reg_write(rt2, read_word(address + 4));

	if (W) {
		CORE_reg_write(rn, offset_addr);
	}

	DBG2("ldrd_imm did lots of stuff\n");

	return SUCCESS;
}

void register_opcodes_ld(void) {
	// ldr1: 0110 1<x's>
	register_opcode_mask(0x6800, 0xffff9000, ldr1);

	// ldr2: 0101 100<x's>
	register_opcode_mask(0x5800, 0xffffa600, ldr2);

	// ldr3: 0100 1<x's>
	register_opcode_mask(0x4800, 0xffffb000, ldr3);

	// ldr4: 1001 1<x's>
	register_opcode_mask(0x9800, 0xffff6000, ldr4);

	// ldrb1: 0111 1<x's>
	register_opcode_mask(0x7800, 0xffff8000, ldrb1);

	// ldrd_imm: 1110 100x x1x1 <x's>
	register_opcode_mask(0xe8500000, 0x16000000, ldrd_imm);
}
