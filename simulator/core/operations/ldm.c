#include "opcodes.h"
#include "helpers.h"

#include "cpu/cpu.h"
#include "cpu/core.h"
#include "cpu/misc.h"

static void ldm(uint8_t rn, uint16_t registers, uint8_t wback) {
	uint32_t address = CORE_reg_read(rn);

	int i;
	for (i = 0; i <= 14; i++) {	// stupid arm inclusive for
		if (registers & (1 << i)) {
			CORE_reg_write(i, read_word(address));
			address += 4;
		}
	}
	if (registers & 0x8000) {
		//LoadWritePC(MemA[address, 4]);
		CORE_ERR_not_implemented("ldm PC");
	}

	if (wback && ((registers & (1 << rn)) == 0)) {
		CORE_reg_write(rn, CORE_reg_read(rn) + 4U*hamming(registers));
	}

	DBG2("ldm had loads of fun\n");
}

static void ldm_t2(uint32_t inst) {
	uint16_t reg_list = inst & 0x1fff;
	bool M = !!(inst & 0x4000);
	bool P = !!(inst & 0x8000);
	uint8_t rn = (inst & 0xf0000) >> 16;
	bool W = !!(inst & 0x200000);

	if ((W == 1) && (rn == 0xd))
		CORE_ERR_unpredictable("ldm_t2 --> pop\n");

	uint16_t registers = (P << 15) | (M << 14) | reg_list;
	bool wback = (W == 1);

	if ((rn == 15) || (hamming(registers) < 2) || ((P == 1) && (M == 1)))
		CORE_ERR_unpredictable("ldm_t2, long ||'s\n");

	if ((registers & 0x8000) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("ldm_t2, itstate stuff\n");

	if (wback && (registers & (1 << (rn))))
		CORE_ERR_unpredictable("ldm_t2, writeback mismatch?\n");

	return ldm(rn, registers, wback);
}

__attribute__ ((constructor))
void register_opcodes_ldm(void) {
	// LDM_t2: 1110 1000 10x1 xxxx xx0<x's>
	// Illegal:            1  1101
	//register_opcode_mask(0xe8900000, 0x17402000, ldm_t2);
	// enfore W == 0
	register_opcode_mask_32_ex(0xe8900000, 0x17402000, ldm_t2,
			0x002d0000, 0x00020000,
			0, 0);
}
