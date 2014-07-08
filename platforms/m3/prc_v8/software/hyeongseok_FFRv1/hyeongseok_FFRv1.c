//*******************************************************************
//Author: ZhiYoong Foo
//Description: FFRv1 Functionality Tests
//*******************************************************************
#include "mbus.h"
#include "PRCv8.h"
#include "FFRv1.h"

#define FFR_ADDR 0x4

//Interrupt Handlers
void handler_ext_int_0(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_0(void){
  *((volatile uint32_t *) 0xE000E280) = 0x1;
}
void handler_ext_int_1(void){
  *((volatile uint32_t *) 0xE000E280) = 0x2;
}
void handler_ext_int_2(void){
  *((volatile uint32_t *) 0xE000E280) = 0x4;
}
void handler_ext_int_3(void){
  *((volatile uint32_t *) 0xE000E280) = 0x8;
}

int main() {
  
  //FFRv1 Register Declerations
  ffrv1_r0_t ffrv1_r0 = FFRv1_R0_DEFAULT;
  ffrv1_r1_t ffrv1_r1 = FFRv1_R1_DEFAULT;
  ffrv1_r2_t ffrv1_r2 = FFRv1_R2_DEFAULT;
  ffrv1_r3_t ffrv1_r3 = FFRv1_R3_DEFAULT;
  ffrv1_r4_t ffrv1_r4 = FFRv1_R4_DEFAULT;

  //Clear All Pending Interrupts
  *((volatile uint32_t *) 0xE000E280) = 0xF;
  //Enable Interrupts
  *((volatile uint32_t *) 0xE000E100) = 0xF;

  //Enumeration
  enumerate(FFR_ADDR);
  asm ("wfi;");
  
  //Start Testing

  


  //End Testing
  //Never Ends
  while (1){}
}
