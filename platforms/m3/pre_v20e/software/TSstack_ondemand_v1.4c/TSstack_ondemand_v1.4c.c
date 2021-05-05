//*******************************************************************
//Author: Gyouho Kim
//Description: TS stack with MRRv10 and SNTv4
//			Modified from 'AHstack_ondemand_v1.22.c'
//			Modified from 'Tstack_ondemand_v6.4.c'
//			v1.0: PMU needs to reconfigure based on VBAT
//			v1.1: Code cleanup
//			v1.1a: Increasing PMU setting at 35C
//			v1.1b: Fixing a bug that incorrectly sets PMU active setting during SNT
//			v1.1c: Operation sleep was missing SNT safeguards
//			v1.2: Transmit SNT temp coefficient B as part of check-in message
//			FIXME: Need to verify PMU settings across temp & voltage
//			v1.3: Adding new MRR FSM burst mode for radio scanning
//				   Adding SNT Timer R tuning
//			v1.3b: Fixes a problem where running send_radio_data_mrr() more than once and 
//				   running radio_power_on() between them causes CL to be in weak mode
//			v1.4: PMU Optimizations
//			v1.4a: Minor updates for debugging; change SAR ratio every time irq wakeup
//				  send exec_count when stopping measurement
//			v1.4b: SNT temp sensor reset & sleep setting restored whenever GOC triggered
//				  Adding a dedicated setting for # of hops
//			v1.4c: Changing default hop spacing to 500
//*******************************************************************
#include "PREv20.h"
#include "PREv20_RF.h"
#include "mbus.h"
#include "SNTv4_RF.h"
#include "PMUv7_RF.h"
#include "MRRv10_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// AM stack 
#define MRR_ADDR 0x4
#define SNT_ADDR 0x5
#define PMU_ADDR 0x6

// System parameters
#define	MBUS_DELAY 200 // Amount of delay between successive messages; 200: 6-7ms

// Tstack states
#define    STK_IDLE       0x0
#define    STK_LDO        0x1
#define    STK_TEMP_START 0x2
#define    STK_TEMP_READ  0x6

// Radio configurations
#define RADIO_DATA_LENGTH 192
#define WAKEUP_PERIOD_RADIO_INIT 0xA // About 2 sec (PRCv17)

#define    PMU_0C 0x0
#define    PMU_10C 0x1
#define    PMU_15C 0x2
#define    PMU_20C 0x3
#define    PMU_25C 0x4
#define    PMU_30C 0x5
#define    PMU_35C 0x6
#define    PMU_40C 0x7
#define    PMU_45C 0x8
#define    PMU_55C 0x9
#define    PMU_65C 0xA
#define    PMU_75C 0xB
#define    PMU_85C 0xC
#define    PMU_95C 0xD

#define NUM_TEMP_MEAS 1

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t stack_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t error_code;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t wakeup_count;
volatile uint32_t exec_count_irq;
volatile uint32_t PMU_ADC_3P0_VAL;
volatile uint32_t pmu_setting_state;
volatile uint32_t pmu_parking_mode;
volatile uint32_t read_data_batadc;
volatile uint32_t read_data_batadc_diff;

volatile uint32_t WAKEUP_PERIOD_SNT; 

volatile uint32_t PMU_10C_threshold_sns;
volatile uint32_t PMU_15C_threshold_sns;
volatile uint32_t PMU_20C_threshold_sns;
volatile uint32_t PMU_25C_threshold_sns;
volatile uint32_t PMU_30C_threshold_sns;
volatile uint32_t PMU_35C_threshold_sns;
volatile uint32_t PMU_40C_threshold_sns;
volatile uint32_t PMU_45C_threshold_sns;
volatile uint32_t PMU_55C_threshold_sns;
volatile uint32_t PMU_65C_threshold_sns;
volatile uint32_t PMU_75C_threshold_sns;
volatile uint32_t PMU_85C_threshold_sns;
volatile uint32_t PMU_95C_threshold_sns;

volatile uint32_t temp_storage_latest = 150; // SNSv10
volatile uint32_t temp_storage_last_wakeup_adjust = 150; // SNSv10
volatile uint32_t temp_storage_diff = 0;
volatile uint32_t temp_storage_count;
volatile uint32_t temp_storage_debug;
volatile uint32_t sns_running;
volatile uint32_t read_data_temp; // [23:0] Temp Sensor D Out
volatile uint32_t TEMP_CALIB_A;
volatile uint32_t TEMP_CALIB_B;

volatile uint32_t snt_wup_counter_cur;
volatile uint32_t snt_timer_enabled = 0;
volatile uint32_t SNT_0P5S_VAL;

volatile uint32_t sns_running;

volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t mrr_freq_hopping;
volatile uint32_t mrr_freq_hopping_step;
volatile uint32_t mrr_cfo_val_fine_min;
volatile uint32_t RADIO_PACKET_DELAY;
volatile uint32_t radio_packet_count;

volatile sntv4_r00_t sntv4_r00 = SNTv4_R00_DEFAULT;
volatile sntv4_r01_t sntv4_r01 = SNTv4_R01_DEFAULT;
volatile sntv4_r03_t sntv4_r03 = SNTv4_R03_DEFAULT;
volatile sntv4_r08_t sntv4_r08 = SNTv4_R08_DEFAULT;
volatile sntv4_r09_t sntv4_r09 = SNTv4_R09_DEFAULT;
volatile sntv4_r0A_t sntv4_r0A = SNTv4_R0A_DEFAULT;
volatile sntv4_r0B_t sntv4_r0B = SNTv4_R0B_DEFAULT;
volatile sntv4_r17_t sntv4_r17 = SNTv4_R17_DEFAULT;

volatile prev20_r0B_t prev20_r0B = PREv20_R0B_DEFAULT;
volatile prev20_r1C_t prev20_r1C = PREv20_R1C_DEFAULT;

volatile mrrv10_r00_t mrrv10_r00 = MRRv10_R00_DEFAULT;
volatile mrrv10_r01_t mrrv10_r01 = MRRv10_R01_DEFAULT;
volatile mrrv10_r02_t mrrv10_r02 = MRRv10_R02_DEFAULT;
volatile mrrv10_r03_t mrrv10_r03 = MRRv10_R03_DEFAULT;
volatile mrrv10_r04_t mrrv10_r04 = MRRv10_R04_DEFAULT;
volatile mrrv10_r07_t mrrv10_r07 = MRRv10_R07_DEFAULT;
volatile mrrv10_r11_t mrrv10_r11 = MRRv10_R11_DEFAULT;
volatile mrrv10_r12_t mrrv10_r12 = MRRv10_R12_DEFAULT;
volatile mrrv10_r13_t mrrv10_r13 = MRRv10_R13_DEFAULT;
volatile mrrv10_r14_t mrrv10_r14 = MRRv10_R14_DEFAULT;
volatile mrrv10_r15_t mrrv10_r15 = MRRv10_R15_DEFAULT;
volatile mrrv10_r16_t mrrv10_r16 = MRRv10_R16_DEFAULT;
volatile mrrv10_r1F_t mrrv10_r1F = MRRv10_R1F_DEFAULT;
volatile mrrv10_r21_t mrrv10_r21 = MRRv10_R21_DEFAULT;


//***************************************************
// Sleep Functions
//***************************************************
static void operation_sns_sleep_check(void);
static void operation_sleep(void);
static void operation_sleep_noirqreset(void);
static void operation_sleep_notimer(void);


//***************************************************
// Timeout Functions
//***************************************************

static void set_timer32_timeout(uint32_t val){
	// Use Timer32 as timeout counter
    wfi_timeout_flag = 0;
	config_timer32(val, 1, 0, 0);
}

static void stop_timer32_timeout_check(uint32_t code){
	// Turn off Timer32
	*TIMER32_GO = 0;
	if (wfi_timeout_flag){
		wfi_timeout_flag = 0;
		error_code = code;
		mbus_write_message32(0xFA, error_code);
	}
}


//*******************************************************************
// INTERRUPT HANDLERS (Updated for PRCv17)
//*******************************************************************

void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
	wfi_timeout_flag = 1;
    set_halt_until_mbus_tx();
    }
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);
}
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1);
}
void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2);
}
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3);
}
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
}
void handler_ext_int_wakeup(void) { // WAKE-UP
//[ 0] = GOC/EP
//[ 1] = Wakeuptimer
//[ 2] = XO timer
//[ 3] = gpio_pad
//[ 4] = mbus message
//[ 8] = gpio[0]
//[ 9] = gpio[1]
//[10] = gpio[2]
//[11] = gpio[3]
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
	// Report who woke up
	delay(MBUS_DELAY);
	mbus_write_message32(0xAA,*SREG_WAKEUP_SOURCE); // 0x1: GOC; 0x2: PRC Timer; 0x10: SNT
}


//************************************
// PMU Related Functions
//************************************

static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
	
	set_timer32_timeout(TIMER32_VAL);
    set_halt_until_mbus_trx();
    mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
    set_halt_until_mbus_tx();
	stop_timer32_timeout_check(0x7);
}


static void pmu_set_sar_override(uint32_t val){
	// SAR_RATIO_OVERRIDE
    pmu_reg_write(0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (val) 		// Binary converter's conversion ratio (7'h00)
	));
    pmu_reg_write(0x05, //default 12'h000
		( (1 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (val) 		// Binary converter's conversion ratio (7'h00)
	));
}

inline static void pmu_set_adc_period(uint32_t val){
	// PMU_CONTROLLER_DESIRED_STATE Active
	// Updated for PMUv9
	pmu_reg_write(0x3C,
		((  1 << 0) //state_sar_scn_on
		| (0 << 1) //state_wait_for_clock_cycles
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
		| (0 << 13) //state_sar_scn_ratio_adjusted
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (0 << 19) //state_vbat_read
		| (1 << 20) //state_state_horizon
	));

	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    pmu_reg_write(0x36,val); 
	delay(MBUS_DELAY*10);

	// Register 0x33: TICK_ADC_RESET
	pmu_reg_write(0x33,2);

	// Register 0x34: TICK_ADC_CLK
	pmu_reg_write(0x34,2);

	// PMU_CONTROLLER_DESIRED_STATE Active
	pmu_reg_write(0x3C,
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
		| (0 << 13) //state_sar_scn_ratio_adjusted
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (0 << 19) //state_vbat_read
		| (1 << 20) //state_state_horizon
	));
}

inline static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

	// The first register write to PMU needs to be repeated
	// Register 0x16: V1P2 Active
    pmu_reg_write(0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
    pmu_reg_write(0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	// Register 0x18: V3P6 Active 
    pmu_reg_write(0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	// Register 0x1A: V0P6 Active
    pmu_reg_write(0x1A,
		( (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));

}

inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

	// Register 0x17: V3P6 Sleep
    pmu_reg_write(0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	// Register 0x15: V1P2 Sleep
    pmu_reg_write(0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	// Register 0x19: V0P6 Sleep
    pmu_reg_write(0x19,
		( (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));

}

inline static void pmu_set_sleep_radio(){
    pmu_set_sleep_clk(0xF,0xA,0x5,0xF/*V1P2*/);
}

inline static void pmu_set_sleep_low(){
    pmu_set_sleep_clk(0xF,0x0,0x1,0x1/*V1P2*/);
}

inline static void pmu_set_sleep_tsns(){
    if (pmu_setting_state >= PMU_75C){
        pmu_set_sleep_clk(0x5,0xA,0x5,0xF/*V1P2*/);

    }else if (pmu_setting_state >= PMU_35C){
        pmu_set_sleep_clk(0xA,0xA,0x5,0xF/*V1P2*/);

    }else if (pmu_setting_state < PMU_20C){
        pmu_set_sleep_clk(0xF,0xA,0x7,0xF/*V1P2*/);

    }else{ // 25C, default
    	pmu_set_sleep_clk(0xF,0xA,0x5,0xF/*V1P2*/);
    }
}

inline static void pmu_sleep_setting_temp_based(){
    
    if (pmu_setting_state >= PMU_95C){
		pmu_set_sleep_clk(0x2,0x0,0x1,0x1/*V1P2*/);

    }else if (pmu_setting_state >= PMU_75C){
		pmu_set_sleep_clk(0x4,0x0,0x1,0x1/*V1P2*/);

    }else if (pmu_setting_state >= PMU_55C){
		pmu_set_sleep_clk(0x6,0x0,0x1,0x1/*V1P2*/);

    }else if (pmu_setting_state >= PMU_35C){
		pmu_set_sleep_low();

    }else if (pmu_setting_state < PMU_10C){
		pmu_set_sleep_clk(0xF,0x2,0x1,0x4/*V1P2*/);

    }else if (pmu_setting_state < PMU_20C){
		pmu_set_sleep_clk(0xF,0x1,0x1,0x2/*V1P2*/);

    }else{ // 25C, default
		pmu_set_sleep_low();
    }
}
inline static void pmu_active_setting_temp_based(){
    
    if (pmu_setting_state >= PMU_95C){
		pmu_set_active_clk(0x1,0x1,0x10,0x2/*V1P2*/);

    }else if (pmu_setting_state == PMU_85C){
		pmu_set_active_clk(0x2,0x1,0x10,0x2/*V1P2*/);

    }else if (pmu_setting_state == PMU_75C){
		pmu_set_active_clk(0x2,0x2,0x10,0x4/*V1P2*/);

    }else if (pmu_setting_state == PMU_65C){
		pmu_set_active_clk(0x4,0x2,0x10,0x4/*V1P2*/);

    }else if (pmu_setting_state == PMU_55C){
		pmu_set_active_clk(0x6,0x2,0x10,0x4/*V1P2*/);

    }else if (pmu_setting_state == PMU_45C){
	    pmu_set_active_clk(0x9,0x2,0x10,0x4/*V1P2*/);

    }else if (pmu_setting_state == PMU_40C){
	    pmu_set_active_clk(0xB,0x2,0x10,0x4/*V1P2*/);

    }else if (pmu_setting_state == PMU_35C){
	    pmu_set_active_clk(0xD,0x2,0x10,0x4/*V1P2*/);

    }else if (pmu_setting_state == PMU_30C){
	    pmu_set_active_clk(0xF,0x3,0x10,0x5/*V1P2*/);

    }else if (pmu_setting_state == PMU_25C){
		pmu_set_active_clk(0xF,0x5,0x10,0xA/*V1P2*/);

    }else if (pmu_setting_state == PMU_20C){
		pmu_set_active_clk(0xD,0xA,0x10,0xE/*V1P2*/);

    }else if (pmu_setting_state == PMU_15C){
		pmu_set_active_clk(0xE,0xA,0x10,0xE/*V1P2*/);

    }else if (pmu_setting_state == PMU_10C){
		pmu_set_active_clk(0xF,0xA,0x10,0xE/*V1P2*/);

    }else{ 
		pmu_set_active_clk(0xF,0xA,0x1F,0xE/*V1P2*/);
    }
}


inline static void pmu_set_clk_init(){
    pmu_setting_state = PMU_25C;
    pmu_active_setting_temp_based();
    pmu_sleep_setting_temp_based();

	// SAR_RATIO_OVERRIDE
	// Use the new reset scheme in PMUv3
    pmu_reg_write(0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (0 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (0 << 7) // Enable override setting [6:0] (1'h0)
		| (0x45) 		// Binary converter's conversion ratio (7'h00)
	));
	pmu_set_sar_override(0x45);

	pmu_set_adc_period(1); // 0x100 about 1 min for 1/2/1 1P2 setting
}


inline static void pmu_adc_reset_setting(){
	// PMU ADC will be automatically reset when system wakes up
	// PMU_CONTROLLER_DESIRED_STATE Active
	// Updated for PMUv9
	pmu_reg_write(0x3C,
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
		| (0 << 13) //state_sar_scn_ratio_adjusted
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (0 << 19) //state_vbat_read
		| (1 << 20) //state_state_horizon
	));
}

inline static void pmu_adc_disable(){
	// PMU ADC will be automatically reset when system wakes up
	// PMU_CONTROLLER_DESIRED_STATE Sleep
	// Updated for PMUv9
	pmu_reg_write(0x3B,
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
		| (1 << 13) //state_sar_scn_ratio_adjusted // Turn on for old adc, off for new adc
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (0 << 19) //state_vbat_read // Turn off for old adc
		| (1 << 20) //state_state_horizon
	));
}

inline static void pmu_adc_enable(){
	// PMU ADC will be automatically reset when system wakes up
	// PMU_CONTROLLER_DESIRED_STATE Sleep
	// Updated for PMUv9
	pmu_reg_write(0x3B,
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
		| (0 << 12) //state_adc_adjusted // Turning off offset cancellation
		| (1 << 13) //state_sar_scn_ratio_adjusted // Turn on for old adc, off for new adc
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (0 << 19) //state_vbat_read // Turn off for old adc
		| (1 << 20) //state_state_horizon
	));
}

inline static void pmu_parking_decision_3v_battery(){
	
	// Battery > 3.0V
	if (read_data_batadc < (PMU_ADC_3P0_VAL)){
		pmu_set_sar_override(0x3C);

	// Battery 2.9V - 3.0V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 4){
		pmu_set_sar_override(0x3F);

	// Battery 2.8V - 2.9V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 8){
		pmu_set_sar_override(0x41);

	// Battery 2.7V - 2.8V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 12){
		pmu_set_sar_override(0x43);

	// Battery 2.6V - 2.7V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 17){
		pmu_set_sar_override(0x45);

	// Battery 2.5V - 2.6V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 21){
		pmu_set_sar_override(0x48);

	// Battery 2.4V - 2.5V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 27){
		pmu_set_sar_override(0x4B);

	// Battery 2.3V - 2.4V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 32){
		pmu_set_sar_override(0x4E);

	// Battery 2.2V - 2.3V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 39){
		pmu_set_sar_override(0x51);

	// Battery 2.1V - 2.2V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 46){
		pmu_set_sar_override(0x56);

	// Battery 2.0V - 2.1V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 53){
		pmu_set_sar_override(0x5A);

	// Battery <= 2.0V
	}else{
		pmu_set_sar_override(0x5F);
	}
	
}

inline static void pmu_adc_read_latest(){
	// Grab latest PMU ADC readings
	// PMU register read is handled differently
	pmu_reg_write(0x00,0x03);
	// Updated for PMUv9
	read_data_batadc = *((volatile uint32_t *) REG0) & 0xFF;

	if (read_data_batadc<PMU_ADC_3P0_VAL){
		read_data_batadc_diff = 0;
	}else{
		read_data_batadc_diff = read_data_batadc - PMU_ADC_3P0_VAL;
	}

	pmu_parking_decision_3v_battery();

}

inline static void pmu_reset_solar_short(){
	// Updated for PMUv9
    pmu_reg_write(0x0E, 
		( (0 << 12) // 1: solar short by latched vbat_high (new); 0: follow [10] setting
		| (1 << 11) // Reset of vbat_high latch for [12]=1
		| (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
		| (1 << 9)  // Enables override setting [8]
		| (0 << 8)  // Turn on the harvester-inhibiting switch
		| (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
		| (0) 		// clamp_tune_top (decreases clamp thresh)
	));
    pmu_reg_write(0x0E, 
		( (0 << 12) // 1: solar short by latched vbat_high (new); 0: follow [10] setting
		| (1 << 11) // Reset of vbat_high latch for [12]=1
		| (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
		| (0 << 9)  // Enables override setting [8]
		| (0 << 8)  // Turn on the harvester-inhibiting switch
		| (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
		| (0) 		// clamp_tune_top (decreases clamp thresh)
	));
}

//***************************************************
// CRC16 Encoding
//***************************************************
#define DATA_LEN 96
#define CRC_LEN 16

uint32_t* crcEnc16(uint32_t data2, uint32_t data1, uint32_t data0)
{
    // intialization
    uint32_t i;
   
    uint16_t poly = 0xc002;
    uint16_t poly_not = ~poly;
    uint16_t remainder = 0x0000;
    uint16_t remainder_shift = 0x0000;
    data2 = (data2 << CRC_LEN) + (data1 >> CRC_LEN);
    data1 = (data1 << CRC_LEN) + (data0 >> CRC_LEN);
    data0 = data0 << CRC_LEN;
    
    // LFSR
    uint16_t input_bit;
    for (i = 0; i < DATA_LEN; i++)
    {
        uint16_t MSB;
        if (remainder > 0x7fff)
            MSB = 0xffff;
        else
            MSB = 0x0000;
        
        if (i < 32)
            input_bit = ((data2 << i) > 0x7fffffff);
        else if (i < 64)
            input_bit = (data1 << (i-32)) > 0x7fffffff;
        else
            input_bit = (data0 << (i-64)) > 0x7fffffff;

        remainder_shift = remainder << 1;
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift))
                         + (input_bit^(remainder > 0x7fff));
    }

    data0 = data0 + remainder;

    static uint32_t msg_out[3];
    msg_out[0] = data2;
    msg_out[1] = data1;
    msg_out[2] = data0;

    return msg_out;    
}

//***************************************************
// MRR Functions
//***************************************************

static void mrr_ldo_vref_on(){
    mrrv10_r04.LDO_EN_VREF    = 1;
    mbus_remote_register_write(MRR_ADDR,0x4,mrrv10_r04.as_int);
}

static void mrr_ldo_power_on(){
    mrrv10_r04.LDO_EN_IREF    = 1;
    mrrv10_r04.LDO_EN_LDO    = 1;
    mbus_remote_register_write(MRR_ADDR,0x4,mrrv10_r04.as_int);
}
static void mrr_ldo_power_off(){
    mrrv10_r04.LDO_EN_VREF    = 0;
    mrrv10_r04.LDO_EN_IREF    = 0;
    mrrv10_r04.LDO_EN_LDO    = 0;
    mbus_remote_register_write(MRR_ADDR,0x4,mrrv10_r04.as_int);
}

static void radio_power_on(){
	// Turn off PMU ADC
	//pmu_adc_disable();

	// Need to speed up sleep pmu clock
	//pmu_set_sleep_radio();

	// New for MRRv10
	mrr_ldo_vref_on();

    // Turn off Current Limter Briefly
    mrrv10_r00.MRR_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);

	// Set decap to parallel
	mrrv10_r03.MRR_DCP_S_OW = 0;  //TX_Decap S (forced charge decaps)
	mbus_remote_register_write(MRR_ADDR,3,mrrv10_r03.as_int);
	mrrv10_r03.MRR_DCP_P_OW = 1;  //RX_Decap P 
	mbus_remote_register_write(MRR_ADDR,3,mrrv10_r03.as_int);
    delay(MBUS_DELAY);

	// Set decap to series
	mrrv10_r03.MRR_DCP_P_OW = 0;  //RX_Decap P 
	mbus_remote_register_write(MRR_ADDR,3,mrrv10_r03.as_int);
	mrrv10_r03.MRR_DCP_S_OW = 1;  //TX_Decap S (forced charge decaps)
	mbus_remote_register_write(MRR_ADDR,3,mrrv10_r03.as_int);
    delay(MBUS_DELAY);

	// Current Limter set-up 
	mrrv10_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);

    radio_on = 1;

	// New for MRRv10
	mrr_ldo_power_on();

    // Turn on Current Limter
    mrrv10_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);

    // Release timer power-gate
    mrrv10_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    //mrrv10_r04.RO_EN_RO_LDO = 1;  //Use LDO for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv10_r04.as_int);
    delay(MBUS_DELAY);

	// Turn on timer
    mrrv10_r04.RO_RESET = 0;  //Release Reset TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv10_r04.as_int);
    delay(MBUS_DELAY);

    mrrv10_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv10_r04.as_int);
    delay(MBUS_DELAY);

    mrrv10_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv10_r04.as_int);

    // Release FSM Sleep
    mrrv10_r11.MRR_RAD_FSM_SLEEP = 0;  // Power on BB
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv10_r11.as_int);
	delay(MBUS_DELAY*5); // Freq stab

}

static void radio_power_off(){
	// Need to restore sleep pmu clock

	// Enable PMU ADC
	//pmu_adc_enable();

    // In case continuous mode was running
	mrrv10_r16.MRR_RAD_FSM_CONT_PULSE_MODEb = 1;
	mbus_remote_register_write(MRR_ADDR,0x16,mrrv10_r16.as_int);
        
    // Turn off FSM
    mrrv10_r03.MRR_TRX_ISOLATEN = 0;     //set ISOLATEN 0
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv10_r03.as_int);

    mrrv10_r11.MRR_RAD_FSM_EN = 0;  //Stop BB
    mrrv10_r11.MRR_RAD_FSM_RSTN = 0;  //RST BB
    mrrv10_r11.MRR_RAD_FSM_SLEEP = 1;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv10_r11.as_int);

    // Turn off Current Limter Briefly
    mrrv10_r00.MRR_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);

	// Current Limter set-up 
	mrrv10_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);

    // Turn on Current Limter
    mrrv10_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);

	// Turn off RO
    mrrv10_r04.RO_RESET = 1;  //Release Reset TIMER
    mrrv10_r04.RO_EN_CLK = 0; //Enable CLK TIMER
    mrrv10_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv10_r04.as_int);

	mrr_ldo_power_off();

    // Enable timer power-gate
    mrrv10_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
    //mrrv10_r04.RO_EN_RO_LDO = 0;  //Use LDO for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv10_r04.as_int);

    radio_on = 0;
	radio_ready = 0;

}

static void mrr_configure_pulse_width_long(){

    mrrv10_r12.MRR_RAD_FSM_TX_PW_LEN = 24; //100us PW
    mrrv10_r13.MRR_RAD_FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv10_r12.MRR_RAD_FSM_TX_PS_LEN = 49; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv10_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv10_r13.as_int);
}

/*
static void mrr_configure_pulse_width_long_2(){

    mrrv10_r12.MRR_RAD_FSM_TX_PW_LEN = 19; //80us PW
    mrrv10_r13.MRR_RAD_FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv10_r12.MRR_RAD_FSM_TX_PS_LEN = 39; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv10_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv10_r13.as_int);
}

static void mrr_configure_pulse_width_long_3(){

    mrrv10_r12.MRR_RAD_FSM_TX_PW_LEN = 9; //40us PW
    mrrv10_r13.MRR_RAD_FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv10_r12.MRR_RAD_FSM_TX_PS_LEN = 19; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv10_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv10_r13.as_int);
}

static void mrr_configure_pulse_width_short(){

    mrrv10_r12.MRR_RAD_FSM_TX_PW_LEN = 0; //4us PW
    mrrv10_r13.MRR_RAD_FSM_TX_C_LEN = 32; // (PW_LEN+1):C_LEN=1:32
    mrrv10_r12.MRR_RAD_FSM_TX_PS_LEN = 1; // PW=PS guard interval betwen 0 and 1 pulse

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv10_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv10_r13.as_int);
}
*/


static void send_radio_data_mrr_sub1(){

	// Use timer32 as timeout counter
	set_timer32_timeout(TIMER32_VAL);

    // Turn on Current Limter
    mrrv10_r00.MRR_CL_EN = 1;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);

    // Fire off data
	mrrv10_r11.MRR_RAD_FSM_EN = 1;  //Start BB
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv10_r11.as_int);

	// Wait for radio response
	WFI();
	stop_timer32_timeout_check(0x3);
	
    // Turn off Current Limter
    mrrv10_r00.MRR_CL_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);

	mrrv10_r11.MRR_RAD_FSM_EN = 0;
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv10_r11.as_int);
}

static void send_radio_data_mrr(uint32_t last_packet, uint8_t radio_packet_prefix, uint32_t radio_data){
	// Sends 192 bit packet, of which 96b is actual data
	// MRR REG_9: reserved for header
	// MRR REG_A: reserved for header
	// MRR REG_B: reserved for header
	// MRR REG_C: reserved for header
	// MRR REG_D: DATA[23:0]
	// MRR REG_E: DATA[47:24]
	// MRR REG_F: DATA[71:48]
	// MRR REG_10: DATA[95:72]

	// CRC16 Encoding 
    uint32_t* output_data;
    //output_data = crcEnc16(((radio_packet_count & 0xFF)<<8) | radio_packet_prefix, (radio_data_2 <<16) | ((radio_data_1 & 0xFFFFFF) >>8), (radio_data_1 << 24) | (radio_data_0 & 0xFFFFFF));
    output_data = crcEnc16(((read_data_batadc & 0xFF)<<8) | ((radio_packet_prefix & 0xF)<<4) | ((radio_packet_count>>16)&0xF), ((radio_packet_count & 0xFFFF)<<16) | (*REG_CHIP_ID & 0xFFFF), radio_data);

    mbus_remote_register_write(MRR_ADDR,0xD,radio_data & 0xFFFFFF);
    mbus_remote_register_write(MRR_ADDR,0xE,(*REG_CHIP_ID<<8)|(radio_data>>24));
    mbus_remote_register_write(MRR_ADDR,0xF,(radio_packet_prefix<<20)|(radio_packet_count&0xFFFFF));
    mbus_remote_register_write(MRR_ADDR,0x10,((output_data[2] & 0xFFFF)/*CRC16*/<<8)|(read_data_batadc&0xFF));

    if (!radio_ready){
		radio_ready = 1;

		// Release FSM Reset
		mrrv10_r11.MRR_RAD_FSM_RSTN = 1;  //UNRST BB
		mbus_remote_register_write(MRR_ADDR,0x11,mrrv10_r11.as_int);
		delay(MBUS_DELAY);

    	mrrv10_r03.MRR_TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    	mbus_remote_register_write(MRR_ADDR,0x03,mrrv10_r03.as_int);
		delay(MBUS_DELAY);

    }

	// Current Limter set-up 
	mrrv10_r00.MRR_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);

	uint32_t count = 0;
	uint32_t mrr_cfo_val_fine = 0;
	uint32_t num_packets = 1;
	if (mrr_freq_hopping) num_packets = mrr_freq_hopping;
	
	// New for mrrv10
	mrr_cfo_val_fine = mrr_cfo_val_fine_min;

	while (count < num_packets){
		#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xCE, mrr_cfo_val);
		#endif

		mrrv10_r01.MRR_TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
		mrrv10_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine;
		mbus_remote_register_write(MRR_ADDR,0x01,mrrv10_r01.as_int);
		send_radio_data_mrr_sub1();
		count++;
		if (count < num_packets){
			delay(RADIO_PACKET_DELAY);
		}
		mrr_cfo_val_fine = mrr_cfo_val_fine + mrr_freq_hopping_step; // 1: 0.8MHz, 2: 1.6MHz step
	}

	radio_packet_count++;

	if (last_packet){
		radio_power_off();
	}else{
		mrrv10_r11.MRR_RAD_FSM_EN = 0;
		mbus_remote_register_write(MRR_ADDR,0x11,mrrv10_r11.as_int);
	}
}

//***************************************************
// Temp Sensor Functions (SNTv4)
//***************************************************

static void temp_sensor_start(){
    sntv4_r01.TSNS_RESETn = 1;
    mbus_remote_register_write(SNT_ADDR,1,sntv4_r01.as_int);
}
static void temp_sensor_reset(){
    sntv4_r01.TSNS_RESETn = 0;
    mbus_remote_register_write(SNT_ADDR,1,sntv4_r01.as_int);
}
static void temp_sensor_power_on(){
    // Turn on digital block
    sntv4_r01.TSNS_SEL_LDO = 1;
    mbus_remote_register_write(SNT_ADDR,1,sntv4_r01.as_int);
    // Turn on analog block
    sntv4_r01.TSNS_EN_SENSOR_LDO = 1;
    mbus_remote_register_write(SNT_ADDR,1,sntv4_r01.as_int);

    delay(MBUS_DELAY);

    // Release isolation
    sntv4_r01.TSNS_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR,1,sntv4_r01.as_int);
}
static void temp_sensor_power_off(){
    sntv4_r01.TSNS_RESETn = 0;
    sntv4_r01.TSNS_SEL_LDO = 0;
    sntv4_r01.TSNS_EN_SENSOR_LDO = 0;
    sntv4_r01.TSNS_ISOLATE = 1;
    mbus_remote_register_write(SNT_ADDR,1,sntv4_r01.as_int);
}
static void sns_ldo_vref_on(){
    sntv4_r00.LDO_EN_VREF    = 1;
    mbus_remote_register_write(SNT_ADDR,0,sntv4_r00.as_int);
}

static void sns_ldo_power_on(){
    sntv4_r00.LDO_EN_IREF    = 1;
    sntv4_r00.LDO_EN_LDO    = 1;
    mbus_remote_register_write(SNT_ADDR,0,sntv4_r00.as_int);
}
static void sns_ldo_power_off(){
    sntv4_r00.LDO_EN_VREF    = 0;
    sntv4_r00.LDO_EN_IREF    = 0;
    sntv4_r00.LDO_EN_LDO    = 0;
    mbus_remote_register_write(SNT_ADDR,0,sntv4_r00.as_int);
}
static void snt_read_wup_counter(){
// WARNING: The read value might be corrupted due to asynchronous reading

	set_timer32_timeout(TIMER32_VAL);
    set_halt_until_mbus_rx();
    mbus_remote_register_read(SNT_ADDR,0x1B,1); // CNT_VALUE_EXT
    snt_wup_counter_cur = (*REG1 & 0xFF)<<24;
    mbus_remote_register_read(SNT_ADDR,0x1C,1); // CNT_VALUE
    snt_wup_counter_cur = snt_wup_counter_cur | (*REG1 & 0xFFFFFF);
    set_halt_until_mbus_tx();
	stop_timer32_timeout_check(0x4);

}
    
static void snt_start_timer_presleep(){

	// New for SNTv3
	sntv4_r08.TMR_SLEEP = 0x0; // Default : 0x1
	mbus_remote_register_write(SNT_ADDR,0x08,sntv4_r08.as_int);
	sntv4_r08.TMR_ISOLATE = 0x0; // Default : 0x1
	mbus_remote_register_write(SNT_ADDR,0x08,sntv4_r08.as_int);

    // TIMER SELF_EN Disable 
    sntv4_r09.TMR_SELF_EN = 0x0; // Default : 0x1
    mbus_remote_register_write(SNT_ADDR,0x09,sntv4_r09.as_int);

    // EN_OSC 
    sntv4_r08.TMR_EN_OSC = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv4_r08.as_int);

    // Release Reset 
    sntv4_r08.TMR_RESETB = 0x1; // Default : 0x0
    sntv4_r08.TMR_RESETB_DIV = 0x1; // Default : 0x0
    sntv4_r08.TMR_RESETB_DCDC = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv4_r08.as_int);

    // TIMER EN_SEL_CLK Reset 
    sntv4_r08.TMR_EN_SELF_CLK = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv4_r08.as_int);

    // TIMER SELF_EN 
    sntv4_r09.TMR_SELF_EN = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x09,sntv4_r09.as_int);
    //delay(100000); 

    snt_timer_enabled = 1;
}

static void snt_start_timer_postsleep(){
    // Turn off sloscillator
    sntv4_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv4_r08.as_int);
}


static void snt_stop_timer(){

    // EN_OSC
    sntv4_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    // RESET
    sntv4_r08.TMR_EN_SELF_CLK = 0x0; // Default : 0x0
    sntv4_r08.TMR_RESETB = 0x0;// Default : 0x0
    sntv4_r08.TMR_RESETB_DIV = 0x0; // Default : 0x0
    sntv4_r08.TMR_RESETB_DCDC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv4_r08.as_int);
    snt_timer_enabled = 0;

    sntv4_r17.WUP_ENABLE = 0x0; // Default : 0x
    mbus_remote_register_write(SNT_ADDR,0x17,sntv4_r17.as_int);

	// New for SNTv3
	sntv4_r08.TMR_SLEEP = 0x1; // Default : 0x1
	sntv4_r08.TMR_ISOLATE = 0x1; // Default : 0x1
	mbus_remote_register_write(SNT_ADDR,0x08,sntv4_r08.as_int);

}

static void snt_set_wup_timer(uint32_t sleep_count){
    
    snt_wup_counter_cur = sleep_count + snt_wup_counter_cur; // should handle rollover

    mbus_remote_register_write(SNT_ADDR,0x19,snt_wup_counter_cur>>24);
    mbus_remote_register_write(SNT_ADDR,0x1A,snt_wup_counter_cur & 0xFFFFFF);
    
    sntv4_r17.WUP_ENABLE = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x17,sntv4_r17.as_int);

}

static void snt_reset_and_restart_timer(){
	
    sntv4_r17.WUP_ENABLE = 0x0;
    mbus_remote_register_write(SNT_ADDR,0x17,sntv4_r17.as_int);
	delay(MBUS_DELAY);
    sntv4_r17.WUP_ENABLE = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x17,sntv4_r17.as_int);
}

static void snt_set_timer_threshold(uint32_t sleep_count){
    
    mbus_remote_register_write(SNT_ADDR,0x19,sleep_count>>24);
    mbus_remote_register_write(SNT_ADDR,0x1A,sleep_count & 0xFFFFFF);
    
}


//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sns_sleep_check(void){
    // Make sure LDO is off
    if (sns_running){
        sns_running = 0;
        temp_sensor_power_off();
        sns_ldo_power_off();
    }
	if (pmu_setting_state != PMU_25C){
		// Set PMU to room temp setting
		pmu_setting_state = PMU_25C;
		pmu_active_setting_temp_based();
		pmu_sleep_setting_temp_based();
	}
}

static void operation_sleep(void){

	// Reset GOC_DATA_IRQ
	*GOC_DATA_IRQ = 0;

    // Go to Sleep
    mbus_sleep_all();
    while(1);

}

static void operation_sleep_snt_timer(void){

    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;

    // Disable PRC Timer
    set_wakeup_timer(0, 0, 0);

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
    
    // Make sure the irq counter is reset    
    exec_count_irq = 0;

    operation_sns_sleep_check();

    // Make sure Radio is off
    if (radio_on){
        radio_power_off();
    }

    // Make sure SNT timer is off
    if (snt_timer_enabled){
        snt_stop_timer();
    }

    // Disable Timer
    set_wakeup_timer(0, 0, 0);

    // Go to sleep
    operation_sleep();

}


uint32_t dumb_divide(uint32_t nu, uint32_t de) {
// Returns quotient of nu/de

    uint32_t temp = 1;
    uint32_t quotient = 0;

    while (de <= nu) {
        de <<= 1;
        temp <<= 1;
    }

    //printf("%d %d\n",de,temp,nu);
    while (temp > 1) {
        de >>= 1;
        temp >>= 1;

        if (nu >= de) {
            nu -= de;
            //printf("%d %d\n",quotient,temp);
            quotient += temp;
        }
    }

    return quotient;
}

static void operation_init(void){
  
	// Set CPU & Mbus Clock Speeds
    prev20_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prev20_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prev20_r0B.CLK_GEN_DIV_CORE = 0x2; // Default 0x3
    prev20_r0B.GOC_CLK_GEN_SEL_FREQ = 0x5; // Default 0x6
    prev20_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
    prev20_r0B.GOC_SEL = 0xF; // Default 0x8
	*REG_CLKGEN_TUNE = prev20_r0B.as_int;

    prev20_r1C.SRAM0_TUNE_DECODER_DLY = 0x10; // Default 0x10, 5 bits
    prev20_r1C.SRAM0_USE_INVERTER_SA = 0;  // Default 0
	*REG_SRAM0_TUNE = prev20_r1C.as_int;
  
  
    //Enumerate & Initialize Registers
    stack_state = STK_IDLE; 	//0x0;
    enumerated = 0x5453104C; // 0x5453 is TS in ascii
    exec_count = 0;
    wakeup_count = 0;
    exec_count_irq = 0;
	PMU_ADC_3P0_VAL = 0x62;
	pmu_parking_mode = 3;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
//    set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(SNT_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(MRR_ADDR);
	delay(MBUS_DELAY);
 	mbus_enumerate(PMU_ADDR);
	delay(MBUS_DELAY);

    // Set CPU Halt Option as TX --> Use for register write e.g.
	//    set_halt_until_mbus_tx();

	// PMU Settings ----------------------------------------------
	pmu_set_clk_init();
	pmu_reset_solar_short();

	// New for PMUv9
	// VBAT_READ_TRIM Register
    pmu_reg_write(0x45,
		( (0x0 << 9) // 0x0: no mon; 0x1: sar conv mon; 0x2: up conv mon; 0x3: down conv mon
		| (0x0 << 8) // 1: vbat_read_mode enable; 0: vbat_read_mode disable
		| (0x48 << 0) //sampling multiplication factor N; vbat_read out = vbat/1p2*N
	));

	// Disable PMU ADC measurement in active mode
	// PMU_CONTROLLER_STALL_ACTIVE
	// Updated for PMUv9
    pmu_reg_write(0x3A, 
		( (1 << 20) // ignore state_horizon; default 1
		| (0 << 19) // state_vbat_read
		| (1 << 13) // ignore adc_output_ready; default 0
		| (1 << 12) // ignore adc_output_ready; default 0
		| (1 << 11) // ignore adc_output_ready; default 0
	));

	pmu_adc_reset_setting();
	pmu_adc_enable();

    // Temp Sensor Settings --------------------------------------
    sntv4_r01.TSNS_RESETn = 0;
    sntv4_r01.TSNS_EN_IRQ = 1;
    sntv4_r01.TSNS_BURST_MODE = 0;
    sntv4_r01.TSNS_CONT_MODE = 0;
    mbus_remote_register_write(SNT_ADDR,1,sntv4_r01.as_int);

    // Set temp sensor conversion time
    sntv4_r03.TSNS_SEL_STB_TIME = 0x1; 
    sntv4_r03.TSNS_SEL_CONV_TIME = 0x6; // Default: 0x6
    mbus_remote_register_write(SNT_ADDR,0x03,sntv4_r03.as_int);

    // SNT Wakeup Timer Settings --------------------------------------
    // Config Register A
    sntv4_r0A.TMR_S = 0x1; // Default: 0x4, use 1 for good TC
    // Tune R for TC
    sntv4_r0A.TMR_DIFF_CON = 0x3FEF; // Default: 0x3FFB
    mbus_remote_register_write(SNT_ADDR,0x0A,sntv4_r0A.as_int);

    // TIMER CAP_TUNE  
    // Tune C for freq
    sntv4_r09.TMR_SEL_CAP = 0x80; // Default : 8'h8
    sntv4_r09.TMR_SEL_DCAP = 0x3F; // Default : 6'h4
    mbus_remote_register_write(SNT_ADDR,0x09,sntv4_r09.as_int);

    // Wakeup Counter
    sntv4_r17.WUP_CLK_SEL = 0x0; 
    sntv4_r17.WUP_AUTO_RESET = 0x0; // Automatically reset counter to 0 upon sleep 
    mbus_remote_register_write(SNT_ADDR,0x17,sntv4_r17.as_int);

    // MRR Settings --------------------------------------
	// Reqruied in MRRv10 to run like MRRv7
	mrrv10_r21.MRR_TRX_ENb_CONT_RC = 0;  //RX_Decap P 
	mbus_remote_register_write(MRR_ADDR,0x21,mrrv10_r21.as_int);

	// Decap in series
	mrrv10_r03.MRR_DCP_P_OW = 0;  //RX_Decap P 
	mbus_remote_register_write(MRR_ADDR,3,mrrv10_r03.as_int);
	mrrv10_r03.MRR_DCP_S_OW = 1;  //TX_Decap S (forced charge decaps)
	mbus_remote_register_write(MRR_ADDR,3,mrrv10_r03.as_int);

	// Current Limter set-up 
	mrrv10_r00.MRR_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);

    // Turn on Current Limter
    mrrv10_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);

	// Wait for charging decap
   	config_timerwd(TIMERWD_VAL);
	*REG_MBUS_WD = 1500000*3; // default: 1500000
	delay(MBUS_DELAY*200); // Wait for decap to charge

	mrrv10_r1F.LC_CLK_RING = 0x3;  // ~ 150 kHz
	mrrv10_r1F.LC_CLK_DIV = 0x3;  // ~ 150 kHz
	mbus_remote_register_write(MRR_ADDR,0x1F,mrrv10_r1F.as_int);

	//mrr_configure_pulse_width_short();
	mrr_configure_pulse_width_long();

	mrr_freq_hopping = 5;
	mrr_freq_hopping_step = 4;

	mrr_cfo_val_fine_min = 0x0000;

	// RO setup (SFO)
	// Adjust Diffusion R
	mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF

	// Adjust Poly R
	mbus_remote_register_write(MRR_ADDR,0x08,0x400000); // RO_POLY

	// Adjust C
	mrrv10_r07.RO_MOM = 0x10;
	mrrv10_r07.RO_MIM = 0x10;
	mbus_remote_register_write(MRR_ADDR,0x07,mrrv10_r07.as_int);

	// TX Setup Carrier Freq
	mrrv10_r00.MRR_TRX_CAP_ANTP_TUNE_COARSE = 0x0;  //ANT CAP 10b unary 830.5 MHz
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);
	mrrv10_r01.MRR_TRX_CAP_ANTN_TUNE_COARSE = 0x0; //ANT CAP 10b unary 830.5 MHz
	mrrv10_r01.MRR_TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
	mrrv10_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
	mbus_remote_register_write(MRR_ADDR,0x01,mrrv10_r01.as_int);
	mrrv10_r02.MRR_TX_BIAS_TUNE = 0x7FF;  //Set TX BIAS TUNE 13b // Max 0x1FFF
	mbus_remote_register_write(MRR_ADDR,0x02,mrrv10_r02.as_int);

	// Turn off RX mode
    mrrv10_r03.MRR_TRX_MODE_EN = 0; //Set TRX mode
	mbus_remote_register_write(MRR_ADDR,3,mrrv10_r03.as_int);

    mrrv10_r14.MRR_RAD_FSM_TX_POWERON_LEN = 0; //3bits
	mrrv10_r15.MRR_RAD_FSM_RX_HDR_BITS = 0x00;  //Set RX header
	mrrv10_r15.MRR_RAD_FSM_RX_HDR_TH = 0x00;    //Set RX header threshold
	mrrv10_r15.MRR_RAD_FSM_RX_DATA_BITS = 0x00; //Set RX data 1b
	mbus_remote_register_write(MRR_ADDR,0x14,mrrv10_r14.as_int);
	mbus_remote_register_write(MRR_ADDR,0x15,mrrv10_r15.as_int);

	// RAD_FSM set-up 
	// Using first 48 bits of data as header
	mbus_remote_register_write(MRR_ADDR,0x09,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0A,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0B,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0C,0x7AC800);
	mrrv10_r11.MRR_RAD_FSM_TX_H_LEN = 0; //31-31b header (max)
	mrrv10_r11.MRR_RAD_FSM_TX_D_LEN = RADIO_DATA_LENGTH; //0-skip tx data
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv10_r11.as_int);

	mrrv10_r13.MRR_RAD_FSM_TX_MODE = 3; //code rate 0:4 1:3 2:2 3:1(baseline) 4:1/2 5:1/3 6:1/4
	mbus_remote_register_write(MRR_ADDR,0x13,mrrv10_r13.as_int);

    mrrv10_r04.LDO_SEL_VOUT = 4; // New for MRRv10
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv10_r04.as_int);
	// Mbus return address
	mbus_remote_register_write(MRR_ADDR,0x1E,0x1002);

	// Additional delay for charging decap
   	config_timerwd(TIMERWD_VAL);
	*REG_MBUS_WD = 1500000; // default: 1500000
	delay(MBUS_DELAY*200); // Wait for decap to charge

    // Initialize other global variables
    sns_running = 0;
    radio_ready = 0;
    radio_on = 0;
	wakeup_data = 0;
	RADIO_PACKET_DELAY = 500;
	radio_packet_count = 0;
	error_code = 0;
	
    PMU_10C_threshold_sns = 600; // Around 10C
    PMU_15C_threshold_sns = 800; 
    PMU_20C_threshold_sns = 1000; // Around 20C
    PMU_25C_threshold_sns = 1300; // Around 20C
    PMU_30C_threshold_sns = 1600; 
    PMU_35C_threshold_sns = 1900; // Around 35C
    PMU_40C_threshold_sns = 2200;
    PMU_45C_threshold_sns = 2500;
    PMU_55C_threshold_sns = 3200; // Around 55C
    PMU_65C_threshold_sns = 4500; 
    PMU_75C_threshold_sns = 7000; // Around 75C
    PMU_85C_threshold_sns = 9400; 
    PMU_95C_threshold_sns = 12000; // Around 95C

    SNT_0P5S_VAL = 1000;
    TEMP_CALIB_A = 240000;
    TEMP_CALIB_B = 20000; // (B-3400)*100


    // Go to sleep without timer
    operation_sleep_notimer();
}

//***************************************************
// Temperature measurement operation
//***************************************************
static void operation_sns_run(void){

    if (stack_state == STK_IDLE){
        stack_state = STK_LDO;

        wfi_timeout_flag = 0;

        // Turn on SNS LDO VREF; requires settling
        sns_ldo_vref_on();

    }else if (stack_state == STK_LDO){
        stack_state = STK_TEMP_START;

        // Power on SNS LDO
        sns_ldo_power_on();

        // Power on temp sensor
        temp_sensor_power_on();
        delay(MBUS_DELAY);

    }else if (stack_state == STK_TEMP_START){
        // Start temp measurement
        stack_state = STK_TEMP_READ;
        pmu_set_sleep_tsns();
        temp_sensor_reset();
        temp_sensor_start();
		// Go to sleep during measurement
		operation_sleep();

    }else if (stack_state == STK_TEMP_READ){

        // Grab Temp Sensor Data
        if (wfi_timeout_flag){
            mbus_write_message32(0xFA, 0xFAFAFAFA);
        }else{
            // Read register
            set_halt_until_mbus_rx();
            mbus_remote_register_read(SNT_ADDR,0x6,1);
            read_data_temp = *REG1;
            set_halt_until_mbus_tx();
        }
        meas_count++;

        // Last measurement from this wakeup
        if (meas_count == NUM_TEMP_MEAS){
            // No error; see if there was a timeout
            if (wfi_timeout_flag){
                temp_storage_latest = 0x666;
                wfi_timeout_flag = 0;
                // In case of timeout, wakeup counter needs to be adjusted 
                snt_read_wup_counter();
            }else{
                temp_storage_latest = read_data_temp;

            }
        }

        // Option to take multiple measurements per wakeup
        if (meas_count < NUM_TEMP_MEAS){    
            // Repeat measurement while awake
            stack_state = STK_TEMP_START;
                
        }else{
            meas_count = 0;

            // Read latest PMU ADC measurement
            pmu_adc_read_latest();
        
			uint32_t pmu_setting_prev = pmu_setting_state;
            // Change PMU based on temp
            if (temp_storage_latest > PMU_95C_threshold_sns){
                pmu_setting_state = PMU_95C;
            }else if (temp_storage_latest > PMU_85C_threshold_sns){
                pmu_setting_state = PMU_85C;
            }else if (temp_storage_latest > PMU_75C_threshold_sns){
                pmu_setting_state = PMU_75C;
            }else if (temp_storage_latest > PMU_65C_threshold_sns){
                pmu_setting_state = PMU_65C;
            }else if (temp_storage_latest > PMU_55C_threshold_sns){
                pmu_setting_state = PMU_55C;
            }else if (temp_storage_latest > PMU_45C_threshold_sns){
                pmu_setting_state = PMU_45C;
            }else if (temp_storage_latest > PMU_40C_threshold_sns){
                pmu_setting_state = PMU_40C;
            }else if (temp_storage_latest > PMU_35C_threshold_sns){
                pmu_setting_state = PMU_35C;
            }else if (temp_storage_latest > PMU_30C_threshold_sns){
                pmu_setting_state = PMU_30C;
            }else if (temp_storage_latest > PMU_25C_threshold_sns){
                pmu_setting_state = PMU_25C;
            }else if (temp_storage_latest > PMU_20C_threshold_sns){
                pmu_setting_state = PMU_20C;
            }else if (temp_storage_latest > PMU_15C_threshold_sns){
                pmu_setting_state = PMU_15C;
            }else if (temp_storage_latest > PMU_10C_threshold_sns){
                pmu_setting_state = PMU_10C;
            }else{
                pmu_setting_state = PMU_0C;
            }

			// Always restore sleep setting from higher pmu meas setting
   	        pmu_sleep_setting_temp_based();

			if (pmu_setting_prev != pmu_setting_state){
	            pmu_active_setting_temp_based();
			}

			// Power on radio
			radio_power_on();

            // Assert temp sensor isolation & turn off temp sensor power
            temp_sensor_power_off();
            sns_ldo_power_off();
            stack_state = STK_IDLE;

            #ifdef DEBUG_MBUS_MSG_1
            mbus_write_message32(0xCC, exec_count);
            #endif
            mbus_write_message32(0xC0, (exec_count << 16) | temp_storage_latest);

            if (temp_storage_debug){
                temp_storage_latest = exec_count;
            }

            // transmit the data
            send_radio_data_mrr(1,0x1,(TEMP_CALIB_B<<16) | (temp_storage_latest&0xFFFF));

            exec_count++;

            // Make sure Radio is off
            if (radio_on){
                radio_ready = 0;
                radio_power_off();
            }

			// Use SNT Timer    
			snt_set_wup_timer(WAKEUP_PERIOD_SNT);
			operation_sleep_snt_timer();

        }

    }else{
        //default:  // THIS SHOULD NOT HAPPEN
        // Reset Temp Sensor 
        temp_sensor_power_off();
        sns_ldo_power_off();
        operation_sleep_notimer();
    }

}


static void operation_goc_trigger_init(void){

	// This is critical
	set_halt_until_mbus_tx();

	// Debug
	//mbus_write_message32(0xAA,0xABCD1234);
	//mbus_write_message32(0xAA,wakeup_data);

	// Initialize variables & registers
    if (sns_running){
        sns_running = 0;
        temp_sensor_power_off();
        sns_ldo_power_off();
		// Restore sleep setting
   	    pmu_sleep_setting_temp_based();
    }
	stack_state = STK_IDLE;
	
	radio_power_off();
}

static void operation_goc_trigger_radio(uint32_t radio_tx_num, uint32_t wakeup_timer_val, uint8_t radio_tx_prefix, uint32_t radio_tx_data){

	// Prepare radio TX
	radio_power_on();
	exec_count_irq++;
	// radio
	send_radio_data_mrr(1,radio_tx_prefix,radio_tx_data);	

	if (exec_count_irq < radio_tx_num){
		// set timer
		set_wakeup_timer (wakeup_timer_val, 0x1, 0x1);
		// go to sleep and wake up with same condition
		operation_sleep_noirqreset();
		
	}else{
		exec_count_irq = 0;
		// Go to sleep without timer
		operation_sleep_notimer();
	}
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(){

    // Only enable relevant interrupts (PRCv17)
	//enable_reg_irq();
	//enable_all_irq();
	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);
  
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);

	wakeup_count++;

	// Figure out who triggered wakeup
	if(*SREG_WAKEUP_SOURCE & 0x00000008){
		// Debug
		#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xAA,0x11331133);
		#endif
	}

	// Record previous sleep time
	//sleep_time_prev = *REG_WUPT_VAL;
	// Debug
	#ifdef DEBUG_MBUS_MSG
	mbus_write_message32(0xAB, sleep_time_prev);
	#endif

    // Initialization sequence
    if (enumerated != 0x5453104C){
        operation_init();
    }

	// Read latest batt voltage
	pmu_adc_read_latest();

    // Check if wakeup is due to GOC interrupt  
    // 0x8C is reserved for GOC-triggered wakeup (Named GOC_DATA_IRQ)
    // 8 MSB bits of the wakeup data are used for function ID
    wakeup_data = *GOC_DATA_IRQ;
    uint32_t wakeup_data_header = (wakeup_data>>24) & 0xFF;
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;
    uint32_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
    uint32_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;

	// In case GOC triggered in the middle of routines
	if ((wakeup_data_header != 0) && (exec_count_irq == 0)){
		operation_goc_trigger_init();
	}

    if(wakeup_data_header == 1){
        // Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the MSB of # of transmissions
		operation_goc_trigger_radio(wakeup_data_field_0 + (wakeup_data_field_2<<8), wakeup_data_field_1, 0x4, exec_count_irq);

    }else if(wakeup_data_header == 0x32){
        // wakeup_data[15:0] is the desired wakeup period in 0.5s (min 0.5s: 0x1, max 32767.5s: 0xFFFF)
        // wakeup_data[20] enables radio tx for each measurement
        // wakeup_data[21] specifies which wakeup timer to use 0: SNT timer, 1: PRC timer
        // wakeup_data[23] is debug mode: logs the execution count as opposed to temp data
        temp_storage_debug = wakeup_data_field_2 & 0x80;
        
		// Use SNT timer
		WAKEUP_PERIOD_SNT = ((wakeup_data & 0xFFFF)<<1)*SNT_0P5S_VAL; // Unit is 0.5s

        exec_count_irq++;

        if (exec_count_irq == 1){
            // SNT pulls higher current in the beginning
            pmu_set_sleep_radio();
            snt_start_timer_presleep();
            // Go to sleep for >3s for timer stabilization
            set_wakeup_timer (WAKEUP_PERIOD_RADIO_INIT*2, 0x1, 0x1);
            operation_sleep_noirqreset();
        }else if (exec_count_irq == 2){
            snt_start_timer_postsleep();
            // Read existing counter value; in case not reset to zero
            snt_read_wup_counter();
            // Restore sleep setting to low
            pmu_set_sleep_low();
        }

		// Prepare for Radio TX
		radio_power_on();

		// Starting Operation
		send_radio_data_mrr(1,0x5,0x0);	

        sns_running = 1;
        exec_count = 0;
        meas_count = 0;
        temp_storage_count = 0;

        // Reset GOC_DATA_IRQ
        *GOC_DATA_IRQ = 0;
        exec_count_irq = 0;

        // Run Temp Sensor Program
    	stack_state = STK_IDLE;
        operation_sns_run();

    }else if(wakeup_data_header == 0x33){
        // Stop temp sensor program and transmit the battery reading and execution count (alternating n times)
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 

        operation_sns_sleep_check();
        snt_stop_timer();

        stack_state = STK_IDLE;

        operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0x6, exec_count);

    }else if(wakeup_data_header == 0x13){
    	// Tune SNT Timer R for TC
        sntv4_r0A.TMR_DIFF_CON = wakeup_data & 0x3FFF; // Default: 0x3FFB
        mbus_remote_register_write(SNT_ADDR,0x0A,sntv4_r0A.as_int);

    }else if(wakeup_data_header == 0x14){
        // Update SNT wakeup counter value for 0.5s
        SNT_0P5S_VAL = wakeup_data & 0xFFFF;
        if (SNT_0P5S_VAL == 0){
            SNT_0P5S_VAL = 1000;
        }        

    }else if(wakeup_data_header == 0x15){
        // Update GOC clock
		prev20_r0B.GOC_CLK_GEN_SEL_FREQ = (wakeup_data >> 4)&0x7; // Default 0x0
		prev20_r0B.GOC_CLK_GEN_SEL_DIV = wakeup_data & 0x3; // Default 0x6
		*REG_CLKGEN_TUNE = prev20_r0B.as_int;

    }else if(wakeup_data_header == 0x17){
		// Change the 3.0V battery reference
		if (wakeup_data_field_0 == 0){
			// Update with the current value
			PMU_ADC_3P0_VAL = read_data_batadc;
		}else{
			PMU_ADC_3P0_VAL = wakeup_data_field_0;
		}

	}else if(wakeup_data_header == 0x18){
		// Manually override the SAR ratio
		pmu_set_sar_override(wakeup_data_field_0);

	}else if(wakeup_data_header == 0x20){
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[23:8] is the desired chip id value
		//					-->  if zero: reports current chip id
		
		uint32_t chip_id_user;
		chip_id_user = (wakeup_data>>8) & 0xFFFF;

		if (chip_id_user == 0){
			chip_id_user = *REG_CHIP_ID;
		}else{
			*REG_CHIP_ID = chip_id_user;
		}

		operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xA, chip_id_user);


	}else if(wakeup_data_header == 0x21){
		// Change the radio tx packet delay
		uint32_t user_val = wakeup_data & 0xFFFFFF;
		if (user_val < 500){
			RADIO_PACKET_DELAY = 4000;
		}else{
			RADIO_PACKET_DELAY = user_val;
		}

	}else if(wakeup_data_header == 0x22){
		// Change the carrier frequency of MRR (CFO)
		// Updated for MRRv6
		// wakeup_data[15:0]: Fine+Coarse setting
		// wakeup_data[23:16]: Turn on/off freq hopping 

		mrr_freq_hopping = wakeup_data_field_2 & 0xF;
		mrr_freq_hopping_step = wakeup_data_field_2 >> 4;

		mrr_cfo_val_fine_min = (wakeup_data >> 10) & 0x3F; // 6 bit
	
		mrrv10_r00.MRR_TRX_CAP_ANTP_TUNE_COARSE = wakeup_data & 0x3FF; // 10 bit coarse setting 
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);
		mrrv10_r01.MRR_TRX_CAP_ANTN_TUNE_COARSE = wakeup_data & 0x3FF; // 10 bit coarse setting

		mrrv10_r01.MRR_TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min; 
		mrrv10_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min;
		mbus_remote_register_write(MRR_ADDR,0x01,mrrv10_r01.as_int);

	}else if(wakeup_data_header == 0x23){
		// Change the baseband frequency of MRR (SFO)
		mrrv10_r07.RO_MOM = wakeup_data & 0x3F;
		mrrv10_r07.RO_MIM = wakeup_data & 0x3F;
		mbus_remote_register_write(MRR_ADDR,0x07,mrrv10_r07.as_int);
		
	}else if(wakeup_data_header == 0x25){
		// Change the # of hops only
		mrr_freq_hopping = wakeup_data_field_2 & 0xF;
		mrr_freq_hopping_step = wakeup_data_field_2 >> 4;

/*
	}else if(wakeup_data_header == 0x24){
		// Switch between short / long pulse
		if (wakeup_data_field_0 == 0x1) {
			mrr_configure_pulse_width_long_3();
			
		}else if (wakeup_data_field_0 == 0x2){
			mrr_configure_pulse_width_long_2();
			
		}else if (wakeup_data_field_0 == 0x3){
			mrr_configure_pulse_width_long();

		}else if (wakeup_data_field_0 == 0x0){
			mrr_configure_pulse_width_short();
		}
		// Go to sleep without timer
		operation_sleep_notimer();
*/
	}else if(wakeup_data_header == 0x26){

		mrrv10_r02.MRR_TX_BIAS_TUNE = wakeup_data & 0x1FFF;  //Set TX BIAS TUNE 13b // Set to max
		mbus_remote_register_write(MRR_ADDR,0x02,mrrv10_r02.as_int);

	}else if(wakeup_data_header == 0x27){

		mrrv10_r04.LDO_SEL_VOUT = wakeup_data & 0x7;
		mbus_remote_register_write(MRR_ADDR,0x04,mrrv10_r04.as_int);

    }else if(wakeup_data_header == 0x2A){
        // Update calibration coefficient A
        // A is the slope, typical value is around 24.000, stored as A*1000
        TEMP_CALIB_A = wakeup_data & 0xFFFFFF; 

    }else if(wakeup_data_header == 0x2B){
        // Update calibration coefficient B 
        // B is the offset, typical value is around -3750.000, stored as -(B-3400)*100 due to limitations on radio bitfield
        TEMP_CALIB_B = wakeup_data & 0xFFFF; 

    }else if(wakeup_data_header == 0x51){
		// Debug trigger for MRR testing; repeat trigger 1 for 0xFFFFFFFF times
		operation_goc_trigger_radio(0xFFFFFFFF, wakeup_data_field_1, 0x4, exec_count_irq);

    }else if(wakeup_data_header == 0x52){
		// Burst mode for MRR radio scanning
		// Prepare radio TX
		disable_timerwd();
		radio_power_on();
		uint32_t mrr_freq_hopping_saved = mrr_freq_hopping;
		mrr_freq_hopping = 0;
		uint32_t ii = 0;
		// Packet Loop 
		while (ii < (wakeup_data & 0xFFFFFF)){
			send_radio_data_mrr(0,0x4,ii);	
			ii++;
		}
			send_radio_data_mrr(1,0x4,ii);	

		mrr_freq_hopping = mrr_freq_hopping_saved;
		operation_sleep_notimer();

    }else if(wakeup_data_header == 0x53){
		// New FSM burst mode for MRR radio scanning
        // wakeup_data[15:0] = Test duration in minutes
		disable_timerwd();
		*MBCWD_RESET = 1;
		radio_power_on();

		// Release FSM Reset
		mrrv10_r11.MRR_RAD_FSM_RSTN = 1;  //UNRST BB
		mbus_remote_register_write(MRR_ADDR,0x11,mrrv10_r11.as_int);
		delay(MBUS_DELAY);

    	mrrv10_r03.MRR_TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    	mbus_remote_register_write(MRR_ADDR,0x03,mrrv10_r03.as_int);
		delay(MBUS_DELAY);

		// Current Limter set-up 
		mrrv10_r00.MRR_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);

        // Turn on Current Limter
        mrrv10_r00.MRR_CL_EN = 1;
        mbus_remote_register_write(MRR_ADDR,0x00,mrrv10_r00.as_int);

        // Enable FSM Continuous Mode
		mrrv10_r16.MRR_RAD_FSM_CONT_PULSE_MODEb = 0;
		mbus_remote_register_write(MRR_ADDR,0x16,mrrv10_r16.as_int);
        
        // Fire off data
    	mrrv10_r11.MRR_RAD_FSM_EN = 1;  //Start BB
    	mbus_remote_register_write(MRR_ADDR,0x11,mrrv10_r11.as_int);

        if ((wakeup_data & 0xFFFF) == 0){
            while(1);
        }else{
            if ((wakeup_data & 0xFFFF) > 340){ // Max count is roughly 5.6 hours
    	        set_timer32_timeout(0xFFFFFFFF);
            }else{
    	        set_timer32_timeout(0xC00000*(wakeup_data & 0xFFFF));
            }
            WFI();
            radio_power_off();
        }

	}else if(wakeup_data_header == 0xF0){
		// Report firmware version

		operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xB, enumerated);

	}else if(wakeup_data_header == 0xF1){
		// Report PRC's Chip ID (Serial Number)
		uint32_t puf_chip_id = 0;
		// Power Up PUF
		*REG_SYS_CONF = (0x0/*PUF_SLEEP*/ << 6) | (0x1/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);
		// Wait (~20ms)
		delay(MBUS_DELAY*4);
		// Release Isolation
		*REG_SYS_CONF = (0x0/*PUF_SLEEP*/ << 6) | (0x0/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);
		// Store the Chip ID
		puf_chip_id = *REG_PUF_CHIP_ID;
		// Power-Off PUF
		*REG_SYS_CONF = (0x1/*PUF_SLEEP*/ << 6) | (0x1/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);

		operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xC, puf_chip_id);

	}else if(wakeup_data_header == 0xFA){

		// Extra protection
		if ((wakeup_data&0xFFFFFF) == 0x89D7E2){
			config_timerwd(0xFF);
			while(1){
				mbus_write_message32(0xE0, 0x0);
				delay(MBUS_DELAY);
			}
		}

    }else{
		if (wakeup_data_header != 0){
			// Invalid GOC trigger
            // Go to sleep without timer
            operation_sleep_notimer();
		}
	}

	if (sns_running){
		// Proceed to continuous mode
		while(1){
			operation_sns_run();
		}
	}

	operation_sleep_notimer();

    while(1);
}


