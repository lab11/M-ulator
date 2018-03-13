//*******************************************************************
//Author: Yejoong Kim
//Description: FLPv3S 6-Socket Testing
//*******************************************************************
#include "PRCv17.h"
#include "FLPv3S.h"
#include "FLPv3S_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define FLP_ADDR    0x4
#define PMU_ADDR    0xE

// Flag Identifiers
#define FLAG_ENUM   0

// Flash Default Timing Setting
#define DEFAULT_TERASE		0x100
#define DEFAULT_TPROG		0x9
#define DEFAULT_TCYCPROG	0x7F

#define DEFAULT_TCAP		0x7CF
#define DEFAULT_TVREF		0x1F3F

//********************************************************************
// Global Variables
//********************************************************************
#define _SIZE_TEMP_ 64
#define _LOG2_TEMP_ 6   // = log2(_SIZE_TEMP_)
#define _ITER_TEMP_ 32  // = 2048 / _SIZE_TEMP_

#define _MAX_NUM_BIT_ERROR_ 1000 // Max No. Bit Errors to be reported

volatile uint32_t temp_storage[_SIZE_TEMP_];
volatile uint32_t result[8];


//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer16  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbustx   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusrx   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusfwd  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg4     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg5     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg6     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusmem  (void) __attribute__ ((interrupt ("IRQ")));

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32); }
void handler_ext_int_timer16(void) { // TIMER16
    *NVIC_ICPR = (0x1 << IRQ_TIMER16); }
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0); }
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1); }
void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2); }
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3); }
void handler_ext_int_reg4(void) { // REG4
    *NVIC_ICPR = (0x1 << IRQ_REG4); }
void handler_ext_int_reg5(void) { // REG5
    *NVIC_ICPR = (0x1 << IRQ_REG5); }
void handler_ext_int_reg6(void) { // REG6
    *NVIC_ICPR = (0x1 << IRQ_REG6); }
void handler_ext_int_reg7(void) { // REG7
    *NVIC_ICPR = (0x1 << IRQ_REG7); }
void handler_ext_int_mbusmem(void) { // MBUS_MEM_WR
    *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM); }
void handler_ext_int_mbusrx(void) { // MBUS_RX
    *NVIC_ICPR = (0x1 << IRQ_MBUS_RX); }
void handler_ext_int_mbustx(void) { // MBUS_TX
    *NVIC_ICPR = (0x1 << IRQ_MBUS_TX); }
void handler_ext_int_mbusfwd(void) { // MBUS_FWD
    *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD); }
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP); }
void handler_ext_int_softreset(void) { // SOFT_RESET
    *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET); }
void handler_ext_int_wakeup(void) { // WAKE-UP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); }

//*******************************************************************
// FLPv3S Functions
//*******************************************************************
void flpv3s_set_irq_addr(uint32_t short_addr, uint32_t reg_addr);
void flpv3s_set_timing();
void flpv3s_set_vclamp(uint32_t i_1stg, uint32_t vout);
void flpv3s_sram_start_addr(uint32_t addr);
void flpv3s_flsh_start_addr(uint32_t addr);
void flpv3s_go(uint32_t length, uint32_t cmd);
void flpv3s_manual_on();
void flpv3s_manual_off();
void flpv3s_sram_write_all(uint32_t data_type);
void flpv3s_prog_page(uint32_t num_pages);
void flpv3s_read_page(uint32_t num_pages);
void flpv3s_erase_page();
void flpv3s_test_prog_all(uint32_t data_type);
void flpv3s_test_erase_all();
void flpv3s_check_prog_all(uint32_t data_type);
void flpv3s_check_erase_all();
void flpv3s_check_sram(uint32_t expected);
void flpv3s_reset_result();
void flpv3s_show_result();
void flpv3s_set_tune();

void flpv3s_set_irq_addr(uint32_t short_addr, uint32_t reg_addr) {
    mbus_write_message32(((FLP_ADDR << 4) | 0x0), ((0xF << 24) | (short_addr << 12) | (reg_addr << 0)));
}

void flpv3s_set_timing() {
    // Populate REG0 ~ REG6 as below:
    // REG0: (Tcyc_read << 19) | (T3us << 13) | (T5us << 7) | (T10us << 0)
    // REG1: (Tcyc_prog << 8) | (Tprog << 0)
    // REG2: Terase
    // REG3: (Thvcp_en << 10) | (Tben << 0)
    // REG4: (Tmvcp_en << 12) | (Tsc_en << 0)
    // REG5: Tcap
    // REG6: Tvref
    //
    // Erase: Terase is important!! Higher number -> robust erase, but high peak current; Lower number -> poor erase, but lower peak current.
    // Program: Tcyc_prog, Tprog
    mbus_copy_registers_from_local_to_remote(FLP_ADDR, 0x0, 0x0, 7);
}

void flpv3s_set_vclamp(uint32_t i_1stg, uint32_t vout) {
    mbus_write_message32(((FLP_ADDR << 4) | 0x0), ((0x19 << 24) | (0x1 << 11) | (i_1stg << 7) | (0x0 << 3) | (vout << 0)));
}

void flpv3s_set_tune() {
    // REG0: I_1STG
    // REG1: VOUT
    // REG2: Tcyc_prog
    // REG3: Tprog
    // REG4: Terase
    // REG5: Tcap
    // REG6: Tvref
    mbus_write_message32(((FLP_ADDR << 4) | 0x0), ((0x19 << 24) | (0x1 << 11) | (*REG0 << 7) | (0x0 << 3) | (*REG1 << 0)));
    mbus_write_message32(((FLP_ADDR << 4) | 0x0), ((0x01 << 24) | (*REG2 << 8) | (*REG3 << 0)));
    mbus_write_message32(((FLP_ADDR << 4) | 0x0), ((0x02 << 24) | (*REG4 << 0)));
    mbus_write_message32(((FLP_ADDR << 4) | 0x0), ((0x05 << 24) | (*REG5 << 0)));
    mbus_write_message32(((FLP_ADDR << 4) | 0x0), ((0x06 << 24) | (*REG6 << 0)));
}

void flpv3s_sram_start_addr(uint32_t addr) {
    mbus_write_message32(((FLP_ADDR << 4) | 0x0), ((0x7 << 24) | addr));
}

void flpv3s_flsh_start_addr(uint32_t addr) {
    mbus_write_message32(((FLP_ADDR << 4) | 0x0), ((0x8 << 24) | addr));
}

void flpv3s_go(uint32_t length, uint32_t cmd) {
    set_halt_until_mbus_trx();
    mbus_write_message32(((FLP_ADDR << 4) | 0x0), ((0x9 << 24) | (length << 6) | (0x1 << 5) | (cmd << 1) | 0x1 ));
    set_halt_until_mbus_tx();
}

void flpv3s_manual_on() {
    set_halt_until_mbus_trx();
    mbus_write_message32(((FLP_ADDR << 4) | 0x0), ((0x11 << 24) | 0x2F));
    set_halt_until_mbus_tx();
}

void flpv3s_manual_off() {
    set_halt_until_mbus_trx();
    mbus_write_message32(((FLP_ADDR << 4) | 0x0), ((0x11 << 24) | 0x2D));
    set_halt_until_mbus_tx();
}

void flpv3s_sram_write_all(uint32_t data_type) {
    uint32_t pattern;
    uint32_t idx_i;

    // Choose Data Pattern
    if      (data_type == 0) pattern = 0x00000000;
    else if (data_type == 1) pattern = 0xFFFFFFFF;
    else if (data_type == 2) pattern = 0xAAAAAAAA;
    else if (data_type == 3) pattern = 0x55555555;

    // Initialize temp_storage
    for (idx_i=0; idx_i<_SIZE_TEMP_; idx_i++) temp_storage[idx_i] = pattern;

    // Write into SRAM
    for (idx_i=0; idx_i<_ITER_TEMP_; idx_i++) {
        uint32_t dest_mem_addr = (idx_i << (_LOG2_TEMP_ + 2));
        mbus_copy_mem_from_local_to_remote_bulk(FLP_ADDR, (uint32_t *) dest_mem_addr, temp_storage, (_SIZE_TEMP_ - 1));
    }

}

void flpv3s_prog_page(uint32_t num_pages) {
    uint32_t length = (num_pages << 8) - 1;
    flpv3s_go(length, FLPV3_CMD_PROG);
}

void flpv3s_read_page(uint32_t num_pages) {
    uint32_t length = (num_pages << 8) - 1;
    flpv3s_go(length, FLPV3_CMD_READ);
}

void flpv3s_erase_page() {
    flpv3s_go(0, FLPV3_CMD_ERASE);
}

void flpv3s_test_prog_all(uint32_t data_type) {
    uint32_t page_id;
    uint32_t expected;

    // Reset result
    flpv3s_reset_result();

    // Choose Data Pattern
    if      (data_type == 0) expected = 0x00000000;
    else if (data_type == 1) expected = 0xFFFFFFFF;
    else if (data_type == 2) expected = 0xAAAAAAAA;
    else if (data_type == 3) expected = 0x55555555;

    // Initialize SRAM with the desired data
    if      (data_type == 0) flpv3s_sram_write_all(0);
    else if (data_type == 1) flpv3s_sram_write_all(1);
    else if (data_type == 2) flpv3s_sram_write_all(2);
    else if (data_type == 3) flpv3s_sram_write_all(3);

    // Set SRAM_START_ADDR=0
    flpv3s_sram_start_addr(0x0);

    // Turn on Flash
    flpv3s_manual_on();

    // Program the entire Flash
    for (page_id=0; page_id<128; page_id=page_id+8) {
        flpv3s_flsh_start_addr(page_id<<8);
        flpv3s_prog_page(8);
    }

    // Negate the SRAM data
    if      (data_type == 0) flpv3s_sram_write_all(1);
    else if (data_type == 1) flpv3s_sram_write_all(0);
    else if (data_type == 2) flpv3s_sram_write_all(3);
    else if (data_type == 3) flpv3s_sram_write_all(2);

    // Read the Flash and Check the Data
    flpv3s_check_sram(expected);

    // Turn off Flash
    flpv3s_manual_off();
}

void flpv3s_test_erase_all() {
    uint32_t page_id;
    uint32_t expected = 0xFFFFFFFF;

    // Reset result
    flpv3s_reset_result();

    // Set SRAM_START_ADDR=0
    flpv3s_sram_start_addr(0x0);

    // Reset the SRAM data to All0
    flpv3s_sram_write_all(0);

    // Turn on Flash
    flpv3s_manual_on();

    // Erase the entire Flash
    for (page_id=0; page_id<128; page_id++) {
        flpv3s_flsh_start_addr(page_id<<8);
        flpv3s_erase_page();
    }

    // Read the Flash and Check the Data
    flpv3s_check_sram(expected);

    // Turn off Flash
    flpv3s_manual_off();
}

void flpv3s_check_prog_all(uint32_t data_type) {
    uint32_t expected;

    // Choose Data Pattern
    if      (data_type == 0) expected = 0x00000000;
    else if (data_type == 1) expected = 0xFFFFFFFF;
    else if (data_type == 2) expected = 0xAAAAAAAA;
    else if (data_type == 3) expected = 0x55555555;

    // Negate the SRAM data
    if      (data_type == 0) flpv3s_sram_write_all(1);
    else if (data_type == 1) flpv3s_sram_write_all(0);
    else if (data_type == 2) flpv3s_sram_write_all(3);
    else if (data_type == 3) flpv3s_sram_write_all(2);

    // Turn on Flash
    flpv3s_manual_on();

    // Read the Flash and Check the Data
    flpv3s_check_sram(expected);

    // Turn off Flash
    flpv3s_manual_off();
}

void flpv3s_check_erase_all() {
    uint32_t expected = 0xFFFFFFFF;

    // Reset the SRAM data to All0
    flpv3s_sram_write_all(0);

    // Turn on Flash
    flpv3s_manual_on();

    // Read the Flash and Check the Data
    flpv3s_check_sram(expected);

    // Turn off Flash
    flpv3s_manual_off();
}

void flpv3s_check_sram(uint32_t expected) {
    uint32_t page_id;
    uint32_t iter;
    uint32_t idx_i;
    uint32_t idx_j;
    uint32_t temp_0;
    uint32_t temp_1;
    uint32_t temp_2;
    uint32_t error = 0;
    uint32_t num_word_error = 0;
    uint32_t num_bit_error = 0;

    // Reset result
    flpv3s_reset_result();

    // Set SRAM_START_ADDR=0
    flpv3s_sram_start_addr(0x0);

    // Read the Flash and Check the Data
    for (page_id=0; page_id<128; page_id=page_id+8) {
        flpv3s_flsh_start_addr(page_id<<8);
        flpv3s_read_page(8);

        for (iter=0; iter<_ITER_TEMP_; iter++) {
            uint32_t source_mem_addr = iter << (_LOG2_TEMP_ + 2);
            set_halt_until_mbus_trx();
            mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR, (uint32_t *) source_mem_addr, PRC_ADDR, temp_storage, (_SIZE_TEMP_)-1);
            set_halt_until_mbus_tx();

            for(idx_i=0; idx_i<_SIZE_TEMP_; idx_i++) {
                // If there is an error, display the info
                if (temp_storage[idx_i]!=expected) {
                    if (num_bit_error < _MAX_NUM_BIT_ERROR_) {
                        error = 1;
                        num_word_error++;
                        temp_2 = 0;
                        for (idx_j=0; idx_j<32; idx_j++) {
                            temp_0 = expected << (31 - idx_j);
                            temp_0 = temp_0 >> 31;
                            temp_1 = temp_storage[idx_i] << (31 - idx_j);
                            temp_1 = temp_1 >> 31;
                            if (temp_0 != temp_1) temp_2++;
                        }
                        num_bit_error = num_bit_error + temp_2;
                        result[0] = 0xDEADBEEF;
                        result[1] = expected;
                        result[2] = temp_storage[idx_i];
                        result[3] = idx_i;
                        result[4] = source_mem_addr;
                        result[5] = page_id;
                        result[6] = num_word_error;
                        result[7] = num_bit_error;
                    }
                }
            }

        }
    }

    // Display Result
    if (!error) result[0] = 0x77777777;

    flpv3s_show_result();
}

void flpv3s_reset_result() {
    uint32_t idx_i;
    for(idx_i=0; idx_i<8; idx_i++) result[idx_i] = 0;
}

void flpv3s_show_result() {
    uint32_t idx_i;
    for(idx_i=0; idx_i<8; idx_i++) mbus_write_message32((0xC0 | idx_i), result[idx_i]);
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    set_flag(FLAG_ENUM, 1);

    // Set Halt
    set_halt_until_mbus_trx();

    // Enumeration
    mbus_enumerate(FLP_ADDR);
    mbus_enumerate(PMU_ADDR);

    //Set Halt
    set_halt_until_mbus_tx();

    flpv3s_set_irq_addr(PRC_ADDR, 0x07);
    flpv3s_reset_result();
}



//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    // Disable Watchdog
    disable_timerwd();

    // Disable MBus Watchdog
    *MBCWD_RESET = 1;

    // Initialization Sequence
    if (!get_flag(FLAG_ENUM)) { 
        initialization();
    }

    uint32_t reg7_header = *REG7 >> 16;
    uint32_t reg7_data   = *REG7 & 0xFFFF;
    *REG7 = 0x0;

    mbus_write_message32(0xD0, reg7_header);
    mbus_write_message32(0xD1, reg7_data);

    if (reg7_header == 0xAA) {
        if      (reg7_data == 0x0000) flpv3s_show_result();
        else if (reg7_data == 0x0001) flpv3s_set_vclamp(/*I_1STG*/ *REG0, /*VOUT*/ *REG1);
        else if (reg7_data == 0x0002) flpv3s_set_timing();
        else if (reg7_data == 0x0003) flpv3s_manual_on();
        else if (reg7_data == 0x0004) flpv3s_manual_off();
        else if (reg7_data == 0x0005) flpv3s_test_prog_all(0);   // All 0
        else if (reg7_data == 0x0006) flpv3s_test_prog_all(1);   // All 1
        else if (reg7_data == 0x0007) flpv3s_test_prog_all(2);   // All A
        else if (reg7_data == 0x0008) flpv3s_test_prog_all(3);   // All 5
        else if (reg7_data == 0x0009) flpv3s_test_erase_all();   // Erase
        else if (reg7_data == 0x1005) flpv3s_check_prog_all(0);   // All 0
        else if (reg7_data == 0x1006) flpv3s_check_prog_all(1);   // All 1
        else if (reg7_data == 0x1007) flpv3s_check_prog_all(2);   // All A
        else if (reg7_data == 0x1008) flpv3s_check_prog_all(3);   // All 5
        else if (reg7_data == 0x1009) flpv3s_check_erase_all();   // Erase
        else if (reg7_data == 0x2000) flpv3s_set_tune();
    }

    // Stay here...
    while(1);

    return 1;
}

