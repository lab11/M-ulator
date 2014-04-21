//*******************************************************************
//Author: Yoonmyung Lee
//        Zhiyoong Foo
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
#define MBUS_DELAY 50 //Amount of delay between successive messages

//Internal Functions
static void delay(unsigned ticks) {
  unsigned i;
  for (i=0; i < ticks; i++)
    asm("nop;");
}



int main() {

  uint32_t temp_data;
  uint32_t radio_data;
  uint32_t exec_marker;
  uint32_t exec_count;
  uint32_t _sns_r3; 
  
  //Clear All Pending Interrupts
  *((volatile uint32_t *) 0xE000E280) = 0xF;
  //Enable Interrupts
  *((volatile uint32_t *) 0xE000E100) = 0xF;



    //Enumeration
    enumerate(RAD_ADDR);
    delay(MBUS_DELAY);
    enumerate(SNS_ADDR);

    //Set up Radio
    //************************************
    //RADv4 Register Defaults
    //************************************
    delay(MBUS_DELAY);
    //Ext Ctrl En
    uint32_t _rad_r23 = 0x0;
    write_mbus_register(RAD_ADDR,0x23,_rad_r23);
    delay(MBUS_DELAY);

    //Current Limiter 0x1F=3.6uA  0x2F=36uA
    uint32_t _rad_r26 = 0x1F;
    write_mbus_register(RAD_ADDR,0x26,_rad_r26);  
    delay(MBUS_DELAY);
    //Tune Power
    uint32_t _rad_r20 = 0x1F;
    write_mbus_register(RAD_ADDR,0x20,_rad_r20);
    delay(MBUS_DELAY);

    // For Hassan's receiver board, Use 0x20 for Tb-stacks
    //Tune Freq 1
    uint32_t _rad_r21 = 0x2;
    write_mbus_register(RAD_ADDR,0x21,_rad_r21);
    delay(MBUS_DELAY);
    //Tune Freq 2
    uint32_t _rad_r22 = 0x0;
    write_mbus_register(RAD_ADDR,0x22,_rad_r22);
    delay(MBUS_DELAY);

    //Tune TX Time
    uint32_t _rad_r25 = 0x4;
    write_mbus_register(RAD_ADDR,0x25,_rad_r25);
    delay(MBUS_DELAY);

    //Zero the TX register
    write_mbus_register(RAD_ADDR,0x27,0x0);
    delay(MBUS_DELAY);


    //Set up wake up register
    *((volatile uint32_t *) 0xA2000010) = 0x00008000 + 4;

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
