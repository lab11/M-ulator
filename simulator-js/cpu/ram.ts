// Peripheral that acts as basic, R/W memory

import { Logger } from "tslog";
const log: Logger = new Logger({ name: "cpu::ram" });
log.silly("cpu::ram");

import { cpu } from "./module";

function create(start_address: number, length: number): cpu.MemmapEntry {
    let memory = new ArrayBuffer(length);
    const mem32 = new Int32Array(memory);

    function read32(address: number) {
        let index = address - start_address;
        if (index < length) {
            return Promise.resolve(mem32[index]);
        } else {
            return Promise.reject();
        }
    }

    function write32(address: number, value: number) {
        let index = address - start_address;
        if (index < length) {
            let writer_fn = function () { mem32[index] = value };
            return Promise.resolve(writer_fn);
        } else {
            return Promise.reject();
        }
    }

    let peripheral = {
        name: "RAM",
        start_address: start_address,
        end_address: start_address + length,
        read32: read32,
        write32: write32,
    }

    return peripheral;
}

cpu.memmap.push(create(0x2000_0000, 0x4000));