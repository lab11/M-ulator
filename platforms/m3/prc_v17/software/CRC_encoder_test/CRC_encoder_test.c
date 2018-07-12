//*******************************************************************
//Author: Gyouho Kim
//Description: Simple code that repeats sleep/wakeup
//*******************************************************************
#include "PRCv17.h"
#include "PRCv17_RF.h"
#include "mbus.h"
#include "SNSv10_RF.h"
#include "HRVv5.h"
#include "RADv9.h"
#include "PMUv7_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG_1

// TStack order  PRC->RAD->SNS->HRV->PMU
#define HRV_ADDR 0x3
#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define PMU_ADDR 0x6

#define WAKEUP_PERIOD_PARKING 30000 // About 2 hours (PRCv17)

// Temp Sensor parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)
#define TEMP_CYCLE_INIT 3 

// Tstack states
#define	TSTK_IDLE       0x0
#define	TSTK_LDO        0x1
#define TSTK_TEMP_START 0x2
#define TSTK_TEMP_READ  0x6

#define NUM_TEMP_MEAS 2

// Radio configurations
#define RADIO_DATA_LENGTH 24
#define WAKEUP_PERIOD_RADIO_INIT 10 // About 2 sec (PRCv17)

#define TEMP_STORAGE_SIZE 600 // Need to leave about 500 Bytes for stack --> around 120

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

//********************************************************************
// Global Variables
//********************************************************************
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

volatile prcv17_r0B_t prcv17_r0B = PRCv17_R0B_DEFAULT;

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

  
    //Enumerate & Initialize Registers
    Tstack_state = TSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEE0;
    exec_count = 0;
    exec_count_irq = 0;
	PMU_ADC_4P2_VAL = 0x4B;
	pmu_parkinglot_mode = 0;
	pmu_harvesting_on = 1;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(RAD_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(SNS_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(HRV_ADDR);
	delay(MBUS_DELAY);
 	mbus_enumerate(PMU_ADDR);
	delay(MBUS_DELAY);

    // Set CPU Halt Option as TX --> Use for register write e.g.
	//set_halt_until_mbus_tx();


    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 10;   // 10: 2-4 sec with PRCv17
	wakeup_data = 0;

    delay(MBUS_DELAY);

    // Go to sleep
	set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
    operation_sleep();
}

//***************************************************
// CRC16 Encoding
//***************************************************


#define INT_LEN 32
#define DATA_LEN 96
#define CRC_LEN 16


uint8_t* myDec2Bin(uint32_t);
uint32_t myBin2Dec(uint8_t []);
uint32_t* crcEnc16(uint32_t, uint32_t, uint32_t); // CRC encoder
uint32_t crcDec16(uint32_t, uint32_t, uint32_t); // CRC decoder (pass->1, error->0)

uint8_t* myDec2Bin(uint32_t data)
{
    static uint8_t b[32];
    uint8_t i;
    for (i=0; i<32; i++)
    {
        b[31-i] = data & 1; //data % 2;
        data = (data >> 1); //data / 2;
    }
    return b;
}

uint32_t myBin2Dec(uint8_t bin[])
{
    uint32_t d = 0;
    uint8_t i;
    for (i=0; i<INT_LEN; i++)
    {
        d = d << 1;
        d = d + bin[i];
    }
    return d;
}

uint32_t* crcEnc16(uint32_t data2, uint32_t data1, uint32_t data0)
{
    // intialization
    uint32_t crcLen = CRC_LEN;
    uint8_t poly[17];
    uint8_t remainder[16];
    uint8_t remainder_next[16];
    uint32_t i;

	for (i=0;i<16;i++){
		poly[i] = 0;
		remainder[i] = 0;
		remainder_next[i] = 0;
	}

	poly[0] = 1;
	poly[2] = 1;
	poly[15] = 1;
	poly[16] = 1;

    uint32_t* msg_tmp;
    uint8_t msg2[INT_LEN];
    uint8_t msg1[INT_LEN];
    uint8_t msg0[INT_LEN];
    
    msg_tmp = myDec2Bin(data2);
    for (i=0;i<INT_LEN;i++)
    {
        msg2[i] = msg_tmp[i];
    }
    msg_tmp = myDec2Bin(data1);
    for (i=0;i<INT_LEN;i++)
    {
        msg1[i] = msg_tmp[i];
    }
    msg_tmp = myDec2Bin(data0);
    for (i=0;i<INT_LEN;i++)
    {
        msg0[i] = msg_tmp[i];
    }

    uint32_t msg[96];
    for (i=0; i<INT_LEN; i++)
    {
        msg[i] = msg2[i];
        msg[i+INT_LEN] = msg1[i];
        msg[i+2*INT_LEN] = msg0[i];
    }
    
    // LFSR
    for (i=0; i<DATA_LEN; i++)
    {
        if (i < DATA_LEN-CRC_LEN)
            msg[i] = msg[i+CRC_LEN];
        else
            msg[i] = 0;
    }    

    for (i = 0; i < DATA_LEN; i++)
    {
        uint32_t MSB = remainder[0];
        uint32_t j;
        for (j = 0; j < CRC_LEN-1; j++)
        {
            if (poly[j])
                remainder_next[j] = (remainder[j+1] != MSB);
            else
                remainder_next[j] = remainder[j+1];
        }
        remainder_next[CRC_LEN-1] = (msg[i] != MSB);
        for (j=0; j<CRC_LEN; j++)
        {
            remainder[j] = remainder_next[j];
        }
    }

    for (i=0; i<CRC_LEN; i++)
    {
        msg[DATA_LEN-CRC_LEN+i] = remainder[i];
    }

    uint8_t bin2[INT_LEN];
    uint8_t bin1[INT_LEN];
    uint8_t bin0[INT_LEN];
    for (i=0; i<INT_LEN; i++)
    {
        bin2[i] = msg[i];
        bin1[i] = msg[i+INT_LEN];
        bin0[i] = msg[i+2*INT_LEN];
    }

    uint32_t data2_out = myBin2Dec(bin2);
    uint32_t data1_out = myBin2Dec(bin1);
    uint32_t data0_out = myBin2Dec(bin0);

    static uint32_t msg_out[3];
    msg_out[0] = data2_out;
    msg_out[1] = data1_out;
    msg_out[2] = data0_out;

    return msg_out;    
}



//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

	// Wakeup timer value
	//mbus_write_message32(0xAA,*REG_WUPT_VAL);

    // Only enable relevant interrupts (PRCv17)
	//enable_reg_irq();
	//enable_all_irq();
	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);
  
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);

    // Initialization sequence
    if (enumerated != 0xDEADBEE0){
        operation_init();
    }

	set_halt_until_mbus_tx();
	mbus_write_message32(0xAA,0xABCD1234);
    delay(MBUS_DELAY);

    uint32_t count;
    for( count=0; count<1; count++ ){
		mbus_write_message32(0xAA,count);
		delay(MBUS_DELAY);
	}

	uint32_t radio_data_0 = 0x11112222;
	uint32_t radio_data_1 = 0x33334444;
	uint32_t radio_data_2 = 0x00005555;

    uint32_t* output_data;
	mbus_write_message32(0xCC,0);
    output_data = crcEnc16(radio_data_2, radio_data_1, radio_data_0);

	
	*output_data = 0xAAAAAAA1;
	*(output_data+1) = 0xAAAAAAA2;
	*(output_data+2) = 0xAAAAAAA3;

	mbus_write_message32(0xC0,*output_data);
	mbus_write_message32(0xC1,*(output_data+1));
	mbus_write_message32(0xC2,*(output_data+2));


	set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
	//mbus_write_message32(0xFF,*REG_WUPT_VAL);
    operation_sleep();

    while(1);
}


