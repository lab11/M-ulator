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
#define IN_ADDR 5632
#define OUT_ADDR 5632
#define READ_SRAM_DELAY 6
#define READ_SRAM_ADDR        ((volatile uint32_t *) 0x00003000)

int main() {
    //Initialize Interrupts
    //disable_all_irq();

//    if (*REG_CHIP_ID != 0x1234) while(1);

		uint16_t inst_no;
		set_dnn_insts();

    // *REG_WAIT_BEFORE_VDD = 0xff;
      // *DNN_SRAM_RSTN_0 = 0x000007ff;
      // *DNN_SRAM_RSTN_1 = 0x000007ff;
      // *DNN_SRAM_RSTN_2 = 0x000007ff;
      // *DNN_SRAM_RSTN_3 = 0x000007ff;
      // delay(3);

      // *DNN_SRAM_ISOL_0 = 0x000007ff;
      // *DNN_SRAM_ISOL_1 = 0x000007ff;
      // *DNN_SRAM_ISOL_2 = 0x000007ff;
      // *DNN_SRAM_ISOL_3 = 0x000007ff;
      delay(3);

      *DNN_PG_0 = 0x000007ff;
      *DNN_PG_1 = 0x000007ff;
      *DNN_PG_2 = 0x000007ff;
      *DNN_PG_3 = 0x000007ff;
      delay(5);

      //*DNN_PG_0 = 0x003ff800;
      //*DNN_PG_1 = 0x003ff800;
      //*DNN_PG_2 = 0x003ff800;
      //*DNN_PG_3 = 0x003ff800;
      //delay(3);

      // *DNN_SRAM_RSTN_0 = 0x000007ff;
      // *DNN_SRAM_RSTN_0 = 0xffffffff;
      // *DNN_SRAM_RSTN_1 = 0xffffffff;
      // *DNN_SRAM_RSTN_2 = 0xffffffff;
      // *DNN_SRAM_RSTN_3 = 0xffffffff;
      delay(3);
	//Random Decoder
     *DNN_RAND_0 = 1;
     *DNN_RAND_1 = 1;
     *DNN_RAND_2 = 1;
     *DNN_RAND_3 = 1;

	//Sequential Decoder
	 // *DNN_RAND_0 = 0;
     // *DNN_RAND_1 = 0;
     // *DNN_RAND_2 = 0;
     // *DNN_RAND_3 = 0;
//      delay(3);
      
      //*DNN_SRAM_ISOL_0 = 0x00000000;
      //*DNN_SRAM_ISOL_1 = 0x00000000;
      //*DNN_SRAM_ISOL_2 = 0x00000000;
      //*DNN_SRAM_ISOL_3 = 0x00000000;
      //delay(3);

		//////////////////////////////////////////////////////
    // working sequence
      ///////////////////
      // a.) mov inputs and offsets to other banks
      // 
      set_all_buffer0();
      inst_no = 0;
      ///
	  write_instruction(1, 0, 0);   // write MOV0
			start_pe_inst(0b0001);              // start MOV0
			//while (check_if_pe_finish(0b0001) != 1) { delay(5);}
		delay(7000);
      signal_debug(1);
	  
      write_instruction(inst_no, 0, 0);   // write MOV0
			start_pe_inst(0b0001);              // start MOV0
			inst_no++;
			while (check_if_pe_finish(0b0001) != 1) { delay(5);}
		//delay(7000);
      //signal_debug(1);

      write_instruction(inst_no, 0, 0);   // write MOV1
			start_pe_inst(0b0001);              // start MOV1
			inst_no++;
			while (check_if_pe_finish(0b0001) != 1) { delay(5);}
		//delay(7000);
      //signal_debug(2);
	  
      // b.) start MAC 
      write_instruction(inst_no, 0, 0);   // write MAC
			start_pe_inst(0b0001);              // start MAC
			inst_no++;
			while (check_if_pe_finish(0b0001) != 1) { delay(5);}
	  //delay(7000);
	  //delay(14000);
      //clock_gate();
      //signal_debug(0);
      signal_done();
      delay(7000);
	  
	
	
	//Read DNN MEMORY and Store to ARM MEMORY
	uint32_t short_addr = 5120;
	uint32_t i,y0, y1, y2, y3;
	for(i=0;i<9;i++)
	{
		*DNN_CTRL_0 = (short_addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		delay(READ_SRAM_DELAY);
		y0 = *DNN_R_DATA_0_0;
		y1 = *DNN_R_DATA_0_1;
		y2 = *DNN_R_DATA_0_2;
		y3 = *DNN_R_DATA_0_3;
		
		*(READ_SRAM_ADDR+3*i)   = (y3<<8)  | ((y2>>16) & 0x000000ff); 
		*(READ_SRAM_ADDR+3*i+1) = (y2<<16) | ((y1>>8)  & 0x0000ffff); 
		*(READ_SRAM_ADDR+3*i+2) = (y1<<24) | (y0       & 0x00ffffff); 
		
		short_addr = short_addr+1;
	}
	
	short_addr = 4096;
	for(i=9;i<11;i++)
	{
		*DNN_CTRL_0 = (short_addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		delay(READ_SRAM_DELAY);
		y0 = *DNN_R_DATA_0_0;
		y1 = *DNN_R_DATA_0_1;
		y2 = *DNN_R_DATA_0_2;
		y3 = *DNN_R_DATA_0_3;
		
		*(READ_SRAM_ADDR+3*i)   = (y3<<8)  | ((y2>>16) & 0x000000ff); 
		*(READ_SRAM_ADDR+3*i+1) = (y2<<16) | ((y1>>8)  & 0x0000ffff); 
		*(READ_SRAM_ADDR+3*i+2) = (y1<<24) | (y0       & 0x00ffffff); 
		
		short_addr = short_addr+1;
	}
	
	short_addr = 5728;
	for(i=11;i<14;i++)
	{
		*DNN_CTRL_0 = (short_addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		delay(READ_SRAM_DELAY);
		y0 = *DNN_R_DATA_0_0;
		y1 = *DNN_R_DATA_0_1;
		y2 = *DNN_R_DATA_0_2;
		y3 = *DNN_R_DATA_0_3;
		
		*(READ_SRAM_ADDR+3*i)   = (y3<<8)  | ((y2>>16) & 0x000000ff); 
		*(READ_SRAM_ADDR+3*i+1) = (y2<<16) | ((y1>>8)  & 0x0000ffff); 
		*(READ_SRAM_ADDR+3*i+2) = (y1<<24) | (y0       & 0x00ffffff); 
		
		short_addr = short_addr+1;
	}
	
	short_addr = 5632;
	for(i=14;i<18;i++)
	{
		*DNN_CTRL_0 = (short_addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		delay(READ_SRAM_DELAY);
		y0 = *DNN_R_DATA_0_0;
		y1 = *DNN_R_DATA_0_1;
		y2 = *DNN_R_DATA_0_2;
		y3 = *DNN_R_DATA_0_3;
		
		*(READ_SRAM_ADDR+3*i)   = (y3<<8)  | ((y2>>16) & 0x000000ff); 
		*(READ_SRAM_ADDR+3*i+1) = (y2<<16) | ((y1>>8)  & 0x0000ffff); 
		*(READ_SRAM_ADDR+3*i+2) = (y1<<24) | (y0       & 0x00ffffff); 
		
		short_addr = short_addr+1;
	}
	 delay(7000);
	 
	// short_addr = 5696;
	// for(i=10;i<20;i++)
	// {
		// *DNN_CTRL_0 = (short_addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		// delay(READ_SRAM_DELAY);
		// y0 = *DNN_R_DATA_0_0;
		// y1 = *DNN_R_DATA_0_1;
		// y2 = *DNN_R_DATA_0_2;
		// y3 = *DNN_R_DATA_0_3;
		
		// *(READ_SRAM_ADDR+3*i)   = (y3<<8)  | ((y2>>16) & 0x000000ff); 
		// *(READ_SRAM_ADDR+3*i+1) = (y2<<16) | ((y1>>8)  & 0x0000ffff); 
		// *(READ_SRAM_ADDR+3*i+2) = (y1<<24) | (y0       & 0x00ffffff); 
		
		// short_addr = short_addr+1;
	// }
	
      *REG_RUN_CPU = 0;
  		// done
		//////////////////////////////////////////////////////


    return 1;
}
