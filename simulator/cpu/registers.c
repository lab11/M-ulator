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

#include "registers.h"

#include "core/state_sync.h"
#include "core/pipeline.h"

#include "cpu/core.h"
#include "cpu/misc.h"
#include "cpu/common/private_peripheral_bus/ppb.h"

/* Ignore mode transitions for now */
static uint32_t reg[SP_REG];	// SP,LR,PC not held here, so 13 registers
static uint32_t sp_process;
static uint32_t sp_main;
static uint32_t *sp = &sp_main;
static uint32_t lr;

#define SP	(*sp)
#define LR	(lr)
#ifdef NO_PIPELINE
#define PC	(pre_if_PC)
#else
#define PC	(id_ex_PC)
#endif

#ifdef M_PROFILE

static union apsr_t apsr;
static union ipsr_t ipsr;
static union epsr_t epsr;

static uint32_t primask;
//     0: priority	The exception mask register, a 1-bit register.
//			Setting PRIMASK to 1 raises the execution priority to 0.
static uint32_t basepri;
/* The base priority mask, an 8-bit register. BASEPRI changes the priority
 * level required for exception preemption. It has an effect only when BASEPRI
 * has a lower value than the unmasked priority level of the currently
 * executing software.  The number of implemented bits in BASEPRI is the same
 * as the number of implemented bits in each field of the priority registers,
 * and BASEPRI has the same format as those fields.  For more information see
 * Maximum supported priority value on page B1-636.  A value of zero disables
 * masking by BASEPRI.
 */
static uint32_t faultmask;
/* The fault mask, a 1-bit register. Setting FAULTMASK to 1 raises the
 * execution priority to -1, the priority of HardFault. Only privileged
 * software executing at a priority below -1 can set FAULTMASK to 1. This means
 * HardFault and NMI handlers cannot set FAULTMASK to 1. Returning from any
 * exception except NMI clears FAULTMASK to 0.
 */

static union control_t control;
//     0: nPRIV, thread mode only (0 == privileged, 1 == unprivileged)
//     1: SPSEL, thread mode only (0 == use SP_main, 1 == use SP_process)
//     2: FPCA, (1 if FP extension active)

#endif // M


////////////////////////////////////////////////////////////////////////////////

/* ARMv7-M implementations treat SP bits [1:0] as RAZ/WI.
 * ARM strongly recommends that software treats SP bits [1:0]
 * as SBZP for maximum portability across ARMv7 profiles.
 */
EXPORT uint32_t CORE_reg_read(int r) {
	assert(r >= 0 && r < 16 && "CORE_reg_read");
	if (r == SP_REG) {
		return SR(&SP) & 0xfffffffc;
	} else if (r == LR_REG) {
		return SR(&LR);
	} else if (r == PC_REG) {
#ifdef NO_PIPELINE
		return SR(&id_ex_PC) & 0xfffffffe;
#else
		return SR(&PC) & 0xfffffffe;
#endif
	} else {
		return SR(&reg[r]);
	}
}

EXPORT void CORE_reg_write(int r, uint32_t val) {
	assert(r >= 0 && r < 16 && "CORE_reg_write");
	if (r == SP_REG) {
		SW(&SP, val & 0xfffffffc);
	} else if (r == LR_REG) {
		SW(&LR, val);
	} else if (r == PC_REG) {
#ifdef NO_PIPELINE
		/*
		if (*state_flags_cur & STATE_DEBUGGING) {
			SW(&pre_if_PC, val & 0xfffffffe);
			SW(&if_id_PC, val & 0xfffffffe);
			SW(&id_ex_PC, val & 0xfffffffe);
		} else {
		*/
			SW(&pre_if_PC, val & 0xfffffffe);
			ERR(E_UNKNOWN, "FIXME: Need to flush still now\n");
		//}
#else
		if (state_is_debugging()) {
			DBG2("PC write + debugging --> flush\n");
			state_pipeline_flush(val & 0xfffffffe);
		} else {
			// Only flush if the new PC differs from predicted in pipeline:
			if (((SR(&if_id_PC) & 0xfffffffe) - 4) == (val & 0xfffffffe)) {
				DBG2("Predicted PC correctly (%08x)\n", val);
			} else {
				state_pipeline_flush(val & 0xfffffffe);
				DBG2("Predicted PC incorrectly\n");
				DBG2("Pred: %08x, val: %08x\n", SR(&if_id_PC), val);
			}
		}
#endif
	}
	else {
		SW(&(reg[r]), val);
	}
}

EXPORT union apsr_t CORE_apsr_read(void) {
	union apsr_t a;
	a.storage = SR(&apsr.storage);
	return a;
}

EXPORT void CORE_apsr_write(union apsr_t val) {
	if (in_ITblock()) {
		WARN("WARN update of apsr in IT block\n");
	}
#ifdef M_PROFILE
	if (val.storage & 0x07f0ffff) {
		DBG1("WARN update of reserved APSR bits\n");
	}
#endif
	SW(&apsr.storage, val.storage);
}

#ifdef M_PROFILE
EXPORT union ipsr_t CORE_ipsr_read(void) {
	union ipsr_t i;
	i.storage = SR(&ipsr.storage);
	return i;
}

EXPORT void CORE_ipsr_write(union ipsr_t val) {
	SW(&ipsr.storage, val.storage);
}

EXPORT union epsr_t CORE_epsr_read(void) {
	union epsr_t e;
	e.storage = SR(&epsr.storage);
	return e;
}

EXPORT void CORE_epsr_write(union epsr_t val) {
	SW(&epsr.storage, val.storage);
}

EXPORT bool CORE_primask_read(void) {
	return SR(&primask);
}

EXPORT void CORE_primask_write(bool val) {
	SW(&primask, val);
}

EXPORT bool CORE_faultmask_read(void) {
	return SR(&faultmask);
}

EXPORT void CORE_faultmask_write(bool val) {
	SW(&faultmask, val);
}

EXPORT uint8_t CORE_basepri_read(void) {
	return SR(&basepri);
}

EXPORT void CORE_basepri_write(uint8_t val) {
	SW(&basepri, val);
}

EXPORT union control_t CORE_control_read(void) {
	union control_t c;
	c.storage = SR(&control.storage);
	return c;
}

EXPORT void CORE_control_write(union control_t val) {
	SW(&control.storage, val.storage);
}
#endif

static void reset_registers(void) {
	DBG2("begin\n");
	uint32_t vectortable = read_word(VECTOR_TABLE_OFFSET);

	// R[0..12] = bits(32) UNKNOWN {nop}

	SW(&sp_main,read_word(vectortable) & 0xfffffffc);

	// sp_process = ((bits(30) UNKNOWN):'00')
	SW(&sp_process, SR(&sp_process) & ~0x3);

	CORE_reg_write(LR_REG, 0xFFFFFFFF);

	uint32_t tmp = read_word(vectortable+4);
	bool tbit = tmp & 0x1;
	CORE_reg_write(PC_REG, tmp & 0xfffffffe);
	if (!tbit) {
		WARN("Reset vector %08x at %08x invalid\n", tmp, vectortable+4);
		CORE_ERR_unpredictable("Thumb bit must be set for M-series\n");
	}

	// ASPR = bits(32) UNKNOWN {nop}

	union ipsr_t ipsr = CORE_ipsr_read();
	ipsr.bits.exception = 0;
	CORE_ipsr_write(ipsr);

	union epsr_t epsr = CORE_epsr_read();
	epsr.bits.T = tbit;
	epsr.bits.ICI_IT_top = 0;
	epsr.bits.ICI_IT_bot = 0;
	CORE_epsr_write(epsr);

	///

	// B1.4.3: The special-purpose mask registers
	SW(&primask, 0);
	SW(&faultmask, 0);
	SW(&basepri, 0);
	SW(&control.storage, 0);
	DBG2("end\n");
}

__attribute__ ((constructor))
static void register_reset_registers(void) {
	assert(sizeof(union apsr_t) == 4);
	{
		union apsr_t a;
		a.storage = 0xf8000000;
		assert(a.bits.N == 1);
		assert(a.bits.Z);
		assert(a.bits.C);
		assert(a.bits.V);
		assert(a.bits.Q);
	}
	assert(sizeof(union ipsr_t) == 4);
	{
		union ipsr_t i;
		i.storage = 0xa5;
		assert(i.bits.exception = 0xa5);
	}
	assert(sizeof(union epsr_t) == 4);

	register_reset(reset_registers);
}
