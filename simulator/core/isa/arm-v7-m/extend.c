/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/extend.h"

// arm-v7-m
static void sxtb_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t rotate = (inst >> 4) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;

	uint8_t rotation = rotate << 3;

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("sxtb_t2 case\n");

	return sxtb(rd, rm, rotation);
}

// arm-v7-m
static void sxth_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t rotate = (inst >> 4) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;

	uint8_t rotation = rotate << 3;

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("sxth_t2 case\n");

	return sxth(rd, rm, rotation);
}

// arm-v7-m
static void uxtb_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t rotate = (inst >> 4) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;

	uint8_t rotation = rotate << 3;

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("uxtb_t2 case\n");

	return uxtb(rd, rm, rotation);
}

// arm-v7-m
static void uxth_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t rotate = (inst >> 4) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;

	uint8_t rotation = rotate << 3;

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("uxth_t2 case\n");

	return uxth(rd, rm, rotation);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_extend(void) {
	// sxtb_t2: 1111 1010 0100 1111 1111 xxxx 10xx xxxx
	register_opcode_mask_32(0xfa4ff080, 0x05b00040, sxtb_t2);

	// sxth_t2: 1111 1010 0000 1111 1111 xxxx 10xx xxxx
	register_opcode_mask_32(0xfa0ff080, 0x05f00040, sxth_t2);

	// uxtb_t2: 1111 1010 0101 1111 1111 xxxx 10xx xxxx
	register_opcode_mask_32(0xfa5ff080, 0x05a00040, uxtb_t2);

	// uxth_t2: 1111 1010 0001 1111 1111 xxxx 10xx xxxx
	register_opcode_mask_32(0xfa1ff080, 0x05e00040, uxth_t2);
}

