/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

void adc_imm(uint8_t rd, uint8_t rn, uint32_t imm32, bool setflags);
void adc_reg(uint8_t rd, uint8_t rn, uint8_t rm,
		bool setflags, enum SRType shift_t, uint8_t shift_n);
void add_imm(uint8_t rn, uint8_t rd, uint32_t imm32, uint8_t setflags);
void add_reg(uint8_t rd, uint8_t rn, uint8_t rm, bool setflags,
		enum SRType shift_t, uint8_t shift_n);
void add_sp_plus_imm(uint8_t rd, uint32_t imm32, bool setflags);
void add_sp_plus_reg(uint8_t rd, uint8_t rm,
		bool setflags, enum SRType shift_t, uint8_t shift_n);
void adr(uint8_t rd, bool add, uint32_t imm32);
