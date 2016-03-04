//****************************************************************************************************
//Author:       Gyouho Kim
//				Yejoong Kim
//              ZhiYoong Foo
//Description:  Derived from Pstack_ondemand_v1.6.c
//              For MDv2 - FLSv1L operation
//****************************************************************************************************
#include "mbus.h"
#include "PRCv9E.h"
#include "MDv3.h"
#include "RADv9.h"
#include "FLSv2L_GPIO.h"


// Sleep Duration
#define SLEEP_DURATION 2

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// uncomment this to only transmit average
//#define TX_AVERAGE

// Stack order  PRC->MD->RAD->HRV
#define MD_ADDR 0x4		// MD Short Address
#define RAD_ADDR 0x5
#define HRV_ADDR 0x6
#define FLS_ADDR 0x7

// Common parameters
#define	MBUS_DELAY 400 // ~6ms; Amount of delay between successive messages
#define WAKEUP_DELAY 40000 // 0.6s
#define DELAY_1 20000 //
//#define DELAY_0.5 2500 // 5000: 0.5s
#define DELAY_IMG 80000 // 1s

#define START_COL_IDX 0 // in words
#define COLS_TO_READ 39 // in # of words: 39 for full frame, 19 for half


// FLSv1 configurations
//#define FLS_RECORD_LENGTH 0x18FE // In words; # of words stored -2
#define FLS_RECORD_LENGTH 6473 // In words; # of words stored -2; Full image with mbus overhead
//#define FLS_RECORD_LENGTH 0x2 // In words; # of words stored -2
#define FLS_CHECK_LENGTH 200 // In words

// Radio configurations
#define RADIO_DATA_LENGTH 24
#define RADIO_PACKET_DELAY 3000
#define RADIO_TIMEOUT_COUNT 500
#define WAKEUP_PERIOD_RADIO_INIT 3

// MD Configurations
#define WAKEUP_PERIOD_CONT 100 // 1: 2-4 sec with PRCv9
#define WAKEUP_PERIOD_CONT_INIT 1   // 0x1E (30): ~1 min with PRCv9

//***************************************************
// Global variables
//***************************************************
	//Test Declerations
	// "static" limits the variables to this file, giving compiler more freedom
	// "volatile" should only be used for MMIO
	volatile uint32_t enumerated;
	volatile uint32_t exec_count;
	volatile uint32_t exec_count_irq;
	volatile uint32_t MBus_msg_flag;
  
	volatile bool radio_ready;
	volatile bool radio_on;

	volatile uint32_t md_count;
	volatile uint8_t md_capture_img;
	volatile uint8_t md_start_motion;

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
// Radio transmission routines for PPM Radio (RADv9)
//***************************************************

static void radio_power_on(){
	// Need to speed up sleep pmu clock
	set_pmu_sleep_clk_fastest();

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
    delay(MBUS_DELAY*3); // At least 20ms required
    
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
	#ifdef DEBUG_MBUS_MSG
		write_mbus_message(0xBB, 0xFAFAFAFA);
	#endif
}


//************************************
// MDv2 Functions
//************************************

static void initialize_md_reg(){

	mdv3_r0.INT_TIME = 5*2;
	mdv3_r0.MD_INT_TIME = 15;
	mdv3_r1.MD_TH = 10;
	mdv3_r1.MD_LOWRES = 1;
	mdv3_r1.MD_LOWRES_B = 0;
	mdv3_r1.MD_FLAG_TOPAD_SEL = 0; // 1: thresholding, 0: no thresholding

	mdv3_r2.MD_RESULTS_MASK = 0x3FF;

	mdv3_r3.SEL_VREF = 0;
	mdv3_r3.SEL_VREFP = 7;
	mdv3_r3.SEL_VBN = 3;
	mdv3_r3.SEL_VBP = 3;
	mdv3_r3.SEL_VB_RAMP = 15;
	mdv3_r3.SEL_RAMP = 1;

	mdv3_r4.SEL_CC  = 3;
	mdv3_r4.SEL_CC_B  = 4;

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

	mdv3_r8.MBUS_REPLY_ADDR_FLAG = 0x17;
	mdv3_r9.MBUS_REPLY_ADDR_DATA = 0x17; // IMG Data return address

	mdv3_r8.MBUS_START_ADDR = 0; // Start column index in words
	mdv3_r8.MBUS_LENGTH_M1 = 39; // Columns to be read; in # of words: 39 for full frame, 19 for half

	//Write Registers
	write_mbus_register(MD_ADDR,0x0,mdv3_r0.as_int);
	delay(MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x1,mdv3_r1.as_int);
	delay(MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x2,mdv3_r2.as_int);
	delay(MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x3,mdv3_r3.as_int);
	delay(MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x4,mdv3_r4.as_int);
	delay(MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x5,mdv3_r5.as_int);
	delay(MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x6,mdv3_r6.as_int);
	delay(MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x7,mdv3_r7.as_int);
	delay(MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x8,mdv3_r8.as_int);
	delay(MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x9,mdv3_r8.as_int);
	delay(MBUS_DELAY);

}

static void poweron_frame_controller(){

  // Release MD Presleep
  // 2:22
  mdv3_r2.PRESLEEP_MD = 0;
  write_mbus_register(MD_ADDR,0x2,mdv3_r2.as_int);
  delay(WAKEUP_DELAY);

  // Release MD Sleep
  // 2:21
  mdv3_r2.SLEEP_MD = 0;
  write_mbus_register(MD_ADDR,0x2,mdv3_r2.as_int);
  delay(WAKEUP_DELAY);

  // Release MD Isolation
  // 7:15
  mdv3_r7.ISOLATE_MD = 0;
  write_mbus_register(MD_ADDR,0x7,mdv3_r7.as_int);
  delay(MBUS_DELAY);
 
  // Release MD Reset
  // 2:23
  mdv3_r2.RESET_MD = 0;
  write_mbus_register(MD_ADDR,0x2,mdv3_r2.as_int);
  delay(MBUS_DELAY);

  // Start MD Clock
  // 5:12
  mdv3_r5.CLK_EN_MD = 1;
  write_mbus_register(MD_ADDR,0x5,mdv3_r5.as_int);
  delay(MBUS_DELAY);

}

static void poweron_frame_controller_short(){

  // Release MD Presleep
  // 2:22
  mdv3_r2.PRESLEEP_MD = 0;
  write_mbus_register(MD_ADDR,0x2,mdv3_r2.as_int);
  delay(MBUS_DELAY);

  // Release MD Sleep
  // 2:21
  mdv3_r2.SLEEP_MD = 0;
  write_mbus_register(MD_ADDR,0x2,mdv3_r2.as_int);
  delay(MBUS_DELAY);

  // Release MD Isolation
  // 7:15
  mdv3_r7.ISOLATE_MD = 0;
  write_mbus_register(MD_ADDR,0x7,mdv3_r7.as_int);
  delay(MBUS_DELAY);
 
  // Release MD Reset
  // 2:23
  mdv3_r2.RESET_MD = 0;
  write_mbus_register(MD_ADDR,0x2,mdv3_r2.as_int);
  delay(MBUS_DELAY);

  // Start MD Clock
  // 5:12
  mdv3_r5.CLK_EN_MD = 1;
  write_mbus_register(MD_ADDR,0x5,mdv3_r5.as_int);
  delay(MBUS_DELAY);

}

static void poweron_array_adc(){

  // Release IMG Presleep 
  // 2:20
  mdv3_r2.PRESLEEP_ADC = 0;
  write_mbus_register(MD_ADDR,0x2,mdv3_r2.as_int);
  delay(WAKEUP_DELAY);

  // Release IMG Sleep
  // 2:19
  mdv3_r2.SLEEP_ADC = 0;
  write_mbus_register(MD_ADDR,0x2,mdv3_r2.as_int);
  delay(WAKEUP_DELAY);

  // Release ADC Isolation
  // 7:17
  mdv3_r7.ISOLATE_ADC_WRAPPER = 0;
  write_mbus_register(MD_ADDR,0x7,mdv3_r7.as_int);
  delay(MBUS_DELAY);

  // Release ADC Wrapper Reset
  // 6:0
  mdv3_r6.RESET_ADC_WRAPPER = 0;
  write_mbus_register(MD_ADDR,0x6,mdv3_r6.as_int);
  delay(MBUS_DELAY);

  // Start ADC Clock
  // 5:13
  mdv3_r5.CLK_EN_ADC = 1;
  write_mbus_register(MD_ADDR,0x5,mdv3_r5.as_int);
  delay(MBUS_DELAY);

}

static void poweroff_array_adc(){

  // Stop ADC Clock
  // 5:13
  mdv3_r5.CLK_EN_ADC = 0;
  write_mbus_register(MD_ADDR,0x5,mdv3_r5.as_int);
  delay(MBUS_DELAY);

  // Assert ADC Wrapper Reset
  // 6:0
  mdv3_r6.RESET_ADC_WRAPPER = 1;
  write_mbus_register(MD_ADDR,0x6,mdv3_r6.as_int);
  delay(MBUS_DELAY);

  // Assert ADC Isolation
  // 7:17
  mdv3_r7.ISOLATE_ADC_WRAPPER = 1;
  write_mbus_register(MD_ADDR,0x7,mdv3_r7.as_int);
  delay(MBUS_DELAY);

  // Assert IMG Presleep 
  // 2:20
  // Assert IMG Sleep
  // 2:19
  mdv3_r2.PRESLEEP_ADC = 1;
  mdv3_r2.SLEEP_ADC = 1;
  write_mbus_register(MD_ADDR,0x2,mdv3_r2.as_int);
  delay(MBUS_DELAY);

}


static void capture_image_single(){

  // Capture Image
  // 0:0
  mdv3_r0.TAKE_IMAGE = 1;
  write_mbus_register(MD_ADDR,0x0,mdv3_r0.as_int);
  delay(MBUS_DELAY*2); //Need ~10ms

  mdv3_r0.TAKE_IMAGE = 0;
  write_mbus_register(MD_ADDR,0x0,mdv3_r0.as_int);

}

static void capture_image_start(){

  // Capture Image
  // 0:0
  mdv3_r0.TAKE_IMAGE = 1;
  write_mbus_register(MD_ADDR,0x0,mdv3_r0.as_int);
  delay(MBUS_DELAY);

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

static void operation_sleep_for(uint16_t num_sleep_cycles){
  // Reset wakeup counter
  // This is required to go back to sleep!!
	set_wakeup_timer (num_sleep_cycles, 1, 0);

  // Go to Sleep
	operation_sleep();
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
	//volatile uint32_t temp_addr;
	//volatile uint32_t temp_data;
	//volatile uint32_t temp_numBit;
  
    // Set PMU Strength & division threshold
    // PMU_CTRL Register
    // PRCv9 Default: 0x8F770049
	*((volatile uint32_t *) 0xA200000C) = 0x4F773879; // works without any override!

    delay(DELAY_1);

	// For PREv9E GPIO Isolation disable >> bits 16, 17, 24
	//*((volatile uint32_t *) 0xA2000008) = 0x0120E608; /* 0000 0001 0010 0000 1110 0110 0000 1000 */
	*((volatile uint32_t *) 0xA2000008) =
		( (0x1 << 24) /* GPIO_ENABLE */
		| (0x0 << 23) /* High Frequency Mode */
		| (0x2 << 20) /* PMUCFG_OSCMID_DIV_F[2:0] */
		| (0x0 << 17) /* PMUCFG_OSCMID_SEL_F[2:0] */
		| (0x0 << 16) /* PMUCFG_ACT_NONOV_SEL */
		| (0x3 << 14) /* ?? */
		| (0x0 << 12) /* CLKX2_SEL_CM[1:0]; Divider 0:/2, 1:/4, 2:/8, 3:/16 */
		| (0x1 << 10) /* CLKX2_SEL_I2C[1:0]; Divider 0:/1, 1:/2, 2:/4, 3:/8 */
		| (0x2 << 8)  /* CLKX2_SEL_RING[1:0] */
		| (0x0 << 7)  /* PMU_FORCE_WAKE */
		| (0x0 << 6)  /* GOC_ONECLK_MODE */
		| (0x0 << 4)  /* GOC_SEL_DLY[1:0] */
		| (0x3 << 0)  /* GOC_SEL[3:0] */
		);

    delay(DELAY_1);
  
    //Enumerate & Initialize Registers
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    exec_count_irq = 0;
    MBus_msg_flag = 0;

    //Enumeration
    enumerate(MD_ADDR);
    delay(MBUS_DELAY*2);
    enumerate(RAD_ADDR);
    delay(MBUS_DELAY*2);

	// Initialize MDv2
	initialize_md_reg();

	delay(MBUS_DELAY*2);

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

	md_start_motion = 0;
	md_capture_img = 0;
	md_count = 0;

	radio_ready = 0;
	radio_on = 0;

    // Reset IRQ11VEC
    IRQ11 = 0;
}

uint32_t check_flash_payload (uint8_t mbus_addr, uint32_t expected_payload) {
		if(FLSv2MBusGPIO_getRxData0() != expected_payload) {
			write_mbus_message(mbus_addr, 0xDEADBEEF);
			delay(MBUS_DELAY);
			delay(MBUS_DELAY);
			write_mbus_message(mbus_addr, FLSv2MBusGPIO_getRxData0());
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
		//write_mbus_message(addr_stamp, fls_rx_data);
		//delay(MBUS_DELAY);
		//write_mbus_message(addr_stamp, idx);
		//delay(MBUS_DELAY);
		send_radio_data_ppm(0,fls_rx_data);
		delay(MBUS_DELAY);
	}

	return 1;
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
	//config_timer( 0, 0, 0, 0, 3000000 );
	config_timer( 0, 0, 0, 0, 0xFFFFFFFF );

    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
        operation_init();

		// Configure GPIO for FLSv1 Interaction
		FLSv2MBusGPIO_initialization();

		// FLSv1 enumeration
		FLSv2MBusGPIO_enumeration(FLS_ADDR); // Enumeration
		FLSv2MBusGPIO_rxMsg(); // Rx Enumeration Response

		write_mbus_message(0xA1, 0x3);

		// Slow down FLSv2L clock 
		FLSv2MBusGPIO_writeReg(FLS_ADDR, 0x18, 
		( (0xC << 2)  /* CLK_RING_SEL[3:0] Default 0xC */
		| (0x1 << 0)  /* CLK_DIV_SEL[1:0] Default 0x1 */
		));

		write_mbus_message(0xA1, 0x5);

    }else{
		// Configure GPIO for FLSv1 Interaction
		FLSv2MBusGPIO_initialization();

		// After enumeration, just wake up FLSv1
		FLSv2MBusGPIO_wakeup();
	}

	if (IRQ11 == 0) { // Flash Erase

		// Set START ADDRESS
		FLSv2MBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x00000000);

		// Voltage Clamp & Timing settings
		FLSv2MBusGPIO_writeReg(FLS_ADDR, 0x0, 0x41205); // Tprog
		FLSv2MBusGPIO_writeReg(FLS_ADDR, 0x4, 0x000500); // Tcyc_prog
		FLSv2MBusGPIO_writeReg(FLS_ADDR, 0x19, 0x3C4303); // Default: 0x3C4103
		
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

		// Increment the count at IRQ11VEC
		IRQ11 = IRQ11 + 1;

	}else if (IRQ11 == 1) { // Take an Image and Stream into Flash. Write into Flash
		// Check Flash SRAM before image
		check_flash_sram(0xE0, 20);

		// Set Flash Ext Streaming Length
		//FLSv2MBusGPIO_setExtStreamLength(FLS_ADDR, FLS_RECORD_LENGTH);
		FLSv2MBusGPIO_configExtStream(FLS_ADDR, 0x1, 0x0, 0xFFFFF);

		// Config MD
		// Release power gates, isolation, and reset for frame controller
		initialize_md_reg();
		poweron_frame_controller();

		// Un-power-gate MD
		// Release power gates, isolation, and reset for imager array
		poweron_array_adc();
		delay(DELAY_1);

		// Give a "Go Flash Ext Streaming" command (only the first 31-bit)
		uint32_t fls_stream_short_prefix = ((uint32_t) FLS_ADDR) << 4;
		uint32_t fls_stream_reg_data = ((0x07 << 24) | (FLS_RECORD_LENGTH << 6) | (0x1 << 5) | (0x6 << 1) | (0x1 << 0));
		FLSv2MBusGPIO_sendMBus31bit (fls_stream_short_prefix, fls_stream_reg_data);

		// Capture Image
		capture_image_single();

		// Give a "Go Flash Ext Streaming" command (the last 1-bit)
		delay(MBUS_DELAY*2); // ~ 15ms
		FLSv2MBusGPIO_sendMBusLast1bit (fls_stream_short_prefix, fls_stream_reg_data);
		
		// Receive the Payload
		FLSv2MBusGPIO_rxMsg();
		delay(DELAY_IMG);
		check_flash_payload (0xA3, 0x000000F9); // 0xF9 (pass), 0xF5 (timeout), 0xF7 (too fast)

		// Power-gate MD
		poweroff_array_adc();

		// Check Flash SRAM after image
		check_flash_sram(0xE1, FLS_CHECK_LENGTH);

		// Turn on Flash
		FLSv2MBusGPIO_turnOn(FLS_ADDR);
		FLSv2MBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA4, 0x000000B5);

		delay(MBUS_DELAY);
		write_mbus_message(0xF1, 0x11111111);
		delay(MBUS_DELAY);

		// Copy SRAM to Flash
		FLSv2MBusGPIO_setFlashStartAddr(FLS_ADDR, 0x800); // Should be a multiple of 0x800
		FLSv2MBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x000);
		FLSv2MBusGPIO_doCopySRAM2Flash(FLS_ADDR, 0x1FFF); // 4 Pages; 0x2000 - 1
		FLSv2MBusGPIO_rxMsg();
		check_flash_payload (0xA5, 0x0000003D);

		delay(MBUS_DELAY);
		write_mbus_message(0xF1, 0x11111111);
		delay(MBUS_DELAY);

		// Turn off Flash
		FLSv2MBusGPIO_turnOff(FLS_ADDR);
		FLSv2MBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA6, 0x000000BB);

		delay(MBUS_DELAY);
		write_mbus_message(0xF1, 0x33333333);
		delay(MBUS_DELAY);

		// Power on radio
		radio_power_on();

		// Increment the count at IRQ11VEC
		IRQ11 = IRQ11 + 1;

	}else if (IRQ11 == 2) { // Read out from Flash

		delay(MBUS_DELAY);
		write_mbus_message(0xF2, 0x11111111);
		delay(MBUS_DELAY);

		// Check Flash SRAM after wake-up
		check_flash_sram(0xE2, FLS_CHECK_LENGTH);

		// Turn on Flash
		FLSv2MBusGPIO_turnOn(FLS_ADDR);
		FLSv2MBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA7, 0x000000B5);

		delay(MBUS_DELAY);
		write_mbus_message(0xF2, 0x22222222);
		delay(MBUS_DELAY);

		// Copy Flash to SRAM
		FLSv2MBusGPIO_setFlashStartAddr(FLS_ADDR, 0x800); // Should be a multiple of 0x800
		FLSv2MBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x000);
		FLSv2MBusGPIO_doCopyFlash2SRAM(FLS_ADDR, 0x1FFF); // 4 Pages
		FLSv2MBusGPIO_rxMsg();
		check_flash_payload (0xA8, 0x00000024);

		delay(MBUS_DELAY);
		write_mbus_message(0xF2, 0x22222222);
		delay(MBUS_DELAY);

		// Turn off Flash
		FLSv2MBusGPIO_turnOff(FLS_ADDR);
		FLSv2MBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA9, 0x000000BB);

		// Check Flash SRAM after recovery
		check_flash_sram(0xE3, FLS_CHECK_LENGTH);
		
		// Radio out image data stored in flash
		send_radio_data_ppm(0,0xFAFA000);
		send_radio_flash_sram(0xE4, 6475); // Full image
		//send_radio_flash_sram(0xE4, 100);
		send_radio_data_ppm(1,0xFAFA000);

		// Increment the count at IRQ11VEC
		IRQ11 = IRQ11 + 1;

	}else if (IRQ11 == 3) { // Everything is done. Keep sending out junk data.


		write_mbus_message(0xAA, 0x12345678);
		delay(MBUS_DELAY);
		write_mbus_message(0xAA, 0x87654321);
		delay(MBUS_DELAY);
		write_mbus_message(0xAA, 0xF0F0);
		delay(MBUS_DELAY);
		write_mbus_message(0xAA, IRQ11);

		// All done: Go to sleep forever
		// Put FLS back to sleep
		IRQ11 = 0;
		//FLSv2MBusGPIO_sleep();
		//operation_sleep_notimer();
		//while (1);
	}

	// Put FLS back to sleep
	FLSv2MBusGPIO_sleep();

    // Go to sleep
    operation_sleep_for(SLEEP_DURATION);

    while(1);

    // Should not reach here
    operation_sleep_notimer();

}

