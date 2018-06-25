#!/usr/bin/env python
# sample usage:
#	python labview_gen_mem.py test_read_dnn_sram/golden_memory/MEMCPY_0 .goldendata

import os
import sys
#from matplotlib import pyplot
from numpy import arange
import bisect
from decimal import *
import numpy
import csv

# try:
    # directory = sys.argv[1] + '/'
    # if not os.path.isfile(directory + '/' + directory.replace('/', '') + ".hex"):
        # print "binary does not exist!!"
        # sys.exit()
# except:
    # print "please provide binary directory"
    # sys.exit()

directory = sys.argv[1]
surfix = sys.argv[2]

mem_L4B1_file = open(directory + "L4B1" + surfix, "w+")
mem_L4B2_file = open(directory + "L4B2" + surfix, "w+")
mem_L4B3_file = open(directory + "L4B3" + surfix, "w+")
mem_L4B4_file = open(directory + "L4B4" + surfix, "w+")
mem_L3_file   = open(directory + "L3"   + surfix, "w+")
mem_L1L2_file = open(directory + "L1L2" + surfix, "w+")

count = 0

hex_mem_0 = ''


with open(directory + surfix) as hex_file:

    for line in hex_file:
        line_split = line.split('\n')
        if(count < 1023):
            hex_mem_0 = hex_mem_0 + str(line_split[0]) + '\n'
            count = count + 1
        elif(count == 1023):
            hex_mem_0 = hex_mem_0 + str(line_split[0])
            mem_L4B1_file.write(hex_mem_0)
            hex_mem_0 = ''
            count = count + 1
        elif(count < 2047):
            hex_mem_0 = hex_mem_0 + str(line_split[0]) + '\n'
            count = count + 1
        elif(count == 2047):
            hex_mem_0 = hex_mem_0 + str(line_split[0])
            mem_L4B2_file.write(hex_mem_0)
            hex_mem_0 = ''
            count = count + 1
        elif(count < 3071):
            hex_mem_0 = hex_mem_0 + str(line_split[0]) + '\n'
            count = count + 1
        elif(count == 3071):
            hex_mem_0 = hex_mem_0 + str(line_split[0])
            mem_L4B3_file.write(hex_mem_0)
            hex_mem_0 = ''
            count = count + 1
        elif(count < 4095):
            hex_mem_0 = hex_mem_0 + str(line_split[0]) + '\n'
            count = count + 1
        elif(count == 4095):
            hex_mem_0 = hex_mem_0 + str(line_split[0])
            mem_L4B4_file.write(hex_mem_0)
            hex_mem_0 = ''
            count = count + 1
        elif(count < 5119):
            hex_mem_0 = hex_mem_0 + str(line_split[0]) + '\n'
            count = count + 1
        elif(count == 5119):
            hex_mem_0 = hex_mem_0 + str(line_split[0])
            mem_L3_file.write(hex_mem_0)
            hex_mem_0 = ''
            count = count + 1
        elif(count < 5759):
            hex_mem_0 = hex_mem_0 + str(line_split[0]) + '\n'
            count = count + 1
        elif(count == 5759):
            hex_mem_0 = hex_mem_0 + str(line_split[0])
            mem_L1L2_file.write(hex_mem_0)
            hex_mem_0 = ''
            count = count + 1
        else:
            print("ERROR")

hex_file.close()
mem_L4B1_file.close()
mem_L4B2_file.close()
mem_L4B3_file.close()
mem_L4B4_file.close()
mem_L3_file.close()
mem_L1L2_file.close()


