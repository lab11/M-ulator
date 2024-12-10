
#include "MRMv1L.h"

void mrm_init(uint32_t irq_reg_idx, uint32_t clk_gen_s) {

    // CLK_GEN Setting
    mbus_remote_register_write(MRM_ADDR, 0x26, /*CLK_GEN_S*/ clk_gen_s);

    // IRQ Register
    mbus_remote_register_write(MRM_ADDR, 0x0F, 0x0
        /* INT_RPLY_SHORT_ADDR (8'h10) */ | (0x10 << 8)
        /* INT_RPLY_REG_ADDR   (8'h00) */ | ((irq_reg_idx & 0xFF) << 0)
    );
    __mrm_irq_reg_idx__     = irq_reg_idx;
    __mrm_irq_reg_addr__    = ((volatile uint32_t *) (0xA0000000 | (__mrm_irq_reg_idx__ << 2)));

    // Enable Auto Power ON/OFF
    mrm_enable_auto_power_on_off();

    // --- TMC Configuration
    mrm_enable_tmc_rst_auto_wk();

    // --- Use CLOCK_MODE=1, which is more energy-efficient.
    mrm_set_clock_mode(/*clock_mode*/1);
    
    // --- LDO Voltage Tune
    mbus_remote_register_write(MRM_ADDR, 0x23, 0x0
       /* LDO_SELB_I_CTRL_LDO_0P8 (5'h01) */ | (0x1F << 19) 
       /* LDO_SELB_I_CTRL_LDO_1P8 (5'h01) */ | (0x1F << 14) 
       /* LDO_SELB_VOUT_LDO_1P8   (4'h7 ) */ | (0x7  << 10) 
       /* LDO_SELB_VOUT_LDO_BUF   (4'h4 ) */ | (0x8  <<  6) 
       /* LDO_SEL_IBIAS_LDO_0P8   (2'h1 ) */ | (0x1  <<  4) 
       /* LDO_SEL_IBIAS_LDO_1P8   (2'h1 ) */ | (0x1  <<  2) 
       /* LDO_SEL_IBIAS_LDO_BUF   (2'h1 ) */ | (0x1  <<  0) 
    );


    mbus_remote_register_write(MRM_ADDR, 0x30, 0x0
        /* TPGM          (8'd11 )  */    | (16 << 16)  //  "[FAST CLOCK] Write pulse width; = (TPGM + 1) x Tclk_fast; Min 250ns, Max 275ns"
        /* TW2R_S        (8'd0  )  */    | (0  <<  8)  //  "[SLOW CLOCK] Write to Read; = (16^TW2R[7]) x TW2R[6:0] x Tclk_slow; Min 1us"
        /* TWRS_S        (8'd0  )  */    | (1  <<  0)  //  "[SLOW CLOCK] Read to Write; = (16^TWRS[7]) x TWRS[6:0] x Tclk_slow; Min 3us"
    );
    mbus_remote_register_write(MRM_ADDR, 0x31, 0x0
        /* TRDL_S        (7'd0  )  */    | (0  <<  4)  // "[SLOW CLOCK] READ low pulse; = (TRDL + 1) x Tclk_slow; Min 200ns"
        /* TCYCRD1_S     (4'd0  )  */    | (0  <<  0)  // "[SLOW CLOCK] When the read state exeed 16 clocks, it uses TCYCRD1 to control for BL leak measurement"
    );
    mbus_remote_register_write(MRM_ADDR, 0x32, 0x0
        /* TRDS_S        (9'd0  )  */    | (0  << 12)  //"[SLOW CLOCK] READ Rising to 1st Clk; = TRDS[8] x (Twk[7:0] + 1) x Tclk_slow; Min 100ns"
        /* TWK_S         (8'd0  )  */    | (1  <<  4)  //"[SLOW CLOCK] Wakeup Time; = (16^TWK[7]) x TWK[6:0] x Tclk_slow; Min 3us"
        /* TCYCRD_S      (4'd0  )  */    | (0  <<  0)  //"[SLOW CLOCK] Read Cycle; = (TCYCRD + 1) x Tclk_slow; Min 16.5ns"
    );
    mbus_remote_register_write(MRM_ADDR, 0x33, 0x0
        /* TPGM_OTP_RBD  (7'd29 )  */    | (43 << 12)  // "[FAST CLOCK] OTP RBD Tprog Pulse Width = (16 x TPGM_OTP_RBD + 1) x Tclk_fast; Min 9000ns, Max 11000ns"
        /* TPGM_OTP_RAP  (6'd5  )  */    | (7  <<  6)  // "[FAST CLOCK] OTP RAP Tprog Pulse Width = (TPGM_OTP_RAP + 1) x Tclk_fast; Min 100ns, Max 140ns"
        /* TPGM_OTP_RP   (6'd5  )  */    | (7  <<  0)  // "[FAST CLOCK] OTP RP Tprog Pulse Width = (TPGM_OTP_RP + 1) x Tclk_fast; Min 100ns, Max 140ns"
    );
    mbus_remote_register_write(MRM_ADDR, 0x34, 0x0
        /* TW2R_F        (8'd50 )  */    | (75 << 16)  //  "[FAST CLOCK] Write to Read; = (16^TW2R[7]) x TW2R[6:0] x Tclk_fast; Min 1us"
        /* TWRS_F        (8'd137)  */    | (142<<  8)  //  "[FAST CLOCK] Read to Write; = (16^TWRS[7]) x TWRS[6:0] x Tclk_fast; Min 3us"
        /* TWK_F         (8'd134)  */    | (137<<  0)  //  "[FAST CLOCK] Wakeup Time; = (16^TWK[7]) x TWK[6:0] x Tclk_fast; Min 3us"
    );
    mbus_remote_register_write(MRM_ADDR, 0x35, 0x0
        /* TRDL_F        (7'd10 )  */    | (15 << 17)  // "[FAST CLOCK] READ low pulse; = (TRDL + 1) x Tclk_fast; Min 200ns"
        /* TRDS_F        (9'd5  )  */    | (7  <<  8)  // "[FAST CLOCK] READ Rising to 1st Clk; = TRDS[8] x (Twk[7:0] + 1) x Tclk_fast; Min 100ns"
        /* TCYCRD1_F     (4'd0  )  */    | (0  <<  4)  // "[FAST CLOCK] When the read state exeed 16 clocks, it uses TCYCRD1 to control for BL leak measurement"
        /* TCYCRD_F      (4'd0  )  */    | (1  <<  0)  // "[FAST CLOCK] Read Cycle; = (TCYCRD + 1) x Tclk_fast; Min 16.5ns"
    );




}

void mrm_enable_auto_power_on_off(void) {
    mbus_remote_register_write(MRM_ADDR, 0x12, 0x0
        /* MRAM_PWR_AUTO_OFF (1'h0) */  | (0x1 << 1)
        /* MRAM_PWR_AUTO_ON  (1'h0) */  | (0x1 << 0)
    );
}

void mrm_disable_auto_power_on_off(void) {
    mbus_remote_register_write(MRM_ADDR, 0x12, 0x0
        /* MRAM_PWR_AUTO_OFF (1'h0) */  | (0x0 << 1)
        /* MRAM_PWR_AUTO_ON  (1'h0) */  | (0x0 << 0)
    );
}

void mrm_disable_tmc_rst_auto_wk(void) {

    set_halt_until_mbus_trx();
    mbus_remote_register_read(MRM_ADDR, 0x2F, __mrm_irq_reg_idx__);
    set_halt_until_mbus_tx();

    uint32_t temp_reg = *__mrm_irq_reg_addr__ & 0xFFFFDC;   // Reset TMC_RST_AUTO_WK, TMC_DO_REG_LOAD, TMC_DO_AWK
    temp_reg |=   (0x0 << 5)  // TMC_RST_AUTO_WK = 0
                | (0x1 << 1)  // TMC_DO_REG_LOAD = 1
                | (0x1 << 0); // TMC_DO_AWK = 1
    mbus_remote_register_write(MRM_ADDR, 0x2F, temp_reg);
}

void mrm_enable_tmc_rst_auto_wk(void) {

    set_halt_until_mbus_trx();
    mbus_remote_register_read(MRM_ADDR, 0x2F, __mrm_irq_reg_idx__);
    set_halt_until_mbus_tx();

    uint32_t temp_reg = *__mrm_irq_reg_addr__ & 0xFFFFDC;   // Reset TMC_RST_AUTO_WK, TMC_DO_REG_LOAD, TMC_DO_AWK
    temp_reg |=   (0x1 << 5)  // TMC_RST_AUTO_WK = 1
                | (0x1 << 1)  // TMC_DO_REG_LOAD = 1
                | (0x1 << 0); // TMC_DO_AWK = 1
    mbus_remote_register_write(MRM_ADDR, 0x2F, temp_reg);
}

void mrm_enable_bist(void) {
    mbus_remote_register_write(MRM_ADDR, 0x2C, 0x0
        /* TMC_SCAN_RST_N (1'h0) */ | (0x0 << 1)
        /* TMC_SCAN_TEST  (1'h0) */ | (0x0 << 0)
    );

    mbus_remote_register_write(MRM_ADDR, 0x28, 0x0
        /* EN_OUT_TDO    (1'h0) */ | (0x1 << 7)
        /* EN_PD_TDI     (1'h1) */ | (0x1 << 6)
        /* EN_PD_TMS     (1'h1) */ | (0x1 << 5)
        /* EN_PD_TCK     (1'h1) */ | (0x1 << 4)
        /* EN_PD_BOOT_EN (1'h1) */ | (0x1 << 3)
        /* EN_PD_D_EXT   (2'h3) */ | (0x3 << 1)
        /* EN_PD_C_EXT   (1'h1) */ | (0x1 << 0)
    );

    mbus_remote_register_write(MRM_ADDR, 0x29, 0x0
        /* BIST_ENABLE   (1'h0) */ | (0x1 << 0)
    );
}

void mrm_disable_bist(void) {
    mbus_remote_register_write(MRM_ADDR, 0x29, 0x0
        /* BIST_ENABLE   (1'h0) */ | (0x0 << 0)
    );

    mbus_remote_register_write(MRM_ADDR, 0x28, 0x0
        /* EN_OUT_TDO    (1'h0) */ | (0x0 << 7)
        /* EN_PD_TDI     (1'h1) */ | (0x1 << 6)
        /* EN_PD_TMS     (1'h1) */ | (0x1 << 5)
        /* EN_PD_TCK     (1'h1) */ | (0x1 << 4)
        /* EN_PD_BOOT_EN (1'h1) */ | (0x1 << 3)
        /* EN_PD_D_EXT   (2'h3) */ | (0x3 << 1)
        /* EN_PD_C_EXT   (1'h1) */ | (0x1 << 0)
    );

    mbus_remote_register_write(MRM_ADDR, 0x2C, 0x0
        /* TMC_SCAN_RST_N (1'h0) */ | (0x0 << 1)
        /* TMC_SCAN_TEST  (1'h0) */ | (0x0 << 0)
    );
}

void mrm_set_clock_mode(uint32_t clock_mode) {

    set_halt_until_mbus_trx();
    mbus_remote_register_read(MRM_ADDR, 0x2F, __mrm_irq_reg_idx__);
    set_halt_until_mbus_tx();

    uint32_t temp_reg = *__mrm_irq_reg_addr__ & 0xFFFFEF;   // Reset TMC_CLOCK_MODE to 0.
    temp_reg |= ((clock_mode&0x1) << 4); // Sets TMC_CLOCK_MODE = clock_mode
    mbus_remote_register_write(MRM_ADDR, 0x2F, temp_reg);
}

void mrm_set_clock_tune(uint32_t s) {
    mbus_remote_register_write(MRM_ADDR, 0x26, /*CLK_GEN_S*/s);
}

void mrm_tune_ldo (uint32_t i0p8, uint32_t i1p8, uint32_t v0p8, uint32_t v1p8) {

    set_halt_until_mbus_trx();
    mbus_remote_register_read(MRM_ADDR, 0x23, __mrm_irq_reg_idx__);
    set_halt_until_mbus_tx();

    uint32_t temp_reg = *__mrm_irq_reg_addr__ & 0x00003F;
    temp_reg |= (i0p8<<19)|(i1p8<<14)|(v1p8<<10)|(v0p8<<6);
    mbus_remote_register_write(MRM_ADDR, 0x23, temp_reg);
}

uint32_t mrm_turn_on_ldo (void) {

    enable_reg_irq(/*reg*/__mrm_irq_reg_idx__);

    mbus_remote_register_write(MRM_ADDR, 0x21, 0x0
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

    mbus_remote_register_write(MRM_ADDR, 0x21, 0x0
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
    mbus_remote_register_write(MRM_ADDR, 0x08, /*MRAM_START_ADDR*/mid << 19);

    enable_reg_irq(/*reg*/__mrm_irq_reg_idx__);

    mbus_remote_register_write(MRM_ADDR, 0x11, 0x0
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

    mbus_remote_register_write(MRM_ADDR, 0x11, 0x0
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
    mbus_remote_register_read(MRM_ADDR, 0x03, __mrm_irq_reg_idx__);
    set_halt_until_mbus_tx();

    uint32_t temp_reg = *__mrm_irq_reg_addr__ & 0x0000FF;   // Reset Tpwr
    temp_reg |= (tpwr << 8);
    mbus_remote_register_write(MRM_ADDR, 0x03, temp_reg);
}

uint32_t mrm_cmd_go (uint32_t cmd, uint32_t len_1, uint32_t expected) {

    enable_reg_irq(/*reg*/__mrm_irq_reg_idx__);

    mbus_remote_register_write(MRM_ADDR, 0x09, 0x0
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

void mrm_cmd_go_irq_nowait (uint32_t cmd, uint32_t len_1) {

    mbus_remote_register_write(MRM_ADDR, 0x09, 0x0
        /* LENGTH (14'h0000) */ | (len_1 << 6)    
        /* IRQ_EN (    1'h0) */ | (0x1 << 5)    
        /* CMD    (    4'h0) */ | (cmd << 1)    
        /* GO     (    1'h0) */ | (0x1 << 0)    
    );

}

void mrm_cmd_go_noirq (uint32_t cmd, uint32_t len_1) {

    mbus_remote_register_write(MRM_ADDR, 0x09, 0x0
        /* LENGTH (14'h0000) */ | (len_1 << 6)    
        /* IRQ_EN (    1'h0) */ | (0x0 << 5)    
        /* CMD    (    4'h0) */ | (cmd << 1)    
        /* GO     (    1'h0) */ | (0x1 << 0)    
    );

}

void mrm_set_sram_addr (uint32_t addr) {
    // Set SRAM Start Address
    mbus_remote_register_write(MRM_ADDR, 0x07, addr);
}

void mrm_set_mram_addr (uint32_t addr) {
    // Set MRAM Start Address
    mbus_remote_register_write(MRM_ADDR, 0x08, addr);
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
        /*source_prefix  */ MRM_ADDR,
        /*source_mem_addr*/ mrm_sram_addr,
        /*dest_prefix    */ 0x1,
        /*dest_mem_addr  */ prc_sram_addr,
        /*length_minus_1 */ num_words - 1
        );
    set_halt_until_mbus_tx();
}

void mrm_read_sram_debug (uint32_t* mrm_sram_addr, uint32_t num_words, uint32_t dest_prefix) {
    mbus_copy_mem_from_remote_to_any_stream(
        /*stream_channel */ 0,
        /*source_prefix  */ MRM_ADDR,
        /*source_mem_addr*/ mrm_sram_addr,
        /*dest_prefix    */ dest_prefix,
        /*length_minus_1 */ num_words - 1
        );
}

void mrm_write_sram (uint32_t* prc_sram_addr, uint32_t num_words, uint32_t* mrm_sram_addr) {
    mbus_copy_mem_from_local_to_remote_bulk(
        /*remote_prefix  */ MRM_ADDR,
        /*remote_mem_addr*/ mrm_sram_addr,
        /*local_mem_addr */ prc_sram_addr,
        /*length_minus_1 */ num_words - 1
        );
}

void mrm_read_sram_page (uint32_t mrm_sram_pid, uint32_t num_pages, uint32_t* prc_sram_addr) {
    set_halt_until_mbus_trx();
    mbus_copy_mem_from_remote_to_any_bulk(
        /*source_prefix  */ MRM_ADDR,
        /*source_mem_addr*/ (uint32_t*) ((mrm_sram_pid<<5)<<2),
        /*dest_prefix    */ 0x1,
        /*dest_mem_addr  */ prc_sram_addr,
        /*length_minus_1 */ (num_pages<<5) - 1
        );
    set_halt_until_mbus_tx();
}

void mrm_read_sram_page_debug (uint32_t mrm_sram_pid, uint32_t num_pages, uint32_t dest_prefix) {
    mbus_copy_mem_from_remote_to_any_stream(
        /*stream_channel */ 0,
        /*source_prefix  */ MRM_ADDR,
        /*source_mem_addr*/ (uint32_t*) ((mrm_sram_pid<<5)<<2),
        /*dest_prefix    */ dest_prefix,
        /*length_minus_1 */ (num_pages<<5) - 1
        );
}

void mrm_write_sram_page (uint32_t* prc_sram_addr, uint32_t num_pages, uint32_t mrm_sram_pid) {
    mbus_copy_mem_from_local_to_remote_bulk(
        /*remote_prefix  */ MRM_ADDR,
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

uint32_t mrm_ext_stream (uint32_t bit_en, uint32_t num_pages, uint32_t mrm_sram_pid) {

    // --- Disable the Ping-Pong Streaming
    mbus_remote_register_write(MRM_ADDR, 0x13, 0x0
        /* PP_STR_LIMIT (21'h000000) */ | (0 << 1) // Limit the number of words streamed. 0 means no-limit; Must be an integer multiple of NUM_WORDS_PER_PAGE.
        /* PP_STR_EN    ( 1'h0     ) */ | (0 << 0) // Ping-Pong Straming Enable
    );

    // --- External Streaming Configuration
    mbus_remote_register_write(MRM_ADDR, 0x0C, 0x0
        /* WRAP_EXT        (1'h0) */ | (0 << 3) 
        /* UPDATE_ADDR_EXT (1'h0) */ | (0 << 2) 
        /* BIT_EN_EXT      (2'h1) */ | (bit_en << 0) 
    );

    // --- Set SRAM Address
    mrm_set_sram_addr (/*addr*/mrm_sram_pid<<MRM_LOG2_NUM_WORDS_PER_PAGE);

    // --- External Streaming Time-out
    mbus_remote_register_write(MRM_ADDR, 0x0D, 0x0
        /* TIMEOUT_EXT (24'hFFFFFF) */ | (0 << 0) // #0 means no time-limit
    );

    // --- Go into the ping-pong listening mode
    mrm_cmd_go (/*cmd*/MRM_CMD_EXT_WR_SRAM, /*len_1*/(num_pages<<MRM_LOG2_NUM_WORDS_PER_PAGE)-1, /*expected*/MRM_EXP_NONE);

    // --- Return the IRQ payload
    return *__mrm_irq_reg_addr__&0xFF;
}

uint32_t mrm_pp_ext_stream (uint32_t bit_en, uint32_t num_pages, uint32_t mram_page_id, uint32_t wait_for_irq) {

    // --- Ping-Pong General Configuration
    mbus_remote_register_write(MRM_ADDR, 0x14, 0x0
        /* PP_EARLY_POWER_ON   (1'h1) */  | (0x1 << 5)    // If 1, it powers on the next mram macro if needed, while it is still in PP_WAIT_FOR_STREAM state.
        /* PP_NO_ERR_DETECTION (1'h0) */  | (0x0 << 4)    // If 1, it does not detect buffer_overrun error. Other errors can be stil detected, though.
        /* RESERVED            (1'h0) */  | (0x0 << 3)    // 
        /* PP_WRAP             (1'h0) */  | (0x0 << 2)    // If 1, PP_MRAM_ADDR will wrap around
        /* PP_BIT_EN_EXT       (2'h1) */  | (bit_en << 0) // 
    );

    // --- Set the streaming length
    mbus_remote_register_write(MRM_ADDR, 0x13, 0x0
        /* PP_STR_LIMIT (21'h000000) */ | ((num_pages<<5) << 1) // Limit the number of words streamed. 0 means no-limit; Must be an integer multiple of NUM_WORDS_PER_PAGE.
        /* PP_STR_EN    ( 1'h0     ) */ | (0x1            << 0) // Ping-Pong Straming Enable
    );

    // --- Set MRAM Address
    mbus_remote_register_write(MRM_ADDR, 0x15, 0x0
        /* PP_FLAG_END_OF_MRAM ( 1'h0    ) */ | (0x0               << 23)
        /* RESERVED            (3'h0)      */ | (0x0               << 20)
        /* PP_MRAM_ADDR        (20'h00000) */ | ((mram_page_id<<5) <<  0)
    );

    // --- Go into the ping-pong listening mode
    if (wait_for_irq) {
        mrm_cmd_go (/*cmd*/MRM_CMD_EXT_WR_SRAM, /*len_1*/0, /*expected*/MRM_EXP_NONE);
        return *__mrm_irq_reg_addr__&0xFF;
    }
    else {
        mrm_cmd_go_irq_nowait (/*cmd*/MRM_CMD_EXT_WR_SRAM, /*len_1*/0);
        return 1;
    }

}

void mrm_pp_ext_stream_unlim (uint32_t bit_en, uint32_t mram_page_id) {

    // --- Ping-Pong General Configuration
    mbus_remote_register_write(MRM_ADDR, 0x14, 0x0
        /* PP_EARLY_POWER_ON   (1'h1) */  | (0x1 << 5)    // If 1, it powers on the next mram macro if needed, while it is still in PP_WAIT_FOR_STREAM state.
        /* PP_NO_ERR_DETECTION (1'h0) */  | (0x0 << 4)    // If 1, it does not detect buffer_overrun error. Other errors can be stil detected, though.
        /* RESERVED            (1'h0) */  | (0x0 << 3)    // 
        /* PP_WRAP             (1'h0) */  | (0x0 << 2)    // If 1, PP_MRAM_ADDR will wrap around
        /* PP_BIT_EN_EXT       (2'h1) */  | (bit_en << 0) // 
    );

    // --- Set the streaming length
    mbus_remote_register_write(MRM_ADDR, 0x13, 0x0
        /* PP_STR_LIMIT (21'h000000) */ | (0x0 << 1) // Limit the number of words streamed. 0 means no-limit; Must be an integer multiple of NUM_WORDS_PER_PAGE.
        /* PP_STR_EN    ( 1'h0     ) */ | (0x1 << 0) // Ping-Pong Straming Enable
    );

    // --- Set MRAM Address
    mbus_remote_register_write(MRM_ADDR, 0x15, 0x0
        /* PP_FLAG_END_OF_MRAM ( 1'h0    ) */ | (0x0               << 23)
        /* RESERVED            (3'h0)      */ | (0x0               << 20)
        /* PP_MRAM_ADDR        (20'h00000) */ | ((mram_page_id<<5) <<  0)
    );

    // --- Go into the ping-pong listening mode
    mbus_remote_register_write(MRM_ADDR, 0x09, 0x0
        /* LENGTH (14'h0000) */ | (0x0 << 6)    
        /* IRQ_EN (    1'h0) */ | (0x1 << 5)    
        /* CMD    (    4'h0) */ | (MRM_CMD_EXT_WR_SRAM << 1)    
        /* GO     (    1'h0) */ | (0x1 << 0)    
    );

}

uint32_t mrm_stop_pp_ext_stream (void) {

    enable_reg_irq(/*reg*/__mrm_irq_reg_idx__);

    mbus_remote_register_write(MRM_ADDR, 0x13, 0x0
        /* PP_STR_LIMIT (21'h000000) */ | (0x0 << 1) // Limit the number of words streamed. 0 means no-limit; Must be an integer multiple of NUM_WORDS_PER_PAGE.
        /* PP_STR_EN    ( 1'h0     ) */ | (0x0 << 0) // Ping-Pong Straming Enable
    );

    WFI();

    // --- Return the IRQ payload
    return *__mrm_irq_reg_addr__&0xFF;
}

// Currently supporting DIN[31:0] only.
uint32_t mrm_tmc_cmd (uint32_t cmd, uint32_t xadr, uint32_t yadr, uint32_t din, uint32_t* result) {

    mbus_remote_register_write(MRM_ADDR, 0x07, 0x0
        /* SRAM_START_ADDR (14'h0) */ | (0x0 << 0) 
    );

    mbus_remote_register_write(MRM_ADDR, 0x08, 0x0
        /* XADR (15'h0) */ | (xadr << 7) 
        /* YADR ( 5'h0) */ | (yadr << 2) 
    );

    mbus_remote_register_write(MRM_ADDR, 0x2B, 0x0
        /* TMC_R_CFG_2CPB         (1'h0 ) */ | (0x0 << 11)      
        /* TMC_R_CFG_VBL_EXT_VMAX (1'h0 ) */ | (0x0 << 10)      
        /* TMC_R_SRAM_DMA         (1'h0 ) */ | (0x0 <<  9)      
        /* TMC_R_IFREN1           (1'h0 ) */ | (0x0 <<  8)      
        /* TMC_R_INFO             (1'h0 ) */ | (0x0 <<  7)      
        /* TMC_R_SLP              (1'h0 ) */ | (0x0 <<  6)      
        /* TMC_R_LPR              (1'h0 ) */ | (0x0 <<  5)      
        /* TMC_R_CMD              (5'h00) */ | (cmd <<  0)   
    );

    mbus_copy_mem_from_local_to_remote_bulk(
        /*remote_prefix  */ MRM_ADDR,
        /*remote_mem_addr*/ 0x0,
        /*local_mem_addr */ &din,
        /*length_minus_1 */ 0
        );

    mrm_cmd_go (/*cmd*/MRM_CMD_TMC_CMD, /*len_1*/0, /*expected*/MRM_EXP_NONE);

    if (cmd == MRM_TMC_READ_CONFIG) {
        mrm_read_sram (/*mrm_sram_addr*/(uint32_t *)0x14, /*num_words*/15, /*prc_sram_addr*/result);
    }

    // --- Return the IRQ payload
    return *__mrm_irq_reg_addr__&0xFF;
}

void mrm_tmc_write_test_reg (uint32_t xadr, uint32_t wdata) {

    // R_TEST = 16'h0001
    mrm_tmc_cmd (/*cmd*/MRM_TMC_WRITE_CONFIG, /*xadr*/0x0, /*yadr*/0x0, /*din*/0x0001, /*result*/(uint32_t*)0x0);
    
    // Write Register
    mrm_tmc_cmd (/*cmd*/MRM_TMC_WRITE_CONFIG, /*xadr*/xadr, /*yadr*/0x0, /*din*/wdata, /*result*/(uint32_t*)0x0);
    
    // R_TEST = 16'h0000
    mrm_tmc_cmd (/*cmd*/MRM_TMC_WRITE_CONFIG, /*xadr*/0x0, /*yadr*/0x0, /*din*/0x0000, /*result*/(uint32_t*)0x0);

}
