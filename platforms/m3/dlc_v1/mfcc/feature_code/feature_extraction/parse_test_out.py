#!/usr/bin/env python
############################
###
def diff_distribution(diff_list):
  count = [0, 0, 0, 0, 0]  # 0.1%, 0.5%, 1%, 5%, 10%
  for item in diff_list:
    if item < 0.001:
      count[0] += 1
    elif item < 0.005:
      count[1] += 1
    elif item < 0.01:
      count[2] += 1
    elif item < 0.05:
      count[3] += 1
    elif item < 0.10:
      count[4] += 1
  for i in range(len(count)):
    print "%0.2f" % (float(count[i]) / sum(count))

############################
fft_input_size = 512
mfcc_filter_size = 26
dct_output_size = 13

############################
fft_inputs = []
fft_result = []
mfcc_result = []
mfcc_fixed_result = []
log_result = []
log_fixed_result = []
dct_result = []
dct_fixed_result = []

fft_parameter = []
mfcc_parameter = []
dct_parameter = []

dnn_input = []
dnn_neuron1 = []
dnn_neuron2 = []
dnn_neuron3 = []

dnn1_parameter = []
dnn2_parameter = []
dnn3_parameter = []

############################
try: 
    test_out = sys.argv[1]
except:
    test_out = open("all.out", 'r')
while(True):
    line = test_out.readline()
    if not line: break
    if line.startswith("## fft inputs"):
        line = test_out.readline()
        for item in line.split("\t"):
            try:
                fft_inputs.append(float(item)) 
            except:
                pass
    elif line.startswith("## fft result"):  
        line = test_out.readline()
        for item in line.split("\t"):
            try:
                fft_result.append(float(item)) 
            except:
                pass
    elif line.startswith("## fft parameter"):  
        line = test_out.readline()
        for item in line.split("\t"):
            try:
                fft_parameter.append(float(item)) 
            except:
                pass
    elif line.startswith("## mfcc result"):  
        line = test_out.readline()
        for item in line.split("\t"):
            try:
                mfcc_result.append(float(item)) 
            except:
                pass
    elif line.startswith("## mfcc parameter"):  
        line = test_out.readline()
        for item in line.split("\t"):
            try:
                mfcc_parameter.append(float(item)) 
            except:
                pass
    elif line.startswith("## log result"):  
        line = test_out.readline()
        for item in line.split("\t"):
            try:
                log_result.append(float(item)) 
            except:
                pass
    elif line.startswith("## dct parameter"):  
        line = test_out.readline()
        for item in line.split("\t"):
            try:
                dct_parameter.append(float(item)) 
            except:
                pass
    elif line.startswith("## dct result"):  
        line = test_out.readline()
        for item in line.split(" "):
            try:
                dct_result.append(float(item)) 
            except:
                pass
    elif line.startswith("## mfcc fixed result"):  
        line = test_out.readline()
        for item in line.split("\t"):
            try:
                mfcc_fixed_result.append(float(item)) 
            except:
                pass
    elif line.startswith("## log fixed result"):  
        line = test_out.readline()
        for item in line.split("\t"):
            try:
                log_fixed_result.append(float(item)) 
            except:
                pass
    elif line.startswith("## dct fixed result"):  
        line = test_out.readline()
        for item in line.split("\t"):
            try:
                dct_fixed_result.append(float(item)) 
            except:
                pass

#### integer part: max and min
print max(fft_inputs), min(fft_inputs)
print max(fft_parameter), min(fft_parameter)
print max(fft_result), min(fft_result)
print max(mfcc_parameter), min(mfcc_parameter)
print max(mfcc_result), min(mfcc_result)
print max(mfcc_fixed_result), min(mfcc_fixed_result)
print max(log_result), min(log_result)
print max(log_fixed_result), min(log_result)
print max(dct_parameter), min(dct_parameter)
print max(dct_fixed_result), min(dct_fixed_result)
print max(dct_result), min(dct_result)


############################
## ******* extract fft_inputs and fft_parameter here
############################

