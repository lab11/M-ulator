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

bool check_array(int32_t a[], int32_t b[], int length) {
	int i; 
	for (i = 0; i < length; i++) {
		if (a[i] != b[i]) return false; 
	}
	return true;
}

int main() {
    //Initialize Interrupts
    //disable_all_irq();

//    if (*REG_CHIP_ID != 0x1234) while(1);

		uint16_t inst_no;
		set_dnn_insts();
		set_nli_parameters();
		*PE_INST &= 0xffff0fff;		// set all INST_SEL to buffer 0


      *DNN_SRAM_RSTN_0 = 0x000007ff;
      *DNN_SRAM_RSTN_1 = 0x000007ff;
      *DNN_SRAM_RSTN_2 = 0x000007ff;
      *DNN_SRAM_RSTN_3 = 0x000007ff;
      delay(3);

      *DNN_SRAM_ISOL_0 = 0x000007ff;
      *DNN_SRAM_ISOL_1 = 0x000007ff;
      *DNN_SRAM_ISOL_2 = 0x000007ff;
      *DNN_SRAM_ISOL_3 = 0x000007ff;
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


      *DNN_SRAM_RSTN_0 = 0x000007ff;
      *DNN_SRAM_RSTN_0 = 0xffffffff;
      *DNN_SRAM_RSTN_1 = 0xffffffff;
      *DNN_SRAM_RSTN_2 = 0xffffffff;
      *DNN_SRAM_RSTN_3 = 0xffffffff;
      delay(3);


  
		//////////////////////////////////////////////////////
    // working sequence
      // a.) wait for MBus transfer input / data to DLC, instruction to M0
      inst_no = 0;
			int32_t a[3] = {1, 2, 3};
			int32_t b[6] = {-1, -2, -3, -4, -5, -6};
			int32_t c[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
			int32_t d[12] = {-1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12};
			// cannot go beyond that: can't compile

			int32_t A[3];
			int32_t B[6];
			int32_t C[9];
			int32_t D[12];

			uint16_t line1 = 32;
			uint16_t line2 = 128;
			uint16_t line3 = 256;
			uint16_t line4 = 1024;
			/* TODO
			if (*M0_START != 1) { 								// check switch to start the working sequence
				fail (0x0, 0x0);		
			}*/

			uint16_t addr;
			// b.) write
			for (addr = 0x0000; addr < 0x8000; addr += 0x2000) {
			  write_dnn_sram(addr + line4 / 2, a, 3 / 3);
			  write_dnn_sram(addr + line4 * 4 + line3 / 2, b, 6 / 3);
			  write_dnn_sram(addr + line4 * 4 + line3 * 4 + line2 / 2, c, 9 / 3);
			  write_dnn_sram(addr + line4 * 4 + line3 * 4 + line2 * 4 + line1 / 2, d, 12 / 3);
			}

      signal_debug();
      
			// c.) read
			bool correct = true;
			for (addr = 0x0000; addr < 0x8000; addr += 0x2000) {
			  read_dnn_sram(addr + line4 / 2, A, 3 / 3);
				correct = check_array(a, A, 3);
				if (!correct) {set_halt_until_mem_wr(); delay(100); set_halt_until_mbus_tx(); correct = true;}  // TODO: report fail
			  read_dnn_sram(addr + line4 * 4 + line3 / 2, B, 6 / 3);
				correct = check_array(b, B, 6);
				if (!correct) {set_halt_until_mem_wr(); delay(100); set_halt_until_mbus_tx();correct = true;}  // TODO: report fail
			  read_dnn_sram(addr + line4 * 4 + line3 * 4 + line2 / 2, C, 9 / 3);
				correct = check_array(c, C, 9);
				if (!correct) {set_halt_until_mem_wr(); delay(100); set_halt_until_mbus_tx();correct = true;}  // TODO: report fail
			  read_dnn_sram(addr + line4 * 4 + line3 * 4 + line2 * 4 + line1 / 2, D, 12 / 3);
				correct = check_array(d, D, 12);
				if (!correct) {set_halt_until_mem_wr(); delay(100); set_halt_until_mbus_tx();correct = true;}  // TODO: report fail
//				if (!correct) clock_gate();   // TODO: report fail
			}
      
			// d.) finish
      signal_done();
      clock_gate();
  		// done
		//////////////////////////////////////////////////////


    return 1;
}
