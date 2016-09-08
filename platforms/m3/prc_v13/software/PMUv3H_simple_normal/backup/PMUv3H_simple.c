//*******************************************************************
//Author: Yejoong Kim
//Description: Developed during PRCv13 tape-out for verification
//*******************************************************************
#include "PRCv13.h"
#include "PRCv13_RF.h"
#include "mbus.h"
#include "SNSv7.h"
#include "HRVv2.h"
#include "RADv9.h"
#include "PMUv2_RF.h"

// TStack order  PRC->RAD->SNS->PMU
#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define HRV_ADDR 0x6
#define FLP_ADDR 0x7
#define PMU_ADDR 0x8

#define MBUS_DELAY	100

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t wakeup_period;
volatile uint32_t cyc_num_prc;
volatile uint32_t sum_num_prc;

volatile prcv13_r0B_t prcv13_r0B = PRCv13_R0B_DEFAULT;


//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_0(void)  __attribute__ ((interrupt ("IRQ")));
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
void handler_ext_int_14(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0(void)  { *NVIC_ICPR = (0x1 << 0);  } // TIMER32
void handler_ext_int_1(void)  { *NVIC_ICPR = (0x1 << 1);  } // TIMER16
void handler_ext_int_2(void)  { *NVIC_ICPR = (0x1 << 2);  } // REG0
void handler_ext_int_3(void)  { *NVIC_ICPR = (0x1 << 3);  } // REG1
void handler_ext_int_4(void)  { *NVIC_ICPR = (0x1 << 4);  } // REG
void handler_ext_int_5(void)  { *NVIC_ICPR = (0x1 << 5);  } // REG3
void handler_ext_int_6(void)  { *NVIC_ICPR = (0x1 << 6);  } // REG4
void handler_ext_int_7(void)  { *NVIC_ICPR = (0x1 << 7);  } // REG5
void handler_ext_int_8(void)  { *NVIC_ICPR = (0x1 << 8);  } // REG6
void handler_ext_int_9(void)  { *NVIC_ICPR = (0x1 << 9);  } // REG7
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); } // MEM WR
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); } // MBUS_RX
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); } // MBUS_TX
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); } // MBUS_FWD
void handler_ext_int_14(void) { *NVIC_ICPR = (0x1 << 14); } // MBUS_FWD


//*******************************************************************
// FUNCTIONS 
//*******************************************************************

inline static void mbus_pmuv3h_register_write(uint32_t reg_id, uint32_t reg_data){
    set_halt_until_mbus_rx();
    mbus_remote_register_write(PMU_ADDR,reg_id,reg_data); 
    set_halt_until_mbus_tx();
}

inline static void mbus_pmuv3h_register_read(uint32_t reg_id){
    set_halt_until_mbus_rx();
    mbus_remote_register_write(PMU_ADDR,0x00,reg_id); 
    set_halt_until_mbus_tx();
}


//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {


    enumerated = 0xDEADBEEF;
	wakeup_period = 5;
	cyc_num_prc = 0;
	sum_num_prc = 0;

    set_halt_until_mbus_rx();
    // Enumeration
 	mbus_enumerate(RAD_ADDR);
 	mbus_enumerate(SNS_ADDR);
 	mbus_enumerate(FLP_ADDR);
 	mbus_enumerate(PMU_ADDR);

    set_halt_until_mbus_tx();

	// Register 0x17: UPCONV_TRIM_V3_SLEEP
    mbus_pmuv3h_register_write(0x17, 
		( (3 << 14) // [0, 3]  Desired Vout/Vin ratio
		| (1 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (0 << 5)  // [0, 15] Frequency multiplier L (actually L+1)
		| (4) 		// [2, 31] Floor frequency base
	));
	// Register 0x18: UPCONV_TRIM_V3_ACTIVE
    mbus_pmuv3h_register_write(0x18, 
		( (3 << 14) // [0, 3]  Desired Vout/Vin ratio
		| (1 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (2 << 5)  // [2, 15] Frequency multiplier L (actually L+1)
		| (10) 		// [2, 31] Floor frequency base
	));
	// Register 0x19: DOWNCONV_TRIM_V3_SLEEP
    mbus_pmuv3h_register_write(0x19,
		( (1 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (0 << 5)  // [0, 15] Frequency multiplier L (actually L+1)
		| (2) 		// [1, 31] Floor frequency base
	));
	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
    mbus_pmuv3h_register_write(0x1A,
		( (1 << 13) // [1, 1]  Enable main feedback loop
		| (8 << 9)  // [4, 15] Frequency multiplier R
		| (4 << 5)  // [2, 15] Frequency multiplier L (actually L+1)
		| (8) 		// [8, 31] Floor frequency base
	));
	// Register 0x15: SAR_TRIM_v3_SLEEP
    mbus_pmuv3h_register_write(0x15, 
		( (0 << 19) // [0, 1]  Enable PFM even during periodic reset
		| (0 << 18) // [0, 1]  Enable PFM even when Vref is not used as ref
		| (0 << 17) // [0, 1]  Enable PFM
		| (3 << 14) // [3, 7]  Comparator clock division ratio
		| (1 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (0 << 5)  // [0, 15] Frequency multiplier L (actually L+1)
		| (4) 		// [1, 31] Floor frequency base
	));
	// Register 0x16: SAR_TRIM_v3_ACTIVE
    mbus_pmuv3h_register_write(0x16, 
		( (0 << 19) // [0, 1]  Enable PFM even during periodic reset
		| (0 << 18) // [0, 1]  Enable PFM even when Vref is not used as ref
		| (0 << 17) // [0, 1]  Enable PFM
		| (3 << 14) // [3, 7]  Comparator clock division ratio
		| (1 << 13) // [1, 1]  Enable main feedback loop
		| (8 << 9)  // [4, 15] Frequency multiplier R
		| (8 << 5)  // [2, 15] Frequency multiplier L (actually L+1)
		| (15) 		// [8, 31] Floor frequency base
	));
	// Register 0x05: SAR_RATIO_OVERRIDE
    mbus_pmuv3h_register_write(0x05,
		( (1 << 13) // [1, 1]
        | (0 << 12) // [0, 1]
        | (1 << 11) // [0, 1]   Enable override setting [10]
		| (0 << 10) // [0, 1]   Have the converter have the periodic reset
		| (1 << 9)  // [0, 1]   Enable override setting [8]
		| (0 << 8)  // [0, 1]   Switch input / output power rails for upconversion
		| (1 << 7)  // [0, 1]   Enable override setting [6:0]
		| (44) 		// [0, 127] Binary converter's conversion ratio
	));
	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    mbus_pmuv3h_register_write(0x36,
        (1)         // [500, 16777215] Num of clock cycles the controller waits per each periodic conversion ratio adjustment after being fully turned-on
    );
	// Register 0x37: TICK_WAKEUP_WAIT
    mbus_pmuv3h_register_write(0x37, 2000); // [500, 16777215] Num of clock cycles the controller waits for internal state transition from sleep to wakeup mode

	// Disable PMU ADC measurement in active mode
	// Register 0x3A: PMU_CONTROLLER_STALL_ACTIVE
    mbus_pmuv3h_register_write(0x3A, 
		(( 0 << 0)  // [0, 1] state_sar_scn_on
		| (0 << 1)  // [0, 1] state_wait_for_clock_cycles
		| (0 << 2)  // [0, 1] state_wait_for_time
		| (0 << 3)  // [0, 1] state_sar_scn_reset
		| (0 << 4)  // [0, 1] state_sar_scn_stabilized
		| (0 << 5)  // [0, 1] state_sar_scn_ratio_roughly_adjusted
		| (0 << 6)  // [0, 1] state_clock_supply_switched
		| (0 << 7)  // [0, 1] state_control_supply_switched
		| (0 << 8)  // [0, 1] state_upconverter_on
		| (0 << 9)  // [0, 1] state_upconverter_stabilized
		| (0 << 10) // [0, 1] state_refgen_on
		| (1 << 11) // [0, 1] state_adc_output_ready
		| (0 << 12) // [0, 1] state_adc_adjusted
		| (0 << 13) // [0, 1] state_sar_scn_ratio_adjusted
		| (0 << 14) // [0, 1] state_downconverter_on
		| (0 << 15) // [0, 1] state_downconverter_stabilized
		| (0 << 16) // [0, 1] state_vdd_3p6_turned_on
		| (0 << 17) // [0, 1] state_vdd_1p2_turned_on
		| (0 << 18) // [0, 1] state_vdd_0P6_turned_on
		| (1 << 19) // [0, 1] state_state_horizon
	));
	// Register 0x3B: PMU_CONTROLLER_DESIRED_STATE_SLEEP
	mbus_pmuv3h_register_write(0x3B,
		(( 1 << 0)  // [1, 1] state_sar_scn_on
		| (1 << 1)  // [1, 1] state_wait_for_clock_cycles
		| (1 << 2)  // [1, 1] state_wait_for_time
		| (1 << 3)  // [1, 1] state_sar_scn_reset
		| (1 << 4)  // [1, 1] state_sar_scn_stabilized
		| (1 << 5)  // [1, 1] state_sar_scn_ratio_roughly_adjusted
		| (1 << 6)  // [1, 1] state_clock_supply_switched
		| (1 << 7)  // [1, 1] state_control_supply_switched
		| (1 << 8)  // [1, 1] state_upconverter_on
		| (1 << 9)  // [1, 1] state_upconverter_stabilized
		| (1 << 10) // [1, 1] state_refgen_on
		| (1 << 11) // [1, 1] state_adc_output_ready
		| (0 << 12) // [1, 1] state_adc_adjusted
		| (1 << 13) // [1, 1] state_sar_scn_ratio_adjusted
		| (1 << 14) // [1, 1] state_downconverter_on
		| (1 << 15) // [1, 1] state_downconverter_stabilized
		| (1 << 16) // [1, 1] state_vdd_3p6_turned_on
		| (1 << 17) // [1, 1] state_vdd_1p2_turned_on
		| (1 << 18) // [1, 1] state_vdd_0P6_turned_on
		| (1 << 19) // [1, 1] state_state_horizon
	));
	// Register 0x3C: PMU_CONTROLLER_DESIRED_STATE_ACTIVE
	mbus_pmuv3h_register_write(0x3C,
		(( 1 << 0)  // [1, 1] state_sar_scn_on
		| (1 << 1)  // [1, 1] state_wait_for_clock_cycles
		| (1 << 2)  // [1, 1] state_wait_for_time
		| (1 << 3)  // [1, 1] state_sar_scn_reset
		| (1 << 4)  // [1, 1] state_sar_scn_stabilized
		| (1 << 5)  // [1, 1] state_sar_scn_ratio_roughly_adjusted
		| (1 << 6)  // [1, 1] state_clock_supply_switched
		| (1 << 7)  // [1, 1] state_control_supply_switched
		| (1 << 8)  // [1, 1] state_upconverter_on
		| (1 << 9)  // [1, 1] state_upconverter_stabilized
		| (1 << 10) // [1, 1] state_refgen_on
		| (1 << 11) // [1, 1] state_adc_output_ready
		| (0 << 12) // [1, 1] state_adc_adjusted
		| (0 << 13) // [0, 1] state_sar_scn_ratio_adjusted
		| (1 << 14) // [1, 1] state_downconverter_on
		| (1 << 15) // [1, 1] state_downconverter_stabilized
		| (1 << 16) // [1, 1] state_vdd_3p6_turned_on
		| (1 << 17) // [1, 1] state_vdd_1p2_turned_on
		| (1 << 18) // [1, 1] state_vdd_0P6_turned_on
		| (0 << 19) // [0, 1] state_state_horizon
	));

}

//********************************************************************
// MAIN function starts here             
//***************************************************************-*****

int main() {


    set_wakeup_timer(100, 0, 1); // Reset Wakeup Timer; 
	enable_reg_irq(); // Initialize Interrupts, Only enable register-related interrupts
    config_timerwd(0x3FFFFF); // Config watchdog timer to about 10 sec (default: 0x02FFFFFF), 0xFFFFF about 13 sec with Y2 run default clock
//	disable_timerwd();

    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        initialization();
    }

	uint32_t cyc_num;
	uint32_t sum_num;

	delay(MBUS_DELAY*10);
	mbus_remote_register_read(PMU_ADDR,0x4B,0x0);
	delay(MBUS_DELAY*10);
	mbus_remote_register_read(PMU_ADDR,0x4C,0x1);
	delay(MBUS_DELAY*10);

	cyc_num = *REG0;
	sum_num = *REG1;

	delay(MBUS_DELAY*10);
	mbus_write_message32(0xCC, cyc_num);
	delay(MBUS_DELAY*10);
	mbus_write_message32(0xCC, sum_num);
	delay(MBUS_DELAY*10);
	mbus_write_message32(0xCC, cyc_num_prc);
	delay(MBUS_DELAY*10);
	mbus_write_message32(0xCC, sum_num_prc);
	delay(MBUS_DELAY*10);
	
	sum_num = sum_num + cyc_num++;
	sum_num_prc = sum_num_prc + cyc_num_prc++;

	delay(MBUS_DELAY*10);
	mbus_remote_register_write(PMU_ADDR,0x4B,cyc_num);
	delay(MBUS_DELAY*10);
	mbus_remote_register_write(PMU_ADDR,0x4C,sum_num);
	delay(MBUS_DELAY*10);

	delay(MBUS_DELAY*10);
	mbus_remote_register_read(PMU_ADDR,0x4B,0x0);
	delay(MBUS_DELAY*10);
	mbus_remote_register_read(PMU_ADDR,0x4C,0x1);
	delay(MBUS_DELAY*10);

	cyc_num = *REG0;
	sum_num = *REG1;

	delay(MBUS_DELAY*10);
	mbus_write_message32(0xCC, cyc_num);
	delay(MBUS_DELAY*10);
	mbus_write_message32(0xCC, sum_num);
	delay(MBUS_DELAY*10);
	mbus_write_message32(0xCC, cyc_num_prc);
	delay(MBUS_DELAY*10);
	mbus_write_message32(0xCC, sum_num_prc);
	delay(MBUS_DELAY*10);


	if (cyc_num_prc == 5){
		// Deep Sleep Operation
		mbus_remote_register_write(PMU_ADDR,0x4D,0);
		delay(MBUS_DELAY*10);
		mbus_remote_register_write(PMU_ADDR,0x4E,0x1E);
		delay(MBUS_DELAY*10);
		mbus_remote_register_write(PMU_ADDR,0x4F,0xDEAD);
		delay(MBUS_DELAY*10);
	}else{
		 set_wakeup_timer(wakeup_period, 1, 1);
    	 mbus_sleep_all();
	}

    while(1){
	delay(100);}
}
