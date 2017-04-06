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

#include <unistd.h>

#include "m3_ctl.h"
#include "i2c.h"
#include "ice_bridge.h"

#include "memmap.h"

#include "cpu/periph.h"
#include "cpu/core.h"

#include "core/state_sync.h"

// GLOBAL STATE
struct i2c_instance* i2c;
struct ice_instance* ice;

// CPU CONF REG'S
static uint32_t m3_ctl_reg_chip_id;
static uint32_t m3_ctl_reg_dma_info;
static uint32_t m3_ctl_reg_goc_ctrl;
static uint32_t m3_ctl_reg_pmu_ctrl;
static uint32_t m3_ctl_reg_wup_ctrl;
static uint32_t m3_ctl_reg_tstamp;

static uint32_t m3_ctl_reg_msg0;
static uint32_t m3_ctl_reg_msg1;
static uint32_t m3_ctl_reg_msg2;
static uint32_t m3_ctl_reg_msg3;
static uint32_t m3_ctl_reg_imsg0;
static uint32_t m3_ctl_reg_imsg1;
static uint32_t m3_ctl_reg_imsg2;
static uint32_t m3_ctl_reg_imsg3;

// GPIO REG'S
static uint32_t gpio_dir;	// 0-input, 1-output
static uint32_t gpio_int_mask;	// INT if dir == 0 and any edge detected. IRQ4
static uint32_t gpio_data_I;
static uint32_t gpio_data_O;

static void m3_ctl_reset(void) {
	m3_ctl_reg_chip_id = 0;
	m3_ctl_reg_dma_info = 0x02000080;

	//1 010 000 0 -- 10 10 01 1 0 11 0010
	// 1010 0000 --10 1001 1011 0010
	m3_ctl_reg_goc_ctrl = 0xa029b2;

	//-- 0 0 11 11 0111 0111 00 000 00 00 010 10 01
	// --00 1111 0111 0111 0000 0000 0010 1001
	m3_ctl_reg_pmu_ctrl = 0x0f770039;

	m3_ctl_reg_wup_ctrl = 0;
	m3_ctl_reg_tstamp = 0;

	m3_ctl_reg_msg0 = 0;
	m3_ctl_reg_msg1 = 0;
	m3_ctl_reg_msg2 = 0;
	m3_ctl_reg_msg3 = 0;
	m3_ctl_reg_imsg0 = 0;
	m3_ctl_reg_imsg1 = 0;
	m3_ctl_reg_imsg2 = 0;
	m3_ctl_reg_imsg3 = 0;
}

__attribute__ ((constructor))
void register_reset_m3_ctl(void) {
	register_reset(m3_ctl_reset);
}

// XXX: External linkage, register hook?
void recv_i2c_message(uint8_t addr, uint32_t length, uint8_t *data) {
	switch (addr) {
		case 0xe0:
			// I2C_CHIP_ID_REG_WR? XXX: Flesh out I2C interface
			if (length > 2)
				WARN("Target reg is 16 bits, truncated (len %d)\n", length);
			SW(&m3_ctl_reg_chip_id, *((uint16_t *) data));
			break;
		case 0xe2:
			// I2C_DMA_INFO_REG_WR?
			if (length != 4)
				CORE_ERR_unpredictable("I2C_DMA_INFO_REG_WR bad length\n");
			SW(&m3_ctl_reg_dma_info, *((uint32_t *) data));
			break;
		case 0xe4:
			// I2C_GOC_CTRL_REG_WR?
			if (length != 4)
				CORE_ERR_unpredictable("I2C_GOC_CTRL_REG_WR bad length\n");
			SW(&m3_ctl_reg_goc_ctrl, *((uint32_t *) data) & 0x00ffffff);
			break;
		case 0xe6:
			// I2C_PMU_CTRL_REG_WR?
			if (length != 4)
				CORE_ERR_unpredictable("I2C_PMU_CTRL_REG_WR bad length\n");
			SW(&m3_ctl_reg_pmu_ctrl, *((uint32_t *) data));
			break;
		case 0xe8:
			// I2C_WUP_CTRL_REG_WR?
			if (length != 2)
				CORE_ERR_unpredictable("I2C_WUP_CTRL_REG_WR bad length\n");
			SW(&m3_ctl_reg_wup_ctrl, *((uint16_t *) data));
			break;
		case 0xea:
			// I2C_TSTAMP_REG_WR?
			// Data is ignored, register is reset (0'd)
			SW(&m3_ctl_reg_tstamp, 0);
			break;

		case 0x80:
			// I2C_MSG_REG0_WR?
			if (length != 4)
				CORE_ERR_unpredictable("I2C_MSG_REG0_WR bad length\n");
			SW(&m3_ctl_reg_msg0, *((uint32_t *) data));
			break;
		case 0x82:
			// I2C_MSG_REG1_WR?
			if (length != 4)
				CORE_ERR_unpredictable("I2C_MSG_REG1_WR bad length\n");
			SW(&m3_ctl_reg_msg1, *((uint32_t *) data));
			break;
		case 0x84:
			// I2C_MSG_REG2_WR?
			if (length != 4)
				CORE_ERR_unpredictable("I2C_MSG_REG2_WR bad length\n");
			SW(&m3_ctl_reg_msg2, *((uint32_t *) data));
			break;
		case 0x86:
			// I2C_MSG_REG3_WR?
			if (length != 4)
				CORE_ERR_unpredictable("I2C_MSG_REG3_WR bad length\n");
			SW(&m3_ctl_reg_msg3, *((uint32_t *) data));
			break;
		case 0x88:
			// I2C_IMSG_REG0_WR?
			if (length != 4)
				CORE_ERR_unpredictable("I2C_IMSG_REG0_WR bad length\n");
			SW(&m3_ctl_reg_imsg0, *((uint32_t *) data));
			break;
		case 0x8a:
			// I2C_IMSG_REG1_WR?
			if (length != 4)
				CORE_ERR_unpredictable("I2C_IMSG_REG1_WR bad length\n");
			SW(&m3_ctl_reg_imsg1, *((uint32_t *) data));
			break;
		case 0x8c:
			// I2C_IMSG_REG2_WR?
			if (length != 4)
				CORE_ERR_unpredictable("I2C_IMSG_REG2_WR bad length\n");
			SW(&m3_ctl_reg_imsg2, *((uint32_t *) data));
			break;
		case 0x8e:
			// I2C_IMSG_REG3_WR?
			if (length != 4)
				CORE_ERR_unpredictable("I2C_IMSG_REG3_WR bad length\n");
			SW(&m3_ctl_reg_imsg3, *((uint32_t *) data));
			break;
		default:
			CORE_ERR_unpredictable("M3 CTL recv i2c bad addr\n");
	}
	CORE_ERR_not_implemented("FIXME: state tracking is a lie here, this is async thread\n");
}

static void m3_ctl_send_i2c_message(uint8_t addr, uint32_t length, char *msg) {
	static int tries = 0;

	//bool acked;
	//acked = i2c_send_message(i2c, addr, length, msg);
	unsigned bytes_sent;
	bytes_sent = ice_i2c_send(ice, addr, msg, length);

	//if (!acked) {
	if (bytes_sent != (length + 1)) {
		tries += 1;
		if (tries > 3) {
			TRAP("I2C Send Max Tries exceeded. Cont to retry\n");
			tries -= 1;
			m3_ctl_send_i2c_message(addr, length, msg);
		} else {
			m3_ctl_send_i2c_message(addr, length, msg);
		}
	}
	tries = 0;
}

////////////////////////////////////////////////////////////////////////////////

static void print_m3_ctl_line(void) {
	;
}

static void i2c_write(uint32_t addr, uint32_t val,
		bool debugger __attribute__ ((unused)) ) {
	assert((addr & 0xfffff000) == 0xA0000000);
	uint8_t length = (addr >> 10) & 0x3;
	if (length == 0)
		length = 4;
	uint8_t address = (addr >> 2) & 0xff;
	assert((addr & 0x3) == 0x0);

	m3_ctl_send_i2c_message(address, length, (char*) &val);
}

static bool cpu_conf_regs_rd(uint32_t addr, uint32_t *val,
		bool debugger __attribute__ ((unused)) ) {
	assert((addr & 0xfffff000) == 0xA0001000);
	switch (addr) {
		case CHIP_ID_REG_RD:
			*val = (uint16_t) SR(&m3_ctl_reg_chip_id);
			return true;
		case DMA_INFO_REG_RD:
			*val = SR(&m3_ctl_reg_dma_info);
			return true;
		case GOC_CTRL_REG_RD:
			*val = SR(&m3_ctl_reg_goc_ctrl) & 0x00ffffff;
			return true;
		case PMU_CTRL_REG_RD:
			*val = SR(&m3_ctl_reg_pmu_ctrl) & 0x1fffffff;
			return true;
		case WUP_CTRL_REG_RD:
			*val = SR(&m3_ctl_reg_wup_ctrl) & 0xffff;
			return true;
		case TSTAMP_REG_RD:
			*val = SR(&m3_ctl_reg_tstamp) & 0xffff;
			return true;
		case MSG_REG0_RD:
			*val = SR(&m3_ctl_reg_msg0);
			return true;
		case MSG_REG1_RD:
			*val = SR(&m3_ctl_reg_msg1);
			return true;
		case MSG_REG2_RD:
			*val = SR(&m3_ctl_reg_msg2);
			return true;
		case MSG_REG3_RD:
			*val = SR(&m3_ctl_reg_msg3);
			return true;
		case INT_MSG_REG0_RD:
			*val = SR(&m3_ctl_reg_imsg0);
			return true;
		case INT_MSG_REG1_RD:
			*val = SR(&m3_ctl_reg_imsg1);
			return true;
		case INT_MSG_REG2_RD:
			*val = SR(&m3_ctl_reg_imsg2);
			return true;
		case INT_MSG_REG3_RD:
			*val = SR(&m3_ctl_reg_imsg3);
			return true;
		default:
			CORE_ERR_unpredictable("Bad CPU Config Reg Read");
	}
}

static void dma_write(uint32_t addr, uint32_t val,
		bool debugger __attribute__ ((unused)) ) {
	assert((addr & 0xFF000000) == 0xA1000000);
	uint8_t target = (addr >> 19) & 0x1f;
	bool write = (addr >> 18) & 0x1;
	assert(0 == ((addr >> 15) & 0x7));
	uint16_t num_words = (addr >> 2) & 0x1fff;
	assert(0 == (addr & 0x3));

	uint16_t target_addr = (val >> 16) & 0xffff;
	uint16_t source_addr = val & 0xffff;

	uint8_t i2c_addr;
	// XXX: Set the X bits for each layer
	INFO("Would write to addres %04x on target layer (silence warn)", target_addr);
	switch (target) {
		case 0x0:
			// DSP CPU: 1001xxx0
			i2c_addr = 0x90;
			break;
		case 0x1:
			// CTL CPU (invalid, cannot send msg to self)
			CORE_ERR_unpredictable("Cannot send I2C to self");
		case 0x2:
			// RF: 1011xxx0
			i2c_addr = 0xb0;
			break;
		case 0x3:
			// IT: 1100xxx0 Imager/Timer (Imager)
			// IT: 1101xxx0 Imager/Timer (Timer)
			CORE_ERR_unpredictable("XXX: Specification ambiguity");
		default:
			if (target & 0x10) {
				// Write arbitrary layer
				i2c_addr = (target & 0xf) << 4;
			} else {
				CORE_ERR_unpredictable("Illegal DMA Write target");
			}
	}

	// The variable sent to us is 'is this a write', but a write means LSB
	// is 0 and a read means LSB is 1, hence the counterintuitive line:
	i2c_addr |= (!write);

	{
		uint32_t buf[num_words];
		int i;
		for (i=0; i<num_words; i++) {
			buf[i] = read_word(source_addr + i*4);
		}

		m3_ctl_send_i2c_message(i2c_addr, num_words*4, (char*) buf);
	}
}

static void cpu_conf_regs_wr(uint32_t addr, uint32_t val,
		bool debugger __attribute__ ((unused)) ) {
	assert((addr & 0xfffffff0) == 0xA2000000);
	switch (addr) {
		case CHIP_ID_REG_WR:
			SW(&m3_ctl_reg_chip_id, (uint16_t) val);
			break;
		case DMA_INFO_REG_WR:
			SW(&m3_ctl_reg_dma_info, val);
			break;
		case GOC_CTRL_REG_WR:
			SW(&m3_ctl_reg_goc_ctrl, val & 0x00ffffff);
			break;
		case PMU_CTRL_REG_WR:
			SW(&m3_ctl_reg_pmu_ctrl, val & 0x1fffffff);
			break;
		case WUP_CTRL_REG_WR:
			SW(&m3_ctl_reg_wup_ctrl, val & 0xffff);
			break;
		case TSTAMP_REG_WR:
			SW(&m3_ctl_reg_tstamp, val & 0xffff);
			break;
		default:
			CORE_ERR_unpredictable("Bad CPU Config Reg Write");
	}
}

static void pmu_special_wr(uint32_t addr, uint32_t val,
		bool debugger __attribute__ ((unused)) ) {
	assert((addr & 0xfffffff0) & 0xA3000000);
	switch (val) {
		case 0x0:
			CORE_ERR_not_implemented("PMU DIV5 mode override");
		case 0x1:
			CORE_ERR_not_implemented("PMU power-on-reset");
		case 0x2:
			CORE_ERR_not_implemented("Ctrl CPU layer sleep");
		default:
			CORE_ERR_unpredictable("Bad CTRL PMU Special");
	}
}

static bool gpio_read(uint32_t addr, uint32_t *val,
		bool debugger __attribute__ ((unused)) ) {
	switch (addr) {
		case 0xA4000000:
			*val = gpio_data_I;
			break;
		case 0xA4001000:
			*val = gpio_dir;
			break;
		case 0xA4001040:
			*val = gpio_int_mask;
			break;
		default:
			CORE_ERR_invalid_addr(false, addr);
	}
	return true;
}

static void gpio_demux(uint32_t old, uint32_t new,
		void (*fn)(struct ice_instance*, uint8_t, bool)) {
	int i;
	for (i=0; i<24 /*XXX: max gpios, make a constant?*/; i++) {
		uint32_t j = (1 << i);
		if ((old & j) != (new & j)) {
			fn(ice, i, new & j);
		}
	}
}

static void gpio_write(uint32_t addr, uint32_t val,
		bool debugger __attribute__ ((unused)) ) {
	switch (addr) {
		case 0xA4000000:
			gpio_demux(gpio_data_O, val, ice_gpio_out);
			gpio_data_O = val;
			break;
		case 0xA4001000:
			gpio_demux(gpio_dir, val, ice_gpio_dir);
			gpio_dir = val;
			break;
		case 0xA4001040:
			gpio_demux(gpio_int_mask, val, ice_gpio_int);
			gpio_int_mask = val;
			break;
		default:
			CORE_ERR_invalid_addr(true, addr);
	}
}

__attribute__ ((constructor))
void register_periph_m3_ctl(void) {
	union memmap_fn mem_fn;

	mem_fn.W_fn32 = i2c_write;
	register_memmap("M3 CTL I2C WR", true, 4, mem_fn, I2C_BOT_WR, I2C_TOP_WR);

	mem_fn.R_fn32 = cpu_conf_regs_rd;
	register_memmap("M3 CTL CONF RD", false, 4, mem_fn, MSG_REG0_RD, TSTAMP_REG_RD+4);

	mem_fn.W_fn32 = dma_write;
	register_memmap("M3 CTL DMA WR", true, 4, mem_fn, DMA_BOT_WR, DMA_TOP_WR);

	mem_fn.W_fn32 = cpu_conf_regs_wr;
	register_memmap("M3 CTL CONF WR", true, 4, mem_fn, CHIP_ID_REG_WR, TSTAMP_REG_WR+4);

	mem_fn.W_fn32 = pmu_special_wr;
	register_memmap("M3 CTL PMU SPECIAL", true, 1, mem_fn, PMU_SPECIAL, PMU_SPECIAL+1);

	mem_fn.R_fn32 = gpio_read;
	register_memmap("M3 CTL GPIO RD", false, 4, mem_fn, GPIO_BOT, GPIO_TOP);
	mem_fn.W_fn32 = gpio_write;
	register_memmap("M3 CTL GPIO WR",  true, 4, mem_fn, GPIO_BOT, GPIO_TOP);

	register_periph_printer(print_m3_ctl_line);

	{
		// Connect to software I2C Bus
		char *host;
		const uint16_t port = 21010; // Hardcoded for now (2C!)
		if (-1 == asprintf(&host, "/tmp/%s.M-ulator.bus", getlogin()))
			ERR(E_UNKNOWN, "Error allocating port path: %s\n", strerror(errno));

		// m3_ctl responds to 10x0xxxx
		i2c = create_i2c_instance("m3_ctl",
				NULL,
				0x80, 0x50,
				host, port);

		free(host);
	}

	{
		// Connect to ICE Hardware Board
		const char *host = "/tmp/m3_ice_com1";
		const uint16_t baud = 0; // use default baud

		ice = create_ice_instance(host, baud);
	}
}
