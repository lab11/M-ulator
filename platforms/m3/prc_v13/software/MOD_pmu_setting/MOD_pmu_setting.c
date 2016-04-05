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
#define FLS_ADDR 0x7
#define PMU_ADDR 0x8

// Common parameters
#define	MBUS_DELAY 200 //Amount of delay between successive messages; 5-6ms
#define WAKEUP_DELAY 20000 // 0.6s
#define DELAY_1 20000 // 5000: 0.5s
#define DELAY_IMG 40000 // 1s
#define IMG_TIMEOUT_COUNT 5000


//***************************************************
// Global variables
//***************************************************
//Test Declerations
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO

volatile prcv13_r0B_t prcv13_r0B = PRCv13_R0B_DEFAULT;

volatile uint32_t mbus_msg_flag;
volatile uint32_t enumerated;
volatile uint32_t exec_count;

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
    
	// Disable Timer
	set_wakeup_timer(0, 0, 0);

	// Go to sleep without timer
	operation_sleep();

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
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (8) 		// Floor frequency base (0-31)
	));
	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
	// V0P6 Supply Active: need to support up to 50uA
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (1 << 13) // Enable main feedback loop
		| (10 << 9)  // Frequency multiplier R
		| (10 << 5)  // Frequency multiplier L (actually L+1)
		| (31) 		// Floor frequency base (0-31)
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
		| (0 << 5)  // Frequency multiplier L (actually L+1)
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
		| (10 << 9)  // Frequency multiplier R
		| (10 << 5)  // Frequency multiplier L (actually L+1)
		| (31) 		// Floor frequency base (0-31) //16
	));
	delay(MBUS_DELAY);
	// SAR_RATIO_OVERRIDE
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
	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    mbus_remote_register_write(PMU_ADDR,0x36,0x000001);
	delay(MBUS_DELAY);
	// Register 0x37: TICK_WAKEUP_WAIT
    mbus_remote_register_write(PMU_ADDR,0x37,0x0011F4); // Default: 0x1F4
	delay(MBUS_DELAY);

}
static void operation_init(void){
  
    // Set CPU Halt Option as TX --> Use for register write e.g.
    set_halt_until_mbus_tx();

	uint32_t temp_32 = *((volatile uint32_t *) REG_CLKGEN_TUNE);
	mbus_write_message32(0xBB, temp_32);
    prcv13_r0B.GOC_CLK_GEN_SEL_FREQ= 0x7; // Default 0x6
	*((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv13_r0B.as_int;
	temp_32 = *((volatile uint32_t *) REG_CLKGEN_TUNE);
	mbus_write_message32(0xBB, temp_32);

	// Set CPU & Mbus Clock Speeds
    prcv13_r0B.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
    prcv13_r0B.CLK_GEN_RING = 0x3; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_MBC = 0x0; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_CORE = 0x2; // Default 0x3
	*((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv13_r0B.as_int;

    //Enumerate & Initialize Registers
    enumerated = 0xDEADBEEF;
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


	// Reset global variables
	WAKEUP_PERIOD_CONT = 2;
	WAKEUP_PERIOD_CONT_INIT = 2; 
	exec_count = 0;

	// Go to sleep w/o timer
	//operation_sleep_notimer();
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
	//config_timerwd(0xFFFFF);
	disable_timerwd();
	
    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
        operation_init();

    }

	delay(MBUS_DELAY);

	exec_count++;
	
	mbus_write_message32(0xAA, exec_count);

	if (exec_count == 4){
		while(1);
	}

	set_wakeup_timer(3, 0x1, 0x1);
	operation_sleep();

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

