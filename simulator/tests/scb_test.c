//------------------------------------------------------------------------------
// scb_test.c
//
// Branden Ghena (brghena@umich.edu)
// 07/10/2013
//
// Entry point for Cortex M0 processor system control block testing. Note that
//  these test functions do _NOT_ respect the ARM Architecture Procedure Call
//  Standard (AAPCS) and may clobber any register values at any time although
//  LR and SP are generally respected
//------------------------------------------------------------------------------

#include "scb_test.h"

/* scb_register_test
 *
 * Tests access to System Control Block Registers
 */
__asm void scb_register_test( ) {

    // Read CPU ID Register
    LDR R0, =SCB_CPUID
    LDR R2, =0x410CC200
    LDR R1, [R0, #0]
    CMP R1, R2
    BNE scb_register_test_led_lbl

    // Read Application Interrupt and Reset Control Register
    LDR  R0, =SCB_AIRCR
    LDR  R2, =0x00000000
    LDR  R1, [R0, #0]
    ANDS R1, R1, R2
    CMP  R1, R2
    BNE  scb_register_test_led_lbl

    // Read Configuration and Control Register
    LDR R0, =SCB_CCR
    LDR R2, =0x00000208
    LDR R1, [R0, #0]
    CMP R1, R2
    BNE scb_register_test_led_lbl

    // Test Complete
    B scb_register_test_rtn_lbl

    // Failure
scb_register_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

    // Return from function
scb_register_test_rtn_lbl
    BX LR
}


/* scb_sysresetreq_test
 *
 * Tests trigging the System Reset Request
 * Note: this function may only be called once per reset of the processor
 */
__asm void scb_sysresetreq_test( ) {

    // Reset register
    MOVS R4, #0

    // Flag a system reset request
    LDR R0, =SCB_AIRCR
    LDR R1, =0x05FA0004
    STR R1, [R0, #0]
    NOP
    NOP

    // Test value of register
    LDR  R5, =NMI_ID
    CMP  R4, R5
    BNE  scb_sysresetreq_test_led_lbl

    // Test Complete
    B scb_sysresetreq_test_rtn_lbl

    // Failure
scb_sysresetreq_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

    // Return from function
scb_sysresetreq_test_rtn_lbl
    BX LR
}


/* scb_sleep_test
 *
 * Tests trigging the System Reset Request
 * Note: this function may only be called once per reset of the processor
 */
__asm void scb_sleep_test( ) {

    // Set up test, fully enable IRQ0
    CPSIE i
    LDR  R0, =NVIC_ISER
    MOVS R1, #1
    STR  R1, [R0, #0]

    // Reset register
    MOVS R4, #0

    // Trigger IRQ0
    LDR  R0, =NVIC_ISPR
    MOVS R1, #1
    STR  R1, [R0, #0]
    NOP
    NOP

    // Test value of register, NMI should have run
    LDR  R5, =NMI_ID
    CMP  R4, R5
    BNE  scb_sleep_test_led_lbl

    // Reset interrupts
    CPSID i
    LDR  R0, =NVIC_ICER
    MOVS R1, #1
    STR  R1, [R0, #0]

    // Test Complete
    B scb_sleep_test_rtn_lbl

    // Failure
scb_sleep_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

    // Return from function
scb_sleep_test_rtn_lbl
    BX LR
}


/* main
 *
 * Entry point for code. Calls test functions
 */
int main(void) {
    // Set clock speed
    (*CLK_SET_ADDR) = CLK_DIVIDE;

    // This test can only be run once
    scb_sysresetreq_test();

    while (1) {
        scb_register_test();
        scb_sleep_test();
    }
}


/* nmi_handler
 *
 * Non-Maskable Interrupt Handler
 */
__asm void nmi_handler( ) {

    // Save value signifying I've run
    LDR R4, =NMI_ID

    // Disable sleep on exit (only relevant for some tests)
    LDR  R0, =SCB_SCR
    MOVS R1, #0x00
    STR  R1, [R0, #0]

    // Return from function
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* irq0_handler
 *
 * Interrupt Request 0 Handler
 */
__asm void irq0_handler( ) {

    // Save value signifying I've run
    LDR R4, =IRQ_ID

    // Enable sleep on exit
    LDR  R0, =SCB_SCR
    MOVS R1, #0x02
    STR  R1, [R0, #0]

    // Call delayed NMI
    LDR R0, =NMI_DELAY_SIG

    // Return from function
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* ledLoop
 *
 * Assembly function which loops infinitely blinking
 *  an LED
 *
 * Inputs
 *   delay_count - number of iterations to wait between blinking
 */
__asm void ledLoop(unsigned int delay_count) {
// infinite loop
start

    // turn on the LED
    // uses R4 so value doesn't automatically go to stack on interrupt
    LDR R4, =LED_ON

    LDR R2, =0
loopA
    ADDS R2, #1
    CMP  R2, R0
    BNE  loopA

    // turn off the LED
    // uses R4 so value doesn't automatically go to stack on interrupt
    LDR R4, =LED_OFF

    LDR R2, =0
loopB
    ADDS R2, #1
    CMP  R2, R0
    BNE  loopB

    B start

    // Protect the literal pool
    NOP
    NOP
}

