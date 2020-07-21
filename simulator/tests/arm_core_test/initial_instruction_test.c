//------------------------------------------------------------------------------
// initial_instruction_test.c
//
// Branden Ghena (brghena@umich.edu)
// 07/09/2013
//
// Tests the branch and compare instructions to ensure that further testing of
//  instructions is possible. Load has already been shown to work in the blink
//  test
//------------------------------------------------------------------------------

#include "testing.h"

void initial_ledLoop(unsigned int);

/* b_cond_test
 *
 * Assembly function which tests the branch conditional instruction by
 *  jumping to various addresses. This test assumes that the NZCV bits are
 *  initialized to 0. The LED lights if this test FAILS
 *
 * Note: this test relies on LDR and ADDS (with an immediate of 1), both of
 *  which have been shown to work in blink_test
 */
__asm void b_cond_test( ) {

    // 0x00000000 + 0x00000001 => NZCV = 0000
    LDR R0, =0
    ADDS R0, #1

    // Fall through branches
    BEQ b_cond_test_led_lbl
    BCS b_cond_test_led_lbl
    BMI b_cond_test_led_lbl
    BVS b_cond_test_led_lbl
    BHI b_cond_test_led_lbl
    BLT b_cond_test_led_lbl
    BLE b_cond_test_led_lbl

    // Taken branches
    BNE b_cond_test_bcc_lbl
    B   b_cond_test_led_lbl
b_cond_test_bcc_lbl
    BCC b_cond_test_bpl_lbl
    B   b_cond_test_led_lbl
b_cond_test_bpl_lbl
    BPL b_cond_test_bvc_lbl
    B   b_cond_test_led_lbl
b_cond_test_bvc_lbl
    BVC b_cond_test_bls_lbl
    B   b_cond_test_led_lbl
b_cond_test_bls_lbl
    BLS b_cond_test_bge_lbl
    B   b_cond_test_led_lbl
b_cond_test_bge_lbl
    BGE b_cond_test_bgt_lbl
    B   b_cond_test_led_lbl
b_cond_test_bgt_lbl
    BAL b_cond_test_rtn_lbl

    // Failure
b_cond_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    B   initial_ledLoop

    // Return from function
b_cond_test_rtn_lbl
    BX LR
}


/* cmp_test
 *
 * Tests the functionality of the compare command and stresses the branch
 *  conditional command by testing each possible condition code in both a
 *  taken and a fall-through state. This does not test every possible NZCV
 *  state, but rather every condition code suffix which is based on them
 */
__asm void cmp_test( ) {

cmp0010_lbl
    // NZCV = 0010
    LDR R0, =1
    LDR R1, =0
    CMP R0, R1

    // Fall through branches
    BEQ cmp_test_led_lbl
    BCC cmp_test_led_lbl
    BMI cmp_test_led_lbl
    BVS cmp_test_led_lbl
    BLS cmp_test_led_lbl
    BLT cmp_test_led_lbl
    BLE cmp_test_led_lbl

    // Taken branches
    BNE cmp0010_bcs_lbl
    B   cmp_test_led_lbl
cmp0010_bcs_lbl
    BCS cmp0010_bpl_lbl
    B   cmp_test_led_lbl
cmp0010_bpl_lbl
    BPL cmp0010_bvc_lbl
    B   cmp_test_led_lbl
cmp0010_bvc_lbl
    BVC cmp0010_bhi_lbl
    B   cmp_test_led_lbl
cmp0010_bhi_lbl
    BHI cmp0010_bge_lbl
    B   cmp_test_led_lbl
cmp0010_bge_lbl
    BGE cmp0010_bgt_lbl
    B   cmp_test_led_lbl
cmp0010_bgt_lbl
    BGT cmp0110_lbl
    B   cmp_test_led_lbl

cmp0110_lbl
    // NZCV = 0110
    LDR R0, =0
    LDR R1, =0
    CMP R0, R1

    // Fall through branches
    BNE cmp_test_led_lbl
    BCC cmp_test_led_lbl
    BMI cmp_test_led_lbl
    BVS cmp_test_led_lbl
    BHI cmp_test_led_lbl
    BLT cmp_test_led_lbl
    BGT cmp_test_led_lbl

    // Taken branches
    BEQ cmp0110_bcs_lbl
    B   cmp_test_led_lbl
cmp0110_bcs_lbl
    BCS cmp0110_bpl_lbl
    B   cmp_test_led_lbl
cmp0110_bpl_lbl
    BPL cmp0110_bvc_lbl
    B   cmp_test_led_lbl
cmp0110_bvc_lbl
    BVC cmp0110_bls_lbl
    B   cmp_test_led_lbl
cmp0110_bls_lbl
    BLS cmp0110_bge_lbl
    B   cmp_test_led_lbl
cmp0110_bge_lbl
    BGE cmp0110_ble_lbl
    B   cmp_test_led_lbl
cmp0110_ble_lbl
    BLE cmp1000_lbl
    B   cmp_test_led_lbl

cmp1000_lbl
    // NZCV = 1000
    LDR R0, =0
    LDR R1, =1
    CMP R0, R1

    // Fall through branches
    BEQ cmp_test_led_lbl
    BCS cmp_test_led_lbl
    BPL cmp_test_led_lbl
    BVS cmp_test_led_lbl
    BHI cmp_test_led_lbl
    BGE cmp_test_led_lbl
    BGT cmp_test_led_lbl

    // Taken branches
    BNE cmp1000_bcc_lbl
    B   cmp_test_led_lbl
cmp1000_bcc_lbl
    BCC cmp1000_bmi_lbl
    B   cmp_test_led_lbl
cmp1000_bmi_lbl
    BMI cmp1000_bvc_lbl
    B   cmp_test_led_lbl
cmp1000_bvc_lbl
    BVC cmp1000_bls_lbl
    B   cmp_test_led_lbl
cmp1000_bls_lbl
    BLS cmp1000_blt_lbl
    B   cmp_test_led_lbl
cmp1000_blt_lbl
    BLT cmp1000_ble_lbl
    B   cmp_test_led_lbl
cmp1000_ble_lbl
    BLE cmp1001_lbl
    B   cmp_test_led_lbl

cmp1001_lbl
    // NZCV = 1001
    LDR R0, =0
    LDR R1, =0x80000000
    CMP R0, R1

    // Fall through branches
    BEQ cmp_test_led_lbl
    BCS cmp_test_led_lbl
    BPL cmp_test_led_lbl
    BVC cmp_test_led_lbl
    BHI cmp_test_led_lbl
    BLT cmp_test_led_lbl
    BLE cmp_test_led_lbl

    // Taken branches
    BNE cmp1001_bcc_lbl
    B   cmp_test_led_lbl
cmp1001_bcc_lbl
    BCC cmp1001_bmi_lbl
    B   cmp_test_led_lbl
cmp1001_bmi_lbl
    BMI cmp1001_bvs_lbl
    B   cmp_test_led_lbl
cmp1001_bvs_lbl
    BVS cmp1001_bls_lbl
    B   cmp_test_led_lbl
cmp1001_bls_lbl
    BLS cmp1001_bge_lbl
    B   cmp_test_led_lbl
cmp1001_bge_lbl
    BGE cmp1001_bgt_lbl
    B   cmp_test_led_lbl
cmp1001_bgt_lbl
    BGT cmp0000_lbl
    B   cmp_test_led_lbl

cmp0000_lbl
    // NZCV = 0000
    LDR R0, =0
    LDR R1, =-1
    CMP R0, R1

    // Fall through branches
    BEQ cmp_test_led_lbl
    BCS cmp_test_led_lbl
    BMI cmp_test_led_lbl
    BVS cmp_test_led_lbl
    BHI cmp_test_led_lbl
    BLT cmp_test_led_lbl
    BLE cmp_test_led_lbl

    // Taken branches
    BNE cmp0000_bcc_lbl
    B   cmp_test_led_lbl
cmp0000_bcc_lbl
    BCC cmp0000_bpl_lbl
    B   cmp_test_led_lbl
cmp0000_bpl_lbl
    BPL cmp0000_bvc_lbl
    B   cmp_test_led_lbl
cmp0000_bvc_lbl
    BVC cmp0000_bls_lbl
    B   cmp_test_led_lbl
cmp0000_bls_lbl
    BLS cmp0000_bge_lbl
    B   cmp_test_led_lbl
cmp0000_bge_lbl
    BGE cmp0000_bgt_lbl
    B   cmp_test_led_lbl
cmp0000_bgt_lbl
    BGT cmp_test_rtn_lbl
    B   cmp_test_led_lbl

    // Failure
cmp_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    B   initial_ledLoop

    // Return from function
cmp_test_rtn_lbl
    BX LR
}


/* add_test
 *
 * Tests the Add instruction
 */
__asm void add_test( ) {

// Test additions
    // 0 + 1 = 1
    LDR R0, =0
    LDR R1, =1
    LDR R2, =1
    ADD R0, R0, R1
    CMP R0, R2
    BNE add_test_led_lbl

    // 0x7FFFFFFF + 1 = 0x80000000
    LDR R0, =0x7FFFFFFF
    LDR R1, =1
    LDR R2, =0x80000000
    ADD R0, R0, R1
    CMP R0, R2
    BNE add_test_led_lbl

// Test condition flags
    // Set NZCV bits (NZCV = 0110)
    LDR R0, =0
    LDR R1, =0
    CMP R0, R1

    // 0x7FFFFFFF + 1 => should not affect NZCV
    LDR R0, =0x7FFFFFFF
    LDR R1, =1
    ADD R0, R0, R1
    BMI add_test_led_lbl
    BNE add_test_led_lbl
    BCC add_test_led_lbl
    BVS add_test_led_lbl

// Test Complete
    B add_test_rtn_lbl

    // Failure
add_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =initial_ledLoop
    BX  R1

    // Return from function
add_test_rtn_lbl
    BX LR
}


/* register_test
 *
 * Tests all General Purpose Registers on the processor (R0-R12)
 */
__asm void register_test( ) {
/*
// Test registers
    // Write zeros
    LDR R0, =0x00000000
    ADD R1,  R0, R0
    ADD R2,  R0, R0
    ADD R3,  R0, R0
    ADD R4,  R0, R0
    ADD R5,  R0, R0
    ADD R6,  R0, R0
    ADD R7,  R0, R0
    ADD R8,  R0, R0
    ADD R9,  R0, R0
    ADD R10, R0, R0
    ADD R11, R0, R0
    ADD R12, R0, R0

    // Check results
    CMP R0, R1
    BNE register_test_led_lbl
    CMP R0, R2
    BNE register_test_led_lbl
    CMP R0, R3
    BNE register_test_led_lbl
    CMP R0, R4
    BNE register_test_led_lbl
    CMP R0, R5
    BNE register_test_led_lbl
    CMP R0, R6
    BNE register_test_led_lbl
    CMP R0, R7
    BNE register_test_led_lbl
    ADD R1, R8, R0
    CMP R0, R1
    BNE register_test_led_lbl
    ADD R1, R9, R0
    CMP R0, R1
    BNE register_test_led_lbl
    ADD R1, R10, R0
    CMP R0, R1
    BNE register_test_led_lbl
    ADD R1, R11, R0
    CMP R0, R1
    BNE register_test_led_lbl
    ADD R1, R12, R0
    CMP R0, R1
    BNE register_test_led_lbl

    // Write ones
    LDR R0, =0xFFFFFFFF
    LDR R1, =0x00000000
    ADD R2,  R0, R1
    ADD R3,  R0, R1
    ADD R4,  R0, R1
    ADD R5,  R0, R1
    ADD R6,  R0, R1
    ADD R7,  R0, R1
    ADD R8,  R0, R1
    ADD R9,  R0, R1
    ADD R10, R0, R1
    ADD R11, R0, R1
    ADD R12, R0, R1
    ADD R1,  R0, R1

    // Check results
    CMP R0, R1
    BNE register_test_led_lbl
    CMP R0, R2
    BNE register_test_led_lbl
    CMP R0, R3
    BNE register_test_led_lbl
    CMP R0, R4
    BNE register_test_led_lbl
    CMP R0, R5
    BNE register_test_led_lbl
    CMP R0, R6
    BNE register_test_led_lbl
    CMP R0, R7
    BNE register_test_led_lbl
    LDR R2, =0x00000000
    ADD R1, R8, R2
    CMP R0, R1
    BNE register_test_led_lbl
    ADD R1, R9, R2
    CMP R0, R1
    BNE register_test_led_lbl
    ADD R1, R10, R2
    CMP R0, R1
    BNE register_test_led_lbl
    ADD R1, R11, R2
    CMP R0, R1
    BNE register_test_led_lbl
    ADD R1, R12, R2
    CMP R0, R1
    BNE register_test_led_lbl

    // Test pattern
    LDR R0, =0xA5A5A5A5
    LDR R1, =0x00000000
    ADD R2,  R0, R1
    ADD R3,  R0, R1
    ADD R4,  R0, R1
    ADD R5,  R0, R1
    ADD R6,  R0, R1
    ADD R7,  R0, R1
    ADD R8,  R0, R1
    ADD R9,  R0, R1
    ADD R10, R0, R1
    ADD R11, R0, R1
    ADD R12, R0, R1
    ADD R1,  R0, R1

    // Check results
    CMP R0, R1
    BNE register_test_led_lbl
    CMP R0, R2
    BNE register_test_led_lbl
    CMP R0, R3
    BNE register_test_led_lbl
    CMP R0, R4
    BNE register_test_led_lbl
    CMP R0, R5
    BNE register_test_led_lbl
    CMP R0, R6
    BNE register_test_led_lbl
    CMP R0, R7
    BNE register_test_led_lbl
    LDR R2, =0x00000000
    ADD R1, R8, R2
    CMP R0, R1
    BNE register_test_led_lbl
    ADD R1, R9, R2
    CMP R0, R1
    BNE register_test_led_lbl
    ADD R1, R10, R2
    CMP R0, R1
    BNE register_test_led_lbl
    ADD R1, R11, R2
    CMP R0, R1
    BNE register_test_led_lbl
    ADD R1, R12, R2
    CMP R0, R1
    BNE register_test_led_lbl

// Test Complete
    B register_test_rtn_lbl

    // Failure
register_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =initial_ledLoop
    BX  R1

    // Return from function
register_test_rtn_lbl
    BX LR
*/
}


/* initial_instruction_test
 *
 * Calls all initial test functions
 */
void initial_instruction_test(void) {
    b_cond_test();
    cmp_test();
    add_test();
}


/* initial_ledLoop
 *
 * Assembly function which loops infinitely blinking
 *  an LED
 *
 * Inputs
 *   delay_count - number of iterations to wait between blinking
 */
__asm void initial_ledLoop(unsigned int delay_count){
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

