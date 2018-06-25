#!/usr/bin/env python
from os import system

num_inputs = 512

system("echo " + str(num_inputs) + " > dnn.input")
string = ""
for i in range(num_inputs):
  string += str((i%10)*0.005) + ' ' 
system("echo " + string + " >> dnn.input")
