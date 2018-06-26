#!/usr/bin/env python

import os
import sys
#from matplotlib import pyplot
from numpy import arange
import bisect
from decimal import *
import numpy
import csv


golden_mem_file = open("test_read_dnn_sram/memory_read_result/SRAM_4K.golden_mem", "w+")
count = 512
mem_pattern1 = 'FFFFFFFF'
mem_pattern2 = '00000000'

for i in range (1, 512):
	golden_mem_file.write(mem_pattern1 + '\n')
	golden_mem_file.write(mem_pattern2 + '\n')
	
golden_mem_file.close()

