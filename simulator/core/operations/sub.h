/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

void rsb_imm(uint8_t rd, uint8_t rn, uint32_t imm32, bool setflags);
void rsb_reg(uint8_t rd, uint8_t rn, uint8_t rm, bool setflags,
		enum SRType shift_t, uint8_t shift_n);
void sbc_imm(uint8_t rd, uint8_t rn, bool setflags, uint32_t imm32);
void sbc_reg(uint8_t rd, uint8_t rn, uint8_t rm, bool setflags,
		enum SRType shift_t, uint8_t shift_n);
void sub_imm(uint8_t rd, uint8_t rn, uint32_t imm32, bool setflags);
void sub_reg(uint8_t rd, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n, bool setflags);
void sub_sp_imm(uint8_t rd, uint32_t imm32, bool setflags);
void sub_sp_reg(uint8_t rd, uint8_t rm, bool setflags,
		enum SRType shift_t, uint8_t shift_n);
