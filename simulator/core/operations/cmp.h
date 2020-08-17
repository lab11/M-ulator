/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

void cmn_imm(uint8_t rn, uint32_t imm32);
void cmn_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n);
void cmp_imm(uint8_t rn, uint32_t imm32);
void cmp_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n);
void teq_imm(uint8_t rn, uint32_t imm32, bool carry);
void teq_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n);
void tst_imm(uint8_t rn, uint32_t imm32, bool carry);
void tst_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n);
