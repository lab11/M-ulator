//*******************************************************************
//Author: ZhiYoong Foo
//Description: Random code snippets used for solo testing
//*******************************************************************
#include "m3_proc.h"

//#define RAD_ADDR 0x4
#define SNS_ADDR 0x4

#define NUM_SAMPLES 10         //Number of CDC samples to take

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

//Internal Functions
static void delay(unsigned ticks) {
  unsigned i;
  for (i=0; i < ticks; i++)
    asm("nop;");
}

int main() {
  
  uint32_t cdc_data[100];

  //Clear All Pending Interrupts
  *((volatile uint32_t *) 0xE000E280) = 0xF;
  //Enable Interrupts
  *((volatile uint32_t *) 0xE000E100) = 0xF;

  //Enumeration
  enumerate(SNS_ADDR);
  asm ("wfi;");

  uint32_t _sns_r0 = (0x7<<18)|(0x7<<15)|(0x0<<14)|(0x0<<12)|(0x1<<9)|(0x0<<8)|(0x1<<7)|(0x0<<6)|(0x1<<5)|(0x0<<4)|(0x7<<1)|(0x0<<0);
  //Write R0;
  write_mbus_register(SNS_ADDR,0,_sns_r0);
  //Set R1;
  uint32_t _sns_r1 = (0x0<<18)|(0x7<<9)|(0x15<<0);
  //Write R1;
  write_mbus_register(SNS_ADDR,1,_sns_r1);
  //Disable ext_reset
  _sns_r0 &= ~(1<<21);
  write_mbus_register(SNS_ADDR,0,_sns_r0);

  //Disable & Enable cdc_clk
  uint32_t i = 0;
  for (i=0; i<NUM_SAMPLES; i++){
    _sns_r0 &= ~(1<<22);
    write_mbus_register(SNS_ADDR,0,_sns_r0);
    delay (1000);
    _sns_r0 |= (1<<22);
    write_mbus_register(SNS_ADDR,0,_sns_r0);
    asm( "wfi;" );
    cdc_data[i] = *((volatile uint32_t *) 0xA0001014);
  }
  
  for (i=0; i<NUM_SAMPLES; i++){
    delay(1000);
    write_mbus_register(0x5,0,cdc_data[i]);
  }
  while (1){}
}
