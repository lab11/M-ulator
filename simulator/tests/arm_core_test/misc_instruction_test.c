//------------------------------------------------------------------------------
// misc_instruction_test.c
//
// Branden Ghena (brghena@umich.edu)
// 07/15/2013
//
// Tests all miscellaneous instructions on the Cortex M0 processor
//------------------------------------------------------------------------------

#include "testing.h"

void misc_ledLoop(unsigned int);

/* mrs_msr_test1
 *
 * Tests the Move to/from Special to/from Register instructions
 */
__asm void mrs_msr_test1( ) {

// Read special registers
    // Interrupt Program Status Register, zero outside of exceptions
    LDR R2, =0x00000000
    LDR R7, =0xFFFFFFFF
    // Read
    MRS R0, IPSR
    CMP R0, R2
    BNE mrs_msr_test1_led_lbl
    // Write, does nothing
    MSR IPSR, R7
    MRS R0, IPSR
    CMP R0, R2
    BNE mrs_msr_test1_led_lbl

    // Execution Program Status Register, always zero
    // Read
    MRS R0, EPSR
    MRS R1, IEPSR
    CMP R0, R2
    BNE mrs_msr_test1_led_lbl
    CMP R1, R2
    BNE mrs_msr_test1_led_lbl
    // Write, does nothing
    MSR EPSR, R7
    MRS R0, EPSR
    CMP R0, R2
    BNE mrs_msr_test1_led_lbl

    // Priority Mask Register
    // Read
    MRS R0, PRIMASK
    CMP R0, R2
    BNE mrs_msr_test1_led_lbl
    // Write
    LDR R7, =0x00000001
    MSR PRIMASK, R7
    MRS R0, PRIMASK
    CMP R0, R7
    BNE mrs_msr_test1_led_lbl
    // Reset
    MSR PRIMASK, R2
    MRS R0, PRIMASK
    CMP R0, R2
    BNE mrs_msr_test1_led_lbl

    // Application Program Status Register, NZCV bits
    // NZCV = 0000
    LDR R3, =-1
    CMP R2, R3
    MRS R0, APSR
    MRS R4, IAPSR
    MRS R5, EAPSR
    MRS R6, PSR
    LDR R1, =0x00000000
    CMP R0, R1
    BNE mrs_msr_test1_led_lbl
    CMP R4, R1
    BNE mrs_msr_test1_led_lbl
    CMP R5, R1
    BNE mrs_msr_test1_led_lbl
    CMP R6, R1
    BNE mrs_msr_test1_led_lbl
    // NZCV = 0010
    LDR R3, =1
    CMP R3, R2
    MRS R0, APSR
    MRS R4, IAPSR
    MRS R5, EAPSR
    MRS R6, PSR
    LDR R1, =0x20000000
    CMP R0, R1
    BNE mrs_msr_test1_led_lbl
    CMP R4, R1
    BNE mrs_msr_test1_led_lbl
    CMP R5, R1
    BNE mrs_msr_test1_led_lbl
    CMP R6, R1
    BNE mrs_msr_test1_led_lbl
    // NZCV = 0110
    CMP R2, R2
    MRS R0, APSR
    MRS R4, IAPSR
    MRS R5, EAPSR
    MRS R6, PSR
    LDR R1, =0x60000000
    CMP R0, R1
    BNE mrs_msr_test1_led_lbl
    CMP R4, R1
    BNE mrs_msr_test1_led_lbl
    CMP R5, R1
    BNE mrs_msr_test1_led_lbl
    CMP R6, R1
    BNE mrs_msr_test1_led_lbl
    // NZCV = 1000
    CMP R2, R3
    MRS R0, APSR
    MRS R4, IAPSR
    MRS R5, EAPSR
    MRS R6, PSR
    LDR R1, =0x80000000
    CMP R0, R1
    BNE mrs_msr_test1_led_lbl
    CMP R4, R1
    BNE mrs_msr_test1_led_lbl
    CMP R5, R1
    BNE mrs_msr_test1_led_lbl
    CMP R6, R1
    BNE mrs_msr_test1_led_lbl
    // Write
    LDR R7, =0xF0000000
    MSR APSR, R7
    BPL mrs_msr_test1_led_lbl
    BNE mrs_msr_test1_led_lbl
    BCC mrs_msr_test1_led_lbl
    BVC mrs_msr_test1_led_lbl
    MRS R0, APSR
    CMP R0, R7
    BNE mrs_msr_test1_led_lbl

// Test Complete
    B mrs_msr_test1_rtn_lbl

    // Failure
mrs_msr_test1_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =misc_ledLoop
    BX  R1

    // Return from function
mrs_msr_test1_rtn_lbl
    BX LR
}


/* mrs_msr_test2
 *
 * Tests the Move to/from Special to/from Register instructions
 */
__asm void mrs_msr_test2( ) {

    // Control Register
    // Read
    LDR R2, =0x00000000
    MRS R0, CONTROL
    CMP R0, R2
    BNE mrs_msr_test2_led_lbl
    // Write
    LDR R1, =0x00000002
    MSR CONTROL, R1
    MRS R0, CONTROL
    CMP R0, R1
    BNE mrs_msr_test2_led_lbl
    // Reset
    MSR CONTROL, R2
    MRS R0, CONTROL
    CMP R0, R2
    BNE mrs_msr_test2_led_lbl

    // Main Stack Pointer
    // Read
    ADD R2, R2, SP
    MRS R1, MSP
    CMP R1, R2
    BNE mrs_msr_test2_led_lbl
    // Write
    LDR R3, =0xDEADBEEC
    MSR MSP, R3
    LDR R2, =0x00000000
    ADD R2, R2, SP
    CMP R2, R3
    BNE mrs_msr_test2_led_lbl
    // Reset
    MSR MSP, R1
    LDR R2, =0x00000000
    ADD R2, R2, SP
    CMP R1, R2
    BNE mrs_msr_test2_led_lbl

    // Process Stack Pointer
    // Write
    LDR R2, =0x00000000
    MSR PSP, R2
    MRS R0, PSP
    CMP R0, R2
    BNE mrs_msr_test2_led_lbl
    ADD R2, R2, SP
    CMP R0, R2
    BEQ mrs_msr_test2_led_lbl
    // Change Control register
    LDR R1, =0x00000002
    MSR CONTROL, R1
    MRS R0, CONTROL
    CMP R0, R1
    BNE mrs_msr_test2_led_lbl
    // Retry
    LDR R2, =0x00000000
    ADD R2, R2, SP
    MRS R0, PSP
    CMP R0, R2
    BNE mrs_msr_test2_led_lbl
    // Read
    LDR R1, =0x00000004
    ADD SP, SP, R1
    MRS R0, PSP
    CMP R0, R1
    BNE mrs_msr_test2_led_lbl
    // Reset
    LDR R2, =0x00000000
    MSR PSP, R2
    MRS R0, PSP
    CMP R0, R2
    BNE mrs_msr_test2_led_lbl
    MSR CONTROL, R2
    MRS R0, CONTROL
    CMP R0, R2
    BNE mrs_msr_test2_led_lbl

// Test Complete
    B mrs_msr_test2_rtn_lbl

    // Failure
mrs_msr_test2_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =misc_ledLoop
    BX  R1

    // Return from function
mrs_msr_test2_rtn_lbl
    BX LR
}

/* nop_test
 *
 * Tests the No Operation instruction
 */
__asm void nop_test( ) {

    // Just run it once to ensure that nothing freaks out...
    NOP

// Test Complete
    B nop_test_rtn_lbl

    // Failure
nop_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =misc_ledLoop
    BX  R1

    // Return from function
nop_test_rtn_lbl
    BX LR
}


/* sev_test
 *
 * Tests the Send Event instruction
 */
__asm void sev_test( ) {

    // Just run it once to ensure that nothing freaks out...
    SEV

// Test Complete
    B sev_test_rtn_lbl

    // Failure
sev_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =misc_ledLoop
    BX  R1

    // Return from function
sev_test_rtn_lbl
    BX LR
}


/* misc_instruction_test
 *
 * Calls all miscellaneous test functions
 */
void misc_instruction_test(void) {
    mrs_msr_test1();
    mrs_msr_test2();
    nop_test();
    sev_test();
}


/* misc_ledLoop
 *
 * Assembly function which loops infinitely blinking
 *  an LED
 *
 * Inputs
 *   delay_count - number of iterations to wait between blinking
 */
__asm void misc_ledLoop(unsigned int delay_count){
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

