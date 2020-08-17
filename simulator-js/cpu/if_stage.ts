import { Logger } from "tslog";
const log: Logger = new Logger({ name: "cpu::if_stage" });
log.silly("cpu::if_stage");

import opcodes = require("../scaffold/opcodes");
import simulator = require("../scaffold/simulator");
import { State } from "../scaffold/state";
import cpu = require("./cpu");
import registers = require("./registers");

// Branch Target Forwarding
// Currently, no speculative branching,
// but we can make unconditional branches quick
function branch_target_forward32(target: number, inst: number, pc: number): number {
    if (opcodes.match_mask32(inst, 0xf0009000, 0x08004000)) {
        log.silly("btf: b_t4\n");
        // This is b_t4
        let imm11 = inst & 0x7ff;
        let J2 = !!(inst & 0x800);
        let J1 = !!(inst & 0x2000);
        let imm10 = (inst >> 16) & 0x3ff;
        let S = !!(inst & 0x04000000);

        let I1 = !(J1 != S) ? 1 : 0;
        let I2 = !(J2 != S) ? 1 : 0;
        let imm32 = (I1 << 23) | (I2 << 22) | (imm10 << 12) | (imm11 << 1);
        if (S) {
            imm32 |= 0xff000000;
        }

        pc = target + imm32;
        return pc;
    }

    if (opcodes.match_mask32(inst, 0xf000d000, 0x08000000)) {
        log.silly("btf: bl_t1\n");
        // This is a bl_t1
        let imm11 = inst & 0x7ff;
        let J2 = !!(inst & 0x800);
        let J1 = !!(inst & 0x2000);
        let imm10 = (inst >> 16) & 0x3ff;
        let S = !!(inst & 0x0400000);

        let I1 = !(J1 != S) ? 1 : 0;
        let I2 = !(J2 != S) ? 1 : 0;
        let imm32 = (I1 << 23) | (I2 << 22) | (imm10 << 12) | (imm11 << 1);

        if (S) {
            imm32 |= 0xff000000;
        }

        pc = target + imm32;
        return pc;
    }

    return pc;
}

function branch_target_forward16(target: number, inst: number, pc: number): number {
    if (opcodes.match_mask32(inst, 0xe000, 0x1000)) {
        log.silly("btf: b_t2\n");
        // This is b_t2
        let imm11 = inst & 0x7ff;
        let imm32 = imm11 << 1;
        if (imm32 & 0x800) {
            imm32 |= 0xfffff000;
        }

        pc = target + imm32;
        return pc;
    }

    if (opcodes.match_mask32(inst, 0x4700, 0xb880)) {
        log.silly("btf: bx_t1\n");
        let rm = (inst >> 3) & 0xf;

        pc = registers.CORE_reg_read(rm) & 0xfffffffe;
        return pc;
    }

    return pc;
}




export let if_stage = {
    last_pc: new State(0xffff_ffff),

    tick: async function (core: simulator.Core) {
        let pc = cpu.pipeline_registers.pre_if_PC.read();

        // Poor man's pipeline hazard
        if (pc > 0xf000_0000) {
            log.warn("pipeline hazard might not work right yet");
            cpu.pipeline_registers.if_id_PC.write(cpu.HAZARD_PC);
            cpu.pipeline_registers.if_id_inst.write(cpu.INST_HAZARD);
            cpu.pipeline_registers.pre_if_PC.write(cpu.HAZARD_PC);
            return;
        }

        let epsr = registers.CORE_epsr_read();
        // FIXME bit
        if (epsr & 0x300000) {
            log.debug("Reading thumb mode instruction");
            let inst = await core.memory_read(pc, 2);

            // If inst[15:11] are any of
            // 11101, 11110, or 11111 then this is
            // a 32-bit thumb inst
            switch (inst & 0xf800) {
                case 0xe800:
                case 0xf000:
                case 0xf800:
                    {
                        this.last_pc.write(pc);
                        pc = pc + 2;

                        inst <<= 16;
                        inst |= await core.memory_read(pc, 2);
                        pc = pc + 2;

                        pc = branch_target_forward32(cpu.pipeline_registers.pre_if_PC.read() + 4, inst, pc);

                        break;
                    }
                default:
                    // 16-bit thumb inst
                    this.last_pc.write(pc);
                    pc = pc + 2;

                    pc = branch_target_forward16(cpu.pipeline_registers.pre_if_PC.read() + 4, inst, pc);
            }

            // A5.1.2 p153
            // use of 0b1111 as a register specifier
            // reading PC must *always* return inst addr + 4
            cpu.pipeline_registers.if_id_PC.write(cpu.pipeline_registers.pre_if_PC.read() + 4);
            cpu.pipeline_registers.if_id_inst.write(inst);
            cpu.pipeline_registers.pre_if_PC.write(pc);

        }
    },
    tock: function (_core: simulator.Core) { return Promise.resolve() }
}