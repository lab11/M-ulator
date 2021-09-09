//*******************************************************************
//Author: Yejoong Kim
//Description: PMUv12 (in CISv2) Testing
//
//Board: Single-Socket Board
//Chips: PREv21E -> SNTv5 -> MRRv11A -> PMUv12 (CISv2)
//
// Behavior upon GOC/EP IRQ:
//
// REG0
// [23:16]  [15:8]  [7:0]   Description
// ----------------------------------------------
// ----------------------------------------------
//
//*******************************************************************
  
#include "PREv21E.h"
#include "PREv21E_RF.h"
#include "SNTv5_RF.h"
#include "MRRv11A_RF.h"
#include "PMUv12_RF.h"
#include "mbus.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// Short Prefixes
#define SNT_ADDR 0x4
#define MRR_ADDR 0x5
#define PMU_ADDR 0x6

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint32_t count;

// CPU IRQ List
volatile cpu_irq_list_t irq_pending = CPU_IRQ_LIST_DEFAULT;

// SNT Register File
volatile sntv5_r08_t sntv5_r08 = SNTv5_R08_DEFAULT;
volatile sntv5_r09_t sntv5_r09 = SNTv5_R09_DEFAULT;
volatile sntv5_r17_t sntv5_r17 = SNTv5_R17_DEFAULT;
volatile sntv5_r19_t sntv5_r19 = SNTv5_R19_DEFAULT;
volatile sntv5_r1A_t sntv5_r1A = SNTv5_R1A_DEFAULT;

//*******************************************************************
// INTERRUPT HANDLERS (Enable only what you need)
//*******************************************************************
//void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ"))); 
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_timer16  (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_mbustx   (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_mbusrx   (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_mbusfwd  (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_reg4     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_reg5     (void) __attribute__ ((interrupt ("IRQ"))); 
void handler_ext_int_reg6     (void) __attribute__ ((interrupt ("IRQ"))); 
void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_mbusmem  (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_aes      (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_gpio     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_spi      (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_xot      (void) __attribute__ ((interrupt ("IRQ"))); 
//-------------------------------------------------------------------
//void handler_ext_int_wakeup   (void){ *NVIC_ICPR = (0x1 << IRQ_WAKEUP    ); irq_pending.wakeup     = 0x1; }
//void handler_ext_int_softreset(void){ *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET); irq_pending.soft_reset = 0x1; }
void handler_ext_int_gocep    (void){ *NVIC_ICPR = (0x1 << IRQ_GOCEP     ); irq_pending.gocep      = 0x1; }
//void handler_ext_int_timer32  (void){ *NVIC_ICPR = (0x1 << IRQ_TIMER32   ); irq_pending.timer32    = 0x1; }
//void handler_ext_int_timer16  (void){ *NVIC_ICPR = (0x1 << IRQ_TIMER16   ); irq_pending.timer16    = 0x1; }
//void handler_ext_int_mbustx   (void){ *NVIC_ICPR = (0x1 << IRQ_MBUS_TX   ); irq_pending.mbus_tx    = 0x1; }
//void handler_ext_int_mbusrx   (void){ *NVIC_ICPR = (0x1 << IRQ_MBUS_RX   ); irq_pending.mbus_rx    = 0x1; }
//void handler_ext_int_mbusfwd  (void){ *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD  ); irq_pending.mbus_fwd   = 0x1; }
//void handler_ext_int_reg0     (void){ *NVIC_ICPR = (0x1 << IRQ_REG0      ); irq_pending.reg0       = 0x1; }
//void handler_ext_int_reg1     (void){ *NVIC_ICPR = (0x1 << IRQ_REG1      ); irq_pending.reg1       = 0x1; }
//void handler_ext_int_reg2     (void){ *NVIC_ICPR = (0x1 << IRQ_REG2      ); irq_pending.reg2       = 0x1; }
//void handler_ext_int_reg3     (void){ *NVIC_ICPR = (0x1 << IRQ_REG3      ); irq_pending.reg3       = 0x1; }
//void handler_ext_int_reg4     (void){ *NVIC_ICPR = (0x1 << IRQ_REG4      ); irq_pending.reg4       = 0x1; }
//void handler_ext_int_reg5     (void){ *NVIC_ICPR = (0x1 << IRQ_REG5      ); irq_pending.reg5       = 0x1; }
void handler_ext_int_reg6     (void){ *NVIC_ICPR = (0x1 << IRQ_REG6      ); irq_pending.reg6       = 0x1; }
void handler_ext_int_reg7     (void){ *NVIC_ICPR = (0x1 << IRQ_REG7      ); irq_pending.reg7       = 0x1; }
//void handler_ext_int_mbusmem  (void){ *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM  ); irq_pending.mbus_mem   = 0x1; }
//void handler_ext_int_aes      (void){ *NVIC_ICPR = (0x1 << IRQ_AES       ); irq_pending.aes        = 0x1; }
//void handler_ext_int_gpio     (void){ *NVIC_ICPR = (0x1 << IRQ_GPIO      ); irq_pending.gpio       = 0x1; }
//void handler_ext_int_spi      (void){ *NVIC_ICPR = (0x1 << IRQ_SPI       ); irq_pending.spi        = 0x1; }
//void handler_ext_int_xot      (void){ *NVIC_ICPR = (0x1 << IRQ_XOT       ); irq_pending.xot        = 0x1; }

//***************************************************
// Function Declarations
//***************************************************
static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data);

static void operation_init(void);
static void operation_sleep(void);
static void operation_sleep_with_timer(uint32_t duration);

static uint32_t get_pmu_adc_result();

//********************************************************************
// User-Defined Functions
//********************************************************************

static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
    set_halt_until_mbus_trx();
    mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
    set_halt_until_mbus_tx();
}

static void operation_init(void){

    //Enumerate & Initialize Registers
    enumerated = 0x5453104b; // 0x5453 is TS in ascii
    count = 0;
  
    //Enumeration
    set_halt_until_mbus_trx();
    mbus_enumerate(SNT_ADDR);
    mbus_enumerate(MRR_ADDR);
    mbus_enumerate(PMU_ADDR);
    set_halt_until_mbus_tx();

	// PMU Settings ----------------------------------------------
    
    // Disable ADC in Active
	// PMU_REG#60 (0x3C): CTRL_DESIRED_STATE_ACTIVE
	pmu_reg_write(60,
		(( 1 <<  0) //sar_on
		| (1 <<  1) //wait_for_clock_cycles
		| (1 <<  2) //wait_for_cap_charge
		| (1 <<  3) //sar_reset
		| (1 <<  4) //sar_stabilized
		| (1 <<  5) //sar_ratio_roughly_adjusted
		| (1 <<  6) //clock_supply_switched
		| (1 <<  7) //control_supply_switched
		| (1 <<  8) //upc_on
		| (1 <<  9) //upc_stabilized
		| (1 << 10) //refgen_on
		| (0 << 11) //adc_output_ready
		| (0 << 12) //adc_adjusted
		| (0 << 13) //sar_ratio_adjusted
		| (1 << 14) //dnc_on
		| (1 << 15) //dnc_stabilized
		| (1 << 16) //vdd_3p6_turned_on
		| (1 << 17) //vdd_1p2_turned_on
		| (1 << 18) //vdd_0p6_turned_on
		| (0 << 19) //vbat_read_only
		| (0 << 20) //horizon
	));

	// PMU_REG#54 (0x36): TICK_REPEAT_VBAT_ADJUST
    pmu_reg_write(54, 1); 
	delay(2000);

	// PMU_REG#51 (0x33): TICK_ADC_RESET
	pmu_reg_write(51, 2);

    // Enable ADC in Sleep
	// PMU_REG#59 (0x3B): CTRL_DESIRED_STATE_SLEEP
	pmu_reg_write(59,
		(( 1 <<  0) //sar_on
		| (1 <<  1) //wait_for_clock_cycles
		| (1 <<  2) //wait_for_cap_charge
		| (1 <<  3) //sar_reset
		| (1 <<  4) //sar_stabilized
		| (1 <<  5) //sar_ratio_roughly_adjusted
		| (1 <<  6) //clock_supply_switched
		| (1 <<  7) //control_supply_switched
		| (1 <<  8) //upc_on
		| (1 <<  9) //upc_stabilized
		| (1 << 10) //refgen_on
		| (1 << 11) //adc_output_ready
		| (0 << 12) //adc_adjusted
		| (1 << 13) //sar_ratio_adjusted
		| (1 << 14) //dnc_on
		| (1 << 15) //dnc_stabilized
		| (1 << 16) //vdd_3p6_turned_on
		| (1 << 17) //vdd_1p2_turned_on
		| (1 << 18) //vdd_0p6_turned_on
		| (0 << 19) //vbat_read_only
		| (1 << 20) //horizon
	));

    //-----------------------------------------
    // Use PMU ADC Flip-Mode
    
//    // Get ADC Offset Measurement
//    uint32_t adc_result = get_pmu_adc_result();
//    uint32_t adc_offset = (adc_result >> 8) & 0xFF;
//
//	// PMU_REG#02 (0x02): ADC_CONFIG_OVERRIDE
//	pmu_reg_write(2,
//		(( 1    << 23) // Enable Override
//		| (1    << 17) // ADC Sampling Voltage (0: VREF; 1: VBAT)
//		| (0    << 16) // ADC Reference Voltage (0: VREF; 1: VBAT)
//		| (0x10 <<  8) // ADC SAMPLING BIT (approx. use (ADC_SAMPLING_BIT+ADC_SAMPLING_LSB / 256) x SamplingVoltage)
//		| (0    <<  7) // ADC SAMPLING BIT (approx. use (ADC_SAMPLING_BIT+ADC_SAMPLING_LSB / 256) x SamplingVoltage)
//		| (adc_offset << 0) // ADC Offset
//	));

    // Fix SAR Ratio
    // PMU_REG#15 (0x0F): SAR_TRIM_SLEEP
    // Bit[16]: Disable reverse configuration for upconversion
    pmu_reg_write(0x00, 0x0F);
    uint32_t pmu_reg15_default = *REG0 & 0xFFFFFF;
	pmu_reg_write(15, ( pmu_reg15_default | (0x1 << 16) ));
    
    // PMU_REG#05 (0x05): SAR_RATIO_OVERRIDE
    pmu_reg_write(0x00, 0x04);
    uint32_t sar_ratio = *REG0 & 0x7F;
	pmu_reg_write(5,
		(( 0 << 13) // Enable override setting [12] (Default: 1'b0)
		| (0 << 12) // Let VDD_CLK always connected to VBAT (Default: 1'b0)
		| (1 << 11) // Enable override setting [10] (Default: 1'b1)
		| (0 << 10) // Have the converter have the periodic reset (Default: 1'b0)
		| (0 <<  9) // Enable override setting [8] (Default: 1'b0)
		| (0 <<  8) // Switch input/output power rails for upconversion (Default: 1'b0)
		| (1 <<  7) // Enable override setting [6:0] (Default: 1'b0)
		| (sar_ratio << 0) // SAR converter's conversion ratio (Default: 7'b0)
	));

    //-----------------------------------------

}

static void operation_sleep(void){
	*GOC_DATA_IRQ = 0; // Reset GOC_DATA_IRQ
    mbus_sleep_all();
    while(1);
}

static void operation_sleep_with_timer(uint32_t duration){
    set_wakeup_timer(/*timestamp*/duration, /*irq_en*/1, /*reset*/1);
    operation_sleep();
}

static uint32_t get_pmu_adc_result(){
    // This function uses REG0.
    // Return Value:
    //      [23:16] - vbat_adc_dout
    //      [15: 8] - adc_offset
    //      [ 7: 0] - adc_dout
	pmu_reg_write(0x00,0x03);
    return *REG0 & 0xFFFFFF;
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(){

    // Only enable relevant interrupts
	*NVIC_ISER = (1 << IRQ_GOCEP);

    // Disable watchdog timers
    *TIMERWD_GO = 0;
    *REG_MBUS_WD = 0;

    // Initialization sequence
    if (enumerated != 0x5453104b){
        operation_init();
    }

    mbus_write_message32(0xA0, count);
    mbus_write_message32(0xA1, get_pmu_adc_result());
    count++;

    // Go to Sleep
    operation_sleep_with_timer(30);

    while(1);
}
