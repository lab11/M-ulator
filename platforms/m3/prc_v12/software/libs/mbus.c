//*******************************************************************
//Author: Pat Pannuto
//Description: MBUS library file
//*******************************************************************

#include "mbus.h"

#define MBUS_CMD0     ((volatile uint32_t *) 0xA0002000)
#define MBUS_CMD1     ((volatile uint32_t *) 0xA0002004)
#define MBUS_CMD2     ((volatile uint32_t *) 0xA0002008)
#define MBUS_FUID_LEN ((volatile uint32_t *) 0xA000200C)


uint8_t mbus_get_short_prefix(void) {
	// TODO: Read from LC
	return 1;
}


void mbus_write_message(uint8_t addr, uint32_t data[], unsigned len) {
	// Goal: Use the "Memory Stream Write" to put unconstrained 32-bit data
	//       onto the bus.
	if (len == 0) return;
	//if (len >= 2**20) return ESIZE;

	*MBUS_CMD0 = (addr << 24) | (len-1);
	*MBUS_CMD1 = (uint32_t) data;

	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x2 << 4);

	// TODO: async / or sync wait for confirm?
}

uint32_t mbus_enumerate(unsigned new_prefix) {
	// assert ( (new_prefix > 0) && (new_prefix < 16) );
	uint32_t payload = (MBUS_ENUMERATE_CMD << 28) | (new_prefix << 24);
	mbus_write_message(MBUS_DISC_AND_ENUM, &payload, 1);

	// TODO: real return value
	return 0;
}

void mbus_all_sleep(void) {
	uint32_t payload = (MBUS_ALL_SLEEP << 28);
	mbus_write_message(MBUS_POWER, &payload, 1);
}

void mbus_copy_registers_from_local_to_remote(
		uint8_t remote_prefix,
		uint8_t remote_reg_start,
		uint8_t local_reg_start,
		uint8_t length_minus_one
		) {
	// Simulate a request from the remote node to read this node

	// assert (remote_prefix < 16 && > 0);
	*MBUS_CMD0 =
		(remote_reg_start << 24) |
		(length_minus_one << 16) |
		((remote_prefix & 0xf) << 12) |
		(MPQ_REG_WRITE << 8) | // Write regs *to* remote node
		(local_reg_start << 0);

	// Read registers *from* this node
	*MBUS_FUID_LEN = MPQ_REG_READ | (0x1 << 4);
}

void mbus_copy_registers_from_remote_to_local(
		uint8_t remote_prefix,
		uint8_t remote_reg_start,
		uint8_t local_reg_start,
		uint8_t length_minus_one
		) {
	// Put a register read command on the bus instructed to write this node

	uint32_t address = ((remote_prefix & 0xf) << 4) | MPQ_REG_READ;
	uint32_t data =
		(remote_reg_start << 24) |
		(length_minus_one << 16) |
		(mbus_get_short_prefix() << 12) |
		(MPQ_REG_WRITE << 8) | // Write regs *to* _this_ node
		(local_reg_start << 0);

	mbus_write_message(address, &data, 1);
}

void mbus_copy_registers_from_remote_to_remote(
		uint8_t source_prefix,
		uint8_t source_reg_start,
		uint8_t dest_prefix,
		uint8_t dest_reg_start,
		uint8_t length_minus_one
		) {
	// Put a register read command on the bus instructed to write dest node

	uint32_t address = ((source_prefix & 0xf) << 4) | MPQ_REG_READ;
	uint32_t data =
		(source_reg_start << 24) |
		(length_minus_one << 16) |
		((dest_prefix & 0xf) < 12) |
		(MPQ_REG_WRITE << 8) |
		(dest_reg_start << 0);

	mbus_write_message(address, &data, 1);
}

void mbus_remote_register_write(
		uint8_t prefix,
		uint8_t dst_reg_addr,
		uint32_t dst_reg_val
		) {
	// assert (prefix < 16 && > 0);
	uint8_t address = ((prefix & 0xf) << 4) | MPQ_REG_WRITE;
	uint32_t data = (dst_reg_addr << 24) | (dst_reg_val & 0xffffff);
	mbus_write_message(address, &data, 1);
}

inline
void mbus_remote_register_read(
		uint8_t remote_prefix,
		uint8_t remote_reg_addr,
		uint8_t local_reg_addr
		) {
	mbus_copy_registers_from_remote_to_local(
			remote_prefix, remote_reg_addr, local_reg_addr, 0);
}

void mbus_DMA_local_to_remote(
		uint8_t   remote_mbus_address,
		uint32_t* remote_memory_address,
		uint32_t* local_address,
		uint32_t  length_in_words_minus_one
		) {
	*MBUS_CMD0 = ( ((uint32_t) remote_mbus_address) << 24 ) | (length_in_words_minus_one & 0xFFFFF);
	*MBUS_CMD1 = (uint32_t) local_address;
	*MBUS_CMD2 = (uint32_t) remote_memory_address;

	*MBUS_FUID_LEN = MPQ_MEM_BULK_WRITE | (0x3 << 4);

	// TODO: async / or sync wait for confirm?
}

void mbus_DMA_remote_to_any(
		uint8_t   source_mbus_address,
		uint32_t* source_memory_address,
		uint8_t   destination_mbus_address,
		uint32_t* destination_memory_address,
		uint32_t  length_in_words_minus_one
		) {
	uint32_t payload[3] = {
		( ((uint32_t) destination_mbus_address) << 24 ) | (length_in_words_minus_one & 0xFFFFF),
		(uint32_t) source_memory_address,
		(uint32_t) destination_memory_address,
	};
	mbus_write_message(source_mbus_address, payload, 3);
}


//Reads Register <reg> on <enum_addr> layer and returns data to <return_addr>
//ONLY FOR RADv4!
void read_mbus_register_RADv4(uint32_t enum_addr, uint8_t reg, uint8_t return_addr){
  uint32_t _mbus_addr = 1;
  uint32_t _mbus_data = (return_addr << 16);
  _mbus_addr |= (enum_addr << 4);
  _mbus_data |= (reg << 24);
  mbus_write_message(_mbus_addr,&_mbus_data,1);
}
