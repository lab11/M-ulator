/******************************************************************************************
 * Author:      Roger Hsiao
 * Description: Monarch butterfly challenge code
 *                                          - PREv20 / PMUv11 / SNTv4 / FLPv3S / MRRv10
 ******************************************************************************************
 * v1: draft version; not tested on chip
 *
 * v3: first deployment in mexico; not using FLP
 * v3.1: compact code
 *
 * PMUv9 version: with PMUv11
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

#define MBUS_DELAY 100  // Amount of delay between seccessive messages; 100: 6-7ms
#define TIMER32_VAL 0xA0000  // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

// SNT states
#define SNT_IDLE        0x0
#define SNT_TEMP_LDO    0x1
#define SNT_TEMP_START  0x2
#define SNT_TEMP_READ   0x3
#define SNT_SET_PMU	0x4

#define LNT_MEAS_TIME 30
#define PMU_SETTING_TIME 10
#define SNT_TEMP_UPDATE_TIME 300

#define MEM_STORAGE_SIZE 8192

#define RADIO_PACKET_DELAY 13000  // Amount of delay between radio packets
#define RADIO_DATA_LENGTH 192

// default register values
volatile prev20_r19_t prev20_r19 = PREv20_R19_DEFAULT;

volatile sntv4_r00_t sntv4_r00 = SNTv4_R00_DEFAULT;
volatile sntv4_r01_t sntv4_r01 = SNTv4_R01_DEFAULT;
volatile sntv4_r07_t sntv4_r07 = SNTv4_R07_DEFAULT;

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
volatile uint32_t op_counter;
volatile uint8_t wfi_timeout_flag;
volatile uint8_t sys_run_continuous;

uint8_t goc_component;
uint8_t goc_func_id;
uint8_t goc_state;
uint16_t goc_data;

volatile uint16_t xo_period;
volatile uint16_t xo_next_timer_val;
volatile uint16_t xo_interval[4];
volatile uint32_t xo_data32;
volatile uint32_t xo_sys_time;
volatile uint32_t xo_end_time;
volatile uint32_t xo_day_time;
volatile uint32_t xo_day_start;
volatile uint32_t xo_day_end;
volatile uint32_t xot_thresh;
volatile uint32_t xot_last_cnt_val;
volatile uint32_t xot_last_timer_val;

volatile uint32_t snt_const_a;
volatile uint32_t snt_const_b;
volatile uint16_t snt_sys_temp;
volatile uint8_t snt_state;
volatile uint8_t snt_update_temp;
volatile uint8_t temp_data_valid;

volatile uint8_t lnt_start_meas;
volatile uint8_t lnt_cur_level;
volatile uint16_t lnt_thresh_data;
volatile uint16_t lnt_upper_thresh[3];
volatile uint16_t lnt_lower_thresh[3];
volatile uint64_t lnt_sys_light;
volatile uint64_t lnt_last_light;

volatile uint32_t mem_addr;
volatile uint32_t mem_write_data;

volatile uint32_t mrr_signal_period;
volatile uint32_t mrr_data_period;
volatile uint16_t mrr_temp_thresh;
volatile uint16_t mrr_volt_thresh;

volatile uint32_t pmu_setting_val;
volatile uint32_t pmu_cur_active_setting;
volatile uint32_t pmu_cur_sleep_setting;
volatile uint32_t pmu_temp_thresh[4];
volatile uint32_t pmu_active_settings[5];
volatile uint32_t pmu_sleep_settings[5];
volatile uint32_t pmu_radio_settings[5];
volatile uint16_t read_data_batadc;
volatile uint16_t read_data_batadc_diff;
volatile uint8_t pmu_sar_conversion_ratio;
volatile uint8_t pmu_sar_ratio_radio;

volatile uint8_t radio_ready;
volatile uint8_t radio_on;
volatile uint32_t radio_data_arr[3];
volatile uint32_t mrr_freq_hopping;
volatile uint32_t mrr_freq_hopping_step;
volatile uint32_t mrr_cfo_val_fine_min;
volatile uint32_t radio_packet_count;

// Message data structures
typedef union header{
    struct{
        unsigned timestamp : 20;
        unsigned len : 8;
        unsigned interval_type : 2;
    };
    uint32_t as_int;
} header_t;

typedef union base_data{
    struct{
        uint16_t base_light_data;
        uint16_t base_temp_data;
    };
    uint32_t as_int;
} base_data_t;

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

static void stop_timer32_timeout_check(uint32_t code){
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

    // BREAKPOint 0x03
    mbus_write_message32(0xBA, 0x03);

}

inline uint32_t get_timer_cnt() {
    return ((*REG_XOT_VAL_U & 0xFFFF) << 16) | (*REG_XOT_VAL_L & 0xFFFF);
}

void update_system_time() {
    uint32_t val = xot_last_cnt_val;
    xot_last_cnt_val = get_timer_cnt();
    // set last_timer_val to 0 if not using timer
    // if *XOT_VAL == 0, it means that system wakeup is due to xo_timer, use last_timer val
    if(xot_last_timer_val != 0 && xot_last_cnt_val == 0) {
    	val = xot_last_timer_val - val;
	xot_last_timer_val = 0;
	start_xo_cnt();
    }
    else {
    	val = xot_last_cnt_val - val;
    }

    xo_sys_time += val;
    xo_day_time += val;
    if(xo_day_time >= (86400 << 15)) {
        xo_day_time -= (86400 << 15);
    }

}

// void update_xo_counters(uint32_t val) {
//     // FIXME: this may be wrong as the timer resets when it gets to the threshold
//     xo_sys_time += val;
//     xo_day_time += val;
//     if(xo_day_time >= 86400) {
//         xo_day_time -= 86400;
//     }
//     
// }

static void set_xot_in_sec(uint8_t mode, 
                           uint16_t time_in_sec,
                           uint8_t wren_irq, 
                           uint8_t en_irq) {
    update_system_time();
    reset_xo_cnt();
    xot_last_cnt_val = 0;
    xot_last_timer_val = time_in_sec << 15;
    set_xo_timer(mode, xot_last_timer_val, wren_irq, en_irq);
}

/*
// TODO: figure out if this is needed
void xo_turn_off( void ) {
    prev20_r19.XO_DRV_CORE = 0x0;
    prev20_r19.XO_SCN_ENB  = 0x1;
    *REG_XO_CONF1 = prev20_r19.as_int;
}

// Sleep xo driver to save power
void xo_sleep( void ) {
    prev20_r19.XO_SLEEP    = 0x0;
    prev20_r19.XO_ISOLATE  = 0x1;
    *REG_XO_CONF1 = prev20_r19.as_int;
}
*/

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
    return 0;
}

static void operation_temp_run() {
    uint32_t temp_code;
    if(snt_state == SNT_IDLE) {
        temp_data_valid = 0;

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
            temp_code = *REG0;
            
            // turn off temp sensor and ldo
            temp_sensor_power_off();
            snt_ldo_power_off();

	    mbus_write_message32(0xDD, 0xBB);
            temp_data_valid = 1;
            snt_sys_temp = process_temp(temp_code);
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
}

static void lnt_stop() {
    // Change Counting Time 
    lntv1a_r03.TIME_COUNTING = 0x000; // Default : 0x258
    mbus_remote_register_write(LNT_ADDR,0x03,lntv1a_r03.as_int);
    delay(MBUS_DELAY*10);
    
    set_halt_until_mbus_trx();
    mbus_copy_registers_from_remote_to_local(LNT_ADDR, 0x10, 0x00, 1);
    set_halt_until_mbus_tx();
    lnt_sys_light = ((*REG1 & 0xFFFFFF) << 24) | (*REG0);

    // Stop FSM: Counter Idle -> Counter Counting 
    lntv1a_r00.DBE_ENABLE = 0x0; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*10);
    
    // Reset LNT
    lntv1a_r00.RESET_AFE = 0x1; // Default : 0x1
    lntv1a_r00.RESETN_DBE = 0x0; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*10);
}

inline static void lnt_init_old() {
    // Config Register A
    lntv1a_r22.TMR_S = 0x1; // Default: 0x4
    lntv1a_r22.TMR_DIFF_CON = 0x3FFD; // Default: 0x3FFB
    lntv1a_r22.TMR_POLY_CON = 0x1; // Default: 0x1

    mbus_remote_register_write(LNT_ADDR,0x22,lntv1a_r22.as_int);

    // TIMER CAP_TUNE  
    lntv1a_r21.TMR_SEL_CAP = 0x80; // Default : 8'h8
    lntv1a_r21.TMR_SEL_DCAP = 0x3F; // Default : 6'h4

    //lntv1a_r21.TMR_SEL_CAP = 0x8; // Default : 8'h8
    //lntv1a_r21.TMR_SEL_DCAP = 0x4; // Default : 6'h4
    lntv1a_r21.TMR_EN_TUNE1 = 0x1; // Default : 1'h1
    lntv1a_r21.TMR_EN_TUNE2 = 0x1; // Default : 1'h1
    mbus_remote_register_write(LNT_ADDR,0x21,lntv1a_r21.as_int);

    // Enable Frequency Monitoring 
    lntv1a_r40.WUP_ENABLE_CLK_SLP_OUT = 0x0; 
    mbus_remote_register_write(LNT_ADDR,0x40,lntv1a_r40.as_int);

    // Change ABUF Input
    lntv1a_r07.OBSSEL_ABUF = 0x2; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x07,lntv1a_r07.as_int);

    // Set LNT Threshold
    lntv1a_r05.THRESHOLD_HIGH = 0x30; // Default : 12'd40
    lntv1a_r05.THRESHOLD_LOW = 0x10; // Default : 12'd20
    mbus_remote_register_write(LNT_ADDR,0x05,lntv1a_r05.as_int);

    // TIMER SELF_EN Disable 
    lntv1a_r21.TMR_SELF_EN = 0x0; // Default : 0x1
    mbus_remote_register_write(LNT_ADDR,0x21,lntv1a_r21.as_int);

    // EN_OSC 
    lntv1a_r20.TMR_EN_OSC = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);

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
    lntv1a_r20.TMR_EN_OSC = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x20,lntv1a_r20.as_int);
    delay(100);

    // Run FDIV
    lntv1a_r17.FDIV_RESETN = 0x1; // Default : 0x0
    lntv1a_r17.FDIV_CTRL_FREQ = 0x8; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x17,lntv1a_r17.as_int);
    delay(MBUS_DELAY*10);

    // Bias Current
    lntv1a_r01.CTRL_IBIAS_VBIAS = 0x7; // Default : 0x7, Chosen : 0x7
    lntv1a_r01.CTRL_IBIAS_I = 0x2; // Default : 0x8, chosen : 0x2
    //lntv1a_r01.CTRL_VOFS_CANCEL = 0x1; // Default : 0x8
    mbus_remote_register_write(LNT_ADDR,0x01,lntv1a_r01.as_int);
    delay(MBUS_DELAY*10);

    // Vbase regulation voltage
    lntv1a_r02.CTRL_VREF_PV_V = 0x1; // Default : 0x2
    mbus_remote_register_write(LNT_ADDR,0x02,lntv1a_r02.as_int);
    delay(MBUS_DELAY*10);

    // Monitor AFEOUT
    lntv1a_r06.OBSEN_AFEOUT = 0x0; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x06,lntv1a_r06.as_int);
    delay(MBUS_DELAY*10);

    // Release LDC_PG 
    lntv1a_r00.LDC_PG = 0x0; // Default : 0x1
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*10);

    // Release LDC_ISOLATE
    lntv1a_r00.LDC_ISOLATE = 0x0; // Default : 0x1
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*10);

    // Change Counting Time 
    lntv1a_r03.TIME_COUNTING = 0x020; // Default : 0x258, It was 0x20
    mbus_remote_register_write(LNT_ADDR,0x03,lntv1a_r03.as_int);
    delay(MBUS_DELAY*10);

    // Change Monitoring & Hold Time 
    lntv1a_r04.TIME_MONITORING = 0x00A; // Default : 0x010
    mbus_remote_register_write(LNT_ADDR,0x04,lntv1a_r04.as_int);
    delay(MBUS_DELAY*10);

    // Release Reset 
    lntv1a_r00.RESET_AFE = 0x0; // Default : 0x1
    lntv1a_r00.RESETN_DBE = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*10);

    lntv1a_r00.DBE_ENABLE = 0x1; // Default : 0x0
    lntv1a_r00.WAKEUP_WHEN_DONE = 0x1; // Default : 0x0
    lntv1a_r00.MODE_CONTINUOUS = 0x1; // Default : 0x0
    mbus_remote_register_write(LNT_ADDR,0x00,lntv1a_r00.as_int);
    delay(MBUS_DELAY*10);
}

static void operation_lnt_run() {
    lnt_last_light = lnt_sys_light;
    lnt_sys_light = 0;
}

static void update_xo_period_under_light() {
    int i;
    if(lnt_last_light > lnt_sys_light) {
        for(i = 2; i >= lnt_cur_level; i--) {
            if(lnt_sys_light > lnt_upper_thresh[i]) {
                lnt_cur_level = i + 1;
                break;
            }
        }
    }
    else {
        for(i = 0; i < lnt_cur_level; i++) {
            if(lnt_sys_light < lnt_lower_thresh[i]) {
                lnt_cur_level = i;
                break;
            }
        }
    }
    xo_period = xo_interval[lnt_cur_level];
}

/**********************************************
 * MEM Functions
 **********************************************/


/**********************************************
 * CRC16 Encoding
 **********************************************/

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
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff));
    }

    data0 = data0 + remainder;

    static uint32_t msg_out[3];
    radio_data_arr[0] = data2;
    radio_data_arr[1] = data1;
    radio_data_arr[2] = data0;

    mbus_write_message32(0xE0, radio_data_arr[0]);
    mbus_write_message32(0xE1, radio_data_arr[1]);
    mbus_write_message32(0xE2, radio_data_arr[2]);

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
    pmu_cur_active_setting = setting;
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
    pmu_cur_sleep_setting = setting;
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

static void pmu_setting_temp_based() {
    int i;
    snt_sys_temp = 20;	// FIXME: set this to the data from the temp meas
    for(i = 0; i < 5; i++) {
        if(i == 4 || snt_sys_temp < pmu_temp_thresh[i]) {
	    // pmu_set_sar_conversion_ratio(pmu_sar_ratio_radio);
	    pmu_set_active_clk(pmu_radio_settings[i]);
	    // if(pmu_use_radio_setting > 0) {
	    //     pmu_set_sar_conversion_ratio(pmu_sar_ratio_radio);
	    //     pmu_set_active_clk(pmu_radio_settings[i]);
	    // }
	    // else {
	    //     pmu_set_sar_conversion_ratio(pmu_sar_conversion_ratio);
	    //     pmu_set_active_clk(pmu_active_settings[i]);
	    // }
            pmu_set_sleep_clk(pmu_sleep_settings[i]);
            break;
        }
    }
}

inline static void pmu_set_clk_init() {
    pmu_setting_temp_based();
    // Use the new reset scheme in PMUv3
    // pmu_set_sar_conversion_ratio(pmu_sar_conversion_ratio);
    pmu_set_sar_conversion_ratio(pmu_sar_ratio_radio);
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

    if(read_data_batadc < mrr_volt_thresh) {
        read_data_batadc_diff = 0;
    }
    else {
        read_data_batadc_diff = read_data_batadc - mrr_volt_thresh;
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
    pmu_adc_enable();
}

/**********************************************
 * MRR functions (MRRv7)
 **********************************************/

static void reset_radio_data_arr() {
    uint8_t i;
    for(i = 0; i < 3; i++) { radio_data_arr[i] = 0; }
}

static void radio_power_on(){
    radio_on = 1;

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


static void mrr_configure_pulse_width_long(){

    //   mrrv7_r12.MRR_RAD_FSM_TX_PW_LEN = 12; //50us PW
    //   mrrv7_r13.MRR_RAD_FSM_TX_C_LEN = 1105;
    //   mrrv7_r12.MRR_RAD_FSM_TX_PS_LEN = 25; // PW=PS   

    mrrv7_r12.MRR_RAD_FSM_TX_PW_LEN = 24; //50us PW
    mrrv7_r13.MRR_RAD_FSM_TX_C_LEN = 200;
    mrrv7_r12.MRR_RAD_FSM_TX_PS_LEN = 49; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv7_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);

}

static void send_radio_data_mrr_sub1(){

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
    stop_timer32_timeout_check(0x3);

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

    // CRC16 Encoding 
    uint32_t* output_data;
    // TODO: add temp and voltage restrictions
    // TODO: figure out how to use this encoding
    output_data = crcEnc16(radio_data_arr[2], radio_data_arr[1], radio_data_arr[0]);

    if(!radio_on) {
	radio_power_on();
    }
    
    mbus_remote_register_write(MRR_ADDR,0xD, radio_data_arr[0] >> 8);
    mbus_remote_register_write(MRR_ADDR,0xE, (radio_data_arr[0] << 24) | (radio_data_arr[1] >> 16));
    mbus_remote_register_write(MRR_ADDR,0xF, (radio_data_arr[1] << 16) | (radio_data_arr[2] >> 24));
    mbus_remote_register_write(MRR_ADDR,0x10, radio_data_arr[2] & 0xFFFFFF);

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

    uint32_t count = 0;
    uint32_t mrr_cfo_val_fine = 0;
    uint32_t num_packets = 1;
    if (mrr_freq_hopping) num_packets = mrr_freq_hopping;

    mrr_cfo_val_fine = 0x2000;

    while (count < num_packets){
#ifdef DEBUG_MBUS_MSG
	mbus_write_message32(0xCE, mrr_cfo_val);
#endif
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
    mrrv7_r00.MRR_TRX_CAP_ANTP_TUNE_COARSE = 0x01f;  //ANT CAP 10b unary 830.5 MHz
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

    xo_period = 60;
    xo_interval[0] = 60;
    xo_interval[1] = 300;
    xo_interval[2] = 1500;
    xo_interval[3] = 7500;

    xo_day_time = 0;
    xo_day_start = 18000;
    xo_day_end = 36400;
    xot_last_timer_val = 0;
    xot_last_cnt_val = 0;

    snt_const_a = 9.45;
    snt_const_b = -1628.85;
    snt_sys_temp = 25;
    snt_state = SNT_IDLE;
    temp_data_valid = 0;
    snt_update_temp = 0b01;

    // lnt_upper_thresh[0] = 500;
    // lnt_upper_thresh[1] = 2000;
    // lnt_upper_thresh[2] = 6000;

    // lnt_lower_thresh[0] = 250;
    // lnt_lower_thresh[1] = 1000;
    // lnt_lower_thresh[2] = 3000;

    lnt_cur_level = 0;
    lnt_start_meas = 0;

    mem_addr = 0;
    mem_write_data = 0;

    mrr_signal_period = 60; // this is supposed to be 300
    mrr_data_period = 18000;
    mrr_temp_thresh = 5;
    mrr_volt_thresh = 0x4B;

    pmu_setting_val = 0;
    pmu_temp_thresh[0] = 0;
    pmu_temp_thresh[1] = 10;
    pmu_temp_thresh[2] = 25;
    pmu_temp_thresh[3] = 65;
    // pmu_sar_conversion_ratio = 0x32;
    pmu_sar_ratio_radio = 0x36;
    // pmu_active_settings[0] = 0x0D021004;    // TODO: update this
    // pmu_active_settings[1] = 0x0D021004;    // PMU10C
    // pmu_active_settings[2] = 0x05011002;    // PMU25C
    // pmu_active_settings[3] = 0x02011002;    // PMU35C
    // pmu_active_settings[4] = 0x0A040708;    // PMU75C
    pmu_sleep_settings[0] = 0x0F010102;
    pmu_sleep_settings[1] = 0x0F010102;
    pmu_sleep_settings[2] = 0x02010101;
    pmu_sleep_settings[3] = 0x02000101;
    pmu_sleep_settings[4] = 0x01010101;
    pmu_radio_settings[0] = 0x07021004;	    // TODO: update these
    pmu_radio_settings[1] = 0x07021004;
    pmu_radio_settings[2] = 0x07021004;
    pmu_radio_settings[3] = 0x07021004;
    pmu_radio_settings[4] = 0x07021004;


    // Initialization
    xo_init();

    // BREAKPOINT 0x02
    mbus_write_message32(0xBA, 0x02);

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
    operation_temp_run();
#ifdef USE_PMU
    pmu_setting_temp_based();
    mbus_write_message32(0xBA, 0x00);
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

static void operation_sleep_with_xo_cnt( void ) {
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;
    start_xo_cnt();

    mbus_sleep_all();
    while(1);
}

static void operation_sleep_noirqreset( void ) {
    mbus_sleep_all();
    while(1);
}

static void operation_sleep_notimer( void ) {
    // Diable timer
    set_wakeup_timer(0, 0, 0);
    set_xo_timer(0, 0, 0, 0);
    config_timer32(0, 0, 0, 0);
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

    // check wakeup is due to GOC
    if(*SREG_WAKEUP_SOURCE & 1) {
        set_goc_cmd();
	// update_xo_counters(get_timer_cnt());
	// xot_last_timer_val -= get_timer_cnt();
    }
    mbus_write_message32(0xCA, xo_day_time >> 15);
}

void handler_ext_int_gocep( void ) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
    set_goc_cmd();
}

void handler_ext_int_timer32( void ) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *TIMER32_STAT = 0x0;
    
    wfi_timeout_flag = 1;
    mbus_write_message32(0xDD, 0xAADDDDD);
}

void handler_ext_int_xot( void ) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_XOT);
    update_system_time();
    mbus_write_message32(0xDD, 0xBBDDDDD);
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

    // BREAKPOINT 0x00
    mbus_write_message32(0xBA, 0x00);

    if(enumerated != ENUMID) {
	mbus_write_message32(0xAA, 0xAAAAAAAA);
	uint32_t a = 13, b = 54;
	mbus_write_message32(0xEE, a * b);
	mbus_write_message32(0xBB, 0xBBBBBBBB);
        operation_init();
        operation_sleep_notimer();
    }

    if(lnt_start_meas) {
	lnt_stop();
	lnt_start_meas = 0;
	mbus_write_message32(0xD2, (lnt_sys_light >> 24) & 0xFFFFFF);
	mbus_write_message32(0xD2, lnt_sys_light & 0xFFFFFF);
    }

    set_xo_timer(0, 0, 0, 0);

#ifdef USE_SNT
    if(snt_update_temp == 0b11) {
	operation_temp_run();
#ifdef USE_PMU
	pmu_setting_temp_based();
#endif
    }
    snt_update_temp |= 0b01;
#endif

    xo_next_timer_val = 0;
    sys_run_continuous = 0;

    do {
        if(goc_component == 0xFF) {}
        else if(goc_component == 0x00) {
            // if(goc_func_id == 0x00) {
            //     mbus_write_message32(0xD0, xo_period);
            //     int i;
            //     for(i = 0; i < 4; i++)
            //     {
            //         mbus_write_message32(0xD0, xo_interval[i]);
            //     }
            //     mbus_write_message32(0xD0, xo_day_time);
            //     mbus_write_message32(0xD0, xo_day_start);
            //     mbus_write_message32(0xD0, xo_day_end);
            // }
            // else if(goc_func_id == 0x01) {
            //     if(sys_run_continuous) {
            //         stop_xo_cout();
            //     }
            //     else {
            //         start_xo_cout();
            //         goc_component = 0xFF;
            //     }
            //     sys_run_continuous = !sys_run_continuous;
            // }
	    // else if(goc_func_id == 0x02) {
	    // 	set_xo_timer(0, 0, 0, 0);
	    // }
            // else if(goc_func_id == 0x03) {
            //     xo_period = goc_data;
            // }
            // else if(goc_func_id == 0x04) {
            //     if(!goc_state) {
            //         goc_state = 1;
            //         op_counter = 0;
            //     }

            //     if(++op_counter <= goc_data) {
	    //         xo_next_timer_val = xo_period;
            //     }
            // }
            // else if(goc_func_id == 0x05) {
            //     int i = (goc_data >> 12) & 0xF;
            //     if(i < 4) {
            //         xo_interval[i] = goc_data & 0xFFF;
            //     }
            // }
            // else if(goc_func_id == 0x06) {
            //     xo_day_time = goc_data;
            // }
            // else if(goc_func_id == 0x07) {
            //     xo_data32 &= 0x0000FFFF;
            //     xo_data32 |= goc_data << 16;
            // }
            // else if(goc_func_id == 0x08) {
            //     xo_data32 &= 0xFFFF0000;
            //     xo_data32 |= goc_data;
            // }
            // else if(goc_func_id == 0x09) {
            //     if(!goc_data) {
            //         xo_day_start = xo_data32;
            //     }
            //     else {
            //         xo_day_end = xo_data32;
            //     }
            // }
        }
        else if(goc_component == 0x01) {
            if(goc_func_id == 0x00) {}
            else if(goc_func_id == 0x01) {
		operation_temp_run();
            }
            else if(goc_func_id == 0x02) {
                if(!goc_state) {
                    goc_state = 1;
                    op_counter = 0;
                }

                if(++op_counter <= goc_data) {
		    mbus_write_message32(0xD1, snt_sys_temp);
		    xo_next_timer_val = xo_period;
                }
            }
	    else if(goc_func_id == 0x03) {
		if(snt_update_temp & 0b10) { snt_update_temp &= 0b01; }
		else { snt_update_temp |= 0b10; }
	    }
        }
        else if(goc_component == 0x02) {
            if(goc_func_id == 0x00) {
                int i;
                for(i = 0; i < 3; i++) {
                    mbus_write_message32(0xD2, lnt_upper_thresh[i]);
                }
            }
            else if(goc_func_id == 0x01) {
		if(!goc_state) {
		    goc_state = 1;
		    op_counter = 0;
		}

		if(goc_state == 1) {
		    goc_state = 2;
		    lnt_start_meas = 1;
		}
		else if(goc_state == 2) {
		    goc_state = 1;
		    if(++op_counter <= goc_data) {
			xo_next_timer_val = xo_period;
		    }
		}
                // operation_lnt_run();
		// lnt_stop();
		// mbus_copy_registers_from_remote_to_local(LNT_ADDR, 0x10, 0x00, 1);
		// lnt_sys_light = ((*REG1 & 0xFFFFFF) << 24) | (*REG0);
		// mbus_write_message32(0xD2, (lnt_sys_light >> 24) & 0xFFFFFF);
		// mbus_write_message32(0xD2, lnt_sys_light & 0xFFFFFF);
            }
            else if(goc_func_id == 0x02) {
                lnt_thresh_data = goc_data;
            }
            else if(goc_func_id == 0x03) {
                uint8_t i = goc_data & 0x000F;
                if(i < 3) {
                    if(goc_data & 0x00F0) {
                        lnt_upper_thresh[i] = lnt_thresh_data;
                    }
                    else {
                        lnt_lower_thresh[i] = lnt_thresh_data;
                    }
                }
            }
            else if(goc_func_id == 0x04) {
                // if(!goc_state) {
                //    goc_state = 1;
                //    op_counter = 0;
                // }

                // if(++op_counter <= goc_data) {
		//     lnt_start_meas = 1;
                // }
            }
            else if(goc_func_id == 0x05) {
                if(!goc_state) {
                    goc_state = 1;
                    op_counter = 0;
                }

		if(goc_state == 1) {
		    goc_state = 2;
		    lnt_start_meas = 1;
		}
		else if(goc_state == 2) {
		    goc_state = 1;
		    if(++op_counter <= goc_data) {
			update_xo_period_under_light();
			xo_next_timer_val = xo_period;
		    }
		}
            }
        }
        else if(goc_component == 0x03) {
            // if(goc_func_id == 0x00) {
            //     mbus_write_message32(0xD3, mem_addr);
            //     mbus_write_message32(0xD3, mem_write_data);
            // }
            // else if(goc_func_id == 0x01) {
            //     mem_addr = goc_data;
            // }
            // else if(goc_func_id == 0x02) {
            //     mem_write_data &= 0x0000FFFF;
            //     mem_write_data |= goc_data << 16;
            // }
            // else if(goc_func_id == 0x03) {
            //     mem_write_data &= 0xFFFF0000;
            //     mem_write_data |= goc_data;
            // }
            // else if(goc_func_id == 0x04) {
	    //     mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*) mem_addr, &mem_write_data, 0);
            // }
            // else if(goc_func_id == 0x05) {
	    //     uint32_t mem_read_data;
	    //     set_halt_until_mbus_trx();
	    //     mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*) mem_addr, PRE_ADDR, &mem_read_data, goc_data & 0xFF);
	    //     set_halt_until_mbus_tx();
            // }
        }
        else if(goc_component == 0x04) {
            if(goc_func_id == 0x00) {
                // mbus_write_message32(0xD4, mrr_signal_period);
                // mbus_write_message32(0xD4, mrr_data_period);
                // mbus_write_message32(0xD4, mrr_temp_thresh);
                // mbus_write_message32(0xD4, mrr_volt_thresh);
            }
            else if(goc_func_id == 0x01) {
                if(goc_state == 0) {
                    goc_state = 1;
                    op_counter = 0;
                    xo_end_time = xo_sys_time + goc_data;
                }

		// FIXME: FIX THIS
                if(goc_state == 1) {
                    if(xo_sys_time >= xo_end_time) {
                        goc_state = 2;
                    }
                    else if(xo_day_time >= xo_day_start && xo_day_time < xo_day_end) {
                        // operation_temp_run();
                        // operation_lnt_run();
                        // storage code
                        update_xo_period_under_light();
                        // set_xot_in_sec(0, xo_period, 1, 0);
                    }
                }

                if(goc_state == 2) {
                    // operation_temp_run();
                    if(xo_sys_time >= xo_end_time) {
                        // if snt_sys_temp >= mrr_temp_thresh 
                        // && sys_volt_lvl >= mrr_volt_thresh
                        // then radio all the data

                        xo_end_time = xo_sys_time + mrr_data_period;
                    }
                    else {
                        // send hello signal
                    }
                    // set_xot_in_sec(0, mrr_signal_period, 1, 0);
                }
            }
            else if(goc_func_id == 0x02) {}
            else if(goc_func_id == 0x03) {
                mrr_signal_period = goc_data;
            }
            else if(goc_func_id == 0x04) {
                mrr_data_period = goc_data;
            }
            else if(goc_func_id == 0x05) {
                mrr_temp_thresh = goc_data;
            }
            else if(goc_func_id == 0x06) {
                if(!goc_state) {
                    goc_state = 1;
                    op_counter = 0;
                }

		if(++op_counter <= goc_data) {
		    xo_next_timer_val = mrr_signal_period;
		    reset_radio_data_arr();
		    radio_data_arr[0] = 0xDEADBEEF;
		    mrr_send_radio_data(1);
                }
            }
            else if(goc_func_id == 0x07) {
                // radio out the first goc_data - 1 words in the mem layer
            }
            else if(goc_func_id == 0x08) {
                if(!goc_state) {
                    goc_state = 1;
                    op_counter = 0;
                }

		if(++op_counter <= goc_data) {
		    xo_next_timer_val = mrr_signal_period;
		    reset_radio_data_arr();
		    operation_temp_run();
		    radio_data_arr[0] = snt_sys_temp;
                    // send out signal
		    mrr_send_radio_data(1);
                }
		else {
		}
            }
	    else if(goc_func_id == 0x09) {
	    	if(!goc_state) {
		    goc_state = 1;
		    lnt_start_meas = 1;
		}
		else if(goc_state == 1) {
		    reset_radio_data_arr();
		    operation_temp_run();
		    radio_data_arr[0] = snt_sys_temp;
		    radio_data_arr[1] = lnt_sys_light;
                    // send out signal
		    mrr_send_radio_data(1);
		}
	    }
        }
        else if(goc_component == 0x05) {
            if(goc_func_id == 0x00) {
                // int i;
                // for(i = 0; i < 5; i++) {
                //     if(i < 4) {
                //         mbus_write_message32(0xD4, pmu_temp_thresh[i]);
                //     }
                //     mbus_write_message32(0xD4, pmu_active_settings[i]);
                //     mbus_write_message32(0xD4, pmu_sleep_settings[i]);
                //     mbus_write_message32(0xD4, pmu_radio_settings[i]);
                // }
                // mbus_write_message32(0xD4, pmu_setting_val);
                // mbus_write_message32(0xD4, pmu_cur_active_setting);
                // mbus_write_message32(0xD4, pmu_cur_sleep_setting);
            }
            else if(goc_func_id == 0x01) {
                pmu_adc_read_latest();
                mbus_write_message32(0xD4, read_data_batadc);
                mbus_write_message32(0xD4, read_data_batadc_diff);
            }
            else if(goc_func_id == 0x02) {
                pmu_adc_read_latest();

                if(goc_data == 0) {
                    mrr_volt_thresh = read_data_batadc;
                }
                else {
                    mrr_volt_thresh = goc_data;
                }
            }
            else if(goc_func_id == 0x03) {
                pmu_sar_conversion_ratio = goc_data & 0xFF;
                pmu_set_sar_conversion_ratio(pmu_sar_conversion_ratio);
            }
            else if(goc_func_id == 0x04) {
                pmu_setting_val &= 0x0000FFFF;
                pmu_setting_val |= goc_data << 16;
            }
            else if(goc_func_id == 0x05) {
                pmu_setting_val &= 0xFFFF0000;
                pmu_setting_val |= goc_data;
            }
            else if(goc_func_id == 0x06) {
                if(goc_data > 0) {
                    pmu_set_active_clk(pmu_setting_val);
                }
                else {
                    pmu_set_sleep_clk(pmu_setting_val);
                }
            }
            else if(goc_func_id == 0x07) {
                sys_run_continuous = !sys_run_continuous;
                goc_component = 0xFF;
            }
            else if(goc_func_id == 0x08) {
                uint8_t i = (goc_data >> 12) & 0xF;
                if(i < 4) {
                    pmu_temp_thresh[i] = goc_data & 0x0FFF;
                }
            }
            else if(goc_func_id == 0x09) {
                uint8_t s = (goc_data >> 4) & 0xF;
                uint8_t i = goc_data & 0xF;
                if(i < 4) {
                    if(s == 2) {
                        pmu_radio_settings[i] = pmu_setting_val;
                    }
                    else if(s == 1) {
                        pmu_active_settings[i] = pmu_setting_val;
                    }
                    else if(s == 0) {
                        pmu_sleep_settings[i] = pmu_setting_val;
                    }
                }
            }
            else if(goc_func_id == 0x0A) {
                // operation_temp_run();
                // pmu_setting_temp_based();
            }
        }
    } while(sys_run_continuous);

    if(lnt_start_meas) { 
	lnt_start(); 
	xo_next_timer_val = LNT_MEAS_TIME;
	snt_update_temp &= 0b10;
    }
    else if(snt_update_temp == 0b11 && xo_next_timer_val == 0) {
    	xo_next_timer_val = SNT_TEMP_UPDATE_TIME;
    }

    if(xo_next_timer_val) {
	set_xot_in_sec(0, xo_next_timer_val, 1, 0);
    }

    mbus_write_message32(0xED, 0xEEEEEEEE);
    operation_sleep_with_xo_cnt();
    
    while(1);
}
