//*******************************************************************
//Author: Yejoong Kim
//Description: GPIO Header File
//*******************************************************************

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

void     init_GPIO (uint32_t new_gpiodir);
void     set_GPIO_DIR (uint32_t new_gpiodir);
uint32_t get_GPIO_DIR ();
uint32_t get_GPIO_DATA ();
void     set_GPIO_WRITE (uint32_t new_gpio_write);
void     set_GPIO_bit (uint32_t loc);
void     kill_GPIO_bit (uint32_t loc);
void     set_GPIO_2bits (uint32_t loc0, uint32_t loc1);
void     set_GPIO_RAW (uint32_t raw_data);

#endif // GPIO_H

