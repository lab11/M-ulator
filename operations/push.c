#include "../cortex_m3.h"

int push(uint32_t inst) {
	uint32_t sp = CORE_reg_read(SP_REG);

	int hamming = 0;
	{
		int reg_list = inst & 0xff;
		for (hamming = 0; reg_list; hamming++) {
			reg_list &= (reg_list - 1); // clears LSB
		}
		hamming += !!(inst & 0x100); // LR?

		assert((hamming > 0) && (hamming <= 9));
	}


	uint32_t start_address = sp - 4*hamming;
	uint32_t end_address = sp - 4;
	uint32_t address = start_address;

	int i;
	for (i = 0; i < 8; i++) {
		if (inst & (1 << i)) {
			write_word(address, CORE_reg_read(i));
			address += 4;
		}
	}
	if (inst & 0x100) {
		write_word(address, CORE_reg_read(LR_REG));
		address += 4;
	}

	assert ((end_address == address - 4) && "Sanity check");

	sp = sp - 4*hamming;
	CORE_reg_write(SP_REG, sp);

	DBG2("push {%sregisters %x (bitwise)}\n",
			(inst & 0x100)?"LR and ":"", inst & 0xff);

	return SUCCESS;
}

void register_opcodes_push(void) {
	// 1011 010x <x's>
	register_opcode_mask(0xb400, 0xffff4a00, push);
}
