/******************************************************************************************
 * Author:      Roger Hsiao, Gordy Carichner
 * Description: Monarch butterfly challenge code
 * 		This is the base code that all will share after version 5.1
 *                                          - PREv20E / PMUv11 / SNTv4 / FLPv3S / MRRv10 / MEMv1
 ******************************************************************************************
 * Current version: 5.2.1
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
 *
 ******************************************************************************************/

#define VERSION_NUM 0x521

#include "huffman_encodings.h"
#include "../include/PREv20E.h"
#include "../include/PREv20E_RF.h"
#include "../include/SNTv4_RF.h"
#include "../include/PMUv9_RF.h"
#include "../include/LNTv1A_RF.h"
#include "../include/MRRv7_RF.h"
#include "../include/mbus.h"
#include "../include/MBC_params_0.h"
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
#define OVERRIDE_RAD

#define MBUS_DELAY 100  // Amount of delay between seccessive messages; 100: 6-7ms
#define TIMER32_VAL 0xA0000  // 0x20000 about 1 sec with Y5 run default clock (PRCv17)


// SNT states
#define SNT_IDLE        0x0
#define SNT_TEMP_LDO    0x1
#define SNT_TEMP_START  0x2
#define SNT_TEMP_READ   0x3
#define SNT_SET_PMU	0x4

#define RADIO_PACKET_DELAY 20000 // Amount of delay between radio packets
#define RADIO_DATA_LENGTH 192

// operation list
#define STORE_SNT   0x0
#define STORE_LNT   0x1
#define SEND_RAD    0x2

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
volatile uint8_t wfi_timeout_flag = 0;

volatile uint8_t radio_after_done;
// volatile uint8_t goc_component;
// volatile uint8_t goc_func_id;
volatile uint8_t goc_state;
// volatile uint16_t goc_data;
volatile uint32_t goc_data_full;

volatile uint32_t xo_sys_time = 0;
volatile uint32_t xo_sys_time_in_sec = 0;
volatile uint32_t xo_day_time_in_sec = 0;
volatile uint32_t store_temp_timestamp = 0;

// #define SNT_TEMP_UPDATE_TIME 680   // 450 seconds
volatile uint32_t snt_sys_temp_code = 0x3E9;	// default 20C
volatile uint8_t snt_state = SNT_IDLE;

volatile uint8_t lnt_counter_base = 0;	// lnt measure time is LNT_MEAS_TIME << lnt_counter_base
// volatile uint8_t lnt_meas_time_mode;

volatile uint64_t lnt_sys_light = 0;

#define MRR_SIGNAL_PERIOD   600     // 10 minutes
#define MRR_TEMP_THRESH     0x258	
uint8_t mrr_send_enable = 1;

#define PMU_SETTINGS_LEN 6
const uint32_t PMU_ACTIVE_SETTINGS[7] = {0x0D0A0F0F, 
					 0x0D050D08, 
					 0x0D050D08, 
					 0x08040F04, 
					 0x08040F04, 
					 0x04040805, 
					 0x01080F0A};

// first 2 values are normal active settings for safety but will not be set
const uint32_t PMU_RADIO_SETTINGS[7] = {0x0D0A0F0F,
					0x0D050D08,	
					0x0F080F0A,
					0x0D020F0F,
					0x0D030808,
					0x06060809,
					0x01011001};

const uint32_t PMU_SLEEP_SETTINGS[7] = {0x0F040306,
					0x0F020303,
					0x0F020104,
					0x0F010102,
					0x07010102,
					0x02010104,
					0x01010103};

const uint8_t PMU_ACTIVE_SAR_SETTINGS[7] = {58, 55, 52, 50, 51, 51, 51};
const uint8_t PMU_RADIO_SAR_SETTINGS[7] = {58, 55, 57, 52, 52, 53, 53};
const uint8_t PMU_SLEEP_SAR_SETTINGS[7] = {50, 50, 50, 50, 50, 50, 51};

volatile uint16_t read_data_batadc;

volatile uint8_t radio_ready;
volatile uint8_t radio_on;
volatile uint8_t radio_counter;
volatile uint8_t radio_beacon_counter;
volatile uint8_t mrr_freq_hopping;
volatile uint8_t mrr_freq_hopping_step;
volatile uint16_t mrr_cfo_val_fine_min;
volatile uint32_t radio_data_arr[3];
volatile uint32_t radio_packet_count;

// sampling variables
#define DAWN 0
#define NOON 1
#define DUSK 2
#define NIGHT 3

#define MAX_EDGE_SHIFT 600
#define MAX_DAY_TIME 86400
#define MID_DAY_TIME 43200
#define IDX_MAX 239         // x = 179, y = 59, x + y + 1
#define EDGE_MARGIN1 10770 // x * 60 + 30 = 10770
#define EDGE_MARGIN2 3570 // y * 60 + 30 = 3570

#define EDGE_THRESHOLD 200 // = log2(2 lux * 1577) * 32
#define THRESHOLD_IDX_SHIFT 8 // = 4 + 4

#define XO_1_MIN 60
#define XO_2_MIN 120
#define XO_6_MIN 360
#define XO_8_MIN 480
#define XO_10_MIN 600
#define XO_16_MIN 960
#define XO_20_MIN 1200
#define XO_30_MIN 1800
#define XO_32_MIN 1920
#define XO_240_MIN 14400
#define XO_270_MIN 19600 // 4 hours and 30 minutes

volatile uint32_t next_light_meas_time = 0;
volatile uint32_t next_radio_debug_time = 0;

volatile uint8_t day_state = NIGHT;
volatile uint8_t max_day_count = 0;
volatile uint8_t day_count = 0;
volatile uint8_t store_temp_index = 0;    // resets every day
volatile bool radio_debug = false;
volatile uint8_t rot_idx = 0;
volatile uint16_t running_avg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
volatile uint32_t running_avg_time[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#define MAX_UINT16 0xFFFF
uint16_t sum = MAX_UINT16;
uint16_t avg_light = 0;

volatile uint32_t cur_sunrise = 0, cur_sunset = 0;
volatile uint32_t next_sunrise = 0, next_sunset = 0, cur_edge = 0;
volatile uint32_t threshold_idx_time = 0;
volatile uint32_t min_light_time = 0;
volatile uint32_t day_state_start_time, day_state_end_time;

#define IDX_INIT 0xFF
volatile uint8_t max_idx = 0;
const uint8_t intervals[4] = {1, 2, 8, 32};
const uint16_t resample_indices[4] = {32, 40, 44, 1000};
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

/**********************************************
 * Forward Declarations
 **********************************************/

static void sys_err(uint32_t);

void send_beacon();

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
    prev20e_r19.XO_PULSE_SEL     = 0x4; // pulse with sel, 1-hot encoded
    prev20e_r19.XO_DELAY_EN      = 0x3; // pair usage together with xo_pulse_sel
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

uint32_t get_timer_cnt_xo() {
    // FIXME: multiply by some constant
    return ((*REG_XOT_VAL_U & 0xFFFF) << 16) | (*REG_XOT_VAL_L & 0xFFFF);
}

#define XO_MAX_DAY_TIME_IN_SEC 86400
#define XO_TO_SEC_SHIFT 15

void update_system_time() {
    // mbus_write_message32(0xC2, xo_sys_time);
    // mbus_write_message32(0xC1, xo_sys_time_in_sec);
    // mbus_write_message32(0xC0, xo_day_time_in_sec);

    uint32_t temp = xo_sys_time;
    xo_sys_time = get_timer_cnt_xo();

    // calculate the difference in seconds
    temp = (xo_sys_time >> XO_TO_SEC_SHIFT) - (temp >> XO_TO_SEC_SHIFT); // Have to shift individually to account for underflow

    if(xo_sys_time < temp) {
    	// xo timer overflowed, want to make sure that 0x00000 - 0x1FFFF = 1
    	temp += 0x20000;
    }

    xo_sys_time_in_sec += temp;
    xo_day_time_in_sec += temp;

    if(xo_day_time_in_sec >= XO_MAX_DAY_TIME_IN_SEC) {
        xo_day_time_in_sec -= XO_MAX_DAY_TIME_IN_SEC;
        day_count++;
    }


    // mbus_write_message32(0xC2, xo_sys_time);
    // mbus_write_message32(0xC1, xo_sys_time_in_sec);
    // mbus_write_message32(0xC0, xo_day_time_in_sec);
}

// FIXME: determine we still need to check day time when radioing out
bool xo_check_is_day() {
    update_system_time();
    return true;
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
void right_shift_arr(uint32_t* arr, uint32_t data, uint8_t arr_len, int16_t shift_len) {
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

    uint8_t start, back;

    if(sign == 1) {
        start = arr_len - 1;
        back = 0;
    }
    else {
        start = 0;
        back = arr_len - 1;
    }

    uint8_t i;

    // shift word be word
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
    uint8_t i;
    for(i = 0; i < LONG_INT_LEN; i++) {
        dest[i] = src[i];
    }
}

static void long_int_mult(const long_int lhs, const uint16_t rhs, long_int res) {
    uint32_t carry_in = 0;
    uint32_t temp_res[LONG_INT_LEN];
    uint8_t i;
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

#define UNIT_HEADER_SIZE 26 // 17 starting timestamp, 2 bit day_state, 7 bit starting idx
bool has_header = false;

// requires code_cache to have enough space
void store_code(int32_t code, uint8_t len) {
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
        code_cache_remainder = CODE_CACHE_MAX_REMAINDER;

    }
}

void store_diff_to_code_cache(int16_t diff, uint8_t start_idx, uint32_t starting_time) {
    uint8_t code_idx = 0;
    uint8_t len_needed = 0;
    uint16_t code = 0;
    if(diff < 32 && diff >= -32) {
        code_idx = diff + 32;
        len_needed = code_lengths[code_idx];
        code = diff_codes[code_idx];
    }
    else if(diff < 256 && diff >= -256) {
        code_idx = 64;
        len_needed = code_lengths[code_idx] + 9;
        code = diff_codes[code_idx];
    }
    else {
        code_idx = 65;
        len_needed = code_lengths[code_idx] + 11;
        code = diff_codes[code_idx];
    }

    if(!has_header) {
        len_needed += UNIT_HEADER_SIZE;
    }

    if(code_cache_remainder < len_needed) {
        flush_code_cache();
        has_header = false;
    }

    // 17 starting timestamp, 2 bit day_state, 7 bit starting idx
    if(!has_header) {
        // use 4 bits less of resolution and add that to msb
        store_code((starting_time >> 4) & 0x1FFFF, 17);
        store_code(day_state, 2);
        store_code(start_idx, 7);
        has_header = true;
    }

    store_code(code, code_lengths[code_idx]);

    if(code_idx == 64) {
        store_code(diff, 9);
    }
    else if(code_idx == 65) {
        store_code(diff, 11);
    }
}

void store_day_state_stop() {
    uint8_t len_needed = code_lengths[66];
    if(code_cache_remainder < len_needed) {
        flush_code_cache();
    }
    else {
        store_code(diff_codes[66], len_needed);
    }
}

/**********************************************
 * Sampling functions
 **********************************************/

void write_to_proc_cache(uint16_t data) {
    const uint8_t len = 11;
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
        return target > 0 && target < MID_DAY_TIME && (cur_sunset - target > XO_270_MIN);
    }
    else {
        return target > MID_DAY_TIME && target < MAX_DAY_TIME && (target - cur_sunset > XO_270_MIN);
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
    else if(day_state == NOON) {
        // FIXME: Using xo_day_time_in_sec will make everything not align to the minute
        cur_edge = xo_day_time_in_sec + XO_32_MIN;
        // cur_edge = xo_day_time_in_sec + XO_16_MIN;  // FIXME: debug
        day_state_start_time = cur_edge;
        day_state_end_time = cur_sunset - EDGE_MARGIN1 - XO_10_MIN;
    }
    else {
        cur_edge = xo_day_time_in_sec + XO_32_MIN;
        day_state_start_time = cur_edge;
        day_state_end_time = cur_sunrise - EDGE_MARGIN2 - XO_10_MIN;
    }

    // set new light measure time
    next_light_meas_time = day_state_start_time + xo_sys_time_in_sec - xo_day_time_in_sec;
}

void sample_light() {
    uint16_t log_light = log2(lnt_sys_light);
    uint16_t last_avg_light = avg_light;
    write_to_proc_cache(log_light);

    // store to running sum
    if(sum == MAX_UINT16) {
        // if uninitialized, initialize running avg
        uint8_t i;
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

    uint32_t target = 0;

    // record min_light in day_state
    if(min_light_idx == IDX_INIT || avg_light < min_light
            || (day_state == DAWN && avg_light == min_light)) { // find last min_light at dawn
        min_light = avg_light;
        min_light_idx = max_idx;
        min_light_time = next_light_meas_time;
    }

    // test if crosses threshold
    if(day_state == DAWN && threshold_idx == IDX_INIT
            && avg_light >= EDGE_THRESHOLD && last_avg_light < EDGE_THRESHOLD) {
        if(max_idx >= THRESHOLD_IDX_SHIFT) {
            threshold_idx = max_idx - THRESHOLD_IDX_SHIFT;
            threshold_idx_time = next_light_meas_time - THRESHOLD_IDX_SHIFT * 60;
        }
        else {
            threshold_idx = 0;
            threshold_idx_time = next_light_meas_time - max_idx * 60;
        }

        target = running_avg_time[(rot_idx + 3) & 7];
    }
    else if(day_state == DUSK && threshold_idx == IDX_INIT
            && avg_light <= EDGE_THRESHOLD && last_avg_light > EDGE_THRESHOLD) {
        if(max_idx + THRESHOLD_IDX_SHIFT <= IDX_MAX) {
            threshold_idx = max_idx + THRESHOLD_IDX_SHIFT;
            threshold_idx_time = next_light_meas_time + THRESHOLD_IDX_SHIFT * 60;
        }
        else {
            threshold_idx = IDX_MAX;
            threshold_idx_time = next_light_meas_time + (IDX_MAX - max_idx) * 60;
        }

        target = running_avg_time[(rot_idx + 3) & 7];
    }

    // set new edge time
    if(target) {
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
        if(day_state != NIGHT) {
            // resample and stoie
            uint16_t starting_idx = 0;
            int16_t start, end, sign;
            uint32_t sample_time;

            if(day_state == DUSK) {
                // from last to first
                // manual set remainder to current data in proc_cache to start reading
                proc_cache_remainder = PROC_CACHE_MAX_REMAINDER - proc_cache_remainder;
                // if crossed threshold
                if(threshold_idx != IDX_INIT) {
                    starting_idx = threshold_idx;
                    sample_time = threshold_idx_time;
                }
                else {
                    starting_idx = min_light_idx;
                    sample_time = min_light_time;
                }
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
                    sample_time = cur_edge + xo_sys_time_in_sec - xo_day_time_in_sec;
                }
                else if(threshold_idx != IDX_INIT) {
                    starting_idx = threshold_idx;
                    sample_time = threshold_idx_time;
                }
                else {
                    starting_idx = min_light_idx;
                    sample_time = min_light_time;
                }
                // first to last
                start = 0;
                end = max_idx + 1;
                sign = 1;
            }

            int16_t i;
            uint16_t last_log_light = 0;
            uint8_t next_sample_idx = starting_idx;
            uint8_t sample_idx = 0;
            uint8_t interval_idx = 0;
            for(i = start; i != end; i += sign) {
                uint16_t log_light = read_next_from_proc_cache();
                if(i == next_sample_idx) {
                    // store diff
                    uint16_t diff = log_light - last_log_light;
                    store_diff_to_code_cache(diff, sample_idx, sample_time);

                    sample_idx++;
                    last_log_light = log_light;
                    if(day_state == NOON) {
                        next_sample_idx++;
                        sample_time += XO_32_MIN;
                    }
                    else {
                        if(sample_idx >= resample_indices[interval_idx]) {
                            interval_idx++;
                        }

                        next_sample_idx += (intervals[interval_idx] * sign);
                        sample_time += (intervals[interval_idx] * sign * 60);
                    }
                }
            }
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
#define TEMP_CACHE_LEN 3
#define TEMP_CACHE_MAX_REMAINDER 64 // 96 - 16 crc - 1 bit light or temp - 5 bit chip id - 4 bit day count - 6 bit timestamp in half hour 
#define TEMP_RES 7

volatile uint32_t temp_cache[TEMP_CACHE_LEN];
volatile uint8_t temp_cache_remainder = TEMP_CACHE_MAX_REMAINDER;

void flush_temp_cache() {
    if(temp_cache_remainder < TEMP_CACHE_MAX_REMAINDER) {
        // left shift 0s into unused space
        right_shift_arr(temp_cache, 0, TEMP_CACHE_LEN, -temp_cache_remainder);
        // add header
        // Use the space for CRC to mark temp packet
        temp_cache[0] = (CHIP_ID << 10) 
                        | ((day_count & 0xF) << 5) 
                        | store_temp_index;

        mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) code_addr, temp_cache, TEMP_CACHE_LEN - 1);

        code_addr += TEMP_CACHE_LEN << 2;
        temp_cache_remainder = TEMP_CACHE_MAX_REMAINDER;
    }
}

void sample_temp() {
    if(temp_cache_remainder < TEMP_RES) {
        flush_temp_cache();
    }

    // Take 3 bits above decimal point and 4 bits under
    uint8_t log_temp = log2(snt_sys_temp_code) >> (8 - TEMP_RES); 

    // left shift into temp cache
    right_shift_arr(temp_cache, log_temp, TEMP_CACHE_LEN, -TEMP_RES);
    temp_cache_remainder -= TEMP_RES;

    // increment temp index here to make sure we store the correct index
    store_temp_index++;

    if(store_temp_index > 47) {
        store_temp_index = 0;   // resets when taking the midnight data
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
#define MPLIER_SHIFT 6
uint8_t xo_lnt_mplier = 0x73;
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

    uint32_t projected_end_time = projected_end_time_in_sec << XO_TO_SEC_SHIFT;

    update_system_time();
    uint64_t temp = mult(projected_end_time - xo_sys_time, xo_lnt_mplier);

    lnt_meas_time = temp >> (MPLIER_SHIFT + XO_TO_SEC_SHIFT - 2); // the -2 is empirical
    // uint32_t val = (end_time - xo_sys_time_in_sec) * 4;
    lntv1a_r03.TIME_COUNTING = lnt_meas_time;
    mbus_remote_register_write(LNT_ADDR, 0x03, lntv1a_r03.as_int);
    lnt_start();

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
    else if(temp <= XO_10_MIN) {
        // set 8 minute buffer time to make sure wake up time is power of 2
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
    uint8_t i;

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

static void pmu_set_adc_period(uint32_t val) {
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
}

static void pmu_set_active_clk(uint32_t setting) {
#ifdef USE_PMU
    uint8_t r = (setting >> 24) & 0xFF;
    uint8_t l = (setting >> 16) & 0xFF;
    uint8_t base = (setting >> 8) & 0xFF;
    uint8_t l_1p2 = setting & 0xFF;

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
    uint8_t r = (setting >> 24) & 0xFF;
    uint8_t l = (setting >> 16) & 0xFF;
    uint8_t base = (setting >> 8) & 0xFF;
    uint8_t l_1p2 = setting & 0xFF;

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

static void pmu_set_sar_conversion_ratio(uint8_t ratio) {
#ifdef USE_PMU
    uint8_t i;
    for(i = 0; i < 2; i++) {
	    pmu_reg_write(0x05,         // PMU_EN_SAR_RATIO_OVERRIDE; default: 12'h000
		    ((i << 13) |    // enable override setting [12] (1'b1)
		     (0 << 12) |    // let vdd_clk always connect to vbat
		     (i << 11) |    // enable override setting [10] (1'h0)
		     (0 << 10) |    // have the converter have the periodic reset (1'h0)
		     (i <<  9) |    // enable override setting [8:0] (1'h0)
		     (0 <<  8) |    // switch input / output power rails for upconversion (1'h0)
		     (i <<  7) |    // enable override setting [6:0] (1'h0)
		     (ratio)));  // binary converter's conversion ratio (7'h00)
    }
#endif
}

// 0 : normal active
// 1 : radio active
// 2 : sleep
static void pmu_setting_temp_based(uint8_t mode) {
#ifdef USE_PMU
    int8_t i;
    for(i = PMU_SETTINGS_LEN; i >= 0; i--) {
	    // mbus_write_message32(0xB3, 0xFFFFFFFF);
        if(i == 0 || snt_sys_temp_code >= PMU_TEMP_THRESH[i - 1]) {
            if(mode == 0) {
	        pmu_set_active_clk(PMU_ACTIVE_SETTINGS[i]);
                pmu_set_sar_conversion_ratio(PMU_ACTIVE_SAR_SETTINGS[i]);
            }
            else if(mode == 2) {
                pmu_set_sleep_clk(PMU_SLEEP_SETTINGS[i]);
                pmu_set_sar_conversion_ratio(PMU_SLEEP_SAR_SETTINGS[i]);
            }
            else {
	        pmu_set_active_clk(PMU_RADIO_SETTINGS[i]);
                pmu_set_sar_conversion_ratio(PMU_RADIO_SAR_SETTINGS[i]);
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
    pmu_set_sar_conversion_ratio(PMU_SLEEP_SAR_SETTINGS[2]);
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
    read_data_batadc = *REG0 & 0xFF;	// TODO: check if only need low 8 bits
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
    pmu_reg_write(0x3A,         // PMU_EN_CONTROLLER_STALL_ACTIVE
                ((1 << 20) |    // ignore state_horizen; default: 1
                 (0 << 19) |    // state_vbat_read
                 (1 << 13) |    // ignore state_adc_output_ready; default: 0
                 (1 << 12) |    // ignore state_adc_output_ready; default:0
                 (1 << 11)));   // ignore state_adc_output_ready; default:0

    pmu_adc_reset_setting();
    pmu_adc_enable();
#endif
}

/**********************************************
 * MRR functions (MRRv7)
 **********************************************/

static void reset_radio_data_arr() {
    uint8_t i;
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

uint8_t mrr_cfo_val_fine = 0;

static void mrr_send_radio_data(uint8_t last_packet) {
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
    uint8_t count = 0;
    uint8_t num_packets = 1;
    if (mrr_freq_hopping) num_packets = mrr_freq_hopping;

    // mrr_cfo_val_fine = 0x0000;

    while (count < num_packets){
	// may be able to remove 2 lines below, GC 1/6/20
	*TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
	*REG_MBUS_WD = 0; // Disables Mbus watchdog timer

	mrrv7_r01.MRR_TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
	mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine;
	mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);
	send_radio_data_mrr_sub1();
	count++;
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
    }else{
	mrrv7_r11.MRR_RAD_FSM_EN = 0;
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
    }
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
    // mrr_freq_hopping = 3; // FIXME: debug
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
    mrrv7_r00.MRR_TRX_CAP_ANTP_TUNE_COARSE = 0xFF;  //ANT CAP 10b unary 830.5 MHz // FIXME: adjust per stack	// TODO: make them the same variable
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
    has_header = false;

    temp_cache_remainder = TEMP_CACHE_MAX_REMAINDER;

    projected_end_time_in_sec = 0;
    next_radio_debug_time = 0;
}

static void operation_init( void ) {
    // BREAKPOINT 0x01
    // mbus_write_message32(0xBA, 0x01);

    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    config_timer32(0, 0, 0, 0);

    // Enumeration
    enumerated = ENUMID;

    // timing variables don't belong to var_init
    xo_sys_time = 0;
    xo_sys_time_in_sec = 0;
    xo_day_time_in_sec = 0;

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

    mbus_write_message32(0xA7, get_timer_cnt_xo());
    mbus_write_message32(0xA7, get_timer_cnt_xo());
    mbus_write_message32(0xA7, get_timer_cnt_xo());
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

// static void operation_sleep_notimer( void ) {
//     // Diable timer
//     lnt_stop();
//     set_wakeup_timer(0, 0, 0);
//     // set_xo_timer(0, 0, 0, 0);
//     config_timer32(0, 0, 0, 0);
//     // TODO: reset SNT timer
//     operation_sleep();
// }

static void sys_err(uint32_t code)
{

    if(radio_debug) {
        radio_data_arr[0] = code;
        radio_data_arr[1] = 0xFFFF0000;
        radio_data_arr[2] = 0xFFFFFFFF;
        send_beacon();
    }

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

void radio_full_data() {
    uint16_t addr = 0;
    pmu_setting_temp_based(1);
    uint8_t packet_num = 0;

    while(addr < code_addr) {
        // read out data and check if it's temp or light
        set_halt_until_mbus_trx();
        mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) addr, PRE_ADDR, code_cache, CODE_CACHE_LEN - 1);
        set_halt_until_mbus_tx();

        if(code_cache[0] & 0x80000000) {
            // is light, radio out four times
            uint8_t i;
            for(i = 0; i < 4; i++) {
                // first 96 bits are read out using temporary arr
                // This is needed because radio_data_arr is big endian
                uint32_t temp_arr[3];
                temp_arr[0] = code_cache[0];
                temp_arr[1] = code_cache[1];
                temp_arr[2] = code_cache[2];
                
                // clear out top 16 bits 
                temp_arr[0] &= 0x0000FFFF;

                // right shift to make space for packet header
                right_shift_arr(temp_arr, 0, 3, LIGHT_HEADER_SIZE);
                
                // insert header
                temp_arr[0] |= (1 << 15) | (CHIP_ID << 10) | (packet_num << 4);
                packet_num = (packet_num + 1) & 0x3F;

                // assign to radio arr
                radio_data_arr[2] = temp_arr[0];
                radio_data_arr[1] = temp_arr[1];
                radio_data_arr[0] = temp_arr[2];

                mrr_send_radio_data(0);

                // left shift a new packet over
                right_shift_arr(code_cache, 0, CODE_CACHE_LEN, -LIGHT_CONTENT_SIZE);
            }
            addr += CODE_CACHE_LEN << 2;
        }
        else {
            // is temp, radio out temp packet
            // no need to insert header because it is already set
            radio_data_arr[2] = code_cache[0];
            radio_data_arr[1] = code_cache[1];
            radio_data_arr[0] = code_cache[2];

            mrr_send_radio_data(0);

            addr += TEMP_CACHE_LEN << 2;
        }
    }
}

uint8_t set_send_enable() {
#ifdef OVERRIDE_RAD
    return 1;
#endif
#ifdef USE_GMB
    return 1;
#endif
#ifndef USE_GMB
    if(snt_sys_temp_code < PMU_TEMP_THRESH[1] || snt_sys_temp_code >= PMU_55C) {
    	return 0;
    }
    uint8_t i;
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
#endif
}

void send_beacon() {
    pmu_setting_temp_based(1);
    // mrr_send_radio_data(1);
    mrr_send_radio_data(0); // FIXME: change this line in real code
    // pmu_setting_temp_based(0);   // FIXME: set this line back
}

int main() {
    // Only enable relevant interrupts (PREv18)
    *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
		  1 << IRQ_REG0 | 1 << IRQ_REG1 | 1 << IRQ_REG2 | 1 << IRQ_REG3);

    if(enumerated != ENUMID) {
        operation_init();
	// set pmu sleep setting
	pmu_setting_temp_based(2);
	operation_sleep();
        // operation_sleep_notimer();
    }
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
    pmu_adc_read_latest();

    mrr_send_enable = set_send_enable();

    if(!(goc_data_full & 0x80000000)) {

        var_init();

        // characterization code
	uint8_t cmd = (goc_data_full >> 28) & 0x7;
        reset_radio_data_arr();
        if(cmd == 0) {
            // send alive beacon
	    pmu_setting_temp_based(1);
	    radio_data_arr[2] = CHIP_ID << 8;
	    radio_data_arr[1] = 0;
	    radio_data_arr[0] = (snt_sys_temp_code << 12) | read_data_batadc;
	    mrr_send_radio_data(1);
        }
        else if(cmd == 1) {
            // packet blaster: send packets for 10s then sleep for 10s
            update_system_time();
	    pmu_setting_temp_based(1);
            uint32_t start_time_in_sec = xo_sys_time_in_sec;
            do {
                // blast packets for 10s regardless of battery state
	        radio_data_arr[2] = CHIP_ID << 8;
	        radio_data_arr[1] = 0x1817;
	        radio_data_arr[0] = op_counter;
	    	mrr_send_radio_data(1);
                op_counter++;
                update_system_time();
            } while(xo_sys_time_in_sec - start_time_in_sec < 10);

            // set 10s timer
	    projected_end_time_in_sec = xo_sys_time_in_sec + 10;

        }
        else if(cmd == 2) {
            uint8_t use_pmu = (goc_data_full >> 27) & 0x1;
            uint16_t goc_radio_time = goc_data_full & 0xFFFF;

            // if !use_pmu, always radio out
            if(!goc_state) {
                goc_state = 1;
            }
            else if(goc_state == 1) {
		goc_state = 0;
                if(!use_pmu || mrr_send_enable) {
		    op_counter++;
                    pmu_setting_temp_based(1);
                    radio_data_arr[0] = lnt_sys_light & 0xFFFFFFFF;
                    radio_data_arr[1] = lnt_sys_light >> 32;
                    radio_data_arr[2] = (op_counter & 0xF) << 8 | (0x40);
                    mrr_send_radio_data(0);
                    radio_data_arr[0] = snt_sys_temp_code;
                    radio_data_arr[1] = read_data_batadc;
                    radio_data_arr[2] = (op_counter & 0xF) << 8 | (0x41);
                    mrr_send_radio_data(0);
		    reset_radio_data_arr();
		    update_system_time();
		    radio_data_arr[0] = xo_sys_time;
                    radio_data_arr[1] = lnt_meas_time;
                    radio_data_arr[2] = (op_counter & 0xF) << 8 | (0x42);
                    mrr_send_radio_data(1);

		    projected_end_time_in_sec = xo_sys_time_in_sec + goc_radio_time;
                }
            }
        }
        else if(cmd == 3) {
            // send radio and sleep
            pmu_setting_temp_based(1);
	    radio_data_arr[2] = CHIP_ID << 8;
	    radio_data_arr[1] = 0;
	    radio_data_arr[0] = (snt_sys_temp_code << 12) | read_data_batadc;
	    mrr_send_radio_data(1);
	    pmu_setting_temp_based(0);

	    // safe sleep mode // manually set_temp_to_some_value
	    pmu_set_sleep_low();
	    operation_sleep();
        }
        else if(cmd == 4) {
            // cymbet distance test
            // blast 10 packets, then sleep for a minute
            uint16_t goc_radio_time = goc_data_full & 0xFFFF;

            uint8_t i = 0;
            for(i = 0; i < 10; i++) {
	        radio_data_arr[2] = CHIP_ID << 8;
	        radio_data_arr[1] = 0;
	        radio_data_arr[0] = 0;
	        mrr_send_radio_data(i == 9);
            }

            projected_end_time_in_sec = xo_sys_time_in_sec + goc_radio_time;
        }
        else if(cmd == 5) {
            // battery drain test, all the measurements are performed when we wake up
            // set 8 minute timers
	    if(!goc_state) {
	    	goc_state = 1;
		projected_end_time_in_sec = xo_sys_time_in_sec + XO_8_MIN;
	    }
	    else {
		projected_end_time_in_sec += XO_8_MIN;
	    }
        }
        else if(cmd == 6) {
            while(1);
        }
        else if(cmd == 7) {
            // extended set of functions
            cmd = (goc_data_full >> 24) & 0xF;
            if(cmd == 0) {
                // set xo_day_time_in_sec in minutes
                xo_day_time_in_sec = (goc_data_full & 0x7FF) * 60;
            }
            else if(cmd == 1) {
                // set current sunrise and sunset times in minutes
                cur_sunset = (goc_data_full & 0x7FF) * 60;
                cur_sunrise = ((goc_data_full >> 11) & 0x7FF) * 60;
            }
            else if(cmd == 2) {
                // Testing initialization
                if(xo_day_time_in_sec < cur_sunrise - EDGE_MARGIN2 - XO_32_MIN) {
                    day_state = NIGHT;
                }
                else if(xo_day_time_in_sec < cur_sunset - EDGE_MARGIN1 - XO_32_MIN){
                    day_state = NOON;
                }
                else {
                    day_state = NIGHT;
                }

                // set new state variables based on day_state
                set_new_state();

                radio_data_arr[0] = cur_sunset;
                radio_data_arr[1] = cur_sunrise;
                radio_data_arr[2] = day_state;
                send_beacon();

                radio_data_arr[0] = next_sunrise;
                radio_data_arr[1] = next_sunset;
                radio_data_arr[2] = 0x100;
                send_beacon();

                radio_data_arr[0] = cur_edge;
                radio_data_arr[1] = next_sunset;
                radio_data_arr[2] = 0x101;
                send_beacon();

                radio_data_arr[0] = day_state_end_time;
                radio_data_arr[1] = day_state_start_time;
                radio_data_arr[2] = xo_day_time_in_sec;
                send_beacon();

                uint32_t store_temp_day_time = 0;
                store_temp_index = 0xFF; 
                while(store_temp_day_time < day_state_start_time) {
                    store_temp_day_time += XO_30_MIN;
                    store_temp_index++;
                    if(store_temp_index > 47) {
                        // Error state
                        break;
                    }
                }
                
                radio_data_arr[0] = store_temp_day_time;
                radio_data_arr[1] = store_temp_index;
                radio_data_arr[2] = 0;
                send_beacon();

            }
            else if(cmd == 3) {
                // send version number
                radio_data_arr[0] = xo_day_time_in_sec;
                radio_data_arr[1] = VERSION_NUM;
                radio_data_arr[2] = CHIP_ID << 8;
                send_beacon();
            }
            else if(cmd == 4) {
                // debug, remove later
                radio_data_arr[0] = code_addr;
                radio_data_arr[1] = 0;
                radio_data_arr[2] = CHIP_ID << 8;
                send_beacon();
            }
        }
	pmu_setting_temp_based(0);
    }
    else {
#define STATE_INIT 0
#define STATE_COLLECT 1
#define STATE_RADIO 2
#define STATE_START 3
#define STATE_WAIT 4
#define STATE_VERIFY 5
        if(goc_state == STATE_INIT) {
            // Need current day time in minutes 11 bits
            // duration in days 7 bits
            // cur_sunrise in minutes 11 bits
            // cur_sunset in minutes 11 bits
            // option to radio out every time

            // reset all variables so that the system has a clean start
            var_init();

            goc_state = STATE_VERIFY;

            radio_debug = (goc_data_full >> 7) & 0x1;  // 1 bit radio out option
            max_day_count = goc_data_full & 0x7F;    // 7 bits duration in days
            day_count = 0;

            // initialize day_state
            if(xo_day_time_in_sec < cur_sunrise - EDGE_MARGIN2 - XO_32_MIN) {
                day_state = NIGHT;
            }
            else if(xo_day_time_in_sec < cur_sunset - EDGE_MARGIN1 - XO_32_MIN){
                day_state = NOON;
            }
            else {
                day_state = NIGHT;
            }

            // set new state variables based on day_state
            set_new_state();

            // find suitable temp storage time based on day_state_start_time
            uint32_t store_temp_day_time = 0;

            // this variable should always reflect the last temp data stored
            store_temp_index = 0xFF; 
            while(store_temp_day_time < day_state_start_time) {
                store_temp_day_time += XO_30_MIN;
                store_temp_index++;
                // if(store_temp_index > 47) {
                //     // Error state
                //     break;
                // }
            }
            store_temp_timestamp = store_temp_day_time + xo_sys_time_in_sec - xo_day_time_in_sec;

            projected_end_time_in_sec = xo_sys_time_in_sec + XO_2_MIN;
            op_counter = 0;
            // op_counter = 2; // FIXME: delete this
        }
	else if(goc_state == STATE_VERIFY) {
	    if(++op_counter < 4) {
		projected_end_time_in_sec += XO_2_MIN;
	    }
	    else {
		goc_state = STATE_COLLECT;
                set_projected_end_time();
	    }

            if(op_counter == 1) {
                radio_data_arr[0] = xo_day_time_in_sec;
                radio_data_arr[1] = xo_sys_time_in_sec;
                radio_data_arr[2] = (CHIP_ID << 10) | op_counter;
            }
            else if(op_counter == 2) {
                radio_data_arr[0] = cur_sunset;
                radio_data_arr[1] = cur_sunrise;
                radio_data_arr[2] = (CHIP_ID << 10) | op_counter;
            }
            else if(op_counter == 3) {
                radio_data_arr[0] = max_day_count;
                radio_data_arr[1] = radio_debug;
                radio_data_arr[2] = (CHIP_ID << 10) | op_counter;
            }
            else if(op_counter == 4) {
                radio_data_arr[0] = store_temp_timestamp;
                radio_data_arr[1] = xo_sys_time_in_sec;
                radio_data_arr[2] = (CHIP_ID << 10) | op_counter;
            }
	    // radio_data_arr[2] = xo_lnt_mplier;
	    send_beacon();
	}
        else if(goc_state == STATE_COLLECT) {
            
            if(day_count >= max_day_count && xo_day_time_in_sec >= MID_DAY_TIME) {
                flush_temp_cache();       // store everything in temp_code_storage
                flush_code_cache();
                update_system_time();

                goc_state = STATE_RADIO;
                projected_end_time_in_sec += MRR_SIGNAL_PERIOD;
                // projected_end_time_in_sec += XO_2_MIN;  // FIXME: debug
                radio_beacon_counter = 0;
                radio_counter = 0;

                radio_data_arr[0] = temp_cache_remainder;
                radio_data_arr[1] = code_addr;
                radio_data_arr[2] = 0x1234;
	        send_beacon();

                radio_data_arr[0] = projected_end_time_in_sec;
                radio_data_arr[1] = xo_sys_time_in_sec;
                radio_data_arr[2] = 0xAAAA;
	        send_beacon();
            }
	    else {
                if(xo_sys_time_in_sec >= store_temp_timestamp) {
                    store_temp_timestamp += XO_30_MIN;    // increment by 30 minutes

                    sample_temp();

                }
                
                if(projected_end_time_in_sec == next_light_meas_time) {
                    sample_light();
                }

                set_projected_end_time();
	    }

            if(radio_debug && xo_sys_time_in_sec >= next_radio_debug_time) {
                // next_radio_debug_time += XO_240_MIN;
                next_radio_debug_time += 0; // FIXME: debug

                // radio_data_arr[0] = snt_sys_temp_code;
                // radio_data_arr[1] = xo_sys_time_in_sec;
                // radio_data_arr[2] = CHIP_ID << 8;
                // send_beacon();
                
                radio_data_arr[0] = projected_end_time_in_sec;
                radio_data_arr[1] = xo_sys_time_in_sec;
                radio_data_arr[2] = CHIP_ID << 8;
                send_beacon();
            }

        }
        else if(goc_state == STATE_RADIO) {
            if(xo_check_is_day() && mrr_send_enable) { // fix this for testing
                pmu_setting_temp_based(1);
                // send data
                if(++radio_beacon_counter >= 6) {
                // if(++radio_beacon_counter >= 0) {   // FIXME: debug
                    radio_beacon_counter = 0;

                    radio_full_data();
                }
                else {
                    // send beacon
                    // If bottom bits are all 1s, then it must be beacon
                    // radio_data_arr[2] = CHIP_ID << 8;
                    // radio_data_arr[1] = (read_data_batadc << 24) | snt_sys_temp_code;
                    // radio_data_arr[0] = (radio_beacon_counter << 28) | (radio_counter << 20) | (xo_day_time_in_sec << 11) | 0x7FF;

                    // FIXME: debug
                    radio_data_arr[2] = CHIP_ID << 8;
                    radio_data_arr[1] = 0xFFFFFFFF;
                    radio_data_arr[0] = code_addr;
                    mrr_send_radio_data(1);
                }
                pmu_setting_temp_based(0);


                radio_counter++;
            }

            projected_end_time_in_sec += MRR_SIGNAL_PERIOD;
            // projected_end_time_in_sec += XO_2_MIN;  // FIXME: debug
        }
    }
    // }

    update_system_time();

    if(projected_end_time_in_sec > xo_sys_time_in_sec) {
        set_lnt_timer();
    }
    else {
	// set this as a sentry to avoid updating multiplier
        projected_end_time_in_sec = 0;
    }

    pmu_setting_temp_based(2);
    operation_sleep();

    while(1);
}
