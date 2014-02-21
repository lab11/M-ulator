//*******************************************************************
//Author: Gyouho Kim
//Description: Program for speeding up GOC frequency up to 880bps	
//*******************************************************************
//#include "m3_proc.h"
#include <stdint.h>
#include <stdbool.h>

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

	// Set PMU_REG for fastest sleep osc frequency
	*((volatile uint32_t *) 0xA200000C) = 0x0F770079;
	delay(10);
	
	// Set GOC_REG for tuning frontend settings
	*((volatile uint32_t *) 0xA2000008) = 0x0020290C;
	delay(10);


	while(1);

}
