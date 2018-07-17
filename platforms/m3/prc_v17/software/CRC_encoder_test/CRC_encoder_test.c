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
volatile uint32_t wfi_timeout_flag;

volatile uint32_t WAKEUP_PERIOD_CONT; 

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



static void operation_init(void){
  
	// Set CPU & Mbus Clock Speeds
    prcv17_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prcv17_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv17_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
    prcv17_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
    prcv17_r0B.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x6
	*REG_CLKGEN_TUNE = prcv17_r0B.as_int;

    //Enumerate & Initialize Registers
    enumerated = 0xDEADBEE0;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();


    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 10;   // 10: 2-4 sec with PRCv17

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
uint32_t crcDec16(uint32_t data2, uint32_t data1, uint32_t data0)
{
    // intialization
    uint32_t i;
   
    uint16_t poly = 0xc002;
    uint16_t poly_not = ~poly;
    uint16_t remainder = 0x0000;
    uint16_t remainder_shift = 0x0000;
    
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
    //printf("remainder = %x\n",remainder);
    uint32_t crcPass;
	// FIXME: debug
	mbus_write_message32(0xFA,remainder);
    if (remainder == 0x0)
        crcPass = 1;
    else
        crcPass = 0;
    return crcPass;
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

	uint32_t radio_data_0 = 0x9879a546;
	uint32_t radio_data_1 = 0x76465426;
	uint32_t radio_data_2 = 0x2481;

    uint32_t* output_data;
	mbus_write_message32(0xCC,0);
    output_data = crcEnc16(radio_data_2, radio_data_1, radio_data_0);

	mbus_write_message32(0xC0,output_data[0]);
	mbus_write_message32(0xC1,output_data[1]);
	mbus_write_message32(0xC2,output_data[2]);


	uint32_t crc_check;

	crc_check = crcDec16(output_data[0],output_data[1],output_data[2]);
	mbus_write_message32(0xCC,crc_check);
	crc_check = crcDec16(output_data[0]>>1,output_data[1],output_data[2]);
	mbus_write_message32(0xCC,crc_check);
	
	
	set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
	//mbus_write_message32(0xFF,*REG_WUPT_VAL);
    operation_sleep();

    while(1);
}


