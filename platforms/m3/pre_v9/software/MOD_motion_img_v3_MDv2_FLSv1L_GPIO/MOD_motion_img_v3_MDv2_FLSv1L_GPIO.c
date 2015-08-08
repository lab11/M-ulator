//****************************************************************************************************
//Author:       Gyouho Kim
//				Yejoong Kim
//              ZhiYoong Foo
//Description:  Derived from Pstack_ondemand_v1.6.c
//              For MDv2 - FLSv1L operation
//****************************************************************************************************
#include "mbus.h"
#include "PRCv9E.h"
//#include "HRVv1.h"
//#include "RADv5.h"
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
//#define RAD_ADDR 0x5
//#define HRV_ADDR 0x6
#define FLS_ADDR 0x7

// Common parameters
#define	MBUS_DELAY 200 //Amount of delay between successive messages
#define WAKEUP_DELAY 10000 // 20s
#define WAKEUP_DELAY_FINAL 5000	// Delay for waiting for internal decaps to stabilize after waking up MDSENSOR
#define DELAY_1 5000 // 5000: 0.5s
#define DELAY_IMG 40000 // 1s


// FLSv1 configurations
#define FLS_RECORD_LENGTH 0x18FE // In words; # of words stored -2
//#define FLS_RECORD_LENGTH 0x000E // In words; # of words stored -2
#define FLS_RADIO_LENGTH 50 // In words

//#define FLS_RECORD_LENGTH 0x10 

// MDv2 configurations
//#define INT_TIME 5
//#define MD_INT_TIME 10
#define MD_TH 10
#define MD_MASK 0x3FF
#define MD_LOWRES 1
#define MD_TOPAD_SEL 0 // 1: thresholding, 0: no thresholding

#define VDD_CC_1P2 1
#define VNW_1P2 1

#define SEL_CC 3
#define SEL_CC_B 4

#define SEL_VREF 0
#define SEL_VREFP 7
#define SEL_VBN 3
#define SEL_VBP 3
#define SEL_VB_RAMP 15
#define SEL_RAMP 1

#define SEL_ADC 1
#define SEL_ADC_B 6
#define SEL_PULSE 1
#define SEL_PULSE_COL 0
#define PULSE_SKIP 0
#define PULSE_SKIP_COL 0
#define TAVG 0

#define SEL_CLK_RING 2
#define SEL_CLK_DIV 4
#define SEL_CLK_RING_4US 0
#define SEL_CLK_DIV_4US 1
#define SEL_CLK_RING_ADC 2 
#define SEL_CLK_DIV_ADC 1
#define SEL_CLK_RING_LC 0
#define SEL_CLK_DIV_LC 1

#define START_ROW_IDX 40
#define END_ROW_IDX 120 // 160

#define ROW_SKIP 0
#define COL_SKIP 0
#define IMG_8BIT 1
#define ROW_IDX_EN 0
#define MD_RETURN_ADDR 0x17

#define START_COL_IDX 0 // in words
#define COLS_TO_READ 39 // in # of words: 39 for full frame, 19 for half

// Radio configurations
#define RAD_BIT_DELAY       14     //40      //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 	2000      //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define RAD_PACKET_NUM      1      //How many times identical data will be TXed

// MD Configurations
#define WAKEUP_PERIOD_CONT 100 // 1: 2-4 sec with PRCv9
#define WAKEUP_PERIOD_CONT_INIT 1   // 0x1E (30): ~1 min with PRCv9
#define	INT_TIME 5
#define MD_INT_TIME 15


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
  
	//static uint32_t radio_tx_count;
	//static uint32_t radio_tx_option;
	//static uint32_t radio_tx_numdata;

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

//************************************
// MDv2 Functions
//************************************

  uint32_t mdreg_0;
  uint32_t mdreg_1;
  uint32_t mdreg_2;
  uint32_t mdreg_3;
  uint32_t mdreg_4;
  uint32_t mdreg_5;
  uint32_t mdreg_6;
  uint32_t mdreg_7;
  uint32_t mdreg_8;

static void initialize_md_reg(){

  mdreg_0 = ((MD_INT_TIME<<13)|(INT_TIME<<(3+1)));
  mdreg_1 = (((!IMG_8BIT)<<16)|(MD_TOPAD_SEL<<12)|(MD_TH<<5)|(!MD_LOWRES<<2)|(MD_LOWRES<<1));
  mdreg_2 = ((0x1F<<19)|(0x3<<12)|(MD_MASK));
  mdreg_3 = ((SEL_RAMP<<18)|(SEL_VB_RAMP<<14)|(SEL_VBP<<12)|(SEL_VBN<<10)|(SEL_VREFP<<7)|(SEL_VREF<<4)|(!VNW_1P2<<3)|(VNW_1P2<<2)|(!VDD_CC_1P2<<1)|(VDD_CC_1P2<<0)); // is this inversion safe??
  mdreg_4 = ((!TAVG<<19)|(TAVG<<18)|(PULSE_SKIP_COL<<17)|(PULSE_SKIP<<16)|(SEL_CC_B<<13)|(SEL_CC<<10)|(SEL_PULSE_COL<<8)|(SEL_PULSE<<6)|(SEL_ADC_B<<3)|(SEL_ADC<<0));
  mdreg_5 = ((SEL_CLK_DIV_LC<<16)|(SEL_CLK_RING_LC<<14)|(SEL_CLK_DIV_ADC<<11)|(SEL_CLK_DIV_4US<<9)|(SEL_CLK_DIV<<6)|(SEL_CLK_RING_ADC<<4)|(SEL_CLK_RING_4US<<2)|(SEL_CLK_RING<<0));
  mdreg_6 = ((ROW_IDX_EN<<21)|(IMG_8BIT<<19)|(COL_SKIP<<18)|(ROW_SKIP<<17)|(END_ROW_IDX<<9)|(START_ROW_IDX<<1)|(0x1<<0));
  mdreg_7 = ((0x7<<15));
  mdreg_8 = ((COLS_TO_READ<<15)|(START_COL_IDX<<8)|(MD_RETURN_ADDR<<0));

  //Write Registers;
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x3,mdreg_3);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x4,mdreg_4);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x5,mdreg_5);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x6,mdreg_6);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x8,mdreg_8);
  delay (MBUS_DELAY);

}

static void poweron_frame_controller(){

  // Release MD Presleep (this also releases reset due to a design bug)
  // 2:22
  mdreg_2 &= ~(1<<22);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay(WAKEUP_DELAY);

  // Release MD Sleep
  // 2:21
  mdreg_2 &= ~(1<<21);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay(WAKEUP_DELAY);

  // Release MD Isolation
  // 7:15
  mdreg_7 &= ~(1<<15);
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);
 
  // Release MD Reset
  // 2:23
  mdreg_2 &= ~(1<<23);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay (MBUS_DELAY);

  // Start MD Clock
  // 5:12
  mdreg_5 |= (1<<12);
  write_mbus_register(MD_ADDR,0x5,mdreg_5);
  delay (MBUS_DELAY);

}

static void poweron_array_adc(){

  // Release IMG Presleep 
  // 2:20
  mdreg_2 &= ~(1<<20);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay(WAKEUP_DELAY);

  // Release IMG Sleep
  // 2:19
  mdreg_2 &= ~(1<<19);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay(WAKEUP_DELAY);

  // Release ADC Isolation
  // 7:17
  mdreg_7 &= ~(1<<17);
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);

  // Release ADC Wrapper Reset
  // 6:0
  mdreg_6 &= ~(1<<0);
  write_mbus_register(MD_ADDR,0x6,mdreg_6);
  delay (MBUS_DELAY);

  // Start ADC Clock
  // 5:13
  mdreg_5 |= (1<<13);
  write_mbus_register(MD_ADDR,0x5,mdreg_5);
  delay (MBUS_DELAY);

}

static void poweroff_array_adc(){

  // Stop ADC Clock
  // 5:13
  mdreg_5 &= ~(1<<13);
  write_mbus_register(MD_ADDR,0x5,mdreg_5);
  delay (MBUS_DELAY);

  // Assert ADC Wrapper Reset
  // 6:0
  mdreg_6 |= 1<<0;
  write_mbus_register(MD_ADDR,0x6,mdreg_6);
  delay (MBUS_DELAY);

  // Assert ADC Isolation
  // 7:17
  mdreg_7 |= 1<<17;
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);

  // Assert IMG Presleep 
  // 2:20
  mdreg_2 |= 1<<20;

  // Assert IMG Sleep
  // 2:19
  mdreg_2 |= 1<<19;
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay (MBUS_DELAY);

}


static void capture_image_single(){

  // Capture Image
  // 0:0
  mdreg_0 |= (1<<0);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay(0x80); // about 6ms

  mdreg_0 &= ~(1<<0);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);

//  delay(DELAY_IMG); // about 1s

}

static void capture_image_start(){

  // Capture Image
  // 0:0
  mdreg_0 |= (1<<0);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay(0x80); // about 6ms

}

static void start_md(){

  // Optionally release MD GPIO Isolation
  // 7:16
  mdreg_7 &= ~(1<<16);
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);
  delay(DELAY_1); // about 0.5s

  // Start MD
  // 0:1
  mdreg_0 |= (1<<1);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);
  delay(DELAY_1); // about 0.5s

  mdreg_0 &= ~(1<<1);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);

  delay(DELAY_1); // about 0.5s

  // Enable MD Flag
  // 1:3
  mdreg_1 |= (1<<3);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);

}

static void clear_md_flag(){

  // Stop MD
  // 0:2
  mdreg_0 |= (1<<2);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);
  delay (0x80); // about 6ms

  mdreg_0 &= ~(1<<2);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);

  // Clear MD Flag
  // 1:4
  mdreg_1 |= (1<<4);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);
  delay (0x80); // about 6ms
  
  mdreg_1 &= ~(1<<4);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);

  // Disable MD Flag
  // 1:3
  mdreg_1 &= ~(1<<3);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);

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
	set_wakeup_timer (num_sleep_cycles, 1, 1);

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
	//volatile uint32_t temp_addr;
	//volatile uint32_t temp_data;
	volatile uint32_t temp_numBit;
  
    // Set PMU Strength & division threshold
    // PMU_CTRL Register
    // PRCv9 Default: 0x8F770049
    // Decrease 5x division switching threshold
	set_pmu_sleep_clk_default();
  
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
		| (0x1 << 10) /* CLKX2_SEL_I2C[1:0]; Divider 0:/1, 1:/2, 2:/4, 3:/8 */
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
    }

	// Configure FLSv1 GPIO
	FLSMBusGPIO_initialization();

	if (IRQ11 == 0) { // if it is the first time FLSv1 wake-up
		FLSMBusGPIO_enumeration(FLS_ADDR); // Enumeration
		FLSMBusGPIO_rxMsg(); // Rx Enumeration Response
		FLSMBusGPIO_setOptTune(FLS_ADDR); // Set Optimum Tuning Bits
	}
	else { // From the 2nd cycle, just wake up FLSv1
		FLSMBusGPIO_wakeup();
	}

	// Send IRQ11 Number
	write_mbus_message(0xCC, (uint32_t) IRQ11);
	delay(MBUS_DELAY); delay(MBUS_DELAY);

	if (IRQ11 == 1) { // Flash Erase
		// Set START ADDRESS
		FLSMBusGPIO_setFlashStartAddr(FLS_ADDR, 0x00000800); // Should be a multiple of 0x800
		FLSMBusGPIO_setSRAMStartAddr(FLS_ADDR, 0x00000000);

		// Turn on Flash
		FLSMBusGPIO_turnOnFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA0, 0x00000003);
		FLSMBusGPIO_enableLargeCap(FLS_ADDR);

		// Flash Erase
		FLSMBusGPIO_doEraseFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA1, 0x00000074);

		// Turn off Flash
		FLSMBusGPIO_turnOffFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA2, 0x00000006);
		FLSMBusGPIO_disableLargeCap(FLS_ADDR);
	}
	else if (IRQ11 == 2) { // Take an Image and Stream into Flash. Write into Flash
		// Check Flash SRAM before image
		check_flash_sram(0xE0, 10);

		// Set Flash Ext Streaming Length
		FLSMBusGPIO_setExtStreamLength(FLS_ADDR, FLS_RECORD_LENGTH);

		// Make Flash FSM's clock frequency faster
		FLSMBusGPIO_writeReg(FLS_ADDR, 0x0B, 0x00000003);

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
		//delay(2000); // ~ 100ms?
		FLSMBusGPIO_sendMBusLast1bit (fls_stream_short_prefix, fls_stream_reg_data);
		
		// Receive the Payload
		FLSMBusGPIO_rxMsg();
		delay(DELAY_IMG);
		check_flash_payload (0xA3, 0x00000082); // 0x82 (pass), 0x80 (timeout), 0x83 (too fast)

		// Power-gate MD
		poweroff_array_adc();

		// Recover Flash FSM's clock frequency
		FLSMBusGPIO_writeReg(FLS_ADDR, 0x0B, 0x00000010);

		// Check Flash SRAM after image
		check_flash_sram(0xE1, 10);

		// Turn on Flash
		FLSMBusGPIO_turnOnFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA4, 0x00000003);
		FLSMBusGPIO_enableLargeCap(FLS_ADDR);

		// Copy SRAM to Flash
		FLSMBusGPIO_doCopySRAM2Flash(FLS_ADDR, 0x7FE);
		FLSMBusGPIO_rxMsg();
		check_flash_payload (0xA5, 0x0000005C);

		// Turn off Flash
		FLSMBusGPIO_turnOffFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA6, 0x00000006);
		FLSMBusGPIO_disableLargeCap(FLS_ADDR);
	}
	else if (IRQ11 == 3) { // Read out from Flash
		// Check Flash SRAM after wake-up
		check_flash_sram(0xE2, 10);

		// Turn on Flash
		FLSMBusGPIO_turnOnFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA7, 0x00000003);
		FLSMBusGPIO_enableLargeCap(FLS_ADDR);

		// Copy Flash to SRAM
		FLSMBusGPIO_doCopyFlash2SRAM(FLS_ADDR, 0x7FE);
		FLSMBusGPIO_rxMsg();
		check_flash_payload (0xA8, 0x00000043);

		// Turn off Flash
		FLSMBusGPIO_turnOffFlash(FLS_ADDR);
		FLSMBusGPIO_rxMsg(); // Rx Payload 
		check_flash_payload (0xA9, 0x00000006);
		FLSMBusGPIO_disableLargeCap(FLS_ADDR);

		// Check Flash SRAM after recovery
		check_flash_sram(0xE3, 10);
	}
	else if (IRQ11 == 4) { // Everything is done. Keep sending out junk data.
			write_mbus_message(0xAA, 0x12345678);
			delay(MBUS_DELAY);
			delay(MBUS_DELAY);
			write_mbus_message(0xAA, 0x87654321);
			delay(MBUS_DELAY);
			delay(MBUS_DELAY);
			write_mbus_message(0xAA, 0xF0F0);
			delay(MBUS_DELAY);
			delay(MBUS_DELAY);
	  		while (1);
	}

	// Increment the count at IRQ11VEC
	IRQ11 = IRQ11 + 1;

	// Put FLS back to sleep
	FLSMBusGPIO_sleep();

    // Go to sleep
    operation_sleep_for(SLEEP_DURATION);

    while(1);

    // Should not reach here
    operation_sleep_notimer();

}

