#!/usr/bin/env python

input_file = open("dnn_0.outputs", 'r')
input_file.readline()
inputs = []
for input in input_file.readline().split(' '):
  try:
    inputs.append(float(input))
  except:
    break

sum = 0
for input in inputs:
  sum += input * 32

print sum / pow(2, 12)
