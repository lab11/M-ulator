#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"
#include "../misc.h"

// BL is actually two instructions to allow for larger offset,
// need to preserve state across calls
void bl(uint32_t inst) {
	static int32_t offset32;

	int32_t signed_offset11 = inst & 0x7ff;

	if ((inst & 0x1000) && (~inst & 0x0800)) {
		DBG2("bl %x (H=10, first call)\n", signed_offset11);

		// H is 10, aka first call
		signed_offset11 <<= 12;

		// Need to sign-extend
		struct {signed int x:23;} s;
		offset32 = s.x = signed_offset11;
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
}

void blx(uint32_t inst __attribute__ ((unused))) {
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

void b(uint8_t cond, uint32_t imm32) {
	if (eval_cond(CORE_cpsr_read(), cond)) {
		uint32_t pc = CORE_reg_read(PC_REG);
		BranchWritePC(pc + imm32);
		DBG2("b taken old pc %08x new pc %08x (imm32: %08x)\n",
				pc, CORE_reg_read(PC_REG), imm32);
	} else {
		DBG2("b <not taken>\n");
	}
}

void b_t1(uint32_t inst) {
	uint8_t imm8 = (inst & 0xff);
	uint8_t cond = (inst & 0xf00) >> 8;

	if (cond == 0xe)
		CORE_ERR_unpredictable("b_t1 UNDEFINED (cond 0xe)\n");

	uint32_t imm32 = SignExtend(imm8 << 1, 9);

	DBG2("PC: %08x, imm32: %08x\n",
			CORE_reg_read(PC_REG), imm32);

	if (in_ITblock())
		CORE_ERR_unpredictable("b_t1 UNPREDICTABLE (b in IT block)\n");

	return b(cond, imm32);
}

void b_t2(uint32_t inst) {
	uint16_t imm11 = (inst & 0x7ff);

	uint32_t imm32 = SignExtend(imm11 << 1, 12);

	if (in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("b_t2 in it block\n");

	return b(0xf, imm32);
}

void b_t3(uint32_t inst) {
	uint16_t imm11 = (inst & 0x7ff);
	bool J2 = !!(inst & 0x800);
	bool J1 = !!(inst & 0x2000);
	uint8_t imm6 = (inst >> 16) & 0x3f;
	uint8_t cond = (inst >> 22) & 0xf;
	bool S = !!(inst & 0x04000000);

	/* THIS FUCKING INSTRUCTION DECODE IS MISSING FROM THE M SERIES ARM ARM,
	   Turns out it is __NOT__ the same as the t4 encoding, as the absence
	   of documentation may lead a naiive reader to believe. From the A
	   seris ARM ARM we learn:

	   if cond<3:1> == ‘111’ then SEE “Related encodings”;
	   imm32 = SignExtend(S:J2:J1:imm6:imm11:’0’, 32);
	   if InITBlock() then UNPREDICTABLE;
	*/

	uint32_t imm32 = (J2 << 19) | (J1 << 18) | (imm6 << 12) | (imm11 << 1);
	if (S) {
		imm32 |= 0xfff00000;
	}

	if (in_ITblock())
		CORE_ERR_unpredictable("b_t3 not allowed in IT block\n");

	return b(cond, imm32);
}

void bl_blx(uint32_t pc, uint8_t targetInstrSet, uint32_t imm32) {
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
}

void bl_t1(uint32_t inst) {
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

	if (in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("bl_t1 in itstate, not ending\n");

	return bl_blx(CORE_reg_read(PC_REG), GET_ISETSTATE, imm32);
}

void bl_t2(uint32_t inst) {
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

	if (in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("bl_t2 in itstate, not ending\n");

	return bl_blx(CORE_reg_read(PC_REG), INST_SET_ARM, imm32);
}

void bx(uint8_t rm) {
	BXWritePC(CORE_reg_read(rm));

	DBG2("bx happened\n");
}

void bx_t1(uint32_t inst) {
	uint8_t rm = (inst & 0x78) >> 3;

	if (in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("bx_t1 in it block\n");

	return bx(rm);
}

/* ARMv6
void bx(uint32_t inst) {
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
}
*/

void register_opcodes_branch(void) {
	// b_t1: 1101 <x's>
	//   ex: xxxx 1111 <x's>
	register_opcode_mask_ex(0xd000, 0xffff2000, b_t1, 0x0f00, 0x0, 0, 0);

	// b_t2: 1110 0<x's>
	register_opcode_mask(0xe000, 0xffff1800, b_t2);

	// b_t3: 1111 0xxx xxxx xxxx 10x0 <x's>
	register_opcode_mask(0xf0008000, 0x08005000, b_t3);

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
