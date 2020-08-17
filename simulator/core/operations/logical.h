/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

void and_imm(union apsr_t apsr, uint8_t setflags, uint8_t rd, uint8_t rn,
		uint32_t imm32, uint8_t carry);
void and_reg(uint8_t rd, uint8_t rn, uint8_t rm,
		bool setflags, enum SRType shift_t, uint8_t shift_n);
void bic_imm(union apsr_t apsr, uint8_t setflags,
		uint8_t rd, uint8_t rn, uint32_t imm32, uint8_t carry);
void bic_reg(uint8_t rd, uint8_t rn, uint8_t rm,
		bool setflags, enum SRType shift_t, uint8_t shift_n);
void eor_imm(uint8_t rd, uint8_t rn, uint32_t imm32,
		bool carry, bool setflags, union apsr_t apsr);
void eor_reg(uint8_t setflags, uint8_t rd, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n);
void mvn_imm(uint8_t rd, bool setflags, union apsr_t apsr,
		uint32_t imm32, bool carry);
void mvn_reg(uint8_t setflags, uint8_t rd, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n);
void orn_imm(uint8_t rd, uint8_t rn, bool setflags,
		union apsr_t apsr, uint32_t imm32, bool carry);
void orn_reg(uint8_t rd, uint8_t rn, uint8_t rm,
		bool setflags, enum SRType shift_t, uint8_t shift_n);
void orr_imm(uint8_t rd, uint8_t rn, bool setflags,
		union apsr_t apsr, uint32_t imm32, bool carry);
void orr_reg(uint8_t setflags, uint8_t rd, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n);

