/******************************************************************************************
 * Author:      Roger Hsiao, Gordy Carichner
 * Description: Monarch butterfly challenge code
 *         This is the base code that all will share after version 5.1
 *                                          - PREv20E / PMUv11 / SNTv4 / FLPv3S / MRRv10 / MEMv1
 ******************************************************************************************
 * Current version: 5.2.15
 *
 * v1: draft version; not tested on chip
 *
 * v3: first deployment in mexico; not using FLP
 * v3.1: compact code
 *
 * v4: deployment code with periodic wakeup
 *
 * v4.3: log code with compression
 * v4.3.1: back to SNT timer
 *  Added self adjusting timer multiplier
 *  fix do while loop in set_next_time
 *  Adjust PMU setting based on temp
 *  Fix light shift upper half bug
 *  Fix reduce_len_counter initialization bug
 *  Fix light store time bug
 *  Fix % operator bug
 *  Fix lnt_counter_base shifting bug
 *  Fix xot_last_timer_list[idx] = xot_timer_list[idx] bug
 *  Fix len mode bug
 *  Fix bug caused by SNT_TEMP_UPDATE_TIME is not the same as LNT_INTERVAL[2]
 *  Fix storing l2 header before storing l1 header
 *  Fix update_light_interval midnight case
 *  Fix radio out bug
 *  Add goc triggers
 *  Fix lnt_meas_time_mode <= 2
 *  Fix read out load mem length
 *  Fix temp time shift
 *  Fix light time storage. Now will store the time the ref data is taken
 *  Fix temp time storage. Now will store the time the ref data is taken
 *  Fix pmu read_bat_data to be only 8 bits 
 *  Fix temp data variable resolution. Now uses log2 - offset
 *  Fix 30 minute LNT interval bug
 *  Fix temp_shift_left_store masking bug
 *  Fix temp_last_ref_time shifting bug
 *  Added compact beacon
 *  Fix light len mode change reduction
 *  Fix light len mode bug
 *  Set start time to a start_hour_in_sec
 *  Set safe sleep mode to 10C setting
 *  Set radio signal to safe sleep mode
 *  MRR_SIGNAL_PERIOD is set to 10 minutes
 *  Remove check for mrr_send_enable in mrr_send_data
 *  Remove check for GOC spurious wakeups
 *  Added SNT timer init delays back
 *  Change Beacon format
 *  Added radio delay between packets
 *
 *  v5: Using PREv20E
 *    back to xo timer
 *    Change trigger cmd format
 *    USING PMU OVERRIDE NOW
 *
 *  v5.1: Changed timer wakeup format
 *    Only keeping one timer at a time
 *    Removed set_next_time() and reset_timer_list()
 *    Fixed set_lnt_timer() multiplication overflow
 *    Fixed store_temp_conditions and end conditions
 *    Added compress_light initialization
 *    Added radio output every wakeup during collection
 *    Commented out some debug code
 *    Fixed blaster bug
 *    Saved as baseline code for all future revisions
 *
 *    Fixed an xo timer bug in update_system_time() where xo_sys_time_in_sec would overflow
 *
 *  v5.2: Using new compression and sampling algorithm
 *    Temp storage doesn't have a separate space anymore but interweaves with light storage
 *    Starting timestamp uses 4 fewers bits of resolution but 4 more bits of MSB
 *    Fixed variable initialization corruption by adding var_init()
 *    2020/7/29 Survived 1 day test but need to implement radio out
 *    Fixed not initializing starting address when radioing out
 *
 *  v5.2.1:
 *    Fixed cleaning up radio_data_arr[2]
 *    Correctly set halt to trx when reading MEM data
 *    Fixed light radio out shifting by adding temp arr
 *
 *  v5.2.2
 *    Fixed SAR radio changes to only toggle bit 13 to prevent current spikes
 *    Disabled watchdog timers while resampling to prevent timeout
 *    Added temperature restrictions back into radio out enabling   (temporary solution, using parameters from part 24 in first Mexico run)
 *    removed var_init from non operation goc triggers
 *
 *  v5.2.3:
 *    Align all operations to the minute
 *    temp huffman encoding
 *    First data in light unit is just 11 bits of raw data
 *    when storing 11 bits light, storing full data instead of diff
 *    when storing 7 bits temp, storing full data instead of diff
 *    Added store_day_state_stop()
 *    XO clock now first goes through a multiplier
 *    Added features
 *    Changed radio phase logic
 *    change temp units to the same size as light units
 *    Fixed divide_by_60 bug to use source value instead of xo_sys_time_in_sec
 *    Starting operation on an edge instead of at NOON or NIGHT
 *    Simplified resampling algorithm
 *    Added trigger for MRR_COARSE_TUNE
 *    Accounting for radio time when doing radio_rest and pmu_check
 *    Incorporated new PMU settings and SAR ratio hotfix
 *    Fixed light packets to store 17 bit timestamp at the start of day state storage
 *    Fixed radio rest and check pmu reset mechanism
 *
 *  v5.2.4:
 *    Added error state that will trigger when XO timer stops
 *    Updated PMU sar ratio setting to reg 0x05 bit 13
 *
 *  v5.2.5:
 *    Changed XO error trigger to compare current xo value to last wakeup xo value
 *
 *  v5.2.6:
 *    In radio_data while loop, check conditions first to avoid an infinite loop when max_unit_count == 0
 *    Changed verification beacons to contain, temp, light and pmu info
 *    Changed debug beacons to contain temp, pmu and xo timer info
 *    Removed OVERRIDE_RAD and UNE_GMB
 *
 *  v5.2.7:
 *    Changed update_system_time to clearer variable names
 *    Changed ENUMID to contain version info
 *
 *  v5.2.8:
 *    Updated LOW_PWR mode to consider temp
 *    LOW_PWR mode has to sense low power 3 times to enter LOW_PWR mode
 *    Changes initial beacon format and WAIT2 beacon format and debug beacon format
 *    Made OVERRIDE_RAD into a trigger
 *    Fixed check_pmu and radio_rest bug
 *
 *  v5.2.9:
 *    Do a majority vote when reading the XO timer
 *    Added upper bound on the difference between updates
 *
 *  v5.2.10:
 *    Incrementing radio_beacon_counter correctly now
 *
 *  v5.2.11:
 *    Correctly storing temp data to temp_cache when difference is over 3 bits
 *
 *  v5.2.12:
 *    Using mrr_send_enable when sending characterization and packet blaster
 *
 *  v5.2.13:
 *    Radioing out on first wakeup in collection phase regardless of radio_debug_setting. Note that this does not radio out if end_day_count is set in the past
 *    Changed WAIT2 beacons to include number of units in the storage
 *    Resetting check_pmu_counter when doing radio rest to avoid concatenating the sleep periods
 *    Added XO restart trigger
 *    Disabled mbus_wd_timer when running the timeout trigger to hard reset the system
 *    Moving the timeout trigger to the interrupt handler to ensure that it's run
 *
 *  v5.2.14:
 *    Added LNT FSM Stuck fix by checking COUNTER_STATE == COUNTING and check FSM after setting LNT timer
 *  
 *  v5.2.15:
 *    Used new LNT FSM FIX by running a simple loop.
 *
 ******************************************************************************************/

#define VERSION_NUM 0x52F

#include "huffman_encodings.h" 
#include "../include/PREv20E.h"
#include "../include/PREv20E_RF.h"
#include "../include/SNTv4_RF.h"
#include "../include/PMUv9_RF.h"
#include "../include/LNTv1A_RF.h"
#include "../include/MRRv7_RF.h"
#include "../include/mbus.h"
// #include "../include/MBC_params_0.h"
// #include "../include/MBC_params_5.h"
// #include "../include/MBC_params_7.h"
// #include "../include/MBC_params_12.h"
// #include "../include/MBC_params_13.h"
// #include "../include/MBC_params_14.h"
// #include "../include/MBC_params_16.h"
// #include "../include/MBC_params_19.h"
// #include "../include/MBC_params_22.h"
// #include "../include/MBC_params_24.h"
// #include "../include/MBC_params_28.h"
// #include "../include/MBC_params_31.h"

#define PRE_ADDR 0x1
#define MRR_ADDR 0x2
#define LNT_ADDR 0x3
#define SNT_ADDR 0x4
#define PMU_ADDR 0x5
#define MEM_ADDR 0x6
#define ENUMID 0xDEADBEEF

#define USE_MRR
#define USE_LNT
#define USE_SNT
#define USE_PMU
#define USE_MEM
#define USE_RAD
#define USE_XO

#define STATE_INIT 0
#define STATE_COLLECT 1
#define STATE_START 3
#define STATE_VERIFY 5
#define STATE_WAIT1 6
#define STATE_WAIT2 7
#define STATE_RADIO_DATA 8
#define STATE_RADIO_REST 9
#define STATE_LOW_PWR 10
#define STATE_ERROR 11

#define MBUS_DELAY 100  // Amount of delay between seccessive messages; 100: 6-7ms
#define TIMER32_VAL 0xA0000  // 0x20000 about 1 sec with Y5 run default clock (PRCv17)


// SNT states
#define SNT_IDLE        0x0
#define SNT_TEMP_LDO    0x1
#define SNT_TEMP_START  0x2
#define SNT_TEMP_READ   0x3
#define SNT_SET_PMU    0x4

#define XO_1_MIN 60
#define XO_2_MIN 120
#define XO_6_MIN 360
#define XO_8_MIN 480
#define XO_10_MIN 600
#define XO_16_MIN 960
#define XO_18_MIN 1080
#define XO_20_MIN 1200
#define XO_30_MIN 1800
#define XO_32_MIN 1920
#define XO_240_MIN 14400
#define XO_270_MIN 19600 // 4 hours and 30 minutes


uint32_t RADIO_PACKET_DELAY = 20000; // Amount of delay between radio packets
#define RADIO_DATA_LENGTH 192

// operation list
#define STORE_SNT   0x0
#define STORE_LNT   0x1
#define SEND_RAD    0x2

volatile uint16_t CHIP_ID = 0;

// default register values

volatile sntv4_r00_t sntv4_r00 = SNTv4_R00_DEFAULT;
volatile sntv4_r01_t sntv4_r01 = SNTv4_R01_DEFAULT;
volatile sntv4_r07_t sntv4_r07 = SNTv4_R07_DEFAULT;
// volatile sntv4_r17_t sntv4_r17 = SNTv4_R17_DEFAULT;

volatile lntv1a_r00_t lntv1a_r00 = LNTv1A_R00_DEFAULT;
// volatile lntv1a_r01_t lntv1a_r01 = LNTv1A_R01_DEFAULT;
// volatile lntv1a_r02_t lntv1a_r02 = LNTv1A_R02_DEFAULT;
volatile lntv1a_r03_t lntv1a_r03 = LNTv1A_R03_DEFAULT;
// volatile lntv1a_r04_t lntv1a_r04 = LNTv1A_R04_DEFAULT;
// volatile lntv1a_r05_t lntv1a_r05 = LNTv1A_R05_DEFAULT;
// volatile lntv1a_r06_t lntv1a_r06 = LNTv1A_R06_DEFAULT;
// volatile lntv1a_r17_t lntv1a_r17 = LNTv1A_R17_DEFAULT;
// volatile lntv1a_r20_t lntv1a_r20 = LNTv1A_R20_DEFAULT;
// volatile lntv1a_r21_t lntv1a_r21 = LNTv1A_R21_DEFAULT;
// volatile lntv1a_r22_t lntv1a_r22 = LNTv1A_R22_DEFAULT;
// volatile lntv1a_r40_t lntv1a_r40 = LNTv1A_R40_DEFAULT;

volatile mrrv7_r00_t mrrv7_r00 = MRRv7_R00_DEFAULT;
volatile mrrv7_r01_t mrrv7_r01 = MRRv7_R01_DEFAULT;
// volatile mrrv7_r02_t mrrv7_r02 = MRRv7_R02_DEFAULT;
volatile mrrv7_r03_t mrrv7_r03 = MRRv7_R03_DEFAULT;
volatile mrrv7_r04_t mrrv7_r04 = MRRv7_R04_DEFAULT;
// volatile mrrv7_r07_t mrrv7_r07 = MRRv7_R07_DEFAULT;
volatile mrrv7_r11_t mrrv7_r11 = MRRv7_R11_DEFAULT;
volatile mrrv7_r12_t mrrv7_r12 = MRRv7_R12_DEFAULT;
volatile mrrv7_r13_t mrrv7_r13 = MRRv7_R13_DEFAULT;
// volatile mrrv7_r14_t mrrv7_r14 = MRRv7_R14_DEFAULT;
// volatile mrrv7_r15_t mrrv7_r15 = MRRv7_R15_DEFAULT;
// volatile mrrv7_r1F_t mrrv7_r1F = MRRv7_R1F_DEFAULT;

/**********************************************
 * Global variables
 **********************************************
 * "static" limits the variables to this file, giving the compiler more freedom
 * "volatile" should only be used for mmio to ensure memory storage
 */
volatile uint32_t enumerated;
volatile uint16_t op_counter;
volatile uint16_t wfi_timeout_flag = 0;

volatile uint16_t radio_after_done;
// volatile uint16_t goc_component;
// volatile uint16_t goc_func_id;
volatile uint16_t goc_state;
// volatile uint16_t goc_data;
volatile uint32_t goc_data_full;

volatile uint32_t xo_sys_time = 0;
volatile uint32_t last_xo_sys_time = 0;
volatile uint32_t xo_sys_time_in_sec = 0;
volatile uint32_t xo_day_time_in_sec = 0;
volatile uint32_t sys_sec_to_min_offset = 0;
volatile uint32_t store_temp_timestamp = 0;

// #define SNT_TEMP_UPDATE_TIME 680   // 450 seconds
volatile uint32_t snt_sys_temp_code = 0x3E9;    // default 20C
volatile uint16_t snt_state = SNT_IDLE;

volatile uint16_t lnt_counter_base = 0;    // lnt measure time is LNT_MEAS_TIME << lnt_counter_base
// volatile uint16_t lnt_meas_time_mode;

volatile uint64_t lnt_sys_light = 0;

volatile uint16_t MRR_TEMP_THRESH_LOW = 0x258;
volatile uint16_t MRR_TEMP_THRESH_HIGH = 0x15F1; // defaults to 55C
volatile uint16_t PMU_WAKEUP_INTERVAL = XO_10_MIN; // defaults to 10 minutes; default time the system wakes up to adjust PMU
volatile uint16_t RADIO_SEQUENCE_PERIOD = 7; // defaults to radio out every week
volatile uint16_t RADIO_DUTY_DURATION = 3;
volatile uint32_t RADIO_INITIAL_BEACON_TIME = 43200;
volatile uint32_t RADIO_BEACON_PERIOD = 600;
volatile uint16_t RADIO_INITIAL_DATA_DAY = 1;
volatile uint16_t RADIO_INITIAL_DATA_TIME = 43200;
volatile uint16_t CHECK_PMU_NUM_UNITS = 16;
volatile uint16_t RADIO_REST_NUM_UNITS = 64;
volatile uint16_t RADIO_REST_TIME = 600;
volatile uint16_t RADIO_DATA_PERIOD = 7200;
volatile uint16_t radio_duty_cycle_end_day = 0;
volatile uint16_t radio_initial_data_start_day = 0;
volatile uint32_t next_beacon_time = 0;
volatile uint32_t next_data_time = 0;
volatile uint32_t radio_rest_end_time = 0;
volatile uint16_t mrr_send_enable = 1;
volatile uint16_t OVERRIDE_RAD = 0;
volatile uint16_t low_pwr_count = 0;
volatile uint32_t RADIO_DEBUG_PERIOD = XO_240_MIN;
volatile uint32_t LOW_PWR_TEMP_THRESH = 0;
volatile uint32_t low_pwr_state_end_day = 0;
volatile uint16_t LOW_PWR_HIGH_ADC_THRESH[7] = {90, 90, 90, 90, 90, 90, 90};
volatile uint16_t LOW_PWR_LOW_ADC_THRESH[7] = {90, 90, 90, 90, 90, 90, 90};
// volatile uint16_t LOW_PWR_VOLTAGE_THRESH_LOW = 0xFFFF;
// volatile uint16_t LOW_PWR_VOLTAGE_THRESH_HIGH = 0xFFFF;
volatile uint32_t DAY_START_TIME = 25200;   // 7 am
volatile uint32_t DAY_END_TIME = 68400;     // 7 pm

#define PMU_SETTINGS_LEN 6
volatile uint32_t PMU_ACTIVE_SETTINGS[7] = {0x0D0A0F0F, 
                                            0x0D080F0C, 
                                            0x0D0A0D09, 
                                            0x08040F04, 
                                            0x08040F04, 
                                            0x04040805, 
                                            0x01080F0A};

// first 2 values are normal active settings for safety but will not be set
volatile uint32_t PMU_RADIO_SETTINGS[7] = {0x0D0A0F0F,
                                           0x0D080F0C,
                                           0x0F080F0A,
                                           0x0D020F0F,
                                           0x0D030808,
                                           0x06060809,
                                           0x01011001};

volatile uint32_t PMU_SLEEP_SETTINGS[7] = {0x0F040306,
                                           0x0F020303,
                                           0x0F040304,
                                           0x0F010102,
                                           0x07010102,
                                           0x02010104,
                                           0x01010103};

volatile uint32_t PMU_ACTIVE_SAR_SETTINGS[7] = {58, 63, 56, 55, 51, 51, 51};
volatile uint32_t PMU_RADIO_SAR_SETTINGS[7] = {58, 63, 57, 52, 52, 53, 53};
volatile uint32_t PMU_SLEEP_SAR_SETTINGS[7] = {50, 52, 52, 52, 52, 50, 51};

volatile uint32_t PMU_TEMP_THRESH[6] = {0x1DD, 0x312, 0x4DE, 0x772, 0xC2C, 0x16A8}; // {0, 10, 20, 30, 45, 60}
volatile uint32_t PMU_ADC_THRESH[4] = {90, 90, 90, 90};

volatile uint16_t read_data_batadc;

volatile uint16_t radio_ready;
volatile uint16_t radio_on;
volatile uint16_t mrr_freq_hopping;
volatile uint16_t mrr_freq_hopping_step;
volatile uint16_t mrr_cfo_val_fine_min;
volatile uint32_t radio_data_arr[3];
volatile uint32_t radio_packet_count;

// sampling variables
#define DAWN 0
#define NOON 1
#define DUSK 2
#define NIGHT 3

volatile uint16_t MAX_EDGE_SHIFT = 600;
#define MAX_DAY_TIME 86400
#define MID_DAY_TIME 43200
// this is now aligned to the minute
volatile uint16_t IDX_MAX = 239;         // x = 180, y = 60, IDX_MAX = x + y - 1
volatile uint16_t EDGE_MARGIN1 = 10740; // (x - 1) * 60
volatile uint16_t EDGE_MARGIN2 = 3600; // y * 60

volatile uint16_t EDGE_THRESHOLD = 200; // = log2(2 lux * 1577) * 32
volatile uint16_t THRESHOLD_IDX_SHIFT = 8; // = 4 + 4

volatile uint32_t next_light_meas_time = 0;
volatile uint32_t next_radio_debug_time = 0;
volatile uint32_t next_radio_signal_time = 0;

volatile uint16_t day_state = NIGHT;
volatile uint32_t start_day_count = 0;
volatile uint32_t end_day_count = 0;
volatile uint32_t radio_day_count = 0;
volatile uint16_t day_count = 0;
volatile uint16_t epoch_days_offset = 0;
volatile uint16_t store_temp_index = 0;    // resets every day
volatile bool radio_debug = false;
volatile uint16_t rot_idx = 0;
volatile uint16_t running_avg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
volatile uint32_t running_avg_time[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#define MAX_UINT16 0xFFFF
volatile uint16_t sum = MAX_UINT16;
volatile uint16_t avg_light = 0;

volatile uint32_t cur_sunrise = 0, cur_sunset = 0;
volatile uint32_t next_sunrise = 0, next_sunset = 0, cur_edge = 0;
volatile uint32_t day_state_start_time, day_state_end_time;
volatile uint32_t light_meas_start_time_in_min = 0;

#define IDX_INIT 0xFF
volatile uint16_t max_idx = 0;
const uint16_t intervals[4] = {1, 2, 8, 32};
volatile uint16_t resample_indices[4] = {32, 40, 44, 1000};
volatile uint16_t min_light = MAX_UINT16;
volatile uint16_t min_light_idx = IDX_INIT;
volatile uint16_t threshold_idx = IDX_INIT;

// We can store 29 data points per 10 words
// => we need 9 * 10 words
// (4096 - 90) << 2 = 16024
#define CACHE_START_ADDR 16024
volatile uint16_t cache_addr = CACHE_START_ADDR;

#define PROC_CACHE_LEN 10
#define PROC_CACHE_MAX_REMAINDER 320
volatile uint32_t proc_cache[PROC_CACHE_LEN];
volatile uint16_t proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;

volatile uint32_t error_code = 0;
volatile uint32_t error_time = 0;

/**********************************************
 * Forward Declarations
 **********************************************/

void set_system_error(uint32_t);
static void sys_err(uint32_t);

void send_beacon();

uint64_t mult(uint32_t, uint16_t);
uint64_t right_shift(uint64_t input, int8_t shift);

/**********************************************
 * Timeout Functions
 **********************************************/

static void set_timer32_timeout(uint32_t val){
    // Use Timer32 as timeout counter
    wfi_timeout_flag = 0;
    config_timer32(val, 1, 0, 0);
}

static void stop_timer32_timeout_check(){
    // Turn off Timer32
    *TIMER32_GO = 0;
    if (wfi_timeout_flag){
        wfi_timeout_flag = 0;
    sys_err(0x04000000);
    }
}

void xo_init( void ) {
    prev20e_r19_t prev20e_r19 = PREv20E_R19_DEFAULT;

    // Parasitic capacitance tuning (6 bits for each; each 1 adds 1.8pF)
    uint32_t xo_cap_drv = 0x3F;
    uint32_t xo_cap_in  = 0x3F;
    *REG_XO_CONF2 = ((xo_cap_drv << 6) | (xo_cap_in << 0));

    // XO xonfiguration
    prev20e_r19.XO_PULSE_SEL     = 0x8; // pulse with sel, 1-hot encoded
    prev20e_r19.XO_DELAY_EN      = 0x7; // pair usage together with xo_pulse_sel
    prev20e_r19.XO_DRV_START_UP  = 0x0;
    prev20e_r19.XO_DRV_CORE      = 0x0;
    prev20e_r19.XO_SCN_CLK_SEL   = 0x0;
    prev20e_r19.XO_SCN_ENB       = 0x1;

    // TODO: check if need 32.768kHz clock
    prev20e_r19.XO_EN_DIV        = 0x1; // divider enable (also enables CLK_OUT)
    prev20e_r19.XO_S             = 0x0; // (not used) division ration for 16kHz out
    prev20e_r19.XO_SEL_CP_DIV    = 0x0; // 1: 0.3v-generation charge-pump uses divided clock
    prev20e_r19.XO_EN_OUT        = 0x1; // xo output enabled;
                           // Note: I think this means output to XOT
    // Pseudo-resistor selection
    prev20e_r19.XO_RP_LOW        = 0x0;
    prev20e_r19.XO_RP_MEDIA      = 0x1;
    prev20e_r19.XO_RP_MVT        = 0x0;
    prev20e_r19.XO_RP_SVT        = 0x0;

    prev20e_r19.XO_SLEEP = 0x0;
    *REG_XO_CONF1 = prev20e_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(10000); // >= 1ms

    prev20e_r19.XO_ISOLATE = 0x0;
    *REG_XO_CONF1 = prev20e_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(10000); // >= 1ms

    prev20e_r19.XO_DRV_START_UP = 0x1;
    *REG_XO_CONF1 = prev20e_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(30000); // >= 1s

    prev20e_r19.XO_SCN_CLK_SEL = 0x1;
    *REG_XO_CONF1 = prev20e_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(3000); // >= 300us

    prev20e_r19.XO_SCN_CLK_SEL = 0x0;
    prev20e_r19.XO_SCN_ENB     = 0x0;
    *REG_XO_CONF1 = prev20e_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(30000);  // >= 1s

    prev20e_r19.XO_DRV_START_UP = 0x0;
    prev20e_r19.XO_DRV_CORE     = 0x1;
    prev20e_r19.XO_SCN_CLK_SEL  = 0x1;
    *REG_XO_CONF1 = prev20e_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);

    enable_xo_timer();
    reset_xo_cnt();
    start_xo_cnt();

    // BREAKPOint 0x03
    mbus_write_message32(0xBA, 0x03);

}

volatile uint16_t XO_TO_SEC_MPLIER_SHIFT = 15;
volatile uint16_t xo_to_sec_mplier = 32768;  // 2^15
#define XO_SYNC_THRESH 4096 // 3 consecutive reads should be less than 1/8 seconds apart

uint32_t get_timer_cnt_xo() {
    // v5.2.9: doing a majority vote to deal with sync problems
    uint32_t t1 = ((*REG_XOT_VAL_U & 0xFFFF) << 16) | (*REG_XOT_VAL_L & 0xFFFF);
    uint32_t t2 = ((*REG_XOT_VAL_U & 0xFFFF) << 16) | (*REG_XOT_VAL_L & 0xFFFF);
    uint32_t t3 = ((*REG_XOT_VAL_U & 0xFFFF) << 16) | (*REG_XOT_VAL_L & 0xFFFF);
    uint32_t raw_cnt;

    if(t2 - t1 < XO_SYNC_THRESH) {
        // if t1 and t2 are close, pick t2
        raw_cnt = t2;
    }
    else {
        // else, the only two cases are t1, t3 are close and t2, t3 are close => pick t3
        raw_cnt = t3;
    }

    uint64_t temp = mult(raw_cnt, xo_to_sec_mplier);
    return right_shift(temp, XO_TO_SEC_MPLIER_SHIFT);
}

#define XO_MAX_DAY_TIME_IN_SEC 86400
#define XO_TO_SEC_SHIFT 15
#define MAX_UPDATE_TIME 3600    // v5.2.9: Upper bounds on time difference between two updates

void update_system_time() {
    // mbus_write_message32(0xC2, xo_sys_time);
    // mbus_write_message32(0xC1, xo_sys_time_in_sec);
    // mbus_write_message32(0xC0, xo_day_time_in_sec);

    // FIXME: global time doesn't reset bug
    uint32_t last_update_val = xo_sys_time;
    xo_sys_time = get_timer_cnt_xo();

    // check if XO clock has stopped
    if(xo_sys_time == last_xo_sys_time) {
    	set_system_error(0x3);
    }

    // calculate the difference in seconds
    uint32_t diff = (xo_sys_time >> XO_TO_SEC_SHIFT) - (last_update_val >> XO_TO_SEC_SHIFT); // Have to shift individually to account for underflow

    if(xo_sys_time < last_update_val) {
        // xo timer overflowed, want to make sure that 0x00000 - 0x1FFFF = 1
        diff += 0x20000;
    }

    // v5.2.9
    if(diff > MAX_UPDATE_TIME) {
    	set_system_error(0x4);
    }

    xo_sys_time_in_sec += diff;
    xo_day_time_in_sec += diff;

    if(xo_day_time_in_sec >= XO_MAX_DAY_TIME_IN_SEC) {
        xo_day_time_in_sec -= XO_MAX_DAY_TIME_IN_SEC;
        day_count++;
    }

    // mbus_write_message32(0xC2, xo_sys_time);
    // mbus_write_message32(0xC1, xo_sys_time_in_sec);
    // mbus_write_message32(0xC0, xo_day_time_in_sec);
}

// divide any time by 60 to get representation in minutes
uint32_t divide_by_60(uint32_t source) {
    uint32_t upper = source >> 5;   // divide by 32
    uint32_t lower = source >> 6;   // divide by 64
    uint32_t res;

    while(1) {
        res = (upper + lower) >> 1;  // average of upper and lower
        uint32_t comp = res * 60;

        if(source < comp) {
            upper = res;
        }
        else if(source - comp < 60) {
            break;
        }
        else {
            lower = res;
        }
    }
    return res;
}

bool xo_check_is_day() {
    update_system_time();
    return (DAY_START_TIME <= xo_day_time_in_sec) && (xo_day_time_in_sec < DAY_END_TIME);
}

/**********************************************
 * Compression Functions
 **********************************************/

#define LONG_INT_LEN 4
#define LOG2_RES 5

uint64_t right_shift(uint64_t input, int8_t shift) {
    int8_t i;
    if(shift >= 0) {
        for(i = 0; i < shift; i++) {
        input = input >> 1;
    }
    }
    else {
        for(i = 0; i > shift; i--) {
        input = input << 1;
    }
    }
    return input;
}

// right shift an array
// + is right shift, - is left shift
// if shift_len is greater than or equal to 32, data must be 0
// if right_shift, data must be 0
// if left shift, then shift data into the array
void right_shift_arr(uint32_t* arr, uint32_t data, uint16_t arr_len, int16_t shift_len) {
    if(shift_len == 0) {
        return;
    }
    else if(shift_len >= 32) {
        data = 0;
    }

    int8_t sign = shift_len > 0? 1 : -1;

    // get abs(shift_len)
    if(sign == -1) {
        shift_len = -shift_len;
    }

    // filter data
    data &= (right_shift(1, -shift_len) - 1);

    uint16_t start, back;

    if(sign == 1) {
        start = arr_len - 1;
        back = 0;
    }
    else {
        start = 0;
        back = arr_len - 1;
    }

    uint16_t i;

    // shift word by word
    while(shift_len >= 32) {
        for(i = start; i != back; i -= sign) {
            arr[i] = arr[i - sign];
        }
        arr[back] = 0;
        shift_len -= 32;
    }

    int8_t shift_len_complement = shift_len - 32;

    if(sign == -1) {
        shift_len = -shift_len;
        shift_len_complement = -shift_len_complement;
    }

    for(i = start; i != back; i -= sign) {
        arr[i] = right_shift(arr[i], shift_len);
        arr[i] |= right_shift(arr[i - sign], shift_len_complement);
    }

    arr[back] = right_shift(arr[back], shift_len);
    if(sign == -1) {
        arr[back] |= data;
    }
}

uint64_t mult(uint32_t lhs, uint16_t rhs) {
    uint32_t res1 = 0, res2 = 0;
    res1 = (lhs >> 16) * rhs;
    res2 = (lhs & 0xFFFF) * rhs;
    return right_shift(res1, -16) + res2;
}

const uint16_t LOG_CONST_ARR[5] = {0b1011010100000100,
                                   0b1001100000110111,
                                   0b1000101110010101,
                                   0b1000010110101010,
                                   0b1000001011001101};

typedef uint32_t* long_int;

static void long_int_assign(long_int dest, const long_int src) {
    uint16_t i;
    for(i = 0; i < LONG_INT_LEN; i++) {
        dest[i] = src[i];
    }
}

static void long_int_mult(const long_int lhs, const uint16_t rhs, long_int res) {
    uint32_t carry_in = 0;
    uint32_t temp_res[LONG_INT_LEN];
    uint16_t i;
    for(i = 0; i < LONG_INT_LEN; i++) {
        uint64_t temp = mult(lhs[i], rhs) + carry_in;
        carry_in = temp >> 32;
        temp_res[i] = temp & 0xFFFFFFFF;
    }
    long_int_assign(res, temp_res);
}

static bool long_int_lte(const long_int lhs, const long_int rhs) {
    int8_t i;
    for(i = 3; i >= 0; i--) {
    if(lhs[i] != rhs[i]) {
        return lhs[i] < rhs[i];
    }
    }
    return false;
}

uint16_t log2(uint64_t input) {
    // mbus_write_message32(0xE7, input & 0xFFFFFFFF);
    // mbus_write_message32(0xE8, input >> 32);

    if(input == 0) { return 0; }

    uint32_t temp_result[LONG_INT_LEN], input_storage[LONG_INT_LEN];
    int8_t i;
    for(i = 0; i < LONG_INT_LEN; i++) {
        temp_result[i] = input_storage[i] = 0;
    }

    input_storage[0] = input & 0xFFFFFFFF;
    input_storage[1] = input >> 32;
    uint16_t out = 0;

    for(i = 47; i >= 0; i--) {
        if(right_shift(input, i) & 0b1) {
            uint64_t temp = right_shift(1, -i);
            temp_result[0] = temp & 0xFFFFFFFF;
            temp_result[1] = temp >> 32;
            out = i << LOG2_RES;
            break;
        }
    }
    for(i = 0; i < LOG2_RES; i++) {
        uint32_t new_result[4];
        long_int_mult(temp_result, LOG_CONST_ARR[i], new_result);
        long_int_mult(input_storage, (1 << 15), input_storage);

        if(long_int_lte(new_result, input_storage)) {
            long_int_assign(temp_result, new_result);
            // out |= (1 << (LOG2_RES - 1 - i));
        out |= right_shift(1, -(LOG2_RES - 1 - i));
        }
        else {
            long_int_mult(temp_result, (1 << 15), temp_result);
        }

    }

    // mbus_write_message32(0xE9, out & 0x7FF);
    return out & 0x7FF;
}

// code_cache is what is actually stored in memory without packet headers, 
// which will be calculated when sending
// packet header = 1 bit light_or_temp, 5 bit chip ID, 6 bit packet number
#define CODE_CACHE_LEN 9
#define CODE_CACHE_MAX_REMAINDER 272 // 320 - 12 * 4 = 272
#define LIGHT_HEADER_SIZE 12    // 1 bit light or temp, 5 bit chip ID, 6 bit packet number
#define LIGHT_CONTENT_SIZE 68 // 80 - 12 = 68
uint32_t code_cache[CODE_CACHE_LEN];
uint16_t code_cache_remainder = CODE_CACHE_MAX_REMAINDER;
uint16_t code_addr = 0; // address to start storing the final data from
uint16_t max_unit_count = 0;    // (code_addr >> 2) / 9, increment everytime code_addr is incremented
uint16_t radio_unit_counter = 0;
uint16_t radio_beacon_counter = 0;
uint16_t check_pmu_counter = 0;
uint16_t radio_rest_counter = 0;
uint16_t light_packet_num = 0;  // light and temp packet number are to disambiguate between units, the same unit doesn't have to have the same packet number between two radio out cycles
uint16_t temp_packet_num = 0;   
uint16_t last_log_light = 0;

#define UNIT_HEADER_SIZE 9      // 2 bit day_state, 7 bit starting idx
#define LIGHT_TIME_LENGTH 17    // 17 bit time in min
bool has_header = false;
bool has_time = false;

// requires code_cache to have enough space
void store_code(int32_t code, uint16_t len) {
    right_shift_arr(code_cache, code, CODE_CACHE_LEN, -len);
    code_cache_remainder -= len;
}

void flush_code_cache() {
    if(code_cache_remainder < CODE_CACHE_MAX_REMAINDER) {
        // this leaves exactly 16 bits of blank space at the front of the code cache
        right_shift_arr(code_cache, 0, CODE_CACHE_LEN, -code_cache_remainder);
        // The beginning of code cache should always be 1
        // Mark as light
        code_cache[0] &= 0x0000FFFF;    // FIXME: this line is not needed
        code_cache[0] |= 0x80000000;

        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, code_cache, CODE_CACHE_LEN - 1);
        code_addr += CODE_CACHE_LEN << 2;
        max_unit_count++;
        code_cache_remainder = CODE_CACHE_MAX_REMAINDER;

        has_header = false;
        has_time = false;
    }
}

void store_to_code_cache(uint16_t log_light, uint16_t start_idx, uint32_t starting_time_in_min) {
    uint16_t code_idx = 0;
    uint16_t len_needed = 0;
    uint16_t code = 0;
    int16_t diff = log_light - last_log_light;
    last_log_light = log_light;

    // calculate len needed
    if(diff < 32 && diff >= -32) {
        code_idx = diff + 32;
        len_needed = light_code_lengths[code_idx];
    }
    else if(diff < 256 && diff >= -256) {
        code_idx = 64;
        len_needed = light_code_lengths[code_idx] + 9;
    }
    else {
        code_idx = 65;
        len_needed = light_code_lengths[code_idx] + 11;
    }
    code = light_diff_codes[code_idx];

    if(!has_time) {
        len_needed += LIGHT_TIME_LENGTH;
    }

    if(!has_header) {
        len_needed += UNIT_HEADER_SIZE;
    }

    if(code_cache_remainder < len_needed) {
        flush_code_cache();
    }

    // 17 starting timestamp is separate for each day state
    if(!has_time) {
        store_code(starting_time_in_min & 0x1FFFF, 17);
        mbus_write_message32(0xC4, starting_time_in_min);
        has_time = true;
    }

    // 2 bit day_state, 7 bit starting idx
    if(!has_header) {
        store_code(day_state, 2);
        store_code(start_idx, 7);
        mbus_write_message32(0xC5, (day_state << 16) | start_idx);
        has_header = true;
    }

    if(code_cache_remainder == CODE_CACHE_MAX_REMAINDER - UNIT_HEADER_SIZE - LIGHT_TIME_LENGTH) {
        // if first data in unit, just store full data
        store_code(log_light, 11);
    }
    else {
        // else store difference
        store_code(code, light_code_lengths[code_idx]);

        if(code_idx == 64) {
            store_code(diff, 9);
        }
        else if(code_idx == 65) {
            // if storing 11 bits, just store data in full
            store_code(log_light, 11);
        }
    }

}

void store_day_state_stop() {
    uint16_t len_needed = light_code_lengths[66];
    if(code_cache_remainder < len_needed) {
        flush_code_cache();
    }
    else {
        store_code(light_diff_codes[66], len_needed);
    }
}

/**********************************************
 * Sampling functions
 **********************************************/

void write_to_proc_cache(uint16_t data) {
    const uint16_t len = 11;
    data &= (right_shift(1, -len) - 1);

    if(len >= proc_cache_remainder) {
        // if no more cache space, store and switch to new cache
        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) cache_addr, proc_cache, PROC_CACHE_LEN - 1);
        cache_addr += (PROC_CACHE_LEN << 2);
        proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
    }

    right_shift_arr(proc_cache, data, PROC_CACHE_LEN, -len);
    proc_cache_remainder -= len;
}

uint16_t read_next_from_proc_cache() {
    uint16_t res = 0;

    if(day_state == DUSK) {
        // start reading from the bottom
        if(proc_cache_remainder < 11) {
            // decrement address before reading because we start on an incorrect address
            cache_addr -= PROC_CACHE_LEN << 2;
            set_halt_until_mbus_trx();
            mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) cache_addr, PRE_ADDR, proc_cache, PROC_CACHE_LEN - 1);
            set_halt_until_mbus_tx();
            proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
        }
        res = proc_cache[PROC_CACHE_LEN - 1] & 0x7FF;
        proc_cache_remainder -= 11;
        right_shift_arr(proc_cache, 0, PROC_CACHE_LEN, 11);
    }
    else {
        if(proc_cache_remainder < 11) {
            set_halt_until_mbus_trx();
            mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) cache_addr, PRE_ADDR, proc_cache, PROC_CACHE_LEN - 1);
            set_halt_until_mbus_tx();
            // increment address after we read from memory because we start on the correct addr
            cache_addr += PROC_CACHE_LEN << 2;
            proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
        }

        res = right_shift(proc_cache[0], 20) & 0x7FF;
        proc_cache_remainder -= 11;

        right_shift_arr(proc_cache, 0, PROC_CACHE_LEN, -11);
    }
    return res;
}

// These invariants are used to prevent pathological corner cases from crashing the system
bool check_new_edge(uint32_t target) {
    if(day_state == DAWN) {
        return (target > EDGE_MARGIN2 + XO_32_MIN) && (target < MID_DAY_TIME - EDGE_MARGIN1 - XO_32_MIN);
    }
    else {
        return (target > MID_DAY_TIME + EDGE_MARGIN1 + XO_32_MIN) && (target < MAX_DAY_TIME - EDGE_MARGIN2 - XO_32_MIN);
    }
}


void set_new_state() {
    // reset new state variables
    cache_addr = CACHE_START_ADDR;
    proc_cache_remainder = PROC_CACHE_MAX_REMAINDER;
    max_idx = 0;
    threshold_idx = IDX_INIT;
    min_light_idx = IDX_INIT;
    sum = MAX_UINT16;

    // set day_state specific variables
    if(day_state == DAWN) {
        cur_sunrise = next_sunrise == 0? cur_sunrise : next_sunrise;
        cur_edge = cur_sunrise;
        day_state_start_time = cur_sunrise - EDGE_MARGIN2;
        day_state_end_time = cur_sunrise + EDGE_MARGIN1;
    }
    else if(day_state == DUSK) {
        cur_sunset = next_sunset == 0? cur_sunset : next_sunset;
        cur_edge = cur_sunset;
        day_state_start_time = cur_sunset - EDGE_MARGIN1;
        day_state_end_time = cur_sunset + EDGE_MARGIN2;
    }
    else {
        // V5.2.1: align everything to the minute
        // day_state == NOON or NIGHT
        uint32_t day_time_in_min = divide_by_60(xo_day_time_in_sec);
        // TODO: check if this needs a bounds check
        cur_edge = day_time_in_min * 60 + XO_32_MIN;
        day_state_start_time = cur_edge;
        if(day_state == NOON) {
            day_state_end_time = cur_sunset - EDGE_MARGIN1 - XO_10_MIN;
        }
        else {
            day_state_end_time = cur_sunrise - EDGE_MARGIN2 - XO_10_MIN;
        }
    }

    // set new light measure time
    next_light_meas_time = day_state_start_time + xo_sys_time_in_sec - xo_day_time_in_sec;

    // if wrapping around. TODO: test this
    if(day_state_start_time < xo_day_time_in_sec) {
        next_light_meas_time += MAX_DAY_TIME;
    }

    // save first time in day_state
    light_meas_start_time_in_min = divide_by_60(next_light_meas_time - sys_sec_to_min_offset);
    mbus_write_message32(0xB8, light_meas_start_time_in_min);
}

void sample_light() {
    uint16_t log_light = log2(lnt_sys_light);
    uint16_t last_avg_light = avg_light;
    write_to_proc_cache(log_light);

    // store to running sum
    if(sum == MAX_UINT16) {
        // if uninitialized, initialize running avg
        uint16_t i;
        for(i = 0; i < 8; i++) {
            running_avg[i] = log_light;
            running_avg_time[i] = xo_day_time_in_sec;
        }
        sum = log_light << 3;
    } 
    else {
        sum -= running_avg[rot_idx];
        sum += log_light;

        running_avg[rot_idx] = log_light;
        running_avg_time[rot_idx] = xo_day_time_in_sec;
        rot_idx = (rot_idx + 1) & 7;
    }

    avg_light = sum >> 3;
    
    // record min_light in day_state
    if(min_light_idx == IDX_INIT || avg_light < min_light
            || (day_state == DAWN && avg_light == min_light)) { // find last min_light at dawn
        min_light = avg_light;
        min_light_idx = max_idx;
    }

    uint32_t target = 0;
    // test if crosses threshold
    if(day_state == DAWN && threshold_idx == IDX_INIT
            && avg_light >= EDGE_THRESHOLD && last_avg_light < EDGE_THRESHOLD) {
        threshold_idx = max_idx;
        target = running_avg_time[(rot_idx + 3) & 7];
    }
    else if(day_state == DUSK && threshold_idx == IDX_INIT
            && avg_light <= EDGE_THRESHOLD && last_avg_light > EDGE_THRESHOLD) {
        threshold_idx = max_idx;
        target = running_avg_time[(rot_idx + 3) & 7];
    }

    // set new edge time after verifying target validity
    if(check_new_edge(target)) {
        if(target > cur_edge + MAX_EDGE_SHIFT) {
            target = cur_edge + MAX_EDGE_SHIFT;
        }
        else if(target < cur_edge - MAX_EDGE_SHIFT) {
            target = cur_edge - MAX_EDGE_SHIFT;
        }

        // check edge invariants before setting new edge
        if(day_state == DAWN && target > 0 && target < MID_DAY_TIME 
                && (cur_sunset - target > XO_270_MIN)) {
            next_sunrise = target;
        }
        else if(day_state == DUSK && target > MID_DAY_TIME && target < MAX_DAY_TIME 
                && (target - cur_sunrise > XO_270_MIN)) {
            next_sunset = target;
        }
    }

    // save the time before updating
    uint32_t light_meas_end_time_in_min = divide_by_60(next_light_meas_time - sys_sec_to_min_offset);
    
    // set next light measure time
    if(day_state == DAWN || day_state == DUSK) {
        next_light_meas_time += XO_1_MIN;
    }
    else {
        next_light_meas_time += XO_32_MIN;
    }

    bool new_state = false;
    uint32_t temp = xo_day_time_in_sec + next_light_meas_time - xo_sys_time_in_sec;
    if(day_state != NIGHT) {
        new_state = (temp >= day_state_end_time);
    }
    else {
        new_state = (temp >= day_state_end_time && temp < MID_DAY_TIME);
    }

    if(new_state) {
        // FIXME: turn it back on later
        *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
        *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
            
        if(day_state != NIGHT) {
            // resample and stoie
            uint16_t starting_idx = 0;
            int16_t start, end, sign;
            uint32_t sample_time_in_min;

            if(day_state == DUSK) {
                // from last to first
                // manual set remainder to current data in proc_cache to start reading
                proc_cache_remainder = PROC_CACHE_MAX_REMAINDER - proc_cache_remainder;
                // if crossed threshold
                if(threshold_idx != IDX_INIT) {
                    if(threshold_idx + THRESHOLD_IDX_SHIFT > max_idx) {
                        starting_idx = max_idx;
                    }
                    else {
                        starting_idx = threshold_idx + THRESHOLD_IDX_SHIFT;
                    }
                }
                else {
                    starting_idx = min_light_idx;
                }

                sample_time_in_min = light_meas_end_time_in_min;
                mbus_write_message32(0xB9, light_meas_start_time_in_min);

                // last to first
                start = max_idx;
                end = -1;
                sign = -1;
            }
            else {
                // going from first to last, left shift 0s into proc_cache
                right_shift_arr(proc_cache, 0, PROC_CACHE_LEN, 1 - proc_cache_remainder);
                mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) cache_addr, proc_cache, PROC_CACHE_LEN - 1);

                proc_cache_remainder = 0; // manually set to 0 to start reading
                cache_addr = CACHE_START_ADDR;

                if(day_state == NOON) {
                    starting_idx = 0;
                }
                else if(threshold_idx != IDX_INIT) {
                    if(threshold_idx < THRESHOLD_IDX_SHIFT) {
                        starting_idx = 0;
                    }
                    else {
                        starting_idx = threshold_idx - THRESHOLD_IDX_SHIFT;
                    }
                }
                else {
                    starting_idx = min_light_idx;
                }

                sample_time_in_min = light_meas_start_time_in_min;

                // first to last
                start = 0;
                end = max_idx + 1;
                sign = 1;
            }

            int16_t i;
            uint16_t next_sample_idx = starting_idx;
            uint16_t sample_idx = 0;
            uint16_t interval_idx = 0;
            has_time = false;       // Store time at the start of each day state TOOD: check if this is correct
            for(i = start; i != end; i += sign) {
                uint16_t log_light = read_next_from_proc_cache();
                if(i == next_sample_idx) {
                    store_to_code_cache(log_light, sample_idx, sample_time_in_min);

                    sample_idx++;
                    if(day_state == NOON) {
                        next_sample_idx++;
                    }
                    else {
                        if(sample_idx >= resample_indices[interval_idx]) {
                            interval_idx++;
                        }

                        next_sample_idx += (intervals[interval_idx] * sign);
                    }
                }

                if(day_state == NOON) {
                    sample_time_in_min += 32;
                }
                else if(day_state == DAWN) {
                    sample_time_in_min += 1;
                }
                else {
                    sample_time_in_min -= 1;
                }
            }
            store_day_state_stop();
        }
    
        // reset new state variables
        day_state = (day_state + 1) & 0b11;
        set_new_state();
    }
    else {
        max_idx++;
    }
}

/**********************************************
 * Temp sensor functions (SNTv4)
 **********************************************/

static void temp_sensor_start() {
    sntv4_r01.TSNS_RESETn = 1;
    sntv4_r01.TSNS_EN_IRQ = 1;
    mbus_remote_register_write(SNT_ADDR, 1, sntv4_r01.as_int);
}

static void temp_sensor_reset() {
    sntv4_r01.TSNS_RESETn = 0;
    mbus_remote_register_write(SNT_ADDR, 1, sntv4_r01.as_int);
}

static void snt_ldo_vref_on() {
    sntv4_r00.LDO_EN_VREF = 1;
    mbus_remote_register_write(SNT_ADDR, 0, sntv4_r00.as_int);
}

static void snt_ldo_power_on() {
    sntv4_r00.LDO_EN_IREF = 1;
    sntv4_r00.LDO_EN_LDO  = 1;
    mbus_remote_register_write(SNT_ADDR, 0, sntv4_r00.as_int);
}

static void snt_ldo_power_off() {
    sntv4_r00.LDO_EN_VREF = 0;
    sntv4_r00.LDO_EN_IREF = 0;
    sntv4_r00.LDO_EN_LDO  = 0;
    mbus_remote_register_write(SNT_ADDR, 0, sntv4_r00.as_int);
}

static void temp_sensor_power_on() {
    // Un-powergate digital block
    sntv4_r01.TSNS_SEL_LDO = 1;
    mbus_remote_register_write(SNT_ADDR, 1, sntv4_r01.as_int);
    // Un-powergate analog block
    sntv4_r01.TSNS_EN_SENSOR_LDO = 1;
    mbus_remote_register_write(SNT_ADDR, 1, sntv4_r01.as_int);

    delay(MBUS_DELAY);

    // Release isolation
    sntv4_r01.TSNS_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR, 1, sntv4_r01.as_int);
}

static void temp_sensor_power_off() {
    sntv4_r01.TSNS_RESETn        = 0;
    sntv4_r01.TSNS_SEL_LDO       = 0;
    sntv4_r01.TSNS_EN_SENSOR_LDO = 0;
    sntv4_r01.TSNS_ISOLATE       = 1;
    mbus_remote_register_write(SNT_ADDR, 1, sntv4_r01.as_int);
}

static void operation_temp_run() {
    if(snt_state == SNT_IDLE) {

        // Turn on snt ldo vref; requires ~30 ms to settle
        // TODo: figure out delay time
        snt_ldo_vref_on();
        delay(MBUS_DELAY);

        snt_state = SNT_TEMP_LDO;

    }
    if(snt_state == SNT_TEMP_LDO) {
        // Power on snt ldo
        snt_ldo_power_on();

        // Power on temp sensor
        temp_sensor_power_on();
        delay(MBUS_DELAY);

        snt_state = SNT_TEMP_START;
    }
    if(snt_state == SNT_TEMP_START) {
        // Use TIMER32 as a timeout counter
        wfi_timeout_flag = 0;
        config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default
        
        // Start temp sensor
        temp_sensor_start();

        // Wait for temp sensor output or TIMER32
    WFI();

        // Turn off timer32
        *TIMER32_GO = 0;

        snt_state = SNT_TEMP_READ;
    }
    if(snt_state == SNT_TEMP_READ) {
        if(wfi_timeout_flag) {
            // if timeout, set error msg
            sys_err(0x01000000);
        }
        else {
            snt_sys_temp_code = *REG0;
            
            // turn off temp sensor and ldo
            temp_sensor_power_off();
            snt_ldo_power_off();

            snt_state = SNT_IDLE;
        }
    }
}

// temp storage
#define TEMP_CACHE_LEN 9
#define TEMP_CACHE_MAX_REMAINDER 272 // 320 - 12 * 4 = 272
#define TEMP_PACKET_HEADER_SIZE 12 // 1 bit light or temp, 5 bit chip ID, 6 bit packet number
#define TEMP_UNIT_HEADER_SIZE  13 // 7 bit day count, 6 bit timestamp in half hour
#define TEMP_CONTENT_SIZE 68 // 80 - 12 = 68
#define TEMP_RES 7

volatile uint32_t temp_cache[TEMP_CACHE_LEN];
volatile uint16_t temp_cache_remainder = TEMP_CACHE_MAX_REMAINDER;
volatile uint16_t last_log_temp = 0;

void flush_temp_cache() {
    if(temp_cache_remainder < TEMP_CACHE_MAX_REMAINDER) {
        // left shift 0s into unused space
        right_shift_arr(temp_cache, 0, TEMP_CACHE_LEN, -temp_cache_remainder);

        // mark temp packet
        temp_cache[0] &= 0x0000FFFF;

        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, temp_cache, TEMP_CACHE_LEN - 1);
        code_addr += TEMP_CACHE_LEN << 2;
        max_unit_count++;
        temp_cache_remainder = TEMP_CACHE_MAX_REMAINDER;
    }
}

void store_code_to_temp_cache(uint16_t code, uint16_t len) {
    right_shift_arr(temp_cache, code, TEMP_CACHE_LEN, -len);
    temp_cache_remainder -= len;
}

void sample_temp() {
    // Take 3 bits above decimal point and 4 bits under
    uint16_t log_temp = log2(snt_sys_temp_code) >> (8 - TEMP_RES); 
    int8_t diff = log_temp - last_log_temp;
    last_log_temp = log_temp;

    uint16_t code_idx = 0;
    uint16_t len_needed = 0;
    uint16_t code = 0;


    if(diff < 2 && diff >= -2) {
        code_idx = diff + 2;
        len_needed = temp_code_lengths[code_idx];
        code = temp_diff_codes[code_idx];
    }
    else {
        code_idx = 4;
        len_needed = temp_code_lengths[code_idx] + TEMP_RES;
        code = temp_diff_codes[code_idx];   // FIXME: redundant code, move outside of block
    }

    if(temp_cache_remainder < len_needed) {
        flush_temp_cache();
    }

    // increment temp index here to make sure we store the correct index
    store_temp_index++;

    if(store_temp_index > 47) {
        store_temp_index = 0;   // resets when taking the midnight data
    }

    if(temp_cache_remainder == TEMP_CACHE_MAX_REMAINDER) {
        // if first data, store timestamp and full data
        store_code_to_temp_cache(day_count, 7);
        store_code_to_temp_cache(store_temp_index, 6);
        store_code_to_temp_cache(log_temp, TEMP_RES);
    }
    else {
        // else just store diff
        store_code_to_temp_cache(code, temp_code_lengths[code_idx]);

        if(code_idx == 4) {
            // if storing 7 bits, just store full data
            store_code_to_temp_cache(log_temp, TEMP_RES);
        }
    }
}

/**********************************************
 * Light functions (LNTv1A)
 **********************************************/

static void lnt_init() {
    //////// TIMER OPERATION //////////
    // make variables local to save space
    lntv1a_r01_t lntv1a_r01 = LNTv1A_R01_DEFAULT;
    lntv1a_r02_t lntv1a_r02 = LNTv1A_R02_DEFAULT;
    lntv1a_r04_t lntv1a_r04 = LNTv1A_R04_DEFAULT;
    lntv1a_r05_t lntv1a_r05 = LNTv1A_R05_DEFAULT;
    // lntv1a_r06_t lntv1a_r06 = LNTv1A_R06_DEFAULT;
    lntv1a_r17_t lntv1a_r17 = LNTv1A_R17_DEFAULT;
    lntv1a_r20_t lntv1a_r20 = LNTv1A_R20_DEFAULT;
    lntv1a_r21_t lntv1a_r21 = LNTv1A_R21_DEFAULT;
    lntv1a_r22_t lntv1a_r22 = LNTv1A_R22_DEFAULT;
    lntv1a_r40_t lntv1a_r40 = LNTv1A_R40_DEFAULT;
    
    // TIMER TUNING  
    lntv1a_r22.TMR_S = 0x1; // Default: 0x4
    lntv1a_r22.TMR_DIFF_CON = 0x3FFD; // Default: 0x3FFB
    lntv1a_r22.TMR_POLY_CON = 0x1; // Default: 0x1
    mbus_remote_register_write(LNT_ADDR,0x22,lntv1a_r22.as_int);
    delay(MBUS_DELAY*10);
    
    lntv1a_r21.TMR_SEL_CAP = 0x80; // Default : 8'h8
    lntv1a_r21.TMR_SEL_DCAP = 0x3F; // Default : 6'h4
    lntv1a_r21.TMR_EN_TUNE1 = 0x1; // Default : 1'h1
    lntv1a_r21.TMR_EN_TUNE2 = 0x1; // Default : 1'h1
    mbus_remote_register_write(LNT_ADDR,0x21,lntv1a_r21.as_int);
    delay(MBUS_DELAY*10);
    
    // Enable Frequency Monitoring 
    lntv1a_r40.WUP_ENABLE_CLK_SLP_OUT = 0x0; 
    mbus_remote_register_write(LNT_ADDR,0x40,lntv1a_r40.as_int);
    delay(MBUS_DELAY*10);
    
    // TIMER SELF_EN Disable 
    lntv1a_r21.TMR_SELF_EN = 0x0; // Default : 0x1
    mbus_remote_register_write(LNT_ADDR,0x21,lntv1a_r21.as_int);
    delay(MBUS_DELAY*10);
    
    // EN_OSC 
    lntv1a_r20.TMR_EN_OSC = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);
    delay(MBUS_DELAY*10);
    
    // Release Reset 
    lntv1a_r20.TMR_RESETB = 0x1; // Default : 0x0
    lntv1a_r20.TMR_RESETB_DIV = 0x1; // Default : 0x0
    lntv1a_r20.TMR_RESETB_DCDC = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);
    delay(2000); 
    
    // TIMER EN_SEL_CLK Reset 
    lntv1a_r20.TMR_EN_SELF_CLK = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);
    delay(10); 
    
    // TIMER SELF_EN 
    lntv1a_r21.TMR_SELF_EN = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x21,lntv1a_r21.as_int);
    delay(100000); 
    
    // TIMER EN_SEL_CLK Reset 
    lntv1a_r20.TMR_EN_OSC = 0x0; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);
    delay(100);
    
    //////// CLOCK DIVIDER OPERATION //////////
    
    // Run FDIV
    lntv1a_r17.FDIV_RESETN = 0x1; // Default : 0x0
    lntv1a_r17.FDIV_CTRL_FREQ = 0x8; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x17,lntv1a_r17.as_int);
    delay(MBUS_DELAY*10);
    
    //////// LNT SETTING //////////
    
    // Bias Current
    lntv1a_r01.CTRL_IBIAS_VBIAS = 0x7; // Default : 0x7
    lntv1a_r01.CTRL_IBIAS_I = 0x2; // Default : 0x8
    lntv1a_r01.CTRL_VOFS_CANCEL = 0x1; // Default : 0x1
    mbus_remote_register_write(LNT_ADDR,0x01,lntv1a_r01.as_int);
    delay(MBUS_DELAY*10);
         
    // Vbase regulation voltage
    lntv1a_r02.CTRL_VREF_PV_V = 0x1; // Default : 0x2
    mbus_remote_register_write(LNT_ADDR,0x02,lntv1a_r02.as_int);
    delay(MBUS_DELAY*10);
        
    // Set LNT Threshold
    lntv1a_r05.THRESHOLD_HIGH = 0x30; // Default : 12'd40
    lntv1a_r05.THRESHOLD_LOW = 0x10; // Default : 12'd20
    mbus_remote_register_write(LNT_ADDR,0x05,lntv1a_r05.as_int);
    
    // Monitor AFEOUT
    // lntv1a_r06.OBSEN_AFEOUT = 0x0; // Default : 0x0
    // mbus_remote_register_write(LNT_ADDR,0x06,lntv1a_r06.as_int);
    mbus_remote_register_write(LNT_ADDR,0x06,0);
    delay(MBUS_DELAY*10);
    
    // Change Counting Time 
    // lntv1a_r03.TIME_COUNTING = 0xFFFFFF; // Default : 0x258
    mbus_remote_register_write(LNT_ADDR,0x03,0xFFFFFF);
    delay(MBUS_DELAY*10);
    
    // Change Monitoring & Hold Time 
    lntv1a_r04.TIME_MONITORING = 0x00A; // Default : 0x010
    mbus_remote_register_write(LNT_ADDR,0x04,lntv1a_r04.as_int);
    delay(MBUS_DELAY*10);
    
    // Release LDC_PG 
    lntv1a_r00.LDC_PG = 0x0; // Default : 0x1
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*10);
    
    // Release LDC_ISOLATE
    lntv1a_r00.LDC_ISOLATE = 0x0; // Default : 0x1
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*10);
}

static void lnt_start() {
    // Release Reset 
    lntv1a_r00.RESET_AFE = 0x0; // Defhttps://www.dropbox.com/s/yh15ux4h8141vu4/ISSCC2019-Digest.pdf?dl=0ault : 0x1
    lntv1a_r00.RESETN_DBE = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*10);
    
    // LNT Start
    lntv1a_r00.DBE_ENABLE = 0x1; // Default : 0x0
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x0; // Default : 0x0
    lntv1a_r00.MODE_CONTINUOUS = 0x0; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*10);

    lntv1a_r00.WAKEUP_WHEN_DONE = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*100);
}

static void lnt_stop() {
    // // Change Counting Time 
    mbus_remote_register_write(LNT_ADDR,0x03,0xFFFFFF);
    
    set_halt_until_mbus_trx();
    mbus_copy_registers_from_remote_to_local(LNT_ADDR, 0x10, 0x00, 1);
    set_halt_until_mbus_tx();
    lnt_sys_light = right_shift(((*REG1 & 0xFFFFFF) << 24) | (*REG0), lnt_counter_base); // >> lnt_counter_base;

    // // Stop FSM: Counter Idle -> Counter Counting 
    lntv1a_r00.DBE_ENABLE = 0x0; // Default : 0x0
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x0;
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*100);
}

#define TIMER_MARGIN 1024 // margin of error is about 1/32 of a second // limited by the frequency of the LNT timer
uint16_t LNT_MPLIER_SHIFT = 6;
uint16_t xo_lnt_mplier = 0x73;
uint32_t projected_end_time_in_sec = 0;
uint32_t lnt_meas_time = 0;

static void update_lnt_timer() {
    if(projected_end_time_in_sec == 0) {
        return;
    }
    uint32_t projected_end_time = projected_end_time_in_sec << XO_TO_SEC_SHIFT;

    if(xo_sys_time - projected_end_time > TIMER_MARGIN 
    && xo_sys_time_in_sec >= projected_end_time_in_sec) {
        xo_lnt_mplier--;
    }
    else if(projected_end_time - xo_sys_time > TIMER_MARGIN 
        && xo_sys_time_in_sec <= projected_end_time_in_sec) {
        xo_lnt_mplier++;
    }
    // mbus_write_message32(0xE2, lnt_meas_time);
    // mbus_write_message32(0xE3, projected_end_time_in_sec);
    // mbus_write_message32(0xE4, xo_sys_time_in_sec);
    // mbus_write_message32(0xE5, projected_end_time);
    // mbus_write_message32(0xE6, xo_sys_time);
    // mbus_write_message32(0xE7, projected_end_time - xo_sys_time);
    // mbus_write_message32(0xE8, xo_lnt_mplier);
}

static void set_lnt_timer() {
    // wake up at projected_end_time_in_sec
    // our wake up mechanism will be aligned to the second
    
    // mbus_write_message32(0xCE, xo_sys_time_in_sec);
    // mbus_write_message32(0xCE, projected_end_time_in_sec);

    // LNT FSM stuck fix
    int count = 0;
    int lnt_state = 0xaabb;
    while(count < 5) {
        count++;
        uint32_t projected_end_time = projected_end_time_in_sec << XO_TO_SEC_SHIFT;

        update_system_time();
        uint64_t temp = mult(projected_end_time - xo_sys_time, xo_lnt_mplier);

        lnt_meas_time = right_shift(temp, LNT_MPLIER_SHIFT + XO_TO_SEC_SHIFT - 2); // the -2 is empirical
        // uint32_t val = (end_time - xo_sys_time_in_sec) * 4;
        lntv1a_r03.TIME_COUNTING = lnt_meas_time;
        mbus_remote_register_write(LNT_ADDR, 0x03, lntv1a_r03.as_int);
        lnt_start();

        set_halt_until_mbus_trx();
        mbus_copy_registers_from_remote_to_local(LNT_ADDR, 0x16, 0, 0);
        set_halt_until_mbus_tx();

        lnt_state = *REG0;
        if(lnt_state == 0x1) {      // if LNT_COUNTER_STATE == STATE_COUNTING
            break;
        }

        lnt_stop();

    }
    if(count == 5) {
        set_system_error(0x5);
    }

    if(error_code == 5) {
    
        // Not going to wake up again, send beacons to alert
        radio_data_arr[2] = 0xFF00 | CHIP_ID;
        radio_data_arr[1] = error_code;
        radio_data_arr[0] = lnt_state;
        int i;
        for(i = 0; i < 10; i++) {
            delay(10000);
            send_beacon();
        }
    }

    // radio_data_arr[0] = projected_end_time_in_sec;
    // radio_data_arr[1] = xo_sys_time_in_sec;
    // radio_data_arr[2] = lnt_meas_time;
    // send_beacon();

    // mbus_write_message32(0xCF, temp & 0xFFFFFFFF);
    // mbus_write_message32(0xCF, temp >> 32);
}


// set next wake up time, projected_end_time_in_sec == next_light_meas_time, then sample light
void set_projected_end_time() {
    uint32_t temp = next_light_meas_time - projected_end_time_in_sec;
    if(temp <= XO_8_MIN) {
        projected_end_time_in_sec = next_light_meas_time;
        if(temp <= XO_1_MIN) {
            lnt_counter_base = 0;
        }
        else if(temp <= XO_2_MIN) {
            lnt_counter_base = 1;
        }
        else {
            lnt_counter_base = 3;
        }
    }
    else if(temp <= XO_18_MIN) {
        // set 8 minute buffer time to make sure wake up time is power of 2
        // Gives a minimum of 2 minutes of sleep time to set to 8 minutes before light meas
        projected_end_time_in_sec = next_light_meas_time - XO_8_MIN;
    }
    else {
        // 32 minute interval case
        projected_end_time_in_sec += XO_8_MIN;
    }
}

/**********************************************
 * MEM Functions
 **********************************************/


/**********************************************
 * CRC16 Encoding
 **********************************************/

#define DATA_LEN 96
#define CRC_LEN 16

uint32_t* crcEnc16()
{
    // intialization
    uint16_t i;

    uint16_t poly = 0xc002;
    uint16_t poly_not = ~poly;
    uint16_t remainder = 0x0000;
    uint16_t remainder_shift = 0x0000;
    uint32_t data2 = (radio_data_arr[2] << CRC_LEN) + (radio_data_arr[1] >> CRC_LEN);
    uint32_t data1 = (radio_data_arr[1] << CRC_LEN) + (radio_data_arr[0] >> CRC_LEN);
    uint32_t data0 = radio_data_arr[0] << CRC_LEN;

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
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff));
    }

    static uint32_t msg_out[1];
    msg_out[0] = data0 + remainder;

    //radio_data_arr[0] = data2;
    //radio_data_arr[1] = data1;
    //radio_data_arr[2] = data0;

    return msg_out;    
}


/**********************************************
 * PMU functions (PMUv11)
 **********************************************/
static void pmu_reg_write(uint32_t reg_addr, uint32_t reg_data) {
#ifdef USE_PMU
    set_halt_until_mbus_trx();
    mbus_remote_register_write(PMU_ADDR, reg_addr, reg_data);
    set_halt_until_mbus_tx();
#endif
}

/*static void pmu_set_adc_period(uint32_t val) {
#ifdef USE_PMU
    // Updated for PMUv9
    pmu_reg_write(0x3C,         // PMU_EN_CONTROLLER_DESIRED_STATE_ACTIVE
                ((1 <<  0) |    // state_sar_scn_on
                 (0 <<  1) |    // state_wait_for_clock_cycles
                 (1 <<  2) |    // state_wait_for_time
                 (1 <<  3) |    // state_sar_scn_reset
                 (1 <<  4) |    // state_sar_scn_stabilized
                 (1 <<  5) |    // state_sar_scn_ratio_roughly_adjusted
                 (1 <<  6) |    // state_clock_supply_switched
                 (1 <<  7) |    // state_control_supply_switched
                 (1 <<  8) |    // state_upconverter_on
                 (1 <<  9) |    // state_upconverter_stabilized
                 (1 << 10) |    // state_refgen_on
                 (0 << 11) |    // state_adc_output_ready
                 (0 << 12) |    // state_adc_adjusted
                 (0 << 13) |    // state_sar_scn_ratio_adjusted
                 (1 << 14) |    // state_downconverter_on
                 (1 << 15) |    // state_downconverter_stabilized
                 (1 << 16) |    // state_vdd_3p6_turned_on
                 (1 << 17) |    // state_vdd_1p2_turned_on
                 (1 << 18) |    // state_vdd_0p6_turned_on
                 (0 << 19) |    // state_vbat_read
                 (1 << 20)));   // state_state_horizon

    // Register 0x36: PMU_EN_TICK_REPEAT_VBAT_ADJUST
    pmu_reg_write(0x36, val);

    // Register 0x33: PMU_EN_TICK_ADC_RESET
    pmu_reg_write(0x33, 2);

    // Register 0x34: PMU_ENTICK_ADC_CLK
    pmu_reg_write(0x34, 2);

    // Updated for PMUv9
    pmu_reg_write(0x3C,         // PMU_EN_CONTROLLER_DESIRED_STATE_ACTIVE
                ((1 <<  0) |    // state_sar_scn_on
                 (0 <<  1) |    // state_wait_for_clock_cycles
                 (1 <<  2) |    // state_wait_for_time
                 (1 <<  3) |    // state_sar_scn_reset
                 (1 <<  4) |    // state_sar_scn_stabilized
                 (1 <<  5) |    // state_sar_scn_ratio_roughly_adjusted
                 (1 <<  6) |    // state_clock_supply_switched
                 (1 <<  7) |    // state_control_supply_switched
                 (1 <<  8) |    // state_upconverter_on
                 (1 <<  9) |    // state_upconverter_stabilized
                 (1 << 10) |    // state_refgen_on
                 (0 << 11) |    // state_adc_output_ready
                 (0 << 12) |    // state_adc_adjusted
                 (0 << 13) |    // state_sar_scn_ratio_adjusted
                 (1 << 14) |    // state_downconverter_on
                 (1 << 15) |    // state_downconverter_stabilized
                 (1 << 16) |    // state_vdd_3p6_turned_on
                 (1 << 17) |    // state_vdd_1p2_turned_on
                 (1 << 18) |    // state_vdd_0p6_turned_on
                 (0 << 19) |    // state_vbat_read
                 (1 << 20)));   // state_state_horizon
#endif
}*/

static void pmu_set_adc_period(uint32_t val) {
#ifdef USE_PMU
    // Updated for PMUv9
    pmu_reg_write(0x3C,         // PMU_EN_CONTROLLER_DESIRED_STATE_ACTIVE
               ((1 <<  0) |    // state_sar_scn_on
                (1 <<  1) |    // state_wait_for_clock_cycles
                (1 <<  2) |    // state_wait_for_time
                (1 <<  3) |    // state_sar_scn_reset
                (1 <<  4) |    // state_sar_scn_stabilized
                (1 <<  5) |    // state_sar_scn_ratio_roughly_adjusted
                (1 <<  6) |    // state_clock_supply_switched
                (1 <<  7) |    // state_control_supply_switched
                (1 <<  8) |    // state_upconverter_on
                (1 <<  9) |    // state_upconverter_stabilized
                (1 << 10) |    // state_refgen_on
                (0 << 11) |    // state_adc_output_ready
                (0 << 12) |    // state_adc_adjusted
                (0 << 13) |    // state_sar_scn_ratio_adjusted
                (1 << 14) |    // state_downconverter_on
                (1 << 15) |    // state_downconverter_stabilized
                (1 << 16) |    // state_vdd_3p6_turned_on
                (1 << 17) |    // state_vdd_1p2_turned_on
                (1 << 18) |    // state_vdd_0p6_turned_on
                (0 << 19) |    // state_vbat_read
                (1 << 20)));   // state_state_horizon
    
    // Register 0x36: PMU_EN_TICK_REPEAT_VBAT_ADJUST
    pmu_reg_write(0x36, val);
    
    // Register 0x33: PMU_EN_TICK_ADC_RESET
    pmu_reg_write(0x33, 2);
    
    // Register 0x34: PMU_ENTICK_ADC_CLK
    pmu_reg_write(0x34, 2);
    
    // Updated for PMUv9
    //pmu_reg_write(0x3C,         // PMU_EN_CONTROLLER_DESIRED_STATE_ACTIVE
    //((1 <<  0) |    // state_sar_scn_on
    // (1 <<  1) |    // state_wait_for_clock_cycles
    // (1 <<  2) |    // state_wait_for_time
    // (1 <<  3) |    // state_sar_scn_reset
    // (1 <<  4) |    // state_sar_scn_stabilized
    // (1 <<  5) |    // state_sar_scn_ratio_roughly_adjusted
    // (1 <<  6) |    // state_clock_supply_switched
    // (1 <<  7) |    // state_control_supply_switched
    // (1 <<  8) |    // state_upconverter_on
    // (1 <<  9) |    // state_upconverter_stabilized
    // (1 << 10) |    // state_refgen_on
    // (0 << 11) |    // state_adc_output_ready
    // (0 << 12) |    // state_adc_adjusted
    // (0 << 13) |    // state_sar_scn_ratio_adjusted
    // (1 << 14) |    // state_downconverter_on
    // (1 << 15) |    // state_downconverter_stabilized
    // (1 << 16) |    // state_vdd_3p6_turned_on
    // (1 << 17) |    // state_vdd_1p2_turned_on
    // (1 << 18) |    // state_vdd_0p6_turned_on
    // (0 << 19) |    // state_vbat_read
    // (1 << 20)));   // state_state_horizon
#endif
}

static void pmu_set_active_clk(uint32_t setting) {
#ifdef USE_PMU
    uint16_t r = (setting >> 24) & 0xFF;
    uint16_t l = (setting >> 16) & 0xFF;
    uint16_t base = (setting >> 8) & 0xFF;
    uint16_t l_1p2 = setting & 0xFF;

    // mbus_write_message32(0xDE, setting);

    // The first register write to PMU needs to be repeated
    // Register 0x16: V1P2 ACTIVE
    pmu_reg_write(0x16,         // PMU_EN_SAR_TRIM_V3_ACTIVE
                ((0 << 19) |    // enable PFM even during periodic reset
                 (0 << 18) |    // enable PFM even when Vref is not used as ref
                 (0 << 17) |    // enable PFM
                 (3 << 14) |    // comparator clock division ratio
                 (0 << 13) |    // enable main feedback loop
                 (r <<  9) |    // frequency multiplier r
                 (l_1p2 << 5) | // frequency multiplier l (actually l+1)
                 (base)));      // floor frequency base (0-63)

    pmu_reg_write(0x16,         // PMU_EN_SAR_TRIM_V3_ACTIVE
                ((0 << 19) |    // enable pfm even during periodic reset
                 (0 << 18) |    // enable pfm even when Vref is not used as ref
                 (0 << 17) |    // enable pfm
                 (3 << 14) |    // comparator clock division ratio
                 (0 << 13) |    // enable main feedback loop
                 (r <<  9) |    // frequency multiplier r
                 (l_1p2 << 5) | // frequency multiplier l (actually l+1)
                 (base)));      // floor frequency base (0-63)

    // Register 0x18: V3P6 ACTIVE
    pmu_reg_write(0x18,         // PMU_EN_UPCONVERTER_TRIM_V3_ACTIVE
                ((3 << 14) |    // desired vout/vin ratio; default: 0
                 (0 << 13) |    // enable main feedback loop
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l
                 (base)));      // floor frequency base (0-63)

    // Register 0x1A: V0P6 ACTIVE
    pmu_reg_write(0x1A,         // PMU_EN_DOWNCONVERTER_TRIM_V3_ACTIVE
                ((0 << 13) |    // enable main feedback loop
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l
                 (base)));      // floor frequency base (0-63)
#endif
}

static void pmu_set_sleep_clk(uint32_t setting) {
#ifdef USE_PMU
    uint16_t r = (setting >> 24) & 0xFF;
    uint16_t l = (setting >> 16) & 0xFF;
    uint16_t base = (setting >> 8) & 0xFF;
    uint16_t l_1p2 = setting & 0xFF;

    // Register 0x17: V3P6 SLEEP
    pmu_reg_write(0x17,         // PMU_EN_UPCONVERTER_TRIM_V3_SLEEP
                ((3 << 14) |    // desired vout/vin ratio; default: 0
                 (0 << 13) |    // enable main feedback loop
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l (actually l+1)
                 (base)));      // floor frequency base (0-63)

    // Register 0x15: V1P2 sleep
    pmu_reg_write(0x15,         // PMU_EN_SAR_TRIM_V3_SLEEP
                ((0 << 19) |    // enable pdm even during periodic reset
                 (0 << 18) |    // enable pfm even when Vref is not used as ref
                 (0 << 17) |    // enable pfm
                 (3 << 14) |    // comparator clock division ratio
                 (0 << 13) |    // enable main feedack loop
                 (r <<  9) |    // frequency multiplier r
                 (l_1p2 << 5) | // frequency multiplier l (actually l+1)
                 (base)));      // floor frequency base (0-63)

    // Register 0x19: V0P6 SLEEP
    pmu_reg_write(0x19,         // PMU_EN_DOWNCONVERTER_TRIM_V3_SLEEP
                ((0 << 13) |    // enable main feedback loop
                 (r <<  9) |    // frequency multiplier r
                 (l <<  5) |    // frequency multiplier l (actually l+1)
                 (base)));      // floor frequency base (0-63)
#endif
}

/*static void pmu_set_sar_conversion_ratio(uint16_t ratio) {
#ifdef USE_PMU
    uint16_t i;
    // only toggling bit 13 to prevent current spikes
    for(i = 0; i < 2; i++) {
        pmu_reg_write(0x05,         // PMU_EN_SAR_RATIO_OVERRIDE; default: 12'h000
            ((i << 13) |    // enable override setting [12] (1'b1)
             (0 << 12) |    // let vdd_clk always connect to vbat
             (1 << 11) |    // enable override setting [10] (1'h0)
             (0 << 10) |    // have the converter have the periodic reset (1'h0)
             (1 <<  9) |    // enable override setting [8:0] (1'h0)
             (0 <<  8) |    // switch input / output power rails for upconversion (1'h0)
             (1 <<  7) |    // enable override setting [6:0] (1'h0)
             (ratio)));  // binary converter's conversion ratio (7'h00)
    }
#endif
}*/

void pmu_set_sar_ratio (uint32_t sar_ratio) {

    // Debug
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, 0x00000000);
    #endif

    // Just to confirm that ADC has been disabled in Active
    // Also disable 'horizon' in Active so that this sar ratio change takes place only once.
    pmu_reg_write (60, // 0x3C (CTRL_DESIRED_STATE_ACTIVE)
        (( 1 << 0)  // sar_on
        | (1 << 1)  // wait_for_clock_cycles
        | (1 << 2)  // not used
        | (1 << 3)  // sar_reset
        | (1 << 4)  // sar_stabilized
        | (1 << 5)  // sar_ratio_roughly_adjusted
        | (1 << 6)  // clock_supply_switched
        | (1 << 7)  // control_supply_switched
        | (1 << 8)  // upc_on
        | (1 << 9)  // upc_stabilized
        | (1 << 10) // refgen_on
        | (0 << 11) // adc_output_ready
        | (0 << 12) // adc_adjusted
        | (0 << 13) // sar_ratio_adjusted
        | (1 << 14) // dnc_on
        | (1 << 15) // dnc_stabilized
        | (1 << 16) // vdd_3p6_turned_on
        | (1 << 17) // vdd_1p2_turned_on
        | (1 << 18) // vdd_0P6_turned_on
        | (0 << 19) // vbat_read_only
        | (0 << 20) // horizon
    ));

    // Read the current ADC offset
    pmu_reg_write(0x00,0x03);
    uint32_t adc_vbat   = (*REG0 & 0xFF0000) >> 16;
    uint32_t adc_offset = (*REG0 & 0x007F00) >> 8;
    uint32_t adc_dout   = (*REG0 & 0x0000FF) >> 0;

    // Debug
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x01 << 24) | *REG0));
    #endif

    // Override ADC Output so that the ADC always outputs near 0
    pmu_reg_write (2, // 0x02 (ADC_CONFIG_OVERRIDE)
        ( (1 << 23)          // Enable ADC config override
        | (0 << 17)          // ADC_SAMPLE_VOLTAGE
        | (1 << 16)          // ADC_REF_VOLTAGE
        | (0 << 8)           // ADC_SAMPLINB_BIT (8 bits)
        | (0 << 7)           // ADC_SAMPLINB_LSB
        | (adc_offset << 0)  // ADC_OFFSET_CANCEL (7 bits)
    ));

    // Debug
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x02 << 24) | *REG0));
    #endif

    // Set the desired SAR Ratio
    pmu_reg_write (10, // 0x0A (SAR_RATIO_MINIMUM)
       ( (sar_ratio << 0)    // SAR_RATIO_MINIMUM (7-bits)
    ));

    // Debug
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x03 << 24) | *REG0));
    #endif

    // Disable SAR_RESET override
    pmu_reg_write (5, // 0x05 (SAR_RATIO_OVERRIDE)
       ( (1 << 13)  // Enable [12]
       | (0 << 12)  // Let VDD_CLK always connected to VBAT
       | (0 << 11)  // Enable [10] (Default: 1)
       | (0 << 10)  // SAR_RESET
       | (0 << 9)   // Enable [8]
       | (0 << 8)   // Switch input/output power rails for upconversion
       | (0 << 7)   // Enable [6:0]
       | (0 << 0)   // SAR_RATIO (7 bits)
    ));

    // Debug
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x04 << 24) | *REG0));
    #endif

    // Run 'sar_ratio_adjusted'
    pmu_reg_write (60, // 0x3C (CTRL_DESIRED_STATE_ACTIVE)
        ((  1 << 0) // sar_on
        | (1 << 1)  // wait_for_clock_cycles
        | (1 << 2)  // not used
        | (1 << 3)  // sar_reset
        | (1 << 4)  // sar_stabilized
        | (1 << 5)  // sar_ratio_roughly_adjusted
        | (1 << 6)  // clock_supply_switched
        | (1 << 7)  // control_supply_switched
        | (1 << 8)  // upc_on
        | (1 << 9)  // upc_stabilized
        | (1 << 10) // refgen_on
        | (1 << 11) // adc_output_ready
        | (0 << 12) // adc_adjusted
        | (1 << 13) // sar_ratio_adjusted
        | (1 << 14) // dnc_on
        | (1 << 15) // dnc_stabilized
        | (1 << 16) // vdd_3p6_turned_on
        | (1 << 17) // vdd_1p2_turned_on
        | (1 << 18) // vdd_0P6_turned_on
        | (0 << 19) // vbat_read_only
        | (0 << 20) // horizon
    ));

    // Debug
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x05 << 24) | *REG0));
    #endif

uint32_t pmu_sar_ratio;
    // Now we don't know how long it would take to set the sar ratio.
    // so let's keep checking the actual sar ratio until it becomes as same as SAR_RATIO_MINIMUM
    do {
        // Read the current SAR RATIO
        pmu_reg_write(0x00,0x04);
        pmu_sar_ratio   = *REG0 & 0x7F;

        // Debug
        #ifdef DEBUG_SET_SAR_RATIO
            mbus_write_message32(0xD8, ((0x06 << 24) | *REG0));
        #endif

    } while (pmu_sar_ratio != sar_ratio);

    // Disable ADC & Auto Ratio Adjust
    pmu_reg_write (60, // 0x3C (CTRL_DESIRED_STATE_ACTIVE)
        ((  1 << 0) // sar_on
        | (1 << 1)  // wait_for_clock_cycles
        | (1 << 2)  // not used
        | (1 << 3)  // sar_reset
        | (1 << 4)  // sar_stabilized
        | (1 << 5)  // sar_ratio_roughly_adjusted
        | (1 << 6)  // clock_supply_switched
        | (1 << 7)  // control_supply_switched
        | (1 << 8)  // upc_on
        | (1 << 9)  // upc_stabilized
        | (1 << 10) // refgen_on
        | (0 << 11) // adc_output_ready
        | (0 << 12) // adc_adjusted
        | (0 << 13) // sar_ratio_adjusted
        | (1 << 14) // dnc_on
        | (1 << 15) // dnc_stabilized
        | (1 << 16) // vdd_3p6_turned_on
        | (1 << 17) // vdd_1p2_turned_on
        | (1 << 18) // vdd_0P6_turned_on
        | (0 << 19) // vbat_read_only
        | (0 << 20) // horizon
    ));

    // Debug
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x07 << 24) | *REG0));
    #endif

    // Override SAR_RESET again
    pmu_reg_write (5, // 0x05 (SAR_RATIO_OVERRIDE)
       ( (1 << 13)  // Enable [12]
       | (0 << 12)  // Let VDD_CLK always connected to VBAT
       | (1 << 11)  // Enable [10] (Default: 1)
       | (0 << 10)  // SAR_RESET
       | (0 << 9)   // Enable [8]
       | (0 << 8)   // Switch input/output power rails for upconversion
       | (0 << 7)   // Enable [6:0]
       | (0 << 0)   // SAR_RATIO (7 bits)
    ));

    // Debug
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x08 << 24) | *REG0));
    #endif

    // Disable ADC override
    pmu_reg_write (2, // 0x02 (ADC_CONFIG_OVERRIDE)
        ( (0 << 23) // Enable ADC config override
        | (0 << 17) // ADC_SAMPLE_VOLTAGE
        | (0 << 16) // ADC_REF_VOLTAGE
        | (0 << 8)  // ADC_SAMPLINB_BIT (8 bits)
        | (0 << 7)  // ADC_SAMPLINB_LSB
        | (0 << 0)  // ADC_OFFSET_CANCEL (7 bits)
    ));

    // Debug
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x09 << 24) | *REG0));
    #endif

}

static void pmu_prep_sar_ratio () {

    // Minimize the SAR_RESET pulse width
    pmu_reg_write (33, // 0x21 (TICK_SAR_RESET)
        ( (1 << 0)     // TICK_SAR_RESET
    ));

    // Set SAR_RATIO_OFFSET_DOWN=0 to enable the ratio override.
    pmu_reg_write (7, // 0x7 (SAR_RATIO_OFFSET_DOWN_ACTIVE)
        ( (0 << 0)    // SAR_RATIO_OFFSET_DOWN_ACTIVE
    ));

    // Set SAR_RATIO_OFFSET_DOWN=0 to enable the ratio override.
    pmu_reg_write (6, // 0x6 (SAR_RATIO_OFFSET_DOWN_SLEEP)
        ( (0 << 0)    // SAR_RATIO_OFFSET_DOWN_SLEEP
    ));

    // Just to make sure you do not 'stall' adc operation in Active
    pmu_reg_write (58, // 0x3A (CTRL_STALL_STATE_ACTIVE)
        (( 0 << 0)
    ));

}

// 0 : normal active
// 1 : radio active
// 2 : sleep
static void pmu_setting_temp_based(uint16_t mode) {
#ifdef USE_PMU
    int8_t i;
    for(i = PMU_SETTINGS_LEN; i >= 0; i--) {
        // mbus_write_message32(0xB3, 0xFFFFFFFF);
        if(i == 0 || snt_sys_temp_code >= PMU_TEMP_THRESH[i - 1]) {
            if(mode == 0) {
            pmu_set_active_clk(PMU_ACTIVE_SETTINGS[i]);
                pmu_set_sar_ratio(PMU_ACTIVE_SAR_SETTINGS[i]);
            }
            else if(mode == 2) {
                pmu_set_sleep_clk(PMU_SLEEP_SETTINGS[i]);
                pmu_set_sar_ratio(PMU_SLEEP_SAR_SETTINGS[i]);
            }
            else {
            pmu_set_active_clk(PMU_RADIO_SETTINGS[i]);
                pmu_set_sar_ratio(PMU_RADIO_SAR_SETTINGS[i]);
            }
            break;
        }
    }
    return;
#endif
}

// safe sleep mode
static void pmu_set_sleep_low() {
#ifdef USE_PMU
    pmu_set_sleep_clk(PMU_SLEEP_SETTINGS[2]);
    pmu_set_sar_ratio(PMU_SLEEP_SAR_SETTINGS[2]);
#endif
}

static void pmu_set_clk_init() {
#ifdef USE_PMU
    pmu_setting_temp_based(0);
#endif
}

static void pmu_adc_reset_setting() {
#ifdef USE_PMU
    // PMU ADC will be automatically reset when system wakes up
    // Updated for PMUv9
    pmu_reg_write(0x3C,         // PMU_EN_CONTROLLER_DESIRED_STATE_ACTIVE
                ((1 <<  0) |    // state_sar_scn_on
                 (1 <<  1) |    // state_wait_for_clock_cycles
                 (1 <<  2) |    // state_wait_for_time
                 (1 <<  3) |    // state_sar_scn_reset
                 (1 <<  4) |    // state_sar_scn_stabilized
                 (1 <<  5) |    // state_sar_scn_ratio_roughly_adjusted
                 (1 <<  6) |    // state_clock_supply_switched
                 (1 <<  7) |    // state_control_supply_switched
                 (1 <<  8) |    // state_upconverter_on
                 (1 <<  9) |    // state_upconverter_stabilized
                 (1 << 10) |    // state_refgen_on
                 (0 << 11) |    // state_adc_output_ready
                 (0 << 12) |    // state_adc_adjusted
                 (0 << 13) |    // state_sar_scn_ratio_adjusted
                 (1 << 14) |    // state_downconverter_on
                 (1 << 15) |    // state_downconverter_stabilized
                 (1 << 16) |    // state_vdd_3p6_turned_on // Needed for other functions
                 (1 << 17) |    // state_vdd_1p2_turned_on
                 (1 << 18) |    // state_vdd_0p6_turned_on
                 (0 << 19) |    // state_vbat_read
                 (1 << 20)));   // state_state_horizon
#endif
}

static void pmu_adc_disable() {
#ifdef USE_PMU
    // PMU ADC will be automatically reset when system wakes up
    // Updated for PMUv9
    pmu_reg_write(0x3B,         // PMU_EN_CONTROLLER_DESIRED_STATE_SLEEP
                ((1 <<  0) |    // state_sar_scn_on
                 (1 <<  1) |    // state_wait_for_clock_cycles
                 (1 <<  2) |    // state_wait_for_time
                 (1 <<  3) |    // state_sar_scn_reset
                 (1 <<  4) |    // state_sar_scn_stabilized
                 (1 <<  5) |    // state_sar_scn_ratio_roughly_adjusted
                 (1 <<  6) |    // state_clock_supply_switched
                 (1 <<  7) |    // state_control_supply_switched
                 (1 <<  8) |    // state_upconverter_on
                 (1 <<  9) |    // state_upconverter_stabilized
                 (1 << 10) |    // state_refgen_on
                 (0 << 11) |    // state_adc_output_ready
                 (0 << 12) |    // state_adc_adjusted
                 (1 << 13) |    // state_sar_scn_ratio_adjusted     // Turn on for old adc, off for new adc
                 (1 << 14) |    // state_downconverter_on
                 (1 << 15) |    // state_downconverter_stabilized
                 (1 << 16) |    // state_vdd_3p6_turned_on
                 (1 << 17) |    // state_vdd_1p2_turned_on
                 (1 << 18) |    // state_vdd_0p6_turned_on
                 (0 << 19) |    // state_vbat_read                  // Turn off for old adc
                 (1 << 20)));   // state_state_horizon
#endif
}

static void pmu_adc_enable() {
#ifdef USE_PMU
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE sleep
    // Updated for PMUv9
    pmu_reg_write(0x3B,         // PMU_EN_CONTROLLER_DESIRED_STATE_SLEEP
                ((1 <<  0) |    // state_sar_scn_on
                 (1 <<  1) |    // state_wait_for_clock_cycles
                 (1 <<  2) |    // state_wait_for_time
                 (1 <<  3) |    // state_sar_scn_reset
                 (1 <<  4) |    // state_sar_scn_stabilized
                 (1 <<  5) |    // state_sar_scn_ratio_roughly_adjusted
                 (1 <<  6) |    // state_clock_supply_switched
                 (1 <<  7) |    // state_control_supply_switched
                 (1 <<  8) |    // state_upconverter_on
                 (1 <<  9) |    // state_upconverter_stabilized
                 (1 << 10) |    // state_refgen_on
                 (1 << 11) |    // state_adc_output_ready
                 (0 << 12) |    // state_adc_adjusted               // Turning off offset cancellation
                 (1 << 13) |    // state_sar_scn_ratio_adjusted     // Turn on for old adc, off for new adc
                 (1 << 14) |    // state_downconverter_on
                 (1 << 15) |    // state_downconverter_stabilized
                 (1 << 16) |    // state_vdd_3p6_turned_on
                 (1 << 17) |    // state_vdd_1p2_turned_on
                 (1 << 18) |    // state_vdd_0p6_turned_on
                 (0 << 19) |    // state_vbat_read                  // Turn of for old adc
                 (1 << 20)));   // state_state_horizon
#endif
}

static void pmu_adc_read_latest() {
#ifdef USE_PMU
    // Grab latest pmu adc readings 
    // PMU register read is handled differently
    pmu_reg_write(0x00, 0x03);
    // Updated for pmuv9
    read_data_batadc = *REG0 & 0xFF;    // TODO: check if only need low 8 bits
#endif
}

static void pmu_enable_4V_harvesting() {
#ifdef USE_PMU
    // Updated for PMUv9
    pmu_reg_write(0x0E,         // PMU_EN_VOLTAGE_CLAMP_TRIM
                ((0 << 12) |    // 1: solar short by latched vbat_high (new); 0: follow [10] setting
                 (0 << 11) |    // Reset of vbat_high latch for [12]=1
                 (1 << 10) |    // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
                 (0 <<  9) |    // Enable override setting [8]
                 (0 <<  8) |    // Turn on the harvester-inhibiting switch
                 (3 <<  4) |    // clamp_tune_bottom (increases clamp thresh)
                 (0)));         // clamp_tune_top (decreases clamp thresh)
#endif
}

/*static void pmu_init() {
#ifdef USE_PMU
    pmu_set_clk_init();
    pmu_enable_4V_harvesting();

    // New for PMUv9
    // VBAT_READ_TRIM Register
    pmu_reg_write(0x45,         // FIXME: this register is reserved in PMUv10
                ((0x00 << 9) |  // 0x0 no:mon; mx1: sar conv mon; 0x2: up conv mon; 0x3: down conv mon
                 (0x00 << 8) |  // 1: vbat_read_mode enable; 0: vbat_read_mode_disable
                 (0x48 << 0))); // sampling multiplication factor N; vbat_read out = vbat/1p2*N

    // Disable PMU ADC measurement in active mode
    // PMU_CONTROLLER_STALL_ACTIVE
    // Updated for PMUv9
    pmu_reg_write(0x3A,         // PMU_EN_CONTROLLER_STALL_ACTIVE
                ((1 << 20) |    // ignore state_horizen; default: 1
                 (0 << 19) |    // state_vbat_read
                 (1 << 13) |    // ignore state_adc_output_ready; default: 0
                 (1 << 12) |    // ignore state_adc_output_ready; default:0
                 (1 << 11)));   // ignore state_adc_output_ready; default:0

    pmu_adc_reset_setting();
    pmu_adc_enable();
#endif
}*/

static void pmu_init() {
#ifdef USE_PMU
    pmu_set_clk_init();
    pmu_enable_4V_harvesting();
    
    // New for PMUv9
    // VBAT_READ_TRIM Register
    pmu_reg_write(0x45,         // FIXME: this register is reserved in PMUv10
                ((0x00 << 9) |  // 0x0 no:mon; mx1: sar conv mon; 0x2: up conv mon; 0x3: down conv mon
                 (0x00 << 8) |  // 1: vbat_read_mode enable; 0: vbat_read_mode_disable
                 (0x48 << 0))); // sampling multiplication factor N; vbat_read out = vbat/1p2*N
    
    // Disable PMU ADC measurement in active mode
    // PMU_CONTROLLER_STALL_ACTIVE
    // Updated for PMUv9
    //pmu_reg_write(0x3A,         // PMU_EN_CONTROLLER_STALL_ACTIVE
                //    ((1 << 20) |    // ignore state_horizen; default: 1
    // (0 << 19) |    // state_vbat_read
    // (1 << 13) |    // ignore state_adc_output_ready; default: 0
    // (1 << 12) |    // ignore state_adc_output_ready; default:0
    // (1 << 11)));   // ignore state_adc_output_ready; default:0
    
    pmu_reg_write(0x10,         // PMU_SAR_TRIM_ACTIVE
                ((0x70A20 << 4) |    // remaining fields
                 (1 << 0)));   // bias current; default:4
    
    pmu_adc_reset_setting();
    pmu_adc_enable();
    pmu_prep_sar_ratio();
#endif
}

/**********************************************
 * MRR functions (MRRv7)
 **********************************************/

static void reset_radio_data_arr() {
    uint16_t i;
    for(i = 0; i < 3; i++) { radio_data_arr[i] = 0; }
}

static void radio_power_on(){

    // Turn off PMU ADC
    //pmu_adc_disable();

    // Need to speed up sleep pmu clock
    //pmu_set_sleep_radio();

    // Turn off Current Limter Briefly
    mrrv7_r00.MRR_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Set decap to parallel
    mrrv7_r03.MRR_DCP_S_OW = 0;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
    mrrv7_r03.MRR_DCP_P_OW = 1;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
    delay(MBUS_DELAY);

    // Set decap to series
    mrrv7_r03.MRR_DCP_P_OW = 0;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
    mrrv7_r03.MRR_DCP_S_OW = 1;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
    delay(MBUS_DELAY);

    // Current Limter set-up 
    mrrv7_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Turn on Current Limter
    mrrv7_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Release timer power-gate
    mrrv7_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
    delay(MBUS_DELAY);

    // Turn on timer
    mrrv7_r04.RO_RESET = 0;  //Release Reset TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
    delay(MBUS_DELAY);

    mrrv7_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
    delay(MBUS_DELAY);

    mrrv7_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);

    // Release FSM Sleep
    mrrv7_r11.MRR_RAD_FSM_SLEEP = 0;  // Power on BB
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
    delay(MBUS_DELAY*5); // Freq stab

}

static void radio_power_off() {
    // Need to restore sleep pmu clock

    // Enable PMU ADC
    //pmu_adc_enable();

    // Turn off Current Limter Briefly
    mrrv7_r00.MRR_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Current Limter set-up 
    mrrv7_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Turn on Current Limter
    mrrv7_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Turn off everything
    mrrv7_r03.MRR_TRX_ISOLATEN = 0;     //set ISOLATEN 0
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv7_r03.as_int);

    mrrv7_r11.MRR_RAD_FSM_EN = 0;  //Stop BB
    mrrv7_r11.MRR_RAD_FSM_RSTN = 0;  //RST BB
    mrrv7_r11.MRR_RAD_FSM_SLEEP = 1;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);

    mrrv7_r04.RO_RESET = 1;  //Release Reset TIMER
    mrrv7_r04.RO_EN_CLK = 0; //Enable CLK TIMER
    mrrv7_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);

    // Enable timer power-gate
    mrrv7_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);

    radio_on = 0;
    radio_ready = 0;

}

static void mrr_configure_pulse_width_long() {

    mrrv7_r12.MRR_RAD_FSM_TX_PW_LEN = 12; //50us PW
    mrrv7_r13.MRR_RAD_FSM_TX_C_LEN = 1105;
    mrrv7_r12.MRR_RAD_FSM_TX_PS_LEN = 25; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv7_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);

}

static void send_radio_data_mrr_sub1() {

    // Use timer32 as timeout counter
    set_timer32_timeout(TIMER32_VAL);

    // Turn on Current Limter
    mrrv7_r00.MRR_CL_EN = 1;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Fire off data
    mrrv7_r11.MRR_RAD_FSM_EN = 1;  //Start BB
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);

    // Wait for radio response
    WFI();
    stop_timer32_timeout_check();

    // Turn off Current Limter
    mrrv7_r00.MRR_CL_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    mrrv7_r11.MRR_RAD_FSM_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
}

uint16_t mrr_cfo_val_fine = 0;
uint16_t mrr_trx_cap_antn_tune_coarse = 0;

static void mrr_send_radio_data(uint16_t last_packet) {
    // Sends 192 bit packet, of which 96b is actual data
    // MRR REG_9: reserved for header
    // MRR REG_A: reserved for header
    // MRR REG_B: reserved for header
    // MRR REG_C: reserved for header
    // MRR REG_D: DATA[23:0]
    // MRR REG_E: DATA[47:24]
    // MRR REG_F: DATA[71:48]
    // MRR REG_10: DATA[95:72]

    // if(!mrr_send_enable) {
    //     return;
    // }


    // clean up radio_data_arr[2]
    radio_data_arr[2] = radio_data_arr[2] & 0x0000FFFF;
        
#ifndef USE_RAD
    mbus_write_message32(0xAA, 0xAAAAAAAA);
    mbus_write_message32(0xAA, 0x00000000);
    
    mbus_write_message32(0xE0, radio_data_arr[0]);
    mbus_write_message32(0xE1, radio_data_arr[1]);
    mbus_write_message32(0xE2, radio_data_arr[2]);
    
    mbus_write_message32(0xAA, 0x00000000);
    mbus_write_message32(0xAA, 0xAAAAAAAA);
#endif

    // CRC16 Encoding 
    uint32_t* crc_data = crcEnc16();

#ifndef USE_RAD
    // mbus_write_message32(0xBB, 0xBBBBBBBB);
    // mbus_write_message32(0xBB, 0x00000000);
    // 
    // mbus_write_message32(0xE0, crc_data[0]);
    // mbus_write_message32(0xE1, crc_data[1]);
    // mbus_write_message32(0xE2, crc_data[2]);
    // 
    // mbus_write_message32(0xBB, 0x00000000);
    // mbus_write_message32(0xBB, 0xBBBBBBBB);
#endif

    // TODO: add temp and voltage restrictions

    if(!radio_on) {
        radio_on = 1;
    radio_power_on();
    }
    
    mbus_remote_register_write(MRR_ADDR,0xD, radio_data_arr[0]);
    mbus_remote_register_write(MRR_ADDR,0xE, (radio_data_arr[1] << 8) | (radio_data_arr[0] >> 24));
    mbus_remote_register_write(MRR_ADDR,0xF, (radio_data_arr[2] << 16) | (radio_data_arr[1] >> 16));
    mbus_remote_register_write(MRR_ADDR,0x10, (crc_data[0]) << 8 | (radio_data_arr[2] >> 8));

    // mbus_remote_register_write(MRR_ADDR,0xD, radio_data_arr[0] & 0xFFFFFF);
    // mbus_remote_register_write(MRR_ADDR,0xE, (radio_data_arr[1] << 8) | ((radio_data_arr[0] >> 24) & 0xFF));
    // mbus_remote_register_write(MRR_ADDR,0xF, (radio_data_arr[2] << 16) | ((radio_data_arr[1] >> 16) & 0xFFFF));
    // mbus_remote_register_write(MRR_ADDR,0x10, ((crc_data[0] & 0xFFFF) << 8 | (radio_data_arr[2] >> 8) & 0xFF));

    if (!radio_ready){
    radio_ready = 1;

    // Release FSM Reset
    mrrv7_r11.MRR_RAD_FSM_RSTN = 1;  //UNRST BB
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
    delay(MBUS_DELAY);

    mrrv7_r03.MRR_TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv7_r03.as_int);
    delay(MBUS_DELAY);

    // Current Limter set-up 
    mrrv7_r00.MRR_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    }

#ifdef USE_RAD
    // uint16_t mrr_cfo_val_fine = 0;
    uint16_t count = 0;
    // uint16_t num_packets = 1;
    // if (mrr_freq_hopping) num_packets = mrr_freq_hopping;

    // mrr_cfo_val_fine = 0x0000;

    while (count < mrr_freq_hopping){
    count++;
    // may be able to remove 2 lines below, GC 1/6/20
        // FIXME: We should probably turn WD back on after sending
    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer

    mrrv7_r01.MRR_TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
    mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine;
    mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);
    send_radio_data_mrr_sub1();
    delay(RADIO_PACKET_DELAY);
    // mrr_cfo_val_fine = mrr_cfo_val_fine + mrr_freq_hopping_step; // 1: 0.8MHz, 2: 1.6MHz step
    // 5 hop pattern
    mrr_cfo_val_fine += 8;
    if(mrr_cfo_val_fine > 20) {
        mrr_cfo_val_fine -= 20;
    }
    }

    radio_packet_count++;
#endif
    if (last_packet){
    radio_ready = 0;
    radio_power_off();
    }else {
    mrrv7_r11.MRR_RAD_FSM_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
    }

    // TODO: check if this works 
    // *TIMERWD_GO = 0x1; // Turn on CPU watchdog timer
    // *REG_MBUS_WD = 1; // Enable Mbus watchdog timer
}

static inline void mrr_init() {
    // MRR Settings --------------------------------------
    // using local variables to save space
    mrrv7_r02_t mrrv7_r02 = MRRv7_R02_DEFAULT;
    mrrv7_r07_t mrrv7_r07 = MRRv7_R07_DEFAULT;
    mrrv7_r14_t mrrv7_r14 = MRRv7_R14_DEFAULT;
    mrrv7_r15_t mrrv7_r15 = MRRv7_R15_DEFAULT;
    mrrv7_r1F_t mrrv7_r1F = MRRv7_R1F_DEFAULT;

    // Decap in series
    mrrv7_r03.MRR_DCP_P_OW = 0;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
    mrrv7_r03.MRR_DCP_S_OW = 1;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);

    // Current Limter set-up 
    mrrv7_r00.MRR_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);


    // Turn on Current Limter
    mrrv7_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Wait for charging decap
    // config_timerwd(TIMERWD_VAL);
    // *REG_MBUS_WD = 1500000*3; // default: 1500000
    delay(MBUS_DELAY*200); // Wait for decap to charge

    mrrv7_r1F.LC_CLK_RING = 0x3;  // ~ 150 kHz
    mrrv7_r1F.LC_CLK_DIV = 0x3;  // ~ 150 kHz
    mbus_remote_register_write(MRR_ADDR,0x1F,mrrv7_r1F.as_int);

    //mrr_configure_pulse_width_short();
    mrr_configure_pulse_width_long();

    //mrr_freq_hopping = 5;
    //mrr_freq_hopping_step = 4;
    mrr_freq_hopping = 2;
    mrr_freq_hopping_step = 4; // determining center freq

    mrr_cfo_val_fine_min = 0x0000;

    // RO setup (SFO)
    // Adjust Diffusion R
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF

    // Adjust Poly R
    mbus_remote_register_write(MRR_ADDR,0x08,0x400000); // RO_POLY

    // Adjust C
    mrrv7_r07.RO_MOM = 0x10;
    mrrv7_r07.RO_MIM = 0x10;
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv7_r07.as_int);

    // TX Setup Carrier Freq
    mrrv7_r00.MRR_TRX_CAP_ANTP_TUNE_COARSE = 0xFF;  //ANT CAP 10b unary 830.5 MHz // FIXME: adjust per stack    // TODO: make them the same variable
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
    mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_COARSE = 0xFF; //ANT CAP 10b unary 830.5 MHz
    mrrv7_r01.MRR_TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
    mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
    mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);
    mrrv7_r02.MRR_TX_BIAS_TUNE = 0x1FFF;  //Set TX BIAS TUNE 13b // Max 0x1FFF
    mbus_remote_register_write(MRR_ADDR,0x02,mrrv7_r02.as_int);

    // Turn off RX mode
    mrrv7_r03.MRR_TRX_MODE_EN = 0; //Set TRX mode
    mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);

    mrrv7_r14.MRR_RAD_FSM_TX_POWERON_LEN = 0; //3bits
    mrrv7_r15.MRR_RAD_FSM_RX_HDR_BITS = 0x00;  //Set RX header
    mrrv7_r15.MRR_RAD_FSM_RX_HDR_TH = 0x00;    //Set RX header threshold
    mrrv7_r15.MRR_RAD_FSM_RX_DATA_BITS = 0x00; //Set RX data 1b
    mbus_remote_register_write(MRR_ADDR,0x14,mrrv7_r14.as_int);
    mbus_remote_register_write(MRR_ADDR,0x15,mrrv7_r15.as_int);

    // RAD_FSM set-up 
    // Using first 48 bits of data as header
    mbus_remote_register_write(MRR_ADDR,0x09,0x0);
    mbus_remote_register_write(MRR_ADDR,0x0A,0x0);
    mbus_remote_register_write(MRR_ADDR,0x0B,0x0);
    mbus_remote_register_write(MRR_ADDR,0x0C,0x7AC800);
    mrrv7_r11.MRR_RAD_FSM_TX_H_LEN = 0; //31-31b header (max)
    mrrv7_r11.MRR_RAD_FSM_TX_D_LEN = RADIO_DATA_LENGTH; //0-skip tx data
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);

    mrrv7_r13.MRR_RAD_FSM_TX_MODE = 3; //code rate 0:4 1:3 2:2 3:1(baseline) 4:1/2 5:1/3 6:1/4
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);

    // Mbus return address
    mbus_remote_register_write(MRR_ADDR,0x1E,0x1002);

    // Additional delay for charging decap
    // config_timerwd(TIMERWD_VAL);
    // *REG_MBUS_WD = 1500000; // default: 1500000
    delay(MBUS_DELAY*200); // Wait for decap to charge
}


/**********************************************
 * Initialization functions
 **********************************************/

static void var_init() {
    // initialize variables that could be corrupted at program time
    lnt_counter_base = 0;

    rot_idx = 0;
    avg_light = 0;
    next_sunrise = 0;
    next_sunset = 0;

    code_cache_remainder = CODE_CACHE_MAX_REMAINDER;
    code_addr = 0;
    max_unit_count = 0;
    radio_beacon_counter = 0;
    low_pwr_count = 0;
    temp_packet_num = 0;
    light_packet_num = 0;
    has_header = false;
    has_time = false;

    temp_cache_remainder = TEMP_CACHE_MAX_REMAINDER;

    projected_end_time_in_sec = 0;
    next_radio_debug_time = 0;
    next_light_meas_time = 0;
    last_log_temp = 0;

    mrr_cfo_val_fine = 0;

    error_code = 0;
    error_time = 0;
}

static void operation_init( void ) {
    // BREAKPOINT 0x01
    // mbus_write_message32(0xBA, 0x01);

    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    config_timer32(0, 0, 0, 0);

    // Enumeration
    // bake version num into the code
    // so it will go through operation_init
    enumerated = ENUMID + VERSION_NUM;

    // timing variables don't belong to var_init
    xo_sys_time = 0;
    xo_sys_time_in_sec = 0;
    xo_day_time_in_sec = 0;

    xo_to_sec_mplier = 32768;
    XO_TO_SEC_MPLIER_SHIFT = 15;
    xo_lnt_mplier = 0x73;
    LNT_MPLIER_SHIFT = 6;

    var_init();

#ifdef USE_MEM
    mbus_enumerate(MEM_ADDR);
    delay(MBUS_DELAY);
#endif
#ifdef USE_MRR
    mbus_enumerate(MRR_ADDR);
    delay(MBUS_DELAY);
#endif
#ifdef USE_LNT
    mbus_enumerate(LNT_ADDR);
    delay(MBUS_DELAY);
#endif
#ifdef USE_SNT
    mbus_enumerate(SNT_ADDR);
    delay(MBUS_DELAY);
#endif
#ifdef USE_PMU
    mbus_enumerate(PMU_ADDR);
    delay(MBUS_DELAY);
#endif

    // Default CPU halt function
    set_halt_until_mbus_tx();

    // Initialization
    // xo_init();

    // BREAKPOINT 0x02
    // mbus_write_message32(0xBA, 0x02);
    // mbus_write_message32(0xED, PMU_ACTIVE_SETTINGS[0]);

#ifdef USE_SNT
    sntv4_r01.TSNS_BURST_MODE = 0;
    sntv4_r01.TSNS_CONT_MODE  = 0;
    mbus_remote_register_write(SNT_ADDR, 1, sntv4_r01.as_int);

    sntv4_r07.TSNS_INT_RPLY_SHORT_ADDR = 0x10;
    sntv4_r07.TSNS_INT_RPLY_REG_ADDR   = 0x00;
    mbus_remote_register_write(SNT_ADDR, 7, sntv4_r07.as_int);

    snt_state = SNT_IDLE;
    operation_temp_run();
#endif

#ifdef USE_LNT
    lnt_init();
#endif

#ifdef USE_MRR
    mrr_init();
    radio_on = 0;
    radio_ready = 0;
#endif

#ifdef USE_PMU
    pmu_init();
#endif

#ifdef USE_XO
    xo_init();

    update_system_time();
    update_system_time();
    update_system_time();
#endif
}

/**********************************************
 * End of program sleep operation
 **********************************************/

static void operation_sleep( void ) {
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;

#ifdef USE_MRR
    if(radio_on) {
        radio_power_off();
    }
#endif

    mbus_sleep_all();
    while(1);
}


void set_system_error(uint32_t code) {
    error_code = code;
    error_time = xo_sys_time_in_sec;
    mbus_write_message32(0xBF, code);
}

static void sys_err(uint32_t code)
{
    set_system_error(code);

    // mbus_write_message32(0xAF, code);
    operation_sleep();

    // operation_sleep_notimer();
}

/**********************************************
 * Interrupt handlers
 **********************************************/

void set_goc_cmd() {
    // disable timer
    lnt_stop();

    // goc_component = (*GOC_DATA_IRQ >> 24) & 0xFF;
    // goc_func_id = (*GOC_DATA_IRQ >> 16) & 0xFF;
    // goc_data = *GOC_DATA_IRQ & 0xFFFF;
    projected_end_time_in_sec = 0;
    goc_data_full = *GOC_DATA_IRQ;
    goc_state = 0;
    update_system_time();

    if((*GOC_DATA_IRQ >> 24) == 0x06) {
        // Timeout trigger implemented here to ensure that it's run
        *TIMERWD_GO = 0x1; // Turn on CPU watchdog timer
        *REG_MBUS_WD = 0; // Disable Mbus watchdog timer

        while(1);
    }
}

void handler_ext_int_wakeup     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep      (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_xot        (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0       (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1       (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2       (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3       (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_wakeup( void ) { // WAKEUP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP);
}

void handler_ext_int_gocep( void ) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
}

void handler_ext_int_timer32( void ) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *TIMER32_STAT = 0x0;
    
    wfi_timeout_flag = 1;
}

void handler_ext_int_xot( void ) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_XOT);
}

void handler_ext_int_reg0( void ) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);
}

void handler_ext_int_reg1( void ) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1);
}

void handler_ext_int_reg2( void ) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2);
}

void handler_ext_int_reg3( void ) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3);
}

/**********************************************
 * MAIN function starts here
 **********************************************/

// requires unit_count < max_unit_count
void radio_unit(uint16_t unit_count) {
    uint16_t addr = (unit_count * CODE_CACHE_LEN) << 2;    // CODE_CACHE_LEN is the size of a unit
    pmu_setting_temp_based(1);
    // read out data and check if temp or light
    set_halt_until_mbus_trx();
    mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) addr, PRE_ADDR, code_cache, CODE_CACHE_LEN - 1);
    set_halt_until_mbus_tx();

    uint32_t temp_arr[3];
    uint16_t i;
    bool is_light = ((code_cache[0] & 0x80000000) != 0);
    // radio out four times
    for(i = 0; i < 4; i++) {
        // first 96 bits are read out using temp_arr
        // This is needed because radio_data_arr is big endian
        temp_arr[0] = code_cache[0];
        temp_arr[1] = code_cache[1];
        temp_arr[2] = code_cache[2];

        // clear out top 16 bits
        temp_arr[0] &= 0x0000FFFF;

        // right shift to make space for packet header
        // 12 bits for both light and temp
        right_shift_arr(temp_arr, 0, 3, LIGHT_HEADER_SIZE);

        // insert header
        if(is_light) {
            temp_arr[0] |= (1 << 15) | (CHIP_ID << 10) | (light_packet_num << 4);
            light_packet_num = (light_packet_num + 1) & 0x3F;
        }
        else {
            temp_arr[0] |= (CHIP_ID << 10) | (temp_packet_num << 4);
            temp_packet_num = (temp_packet_num + 1) & 0x3F;
        }

        // assign to radio arr
        radio_data_arr[2] = temp_arr[0];
        radio_data_arr[1] = temp_arr[1];
        radio_data_arr[0] = temp_arr[2];

        mrr_send_radio_data(0);

        // left shift a new packet over
        // 68 bits for both light and temp
        right_shift_arr(code_cache, 0, CODE_CACHE_LEN, -LIGHT_CONTENT_SIZE);
    }
}

void radio_partial_data(uint16_t start_unit_count, uint16_t len) {
    uint16_t i;
    for(i = 0; i < len; i++) {
        if(start_unit_count + i >= max_unit_count) {
            break;
        }
        radio_unit(start_unit_count + i);
    }
}

uint16_t set_send_enable() {
    if(OVERRIDE_RAD) {
        return 1;
    }
    if(snt_sys_temp_code < MRR_TEMP_THRESH_LOW || snt_sys_temp_code > MRR_TEMP_THRESH_HIGH) {
        return 0;
    }
    uint16_t i;
    for(i = 2; i < 6; i++) {
        if(snt_sys_temp_code < PMU_TEMP_THRESH[i]) {
            if(read_data_batadc <= PMU_ADC_THRESH[i - 2]) {
                return 1;
            }
            else {
                return 0;
            }
        }   
    }
    return 1;
}

// return true if PMU ADC output is higher than LOW_THRESH
// that means system should go into LOW_PWR
uint16_t set_low_pwr_low_trigger() {
    uint16_t i;
    for(i = 0; i < 6; i++) {
        if(snt_sys_temp_code < PMU_TEMP_THRESH[i]) {
            if(read_data_batadc > LOW_PWR_LOW_ADC_THRESH[i]) {
                return 1;
            }
            return 0;
        }
    }
    // > 60C
    if(read_data_batadc > LOW_PWR_LOW_ADC_THRESH[6]) {
        return 1;
    }
    return 0;
}

// return true if PMU ADC output is higher than HIGH_THRESH
// return false if PMU ADC output is lower than HIGH_THRESH
// that means system should get out of LOW_PWR
uint16_t set_low_pwr_high_trigger() {
    uint16_t i;
    for(i = 0; i < 6; i++) {
        if(snt_sys_temp_code < PMU_TEMP_THRESH[i]) {
            if(read_data_batadc > LOW_PWR_HIGH_ADC_THRESH[i]) {
                return 1;
            }
            return 0;
        }
    }
    // > 60C
    if(read_data_batadc > LOW_PWR_HIGH_ADC_THRESH[6]) {
        return 1;
    }
    return 0;
}

void send_beacon() {
    pmu_setting_temp_based(1);
    // mrr_send_radio_data(1);
    mrr_send_radio_data(0); // FIXME: change this line in real code
    // pmu_setting_temp_based(0);   // FIXME: set this line back
}

// reduce repeated code
void initialize_state_collect() {
    // initialize day_state
    // starting from an edge 
    if(xo_day_time_in_sec < cur_sunrise - EDGE_MARGIN2 - XO_10_MIN) {
        day_state = DAWN;
    }
    else if(xo_day_time_in_sec < cur_sunset - EDGE_MARGIN1 - XO_10_MIN){
        day_state = DUSK;
    }
    else {
        // trusting the wrap around from set_new_state()
        day_state = DAWN;
    }

    // set new state variables based on day_state
    // setting next sunrise and sunset so the system won't be confused
    next_sunrise = 0;
    next_sunset = 0;
    set_new_state();

    // find suitable temp storage time based on day_state_start_time
    uint32_t store_temp_day_time = 0;

    // this variable should always reflect the last temp data stored
    store_temp_index = 0; 
    while(store_temp_day_time < day_state_start_time) {
        store_temp_day_time += XO_30_MIN;
        store_temp_index++;
    }
    // shift to index of last data stored
    store_temp_index--;
    store_temp_timestamp = store_temp_day_time + xo_sys_time_in_sec - xo_day_time_in_sec;
    // deal with wrap around issue
    if(store_temp_day_time < xo_day_time_in_sec) {
        store_temp_timestamp += MAX_DAY_TIME;
    }
    goc_state = STATE_COLLECT;
    set_projected_end_time();
}

int main() {
    // Only enable relevant interrupts (PREv18)
    *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
          1 << IRQ_REG0 | 1 << IRQ_REG1 | 1 << IRQ_REG2 | 1 << IRQ_REG3);

    if(enumerated != ENUMID + VERSION_NUM) {
        operation_init();
        // set pmu sleep setting
        pmu_setting_temp_based(2);
        operation_sleep();
        // operation_sleep_notimer();
    }

    // must read adc from sleep mode prior to adjusting PMU SAR ratio in active mode
    pmu_adc_read_latest();
    pmu_setting_temp_based(0);

    update_system_time();
    update_lnt_timer();

    // mbus_write_message32(0xEE, *SREG_WAKEUP_SOURCE);
    // mbus_write_message32(0xEE, *GOC_DATA_IRQ);

    // check wakeup is due to GOC
    if(*SREG_WAKEUP_SOURCE & 1) {
        // if(!(*GOC_DATA_IRQ)) {
        //     operation_sleep(); // Need to protect against spurious wakeups
        // }
        set_goc_cmd();
    }

    lnt_stop();

    operation_temp_run();
    pmu_setting_temp_based(0);

    mrr_send_enable = set_send_enable();

    uint16_t goc_data_header = goc_data_full >> 24;
    // common option
    bool option = ((goc_data_full >> 23) & 0x1)? true : false;

    if(goc_data_header == 0x00) {
        // do nothing
    }
   else if(goc_data_header == 0x01) {
        // Alive beacon, safe sleep mode, and set chip id
        bool option2 = ((goc_data_full >> 22) & 0x1)? true : false;
        if(option2) {
            CHIP_ID = goc_data_full & 0x1F;
        }
    
        // send alive beacon
        radio_data_arr[2] = (0x1 << 8) | CHIP_ID;
        radio_data_arr[1] = (VERSION_NUM << 16) | read_data_batadc;
        radio_data_arr[0] = snt_sys_temp_code;
        send_beacon();

        if(option) {
            // safe sleep mode
            // FIXME: this might be bugged
        pmu_set_sleep_low();
            
        operation_sleep();
        }
    }
    else if(goc_data_header == 0x02) {
        // packet blaster: send packets for N seconds, then sleep for M seconds
        uint8_t N = (goc_data_full >> 16) & 0xFF;
        uint8_t M = (goc_data_full >> 8) & 0xFF;

        if(goc_state == 0) {
            op_counter = 0;   
            goc_state = 1;
        }

        pmu_setting_temp_based(1);
        uint32_t start_time_in_sec = xo_sys_time_in_sec;
        if(mrr_send_enable) {
            do {
                // radio out for N seconds
                radio_data_arr[2] = (0x2 << 8) | CHIP_ID;
                radio_data_arr[1] = goc_data_full & 0xFF;
                radio_data_arr[0] = op_counter++;
                mrr_send_radio_data(0);
                update_system_time();
            } while(xo_sys_time_in_sec - start_time_in_sec < N);
        }

        // set M second timer
        projected_end_time_in_sec = xo_sys_time_in_sec + M;
    }
    else if(goc_data_header == 0x03) {
        // characterization
        // first set next wake up time before radioing
        uint16_t N = goc_data_full & 0xFFFF;
        if(goc_state == 0) {
            goc_state = 1;
            projected_end_time_in_sec = xo_sys_time_in_sec + N;
        }
        else {
            projected_end_time_in_sec += N;
        }

        if(mrr_send_enable) {
            pmu_setting_temp_based(1);
            update_system_time();
            radio_data_arr[2] = (0x3 << 8) | CHIP_ID;
            radio_data_arr[1] = xo_sys_time_in_sec;
            radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
            mrr_send_radio_data(!option);

            // if option is set, send 2 packets
            if(option) {
                radio_data_arr[2] = (0x3 << 8) | (0x1 << 7) | CHIP_ID;
                radio_data_arr[1] = (lnt_meas_time << 16) | lnt_sys_light >> 32;
                radio_data_arr[0] = (uint32_t) lnt_sys_light;
                mrr_send_radio_data(1);
            }
        }

    }
    else if(goc_data_header == 0x04) {
        // restart xo clock
        xo_init();

        update_system_time();
        radio_data_arr[2] = (0x4 << 8) | CHIP_ID;
        radio_data_arr[1] = 0;
        radio_data_arr[0] = xo_sys_time;
        send_beacon();

        update_system_time();
        radio_data_arr[2] = (0x4 << 8) | CHIP_ID;
        radio_data_arr[1] = 1;
        radio_data_arr[0] = xo_sys_time;
        send_beacon();
    }
    else if(goc_data_header == 0x05) {
        // battery drain test: wake up every N seconds and take measurements
        // Which is equivalent to waking up every N seconds
        uint16_t N = goc_data_full & 0xFFFF;
        projected_end_time_in_sec = xo_sys_time_in_sec + N;
    }
    else if(goc_data_header == 0x06) {
        // Implemented in interrupt servvice handler
    }
    else if(goc_data_header == 0x07) {
        // start operation
        // new operation
        if(goc_state == STATE_INIT) {
            // reset all variables so that the system has a clean start
            var_init();

            goc_state = STATE_VERIFY;

            projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
            op_counter = 0;
        }
        else if(goc_state == STATE_VERIFY) {
            projected_end_time_in_sec += XO_2_MIN;
            op_counter++;

            if(op_counter <= 2) {
                radio_data_arr[2] = (0xAB << 8) | CHIP_ID;
                radio_data_arr[1] = (op_counter << 28) | (lnt_sys_light & 0xFFFFFFF);
                radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
            }
            else {
                radio_data_arr[2] = (0xCD << 8) | CHIP_ID;
                radio_data_arr[1] = (op_counter << 28) | (xo_sys_time_in_sec & 0xFFFFFFF);
                radio_data_arr[0] = projected_end_time_in_sec;
            }
            if(op_counter >= 4) {
                // go to STATE_WAIT1
                goc_state = STATE_WAIT1;
            }
            send_beacon();
        }
        else if(goc_state == STATE_WAIT1) {
            if(day_count + epoch_days_offset >= start_day_count) {
                initialize_state_collect();
                op_counter = 0;
            }
            else {
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
            }
        }
        else if(goc_state == STATE_COLLECT) {
            // check LOW_PWR
            if(set_low_pwr_low_trigger()) {
                low_pwr_count++;
            }
            else {
                low_pwr_count = 0;
            }
            
            // FIXME: using MID_DAY_TIME for debug here, see if it needs to be removed
            if(day_count + epoch_days_offset >= end_day_count && xo_day_time_in_sec >= MID_DAY_TIME) {
                flush_temp_cache();       // store everything in temp_code_storage
                flush_code_cache();
                update_system_time();

                goc_state = STATE_WAIT2;
                projected_end_time_in_sec += XO_2_MIN;

                // initialize radio variables
                next_beacon_time = 0;
                next_data_time = 0;
            }
            else if(low_pwr_count >= 3) {
                // goto LOW_PWR_MODE
                low_pwr_count = 0;
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
                low_pwr_state_end_day = day_count + 1;
                goc_state = STATE_LOW_PWR;
            }
            else {
                if(projected_end_time_in_sec >= store_temp_timestamp) {
                    store_temp_timestamp += XO_30_MIN;    // increment by 30 minutes

                    sample_temp();
                }
                
                if(projected_end_time_in_sec == next_light_meas_time) {
                    sample_light();
                }

                set_projected_end_time();

                if((op_counter == 0) || (radio_debug && projected_end_time_in_sec >= next_radio_debug_time)) {
                    // radioing out on first collect regardless of radio_debug setting
                    op_counter = 1;
                    next_radio_debug_time = xo_sys_time_in_sec + RADIO_DEBUG_PERIOD;

                    if(mrr_send_enable) {
                        // FIXME: consider removing this after a while
                        radio_data_arr[2] = (0xDE << 8) | CHIP_ID;
                        radio_data_arr[1] = xo_sys_time_in_sec;
                        radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
                        send_beacon();
                    }
                }
            }
        }
        else if(goc_state == STATE_WAIT2) {
            if(projected_end_time_in_sec == next_beacon_time && mrr_send_enable) {
                // send beacon
                radio_data_arr[2] = (0xEF << 8) | CHIP_ID;
                radio_data_arr[1] = (radio_beacon_counter << 28) | (max_unit_count << 20) | xo_day_time_in_sec;
                radio_data_arr[0] = (read_data_batadc << 24) | snt_sys_temp_code;
                send_beacon();

                radio_beacon_counter++;
            }

            // wait for radio_day_count
            uint16_t epoch_day_count = day_count + epoch_days_offset;
            if(epoch_day_count >= radio_day_count) {
                radio_duty_cycle_end_day = epoch_day_count + RADIO_DUTY_DURATION;
                radio_initial_data_start_day = epoch_day_count + RADIO_INITIAL_DATA_DAY;
                radio_day_count = epoch_day_count + RADIO_SEQUENCE_PERIOD;
            }

            // FIXME: this could be somewhat time consuming
            if(xo_sys_time_in_sec + XO_2_MIN >= next_beacon_time && xo_check_is_day()) {
                // find next available beacon time
                uint32_t next_beacon_day_time = RADIO_INITIAL_BEACON_TIME;
                while(xo_day_time_in_sec + XO_2_MIN >= next_beacon_day_time) {
                    next_beacon_day_time += RADIO_BEACON_PERIOD;
                }
                // convert back to sys time
                next_beacon_time = next_beacon_day_time + xo_sys_time_in_sec - xo_day_time_in_sec;
            }
        
            if(xo_sys_time_in_sec + XO_2_MIN >= next_data_time && xo_check_is_day()) {
                // find next available data time
                uint32_t next_data_day_time = RADIO_INITIAL_DATA_TIME;
                while(xo_day_time_in_sec + XO_2_MIN >= next_data_day_time) {
                    next_data_day_time += RADIO_DATA_PERIOD;
                }
                // convert back to sys time
                next_data_time = next_data_day_time + xo_sys_time_in_sec - xo_day_time_in_sec;
            }
        
            // set projected end time
            // don't check mrr_send_enable here
            bool common_flag = (epoch_day_count < radio_duty_cycle_end_day
                                && xo_check_is_day());
            uint32_t default_projected_end_time = projected_end_time_in_sec + PMU_WAKEUP_INTERVAL + XO_2_MIN;

            // check if can send data first
            if(epoch_day_count >= radio_initial_data_start_day
                    && common_flag
                    && next_data_time <= default_projected_end_time) {
                projected_end_time_in_sec = next_data_time;
                // go to radio data state
                goc_state = STATE_RADIO_DATA;

                // initialize radio parameters
                radio_unit_counter = 0;
                check_pmu_counter = 0;
                radio_rest_counter = 0;
                light_packet_num = 0;
                temp_packet_num = 0;
            }
            else if(common_flag
                    && next_beacon_time <= default_projected_end_time) {
                projected_end_time_in_sec = next_beacon_time;
            }
            else {
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
            }
        }
        else if(goc_state == STATE_RADIO_DATA) {
            if(mrr_send_enable) {
                while(1) {
                    if(radio_unit_counter >= max_unit_count) {
                        update_system_time();
                        projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
                        goc_state = STATE_WAIT2;
                        break;
                    }
                    else if(radio_rest_counter >= RADIO_REST_NUM_UNITS) {
                        radio_rest_counter = 0;
                        check_pmu_counter = 0;  // Resetting check_pmu counter as well to avoid concatenating the sleep periods
                        update_system_time();
                        radio_rest_end_time = xo_sys_time_in_sec + RADIO_REST_TIME;
                        // NOTE: the system will sleep for 2 minutes before waking up
                        // So radio rest time should be greater than 2 minutes
                        projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
                        goc_state = STATE_RADIO_REST;
                        break;
                    }
                    else if(check_pmu_counter >= CHECK_PMU_NUM_UNITS) {
                        check_pmu_counter = 0;
                        update_system_time();
                        projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
                        break;
                    }

                    radio_unit(radio_unit_counter);
                    radio_unit_counter++;
                    check_pmu_counter++;
                    radio_rest_counter++;
                }
            }
            else {
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
                
                // transition back to WAIT2 if too late
                if(xo_day_time_in_sec >= DAY_END_TIME) {
                    goc_state = STATE_WAIT2;
                }
            }
        }
        else if(goc_state == STATE_RADIO_REST) {
            // // FIXME: debug
            // radio_data_arr[2] = 0x3DFB;
            // radio_data_arr[1] = xo_sys_time_in_sec;
            // radio_data_arr[0] = radio_rest_end_time;
            // send_beacon();

            if(projected_end_time_in_sec + PMU_WAKEUP_INTERVAL + XO_2_MIN >= radio_rest_end_time) {
                projected_end_time_in_sec = radio_rest_end_time;
                goc_state = STATE_RADIO_DATA;
            } 
            else {
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
            }
        }
        else if(goc_state == STATE_LOW_PWR) {
            // must pass at least 1 day and be around noon to resume operation
            if(day_count >= low_pwr_state_end_day 
                && xo_day_time_in_sec >= (MID_DAY_TIME - PMU_WAKEUP_INTERVAL)
                && xo_day_time_in_sec <= (MID_DAY_TIME + PMU_WAKEUP_INTERVAL)
                && !set_low_pwr_high_trigger()) {
                // reset go back to state collect
                // flushing temp cache is necessary because there's no day state header for temp cache
                flush_temp_cache();

                initialize_state_collect();
            }
            else {
                projected_end_time_in_sec += PMU_WAKEUP_INTERVAL;
            }

        }

        // all error checking here
        if(projected_end_time_in_sec <= xo_sys_time_in_sec) {
            set_system_error(0x02);
        }

        if(error_code != 0) {
            goc_state = STATE_ERROR;
        }
        
        if(goc_state == STATE_ERROR) {
            // FIXME: freeze xo_lnt_mplier
            // keep in mind that the xo clock could no longer be running
            radio_data_arr[2] = 0xFF00 | CHIP_ID;
            radio_data_arr[1] = error_code;
            radio_data_arr[0] = error_time;
            send_beacon();
            update_system_time();
            projected_end_time_in_sec = xo_sys_time_in_sec + PMU_WAKEUP_INTERVAL;
        }
    }
    else if(goc_data_header == 0x08) {
        // light huffman code
        uint8_t index = (goc_data_full >> 16) & 0x7F;

        // readonly
        radio_data_arr[2] = (0x8 << 8) | CHIP_ID;
        radio_data_arr[1] = (index << 4) | light_code_lengths[index];
        radio_data_arr[0] = light_diff_codes[index];
        send_beacon();
    }
    else if(goc_data_header == 0x09) {
        // temp huffman code
        uint8_t index = (goc_data_full >> 16) & 0x7F;
        // readonly
        // if(option) {
        //     temp_diff_codes[index] = goc_data_full & 0xFFF;
        //     temp_code_lengths[index] = (goc_data_full >> 12) & 0xF;
        // }

        radio_data_arr[2] = (0x9 << 8) | CHIP_ID;
        radio_data_arr[1] = (index << 4) | temp_code_lengths[index];
        radio_data_arr[0] = temp_diff_codes[index];
        send_beacon();
    }
    else if(goc_data_header == 0x0A) {
        // epoch days
        if(option) {
            day_count = 0;
            epoch_days_offset = goc_data_full & 0xFFFF;
        }

        radio_data_arr[2] = (0xA << 8) | CHIP_ID;
        radio_data_arr[1] = epoch_days_offset;
        radio_data_arr[0] = day_count;
        send_beacon();
    }
    else if(goc_data_header == 0x0B) {
        // epoch time
        if(option) {
            uint8_t H = (goc_data_full >> 6) & 0x1F;
            uint8_t M = goc_data_full & 0x3F;
            xo_day_time_in_sec = H * 3600 + M * 60;
            sys_sec_to_min_offset = xo_sys_time_in_sec;
        }

        radio_data_arr[2] = (0xB << 8) | CHIP_ID;
        radio_data_arr[1] = mult(86400, epoch_days_offset) + xo_day_time_in_sec; // epoch_time_offset
        radio_data_arr[0] = divide_by_60(xo_sys_time_in_sec - sys_sec_to_min_offset); // sys_time_in_min
        send_beacon();

        radio_data_arr[2] = (0xB << 8) | (0x1 << 6) | CHIP_ID;
        radio_data_arr[1] = xo_sys_time_in_sec;
        radio_data_arr[0] = sys_sec_to_min_offset;
        send_beacon();

        radio_data_arr[2] = (0xB << 8) | (0x2 << 6) | CHIP_ID;
        radio_data_arr[1] = 0;
        radio_data_arr[0] = xo_day_time_in_sec;
        send_beacon();
    }
    else if(goc_data_header == 0x0C) {
        uint8_t option = (goc_data_full >> 21) & 0x7;
        uint16_t N = goc_data_full & 0xFFFF;
        if(option == 1) {
            xo_to_sec_mplier = N;
        }
        else if(option == 2) {
            xo_lnt_mplier = N;
        }
        else if(option == 3) {
            XO_TO_SEC_MPLIER_SHIFT = N;
        }
        else if(option == 4) {
            LNT_MPLIER_SHIFT = N;
        }

        radio_data_arr[2] = (0xC << 8) | CHIP_ID;
        radio_data_arr[1] = (XO_TO_SEC_MPLIER_SHIFT << 16) | xo_to_sec_mplier;
        radio_data_arr[0] = (LNT_MPLIER_SHIFT << 16) | xo_lnt_mplier;
        send_beacon();
    }
    else if(goc_data_header == 0x0D) {
        uint8_t index = (goc_data_full >> 21) & 0x3;
        if(option) {
            resample_indices[index] = goc_data_full & 0x3F;
        }

        radio_data_arr[2] = (0xD << 8) | CHIP_ID;
        radio_data_arr[1] = index;
        radio_data_arr[0] = resample_indices[index];
        send_beacon();
    }
    else if(goc_data_header == 0x0E) {
        if(option) {
            cur_sunrise = ((goc_data_full >> 11) & 0x7FF) * 60;
            cur_sunset = (goc_data_full & 0x7FF) * 60;
        }

        radio_data_arr[2] = (0xE << 8) | CHIP_ID;
        radio_data_arr[1] = cur_sunrise;
        radio_data_arr[0] = cur_sunset;
        send_beacon();
    }
    else if(goc_data_header == 0x0F) {
        uint8_t option = (goc_data_full >> 22) & 0x3;
        uint8_t N = (goc_data_full >> 8) & 0xFF;
        uint8_t M = goc_data_full & 0xFF;
        if(option == 1) {
            EDGE_MARGIN1 = (N - 1) * 60;
            EDGE_MARGIN2 = M * 60;
            IDX_MAX = N + M - 1;
        }
        else if(option == 2) {
            MAX_EDGE_SHIFT = M * 60;
        }
    
        radio_data_arr[2] = (0xF << 8) | CHIP_ID;
        radio_data_arr[1] = (MAX_EDGE_SHIFT << 16) | IDX_MAX;
        radio_data_arr[0] = (EDGE_MARGIN1 << 16) | EDGE_MARGIN2;
        send_beacon();
    }
    else if(goc_data_header == 0x10) {
        uint8_t settings = (goc_data_full >> 20) & 0x7;
        uint8_t index = (goc_data_full >> 17) & 0x7;
        uint8_t U = (goc_data_full >> 16) & 0x1;
        uint16_t N = goc_data_full & 0xFFFF;
        volatile uint32_t* arr = 0;
        if(settings == 0) {
            arr = PMU_ACTIVE_SETTINGS;
        }
        else if(settings == 1) {
            arr = PMU_RADIO_SETTINGS;
        }
        else if(settings == 2) {
            arr = PMU_SLEEP_SETTINGS;
        }
        else if(settings == 3) {
            arr = PMU_ACTIVE_SAR_SETTINGS;
        }
        else if(settings == 4) {
            arr = PMU_RADIO_SAR_SETTINGS;
        }
        else if(settings == 5) {
            arr = PMU_SLEEP_SAR_SETTINGS;
        }
        else if(settings == 6) {
            arr = PMU_TEMP_THRESH;
        }
        else {
            arr = PMU_ADC_THRESH;
        }

        if(option) {
            if(U) {
                arr[index] &= 0x0000FFFF;
                arr[index] |= (N << 16);
            }
            else {
                arr[index] &= 0xFFFF0000;
                arr[index] |= N;
            }
        }
    
        radio_data_arr[2] = (0x10 << 8) | CHIP_ID;
        radio_data_arr[1] = (settings << 8) | index;
        radio_data_arr[0] = arr[index];
        send_beacon();
    }
    else if(goc_data_header == 0x11) {
        // day time config
        if(option) {
            DAY_START_TIME = ((goc_data_full >> 11) & 0x7FF) * 60;
            DAY_END_TIME = (goc_data_full & 0x7FF) * 60;
        }

        radio_data_arr[2] = (0x11 << 8) | CHIP_ID;
        radio_data_arr[1] = DAY_START_TIME;
        radio_data_arr[0] = DAY_END_TIME;
        send_beacon();
    }
    else if(goc_data_header == 0x12) {
        // Radio config
        uint8_t option2 = (goc_data_full >> 22) & 0x1;
        uint8_t N = (goc_data_full >> 17) & 0x1F;
        uint32_t M = goc_data_full & 0x1FFFF;
        if(option) {
            mrr_freq_hopping = N;
        }
        if(option2) {
            RADIO_PACKET_DELAY = M;
        }
    
        radio_data_arr[2] = (0x12 << 8) | CHIP_ID;
        radio_data_arr[1] = mrr_freq_hopping;
        radio_data_arr[0] = RADIO_PACKET_DELAY;
        send_beacon();
    }
    else if(goc_data_header == 0x13) {
        uint16_t option2 = (goc_data_full >> 22) & 0x1;
        uint16_t N = (goc_data_full >> 17) & 0x1F;
        uint16_t M = goc_data_full & 0xFFFF;
        if(option) {
            THRESHOLD_IDX_SHIFT = N;
        }
        if(option2) {
            EDGE_THRESHOLD = M;
        }
    
        radio_data_arr[2] = (0x13 << 8) | CHIP_ID;
        radio_data_arr[1] = THRESHOLD_IDX_SHIFT;
        radio_data_arr[0] = EDGE_THRESHOLD;
        send_beacon();
    }
    else if(goc_data_header == 0x14) {
        uint16_t option = (goc_data_full >> 20) & 0xF;
        uint32_t N = goc_data_full & 0xFFFFF;

        if(option == 1) {
            PMU_WAKEUP_INTERVAL = N;
        }
        else if(option == 2) {
            RADIO_SEQUENCE_PERIOD = N;
        }
        else if(option == 3) {
            RADIO_DUTY_DURATION = N;
        }
        else if(option == 4) {
            RADIO_INITIAL_BEACON_TIME = N;
        }
        else if(option == 5) {
            RADIO_BEACON_PERIOD = N;
        }
        else if(option == 6) {
            RADIO_INITIAL_DATA_DAY = N;
        }
        else if(option == 7) {
            RADIO_INITIAL_DATA_TIME = N;
        }
        else if(option == 8) {
            CHECK_PMU_NUM_UNITS = N;
        }
        else if(option == 9) {
            RADIO_REST_NUM_UNITS = N;
        }
        else if(option == 10) {
            RADIO_REST_TIME = N;
        }
        else if(option == 11) {
            RADIO_DATA_PERIOD = N;
        }

        radio_data_arr[2] = (0x14 << 8) | CHIP_ID;
        radio_data_arr[1] = (PMU_WAKEUP_INTERVAL << 16) | (RADIO_SEQUENCE_PERIOD << 8) | RADIO_DUTY_DURATION; 
        radio_data_arr[0] = RADIO_INITIAL_BEACON_TIME;
        send_beacon();

        radio_data_arr[2] = (0x14 << 8) | (0x1 << 6) | CHIP_ID;
        radio_data_arr[1] = RADIO_BEACON_PERIOD;
        radio_data_arr[0] = (RADIO_INITIAL_DATA_DAY << 24) | RADIO_INITIAL_DATA_TIME;
        send_beacon();

        radio_data_arr[2] = (0x14 << 8) | (0x2 << 6) | CHIP_ID;
        radio_data_arr[1] = (CHECK_PMU_NUM_UNITS << 20) | RADIO_REST_TIME;
        radio_data_arr[0] = (RADIO_REST_NUM_UNITS << 20) | RADIO_DATA_PERIOD;
        send_beacon();
    }
    else if(goc_data_header == 0x15) {
        uint8_t option2 = (goc_data_full >> 22) & 0x1;
        if(option) {
            if(option2) {
                radio_debug = 1;
                uint16_t N = goc_data_full & 0xFFFF;
                RADIO_DEBUG_PERIOD = N * 60;
            }
            else {
                radio_debug = 0;
            }
        }

        radio_data_arr[2] = (0x15 << 8) | CHIP_ID;
        radio_data_arr[1] = radio_debug;
        radio_data_arr[0] = RADIO_DEBUG_PERIOD;
        send_beacon();
    }
    else if(goc_data_header == 0x16) {
        uint32_t N = goc_data_full & 0x3FFFFF;
        uint8_t option2 = (goc_data_full >> 22) & 0x3;
        if(option2 == 1) {
            MRR_TEMP_THRESH_LOW = N;
        }
        else if(option2 == 2) {
            MRR_TEMP_THRESH_HIGH = N;
        }
        else if(option2 == 3) {
            OVERRIDE_RAD = N;
        }

        radio_data_arr[2] = (0x16 << 8) | CHIP_ID;
        radio_data_arr[1] = (OVERRIDE_RAD << 16) | MRR_TEMP_THRESH_LOW;
        radio_data_arr[0] = MRR_TEMP_THRESH_HIGH;
        send_beacon();
    }
    else if(goc_data_header == 0x17) {
        // read error code

        radio_data_arr[2] = (0x17 << 8) | CHIP_ID;
        radio_data_arr[1] = error_code;
        radio_data_arr[0] = error_time;
        send_beacon();
    }
    else if(goc_data_header == 0x18) {
        uint8_t len = (goc_data_full >> 16) & 0xFF;
        uint32_t* N = (uint32_t*) (goc_data_full & 0xFFFF);

        uint8_t i;
        for(i = 0; i < len; i++) {
            radio_data_arr[2] = (0x18 << 8) | CHIP_ID;
            radio_data_arr[1] = (uint32_t) N + i;
            radio_data_arr[0] = N[i];
            send_beacon();
        }
    }
    else if(goc_data_header == 0x19) {
        uint8_t option = (goc_data_full >> 22) & 0x3;
        uint8_t index = (goc_data_full >> 19) & 0x7;
        uint32_t N = goc_data_full & 0xFF;

        if(option == 1) {
            LOW_PWR_LOW_ADC_THRESH[index] = N;
        }
        else if(option == 2) {
            LOW_PWR_HIGH_ADC_THRESH[index] = N;
        }

        radio_data_arr[2] = (0x19 << 8) | CHIP_ID;
        radio_data_arr[1] = index;
        radio_data_arr[0] = (LOW_PWR_LOW_ADC_THRESH[index] << 8) | LOW_PWR_HIGH_ADC_THRESH[index];
        send_beacon();
    }
    else if(goc_data_header == 0x1A) {
        if(option) {
            mrr_trx_cap_antn_tune_coarse = goc_data_full & 0x3FF;
            // TX Setup Carrier Freq
            mrrv7_r00.MRR_TRX_CAP_ANTP_TUNE_COARSE = mrr_trx_cap_antn_tune_coarse;  //ANT CAP 10b unary 830.5 MHz // TODO: make them the same variable
            mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
            mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_COARSE = mrr_trx_cap_antn_tune_coarse; //ANT CAP 10b unary 830.5 MHz
            mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
            mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);
        }

        radio_data_arr[2] = (0x1A << 8) | CHIP_ID;
        radio_data_arr[1] = 0;
        radio_data_arr[0] = mrr_trx_cap_antn_tune_coarse;
        send_beacon();
    }
else if(goc_data_header == 0x1B) {
        uint8_t option = (goc_data_full >> 22) & 0x3;
        uint32_t N = goc_data_full & 0x3FFFF;

        if(option == 1) {
            start_day_count = N;
        }
        else if(option == 2) {
            end_day_count = N;
        }
        else if(option == 3) {
            radio_day_count = N;
        }

        radio_data_arr[2] = (0x1B << 8) | CHIP_ID;
        radio_data_arr[1] = start_day_count;
        radio_data_arr[0] = (end_day_count << 16) | radio_day_count;
        send_beacon();
    }
    else if(goc_data_header == 0x1C) {
        uint16_t N1 = (goc_data_full >> 12) & 0xFFF;
        uint16_t N2 = goc_data_full & 0xFFF;
        if(!N2) {
            radio_data_arr[2] = (0x1B << 8) | CHIP_ID;
            radio_data_arr[1] = 0;
            radio_data_arr[0] = max_unit_count;
            send_beacon();
        }
        else {
            // FIXME: restart packet number
            uint16_t i;
            for(i = 0; i < N2; i++) {
                if(N1 + i >= max_unit_count) {
                    break;
                }
                radio_unit(N1 + i);
            }
        }
    }
    else if(goc_data_header == 0x1D) {

    }

    update_system_time();

    if(projected_end_time_in_sec > xo_sys_time_in_sec) {
        set_lnt_timer();
    }
    else {
        set_system_error(0x01);
    }

    last_xo_sys_time = xo_sys_time;

    pmu_setting_temp_based(2);
    operation_sleep();

    while(1);
}
