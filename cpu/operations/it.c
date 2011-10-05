#include "opcodes.h"

#include "../misc.h"

int it(uint32_t inst) {
	uint8_t itstate = inst & 0xff;
	write_itstate(itstate);

	DBG2("it{xxx} wrote itstate: %02x\n", itstate);

	return SUCCESS;
}

void register_opcodes_it(void) {
	// Complicated encoding...
	// 1011 1111 firstcond mask
	// mask cannot be 0000
	// firstcond cannot be 1111
	// if firstcont is 1110, then hamming(mask) must be 1

	// The only instr we need to match now is bf03
	//register_opcode_mask(0xbf03, 0xffff40fc, it);
	// 1011 1111 <x's>
	register_opcode_mask(0xbf00, 0xffff4000, it);
}
