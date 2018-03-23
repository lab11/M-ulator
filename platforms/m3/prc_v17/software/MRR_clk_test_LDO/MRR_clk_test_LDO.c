//*******************************************************************
//Author: Inhee Lee 
//Description: MRRv6's CLK Testing
//*******************************************************************
#include "PRCv17.h"
#include "PRCv17_RF.h"
#include "MRRv6_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define MRR_ADDR    0x2

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile mrrv6_r04_t mrrv6_r04 = MRRv6_R04_DEFAULT;
volatile mrrv6_r05_t mrrv6_r05 = MRRv6_R05_DEFAULT;
volatile mrrv6_r07_t mrrv6_r07 = MRRv6_R07_DEFAULT;
volatile mrrv6_r08_t mrrv6_r08 = MRRv6_R08_DEFAULT;

volatile prcv17_r0B_t prcv17_r0B = PRCv17_R0B_DEFAULT;

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

//********************************************************************
// MAIN function starts here             
//***************************************************************-*****

int main() {

  disable_all_irq();
  config_timerwd(0xFFFFFFFF); // Config watchdog timer to about 20 sec (default: 0x0016E360)
 
  // Set CPU & Mbus Clock Speeds
  prcv17_r0B.CLK_GEN_RING = 0x1; // Default 0x1
  prcv17_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
  prcv17_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
  *((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv17_r0B.as_int;

  
  // Enumerate
  set_halt_until_mbus_rx();
  mbus_enumerate(MRR_ADDR);
  enumerated = 0xDEADBEEF;
  set_halt_until_mbus_tx();
  
  // Disable MBus Watchdog Timer
  *REG_MBUS_WD = 0;

  //LDO_EN_VREF
  mbus_remote_register_write(MRR_ADDR,0x04, 
	( (0 << 16) // 4'h0 LDO_VREF_I_AMP
	| (4 << 13) // 3'h4 LDO_SEL_VOUT
	| (1 << 12) // 1'h0 LDO_EN_VREF
	| (0 << 11) // 1'h0 LDO_EN_IREF
	| (0 << 10) // 1'h0 LDO_EN_LDO
	| (0 << 9) // 1'h0 RO_EN_MONITOR
	| (0 << 7) // 2'h0 RO_SEL_DIV
	| (0 << 6) // 1'h0 RO_SEL_EXT_CLK
	| (0 << 5) // 1'h0 RO_EN_CLK
	| (1 << 4) // 1'h1 RO_ISOLATE_CLK
	| (1 << 3) // 1'h1 RO_RESET ~RO_RESET_B~
	| (0 << 2) // 1'h0 RO_EN_RO_LDO
	| (0 << 1) // 1'h0 RO_EN_RO_V1P2_PRE
	| (0) 	   // 1'h0 RO_EN_RO_V1P2
  ));
  //delay(10); 

  //LDO_EN_IREF & LDO_EN_LDO
  mbus_remote_register_write(MRR_ADDR,0x04, 
	( (0 << 16) // 4'h0 LDO_VREF_I_AMP
	| (4 << 13) // 3'h4 LDO_SEL_VOUT
	| (1 << 12) // 1'h0 LDO_EN_VREF
	| (1 << 11) // 1'h0 LDO_EN_IREF
	| (1 << 10) // 1'h0 LDO_EN_LDO
	| (0 << 9) // 1'h0 RO_EN_MONITOR
	| (0 << 7) // 2'h0 RO_SEL_DIV
	| (0 << 6) // 1'h0 RO_SEL_EXT_CLK
	| (0 << 5) // 1'h0 RO_EN_CLK
	| (1 << 4) // 1'h1 RO_ISOLATE_CLK
	| (1 << 3) // 1'h1 RO_RESET ~RO_RESET_B~
	| (0 << 2) // 1'h0 RO_EN_RO_LDO
	| (0 << 1) // 1'h0 RO_EN_RO_V1P2_PRE
	| (0) 	// 1'h0 RO_EN_RO_V1P2
  ));
  //delay(10); 

  //Adjust Capacitance
  mbus_remote_register_write(MRR_ADDR,0x07, 
	( (0x10 << 7) // 4'h1C RO_MIM (0x16)
	| (0x10) 	// 7'h1C RO_MOM
  ));
  //delay(10); 

  //Adjust Diffusion Resistor
  mbus_remote_register_write(MRR_ADDR,0x06, 
	( (0x1000) // 15'h0080 RO_PDIFF
  ));
  //delay(10); 

  //Adjust Poly Resistor
  mbus_remote_register_write(MRR_ADDR,0x08, 
	( (0x400000) // 24'h000040 RO_POLY
  ));
  //delay(10); 

  //Adjust CMP Power
  mbus_remote_register_write(MRR_ADDR,0x05, 
	( (8 << 20) // 4'h8 RO_R_REF
	| (0 << 16) // 4'h0 RO_I_BUF
	| (0 << 12) // 4'h0 RO_I_BUF2
	| (1 << 8) // 4'h1 RO_CMP
	| (1 << 6) // 2'h1 RO_MIRROR
	| (1 << 4) // 2'h1 RO_MIRROR2
	| (13) 	// 4'hD RO_V_MIRRb
  ));
  //delay(10); 
  
  //RO_EN_RO_LDO
  mbus_remote_register_write(MRR_ADDR,0x04, 
	( (0 << 16) // 4'h0 LDO_VREF_I_AMP
	| (4 << 13) // 3'h4 LDO_SEL_VOUT
	| (1 << 12) // 1'h0 LDO_EN_VREF
	| (1 << 11) // 1'h0 LDO_EN_IREF
	| (1 << 10) // 1'h0 LDO_EN_LDO
	| (0 << 9) // 1'h0 RO_EN_MONITOR
	| (0 << 7) // 2'h0 RO_SEL_DIV
	| (0 << 6) // 1'h0 RO_SEL_EXT_CLK
	| (0 << 5) // 1'h0 RO_EN_CLK
	| (1 << 4) // 1'h1 RO_ISOLATE_CLK
	| (1 << 3) // 1'h1 RO_RESET ~RO_RESET_B~
	| (1 << 2) // 1'h0 RO_EN_RO_LDO
	| (0 << 1) // 1'h0 RO_EN_RO_V1P2_PRE
	| (0) 	// 1'h0 RO_EN_RO_V1P2
  ));
  //delay(10); 

  //RO_ISOLATE_CLK RO_EN_CLK, RO_EN_MONITOR & RO_RESET
  mbus_remote_register_write(MRR_ADDR,0x04, 
	( (0 << 16) // 4'h0 LDO_VREF_I_AMP
	| (4 << 13) // 3'h4 LDO_SEL_VOUT
	| (1 << 12) // 1'h0 LDO_EN_VREF
	| (1 << 11) // 1'h0 LDO_EN_IREF
	| (1 << 10) // 1'h0 LDO_EN_LDO
	| (1 << 9) // 1'h0 RO_EN_MONITOR
	| (0 << 7) // 2'h0 RO_SEL_DIV
	| (0 << 6) // 1'h0 RO_SEL_EXT_CLK
	| (1 << 5) // 1'h0 RO_EN_CLK
	| (0 << 4) // 1'h1 RO_ISOLATE_CLK
	| (0 << 3) // 1'h1 RO_RESET ~RO_RESET_B~
	| (1 << 2) // 1'h0 RO_EN_RO_LDO
	| (0 << 1) // 1'h0 RO_EN_RO_V1P2_PRE
	| (0) 	// 1'h0 RO_EN_RO_V1P2
  ));
//  delay(10); 

// //RO_EN_CLK
// mbus_remote_register_write(MRR_ADDR,0x04, 
//   ( (0 << 16) // 4'h0 LDO_VREF_I_AMP
//   | (4 << 13) // 3'h4 LDO_SEL_VOUT
//   | (1 << 12) // 1'h0 LDO_EN_VREF
//   | (1 << 11) // 1'h0 LDO_EN_IREF
//   | (1 << 10) // 1'h0 LDO_EN_LDO
//   | (1 << 9) // 1'h0 RO_EN_MONITOR
//   | (0 << 7) // 2'h0 RO_SEL_DIV
//   | (0 << 6) // 1'h0 RO_SEL_EXT_CLK
//   | (1 << 5) // 1'h0 RO_EN_CLK
//   | (0 << 4) // 1'h1 RO_ISOLATE_CLK
//   | (1 << 3) // 1'h1 RO_RESET ~RO_RESET_B~
//   | (1 << 2) // 1'h0 RO_EN_RO_LDO
//   | (0 << 1) // 1'h0 RO_EN_RO_V1P2_PRE
//   | (0) 	// 1'h0 RO_EN_RO_V1P2
// ));

  //RO_RESET
// mbus_remote_register_write(MRR_ADDR,0x04, 
//   ( (0 << 16) // 4'h0 LDO_VREF_I_AMP
//   | (4 << 13) // 3'h4 LDO_SEL_VOUT
//   | (1 << 12) // 1'h0 LDO_EN_VREF
//   | (1 << 11) // 1'h0 LDO_EN_IREF
//   | (1 << 10) // 1'h0 LDO_EN_LDO
//   | (1 << 9) // 1'h0 RO_EN_MONITOR
//   | (0 << 7) // 2'h0 RO_SEL_DIV
//   | (0 << 6) // 1'h0 RO_SEL_EXT_CLK
//   | (1 << 5) // 1'h0 RO_EN_CLK
//   | (0 << 4) // 1'h1 RO_ISOLATE_CLK
//   | (0 << 3) // 1'h1 RO_RESET ~RO_RESET_B~
//   | (1 << 2) // 1'h0 RO_EN_RO_LDO
//   | (0 << 1) // 1'h0 RO_EN_RO_V1P2_PRE
//   | (0) 	// 1'h0 RO_EN_RO_V1P2
// ));
 
  while (1){
	  delay(1000);
  }

  return 1;
}

