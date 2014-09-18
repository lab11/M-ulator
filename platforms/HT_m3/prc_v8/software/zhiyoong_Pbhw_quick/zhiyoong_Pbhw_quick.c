//*******************************************************************
//Author: ZhiYoong Foo
//Description:  Pbhw Functionality Tests
//		Moving towards Mouse Implantation
//Stacking Diagram: 	\m3_shared\m3_orders\Order to Advotech\140207 - PRCv8 Tbh(3) Tbhr(3) Tbhlw(3) Pbhw(3)\
//			Pbhw Stack Assembly Plan (PRS-BAT-PRC-DCP-SNS-HRV-RAD-SOL).pdf
//*******************************************************************
#include "mbus.h"
#include "PRCv8.h"
#include "SNSv2.h"
#include "HRVv1.h"
#include "RADv5.h"

#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define HRV_ADDR 0x6

#define NUM_SAMPLES 3         //Number of CDC samples to take
#define RAD_BIT_DELAY 40       //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 600   //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define RAD_SAMPLE_DELAY 213   //2//213:10min       //Wake up timer tuning: # of wake up timer cycles to sleep
#define RAD_SAMPLE_DELAY_INITIAL 3 // Wake up timer duration for initial periods


//***************************************************
// Global variables
//***************************************************
//Test Declerations
static uint32_t enumerated;
static uint32_t cdc_reset_sleep;
static uint32_t cdc_data[NUM_SAMPLES];
static uint32_t cdc_data_index;
static uint32_t cdc_running;
static uint32_t cdc_valid;  

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
  //uint32_t data_out =  0x000000 | data_in<<6;
  uint32_t data_out =  0x3C0000 | data_in<<6;
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
  *((volatile uint32_t *) 0xA2000014) = 0x1;

  // Reset IRQ10VEC
  *((volatile uint32_t *) IRQ10VEC/*IMSG0*/) = 0;

  // Go to Sleep
  sleep();
  while(1);

}

int main() {
  
  //SNSv2 Register Declerations
  //Set to defaults
  snsv2_r0_t snsv2_r0 = SNSv2_R0_DEFAULT;
  snsv2_r0.CDC_Bias_1st = 0x7;
  snsv2_r0.CDC_Bias_2nd = 0x7;
  snsv2_r0.CDC_s_recycle = 0x1;
  snsv2_r0.CDC_on_adap = 0x0;

  //Clear All Pending Interrupts
  *((volatile uint32_t *) 0xE000E280) = 0xF;
  //Enable Interrupts
  *((volatile uint32_t *) 0xE000E100) = 0xF;
  
  //Set PMU Division to 5
  //*((volatile uint32_t *) 0xA200000C) = 0x4F770029; 
  //Set PMU Division to 6
  //*((volatile uint32_t *) 0xA200000C) = 0x0F77002B;
  
  //I blame Yoonmyung
  set_wakeup_timer (0xFFF, 0x0, 0x1);

  //Enumerate & Initialize Registers
  if (enumerated != 0xDEADBEEF){
    enumerated = 0xDEADBEEF;
    cdc_data_index = 0;
    //Enumeration
    enumerate(RAD_ADDR);
    WFI();
    enumerate(SNS_ADDR);
    WFI();
    enumerate(HRV_ADDR);
    WFI();
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
    write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
    //RADv5 R23
    radv5_r23.RADIO_EXT_CTRL_EN_1P2 = 0x0; //Ext Ctrl En
    write_mbus_register(RAD_ADDR,23,radv5_r23.as_int);
    //RADv5 R26
    radv5_r26.RADIO_TUNE_CURRENT_LIMITER_1P2 = 0x1F; //Current Limiter 2F = 30uA, 1F = 3uA
    write_mbus_register(RAD_ADDR,26,radv5_r26.as_int);
    //RADv5 R20
    radv5_r20.RADIO_TUNE_POWER_1P2 = 0x1F; //Tune Power
    write_mbus_register(RAD_ADDR,20,radv5_r20.as_int);
    //RADv5 R21
    radv5_r21.RADIO_TUNE_FREQ1_1P2 = 0x0; //Tune Freq 1
    write_mbus_register(RAD_ADDR,21,radv5_r21.as_int);
    //RADv5 R22
    radv5_r22.RADIO_TUNE_FREQ2_1P2 = 0x0; //Tune Freq 2
    write_mbus_register(RAD_ADDR,22,radv5_r22.as_int);
    //RADv5 R25
    radv5_r25.RADIO_TUNE_TX_TIME_1P2 = 0x4; //Tune TX Time
    write_mbus_register(RAD_ADDR,25,radv5_r25.as_int);
    //RADv5 R27
    radv5_r27.RADIO_DATA_1P2 = 0x0; //Zero the TX register
    write_mbus_register(RAD_ADDR,27,radv5_r27.as_int);
    set_wakeup_timer (0xFFF, 0x0, 0x1);
    set_wakeup_timer (0x2, 0x1, 0x0);
    operation_sleep();
  }

  //Disable ext_reset
  if((cdc_reset_sleep != 0xDEADBEEF) && 
     (cdc_running != 0xDEADBEEF) &&
     (cdc_data_index != NUM_SAMPLES)){
    cdc_reset_sleep = 0xDEADBEEF;
    snsv2_r0.CDC_EXT_RESET = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
    operation_sleep();
  }
  else{
    cdc_reset_sleep = 0x0;
  }

  if (cdc_data_index != NUM_SAMPLES){
    if(cdc_running != 0xDEADBEEF){
      cdc_running = 0xDEADBEEF;
      cdc_valid = 0;
      //Enable CDC_CLK
      snsv2_r0.CDC_EXT_RESET = 0x0;
      snsv2_r0.CDC_CLK = 0x1;
      write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
      operation_sleep();
      }
    else{
      //Disable CDC_CLK
      snsv2_r0.CDC_EXT_RESET = 0x0;
      snsv2_r0.CDC_CLK = 0x0;
      write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
      cdc_valid = (((*((volatile uint32_t *) 0xA0001014))>>2)&0x1);
      if(!cdc_valid){
	//Enable CDC_CLK
	snsv2_r0.CDC_EXT_RESET = 0x0;
	snsv2_r0.CDC_CLK = 0x1;
	write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
	operation_sleep();
      }
      else{
	cdc_running = 0x0;
	//Reset CDC
	snsv2_r0.CDC_EXT_RESET = 0x1;
	write_mbus_register(SNS_ADDR,0,snsv2_r0.as_int);
	cdc_data[cdc_data_index] = *((volatile uint32_t *) 0xA0001014);
	cdc_data_index++;
	
	set_wakeup_timer (0xFFF, 0x0, 0x1);
	set_wakeup_timer (0x2, 0x1, 0x0);
	operation_sleep();
      }
    }
  }
  else{
    //chuck off into the void
    uint32_t i;
    for (i=0; i<NUM_SAMPLES; i++){
      delay(10000);
      write_mbus_register(0x7,0,cdc_data[i]>>2);
    }
    for (i=0; i<NUM_SAMPLES; i++){
      delay(10000);
      write_mbus_register(0x8,0,gen_radio_data(cdc_data[i]>>2));
    }
    /*
    for (i=0; i<NUM_SAMPLES; i++){
      delay(10000);
      send_radio_data(gen_radio_data(cdc_data[i]>>2));
    }
    */
    cdc_data_index = 0;
    set_wakeup_timer (0xFFF, 0x0, 0x1);
    set_wakeup_timer (0x4, 0x1, 0x0);
    operation_sleep();
  }


  while (1){}
}

