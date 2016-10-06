#ifndef _DNN_PARAMETERS_H
#define _DNN_PARAMETERS_H
#include <stdint.h>
extern uint32_t PE_INSTS[2][4][6];
void set_dnn_insts();
#define HAS_NLI
extern uint32_t NLI_X[1][4][11];
extern uint32_t NLI_A[1][4][10];
extern uint32_t NLI_B[1][4][10];

void set_nli_parameters();
#endif
