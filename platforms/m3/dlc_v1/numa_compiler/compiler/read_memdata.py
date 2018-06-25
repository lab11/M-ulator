#!/usr/bin/env python
import sys

word_length = 24
try:
    filename = sys.argv[1]
except:
    print "please provide filename"
    sys.exit()
try:
    precision = int(sys.argv[2])
except:
    precision = 0
try:
    start_line = int(sys.argv[3])
except:
    start_line = 1
try:
    end_line = int(sys.argv[4])
except:
    end_line = 5761

def read_line(line):
  print line
  for i in range(word_length, 0, -precision / 4):
    num = line[i - (precision / 4) : i]
    print num,  
    num = int(num, 16)
    if num > pow(2, precision - 1):
      num -= pow(2, precision)
    print num 
#    print i

memfile = open(filename, 'r')
line_no = 1
for line in memfile:
    if line_no < start_line or line_no > end_line:
        continue
    print line_no
    read_line(line)
    line_no += 1
    
