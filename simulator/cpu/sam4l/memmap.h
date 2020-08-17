/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef MEMMAP_H
#define MEMMAP_H

// n.b. these are for the ATSAM4L8, 512 Kbyte flash, 64 Kbyte SRAM

#define ROMBOT 0x00000000
#define ROMTOP 0x00080000

#define BOOTLOADER_BOT 0x00000000
#define BOOTLOADER_TOP 0x00010000
#define BOOTLOADER_REMAP_VECTOR_TABLE BOOTLOADER_TOP

#define RAMBOT 0x20000000
#define RAMTOP 0x20010000

// Debugging
#define PRINT_ROM_ENABLE

#endif // MEMMAP_H
