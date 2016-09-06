//*******************************************************************
//Author: Ziyun Li Cao Gao
//Description: DLCv1 lib file
//*******************************************************************

#include "DLCv1.h"
#include "mbus.h"
#include "utils.h"
#include "dnn_parameters.h"

#define WRITE_INST_DELAY 	4
#define WRITE_NLI_DELAY 	4
#define READ_SRAM_DELAY 	6

//*******************************************************************
// OTHER FUNCTIONS
//*******************************************************************

void delay(unsigned ticks){
  unsigned i;
  for (i=0; i < ticks; i++)
    asm("nop;");
}

void WFI(){
  asm("wfi;");
}

inline void switch_inst_buffer(uint8_t PE, bool slot) {
  if (slot) {	
    *PE_INST |= 1 << (PE + 12);		      // PE_INST_SEL: slot = true: select second instruction buffer
  } else {
    *PE_INST &= ~(0b1 << (PE + 12));		// PE_INST_SEL: slot = false: select first instruction buffer
  }
}

inline void set_all_buffer0() {
  *PE_INST &= ~(0b1111 << 12);
}

inline void set_all_buffer1() {
  *PE_INST |= (0b1111 << 12);
}

void reset_PE(uint8_t PE) {
	switch (PE) {
		case 0:  
      *DNN_RE_INIT_0 |= (1 << 1);
      *DNN_RE_INIT_0 &= ~(1 << 1);
      break;
		case 1:  
      *DNN_RE_INIT_1 |= (1 << 1);
      *DNN_RE_INIT_1 &= ~(1 << 1);
      break;
		case 2:  
      *DNN_RE_INIT_2 |= (1 << 1);
      *DNN_RE_INIT_2 &= ~(1 << 1);
      break;
		case 3:  
      *DNN_RE_INIT_3 |= (1 << 1);
      *DNN_RE_INIT_3 &= ~(1 << 1);
      break;
  	default: break;		// TODO: report fail 
  }
}

void write_instruction_24word(uint32_t word, uint8_t PE, uint8_t addr, bool slot) {
  uint32_t temp_DNN_CTRL = (0 + (1 << 15) + (addr << 16));
  if (slot) temp_DNN_CTRL += (6 << 16);
	uint32_t temp_word0 = word & 0xffffff;
	uint32_t temp_word1 = word >> 24;
  switch (PE) {
  	case 0:  
  		*DNN_INST_0_0 = temp_word0; 
  		*DNN_INST_0_1 = temp_word1; 
      *DNN_CTRL_0 = temp_DNN_CTRL;
  		break;
  	case 1:  
  		*DNN_INST_1_0 = temp_word0; 
  		*DNN_INST_1_1 = temp_word1; 
      *DNN_CTRL_1 = temp_DNN_CTRL;
  		break;
  	case 2:  
  		*DNN_INST_2_0 = temp_word0; 
  		*DNN_INST_2_1 = temp_word1; 
      *DNN_CTRL_2 = temp_DNN_CTRL;
  		break;
  	case 3:  
  		*DNN_INST_3_0 = temp_word0; 
  		*DNN_INST_3_1 = temp_word1; 
      *DNN_CTRL_3 = temp_DNN_CTRL;
  	  break;
  	default: break;		// TODO: report fail 
  }
}

void write_instruction(uint16_t inst_no, uint8_t PE, bool slot) {
	uint8_t addr;
  uint32_t temp_DNN_CTRL;
  uint32_t temp_word0, temp_word1;

	for (addr = 0; addr < 6; addr++) {
		if (addr == 0) {
			temp_DNN_CTRL = (0 & ~(0b1111 << 16));	// PE_INST_ADDR = 0b0000
      if (slot)	temp_DNN_CTRL |= (0b0110 << 16); 
	    temp_DNN_CTRL |= (1 << 15);			// start load: PE_INST_WR = 1
		}	else {
			temp_DNN_CTRL += (1 << 16);			// PE_INST_ADDR += 1 
		}

	  temp_word0 = PE_INSTS[inst_no][PE][addr] & 0xffffff;
	  temp_word1 = PE_INSTS[inst_no][PE][addr] >> 24;
	  switch (PE) {
	  	case 0:  
	  		*DNN_INST_0_0 = temp_word0;
	  		*DNN_INST_0_1 = temp_word1; 
        *DNN_CTRL_0 = temp_DNN_CTRL;
	  		break;
	  	case 1:  
	  		*DNN_INST_1_0 = temp_word0; 
	  		*DNN_INST_1_1 = temp_word1; 
        *DNN_CTRL_1 = temp_DNN_CTRL;
	  		break;
	  	case 2:  
	  		*DNN_INST_2_0 = temp_word0;
	  		*DNN_INST_2_1 = temp_word1; 
        *DNN_CTRL_2 = temp_DNN_CTRL;
	  		break;
	  	case 3:  
	  		*DNN_INST_3_0 = temp_word0; 
	  		*DNN_INST_3_1 = temp_word1;
        *DNN_CTRL_3 = temp_DNN_CTRL;
	  	  break;
	  	default: break;		// TODO: report fail 
	  }
//    delay(WRITE_INST_DELAY);
	}
}

void write_instruction_4PE(uint16_t inst_no, bool slot) {
	uint8_t addr;
  uint32_t temp_DNN_CTRL;
  uint32_t temp_word0, temp_word1;

	for (addr = 0; addr < 6; addr++) {
		if (addr == 0) {
			temp_DNN_CTRL = (0 & ~(0b1111 << 16));	// PE_INST_ADDR = 0b0000
      if (slot)	temp_DNN_CTRL |= (0b0110 << 16); 
	    temp_DNN_CTRL |= (1 << 15);			// start load: PE_INST_WR = 1
		}	else {
			temp_DNN_CTRL += (1 << 16);			// PE_INST_ADDR += 1 
		}

	  temp_word0 = PE_INSTS[inst_no][0][addr] & 0xffffff;
	  temp_word1 = PE_INSTS[inst_no][0][addr] >> 24;
	  *DNN_INST_0_0 = temp_word0;
	  *DNN_INST_0_1 = temp_word1; 
    *DNN_CTRL_0 = temp_DNN_CTRL;
	  temp_word0 = PE_INSTS[inst_no][1][addr] & 0xffffff;
	  temp_word1 = PE_INSTS[inst_no][1][addr] >> 24;
	  *DNN_INST_1_0 = temp_word0; 
	  *DNN_INST_1_1 = temp_word1; 
    *DNN_CTRL_1 = temp_DNN_CTRL;
	  temp_word0 = PE_INSTS[inst_no][2][addr] & 0xffffff;
	  temp_word1 = PE_INSTS[inst_no][2][addr] >> 24;
	  *DNN_INST_2_0 = temp_word0;
	  *DNN_INST_2_1 = temp_word1; 
    *DNN_CTRL_2 = temp_DNN_CTRL;
	  temp_word0 = PE_INSTS[inst_no][3][addr] & 0xffffff;
	  temp_word1 = PE_INSTS[inst_no][3][addr] >> 24;
	  *DNN_INST_3_0 = temp_word0; 
	  *DNN_INST_3_1 = temp_word1;
    *DNN_CTRL_3 = temp_DNN_CTRL;
//    delay(WRITE_INST_DELAY);
	}
}

void start_pe_inst(uint8_t PE) {
	uint32_t temp = 0;
	if (PE & 0b0001)	temp |= 0b1 << 1; 
	if (PE & 0b0010)	temp |= 0b1 << 4; 
	if (PE & 0b0100)	temp |= 0b1 << 7; 
	if (PE & 0b1000)	temp |= 0b1 << 10;
	*PE_INST |= temp;
}

void wait_until_pe_start(uint8_t PE) {
	while ((~PE | *PE_INST_DONE) != 0xffffffff)	{ delay(5);}
}

void wait_until_pe_finish(uint8_t PE) {
	while ((PE & *PE_INST_DONE) != 0)	{ delay(5);}
}

bool check_if_pe_finish(uint8_t PE) {
	return ((PE & *PE_INST_DONE) == 0); // *PE_INST_DONE will be 0 when not running	
}
/*
void read_dnn_sram_16(uint16_t start_address, int16_t* data, uint16_t length) {
	uint16_t addr, temp_addr;
	uint8_t PE = start_address >> 13;
	uint16_t short_address = start_address & 0x1fff;
	switch (PE) {
		case 0:
			*DNN_PG_0 = 0x000007ff;		// only last 11 bits are set
			*DNN_CTRL_0 = 0;  // TODO: make sure it's ok
		case 1:
			*DNN_PG_1 = 0x000007ff;
			*DNN_CTRL_1 = 0;
		case 2:
			*DNN_PG_2 = 0x000007ff;
			*DNN_CTRL_2 = 0;
		case 3:
			*DNN_PG_3 = 0x000007ff;
			*DNN_CTRL_3 = 0;
  }
  temp_addr = 0;
	for (addr = short_address; addr < length + short_address; addr++) {
	  uint32_t y0, y1, y2, y3;
	  switch (PE) {
		  case 0:
		  	*DNN_CTRL_0 = (addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		  	delay(READ_SRAM_DELAY);

		  	y3 = *DNN_R_DATA_0_3; 
		  	y2 = *DNN_R_DATA_0_2; 
		  	y1 = *DNN_R_DATA_0_1; 
		  	y0 = *DNN_R_DATA_0_0; 
		  	break;
		  case 1:
		  	*DNN_CTRL_1 = (addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		  	delay(READ_SRAM_DELAY);

		  	y3 = *DNN_R_DATA_1_3;  
		  	y2 = *DNN_R_DATA_1_2;  
		  	y1 = *DNN_R_DATA_1_1;  
		  	y0 = *DNN_R_DATA_1_0;  
		  	break;
		  case 2:
		  	*DNN_CTRL_2 = (addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		  	delay(READ_SRAM_DELAY);

		  	y3 = *DNN_R_DATA_2_3;   
		  	y2 = *DNN_R_DATA_2_2;   
		  	y1 = *DNN_R_DATA_2_1;   
		  	y0 = *DNN_R_DATA_2_0;   
		  	break;
		  case 3:
		  	*DNN_CTRL_3 = (addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		  	delay(READ_SRAM_DELAY);

		  	y3 = *DNN_R_DATA_3_3;   
		  	y2 = *DNN_R_DATA_3_2;   
		  	y1 = *DNN_R_DATA_3_1;   
		  	y0 = *DNN_R_DATA_3_0;   
		  	break;
	    default: break;		// TODO: report fail 
	  }
	  data[temp_addr]   = (y0 & 0xffff);
	  data[temp_addr+1] = (((y0 >> 16) & 0xff) + ((y1 & 0xff) << 8));
	  data[temp_addr+2] = ((y1 >> 8) & 0xffff); 
	  data[temp_addr+3] = (y2 & 0xffff);                              
	  data[temp_addr+4] = (((y2 >> 16) & 0xff) + ((y3 & 0xff) << 8));
	  data[temp_addr+5] = ((y3 >> 8) & 0xffff); 
    temp_addr += 6;
	}
}
*/

void read_dnn_sram_16(uint16_t start_address, int16_t* data, uint16_t length) {
	uint16_t addr, temp_addr;
	uint8_t PE = start_address >> 13;
	uint16_t short_address = start_address & 0x1fff;
	switch (PE) {
		case 0:
			*DNN_PG_0 = 0x000007ff;		// only last 11 bits are set
			*DNN_CTRL_0 = 0;  // TODO: make sure it's ok
		case 1:
			*DNN_PG_1 = 0x000007ff;
			*DNN_CTRL_1 = 0;
		case 2:
			*DNN_PG_2 = 0x000007ff;
			*DNN_CTRL_2 = 0;
		case 3:
			*DNN_PG_3 = 0x000007ff;
			*DNN_CTRL_3 = 0;
  }

  // fetch first word
	switch (PE) {
	  case 0:
	  	*DNN_CTRL_0 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	  	break;
	  case 1:
	  	*DNN_CTRL_1 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	  	break;
	  case 2:
	  	*DNN_CTRL_2 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	  	break;
	  case 3:
	  	*DNN_CTRL_3 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	  	break;
	  default: break;		// TODO: report fail 
	}
  delay(READ_SRAM_DELAY);
	uint32_t y0, y1, y2, y3;
	switch (PE) {
	  case 0:
	  	y3 = *DNN_R_DATA_0_3; 
	  	y2 = *DNN_R_DATA_0_2; 
	  	y1 = *DNN_R_DATA_0_1; 
	  	y0 = *DNN_R_DATA_0_0; 
	  	break;
	  case 1:
	  	y3 = *DNN_R_DATA_1_3;  
	  	y2 = *DNN_R_DATA_1_2;  
	  	y1 = *DNN_R_DATA_1_1;  
	  	y0 = *DNN_R_DATA_1_0;  
	  	break;
	  case 2:
	  	y3 = *DNN_R_DATA_2_3;   
	  	y2 = *DNN_R_DATA_2_2;   
	  	y1 = *DNN_R_DATA_2_1;   
	  	y0 = *DNN_R_DATA_2_0;   
	  	break;
	  case 3:
	  	y3 = *DNN_R_DATA_3_3;   
	  	y2 = *DNN_R_DATA_3_2;   
	  	y1 = *DNN_R_DATA_3_1;   
	  	y0 = *DNN_R_DATA_3_0;   
	  	break;
	  default: break;		// TODO: report fail 
	}

  temp_addr = 0;
	for (addr = short_address + 1; addr < length + short_address; addr++) {
    // fetch current word
	  switch (PE) {
	    case 0:
	    	*DNN_CTRL_0 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	    	break;
	    case 1:
	    	*DNN_CTRL_1 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	    	break;
	    case 2:
	    	*DNN_CTRL_2 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	    	break;
	    case 3:
	    	*DNN_CTRL_3 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	    	break;
	    default: break;		// TODO: report fail 
	  }
    // process previous word
	  data[temp_addr]   = (y0 & 0xffff);
	  data[temp_addr+1] = (((y0 >> 16) & 0xff) + ((y1 & 0xff) << 8));
	  data[temp_addr+2] = ((y1 >> 8) & 0xffff); 
	  data[temp_addr+3] = (y2 & 0xffff);                              
	  data[temp_addr+4] = (((y2 >> 16) & 0xff) + ((y3 & 0xff) << 8));
	  data[temp_addr+5] = ((y3 >> 8) & 0xffff); 
    temp_addr += 6;
    // fetch current word
	  switch (PE) {
		  case 0:
		  	y3 = *DNN_R_DATA_0_3; 
		  	y2 = *DNN_R_DATA_0_2; 
		  	y1 = *DNN_R_DATA_0_1; 
		  	y0 = *DNN_R_DATA_0_0; 
		  	break;
		  case 1:
		  	y3 = *DNN_R_DATA_1_3;  
		  	y2 = *DNN_R_DATA_1_2;  
		  	y1 = *DNN_R_DATA_1_1;  
		  	y0 = *DNN_R_DATA_1_0;  
		  	break;
		  case 2:
		  	y3 = *DNN_R_DATA_2_3;   
		  	y2 = *DNN_R_DATA_2_2;   
		  	y1 = *DNN_R_DATA_2_1;   
		  	y0 = *DNN_R_DATA_2_0;   
		  	break;
		  case 3:
		  	y3 = *DNN_R_DATA_3_3;   
		  	y2 = *DNN_R_DATA_3_2;   
		  	y1 = *DNN_R_DATA_3_1;   
		  	y0 = *DNN_R_DATA_3_0;   
		  	break;
	    default: break;		// TODO: report fail 
	  }
	}

  // process last word
	data[temp_addr]   = (y0 & 0xffff);
	data[temp_addr+1] = (((y0 >> 16) & 0xff) + ((y1 & 0xff) << 8));
	data[temp_addr+2] = ((y1 >> 8) & 0xffff); 
	data[temp_addr+3] = (y2 & 0xffff);                              
	data[temp_addr+4] = (((y2 >> 16) & 0xff) + ((y3 & 0xff) << 8));
	data[temp_addr+5] = ((y3 >> 8) & 0xffff); 
}

void read_dnn_sram(uint16_t start_address, int32_t* data, uint16_t length) {
	uint16_t addr, temp_addr;
	uint8_t PE = start_address >> 13;
	uint16_t short_address = start_address & 0x1fff;
	switch (PE) {
		case 0:
			*DNN_PG_0 = 0x000007ff;		// only last 11 bits are set
			*DNN_CTRL_0 = 0;  // TODO: make sure it's ok
		case 1:
			*DNN_PG_1 = 0x000007ff;
			*DNN_CTRL_1 = 0;
		case 2:
			*DNN_PG_2 = 0x000007ff;
			*DNN_CTRL_2 = 0;
		case 3:
			*DNN_PG_3 = 0x000007ff;
			*DNN_CTRL_3 = 0;
  }
  temp_addr = 0;
	for (addr = short_address; addr < length + short_address; addr++) {
	  uint32_t y0, y1, y2, y3;
	  switch (PE) {
		  case 0:
		  	*DNN_CTRL_0 = (addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		  	delay(READ_SRAM_DELAY);

		  	y3 = *DNN_R_DATA_0_3; 
		  	y2 = *DNN_R_DATA_0_2; 
		  	y1 = *DNN_R_DATA_0_1; 
		  	y0 = *DNN_R_DATA_0_0; 
		  	break;
		  case 1:
		  	*DNN_CTRL_1 = (addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		  	delay(READ_SRAM_DELAY);

		  	y3 = *DNN_R_DATA_1_3;  
		  	y2 = *DNN_R_DATA_1_2;  
		  	y1 = *DNN_R_DATA_1_1;  
		  	y0 = *DNN_R_DATA_1_0;  
		  	break;
		  case 2:
		  	*DNN_CTRL_2 = (addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		  	delay(READ_SRAM_DELAY);

		  	y3 = *DNN_R_DATA_2_3;   
		  	y2 = *DNN_R_DATA_2_2;   
		  	y1 = *DNN_R_DATA_2_1;   
		  	y0 = *DNN_R_DATA_2_0;   
		  	break;
		  case 3:
		  	*DNN_CTRL_3 = (addr << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
		  	delay(READ_SRAM_DELAY);

		  	y3 = *DNN_R_DATA_3_3;   
		  	y2 = *DNN_R_DATA_3_2;   
		  	y1 = *DNN_R_DATA_3_1;   
		  	y0 = *DNN_R_DATA_3_0;   
		  	break;
	    default: break;		// TODO: report fail 
	  }
	  data[temp_addr]   = (y1 << 24) + y0;					// y1 last 8 bits + y0 
	  data[temp_addr+1] = (y2 << 16) + (y1 >> 8);		// y2 last 16 bits + y1 first 16 bits 
	  data[temp_addr+2] = (y3 << 8) + (y2 >> 16);		// y3 + y2 first 8 bits 
    temp_addr += 3;
	}
}

void read_dnn_sram_12_output(uint16_t start_address, int16_t* data, uint16_t length) {
	uint16_t addr, temp_addr;
	uint8_t PE = start_address >> 13;
	uint16_t short_address = start_address & 0x1fff;
	switch (PE) {
		case 0:
			*DNN_PG_0 = 0x000007ff;		// only last 11 bits are set
			*DNN_CTRL_0 = 0;  // TODO: make sure it's ok
		case 1:
			*DNN_PG_1 = 0x000007ff;
			*DNN_CTRL_1 = 0;
		case 2:
			*DNN_PG_2 = 0x000007ff;
			*DNN_CTRL_2 = 0;
		case 3:
			*DNN_PG_3 = 0x000007ff;
			*DNN_CTRL_3 = 0;
  }

  // fetch first word
	switch (PE) {
	  case 0:
	  	*DNN_CTRL_0 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	  	break;
	  case 1:
	  	*DNN_CTRL_1 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	  	break;
	  case 2:
	  	*DNN_CTRL_2 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	  	break;
	  case 3:
	  	*DNN_CTRL_3 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	  	break;
	  default: break;		// TODO: report fail 
	}
  delay(READ_SRAM_DELAY);
	uint32_t y0, y1, y2, y3;
	switch (PE) {
	  case 0:
	  	y3 = *DNN_R_DATA_0_3; 
	  	y2 = *DNN_R_DATA_0_2; 
	  	y1 = *DNN_R_DATA_0_1; 
	  	y0 = *DNN_R_DATA_0_0; 
	  	break;
	  case 1:
	  	y3 = *DNN_R_DATA_1_3;  
	  	y2 = *DNN_R_DATA_1_2;  
	  	y1 = *DNN_R_DATA_1_1;  
	  	y0 = *DNN_R_DATA_1_0;  
	  	break;
	  case 2:
	  	y3 = *DNN_R_DATA_2_3;   
	  	y2 = *DNN_R_DATA_2_2;   
	  	y1 = *DNN_R_DATA_2_1;   
	  	y0 = *DNN_R_DATA_2_0;   
	  	break;
	  case 3:
	  	y3 = *DNN_R_DATA_3_3;   
	  	y2 = *DNN_R_DATA_3_2;   
	  	y1 = *DNN_R_DATA_3_1;   
	  	y0 = *DNN_R_DATA_3_0;   
	  	break;
	  default: break;		// TODO: report fail 
	}

  temp_addr = 0;
	for (addr = short_address + 1; addr < length + short_address; addr++) {
    // fetch current word
	  switch (PE) {
	    case 0:
	    	*DNN_CTRL_0 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	    	break;
	    case 1:
	    	*DNN_CTRL_1 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	    	break;
	    case 2:
	    	*DNN_CTRL_2 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	    	break;
	    case 3:
	    	*DNN_CTRL_3 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	    	break;
	    default: break;		// TODO: report fail 
	  }
    // process previous word
	  data[temp_addr]   = (y0 & 0xfff);
	  data[temp_addr+1] = (y0 >> 12) & 0xfff; 
	  data[temp_addr+2] = (y1 & 0xfff);
	  data[temp_addr+3] = (y1 >> 12) & 0xfff; 
	  data[temp_addr+4] = (y2 & 0xfff);
	  data[temp_addr+5] = (y2 >> 12) & 0xfff; 
	  data[temp_addr+6] = (y3 & 0xfff);
	  data[temp_addr+7] = (y3 >> 12) & 0xfff; 
    temp_addr += 8;
    // fetch current word
	  switch (PE) {
		  case 0:
		  	y3 = *DNN_R_DATA_0_3; 
		  	y2 = *DNN_R_DATA_0_2; 
		  	y1 = *DNN_R_DATA_0_1; 
		  	y0 = *DNN_R_DATA_0_0; 
		  	break;
		  case 1:
		  	y3 = *DNN_R_DATA_1_3;  
		  	y2 = *DNN_R_DATA_1_2;  
		  	y1 = *DNN_R_DATA_1_1;  
		  	y0 = *DNN_R_DATA_1_0;  
		  	break;
		  case 2:
		  	y3 = *DNN_R_DATA_2_3;   
		  	y2 = *DNN_R_DATA_2_2;   
		  	y1 = *DNN_R_DATA_2_1;   
		  	y0 = *DNN_R_DATA_2_0;   
		  	break;
		  case 3:
		  	y3 = *DNN_R_DATA_3_3;   
		  	y2 = *DNN_R_DATA_3_2;   
		  	y1 = *DNN_R_DATA_3_1;   
		  	y0 = *DNN_R_DATA_3_0;   
		  	break;
	    default: break;		// TODO: report fail 
	  }
	}

  // process last word
	data[temp_addr]   = (y0 & 0xfff);
	data[temp_addr+1] = (y0 >> 12) & 0xfff; 
	data[temp_addr+2] = (y1 & 0xfff);
	data[temp_addr+3] = (y1 >> 12) & 0xfff; 
	data[temp_addr+4] = (y2 & 0xfff);
	data[temp_addr+5] = (y2 >> 12) & 0xfff; 
	data[temp_addr+6] = (y3 & 0xfff);
	data[temp_addr+7] = (y3 >> 12) & 0xfff; 
}

void read_dnn_sram_6_fft_output(uint16_t start_address, int32_t* data_r, int32_t* data_i) {
	uint16_t addr, temp_addr;
	uint8_t PE = start_address >> 13;
	uint16_t short_address = start_address & 0x1fff;
	switch (PE) {
		case 0:
			*DNN_PG_0 = 0x000007ff;		// only last 11 bits are set
			*DNN_CTRL_0 = 0;  // TODO: make sure it's ok
		case 1:
			*DNN_PG_1 = 0x000007ff;
			*DNN_CTRL_1 = 0;
		case 2:
			*DNN_PG_2 = 0x000007ff;
			*DNN_CTRL_2 = 0;
		case 3:
			*DNN_PG_3 = 0x000007ff;
			*DNN_CTRL_3 = 0;
  }

  // fetch first word
	switch (PE) {
	  case 0:
	  	*DNN_CTRL_0 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	  	break;
	  case 1:
	  	*DNN_CTRL_1 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	  	break;
	  case 2:
	  	*DNN_CTRL_2 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	  	break;
	  case 3:
	  	*DNN_CTRL_3 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	  	break;
	  default: break;		// TODO: report fail 
	}
  delay(READ_SRAM_DELAY);
	uint32_t y0, y1, y2, y3;
	switch (PE) {
	  case 0:
	  	y3 = *DNN_R_DATA_0_3; 
	  	y2 = *DNN_R_DATA_0_2; 
	  	y1 = *DNN_R_DATA_0_1; 
	  	y0 = *DNN_R_DATA_0_0; 
	  	break;
	  case 1:
	  	y3 = *DNN_R_DATA_1_3;  
	  	y2 = *DNN_R_DATA_1_2;  
	  	y1 = *DNN_R_DATA_1_1;  
	  	y0 = *DNN_R_DATA_1_0;  
	  	break;
	  case 2:
	  	y3 = *DNN_R_DATA_2_3;   
	  	y2 = *DNN_R_DATA_2_2;   
	  	y1 = *DNN_R_DATA_2_1;   
	  	y0 = *DNN_R_DATA_2_0;   
	  	break;
	  case 3:
	  	y3 = *DNN_R_DATA_3_3;   
	  	y2 = *DNN_R_DATA_3_2;   
	  	y1 = *DNN_R_DATA_3_1;   
	  	y0 = *DNN_R_DATA_3_0;   
	  	break;
	  default: break;		// TODO: report fail 
	}

  temp_addr = 0;
	for (addr = short_address + 1; addr < 32 + short_address; addr++) {
    // fetch current word
	  switch (PE) {
	    case 0:
	    	*DNN_CTRL_0 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	    	break;
	    case 1:
	    	*DNN_CTRL_1 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	    	break;
	    case 2:
	    	*DNN_CTRL_2 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	    	break;
	    case 3:
	    	*DNN_CTRL_3 = (short_address << 2) + 0b01;	// set CPU_ARB_DATA_RD to 1
	    	break;
	    default: break;		// TODO: report fail 
	  }
    // process previous word
	  data_r[temp_addr]   = (y0 & 0777);
	  data_i[temp_addr]   = (y0 >> 6) & 0777; 
	  data_r[temp_addr+1] = (y0 >> 12) & 0777;
	  data_i[temp_addr+1] = (y0 >> 18) & 0777; 
	  data_r[temp_addr+2] = (y1 & 0777);
	  data_i[temp_addr+2] = (y1 >> 6) & 0777; 
	  data_r[temp_addr+3] = (y1 >> 12) & 0777;
	  data_i[temp_addr+3] = (y1 >> 18) & 0777; 
	  data_r[temp_addr+4] = (y2 & 0777);
	  data_i[temp_addr+4] = (y2 >> 6) & 0777; 
	  data_r[temp_addr+5] = (y2 >> 12) & 0777;
	  data_i[temp_addr+5] = (y2 >> 18) & 0777; 
	  data_r[temp_addr+6] = (y3 & 0777);
	  data_i[temp_addr+6] = (y3 >> 6) & 0777; 
	  data_r[temp_addr+7] = (y3 >> 12) & 0777;
	  data_i[temp_addr+7] = (y3 >> 18) & 0777; 
    temp_addr += 8;
    // fetch current word
	  switch (PE) {
		  case 0:
		  	y3 = *DNN_R_DATA_0_3; 
		  	y2 = *DNN_R_DATA_0_2; 
		  	y1 = *DNN_R_DATA_0_1; 
		  	y0 = *DNN_R_DATA_0_0; 
		  	break;
		  case 1:
		  	y3 = *DNN_R_DATA_1_3;  
		  	y2 = *DNN_R_DATA_1_2;  
		  	y1 = *DNN_R_DATA_1_1;  
		  	y0 = *DNN_R_DATA_1_0;  
		  	break;
		  case 2:
		  	y3 = *DNN_R_DATA_2_3;   
		  	y2 = *DNN_R_DATA_2_2;   
		  	y1 = *DNN_R_DATA_2_1;   
		  	y0 = *DNN_R_DATA_2_0;   
		  	break;
		  case 3:
		  	y3 = *DNN_R_DATA_3_3;   
		  	y2 = *DNN_R_DATA_3_2;   
		  	y1 = *DNN_R_DATA_3_1;   
		  	y0 = *DNN_R_DATA_3_0;   
		  	break;
	    default: break;		// TODO: report fail 
	  }
	}

  // process last word
	data_r[temp_addr]   = (y0 & 0777);
	data_i[temp_addr]   = (y0 >> 6) & 0777; 
	data_r[temp_addr+1] = (y0 >> 12) & 0777;
	data_i[temp_addr+1] = (y0 >> 18) & 0777; 
	data_r[temp_addr+2] = (y1 & 0777);
	data_i[temp_addr+2] = (y1 >> 6) & 0777; 
	data_r[temp_addr+3] = (y1 >> 12) & 0777;
	data_i[temp_addr+3] = (y1 >> 18) & 0777; 
	data_r[temp_addr+4] = (y2 & 0777);
	data_i[temp_addr+4] = (y2 >> 6) & 0777; 
	data_r[temp_addr+5] = (y2 >> 12) & 0777;
	data_i[temp_addr+5] = (y2 >> 18) & 0777; 
	data_r[temp_addr+6] = (y3 & 0777);
	data_i[temp_addr+6] = (y3 >> 6) & 0777; 
	data_r[temp_addr+7] = (y3 >> 12) & 0777;
	data_i[temp_addr+7] = (y3 >> 18) & 0777; 
}

void write_dnn_sram(uint16_t start_address, int32_t* data, uint16_t length) {
	uint16_t addr, temp_addr;
	uint8_t PE = start_address >> 13;
	uint16_t short_address = start_address & 0x1fff;
	switch (PE) {
		case 0:
			*DNN_PG_0 = 0x000007ff;		// only last 11 bits are set
//			*DNN_CTRL_0 &= 0xffff8000;  
			*DNN_CTRL_0 = 0;  // TODO: make sure it's ok
		case 1:
			*DNN_PG_1 = 0x000007ff;
			*DNN_CTRL_1 = 0;
		case 2:
			*DNN_PG_2 = 0x000007ff;
			*DNN_CTRL_2 = 0;
		case 3:
			*DNN_PG_3 = 0x000007ff;
			*DNN_CTRL_3 = 0;
  }
  temp_addr = 0;
	for (addr = short_address; addr < length + short_address; addr++) {
    int32_t x0 = data[temp_addr];
    int32_t x1 = data[temp_addr+1];
    int32_t x2 = data[temp_addr+2];
    temp_addr += 3;
	  uint32_t y0 = x0 & 0xffffff;																				// x0 last 24 bits
	  uint32_t y1 = ((x1 & 0xffff) << 8) + ((x0 >> 24) & 0x000000ff);			// x1 last 16 bits + x0 first 8 bits. the last AND op is to avoid x0 being signed 
	  uint32_t y2 = ((x2 & 0xff) << 16) + ((x1 >> 16) & 0x0000ffff);	  	// x2 last 8 bits + x1 first 16 bits 
	  uint32_t y3 = (x2 >> 8) & 0x00ffffff; 															// x2 first 24 bits
	  switch (PE) {
	  	case 0:
	  		*DNN_DATA_0_3   = y3;
	  		*DNN_DATA_0_2   = y2;
	  		*DNN_DATA_0_1   = y1;
	  		*DNN_DATA_0_0   = y0;

	  		*DNN_CTRL_0 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 1:
	  		*DNN_DATA_1_3   = y3;
	  		*DNN_DATA_1_2   = y2;
	  		*DNN_DATA_1_1   = y1;
	  		*DNN_DATA_1_0   = y0;

	  		*DNN_CTRL_1 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 2:
	  		*DNN_DATA_2_3   = y3;
	  		*DNN_DATA_2_2   = y2;
	  		*DNN_DATA_2_1   = y1;
	  		*DNN_DATA_2_0   = y0;

	  		*DNN_CTRL_2 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 3:
	  		*DNN_DATA_3_3   = y3;
	  		*DNN_DATA_3_2   = y2;
	  		*DNN_DATA_3_1   = y1;
	  		*DNN_DATA_3_0   = y0;

	  		*DNN_CTRL_3 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	    default: break;		// TODO: report fail 
	  }
	}
}

void write_dnn_sram_16(uint16_t start_address, int16_t* data, uint16_t length) {
	uint16_t addr, temp_addr;
	uint8_t PE = start_address >> 13;
	uint16_t short_address = start_address & 0x1fff;
	switch (PE) {
		case 0:
			*DNN_PG_0 = 0x000007ff;		// only last 11 bits are set
//			*DNN_CTRL_0 &= 0xffff8000;  
			*DNN_CTRL_0 = 0;  // TODO: make sure it's ok
		case 1:
			*DNN_PG_1 = 0x000007ff;
			*DNN_CTRL_1 = 0;
		case 2:
			*DNN_PG_2 = 0x000007ff;
			*DNN_CTRL_2 = 0;
		case 3:
			*DNN_PG_3 = 0x000007ff;
			*DNN_CTRL_3 = 0;
  }
  temp_addr = 0;
	for (addr = short_address; addr < length + short_address; addr++) {
    int32_t x0 = data[temp_addr];
    int32_t x1 = data[temp_addr+1];
    int32_t x2 = data[temp_addr+2];
    int32_t x3 = data[temp_addr+3];
    int32_t x4 = data[temp_addr+4];
    int32_t x5 = data[temp_addr+5];
    temp_addr += 6;
	  uint32_t y0 = (x0 & 0xffff) + ((x1 & 0xff) << 24);
	  uint32_t y1 = ((x1 >> 8) & 0xff) + ((x2 & 0xffff) << 8);
	  uint32_t y2 = (x3 & 0xffff) + ((x4 & 0xff) << 24);
	  uint32_t y3 = ((x4 >> 8) & 0xff) + ((x5 & 0xffff) << 8);
	  switch (PE) {
	  	case 0:
	  		*DNN_DATA_0_3   = y3;
	  		*DNN_DATA_0_2   = y2;
	  		*DNN_DATA_0_1   = y1;
	  		*DNN_DATA_0_0   = y0;

	  		*DNN_CTRL_0 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 1:
	  		*DNN_DATA_1_3   = y3;
	  		*DNN_DATA_1_2   = y2;
	  		*DNN_DATA_1_1   = y1;
	  		*DNN_DATA_1_0   = y0;

	  		*DNN_CTRL_1 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 2:
	  		*DNN_DATA_2_3   = y3;
	  		*DNN_DATA_2_2   = y2;
	  		*DNN_DATA_2_1   = y1;
	  		*DNN_DATA_2_0   = y0;

	  		*DNN_CTRL_2 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 3:
	  		*DNN_DATA_3_3   = y3;
	  		*DNN_DATA_3_2   = y2;
	  		*DNN_DATA_3_1   = y1;
	  		*DNN_DATA_3_0   = y0;

	  		*DNN_CTRL_3 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	    default: break;		// TODO: report fail 
	  }
	}
}

void write_dnn_sram_16_fft_input(uint16_t start_address, int16_t* data, uint16_t length) {
/* // incorrect
	uint16_t addr, temp_addr;
	uint8_t PE = start_address >> 13;
	uint16_t short_address = start_address & 0x1fff;
	switch (PE) {
		case 0:
			*DNN_PG_0 = 0x000007ff;		// only last 11 bits are set
//			*DNN_CTRL_0 &= 0xffff8000;  
			*DNN_CTRL_0 = 0;  // TODO: make sure it's ok
	  	*DNN_DATA_0_3 = 0;
	  	*DNN_DATA_0_2 = 0;
		case 1:
			*DNN_PG_1 = 0x000007ff;
			*DNN_CTRL_1 = 0;
	  	*DNN_DATA_1_3 = 0;
	  	*DNN_DATA_1_2 = 0;
		case 2:
			*DNN_PG_2 = 0x000007ff;
			*DNN_CTRL_2 = 0;
	  	*DNN_DATA_2_3 = 0;
	  	*DNN_DATA_2_2 = 0;
		case 3:
			*DNN_PG_3 = 0x000007ff;
			*DNN_CTRL_3 = 0;
	  	*DNN_DATA_3_3 = 0;
	  	*DNN_DATA_3_2 = 0;
  }
  temp_addr = 0;
	for (addr = short_address; addr < length + short_address; addr++) {
    int32_t x0 = data[temp_addr];
    int32_t x2 = data[temp_addr+1];
    temp_addr += 2;
	  uint32_t y0 = x0 & 0xffff;
	  uint32_t y1 = (x2 & 0xffff) << 8;
	  switch (PE) {
	  	case 0:
	  		*DNN_DATA_0_1   = y1;
	  		*DNN_DATA_0_0   = y0;

	  		*DNN_CTRL_0 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 1:
	  		*DNN_DATA_1_1   = y1;
	  		*DNN_DATA_1_0   = y0;

	  		*DNN_CTRL_1 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 2:
	  		*DNN_DATA_2_1   = y1;
	  		*DNN_DATA_2_0   = y0;

	  		*DNN_CTRL_2 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 3:
	  		*DNN_DATA_3_1   = y1;
	  		*DNN_DATA_3_0   = y0;

	  		*DNN_CTRL_3 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	    default: break;		// TODO: report fail 
	  }
	}
*/
}

void write_dnn_sram_12_fft_input(uint16_t start_address, int16_t* data) {
/* // incorrect: refer to write_dnn_sram_6_fft_input
	uint16_t addr, temp_addr;
	uint8_t PE = start_address >> 13;
	uint16_t short_address = start_address & 0x1fff;
	switch (PE) {
		case 0:
			*DNN_PG_0 = 0x000007ff;		// only last 11 bits are set
//			*DNN_CTRL_0 &= 0xffff8000;  
			*DNN_CTRL_0 = 0;  // TODO: make sure it's ok
	  	*DNN_DATA_0_3 = 0;
	  	*DNN_DATA_0_2 = 0;
		case 1:
			*DNN_PG_1 = 0x000007ff;
			*DNN_CTRL_1 = 0;
	  	*DNN_DATA_1_3 = 0;
	  	*DNN_DATA_1_2 = 0;
		case 2:
			*DNN_PG_2 = 0x000007ff;
			*DNN_CTRL_2 = 0;
	  	*DNN_DATA_2_3 = 0;
	  	*DNN_DATA_2_2 = 0;
		case 3:
			*DNN_PG_3 = 0x000007ff;
			*DNN_CTRL_3 = 0;
	  	*DNN_DATA_3_3 = 0;
	  	*DNN_DATA_3_2 = 0;
  }
  temp_addr = 0;
	for (addr = short_address; addr < 50 + short_address; addr++) {
  //
    uint16_t x0 = (data[temp_addr]   & 0xfff);
    uint16_t x1 = (data[temp_addr+1] & 0xfff);
    uint16_t x2 = (data[temp_addr+2] & 0xfff);
    uint16_t x3 = (data[temp_addr+3] & 0xfff);
    temp_addr += 4;
	  switch (PE) {
	  	case 0:
	  		*DNN_DATA_0_3  = x3;
	  		*DNN_DATA_0_2  = x2;
	  		*DNN_DATA_0_1  = x1;
	  		*DNN_DATA_0_0  = x0;

	  		*DNN_CTRL_0 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 1:
	  		*DNN_DATA_1_3  = x3;
	  		*DNN_DATA_1_2  = x2;
	  		*DNN_DATA_1_1  = x1;
	  		*DNN_DATA_1_0  = x0;

	  		*DNN_CTRL_1 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 2:
	  		*DNN_DATA_2_3  = x3;
	  		*DNN_DATA_2_2  = x2;
	  		*DNN_DATA_2_1  = x1;
	  		*DNN_DATA_2_0  = x0;

	  		*DNN_CTRL_2 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 3:
	  		*DNN_DATA_3_3  = x3;
	  		*DNN_DATA_3_2  = x2;
	  		*DNN_DATA_3_1  = x1;
	  		*DNN_DATA_3_0  = x0;

	  		*DNN_CTRL_3 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	    default: break;		// TODO: report fail 
  //
	  }
	}
	for (addr = 50 + short_address; addr < 128; addr++) {
	  switch (PE) {
	  	case 0:
	  		*DNN_DATA_0_3 = 0;
	  		*DNN_DATA_0_2 = 0;
	  		*DNN_DATA_0_1 = 0;
	  		*DNN_DATA_0_0 = 0;

	  		*DNN_CTRL_0 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 1:
	  		*DNN_DATA_1_3 = 0;
	  		*DNN_DATA_1_2 = 0;
	  		*DNN_DATA_1_1 = 0;
	  		*DNN_DATA_1_0 = 0;

	  		*DNN_CTRL_1 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 2:
	  		*DNN_DATA_2_3 = 0;
	  		*DNN_DATA_2_2 = 0;
	  		*DNN_DATA_2_1 = 0;
	  		*DNN_DATA_2_0 = 0;

	  		*DNN_CTRL_2 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 3:
	  		*DNN_DATA_3_3 = 0;
	  		*DNN_DATA_3_2 = 0;
	  		*DNN_DATA_3_1 = 0;
	  		*DNN_DATA_3_0 = 0;

	  		*DNN_CTRL_3 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	    default: break;		// TODO: report fail 
  //
	  }
  }
*/
}

// length is fixed
void write_dnn_sram_6_fft_input(uint16_t start_address, int32_t* data_r, int32_t* data_i) {
	uint16_t addr, temp_addr;
	uint8_t PE = start_address >> 13;
	uint16_t short_address = start_address & 0x1fff;
	switch (PE) {
		case 0:
			*DNN_PG_0 = 0x000007ff;		// only last 11 bits are set
			*DNN_CTRL_0 = 0;  // TODO: make sure it's ok
		case 1:
			*DNN_PG_1 = 0x000007ff;
			*DNN_CTRL_1 = 0;
		case 2:
			*DNN_PG_2 = 0x000007ff;
			*DNN_CTRL_2 = 0;
		case 3:
			*DNN_PG_3 = 0x000007ff;
			*DNN_CTRL_3 = 0;
  }
  temp_addr = 0;
	for (addr = short_address; addr < 64 + short_address; addr++) {
    uint32_t x0 = (data_r[temp_addr]    & 077) + ((data_r[temp_addr+1] & 077) << 12)
                + ((data_i[temp_addr]   & 077) << 6) + ((data_i[temp_addr+1] & 077) << 18);
    uint32_t x1 = (data_r[temp_addr+2]  & 077) + ((data_r[temp_addr+3] & 077) << 12)
                + ((data_i[temp_addr+2] & 077) << 6) + ((data_i[temp_addr+3] & 077) << 18);
    uint32_t x2 = (data_r[temp_addr+4]  & 077) + ((data_r[temp_addr+5] & 077) << 12)
                + ((data_i[temp_addr+4] & 077) << 6) + ((data_i[temp_addr+5] & 077) << 18);
    uint32_t x3 = (data_r[temp_addr+6]  & 077) + ((data_r[temp_addr+7] & 077) << 12)
                + ((data_i[temp_addr+6] & 077) << 6) + ((data_i[temp_addr+7] & 077) << 18);
    temp_addr += 8;
	  switch (PE) {
	  	case 0:
	  		*DNN_DATA_0_3 = x3;
	  		*DNN_DATA_0_2 = x2;
	  		*DNN_DATA_0_1 = x1;
	  		*DNN_DATA_0_0 = x0;

	  		*DNN_CTRL_0 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 1:
	  		*DNN_DATA_1_3 = x3;
	  		*DNN_DATA_1_2 = x2;
	  		*DNN_DATA_1_1 = x1;
	  		*DNN_DATA_1_0 = x0;

	  		*DNN_CTRL_1 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 2:
	  		*DNN_DATA_2_3 = x3;
	  		*DNN_DATA_2_2 = x2;
	  		*DNN_DATA_2_1 = x1;
	  		*DNN_DATA_2_0 = x0;

	  		*DNN_CTRL_2 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 3:
	  		*DNN_DATA_3_3 = x3;
	  		*DNN_DATA_3_2 = x2;
	  		*DNN_DATA_3_1 = x1;
	  		*DNN_DATA_3_0 = x0;

	  		*DNN_CTRL_3 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	    default: break;		// TODO: report fail 
	  }
	}
}

void write_dnn_sram_12_dnn_input(uint16_t start_address, int16_t* data, uint16_t length) {
	uint16_t addr, temp_addr;
	uint8_t PE = start_address >> 13;
	uint16_t short_address = start_address & 0x1fff;
	switch (PE) {
		case 0:
			*DNN_PG_0 = 0x000007ff;		// only last 11 bits are set
//			*DNN_CTRL_0 &= 0xffff8000;  
			*DNN_CTRL_0 = 0;  // TODO: make sure it's ok
		case 1:
			*DNN_PG_1 = 0x000007ff;
			*DNN_CTRL_1 = 0;
		case 2:
			*DNN_PG_2 = 0x000007ff;
			*DNN_CTRL_2 = 0;
		case 3:
			*DNN_PG_3 = 0x000007ff;
			*DNN_CTRL_3 = 0;
  }
  temp_addr = 0;
	for (addr = short_address; addr < length + short_address; addr++) {
  //
    int16_t x0 = data[temp_addr];
    int16_t x1 = data[temp_addr+1];
	  uint32_t y0 = (x0 & 0xfff) + ((x1 & 0xfff) << 12);
    temp_addr += 2;
    x0 = data[temp_addr];
    x1 = data[temp_addr+1];
	  uint32_t y1 = (x0 & 0xfff) + ((x1 & 0xfff) << 12);
    temp_addr += 2;
    x0 = data[temp_addr];
    x1 = data[temp_addr+1];
	  uint32_t y2 = (x0 & 0xfff) + ((x1 & 0xfff) << 12);
    temp_addr += 2;
    x0 = data[temp_addr];
    x1 = data[temp_addr+1];
	  uint32_t y3 = (x0 & 0xfff) + ((x1 & 0xfff) << 12);
    temp_addr += 2;
	  switch (PE) {
	  	case 0:
	  		*DNN_DATA_0_3   = y3;
	  		*DNN_DATA_0_2   = y2;
	  		*DNN_DATA_0_1   = y1;
	  		*DNN_DATA_0_0   = y0;

	  		*DNN_CTRL_0 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 1:
	  		*DNN_DATA_1_3   = y3;
	  		*DNN_DATA_1_2   = y2;
	  		*DNN_DATA_1_1   = y1;
	  		*DNN_DATA_1_0   = y0;

	  		*DNN_CTRL_1 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 2:
	  		*DNN_DATA_2_3   = y3;
	  		*DNN_DATA_2_2   = y2;
	  		*DNN_DATA_2_1   = y1;
	  		*DNN_DATA_2_0   = y0;

	  		*DNN_CTRL_2 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	  	case 3:
	  		*DNN_DATA_3_3   = y3;
	  		*DNN_DATA_3_2   = y2;
	  		*DNN_DATA_3_1   = y1;
	  		*DNN_DATA_3_0   = y0;

	  		*DNN_CTRL_3 = (addr << 2) + 0b10;			// set CPU_ARB_DATA_WR to 1
	  		break;
	    default: break;		// TODO: report fail 
  //
	  }
	}
}

#ifdef HAS_NLI
void write_nli(uint16_t inst_no, uint8_t PE) {
	uint8_t addr;
  uint32_t temp_DNN_NLI_CTRL;

	for (addr = 0; addr < 10; addr++) {
	  if (addr == 0) {
	  	temp_DNN_NLI_CTRL = (0 & ~(0b1111 << 1));	// NLI_INST_ADDR = 0b0000
	    temp_DNN_NLI_CTRL |= 1;			        // load: NLI_WR = 1
    }	else {
	  	temp_DNN_NLI_CTRL += (1 << 1);			// NLI_INST_ADDR += 1 
    }

	  switch (PE) {
	  	case 0:  
	  		*DNN_NLI_A_0 = NLI_A[inst_no][PE][addr];
	  		*DNN_NLI_X_0 = NLI_X[inst_no][PE][addr]; 
	  		*DNN_NLI_B_0_0 = cut_24word(NLI_B[inst_no][PE][addr], 0); 
	  		*DNN_NLI_B_0_1 = cut_24word(NLI_B[inst_no][PE][addr], 1); 
	  		*DNN_NLI_CTRL_0 = temp_DNN_NLI_CTRL;
	  		break;
	  	case 1:  
	  		*DNN_NLI_A_1 = NLI_A[inst_no][PE][addr]; 
	  		*DNN_NLI_X_1 = NLI_X[inst_no][PE][addr]; 
	  		*DNN_NLI_B_1_0 = cut_24word(NLI_B[inst_no][PE][addr], 0); 
	  		*DNN_NLI_B_1_1 = cut_24word(NLI_B[inst_no][PE][addr], 1); 
	  		*DNN_NLI_CTRL_1 = temp_DNN_NLI_CTRL;
	  		break;
	  	case 2:  
	  		*DNN_NLI_A_2 = NLI_A[inst_no][PE][addr]; 
	  		*DNN_NLI_X_2 = NLI_X[inst_no][PE][addr]; 
	  		*DNN_NLI_B_2_0 = cut_24word(NLI_B[inst_no][PE][addr], 0); 
	  		*DNN_NLI_B_2_1 = cut_24word(NLI_B[inst_no][PE][addr], 1); 
	  		*DNN_NLI_CTRL_2 = temp_DNN_NLI_CTRL;
	  		break;
	  	case 3:  
	  		*DNN_NLI_A_3 = NLI_A[inst_no][PE][addr]; 
	  		*DNN_NLI_X_3 = NLI_X[inst_no][PE][addr]; 
	  		*DNN_NLI_B_3_0 = cut_24word(NLI_B[inst_no][PE][addr], 0); 
	  		*DNN_NLI_B_3_1 = cut_24word(NLI_B[inst_no][PE][addr], 1); 
	  		*DNN_NLI_CTRL_3 = temp_DNN_NLI_CTRL;
	  	  break;
	  	default: break;		// TODO: report fail 
	  }
//    delay(WRITE_NLI_DELAY);
	}
	switch (PE) {
    case 0: 
  	  *DNN_NLI_X_0 = NLI_X[inst_no][PE][10]; 
		 	*DNN_NLI_CTRL_0 = 0x00000015;	// NLI_INST_ADDR = 0b1010; NLI_WR = 1
    case 1: 
  	  *DNN_NLI_X_1 = NLI_X[inst_no][PE][10]; 
		 	*DNN_NLI_CTRL_1 = 0x00000015;	// NLI_INST_ADDR = 0b1010; NLI_WR = 1
    case 2: 
  	  *DNN_NLI_X_2 = NLI_X[inst_no][PE][10]; 
		 	*DNN_NLI_CTRL_2 = 0x00000015;	// NLI_INST_ADDR = 0b1010; NLI_WR = 1
    case 3: 
  	  *DNN_NLI_X_3 = NLI_X[inst_no][PE][10]; 
		 	*DNN_NLI_CTRL_3 = 0x00000015;	// NLI_INST_ADDR = 0b01010; NLI_WR = 1
	 	default: break;		// TODO: report fail 
  }
//    delay(WRITE_NLI_DELAY);
}
#else
void write_nli(uint16_t inst_no, uint8_t PE) {
	// empty
}
#endif

void clock_gate() {
  //delay(5);
  *REG_CG |= 1;
  //delay(5);
}

void clock_gate_dlc() {
  delay(5);
  *REG_CG |= (1 << 1); 
  delay(5);
}

//TODO
void signal_done() {
  delay(5);
  *DNN_DONE |= 0x1; 
  delay(5);
}

void signal_debug(uint16_t pe_num) {
  delay(5);
  *DNN_DONE |= 0x1 << (pe_num + 1); 
  delay(5);
}

/*
void read_dnn_sram(uint32_t cnt){
	*TIMERWD_GO  = 0x0;
	*TIMERWD_CNT = cnt;
	*TIMERWD_GO  = 0x1;
}

void disable_timerwd(){
	*TIMERWD_GO  = 0x0;
}
*/

//**************************************************
// DNN IRQ SETTING
//**************************************************
void send_dnn_irq() {	
    *DNN_IRQ_2 = 0x45001040;
    *DNN_IRQ_1 = 0x0;//unused
    *DNN_IRQ_0 = 0x0;//unused 

    *DNN_IRQ_CTRL = 0x11;
}

//**************************************************
// M0 IRQ SETTING
//**************************************************
void enable_all_irq() { *NVIC_ICPR = 0xFFFFFFFF; *NVIC_ISER = 0xFFFFFFFF; }
void disable_all_irq() { *NVIC_ICPR = 0xFFFFFFFF; *NVIC_ICER = 0xFFFFFFFF; }
void clear_all_pend_irq() { *NVIC_ICPR = 0xFFFFFFFF; }
void enable_reg_irq() {	*NVIC_ICPR = 0xFFFFFFFF; *NVIC_ISER = 0xFF<<2; }


//**************************************************
// MBUS IRQ SETTING
//**************************************************
void set_halt_until_reg(uint8_t reg_id) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (((uint32_t) reg_id) << 12);
    *REG_IRQ_CTRL = reg_val;
}

void set_halt_until_mem_wr(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MEM_WR << 12);
    *REG_IRQ_CTRL = reg_val;
}

void set_halt_until_mbus_rx(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MBUS_RX << 12);
    *REG_IRQ_CTRL = reg_val;
}

void set_halt_until_mbus_tx(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MBUS_TX << 12);
    *REG_IRQ_CTRL = reg_val;
}

void set_halt_until_mbus_fwd(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_UNTIL_MBUS_FWD << 12);
    *REG_IRQ_CTRL = reg_val;
}

void set_halt_disable(void) {
    uint32_t reg_val = (*REG_IRQ_CTRL) & 0xFFFF0FFF;
    reg_val = reg_val | (HALT_DISABLE << 12);
    *REG_IRQ_CTRL = reg_val;
}

void halt_cpu (void) {
    *SYS_CTRL_REG_ADDR = SYS_CTRL_CMD_HALT_CPU;
}

uint8_t get_current_halt_config(void) {
    uint32_t reg_ = *REG_IRQ_CTRL;
    reg_ = (0x0000F000 & reg_) >> 12;
    return (uint8_t) reg_;
}

void set_halt_config(uint8_t new_config) {
    uint32_t reg_ = *REG_IRQ_CTRL;
    reg_ = (0x00010000 & reg_); // reset
    reg_ = reg_ | (((uint32_t) new_config) << 12);
    *REG_IRQ_CTRL = reg_;
}

void enable_old_msg_irq(void) {
    uint32_t reg_ = *REG_IRQ_CTRL;
    reg_ = (0x0000F000 & reg_); // reset
    reg_ = reg_ | (0x1 << 16);
    *REG_IRQ_CTRL = reg_;
}

void disable_old_msg_irq(void) {
    uint32_t reg_ = *REG_IRQ_CTRL;
    reg_ = (0x0000F000 & reg_); // reset
    *REG_IRQ_CTRL = reg_;
}


//*******************************************************************
// VERIOLG SIM DEBUG PURPOSE ONLY!!
//*******************************************************************
void arb_debug_reg (uint32_t code) { *((volatile uint32_t *) 0xAFFFFFF8) = code; }


