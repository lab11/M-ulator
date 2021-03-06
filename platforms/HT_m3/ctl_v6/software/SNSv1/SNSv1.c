#include "m3_proc.h"

#define RAD_ADDR 0x3
#define SNS_ADDR 0x4

#define SNS_R0 0x1D89AE
#define SNS_R1 0x1E20
#define SNS_R3 0x5F0F0

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
  enumerate(RAD_ADDR);
  delay (1); //Need to Delay in between for some reason.
  enumerate(SNS_ADDR);
  delay (1);

  //******************************
  //HyunSoo's Recommended Settings
  //******************************

  //******************************
  //R0;
  //******************************
  //cdc_bias_1st = 0x7;
  //cdc_bias_2nd = 0x7;
  //op_on = 0x0;
  //td = 0x0;
  //s_recycle = 0x1;
  //on_adap = 0x0;
  //outck_in = 0x1;
  //ext_clk = 0x0;
  //cr_fixb = 0x1;
  //ext_max = 0x0;
  //max_select = 0x7;
  //ext_select_cr = 0x0;
  //******************************
  //R1;
  //******************************
  //cr_ext = 0x4;
  //r_period = 0x7;
  //s_period = 0x15;
  
  //Set R0;
  uint32_t _sns_r0 = (0x7<<18)|(0x7<<15)|(0x0<<14)|(0x0<<12)|(0x1<<9)|(0x0<<8)|(0x1<<7)|(0x0<<6)|(0x1<<5)|(0x0<<4)|(0x7<<1)|(0x0<<0);
  //Write R0;
  write_mbus_register(SNS_ADDR,0,_sns_r0);
  //Set R1;
  uint32_t _sns_r1 = (0x4<<18)|(0x7<<9)|(0x15<<0);
  //Write R1;
  write_mbus_register(SNS_ADDR,1,_sns_r1);
  //Disable ext_reset
  _sns_r0 &= ~(1<<21);
  write_mbus_register(SNS_ADDR,0,_sns_r0);

  //Disable & Enable cdc_clk
  uint32_t i = 0;
  for (i=0; i<100; i++){
    _sns_r0 &= ~(1<<22);
    write_mbus_register(SNS_ADDR,0,_sns_r0);
    delay (10);
    _sns_r0 |= (1<<22);
    write_mbus_register(SNS_ADDR,0,_sns_r0);
    asm( "wfi;" );
    cdc_data[i] = *((volatile uint32_t *) 0xA0001014);
  }
  
  for (i=0; i<100; i++){
    write_mbus_register(0x5,0,cdc_data[i]);
    delay(100);
  }

  while (1){}
}
