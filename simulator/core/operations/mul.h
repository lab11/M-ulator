/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

void mla(uint8_t rd, uint8_t rn, uint8_t rm, uint8_t ra);
void mls(uint8_t rd, uint8_t rn, uint8_t rm, uint8_t ra);
void mul(uint8_t setflags, uint8_t rd, uint8_t rn, uint8_t rm);
void smull(uint8_t rdlo, uint8_t rdhi, uint8_t rn, uint8_t rm, bool setflags);
void umull(uint8_t rdlo, uint8_t rdhi, uint8_t rn, uint8_t rm, bool setflags);

