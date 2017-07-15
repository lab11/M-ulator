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

#include MEMMAP_HEADER

#include "registers.h"

#include "core/state_sync.h"
#include "core/pipeline.h"

#include "cpu/core.h"
#include "cpu/features.h"
#include "cpu/misc.h"
#include "cpu/common/private_peripheral_bus/ppb.h"

enum Mode CurrentMode;
EXPORT uint32_t physical_reg[SP_REG];	// SP,LR,PC not held here, so 13 registers
       uint32_t sp_process; // "private" export
       uint32_t sp_main; // "private" export
EXPORT uint32_t *physical_sp_p = &sp_main;
EXPORT uint32_t physical_lr;

#ifdef M_PROFILE

EXPORT union apsr_t physical_apsr;
EXPORT union ipsr_t physical_ipsr;
EXPORT union epsr_t physical_epsr;

union ufsr_t ufsr;

EXPORT uint32_t physical_primask;
//     0: priority	The exception mask register, a 1-bit register.
//			Setting PRIMASK to 1 raises the execution priority to 0.
EXPORT uint32_t physical_basepri;
/* The base priority mask, an 8-bit register. BASEPRI changes the priority
 * level required for exception preemption. It has an effect only when BASEPRI
 * has a lower value than the unmasked priority level of the currently
 * executing software.  The number of implemented bits in BASEPRI is the same
 * as the number of implemented bits in each field of the priority registers,
 * and BASEPRI has the same format as those fields.  For more information see
 * Maximum supported priority value on page B1-636.  A value of zero disables
 * masking by BASEPRI.
 */
EXPORT uint32_t physical_faultmask;
/* The fault mask, a 1-bit register. Setting FAULTMASK to 1 raises the
 * execution priority to -1, the priority of HardFault. Only privileged
 * software executing at a priority below -1 can set FAULTMASK to 1. This means
 * HardFault and NMI handlers cannot set FAULTMASK to 1. Returning from any
 * exception except NMI clears FAULTMASK to 0.
 */

EXPORT union control_t physical_control;
//     0: nPRIV, thread mode only (0 == privileged, 1 == unprivileged)
//     1: SPSEL, thread mode only (0 == use SP_main, 1 == use SP_process)
//     2: FPCA, (1 if FP extension active)

#endif // M


////////////////////////////////////////////////////////////////////////////////

EXPORT uint32_t CORE_reg_read(int r)
{
	extern uint32_t physical_reg[SP_REG];
	extern uint32_t *physical_sp_p;
	extern uint32_t physical_lr;
	extern uint32_t id_ex_PC;

	assert(r >= 0 && r < 16 && "CORE_reg_read");
	if (r == SP_REG) {
		return SR(physical_sp_p) & 0xfffffffc;
	} else if (r == LR_REG) {
		return SR(&physical_lr);
	} else if (r == PC_REG) {
		return SR(&id_ex_PC) & 0xfffffffe;
	} else {
		return SR(&physical_reg[r]);
	}
}

EXPORT void CORE_reg_write(int r, uint32_t val) {
	assert(r >= 0 && r < 16 && "CORE_reg_write");
	if (r == SP_REG) {
		SW(physical_sp_p, val & 0xfffffffc);
	} else if (r == LR_REG) {
		SW(&physical_lr, val);
	} else if (r == PC_REG) {
		DBG2("Writing %08x to PC\n", val & 0xfffffffe);
#ifdef NO_PIPELINE
		pipeline_flush_exception_handler(val & 0xfffffffe);
#else
		if (state_is_debugging()) {
			DBG1("PC write + debugging --> flush\n");
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
		SW(&(physical_reg[r]), val);
	}
}

#ifdef M_PROFILE
// <8:0> from IPSR
// <26:24,15:10> from ESPR
// <31:27>,[if DSP: <19:16>] from APSR
static const uint32_t xPSR_ipsr_mask = 0x000001ff;
static const uint32_t xPSR_epsr_mask = 0x0700fc00;
static const uint32_t xPSR_apsr_dsp_mask = 0xf80f0000;
static const uint32_t xPSR_apsr_nodsp_mask = 0xf8000000;

EXPORT uint32_t CORE_xPSR_read(void) {
	uint32_t xPSR = 0;
	xPSR |= CORE_ipsr_read().storage & xPSR_ipsr_mask;
	xPSR |= CORE_epsr_read().storage & xPSR_epsr_mask;
	if (HaveDSPExt())
		xPSR |= CORE_apsr_read().storage & xPSR_apsr_dsp_mask;
	else
		xPSR |= CORE_apsr_read().storage & xPSR_apsr_nodsp_mask;
	return xPSR;
}

EXPORT void CORE_xPSR_write(uint32_t xPSR) {
	union ipsr_t i = CORE_ipsr_read();
	union epsr_t e = CORE_epsr_read();
	union apsr_t a = CORE_apsr_read();

	i.storage &= ~xPSR_ipsr_mask;
	i.storage |= xPSR & xPSR_ipsr_mask;
	CORE_ipsr_write(i);

	e.storage &= ~xPSR_epsr_mask;
	e.storage |= xPSR & xPSR_epsr_mask;
	CORE_epsr_write(e);

	uint32_t apsr_mask;
	if (HaveDSPExt())
		apsr_mask = xPSR_apsr_dsp_mask;
	else
		apsr_mask = xPSR_apsr_nodsp_mask;
	a.storage &= ~apsr_mask;
	a.storage |= xPSR & apsr_mask;
	CORE_apsr_write(a);
}

EXPORT enum Mode CORE_CurrentMode_read(void) {
	extern enum Mode CurrentMode;
	return SR(&CurrentMode);
}

EXPORT void CORE_CurrentMode_write(enum Mode mode) {
	extern enum Mode CurrentMode;
	return SW(&CurrentMode, mode);
}

EXPORT union apsr_t CORE_apsr_read(void) {
	extern union apsr_t physical_apsr;
	union apsr_t a;
	a.storage = SR(&physical_apsr.storage);
	return a;
}

EXPORT void CORE_apsr_write(union apsr_t val) {
	extern union apsr_t physical_apsr;
	uint8_t in_ITblock(void);

	if (in_ITblock()) {
		DBG1("WARN update of apsr in IT block\n");
	}
#ifdef M_PROFILE
	if (val.storage & 0x07f0ffff) {
		DBG1("WARN update of reserved APSR bits\n");
	}
#endif
	SW(&physical_apsr.storage, val.storage);
}

EXPORT union ipsr_t CORE_ipsr_read(void) {
	extern union ipsr_t physical_ipsr;
	union ipsr_t i;
	i.storage = SR(&physical_ipsr.storage);
	return i;
}

EXPORT void CORE_ipsr_write(union ipsr_t val) {
	extern union ipsr_t physical_ipsr;
	SW(&physical_ipsr.storage, val.storage);
}

EXPORT union epsr_t CORE_epsr_read(void) {
	extern union epsr_t physical_epsr;
	union epsr_t e;
	e.storage = SR(&physical_epsr.storage);
	return e;
}

EXPORT void CORE_epsr_write(union epsr_t val) {
	extern union epsr_t physical_epsr;
	SW(&physical_epsr.storage, val.storage);
}

EXPORT bool CORE_control_nPRIV_read(void) {
	extern union control_t physical_control;
	union control_t c;
	c.storage = SR(&physical_control.storage);
	return c.nPRIV;
}

EXPORT void CORE_control_nPRIV_write(bool npriv) {
	extern union control_t physical_control;
	union control_t c;
	c.storage = SR(&physical_control.storage);
	c.nPRIV = npriv;
	SW(&physical_control.storage, c.storage);
}

EXPORT bool CORE_control_SPSEL_read(void) {
	extern union control_t physical_control;
	union control_t c;
	c.storage = SR(&physical_control.storage);
	return c.SPSEL;
}


static void control_SPSEL_write(bool spsel, bool force, enum Mode forced_mode) {
	union control_t c;
	c.storage = SR(&physical_control.storage);
	c.SPSEL = spsel;
	SW(&physical_control.storage, c.storage);

	enum Mode mode = (force) ? forced_mode : CORE_CurrentMode_read();
	(void) mode;

	// XXX: I'm confused on exactly the semantics here, esp w.r.t. exceptions
	//if (mode == Mode_Thread) {
		SWP(&physical_sp_p, (spsel) ? &sp_process : &sp_main);
	//} else {
	//	CORE_ERR_unpredictable("SPSEL write in Handler mode\n");
	//}
}

EXPORT void CORE_control_SPSEL_write(bool spsel) {
	control_SPSEL_write(spsel, false, 0);
}

EXPORT void CORE_update_mode_and_SPSEL(enum Mode mode, bool spsel) {
	CORE_CurrentMode_write(mode);
	control_SPSEL_write(spsel, true, mode);
}

EXPORT bool CORE_primask_read(void) {
	extern uint32_t physical_primask;
	return SR(&physical_primask);
}

EXPORT void CORE_primask_write(bool val) {
	extern uint32_t physical_primask;
	SW(&physical_primask, val);
}

EXPORT uint8_t CORE_basepri_read(void) {
	extern uint32_t physical_basepri;
	return SR(&physical_basepri);
}

EXPORT void CORE_basepri_write(uint8_t val) {
	extern uint32_t physical_basepri;
	SW(&physical_basepri, val);
}

EXPORT bool CORE_faultmask_read(void) {
	extern uint32_t physical_faultmask;
	return SR(&physical_faultmask);
}

EXPORT void CORE_faultmask_write(bool val) {
	extern uint32_t physical_faultmask;
	SW(&physical_faultmask, val);
}

EXPORT union ufsr_t CORE_ufsr_read(void) {
	extern union ufsr_t ufsr;
	union ufsr_t u;
	u.storage = SR(&ufsr.storage);
	return u;
}

EXPORT void CORE_ufsr_write(union ufsr_t u) {
	extern union ufsr_t ufsr;
	SW(&ufsr.storage, u.storage);
}
#endif

static void reset_registers(void) {
	DBG2("begin\n");
#ifdef BOOTLOADER_REMAP_VECTOR_TABLE
	// Many chips have a bootloader at address 0 and then expose their
	// actual entry point further forward in flash. System images, however,
	// won't include these bootloaders (b/c trying to would overwrite them)
	write_word(VECTOR_TABLE_OFFSET, BOOTLOADER_REMAP_VECTOR_TABLE);
#endif
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

	CORE_CurrentMode_write(Mode_Thread);

	// ASPR = bits(32) UNKNOWN {nop}

	union ipsr_t ipsr_temp = CORE_ipsr_read();
	ipsr_temp.bits.exception = 0;
	CORE_ipsr_write(ipsr_temp);

	union epsr_t epsr_temp = CORE_epsr_read();
	epsr_temp.bits.T = tbit;
	epsr_temp.bits.ICI_IT_top = 0;
	epsr_temp.bits.ICI_IT_bot = 0;
	CORE_epsr_write(epsr_temp);

	///

	// B1.4.3: The special-purpose mask registers
	SW(&physical_primask, 0);
	SW(&physical_faultmask, 0);
	SW(&physical_basepri, 0);
	SW(&physical_control.storage, 0);
	DBG2("end\n");
}

__attribute__ ((constructor))
static void register_reset_registers(void) {
	_Static_assert(sizeof(union apsr_t) == 4, "Punned structure size");
	{
		union apsr_t a;
		a.storage = 0x80000000;
		if (!a.bits.N || a.bits.Z || a.bits.C || a.bits.V || a.bits.Q)
			CORE_ERR_runtime("Punned structure packing\n");
		a.storage = 0x40000000;
		if (a.bits.N || !a.bits.Z || a.bits.C || a.bits.V || a.bits.Q)
			CORE_ERR_runtime("Punned structure packing\n");
		a.storage = 0x20000000;
		if (a.bits.N || a.bits.Z || !a.bits.C || a.bits.V || a.bits.Q)
			CORE_ERR_runtime("Punned structure packing\n");
		a.storage = 0x10000000;
		if (a.bits.N || a.bits.Z || a.bits.C || !a.bits.V || a.bits.Q)
			CORE_ERR_runtime("Punned structure packing\n");
		a.storage = 0x08000000;
		if (a.bits.N || a.bits.Z || a.bits.C || a.bits.V || !a.bits.Q)
			CORE_ERR_runtime("Punned structure packing\n");
	}
	_Static_assert(sizeof(union ipsr_t) == 4, "Punned structure size");
	{
		union ipsr_t i;
		i.storage = 0xa5;
		if (i.bits.exception != 0xa5)
			CORE_ERR_runtime("Punned structure packing\n");
	}
	_Static_assert(sizeof(union epsr_t) == 4, "Punned structure size");
	_Static_assert(sizeof(union control_t) == 4, "Punned structure size");

	_Static_assert(sizeof(union ufsr_t) == 4, "Punned structure size");

	register_reset(reset_registers);
}
