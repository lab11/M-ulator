//*******************************************************************
//Author: Ziyun Li 
//Description: DLCv1 header file
//*******************************************************************

#ifndef DLCV1_H
#define DLCV1_H

//*********************************************************
// Boolean Constants
//*********************************************************
#define TRUE  1
#define FALSE 0

//*********************************************************
// ARMv6 Architecture NVIC Registers
//*********************************************************
#define NVIC_ISER       ((volatile uint32_t *) 0xE000E100)  // Interrupt Set-Enable Register
#define NVIC_ICER       ((volatile uint32_t *) 0xE000E180)  // Interrupt Clear-Enable Register
#define NVIC_ISPR       ((volatile uint32_t *) 0xE000E200)  // Interrupt Set-Pending Register
#define NVIC_ICPR       ((volatile uint32_t *) 0xE000E280)  // Interrupt Clear-Pending Register
#define NVIC_IPR0       ((volatile uint32_t *) 0xE000E400)  // Interrupt Priority Register
#define NVIC_IPR1       ((volatile uint32_t *) 0xE000E404)  // Interrupt Priority Register
#define NVIC_IPR2       ((volatile uint32_t *) 0xE000E408)  // Interrupt Priority Register
#define NVIC_IPR3       ((volatile uint32_t *) 0xE000E40C)  // Interrupt Priority Register
#define NVIC_IPR4       ((volatile uint32_t *) 0xE000E410)  // Interrupt Priority Register
#define NVIC_IPR5       ((volatile uint32_t *) 0xE000E414)  // Interrupt Priority Register
#define NVIC_IPR6       ((volatile uint32_t *) 0xE000E418)  // Interrupt Priority Register
#define NVIC_IPR7       ((volatile uint32_t *) 0xE000E41C)  // Interrupt Priority Register

//*********************************************************
// Register File MMIO Addresses
//*********************************************************
#define REG0            ((volatile uint32_t *) 0xA0000000)
#define REG1            ((volatile uint32_t *) 0xA0000004)
#define REG2            ((volatile uint32_t *) 0xA0000008)
#define REG3            ((volatile uint32_t *) 0xA000000C)
#define REG4            ((volatile uint32_t *) 0xA0000010)
#define REG5            ((volatile uint32_t *) 0xA0000014)
#define REG6            ((volatile uint32_t *) 0xA0000018)
#define REG7            ((volatile uint32_t *) 0xA000001C)
#define REG_MBUS_THRES  ((volatile uint32_t *) 0xA0000024)
#define REG_IRQ_CTRL    ((volatile uint32_t *) 0xA0000028)
#define REG_CLKGEN_TUNE ((volatile uint32_t *) 0xA000002C)
#define REG_RUN_CPU     ((volatile uint32_t *) 0xA0000040)

#define REG_STR3_BUFL   ((volatile uint32_t *) 0xA000038C)
#define REG_STR3_BUFU   ((volatile uint32_t *) 0xA0000390)
#define REG_STR3_EN     ((volatile uint32_t *) 0xA0000394)
#define REG_STR3_OFF    ((volatile uint32_t *) 0xA0000398)
#define REG_STR2_BUFL   ((volatile uint32_t *) 0xA000039C)
#define REG_STR2_BUFU   ((volatile uint32_t *) 0xA00003A0)
#define REG_STR2_EN     ((volatile uint32_t *) 0xA00003A4)
#define REG_STR2_OFF    ((volatile uint32_t *) 0xA00003A8)
#define REG_STR1_BUFL   ((volatile uint32_t *) 0xA00003AC)
#define REG_STR1_BUFU   ((volatile uint32_t *) 0xA00003B0)
#define REG_STR1_EN     ((volatile uint32_t *) 0xA00003B4)
#define REG_STR1_OFF    ((volatile uint32_t *) 0xA00003B8)
#define REG_STR0_BUFL   ((volatile uint32_t *) 0xA00003BC)
#define REG_STR0_BUFU   ((volatile uint32_t *) 0xA00003C0)
#define REG_STR0_EN     ((volatile uint32_t *) 0xA00003C4)
#define REG_STR0_OFF    ((volatile uint32_t *) 0xA00003C8)
#define REG_BLK_WR      ((volatile uint32_t *) 0xA00003CC)

//*********************************************************
// DNN MMIO Addresses
//*********************************************************

#define REG_DLC_RESETN      ((volatile uint32_t *) 0xA00000FC)
#define REG_SRAM_L1         ((volatile uint32_t *) 0xA0000100)
#define REG_SRAM_L2         ((volatile uint32_t *) 0xA0000104)
#define REG_SRAM_L3         ((volatile uint32_t *) 0xA0000108)
#define REG_SRAM_L4         ((volatile uint32_t *) 0xA000010C)
#define REG_CG              ((volatile uint32_t *) 0xA0000110)
#define REG_WAIT_BEFORE_VDD ((volatile uint32_t *) 0xA0000114)
#define REG_STAB_ENABLE_VDD ((volatile uint32_t *) 0xA0000118)
#define REG_VREF_GEN_0      ((volatile uint32_t *) 0xA000011C)
#define REG_VREF_GEN_1      ((volatile uint32_t *) 0xA0000120)
#define REG_VREF_GEN_2      ((volatile uint32_t *) 0xA0000124)
#define REG_VREF_GEN_3      ((volatile uint32_t *) 0xA0000128)
#define REG_MUX_PE0         ((volatile uint32_t *) 0xA000012C)
#define REG_MUX_PE1         ((volatile uint32_t *) 0xA0000130)
#define REG_MUX_PE2         ((volatile uint32_t *) 0xA0000134)
#define REG_MUX_PE3         ((volatile uint32_t *) 0xA0000138)

#define PE_INST         ((volatile uint32_t *) 0xA000013C)
#define PE_INST_DONE    ((volatile uint32_t *) 0xA0000140)

#define DNN_DATA_0_3    ((volatile uint32_t *) 0xA0000144)
#define DNN_DATA_0_2    ((volatile uint32_t *) 0xA0000148)
#define DNN_DATA_0_1    ((volatile uint32_t *) 0xA000014C)
#define DNN_DATA_0_0    ((volatile uint32_t *) 0xA0000150)
#define DNN_CTRL_0      ((volatile uint32_t *) 0xA0000154)
#define DNN_INST_0_1    ((volatile uint32_t *) 0xA0000158)
#define DNN_INST_0_0    ((volatile uint32_t *) 0xA000015C)
#define DNN_NLI_A_0     ((volatile uint32_t *) 0xA0000160)
#define DNN_NLI_X_0     ((volatile uint32_t *) 0xA0000164)
#define DNN_NLI_B_0_1   ((volatile uint32_t *) 0xA0000168)
#define DNN_NLI_B_0_0   ((volatile uint32_t *) 0xA000016C)
#define DNN_NLI_CTRL_0  ((volatile uint32_t *) 0xA0000170)
#define DNN_PG_0        ((volatile uint32_t *) 0xA0000174)
#define DNN_RE_INIT_0   ((volatile uint32_t *) 0xA0000178)
#define DNN_R_DATA_0_3  ((volatile uint32_t *) 0xA0000244)
#define DNN_R_DATA_0_2  ((volatile uint32_t *) 0xA0000248)
#define DNN_R_DATA_0_1  ((volatile uint32_t *) 0xA000024C)
#define DNN_R_DATA_0_0  ((volatile uint32_t *) 0xA0000250)
#define DNN_RAND_0      ((volatile uint32_t *) 0xA0000254)
#define DNN_SRAM_RSTN_0 ((volatile uint32_t *) 0xA0000258)
#define DNN_SRAM_ISOL_0 ((volatile uint32_t *) 0xA000025C)

#define DNN_DATA_1_3    ((volatile uint32_t *) 0xA0000184)
#define DNN_DATA_1_2    ((volatile uint32_t *) 0xA0000188)
#define DNN_DATA_1_1    ((volatile uint32_t *) 0xA000018C)
#define DNN_DATA_1_0    ((volatile uint32_t *) 0xA0000190)
#define DNN_CTRL_1      ((volatile uint32_t *) 0xA0000194)
#define DNN_INST_1_1    ((volatile uint32_t *) 0xA0000198)
#define DNN_INST_1_0    ((volatile uint32_t *) 0xA000019C)
#define DNN_NLI_A_1     ((volatile uint32_t *) 0xA00001A0)
#define DNN_NLI_X_1     ((volatile uint32_t *) 0xA00001A4)
#define DNN_NLI_B_1_1   ((volatile uint32_t *) 0xA00001A8)
#define DNN_NLI_B_1_0   ((volatile uint32_t *) 0xA00001AC)
#define DNN_NLI_CTRL_1  ((volatile uint32_t *) 0xA00001B0)
#define DNN_PG_1        ((volatile uint32_t *) 0xA00001B4)
#define DNN_RE_INIT_1   ((volatile uint32_t *) 0xA00001B8)
#define DNN_R_DATA_1_3  ((volatile uint32_t *) 0xA0000284)
#define DNN_R_DATA_1_2  ((volatile uint32_t *) 0xA0000288)
#define DNN_R_DATA_1_1  ((volatile uint32_t *) 0xA000028C)
#define DNN_R_DATA_1_0  ((volatile uint32_t *) 0xA0000290)
#define DNN_RAND_1      ((volatile uint32_t *) 0xA0000294)
#define DNN_SRAM_RSTN_1 ((volatile uint32_t *) 0xA0000298)
#define DNN_SRAM_ISOL_1 ((volatile uint32_t *) 0xA000029C)

#define DNN_DATA_2_3    ((volatile uint32_t *) 0xA00001C4)
#define DNN_DATA_2_2    ((volatile uint32_t *) 0xA00001C8)
#define DNN_DATA_2_1    ((volatile uint32_t *) 0xA00001CC)
#define DNN_DATA_2_0    ((volatile uint32_t *) 0xA00001D0)
#define DNN_CTRL_2      ((volatile uint32_t *) 0xA00001D4)
#define DNN_INST_2_1    ((volatile uint32_t *) 0xA00001D8)
#define DNN_INST_2_0    ((volatile uint32_t *) 0xA00001DC)
#define DNN_NLI_A_2     ((volatile uint32_t *) 0xA00001E0)
#define DNN_NLI_X_2     ((volatile uint32_t *) 0xA00001E4)
#define DNN_NLI_B_2_1   ((volatile uint32_t *) 0xA00001E8)
#define DNN_NLI_B_2_0   ((volatile uint32_t *) 0xA00001EC)
#define DNN_NLI_CTRL_2  ((volatile uint32_t *) 0xA00001F0)
#define DNN_PG_2        ((volatile uint32_t *) 0xA00001F4)
#define DNN_RE_INIT_2   ((volatile uint32_t *) 0xA00001F8)
#define DNN_R_DATA_2_3  ((volatile uint32_t *) 0xA00002C4)
#define DNN_R_DATA_2_2  ((volatile uint32_t *) 0xA00002C8)
#define DNN_R_DATA_2_1  ((volatile uint32_t *) 0xA00002CC)
#define DNN_R_DATA_2_0  ((volatile uint32_t *) 0xA00002D0)
#define DNN_RAND_2      ((volatile uint32_t *) 0xA00002D4)
#define DNN_SRAM_RSTN_2 ((volatile uint32_t *) 0xA00002D8)
#define DNN_SRAM_ISOL_2 ((volatile uint32_t *) 0xA00002DC)

#define DNN_DATA_3_3    ((volatile uint32_t *) 0xA0000204)
#define DNN_DATA_3_2    ((volatile uint32_t *) 0xA0000208)
#define DNN_DATA_3_1    ((volatile uint32_t *) 0xA000020C)
#define DNN_DATA_3_0    ((volatile uint32_t *) 0xA0000210)
#define DNN_CTRL_3      ((volatile uint32_t *) 0xA0000214)
#define DNN_INST_3_1    ((volatile uint32_t *) 0xA0000218)
#define DNN_INST_3_0    ((volatile uint32_t *) 0xA000021C)
#define DNN_NLI_A_3     ((volatile uint32_t *) 0xA0000220)
#define DNN_NLI_X_3     ((volatile uint32_t *) 0xA0000224)
#define DNN_NLI_B_3_1   ((volatile uint32_t *) 0xA0000228)
#define DNN_NLI_B_3_0   ((volatile uint32_t *) 0xA000022C)
#define DNN_NLI_CTRL_3  ((volatile uint32_t *) 0xA0000230)
#define DNN_PG_3        ((volatile uint32_t *) 0xA0000234)
#define DNN_RE_INIT_3   ((volatile uint32_t *) 0xA0000238)
#define DNN_R_DATA_3_3  ((volatile uint32_t *) 0xA0000304)
#define DNN_R_DATA_3_2  ((volatile uint32_t *) 0xA0000308)
#define DNN_R_DATA_3_1  ((volatile uint32_t *) 0xA000030C)
#define DNN_R_DATA_3_0  ((volatile uint32_t *) 0xA0000310)
#define DNN_RAND_3      ((volatile uint32_t *) 0xA0000314)
#define DNN_SRAM_RSTN_3 ((volatile uint32_t *) 0xA0000318)
#define DNN_SRAM_ISOL_3 ((volatile uint32_t *) 0xA000031C)


#define DNN_DONE        ((volatile uint32_t *) 0xA0000320)

//*********************************************************
// PRC/PREv12 System Tasks
//*********************************************************
#define SYS_CTRL_REG_ADDR     ((volatile uint32_t *) 0xAFFFF000)
#define SYS_CTRL_CMD_HALT_CPU 0xCAFEF00D

//*********************************************************
// CONFIG_HALT_CPU Values
//*********************************************************
#define HALT_UNTIL_REG0     0x0
#define HALT_UNTIL_REG1     0x1
#define HALT_UNTIL_REG2     0x2
#define HALT_UNTIL_REG3     0x3
#define HALT_UNTIL_REG4     0x4
#define HALT_UNTIL_REG5     0x5
#define HALT_UNTIL_REG6     0x6
#define HALT_UNTIL_REG7     0x7
#define HALT_UNTIL_MEM_WR   0x8
#define HALT_UNTIL_MBUS_RX  0x9
#define HALT_UNTIL_MBUS_TX  0xA
#define HALT_UNTIL_MBUS_FWD 0xB
#define HALT_DISABLE        0xF

//*********************************************************
// DNN IRQ setup 
//*********************************************************
#define DNN_IRQ_2       ((volatile uint32_t *) 0xA0002000)
#define DNN_IRQ_1       ((volatile uint32_t *) 0xA0002004)
#define DNN_IRQ_0       ((volatile uint32_t *) 0xA0002008)
#define DNN_IRQ_CTRL    ((volatile uint32_t *) 0xA000200C)

//*********************************************************
// GOC Trigger
//*********************************************************
#define IRQ14VEC ((volatile uint32_t *) 0x00000078)

//*********************************************************
// INCLUDES...
//*********************************************************
#include <stdint.h>
#include <stdbool.h>
#include "DLCv1_RF.h"

//*********************************************************
// FUNCTIONS
//*********************************************************


/**
 * @brief   Insert 'nop's 
 *
 * @param   ticks       Num of 'nop's to be inserted. The actual idle time would be 2*ticks cycles due to the 'for' loop.
 *
 * @usage   delay(100);
 */
void delay(unsigned ticks);


/**
 * @brief   Wait for interrupt
 *
 * @param   N/A
 *
 * @usage   WFI();
 */
void WFI();


/**
 * @brief   Enable all interrupts
 *
 * @param   N/A
 *
 * @usage   enable_all_irq();
 */
void enable_all_irq();


/**
 * @brief   Disable all interrupts
 *
 * @param   N/A
 *
 * @usage   disable_all_irq();
 */
void disable_all_irq();


/**
 * @brief   Clear all pending interrupts
 *
 * @param   N/A
 *
 * @usage   clear_all_pend_irq();
 */
void clear_all_pend_irq();

/**
 * @brief   Only enable register-related interrupts
 *
 * @param   N/A
 *
 * @usage   enable_reg_irq();
 */
void enable_reg_irq();


/**
 * @brief   Set a new CONFIG_HALT_CPU value
 *
 * @param   new_config  4-bit CONFIG_HALT_CPU value
 *
 * @usage   set_halt_config(0x9);
 *          set_halt_config(HALT_UNTIL_MBUS_RX);
 */
void set_halt_config(uint8_t new_config);


/**
 * @brief   Return the current CONFIG_HALT_CPU value
 *
 * @param   N/A
 *
 * @usage   uint8_t current_halt_cpu_setting = get_current_halt_config();
 */
uint8_t get_current_halt_config(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = reg_id;
 *
 * @param   reg_id  8-bit Register interrupt masking pattern
 *
 * @usage   set_halt_until_reg(0xF0);
 */
void set_halt_until_reg(uint8_t reg_id);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_UNTIL_MEM_WR;
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mem_wr();
 */
void set_halt_until_mem_wr(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_UNTIL_MBUS_RX;
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mbus_rx();
 */
void set_halt_until_mbus_rx(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_UNTIL_MBUS_TX;
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mbus_tx();
 */
void set_halt_until_mbus_tx(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_UNTIL_MBUS_FWD;
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mbus_fwd();
 */
void set_halt_until_mbus_fwd(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_DISABLE;
 *
 * @param   N/A
 *
 * @usage   set_halt_disable();
 */
void set_halt_disable(void);


/**
 * @brief   Immediately put CPU in halt. CPU resumes its operation when the event specifiedin CONFIG_HALT_CPU occurs.
 *
 * @param   N/A
 *
 * @usage   halt_cpu();
 */
void halt_cpu(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              OLD_MSG_REG_MASK     = 1;
 *
 * @param   N/A
 *
 * @usage   enable_old_msg_irq();
 */
void enable_old_msg_irq(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              OLD_MSG_REG_MASK     = 0;
 *
 * @param   N/A
 *
 * @usage   disable_old_msg_irq();
 */
void disable_old_msg_irq(void);


/**
 * @brief   Configure the 16-bit timer
 *
 * @param   cmp0    Number0 to compare
 * @param   cmp1    Number1 to compare
 * @param   irq_en  Interrupt Enable
 * @param   cnt     Manually set the timer counter
 * @param   status  Manually set the timer status
 */

//void config_timerwd(uint32_t cnt);

/**
 * @brief   Disable the Watch-Dog timer
 */

//void disable_timerwd();


/**
 * @brief   Configure the wake-up timer
 *
 * @param   timestamp   Counter value at which the timer generates an interrupt
 * @param   irq_en      Interrupt Enable
 * @param   reset       1 will reset the wake-up timer
 */
//void set_wakeup_timer( uint16_t timestamp, uint8_t irq_en, uint8_t reset );


/**
 * @brief   Write into ARB debug register
 *          !!!    THIS IS FOR VERILOG SIM ONLY    !!!
 *          !!!IT WILL NOT WORK ON THE SILICON CHIP!!!
 *
 * @param   code        data to be written to the debug register
 */
void arb_debug_reg (uint32_t code);

//// user code
#define DNN_PG_CTRL_0  ((volatile uint32_t *) 0xA0000174)
#define DNN_PG_CTRL_1  ((volatile uint32_t *) 0xA00001B4)
#define DNN_PG_CTRL_2  ((volatile uint32_t *) 0xA00001F4)
#define DNN_PG_CTRL_3  ((volatile uint32_t *) 0xA0000234)

void set_all_buffer0();
void set_all_buffer1();
void switch_inst_buffer(uint8_t PE, bool slot);
void write_instruction(uint16_t inst_no, uint8_t PE, bool slot);
void write_instruction_4PE(uint16_t inst_no, bool slot);
void write_instruction_24word(uint32_t word, uint8_t PE, uint8_t addr, bool slot); 
//void read_dnn_sram(uint16_t start_address, uint32_t* data, uint16_t length);  // TODO: make it signed array
void read_dnn_sram(uint16_t start_address, int32_t* data, uint16_t length);
void read_dnn_sram_16(uint16_t start_address, int16_t* data, uint16_t length);
void read_dnn_sram_12_output(uint16_t start_address, int16_t* data, uint16_t length);
void read_dnn_sram_6_fft_output(uint16_t start_address, int32_t* data_r, int32_t* data_i);
void write_dnn_sram_6_fft_input(uint16_t start_address, int32_t* data_r, int32_t* data_i);
void write_dnn_sram_12_fft_input(uint16_t start_address, int16_t* data);
void write_dnn_sram_16_fft_input(uint16_t start_address, int16_t* data, uint16_t length);
void write_dnn_sram_12_dnn_input(uint16_t start_address, int16_t* data, uint16_t length);
void write_dnn_sram(uint16_t start_address, int32_t* data, uint16_t length);
void write_dnn_sram_16(uint16_t start_address, int16_t* data, uint16_t length);
void write_nli(uint16_t inst_no, uint8_t PE);
void reset_PE(uint8_t PE);

void start_pe_inst(uint8_t PE);
void wait_until_pe_start(uint8_t PE);
void wait_until_pe_finish(uint8_t PE);
bool check_if_pe_finish(uint8_t PE);

void clock_gate(); 
void clock_gate_dlc(); 
void signal_done();
void signal_debug(uint16_t pe_num);
void send_dnn_irq();

#endif
/////////////
