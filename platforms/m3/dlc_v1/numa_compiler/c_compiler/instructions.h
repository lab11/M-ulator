#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__

#include "common.h"
#include "config.h"
#include <string>

class instruction {
public:
  virtual string print_binary_inputs(string *memory);
  virtual string print_binary_outputs(string *memory);

  int inputs_start;
  int inputs_size;
  bool inputs_signed;
  int inputs_precision;
  int inputs_int_bits;
  int outputs_start;
  int outputs_size;
  bool outputs_signed;
  int outputs_precision;
  int outputs_int_bits;

  dnn_config* m_config;
  float* inputs;
  float* outputs;
  float* parameters;
}; 

class mac_inst : public instruction {    //(matrix vector multiplication)
public:
  ~mac_inst();
  string print_instruction();
  void generate_outputs();
  void print_binary_data(string *memory);
  int get_tiling_parameters(dnn_config* m_config);
  void get_mac_row_number(dnn_config* m_config);

	int temp_inputs_size;				// equal to inputs_size
  int temp_outputs_start;    
  int temp_outputs_size;    
  bool temp_outputs_signed;
  int temp_outputs_precision;
  int temp_outputs_int_bits;
  bool temp_inputs_first;   // if temp_inputs are put in nearby or temp_outputs

  int parameters_start;
  int parameters_size;
  bool parameters_signed;
  int parameters_precision;
  int parameters_int_bits;

  bool do_relu;
  bool is_dnn;              // is this mac an DNN classification layer
  bool do_saturation;       // [always true] does this mac do saturation (or overflow)
	bool use_nli;
	int mac_row_number;

  int shift;           			// how the temp output gets truncated (shifted to left)
  int non_linear_shift;
	int offsets_shift;

  float *offsets;
	int offsets_start;
  bool offsets_signed;
  int offsets_precision; 
  int offsets_int_bits; 

  int tiling_row;
  int tiling_column;
};

class fft_inst : public instruction {
public:
  ~fft_inst();
  string print_instruction();
  void generate_outputs();
  void print_binary_data(string *memory);
  string print_binary_inputs(string *memory);
  string print_binary_outputs(string *memory);
  int inputs_space;
  int outputs_space;
  int parameters_space;

//  int temp1_size;    
//  int temp2_size;    
//  bool temp1_first;
  bool do_relu;						// hard coded to 0

  int twiddle_start;  
  int twiddle_size;  
  int twiddle_precision;
  int twiddle_int_bits;

  int fft_points; 
  int fft_number; 
  int input_unit_size;
  int output_unit_size;

  int shift;
};

class non_linear_inst : public instruction {
public:
  ~non_linear_inst();
  string print_instruction();
  void generate_outputs();

  bool do_relu;
	int shift;
};

class mov_inst : public instruction { 
public:
  ~mov_inst();
  string print_instruction();
  void generate_outputs();

  bool do_relu;
};

#endif
