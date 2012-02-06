#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"
#include "../misc.h"

static void mrs(uint8_t rd, uint8_t sysm) {
	uint32_t rd_val = 0;

	switch ((sysm >> 3) & 0x1f) {
		case 0x0:
			if (sysm & 0x1) {
				rd_val |= (CORE_ipsr_read() & 0x1ff);
			}
			if (sysm & 0x2) {
				// Clear rd [26:24], [15:10]
				rd_val &= ~0x0700fc00;
			}
			if (!(sysm & 0x4)) {
				// XXX: cpsr vs apsr naming convention
				rd_val |= (CORE_cpsr_read() & 0xf8000000);
				if (false /*HaveDSPext()*/) {
					rd_val |= (CORE_cpsr_read() & 0xf0000);
				}
			}
			break;

		case 0x1:
			CORE_ERR_not_implemented("priviledge separation\n");
			break;

		case 0x2:
			CORE_ERR_not_implemented("priviledge separation & FP\n");
			break;

		default:
			CORE_ERR_unpredictable("bad sysm val\n");
	}

	CORE_reg_write(rd, rd_val);
}

static void mrs_t1(uint32_t inst) {
	uint8_t sysm = inst & 0xff;
	uint8_t rd = (inst >> 8) & 0xf;

	// if d IN {13,15} || !(Uint(SYSm) IN {0..3,5.9.16..20})
	if (BadReg(rd) || false /* XXX lazy */)
		CORE_ERR_unpredictable("bad reg\n");

	return mrs(rd, sysm);
}

static void msr(uint8_t rn, uint8_t mask, uint8_t sysm) {
	uint32_t apsr_val = CORE_cpsr_read();
	uint32_t rn_val = CORE_reg_read(rn);

	switch ((sysm >> 3) & 0x1f) {
		case 0x0:
			if (!(sysm & 0x4)) {
				if (mask & 0x1) {
					if (!(false /*HaveDSPExt()*/)) {
						CORE_ERR_unpredictable("dsp?\n");
					} else {
						apsr_val &= ~0xf0000;
						apsr_val |= (rn_val & 0xf0000);
					}
				}
				if (mask & 0x2) {
					apsr_val &= ~0xf0000000;
					apsr_val |= (rn_val & 0xf0000000);
				}
			}

			break;

		case 0x1:
			CORE_ERR_not_implemented("priviledge\n");
			break;

		case 0x2:
			CORE_ERR_not_implemented("priviledge & others\n");
			break;

		default:
			CORE_ERR_unpredictable("bad sysm\n");
	}

	CORE_cpsr_write(apsr_val);
}

static void msr_t1(uint32_t inst) {
	uint8_t sysm = inst & 0xff;
	uint8_t mask = (inst >> 10) & 0x3;
	uint8_t rn = (inst >> 16) & 0xf;

	if ((mask == 0x0) || ((mask != 0x2) && !(sysm <= 3)))
		CORE_ERR_unpredictable("bad mask\n");

	// if n IN {13,15} || !(Uint(SYSm) IN {0..3,5.9.16..20})
	if (BadReg(rn) || false /* XXX lazy */)
		CORE_ERR_unpredictable("bad reg\n");

	return msr(rn, mask, sysm);
}

static void mov_imm(uint32_t cpsr, uint8_t setflags, uint32_t imm32, uint8_t rd, uint8_t carry){
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
			CORE_cpsr_write(cpsr);
		}
	}

	DBG2("mov_imm r%02d = 0x%08x\n", rd, result);
}

static void mov_imm_t1(uint16_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst >> 8) & 0x7;

	bool setflags = !in_ITblock();
	uint32_t imm32 = imm8;

	uint32_t cpsr = CORE_cpsr_read();
	bool carry = !!(cpsr & xPSR_C);

	return mov_imm(cpsr, setflags, imm32, rd, carry);
}

static void mov_imm_t2(uint32_t inst) {
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
	bool carry;
	ThumbExpandImm_C(arg, !!(cpsr & xPSR_C), &imm32, &carry);

	// if BadReg(d) then UNPREDICTABLE
	if ((Rd == 13) || (Rd == 15)) {
		CORE_ERR_unpredictable("mov to SP or PC\n");
	}

	return mov_imm(cpsr, setflags, imm32, Rd, carry);
}

static void mov_imm_t3(uint32_t inst) {
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

static void mov_reg(uint8_t rd, uint8_t rm, bool setflags) {
	uint32_t rm_val = CORE_reg_read(rm);

	if (rd == 15) {
		CORE_ERR_not_implemented("mov_reg --> alu\n");
		//ALUWritePC(rm_val);
	} else {
		CORE_reg_write(rd, rm_val);
		if (setflags) {
			uint32_t cpsr = CORE_cpsr_read();
			cpsr = GEN_NZCV(
					!!(rm_val & xPSR_N),
					rm_val == 0,
					!!(cpsr & xPSR_C),
					!!(cpsr & xPSR_V)
				       );
			CORE_cpsr_write(cpsr);
		}
	}

	DBG2("mov_reg r%02d = r%02d (val: %08x)\n", rd, rm, rm_val);
}

static void mov_reg_t1(uint16_t inst) {
	uint8_t rd =  (inst & 0x7);
	uint8_t rm =  (inst & 0x78) >> 3;
	uint8_t D = !!(inst & 0x80);

	// d = UInt(D:rd)
	rd |= (D << 3);

	// m = UInt(rm); unecessary

	uint8_t setflags = false;

	if ((rd == 15) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("mov_reg_t1 unpredictable\n");

	return mov_reg(rd, rm, setflags);
}

static void mov_reg_t2(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	bool setflags = true;

	if (in_ITblock())
		CORE_ERR_unpredictable("illegal in it block\n");

	return mov_reg(rd, rm, setflags);
}

static void movt(uint8_t rd, uint16_t imm16) {
	uint32_t rd_val = CORE_reg_read(rd);
	rd_val &= 0x0000ffff;	// clear top bits
	uint32_t wide_imm = imm16;
	rd_val |= (wide_imm << 16);
	CORE_reg_write(rd, rd_val);

	DBG2("movt r%02d = 0x%08x\n", rd, rd_val);
}

static void movt_t1(uint32_t inst) {
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
	// mrs_t1: 1111 0011 1110 1111 1000 xxxx xxxx xxxx
	register_opcode_mask_32(0xf3ef8000, 0x0c107000, mrs_t1);

	// msr_t1: 1111 0011 1000 xxxx 1000 xx00 xxxx xxxx
	register_opcode_mask_32(0xf3808000, 0x0c707300, msr_t1);

	// mov1: 0010 0xxx <x's>
	register_opcode_mask_16(0x2000, 0xd800, mov_imm_t1);

	// mov_imm_t2: 1111 0x00 010x 1111 0<x's>
	register_opcode_mask_32(0xf04f0000, 0x0ba08000, mov_imm_t2);

	// mov_imm_t3: 1111 0x10 0100 xxxx 0<x's>
	register_opcode_mask_32(0xf2400000, 0x09b08000, mov_imm_t3);

	// mov_reg_t1: 0100 0110 <x's>
	register_opcode_mask_16(0x4600, 0xb900, mov_reg_t1);

	// mov_reg_t2: 0000 0000 00xx xxxx
	register_opcode_mask_16(0x0, 0xffc0, mov_reg_t2);

	// movt_t1: 1111 0x10 1100 xxxx 0<x's>
	register_opcode_mask_32(0xf2c00000, 0x09308000, movt_t1);
}
