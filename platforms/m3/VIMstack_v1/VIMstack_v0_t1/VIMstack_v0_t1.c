//*******************************************************************
//Author: Kyojin Choo
//Description: VIM program version 0 trial 1
//
//*******************************************************************
#include "PREv18.h"
#include "PREv18_RF.h"
#include "mbus.h"
#include "PMUv7_RF.h"
#include "MRRv7_RF.h"
#include "FLPv3L_RF.h"
#include "VIMv1_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// VIMstack short prefix
#define PRE_ADDR 			0x1		//set to 0x0 if not present in stack
#define MRR_ADDR 			0x3		//set to 0x0 if not present in stack
#define VIM_ADDR			0x4		//set to 0x0 if not present in stack
#define FLP_ADDR			0x5		//set to 0x0 if not present in stack
#define PMU_ADDR 			0x6		//set to 0x0 if not present in stack

// VIMstack states
#define VIM_mode_fullsequence		0x0
#define VIM_mode_mdnormalloop		0x1
#define VIM_mode_mdonly			0x4
#define VIM_mode_mdonlysingle		0x5
#define VIM_mode_normalonly		0x6
#define VIM_mode_normalonlysingle	0x7

// VIM variables
//#define VIM_prog_FLPmemstartaddr 	4*1024*4 	// [1*1024*4] multiple of 1024 (page size)
#define VIM_prog_CALmemstartaddr	4*1024*4
#define VIM_prog_MDCONFmemstartaddr	1*1024*4
#define VIM_prog_MDmemstartaddr		1*1024*4+100*4
#define VIM_prog_NORCONFmemstartaddr	2*1024*4
#define VIM_prog_NORmemstartaddr	2*1024*4+100*4
#define VIM_image_memstartaddr		10*1024*4
#define	VIM_CONF_DELAY 			10000 		// [10000] Amount of delay between successive messages; 10000: ~1000ms
#define VIM_mbus_streamsize		99*3 		// [256*3] == 2048 pixel values, need to be multiple of 3 because pixel value is 12b
#define VIM_mbus_streamchunk		528	 	// [204] = floor(VIM_image_size / 4 / VIM_mbus_streamsize)
#define VIM_NOR_opcount			0		// [1] 0 = run infinitely

// VIM switches
#define VIM_flp_streammode		1		//(1) 0 = no stream (debug mode), 1 = stream mode, 2 = pingpong mode (does not work yet)
#define VIM_flp_imageerase		1		//(1) 0 = no erase (debug mode), 1 = erase at end of normal + stream & at end of calib
#define VIM_flp_imageerase_callback	0		// 0 = no callback, 1 = callback using MBUS after erase, 2 = callback and do correctnesstest & repeat
#define VIM_mrr_stream			0		// 0 = no mrr stream (debug mode), 1 = mrr stream after Normal capture
#define VIM_mbus_stream			1		//(1) 0 = no mbus stream, 1 = mbus stream after Normal capture (debug mode)

// System parameters
#define	MBUS_DELAY 	300 				// Amount of delay between successive messages; 3000: ~300ms
#define TIMER32_VAL 	0x50000 			// 0x20000 about 1 sec with Y5 run default clock (PRCv17)

// Radio configurations
#define RADIO_DATA_LENGTH 192
#define WAKEUP_PERIOD_RADIO_INIT 10 			// About 2 sec (PRCv17)

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t * program_start_pointer 	= 8008;
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t stack_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t exec_count_irq;
volatile uint32_t PMU_ADC_3P0_VAL;
volatile uint32_t pmu_sar_conv_ratio_val;
volatile uint32_t read_data_batadc;
volatile uint32_t sleep_time_prev;
volatile uint32_t sleep_time_threshold_factor;
volatile uint32_t wakeup_period_calc_factor;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_SNT; 

volatile uint32_t wakeup_timer_option;
volatile uint32_t temp_alert;

volatile uint32_t radio_tx_numdata;
volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t mrr_freq_hopping;
volatile uint32_t mrr_freq_hopping_step;
volatile uint32_t mrr_cfo_val_fine_min;
volatile uint32_t RADIO_PACKET_DELAY;
volatile uint32_t radio_packet_count;

//********************************************************************
// VIM stack specific variables
//********************************************************************
volatile uint32_t * VIM_stack_initialized	= 8004;
volatile uint32_t * VIM_mode			= 8000;
volatile uint32_t VIM_idle			= 1;

// VIM program in FLP start address
//volatile uint32_t VIM_CAL_prog_FLPmemaddr	=VIM_prog_FLPmemstartaddr+0*1024*4; //len=500*4
//volatile uint32_t VIM_MDconf_prog_FLPmemaddr	=VIM_prog_FLPmemstartaddr+1*1024*4; //len=100*4
//volatile uint32_t VIM_MD_prog_FLPmemaddr	=VIM_prog_FLPmemstartaddr+1*1024*4+100*4; //len=500*4
//volatile uint32_t VIM_NORconf_prog_FLPmemaddr	=VIM_prog_FLPmemstartaddr+2*1024*4; //len=100*4
//volatile uint32_t VIM_NOR_prog_FLPmemaddr	=VIM_prog_FLPmemstartaddr+2*1024*4+100*4; //len=500*4
//volatile uint32_t VIM_IMAGE_start_FLPmemaddr 	=VIM_prog_FLPmemstartaddr+4*1024*4+4;
volatile uint32_t VIM_CAL_prog_FLPmemaddr	=VIM_prog_CALmemstartaddr;
volatile uint32_t VIM_MDconf_prog_FLPmemaddr	=VIM_prog_MDCONFmemstartaddr;
volatile uint32_t VIM_MD_prog_FLPmemaddr	=VIM_prog_MDmemstartaddr;
volatile uint32_t VIM_NORconf_prog_FLPmemaddr	=VIM_prog_NORCONFmemstartaddr;
volatile uint32_t VIM_NOR_prog_FLPmemaddr	=VIM_prog_NORmemstartaddr;
volatile uint32_t VIM_IMAGE_start_FLPmemaddr 	=VIM_image_memstartaddr;
volatile uint32_t VIM_IMAGE_linelength	 	=792*12/32; //in 32b words
volatile uint32_t VIM_IMAGE_numrow	 	=528; 
volatile uint32_t VIM_IMAGE_numcol	 	=792; 
volatile uint32_t VIM_IMAGE_size		=(12*792*528)/8; //in bytes
volatile uint32_t VIM_IMAGE_NOR_capture_cnt	=0;

// VIM variables
volatile uint32_t VIM_interrupt_code;

// VIM registers
volatile vimv1_r07_t VIMv1_r07_tmr_ctrl 	= VIMv1_R07_DEFAULT;
volatile vimv1_r08_t VIMv1_r08_tmr_reg 		= VIMv1_R08_DEFAULT;
volatile vimv1_r09_t VIMv1_r09_pmu_ref_ctrl 	= VIMv1_R09_DEFAULT;
volatile vimv1_r0C_t VIMv1_r0C_pmu_ref_irefreg1	= VIMv1_R0C_DEFAULT;
volatile vimv1_r40_t VIMv1_r40_dig_debug0 	= VIMv1_R40_DEFAULT;
volatile vimv1_r41_t VIMv1_r41_dig_ctrl0 	= VIMv1_R41_DEFAULT;
volatile vimv1_r43_t VIMv1_r43_dig_mbusex1 	= VIMv1_R43_DEFAULT;

volatile prev18_r0B_t prev18_r0B = PREv18_R0B_DEFAULT;
volatile prev18_r1C_t prev18_r1C = PREv18_R1C_DEFAULT;
volatile prev18_r1F_t prev18_r1F = PREv18_R1F_DEFAULT;

volatile mrrv7_r00_t mrrv7_r00 = MRRv7_R00_DEFAULT;
volatile mrrv7_r01_t mrrv7_r01 = MRRv7_R01_DEFAULT;
volatile mrrv7_r02_t mrrv7_r02 = MRRv7_R02_DEFAULT;
volatile mrrv7_r03_t mrrv7_r03 = MRRv7_R03_DEFAULT;
volatile mrrv7_r04_t mrrv7_r04 = MRRv7_R04_DEFAULT;
volatile mrrv7_r05_t mrrv7_r05 = MRRv7_R05_DEFAULT;
volatile mrrv7_r07_t mrrv7_r07 = MRRv7_R07_DEFAULT;
volatile mrrv7_r10_t mrrv7_r10 = MRRv7_R10_DEFAULT;
volatile mrrv7_r11_t mrrv7_r11 = MRRv7_R11_DEFAULT;
volatile mrrv7_r12_t mrrv7_r12 = MRRv7_R12_DEFAULT;
volatile mrrv7_r13_t mrrv7_r13 = MRRv7_R13_DEFAULT;
volatile mrrv7_r14_t mrrv7_r14 = MRRv7_R14_DEFAULT;
volatile mrrv7_r15_t mrrv7_r15 = MRRv7_R15_DEFAULT;
volatile mrrv7_r16_t mrrv7_r16 = MRRv7_R16_DEFAULT;
volatile mrrv7_r1E_t mrrv7_r1E = MRRv7_R1E_DEFAULT;
volatile mrrv7_r1F_t mrrv7_r1F = MRRv7_R1F_DEFAULT;


//***************************************************
// Sleep Functions
//***************************************************
static void operation_sleep(void);
static void operation_sleep_noirqreset(void);
static void operation_sleep_notimer(void);

static void vim_ivref_on ();
static void vim_tmr_on_slow ();
static void vim_start_ctrl ();
static void vim_stop_ctrl ();
static void vim_load_program (uint32_t program_addr_in_flash, uint32_t program_length);
static void vim_md_mode_start ();
static void vim_normal_mode_start ();
static void vim_calib_mode_start ();
static void vim_program_debug();

static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data);
static void pmu_set_sar_override(uint32_t val);
inline static void pmu_set_adc_period(uint32_t val);
inline static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);
inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);
inline static void pmu_set_sleep_radio();
inline static void pmu_set_sleep_low();
inline static void pmu_adc_reset_setting();
inline static void pmu_adc_disable();
inline static void pmu_adc_enable_sleep();
inline static void pmu_adc_read_latest();
inline static void pmu_reset_solar_short();

static void radio_power_on();
static void radio_power_off();
static void mrr_configure_pulse_width_long();
static void send_radio_data_mrr_sub1();
static void send_radio_data_mrr(uint32_t last_packet, uint8_t radio_packet_prefix, uint16_t radio_data_2, uint32_t radio_data_1, uint32_t radio_data_0);

static void flp_fail(uint32_t id);
static void FLASH_turn_on();
static void FLASH_turn_off();
static void FLASH_erase_range(uint32_t id_begin, uint32_t id_end);
static void FLASH_str_ready();
static void FLASH_str_go();
static void FLASH_pp_ready();
static void FLASH_pp_on();
static void FLASH_pp_off();
static void FLASH_moveToFlash_fast(uint32_t SRAMaddr, uint32_t FLASHaddr, uint32_t length);
static void FLASH_moveToFlash(uint32_t SRAMaddr, uint32_t FLASHaddr, uint32_t length);
static void FLASH_moveToSram(uint32_t FLASHaddr, uint32_t SRAMaddr, uint32_t length);
static void FLASH_outputimage_mbus();
static void FLASH_fastmode();
static void FLASH_defaultmode();
static void FLASH_defaultmode_2xclock();

static void debug();


//************************************
// VIM Related Functions
//************************************
static void vim_ivref_on () {
	VIMv1_r09_pmu_ref_ctrl.P_PMU_IVREFIDLE_START = 1;
	VIMv1_r09_pmu_ref_ctrl.C_PMU_IVREFIDLE_ENABLE = 1;
	VIMv1_r09_pmu_ref_ctrl.C_PMU_IREF_ENABLE = 1;
	VIMv1_r09_pmu_ref_ctrl.C_PMU_IREF_BIASEN = 1;
	mbus_remote_register_write(VIM_ADDR,0x09,VIMv1_r09_pmu_ref_ctrl.as_int);

	VIMv1_r09_pmu_ref_ctrl.P_PMU_IVREFIDLE_START = 0;
	VIMv1_r09_pmu_ref_ctrl.C_PMU_IVREFIDLE_CAPCONNECT = 1;
	mbus_remote_register_write(VIM_ADDR,0x09,VIMv1_r09_pmu_ref_ctrl.as_int);
}

static void vim_tmr_on_slow () {
	VIMv1_r0C_pmu_ref_irefreg1.R_PMU_IREF_TMR = 4;
	mbus_remote_register_write(VIM_ADDR,0x0C,VIMv1_r0C_pmu_ref_irefreg1.as_int);

	VIMv1_r08_tmr_reg.R_TMR_SLOWOSCSPEED = 4;
	mbus_remote_register_write(VIM_ADDR,0x08,VIMv1_r08_tmr_reg.as_int);

	VIMv1_r07_tmr_ctrl.R_TMR_MASTEREN = 1;
	//VIMv1_r07_tmr_ctrl.R_TMR_PADMONITOR = 1; //debug
	mbus_remote_register_write(VIM_ADDR,0x07,VIMv1_r07_tmr_ctrl.as_int);
}

static void vim_start_ctrl () {
	VIMv1_r41_dig_ctrl0.CTRL_SLEEP = 0;
	mbus_remote_register_write(VIM_ADDR,0x41,VIMv1_r41_dig_ctrl0.as_int);

	VIMv1_r41_dig_ctrl0.CTRL_ISOLATE = 0;
	mbus_remote_register_write(VIM_ADDR,0x41,VIMv1_r41_dig_ctrl0.as_int);

	VIMv1_r41_dig_ctrl0.CTRL_RESET = 0;
	mbus_remote_register_write(VIM_ADDR,0x41,VIMv1_r41_dig_ctrl0.as_int);
}

static void vim_stop_ctrl () {
	VIMv1_r41_dig_ctrl0.CTRL_RESET = 1;
	mbus_remote_register_write(VIM_ADDR,0x41,VIMv1_r41_dig_ctrl0.as_int);

	VIMv1_r41_dig_ctrl0.CTRL_ISOLATE = 1;
	mbus_remote_register_write(VIM_ADDR,0x41,VIMv1_r41_dig_ctrl0.as_int);

	VIMv1_r41_dig_ctrl0.CTRL_SLEEP = 1;
	mbus_remote_register_write(VIM_ADDR,0x41,VIMv1_r41_dig_ctrl0.as_int);
}

static void vim_instwrite_enable () {
	VIMv1_r41_dig_ctrl0.CTRL_SLEEP = 0;
	mbus_remote_register_write(VIM_ADDR,0x41,VIMv1_r41_dig_ctrl0.as_int);

	VIMv1_r41_dig_ctrl0.CTRL_ISOLATE = 0;
	mbus_remote_register_write(VIM_ADDR,0x41,VIMv1_r41_dig_ctrl0.as_int);
	
	VIMv1_r40_dig_debug0.ENABLE_LC_MEM = 1;
	mbus_remote_register_write(VIM_ADDR,0x40,VIMv1_r40_dig_debug0.as_int);
}

static void vim_instwrite_disable () {
	VIMv1_r40_dig_debug0.ENABLE_LC_MEM = 0;
	mbus_remote_register_write(VIM_ADDR,0x40,VIMv1_r40_dig_debug0.as_int);

	VIMv1_r41_dig_ctrl0.CTRL_ISOLATE = 1;
	mbus_remote_register_write(VIM_ADDR,0x41,VIMv1_r41_dig_ctrl0.as_int);

	VIMv1_r41_dig_ctrl0.CTRL_SLEEP = 1;
	mbus_remote_register_write(VIM_ADDR,0x41,VIMv1_r41_dig_ctrl0.as_int);
}

static void vim_md_mode_start () {
	VIM_idle = 1;
	//TODO : add PMU
	vim_stop_ctrl();
	vim_tmr_on_slow();
	FLASH_turn_on();
	FLASH_moveToSram((VIM_MDconf_prog_FLPmemaddr)/4, 0x0, 600);
	{
		//Load MD config program
		vim_instwrite_enable();
		mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR,	(uint32_t*) 0x0, VIM_ADDR,(uint32_t*) 0x0, 99-1);
		vim_instwrite_disable();
		vim_start_ctrl();
		delay(VIM_CONF_DELAY);
		vim_stop_ctrl();
	}
	{
		//Load MD program
		vim_instwrite_enable();
		mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR,	(uint32_t*) 400, VIM_ADDR,(uint32_t*) 0x0, 499-1);
		vim_instwrite_disable();
		FLASH_turn_off();
		vim_start_ctrl();
	}
}

static void vim_md_mode_end() {
	vim_stop_ctrl();
	VIM_idle = 0;
}

static void vim_normal_mode_start () {
	VIM_idle = 1;
	//TODO : add PMU
	vim_stop_ctrl();
	vim_tmr_on_slow();
	FLASH_turn_on();
	FLASH_moveToSram((VIM_NORconf_prog_FLPmemaddr)/4, 0x0, 600);
	{
		//Load normal config program
		vim_instwrite_enable();
		mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR,	(uint32_t*) 0x0, VIM_ADDR,(uint32_t*) 0x0, 99-1);
		vim_instwrite_disable();
		vim_start_ctrl();
		delay(VIM_CONF_DELAY);
		vim_stop_ctrl();
	}
	{
		//Load normal program
		vim_instwrite_enable();
		mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR,	(uint32_t*) 400, VIM_ADDR,(uint32_t*) 0x0, 499-1);
		vim_instwrite_disable();
		#if VIM_flp_streammode == 0
		 FLASH_turn_off();
		 vim_start_ctrl();
		#elif VIM_flp_streammode == 1
		 //FLP - stream mode
		 FLASH_fastmode();
		 FLASH_str_ready(0x0);
		 vim_start_ctrl();
		 uint32_t i = 0;
		 for (i = 0; i < VIM_IMAGE_numrow; i++){
			FLASH_str_go(VIM_IMAGE_linelength); //length
			FLASH_moveToFlash_fast(0x0, VIM_IMAGE_start_FLPmemaddr/4 + i*VIM_IMAGE_linelength, VIM_IMAGE_linelength);
		 }
		#elif VIM_flp_streammode == 2
		 //FLP - pingpong mode
		 FLASH_fastmode();
		 FLASH_pp_ready();
		 FLASH_pp_on();
		 vim_start_ctrl();
		#endif
	}
}

static void vim_normal_mode_end() {
	vim_stop_ctrl();
	#if VIM_flp_streammode == 2
	 FLASH_pp_off();
	#endif
	#if VIM_mbus_stream == 1 && VIM_flp_streammode >= 1
	 FLASH_outputimage_mbus();
	#endif 
	FLASH_turn_off();
	FLASH_defaultmode_2xclock();
	#if VIM_mrr_stream == 1
	//TODO : add MRR stream
	#endif
	#if VIM_flp_imageerase == 1
	 FLASH_turn_on();
	 FLASH_erase_range(VIM_IMAGE_start_FLPmemaddr>>10, 1024);
	 FLASH_turn_off();
	#endif
	VIM_idle = 1;
}

static void vim_calib_mode_start () {
	VIM_idle = 0;
	//TODO : add PMU
	vim_stop_ctrl();
	vim_tmr_on_slow();
	FLASH_turn_on();
	FLASH_moveToSram((VIM_CAL_prog_FLPmemaddr)/4, 0x0, 500);
	{
		//Load calibrate program
		vim_instwrite_enable();
		mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR,	(uint32_t*) 0x0, VIM_ADDR,(uint32_t*) 0x0, 499-1);
		vim_instwrite_disable();
		FLASH_turn_off();
		vim_start_ctrl();
	}
}

static void vim_calib_mode_end() {
	vim_stop_ctrl();
	#if VIM_flp_imageerase == 1
	 FLASH_turn_on();
	 FLASH_erase_range(VIM_IMAGE_start_FLPmemaddr>>10, 1024);
	 FLASH_turn_off();
	#endif
	VIM_idle = 1;
}

static void vim_program_debug() {
	FLASH_turn_on();
	FLASH_moveToSram((VIM_CAL_prog_FLPmemaddr)/4, 0x0, 500);
	mbus_copy_mem_from_remote_to_any_bulk(
			FLP_ADDR,
			(uint32_t*) 0x0,
			PRE_ADDR,
			(uint32_t*) 0x1000,
			500-1);
	FLASH_moveToSram((VIM_MDconf_prog_FLPmemaddr)/4, 0x0, 600);
	mbus_copy_mem_from_remote_to_any_bulk(
			FLP_ADDR,
			(uint32_t*) 0x0,
			PRE_ADDR,
			(uint32_t*) 0x1000,
			100-1);
	//FLASH_moveToSram((VIM_MD_prog_FLPmemaddr)/4, 0x0, 600);
	mbus_copy_mem_from_remote_to_any_bulk(
			FLP_ADDR,
			(uint32_t*) 400,
			PRE_ADDR,
			(uint32_t*) 0x1000,
			500-1);
	FLASH_moveToSram((VIM_NORconf_prog_FLPmemaddr)/4, 0x0, 600);
	mbus_copy_mem_from_remote_to_any_bulk(
			FLP_ADDR,
			(uint32_t*) 0x0,
			PRE_ADDR,
			(uint32_t*) 0x1000,
			100-1);
	//FLASH_moveToSram((VIM_NOR_prog_FLPmemaddr)/4, 0x0, 600);
	mbus_copy_mem_from_remote_to_any_bulk(
			FLP_ADDR,
			(uint32_t*) 400,
			PRE_ADDR,
			(uint32_t*) 0x1000,
			500-1);
	FLASH_turn_off();
}

static void vim_poll_mode() {
	delay(MBUS_DELAY*50);
	VIM_IMAGE_NOR_capture_cnt=0; 
	while (true){
		delay(MBUS_DELAY);
		if (VIM_idle == 1) { break; }
	}
	if (*VIM_mode == VIM_mode_mdonly)		{ vim_md_mode_start(); }
	else if (*VIM_mode == VIM_mode_mdonlysingle)	{ vim_md_mode_start(); }
	else if (*VIM_mode == VIM_mode_normalonly)	{ vim_normal_mode_start(); }
	else if (*VIM_mode == VIM_mode_normalonlysingle){ vim_normal_mode_start(); }
	else if (*VIM_mode == VIM_mode_fullsequence)	{ vim_calib_mode_start(); }
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

//***************************************************
// MRR Functions
//***************************************************
static void radio_power_on(){
	// Turn off PMU ADC
	//pmu_adc_disable();

	// Need to speed up sleep pmu clock
	//pmu_set_sleep_radio();

	// Turn off Current Limter Briefly
	mrrv7_r00.MRR_CL_EN = 0;//Enable CL
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	// Set decap to parallel
	mrrv7_r03.MRR_DCP_S_OW = 0;//TX_Decap S (forced charge decaps)
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
	mrrv7_r03.MRR_DCP_P_OW = 1;//RX_Decap P 
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
	delay(MBUS_DELAY);

	// Set decap to series
	mrrv7_r03.MRR_DCP_P_OW = 0;//RX_Decap P 
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
	mrrv7_r03.MRR_DCP_S_OW = 1;//TX_Decap S (forced charge decaps)
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
	delay(MBUS_DELAY);

	// Current Limter set-up 
	mrrv7_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	radio_on = 1;
	
	// Turn on Current Limter
	mrrv7_r00.MRR_CL_EN = 1;//Enable CL
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
	
	// Release timer power-gate
	mrrv7_r04.RO_EN_RO_V1P2 = 1;//Use V1P2 for TIMER
	mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
	delay(MBUS_DELAY);
	
		// Turn on timer
	mrrv7_r04.RO_RESET = 0;//Release Reset TIMER
	mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
	delay(MBUS_DELAY);
	
	mrrv7_r04.RO_EN_CLK = 1; //Enable CLK TIMER
	mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
	delay(MBUS_DELAY);
	
	mrrv7_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
	mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
	
	// Release FSM Sleep
	mrrv7_r11.MRR_RAD_FSM_SLEEP = 0;// Power on BB
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
	delay(MBUS_DELAY*5); // Freq stab

}

static void radio_power_off(){
	// Need to restore sleep pmu clock

	// Enable PMU ADC
	//pmu_adc_enable_sleep();

	// Turn off Current Limter Briefly
	mrrv7_r00.MRR_CL_EN = 0;//Enable CL
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	// Current Limter set-up 
	mrrv7_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	// Turn on Current Limter
	mrrv7_r00.MRR_CL_EN = 1;//Enable CL
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	// Turn off everything
	mrrv7_r03.MRR_TRX_ISOLATEN = 0; //set ISOLATEN 0
	mbus_remote_register_write(MRR_ADDR,0x03,mrrv7_r03.as_int);

	mrrv7_r11.MRR_RAD_FSM_EN = 0;//Stop BB
	mrrv7_r11.MRR_RAD_FSM_RSTN = 0;//RST BB
	mrrv7_r11.MRR_RAD_FSM_SLEEP = 1;
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
	
	mrrv7_r04.RO_RESET = 1;//Release Reset TIMER
	mrrv7_r04.RO_EN_CLK = 0; //Enable CLK TIMER
	mrrv7_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
	mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
	
	// Enable timer power-gate
	mrrv7_r04.RO_EN_RO_V1P2 = 0;//Use V1P2 for TIMER
	mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
	
	radio_on = 0;
	radio_ready = 0;

}

static void mrr_configure_pulse_width_long(){

	mrrv7_r12.MRR_RAD_FSM_TX_PW_LEN = 24; //100us PW
	mrrv7_r13.MRR_RAD_FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
	mrrv7_r12.MRR_RAD_FSM_TX_PS_LEN = 49; // PW=PS 

	mbus_remote_register_write(MRR_ADDR,0x12,mrrv7_r12.as_int);
	mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);
}

static void send_radio_data_mrr_sub1(){

	// Use Timer32 as timeout counter
	wfi_timeout_flag = 0;
	config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

	// Turn on Current Limter
	mrrv7_r00.MRR_CL_EN = 1;
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	// Fire off data
	mrrv7_r11.MRR_RAD_FSM_EN = 1;//Start BB
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);

	// Wait for radio response
	WFI();

	// Turn off Timer32
	*TIMER32_GO = 0;

	if (wfi_timeout_flag){
		mbus_write_message32(0xFA, 0xFAFAFAFA);
	}

	// Turn off Current Limter
	mrrv7_r00.MRR_CL_EN = 0;
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	mrrv7_r11.MRR_RAD_FSM_EN = 0;
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
}

static void send_radio_data_mrr(uint32_t last_packet, uint8_t radio_packet_prefix, uint16_t radio_data_2, uint32_t radio_data_1, uint32_t radio_data_0){
	// Sends 192 bit packet, of which 96b is actual data
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
	//output_data = crcEnc16(((radio_packet_count & 0xFF)<<8) | radio_packet_prefix, (radio_data_2 <<16) | ((radio_data_1 & 0xFFFFFF) >>8), (radio_data_1 << 24) | (radio_data_0 & 0xFFFFFF));

	mbus_remote_register_write(MRR_ADDR,0xD,radio_data_0);
	mbus_remote_register_write(MRR_ADDR,0xE,radio_data_1);
	mbus_remote_register_write(MRR_ADDR,0xF,(radio_packet_prefix<<16)|radio_data_2);
	mbus_remote_register_write(MRR_ADDR,0x10,(radio_packet_count&0xFF)|((output_data[2] & 0xFFFF)/*CRC16*/<<8));

	if (!radio_ready){
		radio_ready = 1;

		// Release FSM Reset
		mrrv7_r11.MRR_RAD_FSM_RSTN = 1;//UNRST BB
		mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
		delay(MBUS_DELAY);

		mrrv7_r03.MRR_TRX_ISOLATEN = 1; //set ISOLATEN 1, let state machine control
		mbus_remote_register_write(MRR_ADDR,0x03,mrrv7_r03.as_int);
		delay(MBUS_DELAY);

		// Current Limter set-up 
		mrrv7_r00.MRR_CL_CTRL = 2; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	}

	uint32_t count = 0;
	uint32_t mrr_cfo_val_fine = 0;
	uint32_t num_packets = 1;
	if (mrr_freq_hopping) num_packets = mrr_freq_hopping;
	
	// New for MRRv7
	mrr_cfo_val_fine = mrr_cfo_val_fine_min;

	while (count < num_packets){
		#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xCE, mrr_cfo_val);
		#endif

		mrrv7_r01.MRR_TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
		mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine;
		mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);
		send_radio_data_mrr_sub1();
		count++;
		if (count < num_packets){
			delay(RADIO_PACKET_DELAY);
		}
		mrr_cfo_val_fine = mrr_cfo_val_fine + mrr_freq_hopping_step; // 1: 0.8MHz, 2: 1.6MHz step
	}

	radio_packet_count++;

	if (last_packet){
		radio_ready = 0;
		radio_power_off();
	}else{
		mrrv7_r11.MRR_RAD_FSM_EN = 0;
		mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
	}
}

//***************************************************
//FLP functions
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
	uint32_t success = 1;
	uint32_t j;
	uint32_t* data_memaddr = (uint32_t*) 0x0f00;
	for (idx=id_begin; idx<id_end; idx++){ // All Pages (8Mb) are erased. except VIM program part (total page address 0~1023)
		do {
			success = 1;
			page_start_addr = (idx << (10-2));
			mbus_remote_register_write (FLP_ADDR, 0x08, page_start_addr); // Set FLSH_START_ADDR
			set_halt_until_mbus_rx();
			mbus_remote_register_write (FLP_ADDR, 0x09, 1 << 5 | 4 << 1 | 1 ); // Page Erase
			set_halt_until_mbus_tx();
			if (*REG1 != 0x4F) flp_fail((0xFFFF << 16) | idx);
			#if VIM_flp_imageerase_callback == 1
			 FLASH_moveToSram(page_start_addr, 0, 256);
			 mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR, (uint32_t*) 0x0, PRE_ADDR, data_memaddr, 256-1);
			#elif VIM_flp_imageerase_callback == 2
			 FLASH_moveToSram(page_start_addr, 0, 256);
			 mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR, (uint32_t*) 0x0, PRE_ADDR, data_memaddr, 256-1);
			 //correctness test
			 for (j = 0; j < 256; j++){
				if (data_memaddr[j] != 0xffffffff){
					success = 0;
				}
			 }
			#endif
		} while (success == 0);
	}
}

static void FLASH_pp_ready () {
	// Ping Pong Setting
	mbus_remote_register_write (FLP_ADDR, 0x13 , 0x0<<1 | 0x1); // Enable Ping Pong w/ No length limit
	mbus_remote_register_write (FLP_ADDR, 0x14 , 0x1<<4 | 0x1<<3 | 0x0<<2 | 0x3 ); // no overrun detect & fast prog & no wrap & data0/data1 en
	mbus_remote_register_write (FLP_ADDR, 0x15 , VIM_IMAGE_start_FLPmemaddr/4); // Flash program addr started from VIM_IMAGE_start_FLPmemaddr/4
}

static void FLASH_pp_on () {
	mbus_remote_register_write (FLP_ADDR, 0x09, 0x0<<6 | 0x1<<5 | 0x6<<1 | 0x1); // Ping Pong Go 
}

static void FLASH_pp_off () {
	set_halt_until_mbus_rx();
	mbus_remote_register_write (FLP_ADDR, 0x13, 0x0<<1 | 0x0); // Ping Pong End
	set_halt_until_mbus_tx();
}

static void FLASH_str_ready (uint32_t sram_addr) {
	mbus_remote_register_write (FLP_ADDR, 0x13 , 0x0<<1 | 0x0); // Disable ping-pong
	// EXT stream Setting
	mbus_remote_register_write (FLP_ADDR, 0x0C , 0x0<<3 | 0x0<<2 | 0x3); // no wrap, no addr update, data0/data1 en
	mbus_remote_register_write (FLP_ADDR, 0x0E , sram_addr ); // set sram_addr 
	mbus_remote_register_write (FLP_ADDR, 0x0D , 0x0 ); // timeout disabled
}

static void FLASH_str_go (uint32_t data_length) {
	set_halt_until_mbus_rx();
	mbus_remote_register_write (FLP_ADDR, 0x09 , (data_length-1)<<6 | 0x1<<5 | 0x6<<1 | 0x1); // set length, IRQ en, CMD=6, go!
	set_halt_until_mbus_tx();

	if ((*REG1 != 0xf9) & (*REG1 != 0xfd)) flp_fail (9);
}

static void FLASH_moveToFlash_fast(uint32_t SRAMaddr, uint32_t FLASHaddr, uint32_t length){
	mbus_remote_register_write (FLP_ADDR , 0x07 , SRAMaddr);
	mbus_remote_register_write (FLP_ADDR , 0x08 , FLASHaddr);
	set_halt_until_mbus_rx();
	mbus_remote_register_write (FLP_ADDR , 0x09 , (length-1) << 6 | 1 << 5 | 3 << 1 | 1); //length, IRQ_EN, cmd (sramToFlash, fast), go!
	set_halt_until_mbus_tx();

	if (*REG1 != 0x5d) flp_fail (2);
}

static void FLASH_moveToFlash(uint32_t SRAMaddr, uint32_t FLASHaddr, uint32_t length){
	mbus_remote_register_write (FLP_ADDR , 0x07 , SRAMaddr);
	mbus_remote_register_write (FLP_ADDR , 0x08 , FLASHaddr);
	set_halt_until_mbus_rx();
	mbus_remote_register_write (FLP_ADDR , 0x09 , (length-1) << 6 | 1 << 5 | 2 << 1 | 1); //length, IRQ_EN, cmd (sramToFlash), go!
	set_halt_until_mbus_tx();

	if (*REG1 != 0x3f) flp_fail (2);
}

static void FLASH_moveToSram(uint32_t FLASHaddr, uint32_t SRAMaddr, uint32_t length){
	mbus_remote_register_write (FLP_ADDR , 0x07 , SRAMaddr);
	mbus_remote_register_write (FLP_ADDR , 0x08 , FLASHaddr);
	set_halt_until_mbus_rx();
	mbus_remote_register_write (FLP_ADDR , 0x09 , (length-1) << 6 | 1 << 5 | 1 << 1 | 1); //length, IRQ_EN, cmd (flashToSram), go!
	set_halt_until_mbus_tx();

	if (*REG1 != 0x2B) flp_fail (3);
}

static void FLASH_outputimage_mbus(){
	uint32_t i = 0;
	uint32_t count = 0;

	for (i=0; i < VIM_mbus_streamchunk; i++){
		FLASH_moveToSram(
			VIM_IMAGE_start_FLPmemaddr/4 + VIM_mbus_streamsize*i, 
			0, 
			VIM_mbus_streamsize);
		mbus_copy_mem_from_remote_to_any_bulk(	
			FLP_ADDR,
			(uint32_t*) 0x0,
			PRE_ADDR,
			(uint32_t*) 0x0f00, //3840th byte
			VIM_mbus_streamsize-1);
		count ++;
	}
	if ((count) * VIM_mbus_streamsize < VIM_IMAGE_size / 4){
		FLASH_moveToSram(
			VIM_IMAGE_start_FLPmemaddr/4 + VIM_mbus_streamsize*(count), 
			0, 
			VIM_IMAGE_size/4 - (count)*VIM_mbus_streamsize);
		mbus_copy_mem_from_remote_to_any_bulk(	
			FLP_ADDR,
			(uint32_t*) 0x0,
			PRE_ADDR,
			(uint32_t*) 0x0f00, //3840th byte
			VIM_IMAGE_size/4 - (count)*VIM_mbus_streamsize - 1);
	}
}

static void FLASH_fastmode (){
	mbus_remote_register_write (FLP_ADDR , 0x19 , 0x1<<11 | 0xf<<7 | 0x0<<3 | 0x6); // Voltage Clamper Tuning
	mbus_remote_register_write (FLP_ADDR , 0x0F , 0x10<<8 | 0x01); // Flash interrupt target register addr: REG0 -> REG1
	//main clk boost by 8x
	mbus_remote_register_write (FLP_ADDR , 0x00 , 0x1<<19 | 0x05<<13 | 0x08<<7 | 0x10); // Tcyc_read, T3us, T5us, T10us
	mbus_remote_register_write (FLP_ADDR , 0x01 , 0x0001<<8 | 0x11); // Tcyc_prog, Tprog
	mbus_remote_register_write (FLP_ADDR , 0x02 , 0x1400*4); // Terase
	mbus_remote_register_write (FLP_ADDR , 0x03 , 0x1800<<10 | 0x180); // Thvcp, Tben
	mbus_remote_register_write (FLP_ADDR , 0x04 , 0xfff<<12 | 0xfff); // Tmvcp, Tsc
	mbus_remote_register_write (FLP_ADDR , 0x05 , 0x02000); // Tcap
	mbus_remote_register_write (FLP_ADDR , 0x06 , 0x08000); // Tvref
	mbus_remote_register_write (FLP_ADDR , 0x18 , 0x0<<2 | 0x1); // ring, DIV
	//main clk boost by 4x
	//mbus_remote_register_write (FLP_ADDR , 0x00 , 0x1<<19 | 0x4<<13 | 0x8<<7 | 0x10); // Tcyc_read, T3us, T5us, T10us
	//mbus_remote_register_write (FLP_ADDR , 0x01 , 0x0001<<8 | 0x08); // Tcyc_prog, Tprog
	//mbus_remote_register_write (FLP_ADDR , 0x02 , 0x0100*4); // Terase
	//mbus_remote_register_write (FLP_ADDR , 0x03 , 0x03e8*4<<10 | 0x031*4); // Thvcp
	//mbus_remote_register_write (FLP_ADDR , 0x04 , 0x3e8*4<<12 | 0x3e8*4); // Tmvcp
	//mbus_remote_register_write (FLP_ADDR , 0x05 , 0x007cf*4); // Tcap
	//mbus_remote_register_write (FLP_ADDR , 0x06 , 0x01f3f*4); // Tvref
	//mbus_remote_register_write (FLP_ADDR , 0x18 , 0x0<<2 | 0x1); // DIV
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


//***************************************************
// DEBUG functions
//***************************************************
static void debug_message(){
	mbus_remote_register_write (0x01 , 0x04 , 0xDEADBEEF);
}

static void debug_FLASH_erase_test (){
	FLASH_turn_on();
	FLASH_erase_range(0,1023);
	FLASH_turn_off();
}

//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(){

	// Reset GOC_DATA_IRQ
	*GOC_DATA_IRQ = 0;

	// Go to Sleep
	mbus_sleep_all();
	while(1);

}

static void operation_sleep_snt_timer(){

	// Reset GOC_DATA_IRQ
	*GOC_DATA_IRQ = 0;
	
	// Disable PRC Timer
	set_wakeup_timer(0, 0, 0);
	
	// Go to Sleep
	mbus_sleep_all();
	while(1);

}

static void operation_sleep_noirqreset(){

	// Go to Sleep
	mbus_sleep_all();
	while(1);

}

static void operation_sleep_notimer(){

	// Make sure the irq counter is reset
	exec_count_irq = 0;

	// Make sure Radio is off
	if (radio_on){radio_power_off();}

	// Disable Timer
	set_wakeup_timer(0, 0, 0);

	// Go to sleep
	operation_sleep();

}


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

//********************************************************************
// MAIN function starts here 
//********************************************************************

static void prc_init(){
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
				0x3f,//base
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

static void mrr_init(){
	// MRR Settings --------------------------------------
	// Decap in series
	mrrv7_r03.MRR_DCP_P_OW = 0;//RX_Decap P 
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
	mrrv7_r03.MRR_DCP_S_OW = 1;//TX_Decap S (forced charge decaps)
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);

	// Current Limter set-up 
	mrrv7_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	// Turn on Current Limter
	mrrv7_r00.MRR_CL_EN = 1;//Enable CL
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
	delay(MBUS_DELAY*100); // Wait for decap to charge

	mrrv7_r1F.LC_CLK_RING = 0x3;// ~ 150 kHz
	mrrv7_r1F.LC_CLK_DIV = 0x3;// ~ 150 kHz
	mbus_remote_register_write(MRR_ADDR,0x1F,mrrv7_r1F.as_int);

	//mrr_configure_pulse_width_short();
	mrr_configure_pulse_width_long();

	mrr_freq_hopping = 5;
	mrr_freq_hopping_step = 4;

	mrr_cfo_val_fine_min = 0x0000;

	// RO setup (SFO)
	// Adjust Diffusion R
	mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF

	// Adjust Poly R
	mbus_remote_register_write(MRR_ADDR,0x08,0x400000); // RO_POLY

	// Adjust C
	mrrv7_r07.RO_MOM = 0x10;
	mrrv7_r07.RO_MIM = 0x10;
	mbus_remote_register_write(MRR_ADDR,0x07,mrrv7_r07.as_int);

	// TX Setup Carrier Freq
	mrrv7_r00.MRR_TRX_CAP_ANTP_TUNE_COARSE = 0x1F;//ANT CAP 10b unary 830.5 MHz
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
	mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_COARSE = 0x1F; //ANT CAP 10b unary 830.5 MHz
	mrrv7_r01.MRR_TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;//ANT CAP 14b unary 830.5 MHz
	mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
	mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);
	mrrv7_r02.MRR_TX_BIAS_TUNE = 0x1FFF;//Set TX BIAS TUNE 13b // Set to max
	mbus_remote_register_write(MRR_ADDR,0x02,mrrv7_r02.as_int);

	// Turn off RX mode
	mrrv7_r03.MRR_TRX_MODE_EN = 0; //Set TRX mode

	// Keep decap charged in the background
	mrrv7_r03.MRR_DCP_S_OW = 1;//TX_Decap S (forced charge decaps)

	// Forces decaps to be parallel
	//mrrv7_r03.MRR_DCP_S_OW = 0;//TX_Decap S (forced charge decaps)
	//mrrv7_r03.MRR_DCP_P_OW = 1;//RX_Decap P 
	//mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);

	// RX Setup
	mrrv7_r03.MRR_RX_BIAS_TUNE= 0x02AF;//turn on Q_enhancement
	//mrrv7_r03.MRR_RX_BIAS_TUNE= 0x0001;//turn off Q_enhancement
	mrrv7_r03.MRR_RX_SAMPLE_CAP= 0x1;// RX_SAMPLE_CAP
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);

	mrrv7_r14.MRR_RAD_FSM_RX_POWERON_LEN = 0x0;//Set RX Power on length
	//mrrv7_r14.MRR_RAD_FSM_RX_SAMPLE_LEN = 0x3;//Set RX Sample length16us
	mrrv7_r14.MRR_RAD_FSM_RX_SAMPLE_LEN = 0x0;//Set RX Sample length4us
	mrrv7_r14.MRR_RAD_FSM_GUARD_LEN = 0x000F; //Set TX_RX Guard length, TX_RX guard 32 cycle (28+5)
	mbus_remote_register_write(MRR_ADDR,0x14,mrrv7_r14.as_int);

	mrrv7_r14.MRR_RAD_FSM_TX_POWERON_LEN = 2; //3bits
	mrrv7_r15.MRR_RAD_FSM_RX_HDR_BITS = 0x00;//Set RX header
	mrrv7_r15.MRR_RAD_FSM_RX_HDR_TH = 0x00;//Set RX header threshold
	mrrv7_r15.MRR_RAD_FSM_RX_DATA_BITS = 0x00; //Set RX data 1b
	mbus_remote_register_write(MRR_ADDR,0x15,mrrv7_r15.as_int);

	// RAD_FSM set-up 
	// Using first 48 bits of data as header
	mbus_remote_register_write(MRR_ADDR,0x09,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0A,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0B,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0C,0x7AC800);
	mrrv7_r11.MRR_RAD_FSM_TX_H_LEN = 0; //31-31b header (max)
	mrrv7_r11.MRR_RAD_FSM_TX_D_LEN = RADIO_DATA_LENGTH; //0-skip tx data
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);

	mrrv7_r13.MRR_RAD_FSM_TX_MODE = 3; //code rate 0:4 1:3 2:2 3:1(baseline) 4:1/2 5:1/3 6:1/4
	mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);

	// Mbus return address
	mbus_remote_register_write(MRR_ADDR,0x1E,0x1002);
}

static void vim_init(){
	*VIM_mode = VIM_mode_fullsequence;

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

	vim_ivref_on();
	vim_tmr_on_slow();
}

static void operation_init(void){
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
	*VIM_stack_initialized = 1;
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
	VIM_idle = 1;
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
void handler_ext_int_reg4(void) { // REG4
	//VIM triggers MBUS interrupt on REG4
	*NVIC_ICPR = (0x1 << IRQ_REG4);
	VIM_interrupt_code = *REG4;
	if (VIM_interrupt_code == 4) { //Calibration done!
		//typical behavior
		if (*VIM_mode == VIM_mode_fullsequence)		{ vim_calib_mode_end(); *VIM_mode = VIM_mode_mdnormalloop; vim_normal_mode_start(); }
		else { vim_calib_mode_end(); vim_poll_mode(); }
	}
	else if (VIM_interrupt_code == 6) { //Motion frame taken! - but motion not detected
		if (*VIM_mode == VIM_mode_mdonly)		{ /* continue */ }
		else if (*VIM_mode == VIM_mode_mdonlysingle)	{ vim_md_mode_end(); VIM_idle=0; vim_poll_mode(); }
		//typical behavior
		else if (*VIM_mode == VIM_mode_mdnormalloop)	{}
		else { vim_md_mode_end(); vim_poll_mode(); }
	}
	else if (VIM_interrupt_code == 8) { //Motion detected!
		if (*VIM_mode == VIM_mode_mdonly)		{ /* continue */ }
		else if (*VIM_mode == VIM_mode_mdonlysingle)	{ vim_md_mode_end(); VIM_idle=0; vim_poll_mode(); }
		//typical behavior
		else if (*VIM_mode == VIM_mode_mdnormalloop){
			vim_md_mode_end();
			if ((VIM_IMAGE_NOR_capture_cnt < VIM_NOR_opcount) || (VIM_NOR_opcount == 0)){
				vim_normal_mode_start();
			}
			else {
				vim_md_mode_start();
			}
		}
		else { vim_md_mode_end; vim_poll_mode(); }
	}
	else if (VIM_interrupt_code == 32) { //NORMAL DONE
		if (*VIM_mode == VIM_mode_normalonlysingle)	{ vim_normal_mode_end(); VIM_idle=0; vim_poll_mode(); }
		//typical behavior
		else if (*VIM_mode == VIM_mode_mdnormalloop) 	{
			vim_normal_mode_end();
			VIM_IMAGE_NOR_capture_cnt++;
			vim_md_mode_start(); 
		}
		else { vim_normal_mode_end(); vim_poll_mode(); }
	}
}

//*******************************************************************
// MAIN function - executed on turn on
//*******************************************************************

int main(){
	//if (*program_start_pointer == 0xdeadbeef){ while(1); }
	//CPU timeout disable
	*TIMERWD_GO = 0;

	//Enable interrupt
	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3)| (1 << IRQ_REG4);

	if (*VIM_stack_initialized != 1) { operation_init(); }

	vim_poll_mode();

	//operation_sleep();
	while(1);
}


