/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/mov.h"

/* If <rd> and <rm> both in R0-R7 then all thumb */
// arm-v6-m, arm-v7-m, arm-thumb*
static void mov_reg_t1(uint16_t inst) {
	uint8_t rd =  (inst & 0x7);
	uint8_t rm =  (inst & 0x78) >> 3;
	uint8_t D = !!(inst & 0x80);

	// d = UInt(D:rd)
	rd |= (D << 3);

	// m = UInt(rm); unecessary

	uint8_t setflags = false;

	if ((rd == 15) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("mov_reg_t1 unpredictable\n");

	OP_DECOMPILE("MOV<c> <Rd>,<Rm>", rd, rm);
	return mov_reg(rd, rm, setflags);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v6_m_mov(void) {
	// This is a weird corner case
	// mov_reg_t1: 0100 0110 xx<x's> <-- arm-v6-m, arm-v7-m
	// mov_reg_t1: 0100 0110 00<x's> <-- arm-thumb
	register_opcode_mask_16_ex(0x4600, 0xb9c0, mov_reg_t1,
			0x00, 0xc0,
			0, 0);
}
