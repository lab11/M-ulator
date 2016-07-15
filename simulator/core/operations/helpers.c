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

#include "helpers.h"

#include "cpu/exception.h"
#include "cpu/registers.h"

/* From Bit Twiddling Hacks:
 * http://graphics.stanford.edu/~seander/bithacks.html#BitReverseTable */
static const unsigned char BitReverseTable256[256] = {
#   define R2(n)     n,     n + 2*64,     n + 1*64,     n + 3*64
#   define R4(n) R2(n), R2(n + 2*16), R2(n + 1*16), R2(n + 3*16)
#   define R6(n) R4(n), R4(n + 2*4 ), R4(n + 1*4 ), R4(n + 3*4 )
	R6(0), R6(2), R6(1), R6(3)
};

uint32_t reverse_bits(uint32_t v) {
	// reverse 32-bit value, 8 bits at time
	uint32_t c; // c will get v reversed

	// Option 1:
	c = (BitReverseTable256[v & 0xff] << 24) |
		(BitReverseTable256[(v >> 8) & 0xff] << 16) |
		(BitReverseTable256[(v >> 16) & 0xff] << 8) |
		(BitReverseTable256[(v >> 24) & 0xff]);

	return c;
}

/*
// Only supports N == 32 bits
void AddWithCarry(int32_t x, int32_t y, bool carry_in,
		uint32_t *result, bool *carry_out, bool *overflow_out) {
	//uint64_t unsigned_sum = ((uint64_t) x) + ((uint64_t) y) + ((uint64_t) carry_in);
	uint64_t unsigned_sum = ((uint64_t) ((uint32_t) x)) + ((uint64_t) ((uint32_t) y)) + ((uint64_t) carry_in);
	int64_t signed_sum = (((int64_t) x) + ((int64_t) y)) + ((uint64_t) carry_in);

	*result = (uint32_t) unsigned_sum; // 64->32 truncation
	*carry_out    = !(           *result  == unsigned_sum);
	*overflow_out = !(((int32_t) *result) == signed_sum);

	WARN("x %08x, y %08x, carry %d\n", x, y, carry_in);
	WARN("usum %09"PRIx64", ssum %09"PRIx64", result %08x, carry %d, ovflw %d\n",
			unsigned_sum, signed_sum, *result, *carry_out, *overflow_out);
}
*/

void AddWithCarry(uint32_t x, uint32_t y, bool carry_in,
		uint32_t *result, bool *carry_out, bool *overflow_out) {
	uint64_t ux64 = x;
	uint64_t uy64 = y;
	int64_t  sx64 = (int32_t) x;
	int64_t  sy64 = (int32_t) y;

	uint64_t usum = ux64 + uy64 + ((uint64_t) carry_in);
	int64_t  ssum = sx64 + sy64 + (( int64_t) carry_in);

	*result = usum;

	if (*result == usum) *carry_out = 0;
	else                 *carry_out = 1;

	if (*result == ssum) *overflow_out = 0;
	else                 *overflow_out = 1;

	DBG2("x %08x, y %08x, carry %d\n", x, y, carry_in);
	DBG2("usum %09"PRIx64", ssum %09"PRIx64", result %08x, carry %d, ovflw %d\n",
			usum, ssum, *result, *carry_out, *overflow_out);
}

void LoadWritePC(uint32_t addr) {
	BXWritePC(addr);
}

void BXWritePC(uint32_t addr) {
	if ((CORE_CurrentMode_read() == Mode_Handler)
			&& ((addr & 0xf0000000) == 0xf0000000)) {
		exception_return(addr);
	} else {
		BLXWritePC(addr);
	}
}

void BLXWritePC(uint32_t addr) {
	SET_THUMB_BIT(addr & 0x1);
	BranchTo(addr & 0xfffffffe);
}

void BranchTo(uint32_t addr) {
	CORE_reg_write(PC_REG, addr);
}

void BranchWritePC(uint32_t addr) {
	if (get_isetstate() == INST_SET_ARM) {
#ifdef M_PROFILE
		assert(false && "Arm state in M profile?");
#endif
		BranchTo(addr & 0xfffffffc);
	} else if (get_isetstate() == INST_SET_JAZELLE) {
#ifdef M_PROFILE
		assert(false && "Jazelle state in M profile?");
#endif
		CORE_ERR_not_implemented("Jazelle\n");
	} else {
		BranchTo(addr & 0xfffffffe);
	}
}

