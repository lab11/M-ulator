/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/shift.h"

// arm-v7-m
static void ror_imm_t1(uint32_t inst) {
	return shift_imm_t1(inst, ROR);
}

// arm-v7-m
static void asr_imm_t2(uint32_t inst) {
	return shift_imm_t2(inst, ASR);
}

// arm-v7-m
static void lsl_imm_t2(uint32_t inst) {
	return shift_imm_t2(inst, LSL);
}

// arm-v7-m
static void lsr_imm_t2(uint32_t inst) {
	return shift_imm_t2(inst, LSR);
}

// arm-v7-m
static void asr_reg_t2(uint32_t inst) {
	return shift_reg_t2(inst, SRType_ASR);
}

// arm-v7-m
static void lsl_reg_t2(uint32_t inst) {
	return shift_reg_t2(inst, SRType_LSL);
}

// arm-v7-m
static void lsr_reg_t2(uint32_t inst) {
	return shift_reg_t2(inst, SRType_LSR);
}

// arm-v7-m
static void ror_reg_t2(uint32_t inst) {
	return shift_reg_t2(inst, SRType_ROR);
}

// arm-v7-m
static void rrx_t1(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t rd = (inst >> 8) & 0xf;
	bool    S  = (inst >> 20) & 0x1;

	bool setflags = S==1;

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("rrx_t1 case\n");

	OP_DECOMPILE("RRX{S}<c> <Rd>,<Rm>", setflags, rd, rm);
	return rrx(rm, rd, setflags);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_shift(void) {
	// asr_imm_t2: 1110 1010 010x 1111 0xxx xxxx xx10 xxxx
	register_opcode_mask_32(0xea4f0020, 0x15a08010, asr_imm_t2);

	// lsl_imm_t2: 1110 1010 010x 1111 0xxx xxxx xx00 xxxx
	register_opcode_mask_32(0xea4f0000, 0x15a08030, lsl_imm_t2);

	// lsr_imm_t2: 1110 1010 010x 1111 0xxx xxxx xx01 xxxx
	register_opcode_mask_32(0xea4f0010, 0x15a08020, lsr_imm_t2);

	// ror_imm_t1: 1110 1010 010x 1111 0xxx xxxx xx11 xxxx
	register_opcode_mask_32_ex(0xea4f0030, 0x15a08000, ror_imm_t1,
			0x0, 0x70c0,
			0, 0);

	// asr_reg_t2: 1111 1010 010x xxxx 1111 xxxx 0000 xxxx
	register_opcode_mask_32(0xfa40f000, 0x05a000f0, asr_reg_t2);

	// lsl_reg_t2: 1111 1010 000x xxxx 1111 xxxx 0000 xxxx
	register_opcode_mask_32(0xfa00f000, 0x05e000f0, lsl_reg_t2);

	// lsr_reg_t2: 1111 1010 001x xxxx 1111 xxxx 0000 xxxx
	register_opcode_mask_32(0xfa20f000, 0x05c000f0, lsr_reg_t2);

	// ror_reg_t2: 1111 1010 011x xxxx 1111 xxxx 0000 xxxx
	register_opcode_mask_32(0xfa60f000, 0x058000f0, ror_reg_t2);

	// rrx_t1: 1110 1010 010x 1111 0000 xxxx 0011 xxxx
	register_opcode_mask_32(0xea4f0030, 0x15a0f0c0, rrx_t1);
}
