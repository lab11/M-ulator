//*******************************************************************
//Author: Sechang Oh
//Description: Leidos Trestle Gen1 
//          Modified from 'Leidos_dorado_Gen3'
//          v1.0: created   12/14/2018
//*******************************************************************
#include "PREv18.h"
#include "ADOv5V_RF.h"
#include "PMUv7_RF.h"
#include "mbus.h"
 
#define ENUMID 0xDEADBEEF

#include "DFT_LUT.txt"
#include "DCT_PHS.txt"

// uncomment this for debug mbus message
// #define DEBUG_MBUS_MSG
// #define DEBUG_MBUS_MSG_1

#define ADO_ADDR 0x4
#define FLP_ADDR 0x5
#define PMU_ADDR 0x6
#define WAKEUP_PERIOD_PARKING 30000 // About 2 hours (PRCv17)

// System parameters
#define MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)

// CP parameter
#define CP_DELAY 50000 // Amount of delay between successive messages; 100: 6-7ms

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

// GPIO pins
#define GPIO0_IRQ  0
#define GPIO_NNET_IDX0  1
#define GPIO_NNET_IDX1  2
#define GPIO_NNET_IDX2  3
#define GPIO_NNET_IDX3  4
#define GPIO_NNET_IDX4  5

#define EP_MODE     ((volatile uint32_t *) 0xA0000028)

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t irq_history;
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint32_t wakeup_period_count;
volatile uint32_t wakeup_timer_multiplier;
volatile uint32_t PMU_ADC_3P0_VAL;
volatile uint32_t pmu_parkinglot_mode;
volatile uint32_t pmu_sar_conv_ratio_val_test_on;
volatile uint32_t pmu_sar_conv_ratio_val_test_off;
volatile uint32_t read_data_batadc;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile prev18_r0B_t prev18_r0B = PREv18_R0B_DEFAULT;
volatile prev18_r19_t prev18_r19 = PREv18_R19_DEFAULT;
volatile prev18_r1A_t prev18_r1A = PREv18_R1A_DEFAULT;
volatile prev18_r1C_t prev18_r1C = PREv18_R1C_DEFAULT;

volatile adov5v_r00_t adov5v_r00 = ADOv5V_R00_DEFAULT;
volatile adov5v_r04_t adov5v_r04 = ADOv5V_R04_DEFAULT;
volatile adov5v_r07_t adov5v_r07 = ADOv5V_R07_DEFAULT;
volatile adov5v_r0B_t adov5v_r0B = ADOv5V_R0B_DEFAULT;
volatile adov5v_r0D_t adov5v_r0D = ADOv5V_R0D_DEFAULT;
volatile adov5v_r0E_t adov5v_r0E = ADOv5V_R0E_DEFAULT;
volatile adov5v_r0F_t adov5v_r0F = ADOv5V_R0F_DEFAULT;
volatile adov5v_r10_t adov5v_r10 = ADOv5V_R10_DEFAULT;
volatile adov5v_r11_t adov5v_r11 = ADOv5V_R11_DEFAULT;
volatile adov5v_r12_t adov5v_r12 = ADOv5V_R12_DEFAULT;
volatile adov5v_r13_t adov5v_r13 = ADOv5V_R13_DEFAULT;
volatile adov5v_r14_t adov5v_r14 = ADOv5V_R14_DEFAULT;
volatile adov5v_r15_t adov5v_r15 = ADOv5V_R15_DEFAULT;
volatile adov5v_r16_t adov5v_r16 = ADOv5V_R16_DEFAULT;
volatile adov5v_r17_t adov5v_r17 = ADOv5V_R17_DEFAULT;
volatile adov5v_r18_t adov5v_r18 = ADOv5V_R18_DEFAULT;
volatile adov5v_r19_t adov5v_r19 = ADOv5V_R19_DEFAULT;
volatile adov5v_r1A_t adov5v_r1A = ADOv5V_R1A_DEFAULT;
volatile adov5v_r1B_t adov5v_r1B = ADOv5V_R1B_DEFAULT;
volatile adov5v_r1C_t adov5v_r1C = ADOv5V_R1C_DEFAULT;
        
uint32_t read_data[100];


static void operation_sleep_notimer(void);
static void pmu_set_sar_override(uint32_t val);
static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);
static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);

//*******************************************************************
// XO Functions
//*******************************************************************

static void XO_init(void) {
    // Parasitic Capacitance Tuning (6-bit for each; Each 1 adds 1.8pF)
    uint32_t xo_cap_drv = 0x3F; // Additional Cap on OSC_DRV
    uint32_t xo_cap_in  = 0x3F; // Additional Cap on OSC_IN
    prev18_r1A.XO_CAP_TUNE = (
            (xo_cap_drv <<6) | 
            (xo_cap_in <<0));   // XO_CLK Output Pad 
    *REG_XO_CONF2 = prev18_r1A.as_int;

    // XO configuration
    prev18_r19.XO_EN_DIV    = 0x1;// divider enable
    prev18_r19.XO_S         = 0x1;// division ratio for 16kHz out
    prev18_r19.XO_SEL_CP_DIV= 0x0;// 1: 0.3V-generation charge-pump uses divided clock
    prev18_r19.XO_EN_OUT    = 0x1;// XO ouput enable
    prev18_r19.XO_PULSE_SEL = 0x4;// pulse width sel, 1-hot code
    prev18_r19.XO_DELAY_EN  = 0x3;// pair usage together with xo_pulse_sel
    // Pseudo-Resistor Selection
    prev18_r19.XO_RP_LOW    = 0x0;
    prev18_r19.XO_RP_MEDIA  = 0x1;
    prev18_r19.XO_RP_MVT    = 0x0;
    prev18_r19.XO_RP_SVT    = 0x0;
 
    prev18_r19.XO_SLEEP = 0x0;
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(100);
    prev18_r19.XO_ISOLATE = 0x0;
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(100);
    prev18_r19.XO_DRV_START_UP  = 0x1;// 1: enables start-up circuit
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(2000);
    //prev18_r19.XO_SCN_CLK_SEL   = 0x1;// scn clock 1: normal. 0.3V level up to 0.6V, 0:init
    //*REG_XO_CONF1 = prev18_r19.as_int;
    //delay(2000);
    //prev18_r19.XO_SCN_CLK_SEL   = 0x0;
    //prev18_r19.XO_SCN_ENB       = 0x0;// enable_bar of scn
    //*REG_XO_CONF1 = prev18_r19.as_int;
    //delay(2000);
    //prev18_r19.XO_DRV_START_UP  = 0x0;
    //prev18_r19.XO_DRV_CORE      = 0x1;// 1: enables core circuit
    //prev18_r19.XO_SCN_CLK_SEL   = 0x1;
    //*REG_XO_CONF1 = prev18_r19.as_int;

    enable_xo_timer();
    start_xo_cout();
    
    mbus_write_message32(0xE9,0x40D02E); 
}


//************************************
// GPIO Functions
// ***********************************
static void init_gpio(void){
// GPIO in/out direction, input:0, output:1
//    gpio_init( (0 << GPIO0_IRQ) | 
//               (1 << GPIO_NNET_IDX0) |
//               (1 << GPIO_NNET_IDX1) |
//               (1 << GPIO_NNET_IDX2) |
//               (1 << GPIO_NNET_IDX3));      //GPIO initialization, all using pins should be here
//    gpio_set_irq_mask (0x00000000);
    gpio_set_dir (0<<0);
//    gpio_set_data (0x00000000);
    set_gpio_pad (1<<0);
//    unfreeze_gpio_out();
}



//************************************
// PMU Related Functions
//************************************
static void pmu_set_sar_override(uint32_t val){
    // SAR_RATIO_OVERRIDE
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
            ( (0 << 13) // Enables override setting [12] (1'b1)
              | (0 << 12) // Let VDD_CLK always connected to vbat
              | (1 << 11) // Enable override setting [10] (1'h0)
              | (0 << 10) // Have the converter have the periodic reset (1'h0)
              | (1 << 9) // Enable override setting [8] (1'h0)
              | (0 << 8) // Switch input / output power rails for upconversion (1'h0)
              | (1 << 7) // Enable override setting [6:0] (1'h0)
              | (val)       // Binary converter's conversion ratio (7'h00)
            ));
    delay(MBUS_DELAY*2);
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
            ( (1 << 13) // Enables override setting [12] (1'b1)
              | (0 << 12) // Let VDD_CLK always connected to vbat
              | (1 << 11) // Enable override setting [10] (1'h0)
              | (0 << 10) // Have the converter have the periodic reset (1'h0)
              | (1 << 9) // Enable override setting [8] (1'h0)
              | (0 << 8) // Switch input / output power rails for upconversion (1'h0)
              | (1 << 7) // Enable override setting [6:0] (1'h0)
              | (val)       // Binary converter's conversion ratio (7'h00)
            ));
    delay(MBUS_DELAY*2);
}


inline static void pmu_set_adc_period(uint32_t val){
    // PMU_CONTROLLER_DESIRED_STATE Active
    mbus_remote_register_write(PMU_ADDR,0x3C,
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
             | (1 << 19) //state_state_horizon
            ));
    delay(MBUS_DELAY*10);

    // Register 0x36: TICK_REPEAT_VBAT_ADJUST
    mbus_remote_register_write(PMU_ADDR,0x36,val); 
    delay(MBUS_DELAY*10);

    // Register 0x33: TICK_ADC_RESET
    mbus_remote_register_write(PMU_ADDR,0x33,2);
    delay(MBUS_DELAY);

    // Register 0x34: TICK_ADC_CLK
    mbus_remote_register_write(PMU_ADDR,0x34,2);
    delay(MBUS_DELAY);

    // PMU_CONTROLLER_DESIRED_STATE Active
    mbus_remote_register_write(PMU_ADDR,0x3C,
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
             | (1 << 19) //state_state_horizon
            ));
    delay(MBUS_DELAY);
}


inline static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

    // The first register write to PMU needs to be repeated
    // Register 0x16: V1P2 Active
    mbus_remote_register_write(PMU_ADDR,0x16, 
            ( (0 << 19) // Enable PFM even during periodic reset
              | (0 << 18) // Enable PFM even when Vref is not used as ref
              | (0 << 17) // Enable PFM
              | (3 << 14) // Comparator clock division ratio
              | (0 << 13) // Enable main feedback loop
              | (r << 9)  // Frequency multiplier R
              | (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
              | (base)      // Floor frequency base (0-63)
            ));
    delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x16, 
            ( (0 << 19) // Enable PFM even during periodic reset
              | (0 << 18) // Enable PFM even when Vref is not used as ref
              | (0 << 17) // Enable PFM
              | (3 << 14) // Comparator clock division ratio
              | (0 << 13) // Enable main feedback loop
              | (r << 9)  // Frequency multiplier R
              | (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
              | (base)      // Floor frequency base (0-63)
            ));
    delay(MBUS_DELAY);
    // Register 0x18: V3P6 Active 
    mbus_remote_register_write(PMU_ADDR,0x18, 
            ( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
              | (0 << 13) // Enable main feedback loop
              | (r << 9)  // Frequency multiplier R
              | (l << 5)  // Frequency multiplier L (actually L+1)
              | (base)      // Floor frequency base (0-63)
            ));
    delay(MBUS_DELAY);
    // Register 0x1A: V0P6 Active
    mbus_remote_register_write(PMU_ADDR,0x1A,
            ( (0 << 13) // Enable main feedback loop
              | (r << 9)  // Frequency multiplier R
              | (l << 5)  // Frequency multiplier L (actually L+1)
              | (base)      // Floor frequency base (0-63)
            ));
    delay(MBUS_DELAY);

}

inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

    // Register 0x17: V3P6 Sleep
    /*
    mbus_remote_register_write(PMU_ADDR,0x17, 
            ( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
              | (0 << 13) // Enable main feedback loop
              | (r << 9)  // Frequency multiplier R
              | (l << 5)  // Frequency multiplier L (actually L+1)
              | (base)      // Floor frequency base (0-63)
            ));
    */
    mbus_remote_register_write(PMU_ADDR,0x17, 
            ( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
              | (0 << 13) // Enable main feedback loop
              | (1 << 9)  // Frequency multiplier R
              | (0 << 5)  // Frequency multiplier L (actually L+1)
              | (8)         // Floor frequency base (0-63)
            ));
    delay(MBUS_DELAY);
    // Register 0x15: V1P2 Sleep
    mbus_remote_register_write(PMU_ADDR,0x15, 
            ( (0 << 19) // Enable PFM even during periodic reset
              | (0 << 18) // Enable PFM even when Vref is not used as ref
              | (0 << 17) // Enable PFM
              | (3 << 14) // Comparator clock division ratio
              | (0 << 13) // Enable main feedback loop
              | (r << 9)  // Frequency multiplier R
              | (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
              | (base)      // Floor frequency base (0-63)
            ));
    delay(MBUS_DELAY);
    // Register 0x19: V0P6 Sleep
    mbus_remote_register_write(PMU_ADDR,0x19,
            ( (0 << 13) // Enable main feedback loop
              | (r << 9)  // Frequency multiplier R
              | (l << 5)  // Frequency multiplier L (actually L+1)
              | (base)      // Floor frequency base (0-63)
            ));
    delay(MBUS_DELAY);
}

inline static void pmu_set_clk_init(){
    pmu_set_active_clk(0xA,0x1,0x10,0x2);
    //  pmu_set_active_clk(0xA,0xA,0x1F,0x2);
    pmu_set_sleep_clk(0xA,0x1,0x10,0x2); //with TEST1P2
    //  pmu_set_sleep_clk(0x1,0x0,0x4,0x2); // without TEST1P2
    //  pmu_set_sleep_clk(0xF,0x0,0x1,0x1);
    // SAR_RATIO_OVERRIDE
    // Use the new reset scheme in PMUv3
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
            ( (0 << 13) // Enables override setting [12] (1'b1)
              | (0 << 12) // Let VDD_CLK always connected to vbat
              | (1 << 11) // Enable override setting [10] (1'h0)
              | (0 << 10) // Have the converter have the periodic reset (1'h0)
              | (0 << 9) // Enable override setting [8] (1'h0)
              | (0 << 8) // Switch input / output power rails for upconversion (1'h0)
              | (0 << 7) // Enable override setting [6:0] (1'h0)
              | (0x45)      // Binary converter's conversion ratio (7'h00)
            ));
    delay(MBUS_DELAY);
        pmu_set_sar_override(pmu_sar_conv_ratio_val_test_on);
    //  pmu_set_sar_override(0x4D);
    //  pmu_set_sar_override(0x2D);
    //  pmu_set_sar_override(0x29);

    pmu_set_adc_period(1); // 0x100 about 1 min for 1/2/1 1P2 setting
}

inline static void pmu_adc_reset_setting(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Active
    mbus_remote_register_write(PMU_ADDR,0x3C,
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
             | (1 << 19) //state_state_horizon
            ));
    delay(MBUS_DELAY);
}


inline static void pmu_adc_disable(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Sleep
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
             | (0 << 13) //state_sar_scn_ratio_adjusted
             | (1 << 14) //state_downconverter_on
             | (1 << 15) //state_downconverter_stabilized
             | (1 << 16) //state_vdd_3p6_turned_on
             | (1 << 17) //state_vdd_1p2_turned_on
             | (1 << 18) //state_vdd_0P6_turned_on
             | (1 << 19) //state_state_horizon
            ));
    delay(MBUS_DELAY);
}

inline static void pmu_adc_enable(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Sleep
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
             | (0 << 12) //state_adc_adjusted // Turning off offset cancellation
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

inline static void pmu_adc_read_latest(){

    // Grab latest PMU ADC readings
    // PMU register read is handled differently
    mbus_remote_register_write(PMU_ADDR,0x00,0x03);
    delay(MBUS_DELAY);
    read_data_batadc = *((volatile uint32_t *) REG0) & 0xFF;

}

inline static void pmu_reset_solar_short(){
    mbus_remote_register_write(PMU_ADDR,0x0E, 
            ( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
              | (1 << 9)  // Enables override setting [8]
              | (0 << 8)  // Turn on the harvester-inhibiting switch
              | (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
              | (0)         // clamp_tune_top (decreases clamp thresh)
            ));
    delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x0E, 
            ( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
              | (0 << 9)  // Enables override setting [8]
              | (0 << 8)  // Turn on the harvester-inhibiting switch
              | (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
              | (0)         // clamp_tune_top (decreases clamp thresh)
            ));
    delay(MBUS_DELAY);
}

//***************************************************
//  FLP functions
//***************************************************
void flp_fail(uint32_t id)
{
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

void FLASH_initialization (void) {
    
    // Tune Flash
    mbus_remote_register_write (FLP_ADDR , 0x26 , 0x0D7788); // Program Current
    mbus_remote_register_write (FLP_ADDR , 0x27 , 0x011BC8); // Erase Pump Diode Chain
    mbus_remote_register_write (FLP_ADDR , 0x01 , 0x000109); // Tprog idle time
    mbus_remote_register_write (FLP_ADDR , 0x19 , 0x000F03); // Voltage Clamper Tuning
    mbus_remote_register_write (FLP_ADDR , 0x0F , 0x001001); // Flash interrupt target register addr: REG0 -> REG1
    //mbus_remote_register_write (FLP_ADDR , 0x12 , 0x000003); // Auto Power On/Off

}

void FLASH_turn_on()
{
    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP_ADDR , 0x11 , 0x00002F);
    set_halt_until_mbus_tx();

    if (*REG1 != 0xB5) flp_fail (0);
}

void FLASH_turn_off()
{
    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP_ADDR , 0x11 , 0x00002D);
    set_halt_until_mbus_tx();

    if (*REG1 != 0xBB) flp_fail (1);
}

void FLASH_pp_ready (void) {

        // Erase Flash
        uint32_t page_start_addr = 0;
        uint32_t idx;

        for (idx=0; idx<512; idx++){ // All Pages (8Mb) are erased.
            page_start_addr = (idx << 8);

            mbus_remote_register_write (FLP_ADDR, 0x08, page_start_addr); // Set FLSH_START_ADDR

            set_halt_until_mbus_rx();
            mbus_remote_register_write (FLP_ADDR, 0x09, 0x000029); // Page Erase
            set_halt_until_mbus_tx();

            if (*REG1 != 0x00004F) flp_fail((0xFFFF << 16) | idx);
        }
    
    // Ping Pong Setting
        mbus_remote_register_write (FLP_ADDR, 0x13 , 0x000001); // Enable Ping Pong w/ No length limit
        mbus_remote_register_write (FLP_ADDR, 0x14 , 0x00000B); // Over run err no detection & fast programming & no wrapping & DATA0/DATA1 enable
        mbus_remote_register_write (FLP_ADDR, 0x15 , 0x000000); // Flash program addr started from 0x0

}

void FLASH_pp_on (void) {
    mbus_remote_register_write (FLP_ADDR, 0x09, 0x00002D); // Ping Pong Go
}

void FLASH_pp_off (void) {
    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP_ADDR, 0x13, 0x000000); // Ping Pong End
    set_halt_until_mbus_tx();
}

void FLASH_read (void) {
    uint32_t i,j;
    uint32_t flp_sram_addr;
    flp_sram_addr = 0;
    FLASH_turn_on();

    // Flash Read
    mbus_remote_register_write (FLP_ADDR, 0x08 , 0x0); // Set FLSH_START_ADDR
    mbus_remote_register_write (FLP_ADDR, 0x07 , 0x0); // Set SRAM_START_ADDR

    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP_ADDR, 0x09 , 0x02EE23); // Flash -> SRAM
    set_halt_until_mbus_tx();

    if (*REG1 != 0x00002B) flp_fail(4);

    for(j=0; j<16; j++){
        set_halt_until_mbus_rx();
        mbus_copy_mem_from_remote_to_any_bulk (FLP_ADDR, (uint32_t *) flp_sram_addr, 0x1, read_data, 99);
        set_halt_until_mbus_tx();
        for (i=0; i<100; i++) {
            //delay(100);
            mbus_write_message32(0xC0, read_data[i]);
            //delay(100);
        }
        flp_sram_addr = flp_sram_addr + 400;
    }

    // Turn off Flash
    FLASH_turn_off();
}

//***************************************************
//  ADO functions
//***************************************************
inline static void ado_initialization(void){
    // as_int of all zero regs initalization
    adov5v_r07.as_int = 0;
    adov5v_r0B.as_int = 0;
    adov5v_r10.as_int = 0;

    adov5v_r10.VAD_ADC_DOUT_ISOL = 1;
    mbus_remote_register_write(ADO_ADDR, 0x10, adov5v_r10.as_int);
 
    // AFE Initialization
    adov5v_r15.IB_GEN_CLK_EN = 1;
    adov5v_r15.LS_CLK_EN_1P2 = 1;
    adov5v_r15.REC_PGA_BWCON = 24;//24;
    adov5v_r15.REC_PGA_CFBADD = 1;//0;
    adov5v_r15.REC_PGA_GCON = 8;//2;
    mbus_remote_register_write(ADO_ADDR, 0x15, adov5v_r15.as_int); //1F8622

    adov5v_r16.IBC_REC_LNA = 6;
    adov5v_r16.IBC_REC_PGA = 11;
    adov5v_r16.IBC_VAD_LNA = 7;
    adov5v_r16.IBC_VAD_PGA = 5;
    mbus_remote_register_write(ADO_ADDR, 0x16, adov5v_r16.as_int);//0535BD
 
    adov5v_r18.REC_LNA_N1_CON = 2;
    adov5v_r18.REC_LNA_N2_CON = 2;
    adov5v_r18.REC_LNA_P1_CON = 3;
    adov5v_r18.REC_LNA_P2_CON = 3;
    mbus_remote_register_write(ADO_ADDR, 0x18, adov5v_r18.as_int);//0820C3

    adov5v_r19.REC_PGA_P1_CON = 3;
    mbus_remote_register_write(ADO_ADDR, 0x19, adov5v_r19.as_int);//007064

    adov5v_r1B.VAD_LNA_N1_LCON = 5;
    adov5v_r1B.VAD_LNA_N2_LCON = 6;
    adov5v_r1B.VAD_LNA_P1_CON = 3;
    adov5v_r1B.VAD_LNA_P2_CON = 2;
    mbus_remote_register_write(ADO_ADDR, 0x1B, adov5v_r1B.as_int);//0A6062

    adov5v_r1C.VAD_PGA_N1_LCON = 0;
    adov5v_r1C.VAD_PGA_N1_MCON = 1;
    mbus_remote_register_write(ADO_ADDR, 0x1C, adov5v_r1C.as_int);//00079C

    adov5v_r11.LDO_CTRL_VREFLDO_VOUT_1P4HP = 4;
    mbus_remote_register_write(ADO_ADDR, 0x11, adov5v_r11.as_int);//84CAC9

    adov5v_r12.LDO_CTRL_VREFLDO_VOUT_0P6LP = 3;
    adov5v_r12.LDO_CTRL_VREFLDO_VOUT_0P9HP = 3;
    adov5v_r12.LDO_CTRL_VREFLDO_VOUT_1P4LP = 4;
    mbus_remote_register_write(ADO_ADDR, 0x12, adov5v_r12.as_int);//828283
    
    /////////////////
    //DSP LP_TOP
    ///////////////////
    //PHS_FE Programming
    mbus_remote_register_write(ADO_ADDR, 0x05, 0x000004); //PHS_DATA_FE
    mbus_remote_register_write(ADO_ADDR, 0x06, 0x000001); //PHS_WR_FE high
    mbus_remote_register_write(ADO_ADDR, 0x06, 0x000000); //PHS_WR_FE low 
    //SD_TH Programming
    mbus_remote_register_write(ADO_ADDR, 0x09, 0x000064); //TH_DATA
    mbus_remote_register_write(ADO_ADDR, 0x0A, 0x000001); //TH_WR high
    mbus_remote_register_write(ADO_ADDR, 0x0A, 0x000000); //TH_WR low
    //LUT Programming
    uint8_t i;
    for(i= 0; i<LUT_DATA_LENGTH; i++){
        adov5v_r07.DSP_LUT_WR_ADDR = i;
        adov5v_r07.DSP_LUT_DATA_IN = LUT_DATA[i];
        mbus_remote_register_write(ADO_ADDR, 0x07, adov5v_r07.as_int);
        mbus_remote_register_write(ADO_ADDR, 0x08, 0x000001);
        mbus_remote_register_write(ADO_ADDR, 0x08, 0x000000);
    }
    //PHS Programming 
    for(i= 0; i<PHS_DATA_LENGTH; i++){
        adov5v_r0B.DSP_PHS_WR_ADDR_FS = i;
        adov5v_r0B.DSP_PHS_DATA_IN_FS = PHS_DATA[i];
        mbus_remote_register_write(ADO_ADDR, 0x0B, adov5v_r0B.as_int);
        mbus_remote_register_write(ADO_ADDR, 0x0C, 0x000001);
        mbus_remote_register_write(ADO_ADDR, 0x0C, 0x000000);
    }
    
    //N_DCT: 128 points DCT 
    adov5v_r00.DSP_N_DCT = 2;
    mbus_remote_register_write(ADO_ADDR, 0x00, adov5v_r00.as_int);
    
    //WAKEUP REQ EN
    adov5v_r0E.DSP_WAKEUP_REQ_EN = 1;
    mbus_remote_register_write(ADO_ADDR, 0x0E, adov5v_r0E.as_int);
}


inline static void digital_set_mode(uint8_t mode){
    if(mode == 1){      //LP DSP
        adov5v_r0D.DSP_HP_ADO_GO = 0;
        adov5v_r0D.DSP_HP_DNN_GO = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BCB5
        adov5v_r0D.DSP_HP_RESETN = 0;
        adov5v_r0D.DSP_DNN_HP_MODE_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC15
        
        adov5v_r0D.DSP_DNN_RESETN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC05
        adov5v_r0D.DSP_DNN_CLKENB_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC0D
        adov5v_r0D.DSP_DNN_ISOLATEN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC09
        adov5v_r0D.DSP_DNN_PG_SLEEP_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC0B
        adov5v_r0D.DSP_DNN_CTRL_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC0A
        
        adov5v_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov5v_r04.as_int);//E00040
        adov5v_r04.DSP_P2S_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov5v_r04.as_int);//E00040
        
        adov5v_r0D.DSP_LP_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC4A
        }
    else if(mode == 2){ // HP DSP
        adov5v_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80A

        adov5v_r0D.DSP_DNN_CTRL_RF_SEL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80B
        adov5v_r0D.DSP_DNN_PG_SLEEP_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B809
        adov5v_r0D.DSP_DNN_ISOLATEN_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80D
        adov5v_r0D.DSP_DNN_CLKENB_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B805
        adov5v_r0D.DSP_DNN_RESETN_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B815
        
        adov5v_r0D.DSP_DNN_HP_MODE_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B835
        
        adov5v_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov5v_r04.as_int);//E00040
        adov5v_r04.DSP_P2S_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov5v_r04.as_int);//E00040

        adov5v_r0D.DSP_HP_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B8B5
        adov5v_r0D.DSP_HP_ADO_GO = 1;
        adov5v_r0D.DSP_HP_DNN_GO = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BBB5
        }
    else if(mode == 3){ // HP Compression only
        adov5v_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80A
        adov5v_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov5v_r04.as_int);//E00040

        adov5v_r0D.DSP_DNN_CTRL_RF_SEL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80B
        adov5v_r0D.DSP_DNN_PG_SLEEP_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B809
        adov5v_r0D.DSP_DNN_ISOLATEN_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80D
        adov5v_r0D.DSP_DNN_CLKENB_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B805
        adov5v_r0D.DSP_DNN_RESETN_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B815
        
        adov5v_r0D.DSP_DNN_HP_MODE_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B835

        adov5v_r0D.DSP_HP_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B8B5
        adov5v_r0D.DSP_HP_ADO_GO = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BBB5
        }
    else if(mode == 0){      //DSP All Off
        adov5v_r0D.DSP_HP_ADO_GO = 0;
        adov5v_r0D.DSP_HP_DNN_GO = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BCB5
        adov5v_r0D.DSP_HP_RESETN = 0;
        adov5v_r0D.DSP_DNN_HP_MODE_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC15
        
        adov5v_r0D.DSP_DNN_RESETN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC05
        adov5v_r0D.DSP_DNN_CLKENB_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC0D
        adov5v_r0D.DSP_DNN_ISOLATEN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC09
        adov5v_r0D.DSP_DNN_PG_SLEEP_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC0B
        adov5v_r0D.DSP_DNN_CTRL_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC0A
        
        adov5v_r04.DSP_P2S_MON_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov5v_r04.as_int);//A00040
        
        adov5v_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov5v_r04.as_int);//E00040
        
        adov5v_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC4A
        }
}

inline static void afe_set_mode(uint8_t mode){
    if(mode == 1){      //LP AFE
        if(prev18_r19.XO_EN_OUT !=1){// XO ouput enable
            prev18_r19.XO_EN_OUT    = 1;
            *REG_XO_CONF1 = prev18_r19.as_int;
        }
        //enable_xo_timer();
        //start_xo_cout();
        
        adov5v_r0D.REC_ADC_RESETN = 0;
        adov5v_r0D.REC_ADCDRI_EN = 0;
        adov5v_r0D.REC_LNA_AMPEN = 0;
        adov5v_r0D.REC_LNA_AMPSW_EN = 0;
        adov5v_r0D.REC_LNA_FSETTLE = 1;
        adov5v_r0D.REC_LNA_OUTSHORT_EN = 0;
        adov5v_r0D.REC_PGA_AMPEN = 0;
        adov5v_r0D.REC_PGA_OUTSHORT_EN = 0;
        adov5v_r0D.VAD_LNA_AMPSW_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);
        
        adov5v_r13.VAD_PGA_FSETTLE = 1;
        adov5v_r13.VAD_PGA_OUTSHORT_EN = 0;
        adov5v_r13.VAD_LNA_FSETTLE = 0;
        adov5v_r13.VAD_LNA_OUTSHORT_EN = 0;
        adov5v_r13.LDO_PG_IREF = 0;
        adov5v_r13.LDO_PG_VREF_0P6LP = 0;
        adov5v_r13.LDO_PG_LDOCORE_0P6LP = 0;
        adov5v_r13.LDO_PG_VREF_1P4LP = 0;
        adov5v_r13.LDO_PG_LDOCORE_1P4LP = 0;
        adov5v_r13.LDO_PG_VREF_1P4HP = 1;
        adov5v_r13.LDO_PG_LDOCORE_1P4HP = 1;
        adov5v_r13.LDO_PG_VREF_0P9HP = 1;
        adov5v_r13.LDO_PG_LDOCORE_0P9HP = 1;
        mbus_remote_register_write(ADO_ADDR, 0x13, adov5v_r13.as_int);

        adov5v_r0F.VAD_ADC_SDLY_SEL = 0;
        adov5v_r0F.VAD_ADC_DLY_SEL = 4;
        adov5v_r0F.VAD_ADC_RESET = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0F, adov5v_r0F.as_int);

        adov5v_r14.VAD_ADCDRI_EN = 1;
        adov5v_r14.VAD_LNA_AMPEN = 1;
        adov5v_r14.VAD_PGA_AMPEN = 1;
        adov5v_r14.VAD_PGA_BWCON = 13;//13;
        adov5v_r14.VAD_PGA_GCON = 8;//8;
        mbus_remote_register_write(ADO_ADDR, 0x14, adov5v_r14.as_int);

        ////VAD MON setting can come here
        //adov5v_r1A.VAD_MONSEL = 1;
        //adov5v_r1A.VAD_MONBUF_EN = 1;
        //adov5v_r1A.VAD_REFMONBUF_EN = 0;
        //adov5v_r1A.VAD_ADCDRI_OUT_OV_EN = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x1A, adov5v_r1A.as_int);
        
        ////REC MON setting (debug)
        adov5v_r17.REC_MONSEL = 0;
        adov5v_r17.REC_MONBUF_EN = 0;
        //adov5v_r17.REC_REFMONBUF_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x17, adov5v_r17.as_int);

        adov5v_r10.DO_LP_HP_SEL = 0;    //0: LP, 1: HP
        adov5v_r10.DIO_OUT_EN = 1;
        adov5v_r10.VAD_ADC_DOUT_ISOL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x10, adov5v_r10.as_int);

        adov5v_r04.DSP_CLK_MON_SEL = 4; //LP CLK mon
        mbus_remote_register_write(ADO_ADDR, 0x04, adov5v_r04.as_int);
    }
    else if(mode == 2){ // HP AFE
        if(prev18_r19.XO_EN_OUT !=1){// XO ouput enable
            prev18_r19.XO_EN_OUT    = 1;
            *REG_XO_CONF1 = prev18_r19.as_int;
        }
        //enable_xo_timer();
        //start_xo_cout();

        adov5v_r13.LDO_PG_IREF = 0;
        adov5v_r13.LDO_PG_VREF_0P6LP = 0;
        adov5v_r13.LDO_PG_LDOCORE_0P6LP = 0;
        adov5v_r13.LDO_PG_VREF_1P4LP = 0;
        adov5v_r13.LDO_PG_LDOCORE_1P4LP = 0;
        adov5v_r13.LDO_PG_VREF_1P4HP = 0;
        adov5v_r13.LDO_PG_LDOCORE_1P4HP = 0;
        adov5v_r13.LDO_PG_VREF_0P9HP = 0;
        adov5v_r13.LDO_PG_LDOCORE_0P9HP = 0;
        mbus_remote_register_write(ADO_ADDR, 0x13, adov5v_r13.as_int);

        adov5v_r0D.REC_ADC_RESETN = 1;
        adov5v_r0D.REC_ADCDRI_EN = 1;
        adov5v_r0D.REC_LNA_AMPEN = 1;
        adov5v_r0D.REC_LNA_AMPSW_EN = 1;
        adov5v_r0D.REC_LNA_FSETTLE = 1;
        adov5v_r0D.REC_LNA_OUTSHORT_EN = 0;
        adov5v_r0D.REC_PGA_AMPEN = 1;
        adov5v_r0D.REC_PGA_OUTSHORT_EN = 0;
        adov5v_r0D.VAD_LNA_AMPSW_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);
        delay(MBUS_DELAY*10);
        //adov5v_r0D.REC_LNA_FSETTLE = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);

        adov5v_r10.DO_LP_HP_SEL = 1;    //HP
        adov5v_r10.DIO_OUT_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x10, adov5v_r10.as_int);

        adov5v_r04.DSP_CLK_MON_SEL = 2; //HP clock mon
        mbus_remote_register_write(ADO_ADDR, 0x04, adov5v_r04.as_int);
        delay(MBUS_DELAY*1000); //~10sec
    }
    else{       // AFE off
        if(prev18_r19.XO_EN_OUT !=0){// XO ouput enable
            prev18_r19.XO_EN_OUT    = 0;// XO ouput disable
            *REG_XO_CONF1 = prev18_r19.as_int;
        }
        //disable_xo_timer();
        //prev18_r19.XO_S         = mode-3;// division ratio for 16kHz out
        //*REG_XO_CONF1 = prev18_r19.as_int;

        adov5v_r0D.REC_ADC_RESETN = 0;
        adov5v_r0D.REC_ADCDRI_EN = 0;
        adov5v_r0D.REC_LNA_AMPEN = 0;
        adov5v_r0D.REC_LNA_AMPSW_EN = 0;
        adov5v_r0D.REC_LNA_FSETTLE = 1;
        adov5v_r0D.REC_LNA_OUTSHORT_EN = 0;
        adov5v_r0D.REC_PGA_AMPEN = 0;
        adov5v_r0D.REC_PGA_OUTSHORT_EN = 0;
        adov5v_r0D.VAD_LNA_AMPSW_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);
        
        adov5v_r13.VAD_PGA_FSETTLE = 1;
        adov5v_r13.VAD_LNA_FSETTLE = 1;
        adov5v_r13.LDO_PG_IREF = 0;
        adov5v_r13.LDO_PG_VREF_0P6LP = 1;
        adov5v_r13.LDO_PG_LDOCORE_0P6LP = 1;
        adov5v_r13.LDO_PG_VREF_1P4LP = 1;
        adov5v_r13.LDO_PG_LDOCORE_1P4LP = 1;
        adov5v_r13.LDO_PG_VREF_1P4HP = 1;
        adov5v_r13.LDO_PG_LDOCORE_1P4HP = 1;
        adov5v_r13.LDO_PG_VREF_0P9HP = 1;
        adov5v_r13.LDO_PG_LDOCORE_0P9HP = 1;
        mbus_remote_register_write(ADO_ADDR, 0x13, adov5v_r13.as_int);

        adov5v_r0F.VAD_ADC_RESET = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0F, adov5v_r0F.as_int);

        adov5v_r14.VAD_ADCDRI_EN = 0;
        adov5v_r14.VAD_LNA_AMPEN = 0;
        adov5v_r14.VAD_PGA_AMPEN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x14, adov5v_r14.as_int);

        adov5v_r10.DIO_OUT_EN = 0;
        //adov5v_r10.DIO_DIR_IN1OUT0 = 1;
        adov5v_r10.VAD_ADC_DOUT_ISOL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x10, adov5v_r10.as_int);
        ////VAD MON setting can come here
        //adov5v_r1A.VAD_MONSEL = 1;
        //adov5v_r1A.VAD_MONBUF_EN = 1;
        //adov5v_r1A.VAD_REFMONBUF_EN = 0;
        //adov5v_r1A.VAD_ADCDRI_OUT_OV_EN = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x1A, adov5v_r1A.as_int);
        
        ////REC MON setting (debug)
        //adov5v_r17.REC_MONSEL = 1;
        //adov5v_r17.REC_MONBUF_EN = 1;
        //adov5v_r17.REC_REFMONBUF_EN = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x17, adov5v_r17.as_int);

    }
}

//***************************************************
//  ADO+FLP functions
//***************************************************
inline static void comp_stream(void){
    
    afe_set_mode(2); // ADO HP AFE ON

    // Flash setup and start /////
    FLASH_turn_on();
    FLASH_pp_ready();
    FLASH_pp_on();
    /////////////////////////////

    digital_set_mode(3); // HP Compression only start
    
    delay(100000); // Compression-length control: ~10s
    
    digital_set_mode(0); // ADO DSP ALL OFF
    afe_set_mode(0); // ADO AFE OFF

    FLASH_pp_off();
    FLASH_turn_off();
} 

//***************************************************
// End of Program Sleep Operation
//***************************************************

static void operation_sleep(void){
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;

    // Freeze GPIO
    //freeze_gpio_out();

    // Go to Sleep
    mbus_sleep_all();
    while(1);
}


static void operation_sleep_notimer(void){
    // Make sure the irq counter is reset    
    exec_count_irq = 0;

    set_wakeup_timer(0,0,0);    //disable timer
    //set_xo_timer(0,1600,1,1);
    //start_xo_cnt();
    //mbus_sleep_all();
    
    // Go to sleep
    operation_sleep();
}



//***************************************************
// Initialization
//***************************************************
static void operation_init(void){
    pmu_sar_conv_ratio_val_test_on = 0x2E;//0x2D;
    pmu_sar_conv_ratio_val_test_off = 0x2C;//0x2A;

    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);
    *TIMERWD_GO = 0x0;
    *REG_MBUS_WD = 0;

    // EP Mode
    *EP_MODE = 0;

    // Set CPU & Mbus Clock Speeds
    //prev18_r0B.CLK_GEN_RING = 0x1; // Default 0x1, 2bits
    //prev18_r0B.CLK_GEN_DIV_MBC = 0x2; // Default 0x2, 3bits
    //prev18_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3, 3bits
    //prev18_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x1, 2bits
    //prev18_r0B.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x7, 3bits
    //*REG_CLKGEN_TUNE = prev18_r0B.as_int;

    prev18_r1C.SRAM0_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
    prev18_r1C.SRAM0_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
    //prev18_r1C.SRAM0_USE_INVERTER_SA= 1; // Default 0x0, 1bit
    *REG_SRAM0_TUNE = prev18_r1C.as_int;

    //Enumerate & Initialize Registers
    enumerated = ENUMID;
    exec_count = 0;
    exec_count_irq = 0;
    //PMU_ADC_3P0_VAL = 0x62;
    pmu_parkinglot_mode = 3;

    //Enumeration
    mbus_enumerate(ADO_ADDR);
    mbus_enumerate(FLP_ADDR);
    mbus_enumerate(PMU_ADDR);
    delay(MBUS_DELAY);

    // Set CPU Halt Option as TX --> Use for register write e.g.
    //    set_halt_until_mbus_tx();


    // PMU Settings ----------------------------------------------
    mbus_remote_register_write(PMU_ADDR,0x51, 
              ( (1 << 0) // LC_CLK_RING=1; default 1
              | (3 << 2) // LC_CLK_DIV=3; default 3
              | (0 << 4) // PMU_IRQ_EN=0; default 1
              ));

    pmu_set_clk_init();
    pmu_reset_solar_short();

    // Disable PMU ADC measurement in active mode
    // PMU_CONTROLLER_STALL_ACTIVE
    mbus_remote_register_write(PMU_ADDR,0x3A, 
            ( (1 << 19) // ignore state_horizon; default 1
              | (1 << 13) // ignore adc_output_ready; default 0
              | (1 << 12) // ignore adc_output_ready; default 0
              | (1 << 11) // ignore adc_output_ready; default 0
            ));
    delay(MBUS_DELAY);
    pmu_adc_reset_setting();
    delay(MBUS_DELAY);
    pmu_adc_enable();
    delay(MBUS_DELAY);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 3;   // 0x1E (30): ~1 min with PRCv9
    wakeup_data = 0;

    *REG_CPS = 0x7; //PG control [2]=1: Test 1.2V on,  [0]=1: Test 0.6V on
    delay(MBUS_DELAY*10);
    //init_gpio();

    FLASH_initialization(); //FLPv3L initialization
    ado_initialization(); //ADOv5V initialization
    XO_init(); //XO initialization
}

//*******************************************************************
// INTERRUPT HANDLERS 
//*******************************************************************

void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_wakeup(void) { // WAKE-UP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
    delay(MBUS_DELAY);
    // Report who woke up
    mbus_write_message32(0xAA,*SREG_WAKEUP_SOURCE); 
    //[ 0] = GOC/EP
    //[ 1] = Wakeuptimer
    //[ 2] = XO timer
    //[ 3] = gpio_pad
    //[ 4] = mbus message
    //[ 8] = gpio[0]
    //[ 9] = gpio[1]
    //[10] = gpio[2]
    //[11] = gpio[3]
    if(((*SREG_WAKEUP_SOURCE >> 8) & 1) == 1){ //waked up by gpio[0]
        //Do something
        mbus_write_message32(0xAA,0xBBCCDD); 
        delay(MBUS_DELAY*1000); //~10sec
    }
}
void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    wfi_timeout_flag = 1;
}
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);
    mbus_write_message32(0xE0, *REG0);
    if(*REG0 == 1){ // LP -> HP mode change
        *EP_MODE = 1;
                
        adov5v_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80A
       
        afe_set_mode(2);
        digital_set_mode(2);

        mbus_write_message32(0xEE, 0xABCDEF);
    }
    else if(*REG0 == 0) { // HP -> ULP mode change
        *EP_MODE = 0;
        adov5v_r0D.DSP_HP_ADO_GO = 0;
        adov5v_r0D.DSP_HP_DNN_GO = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BCB5
        
        afe_set_mode(1);
        digital_set_mode(1);
        
        mbus_write_message32(0xEE, 0xBEEFAB);
        operation_sleep_notimer();
    }
    
    mbus_write_message32(0xE1, *EP_MODE);

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
    wakeup_data = *GOC_DATA_IRQ;
    uint32_t data_cmd = (wakeup_data>>24) & 0xFF;
    uint32_t data_val = wakeup_data & 0xFF;
    uint32_t data_val2 = (wakeup_data>>8) & 0xFF;
    //uint32_t data_cmd = *REG1;
    //uint32_t data_val = *REG0;

    //mbus_write_message32(0xEE, data_cmd);
    //mbus_write_message32(0xEE, data_val);

    if(data_cmd == 0x01){       // Charge pump control
        if(data_val==1){        //ON
            adov5v_r14.CP_CLK_EN_1P2 = 1;
            adov5v_r14.CP_CLK_DIV_1P2 = 0;
            adov5v_r14.CP_VDOWN_1P2 = 0;
            mbus_remote_register_write(ADO_ADDR, 0x14, adov5v_r14.as_int);
            delay(CP_DELAY); 
            adov5v_r14.CP_VDOWN_1P2 = 1;
            mbus_remote_register_write(ADO_ADDR, 0x14, adov5v_r14.as_int);
        }
        else{                   //OFF
            adov5v_r14.CP_CLK_EN_1P2 = 0;
            adov5v_r14.CP_CLK_DIV_1P2 = 3;
            mbus_remote_register_write(ADO_ADDR, 0x14, adov5v_r14.as_int);
        }
    }
    else if(data_cmd == 0x02){  // SRAM Programming mode
        if(data_val==1){    //Enter
            adov5v_r0E.DSP_DNN_DBG_MODE_EN = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0E, adov5v_r0E.as_int);//000054
            adov5v_r0D.DSP_DNN_CTRL_RF_SEL = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80B
            adov5v_r0D.DSP_DNN_PG_SLEEP_RF = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B809
            adov5v_r0D.DSP_DNN_ISOLATEN_RF = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80D
            adov5v_r0D.DSP_DNN_CLKENB_RF = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B805
            adov5v_r0D.DSP_DNN_RESETN_RF = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B815
        }
        else{               //Exit
            adov5v_r0D.DSP_DNN_RESETN_RF = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B805
            adov5v_r0D.DSP_DNN_CLKENB_RF = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80D
            adov5v_r0D.DSP_DNN_ISOLATEN_RF = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B809
            adov5v_r0D.DSP_DNN_PG_SLEEP_RF = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80B
            adov5v_r0D.DSP_DNN_CTRL_RF_SEL = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80A
            adov5v_r0E.DSP_DNN_DBG_MODE_EN = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0E, adov5v_r0E.as_int);//000050
        }
    }
    else if(data_cmd == 0x03){  // DSP Monitoring Configure
        //adov5v_r0E.DSP_MIX_MON_EN = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x0E, adov5v_r0E.as_int);//000150
        //adov5v_r00.DSP_FE_SEL_EXT = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x00, adov5v_r00.as_int);//9F1B28
        adov5v_r04.DSP_P2S_MON_EN = 1;
        adov5v_r04.DSP_CLK_MON_SEL = 4;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov5v_r04.as_int);//A00040
        adov5v_r04.DSP_P2S_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov5v_r04.as_int);//E00040
        //adov5v_r10.DIO_OUT_EN = 1;
        //adov5v_r10.DIO_IN_EN = 0;
        //adov5v_r10.DIO_DIR_IN1OUT0 = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x10, adov5v_r10.as_int);//000008
    }
    else if(data_cmd == 0x04){  // DSP LP Control
        if(data_val==1){    //Go
            adov5v_r0D.DSP_LP_RESETN = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC4A
        }
        else{               //Stop
            adov5v_r0D.DSP_LP_RESETN = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC0A
        }
    }
    else if(data_cmd == 0x05){  // AFE Control
        afe_set_mode(data_val);
    }
    else if(data_cmd == 0x06){  // Compression + Flash Test
        if(data_val==1){    //Go
            comp_stream();
        }
        else if(data_val==2){
            FLASH_read();
        }
    }
    else if(data_cmd == 0x07){  //wake up by GPIO[0] 
        if(data_val==1){    //ON
            mbus_write_message32(0xE7, 0xBEEF01);
            config_gpio_posedge_wirq(1<<GPIO0_IRQ);
        }
        else{    //OFF
            mbus_write_message32(0xE7, 0xBEEF00);
            config_gpio_posedge_wirq(0);
        }
    }

    //unfreeze_gpio_out();
    //freeze_gpio_out();
    //operation_sleep_notimer();
}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    //init_gpio();
    //unfreeze_gpio_out();


    // Only enable relevant interrupts (PRCv17)
    *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_REG0);

    // Initialization sequence
    if (enumerated != ENUMID){
        operation_init();
    }
    
    mbus_write_message32(0xE2, *EP_MODE);
    //if (0 == 1) {
    if (*EP_MODE == 1) {
        //mbus_write_message32(0xE3, 0xBEEF01);
        while (1) config_timerwd(TIMERWD_VAL);
    }
    else {
        //delay(10000);
        mbus_write_message32(0xE4, 0xBEEF02);
        operation_sleep_notimer();
    }
    return 0;
}

