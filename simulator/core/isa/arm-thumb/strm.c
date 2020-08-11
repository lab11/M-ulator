/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/strm.h"

// arm-thumb
static void stm_t1(uint16_t inst) {
	uint8_t register_list = inst & 0xff;
	uint8_t rn = (inst >> 8) & 0x7;

	uint16_t registers = register_list;
	bool wback = true;

	if (hamming(registers) < 1)
		CORE_ERR_unpredictable("stm_t1 case\n");

	OP_DECOMPILE("STM<c> <Rn>!,<registers>", rn, registers);
	return stm(rn, registers, wback);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_strm(void) {
	// stm_t1: 1100 0<x's>
	register_opcode_mask_16(0xc000, 0x3800, stm_t1);
}
