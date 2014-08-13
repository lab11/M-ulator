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


    delay(MBUS_DELAY);

    delay(4);
    delay(2);

    enumerate(SNS_ADDR);
    //*((volatile uint32_t *) 0xA200000C) = 0x0F773029;

    delay(20000);
    // Dummy MBUS Msg to invalid address
    write_mbus_register(0x38,0x23,0x0);
    delay(MBUS_DELAY);
    write_mbus_register(0x38,0x23,0x0);
    delay(MBUS_DELAY);
    write_mbus_register(0x38,0x23,0x0);
    delay(MBUS_DELAY);
    write_mbus_register(0x38,0x23,0x0);
    delay(MBUS_DELAY);
    write_mbus_register(0x38,0x23,0x0);
    delay(MBUS_DELAY);
    delay(20000);
    //enumerate(RAD_ADDR);



    // Set PMU Strength & division threshold
    // Change PMU_CTRL Register
    // 0x0F770029 = Original
    // Increase sleep oscillator frequency to provide enough power for temp sensor
    //*((volatile uint32_t *) 0xA200000C) = 0xF77007B;

    //Set up wake up register
    *((volatile uint32_t *) 0xA2000010) = 0x00008000 + 3;

    //Reset counter
    *((volatile uint32_t *) 0xA2000014) = 0x1;

    //Go to Sleep
    delay(MBUS_DELAY);
    sleep();
    while(1);



}
