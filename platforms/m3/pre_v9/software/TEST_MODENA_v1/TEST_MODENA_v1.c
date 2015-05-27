//****************************************************************************************************
//Author:       Gyouho Kim
//				Yejoong Kim
//              ZhiYoong Foo
//Description:  For testing Modena
//				5/27/2015: Revision 1
//				- Basic functionality without interfacing with FLSv1
//****************************************************************************************************
#include "mbus.h"
#include "PRCv9E.h"
#include "RADv5.h"
#include "FLSv1_GPIO.h"

// GPIO Pin Assignment
#define MOD_DI 		0
#define MOD_DO 		1
#define MOD_CLK		2
#define MOD_RST		4
#define MOD_SCAN	5

// Stack order  PRC->MD->RAD->HRV
#define RAD_ADDR 0x5

// Common parameters
#define	MBUS_DELAY 200 //Amount of delay between successive messages
#define WAKEUP_DELAY 10000 // 20s
#define WAKEUP_DELAY_FINAL 5000	// Delay for waiting for internal decaps to stabilize after waking up MDSENSOR
#define DELAY_1 5000 // 5000: 0.5s
#define DELAY_IMG 40000 // 1s
			
#define MOD_GPIO_DELAY 100 

// FLSv1 configurations
#define FLS_RECORD_LENGTH 0x1FFE // In words; # of words stored -2
#define FLS_RADIO_LENGTH 50 // In words

//#define FLS_RECORD_LENGTH 0x10 

// Radio configurations
#define RAD_BIT_DELAY       14     //40      //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 	2000      //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define RAD_PACKET_NUM      1      //How many times identical data will be TXed

//***************************************************
// Global variables
//***************************************************
	//Test Declerations
	// "static" limits the variables to this file, giving compiler more freedom
	// "volatile" should only be used for MMIO
	static uint32_t enumerated;
	static uint32_t exec_count;
	static uint32_t exec_count_irq;
	static uint32_t MBus_msg_flag;
  
	static uint32_t WAKEUP_PERIOD_CONT; 
	static uint32_t WAKEUP_PERIOD_CONT_INIT; 

	static uint32_t radio_tx_count;
	static uint32_t radio_tx_option;
	static uint32_t radio_tx_numdata;

	static uint32_t md_count;
	static uint8_t md_capture_img;
	static uint8_t md_start_motion;
	
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
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0(void){
    *((volatile uint32_t *) 0xE000E280) = 0x1;
    MBus_msg_flag = 0x10;
}
void handler_ext_int_1(void){
    *((volatile uint32_t *) 0xE000E280) = 0x2;
    MBus_msg_flag = 0x11;
}
void handler_ext_int_2(void){
    *((volatile uint32_t *) 0xE000E280) = 0x4;
    MBus_msg_flag = 0x12;
}
void handler_ext_int_3(void){
    *((volatile uint32_t *) 0xE000E280) = 0x8;
    MBus_msg_flag = 0x13;
}
void handler_ext_int_4(void){ *((volatile uint32_t *) 0xE000E280) = 0x10; }
void handler_ext_int_5(void){ *((volatile uint32_t *) 0xE000E280) = 0x20; }
void handler_ext_int_6(void){ *((volatile uint32_t *) 0xE000E280) = 0x40; }
void handler_ext_int_7(void){ *((volatile uint32_t *) 0xE000E280) = 0x80; }
void handler_ext_int_8(void){ *((volatile uint32_t *) 0xE000E280) = 0x100; }
void handler_ext_int_9(void){
	*((volatile uint32_t *) 0xE000E280) = 0x200;
	*((volatile uint32_t *) 0xA8000000) = 0x0;
}
void handler_ext_int_10(void){ *((volatile uint32_t *) 0xE000E280) = 0x400; }
void handler_ext_int_11(void){ *((volatile uint32_t *) 0xE000E280) = 0x800; }


//************************************
// GPIO Related Functions
//************************************

void MOD_GPIO_initialization(){
	// GPIO_DIR = 0 (output) (ex. FLS_CIN, FLS_DIN)
	//            1 (input) (ex. FLS_COUT, FLS_DOUT)
	uint32_t gpio_dir = (0x00000000 | (1 << MOD_DO));
	init_GPIO(gpio_dir);
}

void MOD_GPIO_reset(){
	set_GPIO_WRITE(0x00000000);
	delay(MOD_GPIO_DELAY);
	set_GPIO_bit(MOD_CLK);
	delay(MOD_GPIO_DELAY);
	set_GPIO_bit(MOD_RST);
	delay(MOD_GPIO_DELAY*2);
	kill_GPIO_bit(MOD_CLK);
	delay(MOD_GPIO_DELAY*2);
	set_GPIO_bit(MOD_CLK);
	delay(MOD_GPIO_DELAY*2);
	kill_GPIO_bit(MOD_RST);
	delay(MOD_GPIO_DELAY*2);
	kill_GPIO_bit(MOD_CLK);
	delay(MOD_GPIO_DELAY);
	delay(MOD_GPIO_DELAY);
}

void MOD_GPIO_toggle (volatile uint32_t data, volatile uint32_t numBit) {
	volatile uint32_t i;
	volatile uint32_t pos = 1 << (numBit - 1);

	for (i=0; i<numBit; i++){
		if (data & pos) set_GPIO_bit(MOD_DIN);
		else kill_GPIO_bit(MOD_DIN);
		delay(MOD_GPIO_DELAY);
		kill_GPIO_bit(MOD_CLK);
		delay(MOD_GPIO_DELAY);
		set_GPIO_bit(MOD_CLK);
		delay(MOD_GPIO_DELAY);
		pos = pos >> 1;
	}
}

void MOD_GPIO_sof () {
	// DIN goes high while CLK low, followed by posedge clk
	kill_GPIO_bit(MOD_CLK);
	delay(MOD_GPIO_DELAY);
	set_GPIO_bit(MOD_DIN);
	delay(MOD_GPIO_DELAY);
	set_GPIO_bit(MOD_CLK);
	delay(MOD_GPIO_DELAY);
}

void MOD_GPIO_eof () {
	// DIN goes high, negedge clk, then goes low while clk low
	set_GPIO_bit(MOD_DIN);
	delay(MOD_GPIO_DELAY);
	kill_GPIO_bit(MOD_CLK);
	delay(MOD_GPIO_DELAY);
	kill_GPIO_bit(MOD_DIN);
	delay(MOD_GPIO_DELAY);
}

void MOD_GPIO_ack () {
	uint32_t i;
	for (i=0; i<10; i++){
		if (rxBit){ 
			// Acknowledge received
			MOD_GPIO_toggle(0,1);
			return;
		}
	}
	// Timed out
	MOD_GPIO_reset;
}

void MOD_GPIO_sendMSG (volatile uint32_t data_0) {
	MOD_GPIO_sof();
	MOD_GPIO_toggle(data_0,8);
	MOD_GPIO_ack();
	MOD_GPIO_eof();
}

static void operation_modena(void){
    MOD_GPIO_sendMSG(0x55);
}

//************************************
// PMU Related Functions
//************************************
inline static void set_pmu_sleep_clk_low(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F770039; // 0x8F770039: use GOC x0.6-2
}
inline static void set_pmu_sleep_clk_default(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F770049; // 0x8F770049: use GOC x10-25
}
inline static void set_pmu_sleep_clk_fastest(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F770079; // 0x8F770079: use GOC x70-230
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
  for(j=0;j<1;j++){ //Packet Loop
    for(i=31;i>=0;i--){ //Bit Loop
      if ((radio_data>>i)&1) write_mbus_register(RAD_ADDR,0x27,0x1);
      else                   write_mbus_register(RAD_ADDR,0x27,0x0);
      //Must clear register
      delay(RAD_BIT_DELAY);
      write_mbus_register(RAD_ADDR,0x27,0x0);
      delay(RAD_BIT_DELAY); //Set delay between sending subsequent bit
    }
  }
}


//***************************************************
// End of Program Sleep Operation
//***************************************************
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

static void operation_sleep_noirqreset(void){

  // Reset wakeup counter
  // This is required to go back to sleep!!
  *((volatile uint32_t *) 0xA2000014) = 0x1;

  // Go to Sleep
  sleep();
  while(1);

}

static void operation_sleep_notimer(void){
    
  // Disable Timer
  set_wakeup_timer (0, 0x0, 0x0);
  
	// Reset IRQ10VEC
  *((volatile uint32_t *) IRQ10VEC/*IMSG0*/) = 0;

  // Go to sleep without timer
  operation_sleep();

}

static void operation_init(void){
	volatile uint32_t temp_addr;
	volatile uint32_t temp_data;
	volatile uint32_t temp_numBit;
  
    // Set PMU Strength & division threshold
    // PMU_CTRL Register
    // PRCv9 Default: 0x8F770049
    // Decrease 5x division switching threshold
	set_pmu_sleep_clk_default();
  
    // Speed up GOC frontend to match PMU frequency
	// Speed up MBUS
	// GOC_CTRL Register
    // PRCv9 Default: 0x00202903
	//*((volatile uint32_t *) 0xA2000008) = 0x00202608;
  
	// For PREv9E GPIO Isolation disable >> bits 16, 17, 24
	*((volatile uint32_t *) 0xA2000008) = 0x0120E608;


    delay(DELAY_1);
  
    //Enumerate & Initialize Registers
    enumerated = 0xDEADBEEE;
    exec_count = 0;
    exec_count_irq = 0;
    MBus_msg_flag = 0;
    //Enumeration
    enumerate(RAD_ADDR);
    delay(MBUS_DELAY*2);

	// Initialize FLSv1 through GPIO
	FLSMBusGPIO_initialization();
	FLSMBusGPIO_forceStop();
	FLSMBusGPIO_enumeration(FLS_ADDR);
	FLSMBusGPIO_enumeration(FLS_ADDR);
	temp_numBit = FLSMBusGPIO_rxMsg();
	temp_addr = FLSMBusGPIO_getRxAddr(); // Optional
	temp_data = FLSMBusGPIO_getRxData0(); // Optional
	write_mbus_message(0xEE, temp_addr); // Optional
	write_mbus_message(0xEE, temp_data); // Optional
	write_mbus_message(0xEE, temp_numBit); // Optional

	// Set Optimum Tuning Bits
	FLSMBusGPIO_setOptTune(FLS_ADDR);

	// Receive Sleep Msg
	temp_numBit = FLSMBusGPIO_rxMsg();
	temp_addr = FLSMBusGPIO_getRxAddr(); // Optional
	temp_data = FLSMBusGPIO_getRxData0(); // Optional
	write_mbus_message(0xEE, temp_addr); // Optional
	write_mbus_message(0xEE, temp_data); // Optional
	write_mbus_message(0xEE, temp_numBit); // Optional

	delay (1000);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 100;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 1;   // 0x1E (30): ~1 min with PRCv9
    radio_tx_count = 0;
    radio_tx_option = 0;
    
	md_start_motion = 0;
	md_capture_img = 0;
	md_count = 0;

	INT_TIME = 5;
	MD_INT_TIME = 15;

	// Put FLS back to sleep
	FLSMBusGPIO_sleep();

    // Go to sleep without timer
    operation_sleep_notimer();
}

//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
  
    //Clear All Pending Interrupts
    *((volatile uint32_t *) 0xE000E280) = 0xF;
    //Enable Interrupts
    *((volatile uint32_t *) 0xE000E100) = 0xF;
  
	// Config watchdog timer to about 30 sec: 3,000,000 with default PRCv9
	//config_timer( timer_id, go, roi, init_val, sat_val )
	config_timer( 0, 0, 0, 0, 3000000 );

    // Initialization sequence
    if (enumerated != 0xDEADBEEE){
        operation_init();
    }

    // Check if wakeup is due to GOC interrupt  
    // 0x68 is reserved for GOC-triggered wakeup (Named IRQ10VEC)
    // 8 MSB bits of the wakeup data are used for function ID
    uint32_t wakeup_data = *((volatile uint32_t *) IRQ10VEC);
    uint32_t wakeup_data_header = wakeup_data>>24;
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;
    uint32_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
    uint32_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;

    if(wakeup_data_header == 1){
        // Debug mode: Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
        delay(MBUS_DELAY);
        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
            // radio
            send_radio_data(0xFAFA0000+exec_count_irq);	
            // set timer
            set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
            // go to sleep and wake up with same condition
            operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
            // radio
            send_radio_data(0xFAFA0000+exec_count_irq);	
            // Go to sleep without timer
            operation_sleep_notimer();
        }
   
    }else if(wakeup_data_header == 2){
		// Start motion detection
        // wakeup_data[7:0] is the md integration period
        // wakeup_data[15:8] is the img integration period
        // wakeup_data[19:16] indicates whether or not to to take an image
		// 						1: md only, 2: img only, 3: md+img
        // wakeup_data[23:20] indicates whether or not to radio out the result

		MD_INT_TIME = wakeup_data_field_0;
		INT_TIME = wakeup_data_field_1;

        if (exec_count_irq < 1){
            exec_count_irq++;
            // radio
            send_radio_data(0xFAFA0000+(wakeup_data_field_2 & 0xF));	
            // set timer
            set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
            // go to sleep and wake up with same condition
            operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
			// Reset IRQ10VEC
			*((volatile uint32_t *) IRQ10VEC) = 0;

			// Run Program
			exec_count = 0;
			md_count = 0;
			md_start_motion = (wakeup_data_field_2 & 0x1);
			md_capture_img = (wakeup_data_field_2 & 0x2)>>1;
			radio_tx_option = (wakeup_data_field_2 >> 4) & 0x1;
			operation_md();
        }
   
    }else if(wakeup_data_header == 3){
		// Stop MD program and transmit the execution count n times
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
		
		clear_md_flag();
		initialize_md_reg();
        set_pmu_sleep_clk_default();
		md_start_motion = 0;
		md_capture_img = 0;

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
            // radio
            send_radio_data(0xFAFA0000+md_count);	
            // set timer
            set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
            // go to sleep and wake up with same condition
            operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
            // Go to sleep without timer
            operation_sleep_notimer();
        }



    }else if(wakeup_data_header == 4){

    }else if(wakeup_data_header == 0x11){
		// Slow down PMU sleep osc and go to sleep for further programming
        set_pmu_sleep_clk_low();
        // Go to sleep without timer
        operation_sleep_notimer();

    }else if(wakeup_data_header == 0x12){
		// Restore PMU sleep osc and go to sleep for further programming
        set_pmu_sleep_clk_default();
        // Go to sleep without timer
        operation_sleep_notimer();

    }else if(wakeup_data_header == 0x13){
        set_pmu_sleep_clk_fastest();
        // Go to sleep without timer
        operation_sleep_notimer();
    }

    // Proceed to continuous mode
    while(1){
        operation_modena(); 
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

