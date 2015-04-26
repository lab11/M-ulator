//*******************************************************************
//Author: ZhiYoong Foo
//Description: MBUS Header file
//*******************************************************************

#include <stdint.h>
#include <stdbool.h>

// All functions return 0 on sucess, non-zero on failure.
int write_mbus_message(uint32_t addr, uint32_t data);
int write_mbus_msg(uint32_t short_prefix, uint32_t fuid, uint32_t data);
int enumerate(uint32_t addr);
int sleep();
int write_mbus_register(uint32_t enum_addr, uint8_t reg, uint32_t data);
int read_mbus_register(uint32_t enum_addr, uint8_t reg, uint8_t return_addr);

