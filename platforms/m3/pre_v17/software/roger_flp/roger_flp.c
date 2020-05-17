//****************************************************************************************************
//Author:       Roger Hsiao
//Description:  temperature sensor test
//				- PREv17 / PMUv4 / SNSv10
//****************************************************************************************************
#include "PREv17.h"
#include "mbus.h"

// uncomment this for debug mbus message
#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// uncomment this to only transmit average
//#define TX_AVERAGE

#define SNT_ADDR 0x4
#define FLP_ADDR 0x5

// Temp Sensor parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)
#define TEMP_CYCLE_INIT 3 

// CP parameter
#define CP_DELAY 50000 // Amount of delay between successive messages; 100: 6-7ms

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
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
volatile uint32_t PMU_ADC_4P2_VAL;
volatile uint32_t pmu_parkinglot_mode;
volatile uint32_t pmu_harvesting_on;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile prev17_r0B_t prev17_r0B = PREv17_R0B_DEFAULT;
volatile prev17_r0D_t prev17_r0D = PREv17_R0D_DEFAULT;

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
	uint32_t xot_clk_out_sel = 0x3;
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

	*REG_XOT_CONFIG |= (1 << 16);
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

//***************************************************
//  FLP functions
//***************************************************
void flp_fail(uint32_t id)
{
        delay(10000);
        mbus_write_message32(0xE2, 0xDEADBEEF);
        delay(10000);
        mbus_write_message32(0xE2, id);
        delay(10000);
        mbus_write_message32(0xE2, 0xDEADBEEF);
        delay(10000);
        mbus_sleep_all();
        while(1);
}

void FLASH_initialization (void) {
    
    // Tune Flash
    mbus_remote_register_write (FLP_ADDR , 0x26 , 0x0D7788); // Program Current
    mbus_remote_register_write (FLP_ADDR , 0x27 , 0x011BC8); // Erase Pump Diode Chain
    mbus_remote_register_write (FLP_ADDR , 0x01 , 0x000109); // Tprog idle time
    mbus_remote_register_write (FLP_ADDR , 0x19 , 0x000F03); // Voltage Clamper Tuning
    mbus_remote_register_write (FLP_ADDR , 0x0F , 0x001001); // Flash interrupt target register addr: REG0 -> REG1
    //mbus_remote_register_write (FLP_ADDR , 0x12 , 0x000003); // Auto Power On/Off

}

void FLASH_turn_on()
{
    set_halt_until_mbus_trx();
    mbus_remote_register_write (FLP_ADDR , 0x11 , 0x00002F);
    set_halt_until_mbus_tx();
    mbus_write_message32(0xE2, 0xDEADBEE3);

    if (*REG1 != 0xB5) flp_fail (0);
}

void FLASH_turn_off()
{
    set_halt_until_mbus_trx();
    mbus_remote_register_write (FLP_ADDR , 0x11 , 0x00002D);
    set_halt_until_mbus_tx();

    if (*REG1 != 0xBB) flp_fail (1);
}

void FLASH_flash_to_sram(uint32_t sram_start_addr,
		         uint32_t flsh_start_addr, 
		         uint32_t len_min_one, 
			 uint8_t irq_en) {
	mbus_remote_register_write(FLP_ADDR, 0x07, sram_start_addr);
	mbus_remote_register_write(FLP_ADDR, 0x08, flsh_start_addr);
	delay(MBUS_DELAY);

	set_halt_until_mbus_trx();
	mbus_remote_register_write(FLP_ADDR, 0x09, (len_min_one << 6) | (irq_en << 5) | (0x1 << 1) | (0x1));
	set_halt_until_mbus_tx();

	if(*REG1 != 0x2B) flp_fail (2);

}

void FLASH_sram_to_flash(uint32_t sram_start_addr,
		         uint32_t flsh_start_addr, 
		         uint32_t len_min_one, 
			 uint8_t irq_en) {
	mbus_remote_register_write(FLP_ADDR, 0x07, sram_start_addr);
	mbus_remote_register_write(FLP_ADDR, 0x08, flsh_start_addr);
	delay(MBUS_DELAY);

	set_halt_until_mbus_trx();
	mbus_remote_register_write(FLP_ADDR, 0x09, (len_min_one << 6) | (irq_en << 5) | (0x2 << 1) | (0x1));
	set_halt_until_mbus_tx();

	if(*REG1 != 0x3F) flp_fail (3);

}

void FLASH_pp_ready (void) {

        // Erase Flash
        uint32_t page_start_addr = 0;
        uint32_t idx;

        for (idx=0; idx<512; idx++){ // All Pages (8Mb) are erased.
            page_start_addr = (idx << 8);

            mbus_remote_register_write (FLP_ADDR, 0x08, page_start_addr); // Set FLSH_START_ADDR

            set_halt_until_mbus_rx();
            mbus_remote_register_write (FLP_ADDR, 0x09, 0x000029); // Page Erase
            set_halt_until_mbus_tx();

            if (*REG1 != 0x00004F) flp_fail((0xFFFF << 16) | idx);
        }
    
    // Ping Pong Setting
        mbus_remote_register_write (FLP_ADDR, 0x13 , 0x000001); // Enable Ping Pong w/ No length limit
        mbus_remote_register_write (FLP_ADDR, 0x14 , 0x00000B); // Over run err no detection & fast programming & no wrapping & DATA0/DATA1 enable
        mbus_remote_register_write (FLP_ADDR, 0x15 , 0x000000); // Flash program addr started from 0x0

}

void FLASH_pp_on (void) {
    mbus_remote_register_write (FLP_ADDR, 0x09, 0x00002D); // Ping Pong Go
}

void FLASH_pp_off (void) {
    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP_ADDR, 0x13, 0x000000); // Ping Pong End
    set_halt_until_mbus_tx();
}

/*
void FLASH_read (void) {
    uint32_t i,j;
    uint32_t flp_sram_addr;
    flp_sram_addr = 0;
    FLASH_turn_on();

    // Flash Read
    mbus_remote_register_write (FLP_ADDR, 0x08 , 0x0); // Set FLSH_START_ADDR
    mbus_remote_register_write (FLP_ADDR, 0x07 , 0x0); // Set SRAM_START_ADDR

    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP_ADDR, 0x09 , 0x02EE23); // Flash -> SRAM
    set_halt_until_mbus_tx();

    if (*REG1 != 0x00002B) flp_fail(4);

    for(j=0; j<16; j++){
        set_halt_until_mbus_rx();
        mbus_copy_mem_from_remote_to_any_bulk (FLP_ADDR, (uint32_t *) flp_sram_addr, 0x1, read_data, 99);
        set_halt_until_mbus_tx();
        for (i=0; i<100; i++) {
            //delay(100);
            mbus_write_message32(0xC0, read_data[i]);
            //delay(100);
        }
        flp_sram_addr = flp_sram_addr + 400;
    }

    // Turn off Flash
    FLASH_turn_off();
}
*/

//*******************************************************************
// INTERRUPT HANDLERS (Updated for PRCv17)
//*******************************************************************
uint32_t source = 15;

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
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){
    mbus_write_message32(0xBB, 0xDEEDEEEF);

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
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    exec_count_irq = 0;
	PMU_ADC_4P2_VAL = 0x4B;
	pmu_parkinglot_mode = 0;
	pmu_harvesting_on = 1;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(SNT_ADDR);
    mbus_enumerate(FLP_ADDR);
    //mbus_enumerate(RAD_ADDR);
	//delay(MBUS_DELAY);
	delay(MBUS_DELAY);
    //mbus_enumerate(HRV_ADDR);
	//delay(MBUS_DELAY);
 	//mbus_enumerate(PMU_ADDR);
	//delay(MBUS_DELAY);

    // Set CPU Halt Option as TX --> Use for register write e.g.
	//set_halt_until_mbus_tx();

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 100;   // 10: 2-4 sec with PRCv17
	wakeup_data = 0;

	mbus_write_message32(0xBB, 0xDEADBEEA);
    FLASH_initialization(); //FLPv3L initialization
	mbus_write_message32(0xBB, 0xDEADBEEB);

    XO_init(); //XO initialization
    XOT_init();
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
//	disable_timerwd();

    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
        // Set up PMU/GOC register in PRC layer (every time)
        // Enumeration & RAD/SNS layer register configuration
        operation_init();
	mbus_write_message32(0xBB, 0xDEADBEE1);
	//operation_sleep_notimer();
    }
    mbus_write_message32(0xBB, 0xDEADBEE2);

    uint32_t i;
    uint32_t data[30];
    uint32_t read_data[30];
    uint32_t new_data[30];

    data[0] = 0x111;

    for(i = 0 + 0x100; i < 0x100 + 0x100; i++) {
	    mbus_copy_mem_from_local_to_remote_bulk(FLP_ADDR, (uint32_t*) 0x00, data, 1);
	    set_halt_until_mbus_tx();

	    delay(1000);

	    FLASH_turn_on();
	    FLASH_sram_to_flash(0x0, i, 0, 1);
	    // FLASH_turn_off();

	    delay(1000);

	    // FLASH_turn_on();
	    FLASH_flash_to_sram(0x0, i, 0, 1);
	    FLASH_turn_off();

	    delay(1000);

	    set_halt_until_mbus_trx();
	    mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR, (uint32_t*) 0x00, 0x1, new_data, 0);
	    set_halt_until_mbus_tx();

	    delay(1000);

	    if(new_data[0] != data[0]) {
	    	mbus_write_message32(0xFA, new_data[0]);
	    	mbus_write_message32(0xCA, data[0]);
	    }
	    mbus_write_message32(0xCD, i);

	    data[0]++;

	    delay(1000);
    }

    operation_sleep_notimer();

    for(i = 0; i < 30; i++) {
    	data[i] = 0x456 + i;
    }

    mbus_write_message32(0xBB, 0x111111);

    set_halt_until_mbus_trx();
    mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR, (uint32_t*) 0x10, 0x1, new_data, 29);
    set_halt_until_mbus_tx();


    mbus_remote_register_write(FLP_ADDR, 0x08, 0x0);
    delay(MBUS_DELAY);

    set_halt_until_mbus_trx();
    mbus_remote_register_write(FLP_ADDR, 0x09, (0x1 << 5) | (0x4 << 1) | (0x1));
    set_halt_until_mbus_tx();

    if(*REG1 != 0x4F) flp_fail (4);

    mbus_remote_register_write(FLP_ADDR, 0x08, 0x100);
    delay(MBUS_DELAY);

    set_halt_until_mbus_trx();
    mbus_remote_register_write(FLP_ADDR, 0x09, (0x1 << 5) | (0x4 << 1) | (0x1));
    set_halt_until_mbus_tx();

    if(*REG1 != 0x4F) flp_fail (4);

    mbus_remote_register_write(FLP_ADDR, 0x08, 0x200);
    delay(MBUS_DELAY);

    set_halt_until_mbus_trx();
    mbus_remote_register_write(FLP_ADDR, 0x09, (0x1 << 5) | (0x4 << 1) | (0x1));
    set_halt_until_mbus_tx();

    if(*REG1 != 0x4F) flp_fail (4);

    mbus_remote_register_write(FLP_ADDR, 0x08, 0x300);
    delay(MBUS_DELAY);

    set_halt_until_mbus_trx();
    mbus_remote_register_write(FLP_ADDR, 0x09, (0x1 << 5) | (0x4 << 1) | (0x1));
    set_halt_until_mbus_tx();

    if(*REG1 != 0x4F) flp_fail (4);

    FLASH_turn_on();
    FLASH_flash_to_sram(0x0, 0x000, 29, 1);
    FLASH_turn_off();

    mbus_write_message32(0xBB, 0x111111);

    set_halt_until_mbus_trx();
    mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR, (uint32_t*) 0x10, 0x1, new_data, 29);
    set_halt_until_mbus_tx();

    mbus_write_message32(0xBB, 0xDEADBEE2);
    mbus_copy_mem_from_local_to_remote_bulk(FLP_ADDR, (uint32_t*) 0, data, 29);

    set_halt_until_mbus_trx();
    mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR, (uint32_t*) 0, 0x1, read_data, 29);
    set_halt_until_mbus_tx();

    mbus_write_message32(0xBB, 0xDEADBEE2);

    FLASH_turn_on();
    FLASH_sram_to_flash(0x0, 0x000, 29, 1);
    FLASH_turn_off();

    for(i = 0; i < 30; i++) {
    	data[i] = 0xA23 + i;
    }

    mbus_copy_mem_from_local_to_remote_bulk(FLP_ADDR, (uint32_t*) 0, data, 29);

    set_halt_until_mbus_trx();
    mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR, (uint32_t*) 0, 0x1, read_data, 29);
    set_halt_until_mbus_tx();

    mbus_write_message32(0xE2, 0xDEADBEEF);

    for(i = 0; i < 30; i++) {
    	mbus_write_message32(0xE2, read_data[i]);
	delay(MBUS_DELAY);
    }

    delay(10000);
    mbus_write_message32(0xE2, 0xDEADBEEF);
    delay(10000);

    FLASH_turn_on();
    FLASH_flash_to_sram(0x0, 0x000, 29, 1);
    FLASH_turn_off();

    set_halt_until_mbus_trx();
    mbus_copy_mem_from_remote_to_any_bulk(FLP_ADDR, (uint32_t*) 0x0, 0x1, read_data, 29);
    set_halt_until_mbus_tx();

    delay(10000);
    mbus_write_message32(0xE2, 0xDEADBE11);
    delay(10000);

    for(i = 0; i < 30; i++) {
    	mbus_write_message32(0xE2, read_data[i]);
	delay(MBUS_DELAY);
    }

    // Proceed to continuous mode
    while(1){
        delay(1000);
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

