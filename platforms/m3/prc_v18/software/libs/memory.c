//
// Created by whatsthenext on 2019/5/27.
//
#include <stdio.h>
#include <stdint.h>
#include "memory.h"
#include "mbus.h"

#define DEBUG_MBUS_MSG_2

static uint16_t prev_temp;
static uint16_t prev_pressure;
static uint16_t data_set1;// one word in memory is composed by two data sets, representing two different Temperature and Pressure value sets
static uint8_t data_set1_occupied;
static uint32_t store_type;// 1 represent store the data every word, 2 represent store the data every two words, default: interval will be 8192 to preserve the previous data



void initital_memory(uint16_t temp, uint16_t pressure, uint32_t data_storage_count){
    uint32_t send_out_data;
    prev_temp = temp;
    prev_pressure = pressure;
    data_set1_occupied = 0;
    store_type = 1;
    send_out_data = (pressure << 16 | temp);
    #ifdef DEBUG_MBUS_MSG_2
    mbus_write_message32(0xE0, store_type);
    #endif
    mbus_copy_mem_from_local_to_remote_bulk(0x7, (uint32_t*)((data_storage_count>>1)<<2), (uint32_t*)&store_type, 0);// the first word is used to store storage_type, the second word is for the first measurement result
    #ifdef DEBUG_MBUS_MSG_2
    mbus_write_message32(0xE1, send_out_data);
    #endif
    mbus_copy_mem_from_local_to_remote_bulk(0x7, (uint32_t*)(((data_storage_count + 1)>>1)<<2), (uint32_t*)&send_out_data, 0);// 0x7 is defined in as the starting place of memory
}

uint8_t record_value(uint16_t temp, uint16_t pressure, uint32_t data_storage_count){
    uint32_t send_out_data;
    uint8_t send = 0;
    uint16_t new_temp_delta  = (temp - prev_temp)&0x1FF;// use 9 bits to represent temperature difference
    uint8_t new_pressure_delta = (pressure - prev_pressure)&0b01111111;// use 7 bits to represent pressure difference
    uint16_t new_data_set = new_pressure_delta << 9 | (new_temp_delta & 0x1FF);
    if(!data_set1_occupied){
        data_set1 = new_data_set;
        data_set1_occupied = 1;
    } else{
        send_out_data = data_set1 << 16 | new_data_set;
        #ifdef DEBUG_MBUS_MSG_2
        mbus_write_message32(0xEE, send_out_data);
        #endif
        mbus_copy_mem_from_local_to_remote_bulk(0x7, (uint32_t*)((data_storage_count>>1)<<2), (uint32_t*)&send_out_data, 0);
        send = 1;
        data_set1_occupied = 0;
    }

    // update the record temperature and pressure
    if(new_temp_delta >> 8) prev_temp -= ((~new_temp_delta + 1)&0x1ff);
    else prev_temp += (new_temp_delta & 0x1ff);
    if(new_pressure_delta >> 6) prev_pressure -= ((~new_pressure_delta + 1)&0x7f);
    else prev_pressure += (new_pressure_delta & 0x7f);
    return send;
}

uint32_t clean_temp_storage( uint32_t data_storage_count){ // return the place of latest data
    uint32_t send_out_data;
    if(data_set1_occupied){
        send_out_data = data_set1 << 16;
        #ifdef DEBUG_MBUS_MSG_2
        mbus_write_message32(0xEE, send_out_data);
        #endif
        mbus_copy_mem_from_local_to_remote_bulk(0x7, (uint32_t*)((data_storage_count>>1)<<2), (uint32_t*)&send_out_data, 0);
        return data_storage_count;
    }
    return data_storage_count - 1;
}

uint32_t check_storage_type(){
    return store_type;
}

void change_storage_type(){
    switch (store_type){
        case 1:
            store_type = 2;
            break;
        case 2:
            store_type = 8192;
            break;
        default:
            store_type = 8192;
            break;
    }
}
