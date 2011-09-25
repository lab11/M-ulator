#include "../cortex_m3.h"

int uxtb(uint8_t rd, uint8_t rm, uint8_t rotation) {
	uint32_t rd_val = CORE_reg_read(rd);

	uint32_t rotated;
	if (rotation != 0) {
		rm = rm; // Silence compiler until this is implemented
		CORE_ERR_not_implemented("uxtb rotation\n");
	} else {
		rotated = rd_val;
	}

	rd_val = rotated & 0xff;
	CORE_reg_write(rd, rd_val);

	DBG2("uxtb wrote r%02d = %08x\n", rd, rd_val);

	return SUCCESS;
}

int uxtb_t1(uint32_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst & 0x38) >> 3;

	uint8_t rotation = 0;

	return uxtb(rd, rm, rotation);
}

void register_opcodes_extend(void) {
	// uxtb_t1: 1011 0010 11<x's>
	register_opcode_mask(0xb2c0, 0xffff4d00, uxtb_t1);
}
