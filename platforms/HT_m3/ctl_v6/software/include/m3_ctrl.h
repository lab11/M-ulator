#ifndef M3_H
#define M3_H

#include <stdint.h>
#include <stdbool.h>

// All functions return 0 on sucess, non-zero on failure.

// Valid lengths are 1-4 bytes.
int write_i2c_message(uint8_t addr, uint8_t length, uint32_t data);

// Possible reg values:
// Read-Only
//   4'b0000~4'b0011: (Non-INT) MSG REG 0~3
//   4'b0100~4'b0111: INT MSG REG 0~3
// R/W
//           4'b1000: CHIP_ID
//           4'b1001: DMA_INFO
//           4'b1010: GOC_CTRL
//           4'b1011: PMU_CTRL
int read_config_reg(uint8_t reg, uint32_t *data);
int write_config_reg(uint8_t reg, uint32_t data);

#define LAYER_DSP_CPU	0x00
#define LAYER_CTRL_CPU	0x01
#define LAYER_RF	0x02
#define LAYER_IT	0x03
#define LAYER_MASK	0x10
int init_DMA(uint8_t layer, bool write, uint16_t words,
		uint16_t target, uint16_t current);

#define PMU_CTRL_CPU_SLEEP	0x2
#define PMU_RESET		0x1
#define PMU_DIV5_OVERRIDE	0x0
int PMU_req(uint8_t req);

// GPIOs
//
// GPIOs are set en-masse, the bottom 24-bits of the 'mask' variable
// correspond to GPIOs 0-23. The top 8 bits are unused

#define GPIO_DATA	((uint32_t *) 0xA4000000)
#define GPIO_DIR	((uint32_t *) 0xA4001000)
#define GPIO_INTMASK	((uint32_t *) 0xA4001040)

void GPIO_direction_get(uint32_t *mask);
void GPIO_direction_set(uint32_t mask);

void GPIO_get(uint32_t *mask);
void GPIO_set(uint32_t mask);

#endif // M3_H
