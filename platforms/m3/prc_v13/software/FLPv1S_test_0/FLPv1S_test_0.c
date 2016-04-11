//*******************************************************************
//Author: Yejoong Kim
//Description: FLPv1S Testing with PMUv2
//*******************************************************************
#include "PRCv13.h"
#include "FLPv1S_RF.h"
#include "PMUv2_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define FLS_ADDR    0x4
#define PMU_ADDR    0x8

// DELAYS (For MBus Snooping)
#define MBUS_DELAY  1000

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t cyc_num;
volatile uint32_t irq_history;
volatile uint32_t rsvd_16[16];

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

void handler_ext_int_0(void)  { *NVIC_ICPR = (0x1 << 0);  irq_history |= (0x1 << 0);  } // TIMER32
void handler_ext_int_1(void)  { *NVIC_ICPR = (0x1 << 1);  irq_history |= (0x1 << 1);  } // TIMER16
void handler_ext_int_2(void)  { *NVIC_ICPR = (0x1 << 2);  irq_history |= (0x1 << 2);  } // REG0
void handler_ext_int_3(void)  { *NVIC_ICPR = (0x1 << 3);  irq_history |= (0x1 << 3);  } // REG1
void handler_ext_int_4(void)  { *NVIC_ICPR = (0x1 << 4);  irq_history |= (0x1 << 4);  } // REG2
void handler_ext_int_5(void)  { *NVIC_ICPR = (0x1 << 5);  irq_history |= (0x1 << 5);  } // REG3
void handler_ext_int_6(void)  { *NVIC_ICPR = (0x1 << 6);  irq_history |= (0x1 << 6);  } // REG4
void handler_ext_int_7(void)  { *NVIC_ICPR = (0x1 << 7);  irq_history |= (0x1 << 7);  } // REG5
void handler_ext_int_8(void)  { *NVIC_ICPR = (0x1 << 8);  irq_history |= (0x1 << 8);  } // REG6
void handler_ext_int_9(void)  { *NVIC_ICPR = (0x1 << 9);  irq_history |= (0x1 << 9);  } // REG7
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); irq_history |= (0x1 << 10); } // MEM_WR
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); irq_history |= (0x1 << 11); } // MBUS_RX
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); irq_history |= (0x1 << 12); } // MBUS_TX
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); irq_history |= (0x1 << 13); } // MBUS_FWD

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {
    *REG_CHIP_ID = 0xBEEF;
    cyc_num = 0;
    irq_history = 0;

    // Enumeration
    set_halt_until_mbus_rx();
    mbus_enumerate(FLS_ADDR);
    mbus_enumerate(PMU_ADDR);

    // PMU Tuning
    set_halt_until_mbus_rx();

    // PMU_UPCONVERTER_TRIM_V3_SLEEP
    mbus_write_message32 ((PMU_ADDR << 4),
            ( (0x17 << 24)  // PMU_UPCONVERTER_TRIM_V3_SLEEP
            | (0x3  << 14)  // Desired Vout/Vin ratio (39-n:16-n)
            | (0x1  << 13)  // Enable main feedback loop
            | (0x1  << 9)   // Frequency multiplier R
            | (0x0  << 5)   // Frequency multiplier L
            | (0x04)));     // Floor frequency base

    // PMU_UPCONVERTER_TRIM_V3_ACTIVE
    mbus_write_message32 ((PMU_ADDR << 4), 
            ( (0x18 << 24)  // PMU_UPCONVERTER_TRIM_V3_ACTIVE
            | (0x3  << 14)  // Desired Vout/Vin ratio (39-n:16-n)
            | (0x1  << 13)  // Enable main feedback loop
            | (0x1  << 9)   // Frequency multiplier R
            | (0x2  << 5)   // Frequency multiplier L
            | (0x0A)));     // Floor frequency base

    // PMU_DOWNCONVERTER_TRIM_V3_SLEEP
    mbus_write_message32 ((PMU_ADDR << 4),
            ( (0x19 << 24)  // PMU_DOWNCONVERTER_TRIM_V3_SLEEP
            | (0x1  << 13)  // Enable main feedback loop
            | (0x1  << 9)   // Frequency multiplier R
            | (0x0  << 5)   // Frequency multiplier L
            | (0x08)));     // Floor frequency base

    // PMU_DOWNCONVERTER_TRIM_V3_ACTIVE
    mbus_write_message32 ((PMU_ADDR << 4),
            ( (0x1A << 24)  // PMU_DOWNCONVERTER_TRIM_V3_ACTIVE
            | (0x1  << 13)  // Enable main feedback loop
            | (0x4  << 9)   // Frequency multiplier R
            | (0x4  << 5)   // Frequency multiplier L
            | (0x10)));     // Floor frequency base

    // PMU_SAR_TRIM_V3_SLEEP
    mbus_write_message32 ((PMU_ADDR << 4), 
            ( (0x15 << 24)  // PMU_SAR_TRIM_V3_SLEEP
            | (0x0  << 19)  // Enable PFM even during periodic reset
            | (0x0  << 18)  // Enable PFM even when VREF is not used as reference
            | (0x0  << 17)  // Enable PFM
            | (0x3  << 14)  // Comparator clock division ratio (2^n)
            | (0x1  << 13)  // Enable main feedback loop
            | (0x2  << 9)   // Frequency multiplier R
            | (0x2  << 5)   // Frequency multiplier L
            | (0x04)));     // Floor frequency base

    // PMU_SAR_TRIM_V3_ACTIVE
    mbus_write_message32 ((PMU_ADDR << 4),
            ( (0x16 << 24)  // PMU_SAR_TRIM_V3_ACTIVE
            | (0x0  << 19)  // Enable PFM even during periodic reset
            | (0x0  << 18)  // Enable PFM even when VREF is not used as reference
            | (0x0  << 17)  // Enable PFM
            | (0x3  << 14)  // Comparator clock division ratio (2^n)
            | (0x1  << 13)  // Enable main feedback loop
            | (0x5  << 9)   // Frequency multiplier R
            | (0x5  << 5)   // Frequency multiplier L
            | (0x16)));     // Floor frequency base

    // PMU_SAR_RATIO_OVERRIDE
    mbus_write_message32 ((PMU_ADDR << 4), 
            ( (0x05 << 24)  // PMU_SAR_RATIO_OVERRIDE
            | (0x1  << 11)  // Enable Below
            | (0x1  << 10)  // Have the converter have the periodic reset
            | (0x1  << 9)   // Enable Below
            | (0x0  << 8)   // Switch input/output power rails for upconversion
            | (0x1  << 7)   // Enable Below
            | (0x30)));     // Binary converter's conversion ratio
    delay(200);
    mbus_write_message32 ((PMU_ADDR << 4), 
            ( (0x05 << 24)  // PMU_SAR_RATIO_OVERRIDE
            | (0x1  << 11)  // Enable Below
            | (0x0  << 10)  // Have the converter have the periodic reset
            | (0x1  << 9)   // Enable Below
            | (0x0  << 8)   // Switch input/output power rails for upconversion
            | (0x1  << 7)   // Enable Below
            | (0x30)));     // Binary converter's conversion ratio

    // PMU_TICK_REPEAT_VBAT_ADJUST
    mbus_write_message32 ((PMU_ADDR << 4), (0x36 << 24) | 0x000001);

    // Flash Tuning
    set_halt_until_mbus_tx();
    mbus_write_message32 ((FLS_ADDR << 4), (0x19 << 24) | 0x000F03); // Voltage Clamper Tuning
    mbus_write_message32 ((FLS_ADDR << 4), (0x02 << 24) | 0x000100); // Terase
    mbus_write_message32 ((FLS_ADDR << 4), (0x04 << 24) | 0x000020); // Tcyc_prog
    mbus_write_message32 ((FLS_ADDR << 4), (0x0F << 24) | 0x001007); // IRQ Reply Address
}

void mbus_msg32_dly_0 (uint32_t addr, uint32_t data) {
    mbus_write_message32 (addr, data);
    delay(MBUS_DELAY);
}

void mbus_msg32_irq_7 (uint32_t payload, uint32_t addr, uint32_t data) {
    // IRQ Payload must be stored in REG7.
    set_halt_until_mbus_rx();
    mbus_write_message32 (addr, data);
    set_halt_until_mbus_tx();
    if (*REG7 != payload) {
        mbus_msg32_dly_0 (0xEF, 0xDEADBEEF);
        mbus_msg32_dly_0 (0xEF, addr);
        mbus_msg32_dly_0 (0xEF, data);
        mbus_msg32_dly_0 (0xEF, payload);
        mbus_msg32_dly_0 (0xEF, *REG7);
        mbus_msg32_dly_0 (0xEF, 0xDEADBEEF);
        mbus_sleep_all();
    }
}

void flash_operation (uint32_t op_id) {
    // op_id == 0x1 : IDLE
    // op_id == 0x2 : ERASE
    // op_id == 0x3 : ERASE CHECK
    // op_id == 0x4 : Write All0 to SRAM
    // op_id == 0x5 : Write All0 to SRAM CHECK
    // op_id == 0x6 : PROGRAM (ALL0)
    // op_id == 0x7 : PROGRAM (ALL0) CHECK
    // op_id == 0x8 : READ ONLY
    
    uint32_t idx_i;
    uint32_t idx_j;
    uint32_t idx_k;
    uint32_t page_addr;

    // Turn on Flash
    mbus_msg32_irq_7 (0xB5, (FLS_ADDR << 4), ((0x11 << 24) | 0x00002F)); // Turn on VC and Flash (0xB5)

    // Start of Testing
    if (op_id == 0x1) { // Idle
        delay(100000);
    }
    else if (op_id == 0x2) { // Erase
        for (idx_i=8; idx_i<128; idx_i++){ // Do NOT Erase the first 8kB (it has a test boot-up code)
            page_addr = (idx_i << 8);
            mbus_write_message32 ((FLS_ADDR << 4), ((0x09 << 24) | page_addr)); // Set FLSH_START_ADDR
            mbus_msg32_irq_7 (0x4F, (FLS_ADDR << 4), ((0x07 << 24) | 0x000029)); // Page Erase
        }
    }
    else if (op_id == 0x3) { // Check Erase
        for (idx_i=8; idx_i<128; idx_i=idx_i+8){
            page_addr = (idx_i << 8);
            mbus_write_message32 ((FLS_ADDR << 4), ((0x09 << 24) | page_addr)); // Set FLSH_START_ADDR
            mbus_msg32_irq_7 (0x2B, (FLS_ADDR << 4), ((0x07 << 24) | 0x01FFE3)); // Flash -> SRAM (8kB)

            for (idx_j=0; idx_j<2048; idx_j=idx_j+16) {
                set_halt_until_mbus_rx();
                mbus_copy_mem_from_remote_to_any_bulk (
                    FLS_ADDR,                   // source prefix
                    (uint32_t *) (idx_j << 2),  // source memory address
                    PRC_ADDR,                   // destination prefix
                    rsvd_16,                    // destination memory address
                    15                          // length - 1
                );

                set_halt_until_mbus_tx();
                for (idx_k=0; idx_k<16; idx_k++) {
                    if (rsvd_16[idx_k] != 0xFFFFFFFF) {
                        mbus_msg32_dly_0 (0xEE, 0xDEADBEEF);
                        mbus_msg32_dly_0 (0xEE, idx_j);
                        mbus_msg32_dly_0 (0xEE, (idx_j << 2));
                        mbus_msg32_dly_0 (0xEE, idx_k);
                        mbus_msg32_dly_0 (0xEE, rsvd_16[idx_k]);
                        mbus_msg32_dly_0 (0xEE, 0xDEADBEEF);
                        mbus_sleep_all();
                    }
                }
            }
        }
    }
    else if (op_id == 0x4) { // Write All 0 to SRAM
        uint32_t temp_data[16] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
        for (idx_i=0; idx_i<2048; idx_i=idx_i+16){
            mbus_copy_mem_from_local_to_remote_bulk (
		        FLS_ADDR,                   // remote_prefix
                (uint32_t *) (idx_i << 2),  // remote_memory_address
                temp_data,                  // local_address
                15                          // length_in_words_minus_one
            );
        }
    }
    else if (op_id == 0x5) { // Write All 0 to SRAM CHECK
        for (idx_i=0; idx_i<2048; idx_i=idx_i+16) {
            set_halt_until_mbus_rx();
            mbus_copy_mem_from_remote_to_any_bulk (
                FLS_ADDR,                   // source prefix
                (uint32_t *) (idx_i << 2),  // source memory address
                PRC_ADDR,                   // destination prefix
                rsvd_16,                    // destination memory address
                15                          // length - 1
            );

            set_halt_until_mbus_tx();
            for (idx_j=0; idx_j<16; idx_j++) {
                if (rsvd_16[idx_j] != 0x00000000) {
                    mbus_msg32_dly_0 (0xED, 0xDEADBEEF);
                    mbus_msg32_dly_0 (0xED, idx_i);
                    mbus_msg32_dly_0 (0xED, (idx_i << 2));
                    mbus_msg32_dly_0 (0xED, idx_j);
                    mbus_msg32_dly_0 (0xED, rsvd_16[idx_j]);
                    mbus_msg32_dly_0 (0xED, 0xDEADBEEF);
                    mbus_sleep_all();
                }
            }
        }
    }
    else if (op_id == 0x6) { // Program
        for (idx_i=8; idx_i<128; idx_i++){
            page_addr = (idx_i << 8);
            mbus_write_message32 ((FLS_ADDR << 4), ((0x09 << 24) | page_addr)); // Set FLSH_START_ADDR
            mbus_msg32_irq_7 (0x5D, (FLS_ADDR << 4), ((0x07 << 24) | 0x01FFE7)); // Fast Program (8kB)
        }
    }
    else if (op_id == 0x7) { // Check Program
        for (idx_i=8; idx_i<128; idx_i=idx_i+8){
            page_addr = (idx_i << 8);
            mbus_write_message32 ((FLS_ADDR << 4), ((0x09 << 24) | page_addr)); // Set FLSH_START_ADDR
            mbus_msg32_irq_7 (0x2B, (FLS_ADDR << 4), ((0x07 << 24) | 0x01FFE3)); // Flash -> SRAM (8kB)

            for (idx_j=0; idx_j<2048; idx_j=idx_j+16) {
                set_halt_until_mbus_rx();
                mbus_copy_mem_from_remote_to_any_bulk (
                    FLS_ADDR,                   // source prefix
                    (uint32_t *) (idx_j << 2),  // source memory address
                    PRC_ADDR,                   // destination prefix
                    rsvd_16,                    // destination memory address
                    15                          // length - 1
                );

                set_halt_until_mbus_tx();
                for (idx_k=0; idx_k<16; idx_k++) {
                    if (rsvd_16[idx_k] != 0x00000000) {
                        mbus_msg32_dly_0 (0xEC, 0xDEADBEEF);
                        mbus_msg32_dly_0 (0xEC, idx_j);
                        mbus_msg32_dly_0 (0xEC, (idx_j << 2));
                        mbus_msg32_dly_0 (0xEC, idx_k);
                        mbus_msg32_dly_0 (0xEC, rsvd_16[idx_k]);
                        mbus_msg32_dly_0 (0xEC, 0xDEADBEEF);
                        mbus_sleep_all();
                    }
                }
            }
        }
    }
    else if (op_id == 0x8) { // Read
        for (idx_i=0; idx_i<32; idx_i++) {
            for (idx_j=0; idx_j<128; idx_j++){
                page_addr = (idx_j << 8);
                mbus_write_message32 ((FLS_ADDR << 4), ((0x09 << 24) | page_addr)); // Set FLSH_START_ADDR
                mbus_msg32_irq_7 (0x2B, (FLS_ADDR << 4), ((0x07 << 24) | 0x01FFE7)); // Fast Program (8kB)
            }
        }
    }

    // Turn off Flash
    mbus_msg32_irq_7 (0xBB, (FLS_ADDR << 4), ((0x11 << 24) | 0x00002D)); // Turn off VC and Flash (0xB5)
}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    // Initialize IRQ
    disable_all_irq();

    if (*REG_CHIP_ID != 0xBEEF) {
        // Reset Sleep Timer
        set_wakeup_timer (100, 0, 1);
        // Variable Reset & Enumeration
        initialization();
        // Go to indefinite sleep
        mbus_sleep_all();
        while(1);
    }

    cyc_num++;

    mbus_msg32_dly_0 (0xE0, cyc_num);
    mbus_msg32_dly_0 (0xE1, *REG0);

    flash_operation (*REG0);

    // Go to indefinite sleep
    mbus_msg32_dly_0 (0xE2, 0x0EA7F00D);
    mbus_sleep_all();
    while(1) {
        asm("nop;");
    }

    return 1;
}
