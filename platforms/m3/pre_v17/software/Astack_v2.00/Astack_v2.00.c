//*******************************************************************
//Author: ZhiYoong Foo
//Description: Activity Detection System. ADXL362
//*******************************************************************
#include "PREv17.h"
#include "MRRv3.h"
#include "ADXL362.h"
#include "mbus.h"

// AStack order PRE->MRR->PMU
#define MRR_ADDR    0x5
#define PMU_ADDR    0x6

// Global Defines
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms

// ADXL362 Defines
#define SPI_TIME 250

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t wkp_data;

volatile uint16_t wkp_prd_bookkeep;
volatile uint8_t read_data_batadc;
volatile uint8_t pmu_adc_3p0_val;
volatile uint8_t voltage_storage[400] = {0};
volatile uint16_t voltage_storage_ptr;
volatile uint16_t trigger_storage[200] = {0};
volatile uint16_t trigger_storage_ptr;
volatile bool saw_trigger;

volatile prev17_r0B_t prev17_r0B = PREv17_R0B_DEFAULT;

volatile mrrv3_r00_t mrrv3_r00 = MRRv3_R00_DEFAULT;
volatile mrrv3_r01_t mrrv3_r01 = MRRv3_R01_DEFAULT;
volatile mrrv3_r02_t mrrv3_r02 = MRRv3_R02_DEFAULT;
volatile mrrv3_r03_t mrrv3_r03 = MRRv3_R03_DEFAULT;
volatile mrrv3_r04_t mrrv3_r04 = MRRv3_R04_DEFAULT;
volatile mrrv3_r05_t mrrv3_r05 = MRRv3_R05_DEFAULT;
volatile mrrv3_r06_t mrrv3_r06 = MRRv3_R06_DEFAULT;
volatile mrrv3_r07_t mrrv3_r07 = MRRv3_R07_DEFAULT;
volatile mrrv3_r08_t mrrv3_r08 = MRRv3_R08_DEFAULT;
volatile mrrv3_r09_t mrrv3_r09 = MRRv3_R09_DEFAULT;
volatile mrrv3_r0A_t mrrv3_r0A = MRRv3_R0A_DEFAULT;
volatile mrrv3_r0B_t mrrv3_r0B = MRRv3_R0B_DEFAULT;
volatile mrrv3_r0C_t mrrv3_r0C = MRRv3_R0C_DEFAULT;
volatile mrrv3_r0D_t mrrv3_r0D = MRRv3_R0D_DEFAULT;
volatile mrrv3_r0E_t mrrv3_r0E = MRRv3_R0E_DEFAULT;
volatile mrrv3_r0F_t mrrv3_r0F = MRRv3_R0F_DEFAULT;
volatile mrrv3_r10_t mrrv3_r10 = MRRv3_R10_DEFAULT;
volatile mrrv3_r11_t mrrv3_r11 = MRRv3_R11_DEFAULT;
volatile mrrv3_r12_t mrrv3_r12 = MRRv3_R12_DEFAULT;
volatile mrrv3_r13_t mrrv3_r13 = MRRv3_R13_DEFAULT;
volatile mrrv3_r1B_t mrrv3_r1B = MRRv3_R1B_DEFAULT;
volatile mrrv3_r1C_t mrrv3_r1C = MRRv3_R1C_DEFAULT;

//*******************************************************************
// FUNCTION HEADER DECLERATIONS
//*******************************************************************

//***************************************************
// End of Program Sleep Functions
//***************************************************
static void operation_sleep(void);
static void operation_sleep_noirqreset(void);
static void operation_sleep_notimer(void);

//************************************
// PMU Functions
//************************************
static void pmu_set_adc_period(uint32_t);

static void pmu_set_clk_init(void);
static void pmu_reset_solar_short(void);
static void pmu_adc_active_disable(void);
static void pmu_adc_reset_setting(void);
static void pmu_adc_enable(void);
static void pmu_adc_disable(void);
static void pmu_adc_read_latest(void);
static void pmu_set_sar_override(uint8_t);
static void pmu_parkinglot_decision_3v_battery(void);
//***************************************************
// MRR Functions
//***************************************************
static void mrr_configure_pulse_width(void);
static void mrr_power_on();
static void mrr_power_off();
static void mrr_send_packet(void);
//***************************************************
// ADXL362 Functions
//***************************************************
static void ADXL362_reg_wr (uint8_t reg_id, uint8_t);
static void ADXL362_reg_rd (uint8_t reg_id);
static void ADXL362_init(void);
static void ADXL362_stop(void);
static void operation_spi_init(void);
static void operation_spi_stop(void);

//************************************
// Initialization Routine
//************************************
static void operation_init(void);

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
void handler_ext_int_gpio     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_spi      (void) __attribute__ ((interrupt ("IRQ")));

//[ 0] = GOC/EP
//[ 1] = Wakeuptimer
//[ 2] = XO timer
//[ 3] = gpio_pad
//[ 4] = mbus message
//[ 8] = gpio[0]
//[ 9] = gpio[1]
//[10] = gpio[2]
//[11] = gpio[3]
void handler_ext_int_wakeup   (void){
  *NVIC_ICPR = (0x1 << 0);
  //Woke up via GPIO
  if((*SREG_WAKEUP_SOURCE & 0x00000008) >> 3 & 0x1){
    saw_trigger = 1;
    operation_spi_init();
    ADXL362_reg_rd(ADXL362_STATUS);
    ADXL362_reg_rd(ADXL362_XDATA);
    ADXL362_reg_rd(ADXL362_YDATA);
    ADXL362_reg_rd(ADXL362_ZDATA);
    //Sensd "alert"
    mrrv3_r06.MRR_RAD_FSM_TX_DATA_0 = 0x657274;
    mbus_remote_register_write(MRR_ADDR,0x06,mrrv3_r06.as_int);
    mrrv3_r07.MRR_RAD_FSM_TX_DATA_1 = 0x00616C;
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv3_r07.as_int);
    mrr_power_on();
    mrr_send_packet();
    mrr_power_off();
    operation_spi_init();
    ADXL362_reg_rd(ADXL362_STATUS);
    operation_spi_stop();
  }
  //Woke up via Wakeup Timer
  if((*SREG_WAKEUP_SOURCE & 0x00000002) >> 1 & 0x1){
    //BookKeeping
    //Check Current PMU Voltage
    pmu_adc_read_latest();
    voltage_storage[voltage_storage_ptr] = read_data_batadc;
    voltage_storage_ptr++;
    //Make Decision
    pmu_parkinglot_decision_3v_battery();
    //Did we see any wakeups last?
    if (saw_trigger){
      saw_trigger = 0;
      trigger_storage[trigger_storage_ptr] = voltage_storage_ptr;
      trigger_storage_ptr++;
    }
  }
  set_wakeup_timer(wkp_prd_bookkeep, 0x1, 0x1);
  operation_sleep_noirqreset();
}
void handler_ext_int_softreset(void){ *NVIC_ICPR = (0x1 << 1);}
void handler_ext_int_gocep    (void){ *NVIC_ICPR = (0x1 << 2);}
void handler_ext_int_timer32  (void){ *NVIC_ICPR = (0x1 << 3);}
void handler_ext_int_timer16  (void){ *NVIC_ICPR = (0x1 << 4);}
void handler_ext_int_mbustx   (void){ *NVIC_ICPR = (0x1 << 5);}
void handler_ext_int_mbusrx   (void){ *NVIC_ICPR = (0x1 << 6);}
void handler_ext_int_mbusfwd  (void){ *NVIC_ICPR = (0x1 << 7);}
void handler_ext_int_reg0     (void){ *NVIC_ICPR = (0x1 << 8);}
void handler_ext_int_reg1     (void){ *NVIC_ICPR = (0x1 << 9);}
void handler_ext_int_reg2     (void){ *NVIC_ICPR = (0x1 << 10);}
void handler_ext_int_reg3     (void){ *NVIC_ICPR = (0x1 << 11);}
void handler_ext_int_reg4     (void){ *NVIC_ICPR = (0x1 << 12);}
void handler_ext_int_reg5     (void){ *NVIC_ICPR = (0x1 << 13);}
void handler_ext_int_reg6     (void){ *NVIC_ICPR = (0x1 << 14);}
void handler_ext_int_reg7     (void){ *NVIC_ICPR = (0x1 << 15);}
void handler_ext_int_mbusmem  (void){ *NVIC_ICPR = (0x1 << 16);}
void handler_ext_int_gpio     (void){ *NVIC_ICPR = (0x1 << 17);}
void handler_ext_int_spi      (void){ *NVIC_ICPR = (0x1 << 18);}

//***************************************************
// End of Program Sleep Functions
//***************************************************
static void operation_sleep(void){
  // Reset IRQ14VEC
  *IRQ14VEC = 0;
  mbus_sleep_all();
  while(1);
}

static void operation_sleep_noirqreset(void){
  mbus_sleep_all();
  while(1);
}

static void operation_sleep_notimer(void){
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

static void pmu_adc_read_latest(){
  // Grab latest PMU ADC readings
  // PMU register read is handled differently
  mbus_remote_register_write(PMU_ADDR,0x00,0x03);
  delay(MBUS_DELAY);
  read_data_batadc = *((volatile uint32_t *) REG0) & 0xFF;
  voltage_storage[voltage_storage_ptr] = read_data_batadc;
  voltage_storage_ptr++;
}

static void pmu_set_sar_override(uint8_t val){
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

static void pmu_parkinglot_decision_3v_battery(){
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

static void mrr_configure_pulse_width(){

  //Short
  mrrv3_r0F.MRR_RAD_FSM_TX_PW_LEN = 0; //4us PW
  mrrv3_r10.MRR_RAD_FSM_TX_C_LEN = 32; // (PW_LEN+1):C_LEN=1:32
  mrrv3_r0F.MRR_RAD_FSM_TX_PS_LEN = 0; // PW=PS
  mrrv3_r12.MRR_RAD_FSM_TX_HDR_CNST = 0; //no shift in LFSR

  //Long
  //mrrv3_r0F.MRR_RAD_FSM_TX_PW_LEN = 24; //100us PW
  //mrrv3_r10.MRR_RAD_FSM_TX_C_LEN = 800; // (PW_LEN+1):C_LEN=1:32
  //mrrv3_r0F.MRR_RAD_FSM_TX_PS_LEN = 24; // PW=PS   
  //mrrv3_r12.MRR_RAD_FSM_TX_HDR_CNST = 5; //8 bit shift in LFSR
  
  mbus_remote_register_write(MRR_ADDR,0x0F,mrrv3_r0F.as_int);
  mbus_remote_register_write(MRR_ADDR,0x12,mrrv3_r12.as_int);
  
  // Current Limter set-up 
  mrrv3_r00.MRR_CL_CTRL = 8;
  mbus_remote_register_write(MRR_ADDR,0x00,mrrv3_r00.as_int);
  
  mrrv3_r11.MRR_RAD_FSM_TX_POWERON_LEN = 7; //3bits
  mbus_remote_register_write(MRR_ADDR,0x11,mrrv3_r11.as_int);
}

static void mrr_power_on(){
  mrrv3_r00.MRR_CL_EN = 1;  //Enable CL
  mbus_remote_register_write(MRR_ADDR,0x00,mrrv3_r00.as_int);
  delay(MBUS_DELAY*10);
  
  mrrv3_r04.MRR_SCRO_EN_TIMER = 1;  //Power On TIMER
  mbus_remote_register_write(MRR_ADDR,0x04,mrrv3_r04.as_int);
  delay(MBUS_DELAY*300); //LDO stab 1s
  
  mrrv3_r04.MRR_SCRO_RSTN_TIMER = 1;  //Release Reset SCRO
  mbus_remote_register_write(MRR_ADDR,0x04,mrrv3_r04.as_int);
  delay(MBUS_DELAY*100); //freq stab 5s
  
  mrrv3_r04.MRR_SCRO_EN_CLK = 1;  //Enable SCRO
  mbus_remote_register_write(MRR_ADDR,0x04,mrrv3_r04.as_int);
  delay(MBUS_DELAY*100); //freq stab 5s
  
  mrrv3_r0E.MRR_RAD_FSM_SLEEP = 0; //Power On BB
  mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);
  delay(MBUS_DELAY*100); //freq stab 5s

  mrrv3_r0E.MRR_RAD_FSM_RSTN = 1;  //Release Reset BB
  mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);
  delay(MBUS_DELAY*10); //freq stab 5s

  mrrv3_r03.MRR_TRX_ISOLATEN = 1; //Set ISOLATEN 1, let state machine control
  mbus_remote_register_write(MRR_ADDR,0x03,mrrv3_r03.as_int);
  delay(MBUS_DELAY*10); //freq stab 5s
}

static void mrr_power_off(){
  mrrv3_r03.MRR_TRX_ISOLATEN = 0;  //set ISOLATEN 0
  mbus_remote_register_write(MRR_ADDR,0x03,mrrv3_r03.as_int);
  mrrv3_r0E.MRR_RAD_FSM_EN = 0; //Stop BB
  mrrv3_r0E.MRR_RAD_FSM_RSTN = 0; //RST BB
  mrrv3_r0E.MRR_RAD_FSM_SLEEP = 1; // Power off BB
  mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);
  mrrv3_r04.MRR_SCRO_EN_CLK = 0; // Disable clk
  mrrv3_r04.MRR_SCRO_RSTN_TIMER = 0; //Reset SCRO
  mrrv3_r04.MRR_SCRO_EN_TIMER = 0; //Power off SCRO
  mbus_remote_register_write(MRR_ADDR,0x04,mrrv3_r04.as_int);
  mrrv3_r00.MRR_CL_EN = 0;  //Disable CL
  mbus_remote_register_write(MRR_ADDR,0x00,mrrv3_r00.as_int);

}

static void mrr_send_packet(){
  
  mrrv3_r0E.MRR_RAD_FSM_EN = 1;  //Start BB
  mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);
  
  //WFI();
  
  mrrv3_r0E.MRR_RAD_FSM_EN = 0;  //Stop BB
  mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);
}

//***************************************************
// ADXL362 Functions
//***************************************************
static void ADXL362_reg_wr (uint8_t reg_id, uint8_t reg_data){
  gpio_write_data(0);
  *SPI_SSPDR = 0x0A;
  *SPI_SSPDR = reg_id;
  *SPI_SSPDR = reg_data;
  *SPI_SSPCR1 = 0x2;
  while(((*SPI_SSPSR>>4)&0x1) == 0x1){}//Spin
  gpio_write_data(1);
  *SPI_SSPCR1 = 0x0;
  uint32_t junk;
  junk = *SPI_SSPDR;
  junk = *SPI_SSPDR;
  junk = *SPI_SSPDR;
}

static void ADXL362_reg_rd (uint8_t reg_id){
  gpio_write_data(0);
  *SPI_SSPDR = 0x0B;
  *SPI_SSPDR = reg_id;
  *SPI_SSPDR = 0x00;
  *SPI_SSPCR1 = 0x2;
  while(((*SPI_SSPSR>>4)&0x1) == 0x1){}//Spin
  gpio_write_data(1);
  *SPI_SSPCR1 = 0x0;
  uint32_t junk;
  junk = *SPI_SSPDR;
  junk = *SPI_SSPDR;
  mbus_write_message32(0xB1,*SPI_SSPDR);
}

static void ADXL362_init(){
  // Soft Reset
  ADXL362_reg_wr(ADXL362_SOFT_RESET,0x52);
  
  // Check if Alive
  ADXL362_reg_rd(ADXL362_DEVID_AD);
  ADXL362_reg_rd(ADXL362_DEVID_MST);
  ADXL362_reg_rd(ADXL362_PARTID);
  ADXL362_reg_rd(ADXL362_REVID);

  // Set Interrupt1 Awake Bit [4]
  ADXL362_reg_wr(ADXL362_INTMAP1,0x10);

  // Set Activity Threshold
  ADXL362_reg_wr(ADXL362_THRESH_ACT_L,0x60);
  ADXL362_reg_wr(ADXL362_THRESH_ACT_H,0x00);

  // Set Actvity/Inactivity Control
  ADXL362_reg_wr(ADXL362_ACT_INACT_CTL,0x03);

  // Enter Measurement Mode
  ADXL362_reg_wr(ADXL362_POWER_CTL,0x0A);

  // Check Status (Clears first false positive)
  delay(5000);
  ADXL362_reg_rd(ADXL362_STATUS);
}

static void ADXL362_stop(){
  config_gpio_posedge_wirq(0x0);
  *NVIC_ISER = 0<<IRQ_WAKEUP;
  unfreeze_gpio_out();
  set_gpio_pad(3);
  gpio_set_dir(3);
  gpio_write_data(0);
  freeze_gpio_out();
}

static void operation_spi_init(){
  *SPI_SSPCR0 =  0x0207; // Motorola Mode
  *SPI_SSPCPSR = 0x02;   // Clock Prescale Register
  set_spi_pad(1);
  set_gpio_pad(3);
  gpio_set_dir(1);
  gpio_write_data(1);
  unfreeze_gpio_out();
  unfreeze_spi_out();
}

static void operation_spi_stop(){
  freeze_spi_out();
  freeze_gpio_out();
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
  set_halt_until_mbus_rx();
  //mbus_enumerate(MRR_ADDR);
  mbus_enumerate(PMU_ADDR);
  enumerated = 0xDEADBEEF;

  // PMU
  pmu_set_clk_init();
  pmu_reset_solar_short();
  pmu_adc_active_disable();
  pmu_adc_reset_setting();
  pmu_adc_enable();
  set_halt_until_mbus_tx();
  
  // MRR
  mrrv3_r1C.LC_CLK_RING = 0x3;  // ~ 150 kHz
  mrrv3_r1C.LC_CLK_DIV = 0x3;  // ~ 150 kHz
  mbus_remote_register_write(MRR_ADDR,0x1C,mrrv3_r1C.as_int);
  
  mrr_configure_pulse_width();

  // TX Setup Carrier Freq
  mrrv3_r00.MRR_TRX_CAP_ANTP_TUNE = 0x01FF;  //ANT CAP 14b unary 830.5 MHz
  mbus_remote_register_write(MRR_ADDR,0x00,mrrv3_r00.as_int);
  mrrv3_r01.MRR_TRX_CAP_ANTN_TUNE = 0x01FF; //ANT CAP 14b unary 830.5 MHz
  mbus_remote_register_write(MRR_ADDR,0x01,mrrv3_r01.as_int);
  mrrv3_r02.MRR_TX_BIAS_TUNE = 0x1FFF;  //Set TX BIAS TUNE 13b // Set to max
  mbus_remote_register_write(MRR_ADDR,0x02,mrrv3_r02.as_int);

  // RAD_FSM set-up 
  mrrv3_r0E.MRR_RAD_FSM_TX_H_LEN = 31; //31-31b header (max)
  mrrv3_r0E.MRR_RAD_FSM_TX_D_LEN = 40; //0-skip tx data
  mbus_remote_register_write(MRR_ADDR,0x0E,mrrv3_r0E.as_int);
  
  mrrv3_r10.MRR_RAD_FSM_SEED = 1; //default
  mrrv3_r10.MRR_RAD_FSM_TX_MODE = 3; //code rate 0:4 1:3 2:2 3:1(baseline) 4:1/2 5:1/3 6:1/4
  mbus_remote_register_write(MRR_ADDR,0x10,mrrv3_r10.as_int);
  
  // Mbus return address; Needs to be between 0x18-0x1F
  mbus_remote_register_write(MRR_ADDR,0x1B,0x1A00);

  mrrv3_r05.MRR_SCRO_R_SEL = 0x47F;
  mbus_remote_register_write(MRR_ADDR,0x05,mrrv3_r05.as_int);
  
  //Variable Initializations
  voltage_storage_ptr = 0;
  trigger_storage_ptr = 0;
  saw_trigger = 0;
  wkp_prd_bookkeep = 0;
  
  operation_sleep_notimer();
}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

  //disable_timerwd();

  // Initialization Routine
  if (enumerated != 0xDEADBEEF){
    operation_init();
  }
  
  set_halt_until_mbus_tx();

  wkp_data = *((volatile uint32_t *) IRQ14VEC);
  uint8_t wkp_data_hdr   = wkp_data >> 24;
  uint8_t wkp_data_fld_0 = wkp_data >>  0  & 0xFF;
  uint8_t wkp_data_fld_1 = wkp_data >>  8  & 0xFF;
  //uint8_t wkp_data_fld_2 = wkp_data >> 16 & 0xFF;

  //GOC Triggers

  //Operational GOC
  //0x01 --> 0x0F
  //0x01 --> Start Taking Measurements
  //0x02 --> Stop Taking Measurements
  // (Cannot be running programs)
  //0x03 --> Sends out all PMU Voltage Data
  //0x04 --> Sends out all Trigger Data
  //0x05 --> Resets all Data
  
  //Program Configs (Cannot be running programs)
  //0x10 --> 0x1F
  //0x10 --> Sets PMU_ADC_3P0

  //MRR Debugging (Cannot be running programs)
  //0x20 --> 0x2F
  //0x20 --> Change Carrier Frequency
  //0x21 --> Change BaseBand Frequency
  //0x22 --> Enable  MRR Continuous Tone
  //0x23 --> Disable MRR Continuous Tone
  
  if(wkp_data_hdr == 0x00){
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x01){
    mbus_write_message32(0xAA,wkp_data_hdr);
    // Initialize Interrupts
    *NVIC_ISER = 1<<IRQ_WAKEUP;
    //Take Configs
    wkp_prd_bookkeep = (wkp_data_fld_1 << 8 | wkp_data_fld_0);
    //Start Taking Measurements
    operation_spi_init();
    delay(100);
    *REG_CPS = 2;
    delay(200);
    ADXL362_init();
    delay(200);
    config_gpio_posedge_wirq(0x2);
    operation_spi_stop();
    set_wakeup_timer(wkp_prd_bookkeep, 0x1, 0x1);
    operation_sleep_noirqreset();
  }else if(wkp_data_hdr == 0x02){
    mbus_write_message32(0xAA,wkp_data_hdr);
    //Stop Taking Measurements
    ADXL362_stop();
    delay(1000);
    *REG_CPS = 0;
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x03){
    mbus_write_message32(0xAA,wkp_data_hdr);
    // Sends out all PMU Voltage Data
    uint16_t test_i;
    mrrv3_r06.MRR_RAD_FSM_TX_DATA_0 = (voltage_storage_ptr & 0x00FFFF);
    mbus_remote_register_write(MRR_ADDR,0x06,mrrv3_r06.as_int);
    mrrv3_r07.MRR_RAD_FSM_TX_DATA_1 = 0;
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv3_r07.as_int);
    mrr_power_on();
    mrr_send_packet();
    for(test_i=0; test_i<voltage_storage_ptr; test_i++){
      mrrv3_r06.MRR_RAD_FSM_TX_DATA_0 = (voltage_storage[test_i] & 0x00FF);
      mbus_remote_register_write(MRR_ADDR,0x06,mrrv3_r06.as_int);
      mrr_send_packet();
    }
    mrr_power_off();
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x04){
    mbus_write_message32(0xAA,wkp_data_hdr);
    // Sends out all Trigger Data
    uint32_t test_i;
    mrrv3_r06.MRR_RAD_FSM_TX_DATA_0 = (trigger_storage_ptr & 0x00FFFF);
    mbus_remote_register_write(MRR_ADDR,0x06,mrrv3_r06.as_int);
    mrrv3_r07.MRR_RAD_FSM_TX_DATA_1 = 0;
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv3_r07.as_int);
    mrr_power_on();
    mrr_send_packet();
    for(test_i=0; test_i<trigger_storage_ptr; test_i++){
      mrrv3_r06.MRR_RAD_FSM_TX_DATA_0 = (trigger_storage[test_i] & 0x00FFFF);
      mbus_remote_register_write(MRR_ADDR,0x06,mrrv3_r06.as_int);
      mrr_send_packet();
    }
    mrr_power_off();
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x05){
    // Resets all arrays
    mbus_write_message32(0xAA,wkp_data_hdr);
    voltage_storage_ptr = 0;
    trigger_storage_ptr = 0;
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x10){
    //Sets PMU_ADC_3P0
    mbus_write_message32(0xAA,wkp_data_hdr);
    if(wkp_data_fld_1 == 0x01){
      pmu_adc_3p0_val = wkp_data_fld_0;
    }else{
      //Check PMU Voltage
      pmu_adc_read_latest();
      pmu_adc_3p0_val = read_data_batadc;
    }
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x20){
    // Change Carrier Frequency
    mbus_write_message32(0xAA,wkp_data_hdr);
    mrrv3_r00.MRR_TRX_CAP_ANTP_TUNE = (wkp_data & 0x3FFF);
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv3_r00.as_int);
    mrrv3_r01.MRR_TRX_CAP_ANTN_TUNE = (wkp_data & 0x3FFF);
    mbus_remote_register_write(MRR_ADDR,0x01,mrrv3_r01.as_int);
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x21){
    // Change BaseBand Frequency
    mbus_write_message32(0xAA,wkp_data_hdr);
    mrrv3_r05.MRR_SCRO_R_SEL = (wkp_data & 0x7FF);
    mbus_remote_register_write(MRR_ADDR,0x05,mrrv3_r05.as_int);
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x22){
    // Turn On Continuous Tone
    mbus_write_message32(0xAA,wkp_data_hdr);
    disable_timerwd();
    mrrv3_r00.MRR_CL_CTRL = 1;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv3_r00.as_int);
    mrrv3_r02.MRR_TX_EN_OW = 0x1;
    mbus_remote_register_write(MRR_ADDR,0x02,mrrv3_r02.as_int);
    mrrv3_r03.MRR_TRX_ISOLATEN = 0;
    mrrv3_r03.MRR_DCP_S_OW = 1;
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv3_r03.as_int);
    mrrv3_r00.MRR_CL_EN = 1;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv3_r00.as_int);
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x23){
    // Turn Off Continuous Tone
    mbus_write_message32(0xAA,wkp_data_hdr);
    enable_timerwd();
    mrrv3_r00.MRR_CL_CTRL = 8;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv3_r00.as_int);
    mrrv3_r02.MRR_TX_EN_OW = 0x0;
    mbus_remote_register_write(MRR_ADDR,0x02,mrrv3_r02.as_int);
    mrrv3_r03.MRR_TRX_ISOLATEN = 1;
    mrrv3_r03.MRR_DCP_S_OW = 0;
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv3_r03.as_int);
    mrrv3_r00.MRR_CL_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv3_r00.as_int);
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x24){
    // Sends "debug"
    mbus_write_message32(0xAA,wkp_data_hdr);
    mrrv3_r06.MRR_RAD_FSM_TX_DATA_0 = 0x627567;
    mbus_remote_register_write(MRR_ADDR,0x06,mrrv3_r06.as_int);
    mrrv3_r07.MRR_RAD_FSM_TX_DATA_1 = 0x006465;
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv3_r07.as_int);
    mrr_power_on();
    mrr_send_packet();
    mrr_power_off();
    operation_sleep_notimer();
  }else{
    //Invalid GOC Header
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
