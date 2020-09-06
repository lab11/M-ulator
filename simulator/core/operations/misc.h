/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

void bfc(uint8_t rd, uint8_t msbit, uint8_t lsbit);
void bfi(uint8_t rd, uint8_t rn, uint8_t msbit, uint8_t lsbit);
void clz(uint8_t rd, uint8_t rm);
void movt(uint8_t rd, uint16_t imm16);
void rbit(uint8_t rm, uint8_t rd);
void rev(uint8_t rd, uint8_t rm);
void rev16(uint8_t rd, uint8_t rm);
void revsh(uint8_t rd, uint8_t rm);
void sbfx(uint8_t rd, uint8_t rn, uint8_t lsbit, uint8_t widthminus1);
void ubfx(uint8_t rd, uint8_t rn, uint8_t lsbit, uint8_t widthminus1);

