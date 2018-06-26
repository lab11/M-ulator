#!/usr/bin/env python

import numpy

fft_size = 512
fft_size = 16

A = []
for i in range(fft_size):
#  A.append((i % 7) * 12)
  A.append(i)

B = numpy.fft.fft(A)
numpy.set_printoptions(precision = 2)
print A
for it in range(len(B.real)):
  print "%.2f %.2f" % (B.real[it], B.imag[it]), 
print
#print "real:", B.real 
#print "imag:", B.imag 
#print "abs:",  numpy.absolute(B) 

