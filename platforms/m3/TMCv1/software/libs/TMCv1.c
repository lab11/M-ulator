//*******************************************************************************************
// TMC SOURCE FILE
//-------------------------------------------------------------------------------------------
// SUB-LAYER CONNECTION:
//      PREv22E -> SNTv5 -> EIDv1 -> MRRv11A -> MEMv3 -> PMUv13
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Jun 22 2021 -   First commit 
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//*********************************************************
// HEADER AND OTHER SOURCE FILES
//*********************************************************
#include "TMCv1.h"


//*******************************************************************
// TMCv1 FUNCTIONS
//*******************************************************************
// SEE HEADER FILE FOR FUNCTION BRIEFS


//-------------------------------------------------------------------
// Basic Building Functions
//-------------------------------------------------------------------

uint32_t set_bits (uint32_t original_var, uint32_t msb_idx, uint32_t lsb_idx, uint32_t value) {
    uint32_t mask_pattern = (0xFFFFFFFF << (msb_idx + 1)) | (0xFFFFFFFF >> (32 - lsb_idx));
    return (original_var & mask_pattern) | ((value << lsb_idx) & ~mask_pattern);
}

uint32_t get_bits (uint32_t variable, uint32_t msb_idx, uint32_t lsb_idx) {
    return ((variable << (31-msb_idx)) >> (31-msb_idx)) >> lsb_idx;
}

uint32_t get_bit (uint32_t variable, uint32_t idx) {
    return get_bits(variable, idx, idx);
}
