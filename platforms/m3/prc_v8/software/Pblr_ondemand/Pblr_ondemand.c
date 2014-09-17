//*******************************************************************
//Author: ZhiYoong Foo
//        Gyouho Kim
//        Yoonmyung Lee
//Description:  Derived from zhiyoong_Pblr.c and Tstack_ondemand_temp_radio.c
//		Moving towards Mouse Implantation
//Stacking Diagram: 	\m3_shared\m3_orders\Order to Advotech\140317 - Pblr\
//			Pblr Stack Assembly Plan (BAT-CTR-DCP-SNS-RAD-SOL).pdf
//*******************************************************************
#include "mbus.h"
#include "PRCv8.h"
#include "SNSv2.h"
#include "RADv5.h"

#define RAD_ADDR 0x4
#define SNS_ADDR 0x5

#define NUM_SAMPLES 1           //Number of CDC samples to take
#define RAD_BIT_DELAY 40        //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 600    //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define INITIAL_SLEEP_TIME 10   //Initial Sleep Time for Baking
#define CDC_TIMEOUT 0x20        //Timeout for CDC
#define CDC_SAMPLE_TIME 0x3     //Time between CDC Samples
#define WAKEUP_PERIOD_CONT_INITIAL 3  // Wakeup period for initial portion of continuous pressure sensing
#define WAKEUP_PERIOD_CONT 244  // 213:10min Wakeup period for continous pressure sensing


//***************************************************
// Global variables
//***************************************************
//Test Declerations
volatile uint32_t enumerated;
volatile uint32_t Pblr_state;
volatile uint32_t cdc_data[NUM_SAMPLES];
volatile uint32_t cdc_data_index;
volatile uint32_t num_timeouts;
volatile uint32_t execution_count;
volatile uint32_t execution_count_irq;
volatile snsv2_r0_t snsv2_r0;

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
}
void handler_ext_int_1(void){
  *((volatile uint32_t *) 0xE000E280) = 0x2;
}
void handler_ext_int_2(void){
  *((volatile uint32_t *) 0xE000E280) = 0x4;
}
void handler_ext_int_3(void){
  *((volatile uint32_t *) 0xE000E280) = 0x8;
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
      delay (10);
      if ((radio_data>>i)&1) write_mbus_register(RAD_ADDR,0x27,0x1);
      else                   write_mbus_register(RAD_ADDR,0x27,0x0);
      //Must clear register
      delay (RAD_BIT_DELAY);
      write_mbus_register(RAD_ADDR,0x27,0x0);
      delay(RAD_BIT_DELAY); //Set delay between sending subsequent bit
    }
    //delay(RAD_PACKET_DELAY); //Set delays between sending subsequent packet
  }
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

static void operation_init(void){

  // Set PMU Strength & division threshold
  // Change PMU_CTRL Register
  // 0x0F770029 = Original
  // Increase sleep oscillator frequency for GOC and temp sensor
  // Decrease 5x division switching threshold
  *((volatile uint32_t *) 0xA200000C) = 0x0F77004B;

  // Speed up GOC frontend to match PMU frequency
  *((volatile uint32_t *) 0xA2000008) = 0x0020290A;

  delay(1000);

  //Enumerate & Initialize Registers
  Pblr_state = 0x0;
  enumerated = 0xDEADBEEF;
  cdc_data_index = 0;
  num_timeouts = 0;
  execution_count = 0;
  execution_count_irq = 0;
  //Enumeration
  enumerate(SNS_ADDR);
  delay(1000);
  //WFI();
  enumerate(RAD_ADDR);
  delay(1000);
  //WFI();
  //Set & Forget!
  snsv2_r1_t snsv2_r1 = SNSv2_R1_DEFAULT;
  snsv2_r7_t snsv2_r7 = SNSv2_R7_DEFAULT;
  radv5_r23_t radv5_r23; //Ext Ctrl En
  radv5_r26_t radv5_r26; //Current Limiter 2F = 30uA, 1F = 3uA
  radv5_r20_t radv5_r20; //Tune Power
  radv5_r21_t radv5_r21; //Tune Freq 1
  radv5_r22_t radv5_r22; //Tune Freq 2
  radv5_r25_t radv5_r25; //Tune TX Time
  radv5_r27_t radv5_r27; //Zero the TX register
  //SNSv2 R7
  snsv2_r7.REFGENREV_SEL = 0x4;
  snsv2_r7.REFGENREV_CDC_OFF = 0x0;
  write_mbus_register(SNS_ADDR,7,snsv2_r7.as_int);
  //SNSv2 R1
  snsv2_r1.CDC_S_period = 0x1A;
  snsv2_r1.CDC_R_period = 0xC;
  write_mbus_register(SNS_ADDR,1,snsv2_r1.as_int);
  //SNSv2 R0
  snsv2_r0.CDC_ext_select_CR = 0x0;
  snsv2_r0.CDC_max_select = 0x7;
  snsv2_r0.CDC_ext_max = 0x0;
  snsv2_r0.CDC_CR_fixB = 0x1;
  snsv2_r0.CDC_ext_clk = 0x0;
  snsv2_r0.CDC_outck_in = 0x1;
  snsv2_r0.CDC_on_adap = 0x0;
  snsv2_r0.CDC_s_recycle = 0x1;
  snsv2_r0.CDC_Td = 0x0;
  snsv2_r0.CDC_OP_on = 0x0;
  snsv2_r0.CDC_Bias_2nd = 0x7;
  snsv2_r0.CDC_Bias_1st = 0x7;
  snsv2_r0.CDC_EXT_RESET = 0x1;
  snsv2_r0.CDC_CLK = 0x0;
  write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
  delay(1000);
  //RADv5 R23
  radv5_r23.RADIO_EXT_CTRL_EN_1P2 = 0x0; //Ext Ctrl En
  write_mbus_register(RAD_ADDR,0x23,radv5_r23.as_int);
  delay(1000);
  //RADv5 R26
  radv5_r26.RADIO_TUNE_CURRENT_LIMITER_1P2 = 0x1F; //Current Limiter 2F = 30uA, 1F = 3uA
  write_mbus_register(RAD_ADDR,0x26,radv5_r26.as_int);
  delay(1000);
  //RADv5 R20
  radv5_r20.RADIO_TUNE_POWER_1P2 = 0x1F; //Tune Power
  write_mbus_register(RAD_ADDR,0x20,radv5_r20.as_int);
  delay(1000);
  //RADv5 R21
  radv5_r21.RADIO_TUNE_FREQ1_1P2 = 0x0; //Tune Freq 1
  write_mbus_register(RAD_ADDR,0x21,radv5_r21.as_int);
  delay(1000);
  //RADv5 R22
  radv5_r22.RADIO_TUNE_FREQ2_1P2 = 0x0; //Tune Freq 2 //0x0,0x0 = 902MHz on Pblr005
  write_mbus_register(RAD_ADDR,0x22,radv5_r22.as_int);
  delay(1000);
  //RADv5 R25
  radv5_r25.RADIO_TUNE_TX_TIME_1P2 = 0x4; //Tune TX Time
  write_mbus_register(RAD_ADDR,0x25,radv5_r25.as_int);
  delay(1000);
  //RADv5 R27
  radv5_r27.RADIO_DATA_1P2 = 0x0; //Zero the TX register
  write_mbus_register(RAD_ADDR,0x27,radv5_r27.as_int);
  /*
  set_wakeup_timer (0xFFF, 0x0, 0x1);
  set_wakeup_timer (INITIAL_SLEEP_TIME, 0x1, 0x0);
  operation_sleep();
  */
  sleep();
}

static void operation_cdc_reset(){
  //Not Reset
  //Not Running
  //Not Done
  if(((Pblr_state&0x3) == 0x0) && 
     (cdc_data_index != NUM_SAMPLES)){
    Pblr_state |= (0x1<<0);
    snsv2_r0.CDC_EXT_RESET = 0x0;
    snsv2_r0.CDC_CLK = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
    //Timeout
    set_wakeup_timer (0xFFF, 0x0, 0x1);
    set_wakeup_timer (CDC_TIMEOUT, 0x1, 0x0);
    operation_sleep();
  }
  else{
    Pblr_state &= ~(0x1<<0);
  }
}

static void operation_cdc_run(){
  //Not done taking samples
  if (cdc_data_index != NUM_SAMPLES){
    //Not Running (ie: First Sample)
    if(((Pblr_state>>1)&0x1) == 0x0){
      Pblr_state |= (0x1<<1);
      //Enable CDC_CLK
      snsv2_r0.CDC_CLK = 0x1;
      write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
      //Timeout
      set_wakeup_timer (0xFFF, 0x0, 0x1);
      set_wakeup_timer (CDC_TIMEOUT, 0x1, 0x0);
      operation_sleep();
    }
    //Not First Sample
    else{
      //Disable CDC_CLK
      snsv2_r0.CDC_CLK = 0x0;
      write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
      //Check if data is valid
      Pblr_state &= ~(0x1<<2);
      Pblr_state |= ((((*((volatile uint32_t *) 0xA0001014))>>1)&0x1) << 2);
      //If not valid
      if(((Pblr_state>>2)&0x1) == 0x0){
	//Take another sample
	snsv2_r0.CDC_CLK = 0x1;
	write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
	//Timeout
	set_wakeup_timer (0xFFF, 0x0, 0x1);
	set_wakeup_timer (CDC_TIMEOUT, 0x1, 0x0);
	operation_sleep();
      }
      else{
	Pblr_state &= ~(0x1<<1);
	//Reset CDC
	snsv2_r0.CDC_EXT_RESET = 0x1;
	write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
	cdc_data[cdc_data_index] = (*((volatile uint32_t *) 0xA0001014)>>2);
	cdc_data_index++;
        if (NUM_SAMPLES > 1) {
  	  //Go back to sleep
	  set_wakeup_timer (0xFFF, 0x0, 0x1);
	  set_wakeup_timer (CDC_SAMPLE_TIME, 0x1, 0x0);
	  operation_sleep();
        }
      }
    }
  }
}

static void operation_cdc_timeout(){
  if(((*((volatile uint32_t *) 0xA0001030) & 0x7FFF) == CDC_TIMEOUT) &&
     (((*((volatile uint32_t *) 0xA0001034)>>15) & 0x1) == 0x1)){
    num_timeouts++;
    //Failed during reset
    if(((Pblr_state>>0)&0x3) == 0x1){
      delay(10000);
      write_mbus_register(0x8,0,0xDEADBEEA);	// for on-board debug
      delay(10000);
      send_radio_data(gen_radio_data(0x5554));	// for radio debug (25'b1111_1010101010101_00_XXXXXX)
      delay(10000);
      send_radio_data(gen_radio_data(0x5554));	// for radio debug (25'b1111_1010101010101_00_XXXXXX)
      delay(10000);
      Pblr_state &= ~(0x0<<0);
      snsv2_r0.CDC_EXT_RESET = 0x1;
      write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
    }
    //Failed during run
    else if(((Pblr_state>>1)&0x1) == 0x1){
      delay(10000);
      write_mbus_register(0x8,0,0xDEADBEEB);	// for on-board debug
      delay(10000);
      send_radio_data(gen_radio_data(0x5555));	// for radio debug (25'b1111_1010101010101_01_XXXXXX)
      delay(10000);
      send_radio_data(gen_radio_data(0x5555));	// for radio debug (25'b1111_1010101010101_01_XXXXXX)
      delay(10000);
      Pblr_state &= ~(0x1<<1);
      snsv2_r0.CDC_EXT_RESET = 0x1;
      snsv2_r0.CDC_CLK = 0x0;
      write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
    }
    //Should not have woken up?
    else{
      delay(10000);
      write_mbus_register(0x8,0,0xDEADBEEF);	// for on-board debug
      delay(10000);
      send_radio_data(gen_radio_data(0x5556));	// for radio debug (25'b1111_1010101010101_10_XXXXXX)
      delay(10000);
      send_radio_data(gen_radio_data(0x5556));	// for radio debug (25'b1111_1010101010101_10_XXXXXX)
      delay(10000);
    }
  }
  //Reset Sleep Timer Everytime We Wakeup
  set_wakeup_timer (0xFFF, 0x0, 0x1);
}

static void operation_tx_cdc_results(){

  uint32_t i;
  for (i=0; i<NUM_SAMPLES; i++){
    delay(10000);
    send_radio_data(gen_radio_data(cdc_data[i]));
  }

  cdc_data_index = 0;

}


int main() {
  
  //Clear All Pending Interrupts
  *((volatile uint32_t *) 0xE000E280) = 0xF;
  //Enable Interrupts
  *((volatile uint32_t *) 0xE000E100) = 0xF;
  
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
    // Debug mode: Transmit something via radio 8 times and go to sleep w/o timer
    if (execution_count_irq < 8){
      execution_count_irq++;
      // radio
      send_radio_data(0xF0F0F0F0);
      // set timer
      set_wakeup_timer (WAKEUP_PERIOD_CONT_INITIAL, 0x1, 0x0);
      // go to sleep and wake up with same condition
      operation_sleep_noirqreset();

    }else{
      execution_count_irq = 0;
      // radio
      send_radio_data(0xF0F0F0F0);
      // Disable Timer
      set_wakeup_timer (0, 0x0, 0x0);
      // Go to sleep without timer
      operation_sleep();
    }

  }else if(wakeup_data_header == 2){
    // Proceed to continuous mode

  }else if(wakeup_data_header == 3){
    // Disable Timer
    set_wakeup_timer (0, 0x0, 0x0);
    // Go to sleep without timer
    operation_sleep();

  }else if(wakeup_data_header == 4){
    // Debug mode: Transmit something via radio 160 times and go to sleep w/o timer
    if (execution_count_irq < 160){
      execution_count_irq++;
      // radio
      send_radio_data(0xF0F0F0F0);
      // set timer
      set_wakeup_timer (WAKEUP_PERIOD_CONT_INITIAL, 0x1, 0x0);
      // go to sleep and wake up with same condition
      operation_sleep_noirqreset();

    }else{
      execution_count_irq = 0;
      // radio
      send_radio_data(0xF0F0F0F0);
      // Disable Timer
      set_wakeup_timer (0, 0x0, 0x0);
      // Go to sleep without timer
      operation_sleep();
    }

  }else{
    // Proceed to continuous mode

  }

  // Continuous cdc measurement mode 
  operation_cdc_timeout();
  operation_cdc_reset();
  operation_cdc_run();
  operation_tx_cdc_results();

  set_wakeup_timer (0xFFF, 0x0, 0x1);
  if(execution_count < 20){
    execution_count++;
    set_wakeup_timer (WAKEUP_PERIOD_CONT_INITIAL, 0x1, 0x0);
  }
  else{
    set_wakeup_timer (WAKEUP_PERIOD_CONT, 0x1, 0x0);
  }
  operation_sleep();

  while(1);

}

