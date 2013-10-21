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

#define I2C_BOT_WR	0xA0000000
#define I2C_TOP_WR	0xA0001000

#define MSG_REG0_RD	0xA0001000
#define MSG_REG1_RD	0xA0001004
#define MSG_REG2_RD	0xA0001008
#define MSG_REG3_RD	0xA000100c
#define INT_MSG_REG0_RD	0xA0001010
#define INT_MSG_REG1_RD	0xA0001014
#define INT_MSG_REG2_RD	0xA0001018
#define INT_MSG_REG3_RD	0xA000101c
#define CHIP_ID_REG_RD	0xA0001020
#define DMA_INFO_REG_RD	0xA0001024
#define GOC_CTRL_REG_RD	0xA0001028
#define PMU_CTRL_REG_RD	0xA000102c
#define WUP_CTRL_REG_RD	0xA0001030
#define TSTAMP_REG_RD	0xA0001034

#define DMA_BOT_WR	0xA1000000
#define DMA_TOP_WR	0xA2000000

#define CHIP_ID_REG_WR	0xA2000000
#define DMA_INFO_REG_WR	0xA2000004
#define GOC_CTRL_REG_WR	0xA2000008
#define PMU_CTRL_REG_WR	0xA200000C
#define WUP_CTRL_REG_WR	0xA2000010
#define TSTAMP_REG_WR	0xA2000014

#define PMU_SPECIAL	0xA3000000

#define GPIO_BOT	0xA4000000
#define GPIO_TOP	0xA5000000

#define SPI_BOT		0xAF000000
#define SPI_TOP		0xB0000000

#endif // MEMMAP_H
