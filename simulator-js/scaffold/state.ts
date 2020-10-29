import { Logger } from "tslog";
import { Op16Function, Op32Function } from "./opcodes";
const log: Logger = new Logger({ name: "core::state" });
log.silly("scaffold::state");

let work_list: [State | StateNonNumeric, any][] = [];


type ResetFunctionType = (old_value: number) => Promise<number>;

// Interceptor layer for hardware state
//
// Reads return current state, writes are pended until `tock` event.
export class State {
    static hardware: Array<State>;

    reset_value: number;
    reset_fn?: ResetFunctionType;
    value: number;

    constructor(value: number, reset_fn?: ResetFunctionType) {
        this.value = value;
        this.reset_value = value;
        if (reset_fn) {
            this.reset_fn = reset_fn;
        }

        // Hold a reference to all hardware state for resets
        if (!(State.hardware)) {
            State.hardware = [];
        }
        State.hardware.push(this);
    }

    read() {
        return this.value;
    }

    write(value: number) {
        work_list.push([this, value]);
    }

    // TODO: This is different; sync or async write?
    static async reset() {
        for (const hw of State.hardware) {
            if (hw.reset_fn) {
                hw.value = await hw.reset_fn(hw.value);
            } else {
                hw.value = hw.reset_value;
            }
        }
    }
}

// A convenience class for things that should act like hardware registers conceptually, but
// are richer objects than just plain numbers
export class StateNonNumeric {
    static hardware: Array<StateNonNumeric>;

    value: Op16Function | Op32Function | null;
    reset_fn?: ResetFunctionType;
    reset_value: Op16Function | Op32Function | null;

    constructor(value: Op16Function | Op32Function | null, reset_fn?: ResetFunctionType) {
        this.value = value;
        this.reset_value = value;
        if (reset_fn) {
            this.reset_fn = reset_fn;
        }

        // Hold a reference to all hardware state for resets
        if (!(StateNonNumeric.hardware)) {
            StateNonNumeric.hardware = [];
        }
        StateNonNumeric.hardware.push(this);
    }

    read() {
        return this.value;
    }

    write(value: number) {
        work_list.push([this, value]);
    }

    // TODO: This is different; sync or async write?
    static async reset() {
        for (const hw of State.hardware) {
            if (hw.reset_fn) {
                hw.value = await hw.reset_fn(hw.value);
            } else {
                hw.value = hw.reset_value;
            }
        }
    }
}

export function state_exception() {
    return;
}

export function state_tock() {
    for (const write of work_list) {
        write[0].value = write[1];
    }

    work_list = [];
}

/*
function read(obj: object, prop: string): any {
    return obj[prop];
}

function write(obj: object, prop: string, value: any) {
    obj[prop] = value;
}

export let state = {
    read: read,
    write: write,
}
*/