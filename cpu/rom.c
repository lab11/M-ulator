#include "rom.h"

#include "memmap.h"
#include "core.h"

#include "../state.h"

#define ADDR_TO_IDX(_addr, _bot) ((_addr - _bot) >> 2)
static uint32_t rom[ROMSIZE >> 2] = {0};

EXPORT void flash_ROM(char *image, uint32_t nbytes) {
	memcpy(rom, image, nbytes);
	INFO("Flashed %d bytes to ROM\n", nbytes);
}

static bool rom_read(uint32_t addr, uint32_t *val) {
#ifdef DEBUG1
	assert((addr >= ROMBOT) && (addr < ROMTOP) && "CORE_rom_read");
#endif
	if ((addr >= ROMBOT) && (addr < ROMTOP) && (0 == (addr & 0x3))) {
		*val = SR(&rom[ADDR_TO_IDX(addr, ROMBOT)]);
	} else {
		CORE_ERR_invalid_addr(false, addr);
	}

	return true;
}

#ifdef WRITEABLE_ROM
static void rom_write(uint32_t addr, uint32_t val) {
	DBG2("ROM Write request addr %x (idx: %d)\n", addr, ADDR_TO_IDX(addr, ROMBOT));
#ifdef DEBUG1
	assert((addr >= ROMBOT) && (addr < ROMTOP) && "CORE_rom_write");
#endif
	if ((addr >= ROMBOT) && (addr < ROMTOP) && (0 == (addr & 0x3))) {
		SW(&rom[ADDR_TO_IDX(addr, ROMBOT)],val);
	} else {
		CORE_ERR_invalid_addr(true, addr);
	}
}
#endif

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
