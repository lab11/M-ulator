// TODO: Replace with full PPB tooling

import { Logger } from "tslog";
const log: Logger = new Logger({ name: "cpu::ram" });
log.silly("cpu::ram");

import { cpu } from "./module";

// This is a hack to export the one PPB entry that's necessary for startup (vector table location)
// The full PPB is more complex and needs to be ported yet.
let peripheral: cpu.MemmapEntry = {
    name: "PPB",
    start_address: 0xe000_ed08,
    end_address: 0xe000_ed08 + 4,
    read32: () => Promise.resolve(0x0),
    write32: () => Promise.reject(),
}

cpu.memmap.push(peripheral);