//------------------------------------------------------------------------------
// irq_test.h
//
// Branden Ghena (brghena@umich.edu)
// 07/19/2013
//
// Header file for Cortex M0 processor interrupt testing
//------------------------------------------------------------------------------

#ifndef _IRQ_TEST_H_
#define _IRQ_TEST_H_

#include "settings.h"

// Defines
#define LED_ON   0xAAAA5555
#define LED_OFF  0xF0F0F0F0
#define NMI_SIG  0xAAAA0000
#define NMI_DELAY_SIG 0xAAAA1111
#define IRQ_SIG  0xAAAAFFFF
#define TOP_OF_RAM 0x1FFCU

#define NVIC_ISER 0xE000E100
#define NVIC_ICER 0xE000E180
#define NVIC_ISPR 0xE000E200
#define NVIC_ICPR 0xE000E280
#define NVIC_IPR0 0xE000E400
#define NVIC_IPR1 0xE000E404
#define NVIC_IPR2 0xE000E408
#define NVIC_IPR3 0xE000E40C
#define SCB_ICSR  0xE000ED04
#define SCB_SHPR2 0xE000ED1C
#define SCB_SHPR3 0xE000ED20
#define NMI_ID 0x80000001
#define IRQ_ID 0x80000002
#define SVC_ID 0x80000003

// Function Prototypes
int main(void);
void ledLoop(unsigned int);

// Interrupt Handlers
void nmi_handler(void);
void svcall_handler(void);
void pendsv_handler(void);
void irq0_handler(void);
void irq1_handler(void);
void irq2_handler(void);
void irq3_handler(void);
void irq4_handler(void);

#endif /* _IRQ_TEST_H_ */
