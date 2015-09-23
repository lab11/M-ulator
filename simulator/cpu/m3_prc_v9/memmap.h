/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2012  Pat Pannuto <pat.pannuto@gmail.com>
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

#define RAMBOT		0x00000000
#define RAMTOP		0x00000C00

#define MBUS_MMIO_ADDR	0xA0000000
#define MBUS_MMIO_DATA	0xA0000004

#define MSG_REG0_RD	0xA0001000
#define MSG_REG1_RD	0xA0001004
#define MSG_REG2_RD	0xA0001008
#define MSG_REG3_RD	0xA000100c
#define INT_MSG_REG0_RD	0xA0001010
#define INT_MSG_REG1_RD	0xA0001014
#define INT_MSG_REG2_RD	0xA0001018
#define INT_MSG_REG3_RD	0xA000101c

// CHIP ID (used for selective GOC programming)
#define CHIP_ID_REG_NBITS	16
#define CHIP_ID_REG_RESET	0x0000
#define CHIP_ID_REG_MBUS	0x8
#define CHIP_ID_REG_RD		0xA0001020
#define CHIP_ID_REG_WR		0xA2000000

// MBus maximum length
// Any MBus message longer than this value will be terminated by MBus controller
#define MBUS_THRES_REG_NBITS	20
#define MBUS_THRES_REG_RESET	0x01000
#define MBUS_THRES_REG_MBUS	0x9
#define MBUS_THRES_REG_RD	0xA0001024
#define MBUS_THRES_REG_WR	0xA2000004

// GOC Control
#define GOC_CTRL_REG_NBITS	24 // 25 for PRE
#define GOC_CTRL_REG_RESET	0x202903 // {0, 0, 010, 000, 0, xx, 10, 10, 01, 0, 0, 00, 0011}
                          		 // {0 0010 0000 xx10 1001 0000 0011}
#define GOC_CTRL_REG_MBUS	0xA
#define GOC_CTRL_REG_RD		0xA0001028
#define GOC_CTRL_REG_WR		0xA2000008

// PMU Control
#define PMU_CTRL_REG_NBITS	32
#define PMU_CTRL_REG_RESET	0x8f770049	// {1,0,0,0,11,11,0111,0111,00,000,00,00,100,10,01}
                          			// {1000 1111 0111 0111 0000 0000 0100 1001}
#define PMU_CTRL_REG_MBUS	0xB
#define PMU_CTRL_REG_RD		0xA000102C
#define PMU_CTRL_REG_WR		0xA200000C


#define WUP_CTRL_REG_NBITS	16
#define WUP_CTRL_REG_RESET	0x0000
#define WUP_CTRL_REG_MBUS	0xC
#define WUP_CTRL_REG_RD		0xA0001030
#define WUP_CTRL_REG_WR		0xA2000010


#define TSTAMP_REG_NBITS	16
#define TSTAMP_REG_RESET	0x0000
#define TSTAMP_REG_MBUS		0xD
#define TSTAMP_REG_RD		0xA0001034
#define TSTAMP_REG_WR		0xA2000014


#define PMU_RST_REG_NBITS	3
#define PMU_RST_REG_RESET	0x0
// No MBus, no read
#define PMU_RST_REG_WR		0xA3000000


#define GPIO_BOT	0xA4000000
#define GPIO_TOP	0xA5000000

#define SPI_BOT		0xAF000000
#define SPI_TOP		0xB0000000

#endif // MEMMAP_H
