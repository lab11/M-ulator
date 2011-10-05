#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"
#include "../misc.h"

int mov1(uint32_t inst) {
	int8_t rd = (inst & 0x700) >> 8;
	uint8_t uimmed8 = (inst & 0xff);

	CORE_reg_write(rd, uimmed8);

	uint32_t cpsr = CORE_cpsr_read();

	if (!in_ITblock(ITSTATE)) {
		// XXX: How could N ever be set by this instr
		// if uimmed8 is 0-255?
		cpsr = GEN_NZCV(0, uimmed8 == 0, cpsr & xPSR_C, cpsr & xPSR_V);
		CORE_cpsr_write(cpsr);
	}

	DBG2("mov r%02d, #%u\t; 0x%x\n", rd, uimmed8, uimmed8);

	return SUCCESS;
}
/*
int mov3(uint32_t inst) {
	assert((inst & 0x80) || (inst & 0x40)); // ARM ref

	int8_t rd = ((inst & 0x80) >> 4) | (inst & 0x7);
	int8_t rm = (inst & 0x78) >> 3;

	CORE_reg_write(rd, CORE_reg_read(rm));

	DBG2("mov r%02d, r%02d\n", rd, rm);

	return SUCCESS;
}
*/

int mov_imm(uint32_t cpsr, uint8_t setflags, uint32_t imm32, uint8_t rd, uint8_t carry){
	uint32_t result = imm32;

	if (rd == 15) {
		CORE_ERR_not_implemented("A1 case of mov_imm\n");
		//ALUWritePC(result);
	} else {
		CORE_reg_write(rd, result);
		if (setflags) {
			cpsr = GEN_NZCV(
					!!(result & xPSR_N),
					result == 0,
					carry,
					!!(cpsr & xPSR_V)
				       );
		}
	}

	DBG2("mov_imm r%02d = 0x%08x\n", rd, result);

	return SUCCESS;
}

int mov_imm_t2(uint32_t inst) {
	uint32_t cpsr = CORE_cpsr_read();

	int   imm8 =  (inst & 0x000000ff);
	uint8_t Rd =  (inst & 0x00000f00) >> 8;
	int   imm3 =  (inst & 0x00007000) >> 12;
	assert(0 ==   (inst & 0x00008000));
	assert(0 ==  !(inst & 0x00010000));
	assert(0 ==  !(inst & 0x00020000));
	assert(0 ==  !(inst & 0x00040000));
	assert(0 ==  !(inst & 0x00080000));
	uint8_t S = !!(inst & 0x00100000);
	assert(0 ==   (inst & 0x00200000));
	assert(0 ==  !(inst & 0x00400000));
	assert(0 ==   (inst & 0x00800000));
	assert(0 ==   (inst & 0x01000000));
	assert(0 ==   (inst & 0x02000000));
	uint8_t i = !!(inst & 0x04000000);

	// d = UInt(Rd);
	// Unecessary

	uint8_t setflags = (S == 0x1);

	// (imm32, carry) = ThumbExpandImm_C(i:imm3:imm8, APSR.C);
	uint32_t arg;
	arg = imm8;
	arg |= (imm3 << 8);
	arg |= (i << 11);
	uint32_t imm32;
	uint8_t carry;
	ThumbExpandImm_C(arg, !!(cpsr & xPSR_C), &imm32, &carry);

	// if BadReg(d) then UNPREDICTABLE
	if ((Rd == 13) || (Rd == 15)) {
		CORE_ERR_unpredictable("mov to SP or PC\n");
	}

	return mov_imm(cpsr, setflags, imm32, Rd, carry);
}

int mov_imm_t3(uint32_t inst) {
	uint32_t cpsr = CORE_cpsr_read();

	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t imm4 = (inst & 0xf0000) >> 16;
	uint8_t i = !!(inst & 0x04000000);

	// d = uint(rd);
	uint8_t setflags = false;

	uint32_t imm32 = (imm4 << 12) | (i << 11) | (imm3 << 8) | imm8;

	if (BadReg(rd))
		CORE_ERR_unpredictable("BadReg in mov_imm_t3\n");

	// carry set to 0 irrelevant since setflags is false
	return mov_imm(cpsr, setflags, imm32, rd, 0);
}

int mov_reg(uint32_t cpsr, uint8_t setflags,  uint8_t rd, uint8_t rm) {
	uint32_t rm_val = CORE_reg_read(rm);

	if (rd == 15) {
		CORE_ERR_not_implemented("mov_reg --> alu\n");
		//ALUWritePC(rm_val);
	} else {
		CORE_reg_write(rd, rm_val);
		if (setflags) {
			cpsr = GEN_NZCV(
					!!(rm_val & xPSR_N),
					rm_val == 0,
					!!(cpsr & xPSR_C),
					!!(cpsr & xPSR_V)
				       );
		}
	}

	DBG2("mov_reg r%02d = r%02d (val: %08x)\n", rd, rm, rm_val);

	return SUCCESS;
}

int mov_reg_t1(uint32_t inst) {
	uint32_t cpsr = CORE_cpsr_read();

	uint8_t rd =  (inst & 0x7);
	uint8_t rm =  (inst & 0x78) >> 3;
	uint8_t D = !!(inst & 0x80);

	// d = UInt(D:rd)
	rd |= (D << 3);

	// m = UInt(rm); unecessary

	uint8_t setflags = false;

	if ((rd == 15) && in_ITblock(ITSTATE) && !last_in_ITblock(ITSTATE))
		CORE_ERR_unpredictable("mov_reg_t1 unpredictable\n");

	return mov_reg(cpsr, setflags, rd, rm);
}

int movt(uint8_t rd, uint16_t imm16) {
	uint32_t rd_val = CORE_reg_read(rd);
	rd_val &= 0x0000ffff;	// clear top bits
	uint32_t wide_imm = imm16;
	rd_val |= (wide_imm << 16);
	CORE_reg_write(rd, rd_val);

	DBG2("movt r%02d = 0x%08x\n", rd, rd_val);

	return SUCCESS;
}

int movt_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t imm4 = (inst & 0xf0000) >> 16;
	uint8_t i = !!(inst & 0x04000000);

	// d = uint(rd)
	uint16_t imm16 = (imm4 << 12) | (i << 11) | (imm3 << 8) | imm8;

	if (BadReg(rd))
		CORE_ERR_unpredictable("BadReg movt_t1\n");

	return movt(rd, imm16);
}

void register_opcodes_mov(void) {
	// mov1: 0010 0xxx <x's>
	register_opcode_mask(0x2000, 0xffffd800, mov1);

	// NOTE: Same as ADD Rd, Rn, #0
	// mov2: 0001 1100 00xx xxxx
	// register_opcode_mask(0x1c00, 0xe3c0, mov2);

	// mov3: 0100 0110 xxxx xxxx
//	register_opcode_mask(0x4600, 0xffffb900, mov3);

	// mov_imm_t2: 1111 0x00 010x 1111 0<x's>
	register_opcode_mask(0xf04f0000, 0x0ba08000, mov_imm_t2);

	// mov_imm_t3: 1111 0x10 0100 xxxx 0<x's>
	register_opcode_mask(0xf2400000, 0x09b08000, mov_imm_t3);

	// mov_reg_t1: 0100 0110 <x's>
	register_opcode_mask(0x4600, 0xffffb900, mov_reg_t1);

	// movt_t1: 1111 0x10 1100 xxxx 0<x's>
	register_opcode_mask(0xf2c00000, 0x09308000, movt_t1);
}
