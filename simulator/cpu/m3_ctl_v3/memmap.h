/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
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
