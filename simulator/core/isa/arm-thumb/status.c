/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/status.h"

// arm-v6-m, arm-v7-m
static void cps_t1(uint16_t inst) {
	bool  F = !!(inst & 0x1);
	bool  I = !!(inst & 0x2);
	bool im = !!(inst & 0x10);

	bool enable = im==0;
	bool disable = im==1;
	bool affectPri = I==1;
	bool affectFault = F==1;

	return cps(enable, disable, affectPri, affectFault);
}

// arm-v6-m, arm-v7-m
static void mrs_t1(uint32_t inst) {
	uint8_t SYSm = inst & 0xff;
	uint8_t rd = (inst >> 8) & 0xf;

	return mrs(SYSm, rd);
}

// arm-v6-m, arm-v7-m
static void msr_t1(uint32_t inst) {
	uint8_t SYSm = inst & 0xff;
	uint8_t mask = (inst >> 10) & 0x3;
	uint8_t rn   = (inst >> 16) & 0xf;

	return msr(SYSm, mask, rn);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_status(void) {
	// cps_t1: 1011 0110 011x 00xx
	register_opcode_mask_16(0xb660, 0x498c, cps_t1);

	// mrs_t1: 1111 0011 1110 1111 1100 <x's>
	register_opcode_mask_32(0xf3efc000, 0x0c103000, mrs_t1);

	// msr_t1: 1111 0011 1000 xxxx 1000 xx00 <x's>
	register_opcode_mask_32(0xf3808000, 0x0c707300, msr_t1);
}
