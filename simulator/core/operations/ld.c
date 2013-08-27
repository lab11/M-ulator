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

#include "opcodes.h"
#include "helpers.h"

#include "cpu/registers.h"
#include "cpu/core.h"
#include "cpu/misc.h"

static void ldr_imm(uint8_t rt, uint8_t rn, uint32_t imm32, bool index, bool add, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + imm32;
	else
		offset_addr = rn_val - imm32;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	uint32_t data = read_word(address);

	if (wback)
		CORE_reg_write(rn, offset_addr);

	if (rt == 15) {
		if (address & 0x3)
			CORE_ERR_unpredictable("Illegal load of PC in ldr imm\n");
		else
			LoadWritePC(data);
	} else {
		CORE_reg_write(rt, data);
	}
}

// arm-thumb
static void ldr_imm_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm5 = (inst >> 6) & 0x1f;

	uint32_t imm32 = imm5 << 2;
	bool index = true;
	bool add = true;
	bool wback = false;

	OP_DECOMPILE("LDR<c> <Rt>, [<Rn>{,#<imm5>}]", rt, rn, imm32);
	return ldr_imm(rt, rn, imm32, index, add, wback);
}

// arm-thumb
static void ldr_imm_t2(uint16_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rt   = (inst >> 8) & 0x7;

	uint8_t rn = 13;
	uint32_t imm32 = imm8 << 2;
	bool index = true;
	bool add = true;
	bool wback = false;

	OP_DECOMPILE("LDR<c> <Rt>, [SP{,#<imm8>}]", rt, imm32);
	return ldr_imm(rt, rn, imm32, index, add, wback);
}

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

static void ldr_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback,
		enum SRType shift_t, uint8_t shift_n) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);

	union apsr_t apsr = CORE_apsr_read();

	uint32_t offset = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + offset;
	else
		offset_addr = rn_val - offset;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	uint32_t data = read_word(address);

	if (wback)
		CORE_reg_write(rn, offset_addr);

	if (rt == 15) {
		if (address & 0x3)
			CORE_ERR_unpredictable("bad dest pc\n");
		else
			LoadWritePC(data);
	} else {
		CORE_reg_write(rt, data);
	}
}

// arm-thumb
static void ldr_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool index = true;
	bool add = true;
	bool wback = false;
	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("LDR<c> <Rt>,[<Rn>,<Rm>]", rt, rn, rm);
	return ldr_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
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

static void ldr_lit(bool add, uint8_t rt, uint32_t imm32) {
	uint32_t base = 0xfffffffc & CORE_reg_read(PC_REG);

	uint32_t addr;
	if (add) {
		addr = base + imm32;
	} else {
		addr = base - imm32;
	}

	uint32_t data = read_word(addr);

	if (rt == 15) {
		if (addr & 0x3) {
			CORE_ERR_unpredictable("ldr_lit misaligned\n");
		} else {
			LoadWritePC(data);
		}
	} else {
		CORE_reg_write(rt, data);
	}
}

// arm-thumb
static void ldr_lit_t1(uint16_t inst) {
	uint32_t imm8 = inst & 0xff;
	uint8_t rt = (inst & 0x700) >> 8;

	uint32_t imm32 = imm8 << 2;

	OP_DECOMPILE("LDR<c> <Rt>,<label>", rt, imm32);
	return ldr_lit(true, rt, imm32);
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

static void ldrb_imm(uint8_t rt, uint8_t rn, uint32_t imm32, bool add, bool index, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + imm32;
	else
		offset_addr = rn_val - imm32;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	CORE_reg_write(rt, read_byte(address));

	if (wback)
		CORE_reg_write(rn, offset_addr);
}

// arm-thumb
static void ldrb_imm_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm5 = (inst >> 6) & 0x1f;

	uint32_t imm32 = imm5;
	bool index = true;
	bool add = true;
	bool wback = false;

	OP_DECOMPILE("LDRB<c> <Rt>,[<Rn>{,#<imm5>}]", rt, rn, imm5);
	return ldrb_imm(rt, rn, imm32, add, index, wback);
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

static inline void ldrb_lit(uint8_t rt, uint32_t imm32, bool add) {
	uint32_t base = CORE_reg_read(PC_REG) & 0xfffffffc;

	uint32_t address;
	if (add)
		address = base + imm32;
	else
		address = base - imm32;

	CORE_reg_write(rt, read_byte(address));
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

static void ldrb_reg(uint8_t rt, uint8_t rn, uint8_t rm, enum SRType shift_t,
		uint8_t shift_n, bool index, bool add, bool wback) {
	assert(wback == false);

	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rn_val = CORE_reg_read(rn);
	union apsr_t apsr = CORE_apsr_read();

	uint32_t offset = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + offset;
	else
		offset_addr = rn_val - offset;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	CORE_reg_write(rt, read_byte(address));
}

// arm-thumb
static void ldrb_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool index = true;
	bool add = true;
	bool wback = false;

	enum SRType shift_t = LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("LDRB<c> <Rt>,[<Rn>,<Rm>]", rt, rn, rm);
	return ldrb_reg(rt, rn, rm, shift_t, shift_n, index, add, wback);
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

	if ((rt >= 13) || (rt2 >= 13) || (rt == rt2))
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
	////

	uint32_t offset_addr;
	if (add) {
		offset_addr = CORE_reg_read(rn) + imm32;
	} else {
		offset_addr = CORE_reg_read(rn) - imm32;
	}

	uint32_t addr;
	if (index) {
		addr = offset_addr;
	} else {
		addr = CORE_reg_read(rn);
	}

	CORE_reg_write(rt, read_word(addr));
	CORE_reg_write(rt2, read_word(addr + 4));

	if (wback) {
		CORE_reg_write(rn, offset_addr);
	}
}

static inline void ldrd_lit(uint8_t rt, uint8_t rt2,
		uint32_t imm32, bool add) {
	uint32_t pc_val = CORE_reg_read(PC_REG);
	if (pc_val & 0x3)
		CORE_ERR_unpredictable("Unaligned PC in ldrd_lit\n");

	uint32_t address;
	address = (add) ? pc_val + imm32 : pc_val - imm32;

	CORE_reg_write(rt, read_word(address));
	CORE_reg_write(rt2, read_word(address+4));
}

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

static void ldrh_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t offset_addr;
	offset_addr = (add) ? rn_val + imm32 : rn_val - imm32;

	uint32_t address;
	address = (index) ? offset_addr : rn_val;

	uint32_t data;
	data = read_halfword(address);

	if (wback)
		CORE_reg_write(rn, offset_addr);

	CORE_reg_write(rt, data);
}

// arm-thumb
static void ldrh_imm_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm5 = (inst >> 6) & 0x1f;

	uint32_t imm32 = imm5 << 1;
	bool index = true;
	bool add = true;
	bool wback = false;

	OP_DECOMPILE("LDRH <Rt>,[<Rn>{,#<imm5>}]", rt, rn, imm32);
	return ldrh_imm(rt, rn, imm32, index, add, wback);
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

static inline void ldrh_lit(uint8_t rt, uint32_t imm32, bool add) {
	uint32_t pc_val = CORE_reg_read(PC_REG);
	uint32_t base = pc_val & ~0x3;
	uint32_t address;
	address = (add) ? base + imm32 : base - imm32;
	uint32_t data = read_halfword(address);
	CORE_reg_write(rt, data);
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

static void ldrh_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback,
		enum SRType shift_t, uint8_t shift_n) {
	union apsr_t apsr = CORE_apsr_read();

	uint32_t offset;
	offset = Shift(CORE_reg_read(rm), 32, shift_t, shift_n, apsr.bits.C);

	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t offset_addr;
	offset_addr = (add) ? rn_val + offset : rn_val - offset;

	uint32_t address;
	address = (index) ? offset_addr : rn_val;

	uint32_t data = read_halfword(address);

	if (wback)
		CORE_reg_write(rn, offset_addr);

	CORE_reg_write(rt, data);
}

// arm-thumb
static void ldrh_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool index = true;
	bool add = true;
	bool wback = false;
	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("LDRH<c> <Rt>,[<Rn>,<Rm>]", rt, rn, rm);
	return ldrh_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
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

static void ldrsb_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t offset_addr;
	offset_addr = (add) ? rn_val + imm32 : rn_val - imm32;

	uint32_t address;
	address = (index) ? offset_addr : rn_val;

	int8_t sbyte = (int8_t) read_byte(address);
	int32_t sword = sbyte;
	CORE_reg_write(rt, (uint32_t) sword);

	if (wback)
		CORE_reg_write(rn, offset_addr);
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

static inline void ldrsb_lit(uint8_t rt, uint32_t imm32, bool add) {
	uint32_t pc_val = CORE_reg_read(PC_REG);
	uint32_t base = pc_val & ~0x3;
	uint32_t address;
	address = (add) ? base + imm32 : base - imm32;

	int8_t sbyte = (int8_t) read_byte(address);
	int32_t sword = sbyte;
	CORE_reg_write(rt, (uint32_t) sword);
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

static void ldrsb_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback,
		enum SRType shift_t, uint8_t shift_n) {
	assert(wback == false);

	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rn_val = CORE_reg_read(rn);
	union apsr_t apsr = CORE_apsr_read();

	uint32_t offset = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + offset;
	else
		offset_addr = rn_val - offset;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	int32_t signd = (int32_t) read_byte(address);
	CORE_reg_write(rt, signd);
}

// arm-thumb
static void ldrsb_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool index = true;
	bool add = true;
	bool wback = false;

	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("LDRSB<c> <Rt>,[<Rn>,<Rm>]", rt, rn, rm);
	return ldrsb_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
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

static void ldrsh_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t offset_addr;
	offset_addr = (add) ? rn_val + imm32 : rn_val - imm32;

	uint32_t address;
	address = (index) ? offset_addr : rn_val;

	int16_t shword = (int16_t) read_halfword(address);
	int32_t sword = shword;
	CORE_reg_write(rt, (uint32_t) sword);

	if (wback)
		CORE_reg_write(rn, offset_addr);
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

static inline void ldrsh_lit(uint8_t rt, uint32_t imm32, bool add) {
	uint32_t pc_val = CORE_reg_read(PC_REG);
	uint32_t base = pc_val & ~0x3;
	uint32_t address;
	address = (add) ? base + imm32 : base - imm32;

	int16_t shword = (int16_t) read_halfword(address);
	int32_t sword = shword;
	CORE_reg_write(rt, (uint32_t) sword);
}

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

static void ldrsh_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback,
		enum SRType shift_t, uint8_t shift_n) {
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rn_val = CORE_reg_read(rn);
	union apsr_t apsr = CORE_apsr_read();

	uint32_t offset = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);
	uint32_t offset_addr;
	offset_addr = (add) ? rn_val + offset : rn_val - offset;

	uint32_t address;
	address = (index) ? offset_addr : rn_val;

	int16_t shword = (int16_t) read_halfword(address);
	int32_t sword = shword;
	CORE_reg_write(rt, (uint32_t) sword);

	if (wback)
		CORE_reg_write(rn, offset_addr);
}

// arm-thumb
static void ldrsh_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool index = true;
	bool add = true;
	bool wback = true;
	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("LDRSH<c> <Rt>,[<Rn>,<Rm>]", rt, rn, rm);
	return ldrsh_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
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
void register_opcodes_ld(void) {
	// ldr_imm_t1: 0110 1<x's>
	register_opcode_mask_16(0x6800, 0x9000, ldr_imm_t1);

	// ldr_imm_t2: 1001 1<x's>
	register_opcode_mask_16(0x9800, 0x6000, ldr_imm_t2);

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

	// ldr_reg_t1: 0101 100<x's>
	register_opcode_mask_16(0x5800, 0xa600, ldr_reg_t1);

	// ldr_reg_t2: 1111 1000 0101 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_32_ex(0xf8500000, 0x07a00fc0, ldr_reg_t2,
			0xf0000, 0x0, 0, 0);

	// ldr_lit_t1: 0100 1<x's>
	register_opcode_mask_16(0x4800, 0xb000, ldr_lit_t1);

	// ldr_lit_t2: 1111 1000 x101 1111 <x's>
	register_opcode_mask_32(0xf85f0000, 0x07200000, ldr_lit_t2);

	// ldrb_imm_t1: 0111 1<x's>
	register_opcode_mask_16(0x7800, 0x8000, ldrb_imm_t1);

	// ldrb_imm_t2: 1111 1000 1001 <x's>
	register_opcode_mask_32_ex(0xf8900000, 0x07600000, ldrb_imm_t2, 0xf000, 0x0, 0xf0000, 0x0, 0, 0);

	// ldrb_imm_t3: 1111 1000 0001 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_32_ex(0xf8100800, 0x07e00000, ldrb_imm_t3, 0xf400, 0x300, 0xf0000, 0x0, 0x600, 0x100, 0x0, 0x500, 0, 0);

	// ldrb_lit_t1: 1111 1000 x001 1111 <x's>
	register_opcode_mask_32_ex(0xf81f0000, 0x07600000, ldrb_lit_t1,
			0xf000, 0x0, 0, 0);

	// ldrb_reg_t1: 0101 110x xxxx xxxx
	register_opcode_mask_16(0x5c00, 0xa200, ldrb_reg_t1);

	// ldrb_reg_t2: 1111 1000 0001 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_32_ex(0xf8100000, 0x07e00fc0, ldrb_reg_t2, 0xf000, 0x0, 0xf0000, 0x0, 0, 0);

	// ldrd_imm_t1: 1110 100x x1x1 <x's>
	register_opcode_mask_32_ex(0xe8500000, 0x16000000, ldrd_imm_t1, 0x0, 0x01200000, 0xf0000, 0x0, 0, 0);

	// ldrd_lit_t1: 1110 100x x1x1 1111 <x's>
	register_opcode_mask_32_ex(0xe85f0000, 0x16000000, ldrd_lit_t1,
			0x0, 0x01800000,
			0, 0);

	// ldrh_imm_t1: 1000 1<x's>
	register_opcode_mask_16(0x8800, 0x7000, ldrh_imm_t1);

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

	// ldrh_reg_t1: 0101 101<x's>
	register_opcode_mask_16(0x5a00, 0xa400, ldrh_reg_t1);

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

	// ldrsb_reg_t1: 0101 011x xxxx xxxx
	register_opcode_mask_16(0x5600, 0xa800, ldrsb_reg_t1);

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

	// ldrsh_reg_t1: 0101 111x <x's>
	register_opcode_mask_16(0xa000, 0x5e00, ldrsh_reg_t1);

	// ldrsh_reg_t2: 1111 1001 0011 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_32_ex(0xf9300000, 0x06c00fc0, ldrsh_reg_t2,
			0xf0000, 0x0,
			0xf000, 0x0,
			0, 0);
}
