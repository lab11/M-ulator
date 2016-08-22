#!/usr/bin/env python

import os
import sys
#from matplotlib import pyplot
from numpy import arange
import bisect
from decimal import *
import numpy
import csv

mem_0_file = open("SRAM_0.mem", "w+")
mem_1_file = open("SRAM_1.mem", "w+")
mem_2_file = open("SRAM_2.mem", "w+")
mem_3_file = open("SRAM_3.mem", "w+")

count = 0
file_count_0 = 0
file_count_1 = 0
file_count_2 = 0
file_count_3 = 0

hex_mem_0 = ''
hex_mem_1 = ''
hex_mem_2 = ''
hex_mem_3 = ''

with open("test_memcpy.hex") as hex_file:

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
            hex_mem_1 = str(line_split[0]) + hex_mem_1
            count = count + 1
        elif(count == 5):
            hex_mem_1 = str(line_split[0]) + hex_mem_1
            count = count + 1
        elif(count == 6):
            hex_mem_1 = str(line_split[0]) + hex_mem_1
            count = count + 1
        elif(count == 7):
            hex_mem_1 = str(line_split[0]) + hex_mem_1
            mem_1_file.write(hex_mem_1 + '\n')
            hex_mem_1 = '' 
            count = count + 1
            file_count_1 = file_count_1 + 1
        elif(count == 8):
            hex_mem_2 = str(line_split[0]) + hex_mem_2
            count = count + 1
        elif(count == 9):
            hex_mem_2 = str(line_split[0]) + hex_mem_2
            count = count + 1
        elif(count == 10):
            hex_mem_2 = str(line_split[0]) + hex_mem_2
            count = count + 1
        elif(count == 11):
            hex_mem_2 = str(line_split[0]) + hex_mem_2
            mem_2_file.write(hex_mem_2 + '\n')
            hex_mem_2 = ''
            count = count + 1
            file_count_2 = file_count_2 + 1
        elif(count == 12):
            hex_mem_3 = str(line_split[0]) + hex_mem_3
            count = count + 1
        elif(count == 13):
            hex_mem_3 = str(line_split[0]) + hex_mem_3
            count = count + 1
        elif(count == 14):
            hex_mem_3 = str(line_split[0]) + hex_mem_3
            count = count + 1
        elif(count == 15):
            hex_mem_3 = str(line_split[0]) + hex_mem_3
            mem_3_file.write(hex_mem_3 + '\n')
            hex_mem_3 = ''
            count = 0
            file_count_3 = file_count_3 + 1
        else:
            print("ERROR")

for i in range (0, 2048 - file_count_0):
    mem_0_file.write("xxxxxxxx\n")
    #mem_0_file.write("00000000\n")

for i in range (0, 2048 - file_count_1):
    mem_1_file.write("xxxxxxxx\n")
    #mem_1_file.write("00000000\n")

for i in range (0, 2048 - file_count_2):
    mem_2_file.write("xxxxxxxx\n")
    #mem_2_file.write("00000000\n")

for i in range (0, 2048 - file_count_3):
    mem_3_file.write("xxxxxxxx\n")
    #mem_3_file.write("00000000\n")

hex_file.close()
mem_0_file.close()
mem_1_file.close()
mem_2_file.close()
mem_3_file.close()



