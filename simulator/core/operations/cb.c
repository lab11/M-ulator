#include "opcodes.h"
#include "helpers.h"

#include "cpu/cpu.h"
#include "cpu/misc.h"

static void cbz(bool nonzero, uint8_t rn, uint32_t imm32) {
	uint32_t rn_val = CORE_reg_read(rn);

	if (nonzero ^ (rn_val == 0))
		BranchWritePC(CORE_reg_read(PC_REG) + imm32);
}

static void cbz_t1(uint16_t inst) {
	uint8_t rn = (inst & 0x7);
	uint8_t imm5 = (inst >> 3) & 0x1f;
	bool i = (inst >> 9) & 0x1;
	bool op = (inst >> 11) & 0x1;

	uint32_t imm32 = (i << 6) | (imm5 << 1);

	if (in_ITblock())
		CORE_ERR_unpredictable("cb{n}z in IT block\n");

	cbz(op == 1, rn, imm32);
}

__attribute__ ((constructor))
void register_opcodes_cb(void) {
	// cb{N}Z: 1011 x0x1 <x's>
	register_opcode_mask_16(0xb100, 0x4400, cbz_t1);
}
