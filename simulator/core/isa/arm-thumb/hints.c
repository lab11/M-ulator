/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/hints.h"

// arm-v6-m, arm-v7-m
static void sev_t1(uint16_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("SEV<c>");
	return sev();
}

// arm-v6-m, arm-v7-m
static void wfe_t1(uint16_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("WFE<c>");
	return wfe();
}

// arm-v6-m, arm-v7-m
static void wfi_t1(uint16_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("WFI<c>");
	return wfi();
}

// arm-v6-m, arm-v7-m
static void yield_t1(uint16_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("YIELD<c>");
	return yield();
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_hints(void) {
	// sev_t1: 1011 1111 0100 0000
	register_opcode_mask_16(0xbf40, 0x40bf, sev_t1);

	// wfe_t1: 1011 1111 0010 0000
	register_opcode_mask_16(0xbf20, 0x40df, wfe_t1);

	// wfi_t1: 1011 1111 0011 0000
	register_opcode_mask_16(0xbf30, 0x40cf, wfi_t1);

	// yield_t1: 1011 1111 0001 0000
	register_opcode_mask_16(0xbf10, 0x40ef, yield_t1);
}

