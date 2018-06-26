#ifndef __OPERATIONS_H__
#define __OPERATIONS_H__

#include "common.h"
#include "instructions.h"
#include <string>
#include <cmath>

#define HZ2MEL(x) (1127.0*log(1+(x/700.0)))
#define MEL2HZ(x) (700.0*(exp(x/1127.0)-1))

// space: in words, size: in elements
// generate_instruction: new instruction objects, assign sizes, etc. the addresses are not assigned here but later (by update_inputs_address, etc.)

class operation {
public:
//  virtual ~operation();
  int max_temp_space; // temp space in words 
//  int max_volatile_space; // in words (temps + input + output)

  virtual void update_inputs_address(int start_addr);
  virtual void update_outputs_address(int start_addr);
  virtual void update_parameter_address(int start_addr);
  virtual void update_inputs(float *new_inputs);
//  virtual string print_instructions() = 0;
  virtual void print_stat();
//  virtual void print_binary_data(string* memory);
//  virtual void print_binary_inputs(string* memory);
//  virtual void print_binary_outputs(string* memory);
  void print_outputs();
  void print_outputs(int range);

//private:
  dnn_config* m_config;
//  int num_of_insts;
//  instruction* insts; 

  int inputs_start;
  int inputs_size;
  int inputs_precision;
  bool inputs_signed;
  int inputs_int_bits;
  int outputs_start;
  int outputs_size;
  int outputs_precision;
  bool outputs_signed;
  int outputs_int_bits;
  int parameters_start;
  int parameters_size;
  int parameters_precision;
  bool parameters_signed;
  int parameters_int_bits;
  float* inputs;
  float* outputs;
  float* parameters;
}; 

class fft_op : public operation {
public:
  fft_op(dnn_config* config); 
  ~fft_op();

  void print_stat();
  string print_binary_inputs(string* memory);
  string print_binary_outputs(string* memory);
  void print_binary_data(string* memory); // parameters
  void update_inputs_address(int start_addr);
  void update_outputs_address(int start_addr);
  void update_parameter_address(int start_addr);
  void update_inputs(string input_file);
  void update_inputs(float *new_inputs){operation::update_inputs(new_inputs);};
  void generate_outputs();
  string print_instructions();
  int inputs_space;       // memory space for inputs in words
  int outputs_space;      // memory space for outputs in words
  int parameters_space;   // memory space for parameters in words

private:
  void generate_instructions();
  void generate_parameters();
  void bit_reverse(int fftsize, float* inputs);

  fft_inst* ffts;
//  non_linear_inst* get_magnitude;
  int fft_size;
  int num_of_ffts;            
//  int total_temp1_size;
//  int total_temp2_size;
  int fft_min_points;
  int inputs_int_bits;
};

class mel_op : public operation {
public:
  mel_op(dnn_config* config);  
  ~mel_op();

  void print_stat();
  void print_binary_data(string* memory);// parameters
  string print_binary_outputs(string* memory);
  void update_inputs_address(int start_addr);
  void update_outputs_address(int start_addr);
  void update_parameter_address(int start_addr);
  void generate_outputs();
  string print_instructions();

//private:
  void generate_instructions();
  void generate_parameter();

  mac_inst* mac;

  float minmel;
  float maxmel;
  int samprate;
};

class log_op : public operation {
public:
  log_op(dnn_config* config);  
  ~log_op();

  void print_stat();
  string print_binary_outputs(string* memory);
//  string print_binary_data(); //log has no parameter
  void update_inputs_address(int start_addr);
//  void update_outputs_address(int start_addr){}; // output address is input address
  void generate_outputs();
  string print_instructions();

//private:
  void generate_instructions();

  non_linear_inst* non_linear;
};

class dct_op : public operation {
public:
  dct_op(dnn_config* config);  
  ~dct_op();

  void print_stat();
  void print_binary_data(string* memory);// parameters
  string print_binary_outputs(string* memory);
  void update_inputs_address(int start_addr);
  void update_outputs_address(int start_addr);
  void update_parameter_address(int start_addr);
  void generate_outputs();
  string print_instructions();

//private:
  void generate_instructions();
  void generate_parameter();

  mac_inst* mac;
};

class dnn_op : public operation {
public:
  dnn_op(dnn_config* config);
  ~dnn_op();

  string print_instructions();
  void print_stat();
  void print_binary_data(string* memory);// parameters
  string print_binary_inputs(string* memory);
  string print_binary_outputs(string* memory);
  void update_inputs_address(int start_addr);
  void update_parameter_address(int start_addr);
  void update_inputs(string input_file);
  void update_inputs(float *new_inputs){operation::update_inputs(new_inputs);};
  void update_parameter(string weights_file);
  void generate_outputs();
  int parameters_space; // including offsets

//private:
  void generate_instructions();

  mac_inst* layers;
  non_linear_inst* activations;
  int num_of_layers;
  int max_volatile_space; // in words (temps + input + output)
//  int inputs_size;
//  int* outpus_size;
//  int* neuron_size;
//  int* weights_size;
//  int* weights_precision;
//  int* neuron_precision;

  float** neurons;
  float** weights;
  float** offsets;
};

#endif
