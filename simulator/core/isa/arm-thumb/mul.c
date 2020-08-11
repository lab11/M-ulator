/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/mul.h"

// arm-thumb
static void mul_t1(uint16_t inst) {
	uint8_t rdm = inst & 0x7;
	uint8_t rn = inst & 0x7;

	bool setflags = !in_ITblock();

	OP_DECOMPILE("MUL<IT> <Rdm>,<Rn>,<Rdm>", rdm, rn, rdm);
	return mul(setflags, rdm, rn, rdm);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_mul(void) {
	// mul_t1: 0100 0011 01xx xxxx
	register_opcode_mask_16(0x4340, 0xbc80, mul_t1);
}
