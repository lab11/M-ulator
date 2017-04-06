//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv15 lib file
//*******************************************************************

#include "PRCv15.h"
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
void enable_gocep_irq() { *NVIC_ISER = 0x1<<14; }
void reset_all_irq() {clear_all_pend_irq(); disable_all_irq();}


//**************************************************
// MBUS IRQ SETTING
//**************************************************
uint32_t set_halt_until_reg(uint8_t reg_id) {
    uint32_t old_config = (*REG_IRQ_CTRL & 0x0000F000) >> 12;
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (((uint32_t) reg_id) << 12);
    *REG_IRQ_CTRL = reg_val;
    return old_config;
}

uint32_t set_halt_until_mem_wr(void) {
    uint32_t old_config = (*REG_IRQ_CTRL & 0x0000F000) >> 12;
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MEM_WR << 12);
    *REG_IRQ_CTRL = reg_val;
    return old_config;
}

uint32_t set_halt_until_mbus_rx(void) {
    uint32_t old_config = (*REG_IRQ_CTRL & 0x0000F000) >> 12;
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MBUS_RX << 12);
    *REG_IRQ_CTRL = reg_val;
    return old_config;
}

uint32_t set_halt_until_mbus_tx(void) {
    uint32_t old_config = (*REG_IRQ_CTRL & 0x0000F000) >> 12;
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MBUS_TX << 12);
    *REG_IRQ_CTRL = reg_val;
    return old_config;
}

uint32_t set_halt_until_mbus_fwd(void) {
    uint32_t old_config = (*REG_IRQ_CTRL & 0x0000F000) >> 12;
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MBUS_FWD << 12);
    *REG_IRQ_CTRL = reg_val;
    return old_config;
}

uint32_t set_halt_until_mbus_trx(void) {
    uint32_t old_config = (*REG_IRQ_CTRL & 0x0000F000) >> 12;
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MBUS_TRX << 12);
    *REG_IRQ_CTRL = reg_val;
    return old_config;
}

uint32_t set_halt_disable(void) {
    uint32_t old_config = (*REG_IRQ_CTRL & 0x0000F000) >> 12;
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_DISABLE << 12);
    *REG_IRQ_CTRL = reg_val;
    return old_config;
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
    reg_ = (0xFFFF0FFF & reg_); // reset
    reg_ = reg_ | (((uint32_t) new_config) << 12);
    *REG_IRQ_CTRL = reg_;
}

void enable_old_msg_irq(void) {
    uint32_t reg_ = *REG_IRQ_CTRL;
    reg_ = (0xFFFEFFFF & reg_); // reset
    reg_ = reg_ | (0x1 << 16);
    *REG_IRQ_CTRL = reg_;
}

void disable_old_msg_irq(void) {
    uint32_t reg_ = *REG_IRQ_CTRL;
    reg_ = (0xFFFEFFFF & reg_); // reset
    *REG_IRQ_CTRL = reg_;
}

//*******************************************************************
// VERIOLG SIM DEBUG PURPOSE ONLY!!
//*******************************************************************
void arb_debug_reg (uint32_t code) { *((volatile uint32_t *) 0xAFFFFFF8) = code; }

