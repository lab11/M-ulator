//*******************************************************************
//Author: ZhiYoong Foo
//Description: Random code snippets used for solo testing
//*******************************************************************
#include "m3_proc.h"

int main() {
  //Timer Setup
  //WUP_CTRL
  *((volatile uint32_t *) 0xA2000010) = 0x00008008; //Trigger every 2 cycles
  //TSTAMP
  *((volatile uint32_t *) 0xA2000014) = 0x0000ABCD; //Any data will reset timer
  //GOC_CTRL
  //0x002829B2 = Original
  //0x003829B2 = Slower CPU
  //0x002C29B2 = Slower MBus
  *((volatile uint32_t *) 0xA0001028) = 0x003429B2;
  
  //Enter Sleep
  *((volatile uint32_t *) 0xA0000008) = 0x00000000;
  *((volatile uint32_t *) 0xA0000000) = 0x00000001;
  *((volatile uint32_t *) 0xA0000004) = 0x00000000;
  *((volatile uint32_t *) 0xA0000004) = 0x00000000;
  
  while (1){}
}
