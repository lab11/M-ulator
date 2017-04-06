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

#include "core.h"

#include "cpu/registers.h"

#include "common/private_peripheral_bus/ppb.h"

//#define TRAP_ALIGNMENT (read_word(CONFIGURATION_CONTROL) & CONFIGURATION_CONTROL_UNALIGN_TRP_MASK)
#define TRAP_ALIGNMENT false

/* void reset(void)
 *
 * This function is called when the reset pin is triggered on the processor.
 * Recall that most M3s are Power-On Reset, so this is the first function that
 * will be called when the chip turns on
 */
void print_memmap(void);

struct reset_fn {
	struct reset_fn *next;
	void (*fn)(void);
};

struct reset_fn* reset_head = NULL;

EXPORT void register_reset(void (*fn)(void)) {
	struct reset_fn* n = malloc(sizeof(struct reset_fn));
	n->fn = fn;
	n->next = reset_head;

	reset_head = n;
}

EXPORT _Atomic _Bool _CORE_in_reset = false;
EXPORT void reset(void) {
	atomic_store(&_CORE_in_reset, true);
#ifdef DEBUG1
	print_memmap();
#endif

	// ARM ARM B1.5.5 Reset behavior (p641)

	// Clear all exceptions
	//  --> registers.c::reset

	struct reset_fn* r = reset_head;
	while (r) {
		r->fn();
		r = r->next;
	}

	atomic_store(&_CORE_in_reset, false);
}


////////////////////

// XXX: Build efficient structure with masks
struct memmap {
	struct memmap *next;
	struct memmap *prev;
	const char *name;
	union memmap_fn mem_fn;
	uint32_t bot;
	uint32_t top;
	char alignment;
};

struct memmap *reads = NULL;
struct memmap *writes = NULL;

static void bad_memmap_reg(struct memmap *newmap, struct memmap *cur) {
	WARN("Inserting %s at %x--%x, but cur is %s at %x--%x\n",
			newmap->name, newmap->bot, newmap->top,
			cur->name, cur->bot, cur->top);
	ERR(E_INVALID_ADDR, "\
Bad memmap registration, overlapping address range\n");
}

static void insert_memmap_behind(struct memmap **head, struct memmap *newmap,
		struct memmap *cur) {
	// Overlap check: Equal is allowed, range is [bot, top)
	if (newmap->top > cur->bot) {
		bad_memmap_reg(newmap, cur);
	}

	newmap->prev = cur->prev;
	newmap->next = cur;
	if (cur->prev) {
		assert(cur->prev->next == cur);
		cur->prev->next = newmap;
	} else {
		assert(*head == cur);
		*head = newmap;
	}
	cur->prev = newmap;
}

EXPORT void register_memmap(
		const char *name,
		bool write,
		short alignment,
		union memmap_fn mem_fn,
		uint32_t bot,
		uint32_t top
	) {
	assert(bot < top);

	struct memmap *newmap = malloc(sizeof(struct memmap));
	*newmap = (struct memmap){
		NULL, NULL, strdup(name), mem_fn, bot, top, alignment};
	assert(newmap->name);

	struct memmap **head;
	struct memmap *cur;

	if (write)
		head = &writes;
	else
		head = &reads;
	cur = *head;

	if (cur == NULL) {
		*head = newmap;
		return;
	}

	while (cur->next != NULL) {
		// Insert behind cur
		if (bot < cur->bot) {
			insert_memmap_behind(head, newmap, cur);
			return;
		}

		// Overlap check: ensure new range completely exceeds old
		if (bot < cur->top) {
			bad_memmap_reg(newmap, cur);
		}

		cur = cur->next;
	}

	// Insert at end of list
	if (bot < cur->top) {
		insert_memmap_behind(head, newmap, cur);
	} else {
		cur->next = newmap;
		newmap->prev = cur;
	}
}

static void print_memmap_line(
		bool rvalid, uint32_t rval,
		bool wvalid, uint32_t wval) {
	printf("\t");
	if (rvalid)
		printf("|%08x\t\t", rval);
	else
		printf("|\t\t\t");
	if (wvalid)
		printf("|%08x\t\t", wval);
	else
		printf("|\t\t\t");
	printf("\n");
}

static void print_memmap_name(
		bool rvalid, struct memmap *rcur,
		bool wvalid, struct memmap *wcur) {
	printf("\t");
	if (rvalid)
		printf("| | %-20s", rcur->name);
	else
		printf("|\t\t\t");
	if (wvalid)
		printf("| | %-20s", wcur->name);
	else
		printf("|\t\t\t");
	printf("\n");
}

EXPORT void print_memmap(void) {
	INFO("Printing memory map\n");

	struct memmap *rcur = reads;
	struct memmap *wcur = writes;

	if ((rcur == NULL) && (wcur == NULL)) {
		WARN("******************************************************\n");
		WARN("Empty memmap!!\n");
		WARN("\tYou have probably misconfigured something, there is\n");
		WARN("\tno memory attached to this core. Have you configured\n");
		WARN("\t/platforms/YOUR_PLATFORM/simulator/memmap.h ?\n");
		WARN("******************************************************\n");
	}

	bool rvalid = false;
	bool wvalid = false;

	while (rcur || wcur) {
		if (!rvalid && !wvalid) {
			printf("\t|\t\t\t|\n");
			uint32_t rbot = (rcur) ? rcur->bot : UINT32_MAX;
			uint32_t wbot = (wcur) ? wcur->bot : UINT32_MAX;

			if (rbot < wbot) {
				rvalid = true;
				wvalid = false;
			} else if (rbot > wbot) {
				rvalid = false;
				wvalid = true;
			} else {
				rvalid = true;
				wvalid = true;
			}
			print_memmap_line(rvalid, rbot, wvalid, wbot);
			print_memmap_name(rvalid, rcur, wvalid, wcur);
			continue;
		}

		else if (rvalid && !wvalid) {
			if (wcur && wcur->bot < rcur->top) {
				wvalid = true;
				print_memmap_line(false, 0, wvalid, wcur->bot);
				print_memmap_name(false, rcur, wvalid, wcur);
				continue;
			} else if ((!wcur) || (wcur && wcur->bot > rcur->top)) {
				print_memmap_line(rvalid, rcur->top, false, 0);
				rvalid = false;
				rcur = rcur->next;
				continue;
			} else {
				assert(wcur->bot == rcur->top);
				wvalid = true;
				rcur = rcur->next;
				if (rcur && (rcur->bot == wcur->bot))
					rvalid = true;
				else
					rvalid = false;
				print_memmap_line(true, wcur->bot, wvalid, wcur->bot);
				print_memmap_name(rvalid, rcur, wvalid, wcur);
				continue;
			}
		}

		else if (!rvalid && wvalid) {
			if (rcur && rcur->bot < wcur->top) {
				rvalid = true;
				print_memmap_line(rvalid, rcur->bot, false, 0);
				print_memmap_name(rvalid, rcur, false, wcur);
				continue;
			} else if ((!rcur) || (rcur && rcur->bot > wcur->top)) {
				print_memmap_line(false, 0, wvalid, wcur->top);
				wvalid = false;
				wcur = wcur->next;
				continue;
			} else {
				assert(rcur->bot == wcur->top);
				rvalid = true;
				wcur = wcur->next;
				if (wcur && (wcur->bot == rcur->bot))
					wvalid = true;
				else
					wvalid = false;
				print_memmap_line(rvalid, rcur->bot, true, rcur->bot);
				print_memmap_name(rvalid, rcur, wvalid, wcur);
				continue;
			}
		}

		else if (rvalid && wvalid) {
			if (rcur->top > wcur->top) {
				print_memmap_line(false, 0, wvalid, wcur->top);
				if (!(wcur->next && wcur->next->bot == wcur->top))
					wvalid = false;
				wcur = wcur->next;
				continue;
			} else if (rcur->top < wcur->top) {
				print_memmap_line(rvalid, rcur->top, false, 0);
				if (!(rcur->next && rcur->next->bot == rcur->top))
					rvalid = false;
				rcur = rcur->next;
				continue;
			} else {
				print_memmap_line(rvalid, rcur->top, wvalid, wcur->top);
				rvalid = false;
				wvalid = false;
				rcur = rcur->next;
				wcur = wcur->next;
				continue;
			}
		}
	}

	printf("\n");
}

static bool try_read_word(uint32_t addr, uint32_t *val, bool debugger) {
	struct memmap *cur = reads;
	while (cur != NULL) {
		if (cur->alignment == 4)
			if ((cur->bot <= addr) && (addr < cur->top))
				return cur->mem_fn.R_fn32(addr, val, debugger);
		cur = cur->next;
	}

	if (CONF_rzwi_memory) {
		WARN("RZWI RD 0x%08x as 0\n", addr);
		return true;
	}

	DBG1("addr %08x falls outside known range\n", addr);
	return false;

	/*
	} else if (addr >= REGISTERS_BOT && addr < REGISTERS_TOP) {
		*val = ppb_read(addr);
	*/
}

EXPORT uint32_t read_word_quiet(uint32_t addr) {
	uint32_t val;
	if (try_read_word(addr, &val, false)) {
		return val;
	} else {
		MEMTRACE_READ_ERR(4, addr)
		print_memmap();
		CORE_ERR_invalid_addr(false, addr);
	}
}

EXPORT uint32_t read_word(uint32_t addr) {
	uint32_t val;
	if (try_read_word(addr, &val, false)) {
		MEMTRACE_READ(4, addr, val);
		return val;
	} else {
		MEMTRACE_READ_ERR(4, addr)
		print_memmap();
		CORE_ERR_invalid_addr(false, addr);
	}
}

static void try_write_word(uint32_t addr, uint32_t val, bool debugger) {
	DBG2("addr %08x val %08x\n", addr, val);

	struct memmap *cur = writes;
	while (cur != NULL) {
		if (cur->alignment == 4) {
			if ((cur->bot <= addr) && (addr < cur->top)) {
				MEMTRACE_WRITE(4, addr, val);
				return cur->mem_fn.W_fn32(addr, val, debugger);
			}
		}
		cur = cur->next;
	}

	if (CONF_rzwi_memory) {
		WARN("RZWI WR 0x%08x = 0x%08x\n", addr, val);
		return;
	}

	MEMTRACE_WRITE_ERR(4, addr, val);
	print_memmap();
	CORE_ERR_invalid_addr(true, addr);

	/*
	} else if (addr >= REGISTERS_BOT && addr < REGISTERS_TOP) {
		ppb_write(addr, val);
	*/
}

EXPORT void write_word(uint32_t addr, uint32_t val) {
	return try_write_word(addr, val, false);
}

EXPORT void write_word_aligned(uint32_t addr, uint32_t val) {
	return try_write_word(addr, val, false);
}

unsigned core_stats_unaligned_cycle_penalty = 0;
EXPORT void write_word_unaligned(uint32_t addr, uint32_t val) {
	if ( likely((addr & 0x3) == 0) ) {
		return try_write_word(addr, val, false);
	} else if (TRAP_ALIGNMENT) {
		union ufsr_t ufsr = CORE_ufsr_read();
		ufsr.UNALIGNED = 1;
		CORE_ufsr_write(ufsr);
		CORE_ERR_not_implemented("Trap Alignment exception (B2.2.5, p697)\n");
	} else {
		// XXX: Read AIRCR.ENDIANNESS. Currently assumes little endian

		if (core_stats_unaligned_cycle_penalty == 0) {
			WARN("Unaligned access writing 0x%08x = 0x%08x\n", addr, val);
			WARN("Cortex-M's convert unaligned word writes into four 1-byte writes\n");
			WARN("This warning will only issue once\n");
		}
		core_stats_unaligned_cycle_penalty += 3*2;

		write_byte(addr, val & 0xff);
		write_byte(addr + 1, (val >> 8) & 0xff);
		write_byte(addr + 2, (val >> 16) & 0xff);
		write_byte(addr + 3, (val >> 24) & 0xff);
	}
}

EXPORT uint16_t read_halfword(uint32_t addr) {
	DBG2("addr %08x\n", addr);

	if ((addr & 0x1) & TRAP_ALIGNMENT) {
		assert(false && "Alignment exception");
	}

	uint32_t word = read_word(addr & 0xfffffffc);

	uint16_t ret;

	switch (addr & 0x3) {
		case 0x0:
			ret = (word & 0x0000ffff) >> 0;
			break;
		case 0x1:
			ret = (word & 0x00ffff00) >> 8;
			break;
		case 0x2:
			ret = (word & 0xffff0000) >> 16;
			break;
		case 0x3:
			ret = (word & 0xff000000) >> 24;
			ret |= (read_byte(addr + 1) << 8);
			break;
	}

	DBG2("read_halfword: returning %d\t0x%04x\n", ret, ret);

	return ret;
}

EXPORT void write_halfword(uint32_t addr, uint16_t val) {
	DBG2("addr %08x val %04x\n", addr, val);

	// XXX: This doesn't look right, esp w.r.t. alignment stuff

	if ((addr & 0x1) & TRAP_ALIGNMENT) {
		// misaligned access
		assert(false && "Alignment exception");
	}

	uint32_t word = read_word(addr & 0xfffffffc);
	uint32_t val32 = val;

	switch (addr & 0x3) {
		case 0x0:
			word &= 0xffff0000;
			word |= val32;
			break;
		case 0x1:
			word &= 0xff0000ff;
			word |= val32;
			break;
		case 0x2:
			word &= 0x0000ffff;
			word |= (val32 << 16);
			break;
		case 0x3:
			word &= 0x00ffffff;
			word |= (val32 << 24);
			write_byte(addr + 1, val32 >> 8);
			break;
	}

	write_word(addr & 0xfffffffc, word);
}

EXPORT void write_halfword_unaligned(uint32_t addr, uint16_t val) {
	if ( likely((addr & 0x1) == 0) ) {
		return write_halfword(addr, val);
	} else if (TRAP_ALIGNMENT) {
		union ufsr_t ufsr = CORE_ufsr_read();
		ufsr.UNALIGNED = 1;
		CORE_ufsr_write(ufsr);
		CORE_ERR_not_implemented("Trap Alignment exception (B2.2.5, p697)\n");
	} else {
		// XXX: Read AIRCR.ENDIANNESS. Currently assumes little endian

		if (core_stats_unaligned_cycle_penalty == 0) {
			WARN("Unaligned access writing 0x%04x = 0x%04x\n", addr, val);
			WARN("Cortex-M's convert unaligned writes into 1-byte writes\n");
			WARN("This warning will only issue once\n");
		}
		core_stats_unaligned_cycle_penalty += 1*2;

		write_byte(addr, val & 0xff);
		write_byte(addr + 1, (val >> 8) & 0xff);
	}
}

static bool try_read_byte(uint32_t addr, uint8_t* val, bool debugger) {
	struct memmap *cur = reads;
	while (cur != NULL) {
		if (cur->alignment == 1)
			if ((cur->bot <= addr) && (addr < cur->top))
				return cur->mem_fn.R_fn8(addr, val, debugger);
		cur = cur->next;
	}

	uint32_t word;
	if (!try_read_word(addr & 0xfffffffc, &word, debugger))
		return false;

	switch (addr & 0x3) {
		case 0:
			*val = (word & 0x000000ff) >> 0;
			break;
		case 1:
			*val = (word & 0x0000ff00) >> 8;
			break;
		case 2:
			*val = (word & 0x00ff0000) >> 16;
			break;
		case 3:
			*val = (word & 0xff000000) >> 24;
			break;
		default:
			//appease dumb compiler
			assert(false); return 0;
	}

	return true;
}

EXPORT bool gdb_read_byte(uint32_t addr, uint8_t* val) {
	return try_read_byte(addr, val, true);
}

EXPORT uint8_t read_byte(uint32_t addr) {
	uint8_t val;
	if (try_read_byte(addr, &val, false)) {
		return val;
	} else {
		CORE_ERR_unpredictable("read_byte failed unexpectedly\n");
	}
}

static void try_write_byte(uint32_t addr, uint8_t val, bool debugger) {
	struct memmap *cur = writes;
	while (cur != NULL) {
		if (cur->alignment == 1)
			if ((cur->bot <= addr) && (addr < cur->top))
				return cur->mem_fn.W_fn8(addr, val, debugger);
		cur = cur->next;
	}

	uint32_t word = read_word(addr & 0xfffffffc);
	uint32_t val32 = val;

	switch (addr & 0x3) {
		case 0:
			word &= 0xffffff00;
			word |= val32;
			break;
		case 1:
			word &= 0xffff00ff;
			word |= (val32 << 8);
			break;
		case 2:
			word &= 0xff00ffff;
			word |= (val32 << 16);
			break;
		case 3:
			word &= 0x00ffffff;
			word |= (val32 << 24);
			break;
	}

	try_write_word(addr & 0xfffffffc, word, debugger);
}

EXPORT void gdb_write_byte(uint32_t addr, uint8_t val) {
	return try_write_byte(addr, val, true);
}

EXPORT void write_byte(uint32_t addr, uint8_t val) {
	return try_write_byte(addr, val, false);
}
