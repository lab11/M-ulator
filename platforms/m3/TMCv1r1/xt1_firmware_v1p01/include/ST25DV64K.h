//*******************************************************************************************
// ST25DV64K HEADER FILE
//-------------------------------------------------------------------------------------------
// EEPROM in ST25DV64K using I2C interface
//-------------------------------------------------------------------------------------------
//  Memory Capacity: 64 kbits = 8kB = 2k words = 2k pages (1 page = 1 word)
//  
//  A Sequential Write can write up to 256 bytes (= 64 words = 64 pages)
//  2k / 64 = 32 Sequential Writes are needed to overwrite the entire EEPROM.
//
//-------------------------------------------------------------------------------------------
// Little Endianness
//-------------------------------------------------------------------------------------------
//
//                   Bit Index                      |                                   EXAMPLE
//                   Definition                     |
//                                                  |                                      
//  Signal                Bit-Index                 |                   EEPROM Data         Hexadecimal         ST NFC App
//  Width                                           |                                       Representation      Representation
//                                                  |
//  1 byte      ByteID    7  6  5  4  3  2  1  0    |   ByteID    7  6  5  4  3  2  1  0
//              ---------------------------------   |   ---------------------------------
//               n        7  6  5  4  3  2  1  0    |    n        0  0  0  1  1  1  0  0    0x1C                1C
//              ---------------------------------   |   ---------------------------------
//                                                  |
//                                                  |
//  2 bytes     ByteID    7  6  5  4  3  2  1  0    |   ByteID    7  6  5  4  3  2  1  0
//              ---------------------------------   |   ---------------------------------
//               n        7  6  5  4  3  2  1  0    |    n        0  0  0  1  1  1  0  0    0xA21C              1C A2
//               n+1     15 14 13 12 11 10  9  8    |    n+1      1  0  1  0  0  0  1  0
//              ---------------------------------   |   ---------------------------------
//                                                  |
//                                                  |
//  3 bytes     ByteID    7  6  5  4  3  2  1  0    |   ByteID    7  6  5  4  3  2  1  0
//              ---------------------------------   |   ---------------------------------
//               n        7  6  5  4  3  2  1  0    |    n        0  0  0  1  1  1  0  0    0xE7A21C            1C A2 E7
//               n+1     15 14 13 12 11 10  9  8    |    n+1      1  0  1  0  0  0  1  0
//               n+2     23 22 21 20 19 18 17 16    |    n+2      1  1  1  0  0  1  1  1
//              ---------------------------------   |   ---------------------------------
//                                                  |
//                                                  |
//  4 bytes     ByteID    7  6  5  4  3  2  1  0    |   ByteID    7  6  5  4  3  2  1  0
//              ---------------------------------   |   ---------------------------------
//               n        7  6  5  4  3  2  1  0    |    n        0  0  0  1  1  1  0  0    0x38E7A21C          1C A2 E7 38
//               n+1     15 14 13 12 11 10  9  8    |    n+1      1  0  1  0  0  0  1  0
//               n+2     23 22 21 20 19 18 17 16    |    n+2      1  1  1  0  0  1  1  1
//               n+3     31 30 29 28 17 16 15 14    |    n+3      0  0  1  1  1  0  0  0
//              ---------------------------------   |   ---------------------------------
//                                                  |
//
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

//-------------------------------------------------------------------------------------------
// SYSTEM CONFIGURATION
//-------------------------------------------------------------------------------------------
//                                          Byte    Num
//      Name                                Addr    Bytes   Description
//-------------------------------------------------------------------------------------------

//--- System Identification
#define EEPROM_ADDR_HW_ID                   0   //  4       Hardware ID; 0x01005843
                                                //              Byte#0: 0x43    - Customer ID (CubeWorks)
                                                //              Byte#1: 0x58    - Model Number (XT)
                                                //              Byte#2: 0x00    - PCB Revision
                                                //              Byte#3: 0x01    - Manufacturing Number
#define EEPROM_ADDR_FW_ID                   4   //  2       Firmware ID

//----------------------------------------------------
//  ADC Temperature Region Definition
//----------------------------------------------------
//  Z = Temperature Measurement (raw code)
//
//  Temp Measurement        Offset added to ADC threshold values
//----------------------------------------------------
//  ADC_T1 < Z              ADC_OFFSET_R1 
//  ADC_T2 < Z <= ADC_T1    ADC_OFFSET_R2
//  ADC_T3 < Z <= ADC_T2    ADC_OFFSET_R3
//  ADC_T4 < Z <= ADC_T3    ADC_OFFSET_R4
//           Z <= ADC_T4    ADC_OFFSET_R5
//----------------------------------------------------
                                                //          Recommended Value          
#define EEPROM_ADDR_ADC_T1                   6  //  2       (         2534 ) Temperature Threshold (Raw Code) for ADC. See the table above.
#define EEPROM_ADDR_ADC_T2                   8  //  2       (         2534 ) Temperature Threshold (Raw Code) for ADC. See the table above.
#define EEPROM_ADDR_ADC_T3                  10  //  2       (         2534 ) Temperature Threshold (Raw Code) for ADC. See the table above.
#define EEPROM_ADDR_ADC_T4                  12  //  2       (         2534 ) Temperature Threshold (Raw Code) for ADC. See the table above.
#define EEPROM_ADDR_ADC_OFFSET_R1           14  //  1       (            0 ) ADC offset. 2's complement. See the table above.
#define EEPROM_ADDR_ADC_OFFSET_R2           15  //  1       (            0 ) ADC offset. 2's complement. See the table above.
#define EEPROM_ADDR_ADC_OFFSET_R3           16  //  1       (            0 ) ADC offset. 2's complement. See the table above.
#define EEPROM_ADDR_ADC_OFFSET_R4           17  //  1       (            0 ) ADC offset. 2's complement. See the table above.
#define EEPROM_ADDR_ADC_OFFSET_R5           18  //  1       (            0 ) ADC offset. 2's complement. See the table above.
#define EEPROM_ADDR_ADC_LOW_VBAT            19  //  1       (          105 ) ADC code threshold to turn on the Low VBAT indicator.
#define EEPROM_ADDR_ADC_CRIT_VBAT           20  //  1       (           95 ) ADC code threshold to trigger the EID crash handler.
//#define EEPROM_ADDR_RESERVED              21  //  1

//--- EID Configuration                                     Recommended Value
#define EEPROM_ADDR_EID_HIGH_TEMP_THRESHOLD 22  //  2       ( 2000 (=15C)  ) Temperature Threshold (Raw Code) to determine HIGH and MID temperature for EID
#define EEPROM_ADDR_EID_LOW_TEMP_THRESHOLD  24  //  2       ( 800  (=-5C)  ) Temperature Threshold (Raw Code) to determine MID and LOW temperature for EID
#define EEPROM_ADDR_EID_DURATION_HIGH       26  //  2       ( 60   (=0.5s) ) EID duration (ECTR_PULSE_WIDTH) for High Temperature
#define EEPROM_ADDR_EID_DURATION_MID        28  //  2       ( 500  (=4s)   ) EID duration (ECTR_PULSE_WIDTH) for Mid Temperature
#define EEPROM_ADDR_EID_DURATION_LOW        30  //  2       ( 500  (=4s)   ) EID duration (ECTR_PULSE_WIDTH) for Low Temperature
#define EEPROM_ADDR_EID_RFRSH_INT_HR_HIGH   32  //  1       ( 12   (=12hr) ) EID Refresh interval for High Temperature (unit: hr). '0' means 'do not refresh'.
#define EEPROM_ADDR_EID_RFRSH_INT_HR_MID    33  //  1       ( 24   (=24hr) ) EID Refresh interval for Mid Temperature (unit: hr). '0' means 'do not refresh'.
#define EEPROM_ADDR_EID_RFRSH_INT_HR_LOW    34  //  1       ( 0    (=none) ) EID Refresh interval for Low Temperature (unit: hr). '0' means 'do not refresh'.

//--- Other Configuration                                   Recommended Value
#define EEPROM_ADDR_PMU_NUM_CONS_MEAS       35  //  1       (            5 ) Number of consecutive measurements required to change SAR ratio or trigger Low VBAT warning/Crash Handler.

//--- Temperature Calibration Coefficients
#define EEPROM_ADDR_TEMP_CALIB_A            36  //  4       Temperature Calibration Coefficient a
#define EEPROM_ADDR_TEMP_CALIB_B            40  //  4       Temperature Calibration Coefficient b


//-------------------------------------------------------------------------------------------
// START INFO AND USER CONFIGURATION
//-------------------------------------------------------------------------------------------
//                                          Byte    Num
//      Name                                Addr    Bytes   Description
//-------------------------------------------------------------------------------------------

//--- Product Information
#define EEPROM_ADDR_PRODUCT_INFO            44  //  24      Drug Info, Mfg, Batch ID, Exp date

//--- User Commands
#define EEPROM_ADDR_HIGH_TEMP_THRESHOLD     68  //  2       Threshold for High Temperature (Raw Code)
#define EEPROM_ADDR_LOW_TEMP_THRESHOLD      70  //  2       Threshold for Low Temperature (Raw Code)
#define EEPROM_ADDR_TEMP_MEAS_INTERVAL      72  //  2       Temperature Measurement Interval (unit: minute). 0 is invalid.
#define EEPROM_ADDR_TEMP_MEAS_START_DELAY   74  //  2       Temperature Measurement Start Delay (unit: minute). 0 is invalid.

//--- Start Information
#define EEPROM_ADDR_START_TIME              76  //  4       Start Time (GMT, Epoch: Jan 1, 2022)
#define EEPROM_ADDR_START_USER_ID           80  //  4       Start User ID


//-------------------------------------------------------------------------------------------
// STOP INFO
//-------------------------------------------------------------------------------------------
//                                          Byte    Num
//      Name                                Addr    Bytes   Description
//-------------------------------------------------------------------------------------------

//--- Stop Information
#define EEPROM_ADDR_STOP_TIME               84  //  4       Stop Time (GMT, Epoch: Jan 1, 2022)
#define EEPROM_ADDR_STOP_USER_ID            88  //  4       Stop User ID


//-------------------------------------------------------------------------------------------
// HANDSHAKE
//-------------------------------------------------------------------------------------------
//                                          Byte    Num
//      Name                                Addr    Bytes   Description
//-------------------------------------------------------------------------------------------

#define EEPROM_ADDR_CMD                     92  //  1       Command (REQ/ACK/ERR/CMD)
                                                //              [  7] REQ
                                                //              [  6] ACK
                                                //              [  5] ERR
                                                //              [4:0] CMD     0x00: NOP
                                                //                            0x01: START
                                                //                            0x02: STOP
                                                //                            0x07: HARD_RESET
                                                //                            0x10: DISPLAY
                                                //                            0x1E: RESET
#define EEPROM_ADDR_CMD_PARAM               93  //  1       Additional Parameters for CMD
                                                //              START command:
                                                //                  [7:1] Reserved
                                                //                  [0]   EN_ROLL_OVER    If 1, it overwrites the oldest meas data 
                                                //                                              when the #sample exceeds the max allowed in the EEPROM capacity.
                                                //                                        If 0, it stops storing temp measurements once the EEPROM is full. 
                                                //                                              However, #sample @ EEPROM keeps increasing.
                                                //              DISPLAY command:
                                                //                  [6] Make 'Minus' black
                                                //                  [5] Make 'Plus' black
                                                //                  [4] Make 'Slash' black
                                                //                  [3] Make 'Back Slash' black
                                                //                  [2] Make 'Low Battery' black
                                                //                  [1] Make 'Tick' black
                                                //                  [0] Make 'Play' black
#define EEPROM_ADDR_CHKSUM                  94  //  2       (Reserved) Checksum


//-------------------------------------------------------------------------------------------
// SYSTEM STATUS
//-------------------------------------------------------------------------------------------
//                                          Byte    Num
//      Name                                Addr    Bytes   Description
//-------------------------------------------------------------------------------------------

//--- System Status
#define EEPROM_ADDR_SAMPLE_COUNT            96  //  4       Sample Count

//--- Failure Information
#define EEPROM_ADDR_NUM_CALIB_XO_FAILS      100 //  1       Num Calibration Failures (XO timeout/unstable)
#define EEPROM_ADDR_NUM_CALIB_MAX_ERR_FAILS 101 //  1       Num Calibration Failures (Max Change Error)


//--- System State and Display
//------------------------------------------------------------------
//  System State    Display                     System Behavior
//------------------------------------------------------------------
//  XT1_RESET       All segments black          System resets all parameters. 
//
//  XT1_IDLE        if following XT1_RESET:     System wakes up periodically
//                      Check                   for e-ink refresh. 
//                  if following STOP cmd:      Before refreshing the e-ink, 
//                      Check                   it measures the temperature 
//                      Cross+Plus/Minus        and adjusts PMU/e-ink settings 
//                                              as needed.
//
//  XT1_PENDING     Play                        System stays in sleep for 
//                                              the duration of 'Start Delay'
//
//  XT1_ACTIVE      Play+Check                  System periodically wakes up 
//                  Play+Cross+Plus/Minus       to measure the temperature
//                                              and do other operations 
//                                              as configured by the user.
//------------------------------------------------------------------

#define EEPROM_ADDR_SYSTEM_STATE            102 //  1       System State (See comment on EEPROM_ADDR_DISPLAY)
                                                //              [  7] Crash handler 
                                                //                      0x0: Crash handler not yet triggered
                                                //                      0x1: Crash handler has been triggered due to the critical VBAT level.
                                                //                          At this point, the display shows the 'Cross' only.
                                                //              [  6] Buffer Overrun
                                                //                      0x0: No buffer overrun.
                                                //                      0x1: Buffer overrun has occurred.
                                                //              [1:0] Current System state
                                                //                      0x0: System is in XT1_RESET
                                                //                      0x1: System is in XT1_IDLE
                                                //                      0x2: System is in XT1_PEND
                                                //                      0x3: System is in XT1_ACTIVE
#define EEPROM_ADDR_DISPLAY                 103 //  1       Current e-ink display
                                                //              [6] Minus
                                                //              [5] Plus
                                                //              [4] Slash
                                                //              [3] Back Slash
                                                //              [2] Low Battery
                                                //              [1] Tick
                                                //              [0] Play
                                                //              NOTE 1) '[4] Slash' and '[3] Back Slash' make a Cross mark.
                                                //              NOTE 2) '[4] Slash' and '[1] Tick'       make a Check mark.

//--- ADC/SAR Configurations
#define EEPROM_ADDR_LAST_ADC                104 //  1       Lastest ADC Reading
#define EEPROM_ADDR_LAST_SAR                105 //  1       Lastest SAR Ratio

//--- Measurement Snapshot (Peak Temperatures)
#define EEPROM_ADDR_HIGHEST_TEMP            106 //  2       Highest Temperature observed
#define EEPROM_ADDR_LOWEST_TEMP             108 //  2       Lowest Temperature observed

//--- Measurement Snapshot (Sample IDs)
//    [16: 0] The first sample ID that exceeds the threshold (NOTE: Sample ID starts at 0)
//    [23:17] Number of samples in the streak. 0 means invalid (i.e., the streak does not exist).
#define EEPROM_ADDR_HIGH_SAMPLE_0           110 //  3       1st streak of samples that violate the high threshold
#define EEPROM_ADDR_HIGH_SAMPLE_1           113 //  3       2nd streak of samples that violate the high threshold
#define EEPROM_ADDR_HIGH_SAMPLE_2           116 //  3       3rd streak of samples that violate the high threshold
#define EEPROM_ADDR_LOW_SAMPLE_0            119 //  3       1st streak of samples that violate the low threshold 
#define EEPROM_ADDR_LOW_SAMPLE_1            122 //  3       2nd streak of samples that violate the low threshold 
#define EEPROM_ADDR_LOW_SAMPLE_2            125 //  3       3rd streak of samples that violate the low threshold 

//--- Measurement Snapshot (Other Stats)
//#define EEPROM_ADDR_MEAN_KINETIC_TEMP     xxx //  2       (Reserved) Mean Kinetic Temperature 
                                                //              We may need to remove EEPROM_ADDR_PMU_NUM_CONS_MEAS 
                                                //              to get the extra 2 bytes needed for EEPROM_ADDR_MEAN_KINETIC_TEMP)

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
