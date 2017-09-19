//*******************************************************************
//Author: Yejoong Kim
//Description: PREv14 lib file
//*******************************************************************

#include "PREv14.h"
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

void set_wakeup_timer( uint16_t timestamp, uint8_t irq_en, uint8_t reset ){
	uint32_t regval = timestamp;
	if( irq_en ) regval |= 0x30000; // IRQ in Sleep-Only
	else		 regval &= 0x07FFF;
    *REG_WUPT_CONFIG = regval;

	if( reset ) *WUPT_RESET = 0x01;
}

//**************************************************
// M0 IRQ SETTING
//**************************************************
void enable_all_irq() { *NVIC_ICPR = 0xFFFFFFFF; *NVIC_ISER = 0xFFFFFFFF; }
void disable_all_irq() { *NVIC_ICPR = 0xFFFFFFFF; *NVIC_ICER = 0xFFFFFFFF; }
void clear_all_pend_irq() { *NVIC_ICPR = 0xFFFFFFFF; }
void enable_reg_irq() {	*NVIC_ICPR = 0xFFFFFFFF; *NVIC_ISER = 0xFF<<2; }


//**************************************************
// MBUS IRQ SETTING
//**************************************************
void set_halt_until_reg(uint8_t reg_id) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (((uint32_t) reg_id) << 12);
    *REG_IRQ_CTRL = reg_val;
}

void set_halt_until_mem_wr(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MEM_WR << 12);
    *REG_IRQ_CTRL = reg_val;
}

void set_halt_until_mbus_rx(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MBUS_RX << 12);
    *REG_IRQ_CTRL = reg_val;
}

void set_halt_until_mbus_tx(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MBUS_TX << 12);
    *REG_IRQ_CTRL = reg_val;
}

void set_halt_until_mbus_fwd(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MBUS_FWD << 12);
    *REG_IRQ_CTRL = reg_val;
}

void set_halt_disable(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_DISABLE << 12);
    *REG_IRQ_CTRL = reg_val;
}

void halt_cpu (void) {
    *SYS_CTRL_REG_ADDR = SYS_CTRL_CMD_HALT_CPU;
}

uint8_t get_current_halt_config(void) {
    uint32_t reg_ = *REG_IRQ_CTRL;
    reg_ = (0x0000F000 & reg_) >> 12;
    return (uint8_t) reg_;
}

void set_halt_config(uint8_t new_config) {
    uint32_t reg_ = *REG_IRQ_CTRL;
    reg_ = (0x00010000 & reg_); // reset
    reg_ = reg_ | (((uint32_t) new_config) << 12);
    *REG_IRQ_CTRL = reg_;
}

void enable_old_msg_irq(void) {
    uint32_t reg_ = *REG_IRQ_CTRL;
    reg_ = (0x0000F000 & reg_); // reset
    reg_ = reg_ | (0x1 << 16);
    *REG_IRQ_CTRL = reg_;
}

void disable_old_msg_irq(void) {
    uint32_t reg_ = *REG_IRQ_CTRL;
    reg_ = (0x0000F000 & reg_); // reset
    *REG_IRQ_CTRL = reg_;
}

//**************************************************
// SPI/GPIO Pad and COTS Power Switch
//**************************************************
void set_spi_pad (uint8_t config) {
    uint32_t reg_ = *REG_SPI_GPIO;
    reg_ = reg_ & 0xFFCFFFFF;
    if (config == 1) reg_ = reg_ | 0x00300000;
    *REG_SPI_GPIO = reg_;
}
void set_gpio_pad (uint8_t config) {
    uint32_t reg_ = *REG_SPI_GPIO;
    reg_ = reg_ & 0xFFFFFF00;
    *REG_SPI_GPIO = reg_ | config;
}
void set_cps (uint32_t cps_config) {
    *REG_CPS = 0x00000007 & cps_config;
}
void freeze_spi_pad (uint8_t config) {
    uint32_t reg_ = *REG_SPI_GPIO;
    reg_ = reg_ & 0xFFFFFFFF;
    if (config == 1) reg_ = reg_ | 0x00800000;
    *REG_SPI_GPIO = reg_;
}
void freeze_gpio_pad (uint8_t config) {
    uint32_t reg_ = *REG_SPI_GPIO;
    reg_ = reg_ & 0xFFFEFFFF;
    if (config == 1) reg_ = reg_ | 0x00010000;
    *REG_SPI_GPIO = reg_;
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
void arb_debug_reg (uint32_t code) { *((volatile uint32_t *) 0xAFFFFFF8) = code; }

