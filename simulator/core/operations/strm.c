/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "helpers.h"

#include "cpu/registers.h"
#include "cpu/core.h"

void stmdb(uint8_t rn, uint16_t registers, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t address = rn_val - 4 * hamming(registers);

	int i;
	for (i=0; i <= 14; i++) {
		if (registers & (1 << i)) {
			write_word(address, CORE_reg_read(i));
			address += 4;
		}
	}

	if (wback) {
		CORE_reg_write(rn, rn_val - 4 * hamming(registers));
	}
}

void stm(uint8_t rn, uint16_t registers, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t address = rn_val;

	int i;
	for (i = 0; i <= 14; i++) {
		if (registers & (1 << i)) {
			if ((i == rn) && wback) {
				CORE_ERR_not_implemented("complicated case\n");
			} else {
				write_word(address, CORE_reg_read(i));
			}
			address += 4;
		}
	}

	if (wback)
		CORE_reg_write(rn, rn_val + 4*hamming(registers));
}
