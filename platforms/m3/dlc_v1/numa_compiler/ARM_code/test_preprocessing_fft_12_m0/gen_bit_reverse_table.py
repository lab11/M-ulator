#!/usr/bin/env python

def reverse(x, n):
    result = 0
    for i in range(n):
        if (x >> i) & 1: result |= 1 << (n - 1 - i)
    return result

bits = 7

for i in range(pow(2, bits)):
#  print i, '\t', 
  print hex(reverse(i, 7)), ',',
print
