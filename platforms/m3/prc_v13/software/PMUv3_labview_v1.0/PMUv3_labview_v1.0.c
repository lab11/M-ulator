//****************************************************************************************************
//Author:       Yejoong Kim
//Description:  
//              PMUv3 Testing & Debugging
//              Requirements:
//                  PRCv13
//                  PMUv3
//****************************************************************************************************
#include "PRCv13.h"
#include "PRCv13_RF.h"
#include "mbus.h"
#include "PMUv2_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// Stack order  PRC->PMU
#define PMU_ADDR 0x4

// Used as a Read Buffer
#define RDBUF    ((volatile uint32_t *) 0xA0000088)
#define RDBUF_ID 0x22

//***************************************************
// Global variables
//***************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint32_t exec_count;
volatile uint8_t mbus_msg_flag;

volatile prcv13_r0B_t prcv13_r0B = PRCv13_R0B_DEFAULT;

//***************************************************
//Interrupt Handlers
//***************************************************
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
void handler_ext_int_14(void) { *NVIC_ICPR = (0x1 << 14); } // GOC Triggering (Reserved)

//************************************
// PMU Related Functions
//************************************

inline static void mbus_pmu_register_write(uint32_t reg_id, uint32_t reg_data){
    set_halt_until_mbus_rx();
    mbus_remote_register_write(PMU_ADDR,reg_id,reg_data); 
    set_halt_until_mbus_tx();
}

inline static void mbus_pmu_register_read(uint32_t reg_id){
    set_halt_until_mbus_rx();
    mbus_remote_register_write(PMU_ADDR,0x00,reg_id); 
    set_halt_until_mbus_tx();
}

inline static void set_pmu_optimization(){
	// Register 0x17: UPCONV_TRIM_V3_SLEEP
    mbus_pmu_register_write(0x17, 
		( (3 << 14) // [0, 3]  Desired Vout/Vin ratio
		| (0 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (0 << 5)  // [0, 15] Frequency multiplier L (actually L+1)
		| (15) 		// [2, 31] Floor frequency base
	));
	// Register 0x18: UPCONV_TRIM_V3_ACTIVE
    mbus_pmu_register_write(0x18, 
		( (3 << 14) // [0, 3]  Desired Vout/Vin ratio
		| (0 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (2 << 5)  // [2, 15] Frequency multiplier L (actually L+1)
		| (10) 		// [2, 31] Floor frequency base
	));
	// Register 0x19: DOWNCONV_TRIM_V3_SLEEP
    mbus_pmu_register_write(0x19,
		( (0 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (0 << 5)  // [0, 15] Frequency multiplier L (actually L+1)
		| (15) 		// [1, 31] Floor frequency base
	));
	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
    mbus_pmu_register_write(0x1A,
		( (0 << 13) // [1, 1]  Enable main feedback loop
		| (8 << 9)  // [4, 15] Frequency multiplier R
		| (4 << 5)  // [2, 15] Frequency multiplier L (actually L+1)
		| (8) 		// [8, 31] Floor frequency base
	));
	// Register 0x15: SAR_TRIM_v3_SLEEP
    mbus_pmu_register_write(0x15, 
		( (0 << 19) // [0, 1]  Enable PFM even during periodic reset
		| (0 << 18) // [0, 1]  Enable PFM even when Vref is not used as ref
		| (0 << 17) // [0, 1]  Enable PFM
		| (3 << 14) // [3, 7]  Comparator clock division ratio
		| (0 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (0 << 5)  // [0, 15] Frequency multiplier L (actually L+1)
		| (15) 		// [1, 31] Floor frequency base
	));
	// Register 0x16: SAR_TRIM_v3_ACTIVE
    mbus_pmu_register_write(0x16, 
		( (0 << 19) // [0, 1]  Enable PFM even during periodic reset
		| (0 << 18) // [0, 1]  Enable PFM even when Vref is not used as ref
		| (0 << 17) // [0, 1]  Enable PFM
		| (3 << 14) // [3, 7]  Comparator clock division ratio
		| (0 << 13) // [1, 1]  Enable main feedback loop
		| (8 << 9)  // [4, 15] Frequency multiplier R
		| (8 << 5)  // [2, 15] Frequency multiplier L (actually L+1)
		| (15) 		// [8, 31] Floor frequency base
	));
	// Register 0x05: SAR_RATIO_OVERRIDE
    mbus_pmu_register_write(0x05,
		( (0 << 13) // [1, 1]
        | (0 << 12) // [0, 1]
        | (1 << 11) // [0, 1]   Enable override setting [10]
		| (0 << 10) // [0, 1]   Have the converter have the periodic reset
		| (1 << 9)  // [0, 1]   Enable override setting [8]
		| (0 << 8)  // [0, 1]   Switch input / output power rails for upconversion
		| (1 << 7)  // [0, 1]   Enable override setting [6:0]
		| (44) 		// [0, 127] Binary converter's conversion ratio
	));
	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    // If this value is too small, the output voltages, as well as IVBAT, will slightly fluctuate so frequently.
    mbus_pmu_register_write(0x36,
//        (1)         // [500, 16777215] Num of clock cycles the controller waits per each periodic conversion ratio adjustment after being fully turned-on
        (0xFFFFFFFF)         // [500, 16777215] Num of clock cycles the controller waits per each periodic conversion ratio adjustment after being fully turned-on
    );
	// Register 0x37: TICK_WAKEUP_WAIT
    mbus_pmu_register_write(0x37, 2000); // [500, 16777215] Num of clock cycles the controller waits for internal state transition from sleep to wakeup mode
	// Disable PMU ADC measurement in active mode
	// Register 0x3A: PMU_CONTROLLER_STALL_ACTIVE
    mbus_pmu_register_write(0x3A, 
		(( 0 << 0)  // [0, 1] state_sar_scn_on
		| (0 << 1)  // [0, 1] state_wait_for_clock_cycles
		| (0 << 2)  // [0, 1] state_wait_for_time
		| (0 << 3)  // [0, 1] state_sar_scn_reset
		| (0 << 4)  // [0, 1] state_sar_scn_stabilized
		| (0 << 5)  // [0, 1] state_sar_scn_ratio_roughly_adjusted
		| (0 << 6)  // [0, 1] state_clock_supply_switched
		| (0 << 7)  // [0, 1] state_control_supply_switched
		| (0 << 8)  // [0, 1] state_upconverter_on
		| (0 << 9)  // [0, 1] state_upconverter_stabilized
		| (0 << 10) // [0, 1] state_refgen_on
		| (1 << 11) // [0, 1] state_adc_output_ready
		| (0 << 12) // [0, 1] state_adc_adjusted
		| (0 << 13) // [0, 1] state_sar_scn_ratio_adjusted
		| (0 << 14) // [0, 1] state_downconverter_on
		| (0 << 15) // [0, 1] state_downconverter_stabilized
		| (0 << 16) // [0, 1] state_vdd_3p6_turned_on
		| (0 << 17) // [0, 1] state_vdd_1p2_turned_on
		| (0 << 18) // [0, 1] state_vdd_0P6_turned_on
		| (1 << 19) // [0, 1] state_state_horizon
	));
	// Register 0x3B: PMU_CONTROLLER_DESIRED_STATE_SLEEP
	mbus_pmu_register_write(0x3B,
		(( 1 << 0)  // [1, 1] state_sar_scn_on
		| (1 << 1)  // [1, 1] state_wait_for_clock_cycles
		| (1 << 2)  // [1, 1] state_wait_for_time
		| (1 << 3)  // [1, 1] state_sar_scn_reset
		| (1 << 4)  // [1, 1] state_sar_scn_stabilized
		| (1 << 5)  // [1, 1] state_sar_scn_ratio_roughly_adjusted
		| (1 << 6)  // [1, 1] state_clock_supply_switched
		| (1 << 7)  // [1, 1] state_control_supply_switched
		| (1 << 8)  // [1, 1] state_upconverter_on
		| (1 << 9)  // [1, 1] state_upconverter_stabilized
		| (1 << 10) // [1, 1] state_refgen_on
		| (1 << 11) // [1, 1] state_adc_output_ready
		| (0 << 12) // [1, 1] state_adc_adjusted
		| (1 << 13) // [1, 1] state_sar_scn_ratio_adjusted
		| (1 << 14) // [1, 1] state_downconverter_on
		| (1 << 15) // [1, 1] state_downconverter_stabilized
		| (1 << 16) // [1, 1] state_vdd_3p6_turned_on
		| (1 << 17) // [1, 1] state_vdd_1p2_turned_on
		| (1 << 18) // [1, 1] state_vdd_0P6_turned_on
		| (1 << 19) // [1, 1] state_state_horizon
	));
	// Register 0x3C: PMU_CONTROLLER_DESIRED_STATE_ACTIVE
	mbus_pmu_register_write(0x3C,
		(( 1 << 0)  // [1, 1] state_sar_scn_on
		| (1 << 1)  // [1, 1] state_wait_for_clock_cycles
		| (1 << 2)  // [1, 1] state_wait_for_time
		| (1 << 3)  // [1, 1] state_sar_scn_reset
		| (1 << 4)  // [1, 1] state_sar_scn_stabilized
		| (1 << 5)  // [1, 1] state_sar_scn_ratio_roughly_adjusted
		| (1 << 6)  // [1, 1] state_clock_supply_switched
		| (1 << 7)  // [1, 1] state_control_supply_switched
		| (1 << 8)  // [1, 1] state_upconverter_on
		| (1 << 9)  // [1, 1] state_upconverter_stabilized
		| (1 << 10) // [1, 1] state_refgen_on
		| (1 << 11) // [1, 1] state_adc_output_ready
		| (0 << 12) // [1, 1] state_adc_adjusted
		| (0 << 13) // [0, 1] state_sar_scn_ratio_adjusted
		| (1 << 14) // [1, 1] state_downconverter_on
		| (1 << 15) // [1, 1] state_downconverter_stabilized
		| (1 << 16) // [1, 1] state_vdd_3p6_turned_on
		| (1 << 17) // [1, 1] state_vdd_1p2_turned_on
		| (1 << 18) // [1, 1] state_vdd_0P6_turned_on
		| (0 << 19) // [0, 1] state_state_horizon
	)); 
}

//***************************************************
// End of Program Sleep Operation
//***************************************************
void temp_set_wakeup_timer( uint16_t timestamp, uint8_t irq_en, uint8_t reset ){
	uint32_t regval = timestamp;
	if( irq_en ) regval |= 0x30000; // IRQ in Sleep-Only
	else		 regval &= 0x07FFF;
    *REG_WUPT_CONFIG = regval;

	if( reset ) *WUPT_RESET = 0x01;
}

static void operation_sleep(void){
//    mbus_write_message32(0xD2, *REG_WUPT_CONFIG);

    // Go to Sleep
    mbus_sleep_all();
    while(1);
}

static void operation_sleep_notimer(void){
    // Disable Timer
    temp_set_wakeup_timer(0, 0, 0);

    // Go to sleep without timer
    operation_sleep();
}

static void operation_init(void){
  
	// Set CPU & Mbus Clock Speeds
    prcv13_r0B.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
    prcv13_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
	*REG_CLKGEN_TUNE = prcv13_r0B.as_int;
  
    //Enumerate & Initialize Registers
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    mbus_msg_flag = 0;

    //Enumeration
    set_halt_until_mbus_rx();
 	mbus_enumerate(PMU_ADDR);
    set_halt_until_mbus_tx();

    // Adjust PMU configuration
	set_pmu_optimization();

    // Change PMU reply address
    // Write into *RDBUF (REG0x22, 0xA0000088)
    mbus_remote_register_write(PMU_ADDR,0x52,(0x001000 | RDBUF_ID)); 

    // Notifier
    mbus_write_message32(0xEE, 0x1EA7F00D);

    // Go to sleep without timer
    operation_sleep_notimer();
}


//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
  
//    mbus_write_message32(0xD0, *REG_WUPT_CONFIG);

    // Reset Wakeup Timer; This is required for PRCv13
    //temp_set_wakeup_timer(100, 0, 1);

//    mbus_write_message32(0xD1, *REG_WUPT_CONFIG);

    // Initialize Interrupts
    // Only enable register-related interrupts
	enable_reg_irq();
  
    // Config watchdog timer; default: 0x02FFFFFF
    config_timerwd(0xFFFFFFFF);

    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
        // Set up PMU/GOC register in PRC layer (every time)
        operation_init();
    }

    // Increment exec_count
    exec_count++;

    // Send notifier
    mbus_write_message32(0xE0, exec_count);
    mbus_write_message32(0xE0, *REG0);

    // Extract Headers
    uint32_t wakeup_header_2 = (*REG0 & 0x00FF0000) >> 16;  // [23:16]
    uint32_t wakeup_header_1 = (*REG0 & 0x0000FF00) >> 8;   // [15:8]
    uint32_t wakeup_header_0 = (*REG0 & 0x000000FF) >> 0;   // [7:0]

    // wakeup_header 
    // [2] [1] [0] 
    // 00   X   X  : Immediately go into sleep
    // 01  00   X  : Write the main 7 registers (data from REG1 ~ REG7)
    // 01  01  NN  : Write to PMU Register NN (data from REG1)
    // 02  00  NN  : Read from PMU Register NN and put it into RDBUF
    // 02  AA  NN  : Read from PMU Register NN, put it into RDBUF, and send an MBus message going into 'AA'
    // 03  01  NN  : Write to MBus Register NN (data from REG1)
    // 04  00  NN  : Read from MBus Register NN and put it into RDBUF
    // 04  AA  NN  : Read from MBus Register NN, put it into RDBUF, and send an MBus message going into 'AA'
    // EE   X   X  : Be stuck in active
    // FF  AA   X  : Send out the current execution number to MBus Address 'AA'

    // Reset REG0
    *REG0 = 0;
    
    // Send a sleep message (Do nothing)
    if (wakeup_header_2 == 0x00) {
        mbus_write_message32(0xEE, 0xDEADBEEF);
    }
    // PMU Register Write
    else if (wakeup_header_2 == 0x01) {
        // Write the main 7 registers
        if (wakeup_header_1 == 0x00) {
            //---------------------------------------
	        // REG1 : SAR_RATIO_OVERRIDE      (0x05)
	        // REG2 : UPCONV_TRIM_V3_SLEEP    (0x17)
	        // REG3 : SAR_TRIM_v3_SLEEP       (0x15)
	        // REG4 : DOWNCONV_TRIM_V3_SLEEP  (0x19)
	        // REG5 : UPCONV_TRIM_V3_ACTIVE   (0x18)
	        // REG6 : SAR_TRIM_v3_ACTIVE      (0x16)
	        // REG7 : DOWNCONV_TRIM_V3_ACTIVE (0x1A)
            //---------------------------------------
            
	        // Register 0x17: UPCONV_TRIM_V3_SLEEP
            mbus_pmu_register_write(0x17, *REG2);
	        // Register 0x18: UPCONV_TRIM_V3_ACTIVE
            mbus_pmu_register_write(0x18, *REG5);
	        // Register 0x19: DOWNCONV_TRIM_V3_SLEEP
            mbus_pmu_register_write(0x19, *REG4);
	        // Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
            mbus_pmu_register_write(0x1A, *REG7);
	        // Register 0x15: SAR_TRIM_v3_SLEEP
            mbus_pmu_register_write(0x15, *REG3);
	        // Register 0x16: SAR_TRIM_v3_ACTIVE
            mbus_pmu_register_write(0x16, *REG6);
	        // Register 0x05: SAR_RATIO_OVERRIDE
            mbus_pmu_register_write(0x05, *REG1);
        }
        // Write to a specific register
        else if (wakeup_header_1 == 0x01) {
            mbus_pmu_register_write(wakeup_header_0, *REG1);
        }
        // Write to SAR_OVERRIDE and sleep registers
        else if (wakeup_header_1 == 0x02) {
            //---------------------------------------
	        // REG1 : SAR_RATIO_OVERRIDE      (0x05)
	        // REG2 : UPCONV_TRIM_V3_SLEEP    (0x17)
	        // REG3 : SAR_TRIM_v3_SLEEP       (0x15)
	        // REG4 : DOWNCONV_TRIM_V3_SLEEP  (0x19)
            //---------------------------------------
            
	        // Register 0x17: UPCONV_TRIM_V3_SLEEP
            mbus_pmu_register_write(0x17, *REG2);
	        // Register 0x19: DOWNCONV_TRIM_V3_SLEEP
            mbus_pmu_register_write(0x19, *REG4);
	        // Register 0x15: SAR_TRIM_v3_SLEEP
            mbus_pmu_register_write(0x15, *REG3);
	        // Register 0x05: SAR_RATIO_OVERRIDE
            mbus_pmu_register_write(0x05, *REG1);
        }
    }
    // PMU Register Read
    else if (wakeup_header_2 == 0x02) {
        // Read from a specific regsiter
        if (wakeup_header_1 == 0x00) {
            mbus_pmu_register_read(wakeup_header_0);
        }
        // Read from a specific regsiter, then send a MBus message)
        else {
            mbus_pmu_register_read(wakeup_header_0);
            mbus_write_message32(wakeup_header_1, *RDBUF);
        }
    }
    // MBus Register Write
    else if (wakeup_header_2 == 0x03) {
        // Write to a specific register
        if (wakeup_header_1 == 0x01) {
            mbus_remote_register_write(PMU_ADDR, wakeup_header_0, *REG1); 
        }
    }
    // MBus Register Read
    else if (wakeup_header_2 == 0x04) {
        // Read from a specific regsiter
        if (wakeup_header_1 == 0x00) {
            mbus_remote_register_read (PMU_ADDR, wakeup_header_0, RDBUF_ID);
        }
        // Read from a specific regsiter, then send a MBus message)
        else {
            mbus_remote_register_read (PMU_ADDR, wakeup_header_0, RDBUF_ID);
            mbus_write_message32(wakeup_header_1, *RDBUF);
        }
    }
    // Be Stuck in Active
    else if (wakeup_header_2 == 0xEE) {
        while(1);
    }
    // Send out the current execution number
    else if (wakeup_header_2 == 0xFF) {
        mbus_write_message32(wakeup_header_1, exec_count);
    }

    // Notifier
    mbus_write_message32(0xEE, 0x0EA7F00D);

    // Go to indefinite sleep
    operation_sleep_notimer();

    while(1);
}
