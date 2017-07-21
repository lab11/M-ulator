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

#ifndef REGISTERS_H
#define REGISTERS_H

#include "core/common.h"

#ifndef PP_STRING
#define PP_STRING "REG"
#include "core/pretty_print.h"
#endif

#include "core/state_sync.h"

enum Mode {
	Mode_Handler,
	Mode_Thread,
};

#ifdef A_PROFILE

//union __attribute__ ((__packed__)) apsr_t {
union apsr_t {
	uint32_t storage;
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
	struct {
		unsigned mode		:  5;
		unsigned T		:  1;
		unsigned F		:  1;
		unsigned I		:  1;
		unsigned A		:  1;
		unsigned E		:  1;
		unsigned IT_high	:  6;
		unsigned GE		:  4;
		unsigned reserved0	:  4;
		unsigned J		:  1;
		unsigned IT_low		:  2;
		unsigned Q		:  1;
		unsigned V		:  1;
		unsigned C		:  1;
		unsigned Z		:  1;
		unsigned N		:  1;
	} bits;
};
#endif // A_PROFILE

#ifdef M_PROFILE

/* ARMv7-M implementations treat SP bits [1:0] as RAZ/WI.
 * ARM strongly recommends that software treats SP bits [1:0]
 * as SBZP for maximum portability across ARMv7 profiles.
 */
uint32_t    CORE_reg_read(int r);

void		CORE_reg_write(int r, uint32_t val);

uint32_t	CORE_xPSR_read(void);
void		CORE_xPSR_write(uint32_t);

enum Mode CORE_CurrentMode_read(void);

void CORE_CurrentMode_write(enum Mode mode);

//union __attribute__ ((__packed__)) apsr_t {
union apsr_t {
	uint32_t storage;
	//  0-15: <reserved>
	// 16-19: GE[3:0]	// for DSP extension
	// 20-26: <reserved>
	//    27: Q		// Cumulative saturation
	//    28: V		// Overflow
	//    29: C		// Carry
	//    30: Z		// Zero
	//    31: N		// Negative
	struct {
		unsigned reserved0	: 16;
		unsigned GE		:  4;
		unsigned reserved1	:  7;
		unsigned Q		:  1;
		unsigned V		:  1;
		unsigned C		:  1;
		unsigned Z		:  1;
		unsigned N		:  1;
	} bits;
};

union apsr_t CORE_apsr_read(void);

void CORE_apsr_write(union apsr_t val);


//union __attribute__ ((__packed__)) ipsr_t {
union ipsr_t {
	uint32_t storage;
	//   0-8: 0 or Exception Number
	//  9-31: <reserved>
	struct {
		unsigned exception	:  9;
		unsigned reserved0	: 23;
	} bits;
};

union ipsr_t CORE_ipsr_read(void);

void CORE_ipsr_write(union ipsr_t val);


//union __attribute__ ((__packed__)) epsr_t {
union epsr_t {
	uint32_t storage;
	//   0-9: <reserved>
	// 10-15: ICI/IT	//
	// 16-23: <reserved>
	//    24: T		// Thumb bit
	// 25-26: ICI/IT	//
	// 27-31: <reserved>
	struct {
		unsigned reserved0	: 10;
		unsigned ICI_IT_top	:  6;
		unsigned reserved1	:  8;
		unsigned T		:  1;
		unsigned ICI_IT_bot	:  2;
		unsigned reserved2	:  5;
	} bits;
};

union epsr_t CORE_epsr_read(void);

void CORE_epsr_write(union epsr_t val);


//union __attribute__ ((__packed__)) control_t {
union control_t {
	uint32_t storage;
	struct {
		unsigned nPRIV		:  1;
		unsigned SPSEL		:  1;
#ifdef HAVE_FP // How to do this exactly...
		unsigned FPCA		:  1;
		unsigned reserved0	: 29;
#else
		unsigned reserved0	: 30;
#endif
	};
};

bool CORE_control_nPRIV_read(void);

void CORE_control_nPRIV_write(bool npriv);

bool CORE_control_SPSEL_read(void);

void CORE_control_SPSEL_write(bool spsel);

// XXX: Workaround for SR/SW of dependent actions:
void CORE_update_mode_and_SPSEL(enum Mode mode, bool spsel);

#ifdef HAVE_FP
bool		CORE_control_FPCA_read(void);
void		CORE_control_FPCA_write(bool fpca);
#endif

bool CORE_primask_read(void);

void CORE_primask_write(bool val);

uint8_t CORE_basepri_read(void);

void CORE_basepri_write(uint8_t val);

bool CORE_faultmask_read(void); 

void CORE_faultmask_write(bool val);

union ufsr_t {
	uint32_t storage;
	struct {
		unsigned UNDEFINSTR  :  1;
		unsigned INVSTATE    :  1;
		unsigned INVPC       :  1;
		unsigned NOCP        :  1;
		unsigned reserved0   :  4;
		unsigned UNALIGNED   :  1;
		unsigned DIVBYZERO   :  1;
		unsigned reserved1   : 22;
	};
};

union ufsr_t CORE_ufsr_read(void);

void CORE_ufsr_write(union ufsr_t u);


#endif // M_PROFILE

#endif // REGISTERS_H

