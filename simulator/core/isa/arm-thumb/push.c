/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/push.h"

// arm-thumb
static void push_t1(uint16_t inst) {
	uint8_t register_list = inst & 0xff;
	bool M = (inst >> 8) & 0x1;

	uint16_t registers = register_list | (M << 14);

	if (hamming(registers) < 1)
		CORE_ERR_unpredictable("push_t1 case\n");

	OP_DECOMPILE("PUSH<c> <registers>", registers);
	return push(registers);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_push(void) {
	// 1011 010x <x's>
	register_opcode_mask_16(0xb400, 0x4a00, push_t1);
}
