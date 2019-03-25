//*******************************************************************
//Author: Yejoong Kim
//Description: PREv20 lib file
//*******************************************************************

#include "PREv20.h"
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

void enable_timerwd(){
	*TIMERWD_GO  = 0x1;
}

void set_wakeup_timer( uint32_t timestamp, uint8_t irq_en, uint8_t reset ){
	uint32_t regval = timestamp;
	if( irq_en ) regval |= 0x800000; // IRQ in Sleep-Only
	else		 regval &= 0x3FFFFF;
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
    uint32_t regval0 = timestamp & 0x0000FFFF;
    uint32_t regval1 = timestamp & 0xFFFF0000;

    regval0 |= 0x00800000; // XOT_ENABLE = 1;
    if (mode)    regval0 |= 0x00400000; // XOT_MODE = 1
    if (wreq_en) regval0 |= 0x00200000; // XOT_WREQ_EN = 1
    if (irq_en)  regval0 |= 0x00100000; // XOT_IRQ_EN = 1

    *REG_XOT_CONFIGU = regval1;
    *REG_XOT_CONFIG  = regval0;
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
volatile uint32_t gpio_data_;

void gpio_init (uint32_t dir) {
    // Direction: 1=Output, 0=Input
    gpio_set_irq_mask (0x00000000);
    gpio_set_dir (dir);
    gpio_set_data (0x00000000);
    set_gpio_pad(0xFF);
    unfreeze_gpio_out();
}
void gpio_set_dir (uint32_t dir) {
    // Direction: 1=Output, 0=Input
    *GPIO_DIR = dir;
}
uint32_t gpio_get_dir (void) {
    return *GPIO_DIR;
}
uint32_t gpio_get_data (void) {
    return *GPIO_DATA;
}
void gpio_set_data (uint32_t data) {
    gpio_data_ = data;
}
void gpio_write_data (uint32_t data) {
    gpio_data_ = data;
    *GPIO_DATA = gpio_data_;
}
void gpio_write_current_data (void) {
    *GPIO_DATA = gpio_data_;
}
void gpio_write_raw (uint32_t data) {
    *GPIO_DATA = data;
}
void gpio_set_bit (uint32_t loc) {
    gpio_data_ = (gpio_data_ | (1 << loc));
    gpio_write_current_data();
}
void gpio_kill_bit (uint32_t loc) {
    gpio_data_ = ~((~gpio_data_) | (1 << loc));
    gpio_write_current_data();
}
void gpio_set_2bits (uint32_t loc0, uint32_t loc1) {
    gpio_data_ = (gpio_data_ | (1 << loc0) | (1 << loc1));
    gpio_write_current_data();
}
void gpio_set_irq_mask (uint32_t mask) {
    *GPIO_IRQ_MASK = mask;
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
