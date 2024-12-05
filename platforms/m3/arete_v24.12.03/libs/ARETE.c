//*******************************************************************************************
// ARETE SOURCE FILE
//-------------------------------------------------------------------------------------------
// SUB-LAYER CONNECTION:
//-------------------------------------------------------------------------------------------
//      PREv23E -> ADOv1A -> MRMv1L -> PMUv13r1
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//*********************************************************
// HEADER AND OTHER SOURCE FILES
//*********************************************************
#include "ARETE.h"

//*******************************************************************
// ARETE FUNCTIONS
//*******************************************************************
// SEE HEADER FILE FOR FUNCTION BRIEFS


//-------------------------------------------------------------------
// Basic Building Functions
//-------------------------------------------------------------------

uint32_t set_bits (uint32_t var, uint32_t msb_idx, uint32_t lsb_idx, uint32_t value) {
    uint32_t mask_pattern = (0xFFFFFFFF << (msb_idx + 1)) | (0xFFFFFFFF >> (32 - lsb_idx));
    return (var & mask_pattern) | ((value << lsb_idx) & ~mask_pattern);
}

uint32_t set_bit (uint32_t var, uint32_t idx, uint32_t value) {
    if (value & 0x1) return var |  (0x1 << idx);
    else             return var & ~(0x1 << idx);
}

uint32_t get_bits (uint32_t variable, uint32_t msb_idx, uint32_t lsb_idx) {
    return ((variable << (31-msb_idx)) >> (31-msb_idx)) >> lsb_idx;
}

uint32_t get_bit (uint32_t variable, uint32_t idx) {
    return (variable >> idx) & 0x1;
}

uint32_t get_min (uint32_t a, uint32_t b) {
    if (a > b) return b;
    else return a;
}

//*******************************************************************
// PMU FUNCTIONS
//*******************************************************************

void pmu_reg_write_core (uint32_t reg_addr, uint32_t reg_data) {
    enable_reg_irq(PMU_TARGET_REG_IDX);
    mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
    WFI();
}

void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
    pmu_reg_write_core(reg_addr, reg_data);
}

uint32_t pmu_reg_read (uint32_t reg_addr) {
    pmu_reg_write_core(0x00, reg_addr);
    return *PMU_TARGET_REG_ADDR;
}

void pmu_set_sar_ratio (uint32_t ratio) {

    #ifdef DEVEL
        mbus_write_message32(0x94, (__pmu_sar_ratio__<<8)|ratio);
    #endif

    if (ratio != __pmu_sar_ratio__) {

        //---------------------------------------------------------------------------------------
        // SAR_RATIO_OVERRIDE
        //---------------------------------------------------------------------------------------
        pmu_reg_write(0x05, // Default  // Description
        //---------------------------------------------------------------------------------------
            ( (0x3 << 14)   // 0x0      // 0x3: Perform sar_reset followed by sar/upc/dnc_stabilized upon writing into this register
                                        // 0x2: Perform sar_reset upon writing into this register
                                        // 0x1: Perform sar/upc/dnc_stabilized upon writing into this register
                                        // 0x0: Do nothing
            | (0x2 << 12)   // 0x0      // 0x3: Let VDD_CLK always connected to VBAT; 
                                        // 0x2: Let VDD_CLK always connected to V1P2;
                                        // 0x0: Let PMU handle.
            | (0x2 << 10)   // 0x2      // 0x3: Enable the periodic SAR reset; 
                                        // 0x2: Disable the periodic SAR reset; 
                                        // 0x0: Let PMU handle.
                                        // This setting is temporarily ignored when [15] is triggered.
            | (0x2 << 8 )   // 0x0      // 0x3: Let the SAR do UP-conversion (use if VBAT < 1.2V)
                                        // 0x2: Let the SAR do DOWN-conversion (use if VBAT > 1.2V)
                                        // 0x0: Let PMU handle.
            | (0x1 << 7 )   // 0x0      // If 1, override SAR ratio with [6:0].
            | (ratio)       // 0x00     // SAR ratio for overriding (valid only when [7]=1)
        ));

        __pmu_sar_ratio__ = ratio;

        delay(200); // ~10ms with 140kHz CPU clock
    }
}

uint32_t pmu_get_sar_ratio (void) {
    return __pmu_sar_ratio__;
}

uint32_t pmu_read_adc(void) {
    return pmu_reg_read(0x03) & 0xFF;
}

uint32_t pmu_validate_adc_val (uint32_t adc_val) {
    return ((adc_val > __PMU_ADC_LOWER_LIMIT__) && (adc_val < __PMU_ADC_UPPER_LIMIT__));
}

void pmu_set_floor(uint8_t mode, uint8_t r, uint8_t ul, uint8_t sl, uint8_t dl, uint8_t b) {

    // Register Addresses
    uint32_t sar = (mode == PMU_ACTIVE) ? 0x16 : 0x15;
    uint32_t upc = (mode == PMU_ACTIVE) ? 0x18 : 0x17;
    uint32_t dnc = (mode == PMU_ACTIVE) ? 0x1A : 0x19;

    //---------------------------------------------------------------------------------------
    // SAR_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(sar,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0  << 19)    // 1'h0     // Enable PFM even during periodic reset
        | (0  << 18)    // 1'h0     // Enable PFM even when VREF is not used as reference
        | (0  << 17)    // 1'h0     // Enable PFM
        | (1  << 14)    // 3'h3     // Comparator clock division ratio (0x1 being slowest)
        | (0  << 13)    // 1'h0     // Makes the converter clock 2x slower
        | (r  <<  9)    // 4'h8     // Frequency multiplier R
        | (sl <<  5)    // 4'h8     // Frequency multiplier L (actually L+1)
        | (b       )    // 5'h0F    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // UPC_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(upc,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (3  << 14)    // 2'h0     // Desired Vout/Vin ratio
        | (0  << 13)    // 1'h0     // Makes the converter clock 2x slower
        | (r  <<  9)    // 4'h8     // Frequency multiplier R
        | (ul <<  5)    // 4'h4     // Frequency multiplier L (actually L+1)
        | (b       )    // 5'h08    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // DNC_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(dnc,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0  << 13)    // 1'h0     // Makes the converter clock 2x slower
        | (r  <<  9)    // 4'h8     // Frequency multiplier R
        | (dl <<  5)    // 4'h4     // Frequency multiplier L (actually L+1)
        | (b       )    // 5'h08    // Floor frequency base (0-63)
    ));
}

void pmu_active_floor(uint8_t r, uint8_t ul, uint8_t sl, uint8_t dl, uint8_t b) {
    pmu_set_floor (/*mode*/PMU_ACTIVE, /*r*/r, /*ul*/ul, /*sl*/sl, /*dl*/dl, /*b*/b);
}

void pmu_sleep_floor(uint8_t r, uint8_t ul, uint8_t sl, uint8_t dl, uint8_t b) {
    pmu_set_floor (/*mode*/PMU_SLEEP, /*r*/r, /*ul*/ul, /*sl*/sl, /*dl*/dl, /*b*/b);
}

void pmu_set_active_min(){
    pmu_active_floor(/*r*/1, /*ul*/0, /*sl*/0, /*dl*/0, /*b*/1);
}

void pmu_init(uint32_t irq_reg_idx){

    // Set IRQ Destination
    mbus_remote_register_write(PMU_ADDR, 0x52, (0x10 << 8) | irq_reg_idx);

    #ifdef __PMU_CHECK_WRITE__ // See PMU Behavior section at the top)
        //[5]: PMU_CHECK_WRITE(1'h0); [4]: PMU_IRQ_EN(1'h1); [3:2]: LC_CLK_DIV(2'h3); [1:0]: LC_CLK_RING(2'h1)
        mbus_remote_register_write(PMU_ADDR, 0x51, (0x1 << 5) | (0x1 << 4) | (0x3 << 2) | (0x1 << 0));
    #endif

    // Floor Settings
    pmu_sleep_floor (/*r*/6, /*ul*/4, /*sl*/8, /*dl*/4, /*b*/4);
    pmu_active_floor(/*r*/9, /*ul*/6, /*sl*/6, /*dl*/6, /*b*/16);

    // Initialize SAR Ratio
    pmu_set_sar_ratio(76);

    // Disable ADC in Active
    // PMU ADC will be automatically reset when system wakes up
    #ifndef __USE_HCODE__
        //---------------------------------------------------
        // CTRL_DESIRED_STATE_ACTIVE; Disable ADC
        //---------------------------------------------------
        __pmu_desired_state_active__.value              = __PMU_DESIRED_STATE_ACTIVE_DEFAULT__;
        __pmu_desired_state_active__.adc_output_ready   = 0;
        __pmu_desired_state_active__.adc_adjusted       = 0;
        __pmu_desired_state_active__.sar_ratio_adjusted = 0;
        __pmu_desired_state_active__.vbat_read_only     = 0;
        pmu_reg_write(0x3C, __pmu_desired_state_active__.value);

        // Turn off ADC offset measurement in Sleep
        //---------------------------------------------------
        // CTRL_DESIRED_STATE_SLEEP; Use ADC in Flip-Mode
        //---------------------------------------------------
        __pmu_desired_state_sleep__.value              = __PMU_DESIRED_STATE_SLEEP_DEFAULT__;
        __pmu_desired_state_sleep__.adc_output_ready   = 1;
        __pmu_desired_state_sleep__.adc_adjusted       = 0;
        __pmu_desired_state_sleep__.sar_ratio_adjusted = 0;
        __pmu_desired_state_sleep__.vbat_read_only     = 1;
        pmu_reg_write(0x3B, __pmu_desired_state_sleep__.value);
    #else
        pmu_reg_write(0x3C, 0x0017C7FF);
        pmu_reg_write(0x3B, 0x001FCFFF);
    #endif

    // Set ADC Period
    //---------------------------------------------------
    // TICK_REPEAT_VBAT_ADJUST
    //---------------------------------------------------
    pmu_reg_write(0x36, __PMU_ADC_PERIOD__); 

    //---------------------------------------------------
    // TICK_ADC_RESET
    //---------------------------------------------------
    pmu_reg_write(0x33, __PMU_TICK_ADC_RESET__);

    //---------------------------------------------------
    // TICK_ADC_CLK
    //---------------------------------------------------
    pmu_reg_write(0x34, __PMU_TICK_ADC_CLK__);

    //---------------------------------------------------------------------------------------
    // HORIZON_CONFIG
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x45, // Default  // Description
    //---------------------------------------------------------------------------------------
        // NOTE: [14:11] is ignored if the corresponding DESIRED=0 -OR- STALL=1.
        //------------------------------------------------------------------------
        ( (0x1 << 14)   // 0x1      // If 1, 'horizon' enqueues 'wait_clock_count' with TICK_REPEAT_VBAT_ADJUST
        | (0x0 << 13)   // 0x1      // If 1, 'horizon' enqueues 'adjust_adc'
        | (0x0 << 12)   // 0x1      // If 1, 'horizon' enqueues 'adjust_sar_ratio'
        | (0x1 << 11)   // 0x0		// If 1, 'horizon' enqueues 'vbat_read_only'
        //------------------------------------------------------------------------
        | (0x0 << 9 )   // 0x0      // 0x0: Disable clock monitoring
                                    // 0x1: Monitoring SAR clock
                                    // 0x2: Monitoring UPC clock
                                    // 0x3: Monitoring DNC clock
        | (0x0  << 8)   // 0x0      // Reserved
        | (64   << 0)   // 0x48     // Sets ADC_SAMPLING_BIT in 'vbat_read_only' task (0<=N<=255); ADC measures (N/256)xVBAT in Flip mode.
    ));

    // VOLTAGE_CLAMP_TRIM (See PMU VSOLAR SHORT BEHAVIOR section at the top)
    pmu_reg_write(0x0E, (__PMU_VSOLAR_SHORT__ << 8) | (__PMU_VSOLAR_CLAMP_BOTTOM__ << 4) | (__PMU_VSOLAR_CLAMP_TOP__ << 0));

}

//*******************************************************************
// ADO FUNCTIONS
//*******************************************************************

void ado_init(void){

    // Variable initialization
    ado_r00.as_int = ADO_R00_DEFAULT_AS_INT;
    ado_r01.as_int = ADO_R01_DEFAULT_AS_INT;
    ado_r02.as_int = ADO_R02_DEFAULT_AS_INT;
    ado_r03.as_int = ADO_R03_DEFAULT_AS_INT;
    ado_r04.as_int = ADO_R04_DEFAULT_AS_INT;
    ado_r05.as_int = ADO_R05_DEFAULT_AS_INT;
    ado_r06.as_int = ADO_R06_DEFAULT_AS_INT;
    ado_r07.as_int = ADO_R07_DEFAULT_AS_INT;
    ado_r08.as_int = ADO_R08_DEFAULT_AS_INT;
    ado_r09.as_int = ADO_R09_DEFAULT_AS_INT;
    ado_r0A.as_int = ADO_R0A_DEFAULT_AS_INT;
    ado_r0B.as_int = ADO_R0B_DEFAULT_AS_INT;
    ado_r0E.as_int = ADO_R0E_DEFAULT_AS_INT;

    // Adjust layer controller clock frequency
    ado_r0E.LC_CLK_DIV = 2;
    mbus_remote_register_write(ADO_ADDR, 0x0E, ado_r0E.as_int);
	
    // Enable 16k clock going into AFE
    ado_r02.AFE_16K_EN = 1;
    mbus_remote_register_write(ADO_ADDR, 0x02, ado_r02.as_int);

    // CP CLK setting
    ado_r03.CP_CLK_DIV_1P2 = 0; // Default = 2'h3 (1kHz) // 2'h0 (4kHz)
    mbus_remote_register_write(ADO_ADDR, 0x03, ado_r03.as_int);
 
    // Adjust LDO bias current
    ado_r00.LDO_CTRLB_IREF_IBIAS_0P9HP = 15; // PMOS switch, Lowest: 15, Default =4'hA
    ado_r00.LDO_CTRLB_IREF_IBIAS_1P4HP = 15; // PMOS switch, Lowest: 15, Default =4'h9
    mbus_remote_register_write(ADO_ADDR, 0x00, ado_r00.as_int);

    // Adjust LDO 1P4 & 0P9 reference voltage level
    ado_r01.LDO_CTRL_VREFLDO_VOUT_1P4HP = 4; // Default = 4'h6
    ado_r01.LDO_CTRL_VREFLDO_VOUT_0P9HP = 3; // Default = 4'h4
    mbus_remote_register_write(ADO_ADDR, 0x01, ado_r01.as_int);

    // Adjust SAFR clk frequency
    ado_r0B.DIV1 = 15; // Default= 14'h0640
    //ado_r0B.DIV2 = 15; // Default= 14'h064A
    mbus_remote_register_write(ADO_ADDR, 0x0B, ado_r0B.as_int);

    // PGA Setting
    ado_r08.REC_PGA_BWCON = 32; // Default= 9'h0;
    ado_r08.REC_PGA_GCON = 31;   // Default= 5'h1
    mbus_remote_register_write(ADO_ADDR, 0x08, ado_r08.as_int); 

    // PGA feedback cap 
    ado_r06.REC_PGA_CFBADD = 1;   // Default= 1'h0 (10fF); 1'h1 (20fF)
    mbus_remote_register_write(ADO_ADDR, 0x06, ado_r06.as_int); 

    // LGA & PGA bias current setting
    ado_r02.IBC_REC_LNA = 6;  // Default= 5'h3
    ado_r02.IBC_REC_PGA = 11; // Default= 4'h5
    ado_r02.IB_GEN_CLK_EN = 1;
    mbus_remote_register_write(ADO_ADDR, 0x02, ado_r02.as_int);
 
    // LNA bias setting
    ado_r05.REC_LNA_N1_CON = 2; // Default= 6'h7
    ado_r05.REC_LNA_N2_CON = 2; // Default= 6'h7
    ado_r05.REC_LNA_P1_CON = 3; // Default= 6'h8
    ado_r05.REC_LNA_P2_CON = 3; // Default= 6'h8
    mbus_remote_register_write(ADO_ADDR, 0x05, ado_r05.as_int);

    // PGA bias setting
    ado_r07.REC_PGA_P1_CON = 3; // Default= 5'h1
    mbus_remote_register_write(ADO_ADDR, 0x07, ado_r07.as_int);//007064

}

void ado_ldo_set_mode(uint8_t mode){
	if(mode==1){        //ON
		ado_r00.LDO_PG_IREF = 0;
        ado_r00.LDO_PG_VREF_1P4HP = 0;
        ado_r00.LDO_PG_VREF_0P9HP = 0;
		ado_r00.LDO_PG_LDOCORE_1P4HP = 0;
        ado_r00.LDO_PG_LDOCORE_0P9HP = 0;
        mbus_remote_register_write(ADO_ADDR, 0x00, ado_r00.as_int);
		delay(ADO_LDO_DELAY);
	} 
	else {
		ado_r00.LDO_PG_IREF = 1;
        ado_r00.LDO_PG_VREF_1P4HP = 1;
        ado_r00.LDO_PG_VREF_0P9HP = 1;
		ado_r00.LDO_PG_LDOCORE_1P4HP = 1;
        ado_r00.LDO_PG_LDOCORE_0P9HP = 1;
        mbus_remote_register_write(ADO_ADDR, 0x00, ado_r00.as_int);
	}
}

void ado_safr_set_mode(uint8_t mode){
	if(mode==1){        //ON
	// Power-on SAFR 
        ado_r0A.PG_SIG_V1P2_HP = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0A, ado_r0A.as_int);

        ado_r0A.PG_SIG_V1P2_FS = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0A, ado_r0A.as_int);

        delay(ADO_SAFR_DELAY);
        ado_r0A.ISO_1P4_HP = 0;
        ado_r0A.ISO_1P4_FS = 0;
        mbus_remote_register_write(ADO_ADDR, 0x0A, ado_r0A.as_int);

        delay(ADO_SAFR_DELAY);

	// Start SAFR CLK
        ado_r0A.HP_RST = 0;   // Enable slow clk
        ado_r0A.CLK_GATE = 1; // Enable fast clk
        mbus_remote_register_write(ADO_ADDR, 0x0A, ado_r0A.as_int);

	} 
	else {
	// Disable SAFR CLK
        ado_r0A.HP_RST = 1;   
        ado_r0A.CLK_GATE = 0; 
        mbus_remote_register_write(ADO_ADDR, 0x0A, ado_r0A.as_int);

	// Power-off SAFR 
        ado_r0A.PG_SIG_V1P2_HP = 1;
        ado_r0A.PG_SIG_V1P2_FS = 1;
        ado_r0A.ISO_1P4_HP = 1;
        ado_r0A.ISO_1P4_FS = 1;
        mbus_remote_register_write(ADO_ADDR, 0x0A, ado_r0A.as_int);
	}
}

void ado_amp_set_mode(uint8_t mode){
	if(mode==1){        //LNA + PGA ON
	// Enable current bias for buf
        ado_r02.REC_ADCDRI_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x02, ado_r02.as_int);

	// Enable LNA 
        ado_r04.REC_LNA_AMPEN = 1;
        ado_r04.REC_LNA_OUTSHORT_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, ado_r04.as_int);

	// Enable PGA 
        ado_r06.REC_PGA_AMPEN = 1;
        ado_r06.REC_PGA_OUTSHORT_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x06, ado_r06.as_int);

		ado_r0A.HP_F_SEL = 0; // Default= 1'h1
		mbus_remote_register_write(ADO_ADDR, 0x0A, ado_r0A.as_int);

		ado_r0B.DIV1 = 511; // Default= 14'h0640
		//ado_r0B.DIV2 = 15; // Default= 14'h064A
		mbus_remote_register_write(ADO_ADDR, 0x0B, ado_r0B.as_int);
	
	}
	else if(mode==2){	// Turn on LNA only
        ado_r02.REC_ADCDRI_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x02, ado_r02.as_int);

	// Enable LNA 
        ado_r04.REC_LNA_AMPEN = 1;
        ado_r04.REC_LNA_OUTSHORT_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x04, ado_r04.as_int);

		ado_r0A.HP_F_SEL = 0; // Default= 1'h1
		mbus_remote_register_write(ADO_ADDR, 0x0A, ado_r0A.as_int);

		ado_r0B.DIV1 = 511; // Default= 14'h0640
		//ado_r0B.DIV2 = 15; // Default= 14'h064A
		mbus_remote_register_write(ADO_ADDR, 0x0B, ado_r0B.as_int);

	}
	else if(mode==3){	// Turn on PGA only
        ado_r02.REC_ADCDRI_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x02, ado_r02.as_int);

	// Enable PGA 
        ado_r06.REC_PGA_AMPEN = 1;
        ado_r06.REC_PGA_OUTSHORT_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x06, ado_r06.as_int);

		ado_r0A.HP_F_SEL = 0; // Default= 1'h1
		mbus_remote_register_write(ADO_ADDR, 0x0A, ado_r0A.as_int);

		ado_r0B.DIV1 = 511; // Default= 14'h0640
		//ado_r0B.DIV2 = 15; // Default= 14'h064A
		mbus_remote_register_write(ADO_ADDR, 0x0B, ado_r0B.as_int);
	} 
	else {	// Turn off LNA + PGA
		ado_r0A.HP_F_SEL = 1; // Default= 1'h1
		mbus_remote_register_write(ADO_ADDR, 0x0A, ado_r0A.as_int);

		ado_r0B.DIV1 = 15; // Default= 14'h0640
		//ado_r0B.DIV2 = 15; // Default= 14'h064A
		mbus_remote_register_write(ADO_ADDR, 0x0B, ado_r0B.as_int);

	// Disable PGA 
        ado_r06.REC_PGA_AMPEN = 0;
        ado_r06.REC_PGA_OUTSHORT_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x06, ado_r06.as_int);

	// Disable LNA 
        ado_r04.REC_LNA_AMPEN = 0;
        ado_r04.REC_LNA_OUTSHORT_EN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x04, ado_r04.as_int);

	// Disable current bias for buf
        ado_r02.REC_ADCDRI_EN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x02, ado_r02.as_int);
	}
}

void ado_adc_set_mode(uint8_t mode){
	if(mode==1){        //ON
        ado_r09.PG_ADC_CLKGEN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x09, ado_r09.as_int);

		delay(ADO_ADC_DELAY);

        ado_r09.REC_ADC_ISOL_LC = 0;
        mbus_remote_register_write(ADO_ADDR, 0x09, ado_r09.as_int);

        ado_r09.REC_ADC_RESETN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x09, ado_r09.as_int);

	} else {
        ado_r09.REC_ADC_ISOL_LC = 1;
        ado_r09.REC_ADC_RESETN = 0;
        mbus_remote_register_write(ADO_ADDR, 0x09, ado_r09.as_int);

		ado_r09.PG_ADC_CLKGEN = 1;
        mbus_remote_register_write(ADO_ADDR, 0x09, ado_r09.as_int);
	}
}

void ado_cp_set_mode(uint32_t mode){
	if(mode==1){        //ON
   	    ado_r03.CP_CLK_EN_1P2 = 1;
   	    ado_r03.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x03, ado_r03.as_int);
   	    delay(ADO_CP_DELAY); 
   	    ado_r03.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x03, ado_r03.as_int);
   	    delay(ADO_CP_DELAY); 
   	    ado_r03.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x03, ado_r03.as_int);
   	    delay(ADO_CP_DELAY); 
   	    ado_r03.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x03, ado_r03.as_int);
   	    delay(ADO_CP_DELAY); 
   	    ado_r03.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x03, ado_r03.as_int);
   	    delay(ADO_CP_DELAY); 
   	    ado_r03.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x03, ado_r03.as_int);
   	}
   	else if(mode==2){        //ON
   	    ado_r03.CP_CLK_EN_1P2 = 1;
   	    ado_r03.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x03, ado_r03.as_int);
   	    delay(ADO_CP_DELAY*0.2); 
   	    ado_r03.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x03, ado_r03.as_int);
   	}
   	else if(mode == 3){
   	    ado_r03.CP_CLK_EN_1P2 = 1;
   	    ado_r03.CP_VDOWN_1P2 = 0; //vin = V3P6
   	    mbus_remote_register_write(ADO_ADDR, 0x03, ado_r03.as_int);
   	}
   	else if(mode == 4){
   	    ado_r03.CP_CLK_EN_1P2 = 1;
   	    ado_r03.CP_VDOWN_1P2 = 1; //vin = gnd
   	    mbus_remote_register_write(ADO_ADDR, 0x03, ado_r03.as_int);
   	}
   	else{                   //OFF
   	    ado_r03.CP_CLK_EN_1P2 = 0;
   	    ado_r03.CP_CLK_DIV_1P2 = 3;
   	    mbus_remote_register_write(ADO_ADDR, 0x03, ado_r03.as_int);
   	}	
}

void ado_set_rec_pga_gcon (uint32_t rec_pga_gcon) {
    ado_r08.REC_PGA_GCON = rec_pga_gcon & 0x1F;       
    mbus_remote_register_write(ADO_ADDR, 0x08, ado_r08.as_int); 
    delay(ADO_SAFR_DELAY);
}

void ado_set_div1(uint32_t div1) {
    ado_r0B.DIV1 = div1 & 0x1FF; // Default= 14'h0640
    mbus_remote_register_write(ADO_ADDR, 0x0B, ado_r0B.as_int);
}

//*******************************************************************
// MRM FUNCTIONS
//*******************************************************************

void mrm_start_rec (uint32_t offset_sec, uint32_t duration_sec, uint16_t wait_for_irq) {

    uint32_t start_pid = offset_sec << MRM_LOG2_NUM_PAGES_PER_SEC;
    uint32_t num_pages = duration_sec << MRM_LOG2_NUM_PAGES_PER_SEC;

    if (num_pages==0) num_pages = MRM_NUM_PAGES_MRAM;
    mrm_turn_on_ldo();
    mrm_pp_ext_stream (/*bit_en*/0x1, /*num_pages*/num_pages, /*mram_page_id*/start_pid, /*wait_for_irq*/wait_for_irq);
}

void mrm_read (uint32_t offset_sec, uint32_t duration_sec, uint16_t dst_prefix) {

    uint32_t start_pid = offset_sec << MRM_LOG2_NUM_PAGES_PER_SEC;
    uint32_t num_pages = duration_sec << MRM_LOG2_NUM_PAGES_PER_SEC;

    if (num_pages==0) num_pages = MRM_NUM_PAGES_MRAM;

    mrm_turn_on_ldo();

    uint32_t mram_pid = start_pid;
    uint32_t remaining = num_pages;
    while (remaining > NUM_MAX_PAGES_MBUS) {
        mrm_read_mram_page_debug (/*mrm_mram_pid*/mram_pid, /*num_pages*/NUM_MAX_PAGES_MBUS, /*dest_prefix*/dst_prefix);
        mram_pid += NUM_MAX_PAGES_MBUS;
        remaining -= NUM_MAX_PAGES_MBUS;
    }
    if (remaining > 0) {
        mrm_read_mram_page_debug (/*mrm_mram_pid*/mram_pid, /*num_pages*/remaining, /*dest_prefix*/dst_prefix);
    }

    mrm_turn_off_ldo();

}



