//*******************************************************************
//Author: Sechang Oh
//Description: Leidos Trestle Gen2 
//    v1.0: created. Modified from 'Leidos_dorado_Gen3' 12/14/2018
//    v1a02: GPIO functions                             03/19/2019
//    v2a03: 2.5V VBAT, self-booting support            08/14/2019
//    v2a04: SNT layer added                            10/15/2019
//*******************************************************************
#include "PREv18.h"
#include "SNTv3_RF.h"
#include "ADOv6VC_RF.h"
#include "PMUv7_RF.h"
#include "mbus.h"
  
#define ENUMID 0xBEEF

#include "DFT_LUT.txt"
#include "DCT_PHS.txt"

////////////////////////////////////////
// uncomment this for debug 
#define DEBUG_MODE          // Test VDD ON in ULP
#define DEBUG_MBUS_MSG      // Debug MBus message
/////////////////////////////////////////

#define SNT_ADDR 0x3
#define ADO_ADDR 0x4
#define FLP_ADDR 0x5
#define PMU_ADDR 0x6
#define WAKEUP_PERIOD_PARKING 30000 // About 2 hours (PRCv17)

// System parameters
#define MBUS_DELAY 100 // Amount of delay between successive messages; 100: ~9ms
#define WAKEUP_PERIOD_SNT 0x20 // About 3 sec (PRCv18)

// Temp Sensor Parameters
#define TEMP_CYCLE_INIT 1 
// Tempsensor states
#define TSTK_IDLE       0x0
#define TSTK_LDO        0x1
#define TSTK_TEMP_START 0x2
#define TSTK_TEMP_READ  0x6

#define PMU_M10C 0x0
#define PMU_00C 0x1
#define PMU_10C 0x2
#define PMU_20C 0x3
#define PMU_25C 0x4
#define PMU_35C 0x5
#define PMU_55C 0x6

#define NUM_TEMP_MEAS 1


// CP parameter
#define CP_DELAY 50000 // Amount of delay between successive messages; 100: ~9ms

#define TIMERWD_VAL 0x7FFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
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
//volatile uint32_t irq_history;
volatile uint16_t enumerated;
volatile uint32_t wakeup_data;
volatile uint8_t Tstack_state;
volatile uint8_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint8_t meas_count;
volatile uint8_t exec_count_irq;
//volatile uint32_t wakeup_period_count;
//volatile uint32_t wakeup_timer_multiplier;
//volatile uint32_t PMU_ADC_3P0_VAL;
//volatile uint32_t pmu_parkinglot_mode;
volatile uint8_t pmu_setting_state;
volatile uint16_t PMU_M10C_threshold_sns;
volatile uint16_t PMU_00C_threshold_sns;
volatile uint16_t PMU_10C_threshold_sns;
volatile uint16_t PMU_20C_threshold_sns;
volatile uint16_t PMU_35C_threshold_sns;
volatile uint16_t PMU_55C_threshold_sns;
volatile uint16_t PMU_75C_threshold_sns;
//volatile uint32_t NUM_MEAS_USER;

volatile uint32_t temp_storage_latest;
volatile uint32_t read_data_temp; // [23:0] Temp Sensor D Out
volatile uint32_t TEMP_CALIB_A;
volatile uint32_t TEMP_CALIB_B;

volatile uint8_t pmu_sar_conv_ratio_val_test_on;
volatile uint8_t pmu_sar_conv_ratio_val_test_off;
volatile uint8_t read_data_batadc;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
//volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

//volatile uint32_t temp_storage_debug;
volatile uint32_t temp_running;
//volatile uint32_t wakeup_timer_option;
volatile uint32_t snt_wup_counter_cur;
volatile uint8_t snt_timer_enabled;
volatile uint16_t SNT_0P5S_VAL;
   
volatile sntv3_r00_t sntv3_r00 = SNTv3_R00_DEFAULT;
volatile sntv3_r01_t sntv3_r01 = SNTv3_R01_DEFAULT;
//volatile sntv3_r03_t sntv3_r03 = SNTv3_R03_DEFAULT;
volatile sntv3_r08_t sntv3_r08 = SNTv3_R08_DEFAULT;
volatile sntv3_r09_t sntv3_r09 = SNTv3_R09_DEFAULT;
//volatile sntv3_r0A_t sntv3_r0A = SNTv3_R0A_DEFAULT;
//volatile sntv3_r0B_t sntv3_r0B = SNTv3_R0B_DEFAULT;
volatile sntv3_r17_t sntv3_r17 = SNTv3_R17_DEFAULT;

//volatile prev18_r0B_t prev18_r0B = PREv18_R0B_DEFAULT;
//volatile prev18_r19_t prev18_r19 = PREv18_R19_DEFAULT;
//volatile prev18_r1A_t prev18_r1A = PREv18_R1A_DEFAULT;
//volatile prev18_r1C_t prev18_r1C = PREv18_R1C_DEFAULT;
  
//volatile adov6vc_r00_t adov6vc_r00 = ADOv6VC_R00_DEFAULT;
volatile adov6vc_r04_t adov6vc_r04 = ADOv6VC_R04_DEFAULT;
//volatile adov6vc_r07_t adov6vc_r07 = ADOv6VC_R07_DEFAULT;
//volatile adov6vc_r0B_t adov6vc_r0B = ADOv6VC_R0B_DEFAULT;
volatile adov6vc_r0D_t adov6vc_r0D = ADOv6VC_R0D_DEFAULT;
//volatile adov6vc_r0E_t adov6vc_r0E = ADOv6VC_R0E_DEFAULT;
volatile adov6vc_r0F_t adov6vc_r0F = ADOv6VC_R0F_DEFAULT;
volatile adov6vc_r10_t adov6vc_r10 = ADOv6VC_R10_DEFAULT;
//volatile adov6vc_r11_t adov6vc_r11 = ADOv6VC_R11_DEFAULT;
//volatile adov6vc_r12_t adov6vc_r12 = ADOv6VC_R12_DEFAULT;
volatile adov6vc_r13_t adov6vc_r13 = ADOv6VC_R13_DEFAULT;
volatile adov6vc_r14_t adov6vc_r14 = ADOv6VC_R14_DEFAULT;
//volatile adov6vc_r15_t adov6vc_r15 = ADOv6VC_R15_DEFAULT;
//volatile adov6vc_r16_t adov6vc_r16 = ADOv6VC_R16_DEFAULT;
//volatile adov6vc_r17_t adov6vc_r17 = ADOv6VC_R17_DEFAULT;
//volatile adov6vc_r18_t adov6vc_r18 = ADOv6VC_R18_DEFAULT;
//volatile adov6vc_r19_t adov6vc_r19 = ADOv6VC_R19_DEFAULT;
//volatile adov6vc_r1A_t adov6vc_r1A = ADOv6VC_R1A_DEFAULT;
//volatile adov6vc_r1B_t adov6vc_r1B = ADOv6VC_R1B_DEFAULT;
//volatile adov6vc_r1C_t adov6vc_r1C = ADOv6VC_R1C_DEFAULT;
        
uint32_t read_data[50];
volatile uint8_t direction_gpio;


static void operation_sleep(void);
static void operation_sleep_snt_timer(void);
static void operation_sleep_noirqreset(void);
static void operation_sleep_notimer(void);
//static void pmu_set_sar_override(uint32_t val);
//static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);
//static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);

//*******************************************************************
// XO Functions
//*******************************************************************

static void XO_init(void) {
    // Parasitic Capacitance Tuning (6-bit for each; Each 1 adds 1.8pF)
    //uint32_t xo_cap_drv = 0x3F;//0x3F; // Additional Cap on OSC_DRV
    //uint32_t xo_cap_in  = 0x3F;//0x3F; // Additional Cap on OSC_IN
    //prev18_r1A.XO_CAP_TUNE = (
    //        (xo_cap_drv <<6) | 
    //        (xo_cap_in <<0));   // XO_CLK Output Pad 
    //*REG_XO_CONF2 = prev18_r1A.as_int;
    *REG_XO_CONF2 = ( (0x3F << 6)   // xo_cap_drv
                    | (0x3F)        // xo_cap_in
                    );

    *REG_XO_CONF1 = ( (0 << 22)     // XO_SLEEP (1->0)
                    | (1 << 21)     // XO_ISOLATE
                    | (1 << 20)     // XO_EN_DIV
                    | (1 << 17)     // XO_S 1:16kHz
                    | (0 << 16)     // XO_SEL_CP_DIV
                    | (1 << 15)     // XO_EN_OUT 1:enable
                    | (8 << 11)     // XO_PULSE_SEL 1-hot
                    | (7 << 8)      // XO_DELAY_EN pair usage with pulse_sel
                    | (0 << 7)      // XO_DRV_START_UP
                    | (0 << 6)      // XO_DRV_CORE
                    | (0 << 5)      // XO_RP_LOW
                    | (1 << 4)      // XO_RP_MEDIA
                    | (0 << 3)      // XO_RP_MVT
                    | (0 << 2)      // XO_RP_SVT
                    | (0 << 1)      // XO_SCN_CLK_SEL
                    | (1 << 0)      // XO_SCN_ENB
                    );
    delay(1000);

    //XO_ISOLATE 1->0
    *REG_XO_CONF1 = ( (0 << 22)     // XO_SLEEP 
                    | (0 << 21)     // XO_ISOLATE 1->0
                    | (1 << 20)     // XO_EN_DIV
                    | (1 << 17)     // XO_S 1:16kHz
                    | (0 << 16)     // XO_SEL_CP_DIV
                    | (1 << 15)     // XO_EN_OUT 1:enable
                    | (8 << 11)     // XO_PULSE_SEL 1-hot
                    | (7 << 8)      // XO_DELAY_EN pair usage with pulse_sel
                    | (0 << 7)      // XO_DRV_START_UP
                    | (0 << 6)      // XO_DRV_CORE
                    | (0 << 5)      // XO_RP_LOW
                    | (1 << 4)      // XO_RP_MEDIA
                    | (0 << 3)      // XO_RP_MVT
                    | (0 << 2)      // XO_RP_SVT
                    | (0 << 1)      // XO_SCN_CLK_SEL
                    | (1 << 0)      // XO_SCN_ENB
                    );
    delay(1000);

    //XO_DRV_START_UP 0->1
    *REG_XO_CONF1 = ( (0 << 22)     // XO_SLEEP 
                    | (0 << 21)     // XO_ISOLATE 1->0
                    | (1 << 20)     // XO_EN_DIV
                    | (1 << 17)     // XO_S 1:16kHz
                    | (0 << 16)     // XO_SEL_CP_DIV
                    | (1 << 15)     // XO_EN_OUT 1:enable
                    | (8 << 11)     // XO_PULSE_SEL 1-hot
                    | (7 << 8)      // XO_DELAY_EN pair usage with pulse_sel
                    | (1 << 7)      // XO_DRV_START_UP
                    | (0 << 6)      // XO_DRV_CORE
                    | (0 << 5)      // XO_RP_LOW
                    | (1 << 4)      // XO_RP_MEDIA
                    | (0 << 3)      // XO_RP_MVT
                    | (0 << 2)      // XO_RP_SVT
                    | (0 << 1)      // XO_SCN_CLK_SEL
                    | (1 << 0)      // XO_SCN_ENB
                    );
    delay(2000);
    
    //XO_SCN_CLK_SEL 0->1
    *REG_XO_CONF1 = ( (0 << 22)     // XO_SLEEP 
                    | (0 << 21)     // XO_ISOLATE 1->0
                    | (1 << 20)     // XO_EN_DIV
                    | (1 << 17)     // XO_S 1:16kHz
                    | (0 << 16)     // XO_SEL_CP_DIV
                    | (1 << 15)     // XO_EN_OUT 1:enable
                    | (8 << 11)     // XO_PULSE_SEL 1-hot
                    | (7 << 8)      // XO_DELAY_EN pair usage with pulse_sel
                    | (1 << 7)      // XO_DRV_START_UP
                    | (0 << 6)      // XO_DRV_CORE
                    | (0 << 5)      // XO_RP_LOW
                    | (1 << 4)      // XO_RP_MEDIA
                    | (0 << 3)      // XO_RP_MVT
                    | (0 << 2)      // XO_RP_SVT
                    | (1 << 1)      // XO_SCN_CLK_SEL
                    | (1 << 0)      // XO_SCN_ENB
                    );
    delay(2000);

    //XO_SCN_CLK_SEL 1 -> 0
    //XO_SCN_ENB 1->0
    *REG_XO_CONF1 = ( (0 << 22)     // XO_SLEEP 
                    | (0 << 21)     // XO_ISOLATE 1->0
                    | (1 << 20)     // XO_EN_DIV
                    | (1 << 17)     // XO_S 1:16kHz
                    | (0 << 16)     // XO_SEL_CP_DIV
                    | (1 << 15)     // XO_EN_OUT 1:enable
                    | (8 << 11)     // XO_PULSE_SEL 1-hot
                    | (7 << 8)      // XO_DELAY_EN pair usage with pulse_sel
                    | (1 << 7)      // XO_DRV_START_UP
                    | (0 << 6)      // XO_DRV_CORE
                    | (0 << 5)      // XO_RP_LOW
                    | (1 << 4)      // XO_RP_MEDIA
                    | (0 << 3)      // XO_RP_MVT
                    | (0 << 2)      // XO_RP_SVT
                    | (0 << 1)      // XO_SCN_CLK_SEL
                    | (0 << 0)      // XO_SCN_ENB
                    );
    delay(2000);

    //XO_DRV_START_UP  = 0x0;
    //XO_DRV_CORE      = 0x1;// 1: enables core circuit
    //XO_SCN_CLK_SEL   = 0x1;
    *REG_XO_CONF1 = ( (0 << 22)     // XO_SLEEP 
                    | (0 << 21)     // XO_ISOLATE 1->0
                    | (1 << 20)     // XO_EN_DIV
                    | (1 << 17)     // XO_S 1:16kHz
                    | (0 << 16)     // XO_SEL_CP_DIV
                    | (1 << 15)     // XO_EN_OUT 1:enable
                    | (8 << 11)     // XO_PULSE_SEL 1-hot
                    | (7 << 8)      // XO_DELAY_EN pair usage with pulse_sel
                    | (0 << 7)      // XO_DRV_START_UP
                    | (1 << 6)      // XO_DRV_CORE
                    | (0 << 5)      // XO_RP_LOW
                    | (1 << 4)      // XO_RP_MEDIA
                    | (0 << 3)      // XO_RP_MVT
                    | (0 << 2)      // XO_RP_SVT
                    | (1 << 1)      // XO_SCN_CLK_SEL
                    | (0 << 0)      // XO_SCN_ENB
                    );

    enable_xo_timer();
    start_xo_cout();
    
#ifdef DEBUG_MBUS_MSG
    mbus_write_message32(0xE3,0x00000E2D); 
#endif    
}


//************************************
// GPIO Functions
// ***********************************
static void init_gpio(void){
    gpio_set_dir (direction_gpio);  // input:0, output:1
    gpio_write_current_data();
    set_gpio_pad (0xFF); // 0xFF activate all 8 GPIO bits
    unfreeze_gpio_out();
}



//************************************
// PMU Related Functions
//************************************
static void pmu_set_sar_override(uint8_t val){
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
    mbus_remote_register_write(PMU_ADDR,0x17, 
            ( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
              | (0 << 13) // Enable main feedback loop
              | (r << 9)  // Frequency multiplier R
              | (l << 5)  // Frequency multiplier L (actually L+1)
              | (base)      // Floor frequency base (0-63)
            ));
    
    //mbus_remote_register_write(PMU_ADDR,0x17, 
    //        ( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
    //          | (0 << 13) // Enable main feedback loop
    //          | (1 << 9)  // Frequency multiplier R
    //          | (0 << 5)  // Frequency multiplier L (actually L+1)
    //          | (8)         // Floor frequency base (0-63)
    //        ));
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

inline static void pmu_active_setting_temp_based(){
    if (pmu_setting_state == PMU_55C){
        pmu_set_active_clk(0x1,0x0,0x10,0x2/*V1P2*/);
    }else if (pmu_setting_state == PMU_35C){
        pmu_set_active_clk(0x2,0x1,0x10,0x2/*V1P2*/);
    }else if (pmu_setting_state == PMU_20C){
        pmu_set_active_clk(0x7,0x2,0x10,0x4/*V1P2*/);
    }else if (pmu_setting_state == PMU_10C){
        pmu_set_active_clk(0xD,0x2,0x10,0x4/*V1P2*/);
    }else if (pmu_setting_state == PMU_00C){
        pmu_set_active_clk(0xD,0x2,0x10,0x4/*V1P2*/);
    }else if (pmu_setting_state == PMU_M10C){
        pmu_set_active_clk(0xD,0x2,0x10,0x4/*V1P2*/);
    }else{ // 25C, default
        pmu_set_active_clk(0x5,0x1,0x10,0x2/*V1P2*/);
    }
}

inline static void pmu_set_sleep_radio(){
    pmu_set_sleep_clk(0xF,0xA,0x5,0xF/*V1P2*/);
}

inline static void pmu_set_sleep_low(){
    pmu_set_sleep_clk(0x3,0x1,0x1,0x2/*V1P2*/);
}

inline static void pmu_set_sleep_tsns(){
    if (pmu_setting_state >= PMU_35C){
        pmu_set_active_clk(0xA,0xA,0x5,0xF/*V1P2*/);
    }else if (pmu_setting_state < PMU_20C){
        pmu_set_active_clk(0xF,0xA,0x7,0xF/*V1P2*/);
    }else{ // 25C, default
    	pmu_set_sleep_clk(0xF,0xA,0x5,0xF/*V1P2*/);
    }
}

inline static void pmu_sleep_setting_temp_based(){
    if (pmu_setting_state == PMU_55C){
        pmu_set_sleep_clk(0x1,0x1,0x1,0x2/*V1P2*/);
    }else if (pmu_setting_state == PMU_35C){
        pmu_set_sleep_clk(0x2,0x1,0x1,0x2/*V1P2*/);
    }else if (pmu_setting_state == PMU_20C){
        pmu_set_sleep_clk(0xF,0x2,0x1,0x4/*V1P2*/);
    }else if (pmu_setting_state == PMU_10C){
        pmu_set_sleep_clk(0xF,0x1,0x1,0x2/*V1P2*/);
    }else{ // 25C, default
		pmu_set_sleep_low();
    }
}

inline static void pmu_set_clk_init(){
    pmu_set_sar_override(0x4A);
    pmu_set_active_clk(0xA,0x4,0x10,0x4);
    pmu_set_sleep_clk(0xA,0x4,0x10,0x4); //with TEST1P2
    //pmu_set_active_clk(0xA,0x4,0x20,0x4); //-10C test
    //pmu_set_sleep_clk(0xA,0x4,0x20,0x4); //with TEST1P2
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
              | (0)      // Binary converter's conversion ratio (7'h00)
            ));
    delay(MBUS_DELAY);
    //  pmu_set_sar_override(pmu_sar_conv_ratio_val_test_on);
    //pmu_set_sar_override(0x4A);
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
             | (0 << 19) //state_vbat_readonly
             | (1 << 20) //state_state_horizon
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
             | (0 << 19) //state_vbat_readonly
             | (1 << 20) //state_state_horizon
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
             | (0 << 19) //state_vbat_readonly
             | (1 << 20) //state_state_horizon
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
//  SNT functions
//***************************************************
inline static void snt_initialization (void) {
    // Temp Sensor Settings --------------------------------------
    // sntv3_r01
    sntv3_r01.TSNS_RESETn = 0;
    sntv3_r01.TSNS_EN_IRQ = 1;
    sntv3_r01.TSNS_BURST_MODE = 0;
    sntv3_r01.TSNS_CONT_MODE = 0;
    mbus_remote_register_write(SNT_ADDR,1,sntv3_r01.as_int);

    // Set temp sensor conversion time
    //sntv3_r03.TSNS_SEL_STB_TIME = 0x1; 
    //sntv3_r03.TSNS_SEL_CONV_TIME = 0x6; // Default: 0x6
    //mbus_remote_register_write(SNT_ADDR,0x03,sntv3_r03.as_int);
    mbus_remote_register_write(SNT_ADDR,0x03,
            ( (4 << 16)     //TSNS_MIM dflt
            | (4 << 13)     //TSNS_MOM dflt
            | (0xE <<9)     //TSNS_SEL_VVDD dflt
            | (1 << 6)      //TSNS_SEL_STB_TIME
            | (2 << 4)      //TSNS_SEL_REF_STB_TIME dflt
            | (6)           //TSNS_SEL_CONV_TIME dflt
            ));

    // SNT Wakeup Timer Settings --------------------------------------
    // Config Register A
    //sntv3_r0A.TMR_S = 0x1; // Default: 0x4, use 1 for good TC
    // Tune R for TC
    //sntv3_r0A.TMR_DIFF_CON = 0x3FFF; // Default: 0x3FFB
    //mbus_remote_register_write(SNT_ADDR,0x0A,sntv3_r0A.as_int);
    mbus_remote_register_write(SNT_ADDR,0x0A,
            ( (1 << 21)     //TMR_S
            | (0x3FFF << 7) //TMR_DIFF_CON
            | (1 << 6)      //TMR_POLY_CON dlft
            | (1 << 5)      //TMR_EN_TUNE1_RES dlft
            | (1 << 4)      //TMR_EN_TUNE2_RES dlft
            | (1 << 3)      //TMR_SAMPLE_EN dlft
            | (4)           //TMR_AFC dlft
            ));

	// TIMER CAP_TUNE  
    //sntv3_r0B.TMR_TFR_CON = 0xF; // Default: 0xF
    //mbus_remote_register_write(SNT_ADDR,0x0B,sntv3_r0B.as_int);

    // TIMER CAP_TUNE  
    // Tune C for freq
	sntv3_r09.TMR_SEL_CLK_DIV = 0x0; // Default : 1'h1
    sntv3_r09.TMR_SEL_CAP = 0x80; // Default : 8'h8
    sntv3_r09.TMR_SEL_DCAP = 0x3F; // Default : 6'h4

    sntv3_r09.TMR_EN_TUNE1 = 0x1; // Default : 1'h1
    sntv3_r09.TMR_EN_TUNE2 = 0x1; // Default : 1'h1

    // to reduce standby current
    sntv3_r09.TMR_IBIAS_REF = 0x0; // Default : 4'h4

    mbus_remote_register_write(SNT_ADDR,0x09,sntv3_r09.as_int);

    // Wakeup Counter
    sntv3_r17.WUP_CLK_SEL = 0x0; 
    sntv3_r17.WUP_AUTO_RESET = 0x0; // Automatically reset counter to 0 upon sleep 
    mbus_remote_register_write(SNT_ADDR,0x17,sntv3_r17.as_int);
}

//static void temp_sensor_start(){
//    sntv3_r01.TSNS_RESETn = 1;
//    mbus_remote_register_write(SNT_ADDR,1,sntv3_r01.as_int);
//}
//static void temp_sensor_reset(){
//    sntv3_r01.TSNS_RESETn = 0;
//    mbus_remote_register_write(SNT_ADDR,1,sntv3_r01.as_int);
//}
static void temp_sensor_power_on(){
    // Turn on digital block
    sntv3_r01.TSNS_SEL_LDO = 1;
    mbus_remote_register_write(SNT_ADDR,1,sntv3_r01.as_int);
    // Turn on analog block
    sntv3_r01.TSNS_EN_SENSOR_LDO = 1;
    mbus_remote_register_write(SNT_ADDR,1,sntv3_r01.as_int);

    delay(MBUS_DELAY);

    // Release isolation
    sntv3_r01.TSNS_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR,1,sntv3_r01.as_int);
}
static void temp_sensor_power_off(){
    sntv3_r01.TSNS_RESETn = 0;
    sntv3_r01.TSNS_SEL_LDO = 0;
    sntv3_r01.TSNS_EN_SENSOR_LDO = 0;
    sntv3_r01.TSNS_ISOLATE = 1;
    mbus_remote_register_write(SNT_ADDR,1,sntv3_r01.as_int);
}
static void sns_ldo_vref_on(){
    sntv3_r00.LDO_EN_VREF    = 1;
    mbus_remote_register_write(SNT_ADDR,0,sntv3_r00.as_int);
}

static void sns_ldo_power_on(){
    sntv3_r00.LDO_EN_IREF    = 1;
    sntv3_r00.LDO_EN_LDO    = 1;
    mbus_remote_register_write(SNT_ADDR,0,sntv3_r00.as_int);
}
static void sns_ldo_power_off(){
    sntv3_r00.LDO_EN_VREF    = 0;
    sntv3_r00.LDO_EN_IREF    = 0;
    sntv3_r00.LDO_EN_LDO    = 0;
    mbus_remote_register_write(SNT_ADDR,0,sntv3_r00.as_int);
}

static void snt_read_wup_counter(){

    set_halt_until_mbus_rx();
    mbus_remote_register_read(SNT_ADDR,0x1B,1); // CNT_VALUE_EXT
    snt_wup_counter_cur = (*REG1 & 0xFF)<<24;
    mbus_remote_register_read(SNT_ADDR,0x1C,1); // CNT_VALUE
    snt_wup_counter_cur = snt_wup_counter_cur | (*REG1 & 0xFFFFFF);
    set_halt_until_mbus_tx();

}
    
static void snt_start_timer_presleep(){

    sntv3_r09.TMR_IBIAS_REF = 0x4; // Default : 4'h4
    mbus_remote_register_write(SNT_ADDR,0x09,sntv3_r09.as_int);

	// Release Power Gate
	sntv3_r08.TMR_SLEEP = 0x0; // Default : 0x1
	sntv3_r08.TMR_ISOLATE = 0x0; // Default : 0x1
	mbus_remote_register_write(SNT_ADDR,0x08,sntv3_r08.as_int);

    // TIMER SELF_EN Disable 
    sntv3_r09.TMR_SELF_EN = 0x0; // Default : 0x1
    mbus_remote_register_write(SNT_ADDR,0x09,sntv3_r09.as_int);

    // EN_OSC 
    sntv3_r08.TMR_EN_OSC = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv3_r08.as_int);

    // Release Reset 
    sntv3_r08.TMR_RESETB = 0x1; // Default : 0x0
    sntv3_r08.TMR_RESETB_DIV = 0x1; // Default : 0x0
    sntv3_r08.TMR_RESETB_DCDC = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv3_r08.as_int);

    // TIMER EN_SEL_CLK Reset 
    sntv3_r08.TMR_EN_SELF_CLK = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv3_r08.as_int);

    // TIMER SELF_EN 
    sntv3_r09.TMR_SELF_EN = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x09,sntv3_r09.as_int);
    //delay(100000); 

    snt_timer_enabled = 1;
}

static void snt_start_timer_postsleep(){
    // Turn off sloscillator
    sntv3_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv3_r08.as_int);
}


static void snt_stop_timer(){

    // EN_OSC
    sntv3_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    // RESET
    sntv3_r08.TMR_EN_SELF_CLK = 0x0; // Default : 0x0
    sntv3_r08.TMR_RESETB = 0x0;// Default : 0x0
    sntv3_r08.TMR_RESETB_DIV = 0x0; // Default : 0x0
    sntv3_r08.TMR_RESETB_DCDC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv3_r08.as_int);
    snt_timer_enabled = 0;

	// Enable Power Gate
	sntv3_r08.TMR_SLEEP = 0x0; // Default : 0x1
	sntv3_r08.TMR_ISOLATE = 0x0; // Default : 0x1
	mbus_remote_register_write(SNT_ADDR,0x08,sntv3_r08.as_int);

    sntv3_r09.TMR_IBIAS_REF = 0x0; // Default : 4'h4
    mbus_remote_register_write(SNT_ADDR,0x09,sntv3_r09.as_int);

    sntv3_r17.WUP_ENABLE = 0x0; // Default : 0x
    mbus_remote_register_write(SNT_ADDR,0x17,sntv3_r17.as_int);

}

static void snt_set_wup_timer(uint32_t sleep_count){
    
    snt_wup_counter_cur = sleep_count + snt_wup_counter_cur; // should handle rollover

    mbus_remote_register_write(SNT_ADDR,0x19,snt_wup_counter_cur>>24);
    mbus_remote_register_write(SNT_ADDR,0x1A,snt_wup_counter_cur & 0xFFFFFF);
    
    sntv3_r17.WUP_ENABLE = 0x1; // Default : 0x
    mbus_remote_register_write(SNT_ADDR,0x17,sntv3_r17.as_int);

}


//***************************************************
// Temperature measurement operation (SNTv3)
//***************************************************



static void operation_temp_run(void){
    if (Tstack_state == TSTK_IDLE){
        Tstack_state = TSTK_LDO;
        wfi_timeout_flag = 0;
        // Turn on SNS LDO VREF; requires settling
        sns_ldo_vref_on();
    }else if (Tstack_state == TSTK_LDO){
        Tstack_state = TSTK_TEMP_START;
        // Power on SNS LDO
        sns_ldo_power_on();
        // Power on temp sensor
        temp_sensor_power_on();
        delay(MBUS_DELAY);
    }else if (Tstack_state == TSTK_TEMP_START){
        // Start temp measurement
        Tstack_state = TSTK_TEMP_READ;
        pmu_set_sleep_tsns();
        //temp_sensor_start();
        sntv3_r01.TSNS_RESETn = 1;
        mbus_remote_register_write(SNT_ADDR,1,sntv3_r01.as_int);
		// Go to sleep during measurement
		operation_sleep();

    }else if (Tstack_state == TSTK_TEMP_READ){

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
            //temp_sensor_reset();
            sntv3_r01.TSNS_RESETn = 0;
            mbus_remote_register_write(SNT_ADDR,1,sntv3_r01.as_int);
            Tstack_state = TSTK_TEMP_START;
                
        }else{
            meas_count = 0;

            // Read latest PMU ADC measurement
            pmu_adc_read_latest();
        
			uint8_t pmu_setting_prev = pmu_setting_state;
            // Change PMU based on temp
            if (temp_storage_latest > PMU_55C_threshold_sns){
                pmu_setting_state = PMU_55C;
            }else if (temp_storage_latest > PMU_35C_threshold_sns){
                pmu_setting_state = PMU_35C;
            }else if (temp_storage_latest < PMU_10C_threshold_sns){
                pmu_setting_state = PMU_10C;
            }else if (temp_storage_latest < PMU_20C_threshold_sns){
                pmu_setting_state = PMU_20C;
            }else{
                pmu_setting_state = PMU_25C;
            }

			// Always restore sleep setting from higher pmu meas setting
   	        pmu_sleep_setting_temp_based();

			if (pmu_setting_prev != pmu_setting_state){
	            pmu_active_setting_temp_based();
			}


            // Assert temp sensor isolation & turn off temp sensor power
            temp_sensor_power_off();
            sns_ldo_power_off();
            Tstack_state = TSTK_IDLE;

            #ifdef DEBUG_MBUS_MSG
            mbus_write_message32(0xCC, exec_count);
            #endif
            mbus_write_message32(0xC0, (exec_count << 16) | temp_storage_latest);


            //if (temp_storage_debug){
            //    temp_storage_latest = exec_count;
            //}

            exec_count++;
            // If NUM_MEAS_USER == 0, meas infinitely
            //if ((pmu_setting_state == PMU_25C) && (NUM_MEAS_USER && (exec_count >= NUM_MEAS_USER))){
                // Trig 2 stop condition; Need to run until PMU setting is back at RT
                // No more measurement required
                // Make sure temp sensor is off
        //        temp_running = 0;
        //        operation_sleep_notimer();
        //    }else{
            //if (wakeup_timer_option){
            //    // Use PRC Timer
            //    set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
            //    operation_sleep();
            //}else{
                // Use SNT Timer    
                snt_set_wup_timer(WAKEUP_PERIOD_CONT_USER);
                operation_sleep_snt_timer();
            //}

        //    }

        }

    }else{
        //default:  // THIS SHOULD NOT HAPPEN
        // Reset Temp Sensor 
        temp_sensor_power_off();
        sns_ldo_power_off();
        operation_sleep_notimer();
    }

}
static void run_tempsensor (void){
    exec_count_irq++;

    // Skip SNT timer settings if PRC timer is to be used
    //if ((exec_count_irq == 1) && wakeup_timer_option)
    //    exec_count_irq = 3;

    if (exec_count_irq == 1){
        // SNT pulls higher current in the beginning
        pmu_set_sleep_radio();
        snt_start_timer_presleep();
        mbus_write_message32(0xAA, 0x00001003);
        // Go to sleep for >3s for timer stabilization
        set_wakeup_timer (WAKEUP_PERIOD_SNT, 0x1, 0x1);
        operation_sleep_noirqreset();
    }else if (exec_count_irq == 2){
        snt_start_timer_postsleep();
        // Read existing counter value; in case not reset to zero
        snt_read_wup_counter();
        mbus_write_message32(0xAA, 0x00001006);
        // Restore sleep setting to low
        pmu_set_sleep_low();
    }

    if (!temp_running){
        temp_running = 1;
    }

    exec_count = 0;
    meas_count = 0;

    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;
    exec_count_irq = 0;

    // Run Temp Sensor Program
    Tstack_state = TSTK_IDLE;
    mbus_write_message32(0xAA, 0x00001007);
    operation_temp_run();
}

//***************************************************
//  FLP functions
//***************************************************
static void flp_fail(uint16_t id)
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

static void flash_initialization (void) {
    
    // Tune Flash
    mbus_remote_register_write (FLP_ADDR , 0x26 , 0x0D7788); // Program Current
    mbus_remote_register_write (FLP_ADDR , 0x27 , 0x011BC8); // Erase Pump Diode Chain
    mbus_remote_register_write (FLP_ADDR , 0x01 , 0x000109); // Tprog idle time
    mbus_remote_register_write (FLP_ADDR , 0x19 , 0x000F03); // Voltage Clamper Tuning
    mbus_remote_register_write (FLP_ADDR , 0x0F , 0x001001); // Flash interrupt target register addr: REG0 -> REG1
    //mbus_remote_register_write (FLP_ADDR , 0x12 , 0x000003); // Auto Power On/Off

}

static void FLASH_turn_on()
{
    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP_ADDR , 0x11 , 0x00002F);
    set_halt_until_mbus_tx();

    if (*REG1 != 0xB5) flp_fail (0);
}

static void FLASH_turn_off()
{
    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP_ADDR , 0x11 , 0x00002D);
    set_halt_until_mbus_tx();

    if (*REG1 != 0xBB) flp_fail (1);
}

static void FLASH_pp_ready (void) {

        // Erase Flash
        uint32_t page_start_addr = 0;
        uint16_t idx;

        for (idx=0; idx<512; idx++){ // All Pages (8Mb) are erased.
            page_start_addr = (idx << 8);

            mbus_remote_register_write (FLP_ADDR, 0x08, page_start_addr); // Set FLSH_START_ADDR

            set_halt_until_mbus_rx();
            mbus_remote_register_write (FLP_ADDR, 0x09, 0x000029); // Page Erase
            set_halt_until_mbus_tx();

            //if (*REG1 != 0x00004F) flp_fail((0xFFFF << 16) | idx);
            if (*REG1 != 0x00004F) flp_fail((0xF << 12) | idx);
        }
    
    // Ping Pong Setting
        mbus_remote_register_write (FLP_ADDR, 0x13 , 0x000001); // Enable Ping Pong w/ No length limit
        mbus_remote_register_write (FLP_ADDR, 0x14 , 0x00000B); // Over run err no detection & fast programming & no wrapping & DATA0/DATA1 enable
        mbus_remote_register_write (FLP_ADDR, 0x15 , 0x000000); // Flash program addr started from 0x0

}

static void FLASH_pp_on (void) {
    mbus_remote_register_write (FLP_ADDR, 0x09, 0x00002D); // Ping Pong Go
}

static void FLASH_pp_off (void) {
    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP_ADDR, 0x13, 0x000000); // Ping Pong End
    set_halt_until_mbus_tx();
}

static void FLASH_read (void) {
    //uint16_t i;
    uint16_t j;
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

    for(j=0; j<281; j++){
    //for(j=0; j<281; j++){
        set_halt_until_mbus_rx();
        mbus_copy_mem_from_remote_to_any_bulk (FLP_ADDR, (uint32_t *) flp_sram_addr, 0x1, read_data, 49);
        set_halt_until_mbus_tx();
        //for (i=0; i<100; i++) {
            //delay(100);
            //mbus_write_message32(0xC0, read_data[i]);
            //delay(100);
        //}
        flp_sram_addr = flp_sram_addr + 200;
    }

    // Turn off Flash
    FLASH_turn_off();
}

//***************************************************
//  ADO functions
//***************************************************
static void ado_initialization(void){
    // as_int of all zero regs initalization
    //adov6vc_r07.as_int = 0;
    //adov6vc_r0B.as_int = 0;
    adov6vc_r10.as_int = 0;

    adov6vc_r10.VAD_ADC_DOUT_ISOL = 1;
    mbus_remote_register_write(ADO_ADDR, 0x10, adov6vc_r10.as_int);
 
    // AFE Initialization
    //adov6vc_r15.IB_GEN_CLK_EN = 1;
    //adov6vc_r15.LS_CLK_EN_1P2 = 1;
    //adov6vc_r15.REC_PGA_BWCON = 32;//24;
    //adov6vc_r15.REC_PGA_CFBADD = 1;//0;
    //adov6vc_r15.REC_PGA_GCON = 2;//2;
    //mbus_remote_register_write(ADO_ADDR, 0x15, adov6vc_r15.as_int); //1F8822//1F8602
    mbus_remote_register_write(ADO_ADDR, 0x15, 0x1F8822); 

    //adov6vc_r16.IBC_MONAMP = 0;
    //adov6vc_r16.IBC_REC_LNA = 6;
    //adov6vc_r16.IBC_REC_PGA = 11;
    //adov6vc_r16.IBC_VAD_LNA = 7;
    //adov6vc_r16.IBC_VAD_PGA = 5;
    //mbus_remote_register_write(ADO_ADDR, 0x16, adov6vc_r16.as_int);//0535BD
    mbus_remote_register_write(ADO_ADDR, 0x16, 0x0535BD);//0535BD
 
    //adov6vc_r18.REC_LNA_N1_CON = 2;
    //adov6vc_r18.REC_LNA_N2_CON = 2;
    //adov6vc_r18.REC_LNA_P1_CON = 3;
    //adov6vc_r18.REC_LNA_P2_CON = 3;
    //mbus_remote_register_write(ADO_ADDR, 0x18, adov6vc_r18.as_int);//0820C3
    mbus_remote_register_write(ADO_ADDR, 0x18, 0x0820C3);//0820C3

    //adov6vc_r19.REC_PGA_P1_CON = 3;
    //mbus_remote_register_write(ADO_ADDR, 0x19, adov6vc_r19.as_int);//007064
    mbus_remote_register_write(ADO_ADDR, 0x19, 0x007064);//007064

    //adov6vc_r1B.VAD_LNA_N1_LCON = 5;
    //adov6vc_r1B.VAD_LNA_N2_LCON = 6;
    //adov6vc_r1B.VAD_LNA_P1_CON = 3;
    //adov6vc_r1B.VAD_LNA_P2_CON = 2;
    //mbus_remote_register_write(ADO_ADDR, 0x1B, adov6vc_r1B.as_int);//0A6062
    mbus_remote_register_write(ADO_ADDR, 0x1B, 0x0A6062);//0A6062

    //adov6vc_r1C.VAD_PGA_N1_LCON = 0;
    //adov6vc_r1C.VAD_PGA_N1_MCON = 1;
    //mbus_remote_register_write(ADO_ADDR, 0x1C, adov6vc_r1C.as_int);//00079C
    mbus_remote_register_write(ADO_ADDR, 0x1C, 0x00079C);//00079C

    //adov6vc_r11.LDO_CTRL_VREFLDO_VOUT_1P4HP = 4;
    //mbus_remote_register_write(ADO_ADDR, 0x11, adov6vc_r11.as_int);//84CAC9
    mbus_remote_register_write(ADO_ADDR, 0x11, 0x84CAC9);//84CAC9

    //adov6vc_r12.LDO_CTRL_VREFLDO_VOUT_0P6LP = 3;
    //adov6vc_r12.LDO_CTRL_VREFLDO_VOUT_0P9HP = 3;
    //adov6vc_r12.LDO_CTRL_VREFLDO_VOUT_1P4LP = 4;
    //mbus_remote_register_write(ADO_ADDR, 0x12, adov6vc_r12.as_int);//838384
    mbus_remote_register_write(ADO_ADDR, 0x12, 0x838384);
    
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
        //adov6vc_r07.DSP_LUT_WR_ADDR = i;
        //adov6vc_r07.DSP_LUT_DATA_IN = LUT_DATA[i];
        //mbus_remote_register_write(ADO_ADDR, 0x07, adov6vc_r07.as_int);
        mbus_remote_register_write(ADO_ADDR, 0x07, 
                (  (i << 4)         //DSP_LUT_WR_ADDR
                 | (LUT_DATA[i])    //DSP_LUT_DATA_IN
                ));
        mbus_remote_register_write(ADO_ADDR, 0x08, 0x000001);
        mbus_remote_register_write(ADO_ADDR, 0x08, 0x000000);
    }
    //PHS Programming 
    for(i= 0; i<PHS_DATA_LENGTH; i++){
        //adov6vc_r0B.DSP_PHS_WR_ADDR_FS = i;
        //adov6vc_r0B.DSP_PHS_DATA_IN_FS = PHS_DATA[i];
        //mbus_remote_register_write(ADO_ADDR, 0x0B, adov6vc_r0B.as_int);
        mbus_remote_register_write(ADO_ADDR, 0x0B,
               (  (i << 12)     //DSP_PHS_WR_ADDR_FS
                | (PHS_DATA[i]) //DSP_PHS_DATA_IN_FS
               ));
        mbus_remote_register_write(ADO_ADDR, 0x0C, 0x000001);
        mbus_remote_register_write(ADO_ADDR, 0x0C, 0x000000);
    }
    
    //N_DCT: 128 points DCT 
    //adov6vc_r00.DSP_N_DCT = 2; //128-pt DCT
    //adov6vc_r00.DSP_N_FFT = 4; //256-pt FFT
    //mbus_remote_register_write(ADO_ADDR, 0x00, adov6vc_r00.as_int);
    mbus_remote_register_write(ADO_ADDR, 0x00, 0x9E1C48);
    
    //WAKEUP REQ EN
    mbus_remote_register_write(ADO_ADDR, 0x0E, 
                ((1<<7)     //DSP_WAKEUP_REQ_EN ->1
                |(2<<5)     //LC_CLK_DIV
                |(2<<3)     //LC_CLK_RING
                ));
}


static void digital_set_mode(uint8_t mode){
    if(mode == 1){      //LP DSP
        adov6vc_r0D.DSP_HP_ADO_GO = 0;
        adov6vc_r0D.DSP_HP_DNN_GO = 0;
        adov6vc_r0D.DSP_HP_FIFO_GO = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BCB5
        adov6vc_r0D.DSP_HP_RESETN = 0;
        adov6vc_r0D.DSP_DNN_HP_MODE_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC15
        
        adov6vc_r0D.DSP_DNN_RESETN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC05
        adov6vc_r0D.DSP_DNN_ISOLATEN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC09
        adov6vc_r0D.DSP_DNN_PG_SLEEP_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC0B
        adov6vc_r0D.DSP_DNN_CTRL_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC0A
        
        adov6vc_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vc_r04.as_int);//E00040
        adov6vc_r04.DSP_P2S_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vc_r04.as_int);//E00040
        
        adov6vc_r0D.DSP_LP_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC4A
        }
    else if(mode == 2){ // HP DSP
        adov6vc_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80A

        adov6vc_r0D.DSP_DNN_CTRL_RF_SEL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80B
        adov6vc_r0D.DSP_DNN_CLKENB_RF_SEL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80B
        adov6vc_r0D.DSP_DNN_PG_SLEEP_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B809
        adov6vc_r0D.DSP_DNN_ISOLATEN_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80D
        adov6vc_r0D.DSP_DNN_CLKENB_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B805
        adov6vc_r0D.DSP_DNN_RESETN_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B815
        
        adov6vc_r0D.DSP_DNN_HP_MODE_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B835
        
        adov6vc_r0D.DSP_DNN_CLKENB_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80B
        
        adov6vc_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vc_r04.as_int);//E00040
        adov6vc_r04.DSP_P2S_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vc_r04.as_int);//E00040

        adov6vc_r0D.DSP_HP_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B8B5
        adov6vc_r0D.DSP_HP_FIFO_GO = 1;
        adov6vc_r0D.DSP_HP_ADO_GO = 1;
        adov6vc_r0D.DSP_HP_DNN_GO = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BBB5
        }
    else if(mode == 3){ // HP Compression only
        adov6vc_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80A
        adov6vc_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vc_r04.as_int);//E00040

        adov6vc_r0D.DSP_DNN_CTRL_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80B
        adov6vc_r0D.DSP_DNN_CLKENB_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80B
        //adov6vc_r0D.DSP_DNN_PG_SLEEP_RF = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B809
        //adov6vc_r0D.DSP_DNN_ISOLATEN_RF = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80D
        //adov6vc_r0D.DSP_DNN_RESETN_RF = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B815
        
        adov6vc_r0D.DSP_DNN_HP_MODE_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B835

        adov6vc_r0D.DSP_HP_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B8B5
        adov6vc_r0D.DSP_HP_FIFO_GO = 1;
        adov6vc_r0D.DSP_HP_ADO_GO = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BBB5
        }
    else if(mode == 0){      //DSP All Off
        adov6vc_r0D.DSP_HP_FIFO_GO = 0;
        adov6vc_r0D.DSP_HP_ADO_GO = 0;
        adov6vc_r0D.DSP_HP_DNN_GO = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BCB5
        adov6vc_r0D.DSP_HP_RESETN = 0;
        adov6vc_r0D.DSP_DNN_HP_MODE_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC15
        
        adov6vc_r0D.DSP_DNN_CLKENB_RF_SEL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80B
        adov6vc_r0D.DSP_DNN_RESETN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC05
        adov6vc_r0D.DSP_DNN_CLKENB_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC0D
        adov6vc_r0D.DSP_DNN_ISOLATEN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC09
        adov6vc_r0D.DSP_DNN_PG_SLEEP_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC0B
        adov6vc_r0D.DSP_DNN_CLKENB_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80B
        adov6vc_r0D.DSP_DNN_CTRL_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC0A
        
        adov6vc_r04.DSP_P2S_MON_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vc_r04.as_int);//A00040
        
        adov6vc_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vc_r04.as_int);//E00040
        
        adov6vc_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC4A
        }
}

static void afe_set_mode(uint8_t mode){
    if(mode == 1){      //LP AFE
        *REG_XO_CONF1 = *REG_XO_CONF1 | (1 << 15);
        
        adov6vc_r0D.REC_ADC_RESETN = 0;
        adov6vc_r0D.REC_ADCDRI_EN = 0;
        adov6vc_r0D.REC_LNA_AMPEN = 0;
        adov6vc_r0D.REC_LNA_AMPSW_EN = 0;
        adov6vc_r0D.REC_LNA_OUTSHORT_EN = 0;
        adov6vc_r0D.REC_PGA_AMPEN = 0;
        adov6vc_r0D.REC_PGA_OUTSHORT_EN = 0;
        adov6vc_r0D.VAD_LNA_AMPSW_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);
        
        adov6vc_r13.VAD_PGA_OUTSHORT_EN = 0;
        adov6vc_r13.VAD_LNA_OUTSHORT_EN = 0;
        adov6vc_r13.LDO_PG_IREF = 0;
        adov6vc_r13.LDO_PG_VREF_0P6LP = 0;
        adov6vc_r13.LDO_PG_LDOCORE_0P6LP = 0;
        adov6vc_r13.LDO_PG_VREF_1P4LP = 0;
        adov6vc_r13.LDO_PG_LDOCORE_1P4LP = 0;
        adov6vc_r13.LDO_PG_VREF_1P4HP = 1;
        adov6vc_r13.LDO_PG_LDOCORE_1P4HP = 1;
        adov6vc_r13.LDO_PG_VREF_0P9HP = 1;
        adov6vc_r13.LDO_PG_LDOCORE_0P9HP = 1;
        mbus_remote_register_write(ADO_ADDR, 0x13, adov6vc_r13.as_int);

        adov6vc_r0F.VAD_ADC_RESET = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0F, adov6vc_r0F.as_int);

        adov6vc_r14.VAD_ADCDRI_EN = 1;
        adov6vc_r14.VAD_LNA_AMPEN = 1;
        adov6vc_r14.VAD_PGA_AMPEN = 1;
        adov6vc_r14.VAD_PGA_BWCON = 19;//13;
        adov6vc_r14.VAD_PGA_GCON = 4;//8;
        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vc_r14.as_int);

        ////VAD MON setting can come here
        //adov6vc_r1A.VAD_MONSEL = 1;
        //adov6vc_r1A.VAD_MONBUF_EN = 1;
        //adov6vc_r1A.VAD_REFMONBUF_EN = 0;
        //adov6vc_r1A.VAD_ADCDRI_OUT_OV_EN = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x1A, adov6vc_r1A.as_int);
        
        ////REC MON setting (debug)
        //adov6vc_r17.REC_MONSEL = 0;
        //adov6vc_r17.REC_MONBUF_EN = 0;
        //adov6vc_r17.REC_REFMONBUF_EN = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x17, adov6vc_r17.as_int);

        adov6vc_r10.DO_LP_HP_SEL = 0;    //0: LP, 1: HP
        adov6vc_r10.DIO_OUT_EN = 1;
        adov6vc_r10.VAD_ADC_DOUT_ISOL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x10, adov6vc_r10.as_int);

        adov6vc_r04.DSP_CLK_MON_SEL = 4; //LP CLK mon
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vc_r04.as_int);
    }
    else if(mode == 2){ // HP AFE
        *REG_XO_CONF1 = *REG_XO_CONF1 | (1 << 15);
        
        adov6vc_r13.LDO_PG_IREF = 0;
        adov6vc_r13.LDO_PG_VREF_0P6LP = 0;
        adov6vc_r13.LDO_PG_LDOCORE_0P6LP = 0;
        adov6vc_r13.LDO_PG_VREF_1P4LP = 0;
        adov6vc_r13.LDO_PG_LDOCORE_1P4LP = 0;
        adov6vc_r13.LDO_PG_VREF_1P4HP = 0;
        adov6vc_r13.LDO_PG_LDOCORE_1P4HP = 0;
        adov6vc_r13.LDO_PG_VREF_0P9HP = 0;
        adov6vc_r13.LDO_PG_LDOCORE_0P9HP = 0;
        mbus_remote_register_write(ADO_ADDR, 0x13, adov6vc_r13.as_int);

        adov6vc_r0D.REC_ADC_RESETN = 1;
        adov6vc_r0D.REC_ADCDRI_EN = 1;
        adov6vc_r0D.REC_LNA_AMPEN = 1;
        adov6vc_r0D.REC_LNA_AMPSW_EN = 1;
        adov6vc_r0D.REC_LNA_OUTSHORT_EN = 0;
        adov6vc_r0D.REC_PGA_AMPEN = 1;
        adov6vc_r0D.REC_PGA_OUTSHORT_EN = 0;
        adov6vc_r0D.VAD_LNA_AMPSW_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);
        delay(MBUS_DELAY*9);
        //adov6vc_r0D.REC_LNA_FSETTLE = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);

        adov6vc_r10.DO_LP_HP_SEL = 1;    //HP
        adov6vc_r10.DIO_OUT_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x10, adov6vc_r10.as_int);

        adov6vc_r04.DSP_CLK_MON_SEL = 2; //HP clock mon
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vc_r04.as_int);
    }
    else{       // AFE off
        *REG_XO_CONF1 = *REG_XO_CONF1 & ~(1<<15);

        adov6vc_r0D.REC_ADC_RESETN = 0;
        adov6vc_r0D.REC_ADCDRI_EN = 0;
        adov6vc_r0D.REC_LNA_AMPEN = 0;
        adov6vc_r0D.REC_LNA_AMPSW_EN = 0;
        adov6vc_r0D.REC_LNA_OUTSHORT_EN = 0;
        adov6vc_r0D.REC_PGA_AMPEN = 0;
        adov6vc_r0D.REC_PGA_OUTSHORT_EN = 0;
        adov6vc_r0D.VAD_LNA_AMPSW_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);
        
        adov6vc_r13.LDO_PG_IREF = 0;
        adov6vc_r13.LDO_PG_VREF_0P6LP = 1;
        adov6vc_r13.LDO_PG_LDOCORE_0P6LP = 1;
        adov6vc_r13.LDO_PG_VREF_1P4LP = 1;
        adov6vc_r13.LDO_PG_LDOCORE_1P4LP = 1;
        adov6vc_r13.LDO_PG_VREF_1P4HP = 1;
        adov6vc_r13.LDO_PG_LDOCORE_1P4HP = 1;
        adov6vc_r13.LDO_PG_VREF_0P9HP = 1;
        adov6vc_r13.LDO_PG_LDOCORE_0P9HP = 1;
        mbus_remote_register_write(ADO_ADDR, 0x13, adov6vc_r13.as_int);

        adov6vc_r0F.VAD_ADC_RESET = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0F, adov6vc_r0F.as_int);

        adov6vc_r14.VAD_ADCDRI_EN = 0;
        adov6vc_r14.VAD_LNA_AMPEN = 0;
        adov6vc_r14.VAD_PGA_AMPEN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vc_r14.as_int);

        adov6vc_r10.DIO_OUT_EN = 0;
        //adov6vc_r10.DIO_DIR_IN1OUT0 = 1;
        adov6vc_r10.VAD_ADC_DOUT_ISOL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x10, adov6vc_r10.as_int);
        ////VAD MON setting can come here
        //adov6vc_r1A.VAD_MONSEL = 1;
        //adov6vc_r1A.VAD_MONBUF_EN = 1;
        //adov6vc_r1A.VAD_REFMONBUF_EN = 0;
        //adov6vc_r1A.VAD_ADCDRI_OUT_OV_EN = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x1A, adov6vc_r1A.as_int);
        
        ////REC MON setting (debug)
        //adov6vc_r17.REC_MONSEL = 1;
        //adov6vc_r17.REC_MONBUF_EN = 1;
        //adov6vc_r17.REC_REFMONBUF_EN = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x17, adov6vc_r17.as_int);

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

inline static void flash_erasedata(void){
    FLASH_turn_on();
    FLASH_pp_ready();
    FLASH_turn_off();
    /////////////////////////////
}

//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sns_sleep_check(void){
    // Make sure LDO is off
    if (temp_running){
        temp_running = 0;
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

    // Freeze GPIO
    freeze_gpio_out();

    // Go to Sleep
    mbus_sleep_all();
    while(1);
}

static void operation_sleep_snt_timer(void){
    set_wakeup_timer(0,0,0);    //disable timer
    operation_sleep();

}

static void operation_sleep_noirqreset(void){
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
    //pmu_sar_conv_ratio_val_test_on = 0x4A;
    pmu_sar_conv_ratio_val_test_on = 0x34;//0x2D;
    pmu_sar_conv_ratio_val_test_off = 0x30;//0x2A;


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

    //prev18_r1C.SRAM0_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
    //prev18_r1C.SRAM0_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
    ////prev18_r1C.SRAM0_USE_INVERTER_SA= 1; // Default 0x0, 1bit
    //*REG_SRAM0_TUNE = prev18_r1C.as_int;
    *REG_SRAM0_TUNE = ((31 << 9)    //SRAM0_TUNE_ASO_DLY
                      |(2 << 5)     //SRAM0_TUNE_WL_WIDTH
                      |(15 << 1)    //SRAM0_TUNE_DECODER_DLY
                      | 0           //SRAM0_USE_INVERTER_SA
                      );

    //Enumerate & Initialize Registers
    Tstack_state = TSTK_IDLE;    //0x0;
    enumerated = ENUMID;
    exec_count = 0;
    exec_count_irq = 0;
    //PMU_ADC_3P0_VAL = 0x62;
    //pmu_parkinglot_mode = 3;

    //Enumeration
    mbus_enumerate(SNT_ADDR);
    //mbus_enumerate(ADO_ADDR);
    //mbus_enumerate(FLP_ADDR);
    //mbus_enumerate(PMU_ADDR);
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
            ( (1 << 20) // ignore state_horizon; default 1
              | (1 << 13) // ignore adc_output_ready; default 0
              | (1 << 12) // ignore adc_output_ready; default 0
              | (1 << 11) // ignore adc_output_ready; default 0
            ));
    delay(MBUS_DELAY);
    pmu_adc_reset_setting();
    delay(MBUS_DELAY);
    pmu_adc_disable();
    delay(MBUS_DELAY);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
    //WAKEUP_PERIOD_CONT_INIT = 3;   // 0x1E (30): ~1 min with PRCv9
    wakeup_data = 0;
    temp_running = 0;

    pmu_setting_state = PMU_25C;
    PMU_10C_threshold_sns = 600; // Around 10C
    PMU_20C_threshold_sns = 1000; // Around 20C
    PMU_35C_threshold_sns = 2000; // Around 35C
    PMU_55C_threshold_sns = 3200; // Around 55C

    SNT_0P5S_VAL = 1000;
    TEMP_CALIB_A = 240000;
    TEMP_CALIB_B = 3750000;


#ifdef DEBUG_MODE
    *REG_CPS = 0x5;   //both TEST VDD on [2]=1: Test 1.2V on,  [0]=1: Test 0.6V on
#else
    *REG_CPS = 0x0;   //both TEST VDD off
#endif
    delay(MBUS_DELAY*10);

    snt_initialization();   //SNT initialization
    flash_initialization(); //FLPv3L initialization
    ado_initialization(); //ADOv6VC initialization
    XO_init(); //XO initialization

    // Self boot. Offchip NMOS gate control via gpio[7]
    direction_gpio = 0x80; //set gpio[7] direction as output for selfboot support, the others are input
    gpio_set_data(0x80);  // gpio[7]=1 
    init_gpio();
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
#ifdef DEBUG_MBUS_MSG
    mbus_write_message32(0xE4,*SREG_WAKEUP_SOURCE); 
#endif    
    //[ 0] = GOC/EP
    //[ 1] = Wakeuptimer
    //[ 2] = XO timer
    //[ 3] = gpio_pad
    //[ 4] = mbus message
    //[ 8] = gpio[0]
    //[ 9] = gpio[1]
    //[10] = gpio[2]
    //[11] = gpio[3]

    ///////////////////////////////////////////////////////////////
    // example gpio trigger code                            //////
    if(((*SREG_WAKEUP_SOURCE >> 8) & 1) == 1){ //waked up by gpio[0]
        //Do something
        delay(MBUS_DELAY*100); //~1sec
    }
    else if(((*SREG_WAKEUP_SOURCE >> 9) & 1) == 1){ //waked up by gpio[1]
        //Do something
        delay(MBUS_DELAY*200); //~2sec
    }
    else if(((*SREG_WAKEUP_SOURCE >> 10) & 1) == 1){ //waked up by gpio[2]
        //Do something
        delay(MBUS_DELAY*400); //~4sec
    }
    else if(((*SREG_WAKEUP_SOURCE >> 11) & 1) == 1){ //waked up by gpio[3]
        //Do something
        delay(MBUS_DELAY*800); //~8sec
    }   
    ////////////////////////////////////////////////////////////////

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
#ifdef DEBUG_MBUS_MSG
    mbus_write_message32(0xE0, *REG0);
#endif    
    if(*REG0 == 1){ // LP -> HP mode change
        *EP_MODE = 1;
                
        adov6vc_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80A
       
        afe_set_mode(2);
        digital_set_mode(2);
    }
    else if(*REG0 == 0) { // HP -> ULP mode change
        *EP_MODE = 0;
        adov6vc_r0D.DSP_HP_FIFO_GO = 0;
        adov6vc_r0D.DSP_HP_ADO_GO = 0;
        adov6vc_r0D.DSP_HP_DNN_GO = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BCB5
        
        afe_set_mode(1);
        digital_set_mode(1);
        
        operation_sleep_notimer();
    }
#ifdef DEBUG_MBUS_MSG
    mbus_write_message32(0xE1, *EP_MODE);
#endif    
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
#ifdef DEBUG_MBUS_MSG
    mbus_write_message32(0xEE,  *GOC_DATA_IRQ);
#endif    

#ifdef DEBUG_MBUS_MSG
    mbus_write_message32(0xEE, 0x00000E2D);
#endif
}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    if (enumerated != ENUMID) operation_init();
    if(*EP_MODE != 1) init_gpio();      //when not in memory program mode
    
    *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_REG0);

    // Check if wakeup is due to GOC interrupt  
    // 0x8C is reserved for GOC-triggered wakeup (Named GOC_DATA_IRQ)
    // 8 MSB bits of the wakeup data are used for function ID
    wakeup_data = *GOC_DATA_IRQ;
    uint32_t data_cmd = (wakeup_data>>24) & 0xFF;
    uint32_t data_val0 = wakeup_data & 0xFF;
    uint32_t data_val1 = (wakeup_data>>8) & 0xFF;
    //uint32_t data_val2 = (wakeup_data>>16) & 0xFF;

    if(data_cmd == 0x01){       // Charge pump control
        if(data_val0==1){        //ON
            adov6vc_r14.CP_CLK_EN_1P2 = 1;
            adov6vc_r14.CP_CLK_DIV_1P2 = 0;
            adov6vc_r14.CP_VDOWN_1P2 = 0; //vin = V3P6
            mbus_remote_register_write(ADO_ADDR, 0x14, adov6vc_r14.as_int);
            delay(CP_DELAY); 
            adov6vc_r14.CP_VDOWN_1P2 = 1; //vin = gnd
            mbus_remote_register_write(ADO_ADDR, 0x14, adov6vc_r14.as_int);
        }
        else if(data_val0==2){        //ON
            adov6vc_r14.CP_CLK_EN_1P2 = 1;
            adov6vc_r14.CP_CLK_DIV_1P2 = 0;
            adov6vc_r14.CP_VDOWN_1P2 = 0; //vin = V3P6
            mbus_remote_register_write(ADO_ADDR, 0x14, adov6vc_r14.as_int);
            delay(CP_DELAY*0.2); 
            adov6vc_r14.CP_VDOWN_1P2 = 1; //vin = gnd
            mbus_remote_register_write(ADO_ADDR, 0x14, adov6vc_r14.as_int);
        }
        else if(data_val0 == 3){
            adov6vc_r14.CP_CLK_EN_1P2 = 1;
            adov6vc_r14.CP_CLK_DIV_1P2 = 0; //clk 8kHz
            adov6vc_r14.CP_VDOWN_1P2 = 0; //vin = V3P6
            mbus_remote_register_write(ADO_ADDR, 0x14, adov6vc_r14.as_int);
        }
        else if(data_val0 == 4){
            adov6vc_r14.CP_CLK_EN_1P2 = 1;
            adov6vc_r14.CP_CLK_DIV_1P2 = 0; //clk 8kHz
            adov6vc_r14.CP_VDOWN_1P2 = 1; //vin = gnd
            mbus_remote_register_write(ADO_ADDR, 0x14, adov6vc_r14.as_int);
        }
        else{                   //OFF
            adov6vc_r14.CP_CLK_EN_1P2 = 0;
            adov6vc_r14.CP_CLK_DIV_1P2 = 3;
            mbus_remote_register_write(ADO_ADDR, 0x14, adov6vc_r14.as_int);
            //flash_erasedata();
        }
    }
    else if(data_cmd == 0x02){  // SRAM Programming mode
        if(data_val0==1){    //Enter
            //adov6vc_r0E.DSP_DNN_DBG_MODE_EN = 1;
            //mbus_remote_register_write(ADO_ADDR, 0x0E, adov6vc_r0E.as_int);//000054
            mbus_remote_register_write(ADO_ADDR, 0x0E, 
                    ((1<<7)     //DSP_WAKEUP_REQ_EN
                    |(2<<5)     //LC_CLK_DIV
                    |(2<<3)     //LC_CLK_RING
                    |(1<<2)     //DSP_DNN_DBG_MODE_EN ->1
                    ));
            adov6vc_r0D.DSP_DNN_CTRL_RF_SEL = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80B
            adov6vc_r0D.DSP_DNN_CLKENB_RF_SEL = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80B
            adov6vc_r0D.DSP_DNN_PG_SLEEP_RF = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B809
            adov6vc_r0D.DSP_DNN_ISOLATEN_RF = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80D
            adov6vc_r0D.DSP_DNN_CLKENB_RF = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B805
            adov6vc_r0D.DSP_DNN_RESETN_RF = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B815
        }
        else{               //Exit
            adov6vc_r0D.DSP_DNN_RESETN_RF = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B805
            adov6vc_r0D.DSP_DNN_CLKENB_RF = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80D
            adov6vc_r0D.DSP_DNN_ISOLATEN_RF = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B809
            adov6vc_r0D.DSP_DNN_PG_SLEEP_RF = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80B
            adov6vc_r0D.DSP_DNN_CLKENB_RF_SEL = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80B
            adov6vc_r0D.DSP_DNN_CTRL_RF_SEL = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03B80A
            //adov6vc_r0E.DSP_DNN_DBG_MODE_EN = 0;
            //mbus_remote_register_write(ADO_ADDR, 0x0E, adov6vc_r0E.as_int);//000050
            mbus_remote_register_write(ADO_ADDR, 0x0E, 
                    ((1<<7)     //DSP_WAKEUP_REQ_EN
                    |(2<<5)     //LC_CLK_DIV
                    |(2<<3)     //LC_CLK_RING
                    |(0<<2)     //DSP_DNN_DBG_MODE_EN ->0
                    ));

        }
    }
    else if(data_cmd == 0x03){  // DSP Monitoring Configure
        //adov6vc_r0E.DSP_MIX_MON_EN = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x0E, adov6vc_r0E.as_int);//000150
        //adov6vc_r00.DSP_FE_SEL_EXT = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x00, adov6vc_r00.as_int);//9F1B28
        adov6vc_r04.DSP_P2S_MON_EN = 1;
        adov6vc_r04.DSP_CLK_MON_SEL = 4;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vc_r04.as_int);//A00040
        adov6vc_r04.DSP_P2S_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vc_r04.as_int);//E00040
        //adov6vc_r10.DIO_OUT_EN = 1;
        //adov6vc_r10.DIO_IN_EN = 0;
        //adov6vc_r10.DIO_DIR_IN1OUT0 = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x10, adov6vc_r10.as_int);//000008
    }
    else if(data_cmd == 0x04){  // DSP LP Control
        if(data_val0==1){    //Go
            adov6vc_r0D.DSP_LP_RESETN = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC4A
        }
        else{               //Stop
            adov6vc_r0D.DSP_LP_RESETN = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vc_r0D.as_int);//03BC0A
        }
    }
    else if(data_cmd == 0x05){  // AFE Control
        afe_set_mode(data_val0);
        if(data_val0 == 2) delay(MBUS_DELAY*1000); //~10sec, AFE HP test purpose
    }
    else if(data_cmd == 0x06){  // Compression + Flash Test
        if(data_val0==1){    //Go
            comp_stream();
        }
        else if(data_val0==2){
            FLASH_read();
        }
    }
    else if(data_cmd == 0x07){ //Power gate control
        if(data_val0 == 0) { //both TEST VDD off
            *REG_CPS = 0;
            pmu_set_sar_override(pmu_sar_conv_ratio_val_test_off);
            pmu_set_sleep_clk(0xA,0xA,0xF,0xA);
            pmu_set_active_clk(0xA,0x4,0x10,0x4);
        }
        else if(data_val0 == 2){ //both TEST VDD on
            pmu_set_sar_override(pmu_sar_conv_ratio_val_test_on);
            pmu_set_sleep_clk(0xA,0x1,0x10,0x2);
            pmu_set_active_clk(0xA,0x1,0x10,0x2);
            *REG_CPS = 0x5; 
        }
        else if(data_val0 == 3){ //both TEST VDD on, active mode
            pmu_set_sar_override(pmu_sar_conv_ratio_val_test_on);
            pmu_set_sleep_clk(0xA,0x4,0x10,0x4);
            pmu_set_active_clk(0xA,0x4,0x10,0x4);
            *REG_CPS = 0x5; 
        }
        //*REG_CPS = 0x7 & data_val0;
    }
    else if(data_cmd == 0x08){ //Power gate control
        *REG_CPS = 0x7 & data_val0;
    }
    else if(data_cmd == 0x09){
        if(data_val0 != 0xF) pmu_set_sleep_clk(data_val0+1,data_val0,data_val1,data_val0);
        else pmu_set_sleep_clk(data_val0,data_val0,data_val1,data_val0);
    }
    else if(data_cmd == 0x0A){
        if(data_val0 != 0xF) pmu_set_active_clk(data_val0+1,data_val0,data_val1,data_val0);
        else pmu_set_active_clk(data_val0,data_val0,data_val1,data_val0);
        delay(MBUS_DELAY*300); //~3sec
    }
    else if(data_cmd == 0x0B){
        pmu_set_sar_override(data_val0);
    }
    else if(data_cmd == 0x10){  //GPIO direction, trigger
        direction_gpio = data_val0 | 0x80; // input:0, output:1,  eg. 0xF0 set GPIO[7:4] as output, GPIO[3:0] as input 
        init_gpio();
        config_gpio_posedge_wirq((data_val1>>4)&0xF);
        config_gpio_negedge_wirq(data_val1&0xF);
    }
    else if(data_cmd == 0x11){ //GPIO set (only for pins set as output)
        gpio_set_data( data_val0 | 0x80 );   // GPIO[7] should be always 1 for self-boot
        gpio_write_current_data();
        delay(MBUS_DELAY*100);      //~1sec delay
        gpio_set_data( data_val1 | 0x80 );   // since data is overwritten here
        mbus_write_message32(0xE5, *GPIO_DATA);
        gpio_write_current_data();  // and GPIO out values change here
        mbus_write_message32(0xE5, *GPIO_DATA);
    }
    else if(data_cmd == 0x21){  //SNT run temperature sensor
        // Slow down PMU sleep osc and run temp sensor code with desired wakeup period
        // wakeup_data[15:0] is the desired wakeup period in 0.5s (min 0.5s: 0x1, max 32767.5s: 0xFFFF)
        // wakeup_data[20] enables radio tx for each measurement
        // wakeup_data[21] specifies which wakeup timer to use 0: SNT timer, 1: PRC timer
        // wakeup_data[23] is debug mode: logs the execution count as opposed to temp data
        
        //radio_tx_option = wakeup_data_field_2 & 0x10;
        //wakeup_timer_option = data_val2 & 0x20;
        //temp_storage_debug =  data_val2 & 0x80;
        
        //if (wakeup_timer_option){
        //    // Use PRC timer
        //    WAKEUP_PERIOD_CONT_USER = wakeup_data & 0xFFFF; // Unit is 1s
        //}else{
        //    // Use SNT timer
            WAKEUP_PERIOD_CONT_USER = (wakeup_data & 0xFFFF)*SNT_0P5S_VAL; // Unit is 0.5s
            mbus_write_message32(0xAA, 0x00001000);
        //}
        run_tempsensor();

    }
    else if(data_cmd == 0x22){
        // Stop temp sensor program and transmit the battery reading and execution count (alternating n times)
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 

        operation_sns_sleep_check();
        snt_stop_timer();

        Tstack_state = TSTK_IDLE;

    }

    // Initialization sequence
    if (*EP_MODE == 1) {
        while (1) config_timerwd(TIMERWD_VAL);
    }
    else {
        operation_sleep_notimer();
    }
    return 0;
}
