/******************************************************************************************
 * Author:      Roger Hsiao
 * Description: Monarch butterfly challenge code
 *                                          - PREv17 / PMUv4 / SNT / FLP
 ******************************************************************************************
 * v1: draft version; not tested on chip
 *
 ******************************************************************************************/

//#include "../include/PREv18.h"
#include "../../../pre_v17/software/include/PREv17.h"
#include "../include/SNTv4_RF.h"
#include "../include/mbus.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// Stack order: SNT->FLP
#define PRE_ADDR 0x1
#define SNT_ADDR 0x4
#define FLP_ADDR 0x5

// Temp sensor parameters
#define MBUS_DELAY 100      // Amount of delay between successive messages; 100: 6-7ms

// MBC states
#define MBC_IDLE        0x0
#define MBC_READY       0x1
#define MBC_TEMP_READ   0x3
#define MBC_FLASH_WRITE 0x5
#define MBC_FLASH_READ  0x6

// GOC states
#define GOC_IDLE            0x0
#define GOC_FLASH_WRITE1    0x1
#define GOC_FLASH_WRITE2    0x2
#define GOC_FLASH_READ1     0x3
#define GOC_TEMP_TEST 	    0x4

// SNT states
#define SNT_IDLE        0x0
#define SNT_TEMP_LDO    0x1
#define SNT_TEMP_START  0x2
#define SNT_TEMP_READ   0x3

// FIXME: Make this more versatile
// FLP states
#define FLP_OFF     0x0
#define FLP_ON      0x1
#define FLP_LOCAL_TO_SRAM   0x2
#define FLP_SRAM_TO_LOCAL   0x3
#define FLP_SRAM_TO_FLASH   0x4
#define FLP_FLASH_TO_SRAM   0x5
#define FLP_ERASE   0x6

// CP parameters
#define TIMERWD_VAL 0xFFFFF  // 0xFFFFF about 13 sec with Y5 running default clock (PRCv17)
// FIXME: Update this
#define TIMER32_VAL 0x20000  // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

#define ENUMID 0xDEADBEEF

/**********************************************
 * Global variables
 **********************************************
 * "static" limits the variables to this file, giving the compiler more freedom
 * "volatile" should only be used for MMIO to ensure memory storage
 */
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t flash_addr;
volatile uint32_t flash_data;
volatile uint32_t temp_data;
volatile uint32_t goc_temp_arr[20];
volatile uint16_t goc_temp_test_count;
volatile uint8_t wakeup_data_header;
volatile uint8_t mbc_state;
volatile uint8_t goc_state;
volatile uint8_t snt_state;
volatile uint8_t flp_state;
volatile uint8_t temp_data_valid;
volatile uint8_t sensor_queue;      // [0]: LNT; [1]: SNT; [2]: RDC;

// default register values
/*
volatile prev18_r0B_t prev18_r0B = PREv18_R0B_DEFAULT;
volatile prev18_r19_t prev18_r19 = PREv18_R19_DEFAULT;
volatile prev18_r1A_t prev18_r1A = PREv18_R1A_DEFAULT;
volatile prev18_r1C_t prev18_r1C = PREv18_R1C_DEFAULT;
*/

volatile prev17_r0D_t prev17_r0D = PREv17_R0D_DEFAULT;

volatile sntv4_r00_t sntv4_r00 = SNTv4_R00_DEFAULT;
volatile sntv4_r01_t sntv4_r01 = SNTv4_R01_DEFAULT;
volatile sntv4_r07_t sntv4_r07 = SNTv4_R07_DEFAULT;


/**********************************************
 * XO Functions
 **********************************************/

// write to XO driver 0x19
void XO_ctrl(uint32_t xo_pulse_sel,
	     uint32_t xo_delay_en,
	     uint32_t xo_drv_start_up,
	     uint32_t xo_drv_core,
	     uint32_t xo_rp_low,
	     uint32_t xo_rp_media,
	     uint32_t xo_rp_mvt,
	     uint32_t xo_rp_svt,
	     uint32_t xo_scn_clk_sel,
	     uint32_t xo_scn_enb) {
            
    *REG_XO_CONTROL = ((xo_pulse_sel     << 11) |
                       (xo_delay_en      << 8)  |
                       (xo_drv_start_up  << 7)  |
                       (xo_drv_core      << 6)  |
                       (xo_rp_low        << 5)  |
                       (xo_rp_media      << 4)  |
                       (xo_rp_mvt        << 3)  |
                       (xo_rp_svt        << 2)  |
                       (xo_scn_clk_sel   << 1)  |
                       (xo_scn_enb       << 0));
    mbus_write_message32(0xA1, *REG_XO_CONTROL);
}

/*
void xo_init( void ) {
    // Parasitic capacitance tuning (6 bits for each; each 1 adds 1.8pF)
    uint32_t xo_cap_drv = 0x3F;
    uint32_t xo_cap_in  = 0x3F;
    prev18_r1A.XO_CAP_TUNE = ((xo_cap_drv << 6) | (xo_cap_in << 0));    // XO_CLK output pad
    *REG_XO_CONF2 = prev18_r1A.as_int;

    // XO xonfiguration
    prev18_r19.XO_EN_DIV     = 0x1; // divider enable
    prev18_r19.XO_S          = 0x1; // division ration for 16kHz out
    prev18_r19.XO_SEL_CP_DIV = 0x0; // 1: 0.3V-generation charge-pump uses divided clock
    prev18_r19.XO_EN_OUT     = 0x1; // XO output enable
    prev18_r19.XO_PULSE_SEL  = 0x4; // pulse with sel, 1-hot encoded
    prev18_r19.XO_DELAY_EN   = 0x3; // pair usage together with xo_pulse_sel
    // Pseudo-Resistor selection
    prev18_r19.XO_RP_LOW     = 0x0;
    prev18_r19.XO_RP_MEDIA   = 0x1;
    prev18_r19.XO_RP_MVT     = 0x0;
    prev18_r19.XO_RP_SVT     = 0x0;

    prev18_r19.XO_SLEEP = 0x0;
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(100); // 1ms

    prev18_r19.XO_ISOLATE      = 0x0;
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(100); 

    prev18_r19.XO_DRV_START_UP = 0x1;
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(2000); // 1s

    prev18_r19.XO_SCN_CLK_SEL = 0x1;
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(2000); // 300us

    prev18_r19.XO_SCN_CLK_SEL = 0x0;
    prev18_r19.XO_SCN_ENB     = 0x0;
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(2000); // 1s

    prev18_r19.XO_DRV_START_UP = 0x0;
    prev18_r19.XO_DRV_CORE     = 0x1;
    prev18_r19.XO_SCN_CLK_SEL  = 0x1;
    *REG_XO_CONF1 = prev18_r19.as_int;
    
    enable_xo_timer();
    // TODO: Not needed? Takes power
    start_xo_cout();

    // BREAKPOINT 0x03
    mbus_write_message32(0xBA, 0x03);

}
*/

/*
void xo_turn_off( void ) {
    prev18_r19.XO_DRV_CORE = 0x0;
    prev18_r19.XO_SCN_ENB  = 0x1;
    *REG_XO_CONF1 = prev18_r19.as_int;
}

// Sleep xo driver to save power
void xo_sleep( void ) {
    prev18_r19.XO_SLEEP    = 0x0;
    prev18_r19.XO_ISOLATE  = 0x1;
    *REG_XO_CONF1 = prev18_r19.as_int;
}
*/

static void XO_div(uint32_t div_val) {
    uint32_t xo_cap_drv = 0x3F; // Additional cap on OSC_DRV
    uint32_t xo_cap_in  = 0x3F; // Additional cap on OSC_IN
    *REG_XO_CONFIG = ((div_val      << 16)  |
                      (xo_cap_drv   << 6)   |
                      (xo_cap_in    << 0));
}

static void XO_init( void ) {
    
    // XO_CLK output pad (0: Disabled; 1: 32kHz; 2: 16kHz; 3: 8kHz)
    uint32_t xot_clk_out_sel = 0x1;
    // Parasitic capacitance tuning (6-bit for each; each one adds 1.8pF)
    uint32_t xo_cap_drv = 0x3F; // Additional cap on OSC_DRV
    uint32_t xo_cap_in  = 0x3F; // Additional cap on OSC_IN

    // Pulse length selection
    uint32_t xo_pulse_sel = 0x4;    // XO_PULSE_SEL
    uint32_t xo_delay_en  = 0x3;    // XO_DELAY_EN

    // Pseudo-resisitor selection
    uint32_t xo_rp_low   = 0x0;
    uint32_t xo_rp_media = 0x0;
    uint32_t xo_rp_mvt   = 0x1;
    uint32_t xo_rp_svt   = 0x0;

    // Parasitic capacitance tuning
    *REG_XO_CONFIG = ((xot_clk_out_sel << 16) |
                      (xo_cap_drv      << 6)  |
                      (xo_cap_in       << 0));

    // Start XO clock
    // XO_ctrl(xo_pulse_sel, xo_delay_en, xo_drv_start_up, xo_drv_core, xo_rp_low, 
    //         xo_rp_media, xo_rp_mvt, xo_rp_svt, xo_scn_clk_sel, xo_scn_enb);
    XO_ctrl(xo_pulse_sel, xo_delay_en, 1, 0, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 0, 
            1); delay(10000); // XO_DRV_START_UP = 1
    XO_ctrl(xo_pulse_sel, xo_delay_en, 1, 0, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 1, 
            1); delay(10000); // XO_SCN_CLK_SEL  = 1
    XO_ctrl(xo_pulse_sel, xo_delay_en, 1, 0, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 0, 
            0); delay(10000); // XO_SCN_CLK_SEL  = 1; XO_SCN_ENB  = 1
    XO_ctrl(xo_pulse_sel, xo_delay_en, 0, 1, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 1,
            0); delay(10000); // XO_DRV_START_UP = 0; XO_DRV_CORE = 1; XO_SCN_CLK_SEL = 1
}

static void XOT_init(void){
	mbus_write_message32(0xA0,0x6);
	*XOT_RESET = 0x1;
	mbus_write_message32(0xA0,0x7);

	mbus_write_message32(0xA0,*REG_XOT_CONFIG);
	*REG_XOT_CONFIG = (3 << 16);
	mbus_write_message32(0xAB,*REG_XOT_CONFIG);
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
        temp_data_valid = 0;

        // Turn on SNT LDO VREF; requires ~30 ms to settle
        // TODO: Figure out delay time
        snt_ldo_vref_on();
        delay(MBUS_DELAY);

        snt_state = SNT_TEMP_LDO;

    }
    else if(snt_state == SNT_TEMP_LDO) {
        // Power on SNT LDO
        snt_ldo_power_on();

        // Power on temp sensor
        temp_sensor_power_on();
        delay(MBUS_DELAY);

        snt_state = SNT_TEMP_START;
    }
    else if(snt_state == SNT_TEMP_START) {
        // Use Timer32 as a timeout counter
        wfi_timeout_flag = 0;
        config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default
        
        // Start temp sensor
        temp_sensor_start();

        // Wait for temp sensor output or Timer32
	WFI();

        // Turn off Timer32
        *TIMER32_GO = 0;

        snt_state = SNT_TEMP_READ;
    }
    else if(snt_state == SNT_TEMP_READ) {
        if(wfi_timeout_flag) {
            // if timeout, measure again
            mbus_write_message32(0xFA, 0xFAFAFAFA);
	    snt_state = SNT_TEMP_START;
        }
        else {
            // TODO: Verify value measured
            temp_data = *REG0;
            temp_data_valid = 1;
            
            // Turn off temp sensor and ldo
            temp_sensor_power_off();
            snt_ldo_power_off();

            snt_state = SNT_IDLE;
        }
    }
}

/**********************************************
 * Flash Functions
 **********************************************/

void flp_fail(uint32_t id) {
    delay(10000);
    mbus_write_message32(0xE2, 0xDEADBEEF);
    delay(10000);
    mbus_write_message32(0xE2, id);
    delay(10000);
    mbus_write_message32(0xE2, 0xDEADBEEF);
    delay(10000);
    mbus_sleep_all();
    while(1);
}

void FLASH_init( void ) {
    // Tune Flash
    mbus_remote_register_write (FLP_ADDR, 0x26, 0x0D7788); // Program Current
    mbus_remote_register_write (FLP_ADDR, 0x27, 0x011BC8); // Erase Pump Diode Chain
    mbus_remote_register_write (FLP_ADDR, 0x01, 0x000109); // Tprog idle time
    mbus_remote_register_write (FLP_ADDR, 0x19, 0x000F03); // Voltage Clamper Tuning
    mbus_remote_register_write (FLP_ADDR, 0x0F, 0x001001); // Flash interrupt target register addr: REG0 -> REG1
    //mbus_remote_register_write (FLP_ADDR, 0x12, 0x000003); // Auto Power On/Off

}

void FLASH_turn_on() {
    set_halt_until_mbus_trx();
    mbus_remote_register_write(FLP_ADDR, 0x11, 0x00002F);
    set_halt_until_mbus_tx();

    if(*REG1 != 0xB5) { flp_fail(1); }

    flp_state = FLP_ON;
}

void FLASH_turn_off() {
    set_halt_until_mbus_trx();
    mbus_remote_register_write(FLP_ADDR, 0x11, 0x00002D);
    set_halt_until_mbus_tx();

    if(*REG1 != 0xBB) { flp_fail(2); }

    flp_state = FLP_OFF;
}

inline void FLASH_write_to_SRAM_bulk(uint32_t* remote_addr, 
                                     uint32_t* local_addr, 
                                     uint32_t length_in_words_minus_one) {
    mbus_copy_mem_from_local_to_remote_bulk(FLP_ADDR, remote_addr, local_addr, 
                                            length_in_words_minus_one);
}

inline void FLASH_read_from_SRAM_bulk(uint32_t* remote_addr,
                                      uint32_t* local_addr,
                                      uint32_t length_in_words_minus_one) {
    mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR, remote_addr, PRE_ADDR, local_addr,
                                          length_in_words_minus_one);
}

// REQUIRES: FLASH is turned on
void copy_mem_from_SRAM_to_FLASH(uint32_t SRAM_addr, 
                                 uint32_t FLASH_addr, 
                                 uint32_t length_in_words_minus_one) {
    flp_state = FLP_SRAM_TO_FLASH;

    mbus_remote_register_write(FLP_ADDR, 0x07, SRAM_addr);
    mbus_remote_register_write(FLP_ADDR, 0x08, FLASH_addr);
    delay(MBUS_DELAY);

    set_halt_until_mbus_trx();
    mbus_remote_register_write(FLP_ADDR, 0x09, (length_in_words_minus_one << 6) |
                                               (0x1 << 5) |
                                               (0x2 << 1) |
                                               (0x1));
    set_halt_until_mbus_tx();

    if(*REG1 != 0x00003F) { flp_fail(2); }
}

// REQUIRES: FLASH is turned on
void copy_mem_from_FLASH_to_SRAM(uint32_t SRAM_addr, 
                                 uint32_t FLASH_addr,
                                 uint32_t length_in_words_minus_one) {
    flp_state = FLP_FLASH_TO_SRAM;

    mbus_remote_register_write(FLP_ADDR, 0x07, SRAM_addr);
    mbus_remote_register_write(FLP_ADDR, 0x08, FLASH_addr);
    delay(MBUS_DELAY);

    set_halt_until_mbus_trx();
    mbus_remote_register_write(FLP_ADDR, 0x09, (length_in_words_minus_one << 6) |
                                               (0x1 << 5) |
                                               (0x1 << 1) |
                                               (0x1));
    set_halt_until_mbus_tx();

    if(*REG1 != 0x00002B) { flp_fail(3); }
}

// REQUIRES: FLASH is turned on
void FLASH_erase_page(uint32_t FLASH_addr) {
    flp_state = FLP_ERASE;

    mbus_remote_register_write(FLP_ADDR, 0x08, FLASH_addr & 0x7F00);

    set_halt_until_mbus_trx();
    mbus_remote_register_write(FLP_ADDR, 0x09, (0x1 << 5) |
                                               (0x4 << 1) |
                                               (0x1));
    set_halt_until_mbus_tx();

    delay(MBUS_DELAY);

    if(*REG1 != 0x00004F) { flp_fail(4); }
}

// REQUIRES: FLASH is turned on
void FLASH_erase_all() {
    uint32_t i;
    for(i = 0; i <= 0x7F; ++i) {
        FLASH_erase_page(i << 8);
	delay(10000);
    }
}

/**********************************************
 * Interrupt handlers
 **********************************************/

void handler_ext_int_wakeup     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep      (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0       (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1       (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2       (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3       (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_wakeup( void ) { // WAKEUP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP);
    // Report who woke up
    delay(MBUS_DELAY);
    mbus_write_message32(0xAA, *SREG_WAKEUP_SOURCE);

    if(*SREG_WAKEUP_SOURCE == 2) { // Wakeup timer
    	if(goc_state == GOC_TEMP_TEST) {
	    sensor_queue |= 0b010;
	}
    }
}

void handler_ext_int_gocep( void ) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
    mbus_write_message32(0xCC, 0x0);
}

void handler_ext_int_timer32( void ) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
/*
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
*/
    *TIMER32_STAT = 0x0;
    wfi_timeout_flag = 1;
    mbus_write_message32(0xDD, 0x0);
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
 * End of program sleep operation
 **********************************************/

static void operation_sleep( void ) {
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;

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
    operation_sleep();
}

// TEMP WAKEUP TIMER FUNCTION
void set_wakeup_timer_prev17 ( uint32_t timestamp, uint8_t irq_en, uint8_t reset ){
	uint32_t regval = timestamp;
	if( irq_en ) regval |= 0x030000; // IRQ in Sleep-Only
	else		 regval &= 0xFCFFFF;
    *REG_WUPT_CONFIG = regval;

	if( reset ) *WUPT_RESET = 0x01;
}

/**********************************************
 * End of program sleep operation
 **********************************************/

static void operation_init( void ) {
    // BREAKPOINT 0x01
    mbus_write_message32(0xBA, 0x01);

    *TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
    *REG_MBUS_WD = 0; // Disables Mbus watchdog timer

    /*
    // Set CPU and Mbus clock speeds
    // Under default settings Mbus clock speed = 192.5kHz, core clock speed is 96.3kHz
    // TODO: Verify this
    prev18_r0B.CLK_GEN_HIGH_FREQ    = 0x0; // Default 0x0, 1bit
    prev18_r0B.CLK_GEN_RING         = 0x1; // Default 0x1, 2bits
    prev18_r0B.CLK_GEN_DIV_MBC      = 0x2; // Default 0x2, 3bits
    prev18_r0B.CLK_GEN_DIV_CORE     = 0x3; // Default 0x3, 3bits
    // GOC_CLK_GEN_SEL_DIV = 0x0, GOC_CLK_GEN_SEL_FREQ = 0x6 gives GOC clock speed = 11550Hz
    prev18_r0B.GOC_CLK_GEN_SEL_DIV  = 0x0; // Default 0x1, 2bits
    prev18_r0B.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x7, 3bits
    *REG_CLKGEN_TUNE = prev18_r0B.as_int;

    // FIXME: This adds a delay to SRAM reads for debugging purpose, which improves the
    // robustness of SRAM reads at the cost of speed and power. Possibly remove this 
    // after testing
    prev18_r1C.SRAM0_TUNE_ASO_DLY     = 0x1F; // Default 0x00, 5bits
    prev18_r1C.SRAM0_TUNE_DECODER_DLY = 0xF;  // Default 0x2, 4bits
    prev18_r1C.SRAM0_USE_INVERTER_SA  = 0x0;  // Default 0x0, 1bit
    */

    // Enumeration
    enumerated = ENUMID;
    mbus_enumerate(SNT_ADDR);
    mbus_enumerate(FLP_ADDR);
    delay(MBUS_DELAY);

    // Default CPU halt function
    set_halt_until_mbus_tx();

    // Global variables
    wakeup_data = 0;
    wfi_timeout_flag = 0;
    mbc_state = MBC_IDLE;
    goc_state = GOC_IDLE;
    snt_state = SNT_IDLE;
    flp_state = FLP_OFF;

    temp_data = 0;
    temp_data_valid = 0;

    sensor_queue = 0;

    // BREAKPOINT 0x02
    mbus_write_message32(0xBA, 0x02);


    // Initialization

    // Set CPU & Mbus Clock Speeds
    prev17_r0D.SRAM_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
    prev17_r0D.SRAM_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
    prev17_r0D.SRAM_USE_INVERTER_SA= 1; 
    *REG_SRAM_TUNE = prev17_r0D.as_int;

    FLASH_init();
    sntv4_r01.TSNS_BURST_MODE = 0;
    sntv4_r01.TSNS_CONT_MODE  = 0;
    mbus_remote_register_write(SNT_ADDR, 1, sntv4_r01.as_int);

    sntv4_r07.TSNS_INT_RPLY_SHORT_ADDR = 0x10;
    sntv4_r07.TSNS_INT_RPLY_REG_ADDR   = 0x00;
    mbus_remote_register_write(SNT_ADDR, 7, sntv4_r07.as_int);
    
    XO_init();
    XOT_init();
}

/**********************************************
 * MAIN function starts here
 **********************************************/

int main() {
    // Only enable relevant interrupts (PREv18)
    *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_TIMER32 | 
		  1 << IRQ_REG0 | 1 << IRQ_REG1 | 1 << IRQ_REG2 | 1 << IRQ_REG3);

    // BREAKPOINT 0x00
    mbus_write_message32(0xBA, 0x00);

    // Initialization
    if(enumerated != ENUMID) {
        operation_init();
        
        // BREAKPOINT 0x01
        mbus_write_message32(0xBA, 0x01);

	operation_sleep_notimer();
    }

    // FLASH_turn_on();
    
    // check if wakeup is due to GOC
    if((*SREG_WAKEUP_SOURCE) & 1) {

    wakeup_data = *GOC_DATA_IRQ;
    mbus_write_message32(0xAD, wakeup_data);
    wakeup_data_header = (wakeup_data >> 24) & 0xFF;
    // uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;
    // uint32_t wakeup_data_field_1 = (wakeup_data >> 8) & 0xFF;
    // uint32_t wakeup_data_field_2 = (wakeup_data >> 16) & 0xFF;
    if(wakeup_data_header == 0x01) {
        if(snt_state != SNT_IDLE) { return; }

        // For testing
        // Take a manual temp measurement
        do {
            operation_temp_run();
        } while(!temp_data_valid);

        mbus_write_message32(0xAB, temp_data);
    }
    else if(wakeup_data_header == 0x02) {
        if(flp_state != FLP_OFF) { return; }

        // Erase flash
        FLASH_turn_on();
        FLASH_erase_all();
        FLASH_turn_off();
    }
    else if(wakeup_data_header == 0x03) {
        // Store 1 32-bit word in flash
        // Needs 3 GOCEP interrupts
	mbus_write_message32(0xED, goc_state);
        if(goc_state == GOC_IDLE) {
            flash_addr = wakeup_data & 0x7FFF;
            goc_state = GOC_FLASH_WRITE1;
        }
        else if(goc_state == GOC_FLASH_WRITE1) {
            flash_data = (wakeup_data & 0xFFFF) << 16;
            goc_state = GOC_FLASH_WRITE2;
        }
        else if(goc_state == GOC_FLASH_WRITE2) {
            flash_data |= (wakeup_data & 0xFFFF);
            uint32_t temp_arr[1] = { flash_data };

            if(flp_state == FLP_OFF) {
                FLASH_turn_on();
                FLASH_write_to_SRAM_bulk((uint32_t*) 0x000, temp_arr, 0);
                FLASH_read_from_SRAM_bulk((uint32_t*) 0x000, temp_arr, 0);
                copy_mem_from_SRAM_to_FLASH(0x000, flash_addr, 0);
		delay(10000);
		copy_mem_from_FLASH_to_SRAM(0x000, flash_addr, 0);
                FLASH_read_from_SRAM_bulk((uint32_t*) 0x000, temp_arr, 0);
                FLASH_turn_off();

		mbus_write_message32(0xF1, *temp_arr);
		delay(MBUS_DELAY);
		mbus_write_message32(0xF1, flash_addr);
		delay(MBUS_DELAY);
            }

            goc_state = GOC_IDLE;
        }
    }
    else if(wakeup_data_header == 0x04) {
        // Read an array from flash
        // Needs 2 GOCEP interrupts
	mbus_write_message32(0xED, goc_state);
        if(goc_state == GOC_IDLE) {
            flash_addr = wakeup_data & 0x7FFF;
            goc_state = GOC_FLASH_READ1;
        }
        else if(goc_state == GOC_FLASH_READ1) {
            uint32_t len_min_one = wakeup_data & 0xFF;
            uint32_t temp_arr[256];
            
            if(flp_state == FLP_OFF) {
                FLASH_turn_on();
                copy_mem_from_FLASH_to_SRAM(0x000, flash_addr, 
                        		    len_min_one);
		FLASH_read_from_SRAM_bulk((uint32_t*) 0x000, temp_arr,
					  len_min_one);
                FLASH_turn_off();
            }
            uint32_t i;
            for(i = 0; i <= len_min_one; ++i) {
                mbus_write_message32(0xF1, temp_arr[i]);
            }

            goc_state = GOC_IDLE;
        }
    }
    else if(wakeup_data_header == 0x05) {
        // Take a temp measurement every 5 secs
        // Store data into flash after 10 measurements
        if(goc_state == GOC_IDLE) {
	    goc_temp_test_count = 0;
	    goc_state = GOC_TEMP_TEST;
	    //set_wakeup_timer_prev17(10, 1, 1);
	    set_xo_timer(32900, 1, 1);
	    operation_sleep();
        }
    }
    // ERASE BEFORE REWRITE
    else if(wakeup_data_header == 0x06) {
    	if(flp_state == FLP_OFF) {
	    flash_addr = wakeup_data & 0x7FFF;
	    FLASH_turn_on();
	    FLASH_erase_page(flash_addr);
	    FLASH_turn_off();
	}
    }

    mbus_write_message32(0xED, goc_state);

    }

    // testing
    set_xo_timer(0x8888, 1, 1);
    //mbus_write_message32(0xA0, *REG_XOT_CONFIG);
    //*REG_XOT_CONFIG = 0x38888;
    //mbus_write_message32(0xAC, *REG_XOT_CONFIG);
    //mbus_write_message32(0xAC, *REG_XOT_CONFIGU);
    // set_wakeup_timer_prev17(10, 1, 1);
    // timer32_config(1000, 1, 0, 0);
    //mbus_write_message32(0xBC, *REG_XOT_VAL_L);
    // delay(1000);
    //mbus_write_message32(0xBC, *REG_XOT_VAL_U);
    //mbus_write_message32(0xBC, *XOT_VAL);
    //*XOT_RESET = 0x01;
    //mbus_write_message32(0xBC, *REG_XOT_VAL_L);
    //mbus_write_message32(0xBC, *REG_XOT_VAL_U);
    //mbus_write_message32(0xBC, *XOT_VAL);
    //operation_sleep();
    while(1);

    mbc_state = MBC_READY;

    // Finite state machine
    while(1) {

    mbus_write_message32(0xED, mbc_state);
    
    if(mbc_state == MBC_READY) {
        if(sensor_queue & 0b001) {
            // LNT
            sensor_queue &= 0b110;
	    continue;
        }
        else if(sensor_queue & 0b010) {
            mbc_state = MBC_TEMP_READ;
            sensor_queue &= 0b101;
	    continue;
        }
        else if(sensor_queue & 0b100) {
            // RDC
            sensor_queue &= 0b011;
	    continue;
        }
	mbc_state = MBC_IDLE;
    }
    else if(mbc_state == MBC_TEMP_READ) {
        do {
            operation_temp_run();
        } while(!temp_data_valid);

        mbus_write_message32(0xCC, temp_data);

	if(goc_state == GOC_TEMP_TEST) {
	    mbus_write_message32(0xBC, goc_temp_test_count);
	    goc_temp_arr[goc_temp_test_count] = temp_data;
	    ++goc_temp_test_count;

	    if(goc_temp_test_count < 6) {
		// set_wakeup_timer() NOT WORKING
	        // set_wakeup_timer_prev17(10, 1, 1);
		set_xo_timer(32900, 1, 1);
	    }
	    else {
		uint32_t i;
		for(i = 0; i < 6; i++) {
			mbus_write_message32(0xEE, goc_temp_arr[i]);
		}
		FLASH_write_to_SRAM_bulk((uint32_t*) 0x000, goc_temp_arr, 5);
	    	FLASH_turn_on();
		copy_mem_from_SRAM_to_FLASH(0x000, 0x600, 5);
		delay(10000);
		FLASH_read_from_SRAM_bulk((uint32_t*) 0x000, goc_temp_arr, 5);
		delay(10000);
		copy_mem_from_FLASH_to_SRAM(0x000, 0x600, 5);
		delay(10000);
		FLASH_read_from_SRAM_bulk((uint32_t*) 0x000, goc_temp_arr, 5);
		delay(10000);
		FLASH_turn_off();
		goc_state = GOC_IDLE;

	    }
	}

	mbc_state = MBC_READY;
    }
    else if(mbc_state == MBC_IDLE) {
        // Go to sleep and wait for next wakeup
        mbus_write_message32(0xAA, 0XAAAAAAAA);
        operation_sleep();
    }
    else {
        // Should not get here
        // State not defined; go to sleep
        mbus_write_message32(0xBA, 0XDEADBEEF);
        operation_sleep_notimer();
    }

    }

    // Should not get here
    operation_sleep_notimer();

    while(1);
}

