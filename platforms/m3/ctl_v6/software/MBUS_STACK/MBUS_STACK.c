//*******************************************************************
//Author: ZhiYoong Foo
//Description: Code for MBUS Stack Testing
//*******************************************************************
#include "m3_proc.h"

//Defines for easy handling
#define RAD_ADDR 0x3           //RADv4 Short Address

#define RAD_BIT_DELAY 0x54     //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 0x54  //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define RAD_SAMPLE_DELAY 0x54  //10000   //Radio tuning: Delay between samples sent (NUM_SAMPLES sent)

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
  
  uint32_t radio_data;
  //Interrupts
  clear_all_pend_irq();
  enable_all_irq();

  //Enumeration
  enumerate(RAD_ADDR);
  delay (1); //Need to Delay in between for some reason.

  //Junk Setup
  read_mbus_register(RAD_ADDR,0x0,0x13);
  delay(1000);
  read_mbus_register(RAD_ADDR,0x0,0x13);
  delay(1000);
  read_mbus_register(RAD_ADDR,0x0,0x13);
  delay(1000);
  read_mbus_register(RAD_ADDR,0x0,0x13);
  delay(1000);
  //Setup Radio
  delay(1000);
  uint32_t _rad_r26 = 0x0;
  write_mbus_register(RAD_ADDR,0x26,_rad_r26);
  delay(1000);
  uint32_t _rad_r20 = 0x1F;
  write_mbus_register(RAD_ADDR,0x20,_rad_r20);
  delay(1000);
  uint32_t _rad_r21 = 0x3;
  write_mbus_register(RAD_ADDR,0x21,_rad_r21);
  delay(1000);
  uint32_t _rad_r22 = 0x8;
  write_mbus_register(RAD_ADDR,0x22,_rad_r22);
  delay(1000);
  uint32_t _rad_r25 = 0x4;
  write_mbus_register(RAD_ADDR,0x25,_rad_r25);
  delay(1000);
  uint32_t _rad_r23 = 0x1;// EXT_CTRL (default 1'b0)
  write_mbus_register(RAD_ADDR,0x23,_rad_r23);
  delay(1000);
  uint32_t _rad_r24 = 0x0;// OSC_ENB (default 1'b1)
  write_mbus_register(RAD_ADDR,0x24,_rad_r24);
  delay(1000);

  radio_data = gen_radio_data(0xA0A0A0);
  while(1) {
    //write_mbus_register(RAD_ADDR,0x27,0x1);
    //delay(100);
    write_mbus_register(RAD_ADDR,0x27,0x0);
    delay(100);
  }

  send_radio_data(radio_data);
  delay(RAD_SAMPLE_DELAY);
  //  radio_data = gen_radio_data(0xA3);
  //  send_radio_data(radio_data);
  //  delay(RAD_SAMPLE_DELAY);

  while (1){}
}
