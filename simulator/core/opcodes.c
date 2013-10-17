/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2013  Pat Pannuto <pat.pannuto@gmail.com>
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

#ifndef PP_STRING
#define PP_STRING "---"
#include "pretty_print.h"
#endif

struct op_list {
	struct op* o;
	struct op_list* next;
};

static int opcode_masks;
EXPORT inline int get_opcode_masks(void) { return opcode_masks; }

static void (*fn16_root[256]) (uint16_t);
static void (*fn32_11101_root[256]) (uint32_t);
static void (*fn32_11110_root[256]) (uint32_t);
static void (*fn32_11111_root[256]) (uint32_t);

static struct op_list* op16_root[256];
static struct op_list* op32_11101_root[256];
static struct op_list* op32_11110_root[256];
static struct op_list* op32_11111_root[256];

EXPORT bool match_mask8(uint8_t inst, uint8_t ones_mask, uint8_t zeros_mask) {
	return
		(ones_mask  == (ones_mask  & inst)) &&
		(zeros_mask == (zeros_mask & ~inst));
}

EXPORT bool match_mask16(uint16_t inst, uint16_t ones_mask, uint16_t zeros_mask) {
	return
		(ones_mask  == (ones_mask  & inst)) &&
		(zeros_mask == (zeros_mask & ~inst));
}

EXPORT bool match_mask32(uint32_t inst, uint32_t ones_mask, uint32_t zeros_mask) {
	return
		(ones_mask  == (ones_mask  & inst)) &&
		(zeros_mask == (zeros_mask & ~inst));
}

static bool __attribute__ ((pure)) match_exceptions16(uint16_t inst, const struct op* const o) {
	bool exception = false;
	int j;
	for (j = 0; j < o->op16.ex_cnt; j++) {
		if (match_mask16(inst, o->op16.ex_ones[j], o->op16.ex_zeros[j])) {
			exception = true;
			break;
		}
	}
	return exception;
}

static bool __attribute__ ((pure)) match_exceptions32(uint32_t inst, const struct op* const o) {
	bool exception = false;
	int j;
	for (j = 0; j < o->op32.ex_cnt; j++) {
		if (match_mask32(inst, o->op32.ex_ones[j], o->op32.ex_zeros[j])) {
			exception = true;
			break;
		}
	}
	return exception;
}

static bool __attribute__ ((pure)) match_op16(uint16_t inst, const struct op* const o) {
	if (match_mask16(inst, o->op16.ones_mask, o->op16.zeros_mask)) {
		if (!match_exceptions16(inst, o)) {
			return true;
		}
	}
	return false;
}

static bool __attribute__ ((pure)) match_op32(uint32_t inst, const struct op* const o) {
	if (match_mask32(inst, o->op32.ones_mask, o->op32.zeros_mask)) {
		if (!match_exceptions32(inst, o)) {
			return true;
		}
	}
	return false;
}

static struct op* __attribute__ ((pure)) _find_op(uint32_t inst) {
	struct op_list *olist = NULL;

	switch (inst & 0xf8000000) {
		case 0x00000000:
			assert((inst & 0xffff0000) == 0);
			olist = op16_root[(inst >> 8) & 0xff];
			while (olist != NULL) {
				// optimization
				//if (olist->next == NULL) return olist->o;

				if (match_op16(inst, olist->o))
					return olist->o;
				olist = olist->next;
			}
			return NULL;

		case 0xe8000000:
			olist = op32_11101_root[(inst >> 19) & 0xff];
			break;
		case 0xf0000000:
			olist = op32_11110_root[(inst >> 19) & 0xff];
			break;
		case 0xf8000000:
			olist = op32_11111_root[(inst >> 19) & 0xff];
			break;
	}

	while (olist != NULL) {
		// optimization
		// if (olist->next == NULL) return olist->o;

		if (match_op32(inst, olist->o))
			return olist->o;
		olist = olist->next;
	}
	return NULL;
}

EXPORT struct op* find_op(uint32_t inst) {
	struct op *o = _find_op(inst);
	if (o == NULL)
		CORE_ERR_illegal_instr(inst);
	return o;
}

EXPORT int register_opcode_mask_16_ex_real(uint16_t ones_mask, uint16_t zeros_mask,
		void (*fn) (uint16_t), const char *fn_name, ...) {
	va_list va_args;
	va_start(va_args, fn_name);

	struct op *o = malloc(sizeof(struct op));

	o->name = fn_name;
	o->is16 = true;
	o->op16.ones_mask = ones_mask;
	o->op16.zeros_mask = zeros_mask;
	o->op16.fn = fn;

	o->op16.ex_cnt = 0;
	o->op16.ex_ones = NULL;
	o->op16.ex_zeros = NULL;

	uint16_t exc_ones_mask;
	uint16_t exc_zeros_mask;
	exc_ones_mask  = va_arg(va_args, unsigned int);
	exc_zeros_mask = va_arg(va_args, unsigned int);
	while (exc_ones_mask || exc_zeros_mask) {
		// Make the assumption that callers will have one, at most
		// two exceptions; go with the simple realloc scheme
		unsigned idx = o->op16.ex_cnt;

		o->op16.ex_cnt++;
		o->op16.ex_ones  = realloc(o->op16.ex_ones,  o->op16.ex_cnt * sizeof(uint16_t));
		assert(NULL != o->op16.ex_ones && "realloc");
		o->op16.ex_zeros = realloc(o->op16.ex_zeros, o->op16.ex_cnt * sizeof(uint16_t));
		assert(NULL != o->op16.ex_zeros && "realloc");

		o->op16.ex_ones[idx]  = exc_ones_mask;
		o->op16.ex_zeros[idx] = exc_zeros_mask;

		exc_ones_mask  = va_arg(va_args, unsigned int);
		exc_zeros_mask = va_arg(va_args, unsigned int);
	}

	////

	bool inserted = false;
	int i;
	uint8_t ones_bucket = ones_mask >> 8;
	uint8_t zeros_bucket = zeros_mask >> 8;
	for (i = 0; i < 256; i++) {
		if (match_mask8(i, ones_bucket, zeros_bucket)) {
			inserted = true;

			struct op_list* olist = malloc(sizeof(struct op_list));
			assert((olist != NULL) && "alloc olist");
			olist->next = NULL;
			olist->o = o;

			if (op16_root[i] == NULL) {
				op16_root[i] = olist;
				fn16_root[i] = fn;
			} else {
				fn16_root[i] = NULL;
				olist->next = op16_root[i];
				op16_root[i] = olist;
			}
		}
	}

	if (!inserted) {
		WARN("Registration for %s was never inserted\n", fn_name);
		WARN("\t ones: %04x\n", ones_mask);
		WARN("\tzeros: %04x\n", zeros_mask);
		WARN("\t  exc: %d\n", o->op16.ex_cnt);
		int k;
		for (k = 0; k < o->op16.ex_cnt; k++) {
			WARN("\t\tones: %04x zeros: %04x\n",
					o->op16.ex_ones[k], o->op16.ex_zeros[k]);
		}
		ERR(E_BAD_OPCODE, "Uncallable instruction registered?\n");
	}

	va_end(va_args);
	return ++opcode_masks;
}

EXPORT int register_opcode_mask_16_real(uint16_t ones_mask, uint16_t zeros_mask,
		void (*fn) (uint16_t), const char* fn_name) {
	return register_opcode_mask_16_ex_real(ones_mask, zeros_mask,
			fn, fn_name, 0, 0);
}

EXPORT int register_opcode_mask_32_ex_real(uint32_t ones_mask, uint32_t zeros_mask,
		void (*fn) (uint32_t), const char* fn_name, ...) {

	if ((zeros_mask & 0xffff0000) == 0) {
		WARN("%s registered zeros_mask requiring none of the top 4 bytes\n",
				fn_name);
		ERR(E_BAD_OPCODE, "Use register_opcode_mask_16 instead");
	}

	va_list va_args;
	va_start(va_args, fn_name);

	struct op *o = malloc(sizeof(struct op));

	o->name = fn_name;
	o->is16 = false;
	o->op32.ones_mask = ones_mask;
	o->op32.zeros_mask = zeros_mask;
	o->op32.fn = fn;

	o->op32.ex_cnt = 0;
	o->op32.ex_ones = NULL;
	o->op32.ex_zeros = NULL;

	uint32_t exc_ones_mask;
	uint32_t exc_zeros_mask;
	exc_ones_mask  = va_arg(va_args, uint32_t);
	exc_zeros_mask = va_arg(va_args, uint32_t);
	while (exc_ones_mask || exc_zeros_mask) {
		// Make the assumption that callers will have one, at most
		// two exceptions; go with the simple realloc scheme
		unsigned idx = o->op32.ex_cnt;

		o->op32.ex_cnt++;
		o->op32.ex_ones  = realloc(o->op32.ex_ones,  o->op32.ex_cnt * sizeof(uint32_t));
		assert(NULL != o->op32.ex_ones && "realloc");
		o->op32.ex_zeros = realloc(o->op32.ex_zeros, o->op32.ex_cnt * sizeof(uint32_t));
		assert(NULL != o->op32.ex_zeros && "realloc");

		o->op32.ex_ones[idx]  = exc_ones_mask;
		o->op32.ex_zeros[idx] = exc_zeros_mask;

		exc_ones_mask  = va_arg(va_args, uint32_t);
		exc_zeros_mask = va_arg(va_args, uint32_t);
	}

	////

	bool inserted = false;
	int i;
	uint8_t ones_bucket = (ones_mask >> 19) & 0xff;
	uint8_t zeros_bucket = (zeros_mask >> 19) & 0xff;
	for (i = 0; i < 256; i++) {
		if (
				match_mask32(ones_mask, 0xe8000000, 0x10000000) &&
				match_mask32(zeros_mask, 0x10000000, 0xe8000000)
		   ){
			if (match_mask8(i, ones_bucket, zeros_bucket)) {
				inserted = true;

				struct op_list* olist = malloc(sizeof(struct op_list));
				assert((olist != NULL) && "alloc olist");
				olist->next = NULL;
				olist->o = o;

				if (op32_11101_root[i] == NULL) {
					op32_11101_root[i] = olist;
					fn32_11101_root[i] = fn;
				} else {
					fn32_11101_root[i] = NULL;
					olist->next = op32_11101_root[i];
					op32_11101_root[i] = olist;
				}
			}
		} else if (
				match_mask32(ones_mask, 0xf0000000, 0x08000000) &&
				match_mask32(zeros_mask, 0x08000000, 0xf0000000)
			  ){
			if (match_mask8(i, ones_bucket, zeros_bucket)) {
				inserted = true;

				struct op_list* olist = malloc(sizeof(struct op_list));
				assert((olist != NULL) && "alloc olist");
				olist->next = NULL;
				olist->o = o;

				if (op32_11110_root[i] == NULL) {
					op32_11110_root[i] = olist;
					fn32_11110_root[i] = fn;
				} else {
					fn32_11110_root[i] = NULL;
					olist->next = op32_11110_root[i];
					op32_11110_root[i] = olist;
				}
			}
		} else if (
				match_mask32(ones_mask, 0xf8000000, 0x0) &&
				match_mask32(zeros_mask, 0x0, 0xf8000000)
			  ){
			if (match_mask8(i, ones_bucket, zeros_bucket)) {
				inserted = true;

				struct op_list* olist = malloc(sizeof(struct op_list));
				assert((olist != NULL) && "alloc olist");
				olist->next = NULL;
				olist->o = o;

				if (op32_11111_root[i] == NULL) {
					op32_11111_root[i] = olist;
					fn32_11111_root[i] = fn;
				} else {
					fn32_11111_root[i] = NULL;
					olist->next = op32_11111_root[i];
					op32_11111_root[i] = olist;
				}
			}
		} else {
			WARN("Troublesome: ones %08x zeros %08x name %s\n",
					ones_mask, zeros_mask, fn_name);
			ERR(E_BAD_OPCODE, "Legal 32-bit instructions begin with 111{01,10,11}\n");
		}
	}

	if (!inserted) {
		WARN("Registration for %s was never inserted\n", fn_name);
		WARN("\t ones: %08x\n", ones_mask);
		WARN("\tzeros: %08x\n", zeros_mask);
		WARN("\t  exc: %d\n", o->op32.ex_cnt);
		int k;
		for (k = 0; k < o->op32.ex_cnt; k++) {
			WARN("\t\tones: %08x zeros: %08x\n",
					o->op32.ex_ones[k], o->op32.ex_zeros[k]);
		}
		ERR(E_BAD_OPCODE, "Uncallable instruction registered?\n");
	}

	va_end(va_args);
	return ++opcode_masks;
}

EXPORT int register_opcode_mask_32_real(uint32_t ones_mask, uint32_t zeros_mask,
		void (*fn) (uint32_t), const char *fn_name) {
	return register_opcode_mask_32_ex_real(ones_mask, zeros_mask, fn, fn_name, 0, 0);
}

////

EXPORT void opcode_statistics(void) {
#ifdef DEBUG1
	int hist16[12] = {};
	int hist32_11101[12] = {};
	int hist32_11110[12] = {};
	int hist32_11111[12] = {};

	int i;
	for (i = 0; i < 256; i++) {
		int j = 0;
		struct op_list *olist = op16_root[i];
		while (olist != NULL) {
			j++;
			olist = olist->next;
		}
		assert(j < 12);
		hist16[j]++;
	}
	for (i = 0; i < 256; i++) {
		int j = 0;
		struct op_list *olist = op32_11101_root[i];
		while (olist != NULL) {
			j++;
			olist = olist->next;
		}
		assert(j < 12);
		hist32_11101[j]++;
	}
	for (i = 0; i < 256; i++) {
		int j = 0;
		struct op_list *olist = op32_11110_root[i];
		while (olist != NULL) {
			j++;
			olist = olist->next;
		}
		assert(j < 12);
		hist32_11110[j]++;
	}
	for (i = 0; i < 256; i++) {
		int j = 0;
		struct op_list *olist = op32_11111_root[i];
		while (olist != NULL) {
			j++;
			olist = olist->next;
		}
		assert(j < 12);
		hist32_11111[j]++;
	}

	flockfile(stdout); flockfile(stderr);
	_PP_EXTRA(stdout, 'D'); printf("hist16       ");
	for (i = 0; i < 12; i++)
		printf(" %2d: %3d", i, hist16[i]);
	printf("\n");
	_PP_EXTRA(stdout, 'D'); printf("hist32_11101 ");
	for (i = 0; i < 12; i++)
		printf(" %2d: %3d", i, hist32_11101[i]);
	printf("\n");
	_PP_EXTRA(stdout, 'D'); printf("hist32_11110 ");
	for (i = 0; i < 12; i++)
		printf(" %2d: %3d", i, hist32_11110[i]);
	printf("\n");
	_PP_EXTRA(stdout, 'D'); printf("hist32_11111 ");
	for (i = 0; i < 12; i++)
		printf(" %2d: %3d", i, hist32_11111[i]);
	printf("\n");
	funlockfile(stderr); funlockfile(stdout);
#endif
}
