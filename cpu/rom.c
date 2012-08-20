#include "rom.h"

#include "memmap.h"
#include "cpu.h"
#include "bus_interface.h"

static bool rom_read(uint32_t addr, uint32_t *val) {
	*val = CORE_rom_read(addr);
	return true;
}

static void rom_write(uint32_t addr, uint32_t val) {
	CORE_rom_write(addr, val);
}

__attribute__ ((constructor))
void register_memmap_rom(void) {
#if (ROMBOT > ROMTOP)
#error "ROMBOT cannot be larger than ROMTOP"
#elif (ROMBOT == ROMTOP)
#pragma message ( "ROM of size 0. ROM has been disabled" )
#else
	register_memmap_read_word(rom_read, ROMBOT, ROMTOP);
#ifdef WRITEABLE_ROM
	register_memmap_write_word(rom_write, ROMBOT, ROMTOP);
#endif
#endif
}
