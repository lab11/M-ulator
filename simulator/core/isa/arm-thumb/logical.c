/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/logical.h"

// arm-thumb
static void and_reg_t1(uint16_t inst) {
	uint8_t rdn = inst & 0x7;
	uint8_t rm  = (inst >> 3) & 0x7;

	uint8_t rd = rdn;
	uint8_t rn = rdn;
	bool setflags = !in_ITblock();
	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("AND<IT> <Rdn>,Rm>", rdn, rm);
	return and_reg(rd, rn, rm, setflags, shift_t, shift_n);
}

// arm-thumb
static void bic_reg_t1(uint16_t inst) {
	uint8_t rdn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	uint8_t rd = rdn;
	uint8_t rn = rdn;
	bool setflags = !in_ITblock();
	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("BIC<IT> <Rdn>,<Rm>", rdn, rm);
	return bic_reg(rd, rn, rm, setflags, shift_t, shift_n);
}

// arm-thumb
static void eor_reg_t1(uint16_t inst) {
	uint8_t rdn = inst & 0x7;
	uint8_t rm  = (inst >> 3) & 0x7;

	uint8_t rd = rdn;
	uint8_t rn = rdn;
	bool setflags = !in_ITblock();
	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("EOR<IT> <Rdn>,<Rm>", rdn, rm);
	return eor_reg(setflags, rd, rn, rm, shift_t, shift_n);
}

// arm-thumb
static void mvn_reg_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	bool setflags = !in_ITblock();
	enum SRType shift_t = LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("MVN<IT> <Rd>,<Rm>", rd, rm);
	return mvn_reg(setflags, rd, rm, shift_t, shift_n);
}

// arm-thumb
static void orr_reg_t1(uint16_t inst) {
	uint8_t rdn = inst & 0x7;
	uint8_t rm  = (inst & 0x38) >> 3;

	uint8_t rd = rdn;
	uint8_t rn = rdn;

	bool setflags = !in_ITblock();
	enum SRType shift_t = LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("ORR<IT> <Rdn>,<Rm>", rd, rm);

	return orr_reg(setflags, rd, rn, rm, shift_t, shift_n);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_logical(void) {
	// and_reg_t1: 0100 0000 00xx xxxx
	register_opcode_mask_16(0x4000, 0xbfc0, and_reg_t1);

	// bic_reg_t1: 0100 0011 10<x's>
	register_opcode_mask_16(0x4380, 0xbc40, bic_reg_t1);

	// eor_reg_t1: 0100 0000 01xx xxxx
	register_opcode_mask_16(0x4040, 0xbf80, eor_reg_t1);

	// mvn_reg_t1: 0100 0011 11xx xxxx
	register_opcode_mask_16(0x43c0, 0xbc00, mvn_reg_t1);

	// orr_reg_t1: 0100 0011 00<x's>
	register_opcode_mask_16(0x4300, 0xbcc0, orr_reg_t1);
}
