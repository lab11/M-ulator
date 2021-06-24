import { Logger } from "tslog";
const log: Logger = new Logger({ name: "cpu::id_stage" });
log.silly("cpu::id_stage");

import opcodes = require("../scaffold/opcodes");
import simulator = require("../scaffold/simulator");
import cpu = require("./cpu");


let id_stage = {
    name: "Decode",

    tick: async function (core: simulator.Core): Promise<void> {
        let inst = cpu.pipeline_registers.if_id_inst.read();

        let op = opcodes.find_op(inst);

        cpu.pipeline_registers.id_ex_PC.write(cpu.pipeline_registers.if_id_PC.read());
        cpu.pipeline_registers.id_ex_inst.write(inst);
        cpu.pipeline_registers.id_ex_op.write(op);
    },

    exception: async function (core: simulator.Core): Promise<void> {
        cpu.pipeline_registers.id_ex_PC.write(cpu.STALL_PC);
        cpu.pipeline_registers.id_ex_inst.write(cpu.INST_NOP);
        cpu.pipeline_registers.id_ex_op.write(opcodes.find_op(cpu.INST_NOP));
    },

    tock: function (_core: simulator.Core) { return Promise.resolve() }
}

cpu.pipeline.push(id_stage);