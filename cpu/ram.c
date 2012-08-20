#include "ram.h"

#include "memmap.h"
#include "cpu.h"
#include "bus_interface.h"

static bool ram_read(uint32_t addr, uint32_t *val) {
	*val = CORE_ram_read(addr);
	return true;
}

static void ram_write(uint32_t addr, uint32_t val) {
	CORE_ram_write(addr, val);
}

__attribute__ ((constructor))
void register_memmap_ram(void) {
#if (RAMBOT > RAMTOP)
#error "RAMBOT cannot be larger than RAMTOP"
#elif (RAMBOT == RAMTOP)
#pragma message ( "RAM of size 0. RAM has been disabled" )
#else
	register_memmap_read_word(ram_read, RAMBOT, RAMTOP);
	register_memmap_write_word(ram_write, RAMBOT, RAMTOP);
#endif
}
