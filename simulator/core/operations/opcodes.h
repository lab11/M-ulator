#ifndef OPCODES_H
#define OPCODES_H

#include "core/common.h"

// Use this function to register your opcode handler
// if (
//       (ones_mask  == ones_mask  &  inst) &&
//       (zeros_mask == zeros_mask & ~inst)
//    )
// the instruction is considered a match
int		register_opcode_mask_16(uint16_t ones_mask,
		uint16_t zeros_mask, void (*fn) (uint16_t));
int		register_opcode_mask_16_ex(uint16_t ones_mask,
		uint16_t zeros_mask, void (*fn) (uint16_t), ...);
int		register_opcode_mask_32(uint32_t ones_mask,
		uint32_t zeros_mask, void (*fn) (uint32_t));
int		register_opcode_mask_32_ex(uint32_t ones_mask,
		uint32_t zeros_mask, void (*fn) (uint32_t), ...);
/* TTTA: Why do you need to specify two masks?
 *
 *  _Hint:_ When could we consider hardware to be trinary instead of binary?
 *
 */

//////////////////
// MACRO TRICKS //
//////////////////

#define register_opcode_mask_16(_o, _z, _f)\
	register_opcode_mask_16_real((_o), (_z), (_f), __FILE__":"VAL2STR(_f))
int		register_opcode_mask_16_real(uint16_t, uint16_t,
		void (*fn) (uint16_t), const char *);
#define register_opcode_mask_16_ex(_o, _z, _f, ...)\
	register_opcode_mask_16_ex_real((_o), (_z),(_f),\
			__FILE__":"VAL2STR(_f), __VA_ARGS__)
int		register_opcode_mask_16_ex_real(uint16_t, uint16_t,
		void (*fn) (uint16_t), const char*, ...);

#define register_opcode_mask_32(_o, _z, _f)\
	register_opcode_mask_32_real((_o), (_z), (_f), __FILE__":"VAL2STR(_f))
int		register_opcode_mask_32_real(uint32_t, uint32_t,
		void (*fn) (uint32_t), const char *);
#define register_opcode_mask_32_ex(_o, _z, _f, ...)\
	register_opcode_mask_32_ex_real((_o), (_z),(_f),\
			__FILE__":"VAL2STR(_f), __VA_ARGS__)
int		register_opcode_mask_32_ex_real(uint32_t, uint32_t,
		void (*fn) (uint32_t), const char*, ...);

#endif // OPCODES_H
