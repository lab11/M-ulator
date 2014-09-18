//*******************************************************************
//Author: Yoonmyung Lee
//        Zhiyoong Foo
//Description: 	RADv4 + Murata Sensor for Terraswarm Demo (2013 Nov)
//              Operation sequence:
//                     a> (only for the first ex) enumerate & configure
//                     b> request pressure measurement
//                     c> send out result to radio
//                     d> go to sleep
//                     e> repeat from b>
//*******************************************************************
#include "m3_proc.h"

#define RAD_ADDR 0x3
//#define SNS_ADDR 0x4

#define MBUS_DELAY 5 //Amount of delay between successive messages

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

  uint32_t temp_data;
  uint32_t radio_data;
  uint32_t exec_marker;
  
  //Clear All Pending Interrupts
  *((volatile uint32_t *) 0xE000E280) = 0xF;
  //Enable Interrupts
  *((volatile uint32_t *) 0xE000E100) = 0xF;


  //Check if it is the first execution
  exec_marker = *((volatile uint32_t*) 0x00000720);
  if ( exec_marker != 0x12345678 ) {

    //Mark execution
    *((volatile uint32_t *) 0x00000720) = 0x12345678;

    //Enumeration
    enumerate(RAD_ADDR);
    asm ("wfi;");
//    enumerate(SNS_ADDR);
//    asm ("wfi;");
}
    //Set up Radio
    //************************************
    //RADv4 Register Defaults
    //************************************
    delay(MBUS_DELAY);

    // Need to read first (bug workaround)
    read_mbus_register_RADv4(RAD_ADDR,0x1D,0x13);
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

    //Set up CDC
    //************************************
    //RADv4 Register Defaults
    //************************************
    //0x00 PG              (def: 1'h1   use: 0x1)
    write_mbus_register(RAD_ADDR,0x00,0x1);  
    delay(MBUS_DELAY);
    //0x01 OD_PICK         (def: 4'h0   use: 0x0)
    write_mbus_register(RAD_ADDR,0x01,0x0);  
    delay(MBUS_DELAY);
    //0x02 CLKEXT_EN       (def: 1'h0   use: 0x0)
    write_mbus_register(RAD_ADDR,0x02,0x0);  
    delay(MBUS_DELAY);
    //0x03 CLKGEN_EN       (def: 1'h0   use: 0x1)
    write_mbus_register(RAD_ADDR,0x03,0x1);  
    delay(MBUS_DELAY); 
    //0x04 DIV_PICK        (def: 4'h2   use: 0x1)
    write_mbus_register(RAD_ADDR,0x04,0x1);  
    delay(MBUS_DELAY);
    //0x05 RING_PICK       (def: 4'h1   use: 0x1)
    write_mbus_register(RAD_ADDR,0x05,0x1);  
    delay(MBUS_DELAY);
    //0x06 COMP_VGEN_BSEL  (def: 4'h7   use: 0x7)
    write_mbus_register(RAD_ADDR,0x06,0x7);  
    delay(MBUS_DELAY);
    //0x07 COMP_VGEN_TSELB (def: 4'h8   use: 0x8)
    write_mbus_register(RAD_ADDR,0x07,0x8);  
    delay(MBUS_DELAY);
    //0x08 AMP_VGEN_BSEL   (def: 4'h7   use: 0x7)
    write_mbus_register(RAD_ADDR,0x08,0x7);  
    delay(MBUS_DELAY);
    //0x09 AMP_VBEN_TSELB  (def: 4'h8   use: 0x8)
    write_mbus_register(RAD_ADDR,0x09,0x8);  
    delay(MBUS_DELAY);
    //0x0A AMPIN_VGEN_BSEL (def: 4'h7   use: 0x7)
    write_mbus_register(RAD_ADDR,0x0A,0x7);  
    delay(MBUS_DELAY);
    //0x0B AMPIN_VGEN_TSELB(def: 4'h8   use: 0x8)
    write_mbus_register(RAD_ADDR,0x0B,0x8);  
    delay(MBUS_DELAY);
    //0x0C AMP1_CONTB      (def: 2'h1   use: 0x1)
    write_mbus_register(RAD_ADDR,0x0C,0x1);  
    delay(MBUS_DELAY);
    //0x0D AMP2_CONTB      (def: 2'h1   use: 0x1)
    write_mbus_register(RAD_ADDR,0x0D,0x1);  
    delay(MBUS_DELAY);
    //0x0E AMP3_CONTB      (def: 2'h1   use: 0x1)
    write_mbus_register(RAD_ADDR,0x0E,0x1);  
    delay(MBUS_DELAY);
    //0x0F AMPGL_CONT      (def: 3'h3   use: 0x1)
    write_mbus_register(RAD_ADDR,0x0F,0x1);  
    delay(MBUS_DELAY);
    //0x10 CDCEN           (def: 1'h0   use: 0x0)
    write_mbus_register(RAD_ADDR,0x10,0x0);  
    delay(MBUS_DELAY);
    //0x11 CR_CONT         (def: 1'h1   use: 0x0)
    write_mbus_register(RAD_ADDR,0x11,0x0);  
    delay(MBUS_DELAY);
    //0x12 CBCAP_CONT      (def: 5'h10  use: 0x1F)
    write_mbus_register(RAD_ADDR,0x12,0x1F);  
    delay(MBUS_DELAY);
    //0x13 DUALCOMP_EN     (def: 1'h0   use: 0x0)
    write_mbus_register(RAD_ADDR,0x13,0x0);  
    delay(MBUS_DELAY);
    //0x14 COMP_SEL        (def: 1'h0   use: 0x1)
    write_mbus_register(RAD_ADDR,0x14,0x1);  
    delay(MBUS_DELAY);
    //0x15 ACCCOMPDUAL_EN  (def: 1'h0   use: 0x0)
    write_mbus_register(RAD_ADDR,0x15,0x0);  
    delay(MBUS_DELAY);
    //0x16 ACCCOMPCAP_CONT (def: 5'h00  use: 0x0)
    write_mbus_register(RAD_ADDR,0x16,0x0);  
    delay(MBUS_DELAY);
    //0x17 OSR_CONT        (def: 3'h0   use: 0x6)
    write_mbus_register(RAD_ADDR,0x17,0x6);  
    delay(MBUS_DELAY);
    //0x18 CMSP_CONT       (def: 2'h2   use: 0x2)
    write_mbus_register(RAD_ADDR,0x18,0x2);  
    delay(MBUS_DELAY);
    //0x19 CMSTUCK_EN      (def: 1'h0   use: 0x0)
    write_mbus_register(RAD_ADDR,0x19,0x0);  
    delay(MBUS_DELAY);
    //0x1A CRSTUCK_EN      (def: 1'h0   use: 0x0)
    write_mbus_register(RAD_ADDR,0x1A,0x0);  
    delay(MBUS_DELAY);

//  }

    //Release CDC Power gate
    //0x00 PG           (def: 1'h1   use: 0x0)
    write_mbus_register(RAD_ADDR,0x0,0x0);  
    delay(MBUS_DELAY);

    //Enable CDC
    //0x10 CDCEN           (def: 1'h0   use: 0x1)
    write_mbus_register(RAD_ADDR,0x10,0x1);  
    delay(MBUS_DELAY);

    //Poll CDC Done
    do{
      delay(2000);
      read_mbus_register_RADv4(RAD_ADDR,0x1D,0x17);
      asm("wfi;");
      temp_data = *((volatile uint32_t *) IMSG3);
    }
    while(!temp_data);

    // Read out CDC result
    delay(MBUS_DELAY);
    read_mbus_register_RADv4(RAD_ADDR,0x1E,0x17);
    asm("wfi;");
    temp_data = *((volatile uint32_t *) IMSG3);

    //Disable CDC
    //0x10 CDCEN           (def: 1'h0   use: 0x0)
    delay(MBUS_DELAY);
    write_mbus_register(RAD_ADDR,0x10,0x0);  
    delay(MBUS_DELAY);

    //Release CDC Power gate
    //0x00 PG           (def: 1'h1   use: 0x1)
    write_mbus_register(RAD_ADDR,0x0,0x1);  
    delay(MBUS_DELAY);

    //Fire off data to radio
    radio_data = gen_radio_data(temp_data>>0);
    send_radio_data(radio_data);

    //Set up wake up register
    *((volatile uint32_t *) 0xA2000010) = 0x00008000 + RAD_SAMPLE_DELAY;
    //Reset counter
    *((volatile uint32_t *) 0xA2000014) = 0x1;
    //Go to Sleep
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
