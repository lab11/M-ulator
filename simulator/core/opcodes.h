/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef OPCODES_H
#define OPCODES_H

#include "common.h"

struct op {
	const char *name;
	bool is16;
	union {
		struct {
			uint32_t ones_mask;
			uint32_t zeros_mask;
			int ex_cnt;
			uint32_t *ex_ones;
			uint32_t *ex_zeros;
			void (*fn) (uint32_t);
		} op32;
		struct {
			uint16_t ones_mask;
			uint16_t zeros_mask;
			int ex_cnt;
			uint16_t *ex_ones;
			uint16_t *ex_zeros;
			void (*fn) (uint16_t);
		} op16;
	};
};

int get_opcode_masks(void);
bool match_mask8(uint8_t inst, uint8_t ones_mask, uint8_t zeros_mask) __attribute__((const));
bool match_mask16(uint16_t inst, uint16_t ones_mask, uint16_t zeros_mask) __attribute__((const));
bool match_mask32(uint32_t inst, uint32_t ones_mask, uint32_t zeros_mask) __attribute__((const));
struct op* find_op(uint32_t inst) __attribute__((pure));

void opcode_statistics(void);

#endif //OPCODES_H
