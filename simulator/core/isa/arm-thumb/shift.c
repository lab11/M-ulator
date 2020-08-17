/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/shift.h"

// arm-thumb
static void asr_imm_t1(uint16_t inst) {
	return shift_imm_t1(inst, ASR);
}

// arm-thumb
static void lsl_imm_t1(uint16_t inst) {
	return shift_imm_t1(inst, LSL);
}

// arm-thumb
static void lsr_imm_t1(uint16_t inst) {
	return shift_imm_t1(inst, LSR);
}

// arm-thumb
static void asr_reg_t1(uint16_t inst) {
	return shift_reg_t1(inst, SRType_ASR);
}

// arm-thumb
static void lsl_reg_t1(uint16_t inst) {
	return shift_reg_t1(inst, SRType_LSL);
}

// arm-thumb
static void lsr_reg_t1(uint16_t inst) {
	return shift_reg_t1(inst, SRType_LSR);
}

// arm-thumb
static void ror_reg_t1(uint16_t inst) {
	return shift_reg_t1(inst, SRType_ROR);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_shift(void) {
	// asr_imm_t1: 0001 0xxx xxxx xxxx
	register_opcode_mask_16(0x1000, 0xe800, asr_imm_t1);

	// lsl_imm_t1: 0000 0<x's>
	register_opcode_mask_16_ex(0x0000, 0xf800, lsl_imm_t1,
			0x0, 0x7c0,
			0, 0);

	// lsr_imm_t1: 0000 1<x's>
	register_opcode_mask_16(0x0800, 0xf000, lsr_imm_t1);

	// asr_reg_t1: 0100 0001 00xx xxxx
	register_opcode_mask_16(0x4100, 0xbec0, asr_reg_t1);

	// lsl_reg_t1: 0100 0000 10 <x's>
	register_opcode_mask_16(0x4080, 0xbf40, lsl_reg_t1);

	// lsr_reg_t1: 0100 0000 11 <x's>
	register_opcode_mask_16(0x40c0, 0xbf00, lsr_reg_t1);

	// ror_reg_t1: 0100 0001 11<x's>
	register_opcode_mask_16(0x41c0, 0xbe00, ror_reg_t1);
}
