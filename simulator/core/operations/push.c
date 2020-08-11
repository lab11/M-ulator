/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "helpers.h"

#include "cpu/registers.h"
#include "cpu/core.h"

void push(const uint16_t registers) {
	uint32_t sp = CORE_reg_read(SP_REG);

	uint32_t address = sp - 4 * hamming(registers);

	int i;
	for (i=0; i <= 14; i++) {
		if (registers & (1 << i)) {
			write_word(address, CORE_reg_read(i));
			address += 4;
		}
	}

	CORE_reg_write(SP_REG, sp - 4 * hamming(registers));
}
