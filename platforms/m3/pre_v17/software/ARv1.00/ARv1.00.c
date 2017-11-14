//*******************************************************************
//Author: ZhiYoong Foo
//Description: Testbed for initial AR stacks 
//             Used for testing MRR Antenna
//             Used for testing MRR Caps
//             Only to be used for testing Ben's USRP Code
//             or
//             Finding CFO & SFO in Matlab
//*******************************************************************
#include "PREv17.h"
#include "MRRv5.h"
#include "mbus.h"

// AR order PREv17->SNSv10->MRRv5->PMUv7
#define SNS_ADDR    0x4
#define MRR_ADDR    0x5
#define PMU_ADDR    0x6

// Global Defines
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t test_var;
volatile uint32_t test_dat;
volatile uint32_t test_i;
volatile uint32_t wakeup_data ;
volatile uint32_t exec_cnt;
volatile uint32_t wfi_timeout_flag;

volatile uint32_t radio_ready;

volatile uint32_t read_data_batadc;
volatile uint32_t pmu_adc_3p0_val;

volatile prev17_r0B_t prev17_r0B = PREv17_R0B_DEFAULT;

volatile mrrv5_r00_t mrrv5_r00 = MRRv5_R00_DEFAULT;
volatile mrrv5_r01_t mrrv5_r01 = MRRv5_R01_DEFAULT;
volatile mrrv5_r02_t mrrv5_r02 = MRRv5_R02_DEFAULT;
volatile mrrv5_r03_t mrrv5_r03 = MRRv5_R03_DEFAULT;
volatile mrrv5_r04_t mrrv5_r04 = MRRv5_R04_DEFAULT;
volatile mrrv5_r05_t mrrv5_r05 = MRRv5_R05_DEFAULT;
volatile mrrv5_r06_t mrrv5_r06 = MRRv5_R06_DEFAULT;
volatile mrrv5_r07_t mrrv5_r07 = MRRv5_R07_DEFAULT;
volatile mrrv5_r08_t mrrv5_r08 = MRRv5_R08_DEFAULT;
volatile mrrv5_r09_t mrrv5_r09 = MRRv5_R09_DEFAULT;
volatile mrrv5_r0A_t mrrv5_r0A = MRRv5_R0A_DEFAULT;
volatile mrrv5_r0B_t mrrv5_r0B = MRRv5_R0B_DEFAULT;
volatile mrrv5_r0C_t mrrv5_r0C = MRRv5_R0C_DEFAULT;
volatile mrrv5_r0D_t mrrv5_r0D = MRRv5_R0D_DEFAULT;
volatile mrrv5_r0E_t mrrv5_r0E = MRRv5_R0E_DEFAULT;
volatile mrrv5_r0F_t mrrv5_r0F = MRRv5_R0F_DEFAULT;
volatile mrrv5_r10_t mrrv5_r10 = MRRv5_R10_DEFAULT;
volatile mrrv5_r11_t mrrv5_r11 = MRRv5_R11_DEFAULT;
volatile mrrv5_r12_t mrrv5_r12 = MRRv5_R12_DEFAULT;
volatile mrrv5_r13_t mrrv5_r13 = MRRv5_R13_DEFAULT;
volatile mrrv5_r1B_t mrrv5_r1B = MRRv5_R1B_DEFAULT;
volatile mrrv5_r1C_t mrrv5_r1C = MRRv5_R1C_DEFAULT;


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
    *SREG_WAKEUP_SOURCE = 0;
}

//***************************************************
// End of Program Sleep Functions
//***************************************************
static void operation_sleep(void){
	// Reset GOC_DATA_IRQ
	*GOC_DATA_IRQ = 0;

  mbus_sleep_all();
  while(1);
}

static void operation_sleep_noirqreset(void){
  mbus_sleep_all();
  while(1);
}

static void operation_sleep_notimer(void){
  //ldo_power_off();
  //if (radio_on){radio_power_off();}
  // Disable Timer
  set_wakeup_timer(0, 0, 0);
  operation_sleep();
}

//************************************
// PMU Functions
//************************************
static void pmu_set_adc_period(uint32_t val){
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

static void pmu_set_clk_init(){
  // Register 0x17: V3P6 Upconverter Sleep Settings
  mbus_remote_register_write(PMU_ADDR,0x17, 
							 ((  3 << 14) // Desired Vout/Vin ratio; defualt: 0
							  | (0 << 13) // Enable main feedback loop
							  | (1 <<  9) // Frequency multiplier R
							  | (2 <<  5) // Frequency multiplier L (actually L+1)
							  | (1 <<  0) // Floor frequency base (0-63)
							  ));
  // The first register write to PMU needs to be repeated
  mbus_remote_register_write(PMU_ADDR,0x17, 
							 ((  3 << 14) // Desired Vout/Vin ratio; defualt: 0
							  | (0 << 13) // Enable main feedback loop
							  | (1 <<  9) // Frequency multiplier R
							  | (2 <<  5) // Frequency multiplier L (actually L+1)
							  | (1 <<  0) // Floor frequency base (0-63)
							  ));
  // Register 0x18: V3P6 Upconverter Active Settings
  mbus_remote_register_write(PMU_ADDR,0x18, 
							 ((   3 << 14) // Desired Vout/Vin ratio; defualt: 0
							  | ( 0 << 13) // Enable main feedback loop
							  | ( 2 <<  9) // Frequency multiplier R
							  | ( 0 <<  5) // Frequency multiplier L (actually L+1)
							  | (16 <<  0) // Floor frequency base (0-63)
							  ));
  // Register 0x19: DOWNCONV_TRIM_V3_SLEEP
  mbus_remote_register_write(PMU_ADDR,0x19,
							 ((  0 << 13) // Enable main feedback loop
							  | (1 <<  9) // Frequency multiplier R
							  | (1 <<  5) // Frequency multiplier L (actually L+1)
							  | (1 <<  0) // Floor frequency base (0-63)
							  ));
  // Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
  mbus_remote_register_write(PMU_ADDR,0x1A,
							 ((   0 << 13) // Enable main feedback loop
							  | ( 4 <<  9) // Frequency multiplier R
							  | ( 0 <<  5) // Frequency multiplier L (actually L+1)
							  | (16 <<  0) // Floor frequency base (0-63)
							  ));
  // Register 0x15: V1P2 SAR_TRIM_v3_SLEEP
  mbus_remote_register_write(PMU_ADDR,0x15, 
							 ((  0 << 19) // Enable PFM even during periodic reset
							  | (0 << 18) // Enable PFM even when Vref is not used as ref
							  | (0 << 17) // Enable PFM
							  | (3 << 14) // Comparator clock division ratio
							  | (0 << 13) // Enable main feedback loop
							  | (1 <<  9) // Frequency multiplier R
							  | (2 <<  5) // Frequency multiplier L (actually L+1)
							  | (1 <<  1) // Floor frequency base (0-63)
							  ));
  // Register 0x16: V1P2 SAR_TRIM_v3_ACTIVE
  mbus_remote_register_write(PMU_ADDR,0x16, 
							 ((   0 << 19) // Enable PFM even during periodic reset
							  | ( 0 << 18) // Enable PFM even when Vref is not used as ref
							  | ( 0 << 17) // Enable PFM
							  | ( 3 << 14) // Comparator clock division ratio
							  | ( 0 << 13) // Enable main feedback loop
							  | ( 4 <<  9) // Frequency multiplier R
							  | ( 0 <<  5) // Frequency multiplier L (actually L+1)
							  | (16 <<  0) // Floor frequency base (0-63)
							  ));
  // SAR_RATIO_OVERRIDE
  // Use the new reset scheme in PMUv3
  mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
							 ((   0 << 13) // Enables override setting [12] (1'b1)
							  | ( 0 << 12) // Let VDD_CLK always connected to vbat
							  | ( 1 << 11) // Enable override setting [10] (1'h0)
							  | ( 0 << 10) // Have the converter have the periodic reset (1'h0)
							  | ( 0 <<  9) // Enable override setting [8] (1'h0)
							  | ( 0 <<  8) // Switch input / output power rails for upconversion (1'h0)
							  | ( 0 <<  7) // Enable override setting [6:0] (1'h0)
							  | (65 <<  0) // Binary converter's conversion ratio (7'h00)
							  ));
  mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
							 ((   1 << 13) // Enables override setting [12] (1'b1)
							  | ( 0 << 12) // Let VDD_CLK always connected to vbat
							  | ( 1 << 11) // Enable override setting [10] (1'h0)
							  | ( 0 << 10) // Have the converter have the periodic reset (1'h0)
							  | ( 1 <<  9) // Enable override setting [8] (1'h0)
							  | ( 0 <<  8) // Switch input / output power rails for upconversion (1'h0)
							  | ( 1 <<  7) // Enable override setting [6:0] (1'h0)
							  | (65 <<  0) // Binary converter's conversion ratio (7'h00)
							  ));
  pmu_set_adc_period(0x100); // 0x100 about 1 min for 1/2/1 1P2 setting
}

static void pmu_reset_solar_short(){
  mbus_remote_register_write(PMU_ADDR,0x0E, 
							 ((1 << 10)  // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
							  | (1 << 9) // Enables override setting [8]
							  | (0 << 8) // Turn on the harvester-inhibiting switch
							  | (1 << 4) // clamp_tune_bottom (increases clamp thresh)
							  | (0) 	 // clamp_tune_top (decreases clamp thresh)
							  ));
  mbus_remote_register_write(PMU_ADDR,0x0E, 
							 ((1 << 10)  // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
							  | (1 << 9) // Enables override setting [8]
							  | (0 << 8) // Turn on the harvester-inhibiting switch
							  | (1 << 4) // clamp_tune_bottom (increases clamp thresh)
							  | (0) 	 // clamp_tune_top (decreases clamp thresh)
							  ));
  mbus_remote_register_write(PMU_ADDR,0x0E, 
							 ((1 << 10)  // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
							  | (0 << 9) // Enables override setting [8]
							  | (0 << 8) // Turn on the harvester-inhibiting switch
							  | (1 << 4) // clamp_tune_bottom (increases clamp thresh)
							  | (0) 	 // clamp_tune_top (decreases clamp thresh)
							  ));
}

static void pmu_adc_active_disable(){
  // Disable PMU ADC measurement in active mode
  // PMU_CONTROLLER_STALL_ACTIVE
  mbus_remote_register_write(PMU_ADDR,0x3A, 
							 ((  1 << 19) // Ignore state_horizon; default 1
							  | (1 << 13) // Ignore adc_output_ready; default 0
							  | (1 << 12) // Ignore adc_output_ready; default 0
							  | (1 << 11) // Ignore adc_output_ready; default 0
							  ));

}

static void pmu_adc_reset_setting(){
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
}

static void pmu_adc_enable(){
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
}

static void pmu_adc_disable(){
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
}

inline static void pmu_adc_read_latest(){
  // Grab latest PMU ADC readings
  // PMU register read is handled differently
  mbus_remote_register_write(PMU_ADDR,0x00,0x03);
  delay(MBUS_DELAY);
  read_data_batadc = *((volatile uint32_t *) REG0) & 0xFF;
}

static void pmu_set_sar_override(uint32_t val){
  // SAR_RATIO_OVERRIDE
  mbus_remote_register_write(PMU_ADDR,0x05,
			     ((  0 << 13) // Enables override setting [12] (1'b1)
			      | (0 << 12) // Let VDD_CLK always connected to vbat
			      | (1 << 11) // Enable override setting [10] (1'h0)
			      | (0 << 10) // Have the converter have the periodic reset (1'h0)
			      | (1 << 9)  // Enable override setting [8] (1'h0)
			      | (0 << 8)  // Switch input / output power rails for upconversion (1'h0)
			      | (1 << 7)  // Enable override setting [6:0] (1'h0)
			      | (val)     // Binary converter's conversion ratio (7'h00)
			      ));
  mbus_remote_register_write(PMU_ADDR,0x05,
			     ((  1 << 13) // Enables override setting [12] (1'b1)
			      | (0 << 12) // Let VDD_CLK always connected to vbat
			      | (1 << 11) // Enable override setting [10] (1'h0)
			      | (0 << 10) // Have the converter have the periodic reset (1'h0)
			      | (1 << 9)  // Enable override setting [8] (1'h0)
			      | (0 << 8)  // Switch input / output power rails for upconversion (1'h0)
			      | (1 << 7)  // Enable override setting [6:0] (1'h0)
			      | (val)     // Binary converter's conversion ratio (7'h00)
			      ));
}

inline static void pmu_parkinglot_decision_3v_battery(){
  // Battery > 3.0V
  if      (read_data_batadc < pmu_adc_3p0_val     ) pmu_set_sar_override(0x3C);
  // Battery 2.9V - 3.0V
  else if (read_data_batadc < pmu_adc_3p0_val + 4 ) pmu_set_sar_override(0x3F);
  // Battery 2.8V - 2.9V
  else if (read_data_batadc < pmu_adc_3p0_val + 8 ) pmu_set_sar_override(0x41);
  // Battery 2.7V - 2.8V
  else if (read_data_batadc < pmu_adc_3p0_val + 12) pmu_set_sar_override(0x43);
  // Battery 2.6V - 2.7V
  else if (read_data_batadc < pmu_adc_3p0_val + 17) pmu_set_sar_override(0x45);
  // Battery 2.5V - 2.6V
  else if (read_data_batadc < pmu_adc_3p0_val + 21) pmu_set_sar_override(0x48);
  // Battery 2.4V - 2.5V
  else if (read_data_batadc < pmu_adc_3p0_val + 27) pmu_set_sar_override(0x4B);
  // Battery 2.3V - 2.4V
  else if (read_data_batadc < pmu_adc_3p0_val + 32) pmu_set_sar_override(0x4E);
  // Battery 2.2V - 2.3V
  else if (read_data_batadc < pmu_adc_3p0_val + 39) pmu_set_sar_override(0x51);
  // Battery 2.1V - 2.2V
  else if (read_data_batadc < pmu_adc_3p0_val + 46) pmu_set_sar_override(0x56);
  // Battery 2.0V - 2.1V
  else if (read_data_batadc < pmu_adc_3p0_val + 53) pmu_set_sar_override(0x5A);
  // Battery <= 2.0V
  else                                              pmu_set_sar_override(0x5F);
}

//***************************************************
// MRR Functions
//***************************************************
static void mrr_configure_pulse_width_short(){

  mrrv5_r0F.MRR_RAD_FSM_TX_PW_LEN = 0; //4us PW
  mrrv5_r10.MRR_RAD_FSM_TX_C_LEN = 32; // (PW_LEN+1):C_LEN=1:32
  mrrv5_r0F.MRR_RAD_FSM_TX_PS_LEN = 0; // PW=PS
  mrrv5_r12.MRR_RAD_FSM_TX_HDR_CNST = 0; //no shift in LFSR

  //mrrv5_r0F.MRR_RAD_FSM_TX_PW_LEN = 1; //8us PW
  //mrrv5_r10.MRR_RAD_FSM_TX_C_LEN = 64; // (PW_LEN+1):C_LEN=1:32
  //mrrv5_r0F.MRR_RAD_FSM_TX_PS_LEN = 1; // PW=PS

  mbus_remote_register_write(MRR_ADDR,0x0F,mrrv5_r0F.as_int);
  mbus_remote_register_write(MRR_ADDR,0x12,mrrv5_r12.as_int);

  // Current Limter set-up 
  mrrv5_r00.MRR_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
  mbus_remote_register_write(MRR_ADDR,0x00,mrrv5_r00.as_int);

  mrrv5_r11.MRR_RAD_FSM_TX_POWERON_LEN = 7; //3bits
  mbus_remote_register_write(MRR_ADDR,0x11,mrrv5_r11.as_int);
}

static void mrr_radio_power_on(){
  // Turn off PMU ADC
  pmu_adc_disable();
  
  mrrv5_r00.MRR_CL_EN = 1;  //Enable CL
  mbus_remote_register_write(MRR_ADDR,0x00,mrrv5_r00.as_int);
  
  mrrv5_r04.MRR_SCRO_EN_TIMER = 1;  //Power On TIMER
  mbus_remote_register_write(MRR_ADDR,0x04,mrrv5_r04.as_int);
  delay(MBUS_DELAY*300); //LDO stab 1s
  
  mrrv5_r04.MRR_SCRO_RSTN_TIMER = 1;  //Release Reset SCRO
  mbus_remote_register_write(MRR_ADDR,0x04,mrrv5_r04.as_int);
  delay(MBUS_DELAY*100); //freq stab 5s
  
  mrrv5_r04.MRR_SCRO_EN_CLK = 1;  //Enable SCRO
  mbus_remote_register_write(MRR_ADDR,0x04,mrrv5_r04.as_int);
  delay(MBUS_DELAY*100); //freq stab 5s
  
  mrrv5_r0E.MRR_RAD_FSM_SLEEP = 0; //Power On BB
  mbus_remote_register_write(MRR_ADDR,0x0E,mrrv5_r0E.as_int);
  delay(MBUS_DELAY*100);
  
  mrrv5_r0E.MRR_RAD_FSM_RSTN = 1;  //Release Reset BB
  mbus_remote_register_write(MRR_ADDR,0x0E,mrrv5_r0E.as_int);
  delay(MBUS_DELAY*10);
  
  mrrv5_r03.MRR_TRX_ISOLATEN = 1; //Set ISOLATEN 1, let state machine control
  mbus_remote_register_write(MRR_ADDR,0x03,mrrv5_r03.as_int);
  delay(MBUS_DELAY*10);
}

static void mrr_radio_power_off(){
  mrrv5_r03.MRR_TRX_ISOLATEN = 0;  //set ISOLATEN 0
  mbus_remote_register_write(MRR_ADDR,0x03,mrrv5_r03.as_int);
  mrrv5_r0E.MRR_RAD_FSM_EN = 0; //Stop BB
  mrrv5_r0E.MRR_RAD_FSM_RSTN = 0; //RST BB
  mrrv5_r0E.MRR_RAD_FSM_SLEEP = 1; // Power off BB
  mbus_remote_register_write(MRR_ADDR,0x0E,mrrv5_r0E.as_int);
  mrrv5_r04.MRR_SCRO_EN_CLK = 0; // Disable clk
  mrrv5_r04.MRR_SCRO_RSTN_TIMER = 0; //Reset SCRO
  mrrv5_r04.MRR_SCRO_EN_TIMER = 0; //Power off SCRO
  mbus_remote_register_write(MRR_ADDR,0x04,mrrv5_r04.as_int);
  mrrv5_r00.MRR_CL_EN = 0;  //Disable CL
  mbus_remote_register_write(MRR_ADDR,0x00,mrrv5_r00.as_int);

}
static void mrr_send_packet(){
  mrrv5_r0E.MRR_RAD_FSM_EN = 1;  //Start BB
  mbus_remote_register_write(MRR_ADDR,0x0E,mrrv5_r0E.as_int);
  
  WFI();
  
  mrrv5_r0E.MRR_RAD_FSM_EN = 0;  //Start BB
  mbus_remote_register_write(MRR_ADDR,0x0E,mrrv5_r0E.as_int);
}

//************************************
// Initialization Routine
//************************************
static void operation_init(void){

  // Set CPU & Mbus Clock Speeds
  prev17_r0B.CLK_GEN_RING           = 0x1; // Default 0x1
  prev17_r0B.CLK_GEN_DIV_MBC        = 0x1; // Default 0x1
  prev17_r0B.CLK_GEN_DIV_CORE       = 0x3; // Default 0x3
  *REG_CLKGEN_TUNE = prev17_r0B.as_int;
  
  // Enumerate
  //set_halt_until_mbus_rx();
  mbus_enumerate(SNS_ADDR);
	delay(MBUS_DELAY);
  mbus_enumerate(MRR_ADDR);
	delay(MBUS_DELAY);
  mbus_enumerate(PMU_ADDR);
	delay(MBUS_DELAY);

  // PMU
  pmu_set_clk_init();
  pmu_reset_solar_short();
  pmu_adc_active_disable();
  pmu_adc_reset_setting();
  pmu_adc_enable();
  //set_halt_until_mbus_tx();
  enumerated = 0xDEADBEEF;
  
  // MRR
  mrrv5_r1C.LC_CLK_RING = 0x3;  // ~ 150 kHz
  mrrv5_r1C.LC_CLK_DIV = 0x3;  // ~ 150 kHz
  mbus_remote_register_write(MRR_ADDR,0x1C,mrrv5_r1C.as_int);
  
  mrr_configure_pulse_width_short();
  //mrr_configure_pulse_width_long();

  // TX Setup Carrier Freq
  mrrv5_r00.MRR_TRX_CAP_ANTP_TUNE = 0x01FF;  //ANT CAP 14b unary 830.5 MHz
  //mrrv5_r00.MRR_TRX_CAP_ANTP_TUNE = 0x00FF;  //ANT CAP 14b unary 813.8 MHz
  //mrrv5_r00.MRR_TRX_CAP_ANTP_TUNE = 0x0FFF;  //ANT CAP 14b unary 805.5 MHz
  mbus_remote_register_write(MRR_ADDR,0x00,mrrv5_r00.as_int);
  mrrv5_r01.MRR_TRX_CAP_ANTN_TUNE = 0x01FF; //ANT CAP 14b unary 830.5 MHz
  //mrrv5_r01.MRR_TRX_CAP_ANTN_TUNE = 0x00FF; //ANT CAP 14b unary 813.8 MHz
  //mrrv5_r01.MRR_TRX_CAP_ANTN_TUNE = 0x0FFF;  //ANT CAP 14b unary 805.5 MHz
  mbus_remote_register_write(MRR_ADDR,0x01,mrrv5_r01.as_int);
  mrrv5_r02.MRR_TX_BIAS_TUNE = 0x1FFF;  //Set TX BIAS TUNE 13b // Set to max
  mbus_remote_register_write(MRR_ADDR,0x02,mrrv5_r02.as_int);

  // Forces decaps to be parallel
  //mrrv5_r03.MRR_DCP_S_OW = 0;  //TX_Decap S (forced charge decaps)
  //mrrv5_r03.MRR_DCP_P_OW = 1;  //RX_Decap P 
  //mbus_remote_register_write(MRR_ADDR,3,mrrv5_r03.as_int);
  
  // RX Setup
  //mrrv5_r03.MRR_RX_BIAS_TUNE    = 0x0AFF;//  turn on Q_enhancement
  mrrv5_r03.MRR_RX_BIAS_TUNE    = 0x0001;//  turn off Q_enhancement
  mrrv5_r03.MRR_RX_SAMPLE_CAP    = 0x1;  // RX_SAMPLE_CAP
  mbus_remote_register_write(MRR_ADDR,3,mrrv5_r03.as_int);
  
  mrrv5_r11.MRR_RAD_FSM_RX_POWERON_LEN = 0x7;  //Set RX Power on length
  //mrrv5_r11.MRR_RAD_FSM_RX_SAMPLE_LEN = 0x3;  //Set RX Sample length  16us
  mrrv5_r11.MRR_RAD_FSM_RX_SAMPLE_LEN = 0x0;  //Set RX Sample length  4us
  mrrv5_r11.MRR_RAD_FSM_GUARD_LEN = 0x000F; //Set TX_RX Guard length, TX_RX guard 32 cycle (28+5)
  mbus_remote_register_write(MRR_ADDR,0x11,mrrv5_r11.as_int);
  
  mrrv5_r12.MRR_RAD_FSM_RX_HDR_BITS = 0x00;  //Set RX header
  mrrv5_r12.MRR_RAD_FSM_RX_HDR_TH = 0x00;    //Set RX header threshold
  mrrv5_r12.MRR_RAD_FSM_RX_DATA_BITS = 0x03; //Set RX data 1b
  mbus_remote_register_write(MRR_ADDR,0x12,mrrv5_r12.as_int);
  
  mrrv5_r1B.MRR_IRQ_REPLY_PACKET = 0x061400; //Read RX data Reply
  mbus_remote_register_write(MRR_ADDR,0x1B,mrrv5_r1B.as_int);
  
  // RAD_FSM set-up 
  mrrv5_r0E.MRR_RAD_FSM_TX_H_LEN = 0; //31-31b header (max)
  mrrv5_r0E.MRR_RAD_FSM_TX_D_LEN = 128; //0-skip tx data
  mbus_remote_register_write(MRR_ADDR,0x0E,mrrv5_r0E.as_int);
  
  // Use pulse generator -- Not used currently
  mrrv5_r02.MRR_TX_PULSE_FINE = 0;
  mrrv5_r02.MRR_TX_PULSE_FINE_TUNE = 3;
  mbus_remote_register_write(MRR_ADDR,0x02,mrrv5_r02.as_int);
  
  mrrv5_r10.MRR_RAD_FSM_SEED = 1; //default
  mrrv5_r10.MRR_RAD_FSM_TX_MODE = 3; //code rate 0:4 1:3 2:2 3:1(baseline) 4:1/2 5:1/3 6:1/4
  mbus_remote_register_write(MRR_ADDR,0x10,mrrv5_r10.as_int);
  
  // Mbus return address; Needs to be between 0x18-0x1F
  mbus_remote_register_write(MRR_ADDR,0x1B,0x1002);

  // Lower BaseBand Frequency
  mrrv5_r05.MRR_SCRO_R_SEL = 0x47F;
  mbus_remote_register_write(MRR_ADDR,0x05,mrrv5_r05.as_int);

  *REG_GOC_TIMEOUT = 0xFFFFFF;
  
  exec_cnt = 0;
  test_i = 0;
  
  operation_sleep_notimer();
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    // Only enable relevant interrupts (PRCv17)
	//enable_reg_irq();
	//enable_all_irq();
	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);
  disable_timerwd();

  // Initialization Routine
  if (enumerated != 0xDEADBEEF){
	operation_init();
  }
  
  //set_halt_until_mbus_tx();

    wakeup_data = *GOC_DATA_IRQ;
  uint32_t wkp_data_hdr   = wakeup_data >> 24;
  uint32_t wkp_data_fld_0 = wakeup_data >>  0 & 0xFF;
  uint32_t wkp_data_fld_1 = wakeup_data >>  8 & 0xFF;
  uint32_t wkp_data_fld_2 = wakeup_data >> 16 & 0xFF;

  if(wkp_data_hdr == 0x00){
	operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x01){
    //Sets PMU_ADC_3P0
	mbus_write_message32(0xAA,wkp_data_hdr);
	//Check PMU Voltage
	pmu_adc_read_latest();
	pmu_adc_3p0_val = read_data_batadc;
	mbus_write_message32(0xA1,pmu_adc_3p0_val);
	operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x02){
	mbus_write_message32(0xAA,wkp_data_hdr);
	operation_sleep_notimer();

  }else if(wkp_data_hdr == 0x10){
	// Change Carrier Frequency
	mbus_write_message32(0xAA,wkp_data_hdr);
	mrrv5_r00.MRR_TRX_CAP_ANTP_TUNE = (wakeup_data & 0x3FFF);
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv5_r00.as_int);
	mrrv5_r01.MRR_TRX_CAP_ANTN_TUNE = (wakeup_data & 0x3FFF);
	mbus_remote_register_write(MRR_ADDR,0x01,mrrv5_r01.as_int);
	operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x11){
	// Change BaseBand Frequency
	mbus_write_message32(0xAA,wkp_data_hdr);
	mrrv5_r05.MRR_SCRO_R_SEL = (wakeup_data & 0x7FF);
	mbus_remote_register_write(MRR_ADDR,0x05,mrrv5_r05.as_int);
	operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x12){
	//Turn On Continuous Tone
	mbus_write_message32(0xAA,wkp_data_hdr);
	disable_timerwd();
	mrrv5_r00.MRR_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv5_r00.as_int);
	mrrv5_r02.MRR_TX_EN_OW = 0x1;
	mbus_remote_register_write(MRR_ADDR,0x02,mrrv5_r02.as_int);
	mrrv5_r03.MRR_TRX_ISOLATEN = 0;
	mrrv5_r03.MRR_DCP_S_OW = 1;
	mbus_remote_register_write(MRR_ADDR,0x3,mrrv5_r03.as_int);
	mrrv5_r00.MRR_CL_EN = 1;
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv5_r00.as_int);
	operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x13){
	//Turn Off Continuous Tone
	mbus_write_message32(0xAA,wkp_data_hdr);
	enable_timerwd();
	mrrv5_r00.MRR_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv5_r00.as_int);
	mrrv5_r02.MRR_TX_EN_OW = 0x0;
	mbus_remote_register_write(MRR_ADDR,0x02,mrrv5_r02.as_int);
	mrrv5_r03.MRR_TRX_ISOLATEN = 1;
	mrrv5_r03.MRR_DCP_S_OW = 0;
	mbus_remote_register_write(MRR_ADDR,0x3,mrrv5_r03.as_int);
	mrrv5_r00.MRR_CL_EN = 0;
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv5_r00.as_int);
	operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x14){
	//Turn on Continuous Packets (Sleep in between)
	mbus_write_message32(0xAA,wkp_data_hdr);
	mbus_write_message32(0xAA,test_i);
	if(test_i == 0){
	  // Sends "AELRT"
	  mrrv5_r06.MRR_RAD_FSM_TX_DATA_0 = 0x000000;
	  mbus_remote_register_write(MRR_ADDR,0x06,mrrv5_r06.as_int);
	  mrrv5_r07.MRR_RAD_FSM_TX_DATA_1 = 0x7AC800;
	  mbus_remote_register_write(MRR_ADDR,0x07,mrrv5_r07.as_int);
	  mrrv5_r08.MRR_RAD_FSM_TX_DATA_2 = (0x535430 | wkp_data_fld_2);
	  mbus_remote_register_write(MRR_ADDR,0x08,mrrv5_r08.as_int);
	  mrrv5_r09.MRR_RAD_FSM_TX_DATA_3 = 0x005445;
	  mbus_remote_register_write(MRR_ADDR,0x09,mrrv5_r09.as_int);
	}
	mrr_radio_power_on();
	mrr_send_packet();
	mrr_radio_power_off();
	test_i++;
	if ((test_i & 0x3) == 0){
	  //Every 16 transmits, check VBAT
	  pmu_adc_read_latest();
	  pmu_parkinglot_decision_3v_battery();
	}
	if(test_i < wkp_data_fld_0){
	  set_wakeup_timer(wkp_data_fld_1, 0x1, 0x1);
	  operation_sleep_noirqreset();
	}
	else{
	  mbus_write_message32(0xA2,test_i);
	  test_i = 0;
	  operation_sleep_notimer();
	}
  }else if(wkp_data_hdr == 0x15){
	//Turn on Continuous Packets (Sleep in between)
	mbus_write_message32(0xAA,wkp_data_hdr);
	mbus_write_message32(0xAA,test_i);
	if(test_i == 0){
	  mrrv5_r06.MRR_RAD_FSM_TX_DATA_0 = 0x000000;
	  mbus_remote_register_write(MRR_ADDR,0x06,mrrv5_r06.as_int);
	  mrrv5_r07.MRR_RAD_FSM_TX_DATA_1 = 0x000000;
	  mbus_remote_register_write(MRR_ADDR,0x07,mrrv5_r07.as_int);
	  mrrv5_r08.MRR_RAD_FSM_TX_DATA_2 = 0x000000;
	  mbus_remote_register_write(MRR_ADDR,0x08,mrrv5_r08.as_int);
	  mrrv5_r09.MRR_RAD_FSM_TX_DATA_3 = 0x000000;
	  mbus_remote_register_write(MRR_ADDR,0x09,mrrv5_r09.as_int);
	}
	mrr_radio_power_on();
	mrr_send_packet();
	mrr_radio_power_off();
	test_i++;
	if ((test_i & 0x3) == 0){
	  //Every 16 transmits, check VBAT
	  pmu_adc_read_latest();
	  pmu_parkinglot_decision_3v_battery();
	}
	if(test_i < wkp_data_fld_0){
	  set_wakeup_timer(wkp_data_fld_1, 0x1, 0x1);
	  operation_sleep_noirqreset();
	}
	else{
	  mbus_write_message32(0xA2,test_i);
	  test_i = 0;
	  operation_sleep_notimer();
	}
  }else if(wkp_data_hdr == 0x16){
	//Turn on Continuous Packets (No sleep in between)
	mbus_write_message32(0xAA,wkp_data_hdr);
	for(test_i = 0; test_i < wkp_data_fld_0; test_i++){
	  mrr_send_packet();
	}
	operation_sleep_notimer();
  }else{
	//Invalid GOC Header
	mbus_write_message32(0xAA,wkp_data_hdr);
	mbus_write_message32(0xAA,0xFF);
	operation_sleep_notimer();
  }

  while(1){  //Never Quit (should not come here.)
	mbus_write_message32(0xF0,0xDEADBEEF);
	operation_sleep_notimer();
	asm("nop;"); 
  }
  
  return 1;
}

