//*******************************************************************
// Author: Zhiyoong Foo
//Description: Code for IAI demonstration
//             Takes NUM_SAMPLES readings of Temperature from SNSv1
//             Stores into array
//             Throws data into radio
//*******************************************************************
#include "m3_proc.h"

//Defines for easy handling
#define RAD_ADDR 0x4           //RADv4 Short Address

#define NUM_SAMPLES 1000         //Number of T samples to take
#define RAD_BIT_DELAY 0x54     //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 1000  //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define RAD_SAMPLE_DELAY 10000 //10000   //Radio tuning: Delay between samples sent (NUM_SAMPLES sent)

//************************************
//Interrupt Handlers
//Must clear pending bit!
//************************************
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
//************************************
//Internal Functions
//************************************

//************************************
//Delay for X Core Clock ticks
//************************************
static void delay(unsigned ticks) {
  unsigned i;
  for (i=0; i < ticks; i++)
    asm("nop;");
}

static void enable_all_irq(){
  //Enable Interrupts
  //Page 139 of M3 Book
  *((volatile uint32_t *) 0xE000E100) = 0xF; //Actually only enables external 0 -3
}

static void clear_all_pend_irq(){
  //Clear All Pending Interrupts
  //Page 140 of M3 Book
  *((volatile uint32_t *) 0xE000E280) = 0xF;  //Actually only clears external 0 -3
}


int main() {
  //Interrupts
  clear_all_pend_irq();
  enable_all_irq();

  //Enumeration
  //enumerate(RAD_ADDR);
  delay(10000);
 
  
  //delay (1); //Need to Delay in between for some reason.
    
  delay(10000);
  uint32_t _rad_r0 = 0x0; //PG
  uint32_t _rad_r1 = 0x0; //OD_PICK
  uint32_t _rad_r2 = 0x1; //CLKEXT_EN
  uint32_t _rad_r3 = 0x0; //CLKGEN_EN
  uint32_t _rad_r4 = 0x8; //DIV_PICK
  uint32_t _rad_r5 = 0x1; //RING_PICK
  uint32_t _rad_r6 = 0x7; 
  uint32_t _rad_r7 = 0x8;
  uint32_t _rad_r8 = 0x7;
  uint32_t _rad_r9 = 0x8;
  uint32_t _rad_r10 = 0x7;
  uint32_t _rad_r11 = 0x8;
  uint32_t _rad_r12 = 0x1;
  uint32_t _rad_r13 = 0x1;
  uint32_t _rad_r14 = 0x1;
  uint32_t _rad_r15 = 0x1; //AMPGL_CONT
  uint32_t _rad_r16 = 0x0; //CDCEN
  uint32_t _rad_r17 = 0x0;
  uint32_t _rad_r18 = 0x1F; //CBCAP_CONT
  uint32_t _rad_r19 = 0x1; //DUALCOMP_EN
  uint32_t _rad_r20 = 0x1; //COMP_SEL
  uint32_t _rad_r21 = 0x0;
  uint32_t _rad_r22 = 0x0;
  uint32_t _rad_r23 = 0x0; //OSR_CONT
  uint32_t _rad_r24 = 0x2; //CMSP_CONT
  uint32_t _rad_r25 = 0x0; //CMSTUCK_EN
  uint32_t _rad_r26 = 0x0; //CRSTUCK_EN
 
  write_mbus_register(RAD_ADDR,0x0,_rad_r0);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x1,_rad_r1);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x2,_rad_r2);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x3,_rad_r3);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x4,_rad_r4);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x5,_rad_r5);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x6,_rad_r6);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x7,_rad_r7);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x8,_rad_r8);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x9,_rad_r9);
  delay(1000);
  write_mbus_register(RAD_ADDR,0xa,_rad_r10);
  delay(1000);
  write_mbus_register(RAD_ADDR,0xb,_rad_r11);
  delay(1000);
  write_mbus_register(RAD_ADDR,0xc,_rad_r12);
  delay(1000);
  write_mbus_register(RAD_ADDR,0xd,_rad_r13);
  delay(1000);
  write_mbus_register(RAD_ADDR,0xe,_rad_r14);
  delay(1000);
  write_mbus_register(RAD_ADDR,0xf,_rad_r15);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x10,_rad_r16);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x11,_rad_r17);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x12,_rad_r18);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x13,_rad_r19);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x14,_rad_r20);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x15,_rad_r21);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x16,_rad_r22);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x17,_rad_r23);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x18,_rad_r24);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x19,_rad_r25);
  delay(1000);
  write_mbus_register(RAD_ADDR,0x1a,_rad_r26);
  delay(1000);
  
  
  //Sanity Checks
  //int i;
  //for (i=0; i<NUM_SAMPLES; i++) {
  while(1){
    delay(500);
    write_mbus_register(RAD_ADDR,0x10,0x0); //CDCEN=0
	delay(500);
    write_mbus_register(RAD_ADDR,0x10,0x1); //CDCEN=1
	delay(5000);
    read_mbus_register_RADv4(RAD_ADDR,0x1e,0x13); //Read COUNTOUT
	delay(5000);
	}
  //}

  while (1){}

}
