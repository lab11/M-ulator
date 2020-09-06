/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/cmp.h"

// arm-thumb
static void cmn_reg_t1(uint16_t inst) {
	uint8_t rn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("CMN<c> <Rn>,<Rm>", rn, rm);
	return cmn_reg(rn, rm, shift_t, shift_n);
}

// arm-thumb
static void cmp_imm_t1(uint16_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rn = (inst >> 8) & 0x7;

	uint32_t imm32 = imm8;

	OP_DECOMPILE("CMP<c> <Rn>,#<imm8>", rn, imm8);
	return cmp_imm(rn, imm32);
}

// arm-thumb
static void cmp_reg_t1(uint16_t inst) {
	uint8_t rn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	OP_DECOMPILE("CMP<c> <Rn>,<Rm>", rn, rm);
	return cmp_reg(rn, rm, SRType_LSL, 0);
}

// arm-thumb
static void cmp_reg_t2(uint16_t inst) {
	uint8_t rn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0xf;
	bool N = (inst >> 7) & 0x1;

	rn = (N << 3) | rn;

	enum SRType shift_t = LSL;
	uint8_t shift_n = 0;

	if ((rn < 8) && (rm < 8))
		CORE_ERR_unpredictable("cmp bad reg\n");

	if ((rn == 15) && (rm == 15))
		CORE_ERR_unpredictable("cmp bad regs\n");

	OP_DECOMPILE("CMP<c> <Rn>,<Rm>", rn, rm);
	return cmp_reg(rn, rm, shift_t, shift_n);
}

// arm-thumb
static void tst_reg_t1(uint16_t inst) {
	uint8_t rn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("TST<c> <Rn>,<Rm>", rn, rm);
	return tst_reg(rn, rm, shift_t, shift_n);
}

__attribute__ ((constructor))
static void register_opcodes_cmp(void) {
	// cmn_reg_t1: 0100 0010 11xx xxxx
	register_opcode_mask_16(0x42c0, 0xbd00, cmn_reg_t1);

	// cmp_imm_t1: 0010 1<x's>
	register_opcode_mask_16(0x2800, 0xd000, cmp_imm_t1);

	// cmp_reg_t1: 0100 0010 10<x's>
	register_opcode_mask_16(0x4280, 0xbd40, cmp_reg_t1);

	// cmp_reg_t2: 0100 0101 xxxx xxxx
	register_opcode_mask_16(0x4500, 0xba00, cmp_reg_t2);

	// tst_reg_t1: 0100 0010 00xx xxxx
	register_opcode_mask_16(0x4200, 0xbdc0, tst_reg_t1);
}
