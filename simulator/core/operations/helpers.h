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

#ifndef __HELPERS_H
#define __HELPERS_H

#define INST_SET_ARM		0x0
#define INST_SET_THUMB		0x1
#define INST_SET_JAZELLE	0x2
#define INST_SET_THUMBEE	0x3

#define HIGH_BIT(_r) (!!(_r & 0x80000000))

static inline uint32_t hamming(uint32_t val) {
	return __builtin_popcount(val);
}

void AddWithCarry(uint32_t x, uint32_t y, bool carry_in,
		uint32_t *result, bool *carry_out, bool *overflow_out)
		__attribute__ ((nonnull));

uint32_t SignExtend(uint32_t val, uint8_t bits);

void LoadWritePC(uint32_t addr);
void BXWritePC(uint32_t addr);
void BLXWritePC(uint32_t addr);
void BranchTo(uint32_t addr);
void BranchWritePC(uint32_t addr);

#define BadReg(_r)	((_r == 13) || (_r == 15))

#define SRType_LSL	0x0
#define SRType_LSR	0x1
#define SRType_ASR	0x2
#define SRType_RRX	0x3
#define SRType_ROR	0x3

#define SRType_ENUM_TO_MASK(_e) (((_e) == 4) ? 3 : _e)

enum SRType {
	LSL,	// match
	LSR,	// match
	ASR,	// match
	RRX,	// match
	ROR,	// +1
};

// Legal range of shift is 0..31, uint8_t forces callers to be at least somewhat concious of this
void DecodeImmShift(uint8_t type, uint8_t imm5,
		enum SRType *shift_t, uint8_t *shift_n)
		__attribute__ ((nonnull));
void Shift_C(uint32_t value, int Nbits,
		enum SRType type, uint8_t amount, bool carry_in,
		uint32_t *result, bool *carry_out)
		__attribute__((nonnull));
uint32_t Shift(uint32_t value, int Nbits,
		enum SRType type, uint8_t amount, bool carry_in);

// Legal range of shift is 0..31, uint8_t forces callers to be at least somewhat concious of this
void LSL_C(uint32_t x, int Nbits, uint8_t shift, uint32_t *result, bool *carry_out);
void LSR_C(uint32_t x, int Nbits, uint8_t shift, uint32_t *result, bool *carry_out);
void ASR_C(uint32_t x, int Nbits, uint8_t shift, uint32_t *result, bool *carry_out);
void ROR_C(uint32_t x, int Nbits, uint8_t shift, uint32_t *result, bool *carry_out);
uint32_t ThumbExpandImm(uint32_t imm12);
void ThumbExpandImm_C(uint32_t imm12, bool carry_in, uint32_t *imm32, bool *carry_out);

#ifdef M_PROFILE

#define SET_THUMB_BIT(_t) \
	do {\
		union epsr_t epsr = CORE_epsr_read();\
		epsr.bits.T = _t;\
		CORE_epsr_write(epsr);\
	} while (0)

#define SET_ISETSTATE(_i) // NOP for M profile
#define GET_ISETSTATE	INST_SET_THUMB

#endif //M_PROFILE

#endif // __HELPERS_H
