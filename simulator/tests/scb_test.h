//------------------------------------------------------------------------------
// scb_test.h
//
// Branden Ghena (brghena@umich.edu)
// 07/15/2013
//
// Header file for Cortex M0 processor system control block testing
//------------------------------------------------------------------------------

#ifndef _SCB_TEST_H_
#define _SCB_TEST_H_

#include "settings.h"

#define LED_ON   0xAAAA5555
#define LED_OFF  0xF0F0F0F0

#define NMI_DELAY_SIG 0xAAAA1111
#define NMI_ID 0x80000001
#define IRQ_ID 0x80000002

#define NVIC_ISER 0xE000E100
#define NVIC_ICER 0xE000E180
#define NVIC_ISPR 0xE000E200

#define SCB_CPUID 0xE000ED00
#define SCB_AIRCR 0xE000ED0C
#define SCB_SCR   0xE000ED10
#define SCB_CCR   0xE000ED14

#endif /* _SCB_TEST_H_ */

