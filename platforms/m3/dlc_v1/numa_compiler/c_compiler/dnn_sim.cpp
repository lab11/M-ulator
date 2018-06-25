#include "dnn_sim.h"

///////////////////////////
dnn_sim::dnn_sim(dnn_config* config) : m_config(config) {
  if (m_config->do_precompute) {
    m_fft_op = new fft_op(m_config);
    m_mel_op = new mel_op(m_config);
    m_log_op = new log_op(m_config);
    m_dct_op = new dct_op(m_config);
  }
  m_dnn_op = new dnn_op(m_config);

  max_space = (m_config->L1_size + m_config->L2_size + m_config->L3_size + m_config->L4_size) * 4 * 1024 * 8 / WORD_SIZE; 
  SHOW_D(max_space, );
  assign_all_inputs();
  assign_all_parameters();
}

dnn_sim::~dnn_sim() {
  if (m_config->do_precompute) {
    delete m_fft_op; 
    delete m_mel_op; 
    delete m_log_op; 
    delete m_dct_op; 
  }
  delete m_dnn_op;
  delete[] dct2dnn_inputs;
}

float* dnn_sim::get_dct2dnn_outputs() {
  int i = 0, j = 0; 
  dct2dnn_inputs = new float[m_dnn_op->inputs_size];
  for (; i < m_dnn_op->inputs_size; i++) { 
    dct2dnn_inputs[i] = m_dct_op->outputs[j];
    if (j == m_dct_op->outputs_size)
      j = 0;
  }
  return dct2dnn_inputs;
}

///////////////////////////
void dnn_sim::load_inputs(string input_file) {
  if (m_config->do_precompute) {
    m_fft_op->update_inputs(input_file);
    m_fft_op->generate_outputs();
//    m_fft_op->print_outputs(20);
    m_mel_op->update_inputs(m_fft_op->outputs);
    m_mel_op->generate_outputs();
//    m_mel_op->print_outputs(20);
    m_log_op->update_inputs(m_mel_op->outputs);
    m_log_op->generate_outputs();
//    m_log_op->print_outputs(20);
    m_dct_op->update_inputs(m_log_op->outputs);
    m_dct_op->generate_outputs();
//    m_dct_op->print_outputs(20);
    m_dnn_op->update_inputs(get_dct2dnn_outputs());
    m_dnn_op->generate_outputs();
//    m_dnn_op->print_outputs(20);
  }
  else {
    m_dnn_op->update_inputs(input_file);
    m_dnn_op->generate_outputs();
  }
}

string dnn_sim::print_instructions() {
  string instructions = "";

  if (m_config->do_precompute) {
    instructions += m_fft_op->print_instructions();
    instructions += m_mel_op->print_instructions();
    instructions += m_log_op->print_instructions();
    instructions += m_dct_op->print_instructions();
  }
  instructions += m_dnn_op->print_instructions();
  
  return instructions;
}

string dnn_sim::print_memory() {
  string memory((max_space * WORD_SIZE), '0');
  string* memory_ptr = &memory; 

  if (m_config->do_precompute) {
    m_fft_op->print_binary_data(memory_ptr);
    m_mel_op->print_binary_data(memory_ptr);
//    m_log_op->print_memory(); // log does not have parameters
    m_dct_op->print_binary_data(memory_ptr);
  }
  m_dnn_op->print_binary_data(memory_ptr);

/*
  memory = memory.substr(max_volatile_space * WORD_SIZE, (max_space - max_volatile_space) * WORD_SIZE); 
    // only return non volatile part of memory
  memory = "## end address: " + to_string(max_space) + "\n" + memory;
  memory = "## start address: " + to_string(max_space - max_volatile_space) + "\n" + memory;
*/
//  memory = memory.substr(0, (max_space - max_volatile_space) * WORD_SIZE - 1);
  memory = "## end address: " + to_string(max_space - max_volatile_space - 1) + "\n" + memory;
  memory = "## start address: 0\n" + memory;

  return memory;
}

string dnn_sim::print_test_case() {
  string result;
  string test_case((max_space * WORD_SIZE), '0');
  string* test_case_ptr = &test_case;

  if (m_config->do_precompute) {
    result += m_fft_op->print_binary_inputs(test_case_ptr);
    result += m_fft_op->print_binary_outputs(test_case_ptr);
    result += m_mel_op->print_binary_outputs(test_case_ptr);
    result += m_log_op->print_binary_outputs(test_case_ptr);
    result += m_dct_op->print_binary_outputs(test_case_ptr);
  } else {
    result += m_dnn_op->print_binary_inputs(test_case_ptr);
  } 
  result += m_dnn_op->print_binary_outputs(test_case_ptr);

/*
  result = "## end address: " + to_string(max_volatile_space) + "\n" + result;
  result = "## start address: 0\n" + result;
*/
  result = "## end address: " + to_string(max_space - 1) + "\n" + result;
  result = "## start address: " + to_string(max_space - max_volatile_space) + "\n" + result;

  return result;
}

void dnn_sim::print_stat() {
  if (m_config->do_precompute) {
    m_fft_op->print_stat();
    m_mel_op->print_stat();
    m_log_op->print_stat();
    m_dct_op->print_stat();
  }
  m_dnn_op->print_stat();
}

///////////////////////////
void dnn_sim::assign_all_inputs(){
  // TODO: find the most energy-efficient way to assign inputs
  // now just assign by sequence, ignoring memory movement
  // TODO: parallel DNN
  max_volatile_space = 0;   // non-parameter space
 
  if (m_config->do_precompute) {
    int work_space_addr;
    // FFT
    m_fft_op->update_inputs_address(m_fft_op->max_temp_space);
    work_space_addr = m_fft_op->inputs_start + m_fft_op->inputs_space; 
    m_fft_op->update_outputs_address(work_space_addr);
    max_volatile_space = max(max_volatile_space, m_fft_op->outputs_start + m_fft_op->outputs_space);
    // ARM does the complex -> real here before Mel, so everything before can be overwritten
    // Mel
    m_mel_op->update_inputs_address(m_mel_op->max_temp_space);

    // Then, LOG does not have extra output space so Mel output would be DCT input space.
    work_space_addr = m_mel_op->inputs_start + TO_WORD_SIZE(m_mel_op->inputs_size * m_mel_op->inputs_precision);
    max_volatile_space = max(max_volatile_space, max(m_dct_op->max_temp_space, work_space_addr));
    if (m_dct_op->max_temp_space > work_space_addr) {
      m_mel_op->update_outputs_address(m_dct_op->max_temp_space); 
      m_log_op->update_inputs_address(m_dct_op->max_temp_space);
      m_dct_op->update_inputs_address(m_dct_op->max_temp_space);
    } else {
      m_mel_op->update_outputs_address(work_space_addr);
      m_log_op->update_inputs_address(work_space_addr);
      m_dct_op->update_inputs_address(work_space_addr);
    }
    // TODO: output for Mel can be closer
    max_volatile_space = max(m_mel_op->outputs_start + TO_WORD_SIZE(m_mel_op->outputs_size * m_mel_op->outputs_precision), max_volatile_space);

    // Then, DCT output is DNN layer1 inputs.
    // other layers of DNN is handled by dnn_op->update_neuron_address().
    mac_inst* temp_mac = &(m_dnn_op->layers[0]);
    int temp_space = TO_WORD_SIZE(temp_mac->temp_inputs_size * temp_mac->inputs_precision + temp_mac->temp_outputs_size * temp_mac->temp_outputs_precision);
    work_space_addr = m_dct_op->inputs_start + TO_WORD_SIZE(m_dct_op->inputs_size * m_dct_op->inputs_precision);
    max_volatile_space = max(max_volatile_space, max(temp_space, work_space_addr));
    if (temp_space > work_space_addr) {
      m_dct_op->update_outputs_address(temp_space);
      m_dnn_op->update_inputs_address(temp_space);
    } else {
      m_dct_op->update_outputs_address(work_space_addr);
      m_dnn_op->update_inputs_address(work_space_addr);
    }
    // m_dnn_op->update_inputs_address() should finish address assignment in DNNs
    max_volatile_space = max(m_dnn_op->max_volatile_space, max_volatile_space);
  } else {  // no preprocessing
    m_dnn_op->update_inputs_address(m_dnn_op->max_temp_space);
    // m_dnn_op->update_inputs_address() should finish address assignment in DNNs
    max_volatile_space = max(m_dnn_op->max_volatile_space, max_volatile_space);
  }
}

void dnn_sim::assign_all_parameters(){
  int address = max_volatile_space;
  // parameters
  if (m_config->do_precompute) {
    m_fft_op->update_parameter_address(address); 
    address += m_fft_op->parameters_space; 
    m_mel_op->update_parameter_address(address); 
    address += TO_WORD_SIZE(m_mel_op->parameters_size * m_mel_op->parameters_precision);
    m_dct_op->update_parameter_address(address);
    address += TO_WORD_SIZE(m_dct_op->parameters_size * m_dct_op->parameters_precision);
    SHOW_D(address, address after precompute weights);
  }
  m_dnn_op->update_parameter_address(address); 
  address += m_dnn_op->parameters_space;
  SHOW_D(address, address after DNN weights);
  if (max_space - address < 0) {
    cout<<"space not enough when assigning parameters"<<endl;
    exit(1);
  }
}

