//*******************************************************************
//Author: Gyouho Kim, ZhiYoong Foo
//Description: Code for Image capturing with MDLAYER
//*******************************************************************
#include "m3_proc.h"

//Defines for easy handling
#define MD_ADDR 0x4           //MDv1 Short Address
//#define SNS_ADDR 0x4           //SNSv1 Short Address

#define NUM_SAMPLES 10         //Number of T samples to take
#define WAKEUP_DELAY 100	// Delay for waiting for internal decaps to stabilize after waking up MDSENSOR
#define INT_TIME 35
#define MD_INT_TIME 35
#define MD_TH 5
#define MD_MASK 0x3FF

#define VDD_CC_1P2 1
#define VNW_1P2 0

// FILL THIS OUT
#define SEL_VREF 0b111
#define SEL_VREFP 0b111
#define SEL_VBN 0b11
#define SEL_VBP 0b00
#define SEL_VB_RAMP 0b1111
#define SEL_RAMP 0b000001

#define SEL_ADC 0b001
#define SEL_ADC_B 0b110
#define SEL_PULSE 0b01
#define SEL_PULSE_COL 0b00
#define SEL_CC 0b010
#define SEL_CC_B 0b101
#define PULSE_SKIP 0
#define PULSE_SKIP_COL 0
#define TAVG 0

#define SEL_CLK_RING 0b00
#define SEL_CLK_DIV 0b00
#define SEL_CLK_RING_4US 0b01
#define SEL_CLK_DIV_4US 0b10
#define SEL_CLK_RING_ADC 0b01
#define SEL_CLK_DIV_ADC 1
#define SEL_CLK_RING_LC 0
#define SEL_CLK_DIV_LC 0

#define START_ROW_IDX 0
#define END_ROW_IDX 0xA0

#define ROW_SKIP 0
#define COL_SKIP 0
#define IMG_8BIT 1
#define ROW_IDX_EN 0



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
//Internal Functions
//************************************

//************************************
//Delay for X Core Clock ticks
//************************************
static void delay(unsigned ticks) {
  unsigned i;
  for (i=0; i < ticks; i++)
    asm("nop;");
}

//
static void delay_nonop(unsigned ticks) {
  unsigned i;
  for (i=0; i < ticks; i++){
  }
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


int main() {
  
  uint32_t temp_data[NUM_SAMPLES]; //Temperature Data
  uint32_t radio_data; //Radio Data
  
  //Interrupts
  clear_all_pend_irq();
  enable_all_irq();

  //Enumeration
  enumerate(MD_ADDR);
//  delay (1); //Need to Delay in between for some reason.
//  enumerate(SNS_ADDR);
//  delay (1);

  //Setup MDv1
  uint32_t mdreg_0 = ((MD_INT_TIME<<13)|(INT_TIME<<3));
  uint32_t mdreg_1 = ((!IMG_8BIT)|(0x1<<12)|(MD_TH<<5)|(0x1<<2));
  uint32_t mdreg_2 = ((0x1F<<19)|(0x3<<12)|(MD_MASK<<0));
  uint32_t mdreg_3 = ((SEL_RAMP<<18)|(SEL_VB_RAMP<<14)|(SEL_VBP<<12)|(SEL_VBN<<10)|(SEL_VREFP<<7)|(SEL_VREF<<4)|(!VNW_1P2<<3)|(VNW_1P2<<2)|(!VDD_CC_1P2<<1)|(VDD_CC_1P2<<0)); // is this inversion safe??
  uint32_t mdreg_4 = ((!TAVG<<19)|(TAVG<<18)|(PULSE_SKIP_COL<<17)|(PULSE_SKIP<<16)|(SEL_CC_B<<13)|(SEL_CC<<10)|(SEL_PULSE_COL<<8)|(SEL_PULSE<<6)|(SEL_ADC_B<<3)|(SEL_ADC<<0));
  uint32_t mdreg_5 = ((SEL_CLK_DIV_LC<<15)|(SEL_CLK_RING_LC<<13)|(SEL_CLK_DIV_ADC<<10)|(SEL_CLK_DIV_4US<<8)|(SEL_CLK_DIV<<6)|(SEL_CLK_RING_ADC<<4)|(SEL_CLK_RING_4US<<2)|(SEL_CLK_RING<<0));
  uint32_t mdreg_6 = ((ROW_IDX_EN<<21)|(IMG_8BIT<<19)|(COL_SKIP<<18)|(ROW_SKIP<<17)|(END_ROW_IDX<<9)|(START_ROW_IDX<<1)|(0x1<<0));
  uint32_t mdreg_7 = ((0x3<<15));
  uint32_t mdreg_8 = ((0x50<<15)|(0x0<<8)|(0x17<<0));

  //Write Registers;
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  write_mbus_register(MD_ADDR,0x3,mdreg_3);
  write_mbus_register(MD_ADDR,0x4,mdreg_4);
  write_mbus_register(MD_ADDR,0x5,mdreg_5);
  write_mbus_register(MD_ADDR,0x6,mdreg_6);
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  write_mbus_register(MD_ADDR,0x8,mdreg_8);


  

}
