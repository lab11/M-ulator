#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"
#include "../core.h"
#include "../misc.h"

void pop_simple(uint32_t inst) {
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
}

void pop(uint16_t registers) {
	uint32_t address = CORE_reg_read(SP_REG);

	int i;
	for (i = 0; i <= 14; i++) {
		if (registers & (1 << i)) {
			CORE_reg_write(i, read_word(address));
			address += 4;
		}
	}

	if (registers & (1 << 15)) {
		LoadWritePC(read_word(address));
	}

	CORE_reg_write(SP_REG, CORE_reg_read(SP_REG) + 4 * hamming(registers));

	DBG2("pop did stuff\n");
}

void pop_t2(uint32_t inst) {
	uint16_t reg_list = inst & 0x1fff;
	uint8_t M = !!(inst & 0x4000);
	uint8_t P = !!(inst & 0x8000);

	uint16_t registers = (P << 15) | (M << 14) | reg_list;

	DBG2("P %d M %d reg_list %04x, registers %04x\n", P, M, reg_list, registers);

	if ((hamming(registers) < 2) || ((P == 1) && (M == 1)))
		CORE_ERR_unpredictable("pop_t2 not enough regs\n");

	if ((registers & 0x8000) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("pop_t2 itstate\n");

	return pop(registers);
}

void register_opcodes_pop(void) {
	// pop: 1011 110<x's>
	register_opcode_mask(0xbc00, 0xffff4200, pop_simple);

	// pop_t2: 1110 1000 1011 1101 xx0<x's>
	register_opcode_mask(0xe8bd0000, 0x17422000, pop_t2);
}
