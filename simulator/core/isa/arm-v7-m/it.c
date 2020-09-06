/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/it.h"

// arm-v7-m
static void it_t1(uint16_t inst) {
	uint8_t itstate = inst & 0xff;
	OP_DECOMPILE("IT{x{y{z}}}<q> <firstcond>", itstate); // IT -> special
	return it(inst);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_it(void) {
	// it_t1: 1011 1111 xxxx xxxx
	register_opcode_mask_16_ex(0xbf00, 0x4000, it_t1,
			0x0000, 0x000f,
			0, 0);
}
