#include "cortex_m3.h"

/* Boolean answering whether the itstate indicates the processor is
 * currently in an IT Block or not
 */
uint8_t in_ITblock(uint8_t itstate) {
//#define	IN_IT_BLOCK()		((ITSTATE & 0xf) != 0)
	return (itstate & 0xf) != 0;
}

/* Boolean answering whether the current instruction is the last in
 * an IT block or not
 */
uint8_t last_in_ITblock(uint8_t itstate) {
//#define LAST_IN_IT_BLOCK()	((ITSTATE & 0xf) == 0x8)
	return ((itstate & 0xf) == 0x8);
}

/* Utility function to advance from the current itstate to the next
 * (should end with a call to write_itstate
 */
void IT_advance(uint8_t itstate) {
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

/* Manipulating the IT bits is a little annoying, utility function
 * to write them for you
 */
void write_itstate(uint8_t new_state) {
	uint32_t new = new_state;
	uint32_t it_clear_mask = 0xf9ff03ff;
	uint32_t result = 0;

	result |= ((new & 0x3) << 25);
	result |= ((new & 0x3f) << 8);

	uint32_t cpsr = CORE_cpsr_read();
	cpsr = (((cpsr) & it_clear_mask) | result);
	DBG1("write_itstate is writing cpsr\n");
	CORE_cpsr_write(cpsr);
}

uint8_t eval_cond(uint32_t cpsr, uint8_t cond) {
	uint8_t n = !!(cpsr & xPSR_N);
	uint8_t z = !!(cpsr & xPSR_Z);
	uint8_t c = !!(cpsr & xPSR_C);
	uint8_t v = !!(cpsr & xPSR_V);

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
			assert(false); return 0;
	}

	if ((cond & 0x1) && (cond != 0xf))
		ret = !ret;

	return ret;
}


uint8_t Feval_cond(uint32_t cpsr, uint8_t cond) {
	switch (cond) {
		case 0:
			//EQ
			return !!(cpsr & xPSR_Z);
		case 1:
			//NE
			return !(cpsr & xPSR_Z);
		case 2:
			//CS/HS
			return !!(cpsr & xPSR_C);
		case 3:
			//CC/LO
			return !(cpsr & xPSR_C);
		case 4:
			//MI
			return !!(cpsr & xPSR_N);
		case 5:
			//PL
			return !(cpsr & xPSR_N);
		case 6:
			//VS
			return !!(cpsr & xPSR_V);
		case 7:
			//VC
			return !(cpsr & xPSR_V);
		case 8:
			//HI
			return !!(cpsr & xPSR_C) & !(cpsr & xPSR_Z);
		case 9:
			//LS
			return !(cpsr & xPSR_C) & !!(cpsr & xPSR_Z);
		case 10:
			//GE
			return (
				(!!(cpsr & xPSR_N) & !!(cpsr & xPSR_V)) ||
				(!(cpsr & xPSR_N) & !(cpsr & xPSR_V))
			       );
		case 11:
			//LT
			return (
				(!!(cpsr & xPSR_N) & !(cpsr & xPSR_V)) ||
				(!(cpsr & xPSR_N) & !!(cpsr & xPSR_V))
			       );
		case 12:
			//GT
			return (
				(!(cpsr & xPSR_Z)) &
				(
					(!!(cpsr & xPSR_N) & !!(cpsr & xPSR_V)) |
					(!(cpsr & xPSR_N) & !(cpsr & xPSR_V))
				)
			       );
		case 13:
			//LE
			return (
				(!!(cpsr & xPSR_Z)) |
				(!!(cpsr & xPSR_N) & !(cpsr & xPSR_V))  |
				(!(cpsr & xPSR_N) & !!(cpsr & xPSR_V))
			       );
		case 14:
			//AL
			return true;
		case 15:
		default:
			CORE_ERR_unpredictable("Illegal cond\n");
	}
}

