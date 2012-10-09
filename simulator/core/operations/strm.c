#include "opcodes.h"
#include "helpers.h"

#include "cpu/cpu.h"
#include "cpu/core.h"

static void stmdb(const uint8_t rn, const uint16_t registers, const bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t address = rn_val - 4 * hamming(registers);

	int i;
	for (i=0; i <= 14; i++) {
		if (registers & (1 << i)) {
			write_word(address, CORE_reg_read(i));
			address += 4;
		}
	}

	if (wback) {
		CORE_reg_write(rn, rn_val - 4 * hamming(registers));
	}
}

static void stmdb_t1(uint32_t inst) {
	uint16_t register_list = inst & 0xfff;
	bool M = (inst >> 14) & 0x1;
	uint8_t rn = (inst >> 16) & 0xf;
	bool W = (inst >> 21) & 0x1;

	uint16_t registers = (M << 13) | register_list;

	if ((rn == 1) || (hamming(registers) < 2))
		CORE_ERR_unpredictable("stmdb_t1 bad register set\n");

	if (W && (registers & (1 << rn)))
		CORE_ERR_unpredictable("stmdb_t1 wback overlaps regs\n");

	stmdb(rn, registers, W);
}

static void stm(uint8_t rn, uint16_t registers, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t address = rn_val;

	int i;
	for (i = 0; i <= 14; i++) {
		if (registers & (1 << i)) {
			if ((i == rn) && wback) {
				CORE_ERR_not_implemented("complicated case\n");
			} else {
				write_word(address, CORE_reg_read(i));
			}
			address += 4;
		}
	}

	if (wback)
		CORE_reg_write(rn, rn_val + 4*hamming(registers));
}

static void stm_t2(uint32_t inst) {
	uint16_t register_list = inst & 0xfff;
	bool M = !!(inst & 0x4000);
	uint8_t rn = (inst >> 16) & 0xf;
	bool W = !!(inst & 0x200000);

	uint16_t registers = (M << 13) | register_list;
	bool wback = W;

	if ((rn == 15) || (hamming(registers) < 2))
		CORE_ERR_unpredictable("bad regs\n");

	if (wback && (registers & (1 << rn)))
		CORE_ERR_unpredictable("wback conflict\n");

	return stm(rn, registers, wback);
}

__attribute__ ((constructor))
void register_opcodes_strm(void) {
	// 1110 1001 00x0 xxxx 0x0x xxxx xxxx xxxx
	//             1  1101
	register_opcode_mask_32_ex(0xe9000000, 0x16d0a000, stmdb_t1,
			0x002d0000, 0x00020000,
			0, 0);

	// stm{ia,ea}_t2: 1110 1000 10x0 xxxx 0x0x xxxx xxxx xxxx
	register_opcode_mask_32(0xe8800000, 0x1750a000, stm_t2);
}
