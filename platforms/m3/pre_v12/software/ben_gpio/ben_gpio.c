#include "mbus.h"
#include "PRCv9E.h"


volatile uint32_t enumerated;

#define SPI_BASE 0xA7000000
#define SSPCR0 *((volatile uint8_t *) (SPI_BASE + 0x00))
#define SSPCR1 *((volatile uint8_t *) (SPI_BASE + 0x04))
#define SSPDR *((volatile uint32_t *) (SPI_BASE + 0x08))
#define SSPSR *((volatile uint8_t *) (SPI_BASE + 0x0C))
#define SSPCPSR *((volatile uint8_t *) (SPI_BASE + 0x10))
#define SSPIMSC *((volatile uint8_t *) (SPI_BASE + 0x14))

#define SSPSR_BSY_BIT 0x00000010
#define SSPSR_TFE_BIT 0x00000001

#define ACOUSTIC_CS 0x00000002
#define SEISMIC_CS 0x00000001
#define NO_CS 0x00000003
#define I2C_SDA_PD 0x00000008
#define I2C_SCL_PD 0x00000004
#define I2C_SDA    0x00000020
#define I2C_SCL    0x00000010
#define I2C        0x00000030

volatile uint32_t* _GPIO_DATA = (volatile uint32_t *) 0xA6000000;
volatile uint32_t* _GPIO_DIR = (volatile uint32_t *) 0xA6001000;

//***************************************************
//Interrupt Handlers
//Must clear pending bit!
//***************************************************
void handler_ext_int_0(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_4(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_5(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_6(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_7(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_8(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_9(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));


void handler_ext_int_0(void){
	*((volatile uint32_t *) 0xE000E280) = 0x1;
	write_mbus_message(0x13, 0x0F0F00000);
}
void handler_ext_int_1(void){
	*((volatile uint32_t *) 0xE000E280) = 0x2;
	write_mbus_message(0x13, 0x0F0F00001);
}
void handler_ext_int_2(void){
	*((volatile uint32_t *) 0xE000E280) = 0x4;
	write_mbus_message(0x13, 0x0F0F00002);
}
void handler_ext_int_3(void){
	*((volatile uint32_t *) 0xE000E280) = 0x8;
	write_mbus_message(0x13, 0x0F0F00003);
}
void handler_ext_int_4(void){
//	uint32_t *address;
//	uint32_t data;
//	address	= (uint32_t *) 0xA6000000;
//	data = *address;
//	write_mbus_message(0xF0, data);
	*((volatile uint32_t *) 0xE000E280) = 0x10;
	write_mbus_message(0x13, 0x0F0F0004);
}
void handler_ext_int_5(void){
	*((volatile uint32_t *) 0xE000E280) = 0x20;
	write_mbus_message(0x13, 0x0F0F0005);
}
void handler_ext_int_6(void){
	*((volatile uint32_t *) 0xE000E280) = 0x40;
	write_mbus_message(0x13, 0x0F0F0006);
}
void handler_ext_int_7(void){
	*((volatile uint32_t *) 0xE000E280) = 0x80;
	write_mbus_message(0x13, 0x0F0F0007);
}
void handler_ext_int_8(void){
	*((volatile uint32_t *) 0xE000E280) = 0x100;
//	write_mbus_message(0x13, 0x0F0F0008);
//	write_mbus_message(0x13, *((volatile uint32_t *) 0xA500021C));
//	write_mbus_message(0x13, *((volatile uint32_t *) 0xA5000220));
}
void handler_ext_int_9(void){
	*((volatile uint32_t *) 0xE000E280) = 0x200;
	write_mbus_message(0x13, 0x0F0F0009);
//	write_mbus_message(0x13, *((volatile uint32_t *) 0xA8000000));
//	write_mbus_message(0x13, *((volatile uint32_t *) 0xA8000004));
}
void handler_ext_int_10(void){
	*((volatile uint32_t *) 0xE000E280) = 0x400;
	write_mbus_message(0x13, 0x0F0F000A);
}
void handler_ext_int_11(void){
	*((volatile uint32_t *) 0xE000E280) = 0x800;
	write_mbus_message(0x13, 0x0F0F000B);
}

//***************************************************
// Subfunctions
//***************************************************



void initialize(){
	enumerated = 0xABCDEF01;
	write_mbus_message(0x13,0xeeeeeeee);
}

void gpio_control(uint32_t data){
	uint32_t *address;
	address = (uint32_t *) 0xA6000000;
	*address = data;
//	write_mbus_message(0x13,data);
}

uint32_t read_acoustic(){
	gpio_control(ACOUSTIC_CS);
	SSPDR = 0xEF01; //RDSR
	while((SSPSR & SSPSR_BSY_BIT));
	gpio_control(NO_CS);
	return SSPDR;
}
uint32_t read_seismic(){
	gpio_control(SEISMIC_CS);
	SSPDR = 0xABCD; //RDSR
	while((SSPSR & SSPSR_BSY_BIT));
	gpio_control(NO_CS);
	return SSPDR;
}

void wait_for_i2c_bit(){
	uint32_t last_i2c_sample = 0;
	uint32_t cur_i2c_sample = 0;

	//Wait for negative edge-going SCL
	do{
		last_i2c_sample = cur_i2c_sample;
		cur_i2c_sample = *_GPIO_DATA & I2C_SCL;
	} while(!(last_i2c_sample == I2C_SCL && cur_i2c_sample == 0));
}

//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************

int main() {
	volatile uint32_t  gpio_data;
	volatile uint32_t  spi_data;
//	volatile uint32_t i;

	//Clear All Pending Interrupts
	*((volatile uint32_t *) 0xE000E280) = 0x3FF;
	//Enable Interrupts
//	*((volatile uint32_t *) 0xE000E100) = 0x07F;	// All timer ignored
	*((volatile uint32_t *) 0xE000E100) = 0x3FF;	// All interrupt enable
//	*((volatile uint32_t *) 0xE000E100) = 0;	// All interrupt enable
	
	if( enumerated != 0xABCDEF01 ) initialize();

	//Truning off the watch dog
	*((volatile uint32_t *) 0xA5000000) = 0;


	// MEM&CORE CLK /16
	//Fastest clock tested to work.  Faster may work but pads don't seem to keep up
	*((volatile uint32_t *) 0xA2000008) =
		( (0x1 << 24) /* GPIO_ENABLE */
		| (0x1 << 23) /* High Frequency Mode */
		| (0x1 << 20) /* PMUCFG_OSCMID_DIV_F[2:0] */
		| (0x0 << 17) /* PMUCFG_OSCMID_SEL_F[2:0] */
		| (0x0 << 16) /* PMUCFG_ACT_NONOV_SEL */
		| (0x3 << 14) /* ?? */
		| (0x0 << 12) /* CLKX2_SEL_CM[1:0]; Divider 0:/2, 1:/4, 2:/8, 3:/16 */
		| (0x1 << 10) /* CLKX2_SEL_I2C[1:0]; Divider 0:/1, 1:/2, 2:/4, 3:/8 */
		| (0x1 << 8)  /* CLKX2_SEL_RING[1:0] */
		| (0x0 << 7)  /* PMU_FORCE_WAKE */
		| (0x0 << 6)  /* GOC_ONECLK_MODE */
		| (0x0 << 4)  /* GOC_SEL_DLY[1:0] */
		| (0x8 << 0)  /* GOC_SEL[3:0] */
	);
	//*((volatile uint32_t *) 0xA2000008) =
	//	( (0x1 << 24) /* GPIO_ENABLE */
	//	| (0x0 << 23) /* High Frequency Mode */
	//	| (0x1 << 20) /* PMUCFG_OSCMID_DIV_F[2:0] */
	//	| (0x0 << 17) /* PMUCFG_OSCMID_SEL_F[2:0] */
	//	| (0x0 << 16) /* PMUCFG_ACT_NONOV_SEL */
	//	| (0x3 << 14) /* ?? */
	//	| (0x0 << 12) /* CLKX2_SEL_CM[1:0]; Divider 0:/2, 1:/4, 2:/8, 3:/16 */
	//	| (0x1 << 10) /* CLKX2_SEL_I2C[1:0]; Divider 0:/1, 1:/2, 2:/4, 3:/8 */
	//	| (0x3 << 8)  /* CLKX2_SEL_RING[1:0] */
	//	| (0x0 << 7)  /* PMU_FORCE_WAKE */
	//	| (0x0 << 6)  /* GOC_ONECLK_MODE */
	//	| (0x0 << 4)  /* GOC_SEL_DLY[1:0] */
	//	| (0x8 << 0)  /* GOC_SEL[3:0] */
	//);


	*_GPIO_DIR = 0xFFFFFFF0;		// Direction register is reversed.

	//Initialize

	*_GPIO_DATA = 0x00000003;
	
	delay(10000);
	
	while(1){
		uint32_t last_i2c_sample = 0;
		uint32_t cur_i2c_sample = 0;
		int ii, jj;

		//while(1){
		//	if(*_GPIO_DATA & I2C_SDA)
		//		*_GPIO_DATA = 0x0000000F;
		//	else
		//		*_GPIO_DATA = 0x00000003;
		//}

		//Wait for start condition
		do{
			last_i2c_sample = cur_i2c_sample;
			cur_i2c_sample = (*_GPIO_DATA) & I2C;
		} while(!(last_i2c_sample == (I2C_SCL | I2C_SDA) && cur_i2c_sample == I2C_SCL));
		*_GPIO_DATA = 0x0000000F;
		while(1);
		for(jj = 0; jj < 2; jj++){
			for(ii = 0; ii < 9; ii++)
				wait_for_i2c_bit();
			*_GPIO_DATA |= I2C_SDA_PD;
			wait_for_i2c_bit();
			*_GPIO_DATA &= ~I2C_SDA_PD;
		}
	}

}
