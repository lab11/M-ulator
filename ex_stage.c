#include "ex_stage.h"
#include "pipeline.h"
#include "simulator.h"

#include "cpu/cpu.h"
#include "cpu/misc.h"

void tick_ex(void) {
	DBG2("start\n");

	// Execute
	if (in_ITblock()) {
		if (eval_cond(CORE_cpsr_read(), (read_itstate() & 0xf0) >> 4)) {
			if (printcycles) {
				printf("    P: %08d - 0x%08x : %04x (%s)\t%s\n",
						cycle, id_ex_PC, id_ex_inst, id_ex_o->name,
						"ITSTATE {executed}");
			}
			id_ex_o->fn(id_ex_inst);
		} else {
			if (printcycles) {
				printf("    P: %08d - 0x%08x : %04x (%s)\t%s\n",
						cycle, id_ex_PC, id_ex_inst, id_ex_o->name,
						"ITSTATE {skipped}");
			}
			//WARN("itstate skipped instruction\n");
		}
		IT_advance();
	} else {
		if (printcycles) {
			printf("    P: %08d - 0x%08x : %04x (%s)\n",
					cycle, id_ex_PC, id_ex_inst, id_ex_o->name);
		}
		id_ex_o->fn(id_ex_inst);
	}

	DBG2("end\n");
}
