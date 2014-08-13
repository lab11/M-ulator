//*******************************************************************
//Author: Yoonmyung Lee
//        Zhiyoong Foo
//Description: 	Test program for Tb stack
//		Layer configuration: (Bot) CTR+DCP+RAD+SNS (Top)
//*******************************************************************
#include "m3_proc.h"

#define RAD_ADDR 0x3
#define SNS_ADDR 0x4

#define MBUS_DELAY 10000 //Amount of delay between successive messages

#define RAD_BIT_DELAY 0x54     //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 1000  //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define RAD_SAMPLE_DELAY 2     //2       //Wake up timer tuning: # of wake up timer cycles to sleep
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
/*static uint32_t gen_radio_data(uint32_t data_in) {
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
*/
//***************************************************
//Send Radio Data MSB-->LSB
//Two Delays:
//Bit Delay: Delay between each bit (16-bit data)
//Packet Delay: Delay between each packet (3 packets)
//Delays are nop delays (therefore dependent on core speed)
//***************************************************
/*static void send_radio_data(uint32_t radio_data){
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
*/
int main() {

  //Clear All Pending Interrupts
  *((volatile uint32_t *) 0xE000E280) = 0xF;
  //Enable Interrupts
  *((volatile uint32_t *) 0xE000E100) = 0xF;
  
  //Enumeration
  enumerate(RAD_ADDR);
  //delay(MBUS_DELAY);
  asm ("wfi;");
  //enumerate(SNS_ADDR);
  //delay(MBUS_DELAY);
  //  asm ("wfi;");

  //Set up Radio
  //************************************
  //RADv4 Register Defaults
  //************************************
  delay(MBUS_DELAY);

  //Current Limiter  
    uint32_t _rad_r26 = 0x3C;   // Unlimited
    write_mbus_register(RAD_ADDR,0x26,_rad_r26);  
    delay(MBUS_DELAY);

    //Tune Power
    uint32_t _rad_r20 = 0x1F;
    write_mbus_register(RAD_ADDR,0x20,_rad_r20);
    delay(MBUS_DELAY);
    
    //Tune Freq:  
    //		 0x3E = 900 MHz
    //		 0x37 = 905 MHz
    //		 0x2F = 913 MHz
    //           0x14 = 947 MHz
    //           Tb011: 0x2F=914.2 0x2E=915.4     0x2D=916.5 0x2B=918.8
    //           Tb023: 0x2F=    0x2D=918
    //           Tb024: 0x2F=913 0x2D=915
    // @4.3V     MRI01: (x1 g-slide) 0x2E=925.0 0x34=918.7 0x35=917.6 0x36=915.9 0x37=915.0 0x38=914.1
    //                  (x3 g-slide)                       0x35=916.4 0x36=915.4 0x37=914.2 
    //                  (x5 g-slide)                       0x35=916.1 0x36=915.1 0x37=914.0      
    // @4.3V     MRI02: (x1 g-slide)                       0x35=916.9 0x36=916.3 0x37=915.3 0x38=     
    //                  (x3 g-slide)                       0x35=917.0 0x36=916.2 0x37=915.2      
    //                  (x5 g-slide)                       0x35=916.7 0x36=915.7 0x37=914.6@4.3           
    //                                                                           0x37=914.9@4.2           
    //                                                                           0x37=915.3@4.1           
    //Tune Freq 1
    uint32_t _rad_r21 = 0x0;
    write_mbus_register(RAD_ADDR,0x21,_rad_r21);
    delay(MBUS_DELAY);

    //Tune Freq 2
    uint32_t _rad_r22 = 0x0;
    write_mbus_register(RAD_ADDR,0x22,_rad_r22);
    delay(MBUS_DELAY);

    //Tune TX Time
    uint32_t _rad_r25 = 0x4;    // 400ns
    write_mbus_register(RAD_ADDR,0x25,_rad_r25);
    delay(MBUS_DELAY);

    //Ext Ctrl En
    uint32_t _rad_r23 = 0x1;
    write_mbus_register(RAD_ADDR,0x23,_rad_r23);
    delay(MBUS_DELAY);

    //OSC_ENB : Enable Oscillation mode
    uint32_t _rad_r24 = 0x0;   
    write_mbus_register(RAD_ADDR,0x24,_rad_r24);
    delay(MBUS_DELAY);

    //Start TX
    write_mbus_register(RAD_ADDR,0x27,0x1);
    delay(MBUS_DELAY);
    while(1){};
}
