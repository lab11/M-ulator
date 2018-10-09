//*******************************************************************
//Author: Sechang Oh
//Description: PREv18's XTAL Testing
//*******************************************************************
#include "PREv18.h"
#include "PREv18_RF.h"
#include "mbus.h"

#define PRE_ADDR    0x1
#define PMU_ADDR    0x6

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;

volatile prev18_r0B_t prev18_r0B = PREv18_R0B_DEFAULT;
volatile prev18_r19_t prev18_r19 = PREv18_R19_DEFAULT;
volatile prev18_r1A_t prev18_r1A = PREv18_R1A_DEFAULT;
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
void handler_ext_int_4(void)  { *NVIC_ICPR = (0x1 << 4);  } // REG2
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
// FUNCTION HEADER DECLERATIONS
//*******************************************************************

//***************************************************
// End of Program Sleep Functions
//***************************************************
static void operation_sleep(void);
static void operation_sleep_noirqreset(void);
static void operation_sleep_notimer(void);

//***************************************************
// End of Program Sleep Functions
//***************************************************
static void operation_sleep(void){
  // Reset GOC_DATA_IRQ
  *GOC_DATA_IRQ = 0;
  mbus_sleep_all();
  while(1);
}

static void operation_sleep_noirqreset(void){
  mbus_sleep_all();
  while(1);
}

static void operation_sleep_notimer(void){
  set_wakeup_timer(0, 0, 0);
  operation_sleep();
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
static void XO_init(void) {
    // Parasitic Capacitance Tuning (6-bit for each; Each 1 adds 1.8pF)
    uint32_t xo_cap_drv = 0x3F; // Additional Cap on OSC_DRV
    uint32_t xo_cap_in  = 0x3F; // Additional Cap on OSC_IN
    prev18_r1A.XO_CAP_TUNE = (
            (xo_cap_drv <<6) | 
            (xo_cap_in <<0));   // XO_CLK Output Pad (0: Disabled, 1: 32kHz, 2: 16kHz, 3: 8kHz)
    *REG_XO_CONF2 = prev18_r1A.as_int;

    // XO configuration
    prev18_r19.XO_EN_DIV    = 0x1;// divider enable
    prev18_r19.XO_S         = 0x5;// division ratio
    prev18_r19.XO_SEL_CP_DIV= 0x0;// 1: 0.3V-generation charge-pump uses divided clock
    prev18_r19.XO_EN_OUT    = 0x1;// XO ouput enable
    prev18_r19.XO_PULSE_SEL = 0x4;// pulse width sel, 1-hot code
    prev18_r19.XO_DELAY_EN  = 0x3;// pair usage together with xo_pulse_sel
    // Pseudo-Resistor Selection
    prev18_r19.XO_RP_LOW    = 0x0;
    prev18_r19.XO_RP_MEDIA  = 0x0;
    prev18_r19.XO_RP_MVT    = 0x1;
    prev18_r19.XO_RP_SVT    = 0x0;
 
    
    prev18_r19.XO_SLEEP = 0x0;
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(100);
    prev18_r19.XO_ISOLATE = 0x0;
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(100);
    prev18_r19.XO_DRV_START_UP  = 0x1;// 1: enables start-up circuit
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(10000);
    prev18_r19.XO_SCN_CLK_SEL   = 0x1;// scn clock 1: normal. 0.3V level up to 0.6V, 0:init
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(10000);
    prev18_r19.XO_SCN_CLK_SEL   = 0x0;
    prev18_r19.XO_SCN_ENB       = 0x0;// enable_bar of scn
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(10000);
    prev18_r19.XO_DRV_START_UP  = 0x0;
    prev18_r19.XO_DRV_CORE      = 0x1;// 1: enables core circuit
    prev18_r19.XO_SCN_CLK_SEL   = 0x1;
    *REG_XO_CONF1 = prev18_r19.as_int;
}

static void XOT_init(void){
  mbus_write_message32(0xA0,0x6);
  *XOT_RESET_CNT = 0x1;
}

static void operation_init(void){
  // Set CPU & Mbus Clock Speeds
  prev18_r0B.CLK_GEN_RING           = 0x1; // Default 0x1
  prev18_r0B.CLK_GEN_DIV_MBC        = 0x1; // Default 0x1
  prev18_r0B.CLK_GEN_DIV_CORE       = 0x3; // Default 0x3
  *REG_CLKGEN_TUNE = prev18_r0B.as_int;
  
  // Enumerate
  set_halt_until_mbus_rx();
  mbus_enumerate(PMU_ADDR);
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

  if (enumerated != 0xDEADBEEF){
    operation_init();
  }
  
  set_halt_until_mbus_tx();
  XO_init();
  XOT_init();
  delay(10000);
  operation_sleep_notimer(); 
  

  while(1){
    delay(50000);
    mbus_write_message32(0xAA,*XOT_VAL);
    //mbus_write_message32(0xAB,*REG_XOT_VAL_L);
    //mbus_write_message32(0xAC,*REG_XOT_VAL_U);
  }
  
  while(1){  //Never Quit (should not come here.)
    mbus_write_message32(0xF0,0xDEADBEEF);
    operation_sleep_notimer();
    asm("nop;"); 
  }
  
  return 1;
}

