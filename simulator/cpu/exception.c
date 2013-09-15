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

#include "exception.h"
#include "core.h"

#include "features.h"
// "private" export from registers:
extern uint32_t sp_process;
extern uint32_t sp_main;
#include "registers.h"

#include "core/state_sync.h"

#include "common/private_peripheral_bus/ppb.h"
//#define CCR_STKALIGN (read_word(CONFIGURATION_CONTROL) & CONFIGURATION_CONTROL_STKALIGN_MASK)
#define CCR_STKALIGN 0
#define CCR_NONBASETHRDENA (read_word(CONFIGURATION_CONTROL) & CONFIGURATION_CONTROL_NONBASETHRDENA_MASK)


static int  ExceptionActiveBitCount;
static bool ExceptionActive[MAX_EXCEPTION_TYPE];


static uint32_t ReturnAddress(enum ExceptionType type, bool precise,
		uint32_t fault_inst, uint32_t next_inst) {
	// NOTE: ReturnAddress() is always halfword aligned - bit<0> is always zero
	// xPSR.IT bits saved to the stack are consistent with ReturnAddress()

	switch (type) {
		case NMI:
			return next_inst;
		case HardFault:
			if (precise)
				return fault_inst;
			else
				return next_inst;
		case MemManage:
			return fault_inst;
		case BusFault:
			if (precise)
				return fault_inst;
			else
				return next_inst;
		case UsageFault:
			return fault_inst;
		case SVCall:
			return next_inst;
		case DebugMonitor:
			if (precise)
				return fault_inst;
			else
				return next_inst;
		default:
			// "case IRQ:"
			return next_inst;
	}
}

static void PushStack(enum ExceptionType type, bool precise,
		uint32_t fault_inst, uint32_t next_inst) {
	uint8_t framesize;
	bool forcealign;
	bool frameptralign;
	uint32_t spmask;

	bool spsel = CORE_control_SPSEL_read();

	// Lots of floating point details missing

	if (0 /* FP */ ) {
	} else {
		framesize = 0x20;
		forcealign = CCR_STKALIGN;
	}

	spmask = ~(forcealign << 2);

	uint32_t *sp;

	if (spsel && (CORE_CurrentMode_read() == Mode_Thread)) {
		sp = &sp_process;
	} else {
		sp = &sp_main;
	}
	frameptralign = (!!(SR(sp) & 0x4)) & forcealign;
	uint32_t frameptr = (SR(sp) - framesize) & spmask;
	SW(sp, frameptr);

	write_word(frameptr+0x00, CORE_reg_read(0));
	write_word(frameptr+0x04, CORE_reg_read(1));
	write_word(frameptr+0x08, CORE_reg_read(2));
	write_word(frameptr+0x0c, CORE_reg_read(3));
	write_word(frameptr+0x10, CORE_reg_read(12));
	write_word(frameptr+0x14, CORE_reg_read(LR_REG));
	write_word(frameptr+0x18, ReturnAddress(type, precise, fault_inst, next_inst));
	write_word(frameptr+0x1c, CORE_xPSR_read() | (frameptralign << 9));

	if (0 /* FP */) {
	} else {
		uint32_t lr_val;
		if (CORE_CurrentMode_read() == Mode_Handler)
			lr_val = 0xfffffff1;
		else
			lr_val = 0xfffffff9 | (spsel << 2);
		CORE_reg_write(LR_REG, lr_val);
	}
}

static void ExceptionTaken(enum ExceptionType type) {
	bool tbit;
	uint32_t tmp;

	uint32_t vectortable = read_word(VECTOR_TABLE_OFFSET);
	tmp = read_word(vectortable+4*type);
	DBG1("Exception setting PC to %08x\n", tmp & 0xfffffffe);
	CORE_reg_write(PC_REG, tmp & 0xfffffffe);
	tbit = tmp & 0x1;
	CORE_update_mode_and_SPSEL(Mode_Handler, 0);

	union ipsr_t ipsr = CORE_ipsr_read();
	union epsr_t epsr = CORE_epsr_read();

	ipsr.bits.exception = type;
	epsr.bits.T = tbit;
	epsr.bits.ICI_IT_top = 0;
	epsr.bits.ICI_IT_bot = 0;
#ifdef HAVE_FP
	if (HaveFPExt())
		CORE_control_FPCA_write(1);
#endif
	if (ExceptionActive[type] == 0)
		ExceptionActiveBitCount++;
	ExceptionActive[type] = 1;

	WARN("Exception entry skipped some steps. Not executed:\n");
	WARN("   SCS_UpdateStatusRegs()\n");
	WARN("   ClearExclusievLocal()\n");
	WARN("   SetEventRegister()\n");
	WARN("   InstructionSynchronizationBarrier()\n");

	CORE_ipsr_write(ipsr);
	CORE_epsr_write(epsr);
}

static void ExceptionEntry(enum ExceptionType type, bool precise,
		uint32_t fault_inst, uint32_t next_inst) {
	if (unlikely(type >= MAX_EXCEPTION_TYPE))
		CORE_ERR_runtime("Exception number too high\n");
	PushStack(type, precise, fault_inst, next_inst);
	ExceptionTaken(type);
}

static uint32_t PopStack(uint32_t frameptr, uint32_t exc_return) {
	uint32_t new_pc;
	uint8_t framesize;
	bool forcealign;

	if (HaveFPExt() && ((exc_return & 0x10) == 0)) {
		framesize = 0x68;
		forcealign = 1;
	} else {
		framesize = 0x20;
		forcealign = CCR_STKALIGN;
	}

	CORE_reg_write(0,      read_word(frameptr+0x00));
	CORE_reg_write(1,      read_word(frameptr+0x04));
	CORE_reg_write(2,      read_word(frameptr+0x08));
	CORE_reg_write(3,      read_word(frameptr+0x0c));
	CORE_reg_write(12,     read_word(frameptr+0x10));
	CORE_reg_write(LR_REG, read_word(frameptr+0x14));
	new_pc =               read_word(frameptr+0x18);
	CORE_reg_write(PC_REG, new_pc);
	uint32_t xPSR =        read_word(frameptr+0x1c);
	CORE_xPSR_write(xPSR);

#ifdef HAVE_FP
	if (0 /* FP */)
		; // ...
#endif

	uint32_t spmask = (!!(xPSR & 0x200) && forcealign) << 2;

	uint32_t *sp;
	switch (exc_return & 0xf) {
		case 0x1:	// returning to Handler
			sp = &sp_main;
			break;
		case 0x9:	// returning to Thread using Main stack
			sp = &sp_main;
			break;
		case 0xd:	// returning to Thread using Process stack
			sp = &sp_process;
			break;
		default:
			CORE_ERR_unpredictable("Bad exception return\n");
	}
	SW(sp, (SR(sp) + framesize) | spmask);

	return new_pc;
}

static void DeActivate(enum ExceptionType ReturningExceptionNumber) {
	if (ExceptionActive[ReturningExceptionNumber])
		ExceptionActiveBitCount--;
	ExceptionActive[ReturningExceptionNumber] = 0;
	union ipsr_t ipsr = CORE_ipsr_read();
	if (ipsr.bits.exception != NMI)
		CORE_faultmask_write(0);
	return;
}

static void set_ufsr_invpc(bool invpc) {
	union ufsr_t ufsr = CORE_ufsr_read();
	ufsr.INVPC = invpc;
	CORE_ufsr_write(ufsr);
}

void ExceptionReturn(uint32_t exc_return) {
	assert(CORE_CurrentMode_read() == Mode_Handler);
	if (0x0ffffff0 != (0x0ffffff0 & exc_return))
		CORE_ERR_unpredictable("ExceptionReturn bad addr\n");

	union ipsr_t ipsr = CORE_ipsr_read();
	enum ExceptionType ReturningExceptionNumber = ipsr.bits.exception;
	int NestedActivation; // used for Handler -> Thread check when value == 1

	NestedActivation = ExceptionActiveBitCount;

	uint32_t frameptr;

	if (ExceptionActive[ReturningExceptionNumber] == 0) {
		DeActivate(ReturningExceptionNumber);
		set_ufsr_invpc(1);
		CORE_reg_write(LR_REG, 0xf0000000 | exc_return);
		ExceptionTaken(UsageFault); // return from inactive handler
		return;
	} else {
		switch (exc_return & 0xf) {
			case 0x1:	// return to Handler
				frameptr = SR(&sp_main);
				CORE_update_mode_and_SPSEL(Mode_Handler, 0);
				break;
			case 0x9:	// return to Thread using Main stack
				if ((NestedActivation != 1) && (CCR_NONBASETHRDENA == 0)) {
					DeActivate(ReturningExceptionNumber);
					set_ufsr_invpc(1);
					CORE_reg_write(LR_REG, 0xf0000000 | exc_return);
					ExceptionTaken(UsageFault); // return to Thread exception mismatch
					return;
				} else {
					frameptr = SR(&sp_main);
					CORE_update_mode_and_SPSEL(Mode_Thread, 0);
				}
				break;
			case 0xd:	// returning to Thread using Process stack
				if ((NestedActivation != 1) && (CCR_NONBASETHRDENA == 0)) {
					DeActivate(ReturningExceptionNumber);
					set_ufsr_invpc(1);
					CORE_reg_write(LR_REG, 0xf0000000 | exc_return);
					ExceptionTaken(UsageFault); // return to Thread exception mismatch
					return;
				} else {
					frameptr = SR(&sp_process);
					CORE_update_mode_and_SPSEL(Mode_Thread, 1);
				}
				break;
			default:
				DeActivate(ReturningExceptionNumber);
				set_ufsr_invpc(1);
				CORE_reg_write(LR_REG, 0xf0000000 | exc_return);
				ExceptionTaken(UsageFault); // illegal exc_return
				return;
		}

		uint32_t new_pc;
		DeActivate(ReturningExceptionNumber);
		new_pc = PopStack(frameptr, exc_return);

		if ((CORE_CurrentMode_read() == Mode_Handler) && (ipsr.bits.exception == 0)) {
			set_ufsr_invpc(1);
			PushStack(UsageFault, true, new_pc, new_pc);	// undo PopStack
			CORE_reg_write(LR_REG, 0xf0000000 | exc_return);
			ExceptionTaken(UsageFault); // return IPSR inconsistent
			return;
		}

		if ((CORE_CurrentMode_read() == Mode_Thread) && (ipsr.bits.exception != 0)) {
			set_ufsr_invpc(1);
			PushStack(UsageFault, true, new_pc, new_pc);	// undo PopStack
			CORE_reg_write(LR_REG, 0xf0000000 | exc_return);
			ExceptionTaken(UsageFault); // return IPSR inconsistent
			return;
		}

		WARN("Exception return skipped some steps. Not executed:\n");
		WARN("   ClearExclusievLocal()\n");
		WARN("   SetEventRegister()\n");
		WARN("   InstructionSynchronizationBarrier()\n");
		WARN("   if (...) SleepOnExit()\n");

		/*
		if ((CORE_CurrentMode_read() == Mode_Thread) && (NestedActivation == 0) && (SCR.SLEEPONEXIT == 1)) {
			SleepOnExit(); // impl defined
		}
		*/
	}
}

#if 0

void HardFault_on_vector_read_error(void) {
	status HFSR.VECTTBL;
	vector DEMCR.VC_INTERR;
	// Bus error returned when reading the vector table entry
}

void HardFault_on_fault_escalation(void) {
	status HFSR.FORCED;
	vector DEMCR.VC_HARDERR;
	// Fault or supervisor call occurred, and the handler priority is lower than or equal to the execution priority. The exception escalates to a HardFault. The processor updates the fault address and status registers, as appropriate.
}

void HardFault_on_breakpoint_escalation(void) {
	status HFSR.DEBUGEVT;
	vector DEMCR.VC_HARDERR;
	// Occurs when halting debug and the DebugMonitor are disabled, and an exception associated with BKPT is escalated.
}

void BusFault_on_exception_entry_stack_memory_operations(void) {
	status BFSR.STKERR;
	vector DEMCR.VC_INTERR;
	// Failure on a hardware save of context. The fault returns a bus error, but the processor does not update the BusFault Address Register.
}

void MemManage_fault_on_exception_entry_stack_memory_operations(void) {
	status MMFSR.MSTKERR;
	vector DEMCR.VC_INTERR;
	// Failure on a hardware save of context, because of an MPU access violation. The processor does not update the MemManage Address Register.
}

void BusFault_on_exception_return_stack_memory_operations(void) {
	status BFSR.UNSTKERR;
	vector DEMCR.VC_INTERR;
	// Failure on a hardware restore of context. The fault returns a bus error, but the processor does not update the Bus Fault Address Register.
}

void MemMange_fault_on_exception_return_stack_memory_operations(void) {
	status MMFSR.MUNSTKERR;
	vector DEMCR.VC_INTERR;
	// Failure on a hardware restore of context, because of an MPU access violation. The processor does not update the MemManage Address Register.
}

void MemMange_fault_on_data_access(void) {
	status MMFSR.DACCVIOL;
	vector DEMCR.VC_MMERR;
	// MPU violation or fault caused by an explicit memory access. The processor writes the data address of the load or store to the MemManage Address Register.
}

void MemManage_fault_on_instruction_access(void) {
	status MMFSR.IACCVIOL;
	vector DEMCR.VC_MMERR;
	// MPU violation or fault caused by an instruction fetch, or an instruction fetch from XN memory when there is no MPU. The fault occurs only if the processor attempts to execute the instruction. The processor does not update the MemManage Address Register.
}

void BusFault_on_instruction_fetch_precise(void) {
	status BFSR.IBUSERR;
	vector DEMCR.VC_BUSERR;
	// Bus error on an instruction fetch. The fault occurs only if the processor attempts to execute the instruction. The processor does not update the Bus Fault Address Register.
}

void BusFault_on_data_access_precise(void) {
	status BFSR.PRECISERR;
	vector DEMCR.VC_BUSERR;
	// Precise bus error caused by an explicit memory access. The processor writes the data address of the load or store to the Bus Fault Address Register.
}

void BusFault_bus_error_on_data_bus_imprecise(void) {
	status BFSR.IMPRECISERR;
	vector DEMCR.VC_BUSERR;
	// Imprecise bus error caused by an explicit memory access. The processor does not update the Bus Fault Address Register.
}

void UsageFault_no_coprocessor(void) {
	status UFSR.NOCP;
	vector DEMCR.VC_NOCPERR;
	// Occurs on an attempt to access a coprocessor that does not exist, or to which access is denied, see Coprocessor Access Control Register, CPACR on page B3-735.
}

void UsageFault_undefined_instruction(void) {
	status UFSR.UNDEFINSTR;
	vector DEMCR.VC_STATERR;
	// Occurs if the processor attempts to execute an unknown instruction, including any unknown instruction associated with an enabled coprocessor.
}

void UsageFault_attempt_to_execute_an_instruction_when_epsr_t_is_0(void) {
	status UFSR.INVSTATE;
	vector DEMCR.VC_STATERR;
	// Occurs if the processor attempts to execute in an invalid EPSR state, for example after a BX instruction branches to an unsupported state. This fault includes any state change after entry to or return from an exception, as well as from an inter-working instruction.
}

void UsageFault_exception_return_integrity_check_failures(void) {
	status UFSR.INVPC;
	vector DEMCR.VC_STATERR;
	// Indicates any failure of the integrity checks for exception returns described in Integrity checks on exception return on page B1-654.
}

void UsageFault_illegal_unaligned_load_or_store(void) {
	status UFSR.UNALIGNED;
	vector DEMCR.VC_CHKERR;
	// Occurs when a load-store multiple instruction attempts to access a non-word aligned location. If the CCR.UNALIGN_TRP bit is set to 1 it occurs, also, for any load or store that is not naturally aligned.
}

#endif

void UsageFault_divide_by_0(void) {
	//status UFSR.DIVBYZERO;
	//vector DEMCR.VC_CHKERR;
	// If the CCR.DIV_0_TRP bit is set to 1, this occurs when the processor attempts to execute SDIV or UDIV with a divisor of 0.
	WARN("TODO: Sort through how PC's work in precise exceptions\n");
	uint32_t fault_pc = CORE_reg_read(PC_REG);
	return generic_exception(UsageFault, true, fault_pc, fault_pc);
}





#ifdef CYCLE_ACCURATE
void exception_entry_fn(void) {
	;
}

static uint32_t exception_to_take;
void generic_exception(enum ExceptionType type, bool precise) {
	SW(&exception_to_take, type);
	state_take_exception(exception_entry_fn);
}
#else
void generic_exception(enum ExceptionType type, bool precise,
		uint32_t fault_inst, uint32_t next_inst) {
	ExceptionEntry(type, precise, fault_inst, next_inst);
}

void exception_return(uint32_t exception_return_pc) {
	ExceptionReturn(exception_return_pc);
}
#endif
