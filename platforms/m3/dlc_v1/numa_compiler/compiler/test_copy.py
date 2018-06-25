#!/usr/bin/env python
import os

for file in os.listdir('.'):
    if "DNN_INIT_MEM_" in file:
#        print file[3:]
        os.system("cp " + file + " FFT" + file[3:])

os.chdir("memory")
for file in os.listdir('.'):
    if "DNN_INIT_MEM_" in file:
#        print file[3:]
        os.system("cp " + file + " FFT" + file[3:])
       
