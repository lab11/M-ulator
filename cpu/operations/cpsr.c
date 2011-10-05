#include "../../common.h"
#include "cpsr.h"

#include "../cpu.h"

void DecodeImmShift(uint8_t type, uint8_t imm5, enum SRType *shift_t, uint8_t *shift_n) {
	assert(SRType_RRX == SRType_ROR);

	switch (type) {
		case SRType_LSL:
			*shift_t = LSL;
			*shift_n = imm5;
			break;
		case SRType_LSR:
			*shift_t = LSR;
			*shift_n = (imm5 == 0) ? 32 : imm5;
			break;
		case SRType_ASR:
			*shift_t = ASR;
			*shift_n = (imm5 == 0) ? 32 : imm5;
			break;
		case SRType_RRX:
		//case SRType_ROR: // same
			*shift_t = (imm5 == 0) ? RRX : ROR;
			*shift_n = (imm5 == 0) ? 1 : imm5;
			break;
		default:
			CORE_ERR_unpredictable("Bad type DecodeImmShift\n");
	}
}

void Shift_C(uint32_t value, int Nbits, enum SRType type, int amount, uint8_t carry_in, uint32_t *result, uint8_t *carry_out) {
	assert (!((type == SRType_RRX) && (amount != 1)));

	if (amount == 0) {
		*result = value;
		*carry_out = carry_in;
	} else {
		switch (type) {
			case LSL:
				LSL_C(value, Nbits, amount, result, carry_out);
				break;
			case LSR:
				LSR_C(value, Nbits, amount, result, carry_out);
				break;
			case ASR:
				ASR_C(value, Nbits, amount, result, carry_out);
				break;
			case ROR:
				ROR_C(value, Nbits, amount, result, carry_out);
				break;
			case RRX:
				//RRX_C(value, Nbits, amount, result, carry_out);
				CORE_ERR_not_implemented("Shitf_C RRX\n");
				break;
			default:
				CORE_ERR_unpredictable("Shift_C bad type\n");
		}
	}
}

uint32_t Shift(uint32_t value, int Nbits, enum SRType type, int amount, uint8_t carry_in) {
	uint32_t result;
	uint8_t carry_out;
	Shift_C(value, Nbits, type, amount, carry_in, &result, &carry_out);
	return result;
}

void LSL_C(uint32_t x, int Nbits, int shift, uint32_t *result, uint8_t *carry_out) {
	assert(shift > 0);
	// use 64 bit type to catch overflow of shift to the 33rd bit
	uint64_t extended_x = x << shift;
	*result = extended_x & ((1 << Nbits) - 1);
	*carry_out = extended_x & (1 << Nbits);
}

void LSR_C(uint32_t x, int Nbits __attribute__ ((unused)), int shift, uint32_t *result, uint8_t *carry_out) {
	assert(shift > 0);
	// extended_x = ZeroExtend(x, shift+Nbits)
	// ^^unneeded, C-language zero-fills left bits
	*result = x >> shift;
	*carry_out = (x >> (shift - 1)) & 0x1;
}

void ASR_C(uint32_t x, int Nbits, int shift, uint32_t *result, uint8_t *carry_out) {
	assert(shift > 0);

	uint32_t extended_x;
	if (x & (1 << (shift - 1))) {
		// negative, need to extend
		uint32_t mask = 0xffffffff - ((1 << (shift - 1)) - 1);
		extended_x = x | mask;
	} else {
		extended_x = x;
	}

	*result = (extended_x >> shift) & ((1 << Nbits) - 1);
	*carry_out = extended_x & (1 << (shift - 1));
}

void ROR_C(uint32_t x, int Nbits, int shift, uint32_t *result, uint8_t *carry_out) {
	assert(shift != 0);
	int m = shift % Nbits;
	*result = (x >> m) | (x << (Nbits - m));
	*carry_out = (*result) & (1 << (Nbits - 1));
}

uint32_t ThumbExpandImm(uint32_t imm12) {
	uint32_t cpsr = CORE_cpsr_read();

	uint32_t result;
	uint8_t carry;

	ThumbExpandImm_C(imm12, !!(cpsr & xPSR_C), &result, &carry);

	return result;
}

void ThumbExpandImm_C(
		uint16_t imm12, uint8_t carry_in,
		uint32_t *imm32, uint8_t *carry_out
		) {

	if ((imm12 & 0xc00) == 0) {
		switch (imm12 & 0x300) {
			case (0x000):
				*imm32 = imm12 & 0xff;
				break;
			case (0x100):
				if ((imm12 & 0xff) == 0)
					CORE_ERR_unpredictable("ThumbExpandImm_C\n");
				*imm32 = (imm12 & 0xff);
				*imm32 |= ((imm12 & 0xff) << 16);
				break;
			case (0x200):
				if ((imm12 & 0xff) == 0)
					CORE_ERR_unpredictable("ThumbExpandImm_C\n");
				*imm32 = ((imm12 & 0xff) << 8);
				*imm32 |= ((imm12 & 0xff) << 24);
				break;
			case (0x300):
				if ((imm12 & 0xff) == 0)
					CORE_ERR_unpredictable("ThumbExpandImm_C\n");
				*imm32 = (imm12 & 0xff);
				*imm32 |= ((imm12 & 0xff) << 8);
				*imm32 |= ((imm12 & 0xff) << 16);
				*imm32 |= ((imm12 & 0xff) << 24);
				break;
			default:
				assert(false);
		}

		*carry_out = carry_in;
	} else {
		DBG2("ThumbExpandImm_C --> ROR_C\n");
		// unrotated_value is 32 bits
		uint32_t unrotated_value = (1 << 7) | (imm12 & 0x7f);
		uint32_t arg = (imm12 & 0xf80) >> 7;
		ROR_C(unrotated_value, 32, arg, imm32, carry_out);
	}

	DBG2("expanded imm12 %08x carry_in  %d\n", imm12, carry_in);
	DBG2("into     imm32 %08x carry_out %d\n", *imm32, *carry_out);
}
