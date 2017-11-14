//*******************************************************************
//Author: ZhiYoong Foo
//Description: PREv17's XTAL Testing
//*******************************************************************
#include "PREv17.h"
#include "PREv17_RF.h"
#include "mbus.h"

#define PRE_ADDR    0x1
#define SNS_ADDR    0x2

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_0(void)  __attribute__ ((interrupt ("IRQ")));
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
void handler_ext_int_14(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0(void)  { *NVIC_ICPR = (0x1 << 0);  } // TIMER32
void handler_ext_int_1(void)  { *NVIC_ICPR = (0x1 << 1);  } // TIMER16
void handler_ext_int_2(void)  { *NVIC_ICPR = (0x1 << 2);  } // REG0
void handler_ext_int_3(void)  { *NVIC_ICPR = (0x1 << 3);  } // REG1
void handler_ext_int_4(void)  { *NVIC_ICPR = (0x1 << 4);  } // REG
void handler_ext_int_5(void)  { *NVIC_ICPR = (0x1 << 5);  } // REG3
void handler_ext_int_6(void)  { *NVIC_ICPR = (0x1 << 6);  } // REG4
void handler_ext_int_7(void)  { *NVIC_ICPR = (0x1 << 7);  } // REG5
void handler_ext_int_8(void)  { *NVIC_ICPR = (0x1 << 8);  } // REG6
void handler_ext_int_9(void)  { *NVIC_ICPR = (0x1 << 9);  } // REG7
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); } // MEM WR
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); } // MBUS_RX
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); } // MBUS_TX
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); } // MBUS_FWD
void handler_ext_int_14(void) { *NVIC_ICPR = (0x1 << 14); } // MBUS_FWD

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void XO_ctrl (uint32_t xo_pulse_sel,
	      uint32_t xo_delay_en,
	      uint32_t xo_drv_start_up,
	      uint32_t xo_drv_core,
	      uint32_t xo_rp_low,
	      uint32_t xo_rp_media,
	      uint32_t xo_rp_mvt,
	      uint32_t xo_rp_svt,
	      uint32_t xo_scn_clk_sel,
	      uint32_t xo_scn_enb
	      ){
  
  *REG_XO_CONTROL =((xo_pulse_sel    << 11) |
		    (xo_delay_en     << 8) |
		    (xo_drv_start_up << 7) |
		    (xo_drv_core     << 6) |
		    (xo_rp_low       << 5) |
		    (xo_rp_media     << 4) |
		    (xo_rp_mvt       << 3) |
		    (xo_rp_svt       << 2) |
		    (xo_scn_clk_sel  << 1) |
		    (xo_scn_enb      << 0));
}

static void XO_init(void) {

  // XO_CLK Output Pad (0: Disabled, 1: 32kHz, 2: 16kHz, 3: 8kHz)
  uint32_t xot_clk_out_sel = 0x0;
  // Parasitic Capacitance Tuning (6-bit for each; Each 1 adds 1.8pF)
  uint32_t xo_cap_in  = 0x1F; // Additional Cap on OSC_IN
  uint32_t xo_cap_drv = 0x00; // Additional Cap on OSC_DRV
  
  // Pulse Length Selection
  uint32_t xo_pulse_sel = 0x4; // XO_PULSE_SEL
  uint32_t xo_delay_en  = 0x3; // XO_DELAY_EN
  
  // Pseudo-Resistor Selection
  uint32_t xo_rp_low   = 0x0;
  uint32_t xo_rp_media = 0x1;
  uint32_t xo_rp_mvt   = 0x0;
  uint32_t xo_rp_svt   = 0x0;
		    
  // Parasitic Capacitance Tuning
  *REG_XO_CONFIG = ((xot_clk_out_sel << 16) |
		    (cap_in          << 6) |
		    (cap_drv         << 0));
  
  // Start XO Clock
  //XO_ctrl(xo_pulse_sel, xo_delay_en, xo_drv_start_up, xo_drv_core, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, xo_scn_clk_sel, xo_scn_enb);
  XO_ctrl(xo_pulse_sel, xo_delay_en, 0, 0, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 0, 1); delay(10000); //Default
  XO_ctrl(xo_pulse_sel, xo_delay_en, 1, 0, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 0, 1); delay(10000); //XO_DRV_START_UP = 1
  XO_ctrl(xo_pulse_sel, xo_delay_en, 1, 0, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 1, 1); delay(10000); //XO_SCN_CLK_SEL = 1
  XO_ctrl(xo_pulse_sel, xo_delay_en, 1, 0, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 0, 0); delay(10000); //XO_SCN_CLK_SEL = 0 & XO_SCN_ENB = 0
  XO_ctrl(xo_pulse_sel, xo_delay_en, 0, 1, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 0, 0); delay(10000); //XO_DRV_START_UP = 0 & XO_DRV_CORE = 1
}

static void XOT_init(void){
  *XOT_RESET = 0x1;
}

static void operation_init(void){
  // Set CPU & Mbus Clock Speeds
  prev17_r0B.CLK_GEN_RING           = 0x1; // Default 0x1
  prev17_r0B.CLK_GEN_DIV_MBC        = 0x1; // Default 0x1
  prev17_r0B.CLK_GEN_DIV_CORE       = 0x3; // Default 0x3
  *REG_CLKGEN_TUNE = prev17_r0B.as_int;
  
  // Enumerate
  set_halt_until_mbus_rx();
  mbus_enumerate(SNS_ADDR);
  enumerated = 0xDEADBEEF;
  
  // Disable MBus Watchdog Timer
  *REG_MBUS_WD = 0;
}

//********************************************************************
// MAIN function starts here             
//***************************************************************-*****

int main() {

  disable_all_irq();

  config_timerwd(0xFFFFFFFF); // Config watchdog timer to about 20 sec (default: 0x0016E360)

  XO_init();

  XOT_init();

  while(1){
    delay(100);
    mbus_write_message32(0xAA,*XOT_VAL);
  }
  
  while(1){  //Never Quit (should not come here.)
    mbus_write_message32(0xF0,0xDEADBEEF);
    operation_sleep_notimer();
    asm("nop;"); 
  }
  
  return 1;
}

