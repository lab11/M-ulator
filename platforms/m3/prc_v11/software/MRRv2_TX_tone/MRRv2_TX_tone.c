//****************************************************************************************************
//Author:       Li Xuan
//Description:  MRRv2 Initial Testing
//****************************************************************************************************
#include "mbus.h"
#include "PRCv9.h"
#include "MRRv2.h"


#define MRR_ADDR 0x4

// CDC parameters
#define	MBUS_DELAY 100 //Amount of delay between successive messages
#define	LDO_DELAY 500 // 1000: 150msec
#define CDC_TIMEOUT_COUNT 500
#define WAKEUP_PERIOD_RESET 2
#define WAKEUP_PERIOD_LDO 1

// Radio configurations
#define RAD_BIT_DELAY       13     //40      //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 	2000      //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define RAD_PACKET_NUM      1      //How many times identical data will be TXed

//***************************************************
// Global variables
//***************************************************
	//Test Declerations
	// "static" limits the variables to this file, giving compiler more freedom
	// "volatile" should only be used for MMIO
	static uint32_t enumerated;
	static uint32_t MBus_msg_flag;
  
	static uint32_t WAKEUP_PERIOD_CONT; 
	static uint32_t WAKEUP_PERIOD_CONT_INIT; 

	static uint32_t cdc_storage_count;
	static uint32_t radio_tx_count;
	static uint32_t radio_tx_option;
	static uint32_t radio_tx_numdata;

	volatile mrrv2_r00_t mrrv2_r00 = MRRv2_R00_DEFAULT;
	volatile mrrv2_r01_t mrrv2_r01 = MRRv2_R01_DEFAULT;
	volatile mrrv2_r02_t mrrv2_r02 = MRRv2_R02_DEFAULT;
	volatile mrrv2_r03_t mrrv2_r03 = MRRv2_R03_DEFAULT;
	volatile mrrv2_r04_t mrrv2_r04 = MRRv2_R04_DEFAULT;
	volatile mrrv2_r05_t mrrv2_r05 = MRRv2_R05_DEFAULT;
	volatile mrrv2_r06_t mrrv2_r06 = MRRv2_R06_DEFAULT;
	volatile mrrv2_r07_t mrrv2_r07 = MRRv2_R07_DEFAULT;
	volatile mrrv2_r08_t mrrv2_r08 = MRRv2_R08_DEFAULT;
	volatile mrrv2_r09_t mrrv2_r09 = MRRv2_R09_DEFAULT;
	volatile mrrv2_r0A_t mrrv2_r0A = MRRv2_R0A_DEFAULT;
	volatile mrrv2_r0B_t mrrv2_r0B = MRRv2_R0B_DEFAULT;
	volatile mrrv2_r0C_t mrrv2_r0C = MRRv2_R0C_DEFAULT;
	volatile mrrv2_r0D_t mrrv2_r0D = MRRv2_R0D_DEFAULT;
	volatile mrrv2_r0E_t mrrv2_r0E = MRRv2_R0E_DEFAULT;
	volatile mrrv2_r0F_t mrrv2_r0F = MRRv2_R0F_DEFAULT;
	volatile mrrv2_r10_t mrrv2_r10 = MRRv2_R10_DEFAULT;
	volatile mrrv2_r11_t mrrv2_r11 = MRRv2_R11_DEFAULT;
	volatile mrrv2_r12_t mrrv2_r12 = MRRv2_R12_DEFAULT;
	volatile mrrv2_r13_t mrrv2_r13 = MRRv2_R13_DEFAULT;
	volatile mrrv2_r1B_t mrrv2_r1B = MRRv2_R1B_DEFAULT;
	volatile mrrv2_r1C_t mrrv2_r1C = MRRv2_R1C_DEFAULT;


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
    MBus_msg_flag = 0x10;
}
void handler_ext_int_1(void){
    *((volatile uint32_t *) 0xE000E280) = 0x2;
    MBus_msg_flag = 0x11;
}
void handler_ext_int_2(void){
    *((volatile uint32_t *) 0xE000E280) = 0x4;
    MBus_msg_flag = 0x12;
}
void handler_ext_int_3(void){
    *((volatile uint32_t *) 0xE000E280) = 0x8;
    MBus_msg_flag = 0x13;
}

inline static void set_pmu_sleep_clk_low(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F77003B; // 0x8F77003B: use GOC x0.6-2
}
inline static void set_pmu_sleep_clk_high(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F77004B; // 0x8F77004B: use GOC x10-25
}

//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){

  // Reset wakeup counter
  // This is required to go back to sleep!!
  set_wakeup_timer (0x02, 0x1, 0x1);

  // Reset IRQ10VEC
  *((volatile uint32_t *) IRQ10VEC/*IMSG0*/) = 0;

  // Go to Sleep
  sleep();
  while(1);
}

static void operation_init(void){
  
    // Set PMU Strength & division threshold
    // Change PMU_CTRL Register
    // PRCv9 Default: 0x8F770049
    // Decrease 5x division switching threshold
    *((volatile uint32_t *) 0xA200000C) = 0x8F77004B;
  
    // Speed up GOC frontend to match PMU frequency
    // PRCv9 Default: 0x00202903
    *((volatile uint32_t *) 0xA2000008) = 0x00202508;
  
    delay(100);
  
    //Enumerate & Initialize Registers
    enumerated = 0xDEADBEEF;
    MBus_msg_flag = 0;
    //Enumeration
    enumerate(MRR_ADDR);
    delay(MBUS_DELAY*10);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 3;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 1;   // 0x1E (30): ~1 min with PRCv9
    
}


//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
  
    //Clear All Pending Interrupts
    *((volatile uint32_t *) 0xE000E280) = 0xF;
    //Enable Interrupts
    *((volatile uint32_t *) 0xE000E100) = 0xF;
  
	//Config watchdog timer to about 10 sec: 1,000,000 with default PRCv9
	//config_timer( timer_id, go, roi, init_val, sat_val )
	config_timer( 0, 0, 0, 0, 1000000 ); // Check with Gyouho

    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
        // Set up PMU/GOC register in PRC layer (every time)
        // Enumeration & RAD/SNS layer register configuration
        operation_init();
    }

	delay(10000);

	read_mbus_register(MRR_ADDR, 0x0, 0xE0);
	delay(MBUS_DELAY*10);

	delay(10000);

	read_mbus_register(MRR_ADDR, 0xE, 0xE1);
	delay(MBUS_DELAY*10);

	//delay(10000);
//    while(1){
/* 
    mrrv2_r04.MRR_SCRO_EN_TIMER = 1;  //Enable TIMER
    write_mbus_register(MRR_ADDR,0x04,mrrv2_r04.as_int);
    delay(MBUS_DELAY*10);
    mrrv2_r04.MRR_SCRO_RSTN_TIMER = 1;  //RST TIMER
    write_mbus_register(MRR_ADDR,0x04,mrrv2_r04.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x04, 0xE2);
    delay(MBUS_DELAY*10);

    delay(10000);
*/ 
    mrrv2_r03.MRR_TRX_ISOLATEN = 0x0;     //set ISOLATEN 1, let state machine control
    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x3, 0xE1);
    delay(MBUS_DELAY*10); 
//delay(100000);
/*
    mrrv2_r03.MRR_RX_EN_OW = 0x1;         //set RX on
    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x3, 0xE2);
    delay(MBUS_DELAY*10); 
//delay(100000);

    mrrv2_r03.MRR_RX_REC_RST_OW   = 0x0;  //Reset Rectifier
    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x3, 0xE3);
    delay(MBUS_DELAY*10); 
//delay(100000);

    mrrv2_r03.MRR_DCP_P_OW        = 0x1;  //set parallel decap
    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x3, 0xE4);
    delay(MBUS_DELAY*10); 
//delay(100000);

    mrrv2_r03.MRR_DCP_S_OW        = 0x0;  //set serial decap
    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x3, 0xE5);
    delay(MBUS_DELAY*10); 
//delay(100000);

    mrrv2_r03.MRR_RX_BIAS_TUNE    = 0x1FFF;// 0x1FFF; // turn on Q_enhancement
    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x3, 0xE6);
    delay(MBUS_DELAY*10); 

    mrrv2_r00.MRR_CL_EN = 1;  //Enable CL
    mrrv2_r00.MRR_CL_CTRL = 0x01 ; //Set CL
    write_mbus_register(MRR_ADDR,0x00,mrrv2_r00.as_int);
    delay(MBUS_DELAY*10);

    mrrv2_r00.MRR_TRX_CAP_ANTP_TUNE = 0x3FFF;  //ANT CAP
    write_mbus_register(MRR_ADDR,0x00,mrrv2_r00.as_int);
    delay(MBUS_DELAY*10);

    mrrv2_r01.MRR_TRX_CAP_ANTN_TUNE = 0x3FFF ; //ANT CAP
    write_mbus_register(MRR_ADDR,0x01,mrrv2_r01.as_int);
    delay(MBUS_DELAY*10);


    // Use TX bias to boost the Q
    mrrv2_r02.MRR_TX_BIAS_TUNE = 0x0001;  //Set TX BIAS TUNE
    write_mbus_register(MRR_ADDR,0x02,mrrv2_r02.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x02, 0xE7);
    delay(MBUS_DELAY*10);    

    mrrv2_r00.MRR_CL_EN = 1;  //Enable CL
    mrrv2_r00.MRR_CL_CTRL = 0x01 ; //Set CL
    write_mbus_register(MRR_ADDR,0x00,mrrv2_r00.as_int);
    delay(MBUS_DELAY*10);

    mrrv2_r02.MRR_TX_EN_OW = 1;  //Continuous TX
    write_mbus_register(MRR_ADDR,2,mrrv2_r02.as_int);
    delay(MBUS_DELAY*10);

    // End

    while(1){ //toggling RST REC
//    mrrv2_r03.MRR_RX_REC_RST_OW   = 0x1;  //Reset Rectifier
//    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
//    delay(MBUS_DELAY*10);

    mrrv2_r03.MRR_RX_REC_RST_OW   = 0x0;  //Rectifier
    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
    delay(MBUS_DELAY*10);
    }
*/
/*
    mrrv2_r03.MRR_RX_BIAS_TUNE    = 0x1FFF;// 0x1FFF; // turn on Q_enhancement
    mrrv2_r03.MRR_RX_SAMPLE_CAP    = 0x1;  // RX_SAMPLE_CAPi
    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x3, 0xE6);
    delay(MBUS_DELAY*10); 

    mrrv2_r02.MRR_TX_BIAS_TUNE = 0x0DFF;  //Set TX BIAS TUNE
    write_mbus_register(MRR_ADDR,0x02,mrrv2_r02.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x02, 0xE7);
    delay(MBUS_DELAY*10);    
*/

//    mrrv2_r11.MRR_RAD_FSM_RX_POWERON_LEN = 0x3;  //Set RX Power on length
//    mrrv2_r11.MRR_RAD_FSM_GUARD_LEN = 0x01F3; //Set TX_RX Guard length
//    write_mbus_register(MRR_ADDR,0x11,mrrv2_r11.as_int);
//    delay(MBUS_DELAY*10);
//    read_mbus_register(MRR_ADDR, 0x11, 0xE9);
//    delay(MBUS_DELAY*10); 

/*    mrrv2_r03.MRR_DCP_S_OW = 1;  //TX_Decap S (forced charge decaps)
    mrrv2_r03.MRR_DCP_P_OW = 0;  //RX_Decap P 
    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
    delay(MBUS_DELAY*10);
    mrrv2_r00.MRR_CL_EN = 1;  //Enable CL
    mrrv2_r00.MRR_CL_CTRL = 0x0A ; //Set CL
    write_mbus_register(MRR_ADDR,0x00,mrrv2_r00.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x00, 0xE5);

    mrrv2_r03.MRR_DCP_S_OW = 0;  //TX_Decap S (forced charge decaps)
    mrrv2_r03.MRR_DCP_P_OW = 1 ;  //RX_Decap P 
    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
    delay(MBUS_DELAY*10);

    mrrv2_r03.MRR_DCP_S_OW = 1;  //TX_Decap S (forced charge decaps)
    mrrv2_r03.MRR_DCP_P_OW = 0;  //RX_Decap P 
    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
    delay(MBUS_DELAY*10);
*/

//    read_mbus_register(MRR_ADDR, 0x3, 0xE7);
//    delay(100000); 
//    mrrv2_r03.MRR_DCP_S_OW = 0;  //TX_Decap S (Return to BB control)
//    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
//    delay(MBUS_DELAY*10);

//    delay(10000);

//    mrrv2_r0F.MRR_RAD_FSM_TX_PW_LEN = 0x0A0 ; // Pulse-width 4us
//    mrrv2_r0F.MRR_RAD_FSM_TX_PS_LEN = 0x0A0 ; // Pulse-spacing 4us
//    mrrv2_r0F.MRR_RAD_FSM_TX_PR_LEN = 0x0 ;   // Pulse-repetition 4us
//    write_mbus_register(MRR_ADDR,0xF,mrrv2_r0F.as_int);

/* // For pulse TX & pulse RX

    mrrv2_r11.MRR_RAD_FSM_TX_POWERON_LEN = 0x4;  //Set TX Power on length
    mrrv2_r11.MRR_RAD_FSM_RX_POWERON_LEN = 0x3;  //Set RX Power on length
    mrrv2_r11.MRR_RAD_FSM_RX_SAMPLE_LEN = 0x0;  //Set RX Sample length
    mrrv2_r11.MRR_RAD_FSM_GUARD_LEN = 0x0FFF; //Set TX_RX Guard length
    write_mbus_register(MRR_ADDR,0x11,mrrv2_r11.as_int);
    delay(MBUS_DELAY*10);

    mrrv2_r12.MRR_RAD_FSM_RX_HDR_BITS = 0x04;  //Set RX header
    mrrv2_r12.MRR_RAD_FSM_RX_HDR_TH = 0x00;    //Set RX header threshold
    mrrv2_r12.MRR_RAD_FSM_RX_DATA_BITS = 0x0C; //Set RX data
    write_mbus_register(MRR_ADDR,0x12,mrrv2_r12.as_int);
    delay(MBUS_DELAY*10);
    
//    read_mbus_register(MRR_ADDR, 0x11, 0xE9);
//    delay(MBUS_DELAY*10); 

    mrrv2_r12.MRR_EN_DIG_MONITOR = 0x1; // Enable monitor
    write_mbus_register(MRR_ADDR,0x12,mrrv2_r12.as_int);
    delay(MBUS_DELAY*10);

    mrrv2_r13.MRR_DIG_MONITOR_SEL1 = 0xA; // 0xA --> RX_COMP_OUT1     
    mrrv2_r13.MRR_DIG_MONITOR_SEL2 = 0xB; // 0xB --> RX_COMP_OUT2 0x3 --> RX_en monitor
    mrrv2_r13.MRR_DIG_MONITOR_SEL3 = 0xC; // 0xC --> REC_RST 0x2 --> TX_en monitor
    write_mbus_register(MRR_ADDR,0x13,mrrv2_r13.as_int);
    delay(MBUS_DELAY*10);
   
    mrrv2_r00.MRR_TRX_CAP_ANTP_TUNE = 0x3FFF;  //ANT CAP
    write_mbus_register(MRR_ADDR,0x00,mrrv2_r00.as_int);
    delay(MBUS_DELAY*10);

    mrrv2_r01.MRR_TRX_CAP_ANTN_TUNE = 0x3FFF ; //ANT CAP
    write_mbus_register(MRR_ADDR,0x01,mrrv2_r01.as_int);
    delay(MBUS_DELAY*10);

    mrrv2_r05.MRR_SCRO_FREQ_DIV = 0x0; // Frequency divide ratio = 8
    write_mbus_register(MRR_ADDR,0x05,mrrv2_r05.as_int);
    delay(MBUS_DELAY*10);

    mrrv2_r00.MRR_CL_EN = 1;  //Enable CL
    mrrv2_r00.MRR_CL_CTRL = 0x01 ; //Set CL
    write_mbus_register(MRR_ADDR,0x00,mrrv2_r00.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x00, 0xE5);

    mrrv2_r04.MRR_SCRO_EN_CLK = 1;  //Enable BB_CLK
    write_mbus_register(MRR_ADDR,0x04,mrrv2_r04.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x04, 0xE3);
    delay(MBUS_DELAY*10);

    mrrv2_r0E.MRR_RAD_FSM_SLEEP = 0;  // Power on BB
    write_mbus_register(MRR_ADDR,0x0E,mrrv2_r0E.as_int);
    delay(MBUS_DELAY*10);
    mrrv2_r0E.MRR_RAD_FSM_RSTN = 1;  //RST BB
    write_mbus_register(MRR_ADDR,0x0E,mrrv2_r0E.as_int);
    delay(MBUS_DELAY*10);

    mrrv2_r0E.MRR_RAD_FSM_EN = 1;  //Start BB
    write_mbus_register(MRR_ADDR,0x0E,mrrv2_r0E.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x0E, 0xE4);
    delay(MBUS_DELAY*10);

*/  // For pulse TX and pulse RX 
 
//    delay(MBUS_DELAY*10);    

//    mrrv2_r06.MRR_RAD_FSM_TX_DATA_0 = 0xFFFFFF ;   // TX Data
//    write_mbus_register(MRR_ADDR,0x06,mrrv2_r06.as_int);
//    delay(MBUS_DELAY*10);   

//    mrrv2_r10.MRR_RAD_FSM_TX_C_LEN = 0x01F3 ;   // Recharge time 10*4us
//    mrrv2_r10.MRR_RAD_FSM_TX_MODE = 0x0 ;   // TX Mode
//    write_mbus_register(MRR_ADDR,0x10,mrrv2_r10.as_int);
//    delay(MBUS_DELAY*10);   


    //Timer & State Machine run
    //mrrv2_r04.MRR_SCRO_EN_TIMER = 1;  //power on TIMER
    //write_mbus_register(MRR_ADDR,0x04,mrrv2_r04.as_int);
    //delay(MBUS_DELAY*100); //LDO stab 1s
//
  //  mrrv2_r04.MRR_SCRO_RSTN_TIMER = 1;  //UNRST TIMER
  //  write_mbus_register(MRR_ADDR,0x04,mrrv2_r04.as_int);
  //  delay(MBUS_DELAY*100); //freq stab 5s
//
  //  mrrv2_r04.MRR_SCRO_EN_CLK = 1;  //Enable clk
  //  write_mbus_register(MRR_ADDR,0x04,mrrv2_r04.as_int);
  //  delay(MBUS_DELAY*100); //freq stab 5s

    //mrrv2_r05.MRR_EN_CLK_MONITOR = 1;  //
    //write_mbus_register(MRR_ADDR,0x05,mrrv2_r05.as_int);
    //delay(MBUS_DELAY*10);

// For CONT_TX testing
    mrrv2_r00.MRR_TRX_CAP_ANTP_TUNE = 0x0FFF;  //ANT CAP(14b)
    write_mbus_register(MRR_ADDR,0x00,mrrv2_r00.as_int);
    delay(MBUS_DELAY*10);

    mrrv2_r01.MRR_TRX_CAP_ANTN_TUNE = 0x0FFF ; //ANT CAP(14b)
    write_mbus_register(MRR_ADDR,0x01,mrrv2_r01.as_int);
    delay(MBUS_DELAY*10);

    mrrv2_r02.MRR_TX_BIAS_TUNE = 0x1FFF;  //Set TX BIAS TUNE
    write_mbus_register(MRR_ADDR,0x02,mrrv2_r02.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x02, 0xE7);
    delay(MBUS_DELAY*10);    

    mrrv2_r00.MRR_CL_EN = 1;  //Enable CL
    mrrv2_r00.MRR_CL_CTRL = 0x01 ; //Set CL
    write_mbus_register(MRR_ADDR,0x00,mrrv2_r00.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x00, 0xE5);

    mrrv2_r03.MRR_DCP_S_OW = 1;  //TX_Decap S
    write_mbus_register(MRR_ADDR,3,mrrv2_r03.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x3, 0xE7);
    delay(100000); 

    mrrv2_r02.MRR_TX_EN_OW = 1;  //Continuous TX
    write_mbus_register(MRR_ADDR,2,mrrv2_r02.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x2, 0xE6);
    delay(MBUS_DELAY*10);

// For CONT_TX testing

/*
    mrrv2_r0E.MRR_RAD_FSM_SLEEP = 1;  // Power off BB
    write_mbus_register(MRR_ADDR,0x0E,mrrv2_r0E.as_int);
    delay(MBUS_DELAY*10);
    mrrv2_r0E.MRR_RAD_FSM_RSTN = 1;  //RST BB
    write_mbus_register(MRR_ADDR,0x0E,mrrv2_r0E.as_int);
    delay(MBUS_DELAY*10);
    mrrv2_r04.MRR_SCRO_EN_TIMER = 0;  //disable TIMER
    write_mbus_register(MRR_ADDR,0x04,mrrv2_r04.as_int);
    delay(MBUS_DELAY*10);
*/


//    }

    while(1);

    // Should not reach here
    operation_sleep();

    while(1);
}

