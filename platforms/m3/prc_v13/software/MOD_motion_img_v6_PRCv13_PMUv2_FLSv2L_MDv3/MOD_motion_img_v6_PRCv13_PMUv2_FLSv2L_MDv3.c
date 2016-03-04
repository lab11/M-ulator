//*******************************************************************
//Author: Gyouho Kim
//Description: 	MODv2.0 System Code
//				Imaging and motion detection with MDv3
//				FLASH storage with FLSv2
//*******************************************************************
#include "PRCv13.h"
#include "mbus.h"
#include "PMUv2_RF.h"
#include "MDv3.h"
#include "RADv9.h"

// Stack order  PRC->HRV->MD->RAD->FLS
#define HRV_ADDR 0x6
#define MD_ADDR 0x4
#define RAD_ADDR 0x5
#define FLS_ADDR 0x7

#define	MBUS_DELAY 400 //Amount of delay between successive messages
#define WAKEUP_DELAY 40000 // 0.6s
#define DELAY_1 20000 // 5000: 0.5s
#define DELAY_IMG 80000 // 1s

// MDv3 Parameters
#define START_COL_IDX 0 // in words
#define COLS_TO_READ 39 // in # of words: 39 for full frame, 19 for half

// Radio Parameters
#define RADIO_DATA_LENGTH 24
#define RADIO_PACKET_DELAY 3000
#define RADIO_TIMEOUT_COUNT 500
#define WAKEUP_PERIOD_RADIO_INIT 3

// FLSv1 configurations
//#define FLS_RECORD_LENGTH 0x18FE // In words; # of words stored -2
#define FLS_RECORD_LENGTH 6473 // In words; # of words stored -2; Full image with mbus overhead
//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t exec_count;
volatile uint32_t exec_count_irq;
volatile uint32_t MBus_msg_flag;

volatile bool radio_ready;
volatile bool radio_on;
volatile bool radio_tx_option;

volatile uint32_t md_count;
volatile uint8_t md_capture_img;
volatile uint8_t md_start_motion;
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

volatile uint32_t WAKEUP_PERIOD_CONT;
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

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
void handler_ext_int_2(void)  { *NVIC_ICPR = (0x1 << 2);  } // REG0
void handler_ext_int_3(void)  { *NVIC_ICPR = (0x1 << 3);  } // REG1
void handler_ext_int_4(void)  { *NVIC_ICPR = (0x1 << 4);  } // REG2
void handler_ext_int_5(void)  { *NVIC_ICPR = (0x1 << 5);  } // REG3
void handler_ext_int_6(void)  { *NVIC_ICPR = (0x1 << 6);  } // REG4
void handler_ext_int_7(void)  { *NVIC_ICPR = (0x1 << 7);  } // REG5
void handler_ext_int_8(void)  { *NVIC_ICPR = (0x1 << 8);  } // REG6
void handler_ext_int_9(void)  { *NVIC_ICPR = (0x1 << 9);  } // REG7
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); } // MEM WR
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); } // MBUS_RX
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); } // MBUS_TX
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); } // MBUS_FWD
void handler_ext_int_14(void) { *NVIC_ICPR = (0x1 << 14); } // MBUS_FWD

//*******************************************************************
// System Functions
//*******************************************************************
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

static void operation_sleep_for(uint16_t num_sleep_cycles){
	// Set Wakeup Timer
    set_wakeup_timer(num_sleep_cycles, 1, 1);

	// Go to Sleep
	operation_sleep();
	while(1);
}

static void operation_sleep_notimer(void){
    
	// Disable Timer
	set_wakeup_timer (0, 0, 0);

	// Go to sleep without timer
	operation_sleep();

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
}

static void radio_power_off(){
	// Need to restore sleep pmu clock
	set_pmu_sleep_clk_default();

    // Turn off everything
    radio_on = 0;
    radv9_r2.SCRO_ENABLE = 0;
    radv9_r2.SCRO_RESET  = 1;
    mbus_remote_register_write(RAD_ADDR,2,radv9_r2.as_int);
    delay(MBUS_DELAY);
    radv9_r13.RAD_FSM_SLEEP 		= 1;
    radv9_r13.RAD_FSM_ISOLATE 	= 1;
    radv9_r13.RAD_FSM_RESETn 	= 0;
    radv9_r13.RAD_FSM_ENABLE 	= 0;
    mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
    delay(MBUS_DELAY);
}

static void send_radio_data_ppm(bool last_packet, uint32_t radio_data){
    // Write Data: Only up to 24bit data for now
    radv9_r3.RAD_FSM_DATA = radio_data;
    mbus_remote_register_write(RAD_ADDR,3,radv9_r3.as_int);
    delay(MBUS_DELAY);

    if (!radio_ready){
		radio_ready = 1;

		// Release FSM Isolate
		radv9_r13.RAD_FSM_ISOLATE = 0;
		mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
		delay(MBUS_DELAY);

		// Release FSM Reset
		radv9_r13.RAD_FSM_RESETn = 1;
		mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
		delay(MBUS_DELAY);
    }

	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xBB, 0x0);
	#endif
    // Fire off data
    uint32_t count;
    MBus_msg_flag = 0;
    radv9_r13.RAD_FSM_ENABLE = 1;
    mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
    delay(MBUS_DELAY);

    for( count=0; count<RADIO_TIMEOUT_COUNT; count++ ){
		if( MBus_msg_flag ){
			MBus_msg_flag = 0;
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
	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xBB, 0xFAFAFAFA);
	#endif
}

//************************************
// FLSv2 Functions
//************************************
uint32_t check_flash_payload (uint8_t mbus_addr, uint32_t expected_payload) {
		if(FLSv2MBusGPIO_getRxData0() != expected_payload) {
			mbus_write_message32(mbus_addr, 0xDEADBEEF);
			delay(MBUS_DELAY);
			delay(MBUS_DELAY);
			mbus_write_message32(mbus_addr, FLSv2MBusGPIO_getRxData0());
    		operation_sleep_notimer();
			while(1);
		}
		return 1;
}

uint32_t check_flash_sram (uint8_t addr_stamp, uint32_t length) {
	uint32_t idx;

	for(idx=0; idx<length; idx++) {

		FLSv2MBusGPIO_readMem(FLS_ADDR, 0xEE, 0, ((uint32_t) idx) << 2);
		FLSv2MBusGPIO_rxMsg(); // Rx
		
		write_mbus_message (addr_stamp, FLSv2MBusGPIO_getRxData0());
		delay(MBUS_DELAY);
		delay(MBUS_DELAY);
	}

	return 1;
}

uint32_t send_radio_flash_sram (uint8_t addr_stamp, uint32_t length) {
	uint32_t idx;
	uint32_t fls_rx_data;

	for(idx=0; idx<length; idx++) {

		FLSv2MBusGPIO_readMem(FLS_ADDR, 0xEE, 0, ((uint32_t) idx) << 2);
		FLSv2MBusGPIO_rxMsg(); // Rx
		fls_rx_data = FLSv2MBusGPIO_getRxData0();
		mbus_write_message32(addr_stamp, fls_rx_data);
		delay(MBUS_DELAY);
		mbus_write_message32(addr_stamp, idx);
		delay(MBUS_DELAY);
		send_radio_data_ppm(0,fls_rx_data);
		delay(MBUS_DELAY);
	}

	return 1;
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

	mdv3_r8.MBUS_REPLY_ADDR_FLAG = 0x16;
	mdv3_r9.MBUS_REPLY_ADDR_DATA = 0x17; // IMG Data return address

	mdv3_r8.MBUS_START_ADDR = 0; // Start column index in words
	mdv3_r8.MBUS_LENGTH_M1 = 39; // Columns to be read; in # of words: 39 for full frame, 19 for half

	//Write Registers
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);
	delay (MBUS_DELAY);
	mbus_remote_register_write(MD_ADDR,0x1,mdv3_r1.as_int);
	delay (MBUS_DELAY);
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);
	delay (MBUS_DELAY);
	mbus_remote_register_write(MD_ADDR,0x3,mdv3_r3.as_int);
	delay (MBUS_DELAY);
	mbus_remote_register_write(MD_ADDR,0x4,mdv3_r4.as_int);
	delay (MBUS_DELAY);
	mbus_remote_register_write(MD_ADDR,0x5,mdv3_r5.as_int);
	delay (MBUS_DELAY);
	mbus_remote_register_write(MD_ADDR,0x6,mdv3_r6.as_int);
	delay (MBUS_DELAY);
	mbus_remote_register_write(MD_ADDR,0x7,mdv3_r7.as_int);
	delay (MBUS_DELAY);
	mbus_remote_register_write(MD_ADDR,0x8,mdv3_r8.as_int);
	delay (MBUS_DELAY);
	mbus_remote_register_write(MD_ADDR,0x9,mdv3_r9.as_int);
	delay (MBUS_DELAY);

}

static void start_md(){

	// Optionally release MD GPIO Isolation
	// 7:16
	//mdv3_r7.ISOLATE_GPIO = 0;
	//mbus_remote_register_write(MD_ADDR,0x7,mdv3_r7.as_int);
	//delay (MBUS_DELAY);
	//delay(DELAY_500ms); // about 0.5s

	// Start MD
	// 0:1
	mdv3_r0.START_MD = 1;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);
	delay(MBUS_DELAY*2);

	mdv3_r0.START_MD = 0;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);
	delay(MBUS_DELAY);

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
	delay(MBUS_DELAY*2);

	mdv3_r0.STOP_MD = 0;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);
	delay(MBUS_DELAY);

	// Clear MD Flag
	// 1:4
	mdv3_r1.MD_TH_CLEAR = 1;
	mbus_remote_register_write(MD_ADDR,0x1,mdv3_r1.as_int);
	delay(MBUS_DELAY*2);

	mdv3_r1.MD_TH_CLEAR = 0;
	mbus_remote_register_write(MD_ADDR,0x1,mdv3_r1.as_int);
	delay(MBUS_DELAY);

	// Disable MD Flag
	// 1:3
	mdv3_r1.MD_TH_EN = 0;
	mbus_remote_register_write(MD_ADDR,0x1,mdv3_r1.as_int);
	delay(MBUS_DELAY);

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

static void poweron_frame_controller_short(){

	// Release MD Presleep
	// 2:22
	mdv3_r2.PRESLEEP_MD = 0;
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);
	delay(MBUS_DELAY);

	// Release MD Sleep
	// 2:21
	mdv3_r2.SLEEP_MD = 0;
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);
	delay(MBUS_DELAY);

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
	delay (MBUS_DELAY);

	// Release ADC Wrapper Reset
	// 6:0
	mdv3_r6.RESET_ADC_WRAPPER = 0;
	mbus_remote_register_write(MD_ADDR,0x6,mdv3_r6.as_int);
	delay (MBUS_DELAY);

	// Start ADC Clock
	// 5:13
	mdv3_r5.CLK_EN_ADC = 1;
	mbus_remote_register_write(MD_ADDR,0x5,mdv3_r5.as_int);
	delay (MBUS_DELAY);

}

static void poweroff_array_adc(){

	// Stop ADC Clock
	// 5:13
	mdv3_r5.CLK_EN_ADC = 0;
	mbus_remote_register_write(MD_ADDR,0x5,mdv3_r5.as_int);
	delay (MBUS_DELAY);

	// Assert ADC Wrapper Reset
	// 6:0
	mdv3_r6.RESET_ADC_WRAPPER = 1;
	mbus_remote_register_write(MD_ADDR,0x6,mdv3_r6.as_int);
	delay (MBUS_DELAY);

	// Assert ADC Isolation
	// 7:17
	mdv3_r7.ISOLATE_ADC_WRAPPER = 1;
	mbus_remote_register_write(MD_ADDR,0x7,mdv3_r7.as_int);
	delay (MBUS_DELAY);

	// Assert IMG Presleep 
	// 2:20
	// Assert IMG Sleep
	// 2:19
	mdv3_r2.PRESLEEP_ADC = 1;
	mdv3_r2.SLEEP_ADC = 1;
	mbus_remote_register_write(MD_ADDR,0x2,mdv3_r2.as_int);
	delay (MBUS_DELAY);

}

static void capture_image_single(){

	// Capture Image
	// 0:0
	mdv3_r0.TAKE_IMAGE = 1;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);
	delay(MBUS_DELAY*2); //Need ~10ms

	mdv3_r0.TAKE_IMAGE = 0;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);

}

static void capture_image_start(){

	// Capture Image
	// 0:0
	mdv3_r0.TAKE_IMAGE = 1;
	mbus_remote_register_write(MD_ADDR,0x0,mdv3_r0.as_int);
	delay (MBUS_DELAY);

}

static void operation_md(void){

	// Release power gates, isolation, and reset for frame controller
	if (md_count == 0) {
		initialize_md_reg();
		poweron_frame_controller();
	}else{
		// This wakeup is due to motion detection
		// Let the world know!
		mbus_write_message32(0xAA, 0x22222222);
  		delay (MBUS_DELAY);
		clear_md_flag();
        // radio
        send_radio_data_ppm(0,0xFAFA1234);	
	}

	if (md_capture_img){

		// Release power gates, isolation, and reset for imager array
		poweron_array_adc();

		// Capture a single image
		//capture_image_single();
		capture_image_single_with_flash();
		
		poweroff_array_adc();

		if (radio_tx_option){
			// Radio out image data stored in flash
			send_radio_data_ppm(0,0xFAFA0000);
			//send_radio_flash_sram(0xE4, 6475); // Full image
			send_radio_flash_sram(0xE4, 100);
			send_radio_data_ppm(0,0xFAFA0000);
		}
	}

	if (md_start_motion){
		// Only need to set sleep PMU settings
        //set_pmu_sleep_clk_fastest();
		
		md_count++;

		// Start motion detection
		start_md();
		clear_md_flag();
		delay(MBUS_DELAY);
		start_md();

	}else{
		// Restore PMU_CTRL setting
		//set_pmu_sleep_clk_default();
	}

	if (md_capture_img){
		// Make FLS Layer go to sleep through GPIO
		FLSv2MBusGPIO_sleep();
	}

	// Go to sleep w/o timer
	operation_sleep_notimer();
}

static void capture_image_single_with_flash(void){

	// Set Flash Ext Streaming Length
	FLSv2MBusGPIO_configExtStream(FLS_ADDR, 0x1, 0x0, 0xFFFFF);

	// Give a "Go Flash Ext Streaming" command (only the first 31-bit)
	uint32_t fls_stream_short_prefix = ((uint32_t) FLS_ADDR) << 4;
	uint32_t fls_stream_reg_data = ((0x07 << 24) | (FLS_RECORD_LENGTH << 6) | (0x1 << 5) | (0x6 << 1) | (0x1 << 0));
	FLSv2MBusGPIO_sendMBus31bit (fls_stream_short_prefix, fls_stream_reg_data);

	// Start imaging
	capture_image_single();

	// Give a "Go Flash Ext Streaming" command (the last 1-bit)
	delay(MBUS_DELAY*2); // ~ 15ms
	FLSv2MBusGPIO_sendMBusLast1bit (fls_stream_short_prefix, fls_stream_reg_data);
	
	// Receive the Payload
	FLSv2MBusGPIO_rxMsg();
	delay(DELAY_IMG);
	check_flash_payload (0xA3, 0x000000F9); // 0xF9 (pass), 0xF5 (timeout), 0xF7 (too fast)

	// Turn on Flash
	FLSv2MBusGPIO_turnOn(FLS_ADDR);
	FLSv2MBusGPIO_rxMsg(); // Rx Payload 
	check_flash_payload (0xA4, 0x000000B5);

	delay(MBUS_DELAY);
	mbus_write_message32(0xF1, 0x11111111);
	delay(MBUS_DELAY);

	// Copy SRAM to Flash
	FLSv2MBusGPIO_setFlashStartAddr(FLS_ADDR, 0x800); // Should be a multiple of 0x800
	FLSv2MBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x000);
	FLSv2MBusGPIO_doCopySRAM2Flash(FLS_ADDR, 0x1FFF); // 4 Pages; 0x2000 - 1
	FLSv2MBusGPIO_rxMsg();
	check_flash_payload (0xA5, 0x0000003D);

	delay(MBUS_DELAY);
	mbus_write_message32(0xF1, 0x11111111);
	delay(MBUS_DELAY);

	// Turn off Flash
	FLSv2MBusGPIO_turnOff(FLS_ADDR);
	FLSv2MBusGPIO_rxMsg(); // Rx Payload 
	check_flash_payload (0xA6, 0x000000BB);

	delay(MBUS_DELAY);
	mbus_write_message32(0xF1, 0x33333333);
	delay(MBUS_DELAY);

}

static void operation_flash_erase(void){

	// Set START ADDRESS
	FLSv2MBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x00000000);

	// Set Voltage Clamp
	// Optimal: VTG_TUNE = 0x8, CRT_TUNE=0x3F 
	FLSv2MBusGPIO_writeReg(FLS_ADDR, 0x0A, ((0x8 << 6) | (0x3E << 0 )));

	// Set Terase
	// Default: 0x1AA0
	FLSv2MBusGPIO_setTerase(FLS_ADDR, 0x4AA0);

	// Turn on Flash
	FLSv2MBusGPIO_turnOn(FLS_ADDR);
	FLSv2MBusGPIO_rxMsg(); // Rx Payload 
	check_flash_payload (0xA0, 0x000000B5);

	// Flash Erase Page 2
	FLSv2MBusGPIO_setFlashStartAddr(FLS_ADDR, 0x800); // Should be a multiple of 0x800
	FLSv2MBusGPIO_doEraseFlash(FLS_ADDR);
	FLSv2MBusGPIO_rxMsg(); // Rx Payload 
	check_flash_payload (0xA1, 0x00000055);

	// Flash Erase Page 3
	FLSv2MBusGPIO_setFlashStartAddr(FLS_ADDR, 0x1000); // Should be a multiple of 0x800
	FLSv2MBusGPIO_doEraseFlash(FLS_ADDR);
	FLSv2MBusGPIO_rxMsg(); // Rx Payload 
	check_flash_payload (0xA1, 0x00000055);

	// Flash Erase Page 4
	FLSv2MBusGPIO_setFlashStartAddr(FLS_ADDR, 0x1800); // Should be a multiple of 0x800
	FLSv2MBusGPIO_doEraseFlash(FLS_ADDR);
	FLSv2MBusGPIO_rxMsg(); // Rx Payload 
	check_flash_payload (0xA1, 0x00000055);

	// Flash Erase Page 5
	FLSv2MBusGPIO_setFlashStartAddr(FLS_ADDR, 0x2000); // Should be a multiple of 0x800
	FLSv2MBusGPIO_doEraseFlash(FLS_ADDR);
	FLSv2MBusGPIO_rxMsg(); // Rx Payload 
	check_flash_payload (0xA1, 0x00000055);

	// Turn off Flash
	FLSv2MBusGPIO_turnOff(FLS_ADDR);
	FLSv2MBusGPIO_rxMsg(); // Rx Payload 
	check_flash_payload (0xA2, 0x000000BB);

}


static void operation_flash_read(void){

	delay(MBUS_DELAY);
	mbus_write_message32(0xF2, 0x11111111);
	delay(MBUS_DELAY);

	// Check Flash SRAM after wake-up
	check_flash_sram(0xE2, 10);

	// Turn on Flash
	FLSv2MBusGPIO_turnOn(FLS_ADDR);
	FLSv2MBusGPIO_rxMsg(); // Rx Payload 
	check_flash_payload (0xA7, 0x000000B5);

	delay(MBUS_DELAY);
	mbus_write_message32(0xF2, 0x22222222);
	delay(MBUS_DELAY);

	// Copy Flash to SRAM
	FLSv2MBusGPIO_setFlashStartAddr(FLS_ADDR, 0x800); // Should be a multiple of 0x800
	FLSv2MBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x000);
	FLSv2MBusGPIO_doCopyFlash2SRAM(FLS_ADDR, 0x1FFF); // 4 Pages
	FLSv2MBusGPIO_rxMsg();
	check_flash_payload (0xA8, 0x00000024);

	delay(MBUS_DELAY);
	mbus_write_message32(0xF2, 0x22222222);
	delay(MBUS_DELAY);

	// Turn off Flash
	FLSv2MBusGPIO_turnOff(FLS_ADDR);
	FLSv2MBusGPIO_rxMsg(); // Rx Payload 
	check_flash_payload (0xA9, 0x000000BB);

	// Check Flash SRAM after recovery
	check_flash_sram(0xE3, 10);
}

static void operation_init(void){

    //Enumerate & Initialize Registers
    enumerated = 0xDEADBEEF;
    cyc_num = 0;

	// Disable Halting --> Use manual delays
	set_halt_disable();

    // Set Halt RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    // Set Halt TX --> Use for register write e.g.
    //set_halt_until_mbus_tx();

    //Enumeration
    enumerate(MD_ADDR);
    delay(MBUS_DELAY*2);
    enumerate(RAD_ADDR);
    delay(MBUS_DELAY*2);

	// Initialize MDv3
	initialize_md_reg();

    // Radio Settings --------------------------------------
    radv9_r0.RADIO_TUNE_CURRENT_LIMITER = 0x1F; //Current Limiter 2F = 30uA, 1F = 3uA
    radv9_r0.RADIO_TUNE_FREQ1 = 0x0; //Tune Freq 1
    radv9_r0.RADIO_TUNE_FREQ2 = 0x0; //Tune Freq 2 //0x0,0x0 = 902MHz on Pblr005
    radv9_r0.RADIO_TUNE_TX_TIME = 0x6; //Tune TX Time
  
    mbus_remote_register_write(RAD_ADDR,0,radv9_r0.as_int);
    delay(MBUS_DELAY);

    // FSM data length setups
    radv9_r11.RAD_FSM_H_LEN = 16; // N
    radv9_r11.RAD_FSM_D_LEN = RADIO_DATA_LENGTH-1; // N-1
    radv9_r11.RAD_FSM_C_LEN = 10;
    mbus_remote_register_write(RAD_ADDR,11,radv9_r11.as_int);
    delay(MBUS_DELAY);
  
    // Configure SCRO
    radv9_r1.SCRO_FREQ_DIV = 3;
    radv9_r1.SCRO_AMP_I_LEVEL_SEL = 2; // Default 2
    radv9_r1.SCRO_I_LEVEL_SELB = 0x60; // Default 0x6F
    mbus_remote_register_write(RAD_ADDR,1,radv9_r1.as_int);
    delay(MBUS_DELAY);
  
    // LFSR Seed
    radv9_r12.RAD_FSM_SEED = 4;
    mbus_remote_register_write(RAD_ADDR,12,radv9_r12.as_int);
    delay(MBUS_DELAY);

	// Configure GPIO for FLSv2 Interaction
	FLSv2MBusGPIO_initialization();

	// FLSv2 enumeration
	FLSv2MBusGPIO_enumeration(FLS_ADDR); // Enumeration
	FLSv2MBusGPIO_rxMsg(); // Rx Enumeration Response

	// FLSv2L Voltage Clamp & Timing settings
	FLSv2MBusGPIO_writeReg(FLS_ADDR, 0x4, 0x0C70);
	FLSv2MBusGPIO_writeReg(FLS_ADDR, 0x19, 0x3C4783);

	mbus_write_message32(0xAA, 0x11111111);

	// Initialize Global Variables
    exec_count = 0;
    exec_count_irq = 0;
    MBus_msg_flag = 0;

	md_start_motion = 0;
	md_capture_img = 0;
	md_count = 0;

	WAKEUP_PERIOD_CONT = 2;
	WAKEUP_PERIOD_CONT_INIT = 2; 

	USR_MD_INT_TIME = 12;
	USR_INT_TIME = 30;

	// Go to sleep w/o timer
	operation_sleep_notimer();
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
	uint32_t sample_msg[4] = {0x12345678, 0x00000000, 0x87654321, 0xAAAAAAAA};

    //Initialize Interrupts
    disable_all_irq();

    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        initialization();
    }

//	*REG1 = 0xFFFFFFFF;

//	*REG2 = *REG1;

	mbus_write_message32(0xE1, cyc_num);
	mbus_write_message(0xE2, sample_msg, 4);

	// Member Mbus Register Write/Read
	mbus_remote_register_write(SNS_ADDR,0x0,0xFFFF);

    set_halt_until_mbus_rx();
	mbus_remote_register_read(SNS_ADDR,0x0,0x1);

    set_halt_until_mbus_tx();
	mbus_write_message32(0xE1, *REG1);

	// Set wakeup timer and go to sleep
    cyc_num++;
    set_wakeup_timer(3, 1, 1);
    mbus_sleep_all();

    while(1){  //Never Quit (should not come here.)
        asm("nop;"); 
    }

    return 1;
}
