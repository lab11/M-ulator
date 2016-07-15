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

#ifndef __DECODE_HELPERS_H
#define __DECODE_HELPERS_H

#include "core/common.h"
#include "core/isa/arm_types.h"

//XXX: in_ITblock; misfiled?
#include "cpu/misc.h"

#define BadReg(_r)	((_r == 13) || (_r == 15))

static inline uint32_t hamming(uint32_t val) {
	return __builtin_popcount(val);
}

#define SRType_ENUM_TO_MASK(_e) (((_e) == 4) ? 3 : _e)

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

uint32_t SignExtend(uint32_t val, uint8_t bits);


#ifdef M_PROFILE

inline __attribute__ ((always_inline))
uint32_t get_isetstate(void) {
	return INST_SET_THUMB;
}

#endif //M_PROFILE

#endif //__DECODE_HELPERS_H
