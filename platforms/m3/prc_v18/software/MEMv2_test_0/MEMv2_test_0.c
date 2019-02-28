//*******************************************************************
//Author: Yejoong Kim
//Description: MEMv2 Testing
// PRCv18 -> MEMv2 -> SNSv11 -> PMUv9 (optional)
//*******************************************************************
#include "PRCv18.h"
#include "PMUv9_RF.h"
#include "SNSv11_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define MEM_ADDR    0x4
#define SNS_ADDR    0x8
#define PMU_ADDR    0xC

// Define if you use PMU
//#define USE_PMU

// Flag Idx
#define FLAG_ENUM       0

// MEM TESTING
#define PARALLEL_WORDS	  32
#define TARGET_NUM_WORDS  4096
#define MEM_ITER          ((uint32_t) TARGET_NUM_WORDS / PARALLEL_WORDS)

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t mem_temp[PARALLEL_WORDS];

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
// NOTE: IRQ handlers not used.

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    set_flag(FLAG_ENUM, 1);

    // Set Halt
    set_halt_until_mbus_trx();

    // Enumeration
    mbus_enumerate(MEM_ADDR);
    mbus_enumerate(SNS_ADDR);
   #ifdef USE_PMU
    mbus_enumerate(PMU_ADDR);
   #endif

    //Set Halt
    set_halt_until_mbus_tx();

    // Go to sleep
    set_wakeup_timer(0, 0, 0);
    mbus_sleep_all();
    while(1);
}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    uint32_t idx_i;
    uint32_t idx_j;
    uint32_t idx_k;
    uint32_t target_addr = 0;

	
    // Initialization Sequence
    if (!get_flag(FLAG_ENUM)) { 
        initialization();
    }

    // Extract data
    uint32_t data_0 = *REG0;
    uint32_t data_1 = (*REG1 << 16) | (*REG1 & 0xFFFF);
    *REG0 = 0;
    *REG1 = 0;

    // REG0=1: Write entire memory using the pattern given in REG1
    if (data_0 == 0x1) {

        for (idx_i=0; idx_i<32; idx_i++) {mem_temp[idx_i] = data_1;}

	for (idx_i=0; idx_i<MEM_ITER; idx_i++) { 
		
	    mbus_copy_mem_from_local_to_remote_bulk (MEM_ADDR, (uint32_t *) (target_addr << 2), &mem_temp, PARALLEL_WORDS - 1);

	    target_addr = target_addr + PARALLEL_WORDS;
	}
    }

    // REG0=2; Check the memory data. Correct pattern should be given in REG1.
    else if (data_0 == 0x2) {

	uint32_t num_word_err = 0;
	uint32_t num_bit_err = 0;
	uint32_t num_bit10_err = 0;
	uint32_t num_bit01_err = 0;

	for (idx_i=0; idx_i<MEM_ITER; idx_i++) { 

	    set_halt_until_mbus_trx();
	    mbus_copy_mem_from_remote_to_any_bulk (MEM_ADDR, (uint32_t *) (target_addr << 2), PRC_ADDR, &mem_temp, PARALLEL_WORDS - 1);
	    set_halt_until_mbus_tx();

	    for (idx_j=0; idx_j<PARALLEL_WORDS; idx_j++) {
  	        if (mem_temp[idx_j] != data_1) {
		    num_word_err ++;
		    for(idx_k=0; idx_k<32; idx_k++) {
	                uint32_t temp0 = mem_temp[idx_j];
			uint32_t temp1 = data_1;
			temp0 = (temp0 << (31 - idx_k)) >> 31;
			temp1 = (temp1 << (31 - idx_k)) >> 31;
			if(temp0 != temp1) { num_bit_err++;}
			if(!temp0 && temp1) { num_bit10_err++;}
			if(temp0 && !temp1) { num_bit01_err++;}
		    }
		}
            }

	    target_addr = target_addr + PARALLEL_WORDS;
	}

	mbus_write_message32 (0xA0, num_word_err);
	mbus_write_message32 (0xA1, num_bit_err);
	mbus_write_message32 (0xA2, num_bit10_err);
	mbus_write_message32 (0xA3, num_bit01_err);
    }

    // REG0=3; Change Memory Tuning
    else if (data_0 == 0x3) {
	
	mbus_write_message32 (MEM_ADDR << 4, ((0x0 << 24) | (data_1 & 0xFFFFFF)));
	mbus_write_message32 (MEM_ADDR << 4, ((0x1 << 24) | (data_1 & 0xFFFFFF)));

    }
    
    // Go to sleep
    set_wakeup_timer(0, 0, 0);
    mbus_sleep_all();
    while(1);

    return 1;
}
