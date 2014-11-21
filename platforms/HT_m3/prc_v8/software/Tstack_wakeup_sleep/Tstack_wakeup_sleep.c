//*******************************************************************
//Author: Yoonmyung Lee
//        Zhiyoong Foo
//        Gyouho Kim
//
//Date: April 2014
//
//Description: 	Derived from Tstack_longterm_shortwake code
//
//*******************************************************************
#include "mbus.h"
#include "PRCv8.h"
//#include "m3_proc.h"

#define RAD_ADDR 0x3
#define SNS_ADDR 0x4

#define MBUS_DELAY 100 //Amount of delay between successive messages

#define RAD_BIT_DELAY 40     //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 600  //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define TEMP_WAKEUP_CYCLE 10     //2//213:10min       //Wake up timer tuning: # of wake up timer cycles to sleep
#define TEMP_WAKEUP_CYCLE_INITIAL 2 // Wake up timer duration for initial periods
#define NUM_INITIAL_CYCLE 2 // Number of initial cycles
#define DATA_BUFFER_SIZE 256

//***************************************************
// Global variables
//***************************************************
  static uint32_t exec_marker;
  static uint32_t exec_temp_marker;
  static uint32_t exec_count;
  

//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){

  // Reset wakeup counter
  // This is required to go back to sleep!!
  *((volatile uint32_t *) 0xA2000014) = 0x1;

  // Reset IRQ10VEC
  *((volatile uint32_t *) IRQ10VEC) = 0;

  // Go to Sleep
  //delay(MBUS_DELAY);
  sleep();
  while(1);

}



//***************************************************
// Main
//***************************************************
int main() {

  // Repeating wakeup routine 

  delay(10000);
  set_wakeup_timer(100,1,0);
  operation_sleep();

  while(1);

}
