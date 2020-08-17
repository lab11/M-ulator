/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "helpers.h"

#include "cpu/registers.h"
#include "cpu/misc.h"

void cbz(bool nonzero, uint8_t rn, uint32_t imm32) {
	uint32_t rn_val = CORE_reg_read(rn);

	if (nonzero ^ (rn_val == 0))
		BranchWritePC(CORE_reg_read(PC_REG) + imm32);
}

