#!/usr/bin/env python
from os import system
import math
import sys

##################################
## config
debug = 0

fft_size = 8
fft_size = 512

i_exp = 12
w_exp = 12
#######
ABS = 1. / pow(2, w_exp - 2)
if len(sys.argv) > 2:
  fft_size = int(sys.argv[1])
#############
## fake inputs
inputs = []
for i in range(fft_size):
  inputs.append((i % 10) * 10)
#  inputs.append((i % 7) * 12) 
  ## input cannot be too large: overflow
  ## input cannot be too small: lose precision
##################################
def int_bit_reverse(num_in, bits):
    result = 0
    for i in range(bits):
        temp = (num_in >> (bits - i - 1)) & 1
        result |= (temp << i)
#        print temp, result
#    print '\n'
    return result

def bit_reverse(inputs, fft_num):
    dft_size = fft_size / fft_num / 2
    fft_num_bits = int(math.log(fft_num) / math.log(2)) + 1 
    new_inputs = inputs[:]
    for i in range(fft_num * 2):
        base = int_bit_reverse(i, fft_num_bits)
        for j in range(dft_size):
          new_inputs[i * dft_size + j] = inputs[base + j * fft_num * 2]
    return new_inputs

def dft(inputs_r, inputs_i, dft_num):
#    print "dft_num:", dft_num
    for i in range(0, fft_size, dft_num):
      temp_r = [0] * dft_num
      temp_i = [0] * dft_num
      for j in range(dft_num):
        for k in range(dft_num):
          temp_r[j] += inputs_r[k + i] * math.cos(-2*j*k*math.pi/dft_num) - inputs_i[k + i] * math.sin(-2*j*k*math.pi/dft_num)
          temp_i[j] += inputs_i[k + i] * math.cos(-2*j*k*math.pi/dft_num) + inputs_r[k + i] * math.sin(-2*j*k*math.pi/dft_num)
      for j in range(dft_num):
        inputs_r[j + i] = temp_r[j] 
        inputs_i[j + i] = temp_i[j] 
        if (abs(inputs_r[j + i]) < ABS):
          inputs_r[j + i] = 0
        if (abs(inputs_i[j + i]) < ABS):
          inputs_i[j + i] = 0

def separate_group(inputs, dft_size):
    inputs_0 = []
    inputs_1 = []
    for i in range(len(inputs)):
        group = (i / dft_size) % 2
        if (group == 0):
            inputs_0.append(inputs[i])
        if (group == 1):
            inputs_1.append(inputs[i])
    return [inputs_0, inputs_1]

##################################
## main
fft_num = []
## TODO: need to consider w_exp as well
if (i_exp == 6):
  fft_num.append(fft_size / 8 / 2)
elif (i_exp == 8) or (i_exp == 12):
  fft_num.append(fft_size / 4 / 2)
elif (i_exp == 16): 
  fft_num.append(fft_size / 2 / 2)
else:
  print "i_exp not supported"
  sys.exit()
test = fft_num[0] / 2
while (test > 0): 
  fft_num.append(test)
  test /= 2
############# twiddle factors
for i in range(len(fft_num)):
  weight_file = open("fft_" + str(i) + ".weights", 'w')
  weights = fft_size / 2 / fft_num[i]
  weight_file.write(str(weights) + '\n')
  twi_interval = fft_num[i] 
  twi_idx = 0
  for j in range(weights):
    string  = str(math.cos(-2*math.pi*twi_idx / fft_size) * pow(2, w_exp-2)) + ' '
    string += str(math.sin(-2*math.pi*twi_idx / fft_size) * pow(2, w_exp-2)) + ' '
    twi_idx += twi_interval
    if debug:
      string  = str(pow(2, w_exp-4)) + ' '
      string += str(-pow(2, w_exp-4)) + ' '
    weight_file.write(string)
  weight_file.close()

############# inputs
#print "fft_num[0]:", fft_num[0]
#print "inputs:", inputs
inputs = bit_reverse(inputs, fft_num[0]) 
#print "inputs after bit reverse: ", inputs
dft_size = fft_size / fft_num[0] / 2    ## weight_size
#print "dft_size:", dft_size
inputs_r = inputs[:]
inputs_i = [0] * fft_size
dft(inputs_r, inputs_i, dft_size)
#print "inputs_r:", inputs_r
#print "inputs_i:", inputs_i
[inputs_r0, inputs_r1] = separate_group(inputs_r, dft_size)
[inputs_i0, inputs_i1] = separate_group(inputs_i, dft_size)
#print inputs_r0, inputs_r1
#print inputs_i0, inputs_i1

input_file  = open("fft" + ".inputs", 'w')
input_file.write(str(fft_size)+ '\n')
string = ""
#for j in range(fft_size):
#  string  = str(inputs_r[j]) + ' '
#  string += str(inputs_i[j]) + ' '
##  string  = str(int((2**(i_exp-1)-1) * (float(j+1) / fft_size)) 
##              - int(2**(i_exp-2))) + ' '								## increase gradually
##  string += str(int((2**(i_exp-1)-1) * (1 - float(j+1) / fft_size)) 
##              - int(2**(i_exp-2))) + ' '								## decrease gradually
#  input_file.write(string)
for i in range(fft_size / 2):
     string  = str(int(inputs_r0[i])) + ' '
     string += str(int(inputs_i0[i])) + ' '
     input_file.write(string)
     string  = str(int(inputs_r1[i])) + ' '
     string += str(int(inputs_i1[i])) + ' '
     input_file.write(string)
#for i in range(0, fft_size / 2, dft_size * 2):
#    for j in range(dft_size):
#        string  = str(inputs_r0[i + j]) + ' '
#        string += str(inputs_i0[i + j]) + ' '
#        input_file.write(string)
#        string  = str(inputs_r0[i + j + dft_size]) + ' '
#        string += str(inputs_i0[i + j + dft_size]) + ' '
#        input_file.write(string)
#    for j in range(dft_size):
#        string  = str(inputs_r1[i + j]) + ' '
#        string += str(inputs_i1[i + j]) + ' '
#        input_file.write(string)
#        string  = str(inputs_r1[i + j + dft_size]) + ' '
#        string += str(inputs_i1[i + j + dft_size]) + ' '
#        input_file.write(string)
########################## 
input_file.close()

