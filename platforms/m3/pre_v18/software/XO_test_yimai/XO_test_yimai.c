//*******************************************************************
//Author: Sechang Oh
//Description: Leidos Gen3 
//          Modified from 'Leidos_durado_v2'
//          v1.0: created
//*******************************************************************
#include "PREv18.h"
#include "mbus.h"
#include "PMUv7_RF.h"


//Uncomment this for 1st load (Commend this for 2nd load)
#define ENUMID 0xDEADBEE1
#include "hexdata_mem_nzero_p3_simple.txt"  

////Uncomment this for 2nd load (Comment this for 1st load)
//#define ENUMID 0xDEADBEE2
//#include "hexdata_mem_nzero_p3_simple_2.txt"  

#include "LUT_table.txt"
#include "PHS_table.txt"

// uncomment this for debug mbus message
// #define DEBUG_MBUS_MSG
// #define DEBUG_MBUS_MSG_1

#define PMU_ADDR 0x6
#define WAKEUP_PERIOD_PARKING 30000 // About 2 hours (PRCv17)

// System parameters
#define MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)

// Pstack states
#define STK_IDLE        0x0
#define STK_LDO     0x1
#define STK_TEMP_START 0x2
#define STK_TEMP_READ  0x3

// CP parameter
#define CP_DELAY 50000 // Amount of delay between successive messages; 100: 6-7ms

//********************************************************************
// Scan values

//********************************************************************
#define SCAN_PHI    6
#define SCAN_PHI_BAR    5
#define SCAN_DATA_IN    7
#define SCAN_LOAD_CHIP  4
#define SCAN_LOAD_CHAIN 2
#define SCAN_DATA_OUT   3
#define DATA_NNET_VEC   1
#define CLK_NNET_VEC    0
#define scan_reset  0x0
#define scan_reset_BIT  1
#define scan2pad_inen   0x0
#define scan2pad_inen_BIT   1
volatile uint32_t scan2pad_outen;
#define scan2pad_outen_BIT  1
#define scan2pad_dir    0x0     // changed from default
#define scan2pad_dir_BIT    1
#define scan2AMP_SEL_MEMS   0x1F
#define scan2AMP_SEL_MEMS_BIT   5
#define scan2AMP_SEL_MEMS_BASE  0x0
#define scan2AMP_SEL_MEMS_BASE_BIT  1
#define scan2AMP_EN_BUF     0x1
#define scan2AMP_EN_BUF_BIT 1
volatile uint32_t scan2AMP_EN_FS;
#define scan2AMP_EN_FS_BIT  1
#define scan2AMP_vb1_sel    0x0
#define scan2AMP_vb1_sel_BIT    5
#define scan2AMP_vb2_sel    0x0
#define scan2AMP_vb2_sel_BIT    5
#define scan2AMP_vb3_sel    0x1
#define scan2AMP_vb3_sel_BIT    4
#define scan2AMP_vb4_sel    0x2
#define scan2AMP_vb4_sel_BIT    4
#define scan2AMP_ctrlb_IB   0xE
#define scan2AMP_ctrlb_IB_BIT   4
#define scan2AMP_ctrlb_Iamp 0xB
#define scan2AMP_ctrlb_Iamp_BIT 4
volatile uint32_t scan2AMP_SEL_Gain;
#define scan2AMP_SEL_Gain_BIT   6
#define scan2AMP_SEL_LOADp_BASE 0x0
#define scan2AMP_SEL_LOADp_BASE_BIT 1
#define scan2AMP_SEL_LOADp  0x5
#define scan2AMP_SEL_LOADp_BIT  5
#define scan2AMP_SEL_LOADn_BASE 0x0
#define scan2AMP_SEL_LOADn_BASE_BIT 1
#define scan2AMP_SEL_LOADn  0x5
#define scan2AMP_SEL_LOADn_BIT  5
#define scan2AMP_ctrlb_Iamp_2nd 0x15
#define scan2AMP_ctrlb_Iamp_2nd_BIT 5
#define scan2AMP_ctrlb_Iamp_2nd_p   0x3
#define scan2AMP_ctrlb_Iamp_2nd_p_BIT   2
#define scan2AMP_SEL_ADC    0x1
#define scan2AMP_SEL_ADC_BIT    1
#define scan2AMP_SEL_DRV    0x1
#define scan2AMP_SEL_DRV_BIT    1
#define scan2AMP_ctrl_IBUF_P    0x10
#define scan2AMP_ctrl_IBUF_P_BIT    5
#define scan2AMP_ctrl_IBUF_N    0x0
#define scan2AMP_ctrl_IBUF_N_BIT    1
volatile uint32_t scan2CP_RESETn;
#define scan2CP_RESETn_BIT  1
volatile uint32_t scan2CP_SEL_DIV; 
#define scan2CP_SEL_DIV_BIT 3
#define scan2CP_SEL_EXT_CLKb    0x1
#define scan2CP_SEL_EXT_CLKb_BIT    1

#define scan2CP_CP_CAS_EN   0x0
#define scan2CP_CP_CAS_EN_BIT   1
#define scan2CP_CP_CAS_VSH  0x0
#define scan2CP_CP_CAS_VSH_BIT  2
volatile uint32_t scan2CP_CP_HVI_EN;
#define scan2CP_CP_HVI_EN_BIT   1
volatile uint32_t scan2CP_CP_HVI_VSH;
#define scan2CP_CP_HVI_VSH_BIT  1

//#define scan2CS_SELb_I  0x2A
volatile uint32_t scan2CS_SELb_I;
#define scan2CS_SELb_I_BIT  6
#define scan2CS_SEL_I_DIVb  0x0
#define scan2CS_SEL_I_DIVb_BIT  1
#define scan2CS_SEL_I_EXT   0x0
#define scan2CS_SEL_I_EXT_BIT   1
#define scan2CS_SELb_I_DRV  0x1
#define scan2CS_SELb_I_DRV_BIT  1
volatile uint32_t scan2ADC_RESET;
#define scan2ADC_RESET_BIT  1
#define scan2ADC_OFFSET_SELP    0x0
#define scan2ADC_OFFSET_SELP_BIT    3
#define scan2ADC_OFFSET_SELN    0x0
#define scan2ADC_OFFSET_SELN_BIT    3
volatile uint32_t scan2ADC_DLY_SEL;
#define scan2ADC_DLY_SEL_BIT    3
volatile uint32_t scan2ADC_SDLY_SEL;
#define scan2ADC_SDLY_SEL_BIT   2
#define scan2CPSRAM_RESETn  0x0
#define scan2CPSRAM_RESETn_BIT  1
#define scan2CPSRAM_SEL_DIV 0x1
#define scan2CPSRAM_SEL_DIV_BIT 3
#define scan2CPSRAM_SHRT    0x0
#define scan2CPSRAM_SHRT_BIT    2
#define scan2CPSRAM_SEL_EXT_CLKb    0x1
#define scan2CPSRAM_SEL_EXT_CLKb_BIT    1
volatile uint32_t scan2DSP_RESETn_DSP;
#define scan2DSP_RESETn_DSP_BIT 1
//#define scan2DSP_EN_GC  0x1   //CLK AVG_PWR ON
#define scan2DSP_EN_GC  0x0     //CLK AVG_PWR OFF
#define scan2DSP_EN_GC_BIT  1
#define scan2DSP_EN_GC_ANA  0x0
#define scan2DSP_EN_GC_ANA_BIT  1
#define scan2DSP_SEL_EXT    0x0
#define scan2DSP_SEL_EXT_BIT    1
//#define scan2DSP_N_START    0x4 // Use MSB 4bits for DSP (shift 4bits)
#define scan2DSP_N_START    0x0   // Use LSB 4bits for DSP
#define scan2DSP_N_START_BIT    3
#define scan2DSP_N_FE   0x4
#define scan2DSP_N_FE_BIT   3
#define scan2DSP_N_DFT  0x0
#define scan2DSP_N_DFT_BIT  3
volatile uint32_t scan2DSP_PHS_WR;
#define scan2DSP_PHS_WR_BIT 1
volatile uint32_t scan2DSP_PHS_WR_ADDR;
#define scan2DSP_PHS_WR_ADDR_BIT    6
volatile uint32_t scan2DSP_PHS_DATA_IN;
#define scan2DSP_PHS_DATA_IN_BIT    10
volatile uint32_t scan2DSP_LUT_WR;
#define scan2DSP_LUT_WR_BIT 1
volatile uint32_t scan2DSP_LUT_WR_ADDR;
#define scan2DSP_LUT_WR_ADDR_BIT    3
volatile uint32_t scan2DSP_LUT_DATA_IN;
#define scan2DSP_LUT_DATA_IN_BIT    3
#define scan2DSP_SRAM_ADDR_BASE     0xF0
#define scan2DSP_SRAM_ADDR_BASE_BIT 12

#define scan2DSP_DNN_CLK_MON_EN     0x0
#define scan2DSP_DNN_CLK_MON_EN_BIT 1
#define scan2DSP_DNN_CLK_S_DIV      0x3
#define scan2DSP_DNN_CLK_S_DIV_BIT  2
#define scan2DSP_DNN_CLK_S_RING     0x4
#define scan2DSP_DNN_CLK_S_RING_BIT 3
//#define scan2DSP_SRAM0_SA_SEL   0x1     //inverter SA
#define scan2DSP_SRAM0_SA_SEL   0x0
#define scan2DSP_SRAM0_SA_SEL_BIT   1
#define scan2DSP_SRAM0_TUNE_DLY1    0x2
#define scan2DSP_SRAM0_TUNE_DLY1_BIT    4
#define scan2DSP_SRAM0_TUNE_DLY2    0x2
#define scan2DSP_SRAM0_TUNE_DLY2_BIT    4
#define scan2DSP_SRAM0_TUNE_RDRSB   0x0
#define scan2DSP_SRAM0_TUNE_RDRSB_BIT   4
#define scan2DSP_SRAM0_TUNE_RDRSB_EN    0x0
#define scan2DSP_SRAM0_TUNE_RDRSB_EN_BIT    1
//#define scan2DSP_SRAM1_SA_SEL   0x1     //inverter SA
#define scan2DSP_SRAM1_SA_SEL   0x0
#define scan2DSP_SRAM1_SA_SEL_BIT   1
#define scan2DSP_SRAM1_TUNE_DLY1    0x2
#define scan2DSP_SRAM1_TUNE_DLY1_BIT    4
#define scan2DSP_SRAM1_TUNE_DLY2    0x2
#define scan2DSP_SRAM1_TUNE_DLY2_BIT    4
#define scan2DSP_SRAM1_TUNE_RDRSB   0x0
#define scan2DSP_SRAM1_TUNE_RDRSB_BIT   4
#define scan2DSP_SRAM1_TUNE_RDRSB_EN    0x0
#define scan2DSP_SRAM1_TUNE_RDRSB_EN_BIT    1
#define scan2DSP_DNN_CLK_EXT_SEL        0x0
#define scan2DSP_DNN_CLK_EXT_SEL_BIT    1

volatile uint32_t scan2DSP_DNN_CLK_SCAN;
#define scan2DSP_DNN_CLK_SCAN_BIT   1
volatile uint32_t scan2DSP_DNN_CLK_SCAN_SEL;
#define scan2DSP_DNN_CLK_SCAN_SEL_BIT   1
volatile uint32_t scan2DSP_SRAM_EXT_SEL;
#define scan2DSP_SRAM_EXT_SEL_BIT   1
volatile uint32_t scan2DSP_SRAM_ADDR_EXT;
#define scan2DSP_SRAM_ADDR_EXT_BIT  12
volatile uint32_t scan2DSP_SRAM_R0W1_EXT;
#define scan2DSP_SRAM_R0W1_EXT_BIT  1
volatile uint32_t scan2DSP_SRAM_MEM_EN_EXT;
#define scan2DSP_SRAM_MEM_EN_EXT_BIT    1
volatile uint32_t scan2DSP_SRAM_DATAIN_EXT;
#define scan2DSP_SRAM_DATAIN_EXT_BIT    32
volatile uint32_t scan2DSP_DNN_CTRL_SCAN_SEL;
#define scan2DSP_DNN_CTRL_SCAN_SEL_BIT  1
volatile uint32_t scan2DSP_DNN_ISOLATEN_SCAN;
#define scan2DSP_DNN_ISOLATEN_SCAN_BIT  1
volatile uint32_t scan2DSP_PG_SLEEP_SCAN;
#define scan2DSP_PG_SLEEP_SCAN_BIT  1

#define scan2DSP_DNN_CLKENB_SCAN    0x1
#define scan2DSP_DNN_CLKENB_SCAN_BIT    1

volatile uint32_t scan2DSP_DNN_RESETN_SCAN;
#define scan2DSP_DNN_RESETN_SCAN_BIT    1

#define scan2DSP_DNN_EN_SCAN    0x0
#define scan2DSP_DNN_EN_SCAN_BIT    1

//#define scan2DSP_RF_MON_EN  0x1  //CLK, DATA AVG_PWR ON
#define scan2DSP_RF_MON_EN  0x0   //CLK, DATA AVG_PWR OFF
#define scan2DSP_RF_MON_EN_BIT  1     
#define scan2DSP_DBG_EN 0x0
#define scan2DSP_DBG_EN_BIT 1
volatile uint32_t scan2Timer_EN_OSC;
#define scan2Timer_EN_OSC_BIT   1
#define scan2Timer_AFC  0x1
#define scan2Timer_AFC_BIT  3
#define scan2Timer_CASCODE_BOOST    0x1
#define scan2Timer_CASCODE_BOOST_BIT    1
#define scan2Timer_IBIAS_REF    0x4
#define scan2Timer_IBIAS_REF_BIT    4
#define scan2Timer_RESETB   0x0
#define scan2Timer_RESETB_BIT   1
#define scan2Timer_RESETB_DIV   0x0
#define scan2Timer_RESETB_DIV_BIT   1
#define scan2Timer_S    0x7
#define scan2Timer_S_BIT    3
#define scan2Timer_SAMPLE_EN    0x1
#define scan2Timer_SAMPLE_EN_BIT    1
#define scan2Timer_SELF_EN  0x1
#define scan2Timer_SELF_EN_BIT  1
#define scan2Timer_DIFF_CON     0x3FF
#define scan2Timer_DIFF_CON_BIT 14
#define scan2Timer_POLY_CON 0x1
#define scan2Timer_POLY_CON_BIT 1
#define scan2Timer_EN_TUNE1_RES 0x0
#define scan2Timer_EN_TUNE1_RES_BIT 1
#define scan2Timer_EN_TUNE2_RES 0x0
#define scan2Timer_EN_TUNE2_RES_BIT 1
#define scan2Timer_SEL_CAP  0x0
#define scan2Timer_SEL_CAP_BIT  4
#define scan2Timer_EN_SELF_CLK  0x0
#define scan2Timer_EN_SELF_CLK_BIT  1
#define scan2Timer_RESETB_DCDC  0x0
#define scan2Timer_RESETB_DCDC_BIT  1
#define scan2Timer_SEL_CLK_DIV  0x1
#define scan2Timer_SEL_CLK_DIV_BIT  1
#define scan2Timer_SEL_CLK_OSC  0x1
#define scan2Timer_SEL_CLK_OSC_BIT  1
#define scan2Timer_SEL_LC_CURR_EXT  0x0
#define scan2Timer_SEL_LC_CURR_EXT_BIT  1
#define scan2Timer_LC_BIAS  0x1
#define scan2Timer_LC_BIAS_BIT  3
#define scan2Timer_SEL_D    0x2
#define scan2Timer_SEL_D_BIT    2
#define scan2Timer_SEL_VB   0x13
#define scan2Timer_SEL_VB_BIT   6
#define scan2Timer_SEL_VBH  0x0
#define scan2Timer_SEL_VBH_BIT  1
#define scan2Timer_SEL_D_TM2    0x7
#define scan2Timer_SEL_D_TM2_BIT    3
#define scan2Timer_AFC_TM2  0x7
#define scan2Timer_AFC_TM2_BIT  3
volatile uint32_t scan2Timer_RESETb_TM2;
#define scan2Timer_RESETb_TM2_BIT   1
volatile uint32_t scan2Timer_SEL_CLK;   // 0: EXT, 1:1nW time 2: SLOSC
#define scan2Timer_SEL_CLK_BIT  2
#define scan2mux_sel_clk    0x0
#define scan2mux_sel_clk_BIT    1
#define DSP2scan_SRAM_DATAOUT_EXT       0x0
#define DSP2scan_SRAM_DATAOUT_EXT_BIT   32

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)


//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t irq_history;
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t stack_state;
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


static void operation_sleep_notimer(void);
static void pmu_set_sar_override(uint32_t val);
static void program_scan_sram(void);
static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);
static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);
//*******************************************************************
// XO Functions
//*******************************************************************
//
static void XO_div(uint32_t divVal) {
    prev18_r19.XO_S = divVal;
    *REG_XO_CONF1 = prev18_r19.as_int;
}

static void XO_init(void) {
    // Parasitic Capacitance Tuning (6-bit for each; Each 1 adds 1.8pF)
    // Yimai
    uint32_t xo_cap_drv = 0x3F; // Additional Cap on OSC_DRV
    uint32_t xo_cap_in  = 0x3F; // Additional Cap on OSC_IN
    // Yimai
    prev18_r1A.XO_CAP_TUNE = (
            (xo_cap_drv <<6) | 
            (xo_cap_in <<0));   // XO_CLK Output Pad 
    *REG_XO_CONF2 = prev18_r1A.as_int;

    // XO configuration
    prev18_r19.XO_EN_DIV    = 0x1;// divider enable
    prev18_r19.XO_S         = 0x0;// division ratio 0: 32kHz
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
    delay(1000);
    prev18_r19.XO_SCN_CLK_SEL   = 0x1;// scn clock 1: normal. 0.3V level up to 0.6V, 0:init
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(1000);
    prev18_r19.XO_SCN_CLK_SEL   = 0x0;
    prev18_r19.XO_SCN_ENB       = 0x0;// enable_bar of scn
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(1000);
    prev18_r19.XO_DRV_START_UP  = 0x0;
    prev18_r19.XO_DRV_CORE      = 0x1;// 1: enables core circuit
    prev18_r19.XO_SCN_CLK_SEL   = 0x1;
    *REG_XO_CONF1 = prev18_r19.as_int;

    enable_xo_timer();
    start_xo_cout();
}


//************************************
// GPIO Functions
// ***********************************
static void init_scan(void){
    gpio_init( (1 << SCAN_PHI) | (1 << SCAN_PHI_BAR) | (1 << SCAN_DATA_IN) | (1 << SCAN_LOAD_CHIP) | (1 << SCAN_LOAD_CHAIN));
    set_gpio_pad(0xFC);
}

static void program_scan_word(uint32_t writeData, uint32_t nBits){
    uint32_t i;
    uint8_t bitdata;
    bitdata = 0;
    for(i = 0; i < nBits; i++){
        bitdata = (writeData >> i) & 1;
        *GPIO_DATA = ( (bitdata << SCAN_DATA_IN) );
        *GPIO_DATA = ( (bitdata << SCAN_DATA_IN) | (1 << SCAN_PHI) );
        *GPIO_DATA = ( (bitdata << SCAN_DATA_IN) );
        *GPIO_DATA = ( (bitdata << SCAN_DATA_IN) | (1 << SCAN_PHI_BAR) );
    }
}

static void program_scan(void){
    *GPIO_DATA=0;
    //program_scan_word(0, 1);
    program_scan_word(scan_reset, scan_reset_BIT);
    program_scan_word(scan2pad_inen, scan2pad_inen_BIT);
    program_scan_word(scan2pad_outen, scan2pad_outen_BIT);
    program_scan_word(scan2pad_dir, scan2pad_dir_BIT);
    program_scan_word(scan2AMP_SEL_MEMS, scan2AMP_SEL_MEMS_BIT);
    program_scan_word(scan2AMP_SEL_MEMS_BASE, scan2AMP_SEL_MEMS_BASE_BIT);
    program_scan_word(scan2AMP_EN_BUF, scan2AMP_EN_BUF_BIT);
    program_scan_word(scan2AMP_EN_FS, scan2AMP_EN_FS_BIT);
    program_scan_word(scan2AMP_vb1_sel, scan2AMP_vb1_sel_BIT);
    program_scan_word(scan2AMP_vb2_sel, scan2AMP_vb2_sel_BIT);
    program_scan_word(scan2AMP_vb3_sel, scan2AMP_vb3_sel_BIT);
    program_scan_word(scan2AMP_vb4_sel, scan2AMP_vb4_sel_BIT);
    program_scan_word(scan2AMP_ctrlb_IB, scan2AMP_ctrlb_IB_BIT);
    program_scan_word(scan2AMP_ctrlb_Iamp, scan2AMP_ctrlb_Iamp_BIT);
    program_scan_word(scan2AMP_SEL_Gain, scan2AMP_SEL_Gain_BIT);
    program_scan_word(scan2AMP_SEL_LOADp_BASE, scan2AMP_SEL_LOADp_BASE_BIT);
    program_scan_word(scan2AMP_SEL_LOADp, scan2AMP_SEL_LOADp_BIT);
    program_scan_word(scan2AMP_SEL_LOADn_BASE, scan2AMP_SEL_LOADn_BASE_BIT);
    program_scan_word(scan2AMP_SEL_LOADn, scan2AMP_SEL_LOADn_BIT);
    program_scan_word(scan2AMP_ctrlb_Iamp_2nd, scan2AMP_ctrlb_Iamp_2nd_BIT);
    program_scan_word(scan2AMP_ctrlb_Iamp_2nd_p, scan2AMP_ctrlb_Iamp_2nd_p_BIT);
    program_scan_word(scan2AMP_SEL_ADC, scan2AMP_SEL_ADC_BIT);
    program_scan_word(scan2AMP_SEL_DRV, scan2AMP_SEL_DRV_BIT);
    program_scan_word(scan2AMP_ctrl_IBUF_P, scan2AMP_ctrl_IBUF_P_BIT);
    program_scan_word(scan2AMP_ctrl_IBUF_N, scan2AMP_ctrl_IBUF_N_BIT);
    program_scan_word(scan2CP_RESETn, scan2CP_RESETn_BIT);
    program_scan_word(scan2CP_SEL_DIV, scan2CP_SEL_DIV_BIT);
    program_scan_word(scan2CP_SEL_EXT_CLKb, scan2CP_SEL_EXT_CLKb_BIT);
    program_scan_word(scan2CP_CP_CAS_EN, scan2CP_CP_CAS_EN_BIT);
    program_scan_word(scan2CP_CP_CAS_VSH, scan2CP_CP_CAS_VSH_BIT);
    program_scan_word(scan2CP_CP_HVI_EN, scan2CP_CP_HVI_EN_BIT);
    program_scan_word(scan2CP_CP_HVI_VSH, scan2CP_CP_HVI_VSH_BIT);
    program_scan_word(scan2CS_SELb_I, scan2CS_SELb_I_BIT);
    program_scan_word(scan2CS_SEL_I_DIVb, scan2CS_SEL_I_DIVb_BIT);
    program_scan_word(scan2CS_SEL_I_EXT, scan2CS_SEL_I_EXT_BIT);
    program_scan_word(scan2CS_SELb_I_DRV, scan2CS_SELb_I_DRV_BIT);
    program_scan_word(scan2ADC_RESET, scan2ADC_RESET_BIT);
    program_scan_word(scan2ADC_OFFSET_SELP, scan2ADC_OFFSET_SELP_BIT);
    program_scan_word(scan2ADC_OFFSET_SELN, scan2ADC_OFFSET_SELN_BIT);
    program_scan_word(scan2ADC_DLY_SEL, scan2ADC_DLY_SEL_BIT);
    program_scan_word(scan2ADC_SDLY_SEL, scan2ADC_SDLY_SEL_BIT);
    program_scan_word(scan2CPSRAM_RESETn, scan2CPSRAM_RESETn_BIT);
    program_scan_word(scan2CPSRAM_SEL_DIV, scan2CPSRAM_SEL_DIV_BIT);
    program_scan_word(scan2CPSRAM_SHRT, scan2CPSRAM_SHRT_BIT);
    program_scan_word(scan2CPSRAM_SEL_EXT_CLKb, scan2CPSRAM_SEL_EXT_CLKb_BIT);
    program_scan_word(scan2DSP_RESETn_DSP, scan2DSP_RESETn_DSP_BIT);
    program_scan_word(scan2DSP_EN_GC, scan2DSP_EN_GC_BIT);
    program_scan_word(scan2DSP_EN_GC_ANA, scan2DSP_EN_GC_ANA_BIT);
    program_scan_word(scan2DSP_SEL_EXT, scan2DSP_SEL_EXT_BIT);
    program_scan_word(scan2DSP_N_START, scan2DSP_N_START_BIT);
    program_scan_word(scan2DSP_N_FE, scan2DSP_N_FE_BIT);
    program_scan_word(scan2DSP_N_DFT, scan2DSP_N_DFT_BIT);
    program_scan_word(scan2DSP_PHS_WR, scan2DSP_PHS_WR_BIT);
    program_scan_word(scan2DSP_PHS_WR_ADDR, scan2DSP_PHS_WR_ADDR_BIT);
    program_scan_word(scan2DSP_PHS_DATA_IN, scan2DSP_PHS_DATA_IN_BIT);
    program_scan_word(scan2DSP_LUT_WR, scan2DSP_LUT_WR_BIT);
    program_scan_word(scan2DSP_LUT_WR_ADDR, scan2DSP_LUT_WR_ADDR_BIT);
    program_scan_word(scan2DSP_LUT_DATA_IN, scan2DSP_LUT_DATA_IN_BIT);
    program_scan_word(scan2DSP_SRAM_ADDR_BASE, scan2DSP_SRAM_ADDR_BASE_BIT);
    
    program_scan_word(scan2DSP_DNN_CLK_MON_EN, scan2DSP_DNN_CLK_MON_EN_BIT);
    program_scan_word(scan2DSP_DNN_CLK_S_DIV, scan2DSP_DNN_CLK_S_DIV_BIT);
    program_scan_word(scan2DSP_DNN_CLK_S_RING, scan2DSP_DNN_CLK_S_RING_BIT);
    program_scan_word(scan2DSP_SRAM0_SA_SEL,        scan2DSP_SRAM0_SA_SEL_BIT);
    program_scan_word(scan2DSP_SRAM0_TUNE_DLY1,     scan2DSP_SRAM0_TUNE_DLY1_BIT);
    program_scan_word(scan2DSP_SRAM0_TUNE_DLY2,     scan2DSP_SRAM0_TUNE_DLY2_BIT);
    program_scan_word(scan2DSP_SRAM0_TUNE_RDRSB,    scan2DSP_SRAM0_TUNE_RDRSB_BIT);
    program_scan_word(scan2DSP_SRAM0_TUNE_RDRSB_EN, scan2DSP_SRAM0_TUNE_RDRSB_EN_BIT);
    program_scan_word(scan2DSP_SRAM1_SA_SEL,        scan2DSP_SRAM1_SA_SEL_BIT);
    program_scan_word(scan2DSP_SRAM1_TUNE_DLY1,     scan2DSP_SRAM1_TUNE_DLY1_BIT);
    program_scan_word(scan2DSP_SRAM1_TUNE_DLY2,     scan2DSP_SRAM1_TUNE_DLY2_BIT);
    program_scan_word(scan2DSP_SRAM1_TUNE_RDRSB,    scan2DSP_SRAM1_TUNE_RDRSB_BIT);
    program_scan_word(scan2DSP_SRAM1_TUNE_RDRSB_EN, scan2DSP_SRAM1_TUNE_RDRSB_EN_BIT);
    program_scan_word(scan2DSP_DNN_CLK_EXT_SEL, scan2DSP_DNN_CLK_EXT_SEL_BIT);
    program_scan_word(scan2DSP_DNN_CLK_SCAN, scan2DSP_DNN_CLK_SCAN_BIT);
    program_scan_word(scan2DSP_DNN_CLK_SCAN_SEL, scan2DSP_DNN_CLK_SCAN_SEL_BIT);
    program_scan_word(scan2DSP_SRAM_EXT_SEL, scan2DSP_SRAM_EXT_SEL_BIT);
    program_scan_word(scan2DSP_SRAM_ADDR_EXT, scan2DSP_SRAM_ADDR_EXT_BIT);
    program_scan_word(scan2DSP_SRAM_R0W1_EXT, scan2DSP_SRAM_R0W1_EXT_BIT);
    program_scan_word(scan2DSP_SRAM_MEM_EN_EXT, scan2DSP_SRAM_MEM_EN_EXT_BIT);
    program_scan_word(scan2DSP_SRAM_DATAIN_EXT, scan2DSP_SRAM_DATAIN_EXT_BIT);
    program_scan_word(scan2DSP_DNN_CTRL_SCAN_SEL, scan2DSP_DNN_CTRL_SCAN_SEL_BIT);
    program_scan_word(scan2DSP_DNN_ISOLATEN_SCAN, scan2DSP_DNN_ISOLATEN_SCAN_BIT);
    program_scan_word(scan2DSP_PG_SLEEP_SCAN, scan2DSP_PG_SLEEP_SCAN_BIT);
    program_scan_word(scan2DSP_DNN_CLKENB_SCAN, scan2DSP_DNN_CLKENB_SCAN_BIT);
    program_scan_word(scan2DSP_DNN_RESETN_SCAN, scan2DSP_DNN_RESETN_SCAN_BIT);
    program_scan_word(scan2DSP_DNN_EN_SCAN, scan2DSP_DNN_EN_SCAN_BIT);
    program_scan_word(scan2DSP_RF_MON_EN, scan2DSP_RF_MON_EN_BIT);
    program_scan_word(scan2DSP_DBG_EN, scan2DSP_DBG_EN_BIT);
    program_scan_word(scan2Timer_EN_OSC, scan2Timer_EN_OSC_BIT);
    program_scan_word(scan2Timer_AFC, scan2Timer_AFC_BIT);
    program_scan_word(scan2Timer_CASCODE_BOOST, scan2Timer_CASCODE_BOOST_BIT);
    program_scan_word(scan2Timer_IBIAS_REF, scan2Timer_IBIAS_REF_BIT);
    program_scan_word(scan2Timer_RESETB, scan2Timer_RESETB_BIT);
    program_scan_word(scan2Timer_RESETB_DIV, scan2Timer_RESETB_DIV_BIT);
    program_scan_word(scan2Timer_S, scan2Timer_S_BIT);
    program_scan_word(scan2Timer_SAMPLE_EN, scan2Timer_SAMPLE_EN_BIT);
    program_scan_word(scan2Timer_SELF_EN, scan2Timer_SELF_EN_BIT);
    program_scan_word(scan2Timer_DIFF_CON, scan2Timer_DIFF_CON_BIT);
    program_scan_word(scan2Timer_POLY_CON, scan2Timer_POLY_CON_BIT);
    program_scan_word(scan2Timer_EN_TUNE1_RES, scan2Timer_EN_TUNE1_RES_BIT);
    program_scan_word(scan2Timer_EN_TUNE2_RES, scan2Timer_EN_TUNE2_RES_BIT);
    program_scan_word(scan2Timer_SEL_CAP, scan2Timer_SEL_CAP_BIT);
    program_scan_word(scan2Timer_EN_SELF_CLK, scan2Timer_EN_SELF_CLK_BIT);
    program_scan_word(scan2Timer_RESETB_DCDC, scan2Timer_RESETB_DCDC_BIT);
    program_scan_word(scan2Timer_SEL_CLK_DIV, scan2Timer_SEL_CLK_DIV_BIT);
    program_scan_word(scan2Timer_SEL_CLK_OSC, scan2Timer_SEL_CLK_OSC_BIT);
    program_scan_word(scan2Timer_SEL_LC_CURR_EXT, scan2Timer_SEL_LC_CURR_EXT_BIT);
    program_scan_word(scan2Timer_LC_BIAS, scan2Timer_LC_BIAS_BIT);
    program_scan_word(scan2Timer_SEL_D, scan2Timer_SEL_D_BIT);
    program_scan_word(scan2Timer_SEL_VB, scan2Timer_SEL_VB_BIT);
    program_scan_word(scan2Timer_SEL_VBH, scan2Timer_SEL_VBH_BIT);
    program_scan_word(scan2Timer_SEL_D_TM2, scan2Timer_SEL_D_TM2_BIT);
    program_scan_word(scan2Timer_AFC_TM2, scan2Timer_AFC_TM2_BIT);
    program_scan_word(scan2Timer_RESETb_TM2, scan2Timer_RESETb_TM2_BIT);
    program_scan_word(scan2Timer_SEL_CLK, scan2Timer_SEL_CLK_BIT);
    program_scan_word(scan2mux_sel_clk, scan2mux_sel_clk_BIT);
    program_scan_word(DSP2scan_SRAM_DATAOUT_EXT, DSP2scan_SRAM_DATAOUT_EXT_BIT);
    *GPIO_DATA = ( (1 << SCAN_LOAD_CHIP) );
    *GPIO_DATA = 0;
}

static void program_scan_sram(void){
    uint32_t i;
    mbus_write_message32(0xEE, 0xDEAD);
    delay(MBUS_DELAY);
    // Sprint PRE clock frequency
    prev18_r0B.CLK_GEN_DIV_CORE = 0x0; // 8x than 0x3(default)
    *REG_CLKGEN_TUNE = prev18_r0B.as_int;

    scan2DSP_SRAM_EXT_SEL= 1;
    scan2DSP_DNN_CLK_SCAN_SEL = 1;
    scan2DSP_DNN_CTRL_SCAN_SEL = 1;
    program_scan();
    //mbus_write_message32(0xEE, 0xDEAD);
    scan2DSP_PG_SLEEP_SCAN = 0;
    program_scan();
    //mbus_write_message32(0xEE, 0xDEAD);
    scan2DSP_DNN_ISOLATEN_SCAN = 1;
    program_scan();
    scan2DSP_DNN_RESETN_SCAN = 1;
    program_scan();

    for(i = 0; i < 4; i++){
        scan2DSP_DNN_CLK_SCAN = 1;
        program_scan();
        scan2DSP_DNN_CLK_SCAN = 0;
        program_scan();
    }

    scan2DSP_SRAM_MEM_EN_EXT = 1;
    scan2DSP_SRAM_R0W1_EXT = 1;
    program_scan();
    mbus_write_message32(0xEE, 0xDEAE);

    scan2DSP_SRAM_ADDR_EXT = SRAM_FIRST_ADDR;
    for(i = 0; i < SRAM_DATA_LENGTH; i++){
        scan2DSP_SRAM_DATAIN_EXT = SRAM_DATA[i];
        program_scan();
        scan2DSP_DNN_CLK_SCAN = 1;
        program_scan();
        scan2DSP_DNN_CLK_SCAN = 0;
        scan2DSP_SRAM_ADDR_EXT = scan2DSP_SRAM_ADDR_EXT + 1;
        program_scan();
        if( (i & 0x1F) == 0x1) {
            mbus_write_message32(0xEE, 0xDEAD);   //debug message every 32 write 
            delay(MBUS_DELAY);
        }
    }

    scan2DSP_SRAM_MEM_EN_EXT = 0;
    scan2DSP_SRAM_R0W1_EXT = 0;
    program_scan();
    scan2DSP_DNN_RESETN_SCAN = 0;
    program_scan();
    scan2DSP_DNN_ISOLATEN_SCAN = 0;
    program_scan();
    scan2DSP_PG_SLEEP_SCAN = 1;
    program_scan();
    scan2DSP_SRAM_EXT_SEL= 0;
    scan2DSP_DNN_CLK_SCAN_SEL = 0;
    scan2DSP_DNN_CTRL_SCAN_SEL = 0;
    program_scan();
    //scan2DSP_RESETn_DSP = 1;
    //program_scan();
    mbus_write_message32(0xEE, 0xDEAF);

    // Return original PRE clock frequency
    prev18_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
    *REG_CLKGEN_TUNE = prev18_r0B.as_int;
}

static void program_frequencies(void){
    uint32_t i;
    mbus_write_message32(0xEE, 0xDEAC);
    scan2DSP_PHS_WR_ADDR = 0;
    for(i = 0; i < PHS_DATA_LENGTH; i++){
        scan2DSP_PHS_DATA_IN = PHS_DATA[i];
        program_scan();
        mbus_write_message32(0xEE, 0xDEAD);
        scan2DSP_PHS_WR = 1;
        program_scan();
        scan2DSP_PHS_WR = 0;
        program_scan();
        scan2DSP_PHS_WR_ADDR = scan2DSP_PHS_WR_ADDR + 1;
    }

    mbus_write_message32(0xEE, 0xDEAE);
}

static void set_LUT_values(void){
    uint32_t i;
    mbus_write_message32(0xEE, 0xDEAC);
    scan2DSP_LUT_WR_ADDR = 0;
    for(i = 0; i < LUT_DATA_LENGTH; i++){
        scan2DSP_LUT_DATA_IN = LUT_DATA[i];
        program_scan();
        mbus_write_message32(0xEE, 0xDEAD);
        scan2DSP_LUT_WR = 1;
        program_scan();
        scan2DSP_LUT_WR = 0;
        program_scan();
        scan2DSP_LUT_WR_ADDR = scan2DSP_LUT_WR_ADDR + 1;
    }
    mbus_write_message32(0xEE, 0xDEAE);
}

static void program_scan_cp(uint32_t data_val){
    if (data_val == 1) {    //CP_VSH=1
        scan2CP_CP_HVI_EN   = 1;
        scan2CP_CP_HVI_VSH  = 0;
        scan2CP_SEL_DIV     = 0;
        //scan2CP_RESETn      = 1;
        program_scan();
        delay(CP_DELAY);    
        scan2CP_CP_HVI_VSH  = 1;
        program_scan();
    }
    else if (data_val == 2) {   //CP_VSH=0
        scan2CP_CP_HVI_EN   = 1;
        scan2CP_CP_HVI_VSH  = 1;
        scan2CP_SEL_DIV     = 0;
        //scan2CP_RESETn      = 1;
        program_scan();
        delay(CP_DELAY);    
        scan2CP_CP_HVI_VSH  = 0;
        program_scan();
    }
    else {
        //scan2CP_RESETn      = 0;
        scan2CP_SEL_DIV     = 1;
        program_scan();
    }
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
// End of Program Sleep Operation
//***************************************************

static void operation_sleep(void){

    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;

    // Go to Sleep
    mbus_sleep_all();
    while(1);

}


static void operation_sleep_notimer(void){
    // Make sure the irq counter is reset    
    exec_count_irq = 0;

    // Check if sleep parking lot is on
    if (pmu_parkinglot_mode & 0x2){
        set_wakeup_timer(WAKEUP_PERIOD_PARKING,0x1,0x1);
    }else{
        // Disable Timer
        set_wakeup_timer(0, 0, 0);
    }

    // Go to sleep
    operation_sleep();
}


static void operation_init(void){
    pmu_sar_conv_ratio_val_test_on = 0x2E;//0x2D;
    pmu_sar_conv_ratio_val_test_off = 0x2C;//0x2A;
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);
    *TIMERWD_GO = 0x0;

    *REG_MBUS_WD = 0;
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
    stack_state = STK_IDLE;     //0x0;
    enumerated = ENUMID;
    exec_count = 0;
    exec_count_irq = 0;
    PMU_ADC_3P0_VAL = 0x62;
    pmu_parkinglot_mode = 3;

    //Enumeration
    mbus_enumerate(PMU_ADDR);
    delay(MBUS_DELAY);

    // Set CPU Halt Option as TX --> Use for register write e.g.
    //    set_halt_until_mbus_tx();


    // PMU Settings ----------------------------------------------
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

    *REG_CPS = 0x7; // 4: 0.6V, 2: TEST1.2V, 1: All 1.2V
    delay(MBUS_DELAY*100);
    //mbus_write_message32(0xFF,0xF0F0);

    // Initialize NZ
    scan2pad_outen=0x1;
    scan2AMP_EN_FS=0x0;
    scan2AMP_SEL_Gain=0x3;
    scan2CP_RESETn=0;
    scan2CP_SEL_DIV=1;
    scan2CP_CP_HVI_EN=0;
    scan2CP_CP_HVI_VSH=0;
    scan2CS_SELb_I=0x2A;
    scan2ADC_RESET=0x1;
    scan2ADC_DLY_SEL=0x1;
    scan2ADC_SDLY_SEL=0x3;
    scan2DSP_RESETn_DSP = 0;
    scan2DSP_PHS_WR = 0;
    scan2DSP_PHS_WR_ADDR=0x20;
    scan2DSP_PHS_DATA_IN=0x6B;
    scan2DSP_LUT_WR = 0;
    scan2DSP_LUT_WR_ADDR = 0x8;
    scan2DSP_LUT_DATA_IN = 0x7;
    scan2DSP_DNN_CLK_SCAN=0x0;
    scan2DSP_DNN_CLK_SCAN_SEL=0x0;
    scan2DSP_SRAM_EXT_SEL=0x0;
    scan2DSP_SRAM_ADDR_EXT=0x552;
    scan2DSP_SRAM_R0W1_EXT=0x0;
    scan2DSP_SRAM_MEM_EN_EXT=0x0;
    scan2DSP_SRAM_DATAIN_EXT=0x00000000;
    scan2DSP_DNN_CTRL_SCAN_SEL=0x0;
    scan2DSP_DNN_ISOLATEN_SCAN=0x0;
    scan2DSP_PG_SLEEP_SCAN=0x1;
    scan2DSP_DNN_RESETN_SCAN=0x0;
    scan2Timer_EN_OSC=0x0;
    scan2Timer_RESETb_TM2=0x0;
    scan2Timer_SEL_CLK=0x0;
    
    init_scan();
    program_scan();
    set_LUT_values();
    mbus_write_message32(0xAF,0x0FAF);
    delay(MBUS_DELAY);

    XO_init();
    scan2ADC_RESET=0x0;
    program_scan();
    program_scan_cp(1);
    mbus_write_message32(0xAF,0x0FAE);
    delay(MBUS_DELAY);
}

//*******************************************************************
// INTERRUPT HANDLERS (Updated for PRCv17)
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
    *SREG_WAKEUP_SOURCE = 0;
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
    uint32_t data_val2 = (wakeup_data>>8) & 0xFF;
    uint32_t data_val = wakeup_data & 0xFF;
    //uint32_t data_cmd = *REG1;
    //uint32_t data_val = *REG0;
    init_scan();
    unfreeze_gpio_out();

    mbus_write_message32(0xEE, data_cmd);
    delay(MBUS_DELAY*10);
    mbus_write_message32(0xEE, data_val);

    if(data_cmd == 0x11){      // Fast settling enable
        if(data_val) scan2AMP_EN_FS=0;
        else scan2AMP_EN_FS=1;
        program_scan();
    }
    else if(data_cmd == 1){     // CP enable
        program_scan_cp(data_val);
    }
    else if(data_cmd == 2){     // ADC enable
        if(data_val) scan2ADC_RESET=0;
        else scan2ADC_RESET=1;
        program_scan();
    }
    else if(data_cmd == 3){     // Current tuning
        scan2CS_SELb_I = data_val;
        program_scan();
    }
    else if(data_cmd == 4){     // Choose clock source
        if(data_val==0){    // Crystal
            scan2Timer_SEL_CLK=0;   
            scan2Timer_EN_OSC =0;
            scan2Timer_RESETb_TM2=0;
        }
        else if(data_val==2){      //TM2 
            scan2Timer_SEL_CLK=2;
            scan2Timer_EN_OSC=0;
            scan2Timer_RESETb_TM2=1;
        }
        else{
            scan2Timer_SEL_CLK=0;   
            scan2Timer_EN_OSC =0;
            scan2Timer_RESETb_TM2=0;
        }
        program_scan();
    }
    else if(data_cmd == 5){     // Divider selection
        if(data_val == 0) XO_div(0);        //32kHz
        else if(data_val == 1) XO_div(1);   //16kHz
        else if(data_val == 2) XO_div(2);   //8kHz
        else if(data_val == 3) XO_div(3);   //4kHz
        else if(data_val == 4) XO_div(4);   //2kHz
        else if(data_val == 5) XO_div(5);   //1kHz - Default
        else {
            prev18_r19.XO_EN_DIV = 0x0;     //Divider disable
            *REG_XO_CONF1 = prev18_r19.as_int;
        }
    }
    else if(data_cmd ==6){
        scan2AMP_SEL_Gain=data_val;
        program_scan();
    }
    else if(data_cmd ==7){      // Test VDD enable
        if(data_val == 0){  //TESTVDD OFF
            *REG_CPS = 0x5; // 4: 0.6V, 2: TEST1.2V, 1: All 1.2V
            pmu_set_sar_override(pmu_sar_conv_ratio_val_test_off);
            pmu_set_sleep_clk(0xF,0xF,0xF,0xF);
            pmu_set_active_clk(0xA,0x1,0x10,0x2);
        }
        else{   //TESTVDD ON
            pmu_set_sar_override(pmu_sar_conv_ratio_val_test_on);
            pmu_set_sleep_clk(0xA,0x1,0x10,0x2);
            pmu_set_active_clk(0xA,0x1,0x10,0x2);
            *REG_CPS = 0x7; // 4: 0.6V, 2: TEST1.2V, 1: All 1.2V
        }
    }
    else if(data_cmd ==8){
        pmu_set_sar_override(data_val);
    }
    else if(data_cmd ==9){      // DSP enable
        if(data_val == 0) scan2DSP_RESETn_DSP = 0;
        else scan2DSP_RESETn_DSP = 1;
        program_scan();
    }
    else if (data_cmd == 0xA){  //PHS set
        program_frequencies();
    }
    else if(data_cmd ==0xB){
        //if(data_val == 0 | data_val == 1) pmu_set_sleep_clk(2,data_val,0x7,data_val);
        //else pmu_set_sleep_clk(data_val,data_val,0x7,data_val);
        if(data_val != 0xF) pmu_set_sleep_clk(data_val+1,data_val,data_val2,data_val);
        else pmu_set_sleep_clk(data_val,data_val,data_val2,data_val);
    }
    else if(data_cmd == 0xFF){
        program_scan_sram();
    }
    else if(data_cmd == 0xCC){
        prev18_r1A.XO_CAP_TUNE = (
            (data_val2 <<6) | 
            (data_val <<0));   // XO_CLK Output Pad 
        *REG_XO_CONF2 = prev18_r1A.as_int;
    }
    freeze_gpio_out();
    operation_sleep_notimer();
}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    // Only enable relevant interrupts (PRCv17)
    *NVIC_ISER = (1 << IRQ_GOCEP);

    // Initialization sequence
    if (enumerated != ENUMID){
        operation_init();
    }

    operation_sleep_notimer();
    return 0;
}


