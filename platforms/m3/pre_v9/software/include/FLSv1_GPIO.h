//*******************************************************************
//Author: Yejoong Kim
//Description: FLSv1_GPIO Header File
//*******************************************************************

#ifndef FLSv1_GPIO_H
#define FLSv1_GPIO_H

// GPIO Pin Assignment
#define FLS_CIN  12
#define FLS_DIN  13
#define FLS_COUT 14
#define FLS_DOUT 15

#define GPIO_MBus_HalfCycle 10
#define GPIO_MBus_RxBeginDelay 100

#include <stdint.h>
#include <stdbool.h>
#include <PRCv9E.h>
#include <GPIO.h>

volatile uint32_t  RxAddr;
volatile uint32_t  RxData0;
volatile uint32_t  RxData1;
volatile uint32_t  RxData2;

void FLSMBusGPIO_initialization();

uint32_t FLSMBusGPIO_getRxAddr();
uint32_t FLSMBusGPIO_getRxData0();
uint32_t FLSMBusGPIO_getRxData1();
uint32_t FLSMBusGPIO_getRxData2();

void FLSMBusGPIO_resetCinDin ();

void FLSMBusGPIO_toggleCinDin (volatile uint32_t data, volatile uint32_t numBit);

void FLSMBusGPIO_interjection ();

void FLSMBusGPIO_sendMBus8bit (volatile uint32_t short_prefix, volatile uint32_t data_0);
void FLSMBusGPIO_sendMBus32bit (volatile uint32_t short_prefix, volatile uint32_t data_0);
void FLSMBusGPIO_sendMBus64bit (volatile uint32_t short_prefix, volatile uint32_t data_0, volatile uint32_t data_1);
void FLSMBusGPIO_sendMBus96bit (volatile uint32_t short_prefix, volatile uint32_t data_0, volatile uint32_t data_1, volatile uint32_t data_2);

uint32_t FLSMBusGPIO_getCoutDout ();
uint32_t FLSMBusGPIO_getCout ();
uint32_t FLSMBusGPIO_getDout ();

uint32_t FLSMBusGPIO_dataFwd ();

uint32_t FLSMBusGPIO_rxMsg ();

#endif // FLSv1_GPIO_H

