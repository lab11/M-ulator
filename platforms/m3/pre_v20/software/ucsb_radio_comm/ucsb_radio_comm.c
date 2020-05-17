//*******************************************************************
//Author: Gordy Carichner
//Description: transmits and receives data on UCSB's SiLab 4464 radio boards using single PREv20 SPI bus.
// Currently uses PREv20 and SNT layers, though SNT is a dummy layer.
//*******************************************************************
#include "PREv20.h"
#include "PREv20_RF.h"
#include "SNTv3_RF.h"
#include "mbus.h"
#include "si_cmd.h"
#include "si.h"
#include "si_cfg.h"
#include "si_prop.h"

// uncomment this for debug mbus message
#define DEBUG_MBUS_MSG
#define SI_TIMEOUT 2000

// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// uncomment this to only transmit average
//#define TX_AVERAGE

// Stack order  PRC->SNS->PMU
#define SNT_ADDR 0x4
//#define PMU_ADDR 0x6

// Temp Sensor parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)
#define TEMP_CYCLE_INIT 3 
// Tstack states
#define	TSTK_IDLE       0x0
#define	TSTK_LDO        0x1
#define TSTK_TEMP_START 0x2
#define TSTK_TEMP_READ  0x6

#define NUM_TEMP_MEAS 2
//***************************************************
// Global variables
//***************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t Tstack_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint32_t wakeup_period_count;
volatile uint32_t wakeup_timer_multiplier;
volatile uint32_t PMU_ADC_4P2_VAL;
volatile uint32_t pmu_parkinglot_mode;
volatile uint32_t pmu_harvesting_on;

volatile uint32_t gpio_mask1 = (1<<1);
volatile uint32_t gpio_mask2 = (1<<2);

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile prev20_r0B_t prev20_r0B = PREv20_R0B_DEFAULT;

// Temperature Sensor
volatile sntv3_r00_t sntv3_r00 = SNTv3_R00_DEFAULT;
volatile sntv3_r01_t sntv3_r01 = SNTv3_R01_DEFAULT;
volatile sntv3_r03_t sntv3_r03 = SNTv3_R03_DEFAULT;

// WakeUp Timer
volatile sntv3_r08_t sntv3_r08 = SNTv3_R08_DEFAULT;
volatile sntv3_r09_t sntv3_r09 = SNTv3_R09_DEFAULT;
volatile sntv3_r0A_t sntv3_r0A = SNTv3_R0A_DEFAULT;
volatile sntv3_r0B_t sntv3_r0B = SNTv3_R0B_DEFAULT;

volatile sntv3_r17_t sntv3_r17 = SNTv3_R17_DEFAULT;


//*******************************************************************
// INTERRUPT HANDLERS (Updated for PRCv17)
//*******************************************************************

void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
	wfi_timeout_flag = 1;
    }
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);
}
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1);
}
void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2);
}
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3);
}
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
    *SREG_WAKEUP_SOURCE = 0;
}

//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){

	// Reset GOC_DATA_IRQ
	*GOC_DATA_IRQ = 0;

    // Go to Sleep
    mbus_sleep_all();
    while(1);

}
static void operation_sleep_notimer(void){
	// Disable Timer
	set_wakeup_timer(0, 0, 0);

    // Go to sleep
    operation_sleep();

}

static void operation_spi_init(uint32_t mask1, uint32_t mask2){
  *SPI_SSPCR0 =  0x0207; // Motorola Mode, 8-bit word
  *SPI_SSPCPSR = 0x02;   // Clock Prescale Register

	// Enable SPI Input/Output
  set_spi_pad(1);

  set_gpio_pad(0x06); // enable two GPIOs for two radios
  gpio_set_dir_with_mask(mask1,(1<<1));
  gpio_set_dir_with_mask(mask2,(1<<2));  
  ngpio_write_data_with_mask(mask1,(1<<1));
  ngpio_write_data_with_mask(mask2,(1<<2));

  unfreeze_gpio_out();
  unfreeze_spi_out();
}

si_err_t poll_for_CTS (uint8_t mask, uint8_t gpio_rxtx){

	uint8_t cts = 0x00;	
	
	for(unsigned int i = 0; i < SI_TIMEOUT; i++) {	
		//gpio_write_data_with_mask(si4464_mask,(0<<GPIO_SI4464_EN));
		ngpio_write_data_with_mask(mask,(0<<gpio_rxtx));
		*SPI_SSPDR = SI_CMD_READ_CMD_BUFF;
		*SPI_SSPDR = SI_CMD_NOP; // dummy data to read CTS
		*SPI_SSPCR1 = 0x2; // enable serial port
		while((*SPI_SSPSR>>4)&0x1){} // wait until buffer empty
		cts = *SPI_SSPDR; // discard first byte
		cts = *SPI_SSPDR; // CTS response
		*SPI_SSPCR1 = 0x0; // disable serial port
		ngpio_write_data_with_mask(mask,(1<<gpio_rxtx));
		if(cts == 0xFF) {
			break;
		}
	}
	if(cts != 0xFF) {
		return SI_ERR_TIMEOUT;
	}
	else {
		return SI_ERR_SUCCESS;
	}
}

si_err_t poll_for_CTS_keep_en (uint8_t mask, uint8_t gpio_rxtx){

	uint8_t cts = 0x00;	
	
	for(unsigned int i = 0; i < SI_TIMEOUT; i++) {	
		ngpio_write_data_with_mask(mask,(0<<gpio_rxtx));
		*SPI_SSPDR = SI_CMD_READ_CMD_BUFF;
		*SPI_SSPDR = SI_CMD_NOP; // dummy data to read CTS
		*SPI_SSPCR1 = 0x2; // enable serial port
		while((*SPI_SSPSR>>4)&0x1){} // wait until buffer empty
		cts = *SPI_SSPDR; // discard first byte
		cts = *SPI_SSPDR; // CTS response
		*SPI_SSPCR1 = 0x0; // disable serial port
		if(cts == 0xFF) {
			break;
		}
	}
	if(cts != 0xFF) {
		return SI_ERR_TIMEOUT;
	}
	else {
		return SI_ERR_SUCCESS;
	}
}
static void Si4464_send (const uint8_t* tx_buf, uint8_t tx_len, uint8_t mask, uint8_t gpio_rxtx){
	// sends tx_buf array of length tx_len to Si4464 
	uint8_t i;
	uint8_t j;
	uint8_t discard;
	poll_for_CTS(mask, gpio_rxtx);
	ngpio_write_data_with_mask(mask,(0<<gpio_rxtx));
	for(i = 0; i < tx_len; i++) { // send the data in chunks no larger than 8 bytes
		*SPI_SSPDR = tx_buf[i];
		if((i % 8) == 7) {
			*SPI_SSPCR1 = 0x2; // enable transmit
			while((*SPI_SSPSR>>4)&0x1){}
			for(j = 0; j < 8; j++) { // clear trash from receive buffer
				discard = *SPI_SSPDR;
			}
			*SPI_SSPCR1 = 0x0;
		}
	}
	if((i % 8) != 0) {
		*SPI_SSPCR1 = 0x2; // enable transmit - items still in buffer to send
		while((*SPI_SSPSR>>4)&0x1){}
	}
	for(j = i - 1; (j % 8) != 7; j--) { // clear trash data from rx buffer
		discard = *SPI_SSPDR;
	}
	*SPI_SSPCR1 = 0x0;
	ngpio_write_data_with_mask(mask,(1<<gpio_rxtx));
}

static void Si4464_receive (uint8_t* rx_buf, uint8_t rx_len, uint8_t mask, uint8_t gpio_rxtx){
	// receives rx_buf array of length rx_len from Si4464
	uint8_t i;
	uint8_t j;
	poll_for_CTS_keep_en(mask, gpio_rxtx);
	for(i = 0; i < rx_len; i++) { // when receiving, first two bytes transmitted are read cmd buff
		*SPI_SSPDR = SI_CMD_NOP;
		if((i % 8) == 7) {
			*SPI_SSPCR1 = 0x2; // enable transmit
			while((*SPI_SSPSR>>4)&0x1){}
			for(j = i - 7; j < i + 1; j++)
				rx_buf[j] = *SPI_SSPDR;
			*SPI_SSPCR1 = 0x0;
		}
	}
	if((i % 8) != 0) { // items left in rx buffer
		*SPI_SSPCR1 = 0x2; // enable transmit
		while((*SPI_SSPSR>>4)&0x1){}
		for(j = i - (i % 8); j < rx_len; j++) // place the data from SPI rx buffer into the rx array
			rx_buf[j] = *SPI_SSPDR;
	}
	*SPI_SSPCR1 = 0x0;
	ngpio_write_data_with_mask(mask,(1<<gpio_rxtx));
}

static void Si4464_read_rx_fifo (uint8_t* rx_buf, uint8_t rx_len, uint8_t mask, uint8_t gpio_rxtx){
	// reads rx_fifo from Si4464
	uint8_t i;
	uint8_t j;
	uint8_t tmp_buf[rx_len+1];
	poll_for_CTS(mask, gpio_rxtx);
	ngpio_write_data_with_mask(mask,(0<<gpio_rxtx));
	for(i = 0; i < rx_len + 1; i++) {
		if(i == 0)
			*SPI_SSPDR = SI_CMD_READ_RX_FIFO;
		else
			*SPI_SSPDR = SI_CMD_NOP;
		if((i % 8) == 7) {
			*SPI_SSPCR1 = 0x2; // enable transmit
			while((*SPI_SSPSR>>4)&0x1){}
			for(j = i - 7; j < i + 1; j++)
				tmp_buf[j] = *SPI_SSPDR;
			*SPI_SSPCR1 = 0x0;
		}
	}
	if((i % 8) != 0) { // items left in rx buffer
		*SPI_SSPCR1 = 0x2; // enable transmit
		while((*SPI_SSPSR>>4)&0x1){}
		for(j = i - (i % 8); j < rx_len + 1; j++) // place the data from SPI rx buffer into the rx array
			tmp_buf[j] = *SPI_SSPDR;
	}
	*SPI_SSPCR1 = 0x0;
	ngpio_write_data_with_mask(mask,(1<<gpio_rxtx));
	
	for(i = 0; i < rx_len; i++) {
		rx_buf[i] = tmp_buf[i+1];
	}
}

static void si_get_prop(si_prop_t prop, uint32_t* value, unsigned int len, uint8_t mask, uint8_t gpio_rxtx) {
	/*
	Get a config property on the SI4464.
		@arg prop 	property from the enum defined in si_prop.h
		@arg value 	memory to store value of property
		@arg len	length of property in bits (e.g. 8, 16, 32). 
	Returns err/success
	*/
	//Round len up to nearest multiple of, then convert to bytes
	len = (len + 7) / 8;
	//Need room for command, group, number of properties, and property + the actual values
	uint8_t message[4];
	message[0] = SI_CMD_GET_PROPERTY;
	message[1] = SI_GRP(prop);
	message[2] = (uint8_t) len;
	message[3] = SI_PROP(prop);
	uint8_t reply[len];
	Si4464_send(message, 4, mask, gpio_rxtx);
	Si4464_receive(reply, len, mask, gpio_rxtx);
	*value = 0;
	//Skip the first value of reply because itâ€™s CTS
	for(unsigned int i = 0; i < len; i++) {
		*value |= (reply[i] << ((len - i - 1) * 8));
	}	
	//return err;
}

static void si_set_prop(si_prop_t prop, uint32_t value, unsigned int len, uint8_t mask, uint8_t gpio_rxtx) {
	/*
	Set a config property on the SI4464.
		@arg prop 	property from the enum defined in si_prop.h
		@arg value 	desired value to set the property to
		@arg len	length of property in bits (e.g. 8, 16, 32). 
	Returns err/success
	*/
	//Round len up to nearest multiple of, then convert to bytes
	len = (len + 7) / 8;
	//Need room for command, group, number of properties, and property + the actual values
	uint8_t message[4 + len];
	message[0] = SI_CMD_SET_PROPERTY;
	message[1] = SI_GRP(prop);
	message[2] = (uint8_t) len;
	message[3] = SI_PROP(prop);
	for(unsigned int i = 0; i < len; i++) {
		message[4 + i] = (value >> ((len - i - 1) * 8)) & 0xFF;
	}	

	Si4464_send(message, 4 + len, mask, gpio_rxtx);

}

si_err_t si_tx(uint8_t* buf, unsigned int buflen, uint8_t mask, uint8_t gpio_rxtx) {
	/*
		Transmits a variable-length buffer over radio
		Uses Preamble + SYNC + CRC
	*/
	if (buflen > 62) {
		// The tx fifo size max is 64
		//TODO there should be a way to load longer payloads but I'm not sure how
		//log(ERR, "Argument of size %u exceeds maximum capacity of TX_FIFO (2 byte length + 62 byte data).", buflen);
		return SI_ERR_INV_ARG;
	}
	si_err_t err = SI_ERR_SUCCESS;
	
	// clear FIFO buf
	uint8_t fifo_info[2];
	fifo_info[0] = SI_CMD_FIFO_INFO;
	fifo_info[1] = 0x01;
	Si4464_send(fifo_info, 2, mask, gpio_rxtx);
	// load tx fifo with data
	uint8_t i;
	uint8_t txbuf[buflen+1];
	txbuf[0] = SI_CMD_WRITE_TX_FIFO;
	for(i = 0; i < buflen; i++) {
		txbuf[i+1] = buf[i];
	}
	Si4464_send(txbuf, buflen + 1, mask, gpio_rxtx);

	si_set_prop(SI_PKT_FIELD_1_LENGTH, (uint32_t) buflen, 13, mask, gpio_rxtx);
	//si_set_prop(SI_PREAMBLE_CONFIG_STD_2, (uint32_t) 0xFF, 8, mask, gpio_rxtx);

	uint8_t start_tx[5];
	start_tx[0] = SI_CMD_START_TX;
	start_tx[1] = 0x00;
	start_tx[2] = 0x30;
	start_tx[3] = 0x00;
	start_tx[4] = 0x00;	
	Si4464_send(start_tx, 5, mask, gpio_rxtx);

	uint8_t get_int_status[4];
	get_int_status[0] = SI_CMD_GET_INT_STATUS;
	get_int_status[1] = 0x00;
	get_int_status[2] = 0x00;
	get_int_status[3] = 0x00;	
	uint8_t int_status[8];
	int_status[0] = 0x00;
	int_status[1] = 0x00;
	int_status[2] = 0x00;
	int_status[3] = 0x00;
	int_status[4] = 0x00;
	int_status[5] = 0x00;
	int_status[6] = 0x00;
	int_status[7] = 0x00;
	Si4464_send(get_int_status, 4, mask, gpio_rxtx);
	Si4464_receive(int_status, 8, mask, gpio_rxtx);
	uint8_t packet_sent_pend = 0x00;
	uint8_t packet_sent = 0x00;
	uint32_t k;
	while(!(packet_sent || packet_sent_pend) && k++ < SI_TIMEOUT) {
		Si4464_send(get_int_status, 4, mask, gpio_rxtx);
		Si4464_receive(int_status, 9, mask, gpio_rxtx);		
		packet_sent_pend = (int_status[3] >> 5) & 0x01;
		packet_sent = (int_status[4] >> 5) & 0x01;
	}
	if(packet_sent || packet_sent_pend) {
		mbus_write_message32(0xCC, 0x0FAB);
	}
	else {
		mbus_write_message32(0xCC, 0xDEAD);
	}
	return err;
}

void si_setup_rx(unsigned int max_buflen, uint8_t mask, uint8_t gpio_rxtx) {
	/* 
		Enters RX mode and attempts to receive a packet and read it into the buffer.
		Returns error. Loads bytes read into b_read
	*/
	//si_err_t err = SI_ERR_SUCCESS;
	 
	//Clear RX FIFO
	//TODO At higher bitrates, messages are captured in non-rx parts of the loop so we lose information		
	uint8_t fifo_info[2];
	fifo_info[0] = SI_CMD_FIFO_INFO;
	fifo_info[1] = 0x02;
	uint8_t throw[3];
	Si4464_send(fifo_info, 2, mask, gpio_rxtx);
	Si4464_receive(throw, 2, mask, gpio_rxtx);
	//mbus_write_message32(0xEE, throw[0]); 
	//mbus_write_message32(0xEE, throw[1]); 
	//Start RX Mode
	si_set_prop(SI_PKT_FIELD_1_LENGTH, (uint32_t) max_buflen, 13, mask, gpio_rxtx);
	//si_set_prop(SI_PREAMBLE_CONFIG_STD_2, (uint32_t) 0xFF, 8, mask, gpio_rxtx);
	//uint8_t start_rx[8] = { SI_CMD_START_RX, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00 };
	uint8_t start_rx[8];
	start_rx[0] = SI_CMD_START_RX;
	start_rx[1] = 0x00;
	start_rx[2] = 0x00;
	start_rx[3] = 0x00;
	start_rx[4] = 0x00;
	start_rx[5] = 0x00;
	start_rx[6] = 0x03;
	start_rx[7] = 0x00;	
	Si4464_send(start_rx, 8, mask, gpio_rxtx); 
}

si_err_t si_rx(uint8_t* buf, uint8_t buflen, uint8_t mask, uint8_t gpio_rxtx) {
	/* 
		Enters RX mode and attempts to receive a packet and read it into the buffer.
		Returns error. Loads bytes read into b_read
	*/
	si_err_t err = SI_ERR_SUCCESS;
	uint8_t throw[3];
	//Check interrupt status (wait for packet to arrive)
	//uint8_t get_int_status[4] = {SI_CMD_GET_INT_STATUS, 0x00, 0x00, 0x00};
	uint8_t get_rint_status[4];
	get_rint_status[0] = SI_CMD_GET_INT_STATUS;
	get_rint_status[1] = 0x00;
	get_rint_status[2] = 0x00;
	get_rint_status[3] = 0x00;	
	uint8_t rint_status[8];
	rint_status[0] = 0x00;
	rint_status[1] = 0x00;
	rint_status[2] = 0x00;
	rint_status[3] = 0x00;
	rint_status[4] = 0x00;
	rint_status[5] = 0x00;
	rint_status[6] = 0x00;
	rint_status[7] = 0x00;		
	uint8_t packet_rx_pend = 0;
	uint8_t packet_rx = 0;
	uint32_t q = 0;
	while(!(packet_rx || packet_rx_pend) && q++ < SI_TIMEOUT) {
		Si4464_send(get_rint_status, 4, mask, gpio_rxtx);
		Si4464_receive(rint_status, 9, mask, gpio_rxtx);			
		packet_rx_pend = (rint_status[3] >> 4) & 0x01;
		packet_rx = (rint_status[4] >> 4) & 0x01;
	}
	if(!(packet_rx || packet_rx_pend)) {
		mbus_write_message32(0xCD, 0xADAD);
		err |= SI_ERR_TXRX;
	}

	Si4464_read_rx_fifo(buf, buflen, mask, gpio_rxtx);

	return err;
}

static void Si4464_init(uint8_t mask, uint8_t gpio_rxtx){
	// sends power-up command and sets properties to initialize Si4464 radio device
	
	uint8_t length = SI_CFG[0];
	uint8_t* ptr = SI_CFG + 1;
	while(length != 0) {
		uint8_t tx[length];
		for(int i = 0; i < length; i++) {
			tx[i] = *ptr;
			ptr++;
		}
		Si4464_send(tx, length, mask, gpio_rxtx);
		length = *ptr;
		ptr++;
	}
	uint32_t cnt1 = 0;
	uint32_t misc = 0;
	uint32_t search = 0;
	uint32_t control = 0;
	si_get_prop(SI_MODEM_OOK_CNT1, &cnt1, 8, mask, gpio_rxtx);
	si_get_prop(SI_MODEM_OOK_MISC, &misc, 8, mask, gpio_rxtx);
	si_get_prop(SI_MODEM_RAW_SEARCH, &search, 8, mask, gpio_rxtx);
	si_get_prop(SI_MODEM_RAW_CONTROL, &control, 8, mask, gpio_rxtx);
	//mbus_write_message32(0xCC, cnt1);
	//mbus_write_message32(0xCC, misc);
	//mbus_write_message32(0xCC, search);\
	//mbus_write_message32(0xCC, control);	
}

static void operation_init(void){
  
	// Set CPU & Mbus Clock Speeds
    prev20_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prev20_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prev20_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
    prev20_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
    prev20_r0B.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x6
	*REG_CLKGEN_TUNE = prev20_r0B.as_int;

    // Disable MBus Watchdog Timer
    //*REG_MBUS_WD = 0;
	*((volatile uint32_t *) 0xA000007C) = 0;

    //Enumerate & Initialize Registers
    Tstack_state = TSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEE0;
    exec_count = 0;
    exec_count_irq = 0;
	PMU_ADC_4P2_VAL = 0x4B;
	pmu_parkinglot_mode = 0;
	pmu_harvesting_on = 1;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    //Enumeration
    //mbus_enumerate(RAD_ADDR);
	//delay(MBUS_DELAY);
    mbus_enumerate(SNT_ADDR);
	delay(MBUS_DELAY);
    //mbus_enumerate(HRV_ADDR);
	//delay(MBUS_DELAY);
 	//mbus_enumerate(PMU_ADDR);
	//delay(MBUS_DELAY);

    // Set CPU Halt Option as TX --> Use for register write e.g.
	//set_halt_until_mbus_tx();


    // Wakeup Timer Settings --------------------------------------

    // Config Register A
	sntv3_r0A.TMR_S = 0x1; // Default: 0x4
	//sntv3_r0A.TMR_S = 0x4; // Default: 0x4
    //sntv3_r0A.TMR_DIFF_CON = 0x1FFF; // Default: 0x3FFB
    sntv3_r0A.TMR_DIFF_CON = 0x3FFF; // Default: 0x3FFB

    mbus_remote_register_write(SNT_ADDR,0x0A,sntv3_r0A.as_int);

	// TIMER CAP_TUNE  
    sntv3_r0B.TMR_TFR_CON = 0xF; // Default: 0xF
    mbus_remote_register_write(SNT_ADDR,0x0B,sntv3_r0B.as_int);

	// TIMER CAP_TUNE  
	sntv3_r09.TMR_SEL_CLK_DIV = 0x0; // Default : 1'h1
	sntv3_r09.TMR_SEL_CAP = 0x80; // Default : 8'h8
	sntv3_r09.TMR_SEL_DCAP = 0x3F; // Default : 6'h4

	//sntv3_r09.TMR_SEL_CAP = 0x8; // Default : 8'h8
	//sntv3_r09.TMR_SEL_DCAP = 0x4; // Default : 6'h4
	sntv3_r09.TMR_EN_TUNE1 = 0x1; // Default : 1'h1
	sntv3_r09.TMR_EN_TUNE2 = 0x1; // Default : 1'h1
	mbus_remote_register_write(SNT_ADDR,0x09,sntv3_r09.as_int);

	// Enalbe Frequency Monitoring 
	sntv3_r17.WUP_ENABLE_CLK_SLP_OUT = 0x1; 
	mbus_remote_register_write(SNT_ADDR,0x17,sntv3_r17.as_int);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 100;   // 10: 2-4 sec with PRCv17
	wakeup_data = 0;

    delay(MBUS_DELAY);

    // Go to sleep
	//set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
    //operation_sleep();
}

//********************************************************************
// MAIN function starts here             
//***************************************************************-*****

int main() {

	//uint8_t cts = 0x00;	
    // Only enable relevant interrupts (PRCv17)
	//enable_reg_irq();
	//enable_all_irq();
	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);
  
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
	//    config_timerwd(TIMERWD_VAL);

	//	wakeup_count++;
	
	disable_timerwd();

	if (enumerated != 0xDEADBEEF){
	  operation_init();
	}
	
	operation_spi_init(gpio_mask1, gpio_mask2); // enable PREv20 SPI

	Si4464_init(gpio_mask2, 2);
	Si4464_init(gpio_mask1, 1);

	//uint8_t message[8] = { 0x01, 0x09, 0x06, 0x07, 0x04, 0x05, 0x06, 0x07};
	uint8_t message[8];
	message[0] = 0x01;
	message[1] = 0x09;
	message[2] = 0x06;
	message[3] = 0x07;
	message[4] = 0x04;
	message[5] = 0x05;
	message[6] = 0x06;
	message[7] = 0x07;	
	uint8_t rx_message[8];
	si_setup_rx(8, gpio_mask2, 2);
	
	while(1) {
		//Incrementing message
		for(uint8_t q = 0; q < 4; q++) {
			message[7 - q] = message[7 - q] + 1;
		}
		if(si_tx(message, 8, gpio_mask1, 1) == SI_ERR_SUCCESS) {
			mbus_write_message32(0xAA, 0xFABB);
		}
		else {
			mbus_write_message32(0xAA, 0xDEDD);
		}
		
		if(si_rx(rx_message, 8, gpio_mask2, 2) == SI_ERR_SUCCESS) {
			for(uint8_t j=0; j<8; j++) {
				mbus_write_message32(0xBB, rx_message[j]);
			}
		}
		else {
			mbus_write_message32(0xBB, 0xDDED);
		}
		si_setup_rx(8, gpio_mask2, 2);
		
	}

 return 1;
}

