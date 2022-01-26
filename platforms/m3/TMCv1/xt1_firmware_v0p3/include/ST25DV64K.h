//*******************************************************************************************
// ST25DV64K HEADER FILE
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Jan 05 2022 -   First commit 
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

#ifndef ST25DV64K_H
#define ST25DV64K_H

// Number of bytes in EEPROM
#define EEPROM_NUM_BYTES    8192

//-------------------------------------------------------------------------------------------
// EEPROM MEMORY MAP
//-------------------------------------------------------------------------------------------
//                                          Byte    Num
//      Name                                Addr    Bytes   Description
//-------------------------------------------------------------------------------------------

//-------------------------------------
// System Section (Byte 0 - 127)
//-------------------------------------
//--- System Identification
#define EEPROM_ADDR_HW_ID                   0   //  4       Hardware ID
#define EEPROM_ADDR_FW_ID                   4   //  4       Firmware ID

//--- System Status
#define EEPROM_ADDR_SAMPLE_COUNT            8   //  4       Sample Count (Little Endian)
#define EEPROM_ADDR_SYSTEM_STATE            12  //  2       System State
#define EEPROM_ADDR_DISPLAY                 14  //  2       Current e-ink display
#define EEPROM_ADDR_NUM_CALIB_XO_FAILS      16  //  1       Num Calibration Failures (XO does not start)
#define EEPROM_ADDR_NUM_CALIB_MAX_ERR_FAILS 17  //  1       Num Calibration Failures (Max Change Error)
#define EEPROM_ADDR_NUM_CALIB_TIMEOUT_FAILS 18  //  1       Num Calibration Failures (~45sec Timeout; XO frequency is much slower than expected.)
#define EEPROM_ADDR_NUM_I2C_FAILS           19  //  1       Num I2C Failures (Ack timeout)

//--- User Commands
#define EEPROM_ADDR_HIGH_TEMP_THRESHOLD     20  //  2       Threshold for High Temperature (Raw Code)
#define EEPROM_ADDR_LOW_TEMP_THRESHOLD      22  //  2       Threshold for Low Temperature (Raw Code)
#define EEPROM_ADDR_TEMP_MEAS_INTERVAL      24  //  2       Temperature Measurement Interval (unit: minute)
#define EEPROM_ADDR_TEMP_MEAS_START_DELAY   26  //  2       Temperature Measurement Start Delay (unit: minute)
#define EEPROM_ADDR_TIMER_CALIB_INTERVAL    28  //  2       Timer Calibration Interval (unit: minute)
//#define EEPROM_ADDR_RESERVED              30  //  2       Reserved
#define EEPROM_ADDR_CMD                     32  //  1       Command (REQ/ACK/ERR/CMD)
//#define EEPROM_ADDR_RESERVED              33  //  3       Reserved

//--- Start/Stop Information
#define EEPROM_ADDR_START_TIME              36  //  4       Start Time (GMT, Epoch: Jan 1, 2022)
#define EEPROM_ADDR_START_USER_ID           40  //  4       Start User ID
#define EEPROM_ADDR_STOP_TIME               44  //  4       Stop Time (GMT, Epoch: Jan 1, 2022)
#define EEPROM_ADDR_STOP_USER_ID            48  //  4       Stop User ID

//--- Calibration Data
#define EEPROM_ADDR_VBAT_INFO               52  //  2       VBAT + calibration data
#define EEPROM_ADDR_LOW_VBAT_THRESHOLD      54  //  1       Low VBAT threshold (PMU ADC output) that turns on the VBAT indicator on the display
#define EEPROM_ADDR_CRIT_VBAT_THRESHOLD     55  //  1       Critical VBAT threshold (PMU ADC output) that shuts down (and freezes) the system
#define EEPROM_ADDR_SAR_RATIO               56  //  1       SAR RATIO to use
//#define EEPROM_ADDR_RESERVED              57  //  3       Reserved
#define EEPROM_ADDR_TEMP_CALIB_A            60  //  4       Temperature Calibration Coefficient a
#define EEPROM_ADDR_TEMP_CALIB_B            64  //  4       Temperature Calibration Coefficient b
#define EEPROM_ADDR_SNT_BASE_FREQ           68  //  2       SNT Timer Base Frequency in Hz (integer value only)
#define EEPROM_ADDR_SNT_CALIB_CONFIG        70  //  1       [7]: Calibration Duration (0: 7.5sec, 1: 15sec); [6:5]: XO Frequency (0: 4kHz, 1: 8kHz, 2: 16kHz, 3: 32kHz); [4:3]: Reserved; [2:0]: Max Error allowed (# bit shift from SNT_BASE_FREQ)
//#define EEPROM_ADDR_RESERVED              71  //  1       Reserved

//--- EID Configuration                                     Recommended Value
#define EEPROM_ADDR_EID_HIGH_TEMP_THRESHOLD 72  //  2       ( 1250 (=20C)  ) Temperature Threshold (Raw Code) to determin HIGH and MID temperature
#define EEPROM_ADDR_EID_LOW_TEMP_THRESHOLD  74  //  2       ( 700  (=10C)  ) Temperature Threshold (Raw Code) to determin MID and LOW temperature
#define EEPROM_ADDR_EID_DURATION_HIGH       76  //  2       ( 30   (=0.25s)) EID duration (ECTR_PULSE_WIDTH) for High Temperature
#define EEPROM_ADDR_EID_FE_DURATION_HIGH    78  //  2       ( 15   (=0.12s)) EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for High Temperature. '0' makes it skip 'Field Erase'
#define EEPROM_ADDR_EID_RFRSH_INT_HR_HIGH   80  //  1       ( 2    (=2hr)  ) EID Refresh interval for High Temperature (unit: hr). '0' makes it skip the refresh.
//#define EEPROM_ADDR_RESERVED              81  //  1       Reserved
#define EEPROM_ADDR_EID_DURATION_MID        82  //  2       ( 250  (=2s)   ) EID duration (ECTR_PULSE_WIDTH) for Mid Temperature
#define EEPROM_ADDR_EID_FE_DURATION_MID     84  //  2       ( 125  (=1s)   ) EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for Mid Temperature. '0' makes it skip 'Field Erase'
#define EEPROM_ADDR_EID_RFRSH_INT_HR_MID    86  //  1       ( 4    (=4hr)  ) EID Refresh interval for Mid Temperature (unit: hr). '0' makes it skip the refresh.
//#define EEPROM_ADDR_RESERVED              87  //  1       Reserved
#define EEPROM_ADDR_EID_DURATION_LOW        88  //  2       ( 500  (=4s)   ) EID duration (ECTR_PULSE_WIDTH) for Low Temperature
#define EEPROM_ADDR_EID_FE_DURATION_LOW     90  //  2       ( 500  (=4s)   ) EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for Low Temperature. '0' makes it skip 'Field Erase'
#define EEPROM_ADDR_EID_RFRSH_INT_HR_LOW    92  //  1       ( 8    (=8hr)  ) EID Refresh interval for Low Temperature (unit: hr). '0' makes it skip the refresh.
//#define EEPROM_ADDR_RESERVED              93  //  3       Reserved

//--- Reserved
//#define EEPROM_ADDR_RESERVED              96  //  4       Reserved

//--- Product Information
#define EEPROM_ADDR_PRODUCT_INFO            100 //  28      Drug Info, Mfg, Batch ID, Exp date


//-------------------------------------
// Data Section (Byte 128 - 8191)
//-------------------------------------
#define EEPROM_ADDR_DATA_RESET_VALUE        128 //  8064    Temperature Measurements (Max. 4032 samples w/ 16-bit data)


//-------------------------------------------------------------------------------------------
// Static Registers (e2=1)
//-------------------------------------------------------------------------------------------

//-------------------------------------------------
// GPO 
//-------------------------------------------------
//  Bit     Default Name                Description
//-------------------------------------------------
//  [7]     0x1     GPO_EN 
//  [6]     0x0     RF_WRITE_EN 
//  [5]     0x0     RF_GET_MSG_EN 
//  [4]     0x0     RF_PUT_MSG_EN 
//  [3]     0x1     FIELD_CHANGE_EN 
//  [2]     0x0     RF_INTERRUPT_EN 
//  [1]     0x0     RF_ACTIVITY_EN 
//  [0]     0x0     RF_USER_EN
//-------------------------------------------------
#define ST25DV_ADDR_GPO         0x0000  // e2=1

//-------------------------------------------------
// IT_TIME 
//-------------------------------------------------
//  Bit     Default Name                Description
//-------------------------------------------------
//  [7:3]   0x0     RFU
//  [2:0]   0x3     IT_TIME             Pulse Duration = 301us - IT_TIME x 37.65us +/- 2us
//-------------------------------------------------
#define ST25DV_ADDR_IT_TIME     0x0001  // e2=1

//-------------------------------------------------
// RF_MNGT
//-------------------------------------------------
//  Bit     Default Name                Description
//-------------------------------------------------
//  [7:2]   0x0     RFU
//  [1]     0x0     RF_SLEEP            
//  [0]     0x0     RF_DISABLE            
//-------------------------------------------------
#define ST25DV_ADDR_RF_MNGT     0x0003  // e2=1


//-------------------------------------
// Dynamic Registers (e2=0)
//-------------------------------------

//-------------------------------------------------
// GPO_CTRL_Dyn
//-------------------------------------------------
//  Bit     Default Name                Description
//-------------------------------------------------
//  [7]     0x1     GPO_EN 
//  [6]     0x0     RF_WRITE_EN         Read-Only
//  [5]     0x0     RF_GET_MSG_EN       Read-Only
//  [4]     0x0     RF_PUT_MSG_EN       Read-Only
//  [3]     0x1     FIELD_CHANGE_EN     Read-Only
//  [2]     0x0     RF_INTERRUPT_EN     Read-Only
//  [1]     0x0     RF_ACTIVITY_EN      Read-Only
//  [0]     0x0     RF_USER_EN          Read-Only
//-------------------------------------------------
#define ST25DV_ADDR_GPO_CTRL_DYN    0x2000  // e2=0

//-------------------------------------------------
// IT_STS_Dyn
//-------------------------------------------------
//  Bit     Default Name                Description
//-------------------------------------------------
//  [7]     0x0     RF_WRITE            Read-Only
//  [6]     0x0     RF_GET_MSG          Read-Only
//  [5]     0x0     RF_PUT_MSG          Read-Only
//  [4]     0x1     FIELD_RISING        Read-Only
//  [3]     0x1     FIELD_FALLING       Read-Only
//  [2]     0x0     RF_INTERRUPT        Read-Only
//  [1]     0x0     RF_ACTIVITY         Read-Only
//  [0]     0x0     RF_USER             Read-Only
//-------------------------------------------------
#define ST25DV_ADDR_IT_STS_DYN      0x2005  // e2=0      

//-------------------------------------------------
// RF_MNGT_DYN
//-------------------------------------------------
//  Bit     Default Name                Description
//-------------------------------------------------
//  [7:2]   0x0     RFU
//  [1]     0x0     RF_SLEEP            
//  [0]     0x0     RF_DISABLE            
//-------------------------------------------------
#define ST25DV_ADDR_RF_MNGT_DYN     0x2003  // e2=0


#endif // ST25DV64K_H
