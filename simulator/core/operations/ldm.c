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

void ldm(uint8_t rn, uint16_t registers, bool wback) {
	uint32_t address = CORE_reg_read(rn);

	int i;
	for (i = 0; i <= 14; i++) {	// stupid arm inclusive for
		if (registers & (1 << i)) {
			CORE_reg_write(i, read_word(address));
			address += 4;
		}
	}
	if (registers & 0x8000) {
		//LoadWritePC(MemA[address, 4]);
		CORE_ERR_not_implemented("ldm PC");
	}

	if (wback && ((registers & (1 << rn)) == 0)) {
		CORE_reg_write(rn, CORE_reg_read(rn) + 4U*hamming(registers));
	}
}

void ldmdb(uint8_t rn, uint16_t registers, bool wback) {
	uint32_t address = CORE_reg_read(rn) - 4*hamming(registers);

	int i;
	for (i=0;i<14;i++) {
		if (registers & (1 << i)) {
			CORE_reg_write(i, read_word(address));
			address += 4;
		}
	}
	if (registers & (1 << 15)) {
		//LoadWritePC(MemA[address, 4]);
		CORE_ERR_not_implemented("ldmdb PC");
	}

	if (wback && ((registers & (1 << rn)) == 0)) {
		CORE_reg_write(rn, CORE_reg_read(rn) - 4U*hamming(registers));
	}
}
