//------------------------------------------------------------------------------
// sync_instruction_test.c
//
// Branden Ghena (brghena@umich.edu)
// 07/25/2013
//
// Executes synchronization instructions. Verification of the instructions is
//  difficult, so simply running them to ensure that they don't cause a fault
//  is sufficient for the moment
//------------------------------------------------------------------------------

#include "testing.h"

/* dmb_test
 *
 * Executes the Data Memory Barrier instruction
 */
__asm void dmb_test( ) {

    // Just run it once to ensure that nothing freaks out...
    DMB

    // Return from function
    BX LR
}


/* dsb_test
 *
 * Executes the Data Synchronization Barrier instruction
 */
__asm void dsb_test( ) {

    // Just run it once to ensure that nothing freaks out...
    DSB

    // Return from function
    BX LR
}


/* isb_test
 *
 * Executes the Instruction Synchronization Barrier instruction
 */
__asm void isb_test( ) {

    // Just run it once to ensure that nothing freaks out...
    ISB

    // Return from function
    BX LR
}


/* sync_instruction_test
 *
 * Calls synchronization test functions
 */
void sync_instruction_test(void) {
    dmb_test();
    dsb_test();
    isb_test();
}

