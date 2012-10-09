#include "opcodes.h"
#include "helpers.h"

#include "cpu/cpu.h"

static void sxtb(uint8_t rd, uint8_t rm, uint8_t rotation) {
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t rotated = Shift(rm_val, 32, ROR, rotation, 0);
	int8_t trunc = (int8_t) rotated;
	int32_t signd = (int32_t) trunc;
	CORE_reg_write(rd, signd);
}

static void sxtb_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	uint8_t rotation = 0;

	return sxtb(rd, rm, rotation);
}

// XXX: remove attribute after completing implementation
static void uxtb(uint8_t rd, uint8_t rm __attribute__ ((unused)), uint8_t rotation) {
	uint32_t rd_val = CORE_reg_read(rd);

	uint32_t rotated;
	if (rotation != 0) {
		CORE_ERR_not_implemented("uxtb rotation\n");
	} else {
		rotated = rd_val;
	}

	rd_val = rotated & 0xff;
	CORE_reg_write(rd, rd_val);

	DBG2("uxtb wrote r%02d = %08x\n", rd, rd_val);
}

static void uxtb_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst & 0x38) >> 3;

	uint8_t rotation = 0;

	return uxtb(rd, rm, rotation);
}

static void ubfx(uint8_t rd, uint8_t rn, uint8_t lsbit, uint8_t widthminus1) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint8_t msbit = lsbit + widthminus1;
	if (msbit <= 31)
		CORE_reg_write(rd, (rn_val >> lsbit) & ((1 << (msbit - lsbit + 1)) - 1));
	else
		CORE_ERR_unpredictable("msb > 31?\n");
}

static void ubfx_t1(uint32_t inst) {
	uint8_t widthm1 = inst & 0x1f;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;

	uint8_t lsbit = (imm3 << 2) | imm2;
	uint8_t widthminus1 = widthm1;

	if (BadReg(rd) || BadReg(rn))
		CORE_ERR_unpredictable("bad reg\n");

	return ubfx(rd, rn, lsbit, widthminus1);
}

__attribute__ ((constructor))
void register_opcodes_extend(void) {
	// sxtb_t1: 1011 0010 01xx xxxx
	register_opcode_mask_16(0xb240, 0x4d80, sxtb_t1);

	// uxtb_t1: 1011 0010 11<x's>
	register_opcode_mask_16(0xb2c0, 0x4d00, uxtb_t1);

	// ubfx_t1: 1111 0011 1100 xxxx 0xxx xxxx xx0x xxxx
	register_opcode_mask_32(0xf3c00000, 0x0c308020, ubfx_t1);
}
