//*******************************************************************
//Author: ZhiYoong Foo
//Description: Light Detection System.
//*******************************************************************
#include "PREv14.h"
#include "HRVv5.h"
#include "mbus.h"

// AStack order PRE->HRV->PMU
#define HRV_ADDR    0x5
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
volatile uint32_t wakeup_data_header ;
volatile uint32_t wakeup_data ;
volatile uint32_t last_data;

volatile uint32_t radio_ready;

volatile prev14_r0B_t prev14_r0B = PREv14_R0B_DEFAULT;

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_0(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_4(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_5(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_6(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_7(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_8(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_9(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_12(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_13(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_14(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_15(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0 (void) { *NVIC_ICPR = (0x1 << 0);  } // TIMER32
void handler_ext_int_1 (void) { *NVIC_ICPR = (0x1 << 1);  } // TIMER16
void handler_ext_int_2 (void) { *NVIC_ICPR = (0x1 << 2);  } // REG0
void handler_ext_int_3 (void) { *NVIC_ICPR = (0x1 << 3);  } // REG1
void handler_ext_int_4 (void) { *NVIC_ICPR = (0x1 << 4);  } // REG2
void handler_ext_int_5 (void) { *NVIC_ICPR = (0x1 << 5);  } // REG3
void handler_ext_int_6 (void) { *NVIC_ICPR = (0x1 << 6);  } // REG4
void handler_ext_int_7 (void) { *NVIC_ICPR = (0x1 << 7);  } // REG5
void handler_ext_int_8 (void) { *NVIC_ICPR = (0x1 << 8);  } // REG6
void handler_ext_int_9 (void) { *NVIC_ICPR = (0x1 << 9);  } // REG7
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); } // MEM WR
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); } // MBUS_RX
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); } // MBUS_TX
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); } // MBUS_FWD
void handler_ext_int_14(void) { *NVIC_ICPR = (0x1 << 14); } // SPI
void handler_ext_int_15(void) { *NVIC_ICPR = (0x1 << 15); } // GPIO

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

//***************************************************
// HRV Functions
//***************************************************
static void hrv_light_reset(void){
  mbus_remote_register_write(HRV_ADDR,0x01,
			     ( (0  << 5)  // HRV_CNT_CNT_IRQ (default: 0)
			       | (0  << 4)  // HRV_CNT_CNT_ENABLE (default: 0)
			       | (2  << 2)  // LC_CLK_DIV (default: 2)
			       | (1)        // LC_CLK_RING (default: 1)
			       ));
}
	
static void hrv_light_start(void){
  mbus_remote_register_write(HRV_ADDR,0x01,
			     ( (0  << 5)  // HRV_CNT_CNT_IRQ (default: 0)
			       | (1  << 4)  // HRV_CNT_CNT_ENABLE (default: 0)
			       | (2  << 2)  // LC_CLK_DIV (default: 2)
			       | (1)        // LC_CLK_RING (default: 1)
			       ));
}

//************************************
// Initialization Routine
//************************************

static void operation_init(void){
  prev14_r0B_t prev14_r0B_temp;
  
  // Set CPU & Mbus Clock Speeds
  prev14_r0B_temp.as_int = prev14_r0B.as_int;
  prev14_r0B_temp.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
  prev14_r0B_temp.CLK_GEN_RING           = 0x1; // Default 0x1
  prev14_r0B_temp.CLK_GEN_DIV_MBC        = 0x1; // Default 0x1
  prev14_r0B_temp.CLK_GEN_DIV_CORE       = 0x3; // Default 0x3
  prev14_r0B.as_int = prev14_r0B_temp.as_int;
  *REG_CLKGEN_TUNE = prev14_r0B.as_int;
  
  // Enumerate
  set_halt_until_mbus_rx();
  mbus_enumerate(HRV_ADDR);
  //delay(1000);
  mbus_enumerate(PMU_ADDR);

  // PMU
  pmu_set_clk_init();
  pmu_reset_solar_short();
  pmu_adc_active_disable();
  pmu_adc_reset_setting();
  pmu_adc_enable();
  set_halt_until_mbus_tx();
  enumerated = 0xDEADBEEF;
  
  // HRV
  mbus_remote_register_write(HRV_ADDR,0x00,6); // HRV_TOP_CONV_RATIO(0~15 >> 9x~23x); default: 14
  mbus_remote_register_write(HRV_ADDR,0x01,
  			     ( (0  << 5)  // HRV_CNT_CNT_IRQ (default: 0)
  			       | (0  << 4)  // HRV_CNT_CNT_ENABLE (default: 0)
  			       | (2  << 2)  // LC_CLK_DIV (default: 2)
  			       | (1)        // LC_CLK_RING (default: 1)
  			       ));
  mbus_remote_register_write(HRV_ADDR,0x03,0xFFFFFF); // HRV_CNT_CNT_THRESHOLD (default: 0xFFFFFF) 
  mbus_remote_register_write(HRV_ADDR,0x04,0x001000); // HRV_CNT_IRQ_PACKET (default: 0x001400) 
  hrv_light_reset();

  last_data = 0;
  
  operation_sleep_notimer();
}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

  // Initialize Interrupts
  clear_all_pend_irq();
  //enable_all_irq();
  
  //disable_timerwd();

  // Initialization Routine
  if (enumerated != 0xDEADBEEF){
    operation_init();
  }
  
  set_halt_until_mbus_tx();

  wakeup_data = *((volatile uint32_t *) IRQ14VEC);
  uint32_t wkp_data_hdr   = wakeup_data >> 24;
  uint32_t wkp_data_fld_0 = wakeup_data >>  0  & 0xFF;
  uint32_t wkp_data_fld_1 = wakeup_data >>  8  & 0xFF;
  //uint32_t wkp_data_fld_2 = wakeup_data >> 16 & 0xFF;
  uint32_t mbus_read_reg = 0;

  if(wkp_data_hdr == 0x00){
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x01){
    // Start Light Harvesting
    mbus_write_message32(0xAA,0x01);
    hrv_light_start();
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x02){
    // Stop Light Harvesting
    mbus_write_message32(0xAA,0x02);
    hrv_light_reset();
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x03){
    mbus_write_message32(0xAA,0x03);
    set_halt_until_mbus_rx();
    mbus_remote_register_read(HRV_ADDR,0x02,1);
    mbus_read_reg = *((volatile uint32_t *) REG1);
    set_halt_until_mbus_tx();
    mbus_write_message32(0xBB,(mbus_read_reg-last_data));
    last_data = mbus_read_reg;
    operation_sleep_notimer();
  }else if(wkp_data_hdr == 0x04){
    mbus_write_message32(0xAA,0x04);
    set_wakeup_timer(0x2, 0x1, 0x1);
    operation_sleep_noirqreset();
  }else if (wkp_data_hdr == 0x05){
    mbus_write_message32(0xAA,0x05);
    set_wakeup_timer(0x2, 0x1, 0x1);
    operation_sleep_noirqreset();
  }else if (wkp_data_hdr == 0x10){
    mbus_write_message32(0xAA,0x10);
    set_halt_until_mbus_rx();
    mbus_remote_register_read(HRV_ADDR,0x02,1);
    mbus_read_reg = *((volatile uint32_t *) REG1);
    set_halt_until_mbus_tx();
    mbus_write_message32(0xAA,mbus_read_reg);
    operation_sleep_notimer();
  }else if (wkp_data_hdr == 0x11){
    mbus_write_message32(0xAA,0x11);
    set_halt_until_mbus_rx();
    mbus_remote_register_read(HRV_ADDR,0x01,1);
    mbus_read_reg = *((volatile uint32_t *) REG1);
    set_halt_until_mbus_tx();
    mbus_write_message32(0xAA,mbus_read_reg);
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

