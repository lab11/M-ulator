//*******************************************************************
//Author: ZhiYoong Foo
//Description: Pbhw Functionality Tests
//Stacking Diagram: 	\m3_shared\m3_orders\Order to Advotech\140207 - PRCv8 Tbh(3) Tbhr(3) Tbhlw(3) Pbhw(3)\
//			Pbhw Stack Assembly Plan (PRS-BAT-PRC-DCP-SNS-HRV-RAD-SOL).pdf
//*******************************************************************
#include "mbus.h"
#include "PRCv8.h"
#include "SNSv2.h"
#include "HRVv1.h"
#include "RADv5.h"

#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define HRV_ADDR 0x6

#define NUM_SAMPLES 50         //Number of CDC samples to take

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
  
  //Test Declerations
  static uint32_t enumerated;
  uint32_t cdc_data[NUM_SAMPLES];
  
  //SNSv2 Register Declerations
  //Set to defaults
  snsv2_r0_t snsv2_r0 = SNSv2_R0_DEFAULT;
  snsv2_r1_t snsv2_r1 = SNSv2_R1_DEFAULT;;
  snsv2_r7_t snsv2_r7 = SNSv2_R7_DEFAULT;;
  
  //Clear All Pending Interrupts
  *((volatile uint32_t *) 0xE000E280) = 0xF;
  //Enable Interrupts
  *((volatile uint32_t *) 0xE000E100) = 0xF;
  
  //Set PMU Division to 5
  //*((volatile uint32_t *) 0xA200000C) = 0x4F770029; 
  //Set PMU Division to 6
  //*((volatile uint32_t *) 0xA200000C) = 0x0F77002B;

  //Enumerate
  if (enumerated != 0xDEADBEEF){
    enumerated = 0xDEADBEEF;
    //Enumeration
    enumerate(RAD_ADDR);
    WFI();
    enumerate(SNS_ADDR);
    WFI();
    enumerate(HRV_ADDR);
    WFI();
    delay(10000);
  }

  //SNSv2 R7
  delay(10000);
  snsv2_r7.REFGENREV_SEL = 0x4;
  snsv2_r7.REFGENREV_CDC_OFF = 0x0;
  write_mbus_register(SNS_ADDR,7,snsv2_r7.as_int);
  delay(10000);

  //SNSv2 R0;
  snsv2_r0.CDC_Bias_1st = 0x7;
  snsv2_r0.CDC_Bias_2nd = 0x7;
  snsv2_r0.CDC_s_recycle = 0x1;
  snsv2_r0.CDC_on_adap = 0x0;
  write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
  delay(10000);
  
  //SNSv2 R1;
  //(0x0<<18)|(0xF<<9)|(0x20<<0);
  snsv2_r1.CDC_S_period = 0x1A;
  snsv2_r1.CDC_R_period = 0xC;
  write_mbus_register(SNS_ADDR,1,snsv2_r1.as_int);
  delay(10000);
  
  delay(10000);
  //Disable ext_reset
  snsv2_r0.CDC_EXT_RESET = 0x0;
  write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
  asm( "wfi;" );
  
  //Disable & Enable CDC_CLK
  uint32_t i = 0;
  for (i=0; i<NUM_SAMPLES; i++){
    snsv2_r0.CDC_CLK = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
    asm( "wfi;" );
    snsv2_r0.CDC_CLK = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
    cdc_data[i] = *((volatile uint32_t *) 0xA0001014);
  }

  delay(1000);
  for (i=0; i<NUM_SAMPLES; i++){
    delay(1000);
    write_mbus_register(0x5,0,cdc_data[i]);
  }

  sleep();

  while (1){}
}
