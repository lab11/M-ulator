//------------------------------------------------------------------------------
// memory_instruction_test.c
//
// Branden Ghena (brghena@umich.edu)
// 07/15/2013
//
// Tests all memory instructions on the Cortex M0 processor
//------------------------------------------------------------------------------

#include "testing.h"

void memory_ledLoop(unsigned int);

/* adr_test
 *
 * Tests the PC-Relative Address instruction
 */
__asm void adr_test( ) {

// Generate a PC-relative address
    LDR  R2, =adr_test_led_lbl
    LDR  R1, =0xFFFFFFFE
    ANDS R2, R2, R1
    ADR  R0, adr_test_led_lbl
    CMP  R0, R2
    BNE  adr_test_led_lbl

// Test Complete
    B adr_test_rtn_lbl

    // Nop to align adr_test_led_lbl to word boundary
    B adr_test_led_lbl

    // Failure
adr_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
adr_test_rtn_lbl
    BX LR
}


/* str_ldr_test
 *
 * Tests the Load Word and Store Word instructions
 */
__asm void str_ldr_test( ) {

// Load and Store words to test each individual bit of HRDATA and HWDATA
    LDR  R0, =STACK_TOP
    MOVS R1, #0
    MOVS R3, #0
    MOVS R2, #1
    MOVS R4, #32

str_ldr_test_loop
    STR R2, [R0, R1]
    LDR R3, [R0, R1]
    CMP R2, R3
    BNE str_ldr_test_led_lbl

    LSLS R2, #1
    SUBS R4, #1
    CMP  R4, R1
    BNE str_ldr_test_loop

// Test Complete
    B str_ldr_test_rtn_lbl

    // Failure
str_ldr_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
str_ldr_test_rtn_lbl
    BX LR
}


/* ldr_imm_test
 *
 * Tests the Load with immediate instruction
 */
__asm void ldr_imm_test( ) {

// Test load
    LDR  R0, =STACK_TOP
    MOVS R1, #0
    LDR  R2, =0xDEADBEEF
    STR  R2, [R0, R1]
    LDR  R3, [R0, #0]
    CMP  R2, R3
    BNE  ldr_imm_test_led_lbl

// Test Complete
    B ldr_imm_test_rtn_lbl

    // Failure
ldr_imm_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
ldr_imm_test_rtn_lbl
    BX LR
}


/* ldrb_test
 *
 * Tests the Load Byte instruction
 */
__asm void ldrb_test( ) {

// Test load
    LDR  R0, =STACK_TOP
    MOVS R1, #0
    LDR  R2, =0xDEADBEEF
    STR  R2, [R0, R1]
    MOVS R3, #0
    
    // Load byte 0
    MOVS R2, #0xEF
    LDRB R3, [R0, R1]
    CMP  R2, R3
    BNE  ldrb_test_led_lbl

    // Load byte 1
    ADDS R1, #1
    MOVS R2, #0xBE
    LDRB R3, [R0, R1]
    CMP  R2, R3
    BNE  ldrb_test_led_lbl

    // Load byte 2
    ADDS R1, #1
    MOVS R2, #0xAD
    LDRB R3, [R0, R1]
    CMP  R2, R3
    BNE  ldrb_test_led_lbl

    // Load byte 3
    ADDS R1, #1
    MOVS R2, #0xDE
    LDRB R3, [R0, R1]
    CMP  R2, R3
    BNE  ldrb_test_led_lbl

// Test Complete
    B ldrb_test_rtn_lbl

    // Failure
ldrb_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
ldrb_test_rtn_lbl
    BX LR
}


/* ldrb_imm_test
 *
 * Tests the Load Byte with immediate instruction
 */
__asm void ldrb_imm_test( ) {

// Test load
    LDR  R0, =STACK_TOP
    MOVS R1, #0
    LDR  R2, =0xDEADBEEF
    STR  R2, [R0, R1]
    MOVS R3, #0
    
    // Load byte 0
    MOVS R2, #0xEF
    LDRB R3, [R0, #0]
    CMP  R2, R3
    BNE  ldrb_imm_test_led_lbl

    // Load byte 1
    MOVS R2, #0xBE
    LDRB R3, [R0, #1]
    CMP  R2, R3
    BNE  ldrb_imm_test_led_lbl

    // Load byte 2
    MOVS R2, #0xAD
    LDRB R3, [R0, #2]
    CMP  R2, R3
    BNE  ldrb_imm_test_led_lbl

    // Load byte 3
    MOVS R2, #0xDE
    LDRB R3, [R0, #3]
    CMP  R2, R3
    BNE  ldrb_imm_test_led_lbl

// Test Complete
    B ldrb_imm_test_rtn_lbl

    // Failure
ldrb_imm_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
ldrb_imm_test_rtn_lbl
    BX LR
}


/* ldrsb_test
 *
 * Tests the Load Sign-Extended Byte instruction
 */
__asm void ldrsb_test( ) {

// Test load
    LDR  R0, =STACK_TOP
    MOVS R1, #0
    LDR  R2, =0xDEAD7F00
    STR  R2, [R0, R1]
    MOVS R3, #0
    
    // Load byte 0
    MOVS  R2, #0x00
    SXTB  R2, R2
    LDRSB R3, [R0, R1]
    CMP   R2, R3
    BNE   ldrsb_test_led_lbl

    // Load byte 1
    ADDS  R1, #1
    MOVS  R2, #0x7F
    SXTB  R2, R2
    LDRSB R3, [R0, R1]
    CMP   R2, R3
    BNE   ldrsb_test_led_lbl

    // Load byte 2
    ADDS  R1, #1
    MOVS  R2, #0xAD
    SXTB  R2, R2
    LDRSB R3, [R0, R1]
    CMP   R2, R3
    BNE   ldrsb_test_led_lbl

    // Load byte 3
    ADDS  R1, #1
    MOVS  R2, #0xDE
    SXTB  R2, R2
    LDRSB R3, [R0, R1]
    CMP   R2, R3
    BNE   ldrsb_test_led_lbl

// Test Complete
    B ldrsb_test_rtn_lbl

    // Failure
ldrsb_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
ldrsb_test_rtn_lbl
    BX LR
}


/* ldrh_test
 *
 * Tests the Load Halfword instruction
 */
__asm void ldrh_test( ) {

// Test load
    LDR  R0, =STACK_TOP
    MOVS R1, #0
    LDR  R2, =0xDEADBEEF
    STR  R2, [R0, R1]
    MOVS R3, #0
    
    // Load halfword 0
    LDR  R2, =0x0000BEEF
    LDRH R3, [R0, R1]
    CMP  R2, R3
    BNE  ldrh_test_led_lbl

    // Load halfword 1
    ADDS R1, #2
    LDR  R2, =0x0000DEAD
    LDRH R3, [R0, R1]
    CMP  R2, R3
    BNE  ldrh_test_led_lbl

// Test Complete
    B ldrh_test_rtn_lbl

    // Failure
ldrh_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
ldrh_test_rtn_lbl
    BX LR
}


/* ldrh_imm_test
 *
 * Tests the Load Halfword with immediate instruction
 */
__asm void ldrh_imm_test( ) {

// Test load
    LDR  R0, =STACK_TOP
    MOVS R1, #0
    LDR  R2, =0xDEADBEEF
    STR  R2, [R0, R1]
    MOVS R3, #0
    
    // Load halfword 0
    LDR  R2, =0x0000BEEF
    LDRH R3, [R0, #0]
    CMP  R2, R3
    BNE  ldrh_imm_test_led_lbl

    // Load halfword 1
    LDR  R2, =0x0000DEAD
    LDRH R3, [R0, #2]
    CMP  R2, R3
    BNE  ldrh_imm_test_led_lbl

// Test Complete
    B ldrh_imm_test_rtn_lbl

    // Failure
ldrh_imm_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
ldrh_imm_test_rtn_lbl
    BX LR
}


/* ldrsh_test
 *
 * Tests the Load Sign-Extended Halfword instruction
 */
__asm void ldrsh_test( ) {

// Test load
    LDR  R0, =STACK_TOP
    MOVS R1, #0
    LDR  R2, =0x7FFFBEEF
    STR  R2, [R0, R1]
    MOVS R3, #0
    
    // Load halfword 0
    LDR   R2, =0xFFFFBEEF
    LDRSH R3, [R0, R1]
    CMP   R2, R3
    BNE   ldrsh_test_led_lbl

    // Load halfword 1
    ADDS  R1, #2
    LDR   R2, =0x00007FFF
    LDRSH R3, [R0, R1]
    CMP   R2, R3
    BNE   ldrsh_test_led_lbl

// Test Complete
    B ldrsh_test_rtn_lbl

    // Failure
ldrsh_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
ldrsh_test_rtn_lbl
    BX LR
}


/* str_imm_test
 *
 * Tests the Store with immediate instruction
 */
__asm void str_imm_test( ) {

// Test store
    LDR  R0, =STACK_TOP
    MOVS R1, #0
    LDR  R2, =0xDEADBEEF
    STR  R2, [R0, #0]
    LDR  R3, [R0, R1]
    CMP  R2, R3
    BNE  str_imm_test_led_lbl

// Test Complete
    B str_imm_test_rtn_lbl

    // Failure
str_imm_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
str_imm_test_rtn_lbl
    BX LR
}


/* strb_test
 *
 * Tests the Store Byte instruction
 */
__asm void strb_test( ) {

// Test store
    // Write a test pattern
    LDR  R0, =STACK_TOP
    LDR  R1, =0xA5A5A5A5
    STR  R1, [R0, #0]
    MOVS R2, #0
    MOVS R4, #0
    
    // Store byte 0
    MOVS R1, #0xEF
    LDR  R3, =0xA5A5A5EF
    STRB R1, [R0, R4]
    LDR  R2, [R0, #0]
    CMP  R2, R3
    BNE  strb_test_led_lbl

    // Store byte 1
    MOVS R1, #0xBE
    LDR  R3, =0xA5A5BEEF
    ADDS R4, #1
    STRB R1, [R0, R4]
    LDR  R2, [R0, #0]
    CMP  R2, R3
    BNE  strb_test_led_lbl

    // Store byte 2
    MOVS R1, #0xAD
    LDR  R3, =0xA5ADBEEF
    ADDS R4, #1
    STRB R1, [R0, R4]
    LDR  R2, [R0, #0]
    CMP  R2, R3
    BNE  strb_test_led_lbl

    // Store byte 3
    MOVS R1, #0xDE
    LDR  R3, =0xDEADBEEF
    ADDS R4, #1
    STRB R1, [R0, R4]
    LDR  R2, [R0, #0]
    CMP  R2, R3
    BNE  strb_test_led_lbl

// Test Complete
    B strb_test_rtn_lbl

    // Failure
strb_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
strb_test_rtn_lbl
    BX LR
}


/* strb_imm_test
 *
 * Tests the Store Byte with immediate instruction
 */
__asm void strb_imm_test( ) {

// Test store
    // Write a test pattern
    LDR  R0, =STACK_TOP
    LDR  R1, =0xA5A5A5A5
    STR  R1, [R0, #0]
    MOVS R2, #0
    MOVS R4, #0
    
    // Store byte 0
    MOVS R1, #0xEF
    LDR  R3, =0xA5A5A5EF
    STRB R1, [R0, #0]
    LDR  R2, [R0, #0]
    CMP  R2, R3
    BNE  strb_imm_test_led_lbl

    // Store byte 1
    MOVS R1, #0xBE
    LDR  R3, =0xA5A5BEEF
    STRB R1, [R0, #1]
    LDR  R2, [R0, #0]
    CMP  R2, R3
    BNE  strb_imm_test_led_lbl

    // Store byte 2
    MOVS R1, #0xAD
    LDR  R3, =0xA5ADBEEF
    STRB R1, [R0, #2]
    LDR  R2, [R0, #0]
    CMP  R2, R3
    BNE  strb_imm_test_led_lbl

    // Store byte 3
    MOVS R1, #0xDE
    LDR  R3, =0xDEADBEEF
    STRB R1, [R0, #3]
    LDR  R2, [R0, #0]
    CMP  R2, R3
    BNE  strb_imm_test_led_lbl

// Test Complete
    B strb_imm_test_rtn_lbl

    // Failure
strb_imm_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
strb_imm_test_rtn_lbl
    BX LR
}


/* strh_test
 *
 * Tests the Store Halfword instruction
 */
__asm void strh_test( ) {

// Test store
    // Write a test pattern
    LDR  R0, =STACK_TOP
    LDR  R1, =0xA5A5A5A5
    STR  R1, [R0, #0]
    MOVS R2, #0
    MOVS R4, #0
    
    // Store byte 0
    LDR  R1, =0x0000BEEF
    LDR  R3, =0xA5A5BEEF
    STRH R1, [R0, R4]
    LDR  R2, [R0, #0]
    CMP  R2, R3
    BNE  strh_test_led_lbl

    // Store byte 1
    LDR  R1, =0x0000DEAD
    LDR  R3, =0xDEADBEEF
    ADDS R4, #2
    STRH R1, [R0, R4]
    LDR  R2, [R0, #0]
    CMP  R2, R3
    BNE  strh_test_led_lbl

// Test Complete
    B strh_test_rtn_lbl

    // Failure
strh_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
strh_test_rtn_lbl
    BX LR
}


/* strh_imm_test
 *
 * Tests the Store Halfword with immediate instruction
 */
__asm void strh_imm_test( ) {

// Test store
    // Write a test pattern
    LDR  R0, =STACK_TOP
    LDR  R1, =0xA5A5A5A5
    STR  R1, [R0, #0]
    LDR  R1, =0xDEADBEEF
    MOVS R2, #0
    
    // Store byte 0
    LDR  R1, =0x0000BEEF
    LDR  R3, =0xA5A5BEEF
    STRH R1, [R0, #0]
    LDR  R2, [R0, #0]
    CMP  R2, R3
    BNE  strh_imm_test_led_lbl

    // Store byte 1
    LDR  R1, =0x0000DEAD
    LDR  R3, =0xDEADBEEF
    STRH R1, [R0, #2]
    LDR  R2, [R0, #0]
    CMP  R2, R3
    BNE  strh_imm_test_led_lbl

// Test Complete
    B strh_imm_test_rtn_lbl

    // Failure
strh_imm_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
strh_imm_test_rtn_lbl
    BX LR
}


/* ldm_test
 *
 * Tests the Load Multiple instruction
 */
__asm void ldm_test( ) {

// Test load multiple
    // Store data to the heap
    LDR  R4, =HEAP_ADDR
    LDR  R0, =0xFFFFFFFF
    SUBS R4, #12
    STR  R0, [R4, #12]
    LDR  R1, =0xDEADBEEF
    STR  R1, [R4, #8]
    LDR  R2, =0xA5A5A5A5
    STR  R2, [R4, #4]
    LDR  R3, =0x00000000
    STR  R3, [R4, #0]

    // Load data back from heap
    MOVS R5, #0
    MOVS R6, #0
    MOVS R7, #0
    LDM  R4, {R4-R7}

    // Compare results
    CMP R0, R7
    BNE ldm_test_led_lbl
    CMP R1, R6
    BNE ldm_test_led_lbl
    CMP R2, R5
    BNE ldm_test_led_lbl
    CMP R3, R4
    BNE ldm_test_led_lbl

    // Load data from heap again
    MOVS R0, #0
    MOVS R1, #0
    MOVS R2, #0
    MOVS R3, #0
    LDR  R4, =HEAP_ADDR
    SUBS R4, #12
    LDM  R4!, {R0-R3}

    // Compare results
    MOVS R4, #0
    CMP  R0, R4
    BNE  ldm_test_led_lbl
    CMP  R1, R5
    BNE  ldm_test_led_lbl
    CMP  R2, R6
    BNE  ldm_test_led_lbl
    CMP  R3, R7
    BNE  ldm_test_led_lbl

// Test Complete
    B ldm_test_rtn_lbl

    // Failure
ldm_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
ldm_test_rtn_lbl
    BX LR
}


/* stm_test
 *
 * Tests the Store Multiple instruction
 */
__asm void stm_test( ) {

// Test store multiple
    // Store data to the heap
    LDR  R4, =HEAP_ADDR
    SUBS R4, #12
    LDR  R0, =0xFFFFFFFF
    LDR  R1, =0xDEADBEEF
    LDR  R2, =0xA5A5A5A5
    LDR  R3, =0x00000000
    STM  R4!, {R0-R3}

    // Load data back from heap
    MOVS R5, #0
    MOVS R6, #0
    MOVS R7, #0
    SUBS R4, #16
    LDR  R7, [R4, #12]
    LDR  R6, [R4, #8]
    LDR  R5, [R4, #4]
    LDR  R4, [R4, #0]

    // Compare results
    CMP R0, R4
    BNE stm_test_led_lbl
    CMP R1, R5
    BNE stm_test_led_lbl
    CMP R2, R6
    BNE stm_test_led_lbl
    CMP R3, R7
    BNE stm_test_led_lbl

// Test Complete
    B stm_test_rtn_lbl

    // Failure
stm_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
stm_test_rtn_lbl
    BX LR
}


/* push_test
 *
 * Tests the Push instruction
 */
__asm void push_test( ) {

// Test push
    // Store data to the stack
    LDR  R0, =0xFFFFFFFF
    LDR  R1, =0xDEADBEEF
    LDR  R2, =0xA5A5A5A5
    LDR  R3, =0x00000000
    PUSH {R0-R3}

    // Load data back from stack
    MOVS R4, #0
    MOVS R5, #0
    MOVS R6, #0
    MOVS R7, #0
    LDR  R4, [SP, #0]
    LDR  R5, [SP, #4]
    LDR  R6, [SP, #8]
    LDR  R7, [SP, #12]
    ADD  SP, SP, #16

    // Compare results
    CMP R0, R4
    BNE push_test_led_lbl
    CMP R1, R5
    BNE push_test_led_lbl
    CMP R2, R6
    BNE push_test_led_lbl
    CMP R3, R7
    BNE push_test_led_lbl

// Test Complete
    B push_test_rtn_lbl

    // Failure
push_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
push_test_rtn_lbl
    BX LR
}


/* pop_test
 *
 * Tests the Pop instruction
 */
__asm void pop_test( ) {

// Test pop
    // Store data to the stack
    LDR R0, =0xFFFFFFFF
    LDR R1, =0xDEADBEEF
    LDR R2, =0xA5A5A5A5
    LDR R3, =0x00000000
    SUB SP, SP, #16
    STR R0, [SP, #0]
    STR R1, [SP, #4]
    STR R2, [SP, #8]
    STR R3, [SP, #12]

    // Load data back from stack
    POP {R4-R7}

    // Compare results
    CMP R0, R4
    BNE pop_test_led_lbl
    CMP R1, R5
    BNE pop_test_led_lbl
    CMP R2, R6
    BNE pop_test_led_lbl
    CMP R3, R7
    BNE pop_test_led_lbl

// Test Complete
    B pop_test_rtn_lbl

    // Failure
pop_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =memory_ledLoop
    BX  R1

    // Return from function
pop_test_rtn_lbl
    BX LR
}


/* strb_strh_test
 *
 * Calls all Store Byte and Store Halfword test functions
 *
 * Note: The following Store Byte and Store Halfword functions do not function
 *  correctly on the board itself. The HSIZE[0] and HSIZE[1] lines need to be
 *  connected before they can be tested automatically. Otherwise, they can be
 *  partially tested by ensuring that the HSIZE lines are changing value as
 *  expected
 */
void strb_strh_test(void) {
    strb_test();
    strb_imm_test();
    strh_test();
    strh_imm_test();
}


/* memory_instruction_test
 *
 * Calls all memory test functions
 */
void memory_instruction_test(void) {
    
    // Normal Loads and Stores
    adr_test();
    str_ldr_test();
    ldr_imm_test();
    ldrb_test();
    ldrb_imm_test();
    ldrsb_test();
    ldrh_test();
    ldrh_imm_test();
    ldrsh_test();
    str_imm_test();

    // Removed from the test. See function header below
    //strb_strh_test();

    // Load and Store Multiples
    ldm_test();
    stm_test();
    push_test();
    pop_test();
}


/* memory_ledLoop
 *
 * Assembly function which loops infinitely blinking
 *  an LED
 *
 * Inputs
 *   delay_count - number of iterations to wait between blinking
 */
__asm void memory_ledLoop(unsigned int delay_count){
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

