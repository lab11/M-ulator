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
#define OUT_ADDR 5504
#define READ_SRAM_DELAY 6
int main() {
    //Initialize Interrupts
    //disable_all_irq();

//    if (*REG_CHIP_ID != 0x1234) while(1);

		uint16_t inst_no, pe_no;
		set_dnn_insts();

    *REG_WAIT_BEFORE_VDD = 0xff;
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
//      *DNN_RAND_0 = 1;
//      *DNN_RAND_1 = 1;
//      *DNN_RAND_2 = 1;
//      *DNN_RAND_3 = 1;
//      delay(3);
      
      //*DNN_SRAM_ISOL_0 = 0x00000000;
      //*DNN_SRAM_ISOL_1 = 0x00000000;
      //*DNN_SRAM_ISOL_2 = 0x00000000;
      //*DNN_SRAM_ISOL_3 = 0x00000000;
      //delay(3);

		//////////////////////////////////////////////////////
    // working sequence
      volatile int32_t input[408];
      volatile int32_t output[13]; 
      uint16_t iter;
      for (iter = 0; iter < 408; iter++) {
        input[iter] = (iter - 204) << 3;    // check compiler/dnn_gen_fake_mac.py
      }
      inst_no = 0;
      ///////////////////
      // a.) transfer input / data to DLC
//      write_dnn_sram_12_dnn_input(IN_ADDR, input, 408 / 8); 
			*DNN_PG_0 = 0x000007ff;		// only last 11 bits are set
      uint32_t short_addr = IN_ADDR & 0x1fff;
      uint32_t addr = short_addr;
      uint32_t arr_addr = 0;
	    for (; addr < 51 + short_addr; addr++) {
	      *DNN_DATA_0_0 = (input[arr_addr] & 0xfff) | ((input[arr_addr+1] & 0xfff) << 12);
        arr_addr += 2;
	      *DNN_DATA_0_1 = (input[arr_addr] & 0xfff) | ((input[arr_addr+1] & 0xfff) << 12);
        arr_addr += 2;
	      *DNN_DATA_0_2 = (input[arr_addr] & 0xfff) | ((input[arr_addr+1] & 0xfff) << 12);
        arr_addr += 2;
	      *DNN_DATA_0_3 = (input[arr_addr] & 0xfff) | ((input[arr_addr+1] & 0xfff) << 12);
        arr_addr += 2;
	  	  *DNN_CTRL_0 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
      }
      signal_debug(0);

      ///////////////////
      // b.) broadcast inputs 
      // 
      uint32_t temp_DNN_CTRL;
      uint32_t temp_word0, temp_word1;
      set_all_buffer1();
      ///
//      write_instruction(0, 0, 1);   // write MOV0
//      write_instruction(1, 1, 1);   // write MOV1
//      write_instruction(2, 2, 1);   // write MOV2
      temp_DNN_CTRL = (1 << 15);			// start load: PE_INST_WR = 1
      temp_DNN_CTRL |= (6 << 16);			// buffer 1
      for (addr = 0; addr < 6; addr++) {
      	if (addr != 0) {
      		temp_DNN_CTRL += (1 << 16);			// PE_INST_ADDR += 1 
      	}
      
        temp_word0 = PE_INSTS[0][0][addr] & 0xffffff;
        temp_word1 = PE_INSTS[0][0][addr] >> 24;
        *DNN_INST_0_0 = temp_word0;
        *DNN_INST_0_1 = temp_word1; 
        *DNN_CTRL_0 = temp_DNN_CTRL;
        temp_word0 = PE_INSTS[1][1][addr] & 0xffffff;
        temp_word1 = PE_INSTS[1][1][addr] >> 24;
        *DNN_INST_1_0 = temp_word0; 
        *DNN_INST_1_1 = temp_word1; 
        *DNN_CTRL_1 = temp_DNN_CTRL;
        temp_word0 = PE_INSTS[2][2][addr] & 0xffffff;
        temp_word1 = PE_INSTS[2][2][addr] >> 24;
        *DNN_INST_2_0 = temp_word0;
        *DNN_INST_2_1 = temp_word1; 
        *DNN_CTRL_2 = temp_DNN_CTRL;
      }
      ///
			start_pe_inst(0b0001);              // start MOV0
			inst_no++;
			while (check_if_pe_finish(0b0001) != 1) { delay(5);}
			start_pe_inst(0b0010);              // start MOV1
			inst_no++;
			while (check_if_pe_finish(0b0010) != 1) { delay(5);}
			start_pe_inst(0b0100);              // start MOV2

      //////////////
			// c.) layer 0 MAC
      ///
//    for (pe_no = 0; pe_no < 4; pe_no++) {
//      write_instruction(3, pe_no, 0);   // write MACNLI0
//    }
      write_instruction_4PE(3, 0);   // write MACNLI0
      ///
			while (check_if_pe_finish(0b0100) != 1) { delay(5);}
      set_all_buffer0();
			start_pe_inst(0b1111);                    // start MACNLI0
			inst_no++;
      for (pe_no = 0; pe_no < 3; pe_no++) {
        write_instruction_24word(PE_INSTS[4][pe_no][1], pe_no, 1, 1);   // write MOV0
        write_instruction_24word(PE_INSTS[4][pe_no][2], pe_no, 2, 1);   // write MOV0
      }
      write_instruction(4, 3, 1);   // write MOV0
      clock_gate();
      signal_debug(1);              // layer 0 MAC done

      ////////////////
			// d.) layer 0 MOV
      set_all_buffer1();
			start_pe_inst(0b1111);        // start MOV0
			inst_no++;
      ///
//      for (pe_no = 0; pe_no < 4; pe_no++) {
//        write_instruction(7, pe_no, 0);   // write MACNLI1
//      }
      write_instruction_4PE(7, 0);   // write MACNLI1
//      clock_gate();                 
// don't clock_gate: mov is too short. if clock_gate after PEs have already all finished, then ARM won't be woken up
			while (check_if_pe_finish(0b1111) != 1) { delay(5);}
      ///
//      for (pe_no = 0; pe_no < 4; pe_no++) {
//        write_instruction_24word(PE_INSTS[5][pe_no][1], pe_no, 1, 1);   // write MOV1
//        write_instruction_24word(PE_INSTS[5][pe_no][2], pe_no, 2, 1);   // write MOV1
//      }
      temp_DNN_CTRL = (1 << 15);			// start load: PE_INST_WR = 1
      temp_DNN_CTRL |= (6 << 16);			// buffer 1
      temp_DNN_CTRL += (1 << 16);			// start from addr 1
      temp_word0 = PE_INSTS[5][0][1] & 0xffffff;
      temp_word1 = PE_INSTS[5][0][1] >> 24;
      *DNN_INST_0_0 = temp_word0;
      *DNN_INST_0_1 = temp_word1; 
      *DNN_CTRL_0 = temp_DNN_CTRL;
      temp_word0 = PE_INSTS[5][1][1] & 0xffffff;
      temp_word1 = PE_INSTS[5][1][1] >> 24;
      *DNN_INST_1_0 = temp_word0; 
      *DNN_INST_1_1 = temp_word1; 
      *DNN_CTRL_1 = temp_DNN_CTRL;
      temp_word0 = PE_INSTS[5][2][1] & 0xffffff;
      temp_word1 = PE_INSTS[5][2][1] >> 24;
      *DNN_INST_2_0 = temp_word0;
      *DNN_INST_2_1 = temp_word1; 
      *DNN_CTRL_2 = temp_DNN_CTRL;
      temp_word0 = PE_INSTS[5][3][1] & 0xffffff;
      temp_word1 = PE_INSTS[5][3][1] >> 24;
      *DNN_INST_3_0 = temp_word0; 
      *DNN_INST_3_1 = temp_word1;
      *DNN_CTRL_3 = temp_DNN_CTRL;
      temp_DNN_CTRL += (1 << 16);			// PE_INST_ADDR += 1 
      temp_word0 = PE_INSTS[5][0][2] & 0xffffff;
      temp_word1 = PE_INSTS[5][0][2] >> 24;
      *DNN_INST_0_0 = temp_word0;
      *DNN_INST_0_1 = temp_word1; 
      *DNN_CTRL_0 = temp_DNN_CTRL;
      temp_word0 = PE_INSTS[5][1][2] & 0xffffff;
      temp_word1 = PE_INSTS[5][1][2] >> 24;
      *DNN_INST_1_0 = temp_word0; 
      *DNN_INST_1_1 = temp_word1; 
      *DNN_CTRL_1 = temp_DNN_CTRL;
      temp_word0 = PE_INSTS[5][2][2] & 0xffffff;
      temp_word1 = PE_INSTS[5][2][2] >> 24;
      *DNN_INST_2_0 = temp_word0;
      *DNN_INST_2_1 = temp_word1; 
      *DNN_CTRL_2 = temp_DNN_CTRL;
      temp_word0 = PE_INSTS[5][3][2] & 0xffffff;
      temp_word1 = PE_INSTS[5][3][2] >> 24;
      *DNN_INST_3_0 = temp_word0; 
      *DNN_INST_3_1 = temp_word1;
      *DNN_CTRL_3 = temp_DNN_CTRL;
      ///
      
			start_pe_inst(0b1111);        // start MOV1
			inst_no++;
			while (check_if_pe_finish(0b1111) != 1) { delay(5);}
      ///
//      for (pe_no = 0; pe_no < 4; pe_no++) {
//        write_instruction_24word(PE_INSTS[6][pe_no][1], pe_no, 1, 1);   // write MOV2
//        write_instruction_24word(PE_INSTS[6][pe_no][2], pe_no, 2, 1);   // write MOV2
//      }
      temp_DNN_CTRL = (1 << 15);			// start load: PE_INST_WR = 1
      temp_DNN_CTRL |= (6 << 16);			// buffer 1
      temp_DNN_CTRL += (1 << 16);			// start from addr 1
      temp_word0 = PE_INSTS[6][0][1] & 0xffffff;
      temp_word1 = PE_INSTS[6][0][1] >> 24;
      *DNN_INST_0_0 = temp_word0;
      *DNN_INST_0_1 = temp_word1; 
      *DNN_CTRL_0 = temp_DNN_CTRL;
      temp_word0 = PE_INSTS[6][1][1] & 0xffffff;
      temp_word1 = PE_INSTS[6][1][1] >> 24;
      *DNN_INST_1_0 = temp_word0; 
      *DNN_INST_1_1 = temp_word1; 
      *DNN_CTRL_1 = temp_DNN_CTRL;
      temp_word0 = PE_INSTS[6][2][1] & 0xffffff;
      temp_word1 = PE_INSTS[6][2][1] >> 24;
      *DNN_INST_2_0 = temp_word0;
      *DNN_INST_2_1 = temp_word1; 
      *DNN_CTRL_2 = temp_DNN_CTRL;
      temp_word0 = PE_INSTS[6][3][1] & 0xffffff;
      temp_word1 = PE_INSTS[6][3][1] >> 24;
      *DNN_INST_3_0 = temp_word0; 
      *DNN_INST_3_1 = temp_word1;
      *DNN_CTRL_3 = temp_DNN_CTRL;
      temp_DNN_CTRL += (1 << 16);			// PE_INST_ADDR += 1 
      temp_word0 = PE_INSTS[6][0][2] & 0xffffff;
      temp_word1 = PE_INSTS[6][0][2] >> 24;
      *DNN_INST_0_0 = temp_word0;
      *DNN_INST_0_1 = temp_word1; 
      *DNN_CTRL_0 = temp_DNN_CTRL;
      temp_word0 = PE_INSTS[6][1][2] & 0xffffff;
      temp_word1 = PE_INSTS[6][1][2] >> 24;
      *DNN_INST_1_0 = temp_word0; 
      *DNN_INST_1_1 = temp_word1; 
      *DNN_CTRL_1 = temp_DNN_CTRL;
      temp_word0 = PE_INSTS[6][2][2] & 0xffffff;
      temp_word1 = PE_INSTS[6][2][2] >> 24;
      *DNN_INST_2_0 = temp_word0;
      *DNN_INST_2_1 = temp_word1; 
      *DNN_CTRL_2 = temp_DNN_CTRL;
      temp_word0 = PE_INSTS[6][3][2] & 0xffffff;
      temp_word1 = PE_INSTS[6][3][2] >> 24;
      *DNN_INST_3_0 = temp_word0; 
      *DNN_INST_3_1 = temp_word1;
      *DNN_CTRL_3 = temp_DNN_CTRL;
      ///
//      clock_gate();                 
// don't clock_gate: mov is too short. if clock_gate after PEs have already all finished, then ARM won't be woken up

			start_pe_inst(0b1111);        // start MOV2
			inst_no++;

      ////////////////
			// e.) layer 1
			while (check_if_pe_finish(0b1111) != 1) { delay(5);}
      signal_debug(2);    // layer 0 done

      for (pe_no = 0; pe_no < 4; pe_no++) {
        reset_PE(pe_no);
      }
      set_all_buffer0();
			start_pe_inst(0b1111);                    // start MACNLI1
			inst_no++;
      write_instruction_24word(PE_INSTS[8][1][1], 1, 1, 1);   // write MOV0
      write_instruction_24word(PE_INSTS[8][1][2], 1, 2, 1);   // write MOV0
      write_instruction_24word(PE_INSTS[9][2][1], 2, 1, 1);   // write MOV1
      write_instruction_24word(PE_INSTS[9][2][2], 2, 2, 1);   // write MOV1
      write_instruction_24word(PE_INSTS[10][3][1], 3, 1, 1);   // write MOV2
      write_instruction_24word(PE_INSTS[10][3][2], 3, 2, 1);   // write MOV2
      clock_gate();
//			while (~check_if_pe_finish(0b1111)) { delay(5);}// TODO: may not need this 
      signal_debug(3);
      // layer 1 MAC done

      set_all_buffer1();
			start_pe_inst(0b0010);        // start MOV0
			inst_no++;
      write_instruction(11, 0, 0);   // write MACNLI2
//      clock_gate();                 
// don't clock_gate: mov is too short. if clock_gate after PEs have already all finished, then ARM won't be woken up
			while (check_if_pe_finish(0b0010) != 1) { delay(5);}
      
			start_pe_inst(0b0100);        // start MOV1
			inst_no++;
//      clock_gate();                 
// don't clock_gate: mov is too short. if clock_gate after PEs have already all finished, then ARM won't be woken up
			while (check_if_pe_finish(0b0100) != 1) { delay(5);}

			start_pe_inst(0b1000);        // start MOV2
			inst_no++;

      ////////////////
			// f.) layer 2 (PE0 only)
			while (check_if_pe_finish(0b1000) != 1) { delay(5);}
      // layer 1 done (only PE0 has the full result)

      reset_PE(0);
      switch_inst_buffer(0, 0);
			start_pe_inst(0b0001);                    // start MACNLI2
			inst_no++;
      clock_gate();

//      signal_debug(2);    // layer 2 done
//      signal_debug(3);    

			// g.) finish
			if (check_if_pe_finish(0b0001) != 1) { fail (0x0, 0x0); }
      ///
//      read_dnn_sram_12_output(OUT_ADDR, output, 2);     // 13 elements of 12 bits: 2 96bit words
	    volatile uint32_t y0, y1, y2, y3;
      short_addr = OUT_ADDR & 0x1fff;
			*DNN_PG_0 = 0x000007ff;		// only last 11 bits are set
	  	*DNN_CTRL_0 = (short_addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
      delay(READ_SRAM_DELAY);
	  	y0 = *DNN_R_DATA_0_0; 
	  	y1 = *DNN_R_DATA_0_1; 
	  	y2 = *DNN_R_DATA_0_2; 
	  	y3 = *DNN_R_DATA_0_3; 
	   	*DNN_CTRL_0 = ((short_addr + 1) << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	    output[0] = (y0 & 0xfff);
	    output[1] = (y0 >> 12) & 0xfff; 
	    output[2] = (y1 & 0xfff);
	    output[3] = (y1 >> 12) & 0xfff; 
	    output[4] = (y2 & 0xfff);
	    output[5] = (y2 >> 12) & 0xfff; 
	    output[6] = (y3 & 0xfff);
	    output[7] = (y3 >> 12) & 0xfff; 
	  	y0 = *DNN_R_DATA_0_0; 
	  	y1 = *DNN_R_DATA_0_1; 
	  	y2 = *DNN_R_DATA_0_2; 
//	  	y3 = *DNN_R_DATA_0_3; 
	    output[8] = (y0 & 0xfff);
	    output[9] = (y0 >> 12) & 0xfff; 
	    output[10] = (y1 & 0xfff);
	    output[11] = (y1 >> 12) & 0xfff; 
	    output[12] = (y2 & 0xfff);
//	    output[13] = (y2 >> 12) & 0xfff; 
      //
      // output (has to be 32bit to be correct for this code
      for (iter = 0; iter < 13; iter++) {
        *DNN_DATA_1_0 = output[iter];
      }
      signal_done();
      delay(7000);
  		// done
		//////////////////////////////////////////////////////


    return 1;
}
