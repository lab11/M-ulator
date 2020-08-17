/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

void shift_imm(union apsr_t apsr, uint8_t setflags, uint8_t rd, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n);
void shift_imm_t1(uint16_t inst, enum SRType shift_t);
void shift_imm_t2(uint32_t inst, enum SRType shift_t);
void shift_reg(uint8_t rd, uint8_t rn, uint8_t rm,
		enum SRType shift_t, bool setflags);
void shift_reg_t1(uint16_t inst, enum SRType shift_t);
void shift_reg_t2(uint32_t inst, enum SRType shift_t);
void rrx(uint8_t rm, uint8_t rd, bool setflags);

