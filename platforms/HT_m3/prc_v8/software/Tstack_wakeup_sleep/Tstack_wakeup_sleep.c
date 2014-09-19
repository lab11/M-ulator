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
  
  static uint32_t temp_data_stored[DATA_BUFFER_SIZE] = {0};
  static uint32_t temp_data_count;
  uint32_t _sns_r3; 
  uint32_t temp_data;
  uint32_t radio_data;

//***************************************************
//Interrupt Handlers
//Must clear pending bit!
//***************************************************
void handler_ext_int_0(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_4(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_5(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_6(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_7(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_8(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_9(void) __attribute__ ((interrupt ("IRQ")));
// INT vector 10 and 11 is not real interrupt vectors
// These vector location (0x0068 and 0x006C) are used as on demaned request parameter buffer
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));

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
void handler_ext_int_4(void){
  *((volatile uint32_t *) 0xE000E280) = 0x10;
}
void handler_ext_int_5(void){
  *((volatile uint32_t *) 0xE000E280) = 0x20;
}
void handler_ext_int_6(void){
  *((volatile uint32_t *) 0xE000E280) = 0x40;
}
void handler_ext_int_7(void){
  *((volatile uint32_t *) 0xE000E280) = 0x80;
}
void handler_ext_int_8(void){
  *((volatile uint32_t *) 0xE000E280) = 0x100;
}
void handler_ext_int_9(void){
  *((volatile uint32_t *) 0xE000E280) = 0x200;
}
void handler_ext_int_10(void){
  *((volatile uint32_t *) 0xE000E280) = 0x400;
}
void handler_ext_int_11(void){
  *((volatile uint32_t *) 0xE000E280) = 0x800;
}


//***************************************************
// Internal Functions
//***************************************************


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
  delay(MBUS_DELAY);
  sleep();
  while(1);

}



//***************************************************
// Main
//***************************************************
int main() {

  // Repeating wakeup routine 

  delay(10000);
  set_wakeup_timer(50,1,0);
  operation_sleep();

  while(1);

}
