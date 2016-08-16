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

void bfc(uint8_t rd, uint8_t msbit, uint8_t lsbit);
void bfi(uint8_t rd, uint8_t rn, uint8_t msbit, uint8_t lsbit);
void clz(uint8_t rd, uint8_t rm);
void movt(uint8_t rd, uint16_t imm16);
void rbit(uint8_t rm, uint8_t rd);
void rev(uint8_t rd, uint8_t rm);
void rev16(uint8_t rd, uint8_t rm);
void revsh(uint8_t rd, uint8_t rm);
void sbfx(uint8_t rd, uint8_t rn, uint8_t lsbit, uint8_t widthminus1);
void ubfx(uint8_t rd, uint8_t rn, uint8_t lsbit, uint8_t widthminus1);

