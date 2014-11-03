//*******************************************************************
//Author: Yoonmyung Lee
//Description: SNSv3 Functionality Tests
//*******************************************************************
#include <stdint.h>
#include <stdbool.h>

//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************

int main() {
  
  //Chip ID
  *((volatile uint32_t *) 0xA0000000) = 0xDEADBFEE;

  //Initialize
  while(1){
	asm("nop;");
  }

}

