//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv12 lib file
//*******************************************************************

#include "PRCv12.h"

//*******************************************************************
// OTHER FUNCTIONS
//*******************************************************************

void write_config_reg(uint8_t reg, uint32_t data ){
  uint32_t _addr = 0xA0000000;
  _addr |= (reg << 2);
  
  *((volatile uint32_t *) _addr) = data;
}

int read_config_reg(uint8_t reg){
  uint32_t _addr = 0xA0000000;
  _addr |= (reg << 2);
  return *((volatile uint32_t *) _addr);
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
	*REG_WUPT_CONFIG = regval;

	if( reset ) *WUPT_RESET = 0x01;
}

void set_clkfreq( uint8_t fastmode, uint8_t div_core, uint8_t div_mbus, uint8_t ring ) {
	uint32_t regval = *REG_CLKGEN_TUNE;		// Read original reg value
	regval &= 0x00001FFF; // Reset reg value
	regval |= (fastmode<<19) | (div_core<<17) | (div_mbus<<15) | (ring<<13) ;
	*REG_CLKGEN_TUNE = regval; // Write updated reg value
}

// FIXME: Not Verified (Yejoong Kim)
void pon_reset( void ){
	*((volatile uint32_t *) 0xA3000000) = 0x2;
}

// FIXME: Not Verified (Yejoong Kim)
void sleep_req_by_sw( void ){
	*((volatile uint32_t *) 0xA3000000) = 0x4;
}

//**************************************************
// MBUS IRQ SETTING (All Verified)
//**************************************************
void set_halt_until_reg(uint8_t reg_id) {
    //assert (reg_id < 8);
    prcv12_r0A.CONFIG_HALT_CPU = reg_id;
    write_config_reg(0xA,prcv12_r0A.as_int);
}

void set_halt_until_mem_wr(void) {
    prcv12_r0A.CONFIG_HALT_CPU = HALT_UNTIL_MEM_WR;
    write_config_reg(0xA,prcv12_r0A.as_int);
}

void set_halt_until_mbus_rx(void) {
    prcv12_r0A.CONFIG_HALT_CPU = HALT_UNTIL_MBUS_RX;
    write_config_reg(0xA,prcv12_r0A.as_int);
}

void set_halt_until_mbus_tx(void) {
    prcv12_r0A.CONFIG_HALT_CPU = HALT_UNTIL_MBUS_TX;
    write_config_reg(0xA,prcv12_r0A.as_int);
}

void set_halt_until_mbus_fwd(void) {
    prcv12_r0A.CONFIG_HALT_CPU = HALT_UNTIL_MBUS_FWD;
    write_config_reg(0xA,prcv12_r0A.as_int);
}

void set_halt_disable(void) {
    prcv12_r0A.CONFIG_HALT_CPU = HALT_DISABLE;
    write_config_reg(0xA,prcv12_r0A.as_int);
}

void disable_all_mbus_irq(void) {
    prcv12_r0A.RF_WR_IRQ_MASK = 0x00;
    prcv12_r0A.MEM_WR_IRQ_MASK = 0x0;
    prcv12_r0A.MBUS_FWD_IRQ_MASK = 0x0;
    prcv12_r0A.MBUS_RX_IRQ_MASK = 0x0;
    prcv12_r0A.MBUS_TX_IRQ_MASK = 0x0;
    prcv12_r0A.OLD_MSG_REG_MASK = 0x1;
    prcv12_r0A.CONFIG_HALT_CPU = HALT_DISABLE;
    write_config_reg(0xA,prcv12_r0A.as_int);
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
    prcv12_r0A.RF_WR_IRQ_MASK = RF_WR;
    prcv12_r0A.MEM_WR_IRQ_MASK = MEM_WR;
    prcv12_r0A.MBUS_RX_IRQ_MASK = MBUS_RX;
    prcv12_r0A.MBUS_TX_IRQ_MASK = MBUS_TX;
    prcv12_r0A.MBUS_FWD_IRQ_MASK = MBUS_FWD;
    prcv12_r0A.OLD_MSG_REG_MASK = OLD_MSG;
    prcv12_r0A.CONFIG_HALT_CPU = HALT_CONFIG;
    write_config_reg(0xA,prcv12_r0A.as_int);
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
    *REG_IRQ_CTRL = reg_;
}

//*******************************************************************
// VERIOLG SIM DEBUG PURPOSE ONLY!!
//*******************************************************************
void arb_debug_reg (uint32_t code) { *((volatile uint32_t *) 0xAFFFFFF8) = code; }


