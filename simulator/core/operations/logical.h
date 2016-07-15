/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2016  Pat Pannuto <pat.pannuto@gmail.com>
 *
 * This file is part of Mulator.
 *
 * Mulator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mulator.  If not, see <http://www.gnu.org/licenses/>.
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

