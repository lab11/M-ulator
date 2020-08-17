/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "helpers.h"

#include "cpu/registers.h"
#include "cpu/exception.h"

//#include "cpu/common/private_peripheral_bus/ppb.h"
//#define DIV_0_TRP (read_word(CONFIGURATION_CONTROL) & CONFIGURATION_CONTROL_DIV_0_TRAP_MASK)
#define DIV_0_TRP 1

void sdiv(uint8_t rd, uint8_t rn, uint8_t rm) {
	int32_t rn_val = CORE_reg_read(rn);
	int32_t rm_val = CORE_reg_read(rm);

	int32_t result;

	if (rm_val == 0) {
		if (DIV_0_TRP)
			return UsageFault_divide_by_0();
		else
			result = 0;
	} else {
		result = rn_val / rm_val;
	}

	CORE_reg_write(rd, result);
}

void udiv(uint8_t rd, uint8_t rn, uint8_t rm) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t result;

	if (rm_val == 0) {
		if (DIV_0_TRP)
			return UsageFault_divide_by_0();
		else
			result = 0;
	} else {
		result = rn_val / rm_val;
	}

	CORE_reg_write(rd, result);
}

