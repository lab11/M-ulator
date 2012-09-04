#include "core.h"

#include "private_peripheral_bus/ppb.h"
#include "pins.h"
#include "cpu.h"

//#define TRAP_ALIGNMENT (read_word(CONFIGURATION_CONTROL) & CONFIGURATION_CONTROL_UNALIGN_TRP_MASK)
#define TRAP_ALIGNMENT false

/* void reset(void)
 *
 * This function is called when the reset pin is triggered on the processor.
 * Recall that most M3s are Power-On Reset, so this is the first function that
 * will be called when the chip turns on
 */
void print_memmap(void);

void reset(void) {
	print_memmap();
	ppb_reset();
	CORE_reg_write(SP_REG, read_word(0x00000000));
	CORE_reg_write(LR_REG, 0xFFFFFFFF);
	CORE_reg_write(PC_REG, read_word(0x00000004));
}


////////////////////

// XXX: Build efficient structure with masks
struct memmap {
	struct memmap *next;
	struct memmap *prev;
	union memmap_fn mem_fn;
	uint32_t bot;
	uint32_t top;
	char alignment;
};

struct memmap *reads = NULL;
struct memmap *writes = NULL;

static void bad_memmap_reg(struct memmap *newmap, struct memmap *cur) {
	WARN("Inserting %x--%x, but cur is %x--%x\n",
			newmap->bot, newmap->top, cur->bot, cur->top);
	ERR(E_INVALID_ADDR, "\
Bad memmap registration, overlapping address range\n");
}

static void insert_memmap(struct memmap **head, struct memmap *newmap,
		struct memmap *cur) {
	// Overlap check: Equal is allowed, range is [bot, top)
	if (newmap->top > cur->bot) {
		bad_memmap_reg(newmap, cur);
	}
	if (cur->prev) {
		assert(cur->prev->next = cur);
		cur->prev->next = newmap;
		cur->prev = newmap;
	}
	newmap->next = cur;
	newmap->prev = cur->prev;

	if (*head == cur)
		*head = newmap;
}

void register_memmap(
		bool write,
		short alignment,
		union memmap_fn mem_fn,
		uint32_t bot,
		uint32_t top
	) {
	assert(bot < top);
	struct memmap *newmap = malloc(sizeof(struct memmap));
	*newmap = (struct memmap){NULL, NULL, mem_fn, bot, top, alignment};

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
			insert_memmap(head, newmap, cur);
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
		insert_memmap(head, newmap, cur);
	} else {
		cur->next = newmap;
		newmap->prev = cur;
	}
}

void print_memmap(void) {
	printf("\n");

	printf("READ MEMMAP\n");
	struct memmap *cur = reads;
	while (cur != NULL) {
		printf("\t%08x...%08x (align %d)\n",
				cur->bot, cur->top, cur->alignment);
		cur = cur->next;
	}

	printf("WRITE MEMMAP\n");
	cur = writes;
	while (cur != NULL) {
		printf("\t%08x...%08x (align %d)\n",
				cur->bot, cur->top, cur->alignment);
		cur = cur->next;
	}

	printf("\n");
}


static bool try_read_word(uint32_t addr, uint32_t *val) {
	DBG2("addr %08x\n", addr);

	struct memmap *cur = reads;
	while (cur != NULL) {
		if (cur->alignment == 4)
			if ((cur->bot <= addr) && (addr < cur->top))
				return cur->mem_fn.R_fn32(addr, val);
		cur = cur->next;
	}

	DBG1("addr %08x falls outside known range\n", addr);
	return false;

	/*
	} else if (addr >= REGISTERS_BOT && addr < REGISTERS_TOP) {
		*val = ppb_read(addr);
	*/
}

uint32_t read_word(uint32_t addr) {
	uint32_t val;
	if (try_read_word(addr, &val)) {
		return val;
	} else {
		print_memmap();
		CORE_ERR_invalid_addr(false, addr);
	}
}

void write_word(uint32_t addr, uint32_t val) {
	DBG2("addr %08x val %08x\n", addr, val);

	struct memmap *cur = writes;
	while (cur != NULL) {
		if (cur->alignment == 4)
			if ((cur->bot <= addr) && (addr < cur->top))
				return cur->mem_fn.W_fn32(addr, val);
		cur = cur->next;
	}

	print_memmap();
	CORE_ERR_invalid_addr(true, addr);

	/*
	} else if (addr >= REGISTERS_BOT && addr < REGISTERS_TOP) {
		ppb_write(addr, val);
	*/
}

uint16_t read_halfword(uint32_t addr) {
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

void write_halfword(uint32_t addr, uint16_t val) {
	DBG2("addr %08x val %04x\n", addr, val);

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

bool try_read_byte(uint32_t addr, uint8_t* val) {

	struct memmap *cur = reads;
	while (cur != NULL) {
		if (cur->alignment == 1)
			if ((cur->bot <= addr) && (addr < cur->top))
				return cur->mem_fn.R_fn8(addr, val);
		cur = cur->next;
	}

	uint32_t word;
	if (!try_read_word(addr & 0xfffffffc, &word))
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

uint8_t read_byte(uint32_t addr) {
	uint8_t val;
	if (try_read_byte(addr, &val)) {
		return val;
	} else {
		CORE_ERR_unpredictable("read_byte failed unexpectedly\n");
	}
}

void write_byte(uint32_t addr, uint8_t val) {
	struct memmap *cur = writes;
	while (cur != NULL) {
		if (cur->alignment == 1)
			if ((cur->bot <= addr) && (addr < cur->top))
				return cur->mem_fn.W_fn8(addr, val);
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

	write_word(addr & 0xfffffffc, word);
}
