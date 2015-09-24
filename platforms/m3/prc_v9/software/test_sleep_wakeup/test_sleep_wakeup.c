//*******************************************************************
//Author: Gyouho Kim / ZhiYoong Foo
//Description:	Simple program to test basic functionality
//*******************************************************************
#include "PRCv9.h"
#include "mbus.h"

#define RAD_ADDR 0x4
#define SNS_ADDR 0xA

#define DELAY_1 20000 //20000 // 1s

static void operation_sleep(void){

  // Reset wakeup counter
  // This is required to go back to sleep!!
  //set_wakeup_timer (0xFFF, 0x0, 0x1);
  *((volatile uint32_t *) 0xA2000014) = 0x1;

  // Reset IRQ10VEC
  *((volatile uint32_t *) IRQ10VEC/*IMSG0*/) = 0;

  // Go to Sleep
  sleep();
  while(1);

}

static void operation_enumerate(void){

	//Enumeration
	enumerate(RAD_ADDR);
    delay(1000);
	//enumerate(SNS_ADDR);
}


//*******************************************************************
//Main **************************************************************
//*******************************************************************
int main() {

    //Clear All Pending Interrupts
    *((volatile uint32_t *) 0xE000E280) = 0xF;
    //Enable Interrupts
    *((volatile uint32_t *) 0xE000E100) = 0xF;
  
    // Set PMU Strength & division threshold
    // Change PMU_CTRL Register
    // PRCv9 Default: 0x8F770049
    // Decrease 5x division switching threshold
    //*((volatile uint32_t *) 0xA200000C) = 0x8F77204B;
  
    delay(100);
	write_mbus_message(0xAA, 0xFA);
    delay(100);

	//Config watchdog timer to about 10 sec: 1,000,000 with default PRCv9
	//config_timer( timer_id, go, roi, init_val, sat_val )
	config_timer( 0, 1, 0, 0, 1000000 );

	//operation_enumerate();  

	delay(DELAY_1);
	write_mbus_message(0xAA, 0xFA);
	
  
	write_mbus_message(0xAA, 0xFA);

	set_wakeup_timer (2, 0x1, 0x0);
	operation_sleep();

	while(1);

}
