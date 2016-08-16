//*******************************************************************
//Author: Gyouho Kim 
//Description: Developed during PRCv13 tape-out for verification
//*******************************************************************
#include "PRCv13.h"
#include "PRCv13_RF.h"
#include "PMUv2_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define PMU_ADDR    0x3

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t exec_count;
volatile uint32_t adc_offset;
volatile uint8_t mbus_msg_flag;

volatile prcv13_r0B_t prcv13_r0B = PRCv13_R0B_DEFAULT;

#define	MBUS_DELAY 100 //Amount of delay between successive messages; 5-6ms

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

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
inline static void set_pmu_sleep_clk_init(){
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (4) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// The first register write to PMU needs to be repeated
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (4) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (10) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x19: DOWNCONV_TRIM_V3_SLEEP
    mbus_remote_register_write(PMU_ADDR,0x19,
		( (1 << 13) // Enable main feedback loop
		| (2 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (2) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (1 << 13) // Enable main feedback loop
		| (8 << 9)  // Frequency multiplier R
		| (4 << 5)  // Frequency multiplier L (actually L+1)
		| (8) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x15: SAR_TRIM_v3_SLEEP
    mbus_remote_register_write(PMU_ADDR,0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (1 << 13) // Enable main feedback loop
		| (2 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (6) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x16: SAR_TRIM_v3_ACTIVE
    mbus_remote_register_write(PMU_ADDR,0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (1 << 13) // Enable main feedback loop
		| (8 << 9)  // Frequency multiplier R
		| (8 << 5)  // Frequency multiplier L (actually L+1)
		| (15) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// SAR_RATIO_OVERRIDE
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (44) 		// Binary converter's conversion ratio (7'h00)
	));
	delay(MBUS_DELAY);
	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    mbus_remote_register_write(PMU_ADDR,0x36,0x000001);
	delay(MBUS_DELAY);
}

void initialization (void) {

	// Set CPU & Mbus Clock Speeds
    prcv13_r0B.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
    prcv13_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
	*((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv13_r0B.as_int;

    //Enumerate & Initialize Registers
    enumerated = 0xDEADBEEF;
    exec_count = 0;

    // Set Halt
    //set_halt_until_mbus_rx();
    set_halt_until_mbus_tx();

    // Enumeration
    mbus_enumerate(PMU_ADDR);
	delay(MBUS_DELAY);

	set_pmu_sleep_clk_init();

	// Disable ADC measurement in active mode
	// PMU_CONTROLLER_STALL_ACTIVE
    mbus_remote_register_write(PMU_ADDR,0x3A, 
		( (1 << 19) // ignore state_horizon; default 1
		| (1 << 11) // ignore adc_output_ready; default 0
	));
    delay(MBUS_DELAY);
	// Disable ADC offset measurement
    mbus_remote_register_write(PMU_ADDR,0x3B,
		((  1 << 0) //state_sar_scn_on
		| (1 << 1) //state_wait_for_clock_cycles
		| (1 << 2) //state_wait_for_time
		| (1 << 3) //state_sar_scn_reset
		| (1 << 4) //state_sar_scn_stabilized
		| (1 << 5) //state_sar_scn_ratio_roughly_adjusted
		| (1 << 6) //state_clock_supply_switched
		| (1 << 7) //state_control_supply_switched
		| (1 << 8) //state_upconverter_on
		| (1 << 9) //state_upconverter_stabilized
		| (1 << 10) //state_refgen_on
		| (1 << 11) //state_adc_output_ready
		| (0 << 12) //state_adc_adjusted
		| (1 << 13) //state_sar_scn_ratio_adjusted
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (1 << 19) //state_state_horizon
	));
    delay(MBUS_DELAY);

}

void operation_pmu_adc(void) {

	uint32_t read_data;
	uint32_t count;

	for(count=0; count<1; count++){
		delay(MBUS_DELAY);
		// PMUv2 register read is handled differently
		mbus_remote_register_write(PMU_ADDR,0x00,3);
		delay(MBUS_DELAY);
		read_data = *((volatile uint32_t *) REG0);
		mbus_write_message32(0xA0+count, read_data & 0xFF);
		delay(MBUS_DELAY*10);
	}

	// Manually reset ADC
	// PMU_CONTROLLER_DESIRED_STATE
    mbus_remote_register_write(PMU_ADDR,0x3B,
		((  1 << 0) //state_sar_scn_on
		| (1 << 1) //state_wait_for_clock_cycles
		| (1 << 2) //state_wait_for_time
		| (1 << 3) //state_sar_scn_reset
		| (1 << 4) //state_sar_scn_stabilized
		| (1 << 5) //state_sar_scn_ratio_roughly_adjusted
		| (1 << 6) //state_clock_supply_switched
		| (1 << 7) //state_control_supply_switched
		| (1 << 8) //state_upconverter_on
		| (1 << 9) //state_upconverter_stabilized
		| (1 << 10) //state_refgen_on
		| (0 << 11) //state_adc_output_ready
		| (0 << 12) //state_adc_adjusted
		| (1 << 13) //state_sar_scn_ratio_adjusted
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (1 << 19) //state_state_horizon
	));
	delay(MBUS_DELAY*10);

	// Need to poll state_adc_output_ready == 1 but skip for now
    mbus_remote_register_write(PMU_ADDR,0x3B,
		((  1 << 0) //state_sar_scn_on
		| (1 << 1) //state_wait_for_clock_cycles
		| (1 << 2) //state_wait_for_time
		| (1 << 3) //state_sar_scn_reset
		| (1 << 4) //state_sar_scn_stabilized
		| (1 << 5) //state_sar_scn_ratio_roughly_adjusted
		| (1 << 6) //state_clock_supply_switched
		| (1 << 7) //state_control_supply_switched
		| (1 << 8) //state_upconverter_on
		| (1 << 9) //state_upconverter_stabilized
		| (1 << 10) //state_refgen_on
		| (1 << 11) //state_adc_output_ready
		| (0 << 12) //state_adc_adjusted
		| (1 << 13) //state_sar_scn_ratio_adjusted
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (1 << 19) //state_state_horizon
	));

	delay(MBUS_DELAY);

	set_wakeup_timer(10, 0x1, 0x1);
	operation_sleep_noirqreset();
}

void operation_pmu_adc_complicated(void) {

    /////////////////////
    //ADC Testing
    ////////////////////

    delay(MBUS_DELAY*10);
    //0. Send replies to nowhere
    //mbus_remote_register_write(PMU_ADDR,0x52,0x7000);
    
    //1. Turn PMU_CONTROLLER_DESIRED_STATE->sar_scn_ratio_adjusted 0
    //PMU_CONTROLLER_DESIRED_STATE
    mbus_remote_register_write(PMU_ADDR,0x3B,
							   ((  0x1 << 0) //state_sar_scn_on
								| (0x1 << 1) //state_wait_for_clock_cycles
								| (0x1 << 2) //state_wait_for_time
								| (0x1 << 3) //state_sar_scn_reset
								| (0x1 << 4) //state_sar_scn_stabilized
								| (0x1 << 5) //state_sar_scn_ratio_roughly_adjusted
								| (0x1 << 6) //state_clock_supply_switched
								| (0x1 << 7) //state_control_supply_switched
								| (0x1 << 8) //state_upconverter_on
								| (0x1 << 9) //state_upconverter_stabilized
								| (0x1 <<10) //state_refgen_on
								| (0x1 <<11) //state_adc_output_ready
								| (0x1 <<12) //state_adc_adjusted
								| (0x0 <<13) //state_sar_scn_ratio_adjusted
								| (0x1 <<14) //state_downconverter_on
								| (0x1 <<15) //state_downconverter_stabilized
								| (0x1 <<16) //state_vdd_3p6_turned_on
								| (0x1 <<17) //state_vdd_1p2_turned_on
								| (0x1 <<18) //state_vdd_0P6_turned_on
								| (0x1 <<19) //state_state_horizon
								));
    delay(MBUS_DELAY);

    //Check PMU_CONTROLLER_STATE_MAIN
    mbus_remote_register_write(PMU_ADDR,0x00,0x3C);
    delay(MBUS_DELAY);
	
    //2. Override PMU_ADC_CONFIG_OVERRIDE
    mbus_remote_register_write(PMU_ADDR,0x02,
							   ((  0x00 << 0) //Offset cancelation bits
								| (0x0  << 7) // If ADC uses CDAC LSB
								| (0x80 << 8) // If ADC uses corresnpoding CDAC bits
								| (0x0  <<16) // ADC Ref Voltage (0: VREF, 1: VBAT)
								| (0x0  <<17) // ADC Sampling Voltage (0: VREF, 1: VBAT)
								| (0x1  <<23) // Enables Override Settings
								));
    delay(MBUS_DELAY);
    
    //3. Turn PMU_CONTROLLER_DESIRED_STATE->state_adc_output_ready = 0
    //PMU_CONTROLLER_DESIRED_STATE
    mbus_remote_register_write(PMU_ADDR,0x3B,
							   ((  0x1 << 0) //state_sar_scn_on
								| (0x1 << 1) //state_wait_for_clock_cycles
								| (0x1 << 2) //state_wait_for_time
								| (0x1 << 3) //state_sar_scn_reset
								| (0x1 << 4) //state_sar_scn_stabilized
								| (0x1 << 5) //state_sar_scn_ratio_roughly_adjusted
								| (0x1 << 6) //state_clock_supply_switched
								| (0x1 << 7) //state_control_supply_switched
								| (0x1 << 8) //state_upconverter_on
								| (0x1 << 9) //state_upconverter_stabilized
								| (0x1 <<10) //state_refgen_on
								| (0x0 <<11) //state_adc_output_ready
								| (0x1 <<12) //state_adc_adjusted
								| (0x0 <<13) //state_sar_scn_ratio_adjusted
								| (0x1 <<14) //state_downconverter_on
								| (0x1 <<15) //state_downconverter_stabilized
								| (0x1 <<16) //state_vdd_3p6_turned_on
								| (0x1 <<17) //state_vdd_1p2_turned_on
								| (0x1 <<18) //state_vdd_0P6_turned_on
								| (0x1 <<19) //state_state_horizon
								));
    delay(MBUS_DELAY);
	
    //Poll PMU_CONTROLLER_STATE_MAIN
    mbus_remote_register_write(PMU_ADDR,0x00,0x3C);
    delay(MBUS_DELAY);

    //4. Turn PMU_CONTROLLER_DESIRED_STATE->state_adc_output_ready = 1
    //PMU_CONTROLLER_DESIRED_STATE
    mbus_remote_register_write(PMU_ADDR,0x3B,
							   ((  0x1 << 0) //state_sar_scn_on
								| (0x1 << 1) //state_wait_for_clock_cycles
								| (0x1 << 2) //state_wait_for_time
								| (0x1 << 3) //state_sar_scn_reset
								| (0x1 << 4) //state_sar_scn_stabilized
								| (0x1 << 5) //state_sar_scn_ratio_roughly_adjusted
								| (0x1 << 6) //state_clock_supply_switched
								| (0x1 << 7) //state_control_supply_switched
								| (0x1 << 8) //state_upconverter_on
								| (0x1 << 9) //state_upconverter_stabilized
								| (0x1 <<10) //state_refgen_on
								| (0x1 <<11) //state_adc_output_ready
								| (0x1 <<12) //state_adc_adjusted
								| (0x0 <<13) //state_sar_scn_ratio_adjusted
								| (0x1 <<14) //state_downconverter_on
								| (0x1 <<15) //state_downconverter_stabilized
								| (0x1 <<16) //state_vdd_3p6_turned_on
								| (0x1 <<17) //state_vdd_1p2_turned_on
								| (0x1 <<18) //state_vdd_0P6_turned_on
								| (0x1 <<19) //state_state_horizon
								));
    delay(MBUS_DELAY);

    //Poll PMU_CONTROLLER_STATE_MAIN
    mbus_remote_register_write(PMU_ADDR,0x00,0x3C);
    delay(MBUS_DELAY);

	//Read PMU_ADC_STORED_OUT
    mbus_remote_register_write(PMU_ADDR,0x00,0x03);
    delay(MBUS_DELAY);

	adc_offset = *((volatile uint32_t *) 0xA0000000);
	adc_offset = adc_offset >> 8;
	mbus_remote_register_write(0x07,0xAA,adc_offset);

    //2. Override PMU_ADC_CONFIG_OVERRIDE
    mbus_remote_register_write(PMU_ADDR,0x02,
							   ((  0x00 << 0) //Offset cancelation bits
								| (0x0  << 7) // If ADC uses CDAC LSB
								| (0xFF << 8) // If ADC uses corresnpoding CDAC bits
								| (0x1  <<16) // ADC Ref Voltage (0: VREF, 1: VBAT)
								| (0x0  <<17) // ADC Sampling Voltage (0: VREF, 1: VBAT)
								| (0x1  <<23) // Enables Override Settings
								));
    delay(MBUS_DELAY);
    
    //3. Turn PMU_CONTROLLER_DESIRED_STATE->state_adc_output_ready = 0
    //PMU_CONTROLLER_DESIRED_STATE
    mbus_remote_register_write(PMU_ADDR,0x3B,
							   ((  0x1 << 0) //state_sar_scn_on
								| (0x1 << 1) //state_wait_for_clock_cycles
								| (0x1 << 2) //state_wait_for_time
								| (0x1 << 3) //state_sar_scn_reset
								| (0x1 << 4) //state_sar_scn_stabilized
								| (0x1 << 5) //state_sar_scn_ratio_roughly_adjusted
								| (0x1 << 6) //state_clock_supply_switched
								| (0x1 << 7) //state_control_supply_switched
								| (0x1 << 8) //state_upconverter_on
								| (0x1 << 9) //state_upconverter_stabilized
								| (0x1 <<10) //state_refgen_on
								| (0x0 <<11) //state_adc_output_ready
								| (0x1 <<12) //state_adc_adjusted
								| (0x0 <<13) //state_sar_scn_ratio_adjusted
								| (0x1 <<14) //state_downconverter_on
								| (0x1 <<15) //state_downconverter_stabilized
								| (0x1 <<16) //state_vdd_3p6_turned_on
								| (0x1 <<17) //state_vdd_1p2_turned_on
								| (0x1 <<18) //state_vdd_0P6_turned_on
								| (0x1 <<19) //state_state_horizon
								));
    delay(MBUS_DELAY);
	
    //Poll PMU_CONTROLLER_STATE_MAIN
    mbus_remote_register_write(PMU_ADDR,0x00,0x3C);
    delay(MBUS_DELAY);

    //4. Turn PMU_CONTROLLER_DESIRED_STATE->state_adc_output_ready = 1
    //PMU_CONTROLLER_DESIRED_STATE
    mbus_remote_register_write(PMU_ADDR,0x3B,
							   ((  0x1 << 0) //state_sar_scn_on
								| (0x1 << 1) //state_wait_for_clock_cycles
								| (0x1 << 2) //state_wait_for_time
								| (0x1 << 3) //state_sar_scn_reset
								| (0x1 << 4) //state_sar_scn_stabilized
								| (0x1 << 5) //state_sar_scn_ratio_roughly_adjusted
								| (0x1 << 6) //state_clock_supply_switched
								| (0x1 << 7) //state_control_supply_switched
								| (0x1 << 8) //state_upconverter_on
								| (0x1 << 9) //state_upconverter_stabilized
								| (0x1 <<10) //state_refgen_on
								| (0x1 <<11) //state_adc_output_ready
								| (0x1 <<12) //state_adc_adjusted
								| (0x0 <<13) //state_sar_scn_ratio_adjusted
								| (0x1 <<14) //state_downconverter_on
								| (0x1 <<15) //state_downconverter_stabilized
								| (0x1 <<16) //state_vdd_3p6_turned_on
								| (0x1 <<17) //state_vdd_1p2_turned_on
								| (0x1 <<18) //state_vdd_0P6_turned_on
								| (0x1 <<19) //state_state_horizon
								));
    delay(MBUS_DELAY);

    //Poll PMU_CONTROLLER_STATE_MAIN
    mbus_remote_register_write(PMU_ADDR,0x00,0x3C);
    delay(MBUS_DELAY);

	//Read PMU_ADC_STORED_OUT
    mbus_remote_register_write(PMU_ADDR,0x00,0x03);
    delay(MBUS_DELAY);

}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
	//Initialize Interrupts
	disable_all_irq();

	// Initialization Sequence
	if (enumerated != 0xDEADBEEF) { 
		initialization();
	}

	operation_pmu_adc();

    // Should not reach here
    operation_sleep_notimer();

	while(1){  //Never Quit (should not come here.)
		arb_debug_reg (0xDEADBEEF);
		asm("nop;"); 
	}
  
  return 1;
}
