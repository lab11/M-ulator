/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/misc.h"

// arm-v6-m, arm-v7-m
static void rev_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	OP_DECOMPILE("REV<c> <Rd>,<Rm>", rd, rm);
	return rev(rd, rm);
}

// arm-v6-m, arm-v7-m
static void rev16_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	OP_DECOMPILE("REV16<c> <Rd>,<Rm>", rd, rm);
	return rev16(rd, rm);
}

// arm-v6-m, arm-v7-m
static void revsh_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	OP_DECOMPILE("REVSH<c> <Rd>,<Rm>", rd, rm);
	return revsh(rd, rm);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_misc(void) {
	// rev_t1: 1011 1010 00xx xxxx
	register_opcode_mask_16(0xba00, 0x45c0, rev_t1);

	// rev16_t1: 1011 1010 01xx xxxx
	register_opcode_mask_16(0xba40, 0x4580, rev16_t1);

	// revsh_t1: 1011 1010 11xx xxxx
	register_opcode_mask_16(0xbac0, 0x4500, revsh_t1);
}
