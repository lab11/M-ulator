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
//                                                                                                                                                                                                                                                                 
#ifndef ST25DV64K_H
#define ST25DV64K_H
//                                                                                                                                                                                                                                                                 
//      Number of bytes in EEPROM                                                                                                                                                                                                                                  
#define EEPROM_NUM_BYTES    8192
//                                                                                                                                                                                                                                                                 
//                                                                                                                                                                                                                                                                 
//      -------------------------------------------------------------------------------------------
//      EEPROM MEMORY MAP                                                                                                                                                                                                                                          
//      -------------------------------------------------------------------------------------------
//                                                                                                                                                                                                                                                                 
//      -------------------------------------------------------------------------------------------
//      ID SECTION (Byte 0 - 7)                                                                                                                                                                                                                                    
//      -------------------------------------------------------------------------------------------
//                                              Byte         Num                                                                                                                                                                                                   
//      Name                                    Addr       Bytes                Description                                                                                                                                                                        
//      -------------------------------------------------------------------------------------------
#define EEPROM_ADDR_HW_ID                       0      //      4 (  0x58540001) Hardware ID                                                                                                                                                                        
#define EEPROM_ADDR_FW_ID                       4      //      2 (      0xnnnn) Firmware ID                                                                                                                                                                        
//      EEPROM_ADDR_RESERVED_1                  6              2                                                                                                                                                                                                   
//                                                                                                                                                                                                                                                                 
//      -------------------------------------------------------------------------------------------
//      INITIALIZATION SECTION (Byte 8 - 51)                                                                                                                                                                                                                       
//      -------------------------------------------------------------------------------------------
//                                              Byte         Num                                                                                                                                                                                                   
//      Name                                    Addr       Bytes                Description                                                                                                                                                                        
//      -------------------------------------------------------------------------------------------
//                                                                                                                                                                                                                                                                 
#define EEPROM_ADDR_INIT_STATUS                 8      //      2 (      0xF00D) Initialization Status Flag                                                                                                                                                         
//                                                                              0xF00D: App writes 0xF00D if all INITIALIZATION variables are valid. Once read, firmware writes 0x0 in INIT_STATUS.
//                                                                              0xBEEF: App writes 0xBEEF if all INITIALIZATION variables, except TEMP_CALIB_*, are valid. Once read, firmware writes 0x0 in INIT_STATUS.
//                                                                              0xDEAD: Firmware writes 0xDEAD when DEBUG command has been used to store the INITIALIZATION variables back to EEPROM.
//                                                                                                                                                                                                                                                                 
//      ----------------------------------------------------
//       ADC Temperature Region Definition                                                                                                                                                                                                                         
//      ----------------------------------------------------
//       Z = Temperature Measurement (raw code)                                                                                                                                                                                                                    
//                                                                                                                                                                                                                                                                 
//       Temp Measurement        Offset added to ADC threshold values
//      ----------------------------------------------------
//       ADC_T1 < Z              ADC_OFFSET_R1                                                                                                                                                                                                                     
//       ADC_T2 < Z <= ADC_T1    ADC_OFFSET_R2                                                                                                                                                                                                                     
//       ADC_T3 < Z <= ADC_T2    ADC_OFFSET_R3                                                                                                                                                                                                                     
//       ADC_T4 < Z <= ADC_T3    ADC_OFFSET_R4                                                                                                                                                                                                                     
//                Z <= ADC_T4    ADC_OFFSET_R5                                                                                                                                                                                                                     
//      ----------------------------------------------------
//                                                                     Default                                                                                                                                                                                     
#define EEPROM_ADDR_ADC_T1                      10     //      2 (           0) Temperature Threshold "10x(T+80)" for ADC. See the table above.                                                                                                                    
#define EEPROM_ADDR_ADC_T2                      12     //      2 (           0) Temperature Threshold "10x(T+80)" for ADC. See the table above.                                                                                                                    
#define EEPROM_ADDR_ADC_T3                      14     //      2 (           0) Temperature Threshold "10x(T+80)" for ADC. See the table above.                                                                                                                    
#define EEPROM_ADDR_ADC_T4                      16     //      2 (           0) Temperature Threshold "10x(T+80)" for ADC. See the table above.                                                                                                                    
#define EEPROM_ADDR_ADC_OFFSET_R1               18     //      1 (           0) ADC offset. 2's complement. See the table above.                                                                                                                                   
#define EEPROM_ADDR_ADC_OFFSET_R2               19     //      1 (           0) ADC offset. 2's complement. See the table above.                                                                                                                                   
#define EEPROM_ADDR_ADC_OFFSET_R3               20     //      1 (           0) ADC offset. 2's complement. See the table above.                                                                                                                                   
#define EEPROM_ADDR_ADC_OFFSET_R4               21     //      1 (           0) ADC offset. 2's complement. See the table above.                                                                                                                                   
#define EEPROM_ADDR_ADC_OFFSET_R5               22     //      1 (           0) ADC offset. 2's complement. See the table above.                                                                                                                                   
#define EEPROM_ADDR_ADC_LOW_VBAT                23     //      1 (         105) ADC code threshold to turn on the Low VBAT indicator.                                                                                                                              
#define EEPROM_ADDR_ADC_CRIT_VBAT               24     //      1 (          95) ADC code threshold to trigger the EID crash handler.                                                                                                                               
//                                                                                                                                                                                                                                                                 
//      --- EID Configuration                                          Default                                                                                                                                                                                     
#define EEPROM_ADDR_EID_HIGH_TEMP_THRESHOLD     25     //      2 (         950) (=15C) Temperature Threshold "10x(T+80)" to determine HIGH and MID temperature for EID                                                                                             
#define EEPROM_ADDR_EID_LOW_TEMP_THRESHOLD      27     //      2 (         750) (=-5C) Temperature Threshold "10x(T+80)" to determine MID and LOW temperature for EID                                                                                              
#define EEPROM_ADDR_EID_DURATION_HIGH           29     //      2 (          60) (=0.5s) EID duration (ECTR_PULSE_WIDTH) for High Temperature                                                                                                                       
#define EEPROM_ADDR_EID_DURATION_MID            31     //      2 (         500) (=4s) EID duration (ECTR_PULSE_WIDTH) for Mid Temperature                                                                                                                          
#define EEPROM_ADDR_EID_DURATION_LOW            33     //      2 (         500) (=4s) EID duration (ECTR_PULSE_WIDTH) for Low Temperature                                                                                                                          
//      EEPROM_ADDR_RESERVED_2                  35             9                                                                                                                                                                                                   
//                                                                                                                                                                                                                                                                 
//      --- Temperature Calibration                                    Default                                                                                                                                                                                     
#define EEPROM_ADDR_TEMP_CALIB_A_WRITEONLY      44     //      4 (    31.63574) (Write Only) Temperature Calibration Coefficient a                                                                                                                                 
#define EEPROM_ADDR_TEMP_CALIB_B_WRITEONLY      48     //      4 (    5406.431) (Write Only) Temperature Calibration Coefficient b                                                                                                                                 
#define EEPROM_ADDR_HBR_THRESHOLD_RAW_WRITEONLY 52     //      2 (           0) (Write Only) Temperature Threshold for Hibernation (Raw Value)                                                                                                                     
//                                                                                                                                                                                                                                                                 
//      -------------------------------------------------------------------------------------------
//      PRODUCT INFO / CALIB / CRC SECTION (Byte 12 - 43)
//      -------------------------------------------------------------------------------------------
//                                              Byte         Num                                                                                                                                                                                                   
//      Name                                    Addr       Bytes                Description                                                                                                                                                                        
//      -------------------------------------------------------------------------------------------
#define EEPROM_ADDR_PRODUCT_INFO                12     //     18 (            ) Drug Info, Mfg, Batch ID, Exp date                                                                                                                                                 
#define EEPROM_ADDR_HBR_THRESHOLD_RAW           30     //      2 (           0) Temperature Threshold for Hibernation (Raw Value)                                                                                                                                  
#define EEPROM_ADDR_TEMP_CALIB_A                32     //      4 (    31.63574) Temperature Calibration Coefficient a                                                                                                                                              
#define EEPROM_ADDR_TEMP_CALIB_B                36     //      4 (    5406.431) Temperature Calibration Coefficient b                                                                                                                                              
#define EEPROM_ADDR_CRC                         40     //      4 (            ) CRC[31:0]                                                                                                                                                                          
//                                                                                                                                                                                                                                                                 
//      -------------------------------------------------------------------------------------------
//      CONFIG SECTION (Byte 44 - 59)                                                                                                                                                                                                                              
//      -------------------------------------------------------------------------------------------
//                                              Byte         Num                                                                                                                                                                                                   
//      Name                                    Addr       Bytes                Description                                                                                                                                                                        
//      -------------------------------------------------------------------------------------------
#define EEPROM_ADDR_HIGH_TEMP_THRESHOLD         44     //      2 (            ) Threshold for High Temperature "10x(T+80)"                                                                                                                                         
#define EEPROM_ADDR_LOW_TEMP_THRESHOLD          46     //      2 (            ) Threshold for Low Temperature "10x(T+80)"                                                                                                                                          
#define EEPROM_ADDR_TEMP_MEAS_INTERVAL          48     //      2 (            ) Temperature Measurement Interval (unit: minute). 0 is invalid.                                                                                                                     
#define EEPROM_ADDR_TEMP_MEAS_START_DELAY       50     //      2 (            ) Temperature Measurement Start Delay (unit: minute).                                                                                                                                
#define EEPROM_ADDR_NUM_CONS_EXCURSIONS         52     //      2 (            ) Number of consecutive excursions required to trigger an alarm. Valid Range: 1 - 32767                                                                                              
//      EEPROM_ADDR_RESERVED_3                  54             6                                                                                                                                                                                                   
//                                                                                                                                                                                                                                                                 
//      -------------------------------------------------------------------------------------------
//      START SECTION (Byte 60 - 75)                                                                                                                                                                                                                               
//      -------------------------------------------------------------------------------------------
//                                              Byte         Num                                                                                                                                                                                                   
//      Name                                    Addr       Bytes                Description                                                                                                                                                                        
//      -------------------------------------------------------------------------------------------
#define EEPROM_ADDR_START_TIME                  60     //      4 (            ) Start Time (GMT, Epoch: Jan 1, 2022)                                                                                                                                               
#define EEPROM_ADDR_START_USER_ID               64     //      4 (            ) Start User ID                                                                                                                                                                      
//      EEPROM_ADDR_RESERVED_4                  68             8                                                                                                                                                                                                   
//                                                                                                                                                                                                                                                                 
//      -------------------------------------------------------------------------------------------
//      STOP SECTION (Byte 76 - 91)                                                                                                                                                                                                                                
//      -------------------------------------------------------------------------------------------
//                                              Byte         Num                                                                                                                                                                                                   
//      Name                                    Addr       Bytes                Description                                                                                                                                                                        
//      -------------------------------------------------------------------------------------------
#define EEPROM_ADDR_STOP_TIME                   76     //      4 (            ) Stop Time (GMT, Epoch: Jan 1, 2022)                                                                                                                                                
#define EEPROM_ADDR_STOP_USER_ID                80     //      4 (            ) Stop User ID                                                                                                                                                                       
//      EEPROM_ADDR_RESERVED_5                  84             8                                                                                                                                                                                                   
//                                                                                                                                                                                                                                                                 
//      -------------------------------------------------------------------------------------------
//      KEY SECTION (Byte 76 - 91)                                                                                                                                                                                                                                 
//      -------------------------------------------------------------------------------------------
//                                              Byte         Num                                                                                                                                                                                                   
//      Name                                    Addr       Bytes                Description                                                                                                                                                                        
//      -------------------------------------------------------------------------------------------
#define EEPROM_ADDR_AES_KEY                     76     //     16 (            ) AES_KEY[127:0]                                                                                                                                                                     
//                                                                                                                                                                                                                                                                 
//      -------------------------------------------------------------------------------------------
//      HANDSHAKE (Byte 92 - 95)                                                                                                                                                                                                                                   
//      -------------------------------------------------------------------------------------------
//                                              Byte         Num                                                                                                                                                                                                   
//      Name                                    Addr       Bytes                Description                                                                                                                                                                        
//      -------------------------------------------------------------------------------------------
//                                                                                                                                                                                                                                                                 
#define EEPROM_ADDR_CMD                         92     //      1 (            ) Command (REQ/ACK/ERR/CMD)                                                                                                                                                          
//                                                                              [  7] REQ                                                                                                                                                                          
//                                                                              [  6] ACK                                                                                                                                                                          
//                                                                              [  5] ERR                                                                                                                                                                          
//                                                                              [4:0] CMD     0x00: NOP                                                                                                                                                            
//                                                                                            0x01: START                                                                                                                                                          
//                                                                                            0x02: STOP                                                                                                                                                           
//                                                                                            0x03: GETSEC                                                                                                                                                         
//                                                                                            0x04: CONFIG                                                                                                                                                         
//                                                                                            0x07: HARD RESET                                                                                                                                                     
//                                                                                            0x08: NEWKEY                                                                                                                                                         
//                                                                                            0x09: NEWAB                                                                                                                                                          
//                                                                                            0x10: DISPLAY (only for 3.06 or earlier versions)
//                                                                                            0x1E: SOFT RESET                                                                                                                                                     
//                                                                                            0x1F: DEBUG                                                                                                                                                          
//                                                                                                                                                                                                                                                                 
#define EEPROM_ADDR_CMD_PARAM                   93     //      1 (            ) Additional Parameters for CMD                                                                                                                                                      
//                                                                              START command:                                                                                                                                                                     
//                                                                                  [7:2] Reserved                                                                                                                                                                 
//                                                                                  [1] SAMPLE_TYPE     If 1, store 16-bit Raw value.                                                                                                                              
//                                                                                                      If 0, store converted & compressed values.                                                                                                                 
//                                                                                  [0] EN_ROLL_OVER    If 1, it overwrites the oldest meas data                                                                                                                   
//                                                                                                            when the #sample exceeds the max allowed in the EEPROM capacity.                                                                                     
//                                                                                                      If 0, it stops storing temp measurements once the EEPROM is full.                                                                                          
//                                                                                                            However, #sample @ EEPROM keeps increasing.                                                                                                          
//                                                                                                                                                                                                                                                                 
//                                                                              DISPLAY command: (only for 3.06 or earlier versions)                                                                                                                                                                  
//                                                                                  [6] Make 'Minus' black                                                                                                                                                         
//                                                                                  [5] Make 'Plus' black                                                                                                                                                          
//                                                                                  [4] Make 'Slash' black                                                                                                                                                         
//                                                                                  [3] Make 'Back Slash' black                                                                                                                                                    
//                                                                                  [2] Make 'Low Battery' black                                                                                                                                                   
//                                                                                  [1] Make 'Tick' black                                                                                                                                                          
//                                                                                  [0] Make 'Play' black                                                                                                                                                          
//                                                                                                                                                                                                                                                                 
//                                                                              SOFT RESET command:                                                                                                                                                                
//                                                                                  [7:1] Reserved                                                                                                                                                                 
//                                                                                  [0] SKIP_INIT       If 1, it uses previously set init values.                                                                                                                  
//                                                                                                      If 0, it loads init values from EEPROM.                                                                                                                    
//                                                                                                                                                                                                                                                                 
//                                                                              DEBUG command:                                                                                                                                                                     
//                                                                                  0x00: Use the SAR margin specified by EEPROM_ADDR_MISC_CONFIG[6]                                                                                                               
//                                                                                  0x10: Override the SAR margin with 8%                                                                                                                                          
//                                                                                  0x11: Override the SAR margin with 9%                                                                                                                                          
//                                                                                  0x12: Override the SAR margin with 5%                                                                                                                                          
//                                                                                  0x13: Override the SAR margin with 7%                                                                                                                                          
//                                                                                  0x14: Override the SAR margin with 10%                                                                                                                                         
//                                                                                                                                                                                                                                                                 
//                                                                                                                                                                                                                                                                 
//                                                                                                                                                                                                                                                                 
#define EEPROM_ADDR_CMD_CHKSUM                  94     //      1 (            ) Command Checksum                                                                                                                                                                   
//      EEPROM_ADDR_RESERVED_6                                 1                                                                                                                                                                                                   
//                                                                                                                                                                                                                                                                 
#define EEPROM_ADDR_SSLS                        94     //      2 (            ) Seconds Since Last Sample (Updated upon GETSEC command)                                                                                                                            
//                                                                                                                                                                                                                                                                 
//      -------------------------------------------------------------------------------------------
//      SYSTEM STATUS (Byte 96 - 127)                                                                                                                                                                                                                              
//      -------------------------------------------------------------------------------------------
//                                              Byte         Num                                                                                                                                                                                                   
//      Name                                    Addr       Bytes                Description                                                                                                                                                                        
//      -------------------------------------------------------------------------------------------
//                                                                                                                                                                                                                                                                 
//      --- System Status                                                                                                                                                                                                                                          
#define EEPROM_ADDR_SAMPLE_COUNT                96     //      4 (            ) [31:23] Last qword ID (Range: 0 ~ 503)                                                                                                                                             
//                                                                              [22: 0] Sample Count (increments ONLY IF NOT in hibernation)                                                                                                                       
//                                                                                                                                                                                                                                                                 
//      --- Failure Information                                                                                                                                                                                                                                    
#define EEPROM_ADDR_NUM_CALIB_XO_FAILS          100    //      1 (            ) Num Calibration Failures (XO timeout/unstable)                                                                                                                                     
#define EEPROM_ADDR_NUM_CALIB_MAX_ERR_FAILS     101    //      1 (            ) Num Calibration Failures (Max Change Error)                                                                                                                                        
//                                                                                                                                                                                                                                                                 
//                                                                                                                                                                                                                                                                 
//                                                                                                                                                                                                                                                                 
//      --- System State and Display                                                                                                                                                                                                                               
//      ------------------------------------------------------------------
//       System State    Display                     System Behavior
//      ------------------------------------------------------------------
//       XT1_RESET       All segments black          System resets all parameters.
//                                                                                                                                                                                                                                                                 
//       XT1_IDLE        if following XT1_RESET:     System wakes up periodically
//                           Check                   for e-ink refresh.
//                       if following STOP cmd:      Before refreshing the e-ink,
//                           Check                   it measures the temperature
//                           Cross+Plus/Minus        and adjusts PMU/e-ink settings
//                                                   as needed.
//                                                                                                                                                                                                                                                                 
//       XT1_PENDING     Play                        System stays in sleep for
//                                                   the duration of 'Start Delay'
//                                                                                                                                                                                                                                                                 
//       XT1_ACTIVE      Play+Check                  System periodically wakes up
//                       Play+Cross+Plus/Minus       to measure the temperature
//                                                   and do other operations
//                                                   as configured by the user.
//      ------------------------------------------------------------------
//                                                                                                                                                                                                                                                                 
#define EEPROM_ADDR_SYSTEM_STATE                102    //      1 (            ) System State (See comment on EEPROM_ADDR_DISPLAY)                                                                                                                                  
//                                                                                  [  7] Crash handler                                                                                                                                                            
//                                                                                      Becomes 1 when the crash handler is triggered. Need to do HARD-RESET to reset this.                                                                                        
//                                                                                      0x0: Crash handler not yet triggered                                                                                                                                       
//                                                                                      0x1: Crash handler has been triggered due to the critical VBAT level.                                                                                                      
//                                                                                          At this point, the display shows the 'Cross' only.                                                                                                                     
//                                                                                  [  6] Unread Samples                                                                                                                                                           
//                                                                                      Becomes 1 when TMC collects a temperature sample. Becomes 0 when GETSEC is executed.                                                                                       
//                                                                                      0x0: There is no sample collected since the latest GETSEC command.                                                                                                         
//                                                                                      0x1: There are samples collected since the latest GETSEC command.                                                                                                          
//                                                                                  [  5] System Activated                                                                                                                                                         
//                                                                                      Becomes 1 when the very first START is executed. Becomes 0 when HARD- or SOFT-RESET.                                                                                       
//                                                                                      0x0: System has NOT been STARTed since the boot-up or the latest SOFT RESET.                                                                                               
//                                                                                      0x1: System has been STARTed since the boot-up or the latest SOFT RESET.                                                                                                   
//                                                                                  [  4] Sample Type                                                                                                                                                              
//                                                                                      Updated when START is executed (0: NORMAL, 1: RAW). Becomes 0 (NORMAL) when HARD- or SOFT-RESET.                                                                           
//                                                                                      0x0: NORMAL - Converted & compressed values                                                                                                                                
//                                                                                      0x1: RAW - 16-bit Raw values                                                                                                                                               
//                                                                                  [  3] Memory (EEPROM) Full                                                                                                                                                     
//                                                                                      Becomes 1 when the EEPROM gets full. Becomes 0 when HARD- or SOFT-RESET.                                                                                                   
//                                                                                      0x0: Memory is NOT full                                                                                                                                                    
//                                                                                      0x1: Memory is full                                                                                                                                                        
//                                                                                  [  2] Valid Configs                                                                                                                                                            
//                                                                                      Becomes 1 when CONFIG is executed. Becomes 0 when HARD- or SOFT-RESET.                                                                                                     
//                                                                                      0x0: It does NOT have valid user config values                                                                                                                             
//                                                                                      0x1: It has valid user config values                                                                                                                                       
//                                                                                  [1:0] Current System state                                                                                                                                                     
//                                                                                      0x0: System is in XT1_RESET                                                                                                                                                
//                                                                                      0x1: System is in XT1_IDLE                                                                                                                                                 
//                                                                                      0x2: System is in XT1_PEND                                                                                                                                                 
//                                                                                      0x3: System is in XT1_ACTIVE                                                                                                                                               
#define EEPROM_ADDR_DISPLAY                     103    //      1 (            ) Current e-ink display                                                                                                                                                              
//                                                                                  [6] Minus                                                                                                                                                                      
//                                                                                  [5] Plus                                                                                                                                                                       
//                                                                                  [4] Slash                                                                                                                                                                      
//                                                                                  [3] Back Slash                                                                                                                                                                 
//                                                                                  [2] Low Battery                                                                                                                                                                
//                                                                                  [1] Tick                                                                                                                                                                       
//                                                                                  [0] Play                                                                                                                                                                       
//                                                                                  NOTE 1) '[4] Slash' and '[3] Back Slash' make a Cross mark.                                                                                                                    
//                                                                                  NOTE 2) '[4] Slash' and '[1] Tick'       make a Check mark.                                                                                                                    
//                                                                                                                                                                                                                                                                 
//                                                                                                                                                                                                                                                                 
//                                                                                                                                                                                                                                                                 
//                                                                                                                                                                                                                                                                 
//                                                                                                                                                                                                                                                                 
//      --- ADC/SAR Configurations                                                                                                                                                                                                                                 
#define EEPROM_ADDR_LAST_ADC                    104    //      1 (            ) Latest ADC Reading                                                                                                                                                                 
#define EEPROM_ADDR_LAST_SAR                    105    //      1 (            ) Latest SAR Ratio                                                                                                                                                                   
//                                                                                                                                                                                                                                                                 
//      --- Timeout/Failure Information                                                                                                                                                                                                                            
#define EEPROM_ADDR_FAIL                        106    //      1 (            ) Timeout/Failure Information                                                                                                                                                        
//                                                                                  0x1: Timed out during display update (EID)                                                                                                                                     
//                                                                                  0x2: Timed out while reading SNT timer value (WUP)                                                                                                                             
//                                                                                  0x3: Timed out while accessing PMU register (PMU)                                                                                                                              
//                                                                                  0x4: Timed out during temperature measurement (SNT)                                                                                                                            
//                                                                                  0x5: I2C ACK Failure (I2C)                                                                                                                                                     
//                                                                                  0x6: Meta-Stability while reading SNT timer value (MET)                                                                                                                        
//      --- Latest Hibernation Count                                                                                                                                                                                                                               
#define EEPROM_ADDR_MEAS_COUNT                  108    //      4 (            ) Measurement Count (increments regardless of hibernation status)                                                                                                                    
//                                                                                                                                                                                                                                                                 
//      --- Measurement Snapshot (Peak Temperatures)
//      EEPROM_ADDR_HIGHEST_TEMP                106            2                Highest Temperature observed                                                                                                                                                       
//      EEPROM_ADDR_LOWEST_TEMP                 108            2                Lowest Temperature observed                                                                                                                                                        
//                                                                                                                                                                                                                                                                 
//      --- Measurement Snapshot (Sample IDs)                                                                                                                                                                                                                      
//         [16: 0] The first sample ID that exceeds the threshold (NOTE: Sample ID starts at 0)
//         [23:17] Number of samples in the streak. 0 means invalid (i.e., the streak does not exist).
//      EEPROM_ADDR_HIGH_SAMPLE_0               110            3                1st sample number that violates the high threshold                                                                                                                                 
//      EEPROM_ADDR_HIGH_SAMPLE_1               113            3                2nd sample number that violates the high threshold                                                                                                                                 
//      EEPROM_ADDR_HIGH_SAMPLE_2               116            3                3rd sample number that violates the high threshold                                                                                                                                 
//      EEPROM_ADDR_LOW_SAMPLE_0                119            3                1st sample number that violates the low threshold                                                                                                                                  
//      EEPROM_ADDR_LOW_SAMPLE_1                122            3                2nd sample number that violates the low threshold                                                                                                                                  
//      EEPROM_ADDR_LOW_SAMPLE_2                125            3                3rd sample number that violates the low threshold                                                                                                                                  
//      EEPROM_ADDR_MEAN_KINETIC_TEMP                          2                (Reserved) Mean Kinetic Temperature                                                                                                                                                
//                                                                              We may need to remove EEPROM_ADDR_PMU_NUM_CONS_MEAS                                                                                                                                
//                                                                              to get the extra 2 bytes needed for EEPROM_ADDR_MEAN_KINETIC_TEMP)                                                                                                                 
//                                                                                                                                                                                                                                                                 
//      -------------------------------------------------------------------------------------------
//      DATA SECTIOM (Byte 128 - 8191)                                                                                                                                                                                                                             
//      -------------------------------------------------------------------------------------------
//                                                                                                                                                                                                                                                                 
#define EEPROM_ADDR_DATA_RESET_VALUE            128    //   8064 (            ) Temperature Measurements (Max. 4032 samples w/ 16-bit data)                                                                                                                        
//                                                                                                                                                                                                                                                                 
//                                                                                                                                                                                                                                                                 
//      -------------------------------------------------------------------------------------------
//       Static Registers (e2=1)                                                                                                                                                                                                                                   
//      -------------------------------------------------------------------------------------------
//      -------------------------------------------------
//       GPO                                                                                                                                                                                                                                                       
//      -------------------------------------------------
//        Bit     Default Name                Description
//      -------------------------------------------------
//        [7]     0x1     GPO_EN                                                                                                                                                                                                                                   
//        [6]     0x0     RF_WRITE_EN                                                                                                                                                                                                                              
//        [5]     0x0     RF_GET_MSG_EN                                                                                                                                                                                                                            
//        [4]     0x0     RF_PUT_MSG_EN                                                                                                                                                                                                                            
//        [3]     0x1     FIELD_CHANGE_EN                                                                                                                                                                                                                          
//        [2]     0x0     RF_INTERRUPT_EN                                                                                                                                                                                                                          
//        [1]     0x0     RF_ACTIVITY_EN                                                                                                                                                                                                                           
//        [0]     0x0     RF_USER_EN                                                                                                                                                                                                                               
//      -------------------------------------------------
#define ST25DV_ADDR_GPO         0x0000  // e2=1        //        (            )                                                                                                                                                                                    
//      -------------------------------------------------
//       IT_TIME                                                                                                                                                                                                                                                   
//      -------------------------------------------------
//        Bit     Default Name                Description
//      -------------------------------------------------
//        [7:3]   0x0     RFU                                                                                                                                                                                                                                      
//        [2:0]   0x3     IT_TIME             Pulse Duration = 301us - IT_TIME x 37.65us +/- 2us
//      -------------------------------------------------
#define ST25DV_ADDR_IT_TIME     0x0001  // e2=1        //        (            )                                                                                                                                                                                    
//      -------------------------------------------------
//       RF_MNGT                                                                                                                                                                                                                                                   
//      -------------------------------------------------
//        Bit     Default Name                Description
//      -------------------------------------------------
//        [7:2]   0x0     RFU                                                                                                                                                                                                                                      
//        [1]     0x0     RF_SLEEP                                                                                                                                                                                                                                 
//        [0]     0x0     RF_DISABLE                                                                                                                                                                                                                               
//      -------------------------------------------------
#define ST25DV_ADDR_RF_MNGT     0x0003  // e2=1        //        (            )                                                                                                                                                                                    
//      -------------------------------------------------
//       RFA1SS                                                                                                                                                                                                                                                    
//      -------------------------------------------------
//        Bit     Default Name                Description
//      -------------------------------------------------
//        [7:4]   0x0     RFU                                                                                                                                                                                                                                      
//        [3:2]   0x0     RW_PROTECTION_A1                                                                                                                                                                                                                         
//                            00: Area 1 RF access: Read always allowed / Write always allowed
//                            01: Area 1 RF access: Read always allowed / Write allowed if RF user security session is open
//                            10: Area 1 RF access: Read always allowed / Write allowed if RF user security session is open
//                            11: Area 1 RF access: Read always allowed / Write always forbidden
//        [1:0]   0x0     PWD_CTRL_A1                                                                                                                                                                                                                              
//                            00: Area 1 RF user security session cannot be open by password
//                            01: Area 1 RF user security session is open by RF_PWD_1
//                            10: Area 1 RF user security session is open by RF_PWD_2
//                            11: Area 1 RF user security session is open by RF_PWD_3
//      -------------------------------------------------
#define ST25DV_ADDR_RFA1SS     0x0004  // e2=1         //        (            )                                                                                                                                                                                    
//      -------------------------------------------------
//      ENDA1                                                                                                                                                                                                                                                      
//      -------------------------------------------------
//        Bit     Default Name                Description
//      -------------------------------------------------
//        [7:0]   0xFF    ENDA1               End Area 1 = 32 x ENDA1 + 31 when expressed in bytes (I2C)
//      -------------------------------------------------
#define ST25DV_ADDR_ENDA1      0x0005  // e2=1         //        (            )                                                                                                                                                                                    
//      -------------------------------------------------
//       RFA2SS                                                                                                                                                                                                                                                    
//      -------------------------------------------------
//        Bit     Default Name                Description
//      -------------------------------------------------
//        [7:4]   0x0     RFU                                                                                                                                                                                                                                      
//        [3:2]   0x0     RW_PROTECTION_A2                                                                                                                                                                                                                         
//                            00: Area 2 RF access: Read always allowed / Write always allowed
//                            01: Area 2 RF access: Read always allowed / Write allowed if RF user security session is open
//                            10: Area 2 RF access: Read allowed if RF user security session is open / Write allowed if RF user security session is open
//                            11: Area 2 RF access: Read allowed if RF user security session is open / Write always forbidden
//        [1:0]   0x0     PWD_CTRL_A2                                                                                                                                                                                                                              
//                            00: Area 2 RF user security session cannot be open by password
//                            01: Area 2 RF user security session is open by RF_PWD_1
//                            10: Area 2 RF user security session is open by RF_PWD_2
//                            11: Area 2 RF user security session is open by RF_PWD_3
//      -------------------------------------------------
#define ST25DV_ADDR_RFA2SS     0x0006  // e2=1         //        (            )                                                                                                                                                                                    
//      -------------------------------------------------
//      ENDA2                                                                                                                                                                                                                                                      
//      -------------------------------------------------
//        Bit     Default Name                Description
//      -------------------------------------------------
//        [7:0]   0xFF    ENDA2               End Area 2 = 32 x ENDA2 + 31 when expressed in bytes (I2C)
//      -------------------------------------------------
#define ST25DV_ADDR_ENDA2      0x0007  // e2=1         //        (            )                                                                                                                                                                                    
//      -------------------------------------------------
//       RFA3SS                                                                                                                                                                                                                                                    
//      -------------------------------------------------
//        Bit     Default Name                Description
//      -------------------------------------------------
//        [7:4]   0x0     RFU                                                                                                                                                                                                                                      
//        [3:2]   0x0     RW_PROTECTION_A3                                                                                                                                                                                                                         
//                            00: Area 3 RF access: Read always allowed / Write always allowed
//                            01: Area 3 RF access: Read always allowed / Write allowed if RF user security session is open
//                            10: Area 3 RF access: Read allowed if RF user security session is open / Write allowed if RF user security session is open
//                            11: Area 3 RF access: Read allowed if RF user security session is open / Write always forbidden
//        [1:0]   0x0     PWD_CTRL_A3                                                                                                                                                                                                                              
//                            00: Area 3 RF user security session cannot be open by password
//                            01: Area 3 RF user security session is open by RF_PWD_1
//                            10: Area 3 RF user security session is open by RF_PWD_2
//                            11: Area 3 RF user security session is open by RF_PWD_3
//      -------------------------------------------------
#define ST25DV_ADDR_RFA3SS     0x0008  // e2=1         //        (            )                                                                                                                                                                                    
//      -------------------------------------------------
//      ENDA3                                                                                                                                                                                                                                                      
//      -------------------------------------------------
//        Bit     Default Name                Description
//      -------------------------------------------------
//        [7:0]   0xFF    ENDA3               End Area 3 = 32 x ENDA3 + 31 when expressed in bytes (I2C)
//      -------------------------------------------------
#define ST25DV_ADDR_ENDA3      0x0009  // e2=1         //        (            )                                                                                                                                                                                    
//      -------------------------------------------------
//       RFA4SS                                                                                                                                                                                                                                                    
//      -------------------------------------------------
//        Bit     Default Name                Description
//      -------------------------------------------------
//        [7:4]   0x0     RFU                                                                                                                                                                                                                                      
//        [3:2]   0x0     RW_PROTECTION_A4                                                                                                                                                                                                                         
//                            00: Area 4 RF access: Read always allowed / Write always allowed
//                            01: Area 4 RF access: Read always allowed / Write allowed if RF user security session is open
//                            10: Area 4 RF access: Read allowed if RF user security session is open / Write allowed if RF user security session is open
//                            11: Area 4 RF access: Read allowed if RF user security session is open / Write always forbidden
//        [1:0]   0x0     PWD_CTRL_A4                                                                                                                                                                                                                              
//                            00: Area 4 RF user security session cannot be open by password
//                            01: Area 4 RF user security session is open by RF_PWD_1
//                            10: Area 4 RF user security session is open by RF_PWD_2
//                            11: Area 4 RF user security session is open by RF_PWD_3
//      -------------------------------------------------
#define ST25DV_ADDR_RFA4SS     0x000A  // e2=1         //        (            )                                                                                                                                                                                    
//      -------------------------------------                                                                                                                                                                                                                      
//       Dynamic Registers (e2=0)                                                                                                                                                                                                                                  
//      -------------------------------------                                                                                                                                                                                                                      
//      -------------------------------------------------
//       GPO_CTRL_Dyn                                                                                                                                                                                                                                              
//      -------------------------------------------------
//        Bit     Default Name                Description
//      -------------------------------------------------
//        [7]     0x1     GPO_EN                                                                                                                                                                                                                                   
//        [6]     0x0     RF_WRITE_EN         Read-Only
//        [5]     0x0     RF_GET_MSG_EN       Read-Only
//        [4]     0x0     RF_PUT_MSG_EN       Read-Only
//        [3]     0x1     FIELD_CHANGE_EN     Read-Only
//        [2]     0x0     RF_INTERRUPT_EN     Read-Only
//        [1]     0x0     RF_ACTIVITY_EN      Read-Only
//        [0]     0x0     RF_USER_EN          Read-Only
//      -------------------------------------------------
#define ST25DV_ADDR_GPO_CTRL_DYN    0x2000  // e2=0
//      -------------------------------------------------
//       IT_STS_Dyn                                                                                                                                                                                                                                                
//      -------------------------------------------------
//        Bit     Default Name                Description
//      -------------------------------------------------
//        [7]     0x0     RF_WRITE            Read-Only
//        [6]     0x0     RF_GET_MSG          Read-Only
//        [5]     0x0     RF_PUT_MSG          Read-Only
//        [4]     0x1     FIELD_RISING        Read-Only
//        [3]     0x1     FIELD_FALLING       Read-Only
//        [2]     0x0     RF_INTERRUPT        Read-Only
//        [1]     0x0     RF_ACTIVITY         Read-Only
//        [0]     0x0     RF_USER             Read-Only
//      -------------------------------------------------
#define ST25DV_ADDR_IT_STS_DYN      0x2005  // e2=0
//      -----------------------------------------------
//      F_MNGT_DYN                                                                                                                                                                                                                                                 
//      -----------------------------------------------
//      Bit     Default Name                Description
//      -----------------------------------------------
//      [7:2]   0x0     RFU                                                                                                                                                                                                                                        
//      [1]     0x0     RF_SLEEP                                                                                                                                                                                                                                   
//      [0]     0x0     RF_DISABLE                                                                                                                                                                                                                                 
//      -----------------------------------------------
#define ST25DV_ADDR_RF_MNGT_DYN     0x2003  // e2=0
#endif // ST25DV64K_H
