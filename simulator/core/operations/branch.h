/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

void b(uint8_t cond, uint32_t imm32);
void bl_blx(uint32_t pc, uint8_t targetInstrSet, uint32_t imm32);
void blx_reg(uint8_t rm);
void bx(uint8_t rm);
void tbb(uint8_t rn, uint8_t rm, bool is_tbh);

