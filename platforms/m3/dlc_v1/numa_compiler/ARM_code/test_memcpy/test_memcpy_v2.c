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

int main() {
    //Initialize Interrupts
    //disable_all_irq();

//    if (*REG_CHIP_ID != 0x1234) while(1);

		uint16_t inst_no;
		set_dnn_insts();
		set_nli_parameters();

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

    // debug
    int32_t *test = (int32_t*)0x00007000; 
    test[0] = 10;
    test[1] = 11;
    test[2] = 12;
    test[3] = 13;
    *DNN_NLI_A_0 = test[0];
    *DNN_NLI_A_0 = test[1];
    *DNN_NLI_A_0 = test[2];
    *DNN_NLI_A_0 = test[3];
    *DNN_NLI_X_0 = test;
    *DNN_NLI_X_0 = &test[0];
    *DNN_NLI_X_0 = &test[1];
    *DNN_NLI_X_0 = &test[2];
    *DNN_NLI_X_0 = &test[3];
    uint16_t iter;
    for (iter = 0; iter < 4; iter++) {
      test[iter] = iter;
      *DNN_NLI_X_0 = test[iter];
    }

    int16_t *test_16 = (int16_t*)0x00007100; 
    test_16[0] = 20;
    test_16[1] = 21;
    test_16[2] = 22;
    test_16[3] = 23;
    *DNN_NLI_A_0 = test_16[0];
    *DNN_NLI_A_0 = test_16[1];
    *DNN_NLI_A_0 = test_16[2];
    *DNN_NLI_A_0 = test_16[3];
    *DNN_NLI_X_0 = test_16;
    *DNN_NLI_X_0 = &test_16[0];
    *DNN_NLI_X_0 = &test_16[1];
    *DNN_NLI_X_0 = &test_16[2];
    *DNN_NLI_X_0 = &test_16[3];

    int8_t *test_8 = (int8_t*)0x00007200; 
    test_8[0] = 30;
    test_8[1] = 31;
    test_8[2] = 32;
    test_8[3] = 33;
    *DNN_NLI_A_0 = test_8[0];
    *DNN_NLI_A_0 = test_8[1];
    *DNN_NLI_A_0 = test_8[2];
    *DNN_NLI_A_0 = test_8[3];
    *DNN_NLI_X_0 = test_8;
    *DNN_NLI_X_0 = &test_8[0];
    *DNN_NLI_X_0 = &test_8[1];
    *DNN_NLI_X_0 = &test_8[2];
    *DNN_NLI_X_0 = &test_8[3];

    signal_done();
    delay(7000);
    ////////// 

		//////////////////////////////////////////////////////

    return 1;
}
