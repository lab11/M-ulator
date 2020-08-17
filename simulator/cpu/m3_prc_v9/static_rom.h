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
#define STATIC_ROM_NUM_BYTES (90 * 4)
uint32_t static_rom[90];

#define STATIC_ROM_BLURB "\
\t\t\ttest_sleep_wakeup.bin: Put chip to sleep\n"

#endif // STATIC_ROM_H
