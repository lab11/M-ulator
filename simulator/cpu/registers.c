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

/* Ignore mode transitions for now */
static uint32_t reg[SP_REG];	// SP,LR,PC not held here, so 13 registers
static uint32_t sp_process;
static uint32_t sp_main __attribute__ ((unused));
static uint32_t *sp = &sp_process;
static uint32_t lr;

#define SP	(*sp)
#define LR	(lr)
#ifdef NO_PIPELINE
#define PC	(pre_if_PC)
#else
#define PC	(id_ex_PC)
#endif

#ifdef A_PROFILE
// (|= 0x0010) Start is user mode
// (|= 0x0020) CPU only supports thumb (for now)
static uint32_t apsr = 0x0030;

// "Current" program status register
//   0-4: M[4:0]	// Mode field: 10000 user  10001 FIQ 10010 IRQ 10011 svc
			//             10111 abort 11011 und 11111 sys
//     5: T		// Thumb bit
//     6: F		// Fast Interrupt disable (FIQ interrupts)
//     7: I		// Interrupt disable (IRQ interrupts)
//     8: A		// Asynchronous abort disable
//     9: E		// Endianness execution state bit (0: little, 1: big)
// 10-15: IT[7:2]	// it-bits
// 16-19: GE[3:0]	// Greater than or equal flags for SIMD instructions
// 20-23: <reserved>
//    24: J		// Jazelle bit
// 25-26: IT[1:0]	// it-bits
//    27: Q		// Cumulative saturation
//    28: V		// Overflow
//    29: C		// Carry
//    30: Z		// Zero
//    31: N		// Negative
static uint32_t *cpsr = &apsr;
#define CPSR		(*cpsr)
#define ITSTATE		( ((CPSR & 0xfc00) >> 8) | ((CPSR & 0x06000000) >> 25) )

#endif // A

#ifdef M_PROFILE

static uint32_t apsr;
//  0-15: <reserved>
// 16-19: GE[3:0]	// for DSP extension
// 20-26: <reserved>
//    27: Q		// Cumulative saturation
//    28: V		// Overflow
//    29: C		// Carry
//    30: Z		// Zero
//    31: N		// Negative
static uint32_t ipsr = 0x0;
//   0-8: 0 or Exception Number
//  9-31: <reserved>
static uint32_t epsr = 0x01000000;
//   0-9: <reserved>
// 10-15: ICI/IT	//
// 16-23: <reserved>
//    24: T		// Thumb bit
// 25-26: ICI/IT	//
// 27-31: <reserved>
#define CPSR		(apsr)
#define APSR		(apsr)
#define IPSR		(ipsr)
#define EPSR		(epsr)
#define IAPSR		(IPSR | APSR)
#define EAPSR		(EPSR | APSR)
#define XPSR		(IPSR | APSR | EPSR)
#define IEPSR		(IPSR | EPSR)

#define ITSTATE		( ((EPSR & 0xfc00) >> 8) | ((EPSR & 0x06000000) >> 25) )

static uint32_t primask __attribute__ ((unused)) = 0x0;
//     0: priority	The exception mask register, a 1-bit register.
//			Setting PRIMASK to 1 raises the execution priority to 0.
static uint32_t basepri __attribute__ ((unused)) = 0x0;
/* The base priority mask, an 8-bit register. BASEPRI changes the priority
 * level required for exception preemption. It has an effect only when BASEPRI
 * has a lower value than the unmasked priority level of the currently
 * executing software.  The number of implemented bits in BASEPRI is the same
 * as the number of implemented bits in each field of the priority registers,
 * and BASEPRI has the same format as those fields.  For more information see
 * Maximum supported priority value on page B1-636.  A value of zero disables
 * masking by BASEPRI.
 */
static uint32_t faultmask __attribute__ ((unused)) = 0x0;
/* The fault mask, a 1-bit register. Setting FAULTMASK to 1 raises the
 * execution priority to -1, the priority of HardFault. Only privileged
 * software executing at a priority below -1 can set FAULTMASK to 1. This means
 * HardFault and NMI handlers cannot set FAULTMASK to 1. Returning from any
 * exception except NMI clears FAULTMASK to 0.
 */

static uint32_t control __attribute__ ((unused)) = 0x0;
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
		//}
#else
		if (state_is_debugging()) {
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

EXPORT uint32_t CORE_cpsr_read(void) {
	return SR(&CPSR);
}

EXPORT void CORE_cpsr_write(uint32_t val) {
	if (in_ITblock()) {
		DBG1("WARN update of cpsr in IT block\n");
	}
#ifdef M_PROFILE
	if (val & 0x07f0ffff) {
		DBG1("WARN update of reserved CPSR bits\n");
	}
#endif
	SW(&CPSR, val);
}

#ifdef M_PROFILE
EXPORT uint32_t CORE_ipsr_read(void) {
	return SR(&IPSR);
}

EXPORT void CORE_ipsr_write(uint32_t val) {
	SW(&IPSR, val);
}

EXPORT uint32_t CORE_epsr_read(void) {
	return SR(&EPSR);
}

EXPORT void CORE_epsr_write(uint32_t val) {
	SW(&EPSR, val);
}
#endif

static void reset_registers(void) {
	CORE_reg_write(SP_REG, read_word(0x00000000));
	CORE_reg_write(LR_REG, 0xFFFFFFFF);
	CORE_reg_write(PC_REG, read_word(0x00000004));
}

__attribute__ ((constructor))
static void register_reset_registers(void) {
	register_reset(reset_registers);
}
