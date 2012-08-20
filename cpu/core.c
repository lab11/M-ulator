#include "core.h"

#include "private_peripheral_bus/ppb.h"
#include "pins.h"

//#define TRAP_ALIGNMENT (read_word(CONFIGURATION_CONTROL) & CONFIGURATION_CONTROL_UNALIGN_TRP_MASK)
#define TRAP_ALIGNMENT false

/* void reset(void)
 *
 * This function is called when the reset pin is triggered on the processor.
 * Recall that most M3s are Power-On Reset, so this is the first function that
 * will be called when the chip turns on
 */
void reset(void) {
	ppb_reset();
	CORE_reg_write(SP_REG, read_word(0x00000000));
	CORE_reg_write(LR_REG, 0xFFFFFFFF);
	CORE_reg_write(PC_REG, read_word(0x00000004));
}


////////////////////

// XXX: Build efficient structure with masks
struct memmap {
	struct memmap *next;
	union {
		bool (*R_fn)(uint32_t, uint32_t *);
		void (*W_fn)(uint32_t, uint32_t);
	};
	uint32_t bot;
	uint32_t top;
};


struct memmap reads = {0};
struct memmap writes = {0};

void register_memmap_read_word(
		bool (*fn)(uint32_t, uint32_t *),
		uint32_t bot,
		uint32_t top
	) {
	if (reads.R_fn == NULL) {
		reads.R_fn = fn;
		reads.bot = bot;
		reads.top = top;
	} else {
		struct memmap *cur = &reads;
		while (cur->next != NULL)
			cur = cur->next;
		cur->next = malloc(sizeof(struct memmap));
		cur = cur->next;
		cur->next = NULL;
		cur->R_fn = fn;
		cur->bot = bot;
		cur->top = top;
	}
}

void register_memmap_write_word(
		void (*fn)(uint32_t, uint32_t),
		uint32_t bot,
		uint32_t top
	) {
	if (writes.W_fn == NULL) {
		writes.W_fn = fn;
		writes.bot = bot;
		writes.top = top;
	} else {
		struct memmap *cur = &writes;
		while (cur->next != NULL)
			cur = cur->next;
		cur->next = malloc(sizeof(struct memmap));
		cur = cur->next;
		cur->next = NULL;
		cur->W_fn = fn;
		cur->bot = bot;
		cur->top = top;
	}
}

void print_memmap() {
	printf("\n");

	printf("READ MEMMAP\n");
	struct memmap *cur = &reads;
	while (cur != NULL) {
		printf("\t%08x...%08x\n", cur->bot, cur->top);
		cur = cur->next;
	}

	printf("WRITE MEMMAP\n");
	cur = &writes;
	while (cur != NULL) {
		printf("\t%08x...%08x\n", cur->bot, cur->top);
		cur = cur->next;
	}

	printf("\n");
}


static bool try_read_word(uint32_t addr, uint32_t *val) {
	DBG2("addr %08x\n", addr);

	struct memmap *cur = &reads;
	while (cur != NULL) {
		if ((cur->bot <= addr) && (addr < cur->top))
			return cur->R_fn(addr, val);
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

	struct memmap *cur = &writes;
	while (cur != NULL) {
		if ((cur->bot <= addr) && (addr < cur->top))
			return cur->W_fn(addr, val);
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

bool try_read_byte(uint32_t addr, uint8_t* ret) {
	uint32_t word;
	if (!try_read_word(addr & 0xfffffffc, &word))
		return false;

	DBG2("addr %08x, val %08x, case %d\n", addr, word, addr & 0x3);

	switch (addr & 0x3) {
		case 0:
			*ret = (word & 0x000000ff) >> 0;
			break;
		case 1:
			*ret = (word & 0x0000ff00) >> 8;
			break;
		case 2:
			*ret = (word & 0x00ff0000) >> 16;
			break;
		case 3:
			*ret = (word & 0xff000000) >> 24;
			break;
		default:
			//appease dumb compiler
			assert(false); return 0;
	}

	DBG2("returning %c\t%02x\n", *ret, *ret);

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
