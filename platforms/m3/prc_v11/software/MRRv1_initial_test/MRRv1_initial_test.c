//****************************************************************************************************
//Author:       Li Xuan
//Description:  MRR Initial Testing
//****************************************************************************************************
#include "mbus.h"
#include "PRCv9.h"
#include "MRRv1.h"


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

	volatile mrrv1_r00_t mrrv1_r00 = MRRv1_R00_DEFAULT;
	volatile mrrv1_r01_t mrrv1_r01 = MRRv1_R01_DEFAULT;
	volatile mrrv1_r02_t mrrv1_r02 = MRRv1_R02_DEFAULT;
	volatile mrrv1_r03_t mrrv1_r03 = MRRv1_R03_DEFAULT;
	volatile mrrv1_r04_t mrrv1_r04 = MRRv1_R04_DEFAULT;
	volatile mrrv1_r05_t mrrv1_r05 = MRRv1_R05_DEFAULT;
	volatile mrrv1_r06_t mrrv1_r06 = MRRv1_R06_DEFAULT;
	volatile mrrv1_r07_t mrrv1_r07 = MRRv1_R07_DEFAULT;


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

	read_mbus_register(MRR_ADDR, 0x3, 0xE0);
	delay(MBUS_DELAY*10);

	//delay(10000);

	read_mbus_register(MRR_ADDR, 0x3, 0xE1);
	delay(MBUS_DELAY*10);

	//delay(10000);

    mrrv1_r02.MRR_EN_TIMER = 1;  //Enable TIMER
    write_mbus_register(MRR_ADDR,2,mrrv1_r02.as_int);
    delay(MBUS_DELAY*10);
    mrrv1_r02.MRR_RSTN_TIMER = 1;  //RST TIMER
    write_mbus_register(MRR_ADDR,2,mrrv1_r02.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x2, 0xE2);
    delay(MBUS_DELAY*10);

    //delay(100000);

    delay(100000);

    //mrrv1_r00.MRR_EN_TX = 1;  //Enable TX
    //mrrv1_r00.MRR_CL_CTRL = 0x0A ; //Set CL
    //write_mbus_register(MRR_ADDR,0,mrrv1_r00.as_int);
    //delay(MBUS_DELAY*10);
    //read_mbus_register(MRR_ADDR, 0x0, 0xE5);
    //delay(100000);

    mrrv1_r04.MRR_EN_CLK = 1;  //Enable BB_CLK
    //mrrv1_r04.MRR_FREQ_DIV = 0x3;  // Freq_div_by_8
    write_mbus_register(MRR_ADDR,4,mrrv1_r04.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x4, 0xE3);
    delay(MBUS_DELAY*10);

    mrrv1_r04.MRR_RSTN_BB = 1;  //RST BB
    //write_mbus_register(MRR_ADDR,4,mrrv1_r04.as_int);
    //delay(MBUS_DELAY*10);
    mrrv1_r04.MRR_START_BB = 1;  //Start BB
    write_mbus_register(MRR_ADDR,4,mrrv1_r04.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x4, 0xE4);
    delay(MBUS_DELAY*10);

    mrrv1_r00.MRR_CAP_ANTP_TUNE = 0x0000;  //ANT CAP
    write_mbus_register(MRR_ADDR,0,mrrv1_r00.as_int);
    delay(MBUS_DELAY*10);

    mrrv1_r01.MRR_CAP_ANTN_TUNE = 0x0000 ; //ANT CAP
    write_mbus_register(MRR_ADDR,1,mrrv1_r01.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x0, 0xE5);
    read_mbus_register(MRR_ADDR, 0x1, 0xE9);
    delay(10000); 

    //delay(100000); // debug point

    //mrrv1_r01.MRR_CONT_TX = 0;  //Pulse-Mode TX
    //write_mbus_register(MRR_ADDR,1,mrrv1_r01.as_int);
    //delay(MBUS_DELAY*10);
    //read_mbus_register(MRR_ADDR, 0x1, 0xE9);
    //delay(MBUS_DELAY*10);

    mrrv1_r00.MRR_EN_TX = 1;  //Enable TX
    mrrv1_r00.MRR_CL_CTRL = 0x1F ; //Set CL
    write_mbus_register(MRR_ADDR,0,mrrv1_r00.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x0, 0xE5);
    delay(100000); 


    mrrv1_r01.MRR_CONT_TX = 1;  //Continuous TX
    write_mbus_register(MRR_ADDR,1,mrrv1_r01.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x1, 0xE6);
    delay(MBUS_DELAY*10);

    mrrv1_r02.MRR_BIAS_TUNE = 0x1FFF;  //Set TX BIAS TUNE
    write_mbus_register(MRR_ADDR,2,mrrv1_r02.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x2, 0xE7);
    delay(MBUS_DELAY*10); 
    
    delay(100000);delay(100000);

   // mrrv1_r06.MRR_EN_CLK_MONITOR = 1;  //Set CLK MON
   // write_mbus_register(MRR_ADDR,6,mrrv1_r06.as_int);
   // delay(MBUS_DELAY*10);
   // read_mbus_register(MRR_ADDR, 0x6, 0xE8);
   // delay(MBUS_DELAY*10); 


    //mrrv1_r01.MRR_CONT_TX = 0;  //Pulse-Mode TX
    //write_mbus_register(MRR_ADDR,1,mrrv1_r01.as_int);
    //delay(MBUS_DELAY*10);
    //read_mbus_register(MRR_ADDR, 0x1, 0xE9);
    //delay(MBUS_DELAY*10);

    mrrv1_r05.MRR_TX_PW = 0x0F9;  //TX Pulse-width 4us
    write_mbus_register(MRR_ADDR,5,mrrv1_r05.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x5, 0xEA);
    delay(MBUS_DELAY*10);

    mrrv1_r06.MRR_TX_PS = 0x00FA0;  //TX Pulse-spacing 4 x 32 us
    write_mbus_register(MRR_ADDR,6,mrrv1_r06.as_int);
    delay(MBUS_DELAY*10);
    read_mbus_register(MRR_ADDR, 0x6, 0xEB);
    delay(MBUS_DELAY*10);

    //mrrv1_r00.MRR_EN_TX = 1;  //Enable TX
    //mrrv1_r00.MRR_CL_CTRL = 0x05 ; //Set CL
    //write_mbus_register(MRR_ADDR,0,mrrv1_r00.as_int);
    //delay(MBUS_DELAY*10);
    //read_mbus_register(MRR_ADDR, 0x0, 0xEC);

   // mrrv1_r03.MRR_DIV_SEL = 0;
   // mrrv1_r03.MRR_I_LEVEL_SELB = 0;
   // mrrv1_r03.MRR_I_TC_SEL = 0;
   // write_mbus_register(MRR_ADDR,3,mrrv1_r03.as_int);
   // delay(MBUS_DELAY*10);

   //read_mbus_register(MRR_ADDR, 0x3, 0xEB);
   //delay(MBUS_DELAY*10);

    delay(100000);

   // mrrv1_r00.MRR_EN_TX = 0;  //Disable TX
   // write_mbus_register(MRR_ADDR,0,mrrv1_r00.as_int);
   // delay(MBUS_DELAY*10);
   // read_mbus_register(MRR_ADDR, 0x0, 0xE8);
   // delay(MBUS_DELAY*10);


	while(1);

    // Should not reach here
    operation_sleep();

    while(1);
}

