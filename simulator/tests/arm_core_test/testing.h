//------------------------------------------------------------------------------
// testing.h
//
// Branden Ghena (brghena@umich.edu)
// 07/15/2013
//
// Header file for Cortex M0 processor testing
//------------------------------------------------------------------------------

#ifndef TESTING_H
#define TESTING_H

#define STACK_TOP 0x00001FFC
#define HEAP_ADDR 0x00001FD0

void initial_instruction_test(void);
void alu_instruction_test(void);
void memory_instruction_test(void);
void branch_instruction_test(void);
void misc_instruction_test(void);
void sync_instruction_test(void);

#endif /* TESTING_H */
