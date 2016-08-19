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

#define OUT_ADDR 5504
#define TEMP_OUT_ADDR 5696
#define TEMP_OUT_WORD 32  // conservative: actually 32

int main() {
    //Initialize Interrupts
    //disable_all_irq();

//    if (*REG_CHIP_ID != 0x1234) while(1);

		uint16_t inst_no, pe_no, nli_no;
    uint32_t result[6];
		set_dnn_insts();
		set_nli_parameters();
		*PE_INST &= 0xffff0fff;			// set all INST_SEL to buffer 0

    *REG_WAIT_BEFORE_VDD = 0xff;

		//////////////////////////////////////////////////////
    // working sequence
      // a.) wait for MBus transfer input / data to DLC, instruction to M0
      inst_no = 0;
      nli_no = 0;
			/* TODO
			if (*M0_START != 1) { 								// check switch to start the working sequence
				fail (0x0, 0x0);		
			}*/

			// b.) layer 0
      for (pe_no = 0; pe_no < 4; pe_no++) {
        write_instruction(inst_no, pe_no, 0);   // write MACNLI
  			write_nli(nli_no, pe_no);
      }
      set_all_buffer0();
			inst_no++;
			nli_no++;
			start_pe_inst(0b1111);                    // start MACNLI
      clock_gate();

      for (pe_no = 0; pe_no < 4; pe_no++) {
        write_instruction(inst_no, pe_no, 1);   // write MOV0
      }
      set_all_buffer1();
			inst_no++;
			start_pe_inst(0b1111);        // start MOV0
      clock_gate();

      for (pe_no = 0; pe_no < 4; pe_no++) {
        write_instruction(inst_no, pe_no, 0);   // write MOV1
      }
      set_all_buffer0();
			inst_no++;
			start_pe_inst(0b1111);        // start MOV1
      clock_gate();

      for (pe_no = 0; pe_no < 4; pe_no++) {
        write_instruction(inst_no, pe_no, 1);   // write MOV2
      }
      set_all_buffer1();
			inst_no++;
			start_pe_inst(0b1111);        // start MOV2
      clock_gate();


			// c.) layer 1
    *REG_WAIT_BEFORE_VDD = 0xff;
      // debug 510
        *DNN_RE_INIT_0 |= (1 << 1);
        *DNN_RE_INIT_0 &= ~(1 << 1);
        *DNN_RE_INIT_1 |= (1 << 1);
        *DNN_RE_INIT_1 &= ~(1 << 1);
        *DNN_RE_INIT_2 |= (1 << 1);
        *DNN_RE_INIT_2 &= ~(1 << 1);
        *DNN_RE_INIT_3 |= (1 << 1);
        *DNN_RE_INIT_3 &= ~(1 << 1);
      // debug 510
//      for (pe_no = 0; pe_no < 4; pe_no++) {
//        clear_memory_region(TEMP_OUT_ADDR + 0x2000 * pe_no, TEMP_OUT_WORD);
//      }
      for (pe_no = 0; pe_no < 4; pe_no++) {
        write_instruction(inst_no, pe_no, 0);   // write MACNLI
//  			write_nli(nli_no, pe_no);
      }
      set_all_buffer0();
			inst_no++;
			nli_no++;
			start_pe_inst(0b1111);                    // start MACNLI
      clock_gate();

      signal_debug(0);    // layer 1 MACNLI done
      signal_debug(1);    // layer 1 MACNLI done
      signal_debug(2);    // layer 1 MACNLI done
      signal_debug(3);    // layer 1 MACNLI done

      for (pe_no = 0; pe_no < 4; pe_no++) {
        write_instruction(inst_no, pe_no, 1);   // write MOV0
      }
      set_all_buffer1();
			inst_no++;
			start_pe_inst(0b1111);        // start MOV0
      clock_gate();


      for (pe_no = 0; pe_no < 4; pe_no++) {
        write_instruction(inst_no, pe_no, 0);   // write MOV1
      }
      set_all_buffer0();
			inst_no++;
			start_pe_inst(0b1111);        // start MOV1
      clock_gate();

      for (pe_no = 0; pe_no < 4; pe_no++) {
        write_instruction(inst_no, pe_no, 1);   // write MOV2
      }
      set_all_buffer1();
			start_pe_inst(0b1111);        // start MOV2
			inst_no++;
      clock_gate();
//      signal_debug(1);    // layer 1 done

			// d.) layer 2 (PE0 only)
//      clear_memory_region(TEMP_OUT_ADDR, TEMP_OUT_WORD);
//      write_instruction(inst_no, 0, 0);   // write MACNLI
////  		write_nli(nli_no, 0);
//      switch_inst_buffer(0, 0);
//			inst_no++;
//			nli_no++;
//			start_pe_inst(0b0001);                    // start MACNLI
//      clock_gate();

//      signal_debug(2);    // layer 2 done

			// e.) finish
      read_dnn_sram(OUT_ADDR, result, 2);     // 13 elements of 12 bits: 2 96bit words
      signal_done();
  		// done
		//////////////////////////////////////////////////////


    return 1;
}
