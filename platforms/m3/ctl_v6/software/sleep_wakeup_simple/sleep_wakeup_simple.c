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
#define MBUS_DELAY 100 //Amount of delay between successive messages

//Internal Functions
static void delay(unsigned ticks) {
  unsigned i;
  for (i=0; i < ticks; i++)
    asm("nop;");
}



int main() {


    //Enumeration
    delay(MBUS_DELAY);
    enumerate(SNS_ADDR);

    delay(4);
    delay(2);
    //delay(2);

    // Dummy MBUS Msg to invalid address
    write_mbus_register(0x38,0x23,0x0);
    delay(MBUS_DELAY);
    enumerate(RAD_ADDR);



    //Set up wake up register
    *((volatile uint32_t *) 0xA2000010) = 0x00008000 + 3;

    //Reset counter
    *((volatile uint32_t *) 0xA2000014) = 0x1;

    //Go to Sleep
    delay(MBUS_DELAY);
    sleep();
    while(1);



}
