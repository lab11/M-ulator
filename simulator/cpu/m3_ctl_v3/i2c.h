/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef I2C_H
#define I2C_H

#include "core/common.h"

#ifndef PP_STRING
#define PP_STRING "I2C"
#include "core/pretty_print.h"
#endif

struct i2c_instance;

bool i2c_send_message(struct i2c_instance* t,
		uint8_t address, uint32_t length, const char *msg);

// This function *MUST* be called from a peripheral's constructor
struct i2c_instance* create_i2c_instance(const char *periph_name,
		void (*async_recv_message)(uint8_t, uint32_t, char *),
		uint8_t ones_mask, uint8_t zeros_mask,
		const char *host, const uint16_t port);

#endif // I2C_H
