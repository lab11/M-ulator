//****************************************************************************************************
//Author:       Hyeongseok Kim
//              Gyouho Kim, ZhiYoong Foo
//Description:  Derived from Pstack_ondemand_v1.8.c
//****************************************************************************************************
#include "PREv12.h"
#include "AFEv1.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define COR_ADDR    0x4
#define AFE_ADDR    0x5

//***************************************************
// Global variables
//***************************************************
//Test Declerations
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO
static uint32_t enumerated;

volatile afev1_r0_t afev1_r0 = AFEv1_R0_DEFAULT;
volatile afev1_r1_t afev1_r1 = AFEv1_R1_DEFAULT;
volatile afev1_r2_t afev1_r2 = AFEv1_R2_DEFAULT;
volatile afev1_r3_t afev1_r3 = AFEv1_R3_DEFAULT;
volatile afev1_r4_t afev1_r4 = AFEv1_R4_DEFAULT;
volatile afev1_r5_t afev1_r5 = AFEv1_R5_DEFAULT;
volatile afev1_r6_t afev1_r6 = AFEv1_R6_DEFAULT;
volatile afev1_r7_t afev1_r7 = AFEv1_R7_DEFAULT;
volatile afev1_r8_t afev1_r8 = AFEv1_R8_DEFAULT;
volatile afev1_r9_t afev1_r9 = AFEv1_R9_DEFAULT;
volatile afev1_rA_t afev1_rA = AFEv1_RA_DEFAULT;
volatile afev1_rB_t afev1_rB = AFEv1_RB_DEFAULT;
volatile afev1_rC_t afev1_rC = AFEv1_RC_DEFAULT;
volatile afev1_rD_t afev1_rD = AFEv1_RD_DEFAULT;

//***************************************************
//Interrupt Handlers
//Must clear pending bit!
//***************************************************
void init_interrupt (void) {
	*NVIC_ICPR = 0x1FFFF; //Clear All Pending Interrupts
	*NVIC_ISER = 0x0;     //Disable Interrupts
}

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
void handler_ext_int_15(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_16(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0(void)  {/*TIMER32*/  *NVIC_ICPR = (0x1 <<  0); }
void handler_ext_int_1(void)  {/*TIMER16*/  *NVIC_ICPR = (0x1 <<  1); }
void handler_ext_int_2(void)  {/*REG0*/     *NVIC_ICPR = (0x1 <<  2); }
void handler_ext_int_3(void)  {/*REG1*/     *NVIC_ICPR = (0x1 <<  3); }
void handler_ext_int_4(void)  {/*REG2*/     *NVIC_ICPR = (0x1 <<  4); }
void handler_ext_int_5(void)  {/*REG3*/     *NVIC_ICPR = (0x1 <<  5); }
void handler_ext_int_6(void)  {/*REG4*/     *NVIC_ICPR = (0x1 <<  6); }
void handler_ext_int_7(void)  {/*REG5*/     *NVIC_ICPR = (0x1 <<  7); }
void handler_ext_int_8(void)  {/*REG6*/     *NVIC_ICPR = (0x1 <<  8); }
void handler_ext_int_9(void)  {/*REG7*/     *NVIC_ICPR = (0x1 <<  9); }
void handler_ext_int_10(void) {/*MEM_WR*/   *NVIC_ICPR = (0x1 << 10); }
void handler_ext_int_11(void) {/*MBUS_RX*/  *NVIC_ICPR = (0x1 << 11); }
void handler_ext_int_12(void) {/*MBUS_TX*/  *NVIC_ICPR = (0x1 << 12); }
void handler_ext_int_13(void) {/*MBUS_FWD*/ *NVIC_ICPR = (0x1 << 13); }
void handler_ext_int_14(void) {/*GOCEP*/    *NVIC_ICPR = (0x1 << 14); }
void handler_ext_int_15(void) {/*SPI*/      *NVIC_ICPR = (0x1 << 15); }
void handler_ext_int_16(void) {/*GPIO*/     *NVIC_ICPR = (0x1 << 16); }

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
static void initialization(void){
  
    //Enumeration
    enumerated = 0xDEADBEEF;

    //Chip ID
    write_regfile (REG_CHIP_ID, 0xDEAD);

    // Notify the start of the program
    mbus_write_message32(0xAA, 0xAAAAAAAA);

    //Set Halt
    set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(COR_ADDR);
    delay(1000);
    mbus_enumerate(AFE_ADDR);

    //Set Halt
    set_halt_until_mbus_tx();

    //Enable DSP
    afev1_rB.DSP_EN = 1;
    afev1_rB.ADC_EN = 1;
    mbus_remote_register_write(AFE_ADDR,0x0B,afev1_rB.as_int);
    
    afev1_rB.DSP_EN = 1;
    afev1_rB.ADC_EN = 1;
    mbus_remote_register_write(AFE_ADDR,0x0B,afev1_rB.as_int);
    
    //DSP Reset
    afev1_rA.DSP_RESET_N = 0;
    mbus_remote_register_write(AFE_ADDR,0x0A,afev1_rA.as_int);
    
    delay(20000);
    //Release DSP Reset
    afev1_rA.DSP_RESET_N = 1;
    mbus_remote_register_write(AFE_ADDR,0x0A,afev1_rA.as_int);
    
    //Set DSP configuation - external clk on
    afev1_rA.DSP_CONFIG = 0x16A15A;
    mbus_remote_register_write(AFE_ADDR,0x0A,afev1_rA.as_int);
    afev1_rA.DSP_CONFIG = 0x16A15A;
    mbus_remote_register_write(AFE_ADDR,0x0A,afev1_rA.as_int);
    delay(10000);
    
    //PLL Tuning
    afev1_r7.PLL_TRI_7_8_EN = 1;
    afev1_r7.PLL_TRI_CK_EN = 0; // Not working,
    afev1_r7.PLL_TRI_MODE = 0; //1:Tri state(24MHz), 0:L1 band(2.4MHz)
    afev1_r7.PLL_L1_CK_EN = 1; //0:Tri state, 1:L1 band   , Not working
    afev1_r7.PLL_CK_DIV = 0;
    afev1_r7.PLL_REFDIV_EN = 1; //Not working
    afev1_r7.PLL_VCO_TRIM = 3;  //3b,
    afev1_r7.PLL_FBDIV_EN =1;
    afev1_r7.PLL_VCO_EN =1;
    mbus_remote_register_write(AFE_ADDR,7,afev1_r7.as_int);
    delay(10000);

    afev1_r8.PLL_MISC =0x07;
    afev1_r8.PLL_PFD_EN =1;
    afev1_r8.PLL_CP_EN =1;
    afev1_r8.PLL_IREF_EN =1;
    afev1_r8.PLL_CP_IMISM_TRIM =0x7;
    afev1_r8.PLL_VCO_IBIAS_TRIM =0x7;
    afev1_r8.PLL_CP_IBIAS_TRIM =0x7;
    mbus_remote_register_write(AFE_ADDR,8,afev1_r8.as_int);
    delay(10000);

    afev1_r9.PLL_PFD_DEL_TRIM =0x00F7;
    mbus_remote_register_write(AFE_ADDR,9,afev1_r9.as_int);
    delay(10000);
    
    //Rx Tuning	
    afev1_r5.RX_GA2_b2 =  0xF; 
    afev1_r5.RX_GA2_b1 =  0xF;
    afev1_r5.RX_GA2_b0 =  0x6;	
    afev1_r5.RX_GA2_EN =  1;	// VGA Enalber, Active high
    afev1_r5.RX_BETA_b =  0x0; //BETA_b is tuning bits for DAC_ANT
    afev1_r5.RX_BETA_EN =  1;  //Enabler for BETA multiplier 
    mbus_remote_register_write(AFE_ADDR,5,afev1_r5.as_int);
    delay(10000);
   
    afev1_r6.RX_BGR_b0 =0x4;
    afev1_r6.RX_BGR_b1 =0x2;
    afev1_r6.RX_BGR_b2 =0x1; 
    afev1_r6.RX_BGR_b3 =0x0;

    mbus_remote_register_write(AFE_ADDR,6,afev1_r6.as_int);
    delay(10000);	

    afev1_r1.RX_LNA_EN =  1;
    afev1_r1.RX_LNA_b =  0x3;
    afev1_r1.RX_FLTR1_b0 =  0x0;
    afev1_r1.RX_FLTR1_b1 =  0x0;
    afev1_r1.RX_FLTR1_BYPASS =  0; //EN1 Enable at GMC, Active low
    afev1_r1.RX_GA1_EN = 0; //EN0 Enabler at GMC, Active low
    afev1_r1.RX_GA1_b = 1; //4b, Tuning bits for VR, VC
    afev1_r1.RX_FLTR2_EN = 1; //GMC BQ, Active high
    afev1_r1.RX_FLTR2_SEL = 1;  //2b, 1: band pass, 2: low pass at GMC filter
    mbus_remote_register_write(AFE_ADDR,1,afev1_r1.as_int);
    delay(10000);

	//Band pass mode
    afev1_r2.RX_FLTR2_BQ1_b3 =	0x0;
    afev1_r2.RX_FLTR2_BQ1_b2 =	0xE;
    afev1_r2.RX_FLTR2_BQ1_b1 =	0x0;
    afev1_r2.RX_FLTR2_BQ1_b0 =	0x4;

    mbus_remote_register_write(AFE_ADDR,2,afev1_r2.as_int);
    delay(10000);

    afev1_r3.RX_FLTR2_BQ2_b3 =	0x0;
    afev1_r3.RX_FLTR2_BQ2_b2 =	0xF;
    afev1_r3.RX_FLTR2_BQ2_b1 =	0x0;
    afev1_r3.RX_FLTR2_BQ2_b0 =	0xF;

    mbus_remote_register_write(AFE_ADDR,3,afev1_r3.as_int);
    delay(10000);

    afev1_r4.RX_FLTR2_BQ_b4 =	0x2; //3bit BQ cap tuning    
    afev1_r4.RX_FLTR2_BQ3_b3 =	0xF;
    afev1_r4.RX_FLTR2_BQ3_b2 =	0xF;
    afev1_r4.RX_FLTR2_BQ3_b1 =	0x0;
    afev1_r4.RX_FLTR2_BQ3_b0 =	0xF;

    mbus_remote_register_write(AFE_ADDR,4,afev1_r4.as_int);
    delay(10000);
	

    //Low pass mode
    /*
      afev1_r2.RX_FLTR2_BQ1_b3 =	0x3;
      afev1_r2.RX_FLTR2_BQ1_b2 =	0x4;
      afev1_r2.RX_FLTR2_BQ1_b1 =	0x0;
      
      afev1_r2.RX_FLTR2_BQ1_b0 =	0xF;
      
      mbus_remote_register_write(AFE_ADDR,2,afev1_r2.as_int);
      delay(10000);
      
      afev1_r3.RX_FLTR2_BQ2_b3 =	0x3;
      afev1_r3.RX_FLTR2_BQ2_b2 =	0x4;
      afev1_r3.RX_FLTR2_BQ2_b1 =	0x0;
      afev1_r3.RX_FLTR2_BQ2_b0 =	0xF;
      
      mbus_remote_register_write(AFE_ADDR,3,afev1_r3.as_int);
      delay(10000);
      
      afev1_r4.RX_FLTR2_BQ_b4 =	0x1; //3bit BQ cap tuning    
      afev1_r4.RX_FLTR2_BQ3_b3 =	0x3;
      afev1_r4.RX_FLTR2_BQ3_b2 =	0x4;
      afev1_r4.RX_FLTR2_BQ3_b1 =	0x1;
      afev1_r4.RX_FLTR2_BQ3_b0 =	0xF;
      
      mbus_remote_register_write(AFE_ADDR,4,afev1_r4.as_int);
      delay(10000);
    */
    
    //ADC test
    afev1_rB.ADC_COMP3_T2_I =	0x7;
    afev1_rB.ADC_COMP3_T1_I =	0;
    afev1_rB.ADC_COMP2_T2_I =	0x0;    
    afev1_rB.ADC_COMP2_T1_I =	0x0;

    mbus_remote_register_write(AFE_ADDR,0xB,afev1_rB.as_int);

    afev1_rC.ADC_COMP1_T2_I =	0;
    afev1_rC.ADC_COMP1_T1_I =	0x7;
    afev1_rC.ADC_COMP3_T2_Q =	0x1;    
    afev1_rC.ADC_COMP3_T1_Q =	0;

    mbus_remote_register_write(AFE_ADDR,0xC,afev1_rC.as_int);
   
    afev1_rD.ADC_COMP2_T2_Q =	0;
    afev1_rD.ADC_COMP2_T1_Q =	0;
    afev1_rD.ADC_COMP1_T2_Q =	0;    
    afev1_rD.ADC_COMP1_T1_Q =	0x1;

    mbus_remote_register_write(AFE_ADDR,0xD,afev1_rD.as_int);

    //Set DSP configuation - external clk off/Internal clk on
    afev1_rA.DSP_CONFIG = 0x16A15A;
    mbus_remote_register_write(AFE_ADDR,0xA,afev1_rA.as_int);
    afev1_rA.DSP_CONFIG = 0x16A15A;
    mbus_remote_register_write(AFE_ADDR,0xA,afev1_rA.as_int);
    delay(10000);
    
}

//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
  
    //Initialize Interrupts
    init_interrupt();
  
    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
		initialization();
    }
    
    while(1);
    
    // Should not reach here
    mbus_sleep_all();
    while(1);
}

