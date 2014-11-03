//*******************************************************************
//Author: ZhiYoong Foo
//        Gyouho Kim
//        Derived from hyeonseok_FFRv1.c under prc_v8
//Description: FFRv1 Functionality Tests
//*******************************************************************
#include "mbus.h"
#include "PRCv9.h"
#include "FFRv1.h"

#define FFR_ADDR 0x4
#define WAKEUP_PERIOD 1

//***************************************************
// Global variables
//***************************************************
//Test Declerations
volatile uint32_t enumerated;


//***************************************************
//Interrupt Handlers
//Must clear pending bit!
//***************************************************
void handler_ext_int_0(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_0(void){
  *((volatile uint32_t *) 0xE000E280) = 0x1;
}
void handler_ext_int_1(void){
  *((volatile uint32_t *) 0xE000E280) = 0x2;
}
void handler_ext_int_2(void){
  *((volatile uint32_t *) 0xE000E280) = 0x4;
}
void handler_ext_int_3(void){
  *((volatile uint32_t *) 0xE000E280) = 0x8;
}

int main() {
  
  //FFRv1 Register Declerations
  ffrv1_r0_t ffrv1_r0 = FFRv1_R0_DEFAULT;
  ffrv1_r1_t ffrv1_r1 = FFRv1_R1_DEFAULT;
  ffrv1_r2_t ffrv1_r2 = FFRv1_R2_DEFAULT;
  ffrv1_r3_t ffrv1_r3 = FFRv1_R3_DEFAULT;
  ffrv1_r4_t ffrv1_r4 = FFRv1_R4_DEFAULT;
  
  //Clear All Pending Interrupts
  *((volatile uint32_t *) 0xE000E280) = 0xF;
  //Enable Interrupts
  *((volatile uint32_t *) 0xE000E100) = 0xF;


  if (enumerated != 0xDEADBEEF){
    //delay(1000);
    //Enumeration
    enumerate(FFR_ADDR);
    //asm ("wfi;");
    enumerated = 0xDEADBEEF;
  
    ///////////////
    //Start Testing
    ///////////////
    
    // Set PMU Strength & division threshold
    // Change PMU_CTRL Register
    // 0x8F770089 = Original
    // Increase active oscillator freq
    //*((volatile uint32_t *) 0xA200000C) = 0x8F772049;

    delay(1000); 


    //FFRv1 R0
    ffrv1_r0.TX_VCO_b = 0x70; //Tuning at 10GHz
    ffrv1_r0.TX_pulse_b = 0xF ; //max pulse width
    write_mbus_register(FFR_ADDR,0,ffrv1_r0.as_int);
  
    //FFRv1 R1
    ffrv1_r1.TX_FSM_TX_DATA = 0xFFFFFF;
    write_mbus_register(FFR_ADDR,1,ffrv1_r1.as_int);
    
    //FFRv1 R2
    ffrv1_r2.TX_FSM_N_HEADER = 0xF ; //Send 16 "1" headers
    write_mbus_register(FFR_ADDR,2,ffrv1_r2.as_int);
    
    //FFRv1 R3
    ffrv1_r3.I_LIMIT_IL_EN = 0x1;		//Disable Limiter, should set to 1
    ffrv1_r3.I_LIMIT_IL_iTune = 0x3F;	//Turn on all tail current FETs for amplifier, No effect on the output
    ffrv1_r3.I_LIMIT_IL_bypass = 0x0;	//Bypass all Resistors -> Increase the output power
    write_mbus_register(FFR_ADDR,3,ffrv1_r3.as_int);
    
    //FFRv1 R4
    ffrv1_r4.CLKGEN_RINGSEL = 0x0;
    write_mbus_register(FFR_ADDR,1,ffrv1_r4.as_int);
  
  }

  ffrv1_r2.TX_FSM_START = 0;
  write_mbus_register(FFR_ADDR,2,ffrv1_r2.as_int);
  ffrv1_r2.TX_FSM_START = 1;
  write_mbus_register(FFR_ADDR,2,ffrv1_r2.as_int);
  read_mbus_register(FFR_ADDR,2, 0x13);
  delay(3000); 
  //   read_mbus_register(FFR_ADDR,2, 0x13);
  //   delay(10000);

  // Go to sleep
  set_wakeup_timer(WAKEUP_PERIOD, 0x1, 0x1);
  sleep();
  
  /////////////// 
  //End Testing
  ///////////////

  //Never Ends
  while (1){}
}
