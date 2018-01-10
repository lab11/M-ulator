//*******************************************************************
//Author: Seokhyeon 
//Description: SNTv1's CLK Testing
//*******************************************************************
#include "PRCv17.h"
#include "PRCv17_RF.h"
#include "SNTv1_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define SNT_ADDR    0x2

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile sntv1_r08_t sntv1_r08 = SNTv1_R08_DEFAULT;
volatile sntv1_r09_t sntv1_r09 = SNTv1_R09_DEFAULT;
volatile sntv1_r0A_t sntv1_r0A = SNTv1_R0A_DEFAULT;
volatile sntv1_r17_t sntv1_r17 = SNTv1_R17_DEFAULT;

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

static void operation_sleep(void){

	// Reset GOC_DATA_IRQ
	*GOC_DATA_IRQ = 0;

    // Go to Sleep
    mbus_sleep_all();
    while(1);

}
static void operation_sleep_notimer(void){
	// Disable Timer
	set_wakeup_timer(0, 0, 0);

    // Go to sleep
    operation_sleep();

}
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
  mbus_enumerate(SNT_ADDR);
  enumerated = 0xDEADBEEF;
  set_halt_until_mbus_tx();
  
  // Disable MBus Watchdog Timer
  *REG_MBUS_WD = 0;






 mbus_remote_register_write(SNT_ADDR,0x17, 
   ( (0 << 23) // 1'h0 WUP_ENABLE
   | (1 << 22) // 1'h1 WUP_LC_IRQ_EN
   | (1 << 21) 	// 1'h1 WUP_AUTO_RESET
   | (0 << 20) 	// 1'h0 WUP_CLK_SEL
   | (1 << 19)	// 1'h0 WUP_ENABLE_CLK_SLP_OUT
   | (0x10 << 8) 	// 8'h10 WUP_INT_RPLY_SHORT_ADDR
   | (0x07) 	    // 8'h07 WUP_INT_RPLY_REG_ADDR
 ));

 // TIMER SELF_EN Disable
 mbus_remote_register_write(SNT_ADDR,0x09, 
   ( (1 << 23) // 1'h1 TMR_SEL_CLK_DIV
   | (1 << 22) // 1'h1 TMR_SEL_CLK_OSC
   | (0 << 21) 	// 1'h1 TMR_SELF_EN
   | (4 << 17) 	// 4'h4 TMR_IBIAS_REF
   | (0 << 16)	// 1'h0 TMR_CASCODE_BOOST
   | (8 << 8) 	// 8'h8 TMR_SEL_CAP
   | (4 << 2) 	// 6'h4 TMR_SEL_DCAP
   | (1 << 1) 	// 1'h1 TMR_EN_TUNE2
   | (1) 	    // 1'h1 TMR_EN_TUNE1
 ));

 //EN OSC
 mbus_remote_register_write(SNT_ADDR,0x08, 
   ( (0 << 4)  // 1'h0 TMR_RESETB 
   | (1 << 3) 	// 1'h0 TMR_EN_OSC
   | (0 << 2) 	// 1'h0 TMR_RESETB_DIV
   | (0 << 1) 	// 1'h0 TMR_RESETB_DCDC
   | (0) 	    // 1'h0 TMR_EN_SELF_CLK
 ));

 // Release Reset
 mbus_remote_register_write(SNT_ADDR,0x08, 
   ( (1 << 4)  // 1'h0 TMR_RESETB 
   | (1 << 3) 	// 1'h0 TMR_EN_OSC
   | (1 << 2) 	// 1'h0 TMR_RESETB_DIV
   | (1 << 1) 	// 1'h0 TMR_RESETB_DCDC
   | (0) 	    // 1'h0 TMR_EN_SELF_CLK
 ));
 delay(2000); 

 // TIMER EN_SELF_CLK
 mbus_remote_register_write(SNT_ADDR,0x08, 
   ( (1 << 4)  // 1'h0 TMR_RESETB 
   | (1 << 3) 	// 1'h0 TMR_EN_OSC
   | (1 << 2) 	// 1'h0 TMR_RESETB_DIV
   | (1 << 1) 	// 1'h0 TMR_RESETB_DCDC
   | (1) 	    // 1'h0 TMR_EN_SELF_CLK
 ));
 delay(10); 


 // TIMER SELF_EN
 mbus_remote_register_write(SNT_ADDR,0x09, 
   ( (1 << 23) // 1'h1 TMR_SEL_CLK_DIV
   | (1 << 22) // 1'h1 TMR_SEL_CLK_OSC
   | (1 << 21) 	// 1'h1 TMR_SELF_EN
   | (1 << 17) 	// 4'h4 TMR_IBIAS_REF
   | (0 << 16)	// 1'h0 TMR_CASCODE_BOOST
   | (0x08 << 8) 	// 8'h8 TMR_SEL_CAP
   | (2 << 2) 	// 6'h4 TMR_SEL_DCAP
   | (0 << 1) 	// 1'h1 TMR_EN_TUNE2
   | (0) 	    // 1'h1 TMR_EN_TUNE1
 ));
 delay(100000); 

 // Disable OSC
 mbus_remote_register_write(SNT_ADDR,0x08, 
   ( (1 << 4)  // 1'h0 TMR_RESETB 
   | (1 << 3) 	// 1'h0 TMR_EN_OSC
   | (1 << 2) 	// 1'h0 TMR_RESETB_DIV
   | (1 << 1) 	// 1'h0 TMR_RESETB_DCDC
   | (1) 	    // 1'h0 TMR_EN_SELF_CLK
 ));
 delay(100); 

 // Config Register A
 mbus_remote_register_write(SNT_ADDR,0x0A, 
   ( (4 << 21)          // 3'h4 TMR_S 
   | (0x3FFB << 7)    // 14'h3FFFB TMR_DIFF_CON
   | (1 << 6) 	        // 1'h1 TMR_POLY_CON
   | (1 << 5) 	        // 1'h1 TMR_EN_TUNE1_RES
   | (1 << 4) 	        // 1'h1 TMR_EN_TUNE2_RES
   | (1 << 3) 	        // 1'h1 TMR_SAMPLE_EN
   | (4) 	            // 3'h4 TMR_AFC
 ));

//operation_sleep_notimer();

 while (1){
     delay(1000);
 }

 return 1;
}

