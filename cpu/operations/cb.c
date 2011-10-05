#include "opcodes.h"

int cb(uint32_t inst __attribute__ ((unused))) {
	/*
	int32_t imm32 = ((inst & 0x200) >> 4) | ((inst & 0xf8) >> 3);
	uint8_t rn = inst & 0x7;

	uint32_t rn_val = CORE_reg_read(rn);
	*/
	CORE_ERR_not_implemented("CB{N}Z"); return 0;
}

void register_opcodes_cb(void) {
	// b f 0 3, 1011 1111 0000 0011

	// cb{N}Z: 1011 x0x1 <x's>
	register_opcode_mask(0xb100, 0xffff4400, cb);
}
