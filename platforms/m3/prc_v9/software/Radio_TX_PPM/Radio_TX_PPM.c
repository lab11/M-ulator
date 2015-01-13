//*********************************************************************************
//Author:		Yoonmyung Lee
//
//Date:			January 2015
//
//Description:	Radio TX code made with internal timer
//				- variably-spaced header (16bit) 
//				- use timer for precise bit timing 
//				- pulse position modulation (PPM) for data
//				ECC function also updated 
//				- capable of generating ECC code for XX-XX bit data
//				Timer configuration function updated for operation without delay
//*********************************************************************************
#include "mbus.h"
#include "SNSv3.h"
#include "PRCv9.h"
#include "RADv5.h"

#define	RAD_ADDR 	0x3
#define	SNS_ADDR 	0x4
#define	HRV_ADDR 	0x5
	
// Radio TX related definition
#define	TIME_FACTOR				50	//45 //43
#define TIME_FACTOR_11			55
#define TIME_FACTOR_09			45
#define	HEADER_LENGTH			16
#define	DATA_LENGTH				16

#define	MBUS_DELAY 				100 //Amount of delay between successive messages

#define	RAD_BIT_DELAY			12	//0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define	RAD_PACKET_DELAY		600	//1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define	WAKEUP_CYCLE			3	//6	//213:10min       //Wake up timer tuning: # of wake up timer cycles to sleep
#define	WAKEUP_CYCLE_INITIAL	3	// Wake up timer duration for initial periods
#define	NUM_INITIAL_CYCLE 		3	// Number of initial cycles
#define	DATA_BUFFER_SIZE 		256

#define STACK_ID				1

//***************************************************
// Global variables
//***************************************************
volatile uint32_t exec_marker;
volatile uint32_t exec_temp_marker;
volatile uint32_t exec_count;
volatile uint32_t exec_count_irq;

//volatile uint32_t tx_next_bit;	// Removed in PPM
volatile uint32_t tx_next_delay;
  
uint32_t _sns_r3; 
uint32_t temp_data;
uint32_t radio_data;

//***************************************************
// Timer config function
//***************************************************

void timer_init( uint8_t timer_id, uint8_t go, uint8_t roi, uint32_t init_val, uint32_t sat_val) {
	uint32_t _addr = 0xA5000000;
	_addr |= (timer_id<<4);
	// GO  = 0x0
	// SAT = 0x4
	// ROI = 0x8
	// CNT = 0xC
	*((volatile uint32_t *) (_addr | 0x0) ) = 0x0;		// stop timer first
	*((volatile uint32_t *) (_addr | 0x4) ) = sat_val;	// set up values	
	*((volatile uint32_t *) (_addr | 0x8) ) = roi;
	*((volatile uint32_t *) (_addr | 0xC) ) = init_val;
	*((volatile uint32_t *) (_addr | 0x0) ) = go;		// run
}

void timer_isr( uint32_t sat_val) {
	// only for timer ID 2 and setting sat_val
	uint32_t _addr = 0xA5000024;
	// GO  = 0x0
	// SAT = 0x4
	// ROI = 0x8
	// CNT = 0xC
	*((volatile uint32_t *) (_addr) ) = sat_val;	// set up values	
//	*((volatile uint32_t *) (_addr | 0x8) ) = roi;
//	*((volatile uint32_t *) (_addr | 0xC) ) = init_val;
//	*((volatile uint32_t *) (_addr | 0x0) ) = go;		// run
}
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
void handler_ext_int_7(void){ // Timer ID 1
	*((volatile uint32_t *) 0xE000E280) = 0x80;
}
void handler_ext_int_8(void){ // Timer ID 2
    timer_isr(tx_next_delay); 
    *((volatile uint32_t *) 0xE000E280) = 0x100;
	write_mbus_register(RAD_ADDR,0x27,0x1);	
//	write_mbus_register(0xA,0xFD,tx_next_delay);	
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
	//TX related global variable init
	//************************************
	tx_next_delay = 100;

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
	uint32_t _rad_r22 = 0x0;
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


//***************************************************
//Send Radio Data MSB-->LSB
//Two Delays:
//Bit Delay: Delay between each bit (30-bit data)
//Packet Delay: Delay between each packet (3 packets)
//Delays are nop delays (therefore dependent on core speed)
//***************************************************
static void send_radio_data(uint32_t radio_data){
	
	// set up global variable
	tx_next_delay =	16*TIME_FACTOR;

	int32_t i; // loop var

	// Need to include "memcpy" function from library for array declaration => but didn't work corrrectly
//	volatile uint32_t space[16];
//= {23,16,20,25,29,27,24,22,28,17,21,30,18,26,19,16};

	// temporary initialization routine
/*	space[0] = 23;
	space[1] = 16;
	space[2] = 20;
	space[3] = 25;
	space[4] = 29;
	space[5] = 27;
	space[6] = 24;
	space[7] = 22;
	space[8] = 28;
	space[9] = 17;
	space[10] = 21;
	space[11] = 30;
	space[12] = 18;
	space[13] = 26;
	space[14] = 19;
	space[15] = 16;
*/

// Debug routine for memcpy
/*	i = 0;

	delay(2000);
	write_mbus_register(0xA,0x00,space[0]);	
	delay(2000);
	write_mbus_register(0xA,0x00,space[1]);	
	delay(2000);
	write_mbus_register(0xA,0x00,space[2]);	
	delay(2000);
	write_mbus_register(0xA,0x00,space[3]);	

	while(1){
		delay(5000);
		write_mbus_register(0xB,0x00,space[i%4]);	
		++i;
	}
*/
	// Header
	timer_init(2, 1, 1, 0, 300); 
	tx_next_delay = /*space[0]*/23*TIME_FACTOR;		// bit0 -> bit1

/*	for( i=1; i<=HEADER_LENGTH; ++i){
		asm( "wfi;" );
		tx_next_delay = space[i]*TIME_FACTOR;
		write_mbus_register(0xD,0x66,tx_next_delay);	
		if( i == HEADER_LENGTH )
			tx_next_bit = (0x1 & (radio_data >> (DATA_LENGTH+4) ));
		else
			tx_next_bit = 0x1;
		write_mbus_register(RAD_ADDR,0x27,0x0);	
	}
*/
	// Temp Solution
	// bit 0
	asm( "wfi;" );
	tx_next_delay = /*space[1]*/16*TIME_FACTOR;		// bit1 -> bit2
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 1 
	asm( "wfi;" );
	tx_next_delay = /*space[2]*/20*TIME_FACTOR;		// bit2 -> bit3
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 2 
	asm( "wfi;" );
	tx_next_delay = /*space[3]*/25*TIME_FACTOR;		// bit3 -> bit4
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 3 
	asm( "wfi;" );
	tx_next_delay = /*space[4]*/29*TIME_FACTOR;		// bit4 -> bit5
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 4 
	asm( "wfi;" );
	tx_next_delay = /*space[5]*/27*TIME_FACTOR;		// bit5 -> bit6
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 5 
	asm( "wfi;" );
	tx_next_delay = /*space[6]*/24*TIME_FACTOR;		// bit6 -> bit7
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 6 
	asm( "wfi;" );
	tx_next_delay = /*space[7]*/22*TIME_FACTOR;		// bit7 -> bit8
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 7 
	asm( "wfi;" );
	tx_next_delay = /*space[8]*/28*TIME_FACTOR;		// bit8 -> bit9
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 8 
	asm( "wfi;" );
	tx_next_delay = /*space[9]*/17*TIME_FACTOR;		// bit9 -> bit10
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 9
	asm( "wfi;" );
	tx_next_delay = /*space[10]*/21*TIME_FACTOR;	// bit10 -> bit11
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 10
	asm( "wfi;" );
	tx_next_delay = /*space[11]*/30*TIME_FACTOR;	// bit11 -> bit12
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 11
	asm( "wfi;" );
	tx_next_delay = /*space[12]*/18*TIME_FACTOR;	// bit12 -> bit13
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 12
	asm( "wfi;" );
	tx_next_delay = /*space[13]*/26*TIME_FACTOR;	// bit13 -> bit14
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 13
	asm( "wfi;" );
	tx_next_delay = /*space[14]*/19*TIME_FACTOR;	// bit14 -> bit15
	write_mbus_register(RAD_ADDR,0x27,0x0);	
	// bit 14
	asm( "wfi;" );
	if( (radio_data>>(DATA_LENGTH-1)) & 0x1 )
		tx_next_delay = /*space[15]*/16*TIME_FACTOR;	// bit15 -> data15(MSB) 
	else
		tx_next_delay = 16*TIME_FACTOR_11;
	write_mbus_register(RAD_ADDR,0x27,0x0);	

	uint32_t radio_data_ip1, radio_data_i;

	// Data: when index is i-> data[i+2] is transmitted at wfi, then set up delay between data[i+1] and data[i]
	for( i=DATA_LENGTH-2; i>=0; --i){
		asm( "wfi;" );
		radio_data_ip1 = (radio_data>>(i+1))&0x1;	// data[i+1]
		radio_data_i   = (radio_data>>(i))  &0x1;	// data[i]
		if( radio_data_ip1==radio_data_i )			// data[i+1],data[i] = (1,1) OR (0,0)
			tx_next_delay = 16*TIME_FACTOR;
		else if( radio_data_ip1==1 )				// data[i+1],data[i] = (1,0)
			tx_next_delay = 16*TIME_FACTOR_11;
		else 										// data[i+1],data[i] = (0,1)
			tx_next_delay = 16*TIME_FACTOR_09;
		delay (10);
		write_mbus_register(RAD_ADDR,0x27,0x0);	
	}

	//data[1] 
	asm( "wfi;" );
	tx_next_delay = 32*TIME_FACTOR;				// arbitrary large number to reset timer right after finishing for loop execution
	write_mbus_register(RAD_ADDR,0x27,0x0);	

	//data[0]
	asm( "wfi;" );
	timer_init(2, 0, 0, 0, tx_next_delay); 
	write_mbus_register(RAD_ADDR,0x27,0x0);	

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
	send_radio_data( generate_ECC(temp_data) );

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

	// Disable wakeup timer
	set_wakeup_timer(0,0,1);

	
	//Check if it is the first execution
	if ( exec_marker != 0x12345678 ) {

		//Enumeration (RAD->SNS->HRV)
		enumerate(RAD_ADDR);	
		delay(MBUS_DELAY*20);
		enumerate(SNS_ADDR);
		delay(MBUS_DELAY*20);
//		enumerate(HRV_ADDR);

		// Setup Radio & Tempsensor	
		setup_radio();
		setup_tempsensor();

		//Mark execution
		exec_marker = 0x12345678;
		exec_count = 0;
		exec_count_irq = 0;

	}else{
    	// Repeating wakeup routine 
    	exec_count = exec_count + 1;
	}


	//Radio Test code
	send_radio_data(0xF0F0F0F0);
	delay(100);
	set_wakeup_timer(WAKEUP_CYCLE,1,0);
	operation_sleep();
	while(1);
	
}
