#include "ram.h"

#include "memmap.h"
#include "core.h"

#include "../state.h"

#define ADDR_TO_IDX(_addr, _bot) ((_addr - _bot) >> 2)
static uint32_t ram[RAMSIZE >> 2] = {0};

static bool ram_read(uint32_t addr, uint32_t *val) {
#ifdef DEBUG1
	assert((addr >= RAMBOT) && (addr < RAMTOP) && "CORE_ram_read");
#endif
	if ((addr >= RAMBOT) && (addr < RAMTOP) && (0 == (addr & 0x3))) {
		*val = SR(&ram[ADDR_TO_IDX(addr, RAMBOT)]);
	} else {
		CORE_ERR_invalid_addr(false, addr);
	}

	return true;
}

static void ram_write(uint32_t addr, uint32_t val) {
#ifdef DEBUG1
	assert((addr >= RAMBOT) && (addr < RAMTOP) && "CORE_ram_write");
#endif
	if ((addr >= RAMBOT) && (addr < RAMTOP) && (0 == (addr & 0x3))) {
		SW(&ram[ADDR_TO_IDX(addr, RAMBOT)],val);
	} else {
		CORE_ERR_invalid_addr(true, addr);
	}
}

__attribute__ ((constructor))
void register_memmap_ram(void) {
#if (RAMBOT > RAMTOP)
#error "RAMBOT cannot be larger than RAMTOP"
#elif (RAMBOT == RAMTOP)
#pragma message ( "RAM of size 0. RAM has been disabled" )
#else
	union memmap_fn mem_fn;

	mem_fn.R_fn32 = ram_read;
	register_memmap("RAM", false, 4, mem_fn, RAMBOT, RAMTOP);
	mem_fn.W_fn32 = ram_write;
	register_memmap("RAM", true, 4, mem_fn, RAMBOT, RAMTOP);
#endif
}
