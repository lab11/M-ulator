//*******************************************************************
//Author: Gyouho Kim
//Description: Program for speeding up GOC frequency up to 880bps	
//*******************************************************************
#include "PRCv8.h"

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
