#include "cortex_m3.h"

/* void reset(void)
 *
 * This function is called when the reset pin is triggered on the processor.
 * Recall that most M3s are Power-On Reset, so this is the first function that
 * will be called when the chip turns on
 */
void reset(void) {
	CORE_reg_write(SP_REG, read_word(0x00000000));
	CORE_reg_write(LR_REG, 0xFFFFFFFF);
	CORE_reg_write(PC_REG, read_word(0x00000004));
}

/* uint32_t read_word(uint32_t addr)
 *
 * A wrapper function to read a memory address.  This function is responsible
 * for "doing the right thing". That is, if this address is in RAM it should
 * be read from RAM, if it's a peripheral then that should work correctly as
 * well. Consult memmap.txt for details and the CORE_ERR_...() functions may
 * be useful as well
 */
uint32_t read_word(uint32_t addr) {
	if (addr >= ROMBOT && addr < ROMTOP) {
		return CORE_rom_read(addr);
	}
	else if (addr >= RAMBOT && addr < RAMTOP) {
		return CORE_ram_read(addr);
	}
	else if (addr == REDLED) {
		return CORE_red_led_read();
	}
	else if (addr == GRNLED) {
		return CORE_grn_led_read();
	}
	else if (addr == BLULED) {
		return CORE_blu_led_read();
	}
	else if (addr == POLL_UART_STATUS) {
		DBG2("Attempt to read UART STATUS\n");
		return CORE_poll_uart_status_read();
	}
	else if (addr == POLL_UART_RXDATA) {
		DBG1("Attempt to read UART RXDATA\n");
		return CORE_poll_uart_rxdata_read();
	}
	else if (addr == POLL_UART_TXDATA) {
		DBG1("Attempt to read UART TXDATA\n");
		CORE_ERR_invalid_addr(false, addr);
	}
	else {
		CORE_ERR_invalid_addr(false, addr);
	}
}

/* void write_word(uint32_t addr)
 *
 * Like read_word, only for writes
 */
void write_word(uint32_t addr, uint32_t val) {
	if (addr >= ROMBOT && addr < ROMTOP) {
		DBG1("Attempt to write to ROM\n");
		CORE_ERR_invalid_addr(true, addr);
	}
	else if (addr >= RAMBOT && addr < RAMTOP) {
		CORE_ram_write(addr, val);
	}
	else if (addr == REDLED) {
		CORE_red_led_write(val);
	}
	else if (addr == GRNLED) {
		CORE_grn_led_write(val);
	}
	else if (addr == BLULED) {
		CORE_blu_led_write(val);
	}
	else if (addr == POLL_UART_STATUS) {
		CORE_poll_uart_status_write(val);
	}
	else if (addr == POLL_UART_RXDATA) {
		DBG1("Attempt to write UART RXDATA\n");
		CORE_ERR_invalid_addr(true, addr);
	}
	else if (addr == POLL_UART_TXDATA) {
		CORE_poll_uart_txdata_write(val);
	}
	else {
		CORE_ERR_invalid_addr(true, addr);
	}
}

uint16_t read_halfword(uint32_t addr) {
	uint32_t word = read_word(addr & 0xfffffffc);

	if (addr & 0x1)
		CORE_ERR_invalid_addr(false, addr);

	uint16_t ret;

	switch (addr & 0x2) {
		case 0x0:
			ret = (word & 0x0000ffff) >> 0;
			break;
		case 0x2:
			ret = (word & 0xffff0000) >> 16;
			break;
		default:
			// appease compiler
			assert(false); return 0;
	}

	DBG2("read_halfword: returning %d\t0x%04x\n", ret, ret);

	return ret;
}

void write_halfword(uint32_t addr, uint16_t val) {
	CORE_ERR_not_implemented("write_halfword");
}

uint8_t read_byte(uint32_t addr) {
	uint32_t word = read_word(addr & 0xfffffffc);

	DBG2("read_byte: addr %08x, val %08x, case %d\n", addr, word, addr & 0x3);

	uint8_t ret;

	switch (addr & 0x3) {
		case 0:
			ret = (word & 0x000000ff) >> 0;
			break;
		case 1:
			ret = (word & 0x0000ff00) >> 8;
			break;
		case 2:
			ret = (word & 0x00ff0000) >> 16;
			break;
		case 3:
			ret = (word & 0xff000000) >> 24;
			break;
		default:
			//appease dumb compiler
			assert(false); return 0;
	}

	DBG2("read_byte: returning %c\t%02x\n", ret, ret);

	return ret;
}

void write_byte(uint32_t addr, uint8_t val) {
	if (addr == POLL_UART_STATUS)
		return CORE_poll_uart_status_write(val);
	if (addr == POLL_UART_TXDATA)
		return CORE_poll_uart_txdata_write(val);

	uint32_t word = read_word(addr & 0xfffffffc);
	uint32_t val32 = val;

	switch (addr & 0x3) {
		case 0:
			word &= 0xffffff00;
			word |= val32;
			break;
		case 1:
			word &= 0xffff00ff;
			word |= (val32 << 8);
			break;
		case 2:
			word &= 0xff00ffff;
			word |= (val32 << 16);
			break;
		case 3:
			word &= 0x00ffffff;
			word |= (val32 << 24);
			break;
	}

	write_word(addr & 0xfffffffc, word);
}
