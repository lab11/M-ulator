/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2016  Pat Pannuto <pat.pannuto@gmail.com>
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

#include "gpio.h"

#include "cpu/core.h"
#include "cpu/periph.h"

#define ADDR_TO_PORT_IDX(_a)     ((_a - GPIO_BASE) / GPIO_PORT_SIZE)
#define ADDR_TO_PORT_CMN(_a)     ((_a - GPIO_BASE) - (ADDR_TO_PORT_IDX(_a) * GPIO_PORT_SIZE))
#define ADDR_TO_PORT_FN(_a)      ((ADDR_TO_PORT_CMN(_a) % 0x10) / 4)
#define ADDR_TO_PORT_REG_IDX(_a) (ADDR_TO_PORT_CMN(_a) - (ADDR_TO_PORT_FN(_a) * 4))

struct sam4l_gpio_port_rsct_t {
	// The "simple" part which is an array of RW/Set/Clear/Toggle ports
	uint32_t GPIO_enable;
	uint32_t peripheral_mux0;
	uint32_t peripheral_mux1;
	uint32_t peripheral_mux2;
	uint32_t output_driver_enable;
	uint32_t output_value;
	uint32_t pin_value;
	uint32_t pullup_enable;
	uint32_t pulldown_enable;
	uint32_t interrupt_enable;
	uint32_t interrupt_mode0;
	uint32_t interrupt_mode1;
	uint32_t interrupt_mode2;
	uint32_t glitch_filter_enable;
	uint32_t interrupt_flag;
	uint32_t _reserved_0E0;
	uint32_t _reserved_0F0;
	uint32_t output_driving_capability0;
	uint32_t output_driving_capability1;
	uint32_t _reserved_120;
	uint32_t output_slew_rate0;
	uint32_t _reserved_140;
	uint32_t _reserved_150;
	uint32_t schmitt_trigger_enable;
	uint32_t _reserved_170;
	uint32_t event_enable;
};

struct sam4l_gpio_port_t {
	union {
		struct sam4l_gpio_port_rsct_t rsct_struct;
	};
	// gap
	uint32_t parameter;
	uint32_t version;
};

static struct sam4l_gpio_port_t ports[GPIO_PORT_COUNT];

static void sam4l_gpio_reset(void) {
	// TODO Check actual chip behavoir
	memset(ports, 0, sizeof(struct sam4l_gpio_port_t) * GPIO_PORT_COUNT);
}

static void print_gpio(void) {
	// TODO Print state of gpio peripheral
}

static bool gpio_read(uint32_t addr, uint32_t *val,
		bool debugger __attribute__ ((unused)) ) {
	if (ADDR_TO_PORT_CMN(addr) < 0x190) {
		if (ADDR_TO_PORT_FN(addr) != 0) {
			WARN("Read of write-only GPIO control\n");
			return false;
		}
		uint32_t* port_base = (uint32_t*) &ports[ADDR_TO_PORT_IDX(addr)];
		*val = *(port_base + ADDR_TO_PORT_REG_IDX(addr));
		return true;
	} else if (ADDR_TO_PORT_CMN(addr) == 0x1F8) {
		*val = ports[ADDR_TO_PORT_IDX(addr)].parameter;
		return true;
	} else if (ADDR_TO_PORT_CMN(addr) == 0x1FC) {
		*val = ports[ADDR_TO_PORT_IDX(addr)].version;
		return true;
	} else {
		WARN("Read of reserved GPIO space\n");
		return false;
	}
}

static void gpio_write(uint32_t addr, uint32_t val,
		bool debugger __attribute__ ((unused)) ) {
	TODO("SAM4L GPIO write is NOP: %08x = %08x\n", addr, val);
	/*
	if (ADDR_TO_PORT_CMN(addr) < 0x190) {
		// TODO Behavorial effects
		uint32_t* port_base = (uint32_t*) &ports[ADDR_TO_PORT_IDX(addr)];
		uint32_t* reg = port_base + ADDR_TO_PORT_REG_IDX(addr);
		switch (ADDR_TO_PORT_FN(addr)) {
			case 0:
				// Write
				// XXX 0x060, 0x0D0 access limits
				*reg = val;
				return;
			case 1:
				// Set
				*reg = *reg | val;
				return;
			case 2:
				// Clear
				*reg = *reg & ~val;
				return;
			case 3:
				// Toggle
				*reg = *reg ^ val;
				return;
		}
	} else if (ADDR_TO_PORT_CMN(addr) == 0x1F8) {
		ports[ADDR_TO_PORT_IDX(addr)].parameter = val;
	} else if (ADDR_TO_PORT_CMN(addr) == 0x1FC) {
		ports[ADDR_TO_PORT_IDX(addr)].version = val;
	} else {
		WARN("Write to reserved GPIO space (ignored)\n");
	}
	*/
}

__attribute__ ((constructor))
void register_sam4l_gpio_periph(void) {
	register_reset(sam4l_gpio_reset);

	union memmap_fn mem_fn;

	mem_fn.R_fn32 = gpio_read;
	register_memmap("SAM4L GPIO", false, 4, mem_fn, GPIO_BASE, GPIO_END);
	mem_fn.W_fn32 = gpio_write;
	register_memmap("SAM4L GPIO LED", true, 4, mem_fn, GPIO_BASE, GPIO_END);

	register_periph_printer(print_gpio);
}
