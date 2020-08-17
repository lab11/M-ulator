/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

void cps(bool enable, bool disable, bool affectPri, bool affectFault);
void mrs(uint8_t SYSm, uint8_t rd);
void msr(uint8_t SYSm, uint8_t mask, uint8_t rn);
