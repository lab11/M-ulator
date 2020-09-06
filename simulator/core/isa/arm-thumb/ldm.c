/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/ldm.h"

// arm-thumb
static void ldm_t1(uint16_t inst) {
	uint8_t registers = inst & 0xff;
	uint8_t rn = (inst >> 8) & 0x7;

	bool wback = registers & (1 << rn);

	if (hamming(registers) < 1)
		CORE_ERR_unpredictable("ldm_t1 load no regs?\n");

	return ldm(rn, registers, wback);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_ldm(void) {
	// ldm_t1: 1100 1<x's>
	register_opcode_mask_16(0xc800, 0x3000, ldm_t1);
}

