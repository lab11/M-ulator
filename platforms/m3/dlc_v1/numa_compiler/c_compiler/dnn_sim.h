#ifndef __DNN_SIM_H__
#define __DNN_SIM_H__

#include <string>
#include <iostream>

#include "common.h"
#include "config.h"
#include "operations.h"

class dnn_sim {

public:
  dnn_sim(dnn_config* config);
  ~dnn_sim();
  
  void load_inputs(string input_file);
  string print_instructions();
  string print_test_case();     // inputs and outputs for each operation
  string print_memory();        // print non_volatile_space (space for parameters, does not change during runtime)
  void print_stat();
 
private:
  void assign_all_inputs();
  void assign_all_parameters();
  int max_volatile_space;         // non-parameter space (temp + inputs + outputs)
  int max_space;                  // total SRAM space (4PE)
  float* get_dct2dnn_outputs();
  float* dct2dnn_inputs;

  dnn_config* m_config;
// todo: change to lists/vectors, like "operations* m_operations";
  fft_op* m_fft_op;
  mel_op* m_mel_op;
  log_op* m_log_op;
  dct_op* m_dct_op;
  dnn_op* m_dnn_op;
};

#endif
