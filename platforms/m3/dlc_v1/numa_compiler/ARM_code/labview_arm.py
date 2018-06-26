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


mem_0_file = open(directory + "SRAM.mem", "w+")

count = 0
file_count_0 = 0
file_count_1 = 0
file_count_2 = 0
file_count_3 = 0

hex_mem_0 = ''


with open(directory + '/' + directory.replace('/', '') + ".hex") as hex_file:

    for line in hex_file:
        line_split = line.split('\n')
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
            count = count + 1
            file_count_0 = file_count_0 + 1
        elif(count == 4):
            hex_mem_0 = str(line_split[0]) + hex_mem_0
            count = count + 1
        elif(count == 5):
            hex_mem_0 = str(line_split[0]) + hex_mem_0
            count = count + 1
        elif(count == 6):
            hex_mem_0 = str(line_split[0]) + hex_mem_0
            count = count + 1
        elif(count == 7):
            hex_mem_0 = str(line_split[0]) + hex_mem_0
            mem_0_file.write(hex_mem_0 + '\n')
            hex_mem_0 = '' 
            count = count + 1
            file_count_1 = file_count_1 + 1
        elif(count == 8):
            hex_mem_0 = str(line_split[0]) + hex_mem_0
            count = count + 1
        elif(count == 9):
            hex_mem_0 = str(line_split[0]) + hex_mem_0
            count = count + 1
        elif(count == 10):
            hex_mem_0 = str(line_split[0]) + hex_mem_0
            count = count + 1
        elif(count == 11):
            hex_mem_0 = str(line_split[0]) + hex_mem_0
            mem_0_file.write(hex_mem_0 + '\n')
            hex_mem_0 = ''
            count = count + 1
            file_count_2 = file_count_2 + 1
        elif(count == 12):
            hex_mem_0 = str(line_split[0]) + hex_mem_0
            count = count + 1
        elif(count == 13):
            hex_mem_0 = str(line_split[0]) + hex_mem_0
            count = count + 1
        elif(count == 14):
            hex_mem_0 = str(line_split[0]) + hex_mem_0
            count = count + 1
        elif(count == 15):
            hex_mem_0 = str(line_split[0]) + hex_mem_0
            mem_0_file.write(hex_mem_0 + '\n')
            hex_mem_0 = ''
            count = 0
            file_count_3 = file_count_3 + 1
        else:
            print("ERROR")

hex_file.close()
mem_0_file.close()


mem_0_file = open(directory + "SRAM_96.mem", "w+")

count = 0
file_count_0 = 0
file_count_1 = 0
file_count_2 = 0
file_count_3 = 0

hex_mem_0 = ''


with open(directory + "SRAM.mem") as hex_file:

    for line in hex_file:
        line_split = line.split('\n')
        if(count == 0):
            hex_mem_0 = hex_mem_0 + str(line_split[0])
            count = count + 1
        elif(count == 1):
            hex_mem_0 = hex_mem_0 + str(line_split[0])
            count = count + 1
        elif(count == 2):
            hex_mem_0 = hex_mem_0 + str(line_split[0])
            mem_0_file.write(hex_mem_0 + '\n')
            hex_mem_0 = ''
            count = 0
        else:
            print("ERROR")

hex_file.close()
mem_0_file.close()


