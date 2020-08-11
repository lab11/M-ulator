/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef RAM_H
#define RAM_H

#include "core/common.h"

////////////////////////////////////////////////////////////////////////
// Only include this peripheral if requested in the platform memmap.h //
#ifdef RAMBOT

#define HAVE_RAM
#define RAMSIZE (RAMTOP - RAMBOT) // In bytes

void flash_RAM(const uint8_t *image, int offset, uint32_t nbytes);
size_t dump_RAM(FILE *fp);

#endif //RAMBOT
// Only include this peripheral if requested in the platform memmap.h //
////////////////////////////////////////////////////////////////////////

#endif // RAM_H
