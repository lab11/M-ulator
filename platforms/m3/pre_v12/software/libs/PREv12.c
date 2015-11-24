//*******************************************************************
//Author: Yejoong Kim
//Description: PREv12 lib file
//*******************************************************************

#include "PREv12.h"
#include "mbus.h"

//*******************************************************************
// OTHER FUNCTIONS
//*******************************************************************

void write_regfile (volatile uint32_t* reg_addr, uint32_t data) {
    uint32_t reg_id = ((uint32_t) reg_addr >> 2) & 0x000000FF;
    data = data & 0x00FFFFFF;
    *MBUS_CMD0 = (reg_id << 24) | data;
    *MBUS_FUID_LEN = MPQ_REG_WRITE | (0x1 << 4);
    delay(10);
}

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

void set_wakeup_timer( uint16_t timestamp, uint8_t irq_en, uint8_t reset ){
	uint32_t regval = timestamp;
	if( irq_en ) regval |= 0x8000;
	else		 regval &= 0x7FFF;
	write_regfile (REG_WUPT_CONFIG, regval);

	if( reset ) *WUPT_RESET = 0x01;
}

//**************************************************
// MBUS IRQ SETTING (All Verified)
//**************************************************
void set_halt_until_reg(uint8_t reg_id) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFFFF00;
    reg_val = reg_val | ((uint32_t) reg_id);
    write_regfile (REG_IRQ_CTRL, reg_val);
}

void set_halt_until_mem_wr(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MEM_WR << 12);
    write_regfile (REG_IRQ_CTRL, reg_val);
}

void set_halt_until_mbus_rx(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MBUS_RX << 12);
    write_regfile (REG_IRQ_CTRL, reg_val);
}

void set_halt_until_mbus_tx(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MBUS_TX << 12);
    write_regfile (REG_IRQ_CTRL, reg_val);
}

void set_halt_until_mbus_fwd(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MBUS_FWD << 12);
    write_regfile (REG_IRQ_CTRL, reg_val);
}

void set_halt_disable(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_DISABLE << 12);
    write_regfile (REG_IRQ_CTRL, reg_val);
}

void disable_all_mbus_irq(void) {
    write_regfile (REG_IRQ_CTRL, 0x0001F000);
}

void halt_cpu (void) {
    *SYS_CTRL_REG_ADDR = SYS_CTRL_CMD_HALT_CPU;
}

void set_mbus_irq_reg(
        uint8_t RF_WR, 
        uint8_t MEM_WR, 
        uint8_t MBUS_RX, 
        uint8_t MBUS_TX, 
        uint8_t MBUS_FWD, 
        uint8_t OLD_MSG, 
        uint8_t HALT_CONFIG
        ) {
    write_regfile (REG_IRQ_CTRL, 
                      (OLD_MSG << 16)
                    | (HALT_CONFIG << 12)
                    | (MBUS_FWD << 11)
                    | (MBUS_TX << 10)
                    | (MBUS_RX << 9)
                    | (MEM_WR << 8)
                    | (RF_WR << 0)
                );
}

uint8_t get_current_halt_config(void) {
    uint32_t reg_ = *REG_IRQ_CTRL;
    reg_ = (0x0000F000 & reg_) >> 12;
    return (uint8_t) reg_;
}

void set_halt_config(uint8_t new_config) {
    uint32_t reg_ = *REG_IRQ_CTRL;
    reg_ = (0x00010FFF & reg_); // reset
    reg_ = reg_ | (((uint32_t) new_config) << 12);
    write_regfile (REG_IRQ_CTRL, reg_);
}

//**************************************************
// IO Pad and COTS Power Switch
//**************************************************
void enable_io_pad (void) {
    write_regfile(REG_PAD_EN, 0x3);
}
void disable_io_pad (void) {
    write_regfile(REG_PAD_EN, 0x0);
}
void set_cps (uint32_t cps_config) {
    write_regfile(REG_CPS, (0x00000007 & cps_config));
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
    enable_io_pad();
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
    disable_io_pad();
}

//*******************************************************************
// VERIOLG SIM DEBUG PURPOSE ONLY!!
//*******************************************************************
void arb_debug_reg (uint32_t code) { *((volatile uint32_t *) 0xAFFFFFF8) = code; }

