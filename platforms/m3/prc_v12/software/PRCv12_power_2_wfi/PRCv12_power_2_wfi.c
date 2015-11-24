//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv12 Power Measurement
//*******************************************************************
#include "PRCv12.h"
#include "mbus.h"

//********************************************************************
// Global Variables
//********************************************************************

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void init_interrupt (void) {
  *NVIC_ICPR = 0x7FFF; //Clear All Pending Interrupts
  *NVIC_ISER = 0x7FFF; //Disable Interrupts
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    //Initialize Interrupts
    init_interrupt();
    
    delay(100);

    // Wait for interrup
    // There is no interrupt following, so it should be stuck here.
    WFI();

    set_wakeup_timer(5, 1, 1);
    mbus_sleep_all();
    while(1);

    return 1;

}

