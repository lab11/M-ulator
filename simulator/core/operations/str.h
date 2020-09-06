/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

void str_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback);
void str_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n);
void strb_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback);
void strb_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n);
void strd_imm(uint8_t rt, uint8_t rt2, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback);
void strh_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback);
void strh_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n);
