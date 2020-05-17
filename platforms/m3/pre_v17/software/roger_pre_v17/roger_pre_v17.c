//****************************************************************************************************
//Author:       Roger Hsiao
//Description:  temperature sensor test
//				- PREv17 / PMUv4 / SNSv10
//****************************************************************************************************
#include "PREv17.h"
#include "mbus.h"
#include "SNSv11_RF.h"

// uncomment this for debug mbus message
#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// uncomment this to only transmit average
//#define TX_AVERAGE

// Stack order  PRC->SNS->PMU
#define SNS_ADDR 0x4
//#define PMU_ADDR 0x6

// Temp Sensor parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)
#define TEMP_CYCLE_INIT 3 

// CP parameter
#define CP_DELAY 50000 // Amount of delay between successive messages; 100: 6-7ms

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

// Tstack states
#define	TSTK_IDLE       0x0
#define	TSTK_LDO        0x1
#define TSTK_TEMP_START 0x2
#define TSTK_TEMP_READ  0x6

#define NUM_TEMP_MEAS 2

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
volatile uint32_t PMU_ADC_4P2_VAL;
volatile uint32_t pmu_parkinglot_mode;
volatile uint32_t pmu_harvesting_on;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile prev17_r0B_t prev17_r0B = PREv17_R0B_DEFAULT;
volatile prev17_r0D_t prev17_r0D = PREv17_R0D_DEFAULT;

volatile snsv11_r00_t snsv11_r00 = SNSv11_R00_DEFAULT;
volatile snsv11_r01_t snsv11_r01 = SNSv11_R01_DEFAULT;
volatile snsv11_r03_t snsv11_r03 = SNSv11_R03_DEFAULT;
volatile snsv11_r17_t snsv11_r17 = SNSv11_R17_DEFAULT;

//*******************************************************************
// XO Functions
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
	mbus_write_message32(0xA1,*REG_XO_CONTROL);
}


static void XO_div(uint32_t divVal) {
	uint32_t xo_cap_drv = 0x3F; // Additional Cap on OSC_DRV
	uint32_t xo_cap_in  = 0x3F; // Additional Cap on OSC_IN
	*REG_XO_CONFIG = ((divVal << 16) |
			(xo_cap_drv      << 6) |
			(xo_cap_in       << 0));
}

static void XO_init(void) {

	// XO_CLK Output Pad (0: Disabled, 1: 32kHz, 2: 16kHz, 3: 8kHz)
	uint32_t xot_clk_out_sel = 0x1;
	// Parasitic Capacitance Tuning (6-bit for each; Each 1 adds 1.8pF)
	uint32_t xo_cap_drv = 0x3F; // Additional Cap on OSC_DRV
	uint32_t xo_cap_in  = 0x3F; // Additional Cap on OSC_IN

	// Pulse Length Selection
	uint32_t xo_pulse_sel = 0x4; // XO_PULSE_SEL
	uint32_t xo_delay_en  = 0x3; // XO_DELAY_EN

	// Pseudo-Resistor Selection
	uint32_t xo_rp_low   = 0x0;
	uint32_t xo_rp_media = 0x0;
	uint32_t xo_rp_mvt   = 0x1;
	uint32_t xo_rp_svt   = 0x0;

	// Parasitic Capacitance Tuning
	*REG_XO_CONFIG = ((xot_clk_out_sel << 16) |
			(xo_cap_drv      << 6) |
			(xo_cap_in       << 0));

	// Start XO Clock
	//XO_ctrl(xo_pulse_sel, xo_delay_en, xo_drv_start_up, xo_drv_core, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, xo_scn_clk_sel, xo_scn_enb);
	//XO_ctrl(xo_pulse_sel, xo_delay_en, 0, 0, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 0, 1); delay(10000); //Default
	XO_ctrl(xo_pulse_sel, xo_delay_en, 1, 0, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 0, 1); delay(10000); //XO_DRV_START_UP = 1
	XO_ctrl(xo_pulse_sel, xo_delay_en, 1, 0, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 1, 1); delay(10000); //XO_SCN_CLK_SEL = 1
	XO_ctrl(xo_pulse_sel, xo_delay_en, 1, 0, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 0, 0); delay(10000); //XO_SCN_CLK_SEL = 0 & XO_SCN_ENB = 0
	XO_ctrl(xo_pulse_sel, xo_delay_en, 0, 1, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 1, 0); delay(10000); //XO_DRV_START_UP = 0 & XO_DRV_CORE = 1
	//XO_ctrl(xo_pulse_sel, xo_delay_en, 1, 0, xo_rp_low, xo_rp_media, xo_rp_mvt, xo_rp_svt, 0, 0); delay(10000); //XO_DRV_START_UP = 1

}
static void XOT_init(void){
	mbus_write_message32(0xA0,0x6);
	*XOT_RESET = 0x1;
	mbus_write_message32(0xA0,0x7);

	mbus_write_message32(0xA0,*REG_XOT_CONFIG);
	*REG_XOT_CONFIG = (1 << 16);
	mbus_write_message32(0xAB,*REG_XOT_CONFIG);
}

//
//static void XO_init(void) {
//    // Parasitic Capacitance Tuning (6-bit for each; Each 1 adds 1.8pF)
//    uint32_t xo_cap_drv = 0x3F; // Additional Cap on OSC_DRV
//    uint32_t xo_cap_in  = 0x3F; // Additional Cap on OSC_IN
//    prev18_r1A.XO_CAP_TUNE = (
//            (xo_cap_drv <<6) | 
//            (xo_cap_in <<0));   // XO_CLK Output Pad 
//    *REG_XO_CONF2 = prev18_r1A.as_int;
//
//    // XO configuration
//    prev18_r19.XO_EN_DIV    = 0x1;// divider enable
//    prev18_r19.XO_S         = 0x1;// division ratio for 16kHz out
//    prev18_r19.XO_SEL_CP_DIV= 0x0;// 1: 0.3V-generation charge-pump uses divided clock
//    prev18_r19.XO_EN_OUT    = 0x1;// XO ouput enable
//    prev18_r19.XO_PULSE_SEL = 0x4;// pulse width sel, 1-hot code
//    prev18_r19.XO_DELAY_EN  = 0x3;// pair usage together with xo_pulse_sel
//    // Pseudo-Resistor Selection
//    prev18_r19.XO_RP_LOW    = 0x0;
//    prev18_r19.XO_RP_MEDIA  = 0x1;
//    prev18_r19.XO_RP_MVT    = 0x0;
//    prev18_r19.XO_RP_SVT    = 0x0;
// 
//    prev18_r19.XO_SLEEP = 0x0;
//    *REG_XO_CONF1 = prev18_r19.as_int;
//    delay(100);
//    prev18_r19.XO_ISOLATE = 0x0;
//    *REG_XO_CONF1 = prev18_r19.as_int;
//    delay(100);
//    prev18_r19.XO_DRV_START_UP  = 0x1;// 1: enables start-up circuit
//    *REG_XO_CONF1 = prev18_r19.as_int;
//    delay(2000);
//    //prev18_r19.XO_SCN_CLK_SEL   = 0x1;// scn clock 1: normal. 0.3V level up to 0.6V, 0:init
//    //*REG_XO_CONF1 = prev18_r19.as_int;
//    //delay(2000);
//    //prev18_r19.XO_SCN_CLK_SEL   = 0x0;
//    //prev18_r19.XO_SCN_ENB       = 0x0;// enable_bar of scn
//    //*REG_XO_CONF1 = prev18_r19.as_int;
//    //delay(2000);
//    //prev18_r19.XO_DRV_START_UP  = 0x0;
//    //prev18_r19.XO_DRV_CORE      = 0x1;// 1: enables core circuit
//    //prev18_r19.XO_SCN_CLK_SEL   = 0x1;
//    //*REG_XO_CONF1 = prev18_r19.as_int;
//
//    enable_xo_timer();
//    start_xo_cout();
//}
//


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
    mbus_write_message32(0xBB, 0xDEEDEEE1);
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
	wfi_timeout_flag = 1;
    }
void handler_ext_int_reg0(void) { // REG0
    // mbus_write_message32(0xBB, 0xDEEDEEE2);
    *NVIC_ICPR = (0x1 << IRQ_REG0);
}
void handler_ext_int_reg1(void) { // REG1
    // mbus_write_message32(0xBB, 0xDEEDEEE3);
    *NVIC_ICPR = (0x1 << IRQ_REG1);
}
void handler_ext_int_reg2(void) { // REG2
    // mbus_write_message32(0xBB, 0xDEEDEEE4);
    *NVIC_ICPR = (0x1 << IRQ_REG2);
}
void handler_ext_int_reg3(void) { // REG3
    // mbus_write_message32(0xBB, 0xDEEDEEE5);
    *NVIC_ICPR = (0x1 << IRQ_REG3);
}
void handler_ext_int_gocep(void) { // GOCEP
    // mbus_write_message32(0xBB, 0xDEEDEEE6);
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    //mbus_write_message32(0xBB, 0xDEEDEEE7);
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
    *SREG_WAKEUP_SOURCE = 0;
}

//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){
    // mbus_write_message32(0xBB, 0xDEEDEEEF);

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
	prev17_r0D.SRAM_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
	prev17_r0D.SRAM_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
	prev17_r0D.SRAM_USE_INVERTER_SA= 1; 
	*REG_SRAM_TUNE = prev17_r0D.as_int;

    // Disable MBus Watchdog Timer
    //*REG_MBUS_WD = 0;
	*((volatile uint32_t *) 0xA000007C) = 0;

  
    //Enumerate & Initialize Registers
    Tstack_state = TSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    exec_count_irq = 0;
	PMU_ADC_4P2_VAL = 0x4B;
	pmu_parkinglot_mode = 0;
	pmu_harvesting_on = 1;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    //Enumeration
    //mbus_enumerate(RAD_ADDR);
	//delay(MBUS_DELAY);
    mbus_enumerate(SNS_ADDR);
	delay(MBUS_DELAY);
    //mbus_enumerate(HRV_ADDR);
	//delay(MBUS_DELAY);
 	//mbus_enumerate(PMU_ADDR);
	//delay(MBUS_DELAY);

    // Set CPU Halt Option as TX --> Use for register write e.g.
	//set_halt_until_mbus_tx();


    // Temp Sensor Settings --------------------------------------

    // snsv11_r01
	snsv11_r01.TSNS_RESETn = 0;
    snsv11_r01.TSNS_EN_IRQ = 1;
    snsv11_r01.TSNS_BURST_MODE = 0;
    snsv11_r01.TSNS_CONT_MODE = 1;
    snsv11_r01.TSNS_EN_CLK_REF = 1;
    snsv11_r01.TSNS_EN_CLK_REF_PAD = 1;
    mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);

	// Set temp sensor conversion time
	snsv11_r03.TSNS_SEL_STB_TIME = 0x1; // Default: 0x3
	snsv11_r03.TSNS_SEL_CONV_TIME = 0x6; // Default: 0x6
    // Set Sensing OSC speed
    //snsv11_r03.TSNS_SEL_VVDD	= 14; // Default : 4'hE

	mbus_remote_register_write(SNS_ADDR,0x03,snsv11_r03.as_int);

    ////////////////////////////////////////////////////////
    // Enable VREF
    snsv11_r00.LDO_EN_VREF 	= 1;
	mbus_remote_register_write(SNS_ADDR,0,snsv11_r00.as_int);
    //delay(MBUS_DELAY*10);

    ////////////////////////////////////////////////////////
    // Enable IREF & LDO ouput
    snsv11_r00.LDO_EN_IREF 	= 1;
    snsv11_r00.LDO_EN_TSNS_OUT	= 1; 
    mbus_remote_register_write(SNS_ADDR,0,snsv11_r00.as_int);
    //delay(MBUS_DELAY*1);

    ////////////////////////////////////////////////////////
    // Enable TSNS Digital Power Gate (LDO)
    snsv11_r01.TSNS_SEL_LDO = 1;
    mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);

    ////////////////////////////////////////////////////////
    // Enable TSNS Analog Power Gate (LDO)
    snsv11_r01.TSNS_EN_SENSOR_LDO = 1;
    mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 100;   // 10: 2-4 sec with PRCv17
	wakeup_data = 0;

    XO_init(); //XO initialization
    XOT_init();
    delay(MBUS_DELAY);

    // Go to sleep
	//set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
    //operation_sleep();
}

typedef struct Measurement {
    uint32_t temp;
} measurement_t;

////////////////////////////////////////////////////////
// Stores data
// measurement_t data_arr[500];
uint32_t data_arr[500];
uint32_t i;
uint32_t val;

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
	i = 0;
	val = 10000;
	set_xo_timer(val, 1, 0);
	operation_sleep_notimer();
    }
    // mbus_write_message32(0xBB, 0xDEADBEEF);

    while(1) {
    	
	set_xo_timer(val, 1, 1);
	mbus_write_message32(0xBB, *REG_XOT_VAL_U);
	mbus_write_message32(0xBB, *REG_XOT_VAL_L);
    val = 10000;
    	operation_sleep_notimer();
    // set_xo_timer(val, 1, 1);
    //operation_sleep_notimer();
    // WFI();
    //mbus_write_message32(0xBB, 0xDEADBEEF);
    }
    // operation_sleep_notimer();

    /*
    if(i < 100){
        uint32_t sum = 0;
        uint8_t j = 0;
        for(j = 0; j < 4; j++) {
            snsv11_r01.TSNS_RESETn = 1;
            snsv11_r01.TSNS_ISOLATE = 0;
            snsv11_r01.TSNS_EN_IRQ = 1;
            mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);

            WFI();

            sum += *REG0;
            mbus_write_message32(0xBB, *REG0);

            snsv11_r01.TSNS_RESETn = 0;
            mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);
            // delay(MBUS_DELAY);
        }
    mbus_write_message32(0xBB, 0xBBB);
         data_arr[i] = sum >> 2;
    mbus_write_message32(0xBB, 0xBBB);
	i++;
            mbus_write_message32(0xBB, 0xBBB);
		set_wakeup_timer(100, 0x1, 0x1);
		operation_sleep();
    }
    else{
	for(i = 0; i<100; i++) {
		mbus_write_message32(0xBB, data_arr[1]);
		delay(MBUS_DELAY);
	}
    }

    // Proceed to continuous mode
    while(1){
        delay(1000);
    }

    // Should not reach here
    operation_sleep_notimer();
    */

    while(1);
}

