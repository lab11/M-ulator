//*******************************************************************
//Author: Yejoong Kim
//Description: PREv18E MEM Testing
//*******************************************************************
#include "PREv18E.h"
#include "mbus.h"

// PRC->MEM
#define MEM0_ADDR 0x4
#define MEM1_ADDR 0x5

// PARAMETERS
#define NUM_PARALLEL 32

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t irq_history;

//*******************************************************************
// INTERRUPT HANDLERS (Updated for PRCv17)
//*******************************************************************

//********************************************************************
// Inline Functions
//********************************************************************
inline static void m_mbus_send_32(uint8_t addr, uint32_t data) {
    uint32_t mbus_addr = 0xA0003000 | (addr << 4);
    *((volatile uint32_t *) mbus_addr) = data;
}

inline static void m_disable_timerwd(void) {
	*TIMERWD_GO  = 0x0;
	*TIMERWD_CNT = 0x0;
}

inline static void m_disable_wakeup_timer(void) {
    *REG_WUPT_CONFIG = 0;
}

inline static void m_halt_until_tx(void)  { *SREG_CONF_HALT = HALT_UNTIL_MBUS_TX; }
inline static void m_halt_until_trx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_TRX; }

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    uint32_t idx_i;
    uint32_t idx_j;
    uint32_t idx_k;

    m_mbus_send_32 (0xD0, 0x01010101);

    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    m_disable_timerwd();

    if (*GOC_DATA_IRQ == 0) {
        // Invalidate prefix
        m_mbus_send_32(0x00, ((0x3 << 28) | (0xF << 24)));

        // set halt until mbus trx
        m_halt_until_trx();

        // Enumeration
        m_mbus_send_32(0x00, ((0x2 << 28) | (MEM0_ADDR << 24)));
        m_mbus_send_32(0x00, ((0x2 << 28) | (MEM1_ADDR << 24)));

        // set halt until mbus tx
        m_halt_until_tx();

        // SA Selection
        //*REG_SRAM_TUNE = 0x45; // Inverter SA
    }

    // Display GOC_DATA_IRQ
    m_mbus_send_32 (0xC0, *GOC_DATA_IRQ);

    // Task Implementation

    // Copy PRE SRAM into MEM0
    if (*GOC_DATA_IRQ == 0x1) {
        *GOC_DATA_IRQ = 0xDEADBEEF; // Reset GOC_DATA_IRQ

	    *MBUS_CMD0 = (MEM0_ADDR << 28) | (MPQ_MEM_BULK_WRITE << 24) | (4095 << 0);
	    *MBUS_CMD1 = 0x0;
	    *MBUS_CMD2 = 0x0;
	    *MBUS_FUID_LEN = MPQ_MEM_READ | (0x3 << 4);
    }
    // Copy PRE SRAM into MEM1
    else if (*GOC_DATA_IRQ == 0x2) {
        *GOC_DATA_IRQ = 0xDEADBEEF; // Reset GOC_DATA_IRQ

	    *MBUS_CMD0 = (MEM1_ADDR << 28) | (MPQ_MEM_BULK_WRITE << 24) | (4095 << 0);
	    *MBUS_CMD1 = 0x0;
	    *MBUS_CMD2 = 0x0;
	    *MBUS_FUID_LEN = MPQ_MEM_READ | (0x3 << 4);
    }
    // Compare MEM0 and MEM1
    else if (*GOC_DATA_IRQ == 0x3) {
        uint32_t data0[NUM_PARALLEL];
        uint32_t data1[NUM_PARALLEL];

        uint32_t num_word_err = 0;
	    uint32_t num_bit_err = 0;
	    uint32_t num_bit10_err = 0;
	    uint32_t num_bit01_err = 0;

        *GOC_DATA_IRQ = 0xDEADBEEF; // Reset GOC_DATA_IRQ

        for(idx_i=0; idx_i<4096; idx_i=idx_i+NUM_PARALLEL) {

            // Read from MEM0/MEM1 and put it in data0/data1
            m_halt_until_trx();
            mbus_copy_mem_from_remote_to_any_bulk(MEM0_ADDR, (uint32_t *) (0x0 + (idx_i << 2)), 0x1, (uint32_t *) data0, NUM_PARALLEL-1);
            mbus_copy_mem_from_remote_to_any_bulk(MEM1_ADDR, (uint32_t *) (0x0 + (idx_i << 2)), 0x1, (uint32_t *) data1, NUM_PARALLEL-1);
            m_halt_until_tx();

            // Compare
	        for (idx_j=0; idx_j<NUM_PARALLEL; idx_j++) {
  	            if (data0[idx_j] != data1[idx_j]) {
		            num_word_err ++;
		            for(idx_k=0; idx_k<32; idx_k++) {
	                    uint32_t temp0 = data0[idx_j];
		    	        uint32_t temp1 = data1[idx_j];
		    	        temp0 = (temp0 << (31 - idx_k)) >> 31;
		    	        temp1 = (temp1 << (31 - idx_k)) >> 31;
		    	        if(temp0 != temp1) { num_bit_err++;}
		    	        if(temp0 && !temp1) { num_bit10_err++;}
		    	        if(!temp0 && temp1) { num_bit01_err++;}
		            }
		        }
            }
        }

        m_mbus_send_32(0xD0, num_word_err);
        m_mbus_send_32(0xD1, num_bit_err);
        m_mbus_send_32(0xD2, num_bit10_err);
        m_mbus_send_32(0xD3, num_bit01_err);

    }
    else if (*GOC_DATA_IRQ == 0x4) {
        uint32_t cycle;
        cycle=0;

        // Disable MBus Watchdog
        *REG_MBUS_WD = 0;

        while(1){
            mbus_write_message32(0xE0, cycle);
            delay(10000);
            cycle++;
        }
    }

    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0xDEADBEEF;

    // Disable Wakeup Timer
    m_disable_wakeup_timer();

    // Go to sleep
    m_mbus_send_32(0x01, (0x0 << 28));

    // For safety...
    while(1);

}

