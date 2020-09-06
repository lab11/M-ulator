//------------------------------------------------------------------------------
// main.c
//
// Branden Ghena (brghena@umich.edu)
// 07/10/2013
//
// Entry point for Cortex M0 processor testing. Note that these test functions
//  do _NOT_ respect the ARM Architecture Procedure Call Standard (AAPCS) and
//  may clobber any register values at any time although LR and SP are generally
//  respected
//------------------------------------------------------------------------------

#include "testing.h"

/* main
 *
 * Entry point for code. Calls test functions
 */
int main(void) {
    //initial_instruction_test();
    alu_instruction_test();
    //memory_instruction_test();
    //branch_instruction_test();
    //misc_instruction_test();
    //sync_instruction_test();

    return 0;
}

