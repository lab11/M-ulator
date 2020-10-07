/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef STATIC_ROM_H
#define STATIC_ROM_H

// bintoarray.sh; "echo.bin"
#include <stdint.h>
#define STATIC_ROM_NUM_BYTES (107 * 4)
extern uint32_t static_rom[107];

#define STATIC_ROM_BLURB "\
\t\t\techo.bin: Listens for a character on the polling UART\n\
\t\t\t          device and echoes it back\n"

#endif // STATIC_ROM_H
