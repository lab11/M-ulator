//*******************************************************************************************
// Math Functions Testing
//------------------------

#include "mbus.h"
#include "PREv22E.h"
#include "TMCv1r1.h"

#define XO_FREQ 1024

static void operation_sleep (void) {
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;

    // Clear all pending IRQs; otherwise, PREv22E replaces the sleep msg with a selective wakeup msg
    *NVIC_ICER = 0xFFFFFFFF;

    // Go to sleep immediately
    mbus_sleep_all(); 
    while(1);
}

int main(void) {

    // Disable PRE Watchdog Timers (CPU watchdog and MBus watchdog)
    *TIMERWD_GO  = 0;
    *REG_MBUS_WD = 0;

    // GOC Inputs
    uint32_t wakeup_interval = *(GOC_DATA_IRQ+0);
    uint32_t snt_freq        = *(GOC_DATA_IRQ+1);
    uint32_t snt_duration    = *(GOC_DATA_IRQ+2);
    uint32_t delta_xo        = *(GOC_DATA_IRQ+3);

    // Internal Variables
    uint32_t temp_snt_freq;
    uint32_t fail = 0;
    uint32_t a = (0x1 << 28);   // Default: 1 (this is the ideal case; i.e., no error)

    // Calculate 'A' (n=30 fixed-ratio)
    a = mult_b28(/*num_a*/ div(/*numer*/snt_duration, /*denom*/delta_xo, /*n*/28),
                 /*num_b*/ div(/*numer*/XO_FREQ,      /*denom*/snt_freq, /*n*/28)
                 );

    // Calculate the new SNT frequency
    temp_snt_freq = mult_b28(/*num_a*/snt_freq, /*num_b*/a);

    // 'A' must be close to 1.
    //      delta_xo + 25% results in a = 0.8       (0x0CCCCCCC with N=28)
    //      delta_xo - 25% results in a = 1.3333333 (0x15555555 with N=28)
    if ( (a>0x0CCCCCCC) && (a<0x15555555) ) {
        // Update SNT frequency
        snt_freq = temp_snt_freq;
    }
    // FAIL: Something is wrong if 'A' is not close to 1
    else {
        fail = 1;
        #ifdef DEVEL
            mbus_write_message32(0x80, 0xDEAD0001);
        #endif
    }

    // Get wakeup_interval in seconds
    uint32_t wakeup_interval_sec = (wakeup_interval << 6) - (wakeup_interval << 2);

    // If there was a failure: Use previous numbers as they are.
    if (fail) {
        // Update the next duration
        snt_duration = mult(/*num_a*/wakeup_interval_sec, /*num_b*/snt_freq);
    
        #ifdef DEVEL
            mbus_write_message32(0x80, 0xDEAD0002);
        #endif
    }
    // PASS
    else {
    
        #ifdef DEVEL
            mbus_write_message32(0x80, 0x0EA7F00D);
        #endif

        // Update next duration
        //--- if A >= 1
        if (get_bits(/*var*/a, /*msb*/31, /*lsb*/28)>=1) {
            snt_duration = mult(/*num_a*/wakeup_interval_sec, /*num_b*/snt_freq)
                         + mult_b28(/*num_a*/snt_duration, /*num_b*/a&0xEFFFFFFF);
        }
        //--- if A < 1
        else {
            snt_duration = mult(/*num_a*/wakeup_interval_sec, /*num_b*/snt_freq)
                            - mult_b28(/*num_a*/snt_duration, /*num_b*/(~a)&0x0FFFFFFF) 
                            - (snt_duration>>28);
        }
    
    }
    
#ifdef DEVEL
    mbus_write_message32(0x81, wakeup_interval_sec);
    mbus_write_message32(0x82, temp_snt_freq);
    mbus_write_message32(0x83, snt_duration);
    mbus_write_message32(0x84, snt_freq);
    mbus_write_message32(0x85, a);
#endif

    operation_sleep();
    return 1;
}
