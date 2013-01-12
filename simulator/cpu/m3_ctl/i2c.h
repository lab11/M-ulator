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
