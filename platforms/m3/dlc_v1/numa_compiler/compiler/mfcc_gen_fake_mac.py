#!/usr/bin/env python
import sys
from check_overlap import *

## default value
inputs = 257;
outputs = 26;
i_exp = 12;
o_exp = 12;
w_exp = 12;
to_exp = 32;
## 

############# weights
weight_file = open("mfcc.weights", 'w')
weight_file.write(' '.join((str(inputs), str(outputs), str(w_exp))) + '\n')
for k in range(inputs):
  string = ""
  for j in range(outputs):
    string += str(int((2**w_exp-1) * (1 - (float(k+1) / neurons[i+1])) * (float(j+1) / neurons[i]) * (float(i+1) / layers)) - int(2**(w_exp-1))) + ' '  ## increase graudally 
#     string += str(int(2**(w_exp-3))) + ' ' ## constant
  weight_file.write(string + '\n')
string = ""
weight_file.close()

############# inputs
input_file = open("mfcc.inputs", 'w')
input_file.write(' '.join((str(inputs), str(i_exp), str(o_exp), str(to_exp))) + '\n')
string = ""
for i in range(inputs):
  string += str((i - inputs / 2) * int(2**(i_exp-6))) + ' '  
#  string += str(int(2**(i_exp-3))) + ' '  ## constant 
input_file.write(string + '\n')
input_file.close()

############# offsets
offset_file = open("mfcc.offsets", 'w')
string = ""
offset_file.write(str(outputs) + '\n')
for j in range(outputs):
   string += 0 + ' '
offset_file.write(string + '\n')
offset_file.close()
