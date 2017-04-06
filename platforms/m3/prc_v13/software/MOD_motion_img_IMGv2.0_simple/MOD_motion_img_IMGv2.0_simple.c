//****************************************************************************************************
//Author:       Gyouho Kim
//Description: 	MODv2.0 System Code
//				Imaging and motion detection with MDv3
//				FLASH storage with FLSv2
//*******************************************************************
#include "PRCv13.h"
#include "PRCv13_RF.h"
#include "mbus.h"
#include "PMUv2_RF.h"
#include "MDv3.h"
#include "RADv9.h"

// uncomment this for debug mbus message
#define DEBUG_MBUS_MSG

// Stack order: PRC->HRV->MD->RAD->FLS->PMU
#define PRC_ADDR 0x1
#define MD_ADDR 0x4
#define RAD_ADDR 0x5
#define HRV_ADDR 0x6
#define FLS_ADDR 0x9
#define PMU_ADDR 0x8

// Common parameters
#define	MBUS_DELAY 200 //Amount of delay between successive messages; 5-6ms
#define WAKEUP_DELAY 20000 // 0.6s
#define DELAY_1 40000 // 5000: 0.5s
#define IMG_TIMEOUT_COUNT 5000

// MDv3 Parameters
#define START_COL_IDX 0 // in words
#define COLS_TO_READ 39 // in # of words: 39 for full frame, 19 for half

// Radio configurations
#define RADIO_DATA_LENGTH 96
#define RADIO_TIMEOUT_COUNT 500
#define WAKEUP_PERIOD_RADIO_INIT 2
#define RADIO_PACKET_DELAY 4000 // Need 100-200ms

// FLSv1 configurations
//#define FLS_RECORD_LENGTH 0x18FE // In words; # of words stored -2
#define FLS_RECORD_LENGTH 6473 // In words; # of words stored -2; Full image with mbus overhead
#define FLS_CHECK_LENGTH 200 // In words

//***************************************************
// Global variables
//***************************************************
//Test Declerations
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO
volatile uint32_t enumerated;
volatile uint32_t exec_count;
volatile uint32_t exec_count_irq;
volatile uint32_t mbus_msg_flag;

volatile bool radio_ready;
volatile bool radio_on;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_img_idx;
volatile uint32_t radio_tx_img_all;
volatile uint32_t radio_tx_img_one;
volatile uint32_t radio_tx_img_num;

volatile uint32_t md_count;
volatile uint32_t img_count;
volatile uint32_t md_capture_img;
volatile uint32_t md_start_motion;
volatile uint32_t USR_MD_INT_TIME;
volatile uint32_t USR_INT_TIME;

volatile mdv3_r0_t mdv3_r0 = MDv3_R0_DEFAULT;
volatile mdv3_r1_t mdv3_r1 = MDv3_R1_DEFAULT;
volatile mdv3_r2_t mdv3_r2 = MDv3_R2_DEFAULT;
volatile mdv3_r3_t mdv3_r3 = MDv3_R3_DEFAULT;
volatile mdv3_r4_t mdv3_r4 = MDv3_R4_DEFAULT;
volatile mdv3_r5_t mdv3_r5 = MDv3_R5_DEFAULT;
volatile mdv3_r6_t mdv3_r6 = MDv3_R6_DEFAULT;
volatile mdv3_r7_t mdv3_r7 = MDv3_R7_DEFAULT;
volatile mdv3_r8_t mdv3_r8 = MDv3_R8_DEFAULT;
volatile mdv3_r9_t mdv3_r9 = MDv3_R9_DEFAULT;

volatile radv9_r0_t radv9_r0 = RADv9_R0_DEFAULT;
volatile radv9_r1_t radv9_r1 = RADv9_R1_DEFAULT;
volatile radv9_r2_t radv9_r2 = RADv9_R2_DEFAULT;
volatile radv9_r3_t radv9_r3 = RADv9_R3_DEFAULT;
volatile radv9_r11_t radv9_r11 = RADv9_R11_DEFAULT;
volatile radv9_r12_t radv9_r12 = RADv9_R12_DEFAULT;
volatile radv9_r13_t radv9_r13 = RADv9_R13_DEFAULT;

volatile prcv13_r0B_t prcv13_r0B = PRCv13_R0B_DEFAULT;

volatile uint32_t WAKEUP_PERIOD_CONT;
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile uint32_t flash_read_data[3] = {0, 0, 0}; // Read 3 words at a time; 96 bits of radio packet
volatile uint32_t flash_read_data_single = 0; 

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_0(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_4(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_5(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_6(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_7(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_8(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_9(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_12(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_13(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_14(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0(void)  { *NVIC_ICPR = (0x1 << 0);  } // TIMER32
void handler_ext_int_1(void)  { *NVIC_ICPR = (0x1 << 1);  } // TIMER16
void handler_ext_int_2(void)  { *NVIC_ICPR = (0x1 << 2); mbus_msg_flag = 0x10; } // REG0
void handler_ext_int_3(void)  { *NVIC_ICPR = (0x1 << 3); mbus_msg_flag = 0x11; } // REG1
void handler_ext_int_4(void)  { *NVIC_ICPR = (0x1 << 4); mbus_msg_flag = 0x12; } // REG2
void handler_ext_int_5(void)  { *NVIC_ICPR = (0x1 << 5); mbus_msg_flag = 0x13; } // REG3
void handler_ext_int_6(void)  { *NVIC_ICPR = (0x1 << 6); mbus_msg_flag = 0x14; } // REG4
void handler_ext_int_7(void)  { *NVIC_ICPR = (0x1 << 7); mbus_msg_flag = 0x15; } // REG5
void handler_ext_int_8(void)  { *NVIC_ICPR = (0x1 << 8); mbus_msg_flag = 0x16; } // REG6
void handler_ext_int_9(void)  { *NVIC_ICPR = (0x1 << 9); mbus_msg_flag = 0x17; } // REG7
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); } // MEM WR
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); } // MBUS_RX
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); } // MBUS_TX
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); } // MBUS_FWD
void handler_ext_int_14(void) { *NVIC_ICPR = (0x1 << 14); } // MBUS_FWD


//************************************
// PMU Related Functions
//************************************
inline static void set_pmu_sleep_clk_default(){
}
inline static void set_pmu_sleep_clk_fastest(){
}


//***************************************************
// Radio transmission routines for PPM Radio (RADv9)
//***************************************************

static void radio_power_on(){
	// Need to speed up sleep pmu clock
	set_pmu_sleep_clk_fastest();

    // Release FSM Sleep - Requires >2s stabilization time
    radio_on = 1;
    radv9_r13.RAD_FSM_SLEEP = 0;
    mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
    delay(MBUS_DELAY);
    // Release SCRO Reset
    radv9_r2.SCRO_RESET = 0;
    mbus_remote_register_write(RAD_ADDR,2,radv9_r2.as_int);
    delay(MBUS_DELAY);
    
    // Additional delay required after SCRO Reset release
    delay(MBUS_DELAY*3); // At least 20ms required
    
    // Enable SCRO
    radv9_r2.SCRO_ENABLE = 1;
    mbus_remote_register_write(RAD_ADDR,2,radv9_r2.as_int);
    delay(MBUS_DELAY);

	// Release FSM Isolate
	radv9_r13.RAD_FSM_ISOLATE = 0;
	mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
	delay(MBUS_DELAY);

}

static void radio_power_off(){
	// Need to restore sleep pmu clock
	set_pmu_sleep_clk_default();

    // Turn off everything
    radio_on = 0;
    radv9_r2.SCRO_ENABLE = 0;
    radv9_r2.SCRO_RESET  = 1;
    mbus_remote_register_write(RAD_ADDR,2,radv9_r2.as_int);
    radv9_r13.RAD_FSM_SLEEP 	= 1;
    radv9_r13.RAD_FSM_ISOLATE 	= 1;
    radv9_r13.RAD_FSM_RESETn 	= 0;
    radv9_r13.RAD_FSM_ENABLE 	= 0;
    mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
}

static void send_radio_data_ppm(bool last_packet, uint32_t radio_data){
    // Write Data: Only up to 24bit data for now
    radv9_r3.RAD_FSM_DATA = radio_data;
    mbus_remote_register_write(RAD_ADDR,3,radv9_r3.as_int);
    delay(MBUS_DELAY);

    if (!radio_ready){
		radio_ready = 1;

		// Release FSM Reset
		radv9_r13.RAD_FSM_RESETn = 1;
		mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
		delay(MBUS_DELAY);
    }

    // Set CPU Halt Option as RX --> Use for register read e.g.
    set_halt_until_mbus_rx();

    // Fire off data
    uint32_t count;
    mbus_msg_flag = 0;
    radv9_r13.RAD_FSM_ENABLE = 1;
    mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
    delay(MBUS_DELAY);

    for( count=0; count<RADIO_TIMEOUT_COUNT; count++ ){
		if( mbus_msg_flag ){
    		set_halt_until_mbus_tx();
			mbus_msg_flag = 0;
			if (last_packet){
				radio_ready = 0;
				radio_power_off();
			}else{
				radv9_r13.RAD_FSM_ENABLE = 0;
				mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
				delay(MBUS_DELAY);
			}
			return;
		}else{
			delay(MBUS_DELAY);
		}
    }
	
    set_halt_until_mbus_tx();
	mbus_write_message32(0xBB, 0xFAFAFAFA);
}

static void send_radio_data_ppm_96(bool last_packet, uint32_t radio_data_0, uint32_t radio_data_1, uint32_t radio_data_2, uint32_t radio_data_3){
	// Sends 96 bits of data (3 words, 4 RADv9 registers)
	// radio_data_0: DATA[23:0]
	// radio_data_1: DATA[47:24]
	// radio_data_2: DATA[71:48]
	// radio_data_3: DATA[95:72]
    mbus_remote_register_write(RAD_ADDR,3,radio_data_0);
    delay(MBUS_DELAY);
    mbus_remote_register_write(RAD_ADDR,4,radio_data_1);
    delay(MBUS_DELAY);
    mbus_remote_register_write(RAD_ADDR,5,radio_data_2);
    delay(MBUS_DELAY);
    mbus_remote_register_write(RAD_ADDR,6,radio_data_3);
    delay(MBUS_DELAY);

    if (!radio_ready){
		radio_ready = 1;

		// Release FSM Reset
		radv9_r13.RAD_FSM_RESETn = 1;
		mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
		delay(MBUS_DELAY);
    }

    // Set CPU Halt Option as RX --> Use for register read e.g.
    set_halt_until_mbus_rx();

    // Fire off data
    uint32_t count;
    mbus_msg_flag = 0;
    radv9_r13.RAD_FSM_ENABLE = 1;
    mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
    delay(MBUS_DELAY);

    for( count=0; count<RADIO_TIMEOUT_COUNT; count++ ){
		if( mbus_msg_flag ){
    		set_halt_until_mbus_tx();
			mbus_msg_flag = 0;
			if (last_packet){
				radio_ready = 0;
				radio_power_off();
			}else{
				radv9_r13.RAD_FSM_ENABLE = 0;
				mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
				delay(MBUS_DELAY);
			}
			return;
		}else{
			delay(MBUS_DELAY);
		}
    }
	
	// Timeout
    set_halt_until_mbus_tx();
	mbus_write_message32(0xBB, 0xFAFAFAFA);
}
//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){

	// Reset IRQ14VEC
	*((volatile uint32_t *) IRQ14VEC) = 0;

	// Go to Sleep
    mbus_sleep_all();
	while(1);

}

static void operation_sleep_noirqreset(void){

	// Go to Sleep
	mbus_sleep_all();
	while(1);

}

static void operation_sleep_notimer(void){
    
	// Turn off radio
	radio_power_off();

	// Disable Timer
	set_wakeup_timer(0, 0, 0);

	// Go to sleep without timer
	operation_sleep();

}

//************************************
// FLSv2 Functions
//************************************

uint32_t check_flash_sram(uint8_t addr_stamp, uint32_t length){
	uint32_t idx;

	for(idx=0; idx<length; idx++) {

    	set_halt_until_mbus_rx();
		mbus_copy_mem_from_remote_to_any_bulk(FLS_ADDR, (uint32_t*)(idx << 2), PRC_ADDR, (uint32_t*)&flash_read_data_single, 0);
		//FLSv2MBusGPIO_readMem(FLS_ADDR, 0xEE, 0, ((uint32_t) idx) << 2);
		delay(MBUS_DELAY);
    	set_halt_until_mbus_tx();
		mbus_write_message32(addr_stamp, flash_read_data_single);
		delay(MBUS_DELAY);
		
	}

	return 1;
}

uint32_t send_radio_flash_sram(uint8_t addr_stamp, uint32_t length){
	uint32_t idx;
	uint32_t fls_rx_data;

	for(idx=0; (idx*3)<length; idx++) {

    	set_halt_until_mbus_rx();
		mbus_copy_mem_from_remote_to_any_bulk(FLS_ADDR, (uint32_t*)((idx*3) << 2), PRC_ADDR, (uint32_t*)&flash_read_data, 2);
		//FLSv2MBusGPIO_readMem(FLS_ADDR, 0xEE, 0, ((uint32_t) idx) << 2);
		// Send 96 bits of data
		send_radio_data_ppm_96(0,flash_read_data[0],flash_read_data[1],flash_read_data[2],flash_read_data[2]);
		delay(MBUS_DELAY);
	}

	return 1;
}

static void flash_turn_on(){

    set_halt_until_mbus_rx();
	mbus_remote_register_write(FLS_ADDR,0x11,0x00003F);
    set_halt_until_mbus_tx();
	//check_flash_payload(0xA4, 0x000000B5);

}

static void flash_turn_off(){

    set_halt_until_mbus_rx();
	mbus_remote_register_write(FLS_ADDR,0x11,0x00003D);
    set_halt_until_mbus_tx();
	//check_flash_payload(0xA6, 0x000000BB);

}

static void flash_erase_single_page(volatile uint32_t page_num){
	// page_num should be a multiple of 0x800
    mbus_remote_register_write(FLS_ADDR,0x09,page_num); // Set flash start addr
    set_halt_until_mbus_rx();
	mbus_remote_register_write(FLS_ADDR,0x07,((0x0 << 6) | (0x1 << 5) | (0x4 << 1) | (0x1 << 0))); // Do erase
    set_halt_until_mbus_tx();
	//check_flash_payload(0xA1, 0x00000055);

}

static void flash_copy_flash2sram(volatile uint32_t length){

    mbus_remote_register_write(FLS_ADDR,0x08,0x0); // Set SRAM start addr
    set_halt_until_mbus_rx();
	mbus_remote_register_write(FLS_ADDR,0x07,((length << 6) | (0x1 << 5) | (0x1 << 1) | (0x1 << 0)));
    set_halt_until_mbus_tx();
	//check_flash_payload(0xA8, 0x00000024);

}

static void flash_copy_sram2flash(volatile uint32_t length){

    mbus_remote_register_write(FLS_ADDR,0x08,0x0); // Set SRAM start addr
    set_halt_until_mbus_rx();
	mbus_remote_register_write(FLS_ADDR,0x07,((length << 6) | (0x1 << 5) | (0x2 << 1) | (0x1 << 0)));
    set_halt_until_mbus_tx();
	//check_flash_payload(0xA8, 0x0000003D);

}

static void set_pmu_motion(void){

	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
	// V0P6 Supply Active: need to support up to 50uA
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (1 << 13) // Enable main feedback loop
		| (4 << 9)  // Frequency multiplier R
		| (4 << 5)  // Frequency multiplier L (actually L+1)
		| (10) 		// Floor frequency base (0-31)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (1 << 13) // Enable main feedback loop
		| (4 << 9)  // Frequency multiplier R
		| (4 << 5)  // Frequency multiplier L (actually L+1)
		| (10) 		// Floor frequency base (0-31)
	));
	delay(MBUS_DELAY);
	// Register 0x16: SAR_TRIM_v3_ACTIVE
	// V1P2 Supply Active: need to support up to 200uA
    mbus_remote_register_write(PMU_ADDR,0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (1 << 13) // Enable main feedback loop
		| (5 << 9)  // Frequency multiplier R
		| (5 << 5)  // Frequency multiplier L (actually L+1)
		| (10) 		// Floor frequency base (0-31) //16
	));
	delay(MBUS_DELAY);

	// SAR_RATIO_OVERRIDE
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (45) 		// Binary converter's conversion ratio (7'h00)
		// 48: 1.5V
		// 38: 1.1V
		// 41: 1.26V
	));
	delay(MBUS_DELAY);
}

static void set_pmu_img(void){

	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
	// V0P6 Supply Active: need to support up to 50uA
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (1 << 13) // Enable main feedback loop
		| (4 << 9)  // Frequency multiplier R
		| (4 << 5)  // Frequency multiplier L (actually L+1)
		| (16) 		// Floor frequency base (0-31)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (1 << 13) // Enable main feedback loop
		| (4 << 9)  // Frequency multiplier R
		| (4 << 5)  // Frequency multiplier L (actually L+1)
		| (16) 		// Floor frequency base (0-31)
	));
	delay(MBUS_DELAY);
	// Register 0x16: SAR_TRIM_v3_ACTIVE
	// V1P2 Supply Active: need to support up to 200uA
    mbus_remote_register_write(PMU_ADDR,0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (1 << 13) // Enable main feedback loop
		| (5 << 9)  // Frequency multiplier R
		| (5 << 5)  // Frequency multiplier L (actually L+1)
		| (22) 		// Floor frequency base (0-31) //16
	));
	delay(MBUS_DELAY);

	// SAR_RATIO_OVERRIDE
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (46) 		// Binary converter's conversion ratio (7'h00)
		// 48: 1.5V
		// 38: 1.1V
		// 41: 1.26V
	));
	delay(MBUS_DELAY);
	
	// Let it stabilize
	delay(DELAY_1);
}

static void set_pmu_motion_img(void){

	// Set PMU settings
	// PMUv2 MBUS register write results in IRQ, so needs to be in RX mode
	// UPCONV_TRIM_V3 Sleep/Active
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (4) 		// Floor frequency base (0-31)
	));
	delay(MBUS_DELAY);
	// The first register write to PMU needs to be repeated
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (4) 		// Floor frequency base (0-31)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (2) 		// Floor frequency base (0-31)
	));
	delay(MBUS_DELAY);
	// Register 0x19: DOWNCONV_TRIM_V3_SLEEP
	// V0P6 Supply Sleep: need to support up to 200nA
    mbus_remote_register_write(PMU_ADDR,0x19,
		( (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (1 << 5)  // Frequency multiplier L (actually L+1)
		| (8) 		// Floor frequency base (0-31)
	));
	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
	// V0P6 Supply Active: need to support up to 50uA
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (1 << 13) // Enable main feedback loop
		| (4 << 9)  // Frequency multiplier R
		| (4 << 5)  // Frequency multiplier L (actually L+1)
		| (16) 		// Floor frequency base (0-31)
	));
	delay(MBUS_DELAY);
	// Register 0x15: SAR_TRIM_v3_SLEEP
	// V1P2 Supply Sleep: need to support up to 100nA
    mbus_remote_register_write(PMU_ADDR,0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (1 << 5)  // Frequency multiplier L (actually L+1)
		| (4) 		// Floor frequency base (0-31) //8
	));
	delay(MBUS_DELAY);
	// Register 0x16: SAR_TRIM_v3_ACTIVE
	// V1P2 Supply Active: need to support up to 200uA
    mbus_remote_register_write(PMU_ADDR,0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (1 << 13) // Enable main feedback loop
		| (5 << 9)  // Frequency multiplier R
		| (5 << 5)  // Frequency multiplier L (actually L+1)
		| (22) 		// Floor frequency base (0-31) //16
	));
	delay(MBUS_DELAY);
	// SAR_RATIO_OVERRIDE
	// Need to set [10] to 1 and then 0
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (1 << 11) // Enable override setting [10] (1'h0)
		| (1 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (46) 		// Binary converter's conversion ratio (7'h00)
		// 48: 1.5V
		// 38: 1.1V
		// 41: 1.26V
	));
	delay(MBUS_DELAY);
	// SAR_RATIO_OVERRIDE
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (46) 		// Binary converter's conversion ratio (7'h00)
		// 48: 1.5V
		// 38: 1.1V
		// 41: 1.26V
	));
	delay(MBUS_DELAY);
	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    mbus_remote_register_write(PMU_ADDR,0x36,0x000001);
	delay(MBUS_DELAY);
	// Register 0x37: TICK_WAKEUP_WAIT
    mbus_remote_register_write(PMU_ADDR,0x37,0x009F4); // Default: 0x1F4
	delay(MBUS_DELAY);

}

//************************************
// MDv3 Functions
//************************************

static void initialize_md_reg(){

	mdv3_r9 = MDv3_R9_DEFAULT;

	mdv3_r0.INT_TIME = USR_INT_TIME;
	mdv3_r0.MD_INT_TIME = USR_MD_INT_TIME;
	mdv3_r1.MD_TH = 4;
	mdv3_r1.MD_LOWRES = 0;
	mdv3_r1.MD_LOWRES_B = 1;
	mdv3_r1.MD_FLAG_TOPAD_SEL = 0; // 1: thresholding, 0: no thresholding

	mdv3_r2.MD_RESULTS_MASK = 0x3FF;

	mdv3_r3.VDD_CC_ENB_0P6 = 0;
	mdv3_r3.VDD_CC_ENB_1P2 = 1;
	mdv3_r3.SEL_VREF = 0;
	mdv3_r3.SEL_VREFP = 7;
	mdv3_r3.SEL_VBN = 3;
	mdv3_r3.SEL_VBP = 3;
	mdv3_r3.SEL_VB_RAMP = 15;
	mdv3_r3.SEL_RAMP = 1;

	mdv3_r4.SEL_CC  = 4;
	mdv3_r4.SEL_CC_B  = 3;

	mdv3_r5.SEL_CLK_RING = 2;
	mdv3_r5.SEL_CLK_DIV = 4;
	mdv3_r5.SEL_CLK_RING_4US = 0;
	mdv3_r5.SEL_CLK_DIV_4US = 1;
	mdv3_r5.SEL_CLK_RING_ADC = 2; 
	mdv3_r5.SEL_CLK_DIV_ADC = 1;
	mdv3_r5.SEL_CLK_RING_LC = 0;
	mdv3_r5.SEL_CLK_DIV_LC = 1;

	mdv3_r6.START_ROW_IDX = 40;
	mdv3_r6.END_ROW_IDX = 120; // Default: 160
	mdv3_r6.ROW_SKIP = 0;
	mdv3_r6.COL_SKIP = 0;
	mdv3_r6.ROW_IDX_EN = 0;

	mdv3_r8.MBUS_REPLY_ADDR_FLAG = 0x18;
	mdv3_r9.MBUS_REPLY_ADDR_DATA = 0x74; // IMG Data return address

	mdv3_r8.MBUS_START_ADDR = 0; // Start column index in words
	mdv3_r8.MBUS_LENGTH_M1 = 39; // Columns to be read; in # of words: 39 for full frame, 19 for half

	//Write Registers
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);
	mbus_remote_register_write(MD_ADDR,0x1,mdv3_r1.as_int);
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);
	mbus_remote_register_write(MD_ADDR,0x3,mdv3_r3.as_int);
	mbus_remote_register_write(MD_ADDR,0x4,mdv3_r4.as_int);
	mbus_remote_register_write(MD_ADDR,0x5,mdv3_r5.as_int);
	mbus_remote_register_write(MD_ADDR,0x6,mdv3_r6.as_int);
	mbus_remote_register_write(MD_ADDR,0x7,mdv3_r7.as_int);
	mbus_remote_register_write(MD_ADDR,0x8,mdv3_r8.as_int);
	mbus_remote_register_write(MD_ADDR,0x9,mdv3_r9.as_int);

}

static void start_md(){

	// Optionally release MD GPIO Isolation
	// 7:16
	//mdv3_r7.ISOLATE_GPIO = 0;
	//mbus_remote_register_write(MD_ADDR,0x7,mdv3_r7.as_int);
	//delay(MBUS_DELAY);
	//delay(DELAY_500ms); // about 0.5s

	// Start MD
	// 0:1
	mdv3_r0.START_MD = 1;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);
	delay(MBUS_DELAY*4);

	mdv3_r0.START_MD = 0;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);

	delay(DELAY_1); // about 0.5s
	delay(DELAY_1); // about 0.5s

	// Enable MD Flag
	// 1:3
	mdv3_r1.MD_TH_EN = 1;
	mbus_remote_register_write(MD_ADDR,0x1,mdv3_r1.as_int);
	delay(MBUS_DELAY);

}

static void clear_md_flag(){

	// Stop MD
	// 0:2
	mdv3_r0.STOP_MD = 1;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);
	delay(MBUS_DELAY*4);

	mdv3_r0.STOP_MD = 0;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);

	// Clear MD Flag
	// 1:4
	mdv3_r1.MD_TH_CLEAR = 1;
	mbus_remote_register_write(MD_ADDR,0x1,mdv3_r1.as_int);
	delay(MBUS_DELAY*4);

	mdv3_r1.MD_TH_CLEAR = 0;
	mbus_remote_register_write(MD_ADDR,0x1,mdv3_r1.as_int);

	// Disable MD Flag
	// 1:3
	mdv3_r1.MD_TH_EN = 0;
	mbus_remote_register_write(MD_ADDR,0x1,mdv3_r1.as_int);

}

static void poweron_frame_controller(){

	// Release MD Presleep
	// 2:22
	mdv3_r2.PRESLEEP_MD = 0;
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);
	delay(WAKEUP_DELAY);

	// Release MD Sleep
	// 2:21
	mdv3_r2.SLEEP_MD = 0;
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);
	delay(WAKEUP_DELAY);

	// Release MD Isolation
	// 7:15
	mdv3_r7.ISOLATE_MD = 0;
	mbus_remote_register_write(MD_ADDR,0x7,mdv3_r7.as_int);
	delay(MBUS_DELAY);

	// Release MD Reset
	// 2:23
	mdv3_r2.RESET_MD = 0;
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);
	delay(MBUS_DELAY);

	// Start MD Clock
	// 5:12
	mdv3_r5.CLK_EN_MD = 1;
	mbus_remote_register_write(MD_ADDR,0x5,mdv3_r5.as_int);
	delay(MBUS_DELAY);

}

static void poweron_array_adc(){

	// Release IMG Presleep 
	// 2:20
	mdv3_r2.PRESLEEP_ADC = 0;
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);
	delay(WAKEUP_DELAY);

	// Release IMG Sleep
	// 2:19
	mdv3_r2.SLEEP_ADC = 0;
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);
	delay(WAKEUP_DELAY);

	// Release ADC Isolation
	// 7:17
	mdv3_r7.ISOLATE_ADC_WRAPPER = 0;
	mbus_remote_register_write(MD_ADDR,0x7,mdv3_r7.as_int);
	delay(MBUS_DELAY);

	// Release ADC Wrapper Reset
	// 6:0
	mdv3_r6.RESET_ADC_WRAPPER = 0;
	mbus_remote_register_write(MD_ADDR,0x6,mdv3_r6.as_int);
	delay(MBUS_DELAY);

	// Start ADC Clock
	// 5:13
	mdv3_r5.CLK_EN_ADC = 1;
	mbus_remote_register_write(MD_ADDR,0x5,mdv3_r5.as_int);
	delay(MBUS_DELAY);

}

static void poweroff_array_adc(){

	// Stop ADC Clock
	// 5:13
	mdv3_r5.CLK_EN_ADC = 0;
	mbus_remote_register_write(MD_ADDR,0x5,mdv3_r5.as_int);
	delay(MBUS_DELAY);

	// Assert ADC Wrapper Reset
	// 6:0
	mdv3_r6.RESET_ADC_WRAPPER = 1;
	mbus_remote_register_write(MD_ADDR,0x6,mdv3_r6.as_int);
	delay(MBUS_DELAY);

	// Assert ADC Isolation
	// 7:17
	mdv3_r7.ISOLATE_ADC_WRAPPER = 1;
	mbus_remote_register_write(MD_ADDR,0x7,mdv3_r7.as_int);
	delay(MBUS_DELAY);

	// Assert IMG Presleep 
	// 2:20
	// Assert IMG Sleep
	// 2:19
	mdv3_r2.PRESLEEP_ADC = 1;
	mdv3_r2.SLEEP_ADC = 1;
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);
	delay(MBUS_DELAY);

}


static void capture_image_single(){

	// Capture Image
	// 0:0
	mdv3_r0.TAKE_IMAGE = 1;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);
	delay(MBUS_DELAY*3); //Need >10ms

    set_halt_until_mbus_rx();

	mdv3_r0.TAKE_IMAGE = 0;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);

}

static void capture_image_start(){

	// Capture Image
	// 0:0
	mdv3_r0.TAKE_IMAGE = 1;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);
	delay(MBUS_DELAY);

}

static bool wait_for_interrupt(uint32_t wait_count){

    uint32_t count;
    for( count=0; count<wait_count; count++ ){
		if( mbus_msg_flag ){
    		set_halt_until_mbus_tx();
			mbus_msg_flag = 0;
			return 1;
		}else{
			delay(MBUS_DELAY);
		}
    }

	// Timeout
    set_halt_until_mbus_tx();
	mbus_write_message32(0xFF, 0xFAFAFAFA);
	return 0;
}

static void operation_md(void){

	set_pmu_img();

	// Release power gates, isolation, and reset for frame controller
	if (md_count == 0) {
		initialize_md_reg();
		poweron_frame_controller();
	}else{
		// This wakeup is due to motion detection
		// Let the world know!
		mbus_write_message32(0xAA, 0xABCD1234);
  		delay(MBUS_DELAY);
		clear_md_flag();
	}

	// Release power gates, isolation, and reset for imager array
	poweron_array_adc();

	// TAke 3 images
	capture_image_single();
	wait_for_interrupt(IMG_TIMEOUT_COUNT);	
	capture_image_single();
	wait_for_interrupt(IMG_TIMEOUT_COUNT);	
	
	//capture_image_single();
	//wait_for_interrupt(IMG_TIMEOUT_COUNT);	
	poweroff_array_adc();

	// Set PMU settings for motion detection
    set_pmu_motion();
	
	md_count++;

	// Start motion detection
	start_md();
	clear_md_flag();
	start_md();

	// Go to sleep w/o timer
	operation_sleep_notimer();
}


static void operation_init(void){
  
    // Set CPU Halt Option as TX --> Use for register write e.g.
    set_halt_until_mbus_tx();
	
	// Set CPU & Mbus Clock Speeds
    prcv13_r0B.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
    prcv13_r0B.CLK_GEN_RING = 0x3; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_MBC = 0x0; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_CORE = 0x2; // Default 0x3
	*((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv13_r0B.as_int;
    delay(MBUS_DELAY);

    //Enumerate & Initialize Registers
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    exec_count_irq = 0;
    mbus_msg_flag = 0;

    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    // Enumeration
	// Stack order: PRC->HRV->MD->RAD->FLS->PMU
    mbus_enumerate(HRV_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(MD_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(RAD_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(FLS_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(PMU_ADDR);
    delay(MBUS_DELAY);

	// Set PMU
	set_pmu_motion_img();
	delay(MBUS_DELAY*2);

	// Initialize MDv3
	initialize_md_reg();

	delay(MBUS_DELAY);

	md_start_motion = 0;
	md_capture_img = 0;
	md_count = 0;
	img_count = 0;
	
	// Reset global variables
	WAKEUP_PERIOD_CONT = 2;
	WAKEUP_PERIOD_CONT_INIT = 2; 

	USR_MD_INT_TIME = 12;
	USR_INT_TIME = 30;

	// FIXME
	mbus_write_message32(0xAA, 0xABCD1234);
	config_timerwd(200000000); // 2e7: 1min
	while(1);

}


//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
  
    // Reset Wakeup Timer; This is required for PRCv13
    set_wakeup_timer(100, 0, 1);

    // Initialize Interrupts
    // Only enable register-related interrupts
	enable_reg_irq();
  
	// FIXME
	mbus_write_message32(0xAA, 0x11111111);

	// Disable the watch-dog timer
	disable_timerwd();
	
    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
        operation_init();

    }
	
    // Proceed to continuous mode
    while(1){
        operation_md(); 
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

