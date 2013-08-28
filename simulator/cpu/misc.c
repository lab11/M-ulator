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

#include "misc.h"

#include "registers.h"

/* Boolean answering whether the itstate indicates the processor is
 * currently in an IT Block or not
 */
uint8_t in_ITblock(void) {
	return (read_itstate() & 0xf) != 0;
}

/* Boolean answering whether the current instruction is the last in
 * an IT block or not
 */
uint8_t last_in_ITblock(void) {
	return ((read_itstate() & 0xf) == 0x8);
}

/* Utility function to advance from the current itstate to the next
 * (should end with a call to write_itstate
 */
void IT_advance(void) {
	uint32_t itstate = read_itstate();

	if ((itstate & 0x7) == 0) {
		write_itstate(0);
		DBG2("Left itstate\n");
	} else {
		uint8_t temp = itstate & 0x0f; // yes 0f, not 1f
		uint8_t itstate_t = itstate;
		itstate_t &= 0xe0;
		itstate_t |= (temp << 1);
		write_itstate(itstate_t);
		DBG2("New itstate: %02x\n", itstate_t);
	}
}

uint8_t read_itstate(void) {
#ifdef M_PROFILE
	union epsr_t epsr = CORE_epsr_read();
	return (epsr.bits.ICI_IT_top << 2) | epsr.bits.ICI_IT_bot;
#endif // M_PROFILE
}

/* Manipulating the IT bits is a little annoying, utility function
 * to write them for you
 */
void write_itstate(uint8_t new_state) {
#ifdef M_PROFILE
	union epsr_t epsr = CORE_epsr_read();
	epsr.bits.ICI_IT_top = new_state >> 2;
	epsr.bits.ICI_IT_bot = new_state & 0x3f;
	CORE_epsr_write(epsr);
#else
#error Need to implement itstate for non-M Profiles
#endif // M_PROFILE
}

uint8_t eval_cond(union apsr_t apsr, uint8_t cond) {
	bool n = apsr.bits.N;
	bool z = apsr.bits.Z;
	bool c = apsr.bits.C;
	bool v = apsr.bits.V;

	uint8_t ret;

	DBG2("eval_cond: cond %02x n %d z %d c %d v %d\n", cond, n, z, c, v);

	switch (cond) {
		case 0:
		case 1:
			ret = z == 1;
			break;
		case 2:
		case 3:
			ret = c == 1;
			break;
		case 4:
		case 5:
			ret = n == 1;
			break;
		case 6:
		case 7:
			ret = v == 1;
			break;
		case 8:
		case 9:
			ret = (c == 1) && (z == 0);
			break;
		case 10:
		case 11:
			ret = (n == v);
			break;
		case 12:
		case 13:
			ret = (n == v) & (z == 0);
			break;
		case 14:
		case 15:
			ret = true;
			break;
		default:
			CORE_ERR_unpredictable("Bad condition in eval_cond\n");
	}

	if ((cond & 0x1) && (cond != 0xf))
		ret = !ret;

	return ret;
}

