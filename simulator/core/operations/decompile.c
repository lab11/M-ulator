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

#ifdef HAVE_DECOMPILE

#include "core/common.h"

#include "core/pipeline.h" // for STALL_PC

#include "cpu/registers.h"
#include "cpu/core.h"
#include "cpu/misc.h"

#include "helpers.h"

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
		;
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
			printf("!!ERR: IT block decoding not impl!!");
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

static size_t print_it_inst(const char *syntax, va_list args) {
	unsigned itstate = va_arg(args, unsigned);

	uint8_t mask = itstate & 0xf;
	assert(mask != 0);
	bool mask0 = !!(mask & 0x1);
	bool mask1 = !!(mask & 0x2);
	bool mask2 = !!(mask & 0x4);
	bool mask3 = !!(mask & 0x8);
	uint8_t firstcond = (itstate >> 4) & 0xf;
	bool firstcond0 = !!(firstcond & 0x1);

	printf("IT ");
	if (mask3) {
		; // no x, y, or z
	} else {
		if (firstcond0)
			putchar_unlocked('T');
		else
			putchar_unlocked('E');
		if (mask2) {
			; // no y or z
		} else {
			if (firstcond0)
				putchar_unlocked('T');
			else
				putchar_unlocked('E');
			if (mask1) {
				; // no z
			} else {
				if (firstcond0)
					putchar_unlocked('T');
				else
					putchar_unlocked('E');
				assert(mask0);
			}
		}
	}

	putchar_unlocked('\t');

	switch (firstcond) {
		case  0: printf("EQ"); break;
		case  1: printf("NE"); break;
		case  2: printf("CS"); break;
		case  3: printf("CC"); break;
		case  4: printf("MI"); break;
		case  5: printf("PL"); break;
		case  6: printf("VS"); break;
		case  7: printf("VC"); break;
		case  8: printf("HI"); break;
		case  9: printf("LS"); break;
		case 10: printf("GE"); break;
		case 11: printf("LT"); break;
		case 12: printf("GT"); break;
		case 13: printf("LE"); break;
		case 14: printf("AL"); break;
		case 15: assert(false && "illegal condition"); break;
	}

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
		if (syntax[i] == '<')
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
}

#endif // HAVE_DECOMPILE
