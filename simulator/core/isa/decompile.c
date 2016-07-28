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

#include "core/common.h"

#ifdef HAVE_DECOMPILE

#include "core/pipeline.h" // for STALL_PC

#include "core/operations/helpers.h"

#include "cpu/registers.h"
#include "cpu/core.h"
#include "cpu/misc.h"

static const char* it_condition = "!!it_condition not initialized!!";

EXPORT bool decompile_ran;

static int handle_op(const char *syntax, va_list args) {
	assert(syntax[0] == '<');
	assert(syntax[1] != '>');
	char buf[16] = {0};
	int i = 1;

	while (syntax[i] != '>') {
		assert(syntax[i] != '\0');
		buf[i-1] = syntax[i];
		i++;
		assert(i != (16 + 1));
	}

#define IS_OP(_op) (0 == (strcmp(_op, buf)))
	if (IS_OP("c")) {
		if (in_ITblock())
			printf("%s", it_condition);
	} else if ((0 == strncmp("imm", buf, 3))
			|| IS_OP("const")
			|| IS_OP("lsb")
			|| IS_OP("option")
			|| IS_OP("width")
		  ) {
		unsigned imm = va_arg(args, unsigned);
		printf("0x%x", imm);
	} else if (IS_OP("IT")) {
		// Fake option for inside IT block
		if (in_ITblock())
			printf("%s", it_condition);
		else
			printf("S");
	} else if (IS_OP("label")) {
		unsigned imm32 = va_arg(args, unsigned);
		printf("0x%x", imm32);
		uint32_t pc_val = CORE_reg_read(PC_REG);
		printf(" (PC=%08x, PC+label=%08x)", pc_val, pc_val+imm32);
	} else if (IS_OP("Rd")) {
		unsigned rd = va_arg(args, unsigned);
		printf("R%02d(=%08x)", rd, CORE_reg_read(rd));
	} else if (IS_OP("Rm")) {
		unsigned rm = va_arg(args, unsigned);
		printf("R%02d(=%08x)", rm, CORE_reg_read(rm));
	} else if (IS_OP("Rn")) {
		unsigned rn = va_arg(args, unsigned);
		printf("R%02d(=%08x)", rn, CORE_reg_read(rn));
	} else if (IS_OP("Rt")) {
		unsigned rt = va_arg(args, unsigned);
		printf("R%02d(=%08x)", rt, CORE_reg_read(rt));
	} else if (IS_OP("Rt2")) {
		unsigned rt2 = va_arg(args, unsigned);
		printf("R%02d(=%08x)", rt2, CORE_reg_read(rt2));
	} else if (IS_OP("Rdn")) {
		unsigned rdn = va_arg(args, unsigned);
		printf("R%02d(=%08x)", rdn, CORE_reg_read(rdn));
	} else if (IS_OP("RdLo")) {
		unsigned rdlo = va_arg(args, unsigned);
		printf("R%02d(=%08x)", rdlo, CORE_reg_read(rdlo));
	} else if (IS_OP("RdHi")) {
		unsigned rdhi = va_arg(args, unsigned);
		printf("R%02d(=%08x)", rdhi, CORE_reg_read(rdhi));
	} else if (IS_OP("registers")) {
		unsigned registers = va_arg(args, unsigned);
		putchar_unlocked('{');
		int j;
		for (j=0; j<SP_REG; j++) {
			if (registers & (1 << j)) {
				printf("R%02d(=%08x),", j, CORE_reg_read(j));
			}
		}
		if (registers & (1 << SP_REG))
			printf("SP(=%08x),", CORE_reg_read(SP_REG));
		if (registers & (1 << LR_REG))
			printf("LR(=%08x),", CORE_reg_read(LR_REG));
		if (registers & (1 << PC_REG))
			printf("PC(=%08x),", CORE_reg_read(PC_REG));
		putchar_unlocked('}');
	} else if (IS_OP("shift")) {
		enum SRType shift_t = va_arg(args, enum SRType);
		uint8_t shift_n = va_arg(args, unsigned);
		if (shift_t == SRType_LSL)
			printf("LSL ");
		else if (shift_t == SRType_LSR)
			printf("LSR ");
		else if (shift_t == SRType_ASR)
			printf("ASR ");
		else if (shift_t == SRType_ROR)
			printf("ROR ");
		else if (shift_t == SRType_RRX)
			printf("RRX ");
		else
			assert(false && "Illegal shift type?");

		if (shift_t != SRType_RRX)
			printf("#%d", shift_n);
	} else {
		printf("<<unknown: '%s'>>", buf);
	}

	return i;
}

static int print_sign(va_list args) {
	bool add = va_arg(args, unsigned);
	if (!add)
		putchar_unlocked('-');
	return 2;
}

static int handle_braces(const char *syntax, va_list args) {
	assert(syntax[0] == '{');
	assert(syntax[1] != '}');
	int i = 1;

	while (syntax[i] != '}') {
		assert(syntax[i] != '\0');
		if (syntax[i] == '<') {
			i += handle_op(syntax+i, args);
		} else if (0 == strncmp(syntax+i, "+/-", 3)) {
			i += print_sign(args);
		} else if (syntax[i] == '!') {
			bool exc = va_arg(args, unsigned);
			if (exc)
				putchar_unlocked('!');
		} else {
			putchar_unlocked(syntax[i]);
		}
		i++;
	}

	return i;
}

static const char* cond_to_str(uint8_t cond) {
	DBG2("cond: %x %d\n", cond, cond);
	switch (cond) {
		case  0: return "EQ";
		case  1: return "NE";
		case  2: return "CS";
		case  3: return "CC";
		case  4: return "MI";
		case  5: return "PL";
		case  6: return "VS";
		case  7: return "VC";
		case  8: return "HI";
		case  9: return "LS";
		case 10: return "GE";
		case 11: return "LT";
		case 12: return "GT";
		case 13: return "LE";
		case 14: return "AL";
		case 15: return "  "; // unconditional branch
		default:
			 assert(false && "illegal condition"); return "!!";
	}
}

static size_t print_bcond(const char *syntax, va_list args) {
	unsigned cond = va_arg(args, unsigned);

	assert(0 == strncmp(syntax, "B<c>", 4));

	putchar_unlocked('B');
	printf("%s", cond_to_str(cond));
	return 3;
}

static size_t print_it_inst(const char *syntax, va_list args) {
	unsigned itstate = va_arg(args, unsigned);

	uint8_t mask = itstate & 0xf;
	bool mask0 = !!(mask & 0x1);
	bool mask1 = !!(mask & 0x2);
	bool mask2 = !!(mask & 0x4);
	bool mask3 = !!(mask & 0x8);
	uint8_t firstcond = (itstate >> 4) & 0xf;
	bool firstcond0 = !!(firstcond & 0x1);

	printf("IT");
	if (mask3 && !mask2 && !mask1 && !mask0) {
		; // no x, y, or z
	} else {
		if (firstcond0 == mask3)
			putchar_unlocked('T');
		else
			putchar_unlocked('E');
		if (mask2 && !mask1 && !mask0) {
			; // no y or z
		} else {
			if (firstcond0 == mask2)
				putchar_unlocked('T');
			else
				putchar_unlocked('E');
			if (mask1 && !mask0) {
				; // no z
			} else {
				if (firstcond0 == mask1)
					putchar_unlocked('T');
				else
					putchar_unlocked('E');
				assert(mask0);
			}
		}
	}

	putchar_unlocked('\t');

	it_condition = cond_to_str(firstcond);
	printf("%s", it_condition);

	union apsr_t apsr = CORE_apsr_read();
	printf("(N=%d,Z=%d,C=%d,V=%d,Q=%d)",
			apsr.bits.N,
			apsr.bits.Z,
			apsr.bits.C,
			apsr.bits.V,
			apsr.bits.Q);

	return strlen(syntax);
}

static int print_sp(void) {
	printf("SP(=%08x)", CORE_reg_read(SP_REG));
	return 1;
}

static int print_pc(void) {
	printf("PC(=%08x)", CORE_reg_read(PC_REG));
	return 1;
}

static int print_optional_setflags(va_list args) {
	bool setflags = va_arg(args, int);
	if (setflags)
		putchar_unlocked('S');
	return 2;
}

static void _op_decompile(const char *syntax, va_list args) {
	size_t len = strlen(syntax);
	int space_cnt = 0;

	unsigned i;
	for (i=0; i<len; i++) {
		/* The current handling of conditions is a little hack-y.
		   In particular, in the M profile, only branch instructions
		   or instructions inside of an IT block are permitted to have
		   conditions attached to them, thus the decompile ignores
		   the <c> directive outside of IT blocks. This works for
		   everything but B<c>, which we detect explicitly. When/if
		   this expands beyond the M profile, this will need to be
		   adjusted. */
		if ( (i == 0) && (0 == strncmp(syntax+i, "B<c>", 4)) )
			i += print_bcond(syntax+i, args);
		else if (syntax[i] == '<')
			i += handle_op(syntax+i, args);
		else if (0 == strncmp(syntax+i, "IT", 2))
			i += print_it_inst(syntax+i, args);
		else if (0 == strncmp(syntax+i, "PC", 2))
			i += print_pc();
		else if (0 == strncmp(syntax+i, "SP", 2))
			i += print_sp();
		else if (0 == strncmp(syntax+i, "{S}", 3))
			i += print_optional_setflags(args);
		else if (0 == strncmp(syntax+i, "+/-", 3))
			i += print_sign(args);
		else if (syntax[i] == '{')
			i += handle_braces(syntax+i, args);
		else if ((syntax[i] == ' ') && (space_cnt++ == 0))
			putchar_unlocked('\t');
		else
			putchar_unlocked(syntax[i]);
	}
}

EXPORT void op_decompile(const char* syntax, ...) {
	flockfile(stdout); flockfile(stderr);
	va_list va_args;
	va_start(va_args, syntax);
	printf("DECOM: ");
	if (CORE_reg_read(PC_REG) == STALL_PC)
		printf("(STALL)");
	else if ((syntax[0] == '!') && (syntax[1] == '!'))
		printf("%s", syntax);
	else
		_op_decompile(syntax, va_args);
	putchar_unlocked('\n');
	va_end(va_args);
	funlockfile(stderr); funlockfile(stdout);

	decompile_ran = true;
}

#else // HAVE_DECOMPILE

// Leave an empty stub function here to suppress unused argument issues
// from the call sites of variables created only for OP_DECOMPILE support
// in some build variations. Need this b/c there's no way to mark variadic
// arguments as potentially unused in fn decl or call site
EXPORT void op_decompile(const char* syntax __attribute__ ((unused)), ...) {
}

#endif // HAVE_DECOMPILE
