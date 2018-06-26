#!/usr/bin/env python
# sample usage:
#	python labview_INIT_MEM.py compiler/MEMCPY_INIT_MEM .data

import os
import sys
#from matplotlib import pyplot
from numpy import arange
import bisect
from decimal import *
import numpy
import csv

directory = sys.argv[1]
surfix = sys.argv[2]

mem_L4B1_file = open(directory + "L4B1" + surfix, "w+")