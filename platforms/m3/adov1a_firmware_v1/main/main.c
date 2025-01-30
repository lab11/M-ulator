//*******************************************************************
// Starter code for Arete Final Phase 1
// V1
//*******************************************************************

#include "PREv23E.h"
#include "PREv23E_RF.h"
#include "PMUv13r1_RF.h"
#include "FLPv3S.h"
#include "FLPv3S_RF.h"
#include "ADOv1A_RF.h"
#include "MRMv1S_RF.h"

#include "mbus.h"
#define ENUMID 0xDEADBEEF

#include "DFT_LUT.txt"
#include "DCT_PHS.txt"

////////////////////////////////////////
// uncomment this for debug 
#define DEBUG_MODE          // Test VDD ON in ULP
#define DEBUG_MBUS_MSG      // Debug MBus message
/////////////////////////////////////////


#define ADO_ADDR 0x2
#define MRM1_ADDR 0x6
#define MRM2_ADDR 0x7
#define FLP1_ADDR 0x8
#define PMU_ADDR 0x9

// System parameters
#define MBUS_DELAY 100 // Amount of delay between successive messages; 100: ~7ms (MOD Audio Debug Stack)

// XO Initialization Wait Duration
#define XO_WAIT_A  50000    // Must be ~1 second delay. Delay for XO Start-Up. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
#define XO_WAIT_B  50000    // Must be ~1 second delay. Delay for VLDO & IBIAS Generation. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).

// CP parameter
#define CP_DELAY 50000 // Amount of delay for ADO charge pump charging-up; ~4.5s

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

#define EP_MODE     ((volatile uint32_t *) 0xA0000028)

#define TEMP_STORAGE_SIZE 8192 // MEMv1: 16kB, 8k 2-byte data

// Flash access parameters
#define _SIZE_TEMP_ 64
#define _LOG2_TEMP_ 6   // = log2(_SIZE_TEMP_)
#define _ITER_TEMP_ 32  // = 2048 / _SIZE_TEMP_ 

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
//volatile uint32_t irq_history;
uint32_t temp_storage[_SIZE_TEMP_];
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t error_code;
volatile uint32_t finst;
volatile uint32_t pmu_sar_conv_ratio_val_test_on;
volatile uint32_t pmu_sar_conv_ratio_val_test_off;
volatile uint32_t read_data_batadc;
volatile uint32_t read_data_batadc_diff;
volatile uint32_t mem_read_data[2];

volatile prev23e_r0B_t prev23e_r0B = PREv23E_R0B_DEFAULT;
volatile prev23e_r19_t prev23e_r19 = PREv23E_R19_DEFAULT;
volatile prev23e_r1A_t prev23e_r1A = PREv23E_R1A_DEFAULT;
volatile prev23e_r1B_t prev23e_r1B = PREv23E_R1B_DEFAULT;
volatile prev23e_r1C_t prev23e_r1C = PREv23E_R1C_DEFAULT;
volatile prev23e_r1F_t prev23e_r1F = PREv23E_R1F_DEFAULT;

volatile adov1a_r00_t adov1a_r00 = ADOv1A_R00_DEFAULT;
volatile adov1a_r01_t adov1a_r01 = ADOv1A_R01_DEFAULT;
volatile adov1a_r02_t adov1a_r02 = ADOv1A_R02_DEFAULT;
volatile adov1a_r03_t adov1a_r03 = ADOv1A_R03_DEFAULT;
volatile adov1a_r04_t adov1a_r04 = ADOv1A_R04_DEFAULT;
volatile adov1a_r05_t adov1a_r05 = ADOv1A_R05_DEFAULT;
volatile adov1a_r06_t adov1a_r06 = ADOv1A_R06_DEFAULT;
volatile adov1a_r07_t adov1a_r07 = ADOv1A_R07_DEFAULT;
volatile adov1a_r08_t adov1a_r08 = ADOv1A_R08_DEFAULT;
volatile adov1a_r09_t adov1a_r09 = ADOv1A_R09_DEFAULT;
volatile adov1a_r0A_t adov1a_r0A = ADOv1A_R0A_DEFAULT;
volatile adov1a_r0B_t adov1a_r0B = ADOv1A_R0B_DEFAULT;
volatile adov1a_r0C_t adov1a_r0C = ADOv1A_R0C_DEFAULT;
volatile adov1a_r0E_t adov1a_r0E = ADOv1A_R0E_DEFAULT;
volatile flpv3s_r05_t flpv3s_r05 = FLPv3S_R05_DEFAULT;

uint32_t read_data[128];

static void operation_sleep_notimer(void);
//static void operation_sleep_timer(void);
static void pmu_set_sar_override(uint32_t val);
static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);
static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);
static void pmu_set_active_3p6(uint8_t r, uint8_t l, uint8_t base);

volatile uint32_t num_total_16b_packet;
volatile uint32_t TEMP_CALIB_A;
volatile uint32_t TEMP_CALIB_B;
volatile uint32_t NUM_MEAS_USER;


//*******************************************************************
// XO Functions
//*******************************************************************
static void xo_start(void) {


    prev23e_r19.XO_RESETn       = 1;
    prev23e_r19.XO_PGb_START_UP = 1;
    *REG_XO_CONF1 = prev23e_r19.as_int;

	delay(XO_WAIT_A); // Delay A (~1s; XO Start-Up)

    prev23e_r19.XO_ISOL_CLK_HP = 0;
    *REG_XO_CONF1 = prev23e_r19.as_int;
    
    delay(XO_WAIT_B); // Delay B (~1s; VLDO & IBIAS generation)

    prev23e_r19.XO_ISOL_CLK_LP = 0;
    *REG_XO_CONF1 = prev23e_r19.as_int;
    
    delay(100); // Delay C (~1ms; SCN Output Generation)
	
 // ======================================
    prev23e_r19.XO_START_UP = 0;
    *REG_XO_CONF1 = prev23e_r19.as_int;

    prev23e_r19.XO_ISOL_CLK_HP = 1;
    *REG_XO_CONF1 = prev23e_r19.as_int;

    prev23e_r19.XO_PGb_START_UP = 0;
    *REG_XO_CONF1 = prev23e_r19.as_int;

    delay(100); // Dummy Delay

	
    enable_xo_timer();
    start_xo_cout(); // turns on XO clk output pad
}

static void xo_stop( void ) {
    // Stop the XO Driver
    prev23e_r19.XO_ISOL_CLK_LP = 1;
    *REG_XO_CONF1 = prev23e_r19.as_int;

    prev23e_r19.XO_RESETn   = 0;
    prev23e_r19.XO_START_UP = 1;
    *REG_XO_CONF1 = prev23e_r19.as_int;
}

static void XO_init(void) {
    //prev23e_r19.XO_SEL_VLDO = 0x0;	// Default value: 3'h0
    prev23e_r19.XO_CAP_TUNE = 0x3;	// Default value: 3'h0
    //prev22e_r19.XO_SEL_vV0P6 = 0x0;	// Default value: 2'h2
	prev23e_r19.XO_INJ = 0x0; 		// Default value: 2'h2
    *REG_XO_CONF1 = prev23e_r19.as_int;

    prev23e_r1A.XO_SEL_CLK_OUT_DIV = 0x4;
    prev23e_r1A.XO_SEL_DLY = 0x1; 	// Default value: 3'h1

    *REG_XO_CONF2 = prev23e_r1A.as_int;

    xo_start();
    

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

inline static void pmu_set_active_3p6(uint8_t r, uint8_t l, uint8_t base){
	
    // The first register write to PMU needs to be repeated
    // Register 0x16: V1P2 Active
	mbus_remote_register_write(PMU_ADDR,0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
			| (0 << 13) // Enable main feedback loop
			| (r << 9)  // Frequency multiplier R
			| (l << 5)  // Frequency multiplier L (actually L+1)
			| (base)      // Floor frequency base (0-63)
		));
	delay(MBUS_DELAY);
	mbus_remote_register_write(PMU_ADDR,0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
			| (0 << 13) // Enable main feedback loop
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

inline static void pmu_set_clk_init(){
	
    pmu_set_sleep_clk(0x6,0x4,0x4,0x8); //HP XO
    pmu_set_active_clk(0x9,0x6,0x10,0x6); //HP XO	
	
/*     pmu_set_sleep_clk(0x3,0x2,0x03,0x7); 
   
	pmu_set_active_clk(0x9,0x8,0x0D,0x8);	 // MOD settings */
	
/*     pmu_set_sleep_clk(0x3,0x2,0x04,0x7);    
	pmu_set_active_clk(0x9,0x8,0x0F,0x8); */		

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
		//pmu_set_sar_override(0x45); //good for 3V battery
		pmu_set_sar_override(pmu_sar_conv_ratio_val_test_on);
		
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
			| (1 << 19) //state_state_horizon
		));
		delay(MBUS_DELAY);
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

static void FLASH_initialization (uint16_t flash_num) { //initializes all flash instances in range
    uint16_t i;
	for(i=0; i<flash_num; i++){
	// Tune Flash
		mbus_remote_register_write (FLP1_ADDR + i , 0x00 , 0xF84001); // T5us, T10us setting for ~100kHz CLKmain
		mbus_remote_register_write (FLP1_ADDR + i , 0x01 , 0x00AF01); // Tcyc_prog, Tprog setting for ~100kHz CLKmain
		mbus_remote_register_write (FLP1_ADDR + i , 0x26 , 0x0D7788); // Program Current: Default
		mbus_remote_register_write (FLP1_ADDR + i , 0x27 , 0x011BC8); // Erase Pump Diode Chain: Default
		mbus_remote_register_write (FLP1_ADDR + i , 0x01 , 0x007F09); // Tprog idle time: Default
		mbus_remote_register_write (FLP1_ADDR + i , 0x19 , 0x000F86); // Voltage Clamper Tuning: 1STG-0xF, 2STG-max, VOUT-0x6
		mbus_remote_register_write (FLP1_ADDR + i , 0x0F , 0x001001); // Flash interrupt target register addr: REG0 -> REG1
		//mbus_remote_register_write (FLP1_ADDR + i , 0x12 , 0x000003); // Auto Power On/Off
		mbus_remote_register_write (FLP1_ADDR + i , 0x18 , 0x00002F); // CLK_RING_SEL: 4'hF, CLK_DIV_SEL: 2'h2 -> ~200kHz
	}
}

static void FLASH_turn_on(uint16_t flash_inst) { //turn on only the selected flash

/* 	flpv3s_r05.Tcap = 0xFFFFF;
	mbus_remote_register_write(FLP1_ADDR + flash_inst, 0x05, flpv3s_r05.as_int);
	delay(MBUS_DELAY*10);	 */
	
	set_halt_until_mbus_rx();
	mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x11 , 0x00002F);
	set_halt_until_mbus_tx();
	
	if (*REG1 != 0xB5) flp_fail (0);
}

static void FLASH_turn_off(uint16_t flash_inst) { //turn off only the selected flash
    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x11 , 0x00002D);
    set_halt_until_mbus_tx();
	delay(1000);
    if (*REG1 != 0xBB) flp_fail (1);
}

static void FLASH_pp_ready (uint16_t flash_inst) { //prep ping pong for specified flash instance
    
    // Ping Pong Setting
	mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x13 , 0x000001); // Enable Ping Pong w/ No length limit
	//mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x14 , 0x00001B); // Over run err no detection & fast programming & no wrapping & enable DATA0 and DATA1
	mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x14 , 0x000019); // Over run err no detection & fast programming & no wrapping & enable DATA0 only	
	mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x15 , 0x000000); // Flash program addr started from 0x0
}

static void FLASH_pp_on (uint16_t flash_inst) { //turn on ping pong for specified flash instance
    mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x09, 0x00002D); // Ping Pong Go
}

static void FLASH_pp_off (uint16_t flash_inst) { //turn on ping pong for specified flash instance
    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x13, 0x000000); // Ping Pong End
    set_halt_until_mbus_tx();
	//delay(1000);
}
	
static void FLASH_read (uint16_t banks) { //read flash out in 8k word banks to mbus; 32 banks per flash instance
    uint32_t i;
    uint32_t j;
	uint32_t m;
	uint32_t f_inst=0;
	uint32_t first;
    uint32_t flp_sram_addr;

	for(m=0; m<banks; m++) {
		f_inst = m / 32;
		first = !(m % 32); //indicates first bank in a flash instance
		if(first) {
			if(f_inst > 0) { //moving to next flash instance - turn off prev inst
				FLASH_turn_off(f_inst - 1);
			}
			FLASH_turn_on(f_inst); //turn on the flash instance
		}

		mbus_remote_register_write (FLP1_ADDR + f_inst, 0x08 , m*2048); // Set FLSH_START_ADDR
		mbus_remote_register_write (FLP1_ADDR + f_inst, 0x07 , 0x0); // Set SRAM_START_ADDR
	
		prev23e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x0; //disable watchdog for long access
		*REG_MBUS_WD = prev23e_r1F.as_int;
	
		set_halt_until_mbus_rx();
		mbus_remote_register_write (FLP1_ADDR + f_inst, 0x09 , 0x07FFE3); // Flash -> SRAM
		set_halt_until_mbus_tx();
	
		if (*REG1 != 0x00002B) flp_fail(4);
		
		prev23e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x16E360; //set watchdog back to default
		*REG_MBUS_WD = prev23e_r1F.as_int;
		
		flp_sram_addr = 0;
		
		for(j=0; j<16; j++){
			set_halt_until_mbus_rx();
			mbus_copy_mem_from_remote_to_any_bulk (FLP1_ADDR + f_inst, (uint32_t *) flp_sram_addr, 0x1, read_data, 127);
			set_halt_until_mbus_tx();
			delay(100);
			for (i=0; i<128; i++) {
				mbus_write_message32(0xC0, read_data[i]);
			}
			flp_sram_addr = flp_sram_addr + 512;
			mbus_write_message32(0xDD, j);
		}
	}
	FLASH_turn_off(f_inst);
}

//***************************************************
//  FLP functions for flash testing only
//***************************************************

inline static void flash_erasedata(uint16_t flash_inst){ //erase specified flash instance
	
	FLASH_turn_on(flash_inst);
	
	// Erase Flash
	uint32_t page_start_addr = 0;
	uint32_t idx;
	
	for (idx=0; idx<128; idx++){ // All Pages (1Mb) are erased.
		page_start_addr = (idx << 8);
		
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x08, page_start_addr); // Set FLSH_START_ADDR
		
		set_halt_until_mbus_rx();
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x09, 0x000029); // Page Erase
		set_halt_until_mbus_tx();
		
		if (*REG1 != 0x00004F) flp_fail((0xFFFF << 16) | idx);
	}	
	
	FLASH_turn_off(flash_inst);
	 
}

void flpv3l_sram_write_all(uint32_t data_type, uint16_t flash_inst) {
    uint32_t pattern = 0x00000000;
    uint32_t idx_i;

    FLASH_turn_on(flash_inst);
	
    // Choose Data Pattern
    if      (data_type == 0) pattern = 0x00000000;
    else if (data_type == 1) pattern = 0xFFFFFFFF;
    else if (data_type == 2) pattern = 0xAAAAAAAA;
    else if (data_type == 3) pattern = 0x55555555;

    // Initialize temp_storage
    for (idx_i=0; idx_i<_SIZE_TEMP_; idx_i++) temp_storage[idx_i] = pattern;

    // Write into SRAM
    for (idx_i=0; idx_i<_ITER_TEMP_; idx_i++) {
        uint32_t dest_mem_addr = (idx_i << (_LOG2_TEMP_ + 2));
        mbus_copy_mem_from_local_to_remote_bulk(FLP1_ADDR + flash_inst, (uint32_t *) dest_mem_addr, temp_storage, (_SIZE_TEMP_ - 1));
    }

    FLASH_turn_off(flash_inst);
}
	
static void FLASH_prog (uint16_t banks, uint16_t flash_inst) { //program flash in 8k word banks; 32 banks per flash instance
	uint32_t m;
	
	FLASH_turn_on(flash_inst);

	for(m=0; m<banks; m++) {
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x08 , m*2048); // Set FLSH_START_ADDR
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x07 , 0x0); // Set SRAM_START_ADDR
	
		prev23e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x0; //disable watchdog for long access
		*REG_MBUS_WD = prev23e_r1F.as_int;
	
		set_halt_until_mbus_rx();
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x09 , 0x07FFE7); // SRAM -> Flash
		set_halt_until_mbus_tx();
	
		if (*REG1 != 0x00005D) flp_fail(4);
		
		prev23e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x16E360; //set watchdog back to default
		*REG_MBUS_WD = prev23e_r1F.as_int;
	}
	FLASH_turn_off(flash_inst);
}

static void FLASH_test_read (uint16_t banks, uint16_t flash_inst) { //read flash out in 8k word banks to mbus; 32 banks per flash instance
    uint32_t i;
    uint32_t j;
	uint32_t m;
    uint32_t flp_sram_addr;

	FLASH_turn_on(flash_inst);
	
	for(m=0; m<banks; m++) {

		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x08 , m*2048); // Set FLSH_START_ADDR
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x07 , 0x0); // Set SRAM_START_ADDR
	
		prev23e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x0; //disable watchdog for long access
		*REG_MBUS_WD = prev23e_r1F.as_int;
	
		set_halt_until_mbus_rx();
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x09 , 0x07FFE3); // Flash -> SRAM
		set_halt_until_mbus_tx();
	
		if (*REG1 != 0x00002B) flp_fail(4);
		
		prev23e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x16E360; //set watchdog back to default
		*REG_MBUS_WD = prev23e_r1F.as_int;
		
		flp_sram_addr = 0;
		
		for(j=0; j<16; j++){
			set_halt_until_mbus_rx();
			mbus_copy_mem_from_remote_to_any_bulk (FLP1_ADDR + flash_inst, (uint32_t *) flp_sram_addr, 0x1, read_data, 127);
			set_halt_until_mbus_tx();
			delay(100);
			for (i=0; i<128; i++) {
				mbus_write_message32(0xC0, read_data[i]);
			}
			flp_sram_addr = flp_sram_addr + 512;
			mbus_write_message32(0xDD, j);
		}
	}
	FLASH_turn_off(flash_inst);
}
//***************************************************
//  ADO functions
//***************************************************
static void ado_initialization(void){

    // Adjust layer controller clock frequency
    adov1a_r0E.LC_CLK_DIV = 2;
    mbus_remote_register_write(ADO_ADDR, 0x0E, adov1a_r0E.as_int);
	
    // Enable 16k clock going into AFE
    adov1a_r02.AFE_16K_EN = 1;
    mbus_remote_register_write(ADO_ADDR, 0x02, adov1a_r02.as_int);

    // CP CLK setting
    adov1a_r03.CP_CLK_DIV_1P2 = 0; // Default = 2'h3 (1kHz) // 2'h0 (4kHz)
    mbus_remote_register_write(ADO_ADDR, 0x03, adov1a_r03.as_int);
 
    // Adjust LDO bias current
    adov1a_r00.LDO_CTRLB_IREF_IBIAS_0P9HP = 15; // PMOS switch, Lowest: 15, Default =4'hA
    adov1a_r00.LDO_CTRLB_IREF_IBIAS_1P4HP = 15; // PMOS switch, Lowest: 15, Default =4'h9
    mbus_remote_register_write(ADO_ADDR, 0x00, adov1a_r00.as_int);

    // Adjust LDO 1P4 & 0P9 reference voltage level
    adov1a_r01.LDO_CTRL_VREFLDO_VOUT_1P4HP = 4; // Default = 4'h6
    adov1a_r01.LDO_CTRL_VREFLDO_VOUT_0P9HP = 3; // Default = 4'h4
    mbus_remote_register_write(ADO_ADDR, 0x01, adov1a_r01.as_int);

    // Adjust SAFR clk frequency
    adov1a_r0B.DIV1 = 15; // Default= 14'h0640
    //adov1a_r0B.DIV2 = 15; // Default= 14'h064A
    mbus_remote_register_write(ADO_ADDR, 0x0B, adov1a_r0B.as_int);

    // PGA Setting
    adov1a_r08.REC_PGA_BWCON = 32; // Default= 9'h0;
    adov1a_r08.REC_PGA_GCON = 31;   // Default= 5'h1
    mbus_remote_register_write(ADO_ADDR, 0x08, adov1a_r08.as_int); 

    // PGA feedback cap 
    adov1a_r06.REC_PGA_CFBADD = 1;   // Default= 1'h0 (10fF); 1'h1 (20fF)
    mbus_remote_register_write(ADO_ADDR, 0x06, adov1a_r06.as_int); 

    // LGA & PGA bias current setting
    adov1a_r02.IBC_REC_LNA = 6;  // Default= 5'h3
    adov1a_r02.IBC_REC_PGA = 11; // Default= 4'h5
    adov1a_r02.IB_GEN_CLK_EN = 1;
    mbus_remote_register_write(ADO_ADDR, 0x02, adov1a_r02.as_int);
 
    // LNA bias setting
    adov1a_r05.REC_LNA_N1_CON = 2; // Default= 6'h7
    adov1a_r05.REC_LNA_N2_CON = 2; // Default= 6'h7
    adov1a_r05.REC_LNA_P1_CON = 3; // Default= 6'h8
    adov1a_r05.REC_LNA_P2_CON = 3; // Default= 6'h8
    mbus_remote_register_write(ADO_ADDR, 0x05, adov1a_r05.as_int);

    // PGA bias setting
    adov1a_r07.REC_PGA_P1_CON = 3; // Default= 5'h1
    mbus_remote_register_write(ADO_ADDR, 0x07, adov1a_r07.as_int);//007064

    
}

static void ldo_set_mode(uint8_t mode){
	if(mode==1){        //ON
		adov1a_r00.LDO_PG_IREF = 0;
        adov1a_r00.LDO_PG_VREF_1P4HP = 0;
        adov1a_r00.LDO_PG_VREF_0P9HP = 0;
		adov1a_r00.LDO_PG_LDOCORE_1P4HP = 0;
        adov1a_r00.LDO_PG_LDOCORE_0P9HP = 0;
        mbus_remote_register_write(ADO_ADDR, 0x00, adov1a_r00.as_int);
	
		delay(MBUS_DELAY*9);
	} 
	else {
		adov1a_r00.LDO_PG_IREF = 1;
        adov1a_r00.LDO_PG_VREF_1P4HP = 1;
        adov1a_r00.LDO_PG_VREF_0P9HP = 1;
		adov1a_r00.LDO_PG_LDOCORE_1P4HP = 1;
        adov1a_r00.LDO_PG_LDOCORE_0P9HP = 1;
        mbus_remote_register_write(ADO_ADDR, 0x00, adov1a_r00.as_int);
	}
}

static void safr_set_mode(uint8_t mode){
	if(mode==1){        //ON
	// Power-on SAFR 
        adov1a_r0A.PG_SIG_V1P2_HP = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0A, adov1a_r0A.as_int);

        adov1a_r0A.PG_SIG_V1P2_FS = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0A, adov1a_r0A.as_int);

		delay(MBUS_DELAY*9);
        adov1a_r0A.ISO_1P4_HP = 0;
        adov1a_r0A.ISO_1P4_FS = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0A, adov1a_r0A.as_int);

		delay(MBUS_DELAY*9);

	// Start SAFR CLK
        adov1a_r0A.HP_RST = 0;   // Enable slow clk
        adov1a_r0A.CLK_GATE = 1; // Enable fast clk
        mbus_remote_register_write(ADO_ADDR, 0x0A, adov1a_r0A.as_int);

	} 
	else {
	// Disable SAFR CLK
        adov1a_r0A.HP_RST = 1;   
        adov1a_r0A.CLK_GATE = 0; 
        mbus_remote_register_write(ADO_ADDR, 0x0A, adov1a_r0A.as_int);

	// Power-off SAFR 
        adov1a_r0A.PG_SIG_V1P2_HP = 1;
        adov1a_r0A.PG_SIG_V1P2_FS = 1;
        adov1a_r0A.ISO_1P4_HP = 1;
        adov1a_r0A.ISO_1P4_FS = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0A, adov1a_r0A.as_int);
	}
}

static void amp_set_mode(uint8_t mode){
	if(mode==1){        //LNA + PGA ON
	// Enable current bias for buf
        adov1a_r02.REC_ADCDRI_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x02, adov1a_r02.as_int);

	// Enable LNA 
        adov1a_r04.REC_LNA_AMPEN = 1;
        adov1a_r04.REC_LNA_OUTSHORT_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov1a_r04.as_int);

	// Enable PGA 
        adov1a_r06.REC_PGA_AMPEN = 1;
        adov1a_r06.REC_PGA_OUTSHORT_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x06, adov1a_r06.as_int);

		adov1a_r0A.HP_F_SEL = 0; // Default= 1'h1
		mbus_remote_register_write(ADO_ADDR, 0x0A, adov1a_r0A.as_int);

		adov1a_r0B.DIV1 = 511; // Default= 14'h0640
		//adov1a_r0B.DIV2 = 15; // Default= 14'h064A
		mbus_remote_register_write(ADO_ADDR, 0x0B, adov1a_r0B.as_int);
	
	}
	else if(mode==2){	// Turn on LNA only
        adov1a_r02.REC_ADCDRI_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x02, adov1a_r02.as_int);

	// Enable LNA 
        adov1a_r04.REC_LNA_AMPEN = 1;
        adov1a_r04.REC_LNA_OUTSHORT_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov1a_r04.as_int);

		adov1a_r0A.HP_F_SEL = 0; // Default= 1'h1
		mbus_remote_register_write(ADO_ADDR, 0x0A, adov1a_r0A.as_int);

		adov1a_r0B.DIV1 = 511; // Default= 14'h0640
		//adov1a_r0B.DIV2 = 15; // Default= 14'h064A
		mbus_remote_register_write(ADO_ADDR, 0x0B, adov1a_r0B.as_int);

	}
	else if(mode==3){	// Turn on PGA only
        adov1a_r02.REC_ADCDRI_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x02, adov1a_r02.as_int);

	// Enable PGA 
        adov1a_r06.REC_PGA_AMPEN = 1;
        adov1a_r06.REC_PGA_OUTSHORT_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x06, adov1a_r06.as_int);

		adov1a_r0A.HP_F_SEL = 0; // Default= 1'h1
		mbus_remote_register_write(ADO_ADDR, 0x0A, adov1a_r0A.as_int);

		adov1a_r0B.DIV1 = 511; // Default= 14'h0640
		//adov1a_r0B.DIV2 = 15; // Default= 14'h064A
		mbus_remote_register_write(ADO_ADDR, 0x0B, adov1a_r0B.as_int);
	} 
	else {	// Turn off LNA + PGA
		adov1a_r0A.HP_F_SEL = 1; // Default= 1'h1
		mbus_remote_register_write(ADO_ADDR, 0x0A, adov1a_r0A.as_int);

		adov1a_r0B.DIV1 = 15; // Default= 14'h0640
		//adov1a_r0B.DIV2 = 15; // Default= 14'h064A
		mbus_remote_register_write(ADO_ADDR, 0x0B, adov1a_r0B.as_int);

	// Disable PGA 
        adov1a_r06.REC_PGA_AMPEN = 0;
        adov1a_r06.REC_PGA_OUTSHORT_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x06, adov1a_r06.as_int);

	// Disable LNA 
        adov1a_r04.REC_LNA_AMPEN = 0;
        adov1a_r04.REC_LNA_OUTSHORT_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov1a_r04.as_int);

	// Disable current bias for buf
        adov1a_r02.REC_ADCDRI_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x02, adov1a_r02.as_int);
	}
}

static void adc_set_mode(uint8_t mode){
	if(mode==1){        //ON
        adov1a_r09.PG_ADC_CLKGEN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x09, adov1a_r09.as_int);

		delay(MBUS_DELAY*2);

        adov1a_r09.REC_ADC_ISOL_LC = 0;
        mbus_remote_register_write(ADO_ADDR, 0x09, adov1a_r09.as_int);

        adov1a_r09.REC_ADC_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x09, adov1a_r09.as_int);

	} else {
        adov1a_r09.REC_ADC_ISOL_LC = 1;
        adov1a_r09.REC_ADC_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x09, adov1a_r09.as_int);

		adov1a_r09.PG_ADC_CLKGEN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x09, adov1a_r09.as_int);
	}
}

static void afe_set_mode(uint8_t mode){
	if(mode==1){        //ON
	// Turn on LDO
	ldo_set_mode(1);
	
	delay(MBUS_DELAY*9);

	// Start SAFR CLK
	safr_set_mode(1);

	// Enable AMP 
	amp_set_mode(1);

	// Enable ADC 
	adc_set_mode(1);

	} else{       // AFE off

	// Stop ADC 
	adc_set_mode(0);

	// Disable AMP 
	amp_set_mode(0);

	// Disable SAFR CLK
	safr_set_mode(0);

	// Turn off LDO
	ldo_set_mode(0);

    }
}
static void cp_set_mode(uint32_t mode){
	if(mode==1){        //ON
   	    *TIMERWD_GO = 0x0;
   	    adov1a_r03.CP_CLK_EN_1P2 = 1;
   	    adov1a_r03.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x03, adov1a_r03.as_int);
   	    delay(CP_DELAY); 
   	    adov1a_r03.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x03, adov1a_r03.as_int);
   	    delay(CP_DELAY*1); 
   	    adov1a_r03.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x03, adov1a_r03.as_int);
   	    delay(CP_DELAY); 
   	    adov1a_r03.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x03, adov1a_r03.as_int);
   	    delay(CP_DELAY*1); 
   	    adov1a_r03.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x03, adov1a_r03.as_int);
   	    delay(CP_DELAY); 
   	    adov1a_r03.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x03, adov1a_r03.as_int);
   	}
   	else if(mode==2){        //ON
   	    adov1a_r03.CP_CLK_EN_1P2 = 1;
   	    adov1a_r03.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x03, adov1a_r03.as_int);
   	    delay(CP_DELAY*0.2); 
   	    adov1a_r03.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x03, adov1a_r03.as_int);
   	}
   	else if(mode == 3){
   	    adov1a_r03.CP_CLK_EN_1P2 = 1;
   	    adov1a_r03.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x03, adov1a_r03.as_int);
   	}
   	else if(mode == 4){
   	    adov1a_r03.CP_CLK_EN_1P2 = 1;
   	    adov1a_r03.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x03, adov1a_r03.as_int);
   	}
   	else{                   //OFF
   	    adov1a_r03.CP_CLK_EN_1P2 = 0;
   	    adov1a_r03.CP_CLK_DIV_1P2 = 3;
   	    mbus_remote_register_write(ADO_ADDR, 0x03, adov1a_r03.as_int);
   	}	
}

//***************************************************
//  ADO+FLP functions
//***************************************************
inline static void comp_finish(void){
	
    *EP_MODE = (*EP_MODE & 1);
    //digital_set_mode(0); // ADO DSP ALL OFF
    afe_set_mode(0); // ADO AFE OFF
    delay(10000);
    cp_set_mode(0); // ADO CP OFF
	
    FLASH_pp_off(0);
	
    FLASH_turn_off(0);
	//pmu_set_sleep_clk(0x3,0x2,0x3,0x7);  //back to normal pmu sleep
	//pmu_set_sleep_clk(0x6,0x4,0x4,0x8); 
}

inline static void comp_stream(uint32_t sleep_time){ //sleep_time is wakeup timer ticks
	//uint16_t i;
    *EP_MODE = (*EP_MODE | 2);
    //pmu_set_active_clk(0xA,0x9,0x0D,0xA); 
    cp_set_mode(1); 
    afe_set_mode(1); // ADO HP AFE ON
	
	//delay(150000); // delay to unmount system
	
    // Flash setup and start /////
    FLASH_turn_on(0);
    FLASH_pp_ready(0);
	//pmu_set_sleep_clk(0x9,0x8,0x10,0x8); // pmu_sleep_clk for streaming
	
    FLASH_pp_on(0);
	
	//delay(200000);
    /////////////////////////////
    //digital_set_mode(3); // HP Compression only start
    
    delay(330000); // Compression-length control: ~14s
	
    //set_wakeup_timer(sleep_time,1,1);    //enable timer
	
	comp_finish();
    //mbus_write_message32(MBUS_POWER, (MBUS_SLEEP_BY_SHORT_PREFIX << 28) | 0x00002000);
    //mbus_write_message32(0x01, 0x20002000);
    //mbus_write_message32(0x01, 0x27E7E000); // selective sleep except ADO and FLP1
	//mbus_write_message32(0x01, 0x27EFA000); // selective sleep except ADO and FLP1
	
    //mbus_write_message32(0x01, 0x27FDE000); // selective sleep except ADO
    //mbus_write_message32(0x01, 0x27FFE000); // selective sleep for all layers
	
	
}


//***************************************************
// End of Program Sleep Operation
//***************************************************

static void operation_sleep(void){
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;
	
    // Go to Sleep
	//mbus_write_message32(0xFA, 0xABABABAB);
    mbus_sleep_all();
    while(1);
}




static void operation_sleep_notimer(void){
    // Make sure the irq counter is reset    
    //exec_count_irq = 0;

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
    
    pmu_sar_conv_ratio_val_test_on = 76;
    
	
	
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    //config_timerwd(TIMERWD_VAL);
    *TIMERWD_GO = 0x0;
    *REG_MBUS_WD = 0;
	
    // EP Mode
    *EP_MODE = 0;
	
    // Set CPU & Mbus Clock Speeds
	prev23e_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prev23e_r0B.CLK_GEN_DIV_MBC = 0x2; // Default 0x1
    prev23e_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
    //prev22e_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    //prev22e_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    //prev22e_r0B.CLK_GEN_DIV_CORE = 0x2; // Default 0x3
    //prev22e_r0B.GOC_CLK_GEN_SEL_FREQ = 0x0; // Default 0x6
    //prev22e_r0B.GOC_CLK_GEN_SEL_DIV = 0x1; // Default 0x0
    prev23e_r0B.GOC_SEL = 0xf; // Default 0x8
	*REG_CLKGEN_TUNE = prev23e_r0B.as_int;
	
    prev23e_r1C.SRAM0_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
    prev23e_r1C.SRAM0_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
    //prev22e_r1C.SRAM0_USE_INVERTER_SA= 0; // Default 0x0, 1bit
    *REG_SRAM0_TUNE = prev23e_r1C.as_int;
	
	// Wakeup if there's pending request
	//prev22e_r1B.WAKEUP_ON_PEND_REQ = 0x1; // Default 0x0
	//*REG_SYS_CONF = prev22e_r1B.as_int;
	
    //Enumerate & Initialize Registers
    enumerated = ENUMID;
    //exec_count = 0;
    //exec_count_irq = 0;
    //PMU_ADC_3P0_VAL = 0x62;
    //pmu_parkinglot_mode = 3;
	
    //Enumeration



	mbus_enumerate(ADO_ADDR);
    delay(MBUS_DELAY);	
	mbus_enumerate(MRM1_ADDR);
    delay(MBUS_DELAY);	
	mbus_enumerate(MRM2_ADDR);
    delay(MBUS_DELAY);	
    mbus_enumerate(FLP1_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(PMU_ADDR);
    delay(MBUS_DELAY);
	XO_init(); //XO initialization
	
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
			wakeup_data = 0;
			read_data_batadc_diff = 0;
			
			TEMP_CALIB_A = 240000;
			TEMP_CALIB_B = 3750000;
			NUM_MEAS_USER = TEMP_STORAGE_SIZE;
			
			
			ado_initialization(); //ADOv6B initialization			
			FLASH_initialization(1); //initialize 1 flash instances
			//XO_init(); //XO initialization

			
			
			*TIMERWD_GO = 0x0;
			*REG_MBUS_WD = 0;
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
	//uint16_t i;
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
    //[11] = gpio[3////]

	if(((*SREG_WAKEUP_SOURCE >> 1) & 1) == 1){ //wake up by wakeup timer
		*NVIC_IPR0 = 0x00000040; //allow for other interrupts to be handled
	
		comp_finish();

		delay(MBUS_DELAY*200); //~2sec
	}   
}
void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    wfi_timeout_flag = 1;
	//set_halt_until_mbus_tx();
}
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);
		   
}
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1);
}
void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2);
	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xE0, *REG2);
	#endif  
}
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3);
}
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
	
    *TIMERWD_GO = 0x0; //turning off watchdogs
	*REG_MBUS_WD = 0;
	
    if (enumerated != ENUMID) operation_init();
	
    *NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);
	

	if(*SREG_WAKEUP_SOURCE & 1) { //wakeup due to GOC/EP
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xEE,  *GOC_DATA_IRQ);
		#endif    
		wakeup_data = *GOC_DATA_IRQ;
		uint32_t data_cmd = (wakeup_data>>24) & 0xFF;
		uint32_t data_val = wakeup_data & 0xFF;
		uint32_t data_val2 = (wakeup_data>>8) & 0xFF;
		uint32_t data_val3 = (wakeup_data>>16) & 0xFF;
		//uint32_t data_val4 = wakeup_data & 0xFFFF;
		uint32_t val16 = wakeup_data & 0xFFFF;
		
		if(data_cmd == 0x1){
			if(data_val==1){   
				xo_start();
			}
			else if(data_val==2){   
				prev23e_r19.XO_RESETn       = 1;
				prev23e_r19.XO_PGb_START_UP = 1;
				*REG_XO_CONF1 = prev23e_r19.as_int;

				delay(XO_WAIT_A); // Delay A (~1s; XO Start-Up)

				prev23e_r19.XO_ISOL_CLK_HP = 0;
				*REG_XO_CONF1 = prev23e_r19.as_int;
    
				delay(XO_WAIT_B); // Delay B (~1s; VLDO & IBIAS generation)

				prev23e_r19.XO_ISOL_CLK_LP = 0;
				*REG_XO_CONF1 = prev23e_r19.as_int;
			}
			else if(data_val==0){
				xo_stop();
			}
			else if(data_val==3){	
				if(data_val2==1){
					prev23e_r19.XO_EN_SLOW = 0x0;
					prev23e_r19.XO_EN_NOM  = 0x1;
					prev23e_r19.XO_EN_FAST = 0x0;
				}
				else if(data_val2==2){
					prev23e_r19.XO_EN_SLOW = 0x1;
					prev23e_r19.XO_EN_NOM  = 0x0;
					prev23e_r19.XO_EN_FAST = 0x0;
				}
				else if(data_val2==3){
					prev23e_r19.XO_EN_SLOW = 0x0;
					prev23e_r19.XO_EN_NOM  = 0x0;
					prev23e_r19.XO_EN_FAST = 0x1;
				}
				*REG_XO_CONF1 = prev23e_r19.as_int;
				delay(100);
	
				prev23e_r19.XO_CAP_TUNE = data_val3 & 0xF; // Additional Cap on OSC_IN & OSC_DRV
				*REG_XO_CONF1 = prev23e_r19.as_int;
			}
			else if(data_val==4){
				start_xo_cout();
			}
			else if(data_val==5){
				stop_xo_cout();
			}
		}

		else if(data_cmd == 0x03){
			pmu_set_sleep_clk((data_val & 0xF),((data_val>>4) & 0xF),(data_val2 & 0x1F),(data_val3 & 0xF));
		}
		else if(data_cmd == 0x04){
			pmu_set_active_clk((data_val & 0xF),((data_val>>4) & 0xF),(data_val2 & 0x1F),(data_val3 & 0xF));
			delay(MBUS_DELAY*300); //~3sec
		}
		else if(data_cmd == 0x05){ //change active PMU settings for v3p6
			pmu_set_active_3p6((data_val & 0xF),((data_val>>4) & 0xF),(data_val2 & 0x1F));		
			delay(MBUS_DELAY*10);
		}		
		else if(data_cmd == 0x06){
			pmu_set_sar_override(data_val);
		}

		else if(data_cmd == 0x10){       // Charge pump control
			cp_set_mode(data_val);
		}
		else if(data_cmd == 0x07){ // ADC control
			adc_set_mode(data_val);
			if(data_val==1 && data_val2 !=0){
   		   		*TIMERWD_GO = 0x0;
				config_timer32(TIMER32_VAL*data_val2, 1, 0, 0); // 1/10 of MBUS watchdog timer default

				WFI();

				//*TIMER32_GO = 0;
   		   		//*TIMERWD_GO = 0x0;
		   		//uint32_t cnt=0;
		   		//while(cnt<data_val2){
				//	delay(MBUS_DELAY*1000); 
				//	cnt=cnt+1;
		  		//}
			adc_set_mode(0x0); 
	    		}
		}		
		else if(data_cmd == 0x09){ // LDO control
			ldo_set_mode(data_val);
		}
		else if(data_cmd == 0x0A){ // SAFR control
			safr_set_mode(data_val);
		}
		else if(data_cmd == 0x0B){ // AMP control
			amp_set_mode(data_val);
		}			




		else if(data_cmd == 0x16){ //flash erase: data_val = # of instances - 1
			mbus_write_message32(0xDA, 0x23232323);
			flash_erasedata(data_val);
			mbus_write_message32(0xDF, 0x89898989);
		}
		else if(data_cmd == 0x17){ //flash prog A's & 5's: data_val = flash instance, data_val2 = # of 8k banks
			mbus_write_message32(0xDA, 0x12121212);
			flash_erasedata(data_val);
			mbus_write_message32(0xDA, 0x23232323);
			flpv3l_sram_write_all(2,data_val); // write AAAA
			mbus_write_message32(0xDA, 0x34343434);
			FLASH_prog(data_val2, data_val);
			mbus_write_message32(0xDA, 0x45454545);
			flpv3l_sram_write_all(0,data_val); // write 0000
			mbus_write_message32(0xDA, 0x56565656);
			FLASH_test_read(data_val2, data_val);
			mbus_write_message32(0xDA, 0x67676767);
			flash_erasedata(data_val);
			mbus_write_message32(0xDA, 0x78787878);
			flpv3l_sram_write_all(3,data_val); // write 5555
			mbus_write_message32(0xDA, 0x89898989);
			FLASH_prog(data_val2, data_val);
			mbus_write_message32(0xDA, 0x9A9A9A9A);
			flpv3l_sram_write_all(0,data_val); // write 0000
			mbus_write_message32(0xDA, 0xABABABAB);
			FLASH_test_read(data_val2, data_val);
			mbus_write_message32(0xDA, 0xBCBCBCBC);
		}
		else if(data_cmd == 0x18){ //rerun operation_init
			operation_init();
		}
		else if(data_cmd == 0x19){ //go to sleep
			operation_sleep_notimer();	
		}
		else if(data_cmd == 0x1A){ 
			adov1a_r08.REC_PGA_GCON = data_val & 0x1F;		
			mbus_remote_register_write(ADO_ADDR, 0x08, adov1a_r08.as_int); 
			delay(MBUS_DELAY*10);
		}		
		else if(data_cmd == 0x0C){
			adov1a_r0B.DIV1 = data_val & 0x1FF; // Default= 14'h0640
			mbus_remote_register_write(ADO_ADDR, 0x0B, adov1a_r0B.as_int);
		}		
		

		
		else if(data_cmd == 0x20){ //flash read: data_val = number of 8k banks to read
			mbus_write_message32(0xDA, 0xABABABAB);
			FLASH_read(val16);
			mbus_write_message32(0xDF, 0xEFEFEFEF);
		}
		else if(data_cmd == 0x22){ 
			finst=0;
			//cp_set_mode(1);
			comp_stream(val16);
		}




	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xEE, 0x00000E2D);
	#endif
	}
	
    // Initialization sequence
    if ((*EP_MODE & 1) == 1) {
		*GOC_DATA_IRQ = 0;					
		*TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
		*REG_MBUS_WD = 0; // Disables Mbus watchdog timer
        while (1); 
        //config_timerwd(TIMERWD_VAL);
	}
    else {
        operation_sleep_notimer();
	}
	//operation_sleep_notimer();
    return 0;
}
