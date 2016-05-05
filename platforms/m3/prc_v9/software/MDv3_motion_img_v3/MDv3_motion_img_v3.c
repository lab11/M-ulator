//*******************************************************************
//Author: 		Gyouho Kim, ZhiYoong Foo
//Description: 	Code for Motion Detection with MDv3
//				v3: faster imaging
//*******************************************************************
#include "mbus.h"
#include "PRCv9.h"
#include "SNSv3.h"
#include "HRVv1.h"
#include "RADv5.h"
#include "MDv3.h"

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

	uint32_t enumerated;
	uint32_t MBus_msg_flag;

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

//************************************
//Interrupt Handlers
//Must clear pending bit!
//************************************
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

	
//************************************
//Internal Functions
//************************************

static void initialize_md_reg(){

/*
	mdv3_r0 = MDv3_R0_DEFAULT;
	mdv3_r1 = MDv3_R1_DEFAULT;
	mdv3_r2 = MDv3_R2_DEFAULT;
	mdv3_r3 = MDv3_R3_DEFAULT;
	mdv3_r4 = MDv3_R4_DEFAULT;
	mdv3_r5 = MDv3_R5_DEFAULT;
	mdv3_r6 = MDv3_R6_DEFAULT;
	mdv3_r7 = MDv3_R7_DEFAULT;
	mdv3_r8 = MDv3_R8_DEFAULT;
	mdv3_r9 = MDv3_R9_DEFAULT;
*/
	mdv3_r0.INT_TIME = 50;
	mdv3_r0.MD_INT_TIME = 25;
	mdv3_r1.MD_TH = 2;
	mdv3_r1.MD_LOWRES = 0;
	mdv3_r1.MD_LOWRES_B = 1;
	mdv3_r1.MD_FLAG_TOPAD_SEL = 1; // 1: thresholding, 0: no thresholding

	mdv3_r2.MD_RESULTS_MASK = 0x3FF;

	mdv3_r3.VDD_CC_ENB_0P6 = 0;
	mdv3_r3.VDD_CC_ENB_1P2 = 1;
	mdv3_r3.SEL_VREF = 0;
	mdv3_r3.SEL_VREFP = 7;
	mdv3_r3.SEL_VBN = 3;
	mdv3_r3.SEL_VBP = 3;
	mdv3_r3.SEL_VB_RAMP = 15;
	mdv3_r3.SEL_RAMP = 5;

	mdv3_r4.SEL_CC  = 4;
	mdv3_r4.SEL_CC_B  = 3;

	mdv3_r5.SEL_CLK_RING = 2;
	mdv3_r5.SEL_CLK_DIV = 3;
	mdv3_r5.SEL_CLK_RING_4US = 0;
	mdv3_r5.SEL_CLK_DIV_4US = 1;
	mdv3_r5.SEL_CLK_RING_ADC = 0; 
	mdv3_r5.SEL_CLK_DIV_ADC = 1;
	mdv3_r5.SEL_CLK_RING_LC = 0;
	mdv3_r5.SEL_CLK_DIV_LC = 0;

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
	write_mbus_register(MD_ADDR,0x9,mdv3_r9.as_int);
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

static void start_md(){

  // Optionally release MD GPIO Isolation
  // 7:16
  //mdv3_r7.ISOLATE_GPIO = 0;
  //write_mbus_register(MD_ADDR,0x7,mdv3_r7.as_int);
  //delay (MBUS_DELAY);
  //delay(DELAY_500ms); // about 0.5s

  // Start MD
  // 0:1
  mdv3_r0.START_MD = 1;
  write_mbus_register(MD_ADDR,0x0,mdv3_r0.as_int);
  delay(MBUS_DELAY*10);

  mdv3_r0.START_MD = 0;
  write_mbus_register(MD_ADDR,0x0,mdv3_r0.as_int);
  delay(MBUS_DELAY);

  delay(DELAY_1); // about 0.5s
  delay(DELAY_1); // about 0.5s

  // Enable MD Flag
  // 1:3
  mdv3_r1.MD_TH_EN = 1;
  write_mbus_register(MD_ADDR,0x1,mdv3_r1.as_int);
  delay(MBUS_DELAY);

}

static void clear_md_flag(){

  // Stop MD
  // 0:2
  mdv3_r0.STOP_MD = 1;
  write_mbus_register(MD_ADDR,0x0,mdv3_r0.as_int);
  delay(MBUS_DELAY*4); // need ~10ms

  mdv3_r0.STOP_MD = 0;
  write_mbus_register(MD_ADDR,0x0,mdv3_r0.as_int);
  delay(MBUS_DELAY);

  // Clear MD Flag
  // 1:4
  mdv3_r1.MD_TH_CLEAR = 1;
  write_mbus_register(MD_ADDR,0x1,mdv3_r1.as_int);
  delay(MBUS_DELAY*4); // need ~10ms
  
  mdv3_r1.MD_TH_CLEAR = 0;
  write_mbus_register(MD_ADDR,0x1,mdv3_r1.as_int);
  delay(MBUS_DELAY);

  // Disable MD Flag
  // 1:3
  mdv3_r1.MD_TH_EN = 0;
  write_mbus_register(MD_ADDR,0x1,mdv3_r1.as_int);
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

  // Release ADC Wrapper Reset
  // 6:0
  mdv3_r6.RESET_ADC_WRAPPER = 0;
  write_mbus_register(MD_ADDR,0x6,mdv3_r6.as_int);
  delay (MBUS_DELAY);

  // Start ADC Clock
  // 5:13
  mdv3_r5.CLK_EN_ADC = 1;
  write_mbus_register(MD_ADDR,0x5,mdv3_r5.as_int);
  delay (MBUS_DELAY);

  // New in v3
  // Release ADC Isolation
  // 7:17
  //mdv3_r7.ISOLATE_ADC_WRAPPER = 0;
  //write_mbus_register(MD_ADDR,0x7,mdv3_r7.as_int);
  //delay (MBUS_DELAY);

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
  delay(MBUS_DELAY*4);

  mdv3_r0.TAKE_IMAGE = 0;
  write_mbus_register(MD_ADDR,0x0,mdv3_r0.as_int);

  // New in v3
  // Release ADC Isolation
  // 7:17
  mdv3_r7.ISOLATE_ADC_WRAPPER = 0;
  write_mbus_register(MD_ADDR,0x7,mdv3_r7.as_int);
  delay (MBUS_DELAY);

  delay(DELAY_IMG); 

}

static void capture_image_start(){

  // Capture Image
  // 0:0
  mdv3_r0.TAKE_IMAGE = 1;
  write_mbus_register(MD_ADDR,0x0,mdv3_r0.as_int);
  delay(MBUS_DELAY);

}

//***************************************************
// End of Program Sleep Operations
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
		enumerate(MD_ADDR);
		delay(MBUS_DELAY);
		enumerate(RAD_ADDR);
		delay(MBUS_DELAY);
		
		// Set PMU Strength & division threshold
		// Change PMU_CTRL Register
		// PRCv9 Default: 0x8F770049
		//*((volatile uint32_t *) 0xA200000C) = 0x8F770079;
		*((volatile uint32_t *) 0xA200000C) = 0x8F772879; // works without any override!
	  
		delay(DELAY_1);
	  
		// Set MBUS Clock faster
		// Change GOC_CTRL Register
		// PRCv9 Default: 0x00202903
		// 0x00A02932 = Original
		// 0x00A02332 = Fastest MBUS clk
		//*((volatile uint32_t *) 0xA2000008) = 0x00A02332;
		*((volatile uint32_t *) 0xA2000008) = 0x00202603;
		
		delay(DELAY_1);

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
