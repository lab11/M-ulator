/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "helpers.h"

#include "cpu/registers.h"
#include "cpu/core.h"
#include "cpu/misc.h"

void pop(uint16_t registers) {
	uint32_t address = CORE_reg_read(SP_REG);

	int i;
	for (i = 0; i <= 14; i++) {
		if (registers & (1 << i)) {
			CORE_reg_write(i, read_word(address));
			address += 4;
		}
	}

	if (registers & (1 << 15)) {
		LoadWritePC(read_word(address));
	}

	CORE_reg_write(SP_REG, CORE_reg_read(SP_REG) + 4 * hamming(registers));
}
