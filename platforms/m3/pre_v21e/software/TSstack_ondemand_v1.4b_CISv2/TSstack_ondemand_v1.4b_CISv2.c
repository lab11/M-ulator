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
//			v1.4b_CISv2: Converted v1.4b to work with CISv2
//*******************************************************************
  
//[CISv2] Header File Version Change
//-----------------------------------------------
// PREv20  -> PREv21E
// PMUv7   -> PMUv12
// SNTv4   -> SNTv5
// MRRv10A -> MRRv11A
//-----------------------------------------------
//
// PMU State Name Changes
//  This is only for reference purpose;
//  The state name changes do not affect the code itself.
//  -----------------------------------------------------------------------------------------
//      PMUv11 and prior versions                  PMUv12 Family
//  -----------------------------------------------------------------------------------------
//      state_sar_scn_on                        -> sar_on
//      state_wait_for_clock_cycles             -> wait_for_clock_cycles
//      state_wait_for_time                     -> wait_for_cap_charge (only used in PMUv12C)
//      state_sar_scn_reset                     -> sar_reset
//      state_sar_scn_stabilized                -> sar_stabilized
//      state_sar_scn_ratio_roughly_adjusted    -> sar_ratio_roughly_adjusted
//      state_clock_supply_switched             -> clock_supply_switched
//      state_control_supply_switched           -> control_supply_switched
//      state_upconverter_on                    -> upc_on
//      state_upconverter_stabilized            -> upc_stabilized
//      state_refgen_on                         -> refgen_on
//      state_adc_output_ready                  -> adc_output_ready
//      state_adc_adjusted                      -> adc_adjusted
//      state_sar_scn_ratio_adjusted            -> sar_ratio_adjusted
//      state_downconverter_on                  -> dnc_on
//      state_downconverter_stabilized          -> dnc_stabilized
//      state_vdd_3p6_turned_on                 -> vdd_3p6_turned_on
//      state_vdd_1p2_turned_on                 -> vdd_1p2_turned_on
//      state_vdd_0P6_turned_on                 -> vdd_0p6_turned_on
//      state_vbat_read                         -> vbat_read_only
//      state_state_horizon                     -> horizon
//  -----------------------------------------------------------------------------------------
//
// MRR Register Name Changes
//  -------------------------------------------------------------------------------------------------------
//      REG     MRRv10A                            MRRv11A
//  -------------------------------------------------------------------------------------------------------
//      0x00    MRR_CL_EN                       -> TRX_CL_EN
//      0x00    MRR_CL_CTRL                     -> TRX_CL_CTRL
//      0x00    MRR_TRX_CAP_ANTP_TUNE_COARSE    -> TRX_CAP_ANTP_TUNE_COARSE
//      0x01    MRR_TRX_CAP_ANTN_TUNE_COARSE    -> TRX_CAP_ANTN_TUNE_COARSE
//      0x01    MRR_TRX_CAP_ANTN_TUNE_FINE      -> TRX_CAP_ANTN_TUNE_FINE
//      0x01    MRR_TRX_CAP_ANTP_TUNE_FINE      -> TRX_CAP_ANTP_TUNE_FINE
//      0x02    MRR_TX_BIAS_TUNE                -> TX_BIAS_TUNE
//      0x03    MRR_TRX_ISOLATEN                -> TRX_ISOLATEN
//      0x03    MRR_TRX_MODE_EN                 -> TRX_MODE_EN
//      0x03    MRR_DCP_S_OW                    -> TRX_DCP_S_OW1, TRX_DCP_S_OW2
//      0x03    MRR_DCP_P_OW                    -> TRX_DCP_P_OW1, TRX_DCP_P_OW2
//      0x10    MRR_RAD_FSM_TX_DATA_7           -> Moved to memory mapping
//      0x10    n/a                             -> FSM_TX_TP_LEN
//      0x10    n/a                             -> FSM_TX_PASSCODE
//      0x11    MRR_RAD_FSM_SLEEP               -> FSM_SLEEP
//      0x11    MRR_RAD_FSM_EN                  -> FSM_EN
//      0x11    MRR_RAD_FSM_RSTN                -> FSM_RESET_B
//      0x11    MRR_RAD_FSM_TX_H_LEN            -> Deleted
//      0x11    MRR_RAD_FSM_TX_D_LEN            -> FSM_TX_DATA_BITS
//      0x11    MRR_RAD_FSM_CONT_PULSE_MODEb (in 0x16) 
//                                              -> FSM_CONT_PULSE_MODE (note the polarity change)
//      0x12    MRR_RAD_FSM_TX_PW_LEN           -> FSM_TX_PW_LEN
//      0x12    MRR_RAD_FSM_TX_PS_LEN           -> FSM_TX_PS_LEN
//      0x13    MRR_RAD_FSM_TX_C_LEN            -> FSM_TX_C_LEN
//      0x13    MRR_RAD_FSM_TX_MODE             -> Deleted
//      0x14    MRR_RAD_FSM_TX_POWERON_LEN      -> FSM_TX_POWERON_LEN
//      0x15    MRR_RAD_FSM_RX_HDR_BITS         -> Deleted
//      0x15    MRR_RAD_FSM_RX_HDR_TH           -> Deleted
//      0x15    MRR_RAD_FSM_RX_DATA_BITS        -> FSM_RX_DATA_BITS
//      0x16    MRR_RAD_FSM_CONT_PULSE_MODEb    -> FSM_CONT_PULSE_MODE (in 0x11; note the polarity change)
//      0x1E    MRR_RAD_FSM_IRQ_REPLY_PACKET    -> FSM_IRQ_REPLY_PACKET
//      0x21    MRR_TRX_ENb_CONT_RC             -> TRX_ENb_CONT_RC
//  -------------------------------------------------------------------------------------------------------
//-----------------------------------------------
#include "PREv21E.h"
#include "PREv21E_RF.h"
#include "PMUv12_RF.h"
#include "SNTv5_RF.h"
#include "MRRv11A_RF.h"
#include "mbus.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// Short Prefixes
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

//#define RADIO_DATA_LENGTH 192             // [CISv2] Deleted
#define RADIO_DATA_NUM_WORDS 3              // [CISv2] Newly Added; (Num of 32-bit words) =192-96 (NOTE: In CISv2, you do not include the training bits & passcode bits here)
uint32_t mrr_tx_data[RADIO_DATA_NUM_WORDS]; // [CISv2] Newly Added; Can be up to 256 bits (i.e., uint32_t mrr_tx_data[8])

#define WAKEUP_PERIOD_RADIO_INIT 0xA // About 2 sec (PRCv17)

// PMU Temperatures
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

// PRE Timer Thresholds
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
//volatile uint32_t temp_storage_diff = 0;  // [CISv2] Deleted; Not used anywhere.
volatile uint32_t temp_storage_count;
volatile uint32_t temp_storage_debug;
volatile uint32_t sns_running;
volatile uint32_t read_data_temp; // [23:0] Temp Sensor D Out
volatile uint32_t TEMP_CALIB_A;
volatile uint32_t TEMP_CALIB_B;

volatile uint32_t snt_wup_counter_cur;
volatile uint32_t snt_timer_enabled;  // [CISv2] Deleted '= 0;'. Compiler does not explicitly reset global variables if its default value is 0.
volatile uint32_t SNT_0P5S_VAL;

volatile uint32_t sns_running;

volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t mrr_freq_hopping;
volatile uint32_t mrr_freq_hopping_step;
volatile uint32_t mrr_cfo_val_fine_min;
volatile uint32_t RADIO_PACKET_DELAY;
volatile uint32_t radio_packet_count;

volatile sntv5_r00_t sntv5_r00 = SNTv5_R00_DEFAULT;
volatile sntv5_r01_t sntv5_r01 = SNTv5_R01_DEFAULT;
volatile sntv5_r03_t sntv5_r03 = SNTv5_R03_DEFAULT;
volatile sntv5_r08_t sntv5_r08 = SNTv5_R08_DEFAULT;
volatile sntv5_r09_t sntv5_r09 = SNTv5_R09_DEFAULT;
volatile sntv5_r0A_t sntv5_r0A = SNTv5_R0A_DEFAULT;
volatile sntv5_r0B_t sntv5_r0B = SNTv5_R0B_DEFAULT;
volatile sntv5_r17_t sntv5_r17 = SNTv5_R17_DEFAULT;

volatile prev21e_r0B_t prev21e_r0B = PREv21E_R0B_DEFAULT;
volatile prev21e_r1C_t prev21e_r1C = PREv21E_R1C_DEFAULT;

volatile mrrv11a_r00_t mrrv11a_r00 = MRRv11A_R00_DEFAULT;
volatile mrrv11a_r01_t mrrv11a_r01 = MRRv11A_R01_DEFAULT;
volatile mrrv11a_r02_t mrrv11a_r02 = MRRv11A_R02_DEFAULT;
volatile mrrv11a_r03_t mrrv11a_r03 = MRRv11A_R03_DEFAULT;
volatile mrrv11a_r04_t mrrv11a_r04 = MRRv11A_R04_DEFAULT;
volatile mrrv11a_r07_t mrrv11a_r07 = MRRv11A_R07_DEFAULT;
volatile mrrv11a_r10_t mrrv11a_r10 = MRRv11A_R10_DEFAULT;   // [CISv2] Newly Added
volatile mrrv11a_r11_t mrrv11a_r11 = MRRv11A_R11_DEFAULT;
volatile mrrv11a_r12_t mrrv11a_r12 = MRRv11A_R12_DEFAULT;
volatile mrrv11a_r13_t mrrv11a_r13 = MRRv11A_R13_DEFAULT;
volatile mrrv11a_r14_t mrrv11a_r14 = MRRv11A_R14_DEFAULT;
volatile mrrv11a_r15_t mrrv11a_r15 = MRRv11A_R15_DEFAULT;
//volatile mrrv11a_r16_t mrrv11a_r16 = MRRv11A_R16_DEFAULT; // [CISv2] Deleted
volatile mrrv11a_r1F_t mrrv11a_r1F = MRRv11A_R1F_DEFAULT;
volatile mrrv11a_r21_t mrrv11a_r21 = MRRv11A_R21_DEFAULT;
volatile mrrv11a_r2A_t mrrv11a_r2A = MRRv11A_R2A_DEFAULT;   // [CISv2] Newly Added

// CPU IRQ List
volatile cpu_irq_list_t irq_pending = CPU_IRQ_LIST_DEFAULT;

//***************************************************
// Function Declarations
//***************************************************
void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));

static void operation_sns_sleep_check(void);
static void operation_sleep(void);
static void operation_sleep_snt_timer(void);
static void operation_sleep_noirqreset(void);
static void operation_sleep_notimer(void);
static void set_timer32_timeout(uint32_t val);
static void stop_timer32_timeout_check(uint32_t code);

//***************************************************
// Sleep Functions
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
    if (radio_on){ radio_power_off(); }

    // Make sure SNT timer is off
    if (snt_timer_enabled){ snt_stop_timer(); }

    // Disable Timer
    set_wakeup_timer(0, 0, 0);

    // Go to sleep
    operation_sleep();

}

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

void handler_ext_int_reg0(void)  { *NVIC_ICPR = (0x1 << IRQ_REG0);  irq_pending.reg0  = 0x1; }
void handler_ext_int_reg1(void)  { *NVIC_ICPR = (0x1 << IRQ_REG1);  irq_pending.reg1  = 0x1; }
void handler_ext_int_reg2(void)  { *NVIC_ICPR = (0x1 << IRQ_REG2);  irq_pending.reg2  = 0x1; }
void handler_ext_int_reg3(void)  { *NVIC_ICPR = (0x1 << IRQ_REG3);  irq_pending.reg3  = 0x1; }
void handler_ext_int_gocep(void) { *NVIC_ICPR = (0x1 << IRQ_GOCEP); irq_pending.gocep = 0x1; }

void handler_ext_int_timer32(void) {
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    irq_pending.timer32 = 0x1;
//    *REG1 = *TIMER32_CNT;
//    *REG2 = *TIMER32_STAT;
//    *TIMER32_STAT = 0x0;
//	  wfi_timeout_flag = 1;
//    set_halt_until_mbus_tx();
    }

void handler_ext_int_wakeup(void) {
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
    irq_pending.wakeup = 1;

//  delay(MBUS_DELAY);
//	mbus_write_message32(0xAA,*SREG_WAKEUP_SOURCE); // 0x1: GOC; 0x2: PRC Timer; 0x10: SNT

}



START FROM HERE!!!!!!!

// Temporary operation_init
void operation_init(void){
	mbus_write_message32(0xAF, 0x00000003);

    enumerated = 0x5453104b;

    set_halt_until_mbus_trx();
    mbus_enumerate(MEM_ADDR);
    set_halt_until_mbus_tx();

	mbus_write_message32(0xAF, 0x00000004);
}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(){
	mbus_write_message32(0xAF, 0x00000000);
	mbus_write_message32(0xAF, irq_pending.as_int);

	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP);
        
    if (enumerated != 0x5453104b){
        operation_init();
    }

	mbus_write_message32(0xAF, 0xFFFFFFFF);

    while(1){

        if (irq_pending.wakeup) {
            irq_pending.wakeup = 0;
	        mbus_write_message32(0xA5, 1);

        }

        if (irq_pending.gocep) {
            irq_pending.gocep = 0;
	        mbus_write_message32(0xA5, 2);

        }

    }
}


