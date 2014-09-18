#include "m3_ctrl.h"

// HW BUGFIX:
//
// If you write to memory address that corresponds to I2C message TX
// (0xA0000XXX), and if you have 4 or more of the memory write operation
// consecutively, you would want to insert 3 of NOPs next to the memory write
// instruction.  Here, 'consecutive' means that your following I2C write
// instruction is executed before earlier I2C write instruction is completed.
// Usually it takes ~>50 cycles to complete I2C write instruction since I2C is
// much slower than CPU execution.  So when there is consecutive I2C write
// request, I2C TX FIFO buffer is filled.  When FIFO is full, memory bus stalls
// to prevent I2C request loss.  However, when memory bus resume operation,
// there is a fetch error for next instruction - this is why we insert NOP.
#define I2C_HW_BUG_FIX asm("nop\n\tnop\n\tnop")

// [31:12] = 20'hA0000
// [11:10] = Data length (00:4, 01:1, 10:2, 11:3 bytes)
//   [9:2] = Target I2C Addr
//   [1:0] = 0's
int write_i2c_message(uint8_t addr, uint8_t length, uint32_t data) {
	if ((length < 1) || (length > 4))
		return -1;
	if (length == 4)
		length = 0;

	uint32_t _addr = 0xa0000000;
	_addr |= (length << 10);
	_addr |= (addr << 2);

	*((uint32_t *) _addr) = data;
	I2C_HW_BUG_FIX;
	return 0;
}

// [31:8] = 24'hA00010
//  [7:6] = 2'b00
//  [5:2] = Target Register
//  [1:0] = 2'b00
int read_config_reg(uint8_t reg, uint32_t *data) {
	// Max legal reg is PMU_CTRL 4'b1011
	if (reg > 0xb)
		return -1;

	uint32_t _addr = 0xa0001000;
	_addr |= (reg << 2);

	*data = *((uint32_t *) _addr);
	return 0;
}

// [31:24] = 8'hA1
// [23:19] = DMA target layer
//    [18] = 0/1 Read/Write
// [17:15] = 3'b000
//  [14:2] = Number of words to be copied
//   [1:0] = 2'b00
// Data: [31:16] address of target layer, [15:0] address of current layer
int init_DMA(uint8_t layer, bool write, uint16_t words,
		uint16_t target, uint16_t current) {
	if (! ((layer | 0x3) || (layer | 0x10)) )
		return -1;

	if (layer == LAYER_CTRL_CPU)
		return -1;

	uint32_t _addr = 0xa1000000;
	_addr |= (layer << 19);
	_addr |= (write << 18);
	_addr |= (words << 2);

	uint32_t data = (target << 16) | current;

	*((uint32_t *) _addr) = data;
	return 0;
}

// [31:4] = 28'hA200000
//  [3:2] = Target register
//  [1:0] = 2'b00
int write_config_reg(uint8_t reg, uint32_t data) {
	uint32_t _addr = 0xa2000000;
	_addr |= (reg << 2);

	*((uint32_t *) _addr) = data;
	return 0;
}

int PMU_req(uint8_t req) {
	uint32_t _addr = 0xa3000000;

	if (req > PMU_CTRL_CPU_SLEEP)
		return -1;

	uint32_t data = req; // upper bit are don't care
	*((uint32_t *) _addr) = data;
	return 0;
}

// GPIOs
void GPIO_direction_get(uint32_t *mask) {
	uint32_t* _addr = GPIO_DIR;
	*mask = *_addr;
}

void GPIO_direction_set(uint32_t mask) {
	uint32_t* _addr = GPIO_DIR;
	*_addr = mask;
}

void GPIO_get(uint32_t *mask) {
	uint32_t* _addr = GPIO_DATA;
	*mask = *_addr;
}

void GPIO_set(uint32_t mask) {
	uint32_t* _addr = GPIO_DATA;
	*_addr = mask;
}
