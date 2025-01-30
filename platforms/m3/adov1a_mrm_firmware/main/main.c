//*******************************************************************
// Starter code for Arete Final Phase 1
// V1
//*******************************************************************
// PRE -> ADO -> MRMv1L -> PMU
//
// 1) Boot-up the system
// 2) Program the system using EP -> Should be in sleep
// 3) 1A00001F -> ADO: Change gain
// 4) 09000001 -> ADO: Turn on LDO
// 5) 0A000001 -> ADO: Turn on SAFR
// 6) 0B000001 -> ADO: Turn on AMP
// 7) 0C00000F -> ADO: DIV1
// 8) 10000001 -> ADO: Turn on CP
// 9) Turn on the ping-pong mode (starting points must be configrable)
// 10) 07000A01 -> ADO: Turn on ADC
//     ADO starts the streaming, until you turn off the ADC.
// 11) Possible ending scenario:
//      1) MRAM is filled up. It sends the IRQ, then PRE turns off everything and goes back to sleep.
//      2) User uses the XO to measure the streaming time. Once XO generates IRQ, it turns off everything and goes back to sleep.
//      3) (optional) once the MRAM receives a specified number of bits
// 12) MRAM_Read() reads out MRAM, sends the data through MBus for Salaea monitoring.
// 64kbps = 64000 bps or 65536 bps

#include "PREv23E.h"
#include "PREv23E_RF.h"
#include "PMUv13r1_RF.h"
//#include "FLPv3S.h"
//#include "FLPv3S_RF.h"
#include "MRMv1L.h"
#include "ADOv1A_RF.h"
//#include "MRMv1S_RF.h"

#include "mbus.h"

#include "DFT_LUT.txt"
#include "DCT_PHS.txt"

////////////////////////////////////////
// uncomment this for debug 
#define DEBUG_MODE          // Test VDD ON in ULP
#define DEBUG_MBUS_MSG      // Debug MBus message
/////////////////////////////////////////


#define ADO_ADDR 0x2
#define MRM_ADDR 0x6
#define MRM1_ADDR 0x7
#define MRM2_ADDR 0x8
#define PMU_ADDR 0x9

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED         0
#define FLAG_INITIALIZED        1

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
volatile uint32_t goc_data;
volatile uint32_t goc_data_ext;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t error_code;
volatile uint32_t finst;
volatile uint32_t pmu_sar_conv_ratio_val_test_on;
volatile uint32_t pmu_sar_conv_ratio_val_test_off;
volatile uint32_t read_data_batadc;
volatile uint32_t read_data_batadc_diff;
volatile uint32_t mem_read_data[2];

volatile pre_r0B_t pre_r0B = PRE_R0B_DEFAULT;
volatile pre_r19_t pre_r19 = PRE_R19_DEFAULT;
volatile pre_r1A_t pre_r1A = PRE_R1A_DEFAULT;
volatile pre_r1B_t pre_r1B = PRE_R1B_DEFAULT;
volatile pre_r1C_t pre_r1C = PRE_R1C_DEFAULT;
volatile pre_r1F_t pre_r1F = PRE_R1F_DEFAULT;

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
//volatile flpv3s_r05_t flpv3s_r05 = FLPv3S_R05_DEFAULT;

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

//static void xo_start(void) {
//YK: Name changed to pre_xo_start() to avoid name conflict with the function defined in PREv23E.h.
static void pre_xo_start(void) {


    pre_r19.XO_RESETn       = 1;
    pre_r19.XO_PGb_START_UP = 1;
    *REG_XO_CONF1 = pre_r19.as_int;

	delay(XO_WAIT_A); // Delay A (~1s; XO Start-Up)

    pre_r19.XO_ISOL_CLK_HP = 0;
    *REG_XO_CONF1 = pre_r19.as_int;
    
    delay(XO_WAIT_B); // Delay B (~1s; VLDO & IBIAS generation)

    pre_r19.XO_ISOL_CLK_LP = 0;
    *REG_XO_CONF1 = pre_r19.as_int;
    
    delay(100); // Delay C (~1ms; SCN Output Generation)
	
 // ======================================
    pre_r19.XO_START_UP = 0;
    *REG_XO_CONF1 = pre_r19.as_int;

    pre_r19.XO_ISOL_CLK_HP = 1;
    *REG_XO_CONF1 = pre_r19.as_int;

    pre_r19.XO_PGb_START_UP = 0;
    *REG_XO_CONF1 = pre_r19.as_int;

    delay(100); // Dummy Delay

	
    enable_xo_timer();
    start_xo_cout(); // turns on XO clk output pad
}

//static void xo_stop( void ) {
//YK: Name changed to pre_xo_stop() to avoid name conflict with the function defined in PREv23E.h.
static void pre_xo_stop( void ) {
    // Stop the XO Driver
    pre_r19.XO_ISOL_CLK_LP = 1;
    *REG_XO_CONF1 = pre_r19.as_int;

    pre_r19.XO_RESETn   = 0;
    pre_r19.XO_START_UP = 1;
    *REG_XO_CONF1 = pre_r19.as_int;
}

static void XO_init(void) {
    //pre_r19.XO_SEL_VLDO = 0x0;	// Default value: 3'h0
    pre_r19.XO_CAP_TUNE = 0x3;	// Default value: 3'h0
    //prev22e_r19.XO_SEL_vV0P6 = 0x0;	// Default value: 2'h2
	pre_r19.XO_INJ = 0x0; 		// Default value: 2'h2
    *REG_XO_CONF1 = pre_r19.as_int;

    pre_r1A.XO_SEL_CLK_OUT_DIV = 0x4;
    pre_r1A.XO_SEL_DLY = 0x1; 	// Default value: 3'h1

    *REG_XO_CONF2 = pre_r1A.as_int;

    pre_xo_start();
    

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
//  MRM functions
//***************************************************
static void MRAM_start_pp (uint16_t mrm_prefix, uint32_t start_pid, uint32_t num_pages, uint16_t wait_for_irq) {
    if (num_pages==0) num_pages = MRM_NUM_PAGES_MRAM;
    mrm_set_clock_mode(/*mrm_prefix*/mrm_prefix, /*clock_mode*/1);
    mrm_turn_on_ldo(/*mrm_prefix*/mrm_prefix);
    mrm_pp_ext_stream (/*mrm_prefix*/mrm_prefix, /*bit_en*/0x1, /*num_pages*/num_pages, /*mram_page_id*/start_pid, /*wait_for_irq*/wait_for_irq);
}

static void MRAM_read (uint16_t mrm_prefix, uint32_t start_pid, uint32_t num_pages) {
    // Disable PRE Watchdog
    *TIMERWD_GO  = 0;

    if (num_pages==0) num_pages = MRM_NUM_PAGES_MRAM;

    mrm_turn_on_ldo(/*mrm_prefix*/mrm_prefix);

    uint32_t mram_pid = start_pid;
    uint32_t remaining = num_pages;
    while (remaining > NUM_MAX_PAGES_MBUS) {
        mrm_read_mram_page_debug (/*mrm_prefix*/mrm_prefix, /*mrm_mram_pid*/mram_pid, /*num_pages*/NUM_MAX_PAGES_MBUS, /*dest_prefix*/0xC0);
        mram_pid += NUM_MAX_PAGES_MBUS;
        remaining -= NUM_MAX_PAGES_MBUS;
    }
    if (remaining > 0) {
        mrm_read_mram_page_debug (/*mrm_prefix*/mrm_prefix, /*mrm_mram_pid*/mram_pid, /*num_pages*/remaining, /*dest_prefix*/0xC0);
    }

    mrm_turn_off_ldo(/*mrm_prefix*/mrm_prefix);

    // Enable PRE Watchdog
    *TIMERWD_GO  = 1;
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
	pre_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    pre_r0B.CLK_GEN_DIV_MBC = 0x2; // Default 0x1
    pre_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
    //prev22e_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    //prev22e_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    //prev22e_r0B.CLK_GEN_DIV_CORE = 0x2; // Default 0x3
    //prev22e_r0B.GOC_CLK_GEN_SEL_FREQ = 0x0; // Default 0x6
    //prev22e_r0B.GOC_CLK_GEN_SEL_DIV = 0x1; // Default 0x0
    pre_r0B.GOC_SEL = 0xf; // Default 0x8
	*REG_CLKGEN_TUNE = pre_r0B.as_int;
	
    pre_r1C.SRAM0_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
    pre_r1C.SRAM0_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
    //prev22e_r1C.SRAM0_USE_INVERTER_SA= 0; // Default 0x0, 1bit
    *REG_SRAM0_TUNE = pre_r1C.as_int;
	
	// Wakeup if there's pending request
	//prev22e_r1B.WAKEUP_ON_PEND_REQ = 0x1; // Default 0x0
	//*REG_SYS_CONF = prev22e_r1B.as_int;
	
    //Enumerate & Initialize Registers
    //exec_count = 0;
    //exec_count_irq = 0;
    //PMU_ADC_3P0_VAL = 0x62;
    //pmu_parkinglot_mode = 3;
	
    //Enumeration



    set_halt_until_mbus_trx();
	mbus_enumerate(ADO_ADDR);
	mbus_enumerate(MRM_ADDR);
    mbus_enumerate(PMU_ADDR);
    set_halt_until_mbus_tx();

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
	goc_data = 0;
	read_data_batadc_diff = 0;
			
	TEMP_CALIB_A = 240000;
	TEMP_CALIB_B = 3750000;
	NUM_MEAS_USER = TEMP_STORAGE_SIZE;
			
			
	ado_initialization(); //ADOv6B initialization			

    // Initialize MRM Layers
    // NOTE: A later 'irq_reg_idx' assignment overwrits any previous assignments.
    //      Thus, ALL MRM layers will write into the same PRC/PRE register.
    mrm_init(/*mrm_prefix*/MRM_ADDR, /*irq_reg_idx*/1, /*clk_gen_s*/51);
    mrm_init(/*mrm_prefix*/MRM1_ADDR, /*irq_reg_idx*/1, /*clk_gen_s*/51);
    mrm_init(/*mrm_prefix*/MRM2_ADDR, /*irq_reg_idx*/1, /*clk_gen_s*/51);

	//XO_init(); //XO initialization
			
	*TIMERWD_GO = 0x0;
	*REG_MBUS_WD = 0;

    //-------------------------------------------------
    // End of Initialization
    //-------------------------------------------------
    set_flag(FLAG_INITIALIZED, 1);
}

//*******************************************************************
// INTERRUPT HANDLERS 
//*******************************************************************

void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_reg1(void) { // REG1
    disable_reg_irq(/*reg*/1);
}
//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
	
    *TIMERWD_GO = 0x0; //turning off watchdogs
	*REG_MBUS_WD = 0;
	
    if (!get_flag(FLAG_INITIALIZED)) operation_init();
	
    *NVIC_ISER = (1 << IRQ_REG1);
	

	if(*SREG_WAKEUP_SOURCE & 1) { //wakeup due to GOC/EP
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xEE,  *GOC_DATA_IRQ);
			mbus_write_message32(0xEF,  *(GOC_DATA_IRQ+1));
		#endif    
		goc_data     = *GOC_DATA_IRQ;
		goc_data_ext = *(GOC_DATA_IRQ+1);
		*GOC_DATA_IRQ = 0;					
        // goc_data_3   = goc_data[31:24]
        // goc_data_2   = goc_data[24:16]
        // goc_data_1   = goc_data[15:8]
        // goc_data_0   = goc_data[7:0]
		uint32_t goc_data_3  = (goc_data>>24) & 0xFF;
		uint32_t goc_data_2  = (goc_data>>16) & 0xFF;
		uint32_t goc_data_1  = (goc_data>> 8) & 0xFF;
		uint32_t goc_data_0  = (goc_data>> 0) & 0xFF;
		
        //------------------------------------------------------------------------
        // goc_data = 0x01xxxxxx: XO Operation
        //------------------------------------------------------------------------
		if(goc_data_3 == 0x01){
            //------------------------------------------------------------------------
            // goc_data = 0x01xxxx01
            //------------------------------------------------------------------------
			if(goc_data_0==0x01){   
				pre_xo_start();
			}
            //------------------------------------------------------------------------
            // goc_data = 0x01xxxx02: XO Start. Stay in High Power
            //------------------------------------------------------------------------
			else if(goc_data_0==0x02){   
				pre_r19.XO_RESETn       = 1;
				pre_r19.XO_PGb_START_UP = 1;
				*REG_XO_CONF1 = pre_r19.as_int;

				delay(XO_WAIT_A); // Delay A (~1s; XO Start-Up)

				pre_r19.XO_ISOL_CLK_HP = 0;
				*REG_XO_CONF1 = pre_r19.as_int;
    
				delay(XO_WAIT_B); // Delay B (~1s; VLDO & IBIAS generation)

				pre_r19.XO_ISOL_CLK_LP = 0;
				*REG_XO_CONF1 = pre_r19.as_int;
			}
            //------------------------------------------------------------------------
            // goc_data = 0x01xxxx00: XO Stop
            //------------------------------------------------------------------------
			else if(goc_data_0==0x00){
				pre_xo_stop();
			}
            //------------------------------------------------------------------------
            // goc_data = 0x01xxxx03
            //------------------------------------------------------------------------
			else if(goc_data_0==0x3){	
                //------------------------------------------------------------------------
                // goc_data = 0x01xx0103
                //------------------------------------------------------------------------
				if(goc_data_1==0x01){
					pre_r19.XO_EN_SLOW = 0x0;
					pre_r19.XO_EN_NOM  = 0x1;
					pre_r19.XO_EN_FAST = 0x0;
				}
                //------------------------------------------------------------------------
                // goc_data = 0x01xx0203
                //------------------------------------------------------------------------
				else if(goc_data_1==0x02){
					pre_r19.XO_EN_SLOW = 0x1;
					pre_r19.XO_EN_NOM  = 0x0;
					pre_r19.XO_EN_FAST = 0x0;
				}
                //------------------------------------------------------------------------
                // goc_data = 0x01xx0303
                //------------------------------------------------------------------------
				else if(goc_data_1==0x03){
					pre_r19.XO_EN_SLOW = 0x0;
					pre_r19.XO_EN_NOM  = 0x0;
					pre_r19.XO_EN_FAST = 0x1;
				}
				*REG_XO_CONF1 = pre_r19.as_int;
				delay(100);
	
				pre_r19.XO_CAP_TUNE = goc_data_2 & 0xF; // Additional Cap on OSC_IN & OSC_DRV
				*REG_XO_CONF1 = pre_r19.as_int;
			}
            //------------------------------------------------------------------------
            // goc_data = 0x01xxxx04: Enable XO output
            //------------------------------------------------------------------------
			else if(goc_data_0==0x04){
				start_xo_cout();
			}
            //------------------------------------------------------------------------
            // goc_data = 0x01xxxx05: Disable XO output
            //------------------------------------------------------------------------
			else if(goc_data_0==0x05){
				stop_xo_cout();
			}
		}

        //------------------------------------------------------------------------
        // goc_data = 0x03xxxxxx: PMU
        //------------------------------------------------------------------------
		else if(goc_data_3 == 0x03){
			pmu_set_sleep_clk((goc_data_0 & 0xF),((goc_data_0>>4) & 0xF),(goc_data_1 & 0x1F),(goc_data_2 & 0xF));
		}
        //------------------------------------------------------------------------
        // goc_data = 0x04xxxxxx: PMU
        //------------------------------------------------------------------------
		else if(goc_data_3 == 0x04){
			pmu_set_active_clk((goc_data_0 & 0xF),((goc_data_0>>4) & 0xF),(goc_data_1 & 0x1F),(goc_data_2 & 0xF));
			delay(MBUS_DELAY*300); //~3sec
		}
        //------------------------------------------------------------------------
        // goc_data = 0x05xxxxxx: PMU
        //------------------------------------------------------------------------
		else if(goc_data_3 == 0x05){ //change active PMU settings for v3p6
			pmu_set_active_3p6((goc_data_0 & 0xF),((goc_data_0>>4) & 0xF),(goc_data_1 & 0x1F));		
			delay(MBUS_DELAY*10);
		}		
        //------------------------------------------------------------------------
        // goc_data = 0x06xxxxxx: PMU
        //------------------------------------------------------------------------
		else if(goc_data_3 == 0x06){
			pmu_set_sar_override(goc_data_0);
		}



        //------------------------------------------------------------------------
        // goc_data = 0x1Axxxxxx: Change the Gain
        //------------------------------------------------------------------------
		else if(goc_data_3 == 0x1A){ 
			adov1a_r08.REC_PGA_GCON = goc_data_0 & 0x1F;		
			mbus_remote_register_write(ADO_ADDR, 0x08, adov1a_r08.as_int); 
			delay(MBUS_DELAY*10);
		}		
        //------------------------------------------------------------------------
        // goc_data = 0x09xxxxxx: LDO
        //------------------------------------------------------------------------
		else if(goc_data_3 == 0x09){ // LDO control
			ldo_set_mode(goc_data_0);
		}
        //------------------------------------------------------------------------
        // goc_data = 0x0Axxxxxx: SAFR
        //------------------------------------------------------------------------
		else if(goc_data_3 == 0x0A){ // SAFR control
			safr_set_mode(goc_data_0);
		}
        //------------------------------------------------------------------------
        // goc_data = 0x0Bxxxxxx: AMP
        //------------------------------------------------------------------------
		else if(goc_data_3 == 0x0B){ // AMP control
			amp_set_mode(goc_data_0);
		}			
        //------------------------------------------------------------------------
        // goc_data = 0x0Cxxxxxx: DIV1
        //------------------------------------------------------------------------
		else if(goc_data_3 == 0x0C){
			adov1a_r0B.DIV1 = goc_data_0 & 0x1FF; // Default= 14'h0640
			mbus_remote_register_write(ADO_ADDR, 0x0B, adov1a_r0B.as_int);
		}		
        //------------------------------------------------------------------------
        // goc_data = 0x10xxxxxx: CP
        //------------------------------------------------------------------------
		else if(goc_data_3 == 0x10){       // Charge pump control
			cp_set_mode(goc_data_0);
		}
        //------------------------------------------------------------------------
        // goc_data = 0x07xxxxxx: ADC
        //------------------------------------------------------------------------
		else if(goc_data_3 == 0x07){ // ADC control
			adc_set_mode(goc_data_0);
			if(goc_data_0==1 && goc_data_1 !=0){
   		   		*TIMERWD_GO = 0x0;
				config_timer32(TIMER32_VAL*goc_data_1, 1, 0, 0); // 1/10 of MBUS watchdog timer default

				WFI();

				//*TIMER32_GO = 0;
   		   		//*TIMERWD_GO = 0x0;
		   		//uint32_t cnt=0;
		   		//while(cnt<goc_data_1){
				//	delay(MBUS_DELAY*1000); 
				//	cnt=cnt+1;
		  		//}
			    adc_set_mode(0x0); 
	    		}
		}		

        //------------------------------------------------------------------------
        // goc_data = 0x20xnnnnn: Start Ping-Pong Mode
        //          goc_data[16:0]: Start Page ID [0 - 98303]
        //          goc_data_ext  : Number of pages to be streamed.
        //                          Valid Range: [0, 98303]
        //                          'goc_data_ext = 0' translates to 98304 pages (i.e., entire MRAM)
        //                          Each page is 1024 bits (= 15.625ms @ 64kbps)
        //------------------------------------------------------------------------
        else if (goc_data_3 == 0x20) {
            MRAM_start_pp (/*mrm_prefix*/MRM_ADDR, /*start_pid*/goc_data&0x1FFFF, /*num_pages*/goc_data_ext, /*wait_for_irq*/1);
        }
        //------------------------------------------------------------------------
        // goc_data = 0x21xnnnnn: Start ADO, Start Ping-Pong Mode. Go to sleep once the Ping-Pong is done.
        //          goc_data[16:0]: Start Page ID [0 - 98303]
        //          goc_data_ext  : Number of pages to be streamed.
        //                          Valid Range: [0, 98303]
        //                          'goc_data_ext = 0' translates to 98304 pages (i.e., entire MRAM)
        //                          Each page is 1024 bits (= 15.625ms @ 64kbps)
        //------------------------------------------------------------------------
        else if (goc_data_3 == 0x21) {
	        // Turn on LDO
	        ldo_set_mode(1);
	        
	        delay(MBUS_DELAY*9);

	        // Start SAFR CLK
	        safr_set_mode(1);

	        // Enable AMP 
	        amp_set_mode(1);

            // Start Ping-Pong
            MRAM_start_pp (/*mrm_prefix*/MRM_ADDR, /*start_pid*/goc_data&0x1FFFF, /*num_pages*/goc_data_ext, /*wait_for_irq*/0);

	        // Enable ADC 
	        adc_set_mode(1);

            // Wait for IRQ from MRAM
            WFI();

	        // Stop ADC 
	        adc_set_mode(0);

	        // Disable AMP 
	        amp_set_mode(0);

	        // Disable SAFR CLK
	        safr_set_mode(0);

	        // Turn off LDO
	        ldo_set_mode(0);

            // Go to sleep
			operation_sleep_notimer();	
        }
        //------------------------------------------------------------------------
        // goc_data = 0x30xnnnnn: Read the MRAM and put the data onto MBus
        //          goc_data[16:0]: Start Page ID [0 - 98303]
        //          goc_data_ext  : Number of pages to be read.
        //                          Valid Range: [0, 98303]
        //                          'goc_data_ext = 0' translates to 98304 pages (i.e., entire MRAM)
        //                          Each page is 1024 bits (= 15.625ms @ 64kbps)
        //------------------------------------------------------------------------
        else if (goc_data_3 == 0x30) {
            MRAM_read (/*mrm_prefix*/MRM_ADDR, /*start_pid*/goc_data&0x1FFFF, /*num_pages*/goc_data_ext);
        }




        //------------------------------------------------------------------------
        // goc_data = 0xFFxxxxxx: Power Control
        //------------------------------------------------------------------------
		else if(goc_data_3 == 0xFF){
            //------------------------------------------------------------------------
            // goc_data = 0xFFxxxx00: Go to sleep
            //------------------------------------------------------------------------
            if (goc_data_0 == 0x00) {
			    operation_sleep_notimer();	
            }
            //------------------------------------------------------------------------
            // goc_data = 0xFFxxxx01: Re-run operation_init()
            //------------------------------------------------------------------------
		    else if(goc_data_0 == 0x01){
		    	operation_init();
		    }
		}
		
	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xEE, 0x00000E2D);
	#endif
	}
	
    operation_sleep_notimer();
    return 0;
}
