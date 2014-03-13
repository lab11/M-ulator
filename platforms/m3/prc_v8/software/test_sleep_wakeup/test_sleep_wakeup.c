//*******************************************************************
//Author: Yoonmyung Lee / ZhiYoong Foo
//Description:	Post-tapeout Verification for PRCv8 (NOV 2013)
//		#05 - TIMERS
//*******************************************************************
#include "m3_proc.h"

#define RAD_ADDR 0x9
#define SNS_ADDR 0xA

#define DELAY_1 2000 //20000 // 1s


//Internal Functions ************************************************
void delay(unsigned ticks) {
	unsigned i;
	for (i=0; i < ticks; i++)
	asm("nop;");
}


//*******************************************************************
//Main **************************************************************
//*******************************************************************
int main() {

	//Clear All Pending Interrupts
	*((volatile uint32_t *) 0xE000E280) = 0x03FF;
	//Enable Interrupts
	*((volatile uint32_t *) 0xE000E100) = 0x03FF;

	//Enumeration
//	enumerate(RAD_ADDR);
//	asm ("wfi;");
//	enumerate(SNS_ADDR);
//	asm ("wfi;");

  delay(DELAY_1);
  delay(DELAY_1);
  delay(DELAY_1);
  
  // Set WUP_CTRL
  *((volatile uint32_t *) 0xA2000010) = 0x00008005; // every 3 cycle
  delay(DELAY_1);

  // Reset TSTAMP
  *((volatile uint32_t *) 0xA2000014) = 0x0000ABCD; // Any data will reset timer
  delay(DELAY_1);

  sleep();

	while(1);

}
