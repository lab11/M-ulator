//*******************************************************************
//Author: Gyouho Kim
//        Yoonmyung Lee
//        Zhiyoong Foo
//
//Date: April 2014
//
//Description: 	Derived from Tstack_longterm_shortwake code
// Main functionality is to periodically measure temperature data 
// and store the data. When triggered by GOC interrupt (IRQ10VEC), 
// and all the stored data is transmitted out
// Initially, the temp measurement interval is short for testing purposes
//
//*******************************************************************
#include "mbus.h"
#include "SNSv2.h"
#include "PRCv8.h"
#include "RADv5.h"

#define RAD_ADDR 0x3
#define SNS_ADDR 0x4

#define MBUS_DELAY 100 //Amount of delay between successive messages

#define RAD_BIT_DELAY 40     //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 600  //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define TEMP_WAKEUP_CYCLE 10     //2//213:10min       //Wake up timer tuning: # of wake up timer cycles to sleep
#define TEMP_WAKEUP_CYCLE_INITIAL 3 // Wake up timer duration for initial periods
#define NUM_INITIAL_CYCLE 3 // Number of initial cycles
#define DATA_BUFFER_SIZE 256

//***************************************************
// Global variables
//***************************************************
  volatile uint32_t exec_marker;
  volatile uint32_t exec_temp_marker;
  volatile uint32_t exec_count;
  volatile uint32_t exec_count_irq;
  
  volatile uint32_t temp_data_stored[DATA_BUFFER_SIZE] = {0};
  volatile uint32_t temp_data_count;
  uint32_t _sns_r3; 
  uint32_t temp_data;
  uint32_t radio_data;

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
// INT vector 10 and 11 is not real interrupt vectors
// These vector location (0x0068 and 0x006C) are used as on demaned request parameter buffer
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));

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
void handler_ext_int_4(void){
  *((volatile uint32_t *) 0xE000E280) = 0x10;
}
void handler_ext_int_5(void){
  *((volatile uint32_t *) 0xE000E280) = 0x20;
}
void handler_ext_int_6(void){
  *((volatile uint32_t *) 0xE000E280) = 0x40;
}
void handler_ext_int_7(void){
  *((volatile uint32_t *) 0xE000E280) = 0x80;
}
void handler_ext_int_8(void){
  *((volatile uint32_t *) 0xE000E280) = 0x100;
}
void handler_ext_int_9(void){
  *((volatile uint32_t *) 0xE000E280) = 0x200;
}
void handler_ext_int_10(void){
  *((volatile uint32_t *) 0xE000E280) = 0x400;
}
void handler_ext_int_11(void){
  *((volatile uint32_t *) 0xE000E280) = 0x800;
}


//***************************************************
// Internal Functions
//***************************************************

//***************************************************
// Configuration setup for radio (RADv5)
//***************************************************
static void setup_radio(void) {

  //************************************
  //RADv4 Register Defaults
  //************************************
  delay(MBUS_DELAY);

  //Ext Ctrl En
  uint32_t _rad_r23 = 0x0;
  write_mbus_register(RAD_ADDR,0x23,_rad_r23);
  delay(MBUS_DELAY);
  
  //Current Limiter  
  uint32_t _rad_r26 = 0x1F; // 2F is 30uA, 1F is 3uA
  write_mbus_register(RAD_ADDR,0x26,_rad_r26);  
  delay(MBUS_DELAY);

  //Tune Power
  uint32_t _rad_r20 = 0x1F;
  write_mbus_register(RAD_ADDR,0x20,_rad_r20);
  delay(MBUS_DELAY);
  
  
  // For board level testing, 00 corresponds to 902MHz
  //Tune Freq 1
  uint32_t _rad_r21 = 0x0;
  write_mbus_register(RAD_ADDR,0x21,_rad_r21);
  delay(MBUS_DELAY);

  //Tune Freq 2
  uint32_t _rad_r22 = 0x0;
  write_mbus_register(RAD_ADDR,0x22,_rad_r22);
  delay(MBUS_DELAY);
 
  //Tune TX Time
  uint32_t _rad_r25 = 0x4;
  write_mbus_register(RAD_ADDR,0x25,_rad_r25);
  delay(MBUS_DELAY);

  //Zero the TX register
  write_mbus_register(RAD_ADDR,0x27,0x0);
  delay(MBUS_DELAY);

}


//***************************************************
// Configuration for temperature sensor (SNSv2)
//***************************************************
static void setup_tempsensor(void) {

  //************************************
  //SNSv1 Register Defaults
  //uint32_t _sns_r0 = (0x0<<22)|(0x1<<21)|(0x6<<18)|(0x6<<15)|(0x0<<14)|(0x0<<12)|(0x4<<9)|(0x1<<8)|(0x1<<7)|(0x0<<6)|(0x1<<5)|(0x0<<4)|(0x7<<1)|(0x0<<0);
  //uint32_t _sns_r1 = (0x0<<18)|(0xF<<9)|(0x20<<0);
  //uint32_t _sns_r3 = (0x2<<17)|(0x1<<16)|(0xF<<12)|(0x0<<8)|(0xF<<4)|(0x0<<0);
  //************************************
  //Setup T Sensor
  _sns_r3 = (0x3<<17)|(0x1<<16)|(0xF<<12)|(0x0<<8)|(0xF<<4)|(0x0<<0);
  write_mbus_register(SNS_ADDR,3,_sns_r3);
  delay(MBUS_DELAY);

  // Change mbus interrupt address for temp sensor and cdc to 0x15 and 0x16
  uint32_t _sns_r6 = (0x9<<16)|(0x16<<8)|(0x15<<0);
  write_mbus_register(SNS_ADDR,6,_sns_r6);
  delay(MBUS_DELAY);

}


//***************************************************
//Data Setup for Radio (Initial Setup) & ECC [SECDED]
//***************************************************
//0xFXXX => Radio Preamble (bits 12-15)
//0xXX1F => ECC Setup (bits 0-4)
//Truncate last 3 bits of temp_data and put in bits 5-11
//ECC:
//B4 = B11^B10^B9
//B3 = B8 ^B7 ^B6
//B2 = B11^B10^B8 ^B7 ^B5
//B1 = B11^B9 ^B8 ^B6 ^B5
//B0 = B11^B10^B9 ^B8 ^B7 ^B6 ^B5 ^B4 ^B3 ^B2 ^B1
//Must be done in order (B0 is dependent on B1,2,3,4))
static uint32_t gen_radio_data(uint32_t data_in) {
  uint32_t data_out =  0xF000 | (data_in<<5);;
  uint32_t B4 = 
    ((data_out>>11)&0x1) ^ 
    ((data_out>>10)&0x1) ^ 
    ((data_out>> 9)&0x1);
  uint32_t B3 = 
    ((data_out>> 8)&0x1) ^ 
    ((data_out>> 7)&0x1) ^ 
    ((data_out>> 6)&0x1);
  uint32_t B2 = 
    ((data_out>>11)&0x1) ^ 
    ((data_out>>10)&0x1) ^ 
    ((data_out>> 8)&0x1) ^ 
    ((data_out>> 7)&0x1) ^ 
    ((data_out>> 5)&0x1);
  uint32_t B1 = 
    ((data_out>>11)&0x1) ^ 
    ((data_out>> 9)&0x1) ^ 
    ((data_out>> 8)&0x1) ^ 
    ((data_out>> 6)&0x1) ^ 
    ((data_out>> 5)&0x1);
  uint32_t B0 = 
    ((data_out>>11)&0x1) ^ 
    ((data_out>>10)&0x1) ^ 
    ((data_out>> 9)&0x1) ^ 
    ((data_out>> 8)&0x1) ^ 
    ((data_out>> 7)&0x1) ^
    ((data_out>> 6)&0x1) ^ 
    ((data_out>> 5)&0x1) ^ 
    B4 ^ B3 ^ B2 ^ B1;
  data_out |= (B4<<4)|(B3<<3)|(B2<<2)|(B1<<1)|(B0<<0);
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
    for(i=15;i>=0;i--){ //Bit Loop
      delay (10);
      if ((radio_data>>i)&1) write_mbus_register(RAD_ADDR,0x27,0x1);
      else                   write_mbus_register(RAD_ADDR,0x27,0x0);

      // For radio debugging purposes: transmit all 1's
      //write_mbus_register(RAD_ADDR,0x27,0x1);

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
  *((volatile uint32_t *) IRQ10VEC) = 0;
  
  // Disable RADIO tx
  write_mbus_register(RAD_ADDR,0x27,0x0);

  // Go to Sleep
  delay(MBUS_DELAY);
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

//***************************************************
// Temperature measurement operation (SNSv2)
//***************************************************
static void operation_temp(void){

  // Check if wakeup is due to temperature sensor interrupt
  // If so, then skip this section

  if ( exec_temp_marker != 0x87654321 ) {

    // This wakeup is just to enable temp sensor
    exec_count = exec_count - 1;

    // Set exec_temp_marker
    exec_temp_marker = 0x87654321;

    //Enable T Sensor
    _sns_r3 = (0x3<<17)|(0x0<<16)|(0xF<<12)|(0x0<<8)|(0xF<<4)|(0x0<<0);
    delay(MBUS_DELAY);
    write_mbus_register(SNS_ADDR,3,_sns_r3);
    
    operation_sleep();
  }

  delay(MBUS_DELAY);

  // Grab Data after IRQ
  temp_data = *((volatile uint32_t *) IMSG1);

  // Store in memory
  // If the buffer is full, then skip
  if (temp_data_count<DATA_BUFFER_SIZE){
    temp_data_stored[temp_data_count] = temp_data;
    temp_data_count = temp_data_count + 1;
  }

  // Disable T Sensor
  _sns_r3 = (0x3<<17)|(0x1<<16)|(0xF<<12)|(0x0<<8)|(0xF<<4)|(0x0<<0);
  delay(MBUS_DELAY);
  write_mbus_register(SNS_ADDR,3,_sns_r3);

  // Reset exec_temp_marker
  exec_temp_marker = 0;

/*
  //Fire off data to radio
  radio_data = gen_radio_data(temp_data>>5);
  delay(MBUS_DELAY);

  send_radio_data(radio_data);
*/


  // Set up wake up timer register
  // Initial cycles have different wakeup time
  if (exec_count < NUM_INITIAL_CYCLE){
    set_wakeup_timer(TEMP_WAKEUP_CYCLE_INITIAL,1,0);
  }
  else {
    set_wakeup_timer(TEMP_WAKEUP_CYCLE,1,0);
  }

  operation_sleep();

}

//***************************************************
// Temperature measurement operation (SNSv2)
//***************************************************
static void operation_radio(void){

  //Fire off stored data to radio
  uint32_t i;
  for (i=0;i<temp_data_count;i++){

    radio_data = gen_radio_data(temp_data_stored[i]>>5);
    delay(MBUS_DELAY);
  
    send_radio_data(radio_data);
    delay(RAD_PACKET_DELAY); //Set delays between sending subsequent packet
  }

  delay(MBUS_DELAY);

  // Reset temp_data_count
  temp_data_count = 0;

  operation_temp();
  //operation_sleep();
}


//***************************************************
// Main
//***************************************************
int main() {

  //Clear All Pending Interrupts
  *((volatile uint32_t *) 0xE000E280) = 0x3FF;
  //Enable Interrupts
  *((volatile uint32_t *) 0xE000E100) = 0x3FF;

  //Set PMU Division to 5
  //*((volatile uint32_t *) 0xA200000C) = 0x4F770029; 
  //Set PMU Division to 6
  //*((volatile uint32_t *) 0xA200000C) = 0x0F77002B;



  //Check if it is the first execution
  if ( exec_marker != 0x12345678 ) {

    // Initial routine

    // Set PMU Strength & division threshold
    // Change PMU_CTRL Register
    // 0x0F770029 = Original
    // Increase sleep oscillator frequency for GOC and temp sensor
    // Decrease 5x division switching threshold
    *((volatile uint32_t *) 0xA200000C) = 0x0F77004B;

    // Speed up GOC frontend to match PMU frequency
    *((volatile uint32_t *) 0xA2000008) = 0x0020290C;


    //Mark execution
    exec_marker = 0x12345678;
    exec_count = 0;
    exec_count_irq = 0;

    //Enumeration
    enumerate(RAD_ADDR);
    asm ("wfi;");
    delay(MBUS_DELAY);
    enumerate(SNS_ADDR);
    asm ("wfi;");

    // Setup Radio
    setup_radio();

    // Setup T Sensor
    setup_tempsensor();

    // Initialize program variables
    temp_data_count = 0;

  }else{

    // Repeating wakeup routine 
    exec_count = exec_count + 1;

    // Disable wakeup timer
    set_wakeup_timer(0,0,1);

  }

  // Repeating wakeup routine 

  // Check if wakeup is due to GOC interrupt
  // 0x68 is reserved for GOC-triggered wakeup (Named IRQ10VEC)
  // 8 MSB bits of the wakeup data are used for function ID

  uint32_t wakeup_data = *((volatile uint32_t *) IRQ10VEC);
  uint32_t wakeup_data_header = wakeup_data>>24;

  if(wakeup_data_header == 1){
    // Debug mode: Transmit something via radio 16 times and go to sleep w/o timer
    if (exec_count_irq < 16){
      exec_count_irq++;
      // radio
//    send_radio_data(0xF0F0F0F0);
      send_radio_data (gen_radio_data(0x6C));
      // set timer
      set_wakeup_timer (TEMP_WAKEUP_CYCLE_INITIAL, 0x1, 0x0);
      // go to sleep and wake up with same condition
      operation_sleep_noirqreset();
    }else{
      exec_count_irq = 0;
      // radio
//    send_radio_data(0xF0F0F0F0);
      send_radio_data (gen_radio_data(0x6C));
      // Disable Timer
      set_wakeup_timer (0, 0x0, 0x0);
      // Go to sleep without timer
      operation_sleep();
    }

  }else if(wakeup_data_header == 2){
      // Temp operation
      operation_temp();

  }else if(wakeup_data_header == 3){
      // Disable Timer
      set_wakeup_timer (0, 0x0, 0x0);
      // Go to sleep without timer
      operation_sleep();

  }else if(wakeup_data_header == 4){
      // Transmit data via radio and go to sleep
      operation_radio();

  }else{
      // Default case 
      operation_temp();
  }

  // Note: Program should send system to sleep in the switch statement
  // Program should not reach this point

  while(1);

}
