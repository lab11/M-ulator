//*******************************************************************
//Author: Yejoong Kim
//Description: GPIO Lib File
//*******************************************************************

#include "GPIO.h"

volatile uint32_t* GPIO_DATA_ = (volatile uint32_t *) 0xA6000008;
volatile uint32_t* GPIO_DIR_ = (volatile uint32_t *) 0xA6001000;
volatile uint32_t  GPIO_WRITE_;

void init_GPIO (uint32_t new_gpiodir) {
	set_GPIO_DIR(new_gpiodir);
	set_GPIO_WRITE(0x00000000);
}
void set_GPIO_DIR (uint32_t new_gpiodir) {
	*GPIO_DIR_ = new_gpiodir;
}
uint32_t get_GPIO_DIR () {
	return *GPIO_DIR_;
}
uint32_t get_GPIO_DATA () {
	return *GPIO_DATA_;
}
void set_GPIO_WRITE (uint32_t new_gpio_write) {
	GPIO_WRITE_ = new_gpio_write;
}
void set_GPIO_bit(uint32_t loc){ 
	GPIO_WRITE_ = (GPIO_WRITE_ | (1 << loc));
	*GPIO_DATA_ = GPIO_WRITE_;
}
void kill_GPIO_bit(uint32_t loc){ 
	GPIO_WRITE_ = ~( (~GPIO_WRITE_) | (1 << loc));
	*GPIO_DATA_ = GPIO_WRITE_;
}
void set_GPIO_2bits(uint32_t loc0, uint32_t loc1){ 
	GPIO_WRITE_ = (GPIO_WRITE_ | (1 << loc0) | (1 << loc1));
	*GPIO_DATA_ = GPIO_WRITE_;
}
void set_GPIO_RAW(uint32_t raw_data){ 
	*GPIO_DATA_ = raw_data;
}
