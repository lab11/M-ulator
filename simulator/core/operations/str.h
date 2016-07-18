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
