//****************************************************************************************************
//Author:       Gyouho Kim
//              ZhiYoong Foo
//Description:  Derived from zhiyoong_Pblr.c and Tstack_ondemand_temp_radio.c
//              Moving towards Mouse Implantation
//				Revision 1.8
//				- For 2015 APR Tapeout: SNSv5 with Wanyeong's CDC (CDCW)
//****************************************************************************************************
#include "mbus.h"
#include "PRCv9.h"
#include "SNSv5.h"
#include "HRVv1.h"
#include "RADv5.h"

// uncomment this for debug mbus message
#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// uncomment this to only transmit average
//#define TX_AVERAGE

// Stack order  PRC->RAD->SNS
#define SNS_ADDR 0x5
#define HRV_ADDR 0x6
#define RAD_ADDR 0x4

// CDC parameters
#define	MBUS_DELAY 200 //Amount of delay between successive messages
#define	LDO_DELAY 500 // 1000: 150msec
#define CDC_TIMEOUT_COUNT 500 // <<< 500 or 1000
#define WAKEUP_PERIOD_RESET 2
#define WAKEUP_PERIOD_LDO 1

// Pstack states
#define	PSTK_IDLE       0x0
#define PSTK_TEMP_START 0x1
#define PSTK_TEMP_READ  0x3
#define PSTK_LDO1   	0x4
#define PSTK_LDO2  	 	0x5

// Radio configurations
#define RAD_BIT_DELAY       13     //40      //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 	2000      //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define RAD_PACKET_NUM      1      //How many times identical data will be TXed

// CDC configurations
#define NUM_SAMPLES         3      //Number of CDC samples to take (only 2^n allowed for averaging: 2, 4, 8, 16...)
#define NUM_SAMPLES_TX      1      //Number of CDC samples to be TXed (processed by process_data)
#define NUM_SAMPLES_2PWR    0      //NUM_SAMPLES = 2^NUM_SAMPLES_2PWR - used for averaging

#define CDC_STORAGE_SIZE 0  

//***************************************************
// Global variables
//***************************************************
	//Test Declerations
	// "static" limits the variables to this file, giving compiler more freedom
	// "volatile" should only be used for MMIO
	static uint32_t enumerated;
	static uint8_t Pstack_state;
	static uint32_t cdc_data[NUM_SAMPLES];
	static uint32_t cdc_data_tx[NUM_SAMPLES_TX];
	static uint32_t cdc_data_index;
	static uint32_t cdc_reset_timeout_count;
	static uint32_t exec_count;
	static uint32_t meas_count;
	static uint32_t exec_count_irq;
  	static uint32_t exec_temp_marker;
	static uint32_t MBus_msg_flag;
	volatile snsv5_r0_t snsv5_r0 = SNSv5_R0_DEFAULT;
	volatile snsv5_r1_t snsv5_r1 = SNSv5_R1_DEFAULT;
	volatile snsv5_r2_t snsv5_r2 = SNSv5_R2_DEFAULT;
	volatile snsv5_r14_t snsv5_r14 = SNSv5_R14_DEFAULT;
	volatile snsv5_r15_t snsv5_r15 = SNSv5_R15_DEFAULT;
	volatile snsv5_r18_t snsv5_r18 = SNSv5_R18_DEFAULT; // For LDO's
  
	static uint32_t WAKEUP_PERIOD_CONT; 
	static uint32_t WAKEUP_PERIOD_CONT_INIT; 

	static uint32_t cdc_storage[CDC_STORAGE_SIZE] = {0};
	static uint32_t cdc_storage_count;
	static uint32_t radio_tx_count;
	static uint32_t radio_tx_option;
	static uint32_t radio_tx_numdata;

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

//***************************************************
// Radio Transmission SECDED for 16 Bits Data + Preamble
//***************************************************
//Radio Preamble = 4  Bits; [25:22]
//Data 		 = 16 Bits; [21:6]
//SECDEC	 = 5  Bits + 1 Bit; [5:0]
//Radio Length	 = 26 Bits; [25:0]
//ECC Algorithm: (P = parity) (D = data)
//SEC requires 5 bits
//DED requires 1 additional for total parity check
//SEC Section:
//P[0] :0 : 00000 = 1^2^3^4^5^6^7^8^9^10^11^12^13^14^15^16^17^18^19^20^21
//P[1] :1 : 00001 = 3^5^7^9^11^13^15^17^19^21
//P[2] :2 : 00010 = 3^6^7^10^11^14^15^18^19
//D[0] :3 : 00011
//P[3] :4 : 00100 = 5^6^7^12^13^14^15^20^21
//D[1] :5 : 00101
//D[2] :6 : 00110
//D[3] :7 : 00111
//P[4] :8 : 01000 = 9^10^11^12^13^14^15
//D[4] :9 : 01001
//D[5] :10: 01010
//D[6] :11: 01011
//D[7] :12: 01100
//D[8] :13: 01101
//D[9] :14: 01110
//D[10]:15: 01111
//P[5] :16: 10000 = 17^18^19^20^21
//D[11]:17: 10001
//D[12]:18: 10010
//D[13]:19: 10011
//D[14]:20: 10100
//D[15]:21: 10101
static uint32_t gen_radio_data(uint32_t data_in) {
  uint32_t data_out =  0x3C00000 | data_in<<6;
  uint32_t P5 = 
    ((data_in>>15)&0x1) ^ 
    ((data_in>>14)&0x1) ^ 
    ((data_in>>13)&0x1) ^ 
    ((data_in>>12)&0x1) ^ 
    ((data_in>>11)&0x1);
  uint32_t P4 = 
    ((data_in>>10)&0x1) ^ 
    ((data_in>>9 )&0x1) ^ 
    ((data_in>>8 )&0x1) ^ 
    ((data_in>>7 )&0x1) ^ 
    ((data_in>>6 )&0x1) ^ 
    ((data_in>>5 )&0x1) ^ 
    ((data_in>>4 )&0x1);
  uint32_t P3 = 
    ((data_in>>15)&0x1) ^ 
    ((data_in>>14)&0x1) ^ 
    ((data_in>>10)&0x1) ^ 
    ((data_in>>9 )&0x1) ^ 
    ((data_in>>8 )&0x1) ^ 
    ((data_in>>7 )&0x1) ^ 
    ((data_in>>3 )&0x1) ^ 
    ((data_in>>2 )&0x1) ^ 
    ((data_in>>1 )&0x1);
  uint32_t P2 = 
    ((data_in>>13)&0x1) ^ 
    ((data_in>>12)&0x1) ^ 
    ((data_in>>10)&0x1) ^ 
    ((data_in>>9 )&0x1) ^ 
    ((data_in>>6 )&0x1) ^ 
    ((data_in>>5 )&0x1) ^ 
    ((data_in>>3 )&0x1) ^ 
    ((data_in>>2 )&0x1) ^ 
    ((data_in>>0 )&0x1);
  uint32_t P1 = 
    ((data_in>>15)&0x1) ^ 
    ((data_in>>13)&0x1) ^ 
    ((data_in>>11)&0x1) ^ 
    ((data_in>>10)&0x1) ^ 
    ((data_in>>8 )&0x1) ^
    ((data_in>>6 )&0x1) ^ 
    ((data_in>>4 )&0x1) ^ 
    ((data_in>>3 )&0x1) ^ 
    ((data_in>>1 )&0x1) ^ 
    ((data_in>>0 )&0x1);
  uint32_t P0 = 
    ((data_in>>15)&0x1) ^ 
    ((data_in>>14)&0x1) ^ 
    ((data_in>>13)&0x1) ^ 
    ((data_in>>12)&0x1) ^ 
    ((data_in>>11)&0x1) ^ 
    ((data_in>>10)&0x1) ^ 
    ((data_in>>9)&0x1) ^ 
    ((data_in>>8)&0x1) ^ 
    ((data_in>>7)&0x1) ^ 
    ((data_in>>6)&0x1) ^ 
    ((data_in>>5)&0x1) ^ 
    ((data_in>>4)&0x1) ^ 
    ((data_in>>3)&0x1) ^ 
    ((data_in>>2)&0x1) ^ 
    ((data_in>>1)&0x1) ^ 
    ((data_in>>0)&0x1) ^ 
    P5 ^ P4 ^ P3 ^ P2 ^ P1 ;
  data_out |= (P5<<5)|(P4<<4)|(P3<<3)|(P2<<2)|(P1<<1)|(P0<<0);
  return data_out;
}

//***************************************************
//Send Radio Data MSB-->LSB
//Two Delays:
//Bit Delay: Delay between each bit (16-bit data)
//Packet Delay: Delay between each packet (3 packets)
//Delays are nop delays (therefore dependent on core speed)
//***************************************************
static void send_radio_data(uint32_t radio_data){
  int32_t i; //loop var
  uint32_t j; //loop var
  for(j=0;j<1;j++){ //Packet Loop
    for(i=25;i>=0;i--){ //Bit Loop
      delay(10);
      if ((radio_data>>i)&1) write_mbus_register(RAD_ADDR,0x27,0x1);
      else                   write_mbus_register(RAD_ADDR,0x27,0x0);
      //Must clear register
      delay(RAD_BIT_DELAY);
      write_mbus_register(RAD_ADDR,0x27,0x0);
      delay(RAD_BIT_DELAY); //Set delay between sending subsequent bit
    }
  }
}

//***************************************************
// CDCW Functions
// SNSv5
//***************************************************
static void release_cdc_pg(){
    snsv5_r0.CDCW_PG_V1P2 = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
    snsv5_r0.CDCW_PG_VBAT = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
    snsv5_r0.CDCW_PG_VLDO = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_isolate(){
    snsv5_r0.CDCW_ISO = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void assert_cdc_reset(){
    snsv5_r0.CDCW_RESETn = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_reset(){
    snsv5_r0.CDCW_RESETn = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void fire_cdc_meas(){
    snsv5_r0.CDCW_ENABLE = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_meas(){
    snsv5_r0.CDCW_ENABLE = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void ldo_power_off(){
	snsv5_r18.CDC_LDO_CDC_LDO_DLY_ENB = 0x1;
	snsv5_r18.ADC_LDO_ADC_LDO_DLY_ENB = 0x1;
	snsv5_r18.CDC_LDO_CDC_LDO_ENB = 0x1;
	snsv5_r18.ADC_LDO_ADC_LDO_ENB = 0x1;
	write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
    delay(MBUS_DELAY);
}

//***************************************************
// Temp Sensor Functions
// SNSv5
//***************************************************
static void temp_sensor_enable(){
    snsv5_r14.TEMP_SENSOR_ENABLEb = 0;
    write_mbus_register(SNS_ADDR,0xE,snsv5_r14.as_int);
    delay(MBUS_DELAY);
}
static void temp_sensor_disable(){
    snsv5_r14.TEMP_SENSOR_ENABLEb = 1;
    write_mbus_register(SNS_ADDR,0xE,snsv5_r14.as_int);
    delay(MBUS_DELAY);
}
static void temp_sensor_release_reset(){
    snsv5_r14.TEMP_SENSOR_RESETn = 1;
    snsv5_r14.TEMP_SENSOR_ISO = 0;
    write_mbus_register(SNS_ADDR,0xE,snsv5_r14.as_int);
    delay(MBUS_DELAY);
}
static void temp_sensor_assert_reset(){
    snsv5_r14.TEMP_SENSOR_RESETn = 0;
    snsv5_r14.TEMP_SENSOR_ISO = 1;
    write_mbus_register(SNS_ADDR,0xE,snsv5_r14.as_int);
    delay(MBUS_DELAY);
}




inline static void set_pmu_sleep_clk_low(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F77003B; // 0x8F77003B: use GOC x0.6-2
}
inline static void set_pmu_sleep_clk_high(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F77004B; // 0x8F77004B: use GOC x10-25
}
static void process_data(){
    uint8_t i;
    uint8_t j;
    uint32_t temp;
    // bubble sort
    for( i=0; i<NUM_SAMPLES; ++i ){
        for( j=i+1; j<NUM_SAMPLES; ++j){
            if( cdc_data[j] > cdc_data[i] ){
                temp = cdc_data[i];
                cdc_data[i] = cdc_data[j];
                cdc_data[j] = temp;
            }
        }
    }

    // Assuming NUM_SAMPLES = 3, NUM_SAMPLES_TX = 1
    cdc_data_tx[0] = cdc_data[1];

}

//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){

  // Reset wakeup counter
  // This is required to go back to sleep!!
  //set_wakeup_timer (0xFFF, 0x0, 0x1);
  *((volatile uint32_t *) 0xA2000014) = 0x1;

  // Reset IRQ10VEC
  *((volatile uint32_t *) IRQ10VEC/*IMSG0*/) = 0;

  // Go to Sleep
  sleep();
  while(1);

}

static void operation_sleep_noirqreset(void){

  // Reset wakeup counter
  // This is required to go back to sleep!!
  *((volatile uint32_t *) 0xA2000014) = 0x1;

  // Go to Sleep
  sleep();
  while(1);

}

static void operation_sleep_notimer(void){
    
  // Disable Timer
  set_wakeup_timer (0, 0x0, 0x0);
  // Go to sleep without timer
  operation_sleep();

}

static void operation_tx_cdc_results(){
    uint32_t i; 
    uint32_t average;
#ifdef TX_AVERAGE
    average = 0;
    for (i=0; i<NUM_SAMPLES; i++){
        average += (cdc_data[i]>>2);
    }
    average = average >> NUM_SAMPLES_2PWR;  //(avoiding division operation)
    send_radio_data(gen_radio_data(average));
#else
    for (i=0; i<NUM_SAMPLES_TX; i++){
    #if NUM_SAMPLES_TX>1
        delay(RAD_PACKET_DELAY);
    #endif
        send_radio_data(gen_radio_data(cdc_data_tx[i]));
    }
#endif     
}

static void operation_tx_stored(void){

	//Fire off stored data to radio
	uint32_t data = gen_radio_data(cdc_storage[radio_tx_count]);
	delay(MBUS_DELAY);

	send_radio_data(data);
	delay(RAD_PACKET_DELAY*3); //Set delays between sending subsequent packet
	send_radio_data(data);

	if (((!radio_tx_numdata)&&(radio_tx_count > 0)) | ((radio_tx_numdata)&&((radio_tx_numdata+radio_tx_count) > cdc_storage_count))){
		radio_tx_count--;
		// set timer
		set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
		// go to sleep and wake up with same condition
		operation_sleep_noirqreset();

	}else{
		delay(RAD_PACKET_DELAY*3); //Set delays between sending subsequent packet
		send_radio_data(0x3EBE800);

		// This is also the end of this IRQ routine
		exec_count_irq = 0;
		// Go to sleep without timer
		radio_tx_count = cdc_storage_count; // allows data to be sent more than once
		operation_sleep_notimer();
	}

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
    Pstack_state = PSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEF;
    cdc_data_index = 0;
    exec_count = 0;
    exec_count_irq = 0;
    MBus_msg_flag = 0;
    //Enumeration
    enumerate(RAD_ADDR);
    delay(MBUS_DELAY*10);
    enumerate(SNS_ADDR);
    delay(MBUS_DELAY*10);
    enumerate(HRV_ADDR);
    delay(MBUS_DELAY*10);

    // Temp Sensor Settings --------------------------------
    // SNSv5_R14
	snsv5_r14.TEMP_SENSOR_DELAY_SEL = 7;
    write_mbus_register(SNS_ADDR,0xE,snsv5_r14.as_int);
    delay(MBUS_DELAY);
    // SNSv5_R15
	snsv5_r15.TEMP_SENSOR_SEL_CT = 7;
    write_mbus_register(SNS_ADDR,0xF,snsv5_r15.as_int);
    delay(MBUS_DELAY);

    // SNSv5_R18
    snsv5_r18.CDC_LDO_CDC_LDO_ENB      = 0x1;
    snsv5_r18.CDC_LDO_CDC_LDO_DLY_ENB  = 0x1;
    snsv5_r18.ADC_LDO_ADC_LDO_ENB      = 0x1;
    snsv5_r18.ADC_LDO_ADC_LDO_DLY_ENB  = 0x1;
    snsv5_r18.CDC_LDO_CDC_CURRENT_2X  = 0x0;
    snsv5_r18.ADC_LDO_ADC_CURRENT_2X  = 0x1;

    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv5_r18.ADC_LDO_ADC_VREF_MUX_SEL = 0x3;
    snsv5_r18.ADC_LDO_ADC_VREF_SEL     = 0x20;

    // Set CDC LDO to around 1.03V: 0x0//0x20
    snsv5_r18.CDC_LDO_CDC_VREF_MUX_SEL = 0x0;
    snsv5_r18.CDC_LDO_CDC_VREF_SEL     = 0x20;

    write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);


    // Radio Settings --------------------------------------
    radv5_r23_t radv5_r23; //Ext Ctrl En
    radv5_r26_t radv5_r26; //Current Limiter 2F = 30uA, 1F = 3uA
    radv5_r20_t radv5_r20; //Tune Power
    radv5_r21_t radv5_r21; //Tune Freq 1
    radv5_r22_t radv5_r22; //Tune Freq 2
    radv5_r25_t radv5_r25; //Tune TX Time
    radv5_r27_t radv5_r27; //Zero the TX register
    //RADv5 R23
    radv5_r23.RADIO_EXT_CTRL_EN_1P2 = 0x0; //Ext Ctrl En
    write_mbus_register(RAD_ADDR,0x23,radv5_r23.as_int);
    delay(MBUS_DELAY);
    //RADv5 R26
    radv5_r26.RADIO_TUNE_CURRENT_LIMITER_1P2 = 0x1F; //Current Limiter 2F = 30uA, 1F = 3uA
    write_mbus_register(RAD_ADDR,0x26,radv5_r26.as_int);
    delay(MBUS_DELAY);
    //RADv5 R20
    radv5_r20.RADIO_TUNE_POWER_1P2 = 0x1F; //Tune Power 0x1F = Full Power
    write_mbus_register(RAD_ADDR,0x20,radv5_r20.as_int);
    delay(MBUS_DELAY);
    //RADv5 R21
    radv5_r21.RADIO_TUNE_FREQ1_1P2 = 0x0; //Tune Freq 1
    write_mbus_register(RAD_ADDR,0x21,radv5_r21.as_int);
    delay(MBUS_DELAY);
    //RADv5 R22
    radv5_r22.RADIO_TUNE_FREQ2_1P2 = 0x0; //Tune Freq 2 //0x0,0x0 = 902MHz on Pblr005
    write_mbus_register(RAD_ADDR,0x22,radv5_r22.as_int);
    delay(MBUS_DELAY);
    //RADv5 R25
    radv5_r25.RADIO_TUNE_TX_TIME_1P2 = 0x5; //Tune TX Time
    write_mbus_register(RAD_ADDR,0x25,radv5_r25.as_int);
    delay(MBUS_DELAY);
    //RADv5 R27
    radv5_r27.RADIO_DATA_1P2 = 0x0; //Zero the TX register
    write_mbus_register(RAD_ADDR,0x27,radv5_r27.as_int);
  
    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 3;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 1;   // 0x1E (30): ~1 min with PRCv9
    cdc_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0;
    
    // Go to sleep without timer
    //operation_sleep_notimer();
}


//***************************************************
// Temperature measurement operation (SNSv5)
//***************************************************
static void operation_temp(void){
    uint32_t count; 

	if (Pstack_state == PSTK_IDLE){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x0);
		#endif
		Pstack_state = PSTK_LDO1;

		cdc_reset_timeout_count = 0;

		snsv5_r18.CDC_LDO_CDC_LDO_ENB = 0x0;
		snsv5_r18.ADC_LDO_ADC_LDO_ENB = 0x0;
		write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
		// Long delay required here
		// Put system to sleep
		set_wakeup_timer (WAKEUP_PERIOD_LDO, 0x1, 0x0);
		operation_sleep();


	}else if (Pstack_state == PSTK_LDO1){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x1);
		#endif
		Pstack_state = PSTK_LDO2;
		snsv5_r18.CDC_LDO_CDC_LDO_DLY_ENB = 0x0;
		write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
		delay(LDO_DELAY); // This delay is required to avoid current spike
		snsv5_r18.ADC_LDO_ADC_LDO_DLY_ENB = 0x0;
		write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
		// Put system to sleep
		set_wakeup_timer (WAKEUP_PERIOD_LDO, 0x1, 0x0);
		operation_sleep();
    
	}else if (Pstack_state == PSTK_LDO2){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x2);
		#endif
		Pstack_state = PSTK_TEMP_START;

		// Release Temp Sensor Reset
		temp_sensor_release_reset();

	}else if (Pstack_state == PSTK_TEMP_START){
		// Start temp measurement
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x11111111);
			delay(MBUS_DELAY);
		#endif

		MBus_msg_flag = 0;
		temp_sensor_enable();
		for( count=0; count<CDC_TIMEOUT_COUNT; count++ ){
			if( MBus_msg_flag ){
				MBus_msg_flag = 0;
				cdc_reset_timeout_count = 0;
				Pstack_state = PSTK_TEMP_READ;
				return;
			}else{
				delay(MBUS_DELAY);
			}
		}

		// Time out
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0xFAFAFAFA);
		#endif

		temp_sensor_disable();

		if (cdc_reset_timeout_count > 0){
			cdc_reset_timeout_count++;
			temp_sensor_assert_reset();

			if (cdc_reset_timeout_count > 10){
				// CDC is not resetting for some reason. Go to sleep forever
				Pstack_state = PSTK_IDLE;
				ldo_power_off();
				operation_sleep_notimer();
			}else{
				// Put system to sleep to reset the layer controller
				Pstack_state = PSTK_LDO2;
				set_wakeup_timer (WAKEUP_PERIOD_RESET, 0x1, 0x0);
				operation_sleep();
			}

	    }else{
		// Try one more time
	    	cdc_reset_timeout_count++;
			assert_cdc_reset();
			delay(MBUS_DELAY*5);
	    }

	}else if (Pstack_state == PSTK_TEMP_READ){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x22222222);
		#endif

		// FIXME
		uint32_t read_data_regD;
		read_mbus_register(SNS_ADDR,0x10,0x15);
		delay(MBUS_DELAY);
		read_data_regD = *((volatile uint32_t *) 0xA0001014);

		delay(MBUS_DELAY*10);
		delay(MBUS_DELAY*10);
		write_mbus_message(0xAA, 0xF1);
		delay(MBUS_DELAY*20); 
		write_mbus_message(0x73, 1);
		delay(MBUS_DELAY*40);//
		write_mbus_message(0x73, 1);
		delay(MBUS_DELAY*40);//
		write_mbus_message(0x74, (read_data_regD&(~(1<<18))));
		delay(MBUS_DELAY*40);//
		write_mbus_message(0x75, 3);
		delay(MBUS_DELAY*40);//
		write_mbus_message(0x76, 4);
		delay(MBUS_DELAY*20);
		delay(MBUS_DELAY*20);

		// Reset exec_temp_marker
		exec_temp_marker = 0;

		if (meas_count < 310){	
			meas_count++;

			// Repeat measurement while awake
			temp_sensor_disable();
			delay(MBUS_DELAY);
			Pstack_state = PSTK_TEMP_START;
			

		}else{
			meas_count = 0;
			// Finalize CDC operation
			temp_sensor_disable();
			temp_sensor_assert_reset();
			exec_count++;
			Pstack_state = PSTK_IDLE;
			#ifdef DEBUG_MBUS_MSG
				write_mbus_message(0xAA, 0x33333333);
			#endif
		
			// Assert CDC isolation & turn off CDC power
			ldo_power_off();

			// Enter long sleep
			set_wakeup_timer (WAKEUP_PERIOD_CONT, 0x1, 0x0);
			operation_sleep();
		}

	}else{
        //default:  // THIS SHOULD NOT HAPPEN
		// Reset CDC
		temp_sensor_assert_reset();
		ldo_power_off();
		operation_sleep_notimer();
	}

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
	// FIXME
	config_timer( 0, 0, 0, 0, 1000000 );

    // Initialization sequence
	//FIXME
    if (enumerated != 0xDEADBEEF){
        // Set up PMU/GOC register in PRC layer (every time)
        // Enumeration & RAD/SNS layer register configuration
        exec_count = 0;
		meas_count = 0;
        operation_init();
    }

    // Proceed to continuous mode
    while(1){
        operation_temp();
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

