//*******************************************************************
//Author: Sechang Oh
//Description: Leidos Trestle Gen1 
//          Modified from 'Leidos_dorado_Gen3'
//          v1.0: created   12/14/2018
//*******************************************************************
#include "PREv18.h"
#include "ADOv5V_RF.h"
#include "PMUv7_RF.h"
#include "mbus.h"
 
#define ENUMID 0xDEADBEEF

#include "DFT_LUT.txt"
#include "DCT_PHS.txt"

// uncomment this for debug mbus message
// #define DEBUG_MBUS_MSG
// #define DEBUG_MBUS_MSG_1

#define ADO_ADDR 0x4
#define FLP_ADDR 0x5
#define PMU_ADDR 0x6
#define WAKEUP_PERIOD_PARKING 30000 // About 2 hours (PRCv17)

// System parameters
#define MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)

// CP parameter
#define CP_DELAY 50000 // Amount of delay between successive messages; 100: 6-7ms

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

// GPIO pins
#define GPIO0_IRQ  0
#define GPIO_NNET_IDX0  1
#define GPIO_NNET_IDX1  2
#define GPIO_NNET_IDX2  3
#define GPIO_NNET_IDX3  4
#define GPIO_NNET_IDX4  5

#define EP_MODE     ((volatile uint32_t *) 0xA0000028)

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t irq_history;
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint32_t wakeup_period_count;
volatile uint32_t wakeup_timer_multiplier;
volatile uint32_t PMU_ADC_3P0_VAL;
volatile uint32_t pmu_parkinglot_mode;
volatile uint32_t pmu_sar_conv_ratio_val_test_on;
volatile uint32_t pmu_sar_conv_ratio_val_test_off;
volatile uint32_t read_data_batadc;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile prev18_r0B_t prev18_r0B = PREv18_R0B_DEFAULT;
//volatile prev18_r19_t prev18_r19 = PREv18_R19_DEFAULT;
volatile prev18_r1A_t prev18_r1A = PREv18_R1A_DEFAULT;
volatile prev18_r1C_t prev18_r1C = PREv18_R1C_DEFAULT;

volatile adov5v_r00_t adov5v_r00 = ADOv5V_R00_DEFAULT;
volatile adov5v_r04_t adov5v_r04 = ADOv5V_R04_DEFAULT;
volatile adov5v_r07_t adov5v_r07 = ADOv5V_R07_DEFAULT;
volatile adov5v_r0B_t adov5v_r0B = ADOv5V_R0B_DEFAULT;
volatile adov5v_r0D_t adov5v_r0D = ADOv5V_R0D_DEFAULT;
volatile adov5v_r0E_t adov5v_r0E = ADOv5V_R0E_DEFAULT;
volatile adov5v_r0F_t adov5v_r0F = ADOv5V_R0F_DEFAULT;
volatile adov5v_r10_t adov5v_r10 = ADOv5V_R10_DEFAULT;
volatile adov5v_r11_t adov5v_r11 = ADOv5V_R11_DEFAULT;
volatile adov5v_r12_t adov5v_r12 = ADOv5V_R12_DEFAULT;
volatile adov5v_r13_t adov5v_r13 = ADOv5V_R13_DEFAULT;
volatile adov5v_r14_t adov5v_r14 = ADOv5V_R14_DEFAULT;
volatile adov5v_r15_t adov5v_r15 = ADOv5V_R15_DEFAULT;
volatile adov5v_r16_t adov5v_r16 = ADOv5V_R16_DEFAULT;
volatile adov5v_r17_t adov5v_r17 = ADOv5V_R17_DEFAULT;
volatile adov5v_r18_t adov5v_r18 = ADOv5V_R18_DEFAULT;
volatile adov5v_r19_t adov5v_r19 = ADOv5V_R19_DEFAULT;
volatile adov5v_r1A_t adov5v_r1A = ADOv5V_R1A_DEFAULT;
volatile adov5v_r1B_t adov5v_r1B = ADOv5V_R1B_DEFAULT;
volatile adov5v_r1C_t adov5v_r1C = ADOv5V_R1C_DEFAULT;
        
uint32_t read_data[100];


static void operation_sleep_notimer(void);
static void pmu_set_sar_override(uint32_t val);
static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);
static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2);

//*******************************************************************
// XO Functions
//*******************************************************************

//static void XO_init(void) {
static void XO_init(uint32_t xo_cap_drv, uint32_t xo_cap_in) {
    // Parasitic Capacitance Tuning (6-bit for each; Each 1 adds 1.8pF)
    //uint32_t xo_cap_drv = 0x3F; // Additional Cap on OSC_DRV
    //uint32_t xo_cap_in  = 0x3F; // Additional Cap on OSC_IN
    prev18_r1A.XO_CAP_TUNE = (
            (xo_cap_drv <<6) | 
            (xo_cap_in <<0));   // XO_CLK Output Pad 
    *REG_XO_CONF2 = prev18_r1A.as_int;

    prev18_r19_t prev18_r19 = PREv18_R19_DEFAULT;
    // XO configuration
    prev18_r19.XO_EN_DIV    = 0x1;// divider enable
    prev18_r19.XO_S         = 0x1;// division ratio for 16kHz out
    prev18_r19.XO_SEL_CP_DIV= 0x0;// 1: 0.3V-generation charge-pump uses divided clock
    prev18_r19.XO_EN_OUT    = 0x1;// XO ouput enable
    prev18_r19.XO_PULSE_SEL = 0x4;// pulse width sel, 1-hot code
    prev18_r19.XO_DELAY_EN  = 0x3;// pair usage together with xo_pulse_sel
    // Pseudo-Resistor Selection
    prev18_r19.XO_RP_LOW    = 0x0;
    prev18_r19.XO_RP_MEDIA  = 0x1;
    prev18_r19.XO_RP_MVT    = 0x0;
    prev18_r19.XO_RP_SVT    = 0x0;
 
    prev18_r19.XO_SLEEP = 0x0;
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(100);
    prev18_r19.XO_ISOLATE = 0x0;
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(100);
    prev18_r19.XO_DRV_START_UP  = 0x1;// 1: enables start-up circuit
    *REG_XO_CONF1 = prev18_r19.as_int;
    delay(2000);
    ////after start-up
    //prev18_r19.XO_SCN_CLK_SEL   = 0x1;// scn clock 1: normal. 0.3V level up to 0.6V, 0:init
    //*REG_XO_CONF1 = prev18_r19.as_int;
    //delay(2000);
    //prev18_r19.XO_SCN_CLK_SEL   = 0x0;
    //prev18_r19.XO_SCN_ENB       = 0x0;// enable_bar of scn
    //*REG_XO_CONF1 = prev18_r19.as_int;
    //delay(2000);
    //prev18_r19.XO_DRV_START_UP  = 0x0;
    //prev18_r19.XO_DRV_CORE      = 0x1;// 1: enables core circuit
    //prev18_r19.XO_SCN_CLK_SEL   = 0x1;
    //*REG_XO_CONF1 = prev18_r19.as_int;

    enable_xo_timer();
    start_xo_cout();
}

//***************************************************
// End of Program Sleep Operation
//***************************************************

static void operation_sleep(void){
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;

    // Freeze GPIO
    //freeze_gpio_out();

    // Go to Sleep
    mbus_sleep_all();
    while(1);
}


static void operation_sleep_notimer(void){
    // Make sure the irq counter is reset    
    exec_count_irq = 0;

    //set_wakeup_timer(20,0x1,0x1);
    set_xo_timer(0,1600,1,1);
    start_xo_cnt();
    mbus_sleep_all();
    //// Check if sleep parking lot is on
    //if (pmu_parkinglot_mode & 0x2){
    //    set_wakeup_timer(WAKEUP_PERIOD_PARKING,0x1,0x1);
    //}else{
    //    // Disable Timer
    //    set_wakeup_timer(0, 0, 0);
    //}

    // Go to sleep
    operation_sleep();
}



//***************************************************
// Initialization
//***************************************************
static void operation_init(void){
    pmu_sar_conv_ratio_val_test_on = 0x2E;//0x2D;
    pmu_sar_conv_ratio_val_test_off = 0x2C;//0x2A;

    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);
    *TIMERWD_GO = 0x0;
    *REG_MBUS_WD = 0;

    // EP Mode
    *EP_MODE = 0;

    // Set CPU & Mbus Clock Speeds
    //prev18_r0B.CLK_GEN_RING = 0x1; // Default 0x1, 2bits
    //prev18_r0B.CLK_GEN_DIV_MBC = 0x2; // Default 0x2, 3bits
    //prev18_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3, 3bits
    //prev18_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x1, 2bits
    //prev18_r0B.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x7, 3bits
    //*REG_CLKGEN_TUNE = prev18_r0B.as_int;

    prev18_r1C.SRAM0_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
    prev18_r1C.SRAM0_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
    //prev18_r1C.SRAM0_USE_INVERTER_SA= 1; // Default 0x0, 1bit
    *REG_SRAM0_TUNE = prev18_r1C.as_int;

    //Enumerate & Initialize Registers
    enumerated = ENUMID;
    exec_count = 0;
    exec_count_irq = 0;
    //PMU_ADC_3P0_VAL = 0x62;
    pmu_parkinglot_mode = 3;

    //Enumeration
    mbus_enumerate(ADO_ADDR);
    mbus_enumerate(FLP_ADDR);
    mbus_enumerate(PMU_ADDR);
    delay(MBUS_DELAY);

    // Set CPU Halt Option as TX --> Use for register write e.g.
    //    set_halt_until_mbus_tx();

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 3;   // 0x1E (30): ~1 min with PRCv9
    wakeup_data = 0;

    *REG_CPS = 0x7; //PG control [2]=1: Test 1.2V on,  [0]=1: Test 0.6V on
    delay(MBUS_DELAY*100);
    //init_gpio();

    XO_init(0x3F,0x3F); //XO initialization
    //XO_init(); //XO initialization
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
    delay(MBUS_DELAY);
    // Report who woke up
    mbus_write_message32(0xAA,*SREG_WAKEUP_SOURCE); 
    //[ 0] = GOC/EP
    //[ 1] = Wakeuptimer
    //[ 2] = XO timer
    //[ 3] = gpio_pad
    //[ 4] = mbus message
    //[ 8] = gpio[0]
    //[ 9] = gpio[1]
    //[10] = gpio[2]
    //[11] = gpio[3]
    if(((*SREG_WAKEUP_SOURCE >> 8) & 1) == 1){ //waked up by gpio[0]
        //Do something
        mbus_write_message32(0xAA,0xBBCCDD); 
        delay(MBUS_DELAY*1000); //~10sec
    }
}
void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    wfi_timeout_flag = 1;
}
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);
    mbus_write_message32(0xE0, *REG0);


}
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1);
}
void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2);
}
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3);
}
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
    wakeup_data = *GOC_DATA_IRQ;
    uint32_t data_cmd = (wakeup_data>>24) & 0xFF;
    uint32_t data_val = wakeup_data & 0xFF;
    uint32_t data_val2 = (wakeup_data>>8) & 0xFF;
    //uint32_t data_cmd = *REG1;
    //uint32_t data_val = *REG0;

    //mbus_write_message32(0xEE, data_cmd);
    //mbus_write_message32(0xEE, data_val);

    if(data_cmd == 0x01){       // Charge pump control
        if(data_val==1){        //ON
            adov5v_r14.CP_CLK_EN_1P2 = 1;
            adov5v_r14.CP_CLK_DIV_1P2 = 0;
            mbus_remote_register_write(ADO_ADDR, 0x14, adov5v_r14.as_int);
        }
        else{                   //OFF
            adov5v_r14.CP_CLK_EN_1P2 = 0;
            adov5v_r14.CP_CLK_DIV_1P2 = 3;
            mbus_remote_register_write(ADO_ADDR, 0x14, adov5v_r14.as_int);
        }
    }
    else if(data_cmd == 0x02){  // SRAM Programming mode
        if(data_val==1){    //Enter
            adov5v_r0E.DSP_DNN_DBG_MODE_EN = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0E, adov5v_r0E.as_int);//000054
            adov5v_r0D.DSP_DNN_CTRL_RF_SEL = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80B
            adov5v_r0D.DSP_DNN_PG_SLEEP_RF = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B809
            adov5v_r0D.DSP_DNN_ISOLATEN_RF = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80D
            adov5v_r0D.DSP_DNN_CLKENB_RF = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B805
            adov5v_r0D.DSP_DNN_RESETN_RF = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B815
        }
        else{               //Exit
            adov5v_r0D.DSP_DNN_RESETN_RF = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B805
            adov5v_r0D.DSP_DNN_CLKENB_RF = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80D
            adov5v_r0D.DSP_DNN_ISOLATEN_RF = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B809
            adov5v_r0D.DSP_DNN_PG_SLEEP_RF = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80B
            adov5v_r0D.DSP_DNN_CTRL_RF_SEL = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03B80A
            adov5v_r0E.DSP_DNN_DBG_MODE_EN = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0E, adov5v_r0E.as_int);//000050
        }
    }
    else if(data_cmd == 0x03){  // DSP Monitoring Configure
        //adov5v_r0E.DSP_MIX_MON_EN = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x0E, adov5v_r0E.as_int);//000150
        //adov5v_r00.DSP_FE_SEL_EXT = 1;
        //mbus_remote_register_write(ADO_ADDR, 0x00, adov5v_r00.as_int);//9F1B28
        adov5v_r04.DSP_P2S_MON_EN = 1;
        adov5v_r04.DSP_CLK_MON_SEL = 4;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov5v_r04.as_int);//A00040
        adov5v_r04.DSP_P2S_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x04, adov5v_r04.as_int);//E00040
        //adov5v_r10.DIO_OUT_EN = 1;
        //adov5v_r10.DIO_IN_EN = 0;
        //adov5v_r10.DIO_DIR_IN1OUT0 = 0;
        //mbus_remote_register_write(ADO_ADDR, 0x10, adov5v_r10.as_int);//000008
    }
    else if(data_cmd == 0x04){  // DSP LP Control
        if(data_val==1){    //Go
            adov5v_r0D.DSP_LP_RESETN = 1;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC4A
        }
        else{               //Stop
            adov5v_r0D.DSP_LP_RESETN = 0;
            mbus_remote_register_write(ADO_ADDR, 0x0D, adov5v_r0D.as_int);//03BC0A
        }
    }


    else if(data_cmd == 0x07){  //wake up by GPIO[0] 
        if(data_val==1){    //ON
            mbus_write_message32(0xE7, 0xBEEF01);
            config_gpio_posedge_wirq(1<<GPIO0_IRQ);
        }
        else{    //OFF
            mbus_write_message32(0xE7, 0xBEEF00);
            config_gpio_posedge_wirq(0);
        }
    }

    else if(data_cmd == 0xF0){  //XO cap tune
        XO_init(data_val2,data_val);
        //prev18_r1A.XO_CAP_TUNE = (
        //    (data_val2 <<6) | 
        //    (data_val <<0));   // XO_CLK Output Pad 
        //*REG_XO_CONF2 = prev18_r1A.as_int;
        mbus_write_message32(0xE7, 0xBEEF05);
    }

    //unfreeze_gpio_out();
    //freeze_gpio_out();
    //operation_sleep_notimer();
}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    //init_gpio();
    //unfreeze_gpio_out();


    // Only enable relevant interrupts (PRCv17)
    *NVIC_ISER = (1 << IRQ_WAKEUP | 1 << IRQ_GOCEP | 1 << IRQ_REG0);

    // Initialization sequence
    if (enumerated != ENUMID){
        operation_init();
    }
    
    //mbus_write_message32(0xE2, *EP_MODE);
    //if (*EP_MODE == 1) {
    if (0 == 1) {
        //mbus_write_message32(0xE3, 0xBEEF01);
        while (1) config_timerwd(TIMERWD_VAL);
    }
    else {
        delay(10000);
        mbus_write_message32(0xE4, 0xBEEF02);
        operation_sleep_notimer();
    }
    return 0;
}

