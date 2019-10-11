//*******************************************************************
//Author: Kyojin Choo
//Description: initial VIM program upload to FLP
//			
//*******************************************************************
#include "PREv18.h"
#include "PREv18_RF.h"
#include "mbus.h"
#include "FLPv3L_RF.h"
#include "VIMv1_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// VIMstack_v0_t1 short prefix
#define PRE_ADDR 	0x1		//set to 0x0 if not present in stack
#define MRR_ADDR 	0x3		//set to 0x0 if not present in stack
#define VIM_ADDR	0x4		//set to 0x0 if not present in stack
#define FLP_ADDR	0x5		//set to 0x0 if not present in stack
#define PMU_ADDR 	0x6		//set to 0x0 if not present in stack

// System parameters
#define	MBUS_DELAY 	300 // Amount of delay between successive messages; 100: 6-7ms

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t * program_start_pointer = 8008;
volatile uint32_t enumerated;
volatile uint32_t stack_state;

volatile prev18_r0B_t prev18_r0B = PREv18_R0B_DEFAULT;
volatile prev18_r1C_t prev18_r1C = PREv18_R1C_DEFAULT;
volatile prev18_r1F_t prev18_r1F = PREv18_R1F_DEFAULT;

// VIM registers
volatile vimv1_r07_t VIMv1_r07_tmr_ctrl 	= VIMv1_R07_DEFAULT;
volatile vimv1_r08_t VIMv1_r08_tmr_reg 		= VIMv1_R08_DEFAULT;
volatile vimv1_r09_t VIMv1_r09_pmu_ref_ctrl 	= VIMv1_R09_DEFAULT;
volatile vimv1_r0C_t VIMv1_r0C_pmu_ref_irefreg1	= VIMv1_R0C_DEFAULT;
volatile vimv1_r40_t VIMv1_r40_dig_debug0 	= VIMv1_R40_DEFAULT;
volatile vimv1_r41_t VIMv1_r41_dig_ctrl0 	= VIMv1_R41_DEFAULT;
volatile vimv1_r43_t VIMv1_r43_dig_mbusex1 	= VIMv1_R43_DEFAULT;

// PMU variables
volatile uint32_t read_data_batadc;

//***************************************************
// Sleep Functions
//***************************************************
static void operation_sleep(void);

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

//***************************************************
//FLP functions
//***************************************************
static void flp_fail(uint32_t id)
{
	mbus_write_message32(0xE2, id);
	delay(10000);
	mbus_sleep_all();
	while(1);
}

static void FLASH_turn_on() {
	set_halt_until_mbus_rx();
	mbus_remote_register_write (FLP_ADDR , 0x11 , 0x00002F);
	set_halt_until_mbus_tx();
	
	if (*REG1 != 0xB5) flp_fail (0);
}

static void FLASH_turn_off() {
	set_halt_until_mbus_rx();
	mbus_remote_register_write (FLP_ADDR , 0x11 , 0x00002D);
	set_halt_until_mbus_tx();
	
	if (*REG1 != 0xBB) flp_fail (1);
}

static void FLASH_erase_range(uint32_t id_begin, uint32_t id_end) {
	// Erase Flash
	uint32_t page_start_addr = 0;
	uint32_t idx;
	for (idx=id_begin; idx<id_end; idx++){ // All Pages (8Mb) are erased. except VIM program part (total page address 0~1023)
		page_start_addr = (idx << (10-2));
		mbus_remote_register_write (FLP_ADDR, 0x08, page_start_addr); // Set FLSH_START_ADDR
		set_halt_until_mbus_rx();
		mbus_remote_register_write (FLP_ADDR, 0x09, 1 << 5 | 4 << 1 | 1 ); // Page Erase
		set_halt_until_mbus_tx();
		if (*REG1 != 0x4F) flp_fail((0xFFFF << 16) | idx);
	}
}

static void FLASH_moveToFlash_fast(uint32_t SRAMaddr, uint32_t FLASHaddr, uint32_t length){
	mbus_remote_register_write (FLP_ADDR , 0x07 , SRAMaddr);
	mbus_remote_register_write (FLP_ADDR , 0x08 , FLASHaddr);
	set_halt_until_mbus_rx();
	mbus_remote_register_write (FLP_ADDR , 0x09 , (length-1) << 6 | 1 << 5 | 3 << 1 | 1); //length, IRQ_EN, cmd (sramToFlash, fast), go!
	set_halt_until_mbus_tx();

	if (*REG1 != 0x5d) flp_fail (2);
}

static void FLASH_defaultmode (){
	mbus_remote_register_write (FLP_ADDR , 0x19 , 0x1<<11 | 0xf<<7 | 0x0<<3 | 0x6); // Voltage Clamper Tuning
	mbus_remote_register_write (FLP_ADDR , 0x0F , 0x10<<8 | 0x01); // Flash interrupt target register addr: REG0 -> REG1
	//default speed 
	mbus_remote_register_write (FLP_ADDR , 0x00 , 0x1f<<19 | 0x2<<13 | 0x4<<7 | 0x09); // Tcyc_read, T3us, T5us, T10us
	mbus_remote_register_write (FLP_ADDR , 0x01 , 0x0008<<8 | 0x09); // Tcyc_prog, Tprog
	mbus_remote_register_write (FLP_ADDR , 0x02 , 0x1400); // Terase
	mbus_remote_register_write (FLP_ADDR , 0x03 , 0x03e8<<10 | 0x031); // Thvcp
	mbus_remote_register_write (FLP_ADDR , 0x04 , 0x3e8<<12 | 0x3e8); // Tmvcp
	mbus_remote_register_write (FLP_ADDR , 0x05 , 0x007cf); // Tcap
	mbus_remote_register_write (FLP_ADDR , 0x06 , 0x01f3f); // Tvref
	mbus_remote_register_write (FLP_ADDR , 0x18 , 0xC<<2 | 0x1); // DIV
}

static void FLASH_defaultmode_2xclock (){
	mbus_remote_register_write (FLP_ADDR , 0x19 , 0x1<<11 | 0xf<<7 | 0x0<<3 | 0x6); // Voltage Clamper Tuning
	mbus_remote_register_write (FLP_ADDR , 0x0F , 0x10<<8 | 0x01); // Flash interrupt target register addr: REG0 -> REG1
	//default speed x 2
	mbus_remote_register_write (FLP_ADDR , 0x00 , 0x1f<<19 | 0x04<<13 | 0x08<<7 | 0x13); // Tcyc_read, T3us, T5us, T10us
	mbus_remote_register_write (FLP_ADDR , 0x01 , 0x0004<<8 | 0x13); // Tcyc_prog, Tprog
	mbus_remote_register_write (FLP_ADDR , 0x02 , 0x2800); // Terase
	mbus_remote_register_write (FLP_ADDR , 0x03 , 0x07d0<<10 | 0x062); // Thvcp
	mbus_remote_register_write (FLP_ADDR , 0x04 , 0x7d0<<12 | 0x7d0); // Tmvcp
	mbus_remote_register_write (FLP_ADDR , 0x05 , 0x00f9e); // Tcap
	mbus_remote_register_write (FLP_ADDR , 0x06 , 0x03e7e); // Tvref
	mbus_remote_register_write (FLP_ADDR , 0x18 , 0x9<<2 | 0x1); // DIV
}

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

inline static void pmu_set_adc_period(uint32_t val){
	// PMU_CONTROLLER_DESIRED_STATE Active
	// Updated for PMUv9
	pmu_reg_write(0x3C,
		((1 << 0) //state_sar_scn_on
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
		((1 << 0) //state_sar_scn_on
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

	// The first register write to PMU needs to be repeated
	// Register 0x16: V1P2 Active
	pmu_reg_write(0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (r << 9)// Frequency multiplier R
		| (l_1p2 << 5)// Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	pmu_reg_write(0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (r << 9)// Frequency multiplier R
		| (l_1p2 << 5)// Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	// Register 0x18: V3P6 Active 
	pmu_reg_write(0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (r << 9)// Frequency multiplier R
		| (l << 5)// Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	// Register 0x1A: V0P6 Active
	pmu_reg_write(0x1A,
		( (0 << 13) // Enable main feedback loop
		| (r << 9)// Frequency multiplier R
		| (l << 5)// Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));

}

inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

	// Register 0x17: V3P6 Sleep
	pmu_reg_write(0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (r << 9)// Frequency multiplier R
		| (l << 5)// Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	// Register 0x15: V1P2 Sleep
	pmu_reg_write(0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (r << 9)// Frequency multiplier R
		| (l_1p2 << 5)// Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	// Register 0x19: V0P6 Sleep
	pmu_reg_write(0x19,
		( (0 << 13) // Enable main feedback loop
		| (r << 9)// Frequency multiplier R
		| (l << 5)// Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));

}

inline static void pmu_set_sleep_radio(){
	pmu_set_sleep_clk(0xF,0xA,0x5,0xF/*V1P2*/);
}

inline static void pmu_adc_reset_setting(){
	// PMU ADC will be automatically reset when system wakes up
	// PMU_CONTROLLER_DESIRED_STATE Active
	// Updated for PMUv9
	pmu_reg_write(0x3C,
		((1 << 0) //state_sar_scn_on
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
		((1 << 0) //state_sar_scn_on
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

inline static void pmu_adc_enable_sleep(){
	// PMU ADC will be automatically reset when system wakes up
	// PMU_CONTROLLER_DESIRED_STATE Sleep
	// Updated for PMUv9
	pmu_reg_write(0x3B,
		((1 << 0) //state_sar_scn_on
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


inline static void pmu_adc_read_latest(){
	// Grab latest PMU ADC readings
	// PMU register read is handled differently
	pmu_reg_write(0x00,0x03);
	// Updated for PMUv9
	read_data_batadc = *((volatile uint32_t *) REG0) & 0xFF;

}

inline static void pmu_reset_solar_short(){
	// Updated for PMUv9
	pmu_reg_write(0x0E, 
		( (0 << 12) // 1: solar short by latched vbat_high (new); 0: follow [10] setting
		| (1 << 11) // Reset of vbat_high latch for [12]=1
		| (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
		| (1 << 9)// Enables override setting [8]
		| (0 << 8)// Turn on the harvester-inhibiting switch
		| (1 << 4)// clamp_tune_bottom (increases clamp thresh)
		| (0) 		// clamp_tune_top (decreases clamp thresh)
	));
	pmu_reg_write(0x0E, 
		( (0 << 12) // 1: solar short by latched vbat_high (new); 0: follow [10] setting
		| (1 << 11) // Reset of vbat_high latch for [12]=1
		| (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
		| (0 << 9)// Enables override setting [8]
		| (0 << 8)// Turn on the harvester-inhibiting switch
		| (1 << 4)// clamp_tune_bottom (increases clamp thresh)
		| (0) 		// clamp_tune_top (decreases clamp thresh)
	));
}

//*******************************************************************
// INTERRUPT HANDLERS (Updated for PRCv17) - IRQ
//*******************************************************************

void handler_ext_int_gocep(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_wakeup (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0 (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1 (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2 (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3 (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg4 (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_wakeup(void) { // WAKE-UP
	*NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
	// Report who woke up
	delay(MBUS_DELAY);
	mbus_write_message32(0xAA,*SREG_WAKEUP_SOURCE); // 0x1: GOC; 0x2: PRC Timer; 0x10: SNT
}
void handler_ext_int_gocep(void) { // GOCEP
	*NVIC_ICPR = (0x1 << IRQ_GOCEP);
}
void handler_ext_int_timer32(void) { // TIMER32
	*NVIC_ICPR = (0x1 << IRQ_TIMER32);
	*REG1 = *TIMER32_CNT;
	*REG2 = *TIMER32_STAT;
	*TIMER32_STAT = 0x0;
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


//********************************************************************
// MAIN function starts here             
//********************************************************************
static void prc_init() {
	//MBUS timeout disable
	prev18_r1F.MBUS_WATCHDOG_THRESHOLD = 0x0;
	*REG_MBUS_WD = prev18_r1F.as_int;
	// Set CPU & Mbus Clock Speeds
	prev18_r0B.CLK_GEN_RING = 0x1; // Default 0x1
	prev18_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
	prev18_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
	prev18_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
	prev18_r0B.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x6
	*REG_CLKGEN_TUNE = prev18_r0B.as_int;
	prev18_r1C.SRAM0_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
	prev18_r1C.SRAM0_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
	prev18_r1C.SRAM0_USE_INVERTER_SA= 0; 
	*REG_SRAM0_TUNE = prev18_r1C.as_int;
	//Enumerate & Initialize Registers
	enumerated = 0x56491180; // 0x5649 is VI in ascii
}

static void flp_init() {
	FLASH_defaultmode_2xclock();
}

static void pmu_init(){
	// PMU Settings ----------------------------------------------
	pmu_set_active_clk(
				0xf, //r
				0xf, //l
				0x3f,//base (max = 0x3f)
				0xd  //l_1p2
			);

	pmu_set_sleep_clk(
				0xf,
				0xf,
				0x3f,
				0xf
			);

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
		| (0x45) 		// Binary converter's conversion ratio (7'h00)
	));

	pmu_set_sar_override(
				0x3C //SAR value
			);

	//pmu_set_adc_period(1); // 0x100 about 1 min for 1/2/1 1P2 setting
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
	pmu_adc_enable_sleep(); //Enable PMU ADC measurement in sleep mode
}

static void vim_init(){

	VIMv1_r07_tmr_ctrl.C_TMR_SLOWFAST 			= 0;
	VIMv1_r07_tmr_ctrl.C_TMR_FASTDIV 			= 0;
	VIMv1_r07_tmr_ctrl.R_TMR_PADMONITOR			= 0;
	VIMv1_r07_tmr_ctrl.R_TMR_DEBUGSLOW			= 0;
	VIMv1_r07_tmr_ctrl.R_TMR_DEBUGFAST			= 0;
	VIMv1_r07_tmr_ctrl.R_TMR_DEBUGDIV			= 0;
	VIMv1_r07_tmr_ctrl.R_TMR_PADTESTCLK			= 0;
	VIMv1_r07_tmr_ctrl.R_TMR_MASTEREN			= 0;
	mbus_remote_register_write(VIM_ADDR,0x07,VIMv1_r07_tmr_ctrl.as_int);

	VIMv1_r08_tmr_reg.R_TMR_DIVRATIO			= 0;
	VIMv1_r08_tmr_reg.R_TMR_FASTOSCSPEED			= 0;
	VIMv1_r08_tmr_reg.R_TMR_SLOWOSCSPEED			= 0;
	mbus_remote_register_write(VIM_ADDR,0x08,VIMv1_r08_tmr_reg.as_int);

	VIMv1_r09_pmu_ref_ctrl.C_PMU_PGP_EN			= 0;
	VIMv1_r09_pmu_ref_ctrl.C_PMU_PGA_EN			= 0;
	VIMv1_r09_pmu_ref_ctrl.C_PMU_IREF_ENABLE		= 0;
	VIMv1_r09_pmu_ref_ctrl.C_PMU_IREF_BIASEN		= 0;
	VIMv1_r09_pmu_ref_ctrl.C_PMU_IVREFIDLE_ENABLE		= 0;
	VIMv1_r09_pmu_ref_ctrl.C_PMU_IVREFIDLE_CAPCONNECT	= 0;
	VIMv1_r09_pmu_ref_ctrl.P_PMU_IVREFIDLE_START		= 0;
	mbus_remote_register_write(VIM_ADDR,0x09,VIMv1_r09_pmu_ref_ctrl.as_int);

	VIMv1_r0C_pmu_ref_irefreg1.R_PMU_IREF_LDON_AMP		= 0;
	VIMv1_r0C_pmu_ref_irefreg1.R_PMU_IREF_VREF		= 0;
	VIMv1_r0C_pmu_ref_irefreg1.R_PMU_IREF_TMR		= 0;
	VIMv1_r0C_pmu_ref_irefreg1.R_PMU_IREF_PIXBIAS		= 0;
	mbus_remote_register_write(VIM_ADDR,0x0C,VIMv1_r0C_pmu_ref_irefreg1.as_int);

	VIMv1_r40_dig_debug0.ENABLE_LC_MEM			= 0;
	VIMv1_r40_dig_debug0.ENABLE_DEBUG			= 0;
	VIMv1_r40_dig_debug0.DEBUG_CLK_SOURCE			= 0;
	mbus_remote_register_write(VIM_ADDR,0x40,VIMv1_r40_dig_debug0.as_int);

	VIMv1_r41_dig_ctrl0.CTRL_SLEEP				= 1;
	VIMv1_r41_dig_ctrl0.CTRL_ISOLATE			= 1;
	VIMv1_r41_dig_ctrl0.CTRL_RESET				= 1;
	mbus_remote_register_write(VIM_ADDR,0x41,VIMv1_r41_dig_ctrl0.as_int);

	VIMv1_r43_dig_mbusex1.LC_CLK_RING			= 2;
	VIMv1_r43_dig_mbusex1.LC_CLK_DIV			= 0;
	VIMv1_r43_dig_mbusex1.MBUS_IGNORE_RX_FAIL		= 1;
	mbus_remote_register_write(VIM_ADDR,0x43,VIMv1_r43_dig_mbusex1.as_int);

}

static void operation_init(void){
	//Enumeration
	#if MRR_ADDR != 0
	mbus_enumerate(MRR_ADDR);
	delay(MBUS_DELAY);
	#endif
	#if VIM_ADDR != 0
	mbus_enumerate(VIM_ADDR);
	delay(MBUS_DELAY);
	#endif
	#if FLP_ADDR != 0
 	mbus_enumerate(FLP_ADDR);
	delay(MBUS_DELAY);
	#endif
	#if PMU_ADDR != 0
 	mbus_enumerate(PMU_ADDR);
	delay(MBUS_DELAY);
	#endif
	
	#if PRC_ADDR != 0
	prc_init();
	#endif
	#if PMU_ADDR != 0
	pmu_init();
	#endif
	#if MRR_ADDR != 0
	//mrr_init();
	#endif
	#if VIM_ADDR != 0
	vim_init();
	#endif
	#if FLP_ADDR != 0
	flp_init();
	#endif
}

void main(){
	//if (*program_start_pointer == 0xdeadbeef){ while(1); }
	//CPU timeout disable
	*TIMERWD_GO = 0;

	//Enable interrupt
	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3)| (1 << IRQ_REG4);

	//Initialize stack
	operation_init();

	FLASH_turn_on();
	delay(MBUS_DELAY);

	//FLASH_erase_range(0, 1024); // erase all
	delay(MBUS_DELAY);
	
	//sleep
	FLASH_turn_off();
	while(1);
}


