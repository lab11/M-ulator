/* 
 *
 * GDB Hooks for GDBoverMBUS support
 *
 * This works by co-opting the service routine call (svc)
 * as well as Interrupt #9 (handler_ext_int_9)
 *
 *
 *
 * Andrew Lukefahr
 * lukefahr@indiana.edu
 *
 * Much of this is borrowed from:
 *  - https://github.com/adamheinrich/os.h/blob/master/src/os_pendsv_handler.s
 *  - The Definitive Guide to ARM Cortex-M0 and Cortex-M0+ Processors Section 10.7.1
 */

.syntax unified
.cpu cortex-m0
.fpu softvfp

.thumb 


/*
 *
 * use the software trap handler as a soft-Breakpoint
 *
 */
.global handler_svcall
.type handler_svcall, %function
handler_svcall:


    /*
    * Exception frame saved by the NVIC hardware onto stack:
    * +------+
    * |      | <- SP before interrupt (orig. SP)
    * | xPSR |
    * |  PC  |
    * |  LR  |
    * |  R12 |
    * |  R3  |
    * |  R2  |
    * |  R1  |
    * |  R0  | <- SP after entering interrupt (orig. SP + 32 bytes)
    * +------+
    */

    /*
    * Registers saved by the software (PendSV_Handler):
    * +------+
    * |  R7  |
    * |  R6  |
    * |  R5  |
    * |  R4  |
    * |  R11 |
    * |  R10 |
    * |  R9  |
    * |  R8  | <- Saved SP (orig. SP + 64 bytes)
    * +------+
    */
    /* we can push r4-r7 directly, r8-r11 we have to move into a lower
     * register first, then store it */
    push    {r4,r5,r6,r7}
    mov     r4, r8
    mov     r5, r9
    mov     r6, r10
    mov     r7, r11
    push    {r4,r5,r6,r7}

    /*
    * Finally save the (recalculated) origional stack pointer and 
    * the Special LR is saved by the software (PendSV_Handler):
    * +------+
    * |  SP  |  this is the origional (pre-svc) sp value 
    * |ISR_LR| <- Saved SP (orig. SP + 72 bytes)
    * +------+
    */
    mov     r1, sp
    subs    r1, #68 //back to origional sp before svc
    push    {r1}
    push    {lr}

    /* Now we can call the C svc_handler 
     * adapted from 'the_definitive_guide_to_arm_cortex_m0_and_cortex_m0+_processors'
     */

    /* make the stack pointer the first argument to the C svc_handler
     * but first we need to figure out which stack we're using
     * and we can do that by testing the special LR
     */
    movs    r0, #4
    mov     r1, LR // SPECIAL LR
    tst     r0, r1
    beq     stacking_used_MSP

    stacking_used_PSP:
    mrs     r0, PSP         // first parameter - stacking was using PSP
    b       handler_svcall_2 

    stacking_used_MSP:
    mrs     r0, MSP         // first parameter - stacking was using MSP
  
    handler_svcall_2:
    /* ok, now we can actually call the c svc_handler */
    ldr     r2, =_gdb_break // branch to C handler
    blx     r2

    /* pop the Special LR */
    pop     {r1} 

    /* pop the dummy SP */
    pop     {r4} 

    /* pop r8-r11 */
    pop     {r4,r5,r6,r7}
    mov     r8, r4
    mov     r9, r5
    mov     r10, r6
    mov     r11, r7

    /* pop r4-r7 */
    pop     {r4,r5,r6,r7}

    /* and return from SVC */
    bx      r1

/*
 *
 * use the MBUS Reg #7 (IRQ#9) Interrupt as a soft-Halt
 *
 */

.global handler_ext_int_9    /* 25 External Interrupt(9) */
.type handler_ext_int_9, %function
handler_ext_int_9:


    /*
    * Exception frame saved by the NVIC hardware onto stack:
    * +------+
    * |      | <- SP before interrupt (orig. SP)
    * | xPSR |
    * |  PC  |
    * |  LR  |
    * |  R12 |
    * |  R3  |
    * |  R2  |
    * |  R1  |
    * |  R0  | <- SP after entering interrupt (orig. SP + 32 bytes)
    * +------+
    */

    /*
    * Registers saved by the software (PendSV_Handler):
    * +------+
    * |  R7  |
    * |  R6  |
    * |  R5  |
    * |  R4  |
    * |  R11 |
    * |  R10 |
    * |  R9  |
    * |  R8  | <- Saved SP (orig. SP + 64 bytes)
    * +------+
    */
    /* we can push r4-r7 directly, r8-r11 we have to move into a lower
     * register first, then store it */
    push    {r4,r5,r6,r7}
    mov     r4, r8
    mov     r5, r9
    mov     r6, r10
    mov     r7, r11
    push    {r4,r5,r6,r7}

    /*
    * Finally save the (recalculated) origional stack pointer and 
    * the Special LR is saved by the software (PendSV_Handler):
    * +------+
    * |  SP  |  this is the origional (pre-svc) sp value 
    * |ISR_LR| <- Saved SP (orig. SP + 72 bytes)
    * +------+
    */
    mov     r1, sp
    subs    r1, #68 //back to origional sp before svc
    push    {r1}
    push    {lr}

    /* Now we can call the C svc_handler 
     * adapted from 'the_definitive_guide_to_arm_cortex_m0_and_cortex_m0+_processors'
     */

    /* make the stack pointer the first argument to the C svc_handler
     * but first we need to figure out which stack we're using
     * and we can do that by testing the special LR
     */
    movs    r0, #4
    mov     r1, LR // SPECIAL LR
    tst     r0, r1
    beq     irq9_stacking_used_MSP

    irq9_stacking_used_PSP:
    mrs     r0, PSP         // first parameter - stacking was using PSP
    b       handler_ext_int_9_2 

    irq9_stacking_used_MSP:
    mrs     r0, MSP         // first parameter - stacking was using MSP
  
    handler_ext_int_9_2:
    /* ok, now we can actually call the c svc_handler */
    ldr     r2, =_gdb_halt // branch to C handler
    blx     r2

    /* pop the Special LR */
    pop     {r1} 

    /* pop the dummy SP */
    pop     {r4} 

    /* pop r8-r11 */
    pop     {r4,r5,r6,r7}
    mov     r8, r4
    mov     r9, r5
    mov     r10, r6
    mov     r11, r7

    /* pop r4-r7 */
    pop     {r4,r5,r6,r7}

    /* and return from SVC */
    bx      r1

