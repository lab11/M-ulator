//****************************************************************************************************
//Author:       Gyouho Kim
//              ZhiYoong Foo
//				Yoonmyung Lee
//Description:  Derived from zhiyoong_Pblr.c and Tstack_ondemand_temp_radio.c
//              Moving towards Mouse Implantation
//				Revision 1.6
//				- Adding updates from SNSv3_test_v3.c
//				Revision 1.5
//				- Using SNSv3E instead of SNSv2
//              - Adding ADC functionality
//              - CDC with LDO
//              Revision 1.4
//              - Now pressure measurements are stored and radioed out only when triggered
//				Revision 1.3.2
// 				- Adding feature to change program parameter through GOC IRQ
//				Revision 1.3.1
// 				- Using PRCv9, but SNSv2 not SNSv3
//              Revision 1.3
//              - System stays awake during CDC reset/measurement
//              - Data processing function added  (process_data)
//              Revision 1.2
//              - Rename states / subfunctions for readability
//              - state machine merged to operation_cdc_run();
//              - Take average for measurement result TX (#TX_AVERAGE)
//              - Debug flags (#DEBUG_MBUS_MSG / #DEBUG_RADIO_MSG)
//              Revision 1.1 - optimization for implant
//              - HRV layer related configuration added
//              - Removed unnecessary debug functions
//              - Multiple CDC measurement and averaging
//Stack Diagram:\m3_shared\m3_orders\Order to Advotech\141016 - Pstack with 2014JUN+SNSv2+250umSOLv4\
//****************************************************************************************************
#include "mbus.h"
#include "PRCv9.h"
#include "SNSv3.h"
#include "HRVv1.h"
#include "RADv5.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// uncomment this to only transmit average
//#define TX_AVERAGE

// Stack order  PRC->SNS->HRV->RAD
#define SNS_ADDR 0x5
#define HRV_ADDR 0x6
#define RAD_ADDR 0x4

// CDC parameters
#define	MBUS_DELAY 100 //Amount of delay between successive messages
#define	LDO_DELAY 500 // 1000: 150msec
#define CDC_TIMEOUT_COUNT 100
#define WAKEUP_PERIOD_RESET 2
#define WAKEUP_PERIOD_LDO 1

// Pstack states
#define	PSTK_IDLE       0x0
#define PSTK_CDC_RST    0x1
#define PSTK_CDC_MEAS   0x2
#define PSTK_CDC_READ   0x3
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

#define CDC_STORAGE_SIZE 10  

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
	static uint32_t exec_count_irq;
	static uint32_t MBus_msg_flag;
	volatile snsv3_r0_t snsv3_r0;
	volatile snsv3_r1_t snsv3_r1;
	volatile snsv3_r7_t snsv3_r7;
  
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

static void assert_cdc_reset(){
    snsv3_r0.CDC_EXT_RESET = 0x1;	// Assert reset
    snsv3_r0.CDC_CLK = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
}
static void release_cdc_reset(){
    snsv3_r0.CDC_EXT_RESET = 0x0;	// Release reset
    snsv3_r0.CDC_CLK = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
}
static void fire_cdc_meas(){
    snsv3_r0.CDC_EXT_RESET = 0x0;
    snsv3_r0.CDC_CLK = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
}
static void release_cdc_meas(){
    snsv3_r0.CDC_EXT_RESET = 0x0;
    snsv3_r0.CDC_CLK = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
}
static void cdc_ldo_off(){
	snsv3_r7.CDC_LDO_CDC_LDO_DLY_ENB = 0x1;
	snsv3_r7.ADC_LDO_ADC_LDO_DLY_ENB = 0x1;
	snsv3_r7.CDC_LDO_CDC_LDO_ENB = 0x1;
	snsv3_r7.ADC_LDO_ADC_LDO_ENB = 0x1;
	write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
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
    *((volatile uint32_t *) 0xA2000008) = 0x00202908;
  
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

    // CDC Settings --------------------------------------
    // SNSv3_R7
    snsv3_r7.CDC_LDO_CDC_LDO_ENB      = 0x1;
    snsv3_r7.CDC_LDO_CDC_LDO_DLY_ENB  = 0x1;
    snsv3_r7.ADC_LDO_ADC_LDO_ENB      = 0x1;
    snsv3_r7.ADC_LDO_ADC_LDO_DLY_ENB  = 0x1;

    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv3_r7.ADC_LDO_ADC_VREF_MUX_SEL = 0x3;
    snsv3_r7.ADC_LDO_ADC_VREF_SEL     = 0x20;

    // Set CDC LDO to around 1.03V: 0x0//0x20
    snsv3_r7.CDC_LDO_CDC_VREF_MUX_SEL = 0x0;
    snsv3_r7.CDC_LDO_CDC_VREF_SEL     = 0x20;

    snsv3_r7.LC_CLK_CONF              = 0x9; // default = 0x9
    write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);

    // SNSv3_R1
    snsv3_r1.CDC_S_period = 0x1A;
    snsv3_r1.CDC_R_period = 0xC;
    snsv3_r1.CDC_CR_ext = 0x0;
    write_mbus_register(SNS_ADDR,1,snsv3_r1.as_int);


    // SNSv3_R0 (need to be initialized here)
    snsv3_r0.CDC_ext_select_CR = 0x0;
    snsv3_r0.CDC_max_select = 0x7;
    snsv3_r0.CDC_ext_max = 0x0;
    snsv3_r0.CDC_CR_fixB = 0x1;
    snsv3_r0.CDC_ext_clk = 0x0;
    snsv3_r0.CDC_outck_in = 0x1;
    snsv3_r0.CDC_on_adap = 0x1;		//0x0 (default 0x1)
    snsv3_r0.CDC_s_recycle = 0x1;	//0x1 (default 0x4)
    snsv3_r0.CDC_Td = 0x0;
    snsv3_r0.CDC_OP_on = 0x0;
    snsv3_r0.CDC_Bias_2nd = 0x7;
    snsv3_r0.CDC_Bias_1st = 0x7;
    snsv3_r0.CDC_EXT_RESET = 0x1;
    snsv3_r0.CDC_CLK = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);

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
    WAKEUP_PERIOD_CONT = 100;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 1;   // 0x1E (30): ~1 min with PRCv9
    cdc_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0;
    
    // Go to sleep without timer
    operation_sleep_notimer();
}

static void operation_cdc_run(){
    uint32_t read_data;
    uint32_t count; 

	if (Pstack_state == PSTK_IDLE){
		Pstack_state = PSTK_LDO1;

		cdc_reset_timeout_count = 0;

		snsv3_r7.CDC_LDO_CDC_LDO_ENB = 0x0;
		snsv3_r7.ADC_LDO_ADC_LDO_ENB = 0x0;
		write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
		// Long delay required here
		// Put system to sleep
		set_wakeup_timer (WAKEUP_PERIOD_LDO, 0x1, 0x0);
		operation_sleep();


	}else if (Pstack_state == PSTK_LDO1){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x10101010);
		#endif
		Pstack_state = PSTK_LDO2;
		snsv3_r7.CDC_LDO_CDC_LDO_DLY_ENB = 0x0;
		write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
		delay(LDO_DELAY); // This delay is required to avoid current spike
		snsv3_r7.ADC_LDO_ADC_LDO_DLY_ENB = 0x0;
		write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
		// Put system to sleep
		set_wakeup_timer (WAKEUP_PERIOD_LDO, 0x1, 0x0);
		operation_sleep();
    
	}else if (Pstack_state == PSTK_LDO2){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x20202020);
		#endif
		Pstack_state = PSTK_CDC_RST;

		// Release CDC isolation
		snsv3_r0.CDC_on_adap = 0x0; // This is used for isolation after FIB
		write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);

	}else if (Pstack_state == PSTK_CDC_RST){
		// Release reset
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x11111111);
		#endif
		MBus_msg_flag = 0;
		release_cdc_reset();
		for( count=0; count<CDC_TIMEOUT_COUNT; count++ ){
			if( MBus_msg_flag ){
				MBus_msg_flag = 0;
				cdc_reset_timeout_count = 0;
				Pstack_state = PSTK_CDC_MEAS;
				//NEW: This makes measurement more stable
				set_wakeup_timer (1, 0x1, 0x0);
				operation_sleep();
				return;
			}
			else{
				delay(30);
			}
		}

		// Reset Time out
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0xFAFAFAFA);
		#endif
		if (cdc_reset_timeout_count > 0){
		
			write_mbus_message(0xAA, 0xFFFFFFFF);
			cdc_reset_timeout_count++;
			assert_cdc_reset();

			if (cdc_reset_timeout_count > 10){
				// CDC is not resetting for some reason. Go to sleep forever
				Pstack_state = PSTK_IDLE;
				cdc_ldo_off();
				operation_sleep_notimer();
			}else{
				// Put system to sleep to reset the layer controller
				set_wakeup_timer (WAKEUP_PERIOD_RESET, 0x1, 0x0);
				operation_sleep();
			}

	    }else{
		// Try one more time
	    	cdc_reset_timeout_count++;
			assert_cdc_reset();
			delay(MBUS_DELAY*10);
	    }

	}else if (Pstack_state == PSTK_CDC_MEAS){
		// case PSTK_CDC_MEAS:
		// If no time out 
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x22222222);
			write_mbus_message(0xAA, 0x22222222);
			write_mbus_message(0xAA, 0x22222222);
		#endif

		Pstack_state = PSTK_CDC_READ;
		fire_cdc_meas();
		
	}else if (Pstack_state == PSTK_CDC_READ){
		//	case PSTK_CDC_READ:
		for( count=0; count<CDC_TIMEOUT_COUNT; count++ ){
			if( MBus_msg_flag ){
				#ifdef DEBUG_MBUS_MSG
					write_mbus_message(0xAA, 0x33333333);
					write_mbus_message(0xAA, 0x33333333);
					write_mbus_message(0xAA, 0x33333333);
				#endif
				MBus_msg_flag = 0;
				read_data = *((volatile uint32_t *) 0xA0001014);
				if( read_data & 0x02 ) {
					// If CDC data is valid
					// Process Data
					// CDC_DOUT is shifted to start at [0]
					// CR_DATA is moved to start at [24]
					// Valid bit is moved to [28]
					uint32_t read_data_modified;
					read_data_modified = read_data>>5;
					read_data_modified = read_data_modified | ((0x2 & read_data)<<27);
					read_data_modified = read_data_modified | ((0x1C & read_data)<<22);

					cdc_data[cdc_data_index] = read_data_modified;
					++cdc_data_index;

					if( cdc_data_index == NUM_SAMPLES ){
						// Collected all data for radio TX
						// FIXME: comment out ifdef here to observe cdc data on MBUS
						//#ifdef DEBUG_MBUS_MSG
							write_mbus_message(0xAA, 0x44444444);
							delay(MBUS_DELAY);
							uint32_t i, j;
							for( j=0; j<1; ++j){
								for( i=0; i<NUM_SAMPLES; ++i){
									delay(MBUS_DELAY*10);
									write_mbus_message(0x77, cdc_data[i]);
								}
							}
						//#endif
						exec_count++;
						process_data();

						// Store in memory; unless buffer is full
						if (cdc_storage_count < CDC_STORAGE_SIZE){
							cdc_storage[cdc_storage_count] = cdc_data_tx[0];
							cdc_storage_count++;
							radio_tx_count = cdc_storage_count;
						}

						// Optionally transmit the data
						if (radio_tx_option){
							operation_tx_cdc_results();
						}
						cdc_data_index = 0;
						
						// Finalize CDC operation
						assert_cdc_reset();
						release_cdc_meas();
						Pstack_state = PSTK_IDLE;
						#ifdef DEBUG_MBUS_MSG
							write_mbus_message(0xAA, 0x55555555);
							//read_mbus_register(SNS_ADDR,0,0x10);
						#endif

						// Assert CDC isolation
						// FIXME: somehow if I enable the following two lines,
						// the CDC code becomes stuck at ~0E11 and CR 6!!!!!
						//snsv3_r0.CDC_on_adap = 0x1; // This is used for isolation after FIB
						//write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);

						// Turn off LDO's
						cdc_ldo_off();
									
						// Enter long sleep
						if(exec_count < 8){ 
							// Send some signal
							send_radio_data(0x3EBE800);
							set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
						}else{
							set_wakeup_timer (WAKEUP_PERIOD_CONT, 0x1, 0x0);
						}
						operation_sleep();

					}else{ // cdc_data_index == NUMSAMPLES
						// Need more data for radio TX
						release_cdc_meas();
						Pstack_state = PSTK_CDC_MEAS;

/*
						// Reset CDC and re-measure
						Pstack_state = PSTK_CDC_RST;
						release_cdc_meas();
						assert_cdc_reset();
						delay(MBUS_DELAY);
*/
						return;
					}
				}else{ // If CDC data invalid
					#ifdef DEBUG_MBUS_MSG
						write_mbus_message(0xAA, 0x3333FFFF);
						write_mbus_message(0xAA, 0x3333FFFF);
						write_mbus_message(0xAA, 0x3333FFFF);
					#endif
					release_cdc_meas();
					Pstack_state = PSTK_CDC_RST;
					release_cdc_meas();
					assert_cdc_reset();
					delay(MBUS_DELAY);
					return;
				}
			}else{
				delay(30);
			}
		} // for

		// Measurement Time out
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0xFFFFBBBB);
		#endif
		Pstack_state = PSTK_CDC_RST;
		release_cdc_meas();
		assert_cdc_reset();
		delay(MBUS_DELAY);

		// Put system to sleep to reset the layer controller
		set_wakeup_timer (WAKEUP_PERIOD_RESET, 0x1, 0x0);
		operation_sleep();

	}else{
        //default:  // THIS SHOULD NOT HAPPEN
		// Reset CDC
		Pstack_state = PSTK_IDLE;
		assert_cdc_reset();
		set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
		operation_sleep();
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
	config_timer( 0, 1, 0, 0, 1000000 );

    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
        // Set up PMU/GOC register in PRC layer (every time)
        // Enumeration & RAD/SNS layer register configuration
        operation_init();
    }

    // Check if wakeup is due to GOC interrupt  
    // 0x68 is reserved for GOC-triggered wakeup (Named IRQ10VEC)
    // 8 MSB bits of the wakeup data are used for function ID
    uint32_t wakeup_data = *((volatile uint32_t *) IRQ10VEC);
    uint32_t wakeup_data_header = wakeup_data>>24;
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;
    uint32_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
    uint32_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;

    if(wakeup_data_header == 1){
        // Debug mode: Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
        delay(MBUS_DELAY);
        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
            // radio
            send_radio_data(0x3EBE800+exec_count_irq);	
            // set timer
            set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
            // go to sleep and wake up with same condition
            operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
            // radio
            send_radio_data(0x3EBE800+exec_count_irq);	
            // Go to sleep without timer
            operation_sleep_notimer();
        }
   
    }else if(wakeup_data_header == 2){
		// Slow down PMU sleep osc and run CDC code with desired wakeup period
        // wakeup_data[15:0] is the user-specified period
        // wakeup_data[24:16] is the initial user-specified period
    	WAKEUP_PERIOD_CONT = wakeup_data_field_0 + (wakeup_data_field_1<<8);
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_2;
        radio_tx_option = 0;

        set_pmu_sleep_clk_low();
        delay(MBUS_DELAY);
        exec_count = 0;
        cdc_storage_count = 0;

		// Reset IRQ10VEC
		*((volatile uint32_t *) IRQ10VEC) = 0;

        // Run CDC Program
		cdc_reset_timeout_count = 0;
        operation_cdc_run();

    }else if(wakeup_data_header == 3){
		// Stop CDC program and transmit the execution count n times
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        // wakeup_data[24:16] indicates whether or not to speed up PMU sleep clock
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

		if (wakeup_data_field_2){
			// Speed up PMU sleep osc
			set_pmu_sleep_clk_high();
		}

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
            // radio
            //send_radio_data(0x3EBE800+exec_count_irq);	
			//delay(MBUS_DELAY*10);
            send_radio_data(0x3EBE800+exec_count);	
            // set timer
            set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
            // go to sleep and wake up with same condition
            operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
            // Go to sleep without timer
            operation_sleep_notimer();
        }

    }else if(wakeup_data_header == 4){
        // Transmit the stored temp data
        // wakeup_data[7:0] is the # of data to transmit; if zero, all stored data is sent
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

		radio_tx_numdata = wakeup_data_field_0;
		// Make sure the requested numdata makes sense
		if (radio_tx_numdata >= cdc_storage_count){
			radio_tx_numdata = 0;
		}
    
        if (exec_count_irq < 3){
          exec_count_irq++;
          // radio
          send_radio_data(0x3EBE800+exec_count_irq);
    
          // set timer
          set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
          // go to sleep and wake up with same condition
          operation_sleep_noirqreset();
    
        }else{
          operation_tx_stored();
		}

/* FIXME
    }else if(wakeup_data_header == 5){
        // Same as operation #2, but transmitting radio every cdc measurement
		// Slow down PMU sleep osc and run CDC code with desired wakeup period
        // wakeup_data[15:0] is the user-specified period
        // wakeup_data[24:16] is the initial user-specified period
    	WAKEUP_PERIOD_CONT = wakeup_data_field_0 + (wakeup_data_field_1<<8);
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_2;
        radio_tx_option = 1;

        set_pmu_sleep_clk_low();
        delay(MBUS_DELAY*10);
        exec_count = 0;
        cdc_storage_count = 0;
		
		// Reset IRQ10VEC
		*((volatile uint32_t *) IRQ10VEC) = 0;

        // Run CDC Program
		cdc_reset_timeout_count = 0;
        operation_cdc_run();
    
	}else if(wakeup_data_header == 6){
        // Same as operation #5, but w/o slowing down the PMU
        // wakeup_data[15:0] is the user-specified period
        // wakeup_data[24:16] is the initial user-specified period
    	WAKEUP_PERIOD_CONT = wakeup_data_field_0 + (wakeup_data_field_1<<8);
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_2;
        radio_tx_option = 1;

        exec_count = 0;
        cdc_storage_count = 0;

		// Reset IRQ10VEC
		*((volatile uint32_t *) IRQ10VEC) = 0;

        // Run CDC Program
		cdc_reset_timeout_count = 0;
        operation_cdc_run();

    }else if(wakeup_data_header == 0x10){
		// Speed up PMU sleep osc and go to sleep for further programming
        set_pmu_sleep_clk_high();
        // Go to sleep without timer
        operation_sleep_notimer();

*/
    }else if(wakeup_data_header == 0x11){
		// Slow down PMU sleep osc and go to sleep for further programming
        set_pmu_sleep_clk_low();
        // Go to sleep without timer
        operation_sleep_notimer();

/*
    }else if(wakeup_data_header == 0x12){
		// Change the sleep period
        // wakeup_data[15:0] is the user-specified period
		// wakeup_data[23:16] determines whether to resume CDC operation or not
    	WAKEUP_PERIOD_CONT = wakeup_data_field_0 + (wakeup_data_field_1<<8);
		
		if (wakeup_data_field_2){
			// Resume CDC operation
			operation_cdc_run();
		}else{
            // Go to sleep without timer
            operation_sleep_notimer();
		}
*/
    }

    // Proceed to continuous mode
    while(1){
        operation_cdc_run();
    }

    // Should not reach here
    Pstack_state = PSTK_IDLE;
    assert_cdc_reset();
    operation_sleep_notimer();

    while(1);
}

