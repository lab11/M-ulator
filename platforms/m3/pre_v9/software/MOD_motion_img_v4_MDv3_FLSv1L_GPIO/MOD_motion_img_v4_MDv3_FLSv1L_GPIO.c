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
//#include "HRVv1.h"
#include "RADv5.h"
#include "FLSv1_GPIO.h"


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
#define	MBUS_DELAY 200 //Amount of delay between successive messages
#define WAKEUP_DELAY 10000 // 20s
#define WAKEUP_DELAY_FINAL 5000	// Delay for waiting for internal decaps to stabilize after waking up MDSENSOR
#define DELAY_1 5000 // 5000: 0.5s
#define DELAY_0.5 2500 // 5000: 0.5s
#define DELAY_IMG 40000 // 1s

#define START_COL_IDX 0 // in words
#define COLS_TO_READ 39 // in # of words: 39 for full frame, 19 for half


// FLSv1 configurations
//#define FLS_RECORD_LENGTH 0x18FE // In words; # of words stored -2
#define FLS_RECORD_LENGTH 6473 // In words; # of words stored -2; Full image with mbus overhead
//#define FLS_RECORD_LENGTH 0x2 // In words; # of words stored -2
#define FLS_RADIO_LENGTH 50 // In words


// Radio configurations
#define RAD_BIT_DELAY       4     //40      //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 	2000      //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define RAD_PACKET_NUM      1      //How many times identical data will be TXed

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
  
	//volatile uint32_t radio_tx_count;
	//volatile uint32_t radio_tx_option;
	//volatile uint32_t radio_tx_numdata;

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
//Send Radio Data MSB-->LSB
//Bit Delay: Delay between each bit (32-bit data)
//Delays are nop delays (therefore dependent on core speed)
//***************************************************
static void send_radio_data_32b(uint32_t radio_data){
	int32_t i; //loop var
	uint32_t j; //loop var
	for(i=35;i>=0;i--){ //Bit Loop
		if (i>=32) { // 4-bit header
			if ((radio_data>>i)&1) write_mbus_register(RAD_ADDR,0x47,0x1); // dummy message
			else                   write_mbus_register(RAD_ADDR,0x27,0x1);
		}else{ // 32-bit data
			if ((radio_data>>i)&1) write_mbus_register(RAD_ADDR,0x27,0x1);
			else                   write_mbus_register(RAD_ADDR,0x27,0x0);
		}
		//Must clear register
		delay(RAD_BIT_DELAY);
		write_mbus_register(RAD_ADDR,0x27,0x0);
		delay(RAD_BIT_DELAY); //Set delay between sending subsequent bit
	}
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
	delay (MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x1,mdv3_r1.as_int);
	delay (MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x2,mdv3_r2.as_int);
	delay (MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x3,mdv3_r3.as_int);
	delay (MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x4,mdv3_r4.as_int);
	delay (MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x5,mdv3_r5.as_int);
	delay (MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x6,mdv3_r6.as_int);
	delay (MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x7,mdv3_r7.as_int);
	delay (MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x8,mdv3_r8.as_int);
	delay (MBUS_DELAY);
	write_mbus_register(MD_ADDR,0x9,mdv3_r8.as_int);
	delay (MBUS_DELAY);

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
  delay (MBUS_DELAY);

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

}

static void poweroff_array_adc(){

  // Stop ADC Clock
  // 5:13
  mdv3_r5.CLK_EN_ADC = 0;
  write_mbus_register(MD_ADDR,0x5,mdv3_r5.as_int);
  delay (MBUS_DELAY);

  // Assert ADC Wrapper Reset
  // 6:0
  mdv3_r6.RESET_ADC_WRAPPER = 1;
  write_mbus_register(MD_ADDR,0x6,mdv3_r6.as_int);
  delay (MBUS_DELAY);

  // Assert ADC Isolation
  // 7:17
  mdv3_r7.ISOLATE_ADC_WRAPPER = 1;
  write_mbus_register(MD_ADDR,0x7,mdv3_r7.as_int);
  delay (MBUS_DELAY);

  // Assert IMG Presleep 
  // 2:20
  // Assert IMG Sleep
  // 2:19
  mdv3_r2.PRESLEEP_ADC = 1;
  mdv3_r2.SLEEP_ADC = 1;
  write_mbus_register(MD_ADDR,0x2,mdv3_r2.as_int);
  delay (MBUS_DELAY);

}


static void capture_image_single(){

  // Capture Image
  // 0:0
  mdv3_r0.TAKE_IMAGE = 1;
  write_mbus_register(MD_ADDR,0x0,mdv3_r0.as_int);
  delay(100); // about 6ms

  mdv3_r0.TAKE_IMAGE = 0;
  write_mbus_register(MD_ADDR,0x0,mdv3_r0.as_int);

}

static void capture_image_start(){

  // Capture Image
  // 0:0
  mdv3_r0.TAKE_IMAGE = 1;
  write_mbus_register(MD_ADDR,0x0,mdv3_r0.as_int);
  delay(100); // about 6ms

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
    // Decrease 5x division switching threshold
	//set_pmu_sleep_clk_default();
  
	// For PREv9E GPIO Isolation disable >> bits 16, 17, 24
	//*((volatile uint32_t *) 0xA2000008) = 0x0120E608; /* 0000 0001 0010 0000 1110 0110 0000 1000 */
	*((volatile uint32_t *) 0xA2000008) =
		( (0x1 << 24) /* GPIO_ENABLE */
		| (0x0 << 23) /* High Frequency Mode */
		| (0x2 << 20) /* PMUCFG_OSCMID_DIV_F[2:0] */
		| (0x0 << 17) /* PMUCFG_OSCMID_SEL_F[2:0] */
		| (0x0 << 16) /* PMUCFG_ACT_NONOV_SEL */
		| (0x3 << 14) /* ?? */
		| (0x2 << 12) /* CLKX2_SEL_CM[1:0]; Divider 0:/2, 1:/4, 2:/8, 3:/16 */
		| (0x2 << 10) /* CLKX2_SEL_I2C[1:0]; Divider 0:/1, 1:/2, 2:/4, 3:/8 */
		| (0x2 << 8)  /* CLKX2_SEL_RING[1:0] */
		| (0x0 << 7)  /* PMU_FORCE_WAKE */
		| (0x0 << 6)  /* GOC_ONECLK_MODE */
		| (0x0 << 4)  /* GOC_SEL_DLY[1:0] */
		| (0x8 << 0)  /* GOC_SEL[3:0] */
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

	delay (1000);

	md_start_motion = 0;
	md_capture_img = 0;
	md_count = 0;

    // Reset IRQ11VEC
    IRQ11 = 0;
}

uint32_t check_flash_payload (uint8_t mbus_addr, uint32_t expected_payload) {
		if(FLSMBusGPIO_getRxData0() != expected_payload) {
			write_mbus_message(mbus_addr, 0xDEADBEEF);
			delay(MBUS_DELAY);
			delay(MBUS_DELAY);
			write_mbus_message(mbus_addr, FLSMBusGPIO_getRxData0());
    		operation_sleep_notimer();
			while(1);
		}
		return 1;
}

uint32_t check_flash_sram (uint8_t addr_stamp, uint32_t length) {
	uint32_t idx;

	for(idx=0; idx<length; idx++) {

		FLSMBusGPIO_readMem(FLS_ADDR, 0xEE, 0, ((uint32_t) idx) << 2);
		FLSMBusGPIO_rxMsg(); // Rx
		
		write_mbus_message (addr_stamp, FLSMBusGPIO_getRxData0());
		delay(MBUS_DELAY);
		delay(MBUS_DELAY);
	}

	return 1;
}

uint32_t send_radio_flash_sram (uint8_t addr_stamp, uint32_t length) {
	uint32_t idx;
	uint32_t fls_rx_data;

	for(idx=0; idx<length; idx++) {

		FLSMBusGPIO_readMem(FLS_ADDR, 0xEE, 0, ((uint32_t) idx) << 2);
		FLSMBusGPIO_rxMsg(); // Rx
		fls_rx_data = FLSMBusGPIO_getRxData0();
		write_mbus_message(addr_stamp, fls_rx_data);
		delay(MBUS_DELAY);
		write_mbus_message(addr_stamp, idx);
		delay(MBUS_DELAY);
		send_radio_data_32b(fls_rx_data);
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
		FLSMBusGPIO_initialization();

		// FLSv1 enumeration
		FLSMBusGPIO_enumeration(FLS_ADDR); // Enumeration
		FLSMBusGPIO_rxMsg(); // Rx Enumeration Response
		FLSMBusGPIO_setOptTune(FLS_ADDR); // Set Optimum Tuning Bits; Set Tcyc=0x0300 for flash write timing margin
    }else{
		// Configure GPIO for FLSv1 Interaction
		FLSMBusGPIO_initialization();

		// After enumeration, just wake up FLSv1
		FLSMBusGPIO_wakeup();
	}

	if (IRQ11 == 0) { // Flash Erase

		// Set START ADDRESS
		FLSMBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x00000000);

		// Set Voltage Clamp
		// Optimal: VTG_TUNE = 0x8, CRT_TUNE=0x3F 
		FLSMBusGPIO_writeReg(FLS_ADDR, 0x0A, ((0x8 << 6) | (0x3E << 0 )));

		// Set Terase
		// Default: 0x1AA0
		FLSMBusGPIO_setTerase(FLS_ADDR, 0x4AA0);

		// Turn on Flash
		FLSMBusGPIO_turnOnFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA0, 0x00000003);
		FLSMBusGPIO_enableLargeCap(FLS_ADDR);

		// Flash Erase Page 2
		FLSMBusGPIO_setFlashStartAddr(FLS_ADDR, 0x800); // Should be a multiple of 0x800
		FLSMBusGPIO_doEraseFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA1, 0x00000074);

		// Flash Erase Page 3
		FLSMBusGPIO_setFlashStartAddr(FLS_ADDR, 0x1000); // Should be a multiple of 0x800
		FLSMBusGPIO_doEraseFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA1, 0x00000074);

		// Flash Erase Page 4
		FLSMBusGPIO_setFlashStartAddr(FLS_ADDR, 0x1800); // Should be a multiple of 0x800
		FLSMBusGPIO_doEraseFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA1, 0x00000074);

		// Flash Erase Page 5
		FLSMBusGPIO_setFlashStartAddr(FLS_ADDR, 0x2000); // Should be a multiple of 0x800
		FLSMBusGPIO_doEraseFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA1, 0x00000074);

		// Turn off Flash
		FLSMBusGPIO_turnOffFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA2, 0x00000006);
		FLSMBusGPIO_disableLargeCap(FLS_ADDR);

		// Increment the count at IRQ11VEC
		IRQ11 = IRQ11 + 1;

	}else if (IRQ11 == 1) { // Take an Image and Stream into Flash. Write into Flash
		// Check Flash SRAM before image
		check_flash_sram(0xE0, 10);

		// Set Flash Ext Streaming Length
		FLSMBusGPIO_setExtStreamLength(FLS_ADDR, FLS_RECORD_LENGTH);

		// Make Flash FSM's clock frequency faster
		//FLSMBusGPIO_writeReg(FLS_ADDR, 0x0B, 0x00000003); // for "too fast" error
		//FLSMBusGPIO_writeReg(FLS_ADDR, 0x0B, 0x0000001F); // for "timeout" error

		// Config MD
		// Release power gates, isolation, and reset for frame controller
		initialize_md_reg();
		poweron_frame_controller();

		// Set PMU Strength & division threshold
		// PMU_CTRL Register
		// PRCv9 Default: 0x8F770049
		*((volatile uint32_t *) 0xA200000C) = 0x8F772879; // works without any override!
		delay(DELAY_1);

		// Un-power-gate MD
		// Release power gates, isolation, and reset for imager array
		poweron_array_adc();
		delay(DELAY_1);

		// Give a "Go Flash Ext Streaming" command (only the first 31-bit)
		uint32_t fls_stream_short_prefix = ((uint32_t) FLS_ADDR) << 4;
		uint32_t fls_stream_reg_data = (0x05 << 24) | ((0x1 << 15) | (0x6 << 1) | (0x1 << 0));
		FLSMBusGPIO_sendMBus31bit (fls_stream_short_prefix, fls_stream_reg_data);

		// Capture Image
		capture_image_single();

		// Give a "Go Flash Ext Streaming" command (the last 1-bit)
		delay(200); // ~ 15ms
		FLSMBusGPIO_sendMBusLast1bit (fls_stream_short_prefix, fls_stream_reg_data);
		
		// Receive the Payload
		FLSMBusGPIO_rxMsg();
		delay(DELAY_IMG);
		check_flash_payload (0xA3, 0x00000082); // 0x82 (pass), 0x80 (timeout), 0x83 (too fast)

		// Power-gate MD
		poweroff_array_adc();

		// Recover Flash FSM's clock frequency
		//FLSMBusGPIO_writeReg(FLS_ADDR, 0x0B, 0x00000010);

		// Check Flash SRAM after image
		check_flash_sram(0xE1, 10);

		// Turn on Flash
		FLSMBusGPIO_turnOnFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA4, 0x00000003);
		FLSMBusGPIO_enableLargeCap(FLS_ADDR);

		delay(MBUS_DELAY);
		write_mbus_message(0xF1, 0x11111111);
		delay(MBUS_DELAY);

		// Copy SRAM to Flash
		FLSMBusGPIO_setFlashStartAddr(FLS_ADDR, 0x800); // Should be a multiple of 0x800
		FLSMBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x000);
		FLSMBusGPIO_doCopySRAM2Flash(FLS_ADDR, 0x7FE); // 1 Page; 0x800 -2
		FLSMBusGPIO_rxMsg();
		check_flash_payload (0xA5, 0x0000005C);

		delay(MBUS_DELAY);
		write_mbus_message(0xF1, 0x11111111);
		delay(MBUS_DELAY);

		FLSMBusGPIO_setFlashStartAddr(FLS_ADDR, 0x1000); // Should be a multiple of 0x800
		FLSMBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x800);
		FLSMBusGPIO_doCopySRAM2Flash(FLS_ADDR, 0x7FE); // 1 Page; 0x800 -2
		FLSMBusGPIO_rxMsg();
		check_flash_payload (0xA5, 0x0000005C);

		delay(MBUS_DELAY);
		write_mbus_message(0xF1, 0x11111111);
		delay(MBUS_DELAY);

		FLSMBusGPIO_setFlashStartAddr(FLS_ADDR, 0x1800); // Should be a multiple of 0x800
		FLSMBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x1000);
		FLSMBusGPIO_doCopySRAM2Flash(FLS_ADDR, 0x7FE); // 1 Page; 0x800 -2
		FLSMBusGPIO_rxMsg();
		check_flash_payload (0xA5, 0x0000005C);

		delay(MBUS_DELAY);
		write_mbus_message(0xF1, 0x11111111);
		delay(MBUS_DELAY);

		FLSMBusGPIO_setFlashStartAddr(FLS_ADDR, 0x2000); // Should be a multiple of 0x800
		FLSMBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x1800);
		FLSMBusGPIO_doCopySRAM2Flash(FLS_ADDR, 0x7FE); // 1 Page; 0x800 -2
		FLSMBusGPIO_rxMsg();
		check_flash_payload (0xA5, 0x0000005C);

		delay(MBUS_DELAY);
		write_mbus_message(0xF1, 0x22222222);
		delay(MBUS_DELAY);

		// Turn off Flash
		FLSMBusGPIO_turnOffFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA6, 0x00000006);
		FLSMBusGPIO_disableLargeCap(FLS_ADDR);

		delay(MBUS_DELAY);
		write_mbus_message(0xF1, 0x33333333);
		delay(MBUS_DELAY);


		// Increment the count at IRQ11VEC
		IRQ11 = IRQ11 + 1;

	}else if (IRQ11 == 2) { // Read out from Flash

		delay(MBUS_DELAY);
		write_mbus_message(0xF2, 0x11111111);
		delay(MBUS_DELAY);

		// Check Flash SRAM after wake-up
		check_flash_sram(0xE2, 10);

		// Turn on Flash
		FLSMBusGPIO_turnOnFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA7, 0x00000003);
		FLSMBusGPIO_enableLargeCap(FLS_ADDR);

		delay(MBUS_DELAY);
		write_mbus_message(0xF2, 0x22222222);
		delay(MBUS_DELAY);

		// Copy Flash to SRAM
		FLSMBusGPIO_setFlashStartAddr(FLS_ADDR, 0x800); // Should be a multiple of 0x800
		FLSMBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x000);
		FLSMBusGPIO_doCopyFlash2SRAM(FLS_ADDR, 0x7FE); // 1 Page
		FLSMBusGPIO_rxMsg();
		check_flash_payload (0xA8, 0x00000043);

		delay(MBUS_DELAY);
		write_mbus_message(0xF2, 0x22222222);
		delay(MBUS_DELAY);

		FLSMBusGPIO_setFlashStartAddr(FLS_ADDR, 0x1000); // Should be a multiple of 0x800
		FLSMBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x800);
		FLSMBusGPIO_doCopyFlash2SRAM(FLS_ADDR, 0x7FE); // 1 Page
		FLSMBusGPIO_rxMsg();
		check_flash_payload (0xA8, 0x00000043);

		delay(MBUS_DELAY);
		write_mbus_message(0xF2, 0x22222222);
		delay(MBUS_DELAY);

		FLSMBusGPIO_setFlashStartAddr(FLS_ADDR, 0x1800); // Should be a multiple of 0x800
		FLSMBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x1000);
		FLSMBusGPIO_doCopyFlash2SRAM(FLS_ADDR, 0x7FE); // 1 Page
		FLSMBusGPIO_rxMsg();
		check_flash_payload (0xA8, 0x00000043);

		delay(MBUS_DELAY);
		write_mbus_message(0xF2, 0x22222222);
		delay(MBUS_DELAY);

		FLSMBusGPIO_setFlashStartAddr(FLS_ADDR, 0x2000); // Should be a multiple of 0x800
		FLSMBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x1800);
		FLSMBusGPIO_doCopyFlash2SRAM(FLS_ADDR, 0x7FE); // 1 Page
		FLSMBusGPIO_rxMsg();
		check_flash_payload (0xA8, 0x00000043);

		delay(MBUS_DELAY);
		write_mbus_message(0xF2, 0x33333333);
		delay(MBUS_DELAY);

		// Turn off Flash
		FLSMBusGPIO_turnOffFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA9, 0x00000006);
		FLSMBusGPIO_disableLargeCap(FLS_ADDR);

		// Check Flash SRAM after recovery
		check_flash_sram(0xE3, 10);
		
		// Radio out image data stored in flash
		send_radio_data_32b(0xFAFA0000);
		//send_radio_flash_sram(0xE4, 6475); // Full image
		send_radio_flash_sram(0xE4, 100);
		send_radio_data_32b(0xFAFA0000);

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
		//FLSMBusGPIO_sleep();
		//operation_sleep_notimer();
		//while (1);
	}

	// Put FLS back to sleep
	FLSMBusGPIO_sleep();

    // Go to sleep
    operation_sleep_for(SLEEP_DURATION);

    while(1);

    // Should not reach here
    operation_sleep_notimer();

}

