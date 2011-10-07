#ifndef __HELPERS_H
#define __HELPERS_H

#include "../../simulator.h"
#include "cpsr.h"

int hamming(uint32_t val);

void AddWithCarry(uint32_t x, uint32_t y, bool carry_in,
		uint32_t *result, bool *carry_out, bool *overflow_out);

uint32_t SignExtend(uint32_t val, uint8_t bits);

void LoadWritePC(uint32_t addr);
void BXWritePC(uint32_t addr);
void BranchTo(uint32_t addr);
void BranchWritePC(uint32_t addr);

#endif // __HELPERS_H
