//*******************************************************************
//Author: Gyouho Kim, ZhiYoong Foo
//Description: Code for Image capturing with MDLAYER
//*******************************************************************
#include "m3_proc.h"

//Defines for easy handling
#define MD_ADDR 0x4           //MDv1 Short Address
//#define SNS_ADDR 0x4           //SNSv1 Short Address

#define MBUS_DELAY 20
//#define WAKEUP_DELAY 1000 // 50ms
#define WAKEUP_DELAY 300000 // 15s
#define WAKEUP_DELAY_FINAL 100000	// Delay for waiting for internal decaps to stabilize after waking up MDSENSOR
#define DELAY_1 20000 // 1s
#define INT_TIME 5
#define MD_INT_TIME 35
#define MD_TH 10
#define MD_MASK 0x3FF
#define MD_LOWRES 1
#define MD_TOPAD_SEL 0 // 1: thresholding, 0: no thresholding


//************************************
//Interrupt Handlers
//Must clear pending bit!
//************************************
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

//************************************
//Delay for X Core Clock ticks
//************************************
static void delay(unsigned ticks) {
  unsigned i;
  for (i=0; i < ticks; i++)
    asm("nop;");
}

static void enable_all_irq(){
  //Enable Interrupts
  //Page 139 of M3 Book
  *((volatile uint32_t *) 0xE000E100) = 0xF; //Actually only enables external 0 -3
}

static void clear_all_pend_irq(){
  //Clear All Pending Interrupts
  //Page 140 of M3 Book
  *((volatile uint32_t *) 0xE000E280) = 0xF;  //Actually only clears external 0 -3
}

//************************************
//Internal Functions
//************************************


int main() {
  
//  uint32_t temp_data[NUM_SAMPLES]; //Temperature Data
//  uint32_t radio_data; //Radio Data

  // Set PMU Strength & division threshold
  // Change PMU_CTRL Register
  // 0x0F770029 = Original
  // 0x2F773829 = Active clock only
  // 0x2F770079 = Sleep only (CTRv7)
  // 0x2F773069 = Both active & sleep clocks for CTRv6; fastest active ring is not stable
  // 0x2F773079 = Both active & sleep clocks for CTRv7; fastest active ring is not stable
  //*((volatile uint32_t *) 0xA200000C) = 0x2F77307A;
  
  delay(DELAY_1);
  delay(DELAY_1);
  delay(DELAY_1);
  
  // Set WUP_CTRL
  *((volatile uint32_t *) 0xA2000010) = 0x00008003; // every 3 cycle
  delay(DELAY_1);

  // Reset TSTAMP
  *((volatile uint32_t *) 0xA2000014) = 0x0000ABCD; // Any data will reset timer
  delay(DELAY_1);


  // Enumeration
  //enumerate(MD_ADDR);


  sleep();


  while (1){}

}
