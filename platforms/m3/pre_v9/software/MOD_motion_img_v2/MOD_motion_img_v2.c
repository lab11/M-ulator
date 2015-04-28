//****************************************************************************************************
//Author:       Gyouho Kim
//				Yejoong Kim
//              ZhiYoong Foo
//Description:  Derived from Pstack_ondemand_v1.6.c
//              For MOD Imaging System Demo
//				4/27/2015: Revision 2
//				- Adding FLSv1 SRAM write/read
//				4/22/2015: Revision 1
//				- Basic functionality without interfacing with FLSv1
//****************************************************************************************************
#include "mbus.h"
#include "PRCv9E.h"
#include "HRVv1.h"
#include "RADv5.h"
#include "FLSv1_GPIO.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// uncomment this to only transmit average
//#define TX_AVERAGE

// Stack order  PRC->MD->RAD->HRV
#define MD_ADDR 0x4		// MD Short Address
#define RAD_ADDR 0x5
#define HRV_ADDR 0x6
#define FLS_ADDR 0x7

// Common parameters
#define	MBUS_DELAY 200 //Amount of delay between successive messages
#define WAKEUP_DELAY 10000 // 20s
#define WAKEUP_DELAY_FINAL 5000	// Delay for waiting for internal decaps to stabilize after waking up MDSENSOR
#define DELAY_1 5000 // 5000: 0.5s
#define DELAY_IMG 40000 // 1s


// FLSv1 configurations
#define FLS_RECORD_LENGTH 0x1FFE // Maximum
//#define FLS_RECORD_LENGTH 0x10 

// MDv2 configurations
//#define INT_TIME 5
//#define MD_INT_TIME 10
#define MD_TH 10
#define MD_MASK 0x3FF
#define MD_LOWRES 1
#define MD_TOPAD_SEL 0 // 1: thresholding, 0: no thresholding

#define VDD_CC_1P2 1
#define VNW_1P2 1

#define SEL_CC 3
#define SEL_CC_B 4

#define SEL_VREF 0
#define SEL_VREFP 7
#define SEL_VBN 3
#define SEL_VBP 3
#define SEL_VB_RAMP 15
#define SEL_RAMP 1

#define SEL_ADC 1
#define SEL_ADC_B 6
#define SEL_PULSE 1
#define SEL_PULSE_COL 0
#define PULSE_SKIP 0
#define PULSE_SKIP_COL 0
#define TAVG 0

#define SEL_CLK_RING 2
#define SEL_CLK_DIV 4
#define SEL_CLK_RING_4US 0
#define SEL_CLK_DIV_4US 1
#define SEL_CLK_RING_ADC 2 
#define SEL_CLK_DIV_ADC 1
#define SEL_CLK_RING_LC 0
#define SEL_CLK_DIV_LC 1

#define START_ROW_IDX 40
#define END_ROW_IDX 120 // 160

#define ROW_SKIP 0
#define COL_SKIP 0
#define IMG_8BIT 1
#define ROW_IDX_EN 0
#define MD_RETURN_ADDR 0x17

#define START_COL_IDX 0 // in words
#define COLS_TO_READ 39 // in # of words: 39 for full frame, 19 for half

// Radio configurations
#define RAD_BIT_DELAY       13     //40      //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 	2000      //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define RAD_PACKET_NUM      1      //How many times identical data will be TXed

//***************************************************
// Global variables
//***************************************************
	//Test Declerations
	// "static" limits the variables to this file, giving compiler more freedom
	// "volatile" should only be used for MMIO
	static uint32_t enumerated;
	static uint32_t exec_count;
	static uint32_t exec_count_irq;
	static uint32_t MBus_msg_flag;
  
	static uint32_t WAKEUP_PERIOD_CONT; 
	static uint32_t WAKEUP_PERIOD_CONT_INIT; 

	static uint32_t radio_tx_count;
	static uint32_t radio_tx_option;
	static uint32_t radio_tx_numdata;

	static uint32_t md_count;
	static uint8_t md_capture_img;
	static uint8_t md_start_motion;
	
	static uint8_t INT_TIME;
	static uint8_t MD_INT_TIME;

//***************************************************
//Interrupt Handlers
//Must clear pending bit!
//***************************************************
void handler_ext_int_0(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_4(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_5(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_6(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_7(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_8(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_9(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));

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
void handler_ext_int_4(void){ *((volatile uint32_t *) 0xE000E280) = 0x10; }
void handler_ext_int_5(void){ *((volatile uint32_t *) 0xE000E280) = 0x20; }
void handler_ext_int_6(void){ *((volatile uint32_t *) 0xE000E280) = 0x40; }
void handler_ext_int_7(void){ *((volatile uint32_t *) 0xE000E280) = 0x80; }
void handler_ext_int_8(void){ *((volatile uint32_t *) 0xE000E280) = 0x100; }
void handler_ext_int_9(void){
	*((volatile uint32_t *) 0xE000E280) = 0x200;
	*((volatile uint32_t *) 0xA8000000) = 0x0;
}
void handler_ext_int_10(void){ *((volatile uint32_t *) 0xE000E280) = 0x400; }
void handler_ext_int_11(void){ *((volatile uint32_t *) 0xE000E280) = 0x800; }

//************************************
// PMU Related Functions
//************************************
inline static void set_pmu_sleep_clk_low(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F770039; // 0x8F770039: use GOC x0.6-2
}
inline static void set_pmu_sleep_clk_default(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F770049; // 0x8F770049: use GOC x10-25
}
inline static void set_pmu_sleep_clk_fastest(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F770079; // 0x8F770079: use GOC x70-230
}

//************************************
// MDv2 Functions
//************************************

  uint32_t mdreg_0;
  uint32_t mdreg_1;
  uint32_t mdreg_2;
  uint32_t mdreg_3;
  uint32_t mdreg_4;
  uint32_t mdreg_5;
  uint32_t mdreg_6;
  uint32_t mdreg_7;
  uint32_t mdreg_8;

static void initialize_md_reg(){

  mdreg_0 = ((MD_INT_TIME<<13)|(INT_TIME<<(3+1)));
  mdreg_1 = (((!IMG_8BIT)<<16)|(MD_TOPAD_SEL<<12)|(MD_TH<<5)|(!MD_LOWRES<<2)|(MD_LOWRES<<1));
  mdreg_2 = ((0x1F<<19)|(0x3<<12)|(MD_MASK));
  mdreg_3 = ((SEL_RAMP<<18)|(SEL_VB_RAMP<<14)|(SEL_VBP<<12)|(SEL_VBN<<10)|(SEL_VREFP<<7)|(SEL_VREF<<4)|(!VNW_1P2<<3)|(VNW_1P2<<2)|(!VDD_CC_1P2<<1)|(VDD_CC_1P2<<0)); // is this inversion safe??
  mdreg_4 = ((!TAVG<<19)|(TAVG<<18)|(PULSE_SKIP_COL<<17)|(PULSE_SKIP<<16)|(SEL_CC_B<<13)|(SEL_CC<<10)|(SEL_PULSE_COL<<8)|(SEL_PULSE<<6)|(SEL_ADC_B<<3)|(SEL_ADC<<0));
  mdreg_5 = ((SEL_CLK_DIV_LC<<16)|(SEL_CLK_RING_LC<<14)|(SEL_CLK_DIV_ADC<<11)|(SEL_CLK_DIV_4US<<9)|(SEL_CLK_DIV<<6)|(SEL_CLK_RING_ADC<<4)|(SEL_CLK_RING_4US<<2)|(SEL_CLK_RING<<0));
  mdreg_6 = ((ROW_IDX_EN<<21)|(IMG_8BIT<<19)|(COL_SKIP<<18)|(ROW_SKIP<<17)|(END_ROW_IDX<<9)|(START_ROW_IDX<<1)|(0x1<<0));
  mdreg_7 = ((0x7<<15));
  mdreg_8 = ((COLS_TO_READ<<15)|(START_COL_IDX<<8)|(MD_RETURN_ADDR<<0));

  //Write Registers;
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x3,mdreg_3);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x4,mdreg_4);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x5,mdreg_5);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x6,mdreg_6);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);
  write_mbus_register(MD_ADDR,0x8,mdreg_8);
  delay (MBUS_DELAY);

}

static void poweron_frame_controller(){

  // Release MD Presleep (this also releases reset due to a design bug)
  // 2:22
  mdreg_2 &= ~(1<<22);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay(WAKEUP_DELAY);

  // Release MD Sleep
  // 2:21
  mdreg_2 &= ~(1<<21);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay(WAKEUP_DELAY);

  // Release MD Isolation
  // 7:15
  mdreg_7 &= ~(1<<15);
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);
 
  // Release MD Reset
  // 2:23
  mdreg_2 &= ~(1<<23);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay (MBUS_DELAY);

  // Start MD Clock
  // 5:12
  mdreg_5 |= (1<<12);
  write_mbus_register(MD_ADDR,0x5,mdreg_5);
  delay (MBUS_DELAY);

}

static void poweron_array_adc(){

  // Release IMG Presleep 
  // 2:20
  mdreg_2 &= ~(1<<20);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay(WAKEUP_DELAY);

  // Release IMG Sleep
  // 2:19
  mdreg_2 &= ~(1<<19);
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay(WAKEUP_DELAY);

  // Release ADC Isolation
  // 7:17
  mdreg_7 &= ~(1<<17);
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);

  // Release ADC Wrapper Reset
  // 6:0
  mdreg_6 &= ~(1<<0);
  write_mbus_register(MD_ADDR,0x6,mdreg_6);
  delay (MBUS_DELAY);

  // Start ADC Clock
  // 5:13
  mdreg_5 |= (1<<13);
  write_mbus_register(MD_ADDR,0x5,mdreg_5);
  delay (MBUS_DELAY);

}

static void poweroff_array_adc(){

  // Stop ADC Clock
  // 5:13
  mdreg_5 &= ~(1<<13);
  write_mbus_register(MD_ADDR,0x5,mdreg_5);
  delay (MBUS_DELAY);

  // Assert ADC Wrapper Reset
  // 6:0
  mdreg_6 |= 1<<0;
  write_mbus_register(MD_ADDR,0x6,mdreg_6);
  delay (MBUS_DELAY);

  // Assert ADC Isolation
  // 7:17
  mdreg_7 |= 1<<17;
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);

  // Assert IMG Presleep 
  // 2:20
  mdreg_2 |= 1<<20;

  // Assert IMG Sleep
  // 2:19
  mdreg_2 |= 1<<19;
  write_mbus_register(MD_ADDR,0x2,mdreg_2);
  delay (MBUS_DELAY);

}


static void capture_image_single(){

  // Capture Image
  // 0:0
  mdreg_0 |= (1<<0);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay(0x80); // about 6ms

  mdreg_0 &= ~(1<<0);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);

  delay(DELAY_IMG); // about 1s

}

static void capture_image_single_with_flash(){
	volatile uint32_t temp_numBit;
	volatile uint32_t temp_addr;
	volatile uint32_t temp_data;

	write_mbus_message(0xEE, 0x12341234);
	FLSMBusGPIO_initialization();

	// Set Flash Ext Streaming Length
	FLSMBusGPIO_setExtStreamLength(FLS_ADDR, FLS_RECORD_LENGTH);
	// Give a "Go Flash Ext Streaming" command
	FLSMBusGPIO_doExtStream(FLS_ADDR);


	// Capture Image
	// 0:0
	mdreg_0 |= (1<<0);
	write_mbus_register(MD_ADDR,0x0,mdreg_0);
	delay(0x80); // about 6ms

	mdreg_0 &= ~(1<<0);
	write_mbus_register(MD_ADDR,0x0,mdreg_0);

	delay(DELAY_IMG); // about 1s


	// Receive the interrupt from Flash. The payload should be 0x82 (pass) or 0x80 (timeout)
	write_mbus_message(0xEE, 0x22222222);
	temp_numBit = FLSMBusGPIO_rxMsg();
	temp_addr = FLSMBusGPIO_getRxAddr(); // Optional
	temp_data = FLSMBusGPIO_getRxData0(); // Optional
	write_mbus_message(0xEE, temp_addr); // Optional
	write_mbus_message(0xEE, temp_data); // Optional
	write_mbus_message(0xEE, temp_numBit); // Optional

  	// Store result
  	//temp_addr_0 = FLSMBusGPIO_getRxAddr();
  	//temp_data_0 = FLSMBusGPIO_getRxData0();
  
}

static void capture_image_start(){

  // Capture Image
  // 0:0
  mdreg_0 |= (1<<0);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay(0x80); // about 6ms

}

static void start_md(){

  // Optionally release MD GPIO Isolation
  // 7:16
  mdreg_7 &= ~(1<<16);
  write_mbus_register(MD_ADDR,0x7,mdreg_7);
  delay (MBUS_DELAY);
  delay(DELAY_1); // about 0.5s

  // Start MD
  // 0:1
  mdreg_0 |= (1<<1);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);
  delay(DELAY_1); // about 0.5s

  mdreg_0 &= ~(1<<1);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);

  delay(DELAY_1); // about 0.5s

  // Enable MD Flag
  // 1:3
  mdreg_1 |= (1<<3);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);

}

static void clear_md_flag(){

  // Stop MD
  // 0:2
  mdreg_0 |= (1<<2);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);
  delay (0x80); // about 6ms

  mdreg_0 &= ~(1<<2);
  write_mbus_register(MD_ADDR,0x0,mdreg_0);
  delay (MBUS_DELAY);

  // Clear MD Flag
  // 1:4
  mdreg_1 |= (1<<4);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);
  delay (0x80); // about 6ms
  
  mdreg_1 &= ~(1<<4);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);

  // Disable MD Flag
  // 1:3
  mdreg_1 &= ~(1<<3);
  write_mbus_register(MD_ADDR,0x1,mdreg_1);
  delay (MBUS_DELAY);

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
  
	// Reset IRQ10VEC
  *((volatile uint32_t *) IRQ10VEC/*IMSG0*/) = 0;

  // Go to sleep without timer
  operation_sleep();

}

static void operation_temp(void){
	volatile uint32_t temp_addr;
	volatile uint32_t temp_data;
	volatile uint32_t temp_numBit;
  
    // Set PMU Strength & division threshold
    // PMU_CTRL Register
    // PRCv9 Default: 0x8F770049
    // Decrease 5x division switching threshold
	set_pmu_sleep_clk_default();
  
    // Speed up GOC frontend to match PMU frequency
	// Speed up MBUS
	// GOC_CTRL Register
    // PRCv9 Default: 0x00202903
    //*((volatile uint32_t *) 0xA2000008) = 0x00202908;
	*((volatile uint32_t *) 0xA2000008) = 0x00202608;

	// MEM&CORE CLK /16
	*((volatile uint32_t *) 0xA2000008) = 0x120F903;	//Isolation disable
  
    delay(DELAY_1);
  
    //Enumerate & Initialize Registers
    enumerated = 0xDEADBEEE;
    exec_count = 0;
    exec_count_irq = 0;
    MBus_msg_flag = 0;


	// Initialize FLSv1 through GPIO
	FLSMBusGPIO_initialization();
	FLSMBusGPIO_forceStop();
	FLSMBusGPIO_enumeration(FLS_ADDR);
	FLSMBusGPIO_enumeration(FLS_ADDR);
	temp_numBit = FLSMBusGPIO_rxMsg();
	temp_addr = FLSMBusGPIO_getRxAddr();
	temp_data = FLSMBusGPIO_getRxData0();
		write_mbus_message(0xEE, 0x00000000); delay(1000);
		write_mbus_message(0xEE, temp_addr); delay(1000);
		write_mbus_message(0xEE, 0x11111111); delay(1000);
		write_mbus_message(0xEE, temp_data); delay(1000);
		write_mbus_message(0xEE, 0x22222222); delay(1000);
		write_mbus_message(0xEE, temp_numBit); delay(1000);


	// Set Optimum Tuning Bits
	FLSMBusGPIO_setOptTune(FLS_ADDR);

	// Put FLS back to sleep
	FLSMBusGPIO_sleep();

    // Go to sleep without timer
    operation_sleep_notimer();
}

static void operation_init(void){
	volatile uint32_t temp_addr;
	volatile uint32_t temp_data;
	volatile uint32_t temp_numBit;
  
    // Set PMU Strength & division threshold
    // PMU_CTRL Register
    // PRCv9 Default: 0x8F770049
    // Decrease 5x division switching threshold
	set_pmu_sleep_clk_default();
  
    // Speed up GOC frontend to match PMU frequency
	// Speed up MBUS
	// GOC_CTRL Register
    // PRCv9 Default: 0x00202903
	//*((volatile uint32_t *) 0xA2000008) = 0x00202608;
  
	// For PREv9E GPIO Isolation disable >> bits 16, 17, 24
	*((volatile uint32_t *) 0xA2000008) = 0x0120E608;


    delay(DELAY_1);
  
    //Enumerate & Initialize Registers
    enumerated = 0xDEADBEEE;
    exec_count = 0;
    exec_count_irq = 0;
    MBus_msg_flag = 0;
    //Enumeration
    enumerate(MD_ADDR);
    delay(MBUS_DELAY*2);
    enumerate(RAD_ADDR);
    delay(MBUS_DELAY*2);
    enumerate(HRV_ADDR);
    delay(MBUS_DELAY*2);

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
  
	// Initialize MDv2
	initialize_md_reg();

	// Initialize FLSv1 through GPIO
	FLSMBusGPIO_initialization();
	FLSMBusGPIO_forceStop();
	FLSMBusGPIO_enumeration(FLS_ADDR);
	FLSMBusGPIO_enumeration(FLS_ADDR);
	temp_numBit = FLSMBusGPIO_rxMsg();
	temp_addr = FLSMBusGPIO_getRxAddr(); // Optional
	temp_data = FLSMBusGPIO_getRxData0(); // Optional
	write_mbus_message(0xEE, temp_addr); // Optional
	write_mbus_message(0xEE, temp_data); // Optional
	write_mbus_message(0xEE, temp_numBit); // Optional

	// Set Optimum Tuning Bits
	FLSMBusGPIO_setOptTune(FLS_ADDR);

	// Receive Sleep Msg
	temp_numBit = FLSMBusGPIO_rxMsg();
	temp_addr = FLSMBusGPIO_getRxAddr(); // Optional
	temp_data = FLSMBusGPIO_getRxData0(); // Optional
	write_mbus_message(0xEE, temp_addr); // Optional
	write_mbus_message(0xEE, temp_data); // Optional
	write_mbus_message(0xEE, temp_numBit); // Optional

	delay (1000);
/*
	// Check 0x0E Register
	FLSMBusGPIO_readReg(FLS_ADDR, 0x0E, 0, 0xFF, 0xFE);
	temp_numBit = FLSMBusGPIO_rxMsg();
	temp_addr = FLSMBusGPIO_getRxAddr(); // Optional
	temp_data = FLSMBusGPIO_getRxData0(); // Optional
	write_mbus_message(0xEE, temp_addr); // Optional
	write_mbus_message(0xEE, temp_data); // Optional
	write_mbus_message(0xEE, temp_numBit); // Optional
*/

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 100;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 1;   // 0x1E (30): ~1 min with PRCv9
    radio_tx_count = 0;
    radio_tx_option = 0;
    
	md_start_motion = 0;
	md_capture_img = 0;
	md_count = 0;

	INT_TIME = 5;
	MD_INT_TIME = 15;

	// Put FLS back to sleep
	FLSMBusGPIO_sleep();

    // Go to sleep without timer
    operation_sleep_notimer();
}

static void operation_md(void){

	// Release power gates, isolation, and reset for frame controller
	if (md_count == 0) {
		initialize_md_reg();
		poweron_frame_controller();
	}else{
		// This wakeup is due to motion detection
		// Let the world know!
		write_mbus_message(0xAA, 0x22222222);
		write_mbus_message(0xAA, 0x22222222);
		write_mbus_message(0xAA, 0x22222222);
		clear_md_flag();
        // radio
        send_radio_data(0xFAFAF0F0);	
	}

	if (md_capture_img){
		// Set PMU Strength & division threshold
		// PMU_CTRL Register
		// PRCv9 Default: 0x8F770049
		*((volatile uint32_t *) 0xA200000C) = 0x8F772879; // works without any override!
	  
		delay(DELAY_1);

		// Release power gates, isolation, and reset for imager array
		poweron_array_adc();
		delay(DELAY_1);

		// Capture a single image
		if (radio_tx_option){
			//capture_image_single_radio(); // FIXME
		}else{
			capture_image_single_with_flash();
		}
		
		poweroff_array_adc();
	}

	if (md_start_motion){
		// Only need to set sleep PMU settings
        set_pmu_sleep_clk_fastest();
		
		delay(DELAY_1);
		md_count++;

		// Start motion detection
		start_md();

		delay(DELAY_1);
		clear_md_flag();
		delay(DELAY_1);
		start_md();

		delay(DELAY_1);

	}else{
		// Restore PMU_CTRL setting
		set_pmu_sleep_clk_default();
	}

	if (md_capture_img){
		// Make FLS Layer go to sleep through GPIO
		FLSMBusGPIO_sleep();
	}

	// Go to sleep w/o timer
	operation_sleep_notimer();
}

//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
  
    //Clear All Pending Interrupts
    *((volatile uint32_t *) 0xE000E280) = 0xF;
    //Enable Interrupts
    *((volatile uint32_t *) 0xE000E100) = 0xF;
  
	// Config watchdog timer to about 30 sec: 3,000,000 with default PRCv9
	//config_timer( timer_id, go, roi, init_val, sat_val )
//	config_timer( 0, 1, 0, 0, 3000000 );

    // Initialization sequence
    if (enumerated != 0xDEADBEEE){
        operation_init();
//        operation_temp();
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
            send_radio_data(0xFAFA0000+exec_count_irq);	
            // set timer
            set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
            // go to sleep and wake up with same condition
            operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
            // radio
            send_radio_data(0xFAFA0000+exec_count_irq);	
            // Go to sleep without timer
            operation_sleep_notimer();
        }
   
    }else if(wakeup_data_header == 2){
		// Start motion detection
        // wakeup_data[7:0] is the md integration period
        // wakeup_data[15:8] is the img integration period
        // wakeup_data[19:16] indicates whether or not to to take an image
		// 						1: md only, 2: img only, 3: md+img
        // wakeup_data[23:20] indicates whether or not to radio out the result

		MD_INT_TIME = wakeup_data_field_0;
		INT_TIME = wakeup_data_field_1;

        if (exec_count_irq < 2){
            exec_count_irq++;
            // radio
            send_radio_data(0xFAFA0000+(wakeup_data_field_2 & 0xF));	
            // set timer
            set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
            // go to sleep and wake up with same condition
            operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
			// Reset IRQ10VEC
			*((volatile uint32_t *) IRQ10VEC) = 0;

			// Run Program
			exec_count = 0;
			md_count = 0;
			md_start_motion = (wakeup_data_field_2 & 0x1);
			md_capture_img = (wakeup_data_field_2 & 0x2)>>1;
			radio_tx_option = (wakeup_data_field_2 >> 4) & 0x1;
			operation_md();
        }
   
    }else if(wakeup_data_header == 3){
		// Stop MD program and transmit the execution count n times
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
		
		clear_md_flag();
		initialize_md_reg();
        set_pmu_sleep_clk_default();
		md_start_motion = 0;
		md_capture_img = 0;

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
            // radio
            send_radio_data(0xFAFA0000+md_count);	
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

    }else if(wakeup_data_header == 0x11){
		// Slow down PMU sleep osc and go to sleep for further programming
        set_pmu_sleep_clk_low();
        // Go to sleep without timer
        operation_sleep_notimer();

    }else if(wakeup_data_header == 0x12){
		// Restore PMU sleep osc and go to sleep for further programming
        set_pmu_sleep_clk_default();
        // Go to sleep without timer
        operation_sleep_notimer();

    }else if(wakeup_data_header == 0x13){
        set_pmu_sleep_clk_fastest();
        // Go to sleep without timer
        operation_sleep_notimer();
    }

    // Proceed to continuous mode
    while(1){
        operation_md(); 
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

