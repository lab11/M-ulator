//*******************************************************************
//Author: Gyouho Kim
//Description: TS stack with MRRv10 and SNTv4
//          Modified from 'AHstack_ondemand_v1.22.c'
//          Modified from 'Tstack_ondemand_v6.4.c'
//          v1.0: PMU needs to reconfigure based on VBAT
//          v1.1: Code cleanup
//          v1.1a: Increasing PMU setting at 35C
//          v1.1b: Fixing a bug that incorrectly sets PMU active setting during SNT
//          v1.1c: Operation sleep was missing SNT safeguards
//          v1.2: Transmit SNT temp coefficient B as part of check-in message
//          FIXME: Need to verify PMU settings across temp & voltage
//          v1.3: Adding new MRR FSM burst mode for radio scanning
//                 Adding SNT Timer R tuning
//          v1.3b: Fixes a problem where running send_radio_data_mrr() more than once and 
//                 running radio_power_on() between them causes CL to be in weak mode
//          v1.4: PMU Optimizations
//          v1.4a: Minor updates for debugging; change SAR ratio every time irq wakeup
//                send exec_count when stopping measurement
//          v1.4b: SNT temp sensor reset & sleep setting restored whenever GOC triggered
//                Adding a dedicated setting for # of hops
//          v1.4b_CISv2: Converted v1.4b to work with CISv2
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
#define MBUS_DELAY 200 // Amount of delay between successive messages; 200: 6-7ms

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
static void wait_reg0_write_with_timer32_timeout (uint32_t code);
static void wait_reg1_write_with_timer32_timeout (uint32_t code);
static void wait_reg2_write_with_timer32_timeout (uint32_t code);
static void wait_reg3_write_with_timer32_timeout (uint32_t code);

static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data);
static void pmu_set_sar_override(uint32_t val);
inline static void pmu_set_adc_period(uint32_t val);
inline static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);
inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);
inline static void pmu_set_sleep_radio(void);
inline static void pmu_set_sleep_low(void);
inline static void pmu_set_sleep_tsns(void);
inline static void pmu_sleep_setting_temp_based(void);
inline static void pmu_active_setting_temp_based(void);
inline static void pmu_set_clk_init(void);
inline static void pmu_adc_reset_setting(void);
inline static void pmu_adc_disable(void);
inline static void pmu_adc_enable(void);
inline static void pmu_parking_decision_3v_battery(void);
inline static void pmu_adc_read_latest(void);
inline static void pmu_reset_solar_short(void);

uint32_t* crcEnc16(uint32_t data2, uint32_t data1, uint32_t data0);
uint32_t dumb_divide(uint32_t nu, uint32_t de);

static void mrr_ldo_vref_on(void);
static void mrr_ldo_power_on(void);
static void mrr_ldo_power_off(void);
static void radio_power_on(void);
static void radio_power_off(void);
static void mrr_configure_pulse_width_long(void);
//static void mrr_configure_pulse_width_long_2(void);   // Not Used
//static void mrr_configure_pulse_width_long_3(void);   // Not Used
//static void mrr_configure_pulse_width_short(void);    // Not Used
static void send_radio_data_mrr_sub1(void);
static void send_radio_data_mrr(uint32_t last_packet, uint8_t radio_packet_prefix, uint32_t radio_data);

static void temp_sensor_start(void);
static void temp_sensor_reset(void);
static void temp_sensor_power_on(void);
static void temp_sensor_power_off(void);
static void sns_ldo_vref_on(void);
static void sns_ldo_power_on(void);
static void sns_ldo_power_off(void);
static void snt_read_wup_counter(void);
static void snt_start_timer_presleep(void);
static void snt_start_timer_postsleep(void);
static void snt_stop_timer(void);
static void snt_set_wup_timer(uint32_t sleep_count);
static void snt_reset_and_restart_timer(void);
static void snt_set_timer_threshold(uint32_t sleep_count);

static void operation_init(void);
static void operation_sns_run(void);
static void operation_goc_trigger_init(void);
static void operation_goc_trigger_radio(uint32_t radio_tx_num, uint32_t wakeup_timer_val, uint8_t radio_tx_prefix, uint32_t radio_tx_data);


//*******************************************************************
// INTERRUPT HANDLERS
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
//    wfi_timeout_flag = 1;
//    set_halt_until_mbus_tx();
    }

void handler_ext_int_wakeup(void) { *NVIC_ICPR = (0x1 << IRQ_WAKEUP); irq_pending.wakeup = 1;
}

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

static void wait_reg0_write_with_timer32_timeout (uint32_t code) {
    while ((!irq_pending.reg0) & (!irq_pending.timer32));
    irq_pending.reg0 = 0;
    stop_timer32_timeout_check(code);
}
static void wait_reg1_write_with_timer32_timeout (uint32_t code) {
    while ((!irq_pending.reg1) & (!irq_pending.timer32));
    irq_pending.reg1 = 0;
    stop_timer32_timeout_check(code);
}
static void wait_reg2_write_with_timer32_timeout (uint32_t code) {
    while ((!irq_pending.reg2) & (!irq_pending.timer32));
    irq_pending.reg2 = 0;
    stop_timer32_timeout_check(code);
}
static void wait_reg3_write_with_timer32_timeout (uint32_t code) {
    while ((!irq_pending.reg3) & (!irq_pending.timer32));
    irq_pending.reg3 = 0;
    stop_timer32_timeout_check(code);
}

static void stop_timer32_timeout_check(uint32_t code){
    // TIMER32 IRQ Handling
    if (irq_pending.timer32) {
        irq_pending.timer32 = 0;
        *REG1 = *TIMER32_CNT;
        *REG2 = *TIMER32_STAT;
        *TIMER32_STAT = 0x0;
        wfi_timeout_flag = 1;
        //set_halt_until_mbus_tx(); // [CISv2] Deleted
    }
    
    // Turn off Timer32
    *TIMER32_GO = 0;
    if (wfi_timeout_flag){
        wfi_timeout_flag = 0;
        error_code = code;
        mbus_write_message32(0xFA, error_code);
    }
}


//************************************
// PMU Related Functions
//************************************

static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
    set_timer32_timeout(TIMER32_VAL);
    //set_halt_until_mbus_trx();    // [CISv2] Deleted
    mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
    //set_halt_until_mbus_tx();     // [CISv2] Deleted
    //stop_timer32_timeout_check(0x7);  // [CISv2] Deleted
    wait_reg0_write_with_timer32_timeout (0x7);
}


static void pmu_set_sar_override(uint32_t val){
    // SAR_RATIO_OVERRIDE
    pmu_reg_write(0x05,
        ( (0 << 13) // Enables override setting [12] (Default: 1'b0)
        | (0 << 12) // Let VDD_CLK always connected to VBAT (Default: 1'b0)
        | (1 << 11) // Enable override setting [10] (Default: 1'h1)
        | (0 << 10) // Have the converter have the periodic reset (Default: 1'h0)
        | (1 << 9)  // Enable override setting [8] (Default: 1'h0)
        | (0 << 8)  // Switch input / output power rails for upconversion (Default: 1'h0)
        | (1 << 7)  // Enable override setting [6:0] (Default: 1'h0)
        | (val)     // Binary converter's conversion ratio (Default: 7'h00)
    ));
    pmu_reg_write(0x05,
        ( (1 << 13) // Enables override setting [12] (Default: 1'b0)
        | (0 << 12) // Let VDD_CLK always connected to VBAT (Default: 1'b0)
        | (1 << 11) // Enable override setting [10] (Default: 1'h1)
        | (0 << 10) // Have the converter have the periodic reset (Default: 1'h0)
        | (1 << 9)  // Enable override setting [8] (Default: 1'h0)
        | (0 << 8)  // Switch input / output power rails for upconversion (Default: 1'h0)
        | (1 << 7)  // Enable override setting [6:0] (Default: 1'h0)
        | (val)     // Binary converter's conversion ratio (Default: 7'h00)
    ));
}

inline static void pmu_set_adc_period(uint32_t val){
    // PMU_CONTROLLER_DESIRED_STATE Active
    // Updated for PMUv9
    pmu_reg_write(0x3C,
        ((  1 << 0) //sar_on
        | (1 << 1) //wait_for_clock_cycles // [CISv2] Changed the value: 0 -> 1
        | (1 << 2) //wait_for_cap_charge
        | (1 << 3) //sar_reset
        | (1 << 4) //sar_stabilized
        | (1 << 5) //sar_ratio_roughly_adjusted
        | (1 << 6) //clock_supply_switched
        | (1 << 7) //control_supply_switched
        | (1 << 8) //upc_on
        | (1 << 9) //upc_stabilized
        | (1 << 10) //refgen_on
        | (0 << 11) //adc_output_ready
        | (0 << 12) //adc_adjusted
        | (0 << 13) //sar_ratio_adjusted
        | (1 << 14) //dnc_on
        | (1 << 15) //dnc_stabilized
        | (1 << 16) //vdd_3p6_turned_on
        | (1 << 17) //vdd_1p2_turned_on
        | (1 << 18) //vdd_0p6_turned_on
        | (0 << 19) //vbat_read_only
        | (1 << 20) //horizon
    ));

    // Register 0x36: TICK_REPEAT_VBAT_ADJUST
    pmu_reg_write(0x36,val); 
    delay(MBUS_DELAY*10);

    // Register 0x33: TICK_ADC_RESET
    pmu_reg_write(0x33,2);

    // Register 0x34: TICK_ADC_CLK
    //pmu_reg_write(0x34,2);    // [CISv2] Deleted; its default value is already 2.

    // PMU_CONTROLLER_DESIRED_STATE Active
    pmu_reg_write(0x3C,
        ((  1 << 0) //sar_on
        | (1 << 1) //wait_for_clock_cycles
        | (1 << 2) //wait_for_cap_charge
        | (1 << 3) //sar_reset
        | (1 << 4) //sar_stabilized
        | (1 << 5) //sar_ratio_roughly_adjusted
        | (1 << 6) //clock_supply_switched
        | (1 << 7) //control_supply_switched
        | (1 << 8) //upc_on
        | (1 << 9) //upc_stabilized
        | (1 << 10) //refgen_on
        | (0 << 11) //adc_output_ready
        | (0 << 12) //adc_adjusted
        | (0 << 13) //sar_ratio_adjusted
        | (1 << 14) //dnc_on
        | (1 << 15) //dnc_stabilized
        | (1 << 16) //vdd_3p6_turned_on
        | (1 << 17) //vdd_1p2_turned_on
        | (1 << 18) //vdd_0p6_turned_on
        | (0 << 19) //vbat_read_only
        | (1 << 20) //horizon
    ));
}

inline static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){
    // The first register write to PMU needs to be repeated // [CISv2] Deleted; you do not need to send the first message twice anymore.
    // Register 0x16: V1P2 Active
    //pmu_reg_write(0x16, ( (0 << 19) | (0 << 18) | (0 << 17) | (3 << 14) | (0 << 13) | (r << 9) | (l_1p2 << 5) | (base) ));
      
    // Register 0x16: V1P2 Active
    // [CISv2] Definition of Bit[13] has been changed.
    //          [OLD] Enable main feedback loop
    //          [NEW] Makes the converter clock 2x slower (Default: 1'b0)
    pmu_reg_write(0x16, 
        ( (0 << 19)     // Enable PFM even during periodic reset (Default: 1'b0)
        | (0 << 18)     // Enable PFM even when VREF is not used as reference (Default: 1'b0)
        | (0 << 17)     // Enable PFM (Default: 1'b0)
        | (3 << 14)     // Comparator clock division ratio (Default: 3'h3)
        | (0 << 13)     // Makes the converter clock 2x slower (Default: 1'b0)
        | (r << 9)      // Frequency multiplier R
        | (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
        | (base)        // Floor frequency base (0-63)
    ));

    // Register 0x18: V3P6 Active 
    // [CISv2] Definition of Bit[13] has been changed.
    //          [OLD] Enable main feedback loop
    //          [NEW] Makes the converter clock 2x slower (Default: 1'b0)
    pmu_reg_write(0x18, 
        ( (3 << 14) // Desired Vout/Vin ratio (Defualt: 2'h0)
        | (0 << 13) // Makes the converter clock 2x slower (Default: 1'h0)
        | (r << 9)  // Frequency multiplier R
        | (l << 5)  // Frequency multiplier L (actually L+1)
        | (base)    // Floor frequency base (0-63)
    ));
    
    // Register 0x1A: V0P6 Active
    // [CISv2] Definition of Bit[13] has been changed.
    //          [OLD] Enable main feedback loop
    //          [NEW] Makes the converter clock 2x slower (Default: 1'b0)
    pmu_reg_write(0x1A,
        ( (0 << 13) // Makes the converter clock 2x slower (Default: 1'h0)
        | (r << 9)  // Frequency multiplier R
        | (l << 5)  // Frequency multiplier L (actually L+1)
        | (base)    // Floor frequency base (0-63)
    ));
}

inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

    // Register 0x17: V3P6 Sleep
    // [CISv2] Definition of Bit[13] has been changed.
    //          [OLD] Enable main feedback loop
    //          [NEW] Makes the converter clock 2x slower (Default: 1'b0)
    pmu_reg_write(0x17, 
        ( (3 << 14) // Desired Vout/Vin ratio (Defualt: 2'h0)
        | (0 << 13) // Makes the converter clock 2x slower (Default: 1'h0)
        | (r << 9)  // Frequency multiplier R
        | (l << 5)  // Frequency multiplier L (actually L+1)
        | (base)    // Floor frequency base (0-63)
    ));

    // Register 0x15: V1P2 Sleep
    // [CISv2] Definition of Bit[13] has been changed.
    //          [OLD] Enable main feedback loop
    //          [NEW] Makes the converter clock 2x slower (Default: 1'b0)
    pmu_reg_write(0x15, 
        ( (0 << 19)     // Enable PFM even during periodic reset (Default: 1'b0)
        | (0 << 18)     // Enable PFM even when VREF is not used as reference (Default: 1'b0)
        | (0 << 17)     // Enable PFM (Default: 1'b0)
        | (3 << 14)     // Comparator clock division ratio (Default: 3'h3)
        | (0 << 13)     // Makes the converter clock 2x slower (Default: 1'b0)
        | (r << 9)      // Frequency multiplier R
        | (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
        | (base)        // Floor frequency base (0-63)
    ));

    // Register 0x19: V0P6 Sleep
    // [CISv2] Definition of Bit[13] has been changed.
    //          [OLD] Enable main feedback loop
    //          [NEW] Makes the converter clock 2x slower (Default: 1'b0)
    pmu_reg_write(0x19,
        ( (0 << 13) // Makes the converter clock 2x slower (Default: 1'h0)
        | (r << 9)  // Frequency multiplier R
        | (l << 5)  // Frequency multiplier L (actually L+1)
        | (base)    // Floor frequency base (0-63)
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
    pmu_reg_write(0x05,
        ( (0 << 13) // Enables override setting [12] (Default: 1'b0)
        | (0 << 12) // Let VDD_CLK always connected to VBAT (Default: 1'b0)
        | (1 << 11) // Enable override setting [10] (Default: 1'h1)
        | (0 << 10) // Have the converter have the periodic reset (Default: 1'h0)
        | (0 << 9)  // Enable override setting [8] (Default: 1'h0)
        | (0 << 8)  // Switch input / output power rails for upconversion (Default: 1'h0)
        | (0 << 7)  // Enable override setting [6:0] (Default: 1'h0)
        | (0x45)    // Binary converter's conversion ratio (Default: 7'h00)
    ));
    pmu_set_sar_override(0x45);

    pmu_set_adc_period(1); // 0x100 about 1 min for 1/2/1 1P2 setting
}


inline static void pmu_adc_reset_setting(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Active
    // Updated for PMUv9
    pmu_reg_write(0x3C,
        ((  1 << 0) //sar_on
        | (1 << 1) //wait_for_clock_cycles
        | (1 << 2) //wait_for_cap_charge
        | (1 << 3) //sar_reset
        | (1 << 4) //sar_stabilized
        | (1 << 5) //sar_ratio_roughly_adjusted
        | (1 << 6) //clock_supply_switched
        | (1 << 7) //control_supply_switched
        | (1 << 8) //upc_on
        | (1 << 9) //upc_stabilized
        | (1 << 10) //refgen_on
        | (0 << 11) //adc_output_ready
        | (0 << 12) //adc_adjusted
        | (0 << 13) //sar_ratio_adjusted
        | (1 << 14) //dnc_on
        | (1 << 15) //dnc_stabilized
        | (1 << 16) //vdd_3p6_turned_on
        | (1 << 17) //vdd_1p2_turned_on
        | (1 << 18) //vdd_0p6_turned_on
        | (0 << 19) //vbat_read_only
        | (1 << 20) //horizon
    ));
}

inline static void pmu_adc_disable(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Sleep
    // Updated for PMUv9
    pmu_reg_write(0x3B,
        ((  1 << 0) //sar_on
        | (1 << 1) //wait_for_clock_cycles
        | (1 << 2) //wait_for_cap_charge
        | (1 << 3) //sar_reset
        | (1 << 4) //sar_stabilized
        | (1 << 5) //sar_ratio_roughly_adjusted
        | (1 << 6) //clock_supply_switched
        | (1 << 7) //control_supply_switched
        | (1 << 8) //upc_on
        | (1 << 9) //upc_stabilized
        | (1 << 10) //refgen_on
        | (0 << 11) //adc_output_ready
        | (0 << 12) //adc_adjusted
        | (1 << 13) //sar_ratio_adjusted // Turn on for old adc, off for new adc
        | (1 << 14) //dnc_on
        | (1 << 15) //dnc_stabilized
        | (1 << 16) //vdd_3p6_turned_on
        | (1 << 17) //vdd_1p2_turned_on
        | (1 << 18) //vdd_0p6_turned_on
        | (0 << 19) //vbat_read_only // Turn off for old adc
        | (1 << 20) //horizon
    ));
}

inline static void pmu_adc_enable(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Sleep
    // Updated for PMUv9
    pmu_reg_write(0x3B,
        ((  1 << 0) //sar_on
        | (1 << 1) //wait_for_clock_cycles
        | (1 << 2) //wait_for_cap_charge
        | (1 << 3) //sar_reset
        | (1 << 4) //sar_stabilized
        | (1 << 5) //sar_ratio_roughly_adjusted
        | (1 << 6) //clock_supply_switched
        | (1 << 7) //control_supply_switched
        | (1 << 8) //upc_on
        | (1 << 9) //upc_stabilized
        | (1 << 10) //refgen_on
        | (1 << 11) //adc_output_ready
        | (0 << 12) //adc_adjusted // Turning off offset cancellation
        | (1 << 13) //sar_ratio_adjusted // Turn on for old adc, off for new adc
        | (1 << 14) //dnc_on
        | (1 << 15) //dnc_stabilized
        | (1 << 16) //vdd_3p6_turned_on
        | (1 << 17) //vdd_1p2_turned_on
        | (1 << 18) //vdd_0p6_turned_on
        | (0 << 19) //vbat_read_only // Turn off for old adc
        | (1 << 20) //horizon
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
    read_data_batadc = *REG0 & 0xFF; // [CISv2] *((volatile uint32_t *) REG0) -> *REG0

    if (read_data_batadc<PMU_ADC_3P0_VAL){
        read_data_batadc_diff = 0;
    }else{
        read_data_batadc_diff = read_data_batadc - PMU_ADC_3P0_VAL;
    }

    pmu_parking_decision_3v_battery();

}

inline static void pmu_reset_solar_short(){

    //[CISv2] Newly implemented VOLTAGE_CLAMP_TRIM Register (PMU REG#0x0E)
    //-----------------------------------------------
    // [12]  : See <EN_VSOLAR_SHORT Behavior> below
    // [11]  : Set VBAT_HIGH_LATRST
    // [10:8]: See <EN_VSOLAR_SHORT Behavior> below
    // [7]   : Not used
    // [6:4] : CLAMP_TUNE_BOTTOM - increase clamp threshold voltage
    // [3]   : Not used
    // [2:0] : CLAMP_TUNE_TOP - decrease clamp threshold voltage
    //-----------------------------------------------
    // <EN_VSOLAR_SHORT Behavior>
    //          ------------------------------------
    //          VOLTAGE_CLAMP_TRIM | EN_VSOLAR_SHORT
    //          [12] [10] [9] [8]  | Behavior
    //          ------------------------------------
    //            x    x   1   0   | 0
    //            x    x   1   1   | 1
    //            1    x   0   x   | vbat_high_lat_latched (latched comparator output from PMU)
    //            0    1   0   x   | vbat_high_latched     (comparator output from PMU)
    // (default)  0    0   0   x   | 0 until POR activates; then 1 until V0P6 output turns on; then follows vbat_high_latched.
    //-----------------------------------------------

    //[CISv2] Deleted - we should try the default EN_VSOLAR_SHORT behavior
    //-----------------------------------------------
    //pmu_reg_write(0x0E, 
    //  ( (0 << 12) // 1: solar short by latched vbat_high (new); 0: follow [10] setting
    //  | (1 << 11) // Reset of vbat_high latch for [12]=1
    //  | (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
    //  | (1 << 9)  // Enables override setting [8]
    //  | (0 << 8)  // Turn on the harvester-inhibiting switch
    //  | (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
    //  | (0)       // clamp_tune_top (decreases clamp thresh)
    //));
    //pmu_reg_write(0x0E, 
    //  ( (0 << 12) // 1: solar short by latched vbat_high (new); 0: follow [10] setting
    //  | (1 << 11) // Reset of vbat_high latch for [12]=1
    //  | (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
    //  | (0 << 9)  // Enables override setting [8]
    //  | (0 << 8)  // Turn on the harvester-inhibiting switch
    //  | (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
    //  | (0)       // clamp_tune_top (decreases clamp thresh)
    //));
    //-----------------------------------------------
}

//***************************************************
// A Few Arithmetic
//***************************************************
#define DATA_LEN 96
#define CRC_LEN 16

uint32_t* crcEnc16(uint32_t data2, uint32_t data1, uint32_t data0) {
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
        
        if      (i < 32) input_bit = ((data2 << (i   )) > 0x7fffffff);
        else if (i < 64) input_bit = ((data1 << (i-32)) > 0x7fffffff);
        else             input_bit = ((data0 << (i-64)) > 0x7fffffff);

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

//***************************************************
// MRR Functions
//***************************************************

static void mrr_ldo_vref_on(){
    mrrv11a_r04.LDO_EN_VREF    = 1;
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
}

static void mrr_ldo_power_on(){
    mrrv11a_r04.LDO_EN_IREF    = 1;
    mrrv11a_r04.LDO_EN_LDO    = 1;
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
}
static void mrr_ldo_power_off(){
    mrrv11a_r04.LDO_EN_VREF    = 0;
    mrrv11a_r04.LDO_EN_IREF    = 0;
    mrrv11a_r04.LDO_EN_LDO    = 0;
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
}

static void radio_power_on(){
    // Turn off PMU ADC
    //pmu_adc_disable();

    // Need to speed up sleep pmu clock
    //pmu_set_sleep_radio();

    // New for MRRv11A
    mrr_ldo_vref_on();

    // Turn off Current Limter Briefly
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Set decap to parallel
    mrrv11a_r03.TRX_DCP_S_OW1 = 0;  //TX_Decap S (forced charge decaps)
    mrrv11a_r03.TRX_DCP_S_OW2 = 0;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    mrrv11a_r03.TRX_DCP_P_OW1 = 1;  //RX_Decap P 
    mrrv11a_r03.TRX_DCP_P_OW2 = 1;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    delay(MBUS_DELAY);

    // Set decap to series
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    delay(MBUS_DELAY);

    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    radio_on = 1;

    // New for MRRv11A
    mrr_ldo_power_on();

    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Release timer power-gate
    mrrv11a_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    //mrrv11a_r04.RO_EN_RO_LDO = 1;  //Use LDO for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    delay(MBUS_DELAY);

    // Turn on timer
    mrrv11a_r04.RO_RESET = 0;  //Release Reset TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    delay(MBUS_DELAY);

    mrrv11a_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    delay(MBUS_DELAY);

    mrrv11a_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);

    // Release FSM Sleep
    mrrv11a_r11.FSM_SLEEP = 0;  // Power on BB
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    delay(MBUS_DELAY*5); // Freq stab

}

static void radio_power_off(){
    // Need to restore sleep pmu clock

    // Enable PMU ADC
    //pmu_adc_enable();

    // In case continuous mode was running
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
        
    // Turn off FSM
    mrrv11a_r03.TRX_ISOLATEN = 0;     //set ISOLATEN 0
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);

    mrrv11a_r11.FSM_EN = 0;  //Stop BB
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
    mrrv11a_r11.FSM_SLEEP = 1;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);

    // Turn off Current Limter Briefly
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Turn off RO
    mrrv11a_r04.RO_RESET = 1;  //Release Reset TIMER
    mrrv11a_r04.RO_EN_CLK = 0; //Enable CLK TIMER
    mrrv11a_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);

    mrr_ldo_power_off();

    // Enable timer power-gate
    mrrv11a_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
    //mrrv11a_r04.RO_EN_RO_LDO = 0;  //Use LDO for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);

    radio_on = 0;
    radio_ready = 0;

}

static void mrr_configure_pulse_width_long(){

    mrrv11a_r12.FSM_TX_PW_LEN = 24; //100us PW
    mrrv11a_r13.FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv11a_r12.FSM_TX_PS_LEN = 49; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv11a_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv11a_r13.as_int);
}

/*
static void mrr_configure_pulse_width_long_2(){

    mrrv11a_r12.FSM_TX_PW_LEN = 19; //80us PW
    mrrv11a_r13.FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv11a_r12.FSM_TX_PS_LEN = 39; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv11a_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv11a_r13.as_int);
}

static void mrr_configure_pulse_width_long_3(){

    mrrv11a_r12.FSM_TX_PW_LEN = 9; //40us PW
    mrrv11a_r13.FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv11a_r12.FSM_TX_PS_LEN = 19; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv11a_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv11a_r13.as_int);
}

static void mrr_configure_pulse_width_short(){

    mrrv11a_r12.FSM_TX_PW_LEN = 0; //4us PW
    mrrv11a_r13.FSM_TX_C_LEN = 32; // (PW_LEN+1):C_LEN=1:32
    mrrv11a_r12.FSM_TX_PS_LEN = 1; // PW=PS guard interval betwen 0 and 1 pulse

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv11a_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv11a_r13.as_int);
}
*/

static void send_radio_data_mrr_sub1(){

    // Use timer32 as timeout counter
    set_timer32_timeout(TIMER32_VAL);

    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Fire off data // [CISv2] Added FSM_RESET_B=1
    mrrv11a_r11.FSM_RESET_B = 1;    
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    mrrv11a_r11.FSM_EN = 1;  //Start BB
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);

    // Wait for radio response
    //WFI();                            // [CISv2] Deleted
    //stop_timer32_timeout_check(0x3);  // [CISv2] Deleted
    wait_reg2_write_with_timer32_timeout(0x3);
    
    // Turn off Current Limter
    mrrv11a_r00.TRX_CL_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Disable FSM // [CISv2] Added FSM_RESET_B=0
    mrrv11a_r11.FSM_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    mrrv11a_r11.FSM_RESET_B = 0;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
}

static void send_radio_data_mrr(uint32_t last_packet, uint8_t radio_packet_prefix, uint32_t radio_data){

    // CRC16 Encoding 
    uint32_t* output_data;
    output_data = crcEnc16(((read_data_batadc & 0xFF)<<8) | ((radio_packet_prefix & 0xF)<<4) | ((radio_packet_count>>16)&0xF), ((radio_packet_count & 0xFFFF)<<16) | (*REG_CHIP_ID & 0xFFFF), radio_data);

    //[CISv2]
    //-----------------------------------------------
    //  [OLD] Using MBus Register File (Reg0x09 ~ Reg0x10) to store TX Data
    //              Sends 192 bit packet, of which 96b is actual data
    //                  Reg0x09 ~ REG0x0C: reserved for header
    //                  Reg_D: DATA[23:0]
    //                  Reg_E: DATA[47:24]
    //                  Reg_F: DATA[71:48]
    //                  Reg_10: DATA[95:72]
    //        Below, tx_data_0 ~ tx_data_3 are the four 24-bit TX Data used in the old version.
    //              tx_data_0 was written to Reg0x0D, tx_data_1 to Reg0x0E, so on.
    //  [NEW] Using Non-retentive memory
    //-----------------------------------------------
    uint32_t tx_data_0 = radio_data & 0xFFFFFF;
    uint32_t tx_data_1 = (*REG_CHIP_ID<<8)|(radio_data>>24);
    uint32_t tx_data_2 = (radio_packet_prefix<<20)|(radio_packet_count&0xFFFFF);
    uint32_t tx_data_3 = ((output_data[2] & 0xFFFF)/*CRC16*/<<8)|(read_data_batadc&0xFF);

    // [CISv2] Four 24-bit data -> Three 32-bit data conversion
    mrr_tx_data[0] = ((tx_data_1 << 24) & 0xFF000000) | ( tx_data_0        & 0x00FFFFFF);
    mrr_tx_data[1] = ((tx_data_2 << 16) & 0xFFFF0000) | ((tx_data_1 >>  8) & 0x0000FFFF);
    mrr_tx_data[2] = ((tx_data_3 <<  8) & 0xFFFFFF00) | ((tx_data_2 >> 16) & 0x000000FF);
    mbus_copy_mem_from_local_to_remote_bulk(MRR_ADDR, (uint32_t *) 0x00000000, (uint32_t *) mrr_tx_data, RADIO_DATA_NUM_WORDS-1);

    if (!radio_ready){
        radio_ready = 1;

        // Release FSM Reset // [CISv2] Deleted
        //mrrv11a_r11.FSM_RESET_B = 1;  //UNRST BB
        //mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
        //delay(MBUS_DELAY);

        mrrv11a_r03.TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
        mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
        delay(MBUS_DELAY);

    }

    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

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

        mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
        mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine;
        mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
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
        // [CISv2] Deleted; FSM must have been already disabled at the end of send_radio_data_mrr_sub1.
        //mrrv11a_r11.FSM_EN = 0;
        //mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    }
}

//***************************************************
// Temp Sensor Functions (SNTv5)
//***************************************************

static void temp_sensor_start(){
    sntv5_r01.TSNS_RESETn = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
}
static void temp_sensor_reset(){
    sntv5_r01.TSNS_RESETn = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
}
static void temp_sensor_power_on(){
    // Turn on digital block
    sntv5_r01.TSNS_SEL_LDO = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
    // Turn on analog block
    sntv5_r01.TSNS_EN_SENSOR_LDO = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);

    delay(MBUS_DELAY);

    // Release isolation
    sntv5_r01.TSNS_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
}
static void temp_sensor_power_off(){
    sntv5_r01.TSNS_RESETn = 0;
    sntv5_r01.TSNS_SEL_LDO = 0;
    sntv5_r01.TSNS_EN_SENSOR_LDO = 0;
    sntv5_r01.TSNS_ISOLATE = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);
}
static void sns_ldo_vref_on(){
    sntv5_r00.LDO_EN_VREF    = 1;
    mbus_remote_register_write(SNT_ADDR,0x00,sntv5_r00.as_int);
}
static void sns_ldo_power_on(){
    sntv5_r00.LDO_EN_IREF    = 1;
    sntv5_r00.LDO_EN_LDO    = 1;
    mbus_remote_register_write(SNT_ADDR,0x00,sntv5_r00.as_int);
}
static void sns_ldo_power_off(){
    sntv5_r00.LDO_EN_VREF    = 0;
    sntv5_r00.LDO_EN_IREF    = 0;
    sntv5_r00.LDO_EN_LDO    = 0;
    mbus_remote_register_write(SNT_ADDR,0x00,sntv5_r00.as_int);
}
static void snt_read_wup_counter(){

    set_timer32_timeout(TIMER32_VAL);

    //[CISv2] Updated to use the new 'Synchronous Reading' feature in SNTv5
    //-----------------------------------------------
    //  In SNTv5, writing into Register 0x14 triggers a synchronous counter read.
    //      There are 3 types of the reading - See below 'Read Type'
    //      Upon writing into Register 0x14, SNTv5 triggers the synchronous read,
    //      and sends reply messages to PRC.
    mbus_remote_register_write(SNT_ADDR,0x14,   // WUP_CNT_READ_REQ (Memory-Mapped)
                                  (0x00 << 16)  // Read Type (0: 32-bit Sync, 1: Lower 24-bit Sync, 2: Upper 8-bit Sync, 3: Invalid)
                                | (0x10 <<  8)  // MBus Target Address
                                | (0x00 <<  0)  // Destination Register Address
                                );
    // NOTE: For 'Read Type=0', the upper 8-bits is written to 'Destination Register'
    //       and the lower 24-bits is written to 'Destination Register + 1'
    wait_reg1_write_with_timer32_timeout(0x4);
    snt_wup_counter_cur = ((*REG0 & 0x000000FF) << 24) | (*REG1 & 0x00FFFFFF);
}
    
static void snt_start_timer_presleep(){

    // New for SNTv3
    sntv5_r08.TMR_SLEEP = 0x0; // Default : 0x1
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    sntv5_r08.TMR_ISOLATE = 0x0; // Default : 0x1
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);

    // TIMER SELF_EN Disable 
    sntv5_r09.TMR_SELF_EN = 0x0; // Default : 0x1
    mbus_remote_register_write(SNT_ADDR,0x09,sntv5_r09.as_int);

    // EN_OSC 
    sntv5_r08.TMR_EN_OSC = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);

    // Release Reset 
    sntv5_r08.TMR_RESETB = 0x1; // Default : 0x0
    sntv5_r08.TMR_RESETB_DIV = 0x1; // Default : 0x0
    sntv5_r08.TMR_RESETB_DCDC = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);

    // TIMER EN_SEL_CLK Reset 
    sntv5_r08.TMR_EN_SELF_CLK = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);

    // TIMER SELF_EN 
    sntv5_r09.TMR_SELF_EN = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x09,sntv5_r09.as_int);
    //delay(100000); 

    snt_timer_enabled = 1;
}

static void snt_start_timer_postsleep(){
    // Turn off sloscillator
    sntv5_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
}


static void snt_stop_timer(){

    // EN_OSC
    sntv5_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    // RESET
    sntv5_r08.TMR_EN_SELF_CLK = 0x0; // Default : 0x0
    sntv5_r08.TMR_RESETB = 0x0;// Default : 0x0
    sntv5_r08.TMR_RESETB_DIV = 0x0; // Default : 0x0
    sntv5_r08.TMR_RESETB_DCDC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    snt_timer_enabled = 0;

    sntv5_r17.WUP_ENABLE = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);

    // New for SNTv3
    sntv5_r08.TMR_SLEEP = 0x1; // Default : 0x1
    sntv5_r08.TMR_ISOLATE = 0x1; // Default : 0x1
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);

}

static void snt_set_wup_timer(uint32_t sleep_count){
    
    snt_wup_counter_cur = sleep_count + snt_wup_counter_cur; // should handle rollover

    mbus_remote_register_write(SNT_ADDR,0x19,snt_wup_counter_cur>>24);
    mbus_remote_register_write(SNT_ADDR,0x1A,snt_wup_counter_cur & 0xFFFFFF);
    
    sntv5_r17.WUP_ENABLE = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);

}

static void snt_reset_and_restart_timer(){
    
    sntv5_r17.WUP_ENABLE = 0x0;
    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
    delay(MBUS_DELAY);
    sntv5_r17.WUP_ENABLE = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
}

static void snt_set_timer_threshold(uint32_t sleep_count){
    
    mbus_remote_register_write(SNT_ADDR,0x19,sleep_count>>24);
    mbus_remote_register_write(SNT_ADDR,0x1A,sleep_count & 0xFFFFFF);
    
}

//********************************************************************
// Initialization
//********************************************************************

static void operation_init(void){

    mbus_write_message32(0xAF, 0x00001000);
  
    // Set CPU & Mbus Clock Speeds             Default in PREv21E
    prev21e_r0B.CLK_GEN_RING = 0x1;         // 0x1
    prev21e_r0B.CLK_GEN_DIV_MBC = 0x1;      // 0x2
    prev21e_r0B.CLK_GEN_DIV_CORE = 0x2;     // 0x3
    prev21e_r0B.GOC_CLK_GEN_SEL_FREQ = 0x5; // 0x7
    prev21e_r0B.GOC_CLK_GEN_SEL_DIV = 0x0;  // 0x1
    prev21e_r0B.GOC_SEL = 0xF;              // 0x8
    *REG_CLKGEN_TUNE = prev21e_r0B.as_int;

    mbus_write_message32(0xAF, 0x00001001);

    //[CISv2] Deleted - same as the default value
    //-----------------------------------------------
    //prev21e_r1C.SRAM0_TUNE_DECODER_DLY = 0x10; // Default 0x10, 5 bits
    //prev21e_r1C.SRAM0_USE_INVERTER_SA = 0;  // Default 0
    //*REG_SRAM0_TUNE = prev21e_r1C.as_int;
    //-----------------------------------------------
  
    //Enumerate & Initialize Registers
    stack_state = STK_IDLE;     //0x0;
    enumerated = 0x5453104b; // 0x5453 is TS in ascii
    exec_count = 0;
    wakeup_count = 0;
    exec_count_irq = 0;
    PMU_ADC_3P0_VAL = 0x62;
    pmu_parking_mode = 3;
  
    mbus_write_message32(0xAF, 0x00001002);

    //Enumeration
    set_halt_until_mbus_trx();  // [CISv2] Newly Added
    mbus_enumerate(SNT_ADDR);
    //delay(MBUS_DELAY);        // [CISv2] Deleted
    mbus_enumerate(MRR_ADDR);
    //delay(MBUS_DELAY);        // [CISv2] Deleted
    mbus_enumerate(PMU_ADDR);
    //delay(MBUS_DELAY);        // [CISv2] Deleted
    set_halt_until_mbus_tx();   // [CISv2] Newly Added


    // PMU Settings ----------------------------------------------
    pmu_set_clk_init();
    pmu_reset_solar_short();

    //[CISv2] Deleted - same as the default value
    //-----------------------------------------------
    //// VBAT_READ_TRIM Register
    //pmu_reg_write(0x45,
    //  ( (0x0 << 9) // 0x0: no mon; 0x1: sar conv mon; 0x2: up conv mon; 0x3: down conv mon
    //  | (0x0 << 8) // 1: vbat_read_mode enable; 0: vbat_read_mode disable
    //  | (0x48 << 0) //sampling multiplication factor N; vbat_read out = vbat/1p2*N
    //));
    //-----------------------------------------------

    // Disable PMU ADC measurement in active mode
    // PMU_CONTROLLER_STALL_ACTIVE
    // Updated for PMUv9
    pmu_reg_write(0x3A, 
        ( (1 << 20) // ignore state_horizon; default 0
        | (0 << 19) // vbat_read_only
        | (1 << 13) // ignore adc_output_ready; default 0
        | (1 << 12) // ignore adc_output_ready; default 0
        | (1 << 11) // ignore adc_output_ready; default 0
    ));

    pmu_adc_reset_setting();
    pmu_adc_enable();

    // Temp Sensor Settings --------------------------------------
    sntv5_r01.TSNS_RESETn = 0;
    sntv5_r01.TSNS_EN_IRQ = 1;
    sntv5_r01.TSNS_BURST_MODE = 0;
    sntv5_r01.TSNS_CONT_MODE = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,sntv5_r01.as_int);

    // Set temp sensor conversion time
    sntv5_r03.TSNS_SEL_STB_TIME = 0x1; 
    sntv5_r03.TSNS_SEL_CONV_TIME = 0x6; // Default: 0x6
    mbus_remote_register_write(SNT_ADDR,0x03,sntv5_r03.as_int);

    // SNT Wakeup Timer Settings --------------------------------------
    snt_timer_enabled = 0;  // [CISv2] Newly Added

    // Config Register A
    sntv5_r0A.TMR_S = 0x1; // Default: 0x4, use 1 for good TC
    // Tune R for TC
    sntv5_r0A.TMR_DIFF_CON = 0x3FEF; // Default: 0x3FFF
    mbus_remote_register_write(SNT_ADDR,0x0A,sntv5_r0A.as_int);

    // TIMER CAP_TUNE  
    // Tune C for freq
    sntv5_r09.TMR_SEL_CAP = 0x80; // Default : 8'h8
    sntv5_r09.TMR_SEL_DCAP = 0x3F; // Default : 6'h4
    mbus_remote_register_write(SNT_ADDR,0x09,sntv5_r09.as_int);

    // Wakeup Counter
    sntv5_r17.WUP_CLK_SEL = 0x0; 
    sntv5_r17.WUP_AUTO_RESET = 0x0; // Automatically reset counter to 0 upon sleep (Default: 1'h1)
    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);

    // MRR Settings --------------------------------------
    // Required in MRRv11A to run like MRRv7
    mrrv11a_r21.TRX_ENb_CONT_RC = 0;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,0x21,mrrv11a_r21.as_int);
    
    // Decap in series
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    
    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    
    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    
    // Wait for charging decap
    config_timerwd(TIMERWD_VAL);
    *REG_MBUS_WD = 1500000*3; // default: 1500000
    delay(MBUS_DELAY*200); // Wait for decap to charge
    
    mrrv11a_r1F.LC_CLK_RING = 0x3;  // ~ 150 kHz
    mrrv11a_r1F.LC_CLK_DIV = 0x3;  // ~ 150 kHz
    mbus_remote_register_write(MRR_ADDR,0x1F,mrrv11a_r1F.as_int);
    
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
    mrrv11a_r07.RO_MOM = 0x10;
    mrrv11a_r07.RO_MIM = 0x10;
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);
    
    // TX Setup Carrier Freq
    mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = 0x0;  //ANT CAP 10b unary 830.5 MHz
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_COARSE = 0x0; //ANT CAP 10b unary 830.5 MHz
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
    mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
    
    mrrv11a_r02.TX_EN_OW = 0;  // [CISv2] Newly Added; Turn off TX_EN_OW
    mrrv11a_r02.TX_BIAS_TUNE = 0x7FF;  //Set TX BIAS TUNE 13b // Max 0x1FFF
    mbus_remote_register_write(MRR_ADDR,0x02,mrrv11a_r02.as_int);
    
    // Turn off RX mode
    mrrv11a_r03.TRX_MODE_EN = 0; //Set TRX mode
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    
    mrrv11a_r14.FSM_TX_POWERON_LEN = 0; //3bits
    
    //mrrv11a_r15.MRR_RAD_FSM_RX_HDR_BITS = 0x00;  //[CISv2] Deleted; Set RX header
    //mrrv11a_r15.MRR_RAD_FSM_RX_HDR_TH = 0x00;    //[CISv2] Deleted; Set RX header threshold
    mrrv11a_r15.FSM_RX_DATA_BITS = 0x00; //Set RX data 1b
    mbus_remote_register_write(MRR_ADDR,0x14,mrrv11a_r14.as_int);
    mbus_remote_register_write(MRR_ADDR,0x15,mrrv11a_r15.as_int);
    
    // RAD_FSM set-up 
    
    //mbus_remote_register_write(MRR_ADDR,0x09,0x0);        // [CISv2] Deleted
    //mbus_remote_register_write(MRR_ADDR,0x0A,0x0);        // [CISv2] Deleted
    //mbus_remote_register_write(MRR_ADDR,0x0B,0x0);        // [CISv2] Deleted
    //mbus_remote_register_write(MRR_ADDR,0x0C,0x7AC800);   // [CISv2] Deleted
    
    // [CISv2] Newly Added
    //-----------------------------------------------
    mrrv11a_r10.FSM_TX_TP_LEN = 80;       // Num. Training Pulses (bit 0); Valid Range: 1 ~ 255 (Default: 80)
    mrrv11a_r10.FSM_TX_PASSCODE = 0x7AC8; // 16-bit Passcode (sent from LSB to MSB)
    mbus_remote_register_write(MRR_ADDR,0x10,mrrv11a_r10.as_int);
    //-----------------------------------------------
    
    //mrrv11a_r11.MRR_RAD_FSM_TX_H_LEN = 0; // [CISv2] Deleted; 31-31b header (max)
    mrrv11a_r11.FSM_TX_DATA_BITS = RADIO_DATA_NUM_WORDS * 32;   // [CISv2] RADIO_DATA_LENGTH -> RADIO_DATA_NUM_WORDS * 32
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    
    //[CISv2] Deleted
    //-----------------------------------------------
    //mrrv11a_r13.MRR_RAD_FSM_TX_MODE = 3; //code rate 0:4 1:3 2:2 3:1(baseline) 4:1/2 5:1/3 6:1/4
    //mbus_remote_register_write(MRR_ADDR,0x13,mrrv11a_r13.as_int);
    //-----------------------------------------------
    
    //[CISv2] Newly Added
    //-----------------------------------------------
    mrrv11a_r2A.SEL_V1P2_BIAS = 0;
    mrrv11a_r2A.SEL_VLDO_BIAS = 1;
    mbus_remote_register_write(MRR_ADDR,0x2A,mrrv11a_r2A.as_int);
    //-----------------------------------------------
    
    mrrv11a_r04.LDO_SEL_VOUT = 4; // New for MRRv11A
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    
    // MBus return address
    //mbus_remote_register_write(MRR_ADDR,0x1E,0x1002); // [CISv2] Deleted; its default value is already 0x1002
    
    // Additional delay for charging decap
    config_timerwd(TIMERWD_VAL);
    *REG_MBUS_WD = 1500000; // default: 1500000
    delay(MBUS_DELAY*200); // Wait for decap to charge
    
    // Initialize other global variables
    sns_running = 0;
    radio_ready = 0;
    radio_on = 0;
    wakeup_data = 0;
    RADIO_PACKET_DELAY = 4000;
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
            mbus_remote_register_read(SNT_ADDR,0x06,1);
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

    // Only enable relevant interrupts
    *NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);

    // Wakeup IRQ handling
    if (irq_pending.wakeup) {
        //[ 0] = GOC/EP
        //[ 1] = Wakeuptimer
        //[ 2] = XO timer
        //[ 3] = gpio_pad
        //[ 4] = mbus message
        //[ 8] = gpio[0]
        //[ 9] = gpio[1]
        //[10] = gpio[2]
        //[11] = gpio[3]
        irq_pending.wakeup = 0;
        delay(MBUS_DELAY);
        mbus_write_message32(0xAA,*SREG_WAKEUP_SOURCE); // 0x1: GOC; 0x2: PRC Timer; 0x10: SNT
    }
  
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

    // Initialization sequence
    if (enumerated != 0x5453104b){
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
        sntv5_r0A.TMR_DIFF_CON = wakeup_data & 0x3FFF; // Default: 0x3FFF
        mbus_remote_register_write(SNT_ADDR,0x0A,sntv5_r0A.as_int);

    }else if(wakeup_data_header == 0x14){
        // Update SNT wakeup counter value for 0.5s
        SNT_0P5S_VAL = wakeup_data & 0xFFFF;
        if (SNT_0P5S_VAL == 0){
            SNT_0P5S_VAL = 1000;
        }        

    }else if(wakeup_data_header == 0x15){
        // Update GOC clock
        prev21e_r0B.GOC_CLK_GEN_SEL_FREQ = (wakeup_data >> 4)&0x7; // Default 0x7
        prev21e_r0B.GOC_CLK_GEN_SEL_DIV = wakeup_data & 0x3; // Default 0x1
        *REG_CLKGEN_TUNE = prev21e_r0B.as_int;

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
        //                  -->  if zero: reports current chip id
        
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
    
        mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = wakeup_data & 0x3FF; // 10 bit coarse setting 
        mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
        mrrv11a_r01.TRX_CAP_ANTN_TUNE_COARSE = wakeup_data & 0x3FF; // 10 bit coarse setting

        mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min; 
        mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min;
        mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);

    }else if(wakeup_data_header == 0x23){
        // Change the baseband frequency of MRR (SFO)
        mrrv11a_r07.RO_MOM = wakeup_data & 0x3F;
        mrrv11a_r07.RO_MIM = wakeup_data & 0x3F;
        mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);
        
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

        mrrv11a_r02.TX_BIAS_TUNE = wakeup_data & 0x1FFF;  //Set TX BIAS TUNE 13b // Set to max
        mbus_remote_register_write(MRR_ADDR,0x02,mrrv11a_r02.as_int);

    }else if(wakeup_data_header == 0x27){

        mrrv11a_r04.LDO_SEL_VOUT = wakeup_data & 0x7;
        mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);

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
        mrrv11a_r11.FSM_RESET_B = 1;  //UNRST BB
        mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
        delay(MBUS_DELAY);

        mrrv11a_r03.TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
        mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
        delay(MBUS_DELAY);

        // Current Limter set-up 
        mrrv11a_r00.TRX_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
        mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

        // Turn on Current Limter
        mrrv11a_r00.TRX_CL_EN = 1;
        mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

        // Enable FSM Continuous Mode
          
        //[CISv2]
        //-----------------------------------------------
        //mrrv11a_r16.MRR_RAD_FSM_CONT_PULSE_MODEb = 0;
        //mbus_remote_register_write(MRR_ADDR,0x16,mrrv11a_r16.as_int);
        //-->
        mrrv11a_r11.FSM_CONT_PULSE_MODE = 1;
        mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
        //-----------------------------------------------
        
        // Fire off data
        mrrv11a_r11.FSM_EN = 1;  //Start BB
        mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);

        if ((wakeup_data & 0xFFFF) == 0){
            while(1);
        }else{
            if ((wakeup_data & 0xFFFF) > 340){ // Max count is roughly 5.6 hours
                set_timer32_timeout(0xFFFFFFFF);
            }else{
                set_timer32_timeout(0xC00000*(wakeup_data & 0xFFFF));
            }

            //WFI(); // [CISv2] Deleted
            
            while(!irq_pending.timer32);    // [CISv2] Newly Added
            irq_pending.timer32 = 0;        // [CISv2] Newly Added

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
