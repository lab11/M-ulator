//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv12 debug
//*******************************************************************
#include <stdint.h>
#include <stdbool.h>
#define NVIC_ISER       ((volatile uint32_t *) 0xE000E100)  // Interrupt Set-Enable Register
#define NVIC_ICPR       ((volatile uint32_t *) 0xE000E280)  // Interrupt Clear-Pending Register
#define MBUS_CMD0       ((volatile uint32_t *) 0xA0002000)
#define MBUS_CMD1       ((volatile uint32_t *) 0xA0002004)
#define MBUS_CMD2       ((volatile uint32_t *) 0xA0002008)
#define MBUS_FUID_LEN   ((volatile uint32_t *) 0xA000200C)

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t num_cycle;

//*******************************************************************
// INTERRUPTS
//*******************************************************************

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    unsigned i;

    //Initialize Interrupts
    *NVIC_ICPR = 0x7FFF; //Clear All Pending Interrupts
    *NVIC_ISER = 0x7FFF; //Enable Interrupts
  
    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        enumerated = 0xDEADBEEF;
        num_cycle = 0;
        *((volatile uint32_t *) 0xA0000028) = 0x00019000; // Reg0x0A, enable backward-compatibility, halt_until_mbus_rx, disable all other irqs
        *((volatile uint32_t *) 0xA0003000) = 0x24000000; // Enumeration (0x4)
        *((volatile uint32_t *) 0xA0000028) = 0x0001A000; // Reg0x0A, enable backward-compatibility, halt_until_mbus_tx, disable all other irqs
    }
    
    // cycle number
    *((volatile uint32_t *) 0xA0003EF0) = num_cycle;

    // Spits out all mem data
    *MBUS_CMD0 = (0xAA << 24) | (51-1);
    *MBUS_CMD1 = 0x00000000;
    *MBUS_FUID_LEN = 0x23; // CMDLEN=2, MPQ_MEM_READ(3)

    *MBUS_CMD0 = (0xAB << 24) | (51-1);
    *MBUS_CMD1 = 0x000000C8;
    *MBUS_FUID_LEN = 0x23; // CMDLEN=2, MPQ_MEM_READ(3)

    num_cycle++;

        for (i=0; i < 10000; i++) asm("nop;");
    // Wakeup Timer
    *((volatile uint32_t *) 0xA0000034) = 0x0001800A; // WUP Timer. Sleep-Only. IRQ_EN, Duration=10
    *((volatile uint32_t *) 0xA0001300) = 0x00000001; // WUP Timer Reset

    if (num_cycle == 10) {
        *((volatile uint32_t *) 0xA0003BB0) = 0x0EA7F00D; // MBus Msg, 0xFF, 0x0EA7F00D
        while(1); 
    }
    else {
        // Sleep Message
        *((volatile uint32_t *) 0xA0003010) = 0x00000000;
        while(1); 
    }

    //Never Quit (should not come here.)
    while(1){
        *((volatile uint32_t *) 0xA0003FF0) = 0xFFFFFFFF; // MBus Msg, 0xFF, 0xFFFFFFFF
        for (i=0; i < 1000; i++) asm("nop;");
    }

    return 1;

}

