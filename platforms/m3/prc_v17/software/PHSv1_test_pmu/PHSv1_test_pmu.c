//*******************************************************************
//Author: 
//Description: 
//*******************************************************************
#include "PRCv17.h"
//#include "SNSv10_RF.h"
#include "PHSv1A_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define PHS_ADDR    0x4
#define HRV_ADDR 0x3
#define SRR_ADDR 0x7
#define SNT_ADDR 0x5
#define PMU_ADDR 0x6

// Flag Idx
#define FLAG_ENUM       0

//#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define	MBUS_DELAY 200 // Amount of delay between successive messages; 100: 6-7ms
//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t cyc_num;
volatile phsv1a_r00_t phsv1a_r00 = PHSv1A_R00_DEFAULT;
volatile phsv1a_r01_t phsv1a_r01 = PHSv1A_R01_DEFAULT;
volatile phsv1a_r02_t phsv1a_r02 = PHSv1A_R02_DEFAULT;
volatile phsv1a_r03_t phsv1a_r03 = PHSv1A_R03_DEFAULT;
volatile phsv1a_r04_t phsv1a_r04 = PHSv1A_R04_DEFAULT;
volatile phsv1a_r05_t phsv1a_r05 = PHSv1A_R05_DEFAULT;
volatile phsv1a_r06_t phsv1a_r06 = PHSv1A_R06_DEFAULT;
volatile phsv1a_r07_t phsv1a_r07 = PHSv1A_R07_DEFAULT;
volatile phsv1a_r08_t phsv1a_r08 = PHSv1A_R08_DEFAULT;
volatile phsv1a_r09_t phsv1a_r09 = PHSv1A_R09_DEFAULT;
volatile phsv1a_r0E_t phsv1a_r0E = PHSv1A_R0E_DEFAULT;
volatile phsv1a_r0F_t phsv1a_r0F = PHSv1A_R0F_DEFAULT;
volatile phsv1a_r10_t phsv1a_r10 = PHSv1A_R10_DEFAULT;
volatile phsv1a_r11_t phsv1a_r11 = PHSv1A_R11_DEFAULT;
volatile phsv1a_r12_t phsv1a_r12 = PHSv1A_R12_DEFAULT;
volatile phsv1a_r13_t phsv1a_r13 = PHSv1A_R13_DEFAULT;
volatile phsv1a_r14_t phsv1a_r14 = PHSv1A_R14_DEFAULT;
volatile phsv1a_r15_t phsv1a_r15 = PHSv1A_R15_DEFAULT;
volatile phsv1a_r16_t phsv1a_r16 = PHSv1A_R16_DEFAULT;
volatile phsv1a_r17_t phsv1a_r17 = PHSv1A_R17_DEFAULT;
volatile uint32_t wakeup_data;

volatile uint32_t PMU_ADC_4P2_VAL;
volatile uint32_t read_data_batadc_diff;
volatile uint32_t read_data_batadc;

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer16  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbustx   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusrx   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusfwd  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg4     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg5     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg6     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusmem  (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    }
void handler_ext_int_timer16(void) { // TIMER16
    *NVIC_ICPR = (0x1 << IRQ_TIMER16);
    *REG1 = *TIMER16_CNT;
    *REG2 = *TIMER16_STAT;
    *TIMER16_STAT = 0x0;
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
void handler_ext_int_reg4(void) { // REG4
    *NVIC_ICPR = (0x1 << IRQ_REG4);
}
void handler_ext_int_reg5(void) { // REG5
    *NVIC_ICPR = (0x1 << IRQ_REG5);
}
void handler_ext_int_reg6(void) { // REG6
    *NVIC_ICPR = (0x1 << IRQ_REG6);
}
void handler_ext_int_reg7(void) { // REG7
    *NVIC_ICPR = (0x1 << IRQ_REG7);
}
void handler_ext_int_mbusmem(void) { // MBUS_MEM_WR
    *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM);
}
void handler_ext_int_mbusrx(void) { // MBUS_RX
    *NVIC_ICPR = (0x1 << IRQ_MBUS_RX);
}
void handler_ext_int_mbustx(void) { // MBUS_TX
    *NVIC_ICPR = (0x1 << IRQ_MBUS_TX);
}
void handler_ext_int_mbusfwd(void) { // MBUS_FWD
    *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD);
}
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
}
void handler_ext_int_softreset(void) { // SOFT_RESET
    *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET);
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP);
    *SREG_WAKEUP_SOURCE = 0;
}

//************************************
// PMU Related Functions
//************************************

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
		| (base) 		// Floor frequency base (0-63)
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
		| (base) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x18: V3P6 Active 
    mbus_remote_register_write(PMU_ADDR,0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x1A: V0P6 Active
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
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
		| (base) 		// Floor frequency base (0-63)
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
		| (base) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x19: V0P6 Sleep
    mbus_remote_register_write(PMU_ADDR,0x19,
		( (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);

}

inline static void pmu_set_sleep_radio(){
	pmu_set_sleep_clk(0xF,0xA,0x5,0xF/*V1P2*/);
}

inline static void pmu_set_sleep_low(){
	pmu_set_sleep_clk(0x1,0x1,0x1,0x1/*V1P2*/);
}



inline static void pmu_set_clk_init(){
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
		| (47) 		// Binary converter's conversion ratio (7'h00)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (1 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (47) 		// Binary converter's conversion ratio (7'h00)
	));
	delay(MBUS_DELAY);

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
	if (read_data_batadc<PMU_ADC_4P2_VAL){
		read_data_batadc_diff = 0;
	}else{
		read_data_batadc_diff = read_data_batadc - PMU_ADC_4P2_VAL;
	}

}

inline static void pmu_reset_solar_short(){
    mbus_remote_register_write(PMU_ADDR,0x0E, 
		( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
		| (1 << 9)  // Enables override setting [8]
		| (0 << 8)  // Turn on the harvester-inhibiting switch
		| (3 << 4)  // clamp_tune_bottom (increases clamp thresh)
		| (0) 		// clamp_tune_top (decreases clamp thresh)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x0E, 
		( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
		| (0 << 9)  // Enables override setting [8]
		| (0 << 8)  // Turn on the harvester-inhibiting switch
		| (3 << 4)  // clamp_tune_bottom (increases clamp thresh)
		| (0) 		// clamp_tune_top (decreases clamp thresh)
	));
	delay(MBUS_DELAY);
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    set_flag(FLAG_ENUM, 1);
    cyc_num = 0;

    // Set Halt
    //set_halt_until_mbus_trx();

    // Enumeration
    mbus_enumerate(SRR_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(PHS_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(HRV_ADDR);
	delay(MBUS_DELAY);
 	mbus_enumerate(PMU_ADDR);
	delay(MBUS_DELAY);


    //Set Halt
    //set_halt_until_mbus_tx();

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

}

void phs_basic_setup_with_reset(void){
    // enable osc
    phsv1a_r11.osc_en_fmbus_1p2=1;
    phsv1a_r11.sel_osc_freq_fmbus_1p2=1;
    mbus_remote_register_write(PHS_ADDR, 0x11, phsv1a_r11.as_int);
    delay(MBUS_DELAY); // should be long enough

//    // rstb ==> It writes weird data... why?
//    phsv1a_r17.rstb_fmbus_1p2=0;
//    mbus_remote_register_write(PHS_ADDR, 0x17, phsv1a_r17.as_int);
//    delay(MBUS_DELAY);
//    phsv1a_r17.rstb_fmbus_1p2=1;
//    mbus_remote_register_write(PHS_ADDR, 0x17, phsv1a_r17.as_int);
//    delay(MBUS_DELAY);
    // This is working. So, the problem is phsv1a_r17.as_int.. Maybe some bug?
    mbus_remote_register_write(PHS_ADDR, 0x17, 0); 
    delay(MBUS_DELAY);
    mbus_remote_register_write(PHS_ADDR, 0x17, 1); 
    delay(MBUS_DELAY);

    // Release isolation
    phsv1a_r16.ISOLATE_PD1P8=0;
    mbus_remote_register_write(PHS_ADDR, 0x16, phsv1a_r16.as_int);
    delay(MBUS_DELAY);
}

void phs_ldo_turn_off(void){
    phsv1a_r14.sel_avdd_ext_fmbus_1p2=1;
    mbus_remote_register_write(PHS_ADDR, 0x14, phsv1a_r14.as_int);
    delay(MBUS_DELAY);
}

void phs_ldo_turn_on(void){
    phsv1a_r14.sel_avdd_ext_fmbus_1p2=0;
    mbus_remote_register_write(PHS_ADDR, 0x14, phsv1a_r14.as_int);
    delay(MBUS_DELAY);
}
    
void phs_source_drain_obs(void){
    // Enable the observation of source and drain
    phsv1a_r13.sel_vana_obs1_fmbus_1p2=0;
    phsv1a_r13.sel_vana_obs2_fmbus_1p2=0;
    phsv1a_r13.sel_vana_obs3_fmbus_1p2=0;
    phsv1a_r13.sel_vana_obs4_fmbus_1p2=0;
    mbus_remote_register_write(PHS_ADDR, 0x13, phsv1a_r13.as_int);
    delay(MBUS_DELAY);

    phsv1a_r0E.en_drain_obs_isfet_fmbus_1p2=1;
    phsv1a_r0E.en_source_obs_isfet_fmbus_1p2=1;
    mbus_remote_register_write(PHS_ADDR, 0x0E, phsv1a_r0E.as_int);
    delay(MBUS_DELAY);

    phsv1a_r0F.en_drain_obs_refet_fmbus_1p2=1;
    phsv1a_r0F.en_source_obs_refet_fmbus_1p2=1;
    mbus_remote_register_write(PHS_ADDR, 0x0F, phsv1a_r0F.as_int);
    delay(MBUS_DELAY);
}

void phs_prc_reg_write(uint32_t addr, uint32_t data){
    // Kind of trick
    // Must run before actually changing 0x01 of PHS reg
    // Should know the proper function
    // Use phsv1a_r01 as a buffer -> turn it back after the operation
    
    mbus_remote_register_write(PHS_ADDR, 0x01, data);
        set_halt_until_mbus_trx();
    mbus_copy_registers_from_remote_to_local(PHS_ADDR, 0x01, addr, 0);
        set_halt_until_mbus_tx();
    mbus_remote_register_write(PHS_ADDR, 0x01, 0x1f40);
}

void phs_default_trim(void){
    phsv1a_r10.ctrl_vbias100pa_fmbus_1p2=0xa;
    mbus_remote_register_write(PHS_ADDR, 0x10, phsv1a_r10.as_int);
    delay(MBUS_DELAY);
    phsv1a_r11.sel_osc_freq_fmbus_1p2=0x9;
    mbus_remote_register_write(PHS_ADDR, 0x11, phsv1a_r11.as_int);
    delay(MBUS_DELAY);

    // dac_code_ref_adcref
    phsv1a_r04.dac_code_ref_adcref_isfet_fmbus_1p2=300;
    mbus_remote_register_write(PHS_ADDR, 0x04, phsv1a_r04.as_int);
    delay(MBUS_DELAY);
    phsv1a_r09.dac_code_ref_adcref_refet_fmbus_1p2=300;
    mbus_remote_register_write(PHS_ADDR, 0x09, phsv1a_r09.as_int);
    delay(MBUS_DELAY);

    // External Vaq
    phsv1a_r11.en_vaq_ext_fmbus_1p2=1;
    mbus_remote_register_write(PHS_ADDR, 0x11, phsv1a_r11.as_int);
    delay(MBUS_DELAY);

    // continuous count
    phsv1a_r01.count_adc_continuous_value_isfet_fmbus_1p2=4000;
    mbus_remote_register_write(PHS_ADDR, 0x01, phsv1a_r01.as_int);
    delay(MBUS_DELAY);
    phsv1a_r06.count_adc_continuous_value_refet_fmbus_1p2=4000;
    mbus_remote_register_write(PHS_ADDR, 0x06, phsv1a_r06.as_int);
    delay(MBUS_DELAY);
}

//static void operation_goc_trigger_init(void){
//
//    // This is critical
//    set_halt_until_mbus_tx();
//    mbus_write_message32(0xAA,0xABCD1234);
//    mbus_write_message32(0xAA,wakeup_data);
//
//    // Initialize variables & registers
//    sns_running = 0;
//    Pstack_state = PSTK_IDLE;
//    
//    radio_power_off();
//    temp_sensor_power_off();
//    sns_ldo_power_off();
//    rdc_assert_pg();
//}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    // Weird value in r17, code bug
//    mbus_remote_register_write(0x08, 0x17, phsv1a_r17.as_int);
//    delay(MBUS_DELAY);

//**********************************************************
// Copied from Pstack_ondemand_v3.3_BAT3V.c
//**********************************************************
    *NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);

    // Initialization Sequence
    if (!get_flag(FLAG_ENUM)) { 
        initialization();
    }

    // Check if wakeup is due to GOC interrupt  
    // 0x8C is reserved for GOC-triggered wakeup (Named GOC_DATA_IRQ)
    // 8 MSB bits of the wakeup data are used for function ID
    wakeup_data = *GOC_DATA_IRQ;
    uint32_t wakeup_data_header = (wakeup_data>>24) & 0xFF;
    uint32_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;
    uint32_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;

//    // In case GOC triggered in the middle of routines
//    if ((wakeup_data_header != 0) && (exec_count_irq == 0)){
//        operation_goc_trigger_init();
//    }
//***********************************************************
    

    //***********************************************************
    // Routine description
    //***********************************************************
     
    // Turn on LDO : Needs some time to fully turned on 
    if (wakeup_data_header == 1){
        if (wakeup_data_field_2==0){
            phs_ldo_turn_on();
        }
        else if (wakeup_data_field_2==1){
            phs_ldo_turn_off();
        }
    }
    
    // Basic setup
    else if (wakeup_data_header == 2){
        phs_basic_setup_with_reset();
        if (wakeup_data_field_2==0){
            phs_source_drain_obs();
        }
        phs_default_trim();
    }

    // Calibration mode
    else if(wakeup_data_header == 3){
        // turn of VDs first
        phsv1a_r00.en_vds_isfet_fmbus_1p2=0;
        // turn off adc 
        phsv1a_r00.en_adc_continuos_isfet_fmbus_1p2=0;
        mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
        delay(MBUS_DELAY);
        phsv1a_r05.en_vds_refet_fmbus_1p2=0;
        phsv1a_r05.en_adc_continuos_refet_fmbus_1p2=0;
        mbus_remote_register_write(PHS_ADDR, 0x05, phsv1a_r05.as_int);
        delay(MBUS_DELAY);



        // ISFET
        phsv1a_r00.en_cal_isfet_fmbus_1p2=1;
        phsv1a_r00.adc_mode_isfet_fmbus_1p2=1;
        mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
        delay(MBUS_DELAY);
        phsv1a_r0E.sw1_cvcc_isfet_fmbus_1p2=0;
        phsv1a_r0E.sw2_cvcc_isfet_fmbus_1p2=1;
        phsv1a_r0E.sw3_cvcc_isfet_fmbus_1p2=1;
        phsv1a_r0E.sw4_cvcc_isfet_fmbus_1p2=0;
        phsv1a_r0E.sw5_cvcc_isfet_fmbus_1p2=1;
        phsv1a_r0E.sw6_cvcc_isfet_fmbus_1p2=0;
        phsv1a_r0E.en_ccap_isfet_fmbus_1p2=1;
        mbus_remote_register_write(PHS_ADDR, 0x0E, phsv1a_r0E.as_int);
        delay(MBUS_DELAY);
        // REFET
        phsv1a_r05.en_cal_refet_fmbus_1p2=1;
        phsv1a_r05.adc_mode_refet_fmbus_1p2=1;
        mbus_remote_register_write(PHS_ADDR, 0x05, phsv1a_r05.as_int);
        delay(MBUS_DELAY);
        phsv1a_r0F.sw1_cvcc_refet_fmbus_1p2=0;
        phsv1a_r0F.sw2_cvcc_refet_fmbus_1p2=1;
        phsv1a_r0F.sw3_cvcc_refet_fmbus_1p2=1;
        phsv1a_r0F.sw4_cvcc_refet_fmbus_1p2=0;
        phsv1a_r0F.sw5_cvcc_refet_fmbus_1p2=1;
        phsv1a_r0F.sw6_cvcc_refet_fmbus_1p2=0;
        phsv1a_r0F.en_ccap_refet_fmbus_1p2=1;
        mbus_remote_register_write(PHS_ADDR, 0x0F, phsv1a_r0F.as_int);
        delay(MBUS_DELAY);
    }
    
    // single ADc operation and read the output
    else if(wakeup_data_header == 4){
        if (wakeup_data_field_2==0){
            // ISFET
            phsv1a_r00.en_adc_single_isfet_fmbus_1p2=0;
            mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
            delay(MBUS_DELAY);
            phsv1a_r00.en_adc_single_isfet_fmbus_1p2=1;
            mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
            delay(MBUS_DELAY);
            // REFET
            phsv1a_r05.en_adc_single_refet_fmbus_1p2=0;
            mbus_remote_register_write(PHS_ADDR, 0x05, phsv1a_r05.as_int);
            delay(MBUS_DELAY);
            phsv1a_r05.en_adc_single_refet_fmbus_1p2=1;
            mbus_remote_register_write(PHS_ADDR, 0x05, phsv1a_r05.as_int);
            delay(MBUS_DELAY);
        }
        // Read ADC value
        delay(MBUS_DELAY*100);
        set_halt_until_mbus_trx();
        mbus_copy_registers_from_remote_to_local(PHS_ADDR, 0x0a, 0x00, 0);
        set_halt_until_mbus_tx();
        mbus_write_message32(0xAD, *REG0);
    }

    // VDS value write
    else if(wakeup_data_header == 5){
        phsv1a_r03.dac_code_vds_operate_isfet_fmbus_1p2=wakeup_data_field_2*16 + wakeup_data_field_1/16;  
        mbus_remote_register_write(PHS_ADDR, 0x03, phsv1a_r03.as_int);
        delay(MBUS_DELAY);
        phsv1a_r08.dac_code_vds_operate_refet_fmbus_1p2=wakeup_data_field_1%16*256+wakeup_data_field_0;
        mbus_remote_register_write(PHS_ADDR, 0x08, phsv1a_r08.as_int);
        delay(MBUS_DELAY);
    }

    // CVCC
    else if(wakeup_data_header == 6){
        // ISFET
        phsv1a_r00.en_cal_isfet_fmbus_1p2=0;
        phsv1a_r00.adc_mode_isfet_fmbus_1p2=0;
        mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
        delay(MBUS_DELAY);
        phsv1a_r0E.sw1_cvcc_isfet_fmbus_1p2=1;
        phsv1a_r0E.sw2_cvcc_isfet_fmbus_1p2=0;
        phsv1a_r0E.sw3_cvcc_isfet_fmbus_1p2=1;
        phsv1a_r0E.sw4_cvcc_isfet_fmbus_1p2=0;
        phsv1a_r0E.sw5_cvcc_isfet_fmbus_1p2=0;
        phsv1a_r0E.sw6_cvcc_isfet_fmbus_1p2=1;
        phsv1a_r0E.en_ccap_isfet_fmbus_1p2=1;
        mbus_remote_register_write(PHS_ADDR, 0x0E, phsv1a_r0E.as_int);
        delay(MBUS_DELAY);
        // REFET
        phsv1a_r05.en_cal_refet_fmbus_1p2=0;
        phsv1a_r05.adc_mode_refet_fmbus_1p2=0;
        mbus_remote_register_write(PHS_ADDR, 0x05, phsv1a_r05.as_int);
        delay(MBUS_DELAY);
        phsv1a_r0F.sw1_cvcc_refet_fmbus_1p2=1;
        phsv1a_r0F.sw2_cvcc_refet_fmbus_1p2=0;
        phsv1a_r0F.sw3_cvcc_refet_fmbus_1p2=1;
        phsv1a_r0F.sw4_cvcc_refet_fmbus_1p2=0;
        phsv1a_r0F.sw5_cvcc_refet_fmbus_1p2=0;
        phsv1a_r0F.sw6_cvcc_refet_fmbus_1p2=1;
        phsv1a_r0F.en_ccap_refet_fmbus_1p2=1;
        mbus_remote_register_write(PHS_ADDR, 0x0F, phsv1a_r0F.as_int);
        delay(MBUS_DELAY);

        // After changing switch configuration, enable VDS switched capacitor
        phsv1a_r00.en_vds_isfet_fmbus_1p2=1;
        mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
        delay(MBUS_DELAY);
        phsv1a_r05.en_vds_refet_fmbus_1p2=1;
        mbus_remote_register_write(PHS_ADDR, 0x05, phsv1a_r05.as_int);
        delay(MBUS_DELAY);
    }

    // ADC continuous mode
    else if(wakeup_data_header == 7){
        // turn off
        if (wakeup_data_field_2==0){
            phsv1a_r00.en_adc_continuos_isfet_fmbus_1p2=0;
            mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
            delay(MBUS_DELAY);
            phsv1a_r05.en_adc_continuos_refet_fmbus_1p2=0;
            mbus_remote_register_write(PHS_ADDR, 0x05, phsv1a_r05.as_int);
            delay(MBUS_DELAY);
        }
        // en_adc_continuous=1 for isfet
        else if (wakeup_data_field_2==1){
            phsv1a_r00.en_adc_continuos_isfet_fmbus_1p2=1;
            mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
            delay(MBUS_DELAY);
        }
        // en_adc_continuous=1 for refet
        else if (wakeup_data_field_2==2){
            phsv1a_r05.en_adc_continuos_refet_fmbus_1p2=1;
            mbus_remote_register_write(PHS_ADDR, 0x05, phsv1a_r05.as_int);
            delay(MBUS_DELAY);
        }
        // en_adc_continuous=1 for both
        else if (wakeup_data_field_2==3){
            phsv1a_r00.en_adc_continuos_isfet_fmbus_1p2=1;
            mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
            delay(MBUS_DELAY);
            phsv1a_r05.en_adc_continuos_refet_fmbus_1p2=1;
            mbus_remote_register_write(PHS_ADDR, 0x05, phsv1a_r05.as_int);
            delay(MBUS_DELAY);
        }
        
        // en_avg_isfet_fmbus_1p2
        phsv1a_r04.en_avg_isfet_fmbus_1p2=wakeup_data_field_1;
        mbus_remote_register_write(PHS_ADDR, 0x04, phsv1a_r04.as_int);
        delay(MBUS_DELAY);
        phsv1a_r09.en_avg_refet_fmbus_1p2=wakeup_data_field_1;
        mbus_remote_register_write(PHS_ADDR, 0x09, phsv1a_r09.as_int);
        delay(MBUS_DELAY);
    }

    // Read ADC output
    else if(wakeup_data_header == 8){
        // adc_output_code_isfet/refet
        set_halt_until_mbus_trx();
        mbus_copy_registers_from_remote_to_local(PHS_ADDR, 0x0a, 0x00, 0);
        set_halt_until_mbus_tx();
        mbus_write_message32(0xAD, *REG0);
        delay(MBUS_DELAY);
//        // adc_code_avg_output_isfet
//        set_halt_until_mbus_trx();
//        mbus_copy_registers_from_remote_to_local(PHS_ADDR, 0x0b, 0x00, 0);
//        set_halt_until_mbus_tx();
//        mbus_write_message32(0xAA, *REG0);
//        delay(MBUS_DELAY);
//        // adc_code_avg_output_refet
//        set_halt_until_mbus_trx();
//        mbus_copy_registers_from_remote_to_local(PHS_ADDR, 0x0c, 0x00, 0);
//        set_halt_until_mbus_tx();
//        mbus_write_message32(0xAB, *REG0);
//        delay(MBUS_DELAY);
    }

    // Read ADC output continuosly
    else if(wakeup_data_header == 9){
        while(wakeup_data_field_2==1){
            set_halt_until_mbus_trx();
            mbus_copy_registers_from_remote_to_local(PHS_ADDR, 0x0a, 0x00, 0);
            set_halt_until_mbus_tx();
            mbus_write_message32(0xAD, *REG0);
            delay(MBUS_DELAY*150);
			config_timerwd(0xFFFFF);
        }
    }

    //******************************************************
    // Trmming mode
    //******************************************************
    // Usually they are
    // wakeup_data_field_2 : Select the target
    // wakeup_data_field_1 : ISFET/REFET
    // wakeup_data_field_0 : value

    else if(wakeup_data_header == 0xa){
 	    // sel_mux_vbias_iref_isfet_fmbus_1p2
        if (wakeup_data_field_2==0){
         	//ISFET
            if (wakeup_data_field_1==0){
                phsv1a_r11.sel_mux_vbias_iref_isfet_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x11, phsv1a_r11.as_int);
                delay(MBUS_DELAY);
            }
            //REFET
            else if (wakeup_data_field_1==1){
                phsv1a_r12.sel_mux_vbias_iref_refet_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x12, phsv1a_r12.as_int);
                delay(MBUS_DELAY);
            }
        }

        // sel_amp_bias_con
        else if (wakeup_data_field_2==1){
         	//ISFET
            if (wakeup_data_field_1==0){
                phsv1a_r0E.sel_amp_bias_con_isfet_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x0E, phsv1a_r0E.as_int);
                delay(MBUS_DELAY);
            }
         	//REFET
            else if (wakeup_data_field_1==1){
                phsv1a_r0F.sel_amp_bias_con_refet_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x0F, phsv1a_r0F.as_int);
                delay(MBUS_DELAY);
            }
        }

        // sel_mux_vsource_cal_refet_fmbus_1p2
        else if (wakeup_data_field_2==2){
         	//ISFET
            if (wakeup_data_field_1==0){
                phsv1a_r12.sel_mux_vsource_cal_isfet_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x12, phsv1a_r12.as_int);
                delay(MBUS_DELAY);
            }
            else if (wakeup_data_field_1==1){
                phsv1a_r12.sel_mux_vsource_cal_refet_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x12, phsv1a_r12.as_int);
                delay(MBUS_DELAY);
            }
        }

        // IREF
        else if (wakeup_data_field_2==3){
            // ctrl_vbias100pa_fmbus_1p2 : Main current branch
            if (wakeup_data_field_1==0){
                phsv1a_r10.ctrl_vbias100pa_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x10, phsv1a_r10.as_int);
                delay(MBUS_DELAY);
            }
            // Oscillator iref : ctrl_iref1_fmbus_1p2		: 4;
            else if (wakeup_data_field_1==1){
                phsv1a_r10.ctrl_iref1_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x10, phsv1a_r10.as_int);
                delay(MBUS_DELAY);
            } 
            // iref_amp_isfet
            else if (wakeup_data_field_1==2){
                phsv1a_r10.ctrl_iref2_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x10, phsv1a_r10.as_int);
                delay(MBUS_DELAY);
            }
            // iref_amp_refet
            else if (wakeup_data_field_1==3){
                phsv1a_r10.ctrl_iref3_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x10, phsv1a_r10.as_int);
                delay(MBUS_DELAY);
            }
            // iref_amp_vaq
            else if (wakeup_data_field_1==4){
                phsv1a_r10.ctrl_iref4_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x10, phsv1a_r10.as_int);
                delay(MBUS_DELAY);
            }
        }

        // VAQ related
        else if (wakeup_data_field_2==4){
            // vref_aq
            if (wakeup_data_field_1==0){
                phsv1a_r12.sel_mux_vref_aq_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x12, phsv1a_r12.as_int);
                delay(MBUS_DELAY);
            }
            // en_vaq_ext
            else if (wakeup_data_field_1==1){
                phsv1a_r11.en_vaq_ext_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x11, phsv1a_r11.as_int);
                delay(MBUS_DELAY);
            }
            // sel_vaq_amp_bias_con_fmbus_1p2
            else if (wakeup_data_field_1==2){
                phsv1a_r11.sel_vaq_amp_bias_con_fmbus_1p2=wakeup_data_field_0;
                mbus_remote_register_write(PHS_ADDR, 0x11, phsv1a_r11.as_int);
                delay(MBUS_DELAY);
            }
        }

        // sel_osc_freq
        else if (wakeup_data_field_2==5){
            phsv1a_r11.sel_osc_freq_fmbus_1p2=wakeup_data_field_1;
            mbus_remote_register_write(PHS_ADDR, 0x11, phsv1a_r11.as_int);
            delay(MBUS_DELAY);
        }
        // count_adc_continuous_value
        // Temporary
        else if (wakeup_data_field_2==6){
            phsv1a_r01.count_adc_continuous_value_isfet_fmbus_1p2=4000;
            mbus_remote_register_write(PHS_ADDR, 0x01, phsv1a_r01.as_int);
            delay(MBUS_DELAY);
            phsv1a_r06.count_adc_continuous_value_refet_fmbus_1p2=4000;
            mbus_remote_register_write(PHS_ADDR, 0x06, phsv1a_r06.as_int);
            delay(MBUS_DELAY);
        }
    }

    //******************************************************
    // Mode turn on/off
    //******************************************************
    else if(wakeup_data_header == 0x10){

        // source and drain obs on/off
        if (wakeup_data_field_2==0){
            if (wakeup_data_field_1==0){
                phs_source_drain_obs();
            }
            else if (wakeup_data_field_1==1){ // Disconnect
                phsv1a_r0E.en_drain_obs_isfet_fmbus_1p2=0;
                phsv1a_r0E.en_source_obs_isfet_fmbus_1p2=0;
                mbus_remote_register_write(PHS_ADDR, 0x0E, phsv1a_r0E.as_int);
                delay(MBUS_DELAY);
                phsv1a_r0F.en_drain_obs_refet_fmbus_1p2=0;
                phsv1a_r0F.en_source_obs_refet_fmbus_1p2=0;
                mbus_remote_register_write(PHS_ADDR, 0x0F, phsv1a_r0F.as_int);
                delay(MBUS_DELAY);
            }
        }
        
        
        // sel_iref_fet_ext_isfet_fmbus_1p2
        else if (wakeup_data_field_2==1){
            if (wakeup_data_field_1==0){ // ISFET
                if (wakeup_data_field_0==0){ // ext on
                    phsv1a_r0E.sel_iref_fet_ext_isfet_fmbus_1p2=1;
                    mbus_remote_register_write(PHS_ADDR, 0x0E, phsv1a_r0E.as_int);
                    delay(MBUS_DELAY);
                }
                else if (wakeup_data_field_0==1){ // ext off (internal)
                    phsv1a_r0E.sel_iref_fet_ext_isfet_fmbus_1p2=0;
                    mbus_remote_register_write(PHS_ADDR, 0x0E, phsv1a_r0E.as_int);
                    delay(MBUS_DELAY);
                } 
            }
            else if (wakeup_data_field_1==1){ // REFET
                if (wakeup_data_field_0==0){ // ext on
                    phsv1a_r0F.sel_iref_fet_ext_refet_fmbus_1p2=1;
                    mbus_remote_register_write(PHS_ADDR, 0x0F, phsv1a_r0F.as_int);
                    delay(MBUS_DELAY);
                }
                else if (wakeup_data_field_0==1){ // ext off (internal)
                    phsv1a_r0F.sel_iref_fet_ext_refet_fmbus_1p2=0;
                    mbus_remote_register_write(PHS_ADDR, 0x0F, phsv1a_r0F.as_int);
                    delay(MBUS_DELAY);
                } 
            }
        }

        // sel_dac
        else if (wakeup_data_field_2==2){
            phsv1a_r00.sel_dac_isfet_fmbus_1p2=wakeup_data_field_1;
            mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
            delay(MBUS_DELAY);
        }
    }
    

    //******************************************************
    // Debugging mode
    //******************************************************
    // read reg of PHS
    else if(wakeup_data_header == 0x20){
        set_halt_until_mbus_trx();
        mbus_copy_registers_from_remote_to_local(PHS_ADDR, wakeup_data_field_2, 0x00, 0);
        set_halt_until_mbus_tx();
    }

    // obs pad check
    else if(wakeup_data_header == 0x21){
        phsv1a_r13.sel_vana_obs1_fmbus_1p2=3;
        phsv1a_r13.sel_vana_obs2_fmbus_1p2=3;
        phsv1a_r13.sel_vana_obs3_fmbus_1p2=3;
        phsv1a_r13.sel_vana_obs4_fmbus_1p2=3;
        mbus_remote_register_write(PHS_ADDR, 0x13, phsv1a_r13.as_int);
        delay(MBUS_DELAY);

        phsv1a_r13.sel_test1_pad_fmbus_1p2=0;
        mbus_remote_register_write(PHS_ADDR, 0x13, phsv1a_r13.as_int);
        delay(MBUS_DELAY);
    }

    // oscillator check
    else if(wakeup_data_header == 0x22){
        // enable clk_obs    
        if (wakeup_data_field_2==0){ 
            phsv1a_r13.en_clk_osc_obs_fmbus_1p2=1;
            phsv1a_r13.sel_test1_pad_fmbus_1p2=2;
            mbus_remote_register_write(PHS_ADDR, 0x13, phsv1a_r13.as_int);
            delay(MBUS_DELAY);
        }
        // disable clk_obs    
        else if (wakeup_data_field_2==1){ 
            phsv1a_r13.en_clk_osc_obs_fmbus_1p2=0;
            phsv1a_r13.sel_test1_pad_fmbus_1p2=0;
            mbus_remote_register_write(PHS_ADDR, 0x13, phsv1a_r13.as_int);
            delay(MBUS_DELAY);
        }
    }

    // current source tuning
    // Use test1_pad, and it should be 3nA
    else if(wakeup_data_header == 0x23){
        phsv1a_r13.sel_test1_pad_fmbus_1p2=1;
        mbus_remote_register_write(PHS_ADDR, 0x13, phsv1a_r13.as_int);
        delay(MBUS_DELAY);

        phsv1a_r13.sel_vana_obs1_fmbus_1p2=0;
        phsv1a_r13.sel_vana_obs2_fmbus_1p2=0;
        phsv1a_r13.sel_vana_obs3_fmbus_1p2=0;
        phsv1a_r13.sel_vana_obs4_fmbus_1p2=0;
        mbus_remote_register_write(PHS_ADDR, 0x13, phsv1a_r13.as_int);
        delay(MBUS_DELAY);
    }

    else {
    }
    
////***********************************************************
//// test_num
//// 0 : LDO and CLK test
//// 1 : Calibration test
//// 2 : CVCC
//// 3 : Current tuning
//// 4 : OBS check
//// 5 : Reg change try
//    test_num=1;
////---------------------------
//// LDO and CLK test
////---------------------------
//    if (test_num==0){
//        // rstb
//        phsv1a_r17.rstb_fmbus_1p2=1;
//        mbus_remote_register_write(PHS_ADDR, 0x17, phsv1a_r17.as_int);
//        delay(500);
//
//        // turn on the LDO
//        phsv1a_r14.sel_avdd_ext_fmbus_1p2=0;
//        mbus_remote_register_write(PHS_ADDR, 0x14, phsv1a_r14.as_int);
//        delay(500);
//        
//        // iref
//        phsv1a_r10.ctrl_vbias100pa_fmbus_1p2=0xa;
//        mbus_remote_register_write(PHS_ADDR, 0x10, phsv1a_r10.as_int);
//        delay(500);
//
//        // enable osc
//        phsv1a_r11.osc_en_fmbus_1p2=1;
//        phsv1a_r11.sel_osc_freq_fmbus_1p2=1;
//        mbus_remote_register_write(PHS_ADDR, 0x11, phsv1a_r11.as_int);
//        delay(500);
//
//        // enable clk_obs    
//        phsv1a_r13.en_clk_osc_obs_fmbus_1p2=1;
//        phsv1a_r13.sel_test1_pad_fmbus_1p2=2;
//        mbus_remote_register_write(PHS_ADDR, 0x13, phsv1a_r13.as_int);
//        delay(500);
//            
//        
//        
//    }
//    else if (test_num==1){
//        // to check the r00 value
////        phsv1a_r00.en_adc_single_isfet_fmbus_1p2=1;
////        mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
////        delay(500);
//        
//
//
//        phs_basic_setup();
//        phs_source_drain_obs();
//
//        // set PRC reg for interruption
//        // 0x08 : phsv1a_r00, en_adc_single = 0
//        // 0x09 : phsv1a_r00, en_adc_single = 1
//        phs_prc_reg_write(0x08, 0x081020);
//        phs_prc_reg_write(0x09, 0x281020);
//
//        // control iref from the outside voltage    
//        phsv1a_r10.ovsel_vbias100pa_fmbus_1p2=1;
//        mbus_remote_register_write(PHS_ADDR, 0x10, phsv1a_r10.as_int);
//        delay(500);
//
//        // Calibration mode
//        phsv1a_r00.en_cal_isfet_fmbus_1p2=1;
//        mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
//        delay(500);
//        phsv1a_r0E.sw1_cvcc_isfet_fmbus_1p2=0;
//        phsv1a_r0E.sw2_cvcc_isfet_fmbus_1p2=1;
//        phsv1a_r0E.sw3_cvcc_isfet_fmbus_1p2=1;
//        phsv1a_r0E.sw4_cvcc_isfet_fmbus_1p2=0;
//        phsv1a_r0E.sw5_cvcc_isfet_fmbus_1p2=1;
//        phsv1a_r0E.sw6_cvcc_isfet_fmbus_1p2=0;
////        phsv1a_r0E.en_ccap_isfet_fmbus_1p2=1;
////        // Increase amplifier current
////        phsv1a_r0E.sel_amp_bias_con_isfet_fmbus_1p2=0x1f;
//        mbus_remote_register_write(PHS_ADDR, 0x0E, phsv1a_r0E.as_int);
//        delay(500);
//
//        // change Vcal
////        phsv1a_r12.sel_mux_vsource_cal_isfet_fmbus_1p2=10;
////        mbus_remote_register_write(PHS_ADDR, 0x12, phsv1a_r12.as_int);
////        delay(500);
//
//        // ADC single operation
//        phsv1a_r00.en_adc_single_isfet_fmbus_1p2=1;
//        mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
//
//        // read ADC output
//        mbus_write_message32(0xE1, *REG0);
//    
//            set_halt_until_mbus_trx();
//        mbus_copy_registers_from_remote_to_local(PHS_ADDR, 0x0a, 0x00, 0);
//            set_halt_until_mbus_tx();
//    
//        mbus_write_message32(0xE2, *REG0);
//        //---------------------------
//        
//        
//    }
//    else if (test_num==2){
//        phs_basic_setup();
//        phs_source_drain_obs();
//
//        
//        phsv1a_r0E.sw1_cvcc_isfet_fmbus_1p2=1;
//        phsv1a_r0E.sw2_cvcc_isfet_fmbus_1p2=0;
//        phsv1a_r0E.sw3_cvcc_isfet_fmbus_1p2=1;
//        phsv1a_r0E.sw4_cvcc_isfet_fmbus_1p2=0;
//        phsv1a_r0E.sw5_cvcc_isfet_fmbus_1p2=0;
//        phsv1a_r0E.sw6_cvcc_isfet_fmbus_1p2=1;
////        phsv1a_r0E.en_ccap_isfet_fmbus_1p2=1;
////        // Increase amplifier current
////        phsv1a_r0E.sel_amp_bias_con_isfet_fmbus_1p2=0x1f;
//        mbus_remote_register_write(PHS_ADDR, 0x0E, phsv1a_r0E.as_int);
//        delay(500);
//
//        // VDS code    
//        phsv1a_r03.dac_code_vds_operate_isfet_fmbus_1p2=0x100;
//        mbus_remote_register_write(PHS_ADDR, 0x03, phsv1a_r03.as_int);
//        delay(500);
//
//        // CVCC mode
//        phsv1a_r00.en_vds_isfet_fmbus_1p2=1;
//        phsv1a_r00.en_cal_isfet_fmbus_1p2=0;
//        mbus_remote_register_write(PHS_ADDR, 0x00, phsv1a_r00.as_int);
//        delay(500);
//    }
//    else if (test_num==3){
//        phs_basic_setup();
//        phsv1a_r13.sel_test1_pad_fmbus_1p2=1;
//        mbus_remote_register_write(PHS_ADDR, 0x13, phsv1a_r13.as_int);
//        delay(500);
//
//        phsv1a_r10.ovsel_vbias100pa_fmbus_1p2=1;
//        mbus_remote_register_write(PHS_ADDR, 0x10, phsv1a_r10.as_int);
//        delay(500);
//        
//    }
//    else if (test_num==4){
//        phsv1a_r13.sel_vana_obs1_fmbus_1p2=3;
//        phsv1a_r13.sel_vana_obs2_fmbus_1p2=3;
//        mbus_remote_register_write(PHS_ADDR, 0x13, phsv1a_r13.as_int);
//        delay(500);
//
//        phsv1a_r13.sel_test1_pad_fmbus_1p2=0;
//        mbus_remote_register_write(PHS_ADDR, 0x13, phsv1a_r13.as_int);
//        delay(500);
//    }
//    else if (test_num==5){
//        phs_basic_setup();
//        phs_ldo_turn_off();    
//        
//
////        mbus_write_message32(0xE1, *REG1);
////            set_halt_until_mbus_trx();
////        mbus_copy_registers_from_remote_to_local(PHS_ADDR, 0x14, 0x01, 0);
////            set_halt_until_mbus_tx();
////        mbus_write_message32(0xE2, *REG1);
//
//
//        // write local register
//
//        // PRC reg for interrupt
//        // REG 0x8 : LDO on
//        // REG 0x9 : LDO off
//        mbus_write_message32(0xE1, *REG0);
//        phs_prc_reg_write(0x08, 0x853080);
//        phs_prc_reg_write(0x09, 0x853880);
//        mbus_write_message32(0xE2, *REG0);
//
//    }
//    else {
//
//        //---------------------------
//        // Read from REG0x14 in PHS, write into REG0 in PRC
//        mbus_write_message32(0xE1, *REG0);
//            
//            set_halt_until_mbus_trx();
//        mbus_copy_registers_from_remote_to_local(PHS_ADDR, 0x14, 0x00, 0);
//            set_halt_until_mbus_tx();
//    
//        mbus_write_message32(0xE2, *REG0);
//        //---------------------------
//    
//        delay(500);
//    
//        // Change value of REG0x in PHS
//        mbus_remote_register_write(PHS_ADDR, 0x17, 0x1);
//        delay(MBUS_DELAY);
//        mbus_remote_register_write(PHS_ADDR, 0x14, 0x853080);
//    
//        //---------------------------
//        // Read from REG0x14 in PHS, write into REG1 in PRC
//        mbus_write_message32(0xE1, *REG1);
//    
//            set_halt_until_mbus_trx();
//        mbus_copy_registers_from_remote_to_local(PHS_ADDR, 0x14, 0x01, 0);
//            set_halt_until_mbus_tx();
//    
//        mbus_write_message32(0xE2, *REG1);
//        //---------------------------
//    }


//    set_wakeup_timer(5, 1, 1);
    mbus_sleep_all();

    while(1){  //Never Quit (should not come here.)
        asm("nop;"); 
    }

//    return 1;
}
