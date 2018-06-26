#!/usr/bin/env python

import sys
from os import system

################
def to_mem(a):
  if a > 0:
    result = bin(a)[2:]
    result = '0' * (96 - len(result)) + result
  elif a < 0:
    result0 = bin(-a)[2:]
    result = ''
    for bit in result0:
      if bit == '1':
        result += '0'
      else:
        result += '1'
    test = 1
    result0 = ''
    for bit in range(len(result) - 1, -1, -1):
      if (test == 1):
        if (result[bit] == '0'):
          result0 = '1' + result0
          test = 0
        else:
          result0 = '0' + result0
      else:
        result0 = result[bit] + result0 
    if (test == 1): result0 += '1'
    result = result0
    result = '1' * (96 - len(result)) + result
  else:
    result = '0' * 96
  hex_result = ''
  for i in range(0, 96, 4):
    if   result[i: i + 4] == '0000': hex_result += '0'
    elif result[i: i + 4] == '0001': hex_result += '1'
    elif result[i: i + 4] == '0010': hex_result += '2'
    elif result[i: i + 4] == '0011': hex_result += '3'
    elif result[i: i + 4] == '0100': hex_result += '4'
    elif result[i: i + 4] == '0101': hex_result += '5'
    elif result[i: i + 4] == '0110': hex_result += '6'
    elif result[i: i + 4] == '0111': hex_result += '7'
    elif result[i: i + 4] == '1000': hex_result += '8'
    elif result[i: i + 4] == '1001': hex_result += '9'
    elif result[i: i + 4] == '1010': hex_result += 'a'
    elif result[i: i + 4] == '1011': hex_result += 'b'
    elif result[i: i + 4] == '1100': hex_result += 'c'
    elif result[i: i + 4] == '1101': hex_result += 'd'
    elif result[i: i + 4] == '1110': hex_result += 'e'
    elif result[i: i + 4] == '1111': hex_result += 'f'
  return hex_result
################
def pack_words(array):
  new_array = []
  new_int = 0
  for i in range(0, len(array)):
    if (i % 3 == 0):
      new_int += array[i]
    elif (i % 3 == 1):
      new_int += array[i] * pow(2,32)
    elif (i % 3 == 2):
      new_int += array[i] * pow(2,32) * pow(2, 32)
      new_array.append(new_int)
      new_int = 0
  return new_array

################
def generate_empty_memory(memory_filename):
  mem_file = open(memory_filename, 'w')
  for i in range(5760):
    mem_file.write('0' * 24 + '\n')
    
################
try:
  memory_filename = sys.argv[1]
  for ii in range(4):
    system("cp " + memory_filename + "_" + str(ii) + ".data" + " MEMCPY" + "_" + str(ii) + ".goldendata") 
except:
  for ii in range(4):
    generate_empty_memory("MEMCPY"+ "_" + str(ii) + ".data")

a_arr = (1, 2, 3)
b_arr = (-1, -2, -3, -4, -5, -6)
c_arr = (1, 2, 3, 4, 5, 6, 7, 8, 9)
d_arr = (-1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12)
line1 = 32
line2 = 128
line3 = 256
line4 = 1024

a_arr = pack_words(a_arr)
b_arr = pack_words(b_arr)
c_arr = pack_words(c_arr)
d_arr = pack_words(d_arr)

for ii in range(4):
  system("cp " + "MEMCPY" + "_" + str(ii) + ".data" + " MEMCPY" + "_" + str(ii) + ".goldendata") 

for ii in range(4):
  for a in a_arr: 
    addr = str(line4 / 2 + a_arr.index(a) + 1)      ## memory address starts with 0, line no. starts with 1
    new_memory = to_mem(a)
    system("sed -i \'" + addr + "s/.*/" + new_memory + "/\' " + "MEMCPY" + "_" + str(ii) + ".goldendata")
    
  for b in b_arr: 
    addr = str(line4 * 4 + line3 / 2 + b_arr.index(b) + 1) 
    new_memory = to_mem(b) 
    system("sed -i \'" + addr + "s/.*/" + new_memory + "/\' " + "MEMCPY" + "_" + str(ii) + ".goldendata")

  for c in c_arr: 
    addr = str(line4 * 4 + line3 * 4 + line2 / 2 + c_arr.index(c) + 1) 
    new_memory = to_mem(c) 
    system("sed -i \'" + addr + "s/.*/" + new_memory + "/\' " + "MEMCPY" + "_" + str(ii) + ".goldendata")

  for d in d_arr: 
    addr = str(line4 * 4 + line3 * 4 + line2 * 4 + line1 / 2 + d_arr.index(d) + 1) 
    new_memory = to_mem(d) 
    system("sed -i \'" + addr + "s/.*/" + new_memory + "/\' " + "MEMCPY" + "_" + str(ii) + ".goldendata")

  system("perl ./V2_MEM.pl " + "MEMCPY" + "_" + str(ii) + ".data")
  system("perl ./V2_MEM.pl " + "MEMCPY" + "_" + str(ii) + ".goldendata")
