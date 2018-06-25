#!/usr/bin/env python
import sys
import os

## default value
layers = 3
#neurons = [408, 400, 400, 16]
neurons = [408, 384, 384, 24]
#neurons = [48, 96, 96, 24]
i_exp = 12
o_exp = 12
w_exp = 6
to_exp = 32
## 
shift = pow(2, 14)
shift_nli = pow(2, 10)
## 

##########
def nli_func(nli_x, nli_a, nli_b, in_data, nli_shift):
#    print nli_x, in_data
    for x in range(len(nli_x) - 1):
        if in_data >= nli_x[x] and in_data <= nli_x[x+1]:
            return (in_data * nli_a[x] + nli_b[x]) / shift_nli

def read_dnn_file(filename):
    input_file = open(filename, 'r')
    input_file.readline()
    inputs = []
    for item in input_file.readline().split(' '):
        try:
            inputs.append(int(item))
        except:
            pass
    input_file.close()
    return inputs

def read_dnn_line(line):
    result = []
    for item in line.split(','):
        result.append(int(item))
    return result

def do_layer(input_filename, output_filename, weight_filename, offset_filename, nli_filename, input_size, output_size, shift, nli_shift):
    inputs = read_dnn_file(input_filename)
    if len(inputs) != input_size:
        print "input error"; sys.exit()
    offsets = read_dnn_file(offset_filename)
    if len(offsets) != output_size:
        print "offset error"; sys.exit()

    weights = []
    weight_file = open(weight_filename, 'r')
    line_no = 0
    for line in weight_file:
        if line_no == 0:
          line_no += 1
          continue
        for item in line.split(' '):
            try:
                weights.append(int(item))
            except:
                pass
    if len(weights) != (input_size * output_size):
        print len(weights)
        print "weight error"; sys.exit()

    nli_file = open(nli_filename, 'r')
    nli_x = read_dnn_line(nli_file.readline())
    nli_a = read_dnn_line(nli_file.readline())
    nli_b = read_dnn_line(nli_file.readline())

    outputs = []
    for j in range(output_size):
      output = 0
      for i in range(input_size):
        output += inputs[i] * weights[j * input_size + i]  
      output = output / shift + offsets[j]
      outputs.append(nli_func(nli_x, nli_a, nli_b, output, nli_shift))
    if len(outputs) != output_size:
        print "output error"; sys.exit()

    output_file = open(output_filename, 'w')
    output_file.write("deadbeef\n")
    for item in outputs:
        output_file.write(str(item) + ' ')
    output_file.close()
    

##########
do_layer("dnn.inputs", "dnn_0.goldenoutputs", "dnn_0.weights", "dnn_0.offsets", "dnn.nli", neurons[0], neurons[1], shift, shift_nli)
os.system("diff dnn_0.outputs dnn_0.goldenoutputs")
do_layer("dnn_0.goldenoutputs", "dnn_1.goldenoutputs", "dnn_1.weights", "dnn_1.offsets", "dnn.nli", neurons[1], neurons[2], shift, shift_nli)
os.system("diff dnn_1.outputs dnn_1.goldenoutputs")
do_layer("dnn_1.goldenoutputs", "dnn_2.goldenoutputs", "dnn_2.weights", "dnn_2.offsets", "dnn.nli", neurons[2], neurons[3], shift, shift_nli)
os.system("diff dnn_2.outputs dnn_2.goldenoutputs")

