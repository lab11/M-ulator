#!/usr/bin/env python

import os
import sys
#from matplotlib import pyplot
from numpy import arange
import bisect
from decimal import *
import numpy
import csv

try:
    directory = sys.argv[1] + '/'
    if not os.path.isfile(directory + '/' + directory.replace('/', '') + ".hex"):
        print "binary does not exist!!"
        sys.exit()
except:
    print "please provide binary directory"
    sys.exit()


mem_0_file = open(directory + "SRAM_4K_0.mem", "w+")
mem_1_file = open(directory + "SRAM_4K_1.mem", "w+")
mem_2_file = open(directory + "SRAM_4K_2.mem", "w+")
mem_3_file = open(directory + "SRAM_4K_3.mem", "w+")
mem_4_file = open(directory + "SRAM_4K_4.mem", "w+")

count = 0
file_count_0 = 0
file_count_1 = 0
file_count_2 = 0
file_count_3 = 0
file_count_4 = 0

hex_mem_0 = ''
hex_mem_1 = ''
hex_mem_2 = ''
hex_mem_3 = ''

with open(directory + '/' + directory.replace('/', '') + ".hex") as hex_file:

    for line in hex_file:
        line_split = line.split('\n')
        if(file_count_0 < 1024):
            if(count == 0):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 1):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 2):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 3):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                mem_0_file.write(hex_mem_0 + '\n')
                hex_mem_0 = ''
                count = 0
                file_count_0 = file_count_0 + 1
            else:
                print("ERROR")
        elif(file_count_1 < 1024):
            if(count == 0):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 1):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 2):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 3):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                mem_1_file.write(hex_mem_0 + '\n')
                hex_mem_0 = ''
                count = 0
                file_count_1 = file_count_1 + 1
            else:
                print("ERROR")
        elif(file_count_2 < 1024):
            if(count == 0):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 1):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 2):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 3):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                mem_2_file.write(hex_mem_0 + '\n')
                hex_mem_0 = ''
                count = 0
                file_count_2 = file_count_2 + 1
            else:
                print("ERROR")
        elif(file_count_3 < 1024):
            if(count == 0):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 1):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 2):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 3):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                mem_3_file.write(hex_mem_0 + '\n')
                hex_mem_0 = ''
                count = 0
                file_count_3 = file_count_3 + 1
            else:
                print("ERROR")
        elif(file_count_4 < 1024):
            if(count == 0):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 1):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 2):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                count = count + 1
            elif(count == 3):
                hex_mem_0 = str(line_split[0]) + hex_mem_0
                mem_4_file.write(hex_mem_0 + '\n')
                hex_mem_0 = ''
                count = 0
                file_count_4 = file_count_4 + 1
            else:
                print("ERROR")


for i in range (0, 1024 - file_count_0):
   # mem_0_file.write("xxxxxxxx\n")
   mem_0_file.write("00000000\n")

#for i in range (0, 1024 - file_count_1):
#    mem_1_file.write("xxxxxxxx\n")
#    #mem_1_file.write("00000000\n")
#
#for i in range (0, 1024 - file_count_2):
#    mem_2_file.write("xxxxxxxx\n")
#    #mem_2_file.write("00000000\n")
#
#for i in range (0, 1024 - file_count_3):
#    mem_3_file.write("xxxxxxxx\n")
#    #mem_3_file.write("00000000\n")
#
#for i in range (0, 1024 - file_count_4):
#    mem_4_file.write("xxxxxxxx\n")
#    #mem_3_file.write("00000000\n")

hex_file.close()
mem_0_file.close()
mem_1_file.close()
mem_2_file.close()
mem_3_file.close()

