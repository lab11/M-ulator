// A CPU core.

import { Logger } from "tslog";
const log: Logger = new Logger({ name: "cpu::cpu" });
log.silly("cpu::cpu");

import assert = require("assert");

import { Core } from "../scaffold/simulator";
import { State } from "../scaffold/state";
import { if_stage } from "./module";


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
}

let id_stage = {
    tick: function (_core: Core) { return Promise.resolve() },
}

let ex_stage = {
    tick: function (_core: Core) { return Promise.resolve() },
}


// Hook points for peripherals to attach themselves
export let reset_hooks: Array<Function> = [];
export let memmap: Memmap = [];

export let core = {
    reset: async function () {
        let todo = [];
        for (const hook of reset_hooks) {
            todo.push(hook());
        }

        for (const periph of memmap) {
            periph.reset && todo.push(periph.reset());
        }
        return Promise.all(todo);
    },
    init: init,

    pipeline: [if_stage, id_stage, ex_stage],

    memory_read: memory_read,
    memory_write: memory_write,
} as Core;