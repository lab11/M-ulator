#!/usr/bin/env python
import sys
from check_overlap import *

## default value
layers = 3
#neurons = [408, 400, 400, 16]
#neurons = [408, 384, 384, 24]
neurons = [48, 96, 96, 24]
i_exp = 12;
o_exp = 12;
w_exp = 6;
to_exp = 32;
## 

############# weights
for i in range(layers):
  weight_file = open("dnn_" + str(i) + ".weights", 'w')
  weight_file.write(' '.join((str(neurons[i]), str(neurons[i+1]), str(w_exp))) + '\n')
  for k in range(neurons[i+1]):
    string = ""
    for j in range(neurons[i]):
#      string += str(int((2**w_exp-1) * (1 - (float(k+1) / neurons[i+1])) * (float(j+1) / neurons[i]) * (float(i+1) / layers)) - int(2**(w_exp-1))) + ' '  ## original 
      string += str(int(2**(w_exp-3))) + ' ' ## debug
    weight_file.write(string + '\n')
  string = ""
  weight_file.close()

############# inputs
input_file = open("dnn.inputs", 'w')
input_file.write(' '.join((str(neurons[0]), str(i_exp), str(o_exp), str(to_exp))) + '\n')
string = ""
for i in range(neurons[0]):
#  string += str(int((2**i_exp-1) * (float(i+1) / neurons[0])) - int(2**(i_exp-1))) + ' '  ## increase gradually
  string += str(int(2**(i_exp-3))) + ' '  ## increase gradually
input_file.write(string + '\n')
input_file.close()

############# offsets
for i in range(layers):
  offset_file = open("dnn_" + str(i) +  ".offsets", 'w')
  string = ""
  offset_file.write(str(neurons[i+1]) + '\n')
  for j in range(neurons[i+1]):
#    string += str(int((2**(o_exp-1)-1) * (float(j+1) / neurons[i+1]) * (float(i+1) / layers))) + ' '  ## increase gradually
    string += str(j + int(2**(o_exp-5))) + ' '  ## increase gradually
  offset_file.write(string + '\n')
  offset_file.close()
