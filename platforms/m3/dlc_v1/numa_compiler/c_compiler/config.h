#ifndef __DNN_CONFIG_H__
#define __DNN_CONFIG_H__

#include "common.h"
#include "option_parser.h"

class dnn_config {
    
public:
    dnn_config(){};
    ~dnn_config(){};
    
    void reg_options(option_parser_t opp);
    
    // Files (inputs, outputs, parameters)
    char* input_file;
    char* weight_file;
    char* memory_file;
    char* inst_file;
    char* test_case_file;

    // memory config
    int word_size;
    // memory layers
    float L1_size;
    float L2_size;
    float L3_size;
    float L4_size;
    float L1_bank_size;
    float L2_bank_size;
    float L3_bank_size;
    float L4_bank_size;

    // operation options
    bool do_tiling;
    bool do_parallel;

    // precomputation
    bool do_precompute;
    int sample_rate;
    int fft_inputs_size;
    int fft_min_points;
    int fft_inputs_precision;
    int fft_inputs_int_bits;
    int fft_twiddle_precision;
    int fft_twiddle_int_bits;
    int mel_filter_num;
    float mel_filter_minhz;
    float mel_filter_maxhz;
    int mel_parameter_precision;
    int mel_parameter_int_bits;
    int dct_coeff_num;
    int dct_parameter_precision;
    int dct_parameter_int_bits;

    // DNN 
    int layer_num;

    #define LAYER_PARAMETERS(m) \
    int l##m##_inputs;  \
    int l##m##_neuron_precision; \
    int l##m##_neuron_int_bits; \
    bool l##m##_neuron_signed; \
    int l##m##_weight_precision; \
    int l##m##_weight_int_bits; \
    bool l##m##_weight_signed; \
    int l##m##_offset_precision; \
    int l##m##_offset_int_bits; \
    bool l##m##_offset_signed; \
    char* l##m##_function; \
    int l##m##_tempout_precision; \
    int l##m##_tempout_int_bits; \
    bool l##m##_tempout_signed; \
  
    LAYER_PARAMETERS(1)
    LAYER_PARAMETERS(2)
    LAYER_PARAMETERS(3)
    LAYER_PARAMETERS(4)
    LAYER_PARAMETERS(5)
    LAYER_PARAMETERS(6)
    LAYER_PARAMETERS(7)
    LAYER_PARAMETERS(8)
    LAYER_PARAMETERS(9)
    LAYER_PARAMETERS(10)

};

#endif
