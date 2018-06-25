#!/usr/bin/env python
from os import system

layers = 3
neurons = [403, 400, 400, 13]

system("echo " + str(layers) + " > dnn.weights")
for i in range(layers):
  system("echo " + str(neurons[i]) + ' ' + str(neurons[i+1]) + " >> dnn.weights")
  for k in range(neurons[i+1]):
    string = ""
    for j in range(neurons[i]):
      string += str((k + 1) * 0.008) + ' '
    system("echo " + string + " >> dnn.weights")
  string = ""
  for k in range(neurons[i+1]):
    string += str((i + 1) * 0.008) + ' '
  system("echo " + string + " >> dnn.weights")

