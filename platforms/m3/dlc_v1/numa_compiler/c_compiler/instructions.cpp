#include "instructions.h"
#include "fixed.h"
#include "common.h"
#include <string>

string instruction::print_binary_inputs(string *memory) {
  int max_bits = (m_config->L1_size + m_config->L2_size + m_config->L3_size + m_config->L4_size) * 4 * 1024 * 8;
  int max_words = max_bits / 96;
  int start_address = max_words - inputs_start - TO_WORD_SIZE(inputs_size * inputs_precision); // location in words
  int location = start_address * WORD_SIZE;   	// location in bits
//  int location = inputs_start * WORD_SIZE; 		// virtual location in bits
  for (int i = 0; i < inputs_size; i++) {
    location += inputs_precision;
    string binary = dec2bin(inputs[i], inputs_signed, inputs_int_bits, inputs_precision);

    if ((location + binary.size()) > max_bits) {
      cout << "out of memory -- max: " << max_bits << " current: " << location + binary.size() <<endl;
      exit(1);
    }

    for (int j = 0; j < binary.size(); j++)
      (*memory)[location + j] = binary[j];
  }
  
  string result = *memory;
  return result;
}

string instruction::print_binary_outputs(string *memory) {
  int max_bits = (m_config->L1_size + m_config->L2_size + m_config->L3_size + m_config->L4_size) * 4 * 1024 * 8;
  int max_words = max_bits / 96;
  int start_address = max_words - outputs_start - TO_WORD_SIZE(outputs_size * outputs_precision); // location in words
  int location = outputs_start * WORD_SIZE; 		// location in bits
//  int location = outputs_start * WORD_SIZE; 	// location in bits
  for (int i = 0; i < outputs_size; i++) {
    location += outputs_precision;
    string binary = dec2bin(outputs[i], outputs_signed, outputs_int_bits, outputs_precision);

    if ((location + binary.size()) > max_bits) {
      cout << "out of memory -- max: " << max_bits << " current: " << location + binary.size() <<endl;
      exit(2);
    }

    for (int j = 0; j < binary.size(); j++)
      (*memory)[location + j] = binary[j];
  }

  string result = *memory;
  return result;
}

/////////////////////////////
mac_inst::~mac_inst(){
  if (!is_dnn) { 
	  delete[] inputs;
	  delete[] outputs;
	  delete[] parameters;
	}
//  if (is_dnn) delete[] offsets;
}

string mac_inst::print_instruction(){

  string inst;
  opcode op = MAC;
  inst = binary_insert(inst, op, OPCODE_BIT);
  inst = binary_insert(inst, temp_outputs_start, PE_SRAM_ADDR_BIT);
  inst = binary_insert(inst, TO_WORD_SIZE(temp_outputs_size * temp_outputs_precision), PE_SRAM_SIZE_BIT);
  inst = binary_insert(inst, tempout_precision_type[temp_outputs_precision], PRECISION_BIT);

  inst = binary_insert(inst, inputs_start, DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, TO_WORD_SIZE(inputs_size * inputs_precision), DSP_SRAM_SIZE_BIT);
  inst = binary_insert(inst, precision_type[inputs_precision], PRECISION_BIT);

  inst = binary_insert(inst, outputs_start, DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, TO_WORD_SIZE(outputs_size * outputs_precision), DSP_SRAM_SIZE_BIT);
  inst = binary_insert(inst, precision_type[outputs_precision], PRECISION_BIT);
  inst = binary_insert(inst, do_relu, 1);

  inst = binary_insert(inst, offsets_start, DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, parameters_start, DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, TO_WORD_SIZE(parameters_size * parameters_precision), DSP_SRAM_SIZE_BIT);
  inst = binary_insert(inst, precision_type[parameters_precision], PRECISION_BIT);

  inst = binary_insert(inst, is_dnn, 1);	// equal to "use_offset"
  inst = binary_insert(inst, use_nli, 1);
  inst = binary_insert(inst, mac_row_number, DSP_SRAM_ADDR_BIT);

  inst = inst + string(3 * PE_SRAM_ADDR_BIT,  '0');		// fft number, input unit size, output unit size

  inst = binary_insert(inst, shift, SHIFT_BIT);
  inst = binary_insert(inst, offsets_shift, SHIFT_BIT);
  inst = binary_insert(inst, non_linear_shift, SHIFT_BIT);

//  cout << bin2hex(inst) << endl;
  return inst;
}

void mac_inst::generate_outputs() {
  for (int i = 0; i < outputs_size; i++) {
    outputs[i] = 0;
    for (int j = 0; j < inputs_size; j++)
      outputs[i] += fixed_point_mul_ab(inputs[j], parameters[i * inputs_size + j], inputs_int_bits, parameters_int_bits, inputs_precision, parameters_precision); 

    if (is_dnn) {
      outputs[i] += offsets[i]; 
      if (do_relu)
        outputs[i] = (outputs[i] > 0) ? outputs[i] : 0;       
      // TODO: non linear functions 
    }

    if (outputs[i] >= pow(2, outputs_int_bits)) {
//      cout << pow(2, outputs_int_bits) - pow(2, -(outputs_precision - outputs_int_bits - outputs_signed)) << endl; 
      if (do_saturation)
        outputs[i] = pow(2, outputs_int_bits) - pow(2, -(outputs_precision - outputs_int_bits - outputs_signed)); 
      else
        do {outputs[i] -= pow(2, outputs_int_bits);} while (outputs[i] >= pow(2, outputs_int_bits)); 

    } else if (outputs[i] <= -pow(2, outputs_int_bits)) {
//     cout << -pow(2, outputs_int_bits) + pow(2, -(outputs_precision - outputs_int_bits - outputs_signed)) << endl; 
      if (do_saturation)
        outputs[i] = -pow(2, outputs_int_bits) + pow(2, -(outputs_precision - outputs_int_bits - outputs_signed)); 
      else
        do {outputs[i] += pow(2, outputs_int_bits);} while (outputs[i] <= -pow(2, outputs_int_bits)); 

    } else { 
      outputs[i] = float2fixed(outputs[i], outputs_int_bits, outputs_precision);
    }
  }
}

void mac_inst::print_binary_data(string *memory) {
  int max_bits = (m_config->L1_size + m_config->L2_size + m_config->L3_size + m_config->L4_size) * 4 * 1024 * 8;
  int max_words = max_bits / 96;
  int start_address = max_words - parameters_start - TO_WORD_SIZE(parameters_size * parameters_precision); // location in words
	if (is_dnn) // has offset
		start_address -= TO_WORD_SIZE(outputs_size * offsets_precision); // location in words
  int location = start_address * WORD_SIZE;   // location in bits
//  int location = parameters_start * WORD_SIZE;        // virtual location in bits

	// parameter (weight)
	// see digital_0328 page 21 explantation 
	int i_num = WORD_SIZE / inputs_precision;													// how many inputs in a word
	int matrix_x = TO_WORD_SIZE(inputs_size * inputs_precision);			// in words 
	int matrix_x_non_zero_bits = (inputs_size * inputs_precision);					// in bits
	int matrix_y = TO_WORD_SIZE(outputs_size * outputs_precision);		// in words TODO: temp output 
	int i;
	string binary;
	for (int ii = 0; ii < matrix_x; ii++) {		// submatrix
		i = ii * i_num; 
		for (int jj = 0; jj < matrix_y; jj++) {	// row
			for (int kk = 0; kk < i_num; kk++) {	// x in submatrix
    	  location += parameters_precision;
				if ((jj < outputs_size) && ((ii * i_num + kk) < inputs_size)) {
    	  	binary = dec2bin(parameters[i], parameters_signed, parameters_int_bits, parameters_precision);
				}	else {
    	  	binary = string(parameters_precision, '0'); 
				}
			  if ((location + binary.size()) > max_bits) {
			  	cout << "out of memory -- max: " << max_bits << " current: " << location + binary.size() <<endl;
			  	exit(3);
			  }
    	  for (int j = 0; j < binary.size(); j++)
        	(*memory)[location + j] = binary[j];	
				i++;
			}		// end submatrix row
			if (jj >= outputs_size) { 
				if (ii -= matrix_x) {
					i += inputs_size - i_num;
				}	else {
					i += inputs_size - (i_num - (matrix_x - matrix_x_non_zero_bits));
				}
			}
		}		// end submatrix
	}		// end all
			
	// offset
  if (is_dnn) {
    location = parameters_start * WORD_SIZE + parameters_size * parameters_precision;
    for (i = 0; i < outputs_size; i++) {
      location += offsets_precision;
      binary = dec2bin(offsets[i], offsets_signed, offsets_int_bits, offsets_precision);
      
      if ((location + binary.size()) > max_bits) {
        cout << "out of memory -- max: " << max_bits << " current: " << location + binary.size() <<endl;
        exit(4);
      }
      
      for (int j = 0; j < binary.size(); j++)
        (*memory)[location + j] = binary[j];
    }
  }
}

int mac_inst::get_tiling_parameters(dnn_config* m_config){
  if (!m_config->do_tiling) { // no tiling
    tiling_row = 1;
    tiling_column = 1;
    temp_inputs_size = inputs_size;
    temp_outputs_size = 0;
    temp_inputs_first = true;
  } else {
    // TODO
    tiling_row = 1;
    tiling_column = 1;
    temp_inputs_size = inputs_size;
    temp_outputs_size = 0;
    temp_inputs_first = true;
  }
  return temp_inputs_size + temp_outputs_size;
}

void mac_inst::get_mac_row_number(dnn_config* m_config)	{
	int lcm0 = lcm(inputs_precision, parameters_precision);
	int lcm0_i = lcm0 / inputs_precision;
	int lcm0_o = lcm(outputs_precision, lcm0_i);
	int lcm0_to = lcm(temp_outputs_precision, lcm0_i);
	mac_row_number = lcm(lcm0, lcm(lcm0_o, lcm0_to));
//	SHOW_D(mac_row_number, );
}

/////////////////////////////
fft_inst::~fft_inst(){
}

string fft_inst::print_instruction(){

  string inst; 
  opcode op = FFT;
  inst = binary_insert(inst, op, OPCODE_BIT);
		
  inst = inst + string(PE_SRAM_ADDR_BIT + PE_SRAM_SIZE_BIT + PRECISION_BIT , '0');	// mac tempouts

  inst = binary_insert(inst, inputs_start, DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, TO_WORD_SIZE(inputs_size * inputs_precision), DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, precision_type[inputs_precision], PRECISION_BIT);
  inst = binary_insert(inst, outputs_start, DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, TO_WORD_SIZE(outputs_size * outputs_precision), DSP_SRAM_SIZE_BIT);
  inst = binary_insert(inst, precision_type[outputs_precision], PRECISION_BIT);
  inst = binary_insert(inst, do_relu, 1);

  inst = inst + string(DSP_SRAM_SIZE_BIT, '0');	// mac offsets

  inst = binary_insert(inst, twiddle_start, DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, TO_WORD_SIZE(twiddle_size * twiddle_precision), DSP_SRAM_SIZE_BIT);
  inst = binary_insert(inst, precision_type[twiddle_precision], PRECISION_BIT);

  inst = inst + string(1 + 1 + DSP_SRAM_SIZE_BIT, '0');	// mac booleans and row number

  inst = binary_insert(inst, fft_number, PE_SRAM_ADDR_BIT);
  inst = binary_insert(inst, input_unit_size, PE_SRAM_ADDR_BIT);
  inst = binary_insert(inst, output_unit_size, PE_SRAM_ADDR_BIT);

  inst = binary_insert(inst, shift, SHIFT_BIT);
  inst = inst + string(2 * SHIFT_BIT, '0');	

//  cout << bin2hex(inst) << endl;
  return inst;
}

void fft_inst::generate_outputs() {
  for (int i = 0; i < inputs_size; i = i + fft_points) { 
    for (int j = i; j < i + fft_points / 2; j++) {    // upper half
      int lower_index = j + fft_points / 2;
      int parameter_index = j - i;
      float temp2r = inputs[2 * lower_index] * parameters[2 * parameter_index] - inputs[2 * lower_index + 1] * parameters[2 * parameter_index + 1]; 
      float temp2i = inputs[2 * lower_index] * parameters[2 * parameter_index + 1] + inputs[2 * lower_index + 1] * parameters[2 * parameter_index]; 
      outputs[2 * j] = inputs[2 * j] + temp2r;          // real part
      outputs[2 * j + 1] = inputs[2 * j + 1] + temp2i;  // img part
    }
    for (int j = i + fft_points / 2; j < i + fft_points; j++) {   // lower half
      int upper_index = j - fft_points / 2;
      int parameter_index = j - i - fft_points / 2;
      float temp1r = inputs[2 * upper_index] * parameters[2 * parameter_index] - inputs[2 * upper_index + 1] * parameters[2 * parameter_index + 1]; 
      float temp1i = inputs[2 * upper_index] * parameters[2 * parameter_index + 1] + inputs[2 * upper_index + 1] * parameters[2 * parameter_index]; 
      outputs[2 * j] =  temp1r - inputs[2 * j]; 
      outputs[2 * j + 1] = temp1i - inputs[2 * j];
    }
  }
}

void fft_inst::print_binary_data(string *memory) {
  int max_bits = (m_config->L1_size + m_config->L2_size + m_config->L3_size + m_config->L4_size) * 4 * 1024 * 8;
  int max_words = max_bits / 96;
  int start_address = max_words - twiddle_start - parameters_space; // location in words
  int location = start_address * WORD_SIZE;   // location in bits

//  int location = twiddle_start * WORD_SIZE; // virtual location in bits
  for (int i = 0; i < twiddle_size * 2; i++) {
    location += twiddle_precision;
    string binary = dec2bin(parameters[i], true, 1, twiddle_precision);

    if ((location + binary.size()) > max_bits) {
      cout << "out of memory -- max: " << max_bits << " current: " << location + binary.size() <<endl;
      exit(5);
    }

    for (int j = 0; j < binary.size(); j++)
      (*memory)[location + j] = binary[j];
  }
}

string fft_inst::print_binary_inputs(string *memory) {
  int max_bits = (m_config->L1_size + m_config->L2_size + m_config->L3_size + m_config->L4_size) * 4 * 1024 * 8;
  int max_words = max_bits / 96;
  int start_address = max_words - inputs_start - inputs_space; // location in words
  int location = start_address * WORD_SIZE;   // location in bits
//  int location = inputs_start * WORD_SIZE; // virtual location in bits
  for (int i = 0; i < inputs_size * 2; i++) {
    location += inputs_precision;
    string binary = dec2bin(inputs[i], true, 1, inputs_precision);

    if ((location + binary.size()) > max_bits) {
      cout << "out of memory -- max: " << max_bits << " current: " << location + binary.size() <<endl;
      exit(6);
    }

    for (int j = 0; j < binary.size(); j++)
      (*memory)[location + j] = binary[j];

    // zero padding: avoid memory access across multiple words 
    if (  (inputs_precision == 8) && ((i + 1) / 6 == 0)
       || (inputs_precision == 16) && ((i + 1) / 3 == 0)  )
      location += 16;
  }
  
  string result = *memory;
  return result;
}

string fft_inst::print_binary_outputs(string *memory) {
  int max_bits = (m_config->L1_size + m_config->L2_size + m_config->L3_size + m_config->L4_size) * 4 * 1024 * 8;
  int max_words = max_bits / 96;
  int start_address = max_words - outputs_start - outputs_space; // location in words
  int location = outputs_start * WORD_SIZE; // location in bits

  for (int i = 0; i < outputs_size * 2; i++) {
    location += outputs_precision;
    string binary = dec2bin(outputs[i], true, 3, outputs_precision);

    if ((location + binary.size()) > max_bits) {
      cout << "out of memory -- max: " << max_bits << " current: " << location + binary.size() <<endl;
      exit(7);
    }

    for (int j = 0; j < binary.size(); j++)
      (*memory)[location + j] = binary[j];

    if (  (outputs_precision == 8) && ((i + 1) / 6 == 0)
       || (outputs_precision == 16) && ((i + 1) / 3 == 0)  )
      location += 16;
  }

  string result = *memory;
  return result;
}
/////////////////////////////
non_linear_inst::~non_linear_inst(){
  delete[] inputs;
  delete[] outputs;
}

string non_linear_inst::print_instruction(){

  string inst;
  opcode op = NON;
  inst = binary_insert(inst, op, OPCODE_BIT);

  inst = inst + string(PE_SRAM_ADDR_BIT + PE_SRAM_SIZE_BIT + PRECISION_BIT , '0');	// mac tempouts

  inst = binary_insert(inst, inputs_start, DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, TO_WORD_SIZE(inputs_size * inputs_precision), DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, precision_type[inputs_precision], PRECISION_BIT);
	// outputs should be exactly the same as inputs
  inst = binary_insert(inst, outputs_start, DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, TO_WORD_SIZE(outputs_size * outputs_precision), DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, precision_type[outputs_precision], PRECISION_BIT);
  inst = binary_insert(inst, do_relu, 1);

  inst = inst + string(3 * DSP_SRAM_ADDR_BIT + PRECISION_BIT, '0');
  inst = inst + string(1 + 1 + DSP_SRAM_SIZE_BIT, '0');	
  inst = inst + string(3 * PE_SRAM_ADDR_BIT, '0');

  inst = inst + string(2 * SHIFT_BIT, '0');	
  inst = binary_insert(inst, shift, SHIFT_BIT);
//  cout << bin2hex(inst) << endl;
  return inst;
}

void non_linear_inst::generate_outputs() {
  // TODO
}
/////////////////////////////
mov_inst::~mov_inst(){
  delete[] inputs;
  delete[] outputs;
}

string mov_inst::print_instruction(){

  string inst;
  opcode op = MOV;
  inst = binary_insert(inst, op, OPCODE_BIT);

  inst = inst + string(PE_SRAM_ADDR_BIT + PE_SRAM_SIZE_BIT + PRECISION_BIT , '0');	// mac tempouts

  inst = binary_insert(inst, inputs_start, DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, TO_WORD_SIZE(inputs_size * inputs_precision), DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, precision_type[inputs_precision], PRECISION_BIT);
  inst = binary_insert(inst, outputs_start, DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, TO_WORD_SIZE(outputs_size * outputs_precision), DSP_SRAM_ADDR_BIT);
  inst = binary_insert(inst, precision_type[outputs_precision], PRECISION_BIT);
  inst = binary_insert(inst, do_relu, 1);

  inst = inst + string(3 * DSP_SRAM_ADDR_BIT + PRECISION_BIT, '0');
  inst = inst + string(1 + 1 + DSP_SRAM_SIZE_BIT, '0');	
  inst = inst + string(3 * PE_SRAM_ADDR_BIT, '0');
  inst = inst + string(3 * SHIFT_BIT, '0');	
//  cout << bin2hex(inst) << endl;
  return inst;
}

void mov_inst::generate_outputs() {
  for (int i = 0; i < inputs_size; i++)
    outputs[i] = inputs[i];
}
