//*******************************************************************
//Author: Yejoong Kim Ziyun Li Cao Gao
//Description: M0 code for Deep Learning Processor projec
// MACNLI AND MOV TESTING
//*******************************************************************
#include "DLCv1.h"
//#include "FLSv2S_RF.h"
//#include "PMUv2_RF.h"
#include "DLCv1_RF.h"
#include "mbus.h"
#include "dnn_parameters.h"

//#define PRC_ADDR    0x1
#define DLC_ADDR    0x4
//#define NODE_M_ADDR 0x8
//#define NODE_B_ADDR 0xC
//#define PMU_ADDR    0xE


//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t cyc_num;
volatile uint32_t irq_history;

// Just for testing...
volatile uint32_t do_cycle0  = 1; // 
volatile uint32_t do_cycle1  = 1; // PMU Testing
volatile uint32_t do_cycle2  = 1; // Register test
volatile uint32_t do_cycle3  = 1; // MEM IRQ
volatile uint32_t do_cycle4  = 1; // Flash Erase
volatile uint32_t do_cycle5  = 1; // Memory Streaming 1
volatile uint32_t do_cycle6  = 1; // Memory Streaming 2
volatile uint32_t do_cycle7  = 1; // TIMER16
volatile uint32_t do_cycle8  = 1; // TIMER32
volatile uint32_t do_cycle9  = 1; // Watch-Dog
volatile uint32_t do_cycle10 = 0; // 
volatile uint32_t do_cycle11 = 0; // 

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_0(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_4(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_5(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_6(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_7(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_8(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_9(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_12(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_13(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0(void) { *NVIC_ICPR = (0x1 << 0); // TIMER32
    irq_history |= (0x1 << 0);
    *REG0 = 0x1000;
    *REG1 = 0x1;//*TIMER32_CNT;
    *REG2 = 0x1;///*TIMER32_STAT;
    //*TIMER32_STAT = 0x0;
    arb_debug_reg (0xA0000000);
    arb_debug_reg ((0xA1 << 24) | *REG0); // 0x1000
    arb_debug_reg ((0xA1 << 24) | *REG1); // TIMER32_CNT
    arb_debug_reg ((0xA1 << 24) | *REG2); // TIMER32_STAT
    }
void handler_ext_int_1(void) { *NVIC_ICPR = (0x1 << 1); // TIMER16
    irq_history |= (0x1 << 1);
    *REG0 = 0x1001;
    *REG1 = 0x1;//*TIMER16_CNT;
    *REG2 = 0x1;//*TIMER16_STAT;
    //*TIMER16_STAT = 0x0;
    arb_debug_reg (0xA0000001);
    arb_debug_reg ((0xA1 << 24) | *REG0); // 0x1001
    arb_debug_reg ((0xA1 << 24) | *REG1); // TIMER16_CNT
    arb_debug_reg ((0xA1 << 24) | *REG2); // TIMER16_STAT
    }
void handler_ext_int_2(void) { *NVIC_ICPR = (0x1 << 2); // REG0
    irq_history |= (0x1 << 2);
    arb_debug_reg (0xA0000002);
}
void handler_ext_int_3(void) { *NVIC_ICPR = (0x1 << 3); // REG1
    irq_history |= (0x1 << 3);
    arb_debug_reg (0xA0000003);
}
void handler_ext_int_4(void) { *NVIC_ICPR = (0x1 << 4); // REG2
    irq_history |= (0x1 << 4);
    arb_debug_reg (0xA0000004);
}
void handler_ext_int_5(void) { *NVIC_ICPR = (0x1 << 5); // REG3
    irq_history |= (0x1 << 5);
    arb_debug_reg (0xA0000005);
}
void handler_ext_int_6(void) { *NVIC_ICPR = (0x1 << 6); // REG4
    irq_history |= (0x1 << 6);
    arb_debug_reg (0xA0000006);
}
void handler_ext_int_7(void) { *NVIC_ICPR = (0x1 << 7); // REG5
    irq_history |= (0x1 << 7);
    arb_debug_reg (0xA0000007);
}
void handler_ext_int_8(void) { *NVIC_ICPR = (0x1 << 8); // REG6
    irq_history |= (0x1 << 8);
    arb_debug_reg (0xA0000008);
}
void handler_ext_int_9(void) { *NVIC_ICPR = (0x1 << 9); // REG7
    irq_history |= (0x1 << 9);
    arb_debug_reg (0xA0000009);
}
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); // MEM WR
    irq_history |= (0x1 << 10);
    arb_debug_reg (0xA000000A);
}
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); // MBUS_RX
    irq_history |= (0x1 << 11);
    arb_debug_reg (0xA000000B);
}
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); // MBUS_TX
    irq_history |= (0x1 << 12);
    arb_debug_reg (0xA000000C);
}
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); // MBUS_FWD
    irq_history |= (0x1 << 13);
    arb_debug_reg (0xA000000D);
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    enumerated = 0xDEADBEEF;
    cyc_num = 0;
    irq_history = 0;

    // Set Halt
    set_halt_until_mbus_rx();

    // Enumeration
    //mbus_enumerate(DLC_ADDR);

    //Set Halt
    set_halt_until_mbus_tx();
}

void pass (uint32_t id, uint32_t data) {
    arb_debug_reg (0x0EA7F00D);
    arb_debug_reg (id);
    arb_debug_reg (data);
}

void fail (uint32_t id, uint32_t data) {
    arb_debug_reg (0xDEADBEEF);
    arb_debug_reg (id);
    arb_debug_reg (data);
    //*REG_CHIP_ID = 0xFFFF; // This will stop the verilog sim.
}

//********************************************************************
// MAIN function starts here             
//********************************************************************
// #define IN_ADDR 5632
#define READ_SRAM_DELAY 6
#define MBUS_DELAY 1500
#define OUT_ADDR 5491


//Read 100
// #define OUT_ADDR 5491
// #define READ_SRAM_ADDR        ((volatile uint32_t *) 0x00005164)

//Read 4
#define OUT_ADDR 5587
#define READ_SRAM_ADDR        ((volatile uint32_t *) 0x000055E4)
//#define START_ADDR 4000 //5th SRAM_4K  bank
#define START_ADDR        ((volatile uint32_t *) 0x00001000)

// #define READ_SRAM_ADDR        ((volatile uint32_t *) 0x00004000)

uint32_t a[4];
uint32_t error_cnt;
uint32_t error_pe;
uint32_t error_addr;
uint32_t error_subword;
uint32_t error_bit;

void error_msg()
{
	// a[0]=error_cnt;
	a[0]=error_pe;
	a[1]=error_addr;
	a[2]=error_subword;
	a[3]=error_bit;
	mbus_write_message(0x14,a,4);
}


int main() {
    //Initialize Interrupts
    //disable_all_irq();
	set_dnn_insts();
	*DNN_PG_0 = 0x000007ff;
	*DNN_PG_1 = 0x000007ff;
	*DNN_PG_2 = 0x000007ff;
	*DNN_PG_3 = 0x000007ff;
	*DNN_SRAM_ISOL_0 = 0x000007FF;
	*DNN_SRAM_ISOL_1 = 0x000007FF;
	*DNN_SRAM_ISOL_2 = 0x000007FF;
	*DNN_SRAM_ISOL_3 = 0x000007FF;
	*DNN_RAND_0 = 1;
    *DNN_RAND_1 = 1;
    *DNN_RAND_2 = 1;
    *DNN_RAND_3 = 1;
	uint32_t short_addr = OUT_ADDR & 0x1fff;
	
	// *READ_SRAM_ADDR = *DNN_PG_0;
	// *(READ_SRAM_ADDR+1) = *REG_RUN_CPU;
	// *(READ_SRAM_ADDR+2) = *DNN_DATA_0_3;
	// READ_SRAM_ADDR[3] = *REG_RUN_CPU;
	// READ_SRAM_ADDR[4] = *DNN_DATA_0_3;
	
	
	
	
	// *DNN_CTRL_0 = (short_addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	// delay(READ_SRAM_DELAY);
	// *(READ_SRAM_ADDR+3) = *DNN_R_DATA_0_0;
	// *(READ_SRAM_ADDR+2) = *DNN_R_DATA_0_1;
	// *(READ_SRAM_ADDR+1) = *DNN_R_DATA_0_2;
	// *(READ_SRAM_ADDR+0) = *DNN_R_DATA_0_3;

	
	// uint32_t i,y0, y1, y2, y3;
		// y0 = *DNN_R_DATA_0_0;
		// y1 = *DNN_R_DATA_0_1;
		// y2 = *DNN_R_DATA_0_2;
		// y3 = *DNN_R_DATA_0_3;
		
	// *(READ_SRAM_ADDR)   = (y3<<8)  | ((y2>>16) & 0x000000ff); 
	// *(READ_SRAM_ADDR+1) = (y2<<16) | ((y1>>8)  & 0x0000ffff); 
	// *(READ_SRAM_ADDR+2) = (y1<<24) | (y0       & 0x00ffffff); 
	
	uint32_t i,y0, y1, y2, y3;
	uint32_t out1,out2,out3;
	uint32_t data,data1,data2,data3;
	int pe;
	
	// uint32_t a[5];
	// uint32_t error_cnt;
	// uint32_t error_pe;
	// uint32_t error_addr;
	// uint32_t error_subword;
	// uint32_t error_bit;
	
	error_cnt=0;
	error_pe=0;
	error_addr=0;
	error_subword=0;	
	error_bit=0;
	
	//Data Pattern: checker board/ ALL ONE/ ALL ZERO
	// data=0xFFFFFFFF;
	// data=0x00000000;
	data=0xAAAAAAAA;
	// data=0x55555555;
	/* for(pe=0;pe<4;pe++){
		for(i=0;i<5760;i++)
		{
			switch(pe){
				case 0:
					*DNN_CTRL_0 = (i << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
					delay(READ_SRAM_DELAY);
					y0 = *DNN_R_DATA_0_0;
					y1 = *DNN_R_DATA_0_1;
					y2 = *DNN_R_DATA_0_2;
					y3 = *DNN_R_DATA_0_3;
					break;
				case 1:
					*DNN_CTRL_1 = (i << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
					delay(READ_SRAM_DELAY);
					y0 = *DNN_R_DATA_1_0;
					y1 = *DNN_R_DATA_1_1;
					y2 = *DNN_R_DATA_1_2;
					y3 = *DNN_R_DATA_1_3;
					break;
				case 2:
					*DNN_CTRL_2 = (i << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
					delay(READ_SRAM_DELAY);
					y0 = *DNN_R_DATA_2_0;
					y1 = *DNN_R_DATA_2_1;
					y2 = *DNN_R_DATA_2_2;
					y3 = *DNN_R_DATA_2_3;
					break;
				case 3:
					*DNN_CTRL_3 = (i << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
					delay(READ_SRAM_DELAY);
					y0 = *DNN_R_DATA_3_0;
					y1 = *DNN_R_DATA_3_1;
					y2 = *DNN_R_DATA_3_2;
					y3 = *DNN_R_DATA_3_3;
					break;
			}
			
			out1 = (y3<<8)  | ((y2>>16) & 0x000000ff); 
			out2 = (y2<<16) | ((y1>>8)  & 0x0000ffff); 
			out3 = (y1<<24) | (y0       & 0x00ffffff); 

			if(out1 != data){
				error_cnt++;
				error_pe = pe;
				error_addr = i;
				error_subword = 1;
				error_bit = out1^data;
				error_msg();
				delay(MBUS_DELAY);
			}
			if(out2 != data){
				error_cnt++;
				error_pe = pe;
				error_addr = i;
				error_subword = 2;
				error_bit = out2^data;
				error_msg();
				delay(MBUS_DELAY);
			}
			if(out3 != data){
				error_cnt++;
				error_pe = pe;
				error_addr = i;
				error_subword = 3;
				error_bit = out3^data;
				error_msg();
			}
			data=~data;
		}//for(i)
	}//for(pe) */
	
	
	
	
	//Data Pattern: Address
 	// for(pe=0;pe<4;pe++){
		// data1=0;
		// data2=1;
		// data3=2;
		// for(i=0;i<5760;i++)
		// {
			// switch(pe){
				// case 0:
					// *DNN_CTRL_0 = (i << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
					// delay(READ_SRAM_DELAY);
					// y0 = *DNN_R_DATA_0_0;
					// y1 = *DNN_R_DATA_0_1;
					// y2 = *DNN_R_DATA_0_2;
					// y3 = *DNN_R_DATA_0_3;
					// break;
				// case 1:
					// *DNN_CTRL_1 = (i << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
					// delay(READ_SRAM_DELAY);
					// y0 = *DNN_R_DATA_1_0;
					// y1 = *DNN_R_DATA_1_1;
					// y2 = *DNN_R_DATA_1_2;
					// y3 = *DNN_R_DATA_1_3;
					// break;
				// case 2:
					// *DNN_CTRL_2 = (i << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
					// delay(READ_SRAM_DELAY);
					// y0 = *DNN_R_DATA_2_0;
					// y1 = *DNN_R_DATA_2_1;
					// y2 = *DNN_R_DATA_2_2;
					// y3 = *DNN_R_DATA_2_3;
					// break;
				// case 3:
					// *DNN_CTRL_3 = (i << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
					// delay(READ_SRAM_DELAY);
					// y0 = *DNN_R_DATA_3_0;
					// y1 = *DNN_R_DATA_3_1;
					// y2 = *DNN_R_DATA_3_2;
					// y3 = *DNN_R_DATA_3_3;
					// break;
			// }
			
			// out1 = (y3<<8)  | ((y2>>16) & 0x000000ff); 
			// out2 = (y2<<16) | ((y1>>8)  & 0x0000ffff); 
			// out3 = (y1<<24) | (y0       & 0x00ffffff); 
			

			// if(out1 != data1){
				// error_cnt++;
				// error_pe = pe;
				// error_addr = i;
				// error_subword = 1;
				// // error_bit = out1^data1;
				// error_bit = out1;
				// error_msg();
				// delay(MBUS_DELAY);
			// }
			// if(out2 != data2){
				// error_cnt++;
				// error_pe = pe;
				// error_addr = i;
				// error_subword = 2;
				// // error_bit = out2^data2;
				// error_bit = out2;
				// error_msg();
				// delay(MBUS_DELAY);
			// }
			// if(out3 != data3){
				// error_cnt++;
				// error_pe = pe;
				// error_addr = i;
				// error_subword = 3;
				// // error_bit = out3^data3;
				// error_bit = out3;
				// error_msg();
			// }
			// data1+=3;
			// data2+=3;
			// data3+=3;
		// }//for(i)
	// }//for(pe)





/* 	for(i=0;i<5760;i++)
	{
		*DNN_CTRL_1 = (i << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		delay(READ_SRAM_DELAY);
		y0 = *DNN_R_DATA_1_0;
		y1 = *DNN_R_DATA_1_1;
		y2 = *DNN_R_DATA_1_2;
		y3 = *DNN_R_DATA_1_3;
		
		out1 = (y3<<8)  | ((y2>>16) & 0x000000ff); 
		out2 = (y2<<16) | ((y1>>8)  & 0x0000ffff); 
		out3 = (y1<<24) | (y0       & 0x00ffffff); 

		if(out1 != data){
			error_cnt++;
			error_pe = 1;
			error_addr = i;
			error_subword = 1;
			error_bit = out1^data;
			error_msg();
		}
		if(out2 != data){
			error_cnt++;
			error_pe = 1;
			error_addr = i;
			error_subword = 2;
			error_bit = out2^data;
			error_msg();
		}
		if(out3 != data){
			error_cnt++;
			error_pe = 1;
			error_addr = i;
			error_subword = 3;
			error_bit = out3^data;
			error_msg();
		}
		data=~data;
	} */
	
/* 	for(i=0;i<5760;i++)
	{
		*DNN_CTRL_2 = (i << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		delay(READ_SRAM_DELAY);
		y0 = *DNN_R_DATA_2_0;
		y1 = *DNN_R_DATA_2_1;
		y2 = *DNN_R_DATA_2_2;
		y3 = *DNN_R_DATA_2_3;
		
		out1 = (y3<<8)  | ((y2>>16) & 0x000000ff); 
		out2 = (y2<<16) | ((y1>>8)  & 0x0000ffff); 
		out3 = (y1<<24) | (y0       & 0x00ffffff); 

		if(out1 != data){
			error_cnt++;
			error_pe = 2;
			error_addr = i;
			error_subword = 1;
			error_bit = out1^data;
			error_msg();
		}
		if(out2 != data){
			error_cnt++;
			error_pe = 2;
			error_addr = i;
			error_subword = 2;
			error_bit = out2^data;
			error_msg();
		}
		if(out3 != data){
			error_cnt++;
			error_pe = 2;
			error_addr = i;
			error_subword = 3;
			error_bit = out3^data;
			error_msg();
		}
		data=~data;		
	} */
	
/* 	for(i=0;i<5760;i++)
	{
		*DNN_CTRL_3 = (i << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		delay(READ_SRAM_DELAY);
		y0 = *DNN_R_DATA_3_0;
		y1 = *DNN_R_DATA_3_1;
		y2 = *DNN_R_DATA_3_2;
		y3 = *DNN_R_DATA_3_3;
		
		out1 = (y3<<8)  | ((y2>>16) & 0x000000ff); 
		out2 = (y2<<16) | ((y1>>8)  & 0x0000ffff); 
		out3 = (y1<<24) | (y0       & 0x00ffffff); 

		if(out1 != data){
			error_cnt++;
			error_pe = 3;
			error_addr = i;
			error_subword = 1;
			error_bit = out1^data;
			error_msg();
		}
		if(out2 != data){
			error_cnt++;
			error_pe = 3;
			error_addr = i;
			error_subword = 2;
			error_bit = out2^data;
			error_msg();
		}
		if(out3 != data){
			error_cnt++;
			error_pe = 3;
			error_addr = i;
			error_subword = 3;
			error_bit = out3^data;
			error_msg();
		}
		data=~data;		
	} */
	
	
	
	for(i=0; i<1024*4; i++)
	{
		y0 = *(START_ADDR+i);
		if(y0 != data) {
			error_cnt++;
			error_pe = 5;
			error_addr = 1024+i;
			error_subword = 3;
			//error_bit = y0^data;
			error_bit = y0;
			error_msg();
			delay(14000);
		}
		data=~data;	
	}
	
	// int face = (out1>out2)&0x00000001;
	a[0]=error_cnt;
	// a[1]=error_pe;
	// a[2]=error_addr;
	// a[3]=error_subword;
	// a[4]=error_bit;
	mbus_write_message(0x14,a,1);
	
	
	delay(7000);
	*REG_RUN_CPU = 0x00000000;
    
  		// done
		//////////////////////////////////////////////////////


    return 1;
}
