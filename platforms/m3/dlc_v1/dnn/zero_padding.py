#!/usr/bin/env python

import sys
import os

try:
    in_filename = sys.argv[1]
except:
    print "please provide filename"
    exit()
try:
    file_type = sys.argv[2]
except:
    file_type = 'w'
try:
    new_input_size = int(sys.argv[3])
except:
    new_input_size = 408 
try:
    new_output_size = int(sys.argv[4])
except:
    new_output_size = 384 

print "input_size:", new_input_size, "\toutput_size:", new_output_size
in_file  = open(in_filename, 'r') 
out_file  = open(in_filename + ".new", 'w') 
sizes = in_file.readline().split(' ')
if file_type == 'w':
    old_input_size = int(sizes[0])
    old_output_size = int(sizes[1]) 
    out_file.write(str(new_input_size) + ' ')
    out_file.write(str(new_output_size) + ' ')
    for item in sizes[2:]:
        out_file.write(item + ' ')
#    out_file.write('\n')
    if (old_input_size != new_input_size):
        while True:
            data = in_file.readline().replace('\n', '')
            if not data:
                break
            out_file.write(data)
            out_file.write('0 ' * (new_input_size - old_input_size))
            out_file.write('\n')
    else:
       while True:
          data = in_file.readline().replace('\n', '')
          if not data:
              break
          out_file.write(data)
          out_file.write('\n')
    if (old_output_size != new_output_size):
        for i in range(new_output_size - old_output_size):
          out_file.write('0 ' * new_input_size + '\n')
    in_file.close()
    out_file.close()
    os.system("mv " + in_filename + '.new ' + in_filename)
    
elif file_type == 'i' or file_type == 'o':
    old_input_size = int(sizes[0])
    if (old_input_size != new_input_size):
        out_file.write(str(new_input_size) + ' ')
        for item in sizes[1:]:
            out_file.write(item + ' ')
        data = in_file.readline().replace('\n', '')
        out_file.write(data)
        out_file.write('0 ' * (new_input_size - old_input_size))
        out_file.write('\n')
        in_file.close()
        out_file.close()
        os.system("mv " + in_filename + '.new ' + in_filename)
    else:
        in_file.close()
        out_file.close()
        os.system("rm " + in_filename + '.new ')
