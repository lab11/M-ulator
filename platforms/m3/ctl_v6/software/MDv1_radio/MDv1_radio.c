//*******************************************************************
//Author: Gyouho Kim, ZhiYoong Foo
//Description: Code for Image capturing with MDLAYER
//*******************************************************************
#include "m3_proc.h"

// For radio transmission
#define RAD_BIT_DELAY 0x54     //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 1000  //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define RAD_SAMPLE_DELAY 2     //2       //Wake up timer tuning: # of wake up timer cycles to sleep


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

//***************************************************
//Data Setup for Radio (Initial Setup) & ECC [SECDED]
//***************************************************
//0xFXXX => Radio Preamble (bits 12-15)
//0xXX1F => ECC Setup (bits 0-4)
//Truncate last 3 bits of temp_data and put in bits 5-11
//ECC:
//B4 = B11^B10^B9
//B3 = B8 ^B7 ^B6
//B2 = B11^B10^B8 ^B7 ^B5
//B1 = B11^B9 ^B8 ^B6 ^B5
//B0 = B11^B10^B9 ^B8 ^B7 ^B6 ^B5 ^B4 ^B3 ^B2 ^B1
//Must be done in order (B0 is dependent on B1,2,3,4))
static uint32_t gen_radio_data(uint32_t data_in) {
  uint32_t data_out =  0xF000 | (data_in<<5);;
  uint32_t B4 = 
    ((data_out>>11)&0x1) ^ 
    ((data_out>>10)&0x1) ^ 
    ((data_out>> 9)&0x1);
  uint32_t B3 = 
    ((data_out>> 8)&0x1) ^ 
    ((data_out>> 7)&0x1) ^ 
    ((data_out>> 6)&0x1);
  uint32_t B2 = 
    ((data_out>>11)&0x1) ^ 
    ((data_out>>10)&0x1) ^ 
    ((data_out>> 8)&0x1) ^ 
    ((data_out>> 7)&0x1) ^ 
    ((data_out>> 5)&0x1);
  uint32_t B1 = 
    ((data_out>>11)&0x1) ^ 
    ((data_out>> 9)&0x1) ^ 
    ((data_out>> 8)&0x1) ^ 
    ((data_out>> 6)&0x1) ^ 
    ((data_out>> 5)&0x1);
  uint32_t B0 = 
    ((data_out>>11)&0x1) ^ 
    ((data_out>>10)&0x1) ^ 
    ((data_out>> 9)&0x1) ^ 
    ((data_out>> 8)&0x1) ^ 
    ((data_out>> 7)&0x1) ^
    ((data_out>> 6)&0x1) ^ 
    ((data_out>> 5)&0x1) ^ 
    B4 ^ B3 ^ B2 ^ B1;
  data_out |= (B4<<4)|(B3<<3)|(B2<<2)|(B1<<1)|(B0<<0);
  return data_out;
}

//***************************************************
//Send Radio Data MSB-->LSB
//Two Delays:
//Bit Delay: Delay between each bit (16-bit data)
//Packet Delay: Delay between each packet (3 packets)
//Delays are nop delays (therefore dependent on core speed)
//***************************************************
static void send_radio_data(uint32_t radio_data){
  int32_t i; //loop var
  uint32_t j; //loop var
  for(j=0;j<3;j++){ //Packet Loop
    for(i=15;i>=0;i--){ //Bit Loop
      delay (10);
      if ((radio_data>>i)&1) write_mbus_register(RAD_ADDR,0x27,0x1);
      else                   write_mbus_register(RAD_ADDR,0x27,0x0);
      //Must clear register
      delay (RAD_BIT_DELAY);
      write_mbus_register(RAD_ADDR,0x27,0x0);
      delay(RAD_BIT_DELAY); //Set delay between sending subsequent bit
    }
    delay(RAD_PACKET_DELAY); //Set delays between sending subsequent packet
  }
}


int main() {
  
//  uint32_t temp_data[NUM_SAMPLES]; //Temperature Data
//  uint32_t radio_data; //Radio Data

  uint32_t exec_marker;
  uint32_t first_exec;
  uint32_t radio_data;

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
    *((volatile uint32_t *) 0xA200000C) = 0x2FEF307A;
  
    delay(DELAY_1);
    delay(DELAY_1);
  
    // Set MBUS Clock faster
    // Change GOC_CTRL Register
    // 0x00A02932 = Original
    // 0x00A02332 = Fastest MBUS clk
    //*((volatile uint32_t *) 0xA2000008) = 0x00A02332;
    
    delay(DELAY_1);

    //Set up Radio
    //************************************
    //RADv4 Register Defaults
    //************************************
    delay(MBUS_DELAY);
    //Ext Ctrl En
    uint32_t _rad_r23 = 0x0;
    write_mbus_register(RAD_ADDR,0x23,_rad_r23);
    delay(MBUS_DELAY);

    //Current Limiter  
    uint32_t _rad_r26 = 0x0;
    write_mbus_register(RAD_ADDR,0x26,_rad_r26);  
    delay(MBUS_DELAY);
    //Tune Power
    uint32_t _rad_r20 = 0x1F;
    write_mbus_register(RAD_ADDR,0x20,_rad_r20);
    delay(MBUS_DELAY);
    //Tune Freq 1
    uint32_t _rad_r21 = 0x1;
    write_mbus_register(RAD_ADDR,0x21,_rad_r21);
    delay(MBUS_DELAY);
    //Tune Freq 2
    uint32_t _rad_r22 = 0xE;
    write_mbus_register(RAD_ADDR,0x22,_rad_r22);
    delay(MBUS_DELAY);
    //Tune TX Time
    uint32_t _rad_r25 = 0x4;
    write_mbus_register(RAD_ADDR,0x25,_rad_r25);
    delay(MBUS_DELAY);
    //Zero the TX register
    write_mbus_register(RAD_ADDR,0x27,0x0);
    delay(MBUS_DELAY);

  } // if first_exec

  else{

    //Fire off data to radio
    radio_data = gen_radio_data(0x1234);

    send_radio_data(radio_data);

    //Set up wake up register
    *((volatile uint32_t *) 0xA2000010) = 0x00008000 + RAD_SAMPLE_DELAY;
    //Reset counter
    *((volatile uint32_t *) 0xA2000014) = 0x1;

  }

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
  delay(MBUS_DELAY);
  sleep();

  while (1){}

}
