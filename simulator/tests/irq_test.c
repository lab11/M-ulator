//------------------------------------------------------------------------------
// irq_test.c
//
// Branden Ghena (brghena@umich.edu)
// 07/19/2013
//
// Entry point for Cortex M0 processor interrupt testing. Note that these test
//  functions do _NOT_ respect the ARM Architecture Procedure Call Standard
//  (AAPCS) and may clobber any register values at any time, although LR and SP
//  are usually respected
//
// Note: this code cannot be properly simulated since it relies on HW interrupts
//  Also, during this test LED1 may light dimly. This is acceptable behavior and
//  does _NOT_ signal an error. If LED1 lights brightly, an error is occurring
//------------------------------------------------------------------------------

#include "irq_test.h"

/* irq_nvic_test
 *
 * Tests access to NVIC registers
 */
__asm void irq_nvic_test( ) {

    MOVS R0, #0
    LDR  R1, =0x0000FFFF

    // Attempt to clear all enable
    LDR R2, =NVIC_ICER
    STR R1, [R2, #0]
    LDR R2, =NVIC_ISER
    LDR R3, [R2, #0]
    CMP R0, R3
    BNE irq_nvic_test_led_lbl

    // Attempt to clear all pending
    LDR R2, =NVIC_ICPR
    STR R1, [R2, #0]
    NOP
    LDR R2, =NVIC_ISPR
    LDR R3, [R2, #0]
    CMP R0, R3
    BNE irq_nvic_test_led_lbl

    // Walk bits across ISER
    MOVS R0, #0
    LDR  R1, =0x0000FFFF
    LDR  R2, =NVIC_ISER
    MOVS R4, #1
iser_set_loop
    LSLS R0, #1
    ORRS R0, R0, R4
    STR  R0, [R2, #0]
    LDR  R3, [R2, #0]
    CMP  R0, R3
    BNE  irq_nvic_test_led_lbl
    CMP  R0, R1
    BNE  iser_set_loop

    // Walk bits across ICER
    LDR  R0, =0x00010000
    LDR  R1, =0x00000000
    LDR  R2, =NVIC_ICER
    LDR  R4, =0x0000FFFF
icer_set_loop
    LSRS R0, #1
    LSRS R4, #1
    STR  R0, [R2, #0]
    LDR  R3, [R2, #0]
    CMP  R3, R4
    BNE  irq_nvic_test_led_lbl
    CMP  R0, R1
    BNE  icer_set_loop

    // Walk bits across ISPR
    MOVS R0, #0
    LDR  R1, =0x0000FFFF
    LDR  R2, =NVIC_ISPR
    MOVS R4, #1
ispr_set_loop
    LSLS R0, #1
    ORRS R0, R0, R4
    STR  R0, [R2, #0]
    LDR  R3, [R2, #0]
    CMP  R0, R3
    BNE  irq_nvic_test_led_lbl
    CMP  R0, R1
    BNE  ispr_set_loop

    // Walk bits across ICPR
    LDR  R0, =0x00010000
    LDR  R1, =0x00000000
    LDR  R2, =NVIC_ICPR
    LDR  R4, =0x0000FFFF
icpr_set_loop
    LSRS R0, #1
    LSRS R4, #1
    STR  R0, [R2, #0]
    LDR  R3, [R2, #0]
    CMP  R3, R4
    BNE  irq_nvic_test_led_lbl
    CMP  R0, R1
    BNE  icpr_set_loop

    // Set all priorities to lowest value
    LDR R0, =0xFFFFFFFF
    LDR R1, =0xC0C0C0C0
    LDR R2, =NVIC_IPR0
    STR R0, [R2, #0]
    LDR R3, [R2, #0]
    CMP R1, R3
    BNE irq_nvic_test_led_lbl
    LDR R2, =NVIC_IPR1
    STR R0, [R2, #0]
    LDR R3, [R2, #0]
    CMP R1, R3
    BNE irq_nvic_test_led_lbl
    LDR R2, =NVIC_IPR2
    STR R0, [R2, #0]
    LDR R3, [R2, #0]
    CMP R1, R3
    BNE irq_nvic_test_led_lbl
    LDR R2, =NVIC_IPR3
    STR R0, [R2, #0]
    LDR R3, [R2, #0]
    CMP R1, R3
    BNE irq_nvic_test_led_lbl

    // Clear all priorities
    MOVS R0, #0
    LDR R2, =NVIC_IPR0
    STR R0, [R2, #0]
    LDR R3, [R2, #0]
    CMP R0, R3
    BNE irq_nvic_test_led_lbl
    LDR R2, =NVIC_IPR1
    STR R0, [R2, #0]
    LDR R3, [R2, #0]
    CMP R0, R3
    BNE irq_nvic_test_led_lbl
    LDR R2, =NVIC_IPR2
    STR R0, [R2, #0]
    LDR R3, [R2, #0]
    CMP R0, R3
    BNE irq_nvic_test_led_lbl
    LDR R2, =NVIC_IPR3
    STR R0, [R2, #0]
    LDR R3, [R2, #0]
    CMP R0, R3
    BNE irq_nvic_test_led_lbl

    // Test Complete
    B irq_nvic_test_rtn_lbl

    // Failure
irq_nvic_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

    // Return from function
irq_nvic_test_rtn_lbl
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* irq_entry_test
 *
 * Tests entry and exit of an interrupt to ensure that registers are restored
 *  or modified properly
 */
__asm void irq_entry_test( ) {

    // Setup registers
    MOVS R0, #0
    MOVS R1, #1
    MOVS R2, #2
    MOVS R3, #3

    // Trigger interrupt
    LDR R4, =NMI_SIG
    NOP
    NOP

    // Test value of registers
    MOVS R5, #0
    CMP  R0, R5
    BNE  irq_entry_test_led_lbl
    MOVS R5, #1
    CMP  R1, R5
    BNE  irq_entry_test_led_lbl
    MOVS R5, #2
    CMP  R2, R5
    BNE  irq_entry_test_led_lbl
    MOVS R5, #3
    CMP  R3, R5
    BNE  irq_entry_test_led_lbl
    LDR  R5, =NMI_ID
    CMP  R4, R5
    BNE  irq_entry_test_led_lbl

    // Test Complete
    B irq_entry_test_rtn_lbl

    // Failure
irq_entry_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

    // Return from function
irq_entry_test_rtn_lbl
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* irq_cpsi_test
 *
 * Tests the Change Processor State, Interrupts Enable/Disable instructions
 */
__asm void irq_cpsi_test( ) {

    // Set up Primask
    MOVS R0, #0
    MSR PRIMASK, R0

    // Test Change Processor State
    CPSID i
    MRS R1, PRIMASK
    MOVS R0, #1
    CMP R0, R1
    BNE irq_cpsi_test_led_lbl

    CPSIE i
    MRS R1, PRIMASK
    MOVS R0, #0
    CMP R0, R1
    BNE irq_cpsi_test_led_lbl


    // Test Complete
    B irq_cpsi_test_rtn_lbl

    // Failure
irq_cpsi_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

    // Return from function
irq_cpsi_test_rtn_lbl
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* irq_enable_test
 *
 * Tests Enabling and Disabling of interrupts
 * Note: Upon return, IRQ0 will be enabled
 */
__asm void irq_enable_test( ) {

    // Set up test, fully enable IRQ0
    CPSIE i
    LDR  R0, =NVIC_ISER
    MOVS R1, #1
    STR  R1, [R0, #0]

    // Trigger the interrupt
    LDR R4, =IRQ_SIG
    NOP
    NOP
    LDR R5, =IRQ_ID
    CMP R4, R5
    BNE irq_enable_test_led_lbl

    // Disable via NVIC
    MOVS R1, #0
    STR  R1, [R0, #0]

    // Trigger the interrupt (no effect)
    LDR R4, =IRQ_SIG
    NOP
    NOP
    CMP R4, R5
    BEQ irq_enable_test_led_lbl

    // Clear pending and re-enable
    LDR  R2, =NVIC_ICPR
    MOVS R1, #1
    STR  R1, [R2, #0]
    STR  R1, [R0, #0]

    // Disable via Primask
    CPSID i

    // Trigger the interrupt (no effect)
    LDR R4, =IRQ_SIG
    NOP
    NOP
    CMP R4, R5
    BEQ irq_enable_test_led_lbl

    // Clear pending and re-enable
    MOVS R1, #1
    STR  R1, [R2, #0]
    CPSIE i

    // Test Complete
    B irq_enable_test_rtn_lbl

    // Failure
irq_enable_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

    // Return from function
irq_enable_test_rtn_lbl
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* irq_preemption_test
 *
 * Tests preemption of one interrupt by another
 * Note: Upon return, IRQ0 will be enabled
 */
__asm void irq_preemption_test( ) {

    // Set up test, fully enable IRQ0
    CPSIE i
    LDR  R0, =NVIC_ISER
    MOVS R1, #1
    STR  R1, [R0, #0]

    // Trigger interrupt with delay
    LDR R7, =NMI_DELAY_SIG

    // Trigger lower priority interrupt
    LDR R4, =IRQ_SIG
    NOP
    NOP

    // Check result
    LDR  R5, =NMI_ID
    CMP  R4, R5
    BNE  irq_preemption_test_led_lbl

    // Test Complete
    B irq_preemption_test_rtn_lbl

    // Failure
irq_preemption_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

    // Return from function
irq_preemption_test_rtn_lbl
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* irq_priority_test
 *
 * Tests the setting of priorities of IRQ interrupts
 */
__asm void irq_priority_test( ) {

    // Set up test, fully enable IRQ1-3
    CPSIE i
    LDR  R0, =NVIC_ISER
    MOVS R1, #0x0E
    STR  R1, [R0, #0]

    // Set priority values of various interrupts
    LDR R0, =NVIC_IPR0
    LDR R1, =0x4080C000
    STR R1, [R0, #0]

    // Clear registers
    MOVS R4, #0
    MOVS R5, #0
    MOVS R6, #0
    MOVS R7, #0

    // Trigger mid-level interrupt (via SW)
    LDR  R0, =NVIC_ISPR
    MOVS R1, #4
    STR  R1, [R0, #0]
    NOP
    NOP

    // Test result, ensure that IRQ1 has run
    MOVS R0, #1
    CMP  R0, R4
    BNE  irq_priority_test_led_lbl
    CMP  R0, R5
    BNE  irq_priority_test_led_lbl

    // Reset enables
    LDR  R0, =NVIC_ICER
    MOVS R1, #0x0E
    STR  R1, [R0, #0]

    // Reset Priorities
    LDR  R0, =NVIC_IPR0
    MOVS R1, #0
    STR  R1, [R0, #0]

    // Test Complete
    B irq_priority_test_rtn_lbl

    // Failure
irq_priority_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

    // Return from function
irq_priority_test_rtn_lbl
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* irq_sv_test
 *
 * Tests the Supervisor interrupts
 */
__asm void irq_sv_test( ) {

    // Set up test, fully enable IRQ3
    CPSIE i
    LDR  R0, =NVIC_ISER
    MOVS R1, #0x08
    STR  R1, [R0, #0]

    // Set priority values of various interrupts
    LDR R0, =NVIC_IPR0
    LDR R1, =0xC0000000
    STR R1, [R0, #0]
    LDR R0, =SCB_SHPR2
    LDR R1, =0x40000000
    STR R1, [R0, #0]
    LDR R0, =SCB_SHPR3
    LDR R1, =0x00800000
    STR R1, [R0, #0]

    // Clear registers
    MOVS R4, #0
    MOVS R5, #0
    MOVS R6, #0
    MOVS R7, #0

    // Trigger mid-level interrupt (via SW)
    LDR R0, =SCB_ICSR
    LDR R1, =0x10000000
    STR R1, [R0, #0]
    NOP
    NOP

    // Test result, ensure that IRQ3 has run
    MOVS R0, #3
    CMP  R0, R4
    BNE  irq_sv_test_led_lbl
    CMP  R0, R7
    BNE  irq_sv_test_led_lbl

    // Reset enables
    LDR  R0, =NVIC_ICER
    MOVS R1, #0x08
    STR  R1, [R0, #0]

    // Reset Priorities
    LDR  R0, =NVIC_IPR0
    MOVS R1, #0
    STR  R1, [R0, #0]
    LDR  R0, =SCB_SHPR2
    STR  R1, [R0, #0]
    LDR  R0, =SCB_SHPR3
    STR  R1, [R0, #0]

    // Test Complete
    B irq_sv_test_rtn_lbl

    // Failure
irq_sv_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

    // Return from function
irq_sv_test_rtn_lbl
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* irq_clear_test
 *
 * Tests clearing pending interrupts
 */
__asm void irq_clear_test( ) {

    // Set up test, fully enable IRQ3-4
    CPSIE i
    LDR  R0, =NVIC_ISER
    MOVS R1, #0x18
    STR  R1, [R0, #0]

    // Set priorities
    LDR R0, =NVIC_IPR0
    LDR R1, =0x80000000
    STR R1, [R0, #0]
    LDR R0, =NVIC_IPR1
    LDR R1, =0x00000040
    STR R1, [R0, #0]

    // Clear registers
    MOVS R4, #0
    MOVS R5, #0
    MOVS R6, #0
    MOVS R7, #0

    // Trigger interrupts (via SW)
    LDR  R0, =NVIC_ISPR
    MOVS R1, #0x18
    STR  R1, [R0, #0]
    NOP
    NOP

    // Test that IRQ4 was called
    MOVS R0, #4
    CMP  R0, R4
    BNE  irq_clear_test_led_lbl

    // Test that IRQ3 was not called
    MOVS R0, #0
    CMP  R0, R7
    BNE  irq_clear_test_led_lbl

    // Reset enables
    LDR  R0, =NVIC_ICER
    MOVS R1, #0x18
    STR  R1, [R0, #0]

    // Reset Priorities
    LDR  R0, =NVIC_IPR0
    MOVS R1, #0
    STR  R1, [R0, #0]
    LDR  R0, =NVIC_IPR1
    STR  R1, [R0, #0]

    // Test Complete
    B irq_clear_test_rtn_lbl

    // Failure
irq_clear_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

    // Return from function
irq_clear_test_rtn_lbl
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* irq_wait_test
 *
 * Tests waiting on interrupts
 */
__asm void irq_wait_test( ) {

    // Trigger interrupt with delay
    MOVS R4, #0
    LDR  R7, =NMI_DELAY_SIG

    // Wait for Interrupt
    WFI

    // Check result
    LDR  R5, =NMI_ID
    CMP  R4, R5
    BNE  irq_wait_test_led_lbl

    // Trigger interrupt with delay
    MOVS R4, #0
    LDR  R7, =NMI_DELAY_SIG

    // Wait for Event
    // First WFE clears the event register, which was set from the NMI
    WFE
    WFE

    // Check result
    LDR  R5, =NMI_ID
    CMP  R4, R5
    BNE  irq_wait_test_led_lbl

    // Test Complete
    B irq_wait_test_rtn_lbl

    // Failure
irq_wait_test_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

    // Return from function
irq_wait_test_rtn_lbl
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* irq_late_test
 *
 * Tests Late-Arrival and Tail-Chaining of interrupts
 */
__asm void irq_late_test( ) {

    // Set up interrupts
    CPSIE i
    LDR  R0, =NVIC_ISER
    MOVS R1, #2
    STR  R1, [R0, #0]

    // Trigger interrupts (via SW)
    LDR  R0, =NVIC_ISPR
    MOVS R1, #0x02
    LDR  R2, =SCB_ICSR
    LDR  R3, =0x80000000
    STR  R1, [R0, #0] // Lower priority
    STR  R3, [R2, #0] // Higher priority
    NOP
    NOP

    // Flash the LED briefly to trigger a data dump
    LDR R0, =LED_ON
    LDR R0, =LED_OFF

    // Reset interrupts
    LDR  R0, =NVIC_ICER
    MOVS R1, #2
    STR  R1, [R0, #0]

    // Test Complete (no automatic failure possible)
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* main
 *
 * Entry point for code. Calls test functions
 */
int main( ) {
    // Set clock speed
    (*CLK_SET_ADDR) = CLK_DIVIDE;
    
    // Note that the IRQ0 interrupt begins disabled
    while (1) {
        irq_nvic_test();
        irq_entry_test();
        irq_cpsi_test();
        irq_enable_test();
        irq_preemption_test();
        irq_priority_test();
        irq_sv_test();
        irq_clear_test();
        irq_wait_test();
        irq_late_test();
    }
}


/* nmi_handler
 *
 * Non-Maskable Interrupt Handler
 */
__asm void nmi_handler( ) {

    // Mess with lower registers
    ADDS R0, R0, #1
    ADDS R1, R1, #1
    ADDS R2, R2, #1
    ADDS R3, R3, #1

    // Save value signifying I've run
    LDR R4, =NMI_ID

    // Return from function
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* svcall_handler
 *
 * Supervisor Call Handler
 */
__asm void svcall_handler( ) {

    // Save value signifying I've run
    MOVS R4, #1
    MOVS R5, #1

    // Return from function
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* pendsv_handler
 *
 * PendSV Handler
 *
 * Note: the ICSR does not function as the simulator expects. Since the
 *  processor does not have the Debug Extension, ISRPENDING and VECTACTIVE
 *  are reserved
 */
__asm void pendsv_handler( ) {

    // Save value signifying I've run
    MOVS R4, #2
    MOVS R6, #2

    // Check the IPSR register
    MRS  R0, IPSR
    MOVS R1, #14
    CMP  R0, R1
    BNE  pendsv_handler_led_lbl

    // Check the ICSR register
    LDR R0, =SCB_ICSR
    LDR R2, =0x00000000
    LDR R1, [R0, #0]
    CMP R1, R2
    BNE pendsv_handler_led_lbl

    // Trigger a lower and higher priority interrupt
    LDR  R0, =NVIC_ISPR
    MOVS R1, #0x08
    STR  R1, [R0, #0]
    SVC  #0
    NOP
    NOP

    // Ensure that SVCall has run
    MOVS R0, #1
    CMP  R0, R4
    BNE  pendsv_handler_led_lbl
    CMP  R0, R5
    BNE  pendsv_handler_led_lbl

    // Ensure that IRQ3 has not run
    MOVS R0, #3
    CMP  R0, R7
    BEQ  pendsv_handler_led_lbl

    // Check the ICSR register
    LDR R0, =SCB_ICSR
    LDR R2, =0x00013000
    LDR R1, [R0, #0]
    CMP R1, R2
    BNE pendsv_handler_led_lbl

    // Return from function
    BX LR

    // Failure
pendsv_handler_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

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

    // Waste time
    MOVS R0, #8
    MOVS R1, #1
irq0_loop
    SUBS R0, R0, R1
    BNE  irq0_loop

    // Return from function
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* irq1_handler
 *
 * Interrupt Request 1 Handler
 */
__asm void irq1_handler( ) {

    // Save value signifying I've run
    MOVS R4, #1
    MOVS R5, #1

    // Return from function
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* irq2_handler
 *
 * Interrupt Request 2 Handler
 */
__asm void irq2_handler( ) {

    // Save value signifying I've run
    MOVS R4, #2
    MOVS R6, #2

    // Check the IPSR register
    MOVS R0, #18
    MRS  R1, IPSR
    CMP  R0, R1
    BNE  irq2_handler_led_lbl

    // Trigger a lower and higher priority interrupt
    LDR  R0, =NVIC_ISPR
    MOVS R1, #0x0A
    STR  R1, [R0, #0]
    NOP
    NOP

    // Ensure that IRQ3 has run
    MOVS R0, #3
    CMP  R0, R4
    BNE  irq2_handler_led_lbl
    CMP  R0, R7
    BNE  irq2_handler_led_lbl

    // Ensure that IRQ1 has not run
    MOVS R0, #1
    CMP  R0, R5
    BEQ  irq2_handler_led_lbl

    // Return from function
    BX LR

    // Failure
irq2_handler_led_lbl
    LDR R0, =(CLK_FREQ/5)
    LDR R1, =ledLoop
    BX  R1

    // Protect the literal pool
    NOP
    NOP
}


/* irq3_handler
 *
 * Interrupt Request 3 Handler
 */
__asm void irq3_handler( ) {

    // Save value signifying I've run
    MOVS R4, #3
    MOVS R7, #3

    // Return from function
    BX LR

    // Protect the literal pool
    NOP
    NOP
}


/* irq4_handler
 *
 * Interrupt Request 4 Handler
 */
__asm void irq4_handler( ) {

    // Save value signifying I've run
    MOVS R4, #4

    // Clear pending IRQ3
    LDR  R0, =NVIC_ICPR
    MOVS R1, #0x08
    STR  R1, [R0, #0]

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
    NOP
    NOP

    LDR R2, =0
loopA
    ADDS R2, #1
    CMP  R2, R0
    BNE  loopA

    // turn off the LED
    // uses R4 so value doesn't automatically go to stack on interrupt
    LDR R4, =LED_OFF
    NOP

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


