//*******************************************************************
//Author: Gyouho Kim, ZhiYoong Foo
//Description: Code for Image capturing with MDLAYER
//*******************************************************************
#include "m3_proc.h"

//Defines for easy handling
#define MD_ADDR 0x4           //MDv1 Short Address
#define RAD_ADDR 0x2           //RADIO Short Address
//#define SNS_ADDR 0x4           //SNSv1 Short Address

#define MBUS_DELAY 1000
//#define WAKEUP_DELAY 1000 // 50ms
#define WAKEUP_DELAY 400000 // 20s
#define WAKEUP_DELAY_FINAL 100000	// Delay for waiting for internal decaps to stabilize after waking up MDSENSOR
#define DELAY_1 20000 // 1s
#define INT_TIME 5
#define MD_INT_TIME 35
#define MD_TH 10
#define MD_MASK 0x3FF
#define MD_LOWRES 1
#define MD_TOPAD_SEL 0 // 1: thresholding, 0: no thresholding

#define VDD_CC_1P2 1
#define VNW_1P2 1

#define SEL_CC 3
#define SEL_CC_B 4

// FILL THIS OUT
#define SEL_VREF 0
#define SEL_VREFP 7
#define SEL_VBN 3
#define SEL_VBP 3
#define SEL_VB_RAMP 15
#define SEL_RAMP 1

#define SEL_ADC 1
#define SEL_ADC_B 6
#define SEL_PULSE 1
#define SEL_PULSE_COL 0
#define PULSE_SKIP 0
#define PULSE_SKIP_COL 0
#define TAVG 0

#define SEL_CLK_RING 2
#define SEL_CLK_DIV 3
#define SEL_CLK_RING_4US 0
#define SEL_CLK_DIV_4US 1
#define SEL_CLK_RING_ADC 0 
#define SEL_CLK_DIV_ADC 1
#define SEL_CLK_RING_LC 0
#define SEL_CLK_DIV_LC 0

#define START_ROW_IDX 0
#define END_ROW_IDX 0xA0

#define ROW_SKIP 0
#define COL_SKIP 0
#define IMG_8BIT 1
#define ROW_IDX_EN 0
#define MD_RETURN_ADDR 0x17

#define START_COL_IDX 0
#define COLS_TO_READ 0x27


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

  uint32_t mdreg_0 = ((MD_INT_TIME<<13)|(INT_TIME<<(3+1)));
  uint32_t mdreg_1 = (((!IMG_8BIT)<<16)|(MD_TOPAD_SEL<<12)|(MD_TH<<5)|(!MD_LOWRES<<2)|(MD_LOWRES<<1));
  uint32_t mdreg_2 = ((0x1F<<19)|(0x3<<12)|(MD_MASK));
  uint32_t mdreg_3 = ((SEL_RAMP<<18)|(SEL_VB_RAMP<<14)|(SEL_VBP<<12)|(SEL_VBN<<10)|(SEL_VREFP<<7)|(SEL_VREF<<4)|(!VNW_1P2<<3)|(VNW_1P2<<2)|(!VDD_CC_1P2<<1)|(VDD_CC_1P2<<0)); // is this inversion safe??
  uint32_t mdreg_4 = ((!TAVG<<19)|(TAVG<<18)|(PULSE_SKIP_COL<<17)|(PULSE_SKIP<<16)|(SEL_CC_B<<13)|(SEL_CC<<10)|(SEL_PULSE_COL<<8)|(SEL_PULSE<<6)|(SEL_ADC_B<<3)|(SEL_ADC<<0));
  uint32_t mdreg_5 = ((SEL_CLK_DIV_LC<<15)|(SEL_CLK_RING_LC<<13)|(SEL_CLK_DIV_ADC<<10)|(SEL_CLK_DIV_4US<<8)|(SEL_CLK_DIV<<6)|(SEL_CLK_RING_ADC<<4)|(SEL_CLK_RING_4US<<2)|(SEL_CLK_RING<<0));
  uint32_t mdreg_6 = ((ROW_IDX_EN<<21)|(IMG_8BIT<<19)|(COL_SKIP<<18)|(ROW_SKIP<<17)|(END_ROW_IDX<<9)|(START_ROW_IDX<<1)|(0x1<<0));
  uint32_t mdreg_7 = ((0x7<<15));
  uint32_t mdreg_8 = ((COLS_TO_READ<<15)|(START_COL_IDX<<8)|(MD_RETURN_ADDR<<0));

static void initialize_md_reg(){

  //Write Registers;
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x3,mdreg_3);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x4,mdreg_4);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x5,mdreg_5);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x6,mdreg_6);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x8,mdreg_8);
  delay (MBUS_DELAY);

}

static void poweron_frame_controller(){

  // Release MD Presleep (this also releases reset due to a design bug)
  // 2:22
  mdreg_2 &= ~(1<<22);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay(WAKEUP_DELAY);

  // Release MD Sleep
  // 2:21
  mdreg_2 &= ~(1<<21);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay(WAKEUP_DELAY);

  // Release MD Isolation
  // 7:15
  mdreg_7 &= ~(1<<15);
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);

  // Start MD Clock
  // 5:11
  mdreg_5 |= (1<<11);
  write_mbus_register(MD_ADDR,0x5,mdreg_5);
  delay (MBUS_DELAY);

}

static void poweron_frame_controller_short(){

  // Release MD Presleep (this also releases reset due to a design bug)
  // 2:22
  mdreg_2 &= ~(1<<22);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay (MBUS_DELAY);

  // Release MD Sleep
  // 2:21
  mdreg_2 &= ~(1<<21);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay (MBUS_DELAY);

  // Release MD Isolation
  // 7:15
  mdreg_7 &= ~(1<<15);
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);

  // Start MD Clock
  // 5:11
  mdreg_5 |= (1<<11);
  write_mbus_register(MD_ADDR,0x5,mdreg_5);
  delay (MBUS_DELAY);

}

static void start_md(){

  // Optionally release MD GPIO Isolation
  // 7:16
  mdreg_7 &= ~(1<<16);
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);
  delay(10000); // about 0.5s

  // Start MD
  // 0:1
  mdreg_0 |= (1<<1);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);
  delay(10000); // about 0.5s

  mdreg_0 &= ~(1<<1);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);

  delay(10000); // about 0.5s

  // Enable MD Flag
  // 1:3
  mdreg_1 |= (1<<3);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);

}

static void clear_md_flag(){

  // Stop MD
  // 0:2
  mdreg_0 |= (1<<2);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);
  delay (0x80); // about 6ms

  mdreg_0 &= ~(1<<2);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);

  // Clear MD Flag
  // 1:4
  mdreg_1 |= (1<<4);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);
  delay (0x80); // about 6ms
  
  mdreg_1 &= ~(1<<4);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);

  // Disable MD Flag
  // 1:3
  mdreg_1 &= ~(1<<3);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);

}

int main() {
  
  uint32_t exec_marker;
  uint32_t first_exec;

  delay(DELAY_1);

  //Check if it is the first execution
  exec_marker = *((volatile uint32_t *) 0x00000720);
  if (exec_marker != 0x12345678){
    first_exec = 1;
    //Mark execution
    *((volatile uint32_t *) 0x00000720) = 0x12345678;
  }else{
    first_exec = 0;
  }

  delay(DELAY_1);

  // Interrupts
  clear_all_pend_irq();
  enable_all_irq();

  if (first_exec == 1){

    // Enumeration
    enumerate(MD_ADDR);
    delay(DELAY_1);
    enumerate(RAD_ADDR);
    delay(DELAY_1);
    
    // Set PMU Strength & division threshold
    // Change PMU_CTRL Register
    // 0x0F770029 = Original
    // 0x2F77302A = Active clock only
    // 0x2F77007A = Sleep only (CTRv7)
    // 0x2F77306A = Both active & sleep clocks for CTRv6; fastest active ring is not stable
    // 0x2F77307A = Both active & sleep clocks for CTRv7; fastest active ring is not stable
    // 0x2FEFXXXX = Harvesting settings
    // 0x6XXXXXXX = PRCv8 fixed 5x PMU configuration
    *((volatile uint32_t *) 0xA200000C) = 0x2FEF307A;
  
    delay(DELAY_1);
    delay(DELAY_1);
  
    // Set MBUS Clock faster
    // Change GOC_CTRL Register
    // 0x00A02932 = Original
    // 0x00A02332 = Fastest MBUS clk
    //*((volatile uint32_t *) 0xA2000008) = 0x00A02332;
    
    delay(DELAY_1);

  } // if first_exec

  // This is required if this program is used for sleep/wakeup cycling
  clear_md_flag();

  // Initialize
  initialize_md_reg();

  // Release power gates, isolation, and reset for frame controller
  if (first_exec){
    poweron_frame_controller();
  }else{
    poweron_frame_controller_short();
  }

  // Start motion detection
  start_md();

  delay(DELAY_1);
  clear_md_flag();
  delay(DELAY_1);
  start_md();

  delay(DELAY_1);

  // Don't use sleep with CTRv6 or CTRv7 unfibbed
  sleep();

  while (1){}

}
