//*******************************************************************
//Authors: Sechang Oh, Minchang Cho, Gordy Carichner
//Description: MoD audio system
//    v3.0: updates for CISv3B & ADOv7; 2 flash instances only
//    v3.1: move GOCEP processing out of interrupt subroutine; keep XO in HP mode
//    v3.1.1: clean up triggers; generalized flash & radio-out functions; improved compression demo
//    v3.2: switch back to ADOv6B
//*******************************************************************
#include "PREv22E.h"
#include "PREv22E_RF.h"
#include "ADOv6VB_RF.h"
#include "PMUv13_RF.h"
#include "MRRv11A_RF.h"
#include "mbus.h"
#define ENUMID 0xDEADBEEF

#include "DFT_LUT.txt"
#include "DCT_PHS.txt"

////////////////////////////////////////
// uncomment this for debug 
#define DEBUG_MODE          // Test VDD ON in ULP
#define DEBUG_MBUS_MSG      // Debug MBus message
/////////////////////////////////////////
#define SNT_ADDR 0x2
#define MRR_ADDR 0x3
#define FLP1_ADDR 0x4
#define FLP2_ADDR 0x5
#define ADO_ADDR 0x8
#define PMU_ADDR 0x9

// System parameters
#define MBUS_DELAY 100 // Amount of delay between successive messages; 100: ~7ms (MOD Audio Debug Stack)

// XO Initialization Wait Duration
#define XO_WAIT_A  50000    // Must be ~1 second delay. Delay for XO Start-Up. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
#define XO_WAIT_B  50000    // Must be ~1 second delay. Delay for VLDO & IBIAS Generation. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).

// CP parameter
#define CP_DELAY 50000 // Amount of delay for ADO charge pump charging-up; ~4.5s

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

#define EP_MODE     ((volatile uint32_t *) 0xA0000028)

// Radio configurations
#define RADIO_DATA_NUM_WORDS 3 // number of 32-bit words to transmit
#define RADIO_DATA_LENGTH 192 // 96 bit header, 96 bit data
#define WAKEUP_PERIOD_RADIO_INIT 0xA // About 1.5 sec (PRCv18)
#define WAKEUP_PERIOD_SNT 0x20 // About 3 sec (PRCv18)

#define TEMP_STORAGE_SIZE 8192 // MEMv1: 16kB, 8k 2-byte data

// Flash access parameters
#define _SIZE_TEMP_ 64
#define _LOG2_TEMP_ 6   // = log2(_SIZE_TEMP_)
#define _ITER_TEMP_ 128  // = 8192 / _SIZE_TEMP_

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
//volatile uint32_t irq_history;
uint32_t temp_storage[_SIZE_TEMP_];
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t error_code;
volatile uint32_t finst;
volatile uint32_t pmu_sar_conv_ratio_val_test_on;
volatile uint32_t pmu_sar_conv_ratio_val_test_off;
volatile uint32_t read_data_batadc;
volatile uint32_t read_data_batadc_diff;
volatile uint32_t mem_read_data[2];

volatile prev22e_r0B_t prev22e_r0B = PREv22E_R0B_DEFAULT;
volatile prev22e_r19_t prev22e_r19 = PREv22E_R19_DEFAULT;
volatile prev22e_r1A_t prev22e_r1A = PREv22E_R1A_DEFAULT;
volatile prev22e_r1B_t prev22e_r1B = PREv22E_R1B_DEFAULT;
volatile prev22e_r1C_t prev22e_r1C = PREv22E_R1C_DEFAULT;
volatile prev22e_r1F_t prev22e_r1F = PREv22E_R1F_DEFAULT;

volatile adov6vb_r00_t adov6vb_r00 = ADOv6VB_R00_DEFAULT;
volatile adov6vb_r04_t adov6vb_r04 = ADOv6VB_R04_DEFAULT;
volatile adov6vb_r07_t adov6vb_r07 = ADOv6VB_R07_DEFAULT;
volatile adov6vb_r0B_t adov6vb_r0B = ADOv6VB_R0B_DEFAULT;
volatile adov6vb_r0D_t adov6vb_r0D = ADOv6VB_R0D_DEFAULT;
volatile adov6vb_r0E_t adov6vb_r0E = ADOv6VB_R0E_DEFAULT;
volatile adov6vb_r0F_t adov6vb_r0F = ADOv6VB_R0F_DEFAULT;
volatile adov6vb_r10_t adov6vb_r10 = ADOv6VB_R10_DEFAULT;
volatile adov6vb_r11_t adov6vb_r11 = ADOv6VB_R11_DEFAULT;
volatile adov6vb_r12_t adov6vb_r12 = ADOv6VB_R12_DEFAULT;
volatile adov6vb_r13_t adov6vb_r13 = ADOv6VB_R13_DEFAULT;
volatile adov6vb_r14_t adov6vb_r14 = ADOv6VB_R14_DEFAULT;
volatile adov6vb_r15_t adov6vb_r15 = ADOv6VB_R15_DEFAULT;
volatile adov6vb_r16_t adov6vb_r16 = ADOv6VB_R16_DEFAULT;
volatile adov6vb_r17_t adov6vb_r17 = ADOv6VB_R17_DEFAULT;
volatile adov6vb_r18_t adov6vb_r18 = ADOv6VB_R18_DEFAULT;
volatile adov6vb_r19_t adov6vb_r19 = ADOv6VB_R19_DEFAULT;
volatile adov6vb_r1A_t adov6vb_r1A = ADOv6VB_R1A_DEFAULT;
volatile adov6vb_r1B_t adov6vb_r1B = ADOv6VB_R1B_DEFAULT;
volatile adov6vb_r1C_t adov6vb_r1C = ADOv6VB_R1C_DEFAULT;

uint32_t read_data[128];

static void operation_sleep_notimer(void);
//static void operation_sleep_timer(void);
static void pmu_set_sar_override(uint32_t val);
static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);
static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);
static void pmu_set_active_3p6(uint8_t r, uint8_t l, uint8_t base);

volatile uint32_t radio_tx_option;
volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t mrr_freq_hopping;
volatile uint32_t mrr_freq_hopping_step;
volatile uint32_t mrr_cfo_val_fine_min;
volatile uint32_t radio_data_arr[RADIO_DATA_NUM_WORDS];
volatile uint32_t RADIO_PACKET_DELAY;
volatile uint32_t radio_packet_count;
volatile uint32_t num_total_16b_packet;
volatile uint32_t TEMP_CALIB_A;
volatile uint32_t TEMP_CALIB_B;
volatile uint32_t NUM_MEAS_USER;

volatile mrrv11a_r00_t mrrv11a_r00 = MRRv11A_R00_DEFAULT;
volatile mrrv11a_r01_t mrrv11a_r01 = MRRv11A_R01_DEFAULT;
volatile mrrv11a_r02_t mrrv11a_r02 = MRRv11A_R02_DEFAULT;
volatile mrrv11a_r03_t mrrv11a_r03 = MRRv11A_R03_DEFAULT;
volatile mrrv11a_r04_t mrrv11a_r04 = MRRv11A_R04_DEFAULT;
volatile mrrv11a_r07_t mrrv11a_r07 = MRRv11A_R07_DEFAULT;
volatile mrrv11a_r10_t mrrv11a_r10 = MRRv11A_R10_DEFAULT;
volatile mrrv11a_r11_t mrrv11a_r11 = MRRv11A_R11_DEFAULT;
volatile mrrv11a_r12_t mrrv11a_r12 = MRRv11A_R12_DEFAULT;
volatile mrrv11a_r13_t mrrv11a_r13 = MRRv11A_R13_DEFAULT;
volatile mrrv11a_r14_t mrrv11a_r14 = MRRv11A_R14_DEFAULT;
volatile mrrv11a_r15_t mrrv11a_r15 = MRRv11A_R15_DEFAULT;
volatile mrrv11a_r16_t mrrv11a_r16 = MRRv11A_R16_DEFAULT;
volatile mrrv11a_r1F_t mrrv11a_r1F = MRRv11A_R1F_DEFAULT;
volatile mrrv11a_r21_t mrrv11a_r21 = MRRv11A_R21_DEFAULT;

//***************************************************
// CRC16 Encoding
//***************************************************
#define DATA_LEN 96
#define CRC_LEN 16

uint32_t* crcEnc16()
{
    // intialization
    uint16_t i;

    uint16_t poly = 0xc002;
    uint16_t poly_not = ~poly;
    uint16_t remainder = 0x0000;
    uint16_t remainder_shift = 0x0000;
    uint32_t data2 = (radio_data_arr[2] << CRC_LEN) + (radio_data_arr[1] >> CRC_LEN);
    uint32_t data1 = (radio_data_arr[1] << CRC_LEN) + (radio_data_arr[0] >> CRC_LEN);
    uint32_t data0 = radio_data_arr[0] << CRC_LEN;

    // LFSR
    uint16_t input_bit;
    for (i = 0; i < DATA_LEN; i++)
    {
        uint16_t MSB;
        if (remainder > 0x7fff)
            MSB = 0xffff;
        else
            MSB = 0x0000;

        if (i < 32)
            input_bit = ((data2 << i) > 0x7fffffff);
        else if (i < 64)
            input_bit = (data1 << (i-32)) > 0x7fffffff;
        else
            input_bit = (data0 << (i-64)) > 0x7fffffff;

        remainder_shift = remainder << 1;
        remainder = (poly&((remainder_shift)^MSB))|(((poly_not)&(remainder_shift)) + (input_bit^(remainder > 0x7fff)));
    }

    static uint32_t msg_out[1];
    msg_out[0] = data0 + remainder;

    return msg_out;    
}

//***************************************************
// Timeout Functions
//***************************************************

static void set_timer32_timeout(uint32_t val){
	// Use Timer32 as timeout counter
    wfi_timeout_flag = 0;
	config_timer32(val, 1, 0, 0);
}

static void stop_timer32_timeout_check(uint32_t code){
	// Turn off Timer32
	*TIMER32_GO = 0;
	if (wfi_timeout_flag){
		wfi_timeout_flag = 0;
		error_code = code;
		mbus_write_message32(0xFA, error_code);
	}
}

//***************************************************
// MRR Functions
//***************************************************

static void radio_power_on(){

    // Turn off Current Limter Briefly
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Set decap to parallel
    mrrv11a_r03.TRX_DCP_S_OW1 = 0;  //TX_Decap S (forced charge decaps)
    mrrv11a_r03.TRX_DCP_S_OW2 = 0;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    mrrv11a_r03.TRX_DCP_P_OW1 = 1;  //RX_Decap P 
    mrrv11a_r03.TRX_DCP_P_OW2 = 1;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    delay(MBUS_DELAY);

    // Set decap to series
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    delay(MBUS_DELAY);

    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    
    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Release timer power-gate
    mrrv11a_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    delay(MBUS_DELAY);

    // Turn on timer
    mrrv11a_r04.RO_RESET = 0;  //Release Reset TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    delay(MBUS_DELAY);

    mrrv11a_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);
    delay(MBUS_DELAY);

    mrrv11a_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);

    // Release FSM Sleep
    mrrv11a_r11.FSM_SLEEP = 0;  // Power on BB
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    delay(MBUS_DELAY*5); // Freq stab
}

static void radio_power_off() {

    // In case continuous mode was running
    mrrv11a_r11.FSM_CONT_PULSE_MODE = 0;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
        
    // Turn off FSM
    mrrv11a_r03.TRX_ISOLATEN = 0;     //set ISOLATEN 0
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);

    mrrv11a_r11.FSM_EN = 0;  //Stop BB
    mrrv11a_r11.FSM_RESET_B = 0;  //RST BB
    mrrv11a_r11.FSM_SLEEP = 1;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    
    // Turn off Current Limiter Briefly
    mrrv11a_r00.TRX_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Current Limiter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Turn on Current Limiter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Turn off RO
    mrrv11a_r04.RO_RESET = 1;  //Release Reset TIMER
    mrrv11a_r04.RO_EN_CLK = 0; //Enable CLK TIMER
    mrrv11a_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);

    // Enable timer power-gate
    mrrv11a_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv11a_r04.as_int);

    radio_on = 0;
    radio_ready = 0;
}

static void srr_configure_pulse_width_short(){
	
    mrrv11a_r12.FSM_TX_PW_LEN = 0; //4us PW
    mrrv11a_r13.FSM_TX_C_LEN = 32; // (PW_LEN+1):C_LEN=1:32
    mrrv11a_r12.FSM_TX_PS_LEN = 1; // PW=PS   
	
    mbus_remote_register_write(MRR_ADDR,0x12,mrrv11a_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv11a_r13.as_int);
}

static void send_radio_data_mrr_sub1() {

    // Use timer32 as timeout counter
    set_timer32_timeout(TIMER32_VAL);

    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Fire off data
    mrrv11a_r11.FSM_RESET_B = 1;    
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);    
    mrrv11a_r11.FSM_EN = 1;  //Start BB
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);

    // Wait for radio response
    WFI();
    stop_timer32_timeout_check(0x3);

    // Turn off Current Limter
    mrrv11a_r00.TRX_CL_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    mrrv11a_r11.FSM_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
    mrrv11a_r11.FSM_RESET_B = 0;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
}

uint16_t mrr_cfo_val_fine = 0;
uint16_t mrr_trx_cap_antn_tune_coarse = 0;

static void mrr_send_radio_data(uint16_t last_packet) {

    // clean up radio_data_arr[2]
    radio_data_arr[2] = radio_data_arr[2] & 0x0000FFFF;

    // CRC16 Encoding 
    uint32_t* crc_data = crcEnc16();

    if(!radio_on) {
        radio_on = 1;
    radio_power_on();
    }
    
    radio_data_arr[2] = crc_data[0] << 16 | radio_data_arr[2];
    mbus_copy_mem_from_local_to_remote_bulk(MRR_ADDR, (uint32_t *) 0x00000000, (uint32_t *) radio_data_arr, 2);

    if (!radio_ready){
        radio_ready = 1;

        // Release FSM Reset
        //mrrv11a_r11.MRR_RAD_FSM_RSTN = 1;  //UNRST BB
        //mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);
        //delay(MBUS_DELAY);

        mrrv11a_r03.TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
        mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
        delay(MBUS_DELAY);
    }
        
    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 1; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

//#ifdef USE_RAD
    // uint16_t mrr_cfo_val_fine = 0;
    uint16_t count = 0;
    // uint16_t num_packets = 1;
    // if (mrr_freq_hopping) num_packets = mrr_freq_hopping;

    // mrr_cfo_val_fine = 0x0000;

    while (count < mrr_freq_hopping){
		count++;

		*TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
		*REG_MBUS_WD = 0; // Disables Mbus watchdog timer

		mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
		mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine;
		mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
		send_radio_data_mrr_sub1();
		delay(RADIO_PACKET_DELAY);
		// mrr_cfo_val_fine = mrr_cfo_val_fine + mrr_freq_hopping_step; // 1: 0.8MHz, 2: 1.6MHz step
		// 5 hop pattern
		//mrr_cfo_val_fine += 8; //no change to CFO packet-to-packet
		//if(mrr_cfo_val_fine > 20) {
		//	mrr_cfo_val_fine -= 20;
	//}
    }

    radio_packet_count++;
    if (last_packet){
    radio_ready = 0;
    radio_power_off();
    }

}

static inline void mrr_init() {
    // MRR Settings --------------------------------------
    
    // Decap in series
    mrrv11a_r03.TRX_DCP_P_OW1 = 0;  //RX_Decap P 
    mrrv11a_r03.TRX_DCP_P_OW2 = 0;  //RX_Decap P 
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);
    mrrv11a_r03.TRX_DCP_S_OW1 = 1;  //TX_Decap S (forced charge decaps)
    mrrv11a_r03.TRX_DCP_S_OW2 = 1;  //TX_Decap S (forced charge decaps)
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv11a_r03.as_int);

    // Current Limter set-up 
    mrrv11a_r00.TRX_CL_CTRL = 8; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Turn on Current Limter
    mrrv11a_r00.TRX_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);

    // Wait for charging decap
    // config_timerwd(TIMERWD_VAL);
    // *REG_MBUS_WD = 1500000*3; // default: 1500000
    delay(MBUS_DELAY*200); // Wait for decap to charge

    mrrv11a_r1F.LC_CLK_RING = 0x3;  // ~ 150 kHz
    mrrv11a_r1F.LC_CLK_DIV = 0x3;  // ~ 150 kHz
    mbus_remote_register_write(MRR_ADDR,0x1F,mrrv11a_r1F.as_int);

    srr_configure_pulse_width_short();

    mrr_freq_hopping = 1;
    mrr_freq_hopping_step = 4; // determining center freq

    mrr_cfo_val_fine_min = 0x0000;

    // RO setup (SFO)
    // Adjust Diffusion R
    mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF

    // Adjust Poly R
    mbus_remote_register_write(MRR_ADDR,0x08,0x400000); // RO_POLY

    // Adjust C
    mrrv11a_r07.RO_MOM = 0x10;
    mrrv11a_r07.RO_MIM = 0x10;
    mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);

    // TX Setup Carrier Freq
    mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = 0x0;  //ANT CAP 10b unary 830.5 MHz // FIXME: adjust per stack    // TODO: make them the same variable
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_COARSE = 0x0; //ANT CAP 10b unary 830.5 MHz
    mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
    mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
    mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
    mrrv11a_r02.TX_EN_OW = 0; // Turn off TX_EN_OW
    mrrv11a_r02.TX_BIAS_TUNE = 0x1FFF;  //Set TX BIAS TUNE 13b // Max 0x1FFF
    mbus_remote_register_write(MRR_ADDR,0x02,mrrv11a_r02.as_int);

    // Turn off RX mode
    mrrv11a_r03.TRX_MODE_EN = 0; //Set TRX mode
    mbus_remote_register_write(MRR_ADDR,3,mrrv11a_r03.as_int);

    mrrv11a_r14.FSM_TX_POWERON_LEN = 0; //3bits
    mrrv11a_r15.FSM_RX_DATA_BITS = 0x00; //Set RX data 1b
    mbus_remote_register_write(MRR_ADDR,0x14,mrrv11a_r14.as_int);
    mbus_remote_register_write(MRR_ADDR,0x15,mrrv11a_r15.as_int);

    // RAD_FSM set-up 
    mrrv11a_r10.FSM_TX_TP_LEN = 80;       // Num. Training Pulses (bit 0); Valid Range: 1 ~ 255 (Default: 80)
    mrrv11a_r10.FSM_TX_PASSCODE = 0x7AC8; // 16-bit Passcode (sent from LSB to MSB)
    mbus_remote_register_write(MRR_ADDR,0x10,mrrv11a_r10.as_int);
    mrrv11a_r11.FSM_TX_DATA_BITS = RADIO_DATA_NUM_WORDS * 32; //0-skip tx data
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv11a_r11.as_int);

    // Mbus return address
    mbus_remote_register_write(MRR_ADDR,0x1E,0x1002);

    // Additional delay for charging decap
    // config_timerwd(TIMERWD_VAL);
    // *REG_MBUS_WD = 1500000; // default: 1500000
    delay(MBUS_DELAY*200); // Wait for decap to charge
}

//*******************************************************************
// XO Functions
//*******************************************************************
static void xo_start(void) {
    //--------------------------------------------------------------------------
    // XO Driver (XO_DRV_V3_TSMC180) Start-Up Sequence
    //--------------------------------------------------------------------------
    // RESETn       __|*********************************************************
    // PGb_StartUp  __|***************************|_____________________________
    // START_UP     **************************|_________________________________
    // ISOL_CLK_HP  **********|_________________|*******************************
    // ISOL_CLK_LP  ******************|_________________________________________
    //                |<--A-->|<--B-->|<--C-->|.|.|<-- Low Power Operation -->
    //--------------------------------------------------------------------------
    // A: ~1s  (XO Start-Up): NOTE: You may need more time here due to the weak power-gate switch.
    // B: ~1s  (VLDO & IBIAS generation)
    // C: <1ms (SCN Output Generation)
    // .(dot): minimum delay
    //--------------------------------------------------------------------------

    prev22e_r19.XO_RESETn       = 1;
    prev22e_r19.XO_PGb_START_UP = 1;
    *REG_XO_CONF1 = prev22e_r19.as_int;

	delay(XO_WAIT_A); // Delay A (~1s; XO Start-Up)

    prev22e_r19.XO_ISOL_CLK_HP = 0;
    *REG_XO_CONF1 = prev22e_r19.as_int;
    
    delay(XO_WAIT_B); // Delay B (~1s; VLDO & IBIAS generation)

    prev22e_r19.XO_ISOL_CLK_LP = 0;
    *REG_XO_CONF1 = prev22e_r19.as_int;
    
    delay(100); // Delay C (~1ms; SCN Output Generation)
	
 // Stay in HP mode since mbus traffic kills XO w/this PCB
    prev22e_r19.XO_START_UP = 0;
    *REG_XO_CONF1 = prev22e_r19.as_int;

    prev22e_r19.XO_ISOL_CLK_HP = 1;
    *REG_XO_CONF1 = prev22e_r19.as_int;

    prev22e_r19.XO_PGb_START_UP = 0;
    *REG_XO_CONF1 = prev22e_r19.as_int;

    delay(100); // Dummy Delay

	
    enable_xo_timer();
    start_xo_cout(); // turns on XO clk output pad
}

static void xo_stop( void ) {
    // Stop the XO Driver
    prev22e_r19.XO_ISOL_CLK_LP = 1;
    *REG_XO_CONF1 = prev22e_r19.as_int;

    prev22e_r19.XO_RESETn   = 0;
    prev22e_r19.XO_START_UP = 1;
    *REG_XO_CONF1 = prev22e_r19.as_int;
}

static void XO_init(void) {
    prev22e_r19.XO_SEL_VLDO = 0x0;	// Default value: 3'h0
    prev22e_r19.XO_CAP_TUNE = 0x0;	// Default value: 3'h0
    prev22e_r19.XO_SEL_vV0P6 = 0x0;	// Default value: 2'h2
    *REG_XO_CONF1 = prev22e_r19.as_int;

    prev22e_r1A.XO_SEL_CLK_OUT_DIV = 0x4;
    prev22e_r1A.XO_SEL_DLY = 0x1; 	// Default value: 3'h1
    prev22e_r1A.XO_INJ = 0x0; 		// Default value: 2'h2
    *REG_XO_CONF2 = prev22e_r1A.as_int;

    xo_start();
    
#ifdef DEBUG_MBUS_MSG
    mbus_write_message32(0xE3,0x00000E2D); 
#endif    
}

//************************************
// PMU Related Functions
//************************************
static void pmu_set_sar_override(uint32_t val){
    // SAR_RATIO_OVERRIDE
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
			| (0 << 12) // Let VDD_CLK always connected to vbat
			| (1 << 11) // Enable override setting [10] (1'h0)
			| (0 << 10) // Have the converter have the periodic reset (1'h0)
			| (1 << 9) // Enable override setting [8] (1'h0)
			| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
			| (1 << 7) // Enable override setting [6:0] (1'h0)
			| (val)       // Binary converter's conversion ratio (7'h00)
		));
		delay(MBUS_DELAY*2);
		mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
            ( (1 << 13) // Enables override setting [12] (1'b1)
				| (0 << 12) // Let VDD_CLK always connected to vbat
				| (1 << 11) // Enable override setting [10] (1'h0)
				| (0 << 10) // Have the converter have the periodic reset (1'h0)
				| (1 << 9) // Enable override setting [8] (1'h0)
				| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
				| (1 << 7) // Enable override setting [6:0] (1'h0)
				| (val)       // Binary converter's conversion ratio (7'h00)
			));
			delay(MBUS_DELAY*2);
}


inline static void pmu_set_adc_period(uint32_t val){
    // PMU_CONTROLLER_DESIRED_STATE Active
    mbus_remote_register_write(PMU_ADDR,0x3C,
		((  1 << 0) //state_sar_scn_on
			| (0 << 1) //state_wait_for_clock_cycles
			| (1 << 2) //state_wait_for_time
			| (1 << 3) //state_sar_scn_reset
			| (1 << 4) //state_sar_scn_stabilized
			| (1 << 5) //state_sar_scn_ratio_roughly_adjusted
			| (1 << 6) //state_clock_supply_switched
			| (1 << 7) //state_control_supply_switched
			| (1 << 8) //state_upconverter_on
			| (1 << 9) //state_upconverter_stabilized
			| (1 << 10) //state_refgen_on
			| (0 << 11) //state_adc_output_ready
			| (0 << 12) //state_adc_adjusted
			| (0 << 13) //state_sar_scn_ratio_adjusted
			| (1 << 14) //state_downconverter_on
			| (1 << 15) //state_downconverter_stabilized
			| (1 << 16) //state_vdd_3p6_turned_on
			| (1 << 17) //state_vdd_1p2_turned_on
			| (1 << 18) //state_vdd_0P6_turned_on
			| (1 << 19) //state_state_horizon
		));
		delay(MBUS_DELAY*10);
		
		// Register 0x36: TICK_REPEAT_VBAT_ADJUST
		mbus_remote_register_write(PMU_ADDR,0x36,val); 
		delay(MBUS_DELAY*10);
		
		// Register 0x33: TICK_ADC_RESET
		mbus_remote_register_write(PMU_ADDR,0x33,2);
		delay(MBUS_DELAY);
		
		// Register 0x34: TICK_ADC_CLK
		mbus_remote_register_write(PMU_ADDR,0x34,2);
		delay(MBUS_DELAY);
		
		// PMU_CONTROLLER_DESIRED_STATE Active
		mbus_remote_register_write(PMU_ADDR,0x3C,
            ((  1 << 0) //state_sar_scn_on
				| (1 << 1) //state_wait_for_clock_cycles
				| (1 << 2) //state_wait_for_time
				| (1 << 3) //state_sar_scn_reset
				| (1 << 4) //state_sar_scn_stabilized
				| (1 << 5) //state_sar_scn_ratio_roughly_adjusted
				| (1 << 6) //state_clock_supply_switched
				| (1 << 7) //state_control_supply_switched
				| (1 << 8) //state_upconverter_on
				| (1 << 9) //state_upconverter_stabilized
				| (1 << 10) //state_refgen_on
				| (0 << 11) //state_adc_output_ready
				| (0 << 12) //state_adc_adjusted
				| (0 << 13) //state_sar_scn_ratio_adjusted
				| (1 << 14) //state_downconverter_on
				| (1 << 15) //state_downconverter_stabilized
				| (1 << 16) //state_vdd_3p6_turned_on
				| (1 << 17) //state_vdd_1p2_turned_on
				| (1 << 18) //state_vdd_0P6_turned_on
				| (1 << 19) //state_state_horizon
			));
			delay(MBUS_DELAY);
}


inline static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){
	
    // The first register write to PMU needs to be repeated
    // Register 0x16: V1P2 Active
    mbus_remote_register_write(PMU_ADDR,0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
			| (0 << 18) // Enable PFM even when Vref is not used as ref
			| (0 << 17) // Enable PFM
			| (3 << 14) // Comparator clock division ratio
			| (0 << 13) // Enable main feedback loop
			| (r << 9)  // Frequency multiplier R
			| (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
			| (base)      // Floor frequency base (0-63)
		));
		delay(MBUS_DELAY);
		mbus_remote_register_write(PMU_ADDR,0x16, 
            ( (0 << 19) // Enable PFM even during periodic reset
				| (0 << 18) // Enable PFM even when Vref is not used as ref
				| (0 << 17) // Enable PFM
				| (3 << 14) // Comparator clock division ratio
				| (0 << 13) // Enable main feedback loop
				| (r << 9)  // Frequency multiplier R
				| (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
				| (base)      // Floor frequency base (0-63)
			));
			delay(MBUS_DELAY);
			// Register 0x18: V3P6 Active 
			mbus_remote_register_write(PMU_ADDR,0x18, 
				( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
					| (0 << 13) // Enable main feedback loop
					| (r << 9)  // Frequency multiplier R
					| (l << 5)  // Frequency multiplier L (actually L+1)
					| (base)      // Floor frequency base (0-63)
				));
				delay(MBUS_DELAY);
				// Register 0x1A: V0P6 Active
				mbus_remote_register_write(PMU_ADDR,0x1A,
					( (0 << 13) // Enable main feedback loop
						| (r << 9)  // Frequency multiplier R
						| (l << 5)  // Frequency multiplier L (actually L+1)
						| (base)      // Floor frequency base (0-63)
					));
					delay(MBUS_DELAY);
					
}

inline static void pmu_set_active_3p6(uint8_t r, uint8_t l, uint8_t base){
	
    // The first register write to PMU needs to be repeated
    // Register 0x16: V1P2 Active
	mbus_remote_register_write(PMU_ADDR,0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
			| (0 << 13) // Enable main feedback loop
			| (r << 9)  // Frequency multiplier R
			| (l << 5)  // Frequency multiplier L (actually L+1)
			| (base)      // Floor frequency base (0-63)
		));
	delay(MBUS_DELAY);
	mbus_remote_register_write(PMU_ADDR,0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
			| (0 << 13) // Enable main feedback loop
			| (r << 9)  // Frequency multiplier R
			| (l << 5)  // Frequency multiplier L (actually L+1)
			| (base)      // Floor frequency base (0-63)
		));
	delay(MBUS_DELAY);

}
inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){
	
    // Register 0x17: V3P6 Sleep
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
			| (0 << 13) // Enable main feedback loop
			| (r << 9)  // Frequency multiplier R
			| (l << 5)  // Frequency multiplier L (actually L+1)
			| (base)      // Floor frequency base (0-63)
		));
		
		//mbus_remote_register_write(PMU_ADDR,0x17, 
		//        ( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		//          | (0 << 13) // Enable main feedback loop
		//          | (1 << 9)  // Frequency multiplier R
		//          | (0 << 5)  // Frequency multiplier L (actually L+1)
		//          | (8)         // Floor frequency base (0-63)
		//        ));
		delay(MBUS_DELAY);
		// Register 0x15: V1P2 Sleep
		mbus_remote_register_write(PMU_ADDR,0x15, 
            ( (0 << 19) // Enable PFM even during periodic reset
				| (0 << 18) // Enable PFM even when Vref is not used as ref
				| (0 << 17) // Enable PFM
				| (3 << 14) // Comparator clock division ratio
				| (0 << 13) // Enable main feedback loop
				| (r << 9)  // Frequency multiplier R
				| (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
				| (base)      // Floor frequency base (0-63)
			));
			delay(MBUS_DELAY);
			// Register 0x19: V0P6 Sleep
			mbus_remote_register_write(PMU_ADDR,0x19,
				( (0 << 13) // Enable main feedback loop
					| (r << 9)  // Frequency multiplier R
					| (l << 5)  // Frequency multiplier L (actually L+1)
					| (base)      // Floor frequency base (0-63)
				));
				delay(MBUS_DELAY);
}

inline static void pmu_set_clk_init(){
    //pmu_set_sar_override(0x4A);
    //pmu_set_active_clk(0xA,0x4,0x10,0x4);
    //pmu_set_sleep_clk(0xA,0x4,0x10,0x4); //with TEST1P2
    //pmu_set_sar_override(0x5A);
    //pmu_set_active_clk(0xA,0x4,0x10,0x4); //-10C test
    pmu_set_sleep_clk(0x6,0x4,0x4,0x8); //with TEST1P2
    pmu_set_active_clk(0x8,0x4,0x10,0x4); //-10C test
    //pmu_set_sleep_clk(0xF,0xF,0x1F,0xF); //with TEST1P2
    // SAR_RATIO_OVERRIDE
    // Use the new reset scheme in PMUv3
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
			| (0 << 12) // Let VDD_CLK always connected to vbat
			| (1 << 11) // Enable override setting [10] (1'h0)
			| (0 << 10) // Have the converter have the periodic reset (1'h0)
			| (0 << 9) // Enable override setting [8] (1'h0)
			| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
			| (0 << 7) // Enable override setting [6:0] (1'h0)
			| (0x45)      // Binary converter's conversion ratio (7'h00)
		));
		delay(MBUS_DELAY);
		pmu_set_sar_override(pmu_sar_conv_ratio_val_test_on);
		//pmu_set_sar_override(0x4A);
		//  pmu_set_sar_override(0x4D);
		//  pmu_set_sar_override(0x2D);
		//  pmu_set_sar_override(0x29);
		
		pmu_set_adc_period(1); // 0x100 about 1 min for 1/2/1 1P2 setting
}

inline static void pmu_adc_reset_setting(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Active
    mbus_remote_register_write(PMU_ADDR,0x3C,
		((  1 << 0) //state_sar_scn_on
			| (1 << 1) //state_wait_for_clock_cycles
			| (1 << 2) //state_wait_for_time
			| (1 << 3) //state_sar_scn_reset
			| (1 << 4) //state_sar_scn_stabilized
			| (1 << 5) //state_sar_scn_ratio_roughly_adjusted
			| (1 << 6) //state_clock_supply_switched
			| (1 << 7) //state_control_supply_switched
			| (1 << 8) //state_upconverter_on
			| (1 << 9) //state_upconverter_stabilized
			| (1 << 10) //state_refgen_on
			| (0 << 11) //state_adc_output_ready
			| (0 << 12) //state_adc_adjusted
			| (0 << 13) //state_sar_scn_ratio_adjusted
			| (1 << 14) //state_downconverter_on
			| (1 << 15) //state_downconverter_stabilized
			| (1 << 16) //state_vdd_3p6_turned_on
			| (1 << 17) //state_vdd_1p2_turned_on
			| (1 << 18) //state_vdd_0P6_turned_on
			| (1 << 19) //state_state_horizon
		));
		delay(MBUS_DELAY);
}


inline static void pmu_adc_disable(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Sleep
    mbus_remote_register_write(PMU_ADDR,0x3B,
		((  1 << 0) //state_sar_scn_on
			| (1 << 1) //state_wait_for_clock_cycles
			| (1 << 2) //state_wait_for_time
			| (1 << 3) //state_sar_scn_reset
			| (1 << 4) //state_sar_scn_stabilized
			| (1 << 5) //state_sar_scn_ratio_roughly_adjusted
			| (1 << 6) //state_clock_supply_switched
			| (1 << 7) //state_control_supply_switched
			| (1 << 8) //state_upconverter_on
			| (1 << 9) //state_upconverter_stabilized
			| (1 << 10) //state_refgen_on
			| (0 << 11) //state_adc_output_ready
			| (0 << 12) //state_adc_adjusted
			| (0 << 13) //state_sar_scn_ratio_adjusted
			| (1 << 14) //state_downconverter_on
			| (1 << 15) //state_downconverter_stabilized
			| (1 << 16) //state_vdd_3p6_turned_on
			| (1 << 17) //state_vdd_1p2_turned_on
			| (1 << 18) //state_vdd_0P6_turned_on
			| (0 << 19) //state_vbat_readonly
			| (1 << 20) //state_state_horizon
		));
		delay(MBUS_DELAY);
}

inline static void pmu_adc_enable(){
    // PMU ADC will be automatically reset when system wakes up
    // PMU_CONTROLLER_DESIRED_STATE Sleep
    mbus_remote_register_write(PMU_ADDR,0x3B,
		((  1 << 0) //state_sar_scn_on
			| (1 << 1) //state_wait_for_clock_cycles
			| (1 << 2) //state_wait_for_time
			| (1 << 3) //state_sar_scn_reset
			| (1 << 4) //state_sar_scn_stabilized
			| (1 << 5) //state_sar_scn_ratio_roughly_adjusted
			| (1 << 6) //state_clock_supply_switched
			| (1 << 7) //state_control_supply_switched
			| (1 << 8) //state_upconverter_on
			| (1 << 9) //state_upconverter_stabilized
			| (1 << 10) //state_refgen_on
			| (1 << 11) //state_adc_output_ready
			| (0 << 12) //state_adc_adjusted // Turning off offset cancellation
			| (1 << 13) //state_sar_scn_ratio_adjusted
			| (1 << 14) //state_downconverter_on
			| (1 << 15) //state_downconverter_stabilized
			| (1 << 16) //state_vdd_3p6_turned_on
			| (1 << 17) //state_vdd_1p2_turned_on
			| (1 << 18) //state_vdd_0P6_turned_on
			| (1 << 19) //state_state_horizon
		));
		delay(MBUS_DELAY);
}

inline static void pmu_reset_solar_short(){
    mbus_remote_register_write(PMU_ADDR,0x0E, 
		( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
			| (1 << 9)  // Enables override setting [8]
			| (0 << 8)  // Turn on the harvester-inhibiting switch
			| (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
			| (0)         // clamp_tune_top (decreases clamp thresh)
		));
		delay(MBUS_DELAY);
		mbus_remote_register_write(PMU_ADDR,0x0E, 
            ( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
				| (0 << 9)  // Enables override setting [8]
				| (0 << 8)  // Turn on the harvester-inhibiting switch
				| (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
				| (0)         // clamp_tune_top (decreases clamp thresh)
			));
			delay(MBUS_DELAY);
}

//***************************************************
//  FLP functions
//***************************************************

static void flp_fail(uint32_t id)
{
	delay(10000);
	mbus_write_message32(0xE2, 0xDEADBEEF);
	delay(10000);
	mbus_write_message32(0xE2, id);
	delay(10000);
	mbus_write_message32(0xE2, 0xDEADBEEF);
	delay(10000);
	mbus_sleep_all();
	while(1);
}

static void FLASH_initialization (uint16_t flash_num) { //initializes all flash instances in range
    uint16_t i;
	for(i=0; i<flash_num; i++){
	// Tune Flash
		mbus_remote_register_write (FLP1_ADDR + i , 0x00 , 0xF84001); // T5us, T10us setting for ~100kHz CLKmain
		mbus_remote_register_write (FLP1_ADDR + i , 0x01 , 0x00AF01); // Tcyc_prog, Tprog setting for ~100kHz CLKmain
		mbus_remote_register_write (FLP1_ADDR + i , 0x26 , 0x0D7788); // Program Current: Default
		mbus_remote_register_write (FLP1_ADDR + i , 0x27 , 0x011BC8); // Erase Pump Diode Chain: Default
		mbus_remote_register_write (FLP1_ADDR + i , 0x01 , 0x007F09); // Tprog idle time: Default
		mbus_remote_register_write (FLP1_ADDR + i , 0x19 , 0x000F86); // Voltage Clamper Tuning: 1STG-0xF, 2STG-max, VOUT-0x6
		mbus_remote_register_write (FLP1_ADDR + i , 0x0F , 0x001001); // Flash interrupt target register addr: REG0 -> REG1
		//mbus_remote_register_write (FLP1_ADDR + i , 0x12 , 0x000003); // Auto Power On/Off
		mbus_remote_register_write (FLP1_ADDR + i , 0x18 , 0x00002F); // CLK_RING_SEL: 4'hF, CLK_DIV_SEL: 2'h2 -> ~200kHz
	}
}

static void FLASH_turn_on(uint16_t flash_inst) { //turn on only the selected flash
	set_halt_until_mbus_rx();
	mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x11 , 0x00002F);
	set_halt_until_mbus_tx();
	
	if (*REG1 != 0xB5) flp_fail (0);
}

static void FLASH_turn_off(uint16_t flash_inst) { //turn off only the selected flash
    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x11 , 0x00002D);
    set_halt_until_mbus_tx();
	
    if (*REG1 != 0xBB) flp_fail (1);
}

static void FLASH_pp_ready (uint16_t flash_inst) { //prep ping pong for specified flash instance
	
	// Erase Flash
	uint32_t page_start_addr = 0;
	uint32_t idx;
	
	for (idx=0; idx<1024; idx++){ // All Pages (8Mb) are erased.
		page_start_addr = (idx << 8);
		
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x08, page_start_addr); // Set FLSH_START_ADDR
		
		set_halt_until_mbus_rx();
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x09, 0x000029); // Page Erase
		set_halt_until_mbus_tx();
		
		if (*REG1 != 0x00004F) flp_fail((0xFFFF << 16) | idx);
	}
    
    // Ping Pong Setting
	mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x13 , 0x000001); // Enable Ping Pong w/ No length limit
	mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x14 , 0x00001B); // Over run err no detection & fast programming & no wrapping & DATA0/DATA1 enable
	mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x15 , 0x000000); // Flash program addr started from 0x0
}

static void FLASH_pp_on (uint16_t flash_inst) { //turn on ping pong for specified flash instance
    mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x09, 0x00002D); // Ping Pong Go
}

static void FLASH_pp_off (uint16_t flash_inst) { //turn on ping pong for specified flash instance
    set_halt_until_mbus_rx();
    mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x13, 0x000000); // Ping Pong End
    set_halt_until_mbus_tx();
}
	
static void FLASH_read (uint16_t banks) { //read flash out in 8k word banks to mbus; 32 banks per flash instance
    uint32_t i;
    uint32_t j;
	uint32_t m;
	uint32_t f_inst=0;
	uint32_t first;
    uint32_t flp_sram_addr;

	for(m=0; m<banks; m++) {
		f_inst = m / 32;
		first = !(m % 32); //indicates first bank in a flash instance
		if(first) {
			if(f_inst > 0) { //moving to next flash instance - turn off prev inst
				FLASH_turn_off(f_inst - 1);
			}
			FLASH_turn_on(f_inst); //turn on the flash instance
		}

		mbus_remote_register_write (FLP1_ADDR + f_inst, 0x08 , m*8192); // Set FLSH_START_ADDR
		mbus_remote_register_write (FLP1_ADDR + f_inst, 0x07 , 0x0); // Set SRAM_START_ADDR
	
		prev22e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x0; //disable watchdog for long access
		*REG_MBUS_WD = prev22e_r1F.as_int;
	
		set_halt_until_mbus_rx();
		mbus_remote_register_write (FLP1_ADDR + f_inst, 0x09 , 0x07FFE3); // Flash -> SRAM
		set_halt_until_mbus_tx();
	
		if (*REG1 != 0x00002B) flp_fail(4);
		
		prev22e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x16E360; //set watchdog back to default
		*REG_MBUS_WD = prev22e_r1F.as_int;
		
		flp_sram_addr = 0;
		
		for(j=0; j<64; j++){
			set_halt_until_mbus_rx();
			mbus_copy_mem_from_remote_to_any_bulk (FLP1_ADDR + f_inst, (uint32_t *) flp_sram_addr, 0x1, read_data, 127);
			set_halt_until_mbus_tx();
			delay(100);
			for (i=0; i<128; i++) {
				mbus_write_message32(0xC0, read_data[i]);
			}
			flp_sram_addr = flp_sram_addr + 512;
			mbus_write_message32(0xDD, j);
		}
	}
	FLASH_turn_off(f_inst);
}

//***************************************************
//  FLP functions for flash testing only
//***************************************************

inline static void flash_erasedata(uint16_t flash_inst){ //erase specified flash instance
	FLASH_turn_on(flash_inst);
	FLASH_pp_ready(flash_inst);
	FLASH_turn_off(flash_inst);
}

void flpv3l_sram_write_all(uint32_t data_type, uint16_t flash_inst) {
    uint32_t pattern = 0x00000000;
    uint32_t idx_i;

    FLASH_turn_on(flash_inst);
	
    // Choose Data Pattern
    if      (data_type == 0) pattern = 0x00000000;
    else if (data_type == 1) pattern = 0xFFFFFFFF;
    else if (data_type == 2) pattern = 0xAAAAAAAA;
    else if (data_type == 3) pattern = 0x55555555;

    // Initialize temp_storage
    for (idx_i=0; idx_i<_SIZE_TEMP_; idx_i++) temp_storage[idx_i] = pattern;

    // Write into SRAM
    for (idx_i=0; idx_i<_ITER_TEMP_; idx_i++) {
        uint32_t dest_mem_addr = (idx_i << (_LOG2_TEMP_ + 2));
        mbus_copy_mem_from_local_to_remote_bulk(FLP1_ADDR + flash_inst, (uint32_t *) dest_mem_addr, temp_storage, (_SIZE_TEMP_ - 1));
    }

    FLASH_turn_off(flash_inst);
}
	
static void FLASH_prog (uint16_t banks, uint16_t flash_inst) { //program flash in 8k word banks; 32 banks per flash instance
	uint32_t m;
	
	FLASH_turn_on(flash_inst);

	for(m=0; m<banks; m++) {
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x08 , m*8192); // Set FLSH_START_ADDR
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x07 , 0x0); // Set SRAM_START_ADDR
	
		prev22e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x0; //disable watchdog for long access
		*REG_MBUS_WD = prev22e_r1F.as_int;
	
		set_halt_until_mbus_rx();
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x09 , 0x07FFE7); // SRAM -> Flash
		set_halt_until_mbus_tx();
	
		if (*REG1 != 0x00005D) flp_fail(4);
		
		prev22e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x16E360; //set watchdog back to default
		*REG_MBUS_WD = prev22e_r1F.as_int;
	}
	FLASH_turn_off(flash_inst);
}

static void FLASH_test_read (uint16_t banks, uint16_t flash_inst) { //read flash out in 8k word banks to mbus; 32 banks per flash instance
    uint32_t i;
    uint32_t j;
	uint32_t m;
    uint32_t flp_sram_addr;

	FLASH_turn_on(flash_inst);
	
	for(m=0; m<banks; m++) {

		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x08 , m*8192); // Set FLSH_START_ADDR
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x07 , 0x0); // Set SRAM_START_ADDR
	
		prev22e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x0; //disable watchdog for long access
		*REG_MBUS_WD = prev22e_r1F.as_int;
	
		set_halt_until_mbus_rx();
		mbus_remote_register_write (FLP1_ADDR + flash_inst, 0x09 , 0x07FFE3); // Flash -> SRAM
		set_halt_until_mbus_tx();
	
		if (*REG1 != 0x00002B) flp_fail(4);
		
		prev22e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x16E360; //set watchdog back to default
		*REG_MBUS_WD = prev22e_r1F.as_int;
		
		flp_sram_addr = 0;
		
		for(j=0; j<64; j++){
			set_halt_until_mbus_rx();
			mbus_copy_mem_from_remote_to_any_bulk (FLP1_ADDR + flash_inst, (uint32_t *) flp_sram_addr, 0x1, read_data, 127);
			set_halt_until_mbus_tx();
			delay(100);
			for (i=0; i<128; i++) {
				mbus_write_message32(0xC0, read_data[i]);
			}
			flp_sram_addr = flp_sram_addr + 512;
			mbus_write_message32(0xDD, j);
		}
	}
	FLASH_turn_off(flash_inst);
}
//***************************************************
//  ADO functions
//***************************************************
static void ado_initialization(void){
    // as_int of all zero regs initalization
    adov6vb_r07.as_int = 0;
    adov6vb_r0B.as_int = 0;
    adov6vb_r10.as_int = 0;

    adov6vb_r10.VAD_ADC_DOUT_ISOL = 1;
    mbus_remote_register_write(ADO_ADDR, 0x10, adov6vb_r10.as_int);
	
	adov6vb_r04.DSP_CLK_MON_SEL = 4; //LP CLK mon
    mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);
 
    // AFE Initialization
    // adov6vb_r15.VAD_LNA_AMPMOD_EN = 0;
    adov6vb_r15.IB_GEN_CLK_EN = 1;
    adov6vb_r15.LS_CLK_EN_1P2 = 1;
    adov6vb_r15.REC_PGA_BWCON = 32;//24;
    adov6vb_r15.REC_PGA_CFBADD = 1;//0;
    adov6vb_r15.REC_PGA_GCON = 2;
    mbus_remote_register_write(ADO_ADDR, 0x15, adov6vb_r15.as_int); //1F8622

    adov6vb_r16.IBC_MONAMP = 0;
    adov6vb_r16.IBC_REC_LNA = 6;
    adov6vb_r16.IBC_REC_PGA = 11;
    adov6vb_r16.IBC_VAD_LNA = 7;
    adov6vb_r16.IBC_VAD_PGA = 5;
    mbus_remote_register_write(ADO_ADDR, 0x16, adov6vb_r16.as_int);//0535BD
 
    adov6vb_r18.REC_LNA_N1_CON = 2;
    adov6vb_r18.REC_LNA_N2_CON = 2;
    adov6vb_r18.REC_LNA_P1_CON = 3;
    adov6vb_r18.REC_LNA_P2_CON = 3;
    mbus_remote_register_write(ADO_ADDR, 0x18, adov6vb_r18.as_int);//0820C3

    adov6vb_r19.REC_PGA_P1_CON = 3;
    mbus_remote_register_write(ADO_ADDR, 0x19, adov6vb_r19.as_int);//007064

    adov6vb_r1B.VAD_LNA_N1_LCON = 5;
    adov6vb_r1B.VAD_LNA_N2_LCON = 6;
    adov6vb_r1B.VAD_LNA_P1_CON = 3;
    adov6vb_r1B.VAD_LNA_P2_CON = 2;
    mbus_remote_register_write(ADO_ADDR, 0x1B, adov6vb_r1B.as_int);//0A6062

    adov6vb_r1C.VAD_PGA_N1_LCON = 0;
    adov6vb_r1C.VAD_PGA_N1_MCON = 1;
    mbus_remote_register_write(ADO_ADDR, 0x1C, adov6vb_r1C.as_int);//00079C

    adov6vb_r11.LDO_CTRL_VREFLDO_VOUT_1P4HP = 4;
    mbus_remote_register_write(ADO_ADDR, 0x11, adov6vb_r11.as_int);//84CAC9

    adov6vb_r12.LDO_CTRL_VREFLDO_VOUT_0P6LP = 3;
    adov6vb_r12.LDO_CTRL_VREFLDO_VOUT_0P9HP = 3;
    adov6vb_r12.LDO_CTRL_VREFLDO_VOUT_1P4LP = 4;
    mbus_remote_register_write(ADO_ADDR, 0x12, adov6vb_r12.as_int);//828283
    
    /////////////////
    //DSP LP_TOP
    ///////////////////
    //PHS_FE Programming
    mbus_remote_register_write(ADO_ADDR, 0x05, 0x000004); //PHS_DATA_FE
    mbus_remote_register_write(ADO_ADDR, 0x06, 0x000001); //PHS_WR_FE high
    mbus_remote_register_write(ADO_ADDR, 0x06, 0x000000); //PHS_WR_FE low 
    //SD_TH Programming
    mbus_remote_register_write(ADO_ADDR, 0x09, 0x000064); //TH_DATA
    mbus_remote_register_write(ADO_ADDR, 0x0A, 0x000001); //TH_WR high
    mbus_remote_register_write(ADO_ADDR, 0x0A, 0x000000); //TH_WR low
    //LUT Programming
    uint8_t i;
    for(i= 0; i<LUT_DATA_LENGTH; i++){
        adov6vb_r07.DSP_LUT_WR_ADDR = i;
        adov6vb_r07.DSP_LUT_DATA_IN = LUT_DATA[i];
        mbus_remote_register_write(ADO_ADDR, 0x07, adov6vb_r07.as_int);
        mbus_remote_register_write(ADO_ADDR, 0x08, 0x000001);
        mbus_remote_register_write(ADO_ADDR, 0x08, 0x000000);
    }
    //PHS Programming 
    for(i= 0; i<PHS_DATA_LENGTH; i++){
        adov6vb_r0B.DSP_PHS_WR_ADDR_FS = i;
        adov6vb_r0B.DSP_PHS_DATA_IN_FS = PHS_DATA[i];
        mbus_remote_register_write(ADO_ADDR, 0x0B, adov6vb_r0B.as_int);
        mbus_remote_register_write(ADO_ADDR, 0x0C, 0x000001);
        mbus_remote_register_write(ADO_ADDR, 0x0C, 0x000000);
    }
    
    //N_DCT: 128 points DCT 
    adov6vb_r00.DSP_N_DCT = 2; //128-pt DCT
    adov6vb_r00.DSP_N_FFT = 4; //256-pt FFT
    mbus_remote_register_write(ADO_ADDR, 0x00, adov6vb_r00.as_int);
    
    //WAKEUP REQ EN
    adov6vb_r0E.DSP_WAKEUP_REQ_EN = 1;
    mbus_remote_register_write(ADO_ADDR, 0x0E, adov6vb_r0E.as_int);
}
static void digital_set_mode(uint8_t mode){
    if(mode == 1){      //LP DSP

        adov6vb_r0D.DSP_HP_ADO_GO = 0;
        adov6vb_r0D.DSP_HP_DNN_GO = 0;
        adov6vb_r0D.DSP_HP_FIFO_GO = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BCB5
        adov6vb_r0D.DSP_HP_RESETN = 0;
        adov6vb_r0D.DSP_DNN_HP_MODE_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC15
        
        adov6vb_r0D.DSP_DNN_RESETN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC05
        adov6vb_r0D.DSP_DNN_ISOLATEN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC09
        adov6vb_r0D.DSP_DNN_PG_SLEEP_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC0B
        adov6vb_r0D.DSP_DNN_CTRL_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC0A
        
        adov6vb_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040
        adov6vb_r04.DSP_P2S_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040
        
        adov6vb_r0D.DSP_LP_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC4A
        }
    else if(mode == 2){ // HP DSP
        adov6vb_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80A

        adov6vb_r0D.DSP_DNN_CTRL_RF_SEL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        adov6vb_r0D.DSP_DNN_PG_SLEEP_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B809
        adov6vb_r0D.DSP_DNN_ISOLATEN_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80D
        adov6vb_r0D.DSP_DNN_CLKENB_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B805
        adov6vb_r0D.DSP_DNN_RESETN_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B815
        
        adov6vb_r0D.DSP_DNN_HP_MODE_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B835
        
        adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        
        adov6vb_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040
        adov6vb_r04.DSP_P2S_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040

        adov6vb_r0D.DSP_HP_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B8B5
        adov6vb_r0D.DSP_HP_FIFO_GO = 1;
        adov6vb_r0D.DSP_HP_ADO_GO = 1;
        adov6vb_r0D.DSP_HP_DNN_GO = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BBB5
        }
    else if(mode == 3){ // HP Compression only
        adov6vb_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80A
        adov6vb_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040
		////adov6vb_r04.DSP_ADO_POWER_TH = 0x000D;
		////mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);
		//adov6vb_r04.DSP_ADO_POWER_TH = 0x0026;
		//mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);
		delay(MBUS_DELAY*9);

        adov6vb_r0D.DSP_DNN_CTRL_RF_SEL = 0; //from trex
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        //adov6vb_r0D.DSP_DNN_PG_SLEEP_RF = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B809
        //adov6vb_r0D.DSP_DNN_ISOLATEN_RF = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80D
        //adov6vb_r0D.DSP_DNN_RESETN_RF = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B815
        
        adov6vb_r0D.DSP_DNN_HP_MODE_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B835

        adov6vb_r0D.DSP_HP_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B8B5
        adov6vb_r0D.DSP_HP_FIFO_GO = 1;
        adov6vb_r0D.DSP_HP_ADO_GO = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BBB5
        }
    else if(mode == 0){      //DSP All Off
        adov6vb_r0D.DSP_HP_FIFO_GO = 0;
        adov6vb_r0D.DSP_HP_ADO_GO = 0;
        adov6vb_r0D.DSP_HP_DNN_GO = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BCB5
        adov6vb_r0D.DSP_HP_RESETN = 0;
        adov6vb_r0D.DSP_DNN_HP_MODE_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC15
        
        adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        adov6vb_r0D.DSP_DNN_RESETN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC05
        adov6vb_r0D.DSP_DNN_CLKENB_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC0D
        adov6vb_r0D.DSP_DNN_ISOLATEN_RF = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC09
        adov6vb_r0D.DSP_DNN_PG_SLEEP_RF = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC0B
        adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
        adov6vb_r0D.DSP_DNN_CTRL_RF_SEL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC0A
        
        adov6vb_r04.DSP_P2S_MON_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//A00040
        
        adov6vb_r04.DSP_P2S_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040
        
        adov6vb_r0D.DSP_LP_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC4A
        }
}

static void afe_set_mode(uint8_t mode){
    if(mode == 1){      //LP AFE
        //if(prev22e_r19.XO_EN_OUT !=1){// XO ouput enable
        //    prev22e_r19.XO_EN_OUT    = 1;
        //    *REG_XO_CONF1 = prev22e_r19.as_int;
		//}
        adov6vb_r0D.REC_ADC_RESETN = 0;
        adov6vb_r0D.REC_ADCDRI_EN = 0;
        adov6vb_r0D.REC_LNA_AMPEN = 0;
        adov6vb_r0D.REC_LNA_AMPSW_EN = 0;
        adov6vb_r0D.REC_LNA_OUTSHORT_EN = 0;
        adov6vb_r0D.REC_PGA_AMPEN = 0;
        adov6vb_r0D.REC_PGA_OUTSHORT_EN = 0;
        adov6vb_r0D.VAD_LNA_AMPSW_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);
        
        //adov6vb_r13.VAD_PGA_FSETTLE = 0;
        //adov6vb_r13.VAD_LNA_FSETTLE = 1;
        adov6vb_r13.VAD_PGA_OUTSHORT_EN = 0;
										
        adov6vb_r13.VAD_LNA_OUTSHORT_EN = 0;
        adov6vb_r13.LDO_PG_IREF = 0;
        adov6vb_r13.LDO_PG_VREF_0P6LP = 0;
        adov6vb_r13.LDO_PG_LDOCORE_0P6LP = 0;
        adov6vb_r13.LDO_PG_VREF_1P4LP = 0;
        adov6vb_r13.LDO_PG_LDOCORE_1P4LP = 0;
        adov6vb_r13.LDO_PG_VREF_1P4HP = 1;
        adov6vb_r13.LDO_PG_LDOCORE_1P4HP = 1;
        adov6vb_r13.LDO_PG_VREF_0P9HP = 1;
        adov6vb_r13.LDO_PG_LDOCORE_0P9HP = 1;
        mbus_remote_register_write(ADO_ADDR, 0x13, adov6vb_r13.as_int);
		delay(MBUS_DELAY*9);

        adov6vb_r0F.VAD_ADC_RESET = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0F, adov6vb_r0F.as_int);
	
        adov6vb_r14.VAD_ADCDRI_EN = 1;
        adov6vb_r14.VAD_LNA_AMPEN = 1;
        adov6vb_r14.VAD_PGA_AMPEN = 1;
        adov6vb_r14.VAD_PGA_BWCON = 19;//13;
        adov6vb_r14.VAD_PGA_GCON = 4;//8;
        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);

        ////VAD MON setting can come here
        //adov6vb_r1A.VAD_MONSEL = 1;
        //adov6vb_r1A.VAD_MONBUF_EN = 1;
        //adov6vb_r1A.VAD_REFMONBUF_EN = 0;
        //adov6vb_r1A.VAD_ADCDRI_OUT_OV_EN = 1;
        //adov6vb_r16.IBC_VAD_LNA = 5;
        //adov6vb_r16.IBC_VAD_PGA = 5;
        //mbus_remote_register_write(ADO_ADDR, 0x16, adov6vb_r16.as_int);
        
        ////REC MON setting (debug)
        adov6vb_r17.REC_MONSEL = 0;
        adov6vb_r17.REC_MONBUF_EN = 0;
        //adov6vb_r17.REC_REFMONBUF_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x17, adov6vb_r17.as_int);

        adov6vb_r10.DO_LP_HP_SEL = 0;    //0: LP, 1: HP
        adov6vb_r10.DIO_OUT_EN = 1;
        adov6vb_r10.VAD_ADC_DOUT_ISOL = 0;
        mbus_remote_register_write(ADO_ADDR, 0x10, adov6vb_r10.as_int);

        adov6vb_r04.DSP_CLK_MON_SEL = 4; //LP CLK mon
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);
    }
    else if(mode == 2){ // HP AFE
        //if(prev22e_r19.XO_EN_OUT !=1){// XO ouput enable
        //    prev22e_r19.XO_EN_OUT    = 1;
        //    *REG_XO_CONF1 = prev22e_r19.as_int;
        //}
		*EP_MODE= (*EP_MODE | 1);

		adov6vb_r13.LDO_PG_IREF = 0;
        adov6vb_r13.LDO_PG_VREF_0P6LP = 0;
        adov6vb_r13.LDO_PG_LDOCORE_0P6LP = 0;
        adov6vb_r13.LDO_PG_VREF_1P4LP = 0;
        adov6vb_r13.LDO_PG_LDOCORE_1P4LP = 0;
        adov6vb_r13.LDO_PG_VREF_1P4HP = 0;
		adov6vb_r13.LDO_PG_LDOCORE_1P4HP = 0;
        adov6vb_r13.LDO_PG_VREF_0P9HP = 0;
        adov6vb_r13.LDO_PG_LDOCORE_0P9HP = 0;
        mbus_remote_register_write(ADO_ADDR, 0x13, adov6vb_r13.as_int);
		delay(MBUS_DELAY*9);

		//adov6vb_r15.REC_PGA_GCON = 31; //used this setting for v2 demo
		adov6vb_r15.REC_PGA_GCON = 31;
        mbus_remote_register_write(ADO_ADDR, 0x15, adov6vb_r15.as_int);

        //adov6vb_r16.IBC_REC_LNA = 6;
        //adov6vb_r16.IBC_REC_PGA = 11;
        //mbus_remote_register_write(ADO_ADDR, 0x16, adov6vb_r16.as_int);

        adov6vb_r0D.REC_ADC_RESETN = 1;
        adov6vb_r0D.REC_ADCDRI_EN = 1;
        adov6vb_r0D.REC_LNA_AMPEN = 1;
        adov6vb_r0D.REC_LNA_AMPSW_EN = 1;
        //adov6vb_r0D.REC_LNA_FSETTLE = 1;
        //adov6vb_r0D.REC_PGA_FSETTLE = 0;
        adov6vb_r0D.REC_LNA_OUTSHORT_EN = 0;
        adov6vb_r0D.REC_PGA_AMPEN = 1;
        adov6vb_r0D.REC_PGA_OUTSHORT_EN = 0;
        adov6vb_r0D.VAD_LNA_AMPSW_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);
        delay(MBUS_DELAY*9);
        //adov6vb_r0D.REC_LNA_FSETTLE = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);

        adov6vb_r10.DO_LP_HP_SEL = 1;    //HP
        adov6vb_r10.DIO_OUT_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x10, adov6vb_r10.as_int);

        adov6vb_r04.DSP_CLK_MON_SEL = 2; //HP clock mon
        mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);
    }
    else{       // AFE off
        //if(prev22e_r19.XO_EN_OUT !=0){// XO ouput enable
        //    prev22e_r19.XO_EN_OUT    = 0; 
        //    *REG_XO_CONF1 = prev22e_r19.as_int;
        //}
        
		*EP_MODE = (*EP_MODE & 2);

        adov6vb_r0D.REC_ADC_RESETN = 0;
        adov6vb_r0D.REC_ADCDRI_EN = 0;
        adov6vb_r0D.REC_LNA_AMPEN = 0;
        adov6vb_r0D.REC_LNA_AMPSW_EN = 0;
										
        adov6vb_r0D.REC_LNA_OUTSHORT_EN = 0;
        adov6vb_r0D.REC_PGA_AMPEN = 0;
        adov6vb_r0D.REC_PGA_OUTSHORT_EN = 0;
        adov6vb_r0D.VAD_LNA_AMPSW_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);
        
										
										
        adov6vb_r13.LDO_PG_IREF = 0;
        adov6vb_r13.LDO_PG_VREF_0P6LP = 1;
        adov6vb_r13.LDO_PG_LDOCORE_0P6LP = 1;
        adov6vb_r13.LDO_PG_VREF_1P4LP = 1;
        adov6vb_r13.LDO_PG_LDOCORE_1P4LP = 1;
        adov6vb_r13.LDO_PG_VREF_1P4HP = 1;
        adov6vb_r13.LDO_PG_LDOCORE_1P4HP = 1;
        adov6vb_r13.LDO_PG_VREF_0P9HP = 1;
        adov6vb_r13.LDO_PG_LDOCORE_0P9HP = 1;
        mbus_remote_register_write(ADO_ADDR, 0x13, adov6vb_r13.as_int);

        adov6vb_r0F.VAD_ADC_RESET = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0F, adov6vb_r0F.as_int);

        adov6vb_r14.VAD_ADCDRI_EN = 0;
        adov6vb_r14.VAD_LNA_AMPEN = 0;
        adov6vb_r14.VAD_PGA_AMPEN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);

        adov6vb_r10.DIO_OUT_EN = 0;
        //adov6vb_r10.DIO_DIR_IN1OUT0 = 1;
        adov6vb_r10.VAD_ADC_DOUT_ISOL = 1;
        mbus_remote_register_write(ADO_ADDR, 0x10, adov6vb_r10.as_int);
        ////VAD MON setting can come here
        //adov6vb_r1A.VAD_MONSEL = 1;
        //adov6vb_r1A.VAD_MONBUF_EN = 1;
        //adov6vb_r1A.VAD_REFMONBUF_EN = 0;
        //adov6vb_r1A.VAD_ADCDRI_OUT_OV_EN = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x1A, adov6vb_r1A.as_int);
        
        ////REC MON setting (debug)
        //adov6vb_r17.REC_MONSEL = 1;
        //adov6vb_r17.REC_MONBUF_EN = 1;
        //adov6vb_r17.REC_REFMONBUF_EN = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x17, adov6vb_r17.as_int);

    }
}
static void cp_set_mode(uint32_t mode){
	if(mode==1){        //ON
   	    *TIMERWD_GO = 0x0;
   	    adov6vb_r14.CP_CLK_EN_1P2 = 1;
   	    adov6vb_r14.CP_CLK_DIV_1P2 = 2;//0;
   	    adov6vb_r14.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	    delay(CP_DELAY); 
   	    adov6vb_r14.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	    delay(CP_DELAY*1); 
   	    adov6vb_r14.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	    delay(CP_DELAY); 
   	    adov6vb_r14.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	    delay(CP_DELAY*1); 
   	    adov6vb_r14.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	    delay(CP_DELAY); 
   	    adov6vb_r14.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	}
   	else if(mode==2){        //ON
   	    adov6vb_r14.CP_CLK_EN_1P2 = 1;
   	    adov6vb_r14.CP_CLK_DIV_1P2 = 0;
   	    adov6vb_r14.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	    delay(CP_DELAY*0.2); 
   	    adov6vb_r14.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	}
   	else if(mode == 3){
   	    adov6vb_r14.CP_CLK_EN_1P2 = 1;
   	    adov6vb_r14.CP_CLK_DIV_1P2 = 0; //clk 8kHz
   	    adov6vb_r14.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	}
   	else if(mode == 4){
   	    adov6vb_r14.CP_CLK_EN_1P2 = 1;
   	    adov6vb_r14.CP_CLK_DIV_1P2 = 0; //clk 8kHz
   	    adov6vb_r14.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	}
   	else{                   //OFF
   	    adov6vb_r14.CP_CLK_EN_1P2 = 0;
   	    adov6vb_r14.CP_CLK_DIV_1P2 = 3;
   	    mbus_remote_register_write(ADO_ADDR, 0x14, adov6vb_r14.as_int);
   	}	
}

//***************************************************
//  ADO+FLP functions
//***************************************************

inline static void comp_stream(uint32_t sleep_time){ //sleep_time is wakeup timer ticks
	uint16_t i;
    *EP_MODE = (*EP_MODE | 2);
    
    //cp_set_mode(1); // charge pump will already be on from LP mode
    afe_set_mode(2); // ADO HP AFE ON
	
	delay(150000); // delay to unmount system
	
    // Flash setup and start /////
    FLASH_turn_on(0);
    FLASH_pp_ready(0);

	radio_power_on(); //send radio flag to mark start of streaming
	radio_data_arr[0] = 0;
	radio_data_arr[1] = 0xFEED;
	for(i=0; i < 10; i++){
		radio_data_arr[2] = i;
		mrr_send_radio_data(0);
		delay(RADIO_PACKET_DELAY);
	}
	radio_power_off();
	
    FLASH_pp_on(0);
	
    /////////////////////////////
    digital_set_mode(3); // HP Compression only start
    
    delay(200000); // Compression-length control: ~14s
	
    set_wakeup_timer(sleep_time,1,1);    //enable timer
    //mbus_write_message32(MBUS_POWER, (MBUS_SLEEP_BY_SHORT_PREFIX << 28) | 0x00002000);
    //mbus_write_message32(0x01, 0x20002000);
    mbus_write_message32(0x01, 0x27CEE000); // selective sleep except ADO and FLP1
    //mbus_write_message32(0x01, 0x27FDE000); // selective sleep except ADO
    //mbus_write_message32(0x01, 0x27FFE000); // selective sleep for all layers
	
	
}

inline static void comp_stream2(uint32_t sleep_time){ //sleep_time is wakeup timer ticks
	uint16_t i;
    *EP_MODE = (*EP_MODE | 2);
    
    //cp_set_mode(1); // charge pump will already be on from LP mode
    afe_set_mode(2); // ADO HP AFE ON
	
	delay(150000); // delay to unmount system
	
    // Flash setup and start /////
    FLASH_turn_on(1);
    FLASH_pp_ready(1);

	radio_power_on(); //send radio flag to mark start of streaming
	radio_data_arr[0] = 0;
	radio_data_arr[1] = 0xFEED;
	for(i=0; i < 10; i++){
		radio_data_arr[2] = i;
		mrr_send_radio_data(0);
		delay(RADIO_PACKET_DELAY);
	}
	radio_power_off();
	
    FLASH_pp_on(1);
	
    /////////////////////////////
    digital_set_mode(3); // HP Compression only start
    
    delay(200000); // Compression-length control: ~14s
	
    set_wakeup_timer(sleep_time,1,1);    //enable timer
    //mbus_write_message32(MBUS_POWER, (MBUS_SLEEP_BY_SHORT_PREFIX << 28) | 0x00002000);
    //mbus_write_message32(0x01, 0x20002000);
    mbus_write_message32(0x01, 0x27CDE000); // selective sleep except ADO and FLP2
    //mbus_write_message32(0x01, 0x27FDE000); // selective sleep except ADO
    //mbus_write_message32(0x01, 0x27FFE000); // selective sleep for all layers
	
	
}

inline static void comp_finish(void){
	
    *EP_MODE = (*EP_MODE & 1);
    digital_set_mode(0); // ADO DSP ALL OFF
    afe_set_mode(0); // ADO AFE OFF
    delay(10000);
    cp_set_mode(0); // ADO CP OFF
	
    FLASH_pp_off(finst);
    FLASH_turn_off(finst);

}


inline static void comp_stream_read(void){
	
    *EP_MODE = (*EP_MODE | 2);
    delay(200000); // Compression-length control: ~14s
    
    cp_set_mode(1); // ADO CP ON
    afe_set_mode(2); // ADO HP AFE ON
	
    // Flash setup and start /////
    FLASH_turn_on(0);
    FLASH_pp_ready(0);
    FLASH_pp_on(0);
    /////////////////////////////
	
    digital_set_mode(3); // HP Compression only start
    
    delay(800000); // Compression-length control: ~14s
	
    //freeze_gpio_out();
    //set_wakeup_timer(300,1,1);    //enable timer
    //mbus_write_message32(MBUS_POWER, (MBUS_SLEEP_BY_SHORT_PREFIX << 28) | 0x00002000);
    //mbus_write_message32(0x01, 0x20002000);
    //mbus_write_message32(0x01, 0x27CEE000); // selective sleep except ADO and FLP1
    //mbus_write_message32(0x01, 0x27FDE000); // selective sleep except ADO
    //mbus_write_message32(0x01, 0x27FFE000); // selective sleep for all layers
	
	comp_finish();
	
	FLASH_read(1);
	
}

static void operation_tx_stored(uint16_t banks){ // 1 bank = 8k words
    uint32_t j;
	uint32_t m;
	uint32_t f_inst=0;
	uint32_t first;
    uint32_t flp_sram_addr;

	for(m=0; m<banks; m++) {
		f_inst = m / 32;
		first = !(m % 32); //indicates first bank in a flash instance
		if(first) {
			if(f_inst > 0) { //moving to next flash instance - turn off prev inst
				FLASH_turn_off(f_inst - 1);
			}
			FLASH_turn_on(f_inst); //turn on the flash instance
			}

		mbus_remote_register_write (FLP1_ADDR + f_inst, 0x08 , m*8192); // Set FLSH_START_ADDR
		mbus_remote_register_write (FLP1_ADDR + f_inst, 0x07 , 0x0); // Set SRAM_START_ADDR
	
		prev22e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x0; //disable watchdog for long access
		*REG_MBUS_WD = prev22e_r1F.as_int;
	
		set_halt_until_mbus_rx();
		mbus_remote_register_write (FLP1_ADDR + f_inst, 0x09 , 0x07FFE3); // Flash -> SRAM
		set_halt_until_mbus_tx();
	
		if (*REG1 != 0x00002B) flp_fail(4);
		
		prev22e_r1F.MBUS_WATCHDOG_THRESHOLD = 0x16E360; //set watchdog back to default
		*REG_MBUS_WD = prev22e_r1F.as_int;
		
		for(j=0; j<4096; j++){
			uint32_t mem_read_data[2];
			set_halt_until_mbus_rx();
			// Read 2 words (4 data points) at a time
			mbus_copy_mem_from_remote_to_any_bulk(FLP1_ADDR + f_inst, (uint32_t*) flp_sram_addr, 0x01, (uint32_t*)&mem_read_data, 1);
			set_halt_until_mbus_tx();

			radio_data_arr[1] = mem_read_data[0];
			radio_data_arr[0] = mem_read_data[1];
			radio_data_arr[2] = j;
			mrr_send_radio_data(0);
			flp_sram_addr = flp_sram_addr + 8;
		}		
		flp_sram_addr = 0;
	}
	FLASH_turn_off(f_inst);	
}
	
//***************************************************
// End of Program Sleep Operation
//***************************************************

static void operation_sleep(void){
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;
	
    // Go to Sleep
	mbus_write_message32(0xFA, 0xABABABAB);
    mbus_sleep_all();
    while(1);
}


//static void operation_sleep_timer(void){
    // Make sure the irq counter is reset    
    //exec_count_irq = 0;
	
//    set_wakeup_timer(300,1,1);    //enable timer
    //set_xo_timer(0,1600,1,1);
    //start_xo_cnt();
    //mbus_sleep_all();
	
    // Go to sleep
//    operation_sleep();
//}


static void operation_sleep_notimer(void){
    // Make sure the irq counter is reset    
    //exec_count_irq = 0;
    
    // Make sure Radio is off
    //if (radio_on){
    //    radio_power_off();
//}

set_wakeup_timer(0,0,0);    //disable timer
//set_xo_timer(0,1600,1,1);
//start_xo_cnt();
//mbus_sleep_all();

// Go to sleep
operation_sleep();
}

//***************************************************
// Initialization
//***************************************************
static void operation_init(void){
    //pmu_sar_conv_ratio_val_test_on = 0x4A;
    pmu_sar_conv_ratio_val_test_on = 50;
    pmu_sar_conv_ratio_val_test_off = 0x2C;//0x2A;
	
	
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    //config_timerwd(TIMERWD_VAL);
    *TIMERWD_GO = 0x0;
    *REG_MBUS_WD = 0;
	
    // EP Mode
    *EP_MODE = 0;
	
    // Set CPU & Mbus Clock Speeds
    prev22e_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prev22e_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prev22e_r0B.CLK_GEN_DIV_CORE = 0x2; // Default 0x3
    prev22e_r0B.GOC_CLK_GEN_SEL_FREQ = 0x0; // Default 0x6
    prev22e_r0B.GOC_CLK_GEN_SEL_DIV = 0x1; // Default 0x0
    prev22e_r0B.GOC_SEL = 0xf; // Default 0x8
	*REG_CLKGEN_TUNE = prev22e_r0B.as_int;
	
    prev22e_r1C.SRAM0_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
    prev22e_r1C.SRAM0_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
    //prev22e_r1C.SRAM0_USE_INVERTER_SA= 0; // Default 0x0, 1bit
    *REG_SRAM0_TUNE = prev22e_r1C.as_int;
	
	// Wakeup if there's pending request
	//prev22e_r1B.WAKEUP_ON_PEND_REQ = 0x1; // Default 0x0
	//*REG_SYS_CONF = prev22e_r1B.as_int;
	
    //Enumerate & Initialize Registers
    enumerated = ENUMID;
    //exec_count = 0;
    //exec_count_irq = 0;
    //PMU_ADC_3P0_VAL = 0x62;
    //pmu_parkinglot_mode = 3;
	
    //Enumeration
    mbus_enumerate(SNT_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(MRR_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(FLP1_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(FLP2_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(ADO_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(PMU_ADDR);
    delay(MBUS_DELAY);
	
    // Set CPU Halt Option as TX --> Use for register write e.g.
    //    set_halt_until_mbus_tx();
	
	
    // PMU Settings ----------------------------------------------
    mbus_remote_register_write(PMU_ADDR,0x51, 
		( (1 << 0) // LC_CLK_RING=1; default 1
			| (3 << 2) // LC_CLK_DIV=3; default 3
			| (0 << 4) // PMU_IRQ_EN=0; default 1
		));
		
		pmu_set_clk_init();
		pmu_reset_solar_short();
		
		// Disable PMU ADC measurement in active mode
		// PMU_CONTROLLER_STALL_ACTIVE
		mbus_remote_register_write(PMU_ADDR,0x3A, 
            ( (1 << 19) // ignore state_horizon; default 1
				| (1 << 13) // ignore adc_output_ready; default 0
				| (1 << 12) // ignore adc_output_ready; default 0
				| (1 << 11) // ignore adc_output_ready; default 0
			));
			delay(MBUS_DELAY);
			pmu_adc_reset_setting();
			delay(MBUS_DELAY);
			pmu_adc_enable();
			delay(MBUS_DELAY);
			
			// Initialize other global variables
			wakeup_data = 0;
			num_total_16b_packet = 200; //total 1000 words transmission
			radio_tx_option = 0; //enables radio tx for each measurement 
			radio_ready = 0;
			radio_on = 0;
			RADIO_PACKET_DELAY = 80;
			//radio_packet_count = 0;
			read_data_batadc_diff = 0;
			
			TEMP_CALIB_A = 240000;
			TEMP_CALIB_B = 3750000;
			NUM_MEAS_USER = TEMP_STORAGE_SIZE;
			
			FLASH_initialization(2); //initialize 2 flash instances
			ado_initialization(); //ADOv7 initialization
			mrr_init();
			XO_init(); //XO initialization
			
			*TIMERWD_GO = 0x0;
			*REG_MBUS_WD = 0;
}

//*******************************************************************
// INTERRUPT HANDLERS 
//*******************************************************************

void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_wakeup(void) { // WAKE-UP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
	uint16_t i;
    delay(MBUS_DELAY);
    // Report who woke up
	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xE4,*SREG_WAKEUP_SOURCE); 
	#endif    
    //[ 0] = GOC/EP
    //[ 1] = Wakeuptimer
    //[ 2] = XO timer
    //[ 3] = gpio_pad
    //[ 4] = mbus message
    //[ 8] = gpio[0]
    //[ 9] = gpio[1]
    //[10] = gpio[2]
    //[11] = gpio[3////]

	if(((*SREG_WAKEUP_SOURCE >> 1) & 1) == 1){ //wake up by wakeup timer
		radio_power_on();
		*NVIC_IPR0 = 0x00000040; //allow for other interrupts to be handled
		radio_data_arr[0] = 0;
		radio_data_arr[1] = 0xFADE;
		for(i=0; i < 10; i++){
			radio_data_arr[2] = i;
			mrr_send_radio_data(0);
			delay(RADIO_PACKET_DELAY);
		}
		radio_power_off();
	
		comp_finish();

		delay(MBUS_DELAY*200); //~2sec
	}   
}
void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    wfi_timeout_flag = 1;
	//set_halt_until_mbus_tx();
}
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);
	#ifdef DEBUG_MBUS_MSG	
		mbus_write_message32(0xE0, *REG0);
	#endif    	    
    if(*REG0 == 1){ // LP -> HP mode change
        *EP_MODE = (*EP_MODE | 1);
		
		adov6vb_r0D.DSP_LP_RESETN = 0;
		mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80A
		
		afe_set_mode(2);
		digital_set_mode(2);
	}
    else if(*REG0 == 0) { // HP -> ULP mode change
        *EP_MODE = (*EP_MODE & 2);
        adov6vb_r0D.DSP_HP_FIFO_GO = 0;
        adov6vb_r0D.DSP_HP_ADO_GO = 0;
        adov6vb_r0D.DSP_HP_DNN_GO = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BCB5
        
        afe_set_mode(1);
        digital_set_mode(1);
        
        operation_sleep_notimer();
	}
	#ifdef DEBUG_MBUS_MSG	
		mbus_write_message32(0xE1, *EP_MODE);
	#endif  		   
}
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1);
}
void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2);
	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xE0, *REG2);
	#endif  
}
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3);
}
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
	
    *TIMERWD_GO = 0x0; //turning off watchdogs
	*REG_MBUS_WD = 0;
	
    if (enumerated != ENUMID) operation_init();
	
    *NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);
	

	if(*SREG_WAKEUP_SOURCE & 1) { //wakeup due to GOC/EP
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xEE,  *GOC_DATA_IRQ);
		#endif    
		wakeup_data = *GOC_DATA_IRQ;
		uint32_t data_cmd = (wakeup_data>>24) & 0xFF;
		uint32_t data_val = wakeup_data & 0xFF;
		uint32_t data_val2 = (wakeup_data>>8) & 0xFF;
		uint32_t data_val3 = (wakeup_data>>16) & 0xFF;
		uint32_t data_val4 = wakeup_data & 0xFFFF;
		uint32_t val16 = wakeup_data & 0xFFFF;
		
		if(data_cmd == 0x1){
			if(data_val==1){   
				xo_start();
			}
			else if(data_val==2){   
				prev22e_r19.XO_RESETn       = 1;
				prev22e_r19.XO_PGb_START_UP = 1;
				*REG_XO_CONF1 = prev22e_r19.as_int;
		
				delay(XO_WAIT_A); // Delay A (~1s; XO Start-Up)
		
				prev22e_r19.XO_ISOL_CLK_HP = 0;
				*REG_XO_CONF1 = prev22e_r19.as_int;
				
				delay(XO_WAIT_B); // Delay B (~1s; VLDO & IBIAS generation)
		
				prev22e_r19.XO_ISOL_CLK_LP = 0;
				*REG_XO_CONF1 = prev22e_r19.as_int;
			}
			else if(data_val==0){
				xo_stop();
			}
			else if(data_val==3){	
				if(data_val2==1){
					prev22e_r19.XO_EN_SLOW = 0x0;
					prev22e_r19.XO_EN_NOM  = 0x1;
					prev22e_r19.XO_EN_FAST = 0x0;
				}
				else if(data_val2==2){
					prev22e_r19.XO_EN_SLOW = 0x1;
					prev22e_r19.XO_EN_NOM  = 0x0;
					prev22e_r19.XO_EN_FAST = 0x0;
				}
				else if(data_val2==3){
					prev22e_r19.XO_EN_SLOW = 0x0;
					prev22e_r19.XO_EN_NOM  = 0x0;
					prev22e_r19.XO_EN_FAST = 0x1;
				}
				*REG_XO_CONF1 = prev22e_r19.as_int;
				delay(100);
	
				prev22e_r19.XO_CAP_TUNE = data_val3 & 0xF; // Additional Cap on OSC_IN & OSC_DRV
				*REG_XO_CONF1 = prev22e_r19.as_int;
			}
			else if(data_val==4){
				start_xo_cout();
			}
			else if(data_val==5){
				stop_xo_cout();
			}
		}
		else if(data_cmd == 0x02){  // SRAM Programming mode
			if(data_val==1){    //Enter
				adov6vb_r0E.DSP_DNN_DBG_MODE_EN = 1;
				mbus_remote_register_write(ADO_ADDR, 0x0E, adov6vb_r0E.as_int);//000054
				adov6vb_r0D.DSP_DNN_CTRL_RF_SEL = 1;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
				adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 1;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
				adov6vb_r0D.DSP_DNN_PG_SLEEP_RF = 0;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B809
				adov6vb_r0D.DSP_DNN_ISOLATEN_RF = 1;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80D
				adov6vb_r0D.DSP_DNN_CLKENB_RF = 0;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B805
				adov6vb_r0D.DSP_DNN_RESETN_RF = 1;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B815
			}
			else{               //Exit
				adov6vb_r0D.DSP_DNN_RESETN_RF = 0;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B805
				adov6vb_r0D.DSP_DNN_CLKENB_RF = 1;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80D
				adov6vb_r0D.DSP_DNN_ISOLATEN_RF = 0;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B809
				adov6vb_r0D.DSP_DNN_PG_SLEEP_RF = 1;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
				adov6vb_r0D.DSP_DNN_CLKENB_RF_SEL = 0;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80B
				adov6vb_r0D.DSP_DNN_CTRL_RF_SEL = 0;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03B80A
				adov6vb_r0E.DSP_DNN_DBG_MODE_EN = 0;
				mbus_remote_register_write(ADO_ADDR, 0x0E, adov6vb_r0E.as_int);//000050
			}
		}
		else if(data_cmd == 0x03){
			pmu_set_sleep_clk((data_val & 0xF),((data_val>>4) & 0xF),(data_val2 & 0x1F),(data_val3 & 0xF));
		}
		else if(data_cmd == 0x04){
			pmu_set_active_clk((data_val & 0xF),((data_val>>4) & 0xF),(data_val2 & 0x1F),(data_val3 & 0xF));
			delay(MBUS_DELAY*300); //~3sec
		}
		else if(data_cmd == 0x05){ //change active PMU settings for v3p6
			pmu_set_active_3p6((data_val & 0xF),((data_val>>4) & 0xF),(data_val2 & 0x1F));		
			delay(MBUS_DELAY*10);
		}		
		else if(data_cmd == 0x06){
			pmu_set_sar_override(data_val);
		}
		else if(data_cmd == 0x07){ //set delay between radio packets
			RADIO_PACKET_DELAY = val16;
		}
		else if(data_cmd == 0x08) {
			mrr_trx_cap_antn_tune_coarse = val16 & 0x3FF;
            // TX Setup Carrier Freq
            mrrv11a_r00.TRX_CAP_ANTP_TUNE_COARSE = mrr_trx_cap_antn_tune_coarse;  //ANT CAP 10b unary 830.5 MHz // TODO: make them the same variable
            mbus_remote_register_write(MRR_ADDR,0x00,mrrv11a_r00.as_int);
            mrrv11a_r01.TRX_CAP_ANTN_TUNE_COARSE = mrr_trx_cap_antn_tune_coarse; //ANT CAP 10b unary 830.5 MHz
            mrrv11a_r01.TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
            mrrv11a_r01.TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
            mbus_remote_register_write(MRR_ADDR,0x01,mrrv11a_r01.as_int);
		}
		else if(data_cmd == 0x09) {
			// SFO trimming
			uint16_t N = data_val2;
			uint16_t M = data_val;
            mrrv11a_r07.RO_MOM = N;
            mrrv11a_r07.RO_MIM = M;
            mbus_remote_register_write(MRR_ADDR,0x07,mrrv11a_r07.as_int);
		}
		else if(data_cmd == 0x0A){  // MIC VIR Cap Tune
			adov6vb_r16.MIC_VIR_CBASE = data_val;  // Default 2'b00
			mbus_remote_register_write(ADO_ADDR, 0x16, adov6vb_r16.as_int);//0535BD
		}
		else if(data_cmd == 0x0B){ //change gain
			adov6vb_r15.REC_PGA_GCON = data_val & 0x1F;
			mbus_remote_register_write(ADO_ADDR, 0x15, adov6vb_r15.as_int);
			delay(MBUS_DELAY*10);
		}		
		else if(data_cmd == 0x0C){
			*EP_MODE = *EP_MODE & 0xFFFC;
			*EP_MODE = *EP_MODE | data_val;
		}
		if(data_cmd == 0x10){       // Charge pump control
			cp_set_mode(data_val);
		}
		else if(data_cmd == 0x11){  // DSP Monitoring Configure
			if(data_val==1){    //Go
				//adov6vb_r0E.DSP_MIX_MON_EN = 1;
				//mbus_remote_register_write(ADO_ADDR, 0x0E, adov6vb_r0E.as_int);//000150
				//adov6vb_r00.DSP_FE_SEL_EXT = 1;
				//mbus_remote_register_write(ADO_ADDR, 0x00, adov6vb_r00.as_int);//9F1B28
				adov6vb_r04.DSP_P2S_MON_EN = 1;
				adov6vb_r04.DSP_CLK_MON_SEL = 4;
				mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//A00040
				adov6vb_r04.DSP_P2S_RESETN = 1;
				mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040
				//adov6vb_r10.DIO_OUT_EN = 1;
				//adov6vb_r10.DIO_IN_EN = 0;
				//adov6vb_r10.DIO_DIR_IN1OUT0 = 0;
				//mbus_remote_register_write(ADO_ADDR, 0x10, adov6vb_r10.as_int);//000008
			}
			else{               //Stop
				adov6vb_r04.DSP_P2S_MON_EN = 0;
				mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//A00040
				adov6vb_r04.DSP_P2S_RESETN = 0;
				mbus_remote_register_write(ADO_ADDR, 0x04, adov6vb_r04.as_int);//E00040
			}
		}
		else if(data_cmd == 0x12){  // DSP LP Control
			if(data_val==1){    //Go
				adov6vb_r0D.DSP_LP_RESETN = 1;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC4A
			}
			else{               //Stop
				adov6vb_r0D.DSP_LP_RESETN = 0;
				mbus_remote_register_write(ADO_ADDR, 0x0D, adov6vb_r0D.as_int);//03BC0A
			}
		}
		else if(data_cmd == 0x13){  // AFE Control
			afe_set_mode(data_val);
			if(data_val == 2) {
				if(data_val2 !=0){
					*TIMERWD_GO = 0x0;
					uint32_t cnt=0;
					while(cnt<data_val2){
						delay(MBUS_DELAY*1000); 
						cnt=cnt+1;
					}
				}
				else delay(MBUS_DELAY*1000); //~10sec, AFE HP test purpose
			}
		}
		else if(data_cmd == 0x14){ //Power gate control
			*REG_CPS = 0x7 & data_val;
			delay(MBUS_DELAY*10);
		}
		else if(data_cmd == 0x15){ //radio packet blaster
			uint32_t i;
			radio_power_on();
			*NVIC_IPR0 = 0x00400000; //allow for other interrupts to be handled
			mbus_write_message32(0xD0,0xABCD1234); 
			for (i = 0; i < data_val4; i++){
				radio_data_arr[2] = 0;
				radio_data_arr[1] = 0x1817;
				radio_data_arr[0] = i;
				mrr_send_radio_data(0);
				delay(RADIO_PACKET_DELAY); //remove?
			}
			mbus_write_message32(0xD2,0x98765432); 	
			radio_data_arr[2] = 0;
			radio_data_arr[1] = 0x1817;
			radio_data_arr[0] = 0x1234;
			mrr_send_radio_data(1);
		}
		else if(data_cmd == 0x16){ //flash erase: data_val = # of instances - 1
			mbus_write_message32(0xDA, 0x23232323);
			flash_erasedata(data_val);
			mbus_write_message32(0xDF, 0x89898989);
		}
		else if(data_cmd == 0x17){ //flash prog A's & 5's: data_val = flash instance, data_val2 = # of 8k banks
			mbus_write_message32(0xDA, 0x12121212);
			flash_erasedata(data_val);
			mbus_write_message32(0xDA, 0x23232323);
			flpv3l_sram_write_all(2,data_val); // write AAAA
			mbus_write_message32(0xDA, 0x34343434);
			FLASH_prog(data_val2, data_val);
			mbus_write_message32(0xDA, 0x45454545);
			flpv3l_sram_write_all(0,data_val); // write 0000
			mbus_write_message32(0xDA, 0x56565656);
			FLASH_test_read(data_val2, data_val);
			mbus_write_message32(0xDA, 0x67676767);
			flash_erasedata(data_val);
			mbus_write_message32(0xDA, 0x78787878);
			flpv3l_sram_write_all(3,data_val); // write 5555
			mbus_write_message32(0xDA, 0x89898989);
			FLASH_prog(data_val2, data_val);
			mbus_write_message32(0xDA, 0x9A9A9A9A);
			flpv3l_sram_write_all(0,data_val); // write 0000
			mbus_write_message32(0xDA, 0xABABABAB);
			FLASH_test_read(data_val2, data_val);
			mbus_write_message32(0xDA, 0xBCBCBCBC);
		}
		else if(data_cmd == 0x18){ //rerun operation_init
			operation_init();
		}
		else if(data_cmd == 0x19){ //go to sleep
			operation_sleep_notimer();	
		}
		else if(data_cmd == 0x1A){ //periodic mbus - test XO dependence
			uint32_t k=0;
			while(k < 100){
				mbus_write_message32(0xAA, k++);
				delay(MBUS_DELAY*10);
			}
		}
		else if(data_cmd == 0x20){ //flash read: data_val = number of 8k banks to read
			mbus_write_message32(0xDA, 0xABABABAB);
			FLASH_read(val16);
			mbus_write_message32(0xDF, 0xEFEFEFEF);
		}
		else if(data_cmd == 0x21){ //radio out # of 8k word banks spec'd by data_val
			radio_power_on();
			*NVIC_IPR0 = 0x00400000; //allow for other interrupts to be handled
			mbus_write_message32(0xDA, 0xABABABAB);
			operation_tx_stored(data_val);
			mbus_write_message32(0xDF, 0xEFEFEFEF);
		}
		else if(data_cmd == 0x22){  // compression test, 16-bit val16 is wakeup timer value
			finst=0;
			comp_stream(val16);
		}
		else if(data_cmd == 0x32){  // compression test, 16-bit val16 is wakeup timer value, uses 2nd flash instance
			finst=1;
			comp_stream2(val16);
		}
		else if(data_cmd == 0x23){ //audio capture w/o voice detection
		}
		else if(data_cmd == 0x24){ //audio capture w/voice detection
		}
		else if(data_cmd == 0x25){ //turn on CP, LP mode then go into selective sleep
			cp_set_mode(1); // ADO CP ON
			afe_set_mode(1); // ADO LP AFE ON
			digital_set_mode(1); // LP DSP ON
			mbus_write_message32(0x01, 0x27CEE000); // selective sleep except ADO and FLP1
		}
		else if(data_cmd == 0x26){ //turn on HP mode, flash streaming
			*NVIC_IPR0 = 0x00400000; //allow for other interrupts to be handled
			delay(300000);
			cp_set_mode(1); // ADO CP ON
			afe_set_mode(2); // ADO HP AFE ON
		
			FLASH_turn_on(data_val);
			FLASH_pp_ready(data_val);
			FLASH_pp_on(data_val);
		
			digital_set_mode(3); // HP Compression only start
		
			delay(300000); // Compression-length control: ~10s
		
			digital_set_mode(0); // ADO DSP ALL OFF
			afe_set_mode(0); // ADO AFE OFF
	
			FLASH_pp_off(data_val);
			FLASH_turn_off(data_val);
			radio_power_on(data_val);
		
			if (data_val == 1) {
				FLASH_read(1);
			}
			else {
				operation_tx_stored(1);
			}
		
			operation_sleep_notimer();
		}
		else if(data_cmd == 0x27){ //turn on CP, LP 
			cp_set_mode(1); // ADO CP ON
			afe_set_mode(1); // ADO LP AFE ON
		}
		else if(data_cmd == 0x30){
			set_wakeup_timer(data_val,1,1);
			mbus_write_message32(0xDA, data_val);
			while(1) {
				mbus_write_message32(0xDA, *REG_WUPT_VAL);
			}
			operation_sleep();
		}
		else if(data_cmd == 0x31){ 
			FLASH_test_read(data_val2, data_val);
		}
	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xEE, 0x00000E2D);
	#endif
	}
	
    // Initialization sequence
    if ((*EP_MODE & 1) == 1) {
		*GOC_DATA_IRQ = 0;					
		*TIMERWD_GO = 0x0; // Turn off CPU watchdog timer
		*REG_MBUS_WD = 0; // Disables Mbus watchdog timer
        while (1); 
        //config_timerwd(TIMERWD_VAL);
	}
    else {
        operation_sleep_notimer();
	}
    return 0;
}
