//*******************************************************************
//Author: Pat Pannuto
//Description: MBUS Header file
//*******************************************************************

#include <stdint.h>
#include <stdbool.h>

#define MBUS_CMD0     ((volatile uint32_t *) 0xA0002000)
#define MBUS_CMD1     ((volatile uint32_t *) 0xA0002004)
#define MBUS_CMD2     ((volatile uint32_t *) 0xA0002008)
#define MBUS_FUID_LEN ((volatile uint32_t *) 0xA000200C)

typedef enum {
	MBUS_DISC_AND_ENUM	= 0,
	MBUS_POWER		    = 1,
} mbus_broadcast_channel_t;

typedef enum {
	MBUS_QUERY_CMD		    = 0,
	MBUS_QUERY_RESP		    = 1,
	MBUS_ENUMERATE_RESP	    = 1,
	MBUS_ENUMERATE_CMD	    = 2,
	MBUS_INVALIDATE_PREFIX	= 3,
} mbus_disc_and_enum_cmd_t;

typedef enum {
	MBUS_ALL_SLEEP			    = 0,
	MBUS_ALL_WAKE			    = 1,
	MBUS_SLEEP_BY_SHORT_PREFIX	= 2,
	MBUS_WAKE_BY_SHORT_PREFIX	= 3,
	MBUS_SLEEP_BY_FULL_PREFIX	= 4,
	MBUS_WAKE_BY_FULL_PREFIX	= 5,
} mbus_power_cmd_t;

typedef enum {
	MPQ_REG_WRITE			    = 0,
	MPQ_REG_READ			    = 1,
	MPQ_MEM_BULK_WRITE		    = 2,
	MPQ_MEM_READ			    = 3,
	MPQ_MEM_STREAM_WRITE_CH0	= 4,
	MPQ_MEM_STREAM_WRITE_CH1	= 5,
	MPQ_MEM_STREAM_WRITE_CH2	= 6,
	MPQ_MEM_STREAM_WRITE_CH3	= 7,
} mpq_command_t;


/**
 * @brief Get this node's short prefix
 */
uint8_t mbus_get_short_prefix(void);

/**
 * @brief Send a 32-bit MBus message
 *
 * @param addr MBus short address (prefix+FU_ID) to send to
 * @param data Actual 32-bit data to be sent
 */
uint32_t mbus_write_message32(uint8_t addr, uint32_t data);

/**
 * @brief Send an arbitrary MBus message
 *
 * @param addr MBus short address (prefix+FU_ID) to send to
 * @param data A buffer to send data from
 * @param length The amount of data from @p data to send. May be 0 (no-op).
 */
uint32_t mbus_write_message(uint8_t addr, uint32_t data[], unsigned length);

/**
 * @brief Send an MBus Enumeration command
 *
 * This function provides low-level access, directly sending an enumeration
 * request. It is the caller's responsibility to ensure no conflicting short
 * prefixes are assigned.
 *
 * @param new_prefix The new short prefix to assign to a node ( > 0 && < 16)
 * @return The full_prefix of the assigned node, or 0xffffff if no node responds
 *
 * TODO: Return value is currently not correct.
 */
void mbus_enumerate(unsigned new_prefix);


/**
 * @brief Put all MBus nodes to sleep
 */
void mbus_sleep_all(void);

/**
 * @brief Put a specific MBus layer to sleep
 */
void mbus_sleep_layer_short(uint8_t addr);

/**
 * @brief Copy registers from this node to another node
 *
 * @param remote_prefix The short prefix of the destination node (range [1,15] incl)
 * @param remote_reg_start The starting register address on the destination node
 * @param local_reg_start  The register address to read from on this node
 * @param length_minus_one The number of register to write MINUS ONE (0 -> 1, 255->256)
 */
void mbus_copy_registers_from_local_to_remote(
		uint8_t remote_prefix,
		uint8_t remote_reg_start,
		uint8_t local_reg_start,
		uint8_t length_minus_one
		);

/**
 * @brief Copy registers from another node to this node
 *
 * @param remote_prefix The short prefix of the remote node
 * @param remote_reg_start The starting register address on the remote node
 * @param local_reg_start  The register address to start writing to on this node
 * @param length_minus_one The number of regsiters to copy MINUS ONE
 */
void mbus_copy_registers_from_remote_to_local(
		uint8_t remote_prefix,
		uint8_t remote_reg_start,
		uint8_t local_reg_start,
		uint8_t length_minus_one
		);

/**
 * @brief Copy registers from one remote node to another remote node
 *
 * @param source_prefix    The node to copy from
 * @param source_reg_start The register on the source to start copying from
 * @param dest_prefix      The node to copy to
 * @param dest_reg_start   The register on the destination to start writing to
 * @param length_minus_one The number of registers to copy MINUS ONE
 */
void mbus_copy_registers_from_remote_to_remote(
		uint8_t source_prefix,
		uint8_t source_reg_start,
		uint8_t dest_prefix,
		uint8_t dest_reg_start,
		uint8_t length_minus_one
		);

/**
 * @brief Write a register on a remote node
 *
 * @param prefix The short prefix of the destination node (range [1,15] incl)
 * @param dst_reg_addr The address to write on the destination node
 * @param dst_reg_val  The value to write to remote register (24 bit value)
 */
void mbus_remote_register_write(
		uint8_t prefix,
		uint8_t dst_reg_addr,
		uint32_t dst_reg_val
		);

/**
 * @brief Convenience function to read a single register from a remote node
 *
 * @param remote_prefix   The remote node to read from
 * @param remote_reg_addr The register to read on the remote
 * @param local_reg_addr  The register to save the value to on this node
 */
void mbus_remote_register_read(
		uint8_t remote_prefix,
		uint8_t remote_reg_addr,
		uint8_t local_reg_addr
		);

/**
 * @brief Copy memory from this node to a remote node (bulk)
 *
 * @param remote_prefix             MBus address of the remote node to write to
 * @param remote_memory_address     Memory address on the remote node to write to
 * @param local_address             Memory address on this node to read from
 * @param length_in_words_minus_one The number of WORDS of memory to transfer MINUS ONE
 */
void mbus_copy_mem_from_local_to_remote_bulk(
		uint8_t   remote_prefix,
		uint32_t* remote_memory_address,
		uint32_t* local_address,
		uint32_t  length_in_words_minus_one
		);

/**
 * @brief Copy memory from a remote node to any node (bulk)
 *
 * @param source_prefix              MBus address of the remote node to read from
 * @param source_memory_address      Memory address on the remote node to read from
 * @param destination_prefix         MBus address of the node to write to
 * @param destination_memory_address Memory address on this node to write to
 * @param length_in_words_minus_one  The number of WORDS of memory to transfer MINUS ONE
 */
void mbus_copy_mem_from_remote_to_any_bulk (
		uint8_t   source_prefix,
		uint32_t* source_memory_address,
		uint8_t   destination_prefix,
		uint32_t* destination_memory_address,
		uint32_t  length_in_words_minus_one
		);

/**
 * @brief Copy memory from this node to a remote node (stream)
 *
 * @param stream_channel            MBus streaming channel (0 to 3)
 * @param remote_prefix             MBus address of the remote node to write to
 * @param local_address             Memory address on this node to read from
 * @param length_in_words_minus_one The number of WORDS of memory to transfer MINUS ONE
 */
void mbus_copy_mem_from_local_to_remote_stream(
        uint8_t   stream_channel,
		uint8_t   remote_prefix,
		uint32_t* local_address,
		uint32_t  length_in_words_minus_one
		);

/**
 * @brief Copy memory from a remote node to any node (stream)
 *
 * @param stream_channel             MBus streaming channel (0 to 3)
 * @param source_prefix              MBus address of the remote node to read from
 * @param source_memory_address      Memory address on the remote node to read from
 * @param destination_prefix         MBus address of the node to write to
 * @param length_in_words_minus_one  The number of WORDS of memory to transfer MINUS ONE
 */
void mbus_copy_mem_from_remote_to_any_stream (
        uint8_t   stream_channel,
		uint8_t   source_prefix,
		uint32_t* source_memory_address,
		uint8_t   destination_prefix,
		uint32_t  length_in_words_minus_one
		);

/*----------------------------------------------------------------------------*
* MemSet
*/
/*! \brief Fill memory with a specified 16 bit word
*
* \param s Pointer to the destination buffer.
* \param c Value to be written.
* \param len Size of the destination buffer.
*
* \return Original pointer to the destination buffer
*/
/*---------------------------------------------------------------------------*/
void* memset(void *s, uint32_t c, uint32_t len);
