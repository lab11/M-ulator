#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"
#include "../misc.h"

int b1(uint32_t inst) {
	uint8_t cond = (inst & 0xf00) >> 8;
	int32_t signed_immed_8 = (inst & 0xff);

	if (eval_cond(CORE_cpsr_read(), cond)) {
		uint32_t pc = CORE_reg_read(PC_REG);
#ifdef DEBUG2
		uint32_t old_pc = pc;
#endif
		int32_t sign_extended;

		struct {signed int x:8;} s;
		sign_extended = s.x = signed_immed_8;

		pc = pc + (sign_extended << 1) + 2; // PC_HACK
		CORE_reg_write(PC_REG, pc);

		DBG2("b<cond> new_pc: %08x (taken)\told_pc: %08x\n", pc, old_pc);
	} else {
		DBG2("b<cond> not taken\n");
	}

	return SUCCESS;
}

int b2(uint32_t inst) {
	int32_t signed_immed_11 = inst & 0x7ff;
	int32_t sign_extended;

	struct {signed int x:11;} s;
	sign_extended = s.x = signed_immed_11;

	uint32_t pc = CORE_reg_read(PC_REG);

	DBG2("b cur_pc: %08x, immed: %08x, extended: %08x\n",
			pc, signed_immed_11, sign_extended);

/* THE DOCUMENTATION IS A LIE?

 Adding this to the contents of the PC (which contains the address of the branch
 instruction plus 4).

 No it's not, it's PC+4!!

 ^^^Nope, was working from an ARM ARM v6 not v7, this instruction
 no longer exists; screw that
 */

	pc = pc + (sign_extended << 1) + 2; // ^^ correction?
	CORE_reg_write(PC_REG, pc);

	DBG2("b new_pc: %08x\n", pc);

	return SUCCESS;
}

// BL is actually two instructions to allow for larger offset,
// need to preserve state across calls
int bl(uint32_t inst) {
	static int32_t offset32;

	int32_t signed_offset11 = inst & 0x7ff;

	if ((inst & 0x1000) && (~inst & 0x0800)) {
		DBG2("bl %x (H=10, first call)\n", signed_offset11);

		// H is 10, aka first call
		signed_offset11 <<= 12;

		// Need to sign-extend
		struct {signed int x:23;} s;
		offset32 = s.x = signed_offset11;

		return SUCCESS;
	}

	assert((inst & 0x1800) && "Expected H == 11");

	DBG2("bl %x (H=11, second call)\n", signed_offset11);

	uint32_t pc = CORE_reg_read(PC_REG);
	uint32_t lr = pc | 0x1;

	// PC_HACK is self-correct here b/c two instrs
	pc = pc + offset32;

	pc = pc + (2 * signed_offset11);

	CORE_reg_write(PC_REG, pc);
	CORE_reg_write(LR_REG, lr);

	DBG2("bl %08x (net total)\n", pc);

	return SUCCESS;
}

int blx(uint32_t inst __attribute__ ((unused))) {
	CORE_ERR_not_implemented("blx");
}

void SelectInstrSet(uint8_t iset) {
	switch (iset) {
		case INST_SET_ARM:
			DBG2("Set ISETSTATE to Arm\n");
			if (GET_ISETSTATE == INST_SET_THUMBEE) {
				CORE_ERR_unpredictable("ThumbEE -> Arm trans\n");
			} else {
				SET_ISETSTATE(INST_SET_ARM);
			}
			break;
		case INST_SET_THUMB:
			DBG2("Set ISETSTATE to Thumb\n");
			SET_ISETSTATE(INST_SET_THUMB);
			break;
		case INST_SET_JAZELLE:
			DBG2("Set ISETSTATE to Jazelle\n");
			SET_ISETSTATE(INST_SET_JAZELLE);
			break;
		case INST_SET_THUMBEE:
			DBG2("Set ISETSTATE to ThumbEE\n");
			SET_ISETSTATE(INST_SET_THUMBEE);
			break;
		default:
			CORE_ERR_unpredictable("Unknown iset\n");
			break;
	}
}

int b(uint8_t cond, uint32_t imm32) {
	if (eval_cond(CORE_cpsr_read(), cond)) {
		uint32_t pc = CORE_reg_read(PC_REG);
		BranchWritePC(pc + imm32);
		DBG2("b taken old pc %08x new pc %08x\n",
				pc, CORE_reg_read(PC_REG));
	} else {
		DBG2("b <not taken>\n");
	}

	return SUCCESS;
}

int b_t1(uint32_t inst) {
	uint8_t imm8 = (inst & 0xff);
	uint8_t cond = (inst & 0xf00) >> 8;

	if (cond == 0xe)
		CORE_ERR_unpredictable("b_t1 UNDEFINED\n");

	uint32_t imm32 = SignExtend(imm8 << 1, 9);

	DBG2("PC: %08x, imm32: %08x\n",
			CORE_reg_read(PC_REG), imm32);

	if (in_ITblock(ITSTATE))
		CORE_ERR_unpredictable("b_t1 UNPREDICTABLE\n");

	return b(cond, imm32);
}

int b_t2(uint32_t inst) {
	uint16_t imm11 = (inst & 0x7ff);

	uint32_t imm32 = SignExtend(imm11 << 1, 12);

	if (in_ITblock(ITSTATE) && !last_in_ITblock(ITSTATE))
		CORE_ERR_unpredictable("b_t2 in it block\n");

	return b(0xf, imm32);
}

int bl_blx(uint32_t pc, uint8_t targetInstrSet, uint32_t imm32) {
	uint32_t lr;
	DBG2("pc %08x targetInstrSet %x imm32 %d 0x%08x\n",
			pc, targetInstrSet, imm32, imm32);

	if (GET_ISETSTATE == INST_SET_ARM) {
		lr = pc - 4;
	} else {
		lr = ((pc & 0xfffffffe) | 0x1);
	}
	CORE_reg_write(LR_REG, lr);

	uint32_t targetAddress;
	if (targetInstrSet == INST_SET_ARM) {
		targetAddress = (pc & 0xfffffffc) + imm32;
	} else {
		targetAddress = pc + imm32;
	}

	SelectInstrSet(targetInstrSet);
	BranchWritePC(targetAddress);

	return SUCCESS;
}

int bl_t1(uint32_t inst) {
	// top 5 bits fixed
	uint8_t  S = !!(inst & 0x04000000);
	int imm10 =    (inst & 0x03ff0000) >> 16;
	assert(1 == (!!(inst & 0x00008000)));
	assert(1 == (!!(inst & 0x00004000)));
	uint8_t J1 = !!(inst & 0x00002000);
	assert(1 == (!!(inst & 0x00001000)));
	uint8_t J2 = !!(inst & 0x00000800);
	int imm11 =    (inst & 0x000007ff);

	uint8_t I1 = !(J1 ^ S);
	uint8_t I2 = !(J2 ^ S);

	struct {signed int x:25;} s;
	s.x = 0;		// 1
	s.x |= (imm11 << 1);	// 12
	s.x |= (imm10 << 12);	// 22
	s.x |= (I2 << 22);	// 23
	s.x |= (I1 << 23);	// 24
	s.x |= (S << 24);	// 25
	int32_t imm32 = s.x;

	DBG2("Sign bit: %d, imm32: %d 0x%08x\n", S, imm32, imm32);

	// targetInstrSet = CurrentInstrSet

	if (in_ITblock(ITSTATE) && !last_in_ITblock(ITSTATE))
		CORE_ERR_unpredictable("bl_t1 in itstate, not ending\n");

	return bl_blx(CORE_reg_read(PC_REG), GET_ISETSTATE, imm32);
}

int bl_t2(uint32_t inst) {
	// top 5 bits fixed
	uint8_t  S = !!(inst & 0x04000000);
	int imm10H =   (inst & 0x03ff0000) >> 16;
	assert(1 == (!!(inst & 0x00008000)));
	assert(1 == (!!(inst & 0x00004000)));
	uint8_t J1 = !!(inst & 0x00002000);
	assert(0 == (!!(inst & 0x00001000)));
	uint8_t J2 = !!(inst & 0x00000800);
	int imm10L =   (inst & 0x000007fe);
	uint8_t  H = !!(inst & 0x00000001);

	if (GET_ISETSTATE == (INST_SET_THUMBEE || H))
		CORE_ERR_unpredictable("bl_t2 thumbEE\n");

	uint8_t I1 = !(J1 ^ S);
	uint8_t I2 = !(J2 ^ S);

	struct {signed int x:25;} s;
	s.x = 0;		// 2
	s.x |= (imm10L << 2);	// 12
	s.x |= (imm10H << 12);	// 22
	s.x |= (I2 << 22);	// 23
	s.x |= (I1 << 23);	// 24
	s.x |= (S << 24);	// 25
	int32_t imm32 = s.x;

	DBG2("S %d I1 %d I2 %d imm10H %03x imm10L %03x\n",
			S, I1, I2, imm10H, imm10L);

	if (in_ITblock(ITSTATE) && !last_in_ITblock(ITSTATE))
		CORE_ERR_unpredictable("bl_t2 in itstate, not ending\n");

	return bl_blx(CORE_reg_read(PC_REG), INST_SET_ARM, imm32);
}

int bx(uint8_t rm) {
	BXWritePC(CORE_reg_read(rm));

	DBG2("bx happened\n");

	return SUCCESS;
}

int bx_t1(uint32_t inst) {
	uint8_t rm = (inst & 0x78) >> 3;

	if (in_ITblock(ITSTATE) && !last_in_ITblock(ITSTATE))
		CORE_ERR_unpredictable("bx_t1 in it block\n");

	return bx(rm);
}

/* ARMv6
int bx(uint32_t inst) {
	uint8_t rm = (inst & 0x78) >> 3;
	uint32_t target = CORE_reg_read(rm);
	uint32_t pc = target & 0xfffffffe; // target[31:1]<<1

	// Set "T flag"
	uint32_t cpsr = CORE_cpsr_read();
	if (target & 0x1) {
		DBG1("Entered Thumb mode\n");
		cpsr |= 0x10;
	} else {
		DBG1("Exited Thumb mode\n");
		cpsr &= ~0x10;
	}
	CORE_cpsr_write(cpsr);

	CORE_reg_write(PC_REG, pc);

	DBG2("bx %08x (net total)\n", pc);

	return SUCCESS;
}
*/

void register_opcodes_branch(void) {
	/*
	// b1: 1101 <x's>
	register_opcode_mask(0xd000, 0xffff2000, b1);
	*/

	/*
	// b2: 1110 0<x's>
	register_opcode_mask(0xe000, 0xffff1800, b2);

	// AARRGGGG!!! THIS WAS ARMv6!
	// bl,blx:
	// 1111 0<x's>
	// 1111 1<x's>
	// 1110 1<s's>
	// ----------------
	// 1111 x<x's>
	register_opcode_mask(0xf000, 0xffff0000, bl);
	// 1110 1<x's>
	register_opcode_mask(0xe800, 0xffff1000, blx);
	*/

	// b_t1: 1101 <x's>
	//   ex: xxxx 1111 <x's>
	register_opcode_mask_ex(0xd000, 0xffff2000, b_t1, 0x0f00, 0x0, 0, 0);

	// b_t2: 1110 0<x's>
	register_opcode_mask(0xe000, 0xffff1800, b_t2);

	// bl_t1: 1111 0xxx xxxx xxxx 11x1 <x's>
	register_opcode_mask(0xf000d000, 0x08000000, bl_t1);

	// bl_t2: 1111 0xxx xxxx xxxx 11x0 <x's>
	register_opcode_mask(0xf000c000, 0x08001000, bl_t2);

	// bx_t1: 0100 0111 0xxx x000
	register_opcode_mask(0x4700, 0xffffb887, bx_t1);

	/*
	// bx: 0100 0111 0<x's>
	register_opcode_mask(0x4700, 0xffffb800, bx);
	*/
}
