// Actual, physical registers

import { Logger } from "tslog";
const log: Logger = new Logger({ name: "cpu::registers" });
log.silly("cpu::registers");

import { State } from "../scaffold/state";
import { cpu } from "./module";

//let apsr = new State(0);

//   0-8: 0 or Exception Number
//  9-31: <reserved>
const ipsr_mask_exception = 0x1ff;
let ipsr = new State(0);

//   0-9: <reserved>
// 10-15: ICI/IT	    //
// 16-23: <reserved>
//    24: T		        // Thumb bit
// 25-26: ICI/IT	    //
// 27-31: <reserved>
const epsr_mask_T = 1 << 24;
const epsr_mask_ICIIT_top = /*        */ 0b0111_1110_0000_0000;
const epsr_mask_ICIIT_bot = 0b11_0000_0000_0000_0000_0000_0000;
const epsr_mask_ICIIT = epsr_mask_ICIIT_top | epsr_mask_ICIIT_bot;
let epsr = new State(0);

const SP_REG = 13;
const LR_REG = 14;
const PC_REG = 15;

let physical_registers = [
    new State(0),
    new State(0),
    new State(0),
    new State(0),
    new State(0),
    new State(0),
    new State(0),
    new State(0),
    new State(0),
    new State(0),
    new State(0),
    new State(0),
];

// TODO: PPB
const VECTOR_TABLE_OFFSET = 0xE000_ED08;

let sp_process = new State(0);
let sp_main = new State(0);
let physical_lr = new State(0xFFFF_FFFF);

async function reset_registers() {
    log.silly("reset_registers");

    let vectortable = await cpu.core.memory_read(VECTOR_TABLE_OFFSET, 4);
    let initial_stack_pointer = await cpu.core.memory_read(vectortable, 4) & 0xffff_fffc;
    let initial_pc = await cpu.core.memory_read(vectortable + 4, 4);

    // R0-12 UNKNOWN on reset

    // SP = VECTOR_TABLE[0]
    sp_main.write(initial_stack_pointer);

    // Process SP = ((bits(30) UNKNOWN):'00')
    sp_process.write(sp_process.read() & ~0x3);

    physical_lr.write(0xFFFF_FFFF);

    CORE_reg_write(PC_REG, initial_pc & 0xFFFF_FFFE);
    let tbit = initial_pc & 0x1;
    if (tbit != 1) {
        throw cpu.Exception;
    }

    //TODO
    //CORE_CurrentMode_write(Mode_Thread);

    // ASPR = bits(32) UNKNOWN {nop}

    ipsr.write(ipsr.read() & ~ipsr_mask_exception);

    // EPSR only resets some bits on reset:
    let epsr_temp = epsr.read();
    // epsr_temp.bits.T = tbit;
    if (tbit) {
        epsr_temp |= epsr_mask_T
    } else {
        epsr_temp &= ~epsr_mask_T;
    }
    // epsr_temp.bits.ICI_IT_top = 0;
    // epsr_temp.bits.ICI_IT_bot = 0;
    epsr_temp &= ~epsr_mask_ICIIT;
    // Write back reset value
    epsr.write(epsr_temp);

    // B1.4.3: The special-purpose mask registers
    //SW(&physical_primask, 0);
    //SW(&physical_faultmask, 0);
    //SW(&physical_basepri, 0);
    //SW(&physical_control.storage, 0);
}

cpu.reset_hooks.push(reset_registers);

export function CORE_reg_read(r: number) {
    //TODO
    //assert((r >= 0) && (r < 16));

    if (r == SP_REG) {
        // FIXME: Choose stack pointer
        return sp_main.read() & 0xfffffffc;
    } else if (r == LR_REG) {
        return physical_lr.read();
    } else if (r == PC_REG) {
        return cpu.pipeline_registers.id_ex_PC.read() & 0xfffffffe;
    } else {
        return physical_registers[r].read();
    }
}

export function CORE_reg_write(r: number, val: number) {
    //TODO
    //assert(r >= 0 && r < 16 && "CORE_reg_write");

    if (r == SP_REG) {
        // FIXME: Choose stack pointer
        sp_main.write(val & 0xfffffffc);
    } else if (r == LR_REG) {
        physical_lr.write(val);
    } else if (r == PC_REG) {
        log.silly("Writing %08x to PC\n", val & 0xfffffffe);

        if (false /*state_is_debugging()*/) {
            //DBG1("PC write + debugging --> flush\n");
            //state_pipeline_flush(val & 0xfffffffe);
        } else {
            // Only flush if the new PC differs from predicted in pipeline:
            let if_id_PC = cpu.pipeline_registers.if_id_PC.read();
            if (((if_id_PC & 0xfffffffe) - 4) == (val & 0xfffffffe)) {
                log.silly("Predicted PC correctly (%08x)\n", val);
            } else {
                log.silly("Predicted PC incorrectly\n");
                log.silly("Pred: %08x, val: %08x\n", if_id_PC, val);
                throw cpu.Exception; // not impl
                //state_pipeline_flush(val & 0xfffffffe);
            }
        }
    }
    else {
        physical_registers[r].write(val);
    }
}

export function CORE_epsr_read() {
    return epsr.read();
}