/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef STATIC_ROM_H
#define STATIC_ROM_H

// bintoarray.sh; "blink.bin"
#include <stdint.h>
#define STATIC_ROM_NUM_BYTES (97 * 4)
uint32_t static_rom[97];

#define STATIC_ROM_BLURB "\
\t\t\tblink.bin: Turns on and off GPIO's 0 and 1\n"

#endif // STATIC_ROM_H
