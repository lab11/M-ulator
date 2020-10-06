/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2017  Pat Pannuto <pat.pannuto@gmail.com>
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

#ifndef HAVE_DECOMPILE
#error Requires a decompile build
#endif

#include <ctype.h>
#include <getopt.h>
#include <math.h>

// For access to 'raiseonerror' flag
#define PP_STRING "DEC"
#include "core/pretty_print.h"

#include "core/common.h"
#include "core/opcodes.h"
#include "core/simulator.h"

#include "cpu/core.h"
#include "cpu/exception.h"
#include "cpu/registers.h"


///////////////////////////////////////////////////////////////////////
// Fake Core
unsigned core_stats_unaligned_cycle_penalty = 0;

EXPORT void reset(void) {
	printf("Core reset.\n");
}

// registers
static uint32_t _regs[15] = {0};

EXPORT uint32_t CORE_reg_read(int r) {
	uint32_t ret = 0;
	printf("REG  read %d", r);
	printf("\t\t\tvalue to read (hex) [0x%08x]: ", _regs[r]);
	scanf("%x", &ret);
	return ret;
}

EXPORT void CORE_reg_write(int r, uint32_t val) {
	printf("REG write %d <= 0x%08x\n", r, val);
	_regs[r] = val;
}

EXPORT uint32_t CORE_xPSR_read(void) {
	printf("REG xPSR read\n");
	return 0;
}

EXPORT void CORE_xPSR_write(uint32_t xPSR) {
	printf("REG xPSR write <= 0x%08x\n", xPSR);
}

EXPORT enum Mode CORE_CurrentMode_read(void) {
	printf("Read Mode (returning Handler)\n");
	return Mode_Handler;
}

EXPORT void CORE_CurrentMode_write(enum Mode mode) {
	printf("Write Mode <= 0x%08x\n", mode);
}

EXPORT union apsr_t CORE_apsr_read(void) {
	printf("Read APSR\n");
	union apsr_t a = {0};
	return a;
}

EXPORT void CORE_apsr_write(union apsr_t val) {
	printf("Write APSR <= 0x%08x\n", val.storage);
}

EXPORT union ipsr_t CORE_ipsr_read(void) {
	printf("Read IPSR\n");
	union ipsr_t i = {0};
	return i;
}

EXPORT void CORE_ipsr_write(union ipsr_t val) {
	printf("Write IPSR <= 0x%08x\n", val.storage);
}

EXPORT union epsr_t CORE_epsr_read(void) {
	printf("Read EPSR\n");
	union epsr_t e = {0};
	return e;
}

EXPORT void CORE_epsr_write(union epsr_t val) {
	printf("Write EPSR <= 0x%08x\n", val.storage);
}

EXPORT bool CORE_control_nPRIV_read(void) {
	printf("read nPRIV\n");
	union control_t c = {0};
	return c.nPRIV;
}

EXPORT void CORE_control_nPRIV_write(bool npriv) {
	printf("write nPRIV <= 0x%08x\n", npriv);
}

EXPORT bool CORE_control_SPSEL_read(void) {
	printf("SPSEL read\n");
	union control_t c = {0};
	return c.SPSEL;
}
EXPORT void CORE_control_SPSEL_write(bool spsel) {
	printf("SPSEL write <= 0x%x\n", spsel);
}

EXPORT void CORE_update_mode_and_SPSEL(enum Mode mode, bool spsel) {
	CORE_CurrentMode_write(mode);
	CORE_control_SPSEL_write(spsel);
}

EXPORT bool CORE_primask_read(void) {
	printf("primask read\n");
	return 0;
}

EXPORT void CORE_primask_write(bool val) {
	printf("primask write <= 0x%x\n", val);
}

EXPORT uint8_t CORE_basepri_read(void) {
	printf("basepri read\n");
	return 0;
}

EXPORT void CORE_basepri_write(uint8_t val) {
	printf("basepri write <= 0x%02x\n", val);
}

EXPORT bool CORE_faultmask_read(void) {
	printf("faultmask read\n");
	return false;
}

EXPORT void CORE_faultmask_write(bool val) {
	printf("faultmask write <= 0x%x\n", val);
}

EXPORT union ufsr_t CORE_ufsr_read(void) {
	printf("ufsr read\n");
	union ufsr_t u = {0};
	return u;
}

EXPORT void CORE_ufsr_write(union ufsr_t u) {
	printf("ufst write <= 0x%08x\n", u.storage);
}


// core
EXPORT _Atomic _Bool _CORE_in_reset = false;
EXPORT uint32_t read_word_quiet(uint32_t addr __attribute__((unused))) {
	return 0;
}

EXPORT uint32_t read_word(uint32_t addr) {
	uint32_t ret = 0;
	printf("MEM  read addr 0x%08x", addr);
	printf("\tvalue to read (hex): ");
	scanf("%x", &ret);
	return ret;
}
EXPORT void write_word(uint32_t addr, uint32_t val) {
	printf("MEM write addr 0x%08x <= 0x%08x\n", addr, val);
}
EXPORT void write_word_aligned(uint32_t addr, uint32_t val) {
	return write_word(addr, val);
}
EXPORT void write_word_unaligned(uint32_t addr, uint32_t val) {
	return write_word(addr, val);
}
EXPORT uint16_t read_halfword(uint32_t addr) {
	printf("MEM  read addr 0x%04x (halfword)\n", addr);
	return 0;
}
EXPORT void write_halfword(uint32_t addr, uint16_t val) {
	printf("MEM write addr 0x%08x <= 0x%04x (halfword)\n", addr, val);
}
EXPORT void write_halfword_unaligned(uint32_t addr, uint16_t val) {
	return write_halfword(addr, val);
}
EXPORT uint8_t read_byte(uint32_t addr) {
	printf("MEM  read addr 0x%02x (byte)\n", addr);
	return 0;
}
EXPORT bool gdb_read_byte(uint32_t addr, uint8_t* val) {
	printf("GDB!! ");
	*val = read_byte(addr);
	return true;
}
EXPORT void write_byte(uint32_t addr, uint8_t val) {
	printf("MEM write addr 0x%08x <= 0x%02x (byte)\n", addr, val);
}
EXPORT void gdb_write_byte(uint32_t addr, uint8_t val) {
	printf("GDB!! ");
	return write_byte(addr, val);
}


// memory
EXPORT size_t dump_RAM(FILE *fp __attribute__((unused))) {
	assert(false && "Not Impl");
	return 0;
}
EXPORT void flash_ROM(
		const uint8_t *image __attribute__((unused)),
		int offset __attribute__((unused)),
		uint32_t nbytes __attribute__((unused))) {
	assert(false && "falsh rom not impl");
}

EXPORT void register_memmap(
		const char *name __attribute__((unused)),
		bool write __attribute__((unused)),
		short alignment __attribute__((unused)),
		union memmap_fn mem_fn __attribute__((unused)),
		uint32_t bot __attribute__((unused)),
		uint32_t top __attribute__((unused))
	) {
}
EXPORT void register_reset(void (*fn)(void) __attribute__((unused))) {
}

// misc
EXPORT uint8_t read_itstate(void) {
	printf("read itstate\n");
	return 0;
}
uint8_t in_ITblock(void) {
	return (read_itstate() & 0xf) != 0;
}
uint8_t last_in_ITblock(void) {
	return ((read_itstate() & 0xf) == 0x8);
}
void IT_advance(void) {}
uint8_t eval_cond(union apsr_t apsr, uint8_t cond) {
	printf("eval cond apsr 0x%08x cond 0x%02x\n", apsr.storage, cond);
	return 0;
}
void write_itstate(uint8_t new_state) {
	printf("write itstate <= 0x%02x\n", new_state);
}

// exception
void generic_exception(enum ExceptionType type, bool precise,
		uint32_t fault_inst, uint32_t next_inst) {
	printf("type %d precise %d fault inst 0x%08x next inst 0x%08x\n",
			type, precise, fault_inst, next_inst);
	assert(false && "exception?");
}
void UsageFault_divide_by_0(void) {
	assert(false && "div 0 exception");
}
void exception_return(uint32_t exception_return_pc) {
	printf("pc 0x%08x\n", exception_return_pc);
	assert(false && "exception return");
}

///////////////////////////////////////////////////////////////////////
// Main Program

static void usage_fail(int retcode) {
	printf("\nUSAGE: ./decompiler [OPTS]\n\n");
	exit(retcode);
}

static void usage(void) {
	usage_fail(0);
}

int main(int argc, char **argv) {
	// Command line parsing
	while (1) {
		static struct option long_options[] = {
			{"help",          no_argument,       0,              '?'},
			{0,0,0,0}
		};
		int option_index = 0;
		int c;

		char optstring[64] = "?";

		c = getopt_long(argc, argv, optstring, long_options, &option_index);

		if (c == -1) break;

		switch (c) {
			case 0:
				// option set a flag
				break;

			case '?':
			default:
				usage();
				break;
		}
	}

	CONF_rzwi_memory = true;
	decompile_flag = false;
	INFO("Registered %d opcode mask%s\n", get_opcode_masks(),
			(get_opcode_masks() == 1) ? "":"s");

	while (true) {
		uint32_t inst;
		struct op* op;
		int ret;

		printf("\ninst (hex) > ");
		ret = scanf("%x", &inst);
		if (ret != 1) break;

		// Decode
		op = find_op(inst);
		if (NULL == op) {
			printf("No handler for %08x\n", inst);
			continue;
		}
		printf("%s\n", op->name);

		// Execute
		if (op->is16) op->op16.fn(inst);
		else op->op32.fn(inst);
	}
}
