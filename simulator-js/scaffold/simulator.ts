import { Logger } from "tslog";
const log: Logger = new Logger({ name: "simulator" });
log.silly("scaffold::simulator");

import { cpu } from "../cpu/module";
import { state_tock, state_exception } from "./state";


export interface PipelineStage {
    name: string,
    tick(core: Core): Promise<void>,
    exception?(core: Core): Promise<void>,
    tock?(core: Core): Promise<void>,
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
        log.silly("tick");
        let ticks = [];
        for (const stage of cpu.core.pipeline) {
            ticks.push(stage.tick(cpu.core));
        }
        await Promise.allSettled(ticks);

        if (cpu.pending_exception()) {
            log.silly("excpetion");
            // Most likely no-op, but hook is present if needed for advanced peripherals
            let exceptions = [];
            for (const stage of cpu.core.pipeline) {
                if (stage.exception) {
                    exceptions.push(stage.exception(cpu.core));
                }
            }
            await Promise.allSettled(exceptions);
            state_exception();
        }

        log.silly("tock");
        // Most likely no-op, but hook is present if needed for advanced peripherals
        let tocks = [];
        for (const stage of cpu.core.pipeline) {
            if (stage.tock) {
                tocks.push(stage.tock(cpu.core));
            }
        }
        await Promise.allSettled(tocks);
        // Write all hardware updates
        state_tock();
    }
}

/**
 * This function conceptually applies power to the core.
 */
async function power_on(core: Core) {
    log.debug("Power On");
    await sim_reset(core);
}

/**
 * This function resets simulator state and asserts a reset interrupt to the core
 */
async function sim_reset(core: Core) {
    // TODO: Allow debugger hook?

    log.debug("Asserting reset");
    await core.reset();
    log.debug("De-asserting reset");
}

simulator();