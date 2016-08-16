/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2015  Pat Pannuto <pat.pannuto@gmail.com>
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

#include "m3_prc.h"

#include "memmap.h"

#include "cpu/periph.h"
#include "cpu/core.h"

#include "core/state_sync.h"


#define MASK(n) ((1ULL<<(n))-1)

// MBUS MMIO REG'S
static uint32_t mbus_mmio_addr;
static uint32_t mbus_mmio_data;

// CPU CONF REG'S
static uint32_t m3_prc_reg_chip_id;
static uint32_t m3_prc_reg_mbus_thres;
static uint32_t m3_prc_reg_goc_ctrl;
static uint32_t m3_prc_reg_pmu_ctrl;
static uint32_t m3_prc_reg_wup_ctrl;
static uint32_t m3_prc_reg_tstamp;

static uint32_t m3_prc_reg_msg0;
static uint32_t m3_prc_reg_msg1;
static uint32_t m3_prc_reg_msg2;
static uint32_t m3_prc_reg_msg3;
static uint32_t m3_prc_reg_imsg0;
static uint32_t m3_prc_reg_imsg1;
static uint32_t m3_prc_reg_imsg2;
static uint32_t m3_prc_reg_imsg3;

/*
// GPIO REG'S
static uint32_t gpio_dir;	// 0-input, 1-output
static uint32_t gpio_int_mask;	// INT if dir == 0 and any edge detected. IRQ4
static uint32_t gpio_data_I;
static uint32_t gpio_data_O;
*/

static void m3_prc_reset(void) {
	mbus_mmio_addr = 0;
	mbus_mmio_data = 0;

	m3_prc_reg_chip_id = CHIP_ID_REG_RESET;
	m3_prc_reg_mbus_thres = MBUS_THRES_REG_RESET;
	m3_prc_reg_goc_ctrl = GOC_CTRL_REG_RESET;
	m3_prc_reg_pmu_ctrl = PMU_CTRL_REG_RESET;
	m3_prc_reg_wup_ctrl = WUP_CTRL_REG_RESET;
	m3_prc_reg_tstamp = TSTAMP_REG_RESET;

	m3_prc_reg_msg0 = 0;
	m3_prc_reg_msg1 = 0;
	m3_prc_reg_msg2 = 0;
	m3_prc_reg_msg3 = 0;
	m3_prc_reg_imsg0 = 0;
	m3_prc_reg_imsg1 = 0;
	m3_prc_reg_imsg2 = 0;
	m3_prc_reg_imsg3 = 0;
}

__attribute__ ((constructor))
void register_reset_m3_prc(void) {
	register_reset(m3_prc_reset);
}

////////////////////////////////////////////////////////////////////////////////

static void print_m3_prc_line(void) {
	printf("PRCv9 Conf: "); //12

	printf("C %08x M %08x G %08x P %08x W %08x T %08x", //48+5+12
			m3_prc_reg_chip_id,
			m3_prc_reg_mbus_thres,
			m3_prc_reg_goc_ctrl,
			m3_prc_reg_pmu_ctrl,
			m3_prc_reg_wup_ctrl,
			m3_prc_reg_tstamp
			);

	printf("\n");
}

/*
static bool mbus_mmio_rd(uint32_t addr, uint32_t *val,
bool debugger __attribute__ ((unused)) ) {
	printf("%d -> %p\n", addr, val);
	CORE_ERR_not_implemented("mbus mmio rd");
}
*/

static void mbus_mmio_wr(uint32_t addr, uint32_t val,
		bool debugger __attribute__ ((unused)) ) {
	switch (addr) {
		case MBUS_MMIO_ADDR:
			SW(&mbus_mmio_addr, val);
			break;
		case MBUS_MMIO_DATA:
			SW(&mbus_mmio_data, val);
			INFO("MBus message: addr %08x, data %08x\n",
					SR(&mbus_mmio_addr),
					SR(&mbus_mmio_data)
			    );
			break;
		default:
			WARN("Unknown address, %08x\n", addr);
			CORE_ERR_unpredictable("Bad MBus MMIO address");
	}
	fflush(stdout);
}

static bool cpu_conf_regs_rd(uint32_t addr, uint32_t *val,
		bool debugger __attribute__ ((unused)) ) {
	assert((addr & 0xfffff000) == 0xA0001000);
	switch (addr) {
		case CHIP_ID_REG_RD:
			*val = SR(&m3_prc_reg_chip_id) & MASK(CHIP_ID_REG_NBITS);
			return true;
		case GOC_CTRL_REG_RD:
			*val = SR(&m3_prc_reg_goc_ctrl) & MASK(GOC_CTRL_REG_NBITS);
			return true;
		case PMU_CTRL_REG_RD:
			*val = SR(&m3_prc_reg_pmu_ctrl) & MASK(PMU_CTRL_REG_NBITS);
			return true;
		case WUP_CTRL_REG_RD:
			*val = SR(&m3_prc_reg_wup_ctrl) & MASK(WUP_CTRL_REG_NBITS);
			return true;
		case TSTAMP_REG_RD:
			*val = SR(&m3_prc_reg_tstamp) & MASK(TSTAMP_REG_NBITS);
			return true;
		case MSG_REG0_RD:
			*val = SR(&m3_prc_reg_msg0);
			return true;
		case MSG_REG1_RD:
			*val = SR(&m3_prc_reg_msg1);
			return true;
		case MSG_REG2_RD:
			*val = SR(&m3_prc_reg_msg2);
			return true;
		case MSG_REG3_RD:
			*val = SR(&m3_prc_reg_msg3);
			return true;
		case INT_MSG_REG0_RD:
			*val = SR(&m3_prc_reg_imsg0);
			return true;
		case INT_MSG_REG1_RD:
			*val = SR(&m3_prc_reg_imsg1);
			return true;
		case INT_MSG_REG2_RD:
			*val = SR(&m3_prc_reg_imsg2);
			return true;
		case INT_MSG_REG3_RD:
			*val = SR(&m3_prc_reg_imsg3);
			return true;
		default:
			CORE_ERR_unpredictable("Bad CPU Config Reg Read");
	}
}

static void cpu_conf_regs_wr(uint32_t addr, uint32_t val,
		bool debugger __attribute__ ((unused)) ) {
	switch (addr) {
		case CHIP_ID_REG_WR:
			SW(&m3_prc_reg_chip_id, val & MASK(CHIP_ID_REG_NBITS));
			break;
		case GOC_CTRL_REG_WR:
			SW(&m3_prc_reg_goc_ctrl, val & MASK(GOC_CTRL_REG_NBITS));
			break;
		case PMU_CTRL_REG_WR:
			SW(&m3_prc_reg_pmu_ctrl, val & MASK(PMU_CTRL_REG_NBITS));
			break;
		case WUP_CTRL_REG_WR:
			SW(&m3_prc_reg_wup_ctrl, val & MASK(WUP_CTRL_REG_NBITS));
			break;
		case TSTAMP_REG_WR:
			SW(&m3_prc_reg_tstamp, val & MASK(TSTAMP_REG_NBITS));
			break;
		default:
			CORE_ERR_unpredictable("Bad CPU Config Reg Write");
	}
}

static void pmu_reset_wr(uint32_t addr, uint32_t val,
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
			WARN("Write %08x to %08x\n", val, addr);
			CORE_ERR_unpredictable("Bad CTRL PMU Special");
	}
}

/*
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
	// XXX: max gpios, make a constant?
	for (i=0; i<24 ; i++) {
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
*/

__attribute__ ((constructor))
void register_periph_m3_prc(void) {
	union memmap_fn mem_fn;

	/*
	mem_fn.R_fn32 = mbus_mmio_rd;
	register_memmap("M3 MBUS MMIO RD", false, 4, mem_fn, MBUS_MMIO_BOT, MBUS_MMIO_TOP+4);
	*/

	mem_fn.W_fn32 = mbus_mmio_wr;
	register_memmap("M3 MBUS MMIO WR", true, 4, mem_fn, MBUS_MMIO_ADDR, MBUS_MMIO_DATA+4);

	mem_fn.R_fn32 = cpu_conf_regs_rd;
	register_memmap("M3 CTL CONF RD", false, 4, mem_fn, MSG_REG0_RD, TSTAMP_REG_RD+4);

	mem_fn.W_fn32 = cpu_conf_regs_wr;
	register_memmap("M3 CTL CONF WR", true, 4, mem_fn, CHIP_ID_REG_WR, TSTAMP_REG_WR+4);

	mem_fn.W_fn32 = pmu_reset_wr;
	register_memmap("M3 CTL PMU RESET", true, 1, mem_fn, PMU_RST_REG_WR, PMU_RST_REG_WR+1);

	/*
	mem_fn.R_fn32 = gpio_read;
	register_memmap("M3 CTL GPIO RD", false, 4, mem_fn, GPIO_BOT, GPIO_TOP);
	mem_fn.W_fn32 = gpio_write;
	register_memmap("M3 CTL GPIO WR",  true, 4, mem_fn, GPIO_BOT, GPIO_TOP);
	*/

	register_periph_printer(print_m3_prc_line);

	/*
	{
		// Connect to ICE Hardware Board
		const char *host = "/tmp/m3_ice_com1";
		const uint16_t baud = 0; // use default baud

		ice = create_ice_instance(host, baud);
	}
	*/
}
