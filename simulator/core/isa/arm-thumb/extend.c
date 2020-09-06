/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/extend.h"

// arm-v6-m, arm-v7-m
static void sxtb_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	uint8_t rotation = 0;

	return sxtb(rd, rm, rotation);
}

// arm-v6-m, arm-v7-m
static void sxth_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	uint8_t rotation = 0;
	return sxth(rd, rm, rotation);
}

// arm-v6-m, arm-v7-m
static void uxtb_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst & 0x38) >> 3;

	uint8_t rotation = 0;

	OP_DECOMPILE("UXTB<c> <Rd>,<Rm>", rd, rm);
	return uxtb(rd, rm, rotation);
}

// arm-v6-m, arm-v7-m
static void uxth_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	uint8_t rotation = 0;
	return uxth(rd, rm, rotation);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_extend(void) {
	// sxtb_t1: 1011 0010 01xx xxxx
	register_opcode_mask_16(0xb240, 0x4d80, sxtb_t1);

	// sxth_t1: 1011 0010 00xx xxxx
	register_opcode_mask_16(0xb200, 0x4dc0, sxth_t1);

	// uxtb_t1: 1011 0010 11<x's>
	register_opcode_mask_16(0xb2c0, 0x4d00, uxtb_t1);

	// uxth_t1: 1011 0010 10xx xxxx
	register_opcode_mask_16(0xb280, 0x4d40, uxth_t1);
}

