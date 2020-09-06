/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef ROM_H
#define ROM_H

#include "core/common.h"

////////////////////////////////////////////////////////////////////////
// Only include this peripheral if requested in the platform memmap.h //
#ifdef ROMBOT
#define HAVE_ROM

#define ROMSIZE (ROMTOP - ROMBOT) // In bytes

#ifndef PP_STRING
#define PP_STRING "ROM"
#include "core/pretty_print.h"
#endif

void flash_ROM(const uint8_t *image, int offset, uint32_t nbytes);
#ifdef PRINT_ROM_ENABLE
size_t dump_ROM(FILE *fp);
#endif

#endif // ROMBOT
// Only include this peripheral if requested in the platform memmap.h //
////////////////////////////////////////////////////////////////////////

#endif // ROM_H
