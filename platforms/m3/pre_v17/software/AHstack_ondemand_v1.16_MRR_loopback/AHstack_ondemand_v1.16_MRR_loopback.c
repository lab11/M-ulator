//*******************************************************************
//Author: Gyouho Kim
//Description: AH stack with MRRv7 and SNSv10, ADXL362, SHT35
//			Modified from 'AMstack_ondemand_v1.14.c'
//			AH Stack uses 3V battery
//			v1.14: MRRv7 - larger tuning range, stronger drive
//				   New packet definitions with CRC16
//				   SHT35 uses power switch [0]
//				   ADXL uses power switch [1]
//			v1.14b: fixes a bug in humidty/temp reading
//					fixes a bug in REG_CPS control
//					fixes CRC bug
//			v1.15: fixes ADXL / SHT35 conflict
//			v1.15b: improve latency of radio tx for motion detection
//					adding configurability for SHT35
//			v1.16: improve latency of radio tx further
//					changing MRR decap configuration and shorter SFO settling time
//*******************************************************************
#include "PREv17.h"
#include "PREv17_RF.h"
#include "mbus.h"
#include "SNSv10_RF.h"
#include "PMUv7_RF.h"
#include "ADXL362.h"
#include "HRVv5.h"
#include "MRRv7_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// AM stack 
#define HRV_ADDR 0x3
#define MRR_ADDR 0x4
#define SNS_ADDR 0x5
#define PMU_ADDR 0x6

// System parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define SNS_CYCLE_INIT 3 

// Pstack states
#define	STK_IDLE		0x0
#define	STK_LDO		0x1
#define	STK_TEMP_START 0x2
#define	STK_TEMP_READ  0x3
#define	STK_HUM     0x4

// ADXL362 Defines
#define SPI_TIME 250

// Radio configurations
#define RADIO_DATA_LENGTH 192
#define WAKEUP_PERIOD_RADIO_INIT 10 // About 2 sec (PRCv17)

#define TEMP_NUM_MEAS 1

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

#define GPIO_ADXL_INT 2
#define GPIO_ADXL_EN 4
#define GPIO_SDA 1
#define GPIO_SCL 0

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t stack_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint32_t PMU_ADC_3P0_VAL;
volatile uint32_t pmu_sar_conv_ratio_val;
volatile uint32_t read_data_batadc;
volatile uint32_t sleep_time_prev;
volatile uint32_t sleep_time_threshold_factor;
volatile uint32_t wakeup_period_calc_factor;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile uint32_t temp_storage_latest = 150; // SNSv10
volatile uint32_t temp_storage_last_wakeup_adjust = 150; // SNSv10
volatile uint32_t temp_storage_diff = 0;
volatile uint32_t read_data_temp;

volatile uint32_t sns_running;
volatile uint32_t set_sns_exec_count;

volatile uint32_t astack_detection_mode;
volatile uint32_t adxl_enabled;
volatile uint32_t adxl_motion_detected;
volatile uint32_t adxl_user_threshold;
volatile uint32_t adxl_motion_trigger_count;
volatile uint32_t adxl_trigger_mute_count;
volatile uint32_t adxl_mask = (1<<GPIO_ADXL_INT) | (1<<GPIO_ADXL_EN);

volatile uint32_t sht35_temp_data, sht35_hum_data;
volatile uint32_t sht35_mask = (1<<GPIO_SDA) | (1<<GPIO_SCL);
volatile uint32_t sht35_user_repeatability; // Default 0x0B


volatile uint32_t temp_alert;

volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t mrr_freq_hopping;
volatile uint32_t mrr_freq_hopping_step;
volatile uint32_t mrr_cfo_val_fine_min;
volatile uint32_t RADIO_PACKET_DELAY;
volatile uint32_t radio_packet_count;

volatile snsv10_r00_t snsv10_r00 = SNSv10_R00_DEFAULT;
volatile snsv10_r01_t snsv10_r01 = SNSv10_R01_DEFAULT;
volatile snsv10_r03_t snsv10_r03 = SNSv10_R03_DEFAULT;
volatile snsv10_r17_t snsv10_r17 = SNSv10_R17_DEFAULT;

volatile prev17_r0B_t prev17_r0B = PREv17_R0B_DEFAULT;
volatile prev17_r0D_t prev17_r0D = PREv17_R0D_DEFAULT;

volatile mrrv7_r00_t mrrv7_r00 = MRRv7_R00_DEFAULT;
volatile mrrv7_r01_t mrrv7_r01 = MRRv7_R01_DEFAULT;
volatile mrrv7_r02_t mrrv7_r02 = MRRv7_R02_DEFAULT;
volatile mrrv7_r03_t mrrv7_r03 = MRRv7_R03_DEFAULT;
volatile mrrv7_r04_t mrrv7_r04 = MRRv7_R04_DEFAULT;
volatile mrrv7_r05_t mrrv7_r05 = MRRv7_R05_DEFAULT;
volatile mrrv7_r07_t mrrv7_r07 = MRRv7_R07_DEFAULT;
volatile mrrv7_r10_t mrrv7_r10 = MRRv7_R10_DEFAULT;
volatile mrrv7_r11_t mrrv7_r11 = MRRv7_R11_DEFAULT;
volatile mrrv7_r12_t mrrv7_r12 = MRRv7_R12_DEFAULT;
volatile mrrv7_r13_t mrrv7_r13 = MRRv7_R13_DEFAULT;
volatile mrrv7_r14_t mrrv7_r14 = MRRv7_R14_DEFAULT;
volatile mrrv7_r15_t mrrv7_r15 = MRRv7_R15_DEFAULT;
volatile mrrv7_r16_t mrrv7_r16 = MRRv7_R16_DEFAULT;

volatile mrrv7_r1E_t mrrv7_r1E = MRRv7_R1E_DEFAULT;
volatile mrrv7_r1F_t mrrv7_r1F = MRRv7_R1F_DEFAULT;


//***************************************************
// Sleep Functions
//***************************************************
static void operation_sns_sleep_check(void);
static void operation_sleep(void);
static void operation_sleep_noirqreset(void);
static void operation_sleep_notimer(void);


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
//[ 0] = GOC/EP
//[ 1] = Wakeuptimer
//[ 2] = XO timer
//[ 3] = gpio_pad
//[ 4] = mbus message
//[ 8] = gpio[0]
//[ 9] = gpio[1]
//[10] = gpio[2]
//[11] = gpio[3]
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
	// Report who woke up
	delay(MBUS_DELAY);
	mbus_write_message32(0xAA,*SREG_WAKEUP_SOURCE); // 0x1: GOC; 0x2: PRC Timer; 0x10: SNT
}


//***************************************************
// CRC16 Encoding
//***************************************************
#define DATA_LEN 96
#define CRC_LEN 16

uint32_t* crcEnc16(uint32_t data2, uint32_t data1, uint32_t data0)
{
    // intialization
    uint32_t i;
   
    uint16_t poly = 0xc002;
    uint16_t poly_not = ~poly;
    uint16_t remainder = 0x0000;
    uint16_t remainder_shift = 0x0000;
    data2 = (data2 << CRC_LEN) + (data1 >> CRC_LEN);
    data1 = (data1 << CRC_LEN) + (data0 >> CRC_LEN);
    data0 = data0 << CRC_LEN;
    
    // LFSR
    uint16_t input_bit;
    for (i = 0; i < DATA_LEN; i++)
    {
        uint16_t MSB;
        if (remainder > 0x7fff)
            MSB = 0xffff;
        else
            MSB = 0x0000;
        
        if (i < 32)
            input_bit = ((data2 << i) > 0x7fffffff);
        else if (i < 64)
            input_bit = (data1 << (i-32)) > 0x7fffffff;
        else
            input_bit = (data0 << (i-64)) > 0x7fffffff;

        remainder_shift = remainder << 1;
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift))
                         + (input_bit^(remainder > 0x7fff));
    }

    data0 = data0 + remainder;

    static uint32_t msg_out[3];
    msg_out[0] = data2;
    msg_out[1] = data1;
    msg_out[2] = data0;

    return msg_out;    
}

//***************************************************
// MRR Functions
//***************************************************

static void radio_power_on(){
	// Turn off PMU ADC
	//pmu_adc_disable();

	// Need to speed up sleep pmu clock
	//pmu_set_sleep_radio();

    // Turn off Current Limter Briefly
    mrrv7_r00.MRR_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	// Set decap to parallel
	//mrrv7_r03.MRR_DCP_S_OW = 0;  //TX_Decap S (forced charge decaps)
	//mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
	//mrrv7_r03.MRR_DCP_P_OW = 1;  //RX_Decap P 
	//mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
    //delay(MBUS_DELAY);

	// Set decap to series
	//mrrv7_r03.MRR_DCP_P_OW = 0;  //RX_Decap P 
	//mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
	//mrrv7_r03.MRR_DCP_S_OW = 1;  //TX_Decap S (forced charge decaps)
	//mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
    //delay(MBUS_DELAY);

	// Current Limter set-up 
	mrrv7_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    radio_on = 1;

    // Turn on Current Limter
    mrrv7_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Release timer power-gate
    mrrv7_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
    delay(MBUS_DELAY);

	// Turn on timer
    mrrv7_r04.RO_RESET = 0;  //Release Reset TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
    delay(MBUS_DELAY);

    mrrv7_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
    delay(MBUS_DELAY);

    mrrv7_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);

    // Release FSM Sleep
    mrrv7_r11.MRR_RAD_FSM_SLEEP = 0;  // Power on BB
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
	delay(MBUS_DELAY*5); // Freq stab

}

static void radio_power_off(){
	// Need to restore sleep pmu clock

	// Enable PMU ADC
	//pmu_adc_enable();

    // Turn off Current Limter Briefly
    mrrv7_r00.MRR_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	// Current Limter set-up 
	mrrv7_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Turn on Current Limter
    mrrv7_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Turn off everything
    mrrv7_r03.MRR_TRX_ISOLATEN = 0;     //set ISOLATEN 0
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv7_r03.as_int);

    mrrv7_r11.MRR_RAD_FSM_EN = 0;  //Stop BB
    mrrv7_r11.MRR_RAD_FSM_RSTN = 0;  //RST BB
    mrrv7_r11.MRR_RAD_FSM_SLEEP = 1;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);

    mrrv7_r04.RO_RESET = 1;  //Release Reset TIMER
    mrrv7_r04.RO_EN_CLK = 0; //Enable CLK TIMER
    mrrv7_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);

    // Enable timer power-gate
    mrrv7_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);

    radio_on = 0;
	radio_ready = 0;

}

static void mrr_configure_pulse_width_long(){

    mrrv7_r12.MRR_RAD_FSM_TX_PW_LEN = 24; //100us PW
    mrrv7_r13.MRR_RAD_FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv7_r12.MRR_RAD_FSM_TX_PS_LEN = 49; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv7_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);
}

/*
static void mrr_configure_pulse_width_long_2(){

    mrrv7_r12.MRR_RAD_FSM_TX_PW_LEN = 19; //80us PW
    mrrv7_r13.MRR_RAD_FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv7_r12.MRR_RAD_FSM_TX_PS_LEN = 39; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv7_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);
}

static void mrr_configure_pulse_width_long_3(){

    mrrv7_r12.MRR_RAD_FSM_TX_PW_LEN = 9; //40us PW
    mrrv7_r13.MRR_RAD_FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv7_r12.MRR_RAD_FSM_TX_PS_LEN = 19; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv7_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);
}

*/
static void mrr_configure_pulse_width_short(){

    mrrv7_r12.MRR_RAD_FSM_TX_PW_LEN = 0; //4us PW
    mrrv7_r13.MRR_RAD_FSM_TX_C_LEN = 32; // (PW_LEN+1):C_LEN=1:32
    mrrv7_r12.MRR_RAD_FSM_TX_PS_LEN = 1; // PW=PS guard interval betwen 0 and 1 pulse

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv7_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);
}


static void send_radio_data_mrr_sub1(){

	// Use Timer32 as timeout counter
    wfi_timeout_flag = 0;
	config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

    // Turn on Current Limter
    mrrv7_r00.MRR_CL_EN = 1;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Fire off data
	mrrv7_r11.MRR_RAD_FSM_EN = 1;  //Start BB
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);

	// Wait for radio response
	WFI();

	// Turn off Timer32
	*TIMER32_GO = 0;

	if (wfi_timeout_flag){
		mbus_write_message32(0xFA, 0xFAFAFAFA);
	}

    // Turn off Current Limter
    mrrv7_r00.MRR_CL_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	mrrv7_r11.MRR_RAD_FSM_EN = 0;
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
}

static void send_radio_data_mrr(uint32_t last_packet, uint8_t radio_packet_prefix, uint16_t radio_data_2, uint32_t radio_data_1, uint32_t radio_data_0){
	// Sends 192 bit packet, of which 96b is actual data
	// MRR REG_9: reserved for header
	// MRR REG_A: reserved for header
	// MRR REG_B: reserved for header
	// MRR REG_C: reserved for header
	// MRR REG_D: DATA[23:0]
	// MRR REG_E: DATA[47:24]
	// MRR REG_F: DATA[71:48]
	// MRR REG_10: DATA[95:72]

	// CRC16 Encoding 
    uint32_t* output_data;
    output_data = crcEnc16(((radio_packet_count & 0xFF)<<8) | radio_packet_prefix, (radio_data_2 <<16) | ((radio_data_1 & 0xFFFFFF) >>8), (radio_data_1 << 24) | (radio_data_0 & 0xFFFFFF));

    mbus_remote_register_write(MRR_ADDR,0xD,radio_data_0);
    mbus_remote_register_write(MRR_ADDR,0xE,radio_data_1);
    mbus_remote_register_write(MRR_ADDR,0xF,(radio_packet_prefix<<16)|radio_data_2);
    mbus_remote_register_write(MRR_ADDR,0x10,(radio_packet_count&0xFF)|((output_data[2] & 0xFFFF)/*CRC16*/<<8));

    if (!radio_ready){
		radio_ready = 1;

		// Release FSM Reset
		mrrv7_r11.MRR_RAD_FSM_RSTN = 1;  //UNRST BB
		mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
		delay(MBUS_DELAY);

    	mrrv7_r03.MRR_TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    	mbus_remote_register_write(MRR_ADDR,0x03,mrrv7_r03.as_int);
		delay(MBUS_DELAY);

		// Current Limter set-up 
		mrrv7_r00.MRR_CL_CTRL = 2; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    }

	uint32_t count = 0;
	uint32_t mrr_cfo_val_fine = 0;
	uint32_t num_packets = 1;
	if (mrr_freq_hopping) num_packets = mrr_freq_hopping;
	
	// New for MRRv7
	mrr_cfo_val_fine = mrr_cfo_val_fine_min;

	while (count < num_packets){
		#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xCE, mrr_cfo_val);
		#endif

		mrrv7_r01.MRR_TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
		mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine;
		mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);
		send_radio_data_mrr_sub1();
		count++;
		if (count < num_packets){
			delay(RADIO_PACKET_DELAY);
		}
		mrr_cfo_val_fine = mrr_cfo_val_fine + mrr_freq_hopping_step; // 1: 0.8MHz, 2: 1.6MHz step
	}

	radio_packet_count++;

	if (last_packet){
		radio_ready = 0;
		radio_power_off();
	}else{
		mrrv7_r11.MRR_RAD_FSM_EN = 0;
		mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
	}
}
//***************************************************
// Temp Sensor Functions (SNSv10)
//***************************************************

static void temp_sensor_start(){
	snsv10_r01.TSNS_RESETn = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);
}
static void temp_sensor_reset(){
	snsv10_r01.TSNS_RESETn = 0;
	mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);
}
static void temp_sensor_power_on(){
	// Turn on digital block
	snsv10_r01.TSNS_SEL_LDO = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);
	// Turn on analog block
	snsv10_r01.TSNS_EN_SENSOR_LDO = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);

	delay(MBUS_DELAY);

	// Release isolation
	snsv10_r01.TSNS_ISOLATE = 0;
	mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);
}
static void temp_sensor_power_off(){
	snsv10_r01.TSNS_RESETn = 0;
	snsv10_r01.TSNS_SEL_LDO = 0;
	snsv10_r01.TSNS_EN_SENSOR_LDO = 0;
	snsv10_r01.TSNS_ISOLATE = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);
}
static void sns_ldo_vref_on(){
	snsv10_r00.LDO_EN_VREF 	= 1;
	mbus_remote_register_write(SNS_ADDR,0,snsv10_r00.as_int);
}

static void sns_ldo_power_on(){
	snsv10_r00.LDO_EN_IREF 	= 1;
	snsv10_r00.LDO_EN_TSNS_OUT	= 1;
	mbus_remote_register_write(SNS_ADDR,0,snsv10_r00.as_int);
}
static void sns_ldo_power_off(){
	snsv10_r00.LDO_EN_VREF 	= 0;
	snsv10_r00.LDO_EN_IREF 	= 0;
	snsv10_r00.LDO_EN_TSNS_OUT	= 0;
	mbus_remote_register_write(SNS_ADDR,0,snsv10_r00.as_int);
}


//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sns_sleep_check(void){
	// Make sure LDO is off
	if (sns_running){
		sns_running = 0;
		temp_sensor_power_off();
		sns_ldo_power_off();
	}
}

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

	// Make sure the irq counter is reset    
    exec_count_irq = 0;

	operation_sns_sleep_check();	

    // Make sure Radio is off
    if (radio_on){radio_power_off();}

	// Disable Timer
	set_wakeup_timer(0, 0, 0);

    // Go to sleep
    operation_sleep();

}


uint32_t dumb_divide(uint32_t nu, uint32_t de) {
// Returns quotient of nu/de

    uint32_t temp = 1;
    uint32_t quotient = 0;

    while (de <= nu) {
        de <<= 1;
        temp <<= 1;
    }

    //printf("%d %d\n",de,temp,nu);
    while (temp > 1) {
        de >>= 1;
        temp >>= 1;

        if (nu >= de) {
            nu -= de;
            //printf("%d %d\n",quotient,temp);
            quotient += temp;
        }
    }

    return quotient;
}

static void measure_wakeup_period(void){

    // Reset Wakeup Timer
    *WUPT_RESET = 1;

	#ifdef DEBUG_MBUS_MSG
	mbus_write_message32(0xE0, 0x0);
	#endif
	// Prevent watchdogs from kicking in
   	config_timerwd(TIMERWD_VAL*2);
	*REG_MBUS_WD = 1500000*3; // default: 1500000

	uint32_t wakeup_timer_val_0 = *REG_WUPT_VAL;
	uint32_t wakeup_period_count = 0;

	while( *REG_WUPT_VAL <= wakeup_timer_val_0 + 1){
		wakeup_period_count = 0;
	}
	while( *REG_WUPT_VAL <= wakeup_timer_val_0 + 2){
		wakeup_period_count++;
	}
	mbus_write_message32(0xE1, wakeup_period_count);
	#ifdef DEBUG_MBUS_MSG
	mbus_write_message32(0xE2, *REG_WUPT_VAL);
	#endif

   	config_timerwd(TIMERWD_VAL);

	WAKEUP_PERIOD_CONT = dumb_divide(WAKEUP_PERIOD_CONT_USER*1000*wakeup_period_calc_factor, wakeup_period_count);

	// Limitation of PRCv17
    if (WAKEUP_PERIOD_CONT > 0x7FFF){
        WAKEUP_PERIOD_CONT = 0x7FFF;
    }

	#ifdef DEBUG_MBUS_MSG
	mbus_write_message32(0xED, WAKEUP_PERIOD_CONT); 
	#endif
}



static void operation_init(void){
  
	// Set CPU & Mbus Clock Speeds
    prev17_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prev17_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prev17_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
    prev17_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
    prev17_r0B.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x6
	*REG_CLKGEN_TUNE = prev17_r0B.as_int;

    prev17_r0D.SRAM_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
    prev17_r0D.SRAM_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
    prev17_r0D.SRAM_USE_INVERTER_SA= 0; 
	*REG_SRAM_TUNE = prev17_r0D.as_int;
  
  
    //Enumerate & Initialize Registers
    stack_state = STK_IDLE; 	//0x0;
    enumerated = 0x41481160; // 0x4148 is AH in ascii
    exec_count = 0;
    exec_count_irq = 0;
	PMU_ADC_3P0_VAL = 0x62;
  

    //Enumeration
    mbus_enumerate(MRR_ADDR);
	delay(MBUS_DELAY);


    // MRR Settings --------------------------------------

	// Decap in series
	//mrrv7_r03.MRR_DCP_P_OW = 0;  //RX_Decap P 
	//mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
	//mrrv7_r03.MRR_DCP_S_OW = 1;  //TX_Decap S (forced charge decaps)
	//mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);

	// Current Limter set-up 
	mrrv7_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Turn on Current Limter
    mrrv7_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
	delay(MBUS_DELAY*100); // Wait for decap to charge

	mrrv7_r1F.LC_CLK_RING = 0x3;  // ~ 150 kHz
	mrrv7_r1F.LC_CLK_DIV = 0x3;  // ~ 150 kHz
	mbus_remote_register_write(MRR_ADDR,0x1F,mrrv7_r1F.as_int);

	mrr_configure_pulse_width_short();
	//mrr_configure_pulse_width_long();

	mrr_freq_hopping = 5;
	mrr_freq_hopping_step = 4;

	mrr_cfo_val_fine_min = 0x0000;

	// RO setup (SFO)
	// Adjust Diffusion R
	mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF

	// Adjust Poly R
	mbus_remote_register_write(MRR_ADDR,0x08,0x400000); // RO_POLY

	// Adjust C
	mrrv7_r07.RO_MOM = 0x10;
	mrrv7_r07.RO_MIM = 0x10;
	mbus_remote_register_write(MRR_ADDR,0x07,mrrv7_r07.as_int);

	// TX Setup Carrier Freq
	mrrv7_r00.MRR_TRX_CAP_ANTP_TUNE_COARSE = 0x1F;  //ANT CAP 10b unary 830.5 MHz
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
	mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_COARSE = 0x1F; //ANT CAP 10b unary 830.5 MHz
	mrrv7_r01.MRR_TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
	mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
	mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);
	mrrv7_r02.MRR_TX_BIAS_TUNE = 0x1FFF;  //Set TX BIAS TUNE 13b // Set to max
	mbus_remote_register_write(MRR_ADDR,0x02,mrrv7_r02.as_int);

	// Turn on RX mode for loopback
    mrrv7_r03.MRR_TRX_MODE_EN = 1; //Set TRX mode

	// RX Setup
    mrrv7_r03.MRR_RX_BIAS_TUNE    = 0x02AF;//  turn on Q_enhancement
	//mrrv7_r03.MRR_RX_BIAS_TUNE    = 0x0001;//  turn off Q_enhancement
	mrrv7_r03.MRR_RX_SAMPLE_CAP    = 0x1;  // RX_SAMPLE_CAP
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);

	mrrv7_r14.MRR_RAD_FSM_RX_POWERON_LEN = 0x0;  //Set RX Power on length
	mrrv7_r14.MRR_RAD_FSM_GUARD_LEN = 0x000C; //Set TX_RX Guard length, TX_RX guard 32 cycle (28+5)
    mrrv7_r14.MRR_RAD_FSM_TX_POWERON_LEN = 2; //3bits
    mrrv7_r14.MRR_RAD_FSM_RX_POWERON_LEN = 0x0;  //Set RX Power on length
    mrrv7_r14.MRR_RAD_FSM_RX_SAMPLE_LEN = 0x0;  //Set RX Sample length  4us

	mbus_remote_register_write(MRR_ADDR,0x14,mrrv7_r14.as_int);

	mrrv7_r15.MRR_RAD_FSM_RX_HDR_BITS = 0x00;  //Set RX header
	mrrv7_r15.MRR_RAD_FSM_RX_HDR_TH = 0x00;    //Set RX header threshold
	mrrv7_r15.MRR_RAD_FSM_RX_DATA_BITS = 0x1F; //Set RX data 1b
	mbus_remote_register_write(MRR_ADDR,0x15,mrrv7_r15.as_int);

	// RAD_FSM set-up 
	// Using first 48 bits of data as header
	mbus_remote_register_write(MRR_ADDR,0x09,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0A,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0B,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0C,0x7AC800);
	mrrv7_r11.MRR_RAD_FSM_TX_H_LEN = 0; //31-31b header (max)
	mrrv7_r11.MRR_RAD_FSM_TX_D_LEN = RADIO_DATA_LENGTH; //0-skip tx data
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);

	mrrv7_r13.MRR_RAD_FSM_TX_MODE = 3; //code rate 0:4 1:3 2:2 3:1(baseline) 4:1/2 5:1/3 6:1/4
	mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);

	// Mbus return address
	mbus_remote_register_write(MRR_ADDR,0x1E,0x1002);

	// Loopback debugging
	mrrv7_r15.MRR_EN_DIG_MONITOR = 1;
	mbus_remote_register_write(MRR_ADDR,0x15,mrrv7_r15.as_int);

	mrrv7_r16.MRR_DIG_MONITOR_SEL1 = 0xC;
	mbus_remote_register_write(MRR_ADDR,0x16,mrrv7_r16.as_int);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 3;   // 0x1E (30): ~1 min with PRCv9
    radio_tx_option = 0; //enables radio tx for each measurement 
    sns_running = 0;
    radio_ready = 0;
    radio_on = 0;
	wakeup_data = 0;
	set_sns_exec_count = 0; // specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
	RADIO_PACKET_DELAY = 2500;
	radio_packet_count = 0;
	
	astack_detection_mode = 0;
	adxl_enabled = 0;
	adxl_motion_detected = 0;

	adxl_trigger_mute_count = 2;
	sleep_time_threshold_factor = 1;
	
	adxl_user_threshold = 0x060; // rec. 0x60, max 0x7FF
	sht35_user_repeatability = 0x0B; // default 0x0B

	wakeup_period_calc_factor = 18;

    // Go to sleep without timer
    operation_sleep_notimer();
}


static void operation_goc_trigger_init(void){

	// This is critical
	set_halt_until_mbus_tx();

	// Debug
	//mbus_write_message32(0xAA,0xABCD1234);
	//mbus_write_message32(0xAA,wakeup_data);

	// Initialize variables & registers
	sns_running = 0;
	stack_state = STK_IDLE;
	
	radio_power_off();
	temp_sensor_power_off();
	sns_ldo_power_off();

	radio_packet_count = 0;
}

static void operation_goc_trigger_radio(uint32_t radio_tx_num, uint32_t wakeup_timer_val, uint8_t radio_tx_prefix, uint8_t radio_tx_data1, uint32_t radio_tx_data0){

	// Prepare radio TX
	radio_power_on();
	exec_count_irq++;
	// radio
	send_radio_data_mrr(1,radio_tx_prefix,*REG_CHIP_ID,((0xBB00|read_data_batadc)<<8)|radio_tx_data1,radio_tx_data0);	

	if (exec_count_irq < radio_tx_num){
		// set timer
		set_wakeup_timer (wakeup_timer_val, 0x1, 0x1);
		// go to sleep and wake up with same condition
		operation_sleep_noirqreset();
		
	}else{
		exec_count_irq = 0;
		// Go to sleep without timer
		operation_sleep_notimer();
	}
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(){

    // Only enable relevant interrupts (PRCv17)
	//enable_reg_irq();
	//enable_all_irq();
	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);
  
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);

	// Figure out who triggered wakeup
	if(*SREG_WAKEUP_SOURCE & 0x00000008){
		// Debug
		#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xAA,0x11331133);
		#endif
		adxl_motion_detected = 1;
	}

	// Record previous sleep time
	sleep_time_prev = *REG_WUPT_VAL;
	// Debug
	#ifdef DEBUG_MBUS_MSG
	mbus_write_message32(0xAB, sleep_time_prev);
	#endif

    // Initialization sequence
    if (enumerated != 0x41481160){
        operation_init();
    }


    // Check if wakeup is due to GOC interrupt  
    // 0x8C is reserved for GOC-triggered wakeup (Named GOC_DATA_IRQ)
    // 8 MSB bits of the wakeup data are used for function ID
    wakeup_data = *GOC_DATA_IRQ;
    uint32_t wakeup_data_header = (wakeup_data>>24) & 0xFF;
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;
    uint32_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
    uint32_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;

	// In case GOC triggered in the middle of routines
	if ((wakeup_data_header != 0) && (exec_count_irq == 0)){
		operation_goc_trigger_init();
	}

    if(wakeup_data_header == 1){
        // Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the MSB of # of transmissions
		operation_goc_trigger_radio(wakeup_data_field_0 + (wakeup_data_field_2<<8), wakeup_data_field_1, 0xA0, 0, exec_count_irq);

    }else if(wakeup_data_header == 0x51){
		// Debug trigger for MRR testing; repeat trigger 1 for 0xFFFFFFFF times
		operation_goc_trigger_radio(0xFFFFFFFF, wakeup_data_field_1, 0xA0, 0, exec_count_irq);


	}else if(wakeup_data_header == 0x21){
		// Change the radio tx packet delay
		uint32_t user_val = wakeup_data & 0xFFFFFF;
		if (user_val < 500){
			RADIO_PACKET_DELAY = 4000;
		}else{
			RADIO_PACKET_DELAY = user_val;
		}
		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x22){
		// Change the carrier frequency of MRR (CFO)
		// Updated for MRRv7
		// wakeup_data[15:0]: Fine+Coarse setting
		// wakeup_data[23:16]: Turn on/off freq hopping 

		mrr_freq_hopping = wakeup_data_field_2 & 0xF;
		mrr_freq_hopping_step = wakeup_data_field_2 >> 4;

		mrr_cfo_val_fine_min = (wakeup_data >> 10) & 0x3F; // 6 bit
	
		mrrv7_r00.MRR_TRX_CAP_ANTP_TUNE_COARSE = wakeup_data & 0x3FF; // 10 bit coarse setting 
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
		mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_COARSE = wakeup_data & 0x3FF; // 10 bit coarse setting
		mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x23){
		// Change the baseband frequency of MRR (SFO)
		mrrv7_r07.RO_MOM = wakeup_data & 0x3F;
		mrrv7_r07.RO_MIM = wakeup_data & 0x3F;
		mbus_remote_register_write(MRR_ADDR,0x07,mrrv7_r07.as_int);
		
		// Go to sleep without timer
		operation_sleep_notimer();



    }else if(wakeup_data_header == 0x91){

	// Turn on cont mode

		disable_timerwd();
		*MBCWD_RESET = 1;

		//mrrv7_r00.MRR_TRX_CAP_ANTP_TUNE_COARSE = 0x3FF;  //ANT CAP 14b unary 805.5 MHz
		//mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

		//mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_COARSE = 0x3FF;  //ANT CAP 14b unary 805.5 MHz
		//mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);

		mrrv7_r02.MRR_TX_BIAS_TUNE = 0x1FFF;  //Set TX BIAS TUNE 13b // Set to max
		mbus_remote_register_write(MRR_ADDR,0x02,mrrv7_r02.as_int);

		mrrv7_r00.MRR_CL_CTRL = 1;
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
		mrrv7_r00.MRR_CL_EN = 1;
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
		mrrv7_r03.MRR_TRX_ISOLATEN = 0;
		mrrv7_r03.MRR_DCP_S_OW = 1;
		mbus_remote_register_write(MRR_ADDR,0x03,mrrv7_r03.as_int);
		mrrv7_r02.MRR_TX_EN_OW = 0x1;
		mbus_remote_register_write(MRR_ADDR,0x02,mrrv7_r02.as_int);


    }else if(wakeup_data_header == 0x92){
	// Turn off cont mode

		mrrv7_r02.MRR_TX_EN_OW = 0x0;
		mbus_remote_register_write(MRR_ADDR,0x02,mrrv7_r02.as_int);
		mrrv7_r00.MRR_CL_EN = 0;
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

		mrrv7_r03.MRR_DCP_S_OW = 0;
		mbus_remote_register_write(MRR_ADDR,0x03,mrrv7_r03.as_int);






	}else if(wakeup_data_header == 0xF0){

		operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xA2, 0, enumerated);

	}else if(wakeup_data_header == 0xFA){

		// Extra protection
		if ((wakeup_data&0xFFFFFF) == 0x89D7E2){
			config_timerwd(0xFF);
			while(1){
				mbus_write_message32(0xE0, 0x0);
				delay(MBUS_DELAY);
			}
		}else{
			operation_sleep_notimer();
		}

    }else{
		if (wakeup_data_header != 0){
			// Invalid GOC trigger
            // Go to sleep without timer
            operation_sleep_notimer();
		}
	}


	operation_sleep_notimer();

    while(1);
}


