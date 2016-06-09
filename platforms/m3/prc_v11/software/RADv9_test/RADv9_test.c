//****************************************************************************************************
//Author:       Gyouho Kim
//              ZhiYoong Foo
//Description:  
//				RADv9 testing derived from Pstack_ondemand_v1.14
//****************************************************************************************************
#include "mbus.h"
#include "PRCv9.h"
#include "SNSv7.h"
#include "HRVv2.h"
#include "RADv9.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// uncomment this to only transmit average
//#define TX_AVERAGE

// Stack order  PRC->RAD->SNS->HRV
#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define HRV_ADDR 0x6

// CDC parameters
#define	MBUS_DELAY 30 //Amount of delay between successive messages; 100: 6-7ms
#define	LDO_DELAY 500 // 1000: 150msec
#define CDC_TIMEOUT_COUNT 2000
#define WAKEUP_PERIOD_RESET 2
#define WAKEUP_PERIOD_LDO 2
#define CDC_CYCLE_INIT 2

// Pstack states
#define	PSTK_IDLE       0x0
#define PSTK_CDC_RST    0x1
#define PSTK_CDC_READ   0x3
#define PSTK_LDO1   	0x4
#define PSTK_LDO2  	 	0x5

// Radio configurations
#define RADIO_DATA_LENGTH 24
#define RADIO_PACKET_DELAY 3000
#define RADIO_TIMEOUT_COUNT 500
#define WAKEUP_PERIOD_RADIO_INIT 3

// CDC configurations
#define NUM_SAMPLES         3      //Number of CDC samples to take (only 2^n allowed for averaging: 2, 4, 8, 16...)
#define NUM_SAMPLES_TX      1      //Number of CDC samples to be TXed (processed by process_data)
#define NUM_SAMPLES_2PWR    0      //NUM_SAMPLES = 2^NUM_SAMPLES_2PWR - used for averaging

#define CDC_STORAGE_SIZE 48 // FIXME

//***************************************************
// Global variables
//***************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint8_t Pstack_state;
volatile uint32_t cdc_data[NUM_SAMPLES];
volatile uint32_t cdc_data_tx[NUM_SAMPLES_TX];
volatile uint32_t cdc_reset_timeout_count;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint8_t MBus_msg_flag;

volatile snsv7_r0_t snsv7_r0 = SNSv7_R0_DEFAULT;
volatile snsv7_r1_t snsv7_r1 = SNSv7_R1_DEFAULT;
volatile snsv7_r2_t snsv7_r2 = SNSv7_R2_DEFAULT;
volatile snsv7_r18_t snsv7_r18 = SNSv7_R18_DEFAULT;
  
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile uint32_t cdc_storage[CDC_STORAGE_SIZE] = {0};
volatile uint32_t cdc_storage_cref[CDC_STORAGE_SIZE] = {0};
volatile uint32_t cdc_storage_cref_latest;
volatile uint32_t cdc_storage_count;
volatile uint8_t cdc_run_single;
volatile uint8_t cdc_running;
volatile uint8_t set_cdc_exec_count;

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile bool radio_ready;
volatile bool radio_on;

volatile radv9_r0_t radv9_r0 = RADv9_R0_DEFAULT;
volatile radv9_r1_t radv9_r1 = RADv9_R1_DEFAULT;
volatile radv9_r2_t radv9_r2 = RADv9_R2_DEFAULT;
volatile radv9_r3_t radv9_r3 = RADv9_R3_DEFAULT;
volatile radv9_r4_t radv9_r4 = RADv9_R4_DEFAULT;
volatile radv9_r5_t radv9_r5 = RADv9_R5_DEFAULT;
volatile radv9_r11_t radv9_r11 = RADv9_R11_DEFAULT;
volatile radv9_r12_t radv9_r12 = RADv9_R12_DEFAULT;
volatile radv9_r13_t radv9_r13 = RADv9_R13_DEFAULT;
volatile radv9_r14_t radv9_r14 = RADv9_R14_DEFAULT;

volatile hrvv2_r0_t hrvv2_r0 = HRVv2_R0_DEFAULT;

//***************************************************
//Interrupt Handlers
//Must clear pending bit!
//***************************************************
void handler_ext_int_0(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void) __attribute__ ((interrupt ("IRQ")));
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

inline static void set_pmu_sleep_clk_low(){
    // PRCv11 Default: 0x8F770049
    //*((volatile uint32_t *) 0xA200000C) = 0x8F77183B; // 0x8F77003B: Sleep current 1.5nA, use GOC x0.6-2
    *((volatile uint32_t *) 0xA200000C) = 0x8F77194B; // 0x8F77194B: Sleep current 3nA, use GOC x9-15
    //*((volatile uint32_t *) 0xA200000C) = 0x8F7718CB; // 0x8F7718CB: Sleep current 5nA, use GOC x9-15
}
inline static void set_pmu_sleep_clk_default(){
    // PRCv11 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F77184B; // 0x8F7719CB: Sleep current 4nA w/ Poly2, use GOC x9-15
    //*((volatile uint32_t *) 0xA200000C) = 0x8F77184B; // 0x8F77184B: Sleep current 8nA, use GOC x9-15
    //*((volatile uint32_t *) 0xA200000C) = 0x8F7718CB; // 0x8F7718CB: Sleep current 5nA, use GOC x9-15
}

inline static void set_pmu_sleep_clk_high(){
    // PRCv11 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F77186B; // 0x8F77184B: Sleep current 8nA, use GOC x9-15
}


//***************************************************
// Radio transmission routines for PPM Radio (RADv7 & v8)
//***************************************************

static void radio_power_on(){
	// Need to speed up sleep pmu clock
	set_pmu_sleep_clk_high();

    // Release FSM Sleep - Requires >2s stabilization time
    radio_on = 1;
    radv9_r13.RAD_FSM_SLEEP = 0;
    write_mbus_register(RAD_ADDR,13,radv9_r13.as_int);
    delay(MBUS_DELAY);
    // Release SCRO Reset
    radv9_r2.SCRO_RESET = 0;
    write_mbus_register(RAD_ADDR,2,radv9_r2.as_int);
    delay(MBUS_DELAY);
    
    // Additional delay required after SCRO Reset release
    delay(400); // At least 20ms required
    
    // Enable SCRO
    radv9_r2.SCRO_ENABLE = 1;
    write_mbus_register(RAD_ADDR,2,radv9_r2.as_int);
    delay(MBUS_DELAY);
}

static void radio_power_off(){
	// Need to restore sleep pmu clock
	set_pmu_sleep_clk_default();

    // Turn off everything
    radio_on = 0;
    radv9_r2.SCRO_ENABLE = 0;
    radv9_r2.SCRO_RESET  = 1;
    write_mbus_register(RAD_ADDR,2,radv9_r2.as_int);
    delay(MBUS_DELAY);
    radv9_r13.RAD_FSM_SLEEP 		= 1;
    radv9_r13.RAD_FSM_ISOLATE 	= 1;
    radv9_r13.RAD_FSM_RESETn 	= 0;
    radv9_r13.RAD_FSM_ENABLE 	= 0;
    write_mbus_register(RAD_ADDR,13,radv9_r13.as_int);
    delay(MBUS_DELAY);
}

static void send_radio_data_ppm(bool last_packet, uint32_t radio_data){
    // Write Data: Only up to 24bit data for now
    radv9_r3.RAD_FSM_DATA = radio_data;
    write_mbus_register(RAD_ADDR,3,radv9_r3.as_int);
    delay(MBUS_DELAY);

    if (!radio_ready){
		radio_ready = 1;

		// Release FSM Isolate
		radv9_r13.RAD_FSM_ISOLATE = 0;
		write_mbus_register(RAD_ADDR,13,radv9_r13.as_int);
		delay(MBUS_DELAY);

		// Release FSM Reset
		radv9_r13.RAD_FSM_RESETn = 1;
		write_mbus_register(RAD_ADDR,13,radv9_r13.as_int);
		delay(MBUS_DELAY);
    }

	#ifdef DEBUG_MBUS_MSG
		write_mbus_message(0xBB, 0x0);
	#endif
    // Fire off data
    uint32_t count;
    MBus_msg_flag = 0;
    radv9_r13.RAD_FSM_ENABLE = 1;
    write_mbus_register(RAD_ADDR,13,radv9_r13.as_int);
    delay(MBUS_DELAY);

    for( count=0; count<RADIO_TIMEOUT_COUNT; count++ ){
		if( MBus_msg_flag ){
			MBus_msg_flag = 0;
			if (last_packet){
				radio_ready = 0;
				radio_power_off();
			}else{
				radv9_r13.RAD_FSM_ENABLE = 0;
				write_mbus_register(RAD_ADDR,13,radv9_r13.as_int);
				delay(MBUS_DELAY);
			}
			return;
		}else{
			delay(MBUS_DELAY);
		}
    }
	
    // Timeout
	write_mbus_message(0xBB, 0xFAFAFAFA);
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
    
    // Make sure Radio is off
    if (radio_on){
		radio_power_off();
    }

    // Disable Timer
    set_wakeup_timer (0, 0x0, 0x0);
    // Go to sleep without timer
    operation_sleep();

}

static void operation_tx_stored(void){

    //Fire off stored data to radio
#ifdef DEBUG_MBUS_MSG
    delay(MBUS_DELAY*10);
    write_mbus_message(0x70, radio_tx_count);
    delay(MBUS_DELAY*10);
    write_mbus_message(0x74, cdc_storage[radio_tx_count]);
    delay(MBUS_DELAY*10);
    write_mbus_message(0x76, cdc_storage_cref[radio_tx_count]);
    delay(MBUS_DELAY*10);
    write_mbus_message(0x70, radio_tx_count);
    delay(MBUS_DELAY*10);
#endif
    send_radio_data_ppm(0, cdc_storage[radio_tx_count]);
    delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
    send_radio_data_ppm(0, cdc_storage_cref[radio_tx_count]);

    if (((!radio_tx_numdata)&&(radio_tx_count > 0)) | ((radio_tx_numdata)&&((radio_tx_numdata+radio_tx_count) > cdc_storage_count))){
		radio_tx_count--;
		// set timer
		set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
		// go to sleep and wake up with same condition
		operation_sleep_noirqreset();

    }else{
		delay(RADIO_PACKET_DELAY*3); //Set delays between sending subsequent packet
		send_radio_data_ppm(1, 0xFAF000);

		// This is also the end of this IRQ routine
		exec_count_irq = 0;
		// Go to sleep without timer
		radio_tx_count = cdc_storage_count; // allows data to be sent more than once
		operation_sleep_notimer();
    }

}

static void operation_init(void){
  
    // Set PMU Strength & division threshold
    // Change PMU_CTRL Register
    // PRCv11 Default: 0x8F770049
    // Decrease 5x division switching threshold
    //*((volatile uint32_t *) 0xA200000C) = 0x8F77004B;
    // Increase active pmu clock (for PRCv11)
    //*((volatile uint32_t *) 0xA200000C) = 0x8F77184B;
	set_pmu_sleep_clk_default();
  
    // Speed up GOC frontend to match PMU frequency
    // PRCv11 Default: 0x00202903
    //*((volatile uint32_t *) 0xA2000008) = 0x00202908;
    // Slow down MBUS frequency 
    // Gyouho: This is required for running on the board w/o PMU assist (for PRCv11)
    *((volatile uint32_t *) 0xA2000008) = 0x00202D03;
  
    delay(MBUS_DELAY*20);
  
    //Enumerate & Initialize Registers
    Pstack_state = PSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    exec_count_irq = 0;
    MBus_msg_flag = 0;
  
    //Enumeration
    enumerate(RAD_ADDR);
    delay(MBUS_DELAY*10);
  
    //write_mbus_message(0x70, radio_tx_count);
    //operation_sleep_notimer();
  
    enumerate(SNS_ADDR);
    delay(MBUS_DELAY*10);
    enumerate(HRV_ADDR);
    delay(MBUS_DELAY*10);
  
    // Radio Settings --------------------------------------
  
    radv9_r0.RADIO_TUNE_CURRENT_LIMITER = 0x2F; //Current Limiter 2F = 30uA, 1F = 3uA
    radv9_r0.RADIO_TUNE_FREQ1 = 0x0; //Tune Freq 1
    radv9_r0.RADIO_TUNE_FREQ2 = 0x0; //Tune Freq 2 //0x0,0x0 = 902MHz on Pblr005
    radv9_r0.RADIO_TUNE_TX_TIME = 0x6; //Tune TX Time
  
    write_mbus_register(RAD_ADDR,0,radv9_r0.as_int);
    delay(MBUS_DELAY);

    // FSM data length setups
    radv9_r11.RAD_FSM_H_LEN = 16; // N
    radv9_r11.RAD_FSM_D_LEN = RADIO_DATA_LENGTH-1; // N-1
    radv9_r11.RAD_FSM_C_LEN = 10;
    write_mbus_register(RAD_ADDR,11,radv9_r11.as_int);
    delay(MBUS_DELAY);
  
    // Configure SCRO
    radv9_r1.SCRO_FREQ_DIV = 3;
    radv9_r1.SCRO_AMP_I_LEVEL_SEL = 2; // Default 2
    radv9_r1.SCRO_I_LEVEL_SELB = 0x60; // Default 0x6F
    write_mbus_register(RAD_ADDR,1,radv9_r1.as_int);
    delay(MBUS_DELAY);
  
    // LFSR Seed
    radv9_r12.RAD_FSM_SEED = 4;
    write_mbus_register(RAD_ADDR,12,radv9_r12.as_int);
    delay(MBUS_DELAY);
  
    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 5;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 1;   // 0x1E (30): ~1 min with PRCv9
    cdc_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0;
    cdc_run_single = 0;
    cdc_running = 0;
    radio_ready = 0;
    radio_on = 0;
	set_cdc_exec_count = 0;

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
  
    //Config watchdog timer to about 10 sec: 1,000,000 with default PRCv9
    //config_timer( timer_id, go, roi, init_val, sat_val )
    config_timer( 0, 1, 0, 0, 1000000 );

    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
        // Set up PMU/GOC register in PRC layer (every time)
        // Enumeration & RAD/SNS layer register configuration
        operation_init();
    }

        // Debug mode: Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the MSB of # of transmissions
        delay(MBUS_DELAY);
		exec_count_irq++;
		if (exec_count_irq == 1){
			// Prepare radio TX
			radio_power_on();
			// Go to sleep for SCRO stabilitzation
			set_wakeup_timer(2, 0x1, 0x0);
			operation_sleep_noirqreset();
		}else{
			// radio
			send_radio_data_ppm(0,0xFAF000+exec_count_irq);	
			// set timer
			set_wakeup_timer (1, 0x1, 0x0);
			// go to sleep and wake up with same condition
			operation_sleep_noirqreset();
		}
   

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

