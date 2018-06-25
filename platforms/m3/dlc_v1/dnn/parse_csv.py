#!/usr/bin/env python

import sys
import csv
import numpy as np
import scipy.stats as stats
import matplotlib.pyplot as plt

#################
def print_readme():
  print "example:"
  print "for weight matrix with input size of 403, output size of 384, 6 bits with 2 bit integer, use \"./parse_csv.py [weight_file] w 403 384 2 6\"" 
  print "for offset with size of 384, 8 bits with 4 bit integer, use \"./parse_csv.py [offset_file] o 384 1 4 8\"" 
  print "for input with size of 403, 12 bits with 4 bit integer, 12 bits for output and 32 bits for temp output, use \"./parse_csv.py [input_file] i 403 1\". note that the precision for outptu and temp output is required"
#################

### 
def average(array):
  if (len(array) == 0):
    return 0
  else:
    return sum(array) / len(array)
### 

################################
try:
  weight_file = sys.argv[1]
except:
  print_readme()
  exit()
try:
  file_type = sys.argv[2]
except:
  file_type = 'w'
try:
  input_size = int(sys.argv[3])
except:
  input_size = 403
try:
  output_size = int(sys.argv[4])
except:
  if file_type == 'w':
      output_size = 384
if file_type != 'w':
  output_size = 1
try:
  int_bits = int(sys.argv[5])
except:
  if file_type == 'w':
      int_bits = 2
  else:
      int_bits = 4
try:
  total_bits = int(sys.argv[6])
except:
  if file_type == 'w':
      total_bits = 6
  else:
      total_bits = 12
try:
  out_bits = int(sys.argv[7])
except:
  out_bits = 12
try:
  tempout_bits = int(sys.argv[8])
except:
  tempout_bits = 32
float_bits = total_bits - int_bits - 1    ## sign takes one bit 
################################
print "input_size:", input_size, "output_size:", output_size
weights = []
weight_loc = [ [[] for x in range(output_size)] for y in range(input_size)]

with open(weight_file, 'r') as csvfile:
  reader = csv.reader(csvfile, delimiter=',') 
  row = 0   ## input
  for line in reader: ## one row
    if "dimension" in line[0]:
      input_size = int((line[0].split('\t'))[-3])
      output_size = int((line[0].split('\t'))[-2])
      weight_loc = [ [[] for x in range(output_size)] for y in range(input_size)]
      continue
    column = 0    ## output 
    for number in line:
      if len(number.split('\t')) > 1:
        for item in number.split('\t'):
          try: 
            weights.append(float(item))
            weight_loc[row][column].append(float(item))
            column += 1
          except:
            pass
      else:
          weights.append(float(number))
          weight_loc[row][column].append(float(number))
          column += 1
    row += 1
    if (row == input_size):
        row = 0

if file_type == 'w':
  out_weight_filename = weight_file + ".weight"
elif file_type == 'o':
  out_weight_filename = weight_file + ".offsets"
elif file_type == 'i':
  out_weight_filename = weight_file + ".inputs"
out_weight = open(out_weight_filename, 'w')

if file_type == 'w':
   out_weight.write(str(input_size) + ' ' + str(output_size) + ' ' + str(total_bits) + '\n')
elif file_type == 'o':
   out_weight.write(str(input_size) + '\n')
elif file_type == 'i':
   out_weight.write(str(input_size) + ' ' + str(total_bits) + ' ' + str(out_bits) + ' ' + str(tempout_bits) + '\n')

print "%i" % (float_bits)
print "%i" % (2 ** float_bits)
# transpose 
for column in range(output_size):
    for row in range(input_size):
        if (len(weight_loc[row][column]) != 1):
            print "error: more than 1 weight"
        weight = weight_loc[row][column][0]
#        print "%.2f\t" % (weight), 
        if weight >= (2 ** int_bits):
            weight = 2 ** (total_bits - 1) - 1    ## (total_bits - 1): because sign takes one bit as well
        elif weight <= (-2 ** int_bits):
            weight = - 2 ** (total_bits  - 1) + 1
        else:
            weight = int(round(weight * (2 ** float_bits)))
#        print "%i" % (weight)
        out_weight.write(str(weight) + ' ')
    out_weight.write('\n')
out_weight.close()

print "count", len(weights)
print "max", max(weights)
print "min", min(weights)
print "average", average(weights)
weights.sort()
print "std", np.std(weights)
exit()

####################################################
### NOT USED
a = [0, 0, 0, 0, 0]   #0.1, 0.5, 1, 2.5, 5

for item in weights:
  if abs(item) < 0.1:
    a[0] += 1;    
  elif abs(item) < 0.5:
    a[1] += 1;    
  elif abs(item) < 1:
    a[2] += 1;    
  elif abs(item) < 2.5:
    a[3] += 1;    
  elif abs(item) < 5:
    a[4] += 1; 

## weight spatial distribution
print "spatial distribution"
color = []
threshold = (max(weights) - min(weights)) / 10
for x in range(input_size):
  for y in range(output_size):
     color_scale = abs(average(weight_loc[x][y])) - average(weights) / threshold
     color.append(color_scale)
x = []
for i in range(input_size): 
  for j in range(output_size):
    x.append(i)
y = range(output_size) * input_size
plt.scatter(x, y, c=color, s=100, cmap='gray', lw=0)
plt.show()
####################################################
