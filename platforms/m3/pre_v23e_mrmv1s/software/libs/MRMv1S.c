
#include "MRMv1S.h"

void mrm_init(uint32_t mrm_prefix, uint32_t irq_reg_idx) {

    __mrm_prefix__ = mrm_prefix;

    // CLK_GEN Setting
    //      With CLK_GEN_S = 58, then the measured frequecies will be:
    //          CLK_SLOW = 199 kHz
    //          CLK_FAST = 51 MHz
    mbus_remote_register_write(__mrm_prefix__, 0x26, /*CLK_GEN_S*/ 58);

    // IRQ Register
    mbus_remote_register_write(__mrm_prefix__, 0x0F, 0x0
        /* INT_RPLY_SHORT_ADDR (8'h10) */ | (0x10 << 8)
        /* INT_RPLY_REG_ADDR   (8'h00) */ | ((irq_reg_idx & 0xFF) << 0)
    );
    __mrm_irq_reg_idx__     = irq_reg_idx;
    __mrm_irq_reg_addr__    = ((volatile uint32_t *) (0xA0000000 | (__mrm_irq_reg_idx__ << 2)));

    // Enable Auto Power ON/OFF
    mbus_remote_register_write(__mrm_prefix__, 0x12, 0x0
        /* MRAM_PWR_AUTO_OFF (1'h0) */  | (0x1 << 1)
        /* MRAM_PWR_AUTO_ON  (1'h0) */  | (0x1 << 0)
    );

    // --- TMC Configuration
    mrm_disable_tmc_rst_auto_wk();
    
    // --- LDO Voltage Tune
    mbus_remote_register_write(__mrm_prefix__, 0x23, 0x0
       /* LDO_SELB_I_CTRL_LDO_0P8 (5'h01) */ | (0x1F << 19) 
       /* LDO_SELB_I_CTRL_LDO_1P8 (5'h01) */ | (0x1F << 14) 
       /* LDO_SELB_VOUT_LDO_1P8   (4'h7 ) */ | (0x7  << 10) 
       /* LDO_SELB_VOUT_LDO_BUF   (4'h4 ) */ | (0x8  <<  6) 
       /* LDO_SEL_IBIAS_LDO_0P8   (2'h1 ) */ | (0x1  <<  4) 
       /* LDO_SEL_IBIAS_LDO_1P8   (2'h1 ) */ | (0x1  <<  2) 
       /* LDO_SEL_IBIAS_LDO_BUF   (2'h1 ) */ | (0x1  <<  0) 
    );

}

void mrm_disable_tmc_rst_auto_wk(void) {

    set_halt_until_mbus_trx();
    mbus_remote_register_read(__mrm_prefix__, 0x2F, __mrm_irq_reg_idx__);
    set_halt_until_mbus_tx();

    uint32_t temp_reg = *__mrm_irq_reg_addr__ & 0xFFFFDC;   // Reset TMC_RST_AUTO_WK, TMC_DO_REG_LOAD, TMC_DO_AWK
    temp_reg |=   (0x0 << 5)  // TMC_RST_AUTO_WK = 0
                | (0x1 << 1)  // TMC_DO_REG_LOAD = 1
                | (0x1 << 0); // TMC_DO_AWK = 1
    mbus_remote_register_write(__mrm_prefix__, 0x2F, temp_reg);
}

void mrm_enable_tmc_rst_auto_wk(void) {

    set_halt_until_mbus_trx();
    mbus_remote_register_read(__mrm_prefix__, 0x2F, __mrm_irq_reg_idx__);
    set_halt_until_mbus_tx();

    uint32_t temp_reg = *__mrm_irq_reg_addr__ & 0xFFFFDC;   // Reset TMC_RST_AUTO_WK, TMC_DO_REG_LOAD, TMC_DO_AWK
    temp_reg |=   (0x1 << 5)  // TMC_RST_AUTO_WK = 1
                | (0x1 << 1)  // TMC_DO_REG_LOAD = 1
                | (0x1 << 0); // TMC_DO_AWK = 1
    mbus_remote_register_write(__mrm_prefix__, 0x2F, temp_reg);
}

void mrm_enable_bist(void) {
    mbus_remote_register_write(__mrm_prefix__, 0x2C, 0x0
        /* TMC_SCAN_RST_N (1'h0) */ | (0x0 << 1)
        /* TMC_SCAN_TEST  (1'h0) */ | (0x0 << 0)
    );
    mbus_remote_register_write(__mrm_prefix__, 0x29, 0x0
        /* BIST_ENABLE   (1'h0) */ | (0x1 << 0)
    );
}

void mrm_disable_bist(void) {
    mbus_remote_register_write(__mrm_prefix__, 0x29, 0x0
        /* BIST_ENABLE   (1'h0) */ | (0x0 << 0)
    );
    mbus_remote_register_write(__mrm_prefix__, 0x2C, 0x0
        /* TMC_SCAN_RST_N (1'h0) */ | (0x0 << 1)
        /* TMC_SCAN_TEST  (1'h0) */ | (0x0 << 0)
    );
}

void mrm_set_clock_mode(uint32_t clock_mode) {

    set_halt_until_mbus_trx();
    mbus_remote_register_read(__mrm_prefix__, 0x2F, __mrm_irq_reg_idx__);
    set_halt_until_mbus_tx();

    uint32_t temp_reg = *__mrm_irq_reg_addr__ & 0xFFFFEF;   // Reset TMC_CLOCK_MODE to 0.
    temp_reg |= ((clock_mode&0x1) << 4); // Sets TMC_CLOCK_MODE = clock_mode
    mbus_remote_register_write(__mrm_prefix__, 0x2F, temp_reg);
}

void mrm_tune_ldo (uint32_t i0p8, uint32_t i1p8, uint32_t v0p8, uint32_t v1p8) {

    set_halt_until_mbus_trx();
    mbus_remote_register_read(__mrm_prefix__, 0x23, __mrm_irq_reg_idx__);
    set_halt_until_mbus_tx();

    uint32_t temp_reg = *__mrm_irq_reg_addr__ & 0x00003F;
    temp_reg |= (i0p8<<19)|(i1p8<<14)|(v1p8<<10)|(v0p8<<6);
    mbus_remote_register_write(__mrm_prefix__, 0x23, temp_reg);
}

uint32_t mrm_turn_on_ldo (void) {

    enable_reg_irq(/*reg*/__mrm_irq_reg_idx__);

    mbus_remote_register_write(__mrm_prefix__, 0x21, 0x0
        /* LDO_PWR_IRQ_EN (1'h1) */ | (0x1 << 2)
        /* LDO_PWR_SEL_ON (1'h1) */ | (0x1 << 1)
        /* LDO_PWR_GO     (1'h0) */ | (0x1 << 0)
    );

    WFI();

    if ((*__mrm_irq_reg_addr__&0xFF) != 0x7C) return 0;

    // Delay to charge the DCP_0P8 decap
    delay(2000); // delay(1000) =~ 0.6s
    return 1;
}

uint32_t mrm_turn_off_ldo (void) {

    enable_reg_irq(/*reg*/__mrm_irq_reg_idx__);

    mbus_remote_register_write(__mrm_prefix__, 0x21, 0x0
        /* LDO_PWR_IRQ_EN (1'h1) */ | (0x1 << 2)
        /* LDO_PWR_SEL_ON (1'h1) */ | (0x0 << 1)
        /* LDO_PWR_GO     (1'h0) */ | (0x1 << 0)
    );

    WFI();

    if ((*__mrm_irq_reg_addr__&0xFF) != 0x03) return 0;

    return 1;
}

uint32_t mrm_turn_on_macro (uint32_t mid) {
    // --- Select Macro by Writing into MRAM_START_ADDR
    mbus_remote_register_write(__mrm_prefix__, 0x08, /*MRAM_START_ADDR*/mid << 19);

    enable_reg_irq(/*reg*/__mrm_irq_reg_idx__);

    mbus_remote_register_write(__mrm_prefix__, 0x11, 0x0
        /* MRM_PWR_IRQ_EN (1'h1) */ | (0x1 << 2)
        /* MRM_PWR_SEL_ON (1'h1) */ | (0x1 << 1)
        /* MRM_PWR_GO     (1'h0) */ | (0x1 << 0)
    );

    WFI();

    if ((*__mrm_irq_reg_addr__&0xFF) != 0x94) return 0;

    return 1;
}

uint32_t mrm_turn_off_macro (void) {

    enable_reg_irq(/*reg*/__mrm_irq_reg_idx__);

    mbus_remote_register_write(__mrm_prefix__, 0x11, 0x0
        /* MRM_PWR_IRQ_EN (1'h1) */ | (0x1 << 2)
        /* MRM_PWR_SEL_ON (1'h1) */ | (0x0 << 1)
        /* MRM_PWR_GO     (1'h0) */ | (0x1 << 0)
    );

    WFI();

    if ((*__mrm_irq_reg_addr__&0xFF) != 0x98) return 0;

    return 1;
}

void mrm_set_tpwr(uint32_t tpwr) {

    set_halt_until_mbus_trx();
    mbus_remote_register_read(__mrm_prefix__, 0x03, __mrm_irq_reg_idx__);
    set_halt_until_mbus_tx();

    uint32_t temp_reg = *__mrm_irq_reg_addr__ & 0x0000FF;   // Reset Tpwr
    temp_reg |= (tpwr << 8);
    mbus_remote_register_write(__mrm_prefix__, 0x03, temp_reg);
}

uint32_t mrm_cmd_go (uint32_t cmd, uint32_t len_1, uint32_t expected) {

    enable_reg_irq(/*reg*/__mrm_irq_reg_idx__);

    mbus_remote_register_write(__mrm_prefix__, 0x09, 0x0
        /* LENGTH (14'h0000) */ | (len_1 << 6)    
        /* IRQ_EN (    1'h0) */ | (0x1 << 5)    
        /* CMD    (    4'h0) */ | (cmd << 1)    
        /* GO     (    1'h0) */ | (0x1 << 0)    
    );

    WFI();

    if (expected == 0) 
        return 1;
    else {
        uint32_t response = (*__mrm_irq_reg_addr__&0xFF);
        
        if (response == expected) return 1;
        else return response;
    }
}

void mrm_cmd_go_noirq (uint32_t cmd, uint32_t len_1) {

    mbus_remote_register_write(__mrm_prefix__, 0x09, 0x0
        /* LENGTH (14'h0000) */ | (len_1 << 6)    
        /* IRQ_EN (    1'h0) */ | (0x0 << 5)    
        /* CMD    (    4'h0) */ | (cmd << 1)    
        /* GO     (    1'h0) */ | (0x1 << 0)    
    );

}

void mrm_set_sram_addr (uint32_t addr) {
    // Set SRAM Start Address
    mbus_remote_register_write(__mrm_prefix__, 0x07, addr);
}

void mrm_set_mram_addr (uint32_t addr) {
    // Set MRAM Start Address
    mbus_remote_register_write(__mrm_prefix__, 0x08, addr);
}


uint32_t mrm_mram2sram (uint32_t mram_pid, uint32_t num_pages, uint32_t sram_pid) {
    mrm_set_sram_addr(/*addr*/sram_pid<<5);
    mrm_set_mram_addr(/*addr*/mram_pid<<5);  // 32 words/page
    return mrm_cmd_go(/*cmd*/MRM_CMD_CP_MRAM2SRAM, /*len_1*/(num_pages<<5)-1, /*expected*/MRM_EXP_CP_MRAM2SRAM);
}

uint32_t mrm_sram2mram (uint32_t sram_pid, uint32_t num_pages, uint32_t mram_pid) {
    mrm_set_sram_addr(/*addr*/sram_pid<<5); // 32 words/page
    mrm_set_mram_addr(/*addr*/mram_pid<<5); // 32 words/page
    return mrm_cmd_go(/*cmd*/MRM_CMD_CP_SRAM2MRAM, /*len_1*/(num_pages<<5)-1, /*expected*/MRM_EXP_CP_SRAM2MRAM);
}

void mrm_read_sram (uint32_t* mrm_sram_addr, uint32_t num_words, uint32_t* prc_sram_addr) {
    set_halt_until_mbus_trx();
    mbus_copy_mem_from_remote_to_any_bulk(
        /*source_prefix  */ __mrm_prefix__,
        /*source_mem_addr*/ mrm_sram_addr,
        /*dest_prefix    */ 0x1,
        /*dest_mem_addr  */ prc_sram_addr,
        /*length_minus_1 */ num_words - 1
        );
    set_halt_until_mbus_tx();
}

void mrm_read_sram_debug (uint32_t* mrm_sram_addr, uint32_t num_words, uint32_t dest_prefix) {
    mbus_copy_mem_from_remote_to_any_bulk(
        /*source_prefix  */ __mrm_prefix__,
        /*source_mem_addr*/ mrm_sram_addr,
        /*dest_prefix    */ dest_prefix,
        /*dest_mem_addr  */ (uint32_t*) 0x0,
        /*length_minus_1 */ num_words - 1
        );
}

void mrm_write_sram (uint32_t* prc_sram_addr, uint32_t num_words, uint32_t* mrm_sram_addr) {
    mbus_copy_mem_from_local_to_remote_bulk(
        /*remote_prefix  */ __mrm_prefix__,
        /*remote_mem_addr*/ mrm_sram_addr,
        /*local_mem_addr */ prc_sram_addr,
        /*length_minus_1 */ num_words - 1
        );
}

void mrm_read_sram_page (uint32_t mrm_sram_pid, uint32_t num_pages, uint32_t* prc_sram_addr) {
    set_halt_until_mbus_trx();
    mbus_copy_mem_from_remote_to_any_bulk(
        /*source_prefix  */ __mrm_prefix__,
        /*source_mem_addr*/ (uint32_t*) ((mrm_sram_pid<<5)<<2),
        /*dest_prefix    */ 0x1,
        /*dest_mem_addr  */ prc_sram_addr,
        /*length_minus_1 */ (num_pages<<5) - 1
        );
    set_halt_until_mbus_tx();
}

void mrm_read_sram_page_debug (uint32_t mrm_sram_pid, uint32_t num_pages, uint32_t dest_prefix) {
    mbus_copy_mem_from_remote_to_any_bulk(
        /*source_prefix  */ __mrm_prefix__,
        /*source_mem_addr*/ (uint32_t*) ((mrm_sram_pid<<5)<<2),
        /*dest_prefix    */ dest_prefix,
        /*dest_mem_addr  */ (uint32_t*) 0x0,
        /*length_minus_1 */ (num_pages<<5) - 1
        );
}

void mrm_write_sram_page (uint32_t* prc_sram_addr, uint32_t num_pages, uint32_t mrm_sram_pid) {
    mbus_copy_mem_from_local_to_remote_bulk(
        /*remote_prefix  */ __mrm_prefix__,
        /*remote_mem_addr*/ (uint32_t*) ((mrm_sram_pid<<5)<<2),
        /*local_mem_addr */ prc_sram_addr,
        /*length_minus_1 */ (num_pages<<5) - 1
        );
}

uint32_t mrm_read_mram_page (uint32_t mrm_mram_pid, uint32_t num_pages, uint32_t* prc_sram_addr) {
    uint32_t result = mrm_mram2sram(/*mram_pid*/mrm_mram_pid, /*num_pages*/num_pages, /*sram_pid*/0);
    if (result==1) mrm_read_sram_page(/*mrm_sram_pid*/0, /*num_pages*/num_pages, /*prc_sram_addr*/prc_sram_addr);
    return result;
}

uint32_t mrm_read_mram_page_debug (uint32_t mrm_mram_pid, uint32_t num_pages, uint32_t dest_prefix) {
    uint32_t result = mrm_mram2sram(/*mram_pid*/mrm_mram_pid, /*num_pages*/num_pages, /*sram_pid*/0);
    if (result==1) mrm_read_sram_page_debug(/*mrm_sram_pid*/0, /*num_pages*/num_pages, /*dest_prefix*/dest_prefix);
    return result;
}

uint32_t mrm_write_mram_page (uint32_t* prc_sram_addr, uint32_t num_pages, uint32_t mrm_mram_pid) {
    mrm_write_sram_page (/*prc_sram_addr*/prc_sram_addr, /*num_pages*/num_pages, /*mrm_sram_pid*/0);
    return mrm_sram2mram (/*sram_pid*/0, /*num_pages*/num_pages, /*mram_pid*/mrm_mram_pid);
}


uint32_t mrm_pp_ext_stream (uint32_t bit_en, uint32_t num_pages, uint32_t mram_page_id) {

    // --- Ping-Pong General Configuration
    mbus_remote_register_write(__mrm_prefix__, 0x14, 0x0
        /* PP_EARLY_POWER_ON   (1'h1) */  | (0x1 << 5)    // If 1, it powers on the next mram macro if needed, while it is still in PP_WAIT_FOR_STREAM state.
        /* PP_NO_ERR_DETECTION (1'h0) */  | (0x0 << 4)    // If 1, it does not detect buffer_overrun error. Other errors can be stil detected, though.
        /* RESERVED            (1'h0) */  | (0x0 << 3)    // 
        /* PP_WRAP             (1'h0) */  | (0x0 << 2)    // If 1, PP_MRAM_ADDR will wrap around
        /* PP_BIT_EN_EXT       (2'h1) */  | (bit_en << 0) // 
    );

    // --- Set the streaming length
    mbus_remote_register_write(__mrm_prefix__, 0x13, 0x0
        /* PP_STR_LIMIT (21'h000000) */ | ((num_pages<<5) << 1) // Limit the number of words streamed. 0 means no-limit; Must be an integer multiple of NUM_WORDS_PER_PAGE.
        /* PP_STR_EN    ( 1'h0     ) */ | (0x1            << 0) // Ping-Pong Straming Enable
    );

    // --- Set MRAM Address
    mbus_remote_register_write(__mrm_prefix__, 0x15, 0x0
        /* PP_FLAG_END_OF_MRAM ( 1'h0    ) */ | (0x0               << 23)
        /* RESERVED            (3'h0)      */ | (0x0               << 20)
        /* PP_MRAM_ADDR        (20'h00000) */ | ((mram_page_id<<5) <<  0)
    );

    // --- Go into the ping-pong listening mode
    mrm_cmd_go (/*cmd*/MRM_CMD_EXT_WR_SRAM, /*len_1*/0, /*expected*/MRM_EXP_NONE);

    // --- Return the IRQ payload
    return *__mrm_irq_reg_addr__&0xFF;

}

void mrm_pp_ext_stream_unlim (uint32_t bit_en, uint32_t mram_page_id) {

    // --- Ping-Pong General Configuration
    mbus_remote_register_write(__mrm_prefix__, 0x14, 0x0
        /* PP_EARLY_POWER_ON   (1'h1) */  | (0x1 << 5)    // If 1, it powers on the next mram macro if needed, while it is still in PP_WAIT_FOR_STREAM state.
        /* PP_NO_ERR_DETECTION (1'h0) */  | (0x0 << 4)    // If 1, it does not detect buffer_overrun error. Other errors can be stil detected, though.
        /* RESERVED            (1'h0) */  | (0x0 << 3)    // 
        /* PP_WRAP             (1'h0) */  | (0x0 << 2)    // If 1, PP_MRAM_ADDR will wrap around
        /* PP_BIT_EN_EXT       (2'h1) */  | (bit_en << 0) // 
    );

    // --- Set the streaming length
    mbus_remote_register_write(__mrm_prefix__, 0x13, 0x0
        /* PP_STR_LIMIT (21'h000000) */ | (0x0 << 1) // Limit the number of words streamed. 0 means no-limit; Must be an integer multiple of NUM_WORDS_PER_PAGE.
        /* PP_STR_EN    ( 1'h0     ) */ | (0x1 << 0) // Ping-Pong Straming Enable
    );

    // --- Set MRAM Address
    mbus_remote_register_write(__mrm_prefix__, 0x15, 0x0
        /* PP_FLAG_END_OF_MRAM ( 1'h0    ) */ | (0x0               << 23)
        /* RESERVED            (3'h0)      */ | (0x0               << 20)
        /* PP_MRAM_ADDR        (20'h00000) */ | ((mram_page_id<<5) <<  0)
    );

    // --- Go into the ping-pong listening mode
    mbus_remote_register_write(__mrm_prefix__, 0x09, 0x0
        /* LENGTH (14'h0000) */ | (0x0 << 6)    
        /* IRQ_EN (    1'h0) */ | (0x1 << 5)    
        /* CMD    (    4'h0) */ | (MRM_CMD_EXT_WR_SRAM << 1)    
        /* GO     (    1'h0) */ | (0x1 << 0)    
    );

}

uint32_t mrm_stop_pp_ext_stream (void) {

    enable_reg_irq(/*reg*/__mrm_irq_reg_idx__);

    mbus_remote_register_write(__mrm_prefix__, 0x13, 0x0
        /* PP_STR_LIMIT (21'h000000) */ | (0x0 << 1) // Limit the number of words streamed. 0 means no-limit; Must be an integer multiple of NUM_WORDS_PER_PAGE.
        /* PP_STR_EN    ( 1'h0     ) */ | (0x0 << 0) // Ping-Pong Straming Enable
    );

    WFI();

    // --- Return the IRQ payload
    return *__mrm_irq_reg_addr__&0xFF;
}










//uint32_t meas_clk_freq (void) {
//    // [NOTE] Seems like the MEAS_CLK_FREQ command does not work.
//    //          It may be due to the wrong condition defined in MRMv1S_def_common.v, which is:
//    //              `define TPCOND_FULL (time_par_cnt[19:0] == {`OP_TIMEPAR_WIDTH{1'b1}})
//    //          Since `OP_TIMEPAR_WIDTH is 24, the above condition may never be met.
//    //
//    //          Thus, here I am using an indirect approach, using Tpwr.
//    //          I turn on the MRAM macro w/ Tpwr=0 and Tpwr=0xFFFF, and see the difference.
//    //
//    //          Results:
//    //              S=87:
//    //                  Measurement
//    //                      Delay w/ Tpwr=0x0000: 0.3922ms
//    //                      Delay w/ Tpwr=0xFFFF: 0.4798s
//    //                      CLK_SLOW = 65536 / (0.4798 - 0.0003922) = 136.702 kHz
//    //                      CLK_FAST = CLK_SLOW x 256 = 34.996 MHz
//    //                  Previous Simulation:
//    //                      CLK_SLOW = 240 kHz
//    //                      CLK_FAST = 61 MHz
//    //              S=58:
//    //                  Measurement
//    //                      Delay w/ Tpwr=0x0000: 0.2652ms
//    //                      Delay w/ Tpwr=0xFFFF: 0.3294s
//    //                      CLK_SLOW = 65536 / (0.3294 - 0.0002652) = 199.115 kHz
//    //                      CLK_FAST = CLK_SLOW x 256 = 50.973 MHz
//    //                  Previous Simulation:
//    //                      CLK_SLOW = 348 kHz
//    //                      CLK_FAST = 89 MHz
//    //
//    //          CONCLUSION:
//    //              Use CLK_GEN_S = 58, then the measured frequecies will be:
//    //                  CLK_SLOW = 199 kHz
//    //                  CLK_FAST = 51 MHz
//    
//    // --- CLK_GEN Tuning (Default: CLK_GEN_S = 87)
//    mbus_remote_register_write(__mrm_prefix__, 0x26, /*CLK_GEN_S*/ 58);
//
//    // --- TMC Configuration
//    mbus_remote_register_write(__mrm_prefix__, 0x2F, 0x0
//        /* TMC_RST_AUTO_WK  (1'h1) */   | (0x0 << 5)  // If 1, TMC automatically issues AUTO_WAKEUP upon its reset release. This is handled by TMC itself, not the CTRL. TMC_RST_AUTO_WK != TMC_DO_AWK is recommended.
//        /* TMC_CLOCK_MODE   (1'h0) */   | (0x0 << 4)  // See the table in genRF.conf
//        /* TMC_CHECK_ERR    (1'h0) */   | (0x0 << 3)  // If 1, it checks TMC_ERR and terminates the operation if an error occurs. If 0, it ignores TMC_ERR and proceeds.
//        /* TMC_FAST_LOAD    (1'h1) */   | (0x0 << 2)  // If 1, it does not check TMC_BUSY or TMC_ERR for LOAD command. It also performs 4 SRAM readings without a cease.
//        /* TMC_DO_REG_LOAD  (1'h1) */   | (0x0 << 1)  // If 1, it overwrites the selected TMC registers after auto-wakeup (Valid only when TMC_DO_AWK=1)
//        /* TMC_DO_AWK       (1'h0) */   | (0x0 << 0)  // If 1, it issues TMC's Auto-Wakeup Command upon power-up. This is handled by the CTRL, not the TMC. TMC_RST_AUTO_WK != TMC_DO_AWK is recommended. For now, let's do TMC_DO_AWK=0.
//    );
//
//    // --- Tpwr=0
//    mbus_remote_register_write(__mrm_prefix__, 0x03, 0x0
//        /* Tpwr   (16'd200) */ | (0x0000 << 8)
//        /* T100ns ( 8'd0)   */ | (  0x00 << 0)
//    );
//
//    // --- Power-On 
//    *NVIC_ISER = (1 << IRQ_REG0);
//    // REGISTER 0x11
//    //       [2] MRAM_PWR_IRQ_EN: 1'b1
//    //       [1] MRAM_PWR_SEL_ON: 1'b1
//    //       [0] MRAM_PWR_GO    : 1'b1
//    // -----------------------------------------------
//    mbus_remote_register_write(__mrm_prefix__, 0x11, 0x000007);
//    WFI();
//    *NVIC_ICER = (1 << IRQ_REG0);
//
//    if ((*REG0&0xFF) != 0x94) return 0;
//
//    // --- Power-Off
//    *NVIC_ISER = (1 << IRQ_REG0);
//    // REGISTER 0x11
//    //       [2] MRAM_PWR_IRQ_EN: 1'b1
//    //       [1] MRAM_PWR_SEL_ON: 1'b0
//    //       [0] MRAM_PWR_GO    : 1'b1
//    // -----------------------------------------------
//    mbus_remote_register_write(__mrm_prefix__, 0x11, 0x000005);
//    WFI();
//    *NVIC_ICER = (1 << IRQ_REG0);
//
//    if ((*REG0&0xFF) != 0x98) return 0;
//
//    // --- Tpwr=0xFFFF
//    mbus_remote_register_write(__mrm_prefix__, 0x03, 0x0
//        /* Tpwr   (16'd200) */ | (0xFFFF << 8)
//        /* T100ns ( 8'd0)   */ | (  0x00 << 0)
//    );
//
//    // --- Power-On 
//    *NVIC_ISER = (1 << IRQ_REG0);
//    // REGISTER 0x11
//    //       [2] MRAM_PWR_IRQ_EN: 1'b1
//    //       [1] MRAM_PWR_SEL_ON: 1'b1
//    //       [0] MRAM_PWR_GO    : 1'b1
//    // -----------------------------------------------
//    mbus_remote_register_write(__mrm_prefix__, 0x11, 0x000007);
//    WFI();
//    *NVIC_ICER = (1 << IRQ_REG0);
//
//    if ((*REG0&0xFF) != 0x94) return 0;
//
//    // --- Power-Off
//    *NVIC_ISER = (1 << IRQ_REG0);
//    // REGISTER 0x11
//    //       [2] MRAM_PWR_IRQ_EN: 1'b1
//    //       [1] MRAM_PWR_SEL_ON: 1'b0
//    //       [0] MRAM_PWR_GO    : 1'b1
//    // -----------------------------------------------
//    mbus_remote_register_write(__mrm_prefix__, 0x11, 0x000005);
//    WFI();
//    *NVIC_ICER = (1 << IRQ_REG0);
//
//    if ((*REG0&0xFF) != 0x98) return 0;
//
//
//    return 1;
//
//}

