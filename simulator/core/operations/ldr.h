/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

void ldr_imm(uint8_t rt, uint8_t rn, uint32_t imm32, bool index, bool add, bool wback);
void ldr_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback,
		enum SRType shift_t, uint8_t shift_n);
void ldr_lit(bool add, uint8_t rt, uint32_t imm32);
void ldrb_imm(uint8_t rt, uint8_t rn, uint32_t imm32, bool add, bool index, bool wback);
void ldrb_lit(uint8_t rt, uint32_t imm32, bool add);
void ldrb_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n,
		bool index, bool add, bool wback);
void ldrd_imm(uint8_t rt, uint8_t rt2, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback);
void ldrd_lit(uint8_t rt, uint8_t rt2, uint32_t imm32, bool add);
void ldrh_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback);
void ldrh_lit(uint8_t rt, uint32_t imm32, bool add);
void ldrh_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback,
		enum SRType shift_t, uint8_t shift_n);
void ldrsb_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback);
void ldrsb_lit(uint8_t rt, uint32_t imm32, bool add);
void ldrsb_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback,
		enum SRType shift_t, uint8_t shift_n);
void ldrsh_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback);
void ldrsh_lit(uint8_t rt, uint32_t imm32, bool add);
void ldrsh_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback,
		enum SRType shift_t, uint8_t shift_n);

