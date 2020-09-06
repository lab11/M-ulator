/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

void ssat(uint8_t rd, uint8_t rn, uint8_t saturate_to,
		enum SRType shift_t, uint8_t shift_n);
void usat(uint8_t rd, uint8_t rn, uint8_t saturate_to,
		enum SRType shift_t, uint8_t shift_n);

