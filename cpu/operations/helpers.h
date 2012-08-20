#ifndef __HELPERS_H
#define __HELPERS_H

#define INST_SET_ARM		0x0
#define INST_SET_THUMB		0x1
#define INST_SET_JAZELLE	0x2
#define INST_SET_THUMBEE	0x3

#define GEN_NZCV(_n, _z, _c, _v) \
	(\
	 (!!(_n) << N_IDX) |\
	 (!!(_z) << Z_IDX) |\
	 (!!(_c) << C_IDX) |\
	 (!!(_v) << V_IDX) |\
	 (cpsr & 0x0fffffff)\
	)

#define GEN_NZC(_n, _z, _c) \
	(\
	 (!!(_n) << N_IDX) |\
	 (!!(_z) << Z_IDX) |\
	 (!!(_c) << C_IDX) |\
	 (cpsr & 0x1fffffff)\
	)

#define GEN_NZ(_n, _z) \
	(\
	 (!!(_n) << N_IDX) |\
	 (!!(_z) << Z_IDX) |\
	 (cpsr & 0x3fffffff)\
	)

uint32_t hamming(uint32_t val);

void AddWithCarry(uint32_t x, uint32_t y, bool carry_in,
		uint32_t *result, bool *carry_out, bool *overflow_out)
		__attribute__ ((nonnull));

uint32_t SignExtend(uint32_t val, uint8_t bits);

void LoadWritePC(uint32_t addr);
void BXWritePC(uint32_t addr);
void BranchTo(uint32_t addr);
void BranchWritePC(uint32_t addr);

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

// Legal range of shift is 0..31, uint8_t forces callers to be at least somewhat concious of this
void DecodeImmShift(uint8_t type, uint8_t imm5,
		enum SRType *shift_t, uint8_t *shift_n)
		__attribute__ ((nonnull));
void Shift_C(uint32_t value, int Nbits,
		enum SRType type, uint8_t amount, bool carry_in,
		uint32_t *result, bool *carry_out)
		__attribute__((nonnull));
uint32_t Shift(uint32_t value, int Nbits,
		enum SRType type, uint8_t amount, bool carry_in);

// Legal range of shift is 0..31, uint8_t forces callers to be at least somewhat concious of this
void LSL_C(uint32_t x, int Nbits, uint8_t shift, uint32_t *result, bool *carry_out);
void LSR_C(uint32_t x, int Nbits, uint8_t shift, uint32_t *result, bool *carry_out);
void ASR_C(uint32_t x, int Nbits, uint8_t shift, uint32_t *result, bool *carry_out);
void ROR_C(uint32_t x, int Nbits, uint8_t shift, uint32_t *result, bool *carry_out);
uint32_t ThumbExpandImm(uint32_t imm12);
void ThumbExpandImm_C(uint32_t imm12, bool carry_in, uint32_t *imm32, bool *carry_out);

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

#elif defined M_PROFILE

#define ITSTATE			( (((CORE_epsr_read()) & 0xfc00) >> 8) | (((CORE_epsr_read()) & 0x06000000) >> 25) )

#define THUMB_BIT		(0x01000000)
#define GET_THUMB_BIT		(!!(epsr & THUMB_BIT))
#define SET_THUMB_BIT(_t) \
	do {\
		uint32_t epsr = CORE_epsr_read();\
		if (_t)\
			epsr |= THUMB_BIT;\
		else\
			epsr &= ~THUMB_BIT;\
		CORE_epsr_write(epsr);\
	} while (0)
#define SET_ISETSTATE(_i) // NOP for M profile
#define GET_ISETSTATE	INST_SET_THUMB

#endif // M

#endif // __HELPERS_H
