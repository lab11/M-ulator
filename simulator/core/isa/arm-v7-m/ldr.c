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

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/ldr.h"

// arm-v7-m
static void ldr_imm_t3(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm12;
	bool index = true;
	bool add = true;
	bool wback = false;

	if ((rt == 15) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("invalid ldr imm to pc\n");

	OP_DECOMPILE("LDR<c>.W <Rt>,[<Rn>{,#<imm12>}]", rt, rn, imm32);
	ldr_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void ldr_imm_t4(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	bool W = !!(inst & 0x100);
	bool U = !!(inst & 0x200);
	bool P = !!(inst & 0x400);
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm8;
	bool index = P;
	bool add = U;
	bool wback = W;

	if ((wback && (rn == rt)) || ((rt == 15) && in_ITblock() && !last_in_ITblock()))
		CORE_ERR_unpredictable("bad regs\n");

	if (index && !wback) // Offset
		OP_DECOMPILE("LDR<c> <Rt>, [<Rn> {, #+/-<imm>}]", rt, rn, add, imm32);
	else if (index && wback) // Pre-indexed
		OP_DECOMPILE("LDR<c> <Rt>, [<Rn>, #+/-<imm>]!", rt, rn, add, imm32);
	else if (!index && wback) // Post-indexed
		OP_DECOMPILE("LDR<c> <Rt>, [<Rn>], #+/-<imm>", rt, rn, add, imm32);
	else
		OP_DECOMPILE("!!Error decoding LDR? Illegal combination!!");
	return ldr_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void ldr_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t imm2 = (inst >> 4) & 0x3;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	bool index = true;
	bool add = true;
	bool wback = false;

	enum SRType shift_t = LSL;
	uint8_t shift_n = imm2;

	if (BadReg(rm))
		CORE_ERR_unpredictable("bad reg\n");

	if ((rt == 15) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("it probs\n");

	OP_DECOMPILE("LDR<c>.W <Rt>,[<Rn>,<Rm>{,LSL #<imm2>}]",
			rt, rn, rm, imm2);
	return ldr_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
}

// arm-v7-m
static void ldr_lit_t2(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst & 0xf000) >> 12;
	bool U = !!(inst & 0x00800000);

	if ((rt == 15) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("ldr_lit_t2 PC in IT\n");

	uint32_t imm32 = imm12;
	bool add = U==1;

	OP_DECOMPILE("LDR<c> <Rt>, [PC, #+/-<imm>]", rt, add, imm32);
	return ldr_lit(add, rt, imm32);
}

// arm-v7-m
static void ldrb_imm_t2(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm12;
	bool index = true;
	bool add = true;
	bool wback = false;

	if (rt == 13)
		CORE_ERR_unpredictable("reg 13 not allowed\n");

	OP_DECOMPILE("LDRB<c>.W <Rt>,[<Rn>{,#<imm12>}]", rt, rn, imm12);
	return ldrb_imm(rt, rn, imm32, add, index, wback);
}

// arm-v7-m
static void ldrb_imm_t3(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	bool W = !!(inst & 0x100);		// wback
	bool U = !!(inst & 0x200);		// add
	bool P = !!(inst & 0x400);		// index
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm8;
	bool index = P;
	bool add = U;
	bool wback = W;

	if ((rt == 13) && (wback && (rn == rt)))
		CORE_ERR_unpredictable("bad regs\n");

	if ((rt == 15) && ((index == 0) || (add == 1) || (wback == 1)))
		CORE_ERR_unpredictable("bad regs / flags\n");

	if (index && !wback) // Offset
		OP_DECOMPILE("LDRB<c> <Rt>, [<Rn> {, #+/-<imm>}]", rt, rn, add, imm32);
	else if (index && wback) // Pre-indexed
		OP_DECOMPILE("LDRB<c> <Rt>, [<Rn>, #+/-<imm>]!", rt, rn, add, imm32);
	else if (!index && wback) // Post-indexed
		OP_DECOMPILE("LDRB<c> <Rt>, [<Rn>], #+/-<imm>", rt, rn, add, imm32);
	else
		OP_DECOMPILE("!!Error decoding LDR? Illegal combination!!");
	return ldrb_imm(rt, rn, imm32, add, index, wback);
}

// arm-v7-m
static void ldrb_lit_t1(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	bool U = (inst >> 23) & 0x1;

	uint32_t imm32 = imm12;
	bool add = U == 1;

	if (rt == 13)
		CORE_ERR_unpredictable("ldrb_lit_t1 case\n");

	OP_DECOMPILE("LDRB<c> <Rt>,[PC,#+/-<imm>]", rt, add, imm32);
	return ldrb_lit(rt, imm32, add);
}

// arm-v7-m
static void ldrb_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t imm2 = (inst >> 4) & 0x3;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	bool index = true;
	bool add = true;
	bool wback = false;

	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = imm2;

	if ((rt == 13) || BadReg(rm))
		CORE_ERR_unpredictable("bad reg\n");

	OP_DECOMPILE("LDRB<c>.W <Rt>,[<Rn>,<Rm>{,LSL #<imm2}]",
			rt, rn, rm, imm2);
	return ldrb_reg(rt, rn, rm, shift_t, shift_n, index, add, wback);
}

// arm-v7-m
static void ldrd_imm_t1(uint32_t inst) {
	uint32_t imm8 = (inst & 0xff);
	uint8_t rt2 = (inst & 0xf00) >> 8;
	uint8_t rt = (inst & 0xf000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t W = !!(inst & 0x200000);
	uint8_t U = !!(inst & 0x800000);
	uint8_t P = !!(inst & 0x1000000);

	uint32_t imm32 = imm8 << 2;
	bool index = P;
	bool add = U;
	bool wback = W;

	if (wback && ((rn == rt) || (rn == rt2)))
		CORE_ERR_unpredictable("ldrd_imm_t1 wbck + regs\n");

	if ((rt == 13) || (rt == 15) || (rt2 == 13) || (rt2 == 15) || (rt == rt2))
		CORE_ERR_unpredictable("ldrd_imm_t1 bad regs\n");

	if (index && !wback) // Offset
		OP_DECOMPILE("LDRD<c> <Rt>,<Rt2>,[<Rn> {, #+/-<imm>}]",
				rt, rt2, rn, add, imm32);
	else if (index && wback) // Pre-indexed
		OP_DECOMPILE("LDRD<c> <Rt>,<Rt2>,[<Rn>, #+/-<imm>]!",
				rt, rt2, rn, add, imm32);
	else if (!index && wback) // Post-indexed
		OP_DECOMPILE("LDRD<c> <Rt>,<Rt2>,[<Rn>], #+/-<imm>",
				rt, rt2, rn, add, imm32);
	else
		OP_DECOMPILE("!!Error decoding LDR? Illegal combination!!");

	return ldrd_imm(rt, rt2, rn, imm32, index, add, wback);
}

// arm-v7-m
static void ldrd_lit_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rt2  = (inst >> 8) & 0xf;
	uint8_t rt   = (inst >> 12) & 0xf;
	bool    W    = (inst >> 22) & 0x1;
	bool    U    = (inst >> 24) & 0x1;
	/* P is unused, but encoded
	bool    P    = (inst >> 25) & 0x1;
	*/

	uint32_t imm32 = imm8 << 2;
	bool add = U==1;

	if ((rt > 13) || (rt2 > 13) || (rt == rt2) || (W == 1))
		CORE_ERR_unpredictable("ldrd_lit_t1 case\n");

	OP_DECOMPILE("LDRD<c> <Rt>,<Rt2>,[PC,#+/-<imm>]", rt, rt2, add, imm32);
	return ldrd_lit(rt, rt2, imm32, add);
}

// arm-v7-m
static void ldrh_imm_t2(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm12;
	bool index = true;
	bool add = true;
	bool wback = false;

	if (rt == 13)
		CORE_ERR_unpredictable("ldrh_imm_t2 case\n");

	OP_DECOMPILE("LDRH<c>.W <Rt>,[<Rn>{,#<imm12>}]", rt, rn, imm32);
	return ldrh_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void ldrh_imm_t3(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	bool W = (inst >> 8) & 0x1;
	bool U = (inst >> 9) & 0x1;
	bool P = (inst >> 10) & 0x1;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm8;
	bool index = P==1;
	bool add = U==1;
	bool wback = W==1;

	if ((rt == 13) || (wback && (rn == rt)))
		CORE_ERR_unpredictable("ldrh_imm_t3 case 1\n");
	if ((rt == 15) && ((P == 0) || (U == 1) || (W == 1)))
		CORE_ERR_unpredictable("ldrh_imm_t3 case 2\n");

	if (index && !wback) // Offset
		OP_DECOMPILE("LDRH<c> <Rt>, [<Rn> {, #+/-<imm>}]", rt, rn, add, imm32);
	else if (index && wback) // Pre-indexed
		OP_DECOMPILE("LDRH<c> <Rt>, [<Rn>, #+/-<imm>]!", rt, rn, add, imm32);
	else if (!index && wback) // Post-indexed
		OP_DECOMPILE("LDRH<c> <Rt>, [<Rn>], #+/-<imm>", rt, rn, add, imm32);
	else
		OP_DECOMPILE("!!Error decoding LDR? Illegal combination!!");
	return ldrh_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void ldrh_lit_t1(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	bool U = (inst >> 23) & 0x1;

	uint32_t imm32 = imm12;
	bool add = U==1;

	if (rt == 13)
		CORE_ERR_unpredictable("ldrh_lit_t1 case\n");

	OP_DECOMPILE("LDRH<c> <Rt>, [PC, #+/-<imm>]", rt, imm32);
	return ldrh_lit(rt, imm32, add);
}

// arm-v7-m
static void ldrh_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t imm2 = (inst >> 4) & 0x3;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	bool index = true;
	bool add = true;
	bool wback = false;
	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = imm2;

	if ((rt == 13) || (rm > 13))
		CORE_ERR_unpredictable("ldrh_reg_t2 case\n");

	OP_DECOMPILE("LDRH<c>.W <Rt>,[<Rn>,<Rm>{,LSL #<imm2>}]",
			rt, rn, rm, shift_n);
	return ldrh_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
}

// arm-v7-m
static void ldrsb_imm_t1(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm12;
	bool index = true;
	bool add = true;
	bool wback = false;

	if (rt == 13)
		CORE_ERR_unpredictable("ldrsb_imm_t1 case\n");

	OP_DECOMPILE("LDRSB<c> <Rt>,[<Rn>,#<imm12>]", rt, rn, imm32);
	return ldrsb_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void ldrsb_imm_t2(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	bool W = (inst >> 8) & 0x1;
	bool U = (inst >> 9) & 0x1;
	bool P = (inst >> 10) & 0x1;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm8;
	bool index = P==1;
	bool add = U==1;
	bool wback = W==1;

	if ((rt == 13) || (wback && (rn == rt)))
		CORE_ERR_unpredictable("ldrsb_imm_t2 case 1\n");
	if ((rt == 15) && ((P==0) || (U==1) || (W==1)))
		CORE_ERR_unpredictable("ldrsb_imm_t2 case 2\n");

	if (index && !wback) // Offset
		OP_DECOMPILE("LDRSB<c> <Rt>, [<Rn> {, #+/-<imm>}]", rt, rn, add, imm32);
	else if (index && wback) // Pre-indexed
		OP_DECOMPILE("LDRSB<c> <Rt>, [<Rn>, #+/-<imm>]!", rt, rn, add, imm32);
	else if (!index && wback) // Post-indexed
		OP_DECOMPILE("LDRSB<c> <Rt>, [<Rn>], #+/-<imm>", rt, rn, add, imm32);
	else
		OP_DECOMPILE("!!Error decoding LDRSB? Illegal combination!!");
	return ldrsb_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void ldrsb_lit_t1(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	bool U = (inst >> 23) & 0x1;

	uint32_t imm32 = imm12;
	bool add = U==1;

	if (rt == 13)
		CORE_ERR_unpredictable("ldrsb_lit_t1 case\n");

	OP_DECOMPILE("LDRSB<c> <Rt>,[PC,#+/-<imm>]", rt, add, imm32);
	return ldrsb_lit(rt, imm32, add);
}

// arm-v7-m
static void ldrsb_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t imm2 = (inst >> 4) & 0x3;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	bool index = true;
	bool add = true;
	bool wback = false;

	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = imm2;

	if ((rt == 13) || (rm > 13))
		CORE_ERR_unpredictable("ldrsb_reg_t2 case\n");

	OP_DECOMPILE("LDRSB<c>.W <Rt>,[<Rn>,<Rm>{,LSL #<imm2>}]",
			rt, rn, rm, shift_n);
	return ldrsb_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
}

// arm-v7-m
static void ldrsh_imm_t1(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm12;
	bool index = true;
	bool add = true;
	bool wback = false;

	if (rt == 13)
		CORE_ERR_unpredictable("ldrsh_imm_t1 case\n");

	OP_DECOMPILE("LDRSH<c> <Rt>,[<Rn>,#<imm12>", rt, rn, imm32);
	return ldrsh_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void ldrsh_imm_t2(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	bool W = (inst >> 8) & 0x1;
	bool U = (inst >> 9) & 0x1;
	bool P = (inst >> 10) & 0x1;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm8;
	bool index = P==1;
	bool add = U==1;
	bool wback = W==1;

	if ((rt == 13) || (wback && (rn == rt)))
		CORE_ERR_unpredictable("ldrsh_imm_t2 case 1\n");
	if ((rt == 15) && ((P==0) || (U==1) || (W==1)))
		CORE_ERR_unpredictable("ldrsh_imm_t2 case 2\n");

	if (index && !wback) // Offset
		OP_DECOMPILE("LDRSH<c> <Rt>, [<Rn> {, #+/-<imm>}]", rt, rn, add, imm32);
	else if (index && wback) // Pre-indexed
		OP_DECOMPILE("LDRSH<c> <Rt>, [<Rn>, #+/-<imm>]!", rt, rn, add, imm32);
	else if (!index && wback) // Post-indexed
		OP_DECOMPILE("LDRSH<c> <Rt>, [<Rn>], #+/-<imm>", rt, rn, add, imm32);
	else
		OP_DECOMPILE("!!Error decoding LDRSH? Illegal combination!!");
	return ldrsh_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void ldrsh_lit_t1(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	bool U = (inst >> 23) & 0x1;

	uint32_t imm32 = imm12;
	bool add = U==1;

	if (rt == 13)
		CORE_ERR_unpredictable("ldrsh_lit_t1 case\n");

	OP_DECOMPILE("LDRSH<c> <Rt>,[PC,#+/-<imm>", rt, add, imm32);
	return ldrsh_lit(rt, imm32, add);
}

// arm-v7-m
static void ldrsh_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t imm2 = (inst >> 4) & 0x3;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	bool index = true;
	bool add = true;
	bool wback = true;
	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = imm2;

	OP_DECOMPILE("LDRSH<c>.W <Rt>,[<Rn>,<Rm>{,LSL #<imm2>}]",
			rt, rn, rm, shift_n);
	return ldrsh_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_ldr(void) {
	// ldr_imm_t3: 1111 1000 1101 xxxx <x's>
	//                            1111
	register_opcode_mask_32_ex(0xf8d00000, 0x07200000, ldr_imm_t3,
			0x000f0000, 0x0, 0, 0);

	// ldr_imm_t4: 1111 1000 0101 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_32_ex(0xf8500800, 0x07a00000, ldr_imm_t4,
			0xf0000, 0x0,
			0x600, 0x100,
			0xd0304, 0x204fb,
			0x0, 0x500,
			0, 0);

	// ldr_reg_t2: 1111 1000 0101 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_32_ex(0xf8500000, 0x07a00fc0, ldr_reg_t2,
			0xf0000, 0x0, 0, 0);

	// ldr_lit_t2: 1111 1000 x101 1111 <x's>
	register_opcode_mask_32(0xf85f0000, 0x07200000, ldr_lit_t2);

	// ldrb_imm_t2: 1111 1000 1001 <x's>
	register_opcode_mask_32_ex(0xf8900000, 0x07600000, ldrb_imm_t2, 0xf000, 0x0, 0xf0000, 0x0, 0, 0);

	// ldrb_imm_t3: 1111 1000 0001 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_32_ex(0xf8100800, 0x07e00000, ldrb_imm_t3, 0xf400, 0x300, 0xf0000, 0x0, 0x600, 0x100, 0x0, 0x500, 0, 0);

	// ldrb_lit_t1: 1111 1000 x001 1111 <x's>
	register_opcode_mask_32_ex(0xf81f0000, 0x07600000, ldrb_lit_t1,
			0xf000, 0x0, 0, 0);

	// ldrb_reg_t2: 1111 1000 0001 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_32_ex(0xf8100000, 0x07e00fc0, ldrb_reg_t2, 0xf000, 0x0, 0xf0000, 0x0, 0, 0);

	// ldrd_imm_t1: 1110 100x x1x1 <x's>
	register_opcode_mask_32_ex(0xe8500000, 0x16000000, ldrd_imm_t1, 0x0, 0x01200000, 0xf0000, 0x0, 0, 0);

	// ldrd_lit_t1: 1110 100x x1x1 1111 <x's>
	register_opcode_mask_32_ex(0xe85f0000, 0x16000000, ldrd_lit_t1,
			0x0, 0x01800000,
			0, 0);

	// ldrh_imm_t2: 1111 1000 1011 <x's>
	register_opcode_mask_32_ex(0xf8b00000, 0x07400000, ldrh_imm_t2,
			0xf000, 0x0,
			0xf0000, 0x0,
			0, 0);

	// ldrh_imm_t3: 1111 1000 0011 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_32_ex(0xf8300800, 0x07c00000, ldrh_imm_t3,
			0xf0000, 0x0,
			0xf400, 0x300,
			0x600, 0x100,
			0x0, 0x500,
			0, 0);

	// ldrh_lit_t1: 1111 1000 x011 1111 <x's>
	register_opcode_mask_32_ex(0xf83f0000, 0x07400000, ldrh_lit_t1,
			0xf000, 0x0,
			0, 0);

	// ldrh_reg_t2: 1111 1000 0011 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_32_ex(0xf8300000, 0x07c00fc0, ldrh_reg_t2,
			0xf0000, 0x0,
			0xf000, 0x0,
			0, 0);

	// ldrsb_imm_t1: 1111 1001 1001 <x's>
	register_opcode_mask_32_ex(0xf9900000, 0x06600000, ldrsb_imm_t1,
			0xf000, 0x0,
			0xf0000, 0x0,
			0, 0);

	// ldrsb_imm_t2: 1111 1001 0001 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_32_ex(0xf9100800, 0x06e00000, ldrsb_imm_t2,
			0xf400, 0x300,
			0xf0000, 0x0,
			0x600, 0x100,
			0x0, 0x500,
			0, 0);

	// ldrsb_lit_t1: 1111 1001 x001 1111 <x's>
	register_opcode_mask_32_ex(0xf91f0000, 0x06600000, ldrsb_lit_t1,
			0xf000, 0x0,
			0, 0);

	// ldrsb_reg_t2: 1111 1001 0001 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_32_ex(0xf9100000, 0x06e00fc0, ldrsb_reg_t2,
			0xf000, 0x0,
			0xf0000, 0x0,
			0, 0);

	// ldrsh_imm_t1: 1111 1001 1011 <x's>
	register_opcode_mask_32_ex(0xf9b00000, 0x06400000, ldrsh_imm_t1,
			0xf0000, 0x0,
			0xf000, 0x0,
			0, 0);

	// ldrsh_imm_t2: 1111 1001 0011 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_32_ex(0xf9300800, 0x06c00000, ldrsh_imm_t2,
			0xf0000, 0x0,
			0xf400, 0x300,
			0x600, 0x100,
			0x0, 0x500,
			0, 0);

	// ldrsh_lit_t1: 1111 1001 x011 1111 <x's>
	register_opcode_mask_32_ex(0xf93f0000, 0x06400000, ldrsh_lit_t1,
			0xf000, 0x0,
			0, 0);

	// ldrsh_reg_t2: 1111 1001 0011 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_32_ex(0xf9300000, 0x06c00fc0, ldrsh_reg_t2,
			0xf0000, 0x0,
			0xf000, 0x0,
			0, 0);
}
