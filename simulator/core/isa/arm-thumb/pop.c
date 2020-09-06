/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/pop.h"

// arm-thumb
static void pop_t1(uint16_t inst) {
	bool P = inst & 0x100;
	uint16_t registers = (inst & 0xff) | (P << 15);

	if (hamming(registers) == 0) {
		CORE_ERR_unpredictable("pop_t1 with no registers");
	}

	OP_DECOMPILE("POP<c> <registers>", registers);
	return pop(registers);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_pop(void) {
	// pop_t1: 1011 110<x's>
	register_opcode_mask_16(0xbc00, 0x4200, pop_t1);
}
