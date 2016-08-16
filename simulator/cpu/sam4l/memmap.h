/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2016  Pat Pannuto <pat.pannuto@gmail.com>
 *
 * This file is part of Mulator.
 *
 * Mulator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mulator.  If not, see <http://www.gnu.org/licenses/>.
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
