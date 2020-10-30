// A CPU core.

import { Logger } from "tslog";
const log: Logger = new Logger({ name: "cpu::cpu" });
log.silly("cpu::cpu");

import assert = require("assert");

import { Core, Pipeline } from "../scaffold/simulator";
import { State, StateNonNumeric } from "../scaffold/state";

// Hardware exceptions
export class Exception extends Error { }

// How the exception happened
export class SynchronousException extends Exception { }
export class AsynchronousException extends Exception { }

export interface MemmapEntry {
    name: string,
    start_address: number,
    end_address: number,
    reset?(): Promise<VoidFunction>,
    read32(address: number): Promise<number>,
    write32(address: number, value: number): Promise<VoidFunction>,
}

function init() {
    // Do stuff
}

interface Memmap extends Array<MemmapEntry> { }

function peripheral_lookup(address: number) {
    for (const periph of memmap) {
        if (address < periph.start_address) {
            continue
        } else if (address < periph.end_address) {
            return periph;
        }
    }
    log.warn(`MMIO lookup failed for address 0x${address.toString(16)}`);
    throw SynchronousException;
}

function memory_read(address: number, width: number): Promise<number> {
    let periph = peripheral_lookup(address);
    assert(width == 4);
    return periph.read32(address);
}

function memory_write(address: number, width: number, value: number) {
    let periph = peripheral_lookup(address);
    assert(width == 4);
    return periph.write32(address, value);
}

export const STALL_PC = 0;
export const INST_NOP = 0;

export const HAZARD_PC = 0;
export const INST_HAZARD = 0;

export let pipeline_registers = {
    // Inputs to IF stage
    pre_if_PC: new State(HAZARD_PC),

    // Outputs of IF, inputs to ID stage
    if_id_PC: new State(HAZARD_PC),
    if_id_inst: new State(INST_HAZARD),

    // Outputs of ID, inputs to EX stage
    id_ex_PC: new State(HAZARD_PC),
    id_ex_inst: new State(INST_HAZARD),
    id_ex_op: new StateNonNumeric(null),
}

// TODO: Think about this arch in JS
let pending_pipeline_flush: number | null = null;
export function pipeline_flush(new_pc: number) {
    pending_pipeline_flush = new_pc;
}

export function pending_exception(): Boolean {
    return (pending_pipeline_flush == null);
}

let id_stage = {
    name: "Decode",
    tick: function (_core: Core) { return Promise.resolve() },
}

let ex_stage = {
    name: "Execute",
    tick: function (_core: Core) { return Promise.resolve() },
}


// Hook points for peripherals to attach themselves
export let reset_hooks: Array<Function> = [];
export let memmap: Memmap = [];

// Hook point for pipeline stages to attach themselves
// TODO: Can this export export to just this folder?
export let pipeline: Pipeline = [];
pipeline.push(id_stage);
pipeline.push(ex_stage);

export let core = {
    reset: async function () {
        let todo = [];
        for (const hook of reset_hooks) {
            log.debug(hook);
            todo.push(hook());
        }

        for (const periph of memmap) {
            if (periph.reset) {
                log.debug(periph.reset);
                todo.push(periph.reset());
            }
        }
        return Promise.all(todo);
    },
    init: init,

    pipeline: pipeline,
    // exception function?

    memory_read: memory_read,
    memory_write: memory_write,
} as Core;