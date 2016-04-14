//*******************************************************************
//Author: Yejoong Kim
//Description: MOD Flash Demo
//*******************************************************************
#include "PRCv13.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define FLS_ADDR    0x4
#define FLP_ADDR    0x8

#define MBUS_DELAY  1000
#define FAIL_DELAY  10000

#define PAR_WORDS   128

// States
#define MF_FSM_FLS          0x01
#define MF_FSM_FLP          0x02
#define MF_FSM_BOTH         0x03

#define MF_IDLE_FLS         0x11
#define MF_IDLE_FLP         0x12
#define MF_IDLE_BOTH        0x13

#define MF_ERASE_FLS        0x21
#define MF_ERASE_FLP        0x22

#define MF_PROG_FLS         0x31
#define MF_PROG_FLP         0x32

#define MF_READ_FLS         0x41
#define MF_READ_FLP         0x42

#define MF_ALL1_CHK_FLS     0x51
#define MF_ALL1_CHK_FLP     0x52

#define MF_ALL0_CHK_FLS     0x61
#define MF_ALL0_CHK_FLP     0x62

#define MF_ALL1_SRAM_FLS    0x71
#define MF_ALL1_SRAM_FLP    0x72

#define MF_ALL0_SRAM_FLS    0x81
#define MF_ALL0_SRAM_FLP    0x82

#define MF_SLEEP            0x90


//********************************************************************
// Memory Space in FLS and FLP
//********************************************************************
// <FLSv2S>
// 8kB SRAM (Total 2048 words): 0x000 - 0x7FF (0x0000 - 0x1FFC)
// 2Mb Flash (2kB/Page, 128 Pages): 0x0000 - 0xFFFF (0x200 per Page)
//
// <FLPv1S>
// 8kB SRAM (Total 2048 words): 0x000 - 0x7FF (0x0000 - 0x1FFC)
// 1Mb Flash (1kB/Page, 128 Pages): 0x0000 - 0x7FFF (0x100 per Page)
//

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t cyc_num;
volatile uint32_t irq_history;
volatile uint32_t mem_rsvd[PAR_WORDS];

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
void MF_initialization (void) {
    *REG_CHIP_ID = 0xBEEF;
    cyc_num = 0;
    irq_history = 0;

    // Enumeration
    set_halt_until_mbus_rx();
    mbus_enumerate(FLS_ADDR);
    mbus_enumerate(FLP_ADDR);

    // FLS/FLP Tuning
    set_halt_until_mbus_tx();

    // FLS
    mbus_write_message32 ((FLS_ADDR << 4), (0x19 << 24) | 0x3C4703); // Voltage Clamper Tuning (Same as in FLP)
    mbus_write_message32 ((FLS_ADDR << 4), (0x02 << 24) | 0x000100); // Terase (may need to be longer)
    mbus_write_message32 ((FLS_ADDR << 4), (0x04 << 24) | 0x000020); // Tcyc_prog
    mbus_write_message32 ((FLS_ADDR << 4), (0x0F << 24) | 0x001007); // IRQ Reply Address
    
    // FLP
    mbus_write_message32 ((FLP_ADDR << 4), (0x19 << 24) | 0x000F03); // Voltage Clamper Tuning
    mbus_write_message32 ((FLP_ADDR << 4), (0x02 << 24) | 0x000100); // Terase
    mbus_write_message32 ((FLP_ADDR << 4), (0x04 << 24) | 0x000020); // Tcyc_prog
    mbus_write_message32 ((FLP_ADDR << 4), (0x0F << 24) | 0x001007); // IRQ Reply Address
}

void MF_mbus_msg32_dly (uint32_t addr, uint32_t data) {
    mbus_write_message32 (addr, data);
    delay(MBUS_DELAY);
}

void MF_mbus_msg32_irq_7 (uint32_t payload, uint32_t addr, uint32_t data) {
    // IRQ Payload must be stored in REG7.
    set_halt_until_mbus_rx();
    mbus_write_message32 (addr, data);
    set_halt_until_mbus_tx();
    if (*REG7 != payload) {
        delay(FAIL_DELAY);
        MF_mbus_msg32_dly (0xEF, 0xDEADBEEF);
        MF_mbus_msg32_dly (0xE1, addr);
        MF_mbus_msg32_dly (0xE2, data);
        MF_mbus_msg32_dly (0xE3, payload);
        MF_mbus_msg32_dly (0xE4, *REG7);
        MF_mbus_msg32_dly (0xEF, 0xDEADBEEF);
        mbus_sleep_all();
    }
}

void MF_fail (uint32_t id, uint32_t data0, uint32_t data1, uint32_t data2, uint32_t data3) {
    delay(FAIL_DELAY);
    MF_mbus_msg32_dly (0xEE, 0xDEADBEEF);
    MF_mbus_msg32_dly (0xE1, id);
    MF_mbus_msg32_dly (0xE2, data0);
    MF_mbus_msg32_dly (0xE3, data1);
    MF_mbus_msg32_dly (0xE4, data2);
    MF_mbus_msg32_dly (0xE5, data3);
    MF_mbus_msg32_dly (0xEE, 0xDEADBEEF);
    mbus_sleep_all();
}

void MF_flash_operation (uint32_t op_id) {
    
    uint32_t idx_i, idx_j, idx_k;
    uint32_t targ_addr, page_addr, num_shift, num_pages, prog_cmd;

    uint32_t debug_id = 0;
    uint32_t correct_data = 0;

    uint32_t func_id  = (op_id & 0x000000F0) >> 4;
    uint32_t flash_id = (op_id & 0x0000000F);

    uint32_t do_fls = ((flash_id == 0x1) | (flash_id == 0x3));
    uint32_t do_flp = ((flash_id == 0x2) | (flash_id == 0x3));

    uint32_t do_fsm       = (func_id == 0x0);
    uint32_t do_idle      = (func_id == 0x1);
    uint32_t do_erase     = (func_id == 0x2);
    uint32_t do_prog      = (func_id == 0x3);
    uint32_t do_read      = (func_id == 0x4);
    uint32_t do_all1_chk  = (func_id == 0x5);
    uint32_t do_all0_chk  = (func_id == 0x6);
    uint32_t do_all1_sram = (func_id == 0x7);
    uint32_t do_all0_sram = (func_id == 0x8);
    uint32_t do_sleep     = (func_id == 0x9);

    uint32_t do_use_flash_fls = do_fls & (do_idle | do_erase | do_prog | do_read | do_all1_chk | do_all0_chk);
    uint32_t do_use_flash_flp = do_flp & (do_idle | do_erase | do_prog | do_read | do_all1_chk | do_all0_chk);

    if      (do_fls) {targ_addr = FLS_ADDR; num_shift = 9; num_pages = 256; prog_cmd = 0x0001FFE5;} // Normal Program
    else if (do_flp) {targ_addr = FLP_ADDR; num_shift = 8; num_pages = 128; prog_cmd = 0x0001FFE7;} // Fast Program
    else             {targ_addr = 0; num_shift = 0; num_pages = 0; prog_cmd = 0;}

    // Turn on VC and Flash
    if (do_use_flash_fls) MF_mbus_msg32_irq_7 (0xB5, (FLS_ADDR << 4), ((0x11 << 24) | 0x00003F));
    if (do_use_flash_flp) MF_mbus_msg32_irq_7 (0xB5, (FLP_ADDR << 4), ((0x11 << 24) | 0x00002F));

    // Test Cases: FSM Power Measurement
    if (do_fsm) {
        if (do_fls) mbus_write_message32 ((FLS_ADDR << 4), 0x08000000); 
        if (do_flp) mbus_write_message32 ((FLP_ADDR << 4), 0x08000000); 
        while(1) { asm("nop;"); }
    }
    // Test Cases: Flash Idle Power Measurement
    else if (do_idle) {
        while(1) { asm("nop;"); }
    }
    // Test Cases: Flash Erase Power Measurement
    else if (do_erase) {
        for (idx_i=0; idx_i<128; idx_i++){
            page_addr = (idx_i << num_shift);
            mbus_write_message32 ((targ_addr << 4), ((0x09 << 24) | page_addr)); // Set FLSH_START_ADDR
            set_halt_until_mbus_rx();
            mbus_write_message32 ((targ_addr << 4), ((0x07 << 24) | 0x000029)); // Page Erase
            set_halt_until_mbus_tx();
        }
    }
    // Test Cases: Flash Program Power Measurement
    else if (do_prog) {
        for (idx_i=0; idx_i<num_pages; idx_i=idx_i+8) {
            page_addr = (idx_i << 8);
            mbus_write_message32 ((targ_addr << 4), ((0x09 << 24) | page_addr)); // Set FLSH_START_ADDR
            set_halt_until_mbus_rx();
            mbus_write_message32 ((targ_addr << 4), ((0x07 << 24) | prog_cmd)); // SRAM -> Flash (8kB)
            set_halt_until_mbus_tx();
        }
    }
    // Test Cases: Flash Read Power Measurement
    else if (do_read) {
        for (idx_i=0; idx_i<num_pages; idx_i=idx_i+8) {
            page_addr = (idx_i << 8);
            mbus_write_message32 ((targ_addr << 4), ((0x09 << 24) | page_addr)); // Set FLSH_START_ADDR
            set_halt_until_mbus_rx();
            mbus_write_message32 ((targ_addr << 4), ((0x07 << 24) | 0x01FFE3)); // Flash -> SRAM (8kB)
            set_halt_until_mbus_tx();
        }
    }
    // Test Cases: All 1 Check / All 0 Check
    else if (do_all1_chk | do_all0_chk) {
        if (do_fls) {
            if      (do_all1_chk) {debug_id = MF_ALL1_CHK_FLS; correct_data = 0xFFFFFFFF;}
            else if (do_all0_chk) {debug_id = MF_ALL0_CHK_FLS; correct_data = 0x00000000;}
        }
        else if (do_flp) {
            if      (do_all1_chk) {debug_id = MF_ALL1_CHK_FLP; correct_data = 0xFFFFFFFF;}
            else if (do_all0_chk) {debug_id = MF_ALL0_CHK_FLP; correct_data = 0x00000000;}
        }

        for (idx_i=0; idx_i<num_pages; idx_i=idx_i+8){
            page_addr = (idx_i << 8);
            mbus_write_message32 ((targ_addr << 4), ((0x09 << 24) | page_addr)); // Set FLSH_START_ADDR
            MF_mbus_msg32_irq_7 (0x27, (targ_addr << 4), ((0x07 << 24) | 0x01FFE3)); // Flash -> SRAM (8kB)

            for (idx_j=0; idx_j<2048; idx_j=idx_j+PAR_WORDS) {
                set_halt_until_mbus_rx();
                mbus_copy_mem_from_remote_to_any_bulk (
                    targ_addr,                  // source prefix
                    (uint32_t *) (idx_j << 2),  // source memory address
                    PRC_ADDR,                   // destination prefix
                    mem_rsvd,                   // destination memory address
                    (PAR_WORDS - 1)             // length - 1
                );

                set_halt_until_mbus_tx();
                for (idx_k=0; idx_k<PAR_WORDS; idx_k++)
                    if (mem_rsvd[idx_k] != correct_data) 
                        MF_fail (debug_id, idx_i, idx_j, idx_k, mem_rsvd[idx_k]);
            }
        }
    }
    // Test Cases: Write All 0 into SRAM / Write All 1 into SRAM
    else if (do_all1_sram | do_all0_sram) {
        uint32_t temp_data[PAR_WORDS];

        if      (do_all1_sram) correct_data = 0xFFFFFFFF;
        else if (do_all0_sram) correct_data = 0x00000000;

        for (idx_i=0; idx_i<PAR_WORDS; idx_i++) temp_data[idx_i] = correct_data;

        for (idx_i=0; idx_i<2048; idx_i=idx_i+PAR_WORDS){
            mbus_copy_mem_from_local_to_remote_bulk (
		        targ_addr,                  // remote_prefix
                (uint32_t *) (idx_i << 2),  // remote_memory_address
                temp_data,                  // local_address
                (PAR_WORDS - 1)             // length_in_words_minus_one
            );
        }

        while(1) { asm("nop;"); }
    }
    // Test Cases: Go to Sleep
    else if (do_sleep) {
        MF_mbus_msg32_irq_7 (0xBB, (FLS_ADDR << 4), ((0x11 << 24) | 0x00003D));
        MF_mbus_msg32_irq_7 (0xBB, (FLP_ADDR << 4), ((0x11 << 24) | 0x00002D));
        mbus_sleep_all();
        while(1) { asm("nop;"); }
    }

    // Turn off VC and Flash
    if (do_use_flash_fls) MF_mbus_msg32_irq_7 (0xBB, (FLS_ADDR << 4), ((0x11 << 24) | 0x00003D));
    if (do_use_flash_flp) MF_mbus_msg32_irq_7 (0xBB, (FLP_ADDR << 4), ((0x11 << 24) | 0x00002D));
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
        MF_initialization();
        // Go to indefinite sleep
        mbus_sleep_all();
        while(1);
    }

    cyc_num++;

    mbus_write_message32 (0xE0, cyc_num);
    delay(MBUS_DELAY);

    mbus_write_message32 (0xE1, *REG0);
    delay(MBUS_DELAY);

    MF_flash_operation (*REG0);

    mbus_write_message32 (0xE2, 0x0EA7F00D);
    delay(MBUS_DELAY);

    while(1) { asm("nop;"); }
    return 1;
}
