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
//                                          Byte    Num
//      Name                                Addr    Bytes   Description
//-------------------------------------------------------------------------------------------

//-------------------------------------
// System Section (Byte 0 - 127)
//-------------------------------------
//--- System Identification
#define EEPROM_ADDR_HW_ID                   0   //  4       Hardware ID; 0x01000000
#define EEPROM_ADDR_FW_ID                   4   //  4       Firmware ID; 0x58540001

//--- System Status
#define EEPROM_ADDR_SAMPLE_COUNT            8   //  4       Sample Count
#define EEPROM_ADDR_TE_LAST_SAMPLE          12  //  2       Time elapsed since the last sample (unit: minute)
                                                //
                                                //          ------------------------------------------------------------------------------------------------------
                                                //              SAMPLE_COUNT and TE_LAST_SAMPLE
                                                //          ------------------------------------------------------------------------------------------------------
                                                //              Example: TE_LAST_SAMPLE_RES = 5
                                                //                       TEMP_MEAS_INTERVAL = 15
                                                //
                                                //              Time(min)       0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9   0
                                                //              Wakeup          v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v
                                                //              Temp Meas       v                                                           v                
                                                //              SAMPLE_COUNT    n                                                           n+1
                                                //              TE_LAST_SAMPLE  0                   5                   10                  0                   5
                                                //          ------------------------------------------------------------------------------------------------------
                                                //              Example: TE_LAST_SAMPLE_RES = 2
                                                //                       TEMP_MEAS_INTERVAL = 15
                                                //
                                                //              Time(min)       0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9   0
                                                //              Wakeup          v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v
                                                //              Temp Meas       v                                                           v                
                                                //              SAMPLE_COUNT    n                                                           n+1
                                                //              TE_LAST_SAMPLE  0       2       4       6       8       10      12      14  0       2       4   
                                                //          ------------------------------------------------------------------------------------------------------
                                                //
#define EEPROM_ADDR_SYSTEM_STATE            14  //  2       System State (See comment on EEPROM_ADDR_DISPLAY)
                                                //              [   15] Crash handler 
                                                //                          0x0: Crash handler not yet triggered
                                                //                          0x1: Crash handler has been triggered due to the critical VBAT level.
                                                //                                At this point, the display shows the 'Cross' only.
                                                //              [   14] Buffer Overrun
                                                //                          0x0: No buffer overrun.
                                                //                          0x1: Buffer overrun has occurred.
                                                //              [ 1: 0] Current System state (See comment on EEPROM_ADDR_DISPLAY)
                                                //                          0x0: System is in IDLE
                                                //                          0x1: System is in ACTIVATED
                                                //                          0x2: System is in STARTED
                                                //                          0x3: System is in STOPPED
#define EEPROM_ADDR_DISPLAY                 16  //  2       Current e-ink display
                                                //              [6] Minus
                                                //              [5] Plus
                                                //              [4] Slash
                                                //              [3] Back Slash
                                                //              [2] Low Battery
                                                //              [1] Tick
                                                //              [0] Play
                                                //              NOTE 1) '[4] Slash' and '[3] Back Slash' make a Cross mark.
                                                //              NOTE 2) '[4] Slash' and '[1] Tick'       make a Check mark.
                                                //              NOTE 3) System States and Display
                                                //                      - IDLE:
                                                //                          System goes into IDLE state after the initial programming through GOC.
                                                //                          System stays in sleep indefinitely until the user activates it.
                                                //                          All segments become black (DISPLAY = 0x7F)
                                                //                      - ACTIVATED:
                                                //                          System goes into ACTIVATED when the user sends a GOC trigger (0x16002351) while the system is in IDLE.
                                                //                          System wakes up every 1 min and checks whether there is a valid NFC command at EEPROM_ADDR_CMD.
                                                //                          The play sign becomes black (DISPLAY = 0x01)
                                                //                      - STARTED:
                                                //                          System goes into STARTED when it detects the valid START command at EEPROM_ADDR_CMD.
                                                //                          The play sign becomes black at this point (DISPLAY = 0x01) 
                                                //                          System still wakes up every 1 min. 
                                                //                          System does nothing during the 'start delay' given at EEPROM_ADDR_TEMP_MEAS_START_DELAY.
                                                //                          Once the 'start delay' expires, it performs a temperature measurement at every given interval (EEPROM_ADDR_TEMP_MEAS_INTERVAL).
                                                //                          Once it starts measuring the temperatures, it displays one of the following:
                                                //                              Play + Check                (DISPLAY = 0x13)
                                                //                              Play + Cross + Plus         (DISPLAY = 0x39)
                                                //                              Play + Cross + Minus        (DISPLAY = 0x59)
                                                //                              Play + Cross + Plus + Minus (DISPLAY = 0x79)
                                                //                          Additionally, depending on VBAT level, it may also display the 'Low Battery' sign (DISPLY[2])
                                                //                      - STOPPED:
                                                //                          System goes into STOPPED when it detects the valid STOP command at EEPROM_ADDR_CMD.
                                                //                          The play sign (DISPLAY[0]) becomes white. All the other segments remain the same.
                                                //                          System still wakes up every 1 min and checkes whether there is a valid NFC command at EEPROM_ADDR_CMD.
#define EEPROM_ADDR_NUM_CALIB_XO_FAILS      18  //  1       Num Calibration Failures (XO timeout)
#define EEPROM_ADDR_NUM_CALIB_MAX_ERR_FAILS 19  //  1       Num Calibration Failures (Max Change Error)

//--- User Commands
#define EEPROM_ADDR_HIGH_TEMP_THRESHOLD     20  //  2       Threshold for High Temperature (Raw Code)
#define EEPROM_ADDR_LOW_TEMP_THRESHOLD      22  //  2       Threshold for Low Temperature (Raw Code)
#define EEPROM_ADDR_TEMP_MEAS_INTERVAL      24  //  2       Temperature Measurement Interval (unit: minute). 0 is invalid.
#define EEPROM_ADDR_TEMP_MEAS_START_DELAY   26  //  2       Temperature Measurement Start Delay (unit: minute). 0 is invalid.
//#define EEPROM_ADDR_RESERVED              28  //  2       Reserved
#define EEPROM_ADDR_TEMP_MEAS_CONFIG        30  //  2       Misc Configurations
                                                //              [15: 1] Reserved
                                                //              [0]     EN_ROLL_OVER    If 1 (default), it overwrites the oldest meas data when the #sample exceeds the max allowed in the EEPROM capacity.
                                                //                                      If 0, it stops storing temp measurements once the EEPROM is full. However, #sample @ EEPROM keeps increasing.
#define EEPROM_ADDR_CMD                     32  //  1       Command (REQ/ACK/ERR/CMD)
                                                //              [  7] REQ
                                                //              [  6] ACK
                                                //              [  5] ERR
                                                //              [4:0] CMD     0x00: NOP
                                                //                            0x01: START
                                                //                            0x02: STOP
                                                //                            0x10: DISPLAY
                                                //                            0x1E: RESET
#define EEPROM_ADDR_CMD_PARAM               33  //  1       Additional Parameters for CMD
//#define EEPROM_ADDR_RESERVED              34  //  2       Reserved

//--- Start/Stop Information
#define EEPROM_ADDR_START_TIME              36  //  4       Start Time (GMT, Epoch: Jan 1, 2022)
#define EEPROM_ADDR_START_USER_ID           40  //  4       Start User ID
#define EEPROM_ADDR_STOP_TIME               44  //  4       Stop Time (GMT, Epoch: Jan 1, 2022)
#define EEPROM_ADDR_STOP_USER_ID            48  //  4       Stop User ID

//--- Temperature Calibration Coefficients
#define EEPROM_ADDR_TEMP_CALIB_A            52  //  4       Temperature Calibration Coefficient a
#define EEPROM_ADDR_TEMP_CALIB_B            56  //  4       Temperature Calibration Coefficient b

//--- ADC/VBAT Configurations
#define EEPROM_ADDR_VBAT_INFO               60  //  2       VBAT Information
                                                //              [15: 8] Latest SAR Ratio
                                                //              [ 7: 0] Latest VBAT ADC Reading

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
#define EEPROM_ADDR_ADC_T1                  62  //  2       (         2534 ) Temperature Threshold (Raw Code) for ADC. See the table above.
#define EEPROM_ADDR_ADC_T2                  64  //  2       (         2534 ) Temperature Threshold (Raw Code) for ADC. See the table above.
#define EEPROM_ADDR_ADC_T3                  66  //  2       (         2534 ) Temperature Threshold (Raw Code) for ADC. See the table above.
#define EEPROM_ADDR_ADC_T4                  68  //  2       (         2534 ) Temperature Threshold (Raw Code) for ADC. See the table above.
#define EEPROM_ADDR_ADC_OFFSET_R1           70  //  1       (            0 ) ADC offset. 2's complement. See the table above.
#define EEPROM_ADDR_ADC_OFFSET_R2           71  //  1       (            0 ) ADC offset. 2's complement. See the table above.
#define EEPROM_ADDR_ADC_OFFSET_R3           72  //  1       (            0 ) ADC offset. 2's complement. See the table above.
#define EEPROM_ADDR_ADC_OFFSET_R4           73  //  1       (            0 ) ADC offset. 2's complement. See the table above.
#define EEPROM_ADDR_ADC_OFFSET_R5           74  //  1       (            0 ) ADC offset. 2's complement. See the table above.
#define EEPROM_ADDR_ADC_LOW_VBAT            75  //  1       (          105 ) ADC code threshold to turn on the Low VBAT indicator.
#define EEPROM_ADDR_ADC_CRIT_VBAT           76  //  1       (           95 ) ADC code threshold to trigger the EID crash handler.

//--- SNT Timer Configurations
#define EEPROM_ADDR_SNT_CALIB_CONFIG        77  //  1       (          194 ) Configuration for Timer Calibration 
                                                //                              [  7] (Default: 1) Calibration Duration (0: 7.5sec, 1: 15sec)
                                                //                              [6:5] (Default: 2) XO Frequency (0: 4kHz, 1: 8kHz, 2: 16kHz, 3: 32kHz)
                                                //                              [4:3] (Default: 0) Reserved; 
                                                //                              [2:0] (Default: 2) Max Error allowed (# bit shift from SNT_BASE_FREQ)
#define EEPROM_ADDR_SNT_BASE_FREQ           78  //  2       (         1500 ) SNT Timer Base Frequency in Hz (integer value only)

//--- EID Configuration                                     Recommended Value
#define EEPROM_ADDR_EID_HIGH_TEMP_THRESHOLD 80  //  2       ( 2000 (=15C)  ) Temperature Threshold (Raw Code) to determine HIGH and MID temperature for EID
#define EEPROM_ADDR_EID_LOW_TEMP_THRESHOLD  82  //  2       ( 800  (=-5C)  ) Temperature Threshold (Raw Code) to determine MID and LOW temperature for EID
#define EEPROM_ADDR_EID_DURATION_HIGH       84  //  2       ( 60   (=0.5s) ) EID duration (ECTR_PULSE_WIDTH) for High Temperature
#define EEPROM_ADDR_EID_FE_DURATION_HIGH    86  //  2       ( 60   (=0.5s) ) EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for High Temperature. '0' makes it skip 'Field Erase'
#define EEPROM_ADDR_EID_RFRSH_INT_HR_HIGH   88  //  1       ( 12   (=12hr) ) EID Refresh interval for High Temperature (unit: hr). '0' means 'do not refresh'.
#define EEPROM_ADDR_EID_DURATION_MID        89  //  2       ( 500  (=4s)   ) EID duration (ECTR_PULSE_WIDTH) for Mid Temperature
#define EEPROM_ADDR_EID_FE_DURATION_MID     91  //  2       ( 500  (=4s)   ) EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for Mid Temperature. '0' makes it skip 'Field Erase'
#define EEPROM_ADDR_EID_RFRSH_INT_HR_MID    93  //  1       ( 24   (=24hr) ) EID Refresh interval for Mid Temperature (unit: hr). '0' means 'do not refresh'.
#define EEPROM_ADDR_EID_DURATION_LOW        94  //  2       ( 500  (=4s)   ) EID duration (ECTR_PULSE_WIDTH) for Low Temperature
#define EEPROM_ADDR_EID_FE_DURATION_LOW     96  //  2       ( 500  (=4s)   ) EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for Low Temperature. '0' makes it skip 'Field Erase'
#define EEPROM_ADDR_EID_RFRSH_INT_HR_LOW    98  //  1       ( 0    (=none) ) EID Refresh interval for Low Temperature (unit: hr). '0' means 'do not refresh'.
//#define EEPROM_ADDR_RESERVED              99  //  1       Reserved

//--- Other Configuration                                   Recommended Value
#define EEPROM_ADDR_TIMER_CALIB_INTERVAL    100 //  2       (           10 ) Timer Calibration Interval. '0' means 'do not calibrate'. (unit: minute)
#define EEPROM_ADDR_TE_LAST_SAMPLE_RES      102 //  2       (            5 ) Resolution of TE_LAST_SAMPLE (unit: minute). See comment on EEPROM_ADDR_TE_LAST_SAMPLE.

//--- Product Information
#define EEPROM_ADDR_PRODUCT_INFO            104 //  24      Drug Info, Mfg, Batch ID, Exp date


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
