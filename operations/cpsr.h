#ifndef __CPSR_H
#define __CPSR_H

#include "../cortex_m3.h"

/* Not strictly cspr stuff anymore */

#define BadReg(_r)	((_r == 13) || (_r == 15))

#define SRType_LSL	0x0
#define SRType_LSR	0x1
#define SRType_ASR	0x2
#define SRType_RRX	0x3
#define SRType_ROR	0x3

#define SRType_ENUM_TO_MASK(_e) (((_e) == 4) ? 3 : _e)

enum SRType {
	LSL,	// match
	LSR,	// match
	ASR,	// match
	RRX,	// match
	ROR,	// +1
};

void DecodeImmShift(uint8_t type, uint8_t imm5, enum SRType *shift_t, uint8_t *shift_n);
void Shift_C(uint32_t value, int Nbits, enum SRType type, int amount, uint8_t carry_in, uint32_t *result, uint8_t *carry_out);
uint32_t Shift(uint32_t value, int Nbits, enum SRType type, int amount, uint8_t carry_in);

void LSL_C(uint32_t x, int Nbits, int shift, uint32_t *result, uint8_t *carry_out);
void LSR_C(uint32_t x, int Nbits, int shift, uint32_t *result, uint8_t *carry_out);
void ASR_C(uint32_t x, int Nbits, int shift, uint32_t *result, uint8_t *carry_out);
void ROR_C(uint32_t x, int Nbits, int shift, uint32_t *result, uint8_t *carry_out);
uint32_t ThumbExpandImm(uint32_t imm12);
void ThumbExpandImm_C(uint16_t imm12, uint8_t carry_in, uint32_t *imm32, uint8_t *carry_out);

#define OVER_ADD(_res, _a, _b) \
	(\
	 (((_a) & (1<<31)) == ((_b)   & (1<<31))) &&\
	 (((_a) & (1<<31)) != ((_res) & (1<<31)))\
	)

#define OVER_SUB(_res, _a, _b) \
	(\
	 (((_a) & (1<<31)) != ((_b)   & (1<<31))) &&\
	 (((_a) & (1<<31)) != ((_res) & (1<<31)))\
	)

#define INST_SET_ARM		0x0
#define INST_SET_THUMB		0x1
#define INST_SET_JAZELLE	0x2
#define INST_SET_THUMBEE	0x3

#ifdef A_PROFILE

#define ITSTATE			( (((cpsr) & 0xfc00) >> 8) | (((cpsr) & 0x06000000) >> 25) )
#define	IN_IT_BLOCK		((ITSTATE & 0xf) != 0)
#define LAST_IN_IT_BLOCK	((ITSTATE & 0xf) == 0x8)

#define THUMB_BIT		(0x00000020)
#define GET_THUMB_BIT		(!!(cpsr & THUMB_BIT))
#define SET_THUMB_BIT(_t) \
	do {\
		if (_t)\
			cpsr |= THUMB_BIT;\
		else\
			cpsr &= ~THUMB_BIT;\
	} while (0)
#define JAZELLE_BIT		(0x01000000)
#define GET_JAZELLE_BIT		(!!(cpsr & JAZELLE_BIT))
#define SET_JAZELLE_BIT(_j)\
	do {\
		if (_j)\
			cpsr |= JAZELLE_BIT;\
		else\
			cpsr &= ~JAZELLE_BIT;\
	} while (0)
#define SET_ISETSTATE(_i)\
	do {\
		SET_JAZELLE_BIT((_i) >> 1);\
		SET_THUMB_BIT((_i) & 0x1);\
		CORE_cpsr_write(cpsr);\
	} while (0)
#define GET_ISETSTATE		(((GET_JAZELLE_BIT) << 1) | GET_THUMB_BIT)

#define GEN_NZCV(_n, _z, _c, _v) \
	(\
	 ((_n) << N_IDX) |\
	 ((_z) << Z_IDX) |\
	 ((_c) << C_IDX) |\
	 ((_v) << V_IDX) |\
	 (cpsr & 0x0fffffff)\
	)

#elif defined M_PROFILE

#define ITSTATE			( (((CORE_epsr_read()) & 0xfc00) >> 8) | (((CORE_epsr_read()) & 0x06000000) >> 25) )

#define THUMB_BIT		(0x01000000)
#define GET_THUMB_BIT		(!!(cpsr & THUMB_BIT))
#define SET_THUMB_BIT(_t) \
	do {\
		if (_t)\
			cpsr |= THUMB_BIT;\
		else\
			cpsr &= ~THUMB_BIT;\
	} while (0)
#define SET_ISETSTATE(_i) // NOP for M profile
#define GET_ISETSTATE	INST_SET_THUMB

#define GEN_NZCV(_n, _z, _c, _v) \
	(\
	 ((_n) << N_IDX) |\
	 ((_z) << Z_IDX) |\
	 ((_c) << C_IDX) |\
	 ((_v) << V_IDX) |\
	 (cpsr & 0x0fffffff)\
	)

#endif // M

#endif //__CPSR_H
