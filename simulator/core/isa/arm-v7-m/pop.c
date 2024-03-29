/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/pop.h"

// arm-v7-m
static void pop_t2(uint32_t inst) {
	uint16_t reg_list = inst & 0x1fff;
	uint8_t M = !!(inst & 0x4000);
	uint8_t P = !!(inst & 0x8000);

	uint16_t registers = (P << 15) | (M << 14) | reg_list;

	DBG2("P %d M %d reg_list %04x, registers %04x\n", P, M, reg_list, registers);

	if ((hamming(registers) < 2) || ((P == 1) && (M == 1)))
		CORE_ERR_unpredictable("pop_t2 not enough regs\n");

	if ((registers & 0x8000) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("pop_t2 itstate\n");

	OP_DECOMPILE("POP<c>.W <registers>", registers);
	return pop(registers);
}

// arm-v7-m
static void pop_t3(uint32_t inst) {
	uint8_t rt = (inst >> 12) & 0xf;
	uint16_t registers = (1 << rt);

	if ((rt == 13) || ((rt == 15) && in_ITblock() && !last_in_ITblock()))
		CORE_ERR_unpredictable("bad reg\n");

	OP_DECOMPILE("POP<c>.W <registers>", registers);
	return pop(registers);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_pop(void) {
	// pop_t2: 1110 1000 1011 1101 xx0<x's>
	register_opcode_mask_32(0xe8bd0000, 0x17422000, pop_t2);

	// pop_t3: 1111 1000 0101 1101 xxxx 1011 0000 0100
	register_opcode_mask_32(0xf85d0b04, 0x07a204fb, pop_t3);
}
