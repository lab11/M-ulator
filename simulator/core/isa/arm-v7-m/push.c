/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/push.h"

// arm-v7-m
static void push_t2(uint32_t inst) {
	uint16_t register_list = inst & 0x1fff;
	bool M = (inst >> 14) & 0x1;

	uint16_t registers = (M << 14) | register_list;

	if (hamming(registers) < 2)
		CORE_ERR_unpredictable("Too few regs to push\n");

	OP_DECOMPILE("PUSH<c> <registers>", registers);
	push(registers);
}

// arm-v7-m
static void push_t3(uint32_t inst) {
	uint8_t rt = (inst >> 12) & 0xf;
	uint16_t registers = (1 << rt);

	if (BadReg(rt))
		CORE_ERR_unpredictable("bad reg\n");

	OP_DECOMPILE("PUSH<c> <registers>", registers);
	push(registers);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_push(void) {
	// 1110 1001 0010 1101 0x0x xxxx xxxx xxxx
	register_opcode_mask_32(0xe92d0000, 0x16d2a000, push_t2);

	// 1111 1000 0100 1101 xxxx 1101 0000 0100 (t3)
	register_opcode_mask_32(0xf84d0d04, 0x07b202fb, push_t3);
}
