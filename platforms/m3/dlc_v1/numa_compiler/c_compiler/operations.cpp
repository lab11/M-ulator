#include "operations.h"
#include "fixed.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <typeinfo>

void operation::update_inputs_address(int start_addr) {
  inputs_start = start_addr;
}

void operation::update_outputs_address(int start_addr) {
  outputs_start = start_addr;
}

void operation::update_parameter_address(int start_addr) {
  parameters_start = start_addr;
}

void operation::print_outputs() {
  cout << "## " + string(typeid(*this).name()) + " output ##" << endl;
  for (int i = 0; i < outputs_size; i++)
    cout << outputs[i] << '\t'; 
  cout<<endl;
}

void operation::print_outputs(int num) {
  cout << "## " + string(typeid(*this).name()) + " output ##" << endl;
  int range = (num > outputs_size) ? outputs_size : num;
  for (int i = 0; i < range; i++)
    cout << outputs[i] << '\t'; 
  cout<<endl;
}

void operation::update_inputs(float* new_inputs){
  if (inputs == NULL) {
    cout<<"update inputs error: new inputs is NULL"<<endl; 
    exit(0);
  }
  if (new_inputs == NULL) {
    cout<<"update inputs error: new inputs is NULL"<<endl; 
    exit(0);
  }
  for (int i = 0; i < inputs_size; i++)
    inputs[i] = new_inputs[i]; 
}

void operation::print_stat(){
  cout << "## " + string(typeid(*this).name()) + " operation ##" << endl;
  SHOW(inputs_size, );
  SHOW(inputs_start, );
  SHOW(outputs_size, );
  SHOW(outputs_start, );
  SHOW(parameters_size, );
  SHOW(parameters_start, );
}

/////////////////////////////
// FFT
fft_op::fft_op(dnn_config* config){
  m_config = config;
  inputs_precision = m_config->fft_inputs_precision;
  inputs_int_bits = m_config->fft_inputs_int_bits;
	inputs_signed = true;
  outputs_precision = inputs_precision;
  outputs_int_bits = inputs_int_bits;
	outputs_signed = true;
  parameters_precision = m_config->fft_twiddle_precision;
  parameters_int_bits = 1;
	parameters_signed = true;
  fft_min_points = m_config->fft_min_points;

  fft_size = m_config->fft_inputs_size;
  inputs_size = pow(2, int(ceil(log(fft_size) / log(2))));
      // closest 2's power
  outputs_size = inputs_size;
  parameters_size = 0;
  for (int i = fft_min_points; i <= inputs_size; i *= 2)
    parameters_size += i / 2;    // parameters are duplicated; see generate_instructions()
  max_temp_space = 0;
      // total tempoary space needed, in words

  num_of_ffts = int(log(inputs_size / fft_min_points) / log(2)) + 1; 

  inputs_space = TO_WORD_SIZE(2 * inputs_size * inputs_precision);
  if ( (inputs_precision == 8) || (inputs_precision == 16) ) 
    inputs_space = inputs_space * 4 / 3;      // zero padding
  outputs_space = inputs_space;

  ffts = new fft_inst[num_of_ffts];
  generate_instructions();

  parameters_space = 0;
  for (int level = 0; level < num_of_ffts; level++)
    parameters_space += (ffts+level)->parameters_space; 
}

fft_op::~fft_op(){
  delete[] ffts->inputs;
  for (int level = 0; level < num_of_ffts; level++)
    delete[] (ffts+level)->outputs;
  delete[] parameters;
  
  delete[] ffts;
}

string fft_op::print_instructions() {
  string instructions = "## " + string(typeid(*this).name()) + " instructions ##\n";
  for (int inst = 0; inst < num_of_ffts; inst++)
    instructions += (ffts + inst)->print_instruction() + '\n';
  return instructions;
}

void fft_op::generate_instructions(){
  int fft_points = fft_min_points;
  int twiddle_size = fft_min_points / 2;
  for (int level = 0; level < num_of_ffts; level++) {
    (ffts+level)->m_config = m_config;
    (ffts+level)->inputs = new float[inputs_size * 2];
    if (level != 0)
      (ffts+level-1)->outputs = (ffts+level)->inputs; 
    (ffts+level)->inputs_size = inputs_size;
    (ffts+level)->inputs_precision = inputs_precision;
    (ffts+level)->outputs_size = inputs_size;
    (ffts+level)->outputs_precision = inputs_precision;
    (ffts+level)->twiddle_precision = parameters_precision;
    (ffts+level)->twiddle_int_bits = parameters_int_bits;
    (ffts+level)->twiddle_size = twiddle_size; 
    (ffts+level)->fft_points = fft_points;
    (ffts+level)->fft_number = inputs_size / fft_points;
    (ffts+level)->inputs_space = inputs_space;
    (ffts+level)->outputs_space = inputs_space;
    (ffts+level)->parameters_space = TO_WORD_SIZE(2 * twiddle_size * parameters_precision); 
    (ffts+level)->do_relu = false; 

    (ffts+level)->input_unit_size = TO_WORD_SIZE(inputs_size * inputs_precision) / (ffts+level)->fft_number;
    (ffts+level)->output_unit_size = TO_WORD_SIZE(outputs_size * outputs_precision) / (ffts+level)->fft_number;
  
    (ffts+level)->shift = inputs_precision - inputs_int_bits - inputs_signed 
				+ parameters_precision - parameters_int_bits - parameters_signed
        - (outputs_precision - outputs_int_bits - outputs_signed);

    fft_points *= 2;
    twiddle_size *= 2;
  }

  (ffts + num_of_ffts - 1)->outputs = new float[inputs_size * 2]; 
  inputs = ffts->inputs;
  outputs = (ffts + num_of_ffts - 1)->outputs;

  parameters = new float[parameters_size * 2];
  int pointer = 0;
  fft_points = fft_min_points;
  for (int level = 0; level < num_of_ffts; level++) {
    (ffts+level)->parameters = parameters + pointer; 
    for (int i=0; i<fft_points/2; i++) {
      float twidr = cos(-2*M_PI*i/fft_points);
      float twidi = sin(-2*M_PI*i/fft_points);
      parameters[pointer++] = float2fixed(twidr, parameters_int_bits, parameters_precision);    // int bits
      parameters[pointer++] = float2fixed(twidi, parameters_int_bits, parameters_precision);    // int bits
    }
    fft_points *= 2;
  }
}

void fft_op::update_inputs_address(int start_addr){
  inputs_start = start_addr;
  for (int level = 0; level < num_of_ffts; level++)
    (ffts+level)->inputs_start = inputs_start;
}

void fft_op::update_outputs_address(int start_addr){
  outputs_start = start_addr;
  for (int level = 0; level < num_of_ffts - 1; level++)
    (ffts+level)->outputs_start = inputs_start;
  (ffts+num_of_ffts-1)->outputs_start = outputs_start;
}

void fft_op::update_parameter_address(int start_addr){
  operation::update_parameter_address(start_addr);
  int pointer = start_addr;
  for (int level = 0; level < num_of_ffts; level++) {
    (ffts+level)->twiddle_start = pointer;
    pointer += (ffts+level)->parameters_space; 
  }
}

void fft_op::update_inputs(string inputs_file){
  // check file
  ifstream file(inputs_file.c_str());
  if (!file.is_open()) {  
    cout << "error reading input file " << inputs_file << endl;
    exit(1);
  }
  // check inputs size
  int temp_inputs_size;
  file >> temp_inputs_size;
  if ((temp_inputs_size != inputs_size) && (temp_inputs_size != fft_size)) {
    cout << "input file: inputs_size (first line) not match with config file" << endl;
    exit(1);
  }

  if (inputs == NULL) {
    cout << "fft inputs not allocated" << endl;
    exit(0);
  }
  for (int i = 0; i < inputs_size * 2; i++) {
    float temp;
    file >> temp;
    if (i < fft_size * 2)
      inputs[i] = float2fixed(temp, inputs_int_bits, inputs_precision);  
    else
      inputs[i] = 0;
  }

  bit_reverse(inputs_size, inputs);   // TODO: inputs_size or fft_size? inputs_size in mohit's code
}

//TODO: not verified
void fft_op::bit_reverse(int fftsize, float* inputs){
  int j = 0;
  int m;
  float tempr, tempi;
  for (int i=0; i<fftsize-1; i++) {
    if(i<j) {
      tempr = inputs[2 * j];
      inputs[2 * j] = inputs[2 * i];
      inputs[2 * i] = tempr;
      tempi = inputs[2 * j + 1];
      inputs[2 * j + 1] = inputs[2 * i + 1];
      inputs[2 * i + 1] = tempi;
    }
    m = fftsize/2;
    while(m<=j) {
      j -= m;
      m /= 2;
    }
    j += m;
  }
}

void fft_op::generate_outputs(){
  for (int i = 0; i < num_of_ffts; i++)
    (ffts+i)->generate_outputs();
}

void fft_op::print_stat(){
  operation::print_stat();
  SHOW(inputs_precision, );
  SHOW(parameters_precision, );
  SHOW(num_of_ffts, );
  SHOW(fft_min_points, );
}

void fft_op::print_binary_data(string *memory){
  for (int i = 0; i < num_of_ffts; i++)
    (ffts + i)->print_binary_data(memory);
}

string fft_op::print_binary_inputs(string* memory){
  return "\nfft_inputs\n" + (ffts)->print_binary_inputs(memory);
}

string fft_op::print_binary_outputs(string* memory){
  string result;
  for (int i = 0; i < num_of_ffts; i++) {
    result += "\nfft_outputs_level" + to_string(i) + "\n" + (ffts + i)->print_binary_outputs(memory);
  }
  return result;
}

/////////////////////////////
// mel-scale frequency filter
mel_op::mel_op(dnn_config* config){
  m_config = config;
  minmel = HZ2MEL(m_config->mel_filter_minhz);
  maxmel = HZ2MEL(m_config->mel_filter_maxhz);
  samprate = m_config->sample_rate;

  inputs_size = m_config->fft_inputs_size / 2 + 1;
  outputs_size = m_config->mel_filter_num;
  parameters_size = inputs_size * outputs_size;

  inputs_precision = m_config->fft_inputs_precision;
  outputs_precision = m_config->fft_inputs_precision;
  parameters_precision = m_config->mel_parameter_precision;

  mac = new mac_inst;
  generate_instructions();
  generate_parameter();

  max_temp_space = TO_WORD_SIZE(mac->temp_inputs_size * mac->inputs_precision + mac->temp_outputs_size * mac->temp_outputs_precision);
}

mel_op::~mel_op(){
  delete mac;
}

string mel_op::print_instructions() {
  string instructions = "## " + string(typeid(*this).name()) + " instructions ##\n";
  instructions += mac->print_instruction() + '\n';
  return instructions;
}

void mel_op::print_stat(){
  operation::print_stat();
  SHOW(inputs_precision, );
  SHOW(outputs_precision, );
  SHOW(parameters_precision, );
}

void mel_op::update_inputs_address(int start_addr){
  inputs_start = start_addr;
  mac->inputs_start = inputs_start;
	mac->temp_outputs_start = TO_WORD_SIZE(mac->inputs_size * mac->inputs_precision);		// TODO
}

void mel_op::update_outputs_address(int start_addr){
  outputs_start = start_addr;
  mac->outputs_start = outputs_start;
}

void mel_op::update_parameter_address(int start_addr){
  parameters_start = start_addr;
  mac->parameters_start = parameters_start;
  mac->offsets_start = 0;
}

void mel_op::generate_instructions(){
  mac->m_config = m_config;
  mac->inputs_signed = false;
  mac->inputs_size = inputs_size;
  mac->inputs_precision = inputs_precision;
  mac->inputs_int_bits = 3; // TODO

  mac->outputs_signed = false;
  mac->outputs_size = outputs_size;
  mac->outputs_precision = outputs_precision;
  mac->outputs_int_bits = 5; // TODO

  mac->parameters_precision = parameters_precision;
  mac->parameters_size = mac->inputs_size * mac->outputs_size;
  mac->parameters_signed = false;
  mac->parameters_int_bits = 1; // TODO

  mac->do_saturation = true;
  mac->shift = mac->inputs_precision - mac->inputs_int_bits - mac->inputs_signed
                + mac->parameters_int_bits - mac->parameters_int_bits - mac->parameters_signed
                - (mac->outputs_precision - mac->outputs_int_bits - mac->outputs_signed);
              // input_frac_bit + para_frac_bit - output_frac_bit
  mac->offsets_shift = 0; 
  mac->non_linear_shift = 0; 

  mac->is_dnn = false;
	mac->do_relu = false;
	mac->use_nli = false;

  mac->temp_outputs_precision = 16; 
  mac->temp_outputs_signed = true;
  mac->temp_outputs_int_bits = 8;

  mac->get_tiling_parameters(m_config); 
  mac->get_mac_row_number(m_config); 

  mac->inputs = new float[inputs_size];
  mac->outputs = new float[outputs_size];
  mac->parameters = new float[parameters_size];
  inputs = mac->inputs;
  outputs = mac->outputs;
  parameters = mac->parameters;
}

void mel_op::generate_parameter(){
  int specsize = inputs_size;
  int nfilters = outputs_size;
  int fftsize = (inputs_size - 1) * 2;
  
  float filtcents[nfilters];
  float filtwidth = (maxmel-minmel)/(nfilters+1);
  for (int i=0; i<nfilters+2; i++) {
    float tmp = minmel + (filtwidth*i);
    filtcents[i] = MEL2HZ(tmp);
  }
  float filtvals[specsize];
  for (int i=0; i<specsize; i++) {
    filtvals[i] = ((float) i)*samprate/fftsize;
  }
  float fbankmatrix[specsize*nfilters];
  for (int i=0; i<nfilters; i++) {
    for (int j=0; j<specsize; j++) {
      if(filtvals[j]>=filtcents[i] && filtvals[j]<filtcents[i+1])
	      fbankmatrix[(i*specsize)+j] = (filtvals[j]-filtcents[i])/(filtcents[i+1]-filtcents[i]);
      else if(filtvals[j]>=filtcents[i+1] && filtvals[j]<filtcents[i+2])
	      fbankmatrix[(i*specsize)+j] = (filtvals[j]-filtcents[i+2])/(filtcents[i+1]-filtcents[i+2]);
      else
	      fbankmatrix[(i*specsize)+j] = 0;
    }
  }

  for (int i = 0; i < specsize*nfilters; i++)
    parameters[i] = float2fixed(fbankmatrix[i], 1, parameters_precision); // TODO
}


void mel_op::print_binary_data(string* memory){
  mac->print_binary_data(memory);
}

void mel_op::generate_outputs(){
  mac->generate_outputs();
}                                                                  

string mel_op::print_binary_outputs(string* memory){
  return "\nmel_outputs\n" + mac->print_binary_outputs(memory);
}

/////////////////////////////
// log
log_op::log_op(dnn_config* config){
  m_config = config;
  inputs_size = m_config->mel_filter_num;
  inputs_precision = m_config->fft_inputs_precision;
  outputs_size = inputs_size;

  non_linear = new non_linear_inst;
  generate_instructions();

  max_temp_space = 0;
}

log_op::~log_op(){
  delete non_linear;
}

string log_op::print_instructions() {
  string instructions = "## " + string(typeid(*this).name()) + " instructions ##\n";
  instructions += non_linear->print_instruction() + '\n';
  return instructions;
}

void log_op::generate_instructions(){
  non_linear->m_config = m_config;
  non_linear->inputs_size = inputs_size;
  non_linear->inputs_precision = inputs_precision;
  non_linear->inputs_signed = true;
  non_linear->inputs_int_bits = 5;

  non_linear->outputs_size = inputs_size;
  non_linear->outputs_precision = inputs_precision;
  non_linear->outputs_signed = true;
  non_linear->outputs_int_bits = 3;

	non_linear->do_relu = false;
	non_linear->shift = 10;			// TODO

  non_linear->inputs = new float[inputs_size];
  non_linear->outputs = new float[outputs_size];
//  non_linear->parameters = new float[parameters_size];  // TODO
  inputs = non_linear->inputs;
  outputs = non_linear->outputs;
}

void log_op::update_inputs_address(int start_addr){
  inputs_start = start_addr;
  non_linear->inputs_start = inputs_start;
  // input and output share same address
  outputs_start = start_addr;
  non_linear->outputs_start = inputs_start;
}

void log_op::generate_outputs(){
  for (int i = 0; i < inputs_size; i++) {
    if (inputs[i] > 0) 
      outputs[i] = log(inputs[i]); // TODO: non-linear function
    else
      outputs[i] = 0; 
  }
}

void log_op::print_stat(){
  cout << "# log operation #" << endl;
  SHOW(inputs_size, );
  SHOW(inputs_precision, );
  SHOW(inputs_start, );
}

string log_op::print_binary_outputs(string* memory){
  return "\nlog_outputs\n" + non_linear->print_binary_outputs(memory);
}

/////////////////////////////
// dct
dct_op::dct_op(dnn_config* config){
  m_config = config;

  inputs_size = m_config->mel_filter_num;
  outputs_size = m_config->dct_coeff_num;
  parameters_size = inputs_size * outputs_size;

  inputs_precision = m_config->fft_inputs_precision;
  outputs_precision = m_config->fft_inputs_precision;
  parameters_precision = m_config->dct_parameter_precision;

  mac = new mac_inst;
  generate_instructions();
  generate_parameter();

  max_temp_space = TO_WORD_SIZE(mac->temp_inputs_size * mac->inputs_precision + mac->temp_outputs_size * mac->temp_outputs_precision);
}

dct_op::~dct_op(){
  delete mac;
}

string dct_op::print_instructions() {
  string instructions = "## " + string(typeid(*this).name()) + " instructions ##\n";
  instructions += mac->print_instruction() + '\n';
  return instructions;
}

void dct_op::print_stat(){
  operation::print_stat();
  SHOW(inputs_precision, );
  SHOW(parameters_precision, );
}

void dct_op::update_inputs_address(int start_addr){
  inputs_start = start_addr;
  mac->inputs_start = inputs_start;
	mac->temp_outputs_start = TO_WORD_SIZE(mac->inputs_size * mac->inputs_precision);		// TODO
}

void dct_op::update_outputs_address(int start_addr){
  outputs_start = start_addr;
  mac->outputs_start = outputs_start;
}

void dct_op::update_parameter_address(int start_addr){
  parameters_start = start_addr;
  mac->parameters_start = parameters_start;
  mac->offsets_start = 0;
}

void dct_op::generate_instructions(){
  mac->m_config = m_config;
  mac->inputs_signed = true;
  mac->inputs_size = inputs_size;
  mac->inputs_precision = inputs_precision;
  mac->inputs_int_bits = 4; // TODO

  mac->outputs_signed = true;
  mac->outputs_size = outputs_size;
  mac->outputs_precision = outputs_precision;
  mac->outputs_int_bits = 8; // TODO

  mac->parameters_precision = parameters_precision;
  mac->parameters_signed = true;
  mac->parameters_size = mac->inputs_size * mac->outputs_size;
  mac->parameters_int_bits = m_config->dct_parameter_int_bits; 

  mac->do_saturation = true;
  mac->shift = mac->inputs_precision - mac->inputs_int_bits - mac->inputs_signed
                + mac->parameters_int_bits - mac->parameters_int_bits - mac->parameters_signed
                - (mac->outputs_precision - mac->outputs_int_bits - mac->outputs_signed);
              // input_frac_bit + para_frac_bit - output_frac_bit
  mac->offsets_shift = 0; 
  mac->non_linear_shift = 0; 

  mac->is_dnn = false;
	mac->do_relu = false;
	mac->use_nli = false;

  mac->temp_outputs_precision = 16; 
  mac->temp_outputs_signed = true;
  mac->temp_outputs_int_bits = 12; // TODO
  mac->get_tiling_parameters(m_config); 
  mac->get_mac_row_number(m_config); 

  mac->inputs = new float[inputs_size];
  mac->outputs = new float[outputs_size];
  mac->parameters = new float[parameters_size];
  inputs = mac->inputs;
  outputs = mac->outputs;
  parameters = mac->parameters;
}

void dct_op::generate_parameter() {
  int ncoeff = outputs_size;
  int nfilters = inputs_size;

  for (int i=0; i<ncoeff; i++) {
    for (int j=0; j<nfilters; j++) {
      float temp = cos((i+1)*(M_PI/nfilters*(j+0.5))); 
      parameters[(i*nfilters)+j] = float2fixed(temp, m_config->dct_parameter_int_bits, parameters_precision);
    }
  }
}

void dct_op::generate_outputs(){
  mac->generate_outputs();
}

void dct_op::print_binary_data(string* memory){
  mac->print_binary_data(memory);
}

string dct_op::print_binary_outputs(string* memory){
  return "\ndct_outputs\n" + mac->print_binary_outputs(memory);
}
//////////////////////////////////////////////////////////
// dnn
dnn_op::dnn_op(dnn_config* config){
  m_config = config;

  num_of_layers = m_config->layer_num; 

  layers = new mac_inst[num_of_layers];

  generate_instructions();
  update_parameter(string(m_config->weight_file));
}

dnn_op::~dnn_op(){
  delete[] layers;
  for (int layer = 0; layer < num_of_layers; layer++) {
  	delete[] neurons[layer];
  	delete[] weights[layer];
  	delete[] offsets[layer];
	}
	delete[] neurons[num_of_layers];
  delete[] neurons;
  delete[] weights;
  delete[] offsets;
}

string dnn_op::print_instructions() {
  string instructions = "## " + string(typeid(*this).name()) + " instructions ##\n";
  for (int inst = 0; inst < num_of_layers; inst++)
    instructions += (layers + inst)->print_instruction() + '\n';
  return instructions;
}

void dnn_op::generate_instructions(){
  max_volatile_space = 0, parameters_size = 0, parameters_space = 0;

  // temp variables
  int layer;
  mac_inst* temp_mac;
  non_linear_inst* temp_non;
  int temp_space = 0;

  #define INSTANTIATE_LAYER(m, n) \
  temp_mac = layers + m - 1; \
  temp_mac->m_config = m_config;\
  temp_mac->inputs_size = m_config->l##m##_inputs; \
  temp_mac->inputs_precision = m_config->l##m##_neuron_precision; \
  temp_mac->inputs_signed = m_config->l##m##_neuron_signed; \
  temp_mac->inputs_int_bits = m_config->l##m##_neuron_int_bits; \
  \
  temp_mac->outputs_size = m_config->l##n##_inputs; \
  temp_mac->outputs_precision = m_config->l##n##_neuron_precision; \
  temp_mac->outputs_signed = m_config->l##n##_neuron_signed; \
  temp_mac->outputs_int_bits = m_config->l##n##_neuron_int_bits; \
  \
  temp_mac->parameters_precision = m_config->l##m##_weight_precision; \
  temp_mac->parameters_signed = m_config->l##m##_weight_signed; \
  temp_mac->parameters_int_bits = m_config->l##m##_weight_int_bits; \
  temp_mac->parameters_size = temp_mac->inputs_size * temp_mac->outputs_size; \
  \
  temp_mac->do_saturation = true; \
  temp_mac->is_dnn = true; \
  temp_mac->do_relu = m_config->l##m##_function == "relu" ? true : false; \
	temp_mac->use_nli = m_config->l##m##_function == "none"; \
  temp_mac->offsets_precision = m_config->l##m##_offset_precision; \
  temp_mac->offsets_signed = m_config->l##m##_offset_signed; \
  temp_mac->offsets_int_bits = m_config->l##m##_offset_int_bits; \
  \
  temp_mac->temp_outputs_precision = m_config->l##n##_tempout_precision; \
  temp_mac->temp_outputs_signed = m_config->l##n##_tempout_signed; \
  temp_mac->temp_outputs_int_bits = m_config->l##n##_tempout_int_bits; \
	\
  temp_mac->get_tiling_parameters(m_config); \
  temp_mac->get_mac_row_number(m_config); \
  \
  temp_mac->shift = temp_mac->inputs_precision - temp_mac->inputs_int_bits - temp_mac->inputs_signed\
                  + temp_mac->parameters_precision - temp_mac->parameters_int_bits - temp_mac->parameters_signed\
                  - (temp_mac->outputs_precision - temp_mac->outputs_int_bits - temp_mac->outputs_signed); \
  temp_mac->offsets_shift = 0; /*TODO*/\
  temp_mac->non_linear_shift = 10; /*TODO: make in configuartion*/\
  \
  temp_space = TO_WORD_SIZE( (temp_mac->temp_inputs_size + temp_mac->inputs_size) * temp_mac->inputs_precision \
                   + temp_mac->temp_outputs_size * temp_mac->temp_outputs_precision); \
  parameters_size += temp_mac->inputs_size * temp_mac->outputs_size; \
  parameters_space += TO_WORD_SIZE(temp_mac->inputs_size * temp_mac->outputs_size * temp_mac->parameters_precision); \
  parameters_space += TO_WORD_SIZE(temp_mac->outputs_size * temp_mac->offsets_precision); \

  neurons = new float*[num_of_layers + 1];
  weights = new float*[num_of_layers];
  offsets = new float*[num_of_layers];
  for (layer = 0; layer < num_of_layers; layer++) {
    switch (layer) {
      case 0: INSTANTIATE_LAYER(1, 2);  break;  
      case 1: INSTANTIATE_LAYER(2, 3);  break; 
      case 2: INSTANTIATE_LAYER(3, 4);  break; 
      case 3: INSTANTIATE_LAYER(4, 5);  break; 
      case 4: INSTANTIATE_LAYER(5, 6);  break; 
      case 5: INSTANTIATE_LAYER(6, 7);  break; 
      case 6: INSTANTIATE_LAYER(7, 8);  break; 
      case 7: INSTANTIATE_LAYER(8, 9);  break; 
      case 8: INSTANTIATE_LAYER(9, 10);  break; 
    }

    temp_mac = layers + layer;
    if (layer == 0)
      neurons[0] = new float[layers->inputs_size];     // layer 1 input neurons
    temp_mac->inputs = neurons[layer];
    neurons[layer + 1] = new float[temp_mac->outputs_size]; 
    temp_mac->outputs = neurons[layer + 1];
    weights[layer] = new float[temp_mac->parameters_size];
    temp_mac->parameters = weights[layer]; 
    offsets[layer] = new float[temp_mac->outputs_size];
    temp_mac->offsets = offsets[layer]; 
  }
 
  inputs = layers->inputs; 
  inputs_size = layers->inputs_size;
  outputs = (layers + num_of_layers-1)->outputs; 
  outputs_size = (layers + num_of_layers-1)->outputs_size; 
}

void dnn_op::update_parameter(string weights_file){
  // check file
  ifstream file(weights_file.c_str());
  if (!file.is_open()) {  
    cout << "error reading weight file " << weights_file << endl;
    exit(1);
  }
  // check layer number
  int temp_num_of_layers;
  file >> temp_num_of_layers;
  if (temp_num_of_layers != num_of_layers) {
    cout << "weight file number of layers (first line) not equal to config file number of layers" << endl;
    exit(1);
  }

  for (int layer = 0; layer < num_of_layers; layer++) {
    mac_inst* temp_mac = layers + layer;
    // check size
    int temp_in_size, temp_out_size;
    int input_size = temp_mac->inputs_size, output_size = temp_mac->outputs_size;
    file >> temp_in_size;
    file >> temp_out_size;
    if ((input_size != temp_in_size) || (output_size != temp_out_size)) {
      cout << "layer " << layer << ": neuron size in weight file not equal that in config file" << endl;
      cout << temp_in_size << '\t' << temp_out_size << '\t' << input_size << '\t' << output_size << endl; 
      exit(1);
    }
    // read parameter
    // weights
    int pts = 0;
    float temp;
    for (int i = 0; i < output_size; i++) 
      for (int j = 0; j < input_size; j++) { 
        file >> temp;
        temp_mac->parameters[pts++] = float2fixed(temp, temp_mac->parameters_int_bits, temp_mac->parameters_precision - temp_mac->parameters_int_bits); 
      } 
    // offset
    for (int i = 0; i < output_size; i++) { 
      file >> temp;
//      temp_mac->parameters[pts++] = float2fixed(temp, temp_mac->parameters_int_bits, temp_mac->parameters_precision - temp_mac->parameters_int_bits);
      temp_mac->offsets[i] = float2fixed(temp, temp_mac->parameters_int_bits, temp_mac->parameters_precision - temp_mac->parameters_int_bits);
    }
  }
}

void dnn_op::print_stat(){
  cout << "# dnn operation #" << endl;
  SHOW(num_of_layers, );
  for (int layer = 0; layer < num_of_layers; layer++) {
    cout << "layer" << layer + 1 << " inputs_size: " << (layers+layer)->inputs_size << endl;
    cout << "layer" << layer + 1 << " outputs_size: " << (layers+layer)->outputs_size << endl;
    cout << "layer" << layer + 1 << " temp_inputs_size: " << (layers+layer)->temp_inputs_size << endl;
    cout << "layer" << layer + 1 << " temp_outputs_size: " << (layers+layer)->temp_outputs_size << endl;
    cout << "layer" << layer + 1 << " inputs_start: " << (layers+layer)->inputs_start << endl;
    cout << "layer" << layer + 1 << " outputs_start: " << (layers+layer)->outputs_start << endl;
    cout << "layer" << layer + 1 << " parameters_start: " << (layers+layer)->parameters_start << endl;
  }
}

// TODO: better assignment
void dnn_op::update_inputs_address(int start_addr){
  max_volatile_space = start_addr; 
  inputs_start = start_addr;
  (layers)->inputs_start = inputs_start;
  for (int i = 0; i < num_of_layers; i++) {
    mac_inst* temp_mac = layers + i;
    int temp_space = TO_WORD_SIZE(temp_mac->temp_inputs_size * temp_mac->inputs_precision 
                  + temp_mac->temp_outputs_size * temp_mac->temp_outputs_precision);		// TODO
    int work_space_addr = TO_WORD_SIZE(temp_mac->inputs_start + temp_mac->inputs_size * temp_mac->inputs_precision);
    if (temp_space > work_space_addr) {
      temp_mac->outputs_start = temp_space;
      if (i != num_of_layers - 1)   // next layer input
        (temp_mac + 1)->inputs_start = temp_space;
    } else {
      temp_mac->outputs_start = work_space_addr;
      if (i != num_of_layers - 1)
        (temp_mac + 1)->inputs_start = work_space_addr;
    } 
		temp_mac->temp_outputs_start = TO_WORD_SIZE(temp_mac->inputs_size * temp_mac->inputs_precision);		// TODO
    max_volatile_space = max(temp_mac->outputs_start + TO_WORD_SIZE(temp_mac->outputs_size * temp_mac->outputs_precision), max_volatile_space); 
  }
}

void dnn_op::update_parameter_address(int start_addr){
  parameters_start = start_addr;
  int parameters_position = parameters_start;
  for (int layer = 0; layer < num_of_layers; layer++) {
    mac_inst* temp_mac = layers + layer;
    temp_mac->parameters_start = parameters_position;
    parameters_position += TO_WORD_SIZE(temp_mac->parameters_size * temp_mac->parameters_precision);
    temp_mac->offsets_start = parameters_position;
    parameters_position += TO_WORD_SIZE(temp_mac->outputs_size * temp_mac->offsets_precision);
  }
}

void dnn_op::update_inputs(string inputs_file){
  // check file
  ifstream file(inputs_file.c_str());
  if (!file.is_open()) {  
    cout << "error reading input file " << inputs_file << endl;
    exit(1);
  }
  // check inputs size
  int temp_inputs_size;
  file >> temp_inputs_size;
  if (temp_inputs_size != inputs_size) {
    cout << "input file: inputs_size (first line) not match with config file" << endl;
    exit(1);
  }

  if (inputs == NULL)
    inputs = new float[inputs_size];
  for (int i = 0; i < inputs_size; i++) {
    file >> inputs[i];
    inputs[i] = float2fixed(inputs[i], layers->inputs_int_bits, layers->inputs_precision - layers->inputs_int_bits);
  }
}

void dnn_op::generate_outputs(){
  for (int layer = 0; layer < num_of_layers; layer++)
    (layers + layer)->generate_outputs();
}

void dnn_op::print_binary_data(string* memory){
  for (int layer = 0; layer < num_of_layers; layer++)
    (layers + layer)->print_binary_data(memory);
}

string dnn_op::print_binary_inputs(string* memory){
  return "dnn_inputs\n" + layers->print_binary_inputs(memory);
}

string dnn_op::print_binary_outputs(string* memory){
  string result;
  for (int layer = 0; layer < num_of_layers; layer++)
    result += "\ndnn_outputs_layer" + to_string(layer) + "\n" + (layers + layer)->print_binary_outputs(memory);
  return result;
}
