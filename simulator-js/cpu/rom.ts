// Peripheral that acts as basic, RO memory

import { Logger } from "tslog";
const log: Logger = new Logger({ name: "cpu::ram" });
log.silly("cpu::rom");

import { cpu } from "./module";

function create(start_address: number, length: number): cpu.MemmapEntry {
    let memory = new ArrayBuffer(length);
    const mem32 = new Int32Array(memory);

    function read32(address: number) {
        let index = address - start_address;
        if (index < length) {
            return Promise.resolve(mem32[index]);
        } else {
            return Promise.reject(new cpu.SynchronousException("ROM read"));
        }
    }

    function write32(_address: number, _value: number) {
        return Promise.reject(new cpu.SynchronousException("ROM write"));
    }

    let peripheral = {
        name: "ROM",
        start_address: start_address,
        end_address: start_address + length,
        read32: read32,
        write32: write32,
    }

    return peripheral;
}

cpu.memmap.push(create(0x0000_0000, 0x8000));