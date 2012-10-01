#ifndef DIRECT_STATE_H_CHECK
#error "Do not #include state.h directly, rather use state_sync or state_async"
#endif

#ifndef STATE_H
#define STATE_H

#include "common.h"

#ifndef PP_STRING
#define PP_STRING "ST-"
#include "pretty_print.h"
#endif

enum stage {
	PRE  = 0x1,
	IF   = 0x2,
	ID   = 0x4,
	EX   = 0x8,
	PIPE = 0x10,
	SIM  = 0x20,
	PERIPH = 0x40,
	UNK  = 0x80,
	/* MAX: 0xff */
};

#ifndef STAGE
#pragma message "Assigning UNK stage"
#define STAGE UNK
#endif

// Not 100% sure this is appropriate here, but no callers of this yet
// XXX: Moved in core refactor - consider final placement
void stall(enum stage);

bool state_is_debugging(void);
void state_start_tick(void);
void state_tock(void);
#ifndef NO_PIPELINE
void state_pipeline_flush(uint32_t new_pc);
#endif

void state_async_block_start(void);
void state_async_block_end(void);

void state_enter_debugging(void);
void state_exit_debugging(void);

#endif // STATE_H
