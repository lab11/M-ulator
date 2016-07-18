/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2013  Pat Pannuto <pat.pannuto@gmail.com>
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

// A4.5 Status register access instructions
#include "helpers.h"

#include "cpu/features.h"
#include "cpu/registers.h"
#include "cpu/misc.h"

/* XXX EXPORT unimplemented func's to ensure they conflict if they
   are ever implemented elsewhre */
EXPORT bool CurrentModeIsPrivileged(void) {
	CORE_ERR_not_implemented("CurrentModeIsPrivileged\n");
}

EXPORT int ExecutionPriority(void) {
	CORE_ERR_not_implemented("ExecutionPriority\n");
}

void cps(bool enable, bool disable, bool affectPri, bool affectFault) {
	if (in_ITblock())
		CORE_ERR_unpredictable("cps_t1 case\n");

	if (CurrentModeIsPrivileged()) {
		if (enable) {
			if (affectPri)
				CORE_primask_write(0);
			if (affectFault)
				CORE_faultmask_write(0);
		}
		if (disable) {
			if (affectPri)
				CORE_primask_write(1);
			if (affectFault && (ExecutionPriority() > -1))
				CORE_faultmask_write(1);
		}
	}
}

void mrs(uint8_t SYSm, uint8_t rd) {
	if (BadReg(rd))
		CORE_ERR_unpredictable("mrs_t1 case\n");

	switch (SYSm) {
		case 0: case 1: case 2: case 3: case 5: case 6: case 7: case 8:
		case 9: case 16: case 17: case 18: case 19: case 20:
			break;
		default:
			CORE_ERR_unpredictable("Bad SYSm value in mrs_t1\n");
	}

	uint32_t rd_val = CORE_reg_read(rd);

	bool SYSm0 = !!(SYSm & 0x1);
	bool SYSm1 = !!(SYSm & 0x2);
	bool SYSm2 = !!(SYSm & 0x4);
	switch (SYSm >> 3) {
		case 0:
			if (SYSm0) {
				rd_val &= ~0x1ff;
				rd_val |= (CORE_ipsr_read().storage & 0x1ff);
			}
			if (SYSm1) {
				// clear <26:24> and <15:10>
				rd_val &= ~0x700fc00;
			}
			if (SYSm2) {
				rd_val &= ~0xf8000000;
				rd_val |= (CORE_apsr_read().storage & 0xf8000000);
				if (HaveDSPExt()) {
					rd_val &= ~0xf0000;
					rd_val |= (CORE_apsr_read().storage & 0xf0000);
				}
			}
			break;
		case 1:
			if (CurrentModeIsPrivileged()) {
				switch (SYSm & 0x7) {
					case 0:
						//rd_val = MSP;
						CORE_ERR_not_implemented("MSP\n");
						//break;
					case 1:
						//rd_val = PSP;
						CORE_ERR_not_implemented("PSP\n");
						//break;
					default:
						CORE_ERR_unpredictable("Bad SYSm\n");
				}
			}
			break;
		case 2:
			switch (SYSm & 0x7) {
				case 0:
					rd_val &= ~0x1;
					if (CurrentModeIsPrivileged())
						rd_val |= (CORE_primask_read() & 0x1);
					break;
				case 1:
				case 2:
					rd_val &= ~0xff;
					if (CurrentModeIsPrivileged())
						rd_val |= (CORE_basepri_read() & 0xff);
					break;
				case 3:
					rd_val &= ~0x1;
					if (CurrentModeIsPrivileged())
						rd_val |= (CORE_faultmask_read() & 0x1);
					break;
				case 4:
#ifdef HAVE_FP
					if (HaveFPExt()) {
						rd_val &= ~0x7;
						rd_val |= CORE_control_nPRIV_read() << 0;
						rd_val |= CORE_control_SPSEL_read() << 1;
						rd_val |= CORE_control_FPCA_read() << 2;
					} else
#endif
					{
						rd_val &= ~0x3;
						rd_val |= CORE_control_nPRIV_read() << 0;
						rd_val |= CORE_control_SPSEL_read() << 1;
					}
					break;
				default:
					CORE_ERR_unpredictable("Bad SYSm\n");
			}
			break;
		default:
			CORE_ERR_unpredictable("Bad SYSm\n");
	}

	CORE_reg_write(rd, rd_val);
}

void msr(uint8_t SYSm, uint8_t mask, uint8_t rn) {
	switch (mask) {
		case 0x2:
			// _nzcvq
			break;
		case 0x1:
			// _g, Write the GE[3:0] bits, APSR [19:16]
		case 0x3:
			// _nzcvqg, Write the N, Z, C, V, Q, and GE[3:0] bits
			CORE_ERR_not_implemented("DSP Extentions (msr_t1)\n");
			//break;
		default:
			CORE_ERR_unpredictable("Illegal mask (msr_t1)\n");
	}

	if (BadReg(rn))
		CORE_ERR_unpredictable("msr_t1 case\n");

	switch (SYSm) {
		case 0: case 1: case 2: case 3: case 5: case 6: case 7: case 8:
		case 9: case 16: case 17: case 18: case 19: case 20:
			break;
		default:
			CORE_ERR_unpredictable("Bad SYSm value in msr_t1\n");
	}

	uint32_t rn_val = CORE_reg_read(rn);
	union apsr_t apsr = CORE_apsr_read();

	bool SYSm2 = !!(SYSm & 0x4);
	bool mask0 = !!(mask & 0x1);
	bool mask1 = !!(mask & 0x2);
	switch (SYSm >> 3) {
		case 0:
			if (SYSm2 == 0) {
				if (mask0 == 1) {
					if (!HaveDSPExt())
						CORE_ERR_unpredictable("deep\n");
					else {
						apsr.storage &= ~0xf0000;
						apsr.storage |= (rn_val & 0xf0000);
					}
				}
				if (mask1) {
					apsr.storage &= ~0xf8000000;
					apsr.storage |= (rn_val & 0xf8000000);
				}
				CORE_apsr_write(apsr);
			}
			break;
		case 1:
			if (CurrentModeIsPrivileged()) {
				switch (SYSm & 0x7) {
					case 0:
						//MSP = rn_val;
						CORE_ERR_not_implemented("MSP\n");
						//break;
					case 1:
						//PSP = rn_val;
						CORE_ERR_not_implemented("PSP\n");
						//break;
					default:
						CORE_ERR_unpredictable("Bad SYSm\n");
				}
			}
			break;
		case 2:
			switch (SYSm & 0x7) {
				case 0:
					if (CurrentModeIsPrivileged())
						CORE_primask_write(rn_val & 0x1);
					break;
				case 1:
					if (CurrentModeIsPrivileged())
						CORE_basepri_write(rn_val & 0xff);
					break;
				case 2:
					if (CurrentModeIsPrivileged()) {
						uint8_t rn_val8 = rn_val & 0xff;
						if ((rn_val8) != 0) {
							uint8_t basepri = CORE_basepri_read();
							if ((rn_val8 < basepri) || (basepri == 0))
								CORE_basepri_write(rn_val8);
						}
					}
					break;
				case 3:
					if (CurrentModeIsPrivileged()) {
						if (ExecutionPriority() > -1) {
							CORE_faultmask_write(rn_val & 0x1);
						}
					}
					break;
				case 4:
					if (CurrentModeIsPrivileged()) {
						/*
						   CONTROL.nPRIV = R[n]<0>;
						   If CurrentMode == Mode_Thread then
						      CONTROL.SPSEL = R[n]<1>;
						   if HaveFPExt() then
						      CONTROL.FPCA = R[n]<2>;
						*/
					}
					break;
				default:
					CORE_ERR_unpredictable("Bad SYSm\n");
			}
			break;
		default:
			CORE_ERR_unpredictable("Bad SYSm\n");
	}
}
