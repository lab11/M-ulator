//*******************************************************************
//Author: Yejoong Kim
//Description: PREv23M lib file
//-------------------------------------------------------------------
//  <Update History>
//  Jul 13 2020 - PRCv21 Family
//                  - Updated set_wakeup_timer according to the new register bitmap.
//                      Old: uint32_t regval = timestamp;
//                           if( irq_en ) regval |= 0xC00000;
//                           else         regval &= 0x3FFFFF;
//                      New: uint32_t regval = timestamp | 0x800000;
//                           if( irq_en ) regval |= 0xC00000;
//                           else         regval &= 0xBFFFFF;
//                  - Removed *gpio* functions and 
//                      made the previous *ngpio* functions new *gpio* functions
//  Apr 28 2021 - PRCv22 Family
//  Jul 06 2022 - PRCv23 Family
//                  - Updated set_xo_timer() to support the new XO timer threshold access.
//                      Now it writes in *XOT_SAT to change the XO timer threshold.
//  Nov 03 2022 - Added support for PREv23M
//*******************************************************************

#include "PREv23M.h"
#include "mbus.h"

//*******************************************************************
// OTHER FUNCTIONS
//*******************************************************************

void delay(unsigned ticks){
  unsigned i;
  for (i=0; i < ticks; i++)
    asm("nop;");
}

void WFI(){
  asm("wfi;");
}

void config_timer16(uint32_t cmp0, uint32_t cmp1, uint8_t irq_en, uint32_t cnt, uint32_t status){
	*TIMER16_GO    = 0x0;
	*TIMER16_CMP0  = cmp0;
	*TIMER16_CMP1  = cmp1;
	*TIMER16_IRQEN = irq_en;
	*TIMER16_CNT   = cnt;
	*TIMER16_STAT  = status;
	*TIMER16_GO    = 0x1;
}

void config_timer32(uint32_t cmp, uint8_t roi, uint32_t cnt, uint32_t status){
	*TIMER32_GO   = 0x0;
	*TIMER32_CMP  = cmp;
	*TIMER32_ROI  = roi;
	*TIMER32_CNT  = cnt;
	*TIMER32_STAT = status;
	*TIMER32_GO   = 0x1;
}

void config_timerwd(uint32_t cnt){
	*TIMERWD_GO  = 0x0;
	*TIMERWD_CNT = cnt;
	*TIMERWD_GO  = 0x1;
}

void disable_timerwd(){
	*TIMERWD_GO  = 0x0;
}

void set_wakeup_timer( uint32_t timestamp, uint8_t irq_en, uint8_t reset ){
    uint32_t regval = timestamp | 0x800000; // WUP Timer Enable
    if( irq_en ) regval |= 0xC00000;
    else         regval &= 0xBFFFFF;
    *REG_WUPT_CONFIG = regval;

	if( reset ) *WUPT_RESET = 0x01;
}

void enable_xo_timer () {
    uint32_t regval = *REG_XOT_CONFIG;
    regval |= 0x800000; // XOT_ENABLE = 1;
    *REG_XOT_CONFIG = regval;
}

void disable_xo_timer () {
    uint32_t regval = *REG_XOT_CONFIG;
    regval &= 0x7FFFFF; // XOT_ENABLE = 0;
    *REG_XOT_CONFIG = regval;
}

void set_xo_timer (uint8_t mode, uint32_t timestamp, uint8_t wreq_en, uint8_t irq_en) {

    uint32_t regval0 = 0x00800003; // XOT_ENABLE = 1; SEL_PAD_XO_CLK=3
    if (mode)    regval0 |= 0x00400000; // XOT_MODE = 1
    if (wreq_en) regval0 |= 0x00200000; // XOT_WREQ_EN = 1
    if (irq_en)  regval0 |= 0x00100000; // XOT_IRQ_EN = 1

    *REG_XOT_CONFIG  = regval0;
    *XOT_SAT = timestamp;
}

void reset_xo_cnt  () { *XOT_RESET_CNT  = 0x1; }
void start_xo_cnt  () { *XOT_START_CNT  = 0x1; }
void stop_xo_cnt   () { *XOT_STOP_CNT   = 0x1; }
void start_xo_cout () { *XOT_START_COUT = 0x1; }
void stop_xo_cout  () { *XOT_STOP_COUT  = 0x1; }

//**************************************************
// M0 IRQ SETTING
//**************************************************
void enable_all_irq() { *NVIC_ICPR = 0xFFFFFFFF; *NVIC_ISER = 0xFFFFFFFF; }
void disable_all_irq() { *NVIC_ICPR = 0xFFFFFFFF; *NVIC_ICER = 0xFFFFFFFF; }
void clear_all_pend_irq() { *NVIC_ICPR = 0xFFFFFFFF; }

//**************************************************
// PRC/PRE FLAGS Register
//**************************************************
uint32_t set_flag ( uint32_t bit_idx, uint32_t value ) {
    uint32_t reg_val = (*REG_FLAGS & (~(0x1 << bit_idx))) | (value << bit_idx);
    *REG_FLAGS = reg_val;
    return reg_val;
}
    
uint8_t get_flag ( uint32_t bit_idx ) {
    uint8_t reg_val = (*REG_FLAGS & (0x1 << bit_idx)) >> bit_idx;
    return reg_val;
}
    
uint32_t set_mb_flag ( uint32_t lsb_idx, uint32_t num_bits, uint32_t value ) {
    uint32_t pattern = (((0xFFFFFFFF << (32 - (lsb_idx + num_bits))) >> (32 - (lsb_idx + num_bits))) >> lsb_idx) << lsb_idx;
    uint32_t reg_val = (*REG_FLAGS & (~pattern)) | (value << lsb_idx);
    *REG_FLAGS = reg_val;
    return reg_val;
}
    
uint32_t get_mb_flag ( uint32_t lsb_idx, uint32_t num_bits ) {
    uint32_t pattern = (((0xFFFFFFFF << (32 - (lsb_idx + num_bits))) >> (32 - (lsb_idx + num_bits))) >> lsb_idx) << lsb_idx;
    uint32_t reg_val = (*REG_FLAGS & pattern) >> lsb_idx;
    return reg_val;
}
    

//**************************************************
// MBUS IRQ SETTING
//**************************************************
void set_halt_until_reg(uint32_t reg_id) { *SREG_CONF_HALT = reg_id; }
void set_halt_until_mem_wr(void) { *SREG_CONF_HALT = HALT_UNTIL_MEM_WR; }
void set_halt_until_mbus_rx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_RX; }
void set_halt_until_mbus_tx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_TX; }
void set_halt_until_mbus_trx(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_TRX; }
void set_halt_until_mbus_fwd(void) { *SREG_CONF_HALT = HALT_UNTIL_MBUS_FWD; }
void set_halt_disable(void) { *SREG_CONF_HALT = HALT_DISABLE; }
void set_halt_config(uint8_t new_config) { *SREG_CONF_HALT = new_config; }
uint8_t get_current_halt_config(void) { return (uint8_t) *SREG_CONF_HALT; }
void halt_cpu (void) { *SCTR_REG_HALT_ADDR = SCTR_CMD_HALT_CPU; }

//**************************************************
// AES
//**************************************************
void set_aes_key (uint32_t key[]) {
    *AES_KEY_0 = key[0];
    *AES_KEY_1 = key[1];
    *AES_KEY_2 = key[2];
    *AES_KEY_3 = key[3];
}

void set_aes_pt (uint32_t pt[]) {
    *AES_TEXT_0 = pt[0];
    *AES_TEXT_1 = pt[1];
    *AES_TEXT_2 = pt[2];
    *AES_TEXT_3 = pt[3];
}

//**************************************************
// SPI/GPIO Pad and COTS Power Switch
//**************************************************
void set_spi_pad (uint8_t config) {
    uint32_t reg_ = *REG_PERIPHERAL;
    reg_ = reg_ & 0xFFCFFFFF;
    if (config == 1) reg_ = reg_ | 0x00300000;
    *REG_PERIPHERAL = reg_;
}
void set_gpio_pad (uint8_t config) {
    uint32_t reg_ = *REG_PERIPHERAL;
    reg_ = reg_ & 0xFFFFFF00;
    *REG_PERIPHERAL = reg_ | config;
}
void set_gpio_pad_with_mask (uint32_t mask, uint8_t config) {
	mask = mask & 0xFF;
    *REG_PERIPHERAL = (*REG_PERIPHERAL & ~mask) | (mask & config);
}
void freeze_spi_out (void) {
    uint32_t reg_ = *REG_PERIPHERAL | 0x00800000;
    *REG_PERIPHERAL = reg_;
}
void unfreeze_spi_out (void) {
    uint32_t reg_ = *REG_PERIPHERAL & 0xFF7FFFFF;
    *REG_PERIPHERAL = reg_;
}
void freeze_gpio_out (void) {
    uint32_t reg_ = *REG_PERIPHERAL | 0x00010000;
    *REG_PERIPHERAL = reg_;
}
void unfreeze_gpio_out (void) {
    uint32_t reg_ = *REG_PERIPHERAL & 0xFFFEFFFF;
    *GPIO_IRQ_MASK = *R_GPIO_IRQ_MASK;
    *GPIO_DIR = *R_GPIO_DIR;
    *GPIO_DATA = *R_GPIO_DATA;
    *REG_PERIPHERAL = reg_;
}
void config_gpio_posedge_wirq (uint32_t config) {
    config = config & 0xF;
    uint32_t reg_ = *REG_PERIPHERAL & 0xFFFF0FFF;
    reg_ = reg_ | (config << 12);
    *REG_PERIPHERAL = reg_;
}
void config_gpio_negedge_wirq (uint32_t config) {
    config = config & 0xF;
    uint32_t reg_ = *REG_PERIPHERAL & 0xFFFFF0FF;
    reg_ = reg_ | (config << 8);
    *REG_PERIPHERAL = reg_;
}
void set_cps (uint32_t cps_config) {
    *REG_CPS = 0x00000007 & cps_config;
}

//***************************************************
// GPIO
//**************************************************

void gpio_init (uint32_t dir) {
    // Direction: 1=Output, 0=Input
    gpio_set_irq_mask (0x00000000);
    gpio_set_dir (dir);
    gpio_write_data (0x00000000);
    set_gpio_pad(0xFF);
    unfreeze_gpio_out();
}
void gpio_set_dir (uint32_t dir) {
    // Direction: 1=Output, 0=Input
    *R_GPIO_DIR = dir;
    *GPIO_DIR = *R_GPIO_DIR;
}
void gpio_set_dir_with_mask (uint32_t mask, uint32_t dir) {
    // Direction: 1=Output, 0=Input
    *R_GPIO_DIR = (*R_GPIO_DIR & ~mask) | (mask & dir);
    *GPIO_DIR = *R_GPIO_DIR;
}
uint32_t gpio_get_dir (void) {
    return *R_GPIO_DIR;
}
uint32_t gpio_get_data (void) {
    // Read from R_GPIO_DATA if the direction is output.
    // Read from GPIO_DATA if the direction is input.
    return (*R_GPIO_DATA & *R_GPIO_DIR) | *GPIO_DATA;
}
void gpio_write_data (uint32_t data) {
    *R_GPIO_DATA = data;
    *GPIO_DATA = *R_GPIO_DATA;
}
void gpio_write_raw (uint32_t data) {
    *GPIO_DATA = data;
}
void gpio_write_data_with_mask (uint32_t mask, uint32_t data) {
    *R_GPIO_DATA = (*R_GPIO_DATA & ~mask) | (mask & data);
    *GPIO_DATA = *R_GPIO_DATA;
}
void gpio_set_bit (uint32_t loc) {
    *R_GPIO_DATA = (*R_GPIO_DATA | (1 << loc));
    *GPIO_DATA = *R_GPIO_DATA;
}
void gpio_kill_bit (uint32_t loc) {
    *R_GPIO_DATA = ~((~*R_GPIO_DATA) | (1 << loc));
    *GPIO_DATA = *R_GPIO_DATA;
}
void gpio_set_2bits (uint32_t loc0, uint32_t loc1) {
    *R_GPIO_DATA = (*R_GPIO_DATA | (1 << loc0) | (1 << loc1));
    *GPIO_DATA = *R_GPIO_DATA;
}
void gpio_set_irq_mask (uint32_t mask) {
    *R_GPIO_IRQ_MASK = mask;
    *GPIO_IRQ_MASK = *R_GPIO_IRQ_MASK;
}
void gpio_close (void) {
    set_gpio_pad(0x00);
}

//*******************************************************************
// VERIOLG SIM DEBUG PURPOSE ONLY!!
//*******************************************************************
void arb_debug_reg (uint8_t id, uint32_t code) { 
    uint32_t temp_addr = 0xBFFF0000 | (id << 2);
    *((volatile uint32_t *) temp_addr) = code;
}

