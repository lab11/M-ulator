//*******************************************************************************************
// TCONV HEADER FILE
//-------------------------------------------------------------------------------------------
//  Temperature Conversion for XT1
//  This implementation is based on Zhiyoong's work: 20220321 - Temperature Conversion.docx
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

#ifndef TCONV_H
#define TCONV_H
#define TCONV

//#define TCONV_DEBUG

#include <stdint.h>
#include <stdbool.h>
#include "mbus.h"

//-------------------------------------------------------------------
// Function: tconv_log2
// Args    : idx - table index (0 - 1023)
// Description:
//           Returns the indexe value from the log2 lookup table
//           idx=1023 returns a dummy value, which is 0.
// Return  : indexed value from the log2 lookup table
//-------------------------------------------------------------------
uint32_t tconv_log2 (uint32_t idx);

//-------------------------------------------------------------------
// Function: div_n11
// Args    : dividend - dividend (fixed-point, N=11)
//           divisor  - divisor  (fixed-point, N=11)
// Description:
//          Peforms a division (dividend / divisor)
//          and returns the result
// Return  : dividend / divisor (fixed-point, N=11)
//-------------------------------------------------------------------
uint32_t div_n11 (uint32_t dividend, uint32_t divisor);


//-------------------------------------------------------------------
// Function: tconv
// Args    : dout   - raw code (integer)
//           a      - Temperature Coefficient a (fixed-point, N=10)
//           b      - Temperature Coefficient b (fixed-point, N=10)
//           offset - Offset to be subtracted from the result (integer, unit: K)
//                    For example, for the low limit 
// Description:
//          Performs the temperature conversion.
//          Returned value is
//              10 x (T + 273) - offset
//              which is an integer, 
//              where T is the actual temperature in celsius degree
// Return  : 10 x (T + 273) - offset (integer)
//-------------------------------------------------------------------
uint32_t tconv (uint32_t dout, uint32_t a, uint32_t b, uint32_t offset);

#endif // TCONV_H
