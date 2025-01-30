//*******************************************************************
//Author: Gyouho Kim
//Description: Pressure Sensing System with Seiko Battery
//            Based on 'Tstack_Ondemand_v6.5', 'TSstack_ondemand_v1.4b'
//			  v6.6: Using CISv1A (PREv20E)
//					Fix SAR ratio if the temp is above ~40C (FIXME)
//					No harvester
//			  v6.7a: Take 4 RDC readings with different offset, store non-saturated one
//*******************************************************************
#include "PREv20.h"
#include "PREv20_RF.h"
#include "mbus.h"
#include "SNTv2_RF.h"
#include "HRVv5.h"
#include "PMUv7_RF.h"
#include "SRRv4_RF.h"
#include "RDCv4_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG_1

// Stack order PREv20E -> SNTv2 -> MRRv10A -> External (RDC -> SRR -> MEM) -> PMUv11
#define HRV_ADDR 0x3
#define SRR_ADDR 0x4
#define SNT_ADDR 0x5
#define PMU_ADDR 0x6
#define MEM_ADDR 0x7
#define RDC_ADDR 0x8
#define MRR_ADDR 0x9

// Temp Sensor parameters
#define    MBUS_DELAY 200 // Amount of delay between successive messages; 200: 6-7ms
#define TEMP_CYCLE_INIT 1 

// Tstack states
#define    STK_IDLE       0x0
#define    STK_LDO        0x1
#define STK_TEMP_START 0x2
#define STK_RDC 0x3
#define STK_RDC2 0x7
#define STK_RDC3 0x8
#define STK_RDC4 0x9
#define STK_DATA 0x4
#define STK_TEMP_READ  0x6

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

// Radio configurations
#define RADIO_DATA_LENGTH 192 // 96 bit header, 96 bit data
#define WAKEUP_PERIOD_RADIO_INIT 0xA // About 1.5 sec (PRCv18)
#define WAKEUP_PERIOD_SNT 0x20 // About 3 sec (PRCv18)

#define TEMP_STORAGE_SIZE 8192 // MEMv1: 16kB, 8k 2-byte data

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv18)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv18)


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
volatile uint32_t exec_count_irq;
volatile uint32_t wakeup_period_count;
volatile uint32_t wakeup_timer_multiplier;
volatile uint32_t PMU_ADC_3P0_VAL;
volatile uint32_t pmu_setting_state;

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

volatile uint32_t NUM_MEAS_USER;


volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 

//volatile uint32_t temp_meas_data[NUM_TEMP_MEAS] = {0};
//volatile uint32_t temp_storage[TEMP_STORAGE_SIZE] = {0};
volatile uint32_t temp_storage_latest = 150; // SNSv10
volatile uint32_t temp_storage_last_wakeup_adjust = 150; // SNSv10
volatile uint32_t temp_storage_diff = 0;
volatile uint32_t temp_storage_count;
volatile uint32_t temp_storage_debug;
volatile uint32_t sns_running;
volatile uint32_t read_data_temp; // [23:0] Temp Sensor D Out
volatile uint32_t TEMP_CALIB_A;
volatile uint32_t TEMP_CALIB_B;

volatile uint32_t snt_wup_counter_cur;
volatile uint32_t snt_timer_enabled = 0;
volatile uint32_t SNT_0P5S_VAL;

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t RADIO_PACKET_DELAY;
volatile uint32_t radio_packet_count;

volatile uint32_t read_data_batadc_diff;
volatile uint32_t read_data_batadc;

volatile uint32_t read_data_rdc;
volatile uint32_t read_data_rdc2;
volatile uint32_t read_data_rdc3;
volatile uint32_t read_data_rdc4;

volatile rdcv4_r20_t rdcv4_r20 = RDCv4_R20_DEFAULT;
volatile rdcv4_r21_t rdcv4_r21 = RDCv4_R21_DEFAULT;
volatile rdcv4_r22_t rdcv4_r22 = RDCv4_R22_DEFAULT;
volatile rdcv4_r24_t rdcv4_r24 = RDCv4_R24_DEFAULT;
volatile rdcv4_r25_t rdcv4_r25 = RDCv4_R25_DEFAULT;
volatile rdcv4_r28_t rdcv4_r28 = RDCv4_R28_DEFAULT;
volatile rdcv4_r2B_t rdcv4_r2B = RDCv4_R2B_DEFAULT;
volatile rdcv4_r2C_t rdcv4_r2C = RDCv4_R2C_DEFAULT;

volatile sntv2_r00_t sntv2_r00 = SNTv2_R00_DEFAULT;
volatile sntv2_r01_t sntv2_r01 = SNTv2_R01_DEFAULT;
volatile sntv2_r03_t sntv2_r03 = SNTv2_R03_DEFAULT;
volatile sntv2_r08_t sntv2_r08 = SNTv2_R08_DEFAULT;
volatile sntv2_r09_t sntv2_r09 = SNTv2_R09_DEFAULT;
volatile sntv2_r0A_t sntv2_r0A = SNTv2_R0A_DEFAULT;
//volatile sntv2_r0B_t sntv2_r0B = SNTv2_R0B_DEFAULT;
volatile sntv2_r17_t sntv2_r17 = SNTv2_R17_DEFAULT;

volatile srrv4_r00_t srrv4_r00 = SRRv4_R00_DEFAULT;
volatile srrv4_r01_t srrv4_r01 = SRRv4_R01_DEFAULT;
volatile srrv4_r02_t srrv4_r02 = SRRv4_R02_DEFAULT;
volatile srrv4_r03_t srrv4_r03 = SRRv4_R03_DEFAULT;
volatile srrv4_r04_t srrv4_r04 = SRRv4_R04_DEFAULT;
volatile srrv4_r07_t srrv4_r07 = SRRv4_R07_DEFAULT;
volatile srrv4_r11_t srrv4_r11 = SRRv4_R11_DEFAULT;
volatile srrv4_r12_t srrv4_r12 = SRRv4_R12_DEFAULT;
volatile srrv4_r13_t srrv4_r13 = SRRv4_R13_DEFAULT;
volatile srrv4_r14_t srrv4_r14 = SRRv4_R14_DEFAULT;
volatile srrv4_r15_t srrv4_r15 = SRRv4_R15_DEFAULT;
volatile srrv4_r1E_t srrv4_r1E = SRRv4_R1E_DEFAULT;
volatile srrv4_r1F_t srrv4_r1F = SRRv4_R1F_DEFAULT;

volatile prev20_r0B_t prev20_r0B = PREv20_R0B_DEFAULT;
volatile prev20_r1C_t prev20_r1C = PREv20_R1C_DEFAULT;

//*******************************************************************
// INTERRUPT HANDLERS (Updated for PRCv18)
//*******************************************************************

void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));

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
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
    // Report who woke up
    delay(MBUS_DELAY);
    mbus_write_message32(0xAA,*SREG_WAKEUP_SOURCE); // 0x1: GOC; 0x2: PRC Timer; 0x10: SNT
}

//***************************************************
// CRC16 Encoding
//***************************************************
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

//***************************************************
// Sleep Functions
//***************************************************
static void operation_sns_sleep_check(void);
static void operation_sleep(void);
static void operation_sleep_noirqreset(void);
static void operation_sleep_notimer(void);
static void operation_sleep_snt_timer(void);

//************************************
// PMU Related Functions
//************************************

static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
    set_halt_until_mbus_trx();
    mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
    set_halt_until_mbus_tx();
}

static void pmu_set_sar_override(uint32_t val){
	// SAR_RATIO_OVERRIDE
    pmu_reg_write(0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (val) 		// Binary converter's conversion ratio (7'h00)
	));
    pmu_reg_write(0x05, //default 12'h000
		( (1 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (val) 		// Binary converter's conversion ratio (7'h00)
	));
}

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

/*
static void pmu_set_sar_override(uint32_t val){
	// SAR_RATIO_OVERRIDE
    pmu_reg_write(0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (val) 		// Binary converter's conversion ratio (7'h00)
	));
    pmu_reg_write(0x05, //default 12'h000
		( (1 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (val) 		// Binary converter's conversion ratio (7'h00)
	));
}

*/

inline static void pmu_set_adc_period(uint32_t val){
	// PMU_CONTROLLER_DESIRED_STATE Active
	// Updated for PMUv9
	pmu_reg_write(0x3C,
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
		| (0 << 19) //state_vbat_read
		| (1 << 20) //state_state_horizon
	));

	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    pmu_reg_write(0x36,val); 
	delay(MBUS_DELAY*10);

	// Register 0x33: TICK_ADC_RESET
	pmu_reg_write(0x33,2);

	// Register 0x34: TICK_ADC_CLK
	pmu_reg_write(0x34,2);

	// PMU_CONTROLLER_DESIRED_STATE Active
	pmu_reg_write(0x3C,
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
		| (0 << 19) //state_vbat_read
		| (1 << 20) //state_state_horizon
	));
}

inline static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

    // Register 0x16: V1P2 Active
    pmu_reg_write(0x16, 
        ( (0 << 19) // Enable PFM even during periodic reset
        | (0 << 18) // Enable PFM even when Vref is not used as ref
        | (0 << 17) // Enable PFM
        | (3 << 14) // Comparator clock division ratio
        | (0 << 13) // Enable main feedback loop
        | (r << 9)  // Frequency multiplier R
        | (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
        | (base)        // Floor frequency base (0-63)
    ));

    // Register 0x18: V3P6 Active 
    pmu_reg_write(0x18, 
        ( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
        | (0 << 13) // Enable main feedback loop
        | (r << 9)  // Frequency multiplier R
        | (l << 5)  // Frequency multiplier L (actually L+1)
        | (base)        // Floor frequency base (0-63)
    ));
    
    // Register 0x1A: V0P6 Active
    pmu_reg_write(0x1A,
        ( (0 << 13) // Enable main feedback loop
        | (r << 9)  // Frequency multiplier R
        | (l << 5)  // Frequency multiplier L (actually L+1)
        | (base)        // Floor frequency base (0-63)
    ));

}

inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

    // Register 0x17: V3P6 Sleep
    pmu_reg_write(0x17, 
        ( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
        | (0 << 13) // Enable main feedback loop
        | (r << 9)  // Frequency multiplier R
        | (l << 5)  // Frequency multiplier L (actually L+1)
        | (base)        // Floor frequency base (0-63)
    ));
    
    // Register 0x15: V1P2 Sleep
    pmu_reg_write(0x15, 
        ( (0 << 19) // Enable PFM even during periodic reset
        | (0 << 18) // Enable PFM even when Vref is not used as ref
        | (0 << 17) // Enable PFM
        | (3 << 14) // Comparator clock division ratio
        | (0 << 13) // Enable main feedback loop
        | (r << 9)  // Frequency multiplier R
        | (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
        | (base)        // Floor frequency base (0-63)
    ));
    // Register 0x19: V0P6 Sleep
    pmu_reg_write(0x19,
        ( (0 << 13) // Enable main feedback loop
        | (r << 9)  // Frequency multiplier R
        | (l << 5)  // Frequency multiplier L (actually L+1)
        | (base)        // Floor frequency base (0-63)
    ));

}


inline static void pmu_active_setting_temp_based(){
    
    if (pmu_setting_state >= PMU_95C){
        pmu_set_active_clk(0x7,0x2,0x7,0x4/*V1P2*/);

    }else if (pmu_setting_state >= PMU_75C){
        pmu_set_active_clk(0xA,0x4,0x7,0x8/*V1P2*/);

    }else if (pmu_setting_state >= PMU_55C){
        pmu_set_active_clk(0x1,0x0,0x10,0x2/*V1P2*/);

    }else if (pmu_setting_state >= PMU_35C){
        pmu_set_active_clk(0x2,0x1,0x10,0x2/*V1P2*/);

    }else if (pmu_setting_state < PMU_15C){
        pmu_set_active_clk(0xD,0x2,0x10,0x4/*V1P2*/);

    }else if (pmu_setting_state < PMU_25C){
        pmu_set_active_clk(0x7,0x2,0x10,0x4/*V1P2*/);

    }else{ // 25C, default
        pmu_set_active_clk(0x5,0x1,0x10,0x2/*V1P2*/);
    }
}

inline static void pmu_set_sleep_radio(){
    pmu_set_sleep_clk(0xF,0xA,0x5,0xF/*V1P2*/);
}

inline static void pmu_set_sleep_low(){
    pmu_set_sleep_clk(0x5,0x2,0x1,0x2/*V1P2*/);
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
        pmu_set_sleep_clk(0x1,0x1,0x1,0x0/*V1P2*/);

    }else if (pmu_setting_state >= PMU_75C){
        pmu_set_sleep_clk(0x1,0x2,0x1,0x1/*V1P2*/);

    }else if (pmu_setting_state >= PMU_55C){
        pmu_set_sleep_clk(0x1,0x2,0x1,0x2/*V1P2*/);

    }else if (pmu_setting_state >= PMU_35C){
        pmu_set_sleep_clk(0x2,0x2,0x1,0x2/*V1P2*/);

    }else if (pmu_setting_state < PMU_10C){
        pmu_set_sleep_clk(0xF,0x4,0x1,0x4/*V1P2*/);

    }else if (pmu_setting_state < PMU_20C){
        pmu_set_sleep_clk(0xF,0x2,0x1,0x2/*V1P2*/);

    }else{ // 25C, default
		pmu_set_sleep_low();
    }
}



inline static void pmu_set_clk_init(){
    pmu_setting_state = PMU_25C;
    pmu_active_setting_temp_based();
    pmu_sleep_setting_temp_based();

	// SAR_RATIO_OVERRIDE
	// Use the new reset scheme in PMUv3
    pmu_reg_write(0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (0 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (0 << 7) // Enable override setting [6:0] (1'h0)
		| (0x4B) 		// Binary converter's conversion ratio (7'h00)
	));
	pmu_set_sar_override(0x4B);
    pmu_set_adc_period(1); // 0x100 about 1 min for 1/2/1 1P2 setting
}


inline static void pmu_adc_reset_setting(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Active
    // Updated for PMUv9
    pmu_reg_write(0x3C,
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
        | (0 << 19) //state_vbat_read
        | (1 << 20) //state_state_horizon
    ));
}

inline static void pmu_adc_disable(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Sleep
    // Updated for PMUv9
    pmu_reg_write(0x3B,
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
        | (1 << 13) //state_sar_scn_ratio_adjusted // Turn on for old adc, off for new adc
        | (1 << 14) //state_downconverter_on
        | (1 << 15) //state_downconverter_stabilized
        | (1 << 16) //state_vdd_3p6_turned_on
        | (1 << 17) //state_vdd_1p2_turned_on
        | (1 << 18) //state_vdd_0P6_turned_on
        | (0 << 19) //state_vbat_read // Turn off for old adc
        | (1 << 20) //state_state_horizon
    ));
}

inline static void pmu_adc_enable(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Sleep
    // Updated for PMUv9
    pmu_reg_write(0x3B,
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
        | (1 << 13) //state_sar_scn_ratio_adjusted // Turn on for old adc, off for new adc
        | (1 << 14) //state_downconverter_on
        | (1 << 15) //state_downconverter_stabilized
        | (1 << 16) //state_vdd_3p6_turned_on
        | (1 << 17) //state_vdd_1p2_turned_on
        | (1 << 18) //state_vdd_0P6_turned_on
        | (0 << 19) //state_vbat_read // Turn off for old adc
        | (1 << 20) //state_state_horizon
    ));
}


inline static void pmu_reset_solar_short(){
    // Updated for PMUv9
    pmu_reg_write(0x0E, 
        ( (0 << 12) // 1: solar short by latched vbat_high (new); 0: follow [10] setting
        | (1 << 11) // Reset of vbat_high latch for [12]=1
        | (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
        | (1 << 9)  // Enables override setting [8]
        | (0 << 8)  // Turn on the harvester-inhibiting switch
        | (3 << 4)  // clamp_tune_bottom (increases clamp thresh)
        | (0)        // clamp_tune_top (decreases clamp thresh)
    ));
    pmu_reg_write(0x0E, 
        ( (0 << 12) // 1: solar short by latched vbat_high (new); 0: follow [10] setting
        | (1 << 11) // Reset of vbat_high latch for [12]=1
        | (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
        | (0 << 9)  // Enables override setting [8]
        | (0 << 8)  // Turn on the harvester-inhibiting switch
        | (3 << 4)  // clamp_tune_bottom (increases clamp thresh)
        | (0)        // clamp_tune_top (decreases clamp thresh)
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
    read_data_batadc = *((volatile uint32_t *) REG0) & 0xFF;
    
    if (read_data_batadc<PMU_ADC_3P0_VAL){
        read_data_batadc_diff = 0;
    }else{
        read_data_batadc_diff = read_data_batadc - PMU_ADC_3P0_VAL;
    }

}

//***************************************************
// Temp Sensor Functions (SNTv4 retrofit to SNTv2)
//***************************************************

static void temp_sensor_start(){
    sntv2_r01.TSNS_RESETn = 1;
    mbus_remote_register_write(SNT_ADDR,1,sntv2_r01.as_int);
}
static void temp_sensor_reset(){
    sntv2_r01.TSNS_RESETn = 0;
    mbus_remote_register_write(SNT_ADDR,1,sntv2_r01.as_int);
}
static void temp_sensor_power_on(){
    // Turn on digital block
    sntv2_r01.TSNS_SEL_LDO = 1;
    mbus_remote_register_write(SNT_ADDR,1,sntv2_r01.as_int);
    // Turn on analog block
    sntv2_r01.TSNS_EN_SENSOR_LDO = 1;
    mbus_remote_register_write(SNT_ADDR,1,sntv2_r01.as_int);

    delay(MBUS_DELAY);

    // Release isolation
    sntv2_r01.TSNS_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR,1,sntv2_r01.as_int);
}
static void temp_sensor_power_off(){
    sntv2_r01.TSNS_RESETn = 0;
    sntv2_r01.TSNS_SEL_LDO = 0;
    sntv2_r01.TSNS_EN_SENSOR_LDO = 0;
    sntv2_r01.TSNS_ISOLATE = 1;
    mbus_remote_register_write(SNT_ADDR,1,sntv2_r01.as_int);
}
static void sns_ldo_vref_on(){
    sntv2_r00.LDO_EN_VREF    = 1;
    mbus_remote_register_write(SNT_ADDR,0,sntv2_r00.as_int);
}

static void sns_ldo_power_on(){
    sntv2_r00.LDO_EN_IREF    = 1;
    sntv2_r00.LDO_EN_LDO    = 1;
    mbus_remote_register_write(SNT_ADDR,0,sntv2_r00.as_int);
}
static void sns_ldo_power_off(){
    sntv2_r00.LDO_EN_VREF    = 0;
    sntv2_r00.LDO_EN_IREF    = 0;
    sntv2_r00.LDO_EN_LDO    = 0;
    mbus_remote_register_write(SNT_ADDR,0,sntv2_r00.as_int);
}

static void snt_read_wup_counter(){
// WARNING: The read value might be corrupted due to asynchronous reading

    set_halt_until_mbus_rx();
    mbus_remote_register_read(SNT_ADDR,0x1B,1); // CNT_VALUE_EXT
    snt_wup_counter_cur = (*REG1 & 0xFF)<<24;
    mbus_remote_register_read(SNT_ADDR,0x1C,1); // CNT_VALUE
    snt_wup_counter_cur = snt_wup_counter_cur | (*REG1 & 0xFFFFFF);
    set_halt_until_mbus_tx();

}
    
static void snt_start_timer_presleep(){

    sntv2_r09.TMR_IBIAS_REF = 0x4; // Default : 4'h4
    mbus_remote_register_write(SNT_ADDR,0x09,sntv2_r09.as_int);

	// Release Power Gate
	sntv2_r08.TMR_SLEEP = 0x0; // Default : 0x1
	sntv2_r08.TMR_ISOLATE = 0x0; // Default : 0x1
	mbus_remote_register_write(SNT_ADDR,0x08,sntv2_r08.as_int);

    // TIMER SELF_EN Disable 
    sntv2_r09.TMR_SELF_EN = 0x0; // Default : 0x1
    mbus_remote_register_write(SNT_ADDR,0x09,sntv2_r09.as_int);

    // EN_OSC 
    sntv2_r08.TMR_EN_OSC = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv2_r08.as_int);

    // Release Reset 
    sntv2_r08.TMR_RESETB = 0x1; // Default : 0x0
    sntv2_r08.TMR_RESETB_DIV = 0x1; // Default : 0x0
    sntv2_r08.TMR_RESETB_DCDC = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv2_r08.as_int);

    // TIMER EN_SEL_CLK Reset 
    sntv2_r08.TMR_EN_SELF_CLK = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv2_r08.as_int);

    // TIMER SELF_EN 
    sntv2_r09.TMR_SELF_EN = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x09,sntv2_r09.as_int);
    //delay(100000); 

    snt_timer_enabled = 1;
}

static void snt_start_timer_postsleep(){
    // Turn off sloscillator
    sntv2_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv2_r08.as_int);
}


static void snt_stop_timer(){

    // EN_OSC
    sntv2_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    // RESET
    sntv2_r08.TMR_EN_SELF_CLK = 0x0; // Default : 0x0
    sntv2_r08.TMR_RESETB = 0x0;// Default : 0x0
    sntv2_r08.TMR_RESETB_DIV = 0x0; // Default : 0x0
    sntv2_r08.TMR_RESETB_DCDC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv2_r08.as_int);
    snt_timer_enabled = 0;

	// Enable Power Gate
	sntv2_r08.TMR_SLEEP = 0x0; // Default : 0x1
	sntv2_r08.TMR_ISOLATE = 0x0; // Default : 0x1
	mbus_remote_register_write(SNT_ADDR,0x08,sntv2_r08.as_int);

    sntv2_r09.TMR_IBIAS_REF = 0x0; // Default : 4'h4
    mbus_remote_register_write(SNT_ADDR,0x09,sntv2_r09.as_int);

    sntv2_r17.WUP_ENABLE = 0x0; // Default : 0x
    mbus_remote_register_write(SNT_ADDR,0x17,sntv2_r17.as_int);

}

static void snt_set_wup_timer(uint32_t sleep_count){
    
    snt_wup_counter_cur = sleep_count + snt_wup_counter_cur; // should handle rollover

    mbus_remote_register_write(SNT_ADDR,0x19,snt_wup_counter_cur>>24);
    mbus_remote_register_write(SNT_ADDR,0x1A,snt_wup_counter_cur & 0xFFFFFF);
    
    sntv2_r17.WUP_ENABLE = 0x1; // Default : 0x
    mbus_remote_register_write(SNT_ADDR,0x17,sntv2_r17.as_int);

}

//***************************************************
// RDCv3 Functions
//***************************************************

static void rdc_enable_vref(){
	rdcv4_r2C.RDC_ENb_PG_VREF = 1;
	mbus_remote_register_write(RDC_ADDR,0x2C,rdcv4_r2C.as_int);
}
static void rdc_disable_vref(){
	rdcv4_r2C.RDC_ENb_PG_VREF = 0;
	mbus_remote_register_write(RDC_ADDR,0x2C,rdcv4_r2C.as_int);
}

static void rdc_disable_pg_v1p2(){
	rdcv4_r2C.RDC_EN_PG_FSM = 0;
	rdcv4_r2C.RDC_EN_PG_RC_OSC = 0;
	rdcv4_r2C.RDC_EN_PG_AMP_V1P2 = 0;
	rdcv4_r2C.RDC_EN_PG_ADC_V1P2 = 0;
	rdcv4_r2C.RDC_EN_PG_BUF_VH_V1P2 = 0;
	mbus_remote_register_write(RDC_ADDR,0x2C,rdcv4_r2C.as_int);
}
static void rdc_enable_pg_v1p2(){
	rdcv4_r2C.RDC_EN_PG_FSM = 1;
	rdcv4_r2C.RDC_EN_PG_RC_OSC = 1;
	rdcv4_r2C.RDC_EN_PG_AMP_V1P2 = 1;
	rdcv4_r2C.RDC_EN_PG_ADC_V1P2 = 1;
	rdcv4_r2C.RDC_EN_PG_BUF_VH_V1P2 = 1;
	mbus_remote_register_write(RDC_ADDR,0x2C,rdcv4_r2C.as_int);
}

static void rdc_disable_pg_vbat(){
	rdcv4_r2C.RDC_ENb_PG_AMP_VBAT = 1;
	rdcv4_r2C.RDC_ENb_PG_ADC_VBAT = 1;
	rdcv4_r2C.RDC_ENb_PG_BUF_VCM = 1;
	rdcv4_r2C.RDC_ENb_PG_BUF_VH_VBAT = 1;
	rdcv4_r2C.RDC_ENb_MIRROR_LDO = 0;
	mbus_remote_register_write(RDC_ADDR,0x2C,rdcv4_r2C.as_int);
}
static void rdc_enable_pg_vbat(){
	rdcv4_r2C.RDC_ENb_PG_AMP_VBAT = 0;
	rdcv4_r2C.RDC_ENb_PG_ADC_VBAT = 0;
	rdcv4_r2C.RDC_ENb_PG_BUF_VCM = 0;
	rdcv4_r2C.RDC_ENb_PG_BUF_VH_VBAT = 0;
	rdcv4_r2C.RDC_ENb_MIRROR_LDO = 1;
	mbus_remote_register_write(RDC_ADDR,0x2C,rdcv4_r2C.as_int);
}

static void rdc_disable_clock(){
	rdcv4_r28.RDC_RESET_RC_OSC = 1;
	rdcv4_r2B.RDC_CLK_ISOLATE = 1;
	mbus_remote_register_write(RDC_ADDR,0x28,rdcv4_r28.as_int);
	mbus_remote_register_write(RDC_ADDR,0x2B,rdcv4_r2B.as_int);
}
static void rdc_enable_clock(){
	rdcv4_r28.RDC_RESET_RC_OSC = 0;
	rdcv4_r2B.RDC_CLK_ISOLATE = 0;
	mbus_remote_register_write(RDC_ADDR,0x28,rdcv4_r28.as_int);
	mbus_remote_register_write(RDC_ADDR,0x2B,rdcv4_r2B.as_int);
}

static void rdc_start_meas(){
	rdcv4_r20.RDC_RESETn_FSM = 1;
	rdcv4_r20.RDC_ISOLATE = 0;
	mbus_remote_register_write(RDC_ADDR,0x20,rdcv4_r20.as_int);
}
static void rdc_reset(){
	rdcv4_r20.RDC_RESETn_FSM = 0;
	rdcv4_r20.RDC_ISOLATE = 1;
	mbus_remote_register_write(RDC_ADDR,0x20,rdcv4_r20.as_int);
}
static void rdc_reset2(){
	rdcv4_r20.RDC_ISOLATE = 1;
	mbus_remote_register_write(RDC_ADDR,0x20,rdcv4_r20.as_int);
}

static void rdc_power_on(){

	rdc_reset();
	rdc_disable_pg_v1p2();
	delay(MBUS_DELAY);

	rdc_disable_pg_vbat();
	delay(MBUS_DELAY);


	rdc_enable_clock();

}

static void rdc_power_off(){

	rdc_reset2();
	rdc_disable_clock();
	rdc_enable_pg_vbat();
	rdc_enable_pg_v1p2();
	rdc_disable_vref();  

}

//***************************************************
// Radio transmission routines for SRR
//***************************************************

static void radio_power_on(){

    // Switch PMU to higher setting
    //pmu_set_sleep_radio();
    
    radio_on = 1;

    // Turn on Current Limter
    srrv4_r00.SRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(SRR_ADDR,0x00,srrv4_r00.as_int);

    // Release timer power-gate
    srrv4_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    mbus_remote_register_write(SRR_ADDR,0x04,srrv4_r04.as_int);
    delay(MBUS_DELAY);

    // Turn on timer
    srrv4_r04.RO_RESET = 0;  //Release Reset TIMER
    mbus_remote_register_write(SRR_ADDR,0x04,srrv4_r04.as_int);
    delay(MBUS_DELAY);

    srrv4_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    mbus_remote_register_write(SRR_ADDR,0x04,srrv4_r04.as_int);
    delay(MBUS_DELAY);

    srrv4_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(SRR_ADDR,0x04,srrv4_r04.as_int);

    // Release FSM Sleep
    srrv4_r11.SRR_RAD_FSM_SLEEP = 0;  // Power on BB
    mbus_remote_register_write(SRR_ADDR,0x11,srrv4_r11.as_int);
    delay(MBUS_DELAY*5); // Freq stab

}

static void radio_power_off(){

    // Switch PMU to lower setting
    //pmu_set_sleep_low();

    // Turn off everything
    srrv4_r03.SRR_TRX_ISOLATEN = 0;     //set ISOLATEN 0
    mbus_remote_register_write(SRR_ADDR,0x03,srrv4_r03.as_int);

    srrv4_r11.SRR_RAD_FSM_EN = 0;  //Stop BB
    srrv4_r11.SRR_RAD_FSM_RSTN = 0;  //RST BB
    srrv4_r11.SRR_RAD_FSM_SLEEP = 1;
    mbus_remote_register_write(SRR_ADDR,0x11,srrv4_r11.as_int);

    srrv4_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
    // Turn off Current Limter
    srrv4_r00.SRR_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(SRR_ADDR,0x00,srrv4_r00.as_int);

    srrv4_r04.RO_RESET = 1;  //Release Reset TIMER
    srrv4_r04.RO_EN_CLK = 0; //Enable CLK TIMER
    mbus_remote_register_write(SRR_ADDR,0x04,srrv4_r04.as_int);

    // Enable timer power-gate
    srrv4_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
    mbus_remote_register_write(SRR_ADDR,0x04,srrv4_r04.as_int);

    radio_on = 0;
    radio_ready = 0;
}

static void srr_configure_pulse_width_short(){

    srrv4_r12.SRR_RAD_FSM_TX_PW_LEN = 0; //4us PW
    srrv4_r13.SRR_RAD_FSM_TX_C_LEN = 32; // (PW_LEN+1):C_LEN=1:32
    srrv4_r12.SRR_RAD_FSM_TX_PS_LEN = 1; // PW=PS guard interval betwen 0 and 1 pulse

    mbus_remote_register_write(SRR_ADDR,0x12,srrv4_r12.as_int);
    mbus_remote_register_write(SRR_ADDR,0x13,srrv4_r13.as_int);
}

static void send_radio_data_srr_sub1(){

    // Use Timer32 as timeout counter
    wfi_timeout_flag = 0;
    config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

    // Fire off data
    srrv4_r11.SRR_RAD_FSM_EN = 1;  //Start BB
    mbus_remote_register_write(SRR_ADDR,0x11,srrv4_r11.as_int);

    // Wait for radio response
    WFI();

    // Turn off Timer32
    *TIMER32_GO = 0;

    if (wfi_timeout_flag){
        mbus_write_message32(0xFA, 0xFAFAFAFA);
    }

    srrv4_r11.SRR_RAD_FSM_EN = 0;
    mbus_remote_register_write(SRR_ADDR,0x11,srrv4_r11.as_int);
}

static void send_radio_data_srr(uint32_t last_packet, uint8_t radio_packet_prefix, uint16_t radio_data_2, uint32_t radio_data_1, uint32_t radio_data_0, uint8_t num_tx_duplicate){
    // Sends 168 bit packet, of which 96b is actual data
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
    
    output_data = crcEnc16(((radio_packet_count & 0xFF)<<8) | radio_packet_prefix, (radio_data_2 <<16) | ((radio_data_1 & 0xFFFFFF) >>8), (radio_data_1 << 24) | (radio_data_0 & 0xFFFFFF));

    mbus_remote_register_write(SRR_ADDR,0xD,radio_data_0);
    mbus_remote_register_write(SRR_ADDR,0xE,radio_data_1);
    mbus_remote_register_write(SRR_ADDR,0xF,(radio_packet_prefix<<16)|radio_data_2);
    mbus_remote_register_write(SRR_ADDR,0x10,(radio_packet_count&0xFF)|((output_data[2] & 0xFFFF)/*CRC16*/<<8));

    if (!radio_ready){
        radio_ready = 1;

        // Release FSM Reset
        srrv4_r11.SRR_RAD_FSM_RSTN = 1;  //UNRST BB
        mbus_remote_register_write(SRR_ADDR,0x11,srrv4_r11.as_int);
        delay(MBUS_DELAY);

    srrv4_r03.SRR_TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    mbus_remote_register_write(SRR_ADDR,0x03,srrv4_r03.as_int);
        delay(MBUS_DELAY);

    }
    uint8_t ii = 0;

    while (ii<=num_tx_duplicate){
        send_radio_data_srr_sub1();
        delay(RADIO_PACKET_DELAY);
        ii++;
    }

    radio_packet_count++;

    if (last_packet){
        radio_power_off();
    }
}

//***************************************************
// End of Program Sleep Operation
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
    if (radio_on){
        radio_power_off();
    }

    // Make sure SNT timer is off
    if (snt_timer_enabled){
        snt_stop_timer();
    }

    // Disable Timer
    set_wakeup_timer(0, 0, 0);

    // Go to sleep
    operation_sleep();

}


static void operation_tx_stored(uint8_t num_duplicate_packets){

    //Fire off stored data to radio
    while(radio_tx_count < temp_storage_count){
        #ifdef DEBUG_MBUS_MSG_1
            mbus_write_message32(0xDD, radio_tx_count);
            mbus_write_message32(0xDD, temp_storage[radio_tx_count]);
        #endif

        // Reset watchdog timer
        config_timerwd(TIMERWD_VAL);

        // Radio out data
        uint32_t mem_read_data[2];
        set_halt_until_mbus_rx();
        // Read 2 words (4 data points) at a time
        mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*)((radio_tx_count>>1)<<2), 0x01, (uint32_t*)&mem_read_data, 1);
        set_halt_until_mbus_tx();
    
        send_radio_data_srr(0,0xDD,mem_read_data[1]>>16,((mem_read_data[1]&0xFFFF)<<8) | (mem_read_data[0]>>24),mem_read_data[0]&0xFFFFFF,num_duplicate_packets);
        radio_tx_count = radio_tx_count + 4;
    }

    send_radio_data_srr(1,0xD3,*REG_CHIP_ID,(0xBB00|read_data_batadc_diff)<<8,temp_storage_count,1);    
    // This is also the end of this IRQ routine
    exec_count_irq = 0;

    // Go to sleep without timer
    operation_sleep_notimer();
}
/*
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

static void measure_wakeup_period(void){

    // Reset Wakeup Timer
    *WUPT_RESET = 1;

    mbus_write_message32(0xE0, 0x0);
    // Prevent watchdogs from kicking in
    config_timerwd(TIMERWD_VAL*2);
    *REG_MBUS_WD = 1500000*3; // default: 1500000

    uint32_t wakeup_timer_val_0 = *REG_WUPT_VAL;
    wakeup_period_count = 0;

    while( *REG_WUPT_VAL <= wakeup_timer_val_0 + 1){
        wakeup_period_count = 0;
    }
    while( *REG_WUPT_VAL <= wakeup_timer_val_0 + 2){
        wakeup_period_count++;
    }
    mbus_write_message32(0xE1, wakeup_timer_val_0);
    mbus_write_message32(0xE2, wakeup_period_count);

    config_timerwd(TIMERWD_VAL);
    WAKEUP_PERIOD_CONT = dumb_divide(WAKEUP_PERIOD_CONT_USER*1000*8, wakeup_period_count);
    if (WAKEUP_PERIOD_CONT > 0x7FFF){
        WAKEUP_PERIOD_CONT = 0x7FFF;
    }
    mbus_write_message32(0xED, WAKEUP_PERIOD_CONT); 
}
*/

static void operation_init(void){
  
    // Set CPU & Mbus Clock Speeds
    prev20_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prev20_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prev20_r0B.CLK_GEN_DIV_CORE = 0x2; // Default 0x3
    prev20_r0B.GOC_CLK_GEN_SEL_FREQ = 0x5; // Default 0x6
    prev20_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
    *REG_CLKGEN_TUNE = prev20_r0B.as_int;

    prev20_r1C.SRAM0_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
    prev20_r1C.SRAM0_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
    prev20_r1C.SRAM0_USE_INVERTER_SA = 0; // Default 0x2, 4 bits
    *REG_SRAM0_TUNE = prev20_r1C.as_int;
    //*REG_SRAM_TUNE = 0x45; // use inverter SA
    //*REG_SRAM_TUNE = 0x44; // use default SA
  
    //Enumerate & Initialize Registers
    stack_state = STK_IDLE;    //0x0;
    enumerated = 0x5043607A;
    exec_count = 0;
    exec_count_irq = 0;
    PMU_ADC_3P0_VAL = 0x62;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(SNT_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(MRR_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(RDC_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(SRR_ADDR);
	delay(MBUS_DELAY);
 	mbus_enumerate(MEM_ADDR);
	delay(MBUS_DELAY);
 	mbus_enumerate(PMU_ADDR);
	delay(MBUS_DELAY);
/*
    set_halt_until_mbus_trx();
    set_halt_until_mbus_tx();
*/
    // Set CPU Halt Option as TX --> Use for register write e.g.
    //set_halt_until_mbus_tx();

    // PMU Settings ----------------------------------------------
    pmu_set_clk_init();
    pmu_reset_solar_short();

    // New for PMUv9
    // VBAT_READ_TRIM Register
    pmu_reg_write(0x45,
        ( (0x0 << 9) // 0x0: no mon; 0x1: sar conv mon; 0x2: up conv mon; 0x3: down conv mon
        | (0x0 << 8) // 1: vbat_read_mode enable; 0: vbat_read_mode disable
        | (0x48 << 0) //sampling multiplication factor N; vbat_read out = vbat/1p2*N
    ));
    
	// Disable PMU ADC measurement in active mode
	// PMU_CONTROLLER_STALL_ACTIVE
	// Updated for PMUv9
    pmu_reg_write(0x3A, 
		( (1 << 20) // ignore state_horizon; default 1
		| (0 << 19) // state_vbat_read
		| (1 << 13) // ignore adc_output_ready; default 0
		| (1 << 12) // ignore adc_output_ready; default 0
		| (1 << 11) // ignore adc_output_ready; default 0
	));

	pmu_adc_reset_setting();
	pmu_adc_enable();

    // Temp Sensor Settings --------------------------------------
    // sntv4_r01
    sntv2_r01.TSNS_RESETn = 0;
    sntv2_r01.TSNS_EN_IRQ = 1;
    sntv2_r01.TSNS_BURST_MODE = 0;
    sntv2_r01.TSNS_CONT_MODE = 0;
    mbus_remote_register_write(SNT_ADDR,1,sntv2_r01.as_int);

/*
    // Set temp sensor conversion time
    sntv4_r03.TSNS_SEL_STB_TIME = 0x1; 
    sntv4_r03.TSNS_SEL_CONV_TIME = 0x6; // Default: 0x6
    mbus_remote_register_write(SNT_ADDR,0x03,sntv4_r03.as_int);
    // SNT Wakeup Timer Settings --------------------------------------
    // Config Register A
    sntv4_r0A.TMR_S = 0x1; // Default: 0x4, use 1 for good TC
    // Tune R for TC
    sntv4_r0A.TMR_DIFF_CON = 0x3FFF; // Default: 0x3FFB
    mbus_remote_register_write(SNT_ADDR,0x0A,sntv4_r0A.as_int);

	// TIMER CAP_TUNE  
    sntv4_r0B.TMR_TFR_CON = 0xF; // Default: 0xF
    mbus_remote_register_write(SNT_ADDR,0x0B,sntv4_r0B.as_int);

    // TIMER CAP_TUNE  
    // Tune C for freq
	sntv4_r09.TMR_SEL_CLK_DIV = 0x0; // Default : 1'h1
    sntv4_r09.TMR_SEL_CAP = 0x80; // Default : 8'h8
    sntv4_r09.TMR_SEL_DCAP = 0x3F; // Default : 6'h4

    sntv4_r09.TMR_EN_TUNE1 = 0x1; // Default : 1'h1
    sntv4_r09.TMR_EN_TUNE2 = 0x1; // Default : 1'h1

*/
    // to reduce standby current
    sntv2_r09.TMR_IBIAS_REF = 0x0; // Default : 4'h4
    mbus_remote_register_write(SNT_ADDR,0x09,sntv2_r09.as_int);

    // Wakeup Counter
    sntv2_r17.WUP_CLK_SEL = 0x0; 
    sntv2_r17.WUP_AUTO_RESET = 0x0; // Automatically reset counter to 0 upon sleep 
    mbus_remote_register_write(SNT_ADDR,0x17,sntv2_r17.as_int);

    // RDCv3 Settings --------------------------------------
    // Common settings
	// rdcv4_r00
	//rdcv4_r00.WAKEUP_UPON_RDC_IRQ = 0x0;
	//rdcv4_r00.MBC_WAKEUP_ON_PEND_REQ = 0x1;

	// rdcv4_r20
	rdcv4_r20.RDC_CNT_AMP1 = 0xC;
	
	// rdcv4_r21
	rdcv4_r21.RDC_CNT_AMP2 = 0x8;
	
	// rdcv4_r22
	rdcv4_r22.RDC_CNT_SKIP = 0x3;
	rdcv4_r22.RDC_OSR = 0xB;
	
	// rdcv4_r24 & r25
	rdcv4_r24.RDC_SEL_DLY = 0xA;

    // Individual settings
	rdcv4_r24.RDC_SEL_GAIN_LC = 0x0;
	rdcv4_r25.RDC_OFFSET_P_LC = 0x0;
	rdcv4_r25.RDC_OFFSET_PB_LC = 0x1F;

	mbus_remote_register_write(RDC_ADDR,0x1,0x1001);
	mbus_remote_register_write(RDC_ADDR,0x20,rdcv4_r20.as_int);
	mbus_remote_register_write(RDC_ADDR,0x21,rdcv4_r21.as_int);
	mbus_remote_register_write(RDC_ADDR,0x22,rdcv4_r22.as_int);
	mbus_remote_register_write(RDC_ADDR,0x24,rdcv4_r24.as_int);
	mbus_remote_register_write(RDC_ADDR,0x25,rdcv4_r25.as_int);
	
    // Radio Settings (SRRv4) -------------------------------------------
    srrv4_r1F.LC_CLK_RING = 0x3;  // ~ 150 kHz
    srrv4_r1F.LC_CLK_DIV = 0x3;  // ~ 150 kHz
    mbus_remote_register_write(SRR_ADDR,0x1F,srrv4_r1F.as_int);

    srr_configure_pulse_width_short();

    // RO setup (SFO)
    // Adjust Diffusion R
    mbus_remote_register_write(SRR_ADDR,0x06,0x1000); // RO_PDIFF

    // Adjust Poly R
    mbus_remote_register_write(SRR_ADDR,0x08,0x400000); // RO_POLY

    // Adjust C
    srrv4_r07.RO_MOM = 0x10;
    srrv4_r07.RO_MIM = 0x10;
    mbus_remote_register_write(SRR_ADDR,0x07,srrv4_r07.as_int);

    // Current Limter set-up 
    srrv4_r00.SRR_CL_CTRL = 16; //Set CL 1-finite 16-20uA

    // TX Setup Carrier Freq
    srrv4_r00.SRR_TRX_CAP_ANTP_TUNE = 0x0000;  //ANT CAP 14b unary 830.5 MHz
    //srrv4_r00.SRR_TRX_CAP_ANTP_TUNE = 0x00FF;  //ANT CAP 14b unary 813.8 MHz
    //srrv4_r00.SRR_TRX_CAP_ANTP_TUNE = 0x0FFF;  //ANT CAP 14b unary 805.5 MHz
    mbus_remote_register_write(SRR_ADDR,0x00,srrv4_r00.as_int);
    srrv4_r01.SRR_TRX_CAP_ANTN_TUNE = 0x0000; //ANT CAP 14b unary 830.5 MHz
    //srrv4_r01.SRR_TRX_CAP_ANTN_TUNE = 0x00FF; //ANT CAP 14b unary 813.8 MHz
    //srrv4_r01.SRR_TRX_CAP_ANTN_TUNE = 0x0FFF;  //ANT CAP 14b unary 805.5 MHz
    mbus_remote_register_write(SRR_ADDR,0x01,srrv4_r01.as_int);
    srrv4_r02.SRR_TX_BIAS_TUNE = 0x0FFF;  //Set TX BIAS TUNE 13b // Set to max
    mbus_remote_register_write(SRR_ADDR,0x02,srrv4_r02.as_int);

    // RX Setup
    srrv4_r03.SRR_RX_BIAS_TUNE    = 0x000F;//  turn on Q_enhancement
    srrv4_r03.SRR_RX_SAMPLE_CAP    = 0x1;  // RX_SAMPLE_CAP
    mbus_remote_register_write(SRR_ADDR,3,srrv4_r03.as_int);

    srrv4_r14.SRR_RAD_FSM_RX_POWERON_LEN = 0x0;  //Set RX Power on length
    //srrv4_r14.SRR_RAD_FSM_RX_SAMPLE_LEN = 0x3;  //Set RX Sample length  16us
    srrv4_r14.SRR_RAD_FSM_RX_SAMPLE_LEN = 0x7;  //Set RX Sample length  4us
    srrv4_r14.SRR_RAD_FSM_GUARD_LEN = 0x001C; //Set TX_RX Guard length, TX_RX guard 32 cycle (28+5)
    mbus_remote_register_write(SRR_ADDR,0x14,srrv4_r14.as_int);

    srrv4_r15.SRR_RAD_FSM_RX_HDR_BITS = 0x00;  //Set RX header
    srrv4_r15.SRR_RAD_FSM_RX_HDR_TH = 0x00;    //Set RX header threshold
    srrv4_r15.SRR_RAD_FSM_RX_DATA_BITS = 0x1; //Set RX data 16b
    mbus_remote_register_write(SRR_ADDR,0x15,srrv4_r15.as_int);

    srrv4_r1E.SRR_IRQ_REPLY_PACKET = 0x061400; //Read RX data Reply
    mbus_remote_register_write(SRR_ADDR,0x1E,srrv4_r1E.as_int);

    mbus_remote_register_write(SRR_ADDR,0x15,srrv4_r15.as_int);

    // Use pulse generator
    srrv4_r02.SRR_TX_PULSE_FINE = 1;
    srrv4_r02.SRR_TX_PULSE_FINE_TUNE = 1;
    mbus_remote_register_write(SRR_ADDR,0x02,srrv4_r02.as_int);

    srrv4_r13.SRR_RAD_FSM_SEED = 1; //default
    srrv4_r14.SRR_RAD_FSM_TX_POWERON_LEN = 7; //3bits
    mbus_remote_register_write(SRR_ADDR,0x14,srrv4_r14.as_int);

    // RAD_FSM set-up 
    // Using first 48 bits of data as header
    mbus_remote_register_write(SRR_ADDR,0x09,0x0);
    mbus_remote_register_write(SRR_ADDR,0x0A,0x0);
    mbus_remote_register_write(SRR_ADDR,0x0B,0x0);
    mbus_remote_register_write(SRR_ADDR,0x0C,0x7AC800);
    srrv4_r11.SRR_RAD_FSM_TX_H_LEN = 0; //31-31b header (max)
    srrv4_r11.SRR_RAD_FSM_TX_D_LEN = RADIO_DATA_LENGTH; //0-skip tx data
    mbus_remote_register_write(SRR_ADDR,0x11,srrv4_r11.as_int);

    srrv4_r13.SRR_RAD_FSM_TX_MODE = 3; //code rate 0:4 1:3 2:2 3:1(baseline) 4:1/2 5:1/3 6:1/4
    mbus_remote_register_write(SRR_ADDR,0x13,srrv4_r13.as_int);

    // Mbus return address
    mbus_remote_register_write(SRR_ADDR,0x1E,0x1002);


    // Initialize other global variables
    WAKEUP_PERIOD_CONT_USER = 0x2710;
    temp_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0; //enables radio tx for each measurement 
    sns_running = 0;
    radio_ready = 0;
    radio_on = 0;
    wakeup_data = 0;
    RADIO_PACKET_DELAY = 1200;
    radio_packet_count = 0;
    
    pmu_setting_state = PMU_25C;

    PMU_10C_threshold_sns = 600; // Around 10C
    PMU_15C_threshold_sns = PMU_10C_threshold_sns + 200; 
    PMU_20C_threshold_sns = PMU_10C_threshold_sns + 400; 
    PMU_25C_threshold_sns = PMU_10C_threshold_sns + 700; 
    PMU_30C_threshold_sns = PMU_10C_threshold_sns + 1000; 
    PMU_35C_threshold_sns = PMU_10C_threshold_sns + 1300; 
    PMU_40C_threshold_sns = PMU_10C_threshold_sns + 1600; 
    PMU_45C_threshold_sns = PMU_10C_threshold_sns + 1900; 
    PMU_55C_threshold_sns = PMU_10C_threshold_sns + 2600; 
    PMU_65C_threshold_sns = PMU_10C_threshold_sns + 3900; 
    PMU_75C_threshold_sns = PMU_10C_threshold_sns + 6400; 
    PMU_85C_threshold_sns = PMU_10C_threshold_sns + 8800; 
    PMU_95C_threshold_sns = PMU_10C_threshold_sns + 11400; 

    SNT_0P5S_VAL = 1000;
    TEMP_CALIB_A = 240000;
    TEMP_CALIB_B = 3750000;

    NUM_MEAS_USER = TEMP_STORAGE_SIZE;

/*
    // Harvester Settings --------------------------------------
    hrvv5_r0.HRV_TOP_CONV_RATIO = 0x9;
    mbus_remote_register_write(HRV_ADDR,0,hrvv5_r0.as_int);

    delay(MBUS_DELAY);
*/
    // Go to sleep without timer
    operation_sleep_notimer();
}


//***************************************************
// Temperature measurement operation (SNTv4 retrofit to SNTv2)
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
        temp_sensor_start();
		// Go to sleep during measurement
		operation_sleep();

    }else if (stack_state == STK_TEMP_READ){

		// Get a head start on RDC stabilization
		rdc_enable_vref();  
		
		// Read register
		set_halt_until_mbus_rx();
		mbus_remote_register_read(SNT_ADDR,0x6,1);
		set_halt_until_mbus_tx();
		read_data_temp = *REG1;

		// Change SAR Ratio if temp is within RT range
		if (read_data_temp < PMU_40C_threshold_sns){
			pmu_parking_decision_3v_battery();
		}else{
			// Detect when temp becomes higher than 40C & lock SAR Ratio
			if (temp_storage_latest < PMU_40C_threshold_sns){
				
				// Read the current SAR RATIO
				pmu_reg_write(0x00,0x04);
				uint32_t cur_sar_ratio   = *REG0 & 0x7F;
				pmu_set_sar_override(cur_sar_ratio + 4);
			}
		}
		temp_storage_latest = read_data_temp;



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

		// Assert temp sensor isolation & turn off temp sensor power
		temp_sensor_power_off();
		sns_ldo_power_off();
		stack_state = STK_RDC;

    }else if (stack_state == STK_RDC){

		rdc_power_on();

		rdcv4_r25.RDC_OFFSET_P_LC = 0x1;
		rdcv4_r25.RDC_OFFSET_PB_LC = (~0x1)&0x1F;
		mbus_remote_register_write(RDC_ADDR,0x25,rdcv4_r25.as_int);

	    // Use Timer32 as timeout counter
	    wfi_timeout_flag = 0;
	    config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

		// Start measure
		rdc_start_meas();
	
		// Wait for output
		WFI();
   
	    // Turn off Timer32
	    *TIMER32_GO = 0;

        if (wfi_timeout_flag) {
			read_data_rdc = 0;
			mbus_write_message32(0xFA, 0xFAFAFAFA);
		}else{
			set_halt_until_mbus_rx();
			mbus_remote_register_read(RDC_ADDR,0x11,0);
			read_data_rdc = *REG0;
			set_halt_until_mbus_tx();
		}

		rdc_reset();
        stack_state = STK_RDC2;

    }else if (stack_state == STK_RDC2){

		rdcv4_r25.RDC_OFFSET_P_LC = 0xB;
		rdcv4_r25.RDC_OFFSET_PB_LC = (~0xB)&0x1F;
		mbus_remote_register_write(RDC_ADDR,0x25,rdcv4_r25.as_int);

		//rdc_power_on();

	    // Use Timer32 as timeout counter
	    wfi_timeout_flag = 0;
	    config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

		// Start measure
		rdc_start_meas();
	
		// Wait for output
		WFI();
   
	    // Turn off Timer32
	    *TIMER32_GO = 0;

        if (wfi_timeout_flag) {
			read_data_rdc2 = 0;
			mbus_write_message32(0xFA, 0xFAFAFAFA);
		}else{
			set_halt_until_mbus_rx();
			mbus_remote_register_read(RDC_ADDR,0x11,0);
			read_data_rdc2 = *REG0;
			set_halt_until_mbus_tx();
		}

		rdc_reset();
        stack_state = STK_RDC3;

    }else if (stack_state == STK_RDC3){

		rdcv4_r25.RDC_OFFSET_P_LC = 0x15;
		rdcv4_r25.RDC_OFFSET_PB_LC = (~0x15)&0x1F;
		mbus_remote_register_write(RDC_ADDR,0x25,rdcv4_r25.as_int);

		//rdc_power_on();

	    // Use Timer32 as timeout counter
	    wfi_timeout_flag = 0;
	    config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

		// Start measure
		rdc_start_meas();
	
		// Wait for output
		WFI();
   
	    // Turn off Timer32
	    *TIMER32_GO = 0;

        if (wfi_timeout_flag) {
			read_data_rdc3 = 0;
			mbus_write_message32(0xFA, 0xFAFAFAFA);
		}else{
			set_halt_until_mbus_rx();
			mbus_remote_register_read(RDC_ADDR,0x11,0);
			read_data_rdc3 = *REG0;
			set_halt_until_mbus_tx();
		}

		rdc_reset();
        stack_state = STK_RDC4;

    }else if (stack_state == STK_RDC4){

		rdcv4_r25.RDC_OFFSET_P_LC = 0x1F;
		rdcv4_r25.RDC_OFFSET_PB_LC = 0x0;
		mbus_remote_register_write(RDC_ADDR,0x25,rdcv4_r25.as_int);

		//rdc_power_on();

	    // Use Timer32 as timeout counter
	    wfi_timeout_flag = 0;
	    config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

		// Start measure
		rdc_start_meas();
	
		// Wait for output
		WFI();
   
	    // Turn off Timer32
	    *TIMER32_GO = 0;

        if (wfi_timeout_flag) {
			read_data_rdc4 = 0;
			mbus_write_message32(0xFA, 0xFAFAFAFA);
		}else{
			set_halt_until_mbus_rx();
			mbus_remote_register_read(RDC_ADDR,0x11,0);
			read_data_rdc4 = *REG0;
			set_halt_until_mbus_tx();
		}

		rdc_power_off();
        stack_state = STK_DATA;

    }else if (stack_state == STK_DATA){

		#ifdef DEBUG_MBUS_MSG_1
		mbus_write_message32(0xCC, exec_count);
		#endif
		mbus_write_message32(0xC0, (exec_count << 16) | read_data_temp);
		mbus_write_message32(0xC1, (exec_count << 16) | read_data_rdc);
		mbus_write_message32(0xC2, (exec_count << 16) | read_data_rdc2);
		mbus_write_message32(0xC3, (exec_count << 16) | read_data_rdc3);
		mbus_write_message32(0xC3, (exec_count << 16) | read_data_rdc4);

		// Store results in MEMv1; unless memory is full
		if (temp_storage_count < TEMP_STORAGE_SIZE){
			// Read destination address
			// Memory address is per byte
			// Memory is word-addressable; first word: 0x0, second word: 0x4, third word: 0x8 ...
			uint32_t mem_read_data;

			// Select which RDC reading to store
			// read_data_rdc[14:15] indicates offset range
			if (read_data_rdc > 7800){
				// Store 1st offset
				if (read_data_rdc2 > 7800){
					if (read_data_rdc3 > 7800){
						read_data_rdc = (read_data_rdc4&0x1FFF) | (0x3<<13);
					}else{
						read_data_rdc = (read_data_rdc3&0x1FFF) | (0x2<<13);
					}

				}else{
					read_data_rdc = (read_data_rdc2&0x1FFF) | (0x1<<13);
					
				}
			}

			// Store temp & rdc data in pairs
			mem_read_data = (read_data_temp&0xFFFF) | (read_data_rdc<<16);

			mbus_copy_mem_from_local_to_remote_bulk(MEM_ADDR, (uint32_t*)((temp_storage_count>>1)<<2), (uint32_t*)&mem_read_data, 0);
			temp_storage_count++;
			temp_storage_count++;
		}

		// Optionally transmit the data
		if (radio_tx_option || (exec_count < TEMP_CYCLE_INIT)){
			// Power on radio
			radio_power_on();

			send_radio_data_srr(1,0xC0,*REG_CHIP_ID,((0xBB00|read_data_batadc_diff)<<8)|exec_count,temp_storage_latest,0);
		}

		exec_count++;

		// Make sure Radio is off
		if (radio_on){
			radio_power_off();
		}

        stack_state = STK_IDLE;

		// Use SNT Timer    
		snt_set_wup_timer(WAKEUP_PERIOD_CONT_USER);
		operation_sleep_snt_timer();

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

	// In case GOC was triggered during temp measurement
    if (sns_running){
        sns_running = 0;
        temp_sensor_power_off();
        sns_ldo_power_off();
		rdc_power_off();
		// Restore sleep setting
   	    pmu_sleep_setting_temp_based();
    }

    // Initialize variables & registers
    sns_running = 0;
    stack_state = STK_IDLE;
    
    radio_power_off();
}

static void operation_goc_trigger_radio(uint32_t radio_tx_num, uint32_t wakeup_timer_val, uint8_t radio_tx_prefix, uint8_t radio_tx_data1, uint32_t radio_tx_data0){

    // Prepare radio TX
    radio_power_on();

    if (exec_count_irq < radio_tx_num){
        exec_count_irq++;
        // radio
        send_radio_data_srr(1,radio_tx_prefix,*REG_CHIP_ID,((0xBB00|read_data_batadc_diff)<<8)|radio_tx_data1,radio_tx_data0,1);    
        // set timer
        set_wakeup_timer (wakeup_timer_val, 0x1, 0x1);
        // go to sleep and wake up with same condition
        operation_sleep_noirqreset();

    }else{
        // radio
        send_radio_data_srr(1,radio_tx_prefix,*REG_CHIP_ID,((0xBB00|read_data_batadc_diff)<<8)|radio_tx_data1,radio_tx_data0,1);    
        // Go to sleep without timer
        operation_sleep_notimer();
    }
}

static void operation_snt_calibration_radio_binary(uint32_t start_val, uint32_t settle_time){

    // Prepare radio TX
    if (exec_count_irq > 1){
        radio_power_on();
    }

    if ((sntv2_r0A.TMR_DIFF_CON == 0x3FFF) && ((exec_count_irq & 0xF) == 0xF)){
        // Stop condition
        exec_count_irq = 0;
        sntv2_r0A.TMR_DIFF_CON = 0x3FFF; // Default: 0x3FFF
        // radio
        send_radio_data_srr(1,0xB0,*REG_CHIP_ID,sntv2_r0A.TMR_DIFF_CON,exec_count_irq,0);    
        // Go to sleep without timer
        operation_sleep_notimer();
    }else{
        
        if ((exec_count_irq & 0xF) == 0xF){ // every 16 iterations
            sntv2_r0A.TMR_DIFF_CON = sntv2_r0A.TMR_DIFF_CON<<1 | 0x1; // Default: 0x3FFB
            mbus_remote_register_write(SNT_ADDR,0x0A,sntv2_r0A.as_int);
        }

        exec_count_irq++;
        if (exec_count_irq == 1){
            // SNT pulls higher current in the beginning
        pmu_set_sleep_radio();

            // Tune R for TC
            sntv2_r0A.TMR_DIFF_CON = start_val; // Default: 0x3FFB
            mbus_remote_register_write(SNT_ADDR,0x0A,sntv2_r0A.as_int);

            snt_start_timer_presleep();
            // Go to sleep for >3s for timer stabilization
            set_wakeup_timer(WAKEUP_PERIOD_SNT, 0x1, 0x1);
            operation_sleep_noirqreset();
        }else if (exec_count_irq == 2){
            // Restore PMU settings
        pmu_set_sleep_low();
            snt_start_timer_postsleep();
    
            // Disable PRC Wakeup Timer
            set_wakeup_timer(0, 0, 0);

            // Read existing counter value; in case not reset to zero
            snt_read_wup_counter();
            if (settle_time == 0){
                // set to 30s    
                snt_set_wup_timer(0xEA60);
            }else{
                snt_set_wup_timer(0x1D4C0*settle_time); // settling time in minutes; assumes 2khz clock
            }
            operation_sleep_noirqreset();
        }else{
            // radio
            send_radio_data_srr(1,0xB0,*REG_CHIP_ID,sntv2_r0A.TMR_DIFF_CON, exec_count_irq,0);
            snt_set_wup_timer(WAKEUP_PERIOD_CONT_USER);
            operation_sleep_noirqreset();
        }
    }
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    // Only enable relevant interrupts (PRCv20)
    //enable_reg_irq();
    //enable_all_irq();
    *NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);
  
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);

	// Figure out who triggered wakeup
	if(*SREG_WAKEUP_SOURCE & 0x00000008){
		// Debug
		#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xAA,0x11331133);
		#endif
	}

    // Initialization sequence
    if (enumerated != 0x5043607A){
        operation_init();
    }

	// Read latest batt voltage, unless woken up after temp measurement
	if (stack_state != STK_TEMP_READ){
		pmu_adc_read_latest();
	}

	if (!sns_running){
		// If measurement is running, wait until temp measurement
		pmu_parking_decision_3v_battery();
	}

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

    if(wakeup_data_header == 0x01){
        // Debug mode: Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the MSB of # of transmissions
        operation_goc_trigger_radio(wakeup_data_field_0 + (wakeup_data_field_2<<8), wakeup_data_field_1, 0xA0, read_data_batadc, exec_count_irq);


    }else if(wakeup_data_header == 0x02){
        // Slow down PMU sleep osc and run temp sensor code with desired wakeup period
        // wakeup_data[15:0] is the desired wakeup period in 0.5s (min 0.5s: 0x1, max 32767.5s: 0xFFFF)
        // wakeup_data[20] enables radio tx for each measurement
        // wakeup_data[21] specifies which wakeup timer to use 0: SNT timer, 1: PRC timer
        // wakeup_data[23] is debug mode: logs the execution count as opposed to temp data
        radio_tx_option = wakeup_data_field_2 & 0x10;
        temp_storage_debug = wakeup_data_field_2 & 0x80;
        
		// Use SNT timer
		WAKEUP_PERIOD_CONT_USER = (wakeup_data & 0xFFFF)*SNT_0P5S_VAL; // Unit is 0.5s

        exec_count_irq++;

        if (exec_count_irq == 1){
            // SNT pulls higher current in the beginning
            pmu_set_sleep_radio();
            snt_start_timer_presleep();
            // Go to sleep for >3s for timer stabilization
            set_wakeup_timer (WAKEUP_PERIOD_SNT, 0x1, 0x1);
            operation_sleep_noirqreset();
        }else if (exec_count_irq == 2){
            snt_start_timer_postsleep();
            // Read existing counter value; in case not reset to zero
            snt_read_wup_counter();
            // Restore sleep setting to low
            pmu_set_sleep_low();
        }

        if (!sns_running){
            sns_running = 1;
        }

        exec_count = 0;
        temp_storage_count = 0;
        radio_tx_count = 0;

        // Reset GOC_DATA_IRQ
        *GOC_DATA_IRQ = 0;
        exec_count_irq = 0;

        // Run Temp Sensor Program
	    stack_state = STK_IDLE;
        operation_sns_run();

    }else if(wakeup_data_header == 0x03){
        // Stop temp sensor program and transmit the battery reading and execution count (alternating n times)
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 

        operation_sns_sleep_check();
        snt_stop_timer();

        stack_state = STK_IDLE;

        operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0xC1, exec_count_irq, temp_storage_count);


    }else if(wakeup_data_header == 0x04){
        // Transmit the stored temp sensor data
        // wakeup_data[7:0] specifies # of duplicate packets sent during Data TX; if 0, no duplicate packets

        operation_sns_sleep_check();
        snt_stop_timer();

        stack_state = STK_IDLE;

        radio_tx_count = 0;

        // Prepare radio TX
        radio_power_on();

        if (exec_count_irq == 0){
            radio_packet_count = 0;
        }

        if (exec_count_irq < 2){
            exec_count_irq++;

            if (exec_count_irq == 1){
                send_radio_data_srr(1,0xD0,*REG_CHIP_ID,(0xBB00|read_data_batadc_diff)<<8,temp_storage_count,1); 
            }else{
                send_radio_data_srr(1,0xD1,*REG_CHIP_ID,TEMP_CALIB_A,TEMP_CALIB_B,1);    
            }
            set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
            operation_sleep_noirqreset();
        
        }else{
            operation_tx_stored(wakeup_data_field_0);
        }
        
    }else if(wakeup_data_header == 0x05){
        // Transmit missing data requested by GUI
		// wakeup_data[15:0] is the missing data address
		uint32_t missing_packet_index = wakeup_data;

        // Prepare radio TX
        radio_power_on();

        // Radio out data
        uint32_t mem_read_data[2];
        set_halt_until_mbus_rx();
        // Read 2 words (4 data points) at a time
        mbus_copy_mem_from_remote_to_any_bulk(MEM_ADDR, (uint32_t*)(missing_packet_index<<3), 0x01, (uint32_t*)&mem_read_data, 1);
        set_halt_until_mbus_tx();
    
        send_radio_data_srr(1,0xDD,mem_read_data[1]>>16,((mem_read_data[1]&0xFFFF)<<8) | (mem_read_data[0]>>24),mem_read_data[0]&0xFFFFFF,1);


    }else if(wakeup_data_header == 0x13){
    // Tune SNT Timer R for TC
        sntv2_r0A.TMR_DIFF_CON = wakeup_data & 0x3FFF; // Default: 0x3FFB
        mbus_remote_register_write(SNT_ADDR,0x0A,sntv2_r0A.as_int);

    }else if(wakeup_data_header == 0x14){
        // Update SNT wakeup counter value for 0.5s
        SNT_0P5S_VAL = wakeup_data & 0xFFFF;
        if (SNT_0P5S_VAL == 0){
            SNT_0P5S_VAL = 1000;
        }        

    }else if(wakeup_data_header == 0x15){
        // Update GOC clock
		prev20_r0B.GOC_CLK_GEN_SEL_FREQ = (wakeup_data >> 4)&0x7; // Default 0x0
		prev20_r0B.GOC_CLK_GEN_SEL_DIV = wakeup_data & 0x3; // Default 0x6
		*REG_CLKGEN_TUNE = prev20_r0B.as_int;

    }else if(wakeup_data_header == 0x17){
        // Change the battery reference
        if (wakeup_data_field_0 == 0){
            // Update with the current value
            PMU_ADC_3P0_VAL = read_data_batadc;
        }else{
            PMU_ADC_3P0_VAL = wakeup_data_field_0;
        }

/*
    }else if(wakeup_data_header == 0x18){
		// Harvester Settings --------------------------------------
		hrvv5_r0.HRV_TOP_CONV_RATIO = wakeup_data & 0xFFFF;
		mbus_remote_register_write(HRV_ADDR,0,hrvv5_r0.as_int);
*/
    }else if(wakeup_data_header == 0x1A){
        PMU_10C_threshold_sns = wakeup_data & 0xFFFF; // Around 10C

    }else if(wakeup_data_header == 0x20){
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[23:8] is the desired chip id value
        //                    -->  if zero: reports current chip id
        
        uint32_t chip_id_user;
        chip_id_user = (wakeup_data>>8) & 0xFFFF;

        if (chip_id_user == 0){
            chip_id_user = *REG_CHIP_ID;
        }else{
            *REG_CHIP_ID = chip_id_user;
        }

        operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xA1, 0, chip_id_user);


    }else if(wakeup_data_header == 0x21){
        // Change the radio tx packet delay
        uint32_t user_val = wakeup_data & 0xFFFFFF;
        if (user_val < 500){
            RADIO_PACKET_DELAY = 2000;
        }else{
            RADIO_PACKET_DELAY = user_val;
        }

    }else if(wakeup_data_header == 0x22){
        // Change carrier frequency
        srrv4_r00.SRR_TRX_CAP_ANTP_TUNE = (wakeup_data & 0x3FFF);
        mbus_remote_register_write(SRR_ADDR,0x00,srrv4_r00.as_int);
        srrv4_r01.SRR_TRX_CAP_ANTN_TUNE = (wakeup_data & 0x3FFF);
        mbus_remote_register_write(SRR_ADDR,0x01,srrv4_r01.as_int);

    }else if(wakeup_data_header == 0x23){
        // Change the baseband frequency of SRR (SFO)
        srrv4_r07.RO_MOM = wakeup_data_field_0 & 0x3F;
        srrv4_r07.RO_MIM = wakeup_data_field_1 & 0x3F;
        mbus_remote_register_write(SRR_ADDR,0x07,srrv4_r07.as_int);
        
    }else if(wakeup_data_header == 0x24){
        // Pulse generator settings

        // Use pulse generator
        srrv4_r02.SRR_TX_PULSE_FINE = wakeup_data_field_1;
        srrv4_r02.SRR_TX_PULSE_FINE_TUNE = wakeup_data_field_0 & 0xF;
        mbus_remote_register_write(SRR_ADDR,0x02,srrv4_r02.as_int);

    }else if(wakeup_data_header == 0x25){
        // Change the conversion time of the temp sensor
        sntv2_r03.TSNS_SEL_CONV_TIME = wakeup_data & 0xF; // Default: 0x6
        mbus_remote_register_write(SNT_ADDR,0x03,sntv2_r03.as_int);

    }else if(wakeup_data_header == 0x2A){
        // Update calibration coefficient A
        // A is the slope, typical value is around 24.000, stored as A*1000
        TEMP_CALIB_A = wakeup_data & 0xFFFFFF; 

    }else if(wakeup_data_header == 0x2B){
        // Update calibration coefficient B 
        // B is the offset, typical value is around -3750.000, stored as -B*1000
        TEMP_CALIB_B = wakeup_data & 0xFFFFFF; 

    }else if(wakeup_data_header == 0x2C){
        // Change SRR C_LEN
        srrv4_r13.SRR_RAD_FSM_TX_C_LEN = wakeup_data & 0xFFFF; // (PW_LEN+1):C_LEN=1:32
        mbus_remote_register_write(SRR_ADDR,0x13,srrv4_r13.as_int);

    }else if(wakeup_data_header == 0x2E){
        // Change SRR TX Amp Current
    srrv4_r02.SRR_TX_BIAS_TUNE = wakeup_data & 0x1FFF;  //Set TX BIAS TUNE 13b // Set to max
        mbus_remote_register_write(SRR_ADDR,0x02,srrv4_r02.as_int);

    }else if(wakeup_data_header == 0x2F){
        // Change SRR Current Limiter Settings
    srrv4_r00.SRR_CL_CTRL = wakeup_data & 0xFF; //Set CL 1-finite 16-20uA
        mbus_remote_register_write(SRR_ADDR,0x00,srrv4_r00.as_int);

    }else if(wakeup_data_header == 0x41){
        // Change PMU solar short clamp value
        pmu_reg_write(0x0E, 
            ( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
            | (1 << 9)  // Enables override setting [8]
            | (0 << 8)  // Turn on the harvester-inhibiting switch
            | (wakeup_data & 0xF0)  // clamp_tune_bottom (increases clamp thresh)
            | (wakeup_data & 0xF)        // clamp_tune_top (decreases clamp thresh)
        ));
        pmu_reg_write(0x0E, 
            ( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
            | (0 << 9)  // Enables override setting [8]
            | (0 << 8)  // Turn on the harvester-inhibiting switch
            | (wakeup_data & 0xF0)  // clamp_tune_bottom (increases clamp thresh)
            | (wakeup_data & 0xF)        // clamp_tune_top (decreases clamp thresh)
        ));

    }else if(wakeup_data_header == 0x42){
	    // RDC Gain & Offset settings
		rdcv4_r24.RDC_SEL_GAIN_LC = wakeup_data & 0x1F; // 5 bits
		rdcv4_r25.RDC_OFFSET_P_LC = (wakeup_data >> 8) & 0x1F; // 5 bits
		rdcv4_r25.RDC_OFFSET_PB_LC = ~((wakeup_data >> 8) & 0x1F);

		mbus_remote_register_write(RDC_ADDR,0x24,rdcv4_r24.as_int);
		mbus_remote_register_write(RDC_ADDR,0x25,rdcv4_r25.as_int);
		
    }else if(wakeup_data_header == 0x71){
    // Set SNT wakeup period
        WAKEUP_PERIOD_CONT_USER = wakeup_data & 0xFFFFFF;

	}else if(wakeup_data_header == 0x72){
       // Calibration routine for SNT wakeup timer
       operation_snt_calibration_radio_binary(wakeup_data & 0xFFFF, wakeup_data_field_2);

    }else if(wakeup_data_header == 0xF0){

        operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xA2, enumerated>>24, enumerated);

    }else if(wakeup_data_header == 0xFA){
    // Soft reset routine

        // Extra protection
        if ((wakeup_data&0xFFFFFF) == 0x89D7E2){
            config_timerwd(0xFF);
            while(1){
                mbus_write_message32(0xE0, 0x0);
                delay(MBUS_DELAY);
            }
        }
    }else{
    }

    if (sns_running){
        // Proceed to continuous mode
        while(1){
            operation_sns_run();
        }
    }

	// Radio back the GOC data that triggered this wakeup
    operation_goc_trigger_radio(0, WAKEUP_PERIOD_RADIO_INIT, 0xAD, wakeup_data_header, wakeup_data);
    operation_sleep_notimer();

    while(1);
}


