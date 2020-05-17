/******************************************************************************************
 * Author:      Roger Hsiao
 * Description: Monarch butterfly challenge code
 *                                          - PREv20 / PMUv11 / SNTv4 / FLPv3S / MRRv10 / MEMv1
 ******************************************************************************************
 * v1: draft version; not tested on chip
 *
 * v3: first deployment in mexico; not using FLP
 * v3.1: compact code
 *
 * v4: deployment code with periodic wakeup
 ******************************************************************************************/

#include "../include/PREv20.h"
#include "../include/PREv20_RF.h"
#include "../include/SNTv4_RF.h"
#include "../include/PMUv9_RF.h"
#include "../include/LNTv1A_RF.h"
#include "../include/MRRv7_RF.h"
#include "../include/mbus.h"

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
// #define USE_RAD

#define MBUS_DELAY 100  // Amount of delay between seccessive messages; 100: 6-7ms
#define TIMER32_VAL 0xA0000  // 0x20000 about 1 sec with Y5 run default clock (PRCv17)


// SNT states
#define SNT_IDLE        0x0
#define SNT_TEMP_LDO    0x1
#define SNT_TEMP_START  0x2
#define SNT_TEMP_READ   0x3
#define SNT_SET_PMU	0x4

#define LNT_MEAS_TIME 32	// FIXME: change this to 32
#define PMU_SETTING_TIME 10
#define SNT_TEMP_UPDATE_TIME 600

#define MEM_STORAGE_SIZE 8192

#define RADIO_PACKET_DELAY 13000  // Amount of delay between radio packets
#define RADIO_DATA_LENGTH 192

// operation list
#define RUN_SNT     0x0
#define START_LNT   0x1
#define SEND_RAD    0x2

// default register values
volatile prev20_r19_t prev20_r19 = PREv20_R19_DEFAULT;

volatile sntv4_r00_t sntv4_r00 = SNTv4_R00_DEFAULT;
volatile sntv4_r01_t sntv4_r01 = SNTv4_R01_DEFAULT;
volatile sntv4_r07_t sntv4_r07 = SNTv4_R07_DEFAULT;
volatile sntv4_r08_t sntv4_r08 = SNTv4_R08_DEFAULT;
volatile sntv4_r09_t sntv4_r09 = SNTv4_R09_DEFAULT;
volatile sntv4_r17_t sntv4_r17 = SNTv4_R17_DEFAULT;

volatile lntv1a_r00_t lntv1a_r00 = LNTv1A_R00_DEFAULT;
volatile lntv1a_r01_t lntv1a_r01 = LNTv1A_R01_DEFAULT;
volatile lntv1a_r02_t lntv1a_r02 = LNTv1A_R02_DEFAULT;
volatile lntv1a_r03_t lntv1a_r03 = LNTv1A_R03_DEFAULT;
volatile lntv1a_r04_t lntv1a_r04 = LNTv1A_R04_DEFAULT;
volatile lntv1a_r05_t lntv1a_r05 = LNTv1A_R05_DEFAULT;
volatile lntv1a_r06_t lntv1a_r06 = LNTv1A_R06_DEFAULT;
volatile lntv1a_r07_t lntv1a_r07 = LNTv1A_R07_DEFAULT;
volatile lntv1a_r17_t lntv1a_r17 = LNTv1A_R17_DEFAULT;
volatile lntv1a_r20_t lntv1a_r20 = LNTv1A_R20_DEFAULT;
volatile lntv1a_r21_t lntv1a_r21 = LNTv1A_R21_DEFAULT;
volatile lntv1a_r22_t lntv1a_r22 = LNTv1A_R22_DEFAULT;
volatile lntv1a_r40_t lntv1a_r40 = LNTv1A_R40_DEFAULT;

volatile mrrv7_r00_t mrrv7_r00 = MRRv7_R00_DEFAULT;
volatile mrrv7_r01_t mrrv7_r01 = MRRv7_R01_DEFAULT;
volatile mrrv7_r02_t mrrv7_r02 = MRRv7_R02_DEFAULT;
volatile mrrv7_r03_t mrrv7_r03 = MRRv7_R03_DEFAULT;
volatile mrrv7_r04_t mrrv7_r04 = MRRv7_R04_DEFAULT;
volatile mrrv7_r07_t mrrv7_r07 = MRRv7_R07_DEFAULT;
volatile mrrv7_r11_t mrrv7_r11 = MRRv7_R11_DEFAULT;
volatile mrrv7_r12_t mrrv7_r12 = MRRv7_R12_DEFAULT;
volatile mrrv7_r13_t mrrv7_r13 = MRRv7_R13_DEFAULT;
volatile mrrv7_r14_t mrrv7_r14 = MRRv7_R14_DEFAULT;
volatile mrrv7_r15_t mrrv7_r15 = MRRv7_R15_DEFAULT;
volatile mrrv7_r1F_t mrrv7_r1F = MRRv7_R1F_DEFAULT;

/**********************************************
 * Global variables
 **********************************************
 * "static" limits the variables to this file, giving the compiler more freedom
 * "volatile" should only be used for mmio to ensure memory storage
 */
volatile uint32_t enumerated;
volatile uint16_t op_counter;
volatile uint8_t wfi_timeout_flag;
volatile uint8_t sys_run_continuous;

uint8_t goc_component;
uint8_t goc_func_id;
uint8_t goc_state;
uint16_t goc_data;

#define OPERATION_DURATION 79200   // 22 hours
#define SNT_OP_MAX_COUNT 55   // FIXME: change this to 133 22 hours and ten minutes
#define XO_DAY_START 18000  // 5 am
#define XO_DAY_END  68400   // 7 pm
volatile uint8_t xo_is_day;
volatile uint8_t xo_last_is_day;
volatile uint32_t xo_sys_time_in_sec;
volatile uint32_t xo_day_time_in_sec;
// volatile uint32_t xo_end_time_in_sec;
volatile uint32_t xo_sys_time;

#define XOT_TIMER_LIST_LEN 3
volatile uint32_t xot_timer_list[3];
volatile uint32_t xot_last_timer_list[3];

volatile uint32_t snt_sys_temp_code;
volatile uint8_t snt_state;
volatile uint8_t snt_counter;

volatile uint8_t lnt_start_meas = 0;
volatile uint8_t lnt_counter_base = 0;	// lnt measure time is LNT_MEAS_TIME << lnt_counter_base
volatile uint8_t lnt_cur_level;
const uint16_t LNT_UPPER_THRESH[3] = {500, 1500, 5000};
const uint16_t LNT_LOWER_THRESH[3] = {300, 1200, 4000};
const uint16_t LNT_INTERVAL[4] = {60, 300, 600, 1800};
volatile uint64_t lnt_sys_light;
volatile uint64_t lnt_last_light;

#define MRR_SIGNAL_PERIOD   300
#define MRR_DATA_PERIOD     3600
#define MRR_TEMP_THRESH     5 		// FIXME: use code for this
#define MRR_VOLT_THRESH     0x4B 	// FIXME: use code for this

const int32_t PMU_TEMP_THRESH[7] = {-10, 0, 10, 20, 30, 40, 50, 60};
const uint32_t PMU_ACTIVE_SETTINGS[8] = {0x0D021004, 
					 0x0D021004, 
					 0x0D021004, 
					 0x05011002, 
					 0x05011002, 
					 0x02011002, 
					 0x02011002, 
					 0x02011002};
const uint32_t PMU_RADIO_SETTINGS[8] = {0x07021004,
					0x07021004,
					0x07021004,
					0x07021004,
					0x07021004,
					0x07021004,
					0x07021004,
					0x07021004};
const uint32_t PMU_SLEEP_SETTINGS[8] = {0x0F010102,
					0x0F010102,
					0x0F010102,
					0x0F010102,
					0x02010101,
					0x02000101,
					0x01010101,
					0x01010101};
volatile uint16_t read_data_batadc;
volatile uint16_t read_data_batadc_diff; // FIXME: fix this
volatile uint8_t pmu_sar_ratio_radio;

volatile uint8_t radio_ready;
volatile uint8_t radio_on;
volatile uint8_t radio_counter;
volatile uint8_t radio_beacon_counter;
volatile uint8_t mrr_freq_hopping;
volatile uint8_t mrr_freq_hopping_step;
volatile uint16_t mrr_cfo_val_fine_min;
volatile uint32_t radio_data_arr[3];
volatile uint32_t radio_packet_count;

volatile uint16_t mem_light_addr;
volatile uint16_t mem_light_len;
volatile uint16_t mem_temp_addr;
volatile uint16_t mem_temp_len;

/**********************************************
 * Forward Declarations
 **********************************************/

static void sys_err(uint32_t);

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

// write to XO driver 0x19
void xo_ctrl(uint8_t xo_pulse_sel,
	     uint8_t xo_delay_en,
	     uint8_t xo_drv_start_up,
	     uint8_t xo_drv_core,
	     uint8_t xo_rp_low,
	     uint8_t xo_rp_media,
	     uint8_t xo_rp_mvt,
	     uint8_t xo_rp_svt,
	     uint8_t xo_scn_clk_sel,
	     uint8_t xo_scn_enb) {
    *REG_XO_CONF1 = ((xo_pulse_sel     << 11) |
                     (xo_delay_en      << 8)  |
                     (xo_drv_start_up  << 7)  |
                     (xo_drv_core      << 6)  |
                     (xo_rp_low        << 5)  |
                     (xo_rp_media      << 4)  |
                     (xo_rp_mvt        << 3)  |
                     (xo_rp_svt        << 2)  |
                     (xo_scn_clk_sel   << 1)  |
                     (xo_scn_enb       << 0));
    mbus_write_message32(0xA1, *REG_XO_CONF1);
}

void xo_init( void ) {
    // Parasitic capacitance tuning (6 bits for each; each 1 adds 1.8pF)
    uint32_t xo_cap_drv = 0x3F;
    uint32_t xo_cap_in  = 0x3F;
    *REG_XO_CONF2 = ((xo_cap_drv << 6) | (xo_cap_in << 0));

    // XO xonfiguration
    prev20_r19.XO_PULSE_SEL     = 0x4; // pulse with sel, 1-hot encoded
    prev20_r19.XO_DELAY_EN      = 0x3; // pair usage together with xo_pulse_sel
    prev20_r19.XO_DRV_START_UP  = 0x0;
    prev20_r19.XO_DRV_CORE      = 0x0;
    prev20_r19.XO_SCN_CLK_SEL   = 0x0;
    prev20_r19.XO_SCN_ENB       = 0x1;

    // TODO: check if need 32.768kHz clock
    prev20_r19.XO_EN_DIV        = 0x1; // divider enable (also enables CLK_OUT)
    prev20_r19.XO_S             = 0x0; // (not used) division ration for 16kHz out
    prev20_r19.XO_SEL_CP_DIV    = 0x0; // 1: 0.3v-generation charge-pump uses divided clock
    prev20_r19.XO_EN_OUT        = 0x1; // xo output enabled;
    				       // Note: I think this means output to XOT
    // Pseudo-resistor selection
    prev20_r19.XO_RP_LOW        = 0x0;
    prev20_r19.XO_RP_MEDIA      = 0x1;
    prev20_r19.XO_RP_MVT        = 0x0;
    prev20_r19.XO_RP_SVT        = 0x0;

    prev20_r19.XO_SLEEP = 0x0;
    *REG_XO_CONF1 = prev20_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(10000); // >= 1ms

    prev20_r19.XO_ISOLATE = 0x0;
    *REG_XO_CONF1 = prev20_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(10000); // >= 1ms

    prev20_r19.XO_DRV_START_UP = 0x1;
    *REG_XO_CONF1 = prev20_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(30000); // >= 1s

    prev20_r19.XO_SCN_CLK_SEL = 0x1;
    *REG_XO_CONF1 = prev20_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(3000); // >= 300us

    prev20_r19.XO_SCN_CLK_SEL = 0x0;
    prev20_r19.XO_SCN_ENB     = 0x0;
    *REG_XO_CONF1 = prev20_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);
    delay(30000);  // >= 1s

    prev20_r19.XO_DRV_START_UP = 0x0;
    prev20_r19.XO_DRV_CORE     = 0x1;
    prev20_r19.XO_SCN_CLK_SEL  = 0x1;
    *REG_XO_CONF1 = prev20_r19.as_int;
    mbus_write_message32(0xA1, *REG_XO_CONF1);

    enable_xo_timer();
    reset_xo_cnt();
    start_xo_cnt();

    // BREAKPOint 0x03
    mbus_write_message32(0xBA, 0x03);

}

inline uint32_t get_timer_cnt() {
    return ((*REG_XOT_VAL_U & 0xFFFF) << 16) | (*REG_XOT_VAL_L & 0xFFFF);
}

void update_system_time() {
    uint32_t val = xo_sys_time;
    xo_sys_time = get_timer_cnt();
    xo_sys_time_in_sec = xo_sys_time >> 15;
    xo_day_time_in_sec += (xo_sys_time - val) >> 15;

    if(xo_day_time_in_sec >= 86400) {
        xo_day_time_in_sec -= 86400;
    }
}

bool xo_check_is_day() {
    update_system_time();
    return xo_day_time_in_sec >= XO_DAY_START && xo_day_time_in_sec < XO_DAY_END;
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

static uint16_t process_temp(uint32_t temp_code){
    // FIXME: implement log
    return temp_code;
}

static inline void snt_clk_init() {	// NOTE: now using LNT TIMER
    // start SNT clock
    sntv4_r08.TMR_SLEEP = 0;
    sntv4_r08.TMR_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR, 0x8, sntv4_r08.as_int);

    sntv4_r09.TMR_SELF_EN = 0;
    mbus_remote_register_write(SNT_ADDR, 0x9, sntv4_r09.as_int);

    sntv4_r08.TMR_EN_OSC = 1;
    mbus_remote_register_write(SNT_ADDR, 0x8, sntv4_r08.as_int);

    sntv4_r08.TMR_RESETB = 1;
    sntv4_r08.TMR_RESETB_DIV = 1;
    sntv4_r08.TMR_RESETB_DCDC = 1;
    mbus_remote_register_write(SNT_ADDR, 0x8, sntv4_r08.as_int);

    sntv4_r08.TMR_EN_SELF_CLK = 1;
    sntv4_r09.TMR_SELF_EN = 1;
    mbus_remote_register_write(SNT_ADDR, 0x8, sntv4_r08.as_int);
    mbus_remote_register_write(SNT_ADDR, 0x9, sntv4_r09.as_int);

    // sntv4_r08.TMR_EN_OSC = 0;
    // mbus_remote_register_write(SNT_ADDR, 0x8, sntv4_r08.as_int);
}

// static void set_snt_timer(uint32_t end_time) {
//     mbus_write_message32(0xEE, 0xFFFFAAAA);
//     update_system_time();
//     if(end_time <= xo_sys_time_in_sec) {
//         sys_err(0x0);
//     }
//     uint32_t val = (end_time - xo_sys_time_in_sec) * 1564;
// 
//     // TODO: set snt timer
//     mbus_remote_register_write(SNT_ADDR, 0x19, (val >> 24));
//     mbus_remote_register_write(SNT_ADDR, 0x1A, val & 0xFFFFFF);
// 
//     sntv4_r17.WUP_ENABLE = 1;
//     sntv4_r17.WUP_LC_IRQ_EN = 1;
//     sntv4_r17.WUP_AUTO_RESET = 0;
//     mbus_remote_register_write(SNT_ADDR, 0x17, sntv4_r17.as_int);
//     mbus_write_message32(0xCE, end_time - xo_sys_time_in_sec);
// }


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

/**********************************************
 * Light functions (LNTv1A)
 **********************************************/

inline static void lnt_init() {
    //////// TIMER OPERATION //////////
    
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
    lntv1a_r06.OBSEN_AFEOUT = 0x0; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x06,lntv1a_r06.as_int);
    delay(MBUS_DELAY*10);
    
    // Change Counting Time 
    lntv1a_r03.TIME_COUNTING = 0xFFFFFF; // Default : 0x258
    mbus_remote_register_write(LNT_ADDR,0x03,lntv1a_r03.as_int);
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
    // lntv1a_r03.TIME_COUNTING = 0x000; // Default : 0x258
    // mbus_remote_register_write(LNT_ADDR,0x03,lntv1a_r03.as_int);
    // delay(MBUS_DELAY*10);
    
    set_halt_until_mbus_trx();
    mbus_copy_registers_from_remote_to_local(LNT_ADDR, 0x10, 0x00, 1);
    set_halt_until_mbus_tx();
    lnt_sys_light = (((*REG1 & 0xFFFFFF) << 24) | (*REG0)) >> lnt_counter_base;
    mbus_write_message32(0xE0, lnt_sys_light >> 32);
    mbus_write_message32(0xE1, lnt_sys_light & 0xFFFFFFFF);

    // // Stop FSM: Counter Idle -> Counter Counting 
    lntv1a_r00.DBE_ENABLE = 0x0; // Default : 0x0
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x0;
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*100);
    
    // // Reset LNT
    // lntv1a_r00.RESET_AFE = 0x1; // Default : 0x1
    // lntv1a_r00.RESETN_DBE = 0x0; // Default : 0x0
    // mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    // delay(MBUS_DELAY*10);
}

static void set_lnt_timer(uint32_t end_time) {
    mbus_write_message32(0xEE, 0xFFFFAAAA);
    update_system_time();
    if(end_time <= xo_sys_time_in_sec) {
        sys_err(0x0);	// FIXME: remove this error message. Would rather have timing off than crash
    }

    // TODO: set lnt timer
    if(!lnt_start_meas) {
	uint32_t val = (end_time - xo_sys_time_in_sec) * 1564;
	mbus_remote_register_write(LNT_ADDR, 0x41, (val >> 24));
	mbus_remote_register_write(LNT_ADDR, 0x42, val & 0xFFFFFF);

	lntv1a_r40.WUP_ENABLE = 1;
	lntv1a_r40.WUP_LC_IRQ_EN = 1;
	lntv1a_r40.WUP_AUTO_RESET = 1;
	mbus_remote_register_write(LNT_ADDR, 0x40, lntv1a_r40.as_int);
    }
    else {
	uint32_t val = (end_time - xo_sys_time_in_sec) * 8;
    	lntv1a_r03.TIME_COUNTING = val;
	mbus_remote_register_write(LNT_ADDR, 0x03, lntv1a_r03.as_int);
	delay(MBUS_DELAY*10);
	lnt_start();
    }
    mbus_write_message32(0xCE, end_time - xo_sys_time_in_sec);
}

static uint16_t update_light_interval() {
    int i;
    if(lnt_last_light > lnt_sys_light) {
        for(i = 2; i >= lnt_cur_level; i--) {
            if(lnt_sys_light > LNT_UPPER_THRESH[i]) {
                lnt_cur_level = i + 1;
                break;
            }
        }
    }
    else {
        for(i = 0; i < lnt_cur_level; i++) {
            if(lnt_sys_light < LNT_LOWER_THRESH[i]) {
                lnt_cur_level = i;
                break;
            }
        }
    }
    lnt_last_light = lnt_sys_light;
    mbus_write_message32(0xDF, LNT_INTERVAL[lnt_cur_level]);
    return LNT_INTERVAL[lnt_cur_level];
}

// 0 : not in use
// 0xFFFFFFFF : time's up
static void reset_timers_list() {
    uint8_t i;
    update_system_time();
    for(i = 0; i < XOT_TIMER_LIST_LEN; i++) {
        xot_timer_list[i] = 0;
    }
}

static void set_next_time(uint8_t idx, uint16_t step) {
    update_system_time();
    xot_timer_list[idx] = xot_last_timer_list[idx];
    while(xo_sys_time_in_sec + 5 > xot_timer_list[idx]) {    // give some margin of error
        xot_timer_list[idx] += step;
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
    uint32_t i;

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
    set_halt_until_mbus_trx();
    mbus_remote_register_write(PMU_ADDR, reg_addr, reg_data);
    set_halt_until_mbus_tx();
}

static void pmu_set_adc_period(uint32_t val) {
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
}

static void pmu_set_active_clk(uint32_t setting) {
    uint8_t r = (setting >> 16) & 0xFF;
    uint8_t l = (setting >> 12) & 0xFF;
    uint8_t base = (setting >> 8) & 0xFF;
    uint8_t l_1p2 = setting & 0xFF;

    mbus_write_message32(0xDE, setting);

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
}

static void pmu_set_sleep_clk(uint32_t setting) {
    uint8_t r = (setting >> 16) & 0xFF;
    uint8_t l = (setting >> 12) & 0xFF;
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
}

inline static void pmu_set_sleep_radio() {
    pmu_set_sleep_clk(0x0F0A050F);
    // pmu_set_sleep_clk(0xF, 0xA, 0x5, 0xF/*V1P2*/);
}

inline static void pmu_set_sleep_low() {
    pmu_set_sleep_clk(0x02010101);
    // pmu_set_sleep_clk(0x2, 0x1, 0x1, 0x1/*V1P2*/);
}

static void pmu_set_sar_conversion_ratio(uint8_t ratio) {
    int i;
    for(i = 0; i < 2; i++) {
        pmu_reg_write(0x05,         // PMU_EN_SAR_RATIO_OVERRIDE; default: 12'h000
                    ((1 << 13) |    // enable override setting [12] (1'b1)
                     (0 << 12) |    // let vdd_clk always connect to vbat
                     (1 << 11) |    // enable override setting [10] (1'h0)
                     (0 << 10) |    // have the converter have the periodic reset (1'h0)
                     (1 <<  9) |    // enable override setting [8:0] (1'h0)
                     (0 <<  8) |    // switch input / output power rails for upconversion (1'h0)
                     (1 <<  7) |    // enable override setting [6:0] (1'h0)
                     (ratio)));  // binary converter's conversion ratio (7'h00)
    }
}

// 0 : normal active
// 1 : radio active
static void pmu_setting_temp_based(uint8_t mode) {
    int i;
    snt_sys_temp_code = 19; // FIXME: set this to the data from the temp meas
    for(i = 0; i < 7; i++) {
        if(i == 7 || snt_sys_temp_code < PMU_TEMP_THRESH[i]) {
            if(mode == 0) {
	        pmu_set_active_clk(PMU_ACTIVE_SETTINGS[i]);
            }
            else {
	        pmu_set_active_clk(PMU_RADIO_SETTINGS[i]);
            }
            pmu_set_sleep_clk(PMU_SLEEP_SETTINGS[i]);
            break;
        }
    }
    delay(MBUS_DELAY);
}

inline static void pmu_set_clk_init() {
    pmu_setting_temp_based(0);
    // Use the new reset scheme in PMUv3
    pmu_set_sar_conversion_ratio(pmu_sar_ratio_radio);
    pmu_set_adc_period(1); 	// TODO: figure out if this code is needed. Copied from Tstack code
}

inline static void pmu_adc_reset_setting() {
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
}

inline static void pmu_adc_disable() {
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
}

inline static void pmu_adc_enable() {
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
}

inline static void pmu_adc_read_latest() {
    // FIXME: this is weird. Readings are higher when ext_bat is lower
    // Grab latest pmu adc readings 
    // PMU register read is handled differently
    pmu_reg_write(0x00, 0x03);
    // Updated for pmuv9
    read_data_batadc = *REG0 & 0xFFFF;

    if(read_data_batadc < MRR_VOLT_THRESH) {
        read_data_batadc_diff = 0;
    }
    else {
        read_data_batadc_diff = read_data_batadc - MRR_VOLT_THRESH;
    }
}

inline static void pmu_enable_4V_harvesting() {
    // Updated for PMUv9
    pmu_reg_write(0x0E,         // PMU_EN_VOLTAGE_CLAMP_TRIM
                ((0 << 12) |    // 1: solar short by latched vbat_high (new); 0: follow [10] setting
                 (0 << 11) |    // Reset of vbat_high latch for [12]=1
                 (1 << 10) |    // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
                 (0 <<  9) |    // Enable override setting [8]
                 (0 <<  8) |    // Turn on the harvester-inhibiting switch
                 (3 <<  4) |    // clamp_tune_bottom (increases clamp thresh)
                 (0)));         // clamp_tune_top (decreases clamp thresh)
}

inline static void pmu_init() {
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
    // pmu_adc_enable();
}

/**********************************************
 * MRR functions (MRRv7)
 **********************************************/

static void reset_radio_data_arr() {
    uint8_t i;
    for(i = 0; i < 3; i++) { radio_data_arr[i] = 0; }
}

static inline void radio_power_on(){

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
    mbus_write_message32(0xBB, 0xBBBBBBBB);
    mbus_write_message32(0xBB, 0x00000000);
    
    mbus_write_message32(0xE0, radio_data_arr[0]);
    mbus_write_message32(0xE1, radio_data_arr[1]);
    mbus_write_message32(0xE2, radio_data_arr[2]);
    
    mbus_write_message32(0xBB, 0x00000000);
    mbus_write_message32(0xBB, 0xBBBBBBBB);
#endif

    // TODO: add temp and voltage restrictions

    if(!radio_on) {
        radio_on = 1;
	radio_power_on();
    }
    
    mbus_remote_register_write(MRR_ADDR,0xD, radio_data_arr[0] & 0xFFFFFF);
    mbus_remote_register_write(MRR_ADDR,0xE, (radio_data_arr[1] << 8) | (radio_data_arr[0] >> 24));
    mbus_remote_register_write(MRR_ADDR,0xF, (radio_data_arr[2] << 16 | radio_data_arr[1] >> 16));
    mbus_remote_register_write(MRR_ADDR,0x10, ((crc_data[0] & 0xFFFF) << 8 | radio_data_arr[2] >> 8));

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
    uint8_t count = 0;
    uint16_t mrr_cfo_val_fine = 0;
    uint8_t num_packets = 1;
    if (mrr_freq_hopping) num_packets = mrr_freq_hopping;

    mrr_cfo_val_fine = 0x2000;

    while (count < num_packets){
	// may be able to remove 2 lines below, GC 1/6/20
	*TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
	*REG_MBUS_WD = 0; // Disables Mbus watchdog timer

	mrrv7_r01.MRR_TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
	mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine;
	mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);
	send_radio_data_mrr_sub1();
	count++;
	if (count < num_packets){
		delay(RADIO_PACKET_DELAY);
	}
	mrr_cfo_val_fine = mrr_cfo_val_fine + mrr_freq_hopping_step; // 1: 0.8MHz, 2: 1.6MHz step
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
    mrrv7_r00.MRR_TRX_CAP_ANTP_TUNE_COARSE = 0x01f;  //ANT CAP 10b unary 830.5 MHz // FIXME: adjust per stack
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
    mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_COARSE = 0x01f; //ANT CAP 10b unary 830.5 MHz
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

static void operation_init( void ) {
    // BREAKPOINT 0x01
    mbus_write_message32(0xBA, 0x01);

    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer
    config_timer32(0, 0, 0, 0);

    // Enumeration
    enumerated = ENUMID;

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

    // Global variables
    wfi_timeout_flag = 0;

    xo_sys_time = 0;
    xo_sys_time_in_sec = 0;
    xo_day_time_in_sec = 0;

    snt_sys_temp_code = 25; 	// FIXME: use code for this
    snt_state = SNT_IDLE;

    // LNT_UPPER_THRESH[0] = 500;
    // LNT_UPPER_THRESH[1] = 2000;
    // LNT_UPPER_THRESH[2] = 6000;

    // LNT_LOWER_THRESH[0] = 250;
    // LNT_LOWER_THRESH[1] = 1000;
    // LNT_LOWER_THRESH[2] = 3000;

    // LNT_INTERVAL[0] = 60;
    // LNT_INTERVAL[1] = 300;
    // LNT_INTERVAL[2] = 600;
    // LNT_INTERVAL[3] = 1800;

    lnt_cur_level = 0;

    // MRR_SIGNAL_PERIOD = 300;
    // MRR_DATA_PERIOD = 18000;
    // MRR_TEMP_THRESH = 5;    // FIXME: use code
    // MRR_VOLT_THRESH = 0x4B;

    pmu_sar_ratio_radio = 0x36;

    // PMU_TEMP_THRESH[0] = -10;
    // PMU_TEMP_THRESH[1] = 0;
    // PMU_TEMP_THRESH[2] = 20;
    // PMU_TEMP_THRESH[3] = 30;
    // PMU_TEMP_THRESH[4] = 40;
    // PMU_TEMP_THRESH[5] = 50;
    // PMU_TEMP_THRESH[6] = 60;

    // PMU_ACTIVE_SETTINGS[0] = 0x07021004;	    // TODO: update these
    // PMU_ACTIVE_SETTINGS[1] = 0x07021004;
    // PMU_ACTIVE_SETTINGS[2] = 0x07021004;
    // PMU_ACTIVE_SETTINGS[3] = 0x07021004;
    // PMU_ACTIVE_SETTINGS[4] = 0x07021004;
    // PMU_ACTIVE_SETTINGS[5] = 0x07021004;
    // PMU_ACTIVE_SETTINGS[6] = 0x07021004;
    // PMU_ACTIVE_SETTINGS[7] = 0x07021004;

    // PMU_RADIO_SETTINGS[0] = 0x07021004;	    // TODO: update these
    // PMU_RADIO_SETTINGS[1] = 0x07021004;
    // PMU_RADIO_SETTINGS[2] = 0x07021004;
    // PMU_RADIO_SETTINGS[3] = 0x07021004;
    // PMU_RADIO_SETTINGS[4] = 0x07021004;
    // PMU_RADIO_SETTINGS[5] = 0x07021004;
    // PMU_RADIO_SETTINGS[6] = 0x07021004;
    // PMU_RADIO_SETTINGS[7] = 0x07021004;

    // PMU_SLEEP_SETTINGS[0] = 0x0F010102;
    // PMU_SLEEP_SETTINGS[1] = 0x0F010102;
    // PMU_SLEEP_SETTINGS[2] = 0x0F010102;
    // PMU_SLEEP_SETTINGS[3] = 0x0F010102;
    // PMU_SLEEP_SETTINGS[4] = 0x02010101;
    // PMU_SLEEP_SETTINGS[5] = 0x02000101;
    // PMU_SLEEP_SETTINGS[6] = 0x01010101;
    // PMU_SLEEP_SETTINGS[7] = 0x01010101;

    // Initialization
    xo_init();

    // BREAKPOINT 0x02
    mbus_write_message32(0xBA, 0x02);
    mbus_write_message32(0xED, PMU_ACTIVE_SETTINGS[0]);

#ifdef USE_PMU
    pmu_init();
#endif

#ifdef USE_SNT
    sntv4_r01.TSNS_BURST_MODE = 0;
    sntv4_r01.TSNS_CONT_MODE  = 0;
    mbus_remote_register_write(SNT_ADDR, 1, sntv4_r01.as_int);

    sntv4_r07.TSNS_INT_RPLY_SHORT_ADDR = 0x10;
    sntv4_r07.TSNS_INT_RPLY_REG_ADDR   = 0x00;
    mbus_remote_register_write(SNT_ADDR, 7, sntv4_r07.as_int);

    // snt_clk_init();
    operation_temp_run();
#ifdef USE_PMU
    // pmu_setting_temp_based(0);
#endif
#endif

#ifdef USE_LNT
    lnt_init();
#endif

#ifdef USE_MRR
    mrr_init();
    radio_on = 0;
    radio_ready = 0;
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

static void operation_sleep_notimer( void ) {
    // Diable timer
    set_wakeup_timer(0, 0, 0);
    set_xo_timer(0, 0, 0, 0);
    config_timer32(0, 0, 0, 0);
    // TODO: reset SNT timer
    operation_sleep();
}

static void sys_err(uint32_t code)
{
    mbus_write_message32(0xAF, code);
    operation_sleep_notimer();
}

/**********************************************
 * Interrupt handlers
 **********************************************/

void set_goc_cmd() {
    goc_component = (*GOC_DATA_IRQ >> 24) & 0xFF;
    goc_func_id = (*GOC_DATA_IRQ >> 16) & 0xFF;
    goc_data = *GOC_DATA_IRQ & 0xFFFF;
    goc_state = 0;
    update_system_time();
    uint8_t i;
    for(i = 0; i < XOT_TIMER_LIST_LEN; i++) {
	xot_last_timer_list[i] = xo_sys_time_in_sec;
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
    update_system_time();

    *NVIC_ICPR = (0x1 << IRQ_WAKEUP);

    mbus_write_message32(0xEE, *SREG_WAKEUP_SOURCE);
    mbus_write_message32(0xEE, *GOC_DATA_IRQ);

    // check wakeup is due to GOC
    if(*SREG_WAKEUP_SOURCE & 1) {
        if(!(*GOC_DATA_IRQ)) {
            operation_sleep(); // Need to protect against spurious wakeups
        }
        set_goc_cmd();
        reset_timers_list();
    }

    sntv4_r17.WUP_ENABLE = 0;
    mbus_remote_register_write(SNT_ADDR, 0x17, sntv4_r17.as_int);

    mbus_write_message32(0xC0, xo_day_time_in_sec);
    mbus_write_message32(0xC1, xo_sys_time_in_sec);
}

void handler_ext_int_gocep( void ) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
    set_goc_cmd();
    reset_timers_list();
}

void handler_ext_int_timer32( void ) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *TIMER32_STAT = 0x0;
    
    wfi_timeout_flag = 1;
}

void handler_ext_int_xot( void ) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_XOT);
    update_system_time();
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

int main() {
    // Only enable relevant interrupts (PREv18)
    *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
		  1 << IRQ_REG0 | 1 << IRQ_REG1 | 1 << IRQ_REG2 | 1 << IRQ_REG3 |
                  1 << IRQ_XOT);

    if(enumerated != ENUMID) {
        operation_init();
        operation_sleep_notimer();
    }

    if(lnt_start_meas == 1) {
        lnt_start_meas = 2;
        lnt_stop();
    }

    operation_temp_run();
    // pmu_adc_read_latest();

    sys_run_continuous = 0;
    do {
        if(goc_component == 0x00) {
            if(goc_func_id == 0x01) {
                if(sys_run_continuous) {
                    start_xo_cout();
                }
                else {
                    stop_xo_cout();
                }
                sys_run_continuous = !sys_run_continuous;
                goc_func_id = 0xFF;
            }
            else if(goc_func_id == 0x02) {
                // enter time in minutes
                xo_day_time_in_sec = goc_data * 60;
            }
        }
        else if(goc_component == 0x01) {
            if(!goc_state) {
                goc_state = 1;
                lnt_start_meas = 1;
            }
            else if(lnt_start_meas == 2) {
		lnt_start_meas = 0;
		goc_state = 0;
                reset_radio_data_arr();
                radio_data_arr[0] = snt_sys_temp_code;
                radio_data_arr[1] = lnt_sys_light;
                pmu_setting_temp_based(1);
                mrr_send_radio_data(1);
                pmu_setting_temp_based(0);

		set_next_time(START_LNT, 60);
            }
        }
        else if(goc_component == 0x04) {
            if(goc_func_id == 0x01) {
		if(goc_state == 0) {
                    goc_state = 1;
                    // xo_end_time_in_sec = xo_sys_time_in_sec + OPERATION_DURATION;
                    op_counter = 0;

                    snt_counter = 2;    // start code with snt storage
                    radio_beacon_counter = 0;
                    radio_counter = 0;

                    mem_light_addr = 0;
                    mem_light_len = 0;
                    mem_temp_addr = 7000;
                    mem_temp_len = 0;

		    lnt_start_meas = 0;

                    uint32_t next_hour_in_sec = goc_data * 3600;
                    xot_timer_list[RUN_SNT] = xo_sys_time_in_sec + next_hour_in_sec - xo_day_time_in_sec;
                    xot_timer_list[START_LNT] = 0;
		    xo_is_day = 0;
		    xo_last_is_day = 0;

                    radio_data_arr[0] = xo_day_time_in_sec;
                    radio_data_arr[1] = xo_sys_time_in_sec;
                    radio_data_arr[2] = 0xDEAD;
                    mrr_send_radio_data(1);
                }

		else if(goc_state == 1) {
                    if(op_counter >= SNT_OP_MAX_COUNT) {
                        goc_state = 2;
			reset_timers_list();
			update_system_time();
                        xot_timer_list[SEND_RAD] = xo_sys_time_in_sec + 600; // FIXME: set to 600
                    }
                    else {
                        // SNT measurements
                        if(xot_timer_list[RUN_SNT] == 0xFFFFFFFF) {
			    op_counter++;
                            pmu_setting_temp_based(0);

                            // TODO: compensate XO

                            if(++snt_counter >= 3) {
                                snt_counter = 0;
                                // TODO: compress this
                                mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) (mem_temp_addr + mem_temp_len), (uint32_t*) &snt_sys_temp_code, 0);
                                mem_temp_len++;
                            }

			    mbus_write_message32(0xEA, xot_timer_list[RUN_SNT]);
                            set_next_time(RUN_SNT, SNT_TEMP_UPDATE_TIME);
                        }

			xo_is_day = xo_check_is_day();

                        // LNT measurements
                        if(lnt_start_meas == 2) {
                            lnt_start_meas = 0;
                            // TODO: compress this
                            mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) (mem_light_addr + mem_light_len), (uint32_t*) &lnt_sys_light, 1);
                            mem_light_len += 2;

                            if(xo_is_day) {
                                set_next_time(START_LNT, update_light_interval());
				mbus_write_message32(0xEB, LNT_INTERVAL[lnt_cur_level]);
                            }
                        }
			else if(xot_timer_list[START_LNT] == 0xFFFFFFFF) {
                            xot_timer_list[START_LNT] = 0;
                            lnt_start_meas = 1;
                        }

			else if(!xo_last_is_day && xo_is_day) {
			    // set LNT last timer to SNT current timer for synchronization
			    xot_last_timer_list[START_LNT] = xot_last_timer_list[RUN_SNT];
			    lnt_start_meas = 1;
			}

			xo_last_is_day = xo_is_day;
                    }
                }

		else if(goc_state == 2) {
                    // SEND RADIO
                    if(xot_timer_list[SEND_RAD] == 0xFFFFFFFF) {
                        pmu_setting_temp_based(1);

                        if(xo_check_is_day()) {
                            // send beacon
                            reset_radio_data_arr();
                            radio_data_arr[0] = (0xDD << 24) | ((radio_counter & 0xFF) << 16) | (read_data_batadc & 0xFFFF);
                            radio_data_arr[1] = snt_sys_temp_code;

                            mrr_send_radio_data(0);

                            // send data
                            if(++radio_beacon_counter >= 6) { // FIXME: change to 6
                                radio_beacon_counter = 0;
                                int i;
				mbus_write_message32(0xB0, mem_light_len);
                                for(i = 0; i < mem_light_len; i += 2) {
                                    reset_radio_data_arr();
                                    set_halt_until_mbus_trx();
                                    mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) (mem_light_addr + i), PRE_ADDR, radio_data_arr, 1);
                                    set_halt_until_mbus_tx();
				    radio_data_arr[2] &= 0x0000FFFF;

                                    mrr_send_radio_data(0);
                                }

				mbus_write_message32(0xB1, mem_temp_len);
				for(i = 0; i < mem_temp_len; i += 2) {
                                    reset_radio_data_arr();
                                    set_halt_until_mbus_trx();
                                    mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) (mem_temp_addr + i), PRE_ADDR, radio_data_arr, 1);
                                    set_halt_until_mbus_tx();
				    radio_data_arr[2] &= 0x0000FFFF;

                                    mrr_send_radio_data(0);
				}

                            }

                            radio_data_arr[0] = radio_counter;
                            radio_data_arr[1] = radio_beacon_counter;
                            radio_data_arr[2] = 0xFEED;

                            mrr_send_radio_data(1);

                            radio_counter++;
                        }

                        set_next_time(SEND_RAD, 600); // FIXME: set to 600
                    }
                }
            }
        }
    } while(sys_run_continuous);

    // Find min time in system
    uint32_t min_time = 0xFFFFFFFF;
    update_system_time();
    uint8_t i;
    for(i = 0; i < XOT_TIMER_LIST_LEN; i++) {
    	if(xot_timer_list[i] != 0 && xot_timer_list[i] <= min_time) {
	    min_time = xot_timer_list[i];
	}
    }

    if(lnt_start_meas == 1) {
	uint32_t sleep_time;
	if(min_time == 0xFFFFFFFF) {
	    sleep_time = 60;	// set arbitrary value so lnt counting time will default to 32
	}
	else {
	    sleep_time = min_time - xo_sys_time_in_sec;
	}
	mbus_write_message32(0xC0, sleep_time);
	
	lnt_counter_base = 0;
	uint32_t lnt_meas_time = LNT_MEAS_TIME;
	while(((lnt_meas_time << (lnt_counter_base + 1)) + 10 < sleep_time) && lnt_counter_base < 4) {
	    lnt_counter_base++;
	}
	update_system_time();
	min_time = xo_sys_time_in_sec + LNT_MEAS_TIME << lnt_counter_base;
	mbus_write_message32(0xC1, lnt_counter_base);
    }

    if(min_time != 0xFFFFFFFF) {
	// set timer trigger so operations will trigger
	for(i = 0; i < XOT_TIMER_LIST_LEN; i++) {
	    if(xot_timer_list[i] != 0 && xot_timer_list[i] <= min_time) {
                xot_last_timer_list[i] = xot_timer_list[i];
                xot_timer_list[i] = 0xFFFFFFFF;
	    }
	}
	set_lnt_timer(min_time);
    }

    operation_sleep();

    while(1);
}
