//*******************************************************************
//Author: Yoonmyung Lee
//        Gyouho Kim
//        Zhiyoong Foo
//
//Date: October 2014
//
//Description: 	Demonstration code for Terraswarm near-field radio
//              To be used with USRP radio receiver
//              Measures & Transmitts temperature every configurable period
//
//*******************************************************************
#include "mbus.h"
#include "SNSv3.h"
#include "PRCv9.h"
#include "RADv5.h"

#define	RAD_ADDR 0x3
#define	SNS_ADDR 0x4
#define	HRV_ADDR 0x5

#define	MBUS_DELAY 100 //Amount of delay between successive messages

#define	RAD_BIT_DELAY			12	//0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define	RAD_PACKET_DELAY		600	//1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define	WAKEUP_CYCLE			10//6	//213:10min       //Wake up timer tuning: # of wake up timer cycles to sleep
#define	WAKEUP_CYCLE_INITIAL	3	// Wake up timer duration for initial periods
#define	NUM_INITIAL_CYCLE 		3	// Number of initial cycles
#define	DATA_BUFFER_SIZE 		256

#define STACK_ID				2

//***************************************************
// Global variables
//***************************************************
volatile uint32_t exec_marker;
volatile uint32_t exec_temp_marker;
volatile uint32_t exec_count;
volatile uint32_t exec_count_irq;
  
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
// Configuration for temperature sensor (reusing code for SNSv2)
//***************************************************
static void setup_tempsensor(void) {

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

	// Change mbus interrupt address for temp sensor and cdc to 0x15 and 0x16
	uint32_t _sns_r6 = (0x9<<16)|(0x16<<8)|(0x15<<0);
	write_mbus_register(SNS_ADDR,6,_sns_r6);
	delay(MBUS_DELAY);

}

//***************************************************
// Configuration setup for radio (Reusing code for RADv4)
//***************************************************
static void setup_radio(void) {

	//************************************
	//RADv4 Register Defaults
	//************************************
	delay(MBUS_DELAY);
	
	//Ext Ctrl En
	uint32_t _rad_r23 = 0x0;
	write_mbus_register(RAD_ADDR,0x23,_rad_r23);
	delay(MBUS_DELAY);
  
	//Current Limiter  
	uint32_t _rad_r26 = 0x1F; // 2F is 30uA, 1F is 3uA
	write_mbus_register(RAD_ADDR,0x26,_rad_r26);  
	delay(MBUS_DELAY);

	//Tune Power
	uint32_t _rad_r20 = 0x1F;
	write_mbus_register(RAD_ADDR,0x20,_rad_r20);
	delay(MBUS_DELAY);
  
  
	// For board level testing, 00 corresponds to 905MHz
	//Tune Freq 1
	uint32_t _rad_r21 = 0x0;
	write_mbus_register(RAD_ADDR,0x21,_rad_r21);
	delay(MBUS_DELAY);

	//Tune Freq 2
	uint32_t _rad_r22 = 0xA;
	write_mbus_register(RAD_ADDR,0x22,_rad_r22);
	delay(MBUS_DELAY);
 
	//Tune TX Time
	uint32_t _rad_r25 = 0x5;
	write_mbus_register(RAD_ADDR,0x25,_rad_r25);
	delay(MBUS_DELAY);
	
	//Zero the TX register
	write_mbus_register(RAD_ADDR,0x27,0x0);
	delay(MBUS_DELAY);
}

//***************************************************
// Radio Transmission Data formatting
//***************************************************
// Output = 30 bit data to be radioed
// Output[29:26] = 4 bit  preamble
// Output[25:22] = 4 bit  counter
// Output[21:20] = 2 bit  stack ID
// Output[19:6]  = 14 bit temperature value
// Output[5:0]   = 6 bit  ECC for (4 bit counter + 2 bit Stack ID + 14 bit data) portion

//ECC Algorithm: (P = parity) (D = data)
//SEC requires 5 bits
//DED requires 1 additional for total parity check
//SEC Section:
//P[0] :0 : 00000 = 1^2^3^4^5^6^7^8^9^10^11^12^13^14^15^16^17^18^19^20^21^22^23^24^25
//P[1] :1 : 00001 = 3^5^7^9^11^13^15^17^19^21^23^25
//P[2] :2 : 00010 = 3^6^7^10^11^14^15^18^19^22^23
//D[0] :3 : 00011
//P[3] :4 : 00100 = 5^6^7^12^13^14^15^20^21^22^23
//D[1] :5 : 00101
//D[2] :6 : 00110
//D[3] :7 : 00111
//P[4] :8 : 01000 = 9^10^11^12^13^14^15^24^25
//D[4] :9 : 01001
//D[5] :10: 01010
//D[6] :11: 01011
//D[7] :12: 01100
//D[8] :13: 01101
//D[9] :14: 01110
//D[10]:15: 01111
//P[5] :16: 10000 = 17^18^19^20^21^22^23^24^25
//D[11]:17: 10001
//D[12]:18: 10010
//D[13]:19: 10011
//D[14]:20: 10100
//D[15]:21: 10101
//D[16]:22: 10110
//D[17]:23: 10111
//D[18]:24: 11000
//D[19]:25: 11001

static uint32_t gen_radio_data(uint32_t data_in, uint32_t count) {
	uint32_t count_trim = count & 0x000000FF;
	uint32_t data_in_trim = data_in & 0x00003FFF;
	uint32_t stack_id_trim = STACK_ID & 0x3;
	uint32_t data_out =  0x3C000000 | count_trim<<22 | stack_id_trim<<20 | data_in_trim<<6;
	uint32_t ecc_data_in = count_trim<<16 | stack_id_trim<<14 | data_in_trim;	
	uint32_t P5 = 
		((ecc_data_in>>19)&0x1) ^ 
		((ecc_data_in>>18)&0x1) ^ 
		((ecc_data_in>>17)&0x1) ^ 
		((ecc_data_in>>16)&0x1) ^ 
		((ecc_data_in>>15)&0x1) ^ 
		((ecc_data_in>>14)&0x1) ^ 
		((ecc_data_in>>13)&0x1) ^ 
		((ecc_data_in>>12)&0x1) ^ 
		((ecc_data_in>>11)&0x1);
	uint32_t P4 = 
		((ecc_data_in>>19)&0x1) ^ 
		((ecc_data_in>>18)&0x1) ^ 
		((ecc_data_in>>10)&0x1) ^ 
		((ecc_data_in>>9 )&0x1) ^ 
		((ecc_data_in>>8 )&0x1) ^ 
		((ecc_data_in>>7 )&0x1) ^ 
		((ecc_data_in>>6 )&0x1) ^ 
		((ecc_data_in>>5 )&0x1) ^ 
		((ecc_data_in>>4 )&0x1);
	uint32_t P3 = 
		((ecc_data_in>>17)&0x1) ^ 
		((ecc_data_in>>16)&0x1) ^ 
		((ecc_data_in>>15)&0x1) ^ 
		((ecc_data_in>>14)&0x1) ^ 
		((ecc_data_in>>10)&0x1) ^ 
		((ecc_data_in>>9 )&0x1) ^ 
		((ecc_data_in>>8 )&0x1) ^ 
		((ecc_data_in>>7 )&0x1) ^ 
		((ecc_data_in>>3 )&0x1) ^ 
		((ecc_data_in>>2 )&0x1) ^ 
		((ecc_data_in>>1 )&0x1);
	uint32_t P2 = 
		((ecc_data_in>>17)&0x1) ^ 
		((ecc_data_in>>16)&0x1) ^ 
		((ecc_data_in>>13)&0x1) ^ 
		((ecc_data_in>>12)&0x1) ^ 
		((ecc_data_in>>10)&0x1) ^ 
		((ecc_data_in>>9 )&0x1) ^ 
		((ecc_data_in>>6 )&0x1) ^ 
		((ecc_data_in>>5 )&0x1) ^ 
		((ecc_data_in>>3 )&0x1) ^ 
		((ecc_data_in>>2 )&0x1) ^ 
		((ecc_data_in>>0 )&0x1);
	uint32_t P1 = 
		((ecc_data_in>>19)&0x1) ^ 
		((ecc_data_in>>17)&0x1) ^ 
		((ecc_data_in>>15)&0x1) ^ 
		((ecc_data_in>>13)&0x1) ^ 
		((ecc_data_in>>11)&0x1) ^ 
		((ecc_data_in>>10)&0x1) ^ 
		((ecc_data_in>>8 )&0x1) ^
		((ecc_data_in>>6 )&0x1) ^ 
		((ecc_data_in>>4 )&0x1) ^ 
		((ecc_data_in>>3 )&0x1) ^ 
		((ecc_data_in>>1 )&0x1) ^ 
		((ecc_data_in>>0 )&0x1);
	uint32_t P0 = 
		((ecc_data_in>>19)&0x1) ^ 
		((ecc_data_in>>18)&0x1) ^ 
		((ecc_data_in>>17)&0x1) ^ 
		((ecc_data_in>>16)&0x1) ^ 
		((ecc_data_in>>15)&0x1) ^ 
		((ecc_data_in>>14)&0x1) ^ 
		((ecc_data_in>>13)&0x1) ^ 
		((ecc_data_in>>12)&0x1) ^ 
		((ecc_data_in>>11)&0x1) ^ 
		((ecc_data_in>>10)&0x1) ^ 
		((ecc_data_in>>9)&0x1) ^ 
		((ecc_data_in>>8)&0x1) ^ 
		((ecc_data_in>>7)&0x1) ^ 
		((ecc_data_in>>6)&0x1) ^ 
		((ecc_data_in>>5)&0x1) ^ 
		((ecc_data_in>>4)&0x1) ^ 
		((ecc_data_in>>3)&0x1) ^ 
		((ecc_data_in>>2)&0x1) ^ 
		((ecc_data_in>>1)&0x1) ^ 
		((ecc_data_in>>0)&0x1) ^ 
		P5 ^ P4 ^ P3 ^ P2 ^ P1 ;
	data_out |= (P5<<5)|(P4<<4)|(P3<<3)|(P2<<2)|(P1<<1)|(P0<<0);
	return data_out;
}


//***************************************************
//Send Radio Data MSB-->LSB
//Two Delays:
//Bit Delay: Delay between each bit (30-bit data)
//Packet Delay: Delay between each packet (3 packets)
//Delays are nop delays (therefore dependent on core speed)
//***************************************************
static void send_radio_data(uint32_t radio_data){
	int32_t i; //loop var
	uint32_t j; //loop var
	for(j=0;j<1;j++){ //Packet Loop
		for(i=29;i>=0;i--){ //Bit Loop
			delay(10);
			if ((radio_data>>i)&1) write_mbus_register(RAD_ADDR,0x27,0x1);
			else                   write_mbus_register(RAD_ADDR,0x27,0x0);

		    //Must clear register
			delay(RAD_BIT_DELAY);
			write_mbus_register(RAD_ADDR,0x27,0x0);
			delay(RAD_BIT_DELAY); //Set delay between sending subsequent bit
		}
    //delay(RAD_PACKET_DELAY); //Set delays between sending subsequent packet
	}
}

//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){

  // Reset wakeup counter
  // This is required to go back to sleep!!
  *((volatile uint32_t *) 0xA2000014) = 0x1;

  // Reset IRQ10VEC
  *((volatile uint32_t *) IRQ10VEC) = 0;
  
  // Disable RADIO tx
  write_mbus_register(RAD_ADDR,0x27,0x0);

  // Go to Sleep
  delay(MBUS_DELAY);
  sleep();
  while(1);

}

static void operation_sleep_noirqreset(void){

  // Reset wakeup counter
  // This is required to go back to sleep!!
  *((volatile uint32_t *) 0xA2000014) = 0x1;

  // Go to Sleep
  sleep();
  while(1);

}

//***************************************************
// Periodic operation (SNSv2)
//***************************************************
static void operation_periodic(void){

	// Check if wakeup is due to temperature sensor interrupt
	// If so, then skip this section

	if ( exec_temp_marker != 0x87654321 ) {
		// This wakeup is just to enable temp sensor
		exec_count = exec_count - 1;

		// Set exec_temp_marker
		exec_temp_marker = 0x87654321;

		//Enable T Sensor
		_sns_r3 = (0x3<<17)|(0x0<<16)|(0xF<<12)|(0x0<<8)|(0xF<<4)|(0x0<<0);
		delay(MBUS_DELAY);
		write_mbus_register(SNS_ADDR,3,_sns_r3);
		
		operation_sleep();
	}

	delay(MBUS_DELAY);

	// Grab Data after IRQ
	temp_data = *((volatile uint32_t *) IMSG1);

/*
	// Store in memory
	// If the buffer is full, then skip
	if (temp_data_count<DATA_BUFFER_SIZE){
		temp_data_stored[temp_data_count] = temp_data;
		temp_data_count = temp_data_count + 1;
	}
*/

	// Disable T Sensor
	_sns_r3 = (0x3<<17)|(0x1<<16)|(0xF<<12)|(0x0<<8)|(0xF<<4)|(0x0<<0);
	delay(MBUS_DELAY);
	write_mbus_register(SNS_ADDR,3,_sns_r3);

	// Reset exec_temp_marker
	exec_temp_marker = 0;

	// Fire off data to radio
	send_radio_data( gen_radio_data(temp_data>>1, exec_count) );

	// Set up timer for next wake up
	set_wakeup_timer(WAKEUP_CYCLE,1,0);

	operation_sleep();

}



//***************************************************
// Main
//***************************************************
int main() {

	//Clear All Pending Interrupts
	*((volatile uint32_t *) 0xE000E280) = 0x3FF;
	//Enable Interrupts
	*((volatile uint32_t *) 0xE000E100) = 0x3FF;

	//Check if it is the first execution
	if ( exec_marker != 0x12345678 ) {

		// Initial routine
		// Set division 6 override for PMU
		pmu_div6_ovrd(1);

		//Enumeration (SNS->HRV->RAD)
		enumerate(RAD_ADDR);	
		delay(MBUS_DELAY*20);
		enumerate(SNS_ADDR);
		delay(MBUS_DELAY*20);
		enumerate(HRV_ADDR);

		// Setup Radio & Tempsensor	
		setup_radio();
		setup_tempsensor();

		//Mark execution
		exec_marker = 0x12345678;
		exec_count = 0;
		exec_count_irq = 0;

	}else{
		// Disable wakeup timer
		set_wakeup_timer(0,0,1);

    	// Repeating wakeup routine 
    	exec_count = exec_count + 1;
	}

	// Real code
	operation_periodic();
	while(1);


	// Debug code
	uint32_t data = 0xFFFF0000;


	send_radio_data(data + exec_count);
	//send_radio_data(0xFFFFFFFF);
	exec_count++;
	set_wakeup_timer(WAKEUP_CYCLE,1,0);
	operation_sleep();


  // Repeating wakeup routine 
/*
  // Check if wakeup is due to GOC interrupt
  // 0x68 is reserved for GOC-triggered wakeup (Named IRQ10VEC)
  // 8 MSB bits of the wakeup data are used for function ID
  uint32_t wakeup_data = *((volatile uint32_t *) IRQ10VEC);
  uint32_t wakeup_data_header = wakeup_data>>24;

  if(wakeup_data_header == 1){
    // Debug mode: Transmit something via radio 16 times and go to sleep w/o timer
    if (exec_count_irq < 16){
      exec_count_irq++;
      // radio
//    send_radio_data(0xF0F0F0F0);
      send_radio_data (gen_radio_data(0x6C));
      // set timer
      set_wakeup_timer (WAKEUP_CYCLE_INITIAL, 0x1, 0x0);
      // go to sleep and wake up with same condition
      operation_sleep_noirqreset();
    }else{
      exec_count_irq = 0;
      // radio
//    send_radio_data(0xF0F0F0F0);
      send_radio_data (gen_radio_data(0x6C));
      // Disable Timer
      set_wakeup_timer (0, 0x0, 0x0);
      // Go to sleep without timer
      operation_sleep();
    }

  }else if(wakeup_data_header == 2){
      operation_radiotest();

  }else if(wakeup_data_header == 3){
      // Disable Timer
      set_wakeup_timer (0, 0x0, 0x0);
      // Go to sleep without timer
      operation_sleep();

  }else if(wakeup_data_header == 4){
      // radiotest
      operation_radiotest();

  }else{
      // Default case 
      operation_radiotest();
  }

  // Note: Program should send system to sleep in the switch statement
  // Program should not reach this point
*/
  while(1);
}
