#include "../cortex_m3.h"

int pop(uint32_t inst) {
	uint32_t sp = CORE_reg_read(SP_REG);

	int hamming = 0;
	{
		int reg_list = inst & 0xff;
		for (hamming = 0; reg_list; hamming++) {
			reg_list &= (reg_list - 1); // clears LSB
		}
		hamming += !!(inst & 0x100); // PC?

		assert((hamming > 0) && (hamming <= 9));
	}

	uint32_t start_address = sp;
	uint32_t end_address = sp + 4*hamming;
	uint32_t address = start_address;

	int i;
	for (i = 0; i < 8; i++) {
		if (inst & (1 << i)) {
			CORE_reg_write(i, read_word(address));
			address += 4;
		}
	}
	if (inst & 0x100) {
		uint32_t val = read_word(address);
		// val &= 0xfffffffe	// simulator allows thumb bit
		CORE_reg_write(PC_REG, val);
		address += 4;
	}

	assert ((end_address == address) && "Sanity check");
	CORE_reg_write(SP_REG, end_address);

	DBG2("pop {%sregisters %02x (bitwise)}\n",
			(inst & 0x100)?"PC and ":"", inst & 0xff);

	return SUCCESS;
}

void register_opcodes_pop(void) {
	// pop: 1011 110<x's>
	register_opcode_mask(0xbc00, 0xffff4200, pop);
}
