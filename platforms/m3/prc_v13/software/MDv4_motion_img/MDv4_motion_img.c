//*******************************************************************
//Author: 		Gyouho Kim
//Description: 	Code for Motion Detection with MDv4
//				Derived from MDv3_motion_img_v3
//*******************************************************************
#include "PRCv13.h"
#include "PRCv13_RF.h"
#include "mbus.h"
#include "MDv3.h"
#include "SNSv7.h"
#include "HRVv2.h"
#include "RADv9.h"

// Enumeration Sequence: PRC -> MD -> RAD
#define MD_ADDR 0x4           //MDv1 Short Address
#define RAD_ADDR 0x2           //RADIO Short Address
//#define SNS_ADDR 0x4           //SNSv1 Short Address

#define MBUS_DELAY 100
#define WAKEUP_DELAY 5000 // 20s
#define DELAY_1 10000 // 1s
#define DELAY_0P5 5000
#define DELAY_IMG 20000 // 1s

#define START_COL_IDX 0 // in words
#define COLS_TO_READ 39 // in # of words: 39 for full frame, 19 for half


//***************************************************
// Global variables
//***************************************************

volatile uint32_t enumerated;
volatile uint32_t mbus_msg_flag;

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

volatile prcv13_r0B_t prcv13_r0B = PRCv13_R0B_DEFAULT;

//************************************
//Interrupt Handlers
//Must clear pending bit!
//************************************

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
//Internal Functions
//************************************

static void initialize_md_reg(){

	mdv3_r0.INT_TIME = 40;
	mdv3_r0.MD_INT_TIME = 20;
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

	mdv3_r4.SEL_CC  = 7;
	mdv3_r4.SEL_CC_B  = 0;

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

//***************************************************
// End of Program Sleep Operations
//***************************************************
static void operation_sleep(void){

	// Reset IRQ14VEC
	*((volatile uint32_t *) IRQ14VEC) = 0;

	// Reset wakeup timer
	*WUPT_RESET = 0x01;

    // Go to Sleep
    mbus_sleep_all();
    while(1);

}

static void operation_sleep_noirqreset(void){

	// Reset wakeup timer
	*WUPT_RESET = 0x01;

    // Go to Sleep
    mbus_sleep_all();
    while(1);

}

static void operation_sleep_notimer(void){
    
    // Disable Timer
    set_wakeup_timer(0, 0, 0);

    // Go to sleep without timer
    operation_sleep();

}

int main() {
  
	uint32_t first_exec;

	delay(MBUS_DELAY);

	//Check if it is the first execution
	if (enumerated != 0x12345678){
	  first_exec = 1;
	  //Mark execution
	  enumerated = 0x12345678;
	}else{
	  first_exec = 0;
	}

	// Interrupts
    //Clear All Pending Interrupts
    *((volatile uint32_t *) 0xE000E280) = 0xF;
    //Enable Interrupts
    *((volatile uint32_t *) 0xE000E100) = 0xF;

	if (first_exec == 1){

		// Enumeration
		mbus_enumerate(MD_ADDR);
		delay(MBUS_DELAY);
		mbus_enumerate(RAD_ADDR);
		delay(MBUS_DELAY);

		// Set MBUS Clock faster
		// Change GOC_CTRL Register
		// PRCv9 Default: 0x00202903
		//*((volatile uint32_t *) 0xA2000008) = 0x00202603;
		
		// Set CPU & Mbus Clock Speeds
		prcv13_r0B.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
		prcv13_r0B.CLK_GEN_RING = 0x1; // Default 0x1
		prcv13_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
		prcv13_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
		*((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv13_r0B.as_int;
	  
		delay(1000);
  
		// Disable MBus Watchdog Timer
		//*REG_MBUS_WD = 0;
		*((volatile uint32_t *) 0xA000007C) = 0;

		// Initialize MD
		initialize_md_reg();

	} // if first_exec

	//delay(0x10000); // about 3s

	// This is required if this program is used for sleep/wakeup cycling
	clear_md_flag();

	// Initialize
	//initialize_md_reg();

	// Release power gates, isolation, and reset for frame controller
	if (first_exec){
	  poweron_frame_controller();
	}else{
	  poweron_frame_controller_short();
	}

	// Capture 3 images
	poweron_array_adc();
	delay(MBUS_DELAY*20);
	capture_image_single();
	capture_image_single();
	capture_image_single();

	poweroff_array_adc();

	// Start motion detection
	//start_md();

	//clear_md_flag();
	delay(MBUS_DELAY);
	start_md();

	delay(MBUS_DELAY);

	operation_sleep_notimer();

	while(1);

}
