/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

void mov_imm(union apsr_t apsr, uint8_t setflags, uint32_t imm32, uint8_t rd, uint8_t carry);
void mov_reg(uint8_t rd, uint8_t rm, bool setflags);

