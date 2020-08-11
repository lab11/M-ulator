/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "helpers.h"

#include "cpu/features.h"
#include "cpu/registers.h"
#include "cpu/misc.h"

void mov_imm(union apsr_t apsr, uint8_t setflags, uint32_t imm32, uint8_t rd, uint8_t carry){
	uint32_t result = imm32;

	if (rd == 15) {
		CORE_ERR_not_implemented("A1 case of mov_imm\n");
		//ALUWritePC(result);
	} else {
		CORE_reg_write(rd, result);
		if (setflags) {
			apsr.bits.N = HIGH_BIT(result);
			apsr.bits.Z = result == 0;
			apsr.bits.C = carry;
			CORE_apsr_write(apsr);
		}
	}

	DBG2("mov_imm r%02d = 0x%08x\n", rd, result);
}

void mov_reg(uint8_t rd, uint8_t rm, bool setflags) {
	uint32_t rm_val = CORE_reg_read(rm);

	if (rd == 15) {
		CORE_ERR_not_implemented("mov_reg --> alu\n");
		//ALUWritePC(rm_val);
	} else {
		CORE_reg_write(rd, rm_val);
		if (setflags) {
			union apsr_t apsr = CORE_apsr_read();
			apsr.bits.N = HIGH_BIT(rm_val);
			apsr.bits.Z = rm_val == 0;
			CORE_apsr_write(apsr);
		}
	}

	DBG2("mov_reg r%02d = r%02d (val: %08x)\n", rd, rm, rm_val);
}
