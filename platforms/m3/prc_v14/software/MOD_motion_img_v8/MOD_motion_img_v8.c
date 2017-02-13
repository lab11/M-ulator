//****************************************************************************************************
//Author:       Gyouho Kim
//Description: 	IMGv3.0 System Code
//				Imaging and motion detection with MDv3 or MDv4
//				PRCv14/PMUv3/FLPv2/MRRv3
//*******************************************************************
#include "PRCv14.h"
#include "PRCv14_RF.h"
#include "mbus.h"
#include "PMUv3_RF.h"
#include "MDv3.h"
#include "MRRv3.h"

// uncomment this for debug mbus message
#define DEBUG_MBUS_MSG

// Stack order: PRC->HRV->MD->RAD->FLS->PMU
#define PRC_ADDR 0x1
#define MD_ADDR 0x4
#define MRR_ADDR 0x5
#define HRV_ADDR 0x6
#define FLS_ADDR 0x7
#define PMU_ADDR 0x8

// Common parameters
#define	MBUS_DELAY 200 //Amount of delay between successive messages; 5-6ms
#define WAKEUP_DELAY 20000 // 0.6s
#define DELAY_1 40000 // 5000: 0.5s
#define DELAY_2 120000 // 5000: 0.5s
#define IMG_TIMEOUT_COUNT 5000

// MDv3 Parameters
#define START_COL_IDX 0 // in words
#define COLS_TO_READ 39 // in # of words: 39 for full frame, 19 for half

// Radio configurations
#define RADIO_DATA_LENGTH 96
#define RADIO_TIMEOUT_COUNT 500
#define WAKEUP_PERIOD_RADIO_INIT 2
#define RADIO_PACKET_DELAY 4000 // Need 100-200ms

//***************************************************
// Global variables
//***************************************************
//Test Declerations
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t exec_count;
volatile uint32_t exec_count_irq;
volatile uint32_t mbus_msg_flag;
volatile uint32_t sleep_time_prev;
volatile uint32_t false_trigger_count;

volatile bool radio_ready;
volatile bool radio_on;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_img_idx;
volatile uint32_t radio_tx_img_all;
volatile uint32_t radio_tx_img_one;
volatile uint32_t radio_tx_img_num;

volatile uint32_t md_count;
volatile uint32_t md_valid;
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

volatile mrrv3_r00_t mrrv3_r00 = MRRv3_R00_DEFAULT;
volatile mrrv3_r01_t mrrv3_r01 = MRRv3_R01_DEFAULT;
volatile mrrv3_r02_t mrrv3_r02 = MRRv3_R02_DEFAULT;
volatile mrrv3_r03_t mrrv3_r03 = MRRv3_R03_DEFAULT;
volatile mrrv3_r04_t mrrv3_r04 = MRRv3_R04_DEFAULT;
volatile mrrv3_r05_t mrrv3_r05 = MRRv3_R05_DEFAULT;
volatile mrrv3_r06_t mrrv3_r06 = MRRv3_R06_DEFAULT;
volatile mrrv3_r07_t mrrv3_r07 = MRRv3_R07_DEFAULT;
volatile mrrv3_r08_t mrrv3_r08 = MRRv3_R08_DEFAULT;
volatile mrrv3_r09_t mrrv3_r09 = MRRv3_R09_DEFAULT;
volatile mrrv3_r0A_t mrrv3_r0A = MRRv3_R0A_DEFAULT;
volatile mrrv3_r0B_t mrrv3_r0B = MRRv3_R0B_DEFAULT;
volatile mrrv3_r0C_t mrrv3_r0C = MRRv3_R0C_DEFAULT;
volatile mrrv3_r0D_t mrrv3_r0D = MRRv3_R0D_DEFAULT;
volatile mrrv3_r0E_t mrrv3_r0E = MRRv3_R0E_DEFAULT;
volatile mrrv3_r0F_t mrrv3_r0F = MRRv3_R0F_DEFAULT;
volatile mrrv3_r10_t mrrv3_r10 = MRRv3_R10_DEFAULT;
volatile mrrv3_r11_t mrrv3_r11 = MRRv3_R11_DEFAULT;
volatile mrrv3_r12_t mrrv3_r12 = MRRv3_R12_DEFAULT;
volatile mrrv3_r13_t mrrv3_r13 = MRRv3_R13_DEFAULT;
volatile mrrv3_r1B_t mrrv3_r1B = MRRv3_R1B_DEFAULT;
volatile mrrv3_r1C_t mrrv3_r1C = MRRv3_R1C_DEFAULT;

volatile prcv14_r0B_t prcv14_r0B = PRCv14_R0B_DEFAULT;

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

static void set_pmu_sar_override(uint32_t val){
	// SAR_RATIO_OVERRIDE
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (val) 		// Binary converter's conversion ratio (7'h00)
	));
	delay(MBUS_DELAY*10);
}

static void set_pmu_motion(void){

	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
	// V0P6 Supply Active
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (0 << 13) // Enable main feedback loop
		| (8 << 9)  // Frequency multiplier R
		| (4 << 5)  // Frequency multiplier L (actually L+1)
		| (16) 		// Floor frequency base (0-31)
	));
	delay(MBUS_DELAY);
	// Register 0x16: SAR_TRIM_v3_ACTIVE
	// V1P2 Supply Active
    mbus_remote_register_write(PMU_ADDR,0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (5 << 9)  // Frequency multiplier R
		| (5 << 5)  // Frequency multiplier L (actually L+1)
		| (22) 		// Floor frequency base (0-31) //16
	));
	delay(MBUS_DELAY);
	set_pmu_sar_override(47);
	
}

static void set_pmu_img(void){

	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
	// V0P6 Supply Active: need to support up to 50uA
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (0 << 13) // Enable main feedback loop
		| (8 << 9)  // Frequency multiplier R
		| (8 << 5)  // Frequency multiplier L (actually L+1)
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
		| (0 << 13) // Enable main feedback loop
		| (7 << 9)  // Frequency multiplier R
		| (7 << 5)  // Frequency multiplier L (actually L+1)
		| (22) 		// Floor frequency base (0-31) //16
	));
	delay(MBUS_DELAY);

	set_pmu_sar_override(47);
	
}

static void set_pmu_motion_img_default(void){

	// Set PMU settings
	// UPCONV_TRIM_V3 Sleep
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (4) 		// Floor frequency base (0-31)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (4) 		// Floor frequency base (0-31)
	));
	delay(MBUS_DELAY);
	// UPCONV_TRIM_V3 Active
    mbus_remote_register_write(PMU_ADDR,0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (10) 		// Floor frequency base (0-31)
	));
	delay(MBUS_DELAY);
	// Register 0x19: DOWNCONV_TRIM_V3_SLEEP
	// V0P6 Supply Sleep: need to support up to 200nA
    mbus_remote_register_write(PMU_ADDR,0x19,
		( (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (1 << 5)  // Frequency multiplier L (actually L+1)
		| (8) 		// Floor frequency base (0-31)
	));
	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
	// V0P6 Supply Active: need to support up to 50uA
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (0 << 13) // Enable main feedback loop
		| (8 << 9)  // Frequency multiplier R
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
		| (0 << 13) // Enable main feedback loop
		| (2 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (6) 		// Floor frequency base (0-31) //8
	));
	delay(MBUS_DELAY);
	// Register 0x16: SAR_TRIM_v3_ACTIVE
	// V1P2 Supply Active: need to support up to 200uA
    mbus_remote_register_write(PMU_ADDR,0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (5 << 9)  // Frequency multiplier R
		| (5 << 5)  // Frequency multiplier L (actually L+1)
		| (22) 		// Floor frequency base (0-31) //16
	));
	delay(MBUS_DELAY);
	// SAR_RATIO_OVERRIDE
	// Use the new reset scheme in PMUv3
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (0 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (0 << 7) // Enable override setting [6:0] (1'h0)
		| (47) 		// Binary converter's conversion ratio (7'h00)
		// 48: 1.5V
		// 38: 1.1V
		// 41: 1.26V
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (47) 		// Binary converter's conversion ratio (7'h00)
	));
	delay(MBUS_DELAY);
	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    mbus_remote_register_write(PMU_ADDR,0x36,0x000001);
	delay(MBUS_DELAY);

}
//***************************************************
// Radio transmission routines for PPM Radio (RADv9)
//***************************************************

static void radio_power_on(){
	// Need to speed up sleep pmu clock

    // Turn on Current Limter
    mrrv3_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv3_r00.as_int);

    // Release FSM Sleep - Requires >2s stabilization time
    mrrv3_r0E.MRR_RAD_FSM_SLEEP = 0;  // Power on BB
    mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);

    mrrv3_r04.MRR_SCRO_EN_TIMER = 1;  //power on TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv3_r04.as_int);

    mrrv3_r04.MRR_SCRO_RSTN_TIMER = 1;  //UNRST TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv3_r04.as_int);

    // Additional delay required after SCRO Reset release
    delay(MBUS_DELAY*3); // At least 20ms required
    
    mrrv3_r04.MRR_SCRO_EN_CLK = 1;  //Enable clk
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv3_r04.as_int);

    radio_on = 1;

}

static void radio_power_off(){
	// Need to restore sleep pmu clock

	// FIXME

    // Turn off everything
    radio_on = 0;
	radio_ready = 0;

    mrrv3_r03.MRR_TRX_ISOLATEN = 0;     //set ISOLATEN 0
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv3_r03.as_int);

    mrrv3_r0E.MRR_RAD_FSM_EN = 0;  //Stop BB
    mrrv3_r0E.MRR_RAD_FSM_RSTN = 0;  //RST BB
    mrrv3_r0E.MRR_RAD_FSM_SLEEP = 1;
    mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);

    // Turn off Current Limter
    mrrv3_r00.MRR_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv3_r00.as_int);

    mrrv3_r04.MRR_SCRO_EN_TIMER = 0;
    mrrv3_r04.MRR_SCRO_RSTN_TIMER = 0;
    mrrv3_r04.MRR_SCRO_EN_CLK = 1;
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv3_r04.as_int);

}

static void send_radio_data_ppm(bool last_packet, uint32_t radio_data){
    // Write Data: Only up to 24bit data for now
    mrrv3_r06.MRR_RAD_FSM_TX_DATA_0 = radio_data; //SCC
    mbus_remote_register_write(MRR_ADDR,0x06,mrrv3_r06.as_int);
    delay(MBUS_DELAY);

    if (!radio_ready){
		radio_ready = 1;
		
		// Set the correct data length
		mrrv3_r0E.MRR_RAD_FSM_TX_D_LEN = 24;
		mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);

		// Release FSM Reset
		mrrv3_r0E.MRR_RAD_FSM_RSTN = 1;  //UNRST BB
		mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);

    	mrrv3_r03.MRR_TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    	mbus_remote_register_write(MRR_ADDR,0x03,mrrv3_r03.as_int);

		delay(MBUS_DELAY);
    }

    // Set CPU Halt Option as RX --> Use for register read e.g.
    set_halt_until_mbus_rx();

    // Fire off data
    uint32_t count;
    mbus_msg_flag = 0;
	mrrv3_r0E.MRR_RAD_FSM_EN = 1;  //Start BB
	mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);
    delay(MBUS_DELAY);

    for( count=0; count<RADIO_TIMEOUT_COUNT; count++ ){
		if( mbus_msg_flag ){
    		set_halt_until_mbus_tx();
			mbus_msg_flag = 0;
			if (last_packet){
				radio_ready = 0;
				radio_power_off();
			}else{
				mrrv3_r0E.MRR_RAD_FSM_EN = 0;
				mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);
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
    mbus_remote_register_write(MRR_ADDR,0x6,radio_data_0);
    delay(MBUS_DELAY);
    mbus_remote_register_write(MRR_ADDR,0x7,radio_data_1);
    delay(MBUS_DELAY);
    mbus_remote_register_write(MRR_ADDR,0x8,radio_data_2);
    delay(MBUS_DELAY);
    mbus_remote_register_write(MRR_ADDR,0x9,radio_data_3);
    delay(MBUS_DELAY);

    if (!radio_ready){
		radio_ready = 1;

		// Set the correct data length
		mrrv3_r0E.MRR_RAD_FSM_TX_D_LEN = 96;
		mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);

		// Release FSM Reset
		mrrv3_r0E.MRR_RAD_FSM_RSTN = 1;  //UNRST BB
		mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);

    	mrrv3_r03.MRR_TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    	mbus_remote_register_write(MRR_ADDR,0x03,mrrv3_r03.as_int);

		delay(MBUS_DELAY);
    }

    // Set CPU Halt Option as RX --> Use for register read e.g.
    set_halt_until_mbus_rx();

    // Fire off data
    uint32_t count;
    mbus_msg_flag = 0;
	mrrv3_r0E.MRR_RAD_FSM_EN = 1;  //Start BB
	mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);
    delay(MBUS_DELAY);

    for( count=0; count<RADIO_TIMEOUT_COUNT; count++ ){
		if( mbus_msg_flag ){
    		set_halt_until_mbus_tx();
			mbus_msg_flag = 0;
			if (last_packet){
				radio_ready = 0;
				radio_power_off();
			}else{
				mrrv3_r0E.MRR_RAD_FSM_EN = 0;
				mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);
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
	//radio_power_off();

	// Disable Timer
	set_wakeup_timer(0, 0, 0);

	// Go to sleep without timer
	operation_sleep();

}

//************************************
// FLPv2 Functions
//************************************

uint32_t check_flash_sram_full_image(){
	uint32_t idx;

	// Read 1 row at a time: 160*8 = 160 Bytes, 40 words
	// There are 160 rows
	// Flash SRAM Memory space is byte-addressable
   	set_halt_until_mbus_rx();
	for(idx=0; idx<160; idx++) {
		mbus_copy_mem_from_remote_to_any_stream(0xA, FLS_ADDR, (uint32_t*)(idx*160), 0x1, 39);
	}
    set_halt_until_mbus_tx();

	// Send dummy indicator for end of image
	mbus_write_message32(0x1E, 0);
	delay(MBUS_DELAY);

	return 1;
}

uint32_t send_radio_flash_sram(uint8_t addr_stamp, uint32_t length){
	uint32_t idx;
	uint32_t fls_rx_data;

	for(idx=0; (idx*3)<length; idx++) {

    	set_halt_until_mbus_rx();
		mbus_copy_mem_from_remote_to_any_bulk(FLS_ADDR, (uint32_t*)((idx*3) << 2), PRC_ADDR, (uint32_t*)&flash_read_data, 2);
		// Send 96 bits of data
    	set_halt_until_mbus_tx();
		send_radio_data_ppm_96(0,flash_read_data[0],flash_read_data[1],flash_read_data[2],flash_read_data[2]);
		delay(MBUS_DELAY);
	}

	return 1;
}

static void flash_turn_on(){

	set_pmu_sar_override(48);

}

static void flash_turn_off(){

	set_pmu_sar_override(47);

}

static void flash_erase_single_page(volatile uint32_t page_num){
	// page_num should be a multiple of 0x100
    mbus_remote_register_write(FLS_ADDR,0x09,page_num); // Set flash start addr
    set_halt_until_mbus_rx();
	mbus_remote_register_write(FLS_ADDR,0x07,((0x0 << 6) | (0x1 << 5) | (0x4 << 1) | (0x1 << 0))); // Do erase
    set_halt_until_mbus_tx();

}

static void flash_copy_flash2sram(volatile uint32_t length){

    mbus_remote_register_write(FLS_ADDR,0x08,0x0); // Set SRAM_START_ADDR
    set_halt_until_mbus_rx();
	mbus_remote_register_write(FLS_ADDR,0x07,((length << 6) | (0x1 << 5) | (0x1 << 1) | (0x1 << 0)));
    set_halt_until_mbus_tx();
}

static void flash_copy_sram2flash(volatile uint32_t length){

    // Copy SRAM to Flash (Fast Programming)
    mbus_remote_register_write(FLS_ADDR,0x08,0x0); // Set SRAM_START_ADDR
    set_halt_until_mbus_rx();
	mbus_remote_register_write(FLS_ADDR,0x07,((length << 6) | (0x1 << 5) | (0x2 << 1) | (0x1 << 0))); // Fast Programming
    set_halt_until_mbus_tx();
}


//************************************
// MDv3 Functions
//************************************

static void initialize_md_reg(){

	mdv3_r9 = MDv3_R9_DEFAULT;

	mdv3_r0.INT_TIME = USR_INT_TIME;
	mdv3_r0.MD_INT_TIME = USR_MD_INT_TIME;
	mdv3_r1.MD_TH = 10;
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

	mdv3_r4.SEL_CC  = 7; //4
	mdv3_r4.SEL_CC_B  = 0; //3

	mdv3_r5.SEL_CLK_RING = 2;
	mdv3_r5.SEL_CLK_DIV = 4;
	mdv3_r5.SEL_CLK_RING_4US = 0;
	mdv3_r5.SEL_CLK_DIV_4US = 1;
	mdv3_r5.SEL_CLK_RING_ADC = 2; 
	mdv3_r5.SEL_CLK_DIV_ADC = 1;
	mdv3_r5.SEL_CLK_RING_LC = 0;
	mdv3_r5.SEL_CLK_DIV_LC = 0;

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

static void start_md_init(){

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
	delay(MBUS_DELAY*4); //Need >10ms

	mdv3_r0.START_MD = 0;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);
	delay(MBUS_DELAY*4); //Need >10ms

	// Enable MD Flag
	// 1:3
	mdv3_r1.MD_TH_EN = 1;
	mbus_remote_register_write(MD_ADDR,0x1,mdv3_r1.as_int);
	delay(MBUS_DELAY);

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
	delay(MBUS_DELAY*4); //Need >10ms

	mdv3_r0.START_MD = 0;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);

	delay(DELAY_1); // about 0.5s
	delay(DELAY_1); // about 0.5s
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
	delay(MBUS_DELAY*4); //Need >10ms

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

static void poweroff_frame_controller(){

	// Set MD Isolation
	// 7:15
	mdv3_r7.ISOLATE_MD = 1;
	mbus_remote_register_write(MD_ADDR,0x7,mdv3_r7.as_int);
	delay(MBUS_DELAY);

	// Stop MD Clock
	// 5:12
	mdv3_r5.CLK_EN_MD = 0;
	mbus_remote_register_write(MD_ADDR,0x5,mdv3_r5.as_int);
	delay(MBUS_DELAY);

	// Set MD Reset
	// 2:23
	mdv3_r2.RESET_MD = 1;
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);
	delay(MBUS_DELAY);

	// Set MD Presleep & Sleep
	// 2:21-22
	mdv3_r2.PRESLEEP_MD = 1;
	mdv3_r2.SLEEP_MD = 1;
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);
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
	delay(DELAY_1);

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

	// Assert ADC Wrapper Reset
	// 6:0
	mdv3_r6.RESET_ADC_WRAPPER = 1;
	mbus_remote_register_write(MD_ADDR,0x6,mdv3_r6.as_int);

	// Assert ADC Isolation
	// 7:17
	mdv3_r7.ISOLATE_ADC_WRAPPER = 1;
	mbus_remote_register_write(MD_ADDR,0x7,mdv3_r7.as_int);

	// Assert IMG Presleep 
	// 2:20
	// Assert IMG Sleep
	// 2:19
	mdv3_r2.PRESLEEP_ADC = 1;
	mdv3_r2.SLEEP_ADC = 1;
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);

}


static void capture_image_single(){

	// Capture Image
	// 0:0
	mdv3_r0.TAKE_IMAGE = 1;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);
	delay(MBUS_DELAY*4); //Need >10ms

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

static void capture_image_single_with_flash(uint32_t page_offset){

	// Reset Mbus streaming write buffer offset
    mbus_remote_register_write(FLS_ADDR,0x37,0x0);

	// Start imaging
	capture_image_single();
	wait_for_interrupt(IMG_TIMEOUT_COUNT);	

	delay(MBUS_DELAY);

	// Power-gate MD Imager
	poweroff_array_adc();

	// Check Flash SRAM after image
	// FIXME: Debug only
	//check_flash_sram_full_image();


	// Copy SRAM to Flash
    mbus_remote_register_write(FLS_ADDR,0x09,page_offset); // Set flash start addr; should be a multiple of 0x100
	//flash_copy_sram2flash(0x1FFF); // FLS: 4 pages; 32kB
	flash_copy_sram2flash(0x19FF); // FLP: 26 pages; 26kB

}


static void operation_flash_erase(uint32_t erase_num){

	uint32_t count = 0;
	uint32_t page_offset = 0;

	if (!erase_num){
		erase_num = 1024; // Erase all pages (1MB)
	}

    for( count=0; count<erase_num; count++ ){
		flash_erase_single_page(page_offset); // Should be a multiple of 0x100
		page_offset = page_offset + 0x100;
		delay(MBUS_DELAY);
	}

}
static void operation_flash_read(uint32_t page_offset){

	// Copy Flash to SRAM
    mbus_remote_register_write(FLS_ADDR,0x09,page_offset); // Set flash start addr; should be a multiple of 0x800
	//flash_copy_flash2sram(0x1FFF); // FLS: 4 pages; 32kB
	flash_copy_flash2sram(0x19FF); // FLP: 26 pages; 26kB

	// Transmit recovered image via MBus
	check_flash_sram_full_image();

}

static void operation_md(void){

	if (false_trigger_count > 6){
		initialize_md_reg();
		poweron_frame_controller();
	}
	if (false_trigger_count > 30) {
		// Shut down MD
		mbus_write_message32(0xAF, 0xFAFAFAFA);
		clear_md_flag();
		initialize_md_reg();

		// Go to sleep w/o timer
		operation_sleep_notimer();
	}

	//delay(DELAY_2); // FIXME

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
		if (radio_tx_option){
			// radio
			send_radio_data_ppm(0,0xFAFA1234);	
		}
	}


	if (md_capture_img && md_valid){
		
		// Increase PMU strength for imaging and flash operation
		set_pmu_img();

		// Release power gates, isolation, and reset for imager array
		poweron_array_adc();

		// Capture a single image
		//capture_image_single();
		if (img_count < 64){

		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xA1, img_count);
			delay(MBUS_DELAY);
			mbus_write_message32(0xA1, 0x100 + img_count*0x1A00);
			delay(MBUS_DELAY);
		#endif
			capture_image_single_with_flash(0x100+img_count*0x1A00);
			img_count++;

		}

		// Turn off only the Flash layer
		mbus_sleep_layer_short(FLS_ADDR);
  		delay(MBUS_DELAY);

		if (radio_tx_option){
			// Radio out image data stored in flash
			send_radio_data_ppm(0,0xFAF000);
			send_radio_flash_sram(0xE4, 6475); // Full image
			send_radio_data_ppm(0,0xFAF000);
		}
	}

	if (md_start_motion){
		// Turn off other layers
		mbus_sleep_layer_short(MRR_ADDR);
  		delay(MBUS_DELAY);
		mbus_sleep_layer_short(HRV_ADDR);
  		delay(MBUS_DELAY);

		// Set PMU settings for motion detection
		set_pmu_motion();
		md_count++;

		// Start motion detection
		clear_md_flag();

		start_md();

	}else{
		// Turn off MDSENSOR
		poweroff_frame_controller();
		// Set PMU settings back to default
		set_pmu_motion();
	}


	// Reset wakeup timer
	set_wakeup_timer(1000000, 0, 1);

	// Go to sleep w/o timer
	operation_sleep_notimer();
}

static void operation_tx_image(void){

	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xA2, radio_tx_img_idx);
		delay(MBUS_DELAY);
		mbus_write_message32(0xA2, 0x100 + radio_tx_img_idx*0x1A00);
		delay(MBUS_DELAY);
	#endif

	// Set PMU
	set_pmu_img();
	
	// Read image from Flash 
	operation_flash_read(0x100 + radio_tx_img_idx*0x1A00);

	// Send image to radio
	// Commnet out for now
	//send_radio_flash_sram(0xE4, 6475); // Full image

	if (!radio_tx_img_one && (radio_tx_img_idx < radio_tx_img_num)){
		radio_tx_img_idx++;
		
		// Send next image after sleep/wakeup
		set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
		operation_sleep_noirqreset();
    }else{

		// All done
		// Turn off only the Flash layer
		mbus_sleep_layer_short(FLS_ADDR);
  		delay(MBUS_DELAY);
		set_pmu_motion();
  		delay(MBUS_DELAY);

		// This is also the end of this IRQ routine
		exec_count_irq = 0;

		// Go to sleep without timer
		operation_sleep_notimer();
    }

}


static void operation_init(void){
  
    // Set CPU Halt Option as TX --> Use for register write e.g.
    set_halt_until_mbus_tx();
	

	// Set CPU & Mbus Clock Speeds
    prcv14_r0B.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
    prcv14_r0B.CLK_GEN_RING = 0x3; // Default 0x1
    prcv14_r0B.CLK_GEN_DIV_MBC = 0x0; // Default 0x1
    prcv14_r0B.CLK_GEN_DIV_CORE = 0x2; // Default 0x3
	*((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv14_r0B.as_int;

    //Enumerate & Initialize Registers
    enumerated = 0xABCD1234;
    exec_count = 0;
    exec_count_irq = 0;
    mbus_msg_flag = 0;
	sleep_time_prev = 0;

    // Enumeration
	// Stack order: PRC->HRV->MD->RAD->FLS->PMU
    //mbus_enumerate(HRV_ADDR);
    //delay(MBUS_DELAY);
    mbus_enumerate(MD_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(MRR_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(FLS_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(PMU_ADDR);
    delay(MBUS_DELAY);

	// Set PMU
	set_pmu_motion_img_default();
	delay(MBUS_DELAY*2);

    // Radio Settings (MRRv3) -------------------------------------------
    mrrv3_r1C.LC_CLK_RING = 0x3;  // ~ 150 kHz
    mrrv3_r1C.LC_CLK_DIV = 0x3;  // ~ 150 kHz
    mbus_remote_register_write(MRR_ADDR,0x1C,mrrv3_r1C.as_int);

    // Current Limter set-up 
    mrrv3_r00.MRR_CL_CTRL = 0x01; //Set CL 1-finite 16-20uA

    // TX Setup Carrier Freq
    mrrv3_r00.MRR_TRX_CAP_ANTP_TUNE = 0x0FFF;  //ANT CAP 14b unary 830.5 MHz
	//mrrv3_r00.MRR_TRX_CAP_ANTP_TUNE = 0x00FF;  //ANT CAP 14b unary 813.8 MHz
	//mrrv3_r00.MRR_TRX_CAP_ANTP_TUNE = 0x0FFF;  //ANT CAP 14b unary 805.5 MHz
    mrrv3_r01.MRR_TRX_CAP_ANTN_TUNE = 0x0FFF; //ANT CAP 14b unary 830.5 MHz
	//mrrv3_r01.MRR_TRX_CAP_ANTN_TUNE = 0x00FF; //ANT CAP 14b unary 813.8 MHz
	//mrrv3_r01.MRR_TRX_CAP_ANTN_TUNE = 0x0FFF;  //ANT CAP 14b unary 805.5 MHz
    mrrv3_r02.MRR_TX_BIAS_TUNE = 0x1FFF;  //Set TX BIAS TUNE 13b // Set to max
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv3_r00.as_int);
    mbus_remote_register_write(MRR_ADDR,0x01,mrrv3_r01.as_int);
    mbus_remote_register_write(MRR_ADDR,0x02,mrrv3_r02.as_int);

    // RX Setup
    mrrv3_r03.MRR_RX_BIAS_TUNE    = 0x0AFF;//  turn on Q_enhancement
	//mrrv3_r03.MRR_RX_BIAS_TUNE    = 0x0000;//  turn off Q_enhancement
    mrrv3_r03.MRR_RX_SAMPLE_CAP    = 0x1;  // RX_SAMPLE_CAP
    mbus_remote_register_write(MRR_ADDR,3,mrrv3_r03.as_int);

    mrrv3_r11.MRR_RAD_FSM_RX_POWERON_LEN = 0x0;  //Set RX Power on length
    mrrv3_r11.MRR_RAD_FSM_RX_SAMPLE_LEN = 0x3;  //Set RX Sample length  16us
    //mrrv3_r11.MRR_RAD_FSM_RX_SAMPLE_LEN = 0x0;  //Set RX Sample length  4us
    mrrv3_r11.MRR_RAD_FSM_GUARD_LEN = 0x000F; //Set TX_RX Guard length, TX_RX guard 32 cycle (28+5)
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv3_r11.as_int);

    mrrv3_r12.MRR_RAD_FSM_RX_HDR_BITS = 0x00;  //Set RX header
    mrrv3_r12.MRR_RAD_FSM_RX_HDR_TH = 0x00;    //Set RX header threshold
    mrrv3_r12.MRR_RAD_FSM_RX_DATA_BITS = 0x10; //Set RX data 16b
    mbus_remote_register_write(MRR_ADDR,0x12,mrrv3_r12.as_int);

    mrrv3_r1B.MRR_IRQ_REPLY_PACKET = 0x061400; //Read RX data Reply
    mbus_remote_register_write(MRR_ADDR,0x1B,mrrv3_r1B.as_int);

    // RAD_FSM set-up 
    mrrv3_r0E.MRR_RAD_FSM_TX_H_LEN = 31; //31-31b header (max)
    mrrv3_r0E.MRR_RAD_FSM_TX_D_LEN = RADIO_DATA_LENGTH;//40; //0-skip tx data
    mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);

    mrrv3_r0F.MRR_RAD_FSM_TX_PW_LEN = 0; //4us PW
    mrrv3_r10.MRR_RAD_FSM_TX_C_LEN = 32; // (PW_LEN+1):C_LEN=1:32
    mrrv3_r0F.MRR_RAD_FSM_TX_PS_LEN = 0; // PW=PS
    mrrv3_r12.MRR_RAD_FSM_TX_HDR_CNST = 0; //no shift in LFSR

    //mrrv3_r0F.MRR_RAD_FSM_TX_PW_LEN = 1; //8us PW
    //mrrv3_r10.MRR_RAD_FSM_TX_C_LEN = 64; // (PW_LEN+1):C_LEN=1:32
    //mrrv3_r0F.MRR_RAD_FSM_TX_PS_LEN = 1; // PW=PS
    //mrrv3_r0F.MRR_RAD_FSM_TX_PW_LEN = 124; //500us PW
    //mrrv3_r10.MRR_RAD_FSM_TX_C_LEN = 4000; // (PW_LEN+1):C_LEN=1:32
    //mrrv3_r0F.MRR_RAD_FSM_TX_PS_LEN = 124; // PW=PS
    
    //mrrv3_r0F.MRR_RAD_FSM_TX_PW_LEN = 249; //1ms PW
    //mrrv3_r10.MRR_RAD_FSM_TX_C_LEN = 8000; // (PW_LEN+1):C_LEN=1:32
    //mrrv3_r0F.MRR_RAD_FSM_TX_PS_LEN = 249; // PW=PS
    //mrrv3_r12.MRR_RAD_FSM_TX_HDR_CNST = 8; //8 bit shift in LFSR

    mbus_remote_register_write(MRR_ADDR,0x0F,mrrv3_r0F.as_int);
    mbus_remote_register_write(MRR_ADDR,0x12,mrrv3_r12.as_int);
    
	// Use pulse generator -- Not used currently
    mrrv3_r02.MRR_TX_PULSE_FINE = 0;
    mrrv3_r02.MRR_TX_PULSE_FINE_TUNE = 3;
    mbus_remote_register_write(MRR_ADDR,0x02,mrrv3_r02.as_int);

    mrrv3_r10.MRR_RAD_FSM_SEED = 1; //default
    mrrv3_r10.MRR_RAD_FSM_TX_MODE = 3; //code rate 0:4 1:3 2:2 3:1(baseline) 4:1/2 5:1/3 6:1/4
    mbus_remote_register_write(MRR_ADDR,0x10,mrrv3_r10.as_int);

    mrrv3_r11.MRR_RAD_FSM_TX_POWERON_LEN = 7; //3bits
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv3_r11.as_int);

    // Flash settings (FLPv2) ------------------------------
    // Tune Flash
    mbus_remote_register_write(FLS_ADDR,0x26,0x0D7788); // Program Current
    delay(MBUS_DELAY);
    mbus_remote_register_write(FLS_ADDR,0x27,0x011BC8); // Pump Diode Chain
    delay(MBUS_DELAY);
    mbus_remote_register_write(FLS_ADDR,0x01,0x000109); // Tprog idle time
    delay(MBUS_DELAY);
    mbus_remote_register_write(FLS_ADDR,0x19,0x000F03); // Voltage Clamper Tuning // F03 default
    delay(MBUS_DELAY);
    mbus_remote_register_write(FLS_ADDR,0x12,0x000003); // Auto Power On/Off
    delay(MBUS_DELAY);

    // Initialize other global variables
	md_start_motion = 0;
	md_capture_img = 0;
	md_count = 0;
	img_count = 0;
	md_valid = 0;
	false_trigger_count = 0;
	
	radio_on = 0;
	radio_ready = 0;
	radio_tx_option = 0;
	radio_tx_img_idx = 0;
	radio_tx_img_all = 0;
	radio_tx_img_one = 0;
	radio_tx_img_num = 0;

	// Reset global variables
	WAKEUP_PERIOD_CONT = 2;
	WAKEUP_PERIOD_CONT_INIT = 2; 

	USR_MD_INT_TIME = 12;
	USR_INT_TIME = 30;

	// Go to sleep w/o timer
	operation_sleep_notimer();

}

static void operation_goc_trigger_init(void){

	// FIXME

	// This is critical
	set_halt_until_mbus_tx();
	mbus_write_message32(0xAA,0xABCD1234);
	mbus_write_message32(0xAA,wakeup_data);

	// Initialize variables & registers
	//Tstack_state = TSTK_IDLE;
	
	radio_power_off();
	//ldo_power_off();
	//temp_power_off();
}


//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
  
	// Record sleep time
	sleep_time_prev = *((volatile uint32_t *) REG_WUPT_VAL);
	if (sleep_time_prev > 0){
		md_valid = 1;
		false_trigger_count = 0;
	}else{ // May be due to false trigger
		mbus_write_message32(0xAF, sleep_time_prev);
		md_valid = 0;
		false_trigger_count++;
	}

    // Reset Wakeup Timer; This is required for PRCv13
	//mbus_write_message32(0xAA, sleep_time_prev);
    //set_wakeup_timer(100, 0, 1);

    // Initialize Interrupts
    // Only enable register-related interrupts
	enable_reg_irq();

	// Set the watch-dog timer
	config_timerwd(40000000); // 2e7: 1min
	//disable_timerwd();

	// Disable Mbus watchdog
	//*((volatile uint32_t *) MBCWD_RESET) = 1;
	
    // Initialization sequence
    if (enumerated != 0xABCD1234){
        operation_init();
    }
	
    // Check if wakeup is due to GOC interrupt  
    // 0x78 is reserved for GOC-triggered wakeup (Named IRQ14VEC)
    // 8 MSB bits of the wakeup data are used for function ID
    wakeup_data = *((volatile uint32_t *) IRQ14VEC);	// IRQ14VEC[31:0]
    uint32_t wakeup_data_header = wakeup_data>>24;				// IRQ14VEC[31:24]
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;			// IRQ14VEC[7:0]
    uint32_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;		// IRQ14VEC[15:8]
    uint32_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;		// IRQ14VEC[23:16]

	// In case GOC triggered in the middle of routines
	if ((wakeup_data_header != 0) && (exec_count_irq == 0)){
		operation_goc_trigger_init();
	}

    if(wakeup_data_header == 1){
        // Debug mode: Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the MSB of # of transmissions
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
        delay(MBUS_DELAY);
        if (exec_count_irq < (wakeup_data_field_0 + (wakeup_data_field_2<<8))){
            exec_count_irq++;
			if (exec_count_irq == 1){
				// Prepare radio TX
				radio_power_on();
				// Go to sleep for SCRO stabilitzation
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
				operation_sleep_noirqreset();
			}else{
				// radio
				send_radio_data_ppm(0,0xABC000+exec_count_irq);	
				// set timer
				set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
				// go to sleep and wake up with same condition
				operation_sleep_noirqreset();
			}
        }else{
            exec_count_irq = 0;
            // radio
            send_radio_data_ppm(1,0xFAF000);	
            // Go to sleep without timer
            operation_sleep_notimer();
        }
   
    }else if(wakeup_data_header == 2){
		// Start motion detection
        // wakeup_data[7:0] is the md integration period
        // wakeup_data[15:8] is the img integration period
        // wakeup_data[17:16] indicates whether or not to to take an image
		// 						1: md only, 2: img only, 3: md+img
        // wakeup_data[18] indicates whether or not to radio out the result
        // wakeup_data[19] reset img count
		USR_MD_INT_TIME = wakeup_data_field_0;
		USR_INT_TIME = wakeup_data_field_1;

		// Reset IRQ14VEC
		*((volatile uint32_t *) IRQ14VEC) = 0;

		// Run Program
		exec_count_irq = 0;
		exec_count = 0;
		md_count = 0;
		false_trigger_count = 0;
		radio_ready = 0;
		md_start_motion = wakeup_data_field_2 & 0x1;
		md_capture_img = (wakeup_data_field_2 >> 1) & 0x1;
		radio_tx_option = (wakeup_data_field_2 >> 2) & 0x1;

		if ((wakeup_data_field_2 >> 3) & 0x1){
			img_count = 0;
		}

		if (radio_tx_option & !radio_on){
			// Prepare radio TX
			radio_power_on();
			// Go to sleep for SCRO stabilitzation
			set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
			operation_sleep_noirqreset();
		}
		if (md_capture_img){
			// Increase PMU strength for imaging and flash operation
			//set_pmu_img();
			//delay(DELAY_1); // about 0.5s
		}

		operation_md();
   
    }else if(wakeup_data_header == 3){
		// Stop MD program and transmit the execution count n times
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
		
		clear_md_flag();
		md_start_motion = 0;
		md_capture_img = 0;
		radio_ready = 0;

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
			if (exec_count_irq == 1){
				// Initialize MD registers
				initialize_md_reg();
				// Prepare radio TX
				radio_power_on();
				// Go to sleep for SCRO stabilitzation
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
				operation_sleep_noirqreset();
			}else{
				// radio
				send_radio_data_ppm(0,0xC00000+img_count);	
				// set timer
				set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
				// go to sleep and wake up with same condition
				operation_sleep_noirqreset();
			}
        }else{
            exec_count_irq = 0;
            send_radio_data_ppm(1,0xFAF000);	
            // Go to sleep without timer
            operation_sleep_notimer();
        }

    }else if(wakeup_data_header == 4){
		// Read out stored image from flash and transmit
        // wakeup_data[7:0] is the # of image to transmit; Valid range is from 0 (first and oldest image) to img_count-1
        // wakeup_data[15:8] is the user-specified period 
        // wakeup_data[16]: transmit all stored images 
        // wakeup_data[17]: transmit only one image according to the image ID; this will override [16]
		// To read out first X images regardless of the program state, specify 0 to both [16] and [17]
		radio_tx_img_num = wakeup_data_field_0;
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
		radio_tx_img_all = wakeup_data_field_2 & 0x1;
		radio_tx_img_one = (wakeup_data_field_2>>1) & 0x1;

		if (exec_count_irq == 0){ // Only do this once
			// Stop motion detection in case it was running
			clear_md_flag();
			initialize_md_reg();

			if (radio_tx_img_one){
				radio_tx_img_idx = radio_tx_img_num;
			}else{
				radio_tx_img_idx = 0; // start from oldest image
			}
			if (radio_tx_img_all){
				radio_tx_img_num = img_count;
			}
		}
		
		exec_count_irq++;

/*
		// comment out for now
		// Set PMU
		set_pmu_img();
  		delay(MBUS_DELAY);
        if (exec_count_irq < 3){
			if (exec_count_irq == 1){
				// Prepare radio TX
				radio_power_on();
				// Go to sleep for SCRO stabilitzation
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
				operation_sleep_noirqreset();
			}else{
				send_radio_data_ppm(0, 0xFAF000+exec_count_irq);
				// set timer
				set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
				// go to sleep and wake up with same condition
				operation_sleep_noirqreset();
			}
		}else{
			operation_tx_image();
		}
*/		

		operation_tx_image();

    }else if(wakeup_data_header == 5){
		// Erase pages in flash
        // wakeup_data[7:0] is the # of images to erase (4 pages per image)
		// If 0, erase all pages

		// Stop motion detection in case it was running
		clear_md_flag();
		initialize_md_reg();

		// Erase all pages of flash
		set_pmu_img();
  		delay(MBUS_DELAY);
		operation_flash_erase(wakeup_data_field_0*26);

		// Turn off only the Flash layer
		mbus_sleep_layer_short(FLS_ADDR);
  		delay(MBUS_DELAY);
		set_pmu_motion();
  		delay(MBUS_DELAY);

		// Reset img count so that page 0 can be used
		img_count = 0;

		// Go to sleep without timer
		operation_sleep_notimer();

    }

    // Proceed to continuous mode
    while(1){
        operation_md(); 
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

