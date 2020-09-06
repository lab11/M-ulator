//------------------------------------------------------------------------------
// branch_instruction_test.c
//
// Branden Ghena (brghena@umich.edu)
// 07/15/2013
//
// Tests all branch instructions on the Cortex M0 processor
//------------------------------------------------------------------------------

#include "testing.h"

void branch_ledLoop(unsigned int);

/* bl_test
 *
 * Tests the Branch and Link instruction
 *
 * Note: this relies on the Push and Pop instructions which were previously
 *  tested
 */
__asm void bl_test( ) {

// Jump to nearby address
    PUSH{LR}
    BL bl_test_addr

    // Trap failed branching
bl_test_lr_addr
    B bl_test_led_lbl

    // Examine the LR (with a little bit of cheating...)
bl_test_addr
    PUSH {LR}
    POP  {R0}
    LDR  R1, =bl_test_lr_addr
    LDR  R2, =0xFFFFFFFC
    ANDS R1, R1, R2
    ANDS R0, R0, R2
    CMP  R0, R1
    BNE  bl_test_led_lbl

// Test Complete
    B bl_test_rtn_lbl

    // Failure
bl_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =branch_ledLoop
    BX  R1

    // Return from function
bl_test_rtn_lbl
    POP{PC}
}


/* blx_test
 *
 * Tests the Branch Exchange and Link instruction
 *
 * Note: this relies on the Push and Pop instructions which were previously
 *  tested
 */
__asm void blx_test( ) {

// Jump to nearby address
    PUSH{LR}
    LDR R0, =blx_test_addr
    BLX R0

    // Trap failed branching
blx_test_lr_addr
    B blx_test_led_lbl

    // Examine the LR (with a little bit of cheating...)
blx_test_addr
    PUSH {LR}
    POP  {R0}
    LDR  R1, =blx_test_lr_addr
    LDR  R2, =0xFFFFFFFC
    ANDS R1, R1, R2
    ANDS R0, R0, R2
    CMP  R0, R1
    BNE  blx_test_led_lbl

// Test Complete
    B blx_test_rtn_lbl

    // Failure
blx_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =branch_ledLoop
    BX  R1

    // Return from function
blx_test_rtn_lbl
    POP{PC}
}


/* bx_test
 *
 * Tests the Branch Exchange instruction
 *
 * Note: this relies on the Push and Pop instructions which were previously
 *  tested
 */
__asm void bx_test( ) {

// Jump to nearby address
    PUSH{LR}
    LDR R0, =bx_test_addr
    BX R0

    // Trap failed branching
bx_test_lr_addr
    B bx_test_led_lbl

    // Examine the LR (with a little bit of cheating...)
bx_test_addr
    PUSH {LR}
    POP  {R0}
    POP  {R1}
    CMP  R0, R1
    BNE  bx_test_led_lbl

// Test Complete
    B bx_test_rtn_lbl

    // Failure
bx_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =branch_ledLoop
    BX  R1

    // Return from function
bx_test_rtn_lbl
    BX LR
}


/* branch_instruction_test
 *
 * Calls all branch test functions
 */
void branch_instruction_test(void) {
    bl_test();
    blx_test();
    bx_test();
}


/* branch_ledLoop
 *
 * Assembly function which loops infinitely blinking
 *  an LED
 *
 * Inputs
 *   delay_count - number of iterations to wait between blinking
 */
__asm void branch_ledLoop(unsigned int delay_count){
// infinite loop
start

    // turn on the LED
    LDR R1, =LED_ON

    LDR R2, =0
loopA
    ADDS R2, #1
    CMP  R2, R0
    BNE  loopA

    // turn off the LED
    LDR R1, =LED_OFF

    LDR R2, =0
loopB
    ADDS R2, #1
    CMP  R2, R0
    BNE  loopB

    B start
}

