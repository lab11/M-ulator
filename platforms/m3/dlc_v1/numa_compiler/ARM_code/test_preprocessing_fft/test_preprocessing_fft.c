//*******************************************************************
//Author: Yejoong Kim Ziyun Li Cao Gao
//Description: M0 code for Deep Learning Processor project
//*******************************************************************
#include "DLCv1.h"
//#include "FLSv2S_RF.h"
//#include "PMUv2_RF.h"
#include "DLCv1_RF.h"
#include "mbus.h"
#include "dnn_parameters.h"
#include "utils.h"

#define DLC_ADDR    0x4

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t cyc_num;
volatile uint32_t irq_history;

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
#define STACK_TOP       0x00006000
#define RAW_INPUT_ADDR  0x00006000 
#define INPUT_ADDR      0x00006200 
#define OUTPUT_ADDR     0x00006600 
#define MFCC_INPUT_ADDR     0x00006a00 
#define WINDOW_PARA_ADDR  0x00007000
#define DFT_PARA_ADDR   0x00007400 
#define DCT_PARA_ADDR   0x00007800
#define FFT_PREC 16     // essentially hard-coded: other precision won't work
#define DFT_SIZE 2 
#define FFT_NUM 128     // FFT_SIZE / DFT_SIZE / 2
#define FFT_NUM_BITS 8  // log2(FFT_NUM) + 1
#define FFT_INST 8      // total number of FFT instructions

#define FFT_START 5504    // TODO: integrate that into script
void init_input(int16_t *fft_input, uint16_t windowsize) {
  uint16_t iter;
  for (iter = 0; iter < windowsize; iter++) {
    fft_input[iter] = (iter % 10) * 50;
  }
}

int main() {
		//////////////////////////////////////////////////////
    // init
    int16_t* raw_fft_input = (int16_t*) RAW_INPUT_ADDR;    // size: 512 * 16 / 32 = 200 (0x100)
    int16_t* fft_input = (int16_t*) INPUT_ADDR;            // size: 512 * 16 * 2 / 32 * 1.5 = 768 (0x300)
    int16_t* fft_output = (int16_t*) OUTPUT_ADDR;          // size: 257 * 16 * 2 / 32 * 1.5 = 386 (0x182)
    int16_t* window_parameter = (int16_t*) WINDOW_PARA_ADDR;   // size: 400 * 16 / 32 = 200 (0xC8).
    int16_t* dft_parameter_r = (int16_t*) DFT_PARA_ADDR;       // size: 2 * 2 * 16 / 32 = 2 (0x2). 14 fractional bits 
    int16_t* dft_parameter_i = (int16_t*) DFT_PARA_ADDR + DFT_SIZE * DFT_SIZE * sizeof(int16_t);
    int16_t* mfcc_input = (int16_t*) MFCC_INPUT_ADDR;          // size: 257 * 16 * 2 / 32 = 257 (0x101)
//    int16_t* dct_parameter = DCT_PARA_ADDR;
    init_input(raw_fft_input, WINDOW_SIZE);
    window_init(window_parameter, WINDOW_SIZE);
    dft_init(dft_parameter_r, dft_parameter_i, DFT_SIZE);
//    dct_init(dct_parameter);

		//////////////////////////////////////////////////////
    // PE init
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

    signal_debug(1);    // init ends; start counting runtime
		//////////////////////////////////////////////////////
    // M0 working sequence
    
    // TODO: optimize these arrays
    int16_t input_r_dft[FFT_SIZE];
    int16_t input_i_dft[FFT_SIZE];

    // prepare fft
    fft_apply_window(WINDOW_SIZE, raw_fft_input, window_parameter); 
    prepare_dft_for_fft(WINDOW_SIZE, FFT_SIZE, raw_fft_input, input_r_dft, FFT_NUM, FFT_NUM_BITS, DFT_SIZE);

    // dft
    dft(DFT_SIZE, FFT_SIZE, input_r_dft, input_i_dft, dft_parameter_r, dft_parameter_i);
 
    prepare_PE_fft_input(fft_input, input_r_dft, input_i_dft, DFT_SIZE, FFT_SIZE, FFT_PREC);
    signal_debug(2);    // M0 ends; write to PE

    // write to PE
    uint16_t fft_input_space = FFT_SIZE / 2;   // DLC memory occupied. for 16 bits only
    write_dnn_sram_16(FFT_START, fft_input, fft_input_space);
    signal_debug(0);    // start PE runtime, check if memory correct
		//////////////////////////////////////////////////////
    // PE working sequence
    inst_no = 0;
    reset_PE(0);
    write_instruction(inst_no, 0, 0);   // write FFT 0
   
    while (inst_no < FFT_INST - 1) {
      if (inst_no % 2 == 0) {
        switch_inst_buffer(0, 0);
        reset_PE(0);
			  start_pe_inst(0b0001);                    // start FFT 0, 2, 4, 6
			  inst_no++;
        write_instruction(inst_no, 0, 1);         // write FFT 1, 3, 5, 7
//        clock_gate();
      } else {
        switch_inst_buffer(0, 1);
        reset_PE(0);
			  start_pe_inst(0b0001);              // start FFT 1, 3, 5
			  inst_no++;
        write_instruction(inst_no, 0, 0);   // write FFT 2, 4, 6
//        clock_gate(); 
      }
    }

    switch_inst_buffer(0, 1);
    reset_PE(0);
		start_pe_inst(0b0001);        // start FFT 7
		inst_no++;
//    clock_gate(); 

		// finish
    wait_until_pe_finish(0b1111);
		//////////////////////////////////////////////////////

    uint16_t fft_output_space = MFCC_SIZE / 2;   // DLC memory occupied. for 16 bits only 
    read_dnn_sram_16(FFT_START, fft_output, fft_output_space); 
    extract_fft_output(fft_output, mfcc_input, MFCC_SIZE, FFT_PREC);

    // MFCC
//  run_mfcc();   // TODO: on PE

//    mfcc_to_dnn(test, test1);

    signal_done();
    delay(7000);
    return 1;
 		// done
}
