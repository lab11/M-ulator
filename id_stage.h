#ifndef ID_STAGE_H
#define ID_STAGE_H

#include "common.h"

struct op* find_op(uint32_t inst, bool reorder);
void tick_id(void);

#endif //ID_STAGE_H
