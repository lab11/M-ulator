#include "common.h"
#include "config.h"

void dnn_config::reg_options(option_parser_t opp){

    // Files (inputs, outputs, parameters)
    option_parser_register(opp, "-input_file", OPT_CSTR, &input_file,
                           "input filename (default = \"dnn.input\")",
                           "dnn.input");
    option_parser_register(opp, "-weight_file", OPT_CSTR, &weight_file,
                           "weight filename (default = \"dnn.weights\")",
                           "dnn.weights");
    option_parser_register(opp, "-memory_file", OPT_CSTR, &memory_file,
                           "binary memory filename (default = \"dnn.memory\")",
                           "dnn.memory");
    option_parser_register(opp, "-instruction_file", OPT_CSTR, &inst_file,
                           "binary instruction filename (default = \"dnn.inst\")",
                           "dnn.inst");
    option_parser_register(opp, "-test_case_file", OPT_CSTR, &test_case_file,
                           "test case filename (default = \"dnn.test\")",
                           "dnn.test");

    // Memory configuations 
    option_parser_register(opp, "-word_size", OPT_INT32, &word_size,
                           "memory word size (bits, default = 96)",
                           "96");
    ///** THIS IS HARD CODED IN dnn_compiler.cpp

    // Memory layer size 
    option_parser_register(opp, "-L1_size", OPT_FLOAT, &L1_size,
                           "Level 1 SRAM size (kB, 1PE, default = 1.5)",
                           "1.5");
    option_parser_register(opp, "-L2_size", OPT_FLOAT, &L2_size,
                           "Level 2 SRAM size (kB, 1PE, default = 6)",
                           "6");
    option_parser_register(opp, "-L3_size", OPT_FLOAT, &L3_size,
                           "Level 3 SRAM size (kB, 1PE, default = 12)",
                           "12");
    option_parser_register(opp, "-L4_size", OPT_FLOAT, &L4_size,
                           "Level 4 SRAM size (kB, 1PE, default = 48)",
                           "48");
    option_parser_register(opp, "-L1_bank_size", OPT_FLOAT, &L1_bank_size,
                           "Level 1 SRAM bank size (kB, 1PE, default = 0.375)",
                           "0.375");
    option_parser_register(opp, "-L2_bank_size", OPT_FLOAT, &L2_bank_size,
                           "Level 2 SRAM bank size (kB, 1PE, default = 1.5)",
                           "1.5");
    option_parser_register(opp, "-L3_bank_size", OPT_FLOAT, &L3_bank_size,
                           "Level 3 SRAM bank size (kB, 1PE, default = 3)",
                           "3");
    option_parser_register(opp, "-L4_bank_size", OPT_FLOAT, &L4_bank_size,
                           "Level 4 SRAM bank size (kB, 1PE, default = 12)",
                           "12");
    
    // operation options
    option_parser_register(opp, "-do_parallel", OPT_BOOL, &do_parallel,
                           "do parallel dnn or not (default = true)",
                           "1");
    option_parser_register(opp, "-do_tiling", OPT_BOOL, &do_tiling,
                           "do tiling or not (default = true)",
                           "1");

    // Precomputation
    option_parser_register(opp, "-do_precompute", OPT_BOOL, &do_precompute,
                           "do precompute or not (default = true)",
                           "1");
    
    option_parser_register(opp, "-fft_input_size", OPT_INT32, &fft_inputs_size,
                           "fft_input_size (default = 400)",
                           "400");
    option_parser_register(opp, "-fft_min_points", OPT_INT32, &fft_min_points,
                           "minimum fft points (default = 16)",
                           "8");
    option_parser_register(opp, "-fft_inputs_precision", OPT_INT32, &fft_inputs_precision,
                           "fft_inputs_precision (bits, default = 8)",
                           "8");
    option_parser_register(opp, "-fft_inputs_int_bits", OPT_INT32, &fft_inputs_int_bits,
                           "fft_inputs_integer_bits (bits, default = 2)",
                           "2");
    option_parser_register(opp, "-fft_twiddle_precision", OPT_INT32, &fft_twiddle_precision,
                           "fft_twiddle_precision (bits, default = 8)",
                           "8");
/*
    option_parser_register(opp, "-fft_twiddle_int_bits", OPT_INT32, &fft_twiddle_int_bits,
                           "fft_twiddle_integer_bits (bits, default = 1)",
                           "1");
    // THIS IS HARD CODED IN operations.cpp
*/

    option_parser_register(opp, "-number_of_mel_filters", OPT_INT32, &mel_filter_num,
                           "number of mel-scale frequency filters (default = 26)",
                           "26");
    option_parser_register(opp, "-mel_filter_minhz", OPT_FLOAT, &mel_filter_minhz,
                           "mel_filter_minhz (default = 128.0)",
                           "128.0");
    option_parser_register(opp, "-mel_filter_maxhz", OPT_FLOAT, &mel_filter_maxhz,
                           "mel_filter_maxhz (default = 20000.0)",
                           "20000.0");
    option_parser_register(opp, "-sample_rate", OPT_INT32, &sample_rate,
                           "sample_rate (default = 16000)",
                           "16000");
    option_parser_register(opp, "-mel_parameter_precision", OPT_INT32, &mel_parameter_precision,
                           "mel-scale frequency parameter precision (bits, default = 8)",
                           "8");
    option_parser_register(opp, "-mel_parameter_int_bits", OPT_INT32, &mel_parameter_int_bits,
                           "mel-scale frequency parameter int bits (bits, default = 1)",
                           "1");

    option_parser_register(opp, "-dct_coeff_num", OPT_INT32, &dct_coeff_num,
                           "DCT number of coefficients - outputs (default = 13)",
                           "13");
    option_parser_register(opp, "-dct_parameter_precision", OPT_INT32, &dct_parameter_precision,
                           "DCT parameter precision (bits, default = 8)",
                           "8");
    option_parser_register(opp, "-dct_parameter_int_bits", OPT_INT32, &dct_parameter_int_bits,
                           "DCT parameter integer bits (bits, default = 4)",
                           "4");

    // DNN
    option_parser_register(opp, "-layer_num", OPT_INT32, &layer_num,
                           "number of DNN layers (default = 3)",
                           "3");

      // layers
    #define REG_LAYER(m) \
    option_parser_register(opp, "-l"#m"_inputs", OPT_INT32, &l##m##_inputs, \
                           "number of DNN layer"#m" inputs (default = 400)", \
                           "400");  \
    \
    option_parser_register(opp, "-l"#m"_neuron_precision", OPT_INT32, &l##m##_neuron_precision,  \
                           "DNN layer"#m" neuron precision (bits, default = 8)", \
                           "8");  \
    option_parser_register(opp, "-l"#m"_neuron_int_bits", OPT_INT32, &l##m##_neuron_int_bits,  \
                           "DNN layer"#m" neuron integer bits (bits, default = 4)",  \
                           "4");  \
    option_parser_register(opp, "-l"#m"_neuron_signed", OPT_BOOL, &l##m##_neuron_signed,  \
                           "DNN layer"#m" neuron signed (bool, default = true)",  \
                           "1");  \
    \
    option_parser_register(opp, "-l"#m"_weight_precision", OPT_INT32, &l##m##_weight_precision, \
                           "DNN layer"#m" weight precision (bits, default = 6)", \
                           "6");  \
    option_parser_register(opp, "-l"#m"_weight_int_bits", OPT_INT32, &l##m##_weight_int_bits,  \
                           "DNN layer"#m" weight integer bits (bits, default = 2)",  \
                           "2");  \
    option_parser_register(opp, "-l"#m"_weight_signed", OPT_BOOL, &l##m##_weight_signed,  \
                           "DNN layer"#m" weight signed (bool, default = true)",  \
                           "1");  \
    \
    option_parser_register(opp, "-l"#m"_offset_precision", OPT_INT32, &l##m##_offset_precision,  \
                           "DNN layer"#m" offset precision (bits, default = 8)", \
                           "8");  \
    option_parser_register(opp, "-l"#m"_offset_int_bits", OPT_INT32, &l##m##_offset_int_bits,  \
                           "DNN layer"#m" offset integer bits (bits, default = 4)",  \
                           "4");  \
    option_parser_register(opp, "-l"#m"_offset_signed", OPT_BOOL, &l##m##_offset_signed,  \
                           "DNN layer"#m" offset signed (bool, default = true)",  \
                           "1");  \
    \
    option_parser_register(opp, "-l"#m"_function", OPT_CSTR, &l##m##_function,  \
                           "DNN layer"#m" activation function (default = SIGMOID)",  \
                           "sigmoid");  \
		\
    option_parser_register(opp, "-l"#m"_tempout_precision", OPT_INT32, &l##m##_tempout_precision,  \
                           "DNN layer"#m" tempout precision (bits, default = 16)", \
                           "16");  \
    option_parser_register(opp, "-l"#m"_tempout_int_bits", OPT_INT32, &l##m##_tempout_int_bits,  \
                           "DNN layer"#m" tempout integer bits (bits, default = 2)",  \
                           "2");  \
    option_parser_register(opp, "-l"#m"_tempout_signed", OPT_BOOL, &l##m##_tempout_signed,  \
                           "DNN layer"#m" tempout signed (bool, default = true)",  \
                           "1");

    REG_LAYER(1)
    REG_LAYER(2)
    REG_LAYER(3)
    REG_LAYER(4)
    REG_LAYER(5)
    REG_LAYER(6)
    REG_LAYER(7)
    REG_LAYER(8)
    REG_LAYER(9)
    REG_LAYER(10)

//    REG_LAYER2(1)
//    REG_LAYER2(2)
//    REG_LAYER2(3)
//    REG_LAYER2(4)
//    REG_LAYER2(5)
//    REG_LAYER2(6)
//    REG_LAYER2(7)
//    REG_LAYER2(8)
//    REG_LAYER2(9)
//    REG_LAYER2(10)

}
