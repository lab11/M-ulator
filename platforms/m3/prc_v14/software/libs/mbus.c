//*******************************************************************
//Author: Pat Pannuto
//Description: MBUS library file
//*******************************************************************

#include "mbus.h"

uint8_t mbus_get_short_prefix(void) {
	// TODO: Read from LC
	return 1;
}

uint32_t mbus_write_message32(uint8_t addr, uint32_t data) {
    uint32_t mbus_addr = 0xA0003000 | (addr << 4);
    *((volatile uint32_t *) mbus_addr) = data;
    return 1;
}

uint32_t mbus_write_message(uint8_t addr, uint32_t data[], unsigned len) {
	// Goal: Use the "Memory Stream Write" to put unconstrained 32-bit data
	//       onto the bus.
	if (len == 0) return 0;

	*MBUS_CMD0 = (addr << 24) | (len-1);
	*MBUS_CMD1 = (uint32_t) data;
	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x2 << 4);

    return 1;
}

void mbus_enumerate(unsigned new_prefix) {
    mbus_write_message32(MBUS_DISC_AND_ENUM, ((MBUS_ENUMERATE_CMD << 28) | (new_prefix << 24)));
}

void mbus_sleep_all(void) {
    mbus_write_message32(MBUS_POWER, MBUS_ALL_SLEEP << 28);
}

void mbus_sleep_layer_short(uint8_t addr) {
	mbus_write_message32(0x01, (0x2<<28) + (0x1<<(addr+12)));
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
		(local_reg_start << 24) |
		(length_minus_one << 16) |
		((remote_prefix & 0xf) << 12) |
		(MPQ_REG_WRITE << 8) | // Write regs *to* remote node
		(remote_reg_start << 0);

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

	uint8_t address = ((remote_prefix & 0xf) << 4) | MPQ_REG_READ;
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

void mbus_remote_register_read(
		uint8_t remote_prefix,
		uint8_t remote_reg_addr,
		uint8_t local_reg_addr
		) {
	mbus_copy_registers_from_remote_to_local(
			remote_prefix, remote_reg_addr, local_reg_addr, 0);
}

void mbus_copy_mem_from_local_to_remote_bulk(
		uint8_t   remote_prefix,
		uint32_t* remote_memory_address,
		uint32_t* local_address,
		uint32_t  length_in_words_minus_one
		) {
	*MBUS_CMD0 = ( ((uint32_t) remote_prefix) << 28 ) | (MPQ_MEM_BULK_WRITE << 24) | (length_in_words_minus_one & 0xFFFFF);
	*MBUS_CMD1 = (uint32_t) local_address;
	*MBUS_CMD2 = (uint32_t) remote_memory_address;

	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x3 << 4);
}

void mbus_copy_mem_from_remote_to_any_bulk (
		uint8_t   source_prefix,
		uint32_t* source_memory_address,
		uint8_t   destination_prefix,
		uint32_t* destination_memory_address,
		uint32_t  length_in_words_minus_one
		) {
	uint32_t payload[3] = {
		( ((uint32_t) destination_prefix) << 28 )| (MPQ_MEM_BULK_WRITE << 24) | (length_in_words_minus_one & 0xFFFFF),
		(uint32_t) source_memory_address,
		(uint32_t) destination_memory_address,
	};
	mbus_write_message(((source_prefix << 4 ) | MPQ_MEM_READ), payload, 3);
}

void mbus_copy_mem_from_local_to_remote_stream(
        uint8_t   stream_channel,
		uint8_t   remote_prefix,
		uint32_t* local_address,
		uint32_t  length_in_words_minus_one
		) {

	*MBUS_CMD0 = ( ((uint32_t) remote_prefix) << 28 ) | (0x1 << 26) | (((uint32_t) stream_channel) << 24) | (length_in_words_minus_one & 0xFFFFF);
	*MBUS_CMD1 = (uint32_t) local_address;

	*MBUS_FUID_LEN = MPQ_MEM_READ | (0x2 << 4);
}

void mbus_copy_mem_from_remote_to_any_stream (
        uint8_t   stream_channel,
		uint8_t   source_prefix,
		uint32_t* source_memory_address,
		uint8_t   destination_prefix,
		uint32_t  length_in_words_minus_one
		) {

	uint32_t payload[2] = {
		( ((uint32_t) destination_prefix) << 28 )| (0x1 << 26) | (((uint32_t) stream_channel) << 24) | (length_in_words_minus_one & 0xFFFFF),
		(uint32_t) source_memory_address,
	};
	mbus_write_message(((source_prefix << 4 ) | MPQ_MEM_READ), payload, 2);
}

void* memset(void *s, uint32_t c, uint32_t len) {
    unsigned char *dst = s;
    while (len > 0) {
        *dst = (unsigned char) c;
        dst++;
        len--;
    }
    return s;
}
