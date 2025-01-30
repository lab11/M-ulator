//*******************************************************************
//Author: Seok Hyeon Jeong / Minchang Cho
//Originally written by Sehcang Oh
//Description: Leidos Trestle Gen7.1 
//    v1.0: created. Modified from 'Leidos_dorado_Gen3' 12/14/2018
//    v1a02: GPIO functions                             03/19/2019
//    v2a03: 2.5V VBAT, self-booting support            08/14/2019
//    v3.0: Updated GOC trigger                         12/19/2019
//              -PMU individual sleep clk setting
//              -XO control
//              -DSP monitoring configurability
//          Updated PRE, PMU and ADO to latest versions (PREv22,PMUv9,ADOv6VB)
//    v3.1: Modified FLASH_read() function              04/07/2020
//		- Change has been made for flash readout inspection
//    v3.2: Modified afe_set_mode() function            04/25/2020
//		- Added delay between LDO un-power gating and ADC reset relase to provide enough stbilization time
//    v3.3: Modified FLASH_read() function              04/29/2020
//		- Increased FLASH>SRAM data copy and data being read
//    v4.0: Include SNT for temperature monitoring      05/12/2020
//      - Based on PREv22E
//      - Periodic PMU adustment based on temperature reading
//      - Removed Powergate PMU control debug commands (cmd=0x07) to save code space
//    v4.1: Enabled wakeup on pending request 	    	05/18/2020
//    v5.0: Adjusted PMU setting for T2Bs    	    	06/15/2020
//    v5.1: Adjusted Charge Pump start-up sequence	07/20/2020
//    v6.1: New system (T3A) using CIS chip		03/13/2021
//	- New MBUS loop sequence
//	- All IRQ handling is done in the main function (PREv22 bug)
//	- Uses new PMU self-adjustment functions to adjust PMU strength across different temperature
//	- Uses ADOv7
//	- Uses pmu_reg_write function instead of mubs_remote_regiester_write + mbus_delay to save energy
//    v6.2: New system (T3A) using CIS chip		09/18/2021
//	- Removed unnecessary Flash Erase
//	- Adjusted PMU floor setting for slow loop
//	- Time for running AFE HP mode is now configurable (for audio test)
//    v6.3: New system (T3A) using CIS chip		09/28/2021
//	- Fixed flash issue
//    v6.4: New system (T3A) using CIS chip		10/17/2021
//	- Adjust HP mode transition for fast settling (Line 1413-1465)
//	- Adjusted PMU feedback speed in active and sleep (Line 614, 694, 725)
//	- Lowered PGA gain from 31 to 4 (Line 1107, 1389)
//	- Added EP trigger for PGA gain control (debug purpose / Line 2187-2790)
//    v7.1: New system (T4) using CISv2C+ADOv6vB chip		10/27/2021
//    v8.1: New system (T5) using CISv3B+ADOv6vB chip		10/27/2021
//	- gpio_set_data() + gpio_write_current_data() merged to gpio_write_data()
//	- GOC EP is handled by the ISR (i.e. moved from main function to interrupt handler) / irq_pending removed
//	- Added variable for XO start up (XO_WAIT_A, XO_WAIT_B)
//	- Chip version change
//		PREv20E	> PREv22E		 
//		PMUv12C > PMUv13
//		SNTv4	> SNTv5
//		ADOv7 	> ADOv6VB
//	- List of updated/added functions compared to v6.4
//		+ operation_init	(updated)
//		+ operation_sleep	(updated)
//		+ pmu_set_clk_init	(updated)
//		+ pmu_set_sleep_clks	(updated)
//		+ pmu_adc_disable	(updated)
//		+ pmu_set_sar_override	(updated)
//		+ init_gpio		(updated)
//		+ XO_init		(updated)
//		+ xo_start		(new)
//		+ xo_stop 		(new)
//		+ ado_initialization	(updated)
//		+ afe_set_mode		(updated)
//		+ comp_stream		(updated)
//		+ handler_ext_int_gocep	(updated)
//		+ handler_ext_int_reg0	(updated)
//		+ main 			(updated)
//
//
//*******************************************************************
#include "PREv22E.h"
#include "ADOv6VB_RF.h"
#include "PMUv13_RF.h"
#include "mbus.h"
#include "SNTv5_RF.h"
  
#define ENUMID 0xDEADBEEF

#include "DFT_LUT.txt"
#include "DCT_PHS.txt"

////////////////////////////////////////
// uncomment this for debug 
//#define DEBUG_MBUS_MSG      // Debug MBus message
/////////////////////////////////////////
#define SNT_ADDR 0x3
#define ADO_ADDR 0x4
#define FLP_ADDR 0x5
#define PMU_ADDR 0x6
#define MRR_ADDR 0x7
#define WAKEUP_PERIOD_PARKING 30000 // About 2 hours (PRCv17)

// System parameters
#define MBUS_DELAY 100 // Amount of delay between successive messages; 100: ~9ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)

// Tstack states
#define    PMU_m10C 0x1
#define    PMU_0C 0x2
#define    PMU_10C 0x3
#define    PMU_20C 0x4
#define    PMU_25C 0x5
#define    PMU_35C 0x6

#define NUM_TEMP_MEAS 1

// XO Initialization Wait Duration
#define XO_WAIT_A  50000    // Must be ~1 second delay. Delay for XO Start-Up. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
#define XO_WAIT_B  50000    // Must be ~1 second delay. Delay for VLDO & IBIAS Generation. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).

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
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t stack_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t error_code;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint32_t wakeup_period_count;
volatile uint32_t wakeup_timer_multiplier;
volatile uint32_t pmu_setting_state;
volatile uint32_t pmu_setting_prev;
volatile uint32_t read_data_batadc;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_SNT; 

volatile uint32_t PMU_m10C_threshold_sns;
volatile uint32_t PMU_0C_threshold_sns;
volatile uint32_t PMU_10C_threshold_sns;
volatile uint32_t PMU_20C_threshold_sns;
volatile uint32_t PMU_35C_threshold_sns;

volatile uint32_t temp_storage_latest = 150; // SNSv10
volatile uint32_t temp_storage_last_wakeup_adjust = 150; // SNSv10
volatile uint32_t temp_storage_diff = 0;
volatile uint32_t temp_storage_count;
volatile uint32_t temp_storage_debug;
volatile uint32_t sns_running;
volatile uint32_t sns_done;
volatile uint32_t read_data_temp; // [23:0] Temp Sensor D Out
volatile uint32_t TEMP_CALIB_A;
volatile uint32_t TEMP_CALIB_B;
volatile uint32_t SNT_0P5S_VAL;
uint32_t xo_count;
uint8_t mode_select;

volatile uint32_t snt_wup_counter_cur;
volatile uint32_t snt_timer_enabled = 0;

volatile sntv5_r00_t sntv5_r00 = SNTv5_R00_DEFAULT;
volatile sntv5_r01_t sntv5_r01 = SNTv5_R01_DEFAULT;
volatile sntv5_r03_t sntv5_r03 = SNTv5_R03_DEFAULT;
volatile sntv5_r07_t sntv5_r07 = SNTv5_R07_DEFAULT;
volatile sntv5_r08_t sntv5_r08 = SNTv5_R08_DEFAULT;
volatile sntv5_r09_t sntv5_r09 = SNTv5_R09_DEFAULT;
volatile sntv5_r0A_t sntv5_r0A = SNTv5_R0A_DEFAULT;
volatile sntv5_r0B_t sntv5_r0B = SNTv5_R0B_DEFAULT;
volatile sntv5_r17_t sntv5_r17 = SNTv5_R17_DEFAULT;

volatile prev22e_r0B_t prev22e_r0B = PREv22E_R0B_DEFAULT;
volatile prev22e_r19_t prev22e_r19 = PREv22E_R19_DEFAULT;
volatile prev22e_r1A_t prev22e_r1A = PREv22E_R1A_DEFAULT;
volatile prev22e_r1B_t prev22e_r1B = PREv22E_R1B_DEFAULT;
volatile prev22e_r1C_t prev22e_r1C = PREv22E_R1C_DEFAULT;

volatile adov6vb_r00_t adov6vb_r00 = ADOv6VB_R00_DEFAULT;
volatile adov6vb_r04_t adov6vb_r04 = ADOv6VB_R04_DEFAULT;
volatile adov6vb_r07_t adov6vb_r07 = ADOv6VB_R07_DEFAULT;
volatile adov6vb_r0B_t adov6vb_r0B = ADOv6VB_R0B_DEFAULT;
volatile adov6vb_r0D_t adov6vb_r0D = ADOv6VB_R0D_DEFAULT;
volatile adov6vb_r0E_t adov6vb_r0E = ADOv6VB_R0E_DEFAULT;
volatile adov6vb_r0F_t adov6vb_r0F = ADOv6VB_R0F_DEFAULT;
volatile adov6vb_r10_t adov6vb_r10 = ADOv6VB_R10_DEFAULT;
volatile adov6vb_r11_t adov6vb_r11 = ADOv6VB_R11_DEFAULT;
volatile adov6vb_r12_t adov6vb_r12 = ADOv6VB_R12_DEFAULT;
volatile adov6vb_r13_t adov6vb_r13 = ADOv6VB_R13_DEFAULT;
volatile adov6vb_r14_t adov6vb_r14 = ADOv6VB_R14_DEFAULT;
volatile adov6vb_r15_t adov6vb_r15 = ADOv6VB_R15_DEFAULT;
volatile adov6vb_r16_t adov6vb_r16 = ADOv6VB_R16_DEFAULT;
volatile adov6vb_r17_t adov6vb_r17 = ADOv6VB_R17_DEFAULT;
volatile adov6vb_r18_t adov6vb_r18 = ADOv6VB_R18_DEFAULT;
volatile adov6vb_r19_t adov6vb_r19 = ADOv6VB_R19_DEFAULT;
volatile adov6vb_r1A_t adov6vb_r1A = ADOv6VB_R1A_DEFAULT;
volatile adov6vb_r1B_t adov6vb_r1B = ADOv6VB_R1B_DEFAULT;
volatile adov6vb_r1C_t adov6vb_r1C = ADOv6VB_R1C_DEFAULT;
        
uint32_t read_data[100];
volatile uint8_t direction_gpio;


//***************************************************
// Sleep Functions
//***************************************************
//static void operation_sns_sleep_check(void);
static void operation_sleep(void);
static void operation_sleep_notimer(void); 
//static void operation_sleep_xo_timer(void); 
static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1P2);
static void pmu_set_sleep_clks(uint8_t r, uint8_t l, uint8_t base, uint8_t select);

static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);


//*******************************************************************
// XO Functions
//*******************************************************************
static void xo_start(void) {
    //--------------------------------------------------------------------------
    // XO Driver (XO_DRV_V3_TSMC180) Start-Up Sequence
    //--------------------------------------------------------------------------
    // RESETn       __|*********************************************************
    // PGb_StartUp  __|***************************|_____________________________
    // START_UP     **************************|_________________________________
    // ISOL_CLK_HP  **********|_________________|*******************************
    // ISOL_CLK_LP  ******************|_________________________________________
    //                |<--A-->|<--B-->|<--C-->|.|.|<-- Low Power Operation -->
    //--------------------------------------------------------------------------
    // A: ~1s  (XO Start-Up): NOTE: You may need more time here due to the weak power-gate switch.
    // B: ~1s  (VLDO & IBIAS generation)
    // C: <1ms (SCN Output Generation)
    // .(dot): minimum delay
    //--------------------------------------------------------------------------

    prev22e_r19.XO_RESETn       = 1;
    prev22e_r19.XO_PGb_START_UP = 1;
    *REG_XO_CONF1 = prev22e_r19.as_int;

    delay(XO_WAIT_A); // Delay A (~1s; XO Start-Up)

    prev22e_r19.XO_ISOL_CLK_HP = 0;
    *REG_XO_CONF1 = prev22e_r19.as_int;
    
    delay(XO_WAIT_B); // Delay B (~1s; VLDO & IBIAS generation)

    prev22e_r19.XO_ISOL_CLK_LP = 0;
    *REG_XO_CONF1 = prev22e_r19.as_int;
    
    delay(100); // Delay C (~1ms; SCN Output Generation)

    prev22e_r19.XO_START_UP = 0;
    *REG_XO_CONF1 = prev22e_r19.as_int;

    prev22e_r19.XO_ISOL_CLK_HP = 1;
    *REG_XO_CONF1 = prev22e_r19.as_int;

    prev22e_r19.XO_PGb_START_UP = 0;
    *REG_XO_CONF1 = prev22e_r19.as_int;

    delay(100); // Dummy Delay

    enable_xo_timer();
    start_xo_cout();
}

static void xo_stop( void ) {
    // Stop the XO Driver
    prev22e_r19.XO_ISOL_CLK_LP = 1;
    *REG_XO_CONF1 = prev22e_r19.as_int;

    prev22e_r19.XO_RESETn   = 0;
    prev22e_r19.XO_START_UP = 1;
    *REG_XO_CONF1 = prev22e_r19.as_int;
}

static void XO_init(void) {
    prev22e_r19.XO_SEL_VLDO = 0x0;	// Default value: 3'h0
    prev22e_r19.XO_CAP_TUNE = 0x0;	// Default value: 3'h0
    prev22e_r19.XO_SEL_vV0P6 = 0x0;	// Default value: 2'h2
    *REG_XO_CONF1 = prev22e_r19.as_int;

    prev22e_r1A.XO_SEL_CLK_OUT_DIV = 0x4;
    prev22e_r1A.XO_SEL_DLY = 0x1; 	// Default value: 3'h1
    prev22e_r1A.XO_INJ = 0x0; 		// Default value: 2'h2
    *REG_XO_CONF2 = prev22e_r1A.as_int;

    xo_start();
    
#ifdef DEBUG_MBUS_MSG
    mbus_write_message32(0xE3,0x00000E2D); 
#endif    
}


//************************************
// GPIO Functions
// ***********************************
static void init_gpio(void){
    gpio_set_dir (direction_gpio);  // input:0, output:1
    set_gpio_pad (0xFF); // 0xFF activate all 8 GPIO bits
    unfreeze_gpio_out();
}



//************************************
// PMU Functions
//************************************
static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
    set_halt_until_mbus_trx();
    mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
    set_halt_until_mbus_tx();
}

static void pmu_set_sar_override(uint32_t val){
    //---------------------------------------------------------------------------------------
    // SAR_RATIO_OVERRIDE
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x05, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0x3 << 14)   // 0x0      // 0x3: Perform sar_reset followed by sar/upc/dnc_stabilized upon writing into this register
                                    // 0x2: Perform sar_reset upon writing into this register
                                    // 0x1: Perform sar/upc/dnc_stabilized upon writing into this register
                                    // 0x0: Do nothing
        | (0x2 << 12)   // 0x0      // 0x3: Let VDD_CLK always connected to VBAT; 
                                    // 0x2: Let VDD_CLK always connected to V1P2;
                                    // 0x0: Let PMU handle.
        | (0x2 << 10)   // 0x2      // 0x3: Enable the periodic SAR reset; 
                                    // 0x2: Disable the periodic SAR reset; 
                                    // 0x0: Let PMU handle.
                                    // This setting is temporarily ignored when [15] is triggered.
        | (0x0 << 8 )   // 0x0      // 0x3: Let the SAR do UP-conversion (use if VBAT < 1.2V)
                                    // 0x2: Let the SAR do DOWN-conversion (use if VBAT > 1.2V)
                                    // 0x0: Let PMU handle.
        | (0x1 << 7 )   // 0x0      // If 1, override SAR ratio with [6:0].
        | (val) 	// 0x00     // SAR ratio for overriding (valid only when [7]=1)
    ));

}

//************************************
// pmu_prep_sar_ratio()
//************************************

void pmu_prep_sar_ratio (void) {

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

//************************************
// pmu_set_sar_ratio()
//************************************

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
    uint32_t adc_offset = (*REG3 & 0x007F00) >> 8;

    // Debug
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x01 << 24) | *REG3));
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
        mbus_write_message32(0xD8, ((0x02 << 24) | *REG3));
    #endif

    // Set the desired SAR Ratio
    pmu_reg_write (10, // 0x0A (SAR_RATIO_MINIMUM)
       ( (sar_ratio << 0)    // SAR_RATIO_MINIMUM (7-bits)
    ));

    // Debug
    #ifdef DEBUG_SET_SAR_RATIO
        mbus_write_message32(0xD8, ((0x03 << 24) | *REG3));
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
        mbus_write_message32(0xD8, ((0x04 << 24) | *REG3));
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
        mbus_write_message32(0xD8, ((0x05 << 24) | *REG3));
    #endif

    // Now we don't know how long it would take to set the sar ratio.
    // so let's keep checking the actual sar ratio until it becomes as same as SAR_RATIO_MINIMUM
    uint32_t pmu_sar_ratio;
    do {
        // Read the current SAR RATIO
        pmu_reg_write(0x00,0x04);
        pmu_sar_ratio   = *REG3 & 0x7F;

        // Debug
        #ifdef DEBUG_SET_SAR_RATIO
            mbus_write_message32(0xD8, ((0x06 << 24) | *REG3));
        #endif
            mbus_write_message32(0xD8, ((0x06 << 24) | *REG3));
            mbus_write_message32(0xD8, ((0x07 << 24) | pmu_sar_ratio));
            mbus_write_message32(0xD8, ((0x08 << 24) | sar_ratio));

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
        mbus_write_message32(0xD8, ((0x07 << 24) | *REG3));
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
        mbus_write_message32(0xD8, ((0x08 << 24) | *REG3));
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
        mbus_write_message32(0xD8, ((0x09 << 24) | *REG3));
    #endif

}

inline static void pmu_set_adc_period(uint32_t val){
    // PMU_CONTROLLER_DESIRED_STATE Active
    pmu_reg_write (60, //0x3C
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

    // Register 0x36: TICK_REPEAT_VBAT_ADJUST
    pmu_reg_write(54,val); 

    // Register 0x33: TICK_ADC_RESET
    pmu_reg_write(51,2);

    // Register 0x34: TICK_ADC_CLK
    pmu_reg_write(52,2);

    // PMU_CONTROLLER_DESIRED_STATE Active
    pmu_reg_write (60, //0x3C
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
}


inline static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

    // Register 0x16: V1P2 Active 
    pmu_reg_write(22, //0x16 
            ( (0 << 19) // Enable PFM even during periodic reset
              | (0 << 18) // Enable PFM even when Vref is not used as ref
              | (0 << 17) // Enable PFM
              | (3 << 14) // Comparator clock division ratio
              | (0 << 13) // Enable main feedback loop
              | (r << 9)  // Frequency multiplier R
              | (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
              | (base)      // Floor frequency base (0-63)
            ));

    // Register 0x18: V3P6 Active 
    pmu_reg_write(24, //0x18
            ( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
              | (0 << 13) // Enable main feedback loop
              | (r << 9)  // Frequency multiplier R
              | (l << 5)  // Frequency multiplier L (actually L+1)
              | (base)      // Floor frequency base (0-63)
            ));
    
    // Register 0x1A: V0P6 Active
    pmu_reg_write(26, //0x1A
            ( (0 << 13) // Enable main feedback loop
              | (r << 9)  // Frequency multiplier R
              | (l << 5)  // Frequency multiplier L (actually L+1)
              | (base)      // Floor frequency base (0-63)
            ));
}

inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

	// Register 0x17: V3P6 Sleep
    pmu_reg_write(23, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));

	// Register 0x15: V1P2 Sleep
    pmu_reg_write(21, 
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
    pmu_reg_write(25, 
		( (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));

}

inline static void pmu_set_sleep_clks(uint8_t r, uint8_t l, uint8_t base, uint8_t select){
   //mbus_write_message32(0xE1, r);
   //mbus_write_message32(0xE2, l);
   //mbus_write_message32(0xE3, base);
   //mbus_write_message32(0xE4, select);

    // Register 0x17: V3P6 Sleep
    if(select == 0x1){
        pmu_reg_write(23, 
                ( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
                  | (0 << 13) // Enable main feedback loop
                  | (r << 9)  // Frequency multiplier R
                  | (l << 5)  // Frequency multiplier L (actually L+1)
                  | (base)      // Floor frequency base (0-63)
                ));

    // Register 0x15: V1P2 Sleep
    }else if (select == 0x2){
        pmu_reg_write(21, 
                ( (0 << 19) // Enable PFM even during periodic reset
                  | (0 << 18) // Enable PFM even when Vref is not used as ref
                  | (0 << 17) // Enable PFM
                  | (1 << 14) // Comparator clock division ratio
                  | (0 << 13) // Enable main feedback loop
                  | (r << 9)  // Frequency multiplier R
                  | (l << 5)  // Frequency multiplier L (actually L+1)
                  | (base)      // Floor frequency base (0-63)
                ));

    // Register 0x19: V0P6 Sleep
    }else if(select == 0x3){
        pmu_reg_write(25,
                ( (0 << 13) // Enable main feedback loop
                  | (r << 9)  // Frequency multiplier R
                  | (l << 5)  // Frequency multiplier L (actually L+1)
                  | (base)      // Floor frequency base (0-63)
                ));
    }
    else{
        // Register 0x17: V3P6 Sleep
        pmu_reg_write(23, 
                ( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
                  | (0 << 13) // Enable main feedback loop
                  | (r << 9)  // Frequency multiplier R
                  | (l << 5)  // Frequency multiplier L (actually L+1)
                  | (base)      // Floor frequency base (0-63)
                ));
        
        // Register 0x15: V1P2 Sleep
        pmu_reg_write(21, 
                ( (0 << 19) // Enable PFM even during periodic reset
                  | (0 << 18) // Enable PFM even when Vref is not used as ref
                  | (0 << 17) // Enable PFM
                  | (3 << 14) // Comparator clock division ratio
                  | (0 << 13) // Enable main feedback loop
                  | (r << 9)  // Frequency multiplier R
                  | (l << 5)  // Frequency multiplier L (actually L+1)
                  | (base)      // Floor frequency base (0-63)
                ));

        // Register 0x19: V0P6 Sleep
        pmu_reg_write(25,
                ( (0 << 13) // Enable main feedback loop
                  | (r << 9)  // Frequency multiplier R
                  | (l << 5)  // Frequency multiplier L (actually L+1)
                  | (base)      // Floor frequency base (0-63)
                ));
    }
}

//inline static void pmu_set_sleep_tsns(){
//    if (pmu_setting_state >= PMU_35C){
//        pmu_set_active_clk(0x5,0xA,0x5,0xF/*V1P2*/);
//
//    }else if (pmu_setting_state < PMU_20C){
//        pmu_set_active_clk(0xF,0xA,0x7,0xF/*V1P2*/);
//
//    }else{ // 25C, default
//    	pmu_set_sleep_clks(0xF,0xA,0x5,0xF/*V1P2*/);
//    }
//}
//
//inline static void pmu_sleep_setting_temp_based(){
//	// FIXME: needs to be retested
//    
//    if (pmu_setting_state == PMU_35C){
//        mbus_write_message32(0xCE, 0x00000035);
//		pmu_set_sleep_clks(0x1,0x1,0x4,0x1/*V1P2*/);
//		pmu_set_sleep_clks(0x3,0x2,0x4,0x2/*V1P2*/);
//		pmu_set_sleep_clks(0x2,0x2,0x4,0x3/*V1P2*/);
//
//    }else if (pmu_setting_state == PMU_20C){
//        mbus_write_message32(0xCE, 0x00000020);
//		pmu_set_sleep_clks(0x2,0x2,0x4,0x1/*V1P2*/);
//		pmu_set_sleep_clks(0x6,0x6,0x4,0x2/*V1P2*/);
//		pmu_set_sleep_clks(0x6,0x5,0x4,0x3/*V1P2*/);
//
//    }else if (pmu_setting_state == PMU_10C){
//        mbus_write_message32(0xCE, 0x00000010);
//		pmu_set_sleep_clks(0x4,0x4,0x4,0x1/*V1P2*/);
//		pmu_set_sleep_clks(0xA,0xA,0x4,0x2/*V1P2*/);
//		pmu_set_sleep_clks(0x9,0xA,0x4,0x3/*V1P2*/);
//
//    }else if (pmu_setting_state == PMU_0C){
//        mbus_write_message32(0xCE, 0x00000000);
//		pmu_set_sleep_clks(0x2,0x2,0x8,0x1/*V1P2*/);
//		pmu_set_sleep_clks(0x5,0x5,0x8,0x2/*V1P2*/);
//		pmu_set_sleep_clks(0x4,0x5,0x8,0x3/*V1P2*/);
//
//    }else if (pmu_setting_state == PMU_m10C){
//        mbus_write_message32(0xCE, 0x00000F10);
//		pmu_set_sleep_clks(0x6,0x6,0x8,0x1/*V1P2*/);
//		pmu_set_sleep_clks(0xA,0xA,0x8,0x2/*V1P2*/);
//		pmu_set_sleep_clks(0xA,0x9,0x8,0x3/*V1P2*/);
//
//    }else{ // 25C, default
//        mbus_write_message32(0xCE, 0x00000025);
//		pmu_set_sleep_clks(0x2,0x1,0x4,0x1/*V1P2*/);
//		pmu_set_sleep_clks(0x5,0x4,0x4,0x2/*V1P2*/);
//		pmu_set_sleep_clks(0x4,0x4,0x4,0x3/*V1P2*/);
//    }
//}
//
//inline static void pmu_active_setting_temp_based(){
//	// FIXME: needs to be retested
//    if (pmu_setting_prev == PMU_m10C){
//        pmu_set_sar_ratio(0x4F);
//    }
//
//    if (pmu_setting_state == PMU_35C){
//        mbus_write_message32(0xCD, 0x00000035);
//	    pmu_set_active_clk(0x8,0x7,0x0F,0x8/*V1P2*/);
//
//    }else if (pmu_setting_state == PMU_20C){
//        mbus_write_message32(0xCD, 0x00000020);
//	    pmu_set_active_clk(0x5,0x4,0x10,0x5/*V1P2*/);
//
//    }else if (pmu_setting_state == PMU_10C){
//        mbus_write_message32(0xCD, 0x00000010);
//	    pmu_set_active_clk(0x9,0x8,0x10,0x9/*V1P2*/);
//
//    }else if (pmu_setting_state == PMU_0C){
//        mbus_write_message32(0xCD, 0x00000000);
//	    pmu_set_active_clk(0xE,0xD,0x18,0xE/*V1P2*/);
//
//    }else if (pmu_setting_state == PMU_m10C){
//        pmu_set_sar_ratio(0x50);
//        mbus_write_message32(0xCD, 0x00000F10);
//		pmu_set_active_clk(0xE,0xE,0x1F,0xE/*V1P2*/);
//
//    }else{ // 25C, default
//        mbus_write_message32(0xCD, 0x00000025);
//		pmu_set_active_clk(0x4,0x3,0x10,0x4/*V1P2*/);
//    }
//
//}

inline static void pmu_set_clk_init(){
    pmu_set_active_clk(0xA,0x4,0x10,0x4);
    pmu_set_sleep_clks(0xA,0x4,0x10,0x0); //with TEST1P2

    // SAR_RATIO_OVERRIDE
    // Use the new reset scheme in PMUv3
   //pmu_reg_write (5, //0x05 default 12'h000
   //        ( (0 << 13) // Enables override setting [12] (1'b1)
   //          | (0 << 12) // Let VDD_CLK always connected to vbat
   //          | (1 << 11) // Enable override setting [10] (1'h0)
   //          | (0 << 10) // Have the converter have the periodic reset (1'h0)
   //          | (0 << 9) // Enable override setting [8] (1'h0)
   //          | (0 << 8) // Switch input / output power rails for upconversion (1'h0)
   //          | (0 << 7) // Enable override setting [6:0] (1'h0)
   //          | (0)      // Binary converter's conversion ratio (7'h00)
   //        ));

   //pmu_set_adc_period(1); // 0x100 about 1 min for 1/2/1 1P2 setting
}

inline static void pmu_adc_reset_setting(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Active
    pmu_reg_write (60, //0x3C
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
}


inline static void pmu_adc_disable(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Sleep
    pmu_reg_write(59, //0x3B
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
}

inline static void pmu_adc_enable(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Sleep
    pmu_reg_write(59, //0x3B
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
}

inline static void pmu_reset_solar_short(){
    pmu_reg_write(14, //0x0E, 
            ( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
              | (1 << 9)  // Enables override setting [8]
              | (0 << 8)  // Turn on the harvester-inhibiting switch
              | (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
              | (0)         // clamp_tune_top (decreases clamp thresh)
            ));

    pmu_reg_write(14, //0x0E 
            ( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
              | (0 << 9)  // Enables override setting [8]
              | (0 << 8)  // Turn on the harvester-inhibiting switch
              | (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
              | (0)         // clamp_tune_top (decreases clamp thresh)
            ));
}

//***************************************************
//  FLP Functions
//***************************************************
static void flp_fail(uint32_t id)
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

static void FLASH_initialization (void) {
    
    // Tune Flash
    mbus_remote_register_write (FLP_ADDR , 0x26 , 0x0D7788); // Program Current (default)
    mbus_remote_register_write (FLP_ADDR , 0x27 , 0x011BC8); // Erase Pump Diode Chain (default)
    mbus_remote_register_write (FLP_ADDR , 0x01 , 0x000112); // Tprog idle time
    mbus_remote_register_write (FLP_ADDR , 0x19 , 0x000F06); // Voltage Clamper Tuning
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
        uint32_t idx;

        for (idx=0; idx<8; idx++){ // All Pages (1Mb / idx=128) are erased > Erase 8kB / idx=8 instead
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

static void FLASH_pp_on (void) {
    mbus_remote_register_write (FLP_ADDR, 0x09, 0x00002D); // Ping Pong Go
}

static void FLASH_pp_off (void) {
    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP_ADDR, 0x13, 0x000000); // Ping Pong End
    set_halt_until_mbus_tx();
}

static void FLASH_read (void) {
    uint32_t i,j;
    uint32_t flp_sram_addr;
    flp_sram_addr = 0;
    FLASH_turn_on();

    // Flash Read
    mbus_remote_register_write (FLP_ADDR, 0x08 , 0x0); // Set FLSH_START_ADDR
    mbus_remote_register_write (FLP_ADDR, 0x07 , 0x0); // Set SRAM_START_ADDR

    set_halt_until_mbus_rx();
    //mbus_remote_register_write (FLP_ADDR, 0x09 , 0x02EE23); // Flash -> SRAM
    mbus_remote_register_write (FLP_ADDR, 0x09 , 0x07FFE3); // Flash -> SRAM
    set_halt_until_mbus_tx();

    if (*REG1 != 0x00002B) flp_fail(4);

    // Read 8kB SRAM bank
    for(j=0; j<16; j++){
        set_halt_until_mbus_rx();
        mbus_copy_mem_from_remote_to_any_bulk (FLP_ADDR, (uint32_t *) flp_sram_addr, 0x1, read_data, 127);
        set_halt_until_mbus_tx();
        for (i=0; i<128; i++) {
            delay(100);
            mbus_write_message32(0xC0, read_data[i]);
            delay(100);
        }
        flp_sram_addr = flp_sram_addr + 512;
    }

    // Turn off Flash
    FLASH_turn_off();
}

static void SRAM_read (void) {
    uint32_t i,j;
    uint32_t sram_addr;
    sram_addr = 0;

    for(j=0; j<8; j++){
        set_halt_until_mbus_rx();
        mbus_copy_mem_from_remote_to_any_bulk (FLP_ADDR, (uint32_t *) sram_addr, 0x1, read_data, 127);
        set_halt_until_mbus_tx();
        for (i=0; i<128; i++) {
            delay(100);
            mbus_write_message32(0xC0, read_data[i]);
            delay(100);
        }
        sram_addr = sram_addr + 512;
    }

}

//***************************************************
//  ADO Functions
//***************************************************
static void ado_initialization(void){
    // as_int of all zero regs initalization
    adov6vb_r07.as_int = 0;
    adov6vb_r0B.as_int = 0;
    adov6vb_r10.as_int = 0;

    adov6vb_r10.VAD_ADC_DOUT_ISOL = 1;
    mbus_remote_register_write(ADO_ADDR, 0x10, adov6vb_r10.as_int);

    adov6vb_r04.DSP_CLK_MON_SEL = 4; //LP CLK mon
    mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);
 
    // AFE Initialization
    adov6vb_r15.IB_GEN_CLK_EN = 1;
    adov6vb_r15.LS_CLK_EN_1P2 = 1;
    adov6vb_r15.REC_PGA_BWCON = 32;//24;
    adov6vb_r15.REC_PGA_CFBADD = 1;//0;
    adov6vb_r15.REC_PGA_GCON = 4;//2;
    mbus_remote_register_write(ADO_ADDR, 0x15, adov6vb_r15.as_int); //1F8622

    adov6vb_r16.IBC_MONAMP = 0;
    adov6vb_r16.IBC_REC_LNA = 6;
    adov6vb_r16.IBC_REC_PGA = 11;
    adov6vb_r16.IBC_VAD_LNA = 7;
    adov6vb_r16.IBC_VAD_PGA = 5;
    mbus_remote_register_write(ADO_ADDR, 0x16, adov6vb_r16.as_int);//0535BD
 
    adov6vb_r18.REC_LNA_N1_CON = 2;
    adov6vb_r18.REC_LNA_N2_CON = 2;
    adov6vb_r18.REC_LNA_P1_CON = 3;
    adov6vb_r18.REC_LNA_P2_CON = 3;
    mbus_remote_register_write(ADO_ADDR, 0x18, adov6vb_r18.as_int);//0820C3

    adov6vb_r19.REC_PGA_P1_CON = 3;
    mbus_remote_register_write(ADO_ADDR, 0x19, adov6vb_r19.as_int);//007064

    adov6vb_r1B.VAD_LNA_N1_LCON = 5;
    adov6vb_r1B.VAD_LNA_N2_LCON = 6;
    adov6vb_r1B.VAD_LNA_P1_CON = 3;
    adov6vb_r1B.VAD_LNA_P2_CON = 2;
    mbus_remote_register_write(ADO_ADDR, 0x1B, adov6vb_r1B.as_int);//0A6062

    adov6vb_r1C.VAD_PGA_N1_LCON = 0;
    adov6vb_r1C.VAD_PGA_N1_MCON = 1;
    mbus_remote_register_write(ADO_ADDR, 0x1C, adov6vb_r1C.as_int);//

    adov6vb_r11.LDO_CTRL_VREFLDO_VOUT_1P4HP = 4;
    mbus_remote_register_write(ADO_ADDR, 0x11, adov6vb_r11.as_int);//84CAC9

    adov6vb_r12.LDO_CTRL_VREFLDO_VOUT_0P6LP = 3;
    adov6vb_r12.LDO_CTRL_VREFLDO_VOUT_0P9HP = 3;
    adov6vb_r12.LDO_CTRL_VREFLDO_VOUT_1P4LP = 4;
    mbus_remote_register_write(ADO_ADDR, 0x12, adov6vb_r12.as_int);//828283
    
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
        adov6vb_r07.DSP_LUT_WR_ADDR = i;
        adov6vb_r07.DSP_LUT_DATA_IN = LUT_DATA[i];
        mbus_remote_register_write(ADO_ADDR, 0x07, adov6vb_r07.as_int);
        mbus_remote_register_write(ADO_ADDR, 0x08, 0x000001);
        mbus_remote_register_write(ADO_ADDR, 0x08, 0x000000);
    }
    //PHS Programming 
    for(i= 0; i<PHS_DATA_LENGTH; i++){
        adov6vb_r0B.DSP_PHS_WR_ADDR_FS = i;
        adov6vb_r0B.DSP_PHS_DATA_IN_FS = PHS_DATA[i];
        mbus_remote_register_write(ADO_ADDR, 0x0B, adov6vb_r0B.as_int);
        mbus_remote_register_write(ADO_ADDR, 0x0C, 0x000001);
        mbus_remote_register_write(ADO_ADDR, 0x0C, 0x000000);
    }
    
    //N_DCT: 128 points DCT 
    adov6vb_r00.DSP_N_DCT = 2; //128-pt DCT
    adov6vb_r00.DSP_N_FFT = 4; //256-pt FFT
    mbus_remote_register_write(ADO_ADDR, 0x00, adov6vb_r00.as_int);
    
    //WAKEUP REQ EN
    adov6vb_r0E.DSP_WAKEUP_REQ_EN = 1;
    mbus_remote_register_write(ADO_ADDR, 0x0E, adov6vb_r0E.as_int);
}


static void digital_set_mode(uint8_t mode){
    if(mode == 1){      //LP DSP
        adov6vb_r0D.DSP_HP_ADO_GO = 0;
        adov6vb_r0D.DSP_HP_DNN_GO = 0;
        adov6vb_r0D.DSP_HP_FIFO_GO = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BCB5
        adov6vb_r0D.DSP_HP_RESETN = 0;
        adov6vb_r0D.DSP_DNN_HP_MODE_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC15
        
        adov6vb_r0D.DSP_DNN_RESETN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC05
        adov6vb_r0D.DSP_DNN_ISOLATEN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC09
        adov6vb_r0D.DSP_DNN_PG_SLEEP_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC0B
        adov6vb_r0D.DSP_DNN_CTRL_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC0A
        
        adov6vb_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040
        adov6vb_r04.DSP_P2S_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040
        
        adov6vb_r0D.DSP_LP_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC4A
        }
    else if(mode == 2){ // HP DSP
        adov6vb_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80A

        adov6vb_r0D.DSP_DNN_CTRL_RF_SEL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        adov6vb_r0D.DSP_DNN_PG_SLEEP_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B809
        adov6vb_r0D.DSP_DNN_ISOLATEN_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80D
        adov6vb_r0D.DSP_DNN_CLKENB_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B805
        adov6vb_r0D.DSP_DNN_RESETN_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B815
        
        adov6vb_r0D.DSP_DNN_HP_MODE_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B835
        
        adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        
        adov6vb_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040
        adov6vb_r04.DSP_P2S_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040

        adov6vb_r0D.DSP_HP_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B8B5
        adov6vb_r0D.DSP_HP_FIFO_GO = 1;
        adov6vb_r0D.DSP_HP_ADO_GO = 0;
        adov6vb_r0D.DSP_HP_DNN_GO = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BBB5
        }
    else if(mode == 3){ // HP Compression only
        adov6vb_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80A
        adov6vb_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040

        adov6vb_r0D.DSP_DNN_CTRL_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        //adov6vb_r0D.DSP_DNN_PG_SLEEP_RF = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B809
        //adov6vb_r0D.DSP_DNN_ISOLATEN_RF = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80D
        //adov6vb_r0D.DSP_DNN_RESETN_RF = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B815
        
        adov6vb_r0D.DSP_DNN_HP_MODE_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B835

        adov6vb_r0D.DSP_HP_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B8B5
        adov6vb_r0D.DSP_HP_FIFO_GO = 1;
        adov6vb_r0D.DSP_HP_ADO_GO = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BBB5
        }
    else if(mode == 0){      //DSP All Off
        adov6vb_r0D.DSP_HP_FIFO_GO = 0;
        adov6vb_r0D.DSP_HP_ADO_GO = 0;
        adov6vb_r0D.DSP_HP_DNN_GO = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BCB5
        adov6vb_r0D.DSP_HP_RESETN = 0;
        adov6vb_r0D.DSP_DNN_HP_MODE_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC15
        
        adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        adov6vb_r0D.DSP_DNN_RESETN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC05
        adov6vb_r0D.DSP_DNN_CLKENB_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC0D
        adov6vb_r0D.DSP_DNN_ISOLATEN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC09
        adov6vb_r0D.DSP_DNN_PG_SLEEP_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC0B
        adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        adov6vb_r0D.DSP_DNN_CTRL_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC0A
        
        adov6vb_r04.DSP_P2S_MON_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//A00040
        
        adov6vb_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040
        
        adov6vb_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC4A
        }
}

static void afe_set_mode(uint8_t mode){
    if(mode == 1){      //LP AFE
        adov6vb_r0D.REC_ADC_RESETN = 0;
        adov6vb_r0D.REC_ADCDRI_EN = 0;
        adov6vb_r0D.REC_LNA_AMPEN = 0;
        adov6vb_r0D.REC_LNA_AMPSW_EN = 0;
        adov6vb_r0D.REC_LNA_OUTSHORT_EN = 0;
        adov6vb_r0D.REC_PGA_AMPEN = 0;
        adov6vb_r0D.REC_PGA_OUTSHORT_EN = 0;
        adov6vb_r0D.VAD_LNA_AMPSW_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);
        
        adov6vb_r13.VAD_PGA_OUTSHORT_EN = 0;
        adov6vb_r13.VAD_LNA_OUTSHORT_EN = 0;
        adov6vb_r13.LDO_PG_IREF = 0;
        adov6vb_r13.LDO_PG_VREF_0P6LP = 0;
        adov6vb_r13.LDO_PG_LDOCORE_0P6LP = 0;
        adov6vb_r13.LDO_PG_VREF_1P4LP = 0;
        adov6vb_r13.LDO_PG_LDOCORE_1P4LP = 0;
        adov6vb_r13.LDO_PG_VREF_1P4HP = 1;
        adov6vb_r13.LDO_PG_LDOCORE_1P4HP = 1;
        adov6vb_r13.LDO_PG_VREF_0P9HP = 1;
        adov6vb_r13.LDO_PG_LDOCORE_0P9HP = 1;
        mbus_remote_register_write(ADO_ADDR, 0x13, adov6vb_r13.as_int);
        delay(MBUS_DELAY*9);

        adov6vb_r0F.VAD_ADC_RESET = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0F, adov6vb_r0F.as_int);

        adov6vb_r14.VAD_ADCDRI_EN = 1;
        adov6vb_r14.VAD_LNA_AMPEN = 1;
        adov6vb_r14.VAD_PGA_AMPEN = 1;
        adov6vb_r14.VAD_PGA_BWCON = 19;//13;
        adov6vb_r14.VAD_PGA_GCON = 4;//8;
        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);

        ////VAD MON setting can come here
        //adov6vb_r1A.VAD_MONSEL = 1;
        //adov6vb_r1A.VAD_MONBUF_EN = 1;
        //adov6vb_r1A.VAD_REFMONBUF_EN = 0;
        //adov6vb_r1A.VAD_ADCDRI_OUT_OV_EN = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x1A, adov6vb_r1A.as_int);
        
        ////REC MON setting (debug)
        adov6vb_r17.REC_MONSEL = 0;
        adov6vb_r17.REC_MONBUF_EN = 0;
        //adov6vb_r17.REC_REFMONBUF_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x17, adov6vb_r17.as_int);

        adov6vb_r10.DO_LP_HP_SEL = 0;    //0: LP, 1: HP
        adov6vb_r10.DIO_OUT_EN = 1;
        adov6vb_r10.VAD_ADC_DOUT_ISOL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x10, adov6vb_r10.as_int);

        adov6vb_r04.DSP_CLK_MON_SEL = 4; //LP CLK mon
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);
    }
    else if(mode == 2){ // HP AFE
        adov6vb_r13.LDO_PG_IREF = 0;
        adov6vb_r13.LDO_PG_VREF_0P6LP = 0;
        adov6vb_r13.LDO_PG_LDOCORE_0P6LP = 0;
        adov6vb_r13.LDO_PG_VREF_1P4LP = 0;
        adov6vb_r13.LDO_PG_LDOCORE_1P4LP = 0;
        adov6vb_r13.LDO_PG_VREF_1P4HP = 0;
        adov6vb_r13.LDO_PG_LDOCORE_1P4HP = 0;
        adov6vb_r13.LDO_PG_VREF_0P9HP = 0;
        adov6vb_r13.LDO_PG_LDOCORE_0P9HP = 0;
        mbus_remote_register_write(ADO_ADDR, 0x13, adov6vb_r13.as_int);
        delay(MBUS_DELAY*9);

        adov6vb_r0D.REC_ADC_RESETN = 1;
        adov6vb_r0D.REC_ADCDRI_EN = 1;
        adov6vb_r0D.REC_LNA_AMPEN = 1;
        adov6vb_r0D.REC_LNA_AMPSW_EN = 1;
        adov6vb_r0D.REC_LNA_OUTSHORT_EN = 0;
        adov6vb_r0D.REC_PGA_AMPEN = 1;
        adov6vb_r0D.REC_PGA_OUTSHORT_EN = 0;
        adov6vb_r0D.VAD_LNA_AMPSW_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);
        delay(MBUS_DELAY*9);
        //adov6vb_r0D.RST_HP_PGA = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);

        adov6vb_r10.DO_LP_HP_SEL = 1;    //HP
        adov6vb_r10.DIO_OUT_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x10, adov6vb_r10.as_int);

        adov6vb_r04.DSP_CLK_MON_SEL = 2; //HP clock mon
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);
    }
    else{       // AFE off
        adov6vb_r0D.REC_ADC_RESETN = 0;
        adov6vb_r0D.REC_ADCDRI_EN = 0;
        adov6vb_r0D.REC_LNA_AMPEN = 0;
        adov6vb_r0D.REC_LNA_AMPSW_EN = 0;
        adov6vb_r0D.REC_LNA_OUTSHORT_EN = 0;
        adov6vb_r0D.REC_PGA_AMPEN = 0;
        adov6vb_r0D.REC_PGA_OUTSHORT_EN = 0;
        adov6vb_r0D.VAD_LNA_AMPSW_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);
        
        adov6vb_r13.LDO_PG_IREF = 0;
        adov6vb_r13.LDO_PG_VREF_0P6LP = 1;
        adov6vb_r13.LDO_PG_LDOCORE_0P6LP = 1;
        adov6vb_r13.LDO_PG_VREF_1P4LP = 1;
        adov6vb_r13.LDO_PG_LDOCORE_1P4LP = 1;
        adov6vb_r13.LDO_PG_VREF_1P4HP = 1;
        adov6vb_r13.LDO_PG_LDOCORE_1P4HP = 1;
        adov6vb_r13.LDO_PG_VREF_0P9HP = 1;
        adov6vb_r13.LDO_PG_LDOCORE_0P9HP = 1;
        mbus_remote_register_write(ADO_ADDR, 0x13, adov6vb_r13.as_int);

        adov6vb_r0F.VAD_ADC_RESET = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0F, adov6vb_r0F.as_int);

        adov6vb_r14.VAD_ADCDRI_EN = 0;
        adov6vb_r14.VAD_LNA_AMPEN = 0;
        adov6vb_r14.VAD_PGA_AMPEN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);

        adov6vb_r10.DIO_OUT_EN = 0;
        //adov6vb_r10.DIO_DIR_IN1OUT0 = 1;
        adov6vb_r10.VAD_ADC_DOUT_ISOL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x10, adov6vb_r10.as_int);
        ////VAD MON setting can come here
        //adov6vb_r1A.VAD_MONSEL = 1;
        //adov6vb_r1A.VAD_MONBUF_EN = 1;
        //adov6vb_r1A.VAD_REFMONBUF_EN = 0;
        //adov6vb_r1A.VAD_ADCDRI_OUT_OV_EN = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x1A, adov6vb_r1A.as_int);
        
        ////REC MON setting (debug)
        //adov6vb_r17.REC_MONSEL = 1;
        //adov6vb_r17.REC_MONBUF_EN = 1;
        //adov6vb_r17.REC_REFMONBUF_EN = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x17, adov6vb_r17.as_int);

    }
}

//***************************************************
//  ADO+FLP Functions
//***************************************************
inline static void comp_stream(void){
    
    afe_set_mode(2); // ADO HP AFE ON

    // Flash setup and start /////
    FLASH_turn_on();
    FLASH_pp_ready();
    FLASH_pp_on();

    delay(MBUS_DELAY*2);
    /////////////////////////////

    digital_set_mode(3); // HP Compression only start
    
    delay(400000); // Compression-length control: ~10s
    
    digital_set_mode(0); // ADO DSP ALL OFF
    afe_set_mode(0); // ADO AFE OFF

    FLASH_pp_off();
    FLASH_turn_off();
}

inline static void comp_stream2(void){
    
    afe_set_mode(2); // ADO HP AFE ON

    // External Streaming
//   mbus_remote_register_write (FLP_ADDR, 0x0C, 0x000007);  
//   mbus_remote_register_write (FLP_ADDR, 0x0E, 0x000000); // Setup SRAM_START_ADDRSS_EXT
//   mbus_remote_register_write (FLP_ADDR, 0x0D, 0x012AD4); // Setup Timeout
//   mbus_remote_register_write (FLP_ADDR, 0x09, 0x00FFED); // Setup command, lengh, IRQ and go

    /////////////////////////////

    digital_set_mode(3); // HP Compression only start
    
    delay(1000000); // Compression-length control: ~2.5s
    
    digital_set_mode(0); // ADO DSP ALL OFF
    afe_set_mode(0); // ADO AFE OFF
}

inline static void flash_erasedata(void){
    FLASH_turn_on();
    FLASH_pp_ready();
    FLASH_turn_off();
    /////////////////////////////
}

//***************************************************
// Temp Sensor Functions (SNTv5)
//***************************************************

static void SNT_initialization (void) {
    // Temp Sensor Settings --------------------------------------
    sntv5_r01.TSNS_RESETn = 0;
    sntv5_r01.TSNS_EN_IRQ = 1;
    sntv5_r01.TSNS_BURST_MODE = 0;
    sntv5_r01.TSNS_CONT_MODE = 0;
    mbus_remote_register_write(SNT_ADDR,1,sntv5_r01.as_int);

    // Set temp sensor conversion time
    sntv5_r03.TSNS_SEL_STB_TIME = 0x1; 
    sntv5_r03.TSNS_SEL_CONV_TIME = 0x6; // Default: 0x6
    mbus_remote_register_write(SNT_ADDR,0x03,sntv5_r03.as_int);

    // Reply Address
    sntv5_r07.TSNS_INT_RPLY_REG_ADDR = 0x2; // Default: 0x0, Change interrupt target register REG0 -> REG2
    mbus_remote_register_write(SNT_ADDR,0x07,sntv5_r07.as_int);

    // SNT Wakeup Timer Settings --------------------------------------

    // Config Register A
    sntv5_r0A.TMR_S = 0x1; // Default: 0x4, use 1 for good TC
    // Tune R for TC
    sntv5_r0A.TMR_DIFF_CON = 0x3FEF; // Default: 0x3FFB
    mbus_remote_register_write(SNT_ADDR,0x0A,sntv5_r0A.as_int);

    // TIMER CAP_TUNE  
    // Tune C for freq
    sntv5_r09.TMR_SEL_CAP = 0x80; // Default : 8'h8
    sntv5_r09.TMR_SEL_DCAP = 0x3F; // Default : 6'h4

    mbus_remote_register_write(SNT_ADDR,0x09,sntv5_r09.as_int);

    // Wakeup Counter
    sntv5_r17.WUP_CLK_SEL = 0x0; 
    sntv5_r17.WUP_AUTO_RESET = 0x0; // Automatically reset counter to 0 upon sleep 
    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);

}
//static void temp_sensor_power_off(){
//    sntv5_r01.TSNS_RESETn = 0;
//    sntv5_r01.TSNS_SEL_LDO = 0;
//    sntv5_r01.TSNS_EN_SENSOR_LDO = 0;
//    sntv5_r01.TSNS_ISOLATE = 1;
//    mbus_remote_register_write(SNT_ADDR,1,sntv5_r01.as_int);
//}
//static void sns_ldo_power_off(){
//    sntv5_r00.LDO_EN_VREF    = 0;
//    sntv5_r00.LDO_EN_IREF    = 0;
//    sntv5_r00.LDO_EN_LDO    = 0;
//    mbus_remote_register_write(SNT_ADDR,0,sntv5_r00.as_int);
//}
//
//static void snt_start_timer_presleep(){
//
//	// New for SNTv3
//	sntv5_r08.TMR_SLEEP = 0x0; // Default : 0x1
//	mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
//	sntv5_r08.TMR_ISOLATE = 0x0; // Default : 0x1
//	mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
//
//    // TIMER SELF_EN Disable 
//    sntv5_r09.TMR_SELF_EN = 0x0; // Default : 0x1
//    mbus_remote_register_write(SNT_ADDR,0x09,sntv5_r09.as_int);
//
//    // EN_OSC 
//    sntv5_r08.TMR_EN_OSC = 0x1; // Default : 0x0
//    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
//
//    // Release Reset 
//    sntv5_r08.TMR_RESETB = 0x1; // Default : 0x0
//    sntv5_r08.TMR_RESETB_DIV = 0x1; // Default : 0x0
//    sntv5_r08.TMR_RESETB_DCDC = 0x1; // Default : 0x0
//    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
//
//    // TIMER EN_SEL_CLK Reset 
//    sntv5_r08.TMR_EN_SELF_CLK = 0x1; // Default : 0x0
//    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
//
//    // TIMER SELF_EN 
//    sntv5_r09.TMR_SELF_EN = 0x1; // Default : 0x0
//    mbus_remote_register_write(SNT_ADDR,0x09,sntv5_r09.as_int);
//    //delay(100000); 
//
//    snt_timer_enabled = 1;
//}
//
//static void snt_start_timer_postsleep(){
//    // Turn off sloscillator
//    sntv5_r08.TMR_EN_OSC = 0x0; // Default : 0x0
//    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
//}
//
//static void snt_stop_timer(){
//
//    // EN_OSC
//    sntv5_r08.TMR_EN_OSC = 0x0; // Default : 0x0
//    // RESET
//    sntv5_r08.TMR_EN_SELF_CLK = 0x0; // Default : 0x0
//    sntv5_r08.TMR_RESETB = 0x0;// Default : 0x0
//    sntv5_r08.TMR_RESETB_DIV = 0x0; // Default : 0x0
//    sntv5_r08.TMR_RESETB_DCDC = 0x0; // Default : 0x0
//    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
//    snt_timer_enabled = 0;
//
//    sntv5_r17.WUP_ENABLE = 0x0; // Default : 0x
//    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
//
//	// New for SNTv3
//	sntv5_r08.TMR_SLEEP = 0x1; // Default : 0x1
//	sntv5_r08.TMR_ISOLATE = 0x1; // Default : 0x1
//	mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
//
//}
//
//static void snt_set_timer_threshold(uint32_t sleep_count){
//    
//    mbus_remote_register_write(SNT_ADDR,0x19,sleep_count>>24);
//    mbus_remote_register_write(SNT_ADDR,0x1A,sleep_count & 0xFFFFFF);
//    
//}


//***************************************************
// End of Program Sleep Operation
//***************************************************
//static void operation_sns_sleep_check(void){
//    // Make sure LDO is off
//    if (sns_running){
//        sns_running = 0;
//        sns_done = 0;
//        temp_sensor_power_off();
//        sns_ldo_power_off();
//    }
////  if (pmu_setting_state != PMU_25C){
////  	// Set PMU to room temp setting
////  	pmu_setting_state = PMU_25C;
////  	pmu_active_setting_temp_based();
////  	pmu_sleep_setting_temp_based();
////  }
//}

static void operation_sleep(void){

	// Reset GOC_DATA_IRQ
	*GOC_DATA_IRQ = 0;

    // Freeze GPIO
    freeze_gpio_out();

    // Check ADO HP interrupt before going into sleep
    while (*EP_MODE) config_timerwd(TIMERWD_VAL);

    // Go to Sleep
    mbus_sleep_all();
    while(1);

}

//static void operation_sleep_snt_timer(void){
//
//    // Disable PRC Timer
//    set_wakeup_timer(0, 0, 0);
//
//    // Go to sleep
//    operation_sleep();
//
//}

//static void operation_sleep_noirqreset(void){
//
//    // Go to Sleep
//    mbus_sleep_all();
//    while(1);
//
//}

static void operation_sleep_notimer(void){
    // Make sure the irq counter is reset    
    exec_count_irq = 0;

    set_wakeup_timer(0,0,0);    //disable timer
    
    // Go to sleep
    operation_sleep();
}

//static void operation_sleep_xo_timer(void){
//    // Make sure the irq counter is reset    
//    exec_count_irq = 0;
//    
//    // Disable timer
//    set_wakeup_timer(0,0,0);    
//    
//    // Use XO Timer to set wake-up period    
//    xot_enable(3*2048);
//
//    // Go to sleep
//    operation_sleep();
//}

//***************************************************
// Initialization
//***************************************************
static void operation_init(void){
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);
    *TIMERWD_GO = 0x0;
    *REG_MBUS_WD = 0;

    // EP Mode
    *EP_MODE = 0;

    // Set CPU & Mbus Clock Speeds
    prev22e_r0B.CLK_GEN_RING = 0x1; // Default 0x1, 2bits
    prev22e_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x2, 3bits
    prev22e_r0B.CLK_GEN_DIV_CORE = 0x2; // Default 0x3, 3bits
    prev22e_r0B.GOC_CLK_GEN_SEL_DIV = 0x1; // Default 0x1, 2bits
    prev22e_r0B.GOC_CLK_GEN_SEL_FREQ = 0x0; // Default 0x7, 3bits
    *REG_CLKGEN_TUNE = prev22e_r0B.as_int;

    prev22e_r1C.SRAM0_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
    prev22e_r1C.SRAM0_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
    prev22e_r1C.SRAM0_USE_INVERTER_SA= 0; // Default 0x2, 1bit
    *REG_SRAM0_TUNE = prev22e_r1C.as_int;

    // Wakeup if there's pending request
	prev22e_r1B.WAKEUP_ON_PEND_REQ = 0x1; // Default 0x0
	*REG_SYS_CONF = prev22e_r1B.as_int;

    //Enumerate & Initialize Registers
    enumerated = ENUMID;
    exec_count = 0;
    exec_count_irq = 0;

    //Enumeration
    mbus_enumerate(SNT_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(MRR_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(FLP_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(ADO_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(PMU_ADDR);
    delay(MBUS_DELAY);

    // Initialize PMU ------------------------------------------------------
    // Change PMU reply IRQ register REG0 -> REG3
    mbus_remote_register_write(PMU_ADDR,0x52, 
            ( ( 16 << 8) // INT_RPLY_SHORT_ADDR; default 0x10
              | (3) // INT_RPLY REG_ADDR; default 0
            ));
    delay(MBUS_DELAY);

    pmu_set_clk_init();
    pmu_reset_solar_short();

    // Increase reference voltage level 
    pmu_reg_write(12,  
            ( (0) // default 7'h18
            ));

    // Enable Slow loop
    pmu_reg_write(15, //0x0F, 
   	( (1 << 13) // Override SAR_EN_ACC_VOUT with bit [12]
   	| (1 << 12) // Enable slow loop
   	| (4) // default 4'h4
   	));

    // Reduce LDO bias current in active mode
    pmu_reg_write(16, //0x10 
            ( (1 << 13) // Override slow loop enable with bit[12]
            | (0 << 12) // Disable Slow loop 
            | (1) // 
            ));

    // Adjust wait time for SAR and UDC during active->sleep conversion
    pmu_reg_write(46, //0x2E, TICK_UDC_SLEEP_WAIT
            ( (1) // default 12'd64
            ));

    pmu_reg_write(45, //0x2D, TICK_SAR_SLEEP_WAIT
            ( (1) // default 12'd64
            ));

    // Adjust canary-based active pmu strength
    pmu_reg_write(72, //0x48, 
    	( (0 << 10) // SAR_V1P2_DIVSEL[1:0] 
    	| (1 << 9) // SAR_CMP_EN Override
    	| (1 << 8) // UPC_CMP_EN Override
    	| (1 << 7) // DNC_CMP_EN Override
    	| (2 << 4) // SAR_CP_CAP
    	| (0 << 2) // UPC_CP_CAP
    	| (1) // DNC_CP_CAP
    	 ));

    // Adjust hard reset delay
    pmu_reg_write(53, 
            ( (0xAF) // default 6'd1
            ));

    pmu_set_sar_override(0x4F);
    pmu_adc_reset_setting();
    pmu_adc_disable();

    // TEST VDD Control
    *REG_CPS = 0x5;   //both TEST VDD on [2]=1: Test 1.2V on,  [0]=1: Test 0.6V on
    //*REG_CPS = 0x0;   //both TEST VDD off
    delay(MBUS_DELAY*10);

    // Initialize SNT ------------------------------------------------------
    SNT_initialization(); 
    delay(MBUS_DELAY*10);

    // Initialize global variables--------------------------------------
    SNT_0P5S_VAL = 1000;
    xo_count= 28000;
    mode_select=0;
    sns_running = 0;
    sns_done = 0;
    wakeup_data = 0;
    error_code = 0;
	
    PMU_m10C_threshold_sns = 380; // Around -10C
    PMU_0C_threshold_sns = 590; // Around 0C
    PMU_10C_threshold_sns = 940; // Around 10C
    PMU_20C_threshold_sns = 1400; // Around 20C
    PMU_35C_threshold_sns = 2622; // Around 35C

    TEMP_CALIB_A = 240000;
    TEMP_CALIB_B = 3750000;

    FLASH_initialization(); //FLPv3L initialization
    ado_initialization(); //ADOv6VB initialization
    XO_init(); //XO initialization

    direction_gpio = 0x0; 
    gpio_write_data(0x00);
    init_gpio();
    delay(MBUS_DELAY);
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
void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_wakeup(void) { // WAKE-UP
	// Need to reset SREG_WAKEUP_SOURCE since pending wakeup is used	
	*SCTR_REG_CLR_WUP_SOURCE = 0;
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
    //else if(((*SREG_WAKEUP_SOURCE >> 2) & 1) == 1){ //waked up by XO Timer 
    //    //Do something
    //    mbus_write_message32(0xFD, 0xFDFDFDFD);
    //    operation_sns_run();
    //}   
    *SREG_WAKEUP_SOURCE=0;
}
void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    wfi_timeout_flag = 1;
    mbus_write_message32(0xFA, 0xFAFAFAFA);
}
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);

    if(*REG0 == 1){ // LP -> HP mode change
        *NVIC_ICER = ( 1 << IRQ_REG2);
        *EP_MODE = 1;
                
        adov6vb_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80A
       
        afe_set_mode(2);
        digital_set_mode(2);
    }
    else if(*REG0 == 0) { // HP -> ULP mode change
        *NVIC_ISER = (1 << IRQ_REG2);
        *EP_MODE = 0;
        if(sns_done) sns_done=0;
        adov6vb_r0D.DSP_HP_FIFO_GO = 0;
        adov6vb_r0D.DSP_HP_ADO_GO = 0;
        adov6vb_r0D.DSP_HP_DNN_GO = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BCB5
        
        afe_set_mode(1);
        digital_set_mode(1);
    }

#ifdef DEBUG_MBUS_MSG
    mbus_write_message32(0xE0, *REG0);
    mbus_write_message32(0xE1, *EP_MODE);
#endif    
}
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1);
//#ifdef DEBUG_MBUS_MSG
//    mbus_write_message32(0xE5, *REG1);
//#endif    
}

void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2);
    //mbus_write_message32(0xE2, 0x00000222);

    // Read register
//    set_halt_until_mbus_rx();
//    mbus_remote_register_read(SNT_ADDR,0x6,4);
//    read_data_temp = *REG4;
//    set_halt_until_mbus_tx();
//
//    meas_count++;
//
//    // Last measurement from this wakeup
//    if (meas_count == NUM_TEMP_MEAS){
//        // No error; see if there was a timeout
//        if (wfi_timeout_flag){
//            temp_storage_latest = 0x666;
//            wfi_timeout_flag = 0;
//        }else{
//            temp_storage_latest = read_data_temp;
//            mbus_write_message32(0xBB, (exec_count << 16) | temp_storage_latest);
//
//        }
//    }
//
//    if(mode_select == 0x2 || mode_select==0x4){
//        pmu_setting_prev = pmu_setting_state;
//        // Change PMU based on temp
//        if (temp_storage_latest > PMU_35C_threshold_sns){
//            pmu_setting_state = PMU_35C;
//        }else if (temp_storage_latest < PMU_m10C_threshold_sns){
//            pmu_setting_state = PMU_m10C;
//        }else if (temp_storage_latest < PMU_0C_threshold_sns){
//            pmu_setting_state = PMU_0C;
//        }else if (temp_storage_latest < PMU_10C_threshold_sns){
//            pmu_setting_state = PMU_10C;
//        }else if (temp_storage_latest < PMU_20C_threshold_sns){
//            pmu_setting_state = PMU_20C;
//        }else{
//            pmu_setting_state = PMU_25C;
//        }
//    
//        if (pmu_setting_prev != pmu_setting_state){
//            pmu_active_setting_temp_based();
//            pmu_sleep_setting_temp_based();
//        }
//    }
//    
//    meas_count = 0;
//    // Assert temp sensor isolation & turn off temp sensor power
//    temp_sensor_power_off();
//    sns_ldo_power_off();
//    
//    
//    if (temp_storage_debug){
//        temp_storage_latest = exec_count;
//    }
//    sns_running=0;
//    sns_done=1;
//    
//    //Reset and Restart Timer
//    sntv5_r17.WUP_ENABLE = 0x0;
//    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
//	delay(MBUS_DELAY);
//    sntv5_r17.WUP_ENABLE = 0x1;
//    mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);

}
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3);
}
void handler_ext_int_gocep(void) { // GOCEP
	*NVIC_ICPR = (0x1 << IRQ_GOCEP);
	#ifdef DEBUG_MBUS_MSG
	mbus_write_message32(0xEE,  *GOC_DATA_IRQ);
	#endif    
	wakeup_data = *GOC_DATA_IRQ;
   	uint32_t data_cmd = (wakeup_data>>24) & 0xFF;
   	uint32_t data_val = wakeup_data & 0xFF;
   	uint32_t data_val2 = (wakeup_data>>8) & 0xFF;
   	uint32_t data_val3 = (wakeup_data>>16) & 0xFF;
   	if(data_cmd == 0x01){       // Charge pump control
   	    if(data_val==1){        //ON
   	        *TIMERWD_GO = 0x0;
   	        adov6vb_r14.CP_CLK_EN_1P2 = 1;
   	        adov6vb_r14.CP_CLK_DIV_1P2 = 2;//0;
   	        adov6vb_r14.CP_VDOWN_1P2 = 0; //vin = V3P6
   	        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	        delay(CP_DELAY); 
   	        adov6vb_r14.CP_VDOWN_1P2 = 1; //vin = gnd
   	        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	        delay(CP_DELAY*1); 
   	        adov6vb_r14.CP_VDOWN_1P2 = 0; //vin = V3P6
   	        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	        delay(CP_DELAY); 
   	        adov6vb_r14.CP_VDOWN_1P2 = 1; //vin = gnd
   	        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	        delay(CP_DELAY*1); 
   	        adov6vb_r14.CP_VDOWN_1P2 = 0; //vin = V3P6
   	        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	        delay(CP_DELAY); 
   	        adov6vb_r14.CP_VDOWN_1P2 = 1; //vin = gnd
   	        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	    }
   	    else if(data_val==2){        //ON
   	        adov6vb_r14.CP_CLK_EN_1P2 = 1;
   	        adov6vb_r14.CP_CLK_DIV_1P2 = 0;
   	        adov6vb_r14.CP_VDOWN_1P2 = 0; //vin = V3P6
   	        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	        delay(CP_DELAY*0.2); 
   	        adov6vb_r14.CP_VDOWN_1P2 = 1; //vin = gnd
   	        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	    }
   	    else if(data_val == 3){
   	        adov6vb_r14.CP_CLK_EN_1P2 = 1;
   	        adov6vb_r14.CP_CLK_DIV_1P2 = 0; //clk 8kHz
   	        adov6vb_r14.CP_VDOWN_1P2 = 0; //vin = V3P6
   	        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	    }
   	    else if(data_val == 4){
   	        adov6vb_r14.CP_CLK_EN_1P2 = 1;
   	        adov6vb_r14.CP_CLK_DIV_1P2 = 0; //clk 8kHz
   	        adov6vb_r14.CP_VDOWN_1P2 = 1; //vin = gnd
   	        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	    }
   	    else{                   //OFF
   	        adov6vb_r14.CP_CLK_EN_1P2 = 0;
   	        adov6vb_r14.CP_CLK_DIV_1P2 = 3;
   	        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	        //flash_erasedata();
   	    }
   	}
   	else if(data_cmd == 0x02){  // SRAM Programming mode
   	    if(data_val==1){    //Enter
   	        adov6vb_r0E.DSP_DNN_DBG_MODE_EN = 1;
   	        mbus_remote_register_write(ADO_ADDR, 0x0E, adov6vb_r0E.as_int);//000054
   	        adov6vb_r0D.DSP_DNN_CTRL_RF_SEL = 1;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
   	        adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 1;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
   	        adov6vb_r0D.DSP_DNN_PG_SLEEP_RF = 0;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B809
   	        adov6vb_r0D.DSP_DNN_ISOLATEN_RF = 1;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80D
   	        adov6vb_r0D.DSP_DNN_CLKENB_RF = 0;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B805
   	        adov6vb_r0D.DSP_DNN_RESETN_RF = 1;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B815
   	    }
   	    else{               //Exit
   	        adov6vb_r0D.DSP_DNN_RESETN_RF = 0;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B805
   	        adov6vb_r0D.DSP_DNN_CLKENB_RF = 1;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80D
   	        adov6vb_r0D.DSP_DNN_ISOLATEN_RF = 0;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B809
   	        adov6vb_r0D.DSP_DNN_PG_SLEEP_RF = 1;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
   	        adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 0;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
   	        adov6vb_r0D.DSP_DNN_CTRL_RF_SEL = 0;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80A
   	        adov6vb_r0E.DSP_DNN_DBG_MODE_EN = 0;
   	        mbus_remote_register_write(ADO_ADDR, 0x0E, adov6vb_r0E.as_int);//000050
   	    }
   	}
   	else if(data_cmd == 0x03){  // DSP Monitoring Configure
   	    if(data_val==1){    //Go
   	        //adov6vb_r0E.DSP_MIX_MON_EN = 1;
   	        //mbus_remote_register_write(ADO_ADDR, 0x0E, adov6vb_r0E.as_int);//000150
   	        //adov6vb_r00.DSP_FE_SEL_EXT = 1;
   	        //mbus_remote_register_write(ADO_ADDR, 0x00, adov6vb_r00.as_int);//9F1B28
   	        adov6vb_r04.DSP_P2S_MON_EN = 1;
   	        adov6vb_r04.DSP_CLK_MON_SEL = 4;
   	        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//A00040
   	        adov6vb_r04.DSP_P2S_RESETN = 1;
   	        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040
   	        //adov6vb_r10.DIO_OUT_EN = 1;
   	        //adov6vb_r10.DIO_IN_EN = 0;
   	        //adov6vb_r10.DIO_DIR_IN1OUT0 = 0;
   	        //mbus_remote_register_write(ADO_ADDR, 0x10, adov6vb_r10.as_int);//000008
   	    }
   	    else{               //Stop
   	        adov6vb_r04.DSP_P2S_MON_EN = 0;
   	        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//A00040
   	        adov6vb_r04.DSP_P2S_RESETN = 0;
   	        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040
   	    }
   	}
   	else if(data_cmd == 0x04){  // DSP LP Control
   	    if(data_val==1){    //Go
   	        adov6vb_r0D.DSP_LP_RESETN = 1;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC4A
   	    }
   	    else{               //Stop
   	        adov6vb_r0D.DSP_LP_RESETN = 0;
   	        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC0A
   	        *EP_MODE=0;
   	    }
   	}
   	else if(data_cmd == 0x05){  // AFE Control
   	    afe_set_mode(data_val);
   	    if(data_val == 2) {
		    if(data_val2 !=0){
   	        	    *TIMERWD_GO = 0x0;
			    uint32_t cnt=0;
			    while(cnt<data_val2){
				    delay(MBUS_DELAY*1000); 
				    cnt=cnt+1;
			  }
		    }
		    else delay(MBUS_DELAY*1000); //~10sec, AFE HP test purpose
	    }
   	}
   	else if(data_cmd == 0x06){  // Compression + Flash Test
   	    if(data_val==1){    //Go
   	        *TIMERWD_GO = 0x0;
	   	//pmu_set_active_clk(0xB,0xA,0x10,0xA);
   	        comp_stream();
   	    }
   	    else if(data_val==2){
   	        *TIMERWD_GO = 0x0;
   	        FLASH_read();
   	    }
   	    else if(data_val==3){
   	        *TIMERWD_GO = 0x0;
   	        comp_stream2();
   	    }
   	    else if(data_val==4){
   	        *TIMERWD_GO = 0x0;
   	        SRAM_read();
   	    }
   	}
   	else if(data_cmd == 0x08){ //Power gate control
   	    *REG_CPS = 0x7 & data_val;
   	}
   	else if(data_cmd == 0x09){ // PMU sleep setting control
   	    pmu_set_sleep_clks((data_val & 0xF), ((data_val>>4) & 0xF), data_val2 & 0x1F, data_val3 & 0xF);
   	}
   	else if(data_cmd == 0x0A){ // PMU active setting control
   	    if(data_val != 0xF) pmu_set_active_clk(data_val+1,data_val,data_val2,data_val);
   	    else pmu_set_active_clk(data_val,data_val,data_val2,data_val);
   	    delay(MBUS_DELAY*300); //~3sec
   	}
   	else if(data_cmd == 0x0B){
   	    pmu_set_sar_override(data_val);
    	    //pmu_set_sar_ratio(data_val);
   	}
   	else if(data_cmd == 0x0C){
   	    if(data_val==1){   
		xo_start();
   	    }
   	    else if(data_val==2){   
   		prev22e_r19.XO_RESETn       = 1;
   		prev22e_r19.XO_PGb_START_UP = 1;
   		*REG_XO_CONF1 = prev22e_r19.as_int;

   		delay(XO_WAIT_A); // Delay A (~1s; XO Start-Up)

   		prev22e_r19.XO_ISOL_CLK_HP = 0;
   		*REG_XO_CONF1 = prev22e_r19.as_int;
   		
   		delay(XO_WAIT_B); // Delay B (~1s; VLDO & IBIAS generation)

   		prev22e_r19.XO_ISOL_CLK_LP = 0;
   		*REG_XO_CONF1 = prev22e_r19.as_int;
   	    }
   	    else if(data_val==0){
		xo_stop();
   	    }
   	    else if(data_val==3){	
   	    	if(data_val2==1){
   	    	    	prev22e_r19.XO_EN_SLOW = 0x0;
   	    		prev22e_r19.XO_EN_NOM  = 0x1;
   	    		prev22e_r19.XO_EN_FAST = 0x0;
   	    	}
   	    	else if(data_val2==2){
   	    	    	prev22e_r19.XO_EN_SLOW = 0x1;
   	    		prev22e_r19.XO_EN_NOM  = 0x0;
   	    		prev22e_r19.XO_EN_FAST = 0x0;
   	    	}
   	    	else if(data_val2==3){
   	    	    	prev22e_r19.XO_EN_SLOW = 0x0;
   	    		prev22e_r19.XO_EN_NOM  = 0x0;
   	    		prev22e_r19.XO_EN_FAST = 0x1;
   	    	}
   	    	*REG_XO_CONF1 = prev22e_r19.as_int;
   	    	delay(100);

   	    	prev22e_r19.XO_CAP_TUNE = data_val3 & 0xF; // Additional Cap on OSC_IN & OSC_DRV
   	    	*REG_XO_CONF1 = prev22e_r19.as_int;

   	    }
   	    else if(data_val==4){
		start_xo_cout();
   	    }
   	    else if(data_val==5){
		stop_xo_cout();
   	    }
   	}
   	else if(data_cmd == 0x0D){ // Enable slow-loop in sleep monde / PRC CLK asstisted wakeup in active mode
		pmu_reg_write(71, //0x47, 
		( (1 << 19) // Enable RO-Assisted Transition
		| (0 << 18) // Enable SAR Slow-Loop in Active mode
		| (0 << 17) // Enable UPC Slow-Loop in Active mode
		| (0 << 16) // Enable DNC Slow-Loop in Active mode
		| (1 << 15) // Enable SAR Slow-Loop in Sleep mode
		| (1 << 14) // Enable UPC Slow-Loop in Sleep mode
		| (1 << 13) // Enable DNC Slow-Loop in Sleep mode
		| (1 << 11) // PRC ClokcGen Ring Tuning (2'h0: 13 stages / 2'h1: 11 stages / 2'h2: 9 stages / 2'h3: 7 stages)
		| (data_val << 8) // Clock Divider Tuning for SAR Charge Pump Pull-Up (3'h0: divide by 1 / 3'h1: divide by 2 / 3'h2: divide by 4 / 3'h3: divide by 8 / 3'h4: divide by 16)
		| (((data_val2 >>4)& 0xF) << 5) // Clock Divider Tuning for UPC Charge Pump Pull-Up (3'h0: divide by 1 / 3'h1: divide by 2 / 3'h2: divide by 4 / 3'h3: divide by 8 / 3'h4: divide by 16)
		| ((data_val2 & 0xF) << 2) // Clock Divider Tuning for DNC Charge Pump Pull-Up (3'h0: divide by 1 / 3'h1: divide by 2 / 3'h2: divide by 4 / 3'h3: divide by 8 / 3'h4: divide by 16)
		| (data_val3) // Clock Pre-Divider Tuning for UPC/DNC Charge Pump Pull-Up (2'h0: divide by 1 / 2'h1: divide by 2 / 2'h2: divide by 4 / 2'h3: divide by 8)
		 ));
		
		delay(MBUS_DELAY);
		
		pmu_reg_write(72, //0x48,
		    ( (0x2 << 4) //SAR_CP_CAP  = CP_CONFIG[5:4];  Default:0x3 (max)
		    | (0x0 << 2) //UPC_CP_CAP  = CP_CONFIG[3:2];  Default:0x3 (max)
		    | (0x1 << 0) //DNC_CP_CAP  = CP_CONFIG[1:0];  Default:0x3 (max) 
		));
   	}
   	else if(data_cmd == 0x0E){  // Adjust sleep and active floor settings for slow loop and active canary
		if(data_val==1){
			// Enable slow loop and canary	
			pmu_reg_write(71, //0x47, 
			( (1 << 19) // Enable RO-Assisted Transition
			| (0 << 18) // Enable SAR Slow-Loop in Active mode
			| (0 << 17) // Enable UPC Slow-Loop in Active mode
			| (0 << 16) // Enable DNC Slow-Loop in Active mode
			| (1 << 15) // Enable SAR Slow-Loop in Sleep mode
			| (1 << 14) // Enable UPC Slow-Loop in Sleep mode
			| (1 << 13) // Enable DNC Slow-Loop in Sleep mode
			| (1 << 11) // PRC ClokcGen Ring Tuning (2'h0: 13 stages / 2'h1: 11 stages / 2'h2: 9 stages / 2'h3: 7 stages)
			| (4 << 8) // Clock Divider Tuning for SAR Charge Pump Pull-Up (3'h0: divide by 1 / 3'h1: divide by 2 / 3'h2: divide by 4 / 3'h3: divide by 8 / 3'h4: divide by 16)
			| (2 << 5) // Clock Divider Tuning for UPC Charge Pump Pull-Up (3'h0: divide by 1 / 3'h1: divide by 2 / 3'h2: divide by 4 / 3'h3: divide by 8 / 3'h4: divide by 16)
			| (1 << 2) // Clock Divider Tuning for DNC Charge Pump Pull-Up (3'h0: divide by 1 / 3'h1: divide by 2 / 3'h2: divide by 4 / 3'h3: divide by 8 / 3'h4: divide by 16)
			| (3) // Clock Pre-Divider Tuning for UPC/DNC Charge Pump Pull-Up (2'h0: divide by 1 / 2'h1: divide by 2 / 2'h2: divide by 4 / 2'h3: divide by 8)
			 ));
		}
		if(data_val==0){
			pmu_reg_write(71, //0x47, 
			( (0 << 19) // Enable RO-Assisted Transition
			| (0 << 18) // Enable SAR Slow-Loop in Active mode
			| (0 << 17) // Enable UPC Slow-Loop in Active mode
			| (0 << 16) // Enable DNC Slow-Loop in Active mode
			| (0 << 15) // Enable SAR Slow-Loop in Sleep mode
			| (0 << 14) // Enable UPC Slow-Loop in Sleep mode
			| (0 << 13) // Enable DNC Slow-Loop in Sleep mode
			| (1 << 11) // PRC ClokcGen Ring Tuning (2'h0: 13 stages / 2'h1: 11 stages / 2'h2: 9 stages / 2'h3: 7 stages)
			| (3 << 8) // Clock Divider Tuning for SAR Charge Pump Pull-Up (3'h0: divide by 1 / 3'h1: divide by 2 / 3'h2: divide by 4 / 3'h3: divide by 8 / 3'h4: divide by 16)
			| (4 << 5) // Clock Divider Tuning for UPC Charge Pump Pull-Up (3'h0: divide by 1 / 3'h1: divide by 2 / 3'h2: divide by 4 / 3'h3: divide by 8 / 3'h4: divide by 16)
			| (2 << 2) // Clock Divider Tuning for DNC Charge Pump Pull-Up (3'h0: divide by 1 / 3'h1: divide by 2 / 3'h2: divide by 4 / 3'h3: divide by 8 / 3'h4: divide by 16)
			| (3) // Clock Pre-Divider Tuning for UPC/DNC Charge Pump Pull-Up (2'h0: divide by 1 / 2'h1: divide by 2 / 2'h2: divide by 4 / 2'h3: divide by 8)
			 ));
		}
		else if(data_val==2){
	   		pmu_set_active_clk(0x1,0x0,0x1,0x0);
   	   		delay(MBUS_DELAY*10); 
   	   	//	pmu_set_sleep_clks(0x6, 0x6, 0x4, 0x2);
   	   	//	pmu_set_sleep_clks(0x2, 0x1, 0x4, 0x1);
   	   	//	pmu_set_sleep_clks(0x6, 0x6, 0x4, 0x3);
			pmu_set_sleep_clks(0x1,0x1,0x4,0x1/*V1P2*/);
			pmu_set_sleep_clks(0x2,0x2,0x4,0x3/*V1P2*/);
			pmu_set_sleep_clks(0x3,0x2,0x4,0x2/*V1P2*/);
   	   		delay(MBUS_DELAY*5); 
    			pmu_set_sar_override(0x4C);
		}
		else if(data_val==3){
			pmu_reg_write(72, //0x48, 
			( (data_val2 << 10) // SAR_V1P2_DIVSEL[1:0] 
			| (1 << 9) // SAR_CMP_EN Override
			| (1 << 8) // UPC_CMP_EN Override
			| (1 << 7) // DNC_CMP_EN Override
			| (2 << 4) // SAR_CP_CAP
			| (0 << 2) // UPC_CP_CAP
			| (1) // DNC_CP_CAP
			 ));
		}
		else if(data_val==4){
			pmu_reg_write(12, data_val2);
		}

	}
   	else if(data_cmd == 0x0F){  // MIC VIR Cap Tune
		adov6vb_r16.MIC_VIR_CBASE = data_val;  // Default 2'b00
		mbus_remote_register_write(ADO_ADDR, 0x16, adov6vb_r16.as_int);//0535BD

	}
   	else if(data_cmd == 0x10){  //GPIO direction, trigger
   	    direction_gpio = data_val; // input:0, output:1,  eg. 0xF0 set GPIO[7:4] as output, GPIO[3:0] as input 
   	    init_gpio();
   	    config_gpio_posedge_wirq((data_val2>>4)&0xF);
   	    config_gpio_negedge_wirq(data_val2&0xF);
   	}
   	else if(data_cmd == 0x11){ //GPIO set 
   	    gpio_write_data(data_val);   
   	    delay(MBUS_DELAY*100);      //~1sec delay
   	    gpio_write_data(data_val2);   // since data is overwritten here
   	    mbus_write_message32(0xE5, *GPIO_DATA);
   	}
//  	else if(data_cmd == 0x21){ // Configure Temperature Sensor to run for PMU Adjustment
//  	    exec_count = 0;
//  	    meas_count = 0;
//  	    temp_storage_count = 0;
//
//  	    // Reset GOC_DATA_IRQ
//  	    *GOC_DATA_IRQ = 0;
//  	    exec_count_irq = 0;
//
//  	    // Mode Select
//  	    //  0: Turn OFF
//  	    //  1: Single SNT measurement wo PMU adjustment  
//  	    //  2: Single SNT measurement w/ PMU adjustment  
//  	    //  3: Repeating SNT measurement wo PMU adjustment
//  	    //  4: Repeating SNT measurement w/ PMU adustment
//  	    mode_select= (data_val3>>4 & 0xF); 
//
//  	    if(mode_select == 0x3 || mode_select == 0x4){
//  	        // Configure Parameters
//  	        //xo_count = wakeup_data & 0xFFFFF; //28000: ~2s
//  	        WAKEUP_PERIOD_CONT_USER= wakeup_data & 0xFFFF; 
//
//  	            // Use SNT timer
//  	            WAKEUP_PERIOD_SNT = (WAKEUP_PERIOD_CONT_USER<<1)*SNT_0P5S_VAL; // Unit is 1.0s
//  	        *TIMERWD_GO = 0x0;
//  	        snt_start_timer_presleep();
//  	        delay(MBUS_DELAY*900);
//  	        snt_start_timer_postsleep();
//  	            snt_set_timer_threshold(WAKEUP_PERIOD_SNT);
//  	        //delay(MBUS_DELAY*900);
//  	    }
//  	    else if(mode_select == 0x0){
//  	        snt_stop_timer();
//  	    }
//  	}
// 	else if(data_cmd == 0x22){ // Stop Temperature Sensor for PMU Adjustment
// 	    operation_sns_sleep_check();
//    	}
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
    
    mbus_write_message32(0xEE, 0x00000E1D);
    *NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0) | (1 << IRQ_REG1) | (1 << IRQ_REG2) | (1 << IRQ_REG3);

    while (*EP_MODE) config_timerwd(TIMERWD_VAL);

    mbus_write_message32(0xEE, 0x00000E2D);
    operation_sleep_notimer();
//    operation_sleep_xo_timer();
    return 0;
}
