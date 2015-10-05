//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv12 Functionality Tests
//*******************************************************************
#include "PRCv12.h"
#include "mbus.h"

//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************

int main() {
  
    //Initialize Interrupts
    init_interrupt();
  
    //Chip ID
    write_config_reg(0x8,0xDEAD);
  
    //MBUS
    mbus_enumerate(0x4);
  
    // Call it a day (For verilog sim only)
    *((volatile uint32_t *) 0xAFFFFFFF) = 0x1;

    //Never Quit
    //while(1){ asm("nop;"); }
}
