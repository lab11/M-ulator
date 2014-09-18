//*******************************************************************
//Author: Yoonmyung Lee
//        Zhiyoong Foo
//        Gyouho Kim
//        Myungjoon Choi
//Description: 	SNSv1 + RADv4 for Terraswarm Demo (2013 Nov)
//              Operation sequence:
//                     a> (only for the first ex) enumerate & configure
//                     b> request temp measurement
//                     c> send out result to radio
//                     d> go to sleep
//                     e> repeat from b>
//		Battery Testing (validated SNS)
//*******************************************************************
#include "m3_proc.h"

#define RAD_ADDR 0x3
#define SNS_ADDR 0x4

#define MBUS_DELAY 100 //Amount of delay between successive messages

#define RAD_BIT_DELAY 40     //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 600  //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define RAD_SAMPLE_DELAY 400     //2//    //Wake up timer tuning: # of wake up timer cycles to sleep
#define RAD_SAMPLE_DELAY_INITIAL 20 // 20:3sec Wake up timer duration for initial periods
//#define RAD_SAMPLE_DELAY 40000 //10000   //Radio tuning: Delay between samples sent (NUM_SAMPLES sent)

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

//Internal Functions
static void delay(unsigned ticks) {
  unsigned i;
  for (i=0; i < ticks; i++)
    asm("nop;");
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
  for(j=0;j<1;j++){ //Packet Loop
    for(i=15;i>=0;i--){ //Bit Loop
      delay (10);
      if ((radio_data>>i)&1) write_mbus_register(RAD_ADDR,0x27,0x1);
      else                   write_mbus_register(RAD_ADDR,0x27,0x0);

      // For radio debugging purposes: transmit all 1's
      //write_mbus_register(RAD_ADDR,0x27,0x1);

      //Must clear register
      delay (RAD_BIT_DELAY);
      write_mbus_register(RAD_ADDR,0x27,0x0);
      delay(RAD_BIT_DELAY); //Set delay between sending subsequent bit
    }
    //delay(RAD_PACKET_DELAY); //Set delays between sending subsequent packet
  }
}


int main() {

  uint32_t temp_data;
  uint32_t radio_data;
  static uint32_t exec_marker;
  static uint32_t exec_temp_marker;
  static uint32_t exec_count;
  uint32_t _sns_r3; 


  // static uint32_t temp_stored_data [255:0];

  //Clear All Pending Interrupts
  *((volatile uint32_t *) 0xE000E280) = 0xF;
  //Enable Interrupts
  *((volatile uint32_t *) 0xE000E100) = 0xF;


  *((volatile uint32_t *) 0xA200000C) = 0x4F771829;
//  *((volatile uint32_t *) 0xA2000008) = 0x00203903;
  delay(MBUS_DELAY);

  //Check if it is the first execution
  if ( exec_marker != 0x12345678 ) {
    //Mark execution
    exec_marker = 0x12345678;
    exec_count = 16;

    *((volatile uint32_t *) 0xA200000C) = 0x4F771829;
    delay(MBUS_DELAY);

    //Enumeration
//    enumerate(RAD_ADDR);
//    asm ("wfi;");
//    delay(MBUS_DELAY);
    enumerate(SNS_ADDR);
    asm ("wfi;");
    delay(MBUS_DELAY);

    //************************************
    //SNSv1 Register Defaults
    //uint32_t _sns_r0 = (0x0<<22)|(0x1<<21)|(0x6<<18)|(0x6<<15)|(0x0<<14)|(0x0<<12)|(0x4<<9)|(0x1<<8)|(0x1<<7)|(0x0<<6)|(0x1<<5)|(0x0<<4)|(0x7<<1)|(0x0<<0);
    //uint32_t _sns_r1 = (0x0<<18)|(0xF<<9)|(0x20<<0);
    //uint32_t _sns_r3 = (0x2<<17)|(0x1<<16)|(0xF<<12)|(0x0<<8)|(0xF<<4)|(0x0<<0);
    //************************************
    //Setup T Sensor
    _sns_r3 = (0x3<<17)|(0x1<<16)|(0xF<<12)|(0x0<<8)|(0xF<<4)|(0x0<<0);
    write_mbus_register(SNS_ADDR,3,_sns_r3);
    delay(MBUS_DELAY);
  }

  // Wakeup routine for every cycle

  //Clear All Pending Interrupts
  *((volatile uint32_t *) 0xE000E280) = 0xF;
  //Enable Interrupts
  *((volatile uint32_t *) 0xE000E100) = 0xF;

  // Check if wakeup is due to temperature sensor interrupt
  // If so, then skip this section

  if ( exec_temp_marker != 0x87654321 ) {

    // Set exec_temp_marker
    exec_temp_marker = 0x87654321;

    // Debug pointer
    //delay(MBUS_DELAY);
    //write_mbus_register(0x1,3,0xBEEF);
    
    // Set PMU Strength & division threshold
    // Change PMU_CTRL Register
    // 0x0F770029 = Original
    // Increase sleep oscillator frequency to provide enough power for temp sensor
//    *((volatile uint32_t *) 0xA200000C) = 0xF77006B;
    *((volatile uint32_t *) 0xA200000C) = 0x4F771829;
    delay(MBUS_DELAY);

    //Enable T Sensor
    _sns_r3 = (0x3<<17)|(0x0<<16)|(0xF<<12)|(0x0<<8)|(0xF<<4)|(0x0<<0);
    delay(MBUS_DELAY);
    write_mbus_register(SNS_ADDR,3,_sns_r3);
    
    //asm("wfi;");
	
    // Reset wakeup counter
    // This is required to go back to sleep!!
//    *((volatile uint32_t *) 0xA2000014) = 0x1;
    *((volatile uint32_t *) 0xA2000014) = 0x3;
    sleep();
    while(1);
  }

  // Set PMU Strength & division threshold
  // Change PMU_CTRL Register
  // 0x0F770029 = Original
  *((volatile uint32_t *) 0xA200000C) = 0x4F771829;
  delay(MBUS_DELAY);

  //Grab Data after IRQ
  temp_data = *((volatile uint32_t *) IMSG0);

  //Disable T Sensor
  _sns_r3 = (0x3<<17)|(0x1<<16)|(0xF<<12)|(0x0<<8)|(0xF<<4)|(0x0<<0);
  delay(MBUS_DELAY);
  write_mbus_register(SNS_ADDR,3,_sns_r3);

  // Reset exec_temp_marker
  exec_temp_marker = 0;

  //Fire off data to radio
//  radio_data = gen_radio_data(temp_data>>5);
//  delay(MBUS_DELAY);

  //  send_radio_data(radio_data);

  //write_mbus_register(RAD_ADDR,0x27,0x1);
  //Dont need to delay. Radio TX is slow.
  //delay(10000);

  // Check if this cycle belongs to initial 16 cycles
  if( exec_count ){
  	// Set up wake up timer register
  	*((volatile uint32_t *) 0xA2000010) = 0x00008000 + RAD_SAMPLE_DELAY_INITIAL;
  	exec_count = exec_count - 1;
  }
  else {
	*((volatile uint32_t *) 0xA2000010) = 0x00008000 + RAD_SAMPLE_DELAY;
  }

  // Reset wakeup counter
  *((volatile uint32_t *) 0xA2000014) = 0x1;

  // Go to Sleep
  delay(MBUS_DELAY);
  sleep();
  while(1);

  /*
  //Current Limiter
  _rad_r26 = 0x3F;
  write_mbus_register(RAD_ADDR,0x26,_rad_r26);
  delay(MBUS_DELAY);
  //Tune Power
  _rad_r20 = 0x00;
  write_mbus_register(RAD_ADDR,0x20,_rad_r20);
  delay(MBUS_DELAY);
  */

}
