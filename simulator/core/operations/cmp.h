/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2012  Pat Pannuto <pat.pannuto@gmail.com>
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

void cmn_imm(uint8_t rn, uint32_t imm32);
void cmn_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n);
void cmp_imm(uint8_t rn, uint32_t imm32);
void cmp_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n);
void teq_imm(uint8_t rn, uint32_t imm32, bool carry);
void teq_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n);
void tst_imm(uint8_t rn, uint32_t imm32, bool carry);
void tst_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n);
