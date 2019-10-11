//*******************************************************************
//Author: Kyojin Choo
//Description: initial VIM program upload to FLP
//			
//*******************************************************************
#include "PREv18.h"
#include "PREv18_RF.h"
#include "mbus.h"
#include "FLPv3L_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// VIMstack_v0_t1 short prefix
#define PRE_ADDR	0x1
#define FLP_ADDR	0x5

// VIM variables
#define VIM_prog_PRESRAMmemstartaddr 	1400*4 	//defined by how main.py speaks EP
//#define VIM_prog_FLPmemstartaddr 	4*1024*4 	//multiple of 1024 (page size)
#define VIM_prog_CALmemstartaddr	4*1024*4
#define VIM_prog_MDmemstartaddr		1*1024*4
#define VIM_prog_NORmemstartaddr	2*1024*4
#define VIM_prog_ID			0 	//0=calib, 1=md, 2=normal

// System parameters
#define	MBUS_DELAY 	300 // Amount of delay between successive messages; 100: 6-7ms

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t * program_start_pointer = 8008;
volatile uint32_t prog_callback[600];

//********************************************************************
// VIM stack specific variables
//********************************************************************
// VIM program in FLP start address
//volatile uint32_t VIM_CAL_prog_FLPmemaddr		=VIM_prog_FLPmemstartaddr+0*1024*4; //len=500*4
//volatile uint32_t VIM_MD_prog_FLPmemaddr		=VIM_prog_FLPmemstartaddr+1*1024*4; //len=600*4
//volatile uint32_t VIM_NOR_prog_FLPmemaddr		=VIM_prog_FLPmemstartaddr+2*1024*4; //len=600*4
volatile uint32_t VIM_CAL_prog_FLPmemaddr		=VIM_prog_CALmemstartaddr; //len=500*4
volatile uint32_t VIM_MD_prog_FLPmemaddr		=VIM_prog_MDmemstartaddr; //len=600*4
volatile uint32_t VIM_NOR_prog_FLPmemaddr		=VIM_prog_NORmemstartaddr; //len=600*4

volatile uint32_t VIM_prog_PRESRAMmemaddr		=VIM_prog_PRESRAMmemstartaddr;

volatile uint32_t success;
volatile uint32_t* prog_original;
volatile uint32_t prog_length;
volatile uint32_t prog_flp;
volatile uint32_t prog_flp_pagestart;
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
	for (idx=id_begin; idx<id_end; idx++){ // All Pages (8Mb) are erased. except VIM program part (address 0~8000)
		page_start_addr = (idx << 8);
		mbus_remote_register_write (FLP_ADDR, 0x08, page_start_addr); // Set FLSH_START_ADDR
		set_halt_until_mbus_rx();
		mbus_remote_register_write (FLP_ADDR, 0x09, 1 << 5 | 4 << 1 | 1 ); // Page Erase
		set_halt_until_mbus_tx();
		if (*REG1 != 0x4F) flp_fail((0xFFFF << 16) | idx);
	}
}

static void FLASH_moveToSram(uint32_t FLASHaddr, uint32_t SRAMaddr, uint32_t length){
	mbus_remote_register_write (FLP_ADDR , 0x07 , SRAMaddr);
	mbus_remote_register_write (FLP_ADDR , 0x08 , FLASHaddr);
	set_halt_until_mbus_rx();
	mbus_remote_register_write (FLP_ADDR , 0x09 , (length-1) << 6 | 1 << 5 | 1 << 1 | 1); //length, IRQ_EN, cmd (flashToSram), go!
	set_halt_until_mbus_tx();

	if (*REG1 != 0x2B) flp_fail (3);
}

static void FLASH_moveToFlash(uint32_t SRAMaddr, uint32_t FLASHaddr, uint32_t length){
	mbus_remote_register_write (FLP_ADDR , 0x07 , SRAMaddr);
	mbus_remote_register_write (FLP_ADDR , 0x08 , FLASHaddr);
	set_halt_until_mbus_rx();
	mbus_remote_register_write (FLP_ADDR , 0x09 , (length-1) << 6 | 1 << 5 | 2 << 1 | 1); //length, IRQ_EN, cmd (sramToFlash), go!
	set_halt_until_mbus_tx();

	if (*REG1 != 0x3f) flp_fail (2);
}

static void FLASH_moveToFlash_fast(uint32_t SRAMaddr, uint32_t FLASHaddr, uint32_t length){
	mbus_remote_register_write (FLP_ADDR , 0x07 , SRAMaddr);
	mbus_remote_register_write (FLP_ADDR , 0x08 , FLASHaddr);
	set_halt_until_mbus_rx();
	mbus_remote_register_write (FLP_ADDR , 0x09 , (length-1) << 6 | 1 << 5 | 3 << 1 | 1); //length, IRQ_EN, cmd (sramToFlash, fast), go!
	set_halt_until_mbus_tx();

	if (*REG1 != 0x5d) flp_fail (2);
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

void main(){
	//if (*program_start_pointer == 0xdeadbeef){ while(1); }
	//CPU timeout disable
	*TIMERWD_GO = 0;

	//Enable interrupt
	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3)| (1 << IRQ_REG4);

	success = 0;
	prog_original = (uint32_t*) VIM_prog_PRESRAMmemaddr;
	#if VIM_prog_ID == 0
	prog_length = 500;
	prog_flp = VIM_CAL_prog_FLPmemaddr;
	prog_flp_pagestart = VIM_CAL_prog_FLPmemaddr/1024;
	#elif VIM_prog_ID == 1
	prog_length = 600;
	prog_flp = VIM_MD_prog_FLPmemaddr;
	prog_flp_pagestart = VIM_MD_prog_FLPmemaddr/1024;
	#elif VIM_prog_ID == 2
	prog_length = 600;
	prog_flp = VIM_NOR_prog_FLPmemaddr;
	prog_flp_pagestart = VIM_NOR_prog_FLPmemaddr/1024;
	#endif

	FLASH_turn_on();

	while (success == 0){
		uint32_t j = 0;
		//check initial program OR erase result
		FLASH_moveToSram((prog_flp)/4,0x0,prog_length);
		set_halt_until_mbus_trx();
		mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR, (uint32_t*) 0x0, PRE_ADDR, prog_callback, prog_length-1);
		set_halt_until_mbus_tx();
		//correctness test
		success = 1;
		for (j = 0; j < prog_length; j++){
			if (prog_callback[j] != prog_original[j]){
				success = 0;
			}
		}
		if (success == 1) break;
		//copy program
		mbus_copy_mem_from_local_to_remote_bulk(FLP_ADDR, (uint32_t*) 0x0, prog_original, prog_length-1);
		FLASH_moveToFlash_fast(0x0, (prog_flp)/4, prog_length);
		//loopback
		FLASH_moveToSram((prog_flp)/4, 0x0, prog_length);
		set_halt_until_mbus_trx();
		mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR, (uint32_t*) 0x0, PRE_ADDR, prog_callback, prog_length-1);
		set_halt_until_mbus_tx();
		//correctness test
		success = 1;
		for (j = 0; j < prog_length; j++){
			if (prog_callback[j] != prog_original[j]){
				success = 0;
			}
		}
		if (success == 0) FLASH_erase_range(prog_flp_pagestart, prog_flp_pagestart+4);
	}
	
	//sleep
	FLASH_turn_off();
	while(1);
}


