//*******************************************************************
//Author: Yejoong Kim
//Description: 
//*******************************************************************
#include <stdint.h>
#include <stdbool.h>

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t count;

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
#define REG0            ((volatile uint32_t *) 0x00000000)
#define REG1            ((volatile uint32_t *) 0x00000004)
#define REG2            ((volatile uint32_t *) 0x00000008)
#define REG3            ((volatile uint32_t *) 0x0000000C)

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
  //  count = 0;
count = 0;

*REG0 = 0xDEADBEEF;
*REG1= 2;
*REG2 = 3;
*REG3 = count;
    while (1) {
      count = count + 2;
    }



    return 1;
}

