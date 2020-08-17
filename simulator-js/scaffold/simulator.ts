import { Logger } from "tslog";
const log: Logger = new Logger({ name: "simulator" });
log.silly("scaffold::simulator");

import { cpu } from "../cpu/module";
import { state_tock } from "./state";


export interface PipelineStage {
    tick(core: Core): Promise<any>,
    tock?(core: Core): Promise<any>,
}

// Define a pipeline as an Array of `PipelineStage`s.
export interface Pipeline extends Array<PipelineStage> { }

export interface Core {
    reset(): Promise<VoidFunction[]>,
    init(): void,
    pipeline: Pipeline,
    memory_read(address: number, width: number): Promise<number>,
    memory_write(address: number, width: number, value: number): Promise<any>,
}

async function simulator() {
    log.debug("Startup");

    // TODO: Load program image into flash

    // TODO: Populate opcode lookup table

    await power_on(cpu.core);

    log.debug("Entering main loop");
    // TODO: Integration point
    while (true) {
        // Prepare all hardware updates
        for (const stage of cpu.core.pipeline) {
            stage.tick(cpu.core);
        }

        // Most likely no-op, but hook is present if needed for advanced peripherals
        for (const stage of cpu.core.pipeline) {
            if (stage.tock) {
                stage.tock(cpu.core);
            }
        }
        // Write all hardware updates
        state_tock();
    }
}

/**
 * This function conceptually applies power to the core.
 */
async function power_on(core: Core) {
    await sim_reset(core);
}

/**
 * This function resets simulator state and asserts a reset interrupt to the core
 */
async function sim_reset(core: Core) {
    // TODO: Allow debugger hook?

    log.debug("Asserting reset");
    await core.reset();
}

simulator();