//
// Created by whatsthenext on 2019/5/27.
//

#ifndef PSTACK_ONDEMAND_V6_0_C_MEMORY_H
#define PSTACK_ONDEMAND_V6_0_C_MEMORY_H



uint32_t check_memory_initial();
void initital_memory(uint16_t temp, uint16_t pressure, uint32_t data_storage_count);
uint8_t record_value(uint16_t temp, uint16_t pressure,  uint32_t data_storage_count);// return true if it send the data to the memory
uint32_t clean_temp_storage( uint32_t data_storage_count); // send the remaining temp value to the memory, return the final data_storeage_count value. Called before sending the data via the radio
uint32_t check_storage_type();
void change_storage_type();


#endif //PSTACK_ONDEMAND_V6_0_C_MEMORY_H
