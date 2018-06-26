#!/usr/bin/env python

### generate fake input, weight and offest numbers for MAC_MOV test 

import sys
from check_overlap import *

## default value
layers = 1        ### cannot change this in this script
neurons = [400, 400]
i_exp = 12;
o_exp = 12;
w_exp = 6;
to_exp = 32;
## 

if len(sys.argv) > 2:
  neurons[0] = int(sys.argv[1])
  neurons[1] = int(sys.argv[2])
if len(sys.argv) > 6:
  i_exp  = int(sys.argv[3])
  check_legal_precision(i_exp)
  o_exp  = int(sys.argv[4])
  check_legal_precision(o_exp)
  w_exp  = int(sys.argv[5])
  check_legal_precision(w_exp)
  to_exp = int(sys.argv[6])
  check_legal_precision(to_exp, True)
if len(sys.argv) > 7:
  parallel = int(sys.argv[7])
else:
  parallel = True
# not used here
#weights_int_bit = [2]    
#neuron_int_bit = [3, 4]  

if parallel:
  ## TODO: zero pad output if not divisible by 4
  for i in range(layers):
      if ((neurons[i+1] % 4) != 0):
          print("output of layer ", i, " is not divisible by 4!")
          sys.exit()

  check_overlap(neurons[0], neurons[1] / 4, i_exp, o_exp, w_exp, to_exp)
  ### check if the assignment can fit in one PE w/o bank overlap
else:
  check_overlap(neurons[0], neurons[1], i_exp, o_exp, w_exp, to_exp)
  ### check if the assignment can fit in one PE w/o bank overlap

############# weights
weight_file = open("dnn.weights", 'w')
#weight_file.write(str(layers) + '\n')
for i in range(layers):
  weight_file.write(' '.join((str(neurons[i]), str(neurons[i+1]), str(w_exp))) + '\n')
  for k in range(neurons[i+1]):
    string = ""
    for j in range(neurons[i]):
#      string += str(int((2**(w_exp-1)-1) * (float(k+1) / neurons[i+1]) * (float(j+1) / neurons[i]) * (float(i+1) / layers))) + ' '  ## increase gradually
      string += str(int((2**w_exp-1) * (1 - (float(k+1) / neurons[i+1])) * (float(j+1) / neurons[i]) * (float(i+1) / layers)) - int(2**(w_exp-1))) + ' '  ## debug
    weight_file.write(string + '\n')
  string = ""
weight_file.close()

############# inputs
input_file = open("dnn.inputs", 'w')
input_file.write(' '.join((str(neurons[0]), str(i_exp), str(o_exp), str(to_exp))) + '\n')
string = ""
for i in range(neurons[0]):
#  string += str(int((2**(i_exp-1)-1) * (float(i+1) / neurons[0]))) + ' '                      ## increase gradually
  string += str(int((2**i_exp-1) * (float(i+1) / neurons[0])) - int(2**(i_exp-1))) + ' '                      ## increase gradually
input_file.write(string + '\n')
input_file.close()

############# offsets
offset_file = open("dnn.offsets", 'w')
#offset_file.write(str(layers) + '\n')
string = ""
for i in range(layers):
  offset_file.write(str(neurons[i+1]) + '\n')
  for j in range(neurons[i+1]):
#    string += str(int((2**(o_exp-1)-1) * (float(j+1) / neurons[i+1]) * (float(i+1) / layers))) + ' '  ## increase gradually
    string += str(int((2**o_exp-1) * (float(j+1) / neurons[i+1]) * (float(i+1) / layers)) - int(2**(o_exp-1))) + ' '  ## increase gradually
  offset_file.write(string + '\n')
offset_file.close()
