//*******************************************************************************************
// Temperature Conversion Testing
//------------------------

#include "mbus.h"
#include "PREv22E.h"
#include "TMCv1r1.h"

#define COEFF_A 0x00007E8B
#define COEFF_B 0x005479B9

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

    //-------------------------------------------------
    // Temperature Conversion Testing
    //-------------------------------------------------
    //uint32_t dout = *GOC_DATA_IRQ;
    //mbus_write_message32(0xB0, dout);
    //uint32_t temp = tconv(dout, COEFF_A, COEFF_B, 0);
    //mbus_write_message32(0xB1, (dout<<16)|temp);

    //uint32_t msb = *GOC_DATA_IRQ;
    //if (msb!=0) {
    //    *GOC_DATA_IRQ=0;
    //    uint32_t lsb;
    //    for (lsb=0; lsb<0x4000; lsb++) {
    //        uint32_t dout = ((msb-1)<<14) | lsb;
    //        uint32_t temp = tconv(dout, COEFF_A, COEFF_B, 0);
    //        mbus_write_message32(0xB1, (dout<<16)|temp);
    //    }
    //}

    mbus_write_message32(0xB1,
                        mult_b28(/*num_int*/1500,
                             /*num_fp*/*GOC_DATA_IRQ
                             )
                        );

    operation_sleep();
    return 1;
}
