//*******************************************************************
//Author: Yejoong Kim
//Description: Developed during PRCv13 tape-out for verification
//*******************************************************************
#include "PRCv13.h"
#include "PRCv13_RF.h"
#include "mbus.h"
#include "SNSv7.h"
#include "HRVv2.h"
#include "RADv9.h"
#include "PMUv2_RF.h"

// uncomment this for debug mbus message
 #define DEBUG_MBUS_MSG

// TStack order  PRC->RAD->SNS->PMU
#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define HRV_ADDR 0x6
#define FLP_ADDR 0x7
#define PMU_ADDR 0x8

#define MBUS_DELAY	100

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t wakeup_period;
volatile uint32_t exec_count;

volatile snsv7_r14_t snsv7_r14 = SNSv7_R14_DEFAULT;
volatile snsv7_r15_t snsv7_r15 = SNSv7_R15_DEFAULT;
volatile snsv7_r18_t snsv7_r18 = SNSv7_R18_DEFAULT;
volatile snsv7_r25_t snsv7_r25 = SNSv7_R25_DEFAULT;

volatile prcv13_r0B_t prcv13_r0B = PRCv13_R0B_DEFAULT;


//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_0(void)  __attribute__ ((interrupt ("IRQ")));
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
void handler_ext_int_4(void)  { *NVIC_ICPR = (0x1 << 4);  } // REG
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
// FUNCTIONS 
//*******************************************************************

inline static void mbus_pmuv3h_register_write(uint32_t reg_id, uint32_t reg_data){
    set_halt_until_mbus_rx();
    mbus_remote_register_write(PMU_ADDR,reg_id,reg_data); 
    set_halt_until_mbus_tx();
}

inline static void mbus_pmuv3h_register_read(uint32_t reg_id){
    set_halt_until_mbus_rx();
    mbus_remote_register_write(PMU_ADDR,0x00,reg_id); 
    set_halt_until_mbus_tx();
}


//***************************************************
// Temp Sensor Functions (SNSv7)
//***************************************************

static void temp_sensor_enable(){
    snsv7_r14.TEMP_SENSOR_ENABLEb = 0x0;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
	delay(MBUS_DELAY*10);
}

static void temp_sensor_disable(){
    snsv7_r14.TEMP_SENSOR_ENABLEb = 1;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
	delay(MBUS_DELAY*10);
}

static void temp_sensor_release_reset(){
    snsv7_r14.TEMP_SENSOR_RESETn = 1;
    snsv7_r14.TEMP_SENSOR_ISO = 0;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
	delay(MBUS_DELAY*10);
}

static void temp_sensor_assert_reset(){
    snsv7_r14.TEMP_SENSOR_RESETn = 0;
    snsv7_r14.TEMP_SENSOR_ISO = 1;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
	delay(MBUS_DELAY*10);
}

static void ldo_power_on(){
    snsv7_r18.ADC_LDO_ADC_LDO_ENB = 0;
    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
	delay(MBUS_DELAY*300);
    snsv7_r18.ADC_LDO_ADC_LDO_DLY_ENB = 0;
    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
	delay(MBUS_DELAY*300);
}

static void ldo_power_off(){
    snsv7_r18.ADC_LDO_ADC_LDO_DLY_ENB = 1;
    snsv7_r18.ADC_LDO_ADC_LDO_ENB = 1;
    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
	delay(MBUS_DELAY*10);
}

static void temp_power_off(){
    snsv7_r14.TEMP_SENSOR_ENABLEb = 1;
    snsv7_r14.TEMP_SENSOR_RESETn = 0;
    snsv7_r14.TEMP_SENSOR_ISO = 1;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
	delay(MBUS_DELAY*10);
    ldo_power_off();
}


//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {


	// Set CPU & Mbus Clock Speeds
    prcv13_r0B.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
    prcv13_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
	*((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv13_r0B.as_int;

    enumerated = 0xDEADBEEF;
	wakeup_period = 5;
	exec_count = 0;

    set_halt_until_mbus_rx();
    // Enumeration
 	mbus_enumerate(RAD_ADDR);
 	mbus_enumerate(SNS_ADDR);
 	mbus_enumerate(PMU_ADDR);

    set_halt_until_mbus_tx();

    // PMU Settings --------------------------------------
	// Register 0x17: UPCONV_TRIM_V3_SLEEP
    mbus_pmuv3h_register_write(0x17, 
		( (3 << 14) // [0, 3]  Desired Vout/Vin ratio
		| (1 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (0 << 5)  // [0, 15] Frequency multiplier L (actually L+1)
		| (4) 		// [2, 31] Floor frequency base
	));
	// Register 0x18: UPCONV_TRIM_V3_ACTIVE
    mbus_pmuv3h_register_write(0x18, 
		( (3 << 14) // [0, 3]  Desired Vout/Vin ratio
		| (1 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (2 << 5)  // [2, 15] Frequency multiplier L (actually L+1)
		| (10) 		// [2, 31] Floor frequency base
	));
	// Register 0x19: DOWNCONV_TRIM_V3_SLEEP
    mbus_pmuv3h_register_write(0x19,
		( (1 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (0 << 5)  // [0, 15] Frequency multiplier L (actually L+1)
		| (2) 		// [1, 31] Floor frequency base
	));
	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
    mbus_pmuv3h_register_write(0x1A,
		( (1 << 13) // [1, 1]  Enable main feedback loop
		| (8 << 9)  // [4, 15] Frequency multiplier R
		| (4 << 5)  // [2, 15] Frequency multiplier L (actually L+1)
		| (8) 		// [8, 31] Floor frequency base
	));
	// Register 0x15: SAR_TRIM_v3_SLEEP
    mbus_pmuv3h_register_write(0x15, 
		( (0 << 19) // [0, 1]  Enable PFM even during periodic reset
		| (0 << 18) // [0, 1]  Enable PFM even when Vref is not used as ref
		| (0 << 17) // [0, 1]  Enable PFM
		| (3 << 14) // [3, 7]  Comparator clock division ratio
		| (1 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (0 << 5)  // [0, 15] Frequency multiplier L (actually L+1)
		| (4) 		// [1, 31] Floor frequency base
	));
	// Register 0x16: SAR_TRIM_v3_ACTIVE
    mbus_pmuv3h_register_write(0x16, 
		( (0 << 19) // [0, 1]  Enable PFM even during periodic reset
		| (0 << 18) // [0, 1]  Enable PFM even when Vref is not used as ref
		| (0 << 17) // [0, 1]  Enable PFM
		| (3 << 14) // [3, 7]  Comparator clock division ratio
		| (1 << 13) // [1, 1]  Enable main feedback loop
		| (8 << 9)  // [4, 15] Frequency multiplier R
		| (8 << 5)  // [2, 15] Frequency multiplier L (actually L+1)
		| (15) 		// [8, 31] Floor frequency base
	));
	// Register 0x05: SAR_RATIO_OVERRIDE
    mbus_pmuv3h_register_write(0x05,
		( (1 << 13) // [1, 1]
        | (0 << 12) // [0, 1]
        | (1 << 11) // [0, 1]   Enable override setting [10]
		| (0 << 10) // [0, 1]   Have the converter have the periodic reset
		| (1 << 9)  // [0, 1]   Enable override setting [8]
		| (0 << 8)  // [0, 1]   Switch input / output power rails for upconversion
		| (1 << 7)  // [0, 1]   Enable override setting [6:0]
		| (44) 		// [0, 127] Binary converter's conversion ratio
	));
	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    mbus_pmuv3h_register_write(0x36,
        (1)         // [500, 16777215] Num of clock cycles the controller waits per each periodic conversion ratio adjustment after being fully turned-on
    );
	// Register 0x37: TICK_WAKEUP_WAIT
    mbus_pmuv3h_register_write(0x37, 2000); // [500, 16777215] Num of clock cycles the controller waits for internal state transition from sleep to wakeup mode

	// Disable PMU ADC measurement in active mode
	// Register 0x3A: PMU_CONTROLLER_STALL_ACTIVE
    mbus_pmuv3h_register_write(0x3A, 
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
	mbus_pmuv3h_register_write(0x3B,
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
	mbus_pmuv3h_register_write(0x3C,
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


    // Temp Sensor Settings --------------------------------------
	// SNSv7_R25
	snsv7_r25.TEMP_SENSOR_IRQ_PACKET = 0x001000;
    mbus_remote_register_write(SNS_ADDR,0x19,snsv7_r25.as_int);
    // SNSv7_R14
	snsv7_r14.TEMP_SENSOR_DELAY_SEL = 3;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
    // snsv7_R15
	snsv7_r15.TEMP_SENSOR_SEL_CT = 7;
    mbus_remote_register_write(SNS_ADDR,0xF,snsv7_r15.as_int);

    // snsv7_R18
    snsv7_r18.ADC_LDO_ADC_LDO_ENB      = 0x1;
    snsv7_r18.ADC_LDO_ADC_LDO_DLY_ENB  = 0x1;
    snsv7_r18.ADC_LDO_ADC_CURRENT_2X  = 0x1;

    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv7_r18.ADC_LDO_ADC_VREF_MUX_SEL = 0x2;
    snsv7_r18.ADC_LDO_ADC_VREF_SEL     = 0x10;

    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);

	// Mbus return address; Needs to be between 0x18-0x1F
    mbus_remote_register_write(SNS_ADDR,0x18,0x1800);

}

//********************************************************************
// MAIN function starts here             
//***************************************************************-*****

int main() {


    set_wakeup_timer(100, 0, 1); // Reset Wakeup Timer; 
	enable_reg_irq(); // Initialize Interrupts, Only enable register-related interrupts
    config_timerwd(0x3FFFFF); // Config watchdog timer to about 10 sec (default: 0x02FFFFFF), 0xFFFFF about 13 sec with Y2 run default clock
//	disable_timerwd();

    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        initialization();
    }

	uint32_t read_data_temp_done; // [0] Temp Sensor Done
	uint32_t read_data_temp_data; // [23:0] Temp Sensor D Out
		
	ldo_power_on();
	temp_sensor_release_reset();
	temp_sensor_enable();

	do{
		delay(MBUS_DELAY*100);
		mbus_remote_register_read(SNS_ADDR,0x10,1);
		read_data_temp_done = *((volatile uint32_t *) REG1);
		mbus_remote_register_read(SNS_ADDR,0x11,1);
		read_data_temp_data = *((volatile uint32_t *) REG1);
	}while((read_data_temp_data & 0x1) != 0x1);

	temp_sensor_disable();
	temp_sensor_assert_reset();	
	temp_power_off();

	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xCC, exec_count<<4);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_temp_done<<4);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_temp_data<<4);
		delay(MBUS_DELAY);
	#endif
	
	exec_count++;
 	set_wakeup_timer(wakeup_period, 1, 1);
    mbus_sleep_all();

    while(1){
	delay(100);}
}
