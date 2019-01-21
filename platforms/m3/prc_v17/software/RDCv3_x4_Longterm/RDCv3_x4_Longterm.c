//****************************************************************************************************
//Author:       Seokhyoen Jeong
//Description:  temperature sensor test
//				- PRCv17 / PMUv4 / SNSv10
//****************************************************************************************************
#include "PRCv17.h"
#include "PRCv17_RF.h"
#include "mbus.h"
#include "RDCv3_RF.h"

// uncomment this for debug mbus message
#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// uncomment this to only transmit average
//#define TX_AVERAGE

// Stack order  PRC->SNS->PMU
volatile uint32_t RDC_ADDR;
volatile uint32_t RDC_WRITE_ADDR;
//#define PMU_ADDR 0x6

// Temp Sensor parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)
#define TEMP_CYCLE_INIT 3 

// Pstack states
#define	PSTK_IDLE 0x0

#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)



//***************************************************
// Global variables
//***************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t Tstack_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint32_t wakeup_period_count;
volatile uint32_t wakeup_timer_multiplier;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile prcv17_r0B_t prcv17_r0B = PRCv17_R0B_DEFAULT;

volatile uint32_t rdc_output;
volatile uint32_t n_iter;

volatile rdcv3_r20_t rdcv3_r20 = RDCv3_R20_DEFAULT;
volatile rdcv3_r21_t rdcv3_r21 = RDCv3_R21_DEFAULT;
volatile rdcv3_r22_t rdcv3_r22 = RDCv3_R22_DEFAULT;
volatile rdcv3_r23_t rdcv3_r23 = RDCv3_R23_DEFAULT;
volatile rdcv3_r24_t rdcv3_r24 = RDCv3_R24_DEFAULT;
volatile rdcv3_r25_t rdcv3_r25 = RDCv3_R25_DEFAULT;
volatile rdcv3_r26_t rdcv3_r26 = RDCv3_R26_DEFAULT;
volatile rdcv3_r27_t rdcv3_r27 = RDCv3_R27_DEFAULT;
volatile rdcv3_r28_t rdcv3_r28 = RDCv3_R28_DEFAULT;
volatile rdcv3_r29_t rdcv3_r29 = RDCv3_R29_DEFAULT;

volatile rdcv3_r2A_t rdcv3_r2A = RDCv3_R2A_DEFAULT;
volatile rdcv3_r2B_t rdcv3_r2B = RDCv3_R2B_DEFAULT;
volatile rdcv3_r2C_t rdcv3_r2C = RDCv3_R2C_DEFAULT;


//*******************************************************************
// INTERRUPT HANDLERS (Updated for PRCv17)
//*******************************************************************

void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
	wfi_timeout_flag = 1;
    }
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);
}
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1);
}
void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2);
}
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3);
}
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
    *SREG_WAKEUP_SOURCE = 0;
}

//***************************************************
// RDCv3 Functions
//***************************************************

static void rdc_enable_vref(){
	rdcv3_r2C.RDC_ENb_PG_VREF = 1;
	mbus_remote_register_write(RDC_ADDR,1,rdcv3_r2C.as_int);
}
static void rdc_disable_vref(){
	rdcv3_r2C.RDC_ENb_PG_VREF = 0;
	mbus_remote_register_write(RDC_ADDR,1,rdcv3_r2C.as_int);
}

static void rdc_disable_pg_v1p2(){
	rdcv3_r2C.RDC_EN_PG_FSM = 0;
	rdcv3_r2C.RDC_EN_PG_RC_OSC = 0;
	rdcv3_r2C.RDC_EN_PG_AMP_V1P2 = 0;
	rdcv3_r2C.RDC_EN_PG_ADC_V1P2 = 0;
	rdcv3_r2C.RDC_EN_PG_BUF_VH_V1P2 = 0;
	mbus_remote_register_write(RDC_ADDR,1,rdcv3_r2C.as_int);
}
static void rdc_enable_pg_v1p2(){
	rdcv3_r2C.RDC_EN_PG_FSM = 1;
	rdcv3_r2C.RDC_EN_PG_RC_OSC = 1;
	rdcv3_r2C.RDC_EN_PG_AMP_V1P2 = 1;
	rdcv3_r2C.RDC_EN_PG_ADC_V1P2 = 1;
	rdcv3_r2C.RDC_EN_PG_BUF_VH_V1P2 = 1;
	mbus_remote_register_write(RDC_ADDR,1,rdcv3_r2C.as_int);
}

static void rdc_disable_pg_vbat(){
	rdcv3_r2C.RDC_ENb_PG_AMP_VBAT = 1;
	rdcv3_r2C.RDC_ENb_PG_ADC_VBAT = 1;
	rdcv3_r2C.RDC_ENb_PG_BUF_VCM = 1;
	rdcv3_r2C.RDC_ENb_PG_BUF_VH_VBAT = 1;
	rdcv3_r2C.RDC_ENb_MIRROR_LDO = 0;
	mbus_remote_register_write(RDC_ADDR,1,rdcv3_r2C.as_int);
}
static void rdc_enable_pg_vbat(){
	rdcv3_r2C.RDC_ENb_PG_AMP_VBAT = 0;
	rdcv3_r2C.RDC_ENb_PG_ADC_VBAT = 0;
	rdcv3_r2C.RDC_ENb_PG_BUF_VCM = 0;
	rdcv3_r2C.RDC_ENb_PG_BUF_VH_VBAT = 0;
	rdcv3_r2C.RDC_ENb_MIRROR_LDO = 1;
	mbus_remote_register_write(RDC_ADDR,1,rdcv3_r2C.as_int);
}

static void rdc_disable_clock(){
	rdcv3_r28.RDC_RESET_RC_OSC = 1;
	rdcv3_r2B.RDC_CLK_ISOLATE = 1;
	mbus_remote_register_write(RDC_ADDR,1,rdcv3_r28.as_int);
	mbus_remote_register_write(RDC_ADDR,1,rdcv3_r2B.as_int);
}
static void rdc_enable_clock(){
	rdcv3_r28.RDC_RESET_RC_OSC = 0;
	rdcv3_r2B.RDC_CLK_ISOLATE = 0;
	mbus_remote_register_write(RDC_ADDR,1,rdcv3_r28.as_int);
	mbus_remote_register_write(RDC_ADDR,1,rdcv3_r2B.as_int);
}

static void rdc_start_meas(){
	rdcv3_r20.RDC_RESETn_FSM = 1;
	rdcv3_r20.RDC_ISOLATE = 0;
	mbus_remote_register_write(RDC_ADDR,1,rdcv3_r20.as_int);
}
static void rdc_reset(){
	rdcv3_r20.RDC_RESETn_FSM = 0;
	rdcv3_r20.RDC_ISOLATE = 1;
	mbus_remote_register_write(RDC_ADDR,1,rdcv3_r20.as_int);
}

//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){

	// Reset GOC_DATA_IRQ
	*GOC_DATA_IRQ = 0;

    // Go to Sleep
    mbus_sleep_all();
    while(1);

}

static void operation_sleep_noirqreset(void){

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


static void operation_init(void){
  
	// Set CPU & Mbus Clock Speeds
    prcv17_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prcv17_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv17_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
    prcv17_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
    prcv17_r0B.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x6
	*REG_CLKGEN_TUNE = prcv17_r0B.as_int;

    // Disable MBus Watchdog Timer
    //*REG_MBUS_WD = 0;
	*((volatile uint32_t *) 0xA000007C) = 0;

  
    //Enumerate & Initialize Registers
    //Pstack_state = PSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEE0;
    exec_count = 0;
    exec_count_irq = 0;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    //Enumeration
    RDC_ADDR = 4;
    mbus_enumerate(RDC_ADDR);
	delay(MBUS_DELAY);
    RDC_ADDR = 5;
    mbus_enumerate(RDC_ADDR);
	delay(MBUS_DELAY);
    RDC_ADDR = 6;
    mbus_enumerate(RDC_ADDR);
	delay(MBUS_DELAY);
    RDC_ADDR = 7;
    mbus_enumerate(RDC_ADDR);
	delay(MBUS_DELAY);

    // Set CPU Halt Option as TX --> Use for register write e.g.
	//set_halt_until_mbus_tx();

    ////////////////////////////////////////////////////////
    // RDCv3 Settings --------------------------------------
    // Common settings
        // rdcv3_r20
        rdcv3_r20.RDC_CNT_AMP1 = 0xC;
        
        // rdcv3_r21
        rdcv3_r21.RDC_CNT_AMP2 = 0x8;
        
        // rdcv3_r22
        rdcv3_r22.RDC_CNT_SKIP = 0x3;
        rdcv3_r22.RDC_OSR = 0x4;
        
        // rdcv3_r24 & r25
        rdcv3_r24.RDC_SEL_DLY = 0xA;

    // Individual settings
        // 1st chip
        RDC_ADDR = 4;
        rdcv3_r24.RDC_SEL_GAIN_LC = 0x7;
        rdcv3_r25.RDC_OFFSET_P_LC = 0xA;
        rdcv3_r25.RDC_OFFSET_PB_LC = 0xA;
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r20.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r21.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r22.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r24.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r25.as_int);
        
        // 2nd chip
        RDC_ADDR = 5;
        rdcv3_r24.RDC_SEL_GAIN_LC = 0x7;
        rdcv3_r25.RDC_OFFSET_P_LC = 0xA;
        rdcv3_r25.RDC_OFFSET_PB_LC = 0xA;
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r20.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r21.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r22.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r24.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r25.as_int);
        
        // 3rd chip
        RDC_ADDR = 6;
        rdcv3_r24.RDC_SEL_GAIN_LC = 0x7;
        rdcv3_r25.RDC_OFFSET_P_LC = 0xA;
        rdcv3_r25.RDC_OFFSET_PB_LC = 0xA;
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r20.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r21.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r22.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r24.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r25.as_int);
        
        // 4th chip
        RDC_ADDR = 7;
        rdcv3_r24.RDC_SEL_GAIN_LC = 0x7;
        rdcv3_r25.RDC_OFFSET_P_LC = 0xA;
        rdcv3_r25.RDC_OFFSET_PB_LC = 0xA;
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r20.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r21.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r22.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r24.as_int);
        mbus_remote_register_write(RDC_ADDR,1,rdcv3_r25.as_int);

    ////////////////////////////////////////////////////////
    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 100;   // 10: 2-4 sec with PRCv17
	wakeup_data = 0;

    delay(MBUS_DELAY);

    // Go to sleep
	//set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
    //operation_sleep();
}


//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
  
    // Initialize Interrupts
    // Only enable register-related interrupts
	//enable_reg_irq();
	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);
  
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    //config_timerwd(0xFFFFF); // 0xFFFFF about 13 sec with Y2 run default clock
	disable_timerwd();

    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
        // Set up PMU/GOC register in PRC layer (every time)
        // Enumeration & RAD/SNS layer register configuration
        operation_init();

    }

	uint32_t ii;

    // Proceed to continuous mode
    while(1){
        for(ii=0; ii<4; ii++){
            // Address change
            RDC_ADDR = ii+4;

            // Initialize
            rdc_enable_vref();  
            delay(MBUS_DELAY*10);

            rdc_enable_pg_v1p2();
            rdc_enable_pg_vbat();
            delay(MBUS_DELAY);

            rdc_enable_clock();
            delay(MBUS_DELAY);

	        // Use Timer32 as timeout counter
		    wfi_timeout_flag = 0;
	        config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

            // Start measure
            rdc_start_meas();
        
            // Wait for output
        	WFI();
       
	        // Turn off Timer32
	        *TIMER32_GO = 0;

            // Grab data
            if(wfi_timeout_flag){
			    mbus_write_message32(0xAA, 0);
                mbus_write_message32(0xBB, n_iter);
                operation_init();
                ii--;
            }
            else{
			    set_halt_until_mbus_rx();
			    mbus_remote_register_read(RDC_ADDR,0x11,0);
                rdc_output = *REG0;
			    set_halt_until_mbus_tx();
			    mbus_write_message32(0xAA, 1);
                mbus_write_message32(0xBB, n_iter);
                RDC_WRITE_ADDR = 0xC0 + ii;
        	    mbus_write_message32(RDC_WRITE_ADDR, rdc_output);
                n_iter++;
                // Reset everything
                rdc_reset();
                rdc_disable_clock();
                rdc_disable_pg_vbat();
                rdc_disable_pg_v1p2();
                rdc_disable_vref();  
            }
        }
        delay(1000);
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

