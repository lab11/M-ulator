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

//#include "../../compiler/memory/dnn_0L4B1.h"


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
#define IN_ADDR 5632
#define WRITE_SRAM_DELAY 6
#define READ_SRAM_DELAY 6
#define READ_SRAM_ADDR        ((volatile uint32_t *) 0x00003000)


#define WRITE_SRAM_ADDR        ((volatile uint32_t *) 0x00004000)
#define WRITE_SRAM_SIZE 640
#define OUT_ADDR 5120

int main() {
    //Initialize Interrupts
    //disable_all_irq();
	set_dnn_insts();
	*DNN_PG_0 = 0x000007ff;
	*DNN_PG_1 = 0x000007ff;
	*DNN_PG_2 = 0x000007ff;
	*DNN_PG_3 = 0x000007ff;
	
	//set_dnn_data();


	
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
	uint32_t i;
	//uint32_t y0, y1, y2, y3;
	//uint32_t x0,x1,x2,x3;
	//uint32_t z0, z1, z2, z3;
	
	for(i=0;i<WRITE_SRAM_SIZE;i++)
	{  
	  	*DNN_DATA_0_3   = (*(WRITE_SRAM_ADDR+3*i) >> 8) & 0x00ffffff;
	  	*DNN_DATA_0_2   = (*(WRITE_SRAM_ADDR+3*i) << 16) + ((*(WRITE_SRAM_ADDR+3*i+1) >> 16) & 0x0000ffff);
	  	*DNN_DATA_0_1   = ((*(WRITE_SRAM_ADDR+3*i+1) & 0xffff) << 8) + ((*(WRITE_SRAM_ADDR+3*i+2) >> 24) & 0x000000ff);
	  	*DNN_DATA_0_0   = *(WRITE_SRAM_ADDR+3*i+2) & 0xffffff;
	  	*DNN_CTRL_0 = (OUT_ADDR + i << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	}
	// if(array_size > 1){
		// for(i=0;i<array_size-1;)
		// {
			// x3 = PE_MEM[i++];
			// x2 = PE_MEM[i++];
			// x1 = PE_MEM[i++];
			// x0 = PE_MEM[i++];
			

			// y0 = x0 & 0xffffff;																				// x0 last 24 bits
			// y1 = ((x1 & 0xffff) << 8) + ((x0 >> 24) & 0x000000ff);			// x1 last 16 bits + x0 first 8 bits. the last AND op is to avoid x0 being signed 
			// y2 = ((x2 & 0xff) << 16) + ((x1 >> 16) & 0x0000ffff);	  	// x2 last 8 bits + x1 first 16 bits 
			// y3 = (x2 >> 8) & 0x00ffffff; 															// x2 first 24 bits
	  
	  		// *DNN_DATA_0_3   = y3;
	  		// *DNN_DATA_0_2   = y2;
	  		// *DNN_DATA_0_1   = y1;
	  		// *DNN_DATA_0_0   = y0;
			
	  		// *DNN_CTRL_0 = (x3 << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		
			// //delay(WRITE_SRAM_DELAY);
		// }
		
		// for(i=0;i<array_size-1;i++)
		// {
	  
	  		// *DNN_DATA_0_3   = (PE_MEM[i][1] >> 8) & 0x00ffffff;
	  		// *DNN_DATA_0_2   = ((PE_MEM[i][1] & 0xff) << 16) + ((PE_MEM[i][2] >> 16) & 0x0000ffff);
	  		// *DNN_DATA_0_1   = ((PE_MEM[i][2] & 0xffff) << 8) + ((PE_MEM[i][3] >> 24) & 0x000000ff);
	  		// *DNN_DATA_0_0   = PE_MEM[i][3] & 0xffffff;
			
	  		// *DNN_CTRL_0 = (PE_MEM[i][0] << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
		// }
		
		
	// }
			// x0 = 0x00000000;
			// x1 = 0x89ABCDEF;
			// x2 = 0x01234567;
			// y0 = x0 & 0xffffff;																				// x0 last 24 bits
			// y1 = ((x1 & 0xffff) << 8) + ((x0 >> 24) & 0x000000ff);			// x1 last 16 bits + x0 first 8 bits. the last AND op is to avoid x0 being signed 
			// y2 = ((x2 & 0xff) << 16) + ((x1 >> 16) & 0x0000ffff);	  	// x2 last 8 bits + x1 first 16 bits 
			// y3 = (x2 >> 8) & 0x00ffffff; 															// x2 first 24 bits
	  
	  		// *DNN_DATA_0_3   = y3;
	  		// *DNN_DATA_0_2   = y2;
	  		// *DNN_DATA_0_1   = y1;
	  		// *DNN_DATA_0_0   = y0;
	  		// *DNN_CTRL_0 = (0 << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	// delay(7000);
	
		// i=0;
		// z0 = *DNN_DATA_0_0;
		// z1 = *DNN_DATA_0_1;
		// z2 = *DNN_DATA_0_2;
		// z3 = *DNN_DATA_0_3;
		
		// *(READ_SRAM_ADDR+3*i)   = (z3<<8)  | ((z2>>16) & 0x000000ff); 
		// *(READ_SRAM_ADDR+3*i+1) = (z2<<16) | ((z1>>8)  & 0x0000ffff); 
		// *(READ_SRAM_ADDR+3*i+2) = (z1<<24) | (z0       & 0x00ffffff); 
	
	*REG_RUN_CPU = 0x00000000;
    
  		// done
		//////////////////////////////////////////////////////


    return 1;
}
