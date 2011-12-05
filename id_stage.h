#ifndef ID_STAGE_H
#define ID_STAGE_H

#include "common.h"

bool match_mask(uint32_t inst, uint32_t ones_mask, uint32_t zeros_mask);
struct op* find_op(uint32_t inst, bool reorder);
void tick_id(void);

#endif //ID_STAGE_H
